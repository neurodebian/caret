/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <iostream>

#include <QMessageBox>
#include <QTimer>

#include "CommunicatorClientBase.h"
#include "DebugControl.h"
#include "GuiCommunicatorClientConnectDialog.h"
#include "GuiMainWindow.h"
#include "global_variables.h"

/**
 * Constructor.
 *    programName - name of programm connecting to (eg. AFNI).
 *    defaultHostName - the name of the host to connect.
 *    defaultControlPort - the control port of the program connecting to.
 *    defaultDataPort - the data port of the program connecting to.
 *    defaultRequestUpdates - request other program send updates back.
 *    allowUserToEditDataPort - allow the user to change the data port.
 *    allowUserToEditRequestUpdates - allow user to choose to receive updates.
 */
CommunicatorClientBase::CommunicatorClientBase(const QString& programNameIn,
                                   const QString& defaultHostName,
                                   const int defaultControlPort,
                                   const int defaultDataPort,
                                   const bool defaultRequestUpdates,
                                   const bool allowUserToEditDataPort,
                                   const bool allowUserToEditRequestUpdates)
{
   programName = programNameIn;
   hostName = defaultHostName;
   controlPortNumber = defaultControlPort;
   dataPortNumber    = defaultDataPort;
   requestUpdates    = defaultRequestUpdates;
   displayDataPortInConnectDialog = allowUserToEditDataPort,
   displayRequestUpdatesInConnectDialog = allowUserToEditRequestUpdates;
   
   connectionStatus = CONNECTION_CLOSED;
   
   //
   // Create a socket 
   //
   socket = new QTcpSocket(this);
   
   //
   // Setup some of the sockets signals
   //
   QObject::connect(socket, SIGNAL(connected()),
                    this, SLOT(socketConnectedSlot()));
   QObject::connect(socket, SIGNAL(disconnected()),
                    this, SLOT(socketConnectionClosedSlot()));
   QObject::connect(socket, SIGNAL(readyRead()),
                    this, SLOT(socketReadyForReadingSlot()));
   QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(socketErrorSlot(QAbstractSocket::SocketError)));
                    
   //
   // Setup signals available to deriving class
   //
   QObject::connect(socket, SIGNAL(connected()),
                    this, SIGNAL(socketConnectionEstablished()));
   
}

/**
 * Destructor.
 */
CommunicatorClientBase::~CommunicatorClientBase()
{
}

/**
 * open a connection
 */
void
CommunicatorClientBase::openConnection()
{
   //
   // Allow user to modify communication parameters
   //
   GuiCommunicatorClientConnectDialog* cccd =
      new GuiCommunicatorClientConnectDialog(theMainWindow,
                                             programName,
                                             hostName,
                                             controlPortNumber,
                                             dataPortNumber,
                                             requestUpdates,
                                             displayDataPortInConnectDialog,
                                             displayRequestUpdatesInConnectDialog);
   
   //
   // Did user press OK button ?
   //
   if (cccd->exec() == QDialog::Accepted) {
      //
      // Get the parameters from the dialog
      //
      hostName = cccd->getHostName();
      controlPortNumber = cccd->getControlPort();
      if (displayDataPortInConnectDialog) {
         dataPortNumber = cccd->getDataPort();
      }
      if (displayRequestUpdatesInConnectDialog) {
         requestUpdates = cccd->getRequestUpdates();
      }
      
      //
      // Connection Status
      //
      connectionStatus = CONNECTION_OPENING_CONTROL_PORT;
      
      //
      // Try connecting to the host
      //
      if (DebugControl::getDebugOn()) std::cout << "Trying to connect to " << hostName.toAscii().constData() << " on port " << controlPortNumber << std::endl;
      socket->connectToHost(hostName, controlPortNumber);
   }
}

/**
 * Print the status of the socket.
 */
void
CommunicatorClientBase::printSocketStatus()
{
   if (DebugControl::getDebugOn() == false) {
      return;
   }
   
   std::cout << "Checking connection, status: ";
   switch(socket->state()) {
      case QAbstractSocket::UnconnectedState:
         std::cout << " idle" << std::endl;
         break;
      case QAbstractSocket::HostLookupState:
         std::cout << " host lookup" << std::endl;
         break;
      case QAbstractSocket::ConnectingState:
         std::cout << " connecting" << std::endl;
         break;
      case QAbstractSocket::ConnectedState:
         std::cout << " connected" << std::endl;
         break;
      case QAbstractSocket::BoundState:
         std::cout << " bound" << std::endl;
         break;
      case QAbstractSocket::ClosingState:
         std::cout << " closing" << std::endl;
         break;
      case QAbstractSocket::ListeningState:
         std::cout << " listening" << std::endl;
         break;
      default:
         std::cout << " unknown" << std::endl;
         break;
   }
}

/**
 * Close a connection.
 */
void
CommunicatorClientBase::closeConnection()
{
   if (socket != NULL) {
      socket->close();
   }
   connectionStatus = CONNECTION_CLOSED;
}

/**
 * Called when the socket achieves a connection.
 */
void
CommunicatorClientBase::socketConnectedSlot()
{
   if (DebugControl::getDebugOn()) std::cout << "Socket has connected" << std::endl;
   printSocketStatus();
   
   //
   // Connection Status
   //
   if (connectionStatus == CONNECTION_OPENING_CONTROL_PORT) {
      //
      // send control information, flush, and close control channel.
      //
      connectionStatus = CONNECTION_SENDING_CONTROL_INFORMATION;
      sendMessage(controlMessage);
      socket->flush();
      socket->close();
      
      //
      // Allow server time to set up data channel and then connect with data channel.
      //
      QTimer::singleShot(3000, this, SLOT(openDataChannel()));
   }
   else if (connectionStatus == CONNECTION_OPENING_DATA_PORT) {
      connectionStatus = CONNECTION_DATA_PORT_OPEN;
      if (DebugControl::getDebugOn()) std::cout << "Data channel now open" << std::endl;
   }
}

/**
 * Called to open data channel.
 */
void
CommunicatorClientBase::openDataChannel()
{
   if (connectionStatus == CONNECTION_SENDING_CONTROL_INFORMATION) {
      //
      // Reopen socket on the data port
      //
      if (DebugControl::getDebugOn()) std::cout << "Trying to connect to " << hostName.toAscii().constData() << " on port " << dataPortNumber << std::endl;
      socket->connectToHost(hostName, dataPortNumber);
      connectionStatus = CONNECTION_OPENING_DATA_PORT;
   }
}

/**
 * Called when the socket is closed.
 */
void
CommunicatorClientBase::socketConnectionClosedSlot()
{
   if (DebugControl::getDebugOn()) std::cout << "Socket has closed connection" << std::endl;
   //
   // After control information is received, server closes connection.
   //
   if (connectionStatus == CONNECTION_SENDING_CONTROL_INFORMATION) {
      //
      // Reopen socket on the data port
      //
      //if (DebugControl::getDebugOn()) std::cout << "Trying to connect to " << hostName << " on port " << dataPortNumber << std::endl;
      //socket->connectToHost(hostName, dataPortNumber);
      //connectionStatus = CONNECTION_OPENING_DATA_PORT;
   }
   else {
      connectionStatus = CONNECTION_CLOSED;
   }
}

/**
 * Called when the socket has data for reading.
 */
void
CommunicatorClientBase::socketReadyForReadingSlot()
{
   const int BUF_SIZE = 4096;
   char data[BUF_SIZE+1];
   
   if (DebugControl::getDebugOn()) std::cout << "Socket has data for reading" << std::endl;
   while (socket->bytesAvailable() > 0) {
      const int numBytes = socket->bytesAvailable() ;
      if (DebugControl::getDebugOn()) std::cout << "Bytes for reading: " << numBytes << std::endl;
      socket->read(data, BUF_SIZE);
      data[numBytes] = '\0';
      if (DebugControl::getDebugOn()) std::cout << "   Read data: " << data << std::endl;
      
      processReceivedMessage(data);
   }
}

/**
 * Called when the socket has an error.
 */
void
CommunicatorClientBase::socketErrorSlot(QAbstractSocket::SocketError errorCode)
{
   QString msg("Communication error:");
   
   switch(errorCode) {
      case QTcpSocket::ConnectionRefusedError:
         msg.append(" connection refused");
         break;
      case QTcpSocket::RemoteHostClosedError:
         msg.append(" remote host closed error");
         break;
      case QTcpSocket::HostNotFoundError:
         msg.append(" host not found");
         break;
      case QTcpSocket::SocketAccessError:
         msg.append(" socket access error");
         break;
      case QTcpSocket::SocketResourceError:
         msg.append(" socket resource error");
         break;
      case QTcpSocket::SocketTimeoutError:
         msg.append(" socket timeout error");
         break;
      case QTcpSocket::DatagramTooLargeError:
         msg.append(" datagram too large error");
         break;
      case QTcpSocket::NetworkError:
         msg.append(" network error");
         break;
      case QTcpSocket::AddressInUseError:
         msg.append(" address in use error");
         break;
      case QTcpSocket::SocketAddressNotAvailableError:
         msg.append(" socket address not available error");
         break;
      case QTcpSocket::UnsupportedSocketOperationError:
         msg.append(" unsupported socket operation error");
         break;
      case QTcpSocket::UnknownSocketError:
         msg.append(" unknown socket error");
         break;
      case QTcpSocket::SslHandshakeFailedError:
         msg.append(" SSL Handshake Failed error");
         break;
      case QTcpSocket::ProxyAuthenticationRequiredError:
         msg.append(" proxy authentication required error");
         break;
      case QTcpSocket::UnfinishedSocketOperationError:
         msg.append(" unfinished socket operation error");
         break;
   }
   if (DebugControl::getDebugOn()) std::cout << msg.toAscii().constData() << std::endl;
   
   QMessageBox::critical(theMainWindow, "Communication Error", msg);    
}

/**
 * Send a message over the socket
 */
void
CommunicatorClientBase::sendMessage(const QString& message)
{
   //std::cout << std::endl << "Sending message to server: " << message << std::endl << std::endl;
   // Note: Include zero at end of string when setting length
   socket->write(message.toAscii().constData(), (message.length() + 1));
}


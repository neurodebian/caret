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


#ifndef __VE_COMMUNICATOR_CLIENT_BASE_H__
#define __VE_COMMUNICATOR_CLIENT_BASE_H__

#include <QString>

#include <QObject>
#include <QTcpSocket>

/// This base class is used for communicating with other programs
/// via sockets.  You may not instatiate this class, you must derive
/// from it.
class CommunicatorClientBase : public QObject {
   Q_OBJECT
   
   public:
      /// get the host name
      QString getHostName() const { return hostName; }
      
      /// set the host name
      void setHostName(const QString& name) { hostName = name; }
      
      /// get the control port number
      int getControlPortNumber() const { return controlPortNumber; }
      
      /// set the control port number
      void setControlPortNumber(const int portNumber) { controlPortNumber = portNumber; }
   
      /// get the data port number
      int getDataPortNumber() const { return dataPortNumber; }
      
      /// set the control port number
      void setDataPortNumber(const int portNumber) { dataPortNumber = portNumber; }
   
      /// get the request updates flag
      bool getRequestUpdates() const { return requestUpdates; }
      
      /// set the request updates flag
      void setRequestUpdates(const bool getUpdates) { requestUpdates = getUpdates; }
      
      /// get the control message
      QString getControlMessage() const { return controlMessage; }
      
      /// set the control message
      void setControlMessage(const QString& msg) { controlMessage = msg; }
      
      /// destructor
      virtual ~CommunicatorClientBase();
      
      /// print the status of the socket
      void printSocketStatus();
         
   public slots:
      /// open a connection 
      void openConnection();
      
      /// close a connection
      void closeConnection();
      
   signals:
      /// signal emitted when the connection is successfully made
      void socketConnectionEstablished();
      
   private slots:
      /// open the data channel
      void openDataChannel();
      
      /// called when the socket successfully connects
      void socketConnectedSlot();
      
      /// called when the socket connection is closed
      void socketConnectionClosedSlot();
      
      /// called when the socket has data to be read
      void socketReadyForReadingSlot();
      
      /// called when the socket has an error
      void socketErrorSlot(QAbstractSocket::SocketError error);
         
   protected:
      enum CONNECTION_STATUS {
         CONNECTION_OPENING_CONTROL_PORT,
         CONNECTION_SENDING_CONTROL_INFORMATION,
         CONNECTION_OPENING_DATA_PORT,
         CONNECTION_DATA_PORT_OPEN,
         CONNECTION_CLOSED
      };
      
      /// Constructor
      CommunicatorClientBase(const QString& programNameIn,
                       const QString& defaultHostName,
                       const int defaultControlPort,
                       const int defaultDataPort,
                       const bool defaultRequestUpdates,
                       const bool allowUserToEditDataPort,
                       const bool allowUserToEditRequestUpdates);
       
      /// Process a received message.  This method must be implemented
      /// by the deriving class.
      virtual void processReceivedMessage(const QString& msg) = 0;
      
      /// send a message over the socket
      void sendMessage(const QString& message);
      
      /// the connection status
      CONNECTION_STATUS connectionStatus;
      
      /// name of program connecting to
      QString programName;
      
      /// the host name
      QString hostName;
      
      /// the control port number
      int controlPortNumber;
      
      /// the data port number
      int dataPortNumber;
      
      /// request updates flag
      bool requestUpdates;
      
      /// socket used for communication
      QTcpSocket* socket;
      
      /// control message that is sent when establishing a connection
      QString controlMessage;
      
   private:
      
      /// display the data port for user editing in the connection dialog
      bool displayDataPortInConnectDialog;
      
      /// display the request updates checkbox for user editing in the connection dialog
      bool displayRequestUpdatesInConnectDialog;      
      
      
};

#endif


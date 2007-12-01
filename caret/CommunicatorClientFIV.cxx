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
#include <sstream>
#include <vector>

#include <QMessageBox>

#include "CommunicatorClientFIV.h"
#include "DebugControl.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiIdentifyDialog.h"
#include "GuiMainWindow.h"
#include "StringUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
CommunicatorClientFIV::CommunicatorClientFIV() : CommunicatorClientBase("FIV",
                                                        "localhost",
                                                        6440,   // control port 
                                                        6445,   // data port
                                                        true,   // request updates
                                                        true,   // allow data port editing
                                                        true)   // allow request updates editing
{
   //
   // Create the control message and assign it to the base communicator
   //
   createControlMessage();
}

/**
 * Destructor.
 */
CommunicatorClientFIV::~CommunicatorClientFIV()
{
}

/**
 * Create the first message that is to be sent to the program that we are connecting to
 */
void
CommunicatorClientFIV::createControlMessage()
{
   //
   // Create string to send to FIV to give data port and other info.
   // Multiple strings are separated with '\n'.
   //
   QString msg;
   
   //
   // IOCHAN means use this for the data channel
   //
   char str[256];
   sprintf(str, "TAL_PORT tcp:%s:%d", hostName.toAscii().constData(), dataPortNumber);
   msg.append(str);
   msg.append("\n");
      
   setControlMessage(msg);
}

/**
 * Receive the messages from the FIV program
 */
void
CommunicatorClientFIV::processReceivedMessage(const QString& msg)
{
   //
   // Incoming data may have more than one message with the messages
   // separated by a newline.
   //
   std::vector<QString> messages;
   StringUtilities::token(msg, "\n", messages);
   
   for (unsigned int i = 0; i < messages.size(); i++) {
      if (DebugControl::getDebugOn()) std::cout << "FIV Message: " << messages[i].toAscii().constData() << std::endl;
      
      std::vector<QString> tokens;
      StringUtilities::token(messages[i], " \t\n", tokens);
      
      if (tokens.size() > 0) {
         if (tokens[0] == "TAL_XYZ") {
            if (tokens.size() > 3) {
               processVoxelHighlightFromFIV(tokens[1].toInt(), tokens[2].toInt(), 
                                             tokens[3].toInt());
            }
         }
      }
   }
}

/**
 * Process a voxel highlight request from FIV.
 */
void
CommunicatorClientFIV::processVoxelHighlightFromFIV(const int i, const int j, const int k) 
{
   if (DebugControl::getDebugOn()) std::cout << "Voxel highlight: " << i << " " << j << " " << k << std::endl;
   
   BrainModelSurface* bms = theMainWindow->getBrainSet()->getActiveFiducialSurface();
   if (bms == NULL) {
      QMessageBox::critical(theMainWindow, "FIV Comm Error", 
                            "There is no Fiducial Coordinate file which is needed for\n"
                            "successful communication with FIV.  The coordinate file\n"
                            "must be in Talairach Space.");
      return;
   }
   
   const CoordinateFile* cf = bms->getCoordinateFile();
   const int nodeNumber = cf->getCoordinateIndexClosestToPoint(i, j, k);
   
   if (DebugControl::getDebugOn()) std::cout << "closest node " << nodeNumber << std::endl;
   if (nodeNumber > 0) {
      theMainWindow->identifyNodeFromRemoteProgram(nodeNumber);
   }
}

/**
 * Send a voxel highlight to FIV by converting a node position to a voxel position.
 */
void
CommunicatorClientFIV::sendNodeHighlightToFIV(const int nodeNumber) 
{
   if (socket->state() == QTcpSocket::ConnectedState) {
      BrainModelSurface* bms = theMainWindow->getBrainSet()->getActiveFiducialSurface();
      if (bms == NULL) {
         QMessageBox::critical(theMainWindow, "FIV Comm Error", 
                              "There is no Fiducial Coordinate file which is needed for\n"
                              "successful communication with FIV.  The coordinate file\n"
                              "must be in Talairach Space.");
         return;
      }
      
      const CoordinateFile* cf = bms->getCoordinateFile();
      
      if ((nodeNumber >= 0) &&
          (nodeNumber < cf->getNumberOfCoordinates())) {
         float x, y, z;
         cf->getCoordinate(nodeNumber, x, y, z);
         
         int i = static_cast<int>(x);
         int j = static_cast<int>(y);
         int k = static_cast<int>(z);
         
         std::ostringstream str;
         str << "TAL_XYZ "
               << i << " "
               << j << " "
               << k << std::ends;
         sendMessage(str.str().c_str());
      }
   }
}


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

#include "CommunicatorClientAFNI.h"
#include "DebugControl.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiIdentifyDialog.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "FileUtilities.h"
#include "StringUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
CommunicatorClientAFNI::CommunicatorClientAFNI() : CommunicatorClientBase("AFNI",
                                                        "localhost",
                                                        7955,   // control port 
                                                        8009,   // data port
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
CommunicatorClientAFNI::~CommunicatorClientAFNI()
{
}

/**
 * Create the first message that is to be sent to the program that we are connecting to
 */
void
CommunicatorClientAFNI::createControlMessage()
{
   //
   // Create string to send to AFNI to give data port and other info.
   // Multiple strings are separated with '\n'.
   //
   QString msg;
   
   //
   // PONAME means use this string for informative messages
   //
   msg.append("PONAME Caret5-to-AFNI");
   msg.append("\n");
   
   //
   // IOCHAN means use this for the data channel
   //
   char str[256];
   sprintf(str, "tcp:%s:%d", hostName.toAscii().constData(), dataPortNumber);
   msg.append("IOCHAN ");
   msg.append(str);
   msg.append("\n");
   
   //
   // Do not request acknowlegements
   //
   msg.append("NO_ACK");
   msg.append("\n");
   
   if (requestUpdates) {
      //
      // Request volume crosshair updates 
      //
      msg.append("DSET_IJK_DELTA");
      msg.append("\n");
      
      //
      // Request notification when structural volume changes
      //
      msg.append("UNDERLAY_DELTA");
      msg.append("\n");
   }
   
   setControlMessage(msg);
}

/**
 * Receive the messages from the AFNI program
 */
void
CommunicatorClientAFNI::processReceivedMessage(const QString& msg)
{
   //
   // Incoming data may have more than one message with the messages
   // separated by a newline.
   //
   std::vector<QString> messages;
   StringUtilities::token(msg, "\n", messages);
   
   for (unsigned int i = 0; i < messages.size(); i++) {
      if (DebugControl::getDebugOn()) std::cout << "AFNI Message: " << messages[i].toAscii().constData() << std::endl;
      
      std::vector<QString> tokens;
      StringUtilities::token(messages[i], " \t\n", tokens);
      
      if (tokens.size() > 0) {
         if (tokens[0] == "UNDERLAY") {
            if (tokens.size() > 1) {
               findTransformationMatrix(tokens[1]);
            }
            else {
            }
         }
         else if (tokens[0] == "DSET_IJK") {
            if (tokens.size() > 3) {
               processVoxelHighlightFromAFNI(tokens[1].toInt(), tokens[2].toInt(), 
                                             tokens[3].toInt());
            }
         }
      }
   }
}

/**
 * Find the appropriate transformation matrix for the AFNI .HEAD file. 
 */
void
CommunicatorClientAFNI::findTransformationMatrix(const QString& afniHeadFileNameIn)
{
   if (afniHeadFileNameIn.isEmpty()) {
      return;
   }

   const QString fileName(FileUtilities::basename(afniHeadFileNameIn));
   
   bool foundTransformationMatrix = false;
   bool foundCoordinateFile = false;
   QString coordFileName;
   
   TransformationMatrixFile* tmf = theMainWindow->getBrainSet()->getTransformationMatrixFile();
   for (int i = 0; i < tmf->getNumberOfMatrices(); i++) {
      TransformationMatrix* tm = tmf->getTransformationMatrix(i);
      if (tm->getMatrixTargetVolumeFileName() == fileName) {
         if (DebugControl::getDebugOn()) std::cout << "Found proper transformation matrix" << std::endl;
         foundTransformationMatrix = true;
         transformMatrix = *tm;
         coordFileName = tm->getMatrixFiducialCoordFileName();
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
            BrainModel* bm = theMainWindow->getBrainSet()->getBrainModel(i);
            if (bm->getModelType() == BrainModel::BRAIN_MODEL_SURFACE) {
               BrainModelSurface* bms = dynamic_cast<BrainModelSurface*>(bm);
               CoordinateFile* cf = bms->getCoordinateFile();
               if (FileUtilities::basename(cf->getFileName()) == coordFileName) {
                  foundCoordinateFile = true;
                  coordinateFile = *cf;
                  if (DebugControl::getDebugOn()) std::cout << "Found coordinate file" << std::endl;
               }
            }
         }
         break;
      }
   }
   
   if (foundTransformationMatrix == false) {
      QString msg("Unable to find a transformation matrix for volume ");
      msg.append(fileName);
      GuiMessageBox::critical(theMainWindow, "AFNI Communication Error", msg, "OK"); 
   }
   else if (foundCoordinateFile == false) {
      QString msg("Coordinate file needed for AFNI communication not loaded ");
      msg.append(coordFileName);
      GuiMessageBox::critical(theMainWindow, "AFNI Communication Error", msg, "OK"); 
   }
}

/**
 * Process a voxel highlight request from AFNI.
 */
void
CommunicatorClientAFNI::processVoxelHighlightFromAFNI(const int i, const int j, const int k) 
{
   if (DebugControl::getDebugOn()) std::cout << "Voxel highlight: " << i << " " << j << " " << k << std::endl;
   double p[4] = { i, j, k, 1.0 };
   transformMatrix.inverseMultiplyPoint(p);
   if (DebugControl::getDebugOn()) std::cout << "coordinate: " << p[0] << " " << p[1] << " " << p[2] << std::endl;
   
   if (coordinateFile.getNumberOfCoordinates() > 0) {
      const int nodeNumber = coordinateFile.getCoordinateIndexClosestToPoint(p[0], p[1], p[2]);
      if (DebugControl::getDebugOn()) std::cout << "closest node " << nodeNumber << std::endl;
      if (nodeNumber > 0) {
         theMainWindow->identifyNodeFromRemoteProgram(nodeNumber);
      }
   }
}

/**
 * Send a voxel highlight to AFNI by converting a node position to a voxel position.
 */
void
CommunicatorClientAFNI::sendNodeHighlightToAFNI(const int nodeNumber) 
{
   if (socket->state() == QTcpSocket::ConnectedState) {
      if ((coordinateFile.getNumberOfCoordinates() > 0) &&
          (nodeNumber < coordinateFile.getNumberOfCoordinates())) {
         float x, y, z;
         coordinateFile.getCoordinate(nodeNumber, x, y, z);
         double p[4] = { x, y, z, 1.0 };
         transformMatrix.multiplyPoint(p);
         
         int i = static_cast<int>(p[0]);
         int j = static_cast<int>(p[1]);
         int k = static_cast<int>(p[2]);
         
         if ((i >= 0) && (j >= 0) && (k >= 0)) {
            std::ostringstream str;
            str << "DSET_IJK_SET "
               << i << " "
               << j << " "
               << k << std::ends;
            sendMessage(str.str().c_str());
         }
      }
   }
}


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


#ifndef __VE_COMMUNICATOR_CLIENT_AFNI_H__
#define __VE_COMMUNICATOR_CLIENT_AFNI_H__

#include "CommunicatorClientBase.h"
#include "CoordinateFile.h"
#include "TransformationMatrixFile.h"

/// This class is used to communicate with the AFNI program.
class CommunicatorClientAFNI : public CommunicatorClientBase {

   Q_OBJECT
      
   public:
      /// Constructor
      CommunicatorClientAFNI();
      
      /// Destructor
      ~CommunicatorClientAFNI();
      
      ///  send a node highlight to AFNI
      void sendNodeHighlightToAFNI(const int nodeNumber);
      
   public slots:
      /// called when the socket is successfully connected
      //void socketConnected();
      
   private:
      /// create the control message
      void createControlMessage();
      
      /// Find the transformation matrix for the AFNI .HEAD file.
      void findTransformationMatrix(const QString& afniHeadFileName);      
      
      /// Get message from the CommunicatorClientBase
      void processReceivedMessage(const QString& msg);
      
      /// process a voxel highlight request from AFNI
      void processVoxelHighlightFromAFNI(const int i, const int j, const int k);

      /// Coordinate file for converting between voxel and surface spaces
      CoordinateFile coordinateFile;
      
      /// Transformation matrix used for converting between voxel and surface spaces.
      TransformationMatrix transformMatrix;
};

#endif


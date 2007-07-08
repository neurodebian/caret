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


#ifndef __VE_COMMUNICATOR_CLIENT_FIV_H__
#define __VE_COMMUNICATOR_CLIENT_FIV_H__

#include "CommunicatorClientBase.h"
#include "CoordinateFile.h"
#include "TransformationMatrixFile.h"

/// This class is used to communicate with the FIV program.
class CommunicatorClientFIV : public CommunicatorClientBase {

   Q_OBJECT
      
   public:
      /// Constructor
      CommunicatorClientFIV();
      
      /// Destructor
      ~CommunicatorClientFIV();
      
      ///  send a node highlight to FIV
      void sendNodeHighlightToFIV(const int nodeNumber);
      
   public slots:
      /// called when the socket is successfully connected
      //void socketConnected();
      
   private:
      /// create the control message
      void createControlMessage();
      
      /// Get message from the CommunicatorClientBase
      void processReceivedMessage(const QString& msg);
      
      /// process a voxel highlight request from FIV
      void processVoxelHighlightFromFIV(const int i, const int j, const int k);
};

#endif


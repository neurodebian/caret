
#ifndef __BRAIN_MODEL_SURFACE_FIND_EXTREMUM_H__
#define __BRAIN_MODEL_SURFACE_FIND_EXTREMUM_H__

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

#include <vector>

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class BrainModelSurfaceROINodeSelection;
class PaintFile;

/// class for starting at a node, moving as far as possible in specified direction along mesh 
class BrainModelSurfaceFindExtremum : public BrainModelAlgorithm {
   public:
      /// direction for search
      enum DIRECTION {
         /// invalid direction
         DIRECTION_INVALID,
         /// move laterally
         DIRECTION_LATERAL,
         /// move medially
         DIRECTION_MEDIAL,
         /// move negative X
         DIRECTION_X_NEGATIVE,
         /// move positive X
         DIRECTION_X_POSITIVE,
         /// move negative Y
         DIRECTION_Y_NEGATIVE,
         /// move positive Y
         DIRECTION_Y_POSITIVE,
         /// move negative Z
         DIRECTION_Z_NEGATIVE,
         /// move positive Z
         DIRECTION_Z_POSITIVE
      };
      
      /// paint operation
      enum PAINT_OPERATION {
         /// no paint operation
         PAINT_OPERATION_NONE,
         /// move until paint identity changes
         PAINT_OPERATION_STOP_WHEN_PAINT_ID_CHANGES
      };
      
      /// normal restriction
      enum NORMAL_RESTRICTION {
         /// no restriction
         NORMAL_RESTRICTION_NONE,
         /// restrict to negative normals
         NORMAL_RESTRICTION_NEGATIVE,
         /// restrict to positive normals
         NORMAL_RESTRICTION_POSITIVE
      };
      
      // constructor
      BrainModelSurfaceFindExtremum(BrainSet* brainSetIn,
                                    BrainModelSurface* bmsIn,
                                    const DIRECTION searchDirectionIn,
                                    const int startNodeNumberIn,
                                    const float maximumMovementXIn,
                                    const float maximumMovementYIn,
                                    const float maximumMovementZIn);
                                    
      // constructor
      BrainModelSurfaceFindExtremum(BrainSet* brainSetIn,
                                    BrainModelSurface* bmsIn,
                                    const DIRECTION searchDirectionIn,
                                    const float startXYZ[3],
                                    const float maximumMovementXIn,
                                    const float maximumMovementYIn,
                                    const float maximumMovementZIn);
                                    
      // constructor
      BrainModelSurfaceFindExtremum(BrainSet* brainSetIn,
                                    BrainModelSurface* bmsIn,
                                    const DIRECTION searchDirectionIn,
                                    const int startNodeNumberIn,
                                    const float maximumMovementXIn,
                                    const float maximumMovementYIn,
                                    const float maximumMovementZIn,
                                    const PaintFile* paintFileIn,
                                    const int paintColumnNumberIn,
                                    const int paintIndexIn,
                                    const PAINT_OPERATION paintOperationIn);
                                    
      // destructor
      ~BrainModelSurfaceFindExtremum();
      
      // restrict nodes to those in the roi
      void setNodeRestrictionWithROI(const BrainModelSurfaceROINodeSelection* restrictToROIIn);
      
      // restrict nodes to those with normals of specified signs
      void setNodeNormalRestriction(const NORMAL_RESTRICTION xNormalRestrictionIn,
                                    const NORMAL_RESTRICTION yNormalRestrictionIn,
                                    const NORMAL_RESTRICTION zNormalRestrictionIn);
                                    
      // get the extremum node that was found (-1 if invalid)
      int getExtremumNode() const { return extremumNode; }
      
      // get nodes in path to extremum including extremum
      void getNodeInPathToExtremum(std::vector<int>& nodesInPathToExtremumOut) const;
      
      // set a region of interest to nodes in path
      void setRegionOfInterestToNodesInPath(BrainModelSurfaceROINodeSelection& roi) const;
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// see if a node is within maximum movement allowance
      bool withinMovementAllowance(const float x,
                                   const float y,
                                   const float z) const;
      
      /// check the node's normal
      bool checkNodeNormal(const float* normalVector) const;
      
      /// the surface 
      BrainModelSurface* bms;
      
      /// starting node number
      int startNodeNumber;
      
      /// starting XYZ
      float startXYZ[3];
      
      /// direction to search
      DIRECTION searchDirection;
      
      /// the extremum node that was found
      int extremumNode;
      
      /// maximum movement allowed in X direction when searching for extremum
      float maximumMovementX;
      
      /// maximum movement allowed in Y direction when searching for extremum
      float maximumMovementY;
      
      /// maximum movement allowed in Z direction when searching for extremum
      float maximumMovementZ;
      
      /// nodes in path to extremum
      std::vector<int> nodesInPathToExtremum;
      
      /// paint operation
      PAINT_OPERATION paintOperation;
      
      /// paint file
      PaintFile* paintFile;
      
      /// paint column number
      int paintColumnNumber;
      
      /// paint index
      int paintIndex;
      
      /// normal restrictions
      NORMAL_RESTRICTION normalRestriction[3];
      
      /// search ROI restriction
      BrainModelSurfaceROINodeSelection* restrictToROI;
};

#endif // __BRAIN_MODEL_SURFACE_FIND_EXTREMUM_H__

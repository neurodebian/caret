
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

#include <cmath>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceFindExtremum.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
BrainModelSurfaceFindExtremum::BrainModelSurfaceFindExtremum(BrainSet* brainSetIn,
                              BrainModelSurface* bmsIn,
                              const DIRECTION searchDirectionIn,
                              const int startNodeNumberIn,
                              const float maximumMovementXIn,
                              const float maximumMovementYIn,
                              const float maximumMovementZIn)
   : BrainModelAlgorithm(brainSetIn)
{
   bms = bmsIn;
   startNodeNumber = startNodeNumberIn;
   searchDirection = searchDirectionIn;
   startXYZ[0] = 0.0;
   startXYZ[1] = 0.0;
   startXYZ[2] = 0.0;
   extremumNode = -1;
   maximumMovementX = maximumMovementXIn;
   maximumMovementY = maximumMovementYIn;
   maximumMovementZ = maximumMovementZIn;
}
                              
/**
 * constructor.
 */
BrainModelSurfaceFindExtremum::BrainModelSurfaceFindExtremum(BrainSet* brainSetIn,
                              BrainModelSurface* bmsIn,
                              const DIRECTION searchDirectionIn,
                              const float startXYZIn[3],
                              const float maximumMovementXIn,
                              const float maximumMovementYIn,
                              const float maximumMovementZIn)
   : BrainModelAlgorithm(brainSetIn)
{
   bms = bmsIn;
   startNodeNumber = -1;
   searchDirection = searchDirectionIn;
   startXYZ[0] = startXYZIn[0];
   startXYZ[1] = startXYZIn[1];
   startXYZ[2] = startXYZIn[2];
   extremumNode = -1;
   maximumMovementX = maximumMovementXIn;
   maximumMovementY = maximumMovementYIn;
   maximumMovementZ = maximumMovementZIn;
}
                              
/**
 * destructor.
 */
BrainModelSurfaceFindExtremum::~BrainModelSurfaceFindExtremum()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceFindExtremum::execute() throw (BrainModelAlgorithmException)
{
   nodesInPathToExtremum.clear();
   
   if (searchDirection == DIRECTION_INVALID) {
      throw BrainModelAlgorithmException("Search direction is invalid.");
   }
   if (bms == NULL) {
      throw BrainModelAlgorithmException("Surface is invalid.");
   }
   const CoordinateFile* cf = bms->getCoordinateFile();
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes <= 0) {
      throw BrainModelAlgorithmException("Surface contains no nodes.");
   }
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      throw BrainModelAlgorithmException("Surface has no topology.");
   }
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   // Convert lateral/medial search to negative or positive X search
   //
   if (searchDirection == DIRECTION_LATERAL) {
      if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
         searchDirection = DIRECTION_X_NEGATIVE;
      }
      else if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
         searchDirection = DIRECTION_X_POSITIVE;
      }
      else {
         throw BrainModelAlgorithmException(
            "Surface has invalid structure need to for lateral search.");
      }
   }
   else if (searchDirection == DIRECTION_MEDIAL) {
      if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
         searchDirection = DIRECTION_X_POSITIVE;
      }
      else if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
         searchDirection = DIRECTION_X_NEGATIVE;
      }
      else {
         throw BrainModelAlgorithmException(
            "Surface has invalid structure need to for lateral search.");
      }
   }
   
   //
   // If extremum node was not set, find node closest to start XYZ
   //
   if (startNodeNumber < 0) {
      startNodeNumber = bms->getNodeClosestToPoint(startXYZ);
   }
   else {
      cf->getCoordinate(startNodeNumber, startXYZ);
   }
   extremumNode = startNodeNumber;
   if (extremumNode < 0) {
      throw BrainModelAlgorithmException("Unable to determine starting node from XYZ.");
   }
   
   //
   // add nodes to path
   //
   nodesInPathToExtremum.push_back(extremumNode);

   //
   // Search surface until unable to proceed any further
   //
   bool done = false;
   while (done == false) {
      //
      // node's XYZ
      //
      float x, y, z;
      cf->getCoordinate(extremumNode, x, y, z);
      
      //
      // Get the neighbors of the current node
      //
      int numNeighbors = 0;
      const int* neighbors = th->getNodeNeighbors(extremumNode, 
                                                  numNeighbors);
      
      //
      // next node and value for searching
      //
      int nextNode = -1;
      float nextValue = 0.0;
      
      //
      // Loop through neighbors
      //
      for (int j = 0; j < numNeighbors; j++) {
         const int neighborNode = neighbors[j];
         float nx, ny, nz;
         cf->getCoordinate(neighborNode, nx, ny, nz);
         
         switch (searchDirection) {
            case DIRECTION_INVALID:
               break;
            case DIRECTION_LATERAL:
               break;
            case DIRECTION_MEDIAL:
               break;
            case DIRECTION_X_NEGATIVE:
               if (nx < x) {
                  if ((nextNode < 0) ||
                      (nx < nextValue)) {
                     if (withinMovementAllowance(nx, ny, nz)) {
                        nextNode = neighborNode;
                        nextValue = nx;
                     }
                  }
               }
               break;
            case DIRECTION_X_POSITIVE:
               if (nx > x) {
                  if ((nextNode < 0) ||
                      (nx > nextValue)) {
                     if (withinMovementAllowance(nx, ny, nz)) {
                        nextNode = neighborNode;
                        nextValue = nx;
                     }
                  }
               }
               break;
            case DIRECTION_Y_NEGATIVE:
               if (ny < y) {
                  if ((nextNode < 0) ||
                      (ny < nextValue)) {
                     if (withinMovementAllowance(nx, ny, nz)) {
                        nextNode = neighborNode;
                        nextValue = ny;
                     }
                  }
               }
               break;
            case DIRECTION_Y_POSITIVE:
               if (ny > y) {
                  if ((nextNode < 0) ||
                      (ny > nextValue)) {
                     if (withinMovementAllowance(nx, ny, nz)) {
                        nextNode = neighborNode;
                        nextValue = ny;
                     }
                  }
               }
               break;
            case DIRECTION_Z_NEGATIVE:
               if (nz < z) {
                  if ((nextNode < 0) ||
                      (nz < nextValue)) {
                     if (withinMovementAllowance(nx, ny, nz)) {
                        nextNode = neighborNode;
                        nextValue = nz;
                     }
                  }
               }
               break;
            case DIRECTION_Z_POSITIVE:
               if (nz > z) {
                  if ((nextNode < 0) ||
                      (nz > nextValue)) {
                     if (withinMovementAllowance(nx, ny, nz)) {
                        nextNode = neighborNode;
                        nextValue = nz;
                     }
                  }
               }
               break;
         }
      }
      
      //
      // Was next node found ?
      //
      if (nextNode >= 0) {
         //
         // move to next node
         //
         extremumNode = nextNode;
      
         //
         // add nodes to path
         //
         nodesInPathToExtremum.push_back(extremumNode);
      }
      else {
         //
         // done
         //
         done = true;
      }
   }
}
      
/**
 * see if a node is within maximum movement allowance.
 */
bool 
BrainModelSurfaceFindExtremum::withinMovementAllowance(const float x,
                                                       const float y,
                                                       const float z) const
{
   const float dx = std::fabs(x - startXYZ[0]);
   const float dy = std::fabs(y - startXYZ[1]);
   const float dz = std::fabs(z - startXYZ[2]);
   
   //std::cout << "y: " << y 
   //          << "  dy: " << dy
   //          << "  maximumMovementY: " << maximumMovementY
   //          << std::endl;
   if ((dx < maximumMovementX) &&
       (dy < maximumMovementY) &&
       (dz < maximumMovementZ)) {
      return true;
   }
   
   return false;
}
      
/**
 * get nodes in path to extremum including extremum.
 */
void 
BrainModelSurfaceFindExtremum::getNodeInPathToExtremum(std::vector<int>& nodesInPathToExtremumOut) const
{
   nodesInPathToExtremumOut = nodesInPathToExtremum;
}
      
/**
 * set a region of interest to nodes in path.
 */
void 
BrainModelSurfaceFindExtremum::setRegionOfInterestToNodesInPath(BrainModelSurfaceROINodeSelection& roi) const
{
   roi.update();
   
   roi.deselectAllNodes();
   
   const int num = nodesInPathToExtremum.size();
   for (int i = 0; i < num; i++) {
      roi.setNodeSelected(nodesInPathToExtremum[i], 1);
   }
}
      

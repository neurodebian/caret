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

#include "BrainModelSurface.h"
#include "BrainModelSurfacePaintAssignRelativeToLine.h"
#include "CoordinateFile.h"
#include "MathUtilities.h"
#include "PaintFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
BrainModelSurfacePaintAssignRelativeToLine::BrainModelSurfacePaintAssignRelativeToLine(
                                           BrainSet* brainSetIn,
                                           BrainModelSurface * bmsIn,
                                           PaintFile* paintFileIn,
                                           const int paintFileColumnNumberIn,
                                           const int augmentPaintRegionNameIndexIn,
                                           const int newPaintNameIndexIn,
                                           const float lineStartXYZIn[3],
                                           const float lineEndXYZIn[3],
                                           const float minimumDistanceToLineIn,
                                           const float maximumDistanceToLineIn,
                                           const std::vector<int>& limitToPaintIndicesIn,
                                           const float nodeExtentLimitIn[6])
   : BrainModelAlgorithm(brainSetIn)
{
   bms = bmsIn;
   paintFile = paintFileIn;
   paintFileColumnNumber = paintFileColumnNumberIn;
   augmentPaintRegionNameIndex = augmentPaintRegionNameIndexIn;
   newPaintNameIndex = newPaintNameIndexIn;
   lineStartXYZ[0] = lineStartXYZIn[0];
   lineStartXYZ[1] = lineStartXYZIn[1];
   lineStartXYZ[2] = lineStartXYZIn[2];
   lineEndXYZ[0] = lineEndXYZIn[0];
   lineEndXYZ[1] = lineEndXYZIn[1];
   lineEndXYZ[2] = lineEndXYZIn[2];
   minimumDistanceToLine = minimumDistanceToLineIn;
   maximumDistanceToLine = maximumDistanceToLineIn;
   limitToPaintIndices = limitToPaintIndicesIn;
   nodeExtentLimit[0] = nodeExtentLimitIn[0];
   nodeExtentLimit[1] = nodeExtentLimitIn[1];
   nodeExtentLimit[2] = nodeExtentLimitIn[2];
   nodeExtentLimit[3] = nodeExtentLimitIn[3];
   nodeExtentLimit[4] = nodeExtentLimitIn[4];
   nodeExtentLimit[5] = nodeExtentLimitIn[5];
}

/**
 * destructor.
 */
BrainModelSurfacePaintAssignRelativeToLine::~BrainModelSurfacePaintAssignRelativeToLine()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfacePaintAssignRelativeToLine::execute() throw (BrainModelAlgorithmException) 
{
   throw BrainModelAlgorithmException("BrainModelSurfacePaintAssignRelativeToLine  NOT FUNCTIONING YET.");
   if (bms == NULL) {
      throw BrainModelAlgorithmException("Surface is invalid.");
   }
   if (paintFile == NULL) {
      throw BrainModelAlgorithmException("Paint file is invalid.");
   }
   
   //
   // Get coordinates and topology
   //
   const int numNodes = bms->getNumberOfNodes();
   const CoordinateFile* cf = bms->getCoordinateFile();
   const TopologyFile* tf = bms->getTopologyFile();
   
   //
   // Get the topology helper
   //
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   const int numExistingPaintNames = static_cast<int>(limitToPaintIndices.size());

   //
   // Copy the region paint indices
   //
   std::vector<int> regionPaintIndices(numNodes);
   for (int i = 0; i < numNodes; i++) {
      regionPaintIndices[i] = paintFile->getPaint(i, paintFileColumnNumber);
   }
   
   //
   // Loop until done
   //
   bool done = false;
   while (done == false) {
      //
      // Keep track of nodes that get paint changed
      //
      int numNodesChanged = 0;
      
      //
      // loop through nodes
      //
      for (int i = 0; i < numNodes; i++) {
         //
         // If node is not new paint or region paint
         //
         const int nodePaintIndex = paintFile->getPaint(i, paintFileColumnNumber);
         if ((regionPaintIndices[i] != augmentPaintRegionNameIndex) &&
             (nodePaintIndex != newPaintNameIndex)) {
            
            //
            // Get neighbors
            //
            int numNeighbors = 0;
            const int* neighbors = th->getNodeNeighbors(i, numNeighbors);

            //
            // Loop through neighbors
            //
            bool testNode = false;
            for (int j = 0; j < numNeighbors; j++) {
               const int neighborNode = neighbors[j];
               //
               // Is its neighbor part of the augment region?
               //
               if (regionPaintIndices[neighborNode] == augmentPaintRegionNameIndex) {
                  //
                  // Test this node
                  //
                  testNode = true;
                  break;
               }
            }
            
            //
            // Is the paints that can be overwritten limited
            //
            if (numExistingPaintNames > 0) {
               //
               // May not want to test this node
               //
               testNode = false;
               
               //
               // Is it one of the existing paint names that can be overwritten
               //
               for (int j = 0; j < numExistingPaintNames; j++) {
                  if (paintFile->getPaint(i, paintFileColumnNumber) ==
                      limitToPaintIndices[j]) {
                     testNode = true;
                     break;
                  }
               }
            }
            
            //
            // If this node should be tested
            //
            const float* nodeXYZ = cf->getCoordinate(i);
            if (testNode) {
               //
               // See if within abs limits
               //
               testNode = false;
               if ((nodeXYZ[0] >= nodeExtentLimit[0]) &&
                   (nodeXYZ[0] <= nodeExtentLimit[1]) &&
                   (nodeXYZ[1] >= nodeExtentLimit[2]) &&
                   (nodeXYZ[1] <= nodeExtentLimit[3]) &&
                   (nodeXYZ[2] >= nodeExtentLimit[4]) &&
                   (nodeXYZ[2] <= nodeExtentLimit[5])) {
                  testNode = true;
               }
            }
            
            if (testNode) {
               //
               // Get distance to line
               //
               const float distanceToLine =
                  MathUtilities::distancePointToLine3D(nodeXYZ,
                                                       lineStartXYZ,
                                                       lineEndXYZ);
                                                       
               
               //
               // Is distance within limit
               //
               testNode = false;
               if ((distanceToLine >= minimumDistanceToLine) &&
                   (distanceToLine <= maximumDistanceToLine)) {
                  testNode = true;
               }
            }
         }
      }
      
      
      //
      // If no nodes where changed
      //
      if (numNodesChanged <= 0) {
         done = true;
      }
      else {
         //
         // add a layer around augment paints
         //
         for (int i = 0; i < numNodes; i++) {
            //
            // Is this the paint that is to be augmented
            //
            if (regionPaintIndices[i] == augmentPaintRegionNameIndex) {
               //
               // Get the neighbors
               //
               int numNeighbors = 0;
               const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
               for (int j = 0; j < numNeighbors; j++) {
                  
                  //
                  // Add node to region
                  //
                  bool addNodeToRegion = true;
                  
                  const int node = neighbors[j];
                  if (numExistingPaintNames > 0) {
                     //
                     // Is it one of the existing paint names that can be overwritten
                     //
                     addNodeToRegion = false;
                     for (int j = 0; j < numExistingPaintNames; j++) {
                        if (paintFile->getPaint(node, paintFileColumnNumber) ==
                            limitToPaintIndices[j]) {
                           addNodeToRegion = true;
                           break;
                        }
                     }
                  }
                  
                  if (addNodeToRegion) {
                     regionPaintIndices[node] = augmentPaintRegionNameIndex;
                  }
               }
            }
         }
      }
   }
}


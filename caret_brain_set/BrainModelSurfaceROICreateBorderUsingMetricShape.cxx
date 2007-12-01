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
#include "BrainModelSurfaceROICreateBorderUsingMetricShape.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
BrainModelSurfaceROICreateBorderUsingMetricShape::BrainModelSurfaceROICreateBorderUsingMetricShape(
                                              BrainSet* bs,
                                              BrainModelSurface* bmsIn,
                                              BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                              const MODE modeIn,
                                              MetricFile* metricFileIn,
                                              const int metricColumnNumberIn,
                                              const QString& borderNameIn,
                                              const int startNodeIn,
                                              const int endNodeIn,
                                              const float samplingDensityIn)
   : BrainModelSurfaceROIOperation(bs, bmsIn, surfaceROIIn)
{
   mode = modeIn;
   metricFile = metricFileIn;
   metricColumnNumber = metricColumnNumberIn;
   borderName      = borderNameIn;
   borderStartNode = startNodeIn;
   borderEndNode   = endNodeIn;
   borderSamplingDensity = samplingDensityIn;
}

/**
 * destructor.
 */
BrainModelSurfaceROICreateBorderUsingMetricShape::~BrainModelSurfaceROICreateBorderUsingMetricShape()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROICreateBorderUsingMetricShape::executeOperation() throw (BrainModelAlgorithmException)
{
   const int numNodesInROI = surfaceROI->getNumberOfNodesSelected();
   
   if (numNodesInROI == 1) {
      throw BrainModelAlgorithmException("There is only one node, the starting node, in the ROI.");
   }
   
   const int numNodes = bms->getNumberOfNodes();
   const CoordinateFile* cf = bms->getCoordinateFile();
   
   border.clearLinks();
   
   //
   // Check Inputs
   //
   if (metricFile == NULL) {
      throw BrainModelAlgorithmException("Metric/Shape file is invalid.");
   }
   if ((metricColumnNumber < 0) ||
       (metricColumnNumber >= metricFile->getNumberOfColumns())) {
       throw BrainModelAlgorithmException("Metric/Shape file column number is invalid.");
   }
   if (borderName.isEmpty()) {
      throw BrainModelAlgorithmException("Name for border is empty.");
   }
   if ((borderStartNode < 0) ||
       (borderStartNode >= numNodes)) {
         throw BrainModelAlgorithmException("Starting node is invalid.");
   }
   if ((borderEndNode < 0) ||
       (borderEndNode >= numNodes)) {
         throw BrainModelAlgorithmException("Ending node is invalid.");
   }
   if (borderStartNode == borderEndNode) {
      throw BrainModelAlgorithmException("Starting and ending node are the same.");
   }
   if (surfaceROI->getNodeSelected(borderStartNode) == false) {
      throw BrainModelAlgorithmException("Starting node is not in the ROI");
   }
   if (surfaceROI->getNodeSelected(borderEndNode) == false) {
      throw BrainModelAlgorithmException("Ending node is not in the ROI");
   }
   
   //
   // Get a topology helper
   //
   const TopologyFile* tf = bms->getTopologyFile();
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   // Beginning of border
   //
   border.addBorderLink(cf->getCoordinate(borderStartNode));
   
   //
   // Coordinate of ending node
   //
   const float* endXYZ = cf->getCoordinate(borderEndNode);
   
   //
   // Loop until path from starting to ending nodes is found
   //
   int currentNode = borderStartNode;
   bool done = false;
   while (done == false) {
      //
      // Get neighbors of current node
      //
      int numNeighbors = 0;
      const int* neighbors = th->getNodeNeighbors(currentNode, numNeighbors);
      
      //
      // see if any neighbors is the ending node
      //
      for (int i = 0; i < numNeighbors; i++) {
         if (neighbors[i] == borderEndNode) {
            border.addBorderLink(endXYZ);
            done = true;
            break;
         }
      }
      
      //
      // Need to search neighbors
      //
      if (done == false) {
         //
         // Find distance from current node to ending node
         //
         const float currentNodeDistanceToEndingNode = 
            MathUtilities::distanceSquared3D(cf->getCoordinate(currentNode), endXYZ);
            
         //
         // Next node for border
         //
         int nextNode = -1;
         float nextNodeMetricValue = 0.0;
         
         //
         // Loop through neighbors
         //
         for (int i = 0; i < numNeighbors; i++) {
            const int neighborNode = neighbors[i];
            
            //
            // Is node in the ROI
            //
            if (surfaceROI->getNodeSelected(neighborNode)) {
               //
               // Is distance from neighbor to end node closer than distance
               // form current node to end node
               //
               const float distance = 
                  MathUtilities::distanceSquared3D(cf->getCoordinate(neighborNode), endXYZ);
               if (distance < currentNodeDistanceToEndingNode) {
                  //
                  // Is metric value the most positive or negative
                  //
                  const float metricValue = metricFile->getValue(neighborNode,
                                                                 metricColumnNumber);
                  if (nextNode < 0) {
                     nextNode = neighborNode;
                     nextNodeMetricValue = metricValue;
                  }
                  else {
                     switch (mode) {
                        case MODE_FOLLOW_MOST_NEGATIVE:
                           if (metricValue < nextNodeMetricValue) {
                              nextNode = neighborNode;
                              nextNodeMetricValue = metricValue;
                           }
                           break;
                        case MODE_FOLLOW_MOST_POSITIVE:
                           if (metricValue > nextNodeMetricValue) {
                              nextNode = neighborNode;
                              nextNodeMetricValue = metricValue;
                           }
                           break;
                     }
                  }
               } // if
            }
         } // for
         
         if (nextNode >= 0) {
            border.addBorderLink(cf->getCoordinate(nextNode));
            currentNode = nextNode;
         }
         else {
            throw BrainModelAlgorithmException("Create Border: unable to complete path "
                                               "from starting to ending node.  Stuck at "
                                               "node " + QString::number(currentNode));
         }
      }
   }
   
   //
   // Name the border
   //
   border.setName(borderName);
}

/**
 * get the border that was created by create border mode.
 */
Border 
BrainModelSurfaceROICreateBorderUsingMetricShape::getBorder() const
{
   return border;
}

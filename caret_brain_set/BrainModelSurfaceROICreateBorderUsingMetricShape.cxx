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

#include <deque>
#include <iostream>
#include <limits>

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
 * constructor.  It is best to use a "lightly folded" surface such as a 
 * very inflated surface or and ellipsoid surface.  When finding the path,
 * the distance from the next node to the end node must be less than the
 * distance from the current node to the end node.  When the surface is
 * highly folded, it may be necessary to move away from the end node 
 * which will cause the algorithm to fail.
 */
BrainModelSurfaceROICreateBorderUsingMetricShape::BrainModelSurfaceROICreateBorderUsingMetricShape(
                                              BrainSet* bs,
                                              const BrainModelSurface* bmsIn,
                                              const BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                              const MODE modeIn,
                                              const MetricFile* metricFileIn,
                                              const int metricColumnNumberIn,
                                              const QString& borderNameIn,
                                              const int startNodeIn,
                                              const int endNodeIn,
                                              const float samplingDensityIn)
   : BrainModelSurfaceROIOperation(bs, bmsIn, surfaceROIIn),
     mode(modeIn),
     metricFile(metricFileIn),
     metricColumnNumber(metricColumnNumberIn),
     borderName(borderNameIn),
     borderStartNode(startNodeIn),
     borderEndNode(endNodeIn),
     borderSamplingDensity(samplingDensityIn)
{
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
   if (borderName.isEmpty()) {
      throw BrainModelAlgorithmException("Name for border is empty.");
   }

   BrainModelSurfaceROINodeSelection theROI(*operationSurfaceROI);
   const int numNodesInROI = theROI.getNumberOfNodesSelected();
   
   if (numNodesInROI == 1) {
      throw BrainModelAlgorithmException("There is only one node, the starting node, in the ROI "
                                         " for border named " + borderName);
   }
   
   const int numNodes = bms->getNumberOfNodes();
   const CoordinateFile* cf = bms->getCoordinateFile();
   
   border.clearLinks();
   
   //
   // Check Inputs
   //
   if (metricFile == NULL) {
      throw BrainModelAlgorithmException("Metric/Shape file is invalid for border named " + borderName);
   }
   if ((metricColumnNumber < 0) ||
       (metricColumnNumber >= metricFile->getNumberOfColumns())) {
       throw BrainModelAlgorithmException("Metric/Shape file column number is invalid for border named " + borderName);
   }
   if ((borderStartNode < 0) ||
       (borderStartNode >= numNodes)) {
         throw BrainModelAlgorithmException("Starting node is invalid for border named " + borderName);
   }
   if ((borderEndNode < 0) ||
       (borderEndNode >= numNodes)) {
         throw BrainModelAlgorithmException("Ending node is invalid for border named " + borderName);
   }
   if (borderStartNode == borderEndNode) {
      throw BrainModelAlgorithmException("Starting and ending node are the same for border named " + borderName);
   }
   if (theROI.getNodeSelected(borderStartNode) == false) {
      throw BrainModelAlgorithmException("Starting node is not in the ROI for border named " + borderName);
   }
   if (theROI.getNodeSelected(borderEndNode) == false) {
      throw BrainModelAlgorithmException("Ending node is not in the ROI for border named " + borderName);
   }
   
   
   //
   // Get a topology helper
   //
   const TopologyFile* tf = bms->getTopologyFile();
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   // Use a deque (double ended queue) for tracking nodes in path 
   //
   std::deque<int> borderPathNodes;
   borderPathNodes.push_back(borderStartNode);

   //
   // Keep track of nodes added to ROI (used for fast indexing)
   //
   std::vector<int> nodeVisitedFlags(numNodes, 0);
   nodeVisitedFlags[borderStartNode] = 1;
   
   //
   // Coordinate of ending node
   //
   const float* endXYZ = cf->getCoordinate(borderEndNode);
   
   //
   // Loop until path from starting to ending nodes is found
   //
   int currentNode = borderStartNode;
   int lastDilatedNode = -1;
   bool done = false;
   while (done == false) {
      //
      // Get neighbors of current node
      //
      int numNeighbors = 0;
      const int* neighbors = th->getNodeNeighbors(currentNode, numNeighbors);
      
      //
      // see if any neighbor is the ending node
      //
      for (int i = 0; i < numNeighbors; i++) {
         if (neighbors[i] == borderEndNode) {
            borderPathNodes.push_back(borderEndNode);
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
         switch (mode) {
            case MODE_FOLLOW_MOST_NEGATIVE:
               nextNodeMetricValue = std::numeric_limits<float>::max();
               break;
            case MODE_FOLLOW_MOST_POSITIVE:
               nextNodeMetricValue = -std::numeric_limits<float>::max();
               break;
         }
         
         //
         // Loop through neighbors
         //
         for (int i = 0; i < numNeighbors; i++) {
            const int neighborNode = neighbors[i];
            
            //
            // Is node in the ROI
            //
            if (theROI.getNodeSelected(neighborNode) &&
                (nodeVisitedFlags[neighborNode] == 0)) {
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
         
         //
         // If no neighbor in ROI is closer to end node than current node
         //
         if (nextNode < 0) {
            //
            // Move to neighbor that is closest to end node even if that
            // means moving away from the end
            //
            float nearestDistance = std::numeric_limits<float>::max();
            for (int i = 0; i < numNeighbors; i++) {
               const int neighborNode = neighbors[i];
               
               //
               // Is node in the ROI
               //
               if (theROI.getNodeSelected(neighborNode) &&
                   (nodeVisitedFlags[neighborNode] == 0)) {
                  //
                  // Is distance from neighbor than other neighbors
                  //
                  const float distance = 
                     MathUtilities::distanceSquared3D(cf->getCoordinate(neighborNode), endXYZ);
                  if (distance < nearestDistance) {
                     nearestDistance = distance;
                     nextNode = neighborNode;
                  }
               }
            }
         }
         
         if (nextNode >= 0) {
            currentNode = nextNode;
            borderPathNodes.push_back(currentNode);
            lastDilatedNode = -1;
            nodeVisitedFlags[currentNode] = 1;
         }
/*
         else if (borderPathNodes.size() > 1) {
            //
            // Since cannot move closer to end, back up one node 
            // and remove current node from ROI
            //
            theROI.setNodeSelected(currentNode, false);
            if (currentNode != borderPathNodes.back()) {
               std::cout << "ERROR back() is not current node." << std::endl;
            }
            borderPathNodes.pop_back();
            currentNode = borderPathNodes.back();
         }
*/
         else if (currentNode != lastDilatedNode) {
            //
            // Put neighbors in ROI and try again
            //
            theROI.dilateAroundNode(bms, currentNode);
            lastDilatedNode = currentNode;
         }
         else {
            throw BrainModelAlgorithmException(
               "Create Metric/Shape Border: unable to complete path from node "
               + QString::number(borderStartNode)
               + " to node "
               + QString::number(borderEndNode)
               + ".  Stuck at node "
               + QString::number(currentNode)
               + " for border named " + borderName);
         }
      }
   }
   
   //
   // Name and add nodes to the border
   //
   border.clearLinks();
   const int numNodesInPath = static_cast<int>(borderPathNodes.size());
   for (int j = 0; j < numNodesInPath; j++) {
      border.addBorderLink(cf->getCoordinate(borderPathNodes[j]));
   }
   border.setName(borderName);
}

/* 18apr2008
void 
BrainModelSurfaceROICreateBorderUsingMetricShape::executeOperation() throw (BrainModelAlgorithmException)
{
   if (borderName.isEmpty()) {
      throw BrainModelAlgorithmException("Name for border is empty.");
   }

   const int numNodesInROI = theROI->getNumberOfNodesSelected();
   
   if (numNodesInROI == 1) {
      throw BrainModelAlgorithmException("There is only one node, the starting node, in the ROI "
                                         " for border named " + borderName);
   }
   
   const int numNodes = bms->getNumberOfNodes();
   const CoordinateFile* cf = bms->getCoordinateFile();
   
   border.clearLinks();
   
   //
   // Check Inputs
   //
   if (metricFile == NULL) {
      throw BrainModelAlgorithmException("Metric/Shape file is invalid for border named " + borderName);
   }
   if ((metricColumnNumber < 0) ||
       (metricColumnNumber >= metricFile->getNumberOfColumns())) {
       throw BrainModelAlgorithmException("Metric/Shape file column number is invalid for border named " + borderName);
   }
   if ((borderStartNode < 0) ||
       (borderStartNode >= numNodes)) {
         throw BrainModelAlgorithmException("Starting node is invalid for border named " + borderName);
   }
   if ((borderEndNode < 0) ||
       (borderEndNode >= numNodes)) {
         throw BrainModelAlgorithmException("Ending node is invalid for border named " + borderName);
   }
   if (borderStartNode == borderEndNode) {
      throw BrainModelAlgorithmException("Starting and ending node are the same for border named " + borderName);
   }
   if (operationSurfaceROI->getNodeSelected(borderStartNode) == false) {
      throw BrainModelAlgorithmException("Starting node is not in the ROI for border named " + borderName);
   }
   if (operationSurfaceROI->getNodeSelected(borderEndNode) == false) {
      throw BrainModelAlgorithmException("Ending node is not in the ROI for border named " + borderName);
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
         switch (mode) {
            case MODE_FOLLOW_MOST_NEGATIVE:
               nextNodeMetricValue = std::numeric_limits<float>::max();
               break;
            case MODE_FOLLOW_MOST_POSITIVE:
               nextNodeMetricValue = -std::numeric_limits<float>::max();
               break;
         }
         
         //
         // Loop through neighbors
         //
         for (int i = 0; i < numNeighbors; i++) {
            const int neighborNode = neighbors[i];
            
            //
            // Is node in the ROI
            //
            if (operationSurfaceROI->getNodeSelected(neighborNode)) {
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
            throw BrainModelAlgorithmException(
               "Create Border: unable to complete path from node "
               + QString::number(borderStartNode)
               + " to node "
               + QString::number(borderEndNode)
               + ".  Stuck at node "
               + QString::number(currentNode)
               + " for border named " + borderName);
         }
      }
   }
   
   //
   // Name the border
   //
   border.setName(borderName);
}
*/

/**
 * get the border that was created by create border mode.
 */
Border 
BrainModelSurfaceROICreateBorderUsingMetricShape::getBorder() const
{
   return border;
}

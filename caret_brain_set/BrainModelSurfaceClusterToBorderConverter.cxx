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

#include "BorderFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceClusterToBorderConverter.h"
#include "BrainSet.h"
#include "BrainSetNodeAttribute.h"
#include "DebugControl.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
BrainModelSurfaceClusterToBorderConverter::BrainModelSurfaceClusterToBorderConverter(
                                                            BrainSet* bs,
                                                            BrainModelSurface* bmsIn,
                                                            TopologyFile* tfIn,
                                                            const QString& borderNameIn,
                                                            const std::vector<bool>& inputNodeClusterFlagIn,
                                                            const bool projectTheBordersFlagIn)
   : BrainModelAlgorithm(bs)
{
   bms = bmsIn;
   topologyFile = tfIn;
   borderName = borderNameIn;
   numberOfBordersCreated = 0;
   
   inputNodeClusterFlag = inputNodeClusterFlagIn;
   projectTheBordersFlag = projectTheBordersFlagIn;
}

/**
 * destructor.
 */
BrainModelSurfaceClusterToBorderConverter::~BrainModelSurfaceClusterToBorderConverter()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceClusterToBorderConverter::execute() throw (BrainModelAlgorithmException)
{
   //executeOriginal();
   
   executeNew();
}

/**
 * execute original algorithm that misses some clusters.
 */
void 
BrainModelSurfaceClusterToBorderConverter::executeOriginal() throw (BrainModelAlgorithmException)
{
   //
   // Make sure nodes are selected
   //
   if (std::count(inputNodeClusterFlag.begin(), inputNodeClusterFlag.end(), true) <= 0) {
      throw BrainModelAlgorithmException("No nodes are selected.h");
   }
   
   //
   // Check inputs
   //
   if (bms == NULL) {
      throw BrainModelAlgorithmException("Surface is invalid (NULL).");
   }
   
   //
   // Check surface for nodes
   //
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes <= 0) {
      throw BrainModelAlgorithmException("Surface has no nodes.");
   }
   if (static_cast<int>(inputNodeClusterFlag.size()) != numNodes) {
      throw BrainModelAlgorithmException("Cluster node flags size is not equal to number of nodes.");
   }
   
   //
   // Check surface for topology
   //
   if (topologyFile == NULL) {
      topologyFile = bms->getTopologyFile();
      if (topologyFile == NULL) {
         throw BrainModelAlgorithmException("Surface has no topology.h");
      }
   }
   const int numTriangles = topologyFile->getNumberOfTiles();

   //
   // Index of surface
   const int modelIndex = bms->getBrainModelIndex();
   if (modelIndex < 0) {
      throw BrainModelAlgorithmException("Surface has invalid brain model index.");
   }
   
   //
   // Get the topology helper with node info sorted
   //
   topologyHelper = (TopologyHelper*)topologyFile->getTopologyHelper(false, true, true);
   
   //
   // Keep track of each node's status
   //
   nodeStatus.resize(numNodes);
   std::fill(nodeStatus.begin(), nodeStatus.end(), STATUS_OUTSIDE);
   
   //
   // Find nodes of interest (are metric but have neighbors not metric - boundary of cluster)
   //
   for (int i = 0; i < numNodes; i++) {
      //
      // Is node potentially in a cluster
      //
      if (inputNodeClusterFlag[i]) {
      //if (bsnc->getNodeColorSource(modelIndex, i) == colorSource) {
         //
         // Flag as inside the cluster
         //
         nodeStatus[i] = STATUS_INSIDE;
         
         //
         // Get neighbors
         //
         int numNeighbors = 0;
         const int* neighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
         for (int j = 0; j < numNeighbors; j++) {
            //
            // Is it on the boundary of a cluster
            //
            if (inputNodeClusterFlag[neighbors[j]] == false) {
            //if (bsnc->getNodeColorSource(modelIndex, neighbors[j]) != colorSource) {
               nodeStatus[i] = STATUS_BOUNDARY;
               break;
            }
         }
      }
   }

/*   
   //
   // Highlight nodes
   //
   brainSet->clearNodeHighlightSymbols();   
   for (int i = 0; i < numNodes; i++) {
      if (nodeStatus[i] == STATUS_BOUNDARY) {
         BrainSetNodeAttribute* bna = brainSet->getNodeAttributes(i);
         //if (i == startingNode) {
         //   bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_REMOTE);
         //}
         //else {
            bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
         //}
      }
   }
*/
   
   for (int startNode = 0; startNode < numNodes; startNode++) {
      //
      // Clean up cluster nodes with zero or one neighbor
      //
      cleanupClusterNodes();

      //
      // Find a starting node (3 consecutive nodes that each have two boundary neighbors)
      //
      int startingNode = -1;
      int firstNode = -1;
      int secondNode = -1;
      for (int i = startNode; i < numNodes; i++) {
         startNode = i;
         if (nodeStatus[i] == STATUS_BOUNDARY) {
            //
            // Has only two neighbors ?
            //
            if (getBoundaryNeighborCount(i) == 2) {
               if (DebugControl::getDebugOn()) {
                  std::cout << "Examining Node: " << i << std::endl;
               }
               
               //
               // Each of these two neighbors has only two neighbors
               //
               bool valid = true;
               int numNeighbors = 0;
               const int* neighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
               for (int j = 0; j < numNeighbors; j++) {
                  if (nodeStatus[neighbors[j]] == STATUS_BOUNDARY) {
                     if (getBoundaryNeighborCount(neighbors[j]) != 2) {
                        valid = false;
                        break;
                     }
                     else {
                        if (firstNode < 0) {
                           firstNode = neighbors[j];
                        }
                        else if (secondNode < 0) {
                           secondNode = neighbors[j];
                        }
                     }
                  }
               }
               
               if (valid) {
                  startingNode = i;
                  break;
               }
               else {
                  firstNode = -1;   // 5/29/07
                  secondNode = -1;  // 5/29/07
               }
            }
         }
      }
      
      //
      // If no nodes found find a triangle with 3 nodes that are boundary nodes
      // This code added 5/30/2007
      //
      if (secondNode < 0) {
         for (int i = 0; i < numTriangles; i++) {
            const int* nodes = topologyFile->getTile(i);
            if ((nodeStatus[nodes[0]] == STATUS_BOUNDARY) &&
                (nodeStatus[nodes[1]] == STATUS_BOUNDARY) &&
                (nodeStatus[nodes[2]] == STATUS_BOUNDARY)) {
               startingNode = nodes[0];
               firstNode    = nodes[1];
               secondNode   = nodes[2];
               break;
            }
         }
         
         if (secondNode < 0) {
            for (int i = 0; i < numTriangles; i++) {
               const int* nodes = topologyFile->getTile(i);
               if ((nodeStatus[nodes[0]] == STATUS_BOUNDARY) &&
                   (nodeStatus[nodes[1]] == STATUS_BOUNDARY)) {
                  startingNode = nodes[0];
                  firstNode    = nodes[1];
                  secondNode   = firstNode;
                  break;
               }
               if ((nodeStatus[nodes[1]] == STATUS_BOUNDARY) &&
                   (nodeStatus[nodes[2]] == STATUS_BOUNDARY)) {
                  startingNode = nodes[1];
                  firstNode    = nodes[2];
                  secondNode   = firstNode;
                  break;
               }
               if ((nodeStatus[nodes[0]] == STATUS_BOUNDARY) &&
                   (nodeStatus[nodes[2]] == STATUS_BOUNDARY)) {
                  startingNode = nodes[2];
                  firstNode    = nodes[0];
                  secondNode   = firstNode;
                  break;
               }
            }
         }
      }
      
      //
      // For searching
      //
      int node = -1;
      int previousNode = -1;
      
      if ((firstNode >= 0) && (secondNode >= 0)) {
         //
         // Get neighors for first node
         //
         int numNeighbors = 0;
         const int* neighbors = topologyHelper->getNodeNeighbors(firstNode, numNeighbors);
         
         //
         // Neighbors should be CCW
         //
         for (int i = 0; i < numNeighbors; i++) {
            //
            // Find the starting node
            //
            if (neighbors[i] == startingNode) {
               //
               // Get next neighbor after starting node
               //
               int next = i + 1;
               if (next >= numNeighbors) {
                  next = 0;
               }
               const int nextNeighbor = neighbors[next];
               if (nodeStatus[nextNeighbor] == STATUS_OUTSIDE) {
                  previousNode = firstNode;
                  node = startingNode;
               }
               else {
                  previousNode = startingNode;
                  node = firstNode;
               }
               break;
            }
         }
      }
      
      if (DebugControl::getDebugOn()) {
         std::cout << "Start Node: " << previousNode << std::endl;
         std::cout << "2nd Node:   " << node << std::endl;
      }
      
      //
      // Want to move clockwise around the boundary, note that nodes sorted counter-clockwise
      //
      if ((previousNode >= 0) && (node >= 0)) {
         const int originalNode = previousNode;
         
         std::vector<int> borderNodes;
         
         borderNodes.push_back(previousNode);

         while ((node != originalNode) && (node >= 0)) {
            borderNodes.push_back(node);
            
            //
            // Get neighbors for first node
            //
            int numNeighbors = 0;
            const int* neighbors = topologyHelper->getNodeNeighbors(node, numNeighbors);
            
            bool foundNextNode = false;
            for (int j = 0; j < numNeighbors; j++) {
               //
               // Found previous node that is part of border
               //
               if (neighbors[j] == previousNode) {
                  //
                  // Find next clockwise neighbor that is a boundary node
                  //
                  int prev = j - 1;
                  for (int iters = 0; iters < numNeighbors; iters++) {
                     if (prev < 0) {
                        prev = numNeighbors - 1;
                     }
                     
                     //
                     // Is this neighbor a boundary node?
                     //
                     if (nodeStatus[neighbors[prev]] == STATUS_BOUNDARY) {
                        //
                        // Use it and continue
                        //
                        previousNode = node;
                        node = neighbors[prev];
                        foundNextNode = true;
                        break;  // get out of for (int iters... loop
                     }
                     else {
                        prev--;
                     }
                  }
                  break;  // get out of (for int j... loop
               }
            }
            
            //
            // Failure to find next node ?
            //
            if (foundNextNode == false) {
               node = -1;
            }
         }

         //
         // Were node's found
         //
         if (borderNodes.size() > 2) {
            //
            // If last node is connected to first, add first node so border closes.
            //
            const int lastNode = borderNodes[borderNodes.size() - 1];
            int numNeighbors = 0;
            const int* neighbors = topologyHelper->getNodeNeighbors(borderNodes[0], numNeighbors);
            for (int nn = 0; nn < numNeighbors; nn++) {
               if (neighbors[nn] == lastNode) {
                  borderNodes.push_back(borderNodes[0]);
               }
            }
            
            //
            // Create a border
            //
            CoordinateFile* cf = bms->getCoordinateFile();
            Border border;
            border.setName(borderName);
            if (DebugControl::getDebugOn()) {
               std::cout << "Border Nodes: " << std::endl;
            }
            for (unsigned int m = 0; m < borderNodes.size(); m++) {
               nodeStatus[borderNodes[m]] = STATUS_WAS_BOUNDARY;
               if (DebugControl::getDebugOn()) {
                  std::cout << "   " << borderNodes[m] << std::endl;
               }
               const float* xyz = cf->getCoordinate(borderNodes[m]);
               border.addBorderLink(xyz);
            }
            if (DebugControl::getDebugOn()) {
               std::cout << std::endl;
            }
            
            //
            // Get the border set
            //
            BrainModelBorderSet* bmbs = brainSet->getBorderSet();
            
            //
            // Add the border to the border set
            //
            BorderFile bf;
            bf.addBorder(border);
            bmbs->copyBordersFromBorderFile(bms, &bf);
            
            numberOfBordersCreated++;
         } // if (borderNodes.empty()...
      }   
   } // for (int startNode = 0...
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Borders Created: " << numberOfBordersCreated << std::endl;
   }
}

/**
 * execute new algorithm.
 */
void 
BrainModelSurfaceClusterToBorderConverter::executeNew() throw (BrainModelAlgorithmException)
{
   //
   // Make sure nodes are selected
   //
   if (std::count(inputNodeClusterFlag.begin(), inputNodeClusterFlag.end(), true) <= 0) {
      throw BrainModelAlgorithmException("No nodes are selected.h");
   }
   
   //
   // Check inputs
   //
   if (bms == NULL) {
      throw BrainModelAlgorithmException("Surface is invalid (NULL).");
   }
   
   //
   // Check surface for nodes
   //
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes <= 0) {
      throw BrainModelAlgorithmException("Surface has no nodes.");
   }
   if (static_cast<int>(inputNodeClusterFlag.size()) != numNodes) {
      throw BrainModelAlgorithmException("Cluster node flags size is not equal to number of nodes.");
   }
   
   //
   // Check surface for topology
   //
   if (topologyFile == NULL) {
      topologyFile = bms->getTopologyFile();
      if (topologyFile == NULL) {
         throw BrainModelAlgorithmException("Surface has no topology.h");
      }
   }
   const int numTriangles = topologyFile->getNumberOfTiles();

   //
   // Index of surface
   const int modelIndex = bms->getBrainModelIndex();
   if (modelIndex < 0) {
      throw BrainModelAlgorithmException("Surface has invalid brain model index.");
   }
   
   //
   // Get the topology helper with node info sorted
   //
   topologyHelper = (TopologyHelper*)topologyFile->getTopologyHelper(false, true, true);
   
   //
   // Keep track of each node's status
   //
   nodeStatus.resize(numNodes);
   std::fill(nodeStatus.begin(), nodeStatus.end(), STATUS_OUTSIDE);
   
   //
   // Find nodes of interest (are metric but have neighbors not metric - boundary of cluster)
   //
   for (int i = 0; i < numNodes; i++) {
      //
      // Is node potentially in a cluster
      //
      if (inputNodeClusterFlag[i]) {
      //if (bsnc->getNodeColorSource(modelIndex, i) == colorSource) {
         //
         // Flag as inside the cluster
         //
         nodeStatus[i] = STATUS_INSIDE;
         
         //
         // Get neighbors
         //
         int numNeighbors = 0;
         const int* neighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
         for (int j = 0; j < numNeighbors; j++) {
            //
            // Is it on the boundary of a cluster
            //
            if (inputNodeClusterFlag[neighbors[j]] == false) {
            //if (bsnc->getNodeColorSource(modelIndex, neighbors[j]) != colorSource) {
               nodeStatus[i] = STATUS_BOUNDARY;
               break;
            }
         }
      }
   }

/*   
   //
   // Highlight nodes
   //
   brainSet->clearNodeHighlightSymbols();   
   for (int i = 0; i < numNodes; i++) {
      if (nodeStatus[i] == STATUS_BOUNDARY) {
         BrainSetNodeAttribute* bna = brainSet->getNodeAttributes(i);
         //if (i == startingNode) {
         //   bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_REMOTE);
         //}
         //else {
            bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
         //}
      }
   }
*/
   //
   // Get the border set and number of borders before creating new borders
   //
   BrainModelBorderSet* bmbs = brainSet->getBorderSet();
   const int originalNumberOfBorders = bmbs->getNumberOfBorders();
   
   int startingTriangleNumber = 0;
   
   bool doneSearching = false;
   while (doneSearching == false) {
      //
      // Clean up cluster nodes with zero or one neighbor
      //
      cleanupClusterNodes();

      int previousNode = -1;
      int node = -1;
      
      for (int i = startingTriangleNumber; i < numTriangles; i++) {
         const int* nodes = topologyFile->getTile(i);
         if ((nodeStatus[nodes[0]] == STATUS_BOUNDARY) &&
             (nodeStatus[nodes[1]] == STATUS_BOUNDARY)) {
            previousNode = nodes[0];
            node         = nodes[1];
            startingTriangleNumber = i;
            break;
         }
         if ((nodeStatus[nodes[1]] == STATUS_BOUNDARY) &&
             (nodeStatus[nodes[2]] == STATUS_BOUNDARY)) {
            previousNode = nodes[1];
            node         = nodes[2];
            startingTriangleNumber = i;
            break;
         }
         if ((nodeStatus[nodes[0]] == STATUS_BOUNDARY) &&
             (nodeStatus[nodes[2]] == STATUS_BOUNDARY)) {
            previousNode = nodes[2];
            node         = nodes[0];
            startingTriangleNumber = i;
            break;
         }
      }
      
      if (DebugControl::getDebugOn()) {
         std::cout << "Start Node: " << previousNode << std::endl;
         std::cout << "2nd Node:   " << node << std::endl;
      }
      
      //
      // Want to move clockwise around the boundary, note that nodes sorted counter-clockwise
      //
      doneSearching = true;
      if ((previousNode >= 0) && (node >= 0)) {
         doneSearching = false;
         
         const int originalNode = previousNode;
         
         std::vector<int> borderNodes;
         
         borderNodes.push_back(previousNode);

         while ((node != originalNode) && (node >= 0)) {
            borderNodes.push_back(node);
            
            //
            // Get neighbors for first node
            //
            int numNeighbors = 0;
            const int* neighbors = topologyHelper->getNodeNeighbors(node, numNeighbors);
            
            bool foundNextNode = false;
            for (int j = 0; j < numNeighbors; j++) {
               //
               // Found previous node that is part of border
               //
               if (neighbors[j] == previousNode) {
                  //
                  // Find next clockwise neighbor that is a boundary node
                  //
                  int prev = j - 1;
                  for (int iters = 0; iters < numNeighbors; iters++) {
                     if (prev < 0) {
                        prev = numNeighbors - 1;
                     }
                     
                     //
                     // Is this neighbor a boundary node?
                     //
                     if (nodeStatus[neighbors[prev]] == STATUS_BOUNDARY) {
                        //
                        // Use it and continue
                        //
                        previousNode = node;
                        node = neighbors[prev];
                        foundNextNode = true;
                        break;  // get out of for (int iters... loop
                     }
                     else {
                        prev--;
                     }
                  }
                  break;  // get out of (for int j... loop
               }
            }
            
            //
            // Failure to find next node ?
            //
            if (foundNextNode == false) {
               node = -1;
            }
         }

         //
         // Were node's found
         //
         if (borderNodes.size() > 2) {
            //
            // If last node is connected to first, add first node so border closes.
            //
            const int lastNode = borderNodes[borderNodes.size() - 1];
            int numNeighbors = 0;
            const int* neighbors = topologyHelper->getNodeNeighbors(borderNodes[0], numNeighbors);
            for (int nn = 0; nn < numNeighbors; nn++) {
               if (neighbors[nn] == lastNode) {
                  borderNodes.push_back(borderNodes[0]);
               }
            }
            
            //
            // Create a border
            //
            CoordinateFile* cf = bms->getCoordinateFile();
            Border border;
            border.setName(borderName);
            if (DebugControl::getDebugOn()) {
               std::cout << "Border Nodes: " << std::endl;
            }
            for (unsigned int m = 0; m < borderNodes.size(); m++) {
               nodeStatus[borderNodes[m]] = STATUS_WAS_BOUNDARY;
               if (DebugControl::getDebugOn()) {
                  std::cout << "   " << borderNodes[m] << std::endl;
               }
               const float* xyz = cf->getCoordinate(borderNodes[m]);
               border.addBorderLink(xyz);
            }
            if (DebugControl::getDebugOn()) {
               std::cout << std::endl;
            }
            
            //
            // Add the border to the border set
            //
            BorderFile bf;
            bf.addBorder(border);
            bmbs->copyBordersFromBorderFile(bms, &bf);
            
            numberOfBordersCreated++;
         } // if (borderNodes.empty()...
      }   
   } // while (doneSearching == false...
   
   //
   // Optionally project the newly created borders
   //
   if (projectTheBordersFlag) {
      bmbs->projectBorders(bms,
                           false, // NOT barycentric so project to nearest node
                           originalNumberOfBorders);  // first border to project
   }

   if (DebugControl::getDebugOn()) {
      std::cout << "Borders Created: " << numberOfBordersCreated << std::endl;
   }
}      

/**
 * cleanup cluster nodes such as those with one or zero neighbors.
 */
void 
BrainModelSurfaceClusterToBorderConverter::cleanupClusterNodes()
{
   const int numNodes = bms->getNumberOfNodes();
   
   //
   // Check for nodes that might be in cluster but should be ignored.
   // May need to check multiple times if nodes are removed.
   //
   bool checkAgain = true;
   while (checkAgain) {
      checkAgain = false;
      
      for (int i = 0; i < numNodes; i++) {
         if (nodeStatus[i] == STATUS_BOUNDARY) {
            //
            // Count its neighbors that are part of cluster boundary
            //
            const int validCount = getBoundaryNeighborCount(i);
            
            //
            // If node has 0 or 1 neighbors in cluster, do not use it
            //
            if (validCount <= 1) {
               nodeStatus[i] = STATUS_OUTSIDE;
               checkAgain = true;
            }
         }
      }
   }
}      

/**
 * get the boundary neighor count for a node.
 */
int 
BrainModelSurfaceClusterToBorderConverter::getBoundaryNeighborCount(const int nodeNumber) const
{
   int validCount = 0;
   int numNeighbors = 0;
   const int* neighbors = topologyHelper->getNodeNeighbors(nodeNumber, numNeighbors);
   for (int j = 0; j < numNeighbors; j++) {
      if (nodeStatus[neighbors[j]] == STATUS_BOUNDARY) {
         validCount++;
      }
   }
   
   return validCount;
}      


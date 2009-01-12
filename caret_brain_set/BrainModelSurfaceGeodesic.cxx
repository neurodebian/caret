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

#include <QGlobalStatic>
#include <QDateTime>

#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <iostream>
#include <limits>
#include <sstream>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceGeodesic.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "DisplaySettingsGeodesicDistance.h"
#include "GeodesicDistanceFile.h"
#include "MetricFile.h"
#include "StringUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.  If "nodeInROIIn" is NULL, then all nodes are used.
 * If a column number is "-3", that column is not created.
 * If a column number is "-2" a new column is created.
 */
BrainModelSurfaceGeodesic::BrainModelSurfaceGeodesic(
                                          BrainSet* bs,
                                          const BrainModelSurface* surfaceIn,
                                          MetricFile* metricFileIn,
                                          const int metricFileColumnIn,
                                          const QString& metricColumnNameIn,
                                          GeodesicDistanceFile* geodesicDistanceFileIn,
                                          const int geodesicDistanceFileColumnIn,
                                          const QString& geodesicDistanceColumnNameIn,
                                          const int rootNodeNumberIn,
                                          const BrainModelSurfaceROINodeSelection* surfaceROIIn)
   : BrainModelAlgorithm(bs),
     surface(surfaceIn),
     metricFile(metricFileIn),
     metricFileColumn(metricFileColumnIn),
     metricColumnName(metricColumnNameIn),
     geodesicDistanceFile(geodesicDistanceFileIn),
     geodesicDistanceFileColumn(geodesicDistanceFileColumnIn),
     geodesicDistanceColumnName(geodesicDistanceColumnNameIn),
     rootNodeNumber(rootNodeNumberIn)
{
   
   const int numNodes = surface->getNumberOfNodes();
   nodeInROI.resize(surface->getNumberOfNodes(), false);
   if (surfaceROIIn != NULL) {
      for (int i = 0; i < numNodes; i++) {
         if (surfaceROIIn->getNodeSelected(i)) {
            nodeInROI[i] = true;
         }
      }
   }
   else {
      std::fill(nodeInROI.begin(), nodeInROI.end(), true);
   }
}

/**
 * Destructor.
 */
BrainModelSurfaceGeodesic::~BrainModelSurfaceGeodesic()
{
}
                           
/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceGeodesic::execute() throw (BrainModelAlgorithmException)
{
   QTime timer;
   timer.start();
   
   //
   // Check Inputs
   //
   if (surface == NULL) {
      throw BrainModelAlgorithmException("Invalid surface.");
   }
   if (geodesicDistanceFile == NULL) {
      throw BrainModelAlgorithmException("Invalid geodesic distance file.");
   }
   const int numNodes = surface->getNumberOfNodes();
   if ((rootNodeNumber < 0) || (rootNodeNumber >= numNodes)) {
      throw BrainModelAlgorithmException("Invalid starting node number.");
   }
   const TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw BrainModelAlgorithmException("Surface has no topology.");
   }
   
   //
   // Get the coordinate file
   //
   const CoordinateFile* cf = surface->getCoordinateFile();
   
   //
   // Get the topology helper for node neighbors
   //
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);

   //
   // Reserve space to avoid reallocations
   //   
   allVertices.reserve(numNodes);

   //
   // Create the vertex and edge information
   //
   for (int i = 0; i < numNodes; i++) {
      allVertices.push_back(Vertex(i));
      Vertex& v = allVertices[i];      
      
      //
      // Make sure node is in region of interest
      //
      if (nodeInROI[i]) {
         //
         // Only consider node if it has neighbors
         //
         const int numNeighs = th->getNodeNumberOfNeighbors(i);
         if (numNeighs > 0) {
            //
            // Determine distance to each of node's neighbors
            //
            std::vector<int> neighs;
            th->getNodeNeighbors(i, neighs);
            for (int j = 0; j < numNeighs; j++) {
               if (nodeInROI[neighs[j]]) {
                  v.neighbors.push_back(neighs[j]);
                  v.neighborDistance.push_back(cf->getDistanceBetweenCoordinates(i, neighs[j]));
               }
            }
            //
            // Only use node if it has neighbors
            //
            if (v.neighbors.empty() == false) {
               v.numNeighbors = static_cast<int>(v.neighbors.size());
               if (i == rootNodeNumber) {
                  //
                  // Root node goes into the tree
                  //
                  addToTreeVertices(i);
               }
               else {
                  //
                  // All other nodes go into unvisited list
                  //
                  addToUnvisitedVertices(i);
               }
            }
            else if (i == rootNodeNumber) {
               throw BrainModelAlgorithmException("Root node has no neighbors");
            }
         }
      }
      else {
         //
         // Node not in ROI
         //
         v.distance = -1;
      }
   }
   
   //
   // Initialize the tree with the root node and put the roots neighbors in the active set
   //
   Vertex& rootVertex = allVertices[rootNodeNumber];
   rootVertex.distance = 0;
   rootVertex.pathNode = rootNodeNumber;
   for (int i = 0; i < rootVertex.numNeighbors; i++) {
      const int n = rootVertex.neighbors[i];
      Vertex& v = allVertices[n];
      v.pathNode = rootNodeNumber;
      v.distance = rootVertex.neighborDistance[i];
      removeFromUnvisitedVertices(n);
      addToActiveVertices(n);
   } 
   
   //
   // Do until no active nodes 
   //
   while (activeVertices.empty() == false) {
      //
      // Get the active node closest to the root node
      //
      SortedVertexIterator iter = activeVertices.begin();
      SortedVertex sv = *iter;
      Vertex* v = sv.v;
      
      //
      // Keeps track of nodes that need to be moved to the active list
      //
      std::vector<int> moveToActiveVertices;
      
      //
      // Check the node's neighbors
      //
      for (int i = 0; i < v->numNeighbors; i++) {
         //
         // Distance from root to active node to neighbor
         //
         const float dist = v->distance + v->neighborDistance[i];
         //
         // Is this a shorter path than neighbor's current path distance
         //
         const int neighNodeNumber = v->neighbors[i];
         Vertex& neighborVertex = allVertices[neighNodeNumber];
         if (dist < neighborVertex.distance) {
            //
            // Update neighbors distance and path and move it to the active list
            //
            neighborVertex.distance = dist;
            neighborVertex.pathNode = v->nodeNumber;
            moveToActiveVertices.push_back(neighNodeNumber);
         }
      }
      
      //
      // Remove active node from active list and move to the tree
      //
      removeFromActiveVertices(v->nodeNumber);
      addToTreeVertices(v->nodeNumber);
      
      //
      // Move modified nodes to active list
      //
      for (int i = 0; i < static_cast<int>(moveToActiveVertices.size()); i++) {
         const int vn = moveToActiveVertices[i];
         Vertex& v = allVertices[vn];
         const int vertexNumber = v.nodeNumber;
         switch (v.location) {
            case Vertex::VERTEX_LOCATION_UNKNOWN:
               throw BrainModelAlgorithmException("PROGRAM ERROR: VERTEX_LOCATION_UNKNOWN");
               break;
            case Vertex::VERTEX_LOCATION_ACTIVE_LIST:
               removeFromActiveVertices(vertexNumber);
               break;
            case Vertex::VERTEX_LOCATION_TREE:
               removeFromTreeVertices(vertexNumber);
               break;
            case Vertex::VERTEX_LOCATION_UNVISITED:
               removeFromUnvisitedVertices(vertexNumber);
               break;
         }
         addToActiveVertices(vertexNumber);
      }
   }
   
   //
   // column comment
   //
   QString columnComment("Geodesic for node: ");
   columnComment.append(StringUtilities::fromNumber(rootNodeNumber));

   //
   // Add column onto the metric file if needed
   //
   if (metricFile != NULL) {
      if (metricFileColumn >= -2) {
         if (metricFile->getNumberOfColumns() == 0) {
            metricFile->setNumberOfNodesAndColumns(numNodes, 1);   
            metricFileColumn = 0;
         }
         else if ((metricFileColumn == -2) || 
                  (metricFileColumn >= metricFile->getNumberOfColumns())) {
            metricFile->addColumns(1);
            metricFileColumn = metricFile->getNumberOfColumns() - 1;
         }
      }
   }
   
   //
   // Add column onto the geodesic distance file if needed
   //
   if (geodesicDistanceFileColumn >= -2) {
      if (geodesicDistanceFile->getNumberOfColumns() == 0) {
         geodesicDistanceFile->setNumberOfNodesAndColumns(numNodes, 1);   
         geodesicDistanceFileColumn = 0;
      }
      else if ((geodesicDistanceFileColumn == -2) || 
               (geodesicDistanceFileColumn >= geodesicDistanceFile->getNumberOfColumns())) {
         geodesicDistanceFile->addColumns(1);
         geodesicDistanceFileColumn = geodesicDistanceFile->getNumberOfColumns() - 1;
      }
      
      //
      // Set root node number
      //
      geodesicDistanceFile->setRootNode(geodesicDistanceFileColumn, rootNodeNumber);
   }
   
   //
   // Name the metric column
   //
   if (metricFile != NULL) {
      if (metricFileColumn >= 0) {
         if (metricColumnName.isEmpty()) {
            std::ostringstream str1;
            str1 << "Geo Dist Node: " << rootNodeNumber;
            metricColumnName = str1.str().c_str();
         }
         metricFile->setColumnName(metricFileColumn, metricColumnName);
         metricFile->setColumnComment(metricFileColumn, columnComment);
      }
   }
   
   //
   // Name the geodesic column
   //
   if (geodesicDistanceFileColumn >= 0) {
      if (geodesicDistanceColumnName.isEmpty()) {
         std::ostringstream str1;
         str1 << "Geo Dist Node: " << rootNodeNumber;
         geodesicDistanceColumnName = str1.str().c_str();
      }
      geodesicDistanceFile->setColumnName(geodesicDistanceFileColumn, 
                                          geodesicDistanceColumnName);
      geodesicDistanceFile->setColumnComment(geodesicDistanceFileColumn,
                                             columnComment);
   }
   
   //
   // Copy the geodesic distances and nodes to the metric file columns
   //
   for (int i = 0; i < numNodes; i++) {
      if (metricFile != NULL) {
         if (metricFileColumn >= 0) {
            metricFile->setValue(i, metricFileColumn, allVertices[i].distance);
         }
      }
      if (geodesicDistanceFileColumn >= 0) {
         geodesicDistanceFile->setNodeParent(i, geodesicDistanceFileColumn, 
                                             allVertices[i].pathNode);
         geodesicDistanceFile->setNodeParentDistance(i, geodesicDistanceFileColumn, 
                                             allVertices[i].distance);
      }
   }

   brainSet->getDisplaySettingsGeodesicDistance()->update();
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Time to compute geodesic was: "
                << timer.elapsed() * 0.001 
                << std::endl;
   }
}

/**
 * place vertex in active list.
 */
void
BrainModelSurfaceGeodesic::addToActiveVertices(const int vertexNumber)
{
   allVertices[vertexNumber].location = Vertex::VERTEX_LOCATION_ACTIVE_LIST;
   activeVertices.insert(SortedVertex(&allVertices[vertexNumber]));
   if (DebugControl::getDebugOn()) {
      if (vertexNumber == DebugControl::getDebugNodeNumber()) {
         std::cout << "Added " << vertexNumber << " to active vertices." << std::endl;
      }
   }
}

/**
 * remove a vertex from the active list
 */
void
BrainModelSurfaceGeodesic::removeFromActiveVertices(const int vertexNumber) 
                                               throw (BrainModelAlgorithmException)
{
   bool found = false;
   for (SortedVertexIterator iter = activeVertices.begin();
         iter != activeVertices.end(); iter++) {
      if (iter->v->nodeNumber == vertexNumber) {
         activeVertices.erase(iter);
         found = true;
         if (DebugControl::getDebugOn()) {
            if (vertexNumber == DebugControl::getDebugNodeNumber()) {
               std::cout << "Removed " << vertexNumber << " from active vertices." << std::endl;
            }
         }
         break;
      }
   }
   if (found == false) {
      if (DebugControl::getDebugOn()) {
         std::cout << "Active list at time of failure: ";
         for (SortedVertexIterator iter = activeVertices.begin();
            iter != activeVertices.end(); iter++) {
            std::cout << " " << iter->v->nodeNumber;
         }
         std::cout << std::endl;
      }
      throw BrainModelAlgorithmException(
         "PROGRAM ERROR: Failed to remove from activeVertices");
   }
}

/**
 * place vertex in tree.
 */
void
BrainModelSurfaceGeodesic::addToTreeVertices(const int vertexNumber)
{
   allVertices[vertexNumber].location = Vertex::VERTEX_LOCATION_TREE;
   treeVertices.insert(vertexNumber);
   if (DebugControl::getDebugOn()) {
      if (vertexNumber == DebugControl::getDebugNodeNumber()) {
         std::cout << "Added " << vertexNumber << " to tree vertices." << std::endl;
      }
   }
}

/**
 * remove vertex from tree.
 */
void
BrainModelSurfaceGeodesic::removeFromTreeVertices(const int vertexNumber)
{
   treeVertices.erase(vertexNumber);
   if (DebugControl::getDebugOn()) {
      if (vertexNumber == DebugControl::getDebugNodeNumber()) {
         std::cout << "Removed " << vertexNumber << " from tree vertices." << std::endl;
      }
   }
}

/**
 * place vertex in unvisited list.
 */
void
BrainModelSurfaceGeodesic::addToUnvisitedVertices(const int vertexNumber)
{
   allVertices[vertexNumber].location = Vertex::VERTEX_LOCATION_UNVISITED;
   unvisitedVertices.insert(vertexNumber);
   if (DebugControl::getDebugOn()) {
      if (vertexNumber == DebugControl::getDebugNodeNumber()) {
         std::cout << "Added " << vertexNumber << " to unvisited vertices." << std::endl;
      }
   }
}      

/**
 * remove vertex from unvisited.
 */
void
BrainModelSurfaceGeodesic::removeFromUnvisitedVertices(const int vertexNumber)
{
   unvisitedVertices.erase(vertexNumber);
   if (DebugControl::getDebugOn()) {
      if (vertexNumber == DebugControl::getDebugNodeNumber()) {
         std::cout << "Removed " << vertexNumber << " from unvisited vertices." << std::endl;
      }
   }
}

//--------------------------------------------------------------------------------------

/**
 * Constructor.
 */
BrainModelSurfaceGeodesic::Vertex::Vertex(const int nodeNumberIn) 
{ 
   nodeNumber = nodeNumberIn; 
   numNeighbors = 0;
   distance = std::numeric_limits<float>::max();
   pathNode = -1;
   location = VERTEX_LOCATION_UNKNOWN;
}

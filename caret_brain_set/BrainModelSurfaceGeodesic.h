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

#ifndef __BRAIN_MODEL_GEODESIC_H__
#define __BRAIN_MODEL_GEODESIC_H__

#include <set>
#include <vector>

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class BrainModelSurfaceROINodeSelection;
class GeodesicDistanceFile;
class MetricFile;

/// Class for surface geodesic distance computation.  This is performed using
/// Dijkstra's Shortest Path Algorithm with the surface nodes and links.  
/// A root node is provided and for all other nodes in the region of interest,
/// its geodesic distance from the root node and its "parent neighbor" are 
/// added to a geodesic distance file.  The "parent neighbors" can be followed to get
/// the path to the root node.
class BrainModelSurfaceGeodesic : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelSurfaceGeodesic(BrainSet* bs,
                                const BrainModelSurface* surfaceIn,
                                MetricFile* metricFileIn,
                                const int metricFileColumnIn,
                                const QString& metricColumnNameIn,
                                GeodesicDistanceFile* geodesicDistanceFileIn,
                                const int geodesicDistanceFileColumnIn,
                                const QString& geodesicDistanceColumnName,
                                const int rootNodeNumberIn,
                                const BrainModelSurfaceROINodeSelection* surfaceROIIn = NULL);
      
      /// Destructor
      ~BrainModelSurfaceGeodesic();
                                
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// class for storing vertex information
      class Vertex {
         public:
            enum VERTEX_LOCATION {
               VERTEX_LOCATION_UNKNOWN,
               VERTEX_LOCATION_ACTIVE_LIST,
               VERTEX_LOCATION_TREE,
               VERTEX_LOCATION_UNVISITED
            };
            
            /// Constructor
            Vertex(const int nodeNumberIn);
            
            /// less than operator
            bool operator<(const Vertex& v) const { 
               return (distance < v.distance);
            }
               
            /// node node number
            int nodeNumber;
            
            /// neighbors
            std::vector<int> neighbors;
            
            /// distances to neighbors;
            std::vector<float> neighborDistance;
            
            /// number of neighbors
            int numNeighbors;
            
            /// distance from start node to this node
            float distance;
            
            /// previous node in path from start node
            int pathNode;
            
            /// location of this vertex
            VERTEX_LOCATION location;
      };
      
      /// class for sorting vertices by "distance".
      class SortedVertex {
         public:
            /// Constructor
            SortedVertex(Vertex* vin) { v = vin; }
            
            /// less than operator
            bool operator<(const SortedVertex& sv) const {
               return (v->distance < sv.v->distance);
            }
            
            /// pointer to the vertex
            Vertex* v;
      };
      
      typedef std::multiset<SortedVertex>::iterator SortedVertexIterator;
      
      /// place vertex in active list
      void addToActiveVertices(const int vertexNumber);
      
      /// place vertex in tree
      void addToTreeVertices(const int vertexNumber);
      
      /// place vertex in unvisited list
      void addToUnvisitedVertices(const int vertexNumber);
      
      /// remove a vertex from the unvisited list
      void removeFromUnvisitedVertices(const int vertexNumber);
      
      /// remove a vertex from the tree list
      void removeFromTreeVertices(const int vertexNumber);
      
      /// remove a vertex from the active list
      void removeFromActiveVertices(const int vertexNumber) throw (BrainModelAlgorithmException);
      
      /// surface for geodesic 
      const BrainModelSurface* surface;
      
      /// metric file for storing geodesic info
      MetricFile* metricFile;
      
      /// column in metric file
      int metricFileColumn;
      
      /// metric column name
      QString metricColumnName;
      
      /// the geodesic distance file
      GeodesicDistanceFile* geodesicDistanceFile;
      
      /// column in geodesic distance file
      int geodesicDistanceFileColumn;
      
      /// name of geodesic distance column name
      QString geodesicDistanceColumnName;
      
      /// starting node number for geodesic distances
      int rootNodeNumber;
      
      /// nodes that are used for geodesic computation
      std::vector<bool> nodeInROI;
      
      /// nodes not yet visited
      std::set<int> unvisitedVertices;
      
      /// nodes whose parent is in the tree.  Use multiset since it is possible
      /// that nodes may be the same distance from root (distance is the
      /// sorting criterion.
      std::multiset<SortedVertex> activeVertices;
      
      /// nodes that are almost done or done
      std::set<int> treeVertices;
      
      /// all of the node indexed by node number
      std::vector<Vertex> allVertices;
};

#endif // __BRAIN_MODEL_GEODESIC_H__



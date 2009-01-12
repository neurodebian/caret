
#ifndef __BRAIN_MODEL_VOLUME_TOPOLOGY_GRAPH_H__
#define __BRAIN_MODEL_VOLUME_TOPOLOGY_GRAPH_H__

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

#include <ios>
#include <map>
#include <set>
#include <vector>

#include "BrainModelAlgorithm.h"
#include "VoxelIJK.h"

class VolumeFile;

/// create graph of segmentation volume topology
/// This algorithm is based off the paper "Automated Topology Correction
/// for Human Brain Segmentation" by Lin Chen and Gudrun Wagenknecht, 
/// MICCAI 2006, LNCS 4191, pp. 316-323, 2006.  Springer-Verlag.
class BrainModelVolumeTopologyGraph : public BrainModelAlgorithm {
   public:
      // search axis
      enum SEARCH_AXIS {
         // search along X axis
         SEARCH_AXIS_X,
         // search along Y axis
         SEARCH_AXIS_Y,
         // search along Z axis
         SEARCH_AXIS_Z
      };
      
      // voxel neighbor connectivity
      enum VOXEL_NEIGHBOR_CONNECTIVITY {
         // 6-connected
         VOXEL_NEIGHBOR_CONNECTIVITY_6,
         // 18-connected
         VOXEL_NEIGHBOR_CONNECTIVITY_18,
         // 26-connected
         VOXEL_NEIGHBOR_CONNECTIVITY_26
      };
      
      /// Edge in the graph
      class GraphEdge {
         public:
            // constructor
            GraphEdge(const int vertexNumberIn,
                      const int strengthIn) 
               { vertexNumber = vertexNumberIn; strength = strengthIn; }

            // destructor
            ~GraphEdge() { }
            
            // get the vertex number
            int getVertexNumber() const { return vertexNumber; }
            
            // get the strength
            int getStrength() const { return strength; }
            
         protected:
            // the vertex number
            int vertexNumber;
            
            // the strength
            int strength;
      };
      
      /// Vertex in the graph
      class GraphVertex {
         public:
            // constructor
            GraphVertex(const int sliceNumberIn) 
               { sliceNumber = sliceNumberIn; identifier = -1; }
            
            // destructor
            ~GraphVertex() { voxels.clear(); }
            
            /// add a voxel to the vertex
            void addVoxel(const VoxelIJK& v) { voxels.push_back(v); }
            
            /// add connected graph vertex index
            void addConnectedGraphVertex(const int vertexIndex, const int strength) 
               { connectedGraphEdges.push_back(GraphEdge(vertexIndex, strength)); }
               
            /// get number of connected graph edges
            int getNumberOfConnectedGraphEdges() const 
               { return connectedGraphEdges.size(); }
         
            /// get connected graph vertex index
            const GraphEdge* getConnectedGraphEdge(const int indx) const 
               { return &connectedGraphEdges[indx]; }
               
            /// get the number of voxels in the vertex
            int getNumberOfVoxels() const { return voxels.size(); }
            
            /// get a voxel
            const VoxelIJK* getVoxel(const int indx) const { return &voxels[indx]; }

            /// get the slice number
            int getSliceNumber() const { return sliceNumber; }
            
            /// get the identifier
            int getIdentifier() const { return identifier; }
            
            /// set the identifier
            void setIdentifier(const int id) { identifier = id; }
            
            /// get a descriptive name (slice_number and identifier)
            QString getDescriptiveName() const
               { return ("S" + QString::number(sliceNumber) 
                         //+ "I" + QString::number(identifier)
                         + "N" + QString::number(voxels.size())); }
                            
            /// the slice number
            int sliceNumber;
            
            /// voxels in vertex
            std::vector<VoxelIJK> voxels;
            
            /// connected graph edges
            std::vector<GraphEdge> connectedGraphEdges;
            
            /// identifier
            int identifier;
      };
      
      /// cycle in the graph
      class GraphCycle {
         public:
            // constructor
            GraphCycle();
            
            // destructor
            ~GraphCycle();

            // clear the cycle
            void clear();
            
            /// is cycle empty
            bool empty() const { return cycle.empty(); }
            
            // number of graph vertices in cycle
            int getNumberOfGraphVerticesInCycle() const { return cycle.size(); }
            
            // get index of graph vertex in cycle
            int getGraphVertexIndex(const int indx) const { return cycle[indx]; }
            
            // set the cycle
            void set(const std::vector<int>& cycleVerticesIn,
                     const std::vector<int>& cycleSlicesIn);
            
            /// get the handle vertices
            std::vector<int> getHandleVertices() const { return handleVertices; }
            
            /// get the number of voxels that make up the handle
            int getHandleSizeInVoxels() const { return numVoxelsInHandle; }
            
            // set the vertices that form the handle
            void setHandleVertices(const std::vector<int>& handleVerticesIn,
                                   const int numVoxelsInHandleIn);
            
            // equality operator
            bool operator==(const GraphCycle& c) const;
            
            // comparison operator
            bool operator<(const GraphCycle& c) const;
            
            // get the cycle
            std::vector<int> getCycle() const;

         protected:
            /// the cycle
            std::vector<int> cycle;
            
            /// the cycle in sorted order (used for comparisons)
            std::vector<int> cycleSorted;
            
            /// the vertices that form the handle in the cycle
            std::vector<int> handleVertices;
            
            /// number of voxels in the handle
            int numVoxelsInHandle;
      };

      // constructor
      BrainModelVolumeTopologyGraph(BrainSet* bsIn,
                                    const VolumeFile* segmentationVolumeFileIn,
                                    const SEARCH_AXIS searchAxisIn,
                                    const VOXEL_NEIGHBOR_CONNECTIVITY voxelConnectivityIn);
                                    
      // destructor
      ~BrainModelVolumeTopologyGraph();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// get the number of vertices in the graph
      int getNumberOfGraphVertices() const { return graphVertices.size(); }
      
      /// get a vertex in the graph
      GraphVertex* getGraphVertex(const int indx) { return graphVertices[indx]; }
      
      /// get a vertex in the graph (const method)
      const GraphVertex* getGraphVertex(const int indx) const { return graphVertices[indx]; }
      
      /// get number of cycles in graph
      int getNumberOfGraphCycles() const { return graphCycles.size(); }
      
      /// get a cycle in the graph
      GraphCycle* getGraphCycle(const int indx) { return &graphCycles[indx]; }
      
      /// get a cycle in the graph (const method)
      const GraphCycle* getGraphCycle(const int indx) const { return &graphCycles[indx]; }
      
      /// get cycle with smallest vertex (vertex that contains fewest voxels)
      void getGraphCycleWithSmallestVertex(int &cycleIndexOut,
                                           int &vertexIndexOut,
                                           int &numberofVoxelsOut) const;
      
      /// get cycle with smallest handle (handle that contains fewest voxels)
      void getGraphCycleWithSmallestHandle(int &cycleIndexOut,
                                           std::vector<int>& vertexIndicesOut,
                                           int &numberOfVoxelsOut) const;
      
      // print the results
      void printResults() const;
      
      // get the search axis
      SEARCH_AXIS getSearchAxis() const { return searchAxis; }
      
      // create an paint volume file containing the handles
      void createHandlesPaintVolumeFile(VolumeFile& handlesPaintVolumeFile);
      
      // write the graph to a paint volume file
      void writeGraphToPaintVolumeFile(const QString& paintVolumeFileName) const throw (BrainModelAlgorithmException);
      
      // write graph to graphviz file
      void writeGraphVizDotFile(const QString& dotFileName) const throw (BrainModelAlgorithmException);
      
   protected:    
      /// slice neighbor connectivity
      enum SLICE_NEIGHBOR_CONNECTIVITY {
         /// 4-connected
         SLICE_NEIGHBOR_CONNECTIVITY_4,
         /// 8-connected
         SLICE_NEIGHBOR_CONNECTIVITY_8
      };
        
      /// vertex visited status
      enum VISITED_STATUS { 
         NOT_VISITED = 0,
         VISITED = 1
      };
      
      /// A slice of a volume
      class VolumeSlice {
         public:
            // constructor
            VolumeSlice(const int dimIin, const int dimJin);
         
            // destructor
            ~VolumeSlice();
            
            // get indices valid
            bool getIJValid(const int i, const int j) const;
            
            // get dimension I
            int getDimI() const { return dimI; }
            
            // get dimension J
            int getDimJ() const { return dimJ; }
            
            // set a voxel in the slice
            void setVoxel(const SEARCH_AXIS searchAxis, const VoxelIJK& v, const int value);
            
            // get a voxel in the slice
            float getVoxel(const SEARCH_AXIS searchAxis, const VoxelIJK& v) const;
            
            // set a voxel in the slice
            void setVoxel(const int horizIndex, const int vertIndex, const int value);
            
            // get a voxel in the slice
            float getVoxel(const int horizIndex, const int vertIndex) const;
            
            // set all voxels in the slice
            void setAllVoxels(const int value);
         
         protected:
            // get the index
            int getIndex(const SEARCH_AXIS searchAxis, const VoxelIJK& v) const;
            
            // get the index
            int getIndex(const int horizIndex, const int vertIndex) const;
            
            /// the voxels
            int* voxels;
            
            /// dimension I
            int dimI;
            
            /// dimension J
            int dimJ;
      };
      
      // create the vertices in the graph
      void createGraphVertices() throw (BrainModelAlgorithmException);
      
      // create the edges (connections between vertices) in the graph
      void createGraphEdges() throw (BrainModelAlgorithmException);
      
      // search graph for cycles
      void searchGraphForCycles();
      
      // determine the handles
      void determineHandles();
      
      // perform breadth first search
      void breadthFirstSearchForCycles(const int startVertexIndex,
                                       const int searchForVertexIndex,
                                       GraphCycle& cycleOut);
      
      // get voxel slice neighbors
      void getVoxelSliceNeighbors(const VoxelIJK& v,
                             const VolumeSlice& slice,
                             const int searchForValue,
                             std::vector<VoxelIJK>& neighborsOut) const;
      
      // add a slice neighboring voxel if valid and not searched
      void addSliceNeighbor(const VolumeSlice& slice,
                       const int i,
                       const int j,
                       const int k, 
                       const int searchForValue,
                       std::vector<VoxelIJK>& neighborsOut) const;
      
      // get graph vertex connected neighbors
      void getGraphVertexConnectedNeighbors(const VoxelIJK& v,
                                 const bool adjoiningSlicesOnlyFlag,
                                 std::map<int,int>& graphVertexNeighborsInOut) const;
      
      // get graph vertex connected neighbors in next slice only
      void getGraphVertexConnectedNeighborsInNextSlice(const VoxelIJK& v,
                                 std::map<int,int>& graphVertexNeighborsInOut) const;
      
      // add a graph vertex neighbor
      void addGraphVertexNeighbor(const int i,
                                  const int j,
                                  const int k,
                                  std::set<int>& neighborsOut) const;
                             
      // print vertices in the graph
      void printGraphVertices() const;
      
      // print cycles in the graph
      void printGraphCycles() const;
      
      /// adjust IJK for axis being processed
      void ijkForSlice(int& i, int& j, int& k) const;
      
      /// get IJK from loop
      void ijkFromLoop(const int horiz, const int vert, const int slice,
                       int& i, int& j, int& k) const;
      
      /// Get voxel connects to voxels in another vertex.
      bool getVoxelConnectedToGraphVertex(const VoxelIJK& v1,
                                          const int otherGraphVertexIndex) const;
      
      /// input segmentation volume
      const VolumeFile* inputSegmentationVolumeFile;
      
      /// segmentation volume on which topology graph is built
      VolumeFile* segmentationVolumeFile;
      
      /// vertices in the graph
      std::vector<GraphVertex*> graphVertices;
      
      /// cycles in the graph
      std::vector<GraphCycle> graphCycles;
      
      /// the search axis
      SEARCH_AXIS searchAxis;

      /// the voxel connectivity
      VOXEL_NEIGHBOR_CONNECTIVITY volumeConnectivity;
      
      /// the slice connectivity
      SLICE_NEIGHBOR_CONNECTIVITY sliceConnectivity;
      
      // volume where the voxel values are the index of the graph 
      // vertex to which the voxel belongs
      VolumeFile* voxelGraphVertexIndexVolumeFile;
};

#endif // __BRAIN_MODEL_VOLUME_TOPOLOGY_GRAPH_H__

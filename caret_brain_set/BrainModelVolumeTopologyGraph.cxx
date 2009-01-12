
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

#include <QFile>
#include <QTextStream>

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <stack>

#include "BrainModelVolumeTopologyGraph.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelVolumeTopologyGraph::BrainModelVolumeTopologyGraph(
                              BrainSet* bsIn,
                              const VolumeFile* segmentationVolumeFileIn,
                              const SEARCH_AXIS searchAxisIn,
                              const VOXEL_NEIGHBOR_CONNECTIVITY volumeConnectivityIn)
   : BrainModelAlgorithm(bsIn),
     inputSegmentationVolumeFile(segmentationVolumeFileIn),
     searchAxis(searchAxisIn),
     volumeConnectivity(volumeConnectivityIn)
{
   segmentationVolumeFile = NULL;
   voxelGraphVertexIndexVolumeFile = NULL;
}
                              
/**
 * destructor.
 */
BrainModelVolumeTopologyGraph::~BrainModelVolumeTopologyGraph()
{
   const int num = getNumberOfGraphVertices();
   for (int i = 0; i < num; i++) {
      delete graphVertices[i];
      graphVertices[i] = NULL;
   }
   graphVertices.clear();
   
   if (voxelGraphVertexIndexVolumeFile != NULL) {
      delete voxelGraphVertexIndexVolumeFile;
      voxelGraphVertexIndexVolumeFile = NULL;
   }
   
   if (segmentationVolumeFile != NULL) {
      delete segmentationVolumeFile;
      segmentationVolumeFile = NULL;
   }
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeTopologyGraph::execute() throw (BrainModelAlgorithmException)
{
   if (inputSegmentationVolumeFile == NULL) {
      throw BrainModelAlgorithmException("Segmentation Volume is invalid");
   }
   
   //
   // Make a copy of the input segmentation volume file
   //
   segmentationVolumeFile = new VolumeFile(*inputSegmentationVolumeFile);
   
   //
   // Set slice connectivity based opon voxel connectivity
   //
   switch (volumeConnectivity) {
      case VOXEL_NEIGHBOR_CONNECTIVITY_6:
         sliceConnectivity = SLICE_NEIGHBOR_CONNECTIVITY_4;
         break;
      case VOXEL_NEIGHBOR_CONNECTIVITY_18:
         sliceConnectivity = SLICE_NEIGHBOR_CONNECTIVITY_8;
         break;
      case VOXEL_NEIGHBOR_CONNECTIVITY_26:
         sliceConnectivity = SLICE_NEIGHBOR_CONNECTIVITY_8;
         break;
   }
   
   //
   // Create a volume where the voxel values are the index of the graph 
   // vertex to which the voxel belongs
   //
   voxelGraphVertexIndexVolumeFile = new VolumeFile(*segmentationVolumeFile);
   voxelGraphVertexIndexVolumeFile->setAllVoxels(-1.0);

   //
   // Create the vertices in the graph
   //
   createGraphVertices();
   
   //
   // Create the graph edges (connections between vertices)
   //
   createGraphEdges();
   
   //
   // Search for cycles (which are handles)
   //
   searchGraphForCycles();   
   
   //
   // Determine the voxels that form the handle part of the handle 
   //
   determineHandles();
}

/**
 * write the graph to a paint volume file.
 */
void 
BrainModelVolumeTopologyGraph::writeGraphToPaintVolumeFile(const QString& paintVolumeFileName)
                                   const throw (BrainModelAlgorithmException)
{
   VolumeFile paintVolumeFile = *inputSegmentationVolumeFile;
   paintVolumeFile.setVolumeType(VolumeFile::VOLUME_TYPE_PAINT);
   
   const int backgroundValue = paintVolumeFile.addRegionName("???");
   paintVolumeFile.setAllVoxels(backgroundValue);


   const int numGraphVertices = getNumberOfGraphVertices();
   for (int i = 0; i < numGraphVertices; i++) {
      const GraphVertex* vertex = getGraphVertex(i);
      const int numVoxels = vertex->getNumberOfVoxels();
      if (numVoxels > 0) {
         const int paintIndex = paintVolumeFile.addRegionName(vertex->getDescriptiveName());
         for (int i = 0; i < numVoxels; i++) {
            const VoxelIJK* v = vertex->getVoxel(i);
            paintVolumeFile.setVoxel(*v, 0, paintIndex);
         }
      }
   }
   
   try {
      paintVolumeFile.writeFile(paintVolumeFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
}
      
/**
 * write graph to graphvis file.
 */
void 
BrainModelVolumeTopologyGraph::writeGraphVizDotFile(const QString& dotFileName) const
                                               throw (BrainModelAlgorithmException)
{
   QFile file(dotFileName);
   if (file.open(QFile::WriteOnly)) {
      QTextStream stream(&file);

      stream << "graph G {\n";
      
      const int numGraphVertices = getNumberOfGraphVertices();
      for (int i = 0; i < numGraphVertices; i++) {
         const GraphVertex* vertex = getGraphVertex(i);
         const int numConnections = vertex->getNumberOfConnectedGraphEdges();
         for (int m = 0; m < numConnections; m++) {
            const GraphEdge* edge = vertex->getConnectedGraphEdge(m);
            if (i < edge->getVertexNumber()) {
               const GraphVertex* otherVertex = getGraphVertex(edge->getVertexNumber());
               stream << "   "
                      << vertex->getDescriptiveName()
                      << " -- "
                      << otherVertex->getDescriptiveName() 
                      << ";\n";
            }
         }
      }
      
      stream << "}\n";
      
      file.close();
   }
   else {
      throw BrainModelAlgorithmException("Unable to open file "
                                         + dotFileName
                                         + ": "
                                         + file.errorString());
   }
}
   

/**
 * print the results.
 */
void 
BrainModelVolumeTopologyGraph::printResults() const
{
   //
   // Print vertices in graph
   //
   printGraphVertices();
   
   //
   // Print cycles in graph
   //
   printGraphCycles();
}
      
/**
 * create the vertices in the graph.
 */
void 
BrainModelVolumeTopologyGraph::createGraphVertices() throw (BrainModelAlgorithmException)
{
   //
   // Get dimensions of the volume
   //
   int dimensions[3];
   segmentationVolumeFile->getDimensions(dimensions);
   if ((dimensions[0] <= 0) ||
       (dimensions[1] <= 0) ||
       (dimensions[2] <= 0)) {
      throw BrainModelAlgorithmException("At least one volume dimension is zero.");
   }
   
   int sliceLoopEnd   = 0;
   int horizLoopEnd   = 0;
   int vertLoopEnd    = 0;
   
   switch (searchAxis) {
      case SEARCH_AXIS_X:
         sliceLoopEnd  = dimensions[0];
         horizLoopEnd  = dimensions[1];
         vertLoopEnd   = dimensions[2];
         break;
      case SEARCH_AXIS_Y:
         sliceLoopEnd  = dimensions[1];
         horizLoopEnd  = dimensions[0];
         vertLoopEnd   = dimensions[2];
         break;
      case SEARCH_AXIS_Z:
         sliceLoopEnd  = dimensions[2];
         horizLoopEnd  = dimensions[0];
         vertLoopEnd   = dimensions[1];
         break;
      default:
         throw BrainModelAlgorithmException("Invalid search axis none of X/Y/Z.");
         break;
   }
   
   //
   // Keep track of voxels in the slice that have been processed
   //
   VolumeSlice voxelsVisitedSlice(horizLoopEnd, vertLoopEnd);
   
   //
   // Loop through slices
   //
   for (int sliceLoop = 0; sliceLoop < sliceLoopEnd; sliceLoop++) {
      //
      // Initialize all segmentation voxels to unvisited
      //
      for (int horizLoop = 0; horizLoop < horizLoopEnd; horizLoop++) {
         for (int vertLoop = 0; vertLoop < vertLoopEnd; vertLoop++) {
            int i = 0, j = 0, k = 0;
            ijkFromLoop(horizLoop, vertLoop, sliceLoop, i, j, k);
            
            if (segmentationVolumeFile->getVoxel(i, j, k, 0) != 0) {
               voxelsVisitedSlice.setVoxel(horizLoop, vertLoop, NOT_VISITED);
            }
            else {
               voxelsVisitedSlice.setVoxel(horizLoop, vertLoop, VISITED);
            }
         }
      }
      
      //
      // Find connected components in slice, each becomes vertex in graph
      //
      for (int horizLoop = 0; horizLoop < horizLoopEnd; horizLoop++) {
         for (int vertLoop = 0; vertLoop < vertLoopEnd; vertLoop++) {
            int i = 0, j = 0, k = 0;
            ijkFromLoop(horizLoop, vertLoop, sliceLoop, i, j, k);
            
            if (voxelsVisitedSlice.getVoxel(horizLoop, vertLoop) == NOT_VISITED) {
               //
               // Create a vertex in the graph
               //
               GraphVertex* vertex = new GraphVertex(sliceLoop);
               
               //
               // Create a stack for finding connected components
               //
               std::stack<VoxelIJK> stack;
               stack.push(VoxelIJK(i, j, k));
               
               bool done = false;
               while (done == false) {
                  //
                  // Get next voxel to search
                  //
                  const VoxelIJK v = stack.top();
                  stack.pop();
                  
                  //
                  // Is voxel unvisited?
                  //
                  if (voxelsVisitedSlice.getVoxel(searchAxis, v) == NOT_VISITED) {
                     //
                     // Set visited
                     //
                     voxelsVisitedSlice.setVoxel(searchAxis, v, VISITED);
                  
                     //
                     // Add voxel to graph vertex
                     //
                     vertex->addVoxel(v);
                     
                     //
                     // Set graph vertex to which voxel belongs
                     //
                     voxelGraphVertexIndexVolumeFile->setVoxel(v, 0, graphVertices.size());
                     
                     //
                     // Search neighbors
                     //
                     std::vector<VoxelIJK> neighbors;
                     getVoxelSliceNeighbors(v, voxelsVisitedSlice, NOT_VISITED, neighbors);
                     
                     //
                     // Add neighbors to stack
                     //
                     const int numNeigh = static_cast<int>(neighbors.size());
                     for (int m = 0; m < numNeigh; m++) {
                        stack.push(neighbors[m]);
                     }
                  }
                  
                  //
                  // If no more vertices to search, then done
                  //
                  if (stack.empty()) {
                     done = true;
                  }
               }
               
               //
               // Add the vertex to the graph
               //
               const int graphVertexIndex = static_cast<int>(graphVertices.size());
               vertex->setIdentifier(graphVertexIndex);
               graphVertices.push_back(vertex);
            }
         }
      }
   }
}

/**
 * adjust IJK for axis being processed.
 */
void 
BrainModelVolumeTopologyGraph::ijkForSlice(int& i, int& j, int& k) const
{
   int ii = i, jj = j, kk = k;

   switch (searchAxis) {
      case SEARCH_AXIS_X:
         i = jj;
         j = kk;
         k = ii;
         break;
      case SEARCH_AXIS_Y:
         i = ii;
         j = kk;
         k = jj;
         break;
      case SEARCH_AXIS_Z:
         i = ii;
         j = jj;
         k = kk;
         break;
   }
}      

/**
 * get IJK from loop.
 */
void 
BrainModelVolumeTopologyGraph::ijkFromLoop(const int horiz, const int Vert, const int slice,
                                           int& i, int& j, int& k) const
{
   switch (searchAxis) {
      case SEARCH_AXIS_X:
         i = slice;
         j = horiz;
         k = Vert;
         break;
      case SEARCH_AXIS_Y:
         i = horiz;
         j = slice;
         k = Vert;
         break;
      case SEARCH_AXIS_Z:
         i = horiz;
         j = Vert;
         k = slice;
         break;
   }
}
      
/**
 * create the edges (connections) in the graph.
 */
void 
BrainModelVolumeTopologyGraph::createGraphEdges() throw (BrainModelAlgorithmException)
{
   const bool increasingSlicesOnlyFlag = true;
   
   //
   // Loop through the graph vertices
   //
   const int numGraphVertices = getNumberOfGraphVertices();
   for (int m = 0; m < numGraphVertices; m++) {
      //
      // Get the vertex in the graph
      //
      GraphVertex* vertex = getGraphVertex(m);
      
      //
      // Graph vertices to which this graph vertex is connected
      // key is vertex number, value is strength
      //
      std::map<int,int> connectedGraphVertexIndices;
      
      //
      // Loop through the voxels in the graph vertex
      //
      const int numVoxels = vertex->getNumberOfVoxels();
      for (int n = 0; n < numVoxels; n++) {
         //
         // Get the voxel
         //
         const VoxelIJK* voxel = vertex->getVoxel(n);
         
         //
         // Get the connected neighbors
         //
         if (increasingSlicesOnlyFlag) {
            getGraphVertexConnectedNeighborsInNextSlice(*voxel,
                                                        connectedGraphVertexIndices);
         }
         else {
            getGraphVertexConnectedNeighbors(*voxel, 
                                             true, 
                                             connectedGraphVertexIndices);
         }
      }

      //
      // Set the connections
      //
      for (std::map<int,int>::const_iterator iter = connectedGraphVertexIndices.begin();
           iter != connectedGraphVertexIndices.end();
           iter++) {
         const int graphVertexNumber = iter->first;
         const int strength = iter->second;
         if (graphVertexNumber != m) {
            if (vertex->getSliceNumber() == getGraphVertex(graphVertexNumber)->getSliceNumber()) {
               throw BrainModelAlgorithmException(
                  "BrainModelVolumeTopologyGraph ERROR: "
                  "graph vertex connected to another in same slice "
                   + vertex->getSliceNumber());
            }
            vertex->addConnectedGraphVertex(graphVertexNumber, strength);
            
            if (increasingSlicesOnlyFlag) {
               getGraphVertex(graphVertexNumber)->addConnectedGraphVertex(m, strength);
            }
         }
      }
   }
}      

/**
 * get graph vertex connected neighbors in next slice only
 */
void 
BrainModelVolumeTopologyGraph::getGraphVertexConnectedNeighborsInNextSlice(const VoxelIJK& v,
                               std::map<int,int>& graphVertexNeighborsInOut) const
{
   const int i = v.getI();
   const int j = v.getJ();
   const int k = v.getK();
   
   std::set<int> uniqueNeighbors;
   
   switch (volumeConnectivity) {
      case VOXEL_NEIGHBOR_CONNECTIVITY_26:
         switch (searchAxis) {
            case SEARCH_AXIS_X:
               //
               // Add neighbors that share a corner
               //         
               addGraphVertexNeighbor(i + 1, j - 1, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j + 1, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j - 1, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j + 1, k + 1, uniqueNeighbors);
               break;
            case SEARCH_AXIS_Y:
               //
               // Add neighbors that share a corner
               //         
               addGraphVertexNeighbor(i + 1, j + 1, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i - 1, j + 1, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j + 1, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i - 1, j + 1, k + 1, uniqueNeighbors);
               break;
            case SEARCH_AXIS_Z:
               //
               // Add neighbors that share a corner
               //         
               addGraphVertexNeighbor(i - 1, j - 1, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j - 1, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j + 1, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i - 1, j + 1, k + 1, uniqueNeighbors);
               break;
         }
         //
         // NO "break" !!!!!
         // Intentionally fall through to next case to get remaining neighbors
         //
      case VOXEL_NEIGHBOR_CONNECTIVITY_18:
         //
         // Add neighbors that share an edge
         //
         switch (searchAxis) {
            case SEARCH_AXIS_X:
               // 
               // 
               // slice above
               addGraphVertexNeighbor(i + 1, j, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j - 1, k, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j + 1, k, uniqueNeighbors);
               break;
            case SEARCH_AXIS_Y:
               // 
               // 
               // slice above
               addGraphVertexNeighbor(i - 1, j + 1, k, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j + 1, k, uniqueNeighbors);
               addGraphVertexNeighbor(i, j + 1, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i, j + 1, k + 1, uniqueNeighbors);
               break;
            case SEARCH_AXIS_Z:
               // 
               // slice above
               // 
               addGraphVertexNeighbor(i - 1, j, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i, j - 1, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i, j + 1, k + 1, uniqueNeighbors);
               break;
         }
         //
         // NO "break" !!!!!
         // Intentionally fall through to next case to get remaining neighbors
         //
      case VOXEL_NEIGHBOR_CONNECTIVITY_6:
         //
         // Add neighbors that share a face
         //
         switch (searchAxis) {
            case SEARCH_AXIS_X:
               //
               // slice next
               //
               addGraphVertexNeighbor(i + 1, j, k, uniqueNeighbors);
               break;
            case SEARCH_AXIS_Y:
               //
               // slice next
               //
               addGraphVertexNeighbor(i, j + 1, k, uniqueNeighbors);
               break;
            case SEARCH_AXIS_Z:
               //
               // slice next
               //
               addGraphVertexNeighbor(i, j, k + 1, uniqueNeighbors);
               break;
         }
         break;
   }
   
   for (std::set<int>::const_iterator iter = uniqueNeighbors.begin();
        iter != uniqueNeighbors.end();
        iter++) {
      const int vertexNumber = *iter;
      if (graphVertexNeighborsInOut.find(vertexNumber) == graphVertexNeighborsInOut.end()) {
         graphVertexNeighborsInOut[vertexNumber] = 1;
      }
      else {
         graphVertexNeighborsInOut[vertexNumber]++;
      }
   }
}                                   

/**
 * get graph vertex connected neighbors.
 */
void 
BrainModelVolumeTopologyGraph::getGraphVertexConnectedNeighbors(const VoxelIJK& v,
                               const bool adjoiningSlicesOnlyFlag,
                               std::map<int,int>& graphVertexNeighborsInOut) const
{
   const int i = v.getI();
   const int j = v.getJ();
   const int k = v.getK();
   
   std::set<int> uniqueNeighbors;
   
   switch (volumeConnectivity) {
      case VOXEL_NEIGHBOR_CONNECTIVITY_26:
         //
         // Add neighbors that share a corner
         //         
         addGraphVertexNeighbor(i - 1, j - 1, k - 1, uniqueNeighbors);
         addGraphVertexNeighbor(i + 1, j - 1, k - 1, uniqueNeighbors);
         addGraphVertexNeighbor(i + 1, j + 1, k - 1, uniqueNeighbors);
         addGraphVertexNeighbor(i - 1, j + 1, k - 1, uniqueNeighbors);
         addGraphVertexNeighbor(i - 1, j - 1, k + 1, uniqueNeighbors);
         addGraphVertexNeighbor(i + 1, j - 1, k + 1, uniqueNeighbors);
         addGraphVertexNeighbor(i + 1, j + 1, k + 1, uniqueNeighbors);
         addGraphVertexNeighbor(i - 1, j + 1, k + 1, uniqueNeighbors);
         //
         // NO "break" !!!!!
         // Intentionally fall through to next case to get remaining neighbors
         //
      case VOXEL_NEIGHBOR_CONNECTIVITY_18:
         //
         // Add neighbors that share an edge
         //
         switch (searchAxis) {
            case SEARCH_AXIS_X:
               //
               // Same slice
               //
               if (adjoiningSlicesOnlyFlag == false) {
                  addGraphVertexNeighbor(i, j - 1, k - 1, uniqueNeighbors);
                  addGraphVertexNeighbor(i, j - 1, k + 1, uniqueNeighbors);
                  addGraphVertexNeighbor(i, j + 1, k + 1, uniqueNeighbors);
                  addGraphVertexNeighbor(i, j + 1, k - 1, uniqueNeighbors);
               }
               //
               // Slice below
               //
               addGraphVertexNeighbor(i - 1, j, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i - 1, j, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i - 1, j - 1, k, uniqueNeighbors);
               addGraphVertexNeighbor(i - 1, j + 1, k, uniqueNeighbors);
               // 
               // 
               // slice above
               addGraphVertexNeighbor(i + 1, j, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j - 1, k, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j + 1, k, uniqueNeighbors);
               break;
            case SEARCH_AXIS_Y:
               //
               // Same slice
               //
               if (adjoiningSlicesOnlyFlag == false) {
                  addGraphVertexNeighbor(i - 1, j, k - 1, uniqueNeighbors);
                  addGraphVertexNeighbor(i + 1, j, k - 1, uniqueNeighbors);
                  addGraphVertexNeighbor(i + 1, j, k + 1, uniqueNeighbors);
                  addGraphVertexNeighbor(i - 1, j, k + 1, uniqueNeighbors);
               }
               //
               // Slice below
               //
               addGraphVertexNeighbor(i - 1, j - 1, k, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j - 1, k, uniqueNeighbors);
               addGraphVertexNeighbor(i, j - 1, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i, j - 1, k + 1, uniqueNeighbors);
               // 
               // 
               // slice above
               addGraphVertexNeighbor(i - 1, j + 1, k, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j + 1, k, uniqueNeighbors);
               addGraphVertexNeighbor(i, j + 1, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i, j + 1, k + 1, uniqueNeighbors);
               break;
            case SEARCH_AXIS_Z:
               //
               // Same slice
               //
               if (adjoiningSlicesOnlyFlag == false) {
                  addGraphVertexNeighbor(i - 1, j - 1, k, uniqueNeighbors);
                  addGraphVertexNeighbor(i + 1, j - 1, k, uniqueNeighbors);
                  addGraphVertexNeighbor(i + 1, j + 1, k, uniqueNeighbors);
                  addGraphVertexNeighbor(i - 1, j + 1, k, uniqueNeighbors);
               }
               //
               // Slice below
               //
               addGraphVertexNeighbor(i - 1, j, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i, j - 1, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i, j + 1, k - 1, uniqueNeighbors);
               // 
               // slice above
               // 
               addGraphVertexNeighbor(i - 1, j, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i, j - 1, k + 1, uniqueNeighbors);
               addGraphVertexNeighbor(i, j + 1, k + 1, uniqueNeighbors);
               break;
         }
         //
         // NO "break" !!!!!
         // Intentionally fall through to next case to get remaining neighbors
         //
      case VOXEL_NEIGHBOR_CONNECTIVITY_6:
         //
         // Add neighbors that share a face
         //
         switch (searchAxis) {
            case SEARCH_AXIS_X:
               if (adjoiningSlicesOnlyFlag == false) {
                  //
                  // left/right
                  //
                  addGraphVertexNeighbor(i, j - 1, k, uniqueNeighbors);
                  addGraphVertexNeighbor(i, j + 1, k, uniqueNeighbors);
                  //
                  // down/up
                  //
                  addGraphVertexNeighbor(i, j, k - 1, uniqueNeighbors);
                  addGraphVertexNeighbor(i, j, k + 1, uniqueNeighbors);
               }
               //
               // slice prev/next
               //
               addGraphVertexNeighbor(i - 1, j, k, uniqueNeighbors);
               addGraphVertexNeighbor(i + 1, j, k, uniqueNeighbors);
               break;
            case SEARCH_AXIS_Y:
               if (adjoiningSlicesOnlyFlag == false) {
                  //
                  // left/right
                  //
                  addGraphVertexNeighbor(i - 1, j, k, uniqueNeighbors);
                  addGraphVertexNeighbor(i + 1, j, k, uniqueNeighbors);
                  //
                  // down/up
                  //
                  addGraphVertexNeighbor(i, j, k - 1, uniqueNeighbors);
                  addGraphVertexNeighbor(i, j, k + 1, uniqueNeighbors);
               }
               //
               // slice prev/next
               //
               addGraphVertexNeighbor(i, j - 1, k, uniqueNeighbors);
               addGraphVertexNeighbor(i, j + 1, k, uniqueNeighbors);
               break;
            case SEARCH_AXIS_Z:
               if (adjoiningSlicesOnlyFlag == false) {
                  //
                  // left/right
                  //
                  addGraphVertexNeighbor(i - 1, j, k, uniqueNeighbors);
                  addGraphVertexNeighbor(i + 1, j, k, uniqueNeighbors);
                  //
                  // down/up
                  //
                  addGraphVertexNeighbor(i, j - 1, k, uniqueNeighbors);
                  addGraphVertexNeighbor(i, j + 1, k, uniqueNeighbors);
               }
               //
               // slice prev/next
               //
               addGraphVertexNeighbor(i, j, k - 1, uniqueNeighbors);
               addGraphVertexNeighbor(i, j, k + 1, uniqueNeighbors);
               break;
         }
         break;
   }
   
   for (std::set<int>::const_iterator iter = uniqueNeighbors.begin();
        iter != uniqueNeighbors.end();
        iter++) {
      const int vertexNumber = *iter;
      if (graphVertexNeighborsInOut.find(vertexNumber) == graphVertexNeighborsInOut.end()) {
         graphVertexNeighborsInOut[vertexNumber] = 1;
      }
      else {
         graphVertexNeighborsInOut[vertexNumber]++;
      }
   }
}                                   

/**
 * add a graph vertex neighbor.
 */
void 
BrainModelVolumeTopologyGraph::addGraphVertexNeighbor(const int i,
                                                      const int j,
                                                      const int k,
                                           std::set<int>& neighborsOut) const
{
   if (voxelGraphVertexIndexVolumeFile->getVoxelIndexValid(i, j, k)) {
      const int graphVertexNumber = 
         static_cast<int>(voxelGraphVertexIndexVolumeFile->getVoxel(i, j, k, 0));
      if (graphVertexNumber >= 0) {
         neighborsOut.insert(graphVertexNumber);
      }
   }
}                             

/**
 * determine the handles.
 */
void 
BrainModelVolumeTopologyGraph::determineHandles()
{
   const int numCycles = getNumberOfGraphCycles();
   for (int i = 0; i < numCycles; i++) {
      GraphCycle* cycle = getGraphCycle(i);
      
      //
      // Get the vertices in the cycle
      //
      std::vector<int> cycleVertices = cycle->getCycle();
      const int numVertices = static_cast<int>(cycleVertices.size());
      if (numVertices >= 4) {
         //
         // Find a "handleSize" sequence of vertices that contains the 
         // smallest number of voxels
         //
         const int handleSize = (numVertices - 2) / 2;
         int minHandleVoxels = std::numeric_limits<int>::max();
         std::vector<int> handleVertices;
         for (int j = 0; j < numVertices; j++) {
            int ctr = 0;
            int indx = j;
            int numVoxels = 0;
            std::vector<int> vertexSequence;
            while (ctr < handleSize) {
               int vertexIndex = cycleVertices[indx];
               numVoxels += getGraphVertex(vertexIndex)->getNumberOfVoxels();
               vertexSequence.push_back(vertexIndex);
               
               ctr++;
               indx++;
               if (indx >= numVertices) {
                  indx = 0;
               }
            }
            
            if (numVoxels < minHandleVoxels) {
               handleVertices = vertexSequence;
               minHandleVoxels = numVoxels;
            }
         }
      
         cycle->setHandleVertices(handleVertices, minHandleVoxels);
      }
   }
}
      
/**
 * search graph for cycles.
 */
void 
BrainModelVolumeTopologyGraph::searchGraphForCycles()
{
   graphCycles.clear();
   
   //
   // Loop through the graph vertices
   //
   const int numGraphVertices = getNumberOfGraphVertices();
   for (int m = 0; m < numGraphVertices; m++) {
      //
      // Get the vertex in the graph
      //
      const GraphVertex* vertex = getGraphVertex(m);
      const int mySliceNumber = vertex->getSliceNumber();
      
      //
      // Only search using slices "above" this vertex
      //
      for (int n = 0; n < vertex->getNumberOfConnectedGraphEdges(); n++) {
         //
         // Get the connected edge
         //
         const GraphEdge* edge = vertex->getConnectedGraphEdge(n);
         const int neighborIndex = edge->getVertexNumber();
         const GraphVertex* neighborVertex = getGraphVertex(neighborIndex);
         if (neighborVertex->getSliceNumber() > mySliceNumber) {
            GraphCycle cycle;
            breadthFirstSearchForCycles(neighborIndex,
                                        vertex->getIdentifier(),
                                        cycle);
                               
            if (cycle.empty() == false) {
               graphCycles.push_back(cycle);

/*               
               std::cout << "Cycle ("
                         << m << " "
                         << vertex->getIdentifier() << " "
                         << neighborIndex << ") ";
               const std::vector<int> cycleVertexIndices = cycle.getCycle();
               for (unsigned int p = 0; p < cycleVertexIndices.size(); p++) {
                  std::cout << cycleVertexIndices[p] << " ";
               }
               std::cout << std::endl;
*/
            }
         }
      }
   }
   
   //
   // Sort the cycles and remove duplicates
   //
   std::sort(graphCycles.begin(), graphCycles.end());
   std::vector<GraphCycle>::iterator iter = std::unique(graphCycles.begin(), graphCycles.end());
   graphCycles.erase(iter, graphCycles.end());
}      

/**
 * perform breadth first search for cycles in graph
 */
void 
BrainModelVolumeTopologyGraph::breadthFirstSearchForCycles(const int startVertexIndex,
                                                           const int searchForVertexIndex,
                                                           GraphCycle& cycleOut)
{
   cycleOut.clear();
   
   //
   // Track path
   //
   const int numGraphVertices = getNumberOfGraphVertices();
   std::vector<int> vertexParentIndex(numGraphVertices, -1);
   std::vector<int> visited(numGraphVertices, 0);
   
   //
   // Indices of vertices that need to be searched
   //
   std::queue<int> queue;
   
   //
   // Add start vertex to the queue
   //
   queue.push(startVertexIndex);
   vertexParentIndex[startVertexIndex] = searchForVertexIndex;
    
   //
   // While there are vertices to search
   //
   while (queue.empty() == false) {
      //
      // Get a vertex index from the queue
      //
      const int vertexIndex = queue.front();
      queue.pop();
      
      //
      // Mark vertex visited
      //
      visited[vertexIndex] = 1;
      
      //
      // Is this what we are looking for?
      //
      if (vertexIndex == searchForVertexIndex) {
         //
         // Found target
         //
         std::vector<int> cycleVertexIndices;
         cycleVertexIndices.push_back(vertexIndex);
         int parentIndex = vertexParentIndex[vertexIndex];
         while ((parentIndex >= 0) && (parentIndex != searchForVertexIndex)) {
            cycleVertexIndices.push_back(parentIndex);
            parentIndex = vertexParentIndex[parentIndex];
         }
         std::vector<int> sliceNumbers;
         for (unsigned int i = 0; i < cycleVertexIndices.size(); i++) {
            sliceNumbers.push_back(getGraphVertex(cycleVertexIndices[i])->getSliceNumber());
         }
         cycleOut.set(cycleVertexIndices, sliceNumbers);
         return;
      }
      else {
         //
         // Get the vertex
         //
         const GraphVertex* graphVertex = getGraphVertex(vertexIndex);
         
         //
         // Get connection edges
         //
         const int numConnectedGraphEdges = graphVertex->getNumberOfConnectedGraphEdges();
         for (int i = 0; i < numConnectedGraphEdges; i++) {
            //
            // Get index of a connected graph vertex
            //
            const GraphEdge* edge = graphVertex->getConnectedGraphEdge(i);
            const int connVertexIndex = edge->getVertexNumber();
            
            //
            // If vertex has NOT been visited
            //
            if (visited[connVertexIndex] == 0) {
               //
               // We are working with the starting vertex, do not
               // move to its parent which is the search for vertex
               //
               bool useIt = true;
               if (vertexIndex == startVertexIndex) {
                  if (connVertexIndex == searchForVertexIndex) {
                     useIt = false;
                  }
               }
               if (useIt) {
                  //
                  // Set the parent for the connected vertex and place it in queue
                  //
                  vertexParentIndex[connVertexIndex] = vertexIndex;
                  queue.push(connVertexIndex);
               }
            }
         }
      }
   }
}                              

/**
 * get voxel neighbors.
 */
void 
BrainModelVolumeTopologyGraph::getVoxelSliceNeighbors(const VoxelIJK& v,
                                                 const VolumeSlice& slice,
                                                 const int searchForValue,
                                                 std::vector<VoxelIJK>& neighborsOut) const
{
   neighborsOut.clear();

   const int i = v.getI();
   const int j = v.getJ();
   const int k = v.getK();
   
   switch (searchAxis) {
      case SEARCH_AXIS_X:
         switch (sliceConnectivity) {
            case SLICE_NEIGHBOR_CONNECTIVITY_8:
               //
               // Add neighbors that share a corner
               //
               addSliceNeighbor(slice, i, j + 1, k + 1, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i, j + 1, k - 1, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i, j - 1, k - 1, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i, j - 1, k + 1, searchForValue, neighborsOut);
               //
               // Intentionally fall through to get the inclusive 4-connected neighbors
               //
            case SLICE_NEIGHBOR_CONNECTIVITY_4:
               //
               // Add neighbors that share an edge
               //
               addSliceNeighbor(slice, i, j, k + 1, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i, j, k - 1, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i, j + 1, k, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i, j - 1, k, searchForValue, neighborsOut);
               break;
         }
         break;
      case SEARCH_AXIS_Y:
         switch (sliceConnectivity) {
            case SLICE_NEIGHBOR_CONNECTIVITY_8:
               //
               // Add neighbors that share a corner
               //
               addSliceNeighbor(slice, i + 1, j, k + 1, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i - 1, j, k + 1, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i - 1, j, k - 1, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i + 1, j, k - 1, searchForValue, neighborsOut);
               //
               // Intentionally fall through to get the inclusive 4-connected neighbors
               //
            case SLICE_NEIGHBOR_CONNECTIVITY_4:
               //
               // Add neighbors that share an edge
               //
               addSliceNeighbor(slice, i + 1, j, k, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i - 1, j, k, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i, j, k + 1, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i, j, k - 1, searchForValue, neighborsOut);
               break;
         }
         break;
      case SEARCH_AXIS_Z:
         switch (sliceConnectivity) {
            case SLICE_NEIGHBOR_CONNECTIVITY_8:
               //
               // Add neighbors that share a corner
               //
               addSliceNeighbor(slice, i + 1, j + 1, k, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i - 1, j + 1, k, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i - 1, j - 1, k, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i + 1, j - 1, k, searchForValue, neighborsOut);
               //
               // Intentionally fall through to get the inclusive 4-connected neighbors
               //
            case SLICE_NEIGHBOR_CONNECTIVITY_4:
               //
               // Add neighbors that share an edge
               //
               addSliceNeighbor(slice, i + 1, j, k, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i - 1, j, k, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i, j + 1, k, searchForValue, neighborsOut);
               addSliceNeighbor(slice, i, j - 1, k, searchForValue, neighborsOut);
               break;
         }
         break;
   }
}                             

void
BrainModelVolumeTopologyGraph::addSliceNeighbor(const VolumeSlice& slice,
                                           const int ii,
                                           const int jj,
                                           const int kk, 
                                           const int searchForValue,
                                           std::vector<VoxelIJK>& neighborsOut) const
{
   int i = ii, j = jj, k = kk;
   ijkForSlice(i, j, k);
   if (slice.getIJValid(i, j)) {
      if (slice.getVoxel(i, j) == searchForValue) {
         neighborsOut.push_back(VoxelIJK(ii, jj, kk));
      }
   }
}

/**
 * print vertices in the graph.
 */
void 
BrainModelVolumeTopologyGraph::printGraphVertices() const
{
   const int numGraphVertices = getNumberOfGraphVertices();
   std::cout << "There are " << numGraphVertices << " vertices in the graph." << std::endl;
   for (int i = 0; i < numGraphVertices; i++) {
      const GraphVertex* vertex = getGraphVertex(i);
      std::cout << "Vertex-Index/Slice/Num-Voxels: " 
                << i << ", "
                << vertex->getSliceNumber() << ", "
                << vertex->getNumberOfVoxels()
                << std::endl;
                
      std::cout << "   Connections: ";
      const int numConnections = vertex->getNumberOfConnectedGraphEdges();
      for (int m = 0; m < numConnections; m++) {
         const GraphEdge* edge = vertex->getConnectedGraphEdge(m);
         std::cout << edge->getVertexNumber() << "("
                   << edge->getStrength() << ") ";
      }
      std::cout << std::endl;
   }
}

/**
 * print cycles in the graph.
 */
void 
BrainModelVolumeTopologyGraph::printGraphCycles() const
{
   const int numGraphCycles = getNumberOfGraphCycles();
   std::cout << "There are " << numGraphCycles << " cycles in the graph." << std::endl;
   for (int i = 0; i < numGraphCycles; i++) {
      const GraphCycle* cycle = getGraphCycle(i);
      std::cout << "Cycle " << i << " Slices: ";
      const int numGraphVerticesInCycle = cycle->getNumberOfGraphVerticesInCycle();
      bool needNewlineFlag = false;
      for (int j = 0; j < numGraphVerticesInCycle; j++) {
         const int graphVertexIndex = cycle->getGraphVertexIndex(j);
         const GraphVertex* vertex = getGraphVertex(graphVertexIndex);
         std::cout << vertex->getSliceNumber() 
                   << "(" << vertex->getNumberOfVoxels() << ") ";
         if ((j > 0) && ((j % 7) == 0)) {
            std::cout << std::endl;
            if (j < (numGraphVerticesInCycle - 1)) {
               std::cout << "      ";
            }
            needNewlineFlag = false;
         }
         else {
            needNewlineFlag = true;
         }
      }
      if (needNewlineFlag) {
         std::cout << std::endl;
      }
      
      const std::vector<int> handleVertices = cycle->getHandleVertices();
      const int numInHandle = static_cast<int>(handleVertices.size());
      if (numInHandle > 0) {
         std::cout << "   Handle: ";
         for (int m = 0; m < numInHandle; m++) {
            const int graphVertexIndex = handleVertices[m];
            const GraphVertex* vertex = getGraphVertex(graphVertexIndex);
            std::cout << vertex->getSliceNumber() 
                      << "(" << vertex->getNumberOfVoxels() << ") ";
         }
         std::cout << std::endl;
      }
   }
}      

/**
 * get cycle with smallest handle (handle that contains fewest voxels).
 */
void 
BrainModelVolumeTopologyGraph::getGraphCycleWithSmallestHandle(int &cycleIndexOut,
                                                       std::vector<int>& vertexIndicesOut,
                                                       int &numberOfVoxelsOut) const
{
   cycleIndexOut = -1;
   vertexIndicesOut.clear();
   numberOfVoxelsOut = std::numeric_limits<int>::max();
      
   const int numGraphCycles = getNumberOfGraphCycles();
   for (int i = 0; i < numGraphCycles; i++) {
      const GraphCycle* cycle = getGraphCycle(i);
      if (cycle->getHandleSizeInVoxels() < numberOfVoxelsOut) {
         numberOfVoxelsOut = cycle->getHandleSizeInVoxels();
         cycleIndexOut = i;
         vertexIndicesOut = cycle->getHandleVertices();
      }
   }
}      

/**
 * get cycle with smallest vertex (contains fewest voxels).
 */
void 
BrainModelVolumeTopologyGraph::getGraphCycleWithSmallestVertex(int &cycleIndexOut,
                                                               int &vertexIndexOut,
                                                      int &numberofVoxelsOut) const
{
   cycleIndexOut = -1;
   vertexIndexOut = -1;
   numberofVoxelsOut = std::numeric_limits<int>::max();
      
   const int numGraphCycles = getNumberOfGraphCycles();
   for (int i = 0; i < numGraphCycles; i++) {
      const GraphCycle* cycle = getGraphCycle(i);
      const int numGraphVerticesInCycle = cycle->getNumberOfGraphVerticesInCycle();
      for (int j = 0; j < numGraphVerticesInCycle; j++) {
         const int graphVertexIndex = cycle->getGraphVertexIndex(j);
         const GraphVertex* vertex = getGraphVertex(graphVertexIndex);
         const int numVoxels = vertex->getNumberOfVoxels();
         if (numVoxels < numberofVoxelsOut) {
            numberofVoxelsOut = numVoxels;
            cycleIndexOut = i;
            vertexIndexOut = graphVertexIndex;
         }
      }
   }
}      

/**
 * create an paint volume file containing the handles.
 */
void 
BrainModelVolumeTopologyGraph::createHandlesPaintVolumeFile(VolumeFile& handlesPaintVolumeFile)
{
   int dim[3];
   segmentationVolumeFile->getDimensions(dim);
   float space[3], org[3];
   segmentationVolumeFile->getSpacing(space);
   segmentationVolumeFile->getOrigin(org);
   VolumeFile::ORIENTATION orient[3];
   segmentationVolumeFile->getOrientation(orient);
   handlesPaintVolumeFile.initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT,
                                         dim,
                                         orient,
                                         org,
                                         space,
                                         true,
                                         true);
   handlesPaintVolumeFile.setVolumeType(VolumeFile::VOLUME_TYPE_PAINT);
   
   handlesPaintVolumeFile.addRegionName("???");
   
   const int numGraphCycles = getNumberOfGraphCycles();
   for (int i = 0; i < numGraphCycles; i++) {
      const GraphCycle* cycle = getGraphCycle(i);
      const std::vector<int> handlesVertices = cycle->getHandleVertices();
      const int numGraphVerticesInHandle = static_cast<int>(handlesVertices.size());
      
      QString regionName("Handle_" + QString::number(i));
      switch (searchAxis) {
         case SEARCH_AXIS_X:
            regionName += "_X_";
            break;
         case SEARCH_AXIS_Y:
            regionName += "_Y_";
            break;
         case SEARCH_AXIS_Z:
            regionName += "_Z_";
            break;
      }
      int minSliceNumber = std::numeric_limits<int>::max();
      int maxSliceNumber = std::numeric_limits<int>::min();
      for (int j = 0; j < numGraphVerticesInHandle; j++) {
         const int graphVertexIndex = handlesVertices[j];
         const GraphVertex* vertex = getGraphVertex(graphVertexIndex);
         minSliceNumber = std::min(minSliceNumber, vertex->getSliceNumber());
         maxSliceNumber = std::max(maxSliceNumber, vertex->getSliceNumber());
      }
      regionName += (QString::number(minSliceNumber)
                     + "_"
                     + QString::number(maxSliceNumber));
      const int regionIndex = handlesPaintVolumeFile.addRegionName(regionName);
      
      //
      // Set paint voxels corresponding to handle
      //
      for (int j = 0; j < numGraphVerticesInHandle; j++) {
         const int graphVertexIndex = handlesVertices[j];
         const GraphVertex* vertex = getGraphVertex(graphVertexIndex);
         const int numVoxels = vertex->getNumberOfVoxels();
         for (int n = 0; n < numVoxels; n++) {
            const VoxelIJK* v = vertex->getVoxel(n);
            handlesPaintVolumeFile.setVoxel(*v, 0, regionIndex);
         }
      }
   }   
}
      
/**
 * Get voxel connects to voxels in another vertex.
 */
bool 
BrainModelVolumeTopologyGraph::getVoxelConnectedToGraphVertex(const VoxelIJK& v1,
                                                        const int otherGraphVertexIndex) const
{
  
   const GraphVertex* vertex = getGraphVertex(otherGraphVertexIndex);
   const int numVoxels = vertex->getNumberOfVoxels();
   for (int n = 0; n < numVoxels; n++) {
      const VoxelIJK* v2 = vertex->getVoxel(n);
      //
      // Voxels indices cannot differ by more than one if immediate neighbor
      //
      const int di = std::abs(v1.getI() - v2->getI());
      if (di > 1) continue;
      const int dj = std::abs(v1.getJ() - v2->getJ());
      if (dj > 1) continue;
      const int dk = std::abs(v1.getK() - v2->getK());
      if (dk > 1) continue;
      const int dSum = di + dj + dk;
      
      switch (volumeConnectivity) {
         case VOXEL_NEIGHBOR_CONNECTIVITY_6:
            if (dSum == 1) return true;
            break;
         case VOXEL_NEIGHBOR_CONNECTIVITY_18:
            if (dSum <= 2) return true;
            break;
         case VOXEL_NEIGHBOR_CONNECTIVITY_26:
            if (dSum <= 3) return true;
            break;
      }
   }

   return false;
}
      
//==========================================================================
//==========================================================================
//==========================================================================
// 
// Volume Slice Class
//

/**
 * constructor.
 */
BrainModelVolumeTopologyGraph::VolumeSlice::VolumeSlice(const int dimIin, const int dimJin)
{
   dimI = dimIin;
   dimJ = dimJin;
   const int num = dimI * dimJ;
   voxels = new int[num];
}

/**
 * destructor.
 */
BrainModelVolumeTopologyGraph::VolumeSlice::~VolumeSlice()
{
   if (voxels != NULL) {
      delete[] voxels;
      voxels = NULL;
   }
}

/**
 * get indices valid.
 */
bool 
BrainModelVolumeTopologyGraph::VolumeSlice::getIJValid(const int i, const int j) const
{
   if ((i >= 0) && (i < dimI) &&
       (j >= 0) && (j < dimJ)) {
      return true;
   }
   return false;
}

/**
 * set a voxel in the slice.
 */
void 
BrainModelVolumeTopologyGraph::VolumeSlice::setVoxel(const SEARCH_AXIS searchAxis, 
                                                     const VoxelIJK& v, 
                                                     const int value)
{
   const int indx = getIndex(searchAxis, v);
   voxels[indx] = value;
}

/**
 * get a voxel in the slice.
 */
float 
BrainModelVolumeTopologyGraph::VolumeSlice::getVoxel(const SEARCH_AXIS searchAxis, 
                                                     const VoxelIJK& v) const
{
   const int indx = getIndex(searchAxis, v);
   return voxels[indx];
}

/**
 * set all voxels in the slice.
 */
void 
BrainModelVolumeTopologyGraph::VolumeSlice::setAllVoxels(const int value)
{
   const int num = dimI * dimJ;
   for (int i = 0; i < num; i++) {
      voxels[i] = value;
   }
}

/**
 * set a voxel in the slice.
 */
void 
BrainModelVolumeTopologyGraph::VolumeSlice::setVoxel(const int horizIndex, const int vertIndex, const int value)
{
   const int indx = getIndex(horizIndex, vertIndex);
   voxels[indx] = value;
}

/**
 * get a voxel in the slice.
 */
float 
BrainModelVolumeTopologyGraph::VolumeSlice::getVoxel(const int horizIndex, const int vertIndex) const
{
   const int indx = getIndex(horizIndex, vertIndex);
   return voxels[indx];
}

/**
 * get the index.
 */
int 
BrainModelVolumeTopologyGraph::VolumeSlice::getIndex(const int horizIndex, const int vertIndex) const
{
   const int indx = (dimI * vertIndex) + horizIndex;

/*   
   if ((indx < 0) ||
       (indx >= (dimI * dimJ))) {
      std::cout << "VolumeSlice bad index: " 
                << horizIndex << ", " << vertIndex
                << std::endl;
   }
*/
   return indx;
}
            
/**
 * get the index.
 */
int 
BrainModelVolumeTopologyGraph::VolumeSlice::getIndex(const SEARCH_AXIS searchAxis, 
                                                     const VoxelIJK& v) const
{
   int i = 0; 
   int j = 0;
   switch (searchAxis) {
      case SEARCH_AXIS_X:
         i = v.getJ();
         j = v.getK();
         break;
      case SEARCH_AXIS_Y:
         i = v.getI();
         j = v.getK();
         break;
      case SEARCH_AXIS_Z:
         i = v.getI();
         j = v.getJ();
         break;
   }
   const int indx = (dimI * j) + i;

/*
   if ((indx < 0) ||
       (indx >= (dimI * dimJ))) {
      std::cout << "VolumeSlice bad index: " 
                << i << ", " << j
                << std::endl;
   }
*/   
   return indx;
}

//==========================================================================
//==========================================================================
//==========================================================================
// 
// Cycle class
//

/**
 * constructor.
 */
BrainModelVolumeTopologyGraph::GraphCycle::GraphCycle()
{
}

/**
 * destructor.
 */
BrainModelVolumeTopologyGraph::GraphCycle::~GraphCycle()
{
}

/**
 * clear the cycle.
 */
void 
BrainModelVolumeTopologyGraph::GraphCycle::clear()
{
   cycle.clear();
   cycleSorted.clear();
}

/**
 * set the cycle.
 */
void 
BrainModelVolumeTopologyGraph::GraphCycle::set(const std::vector<int>& cycleVerticesIn,
                                               const std::vector<int>& cycleSlicesIn)
{
   if (cycleVerticesIn.size() != cycleSlicesIn.size()) {
      std::cout << "PROGRAM ERROR: size vertices != size slices in "
                   "BrainModelVolumeTopologyGraph::GraphCycle::set()"
                << std::endl;
   }
   
   cycle.clear();
   cycleSorted.clear();
   
   //
   // Sort so cycle starts with smallest SLICE number
   //
   int minSliceNumber = std::numeric_limits<int>::max();
   int minSliceIndex  = -1;
   const int num = static_cast<int>(cycleSlicesIn.size());
   for (int i = 0; i < num; i++) {
      if (cycleSlicesIn[i] < minSliceNumber) {
         minSliceNumber = cycleSlicesIn[i];
         minSliceIndex = i;
      }
   }
   for (int i = minSliceIndex; i < num; i++) {
      cycle.push_back(cycleVerticesIn[i]);
   }
   for (int i = 0; i < minSliceIndex; i++) {
      cycle.push_back(cycleVerticesIn[i]);
   }
   
   cycleSorted = cycleVerticesIn;
   std::sort(cycleSorted.begin(), cycleSorted.end());
}            
            
/**
 * set the vertices that form the handle.
 */
void 
BrainModelVolumeTopologyGraph::GraphCycle::setHandleVertices(const std::vector<int>& handleVerticesIn,
                                                             const int numVoxelsInHandleIn)
{
   handleVertices = handleVerticesIn;
   numVoxelsInHandle = numVoxelsInHandleIn;
}
            
/**
 * equality operator.
 */
bool 
BrainModelVolumeTopologyGraph::GraphCycle::operator==(const GraphCycle& c) const
{
   return std::equal(cycleSorted.begin(), cycleSorted.end(), c.cycleSorted.begin());
}

/**
 * comparison operator (based upon number of elements.
 */
bool 
BrainModelVolumeTopologyGraph::GraphCycle::operator<(const GraphCycle& c) const
{
   if (cycleSorted.size() == c.cycleSorted.size()) {
      return std::lexicographical_compare(cycleSorted.begin(), cycleSorted.end(),
                                          c.cycleSorted.begin(), c.cycleSorted.end());
   }
   return (cycleSorted.size() < c.cycleSorted.size());
}

/**
 * get the cycle.
 */
std::vector<int>
BrainModelVolumeTopologyGraph::GraphCycle::getCycle() const
{
   return cycle;
}

//==========================================================================
//==========================================================================
//==========================================================================
// 
//
//


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
#include <limits>

#include "BrainModelVolumeTopologyGraph.h"
#include "BrainModelVolumeTopologyGraphCorrector.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelVolumeTopologyGraphCorrector::BrainModelVolumeTopologyGraphCorrector(
                                       BrainSet* bsIn,
                                       const CORRECTION_MODE correctionModeIn,
                                       const VolumeFile* segmentationVolumeFileIn)
 : BrainModelAlgorithm(bsIn),
   correctionMode(correctionModeIn),
   segmentationVolumeFile(segmentationVolumeFileIn)
{
   correctedSegmentationVolumeFile = NULL;
   showingCorrectionsPaintVolumeFile = NULL;
   numberOfVoxelsChanged = 0;
}
                                       
/**
 * destructor.
 */
BrainModelVolumeTopologyGraphCorrector::~BrainModelVolumeTopologyGraphCorrector()
{
   if (correctedSegmentationVolumeFile != NULL) {
      delete correctedSegmentationVolumeFile;
      correctedSegmentationVolumeFile = NULL;
   }
   if (showingCorrectionsPaintVolumeFile != NULL) {
      delete showingCorrectionsPaintVolumeFile;
      showingCorrectionsPaintVolumeFile = NULL;
   }
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeTopologyGraphCorrector::execute() throw (BrainModelAlgorithmException)
{
   if (segmentationVolumeFile == NULL) {
      throw BrainModelAlgorithmException("Input segmentation volume is invalid.");
   }
   
   //
   // Copy the input volume
   //
   correctedSegmentationVolumeFile = new VolumeFile(*segmentationVolumeFile);
   correctedSegmentationVolumeFile->makeSegmentationZeroTwoFiftyFive();
   
   //
   // Copy of volume before correction
   //
   const VolumeFile uncorrectedVolumeFile(*correctedSegmentationVolumeFile);
   
   //
   // Create paint volume showing corrections
   //
   showingCorrectionsPaintVolumeFile = new VolumeFile(*segmentationVolumeFile);
   showingCorrectionsPaintVolumeFile->setVolumeType(VolumeFile::VOLUME_TYPE_PAINT);
   const int nonePaintIndex = showingCorrectionsPaintVolumeFile->addRegionName("???");
   showingCorrectionsPaintVolumeFile->setAllVoxels(nonePaintIndex);
   paintVoxelAddedIndex = showingCorrectionsPaintVolumeFile->addRegionName("ADDED"); 
   paintVoxelRemovedIndex = showingCorrectionsPaintVolumeFile->addRegionName("REMOVED"); 

   //
   // Try each connectivity
   //
   for (int conn = 0; conn < 6; conn++) {
      std::cout << "Conn: " << conn << std::endl;
      //
      // Connectivity used for generating graphs
      //
      BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY
         foregroundVoxelConnectivity = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_26;
      switch (conn) {
         case 0:
         case 3:
            foregroundVoxelConnectivity = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_26;
            break;
         case 1:
         case 4:
            foregroundVoxelConnectivity = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_18;
            break;
         case 2:
         case 5:
            foregroundVoxelConnectivity = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_6;
            break;
      }
      
      BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY
         backgroundVoxelConnectivity = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_26;
      switch (foregroundVoxelConnectivity) {
         case BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_6:
            backgroundVoxelConnectivity = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_18;
            break;
         case BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_18:
            backgroundVoxelConnectivity = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_6;
            break;
         case BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_26:
            backgroundVoxelConnectivity = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_6;
            break;
      }
      
      //
      // Do until done
      //
      VolumeFile backgroundVolumeFile;
      bool needToGenerateBackgroundVolumeFlag = true;
      //int ctr = 0;
      bool done = false;
      while (done == false) {
         //ctr++;
         //if (((ctr % 5) == 0) && (ctr > 0)) {
         //   const QString name("graph_corrected_"
         //                      + QString::number(ctr)
         //                      + ".nii.gz");
         //   correctedSegmentationVolumeFile->writeFile(name);
         //}
         
         //
         // Create background volume
         //
         if (needToGenerateBackgroundVolumeFlag) {
            backgroundVolumeFile = (*correctedSegmentationVolumeFile);
            backgroundVolumeFile.invertSegmentationVoxels();
         }
         
         //
         // Compute foreground and background graphs for each axis
         // 0-2 are foreground, 3-5 are background
         //
         const int numGraphs = 6;
         BrainModelVolumeTopologyGraph* graphs[numGraphs];
         createForegroundAndBackgroundGraphs(correctedSegmentationVolumeFile,
                                             &backgroundVolumeFile,
                                             foregroundVoxelConnectivity,
                                             backgroundVoxelConnectivity,
                                             graphs);
         
         //
         // Loop through graphs and find smallest handle
         //
         int fewestVoxelsGraphGraphIndex = -1;
         int fewestVoxelsCycleIndex = -1;
         int fewestVoxelsNumberOfVoxels = std::numeric_limits<int>::max();
         std::vector<BrainModelVolumeTopologyGraph::GraphVertex*> fewestVoxelVertices;
         
         switch (correctionMode) {
            case CORRECTION_MODE_MINIMAL:
               {
                  int fewestVoxelsVertexIndex = -1;
                  for (int i = 0; i < 6; i++) {
                     int cycleIndex;
                     int vertexIndex;
                     int numberOfVoxels;
                     graphs[i]->getGraphCycleWithSmallestVertex(cycleIndex, 
                                                                 vertexIndex,
                                                                 numberOfVoxels);
                     if (cycleIndex >= 0) {
                        if (numberOfVoxels <= fewestVoxelsNumberOfVoxels) {
                           fewestVoxelsNumberOfVoxels = numberOfVoxels;
                           fewestVoxelsCycleIndex = cycleIndex;
                           fewestVoxelsVertexIndex = vertexIndex;
                           fewestVoxelsGraphGraphIndex = i;
                        }
                     }
                  }
                  if (fewestVoxelsVertexIndex >= 0) {
                     fewestVoxelVertices.push_back(
                        graphs[fewestVoxelsGraphGraphIndex]->getGraphVertex(fewestVoxelsVertexIndex));
                  }
               }
               break;
            case CORRECTION_MODE_NORMAL:
               {
                  std::vector<int> fewestVertexIndices;
                  for (int i = 0; i < 6; i++) {
                     int cycleIndex;
                     int numberOfVoxels;
                     std::vector<int> vertexIndices;
                     graphs[i]->getGraphCycleWithSmallestHandle(cycleIndex,
                                                                vertexIndices,
                                                                numberOfVoxels);
                  
                     if (cycleIndex >= 0) {
                        if (numberOfVoxels <= fewestVoxelsNumberOfVoxels) {
                           fewestVoxelsNumberOfVoxels = numberOfVoxels;
                           fewestVoxelsCycleIndex = cycleIndex;
                           fewestVertexIndices = vertexIndices;
                           fewestVoxelsGraphGraphIndex = i;
                        }
                     }
                  }
                  
                  if (fewestVoxelsGraphGraphIndex >= 0) {
                     const int num = static_cast<int>(fewestVertexIndices.size());
                     for (int m = 0; m < num; m++) {
                        fewestVoxelVertices.push_back(
                           graphs[fewestVoxelsGraphGraphIndex]->getGraphVertex(fewestVertexIndices[m]));
                     }
                  }
               }
               break;
         }
         
         if (fewestVoxelsGraphGraphIndex >= 0) {
            const BrainModelVolumeTopologyGraph::GraphCycle* 
               cycle = graphs[fewestVoxelsGraphGraphIndex]->getGraphCycle(fewestVoxelsCycleIndex);
             if ((fewestVoxelsGraphGraphIndex >= 0) &&
                (fewestVoxelsGraphGraphIndex <= 2)) {
               //
               // If smallest handle is in foreground so
               // remove voxels from volume
               //
               addRemoveVoxels(correctedSegmentationVolumeFile,
                               &backgroundVolumeFile,
                               graphs[fewestVoxelsGraphGraphIndex],
                               cycle,
                               fewestVoxelVertices,
                               false);
            }
            else if ((fewestVoxelsGraphGraphIndex >= 3) &&
                     (fewestVoxelsGraphGraphIndex <= 5)) {
               //
               // Smallest handle in background so use
               // it to fill in the foreground
               //
               addRemoveVoxels(correctedSegmentationVolumeFile,
                               &backgroundVolumeFile,
                               graphs[fewestVoxelsGraphGraphIndex],
                               cycle,
                               fewestVoxelVertices,
                               true);
            }
            
            //
            // Remove any islands DO NOT FILL CAVITIES HERE
            //
            needToGenerateBackgroundVolumeFlag = 
               correctedSegmentationVolumeFile->removeIslandsFromSegmentation();
            if (needToGenerateBackgroundVolumeFlag) {
               std::cout << "Volume Topology Graph islands removed." << std::endl;
            }
         }
         else {
            //
            // No handles, so done
            //
            done = true;
         }
         
         //
         // Free the graphs
         //
         for (int i = 0; i < numGraphs; i++) {
            delete graphs[i];
            graphs[i] = NULL;
         }
      }
   }
   
   //
   // Fill any cavities that may be present
   //
   correctedSegmentationVolumeFile->fillSegmentationCavities();
   
   //
   // Count the number of voxels changed
   //
   int dimI, dimJ, dimK;
   uncorrectedVolumeFile.getDimensions(dimI, dimJ, dimK);
   for (int i = 0; i < dimI; i++) {
      for (int j = 0; j < dimJ; j++) {
         for (int k = 0; k < dimK; k++) {
            if (uncorrectedVolumeFile.getVoxel(i, j, k, 0) 
                != correctedSegmentationVolumeFile->getVoxel(i, j, k, 0)) {
               numberOfVoxelsChanged++;
            }
         }
      }
   }
   
   correctedSegmentationVolumeFile->makeDefaultFileName("Segment_GraphErrorCorrected");
   correctedSegmentationVolumeFile->setDescriptiveLabel("Segment_GraphErrorCorrected");
}

/**
 * add or remove voxels to/from a volume.
 */
void 
BrainModelVolumeTopologyGraphCorrector::addRemoveVoxels(
                        VolumeFile* foregroundVolumeFile,
                        VolumeFile* backgroundVolumeFile,
                        const BrainModelVolumeTopologyGraph* graph,
                        const BrainModelVolumeTopologyGraph::GraphCycle* cycle,
                        const std::vector<BrainModelVolumeTopologyGraph::GraphVertex*> vertices,
                        const bool addVoxelsFlag)
{
   std::cout << QString(70, '-').toAscii().constData() << std::endl;
   QString addRemoveString("Removing ");
   int newForegroundVoxelValue = 0;
   int newBackgroundVoxelValue = 255;
   int paintVolumeVoxelIndex = paintVoxelRemovedIndex;
   if (addVoxelsFlag) {
      newForegroundVoxelValue = 255;
      newBackgroundVoxelValue = 0;
      addRemoveString = "Adding ";
      paintVolumeVoxelIndex = paintVoxelAddedIndex;
   }
   
   QString axisText("Unknown");
   switch (graph->getSearchAxis()) {
      case BrainModelVolumeTopologyGraph::SEARCH_AXIS_X:
         axisText = "X-Axis";
         break;
      case BrainModelVolumeTopologyGraph::SEARCH_AXIS_Y:
         axisText = "Y-Axis";
         break;
      case BrainModelVolumeTopologyGraph::SEARCH_AXIS_Z:
         axisText = "Z-Axis";
         break;
   }
   
   const int numVertices = static_cast<int>(vertices.size());
   std::vector<VoxelIJK> voxels;
   for (int i = 0; i < numVertices; i++) {
      const int numVoxelsInVertex = vertices[i]->getNumberOfVoxels();
      for (int j = 0; j < numVoxelsInVertex; j++) {
         voxels.push_back(*vertices[i]->getVoxel(j));
      }
   }
      
   const int numVoxels = static_cast<int>(voxels.size());
   
   std::cout << addRemoveString.toAscii().constData()
             << numVoxels
             << " voxels using vertices in slice ";
   for (int n = 0; n < numVertices; n++) {
      std::cout << vertices[n]->getSliceNumber() << " ";
   }
   std::cout << " along "
             << axisText.toAscii().constData()
             << std::endl;
   
   std::cout << "   from cycle: ";
   const int numGraphVerticesInCycle = cycle->getNumberOfGraphVerticesInCycle();
   for (int j = 0; j < numGraphVerticesInCycle; j++) {
      const int graphVertexIndex = cycle->getGraphVertexIndex(j);
      const BrainModelVolumeTopologyGraph::GraphVertex* vertex = graph->getGraphVertex(graphVertexIndex);
      std::cout << vertex->getSliceNumber() 
                << "(" << vertex->getNumberOfVoxels() << ") ";
   }
   std::cout << std::endl;
   
   for (int i = 0; i < numVoxels; i++) {
      foregroundVolumeFile->setVoxel(voxels[i], 0, newForegroundVoxelValue);
      backgroundVolumeFile->setVoxel(voxels[i], 0, newBackgroundVoxelValue);
      showingCorrectionsPaintVolumeFile->setVoxel(voxels[i], 0,
                                                  paintVolumeVoxelIndex);
   }
}

/**
 * create the foreground and background graphs.
 */
void 
BrainModelVolumeTopologyGraphCorrector::createForegroundAndBackgroundGraphs(
           const VolumeFile* foregroundVolumeFile,
           const VolumeFile* backgroundVolumeFile,
           const BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY foregroundVoxelConnectivity,
           const BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY backgroundVoxelConnectivity,
           BrainModelVolumeTopologyGraph* graphsOut[6]) const
                            throw (BrainModelAlgorithmException)
{
   //
   // Foreground graphs
   //
   graphsOut[0] = new BrainModelVolumeTopologyGraph(brainSet,
                               foregroundVolumeFile,
                               BrainModelVolumeTopologyGraph::SEARCH_AXIS_X,
                               foregroundVoxelConnectivity);
   graphsOut[1] = new BrainModelVolumeTopologyGraph(brainSet,
                               foregroundVolumeFile,
                               BrainModelVolumeTopologyGraph::SEARCH_AXIS_Y,
                               foregroundVoxelConnectivity);
   graphsOut[2] = new BrainModelVolumeTopologyGraph(brainSet,
                               foregroundVolumeFile,
                               BrainModelVolumeTopologyGraph::SEARCH_AXIS_Z,
                               foregroundVoxelConnectivity);
   
   //
   // Background (inverted segmentation) graphs
   //
   graphsOut[3] = new BrainModelVolumeTopologyGraph(brainSet,
                               backgroundVolumeFile,
                               BrainModelVolumeTopologyGraph::SEARCH_AXIS_X,
                               backgroundVoxelConnectivity);
   graphsOut[4] = new BrainModelVolumeTopologyGraph(brainSet,
                               backgroundVolumeFile,
                               BrainModelVolumeTopologyGraph::SEARCH_AXIS_Y,
                               backgroundVoxelConnectivity);
   graphsOut[5] = new BrainModelVolumeTopologyGraph(brainSet,
                               backgroundVolumeFile,
                               BrainModelVolumeTopologyGraph::SEARCH_AXIS_Z,
                               backgroundVoxelConnectivity);
                               
   for (int i = 0; i < 6; i++) {
      graphsOut[i]->execute();
   }
}


#ifndef __BRAIN_MODEL_VOLUME_TOPOLOGY_GRAPH_CORRECTOR_H__
#define __BRAIN_MODEL_VOLUME_TOPOLOGY_GRAPH_CORRECTOR_H__

#include "BrainModelAlgorithm.h"

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

#include "BrainModelVolumeTopologyGraph.h"

class VolumeFile;

/// class for correcting volume topology using a graph
class BrainModelVolumeTopologyGraphCorrector : public BrainModelAlgorithm {
   public:
      /// correction mode
      enum CORRECTION_MODE {
         /// make minimal corrections needed to repair topology
         CORRECTION_MODE_MINIMAL,
         /// normal fill and remove handles
         CORRECTION_MODE_NORMAL
      };
      
      // constructor
      BrainModelVolumeTopologyGraphCorrector(BrainSet* bsIn,
                                             const CORRECTION_MODE correctionModeIn,
                                             const VolumeFile* segmentationVolumeFileIn);
                                             
      // destructor
      ~BrainModelVolumeTopologyGraphCorrector();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// get the number of voxels changed during the correction process
      int getNumberOfVoxelsChanged() const { return numberOfVoxelsChanged; }
      
      // get the corrected segmentation volume file
      const VolumeFile* getCorrectedSegmentationVolumeFile() const 
            { return correctedSegmentationVolumeFile; }
         
      // get the paint volume file showing corrections
      const VolumeFile* getShowingCorrectionsPaintVolumeFile() const 
            { return showingCorrectionsPaintVolumeFile; }
         
   protected:
      // add or remove voxels to/from a volume
      void addRemoveVoxels(VolumeFile* foregroundVolumeFile,
                           VolumeFile* backgroundVolumeFile,
                           const BrainModelVolumeTopologyGraph* graph,
                           const BrainModelVolumeTopologyGraph::GraphCycle* cycle,
                           const std::vector<BrainModelVolumeTopologyGraph::GraphVertex*> vertices,
                           const bool addVoxelsFlag);
                           
      // create the foreground and background graphs
      void createForegroundAndBackgroundGraphs(
           const VolumeFile* foregroundVolumeFile,
           const VolumeFile* backgroundVolumeFile,
           const BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY foregroundVoxelConnectivity,
           const BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY backgroundVoxelConnectivity,
           BrainModelVolumeTopologyGraph* graphsOut[6]) const 
                                    throw (BrainModelAlgorithmException);

      /// correction mode
      CORRECTION_MODE correctionMode;
      
      /// segmentation volume that is to be corrected
      const VolumeFile* segmentationVolumeFile;
      
      /// corrected segmentation volume file
      VolumeFile* correctedSegmentationVolumeFile;
      
      /// number of voxels changed during correction process
      int numberOfVoxelsChanged;
      
      /// paint volume file containing corrections
      VolumeFile* showingCorrectionsPaintVolumeFile;
      
      /// index of voxels added in paint volume containing corrections
      int paintVoxelAddedIndex; 
      
      /// index of voxels added in paint volume containing corrections
      int paintVoxelRemovedIndex; 
};

#endif // __BRAIN_MODEL_VOLUME_TOPOLOGY_GRAPH_CORRECTOR_H__

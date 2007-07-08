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

#ifndef __BRAIN_MODEL_VOLUME_TO_SURFACE_METRIC_MAPPER_H__
#define __BRAIN_MODEL_VOLUME_TO_SURFACE_METRIC_MAPPER_H__

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class MetricFile;
class TopologyHelper;
class VolumeFile;

/// class that maps a volume (typically functional) to a surface's metric file
class BrainModelVolumeToSurfaceMetricMapper : public BrainModelAlgorithm {
   public:
      ///
      enum ALGORITHM {
         ALGORITHM_AVERAGE_NODES,
         ALGORITHM_AVERAGE_VOXEL,
         ALGORITHM_ENCLOSING_VOXEL,
         ALGORITHM_GAUSSIAN,
         ALGORITHM_INTERPOLATED_VOXEL,
         ALGORITHM_MAXIMUM_VOXEL,
         ALGORITHM_MCW_BRAINFISH
      };
      
      /// Constructor for a volume file in memory.
      BrainModelVolumeToSurfaceMetricMapper(BrainSet* bs,
                                            const ALGORITHM algorithmIn,
                                            BrainModelSurface* surfaceIn,
                                            VolumeFile* volumeFileIn,
                                            MetricFile* metricFileIn,
                                            const int metricColumnIn,
                                            const QString& metricColumnNameIn);

      /// Constructor for a volume file that needs to be read.
      BrainModelVolumeToSurfaceMetricMapper(BrainSet* bs,
                                            const ALGORITHM algorithmIn,
                                            BrainModelSurface* surfaceIn,
                                            const QString& volumeFileNameIn,
                                            MetricFile* metricFileIn,
                                            const int metricColumnIn,
                                            const QString& metricColumnNameIn);
                                                                                            
      /// Destructor
      ~BrainModelVolumeToSurfaceMetricMapper();
      
      /// get the average voxel parameters
      void getAlgorithmAverageVoxelParameters(float neighborsOut) const;
      
      /// set the average voxel parameters
      void setAlgorithmAverageVoxelParameters(const float neighborsIn);
      
      /// get the maximum voxel parameters
      void getAlgorithmMaximumVoxelParameters(float neighborsOut) const;
      
      /// set the maximum voxel parameters
      void setAlgorithmMaximumVoxelParameters(const float neighborsIn);
      
      /// set gaussian algorithm parameters
      void getAlgorithmGaussianParameters(float& gaussianNeighborsOut,
                                          float& gaussianSigmaNormOut,
                                          float& gaussianSigmaTangOut,
                                          float& gaussianNormBelowCutoffOut,
                                          float& gaussianNormAboveCutoffOut,
                                          float& gaussianTangCutoffOut) const;
                                          
      /// set gaussian algorithm parameters
      void setAlgorithmGaussianParameters(const float gaussianNeighborsIn,
                                          const float gaussianSigmaNormIn,
                                          const float gaussianSigmaTangIn,
                                          const float gaussianNormBelowCutoffIn,
                                          const float gaussianNormAboveCutoffIn,
                                          const float gaussianTangCutoffIn);
       
      /// get the mcw brainfish algorithm parameters
      void getAlgorithmMcwBrainFishParameters(float& mcwBrainFishMaxDistanceOut,
                                              int& mcwBrainFishSplatFactorOut) const;
                                              
      /// set the mcw brainfish algorithm parameters
      void setAlgorithmMcwBrainFishParameters(const float mcwBrainFishMaxDistanceIn,
                                              const int mcwBrainFishSplatFactorIn);
                                              
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
   
   protected:
      /// Run the Average Nodes algorithm
      void algorithmAverageNodes(const float* allCoords);

      /// Run the Average Voxel algorithm
      void algorithmAverageVoxel(const float* allCoords);
      
      /// Run the Enclosing Voxel algorithm
      void algorithmEnclosingVoxel(const float* allCoords);

      /// Run the Gaussian algorithm
      void algorithmGaussian(const float* allCoords);

      /// Run the Interpolated Voxel algorithm
      void algorithmInterpolatedVoxel(const float* allCoords);

      /// Run the Maximum Voxel algorithm
      void algorithmMaximumVoxel(const float* allCoords);
 
      /// Run the MCW Brain Fish algorithm
      void algorithmMcwBrainFish(const float* allCoords);
   
      /// Get the valid subvolume for neighbor mapping algorithms.
      bool getNeighborsSubVolume(const float xyz[3],
                                 int& iMin, int& iMax,
                                 int& jMin, int& jMax,
                                 int& kMin, int& kMax,
                                 const float neighborsCubeSize) const;
                                 
/*
      // Determine the gaussian for a node
      void gaussian(const float nodeXYZ[3],
                    const float nodeNormal[3],
                    const float voxelXYZ[3],
                    const float voxelValue,
                    float& metricSum,
                    float& weightSum) const;
*/
                                                
      /// volume source type
      enum MODE_VOLUME {
         MODE_VOLUME_IN_MEMORY,
         MODE_VOLUME_ON_DISK
      };
      
      /// surface for mapping
      BrainModelSurface* surface;
      
      /// volume to map
      VolumeFile* volumeFile;
      
      /// metric file to update
      MetricFile* metricFile;
      
      /// the metric file column
      int metricColumn;
      
      /// name for metric column
      QString metricColumnName;
      
      /// algorithm used for mapping
      ALGORITHM algorithm;
      
      /// volume type mode
      MODE_VOLUME volumeMode;
      
      /// volume file name
      QString volumeFileName;

      /// average voxel neighbors
      float averageVoxelNeighbors;
      
      /// maximum voxel neighbors
      float maximumVoxelNeighbors;
      
      /// gaussian  neighbors
      float gaussianNeighbors;
      
      /// gaussian sigma norm
      float gaussianSigmaNorm;
      
      /// gaussian sigma tang
      float gaussianSigmaTang;
      
      /// gaussian norm below cutoff
      float gaussianNormBelowCutoff;
      
      /// gaussian norm above cutoff
      float gaussianNormAboveCutoff;
      
      /// gaussian tang cutoff
      float gaussianTangCutoff;
      
      /// mcw brain fish max distance
      float mcwBrainFishMaxDistance;
      
      /// mcw brain fish splat factor
      int mcwBrainFishSplatFactor;
      
      /// the topology helper
      TopologyHelper* topologyHelper;
      
      /// number of nodes in the surface
      int numberOfNodes;
      
      /// metric column number being updated
      int metricColumnNumber;
      
      /// volume dimensions
      int volumeDimensions[3];
      
      /// volume origin
      float volumeOrigin[3];
      
      /// volume voxel size
      float volumeVoxelSize[3];
};

#endif // __BRAIN_MODEL_VOLUME_TO_SURFACE_METRIC_MAPPER_H__


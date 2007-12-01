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

#ifndef __BRAIN_MODEL_VOLUME_TO_SURFACE_MAPPER_H__
#define __BRAIN_MODEL_VOLUME_TO_SURFACE_MAPPER_H__

#include "BrainModelAlgorithm.h"
#include "BrainModelVolumeToSurfaceMapperAlgorithmParameters.h"

class BrainModelSurface;
class GiftiNodeDataFile;
class MetricFile;
class PaintFile;
class TopologyHelper;
class VolumeFile;

/// class that maps a volume (typically functional) to a surface's node attribute file
class BrainModelVolumeToSurfaceMapper : public BrainModelAlgorithm {
   public:
      /// Constructor for a volume file in memory.
      BrainModelVolumeToSurfaceMapper(BrainSet* bs,
                             BrainModelSurface* surfaceIn,
                             VolumeFile* volumeFileIn,
                             GiftiNodeDataFile* dataFileIn,
                             const BrainModelVolumeToSurfaceMapperAlgorithmParameters algorithmIn,
                             const int dataFileColumnIn,
                             const QString& dataFileColumnNameIn);

      /// Constructor for a volume file that needs to be read.
      BrainModelVolumeToSurfaceMapper(BrainSet* bs,
                             BrainModelSurface* surfaceIn,
                             const QString& volumeFileNameIn,
                             GiftiNodeDataFile* dataFileIn,
                             const BrainModelVolumeToSurfaceMapperAlgorithmParameters algorithmIn,
                             const int dataFileColumnIn,
                             const QString& dataFileColumnNameIn);
                                                                                            
      /// Destructor
      ~BrainModelVolumeToSurfaceMapper();
      
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
   
   protected:
      /// Run the Metric Average Nodes algorithm
      void algorithmMetricAverageNodes(const float* allCoords);

      /// Run the Metric Average Voxel algorithm
      void algorithmMetricAverageVoxel(const float* allCoords);
      
      /// Run the Metric Enclosing Voxel algorithm
      void algorithmMetricEnclosingVoxel(const float* allCoords);

      /// Run the Metric Gaussian algorithm
      void algorithmMetricGaussian(const float* allCoords);

      /// Run the Metric Interpolated Voxel algorithm
      void algorithmMetricInterpolatedVoxel(const float* allCoords);

      /// Run the Metric Maximum Voxel algorithm
      void algorithmMetricMaximumVoxel(const float* allCoords);
 
      /// Run the Metric Strongest Voxel algorithm
      void algorithmMetricStrongestVoxel(const float* allCoords);
      
      /// Run the Metric MCW Brain Fish algorithm
      void algorithmMetricMcwBrainFish(const float* allCoords);
   
      /// Run the Paint Enclosing Voxel algorithm
      void algorithmPaintEnclosingVoxel(const float* allCoords);

      /// add paint names for paint indices without names
      void addPaintNamesForIndicesWithoutNames();
      
      /// Get the valid subvolume for neighbor mapping algorithms.
      bool getNeighborsSubVolume(const float xyz[3],
                                 int& iMin, int& iMax,
                                 int& jMin, int& jMax,
                                 int& kMin, int& kMax,
                                 const float neighborsCubeSize) const;
                                 
      /// volume source type
      enum MODE_VOLUME {
         MODE_VOLUME_IN_MEMORY,
         MODE_VOLUME_ON_DISK
      };
      
      /// the algorithm parameters
      BrainModelVolumeToSurfaceMapperAlgorithmParameters algorithmParameters;
      
      /// surface for mapping
      BrainModelSurface* surface;
      
      /// volume to map
      VolumeFile* volumeFile;
      
      /// the data file
      GiftiNodeDataFile* dataFile;
      
      /// metric file to update
      MetricFile* metricFile;
      
      /// paint file to update
      PaintFile* paintFile;
      
      /// the data file column
      int dataFileColumn;
      
      /// name for data column
      QString dataFileColumnName;
      
      /// volume type mode
      MODE_VOLUME volumeMode;
      
      /// volume file name
      QString volumeFileName;

      /// the topology helper
      TopologyHelper* topologyHelper;
      
      /// number of nodes in the surface
      int numberOfNodes;
      
      /// data file column number being updated
      int dataFileColumnNumber;
      
      /// volume dimensions
      int volumeDimensions[3];
      
      /// volume origin
      float volumeOrigin[3];
      
      /// volume voxel size
      float volumeVoxelSize[3];
      
      /// index of ??? paint name
      int paintQuestionNameIndex;
      
      /// translates paint volume indices to paint file indices
      std::vector<int> paintVolumeIndexToPaintFileNameIndex;

};

#endif // __BRAIN_MODEL_VOLUME_TO_SURFACE_MAPPER_H__


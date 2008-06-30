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



#ifndef __BRAIN_MODEL_SURFACE_TO_VOLUME_CONVERTER_H__
#define __BRAIN_MODEL_SURFACE_TO_VOLUME_CONVERTER_H__

#include <set>
#include <vector>

#include "BrainModelAlgorithm.h"
#include "StereotaxicSpace.h"
#include "VolumeFile.h"

class BrainModelSurface;
class BrainSet;
class BrainModelSurfaceNodeColoring;
class CoordinateFile;
class vtkTriangle;

/// class that determines how a node maps to voxel
class NodeToVoxelMapping {
   public:
      /// constructor
      NodeToVoxelMapping(const int node, const int ijk[3]) {
         nodeNumber  = node;
         voxelIJK[0] = ijk[0];
         voxelIJK[1] = ijk[1];
         voxelIJK[2] = ijk[2];
      }
      
      /// the node number
      int nodeNumber;
      
      /// voxel intersected by node
      int voxelIJK[3];
      
      /// equality operator
      bool operator==(const NodeToVoxelMapping& n) const {
         return ((nodeNumber == n.nodeNumber) &&
                 (voxelIJK[0] == n.voxelIJK[0]) &&
                 (voxelIJK[1] == n.voxelIJK[1]) &&
                 (voxelIJK[2] == n.voxelIJK[2]));
      }
      
      /// less than operator
      bool operator<(const NodeToVoxelMapping& n) const {
         if (nodeNumber < n.nodeNumber) {
             return true;
         }
         else if (nodeNumber == n.nodeNumber) {
            if (voxelIJK[0] < n.voxelIJK[0]) { 
               return true;
            }
            else if (voxelIJK[0] == n.voxelIJK[0]) {
               if (voxelIJK[1] < n.voxelIJK[1]) { 
                  return true;
               }
               else if (voxelIJK[1] == n.voxelIJK[1]) {
                  if (voxelIJK[2] < n.voxelIJK[2]) { 
                     return true;
                  }
               }
            }
         }
         return false;
      }
};

/// Convert a brain model surface into a volume
class BrainModelSurfaceToVolumeConverter : public BrainModelAlgorithm {
   public:
      /// conversion mode
      enum CONVERSION_MODE {
         CONVERT_TO_RGB_VOLUME_USING_NODE_COLORING,
         CONVERT_TO_ROI_VOLUME_USING_ROI_NODES,
         CONVERT_TO_ROI_VOLUME_USING_PAINT,
         CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE,
         CONVERT_TO_ROI_VOLUME_USING_METRIC_NO_INTERPOLATE,
         CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE,
         CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES
      };
      
      /// intersection mode
      enum INTERSECTION_MODE {
         /// intersection mode intersect tiles with voxels
         INTERSECTION_MODE_INTERSECT_TILES_AND_VOXELS,
         /// intersection mode project voxels to surface
         INTERSECTION_MODE_PROJECT_VOXELS_TO_SURFACE
      };
      
      /// Constructor
      BrainModelSurfaceToVolumeConverter(BrainSet* bs,
                           BrainModelSurface* surfaceIn,
                           const StereotaxicSpace volumeSpaceHintIn,
                           const float surfaceOffsetIn[3],
                           const int volumeDimensionsIn[3],
                           const float voxelSizeIn[3],
                           const float volumeOriginIn[3],
                           const float innerBoundaryIn,
                           const float outerBoundaryIn,
                           const float thicknessStepIn,
                           const CONVERSION_MODE convertModeIn,
                           const INTERSECTION_MODE intersectionModeIn
                              = INTERSECTION_MODE_INTERSECT_TILES_AND_VOXELS);
        
      /// Destructor
      ~BrainModelSurfaceToVolumeConverter();
      
      /// get names and values for intersection modes
      static void getIntersectionModeNamesAndValues(
               std::vector<INTERSECTION_MODE>& intersectionModesOut,
               std::vector<QString> intersectionModeNamesOut);

      /// set the node attribute (paint/metric/shape) column for ROI
      void setNodeAttributeColumn(const int column) { nodeAttributeColumn = column; }
      
      /// get the region of interest voxel value
      float getRegionOfInterestVoxelValue() const { return roiVoxelValue; }
      
      /// set the region of interest voxel value
      void setRegionOfInterestVoxelValue(const float val) { roiVoxelValue = val; }
      
      /// get node to voxel mapping enabled and file name
      void getNodeToVoxelMappingEnabled(bool& enabled, 
                                        QString& fileName) const;
      
      /// set node to voxel mapping enabled and file name
      void setNodeToVoxelMappingEnabled(const bool enabled, 
                                        const QString& fileName);
      
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// get the volume that was created
      VolumeFile* getOutputVolume() { return volume; }
      
   private:
      /// perform conversion by intersecting tiles and voxels
      void conversionIntersectTilesAndVoxels() throw (BrainModelAlgorithmException);
      
      /// perform conversion by projecting voxels to surface
      void conversionProjectVoxelsToSurface();
      
      /// get the average rgb colors of three nodes
      void getTilesRgbColor(const int n1, const int n2,
                            const int n3, float rgbFloat[3]);
        
      /// determine if a voxel index is valid
      bool getVoxelIndexValid(const int i, const int j, const int k) const;
      
      /// see if a triangle and a boxel intersect
      bool intersectTriangleWithVoxel(vtkTriangle* tri,
                                      float t1[3],
                                      float t2[3],
                                      float t3[3],
                                      const int ijk[3]);

      /// Resample the volume to a standard space.
      void resampleVolumeToStandardSpace();
      
      /// surface being converted
      BrainModelSurface* surface;
      
      /// output volume
      VolumeFile* volume;
      
      /// volume standard space
      StereotaxicSpace volumeSpaceHint;
      
      /// coloring of surface's nodes
      BrainModelSurfaceNodeColoring* bsnc;
      
      /// surface's coordinate file
      CoordinateFile* cf;
      
      /// surface offset to place it into volume
      float surfaceOffset[3];
      
      /// dimensions of output volume
      int volumeDimensions[3];
      
      /// type of conversion
      CONVERSION_MODE conversionMode;

      /// intersection mode
      INTERSECTION_MODE intersectionMode;
      
      /// flag to check is a voxel already has a value
      std::vector<bool> voxelSet;
      
      /// size of voxels
      float voxelSize[3];
      
      /// origin of volume
      float volumeOrigin[3];
      
      /// value for a region of interest voxel
      float roiVoxelValue;
      
      /// surface inner boundary
      float innerBoundary;
      
      /// surface outer boundary
      float outerBoundary;
      
      /// step when filling "thick" surface
      float thicknessStep;
      
      /// node attribute (metric/paint/shape) column for ROI
      int nodeAttributeColumn;
      
      /// total number of steps for progress dialog
      int progressDialogTotalSteps;
      
      /// current number of steps for progress dialog
      int progressDialogCurrentSteps;
      
      /// mapping of nodes to voxels
      std::set<NodeToVoxelMapping> nodeToVoxelMapping;
      
      /// node to voxel mapping enabled
      bool nodeToVoxelMappingEnabled;
      
      /// node to voxel mapping file name
      QString nodeToVoxelMappingFileName;
};

#endif // __BRAIN_MODEL_SURFACE_TO_VOLUME_CONVERTER_H__


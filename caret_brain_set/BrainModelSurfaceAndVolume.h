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

#ifndef __BRAIN_MODEL_VOLUME_AND_SURFACE_H__
#define __BRAIN_MODEL_VOLUME_AND_SURFACE_H__

#include "BrainModelSurface.h"
#include "SceneFile.h"

class TransformationMatrix;
class VolumeFile;

/// BrainModelSurfaceAndVolume allows combined volume and surface rendering
class BrainModelSurfaceAndVolume : public BrainModelSurface {
   public:
      /// constructor
      BrainModelSurfaceAndVolume(BrainSet* bs);
       
      /// Destructor
      virtual ~BrainModelSurfaceAndVolume();
       
      /// reset the volume and surface
      void reset();
      
      /// Apply a transformation matrix to the surface
      void applyTransformationMatrix(TransformationMatrix& tm);
      
      /// initialize the selected slices
      void initializeSelectedSlices();
      
      /// Get a descriptive name of the model
      QString getDescriptiveName() const;
      
      /// get the anatomical volume file
      VolumeFile* getAnatomyVolumeFile();
      
      /// get the functional volume file
      VolumeFile* getFunctionalVolumeFile();
      
      /// get the segmenation volume file
      VolumeFile* getSegmentationVolumeFile();
      
      /// get the vector volume file
      VolumeFile* getVectorVolumeFile();
      
      /// update functional volume surface distances
      void updateFunctionalVolumeSurfaceDistances();
      
      /// get the display horizontal slice flag
      bool getDisplayHorizontalSlice() const { return displayHorizontalSlice; }
      
      /// set the display horizontal slice flag
      void setDisplayHorizontalSlice(const bool ds) { displayHorizontalSlice = ds; }
      
      /// get the display parasagittal slice flag
      bool getDisplayParasagittalSlice() const { return displayParasagittalSlice; }
      
      /// set the display parasagittal slice flag
      void setDisplayParasagittalSlice(const bool ds) { displayParasagittalSlice = ds; }
      
      /// get the display coronal slice flag
      bool getDisplayCoronalSlice() const { return displayCoronalSlice; }
      
      /// set the display coronal slice flag
      void setDisplayCoronalSlice(const bool ds) { displayCoronalSlice = ds; }
      
      /// get the display view dependent slice flag
      bool getDisplayViewDependentSlice() const { return displayViewDependentSlice; }
      
      /// set the display view dependent slice flag
      void setDisplayViewDependentSlice(const bool ds) { displayViewDependentSlice = ds; }
      
      /// get the display surface flag
      bool getDisplaySurface() const { return displaySurface; }
      
      /// set the display surface flag
      void setDisplaySurface(const bool ds);
      
      /// get the secondary overlay volume file
      VolumeFile* getOverlaySecondaryVolumeFile();

      /// get the primary overlay volume file
      VolumeFile* getOverlayPrimaryVolumeFile();

      /// get the voxel cloud display list
      unsigned int getVoxelCloudDisplayListNumber() const;
      
      /// set the voxel cloud display list
      void setVoxelCloudDisplayListNumber(const unsigned int num);
      
      /// clear the voxel cloud display list
      void clearVoxelCloudDisplayList();
      
      /// get display segmentation volume data cloud
      bool getDisplaySegmentationVolumeCloud() const 
         { return displaySegmentationVolumeCloud; }

      /// set display segmentation volume data cloud
      void setDisplaySegmentationVolumeCloud(const bool fvc);
         
      /// get display vector volume data
      bool getDisplayVectorVolumeCloud() const 
         { return displayVectorVolumeCloud; }
         
      /// set display vector volume data cloud
      void setDisplayVectorVolumeCloud(const bool vvc)
         { displayVectorVolumeCloud = vvc; }
         
      /// get display functional volume data cloud
      bool getDisplayFunctionalVolumeCloud() const 
         { return displayFunctionalVolumeCloud; }

      /// set display functional volume data cloud
      void setDisplayFunctionalVolumeCloud(const bool fvc);
         
      /// get functional volume cloud opacity enabled
      bool getFunctionalVolumeCloudOpacityEnabled() 
         { return functionalVolumeCloudOpacityEnabled; }
         
      /// set functional volume cloud opacity enabled
      void setFunctionalVolumeCloudOpacityEnabled(const bool fcoe);
         
      /// get the functional volume cloud opacity
      float getFunctionalVolumeCloudOpacity() const
         { return functionalVolumeCloudOpacity; }
         
      /// set the functional volume cloud opacity
      void setFunctionalVolumeCloudOpacity(const float fvco)
         { functionalVolumeCloudOpacity = fvco; }
         
      /// get the functional volume distance threshold
      float getFunctionalVolumeDistanceThreshold() const 
         { return functionalVolumeDistanceThreshold; }
         
      /// set the functional volume distance threshold
      void setFunctionalVolumeDistanceThreshold(const float fvdt);
         
      /// get display secondary overlay on slices
      bool getDisplaySecondaryOverlayVolumeOnSlices() const 
         { return displaySecondaryOverlayVolumeOnSlices; }
         
      /// set display secondary overlay on slices
      void setDisplaySecondaryOverlayVolumeOnSlices(const bool sovs)
         { displaySecondaryOverlayVolumeOnSlices = sovs; }
         
      /// get display primary overlay on slices
      bool getDisplayPrimaryOverlayVolumeOnSlices() const 
         { return displayPrimaryOverlayVolumeOnSlices; }
         
      /// set display primary overlay on slices
      void setDisplayPrimaryOverlayVolumeOnSlices(const bool povs)
         { displayPrimaryOverlayVolumeOnSlices = povs; }
         
      /// get the selected slices
      void getSelectedSlices(int slices[3]);
      
      /// set the selected slices
      void setSelectedSlices(const int slices[3]);

      /// Copy active fiducial surface into this surface
      void setSurface();
      
      /// get draw black anatomy voxels
      bool getDrawAnatomyBlackVoxels() const { return drawAnatomyBlackVoxels; }
      
      /// set draw black anatomy voxels
      void setDrawAnatomyBlackVoxels(const bool b) { drawAnatomyBlackVoxels = b; }
      
      /// apply a scene (set display settings)
      void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      void saveScene(SceneFile::Scene& scene,
                     const bool onlyIfSelectedFlag);
                       
   private:
      /// display surface flag
      bool displaySurface;
      
      /// display secondary overlay volume data on slices
      bool displaySecondaryOverlayVolumeOnSlices;
      
      /// display primary overlay volume data on slices
      bool displayPrimaryOverlayVolumeOnSlices;
      
      /// display the functional volume as a cloud
      bool displayFunctionalVolumeCloud;
      
      /// display the segmentation volume as a cloud
      bool displaySegmentationVolumeCloud;
      
      /// display the vector volume as a cloud
      bool displayVectorVolumeCloud;
      
      /// the selected slices
      int selectedSlices[3];
      
      /// display horizontal slice flag
      bool displayHorizontalSlice;
      
      /// display parasagittal slice flag
      bool displayParasagittalSlice;
      
      /// display coronal slice flag
      bool displayCoronalSlice;
      
      /// display view dependent slice
      bool displayViewDependentSlice;

      /// functional volume cloud opacity
      float functionalVolumeCloudOpacity;
  
      /// functional volume cloud opacity enabled
      bool functionalVolumeCloudOpacityEnabled;
      
      /// functional volume distance threshold
      float functionalVolumeDistanceThreshold;
      
      /// voxel cloud display list number
      unsigned int voxelCloudDisplayListNumber;
      
      /// previous functional volume file
      VolumeFile* previousFunctionalVolumeFile;
      
      /// previous segmentation volume file
      VolumeFile* previousSegmentationVolumeFile;
      
      /// draw anatomy voxels that are black
      bool drawAnatomyBlackVoxels;
      
};

#endif // __BRAIN_MODEL_VOLUME_AND_SURFACE_H__


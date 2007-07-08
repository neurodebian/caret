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

#ifndef __BRAIN_MODEL_VOLUME_VOXEL_COLORING_H__
#define __BRAIN_MODEL_VOLUME_VOXEL_COLORING_H__

#include "SceneFile.h"

class BrainSet;
class VolumeFile;

/// class for coloring of volumes
class BrainModelVolumeVoxelColoring {
   public:
      enum UNDERLAY_OVERLAY_TYPE {
         UNDERLAY_OVERLAY_NONE,
         UNDERLAY_OVERLAY_ANATOMY,
         UNDERLAY_OVERLAY_FUNCTIONAL,
         UNDERLAY_OVERLAY_PAINT,
         UNDERLAY_OVERLAY_PROB_ATLAS,
         UNDERLAY_OVERLAY_RGB,
         UNDERLAY_OVERLAY_SEGMENTATION,
         UNDERLAY_OVERLAY_VECTOR
      };
      
      /// constructor
      BrainModelVolumeVoxelColoring(BrainSet* bs);
      
      /// destructor
      ~BrainModelVolumeVoxelColoring();
      
      /// initialize the underlay to the first available volume
      void initializeUnderlay();
     
      /// get the underlay
      UNDERLAY_OVERLAY_TYPE getUnderlay() const { return underlay; }
      
      /// set the underlay
      void setUnderlay(const UNDERLAY_OVERLAY_TYPE u) { underlay = u; }
      
      /// get the primary overlay
      UNDERLAY_OVERLAY_TYPE getPrimaryOverlay() const { return primaryOverlay; }
      
      /// set the primary overlay
      void setPrimaryOverlay(const UNDERLAY_OVERLAY_TYPE po) { primaryOverlay = po; }
      
      /// get the secondary overlay
      UNDERLAY_OVERLAY_TYPE getSecondaryOverlay() const { return secondaryOverlay; }
      
      /// set the secondary overlay
      void setSecondaryOverlay(const UNDERLAY_OVERLAY_TYPE so) { secondaryOverlay = so; }
   
      /// Color all of the volumes voxels
      void colorAllOfTheVolumesVoxels(VolumeFile* vf);
      
      /// Get a voxel's coloring.
      void getVoxelColoring(VolumeFile* vf,
                            const int i,
                            const int j,
                            const int k,
                            unsigned char rgb[4]);
                            
      /// Set all functional volume coloring invalid 
      void setVolumeFunctionalColoringInvalid();
      
      /// Set all paint volume coloring invalid
      void setVolumePaintColoringInvalid();
      
      /// Set all prob atlas volume coloring invalid
      void setVolumeProbAtlasColoringInvalid();
      
      /// Set all functional volume coloring invalid
      void setVolumeAnatomyColoringInvalid();
      
      /// Set all segmentation volume coloring invalid 
      void setVolumeSegmentationColoringInvalid();
      
      /// Set all rgb paint volume coloring invalid 
      void setVolumeRgbPaintColoringInvalid();
      
      /// Set all vector volume coloring invalid
      void setVolumeVectorColoringInvalid();
      
      /// set all volume coloring invalid
      void setVolumeAllColoringInvalid();
      
      /// Get the color for a segmentation voxel that is "on"
      static void getSegmentationVoxelOnColor(unsigned char rgbs[4]);
      
      /// Get the color for a segmentation voxel that is "off"
      static void getSegmentationVoxelOffColor(unsigned char rgbs[4]);

      /// apply a scene (set display settings)
      void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      void saveScene(SceneFile::Scene& scene,
                     const bool onlyIfSelectedFlag);
                       
      /// see if an overlay or underlay is of a specific type
      bool isUnderlayOrOverlay(const UNDERLAY_OVERLAY_TYPE uo) const;
                               
   private:
      /// Assign normal probabilistic coloring to a voxel
      void assignNormalProbAtlasColor(const int i,
                                      const int j,
                                      const int k,
                                      unsigned char rgb[4]);

      /// Assign threshold probabilistic coloring to a voxel
      void assignThresholdProbAtlasColor(const int i,
                                         const int j,
                                         const int k,
                                         unsigned char rgb[4]);
                                                             
      /// the brain set this object is associated with
      BrainSet* brainSet;
      
      /// the underlay
      UNDERLAY_OVERLAY_TYPE underlay;
      
      /// the primary overlay
      UNDERLAY_OVERLAY_TYPE primaryOverlay;
      
      /// the secondary overlay
      UNDERLAY_OVERLAY_TYPE secondaryOverlay;
      
};

#endif // __BRAIN_MODEL_VOLUME_VOXEL_COLORING_H__


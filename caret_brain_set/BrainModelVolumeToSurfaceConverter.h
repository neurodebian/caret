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


#ifndef __BRAIN_MODEL_VOLUME_TO_SURFACE_CONVERTER_H__
#define __BRAIN_MODEL_VOLUME_TO_SURFACE_CONVERTER_H__

#include "BrainModelAlgorithm.h"

class VolumeFile;

/// Class that converts a segmentation volume to a surface
class BrainModelVolumeToSurfaceConverter : public BrainModelAlgorithm {
   public:
      /// reconstruction mode
      enum RECONSTRUCTION_MODE {
         /// generate a brain model surface like SureFit
         RECONSTRUCTION_MODE_SUREFIT_SURFACE,
         /// generate a brain model surface like SureFit with no polygon reduction
         RECONSTRUCTION_MODE_SUREFIT_SURFACE_MAXIMUM_POLYGONS,
         /// generate a VTK model surface
         RECONSTRUCTION_MODE_VTK_MODEL,
         /// generate a VTK model surface with no polygon reduction
         RECONSTRUCTION_MODE_VTK_MODEL_MAXIMUM_POLYGONS,
         /// generate a model that creates a point for every voxel
         RECONSTRUCTION_MODE_SOLID_STRUCTURE
      };
      
      /// Constructor
      BrainModelVolumeToSurfaceConverter(BrainSet* bs,
                                         VolumeFile* segmentationVolumeFileIn,
                                         const RECONSTRUCTION_MODE reconstructionModeIn,
                                         const bool rightHemisphereFlagIn,
                                         const bool leftHemisphereFlagIn,
                                         const bool createHypersmoothSurfaceFlagIn = false);
      
      /// Destructor
      ~BrainModelVolumeToSurfaceConverter();
      
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
      
   private:
      /// generate the surefit surface
      void generateSureFitSurface(const bool maxPolygonsFlag) throw (BrainModelAlgorithmException);
      
      /// generate the surefit surface pro
      void generateSureFitSurfaceOld() throw (BrainModelAlgorithmException);
      
      /// generate the surface
      void generateSurface() throw (BrainModelAlgorithmException);
      
      /// generate a vtk model
      void generateVtkModel(const bool maxPolygonsFlag) throw (BrainModelAlgorithmException);
      
      /// generate a solid structure model
      void generateSolidStructure() throw (BrainModelAlgorithmException);
      
      /// the segmentation volume for converting to a surface
      VolumeFile* segmentationVolumeFile;
      
      /// right hemisphere flag
      bool rightHemisphereFlag;
      
      /// left hemisphere flag
      bool leftHemisphereFlag;

      /// the reconstruction mode
      RECONSTRUCTION_MODE reconstructionMode;
      
      /// create hypersmooth flag
      bool createHypersmoothSurfaceFlag;
};

#endif // __BRAIN_MODEL_VOLUME_TO_SURFACE_CONVERTER_H__


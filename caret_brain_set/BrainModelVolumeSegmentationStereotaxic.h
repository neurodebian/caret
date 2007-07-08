
#ifndef __BRAIN_MODEL_VOLUME_SEGMENTATION_STEREOTAXIC_H__
#define __BRAIN_MODEL_VOLUME_SEGMENTATION_STEREOTAXIC_H__

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

#include "BrainModelAlgorithm.h"
#include "Structure.h"

class VolumeFile;

/// class for automatic segmentation of anatomical volume in know stereotaxic space
class BrainModelVolumeSegmentationStereotaxic : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelVolumeSegmentationStereotaxic(BrainSet* brainSetIn,
                                              const VolumeFile* anatomicalVolumeFileIn,
                                              const int uniformityIterationsIn,
                                              const bool disconnectEyeFlagIn,
                                              const bool errorCorrectVolumeFlagIn,
                                              const bool errorCorrectSurfaceFlagIn,
                                              const bool maxPolygonsFlagIn,
                                              const bool flatteningFilesFlagIn);
                                              
      // destructor
      ~BrainModelVolumeSegmentationStereotaxic();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// get the default number of uniformity iterations
      static int getDefaultUniformityIterations() { return 0; }

   protected:
      /// generate the segmentation
      void generateSegmentation(const QString& maskVolumeFileName) throw (BrainModelAlgorithmException);
      
      /// get the segmentation mask volume file names
      void getSegmentationMaskVolumeFileNames(const QString& spaceName,
                                              const QString& structureName,
                                              QString& maskNameOut) throw (BrainModelAlgorithmException);
      
      /// write debugging volume
      void writeDebugVolume(VolumeFile& vf,
                            const QString& fileName);
                            
      /// name of anatomical volume
      const VolumeFile* anatomicalVolumeFile;
      
      /// error correct volume
      bool errorCorrectVolumeFlag;
      
      /// error correct surface
      bool errorCorrectSurfaceFlag;
      
      /// generate maximum polygons in surface
      bool maxPolygonsFlag;
      
      /// generate flattening files
      bool flatteningFilesFlag;
      
      /// number of uniformity iterations
      int uniformityIterations;
      
      /// disconnect the eye flag
      bool disconnectEyeFlag;
};

#endif // __BRAIN_MODEL_VOLUME_SEGMENTATION_STEREOTAXIC_H__


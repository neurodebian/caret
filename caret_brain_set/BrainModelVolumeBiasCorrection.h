
#ifndef __BRAIN_MODEL_VOLUME_BIAS_CORRECTION_H__
#define __BRAIN_MODEL_VOLUME_BIAS_CORRECTION_H__

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

#ifdef HAVE_ITK
#include "itkArray.h"
#endif // HAVE_ITK

#include "BrainModelAlgorithm.h"
#include "VolumeITKImage.h"

class VolumeFile;

/// class for performing bias correction on a volume file
class BrainModelVolumeBiasCorrection : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelVolumeBiasCorrection(VolumeFile* vf,
                                     const float grayWhiteValuesIn[4],
                                     const float lowerUpperThresholdsIn[2],
                                     const int   axisIterationsIn[3]);
                                     
      // destructor
      ~BrainModelVolumeBiasCorrection();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
#ifdef HAVE_ITK      
      // perform the bias correction
      void biasCorrection(VolumeITKImage::ImagePointer& input,
                          VolumeITKImage::ImagePointer& output,
                          const int sliceDirection,
                          const bool useLog);      
             
      /// determine means and variance
      void meansVariance(const bool useLog);

      /// energy function computation
      void energyFunctions() const;
      
      /// remove edges
      void remove_edges(VolumeITKImage::ImagePointer input, 
                        VolumeITKImage::ImagePointer& NoEdgeImage);

      /// means of gray and white
      itk::Array<double> classMeans;
      
      /// variance of gray and white
      itk::Array<double> classSigmas;
#endif // HAVE_ITK
      
      /// the volume
      VolumeFile* volumeFile;
      
      /// gray and white ranges
      float grayWhiteValues[4];
      
      /// iterations for each axis
      int axisIterations[3];
      
      /// lower and upper thresholds
      float lowerUpperThresholds[2];
};

#endif // __BRAIN_MODEL_VOLUME_BIAS_CORRECTION_H__


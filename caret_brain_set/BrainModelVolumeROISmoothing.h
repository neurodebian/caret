
#ifndef __BRAIN_MODEL_VOLUME_ROI_SMOOTHING_H__
#define __BRAIN_MODEL_VOLUME_ROI_SMOOTHING_H__

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
#include "VolumeFile.h"

// //class VolumeFile;

/// class for for smoothing and resampling a volume region to a given ROI
class BrainModelVolumeROISmoothing : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelVolumeROISmoothing(BrainSet* bs,
                                        VolumeFile* valuesIn,
                                        VolumeFile* regionIn,
                                        std::vector <VolumeFile *> *smoothVolVecIn,                                        
                                        float sigmaIn);
                                            
      /// Destructor
      ~BrainModelVolumeROISmoothing();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   private:
      VolumeFile* values, *region;
      std::vector<VolumeFile *> *smoothVolVec;
      float sigma;      
};

#endif // __BRAIN_MODEL_VOLUME_ROI_ATLAS_SMOOTHING_H__


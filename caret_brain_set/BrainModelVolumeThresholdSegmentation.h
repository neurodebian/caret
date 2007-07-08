#ifndef __BRAIN_MODEL_VOLUME_THRESHOLD_SEGMENTATION_H__
#define __BRAIN_MODEL_VOLUME_THRESHOLD_SEGMENTATION_H__

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

class VolumeFile;

/// class for performing threshold segmentation 
class BrainModelVolumeThresholdSegmentation : public BrainModelAlgorithm {
   public:
      
      // Constructor
      BrainModelVolumeThresholdSegmentation(BrainSet* bs,
                                            VolumeFile* anatomyVolumeIn,
                                            const float thresholdIn[2]);
                                   
                                   
      // Destructor
      ~BrainModelVolumeThresholdSegmentation();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// the anatomy volume
      VolumeFile* anatomyVolume;
      
      /// the threshold to use
      float threshold[2];
};

#endif // __BRAIN_MODEL_VOLUME_THRESHOLD_SEGMENTATION_H__


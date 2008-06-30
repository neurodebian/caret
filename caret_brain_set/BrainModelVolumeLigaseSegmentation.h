
#ifndef __BRAIN_MODEL_VOLUME_LIGASE_SEGMENTATION_H__
#define __BRAIN_MODEL_VOLUME_LIGASE_SEGMENTATION_H__

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

/// class for create a functional volume using a probabilistic volume
class BrainModelVolumeLigaseSegmentation : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelVolumeLigaseSegmentation(BrainSet* bs,
                                            VolumeFile* anatVolumeIn,
                                            VolumeFile* segVolumeIn,
                                            const QString& segVolumeNameIn,
                                            const QString& segVolumeLabelIn,
                                            const int xIn,
                                            const int yIn,
                                            const int zIn,
                                            const float whiteMinIn,
                                            const float whiteMeanIn,
                                            const float whiteMaxIn,
                                            const float diffBaseIn = 0.12f,
                                            const float gradBaseIn = 0.05f);
                                            
      /// Destructor
      ~BrainModelVolumeLigaseSegmentation();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      ///default parameters
      static inline const float defaultDiff() { return 0.12f; };
      static inline const float defaultGrad() { return 0.05f; };
   protected:
      /// segmentation volume, anatomy input volume
      VolumeFile* segVolume;
      VolumeFile* anatVolume;
      
      /// segmentation volume name
      QString segVolumeName;
      
      /// segmentation volume label
      QString segVolumeLabel;
      
      float whiteMin, whiteMean, whiteMax, diffBase, gradBase;
      int x_init, y_init, z_init;
      struct iterNode
      {
         iterNode* next;
         float* iter;
      };
};

#endif // __BRAIN_MODEL_VOLUME_LIGASE_SEGMENTATION_H__


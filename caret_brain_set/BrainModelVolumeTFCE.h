
#ifndef __BRAIN_MODEL_VOLUME_TFCE_H__
#define __BRAIN_MODEL_VOLUME_TFCE_H__

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
class BrainModelVolumeTFCE : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelVolumeTFCE(BrainSet* bs,
                                            VolumeFile* inFuncVolumeIn,
                                            VolumeFile* outFuncVolumeIn,
                                            const QString& outVolumeNameIn,
                                            const QString& outVolumeLabelIn,
                                            const int numStepsIn = 50,
                                            const float EIn = 0.5f,
                                            const float HIn = 2.0f);
                                            
      /// Destructor
      ~BrainModelVolumeTFCE();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      ///default parameters
      static inline const int defaultNumSteps() { return 50; };
      static inline const float defaultE() { return 0.5f; };
      static inline const float defaultH() { return 2.0f; };
      static inline int min(int a, int b) { return (a > b ? b : a); };
      static inline int max(int a, int b) { return (a > b ? a : b); };
   protected:
      /// segmentation volume, anatomy input volume
      VolumeFile* outFuncVolume;
      VolumeFile* inFuncVolume;
      
      /// segmentation volume name
      QString outVolumeName;
      
      /// segmentation volume label
      QString outVolumeLabel;
      
      /// parameter storage
      float H, E;
      int numSteps;
};

#endif // __BRAIN_MODEL_VOLUME_TFCE_H__


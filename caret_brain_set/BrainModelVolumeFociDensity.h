
#ifndef __BRAIN_MODEL_VOLUME_FOCI_DENSITY_H__
#define __BRAIN_MODEL_VOLUME_FOCI_DENSITY_H__

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

class FociProjectionFile;
class VolumeFile;

/// class for generating volume foci density
class BrainModelVolumeFociDensity : public BrainModelAlgorithm {
   public:
      // type of density units
      enum DENSITY_UNITS {
         /// foci per cubic centimeter
         DENSITY_UNITS_FOCI_PER_CUBIC_CENTIMETER,
         /// foci per cubic millimeter
         DENSITY_UNITS_FOCI_PER_CUBIC_MILLIMETER
      };
      
      // constructor
      BrainModelVolumeFociDensity(BrainSet* bsIn,
                                  const FociProjectionFile* fociProjectionFileIn,
                                  const float regionCubeSizeIn,
                                  const DENSITY_UNITS densityUnitsIn,
                                  VolumeFile* outputVolumeFileIn);
      
      // destructor
      ~BrainModelVolumeFociDensity();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// the foci projection file
      const FociProjectionFile* fociProjectionFile;
            
      /// the region cube size
      const float regionCubeSize;

      /// the density units
      const DENSITY_UNITS densityUnits;

      /// the output volume file
      VolumeFile* outputVolumeFile;
      
};

#endif // __BRAIN_MODEL_VOLUME_FOCI_DENSITY_H__

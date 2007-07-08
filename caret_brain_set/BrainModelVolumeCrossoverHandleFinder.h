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



#ifndef __BRAIN_MODEL_VOLUME_CROSSOVER_HANDLE_FINDER_H__
#define __BRAIN_MODEL_VOLUME_CROSSOVER_HANDLE_FINDER_H__

#include <set>
#include <vector>

#include "BrainModelAlgorithm.h"
#include "BrainModelVolumeTopologicalError.h"

class BrainSet;

#include "VolumeFile.h"

/// class that finds handles in a binary volume
class BrainModelVolumeCrossoverHandleFinder : public BrainModelAlgorithm {
   public:
      /// status result
      enum STATUS {
         /// no handles
         STATUS_NO_HANDLES,
         /// has handles
         STATUS_HAS_HANDLES
      };
      
      /// Constructor
      BrainModelVolumeCrossoverHandleFinder(BrainSet* bs,
                                   const VolumeFile* segmentationIn,
                                   const bool addHandlesVolumeToBrainSetIn,
                                   const bool rgbPaintSurfaceHandlesIn);
      
      /// Destructor
      ~BrainModelVolumeCrossoverHandleFinder();
      
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// get the result status
      STATUS getResultStatus() const { return status; }
      
      /// get the number of handles found
      int getNumberOfHandles() const { return handlesFound.size(); }
      
      /// get the information about a handle
      const BrainModelVolumeTopologicalError* getHandleInfo(const int handleNumber) const { 
         return &handlesFound[handleNumber]; 
      }
      
   private:
      /// highlight handles in the surface using RGB Paint
      void highlightHandlesInSurface();
      
      /// create a list of the handles in voxel IJK
      void createVoxelHandleList();
      
      /// result status
      STATUS status;
      
      /// brain volume for searching for handles
      VolumeFile* segmentationVolume;
      
      /// crossovers volume
      VolumeFile* crossoversVolume;
      
      /// brain volume showing handles
      VolumeFile* handlesRgbVolume;
      
      /// x dimensions of volume
      int volumeDimX;
      
      /// x dimensions of volume
      int volumeDimY;
      
      /// x dimensions of volume
      int volumeDimZ;
      
      /// keeps track of handles that are found
      std::vector<BrainModelVolumeTopologicalError> handlesFound;
      
      /// create the handles volume
      bool addHandlesVolumeToBrainSet;
      
      /// paint nodes around handles with rgb paint file
      bool rgbPaintSurfaceHandles;
};

#endif // __BRAIN_MODEL_VOLUME_CROSSOVER_HANDLE_FINDER_H__


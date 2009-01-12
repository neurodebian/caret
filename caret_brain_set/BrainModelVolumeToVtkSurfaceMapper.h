
#ifndef __BRAIN_MODEL_VOLUME_TO_VTK_SURFACE_MAPPER_H__
#define __BRAIN_MODEL_VOLUME_TO_VTK_SURFACE_MAPPER_H__

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

class PaletteFile;
class VolumeFile;
class VtkModelFile;

/// class for mapping a volume to a VTK surface
class BrainModelVolumeToVtkSurfaceMapper : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelVolumeToVtkSurfaceMapper(BrainSet* bs,
                             VtkModelFile* vtkModelFileIn,
                             const VolumeFile* volumeFileIn,
                             const PaletteFile* paletteFileIn,
                             const int paletteNumberIn);
      
      // destructor
      ~BrainModelVolumeToVtkSurfaceMapper();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// the VTK model file
      VtkModelFile* vtkModelFile;
      
      /// the volume that is to be mapped
      const VolumeFile* volumeFile;
      
      /// the palette file
      const PaletteFile* paletteFile;
      
      /// palette number
      const int paletteNumber;
};

#endif // __BRAIN_MODEL_VOLUME_TO_VTK_SURFACE_MAPPER_H__

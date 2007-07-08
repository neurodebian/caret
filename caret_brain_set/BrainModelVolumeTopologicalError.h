
#ifndef __BRAIN_MODEL_VOLUME_TOPOLOGICAL_ERROR_H__
#define __BRAIN_MODEL_VOLUME_TOPOLOGICAL_ERROR_H__

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

#include "VolumeFile.h"

/// stores information about a topological error (handle)
class BrainModelVolumeTopologicalError {
   public:
      /// Constructor
      BrainModelVolumeTopologicalError(const int voxelIJKIn[3],
             const int slicesIn[2],
             const std::vector<int>& handleVoxelsIn,
             const int numVoxelsIn,
             const VolumeFile::VOLUME_AXIS axisIn);
             
      /// Constructor
      BrainModelVolumeTopologicalError(const int voxelIJKIn[3],
                                       const int numVoxelsIn);
             
      /// get the handle info
      void getInfo(int voxelIJKOut[3],
                   int slicesOut[2],
                   int& numVoxelsOut,
                   VolumeFile::VOLUME_AXIS& axisOut) const;
                          
      /// get the voxels that make up the handle (one dimensional indices)
      void getHandleVoxels(std::vector<int>& handleVoxelsOut) const;
      
   protected:
      /// a voxel in the handle
      int voxelIJK[3];
      
      /// starting and ending slice of the handle
      int slices[2];
      
      /// number of voxels in the handle
      int numVoxels;
      
      /// axis along which voxel was found
      VolumeFile::VOLUME_AXIS axis;
      
      /// indices to voxels that make up the handle
      std::vector<int> handleVoxels;
};

#endif // __BRAIN_MODEL_VOLUME_TOPOLOGICAL_ERROR_H__

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

#include "BrainModelVolumeTopologicalError.h"

/**
 * Constructor.
 */
BrainModelVolumeTopologicalError::BrainModelVolumeTopologicalError(const int voxelIJKIn[3],
                                                    const int slicesIn[2],
                                                    const std::vector<int>& handleVoxelsIn,
                                                    const int numVoxelsIn,
                                                    const VolumeFile::VOLUME_AXIS axisIn)
{
   voxelIJK[0]  = voxelIJKIn[0];
   voxelIJK[1]  = voxelIJKIn[1];
   voxelIJK[2]  = voxelIJKIn[2];
   slices[0]    = slicesIn[0];
   slices[1]    = slicesIn[1];
   handleVoxels = handleVoxelsIn;
   numVoxels    = numVoxelsIn;
   axis         = axisIn;
}
       
/**
 * Constructor.
 */
BrainModelVolumeTopologicalError::BrainModelVolumeTopologicalError(const int voxelIJKIn[3],
                                                                   const int numVoxelsIn)
{
   voxelIJK[0]  = voxelIJKIn[0];
   voxelIJK[1]  = voxelIJKIn[1];
   voxelIJK[2]  = voxelIJKIn[2];
   slices[0]    = -1;
   slices[1]    = -1;
   handleVoxels.clear();
   numVoxels    = numVoxelsIn;
   axis         = VolumeFile::VOLUME_AXIS_UNKNOWN;
}
       
/**
 * get the handle info.
 */
void 
BrainModelVolumeTopologicalError::getInfo(int voxelIJKOut[3],
                                          int slicesOut[2],
                                          int& numVoxelsOut,
                                          VolumeFile::VOLUME_AXIS& axisOut) const
{
   voxelIJKOut[0] = voxelIJK[0];
   voxelIJKOut[1] = voxelIJK[1];
   voxelIJKOut[2] = voxelIJK[2];
   slicesOut[0]   = slices[0];
   slicesOut[1]   = slices[1];
   numVoxelsOut   = numVoxels;
   axisOut        = axis;
}
                    
/**
 * get the voxels that make up the handle (one dimensional indices).
 */
void
BrainModelVolumeTopologicalError::getHandleVoxels(std::vector<int>& handleVoxelsOut) const
{
   handleVoxelsOut = handleVoxels;
}      

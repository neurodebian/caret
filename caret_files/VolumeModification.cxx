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
#include "VolumeModification.h"

/**
 * Constructor.
 */
VolumeModification::VolumeModification()
{
}

/**
 * Destructor.
 */
VolumeModification::~VolumeModification()
{
   voxels.clear();
}

/**
 * add a voxel.
 */
void 
VolumeModification::addVoxel(VolumeFile* vf, const int ijk[3], VolumeFile* colorVolumeIn)
{
   if (vf->getVoxelIndexValid(ijk)) {
      VolumeFile* colorVolume = vf;
      if (colorVolumeIn != NULL) {
         colorVolume = colorVolumeIn;
      }
      float value = colorVolume->getVoxel(ijk);
      unsigned char color[4];
      colorVolume->getVoxelColor(ijk, color);
      addVoxel(VoxelModified(ijk, value, color));
   }
}

/**
 * add a voxel.
 */
void 
VolumeModification::addVoxel(VolumeFile* vf, const int i, const int j, const int k,
                             VolumeFile* colorVolume)
{
   const int ijk[3] = { i, j, k };
   addVoxel(vf, ijk, colorVolume);
}

/**
 * add a voxel.
 */
void 
VolumeModification::addVoxel(const VoxelModified& vm)
{
   voxels.push_back(vm);
}

/**
 * get modified voxels.
 */
const VoxelModified* 
VolumeModification::getModifiedVoxel(const int indx) const
{
   return &voxels[indx];
}

/**
 * get the memory required to store the modification.
 */
int 
VolumeModification::getMemorySize() const
{
   const int memSize = getNumberOfVoxels() * sizeof(VoxelModified)
                     + sizeof(std::vector<VoxelModified>);
   return memSize;
}
      

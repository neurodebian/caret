#ifndef __VOLUME_MODIFICATION_H__
#define __VOLUME_MODIFICATION_H__

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

#include <vector>

class VolumeFile;

/// This class tracks the change to a single voxel
class VoxelModified {
   public:
            /// constructor
            VoxelModified(const int ijkIn[3],
                          const float valueIn,
                          const unsigned char colorIn[4]) {
               ijk[0] = ijkIn[0];
               ijk[1] = ijkIn[1];
               ijk[2] = ijkIn[2];
               value = valueIn;
               color[0] = colorIn[0];
               color[1] = colorIn[1];
               color[2] = colorIn[2];
               color[3] = colorIn[3];
            }
            
            /// destructor
            ~VoxelModified() { }
            
            /// get the voxel's IJK index
            const int* getIJK() const { return ijk; }
            
            /// get the voxels value
            const float getVoxelValue() const { return value; }
            
            /// get the voxel's coloring
            const unsigned char* getVoxelColor() const { return color; }
            
   protected:
            /// voxel indices
            int ijk[3];
            
            /// value of voxel
            float value;
            
            /// color of voxel
            unsigned char color[4];   
   
   friend class VolumeModification;
};

/// This class tracks voxels changed when a volume is modified
class VolumeModification {
   public:
      /// Constructor
      VolumeModification();
      
      /// Destructor
      ~VolumeModification();
      
      /// add a voxel (use colorVolumeIn if you want the voxels and color take from another volume)
      void addVoxel(VolumeFile* vf, const int ijk[3],
                    VolumeFile* colorVolumeIn = NULL);
      
      /// add a voxel (use colorVolumeIn if you want the voxels and color take from another volume)
      void addVoxel(VolumeFile* vf, const int i, const int j, const int k,
                    VolumeFile* colorVolumeIn = NULL);
      
      /// add a voxel
      void addVoxel(const VoxelModified& vm);
      
      /// get the number of voxels in this change
      int getNumberOfVoxels() const { return voxels.size(); }

      /// get modified voxels
      const VoxelModified* getModifiedVoxel(const int indx) const;

      /// get the memory required to store the modification
      int getMemorySize() const;
      
   protected:
      /// voxels involved in this change
      std::vector<VoxelModified> voxels;
};

#endif // __VOLUME_MODIFICATION_H__

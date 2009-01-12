
#ifndef __VOXEL_IJK_H__
#define __VOXEL_IJK_H__

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

/// Class for storing voxel indices
class VoxelIJK {
   public:
      /// Constructor
      VoxelIJK() {
         ijkv[0] = -1;
         ijkv[1] = -1;
         ijkv[2] = -1;
      }
      
      /// Constructor
      VoxelIJK(const int i, const int j, const int k) {
         ijkv[0] = i; 
         ijkv[1] = j;
         ijkv[2] = k;
      }  
      /// Constructor
      VoxelIJK(const int ijkIn[3]) {
         ijkv[0] = ijkIn[0];
         ijkv[1] = ijkIn[1];
         ijkv[2] = ijkIn[2];
      }  

      /// get the voxel indices
      const int* getIJK() const { return &ijkv[0]; }
      
      /// get the I component
      int getI() const { return ijkv[0]; }
      
      /// get the J component
      int getJ() const { return ijkv[1]; }
      
      /// get the K component
      int getK() const { return ijkv[2]; }
      
      /// get the voxel indices
      void getIJK(int& i, int& j, int& k) const {
         i = ijkv[0];
         j = ijkv[1];
         k = ijkv[2];
      }

      /// get the voxel indices
      void getIJK(int ijkOut[3]) const {
         ijkOut[0] = ijkv[0];
         ijkOut[1] = ijkv[1];
         ijkOut[2] = ijkv[2];
      }

      /// set the voxel indices
      void setIJK(const int ijkIn[3]) { 
         ijkv[0] = ijkIn[0]; 
         ijkv[1] = ijkIn[1]; 
         ijkv[2] = ijkIn[2]; 
      }
      
      /// get the voxel indices
      void setIJK(const int i, const int j, const int k) {
         ijkv[0] = i;
         ijkv[1] = j;
         ijkv[2] = k;
      }

   protected:
      /// the voxel indices
      int ijkv[3];
      
   friend class VolumeFile;
};

#endif // __VOXEL_IJK_H__

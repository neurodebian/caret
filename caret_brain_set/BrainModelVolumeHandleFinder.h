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



#ifndef __BRAIN_MODEL_VOLUME_HANDLE_FINDER_H__
#define __BRAIN_MODEL_VOLUME_HANDLE_FINDER_H__

#include <set>
#include <vector>

#include "BrainModelAlgorithm.h"
#include "BrainModelVolumeTopologicalError.h"

class BrainSet;

#include "VolumeFile.h"

/// class that finds handles in a binary volume
class BrainModelVolumeHandleFinder : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelVolumeHandleFinder(BrainSet* bs,
                                   VolumeFile* segmentationIn,
                                   const bool addHandlesVolumeToBrainSetIn,
                                   const bool searchAxisXIn,
                                   const bool searchAxisYIn,
                                   const bool searchAxisZIn,
                                   const bool rgbPaintSurfaceHandlesIn);
      
      /// Destructor
      ~BrainModelVolumeHandleFinder();
      
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// get the number of handles found
      int getNumberOfHandles() const { return handlesFound.size(); }
      
      /// get the information about a handle
      const BrainModelVolumeTopologicalError* getHandleInfo(const int handleNumber) const { 
         return &handlesFound[handleNumber]; 
      }
      
   private:
      /// voxel values used while searching
      enum {
         VOXEL_CAVITY       = 200,
         VOXEL_EXTERIOR     = 100,
         VOXEL_HANDLE       = 150,
         VOXEL_NOT_HANDLE   = 175,
         VOXEL_SEGMENTATION = 255,
         VOXEL_UNSET        =   0
      };
      
      /// Class for storing voxel indices used while searching a volume
      class VoxelIJK {
         public:
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
         
            /// get the voxel indices
            void getIJK(int& i, int& j, int& k) const {
               i = ijkv[0];
               j = ijkv[1];
               k = ijkv[2];
            }
            
         private:
            /// the voxel indices         
            int ijkv[3];
      };
      
      /// intialize the exterior voxels
      void initialVoxelAssignments(const VolumeFile::VOLUME_AXIS searchAxis);
      
      /// Flood fill in 2D all 4-connected neighbors orthogonal specified axis.
      void floodFill4Connected(const VolumeFile::VOLUME_AXIS searchAxis,
                        const int x, const int y, const int z,
                        const int replace, const int replaceWith);
      
      /// Find handles along the specified axis                  
      void findHandles(const VolumeFile::VOLUME_AXIS searchAxis);
      
      /// determine if VOXEL_UNSET voxels should be VOXEL_HANDLE
      void findHandleSearch(const VolumeFile::VOLUME_AXIS searchAxis,
                     const int x, const int y, const int z);
      
      /// clear the visited voxel flags
      void clearVisitedVoxels();
      
      /// highlight handles in the surface using RGB Paint
      void highlightHandlesInSurface();
      
      /// brain volume for searching for handles
      VolumeFile* segmentationVolume;
      
      /// brain volume showing handles
      VolumeFile* handlesVolume;
      
      /// x dimensions of volume
      int volumeDimX;
      
      /// x dimensions of volume
      int volumeDimY;
      
      /// x dimensions of volume
      int volumeDimZ;

      /// points to voxels currently being searched (do not delete)
      unsigned char* voxels;
      
      /// total number of voxels
      int numVoxels;
      
      /// slice of external voxels found while searching for handles
      std::set<int> externalVoxelSlice;

      /// visited voxel flag used while searching
      int* visitedVoxels;
      
      /// keeps track of handles that are found
      std::vector<BrainModelVolumeTopologicalError> handlesFound;
      
      /// search along the x axis for handles
      bool searchAxisX;

      /// search along the y axis for handles
      bool searchAxisY;

      /// search along the z axis for handles
      bool searchAxisZ;
      
      /// create the handles volume
      bool addHandlesVolumeToBrainSet;
      
      /// paint nodes around handles with rgb paint file
      bool rgbPaintSurfaceHandles;
};

#endif // __BRAIN_MODEL_VOLUME_HANDLE_FINDER_H__



#include "BrainModelVolumeFociDensity.h"
#include "FociProjectionFile.h"
#include "VolumeFile.h"

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

#include "BrainModelVolumeFociDensity.h"
#include "FociProjectionFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelVolumeFociDensity::BrainModelVolumeFociDensity(BrainSet* bsIn,
                                  const FociProjectionFile* fociProjectionFileIn,
                                  const float regionCubeSizeIn,
                                  VolumeFile* outputVolumeFileIn)
   : BrainModelAlgorithm(bsIn),
     fociProjectionFile(fociProjectionFileIn),
     regionCubeSize(regionCubeSizeIn),
     outputVolumeFile(outputVolumeFileIn)
{
}                            

/**
 * destructor.
 */
BrainModelVolumeFociDensity::~BrainModelVolumeFociDensity()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeFociDensity::execute() throw (BrainModelAlgorithmException)
{
   //
   // Check dimensions
   //
   if (outputVolumeFile == NULL) {
      throw BrainModelAlgorithmException("The volume file is invalid");
   }
   if (outputVolumeFile->getTotalNumberOfVoxels() <= 0) {
      throw BrainModelAlgorithmException("The volume contains no voxels.");
   }
   if (fociProjectionFile == NULL) {
      throw BrainModelAlgorithmException("The foci projection file is invalid");
   }
   const int numFoci = fociProjectionFile->getNumberOfCellProjections();
   if (numFoci <= 0) {
      throw BrainModelAlgorithmException("The foci projection file contains no foci.");
   }
   if (regionCubeSize <= 0.0) {
      throw BrainModelAlgorithmException("Region cube size is less than or equal to zero.");
   }
   
   //
   // Half the cube size
   //
   const float halfRegionCubeSize = regionCubeSize * 0.5;
   
   //
   // Set all voxels to zero
   //
   outputVolumeFile->setAllVoxels(0.0);
   
   //
   // Create progress dialog
   //
   createProgressDialog("Volume Foci Density",
                        numFoci,
                        "volumeFociDensity");
                        
   //
   // Loop through the foci
   //
   for (int n = 0; n < numFoci; n++) {
      //
      // Update progress dialog
      //
      const float pctComplete = (static_cast<float>(n) 
                                 / numFoci) * 100.0;
      const QString pctString(QString::number(pctComplete, 'f', 1)
                              + "% complete.");
      updateProgressDialog(pctString);
      
      //
      // Get the stereotaxic coordinate
      //
      float focusXYZ[3];
      fociProjectionFile->getCellProjection(n)->getVolumeXYZ(focusXYZ);
      if ((focusXYZ[0] != 0.0) ||
          (focusXYZ[1] != 0.0) ||
          (focusXYZ[2] != 0.0)) {
         //
         // place the ROI box at the focus and find its corners
         //
         const float minCornerXYZ[3] = {
            focusXYZ[0] - halfRegionCubeSize,
            focusXYZ[1] - halfRegionCubeSize,
            focusXYZ[2] - halfRegionCubeSize
         };
         const float maxCornerXYZ[3] = {
            focusXYZ[0] + halfRegionCubeSize,
            focusXYZ[1] + halfRegionCubeSize,
            focusXYZ[2] + halfRegionCubeSize
         };
         
         //
         // Get voxel indices containing corners
         //
         int minIJK[3], maxIJK[3];
         outputVolumeFile->convertCoordinatesToVoxelIJK(minCornerXYZ, minIJK);
         outputVolumeFile->convertCoordinatesToVoxelIJK(maxCornerXYZ, maxIJK);
         
         //
         // Limit to valid indices
         //
         outputVolumeFile->clampVoxelIndex(minIJK);
         outputVolumeFile->clampVoxelIndex(maxIJK);
         
         //
         // Increment the voxels
         //
         for (int i = minIJK[0]; i <= maxIJK[0]; i++) {
            for (int j = minIJK[1]; j <= maxIJK[1]; j++) {
               for (int k = minIJK[2]; k <= maxIJK[2]; k++) {
                  outputVolumeFile->setVoxel(i, j, k, 0,
                                             (outputVolumeFile->getVoxel(i, j, k) + 1));
               }
            }
         }
      }
   }
   
   //
   // Scale to foci per mm^3
   //
   int dim[3];
   outputVolumeFile->getDimensions(dim);
   float voxelSize[3];
   outputVolumeFile->getSpacing(voxelSize);
   const float mm3voxel = (regionCubeSize * regionCubeSize * regionCubeSize)
                        * (voxelSize[0] * voxelSize[1] * voxelSize[2]);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            outputVolumeFile->setVoxel(i, j, k, 0,
                                       (outputVolumeFile->getVoxel(i, j, k) / mm3voxel));
         }
      }
   }
   
   removeProgressDialog();
}

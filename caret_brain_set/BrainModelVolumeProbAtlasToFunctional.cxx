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

#include "BrainModelVolume.h"
#include "BrainModelVolumeProbAtlasToFunctional.h"
#include "BrainSet.h"
#include "DisplaySettingsProbabilisticAtlas.h"
#include "VolumeFile.h"

/**
 * Constructor.
 */
BrainModelVolumeProbAtlasToFunctional::BrainModelVolumeProbAtlasToFunctional(
                                               BrainSet* bs,
                                               VolumeFile* funcVolumeIn,
                                               const QString& funcVolumeNameIn,
                                               const QString& funcVolumeLabelIn)
   : BrainModelAlgorithm(bs)
{
   funcVolume      = funcVolumeIn;
   funcVolumeName  = funcVolumeNameIn;
   funcVolumeLabel = funcVolumeLabelIn;
}
                                      
/**
 * Destructor.
 */
BrainModelVolumeProbAtlasToFunctional::~BrainModelVolumeProbAtlasToFunctional()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeProbAtlasToFunctional::execute() throw (BrainModelAlgorithmException)
{
   //
   // Verify volumes exist
   //
   const int numberOfVolumes = brainSet->getNumberOfVolumeProbAtlasFiles();
   if (numberOfVolumes <= 0) {
      throw BrainModelAlgorithmException("There are no probabilistic atlas volumes.");
   }
   VolumeFile* probAtlasVolume = brainSet->getVolumeProbAtlasFile(0);
   if (probAtlasVolume == NULL) {
      throw BrainModelAlgorithmException("Invalid probabilistic atlas volume.");
   }
   bool createdFunctionalVolume = false;
   if (funcVolume == NULL) {
      funcVolume = new VolumeFile(*probAtlasVolume);
      funcVolume->setVolumeType(VolumeFile::VOLUME_TYPE_FUNCTIONAL);
      createdFunctionalVolume = true;
   }
   funcVolume->setFileName(funcVolumeName);
   funcVolume->setDescriptiveLabel(funcVolumeLabel);
   
   //
   // Verify volumes have same dimensions
   //
   int pDim[3], fDim[3];
   probAtlasVolume->getDimensions(pDim);
   funcVolume->getDimensions(fDim);
   for (int i = 0; i < 3; i++) {
      if (pDim[i] != fDim[i]) {
         throw BrainModelAlgorithmException(
            "Prob Atlas and Functional Volumes are of different dimensions.");
      }
   }
   
   DisplaySettingsProbabilisticAtlas* dspa = brainSet->getDisplaySettingsProbabilisticAtlasVolume();
   //BrainModelVolume* bmv = brainSet->getBrainModelVolume();
   const int numPaintNames = probAtlasVolume->getNumberOfRegionNames();
   
   //
   // indices that should be ignored
   //
   const int questIndex = probAtlasVolume->getRegionIndexFromName("???");
   const int gyralIndex = probAtlasVolume->getRegionIndexFromName("GYRAL");
   const int gyrusIndex = probAtlasVolume->getRegionIndexFromName("GYRUS");

    
   //
   // Loop through voxels
   // 
   for (int i = 0; i < pDim[0]; i++) {
      for (int j = 0; j < pDim[1]; j++) {
         for (int k = 0; k < pDim[2]; k++) {

            //
            // Count valid indices
            //
            int count = 0;
            for (int volNum = 0; volNum < numberOfVolumes; volNum++) {
               VolumeFile* vf = brainSet->getVolumeProbAtlasFile(volNum);
               
               int cntIndex = 0;
               if (dspa->getChannelSelected(volNum)) {
                  const int voxel = static_cast<int>(vf->getVoxel(i, j, k));
                  cntIndex = voxel; //vf->getProbAtlasNameTableIndex(volNum, voxel);
               }
               if ((cntIndex > 0) && (cntIndex < numPaintNames)) {
                  if (dspa->getAreaSelected(cntIndex) == false) {
                     cntIndex = -1;
                  }
               }
               if (cntIndex > 0) {
                  //
                  // Skip non-sulci
                  //
                  bool useIt = true;
                  if ((cntIndex == questIndex) ||
                      (cntIndex == gyralIndex) ||
                      (cntIndex == gyrusIndex)) {
                     useIt = false;
                  }
                  
                  if (useIt) {
                     count++;
                  }
               } 
            } // for (int volNum...
            
            //
            // Set the functional voxel
            //
            funcVolume->setVoxel(i, j, k, 0, count);
         }
      }
   }
   
   //
   // Add new functional volume to brain set
   //
   if (createdFunctionalVolume) {
      brainSet->addVolumeFile(VolumeFile::VOLUME_TYPE_FUNCTIONAL,
                              funcVolume,
                              funcVolume->getFileName(),
                              true,
                              false);
   }
   
   funcVolume->setVoxelColoringInvalid();
}

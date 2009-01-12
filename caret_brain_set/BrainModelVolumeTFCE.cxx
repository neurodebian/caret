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
#include "BrainModelVolumeTFCE.h"
#include "BrainSet.h"
#include "VolumeFile.h"
#include <cmath>

/**
 * Constructor.
 */
BrainModelVolumeTFCE::BrainModelVolumeTFCE(BrainSet* bs,
                                            VolumeFile* inFuncVolumeIn,
                                            VolumeFile* outFuncVolumeIn,
                                            const QString& outVolumeNameIn,
                                            const QString& outVolumeLabelIn,
                                            const int numStepsIn,
                                            const float EIn,
                                            const float HIn)
   : BrainModelAlgorithm(bs)
{
   inFuncVolume = inFuncVolumeIn;
   outFuncVolume = outFuncVolumeIn;
   outVolumeName = outVolumeNameIn;
   outVolumeLabel = outVolumeLabelIn;
   numSteps = numStepsIn;
   E = EIn;
   H = HIn;
}
                                      
/**
 * Destructor.
 */
BrainModelVolumeTFCE::~BrainModelVolumeTFCE()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeTFCE::execute() throw (BrainModelAlgorithmException)
{
   //
   // Verify volumes exist
   //
   if (inFuncVolume == NULL) {
      throw BrainModelAlgorithmException("Invalid input volume.");
   }
   if (inFuncVolume->getNumberOfComponentsPerVoxel() != 1)
   {
      throw BrainModelAlgorithmException("Volume has multiple components.");
   }
   bool createdOutVolume = false;
   if (outFuncVolume == NULL) {
      outFuncVolume = new VolumeFile(*inFuncVolume);
      createdOutVolume = true;
   }
   outFuncVolume->setFileName(outVolumeName);
   outFuncVolume->setDescriptiveLabel(outVolumeLabel);

   //
   // Verify volumes have same dimensions
   //
   int aDim[3], sDim[3];
   inFuncVolume->getDimensions(aDim);
   outFuncVolume->getDimensions(sDim);
   for (int i = 0; i < 3; i++) {
      if (aDim[i] != sDim[i]) {
         throw BrainModelAlgorithmException(
            "Input and Output Volumes are of different dimensions.");
      }
   }
   int i, j, k, ti, tj, tk, maxi, maxj, maxk, mini, minj, mink, temp, growingInd, growingCur, numVoxels = aDim[0] * aDim[1] * aDim[2];
   float fmax = 0.0f, thresh, valToAdd;
   float* voxels = inFuncVolume->getVoxelData(), *outData = outFuncVolume->getVoxelData();
   bool* flagUsed = new bool[numVoxels];//bitwise saves little space, due to other arrays, so make it simple
   short* growing = new short[numVoxels * 3];//excessive array for storing clusters, just in case (and in case indices range outside 0-255)
   for (temp = 0; temp < numVoxels; ++temp)
   {
      if (voxels[temp] > fmax) fmax = voxels[temp];
      outData[temp] = 0.0f;
   }
   for (thresh = fmax / numSteps / 2.0; thresh < fmax; thresh += fmax / numSteps)
   {
      for (temp = 0; temp < numVoxels; ++temp) flagUsed[temp] = false;
      for (k = 0; k < aDim[2]; ++k)
      {
         for (j = 0; j < aDim[1]; ++j)
         {
            for (i = 0; i < aDim[0]; ++i)
            {
               temp = inFuncVolume->getVoxelDataIndex(i, j, k);
               if (!flagUsed[temp] && voxels[temp] >= thresh)
               {
                  flagUsed[temp] = true;
                  growingInd = 3;
                  growingCur = 0;
                  growing[0] = i;
                  growing[1] = j;
                  growing[2] = k;
                  while (growingCur < growingInd)
                  {
                     maxi = min(aDim[0], growing[growingCur] + 2);
                     maxj = min(aDim[1], growing[growingCur + 1] + 2);
                     maxk = min(aDim[2], growing[growingCur + 2] + 2);
                     mini = max(0, growing[growingCur] - 1);
                     minj = max(0, growing[growingCur + 1] - 1);
                     mink = max(0, growing[growingCur + 2] - 1);
                     for (tk = mink; tk < maxk; ++tk)
                     {
                        for (tj = minj; tj < maxj; ++tj)
                        {
                           for (ti = mini; ti < maxi; ++ti)
                           {
                              temp = inFuncVolume->getVoxelDataIndex(ti, tj, tk);
                              if (!flagUsed[temp] && voxels[temp] >= thresh)
                              {
                                 flagUsed[temp] = true;
                                 growing[growingInd] = ti;
                                 growing[growingInd + 1] = tj;
                                 growing[growingInd + 2] = tk;
                                 growingInd += 3;
                              }
                           }
                        }
                     }
                     growingCur += 3;
                  }
                  growingCur = 0;
                  valToAdd = std::pow(growingInd / 3.0f, E) * std::pow(thresh, H) * fmax / numSteps;// e(h)^E * h^H * dh
                  //NOTE: integral approximation can be improved using standard weighting techniques, currently uses center of piece method
                  while (growingCur < growingInd)
                  {
                     outData[outFuncVolume->getVoxelDataIndex(growing[growingCur], growing[growingCur + 1], growing[growingCur + 2])] += valToAdd;
                     growingCur += 3;
                  }
               }
            }
         }
      }
   }
   if (createdOutVolume) {
      brainSet->addVolumeFile(VolumeFile::VOLUME_TYPE_SEGMENTATION,
                              outFuncVolume,
                              outFuncVolume->getFileName(),
                              true,
                              false);
   }
   outFuncVolume->setVoxelColoringInvalid();
   delete[] flagUsed;
}

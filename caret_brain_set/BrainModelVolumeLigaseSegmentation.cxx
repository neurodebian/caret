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
#include "BrainModelVolumeLigaseSegmentation.h"
#include "BrainSet.h"
#include "VolumeFile.h"
#include <cmath>

/**
 * Constructor.
 */
BrainModelVolumeLigaseSegmentation::BrainModelVolumeLigaseSegmentation(
                                               BrainSet* bs,
                                               VolumeFile* anatVolumeIn,
                                               VolumeFile* segVolumeOutIn,
                                               const QString& segVolumeNameIn,
                                               const QString& segVolumeLabelIn,
                                               const int xIn,
                                               const int yIn,
                                               const int zIn,
                                               const float whiteMinIn,
                                               const float whiteMeanIn,
                                               const float whiteMaxIn,
                                               const float diffBaseIn,
                                               const float gradBaseIn,
                                               const float highBiasIn,
                                               const float lowBiasIn)
   : BrainModelAlgorithm(bs)
{
   anatVolume     = anatVolumeIn;
   segVolume      = segVolumeOutIn;
   segVolumeName  = segVolumeNameIn;
   segVolumeLabel = segVolumeLabelIn;
   whiteMin = whiteMinIn;
   whiteMean = whiteMeanIn;
   whiteMax = whiteMaxIn;
   x_init = xIn;
   y_init = yIn;
   z_init = zIn;
   diffBase = diffBaseIn * (whiteMax - whiteMin);
   gradBase = gradBaseIn;
   highBias = highBiasIn;
   lowBias = lowBiasIn;
}
                                      
/**
 * Destructor.
 */
BrainModelVolumeLigaseSegmentation::~BrainModelVolumeLigaseSegmentation()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeLigaseSegmentation::execute() throw (BrainModelAlgorithmException)
{
   //
   // Verify volumes exist
   //
   if (anatVolume == NULL) {
      throw BrainModelAlgorithmException("Invalid anatomy volume.");
   }
   if (anatVolume->getNumberOfComponentsPerVoxel() != 1)
   {
      throw BrainModelAlgorithmException("Volume has multiple components.");
   }
   bool createdSegmentationVolume = false;
   if (segVolume == NULL) {
      segVolume = new VolumeFile(*anatVolume);
      segVolume->setVolumeType(VolumeFile::VOLUME_TYPE_SEGMENTATION);
      createdSegmentationVolume = true;
   }
   segVolume->setFileName(segVolumeName);
   segVolume->setDescriptiveLabel(segVolumeLabel);

   //
   // Verify volumes have same dimensions
   //
   int aDim[3], sDim[3];
   anatVolume->getDimensions(aDim);
   segVolume->getDimensions(sDim);
   for (int i = 0; i < 3; i++) {
      if (aDim[i] != sDim[i]) {
         throw BrainModelAlgorithmException(
            "Anatomy and Segmentation Volumes are of different dimensions.");
      }
   }
   
   //
   // Calculate 3D gradient magnitude
   //
   int i, j, k, xstep = 1, ystep = aDim[0], zstep = aDim[0] * aDim[1];
   float idist, jdist, kdist, xd, yd, zd, max = 0.0f, temp, tempa, tempb, cutoff, sig1 = whiteMean - whiteMin, sig2 = whiteMax - whiteMean;
   float* voxels = anatVolume->getVoxelData(), *end = voxels + aDim[0] * aDim[1] * aDim[2] - zstep;
   float* iter, *i_iter, *j_iter, *k_iter;
   float* grad_mag = new float[aDim[0] * aDim[1] * aDim[2]];
   float spacing[3];
   anatVolume->getSpacing(spacing);
   //
   // pointer math gradient
   //
   for (iter = voxels + zstep; iter < end; ++iter)
   {
      xd = (*(iter + xstep) - *(iter - xstep)) / spacing[0];
      yd = (*(iter + ystep) - *(iter - ystep)) / spacing[1];
      zd = (*(iter + zstep) - *(iter - zstep)) / spacing[2];
      temp = sqrtf(xd * xd + yd * yd + zd * zd);
      *(grad_mag + (iter - voxels)) = temp;
      if (temp > max)
      {
         max = temp;
      }
   }//WARNING: edges wrap, assumed to be unimportant, first and last slices uninitialized
   
   if (max == 0.0f)
   {
      delete[] grad_mag;
      throw BrainModelAlgorithmException("Anatomy volume has no gradient!");
   }
   
   //
   // scale from 0 to 1
   //
   end = grad_mag + aDim[0] * aDim[1] * aDim[2];
   for (iter = grad_mag; iter < end; ++iter)
   {
      *iter /= max;
   }
   float* seg = segVolume->getVoxelData();
   end = seg + aDim[0] * aDim[1] * aDim[2];
   //
   // zero segmentation volume
   //
   // change to make all nonzeros less than 254 so it adds to existing segmentation?
   //
   for (iter = seg; iter < end; ++iter)
   {
      *iter = 0.0f;
   }
   end = voxels + aDim[0] * aDim[1] * aDim[2];
   //
   // set first point
   //
   iterNode* head = new iterNode(), *tempIter;
   head->next = NULL;
   head->iter = voxels + anatVolume->getVoxelDataIndex(x_init, y_init, z_init);
   *(seg + (head->iter - voxels)) = 255.0f;
   //
   // loop until seed list clears (no more expansion)
   //
   // pointer math is used mostly to avoid calls to getVoxelDataIndex and to avoid reshaping the voxel data
   //
   while (head)
   {
      iter = head->iter;
      tempIter = head;
      head = head->next;
      delete tempIter;
      //
      // loop through neighbors via pointer math
      //
      for (i = -1; i < 2; ++i)
      {
         i_iter = iter + i * xstep;
         //
         // check if its the same slice
         //
         if (~i || (i_iter - voxels) / ystep == (iter - voxels) / ystep)
         {
            idist = i * spacing[0];
            idist = idist * idist;
            for (j = -1; j < 2; ++j)
            {
               j_iter = i_iter + j * ystep;
               if (~j || (j_iter - voxels) / zstep == (iter - voxels) / zstep)
               {
                  jdist = j * spacing[1];
                  jdist = idist + jdist * jdist;
                  for (k = -1; k < 2; ++k)
                  {
                     k_iter = j_iter + k * zstep;
                     //
                     // check that its not already visited
                     //
                     if ((~k || (k_iter >= voxels && k_iter < end)) && *(seg + (k_iter - voxels)) < 254.0f)
                     {
                        kdist = k * spacing[2];
                        //
                        // incremental distance from parent voxel
                        //
                        kdist = sqrtf(jdist + kdist * kdist);
                        //
                        // probabilistic intensity classification used as cutoff modifier
                        //
                        if (*k_iter < whiteMean)
                        {
                           temp = (whiteMean - *k_iter) / sig1;
                        } else {
                           temp = (whiteMean - *k_iter) / sig2;
                        }
                        cutoff = expf(-temp * temp / 2);
                        //
                        // difference from parent and 3d gradient magnitude used as criteria
                        //
                        temp = (*iter - *k_iter);
                        //if (temp < 0.0f) temp = -temp; // unneeded for elliptical cutoffs
                        tempa = temp / kdist / diffBase / cutoff;
                        tempb = *(grad_mag + (k_iter - voxels)) / gradBase / cutoff;
                        //
                        // if the ordered pair formed by criteria divided by their respective cutoffs has euclidean distance
                        // of less than 1, it grows there
                        //
                        // think of an ellipse on the diff-gradient plane, size determined by intensity and shape by respective
                        // cutoffs, if the values at the point lie inside, the point is used
                        //
                        if (tempa * tempa + tempb * tempb < 1.0f)
                        {
                           *(seg + (k_iter - voxels)) = 255.0f;
                           tempIter = new iterNode();
                           tempIter->next = head;
                           tempIter->iter = k_iter;
                           head = tempIter;
                        }
                     }
                  }
               }
            }
         }
      }
   }
   //
   // Add new segmentation volume to brain set
   //
   if (createdSegmentationVolume) {
      brainSet->addVolumeFile(VolumeFile::VOLUME_TYPE_SEGMENTATION,
                              segVolume,
                              segVolume->getFileName(),
                              true,
                              false);
   }
   delete[] grad_mag;
   segVolume->setVoxelColoringInvalid();
}

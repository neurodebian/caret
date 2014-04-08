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

#include "BrainModelVolumeROISmoothing.h"
#include "VolumeFile.h"
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <iostream>

using namespace std;

BrainModelVolumeROISmoothing::BrainModelVolumeROISmoothing(BrainSet* bs,
                                  VolumeFile* valuesIn,
                                  VolumeFile* regionIn,
                                  std::vector <VolumeFile *> * smoothVolVecIn,
                                  float sigmaIn)
   : BrainModelAlgorithm(bs)
{
   values = valuesIn;
   region = regionIn;
   smoothVolVec = smoothVolVecIn;
   sigma = sigmaIn;
}

BrainModelVolumeROISmoothing::~BrainModelVolumeROISmoothing()
{
}

void BrainModelVolumeROISmoothing::execute() throw (BrainModelAlgorithmException)
{
   int i, j, k, ki, kj, kk, dim[3], dim2[3];
   float spacing[3], spacing2[3], origin[3], origin2[3];
   float inputVal, smoothVal, weightVal;
   if (!values || !region || !smoothVolVec)
   {
      throw BrainModelAlgorithmException("Invalid volume.");
   }
   values->getSpacing(spacing);
   values->getOrigin(origin);
   values->getDimensions(dim);
   bool match = true;
   region->getSpacing(spacing2);
   region->getOrigin(origin2);
   region->getDimensions(dim2);
   
   for (i = 0; i < 3; ++i)
   {
      if (abs(spacing[i] - spacing2[i]) > 0.0001f)
      {
         match = false;
      }
      if (abs(origin[i] - origin2[i]) > 0.0001f)
      {
         match = false;
      }
      if (dim[i] != dim2[i])
      {
         match = false;
      }
   }
   if (!match)
   {
      throw BrainModelAlgorithmException("Input volumes do not match.");
   }
   VolumeFile::ORIENTATION myorient[3];// = {VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
                                      //    VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
                                      //    VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR};   
   values->getOrientation(myorient);
   float frange = sigma * 6.0f;
   int irange[3];
   irange[0] = (int)floor(frange / fabs(spacing[0]));
   irange[1] = (int)floor(frange / fabs(spacing[1]));
   irange[2] = (int)floor(frange / fabs(spacing[2]));
   if (!irange[0] || !irange[1] || !irange[2])
   {
      throw BrainModelAlgorithmException("Kernel too small.");
   }
   float*** weights = new float**[2 * irange[0] + 1];//precompute sigma weights
   float tempf, tempf2;
   for (i = 0; i < 2 * irange[0] + 1; ++i)
   {
      weights[i] = new float*[2 * irange[1] + 1];
      for (j = 0; j < 2 * irange[1] + 1; ++j)
      {
         weights[i][j] = new float[2 * irange[2] + 1];
         for (k = 0; k < 2 * irange[2] + 1; ++k)
         {
            tempf = (i - irange[0]) * spacing[0];
            tempf *= tempf;
            tempf2 = (j - irange[1]) * spacing[1];
            tempf2 *= tempf2;
            tempf += tempf2;
            tempf2 = (k - irange[2]) * spacing[2];
            tempf2 *= tempf2;
            tempf += tempf2;//square of euclidean distance
            tempf /= sigma * sigma;
            weights[i][j][k] = exp(-tempf / 2.0f);
         }
      }
   }
   
   int numSubVolumes = values->getNumberOfSubVolumes();
   smoothVolVec->resize(numSubVolumes);

   for( int s = 0;s<numSubVolumes;s++)
   {
      std::cout << "volume: " << s <<std::endl;
      //set up our output sub volume
      VolumeFile * smoothSubVol = new VolumeFile;
      smoothSubVol->initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT, dim, myorient, origin, spacing);//because setDimensions doesn't allocate...
      (*smoothVolVec)[s] = smoothSubVol;
      
      //get our input sub volume
      VolumeFile subVol;
      try {
         subVol.readFile(values->getFileName(), s);
      }
      catch(FileException e)
      {
         exit(1);
//         std::cout << "Exception caught while reading volume " << i << " of nifti file, " << values->getFileName() << std::endl;
//         std::cout << e.whatQString() << std::endl;
         throw BrainModelAlgorithmException("Error while reading Nifti Sub Volume.");
      }
      for (i = 0; i < dim[0]; ++i)
      {//all voxels
         for (j = 0; j < dim[1]; ++j)
         {
            for (k = 0; k < dim[2]; ++k)
            {
               if (region->getVoxel(i, j, k) > 0.0f)
               {//if selected                  
                  float weightSum = 0.0f;
                  smoothVal = 0.0f;
                  for (ki = -irange[0]; ki <= irange[0]; ++ki)
                  {//loop through kernel
                     if (i + ki < 0 || i + ki >= dim[0])
                     {
                        continue;//makes the indentation a bit less daunting
                     }
                     for (kj = -irange[1]; kj <= irange[1]; ++kj)
                     {
                        if (j + kj < 0 || j + kj >= dim[1])
                        {
                           continue;
                        }
                        for (kk = -irange[2]; kk <= irange[2]; ++kk)
                        {
                           if (k + kk < 0 || k + kk >= dim[2])// || (ki == 0 && kj == 0 && kk == 0))
                           {//dont skip center point?
                              continue;
                           }
                           if (region->getVoxel(i + ki, j + kj, k + kk) > 0.0f)
                           {
                              inputVal = subVol.getVoxel(i + ki, j + kj , k + kk);
                              weightVal = weights[ki + irange[0]][kj + irange[1]][kk + irange[2]];
                              smoothVal += inputVal * weightVal;
                              weightSum += weightVal;                              
                           }
                        }
                     }
                  }
                  if (smoothVal != smoothVal)
                  {
                     smoothVal = 0.0f;//set NaNs to zero
                  }
                  else
                  {
                     smoothVal /= weightSum;
                  }
                  smoothSubVol->setVoxel(i, j, k, 0, smoothVal);
               } else {
                  smoothSubVol->setVoxel(i, j, k, 0, 0.0f);
               }
            }
         }
      }
   }
}

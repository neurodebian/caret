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

#include "BrainModelVolumeROIAtlasResamplingAndSmoothing.h"
#include "VolumeFile.h"
#include <cmath>
#include <iostream>
#include <qshareddata.h>
#include <iostream>

using namespace std;


BrainModelVolumeROIAtlasResamplingAndSmoothing::BrainModelVolumeROIAtlasResamplingAndSmoothing(BrainSet* bs,
                                  VolumeFile* valuesIn,
                                  VolumeFile* regionIn,
                                  VolumeFile* atlasIn,
                                  std::vector <VolumeFile *> * smoothVolVecIn,
                                  float sigmaIn)
   : BrainModelAlgorithm(bs)
{
   values = valuesIn;
   region = regionIn;
   atlas = atlasIn;
   smoothVolVec = smoothVolVecIn,
   sigma = sigmaIn;
}

BrainModelVolumeROIAtlasResamplingAndSmoothing::~BrainModelVolumeROIAtlasResamplingAndSmoothing()
{
}

void BrainModelVolumeROIAtlasResamplingAndSmoothing::execute() throw (BrainModelAlgorithmException)
{
   int i, j, k, ki, kj, kk, dim[3], dim2[3], dim3[3];
   float spacing[3], spacing2[3], spacing3[3], origin[3], origin2[3], origin3[3];
   float inputVal, atlasVal, smoothVal, dx, dy, dz;
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
   atlas->getSpacing(spacing3);
   atlas->getOrigin(origin3);
   atlas->getDimensions(dim3);
   for (i = 0; i < 3; ++i)
   {
      if (abs(spacing[i] - spacing2[i]) > 0.0001f)
      {
         match = false;
      }
      if (abs(spacing[i] - spacing3[i]) > 0.0001f)
      {
         match = false;
      }
      if (abs(origin[i] - origin2[i]) > 0.0001f)
      {
         match = false;
      }
      if (abs(origin[i] - origin3[i]) > 0.0001f)
      {
         match = false;
      }
      if ((dim[i] != dim2[i]) ||(dim2[i] != dim3[i]))
      {
         match = false;
      }
   }
   if (!match)
   {
      throw BrainModelAlgorithmException("Input volumes do not match.");
   }
   VolumeFile::ORIENTATION myorient[3] = {VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
                                          VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
                                          VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR};
   //note: for now, get the volume orientation from the input time series
   //need to ask John H how this relates coordinate space
   //values->getOrientation(myorient);
   float frange = sigma * 6.0f; 
   
   int irange[3];
   irange[0] = (int)floor(frange / fabs(spacing[0]));
   irange[1] = (int)floor(frange / fabs(spacing[1]));
   irange[2] = (int)floor(frange / fabs(spacing[2]));

   //timevol -> values
   //File -> region
   //AtlasFile -> atlas
   //Atlastc  -> smoothValues
   //Dimension bug fixing??
   float displacement[3];
   float distance;
   float atlasCoord[3];
   float indCoord[3];
   float weight;
   
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
      subVol.readFile(values->getFileName(), s);

      for (i = 0; i < dim[0]; ++i)
      {//all voxels
         for (j = 0; j < dim[1]; ++j)
         {
            for (k = 0; k < dim[2]; ++k)
            {
               if (atlas->getVoxel(i, j, k) > 0.0f)
               {//if selected
                  atlasVal = atlas->getVoxel(i, j, k);
                  atlas->getVoxelCoordinate(i,j,k,atlasCoord);
                  float weightSum = 0.0f;
                  smoothVal = 0.0f;
                  for (ki = i-irange[0];ki <= i+irange[0]; ++ki)
                  {//loop through kernel
                     if (ki < 0 || ki >= dim[0]) continue;
                     for (kj = j-irange[1]; kj <= j+irange[1]; ++kj)
                     {                        
                        if (kj < 0 || kj >= dim[1]) continue;
                        for (kk = k-irange[2]; kk <= k+irange[2]; ++kk)
                        {
                           if (kk < 0 || kk >= dim[2]) continue;
                           if (region->getVoxel(ki,kj,kk) > 0.0f)
                           {
                              inputVal = subVol.getVoxel(ki, kj, kk);
                              region->getVoxelCoordinate(ki,kj,kk,indCoord);
                              displacement[0] = indCoord[0] - atlasCoord[0];
                              displacement[1] = indCoord[1] - atlasCoord[1];
                              displacement[2] = indCoord[2] - atlasCoord[2];
                              distance = sqrt(displacement[0]*displacement[0]+displacement[1]*displacement[1]+displacement[2]*displacement[2]);
                              if(distance >= frange) continue;
                              //taking a square root and then squaring below, definitely room for optimization...
                              weight = exp((double)(-distance*distance/(2.0 * sigma*sigma)));
                              smoothVal += inputVal * weight;
                              weightSum += weight;
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
                     if(weightSum) smoothVal /= weightSum;
                     else smoothVal = 0.0f;
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

//This is an optimized version of AtlasResamplingAndSmoothing that is based on Tim Coalson's ROI gradient algorithm,
//which pre-computes gaussian weights before entering the main loop.
/*
void BrainModelVolumeROIAtlasResamplingAndSmoothing::execute_optimized() throw (BrainModelAlgorithmException)
{
   int i, j, k, ki, kj, kk, dim[3], dim2[3], dim3[3];
   float spacing[3], spacing2[3], spacing3[3], origin[3], origin2[3], origin3[3];
   float inputVal, atlasVal, smoothVal, weightVal, dx, dy, dz;
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
   atlas->getSpacing(spacing3);
   atlas->getOrigin(origin3);
   atlas->getDimensions(dim3);
   for (i = 0; i < 3; ++i)
   {
      if (abs(spacing[i] - spacing2[i]) > 0.0001f)
      {
         match = false;
      }
      if (abs(spacing[i] - spacing3[i]) > 0.0001f)
      {
         match = false;
      }
      if (abs(origin[i] - origin2[i]) > 0.0001f)
      {
         match = false;
      }
      if (abs(origin[i] - origin3[i]) > 0.0001f)
      {
         match = false;
      }
      if (dim[i] != dim2[i] != dim3[i])
      {
         match = false;
      }
   }
   if (!match)
   {
      throw BrainModelAlgorithmException("Input volumes do not match.");
   }
   VolumeFile::ORIENTATION myorient[3] = {VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
                                          VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
                                          VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR};   
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
   //timevol -> values
   //File -> region
   //AtlasFile -> atlas
   //Atlastc  -> smoothValues
   //Dimension bug fixing??
   int numSubVolumes = region->getNumberOfSubVolumes();
   std::vector<VolumeFile *> smoothSubVolVec;
   smoothSubVolVec.resize(numSubVolumes);
   for( int i = 0;i<numSubVolumes;i++)
   {
      //set up our output sub volume
      VolumeFile * smoothSubVol = new VolumeFile;
      smoothSubVol->initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT, dim, myorient, origin, spacing);//because setDimensions doesn't allocate...
      smoothSubVolVec[i] = smoothSubVol;
      
      //get our input sub volume
      VolumeFile subVol;
      try {
         subVol.readFile(values->getFileName(), i);
      }
      catch(FileException e)
      {
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

                  inputVal = values->getVoxel(i, j, k);
                  int numWeights = 0;
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
                              atlasVal = atlas->getVoxel(i + ki, j + kj, k + kk);
                              weightVal = weights[ki + irange[0]][kj + irange[1]][kk + irange[2]];
                              smoothVal += inputVal * weightVal;
                              numWeights++;                              
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
                     smoothVal /= numWeights;
                  }
                  smoothSubVol->setVoxel(i, j, k, 0, smoothVal);
               } else {
                  smoothSubVol->setVoxel(i, j, k, 0, 0.0f);
               }
            }
         }
         
         
      }

  }
}*/


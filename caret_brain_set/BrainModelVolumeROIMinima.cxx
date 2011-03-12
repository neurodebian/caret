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

#include "BrainModelVolumeROIMinima.h"
#include "VolumeFile.h"
#include <cmath>

using namespace std;

BrainModelVolumeROIMinima::BrainModelVolumeROIMinima(BrainSet* bs,
                                  VolumeFile* valuesIn,
                                  VolumeFile* regionIn,
                                  VolumeFile* minimaOutIn,
                                  float distIn)
   : BrainModelAlgorithm(bs)
{
   values = valuesIn;
   region = regionIn;
   minimaOut = minimaOutIn;
   dist = distIn;
}

BrainModelVolumeROIMinima::~BrainModelVolumeROIMinima()
{
}

void BrainModelVolumeROIMinima::execute() throw (BrainModelAlgorithmException)
{
   int i, j, k, ki, kj, kk, dim[3], dim2[3];
   float spacing[3], spacing2[3], origin[3], origin2[3];
   float tempf, tempf2, tempf3;
   if (!values || !region || !minimaOut)
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
   VolumeFile::ORIENTATION myorient[3] = {VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
                                          VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
                                          VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR};
   minimaOut->initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT, dim, myorient, origin, spacing);//because setDimensions doesn't allocate...
   tempf3 = dist * dist;
   int irange[3];
   irange[0] = (int)floor(dist / fabs(spacing[0]));
   irange[1] = (int)floor(dist / fabs(spacing[1]));
   irange[2] = (int)floor(dist / fabs(spacing[2]));
   if (!irange[0] || !irange[1] || !irange[2])
   {
      throw BrainModelAlgorithmException("Distance too small.");
   }
   char*** mask = new char**[2 * irange[0] + 1];//precompute "sphere" of voxels
   for (i = 0; i < 2 * irange[0] + 1; ++i)
   {
      mask[i] = new char*[2 * irange[1] + 1];
      for (j = 0; j < 2 * irange[1] + 1; ++j)
      {
         mask[i][j] = new char[2 * irange[2] + 1];
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
            if (tempf <= tempf3)
            {
               mask[i][j][k] = 1;
            } else {
               mask[i][j][k] = 0;
            }
         }
      }
   }
   bool found;
   for (i = 0; i < dim[0]; ++i)
   {//all voxels
      for (j = 0; j < dim[1]; ++j)
      {
         for (k = 0; k < dim[2]; ++k)
         {
            if (region->getVoxel(i, j, k) > 0.0f)
            {//if selected
               tempf = values->getVoxel(i, j, k);
               found = false;
               for (ki = -irange[0]; !found && ki <= irange[0]; ++ki)
               {//loop through kernel
                  if (i + ki < 0 || i + ki >= dim[0])
                  {
                     continue;//makes the indentation a bit less daunting
                  }
                  for (kj = -irange[1]; !found && kj <= irange[1]; ++kj)
                  {
                     if (j + kj < 0 || j + kj >= dim[1])
                     {
                        continue;
                     }
                     for (kk = -irange[2]; kk <= irange[2]; ++kk)
                     {
                        if (k + kk < 0 || k + kk >= dim[2] || (ki == 0 && kj == 0 && kk == 0))
                        {
                           continue;
                        }
                        if (mask[ki + irange[0]][kj + irange[1]][kk + irange[2]] && region->getVoxel(i + ki, j + kj, k + kk) > 0.0f && values->getVoxel(i + ki, j + kj, k + kk) < tempf)
                        {
                           found = true;
                           break;
                        }
                     }
                  }
               }
               if (found)
               {
                  minimaOut->setVoxel(i, j, k, 0, 0.0f);
               } else {
                  minimaOut->setVoxel(i, j, k, 0, 255.0f);
               }
            } else {
               minimaOut->setVoxel(i, j, k, 0, 0.0f);
            }
         }
      }
   }
}

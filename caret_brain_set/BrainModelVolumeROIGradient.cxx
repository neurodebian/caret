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

#include "BrainModelVolumeROIGradient.h"
#include "VolumeFile.h"
#include <cmath>

using namespace std;

void BrainModelVolumeROIGradient::calcrref(float* matrix[], int rows, int cols)
{//assumes more cols than rows
   int i, j, k, temp;
   float tempf, tempf2;
   for (i = 0; i < rows; ++i)
   {
      tempf = std::abs(matrix[i][i]);//search for pivot
      temp = i;
      for (j = i + 1; j < rows; ++j)
      {
         tempf2 = std::abs(matrix[j][i]);
         if (tempf2 > tempf)
         {
            tempf = tempf2;
            temp = j;
         }
      }
      if (i != temp)
      {
         for (j = i; j < cols; ++j)
         {//skip the waste that will end up 0's and 1's
            tempf = matrix[i][j];
            matrix[i][j] = matrix[temp][j];
            matrix[temp][j] = tempf;
         }
      }
      tempf = matrix[i][i];//pivot
      for (j = i + 1; j < cols; ++j)
      {//again, skip the 0's and 1's
         matrix[i][j] /= tempf;
         for (k = 0; k < i; ++k)
         {
            matrix[k][j] -= matrix[k][i] * matrix[i][j];
         }
         for (++k; k < rows; ++k)
         {
            matrix[k][j] -= matrix[k][i] * matrix[i][j];
         }
      }
   }//rref complete for all cols >= rows, just pretend rowsXrows is I
}

BrainModelVolumeROIGradient::BrainModelVolumeROIGradient(BrainSet* bs,
                                  VolumeFile* valuesIn,
                                  VolumeFile* regionIn,
                                  VolumeFile* gradMagOutIn,
                                  float kernelIn)
   : BrainModelAlgorithm(bs)
{
   values = valuesIn;
   region = regionIn;
   gradMagOut = gradMagOutIn;
   kernel = kernelIn;
}

BrainModelVolumeROIGradient::~BrainModelVolumeROIGradient()
{
}

void BrainModelVolumeROIGradient::execute() throw (BrainModelAlgorithmException)
{
   int i, j, k, ki, kj, kk, dim[3], dim2[3];
   float spacing[3], spacing2[3], origin[3], origin2[3];
   float tempf, tempf2, tempf3, dx, dy, dz;
   if (!values || !region || !gradMagOut)
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
   gradMagOut->initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT, dim, myorient, origin, spacing);//because setDimensions doesn't allocate...
   float frange = kernel * 4.0f;
   int irange[3];
   irange[0] = (int)floor(frange / fabs(spacing[0]));
   irange[1] = (int)floor(frange / fabs(spacing[1]));
   irange[2] = (int)floor(frange / fabs(spacing[2]));
   if (!irange[0]) irange[0] = 1;
   if (!irange[1]) irange[1] = 1;
   if (!irange[2]) irange[2] = 1;
   float*** weights = new float**[2 * irange[0] + 1];//precompute kernel weights
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
            tempf /= kernel * kernel;
            weights[i][j][k] = exp(-tempf / 2.0f);
         }
      }
   }
   float* rref[4];
   for (i = 0; i < 4; ++i)
   {
      rref[i] = new float[5];
   }
   for (i = 0; i < dim[0]; ++i)
   {//all voxels
      for (j = 0; j < dim[1]; ++j)
      {
         for (k = 0; k < dim[2]; ++k)
         {
            if (region->getVoxel(i, j, k) > 0.0f)
            {//if selected
               for (ki = 0; ki < 4; ++ki)
               {//reset rref
                  for (kj = 0; kj < 5; ++kj)
                  {
                     rref[ki][kj] = 0.0f;
                  }
               }
               tempf = values->getVoxel(i, j, k);
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
                           dx = ki * spacing[0];
                           dy = kj * spacing[1];
                           dz = kk * spacing[2];
                           tempf2 = values->getVoxel(i + ki, j + kj, k + kk) - tempf;
                           tempf3 = weights[ki + irange[0]][kj + irange[1]][kk + irange[2]];
                           rref[0][0] += dx * dx * tempf3;
                           rref[0][1] += dx * dy * tempf3;
                           rref[0][2] += dx * dz * tempf3;
                           rref[0][3] += dx * tempf3;
                           rref[0][4] += dx * tempf2 * tempf3;
                           rref[1][1] += dy * dy * tempf3;
                           rref[1][2] += dy * dz * tempf3;
                           rref[1][3] += dy * tempf3;
                           rref[1][4] += dy * tempf2 * tempf3;
                           rref[2][2] += dz * dz * tempf3;
                           rref[2][3] += dz * tempf3;
                           rref[2][4] += dz * tempf2 * tempf3;
                           rref[3][3] += tempf3;
                           rref[3][4] += tempf2 * tempf3;
                        }
                     }
                  }
               }
               rref[1][0] = rref[0][1];
               rref[2][0] = rref[0][2];
               rref[2][1] = rref[1][2];
               rref[3][0] = rref[0][3];
               rref[3][1] = rref[1][3];
               rref[3][2] = rref[2][3];
               calcrref(rref, 4, 5);//gradient vector is (rref[0][4], rref[1][4], rref[2][4])
               tempf = sqrt(rref[0][4] * rref[0][4] + rref[1][4] * rref[1][4] + rref[2][4] * rref[2][4]);
               if (tempf != tempf)
               {
                  tempf = 0.0f;//set NaNs to zero
               }
               gradMagOut->setVoxel(i, j, k, 0, tempf);
            } else {
               gradMagOut->setVoxel(i, j, k, 0, 0.0f);
            }
         }
      }
   }
   for (i = 0; i < 2 * irange[0] + 1; ++i)
   {
      for (j = 0; j < 2 * irange[1] + 1; ++j)
      {
         delete[] weights[i][j];
      }
      delete[] weights[i];
   }
   delete[] weights;
}

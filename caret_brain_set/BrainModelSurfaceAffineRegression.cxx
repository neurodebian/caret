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
#include "BrainModelSurfaceAffineRegression.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include <cmath>

/**
 * Constructor.
 */
BrainModelSurfaceAffineRegression::BrainModelSurfaceAffineRegression(
                                               BrainSet* bs,
                                               CoordinateFile* sourceIn,
                                               CoordinateFile* targetIn,
                                               CoordinateFile* registeredOutIn,
                                               const QString& coordNameIn)
   : BrainModelAlgorithm(bs)
{
   source     = sourceIn;
   target     = targetIn;
   registered = registeredOutIn;
   coordName  = coordNameIn;
}
                                      
/**
 * Destructor.
 */
BrainModelSurfaceAffineRegression::~BrainModelSurfaceAffineRegression()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceAffineRegression::execute() throw (BrainModelAlgorithmException)
{
   //
   // Verify files exist, are valid, etc.
   //
   if (source == NULL || target == NULL) {
      throw BrainModelAlgorithmException("Invalid coordinate file.");
   }
   if (source->getNumberOfCoordinates() < 1)
   {
      throw BrainModelAlgorithmException("Not enough nodes in coordinate files.");
   }
   if (source->getNumberOfCoordinates() != target->getNumberOfCoordinates())
   {
      throw BrainModelAlgorithmException("Node numbers do not match.");
   }
   //
   // Create output coord file if neccesary
   //
   bool createdRegisteredCoord = false;
   BrainModelSurface *mySurf = NULL;
   if (registered == NULL) {
      mySurf = new BrainModelSurface(brainSet);
      registered = mySurf->getCoordinateFile();
      registered->setNumberOfCoordinates(source->getNumberOfCoordinates());
      createdRegisteredCoord = true;
   }
   if (source->getNumberOfCoordinates() != registered->getNumberOfCoordinates())
   {
      registered->setNumberOfCoordinates(source->getNumberOfCoordinates());
   }
   registered->setFileName(coordName);
   //
   // Initialization
   //
   double indep[4][4];
   double dep[3][4];
   double tempd;
   long i, j, k, l, nodes = source->getNumberOfCoordinates();
   long triples = nodes * 3;
   float* sourceData = new float[triples];
   float* targetData = new float[triples];
   float* registeredData = new float[triples];
   source->getAllCoordinates(sourceData);
   target->getAllCoordinates(targetData);
   for (i = 0; i < 3; ++i)
   {
      for (j = 0; j < 4; ++j)
      {
         dep[i][j] = 0.0;
         indep[i][j] = 0.0;
      }
   }
   for (i = 0; i < 4; ++i) indep[3][i] = 0.0;
   //
   // Gather Xt * X and (Xt * Y)'s from the normal equations
   //
   for (i = 0; i < triples; i += 3)
   {
      for (j = 0; j < 3; ++j)
      {
         for (k = 0; k < 3; ++k)
         {
            indep[j][k] += ((double)sourceData[i + j]) * ((double)sourceData[i + k]);
            dep[j][k] += ((double)sourceData[i + k]) * ((double)targetData[i + j]);
         }
         indep[j][3] += (double)sourceData[i + j];
         dep[j][3] += (double)targetData[i + j];
      }
      for (j = 0; j < 3; ++j)
      {
         indep[3][j] += (double)sourceData[i + j];
      }
      indep[3][3] += 1.0;
   }
   double rref[5][4];
   for (i = 0; i < 3; ++i)
   {
      //
      // Set up a normal equation as a system of linear equations, solve by reduced row echelon form
      //
      for (j = 0; j < 5; ++j)
      {
         for (k = 0; k < 4; ++k)
         {
            if (j == 4)
            {
               rref[j][k] = dep[i][k];
            } else {
               rref[j][k] = indep[j][k];
            }
         }
      }
      //
      // Reduced row echelon computation
      //
      for (j = 0; j < 4; ++j)
      {
         //
         // Find pivot row, swap if needed
         //
         if (rref[j][j] == 0.0)//naive expectance of exact 0 for nonpivot, but shouldn't happen anyway
         {
            k = j + 1;
            while (k < 4 && rref[j][k] == 0.0) ++k;//ditto
            if (k == 4)
            {
               delete[] sourceData;
               delete[] targetData;
               delete[] registeredData;
               throw BrainModelAlgorithmException("Pivot missing, does the surface have 3 independent dimensions?");
            }
            for (l = 0; l < 5; ++l)
            {
               tempd = rref[l][j];
               rref[l][j] = rref[l][k];
               rref[l][k] = tempd;
            }
         }
         //
         // Divide by pivot to get leading 1
         //
         tempd = rref[j][j];
         for (k = 0; k < 5; ++k)
         {
            rref[k][j] /= tempd;
         }
         //
         // Zero the rest of the pivot column via row operations
         //
         for (k = 0; k < 4; ++k)//assumes exact 1 after division by itself, such that all other rows in the pivot column will be zeroed
         {//by simple subtraction of the assumed one times the value of the position to be zeroed, but all we want is the last column anyway
            if (k != j)
            {
               tempd = rref[j][k];
               for (l = 0; l < 5; ++l)
               {
                  rref[l][k] -= rref[l][j] * tempd;
               }
            }
         }
      }//rref complete!
      //
      // Copy solution into appropriate column of affine transformation matrix (3x3, multiply X by it to get transformed coordinates)
      //
      for (j = 0; j < 4; ++j)
      {
         affine[i][j] = rref[4][j];
      }
   }//affine computation complete!
   //
   // Matrix multiplication by affine transform matrix
   //
   for (i = 0; i < triples; i += 3)
   {
      for (j = 0; j < 3; ++j)
      {
         registeredData[i + j] = 0.0f;
         for (k = 0; k < 3; ++k)
         {
            registeredData[i + j] += affine[j][k] * ((double)sourceData[i + k]);
         }
         registeredData[i + j] += affine[j][3];
      }
   }
   //
   // Set the new coordinates
   //
   registered->setAllCoordinates(registeredData);
   //
   // Add new registered coordinate file to brain set
   //
   if (createdRegisteredCoord) {
      brainSet->addBrainModel(mySurf);
   }
   delete[] sourceData;
   delete[] targetData;
   delete[] registeredData;
}

double BrainModelSurfaceAffineRegression::getAffine(short i, short j)
{
   return affine[i][j];
}

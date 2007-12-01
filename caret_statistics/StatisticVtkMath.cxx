
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

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: StatisticVtkMath.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cmath>
#include <iostream>

#include "StatisticVtkMath.h"

#define VTK_SMALL_NUMBER 1.0e-12

// Invert input square matrix A into matrix AI. Note that A is modified during
// the inversion. The size variable is the dimension of the matrix. Returns 0
// if inverse not computed.
// -----------------------
// For thread safe behavior, temporary arrays tmp1SIze and tmp2Size
// of length size must be passsed in.
int StatisticVtkMath::InvertMatrix(double **A, double **AI, int size,
                          int *tmp1Size, double *tmp2Size)
{
  int i, j;

  //
  // Factor matrix; then begin solving for inverse one column at a time.
  // Note: tmp1Size returned value is used later, tmp2Size is just working
  // memory whose values are not used in LUSolveLinearSystem
  //
  if ( StatisticVtkMath::LUFactorLinearSystem(A, tmp1Size, size, tmp2Size) == 0 )
    {
    return 0;
    }
  
  for ( j=0; j < size; j++ )
    {
    for ( i=0; i < size; i++ )
      {
      tmp2Size[i] = 0.0;
      }
    tmp2Size[j] = 1.0;

    StatisticVtkMath::LUSolveLinearSystem(A,tmp1Size,tmp2Size,size);

    for ( i=0; i < size; i++ )
      {
      AI[i][j] = tmp2Size[i];
      }
    }

  return 1;
}

// Factor linear equations Ax = b using LU decompostion A = LU where L is
// lower triangular matrix and U is upper triangular matrix. Input is 
// square matrix A, integer array of pivot indices index[0->n-1], and size
// of square matrix n. Output factorization LU is in matrix A. If error is 
// found, method returns 0.
//------------------------------------------------------------------
// For thread safe, temporary memory array tmpSize of length size
// must be passed in.
int StatisticVtkMath::LUFactorLinearSystem(double **A, int *index, int size,
                                  double *tmpSize)
{
  int i, j, k;
  int maxI = 0;
  double largest, temp1, temp2, sum;

  //
  // Loop over rows to get implicit scaling information
  //
  for ( i = 0; i < size; i++ ) 
    {
    for ( largest = 0.0, j = 0; j < size; j++ ) 
      {
      if ( (temp2 = fabs(A[i][j])) > largest )
        {
        largest = temp2;
        }
      }

    if ( largest == 0.0 )
      {
      //vtkGenericWarningMacro(<<"Unable to factor linear system");
      return 0;
      }
      tmpSize[i] = 1.0 / largest;
    }
  //
  // Loop over all columns using Crout's method
  //
  for ( j = 0; j < size; j++ ) 
    {
    for (i = 0; i < j; i++) 
      {
      sum = A[i][j];
      for ( k = 0; k < i; k++ )
        {
        sum -= A[i][k] * A[k][j];
        }
      A[i][j] = sum;
      }
    //
    // Begin search for largest pivot element
    //
    for ( largest = 0.0, i = j; i < size; i++ ) 
      {
      sum = A[i][j];
      for ( k = 0; k < j; k++ )
        {
        sum -= A[i][k] * A[k][j];
        }
      A[i][j] = sum;

      if ( (temp1 = tmpSize[i]*fabs(sum)) >= largest ) 
        {
        largest = temp1;
        maxI = i;
        }
      }
    //
    // Check for row interchange
    //
    if ( j != maxI ) 
      {
      for ( k = 0; k < size; k++ ) 
        {
        temp1 = A[maxI][k];
        A[maxI][k] = A[j][k];
        A[j][k] = temp1;
        }
      tmpSize[maxI] = tmpSize[j];
      }
    //
    // Divide by pivot element and perform elimination
    //
    index[j] = maxI;

    if ( fabs(A[j][j]) <= VTK_SMALL_NUMBER )
      {
      //vtkGenericWarningMacro(<<"Unable to factor linear system");
      return 0;
      }

    if ( j != (size-1) ) 
      {
      temp1 = 1.0 / A[j][j];
      for ( i = j + 1; i < size; i++ )
        {
        A[i][j] *= temp1;
        }
      }
    }

  return 1;
}
//----------------------------------------------------------------------------
// Solve linear equations Ax = b using LU decompostion A = LU where L is
// lower triangular matrix and U is upper triangular matrix. Input is 
// factored matrix A=LU, integer array of pivot indices index[0->n-1],
// load vector x[0->n-1], and size of square matrix n. Note that A=LU and
// index[] are generated from method LUFactorLinearSystem). Also, solution
// vector is written directly over input load vector.
void StatisticVtkMath::LUSolveLinearSystem(double **A, int *index, 
                                  double *x, int size)
{
  int i, j, ii, idx;
  double sum;
//
// Proceed with forward and backsubstitution for L and U
// matrices.  First, forward substitution.
//
  for ( ii = -1, i = 0; i < size; i++ ) 
    {
    idx = index[i];
    sum = x[idx];
    x[idx] = x[i];

    if ( ii >= 0 )
      {
      for ( j = ii; j <= (i-1); j++ )
        {
        sum -= A[i][j]*x[j];
        }
      }
    else if (sum)
      {
      ii = i;
      }

    x[i] = sum;
  }
//
// Now, back substitution
//
  for ( i = size-1; i >= 0; i-- ) 
    {
    sum = x[i];
    for ( j = i + 1; j < size; j++ )
      {
      sum -= A[i][j]*x[j];
      }
    x[i] = sum / A[i][i];
    }
}

#undef VTK_SMALL_NUMBER


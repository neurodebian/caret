
#ifndef __STATISTIC_VTK_MATH_H__
#define __STATISTIC_VTK_MATH_H__

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
  Module:    $RCSfile: StatisticVtkMath.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/// stuff taken from vtkMath
class StatisticVtkMath {
   public:
  // Description:
  // Thread safe version of InvertMatrix method.
  // Working memory arrays tmp1SIze and tmp2Size
  // of length size must be passed in.
  static int InvertMatrix(double **A, double **AI, int size,
                          int *tmp1Size, double *tmp2Size);

  // Description:
  // Thread safe version of LUFactorLinearSystem method.
  // Working memory array tmpSize of length size
  // must be passed in.
  static int LUFactorLinearSystem(double **A, int *index, int size,
                                  double *tmpSize);

  // Description:
  // Solve linear equations Ax = b using LU decomposition A = LU where L is
  // lower triangular matrix and U is upper triangular matrix. Input is 
  // factored matrix A=LU, integer array of pivot indices index[0->n-1],
  // load vector x[0->n-1], and size of square matrix n. Note that A=LU and
  // index[] are generated from method LUFactorLinearSystem). Also, solution
  // vector is written directly over input load vector.
  static void LUSolveLinearSystem(double **A, int *index, 
                                  double *x, int size);
};

#endif // __STATISTIC_VTK_MATH_H__

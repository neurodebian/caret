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

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "StatisticAlgorithm.h"
#include "StatisticMatrix.h"
#include "StatisticNumericalRecipes.h"
#include "StatisticVtkMath.h"

#define MATRIX_DEBUG_FLAG 1

/**
 * constructor.
 */
StatisticMatrix::StatisticMatrix()
{
   data = NULL;
   setDimensions(0, 0);
}

/**
 * constructor.
 */
StatisticMatrix::StatisticMatrix(const int numRows,
                                 const int numColumns)
{
   data = NULL;
   setDimensions(numRows, numColumns);
}

/**
 * copy constructor.
 */
StatisticMatrix::StatisticMatrix(const StatisticMatrix& sm)
{
   data = NULL;
   copyHelper(sm);
}

/**
 * destructor.
 */
StatisticMatrix::~StatisticMatrix()
{
   setDimensions(0, 0);
}

/**
 * Assignment operator. 
 */
StatisticMatrix& 
StatisticMatrix::operator=(const StatisticMatrix& sm)
{
   if (this != &sm) {
      copyHelper(sm);
   }
   
   return *this;
}
 
/**
 * copy data for assignment operator and copy constructor.
 */
void 
StatisticMatrix::copyHelper(const StatisticMatrix& sm)
{
   //
   // setDimensions will allocate memory and set number of rows/cols
   //
   setDimensions(sm.getNumberOfRows(), sm.getNumberOfColumns());
   const int numData = numberOfRows * numberOfColumns;
   for (int i = 0; i < numData; i++) {
      data[i] = sm.data[i];
   }
}
      
/**
 * Equality operator.
 */
bool 
StatisticMatrix::operator==(const StatisticMatrix& sm)
{
   if ((numberOfRows != sm.numberOfRows) ||
       (numberOfColumns != sm.numberOfColumns)) {
      return false;
   }
   
   const int numData = numberOfRows * numberOfColumns;
   for (int i = 0; i < numData; i++) {
      if (data[i] != sm.data[i]) {
         return false;
      }
   }
   
   return true;
}
      
/**
 * print a matrix.
 */
void 
StatisticMatrix::print(std::ostream& stream,
                       const std::string& offset,
                       const std::string& matrixName) const
{
   if (matrixName.empty() == false) {
      stream << matrixName << std::endl;
   }
   for (int i = 0; i < numberOfRows; i++) {
      stream << offset;
      for (int j = 0; j < numberOfColumns; j++) {
         stream << getElement(i, j) << " ";
      }
      stream << std::endl;
   }
}
      
/**
 * set the dimensions of a matrix.
 */
void 
StatisticMatrix::setDimensions(const int numRows,
                               const int numColumns)
{
   if (data != NULL) {
      delete[] data;
      data = NULL;
   }
   
   numberOfRows = numRows;
   numberOfColumns = numColumns;
   if ((numberOfRows > 0) &&
       (numberOfColumns > 0)) {
      data = new double[numberOfRows * numberOfColumns];
   }
}
                        
/**
 * get an element from the matrix.
 */
double 
StatisticMatrix::getElement(const int rowNumber,
                            const int columnNumber) const
{
   const int indx = getElementIndex(rowNumber, columnNumber);
   return data[indx];
}
                 
/**
 * set an element from the matrix.
 */
void 
StatisticMatrix::setElement(const int rowNumber,
                            const int columnNumber,
                            const float value)
{
   setElement(rowNumber, columnNumber, static_cast<double>(value));
}
                 
/**
 * set an element from the matrix.
 */
void 
StatisticMatrix::setElement(const int rowNumber,
                            const int columnNumber,
                            const double value)
{
   const int indx = getElementIndex(rowNumber, columnNumber);
   data[indx] = value;
}
                 
// set all elements to value
void 
StatisticMatrix::setAllElements(const float value)
{
   const int num = numberOfRows * numberOfColumns;
   for (int i = 0; i < num; i++) {
      data[i] = value;
   }
}

/**
 * set the matrix from a one dimensional array starting with "top row" of elements
 * matrix must already be set to a number of rows and columns
 */
void 
StatisticMatrix::setMatrixFromOneDimensionalArray(const float dataIn[])
{
#ifdef MATRIX_DEBUG_FLAG
   if (numberOfRows <= 0) {
      std::cout << "StatisticMatrix::setMatrixFromOneDimensionalArray: invalid number of rows " << numberOfRows << std::endl;
      std::abort();
   }
   if (numberOfColumns <= 0) {
      std::cout << "StatisticMatrix::setMatrixFromOneDimensionalArray: invalid number of columns " << numberOfColumns << std::endl;
      std::abort();
   }
#endif // MATRIX_DEBUG_FLAG
   int ctr = 0;
   for (int i = 0; i < numberOfRows; i++) {
      for (int j = 0; j < numberOfColumns; j++) {
         setElement(i, j, dataIn[ctr]);
         ctr++;
      }
   }
}
      
/**
 * set the matrix from a one dimensional array starting with "top row" of elements
 * matrix must already be set to a number of rows and columns
 */
void 
StatisticMatrix::setMatrixFromOneDimensionalArray(const double dataIn[])
{
#ifdef MATRIX_DEBUG_FLAG
   if (numberOfRows <= 0) {
      std::cout << "StatisticMatrix::setMatrixFromOneDimensionalArray: invalid number of rows " << numberOfRows << std::endl;
      std::abort();
   }
   if (numberOfColumns <= 0) {
      std::cout << "StatisticMatrix::setMatrixFromOneDimensionalArray: invalid number of columns " << numberOfColumns << std::endl;
      std::abort();
   }
#endif // MATRIX_DEBUG_FLAG
   int ctr = 0;
   for (int i = 0; i < numberOfRows; i++) {
      for (int j = 0; j < numberOfColumns; j++) {
         setElement(i, j, dataIn[ctr]);
         ctr++;
      }
   }
}
      
/**
 * get the location of an element in the matrix.
 */
int 
StatisticMatrix::getElementIndex(const int rowNumber,
                                 const int columnNumber) const
{
#ifdef MATRIX_DEBUG_FLAG
   if (numberOfRows <= 0) {
      std::cout << "StatisticMatrix::getElementIndex: invalid number of rows " << numberOfRows << std::endl;
      std::abort();
   }
   if (numberOfColumns <= 0) {
      std::cout << "StatisticMatrix::getElementIndex: invalid number of columns " << numberOfColumns << std::endl;
      std::abort();
   }
   if ((rowNumber < 0) || 
       (rowNumber >= numberOfRows)) {
      std::cout << "StatisticMatrix::getElementIndex: invalid row number " << rowNumber << std::endl
                << "  number of rows " << numberOfRows << std::endl;
      std::abort();
   }
   if ((columnNumber < 0) || 
       (columnNumber >= numberOfColumns)) {
      std::cout << "StatisticMatrix::getElementIndex: invalid row number " << columnNumber << std::endl
                << "  number of rows " << numberOfColumns << std::endl;
      std::abort();
   }
#endif // MATRIX_DEBUG_FLAG

   const int indx = (rowNumber * numberOfColumns) + columnNumber;
   return indx;
}

/**
 * get the inverse of a matrix.
 * Taken from VTK.
 */
StatisticMatrix 
StatisticMatrix::inverse() const throw (StatisticException)
{
  StatisticMatrix inverseMatrixOut;
  
  if ((numberOfRows != numberOfColumns)) {
     throw StatisticException("ERROR: Matrix must be square for inverse operation.");
  }
  
  inverseMatrixOut.setDimensions(getNumberOfRows(), getNumberOfColumns());
  
  const int size = numberOfRows;
  int *index=new int[size];
  double *column=new double[size];

  const int sizeSQ = size* size;
  double* matrixIn = new double[sizeSQ];
  double* matrixOut = new double [sizeSQ];
  for (int i = 0; i < sizeSQ; i++) {
     matrixIn[i] = data[i];
  }
  const int MAX_DIM = 100;
  if (size > MAX_DIM) {
     throw StatisticException("ERROR StatisticMatrix::inverse: MAX_DIM  exceeded.");
  }
  double *A[MAX_DIM];
  double *AI[MAX_DIM];
  for (int i = 0; i < size; i++) {
     A[i] = &matrixIn[i * size];
     AI[i] = &matrixOut[i * size];
     //AI[(size - 1 - i)] = &matrixOut[i * size];
  }
  const int retVal = StatisticVtkMath::InvertMatrix(A, AI, size, index, column);
  if (retVal != 0) {
  /*
     for (int i = 0; i < size; i++) {
        double* temp = AI[i];
        for (int j = 0; j < size; j++) {
           matrixOut[getElementIndex(i, j)] = temp[j];
        }
     }
   */
     for (int i = 0; i < sizeSQ; i++) {
        inverseMatrixOut.data[i] = matrixOut[i];
     }
   }
   else {
      throw StatisticException("ERROR: Matrix inverse failed.");
   }

  delete [] index;
  delete [] column;
  
  return inverseMatrixOut;
}

/**
 * get the pseudo inverse of a matrix.
 */
StatisticMatrix 
StatisticMatrix::inversePseudo() const throw (StatisticException)
{
   if ((numberOfRows != numberOfColumns)) {
      throw StatisticException("ERROR: Matrix must be square for pseudo inverse operation.");
   }
   const int n = numberOfRows;
   if (n <= 0) {
      throw StatisticException("ERROR: Matrix is empty for pseudo inverse operation.");
   }
  
   //
   // Allocate for numerical recipes
   //
   float** au = StatisticNumericalRecipes::matrix(1, n, 1, n);
   float*  w = StatisticNumericalRecipes::vector(1, n);
   float** v = StatisticNumericalRecipes::matrix(1, n, 1, n);
  
   //
   // Load the input for numerical recipes
   //
   //for (int i = 0; i < n; i++) {
   for (int i = (n - 1); i >= 0; i--) {
      for (int j = 0; j < n; j++) {
         au[i+1][j+1] = getElement(i, j);
      }
   }
   
   //
   // Do singular value decomposition
   //
   StatisticNumericalRecipes::svdcmp(au, n, n, w, v);
   
   //
   // Make the W+ matrix
   //
   const float VERY_SMALL_NUMBER = 0.001;
   StatisticMatrix wplus(n, n);
   wplus.setAllElements(0.0);
   for (int i = 0; i < n; i++) {
      const float wi = w[i+1];
      if (std::fabs(wi) < VERY_SMALL_NUMBER) {
         wplus.setElement(i, i, 0.0);
      }
      else {
         wplus.setElement(i, i, (1.0 / wi));
      }
   }
   if (StatisticAlgorithm::getDebugOn()) {
      std::cout << "W-vector: ";
      for (int i = 1; i <= n; i++) {
         std::cout << w[i] << ", ";
      }
      std::cout << std::endl;
      wplus.print(std::cout,
                  "",
                  "W-Plus");
   }
   
   //
   // Create the U-Transpose matrix
   //
   StatisticMatrix umatrix(n, n);
   for (int i = (n - 1); i >= 0; i--) {
      for (int j = 0; j < n; j++) {
         umatrix.setElement(i, j, au[i+1][j+1]);
      }
   }
   if (StatisticAlgorithm::getDebugOn()) {
      umatrix.print(std::cout,
                    "",
                    "U");
   }
   const StatisticMatrix uTranspose = umatrix.transpose();
   
   //
   // Create the V-Matrix
   //   
   StatisticMatrix vmatrix(n, n);
   for (int i = (n - 1); i >= 0; i--) {
      for (int j = 0; j < n; j++) {
         vmatrix.setElement(i, j, v[i+1][j+1]);
      }
   }
   if (StatisticAlgorithm::getDebugOn()) {
      vmatrix.print(std::cout,
                    "",
                    "V");
   }
   
   //
   // Pseudo inverse is [V][W+][UT]
   //
   const StatisticMatrix inverseMatrixOut = vmatrix.multiply(wplus).multiply(uTranspose);
   if ((inverseMatrixOut.getNumberOfRows() != n) ||
       (inverseMatrixOut.getNumberOfColumns() != n)) {
      throw StatisticException("Pseudo inverse matrix is not same size as input matrix.");
   }
   
   //
   // Free numerical recipes memory
   //
   StatisticNumericalRecipes::free_matrix(au, 1, n, 1, n);
   StatisticNumericalRecipes::free_matrix(v, 1, n, 1, n);
   StatisticNumericalRecipes::free_vector(w, 1, n);
   
   return inverseMatrixOut;
}

/**
 * multiply by a scalar.
 */
StatisticMatrix 
StatisticMatrix::multiplyByScalar(const double value) const
{
   StatisticMatrix matrixOut = *this;
   const int num = numberOfRows * numberOfColumns;
   for (int i = 0; i < num; i++) {
      matrixOut.data[i] *= value;
   }
   return matrixOut;
}      

/**
 * multiply by a matrix (this * matrixIn).
 */
StatisticMatrix 
StatisticMatrix::multiply(const StatisticMatrix& otherMatrix) const throw (StatisticException)
{
   StatisticMatrix matrixOut;
   
   if (numberOfColumns != otherMatrix.numberOfRows) {
      throw StatisticException(
         "StatisticMatrix::multiply: Number of column in this matrix is "
         "different than the number of rows in the other matrix.");
   }
   
   //
   // Output matrix has same number of rows as "this" matrix
   // Output matrix has same number of columns as "other" matrix
   //
   matrixOut.setDimensions(numberOfRows, otherMatrix.numberOfColumns);
   
   for (int i = 0; i < numberOfRows; i++) {
      for (int j = 0; j < otherMatrix.numberOfColumns; j++) {
         double value = 0.0;
         if (StatisticAlgorithm::getDebugOn()) {
            std::cout << "(" << i << "," << j << ") = ";
         }
         for (int k = 0; k < numberOfColumns; k++) {
            value += getElement(i, k) * otherMatrix.getElement(k, j);
            if (StatisticAlgorithm::getDebugOn()) {
               std::cout << "A(" << i << "," << k << ")*";
               std::cout << "B(" << k << "," << j << ")   ";
               std::cout << "IJK=(" << i << "," << j << "," << k << ")     ";
            }
         }
         if (StatisticAlgorithm::getDebugOn()) {
            std::cout << std::endl;
         }
         matrixOut.setElement(i, j, value);
         //std::cout << std::endl;
      }
   }
   
   return matrixOut;
}
      
/**
 * transpose a matrix.
 */
StatisticMatrix 
StatisticMatrix::transpose() const
{
   StatisticMatrix matrixOut;
   
   if ((numberOfRows > 0) &&
       (numberOfColumns > 0)) {
      matrixOut.setDimensions(numberOfColumns, numberOfRows);
      for (int i = 0; i < numberOfRows; i++) {
         for (int j = 0; j < numberOfColumns; j++) {
            matrixOut.setElement(j, i, getElement(i, j));
         }
      }
   }
   
   return matrixOut;
}
      



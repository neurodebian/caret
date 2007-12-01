
#ifndef __STATISTIC_MATRIX_H__
#define __STATISTIC_MATRIX_H__

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

#include <ostream>
#include <string>

#include "StatisticException.h"

/// class for matrix operations
/// element [0,0] is in the top left corner, i increases down, j increases right
class StatisticMatrix {
   public:
      // constructor
      StatisticMatrix();
      
      // constructor
      StatisticMatrix(const int numRows,
                      const int numColumns);
      
      // copy constructor
      StatisticMatrix(const StatisticMatrix& sm);
      
      // destructor
      ~StatisticMatrix();
      
      // Assignment operator. 
      StatisticMatrix& operator=(const StatisticMatrix& sm);
 
      // Equality operator
      bool operator==(const StatisticMatrix& sm);
      
      // print a matrix
      void print(std::ostream& stream,
                 const std::string& offset,
                 const std::string& matrixName) const;
      
      /// get the number of rows
      int getNumberOfRows() const { return numberOfRows; }
      
      /// get the number of columns
      int getNumberOfColumns() const { return numberOfColumns; }
      
      // set the dimensions of a matrix
      void setDimensions(const int numRows,
                         const int numColumns);
                        
      // get an element from the matrix
      double getElement(const int rowNumber,
                       const int columnNumber) const;
       
      // set the matrix from a one-dimensional array starting with "top row" of elements
      // matrix must already be set to a number of rows and columns
      void setMatrixFromOneDimensionalArray(const float dataIn[]);
      
      // set the matrix from a one-dimensional array starting with "top row" of elements
      // matrix must already be set to a number of rows and columns
      void setMatrixFromOneDimensionalArray(const double dataIn[]);
      
      // set an element from the matrix
      void setElement(const int rowNumber,
                      const int columnNumber,
                      const float value);
       
      // set an element from the matrix
      void setElement(const int rowNumber,
                      const int columnNumber,
                      const double value);
       
      // set all elements to value
      void setAllElements(const float value);
      
      /// get the inverse of a matrix
      StatisticMatrix inverse() const throw (StatisticException);
      
      /// multiply by a matrix (this * matrixIn)
      StatisticMatrix multiply(const StatisticMatrix& sm) const throw (StatisticException);
      
      /// multiply by a scalar
      StatisticMatrix multiplyByScalar(const double value) const;
      
      /// transpose a matrix
      StatisticMatrix transpose() const;
      
   protected:
      /// copy data for assignment operator and copy constructor
      void copyHelper(const StatisticMatrix& sm);
      
      /// get the location of an element in the matrix
      int getElementIndex(const int rowNumber,
                          const int columnNumber) const;
                          
      /// the matrix data
      double* data;
      
      /// number of rows in the matrix
      int numberOfRows;
      
      /// number of columns in the matrix
      int numberOfColumns;
};

#endif // __STATISTIC_MATRIX_H__

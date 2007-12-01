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

#include <iostream>
#include <limits>
#include <sstream>

#include "StatisticDataGroup.h"
#include "StatisticGeneratePValue.h"
#include "StatisticMatrix.h"
#include "StatisticMultipleRegression.h"

/**
 * constructor.
 */
StatisticMultipleRegression::StatisticMultipleRegression()
   : StatisticAlgorithm("Multiple Linear Regression")
{
   dependentDataGroup = NULL;
}

/**
 * destructor.
 */
StatisticMultipleRegression::~StatisticMultipleRegression()
{
   independentDataGroups.clear(); // do not delete elements !!
   dependentDataGroup = NULL;
}

/**
 * execute the algorithm.
 * 
 * This algorithm is from Applied Linear Regression Models
 *                        John Neter, William Wasserman, and Michael H. Kutner
 *                        Second Edition
 *                        Page 252
 */
void 
StatisticMultipleRegression::execute() throw (StatisticException)
{
   //
   // Check inputs
   //
   if (dependentDataGroup == NULL) {
      throw StatisticException("Dependent data group is invalid (NULL)");
   }
   const int numIndependentDataGroups = static_cast<int>(independentDataGroups.size());
   if (numIndependentDataGroups <= 0) {
      throw StatisticException("No Independent data groups ");
   }
   for (int i = 0; i < numIndependentDataGroups; i++) {
      if (independentDataGroups[i] == NULL) {
         std::ostringstream str;
         str << "Independent data group "
             << i
             << " is invalid (NULL)";
         throw StatisticException(str.str());
      }
   }
   const int numData = dependentDataGroup->getNumberOfData();
   if (numData <= 0) {
      throw StatisticException("Dependent data group contains no elements.");
   }
   for (int i = 0; i < numIndependentDataGroups; i++) {
      if (independentDataGroups[i]->getNumberOfData() != numData) {
         std::ostringstream str;
         str << "Independent data group "
             << i
             << " has a different number of elements than dependent data group.";
         throw StatisticException(str.str());
      }
   }
   
   //
   // Y - the Dependent variables matrix
   //
   Y.setDimensions(numData, 1);

   //
   // X - The indepenent variables matrix
   //
   X.setDimensions(numData, numIndependentDataGroups + 1);

   //
   // Load the matrices
   //
   for (int i = 0; i < numData; i++) {
      Y.setElement(i, 0, dependentDataGroup->getData(i));
      X.setElement(i, 0, 1.0);
      for (int j = 0; j < numIndependentDataGroups; j++) {
         X.setElement(i, (j + 1), independentDataGroups[j]->getData(i));
      }
   } 
   
   if (getDebugOn()) {
      X.print(std::cout, "   ", "X");
      Y.print(std::cout, "   ", "Y");
   }

   //
   // Transpose of X
   //
   Xt = X.transpose();
   if (getDebugOn()) {
      Xt.print(std::cout, "   ", "Xt");
   }
   
   //
   // X-transpose times X
   //
   const StatisticMatrix XtX = Xt.multiply(X);
   if (getDebugOn()) {
      XtX.print(std::cout, "   ", "XtX");
   }
   
   //
   // Inverse of X-transpose times X
   //
   const StatisticMatrix IXtX = XtX.inverse();
   if (getDebugOn()) {
      IXtX.print(std::cout, "   ", "IXtX");
   }
   
   //
   // X-transpose times Y
   //
   const StatisticMatrix XtY = Xt.multiply(Y);
   if (getDebugOn()) {
      XtY.print(std::cout, "   ", "XtY");
   }
   
   //
   // Calculate the coefficients
   //
   b = IXtX.multiply(XtY);
   if (getDebugOn()) {
      b.print(std::cout, "   ", "b");
   }
   
   //
   // Make coefficients avaialble to the user
   //
   coefficients.clear();
   for (int i = 0; i < b.getNumberOfRows(); i++) {
      coefficients.push_back(b.getElement(i, 0));
   }
}

/**
 * get the ANOVA parameters (must be called AFTER execute()).
 * This algorithm is from Applied Linear Regression Models
 *                        John Neter, William Wasserman, and Michael H. Kutner
 *                        Second Edition
 *                        Page 256
 */
void 
StatisticMultipleRegression::getAnovaParameters(float& SSTO,
                                                float& SSE,
                                                float& SSR,
                                                float& MSR,
                                                float& MSE,
                                                float& F,
                                                float& pValue,
                                                float& R2,
                                                int& regressionDOF,
                                                int& errorDOF,
                                                int& totalDOF) throw (StatisticException)
{   
   const int numData = dependentDataGroup->getNumberOfData();

   //
   // transpose of Y
   //
   const StatisticMatrix Yt = Y.transpose();
   
   //
   // Y-transpose times Y
   //
   const StatisticMatrix YtY = Yt.multiply(Y);  
   if ((YtY.getNumberOfRows() != 1) &&
       (YtY.getNumberOfColumns() != 1)) {
      std::ostringstream str;
      str << "YtY Matrix should be a 1x1 matrix but is "
          << YtY.getNumberOfRows() 
          << "x"
          << YtY.getNumberOfColumns()
          << ".";
      throw StatisticException(str.str());
   }
   const double YtYValue = YtY.getElement(0, 0);
   
   //
   // J is matrix of all ones
   //
   StatisticMatrix J(numData, numData);
   J.setAllElements(1.0);
   
   //
   // (1/N)Y'JY
   //
   const StatisticMatrix YtJY = Yt.multiply(J).multiply(Y);
   if ((YtJY.getNumberOfRows() != 1) &&
       (YtJY.getNumberOfColumns() != 1)) {
      std::ostringstream str;
      str << "Y'JY Matrix should be a 1x1 matrix but is "
          << YtJY.getNumberOfRows() 
          << "x"
          << YtJY.getNumberOfColumns()
          << ".";
      throw StatisticException(str.str());
   }
   const double YtJYValue = YtJY.getElement(0, 0) / static_cast<double>(numData);
   
   //
   // SSTO = Y'Y - (1/n)Y'JY;
   //
   SSTO = YtYValue - YtJYValue;
   
   //
   // Transpose of b
   //
   const StatisticMatrix bt = b.transpose();
   
   //
   // b'X'Y
   //
   const StatisticMatrix btXtY = bt.multiply(Xt).multiply(Y);
   if ((btXtY.getNumberOfRows() != 1) &&
       (btXtY.getNumberOfColumns() != 1)) {
      std::ostringstream str;
      str << "b'X'Y Matrix should be a 1x1 matrix but is "
          << btXtY.getNumberOfRows() 
          << "x"
          << btXtY.getNumberOfColumns()
          << ".";
      throw StatisticException(str.str());
   }
   const double btXtYValue = btXtY.getElement(0, 0);;
   
   //
   // SSE = Y'Y - b'X'Y from p 256
   //
   SSE = YtYValue - btXtYValue;
   
   //
   // SSR = SSTO - SSE
   //
   SSR = SSTO - SSE;
   
   //
   // Degrees of Freedom
   //
   regressionDOF = getNumberOfIndependentDataGroups();
   errorDOF = numData - (getNumberOfIndependentDataGroups() + 1);
   totalDOF = regressionDOF + errorDOF;
   
   //
   // MSR
   //
   MSR = SSR / static_cast<double>(regressionDOF);
   
   //
   // MSE
   //
   MSE = SSE / static_cast<double>(errorDOF);
   
   //
   // F
   //
   F = MSR / MSE;
   
   //
   // P-Value for F
   //
   pValue = StatisticGeneratePValue::getFStatisticPValue(regressionDOF,
                                                         errorDOF,
                                                         F);

   //
   // Coefficient of Multiple Determination
   //
   R2 = SSR / SSTO;
}

/**
 * get the regression coefficients.
 */
void 
StatisticMultipleRegression::getRegressionCoefficients(std::vector<float>& coefficientsOut) const
{
   coefficientsOut = coefficients;
}

/**
 * set the number of independent data groups.
 */
void 
StatisticMultipleRegression::setNumberOfIndependentDataGroups(const int numGroups)
{
   independentDataGroups.resize(numGroups, NULL);
}
      
/**
 * set the independent data group.
 */
void 
StatisticMultipleRegression::setIndependentDataGroup(const int groupNumber,
                                                     StatisticDataGroup* dataGroup,
                                                     const bool takeOwnershipOfThisDataGroup)
{
   const int indx = addDataGroup(dataGroup, takeOwnershipOfThisDataGroup);
   independentDataGroups[groupNumber] = getDataGroup(indx);
}
                             
/**
 * set the independent data (must call setNumberOfIndependentDataGroups() before this).
 */
void 
StatisticMultipleRegression::setIndependentDataArray(const int groupNumber,
                                                     const float* array,
                                                     const int numItemsInArray,
                                                     const bool takeOwnershipOfTheData)
{
   StatisticDataGroup::DATA_STORAGE_MODE storageMode = StatisticDataGroup::DATA_STORAGE_MODE_POINT;
   if (takeOwnershipOfTheData) {
      storageMode = StatisticDataGroup::DATA_STORAGE_MODE_TAKE_OWNERSHIP;
   }
   StatisticDataGroup* sdg = new StatisticDataGroup(array,
                                                    numItemsInArray,
                                                    storageMode);
   setIndependentDataGroup(groupNumber, sdg, true);
}
                                   
/**
 * set the dependent data group.
 */
void 
StatisticMultipleRegression::setDependentDataGroup(StatisticDataGroup* dataGroup,
                                                 const bool takeOwnershipOfThisDataGroup)
{
   const int indx = addDataGroup(dataGroup, takeOwnershipOfThisDataGroup);
   dependentDataGroup = getDataGroup(indx);
}

/**
 * set the dependent data.
 */
void 
StatisticMultipleRegression::setDependentDataArray(const float* array,
                                                   const int numItemsInArray,
                                                   const bool takeOwnershipOfTheData)
{
   StatisticDataGroup::DATA_STORAGE_MODE storageMode = StatisticDataGroup::DATA_STORAGE_MODE_POINT;
   if (takeOwnershipOfTheData) {
      storageMode = StatisticDataGroup::DATA_STORAGE_MODE_TAKE_OWNERSHIP;
   }
   StatisticDataGroup* sdg = new StatisticDataGroup(array,
                                                    numItemsInArray,
                                                    storageMode);
   setDependentDataGroup(sdg, true);
}       

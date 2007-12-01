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

#include <limits>

#include "StatisticDataGroup.h"
#include "StatisticLinearRegression.h"

/**
 * constructor.
 */
StatisticLinearRegression::StatisticLinearRegression()
   : StatisticAlgorithm("Linear Regression")
{
   dependentDataGroup = NULL;
   independentDataGroup = NULL;
   b0 = 0.0;
   b1 = 0.0;
}

/**
 * destructor.
 */
StatisticLinearRegression::~StatisticLinearRegression()
{
}

/**
 * execute the algorithm.
 * 
 * This algorithm is from Applied Linear Regression Models
 *                        John Neter, William Wasserman, and Michael H. Kutner
 *                        Second Edition
 *                        Page 42
 */
void 
StatisticLinearRegression::execute() throw (StatisticException)
{
   //
   // Check inputs
   //
   if (dependentDataGroup == NULL) {
      throw StatisticException("Dependent data group is invalid (NULL)");
   }
   if (independentDataGroup == NULL) {
      throw StatisticException("Independent data group is invalid (NULL)");
   }
   if (independentDataGroup->getNumberOfData() <= 0) {
      throw StatisticException("Independent data group contains zero elements");
   }
   const int numData = dependentDataGroup->getNumberOfData();
   if (numData != independentDataGroup->getNumberOfData()) {
      throw StatisticException("Independent and dependent data groups have a different number of elements");
   }
   
   //
   // Get pointers to data 
   //
   const float* xi = independentDataGroup->getPointerToData();
   const float* yi = dependentDataGroup->getPointerToData();
   
   //
   // Means of data
   //
   const float meanXBar = independentDataGroup->getMeanOfData();
   const float meanYBar = dependentDataGroup->getMeanOfData();
   
   //
   // Calculate b1, the slope
   //
   double denomenator = 0.0;
   double numerator = 0.0;
   for (int i = 0; i < numData; i++) {
      const float xdev = xi[i] - meanXBar;
      const float ydev = yi[i] - meanYBar;
      denomenator += xdev * xdev;
      numerator += xdev * ydev;
   }
   if (denomenator != 0.0) {
      b1 = numerator / denomenator;
   }
   else {
      //
      // Must be a vertical line since all Xi are the same
      //
      b1 = std::numeric_limits<float>::max();
   }
   
   //
   // Calculate b0, the intercept
   //
   b0 = meanYBar - b1 * meanXBar;
}

/**
 * get the regression coefficients (B0 = intercept, B1 = slope).
 */
void 
StatisticLinearRegression::getRegressionCoefficients(float& b0out, float& b1out) const
{
   b0out = b0;
   b1out = b1;
}

/**
 * set the independent data group.
 */
void 
StatisticLinearRegression::setIndependentDataGroup(StatisticDataGroup* dataGroup,
                                                   const bool takeOwnershipOfThisDataGroup)
{
   const int indx = addDataGroup(dataGroup, takeOwnershipOfThisDataGroup);
   independentDataGroup = getDataGroup(indx);
}
                             
/**
 * set the independent data array.
 */
void 
StatisticLinearRegression::setIndependentDataArray(const float* array,
                                                   const int numItemsInArray,
                                                   const bool takeOwnershipOfThisDataArray)
{
   StatisticDataGroup::DATA_STORAGE_MODE storageMode = StatisticDataGroup::DATA_STORAGE_MODE_POINT;
   if (takeOwnershipOfThisDataArray) {
      storageMode = StatisticDataGroup::DATA_STORAGE_MODE_TAKE_OWNERSHIP;
   }
   StatisticDataGroup* sdg = new StatisticDataGroup(array,
                                                    numItemsInArray,
                                                    storageMode);
   setIndependentDataGroup(sdg, true);
}
                                   
/**
 * set the dependent data group.
 */
void 
StatisticLinearRegression::setDependentDataGroup(StatisticDataGroup* dataGroup,
                                                 const bool takeOwnershipOfThisDataGroup)
{
   const int indx = addDataGroup(dataGroup, takeOwnershipOfThisDataGroup);
   dependentDataGroup = getDataGroup(indx);
}

/**
 * set the dependent data array.
 */
void 
StatisticLinearRegression::setDependentDataArray(const float* array,
                                                 const int numItemsInArray,
                                                 const bool takeOwnershipOfThisDataArray)
{
   StatisticDataGroup::DATA_STORAGE_MODE storageMode = StatisticDataGroup::DATA_STORAGE_MODE_POINT;
   if (takeOwnershipOfThisDataArray) {
      storageMode = StatisticDataGroup::DATA_STORAGE_MODE_TAKE_OWNERSHIP;
   }
   StatisticDataGroup* sdg = new StatisticDataGroup(array,
                                                    numItemsInArray,
                                                    storageMode);
   setDependentDataGroup(sdg, true);
}                                   

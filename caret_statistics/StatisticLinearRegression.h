
#ifndef __STATISTIC_LINEAR_REGRESSION_H__
#define __STATISTIC_LINEAR_REGRESSION_H__

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

#include "StatisticAlgorithm.h"

/// class for computing linear regression
class StatisticLinearRegression : public StatisticAlgorithm {
   public:
      // constructor
      StatisticLinearRegression();
      
      // destructor
      ~StatisticLinearRegression();
      
      // execute the algorithm
      void execute() throw (StatisticException);
      
      // get the regression coefficients (b0 = intercept, b1 = slope)
      void getRegressionCoefficients(float& b0out, float& b1out) const;

      // set the independent data group
      void setIndependentDataGroup(StatisticDataGroup* dataGroup,
                                   const bool takeOwnershipOfThisDataGroup = false);
                                   
      // set the independent data array
      void setIndependentDataArray(const float* array,
                                   const int numItemsInArray,
                                   const bool takeOwnershipOfThisDataArray = false);
                                   
      // set the dependent data group
      void setDependentDataGroup(StatisticDataGroup* dataGroup,
                                 const bool takeOwnershipOfThisDataGroup = false);
                                   
      // set the dependent data array
      void setDependentDataArray(const float* array,
                                 const int numItemsInArray,
                                 const bool takeOwnershipOfThisDataArray = false);
                                   
   protected:
      // b0, the intercept
      float b0;
      
      // Beta 1, the slope
      float b1;
      
      // the independent data group (X)
      StatisticDataGroup* independentDataGroup;
      
      // the dependent data group (Y)
      StatisticDataGroup* dependentDataGroup;
};

#endif // __STATISTIC_LINEAR_REGRESSION_H__

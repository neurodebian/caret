
#ifndef __STATISTIC_MULTIPLE_LINEAR_REGRESSION_H__
#define __STATISTIC_MULTIPLE_LINEAR_REGRESSION_H__

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
#include "StatisticMatrix.h"

/// class for computing multiple linear regression
class StatisticMultipleRegression : public StatisticAlgorithm {
   public:
      // constructor
      StatisticMultipleRegression();
      
      // destructor
      ~StatisticMultipleRegression();
      
      // execute the algorithm
      void execute() throw (StatisticException);
      
      // get the regression coefficients
      void getRegressionCoefficients(std::vector<float>& coefficientsOut) const;

      // set the number of independent data groups
      void setNumberOfIndependentDataGroups(const int numGroups);
      
      // get the number of independent data groups
      int getNumberOfIndependentDataGroups() const { return independentDataGroups.size(); }
      
      
      // set the independent data group (must call setNumberOfIndependentDataGroups() before this)
      void setIndependentDataGroup(const int groupNumber,
                                   StatisticDataGroup* dataGroup,
                                   const bool takeOwnershipOfTheData = false);
                                   
      // set the independent data (must call setNumberOfIndependentDataGroups() before this)
      void setIndependentDataArray(const int groupNumber,
                                   const float* array,
                                   const int numItemsInArray,
                                   const bool takeOwnershipOfThisData = false);
                                   
      // set the dependent data group
      void setDependentDataGroup(StatisticDataGroup* dataGroup,
                                 const bool takeOwnershipOfTheData = false);
       
      // set the dependent data 
      void setDependentDataArray(const float* array,
                                 const int numItemsInArray,
                                 const bool takeOwnershipOfThisData = false);
       
      // get the ANOVA parameters (must be called AFTER execute())
      void getAnovaParameters(float& SSTO,
                              float& SSE,
                              float& SSR,
                              float& MSR,
                              float& MSE,
                              float& F,
                              float& pValue,
                              float& R2,
                              int& regressionDOF,
                              int& errorDOF,
                              int& totalDOF) throw (StatisticException);
                              
   protected:
      // coefficients
      std::vector<float> coefficients;
      
      // the independent data groups (X) (do not delete elements in StatisticAlgorithm will)
      std::vector<StatisticDataGroup*> independentDataGroups;
      
      // the dependent data group (Y) 
      StatisticDataGroup* dependentDataGroup;
      
      // X - The indepenent variables matrix
      StatisticMatrix X;

      // Y - the Dependent variables matrix
      StatisticMatrix Y;

      // Transpose of X
      StatisticMatrix Xt;
      
      // the coefficients
      StatisticMatrix b;
};

#endif // __STATISTIC_MULTIPLE_LINEAR_REGRESSION_H__

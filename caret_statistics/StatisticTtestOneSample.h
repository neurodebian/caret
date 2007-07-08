
#ifndef __STATISTIC_T_TEST_ONE_SAMPLE_H__
#define __STATISTIC_T_TEST_ONE_SAMPLE_H__

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

/// algorithm for performing a one-sample T Test
class StatisticTtestOneSample : public StatisticAlgorithm {
   public:
      // constructor
      StatisticTtestOneSample(const float testMeanValueIn = 0);
      
      // destructor
      ~StatisticTtestOneSample();
      
      // execute the algorithm
      void execute() throw (StatisticException);
   
      // get the t-Value
      float getTValue() const { return tValue; }
      
      /// get the degrees of freedom
      int getDegreesOfFreedom() const { return degreesOfFreedom; }
      
      /// get the p-value
      float getPValue() const { return pValue; }
      
      /// use this value for the variance override
      void setVarianceOverride(const float varianceOverrideIn,
                               const bool varianceOverrideFlagIn);
                                    
   protected:
      /// test to see if mean of data group is different than this value
      float testMeanValue;
      
      /// the t-value
      float tValue;
      
      /// the degrees of freedom
      int degreesOfFreedom;
      
      /// the p-value
      float pValue;
      
      /// variance override value
      float varianceOverride;
      
      /// use man and deviation override values
      bool varianceOverrideFlag;
};

#endif // __STATISTIC_T_TEST_ONE_SAMPLE_H__

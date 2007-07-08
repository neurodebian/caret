#ifndef __STATISTIC_T_TEST_TWO_SAMPLE_H__
#define __STATISTIC_T_TEST_TWO_SAMPLE_H__

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

/// algorithm for performing a two-sample T Test
class StatisticTtestTwoSample : public StatisticAlgorithm {
   public:
      /// variance type
      enum VARIANCE_TYPE {
         /// pooled variance (the variances of the sample are the same)
         VARIANCE_TYPE_POOLED,
         /// unpooled variance (the variances of the samples are not the same)
         VARIANCE_TYPE_UNPOOLED
      };
      
      // constructor
      StatisticTtestTwoSample(const VARIANCE_TYPE varianceTypeIn);
      
      // destructor
      ~StatisticTtestTwoSample();
      
      // execute the algorithm
      void execute() throw (StatisticException);
   
      // get the t-Value
      float getTValue() const { return tValue; }
      
      /// get the degrees of freedom
      float getDegreesOfFreedom() const { return degreesOfFreedom; }
      
      /// get the p-value
      float getPValue() const { return pValue; }

      /// use this value for the variance override
      void setVarianceOverride(const int groupIndex,
                               const float varianceOverrideIn[2],
                               const bool varianceOverrideFlagIn[2]);

   protected:
      /// the variance type
      VARIANCE_TYPE varianceType;
      
      /// the t-value
      float tValue;
      
      /// the degrees of freedom
      float degreesOfFreedom;
      
      /// the p-value
      float pValue;
      
      /// variance override value
      float varianceOverride[2];
      
      /// use man and deviation override values
      bool varianceOverrideFlag[2];
};

#endif // __STATISTIC_T_TEST_TWO_SAMPLE_H__

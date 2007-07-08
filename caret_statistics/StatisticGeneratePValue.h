
#ifndef __STATISTIC_GENERATE_P_VALUE_H__
#define __STATISTIC_GENERATE_P_VALUE_H__

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

/// generate P-Values from a statistic and degrees-of-freedom
class StatisticGeneratePValue : public StatisticAlgorithm {
   public:
      // generate P-Value for an F-Statistic
      static float getFStatisticPValue(const float numeratorDegreesOfFreedom,
                                       const float denominatorDegreesOfFreedom,
                                       const float F);
         
      // generate P-Value for One-Tailed T-Test
      static float getOneTailTTestPValue(const float degreesOfFreedom,
                                         const float T);
                                         
      // generate P-Value for Two-Tailed T-Test
      static float getTwoTailTTestPValue(const float degreesOfFreedom,
                                         const float T);
                                         
      /// type of statistic
      enum INPUT_STATISTIC {
         /// input statistic if F-Statistic
         INPUT_STATISTIC_F,
         /// input statistic is T-statistic with one tale distribution
         INPUT_STATISTIC_T_ONE_TALE,
         /// input statistic is T-statistic with two tale distribution
         INPUT_STATISTIC_T_TWO_TALE
      };
      
      // constructor
      StatisticGeneratePValue(const INPUT_STATISTIC inputStatisticTypeIn);
      
      // destructor
      ~StatisticGeneratePValue();
      
      // generate the P-Values
      void execute() throw (StatisticException);
      
      // get the output data group that contains the P-values
      const StatisticDataGroup* getOutputDataGroupContainingPValues() const 
                                     { return outputDataGroupContainingPValues; }
      
   protected:
      /// type of input statistic
      INPUT_STATISTIC inputStatisticType;

      // from AFNI and used by tStatisticToPValue
      static double incbeta( double x , double p , double q , double beta );
                                          
      // from AFNI and used by tStatisticToPValue
      static double lnbeta( double p , double q );
                                          
      // from AFNI and used by tStatisticToPValue
      static double gamma( double x );
                                          
      // from AFNI and used by tStatisticToPValue
      static double gamma_asympt(double x);
                                          
      // from AFNI and used by tStatisticToPValue
      static double gamma_12( double y );
      
      // the output data group
      StatisticDataGroup* outputDataGroupContainingPValues;
};

#endif // __STATISTIC_GENERATE_P_VALUE_H__

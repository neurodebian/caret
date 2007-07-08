
#ifndef __STATISTIC_ANOVA_ONE_WAY_H__
#define __STATISTIC_ANOVA_ONE_WAY_H__

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

/// class that performs a one way analysis of variance
class StatisticAnovaOneWay : public StatisticAlgorithm {
   public:
      // constructor
      StatisticAnovaOneWay();
      
      // destructor
      ~StatisticAnovaOneWay();
      
      // execute the algorithm
      void execute() throw (StatisticException);
      
      /// get treatment sum of squares
      double getSumOfSquaresTreatmentSSTR() const { return sumOfSquaresTreatmentSSTR; }
      
      /// get error sum of squares
      double getSumOfSquaresErrorSSE() const { return  sumOfSquaresErrorSSE; }
      
      /// total sum of squares
      double getSumOfSquaresTotalSSTO() const { return  sumOfSquaresTotalSSTO; }
      
      /// mean treatment sum of squares
      double getMeanSumOfSquaresTreatmentMSTR() const { return  meanSumOfSquaresTreatmentMSTR; }
      
      /// mean error sum of squares
      double getMeanSumOfSquaresErrorMSE() const { return  meanSumOfSquaresErrorMSE; }
      
      /// between treatment degrees of freedom
      double getDegreesOfFreedomBetweenTreatments() const { return  degreesOfFreedomBetweenTreatments; }
      
      /// within treatment degrees of freedom
      double getDegreesOfFreedomWithinTreatments() const { return  degreesOfFreedomWithinTreatments; }
       
      /// degrees of freedom total
      double getDegreesOfFreedomTotal() const { return  degreesOfFreedomTotal; }
      
      /// F statistic
      double getFStatistic() const { return  fStatistic; }
      
      /// P-Value
      double getPValue() const { return pValue; }
      
   protected:
      /// treatment sum of squares
      double sumOfSquaresTreatmentSSTR;
      
      /// error sum of squares
      double sumOfSquaresErrorSSE;
      
      /// total sum of squares
      double sumOfSquaresTotalSSTO;
      
      /// mean treatment sum of squares
      double meanSumOfSquaresTreatmentMSTR;
      
      /// mean error sum of squares
      double meanSumOfSquaresErrorMSE;
      
      /// between treatment degrees of freedom
      double degreesOfFreedomBetweenTreatments;
      
      /// within treatment degrees of freedom
      double degreesOfFreedomWithinTreatments;
      
      /// degrees of freedom total
      double degreesOfFreedomTotal;
      
      /// F statistic
      double fStatistic;
      
      /// P-Value
      double pValue;
};

#endif // __STATISTIC_ANOVA_ONE_WAY_H__


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

#include "StatisticAnovaOneWay.h"
#include "StatisticDataGroup.h"
#include "StatisticGeneratePValue.h"
#include "StatisticMeanAndDeviation.h"

/**
 * constructor.
 */
StatisticAnovaOneWay::StatisticAnovaOneWay()
   : StatisticAlgorithm("ANOVA One-Way")
{
}

/**
 * destructor.
 */
StatisticAnovaOneWay::~StatisticAnovaOneWay()
{
}

/**
 * execute the algorithm.
 * Formulas are from Analysis of Variance for FMRI Data
 * By Douglas Ward (with modifications by Gang Chen)
 * January 10, 2006
 * http://afni.nimh.nih.gov/afni/doc/manual/ANOVA
 */
void 
StatisticAnovaOneWay::execute() throw (StatisticException)
{
   sumOfSquaresTreatmentSSTR = 0.0;
   sumOfSquaresErrorSSE = 0.0;
   sumOfSquaresTotalSSTO = 0.0;
   meanSumOfSquaresTreatmentMSTR = 0.0;
   meanSumOfSquaresErrorMSE = 0.0;
   degreesOfFreedomBetweenTreatments = 0.0;
   degreesOfFreedomWithinTreatments = 0.0;
   degreesOfFreedomTotal = 0.0;
   fStatistic = 0.0;
   pValue = 0.0;
   
   //
   // Verify that there are groups.  Each group is a factor level.
   //
   const int numberOfFactorLevelsR = getNumberOfDataGroups();
   if (numberOfFactorLevelsR < 2) {
      throw StatisticException("One-way ANOVA requires at least two data groups.");
   }

   //
   // Data Groups and Number of observations in each factor level
   //
   std::vector<StatisticDataGroup*> dataGroupObservations(numberOfFactorLevelsR);
   std::vector<int> numberOfObservationsN(numberOfFactorLevelsR);
   for (int i = 0; i < numberOfFactorLevelsR; i++) {
      dataGroupObservations[i] = getDataGroup(i);
      numberOfObservationsN[i] = dataGroupObservations[i]->getNumberOfData();
   }
   
   //
   // Compute mean at each factor and overall mean
   //
   double overallMeanYBar = 0.0;
   int totalOfObservationsY = 0;
   std::vector<double> factorMeanYiBar(numberOfFactorLevelsR, 0.0);
   for (int i = 0; i < numberOfFactorLevelsR; i++) {
      const int ni = numberOfObservationsN[i];
      for (int j = 0; j < ni; j++) {
         factorMeanYiBar[i] += dataGroupObservations[i]->getData(j);
      }
      
      totalOfObservationsY += ni;
      overallMeanYBar += factorMeanYiBar[i];
      
      factorMeanYiBar[i] /= static_cast<double>(ni);
      
   }
   overallMeanYBar /= static_cast<double>(totalOfObservationsY);
   
   //
   // Compute the various Sums Of Squares
   //
   for (int i = 0; i < numberOfFactorLevelsR; i++) {
      //
      // Treatment Sum Of Squares
      //
      const double tr = (factorMeanYiBar[i] - overallMeanYBar);
      sumOfSquaresTreatmentSSTR += (numberOfObservationsN[i] * tr * tr);
                                   
      const int numObsI = numberOfObservationsN[i];
      for (int j = 0; j < numObsI; j++) {
         //
         // Data element "j" of the i'th factor
         //
         const float Yij = dataGroupObservations[i]->getData(j);
         
         //
         // error sum of squares
         //
         const double te = (Yij - factorMeanYiBar[i]);
         sumOfSquaresErrorSSE += (te * te);
         
         //
         // total sum of squares
         //
         const double ts = (Yij - overallMeanYBar);
         sumOfSquaresTotalSSTO += (ts * ts);
      }
   }
   
   //
   // Between treatments degrees of freedom
   //
   degreesOfFreedomBetweenTreatments = numberOfFactorLevelsR - 1;
   
   //
   // Within treatments degrees of freedom
   //
   for (int i = 0; i < numberOfFactorLevelsR; i++) {
      degreesOfFreedomWithinTreatments += numberOfObservationsN[i];
   }
   degreesOfFreedomWithinTreatments -= numberOfFactorLevelsR;
   
   //
   // Total degrees of freedom
   //
   for (int i = 0; i < numberOfFactorLevelsR; i++) {
      degreesOfFreedomTotal += numberOfObservationsN[i];
   }
   degreesOfFreedomTotal -= 1.0;
   
   //
   // Mean treatment sum of squares
   //
   meanSumOfSquaresTreatmentMSTR = sumOfSquaresTreatmentSSTR / degreesOfFreedomBetweenTreatments;
   
   //
   //  mean error sum of squares
   //
   meanSumOfSquaresErrorMSE = sumOfSquaresErrorSSE / degreesOfFreedomWithinTreatments;
   
   //
   // F-statistic
   //
   if (meanSumOfSquaresErrorMSE == 0) {
      throw StatisticException("Unable to compute F-statistic because mean sum of squares (MSE) is zero.");
   }
   fStatistic = meanSumOfSquaresTreatmentMSTR / meanSumOfSquaresErrorMSE;
   
   pValue = 
        StatisticGeneratePValue::getFStatisticPValue(degreesOfFreedomBetweenTreatments,
                                                     degreesOfFreedomWithinTreatments,
                                                     fStatistic);
/*
   //
   // Determine P-Value
   //
   int which = 1;
   double p = 0.0;
   double q = 0.0;
   double f = fStatistic;
   double dfn = degreesOfFreedomBetweenTreatments;
   double dfd = degreesOfFreedomWithinTreatments;
   int status = 0;
   double bound = 0;
   cdff(&which,
        &p,
        &q,
        &f,
        &dfn,
        &dfd,
        &status,
        &bound);
   pValue = q;
*/
}

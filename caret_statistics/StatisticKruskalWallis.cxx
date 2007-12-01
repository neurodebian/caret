
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

#include "StatisticDataGroup.h"
#include "StatisticGeneratePValue.h"
#include "StatisticKruskalWallis.h"
#include "StatisticMeanAndDeviation.h"
#include "StatisticRankTransformation.h"

/**
 * constructor.
 */
StatisticKruskalWallis::StatisticKruskalWallis()
   : StatisticAlgorithm("Kruskal-Wallis")
{
}

/**
 * destructor.
 */
StatisticKruskalWallis::~StatisticKruskalWallis()
{
}

/**
 * execute the algorithm.
 * Formulas are from Applied Linear Statistical Models by John Neter, 
 * William Wasserman, and Michael H. Kutner, 3rd ed.
 */
void 
StatisticKruskalWallis::execute() throw (StatisticException)
{
   fStatistic = 0.0;
   pValue = 0.0;
   sumOfSquaresTreatmentSSTR = 0.0;
   sumOfSquaresErrorSSE = 0.0;
   meanSumOfSquaresTreatmentMSTR = 0.0;
   meanSumOfSquaresErrorMSE = 0.0;
   degreesOfFreedomBetweenTreatments = 0.0;
   degreesOfFreedomWithinTreatments = 0.0;
   degreesOfFreedomTotal = 0.0;
   
   //
   // Verify that there are groups.  Each group is a factor level.
   //
   const int numberOfDataGroupsR = getNumberOfDataGroups();
   if (numberOfDataGroupsR < 2) {
      throw StatisticException("Kruskal-Wallis requires at least two data groups.");
   }

   //
   // Transform the data into ranks
   //
   StatisticRankTransformation rankTransform;
   for (int i = 0; i < numberOfDataGroupsR; i++) {
      rankTransform.addDataGroup(getDataGroup(i));
   }
   rankTransform.execute();
   
   //
   // Get the transformed data
   //
   std::vector<const StatisticDataGroup*> dataGroupRanks(numberOfDataGroupsR);
   std::vector<int> numberOfRanksNi(numberOfDataGroupsR);
   std::vector<float> meanOfRanksRi(numberOfDataGroupsR);
   int totalNumberOfRanksNT = 0;
   for (int i = 0; i < numberOfDataGroupsR; i++) {
      dataGroupRanks[i] = rankTransform.getOutputDataGroupContainingRankValues(i);
      numberOfRanksNi[i] = dataGroupRanks[i]->getNumberOfData();
      meanOfRanksRi[i] = dataGroupRanks[i]->getMeanOfData();
      totalNumberOfRanksNT += numberOfRanksNi[i];
   }
   if (totalNumberOfRanksNT <= 0) {
      throw StatisticException("All data groups are empty.");
   }
   
   //
   // since ranks are 1..NT, overall mean is one-half of total number of ranks
   //
   const float overallMeanRBar = static_cast<float>(totalNumberOfRanksNT + 1) / 2.0;

   //
   // Compute Sum Squares of Treatment 
   //
   sumOfSquaresTreatmentSSTR = 0.0;
   for (int i = 0; i < numberOfDataGroupsR; i++) {
      const float value = (meanOfRanksRi[i] - overallMeanRBar);
      sumOfSquaresTreatmentSSTR += numberOfRanksNi[i] * (value * value);
   }
   
   //
   // Compute error sum of squares
   //
   sumOfSquaresErrorSSE = 0.0;
   for (int i = 0; i < numberOfDataGroupsR; i++) {
      for (int j = 0; j < numberOfRanksNi[i]; j++) {
         const float value = dataGroupRanks[i]->getData(j) - meanOfRanksRi[i];
         sumOfSquaresErrorSSE += value * value;
      }
   }
   
   //
   // Between treatments degrees of freedom
   //
   degreesOfFreedomBetweenTreatments = numberOfDataGroupsR - 1;
   
   //
   // Within treatments degrees of freedom
   //
   degreesOfFreedomWithinTreatments = totalNumberOfRanksNT - numberOfDataGroupsR;
   
   //
   // Total degrees of freedom
   //
   degreesOfFreedomTotal = degreesOfFreedomBetweenTreatments
                         + degreesOfFreedomWithinTreatments;
                         
   //
   // mean treatment sum of squares
   // 
   meanSumOfSquaresTreatmentMSTR = sumOfSquaresTreatmentSSTR
                                 / degreesOfFreedomBetweenTreatments;
                                 
   //
   // mean error sum of squares
   //
   meanSumOfSquaresErrorMSE = sumOfSquaresErrorSSE
                            / degreesOfFreedomWithinTreatments;
                            
   //
   // Calculate F-Statistic
   //
   if (meanSumOfSquaresErrorMSE == 0.0) {
      throw StatisticException("Unable to compute F-Statistic since Mean Sums of Squares Error (MSE) is zero.");
   }
   fStatistic = meanSumOfSquaresTreatmentMSTR
              / meanSumOfSquaresErrorMSE;
   
   //
   // Compute P-Value
   //
   pValue = 
        StatisticGeneratePValue::getFStatisticPValue(degreesOfFreedomBetweenTreatments,
                                                     degreesOfFreedomWithinTreatments,
                                                     fStatistic);
}

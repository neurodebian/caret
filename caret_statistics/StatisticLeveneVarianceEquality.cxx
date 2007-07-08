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

#include <cmath>
#include <iostream>

#include "StatisticDataGroup.h"
#include "StatisticGeneratePValue.h"
#include "StatisticLeveneVarianceEquality.h"
#include "StatisticMeanAndDeviation.h"

/**
 * constructor.
 */
StatisticLeveneVarianceEquality::StatisticLeveneVarianceEquality()
{
   leveneF = 0.0;
   dof1    = 0.0;
   dof2    = 0.0;
   pValue  = 1;
}

/**
 * destructor.
 */
StatisticLeveneVarianceEquality::~StatisticLeveneVarianceEquality()
{
}

/**
 * execute the algorithm.
 */
void 
StatisticLeveneVarianceEquality::execute() throw (StatisticException)
{
   leveneF = 0.0;
   dof1    = 0.0;
   dof2    = 0.0;
   pValue  = 1;
   
   //
   // This algorithm is from Step 4 of
   //      http://www.people.vcu.edu/~wsstreet/courses/314_20033/Handout.Levene.pdf
   //
   // Note: variables are named as follows
   //    A descriptive name followed by an underscore followed by the name
   //    from the above web site PDF file
   //
   // Check number of groups (t)
   //
   const int numGroups_t = getNumberOfDataGroups();
   if (numGroups_t < 2) {
      throw StatisticException("There must be at least two groups for Levene's Test.");
   }
   
   //
   // Get the data groups
   //
   std::vector<StatisticDataGroup*> groups(numGroups_t);
   for (int i = 0; i < numGroups_t; i++) {
      groups[i] = getDataGroup(i);
   }
   
   //
   // Get the size of each group and the total number of data
   //
   int numberInAllGroups_N = 0;
   std::vector<int> numberInGroup_ni(numGroups_t);
   for (int i = 0; i < numGroups_t; i++) {
      numberInGroup_ni[i] = groups[i]->getNumberOfData();
      numberInAllGroups_N += numberInGroup_ni[i];
   }
   
   //
   // Get the mean for each group
   //
   std::vector<float> meanForGroup_yi(numGroups_t);
   for (int i = 0; i < numGroups_t; i++) {
      StatisticMeanAndDeviation smad;
      smad.addDataGroup(groups[i]);
      try {
         smad.execute();
      }
      catch (StatisticException&) {
      }
      meanForGroup_yi[i] = smad.getMean();
   }
   
   //
   // Get the average absolute deviation within each group
   //
   float allAverageAbsoluteDeviation_D = 0.0;
   std::vector<float> withinGroupAverageAbsoluteDeviation_Di(numGroups_t, 0.0);
   for (int i = 0; i < numGroups_t; i++) {
      for (int j = 0; j < numberInGroup_ni[i]; j++) {
         const float yij = groups[i]->getData(j);
         withinGroupAverageAbsoluteDeviation_Di[i] += (std::fabs(yij - meanForGroup_yi[i]));
      }
      
      //
      // Sum up for all groups
      //
      allAverageAbsoluteDeviation_D += withinGroupAverageAbsoluteDeviation_Di[i];
      
      //
      // average absolute deviation for group
      //
      if (numberInGroup_ni[i] > 0) {
         withinGroupAverageAbsoluteDeviation_Di[i] /= static_cast<float>(numberInGroup_ni[i]);
      }
   }
   if (numberInAllGroups_N > 0) {
      allAverageAbsoluteDeviation_D /= static_cast<float>(numberInAllGroups_N);
   }
   
   //
   // Determine the numerator
   //
   float numerator = 0.0;
   for (int i = 0; i < numGroups_t; i++) {
      const float d = withinGroupAverageAbsoluteDeviation_Di[i] - allAverageAbsoluteDeviation_D;
      numerator += numberInGroup_ni[i] * (d * d);
   }
   numerator /= static_cast<float>(numGroups_t - 1);
   
   //
   // Determine the denominator
   //
   float sum = 0.0;
   for (int i = 0; i < numGroups_t; i++) {
      for (int j = 0; j < numberInGroup_ni[i]; j++) {
         const float Dij = groups[i]->getData(j) - meanForGroup_yi[i];
         const float d = std::fabs(Dij - withinGroupAverageAbsoluteDeviation_Di[i]);
         sum += (d * d);
      }
   }
   float denominator = sum / static_cast<float>(numberInAllGroups_N - numGroups_t);
   if (denominator == 0.0) {
      denominator = 1.0;
   }
   
   //
   // Determine Levene's F
   //
   leveneF = numerator / denominator;
   dof1    = numGroups_t - 1;
   dof2    = numberInAllGroups_N - numGroups_t;
   
   //
   // Determine the P-Value
   // Note this->pValue is passed to the output group so it will get set by the p-value algorithm
   //
   StatisticDataGroup fGroup(&leveneF, 1, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup numeratorGroup(&dof1, 1, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup denominatorGroup(&dof2, 1, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticGeneratePValue genP(StatisticGeneratePValue::INPUT_STATISTIC_F);
   genP.addDataGroup(&fGroup);
   genP.addDataGroup(&numeratorGroup);
   genP.addDataGroup(&denominatorGroup);
   try {
      genP.execute();
      const StatisticDataGroup* pValuesDataGroup = genP.getOutputDataGroupContainingPValues();
      if (pValuesDataGroup->getNumberOfData() > 0) {
         pValue = pValuesDataGroup->getData(0);
      }
      else {
         throw StatisticException("StatisticGeneratePValue did not produce any output.");
      }
   }
   catch (StatisticException& e) {
      std::cout << "Generation of P-Value failed for Levene statistic: " << std::endl
                << "   " << e.whatStdString() << std::endl;
   }   
}

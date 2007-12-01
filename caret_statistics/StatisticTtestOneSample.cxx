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

#include <cmath>

#include "StatisticDataGroup.h"
#include "StatisticGeneratePValue.h"
#include "StatisticMeanAndDeviation.h"
#include "StatisticTtestOneSample.h"

/**
 * constructor.
 */
StatisticTtestOneSample::StatisticTtestOneSample(const float testMeanValueIn)
   : StatisticAlgorithm("T-Test One-Sample")
{
   varianceOverride = 0.0;
   varianceOverrideFlag = false;
   testMeanValue = testMeanValueIn;
}

/**
 * destructor.
 */
StatisticTtestOneSample::~StatisticTtestOneSample()
{
}

/**
 * use this value for the variance override.
 */
void 
StatisticTtestOneSample::setVarianceOverride(const float varianceOverrideIn,
                                             const bool varianceOverrideFlagIn)
{
   varianceOverride = varianceOverrideIn;
   varianceOverrideFlag = varianceOverrideFlagIn;
}
                                    
/**
 * execute the algorithm.
 * Formulas are from:
 *   A Aron and E Aron
 *   Statistics for Psychology (2nd Edition)
 *   Upper Saddle River, NJ
 *   Prentice Hall
 *   1999
 */
void 
StatisticTtestOneSample::execute() throw (StatisticException)
{
   tValue = 0.0;
   degreesOfFreedom = 0;
   pValue = 0.0;
   
   //
   // Verify that there is only one group
   //
   const int numGroups = getNumberOfDataGroups();
   if (numGroups != 1) {
      throw StatisticException("One Sample T-Test requires one and only one data array.");
   }
   
   //
   // Get the data group
   //
   StatisticDataGroup* dataGroup = getDataGroup(0);
   
   //
   // Get the number of items in the data group
   //
   const int numData = dataGroup->getNumberOfData();
   if (numData <= 0) {
      throw StatisticException("Data group sent to One Sample T-Test contains no data.");
   }
   
   float mean = 0.0;
   float estimatedPopulationVarianceSS = 0.0;
   
   if (numData == 1) {
      mean = dataGroup->getData(0);
   }
   else {
      //
      // Generate the mean and deviation
      //
      StatisticMeanAndDeviation smad;
      smad.addDataGroup(dataGroup);
      try {
         smad.execute();
      }
      catch (StatisticException& e) {
      }
      mean = smad.getMean();
      estimatedPopulationVarianceSS = smad.getPopulationSampleVariance();
   }
   
   //
   // Check for variance override
   //
   if (varianceOverrideFlag) {
      estimatedPopulationVarianceSS = varianceOverride;
   }
   
   //
   // Standard deviation of the distribution of means
   //
   const float standardDeviationDistributionOfMeansSM = 
                                   std::sqrt(estimatedPopulationVarianceSS
                                             / static_cast<float>(numData));
                                                                
   //
   // Generate the T-value
   //
   tValue = mean - testMeanValue;
   if (standardDeviationDistributionOfMeansSM != 0.0) {
      tValue /= standardDeviationDistributionOfMeansSM;
   }
   
   degreesOfFreedom = numData - 1;
   
   //
   // Generate the p-value
   //
   pValue = StatisticGeneratePValue::getOneTailTTestPValue(degreesOfFreedom, tValue);
/*
   int which = 1;  // generate P and Q
   double p = 0.0;
   double q = 0.0;
   double t = std::fabs(tValue);
   double df = degreesOfFreedom;
   int status = 0;
   double bound = 0.0;
   cdft(&which,
        &p,
        &q,
        &t,
        &df,
        &status,
        &bound);
   pValue = q;  // p is close to one so want q = 1 - p
*/
}

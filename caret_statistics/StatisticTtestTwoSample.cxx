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
#include "StatisticTtestTwoSample.h"

/**
 * constructor.
 */
StatisticTtestTwoSample::StatisticTtestTwoSample(const VARIANCE_TYPE varianceTypeIn)
   : StatisticAlgorithm("T-Test Two-Sample")
{
   varianceType = varianceTypeIn;
   varianceOverride[0] = 0.0;
   varianceOverride[1] = 0.0;
   varianceOverrideFlag[0] = false;
   varianceOverrideFlag[1] = false;
}

/**
 * destructor.
 */
StatisticTtestTwoSample::~StatisticTtestTwoSample()
{
}

/**
 * use this value for the variance override.
 */
void 
StatisticTtestTwoSample::setVarianceOverride(const int groupIndex,
                                          const float varianceOverrideIn[2],
                                          const bool varianceOverrideFlagIn[2])
{
   varianceOverride[groupIndex] = varianceOverrideIn[groupIndex];
   varianceOverrideFlag[groupIndex] = varianceOverrideFlagIn[groupIndex];
}
                                    
/**
 * execute the algorithm.
 * Formulas are from or equivalent to:
 *   http://www.statsdirect.com/help/parametric_methods/utt.htm
 */
void 
StatisticTtestTwoSample::execute() throw (StatisticException)
{
   tValue = 0.0;
   degreesOfFreedom = 0.0;
   pValue = 0.0;
   
   //
   // Verify that there are groups
   //
   const int numGroups = getNumberOfDataGroups();
   if (numGroups != 2) {
      throw StatisticException("Two-Sample T-Test requires exactly two data groups.");
   }
   
   //
   // Get the data group
   //
   StatisticDataGroup* dataGroup1 = getDataGroup(0);
   StatisticDataGroup* dataGroup2 = getDataGroup(1);
   
   //
   // Get the number of items in the data groups
   //
   const int numData1 = dataGroup1->getNumberOfData();
   const int numData2 = dataGroup2->getNumberOfData();
   if (numData1 <= 0) {
      throw StatisticException("First data group sent to Two-Sample T-Test has less than two elements.");
   }
   if (numData2 <= 0) {
      throw StatisticException("Second data group sent to Two-Sample T-Test has less than two elements.");
   }

   //
   // Compute mean, mean, dof, sum-squared, and 
   // variance of dist or diff of means values for first group
   //
   StatisticMeanAndDeviation smad1;
   smad1.addDataGroup(dataGroup1);
   try {
      smad1.execute();
   }
   catch (StatisticException& e) {
      std::string msg("StatisticTtestTwoSample mean/dev failed: "
                  + e.whatStdString());
      throw StatisticException(msg);
   }
   const float mean1 = smad1.getMean();
   const float df1  = numData1 - 1;
   float populationVariance1 = smad1.getPopulationSampleVariance();
   if (varianceOverrideFlag[0]) {
      populationVariance1 = varianceOverride[0];
   }
   
   //
   // Compute mean, mean, dof, sum-squared, and 
   // variance of dist or diff of means values for second group
   //
   StatisticMeanAndDeviation smad2;
   smad2.addDataGroup(dataGroup2);
   try {
      smad2.execute();
   }
   catch (StatisticException& e) {
      std::string msg("StatisticTtestTwoSample mean/dev failed: "
                  + e.whatStdString());
      throw StatisticException(msg);
   }
   const float mean2 = smad2.getMean();
   const float df2  = numData2 - 1;
   float populationVariance2 = smad2.getPopulationSampleVariance();
   if (varianceOverrideFlag[1]) {
      populationVariance2 = varianceOverride[1];
   }
   
   //
   // Denominator for T-Value computation
   //
   float denominator = 1.0;
   
   //
   // Compute variance based upon type of variance
   //
   switch (varianceType) {
      case VARIANCE_TYPE_POOLED:
         {
            //
            // degrees of freedom total for pooled variance
            //
            degreesOfFreedom = df1 + df2;
            
            //
            // Pooled population variance
            //
            const float pooledPopulationVariance =
               ((df1 * populationVariance1) + (df2 * populationVariance2))
               / degreesOfFreedom;
               
            //
            // Denominator for T-Value computation
            //
            denominator = std::sqrt(pooledPopulationVariance)
                          * std::sqrt((1.0 / static_cast<float>(numData1)) +
                                      (1.0 / static_cast<float>(numData2)));
         }
         break;
      case VARIANCE_TYPE_UNPOOLED:
         {
            //
            // denominator for unpooled variance T-Value
            //
            denominator = std::sqrt((populationVariance1 / static_cast<float>(numData1))
                                    + (populationVariance2 / static_cast<float>(numData2)));

            //
            // Degrees of freedom for unpooled variance
            // From http://www.itl.nist.gov/div898/handbook/eda/section3/eda353.htm
            //
            const float num1 = numData1;
            const float num2 = numData2;
            float numerator = populationVariance1/num1 + populationVariance2/num2;
            numerator = numerator * numerator;
            float denom = (1.0 / df1) * (populationVariance1/num1) 
                                      * (populationVariance1/num1)
                        + (1.0 / df2) * (populationVariance2/num2) 
                                      * (populationVariance2/num2);
            degreesOfFreedom = 0.0;
            if (denom != 0.0) {
               degreesOfFreedom = numerator / denom;
            }  
         }
   }
   
   //
   // Compute the T-Score
   //
   tValue = (mean1 - mean2) / denominator;
   
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


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

#include <algorithm>
#include <cmath>
#include <limits>

#include "StatisticDataGroup.h"
#include "StatisticDescriptiveStatistics.h"

/**
 * constructor.
 */
StatisticDescriptiveStatistics::StatisticDescriptiveStatistics()
{
   mean           = 0.0;
   dataSumSquared = 0.0;
   sumOfSquares   = 0.0;
   sumOfCubes     = 0.0;
   sumOfQuads     = 0.0;
   numberOfDataElements = 0;
}

/**
 * destructor.
 */
StatisticDescriptiveStatistics::~StatisticDescriptiveStatistics()
{
}

/**
 * generate the mean and deviation.
 * Formulas are from:
 *   A Aron and E Aron
 *   Statistics for Psychology (2nd Edition)
 *   Upper Saddle River, NJ
 *   Prentice Hall
 *   1999
 */
void 
StatisticDescriptiveStatistics::execute() throw (StatisticException)
{   
   //
   // Determine mean
   //
   double dataSum = 0.0;
   for (int i = 0; i < getNumberOfDataGroups(); i++) {
      const StatisticDataGroup* sdg = getDataGroup(i);
      const float* data = sdg->getPointerToData();
      const int numData = sdg->getNumberOfData();
      for (int j = 0; j < numData; j++) {
         const float d = data[j];
         dataSum += d;
         dataSumSquared += (d * d);
         numberOfDataElements++;
      }
   }
   
   if (numberOfDataElements <= 0) {
      return;
   }
   
   mean = dataSum / static_cast<float>(numberOfDataElements);
   
   //
   // Determine sum of squares
   //
   for (int i = 0; i < getNumberOfDataGroups(); i++) {
      const StatisticDataGroup* sdg = getDataGroup(i);
      const float* data = sdg->getPointerToData();
      const int numData = sdg->getNumberOfData();
      for (int j = 0; j < numData; j++) {
         const double diff = data[j] - mean;
         sumOfSquares += (diff * diff);
         sumOfCubes   += (sumOfSquares * diff);
         sumOfQuads   += (sumOfCubes * diff);
      }
   }   
}

/**
 * get the skewness.
 * http://www.statsdirect.com/help/basic_descriptive_statistics/desc.htm
 */
float 
StatisticDescriptiveStatistics::getSkewness() const
{
   float s = 0.0;
   
   if (numberOfDataElements > 0) {
      const float numerator = sumOfCubes / static_cast<double>(numberOfDataElements);
      const double variance = getVariance();
      const float denominator = std::pow(variance, 1.5);
      if (denominator > 0.0) {
         s = numerator / denominator;
      }
   }

   return s;
}

/**
 * get the kurtosis.
 * http://www.statsdirect.com/help/basic_descriptive_statistics/desc.htm
 */
float 
StatisticDescriptiveStatistics::getKurtosis() const
{
   float k = 0;
   
   if (numberOfDataElements > 0) {
      const float numerator = sumOfQuads / static_cast<double>(numberOfDataElements);
      const float variance = getVariance();
      const float denominator = variance * variance;
      if (denominator > 0.0) {
         k = numerator / denominator;
      }
   }

   return k;
}
      
/**
 * get the median value.
 */
float 
StatisticDescriptiveStatistics::getMedian() const
{
   if (numberOfDataElements < 1) {
      return 0.0;
   }
   
   std::vector<float> dataSorted;
   
   for (int i = 0; i < getNumberOfDataGroups(); i++) {
      const StatisticDataGroup* sdg = getDataGroup(i);
      const float* data = sdg->getPointerToData();
      const int numData = sdg->getNumberOfData();
      for (int j = 0; j < numData; j++) {
         dataSorted.push_back(data[j]);
      }
   }
   
   std::sort(dataSorted.begin(), dataSorted.end());
   int middleIndex = numberOfDataElements / 2;
   
   const float median = dataSorted[middleIndex];
   return median;
}
      
/**
 * get the minimum and maximum.
 */
void 
StatisticDescriptiveStatistics::getMinimumAndMaximum(float& minimumOut,
                                                     float& maximumOut) const
{
   if (numberOfDataElements < 1) {
      minimumOut = 0.0;
      maximumOut = 0.0;
   }
   minimumOut =  std::numeric_limits<float>::max();
   maximumOut = -std::numeric_limits<float>::max();
   
   for (int i = 0; i < getNumberOfDataGroups(); i++) {
      const StatisticDataGroup* sdg = getDataGroup(i);
      const float* data = sdg->getPointerToData();
      const int numData = sdg->getNumberOfData();
      for (int j = 0; j < numData; j++) {
         const float d = data[j];
         minimumOut = std::min(minimumOut, d);
         maximumOut = std::max(maximumOut, d);
      }
   }
}      

/**
 * get the sample of population standard deviation (divide by N - 1).
 * http://www.statsdirect.com/help/basic_descriptive_statistics/standard_deviation.htm
 */
float 
StatisticDescriptiveStatistics::getPopulationSampleStandardDeviation() const 
{ 
   const float sd = std::sqrt(getPopulationSampleVariance()); 
   return sd;
}

/**
 * get the sample of population variance (divied by N - 1).
 * http://www.statsdirect.com/help/basic_descriptive_statistics/standard_deviation.htm
 */
float 
StatisticDescriptiveStatistics::getPopulationSampleVariance() const 
{ 
   if (numberOfDataElements <= 1) {
      return 0.0;
   }

   const float pv = sumOfSquares / static_cast<double>(numberOfDataElements - 1);
   return pv; 
}

/**
 * get the standard deviation.
 * http://www.statsdirect.com/help/basic_descriptive_statistics/standard_deviation.htm
 */
float 
StatisticDescriptiveStatistics::getStandardDeviation() const 
{ 
   const float d = std::sqrt(getVariance());
   return d; 
}

/**
 * get the variance.
 * http://www.statsdirect.com/help/basic_descriptive_statistics/standard_deviation.htm
 */
float 
StatisticDescriptiveStatistics::getVariance() const 
{ 
   if (numberOfDataElements <= 1) {
      return 0.0;
   }

   const float v= sumOfSquares / static_cast<double>(numberOfDataElements);
   return v; 
}
      
/**
 * get the standard error (standard deviation of distribution of means).
 * http://www.statsdirect.com/help/basic_descriptive_statistics/standard_deviation.htm
 */
float 
StatisticDescriptiveStatistics::getStandardErrorOfTheMean() const
{
   if (numberOfDataElements <= 1) {
      return 0.0;
   }
   
   const double sn = std::sqrt(static_cast<double>(numberOfDataElements));
   const float se = getPopulationSampleStandardDeviation() / sn;
   return se;
}
      
/**
 * get root mean square.
 * http://mathworld.wolfram.com/Root-Mean-Square.html
 */
float 
StatisticDescriptiveStatistics::getRootMeanSquare() const
{
   if (numberOfDataElements <= 0) {
      return 0.0;
   }
   
   const float rms = std::sqrt(dataSumSquared / static_cast<double>(numberOfDataElements));
   return rms;
}      

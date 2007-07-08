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

/*****************************************************************************
   Major portions of this software are copyrighted by the Medical College
   of Wisconsin, 1994-2000, and are released under the Gnu General Public
   License, Version 2.  See the file README.Copyright for details.
   
   The tStatisticToPValue method and related code is from AFNI
******************************************************************************/
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <set>

#include "DebugControl.h"
#include "StatisticsUtilities.h"
#include "ValueIndexSort.h"

/**
 * Compute statistics for a set of values.
 * If "dataIsSampleFlag" is true, the data is assumed to be a sample where the true
 * mean is unknown and so the deviation is computed by dividing by "N - 1".  If 
 * "dataIsSampleFlag" is false, then the data is assumed to be the total population
 * so that the true mean is known and the deviation is computed by dividing by "N".
 */
void
StatisticsUtilities::computeStatistics(const std::vector<float>& valuesIn,
                                       const bool dataIsSampleFlag,
                                       DescriptiveStatistics& statisticsOut)
{
   statisticsOut.reset();
   
   const int numValues = static_cast<int>(valuesIn.size());
   if (numValues == 0) {
      return;
   }
   
   //
   // Sort input values
   //
   std::vector<float> valuesSorted = valuesIn;
   std::sort(valuesSorted.begin(), valuesSorted.end());
   
   //
   // Compute statistics for all values
   //
   computeStatisticsHelper(valuesSorted, false, dataIsSampleFlag, statisticsOut);
      
   //
   // Throw out top and bottom 2%
   //
   const int percent2  = std::min(static_cast<int>(valuesSorted.size() * 0.02), numValues);
   const int percent98 = std::min(static_cast<int>(valuesSorted.size() * 0.98), numValues);
   for (int i = percent2; i < percent98; i++) {
      statisticsOut.values96Percent.push_back(valuesSorted[i]);
   }
   
   //
   // Compute statistics with the top and bottom 2% excluded
   //
   computeStatisticsHelper(statisticsOut.values96Percent, true,
                           dataIsSampleFlag, statisticsOut);
}

/**
 * compute statistics helper
 */
void
StatisticsUtilities::computeStatisticsHelper(const std::vector<float>& values,
                                       const bool do96Percent,
                                       const bool dataIsSampleFlag,
                                       DescriptiveStatistics& statisticsOut)
{
   const int num = static_cast<int>(values.size());
   if (num == 0) {
      return;
   }
   
   //
   // Compute average and averages of absolute values.
   //
   float smallNumber = 0.0000000001;
   double sum = 0.0;
   double absSum = 0.0;
   double mostPositiveValue  = 0;
   double leastPositiveValue = std::numeric_limits<float>::max();
   double mostNegativeValue  = 0;
   double leastNegativeValue = -std::numeric_limits<float>::max();
   bool hadPositiveValues = false;
   bool hadNegativeValues = false;
   std::vector<float> valuesForAbsMedian;
   for (int i = 0; i < num; i++) {
      float v = values[i];
      sum += v;
      
      if (v > smallNumber) {
         hadPositiveValues = true;
         if (v > mostPositiveValue) {
            mostPositiveValue = v;
         }
         if (v < leastPositiveValue) {
            leastPositiveValue = v;
         }
      }
      else if (v < -smallNumber) {
         hadNegativeValues = true;
         if (v < mostNegativeValue) {
            mostNegativeValue = v;
         }
         if (v > leastNegativeValue) {
            leastNegativeValue = v;
         }
      }
      
      if (v < 0.0) v = -v;
      absSum += v;
      valuesForAbsMedian.push_back(v);
   }   
   const double numValues = static_cast<double>(num);
   const float average = sum / static_cast<double>(numValues);
   const float absAverage = absSum / static_cast<double>(numValues);
   
   
   if (hadPositiveValues == false) {
      mostPositiveValue  = 0.0;
      leastPositiveValue = 0.0;
   }
   if (hadNegativeValues == false) {
      mostNegativeValue  = 0.0;
      leastNegativeValue = 0.0;
   }
   
   //
   // Compute Standard Deviation and standard deviation of absolute values.
   //
   sum = 0.0;
   absSum = 0.0;
   for (int i = 0; i < num; i++) {
      double v = values[i];
      double diff = v - average;
      sum += diff * diff;
      
      if (v < 0.0) v = -v;
      diff = v - absAverage;
      absSum += diff * diff;
   }
   
   double devNumValues = numValues;
   if (dataIsSampleFlag) {
      devNumValues = numValues - 1.0;
   }
   if (devNumValues <= 0.0) {
      devNumValues = 1.0;
   }
   
   const float standardDeviation = std::sqrt(sum / devNumValues);
   const float standardError     = standardDeviation / std::sqrt(numValues);
   const float absStandardDeviation = std::sqrt(absSum / devNumValues);
   const float absStandardError = absStandardDeviation / std::sqrt(numValues);
   
   const float minValue = values[0];
   const float maxValue = values[num-1];
   const float range = maxValue - minValue;
   
   const int medianIndex = num / 2;
   const float median = values[medianIndex];
   const float absMedian = valuesForAbsMedian[medianIndex];

   if (do96Percent) {
      statisticsOut.average96 = average;
      statisticsOut.absAverage96 = absAverage;
      statisticsOut.standardDeviation96 = standardDeviation;
      statisticsOut.standardError96 = standardError;
      statisticsOut.absStandardDeviation96 = absStandardDeviation;
      statisticsOut.absStandardError96 = absStandardError;
      
      statisticsOut.mostPositiveValue96 = mostPositiveValue;
      statisticsOut.leastPositiveValue96 = leastPositiveValue;
      statisticsOut.mostNegativeValue96 = mostNegativeValue;
      statisticsOut.leastNegativeValue96 = leastNegativeValue;
      
      statisticsOut.minValue96 = minValue;
      statisticsOut.maxValue96 = maxValue;
      statisticsOut.range96 = range;
      statisticsOut.median96  = median;
      statisticsOut.absMedian96 = absMedian;
   }
   else {
      statisticsOut.average = average;
      statisticsOut.absAverage = absAverage;
      statisticsOut.standardDeviation = standardDeviation;
      statisticsOut.standardError = standardError;
      statisticsOut.absStandardDeviation = absStandardDeviation;
      statisticsOut.absStandardError = absStandardError;
      
      statisticsOut.mostPositiveValue = mostPositiveValue;
      statisticsOut.leastPositiveValue = leastPositiveValue;
      statisticsOut.mostNegativeValue = mostNegativeValue;
      statisticsOut.leastNegativeValue = leastNegativeValue;
      
      statisticsOut.minValue = minValue;
      statisticsOut.maxValue = maxValue;
      statisticsOut.range = range;
      statisticsOut.median = median;
      statisticsOut.absMedian = absMedian;
   }
}

//****************************************************************************************
//
// DescriptiveStatistics methods
//
//****************************************************************************************

/**
 * Constructor.
 */
StatisticsUtilities::DescriptiveStatistics::DescriptiveStatistics()
{
   reset();
}

/**
 * Destructor.
 */
StatisticsUtilities::DescriptiveStatistics::~DescriptiveStatistics()
{
}

/**
 * reset all members.
 */
void 
StatisticsUtilities::DescriptiveStatistics::reset()
{
   average              = 0.0;
   standardDeviation    = 0.0;
   standardError        = 0.0;
   absAverage           = 0.0;
   absStandardDeviation = 0.0;
   absStandardError     = 0.0;
   minValue             = 0.0;
   maxValue             = 0.0;
   range                = 0.0;   
   mostPositiveValue    = 0.0;
   leastPositiveValue   = 0.0;
   mostNegativeValue    = 0.0;
   leastNegativeValue   = 0.0;
   median               = 0.0;
   absMedian            = 0.0;
   
   average96              = 0.0;
   standardDeviation96    = 0.0;
   standardError96        = 0.0;
   absAverage96           = 0.0;
   absStandardDeviation96 = 0.0;
   absStandardError96     = 0.0;
   minValue96             = 0.0;
   maxValue96             = 0.0;
   mostPositiveValue96    = 0.0;
   leastPositiveValue96   = 0.0;
   mostNegativeValue96    = 0.0;
   leastNegativeValue96   = 0.0;
   range96                = 0.0;   
   median96               = 0.0;
   absMedian96            = 0.0;
   
   values96Percent.clear();
}


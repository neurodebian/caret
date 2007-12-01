
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
#include <iostream>

#include "StatisticDataGroup.h"
#include "StatisticNormalizeDistribution.h"
#include "StatisticValueIndexSort.h"

/**
 * constructor.  This algorithm functions on one and only one StatisticDataGroup.
 *
 * IMPORTANT: The output data replaces the input data so be sure to create
 * the input StatisticDataGroup with DATA_STORAGE_MODE_POINT.
 */
StatisticNormalizeDistribution::StatisticNormalizeDistribution(const float meanIn,
                                                               const float deviationIn)
   : StatisticAlgorithm("Normalize Distribution")
{
   outputDataGroupContainingNormalizeValues = NULL;
   mean = meanIn;
   deviation = deviationIn;
}

/**
 * destructor.
 */
StatisticNormalizeDistribution::~StatisticNormalizeDistribution()
{
   if (outputDataGroupContainingNormalizeValues != NULL) {
      delete outputDataGroupContainingNormalizeValues;
      outputDataGroupContainingNormalizeValues = NULL;
   }
}

/**
 * execute the normalization.
 */
void 
StatisticNormalizeDistribution::execute() throw (StatisticException)
{
   //
   // Verify the input data group
   //
   if (getNumberOfDataGroups() != 1) {
      throw StatisticException("Normalization only allows one data group.");
   }
   StatisticDataGroup* sdg = getDataGroup(0);
   const int numValues = sdg->getNumberOfData();
   if (numValues <= 0) {
      throw StatisticException("Normalization data group has no values");
   }
   
   float* outputNormalizedValues = new float[numValues];
   
   if (numValues == 1) {
      outputNormalizedValues[0] = mean;
   }
   else {
      StatisticValueIndexSort svis;
      svis.addDataGroup(sdg);
      svis.execute();
      
      if (svis.getNumberOfItems() != numValues) {
         throw StatisticException("StatisticValueIndexSort failed (has wrong number of values).");
      }
      
      //
      // Sort the values (need indices for output so data remains in origin "temporal" order)
      //
      std::vector<int> indices(numValues);
      std::vector<float> values(numValues);
      for (int i = 0; i < numValues; i++) {
         svis.getValueAndOriginalIndex(i, indices[i], values[i]);
      }
            
      //
      // Get the index of the median value
      //
      const int medianIndex = numValues / 2;
      
      //
      // Do the half below the median
      //
      normalizeHelper(&values[0],
                            medianIndex,
                            NORMALIZE_METHOD_BELOW_MEAN,
                            mean,
                            deviation);
                            
      //
      // Do the half below the median
      //
      normalizeHelper(&values[medianIndex],
                            (numValues - medianIndex),
                            NORMALIZE_METHOD_ABOVE_MEAN,
                            mean,
                            deviation);
                            
      //
      // Put values back in "temporal" location
      //
      for (int i = 0; i < numValues; i++) {
         outputNormalizedValues[indices[i]] = values[i];
      }
   }

   outputDataGroupContainingNormalizeValues = new StatisticDataGroup(outputNormalizedValues,
                                                                     numValues,
                                            StatisticDataGroup::DATA_STORAGE_MODE_TAKE_OWNERSHIP);
}

/**
 * normalize so median is at mean.
 */
void 
StatisticNormalizeDistribution::normalizeHelper(float* valuesInOut,
                                                const int numValues,
                                                const NORMALIZE_METHOD normMethod,
                                                const float mean,
                                                const float deviation) throw (StatisticException)
{
   if (numValues <= 0) {
      return;
   }
   if (numValues == 1) {
      valuesInOut[0] = mean;
      return;
   }
   
   //
   // Sort the values (need indices for output so data remains in origin "temporal" order)
   //
   StatisticDataGroup sdg(valuesInOut, numValues, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticValueIndexSort svis;
   svis.addDataGroup(&sdg);
   svis.execute();
   if (svis.getNumberOfItems() != numValues) {
      throw StatisticException("StatisticValueIndexSort failed (has wrong number of values).");
   }
   
   std::vector<int> indices(numValues);
   std::vector<float> values(numValues);
   for (int i = 0; i < numValues; i++) {
      svis.getValueAndOriginalIndex(i, indices[i], values[i]);
   }
   
   //
   // number of values remapped 
   //
   int numRemapped = 0;
   
   const double oneOverSigmaSqrt2Pi = 1.0 / (deviation * std::sqrt(2.0 * M_PI));
   
   const double deltaX = 0.1;
   double cumulativeAreaUnderCurve = 0.0;
   const double tailSize = deviation * 10.0;
   double maxX = mean + tailSize;
   double minX = mean - tailSize;
   double totalArea = 1.0;
   
   //
   // Limit depending upon part of curve that is being used
   //
   switch (normMethod) {
      case NORMALIZE_METHOD_BELOW_MEAN:
         totalArea = 0.5;
         maxX = mean;
         break;
      case NORMALIZE_METHOD_ABOVE_MEAN:
         totalArea = 0.5;
         minX = mean;
         break;
      case NORMALIZE_METHOD_ALL:
         break;
   }
   
   float prevVal = 0.0;
   bool prevValValid = false;
   
   //
   // For values that do not get mapped due to numerical error
   //
   double lastMappedValueX = minX;
   double prevAreaUnderCurve = 0.0;
   
   for (double x = minX; x <= maxX; x += deltaX) {
      //
      // Determine the gaussian value for this scalar
      //
      const double numeratorE = (x - mean) * (x - mean);
      const double denominatorE = 2.0 * deviation * deviation;
      const double t = std::exp(-(numeratorE / denominatorE));
      const double val = t * oneOverSigmaSqrt2Pi;
      
      if (prevValValid) {
         //
         // Determine the area under the curve between previous and current scalar
         //
         const double areaUnderCurveDelta = (((val + prevVal) * 0.5) * deltaX);
         cumulativeAreaUnderCurve += areaUnderCurveDelta;
         if (getDebugOn()) {
            std::cout << "f(" << x << ") = " << val
                      << "    F(" << x << ") = " << cumulativeAreaUnderCurve << std::endl;
         }
                      
         //
         // Percentage under curve
         //
         const double percentUnderCurve = (areaUnderCurveDelta + prevAreaUnderCurve) / totalArea;
         
         //
         // number of values for this part under the curve
         //
         int numValuesUnderCurve = static_cast<int>(percentUnderCurve * numValues);
         
         //
         // Remap the nodes
         //
         if (numValuesUnderCurve > 0) {
            //
            // Limit so that total number of nodes is not exceeded
            //
            if ((numValuesUnderCurve + numRemapped) > numValues) {
               numValuesUnderCurve = numValues - numRemapped;
            }
            
            //
            // Remap the values
            //
            //rescaleSortedValues(&values[numRemapped], numValuesUnderCurve, x - deltaX, x);
            rescaleSortedValues(&values[numRemapped], numValuesUnderCurve, lastMappedValueX, x);
            
            //
            // Update number of nodes remapped
            //
            numRemapped += numValuesUnderCurve;
            if (numRemapped >= numValues) {
               break;
            }
            
            //
            // Save last mapped values
            //
            lastMappedValueX = x;
            
            //
            // start new accumulation of area
            //
            prevAreaUnderCurve = 0.0;
         }
         else {
            //
            // Accumulate area under curve needed if number of values is small
            //
            prevAreaUnderCurve += areaUnderCurveDelta;
         }
      }
      
      prevVal = val;
      prevValValid = true;
   }
   
   //
   // Process any remaining values
   //
   if (numRemapped < numValues) {
      rescaleSortedValues(&values[numRemapped], (numValues - numRemapped), lastMappedValueX, maxX);
      //for (int i = numRemapped; i < numValues; i++) {
      //   values[i] = (lastMappedValueX + maxX) / 2.0;
      //}
   }
   
   //
   // Put values back in "temporal" location
   //
   for (int i = 0; i < numValues; i++) {
      valuesInOut[indices[i]] = values[i];
   }
   
   if (getDebugOn()) {
      std::cout << "Area under curve: " << cumulativeAreaUnderCurve << std::endl;
   }
}

/**
 * Rescale a sorted set of values to a new range.
 */
void
StatisticNormalizeDistribution::rescaleSortedValues(float* values,
                                         const int numValues,
                                         const float outputMinimum,
                                         const float outputMaximum)
{
   if (numValues <= 0) {
      return;
   }
      
   const float inputMinimum = values[0];
   const float inputMaximum = values[numValues - 1];
   
   float inputDiff  = inputMaximum - inputMinimum;
   if (inputDiff == 0.0) {
      for (int i = 0; i < numValues; i++) {
         values[i] = (outputMinimum + outputMaximum) / 2.0;
      }
      return;
   }

   const float outputDiff = outputMaximum - outputMinimum;

   for (int i = 0; i < numValues; i++) {
      float f = values[i];
      const float normalized = (f - inputMinimum) / inputDiff;
      f = normalized * outputDiff + outputMinimum;
      values[i] = f;
   }
}  


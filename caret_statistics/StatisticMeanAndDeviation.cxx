
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
#include "StatisticMeanAndDeviation.h"

/**
 * constructor.
 */
StatisticMeanAndDeviation::StatisticMeanAndDeviation()
   : StatisticAlgorithm("Mean and Deviation")
{
   mean = 0.0;
   populationSampleDeviation = 0.0;
   populationSampleVariance  = 0.0;
   deviation = 0.0;
   variance  = 0.0;
   sumOfSquares = 0.0;
}

/**
 * destructor.
 */
StatisticMeanAndDeviation::~StatisticMeanAndDeviation()
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
StatisticMeanAndDeviation::execute() throw (StatisticException)
{
   mean = 0.0;
   populationSampleDeviation = 0.0;
   populationSampleVariance  = 0.0;
   deviation = 0.0;
   variance  = 0.0;
   sumOfSquares = 0.0;
   int totalNumberOfData = 0;
   
   //
   // Determine mean
   //
   double meanSum = 0.0;
   for (int i = 0; i < getNumberOfDataGroups(); i++) {
      const StatisticDataGroup* sdg = getDataGroup(i);
      const float* data = sdg->getPointerToData();
      const int numData = sdg->getNumberOfData();
      for (int j = 0; j < numData; j++) {
         meanSum += data[j];
         totalNumberOfData++;
      }
   }
   mean = meanSum / static_cast<float>(totalNumberOfData);
   
   //
   // Determine deviation
   //
   for (int i = 0; i < getNumberOfDataGroups(); i++) {
      const StatisticDataGroup* sdg = getDataGroup(i);
      const float* data = sdg->getPointerToData();
      const int numData = sdg->getNumberOfData();
      for (int j = 0; j < numData; j++) {
         const double diff = data[j] - mean;
         sumOfSquares += (diff * diff);
      }
   }   
   if (totalNumberOfData > 1) {
      variance  = sumOfSquares / static_cast<double>(totalNumberOfData);
      deviation = std::sqrt(variance);
      
      populationSampleVariance  = sumOfSquares / static_cast<double>(totalNumberOfData - 1);
      populationSampleDeviation = std::sqrt(populationSampleVariance);
   }
}


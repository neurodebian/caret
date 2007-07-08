
#ifndef __STATISTIC_DESCRIPTIVE_STATISTICS_H__
#define __STATISTIC_DESCRIPTIVE_STATISTICS_H__

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

/// class for descriptive statistics calculations
class StatisticDescriptiveStatistics : public StatisticAlgorithm {
   public:
      // constructor
      StatisticDescriptiveStatistics();
      
      // destructor
      ~StatisticDescriptiveStatistics();
      
      // generate the mean and deviation
      void execute() throw (StatisticException);
      
      /// get the mean
      float getMean() const { return mean; }
      
      // get the sample of population standard deviation (divide by N - 1)
      float getPopulationSampleStandardDeviation() const;
      
      // get the sample of population variance (divide by N - 1)
      float getPopulationSampleVariance() const;
      
      // get the standard deviation
      float getStandardDeviation() const;
      
      // get the variance
      float getVariance() const;
      
      /// get the sum of squares
      double getSumOfSquares() const { return sumOfSquares; }
      
      // get root mean square
      float getRootMeanSquare() const;
      
      // get the standard error of the mean (standard deviation of distribution of means)
      float getStandardErrorOfTheMean() const;
      
      // get the minimum and maximum
      void getMinimumAndMaximum(float& minimumOut,
                                float& maximumOut) const;
      
      // get the median value
      float getMedian() const;
      
      // get the skewness
      float getSkewness() const;
      
      // get the kurtosis
      float getKurtosis() const;
      
   protected:
      /// the mean of the data
      float mean;
      
      /// number of data elements
      int numberOfDataElements;
      
      /// sum of data squared
      double dataSumSquared;
      
      /// sum of squares  (sum of (xi - mean)^2)
      double sumOfSquares;
      
      /// sum of cubes    (sum of (xi - mean)^3)
      double sumOfCubes;
      
      /// sum of quads  (sum of (xi - mean)^4)
      double sumOfQuads;
};

#endif // __STATISTIC_DESCRIPTIVE_STATISTICS_H__

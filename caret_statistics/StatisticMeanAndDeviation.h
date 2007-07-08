
#ifndef __STATISTIC_MEAN_AND_DEVIATION_H__
#define __STATISTIC_MEAN_AND_DEVIATION_H__

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

/// class for computation of mean and deviations
/// Use sample deviation if data is a sample from a population
/// Use deviation if data is the ENTIRE population
class StatisticMeanAndDeviation : public StatisticAlgorithm {
   public:
      // constructor
      StatisticMeanAndDeviation();
      
      // destructor
      ~StatisticMeanAndDeviation();
      
      // generate the mean and deviation
      void execute() throw (StatisticException);
      
      /// get the mean
      float getMean() const { return mean; }
      
      /// get the sample of population standard deviation (divide by N - 1)
      float getPopulationSampleStandardDeviation() const { return populationSampleDeviation; }
      
      /// get the sample of population variance (divied by N - 1)
      float getPopulationSampleVariance() const { return populationSampleVariance; }
      
      /// get the standard deviation
      float getStandardDeviation() const { return deviation; }
      
      /// get the variance
      float getVariance() const { return variance; }
      
      /// get the sum of squares
      double getSumOfSquares() const { return sumOfSquares; }
      
   protected:
      // the mean of the data
      float mean;
      
      // the sample from population deviation of the data (divide by N - 1)
      float populationSampleDeviation;
      
      // the sample from population variance of the data (divide by N - 1)
      float populationSampleVariance;
      
      // the population deviation of the data (divide by N)
      float deviation;
      
      // the population variance of the data (divide by N)
      float variance;
      
      // sum of squares
      double sumOfSquares;
};

#endif // __STATISTIC_MEAN_AND_DEVIATION_H__

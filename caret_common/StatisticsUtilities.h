
#ifndef __STATISTICS_UTILITIES_H__
#define __STATISTICS_UTILITIES_H__

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

#include <vector>

/// This class contains static methods for performing statisical computations
class StatisticsUtilities {
   public:
      /// class for holding statistics results
      class DescriptiveStatistics {
         public:
            /// Constructor
            DescriptiveStatistics();
            
            /// Destructor
            ~DescriptiveStatistics();
            
            /// reset all members
            void reset();
            
            /// average of values
            float average;
            
            /// standard deviation of values
            float standardDeviation;
            
            /// standard error (dev / sqrt(num - 1))
            float standardError;
            
            /// most positive value
            float mostPositiveValue;
            
            /// least positive value
            float leastPositiveValue;
            
            /// most negative value
            float mostNegativeValue;
            
            /// least negative value
            float leastNegativeValue;
            
            /// average of absolute values
            float absAverage;
            
            /// standard deviation of absolute values
            float absStandardDeviation;
            
            /// standard error (dev / sqrt(num - 1)) of absolute values
            float absStandardError;
            
            /// minimum value
            float minValue;
            
            /// maximum value
            float maxValue;
            
            /// range of values
            float range;
            
            /// median value
            float median;
            
            /// median of absolute values
            float absMedian;
            
            /// average of values with top and bottom 2% excluded
            float average96;
            
            /// standard deviation of values with top and bottom 2% excluded
            float standardDeviation96;
            
            /// standard error (dev / sqrt(num - 1)) with top and bottom 2% excluded
            float standardError96;
            
            /// most positive value
            float mostPositiveValue96;
            
            /// least positive value
            float leastPositiveValue96;
            
            /// most negative value
            float mostNegativeValue96;
            
            /// least negative value
            float leastNegativeValue96;
            
            /// average of absolute values with top and bottom 2% excluded
            float absAverage96;
            
            /// standard deviation of absolute values with top and bottom 2% excluded
            float absStandardDeviation96;
            
            /// standard error (dev / sqrt(num - 1)) of absolute valueswith top and bottom 2% excluded
            float absStandardError96;
            
            /// minimum value with top and bottom 2% excluded
            float minValue96;
            
            /// maximum value with top and bottom 2% excluded
            float maxValue96;
            
            /// range of values with top and bottom 2% excluded
            float range96;
            
            /// median value with top and bottom 2% excluded
            float median96;            
            
            /// median of absolute values with top and bottom 2% excluded
            float absMedian96;            

            /// the "96%" values (top and bottom 2% excluded)
            std::vector<float> values96Percent;
      };
      
      // determine statistics for a set of values
      static void computeStatistics(const std::vector<float>& valuesIn,
                                    const bool dataIsSampleFlag,
                                    DescriptiveStatistics& statisticsOut);
      
      
   protected:      
      // compute statistics helper
      static void computeStatisticsHelper(const std::vector<float>& valuesIn,
                                          const bool do96Percent,
                                          const bool dataIsSampleFlag,
                                          DescriptiveStatistics& statisticsOut);
                                          
};

#endif // __STATISTICS_UTILITIES_H__



#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

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

#include <ostream>

#include "StatisticAlgorithm.h"

/// class for creating a histogram from a set of values
class StatisticHistogram : public StatisticAlgorithm {
   public:
      // constructor
      StatisticHistogram(const int numberOfBucketsIn = 256,
                         const float excludeLeftPercent = 0.0,
                         const float excludeRightPercent = 0.0);
                
      // destructor
      ~StatisticHistogram();
      
      // execute the algorithm
      void execute() throw (StatisticException);
      
      /// get the number of buckets
      int getNumberOfBuckets() const { return buckets.size(); }
      
      // get the data for a bucket
      void getDataForBucket(const int bucketNumber,
                            float& bucketDataValueOut,
                            float& bucketCountOut) const;
      
      /// get the data value for a bucket
      float getDataValueForBucket(const int bucketNumber,
                                  bool* validBucketOut = NULL) const;
      
      /// get statistics on the data
      void getDataStatistics(float& minValue,
                             float& maxValue,
                             float& range,
                             float& mean,
                             float& sampleDeviation) const;

      /// get the estimate of gray and white matter peaks (value of -1 means valid is invalid)
      void getGrayWhitePeakEstimates(int& grayPeakBucketNumber,
                                     int& whitePeakBucketNumber,
                                     int& grayMinimumBucketNumber,
                                     int& whiteMaximumBucketNumber,
                                     int& grayWhiteBoundaryBucketNumber,
                                     int& csfPeakBucketNumber) const;
       
      /// smooth the histogram (MUST be called after execute)
      void smoothHistogram(const float strengthZeroToOne = 0.5,
                           const int iterations = 5,
                           const int neighborDepth = 2) throw (StatisticException);
       
      /// print the histogram peaks
      void printHistogramPeaks(std::ostream& stream) const;
      
   protected:
      /// used to sort items for the histogram
      class HistoPts {
         public:
            HistoPts(const int xin, const int yin) { x = xin; y = yin; }
            int x, y;
            bool operator<(const HistoPts& p) const { return (x < p.x); }
      };

      /// get the largest bucket that is near the specified bucket
      int getLargestBucketNearby(const int bucketIndex,
                                 const int numberOfNeighbors) const;
                       
      /// distance of a point to an infinite line in 3D
      static float distancePointToLine3D(const float pt[3], const float v1[3], const float v2[3]);
  
      /// subtract vectors (3d)
      static void subtractVectors(const float v1[3], const float v2[3], float result[3]);

      /// cross product
      static void crossProduct(const float v1[3],
                               const float v2[3],
                               float crossedVector[3]);

      /// get length of a vector
      static float vectorLength(const float v[3]);

      /// the buckets of the histogram
      std::vector<int> buckets;
      
      /// width of the buckets
      float bucketWidth;
      
      /// the minimum value of the data
      float dataMinimumValue;
      
      /// the maximum value of the data
      float dataMaximumValue;
      
      /// data mean
      float dataMean;
      
      /// data sample deviation
      float dataSampleDeviation;
      
      /// number of buckets
      int numberOfBuckets;
      
      /// percentage to exclude low
      float excludeLeftPercent;
      
      /// percentage to exclude high
      float excludeRightPercent;
      
};

#endif // __HISTOGRAM_H__


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
#include <set>

#include "StatisticDataGroup.h"
#include "StatisticHistogram.h"
#include "StatisticMeanAndDeviation.h"

/**
 * constructor.  Exclude percentages should range 0 to 100.
 * 
 * Note: Input data is not modified.
 */
StatisticHistogram::StatisticHistogram(const int numberOfBucketsIn,
                     const float excludeLeftPercentIn,
                     const float excludeRightPercentIn)
{
   numberOfBuckets = std::max(numberOfBucketsIn, 1);
   excludeLeftPercent = excludeLeftPercentIn;
   excludeRightPercent = excludeRightPercentIn;
}

/**
 * execute the algorithm.
 */
void 
StatisticHistogram::execute() throw (StatisticException)
{
   //
   // Get all the data values and have them sorted (2nd arg == true)
   //
   std::vector<float> values;
   getAllDataValues(values, true);
   if (values.empty()) {
      throw StatisticException("No data supplied for histogram computation");
   }
   
   if (values.size() == 1) {
      buckets.resize(numberOfBuckets, 0);
      buckets[numberOfBuckets / 2] = 1;
      dataMinimumValue = values[0];
      dataMaximumValue = values[0];
      dataMean = values[0];
      dataSampleDeviation = 0.0;
      return;
   }
   
   //
   // Exclude data on left
   //
   int startIndex = 0;
   if (excludeLeftPercent > 0.0) {
      if (excludeLeftPercent >= 100.0) {
         std::cout << "ERROR: Left exclusion percentage for histgram is >= 100.0" << std::endl;
         return;
      }
      else {
         startIndex =
            static_cast<int>((excludeLeftPercent / 100.0) * values.size());
      }
   }
   
   //
   // Exclude data on right
   //
   int endIndex = static_cast<int>(values.size());
   if (excludeRightPercent > 0.0) {
      if (excludeRightPercent >= 100.0) {
         std::cout << "ERROR: Right exclusion percentage for histgram is >= 100.0" << std::endl;
         return;
      }
      else {
         const int numOnRight =
            static_cast<int>((excludeRightPercent / 100.0) * values.size());
         endIndex -= numOnRight;
      }
   }
   
   
   if (endIndex <= startIndex) {
      std::cout << "ERROR: Exclusions for histogram are too large." << std::endl;
      return;
   }
   
   //
   // Determine min and max buckets values
   //
   dataMinimumValue = values[startIndex];
   dataMaximumValue = values[endIndex - 1];
   
   //
   // Determine stats on data
   //
   StatisticMeanAndDeviation meanAndDev;
   StatisticDataGroup sdg(&values[startIndex], 
                          (endIndex - startIndex),
                          StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   meanAndDev.addDataGroup(&sdg);
   try {
      meanAndDev.execute();
   }
   catch (StatisticException&) {
   }
   dataMean = meanAndDev.getMean();
   dataSampleDeviation = meanAndDev.getPopulationSampleStandardDeviation();

   //
   // Determine width of bucket
   //
   bucketWidth = (dataMaximumValue - dataMinimumValue) 
                  / static_cast<float>(numberOfBuckets);
   
   //
   // Create the histogram
   //
   buckets.resize(numberOfBuckets, 0);
   for (int i = startIndex; i < endIndex; i++) {
      int index = static_cast<int>((values[i] - dataMinimumValue) / bucketWidth);
      index = std::max(index, 0);
      index = std::min(index, numberOfBuckets - 1);
      buckets[index]++;
   }
}
          
/**
 * destructor.
 */
StatisticHistogram::~StatisticHistogram()
{
}

/**
 * get the data value for a bucket.
 */
float 
StatisticHistogram::getDataValueForBucket(const int bucketNumber,
                                          bool* validBucketOut) const
{
   if (validBucketOut != NULL) {
      *validBucketOut = false;
   }
   if ((bucketNumber >= 0) && (bucketNumber < getNumberOfBuckets())) {
      const float value = dataMinimumValue + bucketWidth * bucketNumber;
      if (validBucketOut != NULL) {
         *validBucketOut = true;
      }
      return value;
   }
   
   return -1.0;
}
      
/**
 * get the data for a bucket.
 */
void 
StatisticHistogram::getDataForBucket(const int bucketNumber,
                            float& bucketDataValueOut,
                            float& bucketCountOut) const
{
   bucketDataValueOut = 0.0;
   bucketCountOut     = 0.0;
   if ((bucketNumber >= 0) && (bucketNumber < getNumberOfBuckets())) {
      bucketDataValueOut = dataMinimumValue + bucketWidth * bucketNumber;
      bucketCountOut     = buckets[bucketNumber];
   }
}

/**
 * smooth the histogram (MUST be called after execute).
 */
void 
StatisticHistogram::smoothHistogram(const float strengthZeroToOne,
                                    const int iterations,
                                    const int neighborDepth) throw (StatisticException)
{
   if ((strengthZeroToOne < 0.0) ||
       (strengthZeroToOne > 1.0)) {
      throw StatisticException("Strength outside range [0.0, 1.0]");
   }
   if (neighborDepth < 1) {
      throw StatisticException("Neighbor depth must be 1 or greater.");
   }
   if (iterations < 1) {
      throw StatisticException("Number of iterations must be 1 or greater");
   }
   
   const int numBuckets = getNumberOfBuckets();
   if (numBuckets <= 0) {
      return;
   }
   
   //
   // Do as float because of rounding errors
   //
   std::vector<float> bucketsFloat(numBuckets);
   for (int m = 0; m < numBuckets; m++) {
      bucketsFloat[m] = buckets[m];
   }
   
   const float oneMinusStrength = 1.0 - strengthZeroToOne;
   
   //
   // Loop for the specified number of iterations
   //
   for (int iter = 0; iter < iterations; iter++) {
      //
      // Go through all buckets
      //
      for (int i = 0; i < numBuckets; i++) {
         //
         // Determine weight of neighbors
         //
         float neighborCounter = 0.0;
         float neighborSum = 0.0;
         const int iStart = std::max((i - neighborDepth), 0);
         const int iEnd   = std::min((i + neighborDepth), (numBuckets - 1));
         for (int j = iStart; j <= iEnd; j++) {
            if (j != i) {
               neighborSum += bucketsFloat[j];
               neighborCounter += 1.0;
            }
         }
         
         //
         // Smooth bucket "i"
         //
         if (neighborCounter >= 1.0) {
            const float neighborAverage = neighborSum / neighborCounter;
            const float myValue = bucketsFloat[i];
            bucketsFloat[i] = static_cast<int>((myValue * oneMinusStrength)
                                          + (neighborAverage * strengthZeroToOne));
         }
      }
   }
   
   //
   // Put back to int and round by adding (0.5)
   //
   const int bucketNumber = -1;
   for (int m = 0; m < numBuckets; m++) {
      if (m == bucketNumber) {
         std::cout << "Bucket[" << bucketNumber << "] = " 
                   << buckets[bucketNumber] << "  ";
      }
      buckets[m] = static_cast<int>(bucketsFloat[m] + 0.5);
      if (m == bucketNumber) {
         std::cout << buckets[bucketNumber] << std::endl;
      }
   }
   
}

/**
 * get the largest bucket that is near the specified bucket.
 */
int
StatisticHistogram::getLargestBucketNearby(const int bucketIndex,
                                           const int numberOfNeighbors) const 
{
   const int numberOfBuckets = static_cast<int>(buckets.size());
   const int startIndex = std::max(bucketIndex - numberOfNeighbors, 0);
   const int endIndex   = std::min(bucketIndex + numberOfNeighbors + 1, numberOfBuckets);
   
   int biggestBucketIndex = -1;
   int biggestBucketValue = -1;
   
   for (int i = startIndex; i < endIndex; i++) {
      if (buckets[i] > biggestBucketValue) {
         biggestBucketValue = buckets[i];
         biggestBucketIndex = i;
      }
   }
   
   return biggestBucketIndex;
}

/**
 * get the estimate of gray and white matter peaks (value of -1 means value is invalid).
 *
 * grayMinimumBucketNumber is the estimate of the minimum gray value
 * grayPeakBucketNumber is the estimate of the gray matter peak
 * grayWhiteBoundaryBucketNumber is the estimate of the boundary between gray and white values
 * whitePeakBucketNumber is the estimate of the white matter peak
 * whiteMaximumBucketNumber  is the estimate of the maximum white matter value
 *
 * Be sure to call getDatForBucket() with the above parameters to get
 * the actual data values.
 */
void 
StatisticHistogram::getGrayWhitePeakEstimates(int& grayPeakBucketNumber,
                                              int& whitePeakBucketNumber,
                                              int& grayMinimumBucketNumber,
                                              int& whiteMaximumBucketNumber,
                                              int& grayWhiteBoundaryBucketNumber,
                                              int& csfPeakBucketNumber) const
{
   //
   // Reset buckets
   // 
   grayPeakBucketNumber     = -1;
   whitePeakBucketNumber    = -1;
   grayMinimumBucketNumber  = -1;
   whiteMaximumBucketNumber = -1;
   grayWhiteBoundaryBucketNumber = -1;
   csfPeakBucketNumber = -1;
   
   //
   // Copy the histogram since it will be smoothed in order to find peaks
   //
   StatisticHistogram smoothedHistogram = *this;
   
   //
   // Start and ending buckets    
   //
   const int numBuckets = smoothedHistogram.getNumberOfBuckets();
   const int ninetyPercentBucket = static_cast<int>(numBuckets * 0.90);
   const int startBucket = 0; //25;
   const int endBucket = numBuckets - 1;
   if (endBucket <= startBucket) {
      return;
   }
   
   //
   // Storage for finding peaks and valleys
   //
   std::vector<HistoPts> histVector;
   std::vector<HistoPts> histPeaks, histValleys;

   //
   // Loop for smoothing histogram
   //
   bool continueSmoothing = true;
   int totalSmoothingIterations = 0;
   while(continueSmoothing) {
      totalSmoothingIterations++;

      //
      // Smooth the histogram
      //
      const float smoothingStrength = 0.5;
      const int smoothingIterations = 1;
      const int smoothingNeighborDepth = 3;
      smoothedHistogram.smoothHistogram(smoothingStrength, 
                                        smoothingIterations, 
                                        smoothingNeighborDepth);

      //
      // Store line segment points in a vector that is sorted by X-coordinate
      //
      std::set<HistoPts> pts;
      pts.insert(HistoPts(startBucket, 0));  //buckets[startBucket]));
      pts.insert(HistoPts(endBucket, 0));  // buckets[endBucket]));

      //
      // Use a "Douglas Puecker" curve simplification to find the peaks and valleys
      // in the histogram by approximating the curve with a set of line segments
      // It starts with just the endpoints of the curve and finds the point in the
      // curve that is furthest from the line connecting the two endpoints.  This
      // process iterates by finding the point that is furthes from its line segment
      // and inserting it.
      //
      const unsigned int maxSegments = 20;
      while (pts.size() < maxSegments) {
         std::vector<HistoPts> ptsVector(pts.begin(), pts.end());
         const int numSegments = static_cast<int>(ptsVector.size() - 1);

         int furthestX = -1;
         float furthestDist = -1.0;
               
         //
         // Examine each line segment
         //
         for (int m = 0; m < numSegments; m++) {
            const int xStart = static_cast<int>(ptsVector[m].x);
            const int yStart = static_cast<int>(ptsVector[m].y);
            const int xEnd = static_cast<int>(ptsVector[m+1].x);
            const int yEnd = static_cast<int>(ptsVector[m+1].y);
            
            const float p1[3] = { xStart, yStart, 0.0 };
            const float p2[3] = { xEnd, yEnd, 0.0 };
            
            //
            // Test all points to find distance to line segment
            //
            for (int i = xStart + 1; i < xEnd - 1; i++) {
               const float pt[3] = { i, smoothedHistogram.buckets[i], 0.0 };
               const float dist = distancePointToLine3D(pt, p1, p2);
               if (dist > furthestDist) {
                  furthestX = i;
                  furthestDist = dist;
               }
            }
         }
         
         if (furthestX >= 0) {
            pts.insert(HistoPts(furthestX, smoothedHistogram.buckets[furthestX]));
         }
         else {
            break;
         }
      }
      
      //
      // Place the points of the line segment into a vector
      //
      histVector.clear();
      histVector.insert(histVector.end(),
                        pts.begin(), pts.end());
      histPeaks.clear();
      histValleys.clear();
      
      if (getDebugOn()) {
         std::cout << "Peak finding:" << std::endl;
      }
      const int lastIndex = histVector.size() - 1;
      for (int i = 0; i <= lastIndex; i++) {
         const HistoPts& pt = histVector[i];
         if (getDebugOn()) {
            std::cout << "   " << pt.x << ", " << pt.y << std::endl;
         }
         
         //
         // Ignore endpoints
         //
         if ((i > 0) && (i < lastIndex)) {
            //
            // Is this a peak (peak is greater than its left and right neighbors)?
            //
            if ((pt.y > histVector[i-1].y) &&
                (pt.y > histVector[i+1].y)) {
               histPeaks.push_back(pt);
            }
            
            //
            // Is this a valley (peak is less than its left and right neighbors)?
            //
            if ((pt.y < histVector[i-1].y) &&
                (pt.y < histVector[i+1].y)) {
               histValleys.push_back(pt);
            }
         }
      }
      
      const int peakCount = histPeaks.size();
      if (getDebugOn()) {
         std::cout << "Peaks: " << std::endl;
         for (int i = 0; i < peakCount; i++) {
            const HistoPts& pt = histPeaks[i];
            std::cout << "   " << pt.x << ", " << pt.y << std::endl;
         }
         std::cout << std::endl;
         std::cout << "Valleys: " << std::endl;
         for (unsigned int i = 0; i < histValleys.size(); i++) {
            const HistoPts& pt = histValleys[i];
            std::cout << "   " << pt.x << ", " << pt.y << std::endl;
         }
      }
      
      //
      // Have the necessary peaks been found?
      //
      if (peakCount <= 3) {
         //
         // The peaks should be CSF, Gray, White
         //
         continueSmoothing = false;
      }
      else if (peakCount >= 4) {
         bool allPeaks90Plus = true;
         for (int i = 3; i < peakCount; i++) {
            if (histPeaks[i].x < ninetyPercentBucket) {
               allPeaks90Plus = false;
            }
         }
         if (allPeaks90Plus) {
            //
            // Fourth and greater buckets is/are skull, blood, etc
            //
            continueSmoothing = false;
         }
      }
   }
   
   if (getDebugOn()) {
      std::cout << totalSmoothingIterations << " iterations of smoothing needed to find peaks." << std::endl;
   }
   
   //
   // The first peak is the CSF
   // The second peak is the Gray Matter
   // The third peak is the White Matter
   // The fourth peak is Skull or Arteries
   //
   if (histPeaks.size() > 0) {
      csfPeakBucketNumber = histPeaks[0].x;
      if (histPeaks.size() > 1) {
         grayPeakBucketNumber  = histPeaks[0].x;
         whitePeakBucketNumber = histPeaks[1].x;
         if (histPeaks.size() > 2) {
            grayPeakBucketNumber  = histPeaks[1].x;
            whitePeakBucketNumber = histPeaks[2].x;
         }
      }
   }
   
   //
   // First valley BEFORE gray peaks is gray minimum
   // First valley AFTER white peak is white maximum
   //
   for (unsigned int i = 0; i < histValleys.size(); i++) {
      const HistoPts& pt = histValleys[i];
      if (grayPeakBucketNumber >= 0) {
         if (pt.x < grayPeakBucketNumber) {
            grayMinimumBucketNumber = pt.x;
         }
      }
      //if (whitePeakBucketNumber >= 0) {
      //   if (pt.x > whitePeakBucketNumber) {
      //      if (whiteMaximumBucketNumber < 0) {
      //         whiteMaximumBucketNumber = pt.x;
      //      }
      //   }
      //}
      if ((grayPeakBucketNumber >= 0) &&
          (whitePeakBucketNumber >= 0)) {
         if ((grayPeakBucketNumber < pt.x) &&
             (whitePeakBucketNumber > pt.x)) {
            grayWhiteBoundaryBucketNumber = pt.x;
         }
      }
   }
      
         
   //
   // Since histogram was smoothed, may not be at the peak so use max of it or neighbors
   //
   if (grayPeakBucketNumber > 0) {
      grayPeakBucketNumber = getLargestBucketNearby(grayPeakBucketNumber, 3);
   }
   if (whitePeakBucketNumber > 0) {
      whitePeakBucketNumber = getLargestBucketNearby(whitePeakBucketNumber, 3);
   }
   
   //
   // Estimate white maximum value.  Assume distribution of white voxels is symmetric
   // but push white max a little to the right
   //
   if (whiteMaximumBucketNumber < 0) {
      if ((whitePeakBucketNumber > 0) &&
          (grayWhiteBoundaryBucketNumber > 0) &&
          (grayWhiteBoundaryBucketNumber < whitePeakBucketNumber)) {
         const int diff = static_cast<int>(
                     static_cast<float>(whitePeakBucketNumber - grayWhiteBoundaryBucketNumber)
                     * 1.15);
         whiteMaximumBucketNumber = std::min((whitePeakBucketNumber + diff),
                                             endBucket);
      }
   }
   
   if (getDebugOn()) {
      std::cout << "CSF Peak: " << getDataValueForBucket(csfPeakBucketNumber) << std::endl;
      std::cout << "Gray Minimum: " << getDataValueForBucket(grayMinimumBucketNumber) << std::endl;
      std::cout << "Gray Peak: " << getDataValueForBucket(grayPeakBucketNumber) << std::endl;
      std::cout << "Gray/White Boundary: " << getDataValueForBucket(grayWhiteBoundaryBucketNumber) << std::endl;
      std::cout << "White Peak: " << getDataValueForBucket(whitePeakBucketNumber) << std::endl;
      std::cout << "White Maximum: " << getDataValueForBucket(whiteMaximumBucketNumber) << std::endl;
   }
}
/*
   //
   // Create a smoothed version of this histogram to remove noise
   //
   StatisticHistogram smoothedHistogram = *this;
   smoothedHistogram.smoothHistogram(0.5, 5, 5);
   
   grayPeakBucketNumber     = -1;
   whitePeakBucketNumber    = -1;
   grayMinimumBucketNumber  = -1;
   whiteMaximumBucketNumber = -1;
   grayWhiteBoundaryBucketNumber = -1;
   
   //
   // Start and ending buckets    
   //
   const int numBuckets = smoothedHistogram.getNumberOfBuckets();
   const int startBucket = 0; //25;
   const int endBucket = numBuckets - 1;
   if (endBucket <= startBucket) {
      return;
   }
   
   std::set<HistoPts> pts;
   pts.insert(HistoPts(startBucket, 0));  //buckets[startBucket]));
   pts.insert(HistoPts(endBucket, 0));  // buckets[endBucket]));
   
   //
   // Use a "Douglas Puecker" curve simplification
   //
   const unsigned int maxSegments = 20;
   while (pts.size() < maxSegments) {
      std::vector<HistoPts> ptsVector(pts.begin(), pts.end());
      const int numSegments = static_cast<int>(ptsVector.size() - 1);

      int furthestX = -1;
      float furthestDist = -1.0;
            
      for (int m = 0; m < numSegments; m++) {
         const int xStart = static_cast<int>(ptsVector[m].x);
         const int yStart = static_cast<int>(ptsVector[m].y);
         const int xEnd = static_cast<int>(ptsVector[m+1].x);
         const int yEnd = static_cast<int>(ptsVector[m+1].y);
         
         const float p1[3] = { xStart, yStart, 0.0 };
         const float p2[3] = { xEnd, yEnd, 0.0 };
         for (int i = xStart + 1; i < xEnd - 1; i++) {
            const float pt[3] = { i, smoothedHistogram.buckets[i], 0.0 };
            const float dist = distancePointToLine3D(pt, p1, p2);
            if (dist > furthestDist) {
               furthestX = i;
               furthestDist = dist;
            }
         }
      }
      
      if (furthestX >= 0) {
         pts.insert(HistoPts(furthestX, smoothedHistogram.buckets[furthestX]));
      }
      else {
         break;
      }
   }
   
   std::vector<HistoPts> histVector(pts.begin(), pts.end());
   std::vector<HistoPts> histPeaks, histValleys;
   if (getDebugOn()) {
      std::cout << "Peak finding:" << std::endl;
   }
   const int lastIndex = histVector.size() - 1;
   for (int i = 0; i <= lastIndex; i++) {
      const HistoPts& pt = histVector[i];
      if (getDebugOn()) {
         std::cout << "   " << pt.x << ", " << pt.y << std::endl;
      }
      
      //
      // Ignore endpoints
      //
      if ((i > 0) && (i < lastIndex)) {
         //
         // Is this a peak?
         //
         if ((pt.y > histVector[i-1].y) &&
             (pt.y > histVector[i+1].y)) {
            histPeaks.push_back(pt);
         }
         
         //
         // Is this a valley?
         //
         if ((pt.y < histVector[i-1].y) &&
             (pt.y < histVector[i+1].y)) {
            histValleys.push_back(pt);
         }
      }
   }
   
   if (getDebugOn()) {
      std::cout << "Peaks: " << std::endl;
      for (unsigned int i = 0; i < histPeaks.size(); i++) {
         const HistoPts& pt = histPeaks[i];
         std::cout << "   " << pt.x << ", " << pt.y << std::endl;
      }
      std::cout << std::endl;
      std::cout << "Valleys: " << std::endl;
      for (unsigned int i = 0; i < histValleys.size(); i++) {
         const HistoPts& pt = histValleys[i];
         std::cout << "   " << pt.x << ", " << pt.y << std::endl;
      }
   }
   
   //
   // The first peak is the CSF
   // The second peak is the Gray Matter
   // The third peak is the White Matter
   // The fourth peak is Skull or Arteries
   //
   if (histPeaks.size() > 1) {
      grayPeakBucketNumber  = histPeaks[1].x;
      whitePeakBucketNumber = histPeaks[1].x;
      if (histPeaks.size() > 2) {
         whitePeakBucketNumber = histPeaks[2].x;
      }
   }
   
   //
   // First valley BEFORE gray peaks is gray minimum
   // First valley AFTER white peak is white maximum
   //
   for (unsigned int i = 0; i < histValleys.size(); i++) {
      const HistoPts& pt = histValleys[i];
      if (grayPeakBucketNumber >= 0) {
         if (pt.x < grayPeakBucketNumber) {
            grayMinimumBucketNumber = pt.x;
         }
      }
      if (whitePeakBucketNumber >= 0) {
         if (pt.x > whitePeakBucketNumber) {
            if (whiteMaximumBucketNumber < 0) {
               whiteMaximumBucketNumber = pt.x;
            }
         }
      }
      if ((grayPeakBucketNumber >= 0) &&
          (whitePeakBucketNumber >= 0)) {
         if ((grayPeakBucketNumber < pt.x) &&
             (whitePeakBucketNumber > pt.x)) {
            grayWhiteBoundaryBucketNumber = pt.x;
         }
      }
   }
      
   //
   // If white max not found, estimate at midpoint of white peak and last bucket
   //
   if (whiteMaximumBucketNumber < 0) {
      if (whitePeakBucketNumber > 0) {
         whiteMaximumBucketNumber = (whitePeakBucketNumber + histVector[lastIndex].x) / 2;
         //std::cout << "White max guessed." << std::endl;
      }
   }
   
   if (getDebugOn()) {
      std::cout << "Gray Minimum: " << getDataValueForBucket(grayMinimumBucketNumber) << std::endl;
      std::cout << "Gray Peak: " << getDataValueForBucket(grayPeakBucketNumber) << std::endl;
      std::cout << "Gray/White Boundary: " << getDataValueForBucket(grayWhiteBoundaryBucketNumber) << std::endl;
      std::cout << "White Peak: " << getDataValueForBucket(whitePeakBucketNumber) << std::endl;
      std::cout << "White Maximum: " << getDataValueForBucket(whiteMaximumBucketNumber) << std::endl;
   }
}
*/

/**
 * get statistics on the data.
 */
void 
StatisticHistogram::getDataStatistics(float& minValue,
                             float& maxValue,
                             float& range,
                             float& mean,
                             float& sampleDeviation) const
{
   minValue = dataMinimumValue;
   maxValue = dataMaximumValue;
   range = maxValue - minValue;
   mean = dataMean;
   sampleDeviation = dataSampleDeviation;
}

/**
 * distance of a point to an infinite line in 3D.
 * "pt" is the point.  "v1" and "v2" are points on the line.
 * Formula is from "http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html". */
float
StatisticHistogram::distancePointToLine3D(const float pt[3], const float v1[3], const float v2[3]){
   float dv2v1[3];
   subtractVectors(v2, v1, dv2v1);
   float dv1pt[3];
   subtractVectors(v1, pt, dv1pt);

   float crossed[3];
   crossProduct(dv2v1, dv1pt, crossed);

   float numerator = vectorLength(crossed);
   float denomenator = vectorLength(dv2v1);
   float dist = numerator / denomenator;
   return dist;
}

/**
 * subtract vectors (3d)  result = v1 - v2.
 */
void
StatisticHistogram::subtractVectors(const float v1[3], const float v2[3], float result[3])
{
   result[0] = v1[0] - v2[0];
   result[1] = v1[1] - v2[1];
   result[2] = v1[2] - v2[2];
}

/**
 * cross product.
 */
void
StatisticHistogram::crossProduct(const float v1[3],
                            const float v2[3],
                            float crossedVector[3])
{
  crossedVector[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  crossedVector[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  crossedVector[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

/**
 * get length of a vector.
 */
float
StatisticHistogram::vectorLength(const float v[3])
{
   const float len = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
   return len;
}


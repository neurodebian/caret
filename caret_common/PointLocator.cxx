
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
#include <iostream>
#include <limits>
#include <set>

#include "MathUtilities.h"
#include "PointLocator.h"

/**
 * Constructor.
 */
PointLocator::PointLocator(const float boundsIn[6],
                           const int* numBucketsInEachAxis)
{
   for (int i = 0; i < 6; i++) {
      bounds[i] = boundsIn[i];
   }
   
   numX = 10;
   numY = 10;
   numZ = 10;
   
   if (numBucketsInEachAxis != NULL) {
      numX = numBucketsInEachAxis[0];
      numY = numBucketsInEachAxis[1];
      numZ = numBucketsInEachAxis[2];
   }
   
   totalNumBuckets = numX * numY * numZ;
   buckets = new Bucket[totalNumBuckets];
   
   sizeX = (bounds[1] - bounds[0]) / numX;
   sizeY = (bounds[3] - bounds[2]) / numY;
   sizeZ = (bounds[5] - bounds[4]) / numZ;
   
   for (int i = 0; i < numX; i++) {
      for (int j = 0; j < numY; j++) {
         for (int k = 0; k < numZ; k++) {
            const int ijk[3] = { i, j, k };
            const int bi = getBucketIndex(ijk);
            Bucket& b = buckets[bi];
            b.bounds[0] = i * sizeX + bounds[0];
            b.bounds[1] = bounds[0] + sizeX;
            b.bounds[2] = j * sizeY + bounds[2];
            b.bounds[3] = bounds[2] + sizeY;
            b.bounds[4] = k * sizeZ + bounds[4];
            b.bounds[5] = bounds[4] + sizeZ;
         }
      }
   }
   
   pointCounter = 0;
}

/**
 * Constructor.
 */
PointLocator::~PointLocator()
{
   if (buckets != NULL) {
      delete[] buckets;
   }
}

/**
 * add a point to the locator.
 */
void
PointLocator::addPoint(const float xyz[3], const int idIn)
{
   //
   // Determine which bucket point is within
   //
   int ijk[3];
   if (getBucket(xyz, ijk) == false) {
      std::cout << "PointLocator: point out of bounds" << std::endl;
      return;
   }
   
   //
   // Determine the one-dim bucket index
   //
   const int bucketIndex = getBucketIndex(ijk);
   if ((bucketIndex < 0) || (bucketIndex >= totalNumBuckets)) {
      std::cout << "PointLocator: point out of bounds" << std::endl;
      return;
   }
   
   //
   // Put point into bucket
   //
   int idNum = idIn;
   if (idNum < 0) {
      idNum = pointCounter;
   }
   pointCounter++;
   Point p(xyz, idNum);
   buckets[bucketIndex].points.push_back(p);
}

/**
 * get nearest point (returns -1 if not found)
 */
int 
PointLocator::getNearestPoint(const float xyz[3]) const
{
   //
   // Determine which bucket point is within
   //
   int ijk[3];
   if (getBucket(xyz, ijk) == false) {
      return -1;
   }
   
   //
   // Determine the one-dim bucket index
   //
   const int bucketIndex = getBucketIndex(ijk);
   if ((bucketIndex < 0) || (bucketIndex > totalNumBuckets)) {
      return -1;
   }
   
   //
   // Find nearest point in containing bucket
   //
   float nearbyDistSQ, nearbyXYZ[3];
   int nearbyPt = buckets[bucketIndex].getNearestPoint(xyz, nearbyDistSQ, nearbyXYZ);
   
   //
   // Keep track of buckets already searched
   //
   std::set<int> bucketsAlreadySearched;
   bucketsAlreadySearched.insert(bucketIndex);
   
   //
   // If bucket is empty, search nearby buckets and expand search if necessary
   //
   if (nearbyPt < 0) {
      bool done = false;
      int delta = 1;
      while (done == false) {
         //
         // Expand bucket search
         //
         const int minI = std::max(ijk[0] - delta, 0);
         const int maxI = std::min(ijk[0] + delta, numX - 1);
         const int minJ = std::max(ijk[1] - delta, 0);
         const int maxJ = std::min(ijk[1] + delta, numY - 1);
         const int minK = std::max(ijk[2] - delta, 0);
         const int maxK = std::min(ijk[2] + delta, numZ - 1);
         
         for (int i = minI; (i <= maxI) && (done == false); i++) {
            for (int j = minJ; (j <= maxJ) && (done == false); j++) {
               for (int k = minK; (k <= maxK) && (done == false); k++) {
                  const int ijk2[3] = { i, j, k };
                  const int indx = getBucketIndex(ijk2);
                  if (indx >= 0) {
                     nearbyPt = buckets[indx].getNearestPoint(xyz, nearbyDistSQ, nearbyXYZ);
                     if (nearbyPt >= 0) {
                        done = true;
                     }
                     bucketsAlreadySearched.insert(indx);
                  }
               }  // for k
            }  // for j
         } // for i
         
         //
         // have not found bucket yet ?
         //
         if (done == false) {
            delta++;
            //
            // Have all buckets been searched ?
            //
            if ((minI == 0) && (maxI == (numX - 1)) &&
                (minJ == 0) && (maxJ == (numY - 1)) &&
                (minK == 0) && (maxK == (numZ - 1))) {
               done = true;
            }
         }
      }
   }
   
   //
   // Found point in a bucket
   //
   if (nearbyPt >= 0) {
      const float dist = MathUtilities::distance3D(xyz, nearbyXYZ);
      const float minSearchXYZ[3] = {
         xyz[0] - dist, 
         xyz[1] - dist, 
         xyz[2] - dist
      };
      const float maxSearchXYZ[3] = {
         xyz[0] + dist, 
         xyz[1] + dist, 
         xyz[2] + dist
      };
      
      //
      // Minimum bucket to search
      //
      int minIJK[3];
      getBucket(minSearchXYZ, minIJK);
      limitIJK(minIJK);
      
      //
      // Maximum bucket to search
      //
      int maxIJK[3];
      getBucket(maxSearchXYZ, maxIJK);
      limitIJK(maxIJK);
      
      //
      // Search in the neighboring buckets
      //
      for (int i = minIJK[0]; i <= maxIJK[0]; i++) {
         for (int j = minIJK[1]; j <= maxIJK[1]; j++) {
            for (int k = minIJK[2]; k <= maxIJK[2]; k++) {
               const int ijk2[3] = { i, j, k };
               const int bi = getBucketIndex(ijk2);
               if (std::find(bucketsAlreadySearched.begin(), bucketsAlreadySearched.end(),
                             bi) == bucketsAlreadySearched.end()) {
                  const Bucket& b = buckets[bi];
                  float ptXYZ[3], dist;
                  const int pt = b.getNearestPoint(xyz, dist, ptXYZ);
                  if ((pt >= 0) && (dist < nearbyDistSQ)) {
                     nearbyDistSQ = dist;
                     nearbyPt     = pt;
                  }
               }
            }
         }
      }
      
      return nearbyPt;
   }
   return -1;
}      

/**
 * limit the IJK index to valid buckets.
 */
void 
PointLocator::limitIJK(int ijk[3]) const
{
   ijk[0] = std::max(0, ijk[0]);
   ijk[0] = std::min(numX - 1, ijk[0]);
   ijk[1] = std::max(0, ijk[1]);
   ijk[1] = std::min(numY - 1, ijk[1]);
   ijk[2] = std::max(0, ijk[2]);
   ijk[2] = std::min(numZ - 1, ijk[2]);
}
      
/**
 * get bucket for a point (returns false if not in a bucket - out of bounds).
 */
bool 
PointLocator::getBucket(const float xyz[3], int ijk[3]) const
{
   ijk[0] = static_cast<int>((xyz[0] - bounds[0]) / sizeX);
   ijk[1] = static_cast<int>((xyz[1] - bounds[2]) / sizeY);
   ijk[2] = static_cast<int>((xyz[2] - bounds[4]) / sizeZ);
   
   if ((ijk[0] < 0) || (ijk[0] >= numX) ||
       (ijk[1] < 0) || (ijk[1] >= numY) ||
       (ijk[2] < 0) || (ijk[2] >= numZ)) {
      return false;
   }
   
   
   return true;
}

/**
 * get bucket index.
 */
int 
PointLocator::getBucketIndex(const int ijk[3]) const
{
   const int indx = ijk[0] + ijk[1] * numX + ijk[2] * numX * numY;
   return indx;
}
      
/**
 * get one dimensional bucket index.
 */
int 
PointLocator::getBucketIndex(const float xyz[3]) const
{
   int ijk[3];
   if (getBucket(xyz, ijk) == false) {
      return -1;
   }
   return getBucketIndex(ijk);
}
      
//******************************************************************************

/**
 * Constructor. 
 */
PointLocator::Point::Point(const float xyzIn[3], const int idIn)
{
   xyz[0] = xyzIn[0];
   xyz[1] = xyzIn[1];
   xyz[2] = xyzIn[2];
   idNum  = idIn;
}

/**
 * Destructor
 */
PointLocator::Point::~Point()
{
}            

//******************************************************************************

/**
 * constructor.
 */
PointLocator::Bucket::Bucket()
{
   for (int i = 0; i < 6; i++) {
      bounds[i] = 0.0;
   }
}

/**
 * destructor.
 */
PointLocator::Bucket::~Bucket()
{
}
            
/**
 * get nearest point.
 */
int 
PointLocator::Bucket::getNearestPoint(const float xyz[3], float& nearbyDistSQ,
                                      float nearbyXYZ[3]) const
{
   int   n    = -1;
   float dist = std::numeric_limits<float>::max();
   
   const int num = static_cast<int>(points.size());
   for (int i = 0; i < num; i++) {
      const Point& p = points[i];
      const float dx = p.xyz[0] - xyz[0];
      const float dy = p.xyz[1] - xyz[1];
      const float dz = p.xyz[2] - xyz[2];
      const float d  = dx*dx + dy*dy + dz*dz;
      if (d < dist) {
         dist = d;
         n = i;
      }
   }
   
   if (n >= 0) {
      nearbyXYZ[0] = points[n].xyz[0];
      nearbyXYZ[1] = points[n].xyz[1];
      nearbyXYZ[2] = points[n].xyz[2];
      nearbyDistSQ = dist;
      const int pointID = points[n].idNum;
      return pointID;
   }
   
   return -1;
}
            

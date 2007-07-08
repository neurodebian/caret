
#ifndef __POINT_LOCATOR_H__
#define __POINT_LOCATOR_H__

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

/// This class is a 3D point locator.  Points may be added at any time.
class PointLocator {
   protected:
      /// Information about a point
      class Point {
         public:
            /// Constructor
            Point(const float xyzIn[3], const int idIn = -1);
            
            /// Destructor
            ~Point();
            
            /// point's coordinate
            float xyz[3];
            
            /// point's ID
            int idNum;            
      };
      
   public:
      /// Constructor
      PointLocator(const float boundsIn[6],
                   const int* numBucketsInEachAxis = NULL);
      
      /// Constructor
      ~PointLocator();
      
      /// add a point to the locator
      void addPoint(const float xyz[3], const int idIn);
      
      /// get nearest point
      int getNearestPoint(const float xyz[3]) const;
      
   protected:
      /// Class for storing a bucket
      class Bucket {
         public:
            /// constructor
            Bucket();
            
            /// destructor
            ~Bucket();
            
            /// get nearest point
            int getNearestPoint(const float xyz[3], float& nearbyDistSQ,
                                float nearbyXYZ[3]) const;
            
            /// the bounds of the bucket
            float bounds[6];
            
            /// the points in the bucket
            std::vector<Point> points;
      };
      
      /// get bucket for a point (returns false if not in a bucket - out of bounds)
      bool getBucket(const float xyz[3], int ijk[3]) const;
      
      /// get one dimensional bucket index
      int getBucketIndex(const int ijk[3]) const;
      
      /// get one dimensional bucket index
      int getBucketIndex(const float xyz[3]) const;
      
      /// limit the IJK index to valid buckets
      void limitIJK(int ijk[3]) const;
      
      /// the buckets
      Bucket* buckets;
      
      /// size of buckets in X axis
      float sizeX;
      
      /// size of buckets in X axis
      float sizeY;
      
      /// size of buckets in X axis
      float sizeZ;
      
      /// number of buckets in X axis
      int numX;
      
      /// number of buckets in Y axis
      int numY;
      
      /// number of buckets in Z axis
      int numZ;
      
      /// total number of buckets
      int totalNumBuckets;
      
      /// bounds of the point locator
      float bounds[6];
      
      /// keeps count of points added
      int pointCounter;
};

#endif // __POINT_LOCATOR_H__


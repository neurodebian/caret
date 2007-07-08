

#ifndef __VE_MATH_UTILITIES_H__
#define __VE_MATH_UTILITIES_H__

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

/// This class contains static methods for various mathematical formulas.
class MathUtilities {
   public:
      /// normalize a vector
      static float normalize(float v[3]);
      
      /// normalize a vector
      static double normalize(double v[3]);
      
      /// get length of a vector
      static float vectorLength(const float v[3]);
      
      /// get length of a vector
      static double vectorLength(const double v[3]);
      
      /// see if point is in a polygon
      static int pointInPolygon(const double x[3], const int numPts, const double *pts,
                                const double bounds[6], const double n[3]);
                                
      /// see if point is in a polygon
      static int pointInPolygon(const float x[3], const int numPts, const float *pts,
                                const float bounds[6], const float n[3]);
                                
      /// dot product
      static float dotProduct(const float* p1, const float* p2);
      
      /// dot product
      static double dotProduct(const double* p1, const double* p2);
      
      /// distance^2 between two 3d coordinates
      static float distanceSquared3D(const float* p1, const float* p2);
      
      /// distance^2 between two 3d coordinates
      static double distanceSquared3D(const double* p1, const double* p2);
      
      /// distance between two 3d coordinates
      static float distance3D(const float* p1, const float* p2);
      
      /// distance between two 3d coordinates
      static double distance3D(const double* p1, const double* p2);
      
      /// distance of a point to an infinite line in 3D
      static float distancePointToLine3D(const float pt[3], const float v1[3], const float v2[3]);
      
      /// distance of a point to an infinite line in 3D
      static double distancePointToLine3D(const double pt[3], const double v1[3], const double v2[3]);
      
      /// determine if a value is NaN (A comparison with NaN always fails)
      static bool isNaN(const float f) { return (f != f); }
      
      /// subtract vectors (3d)
      static void subtractVectors(const float v1[3], const float v2[3], float result[3]);
      
      /// subtract vectors (3d)
      static void subtractVectors(const double v1[3], const double v2[3], double result[3]);
      
      /// compute angle (in radians) for point p2 
      static float angle(const float* p1, const float* p2, const float* p3);

      /// signed angle "jik"
      static float signedAngle(const float pi[3], const float pj[3], 
                               const float pk[3], const float n[3]);
      
      /// compute the signed distance of a point to a plane
      static float signedDistanceToPlane(const float normal[3], const float vertex[3],
                                         const float point[3]);
                                         
      /// compute the signed distance of a point to a plane
      static double signedDistanceToPlane(const double normal[3], const double vertex[3],
                                         const double point[3]);
                                         
      /// compute the signed area of a triangle in the X-Y plane
      static float signedTriangleArea2D(const float* p1, const float* p2, const float* p3);

      /// compute the signed area of a triangle in 3D space
      static float signedTriangleArea3D(const float* normal,
                                        const float* p1, const float* p2, const float* p3);

      /// compute the signed area of a triangle in 3D space
      static double signedTriangleArea3D(const double* normal,
                                        const double* p1, const double* p2, const double* p3);

      /// Compute the area of a triangle (copied from VTK's float version).
      static double triangleArea(const double p1[3],
                                 const double p2[3],
                                 const double p3[3]);
      
      /// Compute the area of a triangle (copied from VTK's float version).
      static float triangleArea(const float p1[3],
                                 const float p2[3],
                                 const float p3[3]);
      
      /// Compute the normal of three points (copied from VTK)
      static void computeNormal(const double v1[3], const double v2[3], const double v3[3], double n[3]);
      
      /// Compute the normal of three points (copied from VTK)
      static void computeNormal(const float v1[3], const float v2[3], const float v3[3], float n[3]);
      
      /// normalized cross product
      static void normalizedCrossProduct(const float* x1,
                                         const float* x2,
                                         float* product);

      /// convert radians to degrees
      static float radiansToDegrees() { return 57.2957795130; }

      /// convert degrees to radians
      static float degreesToRadians() { return 0.0174444444; }
      
      /// Determine if 2D lines intersect
      static bool lineIntersection2D(const float p1[2], const float p2[2],
                                     const float q1[2], const float q2[2],
                                     float intersection[2]);

      /// Determine if and where a ray intersects a plane
      static bool rayIntersectPlane(const float p1[3], 
                                    const float p2[3], 
                                    const float p3[3],
                                    const float rayOrigin[3],
                                    const float rayVector[3],
                                    float intersection[3],
                                    float* signedDistanceFromPlaneOut = NULL);
      
      /// Determine if and where a ray intersects a plane
      static bool rayIntersectPlane(const double p1[3], 
                                    const double p2[3], 
                                    const double p3[3],
                                    const double rayOrigin[3],
                                    const double rayVector[3],
                                    double intersection[3],
                                    double* signedDistanceFromPlaneOut = NULL);
      
      /// make a vector the specified length
      static void setVectorLength(float vector[3], const float newLength);
      
      /// compute the circumcenter of a 3D triangle
      static void triangleCircumcenter3D(const double a[3],
                                         const double b[3],
                                         const double c[3], 
                                         double circumcenter[3], 
                                         double& circumradius);
                                      
      /// compute the circumcenter of a 3D triangle
      static void triangleCircumcenter3D(const float a[3],
                                         const float b[3],
                                         const float c[3], 
                                         float circumcenter[3], 
                                         float& circumradius);
      
      /// limit a value to be non-negative
      static float limitToPositive(const float v);
      
      /// make the sign of the first arg the same as the sign of the second arg
      static void sign(float& val, const float useMySignArg);
                                 
      /// make the sign of the first arg the same as the sign of the second arg
      static void sign(double& val, const double useMySignArg);
                 
      /// logarithm for general (ie LOG(2, 32) = 5;   2^5 = 32)
      static double log(const double base, const double num);
      
      /// cross product
      static void crossProduct(const float v1[3], 
                               const float v2[3],
                               float crossedVector[3]);
      
      /// cross product
      static void crossProduct(const double v1[3], 
                               const double v2[3],
                               double crossedVector[3]);
                               
      /// factorial
      static int factorial(const int n);
      
      /// permutations (number of possible ordered groups: N=num elements, r=size of permutation)
      /// permutations = n! / (n - r)!
      static int permutations(const int n, const int r);
      
      /// combinations (number of possible unordered groups: N=num elements, k=size of combination)
      /// combinations = n! / (k! * (n - k)!)
      static int combinations(const int n, const int k);
      
   protected:
};

#endif  // __VE_MATH_H__

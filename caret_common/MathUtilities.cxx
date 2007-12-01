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

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: MathUtilities.cxx,v $
  Language:  C++
  Date:      $Date: 2007/07/10 14:10:05 $
  Version:   $Revision: 1.1.1.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

#include "vtkPlane.h"
#include "vtkPolygon.h"
#include "vtkTriangle.h"

#include "MathUtilities.h"

/**
 * limit a value to be non-negative.
 */
float 
MathUtilities::limitToPositive(const float vIn)
{
   float v = vIn;
   if (v < 0.0) {
      v = 0.0;
   }
   return v;
}

/**
 * Distance^2 between two 3D points.
 */
float
MathUtilities::distanceSquared3D(const float* p1, const float* p2)
{
   const float dx = p1[0] - p2[0];
   const float dy = p1[1] - p2[1];
   const float dz = p1[2] - p2[2];
   const float dist2 = dx*dx + dy*dy + dz*dz;
   return dist2;
}

/**
 * Distance^2 between two 3D points.
 */
double
MathUtilities::distanceSquared3D(const double* p1, const double* p2)
{
   const double dx = p1[0] - p2[0];
   const double dy = p1[1] - p2[1];
   const double dz = p1[2] - p2[2];
   const double dist2 = dx*dx + dy*dy + dz*dz;
   return dist2;
}


/**
 * Distance between two 3D points.
 */
float
MathUtilities::distance3D(const float* p1, const float* p2)
{
   const float dx = p1[0] - p2[0];
   const float dy = p1[1] - p2[1];
   const float dz = p1[2] - p2[2];
   const float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
   return dist;
}

/**
 * Distance between two 3D points.
 */
double
MathUtilities::distance3D(const double* p1, const double* p2)
{
   const double dx = p1[0] - p2[0];
   const double dy = p1[1] - p2[1];
   const double dz = p1[2] - p2[2];
   const double dist = std::sqrt(dx*dx + dy*dy + dz*dz);
   return dist;
}

/**
 * distance of a point to an infinite line in 3D.
 * "pt" is the point.  "v1" and "v2" are points on the line.
 * Formula is from "http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html".
 */
float
MathUtilities::distancePointToLine3D(const float pt[3], const float v1[3], const float v2[3])
{
   float dv2v1[3];
   subtractVectors(v2, v1, dv2v1);
   float dv1pt[3];
   subtractVectors(v1, pt, dv1pt);
  
   float crossed[3];
   MathUtilities::crossProduct(dv2v1, dv1pt, crossed);
  
   float numerator = MathUtilities::vectorLength(crossed);
   float denomenator = MathUtilities::vectorLength(dv2v1);
  
   float dist = numerator / denomenator;
   return dist;
}

/**
 * distance of a point to an infinite line in 3D.
 * "pt" is the point.  "v1" and "v2" are points on the line.
 * Formula is from "http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html".
 */
double
MathUtilities::distancePointToLine3D(const double pt[3], const double v1[3], const double v2[3])
{
   double dv2v1[3];
   subtractVectors(v2, v1, dv2v1);
   double dv1pt[3];
   subtractVectors(v1, pt, dv1pt);
  
   double crossed[3];
   MathUtilities::crossProduct(dv2v1, dv1pt, crossed);
  
   double numerator = MathUtilities::vectorLength(crossed);
   double denomenator = MathUtilities::vectorLength(dv2v1);
  
   double dist = numerator / denomenator;
   return dist;
}

/**
 * subtract vectors (3d)  result = v1 - v2.
 */
void 
MathUtilities::subtractVectors(const float v1[3], const float v2[3], float result[3])
{
   result[0] = v1[0] - v2[0];
   result[1] = v1[1] - v2[1];
   result[2] = v1[2] - v2[2];
}

/**
 * subtract vectors (3d)  result = v1 - v2.
 */
void 
MathUtilities::subtractVectors(const double v1[3], const double v2[3], double result[3])
{
   result[0] = v1[0] - v2[0];
   result[1] = v1[1] - v2[1];
   result[2] = v1[2] - v2[2];
}

/**
 * Compute the signed area of a triangle in the X-Y plane
 */
float
MathUtilities::signedTriangleArea2D(const float* p1, const float* p2, 
                                    const float* p3)
{
   float area = (  p1[0]*p2[1] + p2[0]*p3[1] + p3[0]*p1[1]
                 - p1[1]*p2[0] - p2[1]*p3[0] - p3[1]*p1[0] ) * 0.5;
   return area;
}

/**
 * Compute the signed area of a triangle in the 3D space 
 */
float
MathUtilities::signedTriangleArea3D(const float* referenceNormal, const float* p1,  
                                    const float* p2, const float* p3)
{
   //
   // Area of the triangle formed by the three points
   //
   float area = triangleArea((float*)p1, (float*)p2, (float*)p3);
   
   //
   // Normal for the three points
   //
   float triangleNormal[3];
   computeNormal(p1, p2, p3, triangleNormal);
   
   //
   // Dot Product is the cosine of the angle between the two normals.  When this value is less
   // than zero, the absolute angle between the normals is greater than 90 degrees.
   //
   const float dot = MathUtilities::dotProduct(referenceNormal, triangleNormal);
   if (dot < 0.0) {
      area = -area;
   }
   //std::cout << "dot is " << dot << std::endl;
   
   return area;
}

/**
 * Compute the area of a triangle (copied from VTK's float version).
 */
double
MathUtilities::triangleArea(const double p1[3],
                            const double p2[3],
                            const double p3[3])
{
  const double a = distanceSquared3D(p1,p2);
  const double b = distanceSquared3D(p2,p3);
  const double c = distanceSquared3D(p3,p1);
  const double area = (0.25* std::sqrt(fabs(4.0*a*c - (a-b+c)*(a-b+c))));
  return area;
}

/**
 * Compute the area of a triangle (copied from VTK's float version).
 */
float
MathUtilities::triangleArea(const float p1[3],
                            const float p2[3],
                            const float p3[3])
{
  const float a = distanceSquared3D(p1,p2);
  const float b = distanceSquared3D(p2,p3);
  const float c = distanceSquared3D(p3,p1);
  const float area = (0.25* std::sqrt(fabs(4.0*a*c - (a-b+c)*(a-b+c))));
  return area;
}

/**
 * Compute the normal of three points (copied from VTK).
 */
void 
MathUtilities::computeNormal(const double v1[3], const double v2[3], const double v3[3], double n[3])
{
  double length;

  vtkTriangle::ComputeNormalDirection((double*)v1, (double*)v2, (double*)v3, n);

  if ( (length = std::sqrt((n[0]*n[0] + n[1]*n[1] + n[2]*n[2]))) != 0.0 )
    {
    n[0] /= length;
    n[1] /= length;
    n[2] /= length;
    }
}

/**
 * Compute the normal of three points (copied from VTK).
 */
void 
MathUtilities::computeNormal(const float v1[3], const float v2[3], const float v3[3], float n[3])
{
   double v1d[3] = { v1[0], v1[1], v1[2] };
   double v2d[3] = { v2[0], v2[1], v2[2] };
   double v3d[3] = { v3[0], v3[1], v3[2] };
   double nd[3];
   
   computeNormal(v1d, v2d, v3d, nd);
   n[0] = nd[0];
   n[1] = nd[1];
   n[2] = nd[2];
}

/**
 * see if point is in a polygon.
 */
int
MathUtilities::pointInPolygon(const double x[3], const int numPts, const double *pts,
                              const double bounds[6], const double n[3])
{
#ifdef HAVE_VTK5
   const int result = vtkPolygon::PointInPolygon((double*)x, 
                                                 (int)numPts, 
                                                 (double*)pts, 
                                                 (double*)bounds, 
                                                 (double*)n);
   return result;
#else // HAVE_VTK5
   if (numPts > 0) {
      float* dpts = new float[numPts * 3];
      for (int i = 0; i < (numPts * 3); i++) {
         dpts[i] = pts[i];
      }
      float dx[3] = { x[0], x[1], x[2] };
      float dbounds[6] = { bounds[0], bounds[1], bounds[2],
                            bounds[3], bounds[4], bounds[5] 
                          };
      float dn[3] = { n[0], n[1], n[2] };
      
      int result = pointInPolygon(dx, numPts, dpts, dbounds, dn);
      delete[] dpts;
      return result;
   }
   else {
      return 0;
   }
#endif // HAVE_VTK5
}

/**
 * see if point is in a polygon.
 */
int 
MathUtilities::pointInPolygon(const float x[3], const int numPts, const float *pts,
                              const float bounds[6], const float n[3])
{
#ifdef HAVE_VTK5
   if (numPts > 0) {
      double* dpts = new double[numPts * 3];
      for (int i = 0; i < (numPts * 3); i++) {
         dpts[i] = pts[i];
      }
      double dx[3] = { x[0], x[1], x[2] };
      double dbounds[6] = { bounds[0], bounds[1], bounds[2],
                            bounds[3], bounds[4], bounds[5] 
                          };
      double dn[3] = { n[0], n[1], n[2] };
      
      int result = pointInPolygon(dx, numPts, dpts, dbounds, dn);
      delete[] dpts;
      return result;
   }
   else {
      return 0;
   }
#else // HAVE_VTK5
   const int result = vtkPolygon::PointInPolygon((float*)x, 
                                                 (int)numPts, 
                                                 (float*)pts, 
                                                 (float*)bounds, 
                                                 (float*)n);
   return result;
#endif // HAVE_VTK5
}
                                
/**
 * dot product.
 */
float 
MathUtilities::dotProduct(const float* p1, const float* p2)
{
   const float dot = p1[0]*p2[0] + p1[1]*p2[1] + p1[2]*p2[2];
   return dot;
}

/**
 * dot product.
 */
double 
MathUtilities::dotProduct(const double* p1, const double* p2)
{
   const double dot = p1[0]*p2[0] + p1[1]*p2[1] + p1[2]*p2[2];
   return dot;
}
      
/**
 * Compute the signed area of a triangle in the 3D space 
 */
double
MathUtilities::signedTriangleArea3D(const double* referenceNormal, const double* p1,  
                                    const double* p2, const double* p3)
{
   //
   // Area of the triangle formed by the three points
   //
   double area = MathUtilities::triangleArea((double*)p1, (double*)p2, (double*)p3);
   
   //
   // Normal for the three points
   //
   double triangleNormal[3];
   computeNormal(p1, p2, p3, triangleNormal);
   
   //
   // Dot Product is the cosine of the angle between the two normals.  When this value is less
   // than zero, the absolute angle between the normals is greater than 90 degrees.
   //
   const double dot = MathUtilities::dotProduct(referenceNormal, triangleNormal);
   if (dot < 0.0) {
      area = -area;
   }
   //std::cout << "dot is " << dot << std::endl;
   
   return area;
}

/**
 * Determine the signed distance (positive is "above" plane, negative "below" plane).
 * normal - normal of the plane
 * vertex - a point on the plane
 * point - point distance is computed for.
 */
float 
MathUtilities::signedDistanceToPlane(const float normal[3], const float vertex[3],
                                     const float point[3])
{
   //
   // Project query point onto tile's plane
   //
   float projectedOntoPlane[3];
#ifdef HAVE_VTK5
   double p[3] = { point[0], point[1], point[2] };
   double v[3] = { vertex[0], vertex[1], vertex[2] };
   double n[3] = { normal[0], normal[1], normal[2] };
   double out[3];
   vtkPlane::ProjectPoint((double*)p, (double*)v, (double*)n, out);
   projectedOntoPlane[0] = out[0];
   projectedOntoPlane[1] = out[1];
   projectedOntoPlane[2] = out[2];
#else  // HAVE_VTK5
   vtkPlane::ProjectPoint((float*)point, (float*)vertex, (float*)normal, projectedOntoPlane);
#endif // HAVE_VTK5
   //
   // Distance from query point to tile
   //
   const float dx = normal[0] * (point[0] - projectedOntoPlane[0]);
   const float dy = normal[1] * (point[1] - projectedOntoPlane[1]);
   const float dz = normal[2] * (point[2] - projectedOntoPlane[2]);
   const float signedDistance = dx + dy + dz;
/*   
   const float D = -(normal[0]*vertex[0] + normal[1]*vertex[1] + normal[2]*vertex[2]);
   const float signedDistance2 = normal[0]*point[0] + normal[1]*point[1] + normal[2]*point[2] + D;
   std::cout << "signed distance: " << signedDistance << " "
             << "signed distance2 " << signedDistance2 << std::endl;
*/
   return signedDistance;
}

/**
 * Determine the signed distance (positive is "above" plane, negative "below" plane).
 * normal - normal of the plane
 * vertex - a point on the plane
 * point - point distance is computed for.
 */
double 
MathUtilities::signedDistanceToPlane(const double normal[3], const double vertex[3],
                                     const double point[3])
{
   // Project query point onto tile's plane
   //
   double projectedOntoPlane[3];
   vtkPlane::ProjectPoint((double*)point, (double*)vertex, (double*)normal, projectedOntoPlane);
   
   //
   // Distance from query point to tile
   //
   const double dx = normal[0] * (point[0] - projectedOntoPlane[0]);
   const double dy = normal[1] * (point[1] - projectedOntoPlane[1]);
   const double dz = normal[2] * (point[2] - projectedOntoPlane[2]);
   const double signedDistance = dx + dy + dz;
/*   
   const float D = -(normal[0]*vertex[0] + normal[1]*vertex[1] + normal[2]*vertex[2]);
   const float signedDistance2 = normal[0]*point[0] + normal[1]*point[1] + normal[2]*point[2] + D;
   std::cout << "signed distance: " << signedDistance << " "
             << "signed distance2 " << signedDistance2 << std::endl;
*/
   return signedDistance;
}

/**
 * Angle formed by p1, p2, p3 (angle at p2).  Returned angle is in radians.
 */
float
MathUtilities::angle(const float* p1, const float* p2, const float* p3)
{
   //
   // Vector from P2 to P1
   //
   float v21[3] = { p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2] };
   
   //
   // Vector from P2 to P3
   //
   float v23[3] = { p3[0] - p2[0], p3[1] - p2[1], p3[2] - p2[2] };
   
   //
   // Normalize the vectors
   //
   const float v21len = MathUtilities::normalize(v21);
   const float v23len = MathUtilities::normalize(v23);
   
   float angleOut = 0.0;
   if ((v21len > 0.0) && (v23len > 0.0)) {
      //
      // angle is inverse cosine of the dot product
      // and be sure to handle numerical errors.
      //
      float dot = MathUtilities::dotProduct(v21, v23);
      if (dot > 1.0) dot = 1.0;
      else if (dot < -1.0) dot = -1.0;
      angleOut = std::acos(dot);
   }
   
   return angleOut;
}

/**
 * Signed angle for "jik".
 */
float 
MathUtilities::signedAngle(const float pi[3], const float pj[3], 
                           const float pk[3], const float n[3]) 
{
  const float x1 = pj[0] - pi[0];
  const float y1 = pj[1] - pi[1];
  const float z1 = pj[2] - pi[2];
  const float x2 = pk[0] - pi[0];
  const float y2 = pk[1] - pi[1];
  const float z2 = pk[2] - pi[2];

  /* s = |(ji)||(ki)| sin(phi) by cross product */
  const float dx = y1*z2 - y2*z1;
  const float dy = x2*z1 - x1*z2;
  const float dz = x1*y2 - x2*y1;
  const float t = (dx*n [0]) + (dy*n [1]) + (dz*n [2]);
  float s = std::sqrt((dx*dx) + (dy*dy) + (dz*dz));
  if (t < 0.0) {
     s = -s;
   }

  /* c = |(ji)||(ki)| cos(phi) by inner product */
  const float c = x1*x2 + y1*y2 + z1*z2;
  const float phi = std::atan2(s,c);
  return phi;
}

/**
 * Normalized cross product
 */
void 
MathUtilities::normalizedCrossProduct(const float* x1,
                                           const float* x2,
                                           float* product)
{
   float v1[3] = { x1[0], x1[1], x1[2] };
   float v2[3] = { x2[0], x2[1], x2[2] };
   MathUtilities::normalize(v1);
   MathUtilities::normalize(v2);
   MathUtilities::crossProduct(v1, v2, product);
   MathUtilities::normalize(product);
/*
   MathUtilities::crossProduct(x1, x2, product);
   MathUtilities::vectorLength(product);

   product[0] = x1[1] * x2[2] - x2[1] * x1[2];
   product[1] = x2[0] * x1[2] - x1[0] * x2[2];
   product[2] = x1[0] * x2[1] - x2[0] * x1[1];
   const float nmag = MathUtilities::vectorLength(product);
   if (nmag > 0.0) {
      product[0] /= nmag;
      product[1] /= nmag;
      product[2] /= nmag;
   }
   else {
      product[0] = 0.0;
      product[1] = 0.0;
      product[2] = 0.0;
   }
*/
}

/**
 * Determine if 2D lines intersect.
 * Algorithm from http://mathworld.wolfram.com/Line-LineIntersection.html
 */
bool
MathUtilities::lineIntersection2D(const float p1[2], const float p2[2],
                                  const float q1[2], const float q2[2],
                                  float intersection[2])
{
   const double tol = 0.01;
   
   const double x1 = p1[0];
   const double y1 = p1[1];
   const double x2 = p2[0];
   const double y2 = p2[1];
   
   const double x3 = q1[0];
   const double y3 = q1[1];
   const double x4 = q2[0];
   const double y4 = q2[1];
   
   const double denom = ((x1 - x2) * (y3 - y4)) - ((x3 - x4) * (y1 - y2));
   
   if (denom != 0.0) {
      const double a = (x1 * y2) - (x2 * y1);
      const double c = (x3 * y4) - (x4 * y3);
      const double x = ((a * (x3 - x4)) - (c * (x1 - x2))) / denom;
      const double y = ((a * (y3 - y4)) - (c * (y1 - y2))) / denom;
      
      const double pxMax = std::max(x1, x2) + tol;
      const double pxMin = std::min(x1, x2) - tol;
      const double pyMax = std::max(y1, y2) + tol;
      const double pyMin = std::min(y1, y2) - tol;
      
      const double qxMax = std::max(x3, x4) + tol;
      const double qxMin = std::min(x3, x4) - tol;
      const double qyMax = std::max(y3, y4) + tol;
      const double qyMin = std::min(y3, y4) - tol;
      
      intersection[0] = x;
      intersection[1] = y;
      if ((x >= pxMin) && (x <= pxMax) && (x >= qxMin) && (x <= qxMax) &&
          (y >= pyMin) && (y <= pyMax) && (y >= qyMin) && (y <= qyMax)) {
         return true;
      }
   }
   
   return false;
}

/**
 * make a vector the specified length.
 */
void 
MathUtilities::setVectorLength(float vector[3], const float newLength)
{
   const float len = std::sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
   if (len > 0) {
      const float scale = newLength / len;
      vector[0] *= scale;
      vector[1] *= scale;
      vector[2] *= scale;
   }
}

/**
 * See if a ray intersects a plane.
 * The plane is defined by 3 points (p1, p2, p3)
 * Returns true if the ray intersects the plane and false
 * if the ray is parallel to the plane.
 */
bool
MathUtilities::rayIntersectPlane(const float p1[3], 
                                 const float p2[3], 
                                 const float p3[3],
                                 const float rayOrigin[3],
                                 const float rayVector[3],
                                 float intersection[3],
                                 float* signedDistanceFromPlaneOut)
{
   //
   // Convert the ray into a unit vector
   //
   double ray[3] = { rayVector[0], rayVector[1], rayVector[2] };
   MathUtilities::normalize(ray);
   
   //
   // Normal of plane
   //
   float normal[3];
   computeNormal(p1, p2, p3, normal);
   
   //
   // Compute the plane equation
   //
   const double A = normal[0];
   const double B = normal[1];
   const double C = normal[2];
   const double D = -(A*p1[0] + B*p1[1] + C*p1[2]);
   
   //
   // Parametric coordinate of where ray intersects plane
   //
   const double denom = A * ray[0] + B * ray[1] + C * ray[2];
   double t = 0.0;
   if (denom != 0) {
      t = -(A * rayOrigin[0] + B * rayOrigin[1] + C * rayOrigin[2] + D) / denom;
      
      //if (t != 0.0) {
         intersection[0] = rayOrigin[0] + ray[0] * t;
         intersection[1] = rayOrigin[1] + ray[1] * t;
         intersection[2] = rayOrigin[2] + ray[2] * t;
         
         if (signedDistanceFromPlaneOut != NULL) {
            *signedDistanceFromPlaneOut = t;
         }

         return true;
      //}
   }
   
   return false;
}

/**
 * See if a ray intersects a plane.
 * The plane is defined by 3 points (p1, p2, p3)
 * Returns true if the ray intersects the plane and false
 * if the ray is parallel to the plane.
 */
bool
MathUtilities::rayIntersectPlane(const double p1[3], 
                                 const double p2[3], 
                                 const double p3[3],
                                 const double rayOrigin[3],
                                 const double rayVector[3],
                                 double intersection[3],
                                 double* signedDistanceFromPlaneOut)
{
   //
   // Convert the ray into a unit vector
   //
   double ray[3] = { rayVector[0], rayVector[1], rayVector[2] };
   MathUtilities::normalize(ray);
   
   //
   // Normal of plane
   //
   double normal[3];
   computeNormal(p1, p2, p3, normal);
   
   //
   // Compute the plane equation
   //
   const double A = normal[0];
   const double B = normal[1];
   const double C = normal[2];
   const double D = -(A*p1[0] + B*p1[1] + C*p1[2]);
   
   //
   // Parametric coordinate of where ray intersects plane
   //
   const double denom = A * ray[0] + B * ray[1] + C * ray[2];
   double t = 0.0;
   if (denom != 0) {
      t = -(A * rayOrigin[0] + B * rayOrigin[1] + C * rayOrigin[2] + D) / denom;
      
      //if (t != 0.0) {
         intersection[0] = rayOrigin[0] + ray[0] * t;
         intersection[1] = rayOrigin[1] + ray[1] * t;
         intersection[2] = rayOrigin[2] + ray[2] * t;
         
         if (signedDistanceFromPlaneOut != NULL) {
            *signedDistanceFromPlaneOut = t;
         }
         
         return true;
      //}
   }
   
   return false;
}

/**
 * From http://www.ics.uci.edu/~eppstein/junkyard/circumcenter.html
 *                                                                           
 *  Find the circumcenter and circumradiusof a triangle in 3D.         
 *                                                                           
 */
void 
MathUtilities::triangleCircumcenter3D(const float a[3],
                                      const float b[3],
                                      const float c[3], 
                                      float circumcenter[3], 
                                      float& circumradius)
{
   double ad[3], bd[3], cd[3];
   for (int i = 0; i < 3; i++) {
      ad[i] = a[i];
      bd[i] = b[i];
      cd[i] = c[i];
   }
   
   double center[3], radius;
   triangleCircumcenter3D(ad, bd, cd, center, radius);
   
   circumcenter[0] = center[0];
   circumcenter[1] = center[1];
   circumcenter[2] = center[2];
   circumradius    = radius;
}

/**
 * From http://www.ics.uci.edu/~eppstein/junkyard/circumcenter.html
 *                                                                           
 *  Find the circumcenter and circumradiusof a triangle in 3D.         
 *                                                                           
 */
void 
MathUtilities::triangleCircumcenter3D(const double a[3],
                                      const double b[3],
                                      const double c[3], 
                                      double circumcenter[3], 
                                      double& circumradius)
{
  double xba, yba, zba, xca, yca, zca;
  double balength, calength;
  double xcrossbc, ycrossbc, zcrossbc;
  double denominator;
  double xcirca, ycirca, zcirca;

  //
  // Use coordinates relative to point `a' of the triangle.
  //
  xba = b[0] - a[0];
  yba = b[1] - a[1];
  zba = b[2] - a[2];
  xca = c[0] - a[0];
  yca = c[1] - a[1];
  zca = c[2] - a[2];
  
  //
  // Squares of lengths of the edges incident to `a'.
  //
  balength = xba * xba + yba * yba + zba * zba;
  calength = xca * xca + yca * yca + zca * zca;
  
  //
  // Cross product of these edges.
  //
#ifdef EXACT
  //
  // Use orient2d() from http://www.cs.cmu.edu/~quake/robust.html     
  //   to ensure a correctly signed (and reasonably accurate) result, 
  //   avoiding any possibility of division by zero.                  
  //
  xcrossbc = orient2d(b[1], b[2], c[1], c[2], a[1], a[2]);
  ycrossbc = orient2d(b[2], b[0], c[2], c[0], a[2], a[0]);
  zcrossbc = orient2d(b[0], b[1], c[0], c[1], a[0], a[1]);
#else
  /* Take your chances with floating-point roundoff. */
  xcrossbc = yba * zca - yca * zba;
  ycrossbc = zba * xca - zca * xba;
  zcrossbc = xba * yca - xca * yba;
#endif

  //
  // Calculate the denominator of the formulae. 
  //
  denominator = 0.5 / (xcrossbc * xcrossbc + ycrossbc * ycrossbc +
                       zcrossbc * zcrossbc);

  //
  // Calculate offset (from `a') of circumcenter.
  //
  xcirca = ((balength * yca - calength * yba) * zcrossbc -
            (balength * zca - calength * zba) * ycrossbc) * denominator;
  ycirca = ((balength * zca - calength * zba) * xcrossbc -
            (balength * xca - calength * xba) * zcrossbc) * denominator;
  zcirca = ((balength * xca - calength * xba) * ycrossbc -
            (balength * yca - calength * yba) * xcrossbc) * denominator;
  circumcenter[0] = xcirca;
  circumcenter[1] = ycirca;
  circumcenter[2] = zcirca;

  circumradius = MathUtilities::vectorLength(circumcenter);  

  //
  // Make coords absolute
  //
  circumcenter[0] += a[0];
  circumcenter[1] += a[1];
  circumcenter[2] += a[2];
}

/**
 * make the sign of the first arg the same as the sign of the second arg.
 */
void 
MathUtilities::sign(float& val, const float useMySignArg)
{
   if (useMySignArg > 0.0) {
      if (val < 0.0) {
         val = -val;
      }
   }
   else if (useMySignArg < 0.0) {
      if (val > 0.0) {
         val = -val;
      }
   }
}
                           
/**
 * make the sign of the first arg the same as the sign of the second arg.
 */
void 
MathUtilities::sign(double& val, const double useMySignArg)
{
   if (useMySignArg > 0.0) {
      if (val < 0.0) {
         val = -val;
      }
   }
   else if (useMySignArg < 0.0) {
      if (val > 0.0) {
         val = -val;
      }
   }
}

/**
 * logarithm for 2 (ie log(2, 32) = 5;   2^5 = 32).
 *
 * From http://www-lmmb.ncifcrf.gov/~toms/paper/primer/latex/node2.html
 *  log2(num) = (log10(num) / log10(2)
 */
double 
MathUtilities::log(const double base, const double num)
{
   if (num < 0.0) {
      return 0.0;
   }
   if (base < 0.0) {
      return 0.0;
   }
   
   const float val = std::log10(num) / std::log10(base);
   
   return val;
}

/**
 * normalize a vector.
 */
float 
MathUtilities::normalize(float v[3])
{
   const float len = vectorLength(v);
   if (len > 0) {
      v[0] /= len;
      v[1] /= len;
      v[2] /= len;
   }   
   return len;
}

/**
 * normalize a vector.
 */
double 
MathUtilities::normalize(double v[3])
{
   const double len = vectorLength(v);
   if (len > 0) {
      v[0] /= len;
      v[1] /= len;
      v[2] /= len;
   }   
   return len;
}

/**
 * get length of a vector.
 */
float 
MathUtilities::vectorLength(const float v[3])
{
   const float len = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
   return len;
}

/**
 * get length of a vector.
 */
double 
MathUtilities::vectorLength(const double v[3])
{
   const float len = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
   return len;
}
     
/**
 * cross product.
 */
void 
MathUtilities::crossProduct(const float v1[3], 
                            const float v2[3],
                            float crossedVector[3])
{
  crossedVector[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  crossedVector[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  crossedVector[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

/**
 * cross product.
 */
void 
MathUtilities::crossProduct(const double v1[3], 
                            const double v2[3],
                            double crossedVector[3])
{
  crossedVector[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  crossedVector[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  crossedVector[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

/**
 * factorial.
 */
int 
MathUtilities::factorial(const int n)
{
   if (n <= 1) {
      return 1;
   }
   
   int product = 1;
   for (int i = 1; i <= n; i++) {
      product *= i;
   }
   return product;
}

/**
 * permutations (number of possible ordered groups: N=num elements, r=size of permutation)./
 * permutations = n! / (n - r)!
 */
int 
MathUtilities::permutations(const int n, const int r)
{
   const int numerator = factorial(n);
   const int denominator = factorial(n - r);
   const int p = numerator / denominator;
   return p;
}

/**
 * combinations (number of possible unordered groups: N=num elements, k=size of combination).
 * combinations = n! / (k! * (n - k)!)
 */
int 
MathUtilities::combinations(const int n, const int k)
{
   const int numerator = factorial(n);
   const int denominator = factorial(k) * factorial(n - k);
   const int c = numerator / denominator;
   return c;
}      

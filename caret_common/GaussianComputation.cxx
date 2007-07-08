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

#include "GaussianComputation.h"
#include "MathUtilities.h"

/**
 * Constructor.  
 *
 * @param normBelowCutoffIn  This is the cutoff distance in a direction opposite to
 * the normal vector (distance "under" a plane tangent to the reference point).
 *
 * @param normAboveCutoffIn  This is the cutoff distance in the direction of the 
 * normal vector (distance "above" a plane tangent to the reference point).
 * 
 * @param sigmaNormIn  Standard deviation for gaussian along the normal vector
 *
 * @param sigmaTangIn  Standard deviation for gaussian alng plane tangenet to ref point
 *
 * @tangentCutoffIn  Cutoff distance along plane tangent to reference point
 *
 * For more info: http://mathworld.wolfram.com/GaussianFunction.html
 */
GaussianComputation::GaussianComputation(const float normBelowCutoffIn,
                                  const float normAboveCutoffIn,
                                  const float sigmaNormIn,
                                  const float sigmaTangIn,
                                  const float tangentCutoffIn)
{
   normBelowCutoff = normBelowCutoffIn;
   normAboveCutoff = normAboveCutoffIn;
   tangentCutoff   = tangentCutoffIn;
   sigmaNorm       = sigmaNormIn;
   sigmaTang       = sigmaTangIn;
}
         
/**
 * destructor.
 */
GaussianComputation::~GaussianComputation()
{
}

/** evaluate for a set of points related to the reference point
 * returns gaussian computed from all points.
 */
float 
GaussianComputation::evaluate(const float referencePointXYZ[3],
                                  const float referencePointNormal[3],
                                  std::vector<Point3D>& points) const
{
   float valueSum  = 0.0;
   float weightSum = 0.0;
   
   const int num = static_cast<int>(points.size());
   for (int i = 0; i < num; i++) {
      points[i].weight = evaluate(referencePointXYZ,
                                  referencePointNormal,
                                  points[i].xyz);
      valueSum  += points[i].value * points[i].weight;
      weightSum += points[i].weight;
   }
   
   float result = 0.0;
   
   if (weightSum > 0.0) {
      result = valueSum / weightSum;
   }
   
   return result;
}
         
/**
 * evaluate for a point related to the reference point (returns weight).
 */
float 
GaussianComputation::evaluate(const float referencePointXYZ[3],
                                  const float referencePointNormal[3],
                                  const float evaluationPoint[3]) const
{
   float weight = 0.0;
   
   // 
   // "DistanceVector" is the offset vector of a voxel from the origin of the gaussian. 
   //
   const float distanceVector[3] = {
                                      evaluationPoint[0] - referencePointXYZ[0],
                                      evaluationPoint[1] - referencePointXYZ[1],
                                      evaluationPoint[2] - referencePointXYZ[2]
                                   };
   //
   // "dNorm" is"distanceVector" projected onto the normal.
   // See "Computer Graphics" 2nd ed pages 1096-1097
   //
   //   nodeNormal
   //      /|\          x
   //       |
   //       | dTang
   //       ------#       # = voxelXYZ
   //     d ||    /
   //     N ||   /
   //     o ||  / distanceVector
   //     r || /
   //     m ||/
   //       *              * = nodeXYZ
   //
   const float dNorm = MathUtilities::dotProduct(referencePointNormal, distanceVector);
   
   //
   // See if the voxel of interest is within the allowable distances along the normal vector.
   //
   float Wnorm = 0.0;               
   if ((dNorm > -normBelowCutoff) &&
       (dNorm < normAboveCutoff)) {
      //
      // Wnorm is the weighting for the distance along the normal vector.
      // The weighting is inversely related to the distance from the node.
      //
      Wnorm = exp (-(dNorm * dNorm) / (2.0 * sigmaNorm * sigmaNorm));
   }                    

   if (Wnorm > 0.0)  {
      //
      // Tang is the distance vector components from the voxel 
      // to the nearest point on infinitely long normal vector.
      //
      float Tang[3];
      for (int i = 0; i < 3; i++) {
         Tang[i] = distanceVector[i] - dNorm*referencePointNormal[i];
      }
      
      //
      // dTang is the distance from the end of the voxel to the closest
      // point on the infinitely long normal vector.
      //
      const float dTang = sqrt(Tang[0]*Tang[0] + Tang[1]*Tang[1] +
                               Tang[2]*Tang[2]);
                               
      //
      // See if voxel distance tangentially is within the limits.
      //
      float Wtang = 0.0;
      if (dTang < tangentCutoff) {
         //
         // Wtang is the weighting for the distance orthogonal to the 
         // normal vector.  The weighting is inversely related to the 
         // distance from the origin.
         //
         Wtang = exp (-((dTang * dTang)
                        / (2.0 * sigmaTang * sigmaTang)));
      }
      
      if (Wtang > 0.0) {
         //
         // Combine the weights and update the metric and weight sums.
         //
         weight = Wnorm * Wtang;
      }
   }
   
   return weight;
}

/**
 * tooltip text for norm below cutoff parameter.
 */
QString 
GaussianComputation::tooltipTextForNormBelowCutoff()
{
   return "The cutoff distance in direction opposite\n"
          "of the node's normal vector.\n"
          "(\"Under\" a plane tangent to the node).";
}

/**
 * tooltip text for norm above cutoff parameter.
 */
QString 
GaussianComputation::tooltipTextForNormAboveCutoff()
{
   return "The cutoff distance in the direction of\n"
          "the node's normal vector.\n"
          "(\"Above\" a plane tangent to the node).";
}

/**
 * tooltip text for tangent cutoff parameter.
 */
QString 
GaussianComputation::tooltipTextForTangentCutoff()
{
   return "Cutoff distance along a plane\n"
          "tangent to the node.";
}

/**
 * tooltip text for sigma norm parameter.
 */
QString 
GaussianComputation::tooltipTextForSigmaNorm()
{
   return "Standard deviation for gaussian\n"
          "along the node's normal vector.";
}

/**
 * tooltip text for sigma tang parameter.
 */
QString 
GaussianComputation::tooltipTextForSigmaTang()
{
   return "Standard deviation for gaussian\n"
          "along plane tangent to the node.";
}

/**
 * get default parameters.
 */
void 
GaussianComputation::getDefaultParameters(float& normBelowCutoffOut,
                                          float& normAboveCutoffOut,
                                          float& sigmaNormOut,
                                          float& sigmaTangOut,
                                          float& tangentCutoffOut)
{
   //
   // Use the constructor to get the values from default arguments
   //
   GaussianComputation gc;
   
   normBelowCutoffOut = gc.normBelowCutoff;
   normAboveCutoffOut = gc.normAboveCutoff;
   sigmaNormOut       = gc.sigmaNorm;
   sigmaTangOut       = gc.sigmaTang;
   tangentCutoffOut   = gc.tangentCutoff;
}


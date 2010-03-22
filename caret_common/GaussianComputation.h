
#ifndef __GAUSSIAN_COMPUTATION_H__
#define __GAUSSIAN_COMPUTATION_H__

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

#include <QString>
#include <vector>

/// class for gaussian evaluation
class GaussianComputation {
   public:
      /// class for a point that will be evaluated
      class Point3D {
         public:
            /// constructor
            Point3D(const float xyzIn[3],
                    const float valueIn) {
               xyz[0] = xyzIn[0];
               xyz[1] = xyzIn[1];
               xyz[2] = xyzIn[2];
               value  = valueIn;
               weight = 0.0;
            }
                    
            /// Destructor
            ~Point3D() { }
            
            /// get the weight
            float getWeight() const { return weight; }
         
         protected:
            /// coordinate
            float xyz[3];
            
            /// value
            float value;
            
            /// weight
            float weight;
            
         friend class GaussianComputation;
      };
      
      /// Constructor
      GaussianComputation(const float normBelowCutoffIn = 2.0,
                          const float normAboveCutoffIn = 2.0,
                          const float sigmaNormIn = 2.0,
                          const float sigmaTangIn = 2.0,
                          const float tangentCutoffIn = 3.0);
               
      /// destructor
      ~GaussianComputation();
      
      /// evaluate for a point related to the reference point(returns weight)
      float evaluate(const float referencePointXYZ[3],
                     const float referencePointNormal[3],
                     const float evaluationPoint[3]) const;
                     
      /// evaluate for a set of points related to the reference point
      /// returns gaussian computed from all points.  Each "points" 
      /// weight will also be set.
      float evaluate(const float referencePointXYZ[3],
                     const float referencePointNormal[3],
                     std::vector<Point3D>& points) const;
                     
      /// tooltip text for norm below cutoff parameter
      static QString tooltipTextForNormBelowCutoff();
   
      /// tooltip text for norm above cutoff parameter
      static QString tooltipTextForNormAboveCutoff();
   
      /// tooltip text for tangent cutoff parameter
      static QString tooltipTextForTangentCutoff();
   
      /// tooltip text for sigma norm parameter
      static QString tooltipTextForSigmaNorm();
   
      /// tooltip text for sigma tang parameter
      static QString tooltipTextForSigmaTang();
   
      /// get default parameters
      static void getDefaultParameters(float& normBelowCutoffOut,
                                       float& normAboveCutoffOut,
                                       float& sigmaNormOut,
                                       float& sigmaTangOut,
                                       float& tangentCutoffOut);

   protected:
      /// cutoff distance in direction opposite normal
      /// ("under" a plane tangent to the reference point)
      float normBelowCutoff;
      
      /// cutoff distance in direction of normal 
      /// ("above" a plane tangent to the reference point)
      float normAboveCutoff;
      
      /// cutoff for distance along a plane tangent to the reference point
      float tangentCutoff;
      
      /// standard deviation for gaussian along the normal vector
      float sigmaNorm;
      
      /// standard deviation for gaussian along plane tangent to the reference point
      float sigmaTang;
};
      

#endif //  __GAUSSIAN_COMPUTATION_H__


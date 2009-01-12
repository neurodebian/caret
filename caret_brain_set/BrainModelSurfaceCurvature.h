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

#ifndef __BRAIN_MODEL_SURFACE_CURVATURE_H__
#define __BRAIN_MODEL_SURFACE_CURVATURE_H__

#include <QString>
#include <vector>

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class SurfaceShapeFile;

/// Class to generate curvature for a surface
class BrainModelSurfaceCurvature : public BrainModelAlgorithm {
   public:
      enum CURVATUE_COLUMNS {
         CURVATURE_COLUMN_CREATE_NEW = -1,
         CURVATURE_COLUMN_DO_NOT_GENERATE = -2
      };
      
      /// Constructor
      BrainModelSurfaceCurvature(BrainSet* bs,
                                 const BrainModelSurface* surfaceIn,
                                 SurfaceShapeFile* shapeFileIn,
                                 const int meanCurvatureColumnIn,
                                 const int gaussianCurvatureColumnIn,
                                 const QString& meanCurvatureNameIn,
                                 const QString& gaussianCurvatureNameIn,
                                 const bool computePrincipalCurvatures = false);
      
      /// Destructor
      ~BrainModelSurfaceCurvature();
      
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// get the mean curvature column (0..N-1)
      int getMeanCurvatureColumnNumber() const { return meanCurvatureColumn; }
      
      /// get the mean curvature column (0..N-1)
      int getGaussianCurvatureColumnNumber() const { return gaussianCurvatureColumn; }
      
      /// get the k-max (k1, first principal curvature) column (0..N-1)
      int getKMaxColumnNumber() const { return kMaxColumn; }
      
      /// get the k-min (k2, second principal curvature) column (0..N-1)
      int getKMinColumnNumber() const { return kMinColumn; }
      
   protected:
      /// point (xyz) used when computing surface curvature
      class CurvePoint3D {
         public:
            float xyz[3];
      };


      /// determine the curvatures
      void determineCurvature(const int num,
                        const std::vector<CurvePoint3D>& dc,
                        const std::vector<CurvePoint3D>& dn,
                        float& gauss,
                        float& mean,
                        float& kmax,
                        float& kmin);
      
      /// project to a plane?
      void projectToPlane(const float projected[3],
                          const float basis[2][3],
                          float xyz[3]);
      
      /// project something?
      void projection(const float vector[3],
                      const float normal[3],
                      float xyz[3]);
                      
      /// surface for curvature computation
      BrainModelSurface* surface;
      
      /// surface shape file for storage of curvature
      SurfaceShapeFile* shapeFile;
      
      /// column for mean curvature
      int meanCurvatureColumn;
      
      /// column for gaussian curvature
      int gaussianCurvatureColumn;
      
      /// name for mean curvature column
      QString meanCurvatureName;
      
      /// name for gaussian curvature column
      QString gaussianCurvatureName;
      
      /// k-max (k1, first principal curvature) column
      int kMaxColumn;
      
      /// k-min (k2, second principal curvature) column
      int kMinColumn;
      
      /// compute the principal curvatures
      bool computePrincipalCurvatures;
};

#endif // __BRAIN_MODEL_SURFACE_CURVATURE_H__


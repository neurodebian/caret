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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceCurvature.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyHelper.h"
#include "vtkMath.h"

/**
 * Constructor.
 */
BrainModelSurfaceCurvature::BrainModelSurfaceCurvature(
                                          BrainSet* bs,
                                          BrainModelSurface* surfaceIn,
                                          SurfaceShapeFile* shapeFileIn,
                                          const int meanCurvatureColumnIn,
                                          const int gaussianCurvatureColumnIn,
                                          const QString& meanCurvatureNameIn,
                                          const QString& gaussianCurvatureNameIn)
   : BrainModelAlgorithm(bs)
{
   surface                 = surfaceIn;
   shapeFile               = shapeFileIn;
   meanCurvatureColumn     = meanCurvatureColumnIn;
   gaussianCurvatureColumn = gaussianCurvatureColumnIn;
   meanCurvatureName = meanCurvatureNameIn;
   gaussianCurvatureName = gaussianCurvatureNameIn;
}

/**
 * Destructor.
 */
BrainModelSurfaceCurvature::~BrainModelSurfaceCurvature()
{
}

/**
 * Execute the algorithm.
 */
void 
BrainModelSurfaceCurvature::execute() throw (BrainModelAlgorithmException)
{
   //
   // Update the surface's normals
   //
   surface->computeNormals();
   
   //
   // Get the topology helper for the surface
   //
   const TopologyFile* tf = surface->getTopologyFile();
   const TopologyHelper* th = tf->getTopologyHelper(false, true, true);
   
   const CoordinateFile* cf = surface->getCoordinateFile();

   //
   // Create surface shape file columns if needed
   //
   if (meanCurvatureColumn == CURVATURE_COLUMN_CREATE_NEW) {
      if (shapeFile->getNumberOfColumns() == 0) {
         shapeFile->setNumberOfNodesAndColumns(surface->getNumberOfNodes(), 1);
      }
      else {
         shapeFile->addColumns(1);
      }
      meanCurvatureColumn = shapeFile->getNumberOfColumns() - 1;
   }
   if (gaussianCurvatureColumn == CURVATURE_COLUMN_CREATE_NEW) {
      if (shapeFile->getNumberOfColumns() == 0) {
         shapeFile->setNumberOfNodesAndColumns(surface->getNumberOfNodes(), 1);
      }
      else {
         shapeFile->addColumns(1);
      }
      gaussianCurvatureColumn = shapeFile->getNumberOfColumns() - 1;
   }
   
   //
   // Set column names
   //
   if (meanCurvatureColumn >= 0) {
      shapeFile->setColumnName(meanCurvatureColumn, meanCurvatureName);
      shapeFile->setColumnColorMappingMinMax(meanCurvatureColumn, -1.5, 1.5);
   }
   if (gaussianCurvatureColumn >= 0) {
      shapeFile->setColumnName(gaussianCurvatureColumn, gaussianCurvatureName);
      shapeFile->setColumnColorMappingMinMax(gaussianCurvatureColumn, -1.5, 1.5);
   }
   
   //
   // Compute curvature for each node
   //
   const int numNodes = surface->getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {         
      float gauss = 0.0;
      float mean  = 0.0;

      std::vector<int> neighbors;
      th->getNodeNeighbors(i, neighbors);
      const int numNeighbors = static_cast<int>(neighbors.size());
      if (numNeighbors > 0) {
         //
         // Position and normal for node
         //
         const float* nodeCoord  = cf->getCoordinate(i);
         const float* nodeNormal = surface->getNormal(i);
         
         //
         // Find differences between node's and its neighbors' coordinates and normals
         //
         std::vector<CurvePoint3D> deltaNormal(numNeighbors);
         std::vector<CurvePoint3D> deltaCoord(numNeighbors);            
         for (int j = 0; j < numNeighbors; j++) {
            const int neighbor = neighbors[j];
            const float* neighCoord  = cf->getCoordinate(neighbor);
            const float* neighNormal = surface->getNormal(neighbor);
            for (int k = 0; k < 3; k++) {
               deltaNormal[j].xyz[k] = neighNormal[k] - nodeNormal[k];
               deltaCoord[j].xyz[k]  = neighCoord[k] - nodeCoord[k];
            }
         }
         
         //
         // Compute basis functions
         //
         float basis[2][3];
         float t1[3];
         projection(deltaCoord[0].xyz, nodeNormal, basis[0]);
         MathUtilities::normalize(basis[0]);
         for (int k = 0; k < 3; k++) {
            t1[k] = -basis[0][k];
         }
         MathUtilities::normalizedCrossProduct(t1, nodeNormal, basis[1]);
         MathUtilities::normalize(basis[1]);
         
         std::vector<CurvePoint3D> dn(numNeighbors);
         std::vector<CurvePoint3D> dc(numNeighbors);
         for (int j = 0; j < numNeighbors; j++) {
            float projected[3];
            projection(deltaNormal[j].xyz, nodeNormal, projected);
            projectToPlane(projected, basis, dn[j].xyz);
            
            projection(deltaCoord[j].xyz, nodeNormal, projected);
            projectToPlane(projected, basis, dc[j].xyz);
         }
         
         leastSquares(numNeighbors, dc, dn, gauss, mean);
      }
      
      if (meanCurvatureColumn != CURVATURE_COLUMN_DO_NOT_GENERATE) {
         shapeFile->setValue(i, meanCurvatureColumn, mean);
      }
      if (gaussianCurvatureColumn != CURVATURE_COLUMN_DO_NOT_GENERATE) {
         shapeFile->setValue(i, gaussianCurvatureColumn, gauss);
      }
   }
   
   //
   // Create comment info about surface used in distortion
   //
   QString commentInfo("Surface: ");
   commentInfo.append(FileUtilities::basename(surface->getFileName()));
   if (meanCurvatureColumn != CURVATURE_COLUMN_DO_NOT_GENERATE) {
      shapeFile->setColumnComment(meanCurvatureColumn, commentInfo);
   }
   if (gaussianCurvatureColumn != CURVATURE_COLUMN_DO_NOT_GENERATE) {
      shapeFile->setColumnComment(gaussianCurvatureColumn, commentInfo);
   }
}

/**
 *
 */
void
BrainModelSurfaceCurvature::projectToPlane(const float projected[3],
                                           const float basis[2][3],
                                           float xyz[3])
{
   xyz[0] = MathUtilities::dotProduct(projected, basis[0]);
   xyz[1] = MathUtilities::dotProduct(projected, basis[1]);
   xyz[2] = 0.0;
}

/**
 *
 */
void
BrainModelSurfaceCurvature::projection(const float vector[3],
                                       const float normal[3],
                                       float xyz[3])
{
   const float t2 = MathUtilities::dotProduct(vector, normal);
   for (int i = 0; i < 3; i++) {
      xyz[i] = vector[i] - (t2 * normal[i]);
   }
}

/**
 *
 */
void
BrainModelSurfaceCurvature::leastSquares(const int num,
                                         const std::vector<CurvePoint3D>& dc,
                                         const std::vector<CurvePoint3D>& dn,
                                         float& gauss,
                                         float& mean)
{
   float sum1 = 0.0;
   float sum2 = 0.0;
   float sum3 = 0.0;
   float wx   = 0.0;
   float wy   = 0.0;
   float wxy  = 0.0;
   
   for (int i = 0; i < num; i++) {
      sum1 += (dc[i].xyz[0] * dn[i].xyz[0]);
      sum2 += ((dc[i].xyz[0] * dn[i].xyz[1]) + (dc[i].xyz[1] * dn[i].xyz[0]));
      sum3 += (dc[i].xyz[1] * dn[i].xyz[1]);
      wx   += (dc[i].xyz[0] * dc[i].xyz[0]);
      wy   += (dc[i].xyz[1] * dc[i].xyz[1]);
      wxy  += (dc[i].xyz[0] * dc[i].xyz[1]);
   }
   
   const float wx2  = wx  * wx;
   const float wy2  = wy  * wy;
   const float wxy2 = wxy * wxy;
   
   float a = 0.0;
   float b = 0.0;
   float c = 0.0;
   const float t1 = (wx + wy) * (-wxy2 + wx * wy);
   if (t1 > 0.0) {
      a = (sum3 * wxy2 - sum2 * wxy * wy +
           sum1 * (-wxy2 + wx * wy + wy2)) / t1;
      b = (-(sum3 * wx * wxy) + sum2 * wx * wy - sum1 * wxy * wy) / t1;
      c = (-(sum2 * wx * wxy) + sum1 * wxy2 +
           sum3 * (wx2 - wxy2 + wx * wy)) / t1;
   }
   
   const float trC = a + c;
   const float detC = a * c - b * b;
   const float temp = trC * trC - 4 * detC;
   float k1 = 0.0;
   float k2 = 0.0;
   if (temp > 0.0) {
      const float deltaPlus = std::sqrt(temp);
      const float deltaMinus = -deltaPlus;
      k1 = (trC + deltaPlus)  / 2.0;
      k2 = (trC + deltaMinus) / 2.0;
   }
   
   gauss = k1 * k2;
   mean  = (k1 + k2) / 2.0;
}


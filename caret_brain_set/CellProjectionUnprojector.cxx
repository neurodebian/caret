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



#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <algorithm>
#include <cmath>

#include "CellProjectionUnprojector.h"
#include "MathUtilities.h"

#include "vtkMath.h"
#include "vtkTriangle.h"

//#include "gaussj.h"
//#include "nrutil.h"

/**
 * Constructor.
 */
CellProjectionUnprojector::CellProjectionUnprojector()
{
}

/**
 * Destructor.
 */
CellProjectionUnprojector::~CellProjectionUnprojector()
{
}

/**
 * Unproject an outside triangle projections.
 */
void 
CellProjectionUnprojector::unprojectOutsideTriangle(CellProjection& cp, const CoordinateFile& cf)
{
   const int is = 0;
   const int js = 1;
   
   float v[3], v_t1[3];
   MathUtilities::subtractVectors(cp.vertexFiducial[js], cp.vertexFiducial[is], v);
   MathUtilities::subtractVectors(cp.posFiducial, cp.vertexFiducial[is], v_t1);
   
   float s_t2 = MathUtilities::dotProduct(v, v);
   float s_t3 = MathUtilities::dotProduct(v_t1, v);
   float QR[3];
   for (int j = 0; j < 3; j++) {
      QR[j] = cp.vertexFiducial[is][j] + ((s_t3/s_t2) * v[j]);
   }
   
   const int pis = cp.vertex[0];
   const int pjs = cp.vertex[1];
   
   const float* posPIS = cf.getCoordinate(pis);
   const float* posPJS = cf.getCoordinate(pjs);
   MathUtilities::subtractVectors(posPJS, posPIS, v);
   
   float QS[3];
   
   if ((cp.fracRI <= 1.0) && (cp.fracRJ <= 1.0)) {
      for (int j = 0; j < 3; j++) {
         QS[j] = posPIS[j] + cp.fracRI * v[j];
      }
   }
   else if ((cp.fracRI > 1.0) && (cp.fracRI > cp.fracRJ)) {
      MathUtilities::subtractVectors(QR, cp.vertexFiducial[js], v_t1);
      s_t2 = MathUtilities::vectorLength(v_t1);
      
      MathUtilities::subtractVectors(posPJS, posPIS, v);
      s_t3 = MathUtilities::vectorLength(v);
      for (int j = 0; j < 3; j++) {
         QS[j] = posPJS[j] + s_t2 * (v[j]/s_t3);
      }
   }
   else if ((cp.fracRJ > 1.0) && (cp.fracRJ > cp.fracRI)) {
      MathUtilities::subtractVectors(QR, cp.vertexFiducial[is], v_t1);
      s_t2 = MathUtilities::vectorLength(v_t1);
      
      MathUtilities::subtractVectors(posPIS, posPJS, v);
      s_t3 = MathUtilities::vectorLength(v);
      for (int j = 0; j < 3; j++) {
         QS[j] = posPIS[j] + s_t2 * (v[j]/s_t3);
      }
   }
   else {
     // printf("CellProjectionFile: Unrecognized case for fracRI and fracRJ: "
     //        "%.2f %.2f\n", fracRI, fracRJ);
      return;
   }
   
   if ((cp.triVertices[0][0] < 0) || (cp.triVertices[1][0] < 0)) {
      xyz[0] = 0.0;
      xyz[1] = 0.0;
      xyz[2] = 0.0;
      return;
   }
   
   float normalB[3]; 
   MathUtilities::computeNormal((float*)cf.getCoordinate(cp.triVertices[1][0]),
                  (float*)cf.getCoordinate(cp.triVertices[1][1]),
                  (float*)cf.getCoordinate(cp.triVertices[1][2]),
                  normalB);
   
   float normalA[3];
   MathUtilities::computeNormal((float*)cf.getCoordinate(cp.triVertices[0][0]),
                  (float*)cf.getCoordinate(cp.triVertices[0][1]),
                  (float*)cf.getCoordinate(cp.triVertices[0][2]),
                  normalA);
             
   s_t2 = MathUtilities::dotProduct(normalA, normalB);
   s_t2 = std::min(s_t2, (float)1.0); // limit to <= 1.0
   float phiS = std::acos(s_t2);
   
   float thetaS = 0.0;
   if (cp.phiR > 0.0) {
      thetaS = (cp.thetaR/cp.phiR) * phiS;
   }
   else {
      thetaS = 0.5 * phiS;
   }
   
   MathUtilities::subtractVectors(posPJS, posPIS, v);
   MathUtilities::normalize(v);
   
   float v_t3[3];
   MathUtilities::crossProduct(normalA, v, v_t3);
   
   float projection[3] = { 0.0, 0.0, 0.0 };
   computeProjectionPoint(cp, projection);

   MathUtilities::subtractVectors(projection, QR, v_t1);
   MathUtilities::normalize(v_t1);
   
   MathUtilities::subtractVectors(cp.vertexFiducial[js], cp.vertexFiducial[is], v);
   MathUtilities::normalize(v);
   
   float normalA_3D[3];
   MathUtilities::computeNormal(cp.triFiducial[0][0],
               cp.triFiducial[0][1],
               cp.triFiducial[0][2],
               normalA_3D);
   float v_t2[3];
   MathUtilities::crossProduct(normalA_3D, v, v_t2);
   s_t3 = MathUtilities::dotProduct(v_t1, v_t2);
   
   float TS[3];
   for (int k = 0; k < 3; k++) {
      TS[k] = QS[k] + (s_t3 * (cp.dR * std::sin(thetaS)) * v_t2[k]);
   }
   
   MathUtilities::subtractVectors(cp.posFiducial, projection, v);
   MathUtilities::normalize(v);
   
   s_t3 = MathUtilities::dotProduct(normalA_3D, v);
   
   for (int i = 0; i < 3; i++) {
      xyz[i] = TS[i] + (cp.dR * s_t3 * std::cos(thetaS)) * normalA[i];
   }
}

/**
 * Compute the projection point.
 */
void
CellProjectionUnprojector::computeProjectionPoint(CellProjection& cp, float projection[3])
{
   float v[3], w[3], tnormal[3];
   MathUtilities::subtractVectors(cp.triFiducial[0][1],
                    cp.triFiducial[0][0], v);
   MathUtilities::subtractVectors(cp.triFiducial[0][1],
                    cp.triFiducial[0][2], w);
   MathUtilities::crossProduct(w, v, tnormal);
   
   float a[3][3];
   for (int k = 0; k < 3; k++) {
      a[0][k] = v[k];
      a[1][k] = w[k];
      a[2][k] = tnormal[k];
   }
   
   float b[3];
   b[0] = MathUtilities::dotProduct(v, cp.posFiducial);
   b[1] = MathUtilities::dotProduct(w, cp.posFiducial);
   b[2] = MathUtilities::dotProduct(tnormal, cp.triFiducial[0][2]);
   
   
   vtkMath::LinearSolve3x3(a, b, projection);
   
/*
   float **ta = matrix(1, 3, 1, 3);
   float **tb = matrix(1, 3, 1, 1);
   for (int i = 0; i < 3; i++) {
      tb[i+1][1] = b[i];
      for (int j = 0; j < 3; j++) {
         ta[i+1][j+1] = a[i][j];
      }
   }
   
   gaussj(ta, 3, tb, 1);
   
   for (int m = 0; m < 3; m++) {
      projection[m] = tb[1][m+1];
   }
   
   free_matrix(ta, 1, 3, 1, 1);
   free_matrix(tb, 1, 3, 1, 1);
*/   
}

/**
 * Unproject an inside triangle projection.
 */
void 
CellProjectionUnprojector::unprojectInsideTriangle(CellProjection& cp, const CoordinateFile& cf)
{
   const float* v1 = cf.getCoordinate(cp.closestTileVertices[0]);
   const float* v2 = cf.getCoordinate(cp.closestTileVertices[1]);
   const float* v3 = cf.getCoordinate(cp.closestTileVertices[2]);

   float t1[3], t2[3], t3[3];
   for (int i = 0; i < 3; i++) {
      t1[i] = cp.closestTileAreas[0] * v3[i];
      t2[i] = cp.closestTileAreas[1] * v1[i];
      t3[i] = cp.closestTileAreas[2] * v2[i];
   }
   
   const float area = cp.closestTileAreas[0] + cp.closestTileAreas[1] 
                    + cp.closestTileAreas[2];
                    
   float projection[3] = { 0.0, 0.0, 0.0 };
   if (area != 0) {
      for (int i = 0; i < 3; i++) {
         projection[i] = (t1[i] + t2[i] + t3[i]) / area;
      }                                     
   }
   
   //
   // Note: that does caret4 style clockwise orientation
   //
   float tileNormal[3];
   MathUtilities::computeNormal((float*)v3, (float*)v2, (float*)v1, tileNormal);
   
   for (int j = 0; j < 3; j++) {
      if (cp.signedDistanceAboveSurface != 0.0) {
         xyz[j] = projection[j] + tileNormal[j] * cp.signedDistanceAboveSurface;
      }
      else {
         xyz[j] = projection[j] + cp.cdistance[j];
      }
   }
}
 
/**
 * Unproject a cell projection.
 */
void
CellProjectionUnprojector::unprojectCellProjection(CellProjection& cp, 
                                                   const CoordinateFile& cf)
{
   switch(cp.projectionType) {
      case CellProjection::PROJECTION_TYPE_INSIDE_TRIANGLE:
         unprojectInsideTriangle(cp, cf);
         break;
      case CellProjection::PROJECTION_TYPE_OUTSIDE_TRIANGLE:
         unprojectOutsideTriangle(cp, cf);
         break;
      case CellProjection::PROJECTION_TYPE_UNKNOWN:
         break;
   }
}

/**
 * Unproject a cell (or foci) projection file.
 */
void 
CellProjectionUnprojector::unprojectCellProjections(CellProjectionFile& cpf, BrainModelSurface* s,
                                                    CellFile& cf,
                                                    const int startIndex)
{
   if (s == NULL) {
      return;
   }
   
   const int numProj = cpf.getNumberOfCellProjections();
   
   //
   // Transfer study info from the cell proj file to the cell file but
   // only transfer those study info that need to be transferred.
   //      
   std::vector<int> cellProjFileToCellFileStudyInfoIndex;
   int numCellProjStudyInfo = cpf.getNumberOfStudyInfo();
   if (numCellProjStudyInfo > 0) {
      //
      // cellProjFileToCellFileStudyInfoIndex converts cell proj file study info indices
      // into cell file study info indices
      //
      cellProjFileToCellFileStudyInfoIndex.resize(numCellProjStudyInfo, -1);
      for (int i = startIndex; i < numProj; i++) {
         //
         // Get the cell proj file study info index
         //
         const int cellProjStudyInfoIndex = cpf.getCellProjection(i)->getStudyNumber();
         if (cellProjStudyInfoIndex >= 0) {
            //
            // If this cell proj study info index does not yet convert to a cell study info index 
            //
            if (cellProjFileToCellFileStudyInfoIndex[cellProjStudyInfoIndex] < 0) {
               //
               // See if the study is already in the cell file
               //
               const int cellStudyInfoIndex =
                     cf.getStudyInfoIndexFromValue((*cpf.getStudyInfo(cellProjStudyInfoIndex)));
               if (cellStudyInfoIndex >= 0) {
                  //
                  // convert cell proj file study index into existing cell file study index
                  //
                  cellProjFileToCellFileStudyInfoIndex[cellProjStudyInfoIndex] = cellStudyInfoIndex;
               }
               else {
                  //
                  // add study info to cell file and convert cell file proj study info index
                  // into cell file study info index
                  //
                  cellProjFileToCellFileStudyInfoIndex[cellProjStudyInfoIndex] = 
                     cf.addStudyInfo((*cpf.getStudyInfo(cellProjStudyInfoIndex)));
               }
            }
         }
      }
   }
      
   
   for (int i = startIndex; i < numProj; i++) {
      CellProjection* cp = cpf.getCellProjection(i);
      
      xyz[0] = 0.0;
      xyz[1] = 0.0;
      xyz[2] = 0.0;
      
      if (cp->projectionType != CellProjection::PROJECTION_TYPE_UNKNOWN) {
         unprojectCellProjection(*cp, *(s->getCoordinateFile()));
         if (s->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
            if (cp->projectionType == CellProjection::PROJECTION_TYPE_OUTSIDE_TRIANGLE) {
               xyz[0] = cp->posFiducial[0];
               xyz[1] = cp->posFiducial[1];
               xyz[2] = cp->posFiducial[2];
            }
            switch(cp->structure.getType()) {
               case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                  if (xyz[0] > 0.0) xyz[0] = -xyz[0];
                  break;
               case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                  if (xyz[0] < 0.0) xyz[0] = -xyz[0];
                  break;
               case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                  break;
               case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
               case Structure::STRUCTURE_TYPE_SUBCORTICAL:
               case Structure::STRUCTURE_TYPE_ALL:
               case Structure::STRUCTURE_TYPE_INVALID:
                  break;
               case Structure::STRUCTURE_TYPE_CEREBELLUM:
                  break;
               case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
                  break;
               case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
                  break;
               case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
                  break;
               case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
                  break;
            }
         }
      }
      
      int studyNumber = cp->getStudyNumber();
      if ((studyNumber >= 0) && 
          (studyNumber < static_cast<int>(cellProjFileToCellFileStudyInfoIndex.size()))) {
         studyNumber = cellProjFileToCellFileStudyInfoIndex[studyNumber];
      }
      else {
         studyNumber = -1;
      }

      CellData cd(cp->getName(), xyz[0], xyz[1], xyz[2], 
                  cp->getSectionNumber(), cp->getClassName(), studyNumber,
                  cp->getColorIndex());
      cd.copyCellBaseData(*cp);
      cd.setClassName(cp->getClassName());
      cd.setStudyNumber(studyNumber);
      cd.setXYZ(xyz);
      cd.setSignedDistanceAboveSurface(cp->getSignedDistanceAboveSurface());
      
      cd.setCellStructure(cp->structure.getType());
/*
      switch(cp->hemisphere.getType()) {
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
            cd.setCellHemisphere(CellData::CELL_IN_LEFT_HEMISPHERE);
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
            cd.setCellHemisphere(CellData::CELL_IN_RIGHT_HEMISPHERE);
            break;
         case Hemisphere::HEMISPHERE_UNKNOWN:
            cd.setCellHemisphere(CellData::CELL_IN_UNKNOWN_HEMISPHERE);
            break;
      }
*/
      cf.addCell(cd);
   }
}


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
#include <iostream>

#include <QApplication>
#include <QProgressDialog>

#include "vtkLine.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkTriangle.h"

#include "BrainModelSurface.h"
#include "BrainModelSurfacePointProjector.h"
#include "CellFileProjector.h"
#include "CellProjectionFile.h"
#include "MathUtilities.h"
#include "TopologyFile.h"

/**
 * Constructor.  
 */
CellFileProjector::CellFileProjector(const BrainModelSurface* bmsIn) :
   coordinateFile(bmsIn->getCoordinateFile()),   // initialize const members
   topologyFile(bmsIn->getTopologyFile()),
   bms(bmsIn)
{
   pointProjector = new BrainModelSurfacePointProjector(bmsIn,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_OTHER,
                           false);
   hemisphere = bmsIn->getStructure();

   //
   // Check for fiducial surface
   //
   fiducialSurfaceFlag = 
      ((bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_RAW) ||
       (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL));
}

/**
 * Destructor.
 */
CellFileProjector::~CellFileProjector()
{
}

/**
 * Project cell projection file using the BrainModelSurface passed to
 * the constructor.  If "progressDialogParent" is not NULL, a progress dialog will be 
 * displayed while cells are being projected.
 */
void
CellFileProjector::projectFile(CellProjectionFile* cpf,
                                   const int startIndex,
                                   const PROJECTION_TYPE projectionType,
                                   const float projectOntoSurfaceAboveDistance,
                                   const bool projectOntoSurface,
                                   QWidget* progressDialogParent)
{
   const int numCells = cpf->getNumberOfCellProjections();
   if (numCells > 0) {
   
      //
      // Create a progress dialog 
      //
      QProgressDialog* progressDialog = NULL;
      if (progressDialogParent != NULL) {
         progressDialog = new QProgressDialog("Projecting",
                                                0,
                                                0,
                                                numCells + 1,
                                                progressDialogParent);
         progressDialog->setWindowTitle("Projecting");
         progressDialog->setValue(0);
         progressDialog->show();
      }

      //
      // loop through the cells
      //
      for (int i = startIndex; i < numCells; i++) {
         CellProjection* cp = cpf->getCellProjection(i);
         projectCell(*cp, projectionType, 
                     projectOntoSurfaceAboveDistance, projectOntoSurface);
         if (progressDialog != NULL) {
            progressDialog->setValue(i + 1);
         }
      }
      
      //
      // Remove the progress dialog
      //
      if (progressDialog != NULL) {
         progressDialog->setValue(numCells + 1);
         delete progressDialog;
      }
   }
}

/**
 * update a cell projection and return number of cell projection
 */
/*
int 
CellFileProjector::updateCellProjection(const CellFile* cf, const int cellNumber, 
                                        CellProjectionFile* cpf,
                                        const PROJECTION_TYPE projectionType)
{
   const CellData* cd = cf->getCell(cellNumber);
   int cellProjUniqueID = cd->getCellProjectionID();
   int index = -1;
   for (int i = 0; i < cpf->getNumberOfCellProjections(); i++) {
      if (cpf->getCellProjection(i)->getUniqueID() == cellProjUniqueID) {
         index = i;
         break;
      }
   }
   if (index >= 0) {
      CellProjection* cp = cpf->getCellProjection(index);
      if (projectCell(*cd, *cp, projectionType, 0.0, false)) {
         const int cellStudyIndex = cd->getStudyNumber();
         int cellProjStudyInfoNumber = -1;
         if (cellStudyIndex >= 0) {
            int cellProjStudyInfoNumber = cpf->getStudyInfoFromValue((*cf->getStudyInfo(cellStudyIndex)));
            if (cellProjStudyInfoNumber < 0) {
               cellProjStudyInfoNumber = cpf->addStudyInfo((*cf->getStudyInfo(cellStudyIndex)));
            }
         }
         cp->setStudyNumber(cellProjStudyInfoNumber);
         return index;
      }
   }
   
   return -1;
}
*/

/**
 * Project a cell.  Returns true if the cell was projected.
 */
void 
CellFileProjector::projectCell(CellProjection& cp, 
                               const PROJECTION_TYPE projectionType,
                               const float projectOntoSurfaceAboveDistance,
                               const bool projectOntoSurface) 
{
   //
   // Get the position of the cell
   //
   float xyz[3];
   const bool validXYZ = cp.getProjectedPosition(coordinateFile,
                                                 topologyFile,
                                                 fiducialSurfaceFlag,
                                                 false,
                                                 false,
                                                 xyz);
   
   //
   // Default to not projected
   //
   cp.projectionType = CellProjection::PROJECTION_TYPE_UNKNOWN;

   //
   // If position is unknown, cannot project cell
   //
   if (validXYZ == false) {
      return;
   }
   
   //
   // Set the fiducial position of the cell projection
   //
   cp.posFiducial[0] = xyz[0];
   cp.posFiducial[1] = xyz[1];
   cp.posFiducial[2] = xyz[2];
   
   //
   // save original hemisphere
   //
   if (xyz[0] < 0.0) {
      cp.structure.setType(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
   }
   else {
      cp.structure.setType(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
   }
   
   //
   // Modify position if forcing onto hemisphere
   //
   switch (projectionType) {
      case PROJECTION_TYPE_FLIP_TO_MATCH_HEMISPHERE:
         if (hemisphere == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
            if (xyz[0] > 0.0) xyz[0] = -xyz[0];
         }
         else if (hemisphere == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
            if (xyz[0] < 0.0) xyz[0] = -xyz[0];
         }
         break;
      case PROJECTION_TYPE_HEMISPHERE_ONLY:
         if (hemisphere == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
            if (xyz[0] > 0.0) return;
         }
         else if (hemisphere == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
            if (xyz[0] < 0.0) return;
         }
         break;
      case PROJECTION_TYPE_ALL:
         break;
   }
   
   
   int nearestTileNumber, tileNodes[3];
   float barycentricAreas[3], distance, distanceComponents[3], signedDistance;
   
   //
   // Project to the surface
   //
   const int result = pointProjector->projectBarycentricNearestTile(xyz, 
                                                                    nearestTileNumber,
                                                                    tileNodes,
                                                                    barycentricAreas,
                                                                    signedDistance,
                                                                    distance,
                                                                    distanceComponents);
   
   //
   // Signed distance to surface
   //
   cp.setSignedDistanceAboveSurface(signedDistance);

   //
   // Did cell project inside a tile ?
   //
   if (result > 0) {
       cp.projectionType = CellProjection::PROJECTION_TYPE_INSIDE_TRIANGLE;
       
       cp.closestTileVertices[0] = tileNodes[2];
       cp.closestTileVertices[1] = tileNodes[1];
       cp.closestTileVertices[2] = tileNodes[0];
       cp.closestTileAreas[0] = barycentricAreas[1];
       cp.closestTileAreas[1] = barycentricAreas[0];
       cp.closestTileAreas[2] = barycentricAreas[2];
       cp.cdistance[0] = distanceComponents[0];
       cp.cdistance[1] = distanceComponents[1];
       cp.cdistance[2] = distanceComponents[2];
       if (projectOntoSurface) {
          float tileNormal[3];
          MathUtilities::computeNormal((float*)coordinateFile->getCoordinate(tileNodes[0]),
                                     (float*)coordinateFile->getCoordinate(tileNodes[1]),
                                     (float*)coordinateFile->getCoordinate(tileNodes[2]),
                                     tileNormal);
          cp.cdistance[0] = tileNormal[0] * projectOntoSurfaceAboveDistance;
          cp.cdistance[1] = tileNormal[1] * projectOntoSurfaceAboveDistance;
          cp.cdistance[2] = tileNormal[2] * projectOntoSurfaceAboveDistance;
          cp.signedDistanceAboveSurface = projectOntoSurfaceAboveDistance;
       }
   }
   //
   // Is cell supposed to be projected onto the surface ?
   //
   else if (projectOntoSurface) {
      const int nearestNode = pointProjector->projectToNearestNode(xyz);
      if (nearestNode >= 0) {
         cp.projectionType = CellProjection::PROJECTION_TYPE_INSIDE_TRIANGLE;
       
         cp.closestTileVertices[0] = nearestNode;
         cp.closestTileVertices[1] = nearestNode;
         cp.closestTileVertices[2] = nearestNode;
         cp.closestTileAreas[0] = 1.0;
         cp.closestTileAreas[1] = 1.0;
         cp.closestTileAreas[2] = 1.0;
         const float* normal = bms->getNormal(nearestNode);
         cp.cdistance[0] = normal[0] * projectOntoSurfaceAboveDistance;
         cp.cdistance[1] = normal[1] * projectOntoSurfaceAboveDistance;
         cp.cdistance[2] = normal[2] * projectOntoSurfaceAboveDistance;
         cp.signedDistanceAboveSurface = projectOntoSurfaceAboveDistance;
      }
      else {
         return;
      }
   }
   //
   //  Did cell project outside a tile ?
   //
   else if (result < 0) {
       cp.projectionType = CellProjection::PROJECTION_TYPE_OUTSIDE_TRIANGLE;
       
       //
       // Vertices of closest tile
       //
       int v1, v2, v3;
       topologyFile->getTile(nearestTileNumber, v1, v2, v3);
       const float* p1 = coordinateFile->getCoordinate(v1);
       const float* p2 = coordinateFile->getCoordinate(v2);
       const float* p3 = coordinateFile->getCoordinate(v3);
       
       //
       // cell projected to plane of nearest tile
       //
       float planeNormal[3];
       MathUtilities::computeNormal(p1, p2, p3, planeNormal);
       float cellProjectedToPlane[3];
#ifdef HAVE_VTK5
       double xyzd[3] = { xyz[0], xyz[1], xyz[2] };
       double p1d[3] = { p1[0], p1[1], p1[2] };
       double dn[3] = { planeNormal[0], planeNormal[1], planeNormal[2] };
       double cellProjD[3];
       vtkPlane::ProjectPoint(xyzd, p1d, dn, cellProjD);
       cellProjectedToPlane[0] = cellProjD[0];
       cellProjectedToPlane[1] = cellProjD[1];
       cellProjectedToPlane[2] = cellProjD[2];
#else // HAVE_VTK5
       vtkPlane::ProjectPoint((float*)xyz, (float*)p1, planeNormal, cellProjectedToPlane);
#endif // HAVE_VTK5         
       //
       // Find vertices of edge closest to cell projected to plane
       //
       int closestVertices[2];
       /* VTK DistanceToLine appears to be incorrect.
       const float dist1 = vtkLine::DistanceToLine((float*)cellProjectedToPlane, (float*)p1, (float*)p2);
       const float dist2 = vtkLine::DistanceToLine((float*)cellProjectedToPlane, (float*)p2, (float*)p3);
       const float dist3 = vtkLine::DistanceToLine((float*)cellProjectedToPlane, (float*)p3, (float*)p1);
       */
       const float dist1 = MathUtilities::distancePointToLine3D(xyz, p1, p2);
       const float dist2 = MathUtilities::distancePointToLine3D(xyz, p2, p3);
       const float dist3 = MathUtilities::distancePointToLine3D(xyz, p3, p1);
       if ((dist1 < dist2) && (dist1 < dist3)) {
          closestVertices[0] = v1;
          closestVertices[1] = v2;
       }
       else if ((dist2 < dist1) && (dist2 < dist3)) {
          closestVertices[0] = v2;
          closestVertices[1] = v3;
       }
       else {
          closestVertices[0] = v3;
          closestVertices[1] = v1;
       }
       
       /*
       if (vtkMath::Distance2BetweenPoints((float*)xyz, 
                                    (float*)coordinateFile->getCoordinate(closestVertices[1])) <
           vtkMath::Distance2BetweenPoints((float*)xyz, 
                                    (float*)coordinateFile->getCoordinate(closestVertices[0]))) {
          const int temp = closestVertices[0];
          closestVertices[0] = closestVertices[1];
          closestVertices[1] = temp;
       }
       */

       const int iR = closestVertices[0];
       const int jR = closestVertices[1];
       const int triA = nearestTileNumber;
       const int triB = topologyFile->getTileWithVertices(iR, jR, triA);
   
       float coordJR[3];
       coordinateFile->getCoordinate(jR, coordJR);
       float coordIR[3];
       coordinateFile->getCoordinate(iR, coordIR);
         
       float normalA[3];
       if (triA >= 0){
          int v1, v2, v3;
          topologyFile->getTile(triA, v1, v2, v3);
          const float* p1 = coordinateFile->getCoordinate(v1);
          const float* p2 = coordinateFile->getCoordinate(v2);
          const float* p3 = coordinateFile->getCoordinate(v3);
          MathUtilities::computeNormal((float*)p1, (float*)p2, (float*)p3, normalA);
       }
       float normalB[3];
       if (triB >= 0){
          int v1, v2, v3;
          topologyFile->getTile(triB, v1, v2, v3);
          const float* p1 = coordinateFile->getCoordinate(v1);
          const float* p2 = coordinateFile->getCoordinate(v2);
          const float* p3 = coordinateFile->getCoordinate(v3);
          MathUtilities::computeNormal((float*)p1, (float*)p2, (float*)p3, normalB);
       }
       else {
         cp.dR = std::sqrt(vtkMath::Distance2BetweenPoints(cellProjectedToPlane, xyz));
         
         float v[3];
         MathUtilities::subtractVectors(coordJR, coordIR, v);
         
         float t1[3];
         MathUtilities::subtractVectors(xyz, coordIR, t1);
         float t2 = MathUtilities::dotProduct(v, v);
         float t3 = MathUtilities::dotProduct(t1, v);
         
         float QR[3];
         for (int j = 0; j < 3; j++)
            QR[j] = coordIR[j] + ((t3/t2) * v[j]);
         MathUtilities::subtractVectors(coordJR, coordIR, v);
         t2 = MathUtilities::vectorLength(v);
         MathUtilities::subtractVectors(QR, coordIR, t1);
         t3 = MathUtilities::vectorLength(t1);
         if (t2 > 0.0)
            cp.fracRI = t3/t2; 
         else
            cp.fracRI = 0.0;
   
         MathUtilities::subtractVectors(coordIR, coordJR, v);
         t2 = MathUtilities::vectorLength(v);
         MathUtilities::subtractVectors(QR, coordJR, t1);
         t3 = MathUtilities::vectorLength(t1);
         if (t2 > 0.0)
            cp.fracRJ = t3/t2; 
         else
            cp.fracRI = 0.0;
   
         if (cp.fracRI > 1.0){ 
            for (int j = 0; j < 3; j++)
               QR[j] = coordJR[j];
         } 
         if (cp.fracRJ > 1.0){ 
            for (int j = 0; j < 3; j++)
               QR [j] = coordIR[j];
         }
         MathUtilities::subtractVectors(xyz, cellProjectedToPlane, t1);
         t2 = MathUtilities::vectorLength(t1);
         if (t2 > 0){
            for (int j = 0; j < 3; j++)
               t1[j] = t1[j]/t2;
         }
         t3 = MathUtilities::dotProduct(t1, normalA);
         for (int j = 0; j < 3; j++)
            xyz[j] = QR[j] + (cp.dR * t3 * normalA[j]); 
      }
   
      float v[3];
      MathUtilities::subtractVectors(coordJR, coordIR, v);
      float t1[3];
      MathUtilities::subtractVectors(xyz, coordIR, t1);
      float t2 = MathUtilities::dotProduct(v, v);
      float t3 = MathUtilities::dotProduct(t1, v);
      float QR[3];
      for (int j = 0; j < 3; j++)
         QR[j] = coordIR[j] + ((t3/t2) * v[j]);
   
      if ((triA >=0) && (triB >= 0)){
         t2 = MathUtilities::dotProduct(normalA, normalB);
         t2 = std::min(t2, (float)1.0);   // imit to 1.0
         cp.phiR = std::acos(t2);
      }
      else
         cp.phiR = 0.0;
   
      MathUtilities::subtractVectors(xyz, QR, t1);
      t2 = MathUtilities::vectorLength(t1);
      if (t2 > 0.0){
         for (int j = 0; j < 3; j++)
            t1[j] = t1[j]/t2;
      }
      t3 = MathUtilities::dotProduct(normalA, t1); 
      // HAD 11.11.96 Added for H53 where t3 = 0
      if (t3 > 0)
         cp.thetaR = std::acos(t3 * (t3/fabs (t3)));
      else
         cp.thetaR = 0.0;
   
      MathUtilities::subtractVectors(coordJR, coordIR, v);
      t2 = MathUtilities::vectorLength(v);
      MathUtilities::subtractVectors (QR, coordIR, t1);
      t3 = MathUtilities::vectorLength(t1);
      if (t2 > 0.0)
         cp.fracRI = t3/t2; 
      else
         cp.fracRI = 0.0;
   
      MathUtilities::subtractVectors(coordIR, coordJR, v);
      t2 = MathUtilities::vectorLength(v);
      MathUtilities::subtractVectors(QR, coordJR, t1);
      t3 = MathUtilities::vectorLength(t1);
      if (t2 > 0.0)
         cp.fracRJ = t3/t2; 
      else
         cp.fracRJ = 0.0;
   
      cp.dR = std::sqrt(vtkMath::Distance2BetweenPoints(QR, xyz));
      topologyFile->getTile(triA, cp.triVertices[0]);
      topologyFile->getTile(triB, cp.triVertices[1]);
      
      std::swap(cp.triVertices[0][0], cp.triVertices[0][2]);
      coordinateFile->getCoordinate(cp.triVertices[0][0], cp.triFiducial[0][0]);
      coordinateFile->getCoordinate(cp.triVertices[0][1], cp.triFiducial[0][1]);
      coordinateFile->getCoordinate(cp.triVertices[0][2], cp.triFiducial[0][2]);
      if (triB >= 0) {
         std::swap(cp.triVertices[1][0], cp.triVertices[1][2]);
         coordinateFile->getCoordinate(cp.triVertices[1][0], cp.triFiducial[1][0]);
         coordinateFile->getCoordinate(cp.triVertices[1][1], cp.triFiducial[1][1]);
         coordinateFile->getCoordinate(cp.triVertices[1][2], cp.triFiducial[1][2]);
      }

      cp.vertexFiducial[0][0] = coordIR[0];
      cp.vertexFiducial[0][1] = coordIR[1];
      cp.vertexFiducial[0][2] = coordIR[2];
      cp.vertexFiducial[1][0] = coordJR[0];
      cp.vertexFiducial[1][1] = coordJR[1];
      cp.vertexFiducial[1][2] = coordJR[2];
      
      cp.vertex[0] = iR;
      cp.vertex[1] = jR;

   }
}


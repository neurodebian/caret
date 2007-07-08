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
#include <QDir>
#include <QProgressDialog>

#include "vtkLine.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkTriangle.h"

#include "BrainModelSurface.h"
#include "BrainModelSurfacePointProjector.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "FociFileToPalsProjector.h"
#include "FociProjectionFile.h"
#include "MapFmriAtlasSpecFileInfo.h"
#include "MathUtilities.h"
#include "StudyMetaDataFile.h"
#include "TopologyFile.h"

/**
 * Constructor.  
 */
FociFileToPalsProjector::FociFileToPalsProjector(BrainSet* brainSetIn,
                                                 FociProjectionFile* fociProjectionFileIn,
                                                 const int firstFocusIndexIn,
                                                 const float projectOntoSurfaceAboveDistanceIn,
                                                 const bool projectOntoSurfaceFlagIn)
 : BrainModelAlgorithm(brainSetIn)
{
   fociProjectionFile = fociProjectionFileIn;
   firstFocusIndex = firstFocusIndexIn;
   projectOntoSurfaceAboveDistance = projectOntoSurfaceAboveDistanceIn;
   projectOntoSurfaceFlag = projectOntoSurfaceFlagIn;
   atlasesDirectoryLoadedFlag = false;
}

/**
 * Destructor.
 */
FociFileToPalsProjector::~FociFileToPalsProjector()
{
   for (unsigned int i = 0; i < pointProjectors.size(); i++) {
      delete pointProjectors[i];
   }
   pointProjectors.clear();
}

/**
 * set the index of the first focus to project.
 */
void 
FociFileToPalsProjector::setFirstFocusIndex(const int firstFocusIndexIn)
{
   firstFocusIndex = firstFocusIndexIn;
}
      
/**
 * Project foci
 */
void
FociFileToPalsProjector::execute() throw (BrainModelAlgorithmException)
{
   try {
      if (atlasesDirectoryLoadedFlag == false) {      
         atlasesDirectoryLoadedFlag = true;

         std::vector<MapFmriAtlasSpecFileInfo> atlasList;
         MapFmriAtlasSpecFileInfo::getAtlases(brainSet, atlasList);
                                              
         if (atlasList.empty()) {
            throw FileException("Unable to find PALS atlases");
         }
         
         for (unsigned int i = 0; i < atlasList.size(); i++) {
            const MapFmriAtlasSpecFileInfo& mfi = atlasList[i];
            if (mfi.getDataValid() &&
                (mfi.getAverageCoordinateFile().isEmpty() == false) &&
                (mfi.getTopologyFile().isEmpty() == false)) {
               availableAtlases.push_back(mfi);
            }
         }
      }
      
      if (fociProjectionFile == NULL) {
         throw BrainModelAlgorithmException("Invalid file for projecting.");
      }
      const int numFoci = fociProjectionFile->getNumberOfCellProjections();
      if ((numFoci <= 0) ||
          (firstFocusIndex >= numFoci)) {
         throw BrainModelAlgorithmException("No foci for projecting.");
      }
      
      const StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
      
      //
      // Find out the spaces that are needed for projecting the foci
      //
      QString fociWithoutSpaces;
      int spacesCount = 0;
      QString fociWithoutOriginalXYZ;
      int noXYZCount = 0;
      std::vector<PointProjector> projectorsNeeded;
      std::vector<QString> focusSpace;
      if (numFoci > 0) {
         focusSpace.resize(numFoci, "");
      }
      for (int i = firstFocusIndex; i < numFoci; i++) {
         CellProjection* cp = fociProjectionFile->getCellProjection(i);
         const float* xyz = cp->getXYZ();
         const StudyMetaDataLink smdl = cp->getStudyMetaDataLink();
         const int studyMetaDataIndex = smdf->getStudyIndexFromLink(smdl);
         if ((studyMetaDataIndex >= 0) &&
             (studyMetaDataIndex < smdf->getNumberOfStudyMetaData())) {
            const StudyMetaData* smd = smdf->getStudyMetaData(studyMetaDataIndex);
            focusSpace[i] = smd->getStereotaxicSpace();
         }
         else {
            const int studyNumber = cp->getStudyNumber();
            if ((studyNumber >= 0) && 
                (studyNumber < fociProjectionFile->getNumberOfStudyInfo())) {
               const CellStudyInfo* csi = fociProjectionFile->getStudyInfo(studyNumber);
               focusSpace[i] = csi->getStereotaxicSpace();
            }
         }
         if (focusSpace[i].isEmpty()) {
            fociWithoutSpaces += (QString::number(i) + " ");
            spacesCount++;
            if (spacesCount >= 10) {
               fociWithoutSpaces += "\n";
               spacesCount = 0;
            }
         }
         else {
            Structure::STRUCTURE_TYPE structure = Structure::STRUCTURE_TYPE_CORTEX_LEFT;
            if (xyz[0] > 0) {
               structure = Structure::STRUCTURE_TYPE_CORTEX_RIGHT;
            }
            cp->setCellStructure(structure);
         
            //
            // See if a new point projector is needed
            //
            PointProjector pp(focusSpace[i], structure, NULL, NULL, NULL);
            if (std::find(projectorsNeeded.begin(), projectorsNeeded.end(), pp) ==
                projectorsNeeded.end()) {
               projectorsNeeded.push_back(pp);
            }
         }
         
         if ((xyz[0] == 0.0) && (xyz[1] == 0.0) && (xyz[2] == 0.0)) {
            fociWithoutOriginalXYZ += (QString::number(i) + " ");
            noXYZCount++;
            if (noXYZCount >= 10) {
               fociWithoutOriginalXYZ += "\n";
               noXYZCount = 0;
            }
         }
      }
      
      //
      // Cannot project if spaces are missing
      //
      QString errorMessage;
      if (fociWithoutSpaces.isEmpty() == false) {
         errorMessage.append("These foci do not have associated sterotaxic spaces: \n");
         errorMessage.append(fociWithoutSpaces);
         errorMessage.append("\n");
      }
      if (fociWithoutOriginalXYZ.isEmpty() == false) {
         errorMessage.append("These foci are missing their original stereotaxic coordinates.\n"
                             "They were probably loaded from an older version of the foci\n"
                             "projection file that did not store the original foci coordinates.\n"
                             "Indices of these foci: \n");
         errorMessage.append(fociWithoutOriginalXYZ);
         errorMessage.append("\n");
      }
      if (errorMessage.isEmpty() == false) {
         throw BrainModelAlgorithmException(errorMessage);
      }
      
      //
      // Number of foci to project
      //
      const int totalSteps = numFoci - firstFocusIndex + 1;
      
      //
      // Create the progress dialog
      //
      createProgressDialog("Map Foci to PALS Atlas",
                           totalSteps,
                           "mapFociToPalsProgressDialog");
                          
      //
      // load any needed point projectors
      //
      loadNeededPointProjectors(projectorsNeeded);
      
      //
      // Loop through the foci and project them
      //
      for (int i = firstFocusIndex; i < numFoci; i++) {
         updateProgressDialog("Mapping Focus " + QString::number(i), i + 1);
         
         CellProjection* cp = fociProjectionFile->getCellProjection(i);
/*
         QString space;
         const int studyNumber = cp->getStudyNumber();
         if ((studyNumber >= 0) && 
             (studyNumber < fociProjectionFile->getNumberOfStudyInfo())) {
            const CellStudyInfo* csi = fociProjectionFile->getStudyInfo(studyNumber);
            space = csi->getStereotaxicSpace();
            spaceNameConvert(space);
         }
*/
         spaceNameConvert(focusSpace[i]);
         Structure::STRUCTURE_TYPE structure = cp->getCellStructure();
         
         if (DebugControl::getDebugOn()) {
            std::cout << "Mapping " << i << " focus named " << cp->getName().toAscii().constData()
                      << " with space " << focusSpace[i].toAscii().constData() 
                      << " structure " <<  Structure::convertTypeToString(structure).toAscii().constData() 
                      << std::endl;
         }
         
         //
         // Find the point projector
         //
         PointProjector *pp = NULL;
         for (unsigned int j = 0; j < pointProjectors.size(); j++) {
            if ((pointProjectors[j]->spaceName == focusSpace[i]) &&
                (pointProjectors[j]->structureType == structure)) {
               pp = pointProjectors[j];
               break;
            }
         }
         if (pp == NULL) {
            QString msg("Unable to find point projector for focus ");
            msg.append(QString::number(i));
            msg.append(" space ");
            msg.append(focusSpace[i]);
            throw BrainModelAlgorithmException(msg);
         }
         
         //
         // Project the focus
         //
         projectFocus(*cp,
                      pp->bms,
                      pp->pointProjector);
      }
   }
   catch (BrainModelAlgorithmException& e) {
      removeProgressDialog();
      throw BrainModelAlgorithmException(e);
   }
       
   removeProgressDialog();
}

/**
 * load the needed point projectors.
 */
void 
FociFileToPalsProjector::loadNeededPointProjectors(const std::vector<PointProjector>& projectorsNeeded) throw (BrainModelAlgorithmException)
{
   for (unsigned int i = 0; i < projectorsNeeded.size(); i++) {
      //
      // Does this point projector NOT exist ?
      //
      bool ppFound = false;
      for (unsigned int m = 0; m < pointProjectors.size(); m++) {
         if (*(pointProjectors[m]) == projectorsNeeded[i]) {
            ppFound = true;
         }
      }
      if (ppFound == false) {
         //
         // Find matching atlas
         //
         MapFmriAtlasSpecFileInfo* matchingAtlas = NULL;
         for (unsigned int j = 0; j < availableAtlases.size(); j++) {
            const Structure::STRUCTURE_TYPE atlasStructure = 
                        Structure::convertStringToType(availableAtlases[j].getStructure());
            const QString atlasSpace = availableAtlases[j].getSpace();
            
            if ((atlasStructure == projectorsNeeded[i].structureType) &&
                (atlasSpace == projectorsNeeded[i].spaceName)) {
               matchingAtlas = &availableAtlases[j];
               break;
            }
         }
         
         //
         // No atlas available ??
         //
         if (matchingAtlas == NULL) {
            QString msg("Unable to find atlas in space ");
            msg.append(projectorsNeeded[i].originalSpaceName);
            throw BrainModelAlgorithmException(msg);
         }
         
         updateProgressDialog("Loading atlas for " +
                              matchingAtlas->getSpace() +
                              " " +
                              matchingAtlas->getStructure());
                               
         //
         // Save current directory
         //
         const QString savedDirectory = QDir::currentPath();

         //
         // Set to atlas directory
         //
         QDir::setCurrent(matchingAtlas->getSpecFilePath());
         
         //
         // Create surface for brain model
         //
         BrainSet* bs = new BrainSet(matchingAtlas->getTopologyFile(),
                                     matchingAtlas->getAverageCoordinateFile());
         if (bs->getNumberOfBrainModels() == 0) {
            QString msg("Error loading atlas ");
            msg.append(matchingAtlas->getDescription());
            delete bs;
            throw BrainModelAlgorithmException(msg);
         }
         BrainModelSurface* bms = bs->getBrainModelSurface(0);
         if (bms == NULL) {
            QString msg("Error loading atlas (no brain model surface) ");
            msg.append(matchingAtlas->getDescription());
            delete bs;
            throw BrainModelAlgorithmException(msg);
         }
         TopologyFile* tf = bms->getTopologyFile();
         if (tf == NULL) {
            QString msg("Error loading atlas (no topology file) ");
            msg.append(matchingAtlas->getDescription());
            delete bs;
            throw BrainModelAlgorithmException(msg);
         }
         
         //
         // Create a point projector
         //
         BrainModelSurfacePointProjector* pointProjector = 
            new BrainModelSurfacePointProjector(bms,
                                                BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_OTHER,
                                                false);
         //
         // Load the brain model and point projectors for atlas
         //
         PointProjector* pp = new PointProjector(projectorsNeeded[i].spaceName,
                                                projectorsNeeded[i].structureType,
                                                pointProjector,
                                                bs,
                                                bms);
         pointProjectors.push_back(pp);
         
         //
         // Restore directory
         //
         QDir::setCurrent(savedDirectory);
         
         if (DebugControl::getDebugOn()) {
            std::cout << "Loaded PALS " << pp->spaceName.toAscii().constData()
                      << " " << Structure::convertTypeToString(pp->structureType).toAscii().constData() 
                      << std::endl;
         }
      }
   }
}

/**
 * convert space names to identical spaces.
 */
void 
FociFileToPalsProjector::spaceNameConvert(QString& spaceName)
{
   if (spaceName.startsWith("711-2") ||
       spaceName.startsWith("7112")) {
      spaceName = "711-2C";
   }
   else if (spaceName.startsWith("T88")) {
      spaceName = "AFNI";
   }
}

/**
 * Project a focus.  Returns true if the focus was projected.
 */
void 
FociFileToPalsProjector::projectFocus(CellProjection& cp, 
                                      BrainModelSurface* bms,
                                      BrainModelSurfacePointProjector* pointProjector) 
{
   CoordinateFile* coordinateFile = bms->getCoordinateFile();
   TopologyFile* topologyFile = bms->getTopologyFile();
   
   //
   // Get the position of the focus
   //
   float xyz[3];
   cp.getXYZ(xyz);
   
   //
   // Default to not projected
   //
   cp.projectionType = CellProjection::PROJECTION_TYPE_UNKNOWN;

   //
   // If position is unknown, cannot project focus
   //
   if ((xyz[0] == 0.0) && (xyz[1] == 0.0) && (xyz[2] == 0.0)) {
      return;
   }
   
   //
   // Set the fiducial position of the focus projection
   //
   cp.posFiducial[0] = xyz[0];
   cp.posFiducial[1] = xyz[1];
   cp.posFiducial[2] = xyz[2];
   
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
   // Did focus project inside a tile ?
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
       if (projectOntoSurfaceFlag) {
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
   // Is focus supposed to be projected onto the surface ?
   //
   else if (projectOntoSurfaceFlag) {
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
   //  Did focus project outside a tile ?
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
       // focus projected to plane of nearest tile
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

//==============================================================================

/**
 * constructor.
 */
FociFileToPalsProjector::PointProjector::PointProjector(const QString& spaceNameIn,
               const Structure::STRUCTURE_TYPE structureTypeIn,
               BrainModelSurfacePointProjector* pointProjectorIn,
               BrainSet* bsIn,
               BrainModelSurface* bmsIn) 
{
   originalSpaceName = spaceNameIn;
   spaceName = spaceNameIn;
   FociFileToPalsProjector::spaceNameConvert(spaceName);
   structureType = structureTypeIn;
   pointProjector = pointProjectorIn;
   bs  = bsIn;
   bms = bmsIn;
}

/**
 * destructor.
 */
FociFileToPalsProjector::PointProjector::~PointProjector() 
{
   if (pointProjector != NULL) {
      delete pointProjector;
      pointProjector = NULL;
   }
   if (bms != NULL) {
      delete bms;
      bms = NULL;
   }
}

/**
 * equality operator.
 */
bool 
FociFileToPalsProjector::PointProjector::operator==(const FociFileToPalsProjector::PointProjector& pp) const 
{
   if ((spaceName == pp.spaceName) &&
       (structureType == pp.structureType)) {
      return true;
   }
   return false;
}

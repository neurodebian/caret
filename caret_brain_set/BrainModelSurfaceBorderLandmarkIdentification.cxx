
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
#include <iostream>
#include <limits>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BorderProjectionUnprojector.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderLandmarkIdentification.h"
#include "BrainModelSurfaceCurvature.h"
#include "BrainModelSurfaceClusterToBorderConverter.h"
#include "BrainModelSurfaceFindExtremum.h"
#include "BrainModelSurfacePaintSulcalIdentification.h"
#include "BrainModelSurfaceROICreateBorderUsingGeodesic.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelVolumeSureFitSegmentation.h"
#include "BrainSet.h"
#include "CellFileProjector.h"
#include "DebugControl.h"
#include "FociColorFile.h"
#include "FociProjectionFile.h"
#include "MetricFile.h"
#include "NodeRegionOfInterestFile.h"
#include "PaintFile.h"
#include "SurfaceShapeFile.h"
#include "VocabularyFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelSurfaceBorderLandmarkIdentification::BrainModelSurfaceBorderLandmarkIdentification(
                                   BrainSet* bs,
                                   const StereotaxicSpace& stereotaxicSpaceIn,
                                   const VolumeFile* anatomicalVolumeFileIn,
                                   const BrainModelSurface* fiducialSurfaceIn,
                                   const BrainModelSurface* inflatedSurfaceIn,
                                   const BrainModelSurface* veryInflatedSurfaceIn,
                                   const BrainModelSurface* ellipsoidSurfaceIn,
                                   const SurfaceShapeFile* depthSurfaceShapeFileIn,
                                   const int depthSurfaceShapeFileColumnNumberIn,
                                   PaintFile* paintFileInOut,
                                   const int paintFileGeographyColumnNumberIn,
                                   AreaColorFile* areaColorFileInOut,
                                   BorderProjectionFile* borderProjectionFileInOut,
                                   BorderColorFile* borderColorFileInOut,
                                   VocabularyFile* vocabularyFileInOut,
                                   const int operationSelectionMaskIn)
   : BrainModelAlgorithm(bs),
     stereotaxicSpace(stereotaxicSpaceIn),
     anatomicalVolumeFile(anatomicalVolumeFileIn),
     inputFiducialSurface(fiducialSurfaceIn),
     inflatedSurface(inflatedSurfaceIn),
     veryInflatedSurface(veryInflatedSurfaceIn),
     ellipsoidSurface(ellipsoidSurfaceIn),
     depthSurfaceShapeFile(depthSurfaceShapeFileIn),
     depthSurfaceShapeFileColumnNumber(depthSurfaceShapeFileColumnNumberIn),
     paintFile(paintFileInOut),
     paintFileGeographyColumnNumber(paintFileGeographyColumnNumberIn),
     areaColorFile(areaColorFileInOut),
     borderProjectionFile(borderProjectionFileInOut),
     borderColorFile(borderColorFileInOut),
     vocabularyFile(vocabularyFileInOut),
     operationSelectionMask(operationSelectionMaskIn)
{
   fiducialSurface = NULL;
   curvatureFiducialMeanColumnNumber = -1;
   curvatureInflatedMeanColumnNumber = -1;
   curvatureShapeFile = NULL;
   fociProjectionFile = NULL;
   fociColorFile = NULL;
   metricFile = NULL;
   paintFileSulcusIdColumnName = "";
   paintFileSulcusIdColumnNumber = -1;

   calcarineAnteriorNodeNumber = -1;
   calcarinePosteriorExtremeNodeNumber = -1;
   ccGenuBeginningNodeNumber = -1;
   ccSpleniumLimitNodeNumber = -1;
   cesMedialNodeNumber = -1;
   medialWallStartNodeNumber = -1;
   sfInferiorBranchBeginNodeNumber = -1;
   sfVentralFrontalNodeNumber = -1;
   temporalPoleNodeNumber = -1;
   
   calcarineSulcusLandmarkName = "LANDMARK.CalcarineSulcus";
}
                                   
/**
 * destructor.
 */
BrainModelSurfaceBorderLandmarkIdentification::~BrainModelSurfaceBorderLandmarkIdentification()
{
   if (fiducialSurface != NULL) {
      brainSet->deleteBrainModel(fiducialSurface);
      fiducialSurface = NULL;
   }
   if (curvatureShapeFile != NULL) {
      delete curvatureShapeFile;
      curvatureShapeFile = NULL;
   }
   if (metricFile != NULL) {
      delete metricFile;
      metricFile = NULL;
   }
   if (fociProjectionFile != NULL) {
      delete fociProjectionFile;
      fociProjectionFile = NULL;
   }
   if (fociColorFile != NULL) {
      delete fociColorFile;
      fociColorFile = NULL;
   }
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::execute() throw (BrainModelAlgorithmException)
{
   if (anatomicalVolumeFile == NULL) {
      throw BrainModelAlgorithmException("The anatomical volume is invalid.");
   }
   if (inputFiducialSurface == NULL) {
      throw BrainModelAlgorithmException("The fiducial surface is invalid.");
   }
   if (inflatedSurface == NULL) {
      throw BrainModelAlgorithmException("The inflated surface is invalid.");
   }
   if (veryInflatedSurface == NULL) {
      throw BrainModelAlgorithmException("The very inflated is invalid.");
   }
   if (depthSurfaceShapeFile == NULL) {
      throw BrainModelAlgorithmException("The surface shape file is invalid.");
   }
   if (paintFile == NULL) {
      throw BrainModelAlgorithmException("The paint file is invalid.");
   }
   if (areaColorFile == NULL) {
      throw BrainModelAlgorithmException("The area color file is invalid.");
   }
   if (borderProjectionFile == NULL) {
      throw BrainModelAlgorithmException("The border projection file is invalid.");
   }
   if (borderColorFile == NULL) {
      throw BrainModelAlgorithmException("The border color file is invalid.");
   }
   if ((depthSurfaceShapeFileColumnNumber < 0) ||
       (depthSurfaceShapeFileColumnNumber >= depthSurfaceShapeFile->getNumberOfColumns())) {
      throw BrainModelAlgorithmException("Surface Shape File Depth column is invalid.");
   }
   
   //
   // Clear out debug files
   //
   BorderProjectionFile bpf;
   borderDebugFileName = bpf.makeDefaultFileName("DebugBorders");
   QFile::remove(borderDebugFileName);
   FociProjectionFile fpf;
   fociProjectionDebugFileName = fpf.makeDefaultFileName("DebugFoci");
   QFile::remove(fociProjectionDebugFileName);
   FociColorFile fcf;
   fociColorDebugFileName = fcf.makeDefaultFileName("DebugFoci");
   QFile::remove(fociColorDebugFileName);
   
   //
   // Verify left or right only
   //
   leftHemisphereFlag = false;
   surfaceStructure = inputFiducialSurface->getStructure().getType();
   switch (surfaceStructure) {
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT:  
         leftHemisphereFlag = true;
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
         leftHemisphereFlag = false;
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
      case Structure::STRUCTURE_TYPE_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_INVALID:
         throw ("Structure must be left or right hemisphere.");
         break;
   }
   
   //
   // Scale fiducial to be approximately 711-2* size 
   //
   createAndScaleFiducialSurface();
   
   //
   // Create data files
   //
   fociProjectionFile = new FociProjectionFile;
   fociColorFile = new FociColorFile;
   
   //
   // Identify the sulci
   //
   paintSulcalIdentification();
   
   //
   // Generate curvature on Fiducial and Inflated Surfaces
   //
   generateSurfaceCurvatures();
      
   //
   // Identify the landmarks
   //
   QStringList errorMessageStringList;
   if (errorMessageStringList.isEmpty()) {
      try {
         identifyCentralSulcus();
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessageStringList += "Central Sulcus Landmark Error\n";
         errorMessageStringList += e.whatQString();
      }
   }
   
   if (errorMessageStringList.isEmpty()) {
      try {
         identifySuperiorTemporalGyrus();  // Depends upon CeS
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessageStringList += "Superior Temporal Gyrus Landmark Error\n";
         errorMessageStringList += e.whatQString();
      }
   }
   
   if (errorMessageStringList.isEmpty()) {
      try {
         identifySylvianFissure();         // Depends upon STG
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessageStringList += "Sylvian Fissure Landmark Error\n";
         errorMessageStringList += e.whatQString();
      }
   }
   
   if (errorMessageStringList.isEmpty()) {
      try {
         identifyCalcarineSulcus();
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessageStringList += "Calcarine Sulcus Landmark Error\n";
         errorMessageStringList += e.whatQString();
      }
   }
   
   if (errorMessageStringList.isEmpty()) {
      try {
         identifyMedialWall();
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessageStringList += "Medial Wall Landmark Error\n";
         errorMessageStringList += e.whatQString();
      }
   }
   
   if (errorMessageStringList.isEmpty()) {
      try {
         extendCalcarineSulcusToMedialWall();
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessageStringList += "Calcarine Landmark Extension to Medial Wall Error\n";
         errorMessageStringList += e.whatQString();
      }
   }
   
   if (errorMessageStringList.isEmpty()) {
      try {
         identifyCuts();
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessageStringList += "Cut Generation Error\n";
         errorMessageStringList += e.whatQString();
      }
   }
   
   if (errorMessageStringList.isEmpty()) {
      try {
         createMedialWallDorsalAndVentralLandmarks();
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessageStringList += "Create Medial Wall Dorsal and Ventral Segments Error\n";
         errorMessageStringList += e.whatQString();
      }
   }
   
   //
   // Project the foci
   //   
   projectFoci();
   
   if (errorMessageStringList.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessageStringList.join("\n"));
   }
}      

/**
 * create a fiducial surface that is approximately scaled to 711-2* space.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::createAndScaleFiducialSurface() throw (BrainModelAlgorithmException)
{
   //
   // Make a copy of the input surface so that it may be scaled
   //
   fiducialSurface = new BrainModelSurface(*inputFiducialSurface);
   brainSet->addBrainModel(fiducialSurface);
   
   float leftScale[3], rightScale[3];
   if (getScalingForStereotaxicSpace(stereotaxicSpace,
                                     leftScale,
                                     rightScale) == false) {
      throw BrainModelAlgorithmException(
         "Stereotaxic space "
         + stereotaxicSpace.getName()
         + " not supported for border landmark identification.");
   }
   
   TransformationMatrix tm;
   if (leftHemisphereFlag) {
      tm.scale(leftScale);
   }
   else {
      tm.scale(rightScale);
   }
   fiducialSurface->applyTransformationMatrix(tm);

   if (DebugControl::getDebugOn()) {
      try {
         CoordinateFile* cf = fiducialSurface->getCoordinateFile();
         cf->writeFile(cf->makeDefaultFileName("LandmarkFiducialScaled"));
      }
      catch (FileException&) {
         std::cout << "WARNING: Unable to write LandmarkFiducialScaled surface." << std::endl;
      }
   }
}

/**
 * get supported stereotaxic spaces.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::getSupportedStereotaxicSpaces(
                                std::vector<StereotaxicSpace>& spacesOut)
{
   spacesOut.clear();
   
   std::vector<StereotaxicSpace> allSpaces;
   StereotaxicSpace::getAllStereotaxicSpaces(allSpaces);
   
   const int numSpaces = static_cast<int>(allSpaces.size());
   for (int i = 0; i < numSpaces; i++) {
      float dummy[3];
      if (getScalingForStereotaxicSpace(allSpaces[i], dummy, dummy)) {
         spacesOut.push_back(allSpaces[i]);
      }
   }
}
      
/**
 * get scaling for stereotaxic space (returns true if supported space).
 */
bool 
BrainModelSurfaceBorderLandmarkIdentification::getScalingForStereotaxicSpace(
                                                     const StereotaxicSpace& space,
                                                     float scalingOutLeft[3],
                                                     float scalingOutRight[3]) 
{
   scalingOutLeft[0] = 1.0;
   scalingOutLeft[1] = 1.0;
   scalingOutLeft[2] = 1.0;
   scalingOutRight[0] = 1.0;
   scalingOutRight[1] = 1.0;
   scalingOutRight[2] = 1.0;
   
   //
   // Algorithm was developed using 711-2* which is these values
   //
   const float standardLeftMaxXYZ[3] = {
      63.90,
      62.17,
      73.99
   };
   const float standardRightMaxXYZ[3] = {
      64.72,
      63.09,
      73.64
   };
   
   float spaceLeftMaxXYZ[3] = { 0.0, 0.0, 0.0 };
   float spaceRightMaxXYZ[3] = { 0.0, 0.0, 0.0 };
   bool spaceValid = false;
   
   switch(space.getSpace()) {
      case StereotaxicSpace::SPACE_UNKNOWN:
         break;
      case StereotaxicSpace::SPACE_OTHER:
         break;
      case StereotaxicSpace::SPACE_AFNI_TALAIRACH:
         spaceLeftMaxXYZ[0] = 62.17;
         spaceLeftMaxXYZ[1] = 65.15;
         spaceLeftMaxXYZ[2] = 69.70;
         spaceRightMaxXYZ[0] = 63.92;
         spaceRightMaxXYZ[1] = 66.08;
         spaceRightMaxXYZ[2] = 69.86;
         spaceValid = true;
         break;
      case StereotaxicSpace::SPACE_FLIRT:
         spaceLeftMaxXYZ[0] = 64.71;
         spaceLeftMaxXYZ[1] = 68.34;
         spaceLeftMaxXYZ[2] = 77.20;
         spaceRightMaxXYZ[0] = 68.18;
         spaceRightMaxXYZ[1] = 69.42;
         spaceRightMaxXYZ[2] = 76.80;
         spaceValid = true;
         break;
      case StereotaxicSpace::SPACE_FLIRT_222:
         break;
      case StereotaxicSpace::SPACE_MACAQUE_F6:
         break;
      case StereotaxicSpace::SPACE_MACAQUE_F99:
         break;
      case StereotaxicSpace::SPACE_MRITOTAL:
         spaceLeftMaxXYZ[0] = 66.18;
         spaceLeftMaxXYZ[1] = 70.85;
         spaceLeftMaxXYZ[2] = 79.31;
         spaceRightMaxXYZ[0] = 69.96;
         spaceRightMaxXYZ[1] = 71.71;
         spaceRightMaxXYZ[2] = 78.80;
         spaceValid = true;
         break;
      case StereotaxicSpace::SPACE_SPM_99:
         spaceLeftMaxXYZ[0] = 67.72;
         spaceLeftMaxXYZ[1] = 70.82;
         spaceLeftMaxXYZ[2] = 82.64;
         spaceRightMaxXYZ[0] = 70.70;
         spaceRightMaxXYZ[1] = 71.21;
         spaceRightMaxXYZ[2] = 82.44;
         spaceValid = true;
         break;
      case StereotaxicSpace::SPACE_SPM:
         break;
      case StereotaxicSpace::SPACE_SPM_95:
         spaceLeftMaxXYZ[0] = 62.17;
         spaceLeftMaxXYZ[1] = 65.15;
         spaceLeftMaxXYZ[2] = 69.70;
         spaceRightMaxXYZ[0] = 63.92;
         spaceRightMaxXYZ[1] = 66.08;
         spaceRightMaxXYZ[2] = 69.86;
         spaceValid = true;
         break;
      case StereotaxicSpace::SPACE_SPM_96:
         spaceLeftMaxXYZ[0] = 66.18;
         spaceLeftMaxXYZ[1] = 70.85;
         spaceLeftMaxXYZ[2] = 79.31;
         spaceRightMaxXYZ[0] = 69.96;
         spaceRightMaxXYZ[1] = 71.71;
         spaceRightMaxXYZ[2] = 78.80;
         spaceValid = true;
         break;
      case StereotaxicSpace::SPACE_SPM_2:
         spaceLeftMaxXYZ[0] = 66.15;
         spaceLeftMaxXYZ[1] = 70.94;
         spaceLeftMaxXYZ[2] = 80.53;
         spaceRightMaxXYZ[0] = 70.56;
         spaceRightMaxXYZ[1] = 71.54;
         spaceRightMaxXYZ[2] = 80.20;
         spaceValid = true;
         break;
      case StereotaxicSpace::SPACE_SPM_5:
         break;
      case StereotaxicSpace::SPACE_T88:
         break;
      case StereotaxicSpace::SPACE_WU_7112B:
      case StereotaxicSpace::SPACE_WU_7112B_111:
      case StereotaxicSpace::SPACE_WU_7112B_222:
      case StereotaxicSpace::SPACE_WU_7112B_333:
      case StereotaxicSpace::SPACE_WU_7112C:
      case StereotaxicSpace::SPACE_WU_7112C_111:
      case StereotaxicSpace::SPACE_WU_7112C_222:
      case StereotaxicSpace::SPACE_WU_7112C_333:
      case StereotaxicSpace::SPACE_WU_7112O:
      case StereotaxicSpace::SPACE_WU_7112O_111:
      case StereotaxicSpace::SPACE_WU_7112O_222:
      case StereotaxicSpace::SPACE_WU_7112O_333:
      case StereotaxicSpace::SPACE_WU_7112Y:
      case StereotaxicSpace::SPACE_WU_7112Y_111:
      case StereotaxicSpace::SPACE_WU_7112Y_222:
      case StereotaxicSpace::SPACE_WU_7112Y_333:
         spaceLeftMaxXYZ[0] = 63.90;
         spaceLeftMaxXYZ[1] = 62.17;
         spaceLeftMaxXYZ[2] = 73.99;
         spaceRightMaxXYZ[0] = 64.72;
         spaceRightMaxXYZ[1] = 63.09;
         spaceRightMaxXYZ[2] = 73.64;
         spaceValid = true;
         break;
      case StereotaxicSpace::SPACE_NUMBER_OF_SPACES:
         break;
   }
   
   if (spaceValid) {
      for (int i = 0; i < 3; i++) {
         scalingOutLeft[i] = standardLeftMaxXYZ[i] / spaceLeftMaxXYZ[i];
         scalingOutRight[i] = standardRightMaxXYZ[i] / spaceRightMaxXYZ[i];
      }
   }
   
   return spaceValid;
}                                         
      
/**
 * generate surface curvatures.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::generateSurfaceCurvatures() throw (BrainModelAlgorithmException)
{
   //
   // Create the curvature shape file
   //
   curvatureShapeFile = new SurfaceShapeFile;
   
   //
   // Generate mean curvature on fiducial surface
   //
   BrainModelSurfaceCurvature fidCurve(brainSet,
                                       fiducialSurface,
                                       curvatureShapeFile,
                                       BrainModelSurfaceCurvature::CURVATURE_COLUMN_CREATE_NEW,
                                       BrainModelSurfaceCurvature::CURVATURE_COLUMN_DO_NOT_GENERATE,
                                       "Mean Curvature Fiducial",
                                       "");
   fidCurve.execute();
   curvatureFiducialMeanColumnNumber = curvatureShapeFile->getNumberOfColumns() - 1;
   
   //
   // Generate mean curvature on fiducial surface and smooth it
   //
   const QString smoothFidName("Smoothed Mean Curvature Fiducial");
   curvatureShapeFile->smoothAverageNeighbors(
                              curvatureFiducialMeanColumnNumber,
                              -1,
                              smoothFidName,
                              1.0,
                              5,
                              fiducialSurface->getTopologyFile());
   curvatureFiducialSmoothedMeanColumnNumber = 
      curvatureShapeFile->getColumnWithName(smoothFidName);
   
   //
   // Generate mean curvature on inflated surface
   //
   BrainModelSurfaceCurvature infCurve(brainSet,
                                       inflatedSurface,
                                       curvatureShapeFile,
                                       BrainModelSurfaceCurvature::CURVATURE_COLUMN_CREATE_NEW,
                                       BrainModelSurfaceCurvature::CURVATURE_COLUMN_DO_NOT_GENERATE,
                                       "Mean Curvature Inflated",
                                       "");
   infCurve.execute();
   curvatureInflatedMeanColumnNumber = curvatureShapeFile->getNumberOfColumns() - 1;
   
   if (DebugControl::getDebugOn()) {
      try {
         curvatureShapeFile->writeFile(
            curvatureShapeFile->makeDefaultFileName("LandmarkCurvatures"));
      }
      catch (FileException&) {
         std::cout << "WARNING: Unable to write landmark curvatures file." << std::endl;
      }
   }
}

/**
 * identify the cuts.
 */ 
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyCuts() throw (BrainModelAlgorithmException)
{
   //
   // Get the flatten medial wall
   // 
   const BorderProjection* medialWallFlattenBorderProjection =
      borderProjectionFile->getFirstBorderProjectionByName(getFlattenMedialWallBorderName());
   if (medialWallFlattenBorderProjection == NULL) {
      throw BrainModelAlgorithmException("ERROR: Unable to find border named \""
                                         + getFlattenMedialWallBorderName()
                                         + "\" for generating cuts.");
   }
   
   //
   // Get the center of gravity of the medial wall border
   //
   float medWallCOG[3];
   medialWallFlattenBorderProjection->getCenterOfGravity(
      fiducialSurface->getCoordinateFile(),
      medWallCOG);
   const int medialWallCogNodeNumber = 
      fiducialSurface->getCoordinateFile()->getCoordinateIndexClosestToPoint(
                                                                medWallCOG[0],
                                                                medWallCOG[1],
                                                                medWallCOG[2]);
      
   //
   // Add a color for the cuts
   //
   borderColorFile->addColor("FLATTEN.CUT",
                             75, 255, 75);
                             
   //
   // Generate the cuts
   //
   identifyCutCalcarine(medialWallCogNodeNumber);
   identifyCutCingulate(medialWallCogNodeNumber);
   identifyCutFrontal(medialWallCogNodeNumber);
   identifyCutSylvian(medialWallCogNodeNumber);
   identifyCutTemporal(medialWallCogNodeNumber);
}

/**
 * identify the cut calcarine.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyCutCalcarine(const int medialWallCogNodeNumber) throw (BrainModelAlgorithmException)
{
   flattenCutCalcarineName = getFlattenStandardCutsBorderNamePrefix() + "Calcarine";
   const BrainModelSurface* surface = veryInflatedSurface;
   const CoordinateFile* coordFile = surface->getCoordinateFile();
   
   //
   // Remove any existing cuts
   //
   borderProjectionFile->removeBordersWithName(flattenCutCalcarineName);

   //
   // Get the calcarine landmark
   //
   const BorderProjection* calcarineLandmark = 
      borderProjectionFile->getFirstBorderProjectionByName(calcarineSulcusLandmarkName);
   if (calcarineLandmark == NULL) {
      throw BrainModelAlgorithmException("ERROR: Unable to find border named \""
                                         + calcarineSulcusLandmarkName
                                         + "\" for generating cuts.");
   }
   BorderProjection calcarineCut(*calcarineLandmark);
   calcarineCut.setName(flattenCutCalcarineName);
   
   //
   // Get node number of first link in calcarine
   //
   const BorderProjectionLink* firstCalcarineLink =
      calcarineLandmark->getBorderProjectionLink(0);
   float firstCalcarineLinkXYZ[3];
   firstCalcarineLink->unprojectLink(coordFile,
                                     firstCalcarineLinkXYZ);
   const int firstCalcarineLinkNodeNumber =
      coordFile->getCoordinateIndexClosestToPoint(firstCalcarineLinkXYZ);    
                               
   //
   // Get node number of last link in calcarine
   //
   const BorderProjectionLink* lastCalcarineLink =
      calcarineLandmark->getBorderProjectionLink(calcarineLandmark->getNumberOfLinks() - 1);
   float lastCalcarineLinkXYZ[3];
   lastCalcarineLink->unprojectLink(coordFile,
                                    lastCalcarineLinkXYZ);
   const int lastCalcarineLinkNodeNumber =
      coordFile->getCoordinateIndexClosestToPoint(lastCalcarineLinkXYZ);    
                               

   //
   // Get node number that is 15mm lateral of the occipital pole
   //
   const float* occPoleXYZ = 
      inflatedSurface->getCoordinateFile()->getCoordinate(calcarinePosteriorExtremeNodeNumber);
   float occPole15LateralXYZ[3] = {
      occPoleXYZ[0],
      occPoleXYZ[1],
      occPoleXYZ[2]
   };
   occPole15LateralXYZ[0] = (leftHemisphereFlag ? -15.0 : 15.0);
   const int occPoleOffsetNodeNumber =
      inflatedSurface->getCoordinateFile()->getCoordinateIndexClosestToPoint(
                                                 occPole15LateralXYZ);  
   
   //                                              medialWallCogNodeNumber);

   //
   // Draw border from the node lateral to occiptial pole to the occipital pole
   //   
   const QString calcarineSegment0("CalcarineSegment0");
   drawBorderGeodesic(surface,
                      NULL,
                      calcarineSegment0,
                      occPoleOffsetNodeNumber,
                      calcarinePosteriorExtremeNodeNumber,
                      2.0);
                      
   //
   // Draw from occipital pole to first link in calcarine
   //
   const QString calcarineSegment1("CalcarineSegment1");
   drawBorderGeodesic(surface,
                      NULL,
                      calcarineSegment1,
                      calcarinePosteriorExtremeNodeNumber,
                      firstCalcarineLinkNodeNumber,
                      2.0);
   
   //
   // Draw from last link in calcarine to medial wall COG
   //
   const QString calcarineSegment2("CalcarineSegment2");
   drawBorderGeodesic(surface,
                      NULL,
                      calcarineSegment2,
                      lastCalcarineLinkNodeNumber,
                      medialWallCogNodeNumber,
                      2.0);

   //
   // Merge to create the calcarine cut
   //
   std::vector<QString> borderNames;
   borderNames.push_back(calcarineSegment0);
   borderNames.push_back(calcarineSegment1);
   borderNames.push_back(calcarineSulcusLandmarkName);
   borderNames.push_back(calcarineSegment2);
   mergeBorders(flattenCutCalcarineName,
                borderNames,
                false,
                false,
                surface,
                0,
                0);

   //
   // Remove segments
   //
   borderProjectionFile->removeBordersWithName(calcarineSegment0);
   borderProjectionFile->removeBordersWithName(calcarineSegment1);
   borderProjectionFile->removeBordersWithName(calcarineSegment2);

   //
   // Resample 
   //
   resampleBorder(fiducialSurface,
                  flattenCutCalcarineName,
                  2.0,
                  false);
}      

/**
 * identify the cut cingulate.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyCutCingulate(const int medialWallCogNodeNumber) throw (BrainModelAlgorithmException)
{
   const QString flattenCutCingulateName(getFlattenStandardCutsBorderNamePrefix() + "Cingulate");
   
   //
   // Remove any existing cuts
   //
   borderProjectionFile->removeBordersWithName(flattenCutCingulateName);
   
   //
   // Draw from medial wall cog to tip of central sulcus
   //
   drawBorderGeodesic(inflatedSurface,
                      NULL,
                      flattenCutCingulateName,
                      medialWallCogNodeNumber,
                      cesMedialNodeNumber,
                      2.0);
}

/**
 * identify the cut frontal.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyCutFrontal(const int medialWallCogNodeNumber) throw (BrainModelAlgorithmException)
{
   flattenCutFrontalName = getFlattenStandardCutsBorderNamePrefix() + "Frontal";
   
   //
   // Remove any existing cuts
   //
   borderProjectionFile->removeBordersWithName(flattenCutFrontalName);

   //
   // Default for middle point at approximate anterior of orbital sulcus
   //
   const float defaultMiddlePointXYZ[3] = {
      (leftHemisphereFlag ? -22.0 : 22.0),
      46.0
      -6.0
   };
   int middleNodeNumber =
      fiducialSurface->getCoordinateFile()->getCoordinateIndexClosestToPoint(
                                                            defaultMiddlePointXYZ);
                                                            
   //
   // Create an ROI of the orbital sulcus where cut should pass through
   //
   BrainModelSurfaceROINodeSelection oribitalSulcusROI(brainSet);
   QString errorMessage = oribitalSulcusROI.selectNodesWithPaint(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                              fiducialSurface,
                              paintFile,
                              paintFileSulcusIdColumnNumber,
                              "SUL.OrbS");
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   if (oribitalSulcusROI.getNumberOfNodesSelected() >= 0) {
      middleNodeNumber = oribitalSulcusROI.getNodeWithMaximumYCoordinate(fiducialSurface);
   }

   //
   // Default for endpoint
   //
   const float defaultEndPointXYZ[3] = {
      (leftHemisphereFlag ? -53.0: 53.0),
      21.0,
      19.0
   };
   int endNodeNumber =
      fiducialSurface->getCoordinateFile()->getCoordinateIndexClosestToPoint(
                                                            defaultEndPointXYZ);
                                                            
   //
   // Create an ROI of the inferior frontal sulcus where cut should end
   //
   BrainModelSurfaceROINodeSelection inferiorFrontalSulcusROI(brainSet);
   errorMessage = inferiorFrontalSulcusROI.selectNodesWithPaint(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                              fiducialSurface,
                              paintFile,
                              paintFileSulcusIdColumnNumber,
                              "SUL.IFS");
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   if (inferiorFrontalSulcusROI.getNumberOfNodesSelected() >= 0) {
      endNodeNumber = inferiorFrontalSulcusROI.getNodeWithMinimumYCoordinate(fiducialSurface);
   }
   
   //
   // Draw border from medial wall COG to node at front/ventral part of SF
   //
   const QString frontalSegment0("FrontalSegment0");
   drawBorderGeodesic(fiducialSurface,
                      NULL,
                      frontalSegment0,
                      medialWallCogNodeNumber,
                      sfVentralFrontalNodeNumber,
                      2.0);

   //
   // Draw border from front/ventral part of SF to the middle point in OrbS
   //
   const QString frontalSegment1("FrontalSegment1");
   drawBorderGeodesic(fiducialSurface,
                      NULL,
                      frontalSegment1,
                      sfVentralFrontalNodeNumber,
                      middleNodeNumber,
                      2.0);

   //
   // Draw border from middle point in OrbS to the end point 
   //
   const QString frontalSegment2("FrontalSegment2");
   drawBorderGeodesic(fiducialSurface,
                      NULL,
                      frontalSegment2,
                      middleNodeNumber,
                      endNodeNumber,
                      2.0);

   //
   // Merge to create the frontal cut
   //
   std::vector<QString> borderNames;
   borderNames.push_back(frontalSegment0);
   borderNames.push_back(frontalSegment1);
   borderNames.push_back(frontalSegment2);
   mergeBorders(flattenCutFrontalName,
                borderNames,
                true,
                false,
                fiducialSurface,
                0,
                0);

   //
   // Resample 
   //
   resampleBorder(fiducialSurface,
                  flattenCutFrontalName,
                  2.0);
}

/**
 * identify the cut sylvian.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyCutSylvian(const int medialWallCogNodeNumber) throw (BrainModelAlgorithmException)
{
   const QString flattenCutSylvianName(getFlattenStandardCutsBorderNamePrefix() + "Sylvian");
   
   //
   // Remove any existing cuts
   //
   borderProjectionFile->removeBordersWithName(flattenCutSylvianName);
   
   //
   // Draw from medial wall cog to tip of central sulcus
   //
   drawBorderGeodesic(inflatedSurface,
                      NULL,
                      flattenCutSylvianName,
                      medialWallCogNodeNumber,
                      sfInferiorBranchBeginNodeNumber,
                      2.0);
}

/**
 * identify the cut temporal.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyCutTemporal(const int medialWallCogNodeNumber) throw (BrainModelAlgorithmException)
{
   const QString flattenCutTemporalName(getFlattenStandardCutsBorderNamePrefix() + "Temporal");
   
   //
   // Remove any existing cuts
   //
   borderProjectionFile->removeBordersWithName(flattenCutTemporalName);
   
   //
   // Get most inferior node in surface which should be in the ventral
   // part of the temporal lobe
   //
   BrainModelSurfaceROINodeSelection fiducialROI(brainSet);
   fiducialROI.selectAllNodes(fiducialSurface);
   const int mostInferiorNodeNumber = 
      fiducialROI.getNodeWithMinimumZCoordinate(fiducialSurface);
      
   //
   // End point for temporal cut
   //
   const float endPointXYZ[3] = {
      (leftHemisphereFlag ? -60.0 : 60.0),
      -25.0,
      -28.0
   };
   const int endPointNodeNumber = 
      fiducialSurface->getCoordinateFile()->getCoordinateIndexClosestToPoint(
                                                            endPointXYZ);
   //
   // Draw border from Medial Wall COG to inferior temporal lobe
   //
   const QString temporalSegment1("TemporalSegment1");
   drawBorderGeodesic(fiducialSurface,
                      NULL,
                      temporalSegment1,
                      medialWallCogNodeNumber,
                      mostInferiorNodeNumber,
                      2.0);

   //
   // Draw border from middle point in OrbS to the end point 
   //
   const QString temporalSegment2("TemporalSegment2");
   drawBorderGeodesic(fiducialSurface,
                      NULL,
                      temporalSegment2,
                      mostInferiorNodeNumber,
                      endPointNodeNumber,
                      2.0);

   //
   // Merge to create the temporal cut
   //
   std::vector<QString> borderNames;
   borderNames.push_back(temporalSegment1);
   borderNames.push_back(temporalSegment2);
   mergeBorders(flattenCutTemporalName,
                borderNames,
                true,
                false,
                fiducialSurface,
                0,
                0);

   //
   // Resample 
   //
   resampleBorder(fiducialSurface,
                  flattenCutTemporalName,
                  2.0);
}
      
/**
 * generate paint identification of sulci.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::paintSulcalIdentification() throw (BrainModelAlgorithmException)
{
   //
   // Generate the probabilistic identification
   //
   BrainModelSurfacePaintSulcalIdentification 
      sid(brainSet,
          fiducialSurface,
          inflatedSurface,
          veryInflatedSurface,
          paintFile,
          paintFileGeographyColumnNumber,
          areaColorFile,
          depthSurfaceShapeFile,
          depthSurfaceShapeFileColumnNumber,
          vocabularyFile);
   sid.execute();
   
   //
   // Get the Sulcus ID paint column
   //
   paintFileSulcusIdColumnName = BrainModelSurfacePaintSulcalIdentification::getSulcusIdPaintColumnName();
   paintFileSulcusIdColumnNumber = paintFile->getColumnWithName(paintFileSulcusIdColumnName);
   if (paintFileSulcusIdColumnNumber < 0) {
      throw BrainModelAlgorithmException("Unable to find paint column named \""
                                         + BrainModelSurfacePaintSulcalIdentification::getSulcusIdPaintColumnName()
                                         + "\" in the paint file after identifying sulci.");
   }
   
   //
   // Find the "Sulcus ID" paint column
   //
   //
   // Get the output files of the probabilistic identification
   //
   metricFile = new MetricFile(*sid.getMetricFile());
}

/**
 * identify the central sulcus.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyCentralSulcus() throw (BrainModelAlgorithmException)
{
   //
   // Name for ROI file
   //
   const QString cesRoiFileName(createFileName("CeS",
                                  SpecFile::getRegionOfInterestFileExtension()));
   const QString cesRoiStringentFileName(createFileName("CeS_Stringent",
                                  SpecFile::getRegionOfInterestFileExtension()));
                  
   //
   // Remove any prexisting files
   //
   //rm Human.$SUBJECT.$HEM.CeS*.roi
   QFile::remove(cesRoiFileName);
   QFile::remove(cesRoiStringentFileName);
   
   //
   // Name for central sulcus
   //
   const QString centralSulcusLandmarkName("LANDMARK.CentralSulcus");
   const QString cesMedialFocusName("CeS-medial");
   const QString cesVentralFocusName("CeS-ventral");
   const QString cesMedialLandmarkFocusName("CeS-medial-Landmark");
   const QString cesVentralLandmarkFocusName("CeS-ventral-Landmark");
   const QString cesVentralExtremeFocusName("CeS-VentralExtreme");
   const QString cesMedialExtremeFocusName("CeS-MedialExtreme");
   
   //
   // Cleanup foci
   //
   //caret_command -surface-foci-delete  
   //   $OUTFOCIPROJ $OUTFOCIPROJ CeS-medial CeS-ventral CeS-medial-Landmark  
   //   CeS-ventral-Landmark CeS-VentralExtreme CeS-MedialExtreme
   //
   fociProjectionFile->deleteCellProjectionsWithName(cesMedialFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(cesVentralFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(cesMedialLandmarkFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(cesVentralLandmarkFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(cesVentralExtremeFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(cesMedialExtremeFocusName);
   
   //
   // Add foci colors
   //
   //caret_command -color-file-add-color 
   //   $FOCICOLOR $FOCICOLOR CeS-medial 255 0 0 -point-size 3 -symbol SPHERE
   //caret_command -color-file-add-color  
   //   $FOCICOLOR $FOCICOLOR CeS-ventral 0 255 0 -point-size 3 -symbol SPHERE
   //caret_command -color-file-add-color  
   //   $FOCICOLOR $FOCICOLOR CeS-MedialExtreme 255 0 0 -point-size 3 -symbol SPHERE
   //caret_command -color-file-add-color  
   //   $FOCICOLOR $FOCICOLOR CeS-VentralExtreme 0 255 0 -point-size 3 -symbol SPHERE
   //
   addFocusColor(cesMedialFocusName, 255, 0, 0);
   addFocusColor(cesVentralFocusName, 0, 255, 0);
   addFocusColor(cesMedialLandmarkFocusName, 255, 0, 0);
   addFocusColor(cesVentralLandmarkFocusName, 0, 255, 0);
   addFocusColor(cesVentralExtremeFocusName, 0, 255, 0);
   addFocusColor(cesMedialExtremeFocusName, 255, 0, 0);

   //
   // Add border colors
   //
   //caret_command -color-file-add-color  
   //   $BORDERCOLOR $BORDERCOLOR LANDMARK.CentralSulcus 255 255 0
   //
   borderColorFile->addColor(centralSulcusLandmarkName,
                             255, 255, 0);

   //
   // Remove any existing central sulcus landmark from border file
   //
   //caret_command -surface-border-delete  
   //   $OUTBORDERPROJ $OUTBORDERPROJ LANDMARK.CentralSulcus
   //
   borderProjectionFile->removeBordersWithName(centralSulcusLandmarkName);
   
   //
   // Select nodes in the CeS that have fiducial curvture between
   // -100.0 and -0.10.
   //
   //caret_command -surface-region-of-interest-selection   
   //   $INFLATED $TOPO Human.$SUBJECT.$HEM.CeS.roi Human.$SUBJECT.$HEM.CeS.roi  
   //   -paint Human.$SUBJECT.$HEM.SulcalID.paint "Sulcus ID" SUL.CeS NORMAL  
   //   -shape $SHAPE_OUT "Folding (Mean Curvature)" -100.0 -0.10 AND
   //
   BrainModelSurfaceROINodeSelection cesROI(brainSet);
   QString errorMessage = cesROI.selectNodesWithPaint(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                              inflatedSurface,
                              paintFile,
                              paintFileSulcusIdColumnNumber,
                              "SUL.CeS");
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   errorMessage = cesROI.selectNodesWithSurfaceShape(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND,
                              inflatedSurface,
                              curvatureShapeFile,
                              curvatureFiducialMeanColumnNumber,
                              -100.0,
                              -0.10);
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   saveRoiToFile(cesROI,
                 cesRoiFileName);

   //                               
   // Get nodes at min/max geometric values of the ROI
   //
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   cesROI.getNodesWithMinMaxXYZValues(inflatedSurface,
                                    mostMedialXNode,
                                    mostLateralXNode,
                                    minXNode, 
                                    maxXNode,
                                    minYNode,
                                    maxYNode,
                                    minZNode,
                                    maxZNode,
                                    absMinXNode,
                                    absMaxXNode,
                                    absMinYNode,
                                    absMaxYNode,
                                    absMinZNode,
                                    absMaxZNode);

   //
   // Add foci at medial and ventral extent of central sulcus
   //
   //caret_command -surface-place-foci-at-limits  
   //   $FIDUCIAL $INFLATED $TOPO Human.$SUBJECT.$HEM.CeS.roi  
   //   $OUTFOCIPROJ $OUTFOCIPROJ -z-min CeS-ventral -x-most-medial CeS-medial
   //
   const int cesVentralNodeNumber = minZNode;
   addFocusAtNode(cesVentralFocusName, 
                  cesVentralNodeNumber);
   inflatedSurface->getCoordinateFile()->getCoordinate(cesVentralNodeNumber,
                                    inflatedSurfaceCentralSulcusVentralTipXYZ);
   cesMedialNodeNumber = mostMedialXNode;
   addFocusAtNode(cesMedialFocusName,
                  cesMedialNodeNumber);
   if (DebugControl::getDebugOn()) {
      std::cout << "CeS Landmark Medial Node Number: " << cesMedialNodeNumber << std::endl;      
      std::cout << "CeS Landmark Ventral Node Number: " << cesVentralNodeNumber << std::endl;      
   }
     
   //
   // Select nodes in the central sulcus with fiducial curvature
   // between -100.0 and -0.16.  If necessary, dilate the ROI so
   // that it includes the most ventral and medial nodes in the CeS.
   //
   //caret_command -surface-region-of-interest-selection  
   //   $INFLATED $TOPO Human.$SUBJECT.$HEM.CeS_Stringent.roi  
   //   Human.$SUBJECT.$HEM.CeS_Stringent.roi  
   //   -paint Human.$SUBJECT.$HEM.SulcalID.paint "Sulcus ID" SUL.CeS NORMAL  
   //   -shape $SHAPE_OUT "Folding (Mean Curvature)" -100.0 -0.16 AND
   //
   BrainModelSurfaceROINodeSelection cesStringentROI(brainSet);
   errorMessage = cesStringentROI.selectNodesWithPaint(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                              inflatedSurface,
                              paintFile,
                              paintFileSulcusIdColumnNumber,
                              "SUL.CeS");
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   errorMessage = cesStringentROI.selectNodesWithSurfaceShape(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND,
                              inflatedSurface,
                              curvatureShapeFile,
                              curvatureFiducialMeanColumnNumber,
                              -100.0,
                              -0.16);
   saveRoiToFile(cesStringentROI,
                 cesRoiStringentFileName);
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }

   //
   // Draw a border using the geodesic method between the most
   // medial and ventral nodes in the ROI.
   //
   //caret_command -surface-border-draw-geodesic  
   //   $INFLATED $TOPO $OUTFOCIPROJ CeS-ventral CeS-medial  
   //   Human.$SUBJECT.$HEM.CeS_Stringent.roi $OUTBORDERPROJ $OUTBORDERPROJ  
   //   LANDMARK.CentralSulcus 1.0
   //
   drawBorderGeodesic(inflatedSurface,
                      &cesStringentROI,
                      centralSulcusLandmarkName,
                      cesVentralNodeNumber,
                      cesMedialNodeNumber,
                      1.0);

   //
   // Starting at the ventral node of the CeS, move down along 
   // surface links limiting the Y movement.  This will result
   // in the focus directly below the CeS on the edge of the
   // sylvian fissure.
   //
   //caret_command -surface-place-focus-at-extremum  
   //   $FIDUCIAL $INFLATED $TOPO $OUTFOCIPROJ CeS-ventral  
   //   $OUTFOCIPROJ CeS-VentralExtreme Z-NEG 100000.0 3.0 100000.0
   //
   const int cesVentralExtremeNodeNumber = 
      addFocusAtExtremum(inflatedSurface,
                         cesVentralNodeNumber,
                         BrainModelSurfaceFindExtremum::DIRECTION_Z_NEGATIVE,
                         100000.0,
                         3.0,
                         100000.0,
                         cesVentralExtremeFocusName);

   //
   // Starting at the dorsal node of the CeS, move up along 
   // surface links limiting the Y movement.  This will result
   // in the focus at the most dorsal position above the CeS.
   //
   //caret_command -surface-place-focus-at-extremum  
   //   $FIDUCIAL $INFLATED $TOPO $OUTFOCIPROJ CeS-medial  
   //   $OUTFOCIPROJ CeS-MedialExtreme X-MEDIAL 100000.0 5.0 100000.0
   //
   const int cesMedialExtremeNodeNumber = 
      addFocusAtExtremum(inflatedSurface,
                         cesMedialNodeNumber,
                         BrainModelSurfaceFindExtremum::DIRECTION_MEDIAL,
                         100000.0,
                         5.0,
                         100000.0,
                         cesMedialExtremeFocusName);

   //
   // Trim the ventral end of the CeS landmark, remove any links that 
   // are near the sylvian fissure
   //
   //caret_command -surface-border-nibbler  
   //   $INFLATED $TOPO $OUTBORDERPROJ $OUTBORDERPROJ LANDMARK.CentralSulcus  
   //   LANDMARK.CentralSulcus $OUTFOCIPROJ CeS-VentralExtreme -within-z-distance 19
   //
   nibbleBorderWithinDistance(inflatedSurface,
                              centralSulcusLandmarkName,
                              cesVentralExtremeNodeNumber,
                              BORDER_NIBBLE_MODE_WITHIN_DISTANCE_Z,
                              19.0);

   //
   // Trim the dorsal end of the CeS landmark, removing any links that
   // are near the most dorsal part of the surface.
   //
   //caret_command -surface-border-nibbler  
   //   $INFLATED $TOPO $OUTBORDERPROJ $OUTBORDERPROJ LANDMARK.CentralSulcus  
   //   LANDMARK.CentralSulcus $OUTFOCIPROJ CeS-MedialExtreme -within-x-distance  18
   //
   nibbleBorderWithinDistance(inflatedSurface,
                              centralSulcusLandmarkName,
                              cesMedialExtremeNodeNumber,
                              BORDER_NIBBLE_MODE_WITHIN_DISTANCE_X,
                              18.0);

   //
   //caret_command -surface-border-link-to-focus  
   //   $FIDUCIAL $TOPO $OUTBORDERPROJ LANDMARK.CentralSulcus  
   //   $OUTFOCIPROJ $OUTFOCIPROJ -first-link CeS-medial-Landmark
   //
                           

   //
   //caret_command -surface-border-link-to-focus  
   //   $FIDUCIAL $TOPO $OUTBORDERPROJ LANDMARK.CentralSulcus  
   //   $OUTFOCIPROJ $OUTFOCIPROJ -last-link CeS-ventral-Landmark
   //
}

/**
 * draw a border using metric method.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::drawBorderMetric(
                      const BrainModelSurface* borderSurface,
                      const BrainModelSurfaceROICreateBorderUsingMetricShape::MODE drawMode,
                      const MetricFile* metricShapeFile,
                      const int metricShapeFileColumn,
                      const QString borderName,
                      const int startNodeNumber,
                      const int endNodeNumber,
                      const float samplingDistance,
                      const BrainModelSurfaceROINodeSelection* optionalROI) throw (BrainModelAlgorithmException)
{
   BrainModelSurfaceROINodeSelection roi(brainSet);
   if (optionalROI != NULL) {
      roi = *optionalROI;
   }
   else {
      roi.selectAllNodes(borderSurface);
   }
   
   //
   // Force the starting and ending nodes to be in the ROI and connected
   //
   roi.expandSoNodesAreWithinAndConnected(borderSurface,
                                           startNodeNumber,
                                           endNodeNumber);
   
   Border border;
   int counter = 0;
   while (true) {         
      BrainModelSurfaceROICreateBorderUsingMetricShape
         metricDraw(brainSet,
                    (BrainModelSurface*)borderSurface,
                    &roi,
                    drawMode,
                    metricShapeFile,
                    metricShapeFileColumn,
                    borderName,
                    startNodeNumber,
                    endNodeNumber,
                    samplingDistance);
                    
      try {
         metricDraw.execute();
         border = metricDraw.getBorder();
         if (border.getNumberOfLinks() <= 0) {
            throw BrainModelAlgorithmException("Metric/Shape drawing of border named \""
                                               + borderName
                                               + "\" failed.");
         }
         break;
      }
      catch (BrainModelAlgorithmException& e) {
         if (counter < 3) {
            roi.dilate(borderSurface, 1);
         }
         else {
            throw e;
         }
         counter++;
      }
   }

   //
   // Project the border and add to border projection file
   //
   BorderFile borderFile;
   borderFile.addBorder(border);
   BorderProjectionFile bpf;
   BorderFileProjector projector(borderSurface, true);
   projector.projectBorderFile(&borderFile,
                               &bpf,
                               NULL);
   borderProjectionFile->append(bpf);
   
   if (DebugControl::getDebugOn()) {
      try {
         borderProjectionFile->writeFile(borderDebugFileName);
      }
      catch (FileException&) {
         std::cout << "WARNING: Unable to write landmark debug border projection file." << std::endl;
      }
   }
}
                            
/**
 * draw a border using geodesic method.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::drawBorderGeodesic(
                        const BrainModelSurface* borderSurface,
                        const BrainModelSurfaceROINodeSelection* roiIn,
                        const QString borderName,
                        const int startNodeNumber,
                        const int endNodeNumber,
                        const float samplingDistance) throw (BrainModelAlgorithmException)
{
   BrainModelSurfaceROINodeSelection roi(brainSet);
   if (roiIn != NULL) {
      roi = *roiIn;
   }
   else {
      roi.selectAllNodes(borderSurface);
   }
   
   //
   // Force the starting and ending nodes to be in the ROI and connected
   //
   roi.expandSoNodesAreWithinAndConnected(borderSurface,
                                           startNodeNumber,
                                           endNodeNumber);
                                              
   BrainModelSurfaceROICreateBorderUsingGeodesic
      cesDraw(brainSet,
              (BrainModelSurface*)borderSurface,
              &roi,
              borderName,
              startNodeNumber,
              endNodeNumber,
              samplingDistance);
   cesDraw.execute();
   Border border = cesDraw.getBorder();
   if (border.getNumberOfLinks() <= 0) {
      throw BrainModelAlgorithmException("Geodesic drawing of border named \""
                                         + borderName
                                         + "\" failed.");
   }

   //
   // Project the border and add to border projection file
   //
   BorderFile borderFile;
   borderFile.addBorder(border);
   BorderProjectionFile bpf;
   BorderFileProjector projector(borderSurface, true);
   projector.projectBorderFile(&borderFile,
                               &bpf,
                               NULL);
   borderProjectionFile->append(bpf);

   if (DebugControl::getDebugOn()) {
      try {
         borderProjectionFile->writeFile(borderDebugFileName);
      }
      catch (FileException&) {
         std::cout << "WARNING: Unable to write landmark debug border projection file." << std::endl;
      }
   }
}
 
/**
 * Find node at distance along geodesic path between nodes.
 */
int 
BrainModelSurfaceBorderLandmarkIdentification::findNodeAlongGeodesicPathBetweenNodes(
                      const BrainModelSurface* surface,
                      const int startNodeNumber,
                      const int endNodeNumber,
                      const int distanceFromStartNode,
                      const BrainModelSurfaceROINodeSelection* roiIn) throw (BrainModelAlgorithmException)
{
   BrainModelSurfaceROINodeSelection roi(brainSet);
   if (roiIn != NULL) {
      roi = *roiIn;
   }
   else {
      roi.selectAllNodes(surface);
   }
   
   //
   // Force the starting and ending nodes to be in the ROI and connected
   //
   roi.expandSoNodesAreWithinAndConnected(surface,
                                          startNodeNumber,
                                          endNodeNumber);
                                              
   //
   // Generate a border between nodes
   //
   BrainModelSurfaceROICreateBorderUsingGeodesic
      geodesic(brainSet,
              (BrainModelSurface*)surface,
              &roi,
              "JUNK",
              startNodeNumber,
              endNodeNumber,
              1.0);
   geodesic.execute();
   
   Border border = geodesic.getBorder();
   const int numLinks = border.getNumberOfLinks();
   if (numLinks <= 0) {
      throw BrainModelAlgorithmException("Geodesic path between \""
                                         + QString::number(startNodeNumber)
                                         + " "
                                         + QString::number(endNodeNumber)
                                         + "\" failed.");
   }
   
   //
   // Follow border until distance from start node is reached
   //
   const CoordinateFile* cf = surface->getCoordinateFile();
   float nodeXYZ[3];
   border.getLinkXYZ(numLinks - 1, nodeXYZ);
   float totalDistance = 0.0;
   for (int i = 0; i < (numLinks - 1); i++) {
      const float d = border.distanceBetweenLinks(i, i + 1);
      totalDistance += d;
      if (totalDistance >= distanceFromStartNode) {
         border.getLinkXYZ(i, nodeXYZ);
         break;
      }
   }
   
   //
   // Get the node at the distance from starting node
   //
   const int nodeNumber = cf->getCoordinateIndexClosestToPoint(nodeXYZ[0],
                                                               nodeXYZ[1],
                                                               nodeXYZ[2]);
                                                               
   return nodeNumber;
}

/**
 * project the foci.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::projectFoci()
{
   CellFileProjector fociProjector(fiducialSurface);
   fociProjector.projectFile(fociProjectionFile,
                             0,
                             CellFileProjector::PROJECTION_TYPE_ALL,
                             0,
                             false,
                             NULL);
}
      
/**
 * nibble border with offset.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::nibbleBorderWithinOffset(
                           const BrainModelSurface* surface,
                           const QString& borderName,
                           const float xyz[3],
                           const BORDER_NIBBLE_MODE_OFFSET nibbleMode,
                           const float nibbleOffset) throw (BrainModelAlgorithmException)
{
   BorderProjection* bp = borderProjectionFile->getFirstBorderProjectionByName(borderName);
   if (bp == NULL) {
      throw BrainModelAlgorithmException("Border named "
                                         + borderName
                                         + " not found for border nibbling.");
   }
   
   float extent[6] = {
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max()
   };
   switch (nibbleMode) {
      case BORDER_NIBBLE_MODE_OFFSET_GREATER_THAN_X:
         extent[1] = xyz[0] + nibbleOffset;
         break;
      case BORDER_NIBBLE_MODE_OFFSET_GREATER_THAN_Y:
         extent[3] = xyz[1] + nibbleOffset;
         break;
      case BORDER_NIBBLE_MODE_OFFSET_GREATER_THAN_Z:
         extent[5] = xyz[2] + nibbleOffset;
         break;
      case BORDER_NIBBLE_MODE_OFFSET_LESS_THAN_X:
         extent[0] = xyz[0] + nibbleOffset;
         break;
      case BORDER_NIBBLE_MODE_OFFSET_LESS_THAN_Y:
         extent[2] = xyz[1] + nibbleOffset;
         break;
      case BORDER_NIBBLE_MODE_OFFSET_LESS_THAN_Z:
         extent[4] = xyz[2] + nibbleOffset;
         break;
   }
   
   bp->removeLinksOutsideExtent(surface->getCoordinateFile(),
                                extent);
}
      
/**
 * remove loops from a border.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::removeLoopsFromBorder(
                                       const BrainModelSurface* surface,
                                       const QString& borderName,
                                       const char axisChar) throw (BrainModelAlgorithmException)
{
   //
   // Get the border projection
   //
   const BorderProjection* bp = 
      borderProjectionFile->getLastBorderProjectionByName(borderName);
   if (bp == NULL) {
      throw ("Unable to find border \"" + borderName + "\"for resampling");
   }
      
   //
   // Place the border projection in a temporary file
   //
   BorderProjectionFile tempBorderProjectionFile;
   tempBorderProjectionFile.addBorderProjection(*bp);
   
   //
   // Unproject the border
   //
   BorderProjectionUnprojector unprojector;
   BorderFile tempBorderFile;
   unprojector.unprojectBorderProjections(*(surface->getCoordinateFile()),
                                          tempBorderProjectionFile,
                                          tempBorderFile);
                                          
   //
   // Get border
   //
   if (tempBorderFile.getNumberOfBorders() <= 0) {
      throw BrainModelAlgorithmException("Unprojection error when resampling " + borderName);
   }
   
   //
   // Remove loops from the border
   //
   Border* b = tempBorderFile.getBorder(0);
   const int oldNumLinks = b->getNumberOfLinks();
   b->removeIntersectingLoops(axisChar);
   
   //
   // If no links removed, return
   //
   if (b->getNumberOfLinks() == oldNumLinks) {
      return;
   }
   
   //
   // Reproject the border
   //
   tempBorderProjectionFile.clear();
   BorderFileProjector projector(surface, true);
   projector.projectBorderFile(&tempBorderFile,
                               &tempBorderProjectionFile,
                               NULL);
                               
   //
   // Add border projection to output file
   //
   if (tempBorderProjectionFile.getNumberOfBorderProjections() <= 0) {
      throw BrainModelAlgorithmException("Reprojection error when resampling " + borderName);
   }
   borderProjectionFile->removeBordersWithName(borderName);
   borderProjectionFile->addBorderProjection(
                  *tempBorderProjectionFile.getBorderProjection(0));
}

/**
 * resample a border.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::resampleBorder(
                    const BrainModelSurface* surface,
                    const QString& borderName,
                    const float samplingDistance,
                    const bool projectToTilesFlag) throw (BrainModelAlgorithmException)
{
   //
   // Get the border projection
   //
   const BorderProjection* bp = 
      borderProjectionFile->getLastBorderProjectionByName(borderName);
   if (bp == NULL) {
      throw ("Unable to find border \"" + borderName + "\"for resampling");
   }
      
   //
   // Place the border projection in a temporary file
   //
   BorderProjectionFile tempBorderProjectionFile;
   tempBorderProjectionFile.addBorderProjection(*bp);
   
   //
   // Unproject the border
   //
   BorderProjectionUnprojector unprojector;
   BorderFile tempBorderFile;
   unprojector.unprojectBorderProjections(*(surface->getCoordinateFile()),
                                          tempBorderProjectionFile,
                                          tempBorderFile);
                                          
   //
   // Get border
   //
   if (tempBorderFile.getNumberOfBorders() <= 0) {
      throw BrainModelAlgorithmException("Unprojection error when resampling " + borderName);
   }
   
   //
   // Resample the border
   //
   Border* b = tempBorderFile.getBorder(0);
   int newNumLinks = 0;
   b->resampleBorderToDensity(samplingDistance, 2, newNumLinks);
   
   //
   // Reproject the border
   //
   tempBorderProjectionFile.clear();
   BorderFileProjector projector(surface, projectToTilesFlag);
   projector.projectBorderFile(&tempBorderFile,
                               &tempBorderProjectionFile,
                               NULL);
                               
   //
   // Add border projection to output file
   //
   if (tempBorderProjectionFile.getNumberOfBorderProjections() <= 0) {
      throw BrainModelAlgorithmException("Reprojection error when resampling " + borderName);
   }
   borderProjectionFile->removeBordersWithName(borderName);
   borderProjectionFile->addBorderProjection(
                  *tempBorderProjectionFile.getBorderProjection(0));
}

/**
 * nibble border within distance.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::nibbleBorderWithinDistance(
                                          const BrainModelSurface* surface,
                                          const QString& borderName,
                                          const int nodeNumber,
                                          const BORDER_NIBBLE_MODE_DISTANCE nibbleMode,
                                          const float nibbleDistance)
                                              throw (BrainModelAlgorithmException)
{
   const float* xyz = surface->getCoordinateFile()->getCoordinate(nodeNumber);
   BorderProjection* bp = borderProjectionFile->getFirstBorderProjectionByName(borderName);
   if (bp == NULL) {
      throw BrainModelAlgorithmException("Border named "
                                         + borderName
                                         + " not found for border nibbling.");
   }
   
   float withinXDistance = -1.0;
   float withinYDistance = -1.0;
   float withinZDistance = -1.0;
   float withinLinearDistance = -1.0;
   switch (nibbleMode) {
      case BORDER_NIBBLE_MODE_WITHIN_DISTANCE_X:
         withinXDistance = nibbleDistance;
         break;
      case BORDER_NIBBLE_MODE_WITHIN_DISTANCE_Y:
         withinYDistance = nibbleDistance;
         break;
      case BORDER_NIBBLE_MODE_WITHIN_DISTANCE_Z:
         withinZDistance = nibbleDistance;
         break;
      case BORDER_NIBBLE_MODE_WITHIN_DISTANCE_LINEAR:
         withinLinearDistance = nibbleDistance;
         break;
   }
   
   bp->removeLinksNearPoint(surface->getCoordinateFile(),
                            xyz,
                            withinXDistance,
                            withinYDistance,
                            withinZDistance,
                            withinLinearDistance);
}                        

/**
 * add focus color.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::addFocusColor(
                                           const QString& colorName,
                                           const unsigned char red,
                                           const unsigned char green, 
                                           const unsigned char blue)
{
   fociColorFile->addColor(colorName,
                           red, green, blue, 255,
                           3, 1,
                           ColorFile::ColorStorage::SYMBOL_SPHERE);
                           
   if (DebugControl::getDebugOn()) {
      try {
         fociColorFile->writeFile(fociColorDebugFileName);
      }
      catch (FileException&) {
         std::cout << "WARNING: Unable to write landmark debug foci color file." << std::endl;
      }
   }
}                         

/**
 * add focus at extremum.
 */
int 
BrainModelSurfaceBorderLandmarkIdentification::addFocusAtExtremum(
                        const BrainModelSurface* extremumSurface,
                        const int startingNodeNumber,
                        const BrainModelSurfaceFindExtremum::DIRECTION searchDirection,
                        const float xMaximumMovement,
                        const float yMaximumMovement,
                        const float zMaximumMovement,
                        const QString& focusName,
                        BrainModelSurfaceROINodeSelection* roiAlongPath,
                        const BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION xr,
                        const BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION yr,
                        const BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION zr) throw (BrainModelAlgorithmException)
{
   //
   // Get the extremum
   //
   const CoordinateFile* cf = extremumSurface->getCoordinateFile();
   const float* startXYZ = cf->getCoordinate(startingNodeNumber);
   BrainModelSurfaceFindExtremum bmsfe(brainSet,
                                       (BrainModelSurface*)extremumSurface,
                                       searchDirection,
                                       startXYZ,
                                       xMaximumMovement,
                                       yMaximumMovement,
                                       zMaximumMovement);
   bmsfe.setNodeNormalRestriction(xr, yr, zr);
   bmsfe.execute();
   const int node = bmsfe.getExtremumNode();
   if (node < 0) {
      throw ("Find extremum failed to find node for "
             + focusName);
   }
   
   if (focusName.isEmpty() == false) {
      addFocusAtNode(focusName,
                     node);
   }
   
   if (roiAlongPath != NULL) {
      bmsfe.setRegionOfInterestToNodesInPath(*roiAlongPath);
   }
   
   return node;
}

/**
 * add a focus at a node.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::addFocusAtNode(
                       const QString& focusName,
                       const int placeAtNodeNumber) throw (BrainModelAlgorithmException)
{
   if ((placeAtNodeNumber >= 0) &&
       (placeAtNodeNumber < fiducialSurface->getNumberOfNodes())) {
      fociProjectionFile->addCellProjection(
                            CellProjection(focusName,
                                           fiducialSurface->getCoordinateFile(),
                                           placeAtNodeNumber,
                                           fiducialSurface->getStructure()));
      if (DebugControl::getDebugOn()) {
         try {
            fociProjectionFile->writeFile(fociProjectionDebugFileName);
         }
         catch (FileException&) {
            std::cout << "WARNING: Unable to write landmark debug foci projection file." << std::endl;
         }
      }
   }
   else {
      throw BrainModelAlgorithmException("addFocusAtNode() Invalid node number "
                                         + QString::number(placeAtNodeNumber)
                                         + " for focus named "
                                         + focusName);
   }
}
                    
/**
 * add a focus at the node nearest the XYZ.
 */
int 
BrainModelSurfaceBorderLandmarkIdentification::addFocusAtNodeNearestXYZ(
                                               const BrainModelSurface* surface,
                                               const QString& focusName,
                                               const float xyz[3])
{
   const int nodeNumber = 
      surface->getCoordinateFile()->getCoordinateIndexClosestToPoint(xyz[0],
                                                                     xyz[1],
                                                                     xyz[2]);
   addFocusAtNode(focusName,
                  nodeNumber);

   return nodeNumber;
}
                                   
/**
 * add a focus at a location.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::addFocusAtXYZ(
                                                  const QString& focusName,
                                                  const float xyz[3])
{
   CellProjection cp;
   cp.setName(focusName);
   cp.setXYZ(xyz);
   
   fociProjectionFile->addCellProjection(cp);

   if (DebugControl::getDebugOn()) {
      try {
         fociProjectionFile->writeFile(fociProjectionDebugFileName);
      }
      catch (FileException&) {
         std::cout << "WARNING: Unable to write landmark debug foci projection file." << std::endl;
      }
   }
}

/**
 * add a focus at a location.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::addFocusAtXYZ(
                                     const BrainModelSurface* focusSurface,
                                     const QString& focusName,
                                     const float xyz[3])
{
   const CoordinateFile* cf = focusSurface->getCoordinateFile();
   const int nodeNum = cf->getCoordinateIndexClosestToPoint(xyz[0],
                                                            xyz[1],
                                                            xyz[2]);
   addFocusAtNode(focusName, nodeNum);
}      
              
/**
 * add a focus at a border link.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::addFocusAtBorderLink(
                                                  const BorderProjection* bp,
                                                  const int borderLinkNumber,
                                                  const QString& focusName)
{
   if ((borderLinkNumber >= 0) &&
       (borderLinkNumber < bp->getNumberOfLinks())) {
      const BorderProjectionLink* bpl = bp->getBorderProjectionLink(borderLinkNumber);
      float xyz[3];
      bpl->unprojectLink(fiducialSurface->getCoordinateFile(),
                         xyz);
      addFocusAtXYZ(focusName, xyz);
   }
}
                                
/**
 * save an ROI to a file.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::saveRoiToFile(
                   const BrainModelSurfaceROINodeSelection& roi,
                   const QString& roiFileName)
{
   if (DebugControl::getDebugOn()) {
      NodeRegionOfInterestFile nroi;
      roi.setRegionOfInterestIntoFile(nroi);
      try {
         nroi.writeFile(roiFileName);
      }
      catch (FileException& e) {
         std::cout << "WARNING: Unable to write ROI file named "
                   << roiFileName.toAscii().constData()
                   << std::endl
                   << "   Error Message: "
                   << e.whatQString().toAscii().constData()
                   << std::endl;
      }      
   }
}
                         
/**
 * identify the sylvian fissure.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifySylvianFissure() throw (BrainModelAlgorithmException)
{
   //
   // Names of things
   //
   const QString sylvianFissureLandmarkName("LANDMARK.SylvianFissure");
   const QString sfDorsalLandmarkName("LANDMARK.SFdorsal");
   const QString sfAntLandmarkName("LANDMARK.SFant");
   const QString sfSecondaryLandmarkName("LANDMARK.SF-secondary");
   const QString sfGenericColorName("SF");
   const QString sfAnteriorFocusName("SF_Anterior");
   const QString sfAnteriorDeepFocusName("SF_Anterior_Deep");
   const QString sfPosteriorFocusName("SF_Posterior");
   const QString sfPosteriorLandmarkFocusName("SF_Posterior-Landmark"); 
   const QString sfAnteriorLandmarkFocusName("SF_Anterior-Landmark");
   const QString sfDorsalFocusName("SF_Dorsal"); 
   const QString sfVentralFrontalFocusName("SF_VentralFrontal"); 
   const QString sfVentralFrontalExtremeFocusName("SF_VentralFrontalExtreme"); 
   const QString sfInferiorBranchBeginFocusName("SF_Inferior_Branch_Begin"); 
   const QString sfIntersectSuperiorInferiorFocusName("SF_Intersect_Superior_Inferior"); 
   const QString sfSaddleAnteriorLimitFocusName("SFsaddleAnteriorLimit"); 
   const QString sfCogFocusName("SF_COG");
   const QString sfRoiFileName(createFileName("SUL.SF",
                             SpecFile::getRegionOfInterestFileExtension()));
   const QString sfDeep7RoiFileName(createFileName("SUL.SF.deep7",
                             SpecFile::getRegionOfInterestFileExtension()));
   const QString sfDeepFundalRoiFileName(createFileName("SUL.SF.deep_fundal",
                             SpecFile::getRegionOfInterestFileExtension()));
   const QString sfPosteriorRoiFileName(createFileName("SUL.SF.Posterior",
                             SpecFile::getRegionOfInterestFileExtension()));
   const QString sfVeryPosteriorRoiFileName(createFileName("SUL.SF.VeryPosterior",
                             SpecFile::getRegionOfInterestFileExtension()));
   const QString sfAnteriorRoiFileName(createFileName("SUL.SF.Anterior",
                             SpecFile::getRegionOfInterestFileExtension()));
   const QString sfFundalRoiFileName(createFileName("SUL.SF.fundal",
                             SpecFile::getRegionOfInterestFileExtension()));
    
   //
   // Add some colors
   //
   // caret_command -color-file-add-color $BORDERCOLOR $BORDERCOLOR 
   //    LANDMARK.SylvianFissure 0 255 255 -point-size 3 -symbol SPHERE
   // caret_command -color-file-add-color $FOCICOLOR $FOCICOLOR 
   //    SF 48 164 255 -point-size 3 -symbol SPHERE
   borderColorFile->addColor(sylvianFissureLandmarkName,
                           0, 255, 255, 255, 
                           3, 1,
                           ColorFile::ColorStorage::SYMBOL_SPHERE);
   addFocusColor(sfGenericColorName, 48, 164, 255);

   //
   // Remove existing foci 
   //
   // caret_command -surface-foci-delete $OUTFOCIPROJ $OUTFOCIPROJ 
   //    SF_Anterior SF_Posterior SF_Posterior-Landmark SF_Anterior-Landmark
   //    SF_Dorsal SF_VentralFrontal SF_VentralFrontalExtreme 
   //    SF_Inferior_Branch_Begin SF_Intersect_Superior_Inferior 
   //    SFsaddleAnteriorLimit SUL.SF_COG
   fociProjectionFile->deleteCellProjectionsWithName(sfAnteriorFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(sfAnteriorDeepFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(sfPosteriorFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(sfPosteriorLandmarkFocusName); 
   fociProjectionFile->deleteCellProjectionsWithName(sfAnteriorLandmarkFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(sfDorsalFocusName); 
   fociProjectionFile->deleteCellProjectionsWithName(sfVentralFrontalFocusName); 
   fociProjectionFile->deleteCellProjectionsWithName(sfVentralFrontalExtremeFocusName); 
   fociProjectionFile->deleteCellProjectionsWithName(sfInferiorBranchBeginFocusName); 
   fociProjectionFile->deleteCellProjectionsWithName(sfIntersectSuperiorInferiorFocusName); 
   fociProjectionFile->deleteCellProjectionsWithName(sfSaddleAnteriorLimitFocusName); 
   fociProjectionFile->deleteCellProjectionsWithName(sfCogFocusName);

   //
   // Remove borders
   //
   // caret_command -surface-border-delete $OUTBORDERPROJ $OUTBORDERPROJ 
   //    LANDMARK.SylvianFissure LANDMARK.SFdorsal LANDMARK.SFant LANDMARK.SF-secondary
   //
   borderProjectionFile->removeBordersWithName(sylvianFissureLandmarkName);
   borderProjectionFile->removeBordersWithName(sfDorsalLandmarkName);
   borderProjectionFile->removeBordersWithName(sfAntLandmarkName);
   borderProjectionFile->removeBordersWithName(sfSecondaryLandmarkName);

   //
   // Remove ROI files
   //
   QFile::remove(sfRoiFileName);
   QFile::remove(sfDeep7RoiFileName);
   QFile::remove(sfDeepFundalRoiFileName);
   QFile::remove(sfPosteriorRoiFileName);
   QFile::remove(sfVeryPosteriorRoiFileName);
   QFile::remove(sfAnteriorRoiFileName);
   QFile::remove(sfFundalRoiFileName);

   //
   // Select nodes in the Sylvian Fissure
   //
   // caret_command -surface-region-of-interest-selection $INFLATED $TOPO 
   //    SUL.SF.roi SUL.SF.roi -paint $SULCAL_ID_PAINT "Sulcus ID" SUL.SF NORMAL
   //
   BrainModelSurfaceROINodeSelection sfROI(brainSet);
   QString errorMessage = sfROI.selectNodesWithPaint(
                           BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                           inflatedSurface,
                           paintFile,
                           paintFileSulcusIdColumnNumber,
                           "SUL.SF");
   if (errorMessage.isEmpty() == false) {
    throw BrainModelAlgorithmException(errorMessage);
   }

   // 
   //  Remove islands from SUL.SF.roi  (JWH)
   // 
   // caret_command -surface-region-of-interest-selection $INFLATED $TOPO SUL.SF.roi SUL.SF.roi -remove-islands
   //
   sfROI.discardIslands(inflatedSurface);
   saveRoiToFile(sfROI, sfRoiFileName);
   
   // 
   // Start with SF ROI and select nodes with depth (-500, -7)
   //
   // ### JWH 02/29/2008 Fix problem with SF Landmark going out of SF and towards the posterior 
   // ### Using a larger DEPTH = -15.0  fixes case 9 LEFT
   // ###caret_command -surface-region-of-interest-selection $INFLATED $TOPO 
   // ###  SUL.SF.roi SUL.SF.deep7.roi -shape $SHAPE_OUT Depth -500.0 -7.0 AND
   // caret_command -surface-region-of-interest-selection $INFLATED $TOPO 
   //    SUL.SF.roi SUL.SF.deep7.roi -shape $SHAPE_OUT Depth -500.0 -7.0 AND
   //
   BrainModelSurfaceROINodeSelection sfDeep7ROI(sfROI);
   errorMessage = sfDeep7ROI.selectNodesWithSurfaceShape(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND,
                              inflatedSurface,
                              depthSurfaceShapeFile,
                              depthSurfaceShapeFileColumnNumber,
                              -500.0,
                              -7.0);
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   saveRoiToFile(sfDeep7ROI, sfDeep7RoiFileName);

   //
   // Further restrict the ROI to curvature (-100, -0.05) on inflated surface
   //
   // caret_command -surface-region-of-interest-selection $INFLATED $TOPO 
   //    SUL.SF.deep7.roi SUL.SF.deep_fundal.roi -shape $SHAPE_INFLATED 
   //    "Folding (Mean Curvature) INFLATED" -100.0 -0.05 AND
   BrainModelSurfaceROINodeSelection sfDeepFundalROI(sfDeep7ROI);
   errorMessage = sfDeepFundalROI.selectNodesWithSurfaceShape(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND,
                              inflatedSurface,
                              curvatureShapeFile,
                              curvatureInflatedMeanColumnNumber,
                              -100.0,
                              -0.05);
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   saveRoiToFile(sfDeepFundalROI, sfDeepFundalRoiFileName);

   //
   // Get center of gravity and place a focus there
   //
   // caret_command -surface-place-foci-at-limits $FIDUCIAL $FIDUCIAL $TOPO 
   //    SUL.SF.deep_fundal.roi $OUTFOCIPROJ $OUTFOCIPROJ -cog SUL.SF_COG
   //
   float sfCogXYZ[3];
   sfDeepFundalROI.getCenterOfGravityOfSelectedNodes(fiducialSurface, sfCogXYZ);
   addFocusAtXYZ(sfCogFocusName, sfCogXYZ);
                                
   //
   // Limit the maximum-Y in the SF ROI to the center of gravity
   //
   // caret_command -surface-region-of-interest-selection $FIDUCIAL $TOPO 
   //    SUL.SF.roi SUL.SF.Posterior.roi -limit-y-max-focus $OUTFOCIPROJ SUL.SF_COG
   // 
   const float sfROIExtent[6] = {
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
      sfCogXYZ[1],
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
   };
   BrainModelSurfaceROINodeSelection sfPosteriorROI(sfROI);
   sfPosteriorROI.limitExtent(fiducialSurface, sfROIExtent);
   
   //  Remove islands from SUL.SF.Posterior.roi since some appear on
   //  the medial surface which causes an invalid placement of the focus
   //  SF_Inferior_Branch_Begin
   // 
   // caret_command -surface-region-of-interest-selection $FIDUCIAL $TOPO 
   //    SUL.SF.Posterior.roi SUL.SF.Posterior.roi -remove-islands
   sfPosteriorROI.discardIslands(fiducialSurface);
   saveRoiToFile(sfPosteriorROI, sfPosteriorRoiFileName);
   
   //                               
   // Get nodes at min/max geometric values of the SF-Posterior ROI
   //
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   sfPosteriorROI.getNodesWithMinMaxXYZValues(inflatedSurface,
                                    mostMedialXNode,
                                    mostLateralXNode,
                                    minXNode, 
                                    maxXNode,
                                    minYNode,
                                    maxYNode,
                                    minZNode,
                                    maxZNode,
                                    absMinXNode,
                                    absMaxXNode,
                                    absMinYNode,
                                    absMaxYNode,
                                    absMinZNode,
                                    absMaxZNode);
   
   //
   // Get node at z-min of ROI and place a focus there
   //
   // caret_command -surface-place-foci-at-limits $FIDUCIAL $INFLATED $TOPO 
   //    SUL.SF.Posterior.roi $OUTFOCIPROJ $OUTFOCIPROJ -z-min SF_Inferior_Branch_Begin
   //
   sfInferiorBranchBeginNodeNumber = minZNode;
   if (DebugControl::getDebugOn()) {
      std::cout << sfInferiorBranchBeginFocusName.toAscii().constData()
                << " node number is "
                << sfInferiorBranchBeginNodeNumber
                << std::endl;
   }
   addFocusAtNode(sfInferiorBranchBeginFocusName, 
                  sfInferiorBranchBeginNodeNumber);
   
   //
   // Create a very posterior ROI  // JWH 10 April
   //
   float veryPosteriorExtent[6] = {
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
      -17.0,
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
   };
   BrainModelSurfaceROINodeSelection sfVeryPosteriorROI(brainSet);
   while ((sfVeryPosteriorROI.getNumberOfNodesSelected() < 25) &&
          (veryPosteriorExtent[3] <= 0.0)) {
      sfVeryPosteriorROI = sfROI;
      veryPosteriorExtent[3] += 1.0;
      sfVeryPosteriorROI.limitExtent(inflatedSurface, veryPosteriorExtent);      
   }
   saveRoiToFile(sfVeryPosteriorROI, sfVeryPosteriorRoiFileName);
   
   //                               
   // Get nodes at min/max geometric values of the Deep Fundal ROI
   //
   //sfDeepFundalROI.getNodesWithMinMaxXYZValues(inflatedSurface, // JWH 10 April
   sfVeryPosteriorROI.getNodesWithMinMaxXYZValues(fiducialSurface,
                                    mostMedialXNode,
                                    mostLateralXNode,
                                    minXNode, 
                                    maxXNode,
                                    minYNode,
                                    maxYNode,
                                    minZNode,
                                    maxZNode,
                                    absMinXNode,
                                    absMaxXNode,
                                    absMinYNode,
                                    absMaxYNode,
                                    absMinZNode,
                                    absMaxZNode);
   
   //
   // Get node numbers and place foci at Y-Min and Z-Max of Deep-Fundal ROI
   //
   // caret_command -surface-place-foci-at-limits $FIDUCIAL $INFLATED $TOPO 
   //    SUL.SF.deep_fundal.roi $OUTFOCIPROJ $OUTFOCIPROJ -y-min SF_Posterior 
   //    -z-max SF_Dorsal
   const int sfPosteriorNodeNumber = minYNode;
   if (DebugControl::getDebugOn()) {
      std::cout << sfPosteriorFocusName.toAscii().constData()
                << " node number is "
                << sfPosteriorNodeNumber
                << std::endl;
   }
   addFocusAtNode(sfPosteriorFocusName,
                  sfPosteriorNodeNumber);

   // JWH 10 April
   sfVeryPosteriorROI.getNodesWithMinMaxXYZValues(inflatedSurface,
                                    mostMedialXNode,
                                    mostLateralXNode,
                                    minXNode, 
                                    maxXNode,
                                    minYNode,
                                    maxYNode,
                                    minZNode,
                                    maxZNode,
                                    absMinXNode,
                                    absMaxXNode,
                                    absMinYNode,
                                    absMaxYNode,
                                    absMinZNode,
                                    absMaxZNode);
   const int sfDorsalNodeNumber = maxZNode;
   if (DebugControl::getDebugOn()) {
      std::cout << sfDorsalFocusName.toAscii().constData()
                << " node number is "
                << sfDorsalNodeNumber
                << std::endl;
   }
   addFocusAtNode(sfDorsalFocusName,
                  sfDorsalNodeNumber);
   
   //
   // LEFT RIGHT ISSUE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   //
   // caret_command -surface-region-of-interest-selection $FIDUCIAL $TOPO 
   //    SUL.SF.deep_fundal.roi SUL.SF.Anterior.roi -limit-x-max-focus 
   //    $OUTFOCIPROJ SUL.SF_COG -limit-y-min-focus $OUTFOCIPROJ TemporalPole
   BrainModelSurfaceROINodeSelection sfAnteriorROI(sfDeepFundalROI);
   float sfAnteriorROIExtent[6] = {
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      fiducialSurfaceTemporalPoleXYZ[1],
      std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max()
   };
   if (leftHemisphereFlag) {
      sfAnteriorROIExtent[1] = sfCogXYZ[0];
   }
   else {
      sfAnteriorROIExtent[0] = sfCogXYZ[0];
   }
   sfAnteriorROI.limitExtent(fiducialSurface,
                             sfAnteriorROIExtent);
   saveRoiToFile(sfAnteriorROI, sfAnteriorRoiFileName);

   //
   // Place a focus at the COG of the SUL.SF.Anterior.ROI
   //
   // caret_command -surface-place-foci-at-limits $FIDUCIAL $FIDUCIAL $TOPO 
   //    SUL.SF.Anterior.roi $OUTFOCIPROJ $OUTFOCIPROJ -cog SF_Anterior
   float sfAnteriorXYZ[3];
   sfAnteriorROI.getCenterOfGravityOfSelectedNodes(fiducialSurface, sfAnteriorXYZ);
   addFocusAtXYZ(sfAnteriorFocusName, sfAnteriorXYZ);
   const int sfAnteriorNodeNumber = 
      fiducialSurface->getCoordinateFile()->getCoordinateIndexClosestToPoint(
                                                  sfAnteriorXYZ[0],
                                                  sfAnteriorXYZ[1],
                                                  sfAnteriorXYZ[2]);
   if (DebugControl::getDebugOn()) {
      std::cout << sfAnteriorFocusName.toAscii().constData()
                << " node number is "
                << sfAnteriorNodeNumber
                << std::endl;
   }
                                                   
   //
   // Find point inside the curved part of the fissure
   //
/*
   const float* anteriorXYZ = 
      fiducialSurface->getCoordinateFile()->getCoordinate(sfAnteriorFocusNodeNumber);
   const float sfDeepAnteriorExtent[6] = {
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      anteriorXYZ[1] - 20.0,
      anteriorXYZ[1],
      anteriorXYZ[2] - 15,
      anteriorXYZ[2] + 5.0,
   };
*/
   const int sfAnteriorDeepNodeNumber = 
      getNearbyNodeWithShapeValue(fiducialSurface,
                                  depthSurfaceShapeFile,
                                  depthSurfaceShapeFileColumnNumber,
                                  -100000.0,
                                  sfAnteriorNodeNumber,
                                  15.0,
                                  NULL,
                                  NULL);
   addFocusAtNode(sfAnteriorDeepFocusName,
                  sfAnteriorDeepNodeNumber);
   if (DebugControl::getDebugOn()) {
      std::cout << sfAnteriorDeepFocusName.toAscii().constData()
                << " node number is "
                << sfAnteriorDeepNodeNumber
                << std::endl;
   }

   //
   // Draw a border along the deep fundus
   //
   // caret_command -surface-border-draw-geodesic $INFLATED $TOPO $OUTFOCIPROJ 
   //    SF_Posterior SF_Anterior SUL.SF.deep_fundal.roi 
   //    $OUTBORDERPROJ $OUTBORDERPROJ LANDMARK.SFdorsal 3.0
   drawBorderGeodesic(inflatedSurface,
                      &sfDeepFundalROI,
                      sfDorsalLandmarkName,
                      sfPosteriorNodeNumber,
                      sfAnteriorDeepNodeNumber,  //sfAnteriorNodeNumber,
                      3.0);
                      
   // 
   //  Set X-Coord for left or right
   // 
   // if [ $LEFT_FLAG -eq 1 ]
   // then
   //    caret_command -surface-foci-create $FIDUCIAL $TOPO $OUTFOCIPROJ 
   //       $OUTFOCIPROJ -focus SF_VentralFrontal -16 12 -19
   // else
   //    caret_command -surface-foci-create $FIDUCIAL $TOPO $OUTFOCIPROJ 
   //       $OUTFOCIPROJ -focus SF_VentralFrontal 16 12 -19
   // fi
   float sfVentralFrontalNodeXYZ[3] = {
      16.0, 12.0, -19.0
   };
   if (leftHemisphereFlag) {
      sfVentralFrontalNodeXYZ[0] = -16.0;
   }
   sfVentralFrontalNodeNumber = 
      fiducialSurface->getCoordinateFile()->getCoordinateIndexClosestToPoint(
                                                  sfVentralFrontalNodeXYZ[0],
                                                  sfVentralFrontalNodeXYZ[1],
                                                  sfVentralFrontalNodeXYZ[2]);
   addFocusAtNode(sfVentralFrontalFocusName, sfVentralFrontalNodeNumber);
   if (DebugControl::getDebugOn()) {
      std::cout << sfVentralFrontalFocusName.toAscii().constData()
                << " node number is "
                << sfVentralFrontalNodeNumber
                << std::endl;
   }


   // 
   //  Set X-Coord for LEFT
   // 
   // if [ $LEFT_FLAG -eq 1 ]
   // then
   //    caret_command -surface-place-focus-at-extremum $FIDUCIAL $FIDUCIAL $TOPO
   //       $OUTFOCIPROJ SF_VentralFrontal $OUTFOCIPROJ SF_VentralFrontalExtreme Z-NEG -100000.0 100000.0 100000.0
   // else
   //    caret_command -surface-place-focus-at-extremum $FIDUCIAL $FIDUCIAL $TOPO 
   //       $OUTFOCIPROJ SF_VentralFrontal $OUTFOCIPROJ SF_VentralFrontalExtreme Z-NEG 100000.0 100000.0 100000.0
   // fi
   float sfVentralFrontalExtremeLimitX = 100000.0;
   if (leftHemisphereFlag) {
      sfVentralFrontalExtremeLimitX = -100000.0;
   }
   const int sfVentralFrontalExtremeNodeNumber = 
         addFocusAtExtremum(fiducialSurface,
                            sfVentralFrontalNodeNumber,
                            BrainModelSurfaceFindExtremum::DIRECTION_Z_NEGATIVE,
                            sfVentralFrontalExtremeLimitX,
                            100000.0,
                            100000.0,
                            sfVentralFrontalExtremeFocusName);
   if (DebugControl::getDebugOn()) {
      std::cout << sfVentralFrontalExtremeFocusName.toAscii().constData()
                << " node number is "
                << sfVentralFrontalExtremeNodeNumber
                << std::endl;
   }

   //
   // Create ROI of fundal region
   //
   // caret_command -surface-region-of-interest-selection $INFLATED $TOPO 
   //    SUL.SF.roi SUL.SF.fundal.roi -shape 
   //    $SHAPE_INFLATED "Folding (Mean Curvature) INFLATED" -100.0 -0.05 AND
   //
   BrainModelSurfaceROINodeSelection sfFundalROI(sfROI);
   sfFundalROI.selectNodesWithMetric(BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND,
                                     inflatedSurface,
                                     curvatureShapeFile,
                                     curvatureInflatedMeanColumnNumber,
                                     -100.0,
                                     -0.05);
   saveRoiToFile(sfFundalROI, sfFundalRoiFileName);
   
   //
   // Draw a border from SF-Anterior to SF-Ventral Frontal Extreme
   //
   // caret_command -surface-border-draw-geodesic $INFLATED $TOPO 
   //    $OUTFOCIPROJ SF_Anterior SF_VentralFrontalExtreme SUL.SF.fundal.roi 
   //    $OUTBORDERPROJ $OUTBORDERPROJ LANDMARK.SFant 1.0
   drawBorderGeodesic(inflatedSurface,
                      &sfFundalROI,
                      sfAntLandmarkName,
                      sfAnteriorDeepNodeNumber,   // sfAnteriorNodeNumber,
                      sfVentralFrontalExtremeNodeNumber,
                      1.0);
   
   //
   // Merge the LANDMARK.SFdorsal and LANDMARK.SFant into
   // LANDMARK.SylvianFissure
   //
   // caret_command -surface-border-merge $OUTBORDERPROJ $OUTBORDERPROJ 
   //    LANDMARK.SylvianFissure LANDMARK.SFdorsal LANDMARK.SFant 
   //    -delete-input-border-projections -smooth-junctions $INFLATED $TOPO 4 1
   //
   //BorderProjection* sylvianFissureBorderProjection =
      mergeBorders(sylvianFissureLandmarkName,
                   sfDorsalLandmarkName,
                   sfAntLandmarkName,
                   true,
                   false,
                   inflatedSurface,
                   4,
                   1);
   
   //
   // Draw a border along secondary SF
   // Typically starts deep in the Sylvian Fissure, exits SF, and ends
   // near or in parietal lobe.
   //
   // caret_command -surface-border-draw-metric $INFLATED $TOPO 
   //    $SHAPE_INFLATED "Folding (Mean Curvature) INFLATED" NEGATIVE 
   //    $OUTFOCIPROJ SF_Inferior_Branch_Begin SF_Posterior 
   //    $OUTBORDERPROJ $OUTBORDERPROJ LANDMARK.SF-secondary 1.0
   //
   // Try ellipsoid surface since as the border exits the posterior
   // part of the sylvian fissure on the inflated surface, it may need 
   // to turn and go in an anterior direction briefly which causes
   // this drawing method to fail since the as the path is built, the 
   // distance to the end node must always decrease.
   //
   drawBorderMetric(ellipsoidSurface, //inflatedSurface,
                    BrainModelSurfaceROICreateBorderUsingMetricShape::MODE_FOLLOW_MOST_NEGATIVE,
                    curvatureShapeFile,
                    curvatureInflatedMeanColumnNumber,
                    sfSecondaryLandmarkName,
                    sfInferiorBranchBeginNodeNumber,
                    sfDorsalNodeNumber, // JWH 10 April sfPosteriorNodeNumber,
                    1.0);
                    
   //
   // Get intersection of Landmark.SF-secondary and LANDMARK.SylvianFissure
   //
   // caret_command -surface-border-intersection $INFLATED $TOPO 
   //    $OUTBORDERPROJ $OUTFOCIPROJ $OUTFOCIPROJ 
   //    LANDMARK.SF-secondary LANDMARK.SylvianFissure SF_Intersect_Superior_Inferior 3.0
   float sfInsersectSuperiorInferiorXYZ[3];
   getBorderIntersection(ellipsoidSurface, //inflatedSurface,
                         sfSecondaryLandmarkName,
                         sylvianFissureLandmarkName,
                         sfIntersectSuperiorInferiorFocusName,
                         3.0,
                         15.0,
                         1.0,
                         sfInsersectSuperiorInferiorXYZ);
                         
   
   
   
   
   //return;
   
   
   
   //
   // Delete intermediate borders ?
   //
   // if [ "$REMOVE_INTERMEDIATE_BORDERS" == "YES" ]
   // then
   //   caret_command -surface-border-delete $OUTBORDERPROJ $OUTBORDERPROJ 
   //      LANDMARK.SF-secondary
   // fi
   //
   if (DebugControl::getDebugOn() == false) {
      borderProjectionFile->removeBordersWithName(sfSecondaryLandmarkName);
   }
   
   //
   // Trim the sylvian fissure border for everything less than SF/IntSupInf
   // intersection point minus 12 in Y-axis
   //
   // caret_command -surface-border-nibbler $ELLIPSOID $TOPO 
   //    $OUTBORDERPROJ $OUTBORDERPROJ LANDMARK.SylvianFissure 
   //    LANDMARK.SylvianFissure $OUTFOCIPROJ SF_Intersect_Superior_Inferior 
   //    -less-than-y -12
   //
   nibbleBorderWithinOffset(ellipsoidSurface,
                            sylvianFissureLandmarkName,
                            sfInsersectSuperiorInferiorXYZ,
                            BORDER_NIBBLE_MODE_OFFSET_LESS_THAN_Y,
                            -12.0);
                            
   //
   // Trim any 10 mm of Z 
   //
   // caret_command -surface-border-nibbler $INFLATED $TOPO 
   //    $OUTBORDERPROJ $OUTBORDERPROJ LANDMARK.SylvianFissure 
   //    LANDMARK.SylvianFissure $OUTFOCIPROJ SF_VentralFrontalExtreme 
   //    -within-z-distance 10
   //
   nibbleBorderWithinDistance(inflatedSurface,
                              sylvianFissureLandmarkName,
                              sfVentralFrontalExtremeNodeNumber,
                              BORDER_NIBBLE_MODE_WITHIN_DISTANCE_Z,
                              10.0);
                              
   //
   // Resample the Sylvian Fissure
   //
   // caret_command -surface-border-resample $INFLATED $TOPO 
   //    $OUTBORDERPROJ $OUTBORDERPROJ 3.0 -border-name  LANDMARK.SF
   //
   resampleBorder(fiducialSurface,
                  sylvianFissureLandmarkName,
                  3.0);
}
      
/**
 * get intersection of two borders (returns true if intersection valid).
 */
bool 
BrainModelSurfaceBorderLandmarkIdentification::getBorderIntersection(
                           const BrainModelSurface* borderSurface,
                           const QString& border1Name,
                           const QString& border2Name,
                           const QString& intersectionFocusName,
                           const float intersectionTolerance,
                           float* itersectionXYZOut,
                           int* border1IntersectionLinkNumberOut,
                           int* border2IntersectionLinkNumberOut) throw (BrainModelAlgorithmException)
{
   //
   // Find the borders
   //
   BorderProjection* bp1 = borderProjectionFile->getLastBorderProjectionByName(border1Name);
   if (bp1 == NULL) {
      throw BrainModelAlgorithmException("unable to find border named "
                             + border1Name
                             + " when finding intersection of "
                             + border1Name
                             + " and "
                             + border2Name);
   }
   BorderProjection* bp2 = borderProjectionFile->getLastBorderProjectionByName(border2Name);
   if (bp2 == NULL) {
      throw BrainModelAlgorithmException("unable to find border named "
                             " when finding intersection of "
                             + border1Name
                             + " and "
                             + border2Name);
   }
  
   //
   // Create another border projection file containing just the two borders
   //
   BorderProjectionFile tempBorderProjFile;
   tempBorderProjFile.addBorderProjection(*bp1);
   tempBorderProjFile.addBorderProjection(*bp2);

   //
   // Unproject the two borders
   //
   BorderFile borderFile;
   BorderProjectionUnprojector unprojector;
   unprojector.unprojectBorderProjections(*(borderSurface->getCoordinateFile()),
                                          tempBorderProjFile,
                                          borderFile);
   if (borderFile.getNumberOfBorders() != 2) {
      throw BrainModelAlgorithmException("unprojection of borders failed"
                             " when finding intersection of "
                             + border1Name
                             + " and "
                             + border2Name);
   }
  
   //
   // Find intersection of two borders
   //
   const Border* border1 = borderFile.getBorder(0);
   const Border* border2 = borderFile.getBorder(1);
   int border1IntersectionLink, border2IntersectionLink;
   if (border1->intersection3D(border2,
                               intersectionTolerance,
                               border1IntersectionLink,
                               border2IntersectionLink) == false) {
      throw BrainModelAlgorithmException("no intersection within tolerance"
                             " when finding intersection of "
                             + border1Name
                             + " and "
                             + border2Name);
   }
  
   //
   // Get coordinate of intersection
   //
   float intersectionXYZ[3];
   border1->getLinkXYZ(border1IntersectionLink,
                       intersectionXYZ);
   if (itersectionXYZOut != NULL) {
      itersectionXYZOut[0] = intersectionXYZ[0];
      itersectionXYZOut[1] = intersectionXYZ[1];
      itersectionXYZOut[2] = intersectionXYZ[2];
   }
   
   //
   // Add a focus at border intersection
   //
   if (intersectionFocusName.isEmpty() == false) {
      addFocusAtXYZ(borderSurface,
                    intersectionFocusName,
                    intersectionXYZ);
   }
    
   if (border1IntersectionLinkNumberOut != NULL) {
      *border1IntersectionLinkNumberOut = border1IntersectionLink;
   }
   if (border2IntersectionLinkNumberOut != NULL) {
      *border2IntersectionLinkNumberOut = border2IntersectionLink;
   }

   return true;
}
      
/**
 * get intersection of two borders (returns true if intersection valid).
 */
bool 
BrainModelSurfaceBorderLandmarkIdentification::getBorderIntersection(
                           const BrainModelSurface* borderSurface,
                           const QString& border1Name,
                           const QString& border2Name,
                           const QString& intersectionFocusName,
                           const float intersectionToleranceMinimum,
                           const float intersectionToleranceMaximum,
                           const float intersectionToleranceStep,
                           float itersectionXYZOut[3]) throw (BrainModelAlgorithmException)
{
   QString errorMessage;
   
   for (float tolerance = intersectionToleranceMinimum;
        tolerance <= intersectionToleranceMaximum;
        tolerance += intersectionToleranceStep) {
      bool valid = false;
      try {
         valid = getBorderIntersection(borderSurface,
                                       border1Name,
                                       border2Name,
                                       intersectionFocusName,
                                       tolerance,
                                       itersectionXYZOut);
         if (valid) {
            return true;
         }
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessage = e.whatQString();
      }
   }
   
   throw BrainModelAlgorithmException(errorMessage);
}
      
/**
 * merge borders (returned border was added to border projection file).
 */
BorderProjection* 
BrainModelSurfaceBorderLandmarkIdentification::mergeBorders(
                               const QString& outputBorderName,
                               const QString& border1Name,
                               const QString& border2Name,
                               const bool deleteInputBordersFlag,
                               const bool closeBorderFlag,
                               const BrainModelSurface* smoothingSurface,
                               const int smoothingIterations,
                               const int smoothingNeighbors) throw (BrainModelAlgorithmException)
{
   BorderProjection* border1 =
      borderProjectionFile->getFirstBorderProjectionByName(border1Name);
   if (border1 == NULL) {
      throw BrainModelAlgorithmException("Border named "
                                         + border1Name
                                         + " missing when merging to create border named "
                                         + outputBorderName);
   }
   BorderProjection* border2 =
      borderProjectionFile->getFirstBorderProjectionByName(border2Name);
   if (border2 == NULL) {
      throw BrainModelAlgorithmException("Border named "
                                         + border2Name
                                         + " missing when merging to create border named "
                                         + outputBorderName);
   }
   
   //
   // Merge the border
   //
   BorderProjection outputBorderProjection(outputBorderName);
   outputBorderProjection.append(*border1);
   const int junctionIndex = outputBorderProjection.getNumberOfLinks();
   outputBorderProjection.append(*border2);
   
   //
   // Delete input borders
   //
   if (deleteInputBordersFlag) {
      borderProjectionFile->removeBordersWithName(border1Name);
      borderProjectionFile->removeBordersWithName(border2Name);
   }
   
   //
   // Smooth the border
   //
   if ((smoothingSurface != NULL) &&
       (smoothingIterations > 0) &&
       (smoothingNeighbors > 0)) {
      //
      // Put new border projection in temporary border projection file
      //
      BorderProjectionFile tempBorderProjectionFile;
      tempBorderProjectionFile.addBorderProjection(outputBorderProjection);
      
      //
      // Unproject the border
      //
      BorderProjectionUnprojector unprojector;
      BorderFile tempBorderFile;
      unprojector.unprojectBorderProjections(*smoothingSurface->getCoordinateFile(),
                                             tempBorderProjectionFile,
                                             tempBorderFile);
                                             
      //
      // Get border
      //
      if (tempBorderFile.getNumberOfBorders() <= 0) {
         throw BrainModelAlgorithmException("Border unprojection error when merging "
                                + border1Name
                                + " and "
                                + border2Name 
                                + " into "
                                + outputBorderName);
      }
      
      //
      // Get links for smoothing
      //
      Border* b = tempBorderFile.getBorder(0);
      const int numLinks = b->getNumberOfLinks();
      std::vector<bool> smoothFlags(numLinks, true);
      if ((junctionIndex >= 0) &&
          (junctionIndex < numLinks)) {
         smoothFlags[junctionIndex] = false;
      }

      //
      // Smooth the border
      //
      b->smoothBorderLinks(smoothingIterations,
                           closeBorderFlag,
                           &smoothFlags);

      //
      // Reproject the border
      //
      tempBorderProjectionFile.clear();
      BorderFileProjector projector(inflatedSurface, true);
      projector.projectBorderFile(&tempBorderFile,
                                  &tempBorderProjectionFile,
                                  NULL);
                                  
      //
      // Add border projection to output file
      //
      if (tempBorderProjectionFile.getNumberOfBorderProjections() <= 0) {
         throw BrainModelAlgorithmException("Border reprojection error when merging "
                                + border1Name
                                + " and "
                                + border2Name 
                                + " into "
                                + outputBorderName);
      }
      borderProjectionFile->addBorderProjection(
                     *tempBorderProjectionFile.getBorderProjection(0));
   }
   else {
      //
      // Add border projection to output file
      //
      borderProjectionFile->addBorderProjection(outputBorderProjection);
   }
   
   return borderProjectionFile->getBorderProjection(
      borderProjectionFile->getNumberOfBorderProjections() - 1);
}

/**
 * merge borders (returned border was added to border projection file).
 */
BorderProjection* 
BrainModelSurfaceBorderLandmarkIdentification::mergeBorders(
                               const QString& outputBorderName,
                               const std::vector<QString>& borderNames,
                               const bool deleteInputBordersFlag,
                               const bool closeBorderFlag,
                               const BrainModelSurface* smoothingSurface,
                               const int smoothingIterations,
                               const int smoothingNeighbors) throw (BrainModelAlgorithmException)
{
   BorderProjection outputBorderProjection(outputBorderName);
   const int numBorders = static_cast<int>(borderNames.size());
   std::vector<int> junctionIndices;
   std::vector<BorderProjection*> borders;
   for (int i = 0; i < numBorders; i++) {
      BorderProjection* bp = borderProjectionFile->getFirstBorderProjectionByName(borderNames[i]);
      if (bp == NULL) {
         throw BrainModelAlgorithmException("Border named "
                                         + borderNames[i]
                                         + " missing when merging to create border named "
                                         + outputBorderName);
      }
      outputBorderProjection.append(*bp);
      junctionIndices.push_back(outputBorderProjection.getNumberOfLinks());
   }
   
   //
   // Delete input borders
   //
   if (deleteInputBordersFlag) {
      for (int i = 0; i < numBorders; i++) {
         borderProjectionFile->removeBordersWithName(borderNames[i]);
      }
   }
   
   //
   // Smooth the border
   //
   if ((smoothingSurface != NULL) &&
       (smoothingIterations > 0) &&
       (smoothingNeighbors > 0)) {
      //
      // Put new border projection in temporary border projection file
      //
      BorderProjectionFile tempBorderProjectionFile;
      tempBorderProjectionFile.addBorderProjection(outputBorderProjection);
      
      //
      // Unproject the border
      //
      BorderProjectionUnprojector unprojector;
      BorderFile tempBorderFile;
      unprojector.unprojectBorderProjections(*smoothingSurface->getCoordinateFile(),
                                             tempBorderProjectionFile,
                                             tempBorderFile);
                                             
      //
      // Get border
      //
      if (tempBorderFile.getNumberOfBorders() <= 0) {
            throw BrainModelAlgorithmException(
                                " Error getting border after projection when assembling "
                                + outputBorderName);
      }
      
      //
      // Get links for smoothing
      //
      Border* b = tempBorderFile.getBorder(0);
      const int numLinks = b->getNumberOfLinks();
      std::vector<bool> smoothFlags(numLinks, true);
      for (int i = 0; i < numBorders - 1; i++) {
         smoothFlags[junctionIndices[i]] = false;
      }

      //
      // Smooth the border
      //
      b->smoothBorderLinks(smoothingIterations,
                           closeBorderFlag,
                           &smoothFlags);

      //
      // Reproject the border
      //
      tempBorderProjectionFile.clear();
      BorderFileProjector projector(inflatedSurface, true);
      projector.projectBorderFile(&tempBorderFile,
                                  &tempBorderProjectionFile,
                                  NULL);
                                  
      //
      // Add border projection to output file
      //
      if (tempBorderProjectionFile.getNumberOfBorderProjections() <= 0) {
         throw BrainModelAlgorithmException("Border reprojection error when merging "
                                " into "
                                + outputBorderName);
      }
      borderProjectionFile->addBorderProjection(
                     *tempBorderProjectionFile.getBorderProjection(0));
   }
   else {
      //
      // Add border projection to output file
      //
      borderProjectionFile->addBorderProjection(outputBorderProjection);
   }
   
   return borderProjectionFile->getBorderProjection(
      borderProjectionFile->getNumberOfBorderProjections() - 1);
}

/**
 * identify the superior temporal gyrus.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifySuperiorTemporalGyrus() throw (BrainModelAlgorithmException)
{
   //
   // Names of items
   //
   const QString sfStsAntLandmarkBorderName("LANDMARK.SF_STSant");
   const QString stgStsInflateVentralFocusName("STS_Inflated_Ventral");
   const QString stgStsInflatedDorsalFocusName("STS_Inflated_Dorsal"); 
   const QString stgTemporalPoleFocusName("TemporalPole"); 
   const QString stgPosteriorFocusName("STG-posterior"); 
   const QString stgCesLimitFocusName("STG-CES-limit"); 
   const QString stgDorsalLandmarkFocusName("STG-dorsal-Landmark"); 
   const QString stgVentralLandmarkFocusName("STG-ventral-Landmark");
   const QString stgGenericColorName("STG");
   const QString stgRoiFileName(createFileName("STG",
                                SpecFile::getRegionOfInterestFileExtension()));
   const QString stsRoiFileName(createFileName("STS",
                                SpecFile::getRegionOfInterestFileExtension()));
   const QString stgGyralRoiFileName(createFileName("GyralSTG",
                                SpecFile::getRegionOfInterestFileExtension()));
   const QString stgGyralPaintName("GYRAL.STG");
   
   //
   // Remove ROI files
   //
   QFile::remove(stgRoiFileName);
   QFile::remove(stsRoiFileName);
   QFile::remove(stgGyralRoiFileName);
   
   // caret_command -color-file-add-color $FOCICOLOR $FOCICOLOR
   //    STS_Inflated_Ventral 0 150 255 -point-size 3 -symbol SPHERE
   addFocusColor(stgStsInflateVentralFocusName, 0, 150, 255);

   // caret_command -color-file-add-color $FOCICOLOR $FOCICOLOR
   //    STS_Inflated_Dorsal 0 150 255 -point-size 3 -symbol SPHERE
   addFocusColor(stgStsInflatedDorsalFocusName, 0, 150, 255);

   // caret_command -color-file-add-color $FOCICOLOR $FOCICOLOR
   //    TemporalPole 100 0 200 -point-size 3 -symbol SPHERE
   addFocusColor(stgTemporalPoleFocusName, 100, 0, 200);

   // caret_command -color-file-add-color $FOCICOLOR $FOCICOLOR
   //    STG-posterior 0 150 255 -point-size 3 -symbol SPHERE
   addFocusColor(stgPosteriorFocusName, 0, 150, 255);

   // caret_command -color-file-add-color $FOCICOLOR $FOCICOLOR
   //    STG-ventral 0 150 255 -point-size 3 -symbol SPHERE
   addFocusColor(stgVentralLandmarkFocusName, 0, 150, 255);

   // caret_command -color-file-add-color $FOCICOLOR $FOCICOLOR
   //    STG 100 255 150 -point-size 3 -symbol SPHERE
   addFocusColor(stgGenericColorName, 100, 255, 150);


   //
   // Delete foci
   //
   // caret_command -surface-foci-delete $OUTFOCIPROJ $OUTFOCIPROJ 
   //   STS_Inflated_Ventral STS_Inflated_Dorsal TemporalPole STG-posterior 
   //   STG-CES-limit STG-dorsal-Landmark STG-ventral-Landmark
   fociProjectionFile->deleteCellProjectionsWithName(stgStsInflateVentralFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(stgStsInflatedDorsalFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(stgTemporalPoleFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(stgPosteriorFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(stgCesLimitFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(stgDorsalLandmarkFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(stgVentralLandmarkFocusName);

   //
   // Delete the border
   //
   // caret_command -surface-border-delete $OUTBORDERPROJ LANDMARK.SF_STSant
   //
   borderProjectionFile->removeBordersWithName(sfStsAntLandmarkBorderName);
   
   //
   // Add border color
   //
   // caret_command -color-file-add-color $BORDERCOLOR $BORDERCOLOR 
   //   LANDMARK.SF_STSant 255 0 187 -point-size 3 -symbol SPHERE
   //
   borderColorFile->addColor(sfStsAntLandmarkBorderName,
                             255, 0, 187, 255,
                             3, 1,
                             ColorFile::ColorStorage::SYMBOL_SPHERE);
   
                           
   // 
   // Create an ROI of the Superior Temporal Sulcus
   // 
   // caret_command -surface-region-of-interest-selection $INFLATED $TOPO 
   //   Human.$SUBJECT.$HEM.STG.roi Human.$SUBJECT.$HEM.STG.roi 
   //   -paint Human.$SUBJECT.$HEM.SulcalID.paint "Sulcus ID" SUL.STS NORMAL
   //
   BrainModelSurfaceROINodeSelection stsROI(brainSet);
   QString errorMessage = stsROI.selectNodesWithPaint(
                           BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                           inflatedSurface,
                           paintFile,
                           paintFileSulcusIdColumnNumber,
                           "SUL.STS");
   if (errorMessage.isEmpty() == false) {
    throw BrainModelAlgorithmException(errorMessage);
   }
   saveRoiToFile(stsROI, stsRoiFileName);
   
   //
   // Get the extent of the ROI
   //
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   stsROI.getNodesWithMinMaxXYZValues(inflatedSurface,
                                    mostMedialXNode,
                                    mostLateralXNode,
                                    minXNode, 
                                    maxXNode,
                                    minYNode,
                                    maxYNode,
                                    minZNode,
                                    maxZNode,
                                    absMinXNode,
                                    absMaxXNode,
                                    absMinYNode,
                                    absMaxYNode,
                                    absMinZNode,
                                    absMaxZNode);

   //
   // Get node number and place foci at Z-Min and Z-Max of the ROI
   //
   // caret_command -surface-place-foci-at-limits $FIDUCIAL $INFLATED $TOPO 
   //    Human.$SUBJECT.$HEM.STG.roi $OUTFOCIPROJ $OUTFOCIPROJ 
   //    -z-min STS_Inflated_Ventral
   //    -z-max STS_Inflated_Dorsal
   const int stsInflatedVentralNodeNumber = minZNode;
   addFocusAtNode(stgStsInflateVentralFocusName,
                  stsInflatedVentralNodeNumber);
   const int stsInflatedDorsalNodeNumber  = maxZNode;
   addFocusAtNode(stgStsInflatedDorsalFocusName,
                  stsInflatedDorsalNodeNumber);
   
   //
   // 
   //
   // caret_command -surface-place-focus-at-extremum $FIDUCIAL $INFLATED $TOPO 
   //    $OUTFOCIPROJ STS_Inflated_Ventral $OUTFOCIPROJ TemporalPole 
   //    Y-POS 100000.0 100000.0 100000.0
   temporalPoleNodeNumber =
      addFocusAtExtremum(inflatedSurface,
                         stsInflatedVentralNodeNumber,
                         BrainModelSurfaceFindExtremum::DIRECTION_Y_POSITIVE,
                         100000.0,
                         100000.0,
                         100000.0,
                         stgTemporalPoleFocusName);
   fiducialSurface->getCoordinateFile()->getCoordinate(temporalPoleNodeNumber,
                                                       fiducialSurfaceTemporalPoleXYZ);
   inflatedSurface->getCoordinateFile()->getCoordinate(temporalPoleNodeNumber,
                                                       inflatedSurfaceTemporalPoleXYZ);
                                                       
   //
   // Create a matrix for rotating surface +/- 45 degrees
   //
   // #
   // # Left surface needs to rotate POSITIVE 45 degrees
   // #
   // if [ $LEFT_FLAG -eq 1 ]
   // then
   //   caret_command -transformation-matrix-create TempRotateYMinus45.matrix 
   //        TempRotateYMinus45.matrix RotateYMinus45 -delete-all-matrices-from-file
   //         -matrix-comment "rotate minus 45 degrees about Y-axis" 
   //         -rotate  0.0  45.0  0.0
   // else
   //   caret_command -transformation-matrix-create TempRotateYMinus45.matrix 
   //        TempRotateYMinus45.matrix RotateYMinus45 -delete-all-matrices-from-file
   //         -matrix-comment "rotate minus 45 degrees about Y-axis" 
   //         -rotate  0.0  -45.0  0.0
   // fi
   TransformationMatrix matrix45;
   if (leftHemisphereFlag) {
      matrix45.rotateZ(0.0);
      matrix45.rotateX(0.0);
      matrix45.rotateY(45.0);
   }
   else {
      matrix45.rotateZ(0.0);
      matrix45.rotateX(0.0);
      matrix45.rotateY(-45.0);
   }

   //
   // Create a new inflated surface
   //
   // caret_command -surface-apply-transformation-matrix $INFLATED $TOPO 
   //    temp.rotatedY45.coord -matrix-file  TempRotateYMinus45.matrix RotateYMinus45
   //
   BrainModelSurface inflatedRotate45Surface(*inflatedSurface);
   inflatedRotate45Surface.applyTransformationMatrix(matrix45);
   
   //
   // Find the extremum by moving in the Z-direction which should
   // end at the posterior part of the sylvian fissure
   //
   // caret_command -surface-place-focus-at-extremum $FIDUCIAL temp.rotatedY45.coord 
   //    $TOPO $OUTFOCIPROJ TemporalPole $OUTFOCIPROJ STG-posterior 
   //    Z-POS 100000.0 100000.0 100000.0 -create-roi-from-path 
   //    Human.$SUBJECT.$HEM.STG.roi
   //
   BrainModelSurfaceROINodeSelection stgROI(brainSet);
   //const int stgPosteriorNodeNumber =
      addFocusAtExtremum(&inflatedRotate45Surface,
                         temporalPoleNodeNumber,
                         BrainModelSurfaceFindExtremum::DIRECTION_Z_POSITIVE,
                         100000.0,
                         100000.0,
                         100000.0,
                         stgPosteriorFocusName,
                         &stgROI);
   saveRoiToFile(stgROI, stgRoiFileName);
     
   //
   // Get extents of STG roi
   //
   stgROI.getNodesWithMinMaxXYZValues(inflatedSurface,
                                      mostMedialXNode,
                                      mostLateralXNode,
                                      minXNode, 
                                      maxXNode,
                                      minYNode,
                                      maxYNode,
                                      minZNode,
                                      maxZNode,
                                      absMinXNode,
                                      absMaxXNode,
                                      absMinYNode,
                                      absMaxYNode,
                                      absMinZNode,
                                      absMaxZNode);

   //
   // Limit the extent of the STG ROI
   //   
   // caret_command -surface-region-of-interest-selection $INFLATED $TOPO 
   //    Human.$SUBJECT.$HEM.STG.roi Human.$SUBJECT.$HEM.STG.roi -limit-y-min-focus 
   //    $OUTFOCIPROJ CeS-ventral -limit-y-max-focus $OUTFOCIPROJ TemporalPole 
   //    -limit-z-min-focus $OUTFOCIPROJ TemporalPole
   const float stgROIExtent[6] = {
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      inflatedSurfaceCentralSulcusVentralTipXYZ[1],
      inflatedSurfaceTemporalPoleXYZ[1],
      inflatedSurfaceTemporalPoleXYZ[2],
      std::numeric_limits<float>::max(),
   };
   stgROI.limitExtent(inflatedSurface, stgROIExtent);
   saveRoiToFile(stgROI, stgRoiFileName);

   //
   // Get extents of STG roi
   //
   stgROI.getNodesWithMinMaxXYZValues(inflatedSurface,
                                      mostMedialXNode,
                                      mostLateralXNode,
                                      minXNode, 
                                      maxXNode,
                                      minYNode,
                                      maxYNode,
                                      minZNode,
                                      maxZNode,
                                      absMinXNode,
                                      absMaxXNode,
                                      absMinYNode,
                                      absMaxYNode,
                                      absMinZNode,
                                      absMaxZNode);
   //
   // Place a focus at the Y-Min of the ROI which is STG-CES-limit
   //
   // caret_command -surface-place-foci-at-limits $FIDUCIAL $INFLATED $TOPO 
   //    Human.$SUBJECT.$HEM.STG.roi $OUTFOCIPROJ $OUTFOCIPROJ -y-min STG-CES-limit
   //
   const int stgCesLimitNodeNumber = minYNode;
   addFocusAtNode(stgCesLimitFocusName,
                  stgCesLimitNodeNumber);
   
   //
   // Draw a border from STG-CES-limit to the temporal pole
   //
   // caret_command -surface-border-draw-geodesic $INFLATED $TOPO $OUTFOCIPROJ 
   //    STG-CES-limit TemporalPole Human.$SUBJECT.$HEM.STG.roi 
   //    $OUTBORDERPROJ $OUTBORDERPROJ LANDMARK.SF_STSant 1.0
   if (DebugControl::getDebugOn()) {
      std::cout << "STG Landmark CeS Limit node: "
                << stgCesLimitNodeNumber
                << std::endl;
      std::cout << "STG Landmark Temporal Pole node: "
                << temporalPoleNodeNumber
                << std::endl;
   }
   drawBorderGeodesic(inflatedSurface,
                      &stgROI,
                      sfStsAntLandmarkBorderName,
                      stgCesLimitNodeNumber,
                      temporalPoleNodeNumber,
                      3.0);
   
   //
   // Resample to coarse spacing
   //
   resampleBorder(fiducialSurface, sfStsAntLandmarkBorderName, 8.0);
   BorderProjection* stgBorder = 
         borderProjectionFile->getFirstBorderProjectionByName(sfStsAntLandmarkBorderName);   
   
   //
   // Move each link in the border so that it moves up onto the
   // top of the ridge which should be the true crown of the STG
   //
   const int numLinks = stgBorder->getNumberOfLinks();
   const int halfNumLinks = numLinks / 2;
   for (int i = 1; i < (numLinks - 1); i++) {
      BorderProjectionLink* bpl = stgBorder->getBorderProjectionLink(i);
      int section;
      int vertices[3];
      float areas[3];
      float radius;
      bpl->getData(section, vertices, areas, radius);
      
      const float xMovement = (i >= halfNumLinks) ? 10.0 : 4.0;
      const int nodeNumber = 
         addFocusAtExtremum(fiducialSurface,
                            vertices[0],
                            BrainModelSurfaceFindExtremum::DIRECTION_Z_POSITIVE,
                            xMovement,
                            1.0,
                            10000.0,
                            "");
      vertices[0] = nodeNumber;
      vertices[1] = nodeNumber;
      vertices[2] = nodeNumber;
      areas[0] = 1.0;
      areas[1] = 0.0;
      areas[2] = 0.0;
      bpl->setData(section, vertices, areas, radius);
   }
      
   resampleBorder(fiducialSurface, sfStsAntLandmarkBorderName, 3.0); 
   
   //--------------------------------------------------------------------------
   //
   // The following code creates the STG Paint
   //
                      
   //
   // Place foci at ends of STG border
   //
   // caret_command -surface-border-link-to-focus $FIDUCIAL $TOPO $OUTBORDERPROJ 
   //    LANDMARK.SF_STSant $OUTFOCIPROJ $OUTFOCIPROJ -first-link STG-dorsal-Landmark
   // caret_command -surface-border-link-to-focus $FIDUCIAL $TOPO $OUTBORDERPROJ 
   //    LANDMARK.SF_STSant $OUTFOCIPROJ $OUTFOCIPROJ -last-link STG-ventral-Landmark
   const BorderProjection* stgBP = 
      borderProjectionFile->getFirstBorderProjectionByName(sfStsAntLandmarkBorderName);
   if (stgBP != NULL) {
      addFocusAtBorderLink(stgBP,
                           0,
                           stgDorsalLandmarkFocusName);
      addFocusAtBorderLink(stgBP,
                           stgBP->getNumberOfLinks() - 1,
                           stgVentralLandmarkFocusName);
   }
   
   //
   // Create an ROI that identifies the STG
   //
   // caret_command -surface-region-of-interest-selection $INFLATED $TOPO 
   //    Human.$SUBJECT.$HEM.STG.roi Human.$SUBJECT.$HEM.STG.roi 
   //    -dilate-paint Human.$SUBJECT.$HEM.SulcalID.paint "Sulcus ID" SUL.SF 5 
   //    -dilate-paint Human.$SUBJECT.$HEM.SulcalID.paint "Sulcus ID" GYRAL 5 
   //    -limit-y-min-focus $OUTFOCIPROJ CeS-ventral 
   //    -limit-y-max-focus $OUTFOCIPROJ TemporalPole 
   //    -limit-z-min-focus $OUTFOCIPROJ TemporalPole
   //
   BrainModelSurfaceROINodeSelection stgGyralROI(stgROI);
   stgGyralROI.dilatePaintConstrained(inflatedSurface,
                                      paintFile,
                                      paintFileSulcusIdColumnNumber,
                                      "SUL.SF",
                                      5);
   stgGyralROI.dilatePaintConstrained(inflatedSurface,
                                      paintFile,
                                      paintFileSulcusIdColumnNumber,
                                      "GYRAL",
                                      5);
   const float stgGyralROIExtent[6] = {
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      inflatedSurfaceCentralSulcusVentralTipXYZ[1],
      inflatedSurfaceTemporalPoleXYZ[1],
      inflatedSurfaceTemporalPoleXYZ[2],
      std::numeric_limits<float>::max(),
   };
   stgGyralROI.limitExtent(inflatedSurface,
                           stgGyralROIExtent);
   saveRoiToFile(stgGyralROI, stgGyralRoiFileName);
   
   //
   // Color for Gyral.STG
   //
   areaColorFile->addColor(stgGyralPaintName,
                           200, 0, 255);
   vocabularyFile->addVocabularyEntry(
      VocabularyFile::VocabularyEntry(stgGyralPaintName, "Superior Temporal Gyrus"));
   
   //
   // Use the gryal ROI to paint the STG
   //
   // ###caret_command -paint-assign-to-nodes Human.$SUBJECT.$HEM.SulcalID.paint Human.$SUBJECT.$HEM.SulcalID_withSTG.paint "Sulcus ID" GYRAL.STG -assign-from-roi-file Human.$SUBJECT.$HEM.STG.roi
   // caret_command -paint-assign-to-nodes Human.$SUBJECT.$HEM.SulcalID.paint 
   //    $SULCAL_ID_PAINT "Sulcus ID" GYRAL.STG -assign-from-roi-file 
   //    Human.$SUBJECT.$HEM.STG.roi
   NodeRegionOfInterestFile roiStgFile;
   stgGyralROI.setRegionOfInterestIntoFile(roiStgFile);
   try {
      paintFile->assignNodesFromROIFile(paintFileSulcusIdColumnNumber,
                                        roiStgFile,
                                        stgGyralPaintName,
                                        true);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }

   const float fullExtent[6] = {
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
   };
   paintFile->dilatePaintID(fiducialSurface->getTopologyFile(),
                            fiducialSurface->getCoordinateFile(),
                            paintFileSulcusIdColumnNumber,
                            5,
                            paintFile->getPaintIndexFromName(stgGyralPaintName),
                            paintFile->getPaintIndexFromName("???"),
                            fullExtent);
}

/**
 * identify the calcarine sulcus.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyCalcarineSulcus() throw (BrainModelAlgorithmException)
{
   //
   // Names of items
   //
   const QString calcarinePosteriorFocusName("CaS-posterior");
   const QString calcarineAnteriorFocusName("CaS-anterior");
   const QString calcarinePosteriorExtremeFocusName("CaS-PosteriorExtreme");
   const QString calcarineRoiFileName(createFileName("Cas",
                                      SpecFile::getRegionOfInterestFileExtension()));
   const QString calcarineStringentRoiFileName(createFileName("CaS_Stringent",
                                      SpecFile::getRegionOfInterestFileExtension()));
   
   //
   // Remove ROI files
   //
   QFile::remove(calcarineRoiFileName);
   QFile::remove(calcarineStringentRoiFileName);

   //
   // Add some foci colors
   //
   // caret_command -color-file-add-color $FOCICOLOR $FOCICOLOR CaS-posterior 
   //    0 150 255 -point-size 3 -symbol SPHERE
   // caret_command -color-file-add-color $FOCICOLOR $FOCICOLOR CaS-anterior 
   //    0 150 255 -point-size 3 -symbol SPHERE
   // caret_command -color-file-add-color $FOCICOLOR $FOCICOLOR CaS-PosteriorExtreme 
   //    0 255 0 -point-size 3 -symbol SPHERE
   //
   addFocusColor(calcarinePosteriorFocusName, 0, 150, 255);
   addFocusColor(calcarineAnteriorFocusName, 0, 155, 255);
   addFocusColor(calcarinePosteriorExtremeFocusName, 0, 255, 0);
   
   
   //
   // Add a border color for the landmark
   //
   // caret_command -color-file-add-color $BORDERCOLOR $BORDERCOLOR 
   //    LANDMARK.CalcarineSulcus 246 114 0
   //
   borderColorFile->addColor(calcarineSulcusLandmarkName,
                             246, 114, 0, 255,
                             2, 1,
                             ColorFile::ColorStorage::SYMBOL_SPHERE);
   
   //
   // Delete any previously created foci
   //
   // caret_command -surface-foci-delete $OUTFOCIPROJ CaS-anterior CaS-posterior
   //     CaS-PosteriorExtreme CaS-anterior-Landmark CaS-posterior-Landmark
   //
   fociProjectionFile->deleteCellProjectionsWithName(calcarinePosteriorFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(calcarineAnteriorFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(calcarinePosteriorExtremeFocusName);
   
   //
   // Delete previously created border
   //
   // caret_command -surface-border-delete $OUTBORDERPROJ LANDMARK.CalcarineSulcus
   //
   borderProjectionFile->removeBordersWithName(calcarineSulcusLandmarkName);
   
   //
   // Create an ROI of the Calcarine Suclus with inflated surface curvature
   //
   // caret_command -surface-region-of-interest-selection $INFLATED $TOPO 
   //    Human.$SUBJECT.$HEM.CaS.roi Human.$SUBJECT.$HEM.CaS.roi 
   //    -paint Human.$SUBJECT.$HEM.SulcalID.paint "Sulcus ID" SUL.CaS NORMAL 
   //    -shape $SHAPE_INFLATED "Folding (Mean Curvature) INFLATED" -100.0 -0.07 AND
   BrainModelSurfaceROINodeSelection calcarineROI(brainSet);
   QString errorMessage = calcarineROI.selectNodesWithPaint(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                              inflatedSurface,
                              paintFile,
                              paintFileSulcusIdColumnNumber,
                              "SUL.CaS");
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   errorMessage = calcarineROI.selectNodesWithSurfaceShape(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND,
                              inflatedSurface,
                              curvatureShapeFile,
                              curvatureInflatedMeanColumnNumber,
                              -100.0,
                              -0.07);
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   saveRoiToFile(calcarineROI,
                 calcarineRoiFileName);
   
   //                               
   // Get nodes at min/max geometric values of the ROI
   //
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   calcarineROI.getNodesWithMinMaxXYZValues(inflatedSurface,
                                            mostMedialXNode,
                                            mostLateralXNode,
                                            minXNode, 
                                            maxXNode,
                                            minYNode,
                                            maxYNode,
                                            minZNode,
                                            maxZNode,
                                            absMinXNode,
                                            absMaxXNode,
                                            absMinYNode,
                                            absMaxYNode,
                                            absMinZNode,
                                            absMaxZNode);

   //
   // Identify some limits of the Calcarine Sulcus ROI
   //
   // caret_command -surface-place-foci-at-limits $FIDUCIAL $INFLATED $TOPO 
   //    Human.$SUBJECT.$HEM.CaS.roi $OUTFOCIPROJ $OUTFOCIPROJ -y-max CaS-anterior 
   //    -y-min CaS-posterior
   //
   calcarineAnteriorNodeNumber = maxYNode;
   addFocusAtNode(calcarineAnteriorFocusName,
                  calcarineAnteriorNodeNumber);
   const int calcarinePosteriorNodeNumber = minYNode;
   addFocusAtNode(calcarinePosteriorFocusName,
                  calcarinePosteriorNodeNumber);
   if (DebugControl::getDebugOn()) {
      std::cout << "Calcarine Anterior Node Number: "
                << calcarineAnteriorNodeNumber
                << std::endl;
      std::cout << "Calcarine Posterior Node Number: "
                << calcarinePosteriorNodeNumber
                << std::endl;
   }
   
   //
   // Create a more stringent ROI of the Calcarine Suclus with fiducial curvature
   // Try inflated curvature, JWH 24 March 2008
   //
   // caret_command -surface-region-of-interest-selection $INFLATED $TOPO 
   //    Human.$SUBJECT.$HEM.CaS_Stringent.roi Human.$SUBJECT.$HEM.CaS_Stringent.roi 
   //    -paint Human.$SUBJECT.$HEM.SulcalID.paint "Sulcus ID" SUL.CaS NORMAL 
   //    -shape $SHAPE_OUT "Folding (Mean Curvature)" -100.0 -0.16 AND
   //
   BrainModelSurfaceROINodeSelection calcarineStringentROI(brainSet);
   errorMessage = calcarineStringentROI.selectNodesWithPaint(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                              inflatedSurface,
                              paintFile,
                              paintFileSulcusIdColumnNumber,
                              "SUL.CaS");
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   errorMessage = calcarineStringentROI.selectNodesWithSurfaceShape(
                              BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND,
                              inflatedSurface,
                              curvatureShapeFile,
                              curvatureInflatedMeanColumnNumber, //curvatureFiducialMeanColumnNumber,
                              -100.0,
                              -0.16);
   if (errorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   saveRoiToFile(calcarineStringentROI,
                 calcarineStringentRoiFileName);

   
   //
   // Try drawing the border with geodesic method
   //
   const QString testNameGeo(calcarineSulcusLandmarkName); 
   
   //borderColorFile->addColor(testNameGeo,
   //                        0, 100, 220, 255,
   //                        3, 1,
   //                        ColorFile::ColorStorage::SYMBOL_SPHERE);
    
   const QString casSegment2("CaS-Segment2");
   drawBorderGeodesic(inflatedSurface,
                      &calcarineStringentROI,
                      casSegment2,
                      calcarinePosteriorNodeNumber,
                      calcarineAnteriorNodeNumber,
                      2.0);

   // caret_command -surface-place-focus-at-extremum $FIDUCIAL $INFLATED $TOPO 
   //    $OUTFOCIPROJ CaS-posterior $OUTFOCIPROJ CaS-PosteriorExtreme Y-NEG 
   //    100000.0 100000.0 100000.0
   //
   calcarinePosteriorExtremeNodeNumber =
      addFocusAtExtremum(inflatedSurface,
                         calcarinePosteriorNodeNumber,
                         BrainModelSurfaceFindExtremum::DIRECTION_Y_NEGATIVE,
                         100000.0,
                         100000.0,
                         100000.0,
                         calcarinePosteriorExtremeFocusName);

   //
   // All nodes in ROI
   //
   BrainModelSurfaceROINodeSelection allNodesCaS(brainSet);
   allNodesCaS.selectAllNodes(inflatedSurface);
   
   //
   // Draw border from posterior extreme to posterior
   //
   const QString casSegment1("CaS-Segment1");
   drawBorderGeodesic(inflatedSurface,
                      &allNodesCaS,
                      casSegment1,                    
                      calcarinePosteriorExtremeNodeNumber,
                      calcarinePosteriorNodeNumber,
                      2.0);
   
   mergeBorders(calcarineSulcusLandmarkName,
                casSegment1,
                casSegment2,
                true,
                false,
                inflatedSurface,
                5,
                1);
                
   //
   // Trim the calcarine at the posterior
   //
   // caret_command -surface-border-nibbler $INFLATED $TOPO $OUTBORDERPROJ 
   //    $OUTBORDERPROJ LANDMARK.CalcarineSulcus LANDMARK.CalcarineSulcus 
   //    $OUTFOCIPROJ CaS-PosteriorExtreme -within-y-distance 24
   //
   nibbleBorderWithinDistance(inflatedSurface,
                              calcarineSulcusLandmarkName,
                              calcarinePosteriorExtremeNodeNumber,
                              BORDER_NIBBLE_MODE_WITHIN_DISTANCE_Y,
                              24.0);
}

/**
 * extend the calcarine sulcus to the medial wall.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::extendCalcarineSulcusToMedialWall() throw (BrainModelAlgorithmException)      
{
   //
   // Get the border projection
   //
   BorderProjection* calcarineBorderProjection = 
      borderProjectionFile->getLastBorderProjectionByName(calcarineSulcusLandmarkName);
   if (calcarineBorderProjection == NULL) {
      throw BrainModelAlgorithmException("Unable to find calcarine border named \""
                                + calcarineSulcusLandmarkName
                                + "\" for extension of calcarine sulcus.");
   }
   const BorderProjection* medialWallProjection = 
      borderProjectionFile->getLastBorderProjectionByName(getFlattenMedialWallBorderName());
   if (medialWallProjection == NULL) {
      throw BrainModelAlgorithmException("Unable to find medial wall border named \""
                                + getFlattenMedialWallBorderName()
                                + "\" for extension of calcarine sulcus.");
   }
      
   //
   // Place the border projection in a temporary file
   //
   BorderProjectionFile tempBorderProjectionFile;
   tempBorderProjectionFile.addBorderProjection(*calcarineBorderProjection);
   tempBorderProjectionFile.addBorderProjection(*medialWallProjection);
   
   //
   // Unproject the border
   //
   BorderProjectionUnprojector unprojector;
   BorderFile tempBorderFile;
   unprojector.unprojectBorderProjections(*(inflatedSurface->getCoordinateFile()),
                                          tempBorderProjectionFile,
                                          tempBorderFile);
                                          
   //
   // Get the borders
   //
   const Border* tempCalcarineBorder = tempBorderFile.getBorderByName(calcarineSulcusLandmarkName);
   if (tempCalcarineBorder == NULL) {
      throw BrainModelAlgorithmException("Unable to find calcarine border after unprojection during calcarine extension.");
   }
   Border* tempMedialWallBorder = tempBorderFile.getBorderByName(getFlattenMedialWallBorderName());
   if (tempMedialWallBorder == NULL) {
      throw BrainModelAlgorithmException("Unable to find medial wall border after unprojection during calcarine extension.");
   }
   
   //
   // Get last link of calcarine
   //
   if (tempCalcarineBorder->getNumberOfLinks() <= 0) {
      throw BrainModelAlgorithmException("calcarine border contains no links after unprojection during calcarine extension.");
   }
   const float* calcarineLastXYZ = 
      tempCalcarineBorder->getLinkXYZ(tempCalcarineBorder->getNumberOfLinks() - 1);
   const float calcarineZ = calcarineLastXYZ[2];
   
   //
   // Find posterior link in medial wall near Y of last calcarine link
   //
   int nearestLinkNumber = -1;
   float nearestLinkDistance = 10000000.0;
   const int numMedialWallLinks = tempMedialWallBorder->getNumberOfLinks();
   for (int i = 0; i < numMedialWallLinks; i++) {
      const float* xyz = tempMedialWallBorder->getLinkXYZ(i);
      if (xyz[1] < -15.0) {
         const float dz = std::fabs(xyz[2] - calcarineZ);
         if (dz < nearestLinkDistance) {
            nearestLinkDistance = dz;
            nearestLinkNumber = i;
         }
      }
   }
   
   //
   // Was a medial wall link NOT found
   //
   if (nearestLinkNumber < 0) {
      throw BrainModelAlgorithmException("Unable to find medial wall border link near calcarine during calcarine extension.");
   }
   
   //
   // Find nearest node a little postior of medial wall
   //
   float linkXYZ[3];
   tempMedialWallBorder->getLinkXYZ(nearestLinkNumber,
                                    linkXYZ);                                    
   linkXYZ[1] -= 3.0;
   const int nodeNumber = 
      inflatedSurface->getCoordinateFile()->getCoordinateIndexClosestToPoint(
                                          linkXYZ[0], linkXYZ[1], linkXYZ[2]);
   
   //
   // Add additional link to border
   //
   const int nodeNumbers[3] = {
      nodeNumber,
      nodeNumber,
      nodeNumber
   };
   const float areas[3] = { 1.0, 0.0, 0.0 };
   BorderProjectionLink newBPL(0,
                               nodeNumbers,
                               areas,
                               0.00);
   calcarineBorderProjection->addBorderProjectionLink(newBPL);
   
   // 
   // Resample
   //
   resampleBorder(veryInflatedSurface,
                  calcarineSulcusLandmarkName,
                  2.0);
}

/**
 * identify the medial wall.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyMedialWall() throw (BrainModelAlgorithmException)
{
   // 
   // Remove existing flatten medial wall
   //
   borderProjectionFile->removeBordersWithName(getFlattenMedialWallBorderName());
      
   //
   // Create color for medial wall segments and flatten medial wall
   //
   borderColorFile->addColor("LANDMARK.MedialWall",
                             255, 0, 0);
   borderColorFile->addColor(getFlattenMedialWallBorderName(),
                             255, 50, 50);
                             
   //
   // Identify the dorsal and medial sections
   //
   identifyDorsalMedialWall();
   identifyVentralMedialWall();  // depends upon dorsal medial wall   

   //
   // Merge the dorsal and ventral segments of the landmark medial wall
   // to create the flatten medial wall
   //
   BorderProjection medialWallTempBorder(getFlattenMedialWallBorderName());
   medialWallTempBorder.append(
    *borderProjectionFile->getFirstBorderProjectionByName(
       medialWallDorsalSectionName));
   medialWallTempBorder.append(
    *borderProjectionFile->getFirstBorderProjectionByName(
       medialWallVentralSectionName));
   borderProjectionFile->addBorderProjection(medialWallTempBorder);

   //
   // Remove dorsal and ventral sections
   //
   borderProjectionFile->removeBordersWithName(medialWallDorsalSectionName);
   borderProjectionFile->removeBordersWithName(medialWallVentralSectionName);

   //
   // Resample the medial wall border
   //
   resampleBorder(fiducialSurface,
                  getFlattenMedialWallBorderName(),
                  2.0);
}

/**
 * create medial wall dorsal and ventral borders.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::createMedialWallDorsalAndVentralLandmarks() throw (BrainModelAlgorithmException)
{
   const QString landmarkMedialWallDorsalName("LANDMARK.MedialWall.DORSAL");
   const QString landmarkVentralWallDorsalName("LANDMARK.MedialWall.VENTRAL");
   
   // 
   // Remove existing flatten medial wall
   //
   borderProjectionFile->removeBordersWithName(landmarkMedialWallDorsalName);
   borderProjectionFile->removeBordersWithName(landmarkVentralWallDorsalName);
   
   //
   // The dorsal medial wall starts at the frontal cut
   // and continues to the calcarine cut.  The ventral medial
   // wall starts at the calcarine cut and continues to the
   // frontal cut
   //
   
   //
   // Flatten medial wall border, frontal cut, and calcarine cut
   //
   const BorderProjection* flattenMedialWallBorder = 
      borderProjectionFile->getFirstBorderProjectionByName(getFlattenMedialWallBorderName());
   if (flattenMedialWallBorder == NULL) {
      throw BrainModelAlgorithmException("Unable to find \""
                                         + getFlattenMedialWallBorderName()
                                         + " for creating dorsal and ventral medial wall segments.");
   }
   const BorderProjection* calcarineCutBorder = 
      borderProjectionFile->getFirstBorderProjectionByName(flattenCutCalcarineName);
   if (calcarineCutBorder == NULL) {
      throw BrainModelAlgorithmException("Unable to find \""
                                         + flattenCutCalcarineName
                                         + " for creating dorsal and ventral medial wall segments.");
   }
   const BorderProjection* frontalCutBorder = 
      borderProjectionFile->getFirstBorderProjectionByName(flattenCutFrontalName);
   if (frontalCutBorder == NULL) {
      throw BrainModelAlgorithmException("Unable to find \""
                                         + flattenCutFrontalName
                                         + " for creating dorsal and ventral medial wall segments.");
   }
   
   //
   // Find intersection of frontal and medial wall
   //
   int medialWallFrontalCutLinkNumber = 0;
   const bool frontValidFlag = 
      getBorderIntersection(inflatedSurface,
                            getFlattenMedialWallBorderName(),
                            flattenCutFrontalName,
                            "",
                            5.0,
                            NULL,
                            &medialWallFrontalCutLinkNumber,
                            NULL);
   if (frontValidFlag == false) {
      throw BrainModelAlgorithmException("Unable to find intersection of \""
                                         + getFlattenMedialWallBorderName()
                                         + "\" and \""
                                         + flattenCutFrontalName
                                         + " for creating medial wall ventral "
                                           "and dorsal landmarks.");
   }
                            
   //
   // Find intersection of calcarine and medial wall
   //
   int medialWallCalcarineCutLinkNumber = 0;
   const bool calcarineValidFlag = 
      getBorderIntersection(inflatedSurface,
                            getFlattenMedialWallBorderName(),
                            flattenCutCalcarineName,
                            "",
                            5.0,
                            NULL,
                            &medialWallCalcarineCutLinkNumber,
                            NULL);
   if (calcarineValidFlag == false) {
      throw BrainModelAlgorithmException("Unable to find intersection of \""
                                         + getFlattenMedialWallBorderName()
                                         + "\" and \""
                                         + flattenCutCalcarineName
                                         + " for creating medial wall ventral "
                                           "and dorsal landmarks.");
   }
   
   
   //
   // Create the dorsal landmark
   //
   BorderProjection dorsalBP = 
      flattenMedialWallBorder->getSubSetOfBorderProjectionLinks(
                                       medialWallFrontalCutLinkNumber,
                                       medialWallCalcarineCutLinkNumber);
   dorsalBP.setName(landmarkMedialWallDorsalName);
   dorsalBP.removeLastBorderProjectionLink();
   borderProjectionFile->addBorderProjection(dorsalBP);

   //
   // Create the ventral landmark
   //
   BorderProjection ventralBP = 
      flattenMedialWallBorder->getSubSetOfBorderProjectionLinks(
                                       medialWallCalcarineCutLinkNumber,
                                       medialWallFrontalCutLinkNumber);
   ventralBP.setName(landmarkVentralWallDorsalName);
   ventralBP.removeLastBorderProjectionLink();
   borderProjectionFile->addBorderProjection(ventralBP);
}      
      
/**
 * identify the dorsal medial wall with an alternative method.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyDorsalMedialWall() throw (BrainModelAlgorithmException)
{
   const QString ccAltColorName("CC");
   const QString ccAnteriorTopFocusName("CC-anterior");
   const QString ccDorsalFocusName("CC-dorsal");
   const QString ccAnteriorFocusName("CC-anterior");
   const QString ccPosteriorFocusName("CC-posterior");
   const QString ccMedialWallStartFocusName("CC-medial-wall-start");
   const QString ccGenuBeginningFocusName("CC-genu-beginning");
   medialWallDorsalSectionName = "MedialWallDorsalSection";
   const QString landmarkMedialWallStartToGenuBeginningBorderName("LANDMARK.MedWallStartToGenuBeginning");
   const QString landmarkMedialWallGenuBeginningToAnteriorCCBorderName("LANDMARK.MedWallGenuToAnterior");
   const QString landmarkMedialWallAnteriorToDorsalCCBorderName("LANDMARK.MedWallAnteriorToDorsal");
   const QString landmarkMedialWallDorsalToPosteriorCCBorderName("LANDMARK.MedWallAnteriorDorsalToPosterior");
   const QString corpusCallosumRoiFileName(createFileName("CC-CorpusCallosum",
                                      SpecFile::getRegionOfInterestFileExtension()));
   const QString corpusCallosumDorsalRoiFileName(createFileName("CC-CorpusCallosumDorsal",
                                      SpecFile::getRegionOfInterestFileExtension()));
   const QString sulciAroundCallosumDorsalRoiFileName(createFileName("CC-SulciAroundCorpusCallosumDorsal",
                                      SpecFile::getRegionOfInterestFileExtension()));
   
   QFile::remove(corpusCallosumRoiFileName);
   QFile::remove(corpusCallosumDorsalRoiFileName);
   QFile::remove(sulciAroundCallosumDorsalRoiFileName);
   borderProjectionFile->removeBordersWithName(medialWallDorsalSectionName);
   borderProjectionFile->removeBordersWithName(landmarkMedialWallStartToGenuBeginningBorderName);
   borderProjectionFile->removeBordersWithName(landmarkMedialWallGenuBeginningToAnteriorCCBorderName);
   borderProjectionFile->removeBordersWithName(landmarkMedialWallAnteriorToDorsalCCBorderName);
   borderProjectionFile->removeBordersWithName(landmarkMedialWallDorsalToPosteriorCCBorderName);
   fociProjectionFile->deleteCellProjectionsWithName(ccAnteriorTopFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(ccDorsalFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(ccAnteriorFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(ccPosteriorFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(ccMedialWallStartFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(ccGenuBeginningFocusName);
   

   borderColorFile->addColor(medialWallDorsalSectionName,
                             50, 255, 50);
   addFocusColor(ccAltColorName, 0, 255, 0);

   //#
   //# Create the corpus callosum volume slice
   //#
   // if [ $LEFT_FLAG -eq 1 ]
   // then
   //  caret_command -volume-create-corpus-callosum-slice $VOLUME 
   //     CorpusCallosumSlice+orig.nii.gz left
   // else
   //  caret_command -volume-create-corpus-callosum-slice $VOLUME 
   //     CorpusCallosumSlice+orig.nii.gz right
   // fi
   // caret_command -volume-smear-axis CorpusCallosumSlice+orig.nii.gz 
   //  CorpusCallosumSlice_Smear6+orig.nii.gz X 6 -1 1
   // caret_command -volume-smear-axis CorpusCallosumSlice_Smear6+orig.nii.gz 
   //  CorpusCallosumSlice_Smear12+orig.nii.gz X 6 1 1
   // caret_command -volume-smear-axis CorpusCallosumSlice_Smear12+orig.nii.gz 
   //  CorpusCallosumSlice_Smearx12_z3+orig.nii.gz Z 3 1 1
   // caret_command -volume-dilate CorpusCallosumSlice_Smearx12_z3+orig.nii.gz 
   //  Human.$SUBJECT.$HEM.CorpusCallosumSlice_SmearXZ_Dilate+orig.nii.gz 2
   //
   const float grayMatterPeak = -1.0;
   const float whiteMatterPeak = -1.0;
   VolumeFile corpusCallosumVolume;
   BrainModelVolumeSureFitSegmentation::generateCorpusCallosumSlice(
                                                             *anatomicalVolumeFile,
                                                             corpusCallosumVolume,
                                                             surfaceStructure,
                                                             grayMatterPeak,
                                                             whiteMatterPeak);
   corpusCallosumVolume.smearAxis(VolumeFile::VOLUME_AXIS_X, 6, -1, 1);
   corpusCallosumVolume.smearAxis(VolumeFile::VOLUME_AXIS_X, 6, 1, 1);
   corpusCallosumVolume.smearAxis(VolumeFile::VOLUME_AXIS_Z, 3, 1, 1);
   corpusCallosumVolume.doVolMorphOps(2, 0);

   //
   // Create the corpus callosum ROI
   //
   // caret_command -volume-map-to-surface-roi-file 
   //  Human.$SUBJECT.$HEM.CorpusCallosumSlice_SmearXZ_Dilate+orig.nii.gz 
   //  $FIDUCIAL $TOPO Human.$SUBJECT.$HEM.CorpusCallosum.roi
   NodeRegionOfInterestFile corpusCallosumRoiFile;
   corpusCallosumRoiFile.setNumberOfNodes(fiducialSurface->getNumberOfNodes());
   corpusCallosumRoiFile.assignSelectedNodesWithVolumeFile(&corpusCallosumVolume,
                                             fiducialSurface->getCoordinateFile(),
                                             fiducialSurface->getTopologyFile());
   BrainModelSurfaceROINodeSelection corpusCallosumRoi(brainSet);
   corpusCallosumRoi.getRegionOfInterestFromFile(corpusCallosumRoiFile);
   saveRoiToFile(corpusCallosumRoi,
                 corpusCallosumRoiFileName);

   //
   // Find nodes in the groove of the corpus callosum by starting 
   // with the the corpus callsoum roi
   //
   BrainModelSurfaceROINodeSelection corpusCallosumDorsalRoi(brainSet);
   float maxCurvatureValue = -0.150;
   float minSurfaceArea = 600.0; // 650.0;
   int ccdIterations = 0;
   while (/*(corpusCallosumDorsalRoi.getNumberOfNodesSelected() < 350) && */
          (corpusCallosumDorsalRoi.getSurfaceAreaOfROI(fiducialSurface) < minSurfaceArea) &&
          (ccdIterations < 5)) {
      corpusCallosumDorsalRoi = corpusCallosumRoi;
      corpusCallosumDorsalRoi.discardIslands(fiducialSurface);
      //corpusCallosumDorsalRoi.dilate(fiducialSurface, 3); //6);
      corpusCallosumDorsalRoi.selectNodesWithMetric(BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND,
                                                    fiducialSurface,
                                                    curvatureShapeFile,
                                                    curvatureFiducialSmoothedMeanColumnNumber, //curvatureFiducialMeanColumnNumber,
                                                    -50000.0,
                                                    maxCurvatureValue);
      maxCurvatureValue += 0.05;
      ccdIterations++;
   }
   saveRoiToFile(corpusCallosumDorsalRoi,
                  corpusCallosumDorsalRoiFileName);

   //
   // Remove islands with fewer than 2 nodes
   //
   corpusCallosumDorsalRoi.discardIslands(fiducialSurface, 2);
   saveRoiToFile(corpusCallosumDorsalRoi,
                  corpusCallosumDorsalRoiFileName);
   
   //
   // Remove any nearby sulci from the ROI
   //
   //corpusCallosumDorsalRoi.logicallyAND(&sulciAroundCorpusCallosumRoi);
   //saveRoiToFile(corpusCallosumDorsalRoi,
   //               corpusCallosumDorsalRoiFileName);

   const float allExtent[6] = {
      -20.0,
       20.0,
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max()
   };
   
   //
   // Place the posterior focus (splenium is the poster part of the corpus callosum)
   //
   ccPosteriorNodeNumber = 
      getNearbyNodeWithShapeValue(fiducialSurface,  // inflatedSurface
                                  curvatureShapeFile,
                                  curvatureFiducialSmoothedMeanColumnNumber,
                                  -100000.0,
                                  corpusCallosumDorsalRoi.getNodeWithMinimumYCoordinate(inflatedSurface),
                                  5.0,
                                  NULL,
                                  allExtent);
   addFocusAtNode(ccPosteriorFocusName,
                  ccPosteriorNodeNumber);
   
   //
   // Place the anterior focus (genu is the anterior part of the corpus callosum)
   //
   const int ccAnteriorNodeNumber = 
      getNearbyNodeWithShapeValue(fiducialSurface, //inflatedSurface,
                                  curvatureShapeFile,
                                  curvatureFiducialSmoothedMeanColumnNumber,
                                  -100000.0,
                                  corpusCallosumDorsalRoi.getNodeWithMaximumYCoordinate(inflatedSurface),
                                  5.0,
                                  NULL,
                                  allExtent);
   addFocusAtNode(ccAnteriorFocusName,
                  ccAnteriorNodeNumber);
   
   //
   // Limit extent to Y-center region to get a dorsal focus
   //
   const float centerExtent[6] = {
      -16.0,
       16.0,
      -10.0,
       5.0,
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max()
   };
   BrainModelSurfaceROINodeSelection dorsalCenterCorpusCallosumRoi(corpusCallosumDorsalRoi);
   dorsalCenterCorpusCallosumRoi.limitExtent(fiducialSurface, centerExtent);
   
   //
   // Place the dorsal focus
   //
   const int ccDorsalNodeNumber = 
      getNearbyNodeWithShapeValue(fiducialSurface, //inflatedSurface,
                                  curvatureShapeFile,
                                  curvatureFiducialSmoothedMeanColumnNumber,
                                  -100000.0,
                                  dorsalCenterCorpusCallosumRoi.getNodeWithMaximumZCoordinate(inflatedSurface),
                                  5.0,
                                  NULL,
                                  allExtent);
   addFocusAtNode(ccDorsalFocusName,
                  ccDorsalNodeNumber);
   



   //
   // Create a node at anterior of where dorsal medial wall starts
   //
   float medialWallStartXYZ[3] = { 7, 4, -10 };
   if (leftHemisphereFlag) {
      medialWallStartXYZ[0] = -7;
   } 
   medialWallStartNodeNumber =
       addFocusAtNodeNearestXYZ(fiducialSurface,
                                ccMedialWallStartFocusName,
                                medialWallStartXYZ);


   //
   // Create a focus near genu beginning
   //
   float ccGenuBeginningXYZ[3] = { 6, 15, -6 };
   if (leftHemisphereFlag) {
    ccGenuBeginningXYZ[0] = -6;
   }
   ccGenuBeginningNodeNumber = 
    addFocusAtNodeNearestXYZ(fiducialSurface,
                             ccGenuBeginningFocusName,
                             ccGenuBeginningXYZ);
       
    
   //
   // Create border from medial wall start to genu beginning
   //
   BrainModelSurfaceROINodeSelection allNodesRoi(brainSet);
   allNodesRoi.selectAllNodes(fiducialSurface);
   drawBorderGeodesic(veryInflatedSurface,
                      &allNodesRoi,
                      landmarkMedialWallStartToGenuBeginningBorderName,
                      medialWallStartNodeNumber,
                      ccGenuBeginningNodeNumber,
                      3.0);

   //
   // Create border from genu beginning to anterior node
   //
   allNodesRoi.selectAllNodes(fiducialSurface);
   drawBorderGeodesic(veryInflatedSurface,
                      &allNodesRoi,
                      landmarkMedialWallGenuBeginningToAnteriorCCBorderName,
                      ccGenuBeginningNodeNumber,
                      ccAnteriorNodeNumber,
                      3.0);


   //
   // Draw border from anterior to dorsal in dorsal CC ROI
   //
   drawBorderMetric(fiducialSurface,
                    BrainModelSurfaceROICreateBorderUsingMetricShape::MODE_FOLLOW_MOST_NEGATIVE,
                    curvatureShapeFile,
                    curvatureFiducialSmoothedMeanColumnNumber,
                    landmarkMedialWallAnteriorToDorsalCCBorderName,
                    ccAnteriorNodeNumber,
                    ccDorsalNodeNumber,
                    2.0,
                    &corpusCallosumDorsalRoi);

   //
   // Draw border from dorsal to posterior in dorsal CC ROI
   //
   drawBorderMetric(fiducialSurface,
                    BrainModelSurfaceROICreateBorderUsingMetricShape::MODE_FOLLOW_MOST_NEGATIVE,
                    curvatureShapeFile,
                    curvatureFiducialSmoothedMeanColumnNumber,
                    landmarkMedialWallDorsalToPosteriorCCBorderName,
                    ccDorsalNodeNumber,
                    ccPosteriorNodeNumber,
                    2.0,
                    &corpusCallosumDorsalRoi);


   BorderProjection medWallDorsal(medialWallDorsalSectionName);
   medWallDorsal.append(
       *borderProjectionFile->getFirstBorderProjectionByName(
          landmarkMedialWallStartToGenuBeginningBorderName));
   medWallDorsal.append(
       *borderProjectionFile->getFirstBorderProjectionByName(
          landmarkMedialWallGenuBeginningToAnteriorCCBorderName));
   medWallDorsal.append(
       *borderProjectionFile->getFirstBorderProjectionByName(
          landmarkMedialWallAnteriorToDorsalCCBorderName));
   medWallDorsal.append(
       *borderProjectionFile->getFirstBorderProjectionByName(
          landmarkMedialWallDorsalToPosteriorCCBorderName));
   borderProjectionFile->addBorderProjection(medWallDorsal);
          
   //
   // Remove segments
   // 
   borderProjectionFile->removeBordersWithName(landmarkMedialWallStartToGenuBeginningBorderName);
   borderProjectionFile->removeBordersWithName(landmarkMedialWallGenuBeginningToAnteriorCCBorderName);
   borderProjectionFile->removeBordersWithName(landmarkMedialWallAnteriorToDorsalCCBorderName);
   borderProjectionFile->removeBordersWithName(landmarkMedialWallDorsalToPosteriorCCBorderName);
   
   //
   // Remove any loops in the medial wall
   //
   removeLoopsFromBorder(veryInflatedSurface,
                         medialWallDorsalSectionName,
                         'X');

   //
   // Resample border
   //
   resampleBorder(fiducialSurface,
                  medialWallDorsalSectionName,
                  2.0);
}
                     

/**
 * identify the ventral medial wall.
 */
void 
BrainModelSurfaceBorderLandmarkIdentification::identifyVentralMedialWall() throw (BrainModelAlgorithmException)
{
   //
   // names for things
   //
   const QString hippocampalDorsalFocusName("HF.Dorsal");
   const QString hippocampalVentralFocusName("HF.Ventral");
   const QString hippocampalAnteriorFocusName("HF.Anterior");
   const QString hippocampalFissureRoiFileName(createFileName("HF",
                                      SpecFile::getRegionOfInterestFileExtension()));
   const QString landmarkSpleniumLimitToHFDorsalName("LANDMARK.MW.SpleniumLimitToHFDorsal");
   const QString landmarkSpleniumLimitToHFDorsalNameAlternate("LANDMARK.MW.SpleniumLimitToHFDorsalAlternate");
   const QString landmarkHFDorsalToHFVentralName("LANDMARK.MW.HFDorsalToHFVentral"); 
   const QString landmarkHFVentralToMedialWallStart("LANDMARK.MW.HFVentralToMedialWallStart");
   medialWallVentralSectionName = "MedialWallVentralSection";
   
   //
   // Delete old stuff
   //
   QFile::remove(hippocampalFissureRoiFileName);
   fociProjectionFile->deleteCellProjectionsWithName(hippocampalDorsalFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(hippocampalVentralFocusName);
   fociProjectionFile->deleteCellProjectionsWithName(hippocampalAnteriorFocusName);
   borderProjectionFile->removeBordersWithName(landmarkSpleniumLimitToHFDorsalName);
   borderProjectionFile->removeBordersWithName(landmarkSpleniumLimitToHFDorsalNameAlternate);
   borderProjectionFile->removeBordersWithName(landmarkHFDorsalToHFVentralName);
   borderProjectionFile->removeBordersWithName(landmarkHFVentralToMedialWallStart);
   borderProjectionFile->removeBordersWithName(medialWallVentralSectionName);

   //
   // Create colors
   //
   borderColorFile->addColor("LANDMARK.MW",
                             255, 50, 50);
   borderColorFile->addColor(medialWallVentralSectionName,
                             0, 255, 0);
   addFocusColor("HF", 0, 0, 200);
                           
   //
   // Create an ROI in the Hippocampal Fissure
   //
   BrainModelSurfaceROINodeSelection hfRoi(brainSet);
   hfRoi.selectNodesWithPaint(BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                              fiducialSurface,
                              paintFile,
                              paintFileSulcusIdColumnNumber,
                              "SUL.HF");
                              
   //
   // Limit Z-Max to the Z of the splenium limit
   //
   const float* ccSpleniumLimitXYZ = 
      fiducialSurface->getCoordinateFile()->getCoordinate(ccSpleniumLimitNodeNumber);
   const float hfMaxExtent[6] = {
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
      ccSpleniumLimitXYZ[2]
   };
   hfRoi.limitExtent(fiducialSurface,
                     hfMaxExtent);
   
   //
   // Save the HF Roi
   //
   saveRoiToFile(hfRoi,
                 hippocampalFissureRoiFileName);

   
   //
   // Create a more stringent ROI of the hippocampal fissure
   //
   BrainModelSurfaceROINodeSelection hfDeepRoi(hfRoi);
   hfDeepRoi.selectNodesWithMetric(BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND,
                                   fiducialSurface,
                                   depthSurfaceShapeFile,
                                   depthSurfaceShapeFileColumnNumber,
                                   -50000.0,
                                   -10.0);
   //
   // Save the Deep HF Roi
   //
   saveRoiToFile(hfDeepRoi,
                 hippocampalFissureRoiFileName);
   
                                   

   //                               
   // Get limits of hippocampal fissure
   //
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   hfDeepRoi.getNodesWithMinMaxXYZValues(inflatedSurface,
                                    mostMedialXNode,
                                    mostLateralXNode,
                                    minXNode, 
                                    maxXNode,
                                    minYNode,
                                    maxYNode,
                                    minZNode,
                                    maxZNode,
                                    absMinXNode,
                                    absMaxXNode,
                                    absMinYNode,
                                    absMaxYNode,
                                    absMinZNode,
                                    absMaxZNode);
   

   //
   // Dorsal and ventral nodes of Hippocampal Fissure
   //
   const int hfDorsalNodeNumber = maxZNode;
   addFocusAtNode(hippocampalDorsalFocusName,
                  hfDorsalNodeNumber);
   const int hfVentralNodeNumber = minZNode;
   addFocusAtNode(hippocampalVentralFocusName,
                  hfVentralNodeNumber);
   
   {
      //
      // Move 10mm anterior
      //
      const int hippocampalFissureAnteriorNodeNumber =
         addFocusAtExtremum(inflatedSurface,
                            hfVentralNodeNumber,
                            BrainModelSurfaceFindExtremum::DIRECTION_Y_POSITIVE,
                            3.0,
                            8.0, // 10.0,
                            8.0,
                            "HF_Ventral_Anterior_Test",
                            NULL,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_POSITIVE);
         
      //
      // This should put find the node on the parahippocampal gyrus at ventral end
      //
      const int hippocampalGyrusVentralNodeNumber =
         addFocusAtExtremum(inflatedSurface,
                            hippocampalFissureAnteriorNodeNumber, //hfVentralNodeNumber,
                            BrainModelSurfaceFindExtremum::DIRECTION_MEDIAL,
                            100000.0,
                            5.0,
                            100000.0,
                            "HF_Ventral_Test",
                            NULL,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_POSITIVE);
      //const float* ventralXYZ = 
      //   inflatedSurface->getCoordinateFile()->getCoordinate(hippocampalGyrusVentralNodeNumber);

      //
      // Move 12mm laterally
      //
      const float lateralOffset = (leftHemisphereFlag ? -12.0 : 12.0);
      const int hippocampalGyrusVentralBorderNodeNumber =
         addFocusAtExtremum(inflatedSurface,
                            hippocampalGyrusVentralNodeNumber,
                            BrainModelSurfaceFindExtremum::DIRECTION_LATERAL,
                            12.5,
                            5.0,
                            100.0,
                            "HF_Ventral_Border_Node",
                            NULL,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_POSITIVE);
            


      //
      // This should put find the node on the parahippocampal gyrus near middle
      // but limit to nodes with positive Z-normals
      //
      const int hippocampalGyrusMidPointNodeNumber =
         addFocusAtExtremum(inflatedSurface,
                            hippocampalGyrusVentralNodeNumber,
                            BrainModelSurfaceFindExtremum::DIRECTION_Y_NEGATIVE,
                            5.0,
                            35.0, //25.0,
                            100000.0,
                            "HF_Midpoint_Y_Test",
                            NULL,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE,
                            BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_POSITIVE);
      const float* midPointXYZ = 
         inflatedSurface->getCoordinateFile()->getCoordinate(hippocampalGyrusMidPointNodeNumber);

      //
      // Move 12mm laterally
      //
      const int hippocampalGyrusMidPointBorderNodeNumber =
         inflatedSurface->getCoordinateFile()->getCoordinateIndexClosestToPoint(
            midPointXYZ[0] + lateralOffset,
            midPointXYZ[1],
            midPointXYZ[2] + 5.0);
      addFocusAtNode("HF_MidPoint_Border_Node",
                    hippocampalGyrusMidPointBorderNodeNumber);

      //
      // Find node along path from temporal pole to HF ventral node
      // that is 24mm from temporal pole
      //
      const int hfAnteriorNodeNumber =
         findNodeAlongGeodesicPathBetweenNodes(inflatedSurface,
                                               temporalPoleNodeNumber,
                                               hfVentralNodeNumber,
                                               30.0);
      addFocusAtNode(hippocampalAnteriorFocusName,
                     hfAnteriorNodeNumber);
                 
      //
      // Draw a border connecting the splenium limit node to the dorsal HF node
      //
      const QString seg1Name("seg1Name");
      drawBorderGeodesic(inflatedSurface,
                         &hfRoi,
                         seg1Name,
                         hfDorsalNodeNumber,
                         hippocampalGyrusMidPointBorderNodeNumber,
                         3.0);
                         
      //
      // Draw a border connecting the dorsal HF node to the ventral HF node
      //
      const QString seg2Name("seg2Name");
      drawBorderGeodesic(inflatedSurface,
                         &hfDeepRoi,
                         seg2Name,
                         hippocampalGyrusMidPointBorderNodeNumber,
                         hippocampalGyrusVentralBorderNodeNumber,
                         3.0);
                         
      //
      // Draw a border connecting the ventral HF node to node
      // along path to temproral pole
      //
      const QString seg3Name("seg3Name");
      const bool seg3Valid = (hippocampalGyrusVentralBorderNodeNumber != hfAnteriorNodeNumber);
      if (seg3Valid) {
         drawBorderGeodesic(inflatedSurface,
                            NULL,
                            seg3Name,
                            hippocampalGyrusVentralBorderNodeNumber,
                            hfAnteriorNodeNumber,
                            3.0);
      }
      
      //
      // Draw a border connection HF ventral to Medial Wall Start
      //
      const QString seg4Name("seg4Name");
      drawBorderGeodesic(inflatedSurface,
                         NULL,
                         seg4Name,
                         hfAnteriorNodeNumber,
                         medialWallStartNodeNumber,
                         3.0);

       {       
         //
         // Draw a border connecting the splenium limit node to the dorsal HF node
         //
          const QString seg0NameAlternate("seg0NameAlternate");
          drawBorderMetric(inflatedSurface,
                           BrainModelSurfaceROICreateBorderUsingMetricShape::MODE_FOLLOW_MOST_NEGATIVE,
                           curvatureShapeFile,
                           curvatureFiducialSmoothedMeanColumnNumber,
                           seg0NameAlternate,
                           ccPosteriorNodeNumber,
                           hfDorsalNodeNumber,
                           3.0);
          BorderProjection medialWallVentralBorderProjectionTest(medialWallVentralSectionName);
          medialWallVentralBorderProjectionTest.append(
             *borderProjectionFile->getFirstBorderProjectionByName(
                seg0NameAlternate));
          medialWallVentralBorderProjectionTest.append(
             *borderProjectionFile->getFirstBorderProjectionByName(
                seg1Name));
          medialWallVentralBorderProjectionTest.append(
             *borderProjectionFile->getFirstBorderProjectionByName(
                seg2Name));
          if (seg3Valid) {
             medialWallVentralBorderProjectionTest.append(
                *borderProjectionFile->getFirstBorderProjectionByName(
                    seg3Name));
          }
          medialWallVentralBorderProjectionTest.append(
             *borderProjectionFile->getFirstBorderProjectionByName(
                seg4Name));
          borderProjectionFile->addBorderProjection(medialWallVentralBorderProjectionTest);
          borderProjectionFile->removeBordersWithName(seg0NameAlternate);
       }
                         

       //
       // Remove the segments that were merged into the ventral medial wall
       //
       //borderProjectionFile->removeBordersWithName(seg0Name);
       borderProjectionFile->removeBordersWithName(seg1Name);
       borderProjectionFile->removeBordersWithName(seg2Name);
       borderProjectionFile->removeBordersWithName(seg3Name);
       borderProjectionFile->removeBordersWithName(seg4Name);
       
   }
   
   //
   // Remove any loops in the medial wall
   //
   removeLoopsFromBorder(veryInflatedSurface,
                         medialWallVentralSectionName,
                         'X');

   //
   // Resample border
   //
   resampleBorder(fiducialSurface,
                  medialWallVentralSectionName,
                  2.0);
}      


/**
 * create a file name.
 */
QString 
BrainModelSurfaceBorderLandmarkIdentification::createFileName(const QString& description,
                                                     const QString& extension) const
{
   QString species("Species");
   if (brainSet->getSpecies().isEmpty() == false) {
      species = brainSet->getSpecies();
   }
   QString subject("Subject");
   if (brainSet->getSubject().isEmpty() == false) {
      subject = brainSet->getSubject();
   }
   
   const QString name =
      (species
       + "."
       + subject
       + "."
       + Structure::convertTypeToAbbreviatedString(brainSet->getStructure().getType())
       + "."
       + description
       + extension);
   
   return name;
}
                             
/**
 * get node nearby that has the value closest to the target value.
 */
int 
BrainModelSurfaceBorderLandmarkIdentification::getNearbyNodeWithShapeValue(
                                const BrainModelSurface* surface,
                                const SurfaceShapeFile* shapeFile,
                                const int shapeColumnNumber,
                                const float targetValue,
                                const int startNodeNumber,
                                const float maxDistanceIn,
                                const BrainModelSurfaceROINodeSelection* limitToWithinROI,
                                const float* limitToExtent) const
{
   float extent[6] = {
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max()
   };
   if (limitToExtent != NULL) {
      for (int i = 0; i < 6; i++) {
         extent[i] = limitToExtent[i];
      }
   }
   
   BrainModelSurfaceROINodeSelection roi(brainSet);
   if (limitToWithinROI != NULL) {
      roi = *limitToWithinROI;
   }
   else {
      roi.selectNodesWithinGeodesicDistance(
         BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
         surface,
         startNodeNumber,
         maxDistanceIn);
   }
   
   const int numNodes = surface->getNumberOfNodes();
   const CoordinateFile* cf = surface->getCoordinateFile();
   
   float maxDistanceSquared = maxDistanceIn * maxDistanceIn;
   
   int nodeNumber = startNodeNumber;
   float testValue = shapeFile->getValue(startNodeNumber,
                                               shapeColumnNumber);
                                    
   
   for (int i = 0; i < numNodes; i++) {
      //
      // Is node in the ROI
      //
      if (roi.getNodeSelected(i)) {
         //
         // Is node within allowable distance from the starting node number
         //
         if (cf->getDistanceBetweenCoordinatesSquared(i, startNodeNumber)  
             < maxDistanceSquared) {
            //
            // Is value closer ?
            //
            const float value = shapeFile->getValue(i, shapeColumnNumber);
            if (std::fabs(targetValue - value) <
                std::fabs(targetValue - testValue)) {
               //
               // Within extent
               //
               const float* nodeXYZ = cf->getCoordinate(i);
               if ((nodeXYZ[0] >= extent[0]) &&
                   (nodeXYZ[0] <= extent[1]) &&
                   (nodeXYZ[1] >= extent[2]) &&
                   (nodeXYZ[1] <= extent[3]) &&
                   (nodeXYZ[2] >= extent[4]) &&
                   (nodeXYZ[2] <= extent[5])) {
                  testValue = value;
                  nodeNumber = i;
               }
            }
         }
      }
   }
   
   return nodeNumber;
}                                        

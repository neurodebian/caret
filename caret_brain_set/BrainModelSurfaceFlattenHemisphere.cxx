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

#include "AreaColorFile.h"
#include "BorderFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderCutter.h"
#include "BrainModelSurfaceBorderLandmarkIdentification.h"
#include "BrainModelSurfaceFlattenHemisphere.h"
#include "BrainModelSurfaceSulcalIdentificationProbabilistic.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "PaintFile.h"
#include "SpecFile.h"
#include "TopologyFile.h"
#include "TransformationMatrixFile.h"

const QString medialWallName("MEDIAL.WALL");

/**
 * constructor.
 */
BrainModelSurfaceFlattenHemisphere::BrainModelSurfaceFlattenHemisphere(
                             BrainSet* bsIn,
                             const BrainModelSurface* fiducialSurfaceIn,
                             const BrainModelSurface* ellipsoidOrSphericalSurfaceIn,
                             const BorderProjectionFile* flattenBorderProjectionFileIn,
                             PaintFile* paintFileInOut,
                             AreaColorFile* areaColorFileInOut,
                             const bool createFiducialWithSmoothedMedialWallFlagIn,
                             const bool autoSaveFilesFlagIn)
   : BrainModelAlgorithm(bsIn),
     inputFiducialSurface(fiducialSurfaceIn),
     inputSphericalSurface(ellipsoidOrSphericalSurfaceIn),
     inputFlattenBorderProjectionFile(flattenBorderProjectionFileIn),
     paintFile(paintFileInOut),
     areaColorFile(areaColorFileInOut),
     createFiducialWithSmoothedMedialWallFlag(createFiducialWithSmoothedMedialWallFlagIn),
     autoSaveFilesFlag(autoSaveFilesFlagIn)
{
   outputSphericalSurface = NULL;
   outputInitialFlatSurface = NULL;
   outputOpenTopologyFile = NULL;
   outputCutTopologyFile = NULL;
   outputFiducialSurfaceWithSmoothedMedialWall = NULL;
}

/**
 * destructor.
 */
BrainModelSurfaceFlattenHemisphere::~BrainModelSurfaceFlattenHemisphere()
{
}
 
/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceFlattenHemisphere::execute() throw (BrainModelAlgorithmException)
{
   //
   // Check inputs
   //
   if (inputFiducialSurface == NULL) {
      throw BrainModelAlgorithmException("Fiducial surface is invalid.");
   }
   if (inputSphericalSurface == NULL) {
      throw BrainModelAlgorithmException("Ellipsoid/Sphere surface is invalid.");
   }
   if (inputFlattenBorderProjectionFile == NULL) {
      throw BrainModelAlgorithmException("Border projection is invalid.");
   }

   //
   // Make sure there are no islands in the surface
   //   
   const TopologyFile* topologyFile = inputSphericalSurface->getTopologyFile();   
   std::vector<int> dummy1, dummy2, dummy3;
   const int numPiecesOfSurface = topologyFile->findIslands(dummy1, dummy2, dummy3);
   if (numPiecesOfSurface > 1) {
      throw BrainModelAlgorithmException(
         "There are multiple pieces of surface.  Use Surface: Topology: Remove Islands\n"
         "to remove them and verify that the surface remains correct.");
   }

   //
   // Veryify left or right hemisphere
   switch (inputSphericalSurface->getStructure().getType()) {
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
      case Structure::STRUCTURE_TYPE_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_SUBCORTICAL:
      case Structure::STRUCTURE_TYPE_ALL:
      case Structure::STRUCTURE_TYPE_INVALID:
         throw BrainModelAlgorithmException(
            "The ellipsoid/spherical surface's structure is neither "
            "left nor right which is required for flattening.");
         break;
   }
   
   //
   // Cleanup paint file
   //
   removeMedialWallAssignmentsFromPaintFile();
   
   //
   //
   // Make sure the sphere is of the correct size and a sphere
   //
   createSphericalSurface();
   
   //
   // Find the borders used during flattening
   //
   findFlatteningBorders();
   
   //
   // Create the initial flat surface
   //
   createInitialFlatSurface();
}

/**
 * create the spherical surface.
 */
void 
BrainModelSurfaceFlattenHemisphere::createSphericalSurface() throw (BrainModelAlgorithmException)
{
   if (inputSphericalSurface->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
      outputSphericalSurface = new BrainModelSurface(*inputSphericalSurface);
      outputSphericalSurface->convertToSphereWithSurfaceArea(inputFiducialSurface->getSurfaceArea());
      outputSphericalSurface->getCoordinateFile()->setFileName(
         outputSphericalSurface->getCoordinateFile()->makeDefaultFileName("Spherical"));
      brainSet->addBrainModel(outputSphericalSurface);
      
      if (autoSaveFilesFlag) {
         try {
            CoordinateFile* cf = outputSphericalSurface->getCoordinateFile();
            brainSet->writeCoordinateFile(cf->getFileName(),
                                          BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                          cf,
                                          true);
         }
         catch (FileException& e) {
            throw BrainModelAlgorithmException(e);
         }
      }
   }
}

/**
 * create the initial flat surface.
 */
void 
BrainModelSurfaceFlattenHemisphere::createInitialFlatSurface() throw (BrainModelAlgorithmException)
{
   //
   // Get the node at the anterior commissure node number
   //
   const int anteriorCommissureNodeNumber =
      inputFiducialSurface->getCoordinateFile()->getCoordinateIndexClosestToPoint(0,0,0);
   if (anteriorCommissureNodeNumber < 0) {
      throw BrainModelAlgorithmException("Unable to find node near (0, 0, 0) in the fiducial surface.");
   }

   //
   // Copy the spherical surface
   //
   BrainModelSurface flattenSurface(*inputSphericalSurface);
   if (outputSphericalSurface != NULL) {
      flattenSurface = BrainModelSurface(*outputSphericalSurface);
   }

   //
   // Translate the surface to its center of mass
   //
   flattenSurface.translateToCenterOfMass();
   
   //
   // Orient the sphere so that the middle of the medial wall is on the positive Z axix
   //
   const CoordinateFile* flattenCoordinates = flattenSurface.getCoordinateFile();
   const float* sphericalAnteriorCommissureXYZ = 
      flattenCoordinates->getCoordinate(anteriorCommissureNodeNumber);
   flattenSurface.orientPointToNegativeZAxis(sphericalAnteriorCommissureXYZ);
   
   //
   // Rotate the surface so that the medial wall faces the user
   //
   TransformationMatrix tm;
   tm.rotate(TransformationMatrix::ROTATE_Y_AXIS, 180.0);
   if (flattenSurface.getStructure() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
      tm.rotate(TransformationMatrix::ROTATE_Z_AXIS, 90.0);
   }
   else {
      tm.rotate(TransformationMatrix::ROTATE_Z_AXIS, -90.0);
   }
   flattenSurface.applyTransformationMatrix(tm);
   
   if (DebugControl::getDebugOn()) {
      CoordinateFile cf(*flattenSurface.getCoordinateFile());
      try {
         cf.writeFile("sphere_oriented_to_show_medial_wall"
                       + SpecFile::getCoordinateFileExtension());
      }
      catch (FileException&) {
      }
   }
   
   //
   // Copy the surface for in order to determine nodes inside the medial wall
   // and project the nodes with positive Z to the plane
   //
   BrainModelSurface surfaceForBorderInclusion(flattenSurface);
   surfaceForBorderInclusion.projectCoordinatesToPlane(
      BrainModelSurface::COORDINATE_PLANE_MOVE_POSITIVE_Z_TO_ZERO);

   //
   // Unproject the medial wall border
   //
   Border medialWallBorder;
   medialWallBorderProjection.unprojectBorderProjection(
      surfaceForBorderInclusion.getCoordinateFile(),
      medialWallBorder);
   
   //
   // Determine nodes in medial wall border
   //
   const int numNodes = surfaceForBorderInclusion.getNumberOfNodes();
   std::vector<bool> nodesInsideMedialWallBorder(numNodes, false);
   medialWallBorder.pointsInsideBorder2D(
      surfaceForBorderInclusion.getCoordinateFile()->getCoordinate(0), 
      numNodes,
      nodesInsideMedialWallBorder, 
      true);

   //
   // Update paint identification
   //
   int medialWallColumnNumber = -1;
   int sulcusIdPaintColumnNumber = paintFile->getColumnWithName(
      BrainModelSurfaceSulcalIdentificationProbabilistic::getSulcusIdPaintColumnName());
   int geographyPaintColumnNumber = paintFile->getColumnWithName("Geography");
   if ((sulcusIdPaintColumnNumber < 0) &&
       (geographyPaintColumnNumber < 0)) {
      if (paintFile->getNumberOfNodes() == 0) {
         paintFile->setNumberOfNodesAndColumns(inputFiducialSurface->getNumberOfNodes(), 1);
      }
      else {
         paintFile->addColumns(1);
      }
      geographyPaintColumnNumber = paintFile->getNumberOfColumns() - 1;
      paintFile->setColumnName(geographyPaintColumnNumber, "Geography");
   }
   if (sulcusIdPaintColumnNumber > 0) {
      medialWallColumnNumber = sulcusIdPaintColumnNumber;
   }
   else if (geographyPaintColumnNumber > 0) {
      medialWallColumnNumber = geographyPaintColumnNumber;
   }
   const int medialWallPaintNumber = paintFile->addPaintName(medialWallName);
   for (int i = 0; i < numNodes; i++) {
      if (nodesInsideMedialWallBorder[i]) {
         if (sulcusIdPaintColumnNumber >= 0) {
            paintFile->setPaint(i, sulcusIdPaintColumnNumber, medialWallPaintNumber);
         }
         if (geographyPaintColumnNumber >= 0) {
            paintFile->setPaint(i, geographyPaintColumnNumber, medialWallPaintNumber);
         }
      }
   }
   
   //
   // Add area color
   //
   bool matchFlag = false;
   areaColorFile->getColorIndexByName(medialWallName, matchFlag);
   if (matchFlag == false) {
      areaColorFile->addColor(medialWallName, 0, 255, 0);
      if (autoSaveFilesFlag) {
         try {
            brainSet->writeAreaColorFile(areaColorFile->getFileName());
         }
         catch (FileException& e) {
            throw BrainModelAlgorithmException(e);
         }
      }
   }
   
   //
   // Orient the sphere so that the medial wall is on the negative Z axis
   // which is necessary for flattening
   //
   std::vector<QString> paintNames;
   paintNames.push_back(medialWallName);
   QString errorMessage;
   if (flattenSurface.orientPaintedNodesToNegativeZAxis(paintFile,
                                                        paintNames,
                                                        medialWallColumnNumber,
                                                         errorMessage)) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   
   //
   // Compress the fronat face
   //
   flattenSurface.compressFrontFace(0.5, 0);
   
   //
   // Get pointer to closed topology
   //
   const TopologyFile* closedTopologyFile = flattenSurface.getTopologyFile();
   
   //
   // Create new open topology by removing the medial wall and any stragglers
   //
   outputOpenTopologyFile = new TopologyFile(*closedTopologyFile);
   outputOpenTopologyFile->makeDefaultFileName("OPEN");
   brainSet->addTopologyFile(outputOpenTopologyFile);
   brainSet->disconnectNodes(outputOpenTopologyFile, nodesInsideMedialWallBorder);
   outputOpenTopologyFile->setTopologyType(TopologyFile::TOPOLOGY_TYPE_OPEN);
   outputOpenTopologyFile->removeCornerTiles(2);
   if (autoSaveFilesFlag) {
      try {
         brainSet->writeTopologyFile(outputOpenTopologyFile->getFileName(),
                                     outputOpenTopologyFile->getTopologyType(),
                                     outputOpenTopologyFile);
      }
      catch (FileException& e) {
         throw BrainModelAlgorithmException(e);
      }
   }
   
   if (DebugControl::getDebugOn()) {
      CoordinateFile cf2(*flattenSurface.getCoordinateFile());
      try {
         const QString topoFileName = FileUtilities::basename(
            flattenSurface.getTopologyFile()->getFileName());
         if (topoFileName.isEmpty() == false) {
            cf2.setHeaderTag(SpecFile::getUnknownTopoFileMatchTag(), topoFileName);
         }
         cf2.writeFile("sphere_oriented_compressed_for_cuts"
                       + SpecFile::getCoordinateFileExtension());
      }
      catch (FileException&) {
      }
   }
   
   //
   // Create the cut toplogy file and flat surface
   //
   outputCutTopologyFile = new TopologyFile(*outputOpenTopologyFile);
   outputCutTopologyFile->makeDefaultFileName("CUT");
   brainSet->addTopologyFile(outputCutTopologyFile);
   outputInitialFlatSurface = new BrainModelSurface(flattenSurface);
   outputInitialFlatSurface->getCoordinateFile()->makeDefaultFileName("InitialFlat");
   outputInitialFlatSurface->setTopologyFile(outputCutTopologyFile);

   //
   // Project the sphere flat
   //
   outputInitialFlatSurface->convertSphereToFlat();
   
   //
   // Smooth just a little bit
   //
   outputInitialFlatSurface->arealSmoothing(1.0, 5, 0);
   
   //
   // update the normals
   //
   outputInitialFlatSurface->computeNormals();
   
   if (DebugControl::getDebugOn()) {
      CoordinateFile cf3(*outputInitialFlatSurface->getCoordinateFile());
      try {
         cf3.writeFile("initial_flat_before_cuts_applied"
                       + SpecFile::getCoordinateFileExtension());
      }
      catch (FileException&) {
      }
   }

   //
   // Need to do before removing medial wall otherwise a cut may fail
   // near the medial wall
   //
   BrainModelSurfaceBorderCutter cutter(brainSet,
                                        outputInitialFlatSurface,
                                        &cutBorderProjectionFile,
                                        BrainModelSurfaceBorderCutter::CUTTING_MODE_FLAT_SURFACE,
                                        true);
   cutter.execute();
   
   //
   // Scale the surface to 10x the fiducial area
   //
   outputInitialFlatSurface->scaleSurfaceToArea(10.0 * inputFiducialSurface->getSurfaceArea(), false);

   //
   // Translate to center of mass and scale to fit the window
   //
   outputInitialFlatSurface->translateToCenterOfMass();
   outputInitialFlatSurface->updateForDefaultScaling();
   
   
   //
   // Eliminate any islands or stragglers
   //
   outputCutTopologyFile->disconnectIslands();
   outputCutTopologyFile->removeCornerTiles(1);
   outputInitialFlatSurface->moveDisconnectedNodesToOrigin();
   
   //
   // Should a fiducial surface with a smoothed medial wall be produced
   //
   if (createFiducialWithSmoothedMedialWallFlag) {
      const float strength = 1.0;
      const int   iterations = 500;
      const int   smoothEdgesIterations = 0;
      BrainModelSurface* bms = new BrainModelSurface(*inputFiducialSurface);
      bms->arealSmoothing(strength,
                          iterations,
                          smoothEdgesIterations,
                          &nodesInsideMedialWallBorder);
      bms->getCoordinateFile()->makeDefaultFileName("FiducialSmoothedMedialWall");
      brainSet->addBrainModel(bms);
      outputFiducialSurfaceWithSmoothedMedialWall = bms;

      if (autoSaveFilesFlag) {
         try {
            CoordinateFile* cf = bms->getCoordinateFile();
            brainSet->writeCoordinateFile(cf->getFileName(),
                                          bms->getSurfaceType(),
                                          cf,
                                          true);
         }
         catch (FileException& e) {
            throw BrainModelAlgorithmException(e);
         }
      }
   }
   
   //
   // Add the initial flat surface to the brain set
   //
   brainSet->addBrainModel(outputInitialFlatSurface);
   if (autoSaveFilesFlag) {
      try {
         CoordinateFile* cf = outputInitialFlatSurface->getCoordinateFile();
         brainSet->writeCoordinateFile(cf->getFileName(),
                                       outputInitialFlatSurface->getSurfaceType(),
                                       cf,
                                       true);
         TopologyFile* tf = outputInitialFlatSurface->getTopologyFile();
         brainSet->writeTopologyFile(tf->getFileName(),
                                     tf->getTopologyType(),
                                     tf);
      }
      catch (FileException& e) {
         throw BrainModelAlgorithmException(e);
      }
      
      if (paintFile->getModified()) {
         brainSet->writePaintFile(paintFile->getFileName());
      }
   }
}

/**
 * remove medial wall assignments from paint file.
 */
void 
BrainModelSurfaceFlattenHemisphere::removeMedialWallAssignmentsFromPaintFile()
{
   const int medialWallPaintIndex = paintFile->getPaintIndexFromName(medialWallName);
   if (medialWallPaintIndex >= 0) {
      const int questionIndex = paintFile->addPaintName("???");
      const int numNodes = paintFile->getNumberOfNodes();
      const int numCols = paintFile->getNumberOfColumns();
      for (int i = 0; i < numNodes; i++) {
         for (int j = 0; j < numCols; j++) {
            if (paintFile->getPaint(i, j) == medialWallPaintIndex) {
               paintFile->setPaint(i, j, questionIndex);
            }
         }
      }
   }
}

/**
 * find the flattening borders.
 */
void 
BrainModelSurfaceFlattenHemisphere::findFlatteningBorders() throw (BrainModelAlgorithmException)
{
   const int numBorderProjections = 
      inputFlattenBorderProjectionFile->getNumberOfBorderProjections();
      
   for (int i = 0; i < numBorderProjections; i++) {
      const BorderProjection* bp =
         inputFlattenBorderProjectionFile->getBorderProjection(i);
      const QString name = bp->getName();
      
      if (name == 
          BrainModelSurfaceBorderLandmarkIdentification::getFlattenMedialWallBorderName()) {
         medialWallBorderProjection = *bp;
      }
      else if (name.startsWith(
               BrainModelSurfaceBorderLandmarkIdentification::getFlattenStandardCutsBorderNamePrefix())) {
         cutBorderProjectionFile.addBorderProjection(*bp);
      }
   }
   
   //
   // Unable to find medial wall in input borders ?
   //
   if (medialWallBorderProjection.getNumberOfLinks() <= 0) {
      throw BrainModelAlgorithmException("Unable to find border named \""
                             + BrainModelSurfaceBorderLandmarkIdentification::getFlattenMedialWallBorderName()
                             + "\" in input border projection file.");
   }
   
   //
   // Unable to find cuts ?
   //
   if (cutBorderProjectionFile.getNumberOfBorderProjections() <= 0) {
      throw BrainModelAlgorithmException("Unable to find cuts beginning with \""
                             + BrainModelSurfaceBorderLandmarkIdentification::getFlattenStandardCutsBorderNamePrefix()
                             + "\" in input border projection file.");
   }
   
}
      


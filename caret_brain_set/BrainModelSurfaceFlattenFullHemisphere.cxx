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

#include <algorithm>
#include <iostream>
#include <sstream>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfacePointLocator.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceCutter.h"
#include "BrainModelSurfaceFlattenFullHemisphere.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "DisplaySettingsBorders.h"
#include "FileUtilities.h"
#include "PaintFile.h"
#include "SpecFile.h"
#include "SurfaceShapeFile.h"
#include "TransformationMatrixFile.h"

//
// File globals
//
static const QString medialWallName("MEDIAL.WALL");
static const QString calcarineCutName("CalcarineCut");
static const QString frontalCutName("FrontalCut");
static const QString medialWallDorsalLandmark("LANDMARK.MedWall.DORSAL");
static const QString medialWallVentralLandmark("LANDMARK.MedWall.VENTRAL");
static const QString calcarineLandmark("LANDMARK.CalcarineSulcus");

/**
 * Constructor.
 */
BrainModelSurfaceFlattenFullHemisphere::BrainModelSurfaceFlattenFullHemisphere(
                                                         BrainModelSurface* fiducialSurfaceIn,
                                                         BrainModelSurface* ellipsoidSurfaceIn,
                                                         BorderFile* borderFileIn,
                                                         const float acPositionIn[3],
                                                         const float acOffsetIn[3],
                                                         const bool smoothFiducialMedialWallFlagIn)
   : BrainModelAlgorithm(ellipsoidSurfaceIn->getBrainSet())
{
   fiducialSurface = fiducialSurfaceIn;
   flattenSurface  = ellipsoidSurfaceIn;
   borderFile      = borderFileIn;
   acPosition[0]   = acPositionIn[0];
   acPosition[1]   = acPositionIn[1];
   acPosition[2]   = acPositionIn[2];
   acOffset[0]     = acOffsetIn[0];
   acOffset[1]     = acOffsetIn[1];
   acOffset[2]     = acOffsetIn[2];
   smoothFiducialMedialWallFlag = smoothFiducialMedialWallFlagIn;
}

/**
 * Destructor.
 */
BrainModelSurfaceFlattenFullHemisphere::~BrainModelSurfaceFlattenFullHemisphere()
{
}

/**
 * Execute the flattening.
 */
void 
BrainModelSurfaceFlattenFullHemisphere::execute() throw (BrainModelAlgorithmException)
{
   DisplaySettingsBorders* dsb = brainSet->getDisplaySettingsBorders();
   dsb->setDisplayBorders(true);
   
   const TopologyFile* topologyFile = flattenSurface->getTopologyFile();
   
   std::vector<int> dummy1, dummy2, dummy3;
   const int numPiecesOfSurface = topologyFile->findIslands(dummy1, dummy2, dummy3);
   if (numPiecesOfSurface > 1) {
      throw BrainModelAlgorithmException(
         "There are multiple pieces of surface.  Use Surface: Topology: Remove Islands\n"
         "to remove them and verify that the surface remains correct.");
   }
      
   if (borderFile == NULL) {
      throw BrainModelAlgorithmException("Border File not provided.");
   }
   else if (borderFile->getNumberOfBorders() == 0) {
      throw BrainModelAlgorithmException("Border File contains no borders.");
   }
   
   const int numNodes = flattenSurface->getNumberOfNodes();
   
   //
   // Structure naming
   //
   hemStr = flattenSurface->getStructure().getTypeAsAbbreviatedString();
/*
   switch(flattenSurface->getStructure()) {
      case BrainModelSurface::HEMISPHERE_LEFT:
         hemStr = "L");
         break;
      case BrainModelSurface::HEMISPHERE_RIGHT:
         hemStr = "R");
         break;
      case BrainModelSurface::HEMISPHERE_BOTH:
         hemStr = "LR");
         break;
      case BrainModelSurface::HEMISPHERE_UNKNOWN:
         hemStr = "U");
         break;
   }
*/
   
   //
   // original caret coord file and topology file names
   //
   originalCoordFileName = flattenSurface->getFileName();
   if (originalCoordFileName.isEmpty()) {
      std::ostringstream str;
      str << "species.case."
          << hemStr.toAscii().constData()
          << ".descrip."
          << numNodes
          << SpecFile::getCoordinateFileExtension().toAscii().constData();
      originalCoordFileName = str.str().c_str();
   }
   originalTopoFileName = flattenSurface->getTopologyFile()->getFileName();
   if (originalTopoFileName.isEmpty()) {
      std::ostringstream str;
      str << "species.case."
          << hemStr.toAscii().constData()
          << ".descrip."
          << numNodes
          << SpecFile::getTopoFileExtension().toAscii().constData();
      originalTopoFileName = str.str().c_str();
   }
   
   //
   // Change any nodes named medial wall in geography column to gyral.
   // Create the geography column if it does not exist.
   //
   PaintFile* paintFile = brainSet->getPaintFile();
   paintFileEmptyAtStart = (paintFile->getNumberOfColumns() == 0);
   geographyColumnNumber = paintFile->getGeographyColumnNumber();
   if (geographyColumnNumber < 0) {
      if (paintFile->getNumberOfColumns() == 0) {
         paintFile->setNumberOfNodesAndColumns(numNodes, 1);
         geographyColumnNumber = 0;
      }
      else {
         paintFile->addColumns(1);
         geographyColumnNumber = paintFile->getNumberOfColumns() - 1;
      }
      paintFile->setColumnName(geographyColumnNumber, PaintFile::columnNameGeography);
   }

   //
   // Change any nodes named MEDIAL.WALL to GYRAL
   //
   medialWallPaintFileIndex = paintFile->addPaintName(medialWallName);
   const int gyrusIndex = paintFile->addPaintName("GYRAL");
   for (int i = 0; i < numNodes; i++) {
      if (paintFile->getPaint(i, geographyColumnNumber) == medialWallPaintFileIndex) {
         paintFile->setPaint(i, geographyColumnNumber, gyrusIndex);
      }
   }

   //
   // Add the GYRAL and MEDIAL.WALL colors if necessary
   //
   bool addedAreaColor = false;
   AreaColorFile* nodeColors = brainSet->getAreaColorFile();
   bool exactMatch = false;
   nodeColors->getColorIndexByName("GYRAL", exactMatch);
   if (exactMatch == false) {
      nodeColors->addColor("GYRAL", 170, 170, 170, 1, 2);
      addedAreaColor = true;
   }
   exactMatch = false;
   nodeColors->getColorIndexByName(medialWallName, exactMatch);
   if (exactMatch == false) {
      nodeColors->addColor(medialWallName, 255, 0, 0, 1, 2);
      addedAreaColor = true;
   }
   
   //
   // Save the node color file if colors were added
   //
   if (addedAreaColor) {
      QString nodeColorFileName(nodeColors->getFileName());
      
      //
      // Create the node color file name if there is not one
      //
      if (nodeColorFileName.isEmpty() || paintFileEmptyAtStart) {
         QString directory, species, casename, anatomy, hemisphere, description;
         QString descriptionNoType, theDate, numNodes, extension;
         if (FileUtilities::parseCaretDataFileName(originalTopoFileName, directory,
                                                   species, casename, anatomy, hemisphere,
                                                   description, descriptionNoType, theDate,
                                                   numNodes, extension)) {
            nodeColorFileName = FileUtilities::reassembleCaretDataFileName(directory,
                                                       species, casename, anatomy, hemisphere,
                                                       "colors", theDate, numNodes, 
                                                       SpecFile::getAreaColorFileExtension());
         }
         else {
            std::ostringstream str;
            str << "species.case."
               << hemStr.toAscii().constData()
               << ".colors."
               << numNodes.toAscii().constData()
               << SpecFile::getAreaColorFileExtension().toAscii().constData();
            nodeColorFileName = str.str().c_str();
         }
      }
      
      //
      // write the node color file
      //
      brainSet->writeAreaColorFile(nodeColorFileName);
   }
   
   //
   // Make a copy of the surface
   // 
   flattenSurface = new BrainModelSurface(*flattenSurface);
   brainSet->addBrainModel(flattenSurface);
   flattenSurface->appendToCoordinateFileComment("Flattening full hemisphere\n");
   
   //
   // Translate the surface to its center of mass
   //
   flattenSurface->translateToCenterOfMass();
   
   //
   // Convert the ellipsoid into a sphere whose surface area is that of the fiducial surface
   //
   const float surfaceArea = fiducialSurface->getSurfaceArea();
   flattenSurface->convertEllipsoidToSphereWithSurfaceArea(surfaceArea);

   //
   // Save the spherical surface
   //
   const QString sphereCoordName = 
      FileUtilities::changeCaretDataFileDescriptionType(originalCoordFileName, "SPHERE");
   try {
      brainSet->writeCoordinateFile(sphereCoordName,
                                    BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                    flattenSurface->getCoordinateFile());
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
                                 
   //
   // Copy the surface
   //
   flattenSurface = new BrainModelSurface(*flattenSurface);
   brainSet->addBrainModel(flattenSurface);
   
   //
   // Get the radius of spherical surface
   //
   const float sphereRadius = flattenSurface->getSphericalSurfaceRadius();
   
   //
   // Set the radius of spherical borders to the radius of the surface
   //
   borderFile->setSphericalBorderRadius(sphereRadius);
   
   //
   // Approximate middle of medial wall with AC and offset
   //
   const float middleMedialWallPosFiducial[3] = { acPosition[0] + acOffset[0],
                                                  acPosition[1] + acOffset[1],
                                                  acPosition[2] + acOffset[2] };
                                          
   //
   // Find node closest to middle of medial wall in fiducial surface
   //
   BrainModelSurfacePointLocator fiducialLocator(fiducialSurface, true);
   const int middleMedialWallNodeIndex = fiducialLocator.getNearestPoint(
                                                   middleMedialWallPosFiducial);
   if (middleMedialWallNodeIndex < 0) {
      throw BrainModelAlgorithmException(
         "Unable to find node on fiducial surface nearest middle of medial wall");
   }
                                                   
   //
   // The borders projected to the spherical surface
   //
   //BorderProjectionFile borderProjectionFile = brainSet->getBorderProjectionFile();
   
   //
   // Delete any existing borders and copy the border file to the surface
   //
   BrainModelBorderSet* bmbs = brainSet->getBorderSet();
   brainSet->deleteAllBorders();
   bmbs->copyBordersFromBorderFile(flattenSurface, borderFile);
   
   //
   // Color the borders
   //
   bmbs->assignColors();
   
   //
   // Project the borders onto the spherical surface
   //
   bmbs->projectBorders(flattenSurface);
   
   //
   // Orient the sphere so that the middle of the medial wall is on the positive Z axix
   //
   CoordinateFile* flattenCoordinates = flattenSurface->getCoordinateFile();
   const float* middleMedialWallSphereCoord = 
      flattenCoordinates->getCoordinate(middleMedialWallNodeIndex);
   flattenSurface->orientPointToNegativeZAxis(middleMedialWallSphereCoord);
   
   //
   // Compress the medial wall
   //
   flattenSurface->convertSphereToCompressedMedialWall(0.5);

   //
   // Rotate the surface so that the medial wall faces the user
   //
   TransformationMatrix tm;
   tm.rotate(TransformationMatrix::ROTATE_Y_AXIS, 180.0);
   if (flattenSurface->getStructure() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
      tm.rotate(TransformationMatrix::ROTATE_Z_AXIS, 90.0);
   }
   else {
      tm.rotate(TransformationMatrix::ROTATE_Z_AXIS, -90.0);
   }
   flattenSurface->applyTransformationMatrix(tm);
   
   //
   // Save the compressed medial wall coordinates and move all with positive Z
   // to the X-Y plane
   //
   flattenSurface->pushCoordinates();
   flattenSurface->projectCoordinatesToPlane(
                   BrainModelSurface::COORDINATE_PLANE_MOVE_POSITIVE_Z_TO_ZERO);
       
   //
   // Update the surface's normals
   //
   flattenSurface->computeNormals();
   
   //
   // Save the compressed medial wall surface
   //
   const QString cmwCoordName = 
      FileUtilities::changeCaretDataFileDescriptionType(originalCoordFileName, "CMW");
   try {
      brainSet->writeCoordinateFile(cmwCoordName,
                                    BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL,
                                    flattenSurface->getCoordinateFile());
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }

   //
   // Copy the surface
   //
   //moved to executePart2() 11/07/06
   //flattenSurface = new BrainModelSurface(*flattenSurface);
   //brainSet->addBrainModel(flattenSurface);
   
   //
   // Apply the border projection file to the all surfaces
   //
   bmbs->unprojectBordersForAllSurfaces();
   
   //
   // clear the border projection file
   //
   //borderProjectionFile->clear();
   
   //
   // Make sure there is some sort of coloring
   //
   SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
   if (ssf->getNumberOfColumns() > 0) {
      brainSet->getSurfaceUnderlay()->setOverlay(-1, 
                     BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE);
   }
   else {
      PaintFile* pf = brainSet->getPaintFile();
      if (pf->getNumberOfColumns() > 0) {
         brainSet->getSurfaceUnderlay()->setOverlay(-1, 
                     BrainModelSurfaceOverlay::OVERLAY_PAINT);            
      }
   }
}

      
/// Execute the second part of the flattening
void 
BrainModelSurfaceFlattenFullHemisphere::executePart2() 
                            throw (BrainModelAlgorithmException)
{
   const int numNodes = flattenSurface->getNumberOfNodes();
   
   //
   // Get the compressed medial wall border file
   //
   BrainModelBorderSet* bmbs = brainSet->getBorderSet();
   BorderFile* cmwBorder = bmbs->copyBordersOfSpecifiedType(
                            BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL);
   if (cmwBorder == NULL) {
      throw BrainModelAlgorithmException("Unable to find compressed medial wall border.");
   }
   
   //
   // Copy the surface
   //
   //moved here 11/7/06
   flattenSurface = new BrainModelSurface(*flattenSurface);
   brainSet->addBrainModel(flattenSurface);

   //
   // Copy the topology file
   //
   const TopologyFile* oldTopology = flattenSurface->getTopologyFile();
   TopologyFile* topology = new TopologyFile(*oldTopology);
   topology->setTopologyType(TopologyFile::TOPOLOGY_TYPE_OPEN);
   brainSet->addTopologyFile(topology);
   flattenSurface->setTopologyFile(topology);
   
   //
   // Save the cmw border
   //
   QString templateCutsBorderFileName;
   {
      QString directory, species, casename, anatomy, hemisphere, description;
      QString descriptionNoType, theDate, numNodes, extension;
      if (FileUtilities::parseCaretDataFileName(originalTopoFileName, directory,
                                                species, casename, anatomy, hemisphere,
                                                description, descriptionNoType, theDate,
                                                numNodes, extension)) {
         templateCutsBorderFileName = FileUtilities::reassembleCaretDataFileName(directory,
                                                      species, casename, anatomy, hemisphere,
                                                      "TemplateCuts", theDate, numNodes, 
                                                      SpecFile::getBorderFileExtension());
      }
      else {
         std::ostringstream str;
         str << "species.case."
            << hemStr.toAscii().constData()
            << ".TemplateCuts."
            << numNodes.toAscii().constData()
            << SpecFile::getBorderFileExtension().toAscii().constData();
         templateCutsBorderFileName = str.str().c_str();
      }
   }
   brainSet->writeBorderFile(templateCutsBorderFileName, flattenSurface,
                             BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL, "", "");
   
   //
   // Find the border named medial wall
   //
   const int medialWallBorderIndex = cmwBorder->getBorderIndexByName(medialWallName);
   if (medialWallBorderIndex < 0) {
      throw BrainModelAlgorithmException("Unable to find MEDIAL.WALL border.");
   }
   Border* medialWallBorder = cmwBorder->getBorder(medialWallBorderIndex);
   
   //
   // Find all nodes within the medial wall
   //
   CoordinateFile* flattenCoordinates = flattenSurface->getCoordinateFile();
   std::vector<bool> nodesInsideMedialWallBorder(numNodes, false);
   medialWallBorder->pointsInsideBorder2D(flattenCoordinates->getCoordinate(0), numNodes,
                                        nodesInsideMedialWallBorder, true);
   
   //
   // Set geography for all nodes within the medial wall to MEDIAL.WALL
   //
   PaintFile* paintFile = brainSet->getPaintFile();
   for (int i = 0; i < numNodes; i++) {
      if (nodesInsideMedialWallBorder[i]) {
         paintFile->setPaint(i, geographyColumnNumber, medialWallPaintFileIndex);
      }
   }
   
   //
   // Save the paint file
   //
   QString paintFileName(paintFile->getFileName());
   if (paintFileName.isEmpty() || paintFileEmptyAtStart) {
      QString directory, species, casename, anatomy, hemisphere, description;
      QString descriptionNoType, theDate, numNodes, extension;
      if (FileUtilities::parseCaretDataFileName(originalTopoFileName, directory,
                                                species, casename, anatomy, hemisphere,
                                                description, descriptionNoType, theDate,
                                                numNodes, extension)) {
         paintFileName = FileUtilities::reassembleCaretDataFileName(directory,
                                                      species, casename, anatomy, hemisphere,
                                                      "geography", theDate, numNodes, 
                                                      SpecFile::getAreaColorFileExtension());
      }
      else {
         std::ostringstream str;
         str << "species.case."
            << hemStr.toAscii().constData()
            << ".geography."
            << numNodes.toAscii().constData()
            << SpecFile::getPaintFileExtension().toAscii().constData();
         paintFileName = str.str().c_str();
      }
   }
   brainSet->writePaintFile(paintFileName);

   //
   // Should the fiducial surface have its medial wall smoothed ?
   //
   if (smoothFiducialMedialWallFlag) {
      //
      // Smooth the medial wall of the fiducial surface
      //
      fiducialSurface->arealSmoothing(1.0, 500, 0, &nodesInsideMedialWallBorder);
      fiducialSurface->computeNormals();
      
      //
      // Write the fiducial coord file
      //
      try {
         CoordinateFile* cf = fiducialSurface->getCoordinateFile();
         cf->writeFile(cf->getFileName());
      }
      catch (FileException& e) {
         std::cout << "ERROR: Unable to write fiducial coordinate file after "
                   << "smoothing the medial wall: " << e.whatQString().toAscii().constData() << std::endl;
      }
   }
   
   //
   // Save the geography since applying cuts may modify it
   //
   std::vector<int> savedGeography(numNodes);
   for (int i = 0; i < numNodes; i++) {
      savedGeography[i] = paintFile->getPaint(i, geographyColumnNumber);
   }
   
   //
   // Save the medial wall, calcarine cut, and the frontal cut borders
   //
   saveBordersForLandmarks(cmwBorder);
   
   //
   // Remove the medial wall border from all borders
   //
   std::vector<QString> namesToDelete;
   namesToDelete.push_back(medialWallName);
   bmbs->deleteBordersWithNames(namesToDelete);
   
   //
   // Apply all of the cuts
   //
   BrainModelSurfaceCutter cutter(flattenSurface, cmwBorder, 
                                  BrainModelSurfaceCutter::CUTTING_MODE_NON_NEGATIVE_Z_ONLY);
   try {
      cutter.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      throw e;
   }
   
   //
   // Restore the geography
   //
   for (int i = 0; i < numNodes; i++) {
      paintFile->setPaint(i, geographyColumnNumber, savedGeography[i]);
   }
   paintFile->clearModified();
   
   //
   // Create the landmark borders for deformation
   //
   createDeformationBorders(paintFile);
   
   //
   // Restore the coordinates that were saved when positives projected flat
   //
   flattenSurface->popCoordinates();
   
   //
   // update the normals
   //
   flattenSurface->computeNormals();
   
   //
   // Orient the sphere so that the medial wall is on the negative Z axis
   //
   std::vector<QString> paintNames;
   paintNames.push_back(medialWallName);
   QString errorMessage;
   if (flattenSurface->orientPaintedNodesToNegativeZAxis(brainSet->getPaintFile(),
                                                         paintNames,
                                                         geographyColumnNumber,
                                                         errorMessage)) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   
   //
   // Remove the medial wall
   //
   brainSet->disconnectNodes(topology, nodesInsideMedialWallBorder);
   
   //
   // Remove any stragglers (tiles that are connected to other tiles only at a single node)
   //
   flattenSurface->getTopologyFile()->removeCornerTiles(2);
   flattenSurface->moveDisconnectedNodesToOrigin();

   //
   // Project the sphere flat
   //
   flattenSurface->convertSphereToFlat();
   
   //
   // Smooth just a little bit
   //
   flattenSurface->arealSmoothing(1.0, 5, 0);
   
   //
   // update the normals
   //
   flattenSurface->computeNormals();
   
   //
   // Scale the surface to 10x the fiducial area
   //
   flattenSurface->scaleSurfaceToArea(10.0 * fiducialSurface->getSurfaceArea(), false);

   //
   // Translate to center of mass and scale to fit the window
   //
   flattenSurface->translateToCenterOfMass();
   flattenSurface->updateForDefaultScaling();
         
   //
   // Remove any stragglers (tiles that are connected to other tiles only at a single node)
   //
  // flattenSurface->getTopologyFile()->removeCornerTiles(2);
  // flattenSurface->moveDisconnectedNodesToOrigin();
   
   //
   // Classify the nodes
   //
   brainSet->classifyNodes(topology);
   
   //
   // Save the topology file
   //
   const QString cutTopoName = 
      FileUtilities::changeCaretDataFileDescriptionType(originalTopoFileName, "CUT");
   try {
      brainSet->writeTopologyFile(cutTopoName,
                                  TopologyFile::TOPOLOGY_TYPE_CUT,
                                  topology);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   //
   // Save the flat coordinate file
   //
   const QString flatCoordName = 
      FileUtilities::changeCaretDataFileDescriptionType(originalCoordFileName, "InitialFlat");
   try {
      brainSet->writeCoordinateFile(flatCoordName,
                                    BrainModelSurface::SURFACE_TYPE_FLAT,
                                    flattenSurface->getCoordinateFile());
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   //
   // Set all borders as unmodified
   //
   bmbs->setAllModifiedStatus(false);
}

/**
 * Finds and save the borders for creating landmark borders.  The dorsal and ventral 
 * portions of the medial wall border are determined and the part of the calcarine
 * cut that is outside the medial wall.
 */
void 
BrainModelSurfaceFlattenFullHemisphere::saveBordersForLandmarks(BorderFile* bf)
{
   landmarksForDeformation.clear();
   
   int medialWallIndex   = -1;
   int calcarineCutIndex = -1;
   int frontalCutIndex   = -1;
   
   //
   // Find the MEDIAL.WALL, CalcarineCut, and FrontalCut borders.
   //
   const int numBorders = bf->getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      Border* b = bf->getBorder(i);
      if (b->getNumberOfLinks() > 0) {
         if (b->getName() == medialWallName) {
            //landmarksForDeformation.addBorder(*b);
            medialWallIndex = i;
         }
         else if (b->getName() == calcarineCutName) {
            //landmarksForDeformation.addBorder(*b);
            calcarineCutIndex = i;
         }
         else if (b->getName() == frontalCutName) {
            //landmarksForDeformation.addBorder(*b);
            frontalCutIndex = i;
         }
      }
   }
   
   //
   // If borders missing, print error messages
   //
   if (medialWallIndex < 0) {
         std::cout << "Missing " << medialWallName.toAscii().constData() << " border for deformation landmarks."
                  << std::endl;
   }
   if (calcarineCutIndex < 0) {
         std::cout << "Missing " << calcarineCutName.toAscii().constData() << " border for deformation landmarks."
                  << std::endl;
   }
   if (frontalCutIndex < 0) {
         std::cout << "Missing " << frontalCutName.toAscii().constData() << " border for deformation landmarks."
                  << std::endl;
   }

   //
   // If borders found 
   //
   if ((medialWallIndex >= 0) && (calcarineCutIndex >= 0) && (frontalCutIndex >= 0)) {
      //
      // Find out where medial wall border intersects calcarine and frontal
      //
      Border* medialWallBorder = bf->getBorder(medialWallIndex);
      
      Border* calcarineCutBorder = bf->getBorder(calcarineCutIndex);
      
      Border* frontalCutBorder = bf->getBorder(frontalCutIndex);
      
      //
      // NOTE: When viewing the compressed medial wall surface, the surface is presented in
      // the X-Y plane.      
      //
      // End of calcarine cut border should be most negative X for left, most positive X for right
      //
      float calc0[3], calcLast[3];
      calcarineCutBorder->getLinkXYZ(0, calc0);
      calcarineCutBorder->getLinkXYZ(calcarineCutBorder->getNumberOfLinks() - 1, calcLast);
      if (flattenSurface->getStructure() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
         if (calc0[0] > calcLast[0]) {
            calcarineCutBorder->reverseBorderLinks();
         }
      }      
      else {
         if (calc0[0] < calcLast[0]) {
            calcarineCutBorder->reverseBorderLinks();
         }
      }
      
      //
      // Indices to border links where borders intersect
      //
      int medialWallCalcarineIntersect = -1;
      int medialWallFrontalIntersect   = -1;
      int calcarineMedialWallIntersect = -1;
      int frontalMedialWallIntersect   = -1;
      
      //
      // Find intersections
      //
      const bool medialWallCalcarineIntersectionValid =
         medialWallBorder->intersection2D(calcarineCutBorder, 
                                          true,
                                          false,
                                          medialWallCalcarineIntersect,
                                          calcarineMedialWallIntersect);
      const bool medialWallFrontalIntersectionValid =
         medialWallBorder->intersection2D(frontalCutBorder,
                                          true,
                                          false,
                                          medialWallFrontalIntersect,
                                          frontalMedialWallIntersect);
       
      if (medialWallCalcarineIntersectionValid == false) {
         std::cout << "Unable to find medial wall calacarine cut intersection." << std::endl;
      }
      if (medialWallFrontalIntersectionValid == false) {
         std::cout << "Unable to find medial wall frontal cut intersection." << std::endl;
      }
      
      if (medialWallCalcarineIntersectionValid && medialWallFrontalIntersectionValid) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Found medial wall intersections with both the calcarine and " 
                      << "frontal cuts." << std::endl;
         }
         
         //
         // Get the two pieces of the medial wall border
         //
         Border* dorsalMedialWall = medialWallBorder->getSubSet(medialWallCalcarineIntersect,
                                                                medialWallFrontalIntersect);
         if (dorsalMedialWall == NULL) {
            std::cout << "Calcarine to Frontal section of medial wall border is empty" << std::endl;
         }
         Border* ventralMedialWall = medialWallBorder->getSubSet(medialWallFrontalIntersect,
                                                                 medialWallCalcarineIntersect);
         if (ventralMedialWall == NULL) {
            std::cout << "Frontal to Calcarine section of medial wall border is empty" << std::endl;
            if (dorsalMedialWall != NULL) {
               delete dorsalMedialWall;
            }
         }
         
         if ((dorsalMedialWall != NULL) && (ventralMedialWall != NULL)) {
            //
            // Find out which one is dorsal (biggest Y) and ventral (smallest Y)
            //
            float dorsalBounds[6], ventralBounds[6];
            dorsalMedialWall->getBounds(dorsalBounds);
            ventralMedialWall->getBounds(ventralBounds);
            if (dorsalBounds[3] < ventralBounds[3]) {
               //
               // Switch the pointers
               //
               std::swap(dorsalMedialWall, ventralMedialWall);
            }

            //
            // Set the names of the borders
            //
            dorsalMedialWall->setName(medialWallDorsalLandmark);
            ventralMedialWall->setName(medialWallVentralLandmark);
            
            //
            // Reverse links of dorsal border if necessary
            // Most positive X should be at beginning of border for left, most negative for right
            //
            float dorsal0[3], dorsalLast[3];
            dorsalMedialWall->getLinkXYZ(0, dorsal0);
            dorsalMedialWall->getLinkXYZ(dorsalMedialWall->getNumberOfLinks() - 1, dorsalLast);
            if (flattenSurface->getStructure() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
               if (dorsal0[0] < dorsalLast[0]) {
                  dorsalMedialWall->reverseBorderLinks();
               }
            }
            else {
               if (dorsal0[0] > dorsalLast[0]) {
                  dorsalMedialWall->reverseBorderLinks();
               }
            }

            //
            // Reverse links of ventral border if necessary
            // Most negative X should be at beginning of border for left, most positive for right
            //
            float ventral0[3], ventralLast[3];
            ventralMedialWall->getLinkXYZ(0, ventral0);
            ventralMedialWall->getLinkXYZ(ventralMedialWall->getNumberOfLinks() - 1, ventralLast);
            if (flattenSurface->getStructure() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
               if (ventral0[0] > ventralLast[0]) {
                  ventralMedialWall->reverseBorderLinks();
               }
            }
            else {
               if (ventral0[0] < ventralLast[0]) {
                  ventralMedialWall->reverseBorderLinks();
               }
            }
            
            //
            // Get the calcarine piece from its start to the medial wall
            // 
            Border* calcarinePiece = calcarineCutBorder->getSubSet(0, calcarineMedialWallIntersect - 1);
            calcarinePiece->setName(calcarineLandmark);
            
            //
            // save the borders
            //
            landmarksForDeformation.addBorder(*dorsalMedialWall);
            landmarksForDeformation.addBorder(*ventralMedialWall);
            landmarksForDeformation.addBorder(*calcarinePiece);
            
            //
            // free up memory
            //
            delete dorsalMedialWall;
            delete ventralMedialWall;
            delete calcarinePiece;
         }
      }
   }
}

/**
 * create the landarks for deformation
 */
void 
BrainModelSurfaceFlattenFullHemisphere::createDeformationBorders(PaintFile* pf)
{   
   const int numNodes = flattenSurface->getNumberOfNodes();
   
   //
   // Find the medial wall nodes
   //
   std::vector<bool> nonMedialWallNodes(numNodes, true);
   for (int i = 0; i < numNodes; i++) {
      if (pf->getPaint(i, geographyColumnNumber) == medialWallPaintFileIndex) {
         nonMedialWallNodes[i] = false;
      }
   }
   
   //
   // Create a point locator
   //
   BrainModelSurfacePointLocator locator(flattenSurface, true, &nonMedialWallNodes);
   
   //
   // Initialize a border projection file
   //
   BorderProjectionFile bpf;
   
   //
   // Convert the landmarks to a border projection file by dragging each border
   // link to the nearest node.
   //
   for (int m = 0; m < landmarksForDeformation.getNumberOfBorders(); m++) {
      const Border* b = landmarksForDeformation.getBorder(m);
      
      //
      // Create the border projection with the borders attributes
      //
      QString name;
      float center[3], sampling, variance, topographyValue, arealUncertainty;
      b->getData(name, center, sampling, variance, topographyValue, arealUncertainty);
      BorderProjection bp(b->getName(), center, sampling, variance, topographyValue, arealUncertainty);
      
      //
      // Project each border link to the nearest node and save as a border projection link
      //
      int previousNode = -1;
      for (int i = 0; i < b->getNumberOfLinks(); i++) {
         const int nearestNode = locator.getNearestPoint(b->getLinkXYZ(i));
         if (nearestNode != previousNode) {
            const int vertices[3] = { nearestNode, nearestNode, nearestNode };
            const float areas[3] = { 1.0, 0.0, 0.0 };
            BorderProjectionLink bpl(b->getLinkSectionNumber(i), vertices, areas, b->getLinkRadius(i));
            bp.addBorderProjectionLink(bpl);
         }
         previousNode = nearestNode;
      }
      
      //
      // If this projection is not empty
      //
      if (bp.getNumberOfLinks() > 0) {
         //
         // If this is a medial wall border
         //
         if ((bp.getName() == medialWallDorsalLandmark) ||
             (bp.getName() == medialWallVentralLandmark)) {
            //
            // Remove is last link
            //
            bp.removeLastBorderProjectionLink();
         }
         
         //
         // Add the border projection to the file
         //
         bpf.addBorderProjection(bp);
      }
   }
   
   //
   // Save the landmarks in a border projection file
   //
   if (bpf.getNumberOfBorderProjections()) {
      QString borderProjectionFileName;
      QString directory, species, casename, anatomy, hemisphere, description;
      QString descriptionNoType, theDate, numNodes, extension;
      if (FileUtilities::parseCaretDataFileName(originalTopoFileName, directory,
                                                species, casename, anatomy, hemisphere,
                                                description, descriptionNoType, theDate,
                                                numNodes, extension)) {
         borderProjectionFileName = FileUtilities::reassembleCaretDataFileName(directory,
                                                      species, casename, anatomy, hemisphere,
                                                      "LANDMARKS.FromFlattening", theDate, numNodes, 
                                                      SpecFile::getBorderProjectionFileExtension());
      }
      else {
         std::ostringstream str;
         str << "species.case."
            << hemStr.toAscii().constData()
            << ".LANDMARKS.FromFlattening."
            << numNodes.toAscii().constData()
            << SpecFile::getBorderProjectionFileExtension().toAscii().constData();
         borderProjectionFileName = str.str().c_str();
      }
      
      bpf.writeFile(borderProjectionFileName);
      brainSet->addToSpecFile(SpecFile::borderProjectionFileTag, borderProjectionFileName);
   }
}

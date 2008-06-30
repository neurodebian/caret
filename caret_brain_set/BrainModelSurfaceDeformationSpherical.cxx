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
#include <sstream>

#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include "AreaColorFile.h"
#include "BorderFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceDeformationSpherical.h"
#include "BrainModelSurfaceMorphing.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfacePointProjector.h"
#include "BrainSet.h"
#include "ColorFile.h"
#include "DebugControl.h"
#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "PaintFile.h"
#include "StringUtilities.h"

//static const int morphAngleForceAlgorithm = 1;

/**
 * Constructor.
 */
BrainModelSurfaceDeformationSpherical::BrainModelSurfaceDeformationSpherical(
                                 BrainSet* brainSetIn,
                                 DeformationMapFile* deformationMapFileIn)
   : BrainModelSurfaceDeformation(brainSetIn, deformationMapFileIn)
{
}

/**
 * Destructor.
 */
BrainModelSurfaceDeformationSpherical::~BrainModelSurfaceDeformationSpherical()
{
}

/**
 * Determine distortion ratio of fiducial vs spherical tile areas.
 */
void
BrainModelSurfaceDeformationSpherical::determineSphericalDistortion(
                                         const BrainModelSurface* fiducialSurface,
                                         const BrainModelSurface* sphericalSurface,
                                         std::vector<float>& tileDistortion)
{
   //
   // Get topology file and number of tiles
   //
   const TopologyFile* tf = sphericalSurface->getTopologyFile();
   const int numTiles = tf->getNumberOfTiles();
   tileDistortion.resize(numTiles);
   
   //
   // Get the coordinate files
   //
   const CoordinateFile* fiducialCoords = fiducialSurface->getCoordinateFile();
   const CoordinateFile* sphericalCoords = sphericalSurface->getCoordinateFile();
   
   //
   // Determine fiducial/spherical area ratio of tiles 
   //   
   for (int i = 0; i < numTiles; i++) {
      int v1, v2, v3;
      tf->getTile(i, v1, v2, v3);
      
      const float sphereArea = MathUtilities::triangleArea((float*)sphericalCoords->getCoordinate(v1),
                                                         (float*)sphericalCoords->getCoordinate(v2),
                                                         (float*)sphericalCoords->getCoordinate(v3));
      tileDistortion[i] = 1.0;
      if (sphereArea != 0.0) {
         const float fidArea = MathUtilities::triangleArea((float*)fiducialCoords->getCoordinate(v1),
                                                         (float*)fiducialCoords->getCoordinate(v2),
                                                         (float*)fiducialCoords->getCoordinate(v3));
         tileDistortion[i] = fidArea / sphereArea;
      }
   }
}

/**
 * Load the regularly tessellated sphere and set its radius.
 */
void
BrainModelSurfaceDeformationSpherical::loadRegularSphere() throw (BrainModelAlgorithmException)
{
   //
   // Construct the regular sphere file name
   //
   QString specFileName(brainSet->getCaretHomeDirectory());
   specFileName.append("/");
   specFileName.append("data_files/REGISTER.SPHERE");
   specFileName.append("/");  
   switch(deformationMapFile->getSphereResolution()) {
      case 20:
         specFileName.append("sphere.v5.0.spec");
         break;
      case 74:
         specFileName.append("sphere.v5.1.spec");
         break;
      case 290:
         specFileName.append("sphere.v5.2.spec");
         break;
      case 1154:
         specFileName.append("sphere.v5.3.spec");
         break;
      case 4610:
         specFileName.append("sphere.v5.4.spec");
         break;
      case 18434:
         specFileName.append("sphere.v5.5.spec");
         break;
      case 73730:
         specFileName.append("sphere.v5.6.spec");
         break;
      default:
         {
            std::ostringstream str;
            str << "Invalid sphere resolution: "
               << deformationMapFile->getSphereResolution();
            throw BrainModelAlgorithmException(str.str().c_str());
         }
         break;
   }
   
   //
   // Read the spec file
   //
   SpecFile sf;
   try {
      sf.readFile(specFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
   sf.setAllFileSelections(SpecFile::SPEC_TRUE);
   
   //
   // Read the spec file into "this" brain set
   //
   std::vector<QString> errorMessages;
   brainSet->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                          sf, specFileName, errorMessages, NULL, NULL);
   if (errorMessages.empty() == false) {
      QString msg("Error reading data files for ");
      msg.append(specFileName);
      msg.append("\n");
      msg.append(StringUtilities::combine(errorMessages, "\n"));
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // Get the spherical surface and set its radius
   //
   targetDeformationSphere = brainSet->getBrainModelSurface(0);
   if (targetDeformationSphere == NULL) {
      throw BrainModelAlgorithmException("Regular sphere spec contained no coord file.");
   }
   targetDeformationSphere->convertToSphereWithRadius(deformationSphereRadius);
   targetDeformationSphere->updateForDefaultScaling();
   brainSet->drawBrainModel(targetDeformationSphere);
}

/**
 * Tessellate the target border into the target deformation sphere
 */
void
BrainModelSurfaceDeformationSpherical::tessellateTargetBordersIntoDeformationSphere() 
                                                        throw (BrainModelAlgorithmException)
{
   //
   // empty contents of the used and ignored border links
   //
   usedBorderLinks.clear();
   
   //
   // Create a Point Projector with nodes to be added for the deformation sphere.
   //
   BrainModelSurfacePointProjector bspp(targetDeformationSphere,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                           true);
   TopologyFile* tf = targetDeformationSphere->getTopologyFile();
   
   //
   // Project each border link into the deformation sphere
   //
   const int numBorders = targetBorderFile->getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      const Border* b = targetBorderFile->getBorder(i);
      
      const int numLinks = b->getNumberOfLinks();
      for (int j = 0; j < numLinks; j++) {
         const float* xyz = b->getLinkXYZ(j);
         
         //
         // Project target node onto source deformed surface
         //
         int nearestNode = -1;
         int tileNodes[3];
         float tileAreas[3];
         const int tile = bspp.projectBarycentric(xyz, nearestNode,
                                                tileNodes, tileAreas, true);
          
         const int newNodeNumber = targetDeformationSphere->getNumberOfNodes();
         
         //
         // Unproject using the deformed source coordinate file
         //
         if (tile >= 0) {
            //
            // Get the tile
            //
            int v1, v2, v3;
            tf->getTile(tile, v1, v2, v3);
            
            //
            // Create two new tiles
            //
            const int tn1[3] = { v1, v2, newNodeNumber };
            const int tn2[3] = { v2, v3, newNodeNumber };
            const int tn3[3] = { v3, v1, newNodeNumber };
            
            //
            // Replace the original tile and create two new ones
            //
            tf->setTile(tile, tn1);
            tf->addTile(tn2);
            tf->addTile(tn3);
            
            //
            // Add the node to the surface
            //
            targetDeformationSphere->addNode(xyz);
            
            //
            // Keep track of border link assignments
            //
            usedBorderLinks.push_back(std::make_pair(i, j));
         }
         else {
            if (nearestNode >= 0) {
               std::cout << "Border link with closest node ignored: " << i << " " << j << std::endl;
            }
            else {
               std::cout << "Border link without closest node ignored: " << i << " " << j << std::endl;
            }
         }
      }
   }
   
   const int numNodes = targetDeformationSphere->getNumberOfNodes();
   if (numNodes == originalNumberOfNodes) {
      throw BrainModelAlgorithmException("Tessellating in border nodes failed.");
   }
   
   //
   // Make sure all nodes are on the sphere
   //
   targetDeformationSphere->convertToSphereWithRadius(deformationSphereRadius);
   
   //
   // Update the surface's normals and update node attributes for changes in nodes.
   //
   targetDeformationSphere->computeNormals();
   brainSet->resetNodeAttributes();
   
   //
   // Create the spec file name
   //
   QString specFileName(debugTargetFileNamePrefix);
   specFileName.append("_withLandmarks.spec");
   brainSet->setSpecFileName(specFileName);
   intermediateFiles.push_back(specFileName);
   
   //
   // Setup the paint file that shows the landmark nodes
   //
   PaintFile* pf = brainSet->getPaintFile();
   pf->setNumberOfNodesAndColumns(numNodes, 1);
   pf->setColumnName(0, "Landmarks");
   const int nonLandmarkPaintIndex = pf->addPaintName("???");
   const int landmarkPaintIndex    = pf->addPaintName("Landmark");
   for (int i = 0; i < numNodes; i++) {
      if (i < originalNumberOfNodes) {
         pf->setPaint(i, 0, nonLandmarkPaintIndex);
      }
      else {
         pf->setPaint(i, 0, landmarkPaintIndex);
      }
   }
   QString paintFileName(debugTargetFileNamePrefix);
   paintFileName.append("_withLandmarks");
   paintFileName.append(SpecFile::getPaintFileExtension());
   brainSet->writePaintFile(paintFileName);
   intermediateFiles.push_back(paintFileName);
      
   //
   // Setup the node color file
   //
   AreaColorFile* cf = brainSet->getAreaColorFile();
   cf->addColor("Landmark", 0,   255,   0, 2, 1);
   cf->addColor("???",      170, 170, 170, 2, 1);
   QString nodeColorFileName(debugTargetFileNamePrefix);
   nodeColorFileName.append("_withLandmarks");
   nodeColorFileName.append(SpecFile::getAreaColorFileExtension());
   brainSet->writeAreaColorFile(nodeColorFileName);
   intermediateFiles.push_back(nodeColorFileName);
   
   //
   // Write the topology file
   //
   QString topoFileName(debugTargetFileNamePrefix);
   topoFileName.append("_withLandmarks");
   topoFileName.append(SpecFile::getTopoFileExtension());   
   brainSet->writeTopologyFile(topoFileName, TopologyFile::TOPOLOGY_TYPE_CLOSED, tf);
   intermediateFiles.push_back(topoFileName);
   
   //
   // Write the sphere with target landmarks
   //
   QString coordFileName(debugTargetFileNamePrefix);
   coordFileName.append("_withLandmarks");
   coordFileName.append(SpecFile::getCoordinateFileExtension());   
   brainSet->writeCoordinateFile(coordFileName, BrainModelSurface::SURFACE_TYPE_SPHERICAL, 
                                 targetDeformationSphere->getCoordinateFile());
   intermediateFiles.push_back(coordFileName);
   
   //
   // Set node coloring overlay to paint
   //
   BrainModelSurfaceNodeColoring* bsnc = brainSet->getNodeColoring();
   brainSet->getPrimarySurfaceOverlay()->setOverlay(-1,
                        BrainModelSurfaceOverlay::OVERLAY_PAINT);
   bsnc->assignColors();
   
   //
   // Update the displayed surface
   //
   brainSet->drawBrainModel(targetDeformationSphere);
}

//
// Perform landmark constrained smoothing on the target deformation sphere
//
void
BrainModelSurfaceDeformationSpherical::landmarkConstrainedSmoothTarget()
{
   //
   // Perform the landmark constrained smoothing
   //
   targetDeformationSphere->landmarkConstrainedSmoothing(0.5, 20, landmarkNodeFlags);
   
   //
   // Push the nodes back onto the sphere
   //
   targetDeformationSphere->convertToSphereWithRadius(deformationSphereRadius);
   
   //
   // Save the coordinate file
   //
   QString coordFileName(debugTargetFileNamePrefix);
   coordFileName.append("_withLandmarks_smoothed");
   coordFileName.append(SpecFile::getCoordinateFileExtension());   
   brainSet->writeCoordinateFile(coordFileName, BrainModelSurface::SURFACE_TYPE_SPHERICAL, 
                                 targetDeformationSphere->getCoordinateFile());
   intermediateFiles.push_back(coordFileName);
   
   //
   // Update scaling and display surface
   //
   targetDeformationSphere->updateForDefaultScaling();
   brainSet->drawBrainModel(targetDeformationSphere);
}

/**
 * Determine the fiducial sphere distortion.
 */
void
BrainModelSurfaceDeformationSpherical::determineFiducialSphereDistortion()
{
   //
   // Create a point projector for projecting the deformation sphere nodes
   // onto the target surface
   //
   BrainModelSurfacePointProjector bspp(targetSurface,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                           false);
   
   const int numTargetTiles = static_cast<int>(targetTileDistortion.size());
   
   //
   // Project each node in the deformation sphere
   //   
   const int numNodes = targetDeformationSphere->getNumberOfNodes();
   const CoordinateFile* targetDeformationCoords = targetDeformationSphere->getCoordinateFile();
   for (int i = 0; i < numNodes; i++) {
      float xyz[3];
      targetDeformationCoords->getCoordinate(i, xyz);
      
      //
      // Project target node onto source deformed surface
      //
      int nearestNode = -1;
      int tileNodes[3];
      float tileAreas[3];
      const int tile = bspp.projectBarycentric(xyz, nearestNode,
                                             tileNodes, tileAreas, true);
                                             
      //
      // Did it project ?
      //
      if ((tile >= 0) && (tile < numTargetTiles)) {
         fiducialSphereDistortion.setValue(i, 0, targetTileDistortion[tile]);
      }
      else {
         fiducialSphereDistortion.setValue(i, 0, 1.0);
      }
   }
}

/**
 * Update the fiducial sphere distortion.
 */
void
BrainModelSurfaceDeformationSpherical::updateSphereFiducialDistortion(const int cycle)
{
   //
   // Create a point projector for projecting the source deformation sphere
   // nodes to the original source surface
   //
   BrainModelSurfacePointProjector bspp(sourceSurface,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                           false);
   
   //
   // Project deformation sphere nodes onto original source surface
   //  
   const int numNodes = morphedSourceDeformationSphere->getNumberOfNodes();
   const CoordinateFile* coords = morphedSourceDeformationSphere->getCoordinateFile();
   const int numSourceTiles = static_cast<int>(sourceTileDistortion.size());
   for (int i = 0; i < numNodes; i++) {
      float xyz[3];
      coords->getCoordinate(i, xyz);
      
      //
      // Project target node onto source deformed surface
      //
      int nearestNode = -1;
      int tileNodes[3];
      float tileAreas[3];
      const int tile = bspp.projectBarycentric(xyz, nearestNode,
                                               tileNodes, tileAreas, true);
                                             
      //
      // Did it project ?
      //
      if ((tile >= 0) && (tile < numSourceTiles)) {
         fiducialSphereDistortion.setValue(i, 1, sourceTileDistortion[tile]);
      }
      else {
         fiducialSphereDistortion.setValue(i, 1, 1.0);
      }
      fiducialSphereDistortion.setValue(i, 2,
                                        std::sqrt(fiducialSphereDistortion.getValue(i, 1) /
                                             fiducialSphereDistortion.getValue(i, 0)));
   }
   
   //
   // Save the surface shape file
   //
   std::ostringstream str;
   str << debugTargetFileNamePrefix.toAscii().constData()
       << "_cycle_"
       << cycle
       << SpecFile::getSurfaceShapeFileExtension().toAscii().constData();
   fiducialSphereDistortion.writeFile(str.str().c_str());
   brainSet->addToSpecFile(SpecFile::surfaceShapeFileTag, str.str().c_str());
   intermediateFiles.push_back(str.str().c_str());
}

/**
 * Replace the target landmarks in the deformation sphere with the source landmarks
 */
void
BrainModelSurfaceDeformationSpherical::replaceTargetLandmarksWithSourceLandmarks()
{
   //
   // Copy the surface
   //
   unsmoothedSourceDeformationSphere = new BrainModelSurface(*targetDeformationSphere);
   brainSet->addBrainModel(unsmoothedSourceDeformationSphere);
      
   //
   // Replace target landmark nodes in deformation sphere with source landmark coordinates
   //
   const int numNodes = unsmoothedSourceDeformationSphere->getNumberOfNodes();
   CoordinateFile* sourceCoords = unsmoothedSourceDeformationSphere->getCoordinateFile();
   int borderCounter = 0;
   for (int i = originalNumberOfNodes; i < numNodes; i++) {
      const int borderNumber = usedBorderLinks[borderCounter].first;
      const int linkNumber   = usedBorderLinks[borderCounter].second;
      const Border* sourceBorder = sourceBorderFile->getBorder(borderNumber);
      const float* xyz = sourceBorder->getLinkXYZ(linkNumber);
      sourceCoords->setCoordinate(i, xyz);
      borderCounter++;
   }
   
   //
   // Make sure all nodes are on the sphere
   //
   unsmoothedSourceDeformationSphere->convertToSphereWithRadius(deformationSphereRadius);
   
   //
   // Save the coordinate file
   //
   QString coordFileName(debugSourceFileNamePrefix);
   coordFileName.append("_withLandmarks_initial");
   coordFileName.append(SpecFile::getCoordinateFileExtension());   
   brainSet->writeCoordinateFile(coordFileName, BrainModelSurface::SURFACE_TYPE_SPHERICAL, 
                                 unsmoothedSourceDeformationSphere->getCoordinateFile());
   intermediateFiles.push_back(coordFileName);

   unsmoothedSourceDeformationSphere->updateForDefaultScaling();
   brainSet->drawBrainModel(unsmoothedSourceDeformationSphere);
}

/**
 * Perform landmark neighbor constrained smoothing on the sphere with source landmarks.
 */
void
BrainModelSurfaceDeformationSpherical::landmarkNeighborConstrainedSmoothSource(const int cycleNumber)
{
   //
   // Copy the surface
   //
   smoothedSourceDeformationSphere = new BrainModelSurface(*unsmoothedSourceDeformationSphere);
   brainSet->addBrainModel(smoothedSourceDeformationSphere);
   
   //
   // Get smoothing parameters for this cycle
   //
   float strength;
   int numCycles, numIterations, neighborIterations, numFinalIterations;
   deformationMapFile->getSmoothingParameters(cycleNumber - 1,
                                              strength,
                                              numCycles,
                                              numIterations,
                                              neighborIterations,
                                              numFinalIterations);
   
   smoothedSourceDeformationSphere->updateForDefaultScaling();
      
   //
   // perform number of smoothing cycles
   //
   for (int i = 0; i < numCycles; i++) {
      //
      // Perform the landmark constrained smoothing
      //
      smoothedSourceDeformationSphere->landmarkNeighborConstrainedSmoothing(strength,
                                                                            numIterations,
                                                                            landmarkNodeFlags,
                                                                            neighborIterations,
                                                                            0);

      //
      // push non-landmark nodes back onto the sphere
      //
      smoothedSourceDeformationSphere->convertToSphereWithRadius(deformationSphereRadius,
                                                                 0, originalNumberOfNodes);
   }
   
   //
   // Perform final smoothing
   //
   smoothedSourceDeformationSphere->arealSmoothing(strength,
                                                   numFinalIterations,
                                                   0);
   
   //
   // push all nodes back onto the sphere
   //
   smoothedSourceDeformationSphere->convertToSphereWithRadius(deformationSphereRadius);
   smoothedSourceDeformationSphere->updateForDefaultScaling();

   //
   // Save the coordinate file
   //
   QString coordFileName(debugSourceFileNamePrefix);
   coordFileName.append("_withLandmarks_smoothed");
   coordFileName.append(SpecFile::getCoordinateFileExtension());   
   brainSet->writeCoordinateFile(coordFileName, BrainModelSurface::SURFACE_TYPE_SPHERICAL, 
                                 smoothedSourceDeformationSphere->getCoordinateFile());
   intermediateFiles.push_back(coordFileName);

   smoothedSourceDeformationSphere->updateForDefaultScaling();
   brainSet->drawBrainModel(smoothedSourceDeformationSphere);
}

/**
 * Perform landmark constrained morphing on the sphere with source landmarks
 */
void 
BrainModelSurfaceDeformationSpherical::landmarkMorphContrainedSource(const int cycleNumber)
                                            throw (BrainModelAlgorithmException)
{
   //
   // Get smoothing parameters for this cycle (just need strength)
   //
   float strength;
   int numCycles, numIterations, neighborIterations, numFinalIterations;
   deformationMapFile->getSmoothingParameters(cycleNumber - 1,
                                              strength,
                                              numCycles,
                                              numIterations,
                                              neighborIterations,
                                              numFinalIterations);
   //
   // Get the morphing paramters for this cycle
   //
   float linearForce, angularForce, stepSize, landmarkStepSize;
   int numMorphCycles, iterations, smoothIterations;
   deformationMapFile->getMorphingParameters(cycleNumber - 1,
                                             numMorphCycles,
                                             linearForce,
                                             angularForce,
                                             stepSize,
                                             landmarkStepSize,
                                             iterations,
                                             smoothIterations);
    
   morphedSourceDeformationSphere = NULL;
   
   //
   // If morphing should be performed
   //
   if (numMorphCycles > 0) {
      //
      // Copy the surface
      //
      morphedSourceDeformationSphere = new BrainModelSurface(*smoothedSourceDeformationSphere);
      brainSet->addBrainModel(morphedSourceDeformationSphere);

      
      //
      // NON-landmark nodes are morphed
      //
      const int numNodes = morphedSourceDeformationSphere->getNumberOfNodes();
      std::vector<bool> morphNodeFlag(numNodes);
      for (int i = 0; i < numNodes; i++) {
         morphNodeFlag[i] = ( ! landmarkNodeFlags[i]);
      }
      
      //
      // Get fiducial sphere ratio settings
      //
      bool useFiducialSphereRatios = false;
      float fiducialSphereRatioStrength = 0.5;
      deformationMapFile->getSphereFiducialRatio(useFiducialSphereRatios, fiducialSphereRatioStrength);
      std::vector<float> fiducialSphereRatios;      
      if (useFiducialSphereRatios) {
         //
         // Use fiducial/sphere distortion except on first cycle
         //
         if (cycleNumber > 1) {
            const int numNodes = fiducialSphereDistortion.getNumberOfNodes();
            fiducialSphereRatios.resize(numNodes);
            for (int i = 0; i < numNodes; i++) {
               fiducialSphereRatios[i] = fiducialSphereDistortion.getValue(i, 2);
            }
         }
      }
      
      //
      // Morph for the specified number of cycles
      //
      for (int morphCycle = 0; morphCycle < numMorphCycles; morphCycle++) {
         //
         // Create the morphing object and set parameters
         //      
         BrainModelSurfaceMorphing bmsm(brainSet,
                                        targetDeformationSphere,
                                        morphedSourceDeformationSphere,
                                        BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL);
         bmsm.setMorphingParameters(iterations,
                                     linearForce,
                                     angularForce,
                                     stepSize);
         bmsm.setNodesThatShouldBeMorphed(morphNodeFlag, landmarkStepSize);
         if (fiducialSphereRatios.size() > 0) {
            if (useFiducialSphereRatios) {
               bmsm.setFiducialSphereDistortionCorrections(fiducialSphereRatios, 
                                                           fiducialSphereRatioStrength);
            }
         }
         
         //
         // execute the  morphing
         //
         bmsm.execute();
         
         //
         // Perform a little bit of normal smoothing
         //
         morphedSourceDeformationSphere->arealSmoothing(strength, smoothIterations, 0);
         
         //
         // Push nodes back to sphere and draw
         //
         morphedSourceDeformationSphere->convertToSphereWithRadius(deformationSphereRadius);
         
         //
         // Draw the surface
         //
         morphedSourceDeformationSphere->updateForDefaultScaling();
         brainSet->drawBrainModel(morphedSourceDeformationSphere);
         
      }      
   
      //
      // Save the surface
      //
      QString coordFileName(debugSourceFileNamePrefix);
      coordFileName.append("_withLandmarks_morphed");
      coordFileName.append(SpecFile::getCoordinateFileExtension());   
      brainSet->writeCoordinateFile(coordFileName, BrainModelSurface::SURFACE_TYPE_SPHERICAL, 
                                    morphedSourceDeformationSphere->getCoordinateFile());
      intermediateFiles.push_back(coordFileName);
      
      //
      // Draw the surface
      //
      morphedSourceDeformationSphere->updateForDefaultScaling();
      brainSet->drawBrainModel(morphedSourceDeformationSphere);
   }
   else {
      morphedSourceDeformationSphere = smoothedSourceDeformationSphere;      
   }
}      

/**
 * Deform the source sphere using the source and target deformation spheres
 */
void
BrainModelSurfaceDeformationSpherical::createDeformedCoordinateFile(const int cycle)
{
   try {
      //
      // Copy the user's input surface to a new deformed surface
      //
      deformedSourceSurface = new BrainModelSurface(*sourceSurface);
      sourceBrainSet->addBrainModel(deformedSourceSurface);

      //
      // Make sure source surface is same radius as deformation sphere
      //
      sourceSurface->convertToSphereWithRadius(deformationSphereRadius);
         
      //
      // Coordinates of the morphed source deformation sphere
      //
      //const CoordinateFile* deformationMorphedSphereCoords = 
      //   morphedSourceDeformationSphere->getCoordinateFile();
         
      //
      // Coordinates of the original unsmoothed target deformation sphere
      //
      const CoordinateFile* targetDeformationSphereCoords = 
               unsmoothedTargetDeformationSphere->getCoordinateFile();
               
      //
      // Get the coordinate files for the user's surface and its
      // new deformed surface.
      //
      CoordinateFile* sourceCoords = sourceSurface->getCoordinateFile();
      CoordinateFile* deformedSourceCoords = deformedSourceSurface->getCoordinateFile();
      const int numCoords = sourceCoords->getNumberOfCoordinates();
      
      //
      // See if X coordinate will need to be flipped (first cycle only !!!)
      //
      bool diffHemFlag = false;
      if (cycle == 1) {
         diffHemFlag = (sourceBrainSet->getStructure() != targetBrainSet->getStructure());
      }
      
      //
      // Create a Point Projector for the flat subsampled surface.
      //
      BrainModelSurfacePointProjector bspp(morphedSourceDeformationSphere, //targetDeformationSphere);
                              BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                              false);
      
      //
      // Project each point in the users input surface to its deformed surface
      //
      for (int i = 0; i < numCoords; i++) {
         float xyz[3];
         sourceCoords->getCoordinate(i, xyz);
         if (diffHemFlag) {
            xyz[0] = -xyz[0];
            sourceCoords->setCoordinate(i, xyz); // needed for projecting borders
         }
         
         //
         // Project source node onto subsampled surface
         //
         int nearestNode = -1;
         int tileNodes[3];
         float tileAreas[3];
         const int tile = bspp.projectBarycentric(xyz, nearestNode,
                                                  tileNodes, tileAreas, true);
                                                  
         //
         // Unproject using the deformed source sphere coordinate file
         //
         if (tile >= 0) {
            BrainModelSurfacePointProjector::unprojectPoint(tileNodes, 
                                                            tileAreas,
                                                            targetDeformationSphereCoords, //deformationMorphedSphereCoords, 
                                                            xyz);
         }
         else if (nearestNode >= 0) {
            // JWH 08/08/03  2:15pm deformedSourceCoords->getCoordinate(nearestNode, xyz);
            //deformationMorphedSphereCoords->getCoordinate(nearestNode, xyz);
            targetDeformationSphereCoords->getCoordinate(nearestNode, xyz);
         }
         deformedSourceCoords->setCoordinate(i, xyz);
      }
      
      //
      // Write the source deformed coordinates
      //
      QDir::setCurrent(sourceDirectory);
      QString defCoordName(deformationMapFile->getDeformedFileNamePrefix());
      if (cycle == deformationMapFile->getSphericalNumberOfCycles()) {
         QString dn(FileUtilities::dirname(sourceCoords->getFileName()));
         
         if ((dn != ".") && (dn.length() > 0)) {
            QString fn(FileUtilities::basename(sourceCoords->getFileName()));
            QString s(dn);
            s += QDir::separator();
            s += deformationMapFile->getDeformedFileNamePrefix();
            s += fn;
            defCoordName = s;
         }
         else {
            defCoordName.append(sourceCoords->getFileName());         
         }
      }
      else {
         std::ostringstream str;
         str << "_sphere_cycle_"
             << cycle
             << SpecFile::getCoordinateFileExtension().toAscii().constData();
         defCoordName.append(str.str().c_str());
      }
      deformationMapFile->setSourceDeformedSphericalCoordFileName(defCoordName);
      deformedSourceCoords->appendToFileComment("\nDeformation Map File: ");
      deformedSourceCoords->appendToFileComment(
         FileUtilities::basename(sourceToTargetDeformationMapFileName));
      deformedSourceCoords->writeFile(defCoordName);
      QDir::setCurrent(originalDirectory);   
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
}

/**
 * Update the source borders for the next cycle.
 */
void
BrainModelSurfaceDeformationSpherical::updateSourceBordersForNextCycle()
                                                  throw (BrainModelAlgorithmException)
{
   //
   // Get the deformed source surface coordinates
   //
   const CoordinateFile* deformedCoords = deformedSourceSurface->getCoordinateFile();

   //
   // Create a Point Projector for the source surface.
   //
   BrainModelSurfacePointProjector bspp(sourceSurface,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                           false);
   
   //
   // Project each point in the source borders
   //
   const int numBorders = sourceBorderFile->getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      Border* border = sourceBorderFile->getBorder(i);
      const int numLinks = border->getNumberOfLinks();
      for (int j = 0; j < numLinks; j++) {
         float xyz[3];
         border->getLinkXYZ(j, xyz);
         
         //
         // Project border point onto source surface
         //
         int nearestNode = -1;
         int tileNodes[3];
         float tileAreas[3];
         const int tile = bspp.projectBarycentric(xyz, nearestNode,
                                                tileNodes, tileAreas, true);
                                                
         //
         // Unproject using the deformed source surface
         //
         if (tile >= 0) {
            BrainModelSurfacePointProjector::unprojectPoint(tileNodes, 
                                                            tileAreas,
                                                            deformedCoords, 
                                                            xyz);
         }
         else if (nearestNode >= 0) {
            deformedCoords->getCoordinate(nearestNode, xyz);
         }
         
         //
         // Update border link position
         //
         border->setLinkXYZ(j, xyz);
      }
   }
}
/*
void
BrainModelSurfaceDeformationSpherical::updateSourceBordersForNextCycle()
                                                  throw (BrainModelAlgorithmException)
{
   BorderFile* updatedBorders = new BorderFile;
   
   //
   // The border nodes were tessellated in as nodes and added to the original deformation sphere
   //
   const int numNodes = morphedSourceDeformationSphere->getNumberOfNodes();
   const CoordinateFile* morphSourceCoords = morphedSourceDeformationSphere->getCoordinateFile();
   
   //
   // For new borders that are created
   //
   Border* border = NULL;
   int lastBorderNumber = -1;
   
   //
   // Check nodes that were border links tessellated into the deformation sphere
   //
   for (int i = originalNumberOfNodes; i < numNodes; i++) {
      //
      // For morphed deformation sphere, get the border link that created this node
      //
      const int index = i - originalNumberOfNodes;
      const int borderNumber = usedBorderLinks[index].first;
      
      //
      // Has border changed ?
      //
      if (borderNumber != lastBorderNumber) {
         if (border != NULL) {
            updatedBorders->addBorder(*border);
         }
         const Border* sourceBorder = sourceBorderFile->getBorder(borderNumber);
         border = new Border(sourceBorder->getName());
      }
      lastBorderNumber = borderNumber;
      
      //
      // Add the border's link
      //
      border->addBorderLink(morphSourceCoords->getCoordinate(i));
   }
   if (border != NULL) {
      updatedBorders->addBorder(*border);
   }
      
   if (updatedBorders->getNumberOfBorders() != sourceBorderFile->getNumberOfBorders()) {
      throw BrainModelAlgorithmException("Number of borders changed when moving source "
                                         "border links for next cycle.");
   }
   
   delete sourceBorderFile;
   sourceBorderFile = updatedBorders;
}
*/
 
/**
 * Execute the deformation.
 */
void 
BrainModelSurfaceDeformationSpherical::executeDeformation() throw (BrainModelAlgorithmException)
{
   //
   // Get radius of target sphere
   //
   deformationSphereRadius = targetSurface->getSphericalSurfaceRadius();
   
   //
   // Make source surface the same radius as the target surface
   //
   sourceSurface->convertToSphereWithRadius(deformationSphereRadius);
   
   //
   // control of sphere fiducial distortion
   //
   bool useFiducialSphereRatios = false;
   float fiducialSphereRatioStrength = 0.5;
   deformationMapFile->getSphereFiducialRatio(useFiducialSphereRatios, fiducialSphereRatioStrength);
   
   //
   // Determing the ratio of the source fiducial and spherical tile areas
   //
   if (useFiducialSphereRatios) {
      if (DebugControl::getDebugOn()) {
         std::cout << " " << std::endl;
      }
      determineSphericalDistortion(sourceFiducialSurface,
                                 sourceSurface,
                                 sourceTileDistortion);
      
      //
      // Determing the ratio of the target fiducial and spherical tile areas
      //
      determineSphericalDistortion(targetFiducialSurface,
                                 targetSurface,
                                 targetTileDistortion);
   }
  
   //
   // Perform the requested number of cycles
   //
   for (int cycle = 1; cycle <= deformationMapFile->getSphericalNumberOfCycles(); cycle++) {
      //
      // Debug file name prefixes
      //
      std::ostringstream strT;
      strT << "ia_target_cycle"
           << cycle;
      debugTargetFileNamePrefix = strT.str().c_str();
      std::ostringstream strS;
      strS << "ia_source_cycle"
          << cycle;
      debugSourceFileNamePrefix = strS.str().c_str();
      
      // 
      // Resample the border files
      //
      if (DebugControl::getDebugOn()) {
         std::cout << "Cycle " << cycle << " resampling borders." << std::endl;
      }
      resampleBorderFiles();
      
      //
      // Set the radius of the source/target border file to the radius of the target surface
      //
      sourceBorderFile->setSphericalBorderRadius(deformationSphereRadius);
      targetBorderFile->setSphericalBorderRadius(deformationSphereRadius);
      
      //
      // Load the regularly tessellated sphere and make it same radius as target surface
      //
      QString currentDirectory(QDir::currentPath());
      loadRegularSphere();
      QDir::setCurrent(currentDirectory);
      
      //
      // Save the number of nodes before tessellating in the borders
      //
      originalNumberOfNodes = targetDeformationSphere->getNumberOfNodes();
   
      //
      // tessellate the target borders into the target deformation sphere
      //
      if (DebugControl::getDebugOn()) {
         std::cout << "Cycle " << cycle << " tessellating borders into target." << std::endl;
      }
      tessellateTargetBordersIntoDeformationSphere();
   
      //
      // Copy the target deformed sphere making the current sphere the unsmoothed 
      //
      unsmoothedTargetDeformationSphere = targetDeformationSphere;
      targetDeformationSphere = new BrainModelSurface(*unsmoothedTargetDeformationSphere);
      brainSet->addBrainModel(targetDeformationSphere);
      
      //
      // Mark the landmark nodes
      //
      const int numNodes = targetDeformationSphere->getNumberOfNodes();
      landmarkNodeFlags.resize(numNodes);
      for (int i = 0; i < numNodes; i++) {
         landmarkNodeFlags[i] = (i >= originalNumberOfNodes);
      }
   
      //
      // Perform landmark constrained smoothing.  Only the coordinate nodes are
      // smoothed
      //
      if (DebugControl::getDebugOn()) {
         std::cout << "Cycle " << cycle << " landmark constrained smoothing of target." << std::endl;
      }
      landmarkConstrainedSmoothTarget();
      
      //
      // During first cycle, allocate the fiducial sphere distortion surface shape file
      //
      // Update each cycle since number of nodes might vary by a couple due to projection problems
      //if (cycle == 1) {
      
         if (useFiducialSphereRatios) {
            fiducialSphereDistortion.setNumberOfNodesAndColumns(
                                          targetDeformationSphere->getNumberOfNodes(), 3);
         }
      //}
      
      if (useFiducialSphereRatios) {
         //
         // determine the fiducial sphere distortion
         //
         determineFiducialSphereDistortion();
      }
      
      //
      // Replace the target landmarks in the deformation sphere with the source landmarks
      //
      replaceTargetLandmarksWithSourceLandmarks();
      
      //
      // Perform landmark neighbor constrained smoothing on the sphere with source landmarks
      //
      if (DebugControl::getDebugOn()) {
         std::cout << "Cycle " << cycle << " landmark neighbor constrained smoothing." << std::endl;
      }
      landmarkNeighborConstrainedSmoothSource(cycle);
      
      //
      // Perform landmark constrained morphing on the sphere with source landmarks
      //
      if (DebugControl::getDebugOn()) {
         std::cout << "Cycle " << cycle << " landmark constrained morphing." << std::endl;
      }
      landmarkMorphContrainedSource(cycle);
      
      //
      // Update fiducial/sphere with deformed surface
      //
      if (useFiducialSphereRatios) {
         updateSphereFiducialDistortion(cycle);
      }
         
      //
      // make sure normals point out on source sphere
      //
      morphedSourceDeformationSphere->orientNormalsOut();
      
      //
      // Check for crossovers
      //
      int numNodeCrossovers, numTileCrossovers;
      morphedSourceDeformationSphere->crossoverCheck(numTileCrossovers, 
                                                     numNodeCrossovers,
                                                     BrainModelSurface::SURFACE_TYPE_SPHERICAL);
      crossoverCount.push_back(numNodeCrossovers);
      
      //
      // If the last cycle alert user if there are crossovers
      //
      if (cycle == deformationMapFile->getSphericalNumberOfCycles()) {
         if (numNodeCrossovers > 0) {
            QWidget* parent = brainSet->getProgressDialogParent();
            if (parent != NULL) {
               QString msg;
               for (int i = (crossoverCount.size() - 1); i >= 0; i--) {
                  std::ostringstream str;
                  str << "Cycle " << i << " had " << crossoverCount[i] << " crossovers.\n";
                  msg.append(str.str().c_str());
               }
               msg.append("\nContinue Deformation ?");
               QApplication::restoreOverrideCursor();
               QApplication::beep();
               if (QMessageBox::warning(parent, "Crossover Alert", msg, "Yes", "No") != 0) {
                  throw BrainModelAlgorithmException("Deformation terminated by user.");
               }
               //will not compile on mac - don't know why QApplication::setOverrideCursor(waitCursor);
            }
         }
      }
      
      //
      // Project the user's sphere from the original source deformation sphere to
      // the target deformation sphere
      //
      createDeformedCoordinateFile(cycle);
      
      //
      // If this is not the last cycle
      //
      if (cycle < deformationMapFile->getSphericalNumberOfCycles()) {
         //
         // Update the source borders for the next cycle
         //
         updateSourceBordersForNextCycle();
         
         //
         // Make input spherical coordinate file the one that was just deformed
         //
         const QString coordName = sourceSurface->getCoordinateFile()->getFileName();
         sourceSurface = deformedSourceSurface;
         sourceSurface->getCoordinateFile()->setFileName(coordName);
      }
   }
}
      

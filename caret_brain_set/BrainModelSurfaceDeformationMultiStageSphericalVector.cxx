#include <cmath>

#include "BrainModelSurfaceDeformDataFile.h"
#include <iostream>
#include <sstream>

#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include "AreaColorFile.h"
#include "BorderFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceDeformationMultiStageSphericalVector.h"
#include "BrainModelSurfaceMetricSmoothing.h"
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
#include "TopologyHelper.h"

//static const int morphAngleForceAlgorithm = 1;

/**
 * Constructor.
 */
BrainModelSurfaceDeformationMultiStageSphericalVector::BrainModelSurfaceDeformationMultiStageSphericalVector(
                                 BrainSet* brainSetIn,
                                 DeformationMapFile* deformationMapFileIn)
   : BrainModelSurfaceDeformation(brainSetIn, deformationMapFileIn)
{
   targetDeformationBrainSet = NULL;
   workingSourceSurface = NULL;
}

/**
 * Destructor.
 */
BrainModelSurfaceDeformationMultiStageSphericalVector::~BrainModelSurfaceDeformationMultiStageSphericalVector()
{
   if (targetDeformationBrainSet != NULL) {
      delete targetDeformationBrainSet;
      targetDeformationBrainSet = NULL;
   }
   //if (workingSourceSurface != NULL) {
   //   delete workingSourceSurface;
   //   workingSourceSurface = NULL;
   //}
}

/**
 * Determine distortion ratio of fiducial vs spherical tile areas.
 */
void
BrainModelSurfaceDeformationMultiStageSphericalVector::determineSphericalDistortion(
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
 * Determine the fiducial sphere distortion.
 */
void
BrainModelSurfaceDeformationMultiStageSphericalVector::determineFiducialSphereDistortion()
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
BrainModelSurfaceDeformationMultiStageSphericalVector::updateSphereFiducialDistortion(
                                    const int stageIndex,
                                    const int cycleIndex,
                                    BrainModelSurface* morphedSourceDeformationSphere)
{
   //
   // Create a point projector for projecting the source deformation sphere
   // nodes to the original source surface
   //
   BrainModelSurfacePointProjector bspp(workingSourceSurface,
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
   str << "targetFiducialSphereDistortion"
       << "_stage_"
       << stageIndex + 1
       << "_cycle_"
       << cycleIndex + 1
       << SpecFile::getSurfaceShapeFileExtension().toAscii().constData();
   fiducialSphereDistortion.writeFile(str.str().c_str());
   //brainSet->addToSpecFile(SpecFile::getSurfaceShapeFileTag(), str.str().c_str());
   intermediateFiles.push_back(str.str().c_str());
}

/**
 * Load the regularly tessellated sphere and set its radius.
 */
BrainModelSurface*
BrainModelSurfaceDeformationMultiStageSphericalVector::getRegularSphere(BrainSet* bs,
                             const int stageIndex,
                             const float radius) throw (BrainModelAlgorithmException)
{
   //
   // Construct the regular sphere file name
   //
   QString specFileName(bs->getCaretHomeDirectory());
   specFileName.append("/");
   specFileName.append("data_files/REGISTER.LVD.SPHERE");
   specFileName.append("/");
   switch(deformationMapFile->getSphereResolution(stageIndex)) {
      case 20:
         throw BrainModelAlgorithmException(
              "Resolution 20 not available for Multi-Stage Vector Deformation");
         break;
      case 74:
         specFileName.append("sphere.1.LVD.74.spec");
         break;
      case 290:
         specFileName.append("sphere.2.LVD.290.spec");
         break;
      case 1154:
         specFileName.append("sphere.3.LVD.1154.spec");
         break;
      case 4610:
         specFileName.append("sphere.4.LVD.4610.spec");
         break;
      case 18434:
         specFileName.append("sphere.5.LVD.18434.spec");
         break;
      case 73730:
         specFileName.append("sphere.6.LVD.73730.spec");
         break;
      default:
         {
            std::ostringstream str;
            str << "Invalid sphere resolution: "
                << deformationMapFile->getSphereResolution(stageIndex);
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
   // Read the spec file into "this" brain set (use APPEND for source sphere)
   //
   std::vector<QString> errorMessages;
   bs->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
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
   BrainModelSurface* sphereSurface =
           bs->getBrainModelSurface(0);
   if (sphereSurface == NULL) {
      throw BrainModelAlgorithmException("Regular sphere spec contained no coord file.");
   }
   sphereSurface->convertToSphereWithRadius(radius);
   sphereSurface->updateForDefaultScaling();
   updateViewingTransformation(bs);

   return sphereSurface;
}

/**
 * Tessellate the target border into the target deformation sphere
 */
void
BrainModelSurfaceDeformationMultiStageSphericalVector::tessellateTargetBordersIntoDeformationSphere(
                                         const int stageIndex)
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
   targetDeformationBrainSet->resetNodeAttributes();

   QString targetNamePrefix("target_withLandmarks_stage"
                            + QString::number(stageIndex + 1)
                            + ".LVD");

   //
   // Create the spec file name
   //
   QString specFileName = targetNamePrefix + SpecFile::getSpecFileExtension();
   targetDeformationBrainSet->setSpecFileName(specFileName);
   intermediateFiles.push_back(specFileName);
   targetDeformationBrainSet->removeCoordAndTopoFromSpecFile();

   //
   // Setup the paint file that shows the landmark nodes
   //
   PaintFile* pf = targetDeformationBrainSet->getPaintFile();
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
   QString paintFileName(targetNamePrefix + SpecFile::getPaintFileExtension());
   targetDeformationBrainSet->writePaintFile(paintFileName);
   intermediateFiles.push_back(paintFileName);

   //
   // Setup the node color file
   //
   AreaColorFile* cf = targetDeformationBrainSet->getAreaColorFile();
   cf->addColor("Landmark", 255,   0,   0, 2, 1);
   cf->addColor("???",      170, 170, 170, 2, 1);
   QString nodeColorFileName(targetNamePrefix + SpecFile::getAreaColorFileExtension());
   targetDeformationBrainSet->writeAreaColorFile(nodeColorFileName);
   intermediateFiles.push_back(nodeColorFileName);

   //
   // Write the topology file
   //
   QString topoFileName(targetNamePrefix + SpecFile::getTopoFileExtension());
   targetDeformationBrainSet->writeTopologyFile(topoFileName, TopologyFile::TOPOLOGY_TYPE_CLOSED, tf);
   intermediateFiles.push_back(topoFileName);

   //
   // Set node coloring overlay to paint
   //
   BrainModelSurfaceNodeColoring* bsnc = targetDeformationBrainSet->getNodeColoring();
   targetDeformationBrainSet->getPrimarySurfaceOverlay()->setOverlay(-1,
                        BrainModelSurfaceOverlay::OVERLAY_PAINT);
   bsnc->assignColors();

   //
   // Update the displayed surface
   //
   targetDeformationSphere->orientTilesConsistently();
   targetDeformationSphere->computeNormals();
   this->updateViewingTransformation(targetDeformationBrainSet);
   targetDeformationSphere->updateForDefaultScaling();
   targetDeformationBrainSet->drawBrainModel(targetDeformationSphere);

   //
   // Write the sphere with target landmarks
   //
   QString coordFileName(targetNamePrefix + SpecFile::getCoordinateFileExtension());
   targetDeformationBrainSet->writeCoordinateFile(coordFileName, BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                 targetDeformationSphere->getCoordinateFile());
   intermediateFiles.push_back(coordFileName);
}

/**
 * Tessellate the source border into the target deformation sphere
 */
void
BrainModelSurfaceDeformationMultiStageSphericalVector::tessellateSourceBordersIntoDeformationSphere(
                                                     const int stageIndex)
                                                        throw (BrainModelAlgorithmException)
{
   int numNodeCrossovers = 0;
   int numTileCrossovers = 0;
   sourceDeformationSphere->crossoverCheck(numTileCrossovers,
                                           numNodeCrossovers,
                   BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   if (numNodeCrossovers > 0) {
      std::cout << "WARNING: Source deformation sphere contains crossovers "
                   "PRIOR to insertion of landmarks."
                << std::endl;
   }

   //
   // Create the surface shape file containing the border variances
   // Use target BrainSet for number of nodes since it has landmarks
   // inserted so that number of nodes is correct.
   //
   borderVarianceValuesShapeFile.setNumberOfNodesAndColumns(targetBrainSet->getNumberOfNodes(),
                                                            1);
   borderVarianceValuesShapeFile.setColumnName(0, "Border Variances");

   float endpointFactor = 1.0;
   this->deformationMapFile->getLandmarkVectorStageParameters(stageIndex, endpointFactor);


   //
   // Create a Point Projector with nodes to be added for the deformation sphere.
   //
   BrainModelSurfacePointProjector bspp(sourceDeformationSphere,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                           true);
   TopologyFile* tf = sourceDeformationSphere->getTopologyFile();

   //
   // Project each border link into the deformation sphere
   //
   const int numBorderLinks = static_cast<int>(usedBorderLinks.size());
   for (int i = 0; i < numBorderLinks; i++) {
      std::pair<int,int> borderLinkNums = usedBorderLinks[i];
      const int borderNumber = borderLinkNums.first;
      const int linkNumber   = borderLinkNums.second;

      const Border* targetBorder = targetBorderFile->getBorder(borderNumber);
      const QString borderName = targetBorder->getName();
      const Border* sourceBorder = sourceBorderFile->getBorderByName(borderName);
      if (sourceBorder == NULL) {
         throw BrainModelAlgorithmException(
                 "Unable to find source border named \"" + borderName + "\"");
      }
      bool projectedFlag = false;

      //
      // Source border coordinate
      //
      float xyz[3];
      sourceBorder->getLinkXYZ(linkNumber, xyz);

      //
      // Identify first and last links in borders
      //
      bool endPointFlag = false;
      const int iPrevious = i - 1;
      if (iPrevious >= 0) {
         //
         // If the previous border number is different, this is the first link
         //
         const int bnum = usedBorderLinks[iPrevious].first;
         if (bnum != borderNumber) {
            endPointFlag = true;
         }
      }
      else {
         //
         // No previous border, then this is first link in first border
         //
         endPointFlag = true;
      }
      const int iNext = i + 1;
      if (iNext < numBorderLinks) {
         //
         // If the next border number is different, this is the last link
         //
         const int bnum = usedBorderLinks[iNext].first;
         if (bnum != borderNumber) {
            endPointFlag = true;
         }
      }
      else {
         //
         // No next border, then this is the last link in the last border
         //
         endPointFlag = true;
      }
      
      //if (endPointFlag) {
      //   std::cout << "Border "
      //             << borderNumber
      //             << " link "
      //             << linkNumber
      //             << " is an endpoint."
      //             << std::endl;
      //}

      //
      // Projecting may fail in some cases so perturb when there is a failure
      //
      const int iTestMaximum = 10;
      for (int iTest = 0; iTest < iTestMaximum; iTest++) {
          //
          // Project target node onto source deformed surface
          //
          int nearestNode = -1;
          int tileNodes[3];
          float tileAreas[3];

          //
          // perturb if problems projecting
          //
          if (iTest > 0) {
             std::cout << "Moving source border point due to projection failure: "
                       << borderName.toAscii().constData()
                       << " link "
                       << linkNumber
                       << " (" << xyz[0]
                       << "," << xyz[1]
                       << "," << xyz[2]
                       << ")" << std::endl;
             xyz[0] += 0.001;
             xyz[1] += 0.001;
             xyz[2] += 0.001;
          }

          const int tile = bspp.projectBarycentric(xyz, nearestNode,
                                                tileNodes, tileAreas, true);

          const int newNodeNumber = sourceDeformationSphere->getNumberOfNodes();

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
             // Create two new tiles and replace the file node is within
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
             sourceDeformationSphere->addNode(xyz);

             //
             // Update the border variance shape file
             //
             int nodeNum = sourceDeformationSphere->getNumberOfNodes() - 1;
             float variance = targetBorder->getVariance();
             if (endPointFlag) {
                variance *= endpointFactor;
             }
             this->borderVarianceValuesShapeFile.setValue(nodeNum, 0,
                                                          variance);
             if (DebugControl::getDebugOn()) {
                std::cout << targetBorder->getName().toAscii().constData()
                          << " variance: "
                          << targetBorder->getVariance()
                          << std::endl;
             }
             projectedFlag = true;

             //
             // Get out of loop
             //
             iTest = iTestMaximum + 1;
          }
      }

      if (projectedFlag == false) {
         throw BrainModelAlgorithmException(
                 "Unable to tessellate border "
                 + borderName
                 + " link "
                 + linkNumber
                 + " into source surface.");
      }
   }

   //
   // Make sure all nodes are on the sphere
   //
   sourceDeformationSphere->convertToSphereWithRadius(deformationSphereRadius);

   //
   // Update the surface's normals and update node attributes for changes in nodes.
   //
   sourceDeformationSphere->orientNormalsOut();
   sourceDeformationSphere->computeNormals();
   brainSet->resetNodeAttributes();

   //
   // Smooth a tiny bit as some landmark nodes may be essentially on an edge
   //
   sourceDeformationSphere->arealSmoothing(0.5, 1, 0);
   sourceDeformationSphere->convertToSphereWithRadius(deformationSphereRadius);

   sourceDeformationSphere->crossoverCheck(numTileCrossovers,
                                           numNodeCrossovers,
                   BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   if (numNodeCrossovers > 0) {
      std::cout << "WARNING: Source deformation sphere contains crossovers "
                   "AFTER insertion of landmarks."
                << std::endl;
   }

   QString sourceNamePrefix("source_withLandmarks_Stage_"
                            + QString::number(stageIndex + 1)
                            + ".LVD");

   //
   // Create the spec file name
   //
   QString specFileName = sourceNamePrefix + SpecFile::getSpecFileExtension();
   brainSet->setSpecFileName(specFileName);
   intermediateFiles.push_back(specFileName);
   brainSet->removeCoordAndTopoFromSpecFile();

   //
   // Setup the paint file that shows the landmark nodes
   //
   int numNodes = brainSet->getNumberOfNodes();
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
   QString paintFileName(sourceNamePrefix + SpecFile::getPaintFileExtension());
   brainSet->writePaintFile(paintFileName);
   intermediateFiles.push_back(paintFileName);

   //
   // Setup the node color file
   //
   AreaColorFile* cf = brainSet->getAreaColorFile();
   cf->addColor("Landmark", 255,   0,   0, 2, 1);
   cf->addColor("???",      170, 170, 170, 2, 1);
   QString nodeColorFileName(sourceNamePrefix + SpecFile::getAreaColorFileExtension());
   brainSet->writeAreaColorFile(nodeColorFileName);
   intermediateFiles.push_back(nodeColorFileName);

   //
   // Write the topology file
   //
   QString topoFileName(sourceNamePrefix + SpecFile::getTopoFileExtension());
   brainSet->writeTopologyFile(topoFileName, TopologyFile::TOPOLOGY_TYPE_CLOSED, tf);
   intermediateFiles.push_back(topoFileName);

   //
   // Write the sphere with target landmarks
   //
   QString coordFileName(sourceNamePrefix + SpecFile::getCoordinateFileExtension());
   brainSet->writeCoordinateFile(coordFileName, BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                 sourceDeformationSphere->getCoordinateFile());
   intermediateFiles.push_back(coordFileName);

   //
   // Update the displayed surface
   //
   sourceDeformationSphere->orientTilesConsistently();
   sourceDeformationSphere->computeNormals();
   this->updateViewingTransformation(brainSet);
   sourceDeformationSphere->updateForDefaultScaling();
   brainSet->drawBrainModel(sourceDeformationSphere);

   sourceDeformationSphere = new BrainModelSurface(*sourceDeformationSphere);
   brainSet->addBrainModel(sourceDeformationSphere);

   //
   // Save the border variance shape file
   //
   const QString shapeFileName("source_withLandmarksBorderVariance_stage"
                               + QString::number(stageIndex + 1)
                               + SpecFile::getSurfaceShapeFileExtension());
   borderVarianceValuesShapeFile.writeFile(shapeFileName);
   intermediateFiles.push_back(shapeFileName);
   brainSet->addToSpecFile(SpecFile::getSurfaceShapeFileTag(),
                           shapeFileName);
}

/**
 * move the landmark nodes to the average of their neighboring nodes.
 */
void
BrainModelSurfaceDeformationMultiStageSphericalVector::moveLandmarksToAverageOfNeighbors(BrainModelSurface* bms)
{
   const TopologyHelper* th = bms->getTopologyFile()->getTopologyHelper(false, true, false);
   CoordinateFile* cf = bms->getCoordinateFile();
   int numNodes = bms->getNumberOfNodes();
   for (int i = originalNumberOfNodes; i < numNodes; i++) {
      int numNeighbors = 0;
      const int* neighbors = th->getNodeNeighbors(i, numNeighbors);

      float sumXYZ[3] = { 0.0, 0.0, 0.0 };
      for (int j = 0; j < numNeighbors; j++) {
         const float* xyz = cf->getCoordinate(neighbors[j]);
         sumXYZ[0] += xyz[0];
         sumXYZ[1] += xyz[1];
         sumXYZ[2] += xyz[2];
      }

      const float avgXYZ[3] = {
         sumXYZ[0] / numNeighbors,
         sumXYZ[1] / numNeighbors,
         sumXYZ[2] / numNeighbors
      };

      cf->setCoordinate(i, avgXYZ);
   }

   bms->convertToSphereWithRadius(deformationSphereRadius);
}

/**
 * create the surface shape file containing the XYZ differences of the
 * nodes in the target and source surfaces.  Note that only the landmark
 * nodes have their differences set; the original nodes have a difference
 * of zero.
 */
SurfaceShapeFile*
BrainModelSurfaceDeformationMultiStageSphericalVector::createDifferenceShapeFile(int iterations)
{
   int numNodes = targetDeformationSphere->getNumberOfNodes();

   SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
   ssf->clear();
   ssf->setNumberOfNodesAndColumns(numNodes, 9);
   ssf->setColumnName(0, "dX");
   ssf->setColumnName(1, "dY");
   ssf->setColumnName(2, "dZ");
   ssf->setColumnName(3, "dX * Border Variance");
   ssf->setColumnName(4, "dY * Border Variance");
   ssf->setColumnName(5, "dZ * Border Variance");
   ssf->setColumnName(6, "dX Smoothed");
   ssf->setColumnName(7, "dY Smoothed");
   ssf->setColumnName(8, "dZ Smoothed");

   const CoordinateFile* sourceCoords = sourceDeformationSphere->getCoordinateFile();
   const CoordinateFile* targetCoords = targetDeformationSphere->getCoordinateFile();

   for (int i = originalNumberOfNodes; i < numNodes; i++) {
      const float* sxyz = sourceCoords->getCoordinate(i);
      const float* txyz = targetCoords->getCoordinate(i);

      float dxyz[3] = {
         txyz[0] - sxyz[0],
         txyz[1] - sxyz[1],
         txyz[2] - sxyz[2]
      };

      //
      // Displacement
      //
      ssf->setValue(i, 0, dxyz[0]);
      ssf->setValue(i, 1, dxyz[1]);
      ssf->setValue(i, 2, dxyz[2]);

      //
      // Modify displacement using border variance
      //
      const float borderVariance = this->borderVarianceValuesShapeFile.getValue(i, 0);
      if (borderVariance != 0.0) {
         dxyz[0] *= borderVariance;
         dxyz[1] *= borderVariance;
         dxyz[2] *= borderVariance;
      }

      //
      // Displacement multiplied by variance
      //
      ssf->setValue(i, 3, dxyz[0]);
      ssf->setValue(i, 4, dxyz[1]);
      ssf->setValue(i, 5, dxyz[2]);

      //
      // If no smoothing iterations, set smoothed values
      //
      if (iterations <= 0) {
         ssf->setValue(i, 6, dxyz[0]);
         ssf->setValue(i, 7, dxyz[1]);
         ssf->setValue(i, 8, dxyz[2]);
      }
   }

   //
   // Smooth the columns
   //
   if (iterations > 0) {
       float strength = 0.5;
       for (int j = 3; j < 6; j++) {
           BrainModelSurfaceMetricSmoothing bmsms(brainSet,
                                                  sourceDeformationSphere,
                                                  sourceDeformationSphere,
                                                  ssf,
                         BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS,
                                                  j,
                                                  j + 3,
                                                  (ssf->getColumnName(j) + " Smoothed"),
                                                  strength,
                                                  iterations,
                                                  0.0,
                                                  0.0,
                                                  0.0,
                                                  0.0,
                                                  0.0,
                                                  0.0);
           bmsms.execute();
       }
   }

   return ssf;
}

/**
 * Perform landmark neighbor constrained smoothing on the sphere with source landmarks.
 */
void
BrainModelSurfaceDeformationMultiStageSphericalVector::landmarkNeighborConstrainedSmoothSource(
                            const int stageIndex, const int cycleIndex)
{
   //
   // Get smoothing parameters for this cycle
   //
   float strength;
   int numCycles, numIterations, neighborIterations, numFinalIterations;
   deformationMapFile->getSmoothingParameters(stageIndex,
                                              cycleIndex,
                                              strength,
                                              numCycles,
                                              numIterations,
                                              neighborIterations,
                                              numFinalIterations);
   if (numCycles <= 0) {
      return;
   }

   //
   // perform number of smoothing cycles
   //
   for (int i = 0; i < numCycles; i++) {
      //
      // Perform the landmark constrained smoothing
      //
      updateViewingTransformation(sourceDeformationSphere);
      sourceDeformationSphere->landmarkNeighborConstrainedSmoothing(strength,
                                                                    numIterations,
                                                                    landmarkNodeFlags,
                                                                    neighborIterations,
                                                                    0);

      //
      // push non-landmark nodes back onto the sphere
      //
      sourceDeformationSphere->convertToSphereWithRadius(deformationSphereRadius,
                                                                 0, originalNumberOfNodes);
   }

   //
   // Perform final smoothing
   //
   sourceDeformationSphere->arealSmoothing(strength,
                                           numFinalIterations,
                                           0);

   //
   // push all nodes back onto the sphere
   //
   sourceDeformationSphere->convertToSphereWithRadius(deformationSphereRadius);
   sourceDeformationSphere->updateForDefaultScaling();
   updateViewingTransformation(sourceDeformationSphere);
}

/**
 * Perform landmark constrained morphing on the sphere with source landmarks
 */
void
BrainModelSurfaceDeformationMultiStageSphericalVector::landmarkMorphContrainedSource(
                                   BrainModelSurface* referenceDeformationSphere,
                                   const int stageIndex,
                                   const int cycleIndex)
                                            throw (BrainModelAlgorithmException)
{
   if (DebugControl::getDebugOn()) {
       std::cout << "Landmark Constrained Morphing Cycle "
                 << (cycleIndex + 1)
                 << std::endl;
       std::cout << "   REF:  " << referenceDeformationSphere->getCoordinateFile()->getFileNameNoPath().toAscii().constData() << std::endl;
   }

   //
   // Get smoothing parameters for this cycle (just need strength)
   //
   float strength;
   int numCycles, numIterations, neighborIterations, numFinalIterations;
   deformationMapFile->getSmoothingParameters(stageIndex,
                                              cycleIndex,
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
   deformationMapFile->getMorphingParameters(stageIndex,
                                             cycleIndex,
                                             numMorphCycles,
                                             linearForce,
                                             angularForce,
                                             stepSize,
                                             landmarkStepSize,
                                             iterations,
                                             smoothIterations);

   //
   // If morphing should be performed
   //
   if (numMorphCycles > 0) {
      //
      // NON-landmark nodes are morphed
      //
      const int numNodes = sourceDeformationSphere->getNumberOfNodes();
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
         if (cycleIndex > 0) {
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
                                        referenceDeformationSphere,
                                        sourceDeformationSphere,
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
         sourceDeformationSphere->arealSmoothing(strength, smoothIterations, 0);

         //
         // Move landmarks to average of neighbors 5/20/09
         //
         this->moveLandmarksToAverageOfNeighbors(sourceDeformationSphere);

         //
         // Push nodes back to sphere and draw
         //
         sourceDeformationSphere->convertToSphereWithRadius(deformationSphereRadius);

         //
         // Draw the surface
         //
         sourceDeformationSphere->orientTilesConsistently();
         sourceDeformationSphere->computeNormals();
         sourceDeformationSphere->updateForDefaultScaling();
         updateViewingTransformation(brainSet);
         brainSet->drawBrainModel(sourceDeformationSphere);

      }

      //
      // Save the surface
      //
      //QString coordFileName(debugSourceFileNamePrefix);
      //coordFileName.append("_withLandmarks_morphed");
      //coordFileName.append(SpecFile::getCoordinateFileExtension());
      //brainSet->writeCoordinateFile(coordFileName, BrainModelSurface::SURFACE_TYPE_SPHERICAL,
      //                              sourceDeformationSphere->getCoordinateFile());
      //intermediateFiles.push_back(coordFileName);

      //
      // Draw the surface
      //
      sourceDeformationSphere->orientTilesConsistently();
      sourceDeformationSphere->computeNormals();
      sourceDeformationSphere->updateForDefaultScaling();
      this->updateViewingTransformation(brainSet);
      brainSet->drawBrainModel(sourceDeformationSphere);
   }
}

/**
 * Deform the source sphere using the source and target deformation spheres
 * Returns the deformed surface
 */
BrainModelSurface*
BrainModelSurfaceDeformationMultiStageSphericalVector::createDeformedCoordinateFile(
                        BrainModelSurface* originalSourceDeformationSphere,
                        BrainModelSurface* registeredDeformationSourceSphere,
                        const int stageIndex,
                        const int cycleNumber,
                        const QString& usersSourceCoordFileName)
{
   try {
      const int stageNumber = stageIndex + 1;
      bool lastCycleFlag =
         (cycleNumber >= this->deformationMapFile->getSphericalNumberOfCycles(stageIndex));
      //bool lastStageFlag =
      //   (stageNumber >= this->deformationMapFile->getSphericalNumberOfStages());
      
      BrainModelSurface* usersSourceSurface = workingSourceSurface;
      // 17jun2009  if (lastCycleFlag) {
         usersSourceSurface = sourceSurface;
      //}
      
      if (lastCycleFlag) {
          //
          // Check for crossovers in source deformation sphere
          //
          int sourceTileCrossoversCount = 0;
          int sourceNodeCrossoversCount = 0;
          originalSourceDeformationSphere->crossoverCheck(sourceTileCrossoversCount,
                                                          sourceNodeCrossoversCount,
                                         BrainModelSurface::SURFACE_TYPE_SPHERICAL);
          if (sourceNodeCrossoversCount > 0) {
             std::cout << "WARNING: Original source deformation sphere has "
                       << sourceNodeCrossoversCount
                       << " crossovers when creating deformed coordinate file."
                       << std::endl;
          }

          //
          // Check for crossovers in source in target deformation
          //
          int targetTileCrossoversCount = 0;
          int targetNodeCrossoversCount = 0;
          registeredDeformationSourceSphere->crossoverCheck(targetTileCrossoversCount,
                                                          targetNodeCrossoversCount,
                                         BrainModelSurface::SURFACE_TYPE_SPHERICAL);
          if (targetNodeCrossoversCount > 0) {
             std::cout << "WARNING: Final deformation sphere has "
                       << targetNodeCrossoversCount
                       << " crossovers when creating deformed coordinate file."
                       << std::endl;
          }

          //
          // Check for crossovers in user's source surface
          //
          int userTileCrossoversCount = 0;
          int userNodeCrossoversCount = 0;
          usersSourceSurface->crossoverCheck(userTileCrossoversCount,
                                        userNodeCrossoversCount,
                                        BrainModelSurface::SURFACE_TYPE_SPHERICAL);
          if (userNodeCrossoversCount > 0) {
             std::cout << "WARNING: User's individual sphere has "
                       << userNodeCrossoversCount
                       << " crossovers when creating deformed coordinate file."
                       << std::endl;
          }
      }

      //
      // Copy the user's input surface to a new deformed surface
      //
      deformedSourceSurface = new BrainModelSurface(*usersSourceSurface);
      sourceBrainSet->addBrainModel(deformedSourceSurface);
      updateViewingTransformation(sourceBrainSet);

      //
      // Make sure source surface is same radius as deformation sphere
      //
      usersSourceSurface->convertToSphereWithRadius(deformationSphereRadius);

      //
      // Get the coordinate files for the user's surface and its
      // new deformed surface.
      //
      CoordinateFile* sourceCoords = usersSourceSurface->getCoordinateFile();
      CoordinateFile* deformedSourceCoords = deformedSourceSurface->getCoordinateFile();
      const int numCoords = sourceCoords->getNumberOfCoordinates();

      //
      // See if X coordinate will need to be flipped (first cycle only !!!)
      //
      bool diffHemFlag = false;
      if (stageNumber == 1) {
         if (cycleNumber == 1) {
            diffHemFlag = (sourceBrainSet->getStructure() != targetBrainSet->getStructure());
            if (diffHemFlag) {
               std::cout << "Different Hemispheres for Create Deformed Source Coordinates" << std::endl;
            }
         }
      }

      //
      // Create a Point Projector for the source deformation sphere
      //
      BrainModelSurfacePointProjector bspp(originalSourceDeformationSphere, //targetDeformationSphere);
                              BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                              false);

      //
      // Coord file of registered deformation source sphere
      //
      const CoordinateFile* deformSphereRegisteredCoords =
         registeredDeformationSourceSphere->getCoordinateFile();

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
         // Unproject using the deformation sphere coordinate file
         //
         if (tile >= 0) {
            BrainModelSurfacePointProjector::unprojectPoint(tileNodes,
                                                            tileAreas,
                                                            deformSphereRegisteredCoords,
                                                            xyz);
         }
         else if (nearestNode >= 0) {
            // JWH 08/08/03  2:15pm deformedSourceCoords->getCoordinate(nearestNode, xyz);
            //deformationMorphedSphereCoords->getCoordinate(nearestNode, xyz);
            deformSphereRegisteredCoords->getCoordinate(nearestNode, xyz);
         }
         deformedSourceCoords->setCoordinate(i, xyz);
      }

      deformedSourceSurface->orientTilesConsistently();
      deformedSourceSurface->computeNormals();

      if (lastCycleFlag) {
          //
          // Check for crossovers in user's source surface
          //
          int userDeformedTileCrossoversCount = 0;
          int userDeformedNodeCrossoversCount = 0;
          deformedSourceSurface->crossoverCheck(userDeformedTileCrossoversCount,
                                        userDeformedNodeCrossoversCount,
                                        BrainModelSurface::SURFACE_TYPE_SPHERICAL);
          if (userDeformedNodeCrossoversCount > 0) {
             std::cout << "WARNING: User's DEFORMED individual sphere has "
                       << userDeformedNodeCrossoversCount
                       << " after its creation."
                       << std::endl;
          }
      }

      //
      // Write the source deformed coordinates
      //
/*
      QDir::setCurrent(sourceDirectory);
      QString defCoordName(deformationMapFile->getDeformedFileNamePrefix());
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
*/
      //
      // Set name of deformed coordinate file and write it.
      //
      QDir::setCurrent(sourceDirectory);
      QString defCoordName;
      deformedSourceCoords->appendToFileComment("\nDeformation Map File: ");
      deformedSourceCoords->appendToFileComment(
         FileUtilities::basename(sourceToTargetDeformationMapFileName));
      if ((stageNumber >= deformationMapFile->getSphericalNumberOfStages()) &&
          (cycleNumber >= deformationMapFile->getSphericalNumberOfCycles(stageIndex))) {
         defCoordName = deformationMapFile->getDeformedFileNamePrefix()
                      + FileUtilities::basename(usersSourceCoordFileName);
         deformationMapFile->setSourceDeformedSphericalCoordFileName(defCoordName);
      }
      else {
         defCoordName = deformationMapFile->getDeformedFileNamePrefix()
                             + "sphere_stage_"
                             + QString::number(stageNumber)
                             + "_cycle_"
                             + QString::number(cycleNumber)
                             + SpecFile::getCoordinateFileExtension();
      }
      QString descriptionMessage("Projecting "
                                 + sourceCoords->getFileName()
                                 + " to "
                                 + originalSourceDeformationSphere->getCoordinateFile()->getFileName()
                                 + " and unprojecting to "
                                 + registeredDeformationSourceSphere->getCoordinateFile()->getFileName()
                                 + " creating the file "
                                 + defCoordName);
      deformedSourceCoords->writeFile(defCoordName);

      QDir::setCurrent(originalDirectory);

      std::cout << "CREATE DEFORMED: "
                << descriptionMessage.toAscii().constData()
                << std::endl;

      return deformedSourceSurface;
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
}

/**
 * write a border file containing the deformed landmarks.
 */
BorderFile*
BrainModelSurfaceDeformationMultiStageSphericalVector::writeSourceBorderLandmarkFile(BrainModelSurface* surface,
                                                                           const int stageIndex,
                                                                           const int cycleNumber)
{
   const CoordinateFile* cf = surface->getCoordinateFile();
   const int numNodes = cf->getNumberOfCoordinates();
   BorderFile* bf = new BorderFile;
   Border border;

   int lastBorderNumber = -1;
   for (int i = this->originalNumberOfNodes; i < numNodes; i++) {
      int usedIndex = i - this->originalNumberOfNodes;
      std::pair<int,int> borderInfo = usedBorderLinks[usedIndex];
      int borderNumber = borderInfo.first;
      //int linkNumber   = borderInfo.second;

      if (borderNumber != lastBorderNumber) {
         if (border.getNumberOfLinks() > 0) {
            bf->addBorder(border);
            border.clearLinks();
         }
         border.setName(targetBorderFile->getBorder(borderNumber)->getName());
         lastBorderNumber = borderNumber;
      }

      border.addBorderLink(cf->getCoordinate(i));
   }

   if (border.getNumberOfLinks() > 0) {
      bf->addBorder(border);
   }

   bf->setHeaderTag(AbstractFile::headerTagConfigurationID,
      BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_SPHERICAL));
   QString borderFileName("source_after_resample_stage_"
                + QString::number(stageIndex + 1)
                + "_cycle_"
                + QString::number(cycleNumber)
                + SpecFile::getBorderFileExtension());
   bf->writeFile(borderFileName);
   intermediateFiles.push_back(borderFileName);

   brainSet->addToSpecFile(SpecFile::getSphericalBorderFileTag(),
                           borderFileName);

   return bf;
}

/**
 * write coords without the landmarks.
 */
void
BrainModelSurfaceDeformationMultiStageSphericalVector::writeCoordinatesWithoutLandmarks(BrainModelSurface* surface,
                                                                              const int stageIndex,
                                                                              const int cycleNumber)
{
   CoordinateFile cf;
   cf.setNumberOfCoordinates(this->originalNumberOfNodes);

   for (int i = 0; i < this->originalNumberOfNodes; i++) {
      cf.setCoordinate(i, surface->getCoordinateFile()->getCoordinate(i));
   }

   QString coordFileName("source_withoutLandmarks.LVD.stage_"
                 + QString::number(stageIndex + 1)
                 + "_cycle_"
                 + QString::number(cycleNumber)
                 + SpecFile::getCoordinateFileExtension());
   cf.writeFile(coordFileName);
   intermediateFiles.push_back(coordFileName);
}

/**
 * Execute the deformation.
 */
void
BrainModelSurfaceDeformationMultiStageSphericalVector::executeDeformation()
                                         throw (BrainModelAlgorithmException)
{
   //
   // Validate vector smoothing iterations
   //
   QString vectorIterationsErrorMessage = "";
   for (int si = 0;
        si < deformationMapFile->getSphericalNumberOfStages();
        si++) {
       for (int ci = 0;
            ci < deformationMapFile->getSphericalNumberOfCycles(si);
            ci++) {
           //
           // Get vector-landmark parameters
           //
           int iterations = 0;
           float displacementFactor = 1.0;
           this->deformationMapFile->getLandmarkVectorParameters(si,
                                                                 ci,
                                                                 iterations,
                                                                 displacementFactor);
           if (iterations <= 0) {
              if (vectorIterationsErrorMessage.isEmpty()) {
                 vectorIterationsErrorMessage =
                   "Each cycle must have at least one iteration of vector smoothing.\n"
                   "These stages/cycles have zero iterations:";
              }
              vectorIterationsErrorMessage += ("  ("
                                               + QString::number(si + 1)
                                               + ","
                                               + QString::number(ci + 1)
                                               + ")");
           }
       }
   }
   if (vectorIterationsErrorMessage.isEmpty() == false) {
      throw BrainModelAlgorithmException(vectorIterationsErrorMessage);
   }

   //
   // Get radius of target sphere
   //
   deformationSphereRadius = targetSurface->getSphericalSurfaceRadius();

   const QString sourceSurfaceCoordFileName =
      sourceSurface->getCoordinateFile()->getFileName();

   //
   // Make a copy of the source surface so that it is not modified
   //
   workingSourceSurface = new BrainModelSurface(*sourceSurface);

   /// keeps track of crossovers in each cycle
   std::vector<int> crossoverCount;
   std::vector<int> crossoverStage;
   std::vector<int> crossoverCycle;

   //
   // Loop through stages
   //
   for (int stageIndex = 0;
        stageIndex < deformationMapFile->getSphericalNumberOfStages();
        stageIndex++) {
      const bool lastStageFlag =
         ((stageIndex + 1) >= deformationMapFile->getSphericalNumberOfStages());
      QString progressPrefixString = "Stage "
                                   + QString::number(stageIndex + 1)
                                   + " ";

      //
      // If not the first stage
      //
      if (stageIndex > 0) {
         //
         // Read in the target brain set with the border file appropriate
         // for this stage.
         //
         delete targetBrainSet;
         readTargetBrainSet(stageIndex);
      }
       //
       // Make source surface the same radius as the target surface
       //
       workingSourceSurface->convertToSphereWithRadius(deformationSphereRadius);

       //
       // control of sphere fiducial distortion
       //
       bool useFiducialSphereRatios = false;
       float fiducialSphereRatioStrength = 0.5;
       deformationMapFile->getSphereFiducialRatio(useFiducialSphereRatios,
                                                  fiducialSphereRatioStrength);

       //
       // Determing the ratio of the source fiducial and spherical tile areas
       //
       if (useFiducialSphereRatios) {
          if (DebugControl::getDebugOn()) {
             std::cout << " " << std::endl;
          }
          updateProgressDialog(progressPrefixString
                               + "Determining Spherical Distortion");

          determineSphericalDistortion(sourceFiducialSurface,
                                     workingSourceSurface,
                                     sourceTileDistortion);

          //
          // Determing the ratio of the target fiducial and spherical tile areas
          //
          determineSphericalDistortion(targetFiducialSurface,
                                     targetSurface,
                                     targetTileDistortion);
       }

      //
      // Resample the border files
      //
      if (DebugControl::getDebugOn()) {
         std::cout << "Resampling borders." << std::endl;
      }
      updateProgressDialog(progressPrefixString + "Resampling borders.");
      resampleBorderFiles(stageIndex, -1, deformationSphereRadius);

      //
      // Create the target brain set
      //
      targetDeformationBrainSet = new BrainSet();

      //
      // Load the regularly tessellated sphere and make it same radius as target surface
      //
      QString currentDirectory(QDir::currentPath());
      targetDeformationSphere = getRegularSphere(targetDeformationBrainSet,
                                                 stageIndex,
                                                 deformationSphereRadius);
      targetDeformationSphere->orientTilesConsistently();
      targetDeformationSphere->computeNormals();
      targetDeformationSphere->updateForDefaultScaling();
      this->updateViewingTransformation(targetDeformationSphere);
      targetDeformationBrainSet->drawBrainModel(targetDeformationSphere);
      QDir::setCurrent(currentDirectory);

      //
      // Save the number of nodes before tessellating in the borders
      //
      originalNumberOfNodes = targetDeformationSphere->getNumberOfNodes();

      //
      // tessellate the target borders into the target deformation sphere
      //
      if (DebugControl::getDebugOn()) {
         std::cout << "Tessellating borders into target." << std::endl;
      }
      updateProgressDialog(progressPrefixString + "Tessellating Borders into Target");
      tessellateTargetBordersIntoDeformationSphere(stageIndex);

      //
      // Load the regularly tessellated sphere and make it same radius as target surface
      //
      currentDirectory = QDir::currentPath();
      sourceDeformationSphere = getRegularSphere(brainSet, stageIndex, deformationSphereRadius);
      sourceDeformationSphere->orientTilesConsistently();
      sourceDeformationSphere->computeNormals();
      sourceDeformationSphere->updateForDefaultScaling();
      this->updateViewingTransformation(sourceDeformationSphere);
      brainSet->drawBrainModel(sourceDeformationSphere);
      QDir::setCurrent(currentDirectory);

      //
      // tessellate the source borders into the source deformation sphere
      //
      if (DebugControl::getDebugOn()) {
         std::cout << "Tessellating borders into source." << std::endl;
      }
      updateProgressDialog(progressPrefixString + "Tessellating Borders into Source");
      tessellateSourceBordersIntoDeformationSphere(stageIndex);

      //
      // Place copy of target into the brain set
      //
      QString targetCoordName =
              targetDeformationSphere->getCoordinateFile()->getFileName();
      brainSet->readCoordinateFile(targetCoordName,
                                   BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                   false,
                                   true,
                                   true);
      brainSet->getBrainModelSurface(
              brainSet->getNumberOfBrainModels() - 1)->updateForDefaultScaling();

      //
      // Mark the landmark nodes
      //
      //const int numNodes = targetDeformationSphere->getNumberOfNodes();
      int numNodes = sourceDeformationSphere->getNumberOfNodes();
      landmarkNodeFlags.resize(numNodes);
      for (int i = 0; i < numNodes; i++) {
         landmarkNodeFlags[i] = (i >= originalNumberOfNodes);
      }

      //
      // Create the source reference surface
      //
      referenceSourceDeformationSphere =
                        new BrainModelSurface(*sourceDeformationSphere);
      moveLandmarksToAverageOfNeighbors(referenceSourceDeformationSphere);
      referenceSourceDeformationSphere->computeNormals();
      referenceSourceDeformationSphere->updateForDefaultScaling();
      brainSet->addBrainModel(referenceSourceDeformationSphere);
      this->updateViewingTransformation(referenceSourceDeformationSphere);
      const QString sourceRefName("source_withLandmarks_stage_"
                                  + QString::number(stageIndex + 1)
                                  + ".LVD.REF"
                                  + SpecFile::getCoordinateFileExtension());
      brainSet->writeCoordinateFile(sourceRefName,
                                    BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                              referenceSourceDeformationSphere->getCoordinateFile(),
                                    true);
      intermediateFiles.push_back(sourceRefName);

      //
      // Copy of source deformation sphere
      //
      BrainModelSurface originalSourceDeformationSphere(*referenceSourceDeformationSphere);
      originalSourceDeformationSphere.getCoordinateFile()->setFileName(
           "CopyOf_" + referenceSourceDeformationSphere->getCoordinateFile()->getFileName());

       //
       // Perform the requested number of cycles
       //
       for (int cycleIndex = 0;
            cycleIndex <
                   deformationMapFile->getSphericalNumberOfCycles(stageIndex);
            cycleIndex++) {
          const int cycleNumber = cycleIndex + 1;
          const QString cycleString(progressPrefixString
                                    + "Cycle " + QString::number(cycleNumber) + " ");
          bool lastCycleFlag =
             (cycleNumber >=
                    deformationMapFile->getSphericalNumberOfCycles(stageIndex));

           //
           // Get vector-landmark parameters
           //
           int iterations = 0;
           float displacementFactor = 1.0;
           this->deformationMapFile->getLandmarkVectorParameters(stageIndex,
                                                                 cycleIndex,
                                                                 iterations,
                                                                 displacementFactor);
          //
          // Create the surface shape coordinate differences
          //
          if (DebugControl::getDebugOn()) {
             std::cout << "Cycle " << cycleNumber
                       << " smoothing landmarks in source." << std::endl;
          }
          updateProgressDialog(cycleString + "Smoothing Landmarks in Source");
          SurfaceShapeFile* shapeDiffFile =
                  this->createDifferenceShapeFile(iterations);

          //
          // Save the shape file
          //
          const QString shapeFileName("source_withLandmarks.LVD.stage_"
                                      + QString::number(stageIndex + 1)
                                      + "_cycle_"
                                      + QString::number(cycleNumber)
                                      + SpecFile::getSurfaceShapeFileExtension());
          brainSet->writeSurfaceShapeFile(shapeFileName);
          intermediateFiles.push_back(shapeFileName);

          //
          // Displace sphere nodes by smoothed shape data
          //
          CoordinateFile* sourceCoords = sourceDeformationSphere->getCoordinateFile();
          for (int j = 0; j < numNodes; j++) {
             float* xyz = sourceCoords->getCoordinate(j);
             xyz[0] += (shapeDiffFile->getValue(j, 6) * displacementFactor);
             xyz[1] += (shapeDiffFile->getValue(j, 7) * displacementFactor);
             xyz[2] += (shapeDiffFile->getValue(j, 8) * displacementFactor);
             sourceCoords->setCoordinate(j, xyz);
          }
          const QString displacedCoordName("source_withLandmarksDisplaced.LVD.stage_"
                                          + QString::number(stageIndex + 1)
                                          + "_cycle_"
                                          + QString::number(cycleNumber)
                                          + SpecFile::getCoordinateFileExtension());
          brainSet->writeCoordinateFile(displacedCoordName,
                                        BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                        sourceDeformationSphere->getCoordinateFile(),
                                        true);
          intermediateFiles.push_back(displacedCoordName);
          moveLandmarksToAverageOfNeighbors(sourceDeformationSphere);
          sourceDeformationSphere->convertToSphereWithRadius(
                                                    this->deformationSphereRadius);
          const QString displayedSphereCoordName("source_withLandmarksDisplacedBackToSphere.LVD.stage_"
                                          + QString::number(stageIndex + 1)
                                          + "_cycle_"
                                          + QString::number(cycleNumber)
                                          + SpecFile::getCoordinateFileExtension());
          brainSet->writeCoordinateFile(displayedSphereCoordName,
                                        BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                        sourceDeformationSphere->getCoordinateFile(),
                                        true);
          intermediateFiles.push_back(displayedSphereCoordName);
          updateViewingTransformation(brainSet);
          sourceDeformationSphere->orientTilesConsistently();
          sourceDeformationSphere->computeNormals();
          sourceDeformationSphere->updateForDefaultScaling();
          this->updateViewingTransformation(sourceDeformationSphere);
          brainSet->drawBrainModel(sourceDeformationSphere);
          sourceDeformationSphere = new BrainModelSurface(*sourceDeformationSphere);
          brainSet->addBrainModel(sourceDeformationSphere);
          this->updateViewingTransformation(sourceDeformationSphere);

          //
          // Perform landmark neighbor constrained smoothing on the sphere
          // with source landmarks
          //
          if (DebugControl::getDebugOn()) {
             std::cout << "Cycle " << cycleNumber
                       << " landmark neighbor constrained smoothing." << std::endl;
          }
          updateProgressDialog(cycleString
                               + "Landmark Neighbor Constrained Smoothing of Source");
          landmarkNeighborConstrainedSmoothSource(stageIndex, cycleIndex);
          const QString smoothedLandmarkCoordName(
                                          "source_withLandmarksSmoothed.LVD.stage_"
                                          + QString::number(stageIndex + 1)
                                          + "_cycle_"
                                          + QString::number(cycleNumber)
                                          + SpecFile::getCoordinateFileExtension());
          brainSet->writeCoordinateFile(smoothedLandmarkCoordName,
                                        BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                        sourceDeformationSphere->getCoordinateFile(),
                                        true);
          intermediateFiles.push_back(smoothedLandmarkCoordName);
          updateViewingTransformation(brainSet);
          sourceDeformationSphere->orientTilesConsistently();
          sourceDeformationSphere->computeNormals();
          sourceDeformationSphere->updateForDefaultScaling();
          this->updateViewingTransformation(sourceDeformationSphere);
          brainSet->drawBrainModel(sourceDeformationSphere);
          sourceDeformationSphere = new BrainModelSurface(*sourceDeformationSphere);
          this->updateViewingTransformation(sourceDeformationSphere);
          brainSet->addBrainModel(sourceDeformationSphere);

          //
          // During first cycle, allocate the fiducial sphere distortion
          // surface shape file
          //
          if (useFiducialSphereRatios) {
             fiducialSphereDistortion.setNumberOfNodesAndColumns(
                                    targetDeformationSphere->getNumberOfNodes(), 3);
             fiducialSphereDistortion.setColumnName(0, "Target Distortion");
             fiducialSphereDistortion.setColumnName(1, "Source Distortion");
             fiducialSphereDistortion.setColumnName(2, "sqrt(Target/Source)");

             //
             // determine the fiducial sphere distortion
             //
             determineFiducialSphereDistortion();
              //
              // Update fiducial/sphere with deformed surface
              //
              updateSphereFiducialDistortion(stageIndex, cycleIndex, sourceDeformationSphere);
          }

          //
          // Perform landmark neighbor constrained morphing on the sphere
          // with source landmarks
          //
          if (DebugControl::getDebugOn()) {
             std::cout << "Cycle " << cycleNumber
                       << " landmark neighbor constrained morphing." << std::endl;
          }
          updateProgressDialog(cycleString +
                               "Landmark Neighbor Constrained Morphing of Source");
          landmarkMorphContrainedSource(referenceSourceDeformationSphere, stageIndex, cycleIndex);
          const QString morphedLandmarkCoordName(
                                          "source_withLandmarksMorphed.LVD.stage_"
                                          + QString::number(stageIndex + 1)
                                          + "_cycle_"
                                          + QString::number(cycleNumber)
                                          + SpecFile::getCoordinateFileExtension());
          intermediateFiles.push_back(morphedLandmarkCoordName);
          updateViewingTransformation(brainSet);
          sourceDeformationSphere->orientTilesConsistently();
          sourceDeformationSphere->computeNormals();
          sourceDeformationSphere->updateForDefaultScaling();
          brainSet->drawBrainModel(sourceDeformationSphere);
          brainSet->writeCoordinateFile(morphedLandmarkCoordName,
                                        BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                        sourceDeformationSphere->getCoordinateFile(),
                                        true);

          //
          // Use morphed as REFERENCE for next cycle
          //
          referenceSourceDeformationSphere = sourceDeformationSphere;

          //
          // Check for crossovers
          //
          updateProgressDialog(cycleString + "Checking for Crossovers");
          int numNodeCrossovers = 0, numTileCrossovers = 0;
          sourceDeformationSphere->crossoverCheck(numTileCrossovers,
                                         numNodeCrossovers,
                                         BrainModelSurface::SURFACE_TYPE_SPHERICAL);
          crossoverCount.push_back(numNodeCrossovers);
          crossoverStage.push_back(stageIndex + 1);
          crossoverCycle.push_back(cycleNumber);

          if (deformationMapFile->getPauseForCrossoversConfirmation()) {
             if (brainSet->getProgressDialogParent() != NULL) {
                if (numNodeCrossovers > 0) {
                   QString msg = "At stage "
                                 + QString::number(stageIndex + 1)
                                 + " cycle "
                                 + QString::number(cycleNumber)
                                 + " there are "
                                 + QString::number(numNodeCrossovers)
                                 + " crossovers."
                                 + "\nDo you want to continue?";

                   if (QMessageBox::question(brainSet->getProgressDialogParent(),
                                             "Crossovers Warning",
                                             msg,
                                             (QMessageBox::Yes | QMessageBox::No),
                                             QMessageBox::Yes) == QMessageBox::No) {
                      throw BrainModelAlgorithmException("Deformation cancelled by user.");
                   }
                }
             }
          }

          //
          // Project the user's sphere from the original source deformation sphere to
          // the original deformation sphere
          //
          updateProgressDialog(cycleString + "Creating Deformed Coordinate File");
          BrainModelSurface* newDeformedSurface =
             createDeformedCoordinateFile(&originalSourceDeformationSphere,
                                       sourceDeformationSphere,
                                       stageIndex,
                                       cycleNumber,
                                       sourceSurfaceCoordFileName);

          //
          // Create a new sphere for the next cycle
          //
          if (lastCycleFlag == false) {
             sourceDeformationSphere =
                     new BrainModelSurface(*sourceDeformationSphere);
             this->updateViewingTransformation(sourceDeformationSphere);
             brainSet->addBrainModel(sourceDeformationSphere);
          }

          //
          // Set surface for next stage
          //
          workingSourceSurface = deformedSourceSurface;
          
          //
          // Create border file containing the deformed landmarks and use
          // it as the border file for the next stage
          //
          BorderFile* defBorderFile =
            writeSourceBorderLandmarkFile(sourceDeformationSphere, stageIndex, cycleNumber);

          //
          // Write deformation sphere without landmarks
          //
          writeCoordinatesWithoutLandmarks(sourceDeformationSphere, stageIndex, cycleNumber);

          if (lastCycleFlag) {
             //
             // Set borders for next stage
             //
             sourceBorderFile = defBorderFile;

             if (lastStageFlag == false) {
                //
                // Use deformed source surface as input source surface
                // for next stage
                //
                sourceSurface = newDeformedSurface;
             }
          }
       }  // cycle index
   } // stage index

   //
   // If the last cycle alert user if there are crossovers
   //
   bool anyCrossoversFlag = false;
   for (unsigned int i = 0; i < crossoverCount.size(); i++) {
     if (crossoverCount[i] > 0) {
        anyCrossoversFlag = true;
        break;
     }
   }
   if (anyCrossoversFlag > 0) {
     QWidget* parent = brainSet->getProgressDialogParent();
     if (parent != NULL) {
        QString msg;
        for (int i = 0; i < static_cast<int>(crossoverCount.size()); i++) {
           QString s =
                   "Stage " + QString::number(crossoverStage[i])
                   + " Cycle " + QString::number(crossoverCycle[i])
                   + " had " + QString::number(crossoverCount[i])
                   + " crossovers.\n";
           msg += s;
        }
/*
        for (int i = 0;
             i < this->deformationMapFile->getSphericalNumberOfStages();
             i++) {
           for (int j = 0;
                j < this->deformationMapFile->getSphericalNumberOfCycles(i);
                j++) {
              const int index =
                 (i * this->deformationMapFile->getSphericalNumberOfCycles(i))
                 + j;
               std::ostringstream str;
               str << "Stage " << (i + 1) << " Cycle " << (j + 1) << " had "
                   << crossoverCount[index]
                   << " crossovers.\n";
               msg.append(str.str().c_str());
           }
        }
*/
        msg.append("\nContinue Deformation ?");
        QApplication::restoreOverrideCursor();
        QApplication::beep();
        if (QMessageBox::warning(parent, "Crossover Alert", msg,
                                 "Yes", "No") != 0) {
           throw BrainModelAlgorithmException("Deformation terminated by user.");
        }
        //will not compile on mac - don't know why QApplication::setOverrideCursor(waitCursor);
     }
   }
}


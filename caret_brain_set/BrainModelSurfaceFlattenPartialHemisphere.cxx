
#include <iostream>
#include <sstream>

#include <QGlobalStatic>  // needed on windows for TRUE and FALSE
#include <QMessageBox>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceFlattenPartialHemisphere.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "PaintFile.h"

/**
 * Constructor
 */
BrainModelSurfaceFlattenPartialHemisphere::BrainModelSurfaceFlattenPartialHemisphere(
                                          BrainModelSurface* fiducialSurfaceIn,
                                          BrainModelSurface* partialHemisphereSurfaceIn,
                                          const FLATTEN_TYPE flattenSurfaceTypeIn)
   : BrainModelAlgorithm(partialHemisphereSurfaceIn->getBrainSet())
{
   fiducialSurface          = fiducialSurfaceIn;
   partialHemisphereSurface = partialHemisphereSurfaceIn;
   flattenSurfaceType       = flattenSurfaceTypeIn;
}

/**
 * Destructor
 */
BrainModelSurfaceFlattenPartialHemisphere::~BrainModelSurfaceFlattenPartialHemisphere()
{
}

/**
 * Execute the flattening
 */
void 
BrainModelSurfaceFlattenPartialHemisphere::execute() throw (BrainModelAlgorithmException)
{
   const TopologyFile* topologyFile = partialHemisphereSurface->getTopologyFile();
   
   std::vector<int> dummy1, dummy2, dummy3;
   const int numPiecesOfSurface = topologyFile->findIslands(dummy1, dummy2, dummy3);
   if (numPiecesOfSurface > 1) {
      throw BrainModelAlgorithmException(
         "There are multiple pieces of surface.  Use Surface: Topology: Remove Islands\n"
         "to remove them and verify that the surface remains correct.");
   }

   //
   // find the geography paint column
   //
   const PaintFile* pf = brainSet->getPaintFile();
   const int geographyColumn = pf->getGeographyColumnNumber();
   if (geographyColumn < 0) {
      throw BrainModelAlgorithmException(
         "There is no geography paint column which is needed for flattening the\n"
         "partial hemisphere.  The geography paint column contains the \"CUT.FACE\"\n"
         "nodes which are needed to properly orient the partial hemisphere for\n"
         "flattening.");
   }
      
   //
   // original caret coord file and topology file names
   //
   QString originalCoordFileName(partialHemisphereSurface->getFileName());
   if (originalCoordFileName.isEmpty()) {
      originalCoordFileName = "species.case.hem.descrip.num_nodes.coord";
   }
   QString originalTopoFileName(partialHemisphereSurface->getTopologyFile()->getFileName());
   if (originalTopoFileName.isEmpty()) {
      originalTopoFileName = "species.case.hem.descrip.num_nodes.topo";
   }
   
   //
   // Make a copy of the surface
   // 
   partialHemisphereSurface = new BrainModelSurface(*partialHemisphereSurface);
   brainSet->addBrainModel(partialHemisphereSurface);
      
   if (flattenSurfaceType == FLATTEN_TYPE_FIDUCIAL) {
      //
      // Get the surfaced area of the surface
      //
      const float fiducialSurfaceArea = partialHemisphereSurface->getSurfaceArea();
      
      //
      // Convert the surface into a ellipse
      //
      partialHemisphereSurface->convertToEllipsoid();
      
      //
      // Convert the ellipse into a sphere
      //
      partialHemisphereSurface->convertEllipsoidToSphereWithSurfaceArea(fiducialSurfaceArea);
      
      //
      // update the surface's normals
      //
      partialHemisphereSurface->computeNormals();
      
      //
      // Get the number of crossovers
      //
      int numTileCrossovers, numNodeCrossovers;
      partialHemisphereSurface->crossoverCheck(numTileCrossovers, 
                                               numNodeCrossovers,
                                               BrainModelSurface::SURFACE_TYPE_SPHERICAL);
      
      //
      // Parameters for smoothing the surface
      //
      const int smoothingCycles = 20;
      const int smoothingIterations = 100;
      const int smoothingEdgeIterations = 10;
      const float smoothingStrength = 1.0;
      
      //
      // Smooth, reproject to a sphere, update normals, crossover check, and show in main window
      //
      int numCycles = 0;
      while ((numCycles < smoothingCycles) && (numNodeCrossovers > 2)) {
         partialHemisphereSurface->linearSmoothing(smoothingStrength, smoothingIterations,
                                                   smoothingEdgeIterations);
         partialHemisphereSurface->convertToSphereWithSurfaceArea(fiducialSurfaceArea);
         partialHemisphereSurface->computeNormals();
         partialHemisphereSurface->crossoverCheck(numTileCrossovers, 
                                                  numNodeCrossovers,
                                                  BrainModelSurface::SURFACE_TYPE_SPHERICAL);
         brainSet->drawBrainModel(partialHemisphereSurface);
         if (DebugControl::getDebugOn()) {
            std::cout << "Flatten Part Hem: smoothing cycle " << numCycles
                      << " tile crossovers " << numTileCrossovers
                      << " node crossovers " << numNodeCrossovers << std::endl;
         }
         numCycles++;
      }
       
      //
      // If there are crossovers and running in interactive environment,
      // allow user to decide whether or not to continue.
      //
      if (numNodeCrossovers > 2) {
         QWidget* parent = brainSet->getProgressDialogParent();
         if (parent != NULL) {
            std::ostringstream msg;
            msg << "There are " << numNodeCrossovers << " node crossovers.\n"
                << "Continue flattening ?";
            if (QMessageBox::warning(parent, "Crossovers Warning",
                                     msg.str().c_str(), "Yes", "No") != 0) {
               return;
            }
         }
         else {
            if (DebugControl::getDebugOn()) {
               std::cout << "INFO: partial hemisphere has " << numNodeCrossovers << " node crossovers." 
                         << std::endl;
            }
         }
      }
   }
   else if (flattenSurfaceType == FLATTEN_TYPE_ELLIPSOID) {
      //
      // Translate the surface to its center of mass
      //
      partialHemisphereSurface->translateToCenterOfMass();
      
      //
      // Convert the ellipsoid into a sphere whose surface area is that of the fiducial surface
      //
      const float surfaceArea = fiducialSurface->getSurfaceArea();
      partialHemisphereSurface->convertEllipsoidToSphereWithSurfaceArea(surfaceArea);
   }
      
   //
   // Save the spherical surface
   //
   const QString sphereCoordName = 
      FileUtilities::changeCaretDataFileDescriptionType(originalCoordFileName, "SPHERE");
   try {
      brainSet->writeCoordinateFile(sphereCoordName,
                                    BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                    partialHemisphereSurface->getCoordinateFile());
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
                                 
   //
   // Copy the surface
   //
   partialHemisphereSurface = new BrainModelSurface(*partialHemisphereSurface);
   brainSet->addBrainModel(partialHemisphereSurface);
   partialHemisphereSurface->appendToCoordinateFileComment("Flattening partial hemisphere\n");
   
   //
   // Orient the sphere so that "CUT.FACE" nodes are on the negative Z axis
   //
   std::vector<QString> paintNames;
   paintNames.push_back("CUT.FACE");
   QString errorMessage;
   if (partialHemisphereSurface->orientPaintedNodesToNegativeZAxis(brainSet->getPaintFile(),
                                                                   paintNames,
                                                                     geographyColumn,
                                                                     errorMessage)) {
      throw BrainModelAlgorithmException(errorMessage);
   }
   
   //
   // Copy topology file and set its type to OPEN
   //
   const TopologyFile* oldTopology = partialHemisphereSurface->getTopologyFile();
   TopologyFile* topology = new TopologyFile(*oldTopology);
   topology->setTopologyType(TopologyFile::TOPOLOGY_TYPE_OPEN);
   brainSet->addTopologyFile(topology);
   partialHemisphereSurface->setTopologyFile(topology);
   
   //
   // Disconnect any nodes with names CUT.FACE or MEDIAL.WALL
   //
   std::vector<QString> disconnectNames;
   disconnectNames.push_back("CUT.FACE");
   disconnectNames.push_back("MEDIAL.WALL");
   brainSet->disconnectNodes(topology, disconnectNames, geographyColumn);
   
   //
   // Remove any stragglers (tiles that are connected to other tiles only at a single node)
   //
   partialHemisphereSurface->getTopologyFile()->removeCornerTiles(2);
   partialHemisphereSurface->moveDisconnectedNodesToOrigin();

   //
   // Classify the nodes
   //
   brainSet->classifyNodes(topology);
   
   //
   // Save the open topology file
   //
   const QString openTopoName = 
      FileUtilities::changeCaretDataFileDescriptionType(originalTopoFileName, "OPEN");
   try {
      brainSet->writeTopologyFile(openTopoName,
                                 TopologyFile::TOPOLOGY_TYPE_OPEN,
                                 topology);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
                                 
   //
   // Project the surface flat
   //
   partialHemisphereSurface->convertSphereToFlat();
   
   //
   // Smooth just a little bit
   //
   partialHemisphereSurface->arealSmoothing(1.0, 5, 0);
   
   //
   // Update normals
   //
   partialHemisphereSurface->computeNormals();
   
   //
   // Scale the surface to 10x the fiducial area
   //
   partialHemisphereSurface->scaleSurfaceToArea(10.0 * fiducialSurface->getSurfaceArea(), false);

   //
   // Translate to center of mass and scale to fit the window
   //
   partialHemisphereSurface->translateToCenterOfMass();
   partialHemisphereSurface->updateForDefaultScaling();
               
   //
   // Determine crossovers
   //
   int numTileCrossovers, numNodeCrossovers;
   partialHemisphereSurface->crossoverCheck(numTileCrossovers, 
                                            numNodeCrossovers,
                                            BrainModelSurface::SURFACE_TYPE_FLAT);
   
   //
   // Save the flat surface
   //
   const QString flatCoordName = 
      FileUtilities::changeCaretDataFileDescriptionType(originalCoordFileName, "InitialFlat");
   try {
      brainSet->writeCoordinateFile(flatCoordName,
                                    BrainModelSurface::SURFACE_TYPE_FLAT,
                                    partialHemisphereSurface->getCoordinateFile());
                                 
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
}


      

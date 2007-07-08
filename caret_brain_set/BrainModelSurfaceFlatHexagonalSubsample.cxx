
#include <iostream>

#include "BorderFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceFlatHexagonalSubsample.h"
#include "BrainModelSurfacePointProjector.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "MathUtilities.h"

/// used to create hexagonal surface
class HexagonalNode {
   public:
      float xyz[3];
      int indexX;
      int indexY;
      int nn;
      int mm;
      int tile;
      int tileNodes[3];
      float tileAreas[3];
      int nearestNode;
      
      bool isUsed;
      int  nodeIndex;
      HexagonalNode(const float xyzIn[3],
                    const int indexXIn, const int indexYIn, const int nnIn, const int mmIn,
                    const int tileIn, const int tileNodesIn[3], const float tileAreasIn[3],
                    const int nearestNodeIn) {
         xyz[0] = xyzIn[0];
         xyz[1] = xyzIn[1];
         xyz[2] = xyzIn[2];
         indexX = indexXIn;
         indexY = indexYIn;
         nn     = nnIn;
         mm     = mmIn;
         tile   = tileIn;
         tileNodes[0] = tileNodesIn[0];
         tileNodes[1] = tileNodesIn[1];
         tileNodes[2] = tileNodesIn[2];
         tileAreas[0] = tileAreasIn[0];
         tileAreas[1] = tileAreasIn[1];
         tileAreas[2] = tileAreasIn[2];
         nearestNode  = nearestNodeIn;
         
         isUsed   = false;
         nodeIndex = -1;
      }
};


/**
 * Constructor.
 */
BrainModelSurfaceFlatHexagonalSubsample::BrainModelSurfaceFlatHexagonalSubsample(
                                              BrainSet* brainSetIn,
                                              BrainModelSurface* fiducialSurfaceIn,
                                              BrainModelSurface* flatSurfaceIn,
                                              const int numberOfResamplingTilesIn)
   : BrainModelAlgorithm(brainSetIn)
{
   fiducialSurfaceForResampling = fiducialSurfaceIn;
   flatSurfaceForResampling     = flatSurfaceIn;
   numberOfResamplingTiles      = numberOfResamplingTilesIn;

   subSampBrainSet = NULL;
   brainSetRetrieved = false;
}

/**
 * Destructor
 */
BrainModelSurfaceFlatHexagonalSubsample::~BrainModelSurfaceFlatHexagonalSubsample()
{
   if (brainSetRetrieved == false) {
      delete subSampBrainSet;
   }
}

/**
 * Create the subsampled surface.
 *
 * This creates a hexagonal grid that overlays the flat surface.
 * Initially, each row in the grid has the same number of nodes.  The next row up is shifted
 * by one half the distance between nodes.  This results in the nodes forming a parellelogram.
 * After the nodes have been created, the topology is created.  If any of the tile's nodes are 
 * inside the original surface provided by the user all three of the nodes and the tile are
 * retained.  Once the topology has been determined, nodes that are not part of the topology
 * are discarded.
 */
void
BrainModelSurfaceFlatHexagonalSubsample::execute() throw (BrainModelAlgorithmException)
{
   if (flatSurfaceForResampling == NULL) {
      throw BrainModelAlgorithmException("No flat surface was provided.");
   }
   if (fiducialSurfaceForResampling == NULL) {
      throw BrainModelAlgorithmException("No fiducial surface was provided.");
   }
   
   //
   // Set number of tiles if user has not provided number
   //
   TopologyFile* topologyForResampling = flatSurfaceForResampling->getTopologyFile();
   if (numberOfResamplingTiles < 0) {
      if (topologyForResampling == NULL) {
         throw BrainModelAlgorithmException("Flat surface has no topology.");
      }
      if (topologyForResampling->getNumberOfTiles() <= 0) {
         throw BrainModelAlgorithmException("Flat surface has no tiles");
      }
      numberOfResamplingTiles = static_cast<int>(topologyForResampling->getNumberOfTiles() * 0.3);
   }

   //
   // Get the bounds of the flat surface
   //
   CoordinateFile* fiducialCoords = fiducialSurfaceForResampling->getCoordinateFile();
   CoordinateFile* flatCoords = flatSurfaceForResampling->getCoordinateFile();
   float bounds[6];
   flatCoords->getBounds(bounds);

   if (DebugControl::getDebugOn()) {
      try {
         BrainModelSurface tempf(*fiducialSurfaceForResampling);
         tempf.getCoordinateFile()->writeFile("debug_hex_subsamp_fiducial.coord");
         
         BrainModelSurface tempf2(*flatSurfaceForResampling);
         tempf2.getCoordinateFile()->writeFile("debug_hex_subsamp_flat.coord");
      }
      catch (FileException&) {
      }
   }
   
   //
   // Average distance between each linked pair of nodes in flat surface
   //
   const float meanDistance = flatSurfaceForResampling->getMeanDistanceBetweenNodes();

   //
   // Area of flat surface
   //
   const float totalArea = flatSurfaceForResampling->getSurfaceArea();
   if (DebugControl::getDebugOn()) {
      std::cout << "Mean Distance between nodes: " << meanDistance << std::endl;
      std::cout << "Total surface area: " << totalArea << std::endl;
      std::cout << "Resample to " << numberOfResamplingTiles 
                << " tiles." << std::endl;
   }
   

   //
   // Determine spacing between resampled nodes
   //
   const float t1 = 4.0 * totalArea;
   const float sqrtOf3 = 1.7320508075;
   const float t2 = sqrtOf3 * numberOfResamplingTiles;
   const float spaceBetweenNodes = sqrt (t1/t2);
   if (DebugControl::getDebugOn()) {
      std::cout << "Spacing between resampled nodes: " << spaceBetweenNodes << std::endl;
   }

   //
   // 1/2 of surface height
   //
   const float dist = (bounds[3] - bounds[2])/2.0;

   //
   // Determine bounds of parallelogram overlaying input brain model surface
   //
   const int Nmin = static_cast<int>(((bounds[0] - fabs(dist*0.57735))/spaceBetweenNodes) - 20.0);
   const int Nmax = static_cast<int>(((bounds[1] + fabs(dist*0.57735))/spaceBetweenNodes) + 20.0);
   const int Mmin = static_cast<int>(((bounds[2] *(2.0/sqrtOf3))/spaceBetweenNodes) - 2.0);  
   const int Mmax = static_cast<int>(((bounds[3]*(2.0/sqrtOf3))/spaceBetweenNodes) + 2.0);
   const int Nstop = Nmax - Nmin;
   const int Mstop = Mmax - Mmin;
   if (DebugControl::getDebugOn()) {
      std::cout << "New Surface Extent N " << Nmin << " " << Nmax << " M "
                << Mmin << " " << Mmax << std::endl;
      std::cout << "New dimensions: " << Nstop << " " << Mstop << std::endl;
   }
   
   
   
   //
   // Point Projector to project new surface points into original surface
   //
   BrainModelSurfacePointProjector bspp(flatSurfaceForResampling,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_FLAT,
                           false);
   
   //
   // Create the new brain set
   //
   subSampBrainSet = new BrainSet;
   subSampBrainSet->setStructure(fiducialSurfaceForResampling->getStructure());
   
   //
   // Create a new BrainModelSurface (both fiducial and flat and a flat that does not get morphed)
   //
   BrainModelSurface* fiducialSubSampSurface = new BrainModelSurface(subSampBrainSet);
   fiducialSubSampSurface->setSurfaceType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   fiducialSubSampSurface->setStructure(fiducialSurfaceForResampling->getStructure());
   BrainModelSurface* flatSubSampSurface = new BrainModelSurface(subSampBrainSet);
   flatSubSampSurface->setSurfaceType(BrainModelSurface::SURFACE_TYPE_FLAT);
   flatSubSampSurface->setStructure(flatSurfaceForResampling->getStructure());
   
   //
   // add surfaces to brain set
   //
   subSampBrainSet->addBrainModel(fiducialSubSampSurface);
   subSampBrainSet->addBrainModel(flatSubSampSurface);
   
   //
   // height of an equilateral triangle with base = 1.0
   //
   const float triangleHeight = sqrtOf3 / 2.0; 

   Border border("nodes");
   
   //
   // Keep track of coordinates and which nodes are used
   //
   std::vector<HexagonalNode> hexNodes;

   //
   // Subsample the current surface
   //
   // This loop iterates through a "parallelogram" that overlaps the surface.  
   //
   for (int mm = Mmin; mm < Mmax; mm++) {
      for (int nn = Nmin; nn < Nmax; nn++) {
         const float p[3] = { (nn + mm/2.0) * spaceBetweenNodes,
                              triangleHeight * mm * spaceBetweenNodes,
                              0.0 };
                              
         //
         // Indices that start at zero for X & Y
         //
         const int indexX = nn - Nmin;
         const int indexY = mm - Mmin;
         
         //
         // Project point to original surface
         //
         int nearestNode = -1;
         int tileNodes[3];
         float areas[3];
         const int tile = bspp.projectBarycentric(p, nearestNode,
                                                   tileNodes, areas, true);
          
         HexagonalNode hexNode(p, indexX, indexY, nn, mm, tile, tileNodes, areas, nearestNode);
         hexNodes.push_back(hexNode);
      }
   }
   
   //
   // Create topology for the subsampled surface
   //
   TopologyFile* subSampTopology = new TopologyFile;
   subSampTopology->setTopologyType(topologyForResampling->getTopologyType());
   subSampBrainSet->addTopologyFile(subSampTopology);
   
   for (int j = 0; j < (Mstop - 1); j++) {
      for (int i = 0; i < Nstop; i++) {
         const int hexNodeIndex = (j * Nstop) + i;
         HexagonalNode& hexNode = hexNodes[hexNodeIndex];
         
         if (i > 0) {
            //
            // Triangle with neighbor above and left, and other neighbor above and right
            //
            const int neighAboveIndex = hexNodeIndex + Nstop;
            const int neighAboveLeftIndex  = neighAboveIndex - 1;
            HexagonalNode& neighAboveHexNode = hexNodes[neighAboveIndex];
            HexagonalNode& neighAboveLeftHexNode = hexNodes[neighAboveLeftIndex];
            
            //
            // If any of the nodes projected to the surface, use the triangle
            //
            if ((hexNode.tile >= 0) || (neighAboveHexNode.tile >= 0) ||
                (neighAboveLeftHexNode.tile >= 0)) {
               hexNode.isUsed               = true;
               neighAboveHexNode.isUsed     = true;
               neighAboveLeftHexNode.isUsed = true;
               subSampTopology->addTile(hexNodeIndex, neighAboveIndex, neighAboveLeftIndex);
            }
         }
         
         if (i < (Nstop - 1)) {
            //
            // Triangle with neighbor to the right and other neighbor above and right 
            //
            const int neighRightIndex = hexNodeIndex + 1;
            const int neighAboveIndex = hexNodeIndex + Nstop;
            HexagonalNode& neighRightHexNode = hexNodes[neighRightIndex];
            HexagonalNode& neighAboveHexNode = hexNodes[neighAboveIndex];
            
            //
            // If any of the nodes projected to the surface, use the triangle
            //
            if ((hexNode.tile >= 0) || (neighRightHexNode.tile >= 0) ||
                (neighAboveHexNode.tile >= 0)) {
               hexNode.isUsed = true;
               neighRightHexNode.isUsed = true;
               neighAboveHexNode.isUsed = true;
               subSampTopology->addTile(hexNodeIndex, neighRightIndex, neighAboveIndex);
            }
         }
      }
   }
   
   //
   // Find the resample points that are actually used
   //
   const int numHexNodes = static_cast<int>(hexNodes.size());
   for (int i = 0; i < numHexNodes; i++) {
      HexagonalNode& hexNode = hexNodes[i];
      
      //
      // Was this node marked for use
      //
      if (hexNode.isUsed) {
         //
         // Get the corresponding position in the Fiducial coord
         //
         float fiducialXYZ[3] = { 0.0, 0.0, 0.0 };
         if (hexNode.tile >= 0) {
            BrainModelSurfacePointProjector::unprojectPoint(hexNode.tileNodes, 
                                                            hexNode.tileAreas,
                                                            fiducialCoords, 
                                                            fiducialXYZ);
         }
         else {
            fiducialCoords->getCoordinate(hexNode.nearestNode, fiducialXYZ);
         }
         fiducialSubSampSurface->addNode(fiducialXYZ);
            
         //
         // Add to the flat coordinate
         //
         flatSubSampSurface->addNode(hexNode.xyz);
         
         //
         // Set the new index for this point
         //
         hexNode.nodeIndex = flatSubSampSurface->getNumberOfNodes() - 1;
         
         subSampBrainSet->resetNodeAttributes();
         BrainSetNodeAttribute* bna = subSampBrainSet->getNodeAttributes(hexNode.nodeIndex);
         bna->setFlatMorphAttributes(hexNode.mm, hexNode.nn, hexNode.nearestNode);
      }
   }
   
   //
   // Update the topology file for new node indices
   //
   const int numTiles = subSampTopology->getNumberOfTiles();
   for (int i = 0; i < numTiles; i++) {
      int v1, v2, v3;
      subSampTopology->getTile(i, v1, v2, v3);
      v1 = hexNodes[v1].nodeIndex;
      v2 = hexNodes[v2].nodeIndex;
      v3 = hexNodes[v3].nodeIndex;
      subSampTopology->setTile(i, v1, v2, v3);
   }

   //
   // Assign the topology file to both fiducial and flat surfaces
   //
   fiducialSubSampSurface->setTopologyFile(subSampTopology);
   flatSubSampSurface->setTopologyFile(subSampTopology);
   
   //
   // Make sure suffient nodes
   //
   const int numSubSampNodes = flatSubSampSurface->getNumberOfNodes();
   if (numSubSampNodes <= 3) {
      //delete flatSubSampSurface;
      //delete fiducialSubSampSurface;
      //delete subSampTopology;
      delete subSampBrainSet;
      throw BrainModelAlgorithmException("Subsampled surface has no tiles.");
   }

   float resampBounds[6];
   flatSubSampSurface->getCoordinateFile()->getBounds(resampBounds);
   if (DebugControl::getDebugOn()) {
      std::cout << "Resamp Bounds: " << resampBounds[0] << " " << resampBounds[1] << " "
                << resampBounds[2] << " " << resampBounds[3] << std::endl;
   }

   BorderFile bf(subSampTopology, flatSubSampSurface->getCoordinateFile());
   bf.setHeaderTag(AbstractFile::headerTagConfigurationID,
                                  SpecFile::flatBorderFileTagName);
   bf.writeFile("coords_as_border.border");
}

/**
 * get the subsampled surface
 */
BrainSet*
BrainModelSurfaceFlatHexagonalSubsample::getSubsampledBrainSet()
{
   brainSetRetrieved = true;
   return subSampBrainSet;
}


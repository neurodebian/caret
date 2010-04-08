#include <limits>

#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderToPaintConverter.h"
#include "DebugControl.h"
#include "PaintFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

// constructor
BrainModelSurfaceBorderToPaintConverter::BrainModelSurfaceBorderToPaintConverter(
                           BrainSet* bs,
                           BrainModelSurface* surfaceIn,
                           BorderProjectionFile* borderProjectionFileIn,
                           PaintFile* paintFileIn,
                           int paintColumnNumberIn,
                           QString paintColumnNameIn)
        : BrainModelAlgorithm(bs),
          surface(surfaceIn),
          borderProjectionFile(borderProjectionFileIn),
          paintFile(paintFileIn),
          paintColumnNumber(paintColumnNumberIn),
          paintColumnName(paintColumnNameIn)
{
}

// destructor
BrainModelSurfaceBorderToPaintConverter::~BrainModelSurfaceBorderToPaintConverter()
{

}

// execute the algorithm
void
BrainModelSurfaceBorderToPaintConverter::execute()
                                     throw (BrainModelAlgorithmException)
{
   //
   // Verify inputs
   //
   if (surface == NULL) {
      throw BrainModelAlgorithmException("Surface is invalid.");
   }
   const int numNodes = surface->getNumberOfNodes();
   if (numNodes <= 0) {
      throw BrainModelAlgorithmException("Surface contains no nodes.");
   }
   const CoordinateFile* cf = surface->getCoordinateFile();
   const TopologyFile* topologyFile = surface->getTopologyFile();
   if (topologyFile == NULL) {
      throw BrainModelAlgorithmException("Topology is invalid.");
   }
   const TopologyHelper* th = topologyFile->getTopologyHelper(false, true, false);
   if (borderProjectionFile == NULL) {
      throw BrainModelAlgorithmException("Border Projection File is invalid.");
   }
   const int numBorders = borderProjectionFile->getNumberOfBorderProjections();
   if (numBorders <= 0) {
      throw BrainModelAlgorithmException(
              "Border Projection File contains no borders.");
   }
   if (paintFile == NULL) {
      throw BrainModelAlgorithmException("Paint File is invalid.");
   }

   //
   // Create a new paint column, if needed
   //
   if ((paintColumnNumber < 0) ||
       (paintColumnNumber >= paintFile->getNumberOfColumns())) {
      if (paintFile->getNumberOfColumns() <= 0) {
         paintFile->setNumberOfNodesAndColumns(numNodes,
                                               1);
      }
      else {
         paintFile->addColumns(1);
      }
      paintColumnNumber = paintFile->getNumberOfColumns() - 1;
   }
   paintFile->setColumnName(paintColumnNumber, paintColumnName);

   //
   // Debugging info
   //
   int debugNode = -1;
   if (DebugControl::getDebugOn()) {
      if ((DebugControl::getDebugNodeNumber() >= 0) &&
          (DebugControl::getDebugNodeNumber() < paintFile->getNumberOfNodes())) {
         debugNode = DebugControl::getDebugNodeNumber();
      }
   }

   //
   // Distance from node to nearest border point
   //
   std::vector<float> nodeToBorderDistance(numNodes, std::numeric_limits<float>::max());
   std::vector<QString> nodePaintName(numNodes, "");

   //
   // Loop through the border projections
   //
   for (int i = 0; i < numBorders; i++) {
      const BorderProjection* bp = borderProjectionFile->getBorderProjection(i);
      QString name;
      float center[3], samplingDensity, variance, topography, arealUncertainty;
      bp->getData(name, center, samplingDensity, variance,
                  topography, arealUncertainty);
      const int numLinks = bp->getNumberOfLinks();
      for (int j = 0; j < numLinks; j++) {
         const BorderProjectionLink* bpl = bp->getBorderProjectionLink(j);
         int section, nodeIndices[3];
         float nodeAreas[3], radius;
         bpl->getData(section, nodeIndices, nodeAreas, radius);

         const int n1 = nodeIndices[0];
         const int n2 = nodeIndices[1];
         const int n3 = nodeIndices[2];

         if (th->getNodeHasNeighbors(n1) &&
             th->getNodeHasNeighbors(n2) &&
             th->getNodeHasNeighbors(n3)) {

            float borderPointXYZ[3];
            bpl->unprojectLink(cf, borderPointXYZ);

            //
            // Find nodes closest to border point
            //
            for (int m = 0; m < 3; m++) {
               const int node = nodeIndices[m];
               const float distSQ = cf->getDistanceToPointSquared(node, borderPointXYZ);
               if (distSQ < nodeToBorderDistance[node]) {
                  nodePaintName[node] = name;
                  nodeToBorderDistance[node] = distSQ;
               }
            }
         }
      }
   }

   //
   // Assign the paint names to the nodes
   //
   for (int i = 0; i < numNodes; i++) {
      if (nodePaintName[i].isEmpty() == false) {
         const int paintIndex = this->paintFile->addPaintName(nodePaintName[i]);
         this->paintFile->setPaint(i, paintColumnNumber, paintIndex);
      }
   }
}



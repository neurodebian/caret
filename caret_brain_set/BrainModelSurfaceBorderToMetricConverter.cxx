
#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderToMetricConverter.h"
#include "DebugControl.h"
#include "MetricFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

// constructor
BrainModelSurfaceBorderToMetricConverter::BrainModelSurfaceBorderToMetricConverter(
                           BrainSet* bs,
                           BrainModelSurface* surfaceIn,
                           BorderProjectionFile* borderProjectionFileIn,
                           MetricFile* metricFileIn,
                           int metricColumnNumberIn,
                           QString metricColumnNameIn)
        : BrainModelAlgorithm(bs),
          surface(surfaceIn),
          borderProjectionFile(borderProjectionFileIn),
          metricFile(metricFileIn),
          metricColumnNumber(metricColumnNumberIn),
          metricColumnName(metricColumnNameIn)
{
}

// destructor
BrainModelSurfaceBorderToMetricConverter::~BrainModelSurfaceBorderToMetricConverter()
{

}

// execute the algorithm
void
BrainModelSurfaceBorderToMetricConverter::execute()
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
   if (metricFile == NULL) {
      throw BrainModelAlgorithmException("Metric File is invalid.");
   }

   //
   // Create a new metric column, if needed
   //
   if ((metricColumnNumber < 0) ||
       (metricColumnNumber >= metricFile->getNumberOfColumns())) {
      if (metricFile->getNumberOfColumns() <= 0) {
         metricFile->setNumberOfNodesAndColumns(surface->getNumberOfNodes(),
                                                1);
      }
      else {
         metricFile->addColumns(1);
      }
      metricColumnNumber = metricFile->getNumberOfColumns() - 1;
   }
   metricFile->setColumnName(metricColumnNumber, metricColumnName);

   //
   // Debugging info
   //
   int debugNode = -1;
   if (DebugControl::getDebugOn()) {
      if ((DebugControl::getDebugNodeNumber() >= 0) &&
          (DebugControl::getDebugNodeNumber() < metricFile->getNumberOfNodes())) {
         debugNode = DebugControl::getDebugNodeNumber();
      }
   }

   //
   // Sums of border variances for each node
   //
   std::vector<double> nodeSum(numNodes, 0.0);
   std::vector<double> nodeWeight(numNodes, 0.0);
   std::vector<double> nodeCount(numNodes, 0.0);

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

             //
             // Indices in border projection are out of sequence
             //
             const float areas[3] = {
                nodeAreas[1],
                nodeAreas[2],
                nodeAreas[0],
             };

             float totalArea = (areas[0] + areas[1] + areas[2]);
             if (totalArea > 0.0) {
                 const float weights[3] = {
                    areas[0] / totalArea,
                    areas[1] / totalArea,
                    areas[2] / totalArea
                 };
                 nodeSum[n1] += (weights[0] * variance);
                 nodeSum[n2] += (weights[1] * variance);
                 nodeSum[n3] += (weights[2] * variance);

                 nodeWeight[n1] += weights[0]; //1.0;
                 nodeWeight[n2] += weights[1]; //1.0;
                 nodeWeight[n3] += weights[2]; //1.0;

                 nodeCount[n1] += 1.0;
                 nodeCount[n2] += 1.0;
                 nodeCount[n3] += 1.0;
             }
         }
      }
   }

   //
   // Assign the metrics
   //
   for (int i = 0; i < numNodes; i++) {
      if (i == debugNode) {
         if (nodeCount[i] > 1.0) {
         std::cout << "Border to Metric node "
                   << i
                   << ", sum "
                   << nodeSum[i]
                   << ", weight "
                   << nodeWeight[i]
                   << ", count "
                   << nodeCount[i]
                   << std::endl;
         }
      }
      float value = 0.0;
      if (nodeWeight[i] > 0.0) {
         value = nodeSum[i] / nodeWeight[i];
      }
      metricFile->setValue(i, metricColumnNumber, value);
   }
}


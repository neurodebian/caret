
#include "BrainModelSurfaceConnectedSearchMetric.h"
#include "BrainSet.h"
#include "MetricFile.h"

/**
 * Constructor
 */
BrainModelSurfaceConnectedSearchMetric::BrainModelSurfaceConnectedSearchMetric(
                                          BrainSet* bs, 
                                          const BrainModelSurface* bmsIn,
                                          const int startNodeIn,
                                          const MetricFile* metricFileIn,
                                          const int metricColumnIn,
                                          const float metricMinimumIn,
                                          const float metricMaximumIn,
                                          const std::vector<int>* limitToTheseNodesIn)
   : BrainModelSurfaceConnectedSearch(bs, bmsIn, startNodeIn, limitToTheseNodesIn),
     metricFile(metricFileIn),
     metricColumn(metricColumnIn),
     metricMinimum(metricMinimumIn),
     metricMaximum(metricMaximumIn)
{
}

/**
 * Destructor
 */
BrainModelSurfaceConnectedSearchMetric::~BrainModelSurfaceConnectedSearchMetric()
{
}

/**
 * See if nodes metric value is within the search values
 */
bool
BrainModelSurfaceConnectedSearchMetric::acceptNode(const int nodeNumber)
{
   const float metric = metricFile->getValue(nodeNumber, metricColumn);
   if ((metric >= metricMinimum) && (metric <= metricMaximum)) {
      return true;
   }
   
   return false;
}


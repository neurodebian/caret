
#ifndef __BRAIN_MODEL_SURFACE_CONNECTED_METRIC_SEARCH_H__
#define __BRAIN_MODEL_SURFACE_CONNECTED_METRIC_SEARCH_H__


#include "BrainModelSurfaceConnectedSearch.h"

class MetricFile;

/// Class that searches for connected nodes within a range of metric values
class BrainModelSurfaceConnectedSearchMetric : public BrainModelSurfaceConnectedSearch {
   public:
      /// Constructor
      BrainModelSurfaceConnectedSearchMetric(BrainSet* bs, 
                           BrainModelSurface* bmsIn,
                           const int startNodeIn,
                           const MetricFile* metricFileIn,
                           const int metricColumnIn,
                           const float metricMinimumIn,
                           const float metricMaximumIn,
                           const std::vector<int>* limitToTheseNodesIn = NULL);
                                       
      /// Destructor
      virtual ~BrainModelSurfaceConnectedSearchMetric();
      
   protected:
      /// accept a node 
      virtual bool acceptNode(const int nodeNumber);
   
      /// metric file being searched
      const MetricFile* metricFile;
      
      /// metric column for connection search
      int metricColumn;
      
      /// minimum value for search
      float metricMinimum;
      
      /// maximum value for search
      float metricMaximum;
      
};

#endif // __BRAIN_MODEL_SURFACE_CONNECTED_METRIC_SEARCH_H__


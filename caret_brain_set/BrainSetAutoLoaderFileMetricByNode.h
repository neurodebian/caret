#ifndef __BRAIN_SET_AUTO_LOADER_FILE_METRIC_BY_NODE_H__
#define __BRAIN_SET_AUTO_LOADER_FILE_METRIC_BY_NODE_H__

#include "BrainSetAutoLoaderFileMetric.h"

class BrainModelSurface;
class VolumeFile;

class BrainSetAutoLoaderFileMetricByNode : public BrainSetAutoLoaderFileMetric {
   public:
      /// constructor
      BrainSetAutoLoaderFileMetricByNode(BrainSet* bsIn,
                               const int autoLoaderIndexIn);

      /// destructor
      virtual ~BrainSetAutoLoaderFileMetricByNode();

      // auto load the metric file for the specified node number (return error message)
      virtual QString loadFileForNode(const int nodeNumber);

      // is auto loading metric file by node supported for this auto loader
      virtual bool isLoadFileForNodeSupported() const;

      // auto load the metric file for the specified voxel (return error message)
      virtual QString loadFileForVoxel(const VoxelIJK& voxel);

   protected:
};

#endif // __BRAIN_SET_AUTO_LOADER_FILE_METRIC_BY_NODE_H__

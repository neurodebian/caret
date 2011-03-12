#ifndef __BRAIN_SET_AUTOLOADER_MANAGER_H__
#define	__BRAIN_SET_AUTOLOADER_MANAGER_H__

#include "SceneFile.h"
#include "VoxelIJK.h"

class BrainSet;
class BrainSetAutoLoaderFile;
class BrainSetAutoLoaderFileFunctionalVolume;
class BrainSetAutoLoaderFileMetric;
class BrainSetAutoLoaderFileMetricByNode;
class BrainSetAutoLoaderFilePaintCluster;

/// processes automatic loading of data files
class BrainSetAutoLoaderManager {

   public:
      /// number of metric auto loaders
      enum { NUMBER_OF_METRIC_AUTO_LOADERS = 4 };

      /// number of metric node auto loaders
      enum { NUMBER_OF_METRIC_NODE_AUTO_LOADERS = 4 };

      /// number of functional volume auto loaders
      enum { NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS = 1 };

      /// number of cluster auto loaders
      enum { NUMBER_OF_CLUSTER_AUTO_LOADERS = 2 };

      // constructor
      BrainSetAutoLoaderManager(BrainSet* bs);

      // destructor
      ~BrainSetAutoLoaderManager();

      // process autoloading for a voxel.
      QString processAutoLoading(const float xyzIn[3]);

      // Process autoloading for a node. 
      QString processAutoLoading(const int nodeNumber);

      /// get a metric autoloader
      BrainSetAutoLoaderFileMetric* getMetricAutoLoader(const int indx);

      /// get a metric by node autoloader
      BrainSetAutoLoaderFileMetricByNode* getMetricNodeAutoLoader(const int indx);

      /// get a functional volume auto loader
      BrainSetAutoLoaderFileFunctionalVolume* getFunctionalVolumeAutoLoader(const int indx);

      /// get a cluster auto loader
      BrainSetAutoLoaderFilePaintCluster* getClusterAutoLoader(const int indx);

      // see if any auto loaders are selected
      bool getAnyAutoLoaderSelected();

      // reinitialize all display settings
      void reset();

      // update any selections due to changes with loaded data files
      void update();

      // apply a scene (set display settings)
      void showScene(const SceneFile::Scene& scene,
                             QString& errorMessage);

      // create a scene (read display settings)
      void saveScene(SceneFile::Scene& scene,
                             const bool onlyIfSelectedFlag,
                             QString& errorMessage);

   protected:
      /// the brain set
      BrainSet* brainSet;

      /// metric auto loaders
      BrainSetAutoLoaderFileMetric* metricAutoLoaders[NUMBER_OF_METRIC_AUTO_LOADERS];

      /// metric node auto loaders
      BrainSetAutoLoaderFileMetricByNode* metricNodeAutoLoaders[NUMBER_OF_METRIC_NODE_AUTO_LOADERS];

      /// functional volume auto loaders
      BrainSetAutoLoaderFileFunctionalVolume* functionalVolumeAutoLoaders[NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS];

      /// cluster auto loaders
      BrainSetAutoLoaderFilePaintCluster* clusterAutoLoaders[NUMBER_OF_CLUSTER_AUTO_LOADERS];

      /// all of the auto loaders
      std::vector<BrainSetAutoLoaderFile*> allFileAutoLoaders;

};
#endif	/* __BRAIN_SET_AUTOLOADER_MANAGER_H__ */


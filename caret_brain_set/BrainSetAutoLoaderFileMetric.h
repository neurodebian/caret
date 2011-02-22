#ifndef __BRAIN_SET_AUTO_LOADER_FILE_METRIC_H__
#define	__BRAIN_SET_AUTO_LOADER_FILE_METRIC_H__

#include "BrainSetAutoLoaderFile.h"
#include "SceneFile.h"

class BrainModelSurface;
class VolumeFile;

class BrainSetAutoLoaderFileMetric : public BrainSetAutoLoaderFile {
   public:
      /// constructor
      BrainSetAutoLoaderFileMetric(BrainSet* bsIn,
                               const int autoLoaderIndexIn);

      /// destructor
      ~BrainSetAutoLoaderFileMetric();

      /// reinitialize all display settings
      void reset();

      /// update any selections due to changes with loaded data files
      void update();

      /// apply a scene (set display settings)
      void showScene(const SceneFile::Scene& scene,
                             QString& errorMessage);

      /// create a scene (read display settings)
      void saveScene(SceneFile::Scene& scene,
                             const bool onlyIfSelectedFlag,
                             QString& errorMessage);

      /** get the auto load display surface */
      BrainModelSurface* getAutoLoadMetricDisplaySurface() {
         return this->autoLoadDisplaySurface;
      }

      /** set the auto load display surface */
      void setAutoLoadMetricDisplaySurface(BrainModelSurface* bms) {
         this->autoLoadDisplaySurface = bms;
      }

      /**
       * Get the last auto-loaded metric column number.
       */
      int getAutoLoadLastMetricColumnNumber() const {
         return this->autoLoadMetricLastColumnNumber;
      }

      /**
       * Set the last auto-loaded metric column number.
       */
      void setAutoLoadLastMetricColumnNumber(const int columnNumber)  {
         this->autoLoadMetricLastColumnNumber = columnNumber;
      }

      // auto load the metric file for the specified voxel (return error message)
      QString loadFileForVoxel(const VoxelIJK& voxel);

   protected:
      /// auto load metric display surface
      BrainModelSurface* autoLoadDisplaySurface;

      /// column number of last auto-loaded metric (DO NOT SAVE TO SCENE)
      int autoLoadMetricLastColumnNumber;
};

#endif	/* __BRAIN_SET_AUTO_LOADER_FILE_METRIC_H__ */


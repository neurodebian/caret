/* 
 * File:   BrainSetAutoLoaderFileFunctionalVolume.h
 * Author: john
 *
 * Created on August 21, 2009, 10:38 AM
 */

#ifndef __BRAIN_SET_AUTO_LOADER_FILE_FUNCTIONAL_VOLUME_H__
#define	__BRAIN_SET_AUTO_LOADER_FILE_FUNCTIONAL_VOLUME_H__

#include "BrainSetAutoLoaderFile.h"
#include "SceneFile.h"

class VolumeFile;

class BrainSetAutoLoaderFileFunctionalVolume : public BrainSetAutoLoaderFile {
   public:
      /// constructor
      BrainSetAutoLoaderFileFunctionalVolume(BrainSet* bsIn,
                               const int autoLoaderIndexIn);

      /// destructor
      ~BrainSetAutoLoaderFileFunctionalVolume();

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

      /**
       * Get the last auto-loaded functional volume.
       */
      VolumeFile* getAutoLoadLastFunctionalVolume() {
         return this->autoLoadMetricLastFunctionalVolume;
      }

      /**
       * Set the last auto-loaded functional volume.
       */
      void setAutoLoadLastFunctionalVolume(VolumeFile* vf)  {
         this->autoLoadMetricLastFunctionalVolume = vf;
      }

      // auto load the metric file for the specified voxel (return error message)
      QString loadFileForVoxel(const VoxelIJK& voxel);

   protected:
      /// last auto loaded functional volume (do not save to scene)
      VolumeFile* autoLoadMetricLastFunctionalVolume;
};

#endif	/* __BRAIN_SET_AUTO_LOADER_FILE_FUNCTIONAL_VOLUME_H__ */


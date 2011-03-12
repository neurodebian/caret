/* 
 * File:   BrainSetAutoLoaderFile.h
 * Author: john
 *
 * Created on August 20, 2009, 3:23 PM
 */

#ifndef __BRAIN_SET_AUTO_LOADER_FILE_H__
#define	__BRAIN_SET_AUTO_LOADER_FILE_H__

#include "SceneFile.h"
#include "VoxelIJK.h"

class BrainModelSurface;
class BrainSet;
class VolumeFile;

/// base class for file auto loaders
class BrainSetAutoLoaderFile {
   public:
      /// constructor
      BrainSetAutoLoaderFile(BrainSet* bsIn,
                               const int autoLoaderIndexIn);

      /// destructor
      virtual ~BrainSetAutoLoaderFile();

      /// reinitialize all display settings
      virtual void reset();

      /// update any selections due to changes with loaded data files
      virtual void update();

      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene,
                             QString& errorMessage) = 0;

      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene,
                             const bool onlyIfSelectedFlag,
                             QString& errorMessage) = 0;

      /** get the auto load volume file */
      VolumeFile* getAutoLoadAnatomyVolumeFile() {
         return this->autoLoadAnatomyVolumeFile;
      }

      /** set the auto load volume file */
      void setAutoLoadAnatomyVolumeFile(VolumeFile* vf) {
         this->autoLoadAnatomyVolumeFile = vf;
      }

      /** set the auto load directory name */
      QString getAutoLoadDirectoryName() const {
         return this->autoLoadDirectoryName;
      }

      /** Set the auto load directory name */
      void setAutoLoadDirectoryName(const QString& name) {
         this->autoLoadDirectoryName = name;
      }

      /** set the auto load secondary directory name */
      QString getAutoLoadSecondaryDirectoryName() const {
         return this->autoLoadSecondaryDirectoryName;
      }

      /** Set the auto load secondary directory name */
      void setAutoLoadSecondaryDirectoryName(const QString& name) {
         this->autoLoadSecondaryDirectoryName = name;
      }

      /** get the auto load enabled */
      bool getAutoLoadEnabled() const {
         return this->autoLoadEnabledFlag;
      }

      /** set the auto load enabled */
      void setAutoLoadEnabled(bool b) {
         this->autoLoadEnabledFlag = b;
      }

      /**
       * Get replace last auto loaded file.
       */
      bool getAutoLoadReplaceLastFileEnabled() const {
         return this->autoLoadReplaceLastFileFlag;
      }

      /**
       * Set replace last autoloaded last file.
       */
      void setAutoLoadReplaceLastFileEnabled(const bool b) {
         this->autoLoadReplaceLastFileFlag = b;
      }

      // Get the last auto load anatomy volume indices.
      VoxelIJK getLastAutoLoadAnatomyVoxelIndices() const;

      // Set the last auto load anatomy volume indices.
      void setLastAutoLoadAnatomyVoxelIndices(const VoxelIJK& voxel);

      // auto load the metric file for the specified voxel (return error message)
      virtual QString loadFileForVoxel(const VoxelIJK& voxel) = 0;

      // auto load the metric file for the specified node number (return error message)
      virtual QString loadFileForNode(const int nodeNumber);

      // is auto loading metric file by node supported for this auto loader
      virtual bool isLoadFileForNodeSupported() const;

      /** get the auto load volume intersection surface */
      BrainModelSurface* getAutoLoadVolumeIntersectionSurface() {
         return this->autoLoadVolumeIntersectionSurface;
      }

      /** set the auto load volume intersection surface */
      void setAutoLoadVolumeIntersectionSurface(BrainModelSurface* bms) {
         this->autoLoadVolumeIntersectionSurface = bms;
      }

   protected:
      /// Help with applying a scene.
      void showSceneHelper(const SceneFile::SceneClass& sc);

      /// Help saving a scene.
      void saveSceneHelper(SceneFile::SceneClass& sc);

      /// the brain set
      BrainSet* brainSet;

      /// auto load anatomical volume file
      VolumeFile* autoLoadAnatomyVolumeFile;

      /// directory containing auto load files
      QString autoLoadDirectoryName;

      /// secondary directory containing auto load files
      QString autoLoadSecondaryDirectoryName;

      /// auto load file enabled
      bool autoLoadEnabledFlag;

      /// replace previous auto loaded file flag
      bool autoLoadReplaceLastFileFlag;

      /// index of this autoloader
      const int autoLoaderIndex;

      /// get the number of previously loaded voxels. */
      int getNumberOfPreviouslyLoadedVoxels() const { return previouslyLoadedVoxels.size(); }

      /// get a previously loaded voxel (zero is oldest).
      VoxelIJK getPreviouslyLoadedVoxel(const int indx) { return previouslyLoadedVoxels[indx]; }

private:
      /// previously loaded voxels
      std::vector<VoxelIJK> previouslyLoadedVoxels;

      /// auto load volume intersection surface
      BrainModelSurface* autoLoadVolumeIntersectionSurface;
};



#endif	/* __BRAIN_SET_AUTO_LOADER_FILE_H__ */


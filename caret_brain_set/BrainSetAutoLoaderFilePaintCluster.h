#ifndef __BRAIN_SET_AUTO_LOADER_FILE_PAINT_CLUSTER_H__
#define	__BRAIN_SET_AUTO_LOADER_FILE_PAINT_CLUSTER_H__

#include "BrainSetAutoLoaderFile.h"
#include "SceneFile.h"

class BrainModelSurface;
class VolumeFile;

class BrainSetAutoLoaderFilePaintCluster : public BrainSetAutoLoaderFile {
   public:
      /// constructor
      BrainSetAutoLoaderFilePaintCluster(BrainSet* bsIn,
                               const int autoLoaderIndexIn);

      /// destructor
      ~BrainSetAutoLoaderFilePaintCluster();

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
      BrainModelSurface* getAutoLoadDisplaySurface() {
         return this->autoLoadDisplaySurface;
      }

      /** set the auto load display surface */
      void setAutoLoadDisplaySurface(BrainModelSurface* bms) {
         this->autoLoadDisplaySurface = bms;
      }

      /**
       * Get the last auto-loaded cluster metric column number.
       */
      int getAutoLoadLastMetricColumnNumber() const {
         return this->autoLoadClusterLastMetricColumnNumber;
      }

      /**
       * Set the last auto-loaded paint cluster number.
       */
      void setAutoLoadLastMetricColumnNumber(const int columnNumber)  {
         this->autoLoadClusterLastMetricColumnNumber = columnNumber;
      }

      // auto load the metric and volume file for the specified node (return error message)
      QString loadFileForNode(const int nodeNumber);

      /**
       * Get the last auto-loaded functional volume.
       */
      VolumeFile* getAutoLoadLastFunctionalVolume() {
         return this->autoLoadClusterLastFunctionalVolume;
      }

      /**
       * Set the last auto-loaded functional volume.
       */
      void setAutoLoadLastFunctionalVolume(VolumeFile* vf)  {
         this->autoLoadClusterLastFunctionalVolume = vf;
      }

      // Get the last auto load node number.
      int getLastAutoLoadNodeNumber() const;

      // Set the last auto load node number.
      void setLastAutoLoadNodeNumber(const int nodeNumber);

      /// get the selected paint column number
      int getPaintColumnNumber();

      /// set the selected paint column number
      void setPaintColumnNumber(const int columnNumber);

      /// Auto load the metric file for the specified voxel (return error message).
      QString loadFileForVoxel(const VoxelIJK& /*voxel*/) { return ""; } // NOT USED

   protected:
      /// get the number of previously node numbers. */
      int getNumberOfPreviouslyLoadedNodeNumbers() const { return previouslyLoadedNodeNumbers.size(); }

      /// get a previously node number (zero index is oldest).
      int getPreviouslyLoadedNodeNumber(const int indx) const { return previouslyLoadedNodeNumbers[indx]; }

      /// auto load cluster display surface
      BrainModelSurface* autoLoadDisplaySurface;

      /// column number of last auto-loaded metric (DO NOT SAVE TO SCENE)
      int autoLoadClusterLastMetricColumnNumber;

      /// last auto loaded functional volume (do not save to scene)
      VolumeFile* autoLoadClusterLastFunctionalVolume;

      /// previously loaded node numbers
      std::vector<int> previouslyLoadedNodeNumbers;

      /// selected paint column number
      int paintColumnNumber;

};

#endif	/* __BRAIN_SET_AUTO_LOADER_FILE_PAINT_CLUSTER_H__ */


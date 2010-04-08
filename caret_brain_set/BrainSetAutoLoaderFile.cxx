#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "BrainSetAutoLoaderFile.h"
#include "StringUtilities.h"
#include "VolumeFile.h"

/**
 *  constructor.
 */
BrainSetAutoLoaderFile::BrainSetAutoLoaderFile(BrainSet* bsIn,
                                                      const int autoLoaderIndexIn)
   : brainSet(bsIn),
     autoLoaderIndex(autoLoaderIndexIn)
{
   reset();
}

/**
 *  destructor.
 */
BrainSetAutoLoaderFile::~BrainSetAutoLoaderFile()
{

}

/**
 *  reinitialize all display settings.
 */
void
BrainSetAutoLoaderFile::reset()
{
   autoLoadVolumeIntersectionSurface = NULL;
   autoLoadAnatomyVolumeFile = NULL;
   autoLoadDirectoryName = "";
   autoLoadSecondaryDirectoryName = "";
   autoLoadEnabledFlag = false;
   autoLoadReplaceLastFileFlag = false;
   previouslyLoadedVoxels.clear();
}

/**
 *  update any selections due to changes with loaded data files.
 */
void
BrainSetAutoLoaderFile::update()
{
   //validate volume, surface, etc

   //brainSet->getVolumeAnatomyFileByName(const QString& filename);
   //brainSet->getBrainModelSurfaceByCoordinateFileName(const QString& filename);

}

/**
 * Help with applying a scene.
 */
void
BrainSetAutoLoaderFile::showSceneHelper(const SceneFile::SceneClass& sc)
{
   const int num = sc.getNumberOfSceneInfo();
   for (int i = 0; i < num; i++) {
      const SceneFile::SceneInfo* si = sc.getSceneInfo(i);
      const QString infoName = si->getName();

      if (infoName == "autoLoadVolumeIntersectionSurface") {
         autoLoadVolumeIntersectionSurface =
            brainSet->getBrainModelSurfaceWithCoordinateFileName(si->getValueAsString());
      }
      else if (infoName == "autoLoadAnatomyVolumeFile") {
         autoLoadAnatomyVolumeFile =
           brainSet->getVolumeAnatomyFileWithName(si->getValueAsString());
      }
      else if (infoName == "autoLoadDirectoryName") {
         si->getValue(autoLoadDirectoryName);
      }
      else if (infoName == "autoLoadSecondaryDirectoryName") {
         si->getValue(autoLoadSecondaryDirectoryName);
      }
      else if (infoName == "autoLoadEnabledFlag") {
         si->getValue(autoLoadEnabledFlag);
      }
      else if (infoName == "autoLoadReplaceLastFileFlag") {
         si->getValue(autoLoadReplaceLastFileFlag);
      }
      else if (infoName == "previouslyLoadedVoxels") {
         std::vector<int> indices;
         StringUtilities::token(si->getValueAsString(), " ", indices);
         if (indices.size() >= 3) {
            VoxelIJK ijk(indices[0], indices[1], indices[2]);
            previouslyLoadedVoxels.push_back(ijk);
         }
      }
   }

}

/**
 * Help saving a scene.
 */
void
BrainSetAutoLoaderFile::saveSceneHelper(SceneFile::SceneClass& sc)
{
   if (autoLoadVolumeIntersectionSurface != NULL) {
      sc.addSceneInfo(SceneFile::SceneInfo("autoLoadVolumeIntersectionSurface",
                                           autoLoadVolumeIntersectionSurface->getCoordinateFile()->getFileNameNoPath()));
   }
   if (autoLoadAnatomyVolumeFile != NULL) {
      sc.addSceneInfo(SceneFile::SceneInfo("autoLoadAnatomyVolumeFile",
                                           autoLoadAnatomyVolumeFile->getFileNameNoPath()));
   }
   sc.addSceneInfo(SceneFile::SceneInfo("autoLoadDirectoryName",
                                        autoLoadDirectoryName));
   sc.addSceneInfo(SceneFile::SceneInfo("autoLoadSecondaryDirectoryName",
                                        autoLoadSecondaryDirectoryName));
   sc.addSceneInfo(SceneFile::SceneInfo("autoLoadEnabledFlag",
                                        autoLoadEnabledFlag));
   sc.addSceneInfo(SceneFile::SceneInfo("autoLoadReplaceLastFileFlag",
                                        autoLoadReplaceLastFileFlag));
   for (unsigned int i = 0; i < previouslyLoadedVoxels.size(); i++) {
      VoxelIJK voxel = previouslyLoadedVoxels[i];
      std::vector<int> indices;
      indices.push_back(voxel.getI());
      indices.push_back(voxel.getJ());
      indices.push_back(voxel.getK());
      sc.addSceneInfo(SceneFile::SceneInfo("previouslyLoadedVoxels",
                                       StringUtilities::combine(indices, " ")));
   }
}

/**
 * Get the last auto load anatomy volume indices.
 */
VoxelIJK
BrainSetAutoLoaderFile::getLastAutoLoadAnatomyVoxelIndices() const {
   VoxelIJK voxel;
   int num = static_cast<int>(previouslyLoadedVoxels.size());
   if (num > 0) {
      voxel = previouslyLoadedVoxels[num - 1];
   }
   return voxel;
}

/**
 * Set the last auto load anatomy volume indices.
 */
void
BrainSetAutoLoaderFile::setLastAutoLoadAnatomyVoxelIndices(const VoxelIJK& voxel) {
   if (this->autoLoadReplaceLastFileFlag) {
      previouslyLoadedVoxels.clear();
   }
   previouslyLoadedVoxels.push_back(voxel);
}


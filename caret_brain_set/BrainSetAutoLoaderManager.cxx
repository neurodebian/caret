
#include <iostream>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfaceOverlay.h"
#include "BrainSet.h"
#include "BrainSetAutoLoaderManager.h"
#include "BrainSetAutoLoaderFileFunctionalVolume.h"
#include "BrainSetAutoLoaderFileMetric.h"
#include "BrainSetAutoLoaderFileMetricByNode.h"
#include "BrainSetAutoLoaderFilePaintCluster.h"
#include "DebugControl.h"
#include "MetricFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainSetAutoLoaderManager::BrainSetAutoLoaderManager(BrainSet* bs)
{
   this->brainSet = bs;
   for (int i = 0; i < NUMBER_OF_METRIC_AUTO_LOADERS; i++) {
      metricAutoLoaders[i] = new BrainSetAutoLoaderFileMetric(bs, i);
      allFileAutoLoaders.push_back(metricAutoLoaders[i]);
   }
   for (int i = 0; i < NUMBER_OF_METRIC_NODE_AUTO_LOADERS; i++) {
      metricNodeAutoLoaders[i] = new BrainSetAutoLoaderFileMetricByNode(bs, i);
      allFileAutoLoaders.push_back(metricNodeAutoLoaders[i]);
   }
   for (int i = 0; i < NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS; i++) {
      functionalVolumeAutoLoaders[i] = new BrainSetAutoLoaderFileFunctionalVolume(bs, i);
      allFileAutoLoaders.push_back(functionalVolumeAutoLoaders[i]);
   }
   for (int i = 0; i < NUMBER_OF_CLUSTER_AUTO_LOADERS; i++) {
      clusterAutoLoaders[i] = new BrainSetAutoLoaderFilePaintCluster(bs, i);
      allFileAutoLoaders.push_back(clusterAutoLoaders[i]);
   }
}

/**
 * destructor.
 */
BrainSetAutoLoaderManager::~BrainSetAutoLoaderManager()
{
}

/**
 * reinitialize all display settings.
 */
void
BrainSetAutoLoaderManager::reset()
{
   for (unsigned int i = 0; i < allFileAutoLoaders.size(); i++) {
      allFileAutoLoaders[i]->reset();
   }
}

/**
 * update any selections due to changes with loaded data files.
 */
void
BrainSetAutoLoaderManager::update()
{
   for (unsigned int i = 0; i < allFileAutoLoaders.size(); i++) {
      allFileAutoLoaders[i]->update();
   }
}

/**
 * apply a scene (set display settings).
 */
void
BrainSetAutoLoaderManager::showScene(const SceneFile::Scene& scene,
                              QString& errorMessage)
{
   for (unsigned int i = 0; i < allFileAutoLoaders.size(); i++) {
      allFileAutoLoaders[i]->showScene(scene, errorMessage);
   }
}

/**
 * create a scene (read display settings).
 */
void
BrainSetAutoLoaderManager::saveScene(SceneFile::Scene& scene,
                              const bool onlyIfSelectedFlag,
                              QString& errorMessageOut)

{
   for (unsigned int i = 0; i < allFileAutoLoaders.size(); i++) {
      allFileAutoLoaders[i]->saveScene(scene, onlyIfSelectedFlag, errorMessageOut);
   }
}

/**
 * get a metric autoloader.
 */
BrainSetAutoLoaderFileMetric*
BrainSetAutoLoaderManager::getMetricAutoLoader(const int indx)
{
   return this->metricAutoLoaders[indx];
}

/**
 * get a metric node autoloader.
 */
BrainSetAutoLoaderFileMetricByNode*
BrainSetAutoLoaderManager::getMetricNodeAutoLoader(const int indx)
{
   return this->metricNodeAutoLoaders[indx];
}

/**
 * get a functional volume autoloader.
 */
BrainSetAutoLoaderFileFunctionalVolume*
BrainSetAutoLoaderManager::getFunctionalVolumeAutoLoader(const int indx)
{
   return this->functionalVolumeAutoLoaders[indx];
}

/**
 * Get a cluster auto loader.
 */
BrainSetAutoLoaderFilePaintCluster*
BrainSetAutoLoaderManager::getClusterAutoLoader(const int indx)
{
   return this->clusterAutoLoaders[indx];
}

/**
 * see if any auto loaders are selected.
 */
bool
BrainSetAutoLoaderManager::getAnyAutoLoaderSelected()
{
   for (unsigned int i = 0; i < allFileAutoLoaders.size(); i++) {
      if (allFileAutoLoaders[i]->getAutoLoadEnabled()) {
         return true;
      }
   }
   return false;
}

/**
 * Process autoloading for a voxel.
 */
QString
BrainSetAutoLoaderManager::processAutoLoading(const float xyz[3])
{
   QString errorMessage = "";

   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS; i++) {
      const QString indexString = QString::number(i + 1);

      //
      // Auto load metric
      //
      //int modelIndex = this->getDisplayedBrainModelIndex();
      BrainSetAutoLoaderFileMetric* alm = this->getMetricAutoLoader(i);

      //const int modelIndex = bs->getBrainModelIndex(alm->getAutoLoadMetricDisplaySurface());
      if (alm->getAutoLoadEnabled()) {
          VoxelIJK voxelIJK;

          //
          // See if volume selected
          //
          VolumeFile* vf = alm->getAutoLoadAnatomyVolumeFile();
          if (vf != NULL) {
             int ijk[3];
             float pcoords[3];
             if (vf->convertCoordinatesToVoxelIJK(xyz, ijk, pcoords)) {
                const QString msg = alm->loadFileForVoxel(VoxelIJK(ijk));
                if (msg.isEmpty() == false) {
                   errorMessage += msg;
                }
                else {
                   voxelIJK.setIJK(ijk);
                }
             }
          }
          else {
             errorMessage +=
                 "Auto Load Metric " + indexString + " selected but no Anatomical Volume selected.\n";
          }
          alm->setLastAutoLoadAnatomyVoxelIndices(voxelIJK);
      }
   }

   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS; i++) {
      const QString indexString = QString::number(i + 1);

      //
      // Auto load functional volume
      //
      BrainSetAutoLoaderFileFunctionalVolume* alf = this->getFunctionalVolumeAutoLoader(i);

      if (alf->getAutoLoadEnabled()) {
          VoxelIJK voxelIJK;

          //
          // See if volume selected
          //
          VolumeFile* vf = alf->getAutoLoadAnatomyVolumeFile();
          if (vf != NULL) {
             int ijk[3];
             float pcoords[3];
             if (vf->convertCoordinatesToVoxelIJK(xyz, ijk, pcoords)) {
                const QString msg = alf->loadFileForVoxel(VoxelIJK(ijk));
                if (msg.isEmpty() == false) {
                   errorMessage += msg;
                }
                else {
                   voxelIJK.setIJK(ijk);
                }
             }
          }
          else {
             errorMessage +=
                 "Auto Load functional volume " + indexString + " selected but no Anatomical Volume selected.\n";
          }
          alf->setLastAutoLoadAnatomyVoxelIndices(voxelIJK);
      }
   }

/*
   for (int i = 0; i < NUMBER_OF_CLUSTER_AUTO_LOADERS; i++) {
      BrainSetAutoLoaderFilePaintCluster* alc = this->getClusterAutoLoader(i);
      if (alc->getAutoLoadEnabled()) {
         int loadedNodeNumber = -1;
         const QString msg = alc->loadFileForNode(nodeNumber);
         if (msg.isEmpty() == false) {
            errorMessage += msg;
         }
         else {
            loadedNodeNumber = nodeNumber;
         }
         alc->setLastAutoLoadNodeNumber(loadedNodeNumber);
      }
   }
*/

   return errorMessage;

}

/**
 * Process autoloading for a node.
 */
QString
BrainSetAutoLoaderManager::processAutoLoading(const int nodeNumber)
{
   QString errorMessage = "";

   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_METRIC_NODE_AUTO_LOADERS; i++) {
      const QString indexString = QString::number(i + 1);

      //
      // Auto load metric
      //
      BrainSetAutoLoaderFileMetricByNode* alm = this->getMetricNodeAutoLoader(i);

      if (alm->getAutoLoadEnabled()) {
         VoxelIJK voxelIJK;

         if (nodeNumber >= 0) {
             errorMessage += alm->loadFileForNode(nodeNumber);
         }
         else {
            errorMessage += "No node selected for autoloading metric by node.";
         }
      }
   }

   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS; i++) {
      const QString indexString = QString::number(i + 1);
      
      //
      // Auto load metric
      //
      //int modelIndex = this->getDisplayedBrainModelIndex();
      BrainSetAutoLoaderFileMetric* alm = this->getMetricAutoLoader(i);

      //const int modelIndex = bs->getBrainModelIndex(alm->getAutoLoadMetricDisplaySurface());
      if (alm->getAutoLoadEnabled()) {
         VoxelIJK voxelIJK;

         if (nodeNumber >= 0) {
            BrainModelSurface* intersectionSurface =
                    alm->getAutoLoadVolumeIntersectionSurface();
            if (intersectionSurface != NULL) {
                //
                // Get the node's position
                //
                CoordinateFile* cf = intersectionSurface->getCoordinateFile();
                float xyz[3];
                cf->getCoordinate(nodeNumber, xyz);

                //
                // See if volume selected
                //
                VolumeFile* vf = alm->getAutoLoadAnatomyVolumeFile();
                if (vf != NULL) {
                   int ijk[3];
                   float pcoords[3];
                   if (vf->convertCoordinatesToVoxelIJK(xyz, ijk, pcoords)) {
                      const QString msg = alm->loadFileForVoxel(VoxelIJK(ijk));
                      if (msg.isEmpty() == false) {
                         errorMessage += msg;
                      }
                      else {
                         voxelIJK.setIJK(ijk);
                      }
                   }
                }
                else {
                   errorMessage +=
                       "Auto Load Metric " + indexString + " selected but no Anatomical Volume selected.\n";
                }
            }
            else {
               errorMessage +=
                   "Auto Load Metric " + indexString + " selected but no intersection Surface selected.\n";
            }

         }
         else {
            errorMessage += "No node selected for autoloading metric.";
         }

         alm->setLastAutoLoadAnatomyVoxelIndices(voxelIJK);
      }
   }

   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS; i++) {
      const QString indexString = QString::number(i + 1);
      
      //
      // Auto load functional volume
      //
      BrainSetAutoLoaderFileFunctionalVolume* alf = this->getFunctionalVolumeAutoLoader(i);

      if (alf->getAutoLoadEnabled()) {
         VoxelIJK voxelIJK;

         if (nodeNumber >= 0) {
            BrainModelSurface* intersectionSurface =
                    alf->getAutoLoadVolumeIntersectionSurface();
            if (intersectionSurface != NULL) {
                //
                // Get the node's position
                //
                CoordinateFile* cf = intersectionSurface->getCoordinateFile();
                float xyz[3];
                cf->getCoordinate(nodeNumber, xyz);

                //
                // See if volume selected
                //
                VolumeFile* vf = alf->getAutoLoadAnatomyVolumeFile();
                if (vf != NULL) {
                   int ijk[3];
                   float pcoords[3];
                   if (vf->convertCoordinatesToVoxelIJK(xyz, ijk, pcoords)) {
                      const QString msg = alf->loadFileForVoxel(VoxelIJK(ijk));
                      if (msg.isEmpty() == false) {
                         errorMessage += msg;
                      }
                      else {
                         voxelIJK.setIJK(ijk);
                      }
                   }
                }
                else {
                   errorMessage +=
                       "Auto Load functional volume " + indexString + " selected but no Anatomical Volume selected.\n";
                }
            }
            else {
               errorMessage +=
                   "Auto Load functional volume " + indexString + " selected but no intersection Surface selected.\n";
            }
         }
         else {
            errorMessage += "No node selected for autoloading metric.";
         }

         alf->setLastAutoLoadAnatomyVoxelIndices(voxelIJK);
      }
   }

   for (int i = 0; i < NUMBER_OF_CLUSTER_AUTO_LOADERS; i++) {
      BrainSetAutoLoaderFilePaintCluster* alc = this->getClusterAutoLoader(i);
      if (alc->getAutoLoadEnabled()) {
         int loadedNodeNumber = -1;
         const QString msg = alc->loadFileForNode(nodeNumber);
         if (msg.isEmpty() == false) {
            errorMessage += msg;
         }
         else {
            loadedNodeNumber = nodeNumber;
         }
         alc->setLastAutoLoadNodeNumber(loadedNodeNumber);
      }
   }
   return errorMessage;
}

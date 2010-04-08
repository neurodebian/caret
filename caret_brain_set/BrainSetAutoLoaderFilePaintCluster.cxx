#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QStringList>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfaceOverlay.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "BrainSetAutoLoaderFilePaintCluster.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsVolume.h"
#include "DebugControl.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "StringUtilities.h"
#include "VolumeFile.h"
#include "VoxelIJK.h"

/**
 *  constructor.
 */
BrainSetAutoLoaderFilePaintCluster::BrainSetAutoLoaderFilePaintCluster(BrainSet* bsIn,
                                                      const int autoLoaderIndexIn)
   : BrainSetAutoLoaderFile(bsIn, autoLoaderIndexIn)
{
   reset();
}

/**
 *  destructor.
 */
BrainSetAutoLoaderFilePaintCluster::~BrainSetAutoLoaderFilePaintCluster()
{

}

/**
 *  reinitialize all display settings.
 */
void
BrainSetAutoLoaderFilePaintCluster::reset()
{
   BrainSetAutoLoaderFile::reset();

   autoLoadDisplaySurface = NULL;
   autoLoadClusterLastFunctionalVolume = NULL;
   autoLoadClusterLastMetricColumnNumber = -1;
   previouslyLoadedNodeNumbers.clear();
   paintColumnNumber = -1;
}

/**
 *  update any selections due to changes with loaded data files.
 */
void
BrainSetAutoLoaderFilePaintCluster::update()
{
   BrainSetAutoLoaderFile::update();
   //validate volume, surface, etc

   //brainSet->getVolumeAnatomyFileByName(const QString& filename);
   //brainSet->getBrainModelSurfaceByCoordinateFileName(const QString& filename);

}

/**
 *  apply a scene (set display settings).
 */
void
BrainSetAutoLoaderFilePaintCluster::showScene(const SceneFile::Scene& scene,
                                           QString& /*errorMessage*/)
{
   reset();

   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName().startsWith("BrainSetAutoLoaderFilePaintCluster")) {
         std::vector<QString> tokens;
         StringUtilities::token(sc->getName(), ":", tokens);
         int indx = -1;
         if (tokens.size() > 1) {
            indx = tokens[1].toInt();
         }

         if (indx == this->autoLoaderIndex) {
            const int num = sc->getNumberOfSceneInfo();
            //
            // parent class data
            //
            showSceneHelper(*sc);

            for (int i = 0; i < num; i++) {
               const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
               const QString infoName = si->getName();

               if (infoName == "autoLoadDisplaySurface") {
                  autoLoadDisplaySurface =
                     brainSet->getBrainModelSurfaceWithCoordinateFileName(si->getValueAsString());
               }
               else if (infoName == "previouslyLoadedNodeNumbers") {
                  previouslyLoadedNodeNumbers.push_back(si->getValueAsInt());
               }
               else if (infoName == "paintColumnNumber") {
                  PaintFile* pf = brainSet->getPaintFile();
                  paintColumnNumber = pf->getColumnWithName(si->getValueAsString());
               }
            }
         }
      }
   }

   //
   // Autoload any previously loaded voxels
   //
   int num = getNumberOfPreviouslyLoadedNodeNumbers();
   for (int i = 0; i < num; i++) {
      if (this->autoLoadEnabledFlag) {
         this->loadFileForNode(this->getPreviouslyLoadedNodeNumber(i));
      }
   }
}

/**
 *  create a scene (read display settings).
 */
void
BrainSetAutoLoaderFilePaintCluster::saveScene(SceneFile::Scene& scene,
                                           const bool /*onlyIfSelectedFlag*/,
                                           QString& /*errorMessage*/)
{
   //MetricFile* mf = brainSet->getMetricFile();

   SceneFile::SceneClass sc("BrainSetAutoLoaderFilePaintCluster:"
                            + QString::number(this->autoLoaderIndex));

   for (unsigned int i = 0; i < previouslyLoadedNodeNumbers.size(); i++) {
      sc.addSceneInfo(SceneFile::SceneInfo("previouslyLoadedNodeNumbers",
                                           previouslyLoadedNodeNumbers[i]));
   }
   PaintFile* pf = brainSet->getPaintFile();
   if ((paintColumnNumber >= 0) && (paintColumnNumber < pf->getNumberOfColumns())) {
      sc.addSceneInfo(SceneFile::SceneInfo("paintColumnNumber",
                                           pf->getColumnName(paintColumnNumber)));
   }

   if (autoLoadDisplaySurface != NULL) {
      sc.addSceneInfo(SceneFile::SceneInfo("autoLoadDisplaySurface",
                                           autoLoadDisplaySurface->getCoordinateFile()->getFileName()));
   }
   saveSceneHelper(sc);
   // DO NOT add to scene
   //sc.addSceneInfo(SceneFile::SceneInfo("autoLoadMetricLastColumnNumber",
   //                                     autoLoadMetricLastColumnNumber));

   scene.addSceneClass(sc);

}

/**
 * Auto load the metric file for the specified node (return error message).
 */
QString
BrainSetAutoLoaderFilePaintCluster::loadFileForNode(const int nodeNumber)
{
    QString errorMessage = "";

    if ((nodeNumber < 0) ||
        (nodeNumber >= brainSet->getNumberOfNodes())) {
       return "";
    }

    //
    // Clear node highlighting
    //
    brainSet->clearNodeHighlightSymbols();
    
    //
    // Remove previously loaded metric column
    //
    MetricFile* mf = brainSet->getMetricFile();
    int lastCol = this->getAutoLoadLastMetricColumnNumber();
    if (this->getAutoLoadReplaceLastFileEnabled() &&
        (lastCol >= 0) &&
        (lastCol < mf->getNumberOfColumns())) {
       mf->removeColumn(lastCol);
       brainSet->getDisplaySettingsMetric()->update();
       this->setAutoLoadLastMetricColumnNumber(-1);
       lastCol = -1;
    }

    //
    // Remove previously loaded functional volume
    //
    DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
    VolumeFile* lastVolume = this->getAutoLoadLastFunctionalVolume();
    if (this->getAutoLoadReplaceLastFileEnabled() &&
        (lastVolume != NULL)) {
       brainSet->deleteVolumeFile(lastVolume);
       this->setAutoLoadLastFunctionalVolume(NULL);
       dsv->update();
       lastVolume = NULL;
    }

    if (DebugControl::getDebugOn()) {
       std::cout << "Cluster Auto Load Node: "
                 << nodeNumber << std::endl;
    }
    const QString metricDirectoryName =
       this->getAutoLoadDirectoryName();
    const QString functionalVolumeDirectoryName =
       this->getAutoLoadSecondaryDirectoryName();
    int loadedMetricColumn = -1;
    VolumeFile* loadedFunctionalVolume = NULL;

    if (metricDirectoryName.isEmpty()
        && functionalVolumeDirectoryName.isEmpty()) {
       errorMessage += "Both metric and functional volume directory names are empty.";
    }
    bool metricDirectoryNameValid = false;
    if (metricDirectoryName.isEmpty() == false) {
       if (QFile::exists(metricDirectoryName) == false) {
          errorMessage +=
            "Auto Load Cluster selected but metric directory (" + metricDirectoryName + ") is invalid.\n";
       }
       else {
          metricDirectoryNameValid = true;
       }
    }
    bool functionalVolumeDirectoryNameValidFlag = false;
    if (functionalVolumeDirectoryName.isEmpty() == false) {
       if (QFile::exists(functionalVolumeDirectoryName) == false) {
          errorMessage +=
           "Auto Load Cluster selected but functional volume directory (" + functionalVolumeDirectoryName + ") is invalid.\n";
       }
       else {
          functionalVolumeDirectoryNameValidFlag = true;
       }
    }
    PaintFile* pf = brainSet->getPaintFile();
    const int paintIndex = pf->getPaint(nodeNumber, paintColumnNumber);
    if (paintIndex >= 0) {
       const QString paintName = pf->getPaintNameFromIndex(paintIndex);
       const int paintNameNumber = paintName.toInt();
       //std::cout << "Paint Name: " << paintName.toAscii().constData() << std::endl;

       const QString reMetricString(".*_(.+)"
                     + SpecFile::getMetricFileExtension());
       QRegExp metricRE(reMetricString, Qt::CaseInsensitive);

       const QString reFunctionalVolumeString(".*(\\d+)"
                     + SpecFile::getNiftiGzipVolumeFileExtension());
       QRegExp functionalVolumeRE(reFunctionalVolumeString, Qt::CaseInsensitive);

       QString metricFileNameToLoad = "";
       if (metricDirectoryNameValid) {
          QDir metricDir(metricDirectoryName);
          QStringList metricFileList = metricDir.entryList(QDir::Files);
          for (int ni = 0; ni < metricFileList.size(); ni++) {
             const QString fileName = metricFileList.at(ni);
             if (metricRE.exactMatch(fileName)) {
                QStringList matches = metricRE.capturedTexts();
                if (matches.size() >= 2) {
                   //std::cout << "Filename: " << fileName.toAscii().constData() << std::endl;
                   //std::cout << "Match 0: " << matches.at(0).toAscii().constData() << std::endl;
                   //std::cout << "Match 1: " << matches.at(1).toAscii().constData() << std::endl;

                   //
                   // Note: 1st capture is the entire string
                   //
                   //const int filePaintNumber = matches.at(1).toInt();
                   //if (paintNameNumber == filePaintNumber) {
                   QString theMatch = matches.at(1);
                   if (paintName == theMatch) {
                      const QString fullPath =
                         metricDirectoryName + "/" + fileName;
                      metricFileNameToLoad = fullPath;
                      break;
                   }
                }
             }
          }
       }

       QString functionalVolumeFileNameToLoad = "";
       if (functionalVolumeDirectoryNameValidFlag) {
          QDir functionalVolumeDir(functionalVolumeDirectoryName);
          QStringList functionalVolumeFileList = functionalVolumeDir.entryList(QDir::Files);
          for (int ni = 0; ni < functionalVolumeFileList.size(); ni++) {
             const QString fileName = functionalVolumeFileList.at(ni);
             if (functionalVolumeRE.exactMatch(fileName)) {
                QStringList matches = functionalVolumeRE.capturedTexts();
                if (matches.size() >= 2) {
                   //
                   // Note: 1st capture is the entire string
                   //
                   const int filePaintNumber = matches.at(1).toInt();
                   if (paintNameNumber == filePaintNumber) {
                      const QString fullPath =
                         functionalVolumeDirectoryName + "/" + fileName;
                      functionalVolumeFileNameToLoad = fullPath;
                      break;
                   }
                }
             }
          }
       }

       if (metricFileNameToLoad.isEmpty() == false) {
          if (DebugControl::getDebugOn()) {
             std::cout << "Load: "
                       << metricFileNameToLoad.toAscii().constData()
                       << std::endl;
          }
          try {
             brainSet->readMetricFile(metricFileNameToLoad,
                                true,
                                false);
             const int col = brainSet->getMetricFile()->getNumberOfColumns() - 1;

             BrainModelSurfaceOverlay* metricOverlay = NULL;
             int overlayIndex = 3 - this->autoLoaderIndex;
             if ((overlayIndex >= 0) &&
                 (overlayIndex < brainSet->getNumberOfSurfaceOverlays())) {
                metricOverlay = brainSet->getSurfaceOverlay(overlayIndex);
             }

             if (metricOverlay != NULL) {
                BrainModelSurface* displaySurface = this->getAutoLoadDisplaySurface();
                if (displaySurface != NULL) {
                   const int surfaceIndex = brainSet->getBrainModelIndex(displaySurface);
                   metricOverlay->setOverlay(surfaceIndex, BrainModelSurfaceOverlay::OVERLAY_METRIC);
                   metricOverlay->setDisplayColumnSelected(surfaceIndex, col);
                   metricOverlay->setThresholdColumnSelected(surfaceIndex, col);
                }
             }
             loadedMetricColumn = col;

             int numNodes = brainSet->getNumberOfNodes();
             for (int m = 0; m < numNodes; m++) {
                brainSet->getNodeAttributes(m)->setHighlighting(
                           BrainSetNodeAttribute::HIGHLIGHT_NODE_NONE);
                if (pf->getPaint(m, paintColumnNumber) == paintIndex) {
                   if (m == nodeNumber) {
                      //
                      // DO NOT highlight selected node as the identification
                      // code will do this.
                      //
                      //brainSet->getNodeAttributes(m)->setHighlighting(
                      //   BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
                   }
                   else {
                      brainSet->getNodeAttributes(m)->setHighlighting(
                         BrainSetNodeAttribute::HIGHLIGHT_NODE_REMOTE);
                   }
                }
             }
          }
          catch (FileException e) {
             errorMessage += (e.whatQString() + "\n");
          }
       }
       else if (metricDirectoryNameValid) {
          errorMessage += ("No metric file for cluster node "
                          + QString::number(nodeNumber)
                          + " for paint name: "
                          + paintName
                          + "\n");
       }

       if (functionalVolumeFileNameToLoad.isEmpty() == false) {
          if (DebugControl::getDebugOn()) {
             std::cout << "Load: "
                       << functionalVolumeFileNameToLoad.toAscii().constData()
                       << std::endl;
          }
          try {
             brainSet->readVolumeFile(functionalVolumeFileNameToLoad,
                                      VolumeFile::VOLUME_TYPE_FUNCTIONAL,
                                      true,
                                      false);

             const int volumeIndex = brainSet->getNumberOfVolumeFunctionalFiles() - 1;
             if (volumeIndex >= 0) {
                dsv->setSelectedFunctionalVolumeView(volumeIndex);
                dsv->setSelectedFunctionalVolumeThreshold(volumeIndex);
                BrainModelVolumeVoxelColoring* vvc = brainSet->getVoxelColoring();
                vvc->setPrimaryOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL);
                loadedFunctionalVolume = brainSet->getVolumeFunctionalFile(volumeIndex);
             }
          }
          catch (FileException e) {
             errorMessage += (e.whatQString() + "\n");
          }
       }
       else if (functionalVolumeDirectoryNameValidFlag) {
          errorMessage += ("No functional volume file for cluster node "
                          + QString::number(nodeNumber) + "\n");
       }
    }
    else {
       errorMessage += ("No clusters for node with invalid paint index.");
    }

    this->setAutoLoadLastMetricColumnNumber(loadedMetricColumn);
    this->setAutoLoadLastFunctionalVolume(loadedFunctionalVolume);

    brainSet->getDisplaySettingsMetric()->update();
    brainSet->getNodeColoring()->assignColors();
    brainSet->clearAllDisplayLists();

    return errorMessage;
}

/**
 * Get the last auto load node number.
 */
int
BrainSetAutoLoaderFilePaintCluster::getLastAutoLoadNodeNumber() const
{
   const int indx = getNumberOfPreviouslyLoadedNodeNumbers() - 1;
   if (indx >= 0) {
      return getPreviouslyLoadedNodeNumber(indx);
   }
   return -1;
}

/**
 * Set the last auto load node number.
 */
void
BrainSetAutoLoaderFilePaintCluster::setLastAutoLoadNodeNumber(const int nodeNumber)
{
   if (this->autoLoadReplaceLastFileFlag) {
      previouslyLoadedNodeNumbers.clear();
   }
   previouslyLoadedNodeNumbers.push_back(nodeNumber);

}

/**
 * Get the selected paint column number.
 */
int
BrainSetAutoLoaderFilePaintCluster::getPaintColumnNumber()
{
   PaintFile* pf = brainSet->getPaintFile();
   const int numColumns = pf->getNumberOfColumns();
   if (paintColumnNumber >= numColumns) {
      paintColumnNumber = numColumns -1;
   }
   if (paintColumnNumber < 0) {
      if (numColumns > 0) {
         paintColumnNumber = 0;
      }
   }
   return paintColumnNumber;
}

/**
 * Set the selected paint column number.
 */
void
BrainSetAutoLoaderFilePaintCluster::setPaintColumnNumber(const int columnNumber)
{
   paintColumnNumber = columnNumber;
}


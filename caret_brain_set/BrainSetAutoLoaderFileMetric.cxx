
#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QStringList>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfaceOverlay.h"
#include "BrainSet.h"
#include "BrainSetAutoLoaderFileMetric.h"
#include "DebugControl.h"
#include "DisplaySettingsMetric.h"
#include "MetricFile.h"
#include "StringUtilities.h"
#include "VolumeFile.h"
#include "VoxelIJK.h"

/**
 *  constructor.
 */
BrainSetAutoLoaderFileMetric::BrainSetAutoLoaderFileMetric(BrainSet* bsIn,
                                                      const int autoLoaderIndexIn)
   : BrainSetAutoLoaderFile(bsIn, autoLoaderIndexIn)
{
   reset();
}

/**
 *  destructor.
 */
BrainSetAutoLoaderFileMetric::~BrainSetAutoLoaderFileMetric()
{

}

/**
 *  reinitialize all display settings.
 */
void
BrainSetAutoLoaderFileMetric::reset()
{
   BrainSetAutoLoaderFile::reset();

   autoLoadDisplaySurface = NULL;
   autoLoadMetricLastColumnNumber = -1;
}

/**
 *  update any selections due to changes with loaded data files.
 */
void
BrainSetAutoLoaderFileMetric::update()
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
BrainSetAutoLoaderFileMetric::showScene(const SceneFile::Scene& scene,
                                           QString& /*errorMessage*/)
{
   reset();

   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName().startsWith("BrainSetAutoLoaderFileMetric")) {
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
            }
         }
      }
   }

   //
   // Autoload any previously loaded voxels
   //
   int num = getNumberOfPreviouslyLoadedVoxels();
   for (int i = 0; i < num; i++) {
      if (this->autoLoadEnabledFlag) {
         this->loadFileForVoxel(this->getPreviouslyLoadedVoxel(i));
      }
   }
}

/**
 *  create a scene (read display settings).
 */
void
BrainSetAutoLoaderFileMetric::saveScene(SceneFile::Scene& scene,
                                           const bool /*onlyIfSelectedFlag*/,
                                           QString& /*errorMessage*/)
{
   //MetricFile* mf = brainSet->getMetricFile();

   SceneFile::SceneClass sc("BrainSetAutoLoaderFileMetric:"
                            + QString::number(this->autoLoaderIndex));

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
 * Auto load the metric file for the specified voxel (return error message).
 */
QString
BrainSetAutoLoaderFileMetric::loadFileForVoxel(const VoxelIJK& voxel)
{
    QString errorMessage = "";
    int ijk[3];
    voxel.getIJK(ijk);

    MetricFile* mf = brainSet->getMetricFile();
    int lastCol = this->getAutoLoadLastMetricColumnNumber();
    if (this->getAutoLoadReplaceLastFileEnabled() &&
        (lastCol >= 0) &&
        (lastCol < mf->getNumberOfColumns())) {
       mf->removeColumn(lastCol);
       this->setAutoLoadLastMetricColumnNumber(-1);
       lastCol = -1;
    }
    
    if (DebugControl::getDebugOn()) {
       std::cout << "Metric Auto Load Voxel IJK: "
                 << ijk[0] << " "
                 << ijk[1] << " "
                 << ijk[2] << std::endl;
    }

    int autoLoadColumnNumber = -1;
    const QString directoryName =
       this->getAutoLoadDirectoryName();
    if (QFile::exists(directoryName) == false) {
       errorMessage +=
         "Auto Load Metric selected but directory (" + directoryName + ") is invalid.\n";
    }
    else {
       const QString reString("(.*_)(\\d+)_(\\d+)_(\\d+)"
                     + SpecFile::getMetricFileExtension());
       QRegExp re(reString, Qt::CaseInsensitive);

       QString metricFileNameToLoad = "";
       QDir dir(directoryName);
       QStringList fileList = dir.entryList(QDir::Files);
       int fileCounter = 0;
       bool firstMetricFound = false;
       for (int ni = 0; ni < fileList.size(); ni++) {
          const QString fileName = fileList.at(ni);
          if (re.exactMatch(fileName)) {
             QStringList matches = re.capturedTexts();
             if (matches.size() >= 5) {
                //
                // Note: zero'th capture is the entire string
                //
                const int vi = matches.at(2).toInt();
                const int vj = matches.at(3).toInt();
                const int vk = matches.at(4).toInt();
                if ((vi == ijk[0]) &&
                    (vj == ijk[1]) &&
                    (vk == ijk[2])) {
                   const QString fullPath =
                      directoryName + "/" + fileName;
                   metricFileNameToLoad = fullPath;
                   break;
                }

                if (firstMetricFound == false) {
                   QString guessedFileName =
                           directoryName
                           + "/"
                           + matches.at(1)
                           + QString::number(ijk[0])
                           + "_"
                           + QString::number(ijk[1])
                           + "_"
                           + QString::number(ijk[2])
                           + SpecFile::getMetricFileExtension();
                   if (QFile::exists(guessedFileName)) {
                      metricFileNameToLoad = guessedFileName;
                      break;
                   }
                   firstMetricFound = true;
                }
             }
          }
          fileCounter++;
       }
       //std::cout << "Searched " << fileCounter << " files." << std::endl;
       if (metricFileNameToLoad.isEmpty() == false) {
          if (DebugControl::getDebugOn()) {
             std::cout << "Load: "
                       << metricFileNameToLoad.toAscii().constData()
                       << std::endl;
          }
          try {
             int col = -1;
             int lastCol = this->getAutoLoadLastMetricColumnNumber();
             if (this->getAutoLoadReplaceLastFileEnabled() &&
                 (lastCol >= 0) &&
                 (lastCol < brainSet->getMetricFile()->getNumberOfColumns())) {
                std::vector<int> columnDestination;
                columnDestination.push_back(lastCol);
                std::vector<QString> columnNames;
                columnNames.push_back("");
                brainSet->readMetricFile(metricFileNameToLoad,
                                   columnDestination,
                                   columnNames,
                                   AbstractFile::FILE_COMMENT_MODE_LEAVE_AS_IS,
                                   false);
                col = lastCol;
             }
             else {
                brainSet->readMetricFile(metricFileNameToLoad,
                                   true,
                                   false);
                col = brainSet->getMetricFile()->getNumberOfColumns() - 1;
             }
             BrainModelSurfaceOverlay* metricOverlay = NULL;
             int overlayIndex = 3 - this->autoLoaderIndex;
             if ((overlayIndex >= 0) &&
                 (overlayIndex < brainSet->getNumberOfSurfaceOverlays())) {
                metricOverlay = brainSet->getSurfaceOverlay(overlayIndex);
             }

             if (metricOverlay != NULL) {
                BrainModelSurface* displaySurface = this->getAutoLoadMetricDisplaySurface();
                if (displaySurface != NULL) {
                   const int surfaceIndex = brainSet->getBrainModelIndex(displaySurface);
                   metricOverlay->setOverlay(surfaceIndex, BrainModelSurfaceOverlay::OVERLAY_METRIC);
                   metricOverlay->setDisplayColumnSelected(surfaceIndex, col);
                   metricOverlay->setThresholdColumnSelected(surfaceIndex, col);
                }
             }
             autoLoadColumnNumber = col;
          }
          catch (FileException e) {
             errorMessage += (e.whatQString() + "\n");
          }
       }
       else {
          errorMessage = ("No metric file for voxel ("
                          + QString::number(ijk[0]) + ", "
                          + QString::number(ijk[1]) + ", "
                          + QString::number(ijk[2]) + ")\n");
       }
    }

    if (autoLoadColumnNumber >= 0) {
        this->setAutoLoadLastMetricColumnNumber(autoLoadColumnNumber);
    }
    brainSet->getDisplaySettingsMetric()->update();
    brainSet->getNodeColoring()->assignColors();
    brainSet->clearAllDisplayLists();

    return errorMessage;
}

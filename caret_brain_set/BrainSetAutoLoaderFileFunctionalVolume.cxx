
#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QStringList>

#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "BrainSetAutoLoaderFileFunctionalVolume.h"
#include "DebugControl.h"
#include "DisplaySettingsVolume.h"
#include "StringUtilities.h"
#include "VolumeFile.h"
#include "VoxelIJK.h"

/**
 *  constructor.
 */
BrainSetAutoLoaderFileFunctionalVolume::BrainSetAutoLoaderFileFunctionalVolume(BrainSet* bsIn,
                                                      const int autoLoaderIndexIn)
   : BrainSetAutoLoaderFile(bsIn, autoLoaderIndexIn)
{
   reset();
}

/**
 *  destructor.
 */
BrainSetAutoLoaderFileFunctionalVolume::~BrainSetAutoLoaderFileFunctionalVolume()
{

}

/**
 *  reinitialize all display settings.
 */
void
BrainSetAutoLoaderFileFunctionalVolume::reset()
{
   BrainSetAutoLoaderFile::reset();

   autoLoadMetricLastFunctionalVolume = NULL;
}

/**
 *  update any selections due to changes with loaded data files.
 */
void
BrainSetAutoLoaderFileFunctionalVolume::update()
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
BrainSetAutoLoaderFileFunctionalVolume::showScene(const SceneFile::Scene& scene,
                                           QString& /*errorMessage*/)
{
   reset();

   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName().startsWith("BrainSetAutoLoaderFileFunctionalVolume")) {
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
BrainSetAutoLoaderFileFunctionalVolume::saveScene(SceneFile::Scene& scene,
                                           const bool /*onlyIfSelectedFlag*/,
                                           QString& /*errorMessage*/)
{
   SceneFile::SceneClass sc("BrainSetAutoLoaderFileFunctionalVolume:"
                            + QString::number(this->autoLoaderIndex));

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
BrainSetAutoLoaderFileFunctionalVolume::loadFileForVoxel(const VoxelIJK& voxel)
{
    DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
    VolumeFile* lastVolume = this->getAutoLoadLastFunctionalVolume();
    if (this->getAutoLoadReplaceLastFileEnabled() &&
        (lastVolume != NULL)) {
       brainSet->deleteVolumeFile(lastVolume);
       this->setAutoLoadLastFunctionalVolume(NULL);
       dsv->update();
       lastVolume = NULL;
    }

    QString errorMessage = "";
    int ijk[3];
    voxel.getIJK(ijk);
    VolumeFile* autoLoadLastVolumeFile = NULL;

    if (DebugControl::getDebugOn()) {
       std::cout << "Functional Volume Auto Load Voxel IJK: "
                 << ijk[0] << " "
                 << ijk[1] << " "
                 << ijk[2] << std::endl;
    }
    const QString directoryName =
       this->getAutoLoadDirectoryName();
    if (QFile::exists(directoryName) == false) {
       errorMessage +=
         "Auto Load Functional Volume selected but directory (" + directoryName + ") is invalid.\n";
    }
    else {
       const QString reString(".*_(\\d+)_(\\d+)_(\\d+)"
                     + SpecFile::getNiftiGzipVolumeFileExtension());
       QRegExp re(reString, Qt::CaseInsensitive);

       QString functionalVolumeFileNameToLoad = "";
       QDir dir(directoryName);
       QStringList fileList = dir.entryList(QDir::Files);
       for (int ni = 0; ni < fileList.size(); ni++) {
          const QString fileName = fileList.at(ni);
          if (re.exactMatch(fileName)) {
             QStringList matches = re.capturedTexts();
             if (matches.size() >= 4) {
                //
                // Note: 1st capture is the entire string
                //
                const int vi = matches.at(1).toInt();
                const int vj = matches.at(2).toInt();
                const int vk = matches.at(3).toInt();
                if ((vi == ijk[0]) &&
                    (vj == ijk[1]) &&
                    (vk == ijk[2])) {
                   const QString fullPath =
                      directoryName + "/" + fileName;
                   functionalVolumeFileNameToLoad = fullPath;
                   break;
                }
             }
          }
       }
       if (functionalVolumeFileNameToLoad.isEmpty() == false) {
          if (DebugControl::getDebugOn()) {
             std::cout << "Load: "
                       << functionalVolumeFileNameToLoad.toAscii().constData()
                       << std::endl;
          }
          try {
             VolumeFile* lastVolume = this->getAutoLoadLastFunctionalVolume();
             if (this->getAutoLoadReplaceLastFileEnabled() &&
                 (lastVolume != NULL)) {
                brainSet->deleteVolumeFile(lastVolume);
             }

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
                autoLoadLastVolumeFile = brainSet->getVolumeFunctionalFile(volumeIndex);
             }
          }
          catch (FileException e) {
             errorMessage += (e.whatQString() + "\n");
          }
       }
       else {
          errorMessage = ("No functional volume file for voxel ("
                          + QString::number(ijk[0]) + ", "
                          + QString::number(ijk[1]) + ", "
                          + QString::number(ijk[2]) + ")\n");
       }
    }

    dsv->update();
    brainSet->clearAllDisplayLists();
    this->setAutoLoadLastFunctionalVolume(autoLoadLastVolumeFile);

    return errorMessage;
}


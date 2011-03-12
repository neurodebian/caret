
#include <iostream>

#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QStringList>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfaceOverlay.h"
#include "BrainSet.h"
#include "BrainSetAutoLoaderFileMetricByNode.h"
#include "DebugControl.h"
#include "DisplaySettingsMetric.h"
#include "MetricFile.h"
#include "StringUtilities.h"
#include "VolumeFile.h"
#include "VoxelIJK.h"
/**
 *  constructor
 */
BrainSetAutoLoaderFileMetricByNode::BrainSetAutoLoaderFileMetricByNode(BrainSet* bsIn,
                                          const int autoLoaderIndexIn)
   : BrainSetAutoLoaderFileMetric(bsIn, autoLoaderIndexIn)
{

}

/**
 * destructor.
 */
BrainSetAutoLoaderFileMetricByNode::~BrainSetAutoLoaderFileMetricByNode()
{

}

/**
 * auto load the metric file for the specified node number (return error message).
 */
QString
BrainSetAutoLoaderFileMetricByNode::loadFileForNode(const int nodeNumber)
{
   QString errorMessage = "";
   MetricFile* mf = brainSet->getMetricFile();
   int lastCol = this->getAutoLoadLastMetricColumnNumber();
   if (this->getAutoLoadReplaceLastFileEnabled() &&
       (lastCol >= 0) &&
       (lastCol < mf->getNumberOfColumns())) {
      mf->removeColumn(lastCol);
      this->setAutoLoadLastMetricColumnNumber(-1);
      lastCol = -1;
   }

   int autoLoadColumnNumber = -1;
   const QString directoryName =
      this->getAutoLoadDirectoryName();
   if (QFile::exists(directoryName) == false) {
      errorMessage +=
        "Auto Load Metric selected but directory (" + directoryName + ") is invalid.\n";
   }
   else {
      const QString fileNamePrefix = QString::number(nodeNumber);

      const QString reString("(\\d+)_(.*)."
                    + SpecFile::getMetricFileExtension());
      QRegExp re(reString, Qt::CaseInsensitive);

      QString metricFileNameToLoad = "";
      QDir dir(directoryName);
      QStringList fileList = dir.entryList(QDir::Files);
      int fileCounter = 0;
      for (int ni = 0; ni < fileList.size(); ni++) {
         const QString fileName = fileList.at(ni);

         if (re.exactMatch(fileName)) {
            QStringList matches = re.capturedTexts();
            if (matches.size() >= 3) {
               //
               // Note: zero'th capture is the entire string
               //
               const int fileNodeNumber = matches.at(1).toInt();
               if (nodeNumber == fileNodeNumber) {
                  const QString fullPath =
                     directoryName + "/" + fileName;
                  metricFileNameToLoad = fullPath;
                  break;
               }
            }
         }
         fileCounter++;
      }

      //std::cout << "Searched " << fileCounter << " files." << std::endl;
      QString columnName = "";
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
            columnName = brainSet->getMetricFile()->getColumnName(col);
         }
         catch (FileException e) {
            errorMessage += (e.whatQString() + "\n");
         }
      }
      else {
         errorMessage = ("No metric file for node "
                         + QString::number(nodeNumber) + "\n");
      }

      //if (DebugControl::getDebugOn()) {
          std::cout << "Node Number: "
                    << nodeNumber
                    << ", file: "
                    << metricFileNameToLoad.toAscii().constData()
                    << ", column-name: "
                    << columnName.toAscii().constData()
                    << std::endl;
      //}

   }

   if (autoLoadColumnNumber >= 0) {
       this->setAutoLoadLastMetricColumnNumber(autoLoadColumnNumber);
   }
   brainSet->getDisplaySettingsMetric()->update();
   brainSet->getNodeColoring()->assignColors();
   brainSet->clearAllDisplayLists();

   return errorMessage;
}

/**
 * is auto loading metric file by node supported for this auto loader
 */
bool
BrainSetAutoLoaderFileMetricByNode::isLoadFileForNodeSupported() const
{
    return true;
}

/**
 * Auto load the metric file for the specified voxel (return error message).
 */
QString
BrainSetAutoLoaderFileMetricByNode::loadFileForVoxel(const VoxelIJK& voxel)
{
    return "Auto-Loading files by voxel not suppored.";
}

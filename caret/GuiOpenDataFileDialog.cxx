/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/


#include <iostream>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BrainSet.h"
#include "CellColorFile.h"
#include "ContourCellColorFile.h"
#include "DebugControl.h"
#include "DisplaySettingsBorders.h"
#include "FileUtilities.h"
#include "FociColorFile.h"
#include "GeodesicDistanceFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiDataFileCommentDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiLoadNodeAttributeFileColumnSelectionDialog.h"
#define _GUI_OPEN_DATA_FILE_DIALOG_MAIN_
#include "GuiOpenDataFileDialog.h"
#undef _GUI_OPEN_DATA_FILE_DIALOG_MAIN_
#include "GuiSpecFileCreationDialog.h"
#include "GuiToolBar.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "PreferencesFile.h"
#include "QtRadioButtonSelectionDialog.h"
#include "QtUtilities.h"
#include "StudyMetaDataFile.h"
#include "SurfaceShapeFile.h"
#include "SurfaceVectorFile.h"

#include "global_variables.h"

static const QString GENERIC_BORDER("GENERIC_BORDER");
static const QString GENERIC_COORDINATE("GENERIC_COORDINATE");
static const QString GENERIC_TOPOLOGY("GENERIC_TOPOLOGY");

/**
 * Constructor
 */
GuiOpenDataFileDialog::GuiOpenDataFileDialog(QWidget* parent,
                                             const QString& useThisFileFilter)
           : Q3FileDialog(parent, "Open Data File", false)
{
   savedPositionAndSizeValid = false;
   
   appendCheckButton = NULL;
   updateSpecFileCheckButton = NULL;
   
   setWindowTitle("Open Data File");
   setMode(Q3FileDialog::ExistingFile);
   setDir(QDir::currentPath());

   
   QObject::connect(this, SIGNAL(filterSelected(const QString&)),
                    this, SLOT(filterSelectedSlot(const QString&)));
                    
   QObject::connect(this, SIGNAL(fileHighlighted(const QString&)),
                    this, SLOT(fileHighlightedSlot(const QString&)));
   
   //
   // View/Edit comment button
   //
   commentButton = new QPushButton("View/Edit Comment...");
   QObject::connect(commentButton, SIGNAL(clicked()),
                    this, SLOT(commentButtonSlot()));
   
   //
   // Add caret widgets
   //
   QWidget* caretWidget = createCaretUniqueSection();
   addWidgets(NULL, caretWidget, commentButton);
   
   setFilter(areaColorFileFilter);
   addFilter(arealEstimationFileFilter);
   addFilter(borderGenericFileFilter);
   addFilter(borderVolumeFileFilter);
   addFilter(borderColorFileFilter);
   addFilter(borderProjectionFileFilter);
   addFilter(cellFileFilter);
   addFilter(cellColorFileFilter);
   addFilter(cellProjectionFileFilter);
   addFilter(cellVolumeFileFilter);
   addFilter(cocomacFileFilter);
   addFilter(contourFileFilter);
   addFilter(contourCellFileFilter);
   addFilter(contourCellColorFileFilter);
   addFilter(coordinateGenericFileFilter);
   addFilter(cutsFileFilter);
   addFilter(deformationFieldFileFilter);
   addFilter(deformationMapFileFilter);
   addFilter(fociFileFilter);
   addFilter(fociColorFileFilter);
   addFilter(fociProjectionFileFilter);
   addFilter(fociVolumeFileFilter);
   addFilter(geodesicDistanceFileFilter);
   addFilter(latitudeLongitudeFileFilter);
   addFilter(metricFileFilter);
   addFilter(paintFileFilter);
   addFilter(paletteFileFilter);
   addFilter(paramsFileFilter);
   addFilter(probAtlasFileFilter);
   addFilter(rgbPaintFileFilter);
   addFilter(sceneFileFilter);
   addFilter(sectionFileFilter);
   addFilter(studyMetaDataFileFilter);
   addFilter(surfaceShapeFileFilter);
   addFilter(surfaceVectorFileFilter);
   addFilter(topographyFileFilter);
   addFilter(topologyGenericFileFilter);
   addFilter(transformationMatrixFileFilter);
   addFilter(transformationDataFileFilter);
   addFilter(vocabularyFileFilter);
   addFilter(volumeAnatomyFileFilter);
   addFilter(volumeFunctionalFileFilter);
   addFilter(volumePaintFileFilter);
   addFilter(volumeProbAtlasFileFilter);
   addFilter(volumeRgbFileFilter);
   addFilter(volumeSegmentationFileFilter);
   addFilter(volumeVectorFileFilter);
   addFilter(vtkModelFileFilter);
   addFilter(wustlRegionFileFilter);
   
   if (useThisFileFilter.isEmpty() == false) {
      setSelectedFilter(useThisFileFilter);
      filterSelectedSlot(useThisFileFilter);
   }
   else {
      setSelectedFilter(0);
      filterSelectedSlot(0);
   }
   
   QtUtilities::setMaximumHeightToNinetyPercentOfScreenHeight(this);
}

/**
 * Destructor
 */
GuiOpenDataFileDialog::~GuiOpenDataFileDialog()
{
}

/**
 * show the dialog.
 */
void 
GuiOpenDataFileDialog::show()
{
   setDir(QDir::currentPath());
   updateDialog();

   Q3FileDialog::show();

   if (savedPositionAndSizeValid) {
      setGeometry(savedPositionAndSize);
      adjustSize();
   }
}
      
/**
 * update the dialog.
 */
void 
GuiOpenDataFileDialog::updateDialog()
{
   //
   // Previous directories 
   //
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   pf->getRecentDataFileDirectories(previousDirectories);
   previousDirectoryComboBox->clear();
   for (int i = 0; i < static_cast<int>(previousDirectories.size()); i++) {
      QString dirName(previousDirectories[i]);
      const int dirNameLength = static_cast<int>(dirName.length());
      if (dirNameLength > 50) {
         QString s("...");
         s.append(dirName.mid(dirNameLength - 50));
         dirName = s;
      }
      previousDirectoryComboBox->addItem(dirName);
   }
   
}

/**
 * create the caret unique section.
 */
QWidget* 
GuiOpenDataFileDialog::createCaretUniqueSection()
{
   //
   // Previous directories combo box
   //
   previousDirectoryComboBox = new QComboBox;
   QObject::connect(previousDirectoryComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotPreviousDirectory(int)));
                    
   //
   // Previous directories group box
   //
   QGroupBox* prevDirGroupBox = new QGroupBox("Previous Directories");
   QVBoxLayout* prefDirLayout = new QVBoxLayout(prevDirGroupBox);
   prefDirLayout->addWidget(previousDirectoryComboBox);
   
   //
   // Append to current file check box
   //
   appendCheckButton = new QCheckBox("Append to Current File");
   appendCheckButton->setChecked(true);
   
   //
   // Add to spec file check box
   //
   updateSpecFileCheckButton = new QCheckBox("Add to Specification File");
   updateSpecFileCheckButton->setChecked(true);
   
   //
   // SPM flip check box
   //
   spmRightOnLeftVolumeCheckBox = new QCheckBox("Volume is an SPM2 Volume with Right on Left");
                                                
   //
   // Open options group box
   //
   QGroupBox* openOptionsGroupBox = new QGroupBox("Open Options");
   QVBoxLayout* openOptionsLayout = new QVBoxLayout(openOptionsGroupBox);
   openOptionsLayout->addWidget(appendCheckButton);
   openOptionsLayout->addWidget(updateSpecFileCheckButton);
   openOptionsLayout->addWidget(spmRightOnLeftVolumeCheckBox);
   
   //
   // Widget for caret items
   //
   QWidget* caretWidget = new QWidget;
   QVBoxLayout* caretWidgetLayout = new QVBoxLayout(caretWidget);
   caretWidgetLayout->addWidget(prevDirGroupBox);
   caretWidgetLayout->addWidget(openOptionsGroupBox);
   return caretWidget;
}

/**
 * called when a previous directory is selected.
 */
void 
GuiOpenDataFileDialog::slotPreviousDirectory(int item)
{
   if ((item >= 0) && (item < static_cast<int>(previousDirectories.size()))) {
      setDir(previousDirectories[item]);
      setFilter(selectedFilter());
   }
}      

/**
 * Slot called when a file filter is selected
 */
void
GuiOpenDataFileDialog::filterSelectedSlot(const QString& filterName)
{
   commentButton->setEnabled(true);
   if (filterName == cocomacFileFilter) {
      commentButton->setEnabled(false);
   }
   
   appendCheckButton->setEnabled(true);
   if ((filterName == metricFileFilter) ||
       (filterName == paintFileFilter) ||
       (filterName == surfaceShapeFileFilter) ||
       (filterName == surfaceVectorFileFilter)) {
      appendCheckButton->setEnabled(false);
   }
   
   spmRightOnLeftVolumeCheckBox->setEnabled((filterName == volumeAnatomyFileFilter) ||
                                            (filterName == volumeFunctionalFileFilter) ||
                                            (filterName == volumePaintFileFilter) ||
                                            (filterName == volumeProbAtlasFileFilter) ||
                                            (filterName == volumeRgbFileFilter) ||
                                            (filterName == volumeSegmentationFileFilter) ||
                                            (filterName == volumeVectorFileFilter));
}

/**
 * Called when a file is highlighted in the dialog.
 */
void
GuiOpenDataFileDialog::fileHighlightedSlot(const QString& fileName)
{
   highlightedFileName = fileName;
}

/**
 * Called when view comment button is pressed.
 */
void
GuiOpenDataFileDialog::commentButtonSlot()
{
   if (highlightedFileName.isEmpty() == false) {
      const QString filterName = selectedFilter();
      const bool volumeFileFlag =
         (filterName == GuiDataFileDialog::volumeAnatomyFileFilter) ||
         (filterName == GuiDataFileDialog::volumeFunctionalFileFilter) ||
         (filterName == GuiDataFileDialog::volumePaintFileFilter) ||
         (filterName == GuiDataFileDialog::volumeProbAtlasFileFilter) ||
         (filterName == GuiDataFileDialog::volumeRgbFileFilter) ||
         (filterName == GuiDataFileDialog::volumeSegmentationFileFilter) ||
         (filterName == GuiDataFileDialog::volumeVectorFileFilter);

      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(this,
                                                                    highlightedFileName,
                                                                    volumeFileFlag);
      dfcd->show();
   }
}

/**
 * status of append selection
 */
bool
GuiOpenDataFileDialog::getAppendSelection() const
{
   if (appendCheckButton != NULL) {
      return appendCheckButton->isChecked();
   }
   return false;
}

/**
 * status of update spec file selection
 */
bool
GuiOpenDataFileDialog::getUpdateSpecFileSelection() const
{
   if (updateSpecFileCheckButton != NULL) {
      return updateSpecFileCheckButton->isChecked();
   }
   return false;
}

/**
 * Overrides QFileDialog::done(int).  This allows us to examine the file selected by the
 * user and to save the data file.
 */
void
GuiOpenDataFileDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QString name(selectedFile());
      
      const QString filterName = selectedFilter();
      
      const bool append = getAppendSelection();
      const bool update = getUpdateSpecFileSelection();

      //
      // Update previous directories
      //
      PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
      pf->addToRecentDataFileDirectories(FileUtilities::dirname(name), true);
      
      //
      // Allow user to set the spec file name if not already set
      //
      if (update && theMainWindow->getBrainSet()->getSpecFileName().isEmpty()) {
         const int result = GuiMessageBox::question(this, "Spec File", 
                                      "Would you like to create a Spec File ?",
                                      "Yes", "No", "Cancel");
         if (result == 0) {
            //
            // Create spec file dialog will set the directory and create the spec file
            //
            GuiSpecFileCreationDialog sfcd(this);
            sfcd.exec();
            PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
            pf->addToRecentSpecFiles(sfcd.getCreatedSpecFileName(), true);
/*
            QFileDialog specFileNameDialog(this, "specFileNameDialog", true);
            specFileNameDialog.setWindowTitle("Set Spec File Name");
            specFileNameDialog.setFilter("Spec File (*.spec)");
            specFileNameDialog.setFileMode(QFileDialog::AnyFile);
            if (specFileNameDialog.exec() == QFileDialog::Accepted) {
               QString specName(specFileNameDialog.selectedFile()
               if (FileUtilities::filenameExtension(specName) != "spec") {
                  specName.append(".spec");
               }
               theMainWindow->getBrainSet()->setSpecFileName(specName);
               QDir::setCurrent(FileUtilities::dirname(specName));
            }
*/
         }
         else if (result == 1) {
         }
         else if (result == 2) {
            return;
         }
      }
      
      //
      // If file is in another directory, allow user to copy it to current directory
      //
      if (update && (theMainWindow->getBrainSet()->getSpecFileName().isEmpty() == false)) {
         bool copyCheck = true;
         
         //
         // Volume files have both header and data files
         //
         QString volumeDataName;
         if ((filterName == GuiDataFileDialog::volumeAnatomyFileFilter) ||
             (filterName == GuiDataFileDialog::volumeFunctionalFileFilter) ||
             (filterName == GuiDataFileDialog::volumePaintFileFilter) ||
             (filterName == GuiDataFileDialog::volumeProbAtlasFileFilter) ||
             (filterName == GuiDataFileDialog::volumeRgbFileFilter) ||
             (filterName == GuiDataFileDialog::volumeSegmentationFileFilter) ||
             (filterName == GuiDataFileDialog::volumeVectorFileFilter)) {
            copyCheck = false;
            
            QString ext = FileUtilities::filenameExtension(name);
            QString dataExt;
            if (ext == "hdr") {
               dataExt = "img";
            }
            else if (ext == "HEAD") {
               dataExt = "BRIK";
            }
            else if (ext == "ifh") {
               dataExt = "img";
            }
            if (dataExt.isEmpty() == false) {
               volumeDataName.append(FileUtilities::filenameWithoutExtension(name));
               volumeDataName.append(".");
               volumeDataName.append(dataExt);
               if (DebugControl::getDebugOn()) {
                  std::cout << "Volume data name: " << volumeDataName.toAscii().constData() << std::endl;
               }
               if (QFile::exists(volumeDataName)) {
                  copyCheck = true;
               }
               else {
                  volumeDataName.append(".gz");
                  if (QFile::exists(volumeDataName)) {
                     copyCheck = true;
                  }
               }
            }
         }
         if (copyCheck) {
            const QString filePath(FileUtilities::dirname(name));
            QString specPath(FileUtilities::dirname(theMainWindow->getBrainSet()->getSpecFileName()));
            if (specPath == ".") {
               specPath = QDir::currentPath();
            }
            if (DebugControl::getDebugOn()) {
               std::cout << "file path: " << filePath.toAscii().constData() << std::endl;
               std::cout << "spec path: " << specPath.toAscii().constData() << std::endl;
            }
            if (filePath != specPath) {
               const int result = GuiMessageBox::question(this, "Copy File",
                                       "Would you like to copy the "
                                       "file to the current directory.",
                                       "Yes", "No", "Cancel");
               if (result == 0) {
                  if (FileUtilities::copyFile(name,
                                             FileUtilities::basename(name))) {
                     QString msg("Error copying:\n");
                     msg.append(name);
                     msg.append("\nDoes the file exist ?");
                     GuiMessageBox::critical(this, "ERROR", msg, "OK");
                     return;
                  }
                  else if (volumeDataName.isEmpty() == false) {
                     if (FileUtilities::copyFile(volumeDataName,
                                          FileUtilities::basename(volumeDataName))) {
                        QString msg("Error copying:\n");
                        msg.append(volumeDataName);
                        msg.append("\nDoes the file exist ?");
                        GuiMessageBox::critical(this, "ERROR", msg, "OK");
                        return;
                     }
                  }
                  name = FileUtilities::basename(name);
               }
               else if (result == 1) {
               }
               else if (result == 2) {
                  return;
               }
            }
         }
      }

      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      theMainWindow->getBrainSet()->setDisplaySplashImage(false);
      
      QString msg;      
      bool error = false;
      bool warning = false;
      
      if (filterName == GuiDataFileDialog::areaColorFileFilter) {
         error = openDataFile(this, SpecFile::areaColorFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::arealEstimationFileFilter) {
         error = openDataFile(this, SpecFile::arealEstimationFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderRawFileFilter) {
         error = openDataFile(this, SpecFile::rawBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderFiducialFileFilter) {
         error = openDataFile(this, SpecFile::fiducialBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderInflatedFileFilter) {
         error = openDataFile(this, SpecFile::inflatedBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderVeryInflatedFileFilter) {
         error = openDataFile(this, SpecFile::veryInflatedBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderSphericalFileFilter) {
         error = openDataFile(this, SpecFile::sphericalBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderEllipsoidalFileFilter) {
         error = openDataFile(this, SpecFile::ellipsoidBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderCompressedFileFilter) {
         error = openDataFile(this, SpecFile::compressedBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderFlatFileFilter) {
         error = openDataFile(this, SpecFile::flatBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderFlatLobarFileFilter) {
         error = openDataFile(this, SpecFile::lobarFlatBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderHullFileFilter) {
         error = openDataFile(this, SpecFile::hullBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderUnknownFileFilter) {
         error = openDataFile(this, SpecFile::unknownBorderFileMatchTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderVolumeFileFilter) {
         error = openDataFile(this, SpecFile::volumeBorderFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderGenericFileFilter) {
         error = openDataFile(this, GENERIC_BORDER, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderColorFileFilter) {
         error = openDataFile(this, SpecFile::borderColorFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::borderProjectionFileFilter) {
         error = openDataFile(this, SpecFile::borderProjectionFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::cellFileFilter) {
         error = openDataFile(this, SpecFile::cellFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::cellColorFileFilter) {
         error = openDataFile(this, SpecFile::cellColorFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::cellProjectionFileFilter) {
         error = openDataFile(this, SpecFile::cellProjectionFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::cellVolumeFileFilter) {
         error = openDataFile(this, SpecFile::volumeCellFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::cocomacFileFilter) {
         error = openDataFile(this, SpecFile::cocomacConnectivityFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::contourFileFilter) {
         error = openDataFile(this, SpecFile::contourFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::contourCellFileFilter) {
         error = openDataFile(this, SpecFile::contourCellFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::contourCellColorFileFilter) {
         error = openDataFile(this, SpecFile::contourCellColorFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateRawFileFilter) {
         error = openDataFile(this, SpecFile::rawCoordFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateFiducialFileFilter) {
         error = openDataFile(this, SpecFile::fiducialCoordFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateInflatedFileFilter) {
         error = openDataFile(this, SpecFile::inflatedCoordFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateVeryInflatedFileFilter) {
         error = openDataFile(this, SpecFile::veryInflatedCoordFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateSphericalFileFilter) {
         error = openDataFile(this, SpecFile::sphericalCoordFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateEllipsoidalFileFilter) {
         error = openDataFile(this, SpecFile::ellipsoidCoordFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateCompressedFileFilter) {
         error = openDataFile(this, SpecFile::compressedCoordFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateFlatFileFilter) {
         error = openDataFile(this, SpecFile::flatCoordFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateFlatLobarFileFilter) {
         error = openDataFile(this, SpecFile::lobarFlatCoordFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateHullFileFilter) {
         error = openDataFile(this, SpecFile::hullCoordFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateUnknownFileFilter) {
         error = openDataFile(this, SpecFile::unknownCoordFileMatchTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::coordinateGenericFileFilter) {
         error = openDataFile(this, GENERIC_COORDINATE, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::cutsFileFilter) {
         error = openDataFile(this, SpecFile::cutsFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::deformationFieldFileFilter) {
         error = openDataFile(this, SpecFile::deformationFieldFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::deformationMapFileFilter) {
         error = openDataFile(this, SpecFile::deformationMapFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::fociFileFilter) {
         error = openDataFile(this, SpecFile::fociFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::fociColorFileFilter) {
         error = openDataFile(this, SpecFile::fociColorFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::fociProjectionFileFilter) {
         error = openDataFile(this, SpecFile::fociProjectionFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::fociVolumeFileFilter) {
         error = openDataFile(this, SpecFile::volumeFociFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::geodesicDistanceFileFilter) {
         error = openDataFile(this, SpecFile::geodesicDistanceFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::latitudeLongitudeFileFilter) {
         error = openDataFile(this, SpecFile::latLonFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::metricFileFilter) {
         error = openDataFile(this, SpecFile::metricFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::paintFileFilter) {
         error = openDataFile(this, SpecFile::paintFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::paletteFileFilter) {
         error = openDataFile(this, SpecFile::paletteFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::paramsFileFilter) {
         error = openDataFile(this, SpecFile::paramsFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::probAtlasFileFilter) {
         error = openDataFile(this, SpecFile::atlasFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::rgbPaintFileFilter) {
         error = openDataFile(this, SpecFile::rgbPaintFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::sceneFileFilter) {
         error = openDataFile(this, SpecFile::sceneFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::sectionFileFilter) {
         error = openDataFile(this, SpecFile::sectionFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::surfaceShapeFileFilter) {
         error = openDataFile(this, SpecFile::surfaceShapeFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::surfaceVectorFileFilter) {
         error = openDataFile(this, SpecFile::surfaceVectorFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::topographyFileFilter) {
         error = openDataFile(this, SpecFile::topographyFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::topologyClosedFileFilter) {
         error = openDataFile(this, SpecFile::closedTopoFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::topologyOpenFileFilter) {
         error = openDataFile(this, SpecFile::openTopoFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::topologyCutFileFilter) {
         error = openDataFile(this, SpecFile::cutTopoFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::topologyCutLobarFileFilter) {
         error = openDataFile(this, SpecFile::lobarCutTopoFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::topologyUnknownFileFilter) {
         error = openDataFile(this, SpecFile::unknownTopoFileMatchTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::topologyGenericFileFilter) {
         error = openDataFile(this, GENERIC_TOPOLOGY, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::volumeAnatomyFileFilter) {
         error = openDataFile(this, SpecFile::volumeAnatomyFileTag, name, append, update, msg, warning);
         if (spmRightOnLeftVolumeCheckBox->isChecked()) {
            const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles();
            if (volumeIndex > 0) {
               VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeAnatomyFile(volumeIndex - 1);
               vf->flip(VolumeFile::VOLUME_AXIS_X, false);
               vf->clearModified();
            }
         }
      }
      else if (filterName == GuiDataFileDialog::volumeFunctionalFileFilter) {
         error = openDataFile(this, SpecFile::volumeFunctionalFileTag, name, append, update, msg, warning);
         if (spmRightOnLeftVolumeCheckBox->isChecked()) {
            const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles();
            if (volumeIndex > 0) {
               VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeFunctionalFile(volumeIndex - 1);
               vf->flip(VolumeFile::VOLUME_AXIS_X, false);
               vf->clearModified();
            }
         }
      }
      else if (filterName == GuiDataFileDialog::volumePaintFileFilter) {
         error = openDataFile(this, SpecFile::volumePaintFileTag, name, append, update, msg, warning);
         if (spmRightOnLeftVolumeCheckBox->isChecked()) {
            const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles();
            if (volumeIndex > 0) {
               VolumeFile* vf = theMainWindow->getBrainSet()->getVolumePaintFile(volumeIndex - 1);
               vf->flip(VolumeFile::VOLUME_AXIS_X, false);
               vf->clearModified();
            }
         }
      }
      else if (filterName == GuiDataFileDialog::volumeProbAtlasFileFilter) {
         error = openDataFile(this, SpecFile::volumeProbAtlasFileTag, name, append, update, msg, warning);
         if (spmRightOnLeftVolumeCheckBox->isChecked()) {
            const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles();
            if (volumeIndex > 0) {
               VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeProbAtlasFile(volumeIndex - 1);
               vf->flip(VolumeFile::VOLUME_AXIS_X, false);
               vf->clearModified();
            }
         }
      }
      else if (filterName == GuiDataFileDialog::volumeRgbFileFilter) {
         error = openDataFile(this, SpecFile::volumeRgbFileTag, name, append, update, msg, warning);
         if (spmRightOnLeftVolumeCheckBox->isChecked()) {
            const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles();
            if (volumeIndex > 0) {
               VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeRgbFile(volumeIndex - 1);
               vf->flip(VolumeFile::VOLUME_AXIS_X, false);
               vf->clearModified();
            }
         }
      }
      else if (filterName == GuiDataFileDialog::volumeSegmentationFileFilter) {
         error = openDataFile(this, SpecFile::volumeSegmentationFileTag, name, append, update, msg, warning);
         if (spmRightOnLeftVolumeCheckBox->isChecked()) {
            const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles();
            if (volumeIndex > 0) {
               VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeSegmentationFile(volumeIndex - 1);
               vf->flip(VolumeFile::VOLUME_AXIS_X, false);
               vf->clearModified();
            }
         }
      }
      else if (filterName == GuiDataFileDialog::volumeVectorFileFilter) {
         error = openDataFile(this, SpecFile::volumeVectorFileTag, name, append, update, msg, warning);
         if (spmRightOnLeftVolumeCheckBox->isChecked()) {
            const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles();
            if (volumeIndex > 0) {
               VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeVectorFile(volumeIndex - 1);
               vf->flip(VolumeFile::VOLUME_AXIS_X, false);
               vf->clearModified();
            }
         }
      }
      else if (filterName == GuiDataFileDialog::studyMetaDataFileFilter) {
         error = openDataFile(this, SpecFile::studyMetaDataFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::transformationMatrixFileFilter) {
         error = openDataFile(this, SpecFile::transformationMatrixFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::transformationDataFileFilter) {
         error = openDataFile(this, SpecFile::transformationDataFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::vtkModelFileFilter) {
         error = openDataFile(this, SpecFile::vtkModelFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::vocabularyFileFilter) {
         error = openDataFile(this, SpecFile::vocabularyFileTag, name, append, update, msg, warning);
      }
      else if (filterName == GuiDataFileDialog::wustlRegionFileFilter) {
         error = openDataFile(this, SpecFile::wustlRegionFileTag, name, append, update, msg, warning);
      }
      else {
         std::cerr << "PROGRAM ERROR: unhandled file type at " << __LINE__
                  << " in " << __FILE__ << std::endl;
         QApplication::restoreOverrideCursor(); 
         return;
      }
      
      QApplication::restoreOverrideCursor(); 
      if (error) {
         GuiMessageBox::critical(this, "Error Reading File", msg, "OK");
         return;
      }
   }
      
   savedPositionAndSize = geometry();
   savedPositionAndSizeValid = true;

   Q3FileDialog::done(r);
}

/**
 * This static method provides one place to open data files and make the necessary
 * updates as a result.  Returns true if an error occurs.
 */
bool
GuiOpenDataFileDialog::openDataFile(QWidget* parentWidget, const QString specFileTag,
                      const QString& name, const bool append, const bool update,
                      QString& errorMessage, bool& relatedFileWarningFlag)
{
   errorMessage = "";
   relatedFileWarningFlag = false;
   
   const bool needAreaColors        = (theMainWindow->getBrainSet()->getAreaColorFile()->getNumberOfColors() <= 0);
   const bool needBorderColors      = (theMainWindow->getBrainSet()->getBorderColorFile()->getNumberOfColors() <= 0);
   const bool needCellColors        = (theMainWindow->getBrainSet()->getCellColorFile()->getNumberOfColors() <= 0);
   const bool needContourCellColors = (theMainWindow->getBrainSet()->getContourCellColorFile()->getNumberOfColors() <= 0);
   const bool needFociColors        = (theMainWindow->getBrainSet()->getFociColorFile()->getNumberOfColors() <= 0);

   bool areaColorWarning        = false;
   bool borderColorWarning      = false;
   bool cellColorWarning        = false;
   bool contourCellColorWarning = false;
   bool fociColorWarning        = false;
   
   bool bordersLoaded = false;
   
   //
   // See if file exists
   //
   QFileInfo fileInfo(name);
   if (fileInfo.exists() == false) {
      if (fileInfo.isReadable() == false) {
         errorMessage = "File ";
         errorMessage.append(name);
         errorMessage.append(" does not exist!");
         return true;
      }
   }
   else {
      if (fileInfo.isReadable() == false) {
         errorMessage.append("File ");
         errorMessage.append(name);
         errorMessage.append(" does not have read permission!");
         return true;
      }
   }
   
   GuiFilesModified fm;;
   
   try {
      if (specFileTag == SpecFile::areaColorFileTag) {
         theMainWindow->getBrainSet()->readAreaColorFile(name, append, update);
         fm.setAreaColorModified();
      }
      else if (specFileTag == SpecFile::arealEstimationFileTag) {
         theMainWindow->getBrainSet()->readArealEstimationFile(name, append, update);
         fm.setArealEstimationModified();;
         areaColorWarning = needAreaColors;
      }
      else if (specFileTag == SpecFile::rawBorderFileTag) {
         theMainWindow->getBrainSet()->readBorderFile(name, BrainModelSurface::SURFACE_TYPE_RAW, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag == SpecFile::fiducialBorderFileTag) {
         theMainWindow->getBrainSet()->readBorderFile(name, BrainModelSurface::SURFACE_TYPE_FIDUCIAL, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag == SpecFile::inflatedBorderFileTag) {
         theMainWindow->getBrainSet()->readBorderFile(name, BrainModelSurface::SURFACE_TYPE_INFLATED, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag == SpecFile::veryInflatedBorderFileTag) {
         theMainWindow->getBrainSet()->readBorderFile(name, BrainModelSurface::SURFACE_TYPE_VERY_INFLATED, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag == SpecFile::sphericalBorderFileTag) {
         theMainWindow->getBrainSet()->readBorderFile(name, BrainModelSurface::SURFACE_TYPE_SPHERICAL, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag == SpecFile::ellipsoidBorderFileTag) {
         theMainWindow->getBrainSet()->readBorderFile(name, BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag == SpecFile::compressedBorderFileTag) {
         theMainWindow->getBrainSet()->readBorderFile(name, BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag == SpecFile::flatBorderFileTag) {
         theMainWindow->getBrainSet()->readBorderFile(name, BrainModelSurface::SURFACE_TYPE_FLAT, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag == SpecFile::lobarFlatBorderFileTag) {
         theMainWindow->getBrainSet()->readBorderFile(name, BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag.indexOf(SpecFile::unknownBorderFileMatchTag) >= 0) {
         theMainWindow->getBrainSet()->readBorderFile(name, BrainModelSurface::SURFACE_TYPE_UNKNOWN, append, update);
         fm.setBorderModified();
         bordersLoaded = true;
         borderColorWarning = needBorderColors;
      }
      else if (specFileTag == SpecFile::volumeBorderFileTag) {
         theMainWindow->getBrainSet()->readVolumeBorderFile(name, append, update);
         fm.setBorderModified();
         bordersLoaded = true;
         borderColorWarning = needBorderColors;
      }
      else if (specFileTag == GENERIC_BORDER) {
         BorderFile bf;
         try {
            bf.readFileMetaDataOnly(name);
         }
         catch (FileException&) {
            throw FileException(name, "Unable to read border file to check type.");
         }
         BrainModelSurface::SURFACE_TYPES st = BrainModelSurface::SURFACE_TYPE_UNSPECIFIED;

/*
         AbstractFileHeaderUpdater afhu;
         afhu.readFile(name);
         const QString typeTag = afhu.getHeaderTag(AbstractFile::headerTagConfigurationID);
*/
         const QString typeTag = bf.getHeaderTag(AbstractFile::headerTagConfigurationID);
         if (typeTag.isEmpty() == false) {
            st = BrainModelSurface::getSurfaceTypeFromConfigurationID(typeTag);
         }
         
         QApplication::restoreOverrideCursor();
         if ((st == BrainModelSurface::SURFACE_TYPE_UNSPECIFIED) ||
             (st == BrainModelSurface::SURFACE_TYPE_UNKNOWN)) {
            std::vector<QString> labels;
            labels.push_back("RAW");
            labels.push_back("FIDUCIAL");
            labels.push_back("INFLATED");
            labels.push_back("VERY_INFLATED");
            labels.push_back("SPHERICAL");
            labels.push_back("ELLIPSOIDAL");
            labels.push_back("COMPRESSED_MEDIAL_WALL");
            labels.push_back("FLAT");
            labels.push_back("FLAT_LOBAR");
            labels.push_back("HULL");
            QApplication::beep();
            QtRadioButtonSelectionDialog rbsd(parentWidget,
                                              "Choose Border Type",
                                              "Choose the Type of Borders",
                                              labels,
                                              -1);
            if (rbsd.exec() == QDialog::Accepted) {
               st = static_cast<BrainModelSurface::SURFACE_TYPES>(rbsd.getSelectedItemIndex());
            }
            else {
               throw FileException(name, "You must select the type of border");
            }
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         theMainWindow->getBrainSet()->readBorderFile(name, st, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag == SpecFile::borderColorFileTag) {
         theMainWindow->getBrainSet()->readBorderColorFile(name, append, update);
         fm.setBorderColorModified();
      }
      else if (specFileTag == SpecFile::borderProjectionFileTag) {
         theMainWindow->getBrainSet()->readBorderProjectionFile(name, append, update);
         fm.setBorderModified();
         borderColorWarning = needBorderColors;
         bordersLoaded = true;
      }
      else if (specFileTag == SpecFile::cellFileTag) {
         theMainWindow->getBrainSet()->readCellFile(name, append, update);
         fm.setCellModified();
         cellColorWarning = needCellColors;
      }
      else if (specFileTag == SpecFile::cellColorFileTag) {
         theMainWindow->getBrainSet()->readCellColorFile(name, append, update);
         fm.setCellColorModified();
      }
      else if (specFileTag == SpecFile::cellProjectionFileTag) {
         theMainWindow->getBrainSet()->readCellProjectionFile(name, append, update);
         fm.setCellProjectionModified();
         cellColorWarning = needCellColors;
      }
      else if (specFileTag == SpecFile::volumeCellFileTag) {
         theMainWindow->getBrainSet()->readVolumeCellFile(name, append, update);
         fm.setCellModified();
         cellColorWarning = needCellColors;
      }
      else if (specFileTag == SpecFile::cocomacConnectivityFileTag) {
         theMainWindow->getBrainSet()->readCocomacConnectivityFile(name, append, update);
         fm.setCocomacModified();
      }
      else if (specFileTag == SpecFile::contourFileTag) {
         theMainWindow->getBrainSet()->readContourFile(name, append, update);
         fm.setContourModified();
      }
      else if (specFileTag == SpecFile::contourCellFileTag) {
         theMainWindow->getBrainSet()->readContourCellFile(name, append, update);
         fm.setContourCellModified();
         contourCellColorWarning = needContourCellColors;
      }
      else if (specFileTag == SpecFile::contourCellColorFileTag) {
         theMainWindow->getBrainSet()->readContourCellColorFile(name, append, update);
         fm.setContourCellModified();
      }
      else if (specFileTag == SpecFile::rawCoordFileTag) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_RAW, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag == SpecFile::fiducialCoordFileTag) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_FIDUCIAL, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag == SpecFile::inflatedCoordFileTag) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_INFLATED, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag == SpecFile::veryInflatedCoordFileTag) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_VERY_INFLATED, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag == SpecFile::sphericalCoordFileTag) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_SPHERICAL, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag == SpecFile::ellipsoidCoordFileTag) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag == SpecFile::compressedCoordFileTag) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag == SpecFile::flatCoordFileTag) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_FLAT, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag == SpecFile::lobarFlatCoordFileTag) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag == SpecFile::hullCoordFileTag) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_HULL, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag.indexOf(SpecFile::unknownCoordFileMatchTag) >= 0) {
         theMainWindow->getBrainSet()->readCoordinateFile(name, BrainModelSurface::SURFACE_TYPE_UNKNOWN, false, append, update);
         fm.setCoordinateModified();
      }
      else if (specFileTag == GENERIC_COORDINATE) {
         CoordinateFile cf;
         try {
            cf.readFileMetaDataOnly(name);
         }
         catch (FileException& e) {
            throw FileException(name, "Unable to open coord file to check type.");
         }

         BrainModelSurface::SURFACE_TYPES st = BrainModelSurface::SURFACE_TYPE_UNSPECIFIED;
/*
         AbstractFileHeaderUpdater afhu;
         afhu.readFile(name);
         const QString typeTag = afhu.getHeaderTag(AbstractFile::headerTagConfigurationID);
*/
         const QString typeTag = cf.getHeaderTag(AbstractFile::headerTagConfigurationID);
         if (typeTag.isEmpty() == false) {
            st = BrainModelSurface::getSurfaceTypeFromConfigurationID(typeTag);
         }
         
         if ((st == BrainModelSurface::SURFACE_TYPE_UNSPECIFIED) ||
             (st == BrainModelSurface::SURFACE_TYPE_UNKNOWN)) {
            std::vector<QString> labels;
            labels.push_back("RAW");
            labels.push_back("FIDUCIAL");
            labels.push_back("INFLATED");
            labels.push_back("VERY_INFLATED");
            labels.push_back("SPHERICAL");
            labels.push_back("ELLIPSOIDAL");
            labels.push_back("COMPRESSED_MEDIAL_WALL");
            labels.push_back("FLAT");
            labels.push_back("FLAT_LOBAR");
            labels.push_back("HULL");
            QApplication::restoreOverrideCursor();
            QApplication::beep();
            QtRadioButtonSelectionDialog rbsd(parentWidget,
                                              "Choose Coordinate Type",
                                              "Choose the Type of Coordinate File",
                                              labels,
                                              -1);
            if (rbsd.exec() == QDialog::Accepted) {
               st = static_cast<BrainModelSurface::SURFACE_TYPES>(rbsd.getSelectedItemIndex());
            }
            else {
               throw FileException(name, "You must select the type of coordinate file.");
            }
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         theMainWindow->getBrainSet()->readCoordinateFile(name, st, false, append, update);
         fm.setCoordinateModified();
         QApplication::restoreOverrideCursor();
      }
      else if (specFileTag == SpecFile::cutsFileTag) {
         theMainWindow->getBrainSet()->readCutsFile(name, append, update);
         fm.setCutModified();
      }
      else if (specFileTag == SpecFile::deformationFieldFileTag) {
         theMainWindow->getBrainSet()->readDeformationFieldFile(name, append, update);
         fm.setDeformationFieldModified();
      }
      else if (specFileTag == SpecFile::deformationMapFileTag) {
         theMainWindow->getBrainSet()->setDeformationMapFileName(name, update);
         fm.setDeformationMapModified();
      }
      else if (specFileTag == SpecFile::fociFileTag) {
         theMainWindow->getBrainSet()->readFociFile(name, append, update);
         fm.setFociModified();
         fociColorWarning = needFociColors;
      }
      else if (specFileTag == SpecFile::fociColorFileTag) {
         theMainWindow->getBrainSet()->readFociColorFile(name, append, update);
         fm.setFociColorModified();
      }
      else if (specFileTag == SpecFile::fociProjectionFileTag) {
         theMainWindow->getBrainSet()->readFociProjectionFile(name, append, update);
         fm.setFociProjectionModified();
         fociColorWarning = needFociColors;
      }
      else if (specFileTag == SpecFile::volumeFociFileTag) {
         theMainWindow->getBrainSet()->readVolumeFociFile(name, append, update);
         fm.setFociModified();
         fociColorWarning = needFociColors;
      }
      else if (specFileTag == SpecFile::geodesicDistanceFileTag) {
         GeodesicDistanceFile* gdf = theMainWindow->getBrainSet()->getGeodesicDistanceFile();
         GeodesicDistanceFile newFile;
         newFile.readFileMetaDataOnly(name);
         QApplication::restoreOverrideCursor();
         GuiLoadNodeAttributeFileColumnSelectionDialog fcsd(theMainWindow,
                                                      &newFile,
                                                      gdf);
         const int result = fcsd.exec(); 
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         if (result == QDialog::Accepted) {
            if (fcsd.getEraseAllExistingColumns()) {
               theMainWindow->getBrainSet()->clearGeodesicDistanceFile();
            }
            theMainWindow->getBrainSet()->readGeodesicDistanceFile(name, 
                                 fcsd.getDestinationColumns(), 
                                 fcsd.getNewFileColumnNames(),
                                 fcsd.getAppendFileCommentSelection(),
                                 update);
            fm.setGeodesicModified();
         }
      }
      else if (specFileTag == SpecFile::imageFileTag) {
         theMainWindow->getBrainSet()->readImageFile(name, 
                              append, 
                              update);
         fm.setImagesModified();
      }
      else if (specFileTag == SpecFile::latLonFileTag) {
         LatLonFile* llf = theMainWindow->getBrainSet()->getLatLonFile();
         LatLonFile newFile;
         newFile.readFileMetaDataOnly(name);
         QApplication::restoreOverrideCursor();
         GuiLoadNodeAttributeFileColumnSelectionDialog fcsd(theMainWindow,
                                                      &newFile,
                                                      llf);
         const int result = fcsd.exec(); 
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         if (result == QDialog::Accepted) {
            if (fcsd.getEraseAllExistingColumns()) {
               theMainWindow->getBrainSet()->clearLatLonFile();
            }
            theMainWindow->getBrainSet()->readLatLonFile(name, 
                                 fcsd.getDestinationColumns(), 
                                 fcsd.getNewFileColumnNames(),
                                 fcsd.getAppendFileCommentSelection(),
                                 update);
            fm.setLatLonModified();
         }
      }
      else if (specFileTag == SpecFile::metricFileTag) {
         MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
         MetricFile newFile;
         newFile.readFileMetaDataOnly(name);
         QApplication::restoreOverrideCursor();
         GuiLoadNodeAttributeFileColumnSelectionDialog fcsd(theMainWindow,
                                                      &newFile,
                                                      mf);
         const int result = fcsd.exec(); 
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         if (result == QDialog::Accepted) {
            if (fcsd.getEraseAllExistingColumns()) {
               theMainWindow->getBrainSet()->clearMetricFile();
            }
            theMainWindow->getBrainSet()->readMetricFile(name, 
                                 fcsd.getDestinationColumns(), 
                                 fcsd.getNewFileColumnNames(),
                                 fcsd.getAppendFileCommentSelection(),
                                 update);
            fm.setMetricModified();
         }
      }
      else if (specFileTag == SpecFile::paintFileTag) {
         PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
         PaintFile newFile;
         newFile.readFileMetaDataOnly(name);
         QApplication::restoreOverrideCursor();
         GuiLoadNodeAttributeFileColumnSelectionDialog fcsd(theMainWindow,
                                                      &newFile,
                                                      pf);
         const int result = fcsd.exec(); 
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         if (result == QDialog::Accepted) {
            if (fcsd.getEraseAllExistingColumns()) {
               theMainWindow->getBrainSet()->clearPaintFile();
            }
            theMainWindow->getBrainSet()->readPaintFile(name, 
                                 fcsd.getDestinationColumns(), 
                                 fcsd.getNewFileColumnNames(),
                                 fcsd.getAppendFileCommentSelection(),
                                 update);
            fm.setPaintModified();
         }
         areaColorWarning = needAreaColors;
      }
      else if (specFileTag == SpecFile::paletteFileTag) {
         theMainWindow->getBrainSet()->readPaletteFile(name, append, update);
         fm.setPaletteModified();
      }
      else if (specFileTag == SpecFile::paramsFileTag) {
         theMainWindow->getBrainSet()->readParamsFile(name, append, update);
         fm.setParameterModified();
      }
      else if (specFileTag == SpecFile::atlasFileTag) {
         theMainWindow->getBrainSet()->readProbabilisticAtlasFile(name, append, update);
         fm.setProbabilisticAtlasModified();
         areaColorWarning = needAreaColors;
      }
      else if (specFileTag == SpecFile::rgbPaintFileTag) {
         theMainWindow->getBrainSet()->readRgbPaintFile(name, append, update);
         fm.setRgbPaintModified();
      }
      else if (specFileTag == SpecFile::sceneFileTag) {
         theMainWindow->getBrainSet()->readSceneFile(name, append, update);
         fm.setSceneModified();
      }
      else if (specFileTag == SpecFile::sectionFileTag) {
         theMainWindow->getBrainSet()->readSectionFile(name, append, update);
         fm.setSectionModified();
      }
      else if (specFileTag == SpecFile::rawSurfaceFileTag) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_RAW, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::fiducialSurfaceFileTag) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_FIDUCIAL, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::inflatedSurfaceFileTag) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_INFLATED, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::veryInflatedSurfaceFileTag) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_VERY_INFLATED, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::sphericalSurfaceFileTag) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_SPHERICAL, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::ellipsoidSurfaceFileTag) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::compressedSurfaceFileTag) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::flatSurfaceFileTag) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_FLAT, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::lobarFlatSurfaceFileTag) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::hullSurfaceFileTag) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_HULL, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag.indexOf(SpecFile::unknownSurfaceFileMatchTag) >= 0) {
         theMainWindow->getBrainSet()->readSurfaceFile(name, BrainModelSurface::SURFACE_TYPE_UNKNOWN, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::studyMetaDataFileTag) {
         theMainWindow->getBrainSet()->readStudyMetaDataFile(name, append, update);
         fm.setStudyMetaDataModified();
      }
      else if (specFileTag == SpecFile::surfaceShapeFileTag) {
         SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
         SurfaceShapeFile newFile;
         newFile.readFileMetaDataOnly(name);
         QApplication::restoreOverrideCursor();
         GuiLoadNodeAttributeFileColumnSelectionDialog fcsd(theMainWindow,
                                                      &newFile,
                                                      ssf);
         const int result = fcsd.exec(); 
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         if (result == QDialog::Accepted) {
            if (fcsd.getEraseAllExistingColumns()) {
               theMainWindow->getBrainSet()->clearSurfaceShapeFile();
            }
            theMainWindow->getBrainSet()->readSurfaceShapeFile(name, 
                                 fcsd.getDestinationColumns(), 
                                 fcsd.getNewFileColumnNames(),
                                 fcsd.getAppendFileCommentSelection(),
                                 update);
            fm.setSurfaceShapeModified();
         }
      }
      else if (specFileTag == SpecFile::surfaceVectorFileTag) {
         SurfaceVectorFile* svf = theMainWindow->getBrainSet()->getSurfaceVectorFile();
         SurfaceVectorFile newFile;
         newFile.readFileMetaDataOnly(name);
         QApplication::restoreOverrideCursor();
         GuiLoadNodeAttributeFileColumnSelectionDialog fcsd(theMainWindow,
                                                      &newFile,
                                                      svf);
         const int result = fcsd.exec(); 
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         if (result == QDialog::Accepted) {
            if (fcsd.getEraseAllExistingColumns()) {
               theMainWindow->getBrainSet()->clearSurfaceVectorFile();
            }
            theMainWindow->getBrainSet()->readSurfaceVectorFile(name, 
                                 fcsd.getDestinationColumns(), 
                                 fcsd.getNewFileColumnNames(),
                                 fcsd.getAppendFileCommentSelection(),
                                 update);
            fm.setSurfaceVectorModified();
         }
      }
      else if (specFileTag == SpecFile::topographyFileTag) {
         theMainWindow->getBrainSet()->readTopographyFile(name, append, update);
         fm.setTopographyModified();
      }
      else if (specFileTag == SpecFile::closedTopoFileTag) {
         theMainWindow->getBrainSet()->readTopologyFile(name, TopologyFile::TOPOLOGY_TYPE_CLOSED, append, update);
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::openTopoFileTag) {
         theMainWindow->getBrainSet()->readTopologyFile(name, TopologyFile::TOPOLOGY_TYPE_OPEN, append, update);
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::cutTopoFileTag) {
         theMainWindow->getBrainSet()->readTopologyFile(name, TopologyFile::TOPOLOGY_TYPE_CUT, append, update);
         fm.setTopologyModified();
      }
      else if (specFileTag == SpecFile::lobarCutTopoFileTag) {
         theMainWindow->getBrainSet()->readTopologyFile(name, TopologyFile::TOPOLOGY_TYPE_LOBAR_CUT, append, update);
         fm.setTopologyModified();
      }
      else if (specFileTag.indexOf(SpecFile::unknownTopoFileMatchTag) >= 0) {
         theMainWindow->getBrainSet()->readTopologyFile(name, TopologyFile::TOPOLOGY_TYPE_UNKNOWN, append, update);
         fm.setTopologyModified();
      }
      else if (specFileTag == GENERIC_TOPOLOGY) {
         TopologyFile::TOPOLOGY_TYPES tt = TopologyFile::TOPOLOGY_TYPE_UNKNOWN;
         TopologyFile tf;
         try {
            tf.readFileMetaDataOnly(name);
         }
         catch (FileException& e) {
            throw FileException(name, "Unable to read topology file to check type.");
         }
/*
         AbstractFileHeaderUpdater afhu;
         afhu.readFile(name);
         const QString typeTag = afhu.getHeaderTag(AbstractFile::headerTagPerimeterID);
*/
         const QString typeTag = tf.getHeaderTag(AbstractFile::headerTagPerimeterID);
         if (typeTag.isEmpty() == false) {
            tt = TopologyFile::getTopologyTypeFromPerimeterID(typeTag);
         }
         
         if ((tt == TopologyFile::TOPOLOGY_TYPE_UNKNOWN) ||
             (tt == TopologyFile::TOPOLOGY_TYPE_UNSPECIFIED)) {
            std::vector<QString> labels;
            labels.push_back("CLOSED");
            labels.push_back("OPEN");
            labels.push_back("CUT");
            labels.push_back("LOBAR_CUT");
            QApplication::restoreOverrideCursor();
            QApplication::beep();
            QtRadioButtonSelectionDialog rbsd(parentWidget,
                                              "Choose Topology Type",
                                              "Choose the Type of Topology File",
                                              labels,
                                              -1);
            if (rbsd.exec() == QDialog::Accepted) {
               tt = static_cast<TopologyFile::TOPOLOGY_TYPES>(rbsd.getSelectedItemIndex());
            }
            else {
               throw FileException(name, "You must select the type of topology.");
            }
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         theMainWindow->getBrainSet()->readTopologyFile(name, tt, append, update);
         fm.setTopologyModified();
         QApplication::restoreOverrideCursor();
      }
      else if (specFileTag == SpecFile::transformationMatrixFileTag) {
         theMainWindow->getBrainSet()->readTransformationMatrixFile(name, append, update);
         fm.setTransformationMatrixModified();
      }
      else if (specFileTag == SpecFile::transformationDataFileTag) {
         theMainWindow->getBrainSet()->readTransformationDataFile(name, append, update);
         theMainWindow->getBrainSet()->assignTransformationDataFileColors();
         fm.setTransformationDataModified();
      }
      else if (specFileTag == SpecFile::volumeAnatomyFileTag) {
         theMainWindow->getBrainSet()->readVolumeFile(name, VolumeFile::VOLUME_TYPE_ANATOMY, append, update);
         fm.setVolumeModified();
      }
      else if (specFileTag == SpecFile::volumeFunctionalFileTag) {
         theMainWindow->getBrainSet()->readVolumeFile(name, VolumeFile::VOLUME_TYPE_FUNCTIONAL, append, update);
         fm.setVolumeModified();
      }
      else if (specFileTag == SpecFile::volumePaintFileTag) {
         theMainWindow->getBrainSet()->readVolumeFile(name, VolumeFile::VOLUME_TYPE_PAINT, append, update);
         fm.setVolumeModified();
         areaColorWarning = needAreaColors;
      }
      else if (specFileTag == SpecFile::volumeProbAtlasFileTag) {
         theMainWindow->getBrainSet()->readVolumeFile(name, VolumeFile::VOLUME_TYPE_PROB_ATLAS, append, update);
         fm.setVolumeModified();
         areaColorWarning = needAreaColors;
      }
      else if (specFileTag == SpecFile::volumeRgbFileTag) {
         theMainWindow->getBrainSet()->readVolumeFile(name, VolumeFile::VOLUME_TYPE_RGB, append, update);
         fm.setVolumeModified();
      }
      else if (specFileTag == SpecFile::volumeSegmentationFileTag) {
         theMainWindow->getBrainSet()->readVolumeFile(name, VolumeFile::VOLUME_TYPE_SEGMENTATION, append, update);
         fm.setVolumeModified();
      }
      else if (specFileTag == SpecFile::volumeVectorFileTag) {
         theMainWindow->getBrainSet()->readVolumeFile(name, VolumeFile::VOLUME_TYPE_VECTOR, append, update);
         fm.setVolumeModified();
      }
      else if (specFileTag == SpecFile::vtkModelFileTag) {
         theMainWindow->getBrainSet()->readVtkModelFile(name, append, update);
         fm.setVtkModelModified();
      }
      else if (specFileTag == SpecFile::vocabularyFileTag) {
         theMainWindow->getBrainSet()->readVocabularyFile(name, append, update);
         fm.setVocabularyModified();
      }
      else if (specFileTag == SpecFile::wustlRegionFileTag) {
         theMainWindow->getBrainSet()->readWustlRegionFile(name, append, update);
         fm.setWustlRegionModified();
      }
      else {
         std::cerr << "PROGRAM ERROR: unhandled file type " << specFileTag.toAscii().constData() << " at " << __LINE__
                  << " in " << __FILE__ << std::endl;
      }
   }
   catch (FileException& e) {
      errorMessage = e.whatQString();
      return true;
   }
   
   if (bordersLoaded) {
      DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
      if (dsb->getDisplayBorders() == false) {
         dsb->setDisplayBorders(true);
         theMainWindow->updateDisplayControlDialog();
      }
   }
   
   theMainWindow->fileModificationUpdate(fm);
   if (fm.getCoordinateModified()) {
      theMainWindow->displayNewestSurfaceInMainWindow();
   }
   
   GuiBrainModelOpenGL::updateAllGL(NULL);
   
   if (areaColorWarning) {
      GuiMessageBox::warning(theMainWindow, "Warning",
                           "The file just opened requires area colors\n"
                           "but there are no area colors loaded.", "OK");
      relatedFileWarningFlag = true;
   }
   
   if (borderColorWarning) {
      GuiMessageBox::warning(theMainWindow, "Warning",
                           "The file just opened requires border colors\n"
                           "but there are no border colors loaded.", "OK");
      relatedFileWarningFlag = true;
   }
   
   if (cellColorWarning) {
      GuiMessageBox::warning(theMainWindow, "Warning",
                           "The file just opened requires cell colors\n"
                           "but there are no cell colors loaded.", "OK");
      relatedFileWarningFlag = true;
   }
   
   if (contourCellColorWarning) {
      GuiMessageBox::warning(theMainWindow, "Warning",
                           "The file just opened requires contour cell colors\n"
                           "but there are no contour cell colors loaded.", "OK");
      relatedFileWarningFlag = true;
   }
   
   if (fociColorWarning) {
      GuiMessageBox::warning(theMainWindow, "Warning",
                           "The file just opened requires foci colors\n"
                           "but there are no foci colors loaded.", "OK");
      relatedFileWarningFlag = true;
   }
   
   return false;
}


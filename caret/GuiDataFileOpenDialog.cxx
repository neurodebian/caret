
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
#include <QDir>
#include <QGroupBox>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BrainModelContours.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "CellColorFile.h"
#include "ContourCellColorFile.h"
#include "ContourCellFile.h"
#include "CoordinateFile.h"
#include "DisplaySettingsBorders.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "FociColorFile.h"
#include "FociFile.h"
#include "GeodesicDistanceFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiDataFileCommentDialog.h"
#include "GuiDataFileImportOptionsDialog.h"
#include "GuiDataFileOpenDialog.h"
#include "GuiFilesModified.h"
#include "GuiLoadNodeAttributeFileColumnSelectionDialog.h"
#include "GuiMainWindow.h"
#include "GuiSpecFileCreationDialog.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "QtRadioButtonSelectionDialog.h"
#include "RgbPaintFile.h"
#include "SpecFile.h"
#include "SurfaceFile.h"
#include "SurfaceShapeFile.h"
#include "SurfaceVectorFile.h"
#include "TopologyFile.h"
#include "VolumeFile.h"
#include "global_variables.h"

static const QString GENERIC_BORDER("GENERIC_BORDER");
static const QString GENERIC_COORDINATE("GENERIC_COORDINATE");
static const QString GENERIC_SURFACE("GENERIC_SURFACE");
static const QString GENERIC_TOPOLOGY("GENERIC_TOPOLOGY");

/**
 * constructor.
 */
GuiDataFileOpenDialog::GuiDataFileOpenDialog(QWidget* parent,
                                             Qt::WindowFlags flags)
   : WuQFileDialog(parent, flags)
{
   setWindowTitle("Open Data File");
   setAcceptMode(AcceptOpen);
   setFileMode(ExistingFiles);
   setDirectory(QDir::currentPath());

   //
   // Caret files
   //
   QStringList filterNames;
   filterNames << FileFilters::getAnyFileFilter();
   filterNames << FileFilters::getAreaColorFileFilter();
   filterNames << FileFilters::getArealEstimationFileFilter();
   filterNames << FileFilters::getBorderGenericFileFilter();
   filterNames << FileFilters::getBorderVolumeFileFilter();
   filterNames << FileFilters::getBorderColorFileFilter();
   filterNames << FileFilters::getBorderProjectionFileFilter();
   filterNames << FileFilters::getCellFileFilter();
   filterNames << FileFilters::getCellColorFileFilter();
   filterNames << FileFilters::getCellProjectionFileFilter();
   filterNames << FileFilters::getCellVolumeFileFilter();
   filterNames << FileFilters::getCocomacFileFilter();
   filterNames << FileFilters::getContourFileFilter();
   filterNames << FileFilters::getContourCellFileFilter();
   filterNames << FileFilters::getContourCellColorFileFilter();
   filterNames << FileFilters::getCoordinateGenericFileFilter();
   filterNames << FileFilters::getCutsFileFilter();
   filterNames << FileFilters::getDeformationFieldFileFilter();
   filterNames << FileFilters::getDeformationMapFileFilter();
   filterNames << FileFilters::getFociFileFilter();
   filterNames << FileFilters::getFociColorFileFilter();
   filterNames << FileFilters::getFociProjectionFileFilter();
   filterNames << FileFilters::getFociVolumeFileFilter();
   filterNames << FileFilters::getGeodesicDistanceFileFilter();
   if (GiftiDataArrayFile::getGiftiXMLEnabled()) {
      filterNames << FileFilters::getGiftiCoordinateFileFilter();
      filterNames << FileFilters::getGiftiFunctionalFileFilter();
      filterNames << FileFilters::getGiftiLabelFileFilter();
      filterNames << FileFilters::getGiftiShapeFileFilter();
      filterNames << FileFilters::getGiftiSurfaceFileFilter();
      filterNames << FileFilters::getGiftiTopologyFileFilter();
   }
   filterNames << FileFilters::getImageOpenFileFilter();
   filterNames << FileFilters::getLatitudeLongitudeFileFilter();
   filterNames << FileFilters::getMetricOrShapeAsMetricFileFilter();
   filterNames << FileFilters::getPaintFileFilter();
   filterNames << FileFilters::getPaletteFileFilter();
   filterNames << FileFilters::getParamsFileFilter();
   filterNames << FileFilters::getProbAtlasFileFilter();
   filterNames << FileFilters::getRgbPaintFileFilter();
   filterNames << FileFilters::getSceneFileFilter();
   filterNames << FileFilters::getSectionFileFilter();
   filterNames << FileFilters::getStudyMetaDataFileFilter();
   filterNames << FileFilters::getSurfaceShapeOrMetricAsShapeFileFilter();
   filterNames << FileFilters::getSurfaceVectorFileFilter();
   filterNames << FileFilters::getTopographyFileFilter();
   filterNames << FileFilters::getTopologyGenericFileFilter();
   filterNames << FileFilters::getTransformationMatrixFileFilter();
   filterNames << FileFilters::getTransformationDataFileFilter();
   filterNames << FileFilters::getVocabularyFileFilter();
   filterNames << FileFilters::getVolumeAnatomyFileFilter();
   filterNames << FileFilters::getVolumeFunctionalFileFilter();
   filterNames << FileFilters::getVolumePaintFileFilter();
   filterNames << FileFilters::getVolumeProbAtlasFileFilter();
   filterNames << FileFilters::getVolumeRgbFileFilter();
   filterNames << FileFilters::getVolumeSegmentationFileFilter();
   filterNames << FileFilters::getVolumeVectorFileFilter();
   filterNames << FileFilters::getVtkModelFileFilter();
   filterNames << FileFilters::getWustlRegionFileFilter();
   
   //
   // Import Files
   //
   filterNames << FileFilters::getAnalyzeVolumeFileFilter();
   filterNames << FileFilters::getBrainVoyagerFileFilter();
   filterNames << FileFilters::getByuSurfaceFileFilter();
   filterNames << FileFilters::getFreeSurferAsciiSurfaceFileFilter();
   filterNames << FileFilters::getFreeSurferBinarySurfaceFileFilter();
   filterNames << FileFilters::getFreeSurferAsciiCurvatureFileFilter();
   filterNames << FileFilters::getFreeSurferBinaryCurvatureFileFilter();
   filterNames << FileFilters::getFreeSurferAsciiFunctionalFileFilter();
   filterNames << FileFilters::getFreeSurferBinaryFunctionalFileFilter();
   filterNames << FileFilters::getFreeSurferAsciiLabelFileFilter();
   filterNames << FileFilters::getMdPlotFileFilter();
#ifdef HAVE_MINC
   filterNames << FileFilters::getMincVolumeFileFilter();
#endif // HAVE_MINC
   filterNames << FileFilters::getNeurolucidaFileFilter();
   filterNames << FileFilters::getRawVolumeFileFilter();
   filterNames << FileFilters::getStlSurfaceFileFilter();
   filterNames << FileFilters::getSumaRgbFileFilter();
   filterNames << FileFilters::getVtkSurfaceFileFilter();
   filterNames << FileFilters::getVtkXmlSurfaceFileFilter();
   filterNames << FileFilters::getVtkVolumeFileFilter();
   
   //
   // Add the filters to the dialog
   //
   setFilters(filterNames);

   //
   // Connect signals
   //
   QObject::connect(this, SIGNAL(filesSelected(const QStringList&)),
                    this, SLOT(slotFilesSelected(const QStringList&)));
                    
   //
   // View/Edit comment button
   //
   viewEditCommentPushButton = new QPushButton("View/Edit Comment...");
   viewEditCommentPushButton->setEnabled(false);
   QObject::connect(viewEditCommentPushButton, SIGNAL(clicked()),
                    this, SLOT(slotViewEditCommentPushButton()));
   
   //
   // Add to spec file check box
   //
   addToSpecFileCheckBox = new QCheckBox("Add to Specification File");
   addToSpecFileCheckBox->setChecked(true);
   
   //
   // Append to current file check box
   //
   appendToCurrentFileCheckBox = new QCheckBox("Append to Current File");
   appendToCurrentFileCheckBox->setChecked(true);
   
   //
   // Group box for options
   //
   QGroupBox* optionsGroupBox = new QGroupBox("Open File Options");
   QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroupBox);
   optionsLayout->addWidget(addToSpecFileCheckBox);
   optionsLayout->addWidget(appendToCurrentFileCheckBox);
   
   //
   // Add widgets to dialog
   //
   addWidgets(optionsGroupBox, optionsGroupBox, viewEditCommentPushButton);
}
                  
/**
 * destructor.
 */
GuiDataFileOpenDialog::~GuiDataFileOpenDialog()
{
}

/**
 * called when view/edit comment button pressed.
 */
void 
GuiDataFileOpenDialog::slotViewEditCommentPushButton()
{
   const QStringList files = selectedFiles();
   if (files.count() == 1) {
      const QString highlightedFileName = files.at(0);
      if (highlightedFileName.isEmpty() == false) {
         const QString filterName = selectedFilter();
         
         //
         // See if a volume file that needs to be handles specially
         //
         bool volumeFileFlag =
            (filterName == FileFilters::getVolumeAnatomyFileFilter()) ||
            (filterName == FileFilters::getVolumeFunctionalFileFilter()) ||
            (filterName == FileFilters::getVolumePaintFileFilter()) ||
            (filterName == FileFilters::getVolumeProbAtlasFileFilter()) ||
            (filterName == FileFilters::getVolumeRgbFileFilter()) ||
            (filterName == FileFilters::getVolumeSegmentationFileFilter()) ||
            (filterName == FileFilters::getVolumeVectorFileFilter());
         if (selectedFilter() == FileFilters::getAnyFileFilter()) {
            const QStringList mf = matchingFilters(highlightedFileName);
            for (int i = 0; i < mf.count(); i++) {
               if (mf.at(i) == FileFilters::getVolumeAnatomyFileFilter()) {
                  volumeFileFlag = true;
                  break;
               }
            }
         }
         
         GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(this,
                                                                       highlightedFileName,
                                                                       volumeFileFlag);
         dfcd->show();
      }
   }
}

/**
 * called when the file selection is changed.
 */
void 
GuiDataFileOpenDialog::slotFilesSelected(const QStringList& fileNames)
{
   viewEditCommentPushButton->setEnabled(fileNames.count() == 1);
   
   if (fileNames.count() == 1) {
      const QStringList mf = matchingFilters(fileNames.at(0));
      for (int i = 0; i < mf.count(); i++) {
         //std::cout << "Matching Filters: " << mf.at(i).toAscii().constData() << std::endl;
      }
   }
}

/**
 * returns file filters that match the name but exludes any file filter
 */
QStringList 
GuiDataFileOpenDialog::matchingFilters(const QString& name)
{
   //
   // Try a list of all but ANY and filters that have no extension
   //
   QStringList filterList = WuQFileDialog::matchingFilters(name);
   filterList.removeAll(FileFilters::getAnyFileFilter());
   filterList.removeAll(FileFilters::getFreeSurferBinarySurfaceFileFilter());
   filterList.removeAll(FileFilters::getRawVolumeFileFilter());
   
   if (filterList.count() == 0) {
      //
      // Get a list of all but ANY
      //
      filterList = WuQFileDialog::matchingFilters(name);
      filterList.removeAll(FileFilters::getAnyFileFilter());
   }
   
   return filterList;
}
      
/**
 * called when dialog is closed.
 */
void 
GuiDataFileOpenDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      //
      // Get the selected files
      //
      QStringList filesChosen(selectedFiles());
      const int numFiles = filesChosen.count();
      if (numFiles > 0) {
         //
         // Update previous directories
         //
         PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
         pf->addToRecentDataFileDirectories(FileUtilities::dirname(filesChosen.at(0)), true);
         
         //
         // Add to spec file status and append status
         //
         const bool addToSpecFlag = addToSpecFileCheckBox->isChecked();
         const bool appendToCurrentFileFlag = appendToCurrentFileCheckBox->isChecked();
         
         //
         // Allow user to set the spec file name if not already set
         //
         const QString specFileName(theMainWindow->getBrainSet()->getSpecFileName());
         if (addToSpecFlag && specFileName.isEmpty()) {
            const int result = QMessageBox::question(this, "Spec File", 
                                         "Would you like to create a Spec File ?",
                                         (QMessageBox::Yes |
                                          QMessageBox::No  |
                                          QMessageBox::Cancel),
                                         QMessageBox::No);
            if (result == QMessageBox::Yes) {
               //
               // Create spec file dialog will set the directory and create the spec file
               //
               GuiSpecFileCreationDialog sfcd(this);
               sfcd.exec();
               PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
               pf->addToRecentSpecFiles(sfcd.getCreatedSpecFileName(), true);
            }
            else if (result == QMessageBox::No) {
            }
            else if (result == QMessageBox::Cancel) {
               return;
            }
         }

         //
         // See if file(s) may need to be copied to current directory
         //
         bool copyFlag = false;
         if (addToSpecFlag && 
             (specFileName.isEmpty() == false)) {
            //
            // Is data file in a directory different than that of spec file
            //
            QFileInfo dataFileInfo(filesChosen.at(0));
            QFileInfo specFileInfo(specFileName);
            if (dataFileInfo.absolutePath() != specFileInfo.absolutePath()) {
               const int result = QMessageBox::question(this, "Copy File",
                                       "Would you like to copy the "
                                       "file(s) to the current directory.",
                                         (QMessageBox::Yes |
                                          QMessageBox::No  |
                                          QMessageBox::Cancel),
                                         QMessageBox::No);
               if (result == QMessageBox::Yes) {
                  copyFlag = true;
               }
               else if (result == QMessageBox::No) {
               }
               else if (result == QMessageBox::Cancel) {
                  return;
               }
            }
         }
         
         //
         // Get the name of the file filter
         //
         const QString selectedFilterName(selectedFilter());
         
         //
         // Loop through the selected files
         //
         for (int i = 0; i < numFiles; i++) {
            //
            // Get the file
            //
            QString fileName = filesChosen.at(i);
            const QString nameNoPath(FileUtilities::basename(fileName));
            //std::cout << "Loading: " << fileName.toAscii().constData() << std::endl;
            
            //
            // Should file be copied?
            //
            if (copyFlag) {
               if (copyFile(fileName) == false) {
                  return;
               }
               
               //
               // Remove path since file copied
               //
               fileName = FileUtilities::basename(fileName);
            }
            
            //
            // Get the file filter
            //
            QString filterName = selectedFilterName;
            
            //
            // If filter is any file filter, find matching filter
            //
            if (filterName == FileFilters::getAnyFileFilter()) {
               //
               // Get filter that match the file name
               //
               QStringList filtersThatMatch = matchingFilters(fileName);
               
               //
               // If only one filter, then use it, otherwise ask user
               //
               if (filtersThatMatch.count() == 1) {
                  filterName = filtersThatMatch.at(0);
               }
               else if (filtersThatMatch.count() <= 0) {
                  const QString msg(FileUtilities::basename(fileName) 
                                    + " is not a supported Caret file type.");
                  QMessageBox::critical(this,
                                          "ERROR",
                                          msg);
                  return;
               }
               else {
                  //
                  // Ask user which file type to use
                  //
                  std::vector<QString> labels;
                  for (int i = 0; i < filtersThatMatch.count(); i++) {
                     QString name = filtersThatMatch.at(i);
                     const int indx = name.indexOf('(');
                     if (indx > 0) {
                        name = name.left(indx);
                     }
                     labels.push_back(name);
                  }
                  
                  QtRadioButtonSelectionDialog rbsd(this,
                                                    "Choose File Type",
                                                    "Type of File for " + nameNoPath,
                                                    labels,
                                                    0);
                  if (rbsd.exec() == QtRadioButtonSelectionDialog::Accepted) {
                     filterName = filtersThatMatch.at(rbsd.getSelectedItemIndex());
                  }
                  else {
                     return;
                  }
               }
            }
            
            //
            // Read the file
            //
            readFile(fileName,
                     filterName,
                     addToSpecFlag,
                     appendToCurrentFileFlag);
         }
      }
   }
   
   WuQFileDialog::done(r);
}

/**
 * read the file.
 */
void 
GuiDataFileOpenDialog::readFile(const QString& fileName,
                                const QString& filterName,
                                const bool addToSpecFileFlag,
                                const bool appendToCurrentFileFlag)
{
   //
   // SPM Volume use Analyze file extension and sometimes 
   // are left/right flipped
   //
   bool spmLeftOnRightFlag = false;
   const bool volumeFileFlag =
      (filterName == FileFilters::getVolumeAnatomyFileFilter()) ||
      (filterName == FileFilters::getVolumeFunctionalFileFilter()) ||
      (filterName == FileFilters::getVolumePaintFileFilter()) ||
      (filterName == FileFilters::getVolumeProbAtlasFileFilter()) ||
      (filterName == FileFilters::getVolumeRgbFileFilter()) ||
      (filterName == FileFilters::getVolumeSegmentationFileFilter()) ||
      (filterName == FileFilters::getVolumeVectorFileFilter());
   if (volumeFileFlag) {
      if (fileName.endsWith(SpecFile::getAnalyzeVolumeFileExtension())) {
         const QString msg("If the volume being loaded an SPM volume that\n"
                           "is left/right flipped (radiological orientation)?");
         spmLeftOnRightFlag = (QMessageBox::question(this,
                                                       "SPM Flipped",
                                                       msg,
                                                       (QMessageBox::Yes | QMessageBox::No))
                                                     == QMessageBox::Yes);
      }
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   theMainWindow->getBrainSet()->setDisplaySplashImage(false);
   
   QString msg;      
   bool error = false;
   bool warning = false;
   
   if (filterName == FileFilters::getAreaColorFileFilter()) {
      error = openDataFile(this, SpecFile::areaColorFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getArealEstimationFileFilter()) {
      error = openDataFile(this, SpecFile::arealEstimationFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderRawFileFilter()) {
      error = openDataFile(this, SpecFile::rawBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderFiducialFileFilter()) {
      error = openDataFile(this, SpecFile::fiducialBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderInflatedFileFilter()) {
      error = openDataFile(this, SpecFile::inflatedBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderVeryInflatedFileFilter()) {
      error = openDataFile(this, SpecFile::veryInflatedBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderSphericalFileFilter()) {
      error = openDataFile(this, SpecFile::sphericalBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderEllipsoidalFileFilter()) {
      error = openDataFile(this, SpecFile::ellipsoidBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderCompressedFileFilter()) {
      error = openDataFile(this, SpecFile::compressedBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderFlatFileFilter()) {
      error = openDataFile(this, SpecFile::flatBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderFlatLobarFileFilter()) {
      error = openDataFile(this, SpecFile::lobarFlatBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderHullFileFilter()) {
      error = openDataFile(this, SpecFile::hullBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderUnknownFileFilter()) {
      error = openDataFile(this, SpecFile::unknownBorderFileMatchTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderVolumeFileFilter()) {
      error = openDataFile(this, SpecFile::volumeBorderFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderGenericFileFilter()) {
      error = openDataFile(this, GENERIC_BORDER, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderColorFileFilter()) {
      error = openDataFile(this, SpecFile::borderColorFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getBorderProjectionFileFilter()) {
      error = openDataFile(this, SpecFile::borderProjectionFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCellFileFilter()) {
      error = openDataFile(this, SpecFile::cellFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCellColorFileFilter()) {
      error = openDataFile(this, SpecFile::cellColorFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCellProjectionFileFilter()) {
      error = openDataFile(this, SpecFile::cellProjectionFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCellVolumeFileFilter()) {
      error = openDataFile(this, SpecFile::volumeCellFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCocomacFileFilter()) {
      error = openDataFile(this, SpecFile::cocomacConnectivityFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getContourFileFilter()) {
      error = openDataFile(this, SpecFile::contourFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getContourCellFileFilter()) {
      error = openDataFile(this, SpecFile::contourCellFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getContourCellColorFileFilter()) {
      error = openDataFile(this, SpecFile::contourCellColorFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateRawFileFilter()) {
      error = openDataFile(this, SpecFile::rawCoordFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateFiducialFileFilter()) {
      error = openDataFile(this, SpecFile::fiducialCoordFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateInflatedFileFilter()) {
      error = openDataFile(this, SpecFile::inflatedCoordFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateVeryInflatedFileFilter()) {
      error = openDataFile(this, SpecFile::veryInflatedCoordFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateSphericalFileFilter()) {
      error = openDataFile(this, SpecFile::sphericalCoordFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateEllipsoidalFileFilter()) {
      error = openDataFile(this, SpecFile::ellipsoidCoordFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateCompressedFileFilter()) {
      error = openDataFile(this, SpecFile::compressedCoordFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateFlatFileFilter()) {
      error = openDataFile(this, SpecFile::flatCoordFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateFlatLobarFileFilter()) {
      error = openDataFile(this, SpecFile::lobarFlatCoordFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateHullFileFilter()) {
      error = openDataFile(this, SpecFile::hullCoordFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateUnknownFileFilter()) {
      error = openDataFile(this, SpecFile::unknownCoordFileMatchTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCoordinateGenericFileFilter()) {
      error = openDataFile(this, GENERIC_COORDINATE, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getCutsFileFilter()) {
      error = openDataFile(this, SpecFile::cutsFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getDeformationFieldFileFilter()) {
      error = openDataFile(this, SpecFile::deformationFieldFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getDeformationMapFileFilter()) {
      error = openDataFile(this, SpecFile::deformationMapFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getFociFileFilter()) {
      error = openDataFile(this, SpecFile::fociFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getFociColorFileFilter()) {
      error = openDataFile(this, SpecFile::fociColorFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getFociProjectionFileFilter()) {
      error = openDataFile(this, SpecFile::fociProjectionFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getFociVolumeFileFilter()) {
      error = openDataFile(this, SpecFile::volumeFociFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getGeodesicDistanceFileFilter()) {
      error = openDataFile(this, SpecFile::geodesicDistanceFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getGiftiCoordinateFileFilter()) {
      error = openDataFile(this, GENERIC_COORDINATE, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getGiftiFunctionalFileFilter()) {
      error = openDataFile(this, SpecFile::metricFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getGiftiLabelFileFilter()) {
      error = openDataFile(this, SpecFile::paintFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getGiftiShapeFileFilter()) {
      error = openDataFile(this, SpecFile::surfaceShapeFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getGiftiSurfaceFileFilter()) {
      error = openDataFile(this, GENERIC_SURFACE, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getGiftiTopologyFileFilter()) {
      error = openDataFile(this, GENERIC_TOPOLOGY, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getImageOpenFileFilter()) {
      error = openDataFile(this, SpecFile::imageFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getLatitudeLongitudeFileFilter()) {
      error = openDataFile(this, SpecFile::latLonFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if ((filterName == FileFilters::getMetricFileFilter()) ||
            (filterName == FileFilters::getMetricOrShapeAsMetricFileFilter())) {
      error = openDataFile(this, SpecFile::metricFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getPaintFileFilter()) {
      error = openDataFile(this, SpecFile::paintFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getPaletteFileFilter()) {
      error = openDataFile(this, SpecFile::paletteFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getParamsFileFilter()) {
      error = openDataFile(this, SpecFile::paramsFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getProbAtlasFileFilter()) {
      error = openDataFile(this, SpecFile::atlasFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getRgbPaintFileFilter()) {
      error = openDataFile(this, SpecFile::rgbPaintFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getSceneFileFilter()) {
      error = openDataFile(this, SpecFile::sceneFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getSectionFileFilter()) {
      error = openDataFile(this, SpecFile::sectionFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if ((filterName == FileFilters::getSurfaceShapeFileFilter()) ||
            (filterName == FileFilters::getSurfaceShapeOrMetricAsShapeFileFilter())) {
      error = openDataFile(this, SpecFile::surfaceShapeFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getSurfaceVectorFileFilter()) {
      error = openDataFile(this, SpecFile::surfaceVectorFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getTopographyFileFilter()) {
      error = openDataFile(this, SpecFile::topographyFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getTopologyClosedFileFilter()) {
      error = openDataFile(this, SpecFile::closedTopoFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getTopologyOpenFileFilter()) {
      error = openDataFile(this, SpecFile::openTopoFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getTopologyCutFileFilter()) {
      error = openDataFile(this, SpecFile::cutTopoFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getTopologyCutLobarFileFilter()) {
      error = openDataFile(this, SpecFile::lobarCutTopoFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getTopologyUnknownFileFilter()) {
      error = openDataFile(this, SpecFile::unknownTopoFileMatchTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getTopologyGenericFileFilter()) {
      error = openDataFile(this, GENERIC_TOPOLOGY, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getVolumeAnatomyFileFilter()) {
      error = openDataFile(this, SpecFile::volumeAnatomyFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
      if (spmLeftOnRightFlag) {
         const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles();
         if (volumeIndex > 0) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeAnatomyFile(volumeIndex - 1);
            vf->flip(VolumeFile::VOLUME_AXIS_X, false);
            vf->clearModified();
         }
      }
   }
   else if (filterName == FileFilters::getVolumeFunctionalFileFilter()) {
      error = openDataFile(this, SpecFile::volumeFunctionalFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
      if (spmLeftOnRightFlag) {
         const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles();
         if (volumeIndex > 0) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeFunctionalFile(volumeIndex - 1);
            vf->flip(VolumeFile::VOLUME_AXIS_X, false);
            vf->clearModified();
         }
      }
   }
   else if (filterName == FileFilters::getVolumePaintFileFilter()) {
      error = openDataFile(this, SpecFile::volumePaintFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
      if (spmLeftOnRightFlag) {
         const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles();
         if (volumeIndex > 0) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumePaintFile(volumeIndex - 1);
            vf->flip(VolumeFile::VOLUME_AXIS_X, false);
            vf->clearModified();
         }
      }
   }
   else if (filterName == FileFilters::getVolumeProbAtlasFileFilter()) {
      error = openDataFile(this, SpecFile::volumeProbAtlasFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
      if (spmLeftOnRightFlag) {
         const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles();
         if (volumeIndex > 0) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeProbAtlasFile(volumeIndex - 1);
            vf->flip(VolumeFile::VOLUME_AXIS_X, false);
            vf->clearModified();
         }
      }
   }
   else if (filterName == FileFilters::getVolumeRgbFileFilter()) {
      error = openDataFile(this, SpecFile::volumeRgbFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
      if (spmLeftOnRightFlag) {
         const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles();
         if (volumeIndex > 0) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeRgbFile(volumeIndex - 1);
            vf->flip(VolumeFile::VOLUME_AXIS_X, false);
            vf->clearModified();
         }
      }
   }
   else if (filterName == FileFilters::getVolumeSegmentationFileFilter()) {
      error = openDataFile(this, SpecFile::volumeSegmentationFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
      if (spmLeftOnRightFlag) {
         const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles();
         if (volumeIndex > 0) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeSegmentationFile(volumeIndex - 1);
            vf->flip(VolumeFile::VOLUME_AXIS_X, false);
            vf->clearModified();
         }
      }
   }
   else if (filterName == FileFilters::getVolumeVectorFileFilter()) {
      error = openDataFile(this, SpecFile::volumeVectorFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
      if (spmLeftOnRightFlag) {
         const int volumeIndex = theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles();
         if (volumeIndex > 0) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeVectorFile(volumeIndex - 1);
            vf->flip(VolumeFile::VOLUME_AXIS_X, false);
            vf->clearModified();
         }
      }
   }
   else if (filterName == FileFilters::getStudyMetaDataFileFilter()) {
      error = openDataFile(this, SpecFile::studyMetaDataFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getTransformationMatrixFileFilter()) {
      error = openDataFile(this, SpecFile::transformationMatrixFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getTransformationDataFileFilter()) {
      error = openDataFile(this, SpecFile::transformationDataFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getVtkModelFileFilter()) {
      error = openDataFile(this, SpecFile::vtkModelFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getVocabularyFileFilter()) {
      error = openDataFile(this, SpecFile::vocabularyFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (filterName == FileFilters::getWustlRegionFileFilter()) {
      error = openDataFile(this, SpecFile::wustlRegionFileTag, fileName, appendToCurrentFileFlag, addToSpecFileFlag, msg, warning);
   }
   else if (importFile(fileName, filterName)) {
      // do nothing here
   }
   else {
      std::cerr << "PROGRAM ERROR: unhandled file type at " << __LINE__
               << " in " << __FILE__ << std::endl;
      QApplication::restoreOverrideCursor(); 
      return;
   }
   
   QApplication::restoreOverrideCursor(); 
   if (error) {
      QMessageBox::critical(this, "Error Reading File", msg);
      return;
   }
}                    

/**
 * process import files (returns true if filter matched an import file).
 */
bool 
GuiDataFileOpenDialog::importFile(const QString& fileName,
                                  const QString& filterName)
{
   GuiFilesModified fm;
   BrainSet* brainSet = theMainWindow->getBrainSet();

   QApplication::restoreOverrideCursor();
   
   if (filterName == FileFilters::getAnalyzeVolumeFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         VolumeFile::VOLUME_TYPE volumeType;
         int dimensions[3];
         VolumeFile::VOXEL_DATA_TYPE voxelDataType;
         bool byteSwap;
         iod.getVolumeOptions(volumeType,
                              dimensions,
                              voxelDataType,
                              byteSwap);
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importAnalyzeVolumeFile(fileName,
                                           volumeType);
         fm.setVolumeModified();
      }
   }
   else if (filterName == FileFilters::getBrainVoyagerFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         Structure::STRUCTURE_TYPE structureType;
         bool importTopology;
         TopologyFile::TOPOLOGY_TYPES topologyType;
         bool importCoordinates;
         BrainModelSurface::SURFACE_TYPES coordinateType;
         bool importColorsAsPaint;
         bool importColorsAsRgbPaint;
         iod.getSurfaceOptions(structureType,
                               importTopology,
                               topologyType,
                               importCoordinates,
                               coordinateType,
                               importColorsAsPaint,
                               importColorsAsRgbPaint);
         if (brainSet->getStructure().getType() == Structure::STRUCTURE_TYPE_INVALID) {
            brainSet->setStructure(structureType);
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importBrainVoyagerFile(fileName,
                                          importCoordinates,
                                          importTopology,
                                          importColorsAsPaint,
                                          coordinateType,
                                          topologyType);
         fm.setCoordinateModified();
         fm.setPaintModified();
         fm.setAreaColorModified();
         fm.setTopologyModified();
      }
   }
   else if (filterName == FileFilters::getByuSurfaceFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         Structure::STRUCTURE_TYPE structureType;
         bool importTopology;
         TopologyFile::TOPOLOGY_TYPES topologyType;
         bool importCoordinates;
         BrainModelSurface::SURFACE_TYPES coordinateType;
         bool importColorsAsPaint;
         bool importColorsAsRgbPaint;
         iod.getSurfaceOptions(structureType,
                               importTopology,
                               topologyType,
                               importCoordinates,
                               coordinateType,
                               importColorsAsPaint,
                               importColorsAsRgbPaint);
         if (brainSet->getStructure().getType() == Structure::STRUCTURE_TYPE_INVALID) {
            brainSet->setStructure(structureType);
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importByuSurfaceFile(fileName,
                                          importCoordinates,
                                          importTopology,
                                          coordinateType,
                                          topologyType);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
   }
   else if (filterName == FileFilters::getFreeSurferAsciiSurfaceFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         Structure::STRUCTURE_TYPE structureType;
         bool importTopology;
         TopologyFile::TOPOLOGY_TYPES topologyType;
         bool importCoordinates;
         BrainModelSurface::SURFACE_TYPES coordinateType;
         bool importColorsAsPaint;
         bool importColorsAsRgbPaint;
         iod.getSurfaceOptions(structureType,
                               importTopology,
                               topologyType,
                               importCoordinates,
                               coordinateType,
                               importColorsAsPaint,
                               importColorsAsRgbPaint);
         if (brainSet->getStructure().getType() == Structure::STRUCTURE_TYPE_INVALID) {
            brainSet->setStructure(structureType);
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importFreeSurferSurfaceFile(fileName,
                                          importCoordinates,
                                          importTopology,
                                          AbstractFile::FILE_FORMAT_ASCII,
                                          coordinateType,
                                          topologyType);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
   }
   else if (filterName == FileFilters::getFreeSurferBinarySurfaceFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         Structure::STRUCTURE_TYPE structureType;
         bool importTopology;
         TopologyFile::TOPOLOGY_TYPES topologyType;
         bool importCoordinates;
         BrainModelSurface::SURFACE_TYPES coordinateType;
         bool importColorsAsPaint;
         bool importColorsAsRgbPaint;
         iod.getSurfaceOptions(structureType,
                               importTopology,
                               topologyType,
                               importCoordinates,
                               coordinateType,
                               importColorsAsPaint,
                               importColorsAsRgbPaint);
         if (brainSet->getStructure().getType() == Structure::STRUCTURE_TYPE_INVALID) {
            brainSet->setStructure(structureType);
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importFreeSurferSurfaceFile(fileName,
                                          importCoordinates,
                                          importTopology,
                                          AbstractFile::FILE_FORMAT_BINARY,
                                          coordinateType,
                                          topologyType);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
   }
   else if (filterName == FileFilters::getFreeSurferAsciiCurvatureFileFilter()) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
      ssf->importFreeSurferCurvatureFile(brainSet->getNumberOfNodes(),
                                         fileName,
                                         AbstractFile::FILE_FORMAT_ASCII);
      fm.setSurfaceShapeModified();
   }
   else if (filterName == FileFilters::getFreeSurferBinaryCurvatureFileFilter()) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
      ssf->importFreeSurferCurvatureFile(brainSet->getNumberOfNodes(),
                                         fileName,
                                         AbstractFile::FILE_FORMAT_BINARY);
      fm.setSurfaceShapeModified();
   }
   else if (filterName == FileFilters::getFreeSurferAsciiFunctionalFileFilter()) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      MetricFile* mf = brainSet->getMetricFile();
      mf->importFreeSurferFunctionalFile(brainSet->getNumberOfNodes(),
                                         fileName,
                                         AbstractFile::FILE_FORMAT_ASCII);
      fm.setMetricModified();
   }
   else if (filterName == FileFilters::getFreeSurferBinaryFunctionalFileFilter()) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      MetricFile* mf = brainSet->getMetricFile();
      mf->importFreeSurferFunctionalFile(brainSet->getNumberOfNodes(),
                                         fileName,
                                         AbstractFile::FILE_FORMAT_BINARY);
      fm.setMetricModified();
   }
   else if (filterName == FileFilters::getFreeSurferAsciiLabelFileFilter()) {
      bool importAll = false;
      if (QMessageBox::question(this, "Import All",
                                "Import all label files in directory ?",
                                (QMessageBox::Yes | QMessageBox::No)) == QMessageBox::Yes) {
         importAll = true;
      }
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      PaintFile* pf = brainSet->getPaintFile();
      pf->importFreeSurferAsciiLabelFile(brainSet->getNumberOfNodes(),
                                         fileName,
                                         brainSet->getAreaColorFile(),
                                         importAll);
      fm.setPaintModified();
      fm.setAreaColorModified();
   }
   else if (filterName == FileFilters::getMdPlotFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         bool importContourCells;
         bool importContours;
         iod.getContourOptions(importContours, importContourCells);
         const BrainModelContours* bmc = brainSet->getBrainModelContours();
         bool appendContoursFlag = false;
         bool appendCellsFlag = false;
         if (importContours) {
            if (bmc != NULL) {
               const ContourFile* cf = bmc->getContourFile();
               if (cf->empty() == false) {
                  appendContoursFlag = (QMessageBox::question(this, "Import MD Plot",
                                              "Append to existing contours?",
                                              (QMessageBox::Yes | QMessageBox::No)) == QMessageBox::Yes);
               }
            }
         }
         if (importContourCells) {
            const ContourCellFile* cf = brainSet->getContourCellFile();
            if (cf->empty() == false) {
               appendCellsFlag = (QMessageBox::question(this, "Import MD Plot",
                                           "Append to existing contour cells?",
                                           (QMessageBox::Yes | QMessageBox::No)) == QMessageBox::Yes);
            }
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importMDPlotFile(fileName, 
                                 importContourCells,
                                 importContours,
                                 appendContoursFlag,
                                 appendCellsFlag);
         fm.setContourModified();
         fm.setContourCellModified();
         fm.setContourCellColorModified();
      }
   }
#ifdef HAVE_MINC
   else if (filterName == FileFilters::getMincVolumeFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         VolumeFile::VOLUME_TYPE volumeType;
         int dimensions[3];
         VolumeFile::VOXEL_DATA_TYPE voxelDataType;
         bool byteSwap;
         iod.getVolumeOptions(volumeType,
                              dimensions,
                              voxelDataType,
                              byteSwap);
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importMincVolumeFile(fileName,
                                        volumeType);
         fm.setVolumeModified();
      }
   }
#endif // HAVE_MINC
   else if (filterName == FileFilters::getNeurolucidaFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         bool importContourCells;
         bool importContours;
         iod.getContourOptions(importContours, importContourCells);
         const BrainModelContours* bmc = brainSet->getBrainModelContours();
         bool appendContoursFlag = false;
         bool appendCellsFlag = false;
         if (importContours) {
            if (bmc != NULL) {
               const ContourFile* cf = bmc->getContourFile();
               if (cf->empty() == false) {
                  appendContoursFlag = (QMessageBox::question(this, "Import Neurolucida",
                                              "Append to existing contours?",
                                              (QMessageBox::Yes | QMessageBox::No)) == QMessageBox::Yes);
               }
            }
         }
         if (importContourCells) {
            const ContourCellFile* cf = brainSet->getContourCellFile();
            if (cf->empty() == false) {
               appendCellsFlag = (QMessageBox::question(this, "Import Neurolucida",
                                           "Append to existing contour cells?",
                                           (QMessageBox::Yes | QMessageBox::No)) == QMessageBox::Yes);
            }
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importNeurolucidaFile(fileName, 
                                 importContourCells,
                                 importContours,
                                 appendContoursFlag,
                                 appendCellsFlag);
         fm.setContourModified();
         fm.setContourCellModified();
         fm.setContourCellColorModified();
      }
   }
   else if (filterName == FileFilters::getRawVolumeFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         VolumeFile::VOLUME_TYPE volumeType;
         int dimensions[3];
         VolumeFile::VOXEL_DATA_TYPE voxelDataType;
         bool byteSwap;
         iod.getVolumeOptions(volumeType,
                              dimensions,
                              voxelDataType,
                              byteSwap);
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importRawVolumeFile(fileName,
                                       volumeType,
                                       dimensions,
                                       voxelDataType,
                                       byteSwap);
         fm.setVolumeModified();
      }
   }
   else if (filterName == FileFilters::getStlSurfaceFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         Structure::STRUCTURE_TYPE structureType;
         bool importTopology;
         TopologyFile::TOPOLOGY_TYPES topologyType;
         bool importCoordinates;
         BrainModelSurface::SURFACE_TYPES coordinateType;
         bool importColorsAsPaint;
         bool importColorsAsRgbPaint;
         iod.getSurfaceOptions(structureType,
                               importTopology,
                               topologyType,
                               importCoordinates,
                               coordinateType,
                               importColorsAsPaint,
                               importColorsAsRgbPaint);
         if (brainSet->getStructure().getType() == Structure::STRUCTURE_TYPE_INVALID) {
            brainSet->setStructure(structureType);
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importStlSurfaceFile(fileName,
                                          importCoordinates,
                                          importTopology,
                                          coordinateType,
                                          topologyType);
         fm.setCoordinateModified();
         fm.setTopologyModified();
      }
   }
   else if (filterName == FileFilters::getSumaRgbFileFilter()) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      RgbPaintFile* rgb = brainSet->getRgbPaintFile();
      rgb->importFromSuma(fileName);
      fm.setRgbPaintModified();
   }
   else if (filterName == FileFilters::getVtkSurfaceFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         Structure::STRUCTURE_TYPE structureType;
         bool importTopology;
         TopologyFile::TOPOLOGY_TYPES topologyType;
         bool importCoordinates;
         BrainModelSurface::SURFACE_TYPES coordinateType;
         bool importColorsAsPaint;
         bool importColorsAsRgbPaint;
         iod.getSurfaceOptions(structureType,
                               importTopology,
                               topologyType,
                               importCoordinates,
                               coordinateType,
                               importColorsAsPaint,
                               importColorsAsRgbPaint);
         if (brainSet->getStructure().getType() == Structure::STRUCTURE_TYPE_INVALID) {
            brainSet->setStructure(structureType);
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importVtkSurfaceFile(fileName,
                                          importCoordinates,
                                          importTopology,
                                          importColorsAsRgbPaint,
                                          coordinateType,
                                          topologyType);
         fm.setCoordinateModified();
         fm.setRgbPaintModified();
         fm.setTopologyModified();
      }
   }
   else if (filterName == FileFilters::getVtkXmlSurfaceFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         Structure::STRUCTURE_TYPE structureType;
         bool importTopology;
         TopologyFile::TOPOLOGY_TYPES topologyType;
         bool importCoordinates;
         BrainModelSurface::SURFACE_TYPES coordinateType;
         bool importColorsAsPaint;
         bool importColorsAsRgbPaint;
         iod.getSurfaceOptions(structureType,
                               importTopology,
                               topologyType,
                               importCoordinates,
                               coordinateType,
                               importColorsAsPaint,
                               importColorsAsRgbPaint);
         if (brainSet->getStructure().getType() == Structure::STRUCTURE_TYPE_INVALID) {
            brainSet->setStructure(structureType);
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importVtkXmlSurfaceFile(fileName,
                                          importCoordinates,
                                          importTopology,
                                          importColorsAsRgbPaint,
                                          coordinateType,
                                          topologyType);
         fm.setCoordinateModified();
         fm.setRgbPaintModified();
         fm.setTopologyModified();
      }
   }
   else if (filterName == FileFilters::getVtkVolumeFileFilter()) {
      GuiDataFileImportOptionsDialog iod(this, filterName);
      if (iod.exec() == GuiDataFileImportOptionsDialog::Accepted) {
         VolumeFile::VOLUME_TYPE volumeType;
         int dimensions[3];
         VolumeFile::VOXEL_DATA_TYPE voxelDataType;
         bool byteSwap;
         iod.getVolumeOptions(volumeType,
                              dimensions,
                              voxelDataType,
                              byteSwap);
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         brainSet->importVtkStructuredPointsVolumeFile(fileName,
                                       volumeType);
         fm.setVolumeModified();
      }
   }
   else {
      return false;
   }
   
   //
   // Remove splash image
   //
   brainSet->setDisplaySplashImage(false);
   
   //
   // Update controls and redraw
   //
   theMainWindow->fileModificationUpdate(fm);
   if (fm.getCoordinateModified()) {
      theMainWindow->displayNewestSurfaceInMainWindow();
   }
   brainSet->getNodeColoring()->assignColors();
   
   GuiBrainModelOpenGL::updateAllGL(NULL);

   QApplication::restoreOverrideCursor(); 
   
   return true;
}
                      
/**
 * This static method provides one place to open data files and make the necessary
 * updates as a result.  Returns true if an error occurs.
 */
bool
GuiDataFileOpenDialog::openDataFile(QWidget* parentWidget, const QString specFileTag,
                      const QString& name, const bool append, const bool update,
                      QString& errorMessage, bool& relatedFileWarningFlag)
{
   const QString nameNoPath(FileUtilities::basename(name));
   
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
         catch (FileException& e) {
            throw FileException(name, "Unable to read border file to check type.\n"
                                      + e.whatQString());
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
                                              "Type of Borders for " + nameNoPath,
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
            throw FileException(name, "Unable to open coord file to check type.\n"
                                      + e.whatQString());
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
            QApplication::restoreOverrideCursor();
            QApplication::beep();
            QtRadioButtonSelectionDialog rbsd(parentWidget,
                                              "Choose Coordinate Type",
                                              "Type of Coordinates for " + nameNoPath,
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
      else if (specFileTag == GENERIC_SURFACE) {
         SurfaceFile sf;
         try {
            sf.readFileMetaDataOnly(name);
         }
         catch (FileException& e) {
            throw FileException(name, "Unable to open surface file to check type.\n"
                                      + e.whatQString());
         }

         BrainModelSurface::SURFACE_TYPES st = BrainModelSurface::SURFACE_TYPE_UNSPECIFIED;
         const QString typeTag = sf.getHeaderTag(AbstractFile::headerTagConfigurationID);
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
                                              "Choose Surface Type",
                                              "Type of Surface for " + nameNoPath,
                                              labels,
                                              -1);
            if (rbsd.exec() == QDialog::Accepted) {
               st = static_cast<BrainModelSurface::SURFACE_TYPES>(rbsd.getSelectedItemIndex());
            }
            else {
               throw FileException(name, "You must select the type of surface file.");
            }
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         theMainWindow->getBrainSet()->readSurfaceFile(name, st, false, append, update);
         fm.setCoordinateModified();
         fm.setTopologyModified();
         QApplication::restoreOverrideCursor();
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
            throw FileException(name, "Unable to read topology file to check type.\n"
                                      + e.whatQString());
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
                                              "Type of Topology for " + nameNoPath,
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
   
   QApplication::restoreOverrideCursor();
   if (areaColorWarning) {
      QMessageBox::warning(theMainWindow, "Warning",
                           "The file just opened requires area colors\n"
                           "but there are no area colors loaded.");
      relatedFileWarningFlag = true;
   }
   
   if (borderColorWarning) {
      QMessageBox::warning(theMainWindow, "Warning",
                           "The file just opened requires border colors\n"
                           "but there are no border colors loaded.");
      relatedFileWarningFlag = true;
   }
   
   if (cellColorWarning) {
      QMessageBox::warning(theMainWindow, "Warning",
                           "The file just opened requires cell colors\n"
                           "but there are no cell colors loaded.");
      relatedFileWarningFlag = true;
   }
   
   if (contourCellColorWarning) {
      QMessageBox::warning(theMainWindow, "Warning",
                           "The file just opened requires contour cell colors\n"
                           "but there are no contour cell colors loaded.");
      relatedFileWarningFlag = true;
   }
   
   if (fociColorWarning) {
      QMessageBox::warning(theMainWindow, "Warning",
                           "The file just opened requires foci colors\n"
                           "but there are no foci colors loaded.");
      relatedFileWarningFlag = true;
   }
   
   return false;
}

/**
 * copy a file to the current directory, returns true if successful.
 */
bool 
GuiDataFileOpenDialog::copyFile(const QString& name)
{
   const QString nameNoExt(FileUtilities::filenameWithoutExtension(name));
   QString volumeDataFileName;
   if (name.endsWith(SpecFile::getAnalyzeVolumeFileExtension())) {
      volumeDataFileName = nameNoExt + ".img";
   }
   else if (name.endsWith(SpecFile::getAfniVolumeFileExtension())) {
      volumeDataFileName = nameNoExt + ".BRIK";
   }
   else if (name.endsWith(SpecFile::getWustlVolumeFileExtension())) {
      volumeDataFileName = nameNoExt + ".img";
   }
   if (volumeDataFileName.isEmpty() == false) {
      if (QFile::exists(volumeDataFileName) == false) {
         volumeDataFileName += ".gz";
      }
   }
   
   if (FileUtilities::copyFile(name, FileUtilities::basename(name))) {
      QString msg("Error copying:\n");
      msg.append(name);
      msg.append("\nDoes the file exist ?");
      QMessageBox::critical(this, "ERROR", msg);
      return false;      
   }
   
   if (volumeDataFileName.isEmpty() == false) {
      if (FileUtilities::copyFile(volumeDataFileName,
                           FileUtilities::basename(volumeDataFileName))) {
         QString msg("Error copying:\n");
         msg.append(volumeDataFileName);
         msg.append("\nDoes the file exist ?");
         QMessageBox::critical(this, "ERROR", msg);
         return false;
      }
   }
   
   return true;
}      
      
      

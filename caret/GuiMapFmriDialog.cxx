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

#include <algorithm>
#include <iostream>
#include <sstream>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include "WuQFileDialog.h"
#include <QGroupBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QToolTip>

#include "BrainModelVolumeToSurfaceMapper.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GaussianComputation.h"
#include "GuiCopySpecFileDialog.h"
#include "GuiFileDialogWithInstructions.h"
#include "GuiMainWindow.h"
#include "GuiMapFmriAtlasDialog.h"
#include "GuiMapFmriDialog.h"
#include "GuiMapFmriSpecFileTopoCoordDialog.h"
#include "GuiMapFmriThresholdDialog.h"
#include "GuiSumsDialog.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "StringUtilities.h"
#include <QDoubleSpinBox>
#include "QtListBoxSelectionDialog.h"
#include "QtTextEditDialog.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiMapFmriDialog::GuiMapFmriDialog(QWidget* parent,
                                   const bool runningAsPartOfCaretIn,
                                   bool modalFlag,
                                   Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   setModal(modalFlag);
   dataMappingType = DATA_MAPPING_TYPE_NONE;
   
   readAtlases();
   
   setWindowTitle("Map Volumes To Surfaces");
   
   runningAsPartOfCaret = runningAsPartOfCaretIn;
   
   //
   // Label for title of current page
   //
   currentPageTitleLabel = new QLabel;
   
   //
   // Stacked widget for all pages
   //
   pagesStackedWidget = new QStackedWidget;
   
   pageDataMappingType = createDataMappingTypePage();
   addPage(pageDataMappingType, "Data Mapping Type");
   
   pageVolumeSelection = createVolumeSelectionPage();
   addPage(pageVolumeSelection, "Volume Selection");
   
   pageSpecFileAndSurfaceSelection = createSpecFileAndSurfaceSelectionPage();
   addPage(pageSpecFileAndSurfaceSelection, "Spec File And Surface Selection");
   
   pageMetricNaming = createMetricNamingPage();
   addPage(pageMetricNaming, "Data File Naming");
   
   pageAlgorithm = createAlgorithmPage();
   addPage(pageAlgorithm, "Mapping Algorithm");
   
   pageSummary = createSummaryPage();
   addPage(pageSummary, "Summary");
   
   //
   // Back page push button
   //
   QPushButton* backPushButton = new QPushButton("Back");
   backPushButton->setAutoDefault(false);
   QObject::connect(backPushButton, SIGNAL(clicked()),
                    this, SLOT(slotBack()));
                    
   //
   // Next push button
   //
   nextPushButton = new QPushButton("Next");
   nextPushButton->setAutoDefault(true);
   QObject::connect(nextPushButton, SIGNAL(clicked()),
                    this, SLOT(slotNext()));
                    
   //
   // Close push button
   //
   QPushButton* closePushButton = new QPushButton("Close");
   closePushButton->setAutoDefault(true);
   QObject::connect(closePushButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseOrCancelButton()));
                    
   QtUtilities::makeButtonsSameSize(backPushButton,
                                    nextPushButton,
                                    closePushButton);
                                    
   if (runningAsPartOfCaret) {
      //
      // Make the Cancel button the Quit button
      //
      closePushButton->setText("Close");
   }
   else {
      //
      // Make the Cancel button the Quit button
      //
      closePushButton->setText("Quit");
   }
   QObject::connect(closePushButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseOrCancelButton()));
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(backPushButton);
   buttonsLayout->addWidget(nextPushButton);
   buttonsLayout->addWidget(closePushButton);

   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(currentPageTitleLabel);
   dialogLayout->addWidget(pagesStackedWidget);
   dialogLayout->addLayout(buttonsLayout);
   
   slotSetAppropriatePages();
   
   //
   // Start with the data mapping type selection page (skips copy/download spec page)
   //
   showPage(pageDataMappingType);
}
                 
/**
 * Destructor.
 */
GuiMapFmriDialog::~GuiMapFmriDialog()
{
   for (int i = 0; i < static_cast<int>(volumesToBeMapped.size()); i++) {
      delete volumesToBeMapped[i];
      volumesToBeMapped[i] = NULL;
   }
   resetMappingSets();
}

/**
 * Called when close/cancel button pressed.
 */
void
GuiMapFmriDialog::slotCloseOrCancelButton()
{
   if (runningAsPartOfCaret == false) {
      if (QMessageBox::warning(this, 
              "Confirm", 
              "Are you sure you want to quit ?", 
              (QMessageBox::Yes | QMessageBox::No),
              QMessageBox::Yes)
                 == QMessageBox::No) {
         return;
      }
   }
   
   WuQDialog::close();
}

/**
 * Overrides parents close method.
 */
void
GuiMapFmriDialog::done(int r)
{
   //
   // Do nothing - overriding this method prevents the window from closing
   // when the Finish button is pressed.
   //
   WuQDialog::done(r);
}

/**
 * Initialize the dialog (resets to first page and clears everything).
 */
void 
GuiMapFmriDialog::initializeDialog()
{
   atlasSpecFileInfo.clear();
   volumesToBeMapped.clear();
   mappingSets.clear();
   readAtlases();
   mappingTypeFunctionalRadioButton->setChecked(false);
   mappingTypePaintRadioButton->setChecked(false);
   mappingTypeFunctionalRadioButton->setChecked(true);
   dataMappingType = DATA_MAPPING_TYPE_METRIC;
   slotDataMappingTypeButtGroup(dataMappingType);   
   showPage(pageDataMappingType);
}
      
/**
 * add a page to the dialog.
 */
void 
GuiMapFmriDialog::addPage(QWidget* pageIn, const QString& pageTitleIn)
{
   pagesStackedWidget->addWidget(pageIn);
   pageTitles.push_back(pageTitleIn);
}
      
/**
 * Called to select the next page.
 */
void
GuiMapFmriDialog::slotNext()
{
   //
   // If the current page is volume selection
   //
   if (pagesStackedWidget->currentWidget() == pageVolumeSelection) {
   }
   
   //
   // If the current page is the metric naming page
   //
   if (pagesStackedWidget->currentWidget() == pageMetricNaming) {
      //
      // Read the metric naming page
      //
      slotReadMetricNamingPage();
   }
   
   //
   // If the current page is the algorithm page
   //
   if (pagesStackedWidget->currentWidget() == pageAlgorithm) {
   }
   
   //
   // Determine and show the next page
   //
   int nextIndx = pagesStackedWidget->currentIndex() + 1;
   if (nextIndx < pagesStackedWidget->count()) {
      QWidget* nextWidget = pagesStackedWidget->widget(nextIndx);
      if (nextWidget == pageAlgorithm) {
         switch (dataMappingType) {
            case DATA_MAPPING_TYPE_NONE:
               break;
            case DATA_MAPPING_TYPE_METRIC:
               break;
            case DATA_MAPPING_TYPE_PAINT:
               nextIndx++;
               nextWidget = pagesStackedWidget->widget(nextIndx);
               break;
         }
      }
      showPage(nextWidget);
   }
   else {
      mapVolumesToSurfaces();
   }
}

/**
 * Called to select the previous page.
 */
void
GuiMapFmriDialog::slotBack()
{
   //
   // If the current page is the metric naming page
   //
   if (pagesStackedWidget->currentWidget() == pageMetricNaming) {
      //
      // Read the metric naming page
      //
      slotReadMetricNamingPage();
   }
   
   //
   // If the current page is the algorithm page
   //
   if (pagesStackedWidget->currentWidget() == pageAlgorithm) {
   }
   
   //
   // Determine and show the next page
   //
   int backIndx = pagesStackedWidget->currentIndex() - 1;
   if (backIndx >= 0) {
      QWidget* backWidget = pagesStackedWidget->widget(backIndx);
      if (backWidget == pageAlgorithm) {
         switch (dataMappingType) {
            case DATA_MAPPING_TYPE_NONE:
               break;
            case DATA_MAPPING_TYPE_METRIC:
               break;
            case DATA_MAPPING_TYPE_PAINT:
               backIndx--;
               backWidget = pagesStackedWidget->widget(backIndx);
               break;
         }
      }
      showPage(backWidget);
   }
}

/**
 * Prepare some pages when they are about to be shown
 */
void
GuiMapFmriDialog::showPage(QWidget* page)
{
   nextPushButton->setText("Next");
   
   //
   // If the page about to be shown is the volume selection page
   //
   if (page == pageVolumeSelection) {
      //
      // Load the selected volumes into the list box
      //
      loadVolumesListBox();
   }
   
   //
   // If the page about to be shown is the mapping surface selection page
   //
   if (page == pageSpecFileAndSurfaceSelection) {
      //
      // Disable buttons if atlas or caret surfaces not available
      //
      std::vector<BrainModelSurface*> surfaces;
      bool haveAnySurface = false;
      getFiducialSurfaces(surfaces, haveAnySurface);
      const bool haveCaretFiducialSurfaces = (surfaces.empty() == false);
      addCaretSurfacesPushButton->setEnabled(haveCaretFiducialSurfaces);
      addCaretMapWithAtlasPushButton->setEnabled(haveAnySurface && haveAtlases());
      addSpecAtlasPushButton->setEnabled(haveAtlases());

      //
      // Load the mapping sets
      //
      loadMappingSetsListBox();
   }
   
   //
   // If page about to be shown is metric naming
   //
   if (page == pageMetricNaming) {
      //
      // load the metric naming combo box
      //
      loadMetricNamingComboBox();
      
      //
      // Load the metric naming data
      //
      slotLoadMetricNamingPage();
   }
   
   //
   // If page about to be shown is the algorithm page
   //
   if (page == pageAlgorithm) {
      //
      // Load the algorithm parameters into the widgets
      //
      loadAlgorithmParametersFromPreferences();
   }
   
   //
   // If page about to be shown is the summary page
   //
   if (page == pageSummary) {
      //
      // Load the text edit in the summary page
      //
      loadSummaryTextEdit();
      nextPushButton->setText("Finish");
   }
   else {
      nextPushButton->setEnabled(false);
   }
   
   pagesStackedWidget->setCurrentWidget(page);
   const int indx = pagesStackedWidget->currentIndex();
   currentPageTitleLabel->setText("<B>" + pageTitles[indx] + "</B>");
   
   slotSetAppropriatePages();
}

/**
 * Read the atlases
 */
void
GuiMapFmriDialog::readAtlases()
{
   //
   // Look for atlas spec files in Caret installation directory.
   //
   atlasFilesDirectory = theMainWindow->getBrainSet()->getCaretHomeDirectory();
   atlasFilesDirectory.append("/data_files/fmri_mapping_files");
   std::vector<QString> files;
   QString fileExt("*");
   fileExt.append(SpecFile::getSpecFileExtension());
   FileUtilities::findFilesInDirectory(atlasFilesDirectory,
                                       QStringList(fileExt),
                                       files);
   
   //
   // Process each of the atlas spec files
   //
   for (int i = 0; i < static_cast<int>(files.size()); i++) {
      //
      // Prepend path to name of atlas spec file
      //
      QString name(atlasFilesDirectory);
      name.append("/");
      name.append(files[i]);
      
      //
      // See if spec files is valid
      //
      MapFmriAtlasSpecFileInfo asfi(name);
      if (asfi.getDataValid()) {
         atlasSpecFileInfo.push_back(asfi);
      }
      else {
         std::cout << "WARNING: invalid atlas spec file: "
                   << name.toAscii().constData() << std::endl;
      }
   }
   
   //
   // Sort atlas spec file info
   //
   std::sort(atlasSpecFileInfo.begin(), atlasSpecFileInfo.end());
}

/**
 * Set the pages that are valid for viewing according to the current selections.
 */
void
GuiMapFmriDialog::slotSetAppropriatePages()
{
   bool nextButtonEnabled = true;
   
   QWidget* currentWidget = pagesStackedWidget->currentWidget();
   
   //
   // Only allow next on data mapping if a type has been selected
   //
   if (currentWidget == pageDataMappingType) {
      nextButtonEnabled = (dataMappingType != DATA_MAPPING_TYPE_NONE);
   }
   
   //
   // Functional volume page next button is valid if volumes selected
   //
   if (currentWidget == pageVolumeSelection) {
      nextButtonEnabled = (volumesToBeMapped.empty() == false);
   }
   
   //
   // Mapping sets page next button is valid if caret surfaces/spec files selected
   //
   if (currentWidget == pageSpecFileAndSurfaceSelection) {
      nextButtonEnabled = (mappingSets.empty() == false);
   }
   
   nextPushButton->setEnabled(nextButtonEnabled);
}

/**
 * Called to map functional volume(s) to the surface(s).
 */
void 
GuiMapFmriDialog::mapVolumesToSurfaces()
{
   nextPushButton->setEnabled(false);
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // Hide this dialog
   //
   this->hide();
   
   //
   // Count number of steps for progress dialog
   //
   int progressSteps = 0;
   for (int i = 0; i < static_cast<int>(mappingSets.size()); i++) {
      const int num = mappingSets[i].getNumberOfMetricMappingInfo();
      for (int j = 0; j < num; j++) {
         progressSteps += mappingSets[i].getMetricMappingInfo(j)->getNumberOfMetricColumns();
      }
      if (mappingSets[i].getDoAverageFiducialFileFlag()) {
         progressSteps++;
      }
      if (mappingSets[i].getDoAvgOfAllCoordFileFlag() ||
          mappingSets[i].getDoMostCommonOfAllCasesFlag() ||
          mappingSets[i].getDoMostCommonExcludeUnidentifiedOfAllCasesFlag() ||
          mappingSets[i].getDoStdDevOfAllCoordFileFlag() ||
          mappingSets[i].getDoStdErrorOfAllCoordFileFlag() ||
          mappingSets[i].getDoMinOfAllCoordFileFlag() ||
          mappingSets[i].getDoMaxOfAllCoordFileFlag()) {
         progressSteps++;
      }
   }
   
   //
   // Create a progress dialog 
   //
   QWidget* progressParent = parentWidget();
   QProgressDialog progressDialog(progressParent);
   progressDialog.move(progressParent->pos());
   progressDialog.setMaximum(progressSteps + 1);
   progressDialog.setValue(0);
   progressDialog.setWindowTitle("Map Volumes");
   progressDialog.show();
   
   //
   // Save the current directory
   //
   const QString curDir(QDir::currentPath());
   
   //
   // Map the data
   //
   switch (dataMappingType) {
      case DATA_MAPPING_TYPE_NONE:
         break;
      case DATA_MAPPING_TYPE_METRIC:
         mapDataToMetricFiles(progressDialog);
         break;
      case DATA_MAPPING_TYPE_PAINT:
         mapDataToPaintFiles(progressDialog);
         break;
   }
   
   //
   // Restore the current directory
   //
   QDir::setCurrent(curDir);
   
   QApplication::restoreOverrideCursor();
   
   progressDialog.cancel();
   
   //
   // show this dialog
   //
   show();
   
   QApplication::beep();
   
   emit signalMappingComplete();
}

/**
 * map the volumes to paint files.
 */
void 
GuiMapFmriDialog::mapDataToPaintFiles(QProgressDialog& progressDialog)
{
   //
   // For each mapping set
   //
   for (int i = 0; i < static_cast<int>(mappingSets.size()); i++) {
      const QString savedDirectory(QDir::currentPath());
      
      //
      // Get the mapping set
      //
      GuiMapFmriMappingSet& mms = mappingSets[i];

      //
      // Get the number of items in the set
      //
      const int numMappingInfo = mms.getNumberOfMetricMappingInfo();
      
      //
      // Create the output paint file
      //
      PaintFile outputPaintFile;
      
      //
      // For each set of mapping info
      //
      for (int j = 0; j < numMappingInfo; j++) {
      
         //
         // Save the current directory
         //
         const QString savedDirectory(QDir::currentPath());
      
         //
         // Get the topo file name, coord file names, and caret surfaces
         //      
         const QString mappingTopoFileName(mms.getMappingTopoFileName());
         std::vector<QString> mappingCoordFileNames;
         mms.getMappingCoordFileNames(mappingCoordFileNames);
         std::vector<BrainModelSurface*> mappingCaretSurfaces;
         mms.getMappingCaretSurfaces(mappingCaretSurfaces);
      
         //
         // Get the metric info
         //
         const GuiMapFmriMetricInfo* metricInfo = mms.getMetricMappingInfo(j);

         //
         // Create a metric file for mapping the data
         //
         PaintFile mappingPaintFile;
      
         //
         // Do each column in the metric info
         //
         const int numberOfColumns = metricInfo->getNumberOfMetricColumns();
         for (int metricColumn = 0; metricColumn < numberOfColumns; metricColumn++) {
            //
            // There is one metric column for each surface
            //
            const int surfaceIndex = metricColumn;
            
            //
            // Get volume information
            //
            const int volumeIndex = metricInfo->getVolumeIndex();
            const int subVolumeIndex = metricInfo->getSubVolumeIndex();
            
            //
            // Update the progress dialog
            //
            qApp->processEvents();  // note: qApp is global in QApplication
            if (progressDialog.wasCanceled()) {
               return;
            }
            QString progressMessage("Mapping Volume: ");
            progressMessage.append(volumesToBeMapped[volumeIndex]->getDescriptiveName());
            progressMessage.append("\nSub-Volume: ");
            progressMessage.append(volumesToBeMapped[volumeIndex]->getSubVolumeName(subVolumeIndex));
            progressMessage.append(" to \n");
            progressMessage.append(metricInfo->getSurfaceNameForMetricColumn(metricColumn));
            progressMessage.append(".");
            progressDialog.setLabelText(progressMessage);
            progressDialog.setValue(progressDialog.value() + 1);
            progressDialog.show();
            
            //
            // Get the brain set and surface for mapping the data
            //
            BrainSet* bs = NULL;
            BrainModelSurface* bms = NULL;
            bool createdBrainSet = false;

            //
            // Set/Load the brain set and the brain model surface
            //
            switch (mms.getMappingType()) {
               case GuiMapFmriMappingSet::MAPPING_TYPE_NONE:
                  break;
               case GuiMapFmriMappingSet::MAPPING_TYPE_WITH_CARET_SURFACES:
                  bs = theMainWindow->getBrainSet();
                  bms = mappingCaretSurfaces[surfaceIndex];
                  break;
               case GuiMapFmriMappingSet::MAPPING_TYPE_WITH_SPEC_FILES:
                  {
                     //
                     // Set to the spec file directory
                     //
                     const QString directory(mms.getMappingFilesPath());
                     if (directory.isEmpty() == false) {
                        QDir::setCurrent(directory);
                     }
                     
                     // 
                     // Create a spec file and initialize it for mapping files
                     //
                     SpecFile sf;
                     sf.setTopoAndCoordSelected(mappingTopoFileName, 
                                                mappingCoordFileNames[surfaceIndex],
                                                sf.getStructure());
                     
                     //
                     // Create the brain set
                     //
                     bs = new BrainSet;
                     createdBrainSet = true;

                     //
                     // Load the brain set
                     //
                     std::vector<QString> errorMessages;
                     bs->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                      sf, "mapping_spec", errorMessages, NULL, NULL);
                     if (errorMessages.empty() == false) {
                        progressDialog.cancel();
                        QString msg("Error creating BrainSet for mapping data.\n");
                        msg.append(StringUtilities::combine(errorMessages, "\n"));
                        QApplication::restoreOverrideCursor();
                        QMessageBox::critical(this, "ERROR", msg, "OK");
                        return;
                     }
                     
                     //
                     // If spec file read correctly, there is one brain model
                     //
                     if (bs->getNumberOfBrainModels() == 1) {
                        //
                        // Get the surface
                        //
                        bms = bs->getBrainModelSurface(0);
                        
                        //
                        // Make sure valid
                        //
                        if (bms == NULL) {
                           progressDialog.cancel();
                           QString msg("No brain model surface after reading ");
                           msg.append(mappingTopoFileName);
                           msg.append(" and ");
                           msg.append(mappingCoordFileNames[surfaceIndex]);
                           QApplication::restoreOverrideCursor();
                           QMessageBox::critical(this, "ERROR", msg);
                           delete bs;
                           return;
                        }
                     }
                     else {
                        progressDialog.cancel();
                        QString msg("No brain model after reading ");
                        msg.append(mappingTopoFileName);
                        msg.append(" and ");
                        msg.append(mappingCoordFileNames[surfaceIndex]);
                        QApplication::restoreOverrideCursor();
                        QMessageBox::critical(this, "ERROR", msg);
                        delete bs;
                        return;
                     }
                  }
                  break;
            }
      
            //
            // Allocate number of nodes and columns for metric file
            //
            if (mappingPaintFile.getNumberOfNodes() == 0) {
               mappingPaintFile.setNumberOfNodesAndColumns(bs->getNumberOfNodes(), numberOfColumns);
            }
            
            //
            // Volume data that is to be mapped
            //
            VolumeFile* vf = NULL;
            bool createdVolumeFile = false;
            
            //
            // Get info about volume that is to be mapped
            //
            GuiMapFmriVolume* fmriVolume = volumesToBeMapped[volumeIndex];
            switch(fmriVolume->getFmriVolumeType()) {
               case GuiMapFmriVolume::FMRI_VOLUME_TYPE_FILE_ON_DISK:
                  //
                  // Read in the volume data
                  //
                  vf = new VolumeFile;
                  createdVolumeFile = true;
                  try {
                     vf->readFile(fmriVolume->getVolumeFileName(),
                                  subVolumeIndex);
                  }
                  catch (FileException& e) {
                  }
                  break;
               case GuiMapFmriVolume::FMRI_VOLUME_TYPE_FILE_IN_MEMORY:
                  vf = fmriVolume->getInMemoryVolumeFile();
                  break;
            }
         
            //
            // Create the mapper class
            //
            mappingParameters.setAlgorithm(
               BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_PAINT_ENCLOSING_VOXEL);
            BrainModelVolumeToSurfaceMapper bmvsmm(
               bs,
               bms,
               vf,
               &mappingPaintFile,
               mappingParameters,
               metricColumn,
               metricInfo->getMetricColumnName(metricColumn));
         
            //
            // Run the mapper
            //
            try {
               bmvsmm.execute();
            }
            catch (BrainModelAlgorithmException& e) {
               progressDialog.cancel();
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", e.whatQString());
               if (createdBrainSet) {
                  delete bs;
               }
               return;
            }
         
            //
            // Prepend user's column comment since mapper overwrites with mapping parameters
            //
            QString columnComment(metricInfo->getMetricColumnComment(metricColumn));
            if (columnComment.isEmpty() == false) {
               columnComment.append("\n");
               mappingPaintFile.prependToColumnComment(metricColumn, columnComment);
            }
            
            //
            // Free up the volume file if needed
            //
            if (createdVolumeFile) {
               delete vf;
               vf = NULL;
            }
            
            //
            // Free up the brain set if it was created
            //
            if (createdBrainSet) {
               delete bs;
               bs = NULL;
            }
         }  // for (int metricColumn
      
         //
         // Do most common if needed
         //
         if (mms.getDoMostCommonOfAllCasesFlag() ||
             mms.getDoMostCommonExcludeUnidentifiedOfAllCasesFlag()) {
            
            //
            // Update progress dialog
            //
            QString progressMessage("Generating ");
            QString mostCommonName;
            if (mms.getDoMostCommonOfAllCasesFlag()) {
               mostCommonName = metricInfo->getMetricMostCommonValueColumnName();
               progressMessage.append("\nMost Common ");
            }
            progressDialog.setLabelText(progressMessage);
            progressDialog.show();
            
            QString mostCommonExcludeUnidentifiedName;
            if (mms.getDoMostCommonExcludeUnidentifiedOfAllCasesFlag()) {
               mostCommonExcludeUnidentifiedName = metricInfo->getMetricMostCommonExcludeUnidentifiedValueColumnName();
               progressMessage.append("\nMost Common Exclude No ID");
            }
            progressDialog.setLabelText(progressMessage);
            progressDialog.show();
            
            //
            // Find most common
            //
            mappingPaintFile.appendMostCommon(mostCommonName,
                                              mostCommonExcludeUnidentifiedName);
                                              
            //
            // Add comments
            //
            if (mostCommonName.isEmpty() == false) {
               const int mostCommonColumn = mappingPaintFile.getColumnWithName(mostCommonName);
               if (mostCommonColumn >= 0) {
                  mappingPaintFile.prependToColumnComment(mostCommonColumn,
                                                          metricInfo->getMetricMostCommonValueComment());
               }
            }
            if (mostCommonExcludeUnidentifiedName.isEmpty() == false) {
               const int mostCommonExcludeUnidentifiedColumn = mappingPaintFile.getColumnWithName(mostCommonExcludeUnidentifiedName);
               if (mostCommonExcludeUnidentifiedColumn >= 0) {
                  mappingPaintFile.prependToColumnComment(mostCommonExcludeUnidentifiedColumn,
                                                          metricInfo->getMetricMostCommonExcludeUnidentifiedValueComment());
               }
            }
         }  // map most common
         
         //
         // Map to Average Fiducial Surface
         //
         PaintFile averageFiducialPaintFile;
         if (mms.getDoAverageFiducialFileFlag()) {
            //
            // Get volume information
            //
            const int volumeIndex = metricInfo->getVolumeIndex();
            const int subVolumeIndex = metricInfo->getSubVolumeIndex();
            
            //
            // Update the progress dialog
            //
            qApp->processEvents();  // note: qApp is global in QApplication
            if (progressDialog.wasCanceled()) {
               return;
            }
            QString progressMessage("Mapping Volume: ");
            progressMessage.append(volumesToBeMapped[volumeIndex]->getDescriptiveName());
            progressMessage.append("\nSub-Volume: ");
            progressMessage.append(volumesToBeMapped[volumeIndex]->getSubVolumeName(subVolumeIndex));
            progressMessage.append(" to \n");
            progressMessage.append(metricInfo->getMetricAverageFiducialCoordColumnName());
            progressMessage.append(".");
            progressDialog.setLabelText(progressMessage);
            progressDialog.setValue(progressDialog.value() + 1);
            progressDialog.show();
            
            //
            // Set to the spec file directory
            //
            const QString directory(mms.getMappingFilesPath());
            if (directory.isEmpty() == false) {
               QDir::setCurrent(directory);
            }
            
            // 
            // Create a spec file and initialize it for mapping files
            //
            SpecFile sf;
            sf.setTopoAndCoordSelected(mappingTopoFileName, 
                                       mms.getAverageFiducialCoordFileName(),
                                       sf.getStructure());
            
            //
            // Create the brain set
            //
            BrainSet* bs = new BrainSet;
            BrainModelSurface* bms = NULL;

            //
            // Load the brain set
            //
            std::vector<QString> errorMessages;
            bs->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                             sf, "mapping_spec", errorMessages, NULL, NULL);
            if (errorMessages.empty() == false) {
               progressDialog.cancel();
               QString msg("Error creating BrainSet for mapping data.\n");
               msg.append(StringUtilities::combine(errorMessages, "\n"));
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", msg);
               return;
            }
            
            //
            // If spec file read correctly, there is one brain model
            //
            if (bs->getNumberOfBrainModels() == 1) {
               //
               // Get the surface
               //
               bms = bs->getBrainModelSurface(0);
               
               //
               // Make sure valid
               //
               if (bms == NULL) {
                  progressDialog.cancel();
                  QString msg("No brain model surface after reading ");
                  msg.append(mappingTopoFileName);
                  msg.append(" and ");
                  msg.append(mms.getAverageFiducialCoordFileName());
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", msg);
                  delete bs;
                  return;
               }
            }
            else {
               progressDialog.cancel();
               QString msg("No brain model after reading ");
               msg.append(mappingTopoFileName);
               msg.append(" and ");
               msg.append(mms.getAverageFiducialCoordFileName());
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", msg);
               delete bs;
               return;
            }
      
            //
            // Allocate number of nodes and columns for paint file
            //
            averageFiducialPaintFile.setNumberOfNodesAndColumns(bs->getNumberOfNodes(), 1);
            
            //
            // Volume data that is to be mapped
            //
            VolumeFile* vf = NULL;
            bool createdVolumeFile = false;
            
            //
            // Get info about volume that is to be mapped
            //
            GuiMapFmriVolume* fmriVolume = volumesToBeMapped[volumeIndex];
            switch(fmriVolume->getFmriVolumeType()) {
               case GuiMapFmriVolume::FMRI_VOLUME_TYPE_FILE_ON_DISK:
                  //
                  // Read in the volume data
                  //
                  vf = new VolumeFile;
                  createdVolumeFile = true;
                  try {
                     vf->readFile(fmriVolume->getVolumeFileName(),
                                  subVolumeIndex);
                  }
                  catch (FileException& e) {
                  }
                  break;
               case GuiMapFmriVolume::FMRI_VOLUME_TYPE_FILE_IN_MEMORY:
                  vf = fmriVolume->getInMemoryVolumeFile();
                  break;
            }
         
            //
            // Create the mapper class
            //
            mappingParameters.setAlgorithm(
               BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_PAINT_ENCLOSING_VOXEL);
            BrainModelVolumeToSurfaceMapper bmvsmm(
               bs,
               bms,
               vf,
               &averageFiducialPaintFile,
               mappingParameters,
               0,
               metricInfo->getMetricAverageFiducialCoordColumnName());
         
            //
            // Run the mapper
            //
            try {
               bmvsmm.execute();
            }
            catch (BrainModelAlgorithmException& e) {
               progressDialog.cancel();
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", e.whatQString());
               delete bs;
               return;
            }
         
            //
            // Prepend user's column comment since mapper overwrites with mapping parameters
            //
            QString columnComment(metricInfo->getMetricAverageFiducialCoordComment());
            if (columnComment.isEmpty() == false) {
               columnComment.append("\n");
               averageFiducialPaintFile.prependToColumnComment(0, columnComment);
            }
            
            //
            // Free up the volume file if needed
            //
            if (createdVolumeFile) {
               delete vf;
               vf = NULL;
            }
            
            //
            // Free up the brain set
            //
            delete bs;
            bs = NULL;
            
            //
            // Add onto output paint file
            //
            outputPaintFile.append(averageFiducialPaintFile);
         }
         
         //
         // Append most common columns first so that they are before indiv cases
         //
         bool haveMostCommonCases = false;
         std::vector<int> paintColumnDestination(mappingPaintFile.getNumberOfColumns(),
                                                  MetricFile::APPEND_COLUMN_NEW);
         for (int n = 0; n < mappingPaintFile.getNumberOfColumns(); n++) {
            if (n >= metricInfo->getNumberOfMetricColumns()) {
               paintColumnDestination[n] = PaintFile::APPEND_COLUMN_NEW;
               haveMostCommonCases = true;
            }
            else {
               paintColumnDestination[n] = PaintFile::APPEND_COLUMN_DO_NOT_LOAD;
            }
         }
         if (haveMostCommonCases) {
            outputPaintFile.append(mappingPaintFile, paintColumnDestination,
                                    PaintFile::FILE_COMMENT_MODE_APPEND);
         }
         
         //
         // Append indiv cases.
         //
         if (mms.getDoAllCasesCoordFileFlag()) {
            for (int n = 0; n < mappingPaintFile.getNumberOfColumns(); n++) {
               if (n < metricInfo->getNumberOfMetricColumns()) {
                  paintColumnDestination[n] = PaintFile::APPEND_COLUMN_NEW;
               }
               else {
                  paintColumnDestination[n] = PaintFile::APPEND_COLUMN_DO_NOT_LOAD;
               }
            }
            NodeAttributeFile::FILE_COMMENT_MODE fcm = PaintFile::FILE_COMMENT_MODE_APPEND;
            if (haveMostCommonCases) {
               fcm = MetricFile::FILE_COMMENT_MODE_LEAVE_AS_IS;
            }
            outputPaintFile.append(mappingPaintFile, paintColumnDestination, fcm);
         }
      } // number of metric mapping info
   
      //
      // Update Caret/Spec file with new metric data
      //
      switch (mms.getMetricOutputType()) {
         case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_NONE:
            break;
         case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_CARET_METRIC:
            //
            // Append to existing metrics
            //
            {
               PaintFile* currentPaintFile = theMainWindow->getBrainSet()->getPaintFile();
               try {
                  currentPaintFile->append(outputPaintFile);
               }
               catch (FileException& e) {
                  progressDialog.cancel();
                  QString msg("Data mapped but unable to append to current paint file.\n");
                  msg.append(e.whatQString());
                  QMessageBox::critical(this, "ERROR", msg);
                  return;
               }
            }
            break;
         case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_SPEC_FILE:
            {
               //
               //  Change to the spec file's directory
               //
               const QString directory(FileUtilities::dirname(mms.getOutputSpecFileName()));
               if (directory.isEmpty() == false) {
                  QDir::setCurrent(directory);
               }
               
               //
               // Metric file is always written in same directory as spec file
               // so chop off path
               //
               const QString metricName(FileUtilities::basename(mms.getMetricFileName()));
               
               //
               // Save the metric file
               //
               try {
                  outputPaintFile.writeFile(metricName);
               }
               catch (FileException& e) {
                  progressDialog.cancel();
                  QString msg("Unable to save paint file ");
                  msg.append(metricName);
                  msg.append("\n");
                  msg.append(e.whatQString());
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", msg);
                  return;
               }    
               
               //
               // Update the spec file
               //
               SpecFile sf;
               try {
                  //
                  // Read the spec file for updating
                  //
                  sf.readFile(mms.getOutputSpecFileName());

                  //
                  // Add the metric file to the spec file
                  //
                  sf.addToSpecFile(SpecFile::getPaintFileTag(), metricName, "", false);
                  
                  //
                  // Write the spec file
                  //
                  sf.writeFile(mms.getOutputSpecFileName());
                  
                  //
                  // Add the spec file to the recent spec files in preferences
                  //
                  PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
                  pf->addToRecentSpecFiles(mms.getOutputSpecFileName(), true);
               }
               catch (FileException& e) {
                  progressDialog.cancel();
                  QString msg("Unable to update spec file: ");
                  msg.append(mms.getOutputSpecFileName());
                  msg.append("\n");
                  msg.append(e.whatQString());
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", msg);
                  return;
               }
            }
            break;
      }
      
      //
      // Restore directory
      //
      QDir::setCurrent(savedDirectory);
   }  // for each mapping set
}      
   
/**
 * map the volumes to metric files.
 */
void 
GuiMapFmriDialog::mapDataToMetricFiles(QProgressDialog& progressDialog)
{   
   //
   // For each mapping set
   //
   for (int i = 0; i < static_cast<int>(mappingSets.size()); i++) {
      const QString savedDirectory(QDir::currentPath());
      
      //
      // Get the mapping set
      //
      GuiMapFmriMappingSet& mms = mappingSets[i];

      //
      // Get the number of items in the set
      //
      const int numMappingInfo = mms.getNumberOfMetricMappingInfo();
      
      //
      // Create the output metric file
      //
      MetricFile outputMetricFile;
      
      //
      // For each set of mapping info
      //
      for (int j = 0; j < numMappingInfo; j++) {
      
         //
         // Save the current directory
         //
         const QString savedDirectory(QDir::currentPath());
      
         //
         // Get the topo file name, coord file names, and caret surfaces
         //      
         const QString mappingTopoFileName(mms.getMappingTopoFileName());
         std::vector<QString> mappingCoordFileNames;
         mms.getMappingCoordFileNames(mappingCoordFileNames);
         std::vector<BrainModelSurface*> mappingCaretSurfaces;
         mms.getMappingCaretSurfaces(mappingCaretSurfaces);
      
         //
         // Get the metric info
         //
         const GuiMapFmriMetricInfo* metricInfo = mms.getMetricMappingInfo(j);

         //
         // Create a metric file for mapping the data
         //
         MetricFile mappingMetricFile;
      
         //
         // Do each column in the metric info
         //
         const int numberOfColumns = metricInfo->getNumberOfMetricColumns();
         for (int metricColumn = 0; metricColumn < numberOfColumns; metricColumn++) {
            //
            // There is one metric column for each surface
            //
            const int surfaceIndex = metricColumn;
            
            //
            // Get volume information
            //
            const int volumeIndex = metricInfo->getVolumeIndex();
            const int subVolumeIndex = metricInfo->getSubVolumeIndex();
            
            //
            // Update the progress dialog
            //
            qApp->processEvents();  // note: qApp is global in QApplication
            if (progressDialog.wasCanceled()) {
               return;
            }
            QString progressMessage("Mapping Volume: ");
            progressMessage.append(volumesToBeMapped[volumeIndex]->getDescriptiveName());
            progressMessage.append("\nSub-Volume: ");
            progressMessage.append(volumesToBeMapped[volumeIndex]->getSubVolumeName(subVolumeIndex));
            progressMessage.append(" to \n");
            progressMessage.append(metricInfo->getSurfaceNameForMetricColumn(metricColumn));
            progressMessage.append(".");
            progressDialog.setLabelText(progressMessage);
            progressDialog.setValue(progressDialog.value() + 1);
            progressDialog.show();
            
            //
            // Get the brain set and surface for mapping the data
            //
            BrainSet* bs = NULL;
            BrainModelSurface* bms = NULL;
            bool createdBrainSet = false;

            //
            // Set/Load the brain set and the brain model surface
            //
            switch (mms.getMappingType()) {
               case GuiMapFmriMappingSet::MAPPING_TYPE_NONE:
                  break;
               case GuiMapFmriMappingSet::MAPPING_TYPE_WITH_CARET_SURFACES:
                  bs = theMainWindow->getBrainSet();
                  bms = mappingCaretSurfaces[surfaceIndex];
                  break;
               case GuiMapFmriMappingSet::MAPPING_TYPE_WITH_SPEC_FILES:
                  {
                     //
                     // Set to the spec file directory
                     //
                     const QString directory(mms.getMappingFilesPath());
                     if (directory.isEmpty() == false) {
                        QDir::setCurrent(directory);
                     }
                     
                     // 
                     // Create a spec file and initialize it for mapping files
                     //
                     SpecFile sf;
                     sf.setTopoAndCoordSelected(mappingTopoFileName, 
                                                mappingCoordFileNames[surfaceIndex],
                                                sf.getStructure());
                     
                     //
                     // Create the brain set
                     //
                     bs = new BrainSet;
                     createdBrainSet = true;

                     //
                     // Load the brain set
                     //
                     std::vector<QString> errorMessages;
                     bs->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                      sf, "mapping_spec", errorMessages, NULL, NULL);
                     if (errorMessages.empty() == false) {
                        progressDialog.cancel();
                        QString msg("Error creating BrainSet for mapping data.\n");
                        msg.append(StringUtilities::combine(errorMessages, "\n"));
                        QApplication::restoreOverrideCursor();
                        QMessageBox::critical(this, "ERROR", msg);
                        return;
                     }
                     
                     //
                     // If spec file read correctly, there is one brain model
                     //
                     if (bs->getNumberOfBrainModels() == 1) {
                        //
                        // Get the surface
                        //
                        bms = bs->getBrainModelSurface(0);
                        
                        //
                        // Make sure valid
                        //
                        if (bms == NULL) {
                           progressDialog.cancel();
                           QString msg("No brain model surface after reading ");
                           msg.append(mappingTopoFileName);
                           msg.append(" and ");
                           msg.append(mappingCoordFileNames[surfaceIndex]);
                           QApplication::restoreOverrideCursor();
                           QMessageBox::critical(this, "ERROR", msg);
                           delete bs;
                           return;
                        }
                     }
                     else {
                        progressDialog.cancel();
                        QString msg("No brain model after reading ");
                        msg.append(mappingTopoFileName);
                        msg.append(" and ");
                        msg.append(mappingCoordFileNames[surfaceIndex]);
                        QApplication::restoreOverrideCursor();
                        QMessageBox::critical(this, "ERROR", msg);
                        delete bs;
                        return;
                     }
                  }
                  break;
            }
      
            //
            // Allocate number of nodes and columns for metric file
            //
            if (mappingMetricFile.getNumberOfColumns() == 0) {
               mappingMetricFile.setNumberOfNodesAndColumns(bs->getNumberOfNodes(), numberOfColumns);
            }
            
            //
            // Volume data that is to be mapped
            //
            VolumeFile* vf = NULL;
            bool createdVolumeFile = false;
            
            //
            // Get info about volume that is to be mapped
            //
            GuiMapFmriVolume* fmriVolume = volumesToBeMapped[volumeIndex];
            switch(fmriVolume->getFmriVolumeType()) {
               case GuiMapFmriVolume::FMRI_VOLUME_TYPE_FILE_ON_DISK:
                  //
                  // Read in the volume data
                  //
                  vf = new VolumeFile;
                  createdVolumeFile = true;
                  try {
                     vf->readFile(fmriVolume->getVolumeFileName(),
                                  subVolumeIndex);
                  }
                  catch (FileException& e) {
                  }
                  break;
               case GuiMapFmriVolume::FMRI_VOLUME_TYPE_FILE_IN_MEMORY:
                  vf = fmriVolume->getInMemoryVolumeFile();
                  break;
            }
         
            //
            // Update mapping algorithm parameters
            //
            mappingParameters.setAlgorithm(
               static_cast<BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM>(
                  algorithmComboBox->currentIndex()));
            mappingParameters.setAlgorithmMetricAverageVoxelParameters(algorithmAverageVoxelNeighborDoubleSpinBox->value());
            mappingParameters.setAlgorithmMetricMaximumVoxelParameters(algorithmMaximumVoxelNeighborDoubleSpinBox->value());
            mappingParameters.setAlgorithmMetricStrongestVoxelParameters(algorithmStrongestVoxelNeighborDoubleSpinBox->value());
            mappingParameters.setAlgorithmMetricGaussianParameters(algorithmGaussianNeighborDoubleSpinBox->value(),
                                                  algorithmGaussianSigmaNormDoubleSpinBox->value(),
                                                  algorithmGaussianSigmaTangDoubleSpinBox->value(),
                                                  algorithmGaussianNormBelowDoubleSpinBox->value(),
                                                  algorithmGaussianNormAboveDoubleSpinBox->value(),
                                                  algorithmGaussianTangCutoffDoubleSpinBox->value());
            mappingParameters.setAlgorithmMetricMcwBrainFishParameters(algorithmBrainFishMaxDistanceDoubleSpinBox->value(),
                                                      algorithmBrainFishSplatFactorSpinBox->value());
            
            //
            // Create the mapper class
            //
            BrainModelVolumeToSurfaceMapper bmvsmm(
               bs,
               bms,
               vf,
               &mappingMetricFile,
               mappingParameters,
               metricColumn,
               metricInfo->getMetricColumnName(metricColumn));
         
            //
            // Run the mapper
            //
            try {
               bmvsmm.execute();
            }
            catch (BrainModelAlgorithmException& e) {
               progressDialog.cancel();
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", e.whatQString());
               if (createdBrainSet) {
                  delete bs;
               }
               return;
            }
         
            //
            // Prepend user's column comment since mapper overwrites with mapping parameters
            //
            QString columnComment(metricInfo->getMetricColumnComment(metricColumn));
            if (columnComment.isEmpty() == false) {
               columnComment.append("\n");
               mappingMetricFile.prependToColumnComment(metricColumn, columnComment);
            }
            
            //
            // Set the thresholds
            //
            float negThresh, posThresh;
            metricInfo->getMetricColumnThresholds(metricColumn, negThresh, posThresh);
            mappingMetricFile.setColumnThresholding(metricColumn, negThresh, posThresh);
            
            //
            // Free up the volume file if needed
            //
            if (createdVolumeFile) {
               delete vf;
               vf = NULL;
            }
            
            //
            // Free up the brain set if it was created
            //
            if (createdBrainSet) {
               delete bs;
               bs = NULL;
            }
         }  // for (int metricColumn
      
         //
         // Do metric average/deviation/error if requested
         //
         if (mms.getDoAvgOfAllCoordFileFlag() ||
             mms.getDoStdDevOfAllCoordFileFlag() ||
             mms.getDoStdErrorOfAllCoordFileFlag() ||
             mms.getDoMinOfAllCoordFileFlag() ||
             mms.getDoMaxOfAllCoordFileFlag()) {
             
            QString progressMessage("Generating ");
            
            //
            // Get names of stats (note that stats not generated for blank names)
            //
            QString avgName, devName, errorName, minName, maxName;
            if (mms.getDoAvgOfAllCoordFileFlag()) {
               avgName = metricInfo->getMetricAverageOfAllColumnName();
               progressMessage.append("Average ");
            }
            if (mms.getDoStdDevOfAllCoordFileFlag()) {
               devName = metricInfo->getMetricStdDevColumnName();
               progressMessage.append(" Sample Standard Deviation ");
            }
            if (mms.getDoStdErrorOfAllCoordFileFlag()) {
               errorName = metricInfo->getMetricStdErrorColumnName();
               progressMessage.append(" Standard Error ");
            }
            if (mms.getDoMinOfAllCoordFileFlag()) {
               minName = metricInfo->getMetricMinValueColumnName();
               progressMessage.append(" Minimum Value");
            }
            if (mms.getDoMaxOfAllCoordFileFlag()) {
               maxName = metricInfo->getMetricMaxValueColumnName();
               progressMessage.append(" Maximum Value");
            }
            
            progressDialog.setLabelText(progressMessage);
            progressDialog.show();
            
            //
            // Generate stats
            //
            mappingMetricFile.computeStatistics(avgName, devName, errorName, minName, maxName);
            
            //
            // Add comments and thresholds
            //
            if (avgName.isEmpty() == false) {
               const int avgCol = mappingMetricFile.getColumnWithName(avgName);
               if (avgCol >= 0) {
                  mappingMetricFile.prependToColumnComment(avgCol, 
                              metricInfo->getMetricAverageOfAllComment());
                  float negThresh, posThresh;
                  metricInfo->getMetricAverageOfAllThresholds(negThresh, posThresh);
                  mappingMetricFile.setColumnThresholding(avgCol, negThresh, posThresh);
               
                  //
                  // Number of nodes above the thresholds
                  //
                  std::vector<int> negCount, posCount;
                  for (int iCol = 0; iCol < avgCol; iCol++) {
                     int nc, pc;
                     mappingMetricFile.getThresholdExceededCounts(iCol, negThresh, posThresh,
                                                                   nc, pc);
                     negCount.push_back(nc);
                     posCount.push_back(pc);
                  }
                  const int numColCount = static_cast<int>(posCount.size());
                  
                  //
                  // Get the average nodes exceeding thresholds
                  //
                  int posCountAvgNum = 0, negCountAvgNum= 0;
                  for (int iCol = 0; iCol < numColCount; iCol++) {
                     posCountAvgNum += posCount[iCol];
                     negCountAvgNum += negCount[iCol];
                  }
                  if (posCountAvgNum > 0) {
                     posCountAvgNum = posCountAvgNum / numColCount;
                  }
                  if (negCountAvgNum > 0) {
                     negCountAvgNum = negCountAvgNum / numColCount;
                  }
                  if (DebugControl::getDebugOn()) {
                     std::cout << "Pos/Neg counts: " << negCountAvgNum << " " << posCountAvgNum << std::endl;
                  }
                  
                  //
                  // Get all values in the average column
                  //
                  std::vector<float> avgValues;
                  mappingMetricFile.getColumnForAllNodes(avgCol, avgValues);
                  
                  //
                  // Sort the average column
                  //
                  std::sort(avgValues.begin(), avgValues.end());
                  
                  //
                  // Set the average thresholds
                  //
                  float avgPosThresh = 0.0, avgNegThresh = 0.0;
                  const int num = mappingMetricFile.getNumberOfNodes();
                  const int posIndex = num - posCountAvgNum;
                  if ((posIndex >= 0) && (posIndex < num)) {
                     avgPosThresh = avgValues[posIndex];
                  }
                  if ((negCountAvgNum >= 0) && (negCountAvgNum < num)) {
                     avgNegThresh = avgValues[negCountAvgNum];
                  }
                  for (int iCol = 0; iCol <= avgCol; iCol++) {
                     mappingMetricFile.setColumnAverageThresholding(iCol, avgNegThresh, avgPosThresh);
                  }
               }
            }
            
            if (devName.isEmpty() == false) {
               const int col = mappingMetricFile.getColumnWithName(devName);
               if (col >= 0) {
                  mappingMetricFile.prependToColumnComment(col, 
                              metricInfo->getMetricStdDevComment());
                  float negThresh, posThresh;
                  metricInfo->getMetricStdDevThresholds(negThresh, posThresh);
                  mappingMetricFile.setColumnThresholding(col, negThresh, posThresh);
               }
            }
            if (errorName.isEmpty() == false) {
               const int col = mappingMetricFile.getColumnWithName(errorName);
               if (col >= 0) {
                  mappingMetricFile.prependToColumnComment(col, 
                              metricInfo->getMetricStdErrorComment());
                  float negThresh, posThresh;
                  metricInfo->getMetricStdErrorThresholds(negThresh, posThresh);
                  mappingMetricFile.setColumnThresholding(col, negThresh, posThresh);
               }
            }
            if (minName.isEmpty() == false) {
               const int col = mappingMetricFile.getColumnWithName(minName);
               if (col >= 0) {
                  mappingMetricFile.prependToColumnComment(col,
                              metricInfo->getMetricMinValueComment());
                  float negThresh, posThresh;
                  metricInfo->getMetricMinValueThresholds(negThresh, posThresh);
                  mappingMetricFile.setColumnThresholding(col, negThresh, posThresh);
               }
            }
            if (maxName.isEmpty() == false) {
               const int col = mappingMetricFile.getColumnWithName(maxName);
               if (col >= 0) {
                  mappingMetricFile.prependToColumnComment(col,
                              metricInfo->getMetricMaxValueComment());
                  float negThresh, posThresh;
                  metricInfo->getMetricMaxValueThresholds(negThresh, posThresh);
                  mappingMetricFile.setColumnThresholding(col, negThresh, posThresh);
               }
            }
         }
         
         //
         // Map to the average fiducial surface (average fiducial only used when map to atlas)
         //
         MetricFile averageFiducialMetricFile;
         if (mms.getDoAverageFiducialFileFlag()) {
            //
            // Get volume information
            //
            const int volumeIndex = metricInfo->getVolumeIndex();
            const int subVolumeIndex = metricInfo->getSubVolumeIndex();
            
            //
            // Update the progress dialog
            //
            qApp->processEvents();  // note: qApp is global in QApplication
            if (progressDialog.wasCanceled()) {
               return;
            }
            QString progressMessage("Mapping Volume: ");
            progressMessage.append(volumesToBeMapped[volumeIndex]->getDescriptiveName());
            progressMessage.append("\nSub-Volume: ");
            progressMessage.append(volumesToBeMapped[volumeIndex]->getSubVolumeName(subVolumeIndex));
            progressMessage.append(" to \n");
            progressMessage.append(metricInfo->getMetricAverageFiducialCoordColumnName());
            progressMessage.append(".");
            progressDialog.setLabelText(progressMessage);
            progressDialog.setValue(progressDialog.value() + 1);
            progressDialog.show();
            
            //
            // Set to the spec file directory
            //
            const QString directory(mms.getMappingFilesPath());
            if (directory.isEmpty() == false) {
               QDir::setCurrent(directory);
            }
            
            // 
            // Create a spec file and initialize it for mapping files
            //
            SpecFile sf;
            sf.setTopoAndCoordSelected(mappingTopoFileName, 
                                       mms.getAverageFiducialCoordFileName(),
                                       sf.getStructure());
            
            //
            // Create the brain set
            //
            BrainSet* bs = new BrainSet;
            BrainModelSurface* bms = NULL;

            //
            // Load the brain set
            //
            std::vector<QString> errorMessages;
            bs->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                             sf, "mapping_spec", errorMessages, NULL, NULL);
            if (errorMessages.empty() == false) {
               progressDialog.cancel();
               QString msg("Error creating BrainSet for mapping average fiducial data.\n");
               msg.append(StringUtilities::combine(errorMessages, "\n"));
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", msg);
               return;
            }
            
            //
            // If spec file read correctly, there is one brain model
            //
            if (bs->getNumberOfBrainModels() == 1) {
               //
               // Get the surface
               //
               bms = bs->getBrainModelSurface(0);
               
               //
               // Make sure valid
               //
               if (bms == NULL) {
                  progressDialog.cancel();
                  QString msg("No brain model surface after reading ");
                  msg.append(mappingTopoFileName);
                  msg.append(" and ");
                  msg.append(mms.getAverageFiducialCoordFileName());
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", msg);
                  delete bs;
                  return;
               }
            }
            else {
               progressDialog.cancel();
               QString msg("No brain model after reading ");
               msg.append(mappingTopoFileName);
               msg.append(" and ");
               msg.append(mms.getAverageFiducialCoordFileName());
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", msg);
               delete bs;
               return;
            }
      
            //
            // Allocate number of nodes and columns for metric file
            //
            averageFiducialMetricFile.setNumberOfNodesAndColumns(bs->getNumberOfNodes(), 1);
            
            //
            // Volume data that is to be mapped
            //
            VolumeFile* vf = NULL;
            bool createdVolumeFile = false;
            
            //
            // Get info about volume that is to be mapped
            //
            GuiMapFmriVolume* fmriVolume = volumesToBeMapped[volumeIndex];
            switch(fmriVolume->getFmriVolumeType()) {
               case GuiMapFmriVolume::FMRI_VOLUME_TYPE_FILE_ON_DISK:
                  //
                  // Read in the volume data
                  //
                  vf = new VolumeFile;
                  createdVolumeFile = true;
                  try {
                     vf->readFile(fmriVolume->getVolumeFileName(),
                                  subVolumeIndex);
                  }
                  catch (FileException& e) {
                  }
                  break;
               case GuiMapFmriVolume::FMRI_VOLUME_TYPE_FILE_IN_MEMORY:
                  vf = fmriVolume->getInMemoryVolumeFile();
                  break;
            }
         
            //
            // Update mapping algorithm parameters
            //
            mappingParameters.setAlgorithm(
               static_cast<BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM>(
                  algorithmComboBox->currentIndex()));
            mappingParameters.setAlgorithmMetricAverageVoxelParameters(algorithmAverageVoxelNeighborDoubleSpinBox->value());
            mappingParameters.setAlgorithmMetricMaximumVoxelParameters(algorithmMaximumVoxelNeighborDoubleSpinBox->value());
            mappingParameters.setAlgorithmMetricStrongestVoxelParameters(algorithmStrongestVoxelNeighborDoubleSpinBox->value());
            mappingParameters.setAlgorithmMetricGaussianParameters(algorithmGaussianNeighborDoubleSpinBox->value(),
                                                  algorithmGaussianSigmaNormDoubleSpinBox->value(),
                                                  algorithmGaussianSigmaTangDoubleSpinBox->value(),
                                                  algorithmGaussianNormBelowDoubleSpinBox->value(),
                                                  algorithmGaussianNormAboveDoubleSpinBox->value(),
                                                  algorithmGaussianTangCutoffDoubleSpinBox->value());
            mappingParameters.setAlgorithmMetricMcwBrainFishParameters(algorithmBrainFishMaxDistanceDoubleSpinBox->value(),
                                                      algorithmBrainFishSplatFactorSpinBox->value());
            
            //
            // Create the mapper class
            //
            BrainModelVolumeToSurfaceMapper bmvsmm(
               bs,
               bms,
               vf,
               &averageFiducialMetricFile,
               mappingParameters,
               0,
               metricInfo->getMetricAverageFiducialCoordColumnName());
         
            //
            // Run the mapper
            //
            try {
               bmvsmm.execute();
            }
            catch (BrainModelAlgorithmException& e) {
               progressDialog.cancel();
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", e.whatQString());
               delete bs;
               bs = NULL;
               return;
            }
         
            //
            // Prepend user's column comment since mapper overwrites with mapping parameters
            //
            QString columnComment(metricInfo->getMetricAverageFiducialCoordComment());
            if (columnComment.isEmpty() == false) {
               columnComment.append("\n");
               averageFiducialMetricFile.prependToColumnComment(0, columnComment);
            }
            
            //
            // Set the thresholds
            //
            float negThresh, posThresh;
            metricInfo->getMetricAverageFiducialCoordThresholds(negThresh, posThresh);
            averageFiducialMetricFile.setColumnThresholding(0, negThresh, posThresh);
            
            //
            // Free up the volume file if needed
            //
            if (createdVolumeFile) {
               delete vf;
               vf = NULL;
            }
            
            //
            // Free up the brain set if it was created
            //
            delete bs;
            bs = NULL;
            
            //
            // Add onto output metric file
            //
            outputMetricFile.append(averageFiducialMetricFile);
         }
   
   
         //
         // Append average, dev, etc columns first so that they are before indiv cases
         // The average, dev, etc are after the individual cases
         //
         bool haveGroupStat = false;
         std::vector<int> metricColumnDestination(mappingMetricFile.getNumberOfColumns(),
                                                  MetricFile::APPEND_COLUMN_NEW);
         for (int n = 0; n < mappingMetricFile.getNumberOfColumns(); n++) {
            if (n >= metricInfo->getNumberOfMetricColumns()) {
               metricColumnDestination[n] = MetricFile::APPEND_COLUMN_NEW;
               haveGroupStat = true;
            }
            else {
               metricColumnDestination[n] = MetricFile::APPEND_COLUMN_DO_NOT_LOAD;
            }
         }
         if (haveGroupStat) {
            outputMetricFile.append(mappingMetricFile, metricColumnDestination,
                                    MetricFile::FILE_COMMENT_MODE_APPEND);
         }
         
         //
         // Append indiv cases.
         //
         if (mms.getDoAllCasesCoordFileFlag()) {
            for (int n = 0; n < mappingMetricFile.getNumberOfColumns(); n++) {
               if (n < metricInfo->getNumberOfMetricColumns()) {
                  metricColumnDestination[n] = MetricFile::APPEND_COLUMN_NEW;
               }
               else {
                  metricColumnDestination[n] = MetricFile::APPEND_COLUMN_DO_NOT_LOAD;
               }
            }
            NodeAttributeFile::FILE_COMMENT_MODE fcm = MetricFile::FILE_COMMENT_MODE_APPEND;
            if (haveGroupStat) {
               fcm = MetricFile::FILE_COMMENT_MODE_LEAVE_AS_IS;
            }
            outputMetricFile.append(mappingMetricFile, metricColumnDestination, fcm);
         }
                                 
      } // number of metric mapping info
   
      //
      // Update Caret/Spec file with new metric data
      //
      switch (mms.getMetricOutputType()) {
         case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_NONE:
            break;
         case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_CARET_METRIC:
            //
            // Append to existing metrics
            //
            {
               MetricFile* currentMetricFile = theMainWindow->getBrainSet()->getMetricFile();
               try {
                  currentMetricFile->append(outputMetricFile);
               }
               catch (FileException& e) {
                  progressDialog.cancel();
                  QString msg("Data mapped but unable to append to current metric file.\n");
                  msg.append(e.whatQString());
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", msg);
                  return;
               }
            }
            break;
         case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_SPEC_FILE:
            {
               //
               //  Change to the spec file's directory
               //
               const QString directory(FileUtilities::dirname(mms.getOutputSpecFileName()));
               if (directory.isEmpty() == false) {
                  QDir::setCurrent(directory);
               }
               
               //
               // Metric file is always written in same directory as spec file
               // so chop off path
               //
               const QString metricName(FileUtilities::basename(mms.getMetricFileName()));
               
               //
               // Save the metric file
               //
               try {
                  outputMetricFile.writeFile(metricName);
               }
               catch (FileException& e) {
                  progressDialog.cancel();
                  QString msg("Unable to save metric file ");
                  msg.append(metricName);
                  msg.append("\n");
                  msg.append(e.whatQString());
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", msg);
                  return;
               }    
               
               //
               // Update the spec file
               //
               SpecFile sf;
               try {
                  //
                  // Read the spec file for updating
                  //
                  sf.readFile(mms.getOutputSpecFileName());

                  //
                  // Add the metric file to the spec file
                  //
                  sf.addToSpecFile(SpecFile::getMetricFileTag(), metricName, "", false);
                  
                  //
                  // Write the spec file
                  //
                  sf.writeFile(mms.getOutputSpecFileName());
                  
                  //
                  // Add the spec file to the recent spec files in preferences
                  //
                  PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
                  pf->addToRecentSpecFiles(mms.getOutputSpecFileName(), true);
               }
               catch (FileException& e) {
                  progressDialog.cancel();
                  QString msg("Unable to update spec file: ");
                  msg.append(mms.getOutputSpecFileName());
                  msg.append("\n");
                  msg.append(e.whatQString());
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", msg);
                  return;
               }
            }
            break;
      }
      
      //
      // Restore directory
      //
      QDir::setCurrent(savedDirectory);
   }
}
   
/**
 * create the data mapping type page.
 */
QWidget* 
GuiMapFmriDialog::createDataMappingTypePage()
{
   const QString infoMessage = 
      "Choose the type of volume(s) that are to be mapped.";
   QWidget* infoWidget = addPageInformation("Choose Data Mapping Type", infoMessage);
   
   //
   // Mapping data file types
   //
   mappingTypeFunctionalRadioButton = new QRadioButton("Metric (Functional) or Surface Shape Data");
   mappingTypePaintRadioButton = new QRadioButton("Paint (ROI) or Probabilistic Atlas Data");
   
   //
   // Button group to keep buttons exclusive
   //
   QButtonGroup* mappingTypeButtonGroup = new QButtonGroup(this);
   QObject::connect(mappingTypeButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotDataMappingTypeButtGroup(int)));
   mappingTypeButtonGroup->addButton(mappingTypeFunctionalRadioButton);
   mappingTypeButtonGroup->addButton(mappingTypePaintRadioButton);
   
   //
   // Mapping type group box and layout
   //
   QGroupBox* mappingTypeGroupBox = new QGroupBox("Data Mapping Type");
   QVBoxLayout* mappingTypeLayout = new QVBoxLayout(mappingTypeGroupBox);
   mappingTypeLayout->addWidget(mappingTypeFunctionalRadioButton);
   mappingTypeLayout->addWidget(mappingTypePaintRadioButton);
   mappingTypeGroupBox->setFixedHeight(mappingTypeGroupBox->sizeHint().height());
   
   //
   // Layout for page
   //
   QWidget* mapTypePageWidget = new QWidget;
   QVBoxLayout* mapTypeLayout = new QVBoxLayout(mapTypePageWidget);
   mapTypeLayout->addWidget(mappingTypeGroupBox);
   mapTypeLayout->addWidget(infoWidget);
   
   return mapTypePageWidget;
}      

/**
 * called when a data mapping type is selected.
 */
void 
GuiMapFmriDialog::slotDataMappingTypeButtGroup(int /*item*/)
{
   dataMappingType = DATA_MAPPING_TYPE_NONE;
   if (mappingTypeFunctionalRadioButton->isChecked()) {
      dataMappingType = DATA_MAPPING_TYPE_METRIC;
   }
   else if (mappingTypePaintRadioButton->isChecked()) {
      dataMappingType = DATA_MAPPING_TYPE_PAINT;
   }
   slotSetAppropriatePages();
            
   outputDataFileExtension = "";
   switch (dataMappingType) {
      case DATA_MAPPING_TYPE_NONE:
         break;
      case DATA_MAPPING_TYPE_METRIC:
         setWindowTitle("Map Functional Volumes To Surfaces");
         outputDataFileExtension = SpecFile::getMetricFileExtension();
         break;
      case DATA_MAPPING_TYPE_PAINT:
         setWindowTitle("Map Paint Volumes To Surfaces");
         outputDataFileExtension = SpecFile::getPaintFileExtension();
         break;
   }   

   if (runningAsPartOfCaret) {
      bool enableIt = false;
      switch (dataMappingType) {
         case DATA_MAPPING_TYPE_NONE:
            break;
         case DATA_MAPPING_TYPE_METRIC:
            if (theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles() > 0) {
               enableIt = true;
            }
            break;
         case DATA_MAPPING_TYPE_PAINT:
            if (theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles() > 0) {
               enableIt = true;
            }
            break;
      }
      if (enableIt) {
         loadedVolumesPushButton->setEnabled(true);
      }
   }
}
      
/**
 * create the volume selection page.
 */
QWidget* 
GuiMapFmriDialog::createVolumeSelectionPage()
{
   //
   // List box for displaying functional volumes that are to be mapped
   //   
   volumeListBox = new QListWidget;
   volumeListBox->setMinimumWidth(400);
   volumeListBox->setMinimumHeight(100);
   volumeListBox->setSelectionMode(QListWidget::ExtendedSelection);
   
   //
   // Pushbutton for selecting volumes from disk
   //
   QPushButton* diskVolumesPushButton = new QPushButton("Add Volumes From Disk...");
   diskVolumesPushButton->setAutoDefault(false);
   QObject::connect(diskVolumesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDiskVolumesPushButton()));
    
   //
   // Pushbutton for removing volumes in dialog
   //
   QPushButton* removeVolumePushButton = new QPushButton("Remove Selected Volumes");
   removeVolumePushButton->setAutoDefault(false);
   QObject::connect(removeVolumePushButton, SIGNAL(clicked()),
                    this, SLOT(slotRemoveVolumesPushButton()));
   
   //
   // Pushbutton for selecting loaded volumes
   //
   loadedVolumesPushButton = new QPushButton("Add Loaded Volumes...");
   loadedVolumesPushButton->setAutoDefault(false);
   QObject::connect(loadedVolumesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotLoadedVolumesPushButton()));
   loadedVolumesPushButton->setEnabled(false);   
    
   QtUtilities::makeButtonsSameSize(loadedVolumesPushButton,
                                    diskVolumesPushButton,
                                    removeVolumePushButton);
   
   //
   // Grid for volume selection push buttons
   //
   QGridLayout* selectionButtonGridLayout = new QGridLayout;
   selectionButtonGridLayout->addWidget(diskVolumesPushButton, 0, 0);
   selectionButtonGridLayout->addWidget(removeVolumePushButton, 0, 1);
   selectionButtonGridLayout->addWidget(loadedVolumesPushButton, 1, 0);

   //
   // Group box for volume selection
   //
   QGroupBox* selectionGroupBox = new QGroupBox("Volume Selection");
   QVBoxLayout* selectionLayout = new QVBoxLayout(selectionGroupBox);
   selectionLayout->addWidget(volumeListBox);
   selectionLayout->addLayout(selectionButtonGridLayout);
   
   //
   // Threshold entry selection
   //   
   volumeEnterThreshCheckBox = new QCheckBox("Enable Entry of Volume Threshold");
   volumeEnterThreshCheckBox->setChecked(false);   
   QGroupBox* threshGroupBox = new QGroupBox("Volume Thresholding");
   QVBoxLayout* threshGroupLayout = new QVBoxLayout(threshGroupBox);
   threshGroupLayout->addWidget(volumeEnterThreshCheckBox);
   
   QString text ="";
   text += "<B>Add Data Volumes From Disk</B> Press this button to select data volume "
           "files.<P>";
   if (runningAsPartOfCaret) {
      text += "<B>Add Loaded Volumes</B> Press this button to select data "
              "volumes that are currently loaded in Caret.<P>";
   }
   text += "<B>Remove Selected Volumes</B> Press this button to remove all selected volumes.<P>";
   text += "In the file selection dialog, multiple files may be selected by holding "
           "down the CTRL key (Apple key on Macintosh).<P>";
   QWidget* infoWidget = addPageInformation("Functional Volume Selection Information", text);
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(selectionGroupBox);
   layout->addWidget(threshGroupBox);
   layout->addWidget(infoWidget);
   
   return w;
}

/**
 * Called when Remove Volumes pushbutton pressed.
 */
void
GuiMapFmriDialog::slotRemoveVolumesPushButton()
{
   bool volumeRemoved = false;
   for (int i = static_cast<int>(volumesToBeMapped.size()) - 1; i >= 0; i--) {
      const QListWidgetItem* item = volumeListBox->item(i);
      if (volumeListBox->isItemSelected(item)) {
         volumesToBeMapped.erase(volumesToBeMapped.begin() + i);
         volumeRemoved = true;
      }
   }
   
   if (volumeRemoved) {
      //
      // Clear mapping sets since volumes have changed
      //
      resetMappingSets();
   }

   loadVolumesListBox();
}

/**
 * Called when Select Loaded Volumes pushbutton pressed.
 */
void
GuiMapFmriDialog::slotLoadedVolumesPushButton()
{
   int num = 0;
   
   QString typeName;
   switch (dataMappingType) {
      case DATA_MAPPING_TYPE_NONE:
         break;
      case DATA_MAPPING_TYPE_METRIC:
         num = theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles();
         if (num == 0) {
            QMessageBox::warning(this, "Warning", "There are no loaded functional volumes");
            return;
         }
         typeName = "func";
         break;
      case DATA_MAPPING_TYPE_PAINT:
         num = theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles();
         if (num == 0) {
            QMessageBox::warning(this, "Warning", "There are no loaded paint volumes");
            return;
         }
         typeName = "paint";
         break;
   }
   
   
   std::vector<VolumeFile*> dataVolumes;
   
   std::vector<QString> names;
   for (int i = 0; i < num; i++) {
      VolumeFile* vf = NULL;
      switch (dataMappingType) {
         case DATA_MAPPING_TYPE_NONE:
            break;
         case DATA_MAPPING_TYPE_METRIC:
            vf = theMainWindow->getBrainSet()->getVolumeFunctionalFile(i);
            break;
         case DATA_MAPPING_TYPE_PAINT:
            vf = theMainWindow->getBrainSet()->getVolumePaintFile(i);
            break;
      }
      
      if (vf != NULL) {
         dataVolumes.push_back(vf);
         QString name(FileUtilities::basename(vf->getDescriptiveLabel()));
         if (name.isEmpty()) {
            name.append(typeName);
            name.append(" volume ");
            name.append(StringUtilities::fromNumber(i));
         }
         names.push_back(name);
      }
   }
   
   QtListBoxSelectionDialog lbsd(this,
                                 "Select Volumes",
                                 "",
                                 names,
                                 -1);
   lbsd.setAllowMultipleItemSelection(true);
   if (lbsd.exec() == QDialog::Accepted) {
      std::vector<int> selectedIndices;
      lbsd.getSelectedItemsIndices(selectedIndices);
      
      const int num = static_cast<int>(selectedIndices.size());
      for (int i = 0; i < num; i++) {
         VolumeFile* vf = dataVolumes[selectedIndices[i]];
         float negThresh = 0.0, posThresh = 0.0;
         
         switch (dataMappingType) {
            case DATA_MAPPING_TYPE_NONE:
               break;
            case DATA_MAPPING_TYPE_METRIC:
               //
               // Get the thresholds for metric data
               //
               if (volumeEnterThreshCheckBox->isChecked()) {
                  QApplication::beep();
                  GuiMapFmriThresholdDialog td(this, vf->getFileName());
                  if (td.exec() == QDialog::Accepted) {
                     td.getThresholds(negThresh, posThresh);
                  }
               }
               break;
            case DATA_MAPPING_TYPE_PAINT:
               break;
         }
         
         GuiMapFmriVolume* vol = new GuiMapFmriVolume(vf, negThresh, posThresh);
         volumesToBeMapped.push_back(vol);
      }
      
      if (num > 0) {
         //
         // Clear mapping sets since volumes have changed
         //
         resetMappingSets();
      }
   }
   
   loadVolumesListBox();
}

/**
 * Called when Select Volumes From Disk pushbutton pressed.
 */
void
GuiMapFmriDialog::slotDiskVolumesPushButton()
{
   static QString lastVolumeDirectory;
   static bool hdrWarningHasBeenDisplayed = false;
   
   QString instructions = "To select multiple volumes, hold down the CTRL key while selecting "
                          "volume file names with the mouse (Macintosh users should hold down "
                          "the Apple key).";
/*
   std::ostringstream str;
       << "AFNI " 
       << "NIFTI "
       << "SPM "
       << "WuNIL ("
       << "*" << SpecFile::getAfniVolumeFileExtension().toAscii().constData() << " "
       << "*" << SpecFile::getNiftiVolumeFileExtension().toAscii().constData() << " "
       << "*" << SpecFile::getAnalyzeVolumeFileExtension().toAscii().constData() << " "
       << "*" << SpecFile::getWustlVolumeFileExtension().toAscii().constData()
       << ")";
*/
   GuiFileDialogWithInstructions openVolumeFileDialog(this, instructions, "openVolSpec", true);
   openVolumeFileDialog.setWindowTitle("Choose Volume File");
   openVolumeFileDialog.setFileMode(GuiFileDialogWithInstructions::ExistingFiles);
   openVolumeFileDialog.setFilters(QStringList(FileFilters::getVolumeGenericFileFilter()));
   openVolumeFileDialog.selectFilter(FileFilters::getVolumeGenericFileFilter());
   if (lastVolumeDirectory.isEmpty() == false) {
      openVolumeFileDialog.setDirectory(lastVolumeDirectory);
   }
   openVolumeFileDialog.rereadDir();
   if (openVolumeFileDialog.exec() == QDialog::Accepted) {
      //
      // Check for HDR files
      //
      if (hdrWarningHasBeenDisplayed == false) {
         bool haveHDR = false;
         QStringList listHDR = openVolumeFileDialog.selectedFiles();
         QStringList::Iterator itHDR = listHDR.begin();
         while (itHDR != listHDR.end()) {
            if (itHDR->endsWith(SpecFile::getAnalyzeVolumeFileExtension())) {
              haveHDR = true;
              break;
            }
            ++itHDR;
         }
         if (haveHDR) {
            hdrWarningHasBeenDisplayed = true;
            const QString msg("You have selected a \".hdr\" file.  If this \".hdr\" file does\n"
                              "not contain an SPM originator, it is unlikely that it will get\n"
                              "mapped to the surface correctly.");
            if (QMessageBox::warning(this, 
                                     "WARNING", 
                                     msg, 
                                     (QMessageBox::Ok | QMessageBox::Cancel),
                                     QMessageBox::Cancel)
                                        == QMessageBox::Cancel) {
               return;
            }
         }
      }
   
      //
      // process files
      //
      QStringList list = openVolumeFileDialog.selectedFiles();
      QStringList::Iterator it = list.begin();
      bool firstFileFlag = true;
      bool volumeAdded = false;
      while( it != list.end() ) {
         QString filename((*it));
         if (firstFileFlag) {
            lastVolumeDirectory = FileUtilities::dirname(filename);
            firstFileFlag = false;
         }
         
         float negThresh = 0.0, posThresh = 0.0;

         switch (dataMappingType) {
            case DATA_MAPPING_TYPE_NONE:
               break;
            case DATA_MAPPING_TYPE_METRIC:
               //
               // Get the thresholds for metric data
               //
               if (volumeEnterThreshCheckBox->isChecked()) {
                  //
                  // Get the thresholds
                  //
                  QApplication::beep();
                  GuiMapFmriThresholdDialog td(this, filename);
                  if (td.exec() == QDialog::Accepted) {
                     td.getThresholds(negThresh, posThresh);
                  }
               }
               break;
            case DATA_MAPPING_TYPE_PAINT:
               break;
         }
         
         GuiMapFmriVolume* vol = new GuiMapFmriVolume(filename, negThresh, posThresh);
         volumesToBeMapped.push_back(vol);
         ++it;
         volumeAdded = true;
      }
      
      if (volumeAdded) {
         //
         // Clear mapping sets since volumes have changed
         //
         resetMappingSets();
      }
   }
   
   loadVolumesListBox();
}

/**
 * Load the volumes list box.
 */
void
GuiMapFmriDialog::loadVolumesListBox()
{
   //
   // Remove everything from the list box
   //
   volumeListBox->clear();
   
   for (int i = 0; i < static_cast<int>(volumesToBeMapped.size()); i++) {
      volumeListBox->addItem(volumesToBeMapped[i]->getDescriptiveName());
   }

   slotSetAppropriatePages();
}

/**
 * create the spec file and surface selection page.
 */
QWidget* 
GuiMapFmriDialog::createSpecFileAndSurfaceSelectionPage()
{
   //
   // Add list box for mapping sets
   //
   mappingSetListBox = new QListWidget;
   mappingSetListBox->setMinimumWidth(400);
   mappingSetListBox->setMinimumHeight(100);
   mappingSetListBox->setSelectionMode(QListWidget::ExtendedSelection);
   
   //
   // Pushbutton for selecting caret surfaces
   //
   addCaretSurfacesPushButton = new QPushButton("Map to Caret...");
   addCaretSurfacesPushButton->setAutoDefault(false);
   QObject::connect(addCaretSurfacesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddCaretSurfacesPushButton()));
   addCaretSurfacesPushButton->setToolTip(
                 "Press this button if you would like to map the\n"
                 "volume using surface(s) currently loaded in\n"
                 "Caret and place the output into the data file\n"
                 "currently loaded into Caret.  After doing so you\n"
                 "will need to save the metric file.");

   //
   // Pushbutton for adding spec files
   //
   QPushButton* addSpecFilesPushButton = new QPushButton("Map to Spec File...");
   addSpecFilesPushButton->setAutoDefault(false);
   QObject::connect(addSpecFilesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddSpecFilesPushButton()));
   addSpecFilesPushButton->setToolTip(
                 "Press this button if you would like to map the \n"
                 "volume using surface(s) in a spec file and add\n"
                 "the output metric file to the spec file.");
   
   //
   // Map to Caret using Atlas
   //
   addCaretMapWithAtlasPushButton = new QPushButton("Map to Caret With Atlas...");
   addCaretMapWithAtlasPushButton->setAutoDefault(false);
   QObject::connect(addCaretMapWithAtlasPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddCaretMapWithAtlasPushButton()));
   addCaretMapWithAtlasPushButton->setToolTip(
                 "Press this button if you would like to map the \n"
                 "volume using an atlas but add the output\n"
                 "into the data file currently loaded in Caret.\n"
                 "After doing so, you will need to save the data\n"
                 "file.");
                    
   //
   // Pushbutton for adding spec files but mapping with an atlas
   //
   addSpecAtlasPushButton = new QPushButton("Map to Spec File With Atlas...");
   addSpecAtlasPushButton->setAutoDefault(false);
   QObject::connect(addSpecAtlasPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddSpecAtlasPushButton()));
   addSpecAtlasPushButton->setToolTip(
                 "Press this button if you would like to map the\n"
                 "volume using an atlas but place the output into\n"
                 "a data file and add the data file to a spec file.");
                    
   //
   // Pushbutton for removing items in dialog
   //
   QPushButton* removeMappingSetsPushButton = new QPushButton("Remove Selected Items");
   removeMappingSetsPushButton->setAutoDefault(false);
   QObject::connect(removeMappingSetsPushButton, SIGNAL(clicked()),
                    this, SLOT(slotRemoveMappingSetsPushButton()));
   
   QtUtilities::makeButtonsSameSize(addCaretSurfacesPushButton,
                                    addSpecFilesPushButton,
                                    removeMappingSetsPushButton,
                                    addSpecAtlasPushButton,
                                    addCaretMapWithAtlasPushButton);
   
   //
   // Grid layout for buttons
   //
   QGridLayout* buttonsGridLayout = new QGridLayout;
   buttonsGridLayout->addWidget(addCaretSurfacesPushButton, 0, 0);
   buttonsGridLayout->addWidget(addSpecFilesPushButton, 0, 1);
   buttonsGridLayout->addWidget(addCaretMapWithAtlasPushButton, 1, 0);
   buttonsGridLayout->addWidget(addSpecAtlasPushButton, 1, 1);
   buttonsGridLayout->addWidget(removeMappingSetsPushButton, 2, 0);
   
   //
   // Group box and layout for spec file and surface selection
   //
   QGroupBox* groupBox = new QGroupBox("Spec File and Surface Selection");
   QVBoxLayout* groupBoxLayout = new QVBoxLayout(groupBox);
   groupBoxLayout->addWidget(mappingSetListBox);
   groupBoxLayout->addLayout(buttonsGridLayout);
   
   const QString infoMessage = 
      "<B>Map to Caret</B> - Press this button to map volumes to the metric file currently "
      "loaded in Caret using surfaces loaded in Caret.<P>"
      "<B>Map To Caret With Atlas</B> - Press this button to map volumes to the metric file "
      "currently loaded in Caret using Atlas surface(s).<P>"
      "<B>Map to Spec File</B> - Press this button to map volumes to a metric file that will "
      "be added to the selected spec file using topology and coordinate files listed in the spec "
      "file.<P>"
      "<B>Map to Spec File With Atlas</B> - Press this button to map volumes to a metric file "
      "that will be added to the selected spec file using Atlas surface(s).";
   QWidget* infoWidget = addPageInformation("Spec File and Surface Selection Information", infoMessage);
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(groupBox);
   layout->addWidget(infoWidget);
   
   return w;
}

/**
 * Called when add spec file map with atlas pushbutton pressed.
 */
void 
GuiMapFmriDialog::slotAddSpecAtlasPushButton()
{
   bool enableMetricMultiFidOptions = false;   
   bool enablePaintMultiFidOptions = false;   
   switch (dataMappingType) {
      case DATA_MAPPING_TYPE_NONE:
         break;
      case DATA_MAPPING_TYPE_METRIC:
         enableMetricMultiFidOptions = true;
         break;
      case DATA_MAPPING_TYPE_PAINT:
         enablePaintMultiFidOptions = true;
         break;
   }

   //
   // Choose output spec file and atlas files
   //
   GuiMapFmriAtlasDialog fad(this, &atlasSpecFileInfo, theMainWindow->getBrainSet()->getPreferencesFile(),
                             "", "", true, enableMetricMultiFidOptions, enablePaintMultiFidOptions);
   if (fad.exec() == QDialog::Accepted) {
      QString atlasPath, atlasTopoFileName, description, metricNameHint, atlasAvgCoordFileName;
      QString structureName;
      std::vector<QString> atlasCoordFileNames;
      bool mapToAvgCoordFileFlag = false,
           mapToAvgOfAllFlag = false,
           mapToStdDevOfAllFlag = false,
           mapToStdErrorOfAllFlag = false,
           mapToMinOfAllFlag = false,
           mapToMaxOfAllFlag = false,
           mapToAllCasesFlag = false,
           mapToMostCommonOfAllFlag = false,
           mapToMostCommonExcludeUnidentifiedOfAllFlag = false;
      
      if (enableMetricMultiFidOptions) {
         fad.getSelectedMetricAtlasData(atlasPath, 
                                        atlasTopoFileName, 
                                        description,
                                        atlasCoordFileNames, 
                                        atlasAvgCoordFileName, 
                                        metricNameHint,
                                        structureName,
                                        mapToAvgCoordFileFlag, 
                                        mapToAvgOfAllFlag, 
                                        mapToStdDevOfAllFlag,
                                        mapToStdErrorOfAllFlag, 
                                        mapToMinOfAllFlag, 
                                        mapToMaxOfAllFlag,
                                        mapToAllCasesFlag);
      }
      else if (enablePaintMultiFidOptions) {
         fad.getSelectedPaintAtlasData(atlasPath, 
                                        atlasTopoFileName, 
                                        description,
                                        atlasCoordFileNames, 
                                        atlasAvgCoordFileName, 
                                        metricNameHint,
                                        structureName,
                                        mapToAvgCoordFileFlag, 
                                        mapToMostCommonOfAllFlag, 
                                        mapToMostCommonExcludeUnidentifiedOfAllFlag,
                                        mapToAllCasesFlag);
      }
      
      const QString outputSpecFileName(fad.getOutputSpecFileName());
      
      if ((atlasCoordFileNames.empty() == false) ||
          (atlasAvgCoordFileName.isEmpty() == false)) {
         //
         // Create the mapping set
         //
         mappingSets.push_back(GuiMapFmriMappingSet(outputSpecFileName,
                                                    atlasPath,
                                                    atlasTopoFileName,
                                                    atlasCoordFileNames,
                                                    atlasAvgCoordFileName,
                                                    volumesToBeMapped,
                                                    outputDataFileExtension,
                                                    description,
                                                    metricNameHint,
                                                    structureName,
                                                    true,
                                                    mapToAvgCoordFileFlag,
                                                    mapToAvgOfAllFlag,
                                                    mapToStdDevOfAllFlag,
                                                    mapToStdErrorOfAllFlag,
                                                    mapToMinOfAllFlag,
                                                    mapToMaxOfAllFlag,
                                                    mapToAllCasesFlag,
                                                    mapToMostCommonOfAllFlag,
                                                    mapToMostCommonExcludeUnidentifiedOfAllFlag));
      }
   }

   loadMappingSetsListBox();
}      

/**
 * Called when add caret map with spec pushbutton is pressed.
 */
void 
GuiMapFmriDialog::slotAddCaretMapWithAtlasPushButton()
{
   bool enableMetricMultiFidOptions = false;  
   bool enablePaintMultiFidOptions = false; 
   switch (dataMappingType) {
      case DATA_MAPPING_TYPE_NONE:
         break;
      case DATA_MAPPING_TYPE_METRIC:
         enableMetricMultiFidOptions = true;
         break;
      case DATA_MAPPING_TYPE_PAINT:
         enablePaintMultiFidOptions = true;
         break;
   }

   //
   // Choose atlas files
   //
   GuiMapFmriAtlasDialog fad(this, &atlasSpecFileInfo,
                             theMainWindow->getBrainSet()->getPreferencesFile(),
                             theMainWindow->getBrainSet()->getSpecies().getName(),
                             theMainWindow->getBrainSet()->getStructure().getTypeAsString(),
                             false,
                             enableMetricMultiFidOptions, enablePaintMultiFidOptions);
   if (fad.exec() == QDialog::Accepted) {
      QString atlasPath, atlasTopoFileName, description, metricNameHint, atlasAvgCoordFileName;
      QString structureName;
      std::vector<QString> atlasCoordFileNames;
      bool mapToAvgCoordFileFlag = false,
           mapToAvgOfAllFlag = false,
           mapToStdDevOfAllFlag = false,
           mapToStdErrorOfAllFlag = false,
           mapToMinOfAllFlag = false,
           mapToMaxOfAllFlag = false,
           mapToAllCasesFlag = false,
           mapToMostCommonOfAllFlag = false,
           mapToMostCommonExcludeUnidentifiedOfAllFlag = false;
           
      if (enableMetricMultiFidOptions) {
         fad.getSelectedMetricAtlasData(atlasPath, 
                                        atlasTopoFileName, 
                                        description,
                                        atlasCoordFileNames, 
                                        atlasAvgCoordFileName, 
                                        metricNameHint,
                                        structureName,
                                        mapToAvgCoordFileFlag, 
                                        mapToAvgOfAllFlag, 
                                        mapToStdDevOfAllFlag,
                                        mapToStdErrorOfAllFlag, 
                                        mapToMinOfAllFlag, 
                                        mapToMaxOfAllFlag,
                                        mapToAllCasesFlag);
      
      }
      else if (enablePaintMultiFidOptions) {
         fad.getSelectedPaintAtlasData(atlasPath, 
                                        atlasTopoFileName, 
                                        description,
                                        atlasCoordFileNames, 
                                        atlasAvgCoordFileName, 
                                        metricNameHint,
                                        structureName,
                                        mapToAvgCoordFileFlag, 
                                        mapToMostCommonOfAllFlag, 
                                        mapToMostCommonExcludeUnidentifiedOfAllFlag,
                                        mapToAllCasesFlag);
      }
      
      if ((atlasCoordFileNames.empty() == false) ||
          (atlasAvgCoordFileName.isEmpty() == false)) {
         //
         // Create the mapping set for use atlas to map to caret metric file
         //
         mappingSets.push_back(GuiMapFmriMappingSet(atlasPath,
                                                    atlasTopoFileName,
                                                    atlasCoordFileNames,
                                                    atlasAvgCoordFileName,
                                                    volumesToBeMapped,
                                                    outputDataFileExtension,
                                                    description,
                                                    structureName,
                                                    mapToAvgCoordFileFlag,
                                                    mapToAvgOfAllFlag,
                                                    mapToStdDevOfAllFlag,
                                                    mapToStdErrorOfAllFlag,
                                                    mapToMinOfAllFlag,
                                                    mapToMaxOfAllFlag,
                                                    mapToAllCasesFlag,
                                                    mapToMostCommonOfAllFlag,
                                                    mapToMostCommonExcludeUnidentifiedOfAllFlag));
      }
   }
   
   loadMappingSetsListBox();
}
      
/**
 * Called when add caret surfaces pushbutton pressed.
 */
void 
GuiMapFmriDialog::slotAddCaretSurfacesPushButton()
{
   bool haveAnySurface = false;
   std::vector<BrainModelSurface*> surfaces;
   getFiducialSurfaces(surfaces, haveAnySurface);
   
   std::vector<QString> names;
   for (int i = 0; i < static_cast<int>(surfaces.size()); i++) {
      names.push_back(surfaces[i]->getDescriptiveName());
   }   
   
   int defaultIndex = -1;
   if (names.size() == 1) {
      defaultIndex = 0;
   }
   
   QtListBoxSelectionDialog lbsd(this,
                                 "Select Caret Surface(s)",
                                 "",
                                 names,
                                 "Select All Surfaces",
                                 defaultIndex);
   lbsd.setAllowMultipleItemSelection(true);

   if (lbsd.exec() == QDialog::Accepted) {
      std::vector<int> indices;
      lbsd.getSelectedItemsIndices(indices);
      std::vector<BrainModelSurface*> surfacesSelected;
      for (int i = 0; i < static_cast<int>(indices.size()); i++) {
         surfacesSelected.push_back(surfaces[indices[i]]);
      }
      if (surfacesSelected.empty() == false) {
         mappingSets.push_back(GuiMapFmriMappingSet(surfacesSelected, volumesToBeMapped, 
                                                    outputDataFileExtension, ""));
      }
   }
   
   loadMappingSetsListBox();
}

/**
 * Get the Caret fiducial surfaces.
 */
void
GuiMapFmriDialog::getFiducialSurfaces(std::vector<BrainModelSurface*>& surfaces,
                                      bool& haveAnySurface) const
{
   haveAnySurface = false;
   surfaces.clear();
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
      BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(i);
      if (bms != NULL) {
         haveAnySurface = true;
         if ((bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_RAW) ||
             (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL)) {
            surfaces.push_back(bms);
         }
      }
   }
}

/**
 * reset (erase) the mapping sets.
 */
void 
GuiMapFmriDialog::resetMappingSets()
{
   mappingSets.clear();
}      

/**
 * load the mapping sets list box.
 */
void 
GuiMapFmriDialog::loadMappingSetsListBox()
{
   //
   // Remove everything from the list box
   //
   mappingSetListBox->clear();
   
   for (int i = 0; i < static_cast<int>(mappingSets.size()); i++) {
      QString label;
      mappingSetListBox->addItem(mappingSets[i].getDescriptiveLabel());
   }
   
   slotSetAppropriatePages();
}

      
/**
 * Called when add spec files pushbutton pressed.
 */
void 
GuiMapFmriDialog::slotAddSpecFilesPushButton()
{
   //
   // List the topo and coord files for selection
   //
   GuiMapFmriSpecFileTopoCoordDialog sftcd(this, theMainWindow->getBrainSet()->getPreferencesFile());
   if (sftcd.exec() == QDialog::Accepted) {
      //
      // Create a new mapping set
      //
      const QString topoName(sftcd.getTopoFileName());
      std::vector<QString> coordNames;
      sftcd.getCoordFileNames(coordNames);
      const QString specFileName = sftcd.getSpecFileName();
      mappingSets.push_back(GuiMapFmriMappingSet(specFileName,
                                                 FileUtilities::dirname(specFileName),
                                                 topoName,
                                                 coordNames,
                                                 "",
                                                 volumesToBeMapped,
                                                 outputDataFileExtension,
                                                 "",
                                                 "",
                                                 "",
                                                 false,
                                                 false,
                                                 false,
                                                 false,
                                                 false,
                                                 false,
                                                 false,
                                                 true,
                                                 false,
                                                 false));
   }
   
   loadMappingSetsListBox();
}

/**
 * Called when remove mapping sets (caret surfaces/spec files) pushbutton pressed.
 */
void 
GuiMapFmriDialog::slotRemoveMappingSetsPushButton()
{
   for (int i = static_cast<int>(mappingSets.size()) - 1; i >= 0; i--) {
      const QListWidgetItem* item = mappingSetListBox->item(i);
      if (mappingSetListBox->isItemSelected(item)) {
         mappingSets.erase(mappingSets.begin() + i);
      }
   }
   loadMappingSetsListBox();
}

/**
 * create the metric naming page.
 */
QWidget* 
GuiMapFmriDialog::createMetricNamingPage()
{
   //
   // label and combo box for mapping set selection
   //
   QLabel* surfaceFamilyLabel = new QLabel("Surface Family ");
   metricMappingSetComboBox = new QComboBox;
   QObject::connect(metricMappingSetComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotLoadMetricNamingPage()));
   QObject::connect(metricMappingSetComboBox, SIGNAL(highlighted(int)),
                    this, SLOT(slotReadMetricNamingPage()));
   
   //
   // label and data file selection
   //
   metricFilePushButton = new QPushButton("Data File...");
   metricFilePushButton->setFixedSize(metricFilePushButton->sizeHint());
   metricFilePushButton->setAutoDefault(false);
   QObject::connect(metricFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotMetricFilePushButton()));
   metricFileNameLineEdit = new QLineEdit;
   
   //
   // Grid for surface family and data file selections
   //
   QGridLayout* surfaceAndDataFileGridLayout = new QGridLayout;
   surfaceAndDataFileGridLayout->addWidget(surfaceFamilyLabel, 0, 0);
   surfaceAndDataFileGridLayout->addWidget(metricMappingSetComboBox, 0, 1);
   surfaceAndDataFileGridLayout->addWidget(metricFilePushButton, 1, 0);
   surfaceAndDataFileGridLayout->addWidget(metricFileNameLineEdit, 1, 1);
   surfaceAndDataFileGridLayout->setColumnStretch(0, 0);
   surfaceAndDataFileGridLayout->setColumnStretch(1, 100);
   
   //
   // Metric naming table
   //
   metricNamingTable = new QTableWidget;
   metricNamingTable->setColumnCount(METRIC_NAMING_NUMBER_OF_COLUMNS);
//QT4   metricNamingTable->setColumnReadOnly(METRIC_NAMING_COLUMN_SURFACE, true);
//   metricNamingTable->setColumnReadOnly(METRIC_NAMING_COLUMN_VOLUME, true);
//   metricNamingTable->setColumnReadOnly(METRIC_NAMING_COLUMN_SUB_VOLUME, true);
   
   //
   // Preset some column widths
   //
   metricNamingTable->setColumnWidth(METRIC_NAMING_COLUMN_NAME, 200);
   metricNamingTable->setColumnWidth(METRIC_NAMING_COLUMN_COMMENT, 150);
   metricNamingTable->setColumnWidth(METRIC_NAMING_COLUMN_SURFACE, 400);
   metricNamingTable->setColumnWidth(METRIC_NAMING_COLUMN_VOLUME, 400);
   metricNamingTable->setColumnWidth(METRIC_NAMING_COLUMN_SUB_VOLUME, 200);
   metricNamingTable->setColumnWidth(METRIC_NAMING_COLUMN_NEG_THRESH, 100);
   metricNamingTable->setColumnWidth(METRIC_NAMING_COLUMN_POS_THRESH, 100);
   
   //
   // Signal if a metric naming table cell is double clicked
   //
   QObject::connect(metricNamingTable, SIGNAL(cellDoubleClicked(int,int)),
                    this, SLOT(slotMetricNamingTableCellDoubleClicked(int,int)));
                    
   //
   // Metric naming table headers
   //
   QStringList headerLabels;
   headerLabels << "Data Column Name";
   headerLabels << "Data Column Comment";
   headerLabels << "Surface File";
   headerLabels << "Volume File";
   headerLabels << "Sub Volume";
   headerLabels << "Neg Thresh";
   headerLabels << "Pos Thresh";
   metricNamingTable->setHorizontalHeaderLabels(headerLabels);
/*
   QHeaderView* header = metricNamingTable->horizontalHeader();
   header->setLabel(METRIC_NAMING_COLUMN_NAME,    "Data Column Name");
   header->setLabel(METRIC_NAMING_COLUMN_COMMENT, "Data Column Comment");
   header->setLabel(METRIC_NAMING_COLUMN_SURFACE, "Surface File");
   header->setLabel(METRIC_NAMING_COLUMN_VOLUME,  "Volume File");
   header->setLabel(METRIC_NAMING_COLUMN_SUB_VOLUME, "Sub Volume");
   header->setLabel(METRIC_NAMING_COLUMN_NEG_THRESH, "Neg Thresh");
   header->setLabel(METRIC_NAMING_COLUMN_POS_THRESH, "Pos Thresh");
*/
   
   QGroupBox* namingGroupBox = new QGroupBox("Data File Naming");
   QVBoxLayout* namingGroupLayout = new QVBoxLayout(namingGroupBox);
   namingGroupLayout->addLayout(surfaceAndDataFileGridLayout);
   namingGroupLayout->addWidget(metricNamingTable);
   namingGroupLayout->addWidget(new QLabel("Double-click a cell to edit the cell's contents in a text editor"));
   
   const QString infoMessage = 
      "If mapping to more than one spec file, use the <B>Surface Family</B> control to choose "
      "the spec file so that its metric column names and comments may be set.<P>"
      "Note: When avolume is mapped, a description of the surface, volume, "
      "mapping parameters will be added to the column comment information.";
   QWidget* infoWidget = addPageInformation("Data File Naming Information", infoMessage);
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(namingGroupBox);
   layout->addWidget(infoWidget);
   
   return w;
}

/**
 * called when a cell in the metric naming table is double clicked.
 */
void 
GuiMapFmriDialog::slotMetricNamingTableCellDoubleClicked(int row, int col)
{
   //
   // Get the table widget item that was clicked
   //
   QTableWidgetItem* twi = metricNamingTable->item(row, col);
   
   //
   // Load the text from the table's cell into a text editor
   //
   QtTextEditDialog te(this,
                       false, // NOT read-only
                       true); // modal
   te.setText(twi->text());
   if (te.exec() == QtTextEditDialog::Accepted) {
      twi->setText(te.getText());
   }
}
      
/**
 * called when metric file name select button pressed.
 */
void 
GuiMapFmriDialog::slotMetricFilePushButton()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(WuQFileDialog::AcceptSave);
   fd.setWindowTitle("Choose Data File");
   fd.setFileMode(WuQFileDialog::AnyFile);
   
   QStringList fileFilters;
   switch (dataMappingType) {
      case DATA_MAPPING_TYPE_NONE:
         break;
      case DATA_MAPPING_TYPE_METRIC:
         fileFilters << FileFilters::getMetricFileFilter();
         fileFilters << FileFilters::getSurfaceShapeFileFilter();
         break;
      case DATA_MAPPING_TYPE_PAINT:
         fileFilters << FileFilters::getPaintFileFilter();
         fileFilters << FileFilters::getProbAtlasFileFilter();
         break;
   }
   fd.setFilters(fileFilters);
   fd.selectFilter(fileFilters.at(0));
   fd.selectFile(metricFileNameLineEdit->text());
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         metricFileNameLineEdit->setText(fd.selectedFiles().at(0)); 
      }
   }
}

/**
 * Load the metric naming combo box.
 */
void 
GuiMapFmriDialog::loadMetricNamingComboBox()
{
   metricMappingSetComboBox->clear();
   
   for (int i = 0; i < static_cast<int>(mappingSets.size()); i++) {
      metricMappingSetComboBox->addItem(mappingSets[i].getDescriptiveLabel());
   }
}      

/**
 * Read the metric naming page
 */
void
GuiMapFmriDialog::slotReadMetricNamingPage()
{
   //
   // Is there anything in the combo box ?
   //
   if (metricMappingSetComboBox->count() > 0) {
      //
      // Get the current item of the combo box
      //
      const int mappingSetIndex = metricMappingSetComboBox->currentIndex();
      if ((mappingSetIndex >= 0) && 
          (mappingSetIndex < static_cast<int>(mappingSets.size()))) {
         //
         // Get the mapping set and its metric file
         //
         GuiMapFmriMappingSet& ms = mappingSets[mappingSetIndex];
      
         //
         // Read the metric file name
         //
         ms.setMetricFileName(metricFileNameLineEdit->text());
         
         //
         // Get the number of metric mapping info
         //
         const int numMetricInfo = ms.getNumberOfMetricMappingInfo();
      
         //
         // Loop through metric info
         //
         for (int i = 0; i < numMetricInfo; i++) {
            //
            // Get the metric info
            //
            GuiMapFmriMetricInfo* metricInfo = ms.getMetricMappingInfo(i);
            
            //
            // Get number of metric columns (one per surface)
            //
            const int numCols = metricInfo->getNumberOfMetricColumns();
            
            //
            // Info about functional volume
            //
            const int volumeIndex = metricInfo->getVolumeIndex();
            const int subVolumeIndex = metricInfo->getSubVolumeIndex();
            const QString volumeName(volumesToBeMapped[volumeIndex]->getDescriptiveName());
            const QString subVolumeName(volumesToBeMapped[volumeIndex]->getSubVolumeName(subVolumeIndex));

            //
            // Process each metric column
            //
            for (int j = 0; j < numCols; j++) {
               //
               // If columns are being output
               //
               const int rowNumber = metricInfo->getMetricColumnNameRowNumber(j);
               if (rowNumber >= 0) {
                  //
                  // Set the table data
                  //
                  metricInfo->setMetricColumnName(j,
                          metricNamingTable->item(rowNumber, METRIC_NAMING_COLUMN_NAME)->text());
                  metricInfo->setMetricColumnComment(j,
                          metricNamingTable->item(rowNumber, METRIC_NAMING_COLUMN_COMMENT)->text());
                  metricInfo->setMetricColumnThresholds(j,
                          metricNamingTable->item(rowNumber, 
                                                  METRIC_NAMING_COLUMN_NEG_THRESH)->text().toFloat(),
                          metricNamingTable->item(rowNumber,
                                                  METRIC_NAMING_COLUMN_POS_THRESH)->text().toFloat());
               }
            }
            
            
            //
            // Add a row for average fiducial if needed
            //
            const int avgFiducialRowNumber = metricInfo->getMetricAverageFiducialCoordNameRowNumber();
            if (avgFiducialRowNumber >= 0) {
               //
               // Set the table data
               //
               metricInfo->setMetricAverageFiducialCoordColumnName(
                       metricNamingTable->item(avgFiducialRowNumber, METRIC_NAMING_COLUMN_NAME)->text());
               metricInfo->setMetricAverageFiducialCoordComment(
                       metricNamingTable->item(avgFiducialRowNumber, METRIC_NAMING_COLUMN_COMMENT)->text());
               metricInfo->setMetricAverageFiducialCoordThresholds(
                       metricNamingTable->item(avgFiducialRowNumber, 
                                               METRIC_NAMING_COLUMN_NEG_THRESH)->text().toFloat(),
                       metricNamingTable->item(avgFiducialRowNumber,
                                               METRIC_NAMING_COLUMN_POS_THRESH)->text().toFloat());
            }
            
            //
            // Add a row for average if needed
            //
            const int avgRowNumber = metricInfo->getMetricAverageOfAllColumnNameRowNumber();
            if (avgRowNumber >= 0) {
               //
               // Set the table data
               //
               metricInfo->setMetricAverageOfAllColumnName(
                       metricNamingTable->item(avgRowNumber, METRIC_NAMING_COLUMN_NAME)->text());
               metricInfo->setMetricAverageOfAllComment(
                       metricNamingTable->item(avgRowNumber, METRIC_NAMING_COLUMN_COMMENT)->text());
               metricInfo->setMetricAverageOfAllThresholds(
                       metricNamingTable->item(avgRowNumber, 
                                               METRIC_NAMING_COLUMN_NEG_THRESH)->text().toFloat(),
                       metricNamingTable->item(avgRowNumber,
                                               METRIC_NAMING_COLUMN_POS_THRESH)->text().toFloat());
            }
            
            //
            // Add a row for most common if needed
            //
            const int mostCommonRowNumber = metricInfo->getMetricMostCommonValueColumnNameRowNumber();
            if (mostCommonRowNumber >= 0) {
               //
               // Set the table data
               //
               metricInfo->setMetricMostCommonValueColumnName(
                       metricNamingTable->item(mostCommonRowNumber, METRIC_NAMING_COLUMN_NAME)->text());
               metricInfo->setMetricMostCommonValueComment(
                       metricNamingTable->item(mostCommonRowNumber, METRIC_NAMING_COLUMN_COMMENT)->text());
               metricInfo->setMetricMostCommonValueThresholds(
                       metricNamingTable->item(mostCommonRowNumber, 
                                               METRIC_NAMING_COLUMN_NEG_THRESH)->text().toFloat(),
                       metricNamingTable->item(mostCommonRowNumber,
                                               METRIC_NAMING_COLUMN_POS_THRESH)->text().toFloat());
            }
            
            //
            // Add a row for most common exclude unidentified if needed
            //
            const int mostCommonExcludeUnidentifiedRowNumber = metricInfo->getMetricMostCommonExcludeUnidentifiedValueColumnNameRowNumber();
            if (mostCommonExcludeUnidentifiedRowNumber >= 0) {
               //
               // Set the table data
               //
               metricInfo->setMetricMostCommonExcludeUnidentifiedValueColumnName(
                       metricNamingTable->item(mostCommonExcludeUnidentifiedRowNumber, METRIC_NAMING_COLUMN_NAME)->text());
               metricInfo->setMetricMostCommonExcludeUnidentifiedValueComment(
                       metricNamingTable->item(mostCommonExcludeUnidentifiedRowNumber, METRIC_NAMING_COLUMN_COMMENT)->text());
               metricInfo->setMetricMostCommonExcludeUnidentifiedValueThresholds(
                       metricNamingTable->item(mostCommonExcludeUnidentifiedRowNumber, 
                                               METRIC_NAMING_COLUMN_NEG_THRESH)->text().toFloat(),
                       metricNamingTable->item(mostCommonRowNumber,
                                               METRIC_NAMING_COLUMN_POS_THRESH)->text().toFloat());
            }
            
            //
            // Add a row for deviation if needed
            //
            const int stdDevRowNumber = metricInfo->getMetricStdDevColumnNameRowNumber();
            if (stdDevRowNumber >= 0) {
               //
               // Set the table data
               //
               metricInfo->setMetricStdDevColumnName(
                         metricNamingTable->item(stdDevRowNumber, METRIC_NAMING_COLUMN_NAME)->text());
               metricInfo->setMetricStdDevComment(
                         metricNamingTable->item(stdDevRowNumber, METRIC_NAMING_COLUMN_COMMENT)->text());
               metricInfo->setMetricStdDevThresholds(
                       metricNamingTable->item(stdDevRowNumber, 
                                               METRIC_NAMING_COLUMN_NEG_THRESH)->text().toFloat(),
                       metricNamingTable->item(stdDevRowNumber,
                                               METRIC_NAMING_COLUMN_POS_THRESH)->text().toFloat());
            }

            //
            // Add a row for standard error if needed
            //
            const int stdErrorRowNumber = metricInfo->getMetricStdErrorColumnNameRowNumber();
            if (stdErrorRowNumber >= 0) {
               //
               // Set the table data
               //
               metricInfo->setMetricStdErrorColumnName(
                   metricNamingTable->item(stdErrorRowNumber, METRIC_NAMING_COLUMN_NAME)->text());
               metricInfo->setMetricStdErrorComment(
                   metricNamingTable->item(stdErrorRowNumber, METRIC_NAMING_COLUMN_COMMENT)->text());
               metricInfo->setMetricStdErrorThresholds(
                       metricNamingTable->item(stdErrorRowNumber, 
                                               METRIC_NAMING_COLUMN_NEG_THRESH)->text().toFloat(),
                       metricNamingTable->item(stdErrorRowNumber,
                                               METRIC_NAMING_COLUMN_POS_THRESH)->text().toFloat());
            }

            //
            // Add a row for minimum value if needed
            //
            const int minValueRowNumber = metricInfo->getMetricMinValueColumnNameRowNumber();
            if (minValueRowNumber >= 0) {
               //
               // Set the table data
               //
               metricInfo->setMetricMinValueColumnName(
                   metricNamingTable->item(minValueRowNumber, METRIC_NAMING_COLUMN_NAME)->text());
               metricInfo->setMetricMinValueComment(
                   metricNamingTable->item(minValueRowNumber, METRIC_NAMING_COLUMN_COMMENT)->text());
               metricInfo->setMetricMinValueThresholds(
                       metricNamingTable->item(minValueRowNumber, 
                                               METRIC_NAMING_COLUMN_NEG_THRESH)->text().toFloat(),
                       metricNamingTable->item(minValueRowNumber,
                                               METRIC_NAMING_COLUMN_POS_THRESH)->text().toFloat());
            }

            //
            // Add a row for maximum value if needed
            //
            const int maxValueRowNumber = metricInfo->getMetricMaxValueColumnNameRowNumber();
            if (maxValueRowNumber >= 0) {
               //
               // Set the table data
               //
               metricInfo->setMetricMaxValueColumnName(
                   metricNamingTable->item(maxValueRowNumber, METRIC_NAMING_COLUMN_NAME)->text());
               metricInfo->setMetricMaxValueComment(
                   metricNamingTable->item(maxValueRowNumber, METRIC_NAMING_COLUMN_COMMENT)->text());
               metricInfo->setMetricMaxValueThresholds(
                       metricNamingTable->item(maxValueRowNumber, 
                                               METRIC_NAMING_COLUMN_NEG_THRESH)->text().toFloat(),
                       metricNamingTable->item(maxValueRowNumber,
                                               METRIC_NAMING_COLUMN_POS_THRESH)->text().toFloat());
            }
         }      
      }
   }
}

/**
 * load the metric naming page
 */
void
GuiMapFmriDialog::slotLoadMetricNamingPage()
{
   const int mappingSetIndex = metricMappingSetComboBox->currentIndex();
   if (mappingSetIndex < static_cast<int>(mappingSets.size())) {
      //
      // Get the mapping set and its metric file
      //
      GuiMapFmriMappingSet& ms = mappingSets[mappingSetIndex];
      
      //
      // Total number of rows in the table
      //
      int totalRows = 0;
      
      //
      //  Metric File name
      //
      metricFileNameLineEdit->setText(ms.getMetricFileName());
      
      //
      // Hide metric file items if mapping within caret
      //
      metricFilePushButton->setHidden(ms.getMappingType() != GuiMapFmriMappingSet::MAPPING_TYPE_WITH_SPEC_FILES);
      metricFileNameLineEdit->setHidden(ms.getMappingType() != GuiMapFmriMappingSet::MAPPING_TYPE_WITH_SPEC_FILES);

      //
      // Get the number of metric mapping info
      //
      const int numMetricInfo = ms.getNumberOfMetricMappingInfo();
      
      //
      // Loop through metric info
      //
      for (int i = 0; i < numMetricInfo; i++) {
         //
         // Get the metric info
         //
         GuiMapFmriMetricInfo* metricInfo = ms.getMetricMappingInfo(i);
         
         //
         // Get number of metric columns (one per surface)
         //
         const int numCols = metricInfo->getNumberOfMetricColumns();
         
         //
         // Add a row for average fiducial if needed
         //
         if (ms.getDoAverageFiducialFileFlag()) {
            metricInfo->setMetricAverageFiducialCoordNameRowNumber(totalRows);
            totalRows++;
         }
         else {
            metricInfo->setMetricAverageFiducialCoordNameRowNumber(-1);
         }
         
         //
         // Count number of case (indiv) columns
         //
         int numColumnsBeingMapped = 0;
         for (int j = 0; j < numCols; j++) {
            numColumnsBeingMapped++;
         }
         
         //
         // Add a row for average if needed
         //
         if (ms.getDoAvgOfAllCoordFileFlag() && (numColumnsBeingMapped > 1)) {
            metricInfo->setMetricAverageOfAllColumnNameRowNumber(totalRows);
            totalRows++;
         }
         else {
            metricInfo->setMetricAverageOfAllColumnNameRowNumber(-1);
         }
         
         //
         // Add a row for most common if needed
         //
         if (ms.getDoMostCommonOfAllCasesFlag() && (numColumnsBeingMapped > 1)) {
            metricInfo->setMetricMostCommonValueColumnNameRowNumber(totalRows);
            totalRows++;
         }
         else {
            metricInfo->setMetricMostCommonValueColumnNameRowNumber(-1);
         }
         
         //
         // Add a row for most common exclude unidentified if needed
         //
         if (ms.getDoMostCommonExcludeUnidentifiedOfAllCasesFlag() && (numColumnsBeingMapped > 1)) {
            metricInfo->setMetricMostCommonExcludeUnidentifiedValueColumnNameRowNumber(totalRows);
            totalRows++;
         }
         else {
            metricInfo->setMetricMostCommonExcludeUnidentifiedValueColumnNameRowNumber(-1);
         }
         
         //
         // Add a row for deviation if needed
         //
         if (ms.getDoStdDevOfAllCoordFileFlag() && (numColumnsBeingMapped > 1)) {
            metricInfo->setMetricStdDevColumnNameRowNumber(totalRows);
            totalRows++;
         }
         else {
            metricInfo->setMetricStdDevColumnNameRowNumber(-1);
         }
         
         //
         // Add a row for standard error if needed
         //
         if (ms.getDoStdErrorOfAllCoordFileFlag() && (numColumnsBeingMapped > 1)) {
            metricInfo->setMetricStdErrorColumnNameRowNumber(totalRows);
            totalRows++;
         }
         else {
            metricInfo->setMetricStdErrorColumnNameRowNumber(-1);
         }
         
         //
         // Add a row for minimum value if needed
         //
         if (ms.getDoMinOfAllCoordFileFlag() && (numColumnsBeingMapped > 1)) {
            metricInfo->setMetricMinValueColumnNameRowNumber(totalRows);
            totalRows++;
         }
         else {
            metricInfo->setMetricMinValueColumnNameRowNumber(-1);
         }
         
         //
         // Add a row for maximum value if needed
         //
         if (ms.getDoMaxOfAllCoordFileFlag() && (numColumnsBeingMapped > 1)) {
            metricInfo->setMetricMaxValueColumnNameRowNumber(totalRows);
            totalRows++;
         }
         else {
            metricInfo->setMetricMaxValueColumnNameRowNumber(-1);
         }
         
         //
         // Process each metric column
         //
         for (int j = 0; j < numCols; j++) {
            //
            // If columns are being output
            //
            if (ms.getDoAllCasesCoordFileFlag()) {
               metricInfo->setMetricColumnNameRowNumber(j, totalRows);
               totalRows++;
            }
            else {
               metricInfo->setMetricColumnNameRowNumber(j, -1);
            }
         }
         
         //
         // Add a blank row between sets
         //
         totalRows++;
      }
      
      
      //
      // Set the number of rows for the table
      //
      metricNamingTable->setRowCount(totalRows);

      //
      // Set all rows to blanks
      //
      for (int i = 0; i < totalRows; i++) {
         metricNamingTable->setItem(i, METRIC_NAMING_COLUMN_NAME, new QTableWidgetItem(""));
         metricNamingTable->setItem(i, METRIC_NAMING_COLUMN_COMMENT, new QTableWidgetItem(""));
         metricNamingTable->setItem(i, METRIC_NAMING_COLUMN_SURFACE, new QTableWidgetItem(""));
         metricNamingTable->setItem(i, METRIC_NAMING_COLUMN_VOLUME, new QTableWidgetItem(""));
         metricNamingTable->setItem(i, METRIC_NAMING_COLUMN_SUB_VOLUME, new QTableWidgetItem(""));
         metricNamingTable->setItem(i, METRIC_NAMING_COLUMN_NEG_THRESH, new QTableWidgetItem(""));
         metricNamingTable->setItem(i, METRIC_NAMING_COLUMN_POS_THRESH, new QTableWidgetItem(""));
      }
      
      //
      // Loop through metric info
      //
      for (int i = 0; i < numMetricInfo; i++) {
         //
         // Get the metric info
         //
         GuiMapFmriMetricInfo* metricInfo = ms.getMetricMappingInfo(i);
         
         //
         // Get number of metric columns (one per surface)
         //
         const int numCols = metricInfo->getNumberOfMetricColumns();
         
         //
         // Info about functional volume
         //
         const int volumeIndex = metricInfo->getVolumeIndex();
         const int subVolumeIndex = metricInfo->getSubVolumeIndex();
         const QString volumeName(volumesToBeMapped[volumeIndex]->getDescriptiveName());
         const QString subVolumeName(volumesToBeMapped[volumeIndex]->getSubVolumeName(subVolumeIndex));

         //
         // Process each metric column
         //
         for (int j = 0; j < numCols; j++) {
            //
            // If columns are being output
            //
            const int rowNumber = metricInfo->getMetricColumnNameRowNumber(j);
            if (rowNumber >= 0) {
               //
               // Set the table data
               //
               metricNamingTable->setItem(rowNumber, METRIC_NAMING_COLUMN_NAME, 
                                          new QTableWidgetItem(metricInfo->getMetricColumnName(j)));
               metricNamingTable->setItem(rowNumber, METRIC_NAMING_COLUMN_COMMENT, 
                                          new QTableWidgetItem(metricInfo->getMetricColumnComment(j)));
               metricNamingTable->setItem(rowNumber, METRIC_NAMING_COLUMN_SURFACE, 
                                          new QTableWidgetItem(metricInfo->getSurfaceNameForMetricColumn(j)));
               metricNamingTable->setItem(rowNumber, METRIC_NAMING_COLUMN_VOLUME, 
                                          new QTableWidgetItem(volumeName));
               metricNamingTable->setItem(rowNumber, METRIC_NAMING_COLUMN_SUB_VOLUME, 
                                          new QTableWidgetItem(subVolumeName));
               float negThresh, posThresh;
               metricInfo->getMetricColumnThresholds(j, negThresh, posThresh);
               metricNamingTable->setItem(rowNumber, METRIC_NAMING_COLUMN_NEG_THRESH,
                                          new QTableWidgetItem(QString::number(negThresh, 'f', 3)));
               metricNamingTable->setItem(rowNumber, METRIC_NAMING_COLUMN_POS_THRESH,
                                          new QTableWidgetItem(QString::number(posThresh, 'f', 3)));
            }
         }
         
         //
         // Add a row for average fiducial if needed
         //
         const int avgFiducialRowNumber = metricInfo->getMetricAverageFiducialCoordNameRowNumber();
         if (avgFiducialRowNumber >= 0) {
            //
            // Set the table data
            //
            metricNamingTable->setItem(avgFiducialRowNumber, METRIC_NAMING_COLUMN_NAME, 
                                       new QTableWidgetItem(metricInfo->getMetricAverageFiducialCoordColumnName()));
            metricNamingTable->setItem(avgFiducialRowNumber, METRIC_NAMING_COLUMN_COMMENT, 
                                       new QTableWidgetItem(metricInfo->getMetricAverageFiducialCoordComment()));
            metricNamingTable->setItem(avgFiducialRowNumber, METRIC_NAMING_COLUMN_SURFACE,
                                       new QTableWidgetItem(ms.getAverageFiducialCoordFileName()));
            metricNamingTable->setItem(avgFiducialRowNumber, METRIC_NAMING_COLUMN_VOLUME, 
                                       new QTableWidgetItem(volumeName));
            metricNamingTable->setItem(avgFiducialRowNumber, METRIC_NAMING_COLUMN_SUB_VOLUME, 
                                       new QTableWidgetItem(subVolumeName));
            float negThresh, posThresh;
            metricInfo->getMetricAverageFiducialCoordThresholds(negThresh, posThresh);
            metricNamingTable->setItem(avgFiducialRowNumber, METRIC_NAMING_COLUMN_NEG_THRESH,
                                       new QTableWidgetItem(QString::number(negThresh, 'f', 3)));
            metricNamingTable->setItem(avgFiducialRowNumber, METRIC_NAMING_COLUMN_POS_THRESH,
                                       new QTableWidgetItem(QString::number(posThresh, 'f', 3)));
         }
         
         //
         // Add a row for average if needed
         //
         const int avgRowNumber = metricInfo->getMetricAverageOfAllColumnNameRowNumber();
         if (avgRowNumber >= 0) {
            //
            // Set the table data
            //
            metricNamingTable->setItem(avgRowNumber, METRIC_NAMING_COLUMN_NAME, 
                                       new QTableWidgetItem(metricInfo->getMetricAverageOfAllColumnName()));
            metricNamingTable->setItem(avgRowNumber, METRIC_NAMING_COLUMN_COMMENT, 
                                       new QTableWidgetItem(metricInfo->getMetricAverageOfAllComment()));
            metricNamingTable->setItem(avgRowNumber, METRIC_NAMING_COLUMN_SURFACE, 
                                       new QTableWidgetItem("All Surfaces"));
            metricNamingTable->setItem(avgRowNumber, METRIC_NAMING_COLUMN_VOLUME, 
                                       new QTableWidgetItem(volumeName));
            metricNamingTable->setItem(avgRowNumber, METRIC_NAMING_COLUMN_SUB_VOLUME, 
                                       new QTableWidgetItem(subVolumeName));
            float negThresh, posThresh;
            metricInfo->getMetricAverageOfAllThresholds(negThresh, posThresh);
            metricNamingTable->setItem(avgRowNumber, METRIC_NAMING_COLUMN_NEG_THRESH,
                                       new QTableWidgetItem(QString::number(negThresh, 'f', 3)));
            metricNamingTable->setItem(avgRowNumber, METRIC_NAMING_COLUMN_POS_THRESH,
                                       new QTableWidgetItem(QString::number(posThresh, 'f', 3)));
         }
         
         //
         // Add a row for most common if needed
         //
         const int mostCommonRowNumber = metricInfo->getMetricMostCommonValueColumnNameRowNumber();
         if (mostCommonRowNumber >= 0) {
            //
            // Set the table data
            //
            metricNamingTable->setItem(mostCommonRowNumber, METRIC_NAMING_COLUMN_NAME, 
                                       new QTableWidgetItem(metricInfo->getMetricMostCommonValueColumnName()));
            metricNamingTable->setItem(mostCommonRowNumber, METRIC_NAMING_COLUMN_COMMENT, 
                                       new QTableWidgetItem(metricInfo->getMetricMostCommonValueComment()));
            metricNamingTable->setItem(mostCommonRowNumber, METRIC_NAMING_COLUMN_SURFACE, 
                                       new QTableWidgetItem("All Surfaces"));
            metricNamingTable->setItem(mostCommonRowNumber, METRIC_NAMING_COLUMN_VOLUME, 
                                       new QTableWidgetItem(volumeName));
            metricNamingTable->setItem(mostCommonRowNumber, METRIC_NAMING_COLUMN_SUB_VOLUME, 
                                       new QTableWidgetItem(subVolumeName));
            float negThresh, posThresh;
            metricInfo->getMetricMostCommonValueThresholds(negThresh, posThresh);
            metricNamingTable->setItem(mostCommonRowNumber, METRIC_NAMING_COLUMN_NEG_THRESH,
                                       new QTableWidgetItem(QString::number(negThresh, 'f', 3)));
            metricNamingTable->setItem(mostCommonRowNumber, METRIC_NAMING_COLUMN_POS_THRESH,
                                       new QTableWidgetItem(QString::number(posThresh, 'f', 3)));
         }
         
         //
         // Add a row for most common exclude unidentified if needed
         //
         const int mostCommonExcludeUnidentifiedRowNumber = metricInfo->getMetricMostCommonExcludeUnidentifiedValueColumnNameRowNumber();
         if (mostCommonExcludeUnidentifiedRowNumber >= 0) {
            //
            // Set the table data
            //
            metricNamingTable->setItem(mostCommonExcludeUnidentifiedRowNumber, METRIC_NAMING_COLUMN_NAME, 
                                       new QTableWidgetItem(metricInfo->getMetricMostCommonExcludeUnidentifiedValueColumnName()));
            metricNamingTable->setItem(mostCommonExcludeUnidentifiedRowNumber, METRIC_NAMING_COLUMN_COMMENT, 
                                       new QTableWidgetItem(metricInfo->getMetricMostCommonExcludeUnidentifiedValueComment()));
            metricNamingTable->setItem(mostCommonExcludeUnidentifiedRowNumber, METRIC_NAMING_COLUMN_SURFACE, 
                                       new QTableWidgetItem("All Surfaces"));
            metricNamingTable->setItem(mostCommonExcludeUnidentifiedRowNumber, METRIC_NAMING_COLUMN_VOLUME, 
                                       new QTableWidgetItem(volumeName));
            metricNamingTable->setItem(mostCommonExcludeUnidentifiedRowNumber, METRIC_NAMING_COLUMN_SUB_VOLUME, 
                                       new QTableWidgetItem(subVolumeName));
            float negThresh, posThresh;
            metricInfo->getMetricMostCommonExcludeUnidentifiedValueThresholds(negThresh, posThresh);
            metricNamingTable->setItem(mostCommonExcludeUnidentifiedRowNumber, METRIC_NAMING_COLUMN_NEG_THRESH,
                                       new QTableWidgetItem(QString::number(negThresh, 'f', 3)));
            metricNamingTable->setItem(mostCommonExcludeUnidentifiedRowNumber, METRIC_NAMING_COLUMN_POS_THRESH,
                                       new QTableWidgetItem(QString::number(posThresh, 'f', 3)));
         }
         
         //
         // Add a row for deviation if needed
         //
         const int stdDevRowNumber = metricInfo->getMetricStdDevColumnNameRowNumber();
         if (stdDevRowNumber >= 0) {
            //
            // Set the table data
            //
            metricNamingTable->setItem(stdDevRowNumber, METRIC_NAMING_COLUMN_NAME, 
                                       new QTableWidgetItem(metricInfo->getMetricStdDevColumnName()));
            metricNamingTable->setItem(stdDevRowNumber, METRIC_NAMING_COLUMN_COMMENT, 
                                       new QTableWidgetItem(metricInfo->getMetricStdDevComment()));
            metricNamingTable->setItem(stdDevRowNumber, METRIC_NAMING_COLUMN_SURFACE, 
                                       new QTableWidgetItem("All Surfaces"));
            metricNamingTable->setItem(stdDevRowNumber, METRIC_NAMING_COLUMN_VOLUME, 
                                       new QTableWidgetItem(volumeName));
            metricNamingTable->setItem(stdDevRowNumber, METRIC_NAMING_COLUMN_SUB_VOLUME, 
                                       new QTableWidgetItem(subVolumeName));
            float negThresh, posThresh;
            metricInfo->getMetricStdDevThresholds(negThresh, posThresh);
            metricNamingTable->setItem(stdDevRowNumber, METRIC_NAMING_COLUMN_NEG_THRESH,
                                       new QTableWidgetItem(QString::number(negThresh, 'f', 3)));
            metricNamingTable->setItem(stdDevRowNumber, METRIC_NAMING_COLUMN_POS_THRESH,
                                       new QTableWidgetItem(QString::number(posThresh, 'f', 3)));
         }

         //
         // Add a row for standard error if needed
         //
         const int stdErrorRowNumber = metricInfo->getMetricStdErrorColumnNameRowNumber();
         if (stdErrorRowNumber >= 0) {
            //
            // Set the table data
            //
            metricNamingTable->setItem(stdErrorRowNumber, METRIC_NAMING_COLUMN_NAME, 
                                      new QTableWidgetItem(metricInfo->getMetricStdErrorColumnName()));
            metricNamingTable->setItem(stdErrorRowNumber, METRIC_NAMING_COLUMN_COMMENT, 
                                       new QTableWidgetItem(metricInfo->getMetricStdErrorComment()));
            metricNamingTable->setItem(stdErrorRowNumber, METRIC_NAMING_COLUMN_SURFACE, 
                                       new QTableWidgetItem("All Surfaces"));
            metricNamingTable->setItem(stdErrorRowNumber, METRIC_NAMING_COLUMN_VOLUME, 
                                       new QTableWidgetItem(volumeName));
            metricNamingTable->setItem(stdErrorRowNumber, METRIC_NAMING_COLUMN_SUB_VOLUME, 
                                       new QTableWidgetItem(subVolumeName));
            float negThresh, posThresh;
            metricInfo->getMetricStdErrorThresholds(negThresh, posThresh);
            metricNamingTable->setItem(stdErrorRowNumber, METRIC_NAMING_COLUMN_NEG_THRESH,
                                       new QTableWidgetItem(QString::number(negThresh, 'f', 3)));
            metricNamingTable->setItem(stdErrorRowNumber, METRIC_NAMING_COLUMN_POS_THRESH,
                                       new QTableWidgetItem(QString::number(posThresh, 'f', 3)));
         }

         //
         // Add a row for minimum value if needed
         //
         const int minValueRowNumber = metricInfo->getMetricMinValueColumnNameRowNumber();
         if (minValueRowNumber >= 0) {
            //
            // Set the table data
            //
            metricNamingTable->setItem(minValueRowNumber, METRIC_NAMING_COLUMN_NAME, 
                                       new QTableWidgetItem(metricInfo->getMetricMinValueColumnName()));
            metricNamingTable->setItem(minValueRowNumber, METRIC_NAMING_COLUMN_COMMENT, 
                                       new QTableWidgetItem(metricInfo->getMetricMinValueComment()));
            metricNamingTable->setItem(minValueRowNumber, METRIC_NAMING_COLUMN_SURFACE, 
                                       new QTableWidgetItem("All Surfaces"));
            metricNamingTable->setItem(minValueRowNumber, METRIC_NAMING_COLUMN_VOLUME, 
                                       new QTableWidgetItem(volumeName));
            metricNamingTable->setItem(minValueRowNumber, METRIC_NAMING_COLUMN_SUB_VOLUME, 
                                       new QTableWidgetItem(subVolumeName));
            float negThresh, posThresh;
            metricInfo->getMetricMinValueThresholds(negThresh, posThresh);
            metricNamingTable->setItem(minValueRowNumber, METRIC_NAMING_COLUMN_NEG_THRESH,
                                       new QTableWidgetItem(QString::number(negThresh, 'f', 3)));
            metricNamingTable->setItem(minValueRowNumber, METRIC_NAMING_COLUMN_POS_THRESH,
                                       new QTableWidgetItem(QString::number(posThresh, 'f', 3)));
         }

         //
         // Add a row for maximum value if needed
         //
         const int maxValueRowNumber = metricInfo->getMetricMaxValueColumnNameRowNumber();
         if (maxValueRowNumber >= 0) {
            //
            // Set the table data
            //
            metricNamingTable->setItem(maxValueRowNumber, METRIC_NAMING_COLUMN_NAME, 
                                       new QTableWidgetItem(metricInfo->getMetricMaxValueColumnName()));
            metricNamingTable->setItem(maxValueRowNumber, METRIC_NAMING_COLUMN_COMMENT, 
                                       new QTableWidgetItem(metricInfo->getMetricMaxValueComment()));
            metricNamingTable->setItem(maxValueRowNumber, METRIC_NAMING_COLUMN_SURFACE, 
                                       new QTableWidgetItem("All Surfaces"));
            metricNamingTable->setItem(maxValueRowNumber, METRIC_NAMING_COLUMN_VOLUME, 
                                       new QTableWidgetItem(volumeName));
            metricNamingTable->setItem(maxValueRowNumber, METRIC_NAMING_COLUMN_SUB_VOLUME, 
                                       new QTableWidgetItem(subVolumeName));
            float negThresh, posThresh;
            metricInfo->getMetricMaxValueThresholds(negThresh, posThresh);
            metricNamingTable->setItem(maxValueRowNumber, METRIC_NAMING_COLUMN_NEG_THRESH,
                                       new QTableWidgetItem(QString::number(negThresh, 'f', 3)));
            metricNamingTable->setItem(maxValueRowNumber, METRIC_NAMING_COLUMN_POS_THRESH,
                                       new QTableWidgetItem(QString::number(posThresh, 'f', 3)));
         }
      }      
   }
   else {
      metricNamingTable->setRowCount(0);
   }

   bool showThreshColumns = false;
   switch (dataMappingType) {   
      case DATA_MAPPING_TYPE_NONE:
         break;
      case DATA_MAPPING_TYPE_METRIC:
         showThreshColumns = true;
         break;
      case DATA_MAPPING_TYPE_PAINT:
         break;
   }
   if (showThreshColumns) {
      metricNamingTable->showColumn(METRIC_NAMING_COLUMN_NEG_THRESH);
      metricNamingTable->showColumn(METRIC_NAMING_COLUMN_POS_THRESH);
   }
   else {
      metricNamingTable->hideColumn(METRIC_NAMING_COLUMN_NEG_THRESH);
      metricNamingTable->hideColumn(METRIC_NAMING_COLUMN_POS_THRESH);
   }
}

/**
 * create the algorithm page.
 */
QWidget* 
GuiMapFmriDialog::createAlgorithmPage()
{
   std::vector<QString> algNames;
   std::vector<BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM> algValues;   
   BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmNamesAndValues(
                                                                        algNames,
                                                                        algValues);
   algorithmComboBox = new QComboBox;
   for (unsigned int i = 0; i < algNames.size(); i++) {
      algorithmComboBox->insertItem(algValues[i], algNames[i]);
   }
   QObject::connect(algorithmComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotAlgorithmComboBox(int)));

   //
   // Widget stack for algorithm parameters
   //
   algorithmParameterWidgetStack = new QStackedWidget;
   
   //
   // Parameters for average nodes algorithm
   //
   algorithmMetricAverageNodesParametersBox = new QWidget;
   algorithmParameterWidgetStack->addWidget(algorithmMetricAverageNodesParametersBox);
   
   //
   // Parameters for average voxel algorithm
   //
   QLabel* avgVoxLabel = new QLabel("Neighbor Box Size");
   algorithmAverageVoxelNeighborDoubleSpinBox = new QDoubleSpinBox;
   algorithmAverageVoxelNeighborDoubleSpinBox->setMinimum(0.0);
   algorithmAverageVoxelNeighborDoubleSpinBox->setMaximum(10000.0);
   algorithmAverageVoxelNeighborDoubleSpinBox->setSingleStep(1.0);
   algorithmAverageVoxelNeighborDoubleSpinBox->setDecimals(2);
   algorithmMetricAverageVoxelParametersBox = new QWidget;
   QHBoxLayout* avgVoxLayout = new QHBoxLayout(algorithmMetricAverageVoxelParametersBox);
   avgVoxLayout->addWidget(avgVoxLabel);
   avgVoxLayout->addWidget(algorithmAverageVoxelNeighborDoubleSpinBox);
   avgVoxLayout->addStretch();
   algorithmMetricAverageVoxelParametersBox->setFixedSize(algorithmMetricAverageVoxelParametersBox->sizeHint());
   algorithmParameterWidgetStack->addWidget(algorithmMetricAverageVoxelParametersBox);
   
   //
   // Parameters for enclosing voxel algorithm
   //
   algorithmMetricEnclosingVoxelParametersBox = new QWidget;
   algorithmParameterWidgetStack->addWidget(algorithmMetricEnclosingVoxelParametersBox);
   
   //
   // Parameters for gaussian algorithm
   //
   QLabel* gaussBoxLabel = new QLabel("Neighbor Box Size");
   algorithmGaussianNeighborDoubleSpinBox = new QDoubleSpinBox;
   algorithmGaussianNeighborDoubleSpinBox->setMinimum(0.0);
   algorithmGaussianNeighborDoubleSpinBox->setMaximum(10000.0);
   algorithmGaussianNeighborDoubleSpinBox->setSingleStep(1.0);
   algorithmGaussianNeighborDoubleSpinBox->setDecimals(3);
   QLabel* gaussSNLabel = new QLabel("Sigma Normal");
   algorithmGaussianSigmaNormDoubleSpinBox = new QDoubleSpinBox;
   algorithmGaussianSigmaNormDoubleSpinBox->setMinimum(0.01);
   algorithmGaussianSigmaNormDoubleSpinBox->setMaximum(10000.0);
   algorithmGaussianSigmaNormDoubleSpinBox->setSingleStep(1.0);
   algorithmGaussianSigmaNormDoubleSpinBox->setDecimals(3);
   QLabel* gaussSTLabel = new QLabel("Sigma Tangent");
   algorithmGaussianSigmaTangDoubleSpinBox = new QDoubleSpinBox;
   algorithmGaussianSigmaTangDoubleSpinBox->setMinimum(0.01);
   algorithmGaussianSigmaTangDoubleSpinBox->setMaximum(10000.0);
   algorithmGaussianSigmaTangDoubleSpinBox->setSingleStep(1.0);
   algorithmGaussianSigmaTangDoubleSpinBox->setDecimals(3);
   QLabel* gaussNBCLabel = new QLabel("Normal Below Cutoff");
   algorithmGaussianNormBelowDoubleSpinBox = new QDoubleSpinBox;
   algorithmGaussianNormBelowDoubleSpinBox->setMinimum(0.01);
   algorithmGaussianNormBelowDoubleSpinBox->setMaximum(10000.0);
   algorithmGaussianNormBelowDoubleSpinBox->setSingleStep(1.0);
   algorithmGaussianNormBelowDoubleSpinBox->setDecimals(3);
   QLabel* gaussNACLabel = new QLabel("Normal Above Cutoff");
   algorithmGaussianNormAboveDoubleSpinBox = new QDoubleSpinBox;
   algorithmGaussianNormAboveDoubleSpinBox->setMinimum(0.01);
   algorithmGaussianNormAboveDoubleSpinBox->setMaximum(10000.0);
   algorithmGaussianNormAboveDoubleSpinBox->setSingleStep(1.0);
   algorithmGaussianNormAboveDoubleSpinBox->setDecimals(3);
   QLabel* gaussTCLabel = new QLabel("Tangent Cutoff");
   algorithmGaussianTangCutoffDoubleSpinBox = new QDoubleSpinBox;
   algorithmGaussianTangCutoffDoubleSpinBox->setMinimum(0.01);
   algorithmGaussianTangCutoffDoubleSpinBox->setMaximum(10000.0);
   algorithmGaussianTangCutoffDoubleSpinBox->setSingleStep(1.0);
   algorithmGaussianTangCutoffDoubleSpinBox->setDecimals(3);
   algorithmMetricGaussianParametersBox = new QWidget;
   QGridLayout* gaussLayout = new QGridLayout(algorithmMetricGaussianParametersBox);
   gaussLayout->addWidget(gaussBoxLabel, 0, 0);
   gaussLayout->addWidget(algorithmGaussianNeighborDoubleSpinBox, 0, 1);
   gaussLayout->addWidget(gaussSNLabel, 1, 0);
   gaussLayout->addWidget(algorithmGaussianSigmaNormDoubleSpinBox, 1, 1);
   gaussLayout->addWidget(gaussSTLabel, 2, 0);
   gaussLayout->addWidget(algorithmGaussianSigmaTangDoubleSpinBox, 2, 1);
   gaussLayout->addWidget(gaussNBCLabel, 3, 0);
   gaussLayout->addWidget(algorithmGaussianNormBelowDoubleSpinBox, 3, 1);
   gaussLayout->addWidget(gaussNACLabel, 4, 0);
   gaussLayout->addWidget(algorithmGaussianNormAboveDoubleSpinBox, 4, 1);
   gaussLayout->addWidget(gaussTCLabel, 5, 0);
   gaussLayout->addWidget(algorithmGaussianTangCutoffDoubleSpinBox, 5, 1);
   algorithmMetricGaussianParametersBox->setFixedSize(algorithmMetricGaussianParametersBox->sizeHint());
   algorithmParameterWidgetStack->addWidget(algorithmMetricGaussianParametersBox);
   
   //
   // Parameters for interpolated voxel algorithm
   //
   algorithmMetricInterpolatedVoxelParametersBox = new QWidget;
   algorithmParameterWidgetStack->addWidget(algorithmMetricInterpolatedVoxelParametersBox);
   
   //
   // Parameters for maximum voxel algorithm
   //
   QLabel* maxLabel = new QLabel("Neighbor Box Size");
   algorithmMaximumVoxelNeighborDoubleSpinBox = new QDoubleSpinBox;
   algorithmMaximumVoxelNeighborDoubleSpinBox->setMinimum(0.0);
   algorithmMaximumVoxelNeighborDoubleSpinBox->setMaximum(10000.0);
   algorithmMaximumVoxelNeighborDoubleSpinBox->setSingleStep(1.0);
   algorithmMaximumVoxelNeighborDoubleSpinBox->setDecimals(2);
   algorithmMetricMaximumVoxelParametersBox = new QWidget;
   QHBoxLayout* maxVoxLayout = new QHBoxLayout(algorithmMetricMaximumVoxelParametersBox);
   maxVoxLayout->addWidget(maxLabel);
   maxVoxLayout->addWidget(algorithmMaximumVoxelNeighborDoubleSpinBox);
   maxVoxLayout->addStretch();
   algorithmMetricMaximumVoxelParametersBox->setFixedSize(algorithmMetricMaximumVoxelParametersBox->sizeHint());
   algorithmParameterWidgetStack->addWidget(algorithmMetricMaximumVoxelParametersBox);
   
   //
   // Parameters for MCW BrainFish algorithm
   //
   QLabel* mcwDistLabel = new QLabel("Max Distance");
   algorithmBrainFishMaxDistanceDoubleSpinBox = new QDoubleSpinBox;
   algorithmBrainFishMaxDistanceDoubleSpinBox->setMinimum(0.0);
   algorithmBrainFishMaxDistanceDoubleSpinBox->setMaximum(10000.0);
   algorithmBrainFishMaxDistanceDoubleSpinBox->setSingleStep(1.0);
   algorithmBrainFishMaxDistanceDoubleSpinBox->setDecimals(3);
   QLabel* mcwSplatLabel = new QLabel("Splat Factor");
   algorithmBrainFishSplatFactorSpinBox = new QSpinBox;
   algorithmBrainFishSplatFactorSpinBox->setMinimum(0);
   algorithmBrainFishSplatFactorSpinBox->setMaximum(10000);
   algorithmBrainFishSplatFactorSpinBox->setSingleStep(1);
   algorithmMetricMcwBrainFishParametersBox = new QWidget;
   QGridLayout* mcwLayout = new QGridLayout(algorithmMetricMcwBrainFishParametersBox);
   mcwLayout->addWidget(mcwDistLabel, 0, 0);
   mcwLayout->addWidget(algorithmBrainFishMaxDistanceDoubleSpinBox, 0, 1);
   mcwLayout->addWidget(mcwSplatLabel, 1, 0);
   mcwLayout->addWidget(algorithmBrainFishSplatFactorSpinBox, 1, 1);
   algorithmMetricMcwBrainFishParametersBox->setFixedSize(algorithmMetricMcwBrainFishParametersBox->sizeHint());
   algorithmParameterWidgetStack->addWidget(algorithmMetricMcwBrainFishParametersBox);

   //
   // Parameters for strongest voxel algorithm
   //
   QLabel* strongestLabel = new QLabel("Neighbor Box Size");
   algorithmStrongestVoxelNeighborDoubleSpinBox = new QDoubleSpinBox;
   algorithmStrongestVoxelNeighborDoubleSpinBox->setMinimum(0.0);
   algorithmStrongestVoxelNeighborDoubleSpinBox->setMaximum(10000.0);
   algorithmStrongestVoxelNeighborDoubleSpinBox->setSingleStep(1.0);
   algorithmStrongestVoxelNeighborDoubleSpinBox->setDecimals(2);
   algorithmMetricStrongestVoxelParametersBox = new QWidget;
   QHBoxLayout* strongestVoxLayout = new QHBoxLayout(algorithmMetricStrongestVoxelParametersBox);
   strongestVoxLayout->addWidget(strongestLabel);
   strongestVoxLayout->addWidget(algorithmStrongestVoxelNeighborDoubleSpinBox);
   strongestVoxLayout->addStretch();
   algorithmMetricStrongestVoxelParametersBox->setFixedSize(algorithmMetricStrongestVoxelParametersBox->sizeHint());
   algorithmParameterWidgetStack->addWidget(algorithmMetricStrongestVoxelParametersBox);
   
   //
   // Parameters for enclosing voxel algorithm
   //
   algorithmPaintEnclosingVoxelParametersBox = new QWidget;
   algorithmParameterWidgetStack->addWidget(algorithmPaintEnclosingVoxelParametersBox);
   
   //
   // Group box for mapping selection and parameters
   //
   QGroupBox* mappingGroupBox = new QGroupBox("Mapping Algorithm");
   QVBoxLayout* mappingGroupLayout = new QVBoxLayout(mappingGroupBox);
   mappingGroupLayout->addWidget(algorithmComboBox);
   mappingGroupLayout->addWidget(algorithmParameterWidgetStack);
   
   //
   // Help Text
   //   
   algorithmInformationTextEdit = new QTextBrowser;
   QWidget* infoWidget = addPageInformation("Mapping Algorithm Information", "", algorithmInformationTextEdit);
   
   //
   // widget for page and layout
   //
   QWidget* w = new QWidget;
   QVBoxLayout* algLayout = new QVBoxLayout(w);
   algLayout->addWidget(mappingGroupBox);
   algLayout->addWidget(infoWidget);
   
   return w;
}

/**
 * Load the algorithm parameters from the preferences file.
 */
void
GuiMapFmriDialog::loadAlgorithmParametersFromPreferences()
{
   static bool loadedFlag = false;
   if (loadedFlag) {
      return;
   }
   loadedFlag = true;
   
   //
   // Algorithm 
   //
   algorithmComboBox->setCurrentIndex(mappingParameters.getAlgorithm());
   slotAlgorithmComboBox(algorithmComboBox->currentIndex());
   
   //
   // Average voxel parameters
   //
   float avgVoxNeighbor = 1.0;
   mappingParameters.getAlgorithmMetricAverageVoxelParameters(avgVoxNeighbor);
   algorithmAverageVoxelNeighborDoubleSpinBox->setValue(avgVoxNeighbor);
   
   //
   // Maximum voxel parameters
   //
   float maxVoxNeighbor = 1.0;
   mappingParameters.getAlgorithmMetricMaximumVoxelParameters(maxVoxNeighbor);
   algorithmMaximumVoxelNeighborDoubleSpinBox->setValue(maxVoxNeighbor);
   
   //
   // Strongest voxel parameters
   //
   float strongestVoxNeighbor = 1.0;
   mappingParameters.getAlgorithmMetricStrongestVoxelParameters(strongestVoxNeighbor);
   algorithmStrongestVoxelNeighborDoubleSpinBox->setValue(strongestVoxNeighbor);
   
   //
   // Gaussian parameters
   //
   float gaussNeighbor;
   float gaussSigmaTang;
   float gaussSigmaNorm;
   float gaussNormBelow;
   float gaussNormAbove;
   float gaussTang;
   mappingParameters.getAlgorithmMetricGaussianParameters(gaussNeighbor,
                                 gaussSigmaTang,
                                 gaussSigmaNorm,
                                 gaussNormBelow,
                                 gaussNormAbove,
                                 gaussTang);
   algorithmGaussianNeighborDoubleSpinBox->setValue(gaussNeighbor);
   algorithmGaussianSigmaNormDoubleSpinBox->setValue(gaussSigmaNorm);
   algorithmGaussianSigmaTangDoubleSpinBox->setValue(gaussSigmaTang);
   algorithmGaussianNormBelowDoubleSpinBox->setValue(gaussNormBelow);
   algorithmGaussianNormAboveDoubleSpinBox->setValue(gaussNormAbove);
   algorithmGaussianTangCutoffDoubleSpinBox->setValue(gaussTang);
   
   //
   // MCW BrainFish Parameters
   //
   float bfMaxDist;
   int   bfSplat;
   mappingParameters.getAlgorithmMetricMcwBrainFishParameters(bfMaxDist,
                                                              bfSplat);
   algorithmBrainFishMaxDistanceDoubleSpinBox->setValue(bfMaxDist);
   algorithmBrainFishSplatFactorSpinBox->setValue(bfSplat);      
}

/**
 * Called when an algorithm is selected.
 */  
void
GuiMapFmriDialog::slotAlgorithmComboBox(int item)
{
   const BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM alg =
      static_cast<BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM>(item);
   mappingParameters.setAlgorithm(alg);
   
   QString infoText = "For all algorithms, the node's stereotaxic position identifies "
                      "a voxel in the volume.<P>";
   
   switch(alg) {
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_AVERAGE_NODES:
         algorithmParameterWidgetStack->setCurrentWidget(algorithmMetricAverageNodesParametersBox);
         infoText += "<B>Average Nodes Algorithm:</B><P>"
                     "Each node is set to the average of the voxel it falls within and "
                     "the voxels containing the node's connected neighbors.";
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_AVERAGE_VOXEL:
         algorithmParameterWidgetStack->setCurrentWidget(algorithmMetricAverageVoxelParametersBox);
         infoText += "<B>Average Voxel Algorithm:</B><P>"
                     "A cube with each edge <B>NEIGHBORHOOD SIZE</B> millimeters long is "
                     "centered at the node.  The node is assigned the average of "
                     "all voxels that are contained in the cube.";
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_ENCLOSING_VOXEL:
         algorithmParameterWidgetStack->setCurrentWidget(algorithmMetricEnclosingVoxelParametersBox);
         infoText += "<B>Enclosing Voxel Algorithm:</B><P>"
                     "The node is assigned the voxel that contains the node.";
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_GAUSSIAN:
         algorithmParameterWidgetStack->setCurrentWidget(algorithmMetricGaussianParametersBox);
         infoText += "<B>Gaussian Algorithm:</B><P>"
                     "The gaussian is defined by the equation e(-(norm*norm)/(sigma*sigma))<P>"
                     "A cube whose sides are of length <B>NEIGHBOR BOX SIZE</B> is placed around "
                     "each node.  All voxels within this box contribute to the node's value.<P>"
                     "<B>SIGMA NORMAL</B> ";
         infoText += StringUtilities::replace(
                           GaussianComputation::tooltipTextForSigmaNorm(), '\n', ' ');
         infoText += "<P>";
         infoText += "<B>SIGMA TANGENT</B> ";
         infoText += StringUtilities::replace(
                           GaussianComputation::tooltipTextForSigmaTang(), '\n', ' ');
         infoText += "<P>";
         infoText += "<B>NORMAL BELOW CUTOFF</B> ";
         infoText += StringUtilities::replace(
                           GaussianComputation::tooltipTextForNormBelowCutoff(), '\n', ' ');
         infoText += "<P>";
         infoText += "<B>NORMAL ABOVE CUTOFF</B> ";
         infoText += StringUtilities::replace(
                           GaussianComputation::tooltipTextForNormAboveCutoff(), '\n', ' ');
         infoText += "<P>";
         infoText += "<B>TANGENT CUTOFF</B> ";
         infoText += StringUtilities::replace(
                           GaussianComputation::tooltipTextForTangentCutoff(), '\n', ' ');
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_INTERPOLATED_VOXEL:
         algorithmParameterWidgetStack->setCurrentWidget(algorithmMetricInterpolatedVoxelParametersBox);
         infoText += "<B>Interpolated Voxel Algorithm:</B><P>"
                     "The node containing the voxel is determined.  A cube the "
                     "size of the volume's voxel size is centered at the node. "
                     "This cube intersects the voxel containing the node and "
                     "parts of neighboring voxels.  A weighted average is assigned "
                     "to the node based upon the cubes intersection with the neighboring "
                     "voxels.  If the node is at the exact center of a voxel, the value "
                     "assigned to the node would be the same as the Enclosing Voxel "
                     "Algorithm";
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_MAXIMUM_VOXEL:
         algorithmParameterWidgetStack->setCurrentWidget(algorithmMetricMaximumVoxelParametersBox);
         infoText += "<B>Maximum Voxel Algorithm:</B><P>"
                     "A cube with each edge <B>NEIGHBORHOOD SIZE</B> millimeters long is "
                     "centered at the node.  The node is assigned the maximum of "
                     "all voxels that are contained in the cube.  If the cube "
                     "contains only voxels with negative values, the node will be "
                     "assigned the most negative value.  If the cube contains any "
                     "positive values, the node will be assigned the most positive value.";
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_MCW_BRAINFISH:
         algorithmParameterWidgetStack->setCurrentWidget(algorithmMetricMcwBrainFishParametersBox);
         infoText += "<B>MCW BrainFish Algorithm:</B><P>"
                     "This algorithm loops through the voxels.  For each voxel that "
                     "is non-zero, the algorithm find the closest node that is within "
                     "<B>MAX DISTANCE</B> millimeters.  If the node's current value is less "
                     "than that of the current voxel, the voxel is assigned to the node "
                     "The splat factor essentially smooths the nodes for <B>SPLAT FACTOR</B> "
                     "iterations.";
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_STRONGEST_VOXEL:
         algorithmParameterWidgetStack->setCurrentWidget(algorithmMetricStrongestVoxelParametersBox);
         infoText += "<B>Strongest Voxel Algorithm:</B><P>"
                     "A cube with each edge <B>NEIGHBORHOOD SIZE</B> millimeters long is "
                     "centered at the node.  The node is assigned the value of the "
                     "voxel that is furthest from zero. ";
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_PAINT_ENCLOSING_VOXEL:
         algorithmParameterWidgetStack->setCurrentWidget(algorithmPaintEnclosingVoxelParametersBox);
         infoText += "<B>Enclosing Voxel Algorithm:</B><P>"
                     "The node is assigned the voxel that contains the node.";
         break;
   }
   
   algorithmInformationTextEdit->setHtml(infoText);
}

/**
 * create the summary page.
 */
QWidget* 
GuiMapFmriDialog::createSummaryPage()
{
   //
   // Text edit for summary
   //
   summaryTextEdit = new QTextEdit;
   summaryTextEdit->setReadOnly(true);

   //
   // Group box and layout for summary
   //
   QGroupBox* summaryGroupBox = new QGroupBox("Summary");
   QVBoxLayout* summaryLayout = new QVBoxLayout(summaryGroupBox);
   summaryLayout->addWidget(summaryTextEdit);
   
   QWidget* w = new QWidget;
   QVBoxLayout* pageLayout = new QVBoxLayout(w);
   pageLayout->addWidget(summaryGroupBox);
   
   return w;
}

/**
 * Load the summary text edit
 */
void
GuiMapFmriDialog::loadSummaryTextEdit()
{
   QString text;
   
   //
   // For each mapping set
   //
   for (int i = 0; i < static_cast<int>(mappingSets.size()); i++) {
      if (i > 0) {
         text.append("---------------------------------------------------------------------\n");
      }
      
      //
      // Get mapping set
      //
      const GuiMapFmriMappingSet& mms = mappingSets[i];

      for (int m = 0; m < mms.getNumberOfMetricMappingInfo(); m++) {
         //
         // List the destination surface
         //
         switch (mms.getMetricOutputType()) {
            case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_NONE:
               break;
            case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_CARET_METRIC:
               if (mms.getMappingWithAtlasFlag()) {
                  text.append("Map to Caret5 With Atlas");
               }
               else {
                  text.append("Map to Surface(s) in Caret5");
               }
               text.append("\n");
               break;
            case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_SPEC_FILE:
               text.append("Map to Spec File: ");
               text.append(FileUtilities::basename(mms.getOutputSpecFileName()));
               if (mms.getMappingWithAtlasFlag()) {
                  text.append(" With Atlas");
               }
               text.append("\n");
               text.append("Metric File: ");
               text.append(mms.getMetricFileName());
               text.append("\n");
               break;
         }
         text.append("\n");
         
         //
         // Get the metric info
         //
         const GuiMapFmriMetricInfo* mi = mms.getMetricMappingInfo(m);
         const int volumeNumber = mi->getVolumeIndex();
         const int subVolumeNumber = mi->getSubVolumeIndex();
         const QString volumeName(volumesToBeMapped[volumeNumber]->getDescriptiveName());
         const QString subVolumeName(volumesToBeMapped[volumeNumber]->getSubVolumeName(subVolumeNumber));

         if (mms.getDoAverageFiducialFileFlag() &&
             (mi->getMetricAverageFiducialCoordNameRowNumber() >= 0)) {
            text.append("Average Fiducial: ");
            text.append(mi->getMetricAverageFiducialCoordColumnName());
            text.append("\n");
            //
            // Surface and Volume name
            //
            text.append("   using surface: ");
            text.append(FileUtilities::basename(mms.getAverageFiducialCoordFileName()));
            text.append("\n");
            text.append("   from volume: ");
            text.append(volumeName);
            text.append("\n");
            text.append("   sub-volume: ");
            text.append(subVolumeName);
            text.append("\n\n");
         }
         
         //
         // List average if applicable
         //
         if (mms.getDoAvgOfAllCoordFileFlag() &&
             (mi->getMetricAverageOfAllColumnNameRowNumber() >= 0)) {
            text.append("Average Column: ");
            text.append(mi->getMetricAverageOfAllColumnName());
            text.append("\n\n");
         }
         
         //
         // List most common if applicable
         //
         if (mms.getDoMostCommonOfAllCasesFlag() &&
             (mi->getMetricMostCommonValueColumnNameRowNumber() >= 0)) {
            text.append("Most Common Column: ");
            text.append(mi->getMetricMostCommonValueColumnName());
            text.append("\n\n");
         }
         
         //
         // List most common exclude unidentified if applicable
         //
         if (mms.getDoMostCommonExcludeUnidentifiedOfAllCasesFlag() &&
             (mi->getMetricMostCommonExcludeUnidentifiedValueColumnNameRowNumber() >= 0)) {
            text.append("Most Common Exclude No ID Column: ");
            text.append(mi->getMetricMostCommonExcludeUnidentifiedValueColumnName());
            text.append("\n\n");
         }
         
         //
         // List Standard Deviation if applicable
         //
         if (mms.getDoStdDevOfAllCoordFileFlag() &&
             (mi->getMetricStdDevColumnNameRowNumber() >= 0)) {
            text.append("Sample Standard Deviation Column: ");
            text.append(mi->getMetricStdDevColumnName());
            text.append("\n");
         }
         
         //
         // List Standard Error if applicable
         //
         if (mms.getDoStdErrorOfAllCoordFileFlag() &&
             (mi->getMetricStdErrorColumnNameRowNumber() >= 0)) {
            text.append("Standard Error Column: ");
            text.append(mi->getMetricStdErrorColumnName());
            text.append("\n\n");
         }
         
         //
         // List Min Value if applicable
         //
         if (mms.getDoMinOfAllCoordFileFlag() &&
             (mi->getMetricMinValueColumnNameRowNumber() >= 0)) {
            text.append("Minimum Value Column: ");
            text.append(mi->getMetricMinValueColumnName());
            text.append("\n\n");
         }
         
         //
         // List Max Value if applicable
         //
         if (mms.getDoMaxOfAllCoordFileFlag() &&
             (mi->getMetricMaxValueColumnNameRowNumber() >= 0)) {
            text.append("Maximum Value Column: ");
            text.append(mi->getMetricMaxValueColumnName());
            text.append("\n\n");
         }
         
         //
         // are columns being generated
         //
         if (mms.getDoAllCasesCoordFileFlag()) {
            //
            // List metric naming information
            //
            for (int j = 0; j < mi->getNumberOfMetricColumns(); j++) {
               //
               // Column name
               //
               text.append("Data Column ");
               text.append(StringUtilities::fromNumber(j));
               text.append(" Name : \"");
               text.append(mi->getMetricColumnName(j));
               text.append("\"\n");
               
               //
               // Surface and Volume name
               //
               text.append("   using surface: ");
               text.append(mi->getSurfaceNameForMetricColumn(j));
               text.append("\n");
               text.append("   from volume: ");
               text.append(volumeName);
               text.append("\n");
               text.append("   sub-volume: ");
               text.append(subVolumeName);
               text.append("\n");
            }
         }
         
         text.append("\n");
      }
   }
 
   bool showAlgorithm = false;
   switch (dataMappingType) {
      case DATA_MAPPING_TYPE_NONE:
         break;
      case DATA_MAPPING_TYPE_METRIC:
         showAlgorithm = true;
         break;
      case DATA_MAPPING_TYPE_PAINT:
         break;
   }
   
   if (showAlgorithm) {
      text += "Algorithm: ";
      text += BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmName(mappingParameters.getAlgorithm());
   }
   text += "\n\n";
   
   summaryTextEdit->setPlainText(text);
}

/**
 * Adds help information in a read only text widget.
 */
QWidget*
GuiMapFmriDialog::addPageInformation(const QString& title,
                                     const QString& text,
                                     QTextBrowser* textBrowserIn)
{
    
   const int maxHeight = 150;
   QTextBrowser* textEditor = textBrowserIn;
   if (textEditor == NULL) {
      textEditor = new QTextBrowser;
   }
   textEditor->setReadOnly(true);
   textEditor->setHtml(text);
   
   QGroupBox* group = new QGroupBox(title);
   QVBoxLayout* layout = new QVBoxLayout(group);
   layout->addWidget(textEditor);
   int height = group->sizeHint().height();
   if (height > maxHeight) {
      height = maxHeight;
   }
   group->setMaximumHeight(height);
   return group;
}


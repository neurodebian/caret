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
#include <set>
#include <sstream>

#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QStackedWidget>

#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiVolumeRegionOfInterestDialogOld.h"
#include "GuiVolumeSelectionControl.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "VolumeFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiVolumeRegionOfInterestDialogOld::GuiVolumeRegionOfInterestDialogOld(QWidget* parent)
   : QtDialog(parent)
{
   setWindowTitle("Volume Region of Interest");
   
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   createQuerySelectionPage();
      
   createReportPage();
   
   tabWidget = new QTabWidget;
   dialogLayout->addWidget(tabWidget);   
   tabWidget->addTab(queryWidget, "Query");
   tabWidget->addTab(reportPage, "Report");

   updateDialog();

   selectionModeComboBox->setCurrentIndex(SELECTION_MODE_ALL_VOXELS);
   slotSelectionMode(selectionModeComboBox->currentIndex());
   operationsModeComboBox->setCurrentIndex(0);
   slotOperationMode(operationsModeComboBox->currentIndex());
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // The close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   closeButton->setAutoDefault(false);
   closeButton->setFixedSize(closeButton->sizeHint());
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
}

/**
 * Destructor.
 */
GuiVolumeRegionOfInterestDialogOld::~GuiVolumeRegionOfInterestDialogOld()
{
}

/**
 * update the dialog (typically due to file changes).
 */
void 
GuiVolumeRegionOfInterestDialogOld::updateDialog()
{
   selectRoiVolumeAnatomyControl->updateControl();
   selectRoiVolumeFunctionalControl->updateControl();
   selectRoiVolumeSegmentationControl->updateControl();
   segmentationVolumeCogControl->updateControl();
   paintVolumeCogControl->updateControl();
   paintVolumePercentControl->updateControl();
}

/**
 * called when dialog closed.
 */
void 
GuiVolumeRegionOfInterestDialogOld::close()
{
   QDialog::close();
}

/**
 * create the query page.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createQuerySelectionPage()
{
   //
   // Create the query page
   //
   queryWidget = new QWidget;  
   QVBoxLayout* queryLayout = new QVBoxLayout(queryWidget);
   
   //-----------------------------------------------------------------------------
   //
   // Group Box for node selection
   //
   QGroupBox* selectionModeGroupBox = new QGroupBox("Region of Interest Voxel Selection");
   queryLayout->addWidget(selectionModeGroupBox);
   QVBoxLayout* selectionModeGroupLayout = new QVBoxLayout(selectionModeGroupBox);
   
   //
   // Node selection method label and combo box
   //
   QLabel* selectionMethodLabel = new QLabel("Selection Method ");
   selectionModeComboBox = new QComboBox;
   QObject::connect(selectionModeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSelectionMode(int)));
   selectionModeComboBox->insertItem(SELECTION_MODE_ALL_VOXELS,
                                     "All Voxels");
   selectionModeComboBox->insertItem(SELECTION_MODE_VOXELS_WITHIN_SEGMENTATION_VOLUME,
                                     "Voxels Within Segmentation Volume");
   selectionModeComboBox->insertItem(SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_ANATOMY_VOLUME,
                                     "Voxels Within Thresholded Anatomy Volume");
   selectionModeComboBox->insertItem(SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_FUNCTIONAL_VOLUME,
                                     "Voxels Within Thresholded Functional Volume");
   QHBoxLayout* selectionMethodLayout = new QHBoxLayout;
   selectionMethodLayout->addWidget(selectionMethodLabel);
   selectionMethodLayout->addWidget(selectionModeComboBox);
   selectionMethodLayout->setStretchFactor(selectionMethodLabel, 0);
   selectionMethodLayout->setStretchFactor(selectionModeComboBox, 100);
   selectionModeGroupLayout->addLayout(selectionMethodLayout);
   
   //-----------------------------------------------------------------------------
   //
   // Widget stack for different query controls
   //
   queryControlWidgetStack = new QStackedWidget;
   selectionModeGroupLayout->addWidget(queryControlWidgetStack);
   
   //
   // Create the various selection widgets
   //
   createSelectionAllVoxels(queryControlWidgetStack);
   createSelectionWithinSegmentation(queryControlWidgetStack);
   createSelectionWithinAnatomyThreshold(queryControlWidgetStack);
   createSelectionWithinFunctionalThreshold(queryControlWidgetStack);
   
   //-----------------------------------------------------------------------------
   //
   // Operations mode combo box
   //
   QLabel* operationsModeLabel = new QLabel("Operation");
   operationsModeComboBox = new QComboBox;
   QObject::connect(operationsModeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotOperationMode(int)));
   operationsModeComboBox->insertItem(OPERATIONS_MODE_PAINT_CENTER_OF_GRAVITY,
                                      "Paint Region Center of Gravity");
   operationsModeComboBox->insertItem(OPERATIONS_MODE_PAINT_PERCENTAGE_REPORT,
                                      "Paint Percentage Report");
   operationsModeComboBox->insertItem(OPERATIONS_MODE_PROBABILISTIC_PAINT,
                                      "Probabilistic Paint Volume Overlap Analysis");
   operationsModeComboBox->insertItem(OPERATIONS_MODE_SEGMENTATION_CENTER_OF_GRAVITY,
                                      "Segmentation Center of Gravity Analysis");
   QHBoxLayout* operationsModeLayout = new QHBoxLayout;
   operationsModeLayout->addWidget(operationsModeLabel);
   operationsModeLayout->addWidget(operationsModeComboBox);
   operationsModeLayout->setStretchFactor(operationsModeLabel, 0);
   operationsModeLayout->setStretchFactor(operationsModeComboBox, 1000);

   //-----------------------------------------------------------------------------
   //
   // Widget stack for different operations
   //
   operationsWidgetStack = new QStackedWidget;
   
   //
   // Create the operations sections
   //
   createOperationsPaintCenterOfGravity(operationsWidgetStack);
   createOperationsPaintPercentage(operationsWidgetStack);
   createOperationsProbabilisticPaint(operationsWidgetStack);
   createOperationsSegmentationCenterOfGravity(operationsWidgetStack);
   

   //
   // Group box for operations
   //
   QGroupBox* operationsGroupBox = new QGroupBox("Operate on Selected Voxels");
   queryLayout->addWidget(operationsGroupBox);
   QVBoxLayout* operationsGroupLayout = new QVBoxLayout(operationsGroupBox);
   operationsGroupLayout->addLayout(operationsModeLayout);
   operationsGroupLayout->addWidget(operationsWidgetStack);
}

/**
 * create the voxel selection ALL section.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createSelectionAllVoxels(QStackedWidget* parent)
{
   selectAllVoxelsQVBox = new QWidget;
   parent->addWidget(selectAllVoxelsQVBox);   
}

/**
 * create the voxel selection within segmentation section.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createSelectionWithinSegmentation(QStackedWidget* parent)
{
   selectRoiVolumeSegmentationControl = 
                  new GuiVolumeSelectionControl(0,
                                                false,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true,
                                                false,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                "selectRoiVolumeSegmentationControl",
                                                false,
                                                false,
                                                false);
   selectSegmentationVoxelsQVBox = new QWidget;
   QVBoxLayout* selectSegmentationVoxelsLayout = new QVBoxLayout(selectSegmentationVoxelsQVBox);
   selectSegmentationVoxelsLayout->addWidget(selectRoiVolumeSegmentationControl);
   parent->addWidget(selectSegmentationVoxelsQVBox);
}

/**
 * create the voxel selection thresholded anatomy section.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createSelectionWithinAnatomyThreshold(QStackedWidget* parent)
{
   selectRoiVolumeAnatomyControl = 
                  new GuiVolumeSelectionControl(0,
                                                true,
                                                false,
                                                false,
                                                false,
                                                false, 
                                                false,
                                                false,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                "selectRoiVolumeAnatomyControl",
                                                false,
                                                false,
                                                false);
   QLabel* threshLabel = new QLabel("Threshold ");
   anatomicalVolumeThresholdDoubleSpinBox = new QDoubleSpinBox;
   anatomicalVolumeThresholdDoubleSpinBox->setMinimum(-256.0);
   anatomicalVolumeThresholdDoubleSpinBox->setMaximum(256.0);
   anatomicalVolumeThresholdDoubleSpinBox->setSingleStep(1.0);
   anatomicalVolumeThresholdDoubleSpinBox->setDecimals(2);
   anatomicalVolumeThresholdDoubleSpinBox->setValue(0.0);
   QHBoxLayout* threshLayout = new QHBoxLayout;
   threshLayout->addWidget(threshLabel);
   threshLayout->addWidget(threshLabel);
   threshLayout->addWidget(anatomicalVolumeThresholdDoubleSpinBox, 0);
   threshLayout->addWidget(anatomicalVolumeThresholdDoubleSpinBox, 100);
   
   //
   // layout for page
   //
   selectAnatomyVoxelsQVBox = new QWidget;
   parent->addWidget(selectAnatomyVoxelsQVBox);
   QVBoxLayout* selectAnatomyLayout = new QVBoxLayout(selectAnatomyVoxelsQVBox);
   selectAnatomyLayout->addWidget(selectRoiVolumeAnatomyControl);
   selectAnatomyLayout->addLayout(threshLayout);
}

/**
 * create the voxel selection thresholded functional section.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createSelectionWithinFunctionalThreshold(QStackedWidget* parent)
{
   selectRoiVolumeFunctionalControl = 
                  new GuiVolumeSelectionControl(0,
                                                false,
                                                true,
                                                false,
                                                false,
                                                false,
                                                false,
                                                false,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                "selectRoiVolumeFunctionalControl",
                                                false,
                                                false,
                                                false);
                                                
   QLabel* posLabel = new QLabel("Positive Threshold ");
   functionalVolumePositiveThresholdDoubleSpinBox = new QDoubleSpinBox;
   functionalVolumePositiveThresholdDoubleSpinBox->setMinimum(0.0);
   functionalVolumePositiveThresholdDoubleSpinBox->setMaximum(50000.0);
   functionalVolumePositiveThresholdDoubleSpinBox->setSingleStep(1.0);
   functionalVolumePositiveThresholdDoubleSpinBox->setDecimals(2);
   functionalVolumePositiveThresholdDoubleSpinBox->setValue(0.0);
   QLabel* negLabel = new QLabel("Negative Threshold ");
   functionalVolumeNegativeThresholdDoubleSpinBox = new QDoubleSpinBox;
   functionalVolumeNegativeThresholdDoubleSpinBox->setMinimum(0.0);
   functionalVolumeNegativeThresholdDoubleSpinBox->setMaximum(1.0);
   functionalVolumeNegativeThresholdDoubleSpinBox->setSingleStep(2);
   functionalVolumeNegativeThresholdDoubleSpinBox->setDecimals(0);
   functionalVolumeNegativeThresholdDoubleSpinBox->setValue(0.0);

   QGridLayout* threshLayout = new QGridLayout;
   threshLayout->addWidget(posLabel, 0, 0);
   threshLayout->addWidget(functionalVolumePositiveThresholdDoubleSpinBox, 0, 1);
   threshLayout->addWidget(negLabel, 1, 0);
   threshLayout->addWidget(functionalVolumeNegativeThresholdDoubleSpinBox, 1, 1);
   
   selectFunctionalVoxelsQVBox = new QWidget;
   parent->addWidget(selectFunctionalVoxelsQVBox);
   QVBoxLayout* selectFunctionalLayout = new QVBoxLayout(selectFunctionalVoxelsQVBox);
   selectFunctionalLayout->addWidget(selectRoiVolumeFunctionalControl);
   selectFunctionalLayout->addLayout(threshLayout);
}
      
/**
 * create the report page.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createReportPage()
{
   //
   // TextEdit for report contents
   //
   reportTextEdit = new QTextEdit;
   
   //
   // Buttons
   //
   QPushButton* clearPushButton = new QPushButton("Clear");
   clearPushButton->setAutoDefault(false);
   QObject::connect(clearPushButton, SIGNAL(clicked()),
                    this, SLOT(slotReportClearPushButton()));
   QPushButton* savePushButton = new QPushButton("Save To File...");
   savePushButton->setAutoDefault(false);
   QObject::connect(savePushButton, SIGNAL(clicked()),
                    this, SLOT(slotReportSavePushButton()));
   QtUtilities::makeButtonsSameSize(clearPushButton, savePushButton);
   QHBoxLayout* buttonHBoxLayout = new QHBoxLayout;
   buttonHBoxLayout->addWidget(clearPushButton);
   buttonHBoxLayout->addWidget(savePushButton);
   
   //
   // Create the report page
   //
   reportPage = new QWidget;
   QVBoxLayout* reportLayout = new QVBoxLayout(reportPage);
   reportLayout->addWidget(reportTextEdit);
   reportLayout->addLayout(buttonHBoxLayout);
}

/**
 * slot called to report page.
 */
void 
GuiVolumeRegionOfInterestDialogOld::slotReportClearPushButton()
{
   reportTextEdit->clear();
}

/**
 * slot called to report page.
 */
void 
GuiVolumeRegionOfInterestDialogOld::slotReportSavePushButton()
{
   QFileDialog fd(this);
   fd.setModal(true);
   fd.setWindowTitle("Choose ROI Text File Name");
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(QFileDialog::AcceptSave);
   fd.setFilter("Text Files (*.txt)");
   fd.setFileMode(QFileDialog::AnyFile);
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         const QString fileName = fd.selectedFiles().at(0);
         QFile file(fileName);
         if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream << reportTextEdit->toPlainText();
            file.close();
         }
         else {
            QString msg("Unable to open for writing: ");
            msg.append(fileName);
            GuiMessageBox::critical(this, "Error Opening File", msg, "OK");         
         }
      }
   }
}
      
/**
 * slot called to set the selection mode.
 */
void 
GuiVolumeRegionOfInterestDialogOld::slotSelectionMode(int item)
{
   selectionMode = static_cast<SELECTION_MODE>(item);
   switch (selectionMode) {
      case SELECTION_MODE_ALL_VOXELS:
         queryControlWidgetStack->setCurrentWidget(selectAllVoxelsQVBox);
         break;
      case SELECTION_MODE_VOXELS_WITHIN_SEGMENTATION_VOLUME:
         queryControlWidgetStack->setCurrentWidget(selectSegmentationVoxelsQVBox);
         break;
      case SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_ANATOMY_VOLUME:
         queryControlWidgetStack->setCurrentWidget(selectAnatomyVoxelsQVBox);
         break;
      case SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_FUNCTIONAL_VOLUME:
         queryControlWidgetStack->setCurrentWidget(selectFunctionalVoxelsQVBox);
         break;
   }
}

/**
 * create the operations probabilistic paint.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createOperationsProbabilisticPaint(QStackedWidget* parent)
{
   QPushButton* probPaintVoxelReportPushButton = new QPushButton("Create Report");
   probPaintVoxelReportPushButton->setFixedSize(probPaintVoxelReportPushButton->sizeHint());
   QObject::connect(probPaintVoxelReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotOperationProbabilisticPaint()));

   operationsProbabilisticPaintQVBox = new QWidget;
   parent->addWidget(operationsProbabilisticPaintQVBox);
   QVBoxLayout* operationsProbabilisticLayout = new QVBoxLayout(operationsProbabilisticPaintQVBox);
   operationsProbabilisticLayout->addWidget(probPaintVoxelReportPushButton);
}

/**
 * create the report header.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createReportHeader(const VolumeFile* vf,
                                                    const int totalNumVoxels,
                                                    const int numVoxelsInROI,
                                                    const QString& description)
{
   reportTextEdit->append(" ");
   reportTextEdit->append(description);
   reportTextEdit->append(" ");
   
   std::ostringstream str;
   
   str.str("");
   str << "Selection Mode: ";
   switch (selectionMode) {
      case SELECTION_MODE_ALL_VOXELS:
         {
            str << "All Voxels";
         }
         break;
      case SELECTION_MODE_VOXELS_WITHIN_SEGMENTATION_VOLUME:
         {
            str << "Voxels Within Segmentation Volume ";
            const VolumeFile* vf = selectRoiVolumeSegmentationControl->getSelectedVolumeFile();
            if (vf != NULL) {
               str << FileUtilities::basename(vf->getFileName()).toAscii().constData();
            }
            else {
               str << "Invalid";
            }
            str << "\n";
         }
         break;
      case SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_ANATOMY_VOLUME:
         {
            str << "Voxels Within Thresholded Anatomy Volume ";
            const VolumeFile* vf = selectRoiVolumeAnatomyControl->getSelectedVolumeFile();
            if (vf != NULL) {
               str << FileUtilities::basename(vf->getFileName()).toAscii().constData();
            }
            else {
               str << "Invalid";
            }
            str << "\n";
            
            str << "Threshold: "
                << anatomicalVolumeThresholdDoubleSpinBox->value()
                << "\n";
         }
         break;
      case SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_FUNCTIONAL_VOLUME:
         {
            str << "Voxels Within Thresholded Functional Volume ";
            const VolumeFile* vf = selectRoiVolumeFunctionalControl->getSelectedVolumeFile();
            if (vf != NULL) {
               str << FileUtilities::basename(vf->getFileName()).toAscii().constData();
            }
            else {
               str << "Invalid";
            }
            str << "\n";
            
            str << "Positive Threshold: "
                << functionalVolumePositiveThresholdDoubleSpinBox->value()
                << "\n";
            str << "Negative Threshold: "
                << functionalVolumeNegativeThresholdDoubleSpinBox->value()
                << "\n";
         }
         break;
   }
   reportTextEdit->append(str.str().c_str());
   
   if (vf != NULL) {
      str.str("");
      str << "Operating on volume: " << FileUtilities::basename(vf->getFileName()).toAscii().constData() << "\n";
      str << "              label: " << vf->getDescriptiveLabel().toAscii().constData() << "\n";
      reportTextEdit->append(str.str().c_str());
   }
   
   str.str("");
   str << numVoxelsInROI
       << " of "
       << totalNumVoxels
       << " voxels are in the ROI.";
   reportTextEdit->append(str.str().c_str());
   
   reportTextEdit->append("\n");
}

/**
 * slot called to create probabilistic paint report.
 */
void 
GuiVolumeRegionOfInterestDialogOld::slotOperationProbabilisticPaint()
{
   const int numVolumes = theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles();
   if (numVolumes <= 0) {
      GuiMessageBox::critical(this, "ERROR", "There are no paint volumes loaded.", "OK");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // Make sure all volumes are of the same dimensions
   //
   std::vector<VolumeFile*> paintVolumes;
   int dim[3];
   for (int i = 0; i < numVolumes; i++) {
      VolumeFile* vf = theMainWindow->getBrainSet()->getVolumePaintFile(i);
      if (i == 0) {
         vf->getDimensions(dim);
      }
      else {
         int dim2[3];
         vf->getDimensions(dim2);
         for (int j = 0; j < 3; j++) {
            if (dim[j] != dim2[j]) {
               VolumeFile* vf0 = theMainWindow->getBrainSet()->getVolumePaintFile(0);
               std::ostringstream str;
               str << "All volumes must be the same dimensions.\n"
                   << "Volume " << FileUtilities::basename(vf0->getFileName()).toAscii().constData() << " dimensions: "
                   << "(" << dim[0] << ", " << dim[1] << ", " << dim[2] << ")\n"
                   << "Volume " << FileUtilities::basename(vf->getFileName()).toAscii().constData() << " dimensions: "
                   << "(" << dim2[0] << ", " << dim2[1] << ", " << dim2[2] << ").\n";
               GuiMessageBox::critical(this, "ERROR", str.str().c_str(), "OK");
               return;
            }
         }
      }
      paintVolumes.push_back(vf);
   }

   //
   // Use first paint volume to determine voxels in ROI
   //
   std::vector<int> voxelInROI;
   const int numVoxelsInROI = getVoxelsInROI(paintVolumes[0], voxelInROI);
   if (numVoxelsInROI <= 0) {
      return;
   }

   //
   // Create the report header
   //
   const int numVoxels = paintVolumes[0]->getTotalNumberOfVoxels();
   createReportHeader(NULL, numVoxels, numVoxelsInROI,
                      "Probabilistic Atlas Overlap Analysis");
   
   //
   // Get all paint names
   //
   std::set<QString> paintNamesSorted;
   for (int i = 0; i < numVolumes; i++) {
      VolumeFile* vf = paintVolumes[i];
      const int numNames = vf->getNumberOfRegionNames();
      for (int j = 0; j < numNames; j++) {
         paintNamesSorted.insert(vf->getRegionNameFromIndex(j));
      }
   }
   std::vector<QString> paintNames(paintNamesSorted.begin(), paintNamesSorted.end());
   const int numPaintNames = static_cast<int>(paintNames.size());
   
   //
   // Counts for each paint index
   //
   const int numColumns = numVolumes + 1;
   const int totalCountSize = numPaintNames * numColumns;
   std::vector<int> totalCounts(totalCountSize, 0);
   
   //
   // Get conversion from each paint volume index to all paint names
   //   
   std::vector< std::vector<int> > paintIndexToAllPaintNames;
   for (int i = 0; i < numVolumes; i++) {
      VolumeFile* vf = paintVolumes[i];
      const int numNames = vf->getNumberOfRegionNames();
      std::vector<int> indexConversion;
      for (int j = 0; j < numNames; j++) {
         int indx = -1;
         const QString name(vf->getRegionNameFromIndex(j));
         for (int k = 0; k < numPaintNames; k++) {
            if (name == paintNames[k]) {
               indx = k;
            }
         }
         if (indx == -1) {
            std::cout << "PROGRAM ERROR: Invalid paint index volume paint report" << std::endl;
         }
         indexConversion.push_back(indx);
      }
      paintIndexToAllPaintNames.push_back(indexConversion);
   }
   
   //
   // Get the number of voxels
   //
   for (int i = 0; i < numVoxels; i++) {
      //
      // Is this voxel in the ROI
      //
      if (voxelInROI[i]) {
         //
         // Get the paint names used by this voxel in all volumes
         //
         std::vector<int> indicesUsedByThisVoxel;
         for (int j = 0; j < numVolumes; j++) {
            std::vector<int>& indexConversion = paintIndexToAllPaintNames[j];
            indicesUsedByThisVoxel.push_back(
               indexConversion[static_cast<int>(
                  paintVolumes[j]->getVoxelWithFlatIndex(i))]);
         }   
         std::sort(indicesUsedByThisVoxel.begin(), indicesUsedByThisVoxel.end());
          
         //
         // Get unique list of paint indices used by this voxel in all volumes
         //
         std::set<int> uniqueIndices(indicesUsedByThisVoxel.begin(),
                                     indicesUsedByThisVoxel.end());
                          
         //
         // Update counts
         //
         for (std::set<int>::iterator iter = uniqueIndices.begin();
              iter != uniqueIndices.end(); iter++) {
            const int n = *iter;
            int cnt = 0;
            for (int p = 0; p < numVolumes; p++) {
               if (n == indicesUsedByThisVoxel[p]) {
                  cnt++;
               }
            }
            totalCounts[n * numColumns + cnt]++;
         }
      }
   }
   
   const QString separator(";");
   
   //
   // Create the report for each paint name
   //
   for (int i = 0; i < numPaintNames; i++) {
      const int offset = i * numColumns;
      
      //
      // Determine number of zero entries
      //
      int numZero = numVoxelsInROI;
      for (int j = 1; j < numColumns; j++) {
         numZero -= totalCounts[offset + j];
      }
      totalCounts[offset] = numZero;
      
      //
      // Print counts
      //
      std::ostringstream str;
      str << paintNames[i].toAscii().constData() << " - number of voxels with:\n";
      for (int j = 0; j < numColumns; j++) {
         str << j << " entries" << separator.toAscii().constData();
      }
      str << "\n";
      for (int j = 0; j < numColumns; j++) {
         str << totalCounts[offset + j] << separator.toAscii().constData();
      }
      str << "\n";
      reportTextEdit->append(str.str().c_str());
      
      //
      // Print percentages
      //
      str.str("");
      str.precision(2);
      str.setf(std::ios::fixed);
      str << paintNames[i].toAscii().constData() << " - percent of voxels with:\n";
      for (int j = 0; j < numColumns; j++) {
         str << j << " entries" << separator.toAscii().constData();
      }
      str << "\n";
      for (int j = 0; j < numColumns; j++) {
         const float pct = (static_cast<float>(totalCounts[offset + j]) / 
                            static_cast<float>(numVoxelsInROI)) * 100.0;
         str << pct << separator.toAscii().constData();
      }
      str << "\n";
      reportTextEdit->append(str.str().c_str());
   }
   
   //
   // Swith to report page
   //
   tabWidget->setCurrentWidget(reportPage);
   
   QApplication::beep();
   
   QApplication::restoreOverrideCursor();
}

/**
 * create the operations segmentation center of gravity.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createOperationsSegmentationCenterOfGravity(QStackedWidget* parent)
{
   segmentationVolumeCogControl = 
                  new GuiVolumeSelectionControl(0,
                                                false,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true,
                                                false,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                "segmentationVolumeCogControl",
                                                false,
                                                false,
                                                false);

   QPushButton* segmentationCogReportPushButton = new QPushButton("Create Report");
   segmentationCogReportPushButton->setFixedSize(segmentationCogReportPushButton->sizeHint());
   QObject::connect(segmentationCogReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotOperationSegmentationCOG()));

   operationsSegmentationCenterOfGravityQVBox = new QWidget;
   parent->addWidget(operationsSegmentationCenterOfGravityQVBox);
   QVBoxLayout* operationsSegmentationLayout = new QVBoxLayout(operationsSegmentationCenterOfGravityQVBox);
   operationsSegmentationLayout->addWidget(segmentationVolumeCogControl);
   operationsSegmentationLayout->addWidget(segmentationCogReportPushButton);
}
      
/**
 * slot called to create the segmentation COG report.
 */
void 
GuiVolumeRegionOfInterestDialogOld::slotOperationSegmentationCOG()
{
   //
   // Make sure a segmentation volume is selected.
   //
   const VolumeFile* vf = segmentationVolumeCogControl->getSelectedVolumeFile();
   if (vf == NULL) {
      GuiMessageBox::critical(this, "ERROR", "No segmentation volume selected.", "OK");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // See which voxels are in the ROI
   //
   std::vector<int> voxelInROI;
   const int numVoxelsInROI = getVoxelsInROI(vf, voxelInROI);
   if (numVoxelsInROI <= 0) {
      return;
   }
   
   //
   // Create the report header
   //
   const int numVoxels = vf->getTotalNumberOfVoxels();
   createReportHeader(vf, numVoxels, numVoxelsInROI, "Segmentation Center of Gravity");
   
   //
   // Determine the COG of the voxels in the ROI
   //
   double voxelCOG[3] = { 0.0, 0.0, 0.0 };
   double coordCOG[3] = { 0.0, 0.0, 0.0 };
   double counter = 0;
   int dim[3];
   vf->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const int flatIndex = vf->getVoxelDataIndex(i, j, k);
            if (voxelInROI[flatIndex]) {
               const float voxel = vf->getVoxel(i, j, k);
               if (voxel > 0.0) {
                  voxelCOG[0] += i;
                  voxelCOG[1] += j;
                  voxelCOG[2] += k;
                  
                  float xyz[3];
                  vf->getVoxelCoordinate(i, j, k, true, xyz);
                  coordCOG[0] += xyz[0];
                  coordCOG[1] += xyz[1];
                  coordCOG[2] += xyz[2];

                  counter += 1.0;
               }
            }
         }
      }
   }
   
   if (counter > 0) {
      std::ostringstream str;
      str.precision(2);
      str.setf(std::ios::fixed);
      
      str << "Voxel IJK C.O.G.: ("
          << voxelCOG[0] / counter << ", "
          << voxelCOG[1] / counter << ", "
          << voxelCOG[2] / counter << ")";
      reportTextEdit->append(str.str().c_str());
      
      str.str("");
      str << "Voxel Coordinate C.O.G.: ("
          << coordCOG[0] / counter << ", "
          << coordCOG[1] / counter << ", "
          << coordCOG[2] / counter << ")";
      reportTextEdit->append(str.str().c_str());
   }
   else {
      reportTextEdit->append("There are no non-zero segmentation voxels in the ROI.");
   }
   
   //
   // Swith to report page
   //
   tabWidget->setCurrentWidget(reportPage);
   
   QApplication::beep();
   
   QApplication::restoreOverrideCursor();
}
      
/**
 * slot called to set the operation mode.
 */
void 
GuiVolumeRegionOfInterestDialogOld::slotOperationMode(int item)
{
   operationsMode = static_cast<OPERATIONS_MODE>(item);
   switch (operationsMode) {
      case OPERATIONS_MODE_PROBABILISTIC_PAINT:
         operationsWidgetStack->setCurrentWidget(operationsProbabilisticPaintQVBox);
         break;
      case OPERATIONS_MODE_SEGMENTATION_CENTER_OF_GRAVITY:
         operationsWidgetStack->setCurrentWidget(operationsSegmentationCenterOfGravityQVBox);
         break;
      case OPERATIONS_MODE_PAINT_CENTER_OF_GRAVITY:
         operationsWidgetStack->setCurrentWidget(operationsPaintCenterOfGravityQVBox);
         break;
      case OPERATIONS_MODE_PAINT_PERCENTAGE_REPORT:
         operationsWidgetStack->setCurrentWidget(operationsPaintPercentageQVBox);
   }
}
      
/**
 * get the valid voxels for a volume.
 */
int 
GuiVolumeRegionOfInterestDialogOld::getVoxelsInROI(const VolumeFile* operationVolume,
                                                std::vector<int>& voxelInROI) 
{
   //
   // Make sure volume is valid and has voxels
   //
   if (operationVolume == NULL) {
      GuiMessageBox::critical(this, "ERROR",
         "The selected volume is invalid.", "OK");
      return 0;
   }
   const int numVoxels = operationVolume->getTotalNumberOfVoxels();
   if (numVoxels <= 0) {
      GuiMessageBox::critical(this, "ERROR",
         "The selected volume contains no voxels.", "OK");
      return 0;
   }
   
   //
   // Resize voxel in ROI flags
   //
   voxelInROI.resize(numVoxels);
   
   VolumeFile* roiVolume = NULL;
   
   switch (selectionMode) {
      case SELECTION_MODE_ALL_VOXELS:
         std::fill(voxelInROI.begin(), voxelInROI.end(), true);
         return voxelInROI.size();
         break;
      case SELECTION_MODE_VOXELS_WITHIN_SEGMENTATION_VOLUME:
         roiVolume = selectRoiVolumeSegmentationControl->getSelectedVolumeFile();
         if (roiVolume == NULL) {
            GuiMessageBox::critical(this, "ERROR",
               "There is not a selected segmentation volume.", "OK");
            return 0;
         }
         break;
      case SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_ANATOMY_VOLUME:
         roiVolume = selectRoiVolumeAnatomyControl->getSelectedVolumeFile();
         if (roiVolume == NULL) {
            GuiMessageBox::critical(this, "ERROR",
               "There is not a selected anatomical volume.", "OK");
            return 0;
         }
         break;
      case SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_FUNCTIONAL_VOLUME:
         roiVolume = selectRoiVolumeFunctionalControl->getSelectedVolumeFile();
         if (roiVolume == NULL) {
            GuiMessageBox::critical(this, "ERROR",
               "There is not a selected functional volume.", "OK");
            return 0;
         }
         break;
   }
   
   //
   // Get dimensions of the volume
   //
   int dim[3];
   operationVolume->getDimensions(dim);
   
   //
   // Get the various thresholds
   //
   const float anatomyThreshold = anatomicalVolumeThresholdDoubleSpinBox->value();
   const float functionalPositiveThreshold = functionalVolumePositiveThresholdDoubleSpinBox->value();
   const float functionalNegativeThreshold = functionalVolumeNegativeThresholdDoubleSpinBox->value();
   
   //
   // Test each voxel
   //
   int numVoxelsInROI = 0;
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const int flatIndex = operationVolume->getVoxelDataIndex(i, j, k);
            voxelInROI[flatIndex] = false;
        
            //
            // Determine coordinate of voxel
            //
            float xyz[3];
            operationVolume->getVoxelCoordinate(i, j, k, true, xyz);
            
            //
            // See if voxel is within the ROI volume
            //
            int roiIJK[3];
            if (roiVolume->convertCoordinatesToVoxelIJK(xyz, roiIJK)) {
               const float roiVoxel = roiVolume->getVoxel(roiIJK);
               switch (selectionMode) {
                  case SELECTION_MODE_ALL_VOXELS:
                     break;
                  case SELECTION_MODE_VOXELS_WITHIN_SEGMENTATION_VOLUME:
                     voxelInROI[flatIndex] = (roiVoxel != 0.0);
                     break;
                  case SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_ANATOMY_VOLUME:
                     voxelInROI[flatIndex] = (roiVoxel > anatomyThreshold);
                     break;
                  case SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_FUNCTIONAL_VOLUME:
                     if (roiVoxel > 0.0) {
                        voxelInROI[flatIndex] = (roiVoxel > functionalPositiveThreshold);
                     }
                     else if (roiVoxel < 0.0) {
                        voxelInROI[flatIndex] = (roiVoxel < functionalNegativeThreshold);
                     }
                     break;
               }
            }
            
            if (voxelInROI[flatIndex]) {
               numVoxelsInROI++;
            }
         }
      }
   }
   
   if (numVoxelsInROI > 0) {
      return numVoxelsInROI;
   }
   GuiMessageBox::critical(this, "ERROR",
      "There are no voxels found within the ROI.", "OK");
   return 0;
}

/**
 * create the operations paint center of gravity.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createOperationsPaintPercentage(QStackedWidget* parent)
{
   paintVolumePercentControl = 
                  new GuiVolumeSelectionControl(0,
                                                false,
                                                false,
                                                true,
                                                false,
                                                false,
                                                false,
                                                false,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                "paintVolumePercentControl",
                                                false,
                                                false,
                                                false);

   QPushButton* paintPercentReportPushButton = new QPushButton("Create Report");
   paintPercentReportPushButton->setFixedSize(paintPercentReportPushButton->sizeHint());
   QObject::connect(paintPercentReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotOperationPaintPercentage()));

   operationsPaintPercentageQVBox = new QWidget;
   parent->addWidget(operationsPaintPercentageQVBox);
   QVBoxLayout* operationsPaintLayout = new QVBoxLayout(operationsPaintPercentageQVBox);
   operationsPaintLayout->addWidget(paintVolumePercentControl);
   operationsPaintLayout->addWidget(paintPercentReportPushButton);
}
      
/**
 * slot called to create the paint percentage report.
 */
void 
GuiVolumeRegionOfInterestDialogOld::slotOperationPaintPercentage()
{
   //
   // Make sure a paint volume is selected.
   //
   const VolumeFile* vf = paintVolumePercentControl->getSelectedVolumeFile();
   if (vf == NULL) {
      GuiMessageBox::critical(this, "ERROR", "No paint volume selected.", "OK");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // See which voxels are in the ROI
   //
   std::vector<int> voxelInROI;
   const int numVoxelsInROI = getVoxelsInROI(vf, voxelInROI);
   if (numVoxelsInROI <= 0) {
      return;
   }
   
   //
   // Create the report header
   //
   const int numVoxels = vf->getTotalNumberOfVoxels();
   createReportHeader(vf, numVoxels, numVoxelsInROI, "Paint Percentage Report");
   
   //
   // Get number of paint names
   //
   const int numPaintNames = vf->getNumberOfRegionNames();
   if (numPaintNames <= 0) {
      GuiMessageBox::critical(this, "ERROR", "There are no paint regions in the volume.", "OK");
      return;
   }
   double* counter  = new double[numPaintNames];
   for (int i = 0; i < numPaintNames; i++) {
      counter[i] = 0.0;
   }
   
   //
   // Determine the COG of the voxels in the ROI
   //
   int dim[3];
   vf->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const int flatIndex = vf->getVoxelDataIndex(i, j, k);
            if (voxelInROI[flatIndex]) {
               const int paintIndex = static_cast<int>(vf->getVoxel(i, j, k));
               counter[paintIndex] += 1.0;
            }
         }
      }
   }
   
   std::ostringstream str;
   str.precision(2);
   str.setf(std::ios::fixed);
   std::vector<QString> sortedInfo;
   
   for (int i = 0; i < numPaintNames; i++) {
      str.str("");
      
      const double count = counter[i];
      str << vf->getRegionNameFromIndex(i).toAscii().constData() 
          << "; "
          << static_cast<int>(count)
          << "; "
          << (count / numVoxelsInROI) * 100.0
          << "\n";
      
      sortedInfo.push_back(str.str().c_str());
   }
      
   std::sort(sortedInfo.begin(), sortedInfo.end());
      
   reportTextEdit->append("Name; Voxels; Percent\n");
   
   for (int i = 0; i < static_cast<int>(sortedInfo.size()); i++) {
      reportTextEdit->append(sortedInfo[i]);
   }
   
   delete[] counter;
   
   //
   // Swith to report page
   //
   tabWidget->setCurrentWidget(reportPage);
   
   QApplication::beep();
   
   QApplication::restoreOverrideCursor();
}      
                          
/**
 * create the operations paint center of gravity.
 */
void 
GuiVolumeRegionOfInterestDialogOld::createOperationsPaintCenterOfGravity(QStackedWidget* parent)
{
   paintVolumeCogControl = 
                  new GuiVolumeSelectionControl(0,
                                                false,
                                                false,
                                                true,
                                                false,
                                                false,
                                                false,
                                                false,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                "paintVolumeCogControl",
                                                false,
                                                false,
                                                false);

   QPushButton* paintCogReportPushButton = new QPushButton("Create Report");
   paintCogReportPushButton->setFixedSize(paintCogReportPushButton->sizeHint());
   QObject::connect(paintCogReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotOperationPaintCOG()));

   operationsPaintCenterOfGravityQVBox = new QWidget;
   parent->addWidget(operationsPaintCenterOfGravityQVBox);
   QVBoxLayout* operationsPaint = new QVBoxLayout(operationsPaintCenterOfGravityQVBox);
   operationsPaint->addWidget(paintVolumeCogControl);
   operationsPaint->addWidget(paintCogReportPushButton);
}
      
/**
 * slot called to create the paint COG report.
 */
void 
GuiVolumeRegionOfInterestDialogOld::slotOperationPaintCOG()
{
   //
   // Make sure a paint volume is selected.
   //
   const VolumeFile* vf = paintVolumeCogControl->getSelectedVolumeFile();
   if (vf == NULL) {
      GuiMessageBox::critical(this, "ERROR", "No paint volume selected.", "OK");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // See which voxels are in the ROI
   //
   std::vector<int> voxelInROI;
   const int numVoxelsInROI = getVoxelsInROI(vf, voxelInROI);
   if (numVoxelsInROI <= 0) {
      return;
   }
   
   //
   // Create the report header
   //
   const int numVoxels = vf->getTotalNumberOfVoxels();
   createReportHeader(vf, numVoxels, numVoxelsInROI, "Paint Identification Center of Gravity");
   
   //
   // Get number of paint names
   //
   const int numPaintNames = vf->getNumberOfRegionNames();
   if (numPaintNames <= 0) {
      GuiMessageBox::critical(this, "ERROR", "There are no paint regions in the volume.", "OK");
      return;
   }
   const int totalSize = numPaintNames * 3;
   double* voxelCOG = new double[totalSize];
   double* coordCOG = new double[totalSize];
   double* counter  = new double[numPaintNames];
   for (int i = 0; i < totalSize; i++) {
      voxelCOG[i] = 0.0;
      coordCOG[i] = 0.0;
      if (i < numPaintNames) {
         counter[i] = 0.0;
      }
   }
   
   //
   // Determine the COG of the voxels in the ROI
   //
   int dim[3];
   vf->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const int flatIndex = vf->getVoxelDataIndex(i, j, k);
            if (voxelInROI[flatIndex]) {
               const int paintIndex = static_cast<int>(vf->getVoxel(i, j, k));
               const int paintIndex3 = paintIndex * 3;
               voxelCOG[paintIndex3 + 0] += i;
               voxelCOG[paintIndex3 + 1] += j;
               voxelCOG[paintIndex3 + 2] += k;
               
               float xyz[3];
               vf->getVoxelCoordinate(i, j, k, true, xyz);
               coordCOG[paintIndex3 + 0] += xyz[0];
               coordCOG[paintIndex3 + 1] += xyz[1];
               coordCOG[paintIndex3 + 2] += xyz[2];

               counter[paintIndex] += 1.0;
            }
         }
      }
   }
   
   std::ostringstream str;
   str.precision(2);
   str.setf(std::ios::fixed);
   std::vector<QString> sortedInfo;
   
   for (int i = 0; i < numPaintNames; i++) {
      const int i3 = i * 3;
      str.str("");
      
      const QString name(vf->getRegionNameFromIndex(i));
      const double count = counter[i];
      str << name.toAscii().constData() << " has " << static_cast<int>(count) << " matching voxels.\n";
      if (count > 0) {         
         str << "   Voxel IJK C.O.G.: ("
             << voxelCOG[i3 + 0] / count << ", "
             << voxelCOG[i3 + 1] / count << ", "
             << voxelCOG[i3 + 2] / count << ")\n";
         
         str << "   Voxel Coordinate C.O.G.: ("
             << coordCOG[i3 + 0] / count << ", "
             << coordCOG[i3 + 1] / count << ", "
             << coordCOG[i3 + 2] / count << ")\n";
      }
      
      sortedInfo.push_back(str.str().c_str());
   }
   
   std::sort(sortedInfo.begin(), sortedInfo.end());
      
   for (int i = 0; i < static_cast<int>(sortedInfo.size()); i++) {
      reportTextEdit->append(sortedInfo[i]);
   }
   
   delete[] voxelCOG;
   delete[] coordCOG;
   delete[] counter;
   
   //
   // Swith to report page
   //
   tabWidget->setCurrentWidget(reportPage);
   
   QApplication::beep();
   
   QApplication::restoreOverrideCursor();
} 
      


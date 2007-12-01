
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

#include <limits>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCursor>
#include <QDoubleSpinBox>
#include <QFile>
#include "WuQFileDialog.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>
#include <QTextStream>

#include "BrainModelVolume.h"
#include "BrainModelVolumeRegionOfInterest.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiNameSelectionDialog.h"
#include "GuiVolumeRegionOfInterestDialog.h"
#include "GuiVolumeSelectionControl.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiVolumeRegionOfInterestDialog::GuiVolumeRegionOfInterestDialog(QWidget* parent)
   : QtDialogWizard(parent)
{
   setWindowTitle("Volume Region of Interest");
   voxelsSelectedText = " voxels selected.  \"Next\" disabled until voxels are selected.";

   //
   // Create the pages
   //
   selectionPage = createSelectionPage();
   operationsPage = createOperationPage();
   operationPaintIdAssignmentPage = createOperationPaintIdAssignmentPage();
   operationFunctionalVolumeAssignmentPage = createOperationFunctionalValueAssignmentPage();
   operationPaintRegionCOGPage = createOperationPaintRegionCOGPage();
   operationPaintPercentReportPage = createOperationPaintPercentReportPage();
   operationProbPaintOverlapPage = createOperationPageProbPaintOverlap();
   operationSegmentationCOGPage = createOperationPageSegmentationCOG();
   outputReportPage = createOutputReportPage();
   
   //
   // Add pages to dialog
   //
   addPage(selectionPage);
   addPage(operationsPage);
   addPage(operationPaintIdAssignmentPage);
   addPage(operationFunctionalVolumeAssignmentPage);
   addPage(operationPaintRegionCOGPage);
   addPage(operationPaintPercentReportPage);
   addPage(operationProbPaintOverlapPage);
   addPage(operationSegmentationCOGPage);
   addPage(outputReportPage);
   
   selectionMode = SELECTION_MODE_NONE;
   operationsMode = OPERATIONS_MODE_NONE;
   
   //
   // Connect the finish button
   //
   QObject::connect(this, SIGNAL(signalFinishButtonPressed()),
                    this, SLOT(slotFinishPushButtonPressed()));
                    
   initializeDialog();
}

/**
 * destructor.
 */
GuiVolumeRegionOfInterestDialog::~GuiVolumeRegionOfInterestDialog()
{
}

/**
 * update the dialog.
 */
void 
GuiVolumeRegionOfInterestDialog::updateDialog()
{
   segmentationVolumeCogControl->updateControl();
   paintVolumePercentControl->updateControl();
   paintVolumeCogControl->updateControl();
   paintVolumeAssignmentControl->updateControl();
   functionalVolumeAssignmentControl->updateControl();
   setNumberOfSelectedVoxelsLabel();
   
   slotUpdatePageAndButtonValidity();
}

/**
 * called when a page is about to show user should override this.
 */
void 
GuiVolumeRegionOfInterestDialog::pageAboutToShow(QWidget* pageWidget)
{
   BrainSet* bs = theMainWindow->getBrainSet();
   //BrainModelVolume* bmv = bs->getBrainModelVolume();
   
   if (pageWidget == selectionPage) {
      //selectionAllUnderlayVoxelsRadioButton->setEnabled(bmv->getUnderlayVolumeFile() != NULL);
      //selectionAnatomyVoxelsDisplayedRadioButton->setEnabled(bmv->getVolumeIsAnUnderlayOrAnOverlay(bmv->getSelectedVolumeAnatomyFile()));
      //selectionSegmentationVoxelsDisplayedRadioButton->setEnabled(bmv->getVolumeIsAnUnderlayOrAnOverlay(bmv->getSelectedVolumeSegmentationFile()));
      //selectionFunctionalVoxelsDisplayedRadioButton->setEnabled(bmv->getVolumeIsAnUnderlayOrAnOverlay(bmv->getSelectedVolumeFunctionalViewFile()));
      //selectionFunctionalVoxelsRangeRadioButton->setEnabled(bmv->getSelectedVolumeFunctionalViewFile() != NULL);
      //selectionPaintVoxelIdRadioButton->setEnabled(bmv->getSelectedVolumePaintFile() != NULL);
   }
   else if (pageWidget == operationsPage) {
      operationPaintIdAssignmentRadioButton->setEnabled(bs->getNumberOfVolumePaintFiles() > 0);
      operationFunctionalVolumeAssignmentRadioButton->setEnabled(bs->getNumberOfVolumeFunctionalFiles());
      operationPaintRegionCOGRadioButton->setEnabled(bs->getNumberOfVolumePaintFiles() > 0);
      operationPaintPercentReportRadioButton->setEnabled(bs->getNumberOfVolumePaintFiles() > 0);
      operationProbPaintOverlapRadioButton->setEnabled(bs->getNumberOfVolumePaintFiles());
      operationSegmentationCOGRadioButton->setEnabled(bs->getNumberOfVolumeSegmentationFiles() > 0);
   }
}
      
/**
 * set the pages that are valid and buttons that are valid (all invalid when this called).
 */
void 
GuiVolumeRegionOfInterestDialog::setValidPagesAndButtons(QWidget* currentPage)
{
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelVolumeRegionOfInterest* roi = bs->getVolumeRegionOfInterestController();

   setPageValid(selectionPage, true);
   
   if (currentPage == selectionPage) {
      selectionMode = SELECTION_MODE_NONE;
      if (selectionAllUnderlayVoxelsRadioButton->isChecked() &&
          selectionAllUnderlayVoxelsRadioButton->isEnabled()) {
         selectionMode = SELECTION_MODE_ALL_UNDERLAY_VOXELS;
      }
      else if (selectionAnatomyVoxelsDisplayedRadioButton->isChecked() &&
               selectionAnatomyVoxelsDisplayedRadioButton->isEnabled()) {
         selectionMode = SELECTION_MODE_ANATOMY_ALL_VOXELS_DISPLAYED;
      }
      else if (selectionFunctionalVoxelsDisplayedRadioButton->isChecked() &&
               selectionFunctionalVoxelsDisplayedRadioButton->isEnabled()) {
         selectionMode = SELECTION_MODE_FUNCTIONAL_ALL_VOXELS_DISPLAYED;
      }
      else if (selectionFunctionalVoxelsRangeRadioButton->isChecked() &&
               selectionFunctionalVoxelsRangeRadioButton->isEnabled()) {
         selectionMode = SELECTION_MODE_FUNCTIONAL_VOXEL_RANGE;
      }
      else if (selectionPaintVoxelIdRadioButton->isChecked() &&
               selectionPaintVoxelIdRadioButton->isEnabled()) {
         selectionMode = SELECTION_MODE_PAINT_VOXELS_WITH_PAINT_ID;
      }
      else if (selectionSegmentationVoxelsDisplayedRadioButton->isChecked() &&
               selectionSegmentationVoxelsDisplayedRadioButton->isEnabled()) {
         selectionMode = SELECTION_MODE_SEGMENTATION_ALL_VOXELS_DISPLAYED;
      }
      //setNextButtonEnabled(true);
   }
   else if (currentPage == operationsPage) {
      operationsMode = OPERATIONS_MODE_NONE;
      if (operationFunctionalVolumeAssignmentRadioButton->isChecked()) {
         operationsMode = OPERATIONS_MODE_FUNCTIONAL_ASSIGNMENT;
      }
      else if (operationPaintIdAssignmentRadioButton->isChecked()) {
         operationsMode = OPERATIONS_MODE_PAINT_ID_ASSIGNMENT;
      }
      else if (operationPaintRegionCOGRadioButton->isChecked()) {
         operationsMode = OPERATIONS_MODE_PAINT_REGION_CENTER_OF_GRAVITY;
      }
      else if (operationPaintPercentReportRadioButton->isChecked()) {
         operationsMode = OPERATIONS_MODE_PAINT_PERCENTAGE_REPORT;
      }
      else if (operationProbPaintOverlapRadioButton->isChecked()) {
         operationsMode = OPERATIONS_MODE_PROBABILISTIC_PAINT;
      }
      else if (operationSegmentationCOGRadioButton->isChecked()) {
         operationsMode = OPERATIONS_MODE_SEGMENTATION_CENTER_OF_GRAVITY;
      }
      
      //setFinishButtonEnabled(true);
      //setNextButtonEnabled(true);
   }
   else {
      //setNextButtonEnabled(true);
   }

   switch (selectionMode) {
      case SELECTION_MODE_NONE:
         break;
      case SELECTION_MODE_ALL_UNDERLAY_VOXELS:
      case SELECTION_MODE_ANATOMY_ALL_VOXELS_DISPLAYED:
      case SELECTION_MODE_FUNCTIONAL_ALL_VOXELS_DISPLAYED:
      case SELECTION_MODE_FUNCTIONAL_VOXEL_RANGE:
      case SELECTION_MODE_PAINT_VOXELS_WITH_PAINT_ID:
      case SELECTION_MODE_SEGMENTATION_ALL_VOXELS_DISPLAYED:
         if (roi->getNumberOfVoxelsInROI() > 0) {
            setPageValid(operationsPage, true);
         }
         break;
   }

   if (getPageValid(operationsPage)) {
      switch (operationsMode) {
         case OPERATIONS_MODE_NONE:
            break;
         case OPERATIONS_MODE_FUNCTIONAL_ASSIGNMENT:
            setPageValid(operationFunctionalVolumeAssignmentPage, 
                         operationFunctionalVolumeAssignmentRadioButton->isEnabled());
            break;
         case OPERATIONS_MODE_PAINT_ID_ASSIGNMENT:
            setPageValid(operationPaintIdAssignmentPage, 
                         operationPaintIdAssignmentRadioButton->isEnabled());
            break;
         case OPERATIONS_MODE_PAINT_REGION_CENTER_OF_GRAVITY:
            setPageValid(operationPaintRegionCOGPage, 
                         operationPaintRegionCOGRadioButton->isEnabled());
            break;
         case OPERATIONS_MODE_PAINT_PERCENTAGE_REPORT:
            setPageValid(operationPaintPercentReportPage, 
                         operationPaintPercentReportRadioButton->isEnabled());
            break;
         case OPERATIONS_MODE_PROBABILISTIC_PAINT:
            setPageValid(operationProbPaintOverlapPage, 
                         operationProbPaintOverlapRadioButton->isEnabled());
            break;
         case OPERATIONS_MODE_SEGMENTATION_CENTER_OF_GRAVITY:
            setPageValid(operationSegmentationCOGPage, 
                         operationSegmentationCOGRadioButton->isEnabled());
            break;
      }
   }
   
   //
   // Is functional volume assigment ready?
   //
   if (currentPage == operationFunctionalVolumeAssignmentPage) {
      if (getPageValid(operationFunctionalVolumeAssignmentPage)) {
         if (functionalVolumeAssignmentControl->getSelectedVolumeFile() != NULL) {
            setFinishButtonEnabled(true);
         }
      }
   }
   
   //
   // Is paint ID assigment ready
   //
   if (currentPage == operationPaintIdAssignmentPage) {
      if (getPageValid(operationPaintIdAssignmentPage)) {
         if ((paintVolumeAssignmentControl->getSelectedVolumeFile() != NULL) &&
             (paintVolumeAssignmentLineEdit->text().isEmpty() == false)) {
            setFinishButtonEnabled(true);
         }
      }
   }
   
   //
   // Is paint region center of gravity ready
   //
   if (currentPage == operationPaintRegionCOGPage) {
      if (getPageValid(operationPaintRegionCOGPage)) {
         if (paintVolumeCogControl->getSelectedVolumeFile() != NULL) {
            setFinishButtonEnabled(true);
         }
      }
   }
   
   //
   // Is paint percentage report ready
   //
   if (currentPage == operationPaintPercentReportPage) {
      if (getPageValid(operationPaintPercentReportPage)) {
         if (paintVolumePercentControl->getSelectedVolumeFile() != NULL) {
            setFinishButtonEnabled(true);
         }
      }
   }
   
   //
   // Is segmentation cog ready
   //
   if (currentPage == operationSegmentationCOGPage) {
      if (getPageValid(operationSegmentationCOGPage)) {
         if (segmentationVolumeCogControl->getSelectedVolumeFile() != NULL) {
            setFinishButtonEnabled(true);
         }
      }
   }
   
   //
   // Is paint prob overlap ready
   //
   if (currentPage == operationProbPaintOverlapPage) {
      if (getPageValid(operationProbPaintOverlapPage)) {
         setFinishButtonEnabled(true);
      }
   }
}

/**
 * create the selection page.
 */
QWidget* 
GuiVolumeRegionOfInterestDialog::createSelectionPage()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelVolumeRegionOfInterest* roi = bs->getVolumeRegionOfInterestController();

   const int doubleSpinBoxWidth = 140;
   
   //
   // all underlay voxels radio button
   //
   selectionAllUnderlayVoxelsRadioButton = new QRadioButton("All Underlay Voxels");
   selectionAllUnderlayVoxelsRadioButton->setToolTip("Use to select all voxels\n"
                                                     "in the underlay volume.");
   
   //
   // dispalyed anatomy voxels radio button
   //
   selectionAnatomyVoxelsDisplayedRadioButton = new QRadioButton("Anatomy Voxels");
   selectionAnatomyVoxelsDisplayedRadioButton->setToolTip("Use to select all anatomy voxels\n"
                                                          "that are or would be displayed.");

   //
   // displayed segmentation voxels radio button
   //
   selectionSegmentationVoxelsDisplayedRadioButton = new QRadioButton("Segmentation Voxels");
   selectionSegmentationVoxelsDisplayedRadioButton->setToolTip("Use to select all segmentation voxels\n"
                                                               "that are or would be displayed.");

   //
   // displayed functional voxels radio button
   //
   selectionFunctionalVoxelsDisplayedRadioButton = new QRadioButton("Functional Voxels");
   selectionFunctionalVoxelsDisplayedRadioButton->setToolTip("Use to select all functional voxels\n"
                                                             "that are or would be displayed.");

   //
   // functional voxel range radio button
   //
   selectionFunctionalVoxelsRangeRadioButton = new QRadioButton("Functional Voxels Range");
   selectionFunctionalVoxelsRangeRadioButton->setToolTip("Use to select voxels in the selected functional\n"
                                                         "volume that are within a specified range.");
   functionalRangeMinimumDoubleSpinBox = new QDoubleSpinBox;
   functionalRangeMinimumDoubleSpinBox->setFixedWidth(doubleSpinBoxWidth);
   functionalRangeMinimumDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   functionalRangeMinimumDoubleSpinBox->setMaximum( std::numeric_limits<double>::max());
   functionalRangeMinimumDoubleSpinBox->setSingleStep(1.0);
   functionalRangeMinimumDoubleSpinBox->setDecimals(3);
   functionalRangeMinimumDoubleSpinBox->setToolTip("Enter range minimum here.");
   functionalRangeMaximumDoubleSpinBox = new QDoubleSpinBox;
   functionalRangeMaximumDoubleSpinBox->setFixedWidth(doubleSpinBoxWidth);
   functionalRangeMaximumDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   functionalRangeMaximumDoubleSpinBox->setMaximum( std::numeric_limits<double>::max());
   functionalRangeMaximumDoubleSpinBox->setSingleStep(1.0);
   functionalRangeMaximumDoubleSpinBox->setDecimals(3);
   functionalRangeMaximumDoubleSpinBox->setToolTip("Enter range maximum here.");
   QHBoxLayout* funcRangeLayout = new QHBoxLayout;
   funcRangeLayout->addWidget(selectionFunctionalVoxelsRangeRadioButton);
   funcRangeLayout->addWidget(functionalRangeMinimumDoubleSpinBox);
   funcRangeLayout->addWidget(functionalRangeMaximumDoubleSpinBox);
   funcRangeLayout->addStretch();
   
   //
   // paint voxel ID radio button
   //
   selectionPaintVoxelIdRadioButton = new QRadioButton("Paint Voxels with ID");
   selectionPaintVoxelIdRadioButton->setToolTip("Use to select paint voxels\n"
                                                "with a specific paint ID.");
   selectionPaintVoxelIdPushButton = new QPushButton("Name...");
   selectionPaintVoxelIdPushButton->setAutoDefault(false);
   selectionPaintVoxelIdPushButton->setFixedSize(selectionPaintVoxelIdPushButton->sizeHint());
   QObject::connect(selectionPaintVoxelIdPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectionPaintVoxelIdPushButton()));
   selectionPaintNameLabel = new QLabel("                         ");
   selectionPaintNameLabel->setMinimumWidth(250);
   QHBoxLayout* paintLayout = new QHBoxLayout;
   paintLayout->addWidget(selectionPaintVoxelIdRadioButton);
   paintLayout->addWidget(selectionPaintVoxelIdPushButton);
   paintLayout->addWidget(selectionPaintNameLabel);
   paintLayout->addStretch();
   
   //
   // button group to keep radio buttons mutually exclusive
   //
   QButtonGroup* buttGroup = new QButtonGroup(this);
   buttGroup->addButton(selectionAllUnderlayVoxelsRadioButton);
   buttGroup->addButton(selectionAnatomyVoxelsDisplayedRadioButton);
   buttGroup->addButton(selectionSegmentationVoxelsDisplayedRadioButton);
   buttGroup->addButton(selectionFunctionalVoxelsDisplayedRadioButton);
   buttGroup->addButton(selectionFunctionalVoxelsRangeRadioButton);
   buttGroup->addButton(selectionPaintVoxelIdRadioButton);
   QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotClearSelectedVoxels()));
   QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotUpdatePageAndButtonValidity()));
          
   //
   // Select voxels radio button
   //
   QPushButton* selectVoxelsPushButton = new QPushButton("Select Voxels");
   selectVoxelsPushButton->setAutoDefault(false);
   selectVoxelsPushButton->setFixedSize(selectVoxelsPushButton->sizeHint());
   QObject::connect(selectVoxelsPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectVoxelsPushButton()));
   
   //
   // Number of selected voxels label
   //
   numberOfSelectedVoxelsLabel = new QLabel("0" + voxelsSelectedText);
   
   // 
   // Show selected voxels check box
   //
   QCheckBox* showSelectedVoxelsCheckBox = new QCheckBox("Show Selected Voxels");
   showSelectedVoxelsCheckBox->setChecked(true);
   QObject::connect(showSelectedVoxelsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowSelectedVoxelsCheckBox(bool)));
   roi->setDisplayVolumeROI(showSelectedVoxelsCheckBox->isChecked());
   
   //
   // Widget and layout for page
   //
   QGroupBox* w = new QGroupBox("ROI Selection");
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addWidget(selectionAllUnderlayVoxelsRadioButton, 0, Qt::AlignLeft);
   l->addWidget(selectionAnatomyVoxelsDisplayedRadioButton, 0, Qt::AlignLeft);
   l->addWidget(selectionFunctionalVoxelsDisplayedRadioButton, 0, Qt::AlignLeft);
   l->addLayout(funcRangeLayout);
   l->addLayout(paintLayout);
   l->addWidget(selectionSegmentationVoxelsDisplayedRadioButton, 0, Qt::AlignLeft);
   l->addWidget(new QLabel(" "));
   l->addWidget(selectVoxelsPushButton);
   l->addWidget(showSelectedVoxelsCheckBox);
   l->addWidget(numberOfSelectedVoxelsLabel);
   l->addStretch();
   
   return w;
}

/**
 * called to select a paint id for voxel selection.
 */
void 
GuiVolumeRegionOfInterestDialog::slotSelectionPaintVoxelIdPushButton()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelVolume* bmv = bs->getBrainModelVolume();
   VolumeFile* paintVolume = bmv->getSelectedVolumePaintFile();
   if (paintVolume != NULL) {
      GuiNameSelectionDialog gnsd(this,
                                  GuiNameSelectionDialog::LIST_VOLUME_PAINT_NAMES_ALPHA,
                                  GuiNameSelectionDialog::LIST_VOLUME_PAINT_NAMES_ALPHA);
      if (gnsd.exec() == GuiNameSelectionDialog::Accepted) {
         slotClearSelectedVoxels();
         selectionPaintNameLabel->setText(gnsd.getName());
      }
   }
   else {
      QMessageBox::critical(this, "ERROR", "No paint volume is loaded.");
   }
}
       
/**
 * called when select voxels push button pressed.
 */
void 
GuiVolumeRegionOfInterestDialog::slotSelectVoxelsPushButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelVolume* bmv = bs->getBrainModelVolume();
   BrainModelVolumeRegionOfInterest* roi = bs->getVolumeRegionOfInterestController();
   
   switch (selectionMode) {
      case SELECTION_MODE_NONE:
         break;
      case SELECTION_MODE_ALL_UNDERLAY_VOXELS:
         roi->setVolumeROIToAllVoxels(bmv->getUnderlayVolumeFile());
         break;
      case SELECTION_MODE_ANATOMY_ALL_VOXELS_DISPLAYED:
         roi->setVolumeROIToAllDisplayedVoxels(bmv->getSelectedVolumeAnatomyFile());
         break;
      case SELECTION_MODE_FUNCTIONAL_ALL_VOXELS_DISPLAYED:
         roi->setVolumeROIToAllDisplayedVoxels(bmv->getSelectedVolumeFunctionalViewFile());
         break;
      case SELECTION_MODE_FUNCTIONAL_VOXEL_RANGE:
         roi->setVolumeROIToVoxelsInRangeOfValues(bmv->getSelectedVolumeFunctionalViewFile(),
                                            functionalRangeMinimumDoubleSpinBox->value(),
                                            functionalRangeMaximumDoubleSpinBox->value());
         break;
      case SELECTION_MODE_PAINT_VOXELS_WITH_PAINT_ID:
         roi->setVolumeROIToPaintIDVoxels(bmv->getSelectedVolumePaintFile(),
                                          selectionPaintNameLabel->text());
         break;
      case SELECTION_MODE_SEGMENTATION_ALL_VOXELS_DISPLAYED:
         roi->setVolumeROIToAllDisplayedVoxels(bmv->getSelectedVolumeSegmentationFile());
         break;
   }
      
   setNumberOfSelectedVoxelsLabel();
   slotUpdatePageAndButtonValidity();

   GuiBrainModelOpenGL::updateAllGL();
   
   QApplication::restoreOverrideCursor();
}

/**
 * called to clear the selected voxels.
 */
void 
GuiVolumeRegionOfInterestDialog::slotClearSelectedVoxels()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelVolumeRegionOfInterest* roi = bs->getVolumeRegionOfInterestController();
   roi->setVolumeROIToAllVoxels(NULL);
   setNumberOfSelectedVoxelsLabel();
   slotUpdatePageAndButtonValidity();
   GuiBrainModelOpenGL::updateAllGL();
}
      
/**
 * called to set the number of selected voxels label.
 */
void 
GuiVolumeRegionOfInterestDialog::setNumberOfSelectedVoxelsLabel()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelVolumeRegionOfInterest* roi = bs->getVolumeRegionOfInterestController();
   const QString labelText =
      QString::number(roi->getNumberOfVoxelsInROI())
      + voxelsSelectedText;
   numberOfSelectedVoxelsLabel->setText(labelText);
}      

/**
 * called when show selected voxels check box.
 */
void 
GuiVolumeRegionOfInterestDialog::slotShowSelectedVoxelsCheckBox(bool val)
{
   BrainModelVolumeRegionOfInterest* roi = theMainWindow->getBrainSet()->getVolumeRegionOfInterestController();
   roi->setDisplayVolumeROI(val);

   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * create the operation page.
 */
QWidget* 
GuiVolumeRegionOfInterestDialog::createOperationPage()
{
   operationPaintIdAssignmentRadioButton = new QRadioButton("Paint ID Assigment");
   operationPaintIdAssignmentRadioButton->setToolTip("");
   
   operationFunctionalVolumeAssignmentRadioButton = new QRadioButton("Functional Volume Assignment");
   operationFunctionalVolumeAssignmentRadioButton->setToolTip("");
   
   operationPaintRegionCOGRadioButton = new QRadioButton("Paint Region Center of Gravity Analysis");
   operationPaintRegionCOGRadioButton->setToolTip("");
   
   operationPaintPercentReportRadioButton = new QRadioButton("Paint Percent Report");
   operationPaintPercentReportRadioButton->setToolTip("");
   
   operationProbPaintOverlapRadioButton = new QRadioButton("Probabilistic Paint Volume Overlap Analysis");
   operationProbPaintOverlapRadioButton->setToolTip("");
   
   operationSegmentationCOGRadioButton = new QRadioButton("Segmentation Center of Gravity Analysis");
   operationSegmentationCOGRadioButton->setToolTip("");
   
   //
   // button group to keep radio buttons mutually exclusive
   //
   QButtonGroup* buttGroup = new QButtonGroup(this);
   buttGroup->addButton(operationPaintIdAssignmentRadioButton);
   buttGroup->addButton(operationFunctionalVolumeAssignmentRadioButton);
   buttGroup->addButton(operationPaintRegionCOGRadioButton);
   buttGroup->addButton(operationPaintPercentReportRadioButton);
   buttGroup->addButton(operationProbPaintOverlapRadioButton);
   buttGroup->addButton(operationSegmentationCOGRadioButton);
   QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotUpdatePageAndButtonValidity()));

   //
   // widget and layout for the page
   //                    
   QGroupBox* w = new QGroupBox("ROI Operation");
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addWidget(operationFunctionalVolumeAssignmentRadioButton);
   l->addWidget(operationPaintIdAssignmentRadioButton);
   l->addWidget(operationPaintPercentReportRadioButton);
   l->addWidget(operationPaintRegionCOGRadioButton);
   l->addWidget(operationProbPaintOverlapRadioButton);
   l->addWidget(operationSegmentationCOGRadioButton);
   l->addStretch();
   return w;
}

/**
 * create the operation paint ID assignment page.
 */
QWidget* 
GuiVolumeRegionOfInterestDialog::createOperationPaintIdAssignmentPage()
{
   paintVolumeAssignmentControl = 
                  new GuiVolumeSelectionControl(0,
                                                false,
                                                false,
                                                true,
                                                false,
                                                false,
                                                false,
                                                false,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                "paintVolumeAssignmentControl",
                                                false,
                                                false,
                                                false);

   QPushButton* paintAssignmentNamePushButton = new QPushButton("Paint Name...");
   paintAssignmentNamePushButton->setFixedSize(paintAssignmentNamePushButton->sizeHint()),
   paintAssignmentNamePushButton->setAutoDefault(false);
   QObject::connect(paintAssignmentNamePushButton, SIGNAL(clicked()),
                    this, SLOT(slotPaintAssignmentNamePushButton()));
   paintVolumeAssignmentLineEdit = new QLineEdit;
   QObject::connect(paintVolumeAssignmentLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotUpdatePageAndButtonValidity()));
   QGridLayout* volLayout = new QGridLayout;
   volLayout->addWidget(paintVolumeAssignmentControl, 0, 0, 1, 2);
   volLayout->addWidget(paintAssignmentNamePushButton, 1, 0, 1, 1);
   volLayout->addWidget(paintVolumeAssignmentLineEdit, 1, 1, 1, 1);
   volLayout->setColumnStretch(0, 0);
   volLayout->setColumnStretch(1, 100);
   
   QGroupBox* w = new QGroupBox("Paint Volume Assignment");
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addLayout(volLayout);
   l->addStretch();
   return w;
}

/**
 * called to choose paint assignment name.
 */
void 
GuiVolumeRegionOfInterestDialog::slotPaintAssignmentNamePushButton()
{
   GuiNameSelectionDialog gnsd(this,
                               GuiNameSelectionDialog::LIST_ALL,
                               GuiNameSelectionDialog::LIST_VOLUME_PAINT_NAMES_ALPHA);
   if (gnsd.exec() == GuiNameSelectionDialog::Accepted) {
      paintVolumeAssignmentLineEdit->setText(gnsd.getName());
   }
}
      
/**
 * create the operation functional value assignment page.
 */
QWidget* 
GuiVolumeRegionOfInterestDialog::createOperationFunctionalValueAssignmentPage()
{
   functionalVolumeAssignmentControl = 
                  new GuiVolumeSelectionControl(0,
                                                false,
                                                true,
                                                false,
                                                false,
                                                false,
                                                false,
                                                false,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                "functionalVolumeAssignmentControl",
                                                false,
                                                false,
                                                false);

   QLabel* funcLabel = new QLabel("Functional Value ");
   functionalVolumeAssignmentValueDoubleSpinBox = new QDoubleSpinBox;
   functionalVolumeAssignmentValueDoubleSpinBox->setMinimum(-std::numeric_limits<float>::max());
   functionalVolumeAssignmentValueDoubleSpinBox->setMaximum(std::numeric_limits<float>::max());
   functionalVolumeAssignmentValueDoubleSpinBox->setDecimals(3);
   functionalVolumeAssignmentValueDoubleSpinBox->setSingleStep(1.0);
   
   QGridLayout* volLayout = new QGridLayout;
   volLayout->addWidget(functionalVolumeAssignmentControl, 0, 0, 1, 2);
   volLayout->addWidget(funcLabel, 1, 0, 1, 1);
   volLayout->addWidget(functionalVolumeAssignmentValueDoubleSpinBox, 1, 1, 1, 1);
   volLayout->setColumnStretch(0, 0);
   volLayout->setColumnStretch(1, 100);

   QGroupBox* w = new QGroupBox("Functional Volume Assignment");
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addLayout(volLayout);
   l->addStretch();
   return w;
}

/**
 * create the operation paint region COG page.
 */
QWidget* 
GuiVolumeRegionOfInterestDialog::createOperationPaintRegionCOGPage()
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

   QHBoxLayout* volLayout = new QHBoxLayout;
   volLayout->addWidget(paintVolumeCogControl, 1000);

   QGroupBox* w = new QGroupBox("Paint Region Center Of Gravity");
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addLayout(volLayout);
   l->addStretch();
   return w;
}

/**
 * create the operation paint percent report page.
 */
QWidget* 
GuiVolumeRegionOfInterestDialog::createOperationPaintPercentReportPage()
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

   QHBoxLayout* volLayout = new QHBoxLayout;
   volLayout->addWidget(paintVolumePercentControl, 1000);

   QGroupBox* w = new QGroupBox("Paint Percentage Report");
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addLayout(volLayout);
   l->addStretch();
   return w;
}

/**
 * create the operation prob paint overlap page.
 */
QWidget* 
GuiVolumeRegionOfInterestDialog::createOperationPageProbPaintOverlap()
{
   QGroupBox* w = new QGroupBox("Probabilistic Paint Volume Overlap Analysis");
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addWidget(new QLabel("Press Finish button to generate report."));
   l->addStretch();
   return w;
}

/**
 * create the operation  page.
 */
QWidget* 
GuiVolumeRegionOfInterestDialog::createOperationPageSegmentationCOG()
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

   QHBoxLayout* volLayout = new QHBoxLayout;
   volLayout->addWidget(segmentationVolumeCogControl, 1000);

   QGroupBox* w = new QGroupBox("Segmentation Center of Gravity");
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addLayout(volLayout);
   l->addStretch();
   return w;
}
      
/**
 * slot called to clear report page.
 */
void 
GuiVolumeRegionOfInterestDialog::slotReportClearPushButton()
{
   reportTextEdit->clear();
}

/**
 * slot called to save report page to a file.
 */
void 
GuiVolumeRegionOfInterestDialog::slotReportSavePushButton()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setWindowTitle("Choose ROI Text File Name");
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(WuQFileDialog::AcceptSave);
   fd.setFilter("Text Files (*.txt)");
   fd.setFileMode(WuQFileDialog::AnyFile);
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
            QMessageBox::critical(this, "Error Opening File", msg);         
         }
      }
   }
}
      
/**
 * create the output report page.
 */
QWidget* 
GuiVolumeRegionOfInterestDialog::createOutputReportPage()
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
   
   QGroupBox* w = new QGroupBox("Output Report");
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addWidget(reportTextEdit, 100);
   l->addLayout(buttonHBoxLayout, 0);
   return w;
}      

/**
 * called when the finished button is pressed.
 */
void 
GuiVolumeRegionOfInterestDialog::slotFinishPushButtonPressed()
{
   try {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      QString reportText;
      
      BrainSet* bs = theMainWindow->getBrainSet();
      BrainModelVolumeRegionOfInterest* roi = bs->getVolumeRegionOfInterestController();
      switch (operationsMode) {
         case OPERATIONS_MODE_NONE:
            break;
         case OPERATIONS_MODE_FUNCTIONAL_ASSIGNMENT:
            roi->operationAssignFunctionalVolumeValue(functionalVolumeAssignmentControl->getSelectedVolumeFile(),
                                                      functionalVolumeAssignmentValueDoubleSpinBox->value());
            break;
         case OPERATIONS_MODE_PAINT_ID_ASSIGNMENT:
            roi->operationAssignPaintVolumeID(paintVolumeAssignmentControl->getSelectedVolumeFile(),
                                             paintVolumeAssignmentLineEdit->text());
            break;
         case OPERATIONS_MODE_PAINT_REGION_CENTER_OF_GRAVITY:
            roi->operationPaintRegionCenterOfGravity(paintVolumeCogControl->getSelectedVolumeFile(),
                                                     reportText);
            break;
         case OPERATIONS_MODE_PAINT_PERCENTAGE_REPORT:
            roi->operationPaintPercentageReport(paintVolumePercentControl->getSelectedVolumeFile(),
                                                reportText);
            break;
         case OPERATIONS_MODE_PROBABILISTIC_PAINT:
            {
               std::vector<VolumeFile*> paintVolumes;
               bs->getVolumePaintFiles(paintVolumes);
               roi->operationPaintOverlapAnalysis(paintVolumes,
                                                  reportText);
            }
            break;
         case OPERATIONS_MODE_SEGMENTATION_CENTER_OF_GRAVITY:
            roi->operationSegmentationRegionCenterOfGravity(segmentationVolumeCogControl->getSelectedVolumeFile(),
                                                            reportText);
            break;
      }
      
      if (reportText.isEmpty() == false) {
         const QString newText = reportTextEdit->toPlainText()
                               + "\n"
                               + reportText;
         reportTextEdit->setPlainText(newText);
         setPageValid(outputReportPage, true);
         slotNextPushButton();
         
      }
      
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::restoreOverrideCursor();
   }
   catch (BrainModelAlgorithmException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "ERROR", e.whatQString());
      return;
   }
}


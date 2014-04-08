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

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QToolButton>

#include "ArealEstimationFile.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfaceOverlay.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiDisplayControlDialog.h"
#include "GuiDisplayControlSurfaceOverlayWidget.h"
#include "GuiMainWindow.h"
#include "GuiDataFileCommentDialog.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "RgbPaintFile.h"
#include "SurfaceShapeFile.h"
#include "TopographyFile.h"
#include "WuQWidgetGroup.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiDisplayControlSurfaceOverlayWidget::GuiDisplayControlSurfaceOverlayWidget(
                                                     const int overlayNumberIn,
                                 GuiDisplayControlDialog* displayControlDialogIn)
   : QGroupBox(0)
{
   //
   // Set member variables
   //
   overlayNumber = overlayNumberIn;
   displayControlDialog = displayControlDialogIn;

   //
   // Set title
   //
   bool underlayFlag = false;
   const QString groupBoxTitle = theMainWindow->getBrainSet()->getSurfaceOverlay(overlayNumber)->getName();
   if (overlayNumber == 0) {
      underlayFlag  = true;
   }
   this->setTitle(groupBoxTitle);

   //
   // Data type selection control
   //
   QLabel* dataTypeLabel = new QLabel("Data Type");
   dataTypeComboBox = new QComboBox;
   QObject::connect(dataTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotDataTypeComboBox(int)));
   dataTypeWidgetGroup = new WuQWidgetGroup(this);
   dataTypeWidgetGroup->addWidget(dataTypeLabel);
   dataTypeWidgetGroup->addWidget(dataTypeComboBox);
   
   //
   // Display Column
   //
   QLabel* displayColumnLabel = new QLabel("Display Column");
   QToolButton* displayColumnMetaDataToolButton = new QToolButton;
   displayColumnMetaDataToolButton->setText("?");
   QObject::connect(displayColumnMetaDataToolButton, SIGNAL(clicked()),
                    this, SLOT(slotDisplayColumnMetaDataToolButton()));
   displayColumnComboBox = new QComboBox;
   QObject::connect(displayColumnComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotDisplayColumnComboBox(int)));
   displayColumnWidgetGroup = new WuQWidgetGroup(this);
   displayColumnWidgetGroup->addWidget(displayColumnLabel);
   displayColumnWidgetGroup->addWidget(displayColumnMetaDataToolButton);
   displayColumnWidgetGroup->addWidget(displayColumnComboBox);
   
   //
   // Threshold Column
   //
   QLabel* thresholdColumnLabel = new QLabel("Threshold Column");
   QToolButton* thresholdColumnMetaDataToolButton = new QToolButton;
   thresholdColumnMetaDataToolButton->setText("?");
   QObject::connect(thresholdColumnMetaDataToolButton, SIGNAL(clicked()),
                    this, SLOT(slotThresholdColumnMetaDataToolButton()));
   thresholdColumnComboBox = new QComboBox;
   QObject::connect(thresholdColumnComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotThresholdColumnComboBox(int)));
   thresholdColumnWidgetGroup = new WuQWidgetGroup(this);
   thresholdColumnWidgetGroup->addWidget(thresholdColumnLabel);
   thresholdColumnWidgetGroup->addWidget(thresholdColumnMetaDataToolButton);
   thresholdColumnWidgetGroup->addWidget(thresholdColumnComboBox);
   
   //
   // lighting
   //
   lightingCheckBox = new QCheckBox("Lighting");
   QObject::connect(lightingCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotLightingCheckBox(bool)));
   
   //
   // Opacity
   //
   QLabel* opacityLabel = new QLabel("Opacity ");
   opacityDoubleSpinBox = new QDoubleSpinBox;
   opacityDoubleSpinBox->setMinimum(0.0);
   opacityDoubleSpinBox->setMaximum(1.0);
   opacityDoubleSpinBox->setDecimals(2);
   opacityDoubleSpinBox->setSingleStep(0.1);
   QObject::connect(opacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotOpacityDoubleSpinBox(double)));
   QWidget* opacityWidget = new QWidget;
   QHBoxLayout* opacityLayout = new QHBoxLayout(opacityWidget);
   opacityLayout->addWidget(opacityLabel);
   opacityLayout->addWidget(opacityDoubleSpinBox);
   opacityLayout->addStretch();
   opacityLayout->setSpacing(0);
   if (underlayFlag) {
      opacityWidget->setHidden(true);
   }
   
   //
   // Layout
   //
   QGridLayout* gridLayout = new QGridLayout(this);
   gridLayout->addWidget(dataTypeLabel, 0, 0);
   gridLayout->addWidget(dataTypeComboBox, 0, 2);
   gridLayout->addWidget(displayColumnLabel, 1, 0);
   gridLayout->addWidget(displayColumnMetaDataToolButton, 1, 1);
   gridLayout->addWidget(displayColumnComboBox, 1, 2);
   gridLayout->addWidget(thresholdColumnLabel, 2, 0);
   gridLayout->addWidget(thresholdColumnMetaDataToolButton, 2, 1);
   gridLayout->addWidget(thresholdColumnComboBox, 2, 2);
   gridLayout->addWidget(lightingCheckBox, 3, 0);
   gridLayout->addWidget(opacityWidget, 3, 2);
   gridLayout->setColumnStretch(0, 0);
   gridLayout->setColumnStretch(1, 0);
   gridLayout->setColumnStretch(2, 1000);
   gridLayout->setVerticalSpacing(0);
   
   updateWidget();
}

/**
 * destructor.
 */
GuiDisplayControlSurfaceOverlayWidget::~GuiDisplayControlSurfaceOverlayWidget()
{
}

/**
 * update the widget.
 */
void
GuiDisplayControlSurfaceOverlayWidget::updateWidget()
{
   //
   // Get the surface overlay and update it
   //
   const BrainModelSurfaceOverlay* bmsOverlay =
      theMainWindow->getBrainSet()->getSurfaceOverlay(overlayNumber);
   
   //
   // Load the data type combo box
   //
   dataTypeWidgetGroup->blockSignals(true);
   std::vector<BrainModelSurfaceOverlay::OVERLAY_SELECTIONS> dataTypes;
   std::vector<QString> dataTypeNames;
   bmsOverlay->getDataTypesAndNames(dataTypes, dataTypeNames);
   const BrainModelSurfaceOverlay::OVERLAY_SELECTIONS selectedOverlay =
      bmsOverlay->getOverlay(getSurfaceModelIndexFromDisplayControlDialog());
   const int numDataTypes = static_cast<int>(dataTypes.size());
   dataTypeComboBox->clear();
   int selectedIndex = -1;
   for (int i = 0; i < numDataTypes; i++) {
      dataTypeComboBox->addItem(dataTypeNames[i],
                                QVariant(static_cast<int>(dataTypes[i])));
      if (selectedOverlay == dataTypes[i]) {
         selectedIndex = i;
      }
   }
   if (selectedIndex >= 0) {
      dataTypeComboBox->setCurrentIndex(selectedIndex);
   }
   else {
      std::cout << "PROGRAM ERROR: Selected Overlay["
                << overlayNumber
                << "] is not a valid overlay."
                << std::endl;
   }
   dataTypeWidgetGroup->blockSignals(false);
   
   //
   // Update the display column combo box
   //
   displayColumnWidgetGroup->blockSignals(true);
   displayColumnComboBox->clear();
   displayColumnComboBox->addItems(
      bmsOverlay->getDisplayColumnNames(
         getSurfaceModelIndexFromDisplayControlDialog()));
   const int displayColumnSelected = 
      bmsOverlay->getDisplayColumnSelected(
         getSurfaceModelIndexFromDisplayControlDialog());
   if ((displayColumnSelected >= 0) &&
       (displayColumnSelected < static_cast<int>(displayColumnComboBox->count()))) {
      displayColumnComboBox->setCurrentIndex(displayColumnSelected);
   }
   const bool displayValidFlag = 
      bmsOverlay->getDisplayColumnValid(
         getSurfaceModelIndexFromDisplayControlDialog());
   displayColumnWidgetGroup->setEnabled(displayValidFlag);
   displayColumnWidgetGroup->blockSignals(false);

   //
   // Hide display controls if disabled
   //
   displayColumnWidgetGroup->setHidden(displayValidFlag == false);
   
   //
   // Update the threshold column combo box
   //
   thresholdColumnWidgetGroup->blockSignals(true);
   thresholdColumnComboBox->clear();
   thresholdColumnComboBox->addItems(
      bmsOverlay->getThresholdColumnNames(
         getSurfaceModelIndexFromDisplayControlDialog()));
   const int thresholdColumnSelected = 
      bmsOverlay->getThresholdColumnSelected(
         getSurfaceModelIndexFromDisplayControlDialog());
   if ((thresholdColumnSelected >= 0) &&
       (thresholdColumnSelected < static_cast<int>(thresholdColumnComboBox->count()))) {
      thresholdColumnComboBox->setCurrentIndex(thresholdColumnSelected);
   }
   const bool thresholdValidFlag = 
      bmsOverlay->getThresholdColumnValid(
         getSurfaceModelIndexFromDisplayControlDialog());
   thresholdColumnWidgetGroup->setEnabled(thresholdValidFlag);
   thresholdColumnWidgetGroup->blockSignals(false);
   
   //
   // Hide threshold controls if disabled
   //
   thresholdColumnWidgetGroup->setHidden(thresholdValidFlag == false);
   
   
   //
   // Update lighting
   //
   lightingCheckBox->blockSignals(true);
   lightingCheckBox->setChecked(bmsOverlay->getLightingEnabled());
   lightingCheckBox->blockSignals(false);
   
   //
   // Update opacity
   //
   opacityDoubleSpinBox->blockSignals(true);
   opacityDoubleSpinBox->setValue(bmsOverlay->getOpacity());
   opacityDoubleSpinBox->blockSignals(false);
}

/**
 * called when data type selected.
 */
void 
GuiDisplayControlSurfaceOverlayWidget::slotDataTypeComboBox(int item)
{
   //
   // Get the surface overlay
   //
   BrainModelSurfaceOverlay* bmsOverlay =
      theMainWindow->getBrainSet()->getSurfaceOverlay(overlayNumber);
   bmsOverlay->setOverlay(getSurfaceModelIndexFromDisplayControlDialog(),
      static_cast<BrainModelSurfaceOverlay::OVERLAY_SELECTIONS>(
         dataTypeComboBox->itemData(item).toInt()));
         
   updateDisplayControlAndRedrawSurfaces();
}

/**
 * update Display Control and redraw the surfaces.
 */
void 
GuiDisplayControlSurfaceOverlayWidget::updateDisplayControlAndRedrawSurfaces()
{
   updateWidget();
   
   //
   // Get the surface overlay
   //
   BrainModelSurfaceOverlay* bmsOverlay =
      theMainWindow->getBrainSet()->getSurfaceOverlay(overlayNumber);
   switch (bmsOverlay->getOverlay(getSurfaceModelIndexFromDisplayControlDialog())) {
      case BrainModelSurfaceOverlay::OVERLAY_NONE:
         break;
      case BrainModelSurfaceOverlay::OVERLAY_AREAL_ESTIMATION:
         displayControlDialog->updateArealEstimationItems();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_COCOMAC:
         displayControlDialog->updateCocomacItems();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_METRIC:
         displayControlDialog->updateMetricSelectionPage();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_PAINT:
         displayControlDialog->updatePaintColumnPage();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_PROBABILISTIC_ATLAS:
         break;
      case BrainModelSurfaceOverlay::OVERLAY_RGB_PAINT:
         displayControlDialog->updateRgbPaintItems();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_SECTIONS:
         displayControlDialog->updateSectionMainPage();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS:
         break;
      case BrainModelSurfaceOverlay::OVERLAY_SHOW_EDGES:
         break;
      case BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE:
         displayControlDialog->updateShapeSelections();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_TOPOGRAPHY:
         displayControlDialog->updateTopographyItems();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_GEOGRAPHY_BLENDING:
         break;
   }
   
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * called when display column selected.
 */
void 
GuiDisplayControlSurfaceOverlayWidget::slotDisplayColumnComboBox(int item)
{
   //
   // Get the surface overlay
   //
   BrainModelSurfaceOverlay* bmsOverlay =
      theMainWindow->getBrainSet()->getSurfaceOverlay(overlayNumber);
   bmsOverlay->setDisplayColumnSelected(getSurfaceModelIndexFromDisplayControlDialog(),
                                        item);
   if (displayColumnComboBox->count() == thresholdColumnComboBox->count()) {
      bmsOverlay->setThresholdColumnSelected(getSurfaceModelIndexFromDisplayControlDialog(),
                                             item);
      thresholdColumnComboBox->blockSignals(true);
      thresholdColumnComboBox->setCurrentIndex(item);
      thresholdColumnComboBox->blockSignals(false);
   }                             
   updateDisplayControlAndRedrawSurfaces();
}

/**
 * called when threshold column selected.
 */
void 
GuiDisplayControlSurfaceOverlayWidget::slotThresholdColumnComboBox(int item)
{
   //
   // Get the surface overlay
   //
   BrainModelSurfaceOverlay* bmsOverlay =
      theMainWindow->getBrainSet()->getSurfaceOverlay(overlayNumber);
   bmsOverlay->setThresholdColumnSelected(getSurfaceModelIndexFromDisplayControlDialog(),
                                item);
                                
   updateDisplayControlAndRedrawSurfaces();
}

/**
 * called when lighting checkbox toggled.
 */
void 
GuiDisplayControlSurfaceOverlayWidget::slotLightingCheckBox(bool toggled)
{
   //
   // Get the surface overlay
   //
   BrainModelSurfaceOverlay* bmsOverlay =
      theMainWindow->getBrainSet()->getSurfaceOverlay(overlayNumber);
   bmsOverlay->setLightingEnabled(toggled);

   updateDisplayControlAndRedrawSurfaces();
}

/**
 * called when opacity spin box value changed.
 */
void 
GuiDisplayControlSurfaceOverlayWidget::slotOpacityDoubleSpinBox(double value)
{
   //
   // Get the surface overlay
   //
   BrainModelSurfaceOverlay* bmsOverlay =
      theMainWindow->getBrainSet()->getSurfaceOverlay(overlayNumber);
   bmsOverlay->setOpacity(value);

   updateDisplayControlAndRedrawSurfaces();
}

/**
 * get the surface model index from display control dialog.
 */
int 
GuiDisplayControlSurfaceOverlayWidget::getSurfaceModelIndexFromDisplayControlDialog() const
{
   const int indx = displayControlDialog->getSurfaceModelIndex();
   return indx;
}

/**
 * edit column metadata. 
 */
void 
GuiDisplayControlSurfaceOverlayWidget::editColumnMetaData(const int columnNumber)
{
   //
   // Get the surface overlay
   //
   BrainModelSurfaceOverlay* bmsOverlay =
      theMainWindow->getBrainSet()->getSurfaceOverlay(overlayNumber);
      
   //
   // Get the data file
   //
   BrainSet* bs = theMainWindow->getBrainSet();
   GiftiNodeDataFile* gndf = NULL;
   NodeAttributeFile* naf = NULL;
   switch (bmsOverlay->getOverlay(getSurfaceModelIndexFromDisplayControlDialog())) {
      case BrainModelSurfaceOverlay::OVERLAY_NONE:
         break;
      case BrainModelSurfaceOverlay::OVERLAY_AREAL_ESTIMATION:
         naf = bs->getArealEstimationFile();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_COCOMAC:
         break;
      case BrainModelSurfaceOverlay::OVERLAY_METRIC:
         gndf = bs->getMetricFile();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_PAINT:
         gndf = bs->getPaintFile();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_PROBABILISTIC_ATLAS:
         break;
      case BrainModelSurfaceOverlay::OVERLAY_RGB_PAINT:
         naf = bs->getRgbPaintFile();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_SECTIONS:
         break;
      case BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS:
         break;
      case BrainModelSurfaceOverlay::OVERLAY_SHOW_EDGES:
         break;
      case BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE:
         gndf = bs->getSurfaceShapeFile();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_TOPOGRAPHY:
         naf = bs->getTopographyFile();
         break;
      case BrainModelSurfaceOverlay::OVERLAY_GEOGRAPHY_BLENDING:
         break;
   }
   
   //
   // Display help dialog
   //
   if (gndf != NULL) {
      if ((columnNumber >= 0) && (columnNumber < gndf->getNumberOfColumns())) {
         GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                       gndf, 
                                                                       columnNumber);
         dfcd->show();
      }
   }
   if (naf != NULL) {
      if ((columnNumber >= 0) && (columnNumber < naf->getNumberOfColumns())) {
         GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                       naf, 
                                                                       columnNumber);
         dfcd->show();
      }
   }
}
                        
/**
 * called when display column metadata button clicked.
 */
void 
GuiDisplayControlSurfaceOverlayWidget::slotDisplayColumnMetaDataToolButton()
{
   editColumnMetaData(displayColumnComboBox->currentIndex());
}

/**
 * called when threshold column metadata button clicked.
 */
void 
GuiDisplayControlSurfaceOverlayWidget::slotThresholdColumnMetaDataToolButton()
{
   editColumnMetaData(thresholdColumnComboBox->currentIndex());
}
      
      

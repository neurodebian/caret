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

#include <sstream>

#include <QButtonGroup>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QRadioButton>

#include "ArealEstimationFile.h"
#include "BrainSet.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "DeformationFieldFile.h"
#include "DisplaySettings.h"
#include "DisplaySettingsArealEstimation.h"
#include "DisplaySettingsDeformationField.h"
#include "DisplaySettingsGeodesicDistance.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsPaint.h"
#include "DisplaySettingsProbabilisticAtlas.h"
#include "DisplaySettingsRgbPaint.h"
#include "DisplaySettingsSection.h"
#include "DisplaySettingsSurfaceShape.h"
#include "DisplaySettingsSurfaceVectors.h"
#include "DisplaySettingsTopography.h"
#include "GeodesicDistanceFile.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiNodeAttributeFileClearResetDialog.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProbabilisticAtlasFile.h"
#include "QtUtilities.h"
#include "RgbPaintFile.h"
#include "SectionFile.h"
#include "SurfaceShapeFile.h"
#include "SurfaceVectorFile.h"
#include "TopographyFile.h"
#include "global_variables.h"

/**
 * constructor
 */
GuiNodeAttributeFileClearResetDialog::GuiNodeAttributeFileClearResetDialog(
                                           QWidget* parent,
                                           const GUI_NODE_FILE_TYPE nodeFileTypeIn)
   : WuQDialog(parent)
{
   setModal(true);
   nodeFileType = nodeFileTypeIn;
   
   QString caption;
   caption.append("Clear/Reset Data File");
   setWindowTitle(caption);
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Button group to keep the radio buttons mutually exclusive
   //
   QButtonGroup* radioButtonGroup = new QButtonGroup(this);
   
   //
   // Clear file contents radio button
   //
   clearFileRadioButton = new QRadioButton("Clear File Contents");
   dialogLayout->addWidget(clearFileRadioButton);
   radioButtonGroup->addButton(clearFileRadioButton);
                                           
   //
   // Clear file column radio button and combo box
   //
   QHBoxLayout* clearColumnHBoxLayout = new QHBoxLayout;
   dialogLayout->addLayout(clearColumnHBoxLayout);
   clearColumnRadioButton = new QRadioButton("Clear File Column");
   clearColumnHBoxLayout->addWidget(clearColumnRadioButton);
   radioButtonGroup->addButton(clearColumnRadioButton);
   clearColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                              nodeFileType,
                                              false,
                                              false,
                                              false);
   clearColumnHBoxLayout->addWidget(clearColumnComboBox);
                                           
   //
   // Remove file column radio button
   //
   QHBoxLayout* removeColumnHBoxLayout = new QHBoxLayout;
   dialogLayout->addLayout(removeColumnHBoxLayout);
   removeColumnRadioButton = new QRadioButton("Remove File Column");
   removeColumnHBoxLayout->addWidget(removeColumnRadioButton);
   radioButtonGroup->addButton(removeColumnRadioButton);
   removeColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                              nodeFileType,
                                              false,
                                              false,
                                              false);
   removeColumnHBoxLayout->addWidget(removeColumnComboBox);
                                           
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Apply button 
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
                    
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   updateDialog();
}

/**
 * Destructor
 */
GuiNodeAttributeFileClearResetDialog::~GuiNodeAttributeFileClearResetDialog()
{
}

/**
 * called when apply button pressed.
 */
void 
GuiNodeAttributeFileClearResetDialog::slotApplyButton()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   GuiFilesModified fm;
   
   switch (nodeFileType) {
      case GUI_NODE_FILE_TYPE_NONE:
         break;
      case GUI_NODE_FILE_TYPE_AREAL_ESTIMATION:
         if (clearFileRadioButton->isChecked()) {
            bs->clearArealEstimationFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getArealEstimationFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getArealEstimationFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsArealEstimation()->update();
         fm.setArealEstimationModified();
         break;
      case GUI_NODE_FILE_TYPE_DEFORMATION_FIELD:
         if (clearFileRadioButton->isChecked()) {
            bs->clearDeformationFieldFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getDeformationFieldFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getDeformationFieldFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsDeformationField()->update();
         fm.setDeformationFieldModified();
         break;
      case GUI_NODE_FILE_TYPE_GEODESIC_DISTANCE:
         if (clearFileRadioButton->isChecked()) {
            bs->clearGeodesicDistanceFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getGeodesicDistanceFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getGeodesicDistanceFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsGeodesicDistance()->update();
         fm.setGeodesicModified();
         break;
      case GUI_NODE_FILE_TYPE_LAT_LON:
         if (clearFileRadioButton->isChecked()) {
            bs->clearLatLonFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getLatLonFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getLatLonFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         fm.setLatLonModified();
         break;
      case GUI_NODE_FILE_TYPE_METRIC:
         if (clearFileRadioButton->isChecked()) {
            bs->clearMetricFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getMetricFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getMetricFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsMetric()->update();
         fm.setMetricModified();
         break;
      case GUI_NODE_FILE_TYPE_PAINT:
         if (clearFileRadioButton->isChecked()) {
            bs->clearPaintFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getPaintFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getPaintFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsPaint()->update();
         fm.setPaintModified();
         break;
      case GUI_NODE_PROBABILISTIC_ATLAS:
         if (clearFileRadioButton->isChecked()) {
            bs->clearProbabilisticAtlasFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getProbabilisticAtlasSurfaceFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getProbabilisticAtlasSurfaceFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsProbabilisticAtlasSurface()->update();
         fm.setProbabilisticAtlasModified();
         break;
      case GUI_NODE_FILE_TYPE_RGB_PAINT:
         if (clearFileRadioButton->isChecked()) {
            bs->clearRgbPaintFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getRgbPaintFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getRgbPaintFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsRgbPaint()->update();
         fm.setRgbPaintModified();
         break;
      case GUI_NODE_FILE_TYPE_SECTION:
         if (clearFileRadioButton->isChecked()) {
            bs->clearSectionFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getSectionFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getSectionFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsSection()->update();
         fm.setSectionModified();
         break;
      case GUI_NODE_FILE_TYPE_SURFACE_SHAPE:
         if (clearFileRadioButton->isChecked()) {
            bs->clearSurfaceShapeFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getSurfaceShapeFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getSurfaceShapeFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsSurfaceShape()->update();
         fm.setSurfaceShapeModified();
         break;
      case GUI_NODE_FILE_TYPE_SURFACE_VECTOR:
         if (clearFileRadioButton->isChecked()) {
            bs->clearSurfaceVectorFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getSurfaceVectorFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getSurfaceVectorFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsSurfaceVectors()->update();
         fm.setSurfaceVectorModified();
         break;
      case GUI_NODE_FILE_TYPE_TOPOGRAPHY:
         if (clearFileRadioButton->isChecked()) {
            bs->clearTopographyFile();
         }
         else if (clearColumnRadioButton->isChecked()) {
            bs->getTopographyFile()->resetColumn(clearColumnComboBox->currentIndex());
         }
         else if (removeColumnRadioButton->isChecked()) {
            bs->getTopographyFile()->removeColumn(removeColumnComboBox->currentIndex());
         }
         bs->getDisplaySettingsTopography()->update();
         fm.setTopographyModified();
         break;
   }

   theMainWindow->fileModificationUpdate(fm);
   bs->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL();
   updateDialog();
}

/**
 * called to update the dialog.
 */
void 
GuiNodeAttributeFileClearResetDialog::updateDialog()
{
   const bool haveColumns = (clearColumnComboBox->count() > 0);
   clearColumnComboBox->setEnabled(haveColumns);
   removeColumnComboBox->setEnabled(haveColumns);
   clearFileRadioButton->setEnabled(haveColumns);
   clearColumnRadioButton->setEnabled(haveColumns);
   removeColumnRadioButton->setEnabled(haveColumns);

   clearColumnComboBox->updateComboBox();
   removeColumnComboBox->updateComboBox();
}



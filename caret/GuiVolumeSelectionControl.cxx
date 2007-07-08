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

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiMainWindow.h"
#include "GuiVolumeSelectionControl.h"
#include "global_variables.h"

/**
 * Constructor.  Creates a grid widget with labels and combo boxes for selecting
 * a volume type and a specific volume of the selected volume type.  If only one
 * type of volume is enabled, the type selection is hidden.
 */
GuiVolumeSelectionControl::GuiVolumeSelectionControl(QWidget* parent,
                                                     const bool showAnatomyVolumes,
                                                     const bool showFunctionalVolumes,
                                                     const bool showPaintVolumes,
                                                     const bool showProbAtlasVolumes,
                                                     const bool showRgbPaintVolumes,
                                                     const bool showSegmentationVolumes,
                                                     const bool showVectorVolumes,
                                                     const LABEL_MODE labelModeIn,
                                                     const char* name,
                                                     const bool enableNewVolumeSelectionIn,
                                                     const bool enableNameTextEntryIn,
                                                     const bool enableDescriptiveLabelTextEntryIn)
   : QWidget(parent)
{
   setObjectName(name);
   
   enableNewVolumeSelection = enableNewVolumeSelectionIn;
   labelMode = labelModeIn;
   
   //
   //  Load the volume type combo box
   //
   volumeTypeLabel = new QLabel("Volume Type ");
   volumeTypeLabel->setFixedSize(volumeTypeLabel->sizeHint());
   volumeTypeComboBox = new QComboBox;
   QObject::connect(volumeTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(updateControl()));
   QString singleVolumeLabel;
   if (showAnatomyVolumes) {
      volumeTypesToDisplay.push_back(VolumeFile::VOLUME_TYPE_ANATOMY);
      volumeTypeComboBox->addItem("Anatomy");
      singleVolumeLabel = "Anatomy";
   }
   if (showFunctionalVolumes) {
      volumeTypesToDisplay.push_back(VolumeFile::VOLUME_TYPE_FUNCTIONAL);
      volumeTypeComboBox->addItem("Functional");
      singleVolumeLabel = "Functional";
   }
   if (showPaintVolumes) {
      volumeTypesToDisplay.push_back(VolumeFile::VOLUME_TYPE_PAINT);
      volumeTypeComboBox->addItem("Paint");
      singleVolumeLabel = "Paint";
   }
   if (showProbAtlasVolumes) {
      volumeTypesToDisplay.push_back(VolumeFile::VOLUME_TYPE_PROB_ATLAS);
      volumeTypeComboBox->addItem("Prob Atlas");
      singleVolumeLabel = "Prob Atlas";
   }
   if (showRgbPaintVolumes) {
      volumeTypesToDisplay.push_back(VolumeFile::VOLUME_TYPE_RGB);
      volumeTypeComboBox->addItem("RGB Paint");
      singleVolumeLabel = "RGB Paint";
   }
   if (showSegmentationVolumes) {
      volumeTypesToDisplay.push_back(VolumeFile::VOLUME_TYPE_SEGMENTATION);
      volumeTypeComboBox->addItem("Segmentation");
      singleVolumeLabel = "Segmentation";
   }
   if (showVectorVolumes) {
      volumeTypesToDisplay.push_back(VolumeFile::VOLUME_TYPE_VECTOR);
      volumeTypeComboBox->addItem("Vector");
      singleVolumeLabel = "Vector";
   }

   //
   // If only one type of volume is displayed, hide type selection
   //
   if (volumeTypeComboBox->count() == 1) {
      volumeTypeLabel->hide();
      volumeTypeComboBox->hide();
   }
   
   // 
   // Create the volume file selection combo box
   //
   volumeFileLabel = NULL;   
   if (volumeTypeComboBox->count() == 1) {
      volumeFileLabel = new QLabel(singleVolumeLabel);
   }
   else {
      volumeFileLabel = new QLabel("Volume File ");
   }
   volumeFileLabel->setFixedSize(volumeFileLabel->sizeHint());
   volumeFileComboBox = new QComboBox;
   QObject::connect(volumeFileComboBox, SIGNAL(activated(int)),
                    this, SLOT(updateNameLineEdit()));
   
   //
   // layout for widget
   //                                  
   QGridLayout* gridLayout = new QGridLayout(this);
   gridLayout->setSpacing(5);
   gridLayout->addWidget(volumeTypeLabel, 0, 0);
   gridLayout->addWidget(volumeTypeComboBox, 0, 1);
   gridLayout->addWidget(volumeFileLabel, 1, 0);
   gridLayout->addWidget(volumeFileComboBox, 1, 1);
   
   //
   // Should the volume name text entry be enabled
   //
   nameLabel = NULL;
   nameLineEdit = NULL;
   if (enableNameTextEntryIn) {
      const int rowNum = gridLayout->rowCount();
      nameLabel = new QLabel("Volume Name");
      gridLayout->addWidget(nameLabel, rowNum, 0);
      nameLineEdit = new QLineEdit;
      gridLayout->addWidget(nameLineEdit, rowNum, 1);
   }
   
   //
   // Should the volume descriptive label text entry be enabled
   //
   descriptiveLabel = NULL;
   descriptiveLabelLineEdit = NULL;
   if (enableDescriptiveLabelTextEntryIn) {
      descriptiveLabel = new QLabel("Volume Label");
      const int rowNum = gridLayout->rowCount();
      gridLayout->addWidget(descriptiveLabel, rowNum, 0);
      descriptiveLabelLineEdit = new QLineEdit;
      gridLayout->addWidget(descriptiveLabelLineEdit, rowNum, 1);
   }
   
   //
   // default the new volume index
   //
   newVolumeIndex = -1;
   
   //
   // Load the volume files
   //
   updateControl();
   
   QObject::connect(volumeTypeComboBox, SIGNAL(activated(int)),
                    this, SIGNAL(signalSelectionChanged()));
   QObject::connect(volumeFileComboBox, SIGNAL(activated(int)),
                    this, SIGNAL(signalSelectionChanged()));
}
                          
/**
 * Destructor.
 */
GuiVolumeSelectionControl::~GuiVolumeSelectionControl()
{
}

/**
 * show only the volume type.
 */
void 
GuiVolumeSelectionControl::showOnlyVolumeType(const bool hideIt)
{
   volumeFileLabel->setHidden(hideIt);
   volumeFileComboBox->setHidden(hideIt);
   if (nameLabel != NULL) {
      nameLabel->setHidden(hideIt);
   }
   if (nameLineEdit != NULL) {
      nameLineEdit->setHidden(hideIt);
   }
   if (descriptiveLabel != NULL) {
      descriptiveLabel->setHidden(hideIt);
   }
   if (descriptiveLabelLineEdit != NULL) {
      descriptiveLabelLineEdit->setHidden(hideIt);
   }
   
}
      
/**
 * Update the control.
 */
void 
GuiVolumeSelectionControl::updateControl()
{
   VolumeFile* currentVolume = NULL;
   
   //
   // Get volume type currently selected
   //
   const VolumeFile::VOLUME_TYPE vt = volumeTypesToDisplay[volumeTypeComboBox->currentIndex()];
      
   //
   // If there were volume files previously in this control
   //
   if (volumeFileComboBox->count() > 0) {
      //
      // Get the previously selected volume file
      //
      const int item = volumeFileComboBox->currentIndex();
      if ((item >= 0) && (item < static_cast<int>(volumeFiles.size()))) {
         currentVolume = volumeFiles[item];
      }
   }
   
   //
   // clear the volume file selection
   //
   volumeFiles.clear();
   volumeFileComboBox->clear();
   
   //
   // Get volumes for selected type
   //
   BrainSet* brainSet = theMainWindow->getBrainSet();
   BrainModelVolume* bmv = brainSet->getBrainModelVolume();
   VolumeFile* selVolume = NULL;
   switch (vt) {
      case VolumeFile::VOLUME_TYPE_ANATOMY:
         brainSet->getVolumeAnatomyFiles(volumeFiles);
         if (bmv != NULL) {
            selVolume = bmv->getSelectedVolumeAnatomyFile();
         }
         break;
      case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
         brainSet->getVolumeFunctionalFiles(volumeFiles);
         if (bmv != NULL) {
            selVolume = bmv->getSelectedVolumeFunctionalViewFile();
         }
         break;
      case VolumeFile::VOLUME_TYPE_PAINT:
         brainSet->getVolumePaintFiles(volumeFiles);
         if (bmv != NULL) {
            selVolume = bmv->getSelectedVolumePaintFile();
         }
         break;
      case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
         brainSet->getVolumeProbAtlasFiles(volumeFiles);
         break;
      case VolumeFile::VOLUME_TYPE_RGB:
         brainSet->getVolumeRgbFiles(volumeFiles);
         if (bmv != NULL) {
            selVolume = bmv->getSelectedVolumeRgbFile();
         }
         break;
      case VolumeFile::VOLUME_TYPE_ROI:
         break;
      case VolumeFile::VOLUME_TYPE_SEGMENTATION:
         brainSet->getVolumeSegmentationFiles(volumeFiles);
         if (bmv != NULL) {
            selVolume = bmv->getSelectedVolumeSegmentationFile();
         }
         break;
      case VolumeFile::VOLUME_TYPE_VECTOR:
         brainSet->getVolumeVectorFiles(volumeFiles);
         if (bmv != NULL) {
            selVolume = bmv->getSelectedVolumeVectorFile();
         }
      case VolumeFile::VOLUME_TYPE_UNKNOWN:
         break;
   }
   
   //
   // Override current volume with the selected volume
   //
   currentVolume = selVolume;
   
   //
   // Load the combo box
   //
   int defaultItem = -1;
   for (unsigned int i = 0; i < volumeFiles.size(); i++) {
      QString comboBoxLabel;
      switch (labelMode) {
         case LABEL_MODE_FILE_NAME:
            comboBoxLabel = FileUtilities::basename(volumeFiles[i]->getFileName());
            break;
         case LABEL_MODE_FILE_LABEL:
            comboBoxLabel = volumeFiles[i]->getDescriptiveLabel();
            break;
         case LABEL_MODE_FILE_LABEL_AND_NAME:
            {
               const QString name(FileUtilities::basename(volumeFiles[i]->getFileName()));
               const QString label(volumeFiles[i]->getDescriptiveLabel());
               if ((name != label) && (label.isEmpty() == false)) {
                  comboBoxLabel.append(label);
                  comboBoxLabel.append("  (");
                  comboBoxLabel.append(name);
                  comboBoxLabel.append(")");
               }
               else {
                  if (label.isEmpty() == false) {
                     comboBoxLabel.append(label);
                  }
                  else {
                     comboBoxLabel.append(name);
                  }
               }
            }            
            break;
      }
      volumeFileComboBox->addItem(comboBoxLabel);
      if (volumeFiles[i] == currentVolume) {
         defaultItem = i;
      }
   }
   
   //
   // If new volume enabled
   //
   newVolumeIndex = -1;
   if (enableNewVolumeSelection) {
      newVolumeIndex = volumeFileComboBox->count();
      volumeFileComboBox->addItem("New Volume");
   }
   
   //
   // If volume file was already in combo box
   //
   if (defaultItem >= 0) {
      volumeFileComboBox->setCurrentIndex(defaultItem);
   }
   
   updateNameLineEdit();
}
      
/**
 * get the name for the selected file.
 */
QString 
GuiVolumeSelectionControl::getSelectedVolumeFileName() const
{
   QString name("Invalid");
   if (nameLineEdit != NULL) {
      name = nameLineEdit->text();
   }
   else {
      const VolumeFile* vf = getSelectedVolumeFile();
      if (vf != NULL) {
         name = vf->getFileName();
      }      
   }
   
   return name;   
}
      
/**
 * get the descriptive label for the selected file.
 */
QString 
GuiVolumeSelectionControl::getSelectedVolumeDescriptiveLabel() const
{
   QString label("Invalid");
   if (descriptiveLabelLineEdit != NULL) {
      label = descriptiveLabelLineEdit->text();
   }
   else {
      const VolumeFile* vf = getSelectedVolumeFile();
      if (vf != NULL) {
         label = vf->getFileName();
         label = vf->getDescriptiveLabel();
      }      
   }
   
   return label;   
}
      
/**
 * Update the volume name line edit.
 */
void 
GuiVolumeSelectionControl::updateNameLineEdit()
{      
   if (nameLineEdit != NULL) {
      QString name("volume");
      const VolumeFile* vf = getSelectedVolumeFile();
      if (vf != NULL) {
         name = vf->getFileName();
      }
      nameLineEdit->setText(name);
   }
   
   if (descriptiveLabelLineEdit != NULL) {
      QString name("label");
      const VolumeFile* vf = getSelectedVolumeFile();
      if (vf != NULL) {
         name = vf->getDescriptiveLabel();
      }
      descriptiveLabelLineEdit->setText(name);
   }
}

/**
 * get the selected volume type.
 */
VolumeFile::VOLUME_TYPE 
GuiVolumeSelectionControl::getSelectedVolumeType() const
{
   VolumeFile::VOLUME_TYPE vt = VolumeFile::VOLUME_TYPE_UNKNOWN;
   const int item = volumeTypeComboBox->currentIndex();
   if ((item >= 0) && (item < static_cast<int>(volumeTypesToDisplay.size()))) {
      vt = volumeTypesToDisplay[item];
   }
   return vt;
}
      
/**
 * get the selected volume file.
 */
VolumeFile* 
GuiVolumeSelectionControl::getSelectedVolumeFile()
{
   VolumeFile * vf = NULL;
   const int item = volumeFileComboBox->currentIndex();
   if ((item >= 0) && (item < static_cast<int>(volumeFiles.size()))) {
      vf = volumeFiles[item];
   } 
   return vf;
}

/**
 * get the selected volume file (const method).
 */
const VolumeFile* 
GuiVolumeSelectionControl::getSelectedVolumeFile() const
{
   VolumeFile * vf = NULL;
   const int item = volumeFileComboBox->currentIndex();
   if ((item >= 0) && (item < static_cast<int>(volumeFiles.size()))) {
      vf = volumeFiles[item];
   } 
   return vf;
}      

/**
 * get "New Volume" selected.
 */
bool 
GuiVolumeSelectionControl::getNewVolumeSelected() const
{
   const int item = volumeFileComboBox->currentIndex();
   if (item >= 0) {
      if (item == newVolumeIndex) {
         return true;
      }
   }
   return false;
}

/**
 * set the selected volume file.
 */
void 
GuiVolumeSelectionControl::setSelectedVolumeFile(const VolumeFile* vf)
{
   for (unsigned int i = 0; i < volumeFiles.size(); i++) {
      if (volumeFiles[i] == vf) {
         volumeFileComboBox->setCurrentIndex(i);
         updateNameLineEdit();
         break;
      }
   }
}      

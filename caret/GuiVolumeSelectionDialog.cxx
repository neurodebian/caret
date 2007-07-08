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

#include <QLayout>
#include <QPushButton>

#include "GuiVolumeSelectionDialog.h"
#include "QtUtilities.h"
/**
 * Constructor.
 */
GuiVolumeSelectionDialog::GuiVolumeSelectionDialog(QWidget* parent,
                          const bool showAnatomyVolumes,
                          const bool showFunctionalVolumes,
                          const bool showPaintVolumes,
                          const bool showProbAtlasVolumes,
                          const bool showRgbPaintVolumes,
                          const bool showSegmentationVolumes,
                          const bool showVectorVolumes,
                          const GuiVolumeSelectionControl::LABEL_MODE labelModeIn,
                          const char* name,
                          const bool enableNewVolumeSelectionIn,
                          const bool enableNameTextEntryIn,
                          const bool enableDescriptiveLabelTextEntryIn)
   : QtDialog(parent, true)
{
   setWindowTitle("Volume Selection");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(3);
   dialogLayout->setSpacing(3);
   
   //
   // Volume selection control
   //
   volumeSelectionControl = new GuiVolumeSelectionControl(
                                  this,
                                  showAnatomyVolumes,
                                  showFunctionalVolumes,
                                  showPaintVolumes,
                                  showProbAtlasVolumes,
                                  showRgbPaintVolumes,
                                  showSegmentationVolumes,
                                  showVectorVolumes,
                                  labelModeIn,
                                  name,
                                  enableNewVolumeSelectionIn,
                                  enableNameTextEntryIn,
                                  enableDescriptiveLabelTextEntryIn);
   dialogLayout->addWidget(volumeSelectionControl);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;;
   buttonsLayout->setSpacing(3);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   buttonsLayout->addWidget(okButton);
   okButton->setAutoDefault(false);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
                    
   //
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   cancelButton->setAutoDefault(false);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * Destructor.
 */
GuiVolumeSelectionDialog::~GuiVolumeSelectionDialog()
{
}

/** 
 * get the selected volume type.
 */
VolumeFile::VOLUME_TYPE 
GuiVolumeSelectionDialog::getSelectedVolumeType() const
{
   return volumeSelectionControl->getSelectedVolumeType();
}

/**
 * get the selected volume file (if null NEW might be selected).
 */
VolumeFile* 
GuiVolumeSelectionDialog::getSelectedVolumeFile()
{
   return volumeSelectionControl->getSelectedVolumeFile();
}

/**
 * get the selected volume file (const method).
 */
const VolumeFile* 
GuiVolumeSelectionDialog::getSelectedVolumeFile() const
{
   return volumeSelectionControl->getSelectedVolumeFile();
}

/**
 * get "New Volume" selected.
 */
bool 
GuiVolumeSelectionDialog::getNewVolumeSelected() const
{
   return volumeSelectionControl->getNewVolumeSelected();
}

/**
 * set the selected volume file.
 */
void 
GuiVolumeSelectionDialog::setSelectedVolumeFile(const VolumeFile* vf)
{
   volumeSelectionControl->setSelectedVolumeFile(vf);
}

/**
 * get the name for the selected file.
 */
QString 
GuiVolumeSelectionDialog::getSelectedVolumeFileName() const
{
   return volumeSelectionControl->getSelectedVolumeFileName();
}

/**
 * get the descriptive label for the selected file.
 */
QString 
GuiVolumeSelectionDialog::getSelectedVolumeDescriptiveLabel() const
{
   return volumeSelectionControl->getSelectedVolumeDescriptiveLabel();
}      


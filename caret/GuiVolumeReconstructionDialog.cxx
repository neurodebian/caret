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
#include <limits>

#define __GUI_VOLUME_RECONSTRUCTION_DIALOG_MAIN__
#include "GuiVolumeReconstructionDialog.h"
#undef __GUI_VOLUME_RECONSTRUCTION_DIALOG_MAIN__

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include "BrainModelVolumeToSurfaceConverter.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "QtUtilities.h"
#include "VolumeFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiVolumeReconstructionDialog::GuiVolumeReconstructionDialog(QWidget* parent,
                                                      VolumeFile* segmentationVolumeFileIn,
                                                      bool modalFlag, Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   setModal(modalFlag);
   
   segmentationVolumeFile = segmentationVolumeFileIn;
   
   setWindowTitle("Volume Reconstruction Into Surface");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);

   //
   // Type of surface to generate
   //   
   surfaceTypeComboBox = new QComboBox;
   surfaceTypeComboBox->insertItem(SURFACE_TYPE_BRAIN_MODEL, "Brain Model Surface");
   surfaceTypeComboBox->insertItem(SURFACE_TYPE_SOLID_STRUCTURE, "Solid Structure VTK Model");
   surfaceTypeComboBox->insertItem(SURFACE_TYPE_VTK_MODEL, "VTK Model");
   QObject::connect(surfaceTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSurfaceTypeComboBox(int)));
   QGroupBox* surfaceTypeGroupBox = new QGroupBox("Surface Type");
   dialogLayout->addWidget(surfaceTypeGroupBox);
   QHBoxLayout* surfaceTypeGroupLayout = new QHBoxLayout(surfaceTypeGroupBox);
   surfaceTypeGroupLayout->addWidget(surfaceTypeComboBox);
   
   //
   // left and right hemisphere checkboxes
   //
   leftHemisphereCheckBox = new QCheckBox("Left");
   rightHemisphereCheckBox = new QCheckBox("Right");
   
   //
   // Group box for hemisphere
   //
   hemisphereGroup = new QGroupBox("Hemisphere");
   dialogLayout->addWidget(hemisphereGroup);
   QVBoxLayout* hemisphereLayout = new QVBoxLayout(hemisphereGroup);
   hemisphereLayout->addWidget(leftHemisphereCheckBox);
   hemisphereLayout->addWidget(rightHemisphereCheckBox);
   
   //
   // Hypersmooth option
   //
   hypersmoothCheckBox = new QCheckBox("Generate Hypersmooth Surface");
   
   //
   // Maximum Polygons check box
   //
   maximumPolygonsCheckBox = new QCheckBox("Maximum Polygons");
                                           
   //
   // Misc box
   //
   miscGroup = new QGroupBox("Misc", this);
   dialogLayout->addWidget(miscGroup);
   QVBoxLayout* miscLayout = new QVBoxLayout(miscGroup);
   miscLayout->addWidget(hypersmoothCheckBox);
   miscLayout->addWidget(maximumPolygonsCheckBox);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button 
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
                    
   //
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   
   switch (theMainWindow->getBrainSet()->getStructure().getType()) {
      case Structure::STRUCTURE_TYPE_INVALID:
         leftHemisphereCheckBox->setChecked(false);
         rightHemisphereCheckBox->setChecked(false);
         break;         
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
         leftHemisphereCheckBox->setChecked(true);
         rightHemisphereCheckBox->setChecked(false);
         break;         
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
         leftHemisphereCheckBox->setChecked(false);
         rightHemisphereCheckBox->setChecked(true);
         break;         
      case Structure::STRUCTURE_TYPE_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
         leftHemisphereCheckBox->setChecked(true);
         rightHemisphereCheckBox->setChecked(true);
         break;         
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
         leftHemisphereCheckBox->setChecked(false);
         rightHemisphereCheckBox->setChecked(false);
         break;         
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
         leftHemisphereCheckBox->setChecked(false);
         rightHemisphereCheckBox->setChecked(false);
         break;         
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
         leftHemisphereCheckBox->setChecked(false);
         rightHemisphereCheckBox->setChecked(false);
         break;         
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
         leftHemisphereCheckBox->setChecked(false);
         rightHemisphereCheckBox->setChecked(false);
         break;         
   }
   
   surfaceTypeComboBox->setCurrentIndex(0);
   slotSurfaceTypeComboBox(0);
}

/**
 * Destructor.
 */
GuiVolumeReconstructionDialog::~GuiVolumeReconstructionDialog()
{
}

/**
 * called when surface type selected.
 */
void 
GuiVolumeReconstructionDialog::slotSurfaceTypeComboBox(int item)
{
   hemisphereGroup->setEnabled(item == 0);
   miscGroup->setEnabled(item == 0);
}      

/**
 * Called when OK or Cancel button pressed.
 */
void
GuiVolumeReconstructionDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      if (segmentationVolumeFile == NULL) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Convert to Surface Error", 
                               "There is no selected segmentation volume file.");
         return;
      }
      
      BrainModelVolumeToSurfaceConverter::RECONSTRUCTION_MODE reconMode =
                     BrainModelVolumeToSurfaceConverter::RECONSTRUCTION_MODE_SUREFIT_SURFACE;
      switch (static_cast<SURFACE_TYPE>(surfaceTypeComboBox->currentIndex())) {
         case SURFACE_TYPE_BRAIN_MODEL:
            if (maximumPolygonsCheckBox->isChecked()) {
               reconMode = BrainModelVolumeToSurfaceConverter::RECONSTRUCTION_MODE_SUREFIT_SURFACE_MAXIMUM_POLYGONS;
            }
            else {
               reconMode = BrainModelVolumeToSurfaceConverter::RECONSTRUCTION_MODE_SUREFIT_SURFACE;
            }
            break;
         case SURFACE_TYPE_SOLID_STRUCTURE:
            reconMode = BrainModelVolumeToSurfaceConverter::RECONSTRUCTION_MODE_SOLID_STRUCTURE;
            break;
         case SURFACE_TYPE_VTK_MODEL:
            if (maximumPolygonsCheckBox->isChecked()) {
               reconMode = BrainModelVolumeToSurfaceConverter::RECONSTRUCTION_MODE_VTK_MODEL_MAXIMUM_POLYGONS;
            }
            else {
               reconMode = BrainModelVolumeToSurfaceConverter::RECONSTRUCTION_MODE_VTK_MODEL;
            }
            break;
      }
 
      const bool doHyper = hypersmoothCheckBox->isChecked() &&
                           (reconMode != BrainModelVolumeToSurfaceConverter::RECONSTRUCTION_MODE_VTK_MODEL);
            
      BrainModelVolumeToSurfaceConverter bmvsc(theMainWindow->getBrainSet(),
                                              segmentationVolumeFile,
                                              reconMode,
                                              rightHemisphereCheckBox->isChecked(),
                                              leftHemisphereCheckBox->isChecked(),
                                              doHyper);
      try {
         bmvsc.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Convert to Surface Error", e.whatQString());
         return;
      }
      
      GuiFilesModified fm;
      fm.setStatusForAll(true);

/**
      fm.setCoordinateModified();
      fm.setTopologyModified();
      fm.setVtkModelModified();
 */
      theMainWindow->fileModificationUpdate(fm);
      BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
      if (bms != NULL) {
         theMainWindow->displayBrainModelInMainWindow(bms);
      }
      else {
         theMainWindow->displayNewestSurfaceInMainWindow();
      }
      GuiBrainModelOpenGL::updateAllGL(NULL);
      
      QApplication::restoreOverrideCursor();
      
      theMainWindow->speakText("The surface has been reconstructed.", false);
   }
   
   QDialog::done(r);
}

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
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QPushButton>

#include "AreaColorFile.h"
#include "BrainModelVolume.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorSelectionDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiNameSelectionDialog.h"
#include "GuiVolumePaintEditorDialog.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiVolumePaintEditorDialog::GuiVolumePaintEditorDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setWindowTitle("Paint Volume Editor");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Create the toggle voxels section
   //
   dialogLayout->addWidget(createToggleVoxelsSection());
   
   //
   // Paint name
   //
   QLabel* paintNameLabel = new QLabel("Paint Name ");
   paintNameLineEdit = new QLineEdit;
   QPushButton* nameButton = new QPushButton("Select...");
   nameButton->setFixedSize(nameButton->sizeHint());
   nameButton->setAutoDefault(false);
   QObject::connect(nameButton, SIGNAL(clicked()),
                    this, SLOT(slotNameSelectionPushButton()));
   
   //
   // Arrange paint name items
   //
   QHBoxLayout* paintNameLayout = new QHBoxLayout;
   paintNameLayout->addWidget(paintNameLabel);
   paintNameLayout->addWidget(paintNameLineEdit);
   paintNameLayout->addWidget(nameButton);
   paintNameLayout->setStretchFactor(paintNameLabel, 0);
   paintNameLayout->setStretchFactor(paintNameLineEdit, 1000);
   paintNameLayout->setStretchFactor(nameButton, 0);
   dialogLayout->addLayout(paintNameLayout);
   
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
   buttonsLayout->addWidget(applyButton);
   applyButton->setAutoDefault(false);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
                    
   //
   //
   // Undo button
   //
   undoButton = new QPushButton("Undo");
   buttonsLayout->addWidget(undoButton);
   undoButton->setAutoDefault(false);
   QObject::connect(undoButton, SIGNAL(clicked()),
                    this, SLOT(slotUndoButton()));
                    
   //
   // Close button 
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));

   //
   // Make buttons same size
   //
   QtUtilities::makeButtonsSameSize(applyButton, undoButton, closeButton);
       
   //
   // Initialize dialog
   //
   clearUndoStack();
   
   //
   // Update the dialog
   //
   updateDialog();
}

/**
 * Destructor.
 */
GuiVolumePaintEditorDialog::~GuiVolumePaintEditorDialog()
{
}

/**
 * called when name select push button is pressed.
 */
void 
GuiVolumePaintEditorDialog::slotNameSelectionPushButton()
{
   static GuiNameSelectionDialog::LIST_ITEMS_TYPE itemForDisplay =
                   GuiNameSelectionDialog::LIST_AREA_COLORS_ALPHA;

   GuiNameSelectionDialog nsd(this,
                              GuiNameSelectionDialog::LIST_ALL,
                              itemForDisplay);
   if (nsd.exec() == QDialog::Accepted) {
      QString name(nsd.getName());
      if (name.isEmpty() == false) {
         paintNameLineEdit->setText(name);
      }
   }
}

/**
 * update the dialog.
 */
void 
GuiVolumePaintEditorDialog::updateDialog()
{
}
      
/**
 * called when apply button is pressed.
 */
void 
GuiVolumePaintEditorDialog::slotApplyButton()
{
   QString paintName = paintNameLineEdit->text();

   //
   // Find the matching color
   //
   bool areaColorMatch = false;
   AreaColorFile* areaColorFile = theMainWindow->getBrainSet()->getAreaColorFile();
   const int areaColorIndex = areaColorFile->getColorIndexByName(paintName, areaColorMatch);
   
   //
   // Area color may need to be created
   //
   if (areaColorMatch == false) {
      QString msg("Matching area color \"");
      msg.append(paintName);
      msg.append("\" not found");
      QString defineButton("Define Area Color ");
      int result = 0;
      if (areaColorIndex >= 0) {
         QString partialMatchButton("Use ");
         partialMatchButton.append(areaColorFile->getColorNameByIndex(areaColorIndex));
         result = GuiMessageBox::information(this, "Set Area Color",
                                 msg, 
                                 defineButton,
                                 partialMatchButton);
      }
      
      if (result == 0) {
         //
         // define the area color
         //
         QString title("Create Area Color: ");
         title.append(paintName);
         GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this, 
                                                                    title,
                                                                    false,
                                                                    false, 
                                                                    false, 
                                                                    false);
         csd->exec();
         
         //
         // Add new area color
         //
         float pointSize = 2.0, lineSize = 1.0;
         unsigned char r, g, b, a;
         ColorFile::ColorStorage::SYMBOL symbol;
         csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
         areaColorFile->addColor(paintName, r, g, b, a, pointSize, lineSize, symbol);  
                   
         //
         // Area Color File has changed
         //
         GuiFilesModified fm;
         fm.setAreaColorModified();
         theMainWindow->fileModificationUpdate(fm);
      }
      else if (result == 2) {
         //
         // do nothing so that partially matching color is used
         //
      }
   }

   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                  GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_PAINT_EDIT);
}
      
/**
 * Create the toggle voxels widget.
 */
QWidget*
GuiVolumePaintEditorDialog::createToggleVoxelsSection()
{
   //
   // Voxel size combo box and label
   //
   QLabel* toggleVoxelsLabel = new QLabel("Brush Size");
   toggleVoxelsSizeComboBox = new QComboBox;
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_1_BY_1, "1 x 1");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_3_BY_3, "3 x 3");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_5_BY_5, "5 x 5");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_7_BY_7, "7 x 7");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_9_BY_9, "9 x 9");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_11_BY_11, "11 x 11");
   toggleVoxelsSizeComboBox->setCurrentIndex(TOGGLE_VOXELS_SIZE_1_BY_1);

   //
   // Layout the items
   //
   QWidget* toggleVoxelsWidget = new QWidget;
   QHBoxLayout* toggleVoxelsLayout = new QHBoxLayout(toggleVoxelsWidget);
   toggleVoxelsLayout->addWidget(toggleVoxelsLabel);
   toggleVoxelsLayout->addWidget(toggleVoxelsSizeComboBox);
   toggleVoxelsLayout->addStretch();
   return toggleVoxelsWidget;
}

/**
 * called when dialog is closed.
 */
void
GuiVolumePaintEditorDialog::close()
{
   clearUndoStack();
   
   //
   // If in paint edit mode, switch to view mode
   //
   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   if (openGL->getMouseMode() == GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_PAINT_EDIT) {
      openGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   }
   QDialog::close();
}

/**
 * called when dialog is popped up.
 */
void 
GuiVolumePaintEditorDialog::show()
{
   clearUndoStack();
   
   QtDialog::show();
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                  GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_PAINT_EDIT);
}
      
/**
 * Called when a voxel is selected for editing in the main window.
 */
void 
GuiVolumePaintEditorDialog::processVoxel(const int ii, const int jj, const int kk,
                                         const VolumeFile::VOLUME_AXIS pickAxis)
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      close();
      return;
   }
   
   VolumeFile::VOLUME_AXIS axis = bmv->getSelectedAxis(0);
   switch (axis) {
      case VolumeFile::VOLUME_AXIS_X:
         break;
      case VolumeFile::VOLUME_AXIS_Y:
         break;
      case VolumeFile::VOLUME_AXIS_Z: 
         break;
      case VolumeFile::VOLUME_AXIS_ALL:
         if ((pickAxis == VolumeFile::VOLUME_AXIS_X) ||
             (pickAxis == VolumeFile::VOLUME_AXIS_Y) ||
             (pickAxis == VolumeFile::VOLUME_AXIS_Z)) {
            axis = pickAxis;
         }
         else {
            return;
         }
         break;
      case VolumeFile::VOLUME_AXIS_OBLIQUE:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
         GuiMessageBox::critical(this, "ERROR", 
                                 "Editing of oblique slices is not allowed.", "OK");
         return;
      case VolumeFile::VOLUME_AXIS_UNKNOWN:
         return;
         break;
   }
   
   VolumeFile* vf = bmv->getSelectedVolumePaintFile();
   if (vf != NULL) {
      //
      // Set indice range for voxels that are to be processed
      //               
      int mm = 0;
      switch (static_cast<TOGGLE_VOXELS_SIZE>(toggleVoxelsSizeComboBox->currentIndex())) {
         case TOGGLE_VOXELS_SIZE_1_BY_1:
            mm = 0;
            break;
         case TOGGLE_VOXELS_SIZE_3_BY_3:
            mm = 1;
            break;
         case TOGGLE_VOXELS_SIZE_5_BY_5:
            mm = 2;
            break;
         case TOGGLE_VOXELS_SIZE_7_BY_7:
            mm = 3;
            break;
         case TOGGLE_VOXELS_SIZE_9_BY_9:
            mm = 4;
            break;
         case TOGGLE_VOXELS_SIZE_11_BY_11:
            mm = 5;
            break;
      }
      int ijkMin[3] = { ii - mm, jj - mm, kk - mm };
      int ijkMax[3] = { ii + mm, jj + mm, kk + mm };
      
      //
      // Process three dim aspect
      //
      int threeDim = 0;
      switch (axis) {
         case VolumeFile::VOLUME_AXIS_X:
            ijkMin[0] = ii - threeDim;
            ijkMax[0] = ii + threeDim;
            break;
         case VolumeFile::VOLUME_AXIS_Y:
            ijkMin[1] = jj - threeDim;
            ijkMax[1] = jj + threeDim;
            break;
         case VolumeFile::VOLUME_AXIS_Z: 
            ijkMin[2] = kk - threeDim;
            ijkMax[2] = kk + threeDim;
            break;
         case VolumeFile::VOLUME_AXIS_ALL:
         case VolumeFile::VOLUME_AXIS_OBLIQUE:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
         case VolumeFile::VOLUME_AXIS_UNKNOWN:
            break;
      }
      
      VolumeModification modifiedVoxels;
      
      //
      // Perform operation on the volume
      //
      vf->assignPaintVolumeVoxels(paintNameLineEdit->text(),
                                  ijkMin,
                                  ijkMax,
                                  &modifiedVoxels);
      if (modifiedVoxels.getNumberOfVoxels()) {
         undoStack.push(modifiedVoxels);
      }
      enableUndoButton();
      
      GuiBrainModelOpenGL::updateAllGL();
   }
   
   QApplication::restoreOverrideCursor();
}
   
/**
 * Clear the undo stack
 */
void
GuiVolumePaintEditorDialog::clearUndoStack()
{
   while (undoStack.empty() == false) {
      undoStack.pop();
   }
   enableUndoButton();
}

/**
 * Called when the undo button is pressed.
 */
void
GuiVolumePaintEditorDialog::slotUndoButton()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      close();
      return;
   }
   
   VolumeFile* vf = bmv->getSelectedVolumePaintFile();
   if (vf != NULL) {
      if (undoStack.empty() == false) {
         VolumeModification volumeModification = undoStack.top();
         undoStack.pop();
         
         vf->undoModification(&volumeModification);
      }
      
      GuiBrainModelOpenGL::updateAllGL();
   }
   
   enableUndoButton();
}

/**
 * Enable the undo button.
 */
void
GuiVolumePaintEditorDialog::enableUndoButton()
{
   undoButton->setEnabled(undoStack.empty() == false);
}


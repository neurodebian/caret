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
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTextBrowser>

#include "BrainModelVolume.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiVolumeSegmentationEditorDialog.h"
#include "GuiVolumeSelectionControl.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiVolumeSegmentationEditorDialog::GuiVolumeSegmentationEditorDialog(QWidget* parent)
   : WuQDialog(parent)
{
   editMode = EDIT_MODE_TURN_VOXELS_ON;
   setWindowTitle("Segmentation Volume Editor");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Add to the mode combo box
   //
   modeComboBox = new QComboBox;
   QObject::connect(modeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotModeComboBox(int)));
   modeComboBox->insertItem(EDIT_MODE_COPY_SUBVOLUME, "Copy Sub-Volume");
   modeComboBox->insertItem(EDIT_MODE_DILATE, "Dilate");
   modeComboBox->insertItem(EDIT_MODE_ERODE, "Erode");
   modeComboBox->insertItem(EDIT_MODE_FLOOD_FILL_2D, "Flood Fill 2D");
   modeComboBox->insertItem(EDIT_MODE_FLOOD_FILL_3D, "Flood Fill 3D");
   modeComboBox->insertItem(EDIT_MODE_REMOVE_CONNECTED_2D, "Remove Connected Voxels 2D");
   modeComboBox->insertItem(EDIT_MODE_REMOVE_CONNECTED_3D, "Remove Connected Voxels 3D");
   modeComboBox->insertItem(EDIT_MODE_RETAIN_CONNECTED_3D, "Retain Connected Voxels 3D");
   modeComboBox->insertItem(EDIT_MODE_TURN_VOXELS_ON, "Turn Voxels On");
   modeComboBox->insertItem(EDIT_MODE_TURN_VOXELS_OFF, "Turn Voxels Off");
   
   //
   // Horizontal layout for mode label and combo box
   //
   QGroupBox* modeGroupBox = new QGroupBox("Editing Mode");
   dialogLayout->addWidget(modeGroupBox);
   QVBoxLayout* modeGroupLayout = new QVBoxLayout(modeGroupBox);
   modeGroupLayout->addWidget(modeComboBox);
   modeGroupLayout->addStretch();
   
   //
   // Mode options widget stack
   //   
   QGroupBox* optionsGroupBox = new QGroupBox("Editing Control");
   dialogLayout->addWidget(optionsGroupBox);
   QVBoxLayout* optionsGroupLayout = new QVBoxLayout(optionsGroupBox);
   
   //
   // Create the toggle voxels section
   //
   optionsGroupLayout->addWidget(createToggleVoxelsSection());
   
   //
   // Create the other volume section
   dialogLayout->addWidget(createOtherVolumeSection());
   
   //
   // Text Browser for help 
   //
   helpBrowser = new QTextBrowser;
   //QT4 CONVERSION helpBrowser->setBackgroundMode(PaletteBackground);
   helpBrowser->setReadOnly(true);
   QGroupBox* helpGroup = new QGroupBox("Information");
   dialogLayout->addWidget(helpGroup);
   QHBoxLayout* helpLayout = new QHBoxLayout(helpGroup);
   helpLayout->addWidget(helpBrowser);
   
   //
   // Limit height of help group
   //
   const int maxHeight = 150;
   int height = helpGroup->sizeHint().height();
   if (height > maxHeight) {
      height = maxHeight;
   }
   helpGroup->setMaximumHeight(maxHeight);
   
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
   modeComboBox->setCurrentIndex(editMode);
   slotModeComboBox(editMode);
   clearUndoStack();
   
   //
   // Update the dialog
   //
   updateDialog();
}

/**
 * Destructor.
 */
GuiVolumeSegmentationEditorDialog::~GuiVolumeSegmentationEditorDialog()
{
}

/**
 * update the dialog.
 */
void 
GuiVolumeSegmentationEditorDialog::updateDialog()
{
   otherVolumeComboBox->updateControl();
}
      
/**
 * called when apply button is pressed.
 */
void 
GuiVolumeSegmentationEditorDialog::slotApplyButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                  GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_SEGMENTATION_EDIT);
}
      
/**
 * Set the help information.
 */
void
GuiVolumeSegmentationEditorDialog::loadHelpInformation(const QString& info)
{
   helpBrowser->setHtml(info);
}

/**
 * Create the toggle voxels widget.
 */
QWidget*
GuiVolumeSegmentationEditorDialog::createToggleVoxelsSection()
{
   //
   // Voxel size combo box and label
   //
   QLabel* brushSizeLabel = new QLabel("Brush Size");
   toggleVoxelsSizeComboBox = new QComboBox;
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_1_BY_1, "1 x 1");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_3_BY_3, "3 x 3");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_5_BY_5, "5 x 5");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_7_BY_7, "7 x 7");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_9_BY_9, "9 x 9");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_11_BY_11, "11 x 11");
   toggleVoxelsSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_ALL, "All Voxels in Slice");
   toggleVoxelsSizeComboBox->setCurrentIndex(TOGGLE_VOXELS_SIZE_1_BY_1);
   currentSliceGroupBox = new QGroupBox("Current Slice");
   QHBoxLayout* currentSliceGroupLayout = new QHBoxLayout(currentSliceGroupBox);
   currentSliceGroupLayout->addWidget(brushSizeLabel);
   currentSliceGroupLayout->addWidget(toggleVoxelsSizeComboBox);
   
   //
   // Three D
   //
   QLabel* brushThicknessLabel = new QLabel("Brush Thickness (3D)");
   threeDimSizeComboBox = new QComboBox;
   threeDimSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_1_BY_1, "Current Slice Only");
   threeDimSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_3_BY_3, "Current Slice +/- 1");
   threeDimSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_5_BY_5, "Current Slice +/- 2");
   threeDimSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_7_BY_7, "Current Slice +/- 3");
   threeDimSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_9_BY_9, "Current Slice +/- 4");
   threeDimSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_11_BY_11, "Current Slice +/- 5");
   threeDimSizeComboBox->insertItem(TOGGLE_VOXELS_SIZE_ALL, "All Slices");
   threeDimSizeComboBox->setCurrentIndex(TOGGLE_VOXELS_SIZE_1_BY_1);
   threeDimGroupBox = new QGroupBox("Three Dimensional");
   QHBoxLayout* threeDimGroupLayout = new QHBoxLayout(threeDimGroupBox);
   threeDimGroupLayout->addWidget(brushThicknessLabel);
   threeDimGroupLayout->addWidget(threeDimSizeComboBox);
   
   QWidget* toggleVoxelsWidget = new QWidget;
   QVBoxLayout* toggleVoxelsLayout = new QVBoxLayout(toggleVoxelsWidget);
   toggleVoxelsLayout->addWidget(currentSliceGroupBox);
   toggleVoxelsLayout->addWidget(threeDimGroupBox);
   toggleVoxelsLayout->setAlignment(currentSliceGroupBox, Qt::AlignLeft);
   toggleVoxelsLayout->setAlignment(threeDimGroupBox, Qt::AlignLeft);

   return toggleVoxelsWidget;
}

/**
 * create the other volume section.
 */
QWidget*
GuiVolumeSegmentationEditorDialog::createOtherVolumeSection()
{
   otherVolumeComboBox = new GuiVolumeSelectionControl(0,
                                                       false,
                                                       false,
                                                       false,
                                                       false,
                                                       false,
                                                       true,
                                                       false,
                                                       GuiVolumeSelectionControl::LABEL_MODE_FILE_NAME,
                                                       "otherVolumeComboBox",
                                                       false,
                                                       false,
                                                       false);
   otherVolumeVBox = new QGroupBox("Other Volume");
   QVBoxLayout* otherVolumeLayout = new QVBoxLayout(otherVolumeVBox);
   otherVolumeLayout->addWidget(otherVolumeComboBox);
   return otherVolumeVBox;
}      

/**
 * called when dialog is closed.
 */
void
GuiVolumeSegmentationEditorDialog::close()
{
   clearUndoStack();
   
   //
   // If in segmentation edit mode, switch to view mode
   //
   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   if (openGL->getMouseMode() == GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_SEGMENTATION_EDIT) {
      openGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   }
   QDialog::close();
}

/**
 * called when dialog is popped up.
 */
void 
GuiVolumeSegmentationEditorDialog::show()
{
   clearUndoStack();
   
   WuQDialog::show();
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                  GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_SEGMENTATION_EDIT);
}
      
/**
 * Called when a voxel is selected for editing in the main window.
 */
void 
GuiVolumeSegmentationEditorDialog::processVoxel(const int ii, const int jj, const int kk,  
                                                const VolumeFile::VOLUME_AXIS pickAxis)
{
   unsigned char rgbsON[4], rgbsOFF[4];
   BrainModelVolumeVoxelColoring::getSegmentationVoxelOnColor(rgbsON);
   BrainModelVolumeVoxelColoring::getSegmentationVoxelOffColor(rgbsOFF);
   const float voxelValueON  = 255.0;
   const float voxelValueOFF = 0.0;
   
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      close();
      return;
   }
   
   switch (editMode) {
      case EDIT_MODE_COPY_SUBVOLUME:
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         break;
      case EDIT_MODE_DILATE:
         break;
      case EDIT_MODE_ERODE:
         break;
      case EDIT_MODE_FLOOD_FILL_2D:
         break;
      case EDIT_MODE_FLOOD_FILL_3D:
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         break;
      case EDIT_MODE_REMOVE_CONNECTED_2D:
         break;
      case EDIT_MODE_REMOVE_CONNECTED_3D:
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         break;
      case EDIT_MODE_RETAIN_CONNECTED_3D:
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         break;
      case EDIT_MODE_TURN_VOXELS_ON:
         break;
      case EDIT_MODE_TURN_VOXELS_OFF:
         break;
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
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", 
                                 "Editing of oblique slices is not allowed.");
         return;
      case VolumeFile::VOLUME_AXIS_UNKNOWN:
         return;
         break;
   }
   
   VolumeFile* vf = bmv->getSelectedVolumeSegmentationFile();
   if (vf != NULL) {
   
      //
      // Voxel value and its color
      //
      float voxelValue = 0.0;
      unsigned char rgb[4] = { 0, 0, 0, 0 };
      
      switch (editMode) {
         case EDIT_MODE_COPY_SUBVOLUME:
            break;
         case EDIT_MODE_DILATE:
         case EDIT_MODE_FLOOD_FILL_2D:
         case EDIT_MODE_FLOOD_FILL_3D:
         case EDIT_MODE_TURN_VOXELS_ON:
            voxelValue = voxelValueON;
            rgb[0] = rgbsON[0];
            rgb[1] = rgbsON[1];
            rgb[2] = rgbsON[2];
            rgb[3] = rgbsON[3];
            break;
         case EDIT_MODE_REMOVE_CONNECTED_2D:
         case EDIT_MODE_REMOVE_CONNECTED_3D:
            voxelValue = voxelValueOFF;
            rgb[0] = rgbsOFF[0];
            rgb[1] = rgbsOFF[1];
            rgb[2] = rgbsOFF[2];
            rgb[3] = rgbsOFF[3];
            break;
         case EDIT_MODE_RETAIN_CONNECTED_3D:
            break;
         case EDIT_MODE_TURN_VOXELS_OFF:
         case EDIT_MODE_ERODE:
            voxelValue = voxelValueOFF;
            rgb[0] = rgbsOFF[0];
            rgb[1] = rgbsOFF[1];
            rgb[2] = rgbsOFF[2];
            rgb[3] = rgbsOFF[3];
            break;
      }
      
      
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
         case TOGGLE_VOXELS_SIZE_ALL:
            mm = 10000;
            break;
      }
      int ijkMin[3] = { ii - mm, jj - mm, kk - mm };
      int ijkMax[3] = { ii + mm, jj + mm, kk + mm };
      
      //
      // Process three dim aspect
      //
      int threeDim = 0;
      bool threeDimFlag = false;
      switch (static_cast<TOGGLE_VOXELS_SIZE>(threeDimSizeComboBox->currentIndex())) {
         case TOGGLE_VOXELS_SIZE_1_BY_1:
            threeDim = 0;
            threeDimFlag = false;
            break;
         case TOGGLE_VOXELS_SIZE_3_BY_3:
            threeDim = 1;
            threeDimFlag = true;
            break;
         case TOGGLE_VOXELS_SIZE_5_BY_5:
            threeDim = 2;
            threeDimFlag = true;
            break;
         case TOGGLE_VOXELS_SIZE_7_BY_7:
            threeDim = 3;
            threeDimFlag = true;
            break;
         case TOGGLE_VOXELS_SIZE_9_BY_9:
            threeDim = 4;
            threeDimFlag = true;
            break;
         case TOGGLE_VOXELS_SIZE_11_BY_11:
            threeDim = 5;
            threeDimFlag = true;
            break;
         case TOGGLE_VOXELS_SIZE_ALL:
            threeDim = 1000000;
            threeDimFlag = true;
            break;
      }

      //
      // handle three dim "thickness" 
      //
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
      
      //
      // Some modes only need the starting voxel
      //
      bool singleVoxelOnly = false;
      switch (editMode) {
         case EDIT_MODE_COPY_SUBVOLUME:
            break;
         case EDIT_MODE_DILATE:
            break;
         case EDIT_MODE_ERODE:
            break;
         case EDIT_MODE_FLOOD_FILL_2D:
            singleVoxelOnly = true;
            break;
         case EDIT_MODE_FLOOD_FILL_3D:
            singleVoxelOnly = true;
            break;
         case EDIT_MODE_REMOVE_CONNECTED_2D:
            singleVoxelOnly = true;
            break;
         case EDIT_MODE_REMOVE_CONNECTED_3D:
            singleVoxelOnly = true;
            break;
         case EDIT_MODE_RETAIN_CONNECTED_3D:
            break;
         case EDIT_MODE_TURN_VOXELS_ON:
            break;
         case EDIT_MODE_TURN_VOXELS_OFF:
            break;
      }      
      if (singleVoxelOnly) {
         ijkMin[0] = ii;
         ijkMax[0] = ii;
         ijkMin[1] = jj;
         ijkMax[1] = jj;
         ijkMin[2] = kk;
         ijkMax[2] = kk;
      }
      
      VolumeModification modifiedVoxels;
      
      //
      // Perform operation on the volume
      //
      switch (editMode) {
         case EDIT_MODE_COPY_SUBVOLUME:
            {
               const VolumeFile* otherVolume = otherVolumeComboBox->getSelectedVolumeFile();
               if (otherVolume == NULL) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", 
                     "There is no \"Other Volume\" from which to copy voxels.");
               }
               else if (otherVolume == vf) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", 
                     "The \"Other Volume\" is the same as the volume being edited.");
               }
               try {
                  const int extent[6] = {
                     ijkMin[0], ijkMax[0],
                     ijkMin[1], ijkMax[1],
                     ijkMin[2], ijkMax[2]
                  };
                  vf->copySubVolume(otherVolume,
                                    extent,
                                    rgbsOFF,
                                    rgbsON,
                                    &modifiedVoxels);
               }
               catch (FileException& e) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", e.whatQString());
               }
            }
            break;
         case EDIT_MODE_DILATE:
            vf->performSegmentationOperation(VolumeFile::SEGMENTATION_OPERATION_DILATE,
                                             axis,
                                             threeDimFlag,
                                             ijkMin,
                                             ijkMax,
                                             voxelValue,
                                             rgb,
                                             &modifiedVoxels);
            break;
         case EDIT_MODE_ERODE:
            vf->performSegmentationOperation(VolumeFile::SEGMENTATION_OPERATION_ERODE,
                                             axis,
                                             threeDimFlag,
                                             ijkMin,
                                             ijkMax,
                                             voxelValue,
                                             rgb,
                                             &modifiedVoxels);
            break;
         case EDIT_MODE_FLOOD_FILL_2D:
            threeDimFlag = false;
            vf->performSegmentationOperation(VolumeFile::SEGMENTATION_OPERATION_FLOOD_FILL_2D,
                                             axis,
                                             threeDimFlag,
                                             ijkMin,
                                             ijkMax,
                                             voxelValue,
                                             rgb,
                                             &modifiedVoxels);
            break;
         case EDIT_MODE_FLOOD_FILL_3D:
            vf->performSegmentationOperation(VolumeFile::SEGMENTATION_OPERATION_FLOOD_FILL_3D,
                                             axis,
                                             threeDimFlag,
                                             ijkMin,
                                             ijkMax,
                                             voxelValue,
                                             rgb,
                                             &modifiedVoxels);
            break;
         case EDIT_MODE_REMOVE_CONNECTED_2D:
            threeDimFlag = false;
            vf->performSegmentationOperation(VolumeFile::SEGMENTATION_OPERATION_REMOVE_CONNECTED_2D,
                                             axis,
                                             threeDimFlag,
                                             ijkMin,
                                             ijkMax,
                                             voxelValue,
                                             rgb,
                                             &modifiedVoxels);
            break;
         case EDIT_MODE_REMOVE_CONNECTED_3D:
            vf->performSegmentationOperation(VolumeFile::SEGMENTATION_OPERATION_REMOVE_CONNECTED_3D,
                                             axis,
                                             threeDimFlag,
                                             ijkMin,
                                             ijkMax,
                                             voxelValue,
                                             rgb,
                                             &modifiedVoxels);
            break;
         case EDIT_MODE_RETAIN_CONNECTED_3D:
            vf->floodFillWithVTK(ijkMin,
                                 255,
                                 255,
                                 0,
                                 &modifiedVoxels);
            vf->setVoxelColoringInvalid();
            break;
         case EDIT_MODE_TURN_VOXELS_ON:
            vf->performSegmentationOperation(VolumeFile::SEGMENTATION_OPERATION_TOGGLE_ON,
                                             axis,
                                             threeDimFlag,
                                             ijkMin,
                                             ijkMax,
                                             voxelValue,
                                             rgb,
                                             &modifiedVoxels);
            break;
         case EDIT_MODE_TURN_VOXELS_OFF:
            vf->performSegmentationOperation(VolumeFile::SEGMENTATION_OPERATION_TOGGLE_OFF,
                                             axis,
                                             threeDimFlag,
                                             ijkMin,
                                             ijkMax,
                                             voxelValue,
                                             rgb,
                                             &modifiedVoxels);
            break;
      }
                                       
      if (modifiedVoxels.getNumberOfVoxels() > 0) {
         undoContainer.push_front(modifiedVoxels);
         
         //
         // Limit the size of the undo container
         //
         const int maxSize = 1024 * 1024 * 10;  // 10 meg
         while (getUndoContainerMemorySize() > maxSize) {
            if (undoContainer.size() <= 1) {
               break;
            }
            undoContainer.pop_back();
         }
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
GuiVolumeSegmentationEditorDialog::clearUndoStack()
{
   undoContainer.clear();
   enableUndoButton();
}

/**
 * Called when the undo button is pressed.
 */
void
GuiVolumeSegmentationEditorDialog::slotUndoButton()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      close();
      return;
   }
   
   VolumeFile* vf = bmv->getSelectedVolumeSegmentationFile();
   if (vf != NULL) {
      if (undoContainer.empty() == false) {
         VolumeModification volumeModification = undoContainer.front();
         undoContainer.pop_front();
         
         vf->undoModification(&volumeModification);
         getUndoContainerMemorySize();
      }
      
      GuiBrainModelOpenGL::updateAllGL();
   }
   
   enableUndoButton();
}

/**
 * Enable the undo button.
 */
void
GuiVolumeSegmentationEditorDialog::enableUndoButton()
{
   undoButton->setEnabled(undoContainer.empty() == false);
}

/**
 * Called when an edit mode combo box selection is made.
 */
void
GuiVolumeSegmentationEditorDialog::slotModeComboBox(int item)
{
   editMode = static_cast<EDIT_MODE>(item);
   
   QString helpInfo;
   
   otherVolumeComboBox->setEnabled(false);
   bool enableVoxelSize = false;
   switch (editMode) {
      case EDIT_MODE_COPY_SUBVOLUME:
         otherVolumeComboBox->setEnabled(true);
         enableVoxelSize = true;
         helpInfo = "<B>Copy Sub-Volume</B> will copy of regions from the other segmentation "
                    "volume into the currently selected segmentation volume.   To use, set "
                    "the \"other volume\" from which you want to copy voxels.  Drag the mouse "
                    "over voxels and the corresponding voxels from the \"other volume\" "
                    "will be copied to the currently displayed volume.";
         break;
      case EDIT_MODE_DILATE:
         enableVoxelSize = true;
         helpInfo = "<B>Dilate</B> will add voxels onto the edge of a region.";
         break;
      case EDIT_MODE_ERODE:
         enableVoxelSize = true;
         helpInfo = "<B>Erode</B> will remove voxels from the edge of a region.";
         break;
      case EDIT_MODE_FLOOD_FILL_2D:
         enableVoxelSize = false;
         helpInfo = "<B>Flood Fill</B> will fill a hole in a region for the current slice.";
         break;
      case EDIT_MODE_FLOOD_FILL_3D:
         enableVoxelSize = false;
         helpInfo = "<B>Flood Fill</B> will fill a hole in a region for all voxels connected to the selected voxel.";
         break;
      case EDIT_MODE_REMOVE_CONNECTED_2D:
         enableVoxelSize = false;
         helpInfo = "<B>Remove Connected Voxels</B> will remove a connected set of voxels "
                    "from the current slice only.";
         break;
      case EDIT_MODE_REMOVE_CONNECTED_3D:
         enableVoxelSize = false;
         helpInfo = "<B>Remove Connected Voxels</B> will remove a connected set of voxels.";
         break;
      case EDIT_MODE_RETAIN_CONNECTED_3D:
         enableVoxelSize = false;
         helpInfo = "<B>Retain Connected Voxels 3D</B> will turn off all voxels not "
                    "connected to the selected voxels.  After selecting this mode, "
                    "press the Apply button.  After doing so, click the mouse on a "
                    "voxel in the part of the segmentation that is to remain.  All "
                    "voxels not connected to the selected voxel will be discarded.";
         break;
      case EDIT_MODE_TURN_VOXELS_ON:
         enableVoxelSize = true;
         helpInfo = "<B>Toggle Voxels On</B> will turn on voxels around the cursor.";
         break;
      case EDIT_MODE_TURN_VOXELS_OFF:
         enableVoxelSize = true;
         helpInfo = "<B>Toggle Voxels Off</B> will turn off voxels around the cursor.";
         break;
   }
   
   //
   // Set the help information
   //
   loadHelpInformation(helpInfo);
   
   currentSliceGroupBox->setEnabled(enableVoxelSize);
   threeDimGroupBox->setEnabled(enableVoxelSize);
   
   slotApplyButton(); 
}

/**
 * get the size of the undo container.
 */
int 
GuiVolumeSegmentationEditorDialog::getUndoContainerMemorySize() const
{
   int totalSize = 0;
   
   for (std::list<VolumeModification>::const_iterator iter = undoContainer.begin();
        iter != undoContainer.end(); iter++) {
      totalSize += iter->getMemorySize();
   }
   
   //std::cout << "UNDO container size: " << totalSize;
   
   return totalSize;
}


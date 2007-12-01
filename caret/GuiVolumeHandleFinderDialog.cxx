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

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelVolumeCrossoverHandleFinder.h"
#include "BrainModelVolumeHandleFinder.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "DebugControl.h"
#include "DisplaySettingsRgbPaint.h"
#include "DisplaySettingsVolume.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiToolBar.h"
#include "GuiVolumeHandleFinderDialog.h"
#include "WuQWidgetGroup.h"
#include "RgbPaintFile.h"

#include "global_variables.h"

/**
 * constructor.
 */
GuiVolumeHandleFinderDialog::GuiVolumeHandleFinderDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setWindowTitle("Volume Handle Finder");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Search axis check boxes
   //
   searchAxisXCheckBox = new QCheckBox("X Axis (Parasagittal)");
   searchAxisYCheckBox = new QCheckBox("Y Axis (Coronal)");
   searchAxisZCheckBox = new QCheckBox("Z Axis (Horizontal)");
   QVBoxLayout* searchSegmentationVerticalLayout = new QVBoxLayout;
   searchSegmentationVerticalLayout->addWidget(searchAxisXCheckBox);
   searchSegmentationVerticalLayout->addWidget(searchAxisYCheckBox);
   searchSegmentationVerticalLayout->addWidget(searchAxisZCheckBox);
   QHBoxLayout* searchSegmentationGroupLayout = new QHBoxLayout;
   searchSegmentationGroupLayout->addWidget(new QLabel(" "));
   searchSegmentationGroupLayout->addLayout(searchSegmentationVerticalLayout);
   searchSegmentationGroupLayout->addStretch();

   //
   // Widget group for enabling/disabling search axis check boxes
   //
   searchSegmentationWidgetGroup = new WuQWidgetGroup(this);
   searchSegmentationWidgetGroup->addWidget(searchAxisXCheckBox);
   searchSegmentationWidgetGroup->addWidget(searchAxisYCheckBox);
   searchSegmentationWidgetGroup->addWidget(searchAxisZCheckBox);
   
   //
   // Search segmentation and crossovradio button
   //
   searchSegmentationRadioButton = new QRadioButton("Search Segmentation Method");
   searchSegmentationRadioButton->setToolTip("Search the segmentation volume for \n"
                                             "topological defects along the sepcified\n"
                                             "axes.  Once no more defects are found\n"
                                             "using this method, switch to the\n"
                                             "crossover search.");
   searchCrossoversRadioButton = new QRadioButton("Search Crossovers Method");
   searchCrossoversRadioButton->setToolTip("Create a spherical surface from the \n"
                                           "segmentation volume and examine the\n"
                                           "spherical surface for crossovers.  Map\n"
                                           "any crossovers back to the volume. Use\n"
                                           "this method after no defects are detected\n"
                                           "using the segmentation method.");
   
   //
   // Button group to keep mutually exclusive
   //
   QButtonGroup* searchButtonGroup = new QButtonGroup(this);
   QObject::connect(searchButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotSearchMethodButton()));
   searchButtonGroup->addButton(searchSegmentationRadioButton);
   searchButtonGroup->addButton(searchCrossoversRadioButton);
   
   //
   // Search for handles push button
   //
   QPushButton* searchPushButton = new QPushButton("Search For Handles");
   searchPushButton->setAutoDefault(false);
   searchPushButton->setFixedSize(searchPushButton->sizeHint());
   QObject::connect(searchPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFindHandlesPushButton()));
   
   //
   // Search Group Box and Layout
   //
   QGroupBox* searchGroupBox = new QGroupBox("Search");
   dialogLayout->addWidget(searchGroupBox);
   QVBoxLayout* searchGroupLayout = new QVBoxLayout(searchGroupBox);
   searchGroupLayout->addWidget(searchSegmentationRadioButton);
   searchGroupLayout->addLayout(searchSegmentationGroupLayout);
   searchGroupLayout->addWidget(searchCrossoversRadioButton);
   searchGroupLayout->addWidget(searchPushButton);
   
   //
   // add some space
   //
   //searchGroupBox->addSpace(10);
   
   //
   // Create rgb volume showing handles check box
   //
   createVolumeShowingHandlesCheckBox = new QCheckBox("Create RGB Volume Showing Handles");
   createVolumeShowingHandlesCheckBox->setChecked(true);
   
   //
   // Create rgb paint showing handles on surface check box
   //
   createRgbPaintSurfaceHandlesCheckBox = new QCheckBox("Create RGB Paint Showing Handles on Surfaces");
   bool haveSurfaces = false;
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
      if (theMainWindow->getBrainSet()->getBrainModelSurface(i) != NULL) {
         haveSurfaces = true;
         break;
      }
   }
   createRgbPaintSurfaceHandlesCheckBox->setEnabled(true);
   createRgbPaintSurfaceHandlesCheckBox->setChecked(true);
   
   //
   // Show Handles Group Box
   //
   QGroupBox* showGroupBox = new QGroupBox("Show Handles");
   dialogLayout->addWidget(showGroupBox);
   QVBoxLayout* showGroupLayout = new QVBoxLayout(showGroupBox);
   showGroupLayout->addWidget(createVolumeShowingHandlesCheckBox);
   showGroupLayout->addWidget(createRgbPaintSurfaceHandlesCheckBox);
   
   //
   // Handles list box
   //
   handlesListWidget = new QListWidget;
   QObject::connect(handlesListWidget, SIGNAL(currentRowChanged(int)),
                    this, SLOT(handleListBoxSelection(int)));
                    
   //
   // Fill selected handle button
   //
   fillHandlePushButton = new QPushButton("Fill Selected Handle");
   fillHandlePushButton->setAutoDefault(false);
   fillHandlePushButton->setFixedSize(fillHandlePushButton->sizeHint());
   QObject::connect(fillHandlePushButton, SIGNAL(clicked()),
                    this, SLOT(slotFillHandlePushButton()));
                    
   //
   // Group box for handles
   //
   QGroupBox* handlesGroupBox = new QGroupBox("Handles");
   dialogLayout->addWidget(handlesGroupBox);
   QVBoxLayout* handlesGroupLayout = new QVBoxLayout(handlesGroupBox);
   handlesGroupLayout->addWidget(handlesListWidget);
   handlesGroupLayout->addWidget(fillHandlePushButton);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Close button 
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   closeButton->setAutoDefault(false);
   closeButton->setFixedSize(closeButton->sizeHint());
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   clearHandles();
}

/**
 * destructor.
 */
GuiVolumeHandleFinderDialog::~GuiVolumeHandleFinderDialog()
{
}

/**
 * called when search method radio button clicked.
 */
void 
GuiVolumeHandleFinderDialog::slotSearchMethodButton()
{
   fillHandlePushButton->setEnabled(searchSegmentationRadioButton->isChecked());
   searchSegmentationWidgetGroup->setEnabled(searchSegmentationRadioButton->isChecked());
}
      
/**
 * called to display the dialog.
 */
void 
GuiVolumeHandleFinderDialog::show()
{
   clearHandles();
   QtDialog::show();
}

/**
 * called to close the dialog.
 */
void 
GuiVolumeHandleFinderDialog::close()
{
   clearHandles();
   QDialog::close();
}

/**
 * Called when go to handle push button pressed.
 */
void
GuiVolumeHandleFinderDialog::handleListBoxSelection(int item)
{
   selectedHandleIndex = -1;
   
   BrainSet* bs = theMainWindow->getBrainSet();
   
   if ((item >= 0) && (item < static_cast<int>(handles.size()))) {
      BrainModelVolume* bmv = bs->getBrainModelVolume();
      if (bmv != NULL) {
         selectedHandleIndex = item;
         BrainModelVolumeTopologicalError& handle = handles[item];
         
         float xyz[3];
         int ijk[3], slices[2], numVoxels;
         VolumeFile::VOLUME_AXIS axis;
         handle.getInfo(ijk, xyz, slices, numVoxels, axis);
         
         const VolumeFile* vf = bmv->getMasterVolumeFile();
         if (vf != NULL) {
            vf->convertCoordinatesToVoxelIJK(xyz, ijk);
            //
            // Highlight in all windows
            //
            for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
               bmv->setSelectedOrthogonalSlices(i, ijk);
               
               if ((axis == VolumeFile::VOLUME_AXIS_X) ||
                   (axis == VolumeFile::VOLUME_AXIS_Y) ||
                   (axis == VolumeFile::VOLUME_AXIS_Z)) {
                  bmv->setSelectedAxis(i, axis);
               }
            }
            
            //
            // Highlight nearby node in surfaces
            //
            BrainModelSurface* bms = bs->getActiveFiducialSurface();
            if (bms != NULL) {
               const CoordinateFile* cf = bms->getCoordinateFile();
               const int nodeNum = cf->getCoordinateIndexClosestToPoint(xyz[0], xyz[1], xyz[2]);
               if (nodeNum >= 0) {
                  bs->setDisplayCrossForNode(nodeNum, NULL);
                  BrainSetNodeAttribute* bna = bs->getNodeAttributes(nodeNum);
                  bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
               }
            }
            
            GuiToolBar::updateAllToolBars(false);
            GuiBrainModelOpenGL::updateAllGL();
         }
      }
   }
}

/**
 * called when fill selected handle push button is pressed.
 */
void 
GuiVolumeHandleFinderDialog::slotFillHandlePushButton()
{
   if ((selectedHandleIndex >= 0) && (selectedHandleIndex < static_cast<int>(handles.size()))) {
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         //
         // Get the segmentation volume
         //
         VolumeFile* segmentVolume = bmv->getSelectedVolumeSegmentationFile();
         
         //
         // Get the voxels that make up the handle
         //
         std::vector<int> handleVoxels;
         handles[selectedHandleIndex].getHandleVoxels(handleVoxels);
         
         if (handleVoxels.size() > 0) {
            //
            // Set the voxels
            //
            segmentVolume->setVoxel(handleVoxels, 255.0);
            
            //
            // Assign voxel colors and redraw
            //
            //BrainModelVolumeVoxelColoring* voxelColoring = theMainWindow->getBrainSet()->getVoxelColoring();
            segmentVolume->setVoxelColoringInvalid();
            GuiBrainModelOpenGL::updateAllGL();
         }
      }
   }
}      

/**
 * Clear any handles currently in dialog.
 */
void
GuiVolumeHandleFinderDialog::clearHandles()
{
   selectedHandleIndex = -1;
   handles.clear();
   handlesListWidget->clear();
}

/**
 * load handles into list box.
 */
void 
GuiVolumeHandleFinderDialog::loadHandlesIntoListBox()
{
   const int numHandles = static_cast<int>(handles.size());
   for (int i = 0; i < numHandles; i++) {
      const BrainModelVolumeTopologicalError& handle = handles[i];
      float xyz[3];
      int ijk[3], slices[2], numVoxels;
      VolumeFile::VOLUME_AXIS axis;
      
      handle.getInfo(ijk, xyz, slices, numVoxels, axis);
      
      char axisChar = ' ';
      switch (axis) {
         case VolumeFile::VOLUME_AXIS_X:
            axisChar = 'X';
            break;
         case VolumeFile::VOLUME_AXIS_Y:
            axisChar = 'Y';
            break;
         case VolumeFile::VOLUME_AXIS_Z:
            axisChar = 'Z';
            break;
         case VolumeFile::VOLUME_AXIS_ALL:
            axisChar = 'A';
            break;
         case VolumeFile::VOLUME_AXIS_OBLIQUE:
            axisChar = 'O';
            break;
         case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
            axisChar = 'O';
            break;
         case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
            axisChar = 'O';
            break;
         case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
            axisChar = 'O';
            break;
         case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
            axisChar = 'O';
            break;
         case VolumeFile::VOLUME_AXIS_UNKNOWN:
            axisChar = 'U';
            break;
      }
      
      QString str =
         // + "voxel ("
         // + QString::number(ijk[0]) + ","
         // + QString::number(ijk[1]) + ","
         // + QString::number(ijk[2]) + ") "
          + "xyz ("
          + QString::number(xyz[0], 'f', 1) + ","
          + QString::number(xyz[1], 'f', 1) + ","
          + QString::number(xyz[2], 'f', 1) + ") ";
      if (axisChar != 'U') {
         str += axisChar;
      }
      if (slices[0] >= 0) {
         str += " slices ("
                + QString::number(slices[0]) 
                + ", "
                + QString::number(slices[1])
                + ")";
      }
      str += " involves "
             + QString::number(numVoxels)
             + " voxels.";
          
      if (DebugControl::getDebugOn()) {
         std::cout << str.toAscii().constData() << std::endl;
      }
      handlesListWidget->blockSignals(true);
      handlesListWidget->addItem(str);
      handlesListWidget->blockSignals(false);
   }
}
      
/**
 * Called when the find handles pushbutton is pressed.
 */
void
GuiVolumeHandleFinderDialog::slotFindHandlesPushButton()
{
   clearHandles();
   
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume(-1);
   if (bmv == NULL) {
      return;
   }
   VolumeFile* vf = bmv->getSelectedVolumeSegmentationFile();
   
   if (vf != NULL) {
      if (searchSegmentationRadioButton->isChecked()) {
         if ((searchAxisXCheckBox->isChecked() == false) &&
             (searchAxisYCheckBox->isChecked() == false) &&
             (searchAxisZCheckBox->isChecked() == false)) {
            QMessageBox::critical(this, "ERROR", "You must choose at least one axis to search.");
            return;
         }
      }
      else if (searchCrossoversRadioButton->isChecked() == false) {
         QMessageBox::critical(this, "ERROR", "You must select a search method.");
         return;
      }
       
      bool doRgbPaintSurfaceHandles = false;
      if (createRgbPaintSurfaceHandlesCheckBox->isEnabled()) {
         doRgbPaintSurfaceHandles = createRgbPaintSurfaceHandlesCheckBox->isChecked();
      }
      
      bool doRgbVolumeHandles = createVolumeShowingHandlesCheckBox->isChecked();
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      if (searchCrossoversRadioButton->isChecked()) {
         BrainModelVolumeCrossoverHandleFinder bmvchf(theMainWindow->getBrainSet(),
                                                      vf,
                                                      doRgbVolumeHandles,
                                                      doRgbPaintSurfaceHandles);
         try {
            bmvchf.execute();
            const int numHandles = bmvchf.getNumberOfHandles();
            if (numHandles == 0) {
               QApplication::restoreOverrideCursor();
               QMessageBox::information(this, "Information", "No handles found.");
            }
            else {
               for (int i = 0; i < numHandles; i++) {
                  const BrainModelVolumeTopologicalError* handle = bmvchf.getHandleInfo(i);
                  handles.push_back(*handle);
               }
               loadHandlesIntoListBox();
               
               GuiFilesModified fm;
               fm.setVolumeModified();
               fm.setRgbPaintModified();
               theMainWindow->fileModificationUpdate(fm);
               
               GuiBrainModelOpenGL::updateAllGL();
               QApplication::restoreOverrideCursor();
            }
         }
         catch (BrainModelAlgorithmException& e) {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "ERROR", e.whatQString());
         }
      }
      else {
         BrainModelVolumeHandleFinder bmvhf(theMainWindow->getBrainSet(), 
                                            vf, 
                                            doRgbVolumeHandles,
                                            searchAxisXCheckBox->isChecked(),
                                            searchAxisYCheckBox->isChecked(),
                                            searchAxisZCheckBox->isChecked(),
                                            doRgbPaintSurfaceHandles);
         try {
            bmvhf.execute();
            GuiFilesModified fm;
            fm.setVolumeModified();
            fm.setRgbPaintModified();
            theMainWindow->fileModificationUpdate(fm);
            
            GuiBrainModelOpenGL::updateAllGL();
            QApplication::restoreOverrideCursor();
            
            const int numHandles = bmvhf.getNumberOfHandles();
            if (numHandles == 0) {
               QApplication::restoreOverrideCursor();
               QMessageBox::information(this, "Information", "No handles found.");
            }
            else {
               for (int i = 0; i < numHandles; i++) {
                  const BrainModelVolumeTopologicalError* handle = bmvhf.getHandleInfo(i);
                  handles.push_back(*handle);
               }
               loadHandlesIntoListBox();
/*
               std::ostringstream str;
               str << numHandles << " handles were found.";
               theMainWindow->speakText(str.str().c_str(), false);
               QString s(str.str().c_str());
               s.append("\n\n");
               for (int i = 0; i < numHandles; i++) {
                  const BrainModelVolumeTopologicalError* handle = bmvhf.getHandleInfo(i);
                  handles.push_back(*handle);
                  int ijk[3], slices[2], numVoxels;
                  VolumeFile::VOLUME_AXIS axis;
                  handle->getInfo(ijk, slices, numVoxels, axis);
                  
                  char axisChar = ' ';
                  switch (axis) {
                     case VolumeFile::VOLUME_AXIS_X:
                        axisChar = 'X';
                        break;
                     case VolumeFile::VOLUME_AXIS_Y:
                        axisChar = 'Y';
                        break;
                     case VolumeFile::VOLUME_AXIS_Z:
                        axisChar = 'Z';
                        break;
                     case VolumeFile::VOLUME_AXIS_ALL:
                        axisChar = 'A';
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE:
                        axisChar = 'O';
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
                        axisChar = 'O';
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                        axisChar = 'O';
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                        axisChar = 'O';
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                        axisChar = 'O';
                        break;
                     case VolumeFile::VOLUME_AXIS_UNKNOWN:
                        axisChar = 'U';
                        break;
                  }
                  
                  std::ostringstream str;
                  str << "voxel ("
                     << ijk[0] << ", "
                     << ijk[1] << ", "
                     << ijk[2] << ") "
                     << axisChar
                     << " slices ("
                     << slices[0] 
                     << ", "
                     << slices[1]
                     << ") involves "
                     << numVoxels
                     << " voxels.";
                  if (DebugControl::getDebugOn()) {
                     std::cout << str.str().c_str() << std::endl;
                  }
                  handlesListWidget->blockSignals(true);
                  handlesListWidget->addItem(str.str().c_str());
                  handlesListWidget->blockSignals(false);
               }
*/
            }
         }
         catch (BrainModelAlgorithmException& e) {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "ERROR", e.whatQString());
         }
               
      }
      
      if (doRgbVolumeHandles) {
         //
         // Make newest RGB volume to show the handles
         //
         DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
         dsv->setSelectedRgbVolume(theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles() - 1);
         
         //
         // Set primary overlay to Segmentation, secondary overlay to RGB Volume,
         // and underlay to Anatomy
         //
         BrainModelVolumeVoxelColoring* voxelColoring = theMainWindow->getBrainSet()->getVoxelColoring();
         voxelColoring->setPrimaryOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION);
         voxelColoring->setSecondaryOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB);
         voxelColoring->setUnderlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY);
      }

      if (doRgbPaintSurfaceHandles) {
         //
         // Make RGB paint the newest RGB Paint
         //
         DisplaySettingsRgbPaint* dsrp = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();
         RgbPaintFile* rgbPaint = theMainWindow->getBrainSet()->getRgbPaintFile();
         dsrp->setSelectedColumn(-1, rgbPaint->getNumberOfColumns() - 1);
         
         //
         // Set primary overlay to RGB Paint and Underlay to shape
         //
         BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
         bsnc->setPrimaryOverlay(-1, BrainModelSurfaceNodeColoring::OVERLAY_RGB_PAINT);
         bsnc->setUnderlay(-1, BrainModelSurfaceNodeColoring::OVERLAY_SURFACE_SHAPE);
         bsnc->assignColors();
      }
               
      GuiBrainModelOpenGL::updateAllGL();
      GuiToolBar::updateAllToolBars(false);
      theMainWindow->updateDisplayControlDialog();
   }
}


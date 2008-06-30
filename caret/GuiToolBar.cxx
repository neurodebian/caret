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

#include <QAction>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QToolButton>
#include <QToolTip>

#include "BrainModelContours.h"
#include "BrainModelSurfaceAndVolume.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsCuts.h"
#include "DisplaySettingsSurface.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainSetAndModelSelectionControl.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowLayersActions.h"
#include "GuiMainWindowVolumeActions.h"
#include "GuiMouseModePopupMenu.h"
#include "GuiToolBarActions.h"

#define __GUI_TOOLBAR_MAIN__
#include "GuiToolBar.h"
#undef  __GUI_TOOLBAR_MAIN__

#include "FileUtilities.h"
#include "global_variables.h"
#include "StringUtilities.h"

/**
 * Constructor.
 * Parent is the window that this toolbar is being added to.
 * mainWindowIn is Caret's main window.
 * mainWindowFlag is set if this tool bar is being added to Caret's main window.
 */
GuiToolBar::GuiToolBar(QMainWindow* parent, GuiMainWindow* mainWindowIn,
                       GuiBrainModelOpenGL* brainModelOpenGLIn,
                       const BrainModel::BRAIN_MODEL_VIEW_NUMBER viewWindowNumberIn)
   : QToolBar(parent)
{
   setMovable(false);
   
   mainWindow = mainWindowIn;
   brainModelOpenGL = brainModelOpenGLIn;
   viewWindowNumber = viewWindowNumberIn;
   const bool mainWindowFlag =
      (viewWindowNumber == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW);
      
   //
   // actions for toolbar
   //
   GuiToolBarActions* toolBarActions = new GuiToolBarActions(brainModelOpenGL, this);
   
   //
   // Combo box for model files
   //
   modelFileComboBox = new GuiBrainSetAndModelSelectionControl;
   modelFileComboBox->addItem("             ");
//   if (mainWindowFlag) {
//      //setStretchableWidget(modelFileComboBox);
//   }
//   else {
//      modelFileComboBox->setFixedSize(modelFileComboBox->sizeHint());
//   }
   modelFileComboBox->setEditable(false);
   modelFileComboBox->setToolTip(
                   "This control is used to select the\n"
                   "displayed contour, surface, or\n"
                   "volume.");
   QObject::connect(modelFileComboBox, SIGNAL(activated(int)),
                    this, SLOT(setModelSelection(int)));
   QObject::connect(modelFileComboBox, SIGNAL(activated(int)),
                    this, SIGNAL(modelSelection(int)));
   //modelFileComboBox->updateControl();
   brainModelOpenGL->setModelSelectionControl(modelFileComboBox);
   
   QToolButton* viewButton = NULL;
   if (mainWindowFlag) {
      //
      // View mode tool button
      //
      viewButton = new QToolButton;
      viewButton->setText("View");
      QObject::connect(viewButton, SIGNAL(clicked()),
                       this, SLOT(slotViewModeButtonClicked()));
      //viewButton->setDefaultAction(toolBarActions->getViewModeAction());
      viewButton->setMenu(new GuiMouseModePopupMenu(brainModelOpenGL,
                                                    false,
                                                    viewButton));
      viewButton->setPopupMode(QToolButton::MenuButtonPopup);
   }
                   
   //
   // Standard Medial View Button
   //
   medialViewToolButton = new QToolButton;
   medialViewToolButton->setDefaultAction(toolBarActions->getMedialViewAction());
   
   //
   // Standard View Lateral Button
   //
   lateralViewToolButton = new QToolButton;
   lateralViewToolButton->setDefaultAction(toolBarActions->getLateralViewAction());
   
   //
   // Standard View Anterior Button
   //
   anteriorViewToolButton = new QToolButton;
   anteriorViewToolButton->setDefaultAction(toolBarActions->getAnteriorViewAction());
   
   //
   // Standard View Posterior Button
   //
   posteriorViewToolButton = new QToolButton;
   posteriorViewToolButton->setDefaultAction(toolBarActions->getPosteriorViewAction());
   
   //
   // Standard View Dorsal Button
   //
   dorsalViewToolButton = new QToolButton;
   dorsalViewToolButton->setDefaultAction(toolBarActions->getDorsalViewAction());
   
   //
   // Standard View Ventral Button
   //
   ventralViewToolButton = new QToolButton;
   ventralViewToolButton->setDefaultAction(toolBarActions->getVentralViewAction());
   
   //
   // Standard View Reset Button
   //
   resetViewToolButton = new QToolButton;
   resetViewToolButton->setDefaultAction(toolBarActions->getResetViewAction());
   
   //
   // X Rotation 90 Button
   //
   xRotation90ToolButton = new QToolButton;
   xRotation90ToolButton->setDefaultAction(toolBarActions->getXRotation90Action());
   
   //
   // Y Rotation 90 Button
   //
   yRotation90ToolButton = new QToolButton;
   yRotation90ToolButton->setDefaultAction(toolBarActions->getYRotation90Action());
   
   //
   // Z Rotation 90 Button
   //
   zRotation90ToolButton = new QToolButton;
   zRotation90ToolButton->setDefaultAction(toolBarActions->getZRotation90Action());
   
   //
   // Combo box for rotation axis
   //
   rotationAxisComboBox = new QComboBox;
   rotationAxisComboBox->insertItem(GuiBrainModelOpenGL::BRAIN_MODEL_ROTATION_AXIS_X, "X");
   rotationAxisComboBox->insertItem(GuiBrainModelOpenGL::BRAIN_MODEL_ROTATION_AXIS_Y, "Y");
   rotationAxisComboBox->insertItem(GuiBrainModelOpenGL::BRAIN_MODEL_ROTATION_AXIS_Z, "Z");
   rotationAxisComboBox->insertItem(GuiBrainModelOpenGL::BRAIN_MODEL_ROTATION_AXIS_XY, "XY");
   rotationAxisComboBox->insertItem(GuiBrainModelOpenGL::BRAIN_MODEL_ROTATION_AXIS_OFF, "OFF");
   QSize rotationSize = rotationAxisComboBox->sizeHint();
   rotationSize.setWidth(55);
   rotationAxisComboBox->setFixedSize(rotationSize);
   QObject::connect(rotationAxisComboBox, SIGNAL(activated(int)),
                    this, SIGNAL(rotationAxisSelection(int)));
   QObject::connect(rotationAxisComboBox, SIGNAL(activated(int)),
                    this, SLOT(setRotationSelection(int)));
   rotationAxisComboBox->setToolTip("This control is used to\n"
                                         "select the axis that the\n"
                                         "surface rotates around.");
   rotationAxisComboBox->setCurrentIndex(static_cast<int>(brainModelOpenGL->getRotationAxis()));
   
   //
   // Stereotaxic coordinates action and button
   //
   volumeStereotaxicCoordinatesToolButton = new QToolButton;
   volumeStereotaxicCoordinatesToolButton->setText("S");
   volumeStereotaxicCoordinatesToolButton->setCheckable(true);
   volumeStereotaxicCoordinatesToolButton->setToolTip("For orthogonal views only.  When on, the\n"
                                                      "volume slice controls show stereotaxic\n"
                                                      "stereotaxic coordinates.  When off, the \n"
                                                      "volume slice controls show slice indices.\n");
   QObject::connect(volumeStereotaxicCoordinatesToolButton, SIGNAL(toggled(bool)),
                     this, SLOT(volumeStereotaxicCoordinatesSlot(bool)));
   
   //
   // Combo box for volume axis
   //
   volumeAxisComboBox = new QComboBox;
   volumeAxisComboBox->insertItem(VolumeFile::VOLUME_AXIS_X, "P (YZ)");
   volumeAxisComboBox->insertItem(VolumeFile::VOLUME_AXIS_Y, "C (XZ)");
   volumeAxisComboBox->insertItem(VolumeFile::VOLUME_AXIS_Z, "H (XY)");
   volumeAxisComboBox->insertItem(VolumeFile::VOLUME_AXIS_ALL, "All");
   volumeAxisComboBox->insertItem(VolumeFile::VOLUME_AXIS_OBLIQUE, "Oblique");
   volumeAxisComboBox->insertItem(VolumeFile::VOLUME_AXIS_OBLIQUE_X, "X-Oblique");
   volumeAxisComboBox->insertItem(VolumeFile::VOLUME_AXIS_OBLIQUE_Y, "Y-Oblique");
   volumeAxisComboBox->insertItem(VolumeFile::VOLUME_AXIS_OBLIQUE_Z, "Z-Oblique");
   volumeAxisComboBox->insertItem(VolumeFile::VOLUME_AXIS_OBLIQUE_ALL, "All-Oblique");
   QSize axisSize = volumeAxisComboBox->sizeHint();
   axisSize.setWidth(70);
   volumeAxisComboBox->setFixedSize(axisSize);
   QObject::connect(volumeAxisComboBox, SIGNAL(activated(int)),
                    this, SLOT(volumeAxisChanged(int)));
   volumeAxisComboBox->setToolTip("This control chooses the axis\n"
                                       "for viewing the volume\n"
                                     "   Coronal\n"
                                     "   Parasagittal\n"
                                     "   Horizontal\n"
                                     "   All 3 Slices\n"
                                     "   Oblique Rotatable Slice\n"
                                     "   Oblique \"X\" Slice\n"
                                     "   Oblique \"Y\" Slice\n"
                                     "   Oblique \"Z\" Slice\n"
                                     "   Oblique ALL Slices\n"
                                     );
   
   //
   // Volume Slice X Spin Box
   //
   volumeSpinBoxSliceX = new QDoubleSpinBox;
   QObject::connect(volumeSpinBoxSliceX, SIGNAL(valueChanged(double)),
                    this, SLOT(volumeSliceXChanged(double)));
   volumeSpinBoxSliceX->setToolTip("This control is used to\n"
                                        "select the parasagittal slice."); 
                                        
   //
   // Volume Slice Y Spin Box
   //
   volumeSpinBoxSliceY = new QDoubleSpinBox;
   QObject::connect(volumeSpinBoxSliceY, SIGNAL(valueChanged(double)),
                    this, SLOT(volumeSliceYChanged(double)));
   volumeSpinBoxSliceY->setToolTip("This control is used to\n"
                                        "select the coronal slice."); 
                                        
   //
   // Volume Slice Z Spin Box
   //
   volumeSpinBoxSliceZ = new QDoubleSpinBox;
   QObject::connect(volumeSpinBoxSliceZ, SIGNAL(valueChanged(double)),
                    this, SLOT(volumeSliceZChanged(double)));
   volumeSpinBoxSliceZ->setToolTip("This control is used to\n"
                                        "select the horizontal slice."); 
                                        
   QToolButton* dcButton = NULL;
   QToolButton* specButton = NULL;
   yokeButton = NULL;
   QToolButton* swapButton = NULL;
   volumeUnderlayOnlyButton = NULL;
   if (mainWindowFlag) {
      //
      // Display Control toolbar button
      //
      dcButton = new QToolButton;
      dcButton->setDefaultAction(toolBarActions->getDisplayControlDialogAction());
                     
      //
      // Fast Open Data file dialog
      //
      specButton = new QToolButton;
      specButton->setDefaultAction(toolBarActions->getSpecDialogAction());
   }
   else {
      //
      // Underlay only button
      //
      volumeUnderlayOnlyButton = new QToolButton;
      volumeUnderlayOnlyButton->setDefaultAction(toolBarActions->getUnderlayVolumeOnlyAction());
      
      //
      // Yoke toolbar button
      //
      yokeButton = new QToolButton;
      yokeButton->setDefaultAction(toolBarActions->getYokeAction());
      
      //
      // Swap toolbar button
      //
      swapButton = new QToolButton;
      swapButton->setDefaultAction(toolBarActions->getSwapAction());
   }

   QHBoxLayout* l1 = new QHBoxLayout;
   if (viewButton != NULL) {
      l1->addWidget(viewButton, 0);
   }
   l1->addWidget(medialViewToolButton, 0);
   l1->addWidget(lateralViewToolButton, 0);
   l1->addWidget(anteriorViewToolButton, 0);
   l1->addWidget(posteriorViewToolButton, 0);
   l1->addWidget(dorsalViewToolButton, 0);
   l1->addWidget(ventralViewToolButton, 0);
   l1->addWidget(resetViewToolButton, 0);
   l1->addWidget(xRotation90ToolButton, 0);
   l1->addWidget(yRotation90ToolButton, 0);
   l1->addWidget(zRotation90ToolButton, 0);
   l1->addWidget(rotationAxisComboBox, 0);
   l1->addWidget(volumeStereotaxicCoordinatesToolButton, 0);
   l1->addWidget(volumeAxisComboBox, 0);
   l1->addWidget(volumeSpinBoxSliceX, 0);
   l1->addWidget(volumeSpinBoxSliceY);
   l1->addWidget(volumeSpinBoxSliceZ, 0);
   if (dcButton != NULL) {
      l1->addWidget(dcButton, 0);
   }
   if (specButton != NULL) {
      l1->addWidget(specButton, 0);
   }
   if (volumeUnderlayOnlyButton != NULL) {
      l1->addWidget(volumeUnderlayOnlyButton, 0);
   }
   if (yokeButton != NULL) {
      l1->addWidget(yokeButton, 0);
   }
   if (swapButton != NULL) {
      l1->addWidget(swapButton, 0);
   }
   l1->addStretch();  //1000);
   
   //
   // Layout for second and third rows (mode and mouse)
   //
   QGridLayout* modeMouseLayout = new QGridLayout;
   modeMouseLayout->setColumnStretch(0, 0);
   modeMouseLayout->setColumnStretch(1, 1000);
   
   //
   // Model selection control
   //
   QLabel* modelLabel = new QLabel("Model ");
   modeMouseLayout->addWidget(modelLabel, 0, 0);
   modeMouseLayout->addWidget(modelFileComboBox, 0, 1);
   
   //
   // Mouse mode combo box
   //
   mouseModeComboBox = NULL;
   const bool showMouseModeComboBoxFlag = false;  //mainWindowFlag;
   if (showMouseModeComboBoxFlag) {
      QLabel* mouseModeLabel = new QLabel("Mouse ");
      mouseModeComboBox = new QComboBox;
      QObject::connect(mouseModeComboBox, SIGNAL(activated(int)),
                       this, SLOT(slotMouseModeComboBoxActivated(int)));
      modeMouseLayout->addWidget(mouseModeLabel, 1, 0);
      modeMouseLayout->addWidget(mouseModeComboBox, 1, 1);
   }
   
   QWidget* vb = new QWidget;
   QVBoxLayout* vl = new QVBoxLayout(vb);
   vl->addLayout(l1);
   vl->addLayout(modeMouseLayout);
   
   addWidget(vb);
   
   //
   // Keep track of all toolbars that get created
   //
   bool found = false;
   for (unsigned int i = 0; i < allToolBars.size(); i++) {
      if (allToolBars[i] == NULL) {
         allToolBars[i] = this;
         found = true;
         break;
      }
   }
   if (found == false) {
      allToolBars.push_back(this);
   }
   
//   if (mainWindowFlag) {
      loadModelComboBox();
      updateViewControls();
//   }

   setMinimumWidth(512);
#if (QT_VERSION == QT_VERSION_CHECK(4,3,0))
 //  setMaximumWidth(512);   // QT 4.3 bug make toolbar way too wide
#endif
}

/**
 * Destructor.
 */
GuiToolBar::~GuiToolBar()
{
   for (unsigned int i = 0; i < allToolBars.size(); i++) {
      if (allToolBars[i] == this) {
         allToolBars[i] = NULL;
         break;
      }
   }
}

/**
 * view mode button clicked.
 */
void 
GuiToolBar::slotViewModeButtonClicked()
{
   brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
}

/**
 * Update toolbar
 */
void
GuiToolBar::updateViewControls()
{
   bool showContourControls = false;
   bool showSurfaceControls = false;
   bool showVolumeControls  = false;

   //
   // Model number selected
   //
   const int modelNum = modelFileComboBox->currentIndex();

   //
   // Loop through brain sets
   //
   if (theMainWindow != NULL) {
      int modelComboBoxOffset = 0;
      const int numBrainSets = theMainWindow->getNumberOfBrainSets();
      for (int i = 0; i < numBrainSets; i++) {
         //
         // Get a brain set and its number of models
         //
         BrainSet* bs = theMainWindow->getBrainSetByIndex(i);
         const int brainSetNumberOfModels = bs->getNumberOfBrainModels();
         
         //
         // See if model combo box is using this brain set's model
         //
         const int firstBrainModelIndex = modelComboBoxOffset;
         const int lastBrainModelIndex  = modelComboBoxOffset + brainSetNumberOfModels;
         if ((modelNum >= firstBrainModelIndex) &&
             (modelNum < lastBrainModelIndex)) {
            //
            // Update controls based upon type of brain model 
            //
            BrainModel* bm = bs->getBrainModel(modelNum - modelComboBoxOffset);
            if (bm != NULL) {
               switch (bm->getModelType()) {
                  case BrainModel::BRAIN_MODEL_CONTOURS:
                     showContourControls = true;
                     break;
                  case BrainModel::BRAIN_MODEL_SURFACE:
                     showSurfaceControls = true;
                     break;
                  case BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME:
                     showSurfaceControls = true;
                     break;
                  case BrainModel::BRAIN_MODEL_VOLUME:
                     showVolumeControls = true;
                     break;
               }
            }
            
            //
            // Jump out of loop
            //
            break;
         }
         
         //
         // model combo box offset for next brain set
         //
         modelComboBoxOffset += brainSetNumberOfModels;
      }
   }
   
   //
   // If no controls enabled, show surface controls
   //
   if ((showContourControls == false) &&
       (showSurfaceControls == false) &&
       (showVolumeControls  == false)) {
      showSurfaceControls = true;
   }
   
/*
   if (theMainWindow != NULL) {
      const int numModels = theMainWindow->getBrainSet(brainModelOpenGL->getModelViewNumber())->getNumberOfBrainModels();
      if ((modelNum >= 0) &&
          (modelNum < numModels) &&
          (numModels > 0)) {
         BrainModel* bm = theMainWindow->getBrainSet(brainModelOpenGL->getModelViewNumber())->getBrainModel(modelNum);
         if (bm != NULL) {
            switch (bm->getModelType()) {
               case BrainModel::BRAIN_MODEL_CONTOURS:
                  showContourControls = true;
                  break;
               case BrainModel::BRAIN_MODEL_SURFACE:
                  showSurfaceControls = true;
                  break;
               case BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME:
                  showSurfaceControls = true;
                  break;
               case BrainModel::BRAIN_MODEL_VOLUME:
                  showVolumeControls = true;
                  break;
            }
         }
         else {
            showSurfaceControls = true;
         }
      }
      else {
         showSurfaceControls = true;
      }
   }
*/
   
   //
   // Show surface controls
   //
   medialViewToolButton->setHidden(! showSurfaceControls);
   lateralViewToolButton->setHidden(! showSurfaceControls);
   anteriorViewToolButton->setHidden(! showSurfaceControls);
   posteriorViewToolButton->setHidden(! showSurfaceControls);
   dorsalViewToolButton->setHidden(! showSurfaceControls);
   ventralViewToolButton->setHidden(! showSurfaceControls);
   xRotation90ToolButton->setHidden(! showSurfaceControls);
   yRotation90ToolButton->setHidden(! showSurfaceControls);
   zRotation90ToolButton->setHidden(! showSurfaceControls);
   rotationAxisComboBox->setHidden(! showSurfaceControls);
   rotationAxisComboBox->blockSignals(true);
   rotationAxisComboBox->setCurrentIndex(brainModelOpenGL->getRotationAxis());
   rotationAxisComboBox->blockSignals(false);
   
   //
   // Show volume controls
   //
   volumeSpinBoxSliceX->setHidden(! showVolumeControls);
   volumeSpinBoxSliceY->setHidden(! showVolumeControls);
   volumeSpinBoxSliceZ->setHidden(! showVolumeControls);
   volumeAxisComboBox->setHidden(! showVolumeControls);
   volumeStereotaxicCoordinatesToolButton->setHidden( ! showVolumeControls);
   if (volumeUnderlayOnlyButton != NULL) {
      volumeUnderlayOnlyButton->setHidden(! showVolumeControls);
   }
   
   if (showVolumeControls) {
      //
      // Block signals from slice controls
      //
      volumeSpinBoxSliceX->blockSignals(true);
      volumeSpinBoxSliceY->blockSignals(true);
      volumeSpinBoxSliceZ->blockSignals(true);
      volumeStereotaxicCoordinatesToolButton->blockSignals(true);
   
      BrainModelVolume* bmv = brainModelOpenGL->getDisplayedBrainModelVolume();
      if (bmv != NULL) {
         const int viewNumber = brainModelOpenGL->getModelViewNumber();
         
         volumeAxisComboBox->blockSignals(true);
         volumeAxisComboBox->setCurrentIndex(
            static_cast<int>(bmv->getSelectedAxis(viewNumber)));
         volumeAxisComboBox->blockSignals(false);
         
         volumeStereotaxicCoordinatesToolButton->setChecked(bmv->getViewStereotaxicCoordinatesFlag(viewNumber));
         
         if (volumeUnderlayOnlyButton != NULL) {
            volumeUnderlayOnlyButton->blockSignals(true);
            volumeUnderlayOnlyButton->setChecked(bmv->getShowUnderlayOnlyInWindow(viewNumber));
            volumeUnderlayOnlyButton->blockSignals(false);
         }
         
         bool slicesViewFlag = false;
         
         switch (bmv->getSelectedAxis(viewNumber)) {
            case VolumeFile::VOLUME_AXIS_X:
            case VolumeFile::VOLUME_AXIS_Y:
            case VolumeFile::VOLUME_AXIS_Z:
            case VolumeFile::VOLUME_AXIS_ALL:
               {
                  VolumeFile* vf = bmv->getMasterVolumeFile();
                  if (vf != NULL) {
                     int dim[3] = { 0, 0, 0 };
                     vf->getDimensions(dim);
                     int slices[3];
                     bmv->getSelectedOrthogonalSlices(viewNumber, slices);
                     
                     if (bmv->getViewStereotaxicCoordinatesFlag(viewNumber)) {
                        float minXYZ[3], maxXYZ[3];
                        vf->getVoxelCoordinate(0, 0, 0, true, minXYZ);
                        vf->getVoxelCoordinate(dim, true, maxXYZ);
                        
                        volumeSpinBoxSliceX->setMinimum(minXYZ[0]);
                        volumeSpinBoxSliceX->setMaximum(maxXYZ[1]);
                        volumeSpinBoxSliceX->setSingleStep(1);
                        volumeSpinBoxSliceX->setDecimals(1);
                        volumeSpinBoxSliceY->setMinimum(minXYZ[1]);
                        volumeSpinBoxSliceY->setMaximum(maxXYZ[1]);
                        volumeSpinBoxSliceY->setSingleStep(1);
                        volumeSpinBoxSliceY->setDecimals(1);
                        volumeSpinBoxSliceZ->setMinimum(minXYZ[2]);
                        volumeSpinBoxSliceZ->setMaximum(maxXYZ[2]);
                        volumeSpinBoxSliceZ->setSingleStep(1);
                        volumeSpinBoxSliceZ->setDecimals(1);
                     
                        float sliceXYZ[3];
                        vf->getVoxelCoordinate(slices, true, sliceXYZ);
                        volumeSpinBoxSliceX->setValue(sliceXYZ[0]);
                        volumeSpinBoxSliceY->setValue(sliceXYZ[1]);
                        volumeSpinBoxSliceZ->setValue(sliceXYZ[2]);
                     }
                     else {
                        volumeSpinBoxSliceX->setMinimum(0);
                        volumeSpinBoxSliceX->setMaximum(dim[0] - 1);
                        volumeSpinBoxSliceX->setSingleStep(1);
                        volumeSpinBoxSliceX->setDecimals(0);
                        volumeSpinBoxSliceY->setMinimum(0);
                        volumeSpinBoxSliceY->setMaximum(dim[1] - 1);
                        volumeSpinBoxSliceY->setSingleStep(1);
                        volumeSpinBoxSliceY->setDecimals(0);
                        volumeSpinBoxSliceZ->setMinimum(0);
                        volumeSpinBoxSliceZ->setMaximum(dim[2] - 1);
                        volumeSpinBoxSliceZ->setSingleStep(1);
                        volumeSpinBoxSliceZ->setDecimals(0);
                     
                        volumeSpinBoxSliceX->setValue(slices[0]);
                        volumeSpinBoxSliceY->setValue(slices[1]);
                        volumeSpinBoxSliceZ->setValue(slices[2]);
                     }
                  }
               }
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE:
               {
                  VolumeFile* vf = bmv->getMasterVolumeFile();
                  if (vf != NULL) {
                     int dim[3] = { 0, 0, 0 };
                     vf->getDimensions(dim);
                     volumeSpinBoxSliceX->setMinimum(0);
                     volumeSpinBoxSliceX->setMaximum(dim[0] - 1);
                     volumeSpinBoxSliceX->setSingleStep(1);
                     volumeSpinBoxSliceX->setDecimals(0);
                     volumeSpinBoxSliceY->setMinimum(0);
                     volumeSpinBoxSliceY->setMaximum(dim[1] - 1);
                     volumeSpinBoxSliceY->setSingleStep(1);
                     volumeSpinBoxSliceY->setDecimals(0);
                     volumeSpinBoxSliceZ->setMinimum(0);
                     volumeSpinBoxSliceZ->setMaximum(dim[2] - 1);
                     volumeSpinBoxSliceZ->setSingleStep(1);
                     volumeSpinBoxSliceZ->setDecimals(0);
                  
                     int slices[3];
                     bmv->getSelectedObliqueSlices(slices);
                     volumeSpinBoxSliceX->setValue(slices[0]);
                     volumeSpinBoxSliceY->setValue(slices[1]);
                     volumeSpinBoxSliceZ->setValue(slices[2]);
                   
                  }
               }
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
               {
                  const int bigNum = 1000000;
                  volumeSpinBoxSliceX->setMinimum(-bigNum);
                  volumeSpinBoxSliceX->setMaximum(bigNum);
                  volumeSpinBoxSliceX->setSingleStep(1);
                  volumeSpinBoxSliceX->setDecimals(0);
                  volumeSpinBoxSliceY->setMinimum(-bigNum);
                  volumeSpinBoxSliceY->setMaximum(bigNum);
                  volumeSpinBoxSliceY->setSingleStep(1);
                  volumeSpinBoxSliceY->setDecimals(0);
                  volumeSpinBoxSliceZ->setMinimum(-bigNum);
                  volumeSpinBoxSliceZ->setMaximum(bigNum);
                  volumeSpinBoxSliceZ->setSingleStep(1);
                  volumeSpinBoxSliceZ->setDecimals(0);
               
                  int slices[3];
                  bmv->getSelectedObliqueSliceOffsets(viewNumber, slices);
                  volumeSpinBoxSliceX->setValue(slices[0]);
                  volumeSpinBoxSliceY->setValue(slices[1]);
                  volumeSpinBoxSliceZ->setValue(slices[2]);
               }
               slicesViewFlag = true;
               break;
            case VolumeFile::VOLUME_AXIS_UNKNOWN:
               break;
         }
         
         if (slicesViewFlag) {
            volumeSpinBoxSliceX->setToolTip("This control is used to\n"
                                               "select the X' slice offset\n"
                                               "from the oblique view.");
            volumeSpinBoxSliceY->setToolTip("This control is used to\n"
                                               "select the Y' slice offset\n"
                                               "from the oblique view.");
            volumeSpinBoxSliceZ->setToolTip("This control is used to\n"
                                               "select the Z' slice offset\n"
                                               "from the oblique view.");
         }
         else {
            volumeSpinBoxSliceX->setToolTip("This control is used to\n"
                                               "select the parasagittal slice.");
            volumeSpinBoxSliceY->setToolTip("This control is used to\n"
                                               "select the coronal slice.");
            volumeSpinBoxSliceZ->setToolTip("This control is used to\n"
                                               "select the horizontal slice.");
         }
      }
      
      //
      // Unblock signals from slice controls
      //
      volumeSpinBoxSliceX->blockSignals(false);
      volumeSpinBoxSliceY->blockSignals(false);
      volumeSpinBoxSliceZ->blockSignals(false);
      volumeStereotaxicCoordinatesToolButton->blockSignals(false);
   }
   
   updateMouseModeComboBox();
   
   adjustSize();
}

 
/**
 * Update all toolbars with brain models.
 */
void
GuiToolBar::updateAllToolBars(const bool additionalFilesLoaded)
{
   GuiBrainModelOpenGL::setPaintingEnabled(false);
   for (unsigned int i = 0; i < allToolBars.size(); i++) {
      if (allToolBars[i] != NULL) {
         allToolBars[i]->loadModelComboBox();
         allToolBars[i]->updateViewControls();
         //
         // if loading all files then turn off yoking
         //
         if (allToolBars[i]->yokeButton != NULL) {
            if (additionalFilesLoaded) {
               //allToolBars[i]->yokeButton->setDown(false);
               allToolBars[i]->brainModelOpenGL->setYokeView(false);
            }
            allToolBars[i]->yokeButton->setChecked(allToolBars[i]->brainModelOpenGL->getYokeView());
         }
      }
   }   
   GuiBrainModelOpenGL::setPaintingEnabled(true);
}

/**
 * Load the model names into the model combo box.
 */
void
GuiToolBar::loadModelComboBox()
{
   if (theMainWindow == NULL) {
      return;
   }
   
   BrainSet* theBrain = theMainWindow->getBrainSet(brainModelOpenGL->getModelViewNumber());
   BrainModel* theBrainModel = brainModelOpenGL->getDisplayedBrainModel();

   modelFileComboBox->updateControl(theBrain, theBrainModel);   
}

/**
 * Called when a standard view is selected.
 */
void
GuiToolBar::setViewSelection(const BrainModel::STANDARD_VIEWS standardView)
{
   BrainModel* bm = brainModelOpenGL->getDisplayedBrainModel();
   if (bm != NULL) {
      switch (bm->getModelType()) {
         case BrainModel::BRAIN_MODEL_SURFACE:
            {
               BrainModelSurface* s = dynamic_cast<BrainModelSurface*>(bm);
               
               //
               // If yoked set the view of the caret main window
               //
               bool surfaceYokedFlag = false;
               BrainModelSurface* mainWindowModelSurface = NULL;
               GuiBrainModelOpenGL* mainWindowBrainModelOpenGL = NULL;
               int mainWindowModelViewNumber = -1;
               if (brainModelOpenGL->getYokeView()) {
                  surfaceYokedFlag = GuiBrainModelOpenGL::getCaretMainWindowModelInfo(mainWindowModelSurface,
                                                                           mainWindowBrainModelOpenGL,
                                                                           mainWindowModelViewNumber);
               }
               
               //
               // Might be yoked to volume
               //
               bool volumeYokedFlag = false;
               BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
               if (brainModelOpenGL->getYokeView()) {
                  if (bmv != NULL) {
                     if (bmv->getSelectedAxis(0) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
                        volumeYokedFlag = true;
                     }
                  }
               }
   
               if (surfaceYokedFlag) {
                  mainWindowModelSurface->setToStandardView(mainWindowModelViewNumber, standardView);
               }
               else if (volumeYokedFlag) {
                  bmv->setToStandardView(0, standardView);
               }
               else {
                  s->setToStandardView(brainModelOpenGL->getModelViewNumber(), standardView);
               }
               if (brainModelOpenGL->getModelViewNumber() == BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW) {
                  theMainWindow->updateTransformationMatrixEditor(NULL);
               }
               GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL); 
            }
            break;
         case BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME:
            {
               BrainModelSurfaceAndVolume* s = dynamic_cast<BrainModelSurfaceAndVolume*>(bm);
               s->setToStandardView(brainModelOpenGL->getModelViewNumber(), standardView);
               if (brainModelOpenGL->getModelViewNumber() == BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW) {
                  theMainWindow->updateTransformationMatrixEditor(NULL);
               }
               GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL); 
            }
            break;
         case BrainModel::BRAIN_MODEL_CONTOURS:
            {
               BrainModelContours* bmc = dynamic_cast<BrainModelContours*>(bm);
               bmc->resetViewingTransform(brainModelOpenGL->getModelViewNumber());
               if (brainModelOpenGL->getModelViewNumber() == BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW) {
                  theMainWindow->updateTransformationMatrixEditor(NULL);
               }
               GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL); 
            }
            break;
         case BrainModel::BRAIN_MODEL_VOLUME:
            {
               BrainModelVolume* bmv = dynamic_cast<BrainModelVolume*>(bm);
               bmv->resetViewingTransform(brainModelOpenGL->getModelViewNumber());
               bmv->initializeSelectedSlices(brainModelOpenGL->getModelViewNumber(), false);
               updateAllToolBars(false);
               if (brainModelOpenGL->getModelViewNumber() == BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW) {
                  theMainWindow->updateTransformationMatrixEditor(NULL);
               }
               GuiBrainModelOpenGL::updateAllGL(); // update all, not just this window's OpenGL 
            }
            break;
      }
   }
}

/**
 * Called when a model is selected 
 */
void
GuiToolBar::setModelSelection(int modelNum)
{
   if (theMainWindow->getBrainSet()->getNumberOfBrainModels() != modelFileComboBox->count()) {
      updateAllToolBars(true);
   }
   
   if (modelNum < modelFileComboBox->count()) {
      if (modelNum < modelFileComboBox->count()) {
         modelFileComboBox->setCurrentIndex(modelNum);
         const BrainSet* oldBrainSet = theMainWindow->getBrainSet(brainModelOpenGL->getModelViewNumber());
         theMainWindow->setBrainSet(brainModelOpenGL->getModelViewNumber(),
                                    modelFileComboBox->getSelectedBrainSet());
         brainModelOpenGL->setDisplayedBrainModel(modelFileComboBox->getSelectedBrainModel());
         if (brainModelOpenGL == mainWindow->getBrainModelOpenGL()) {
            mainWindow->updateStatusBarLabel();
         //}
         //if (brainModelOpenGL->getModelViewNumber() == BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW) {
            theMainWindow->updateTransformationMatrixEditor(NULL);
            
            //
            // If brain set has been changed need to update all display settings
            //
            if (oldBrainSet != theMainWindow->getBrainSet(brainModelOpenGL->getModelViewNumber())) {
               GuiFilesModified fm;
               fm.setStatusForAll(true);
               theMainWindow->fileModificationUpdate(fm);
            }
         }
         GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL);
      }
      updateViewControls();
   }
}

/** 
 * Called when a rotation axis is selected.
 */
void
GuiToolBar::setRotationSelection(int axis)
{
   brainModelOpenGL->setRotationAxis(static_cast<GuiBrainModelOpenGL::BRAIN_MODEL_ROTATION_AXIS>(axis));
}

/**
 * Called when slice X changed.
 */
void
GuiToolBar::volumeSliceXChanged(double value)
{
   const int viewNumber = brainModelOpenGL->getModelViewNumber();
   BrainModelVolume* bmv = brainModelOpenGL->getDisplayedBrainModelVolume();
   if (bmv != NULL) {
      VolumeFile* vf = bmv->getMasterVolumeFile();
      if (vf != NULL) {
         switch (bmv->getSelectedAxis(viewNumber)) {
            case VolumeFile::VOLUME_AXIS_X:
            case VolumeFile::VOLUME_AXIS_Y:
            case VolumeFile::VOLUME_AXIS_Z:
            case VolumeFile::VOLUME_AXIS_ALL:
               {
                  int slices[3];
                  bmv->getSelectedOrthogonalSlices(viewNumber, slices);
                  if (bmv->getViewStereotaxicCoordinatesFlag(viewNumber)) {
                     const float xyz[3] = {
                        value,
                        volumeSpinBoxSliceY->value(),
                        volumeSpinBoxSliceZ->value()
                     };
                     vf->convertCoordinatesToVoxelIJK(xyz, slices);
                  }
                  else {
                     slices[0] = static_cast<int>(value);
                  }
                  bmv->setSelectedOrthogonalSlices(viewNumber, slices);
                  
                  //
                  // If main window or yoked to main window
                  // update selected slices in the main and yoked windows
                  //
                  if ((brainModelOpenGL == GuiBrainModelOpenGL::getBrainModelOpenGLForWindow(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW)) ||
                      brainModelOpenGL->getYokeView()) {
                     for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
                        GuiBrainModelOpenGL* glWindow = 
                           GuiBrainModelOpenGL::getBrainModelOpenGLForWindow(static_cast<BrainModel::BRAIN_MODEL_VIEW_NUMBER>(i));
                        if (glWindow != NULL) {
                           BrainModelVolume* bmv2 = glWindow->getDisplayedBrainModelVolume();
                           if (bmv2 != NULL) {
                              if ((i == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) ||
                                  glWindow->getYokeView()) {
                                 bmv2->setSelectedOrthogonalSlices(i, slices);
                                 if (allToolBars[i] != this) {
                                    allToolBars[i]->updateViewControls();
                                 }
                              }
                           }
                        }
                     }
                  }
               }
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE:
               {
                  int slices[3];
                  bmv->getSelectedObliqueSlices(slices);
                  slices[0] = static_cast<int>(value);
                  bmv->setSelectedObliqueSlices(slices);
               }
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
               {
                  int slices[3];
                  bmv->getSelectedObliqueSliceOffsets(viewNumber, slices);
                  slices[0] = static_cast<int>(value);
                  bmv->setSelectedObliqueSliceOffsets(viewNumber, slices);
               }
               break;
            case VolumeFile::VOLUME_AXIS_UNKNOWN:
               break;
         }
         if (bmv->getSelectedAxis(viewNumber) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
            GuiBrainModelOpenGL::updateAllGL(); 
         }
         else {
            GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL); 
         }
      }
   }
}

/**
 * Called when slice Y changed.
 */
void
GuiToolBar::volumeSliceYChanged(double value)
{
   const int viewNumber = brainModelOpenGL->getModelViewNumber();
   BrainModelVolume* bmv = brainModelOpenGL->getDisplayedBrainModelVolume();
   if (bmv != NULL) {
      VolumeFile* vf = bmv->getMasterVolumeFile();
      if (vf != NULL) {
         switch (bmv->getSelectedAxis(viewNumber)) {
            case VolumeFile::VOLUME_AXIS_X:
            case VolumeFile::VOLUME_AXIS_Y:
            case VolumeFile::VOLUME_AXIS_Z:
            case VolumeFile::VOLUME_AXIS_ALL:
               /*
               {
                  int slices[3];
                  bmv->getSelectedOrthogonalSlices(viewNumber, slices);
                  slices[1] = static_cast<int>(value);
                  bmv->setSelectedOrthogonalSlices(viewNumber, slices);
               }
               */
               {
                  int slices[3];
                  bmv->getSelectedOrthogonalSlices(viewNumber, slices);
                  if (bmv->getViewStereotaxicCoordinatesFlag(viewNumber)) {
                     const float xyz[3] = {
                        volumeSpinBoxSliceX->value(),
                        value,
                        volumeSpinBoxSliceZ->value()
                     };
                     vf->convertCoordinatesToVoxelIJK(xyz, slices);
                  }
                  else {
                     slices[1] = static_cast<int>(value);
                  }
                  bmv->setSelectedOrthogonalSlices(viewNumber, slices);

                  //
                  // If main window or yoked to main window
                  // update selected slices in the main and yoked windows
                  //
                  if ((brainModelOpenGL == GuiBrainModelOpenGL::getBrainModelOpenGLForWindow(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW)) ||
                      brainModelOpenGL->getYokeView()) {
                     for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
                        GuiBrainModelOpenGL* glWindow = 
                           GuiBrainModelOpenGL::getBrainModelOpenGLForWindow(static_cast<BrainModel::BRAIN_MODEL_VIEW_NUMBER>(i));
                        if (glWindow != NULL) {
                           BrainModelVolume* bmv2 = glWindow->getDisplayedBrainModelVolume();
                           if (bmv2 != NULL) {
                              if ((i == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) ||
                                  glWindow->getYokeView()) {
                                 bmv2->setSelectedOrthogonalSlices(i, slices);
                                 if (allToolBars[i] != this) {
                                    allToolBars[i]->updateViewControls();
                                 }
                              }
                           }
                        }
                     }
                  }
               }
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE:
               {
                  int slices[3];
                  bmv->getSelectedObliqueSlices(slices);
                  slices[1] = static_cast<int>(value);
                  bmv->setSelectedObliqueSlices(slices);
               }
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
               {
                  int slices[3];
                  bmv->getSelectedObliqueSliceOffsets(viewNumber, slices);
                  slices[1] = static_cast<int>(value);
                  bmv->setSelectedObliqueSliceOffsets(viewNumber, slices);
               }
               break;
            case VolumeFile::VOLUME_AXIS_UNKNOWN:
               break;
         }
         if (bmv->getSelectedAxis(viewNumber) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
            GuiBrainModelOpenGL::updateAllGL(); 
         }
         else {
            GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL); 
         }
      }
   }
}

/**
 * Called when slice Z changed.
 */
void
GuiToolBar::volumeSliceZChanged(double value)
{
   const int viewNumber = brainModelOpenGL->getModelViewNumber();
   BrainModelVolume* bmv = brainModelOpenGL->getDisplayedBrainModelVolume();
   if (bmv != NULL) {
      VolumeFile* vf = bmv->getMasterVolumeFile();
      if (vf != NULL) {
         switch (bmv->getSelectedAxis(viewNumber)) {
            case VolumeFile::VOLUME_AXIS_X:
            case VolumeFile::VOLUME_AXIS_Y:
            case VolumeFile::VOLUME_AXIS_Z:
            case VolumeFile::VOLUME_AXIS_ALL:
               /*
               {
                  int slices[3];
                  bmv->getSelectedOrthogonalSlices(viewNumber, slices);
                  slices[2] = static_cast<int>(value);
                  bmv->setSelectedOrthogonalSlices(viewNumber, slices);
               }
               */
               {
                  int slices[3];
                  bmv->getSelectedOrthogonalSlices(viewNumber, slices);
                  if (bmv->getViewStereotaxicCoordinatesFlag(viewNumber)) {
                     const float xyz[3] = {
                        volumeSpinBoxSliceX->value(),
                        volumeSpinBoxSliceY->value(),
                        value
                     };
                     vf->convertCoordinatesToVoxelIJK(xyz, slices);
                  }
                  else {
                     slices[2] = static_cast<int>(value);
                  }
                  bmv->setSelectedOrthogonalSlices(viewNumber, slices);

                  //
                  // If main window or yoked to main window
                  // update selected slices in the main and yoked windows
                  //
                  if ((brainModelOpenGL == GuiBrainModelOpenGL::getBrainModelOpenGLForWindow(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW)) ||
                      brainModelOpenGL->getYokeView()) {
                     for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
                        GuiBrainModelOpenGL* glWindow = 
                           GuiBrainModelOpenGL::getBrainModelOpenGLForWindow(static_cast<BrainModel::BRAIN_MODEL_VIEW_NUMBER>(i));
                        if (glWindow != NULL) {
                           BrainModelVolume* bmv2 = glWindow->getDisplayedBrainModelVolume();
                           if (bmv2 != NULL) {
                              if ((i == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) ||
                                  glWindow->getYokeView()) {
                                 bmv2->setSelectedOrthogonalSlices(i, slices);
                                 if (allToolBars[i] != this) {
                                    allToolBars[i]->updateViewControls();
                                 }
                              }
                           }
                        }
                     }
                  }
               }
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE:
               {
                  int slices[3];
                  bmv->getSelectedObliqueSlices(slices);
                  slices[2] = static_cast<int>(value);
                  bmv->setSelectedObliqueSlices(slices);
               }
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
               {
                  int slices[3];
                  bmv->getSelectedObliqueSliceOffsets(viewNumber, slices);
                  slices[2] = static_cast<int>(value);
                  bmv->setSelectedObliqueSliceOffsets(viewNumber, slices);
               }
               break;
            case VolumeFile::VOLUME_AXIS_UNKNOWN:
               break;
         }
         if (bmv->getSelectedAxis(viewNumber) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
            GuiBrainModelOpenGL::updateAllGL(); 
         }
         else {
            GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL); 
         }
      }
   }
}

/**
 * Called when volume view axis changed.
 */
void
GuiToolBar::volumeAxisChanged(int value)
{
   BrainModelVolume* bmv = brainModelOpenGL->getDisplayedBrainModelVolume();
   if (bmv != NULL) {
      bmv->setSelectedAxis(brainModelOpenGL->getModelViewNumber(),
                           static_cast<VolumeFile::VOLUME_AXIS>(value));
      updateViewControls();
      GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL);
   }
   mainWindow->updateStatusBarLabel();
}

/**
 * called when volume stereotaxic coordinates is toggled.
 */
void 
GuiToolBar::volumeStereotaxicCoordinatesSlot(bool b)
{
   BrainModelVolume* bmv = brainModelOpenGL->getDisplayedBrainModelVolume();
   if (bmv != NULL) {
      bmv->setViewStereotaxicCoordinatesFlag(brainModelOpenGL->getModelViewNumber(), b); 
      updateViewControls();
      GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL);
   }
}
      
/**
 * called when toolbar is resized
 */
void 
GuiToolBar::resizeEvent(QResizeEvent* /*e*/)
{
/*
   updateViewControls();
   QToolBar::resizeEvent(e);
   updateViewControls();
*/
}
      
/**
 * update mouse mode combo box.
 */
void 
GuiToolBar::updateMouseModeComboBox()
{
   if (mouseModeComboBox == NULL) {
      return;
   }
   
   //
   // Clear combo box
   //
   mouseModeComboBox->clear();
   mouseModeComboBox->addItem("Select Mode", GuiBrainModelOpenGL::MOUSE_MODE_NONE);
   
   BrainModel* bm = brainModelOpenGL->getDisplayedBrainModel();
   if (bm != NULL) {
      switch (bm->getModelType()) {
         case BrainModel::BRAIN_MODEL_SURFACE:
            mouseModeComboBox->addItem("View", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_VIEW));
            mouseModeComboBox->addItem("Border Draw", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DRAW));
            mouseModeComboBox->addItem("Border Delete", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE));
            mouseModeComboBox->addItem("Border Rename", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_RENAME));
            mouseModeComboBox->addItem("Border Reverse Point Order", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_REVERSE));
            mouseModeComboBox->addItem("Border Point Delete", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE_POINT));
            mouseModeComboBox->addItem("Border Point Move", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_MOVE_POINT));
            mouseModeComboBox->addItem("Border Update", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_UPDATE));
            mouseModeComboBox->addItem("Cell Add", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CELL_ADD));
            mouseModeComboBox->addItem("Cell Delete", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CELL_DELETE));
            mouseModeComboBox->addItem("Cut Draw", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CUT_DRAW));
            mouseModeComboBox->addItem("Cut Delete", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CUT_DELETE));
            mouseModeComboBox->addItem("Foci Delete", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_FOCI_DELETE));
            break;
         case BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME:
            mouseModeComboBox->addItem("View", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_VIEW));
            break;
         case BrainModel::BRAIN_MODEL_CONTOURS:
            mouseModeComboBox->addItem("View", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_VIEW));
            mouseModeComboBox->addItem("Contour Draw", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DRAW));
            mouseModeComboBox->addItem("Contour Delete", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DELETE));
            mouseModeComboBox->addItem("Contour Merge", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_MERGE));
            mouseModeComboBox->addItem("Contour Reverse Point Order", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_REVERSE));
            mouseModeComboBox->addItem("Contour Point Delete", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_POINT_DELETE));
            mouseModeComboBox->addItem("Contour Point Move", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_POINT_MOVE));
            mouseModeComboBox->addItem("Contour Cell Add", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_ADD));
            mouseModeComboBox->addItem("Contour Cell Delete", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_DELETE));
            mouseModeComboBox->addItem("Contour Cell Move", 
               static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_MOVE));
            break;
         case BrainModel::BRAIN_MODEL_VOLUME:
            {
               BrainModelVolumeVoxelColoring* voxelColoring =
                  theMainWindow->getBrainSet()->getVoxelColoring();
               mouseModeComboBox->addItem("View", 
                  static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_VIEW));
               mouseModeComboBox->addItem("Border Delete", 
                  static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE));
               mouseModeComboBox->addItem("Border Rename", 
                  static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_RENAME));
               mouseModeComboBox->addItem("Border Reverse Point Order", 
                  static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_REVERSE));
               mouseModeComboBox->addItem("Border Point Delete", 
                  static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE_POINT));
               mouseModeComboBox->addItem("Border Point Move", 
                  static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_MOVE_POINT));
                  
               if (voxelColoring->isUnderlayOrOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT)) {
                  mouseModeComboBox->addItem("Volume Paint Edit", 
                     static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_PAINT_EDIT));
               }
               if (voxelColoring->isUnderlayOrOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION)) {
                  mouseModeComboBox->addItem("Volume Segmentation Edit", 
                     static_cast<int>(GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_SEGMENTATION_EDIT));
               }
            }
            break;
      }
   }
   
   //
   // Display the current mode 
   //
   int defaultItem = 0;
   const int num = mouseModeComboBox->count();
   for (int i = 0; i < num; i++) {
      const GuiBrainModelOpenGL::MOUSE_MODES mode =
         static_cast<GuiBrainModelOpenGL::MOUSE_MODES>(mouseModeComboBox->itemData(i).toInt());
      if (mode == brainModelOpenGL->getMouseMode()) {
         defaultItem = i;
         break;
      }
   }
   
   mouseModeComboBox->blockSignals(true);
   mouseModeComboBox->setCurrentIndex(defaultItem);
   mouseModeComboBox->blockSignals(false);
}
      
/**
 * called when a mouse mode is selected.
 */
void 
GuiToolBar::slotMouseModeComboBoxActivated(int indx)
{
   
   //
   // Set the mouse mode
   //
   if ((indx >= 0) && (indx < mouseModeComboBox->count())) {
      const GuiBrainModelOpenGL::MOUSE_MODES mode =
         static_cast<GuiBrainModelOpenGL::MOUSE_MODES>(mouseModeComboBox->itemData(indx).toInt());
      if (mode != GuiBrainModelOpenGL::MOUSE_MODE_NONE) {
         brainModelOpenGL->setMouseMode(mode);
         
         //
         // main window actions
         //
         GuiMainWindowLayersActions* layersActions = theMainWindow->getLayersActions();
         GuiMainWindowVolumeActions* volumeActions = theMainWindow->getVolumeActions();
         
         //
         // Possibly display dialogs
         //
         switch (mode) {
            case GuiBrainModelOpenGL::MOUSE_MODE_NONE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_VIEW:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DRAW:
               {
                  QAction* action = layersActions->getBordersDrawAction();
                  action->trigger();
               }
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE_POINT:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_INTERPOLATE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_MOVE_POINT:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_REVERSE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_RENAME:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_UPDATE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CUT_DRAW:
               {
                  DisplaySettingsCuts* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCuts();
                  dsc->setDisplayCuts(! dsc->getDisplayCuts());
                  GuiBrainModelOpenGL::updateAllGL(NULL);
               }
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CUT_DELETE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_FOCI_DELETE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CELL_ADD:
               {
                  QAction* action = layersActions->getCellsAddAction();
                  action->trigger();
               }
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CELL_DELETE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_BORDER_SELECT:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_PAINT_INDEX_SELECT:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_METRIC_NODE_SELECT:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_GEODESIC_NODE_SELECT:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_ALIGN_STANDARD_ORIENTATION:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_SET_SCALE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DRAW:
               {
                  QAction* action = layersActions->getContourDrawAction();
                  action->trigger();
               }
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN_REGION:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_POINT_MOVE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_POINT_DELETE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DELETE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_REVERSE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_MERGE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_ADD:
               {
                  QAction* action = layersActions->getCellsAddAction();
                  action->trigger();
               }
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_DELETE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_MOVE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_SEGMENTATION_EDIT:
               {
                  QAction* action = volumeActions->getEditSegmentationAction();
                  action->trigger();
               }
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_EDIT_ADD_NODE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_EDIT_ADD_TILE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_EDIT_DELETE_TILE_BY_LINK:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_EDIT_DISCONNECT_NODE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_EDIT_MOVE_NODE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_SHAPE_NODE_SELECT:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_TRANSFORMATION_MATRIX_AXES:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_TRANSFORMATION_MATRIX_SET_TRANSLATE:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_PAINT_EDIT:
               {
                  QAction* action = volumeActions->getEditPaintVolumeAction();
                  action->trigger();
               }
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_IMAGE_SUBREGION:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_SULCAL_BORDER_NODE_START:
               break;
            case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_SULCAL_BORDER_NODE_END:
               break;
         }
      }
   }   
}

      


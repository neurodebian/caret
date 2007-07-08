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
#include "BrainModelContours.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceAndVolume.h"
#include "BrainModelVolume.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiToolBar.h"
#include "GuiToolBarActions.h"
#include "global_variables.h"

/**
 * Constructor.
 * Parent is the window that this toolbar is being added to.
 * mainWindowIn is Caret's main window.
 * mainWindowFlag is set if this tool bar is being added to Caret's main window.
 */
GuiToolBarActions::GuiToolBarActions(GuiBrainModelOpenGL* brainModelOpenGLIn,
                                     QWidget* parent)
   : QObject(parent)
{
    brainModelOpenGL = brainModelOpenGLIn;
    
    viewModeAction = new QAction(this);
    viewModeAction->setText("View");
    viewModeAction->setToolTip("Set Mouse Mode to View");
    QObject::connect(viewModeAction, SIGNAL(triggered()),
                     this, SLOT(viewModeSlot()));
                     
    medialViewAction = new QAction(this);
    medialViewAction->setText("M");
    medialViewAction->setToolTip("Switch to Medial View of Surface");
    QObject::connect(medialViewAction, SIGNAL(triggered()),
                     this, SLOT(medialViewSlot()));
                     
    lateralViewAction = new QAction(this);
    lateralViewAction->setText("L");
    lateralViewAction->setToolTip("Switch to Lateral View of Surface");
    QObject::connect(lateralViewAction, SIGNAL(triggered()),
                     this, SLOT(lateralViewSlot()));
                     
    anteriorViewAction = new QAction(this);
    anteriorViewAction->setText("A");
    anteriorViewAction->setToolTip("Switch to Anterior View of Surface");
    QObject::connect(anteriorViewAction, SIGNAL(triggered()),
                     this, SLOT(anteriorViewSlot()));
                     
    posteriorViewAction = new QAction(this);
    posteriorViewAction->setText("P");
    posteriorViewAction->setToolTip("Switch to Posterior View of Surface");
    QObject::connect(posteriorViewAction, SIGNAL(triggered()),
                     this, SLOT(posteriorViewSlot()));
                     
    dorsalViewAction = new QAction(this);
    dorsalViewAction->setText("D");
    dorsalViewAction->setToolTip("Switch to Dorsal View of Surface");
    QObject::connect(dorsalViewAction, SIGNAL(triggered()),
                     this, SLOT(dorsalViewSlot()));
                     
    ventralViewAction = new QAction(this);
    ventralViewAction->setText("V");
    ventralViewAction->setToolTip("Switch to Ventral View of Surface");
    QObject::connect(ventralViewAction, SIGNAL(triggered()),
                     this, SLOT(ventralViewSlot()));
                     
    resetViewAction = new QAction(this);
    resetViewAction->setText("R");
    resetViewAction->setToolTip("Reset the View of Model");
    QObject::connect(resetViewAction, SIGNAL(triggered()),
                     this, SLOT(resetViewSlot()));
                     
    xRotation90Action = new QAction(this);
    xRotation90Action->setText("X");
    xRotation90Action->setToolTip("Rotate Surface 90 Degress Around X Axis");
    QObject::connect(xRotation90Action, SIGNAL(triggered()),
                     this, SLOT(xRotation90Slot()));
                     
    yRotation90Action = new QAction(this);
    yRotation90Action->setText("Y");
    yRotation90Action->setToolTip("Rotate Surface 90 Degress Around Y Axis");
    QObject::connect(yRotation90Action, SIGNAL(triggered()),
                     this, SLOT(yRotation90Slot()));
                     
    zRotation90Action = new QAction(this);
    zRotation90Action->setText("Z");
    zRotation90Action->setToolTip("Rotate Surface 90 Degress Around Z Axis");
    QObject::connect(zRotation90Action, SIGNAL(triggered()),
                     this, SLOT(zRotation90Slot()));
                     
    displayControlDialogAction = new QAction(this);
    displayControlDialogAction->setText("D/C");
    displayControlDialogAction->setToolTip("Show Display Control Dialog");
    QObject::connect(displayControlDialogAction, SIGNAL(triggered()),
                     this, SLOT(displayControlDialogSlot()));
                     
    specDialogAction = new QAction(this);
    specDialogAction->setText("Spec");
    specDialogAction->setToolTip("Quickly open file from spec file.");
    QObject::connect(specDialogAction, SIGNAL(triggered()),
                     this, SLOT(specDialogSlot()));
           
    yokeAction = new QAction(this);
    yokeAction->setText("Yoke");
    yokeAction->setCheckable(true);
    yokeAction->setToolTip("Selecting Yoke will set the view of\n"
                           "model in this window to the view used\n"
                           "in the main window.");
    QObject::connect(yokeAction, SIGNAL(triggered(bool)),
                     this, SLOT(yokeSlot(bool)));

    volumeUnderlayOnlyAction = new QAction(this);
    volumeUnderlayOnlyAction->setText("UO");
    volumeUnderlayOnlyAction->setCheckable(true);
    volumeUnderlayOnlyAction->setToolTip("Selecting Underlay Only will inhibit\n"
                                         "the display of overlays on volume \n"
                                         "slices in this window.  This may be \n"
                                         "useful when editing a segmentation \n"
                                         "volume in the Main Window.");
    QObject::connect(volumeUnderlayOnlyAction, SIGNAL(triggered(bool)),
                     this, SLOT(underlayOnlySlot(bool)));
}

/**
 * Destructor.
 */
GuiToolBarActions::~GuiToolBarActions()
{
}

/**
 * Called when a standard view is selected.
 */
void
GuiToolBarActions::setViewSelection(const BrainModel::STANDARD_VIEWS standardView)
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
               GuiToolBar::updateAllToolBars(false);
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
 * Called when a rotation axis is selected.
 */
void
GuiToolBarActions::setRotationSelection(int axis)
{
   brainModelOpenGL->setRotationAxis(static_cast<GuiBrainModelOpenGL::BRAIN_MODEL_ROTATION_AXIS>(axis));
}

/**
 * Called when yoke button is pressed
 */
void
GuiToolBarActions::yokeSlot(bool selected)
{
   brainModelOpenGL->setYokeView(selected);
   GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL);
}

/**
 * called when underlay only button toggled.
 */
void 
GuiToolBarActions::underlayOnlySlot(bool b)
{
   BrainModelVolume* bmv = brainModelOpenGL->getDisplayedBrainModelVolume();
   bmv->setShowUnderlayOnlyInWindow(brainModelOpenGL->getModelViewNumber(), b);
   GuiBrainModelOpenGL::updateAllGL(brainModelOpenGL);
}

/*
 * Called when medial view tool button is pressed.
 */
void
GuiToolBarActions::medialViewSlot()
{
   setViewSelection(BrainModelSurface::VIEW_MEDIAL);
}

/**
 * Called when lateral view tool button is pressed.
 */
void
GuiToolBarActions::lateralViewSlot()
{
   setViewSelection(BrainModelSurface::VIEW_LATERAL);
}

/**
 * Called when anterior view tool button is pressed.
 */
void
GuiToolBarActions::anteriorViewSlot()
{
   setViewSelection(BrainModelSurface::VIEW_ANTERIOR);
}

/**
 * Called when posterior view tool button is pressed.
 */
void
GuiToolBarActions::posteriorViewSlot()
{
   setViewSelection(BrainModelSurface::VIEW_POSTERIOR);
}

/**
 * Called when dorsal view tool button is pressed.
 */
void
GuiToolBarActions::dorsalViewSlot()
{
   setViewSelection(BrainModelSurface::VIEW_DORSAL);
}

/**
 * Called when ventral view tool button is pressed.
 */
void
GuiToolBarActions::ventralViewSlot()
{
   setViewSelection(BrainModelSurface::VIEW_VENTRAL);
}

/**
 * Called when reset view tool button is pressed.
 */
void
GuiToolBarActions::resetViewSlot()
{
   setViewSelection(BrainModelSurface::VIEW_RESET);
}

/**
 * Called when rotate X 90 tool button is pressed.
 */
void
GuiToolBarActions::xRotation90Slot()
{
   setViewSelection(BrainModelSurface::VIEW_ROTATE_X_90);
}

/**
 * Called when rotate Y 90 tool button is pressed.
 */
void
GuiToolBarActions::yRotation90Slot()
{
   setViewSelection(BrainModelSurface::VIEW_ROTATE_Y_90);
}

/**
 * Called when rotate Z 90 tool button is pressed.
 */
void
GuiToolBarActions::zRotation90Slot()
{
   setViewSelection(BrainModelSurface::VIEW_ROTATE_Z_90);
}

/**
 * called to switch to view mode
 */
void
GuiToolBarActions::viewModeSlot()
{
   brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
}

/**
 * called when display control button pressed.
 */
void 
GuiToolBarActions::displayControlDialogSlot()
{
   theMainWindow->displayDisplayControlDialog();
}

/**
 * called when spec button pressed.
 */
void 
GuiToolBarActions::specDialogSlot()
{
   theMainWindow->displayFastOpenDataFileDialog();
}
      


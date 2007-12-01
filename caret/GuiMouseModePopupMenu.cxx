
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

#include <QAction>

#include "BorderFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelContours.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "CellProjectionFile.h"
#include "ContourCellFile.h"
#include "ContourFile.h"
#include "CutsFile.h"
#include "DisplaySettingsBorders.h"
#include "DisplaySettingsCells.h"
#include "DisplaySettingsContours.h"
#include "DisplaySettingsCuts.h"
#include "DisplaySettingsFoci.h"
#include "FociProjectionFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowLayersActions.h"
#include "GuiMainWindowVolumeActions.h"
#include "GuiMouseModePopupMenu.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiMouseModePopupMenu::GuiMouseModePopupMenu(GuiBrainModelOpenGL* brainModelOpenGLIn,
                                             const bool showViewModeIn,
                                             QWidget* parent)
   : QMenu(parent)
{
   brainModelOpenGL = brainModelOpenGLIn;
   showViewMode = showViewModeIn;
   
   QObject::connect(this, SIGNAL(aboutToShow()),
                    this, SLOT(slotAboutToShow()));
   QObject::connect(this, SIGNAL(triggered(QAction*)),
                    this, SLOT(slotMenuItemSelected(QAction*)));
}

/**
 * destructor.
 */
GuiMouseModePopupMenu::~GuiMouseModePopupMenu()
{
}

/**
 * called when menu item selected.
 */
void 
GuiMouseModePopupMenu::slotMenuItemSelected(QAction* action)
{
   if (action != NULL) {
      const QString name = action->text();
      //std::cout << "Item Selected: " << name.toAscii().constData() 
      //          << std::endl;
                
      //
      // main window actions
      //
      GuiMainWindowLayersActions* layersActions = theMainWindow->getLayersActions();
      GuiMainWindowVolumeActions* volumeActions = theMainWindow->getVolumeActions();
         
      if (name == getActionBorderDrawName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DRAW);
         QAction* action = layersActions->getBordersDrawAction();
         action->trigger();
      }
      else if (name == getActionBorderDeleteName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE);
      }
      else if (name == getActionBorderRenameName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_RENAME);
      }
      else if (name == getActionBorderReversePointOrderName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_REVERSE);
      }
      else if (name == getActionBorderPointDeleteName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE_POINT);
      }
      else if (name == getActionBorderPointMoveName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_MOVE_POINT);
      }
      else if (name == getActionCellAddName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CELL_ADD);
         QAction* action = layersActions->getCellsAddAction();
         action->trigger();
      }
      else if (name == getActionCellDeleteName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CELL_DELETE);
      }
      else if (name == getActionContourDrawName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DRAW);
         QAction* action = layersActions->getContourDrawAction();
         action->trigger();
      }
      else if (name == getActionContourDeleteName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DELETE);
      }
      else if (name == getActionContourMergeName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_MERGE);
      }
      else if (name == getActionContourReversePointOrderName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_REVERSE);
      }
      else if (name == getActionContourPointDeleteName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_POINT_DELETE);
      }
      else if (name == getActionContourPointMoveName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_POINT_MOVE);
      }
      else if (name == getActionContourCellAddName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_ADD);
         QAction* action = layersActions->getCellsAddAction();
         action->trigger();
      }
      else if (name == getActionContourCellDeleteName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_DELETE);
      }
      else if (name == getActionContourCellMoveName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_MOVE);
      }
      else if (name == getActionCutDrawName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CUT_DRAW);
         DisplaySettingsCuts* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCuts();
         dsc->setDisplayCuts(! dsc->getDisplayCuts());
         GuiBrainModelOpenGL::updateAllGL(NULL);
      }
      else if (name == getActionCutDeleteName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CUT_DELETE);
      }
      else if (name == getActionFociDeleteName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_FOCI_DELETE);
      }
      else if (name == getActionTransformationAxesName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_TRANSFORMATION_MATRIX_AXES);
      }
      else if (name == getActionVolumePaintEditName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_PAINT_EDIT);
         QAction* action = volumeActions->getEditPaintVolumeAction();
         action->trigger();
      }
      else if (name == getActionVolumeSegmentationEditName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_SEGMENTATION_EDIT);
         QAction* action = volumeActions->getEditSegmentationAction();
         action->trigger();
      }
      else if (name == getActionViewName()) {
         brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
      }
   }
}
      
/**
 * called when menu is about to be displayed.
 */
void 
GuiMouseModePopupMenu::slotAboutToShow()
{
   //
   // Clear menu
   //
   clear();
   
   //
   // Types of data loaded
   //
   BrainSet* bs = theMainWindow->getBrainSet();
   
   //
   // Create menu
   //
   BrainModel* bm = brainModelOpenGL->getDisplayedBrainModel();
   if (bm != NULL) {
      switch (bm->getModelType()) {
         case BrainModel::BRAIN_MODEL_SURFACE:
            if (showViewMode) {
               addAction(getActionViewName());
            }
            
            addAction(getActionBorderDrawName());
            if ((bs->getBorderSet()->getNumberOfBorders() > 0) &&
                (bs->getDisplaySettingsBorders()->getDisplayBorders())) {
               addAction(getActionBorderDeleteName());
               addAction(getActionBorderRenameName());
               addAction(getActionBorderReversePointOrderName());
               addAction(getActionBorderPointDeleteName());
               addAction(getActionBorderPointMoveName());
            }
            
            addAction(getActionCellAddName());
            if ((bs->getCellProjectionFile()->getNumberOfCellProjections() > 0) &&
                (bs->getDisplaySettingsCells()->getDisplayCells())) {
               addAction(getActionCellDeleteName());
            }
            
            addAction(getActionCutDrawName());
            if ((bs->getCutsFile()->getNumberOfBorders() > 0) &&
                (bs->getDisplaySettingsCuts()->getDisplayCuts())) {
               addAction(getActionCutDeleteName());
            }
            if ((bs->getFociProjectionFile()->getNumberOfCellProjections() > 0) &&
                (bs->getDisplaySettingsFoci()->getDisplayCells())) {
               addAction(getActionFociDeleteName());
            }
            addAction(getActionTransformationAxesName());
            break;
         case BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME:
            if (showViewMode) {
               addAction(getActionViewName());
            }
            break;
         case BrainModel::BRAIN_MODEL_CONTOURS:
            {
               if (showViewMode) {
                  addAction(getActionViewName());
               }
               addAction(getActionContourDrawName());
               BrainModelContours* bmc = bs->getBrainModelContours();
               if (bmc->getContourFile()->getNumberOfContours() > 0) {
                  addAction(getActionContourDeleteName());
                  addAction(getActionContourMergeName());
                  addAction(getActionContourReversePointOrderName());
                  addAction(getActionContourPointDeleteName());
                  addAction(getActionContourPointMoveName());
                  addAction(getActionContourCellAddName());
                  if ((bs->getContourCellFile()->getNumberOfCells() > 0) &&
                      (bs->getDisplaySettingsContours()->getDisplayContourCells())) {
                     addAction(getActionContourCellDeleteName());
                     addAction(getActionContourCellMoveName());
                  }
               }
            }
            break;
         case BrainModel::BRAIN_MODEL_VOLUME:
            {
               BrainModelVolumeVoxelColoring* voxelColoring =
                  theMainWindow->getBrainSet()->getVoxelColoring();
               if (showViewMode) {
                  addAction(getActionViewName());
               }
               addAction(getActionBorderDrawName());
               if ((bs->getVolumeBorderFile()->getNumberOfBorders() > 0) &&
                   (bs->getDisplaySettingsBorders()->getDisplayBorders())) {
                  addAction(getActionBorderDeleteName());
                  addAction(getActionBorderRenameName());
                  addAction(getActionBorderReversePointOrderName());
                  addAction(getActionBorderPointDeleteName());
                  addAction(getActionBorderPointMoveName());
               }
               if (voxelColoring->isUnderlayOrOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT)) {
                  addAction(getActionVolumePaintEditName());
               }
               if (voxelColoring->isUnderlayOrOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION)) {
                  addAction(getActionVolumeSegmentationEditName());
               }
            }
            break;
      }
   }   
}

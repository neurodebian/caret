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

#include "DebugControl.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowLayersActions.h"
#include "GuiMainWindowLayersMenu.h"

/**
 * Constructor.
 */
GuiMainWindowLayersMenu::GuiMainWindowLayersMenu(GuiMainWindow* parent)
   : QMenu("Layers", parent)
{
   setObjectName("GuiMainWindowLayersMenu");
   
   GuiMainWindowLayersActions* layersActions = parent->getLayersActions();
   
   createBordersSubMenu(layersActions);
   
   createCellsSubMenu(layersActions);
   
   createContoursSubMenu(layersActions);
   
   createContourCellsSubMenu(layersActions);
   
   createFociSubMenu(layersActions);

   QObject::connect(this, SIGNAL(aboutToShow()),
                    layersActions, SLOT(updateActions()));
}

/**
 * Destructor.
 */
GuiMainWindowLayersMenu::~GuiMainWindowLayersMenu()
{
}

/**
 * Create the cell sub-menu.
 */
void
GuiMainWindowLayersMenu::createCellsSubMenu(GuiMainWindowLayersActions* layersActions)
{
   cellsSubMenu = addMenu("Cells");
   
   cellsSubMenu->addAction(layersActions->getCellsAddAction());
   cellsSubMenu->addSeparator();
   
   cellsSubMenu->addAction(layersActions->getCellReportAction());
   cellsSubMenu->addSeparator();
   
   cellsSubMenu->addAction(layersActions->getCellsDensityToMetricAction());
   cellsSubMenu->addAction(layersActions->getCellsConvertToVtkModelAction());
   cellsSubMenu->addSeparator();

   cellsSubMenu->addAction(layersActions->getCellsEditAttributesAction());
   cellsSubMenu->addAction(layersActions->getCellsEditColorsAction());
   cellsSubMenu->addAction(layersActions->getCellsProjectAction());
   cellsSubMenu->addSeparator();

   cellsSubMenu->addAction(layersActions->getCellsDeleteAllAction());
   cellsSubMenu->addAction(layersActions->getCellsDeleteUsingMouseAction());
}

/**
 * Create the foci sub-menu.
 */
void
GuiMainWindowLayersMenu::createFociSubMenu(GuiMainWindowLayersActions* layersActions)
{
   fociSubMenu = addMenu("Foci");
   
   fociSubMenu->addAction(layersActions->getFociAssignClassToDisplayedFoci());
   fociSubMenu->addAction(layersActions->getFociAttributeAssignmentAction());
   fociSubMenu->addSeparator();
   
   fociSubMenu->addAction(layersActions->getFociDensityToMetricAction());
   fociSubMenu->addAction(layersActions->getFociDensityToVolumeAction());
   fociSubMenu->addAction(layersActions->getFociUncertaintyLimitsAction());
   fociSubMenu->addAction(layersActions->getFociConvertToVtkModelAction());
   fociSubMenu->addSeparator();
   
   fociSubMenu->addAction(layersActions->getFociClearHighlightingAction());
   fociSubMenu->addSeparator();
   
   fociSubMenu->addAction(layersActions->getFociReportAction());
   fociSubMenu->addAction(layersActions->getFociAttributeReportAction());
   fociSubMenu->addSeparator();

   fociSubMenu->addAction(layersActions->getFociMapStereotaxicFocusAction());
   fociSubMenu->addSeparator();
   
   fociSubMenu->addAction(layersActions->getFociEditColorsAction());
   fociSubMenu->addAction(layersActions->getFociDeleteNonMatchingColorsAction());
   fociSubMenu->addSeparator();
   
   fociSubMenu->addAction(layersActions->getFociProjectAction());
   fociSubMenu->addAction(layersActions->getFociPalsProjectAction());
   fociSubMenu->addAction(layersActions->getFociProjectToVolumeAction());
   fociSubMenu->addSeparator();
   
   fociSubMenu->addAction(layersActions->getFociDeleteAllAction());
   fociSubMenu->addAction(layersActions->getFociDeleteNonDisplayedAction());
   fociSubMenu->addAction(layersActions->getFociDeleteUsingMouseAction());
   
   fociSubMenu->addSeparator();
   fociSubMenu->addAction(layersActions->getFociUpdateClassesWithTableSubheaderShortNamesAction());
   fociSubMenu->addAction(layersActions->getFociUpdatePubMedIDIfFocusNameMatchesStudyNameAction());
   fociSubMenu->addAction(layersActions->getFociStudyInfoToStudyMetaDataFileAction());
}

/**
 * Create the borders sub-menu
 */
void
GuiMainWindowLayersMenu::createBordersSubMenu(GuiMainWindowLayersActions* layersActions)
{
   bordersSubMenu = addMenu("Borders");

   //if (DebugControl::getTestFlag1()) {
      bordersSubMenu->addAction(layersActions->getBorderOperationsDialogAction());
   //}
   //bordersSubMenu->addAction(layersActions->getBordersCompareAction());
   bordersSubMenu->addAction(layersActions->getBordersVolumeToBordersFiducialAction());
   bordersSubMenu->addAction(layersActions->getBordersVolumeToFiducialCellsAction());
   bordersSubMenu->addAction(layersActions->getBordersConvertToVtkModelAction());
   bordersSubMenu->addSeparator();

   bordersSubMenu->addAction(layersActions->getBordersAverageAction());
   bordersSubMenu->addAction(layersActions->getBorderCreatedFromPaintAction());
   bordersSubMenu->addAction(layersActions->getBordersCreateGridAction());
   bordersSubMenu->addAction(layersActions->getBordersCreateAnalysisGridAction());
   bordersSubMenu->addAction(layersActions->getBordersCreateInterpolatedAction());
   bordersSubMenu->addAction(layersActions->getBordersCreateSphericalAction()); 
   bordersSubMenu->addAction(layersActions->getBordersDrawAction());
   bordersSubMenu->addAction(layersActions->getBorderDrawUpdateAction());
   bordersSubMenu->addAction(layersActions->getCopyBorderByNameAction());
   bordersSubMenu->addSeparator();

   bordersSubMenu->addAction(layersActions->getBordersEditAttributesAction());
   bordersSubMenu->addAction(layersActions->getBordersEditColorsAction());
   bordersSubMenu->addSeparator();

   bordersSubMenu->addAction(layersActions->getBordersClearHighlightingAction());
   bordersSubMenu->addSeparator();
   
   bordersSubMenu->addAction(layersActions->getBordersMovePointWithMouseAction());
   bordersSubMenu->addAction(layersActions->getBordersRenameWithMouseAction()); 
   bordersSubMenu->addAction(layersActions->getBordersReverseWithMouseAction());
                              
   bordersSubMenu->addSeparator();
   bordersSubMenu->addAction(layersActions->getBordersOrientDisplayedClockwiseAction());
   bordersSubMenu->addAction(layersActions->getBordersResampleDisplayedAction());
   bordersSubMenu->addAction(layersActions->getBordersReverseDisplayedAction());
   bordersSubMenu->addSeparator();

   bordersSubMenu->addAction(layersActions->getBordersProjectAction());
   bordersSubMenu->addSeparator();

   bordersSubMenu->addAction(layersActions->getBordersDeleteAllAction());
   bordersSubMenu->addAction(layersActions->getDeleteBordersByNameAction());
   bordersSubMenu->addAction(layersActions->getBordersDeletePointWithMouseAction());
   bordersSubMenu->addAction(layersActions->getBordersDeleteWithMouseAction());
   bordersSubMenu->addAction(layersActions->getBordersDeletePointsOutsideSurfaceAction());
}

/**
 * Create the contour cells sub-menu.
 */
void
GuiMainWindowLayersMenu::createContourCellsSubMenu(GuiMainWindowLayersActions* layersActions)
{
   contourCellsSubMenu = addMenu("Contour Cells");
   
   contourCellsSubMenu->addAction(layersActions->getContourCellsAddAction());

   contourCellsSubMenu->addSeparator();
    
   contourCellsSubMenu->addAction(layersActions->getContourCellsDeleteAllAction());

   contourCellsSubMenu->addAction(layersActions->getContourCellsDeleteWithMouseAction());

   contourCellsSubMenu->addAction(layersActions->getContourCellsEditColorsAction());
                            
   contourCellsSubMenu->addAction(layersActions->getContourCellsMoveWithMouseAction());
}

/**
 * Create the contours sub-menu.
 */
void
GuiMainWindowLayersMenu::createContoursSubMenu(GuiMainWindowLayersActions* layersActions)
{
   contoursSubMenu = addMenu("Contours");
   
   contoursSubMenu->addAction(layersActions->getContourNewSetAction());
   contoursSubMenu->addSeparator();

   contoursSubMenu->addAction(layersActions->getContourInformationAction());
   contoursSubMenu->addSeparator();
   
   contoursSubMenu->addAction(layersActions->getContourSetScaleAction());
   contoursSubMenu->addAction(layersActions->getContourSectionsAction());
   contoursSubMenu->addAction(layersActions->getContourSpacingAction());
   contoursSubMenu->addSeparator();

   contoursSubMenu->addAction(layersActions->getContourDrawAction());
   contoursSubMenu->addSeparator();
                            
   contoursSubMenu->addAction(layersActions->getContourApplyCurrentViewAction());
   contoursSubMenu->addAction(layersActions->getContourAlignAction());
   contoursSubMenu->addAction(layersActions->getContourMergeAction());
   contoursSubMenu->addAction(layersActions->getContourMovePointAction());
   contoursSubMenu->addAction(layersActions->getContourResampleAction());
   contoursSubMenu->addAction(layersActions->getContourReverseAction());
   contoursSubMenu->addSeparator();
                            
   contoursSubMenu->addAction(layersActions->getContourCleanUpAction());
   contoursSubMenu->addAction(layersActions->getContourDeleteAllAction());
   contoursSubMenu->addAction(layersActions->getContourDeleteAction());
   contoursSubMenu->addAction(layersActions->getContourDeletePointAction());
   contoursSubMenu->addSeparator();
                            
   contoursSubMenu->addAction(layersActions->getContourReconstructAction());
}

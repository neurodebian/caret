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

#include <QApplication>
#include <QMenu>

#include "ArealEstimationFile.h"
#include "BrainSet.h"
#include "BrainModelBorderSet.h"
#include "BrainModelContours.h"
#include "BrainModelIdentification.h"
#include "BrainModelSurface.h"
#include "CellProjectionFile.h"
#include "ContourCellFile.h"
#include "CutsFile.h"
#include "DisplaySettingsBorders.h"
#include "DisplaySettingsCells.h"
#include "DisplaySettingsContours.h"
#include "DisplaySettingsCuts.h"
#include "DisplaySettingsFoci.h"
#include "DisplaySettingsSurface.h"
#include "FociProjectionFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelOpenGLPopupMenu.h"
#include "GuiIdentifyDialog.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowLayersActions.h"
#include "GuiMainWindowLayersMenu.h"
#include "GuiMainWindowSurfaceActions.h"
#include "GuiMapStereotaxicFocusDialog.h"
#include "GuiToolBar.h"
#include "GuiSetViewDialog.h"
#include "PaintFile.h"
#include "ProbabilisticAtlasFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiBrainModelOpenGLPopupMenu::GuiBrainModelOpenGLPopupMenu(GuiBrainModelOpenGL* brainModelOpenGLIn)
   : QMenu(0)
{
   QObject::connect(this, SIGNAL(aboutToShow()),
                    this, SLOT(slotAboutToShow()));
         
   brainModelOpenGL = brainModelOpenGLIn;
   selectedItems.clear();
   //QT4setCheckable(true);
   setSelectedItems(selectedItems);
}

/**
 * Destructor.
 */
GuiBrainModelOpenGLPopupMenu::~GuiBrainModelOpenGLPopupMenu()
{
}

/**
 * create the color key sub menu.
 */
void 
GuiBrainModelOpenGLPopupMenu::createColorKeySubMenu()
{
   BrainSet* bs = brainModelOpenGL->getBrainSet();
   const bool haveArealEst = (bs->getArealEstimationFile()->empty() == false);
   const bool haveBorders  = (bs->getBorderSet()->getNumberOfBorders() > 0);
   const bool haveCells = (bs->getCellProjectionFile()->getNumberOfCellProjections() > 0);
   const bool haveFoci  = (bs->getFociProjectionFile()->getNumberOfCellProjections() > 0);
   const bool havePaint = (bs->getPaintFile()->empty() == false);
   const bool haveProbAtlas = (bs->getProbabilisticAtlasSurfaceFile()->empty() == false);
   const bool havePaintVolume = (bs->getNumberOfVolumePaintFiles() > 0);
   const bool haveProbAtlasVolume = (bs->getNumberOfVolumeProbAtlasFiles() > 0);
   
   if (haveArealEst ||
       haveBorders ||
       haveCells ||
       haveFoci ||
       havePaint ||
       haveProbAtlas ||
       havePaintVolume ||
       haveProbAtlasVolume) {
       
      QMenu* colorKeyMenu = addMenu("Color Keys");
      
      if (haveArealEst) {
         colorKeyMenu->addAction("Areal Estimation", theMainWindow, SLOT(displayArealEstimationColorKey()));
      }
      if (haveBorders) {
         colorKeyMenu->addAction("Borders", theMainWindow, SLOT(displayBorderColorKey()));
      }
      if (haveCells) {
         colorKeyMenu->addAction("Cells", theMainWindow, SLOT(displayCellColorKey()));
      }
      if (haveFoci) {
         colorKeyMenu->addAction("Foci", theMainWindow, SLOT(displayFociColorKey()));
      }
      if (havePaint) {
         colorKeyMenu->addAction("Paint", theMainWindow, SLOT(displayPaintColorKey()));
      }
      if (haveProbAtlas) {
         colorKeyMenu->addAction("Probabilistic Atlas", theMainWindow, SLOT(displayProbabilisticAtlasColorKey()));
      }
      if (havePaintVolume) {
         colorKeyMenu->addAction("Volume Paint", theMainWindow, SLOT(displayVolumePaintColorKey()));
      }
      if (haveProbAtlasVolume) {
         colorKeyMenu->addAction("Volume Probabilistic Atlas", theMainWindow, SLOT(displayVolumeProbabilisticAtlasColorKey()));
      }
   }
}
      
/**
 * Create the brain model sub menu
 */
void
GuiBrainModelOpenGLPopupMenu::createBrainModelSubMenu()
{
   brainModelSubMenu = NULL;
   
   const int numModels = brainModelOpenGL->getBrainSet()->getNumberOfBrainModels();
   if (numModels > 0) {
      brainModelSubMenu = addMenu("Show Brain Model");
      QObject::connect(brainModelSubMenu, SIGNAL(triggered(QAction*)),
                       this, SLOT(displayBrainModel(QAction*)));
      
      //
      // Insert view mode
      //
      for (int i = 0; i < numModels; i++) {
         const BrainModel* bm = brainModelOpenGL->getBrainSet()->getBrainModel(i);
         QAction* action = brainModelSubMenu->addAction(bm->getDescriptiveName());
         action->setData(i);
      }
   }
}

/**
 * called to display a brain model.
 */
void 
GuiBrainModelOpenGLPopupMenu::displayBrainModel(QAction* action)
{
   const int item = action->data().toInt();
      
   if (item >= 0) {
      brainModelOpenGL->setDisplayedBrainModelIndex(item);

      //
      // Update the toolbars
      //
      GuiToolBar::updateAllToolBars(false);
               
      brainModelOpenGL->updateGL();
   }
}
      
/**
 * Create the mouse mode sub menu.
 */
void
GuiBrainModelOpenGLPopupMenu::createMouseModeSubMenu()
{
   GuiMainWindowLayersActions* layersActions = theMainWindow->getLayersActions();
   GuiMainWindowSurfaceActions* surfaceActions = theMainWindow->getSurfaceActions();
   const BrainModelContours* contours = brainModelOpenGL->getDisplayedBrainModelContours();
   const BrainModelSurface* surface = brainModelOpenGL->getDisplayedBrainModelSurface();
  // const bool volumeDisplayed   = (brainModelOpenGL->getDisplayedBrainModelVolume() != NULL);
   
   bool bordersDisplayed = false;
   bool cellsDisplayed   = false;
   bool cutsDisplayed    = false;
   bool fociDisplayed    = false;
   BrainModelSurface::SURFACE_TYPES surfaceType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
   if (surface != NULL) {
      surfaceType = surface->getSurfaceType();
      
      DisplaySettingsBorders* dsb = brainModelOpenGL->getBrainSet()->getDisplaySettingsBorders();
      if (dsb->getDisplayBorders()) {
         BrainModelBorderSet* bmbs = brainModelOpenGL->getBrainSet()->getBorderSet();
         BorderFile bf;
         bmbs->copyBordersToBorderFile(surface, bf);
         if (bf.getNumberOfBorders() > 0) {
            bordersDisplayed = true;
         }
      }
      
      DisplaySettingsCells* dsc = brainModelOpenGL->getBrainSet()->getDisplaySettingsCells();
      if (dsc->getDisplayCells()) {
         const CellProjectionFile* cf = brainModelOpenGL->getBrainSet()->getCellProjectionFile();
         if (cf != NULL) {
            if (cf->getNumberOfCellProjections() > 0) {
               cellsDisplayed = true;
            }
         }
      }

      DisplaySettingsFoci* dsf = brainModelOpenGL->getBrainSet()->getDisplaySettingsFoci();
      if (dsf->getDisplayCells()) {
         const FociProjectionFile* ff = brainModelOpenGL->getBrainSet()->getFociProjectionFile();
         if (ff != NULL) {
            if (ff->getNumberOfCellProjections()) {
               fociDisplayed = true;
            }
         }
      }
      
      DisplaySettingsCuts* dst = brainModelOpenGL->getBrainSet()->getDisplaySettingsCuts();
      if (dst->getDisplayCuts()) {
         const CutsFile * cf = brainModelOpenGL->getBrainSet()->getCutsFile();
         if (cf != NULL) {
            if (cf->getNumberOfBorders() > 0) {
               cutsDisplayed = true;
            }
         }
      }
   }
   
   QMenu* mouseMenu = addMenu("Set Mouse Mode");

   //
   // Insert view mode
   //
   mouseMenu->addAction("View Mode", this, SLOT(slotViewMode()));

   //
   // Insert border items
   //
   if (surface != NULL) {
      mouseMenu->addSeparator();
      mouseMenu->addAction(layersActions->getBordersDrawAction());
      if (bordersDisplayed) {
         mouseMenu->addAction(layersActions->getBordersDeleteWithMouseAction());
         mouseMenu->addAction(layersActions->getBordersDeletePointWithMouseAction());
         mouseMenu->addAction(layersActions->getBordersMovePointWithMouseAction());
         mouseMenu->addAction(layersActions->getBordersRenameWithMouseAction());
         mouseMenu->addAction(layersActions->getBordersReverseWithMouseAction());
      }
   }
   
   //
   // Insert cell items
   //
   if (surface != NULL) {
      mouseMenu->addSeparator();
      mouseMenu->addAction(layersActions->getCellsAddAction());
      if (cellsDisplayed) {
         mouseMenu->addAction(layersActions->getCellsDeleteUsingMouseAction());
      }
   }
   
   if (contours != NULL) {
      mouseMenu->addSeparator();
      DisplaySettingsContours* dsc = brainModelOpenGL->getBrainSet()->getDisplaySettingsContours();
      const ContourFile* cf = contours->getContourFile();
      const ContourCellFile* cells = brainModelOpenGL->getBrainSet()->getContourCellFile();
      mouseMenu->addAction(layersActions->getContourDrawAction());
      if (cf->getNumberOfContours() > 0) {
         mouseMenu->addAction(layersActions->getContourDeleteAction());
         mouseMenu->addAction(layersActions->getContourMergeAction());
         mouseMenu->addAction(layersActions->getContourMovePointAction());
         mouseMenu->addAction(layersActions->getContourDeletePointAction());
         mouseMenu->addAction(layersActions->getContourReverseAction());
      }
      
      mouseMenu->addAction(layersActions->getContourCellsAddAction());
      if (cells != NULL) {
         if (cells->getNumberOfCells() > 0) {
            if (dsc->getDisplayContourCells()) {
               mouseMenu->addAction(layersActions->getContourCellsDeleteWithMouseAction());
               mouseMenu->addAction(layersActions->getContourCellsMoveWithMouseAction());
            }
         }
      }
   }
      
   //
   // Insert cut items
   //
   if (surface != NULL) {
      mouseMenu->addSeparator();
      mouseMenu->addAction(surfaceActions->getCutsDrawAction());
      if (cutsDisplayed) {
         mouseMenu->addAction(surfaceActions->getCutsDeleteWithMouseAction());
      }
   }
   
   //
   // Insert foci items
   //
   if (fociDisplayed) {
      mouseMenu->addSeparator();
      mouseMenu->addAction(layersActions->getFociDeleteUsingMouseAction());
   }
   
   if (surface != NULL) {
      mouseMenu->addSeparator();
      mouseMenu->addAction("Transformation Axes", this, SLOT(slotTransformationAxes()));
   }
}

/**
 * called for transformation axes.
 */
void 
GuiBrainModelOpenGLPopupMenu::slotTransformationAxes()
{
   brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_TRANSFORMATION_MATRIX_AXES);
}

/**
 * caled for view mode.
 */
void 
GuiBrainModelOpenGLPopupMenu::slotViewMode()
{
   brainModelOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
}

/**
 * Called when this menu is about to be popped up
 */
void
GuiBrainModelOpenGLPopupMenu::slotAboutToShow()
{
   clear();

   GuiMainWindowLayersActions* layersActions = theMainWindow->getLayersActions();

   //
   // Only main window gets mouse modes
   //
   if (brainModelOpenGL->isMainWindowOpenGL()) {
      createMouseModeSubMenu();
   }
   
   //
   // Create the brain model sub menu
   //
   createBrainModelSubMenu();

   //
   // Color Keys
   //   
   createColorKeySubMenu();
   
   const BrainModelSurface* surface = brainModelOpenGL->getDisplayedBrainModelSurface();

   addSeparator();
   addAction("Clear Node ID Symbols", this, SLOT(slotClearIDSymbols()));
   addAction(layersActions->getBordersClearHighlightingAction());
   addAction(layersActions->getFociClearHighlightingAction());
   
   if (selectedNode != NULL) {
      addAction("Identify Node Under Mouse", this, SLOT(slotNodeID()));
   }
   
   if (selectedBorder1 != NULL) {
      addAction("Identify Border Under Mouse", this, SLOT(slotBorderID()));
   }
   
   if (selectedCellProjection != NULL) {
      addAction("Identify Cell Under Mouse", this, SLOT(slotCellID()));
   }
   
   if (selectedFocusProjection != NULL) {
      addAction("Identify Focus Under Mouse", this, SLOT(slotFocusID()));
      if (brainModelOpenGL->isMainWindowOpenGL()) {
         const BrainModelSurface* bms = brainModelOpenGL->getDisplayedBrainModelSurface();
         if (bms != NULL) {
            if ((bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_RAW) ||
                (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL)) {
               addAction("Edit Focus Under Mouse", this, SLOT(slotFocusEdit()));
            }
         }
      }
   }
   
   if ((selectedBorder1 != NULL) || 
       (selectedCellProjection != NULL) ||
       (selectedFocusProjection != NULL)) {
      addSeparator();
      
      if (selectedBorder1 != NULL) {
         addAction("Delete Border Under Mouse", this, SLOT(slotBorderDelete()));
      }
      if (selectedCellProjection != NULL) {
         addAction("Delete Cell Under Mouse", this, SLOT(slotCellDelete()));
      }
      if (selectedFocusProjection != NULL) {
         addAction("Delete Focus Under Mouse", this, SLOT(slotFocusDelete()));
      }
   }

   //
   // Project border under mouse
   //
   if (selectedBorder1 != NULL) {
      addSeparator();
      addAction("Project Border Under Mouse", this, SLOT(slotBorderProject()));
   }
   
   //
   // Create the show submenu
   //
   if (surface != NULL) {
      bool addedMenuItem = false;
      BrainModelBorderSet* bmbs = brainModelOpenGL->getBrainSet()->getBorderSet();
      
      if ((bmbs->getNumberOfBorders() > 0) ||
          (brainModelOpenGL->getBrainSet()->getCellProjectionFile()->getNumberOfCellProjections() > 0) ||
          (brainModelOpenGL->getBrainSet()->getFociProjectionFile()->getNumberOfCellProjections() > 0)) {
         addSeparator();
      }
            
      if (bmbs->getNumberOfBorders() > 0) {
         QAction* borderAction = addAction("Show Borders", this, SLOT(slotShowBorders()));
         DisplaySettingsBorders* dsb = brainModelOpenGL->getBrainSet()->getDisplaySettingsBorders();
         borderAction->setCheckable(true);
         borderAction->setChecked(dsb->getDisplayBorders());
         addedMenuItem = true;
      }
      if (brainModelOpenGL->getBrainSet()->getCellProjectionFile()->getNumberOfCellProjections() > 0) {
         QAction* cellAction = addAction("Show Cells", this, SLOT(slotShowCells()));
         DisplaySettingsCells* dsc = brainModelOpenGL->getBrainSet()->getDisplaySettingsCells();
         cellAction->setCheckable(true);
         cellAction->setChecked(dsc->getDisplayCells());
         addedMenuItem = true;
      }
      if (brainModelOpenGL->getBrainSet()->getFociProjectionFile()->getNumberOfCellProjections() > 0) {
         QAction* fociAction = addAction("Show Foci", this, SLOT(slotShowFoci()));
         DisplaySettingsCells* dsc = brainModelOpenGL->getBrainSet()->getDisplaySettingsFoci();
         fociAction->setCheckable(true);
         fociAction->setChecked(dsc->getDisplayCells());
         addedMenuItem = true;
      }
   }   
}

/**
 * called to remove ID symbols.
 */
void 
GuiBrainModelOpenGLPopupMenu::slotClearIDSymbols()
{
   brainModelOpenGL->getBrainSet()->clearNodeHighlightSymbols();
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Called when a border ID is selected.
 */
void
GuiBrainModelOpenGLPopupMenu::slotBorderID()
{
   if (selectedBorder1 != NULL) {
      BrainModelIdentification* bmi = brainModelOpenGL->getBrainSet()->getBrainModelIdentification();

      const QString idString = bmi->getIdentificationTextForBorder(brainModelOpenGL->getOpenGLDrawing(),
                                                                 true,
                                                                 true);
      if (idString.isEmpty() == false) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->appendHtml(idString);
      }
/*
      GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
      id->displayBorder(*selectedBorder1, brainModelOpenGL->getModelViewNumber());
      if (selectedBorder2 != NULL) {
         id->displayBorder(*selectedBorder2, brainModelOpenGL->getModelViewNumber());
      }
*/
   }
}
/**
 * Called when a border delete is selected.
 */
void
GuiBrainModelOpenGLPopupMenu::slotBorderDelete()
{
   if (selectedBorder1 != NULL) {
      BrainModelBorderSet* bmbs = brainModelOpenGL->getBrainSet()->getBorderSet();
      bmbs->deleteBorder(selectedBorder1->getItemIndex2());
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * called when a border is projected.
 */
void 
GuiBrainModelOpenGLPopupMenu::slotBorderProject()
{
   if (selectedBorder1 != NULL) {
      BrainModelBorderSet* bmbs = brainModelOpenGL->getBrainSet()->getBorderSet();
      BrainModelSurface* bms = brainModelOpenGL->getDisplayedBrainModelSurface();
      bmbs->projectBorders(bms,
                           true,
                           selectedBorder1->getItemIndex2(),
                           selectedBorder1->getItemIndex2());
      GuiBrainModelOpenGL::updateAllGL();
   }
}
      
/**
 * Called when a cell ID is selected.
 */
void
GuiBrainModelOpenGLPopupMenu::slotCellID()
{
   if (selectedCellProjection != NULL) {
      BrainModelIdentification* bmi = brainModelOpenGL->getBrainSet()->getBrainModelIdentification();

      const QString idString = bmi->getIdentificationTextForCell(brainModelOpenGL->getOpenGLDrawing(),
                                                                 true,
                                                                 true);
      if (idString.isEmpty() == false) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->appendHtml(idString);
      }
      //GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
      //id->displayCell(*selectedCellProjection, brainModelOpenGL->getModelViewNumber());
   }
}
/**
 * Called when a cell delete is selected.
 */
void
GuiBrainModelOpenGLPopupMenu::slotCellDelete()
{
   if (selectedCellProjection != NULL) {
      brainModelOpenGL->getBrainSet()->deleteCell(selectedCellProjection->getItemIndex1());
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * Called when a focus ID is selected.
 */
void
GuiBrainModelOpenGLPopupMenu::slotFocusID()
{
   if (selectedFocusProjection != NULL) {
      BrainModelIdentification* bmi = brainModelOpenGL->getBrainSet()->getBrainModelIdentification();

      const QString idString = bmi->getIdentificationTextForFocus(brainModelOpenGL->getOpenGLDrawing(),
                                                                 true,
                                                                 true);
      if (idString.isEmpty() == false) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->appendHtml(idString);
      }
      //GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
      //id->displayFocus(brainModelOpenGL->getDisplayedBrainModelSurface(),
      //                 *selectedFocusProjection, 
      //                 brainModelOpenGL->getModelViewNumber());
   }
}

/**
 * Called when a focus delete is selected.
 */
void
GuiBrainModelOpenGLPopupMenu::slotFocusDelete()
{
   if (selectedFocusProjection != NULL) {
      brainModelOpenGL->getBrainSet()->deleteFocus(selectedFocusProjection->getItemIndex1());
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * Called when a focus edit is selected.
 */
void
GuiBrainModelOpenGLPopupMenu::slotFocusEdit()
{
   if (selectedFocusProjection != NULL) {
      GuiMapStereotaxicFocusDialog* sfd = theMainWindow->getMapStereotaxicFocusDialog(true);
      sfd->editFocus(selectedFocusProjection->getItemIndex1());
   }
}

/**
 * Called to ID a node
 */
void
GuiBrainModelOpenGLPopupMenu::slotNodeID()
{
   if (selectedNode != NULL) {
      const int nodeNum = selectedNode->getItemIndex1();
      if (nodeNum >= 0) {
         BrainSetNodeAttribute* bna = brainModelOpenGL->getBrainSet()->getNodeAttributes(nodeNum);
         if (bna->getHighlighting() == BrainSetNodeAttribute::HIGHLIGHT_NODE_NONE) {
            bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
            //GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
            //BrainModelSurface* surface = brainModelOpenGL->getDisplayedBrainModelSurface();
            //id->displayNode(*selectedNode, brainModelOpenGL->getModelViewNumber(), surface);
            
            BrainModelIdentification* bmi = brainModelOpenGL->getBrainSet()->getBrainModelIdentification();

            const QString idString = bmi->getIdentificationTextForNode(brainModelOpenGL->getOpenGLDrawing(),
                                                                       true,
                                                                       true);
            if (idString.isEmpty() == false) {
               GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
               id->appendHtml(idString);
            }
         }
         else {
            bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_NONE);
         }
         brainModelOpenGL->getBrainSet()->clearAllDisplayLists();
      }
   }

   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * called to show/not show borders.
 */
void 
GuiBrainModelOpenGLPopupMenu::slotShowBorders()
{
   DisplaySettingsBorders* dsb = brainModelOpenGL->getBrainSet()->getDisplaySettingsBorders();
   dsb->setDisplayBorders(! dsb->getDisplayBorders());
   dsb->determineDisplayedBorders();
   theMainWindow->updateDisplayControlDialog();  
   //GuiBrainModelOpenGL::updateAllGL();
}

/**
 * called to show/not show cells.
 */
void 
GuiBrainModelOpenGLPopupMenu::slotShowCells()
{
   DisplaySettingsCells* dsc = brainModelOpenGL->getBrainSet()->getDisplaySettingsCells();
   dsc->setDisplayCells(! dsc->getDisplayCells());
   theMainWindow->updateDisplayControlDialog();
}

/**
 * called to show/not show foci.
 */
void 
GuiBrainModelOpenGLPopupMenu::slotShowFoci()
{
   DisplaySettingsCells* dsc = brainModelOpenGL->getBrainSet()->getDisplaySettingsFoci();
   dsc->setDisplayCells(! dsc->getDisplayCells());
   theMainWindow->updateDisplayControlDialog();
}

/**
 * set the selected items.
 */
void 
GuiBrainModelOpenGLPopupMenu::setSelectedItems(const std::vector<BrainModelOpenGLSelectedItem>& itemsIn)
{
   selectedItems = itemsIn;
   
   selectedNode = NULL;
   selectedBorder1 = NULL;
   selectedBorder2 = NULL;
   selectedCellProjection = NULL;
   selectedCut = NULL;
   selectedFocusProjection = NULL;
   selectedPaletteMetric = NULL;
   selectedPaletteShape = NULL;
   selectedContour = NULL;
   selectedContourCell = NULL;
   selectedVoxelUnderlay = NULL;
   selectedVoxelOverlaySecondary = NULL;
   selectedVoxelOverlayPrimary = NULL;
   selectedVoxelFunctionalCloud = NULL;
   selectedTransformationAxes = NULL;
   
   for (unsigned int i = 0; i < selectedItems.size(); i++) {
      switch (selectedItems[i].getItemType()) {
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_NONE:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_TILE:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_LINK:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_NODE:
            selectedNode = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_BORDER:
            if (selectedBorder1 == NULL) {
               selectedBorder1 = &selectedItems[i];
            }
            else if (selectedBorder2 == NULL) {
               selectedBorder2 = &selectedItems[i];
            }
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_VOLUME_BORDER:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_BORDER_PROJ:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_CELL_PROJECTION:
            selectedCellProjection = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_VOLUME_CELL:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_CONTOUR:
            selectedContour = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_CONTOUR_CELL:
            selectedContourCell = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_CUT:
            selectedCut = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_FOCUS_PROJECTION:
            selectedFocusProjection = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_VOLUME_FOCI:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_PALETTE_METRIC:
            selectedPaletteMetric = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_PALETTE_SHAPE:
            selectedPaletteShape = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_VOXEL_UNDERLAY:
            selectedVoxelUnderlay = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_VOXEL_OVERLAY_SECONDARY:
            selectedVoxelOverlaySecondary = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_VOXEL_OVERLAY_PRIMARY:
            selectedVoxelOverlayPrimary = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_VOXEL_FUNCTIONAL_CLOUD:
            selectedVoxelFunctionalCloud = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_TRANSFORMATION_MATRIX_AXES:
            selectedTransformationAxes   = &selectedItems[i];
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_VTK_MODEL:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_TRANSFORM_CELL:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_TRANSFORM_FOCI:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_TRANSFORM_CONTOUR:
            break;
         case BrainModelOpenGLSelectedItem::ITEM_TYPE_TRANSFORM_CONTOUR_CELL:
            break;
      }
   }
}


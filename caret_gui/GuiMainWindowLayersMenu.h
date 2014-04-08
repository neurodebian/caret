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


#ifndef __GUI_MAIN_WINDOW_LAYERS_MENU_H__
#define __GUI_MAIN_WINDOW_LAYERS_MENU_H__

#include <QMenu>

#include "GuiMainWindowLayersActions.h"

class GuiMainWindow;

/// This class constructs the layers menu
class GuiMainWindowLayersMenu : public QMenu {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMainWindowLayersMenu(GuiMainWindow* parent);
      
      /// Destructor
      ~GuiMainWindowLayersMenu();
      
   protected:
      /// Create the border sub menu
      void createBordersSubMenu(GuiMainWindowLayersActions* layersActions);
      
      /// Create the foci sub menu
      void createFociSubMenu(GuiMainWindowLayersActions* layersActions);
      
      /// Create the cells sub menu
      void createCellsSubMenu(GuiMainWindowLayersActions* layersActions);
      
      /// Create the contours sub menu
      void createContoursSubMenu(GuiMainWindowLayersActions* layersActions);
      
      /// Create the contour cells sub menu
      void createContourCellsSubMenu(GuiMainWindowLayersActions* layersActions);
      
      /// Borders sub menu
      QMenu* bordersSubMenu;
      
      /// Cells sub menu
      QMenu* cellsSubMenu;
      
      /// Foci sub menu
      QMenu* fociSubMenu;
      
      /// Contours sub menu
      QMenu* contoursSubMenu;
      
      /// contours new set menu id
      int contourNewSetID;
      
      /// contours set scale menu id
      int contourSetScaleID;
      
      /// contours section menu id
      int contourSectionID;
      
      /// contours spacing menu id
      int contoursSpacingID;
      
      /// contours draw menu id
      int contoursDrawID;
      
      /// contours apply view menu id
      int contoursApplyViewID;
      
      /// contours align menu id
      int contoursAlignID;
      
      /// contours merge menu id
      int contourMergeID;
      
      /// contours move point menu id
      int contoursMovePointID;
      
      /// contours delete all menu id
      int contoursDeleteAllID;
      
      /// contours delete menu id
      int contoursDeleteID;
      
      /// contours delete point menu id
      int contoursDeletePointID;
      
      /// contours reconstruct menu id
      int contoursReconstructID;
      
      /// contour cells add menu id
      int contourCellsAddID;
      
      /// contour cells delete all menu id
      int contourCellsDeleteAllID;
      
      /// contour cells delete menu id
      int contourCellsDeleteWithMouseID;
      
      /// contour cells edit colors menu id
      int contourCellsEditColorsID;
      
      /// contour cells move menu id
      int contourCellsMoveWithMouseID;
      
      /// contour cells sub menu
      QMenu* contourCellsSubMenu;
};

#endif  // __GUI_MAIN_WINDOW_LAYERS_MENU_H__


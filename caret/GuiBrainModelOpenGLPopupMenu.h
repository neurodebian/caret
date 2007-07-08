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

#ifndef __GUI_BRAIN_MODEL_OPENGL_POPUP_MENU_H__
#define __GUI_BRAIN_MODEL_OPENGL_POPUP_MENU_H__

#include <vector>

#include <QMenu>

#include "BrainModelOpenGLSelectedItem.h"

#include "GuiBrainModelOpenGL.h"

/// Menu popped up with right click in OpenGL widget
class GuiBrainModelOpenGLPopupMenu : public QMenu {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiBrainModelOpenGLPopupMenu(GuiBrainModelOpenGL* brainModelOpenGLIn);
      
      /// Destructor
      ~GuiBrainModelOpenGLPopupMenu();
      
      /// set the selected items
      void setSelectedItems(const std::vector<BrainModelOpenGLSelectedItem>& itemsIn);
      
   private slots:
      /// caled for view mode
      void slotViewMode();
      
      /// called for transformation axes
      void slotTransformationAxes();
      
      /// called to remove ID symbols
      void slotClearIDSymbols();
      
      /// called when this menu is about to popup
      void slotAboutToShow();
      
      /// called to id a node
      void slotNodeID();
      
      /// called when a border ID is selected
      void slotBorderID();
      
      /// called when a border delete is selected
      void slotBorderDelete();
      
      /// called when a border is projected
      void slotBorderProject();
      
      /// called when a cell ID is selected
      void slotCellID();
      
      /// called when a cell delete is selected
      void slotCellDelete();
      
      /// called when a focus ID is selected
      void slotFocusID();
      
      /// called when a focus delete is selected
      void slotFocusDelete();
      
      /// called when focus edit is selected
      void slotFocusEdit();
      
      /// called to show/not show borders
      void slotShowBorders();
      
      /// called to show/not show cells
      void slotShowCells();
      
      /// called to show/not show foci
      void slotShowFoci();
      
      /// called to display a brain model
      void displayBrainModel(QAction* action);
      
   private:
      /// Create the brain model sub menu
      void createBrainModelSubMenu();
      
      /// create the mouse mode sub menu
      void createMouseModeSubMenu();
      
      /// create the color key sub menu
      void createColorKeySubMenu();
      
      /// parent brain model opengl widget
      GuiBrainModelOpenGL* brainModelOpenGL;
      
      /// selected items
      std::vector<BrainModelOpenGLSelectedItem> selectedItems;

       /// selected node
       BrainModelOpenGLSelectedItem *selectedNode;
       
       /// selected border (first)
       BrainModelOpenGLSelectedItem *selectedBorder1;
       
       /// selected border (second)
       BrainModelOpenGLSelectedItem *selectedBorder2;
       
       /// selected cell
       BrainModelOpenGLSelectedItem *selectedCellProjection;
       
       /// selected cut
       BrainModelOpenGLSelectedItem *selectedCut;
       
       /// selected foci
       BrainModelOpenGLSelectedItem *selectedFocusProjection;
       
       /// selected palette metric
       BrainModelOpenGLSelectedItem *selectedPaletteMetric;
       
       /// selected palette shape
       BrainModelOpenGLSelectedItem *selectedPaletteShape;
       
       /// selected contour
       BrainModelOpenGLSelectedItem *selectedContour;
       
       /// selected contour cell
       BrainModelOpenGLSelectedItem *selectedContourCell;
       
       /// selected voxel underlay
       BrainModelOpenGLSelectedItem *selectedVoxelUnderlay;
       
       /// selected voxel overlay secondary
       BrainModelOpenGLSelectedItem *selectedVoxelOverlaySecondary;
       
       /// selected voxel overlay primary
       BrainModelOpenGLSelectedItem *selectedVoxelOverlayPrimary;
       
       /// selected voxel functiona cloud
       BrainModelOpenGLSelectedItem* selectedVoxelFunctionalCloud;
       
       /// selected transformation axes
       BrainModelOpenGLSelectedItem* selectedTransformationAxes;
        
       /// the brain model sub menu
       QMenu* brainModelSubMenu;
};

#endif // __GUI_BRAIN_MODEL_OPENGL_POPUP_MENU_H__


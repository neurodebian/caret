
#ifndef __GUI_MOUSE_MODE_POPUP_MENU_H__
#define __GUI_MOUSE_MODE_POPUP_MENU_H__

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

#include <QMenu>

class GuiBrainModelOpenGL;

/// class for a popup menu that sets the mouse mode
class GuiMouseModePopupMenu : public QMenu {
   Q_OBJECT
   
   public:
      // constructor
      GuiMouseModePopupMenu(GuiBrainModelOpenGL* brainModelOpenGLIn,
                            const bool showViewModeIn,
                            QWidget* parent = 0);
      
      // destructor
      ~GuiMouseModePopupMenu();
      
   protected slots:
      // called when menu is about to be displayed
      void slotAboutToShow();
      
      // called when menu item selected
      void slotMenuItemSelected(QAction*);
      
   protected:
      // brain model OpenGL for which this is used
      GuiBrainModelOpenGL* brainModelOpenGL;      
      
      // show view mode
      bool showViewMode;
      
      // name of action
      static QString getActionBorderDrawName() { return "Border Draw"; }
      
      // name of action
      static QString getActionBorderDeleteName() { return "Border Delete"; }
      
      // name of action
      static QString getActionBorderRenameName() { return "Border Rename"; }
      
      // name of action
      static QString getActionBorderReversePointOrderName() { return "Border Reverse Point Order"; }
      
      // name of action
      static QString getActionBorderPointDeleteName() { return "Border Point Delete"; }
      
      // name of action
      static QString getActionBorderPointMoveName() { return "Border Point Move"; }
      
      // name of action
      static QString getActionCellAddName() { return "Cell Add"; }
      
      // name of action
      static QString getActionCellDeleteName() { return "Cell Delete"; }
      
      // name of action
      static QString getActionContourDrawName() { return "Contour Draw"; }
      
      // name of action
      static QString getActionContourDeleteName() { return "Contour Delete"; }
      
      // name of action
      static QString getActionContourMergeName() { return "Contour Merge"; }
      
      // name of action
      static QString getActionContourReversePointOrderName() { return "Contour Reverse Point Order"; }
      
      // name of action
      static QString getActionContourPointDeleteName() { return "Contour Point Delete"; }
      
      // name of action
      static QString getActionContourPointMoveName() { return "Contour Point Move"; }
      
      // name of action
      static QString getActionContourCellAddName() { return "Contour Cell Add"; }
      
      // name of action
      static QString getActionContourCellDeleteName() { return "Contour Cell Delete"; }
      
      // name of action
      static QString getActionContourCellMoveName() { return "Contour Cell Move"; }
      
      // name of action
      static QString getActionCutDrawName() { return "Cut Draw"; }
      
      // name of action
      static QString getActionCutDeleteName() { return "Cut Delete"; }
      
      // name of action
      static QString getActionFociDeleteName() { return "Foci Delete"; }
      
      // name of action
      static QString getActionVolumePaintEditName() { return "Volume Paint Edit"; }
      
      // name of action
      static QString getActionVolumeSegmentationEditName() { return "Volume Segmentation Edit"; }
      
      // name of action
      static QString getActionViewName() { return "View"; }
      
      // name of action
      static QString getActionTransformationAxesName() { return "Transformation Axes"; }
      
      // name of action
      //static QString getActionName() { return ""; }
      
};

#endif // __GUI_MOUSE_MODE_POPUP_MENU_H__

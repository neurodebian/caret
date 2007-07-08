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


#ifndef __GUI_MAIN_WINDOW_FILE_MENU_H__
#define __GUI_MAIN_WINDOW_FILE_MENU_H__

#include <vector>
#include <QMenu>

class GuiBrainModelOpenGL;
class GuiMainWindow;

/// This class creates the Main Window's File Menu.
class GuiMainWindowFileMenu : public QMenu {

   Q_OBJECT
   
   public:
      // constructor
      GuiMainWindowFileMenu(GuiMainWindow* parent, GuiBrainModelOpenGL* brainModelOpenGLIn);
      
      // destructor
      ~GuiMainWindowFileMenu();
   
   protected slots:
      // recent spec file menu about to show
      void recentSpecFileMenuAboutToBeDisplayedSlot();
      
      // called for recent spec file menu selection
      void recentSpecFileMenuSlot(QAction* action);
      
   protected:
      /// The main window this menu is attached to
      GuiMainWindow* mainWindow;
      
      /// The brain model OpenGL displayed in the main window
      GuiBrainModelOpenGL* brainModelOpenGL;
      
      /// recent spec files menu
      QMenu* recentSpecFileMenu;
};

#endif  // __GUI_MAIN_WINDOW_FILE_MENU_H__


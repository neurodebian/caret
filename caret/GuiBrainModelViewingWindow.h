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


#ifndef __GUI_BRAIN_VIEWING_WINDOW_H__
#define __GUI_BRAIN_VIEWING_WINDOW_H__

#include "QtMainWindow.h"
#include "QtDialog.h"

class GuiMainWindow;
class GuiToolBar;

#include "GuiBrainModelOpenGL.h"

/// Main Window that is placed in the dialog to allow a toolbar
class GuiBrainModelViewingMainWindow : public QtMainWindow {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiBrainModelViewingMainWindow(QWidget* parent, 
                                     GuiMainWindow* caretMainWindow,
                                     const BrainModel::BRAIN_MODEL_VIEW_NUMBER svn,
                                     const char* name = 0);
      
      /// Destructor
      ~GuiBrainModelViewingMainWindow();
      
      /// initialize the toolbar
      void initializeToolBar();
      
      /// get the toolbar
      GuiToolBar* getToolBar() { return toolBar; }
      
   private:
      /// the toolbar
      GuiToolBar* toolBar;
      
      /// OpenGL Renderer
      GuiBrainModelOpenGL* brainModelOpenGL;
      
};

///
/// Dialog used to display additional brain models
///
class GuiBrainModelViewingWindow : public QtDialog {
   
   Q_OBJECT
   
   public:
      /// Constructor
      GuiBrainModelViewingWindow(QWidget* parent,
                          GuiMainWindow* caretMainWindowIn,
                          const BrainModel::BRAIN_MODEL_VIEW_NUMBER svn);
      
      /// Destructor
      ~GuiBrainModelViewingWindow();
      
      /// initialize the toolbar
      void initializeToolBar();
      
      /// display the brain model in the window
      void displayBrainModelInWindow(BrainModel* bm);
      
   private:
      /// the viewing main window
      GuiBrainModelViewingMainWindow* viewingMainWindow;
      
      /// the caret main window in this dialog
      GuiMainWindow* mainWindow;
      
      /// the window number
      BrainModel::BRAIN_MODEL_VIEW_NUMBER windowNumber; 
};

#endif

#ifndef __GUI_MAIN_WINDOW_WINDOW_ACTIONS_H__
#define __GUI_MAIN_WINDOW_WINDOW_ACTIONS_H__

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

#include <vector>

#include <QObject>

class GuiMainWindow;
class QAction;

/// the main window's window actions
class GuiMainWindowWindowActions : public QObject {
   Q_OBJECT
   
   public:
      // constructor
      GuiMainWindowWindowActions(GuiMainWindow* mainWindowParent);
      
      // destructor
      ~GuiMainWindowWindowActions();
   
      /// window 2 action
      QAction* getWindow2Action() { return window2Action; }
      
      /// window 3 action
      QAction* getWindow3Action() { return window3Action; }
      
      /// window 4 action
      QAction* getWindow4Action() { return window4Action; }
      
      /// window 5 action
      QAction* getWindow5Action() { return window5Action; }
      
      /// window 6 action
      QAction* getWindow6Action() { return window6Action; }
      
      /// window 7 action
      QAction* getWindow7Action() { return window7Action; }
      
      /// window 8 action
      QAction* getWindow8Action() { return window8Action; }
      
      /// window 9 action
      QAction* getWindow9Action() { return window9Action; }
      
      /// window 10 action
      QAction* getWindow10Action() { return window10Action; }
      
      /// resize viewing windows action
      QAction* getResizeViewWindowsAction() { return resizeViewWindowsAction; }
      
      /// display control window action
      QAction* getDisplayControlAction() { return displayControlAction; }
      
      /// id window action
      QAction* getIdWindowAction() { return idWindowAction; }
      
      /// image viewing window action
      QAction* getImageViewingWindowAction() { return imageViewingWindowAction; }

      /// display image editor action
      QAction* getDisplayImageEditorAction() { return displayImageEditorAction; }

      /// text file editor action
      QAction* getTextFileEditorAction() { return textFileEditorAction; }
      
      /// shell command window action
      QAction* getShellCommandWindowAction() { return shellCommandWindowAction; }
      
      /// transformation matrix editor action
      QAction* getTransformationMatrixEditorAction() { return transformationMatrixEditorAction; }

      /// caret command executor action
      QAction* getCaretCommandExecutorAction() { return caretCommandExecutorAction; }
      
      /// caret command script builder action
      QAction* getCaretCommandScriptBuilderAction() { return caretCommandScriptBuilderAction; }
      
      /// get stack windows action
      QAction* getStackWindowsAction() { return stackWindowsAction; }
      
      /// get the next window action
      QAction* getNextWindowAction() { return nextWindowAction; }
      
      /// bring all to front action
      QAction* getBringAllToFrontAction() { return bringAllToFrontAction; }
      
   public slots:
      /// update the actions (typically called when menu is about to show)
      void updateActions();

      /// Called when window 2 action selected
      void window2Popup();
            
      /// Called when window 3 action selected
      void window3Popup();
            
      /// Called when window 4 action selected
      void window4Popup();
            
      /// Called when window 5 action selected
      void window5Popup();
            
      /// Called when window 6 action selected
      void window6Popup();
            
      /// Called when window 7 action selected
      void window7Popup();
            
      /// Called when window 8 action selected
      void window8Popup();
            
      /// Called when window 9 action selected
      void window9Popup();
            
      /// Called when window 10 action selected
      void window10Popup();
                 
      /// Called when resize viewing window action selected
      void slotResizeViewingWindowsAction();
      
      /// text editor action
      void slotTextFileEditor();
      
      /// called when shell command action triggered
      void slotShellCommandWindow();
      
      /// called when stack windows action triggered
      void slotStackWindowsAction();
      
      /// called when next window action triggered
      void slotNextWindowAction();
      
      /// called when bring all to front is triggered
      void slotBringAllToFrontAction();
      
   protected:
      /// get all open windows that are children of main window
      static void getOpenWindowsThatAreChildrenOfMainWindow(std::vector<QWidget*>& windows,
                                                     const bool includeMainWindow);
      
      /// window 2 action
      QAction* window2Action;
      
      /// window 3 action
      QAction* window3Action;
      
      /// window 4 action
      QAction* window4Action;
      
      /// window 5 action
      QAction* window5Action;
      
      /// window 6 action
      QAction* window6Action;
      
      /// window 7 action
      QAction* window7Action;
      
      /// window 8 action
      QAction* window8Action;
      
      /// window 9 action
      QAction* window9Action;
      
      /// window 10 action
      QAction* window10Action;
      
      /// resize viewing windows action
      QAction* resizeViewWindowsAction;
      
      /// display control window action
      QAction* displayControlAction;
      
      /// id window action
      QAction* idWindowAction;
      
      /// image viewing window action
      QAction* imageViewingWindowAction;
      
      /// display image editor action
      QAction* displayImageEditorAction;
      
      /// text file editor action
      QAction* textFileEditorAction;
      
      /// shell command window action
      QAction* shellCommandWindowAction;
      
      /// transformation matrix editor action
      QAction* transformationMatrixEditorAction;
      
      /// caret command builder action
      QAction* caretCommandExecutorAction;
      
      /// caret command script builder action
      QAction* caretCommandScriptBuilderAction;
      
      /// stack windows action
      QAction* stackWindowsAction;
      
      /// next window action
      QAction* nextWindowAction;
      
      /// bring all to front action
      QAction* bringAllToFrontAction;
      
      /// last window from previous next window action
      QWidget* lastWindowFromPreviousNextAction;
};

#endif // __GUI_MAIN_WINDOW_WINDOW_ACTIONS_H__


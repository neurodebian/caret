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

#include <sstream>

#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>

#include "BrainSet.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowWindowActions.h"
#include "GuiShellCommandWindow.h"
#include "QtTextFileEditorDialog.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiMainWindowWindowActions::GuiMainWindowWindowActions(GuiMainWindow* mainWindowParent)
 : QObject(mainWindowParent)
{
   setObjectName("GuiMainWindowWindowActions");

   window2Action = new QAction(mainWindowParent);
   window2Action->setText("Viewing Window 2...");
   window2Action->setObjectName("window2Action");
   window2Action->setShortcut( Qt::CTRL+Qt::Key_2);
   QObject::connect(window2Action, SIGNAL(triggered(bool)),
                    this, SLOT(window2Popup()));

   window3Action = new QAction(mainWindowParent);
   window3Action->setText("Viewing Window 3...");
   window3Action->setObjectName("window3Action");
   window3Action->setShortcut( Qt::CTRL+Qt::Key_3);
   QObject::connect(window3Action, SIGNAL(triggered(bool)),
                    this, SLOT(window3Popup()));

   window4Action = new QAction(mainWindowParent);
   window4Action->setText("Viewing Window 4...");
   window4Action->setObjectName("window4Action");
   window4Action->setShortcut( Qt::CTRL+Qt::Key_4);
   QObject::connect(window4Action, SIGNAL(triggered(bool)),
                    this, SLOT(window4Popup()));

   window5Action = new QAction(mainWindowParent);
   window5Action->setText("Viewing Window 5...");
   window5Action->setObjectName("window5Action");
   window5Action->setShortcut( Qt::CTRL+Qt::Key_5);
   QObject::connect(window5Action, SIGNAL(triggered(bool)),
                    this, SLOT(window5Popup()));

   window6Action = new QAction(mainWindowParent);
   window6Action->setText("Viewing Window 6...");
   window6Action->setObjectName("window6Action");
   window6Action->setShortcut( Qt::CTRL+Qt::Key_6);
   QObject::connect(window6Action, SIGNAL(triggered(bool)),
                    this, SLOT(window6Popup()));

   window7Action = new QAction(mainWindowParent);
   window7Action->setText("Viewing Window 7...");
   window7Action->setObjectName("window7Action");
   window7Action->setShortcut( Qt::CTRL+Qt::Key_7);
   QObject::connect(window7Action, SIGNAL(triggered(bool)),
                    this, SLOT(window7Popup()));

   window8Action = new QAction(mainWindowParent);
   window8Action->setText("Viewing Window 8...");
   window8Action->setObjectName("window8Action");
   window8Action->setShortcut( Qt::CTRL+Qt::Key_8);
   QObject::connect(window8Action, SIGNAL(triggered(bool)),
                    this, SLOT(window8Popup()));

   window9Action = new QAction(mainWindowParent);
   window9Action->setText("Viewing Window 9...");
   window9Action->setObjectName("window9Action");
   window9Action->setShortcut( Qt::CTRL+Qt::Key_9);
   QObject::connect(window9Action, SIGNAL(triggered(bool)),
                    this, SLOT(window9Popup()));

   window10Action = new QAction(mainWindowParent);
   window10Action->setText("Viewing Window 10...");
   window10Action->setObjectName("window10Action");
   window10Action->setShortcut( Qt::CTRL+Qt::Key_0);
   QObject::connect(window10Action, SIGNAL(triggered(bool)),
                    this, SLOT(window10Popup()));

   resizeViewWindowsAction = new QAction(mainWindowParent);
   resizeViewWindowsAction->setText("Resize Viewing Windows...");
   resizeViewWindowsAction->setObjectName("resizeViewWindowsAction");
   QObject::connect(resizeViewWindowsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotResizeViewingWindowsAction()));

   displayControlAction = new QAction(mainWindowParent);
   displayControlAction->setText("Display Control Window...");
   displayControlAction->setObjectName("displayControlAction");
   displayControlAction->setShortcut(Qt::CTRL+Qt::Key_D);
   QObject::connect(displayControlAction, SIGNAL(triggered(bool)),
                    mainWindowParent, SLOT(displayDisplayControlDialog()));

   idWindowAction = new QAction(mainWindowParent);
   idWindowAction->setText("Identify Window...");
   idWindowAction->setObjectName("idWindowAction");
   idWindowAction->setShortcut(Qt::CTRL+Qt::Key_I);
   QObject::connect(idWindowAction, SIGNAL(triggered(bool)),
                    mainWindowParent, SLOT(displayIdentifyDialog()));

   imageViewingWindowAction = new QAction(mainWindowParent);
   imageViewingWindowAction->setText("New Image Viewing Window");
   imageViewingWindowAction->setObjectName("imageViewingWindowAction");
   QObject::connect(imageViewingWindowAction, SIGNAL(triggered(bool)),
                    mainWindowParent, SLOT(displayImageViewingWindow()));

   displayImageEditorAction = new QAction(mainWindowParent);
   displayImageEditorAction->setText("Image Editor...");
   displayImageEditorAction->setObjectName("displayImageEditorAction");
   QObject::connect(displayImageEditorAction, SIGNAL(triggered(bool)),
                    mainWindowParent, SLOT(displayImageEditorWindow()));

   textFileEditorAction = new QAction(mainWindowParent);
   textFileEditorAction->setText("Text File Editor...");
   textFileEditorAction->setObjectName("textFileEditorAction");
   QObject::connect(textFileEditorAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTextFileEditor()));


   transformationMatrixEditorAction = new QAction(mainWindowParent);
   transformationMatrixEditorAction->setText("Transformation Matrix Editor...");
   transformationMatrixEditorAction->setObjectName("transformationMatrixEditorAction");
   transformationMatrixEditorAction->setShortcut(Qt::CTRL+Qt::Key_T);
   QObject::connect(transformationMatrixEditorAction, SIGNAL(triggered(bool)),
                    mainWindowParent, SLOT(displayTransformMatrixEditor()));

   caretCommandExecutorAction = new QAction(mainWindowParent);
   caretCommandExecutorAction->setText("Caret Command Executor...");
   caretCommandExecutorAction->setObjectName("caretCommandExecutorAction");
   QObject::connect(caretCommandExecutorAction, SIGNAL(triggered(bool)),
                    mainWindowParent, SLOT(displayCaretCommandExecutorDialog()));

   caretCommandScriptBuilderAction = new QAction(mainWindowParent);
   caretCommandScriptBuilderAction->setText("Caret Command Script Builder...");
   caretCommandScriptBuilderAction->setObjectName("caretCommandScriptBuilderAction");
   QObject::connect(caretCommandScriptBuilderAction, SIGNAL(triggered(bool)),
                    mainWindowParent, SLOT(displayCaretCommandScriptBuilderDialog()));

   shellCommandWindowAction = new QAction(mainWindowParent);
   shellCommandWindowAction->setText("Shell Command Window...");
   shellCommandWindowAction->setObjectName("shellCommandWindowAction");
   QObject::connect(shellCommandWindowAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotShellCommandWindow()));
   
   stackWindowsAction = new QAction(mainWindowParent);
   stackWindowsAction->setText("Stack Windows");
   stackWindowsAction->setObjectName("stackWindowsAction");
   QObject::connect(stackWindowsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotStackWindowsAction()));
                    
   lastWindowFromPreviousNextAction = NULL;
   nextWindowAction = new QAction(mainWindowParent);
   nextWindowAction->setText("Next Window");
   nextWindowAction->setShortcut(Qt::CTRL+Qt::Key_F12);
   nextWindowAction->setObjectName("nextWindowAction");
   QObject::connect(nextWindowAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNextWindowAction()));
                    
   bringAllToFrontAction = new QAction(mainWindowParent);
   bringAllToFrontAction->setText("Bring All to Front");
   bringAllToFrontAction->setObjectName("bringAllToFrontAction");
   QObject::connect(bringAllToFrontAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBringAllToFrontAction()));
}

/**
 * destructor.
 */
GuiMainWindowWindowActions::~GuiMainWindowWindowActions()
{
}

/**
 * Popup surface 2 window
 */
void
GuiMainWindowWindowActions::window2Popup()
{
   theMainWindow->showViewingWindow(BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_2);
}

/**
 * Popup surface 3 window
 */
void
GuiMainWindowWindowActions::window3Popup()
{
   theMainWindow->showViewingWindow(BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_3);
}

/**
 * Popup surface 4 window
 */
void
GuiMainWindowWindowActions::window4Popup()
{
   theMainWindow->showViewingWindow(BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_4);
}

/**
 * Popup surface 5 window
 */
void
GuiMainWindowWindowActions::window5Popup()
{
   theMainWindow->showViewingWindow(BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_5);
}

/**
 * Popup surface 6 window
 */
void
GuiMainWindowWindowActions::window6Popup()
{
   theMainWindow->showViewingWindow(BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_6);
}

/**
 * Popup surface 7 window
 */
void
GuiMainWindowWindowActions::window7Popup()
{
   theMainWindow->showViewingWindow(BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_7);
}

/**
 * Popup surface 8 window
 */
void
GuiMainWindowWindowActions::window8Popup()
{
   theMainWindow->showViewingWindow(BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_8);
}

/**
 * Popup surface 9 window
 */
void
GuiMainWindowWindowActions::window9Popup()
{
   theMainWindow->showViewingWindow(BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_9);
}

/**
 * Popup surface 10 window
 */
void
GuiMainWindowWindowActions::window10Popup()
{
   theMainWindow->showViewingWindow(BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_10);
}

/**
 * Called when resize viewing window action selected.
 */
void 
GuiMainWindowWindowActions::slotResizeViewingWindowsAction()
{
   theMainWindow->resizeViewingWindows();
}
      
/**
 * Called to launch text editor.
 */
void 
GuiMainWindowWindowActions::slotTextFileEditor()
{
   QtTextFileEditorDialog* tfed = new QtTextFileEditorDialog(theMainWindow);
   QStringList caretFileFilters;
   FileFilters::getAllCaretFileFilters(caretFileFilters);
   tfed->addAdditionalFileFilters(caretFileFilters);
   tfed->setPreferencesFile(theMainWindow->getBrainSet()->getPreferencesFile());
   tfed->show();
}      

/**
 * called when shell command action triggered.
 */
void 
GuiMainWindowWindowActions::slotShellCommandWindow()
{
   static GuiShellCommandWindow* shellWindow = NULL;
   if (shellWindow == NULL) {
      shellWindow = new GuiShellCommandWindow(theMainWindow);
   }
   shellWindow->show();
   shellWindow->activateWindow();
}

/**
 * update the actions (typically called when menu is about to show)
 */
void 
GuiMainWindowWindowActions::updateActions()
{
   for (int i = BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_2; 
        i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
      const BrainModel::BRAIN_MODEL_VIEW_NUMBER windowNum = 
         static_cast<BrainModel::BRAIN_MODEL_VIEW_NUMBER>(i);
      std::ostringstream str;
      str << "Viewing Window "
          << (windowNum + 1);
          
      GuiBrainModelOpenGL* bmo = GuiBrainModelOpenGL::getBrainModelOpenGLForWindow(windowNum);
      if (bmo != NULL) {
         BrainModel* bm = bmo->getDisplayedBrainModel();
         if (bm != NULL) {
            str << " "
                << bm->getDescriptiveName().toAscii().constData();
         }
      }
      
      str << "...";
      
      QAction* action = NULL;
      switch (static_cast<BrainModel::BRAIN_MODEL_VIEW_NUMBER>(i)) {
         case BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW:
            action = NULL;
            break;
         case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_2:
            action = window2Action;
            break;
         case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_3:
            action = window3Action;
            break;
         case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_4:
            action = window4Action;
            break;
         case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_5:
            action = window5Action;
            break;
         case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_6:
            action = window6Action;
            break;
         case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_7:
            action = window7Action;
            break;
         case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_8:
            action = window8Action;
            break;
         case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_9:
            action = window9Action;
            break;
         case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_10:
            action = window10Action;
            break;
         case BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS:
            action = NULL;
            break;
      }
      
      if (action != NULL) {
         action->setText(str.str().c_str());
      }
   }
   
   imageViewingWindowAction->setEnabled(theMainWindow->getBrainSet()->getNumberOfImageFiles() > 0);
}

/**
 * called when stack windows action triggered.
 */
void 
GuiMainWindowWindowActions::slotStackWindowsAction()
{
   //
   // Screen width and height
   //
   QDesktopWidget* dt = QApplication::desktop();
   const int screenWidth = dt->width();
   const int screenHeight = dt->height();

   //
   // Position off of main window
   //
   const int increment = 25;
   int x = theMainWindow->x() + increment;
   int y = theMainWindow->y() + increment;
   
   //
   // Get the children of the main window
   //
   std::vector<QWidget*> dialogsAndWindows;
   getOpenWindowsThatAreChildrenOfMainWindow(dialogsAndWindows, false);
   
   //
   // Stack the windows with some offset
   //
   for (int i = 0; i < static_cast<int>(dialogsAndWindows.size()); i++) {
      //
      // Limit to valid screen position
      //
      x = std::min(x, screenWidth - increment);
      y = std::min(y, screenHeight - increment);
      
      //
      // Need to get rid of "constness" and set window location
      //
      QWidget* w = dialogsAndWindows[i];
      w->setGeometry(x, y, w->width(), w->height());
      w->show();
      w->activateWindow();
      
      //
      // Offset for next window
      //
      x += increment;
      y += increment;
   }
}      

/**
 * get all open windows that are children of main window.
 */
void 
GuiMainWindowWindowActions::getOpenWindowsThatAreChildrenOfMainWindow(std::vector<QWidget*>& windowsOut,
                                                const bool includeMainWindow)
{
   windowsOut.clear();
   
   if (includeMainWindow) {
      windowsOut.push_back(theMainWindow);
   }
   
   //
   // Get the children of the main window
   //
   const QObjectList listOfChildren = theMainWindow->children();
   for (int i = 0; i < listOfChildren.size(); i++) {
      const QObject* object = listOfChildren.at(i);
      const QWidget* widget = dynamic_cast<const QWidget*>(object);
      if (widget != NULL) {
         const bool isWindow = (dynamic_cast<const QMainWindow*>(widget) != NULL);
         const bool isDialog = (dynamic_cast<const QDialog*>(widget) != NULL);
         if (isWindow || isDialog) {
            if (DebugControl::getDebugOn()) {
               const QString hiddenString = (widget->isHidden() ? " hidden" : " displayed");
               std::cout << "Child of main window " << hiddenString.toAscii().constData()
                         << ": " 
                         << widget->windowTitle().toAscii().constData() << std::endl;
            }
             
            //
            // If not hidden (ie: closed)
            //
            if (widget->isHidden() == false) {
               QWidget* w = (QWidget*)widget;
               windowsOut.push_back(w);
            }
         }
      }
   }
}
      
/**
 * called when next window action triggered.
 */
void 
GuiMainWindowWindowActions::slotNextWindowAction()
{
   //
   // Get all open windows including main window
   //
   std::vector<QWidget*> windows;
   getOpenWindowsThatAreChildrenOfMainWindow(windows, true);
   
   //
   // Find the next window to activate
   //
   QWidget* windowToActivate = theMainWindow;
   for (int i = 0; i < static_cast<int>(windows.size()); i++) {
      if (windows[i] == lastWindowFromPreviousNextAction) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Previous window: "
                      << lastWindowFromPreviousNextAction->windowTitle().toAscii().constData() << std::endl;
         }
         int next = i + 1;
         if (next >= static_cast<int>(windows.size())) {
            next = 0;
         }
         windowToActivate = windows[next];
         break;
      }
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Activating window: "
                << windowToActivate->windowTitle().toAscii().constData() << std::endl;
   }
   
   windowToActivate->show();
   windowToActivate->activateWindow();
   lastWindowFromPreviousNextAction = windowToActivate;
}      

/**
 * called when bring all to front is triggered.
 */
void 
GuiMainWindowWindowActions::slotBringAllToFrontAction()
{
   //
   // Get all open windows including main window
   //
   std::vector<QWidget*> windows;
   getOpenWindowsThatAreChildrenOfMainWindow(windows, true);
   
   //
   // Show all windows
   //
   for (int i = 0; i < static_cast<int>(windows.size()); i++) {
      windows[i]->show();
      windows[i]->activateWindow();
   }
}      

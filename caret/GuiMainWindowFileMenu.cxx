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

#include <QGlobalStatic>

#include <iostream>

#include <QApplication>
#include <QDir>

#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowFileActions.h"
#include "GuiMainWindowFileMenu.h"

#include "global_variables.h"

/**
 * Constructor.
 */
GuiMainWindowFileMenu::GuiMainWindowFileMenu(GuiMainWindow* parent,
                                                   GuiBrainModelOpenGL* brainModelOpenGLIn) :
   QMenu("File", parent)
{
   mainWindow = parent;
   brainModelOpenGL = brainModelOpenGLIn;
   clear();
   
   GuiMainWindowFileActions* fileActions = mainWindow->getFileActions();
   
   QObject::connect(this, SIGNAL(aboutToShow()),
                    fileActions, SLOT(updateActions()));
                    
   // preferences (On Macs, QT will place this on the "caret" menu).
   addAction(fileActions->getShowPreferencesDialogAction());
#ifndef Q_OS_MACX
   addSeparator();
#endif
     
   addAction(fileActions->getSetCurrentDirectoryAction());
   addSeparator();
   
   addAction(fileActions->getConvertDataFileAction());
   addAction(fileActions->getCopySpecFileAction());
   addAction(fileActions->getValidateSpecFileAction());
   addAction(fileActions->getZipSpecFileAction());
   addSeparator();
   
   addAction(fileActions->getViewCurrentFilesAction());
   addSeparator();
   
   addAction(fileActions->getAddDocumentToSpecFileAction());
   addAction(fileActions->getOpenSpecFileAction());
   
   //
   // Recent spec file menu
   //
   recentSpecFileMenu = addMenu("Open Recent Spec File");
   QObject::connect(recentSpecFileMenu, SIGNAL(aboutToShow()),
                    this, SLOT(recentSpecFileMenuAboutToBeDisplayedSlot()));
   QObject::connect(recentSpecFileMenu, SIGNAL(triggered(QAction*)),
                    this, SLOT(recentSpecFileMenuSlot(QAction*)));
      
   addAction(fileActions->getCloseSpecFileAction());
              
   addSeparator();
   addAction(fileActions->getFastOpenDataFileAction());
   addAction(fileActions->getDataFileOpenAction());
   addAction(fileActions->getDataFileSaveAction());

   addSeparator();
   
   addAction(fileActions->getCaptureMainWindowImageAction());

   addAction(fileActions->getRecordAsMpegAction());
              
#ifndef Q_OS_MACX
   addSeparator();
#endif
   
   addAction(fileActions->getExitCaretAction());
}

/**
 * Destructor.
 */
GuiMainWindowFileMenu::~GuiMainWindowFileMenu()
{
}

/**
 * Create recent spec files menu
 *recent spec file menu about to show.
 */
void 
GuiMainWindowFileMenu::recentSpecFileMenuAboutToBeDisplayedSlot()
{
   recentSpecFileMenu->clear();
   
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   
   pf->getRecentSpecFiles(mainWindow->recentSpecFiles);
   
   for (int i = 0; i < static_cast<int>(mainWindow->recentSpecFiles.size()); i++) {
      QAction* action = recentSpecFileMenu->addAction(
                 FileUtilities::rearrangeFileName(mainWindow->recentSpecFiles[i]));
      action->setData(i);
   }   
}

/**
 * called for recent spec file menu selection.
 */
void 
GuiMainWindowFileMenu::recentSpecFileMenuSlot(QAction* action)
{
   const int item = action->data().toInt();
   mainWindow->recentSpecFileMenuSelection(item);
}
      


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

#include "GuiMainWindow.h"
#include "GuiMainWindowWindowActions.h"
#include "GuiMainWindowWindowMenu.h"

/**
 * Constructor.
 */
GuiMainWindowWindowMenu::GuiMainWindowWindowMenu(GuiMainWindow* parent) :
   QMenu("Window", parent)
{
   GuiMainWindowWindowActions* windowActions = parent->getWindowActions();
   
#ifdef Q_OS_MACX
   addAction(windowActions->getBringAllToFrontAction());
   //addAction(windowActions->getNextWindowAction());
#endif
   addAction(windowActions->getStackWindowsAction());
   addSeparator();
   
   //
   // NOTE: Actions for D/C, ID, Image windows are available for QT 4
   // 
   addAction(windowActions->getDisplayControlAction());
   addAction(windowActions->getIdWindowAction());
   addAction(windowActions->getTransformationMatrixEditorAction());

   addSeparator();
   addAction(windowActions->getDisplayImageEditorAction());
   addAction(windowActions->getScriptBuilderAction());   
   addAction(windowActions->getShellCommandWindowAction());
   addAction(windowActions->getTextFileEditorAction());
   addSeparator();

   addAction(windowActions->getWindow2Action());
   addAction(windowActions->getWindow3Action());
   addAction(windowActions->getWindow4Action());
   addAction(windowActions->getWindow5Action());   
   addAction(windowActions->getWindow6Action());
   addAction(windowActions->getWindow7Action());
   addAction(windowActions->getWindow8Action());
   addAction(windowActions->getWindow9Action());   
   addAction(windowActions->getWindow10Action()); 
      
   addSeparator();
   addAction(windowActions->getImageViewingWindowAction());
   
   QObject::connect(this, SIGNAL(aboutToShow()),
                    windowActions, SLOT(updateActions()));
}

/**
 * Destructor.
 */
GuiMainWindowWindowMenu::~GuiMainWindowWindowMenu()
{
}

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
/*
 *  XpmApplication.cpp
 *  appleevents
 *
 *  Created by Trenton Schulz on 10/31/04.
 *  Copyright 2004 Trolltech AS. All rights reserved.
 *
 */

#include <QGlobalStatic>

#ifdef Q_OS_MACX

#include <QEvent>
#include <QFileOpenEvent>
#include <QMessageBox>

#include "GuiMacOSXApplication.h"
#include "GuiMainWindow.h"

/**
 * Constructor
 */
GuiMacOSXApplication::GuiMacOSXApplication(int argc, char *argv[])
    : QApplication(argc, argv), caretMainWindow(0)
{
}

/**
 * Destructor
 */
GuiMacOSXApplication::~GuiMacOSXApplication()
{
}

/**
 * handle the custom event
 */
bool
GuiMacOSXApplication::event(QEvent* event)
{
   switch (event->type()) {
      case QEvent::FileOpen:
         {
            QFileOpenEvent* foe = static_cast<QFileOpenEvent*>(event);
            if (foe != NULL) {
               const QString fileName(foe->file());
               if (caretMainWindow != NULL) {
                  caretMainWindow->readSpecFile(fileName);
               }
            }
            return true;
         }
         break;
      default:
         return QApplication::event(event);
         break;
    }
}

/**
 * set the main window that is to get the open event
 */
void GuiMacOSXApplication::setGuiMainWindow(GuiMainWindow *mw)
{
    caretMainWindow = mw;
}

#endif // Q_OS_MACX


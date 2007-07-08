
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
 *  XpmApplication.h
 *  appleevents
 *
 *  Created by Trenton Schulz on 10/31/04.
 *  Copyright 2004 Trolltech AS. All rights reserved.
 *
 */

#ifndef __GUI_MAC_OSX_APPLICATION_H__
#define __GUI_MAC_OSX_APPLICATION_H__

#include <QGlobalStatic>

#ifdef Q_OS_MACX

#include <QApplication>

class GuiMainWindow;

/// This class allows the Mac OSX version of Caret to get the open file event
/// that occurs when a ".spec" file is double clicked.
class GuiMacOSXApplication : public QApplication {
   Q_OBJECT
   
   public:
      // constructor
      GuiMacOSXApplication(int argc, char *argv[]);
    
      // destructor
      ~GuiMacOSXApplication();
    
      // set the main window that is to get the open event
      void setGuiMainWindow(GuiMainWindow *mw);
    
   protected:
      // handle events
      bool event(QEvent *ev);
    
      // main window that is to get open event
      GuiMainWindow *caretMainWindow;
};

#endif // Q_OS_MACX

#endif // __GUI_MAC_OSX_APPLICATION_H__

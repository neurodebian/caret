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




#include <qapplication.h>
#include <qmessagebox.h>

#include "GuiMapFmriMainWindow.h"
#include "GuiMapFmriMainWindowHelpMenu.h"

/**
 * Constructor.
 */
GuiMapFmriMainWindowHelpMenu::GuiMapFmriMainWindowHelpMenu(GuiMapFmriMainWindow* parent) :
   QPopupMenu(parent)
{
   mainWindow = parent;
   
   insertItem("About Map fMRI to Surface...", this, SLOT(slotAbout()), CTRL+Key_Q);
}

/**
 * Destructor.
 */
GuiMapFmriMainWindowHelpMenu::~GuiMapFmriMainWindowHelpMenu()
{
}

/**
 * Call when about map fmri to surface selected.
 */
void
GuiMapFmriMainWindowHelpMenu::slotAbout()
{
   std::string msg;
   msg.append("Map fMRI to Surface Beta\n");
   msg.append("\n");
   msg.append("David Van Essen, John Harwell, Heather Drury, and Donna Hanlon\n");
   msg.append("Copyright 1995-2003 Washington University\n");
   msg.append("\n");
   msg.append("Washington University School of Medicine\n");
   msg.append("660 S. Euclid Ave\n");
   msg.append("St. Louis, MO 63110  USA\n");
   msg.append("\n");
   msg.append("http://brainmap.wustl.edu\n");
   msg.append("caret@v1.wustl.edu\n");
   msg.append("\n");
   msg.append("Map fMRI to Surface may also contain the following software:\n");
   msg.append("\n");
   msg.append("QT\n");
   msg.append("    Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.\n");
   msg.append("    http://www.trolltech.com\n");
   msg.append("\n");
   msg.append("VTK - Visualization Toolkit\n");
   msg.append("    Copyright (c) 1993-2001 Ken Martin, Will Schroeder, Bill Lorensen\n");
   msg.append("    http://www.visualizationtoolkit.org\n");
   msg.append("\n");
   msg.append("ZLIB\n");
   msg.append("    Copyright (c) 1995-2002 Jean-loup Gailly and Mark Adler\n");
   msg.append("    http://www.gzip.org/zlib/\n");
   msg.append("\n");

   QMessageBox::about(this, "Map fMRI to Surface", msg.c_str());
}


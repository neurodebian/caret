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
#include <qfiledialog.h>
#include <qstring.h>

#include "GuiMapFmriMainWindow.h"
#include "GuiMapFmriMainWindowAtlasMenu.h"

#include "map_fmri_global_variables.h"
#include "map_fmri_prototypes.h"

/**
 * Constructor.
 */
GuiMapFmriMainWindowAtlasMenu::GuiMapFmriMainWindowAtlasMenu(GuiMapFmriMainWindow* parent) :
   QPopupMenu(parent)
{
   mainWindow = parent;
   
   insertItem("Add New...", this, SLOT(slotAddAtlas()));
   
   insertItem("Delete...", this, SLOT(slotDeleteAtlas()));
}

/**
 * Destructor.
 */
GuiMapFmriMainWindowAtlasMenu::~GuiMapFmriMainWindowAtlasMenu()
{
}

/**
 * Called when add atlas menu item selected.
 */
void
GuiMapFmriMainWindowAtlasMenu::slotAddAtlas()
{
    QFileDialog fd(this, "atlas-file-dialog", true);
    fd.setCaption("Choose Atlas File");
    fd.setFilter("Atlas Directory Files (*.directory)");
    fd.setMode(QFileDialog::ExistingFile);
    if (fd.exec() == QDialog::Accepted) {
       const std::string fileName(fd.selectedFile().latin1());
       if (fileName.empty() == false) {
          std::string errorMessage;
          if (fmriMapper.addAtlasDirectory(fileName, true, errorMessage)) {
             displayErrorMessage(errorMessage);
             return;
          }
         
          fmriMapper.setSelectedAtlasDirectory(fmriMapper.getNumberOfAtlasDirectories() - 1);
          theMainWindow->showSelectedAtlas();
       }
    }
}

/**
 * Called when delete atlas menu item selected.
 */
void
GuiMapFmriMainWindowAtlasMenu::slotDeleteAtlas()
{
}


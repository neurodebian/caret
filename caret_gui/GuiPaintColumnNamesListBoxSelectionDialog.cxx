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


#include "GuiPaintColumnNamesListBoxSelectionDialog.h"

#include "BrainSet.h"
#include "GuiMainWindow.h"
#include "PaintFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiPaintColumnNamesListBoxSelectionDialog::GuiPaintColumnNamesListBoxSelectionDialog(QWidget* parent,
                                                         const int paintColumn)
   : QtListBoxSelectionDialog(parent, "Paint Name Selection")
{
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   pf->getPaintNamesForColumn(paintColumn, paintNameIndices);
   
   const int numNames = static_cast<int>(paintNameIndices.size());
   std::vector<QString> names;
   for (int i = 0; i < numNames; i++) {
      names.push_back(pf->getPaintNameFromIndex(paintNameIndices[i]));
   }
   
   setListBoxContents(names, 0);
}

/**
 * Destructor.
 */
GuiPaintColumnNamesListBoxSelectionDialog::~GuiPaintColumnNamesListBoxSelectionDialog()
{
}

/**
 * Get index of selected paint name
 */
int
GuiPaintColumnNamesListBoxSelectionDialog::getSelectedItemIndex() const
{
   const int item = QtListBoxSelectionDialog::getSelectedItemIndex();
   if ((item >= 0) && (item < static_cast<int>(paintNameIndices.size()))) {
      return paintNameIndices[item];
   }
   return -1;
}

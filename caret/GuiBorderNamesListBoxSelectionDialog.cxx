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


#include <algorithm>
#include <vector>

#include "BrainModelBorderSet.h"
#include "BrainSet.h"
#include "GuiBorderNamesListBoxSelectionDialog.h"
#include "GuiMainWindow.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiBorderNamesListBoxSelectionDialog::GuiBorderNamesListBoxSelectionDialog(QWidget* parent,
                                                            const QString& defaultBorderName)
   : QtListBoxSelectionDialog(parent, "Border Name Selection")
{
   std::vector<QString> names;
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->getAllBorderNames(names);
   QString nameToFind(defaultBorderName);
   std::vector<QString>::iterator iter = std::find(names.begin(), names.end(), 
                                                      defaultBorderName);
   int defaultIndex = 0;
   if (iter != names.end()) {
      defaultIndex = iter - names.begin();
   }
   setListBoxContents(names, defaultIndex);
}

/**
 * Destructor.
 */
GuiBorderNamesListBoxSelectionDialog::~GuiBorderNamesListBoxSelectionDialog()
{
}


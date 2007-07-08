
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

#include "GuiPreviousSpecFileComboBox.h"
#include "FileUtilities.h"

/**
 * Constructor.
 */
GuiPreviousSpecFileComboBox::GuiPreviousSpecFileComboBox(QWidget*parent, 
                                             const std::vector<QString>& specFilesIn)
   : QComboBox(parent)
{
   initializeComboBox(specFilesIn);
}
/**
 * Constructor.
 */
GuiPreviousSpecFileComboBox::GuiPreviousSpecFileComboBox(QWidget*parent, 
                                                         const PreferencesFile* pf)
   : QComboBox(parent)
{
   std::vector<QString> specFileNames;
   if (pf != NULL) {
      pf->getRecentSpecFiles(specFileNames);
   }
   initializeComboBox(specFileNames);
}

/**
 * Destructor.
 */
GuiPreviousSpecFileComboBox::~GuiPreviousSpecFileComboBox()
{
}

/**
 * initialize combo box.
 */
void 
GuiPreviousSpecFileComboBox::initializeComboBox(const std::vector<QString>& specFileNames)
{
   previousSpecFiles = specFileNames;
   
   QObject::connect(this, SIGNAL(activated(int)),
                    this, SLOT(slotItemSelected(int)));
   
   const int numSpecFiles = static_cast<int>(previousSpecFiles.size());

   //
   // Find longest file name
   //
   int longestNameLength = -1;
   for (int i = 0; i < numSpecFiles; i++) {
      longestNameLength = std::max(longestNameLength, 
                    static_cast<int>(FileUtilities::basename(previousSpecFiles[i]).length()));
   }
   
   //
   // Load the combo box
   //
   addItem("Choose Recent Spec File");
   for (int i = 0; i < numSpecFiles; i++) {
      //
      // Maximum path length various due to differences in file names
      //
      const int maxPathLength = longestNameLength
                              - FileUtilities::basename(previousSpecFiles[i]).length()
                              + 25;
      addItem(FileUtilities::rearrangeFileName(previousSpecFiles[i], maxPathLength));
   }
}

/**
 * called when a combo box item is selected.
 */
void 
GuiPreviousSpecFileComboBox::slotItemSelected(int item)
{
   if (item > 0) {
      QString name = previousSpecFiles[item - 1];
      emit specFileSelected(name);
   }
}

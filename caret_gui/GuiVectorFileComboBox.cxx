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

#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiMainWindow.h"
#include "GuiVectorFileComboBox.h"
#include "VectorFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiVectorFileComboBox::GuiVectorFileComboBox(QWidget* parent,
                                                 const char* /*name*/)
   : QComboBox(parent)
{
   updateComboBox();
}
                     
/**
 * Destructor.
 */
GuiVectorFileComboBox::~GuiVectorFileComboBox()
{
}

/**
 * set the selected vector file.
 */
void 
GuiVectorFileComboBox::setSelectedVectorFile(const VectorFile* tf)
{
   if (tf != NULL) {
      for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVectorFiles(); i++) {
         if (tf == theMainWindow->getBrainSet()->getVectorFile(i)) {
            setCurrentIndex(i);
            break;
         }
      }
   }
}      

/**
 * get the selected vector file.
 */
VectorFile*
GuiVectorFileComboBox::getSelectedVectorFile() const
{
   const int index = getSelectedVectorFileIndex();
   if (index >= 0) {
      return vectorFilePointers[index];
   }
   return NULL;
}

/**
 * get the selected vector file index.
 */
int 
GuiVectorFileComboBox::getSelectedVectorFileIndex() const
{
   const int index = currentIndex();
   if ((index >= 0) && (index < count())) {
      return index;
   }
   return -1;
}

/**
 * update the items in the combo box.
 */
void 
GuiVectorFileComboBox::updateComboBox()
{
   //
   // Find currently selected vector file
   //
   VectorFile* defaultVectorFile = NULL;
   const int index = currentIndex();
   if ((index >= 0) && (index < count())) {
      defaultVectorFile = vectorFilePointers[index];
   }
   
   //
   // Clear out the vector selections
   //
   clear();
   vectorFilePointers.clear();
   
   //
   // Load up the vector files
   //
   int defaultIndex = 0;
   const int num = theMainWindow->getBrainSet()->getNumberOfVectorFiles();
   for (int i = 0; i < num; i++) {
      VectorFile* vf = theMainWindow->getBrainSet()->getVectorFile(i);
      vectorFilePointers.push_back(vf);
      addItem(FileUtilities::basename(vf->getFileName()));
      if (vf == defaultVectorFile) {
         defaultIndex = i;
      }
   }
   
   if (count() > 0) {
      setCurrentIndex(defaultIndex);
   }
}


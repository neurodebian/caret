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
#include "GuiMainWindow.h"
#include "GuiTopologyFileComboBox.h"
#include "TopologyFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiTopologyFileComboBox::GuiTopologyFileComboBox(QWidget* parent,
                                                 const char* /*name*/)
   : QComboBox(parent)
{
   updateComboBox();
}
                     
/**
 * Destructor.
 */
GuiTopologyFileComboBox::~GuiTopologyFileComboBox()
{
}

/**
 * set the selected topology file.
 */
void 
GuiTopologyFileComboBox::setSelectedTopologyFile(const TopologyFile* tf)
{
   if (tf != NULL) {
      for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfTopologyFiles(); i++) {
         if (tf == theMainWindow->getBrainSet()->getTopologyFile(i)) {
            setCurrentIndex(i);
            break;
         }
      }
   }
}      

/**
 * get the selected topology file.
 */
TopologyFile* 
GuiTopologyFileComboBox::getSelectedTopologyFile() const
{
   const int index = getSelectedTopologyFileIndex();
   if (index >= 0) {
      return topologyFilePointers[index];
   }
   return NULL;
}

/**
 * get the selected topology file index.
 */
int 
GuiTopologyFileComboBox::getSelectedTopologyFileIndex() const
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
GuiTopologyFileComboBox::updateComboBox()
{
   //
   // Find currently selected topology file
   //
   TopologyFile* defaultTopologyFile = NULL;
   const int index = currentIndex();
   if ((index >= 0) && (index < count())) {
      defaultTopologyFile = topologyFilePointers[index];
   }
   
   //
   // Clear out the topology selections
   //
   clear();
   topologyFilePointers.clear();
   
   //
   // Load up the topology files
   //
   int defaultIndex = 0;
   const int num = theMainWindow->getBrainSet()->getNumberOfTopologyFiles();
   for (int i = 0; i < num; i++) {
      TopologyFile* tf = theMainWindow->getBrainSet()->getTopologyFile(i);
      topologyFilePointers.push_back(tf);
      addItem(tf->getDescriptiveName());
      if (tf == defaultTopologyFile) {
         defaultIndex = i;
      }
   }
   
   if (count() > 0) {
      setCurrentIndex(defaultIndex);
   }
}


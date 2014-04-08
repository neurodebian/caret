
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
#include "GuiModelSelectionControl.h"
#include "VtkModelFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiModelSelectionControl::GuiModelSelectionControl(QWidget* parent)
   : QComboBox(parent)
{
   setObjectName("GuiModelSelectionControl");
   
   updateControl();
}
                         
/**
 * destructor.
 */
GuiModelSelectionControl::~GuiModelSelectionControl()
{
}

/**
 * get selected model.
 */
VtkModelFile* 
GuiModelSelectionControl::getSelectedModel()
{
   const int indx = getSelectedModelIndex();
   if (indx >= 0) {
      return models[indx];
   }
   return NULL;
}

/**
 * get selected model.
 */
const VtkModelFile* 
GuiModelSelectionControl::getSelectedModel() const
{
   const int indx = getSelectedModelIndex();
   if (indx >= 0) {
      return models[indx];
   }
   return NULL;
}

/**
 * get selected model.
 */
int 
GuiModelSelectionControl::getSelectedModelIndex() const
{
   const int indx = currentIndex();
   if ((indx >= 0) && (indx < static_cast<int>(models.size()))) {
      return indx;
   }
   return -1;
}

/**
 * set the selected model.
 */
void 
GuiModelSelectionControl::setSelectedModel(const VtkModelFile* vmf)
{
   for (int i = 0; i < static_cast<int>(models.size()); i++) {
      if (models[i] == vmf) {
         setCurrentIndex(i);
         break;
      }
   }
}

/**
 * set the selected model index.
 */
void 
GuiModelSelectionControl::setSelectedModelIndex(const int indx)
{
   if ((indx >= 0) && (indx < static_cast<int>(models.size()))) {
      setCurrentIndex(indx);
   }
}

/**
 * update the control.
 */
void 
GuiModelSelectionControl::updateControl()
{
   VtkModelFile* selectedModel = NULL;
   int selectedIndex = getSelectedModelIndex();
   if ((selectedIndex >= 0) && (selectedIndex < static_cast<int>(models.size()))) {
      selectedModel = models[selectedIndex];
   }
   
   this->clear();
   selectedIndex = -1;
   models.clear();
   
   const int num = theMainWindow->getBrainSet()->getNumberOfVtkModelFiles();
   for (int i = 0; i < num; i++) {
      VtkModelFile* vmf = theMainWindow->getBrainSet()->getVtkModelFile(i);
      models.push_back(vmf);
      addItem(FileUtilities::basename(vmf->getFileName()));
      
      if (selectedModel == vmf) {
         selectedIndex = i;
      }
   }
   
   if (selectedIndex >= 0) {
      setSelectedModelIndex(selectedIndex);
   }
}      

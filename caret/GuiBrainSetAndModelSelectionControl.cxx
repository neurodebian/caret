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

#include "BrainModel.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiBrainSetAndModelSelectionControl.h"
#include "GuiMainWindow.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiBrainSetAndModelSelectionControl::GuiBrainSetAndModelSelectionControl(QWidget* parent)
   : QComboBox(parent)
{
   firstFiducialBrainSet = NULL;
   firstFiducialModel = NULL;
   allFiducialIndex = -1;
   //updateControl();
}

/**
 * destructor.
 */
GuiBrainSetAndModelSelectionControl::~GuiBrainSetAndModelSelectionControl()
{
}

/**
 * get the selected brain set.
 */
BrainSet* 
GuiBrainSetAndModelSelectionControl::getSelectedBrainSet()
{
   const int selItem = currentIndex();
   if ((selItem >= 0) && (selItem < static_cast<int>(theBrainSets.size()))) {
      return theBrainSets[selItem];
   }
   return NULL;
}

/**
 * get the selected model.
 */
BrainModel* 
GuiBrainSetAndModelSelectionControl::getSelectedBrainModel()
{
   const int selItem = currentIndex();
   if ((selItem >= 0) && (selItem < static_cast<int>(theBrainModels.size()))) {
      return theBrainModels[selItem];
   }
   return NULL;
}

/**
 * update the control.
 */
void 
GuiBrainSetAndModelSelectionControl::updateControl(BrainSet* selectedBrain,
                                                   BrainModel* selectedModel)
{
   //
   // Save the current selection
   //
   BrainSet* currentBrainSet = getSelectedBrainSet();
   BrainModel* currentBrainModel = getSelectedBrainModel();
   if ((selectedBrain != NULL) && (selectedModel != NULL)) {
      currentBrainSet = selectedBrain;
      currentBrainModel = selectedModel;
   }
   
   //
   // clear out the control
   //
   bool allFiducialWasSelected = false;
   if (currentIndex() >= 0) {
      allFiducialWasSelected = (currentIndex() == allFiducialIndex);
   }
   clear();
   theBrainSets.clear();
   theBrainModels.clear();
   firstFiducialBrainSet = NULL;
   firstFiducialModel = NULL;
   int fiducialCount = 0;
   allFiducialIndex = -1;
   
   //
   // When GUI is being constructed, "theMainWindow" will be NULL
   //
   if (theMainWindow == NULL) {
      return;
   }
   
   //
   // Get all of the loaded brain sets
   //
   std::vector<BrainSet*> allBrains;
   theMainWindow->getAllBrainSets(allBrains);
   
   //
   // Load up the control
   //
   const bool multipleBrainSetsFlag = (allBrains.size() > 1);
   int defaultIndex = 0;
   for (unsigned int i = 0; i < allBrains.size(); i++) {
      BrainSet* bs = allBrains[i];
      const QString structAbbrevName = bs->getStructure().getTypeAsAbbreviatedString();
      
      for (int j = 0; j < bs->getNumberOfBrainModels(); j++) {
         BrainModel* bm = bs->getBrainModel(j);
         
         theBrainSets.push_back(bs);
         theBrainModels.push_back(bm);      
         
         QString name;
         if (multipleBrainSetsFlag) {
            name.append(structAbbrevName);
            name.append("-");
         }
         name.append(bm->getDescriptiveName());
         addItem(name);
         
         if ((bs == currentBrainSet) &&
             (bm == currentBrainModel)) {
            defaultIndex = theBrainSets.size() - 1;
         }
      }
      
      //
      // Look for fiducial surfaces
      //
      if (bs->getActiveFiducialSurface() != NULL) {
         if (firstFiducialModel == NULL) {
            firstFiducialModel = bs->getActiveFiducialSurface();
            firstFiducialBrainSet = bs;
         }
         fiducialCount++;
      }
   }
   
   if (fiducialCount > 1) {
      theBrainSets.push_back(firstFiducialBrainSet);
      theBrainModels.push_back(firstFiducialModel);
      addItem("ALL FIDUCIAL SURFACES");
      allFiducialIndex = count() - 1;
      if (allFiducialWasSelected) {
         defaultIndex = allFiducialIndex;
      }
   }
   
   if (defaultIndex < count()) {
      setCurrentIndex(defaultIndex);
   }
}      

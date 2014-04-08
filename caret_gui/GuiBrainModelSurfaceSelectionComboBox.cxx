/*
 *  GuiBrainModelSurfaceSelectionComboBox.cxx
 *  caret
 *
 *  Created by John Harwell on 8/28/08.
 *  Copyright 2008 Washington University School of Medicine. All rights reserved.
 *
 */

#include <algorithm>

#include "BrainSet.h"
#include "GuiBrainModelSurfaceSelectionComboBox.h"
#include "GuiMainWindow.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiBrainModelSurfaceSelectionComboBox::GuiBrainModelSurfaceSelectionComboBox(
   const BrainModelSurface::SURFACE_TYPES surfaceTypeIn)
{
   allowedSurfaceTypes.push_back(surfaceTypeIn);
   
   updateComboBox();
}
   
/**
 * constructor.
 */
GuiBrainModelSurfaceSelectionComboBox::GuiBrainModelSurfaceSelectionComboBox(
       const std::vector<BrainModelSurface::SURFACE_TYPES> surfaceTypesIn)
{
   allowedSurfaceTypes = surfaceTypesIn;
   
   updateComboBox();
}
         
/**
 * destructor.
 */
GuiBrainModelSurfaceSelectionComboBox::~GuiBrainModelSurfaceSelectionComboBox()
{
}

/**
 * get the selected surface.
 */
BrainModelSurface* 
GuiBrainModelSurfaceSelectionComboBox::getSelectedBrainModelSurface()
{
   const int indx = currentIndex();
   if ((indx >= 0) &&
       (indx < static_cast<int>(brainModelSurfaces.size()))) {
      return brainModelSurfaces[indx];
   }
   
   return NULL;
}

/**
 * set the selected surface.
 */
void 
GuiBrainModelSurfaceSelectionComboBox::setSelectedBrainModelSurface(const BrainModelSurface* bms)
{
   for (int i = 0; i < static_cast<int>(brainModelSurfaces.size()); i++) {
      if (brainModelSurfaces[i] == bms) {
         setCurrentIndex(i);
         break;
      }
   }
}

/**
 * update the combo box.
 */
void 
GuiBrainModelSurfaceSelectionComboBox::updateComboBox()
{
   blockSignals(true);
   
   BrainModelSurface* currentBMS = getSelectedBrainModelSurface();
   
   brainModelSurfaces.clear();
   clear();
   
   int defaultIndex = 0;
   BrainSet* bs = theMainWindow->getBrainSet();
   const int numBrainModels = bs->getNumberOfBrainModels();
   for (int i = 0; i < numBrainModels; i++) {
      BrainModelSurface* bms = bs->getBrainModelSurface(i);
      if (bms != NULL) {
         const BrainModelSurface::SURFACE_TYPES surfaceType =
            bms->getSurfaceType();
         if (std::find(allowedSurfaceTypes.begin(),
                       allowedSurfaceTypes.end(),
                       surfaceType) != allowedSurfaceTypes.end()) {
            if (bms == currentBMS) {
               defaultIndex = count();
            }

            addItem(bms->getDescriptiveName());
            brainModelSurfaces.push_back(bms);            
         }
      }
   }
   
   if (defaultIndex < count()) {
      setCurrentIndex(defaultIndex);
   }
   
   blockSignals(false);
}

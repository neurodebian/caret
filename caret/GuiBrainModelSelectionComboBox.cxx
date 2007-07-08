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

#include "BrainModelContours.h"
#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiMainWindow.h"
#include "global_variables.h"

/**
 * Constructor.
 * If "addNewName" has 1 or more characters, it is added at the end of the combo box.
 */
GuiBrainModelSelectionComboBox::GuiBrainModelSelectionComboBox(const bool showContoursIn, 
                                                               const bool showSurfacesIn,
                                                               const bool showVolumesIn,
                                                               const QString& addNewNameIn,
                                                               QWidget* parent,
                                                               const char* name,
                                                               const bool flatSurfacesOnlyIn,
                                                               const bool fiducialSurfacesOnlyIn,
                                                               const bool hullSurfacesOnlyIn)
   : QComboBox(parent)
{
   setObjectName(name);
   showContours = showContoursIn;
   showSurfaces = showSurfacesIn;
   showVolumes  = showVolumesIn;
   addNewName = addNewNameIn;
   addNewIndex  = -1;
   flatSurfacesOnly = flatSurfacesOnlyIn;
   fiducialSurfacesOnly = fiducialSurfacesOnlyIn;
   hullSurfacesOnly = hullSurfacesOnlyIn;
   limitToStructure = Structure::STRUCTURE_TYPE_INVALID;
   
   updateComboBox();
}

/**
 * Destructor
 */
GuiBrainModelSelectionComboBox::~GuiBrainModelSelectionComboBox()
{
}

/**
 * limit surface to this structure.
 */
void 
GuiBrainModelSelectionComboBox::setSurfaceStructureRequirement(const Structure::STRUCTURE_TYPE limitToStructureIn)
{
   limitToStructure = limitToStructureIn;
}
      
/**
 * Update the items in the combo box.
 */
void
GuiBrainModelSelectionComboBox::updateComboBox()
{   
   //
   // Find out what is currently selected.
   //
   BrainModel* currentBrainModel = NULL;
   int defaultItemIndex = -1;
   if (getAddNewSelected()) {
      defaultItemIndex = ADD_NEW_INDEX;
   }
   else {
      if (count() > 0) {
         currentBrainModel = brainModelPointers[currentIndex()];
      }
   }
   
   //
   // clear the combo box and indices/pointers
   //
   clear();
   brainModelIndices.clear();
   brainModelPointers.clear();
   
   BrainSet* brainSet = theMainWindow->getBrainSet();
   if (brainSet == NULL) {
      return;
   }

   //
   // load models into the combo box
   //
   const int numModels = brainSet->getNumberOfBrainModels();
   for (int i = 0; i < numModels; i++) {
      BrainModel* bm = brainSet->getBrainModel(i);
      
      bool useIt = false;
      switch(bm->getModelType()) {
         case BrainModel::BRAIN_MODEL_CONTOURS:
            useIt = showContours;
            break;
         case BrainModel::BRAIN_MODEL_SURFACE:
            useIt = showSurfaces;
            if (useIt) {
               const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
               if (flatSurfacesOnly) {
                  if ((bms->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FLAT) &&
                      (bms->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
                     useIt = false;
                  }
               }
               if (fiducialSurfacesOnly) {
                  if (bms->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
                     useIt = false;
                  }
               }
               if (hullSurfacesOnly) {
                  if (bms->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_HULL) {
                     useIt = false;
                  }
               }
               
               if (useIt) {
                  switch (limitToStructure) {
                     case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                        if (bms->getStructure().getType() != Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
                           useIt = false;
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                        if (bms->getStructure().getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
                           useIt = false;
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM:
                        break;
                     case Structure::STRUCTURE_TYPE_INVALID:
                        break;
                  }
               }
            }
            break;
         case BrainModel::BRAIN_MODEL_VOLUME:
            useIt = showVolumes;
            break;
         case BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME:
            break;
      }
      
      if (useIt) {
         if (bm == currentBrainModel) {
            defaultItemIndex = i;
         }
         addItem(bm->getDescriptiveName());
         brainModelIndices.push_back(i);
         brainModelPointers.push_back(bm);
      }
   }
   
   //
   // Add the "Add New" selection if the add new name is not empty
   //
   addNewIndex = -1;
   if (addNewName.isEmpty() == false) {
      addNewIndex = count();
      addItem(addNewName);
      brainModelIndices.push_back(ADD_NEW_INDEX);
      brainModelPointers.push_back(NULL);
   }
   
   //
   // Set to default item
   //
   setSelectedBrainModelIndex(defaultItemIndex);
}

/**
 * get the selected brain model index.
 * Returns -1 if there is no selection.
 * Returns ADD_NEW_INDEX if the addNewName was set in the constructor.
 */
int
GuiBrainModelSelectionComboBox::getSelectedBrainModelIndex() const
{
   if (count() > 0) {
      if (addNewIndex >= 0) {
         if (currentIndex() == addNewIndex) {
            return ADD_NEW_INDEX;
         }
      }
      return brainModelIndices[currentIndex()];
   }
   return -1;
}

/**
 * See if "add new" is the selected item
 */
bool
GuiBrainModelSelectionComboBox::getAddNewSelected() const
{
   if (count() > 0) {
      if (addNewIndex >= 0) {
         if (currentIndex() == addNewIndex) {
            return true;
         }
      }
   }
   return false;
}
 
/**
 * get the selected brain model.
 */
BrainModel*
GuiBrainModelSelectionComboBox::getSelectedBrainModel() const
{
   const int index = getSelectedBrainModelIndex();
   if (index == ADD_NEW_INDEX) {
      return NULL;
   }
   if (index >= 0) {
      return theMainWindow->getBrainSet()->getBrainModel(index);
   }
   return NULL;
}

/**
 * get the selected brain model contours
 */
BrainModelContours* 
GuiBrainModelSelectionComboBox::getSelectedBrainModelContours() const
{
   BrainModel* bm = getSelectedBrainModel();
   if (bm != NULL) {
      return dynamic_cast<BrainModelContours*>(bm);
   }
   return NULL;
}
      

/**
 * get the selected brain model surface
 */
BrainModelSurface* 
GuiBrainModelSelectionComboBox::getSelectedBrainModelSurface() const
{
   BrainModel* bm = getSelectedBrainModel();
   if (bm != NULL) {
      return dynamic_cast<BrainModelSurface*>(bm);
   }
   return NULL;
}
      
/**
 * get the selected brain model volume
 */
BrainModelVolume* 
GuiBrainModelSelectionComboBox::getSelectedBrainModelVolume() const
{
   BrainModel* bm = getSelectedBrainModel();
   if (bm != NULL) {
      return dynamic_cast<BrainModelVolume*>(bm);
   }
   return NULL;
}

/**
 * set the combo box to show the model with the specified index.
 * Pass ADD_NEW_INDEX to set the add new label.
 */
void
GuiBrainModelSelectionComboBox::setSelectedBrainModelIndex(const int bmi)
{
   if (bmi == ADD_NEW_INDEX) {
      setCurrentIndex(addNewIndex);
   }
   else {
      for (int i = 0; i < static_cast<int>(brainModelIndices.size()); i++) {
         if (bmi == brainModelIndices[i]) {
            setCurrentIndex(i);
            break;
         }
      }
   }
}

/**
 * Set the selected brain model.
 */
void
GuiBrainModelSelectionComboBox::setSelectedBrainModel(const BrainModel* bm)
{
   for (int i = 0; i < static_cast<int>(brainModelPointers.size()); i++) {
      if (brainModelPointers[i] == bm) {
         setCurrentIndex(i);
      }
   }
}

/**
 * set the selected brain model to the first surface of the specified type.
 */
void 
GuiBrainModelSelectionComboBox::setSelectedBrainModelToSurfaceOfType(
                                        const BrainModelSurface::SURFACE_TYPES st)
{
   if (st == BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
      setSelectedBrainModel(theMainWindow->getBrainSet()->getActiveFiducialSurface());
   }
   else {
      for (int i = (static_cast<int>(brainModelIndices.size()) - 1); i >= 0; i--) {
         const int modelNumber = brainModelIndices[i];
         BrainModel* bm = brainModelPointers[i];
         if (bm->getModelType() == BrainModel::BRAIN_MODEL_SURFACE) {
            BrainModelSurface* bms = dynamic_cast<BrainModelSurface*>(bm);
            if (bms->getSurfaceType() == st) {
               setSelectedBrainModelIndex(modelNumber);
               break;
            }
         }
      }
   }
}


      

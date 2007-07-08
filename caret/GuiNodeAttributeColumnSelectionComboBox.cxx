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

#include "ArealEstimationFile.h"
#include "BrainSet.h"
#include "DeformationFieldFile.h"
#include "DeformationMapFile.h"
#include "GeodesicDistanceFile.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GiftiNodeDataFile.h"
#include "GuiMainWindow.h"
#include "LatLonFile.h"
#include "NodeAttributeFile.h"
#include "PaintFile.h"
#include "RgbPaintFile.h"
#include "SectionFile.h"
#include "SurfaceShapeFile.h"
#include "SurfaceVectorFile.h"
#include "ProbabilisticAtlasFile.h"
#include "TopographyFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiNodeAttributeColumnSelectionComboBox::GuiNodeAttributeColumnSelectionComboBox(
                                        const GUI_NODE_FILE_TYPE nodeFileTypeIn,
                                        const bool addNewSelection,
                                        const bool addNoneSelection,
                                        const bool addAllSelection,
                                        QWidget* parent)
   : QComboBox(parent)
{
   initializeDialog(nodeFileTypeIn,
              NULL,
              NULL,
              addNewSelection,
              addNoneSelection,
              addAllSelection);
}

/** 
 * Constructor. 
 *  USE ONLY WITH FILES THAT ARE NOT IN THE BRAIN SET
 */
GuiNodeAttributeColumnSelectionComboBox::GuiNodeAttributeColumnSelectionComboBox(
                                              NodeAttributeFile* nonBrainSetNodeAttributeFileIn,
                                              const bool addNewSelection,
                                              const bool addNoneSelection,
                                              const bool addAllSelection,
                                              QWidget* parent)
   : QComboBox(parent)
{
   initializeDialog(GUI_NODE_FILE_TYPE_NONE,
              nonBrainSetNodeAttributeFileIn,
              NULL,
              addNewSelection,
              addNoneSelection,
              addAllSelection);
}

/**
 * Constructor 
 *  USE ONLY WITH FILES THAT ARE NOT IN THE BRAIN SET
 */
GuiNodeAttributeColumnSelectionComboBox::GuiNodeAttributeColumnSelectionComboBox(
                                        GiftiNodeDataFile* nonBrainSetGiftiNodeDataFileIn,
                                        const bool addNewSelection,
                                        const bool addNoneSelection,
                                        const bool addAllSelection,
                                        QWidget* parent)
   : QComboBox(parent)
{
   initializeDialog(GUI_NODE_FILE_TYPE_NONE,
              NULL,
              nonBrainSetGiftiNodeDataFileIn,
              addNewSelection,
              addNoneSelection,
              addAllSelection);
}

/**
 * initialize the dialog.
 */
void 
GuiNodeAttributeColumnSelectionComboBox::initializeDialog(
                                     const GUI_NODE_FILE_TYPE nodeFileTypeIn,
                                     NodeAttributeFile* nonBrainSetNodeAttributeFileIn,
                                     GiftiNodeDataFile* nonBrainSetGiftiNodeDataFileIn,
                                     const bool addNewSelection,
                                     const bool addNoneSelection,
                                     const bool addAllSelection)
{
   nodeFileType = nodeFileTypeIn;
   nonBrainSetNodeAttributeFile = nonBrainSetNodeAttributeFileIn;
   nonBrainSetGiftiNodeDataFile = nonBrainSetGiftiNodeDataFileIn;

   QObject::connect(this, SIGNAL(activated(int)),
                    this, SLOT(slotActivated()));
   allowNewColumnSelection = addNewSelection;
   allowNoneColumnSelection = addNoneSelection;
   allowAllColumnSelection = addAllSelection;
   noneSelectionLabel = "None";
   newSelectionLabel = "Create New Column";
   allSelectionLabel = "All Columns";
   updateComboBox();
}

/**
 * Destructor.
 */
GuiNodeAttributeColumnSelectionComboBox::~GuiNodeAttributeColumnSelectionComboBox()
{
}

/**
 * set the none selection entry string.
 */
void 
GuiNodeAttributeColumnSelectionComboBox::setNoneSelectionLabel(const QString& s)
{
   noneSelectionLabel = s;
   updateComboBox();
}      

/**
 * called by QComboBox when an item is selected
 */
void
GuiNodeAttributeColumnSelectionComboBox::slotActivated()
{
   const int item = currentIndex();

   emit itemSelected(item);

   QString name;
   if (item == CURRENT_ITEM_NEW) {
      name = newSelectionLabel;
   }   
   else if (item == CURRENT_ITEM_NONE) {
      name = noneSelectionLabel;
   }
   else if (item == CURRENT_ITEM_ALL) {
      name = allSelectionLabel;
   }
   else if (item >= 0) {
      name = currentText();
   }
   emit itemNameSelected(name);
}

/**
 * Get the current selection
 * Returns  CURRENT_ITEM_NEW if a new column is selected.
 *          CURRENT_ITEM_NONE if none is selected
 *          CURRENT_ITEM_ALL if all columns is selected
 */
int
GuiNodeAttributeColumnSelectionComboBox::currentIndex() const
{
   const int item = QComboBox::currentIndex();
   if ((item < 0) || (item >= static_cast<int>(itemValues.size()))) {
      return CURRENT_ITEM_INVALID;
   }
   
   return itemValues[item];
}

/**
 * Get the current label
 */
QString 
GuiNodeAttributeColumnSelectionComboBox::getCurrentLabel() const
{
   return currentText();
}

/**
 * Set the current selection
 * Pass CURRENT_ITEM_NEW for new column.
 *      CURRENT_ITEM_NONE for none.
 */
void
GuiNodeAttributeColumnSelectionComboBox::setCurrentIndex(int itemIn)
{
   int selectedItem = CURRENT_ITEM_INVALID;
   for (int i = 0; i < static_cast<int>(itemValues.size()); i++) {
      if (itemValues[i] == itemIn) {
         selectedItem = i;
         break;
      }
   }
   
   if (selectedItem != CURRENT_ITEM_INVALID) {
      QComboBox::setCurrentIndex(selectedItem);
   }
}

/**
 * Enable/Disable the ALL selection.
 */
void
GuiNodeAttributeColumnSelectionComboBox::enableDisableAllSelection(const bool enableIt)
{
   allowAllColumnSelection = enableIt;
   updateComboBox();
}

/**
 * Update the combo box
 */ 
void
GuiNodeAttributeColumnSelectionComboBox::updateComboBox()
{
   NodeAttributeFile* nodeAttFile = NULL;
   GiftiNodeDataFile* giftiNodeDataFile = NULL;
   
   BrainSet* bs = theMainWindow->getBrainSet();
   
   switch (nodeFileType) {
      case GUI_NODE_FILE_TYPE_NONE:
         if (nonBrainSetNodeAttributeFile) {
            nodeAttFile = nonBrainSetNodeAttributeFile;
         }
         else if (nonBrainSetGiftiNodeDataFile != NULL) {
            giftiNodeDataFile = nonBrainSetGiftiNodeDataFile;
         }
         break;
      case GUI_NODE_FILE_TYPE_AREAL_ESTIMATION:
         nodeAttFile = bs->getArealEstimationFile();
         break;
      case GUI_NODE_FILE_TYPE_DEFORMATION_FIELD:
         nodeAttFile = bs->getDeformationFieldFile();
         break;
      case GUI_NODE_FILE_TYPE_GEODESIC_DISTANCE:
         nodeAttFile = bs->getGeodesicDistanceFile();
         break;
      case GUI_NODE_FILE_TYPE_LAT_LON:
         nodeAttFile = bs->getLatLonFile();
         break;
      case GUI_NODE_FILE_TYPE_METRIC:   // gifti
         giftiNodeDataFile = bs->getMetricFile();
         break;
      case GUI_NODE_FILE_TYPE_PAINT:    // gifti
         giftiNodeDataFile = bs->getPaintFile();
         break;
      case GUI_NODE_PROBABILISTIC_ATLAS:
         giftiNodeDataFile = bs->getProbabilisticAtlasSurfaceFile();
         break;
      case GUI_NODE_FILE_TYPE_RGB_PAINT:
         nodeAttFile = bs->getRgbPaintFile();
         break;
      case GUI_NODE_FILE_TYPE_SECTION:
         nodeAttFile = bs->getSectionFile();
         break;
      case GUI_NODE_FILE_TYPE_SURFACE_SHAPE:
         giftiNodeDataFile = bs->getSurfaceShapeFile();
         break;
      case GUI_NODE_FILE_TYPE_SURFACE_VECTOR:
         nodeAttFile = bs->getSurfaceVectorFile();
         break;
      case GUI_NODE_FILE_TYPE_TOPOGRAPHY:
         nodeAttFile = bs->getTopographyFile();
         break;
   }
   
   if (nodeAttFile != NULL) {
      updateComboBox(nodeAttFile);
   }
   else if (giftiNodeDataFile != NULL) {
      updateComboBox(giftiNodeDataFile);
   }
   else {
      std::cout << "PROGRAM ERROR in GuiNodeAttributeColumnSelectionComboBox::updateComboBox()\n"
                << "   invalid file type." << std::endl;
   }
   
   update();
}

/**
 * Update the combo box
 */
void
GuiNodeAttributeColumnSelectionComboBox::updateComboBox(NodeAttributeFile* nodeAttFile)
{
   //
   // get the currently selected item
   //
   const int selectedItem = currentIndex();
   
   //
   // Save text of current selection
   //
   QString selectedItemText;
   if (selectedItem != CURRENT_ITEM_INVALID) {
      selectedItemText = currentText();
   }
   
   //
   // clear the combo box and the item values storage
   //
   clear();
   itemValues.clear();
      
   const int num = nodeAttFile->getNumberOfColumns();
   for (int i = 0; i < num; i++) {
      addItem(nodeAttFile->getColumnName(i));
      itemValues.push_back(i);
   }
   if (allowNewColumnSelection) {
      addItem(newSelectionLabel);
      itemValues.push_back(CURRENT_ITEM_NEW);
   }
   if (allowNoneColumnSelection) {
      addItem(noneSelectionLabel);
      itemValues.push_back(CURRENT_ITEM_NONE);
   }
   if (allowAllColumnSelection) {
      addItem(allSelectionLabel);
      itemValues.push_back(CURRENT_ITEM_ALL);
   }
   
   //
   // try to find previously selected item
   //
   int item = 0;
   if (selectedItemText.isEmpty() == false) {
      for (int i = 0; i < count(); i++) {
         if (itemText(i) == selectedItemText) {
            item = itemValues[i];
            break;
         }
      }
   }
   
   //
   // Set the current item
   //
   //if ((item >= 0) && (item < count())) {
      setCurrentIndex(item);
   //}
}

/**
 * Update the combo box
 */
void
GuiNodeAttributeColumnSelectionComboBox::updateComboBox(GiftiNodeDataFile* giftiNodeDataFile)
{
   //
   // get the currently selected item
   //
   const int selectedItem = currentIndex();
   
   //
   // Save text of current selection
   //
   QString selectedItemText;
   if (selectedItem != CURRENT_ITEM_INVALID) {
      selectedItemText = currentText();
   }
   
   //
   // clear the combo box and the item values storage
   //
   clear();
   itemValues.clear();
      
   const int num = giftiNodeDataFile->getNumberOfColumns();
   for (int i = 0; i < num; i++) {
      addItem(giftiNodeDataFile->getColumnName(i));
      itemValues.push_back(i);
   }
   if (allowNewColumnSelection) {
      addItem(newSelectionLabel);
      itemValues.push_back(CURRENT_ITEM_NEW);
   }
   if (allowNoneColumnSelection) {
      addItem(noneSelectionLabel);
      itemValues.push_back(CURRENT_ITEM_NONE);
   }
   if (allowAllColumnSelection) {
      addItem(allSelectionLabel);
      itemValues.push_back(CURRENT_ITEM_ALL);
   }
   
   //
   // try to find previously selected item
   //
   int item = 0;
   if (selectedItemText.isEmpty() == false) {
      for (int i = 0; i < count(); i++) {
         if (itemText(i) == selectedItemText) {
            item = itemValues[i];
            break;
         }
      }
   }
   
   //
   // Set the current item
   //
   //if ((item >= 0) && (item < count())) {
      setCurrentIndex(item);
   //}
}

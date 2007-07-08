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
#include <set>

#include <QComboBox>
#include <QLayout>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "CellColorFile.h"
#include "CellFile.h"
#include "ColorFile.h"
#include "ContourCellColorFile.h"
#include "GuiMainWindow.h"
#include "FociProjectionFile.h"
#include "PaintFile.h"
#include "QtUtilities.h"
#include "StatisticTestNames.h"
#include "StereotaxicSpace.h"
#include "StringUtilities.h"
#include "VocabularyFile.h"
#include "VolumeFile.h"
#include "global_variables.h"

#define __NAME_DIALOG_MAIN__
#include "GuiNameSelectionDialog.h"
#undef __NAME_DIALOG_MAIN__

/**
 * Constructor
 */
GuiNameSelectionDialog::GuiNameSelectionDialog(QWidget* parent, 
                                               const unsigned int itemsToDisplay,
                                               const LIST_ITEMS_TYPE defaultItem) :
   QtDialog(parent, true)
{
   setWindowTitle("Name Selection");
   
   //
   // layout for dialog
   //
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(5);

   //
   // Combo box for name types
   //
   fileTypeComboBox = new QComboBox(this);
   rows->addWidget(fileTypeComboBox);
   QObject::connect(fileTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(fileTypeSelectionSlot(int)));
   
   areaColorsAlphaItemNumber        = -1;
   borderNamesAlphaItemNumber       = -1;
   borderColorsAlphaItemNumber      = -1;
   borderColorsFileItemNumber       = -1;
   cellColorsAlphaItemNumber        = -1;
   cellColorsFileItemNumber         = -1;
   fociNamesAlphaItemNumber         = -1;
   paintNamesAlphaItemNumber        = -1;
   contourCellColorsAlphaItemNumber = -1;
   contourCellColorsFileItemNumber  = -1;
   vocabularyAlphaItemNumber        = -1;
   stereotaxicSpaceItemNumber       = -1;
   statisticsItemNumber             = -1;
   volumePaintNamesAlphaOrderItemNumber = -1;
   
   int itemNumber = 0;
   defaultSelection = 0;
   
   if (itemsToDisplay & LIST_AREA_COLORS_ALPHA) {
      fileTypeComboBox->addItem("Area Colors (alphabetical)");
      if (defaultItem == LIST_AREA_COLORS_ALPHA) {
         defaultSelection = itemNumber;
      }
      areaColorsAlphaItemNumber = itemNumber++;
   }   
   if (itemsToDisplay & LIST_BORDER_NAMES_ALPHA) {
      fileTypeComboBox->addItem("Border Names (alphabetical)");
      if (defaultItem == LIST_BORDER_NAMES_ALPHA) {
         defaultSelection = itemNumber;
      }
      borderNamesAlphaItemNumber = itemNumber++;
   }   
   if (itemsToDisplay & LIST_BORDER_COLORS_ALPHA) {
      fileTypeComboBox->addItem("Border Colors (alphabetical)");
      if (defaultItem == LIST_BORDER_COLORS_ALPHA) {
         defaultSelection = itemNumber;
      }
      borderColorsAlphaItemNumber = itemNumber++;
   }   
   if (itemsToDisplay & LIST_BORDER_COLORS_FILE) {
      fileTypeComboBox->addItem("Border Colors (file order)");
      if (defaultItem == LIST_BORDER_COLORS_FILE) {
         defaultSelection = itemNumber;
      }
      borderColorsFileItemNumber = itemNumber++;
   }   
   if (itemsToDisplay & LIST_CELL_COLORS_ALPHA) {
      fileTypeComboBox->addItem("Cell Colors (alphabetical)");
      if (defaultItem == LIST_CELL_COLORS_ALPHA) {
         defaultSelection = itemNumber;
      }
      cellColorsAlphaItemNumber = itemNumber++;
   }   
   if (itemsToDisplay & LIST_CELL_COLORS_FILE) {
      fileTypeComboBox->addItem("Cell Colors (file order)");
      if (defaultItem == LIST_CELL_COLORS_FILE) {
         defaultSelection = itemNumber;
      }
      cellColorsFileItemNumber = itemNumber++;
   }   
   if (itemsToDisplay & LIST_CONTOUR_CELL_COLORS_ALPHA) {
      fileTypeComboBox->addItem("Contour Cell Colors (alphabetical)");
      if (defaultItem == LIST_CONTOUR_CELL_COLORS_ALPHA) {
         defaultSelection = itemNumber;
      }
      contourCellColorsAlphaItemNumber = itemNumber++;
   }   
   if (itemsToDisplay & LIST_CONTOUR_CELL_COLORS_FILE) {
      fileTypeComboBox->addItem("Contour Cell Colors (file order)");
      if (defaultItem == LIST_CONTOUR_CELL_COLORS_FILE) {
         defaultSelection = itemNumber;
      }
      contourCellColorsFileItemNumber = itemNumber++;
   }   
   if (itemsToDisplay & LIST_FOCI_NAMES_ALPHA) {
      fileTypeComboBox->addItem("Foci Names (alphabetical)");
      if (defaultItem == LIST_FOCI_NAMES_ALPHA) {
         defaultSelection = itemNumber;
      }
      fociNamesAlphaItemNumber = itemNumber++;      
   }   
   if (itemsToDisplay & LIST_PAINT_NAMES_ALPHA) {
      fileTypeComboBox->addItem("Paint Names (alphabetical)");
      if (defaultItem == LIST_PAINT_NAMES_ALPHA) {
         defaultSelection = itemNumber;
      }
      paintNamesAlphaItemNumber = itemNumber++;
   }  
   if (itemsToDisplay & LIST_STATISTICS) {
      fileTypeComboBox->addItem("Statistics");
      if (defaultItem == LIST_STATISTICS) {
         defaultSelection = itemNumber;
      }
      statisticsItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_STEREOTAXIC_SPACES) {
      fileTypeComboBox->addItem("Stereotaxic Spaces");
      if (defaultItem == LIST_STEREOTAXIC_SPACES) {
         defaultSelection = itemNumber;
      }
      stereotaxicSpaceItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_VOCABULARY_ALPHA) {
      fileTypeComboBox->addItem("Vocabulary Abbreviations (alphabetical)");
      if (defaultItem == LIST_VOCABULARY_ALPHA) {
         defaultSelection = itemNumber;
      }
      vocabularyAlphaItemNumber = itemNumber++;
   } 
   if (itemsToDisplay & LIST_VOLUME_PAINT_NAMES_ALPHA) {
      fileTypeComboBox->addItem("Volume Paint Names (alphabetical)");
      if (defaultItem == LIST_VOLUME_PAINT_NAMES_ALPHA) {
         defaultSelection = itemNumber;
      }
      volumePaintNamesAlphaOrderItemNumber = itemNumber++;
   }
   
   //
   // Listbox for names
   //
   nameListBox = new QListWidget(this);
   nameListBox->setMinimumSize(QSize(300, 300));
   rows->addWidget(nameListBox);
   QObject::connect(nameListBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                    this, SLOT(listBoxSelectionSlot(QListWidgetItem*)));
   QObject::connect(nameListBox, SIGNAL(itemClicked(QListWidgetItem*)),
                    this, SLOT(listBoxHighlightSlot(QListWidgetItem*)));
   
   //
   // Set initial selections
   //
   fileTypeComboBox->setCurrentIndex(defaultSelection);
   fileTypeSelectionSlot(defaultSelection);
   
   //
   // Dialog Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   rows->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* ok = new QPushButton("OK");
   QObject::connect(ok, SIGNAL(clicked()),
                  this, SLOT(accept()));
   buttonsLayout->addWidget(ok);

   //
   // Cancel button
   //
   QPushButton* cancel = new QPushButton("Cancel");
   QObject::connect(cancel, SIGNAL(clicked()),
                  this, SLOT(reject()));
   buttonsLayout->addWidget(cancel);
   
   QtUtilities::makeButtonsSameSize(ok, cancel);
}

/**
 * Get the selected item type.
 */
GuiNameSelectionDialog::LIST_ITEMS_TYPE 
GuiNameSelectionDialog::getSelectedItemType() const
{
   const int item = fileTypeComboBox->currentIndex();
   if (item == areaColorsAlphaItemNumber) {
      return LIST_AREA_COLORS_ALPHA;
   }
   else if (item == borderNamesAlphaItemNumber) {
      return LIST_BORDER_NAMES_ALPHA;
   }
   else if (item == borderColorsAlphaItemNumber) {
      return LIST_BORDER_COLORS_ALPHA;
   }
   else if (item == borderColorsFileItemNumber) {
      return LIST_BORDER_COLORS_FILE;
   }
   else if (item == cellColorsAlphaItemNumber) {
      return LIST_CELL_COLORS_ALPHA;
   }
   else if (item == cellColorsFileItemNumber) {
      return LIST_CELL_COLORS_FILE;
   }
   else if (item == fociNamesAlphaItemNumber) {
      return LIST_FOCI_NAMES_ALPHA;
   }
   else if (item == paintNamesAlphaItemNumber) {
      return LIST_PAINT_NAMES_ALPHA;
   }
   else if (item == contourCellColorsAlphaItemNumber) {
      return LIST_CONTOUR_CELL_COLORS_ALPHA;
   }
   else if (item == contourCellColorsFileItemNumber) {
      return LIST_CONTOUR_CELL_COLORS_FILE;
   }
   else if (item == stereotaxicSpaceItemNumber) {
      return LIST_STEREOTAXIC_SPACES;
   }
   else if (item == statisticsItemNumber) {
      return LIST_STATISTICS;
   }
   else if (item == vocabularyAlphaItemNumber) {
      return LIST_VOCABULARY_ALPHA;
   }
   else if (item == volumePaintNamesAlphaOrderItemNumber) {
      return LIST_VOLUME_PAINT_NAMES_ALPHA;
   }
   return LIST_NONE;
}
      
/**
 * Called when a name is selected (double clicked) in the list box.
 */
void
GuiNameSelectionDialog::listBoxSelectionSlot(QListWidgetItem* item)
{
   name = item->text();
   done(QDialog::Accepted);
}

/**
 * Called when a name is selected (single clicked) in the list box.
 */
void
GuiNameSelectionDialog::listBoxHighlightSlot(QListWidgetItem* item)
{
   name = item->text();
}

/**
 * Called when a file type radio button is pressed.  Load appropriate names.
 */
void
GuiNameSelectionDialog::fileTypeSelectionSlot(int buttNum)
{
   defaultSelection = buttNum;
   
   if (buttNum == areaColorsAlphaItemNumber) {
      loadAreaColorsAlphaOrder();
   }
   else if (buttNum == borderNamesAlphaItemNumber) {
      loadBorderNamesAlphaOrder();
   }
   else if (buttNum == borderColorsAlphaItemNumber) {
      loadBorderColorsAlphaOrder();
   }
   else if (buttNum == borderColorsFileItemNumber) {
      loadBorderColorsFileOrder();
   }
   else if (buttNum == cellColorsAlphaItemNumber) {
      loadCellColorsAlphaOrder();
   }
   else if (buttNum == cellColorsFileItemNumber) {
      loadCellColorsFileOrder();
   }
   else if (buttNum == contourCellColorsAlphaItemNumber) {
      loadContourCellColorsAlphaOrder();
   }
   else if (buttNum == contourCellColorsFileItemNumber) {
      loadContourCellColorsFileOrder();
   }
   else if (buttNum == fociNamesAlphaItemNumber) {
      loadFociNamesAlphaOrder();
   }
   else if (buttNum == paintNamesAlphaItemNumber) {
      loadPaintNamesAlphaOrder();
   }
   else if (buttNum == statisticsItemNumber) {
      loadStatistics();
   }
   else if (buttNum == stereotaxicSpaceItemNumber) {
      loadStereotaxicSpaces();
   }
   else if (buttNum == vocabularyAlphaItemNumber) {
      loadVocabularyAlphaOrder();
   }
   else if (buttNum == volumePaintNamesAlphaOrderItemNumber) {
      loadVolumePaintNamesAlpha();
   }
   else {
      nameListBox->clear();
   } 
}

/**
 * Add the names to the list box and sort if necessary.
 */
void
GuiNameSelectionDialog::addNamesToListBox(std::vector<QString>& names, const bool sortThem)
{
   nameListBox->clear();
   
   if (sortThem) {
      //
      // sort and remove duplicates
      //
      std::set<QString> sorted(names.begin(), names.end());
      names.clear();
      names.insert(names.begin(), sorted.begin(), sorted.end());
   }
   
   const unsigned int num = names.size();
   for (unsigned int i = 0; i < num; i++) {
      nameListBox->addItem(names[i]);
   }
   if (num > 0) {
      QListWidgetItem item(names[0]);
      listBoxHighlightSlot(&item);
      nameListBox->setCurrentRow(0);
   }
}

/**
 * Add a color file to the list box.
 */
void
GuiNameSelectionDialog::addColorFileToListBox(ColorFile* cf, const bool sortThem)
{
   std::vector<QString> names;
   const int num = cf->getNumberOfColors();
   for (int i = 0; i < num; i++) {
      names.push_back(cf->getColorNameByIndex(i));
   }
   addNamesToListBox(names, sortThem);
}

/**
 * load the names for paints in alphabetical order
 */ 
void
GuiNameSelectionDialog::loadPaintNamesAlphaOrder()
{
   std::vector<QString> names;
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   int num = pf->getNumberOfPaintNames();
   for (int i = 0; i < num; i++) {
      names.push_back(pf->getPaintNameFromIndex(i));
   }
   addNamesToListBox(names, true);
}

/**
 * load the names for area colors in alphabetical order
 */
void
GuiNameSelectionDialog::loadAreaColorsAlphaOrder()
{
   addColorFileToListBox(theMainWindow->getBrainSet()->getAreaColorFile(), true);
}

/**
 * load the names for the file type and order.
 */
void 
GuiNameSelectionDialog::loadVocabularyAlphaOrder()
{
   std::vector<QString> names;
   const VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();
   const int num = vf->getNumberOfVocabularyEntries();
   for (int i = 0; i < num; i++) {
      const VocabularyFile::VocabularyEntry* ve = vf->getVocabularyEntry(i);
      names.push_back(ve->getAbbreviation());
   }
   addNamesToListBox(names, true);
}
      
/** 
 * load the name of foci in alphabetical order
 */
void
GuiNameSelectionDialog::loadFociNamesAlphaOrder()
{
   std::vector<QString> names;
   
   FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   const int num = fpf->getNumberOfCellProjections();
   for (int j = 0; j < num; j++) {
      names.push_back(fpf->getCellProjection(j)->getName());
   }
   addNamesToListBox(names, true);
}

/**
 * load the names for border names in alphabetical order
 */
void
GuiNameSelectionDialog::loadBorderNamesAlphaOrder()
{
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   std::vector<QString> names;
   bmbs->getAllBorderNames(names);
   addNamesToListBox(names, true);
}

/**
 * load the names for border colors in alphabetical order
 */
void
GuiNameSelectionDialog::loadBorderColorsAlphaOrder()
{
   addColorFileToListBox(theMainWindow->getBrainSet()->getBorderColorFile(), true);
}

/**
 * load the names for border colors in file order
 */
void
GuiNameSelectionDialog::loadBorderColorsFileOrder()
{
   addColorFileToListBox(theMainWindow->getBrainSet()->getBorderColorFile(), false);
}

/**
 * load the names for cell colors in alphabetical order
 */
void
GuiNameSelectionDialog::loadCellColorsAlphaOrder()
{
   addColorFileToListBox(theMainWindow->getBrainSet()->getCellColorFile(), true);
}

/**
 * load the names for cell colors in file order
 */
void
GuiNameSelectionDialog::loadCellColorsFileOrder()
{
   addColorFileToListBox(theMainWindow->getBrainSet()->getCellColorFile(), false);
}

/**
 * load the names for cell colors in alphabetical order
 */
void
GuiNameSelectionDialog::loadContourCellColorsAlphaOrder()
{
   addColorFileToListBox(theMainWindow->getBrainSet()->getContourCellColorFile(), true);
}

/**
 * load the names for cell colors in file order
 */
void
GuiNameSelectionDialog::loadContourCellColorsFileOrder()
{
   addColorFileToListBox(theMainWindow->getBrainSet()->getContourCellColorFile(), false);
}

/**
 * Destructor
 */
GuiNameSelectionDialog::~GuiNameSelectionDialog()
{
}

/**
 * load the stereotaxic space names.
 */
void 
GuiNameSelectionDialog::loadStereotaxicSpaces()
{
   std::vector<QString> names;
   std::vector<StereotaxicSpace> allSpaces;
   StereotaxicSpace::getAllStereotaxicSpaces(allSpaces);
   for (unsigned int i = 0; i < allSpaces.size(); i++) {
      names.push_back(allSpaces[i].getName());
   }
   addNamesToListBox(names, true);
}

/**
 * load the statistics.
 */
void 
GuiNameSelectionDialog::loadStatistics()
{
   std::vector<std::string> names;
   StatisticTestNames::getTestNames(names);
   std::vector<QString> names2;
   for (unsigned int i = 0; i < names.size(); i++) {
      names2.push_back(StringUtilities::fromStdString(names[i]));
   }
   addNamesToListBox(names2, true);
}

/**
 * load volume paint name in alpha order.
 */
void 
GuiNameSelectionDialog::loadVolumePaintNamesAlpha()
{
   std::vector<QString> names;
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelVolume* bmv = bs->getBrainModelVolume();
   VolumeFile* paintVolume = bmv->getSelectedVolumePaintFile();
   if (paintVolume != NULL) {
      const int numNames = paintVolume->getNumberOfRegionNames();
      for (int i = 0; i < numNames; i++) {
         names.push_back(paintVolume->getRegionNameFromIndex(i));
      }
   }
   addNamesToListBox(names, true);
}

      

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
#include "Species.h"
#include "StatisticTestNames.h"
#include "StereotaxicSpace.h"
#include "StringUtilities.h"
#include "Structure.h"
#include "StudyMetaDataFile.h"
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
                                               const LIST_ITEMS_TYPE defaultItem,
                                               const bool allowMultipleSelectionsFlagIn) :
   WuQDialog(parent)
{
   setModal(true);
   setWindowTitle("Name Selection");
   allowMultipleSelectionsFlag = allowMultipleSelectionsFlagIn;
   
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
   fociAreasAlphaItemNumber         = -1;
   fociClassesAlphaItemNumber       = -1;
   fociCommentAlphaItemNumber       = -1;
   fociGeographyAlphaItemNumber     = -1;
   fociNamesAlphaItemNumber         = -1;
   fociRegionOfInterestAlphaItemNumber = -1;
   paintNamesAlphaItemNumber        = -1;
   contourCellColorsAlphaItemNumber = -1;
   contourCellColorsFileItemNumber  = -1;
   vocabularyAlphaItemNumber        = -1;
   speciesItemNumber                = -1;
   stereotaxicSpaceItemNumber       = -1;
   statisticsItemNumber             = -1;
   structuresItemNumber             = -1;
   studyCitationItemNumber          = -1;
   studyDataFormatItemNumber        = -1;
   studyDataTypeItemNumber          = -1;
   studyKeywordsItemNumber          = -1;
   studyMedicalSubjectHeadingsItemNumber = -1;
   studyTableHeadersItemNumber      = -1;
   studyTableSubHeadersItemNumber   = -1;
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
   if (itemsToDisplay & LIST_FOCI_AREAS_ALPHA) {
      fileTypeComboBox->addItem("Foci Areas (alphabetical)");
      if (defaultItem == LIST_FOCI_AREAS_ALPHA) {
         defaultSelection = itemNumber;
      }
      fociAreasAlphaItemNumber = itemNumber++;      
   }   
   if (itemsToDisplay & LIST_FOCI_CLASSES_ALPHA) {
      fileTypeComboBox->addItem("Foci Classes (alphabetical)");
      if (defaultItem == LIST_FOCI_CLASSES_ALPHA) {
         defaultSelection = itemNumber;
      }
      fociClassesAlphaItemNumber = itemNumber++;      
   }   
   if (itemsToDisplay & LIST_FOCI_COMMENT_ALPHA) {
      fileTypeComboBox->addItem("Foci Comments (alphabetical)");
      if (defaultItem == LIST_FOCI_COMMENT_ALPHA) {
         defaultSelection = itemNumber;
      }
      fociCommentAlphaItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_FOCI_GEOGRAPHY_ALPHA) {
      fileTypeComboBox->addItem("Foci Geography (alphabetical)");
      if (defaultItem == LIST_FOCI_GEOGRAPHY_ALPHA) {
         defaultSelection = itemNumber;
      }
      fociGeographyAlphaItemNumber = itemNumber++;      
   }   
   if (itemsToDisplay & LIST_FOCI_NAMES_ALPHA) {
      fileTypeComboBox->addItem("Foci Names (alphabetical)");
      if (defaultItem == LIST_FOCI_NAMES_ALPHA) {
         defaultSelection = itemNumber;
      }
      fociNamesAlphaItemNumber = itemNumber++;      
   }   
   if (itemsToDisplay & LIST_FOCI_ROI_ALPHA) {
      fileTypeComboBox->addItem("Foci ROI (alphabetical)");
      if (defaultItem == LIST_FOCI_ROI_ALPHA) {
        defaultSelection = itemNumber;
      }
      fociRegionOfInterestAlphaItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_PAINT_NAMES_ALPHA) {
      fileTypeComboBox->addItem("Paint Names (alphabetical)");
      if (defaultItem == LIST_PAINT_NAMES_ALPHA) {
         defaultSelection = itemNumber;
      }
      paintNamesAlphaItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_SPECIES) {
      fileTypeComboBox->addItem("Species");
      if (defaultItem == LIST_SPECIES) {
         defaultSelection = itemNumber;
      }
      speciesItemNumber = itemNumber++;
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
   if (itemsToDisplay & LIST_STRUCTURE) {
      fileTypeComboBox->addItem("Structure");
      if (defaultItem == LIST_STRUCTURE) {
         defaultSelection = itemNumber;
      }
      structuresItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_STUDY_CITATION) {
      fileTypeComboBox->addItem("Study Citation");
      if (defaultItem == LIST_STUDY_CITATION) {
         defaultSelection = itemNumber;
      }
      studyCitationItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_STUDY_DATA_FORMAT) {
      fileTypeComboBox->addItem("Study Data Format");
      if (defaultItem == LIST_STUDY_DATA_FORMAT) {
         defaultSelection = itemNumber;
      }
      studyDataFormatItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_STUDY_DATA_TYPE) {
      fileTypeComboBox->addItem("Study Data Type");
      if (defaultItem == LIST_STUDY_DATA_TYPE) {
         defaultSelection = itemNumber;
      }
      studyDataTypeItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_STUDY_KEYWORDS_ALPHA) {
      fileTypeComboBox->addItem("Study Keywords (alphabetical)");
      if (defaultItem == LIST_STUDY_KEYWORDS_ALPHA) {
         defaultSelection = itemNumber;
      }
      studyKeywordsItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_STUDY_MESH_ALPHA) {
      fileTypeComboBox->addItem("Study Medical Subject Headings (alphabetical)");
      if (defaultItem == LIST_STUDY_MESH_ALPHA) {
         defaultSelection = itemNumber;
      }
      studyMedicalSubjectHeadingsItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_STUDY_TABLE_HEADERS) {
      fileTypeComboBox->addItem("Study Table Headers (alphabetical)");
      if (defaultItem == LIST_STUDY_TABLE_HEADERS) {
         defaultSelection = itemNumber;
      }
      studyTableHeadersItemNumber = itemNumber++;
   }
   if (itemsToDisplay & LIST_STUDY_TABLE_SUBHEADERS) {
      fileTypeComboBox->addItem("Study Table Tab SubHeaders (alphabetical)");
      if (defaultItem == LIST_STUDY_TABLE_SUBHEADERS) {
         defaultSelection = itemNumber;
      }
      studyTableSubHeadersItemNumber = itemNumber++;
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
   if (allowMultipleSelectionsFlag) {
      nameListBox->setSelectionMode(QListWidget::ExtendedSelection);
   }
   else {
      nameListBox->setSelectionMode(QListWidget::SingleSelection);
   }
   rows->addWidget(nameListBox);
   
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
 * Get the names that were selected (use if multiple selections).
 */
QStringList 
GuiNameSelectionDialog::getNamesSelected() const
{
   QStringList sl;
   
   const QList<QListWidgetItem*> selectedItems = nameListBox->selectedItems();
   const int num = selectedItems.size();
   for (int i = 0; i < num; i++) {
      sl << selectedItems.at(i)->text();
   }
   
   return sl;
}

/**
 * Get the name that was selected (use if single selections).
 */
QString 
GuiNameSelectionDialog::getNameSelected() const
{
   const QStringList nameList = getNamesSelected();
   if (nameList.size() > 0) {
      return nameList.at(0);
   }
   
   return "";
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
   else if (item == fociAreasAlphaItemNumber) {
      return LIST_FOCI_AREAS_ALPHA;
   }
   else if (item == fociClassesAlphaItemNumber) {
      return LIST_FOCI_CLASSES_ALPHA;
   }
   else if (item == fociCommentAlphaItemNumber) {
      return LIST_FOCI_COMMENT_ALPHA;
   }
   else if (item == fociGeographyAlphaItemNumber) {
      return LIST_FOCI_GEOGRAPHY_ALPHA;
   }
   else if (item == fociNamesAlphaItemNumber) {
      return LIST_FOCI_NAMES_ALPHA;
   }
   else if (item == fociRegionOfInterestAlphaItemNumber) {
      return LIST_FOCI_ROI_ALPHA;
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
   else if (item == speciesItemNumber) {
      return LIST_SPECIES;
   }
   else if (item == stereotaxicSpaceItemNumber) {
      return LIST_STEREOTAXIC_SPACES;
   }
   else if (item == structuresItemNumber) {
      return LIST_STRUCTURE;
   }
   else if (item == statisticsItemNumber) {
      return LIST_STATISTICS;
   }
   else if (item == studyCitationItemNumber) {
      return LIST_STUDY_CITATION;
   }
   else if (item == studyDataFormatItemNumber) {
      return LIST_STUDY_DATA_FORMAT;
   }
   else if (item == studyDataTypeItemNumber) {
      return LIST_STUDY_DATA_TYPE;
   }
   else if (item == studyKeywordsItemNumber) {
      return LIST_STUDY_KEYWORDS_ALPHA;
   }
   else if (item == studyMedicalSubjectHeadingsItemNumber) {
      return LIST_STUDY_MESH_ALPHA;
   }
   else if (item == studyTableHeadersItemNumber) {
      return LIST_STUDY_TABLE_HEADERS;
   }
   else if (item == studyTableSubHeadersItemNumber) {
      return LIST_STUDY_TABLE_SUBHEADERS;
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
   else if (buttNum == fociAreasAlphaItemNumber) {
      loadFociAreasAlphaOrder();
   }
   else if (buttNum == fociClassesAlphaItemNumber) {
      loadFociClassesAlphaOrder();
   }
   else if (buttNum == fociCommentAlphaItemNumber) {
      loadFociCommentAlphaOrder();
   }
   else if (buttNum == fociGeographyAlphaItemNumber) {
      loadFociGeographyAlphaOrder();
   }
   else if (buttNum == fociNamesAlphaItemNumber) {
      loadFociNamesAlphaOrder();
   }
   else if (buttNum == fociRegionOfInterestAlphaItemNumber) {
      loadFociROIAlphaOrder();
   }
   else if (buttNum == paintNamesAlphaItemNumber) {
      loadPaintNamesAlphaOrder();
   }
   else if (buttNum == speciesItemNumber) {
      loadSpecies();
   }
   else if (buttNum == statisticsItemNumber) {
      loadStatistics();
   }
   else if (buttNum == stereotaxicSpaceItemNumber) {
      loadStereotaxicSpaces();
   }
   else if (buttNum == structuresItemNumber) {
      loadStructures();
   }
   else if (buttNum == studyCitationItemNumber) {
      loadStudyCitation();
   }
   else if (buttNum == studyDataFormatItemNumber) {
      loadStudyDataFormat();
   }
   else if (buttNum == studyDataTypeItemNumber) {
      loadStudyDataType();
   }
   else if (buttNum == studyKeywordsItemNumber) {
      loadStudyKeywords();
   }
   else if (buttNum == studyMedicalSubjectHeadingsItemNumber) {
      loadStudyMedialSubjectHeadings();
   }
   else if (buttNum == studyTableHeadersItemNumber) {
      loadStudyTableHeaders();
   }
   else if (buttNum == studyTableSubHeadersItemNumber) {
      loadStudyTableSubHeaders();
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
   
   //
   // Remove blanks
   //
   std::remove(names.begin(), names.end(), "");
   
   if (sortThem) {
      //
      // sort and remove duplicates
      //
      StringUtilities::sortCaseInsensitive(names, false, true);
      //std::set<QString> sorted(names.begin(), names.end());
      //names.clear();
      //names.insert(names.begin(), sorted.begin(), sorted.end());
   }
   
   const unsigned int num = names.size();
   for (unsigned int i = 0; i < num; i++) {
      nameListBox->addItem(names[i]);
   }
   if (num > 0) {
      QListWidgetItem item(names[0]);
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
 * load the name for the file type and order.
 */
void 
GuiNameSelectionDialog::loadFociAreasAlphaOrder()
{
   std::vector<QString> names;
   FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   fpf->getAllCellAreas(names);
   addNamesToListBox(names, true);
}

/**
 * load the names for the file type and order.
 */
void 
GuiNameSelectionDialog::loadFociCommentAlphaOrder()
{
   std::vector<QString> names;
   FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   fpf->getAllCellComments(names);
   addNamesToListBox(names, true);
}
      
/**
 * load the name for the file type and order.
 */
void 
GuiNameSelectionDialog::loadFociGeographyAlphaOrder()
{
   std::vector<QString> names;
   
   FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   fpf->getAllCellGeography(names);
   addNamesToListBox(names, true);
}
      
/**
 * load the name for the file type and order.
 */
void 
GuiNameSelectionDialog::loadFociROIAlphaOrder()
{
   std::vector<QString> names;
   
   FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   fpf->getAllCellRegionsOfInterest(names);
   addNamesToListBox(names, true);
}
      
/**
 * load the study citations.
 */
void 
GuiNameSelectionDialog::loadStudyCitation()
{
   std::vector<QString> names;
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   smdf->getAllCitations(names);
   addNamesToListBox(names, true);
}

/**
 * load the study data format.
 */
void 
GuiNameSelectionDialog::loadStudyDataFormat()
{
   std::vector<QString> names;
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   smdf->getAllDataFormats(names);
   addNamesToListBox(names, true);
}

/**
 * load the study data type.
 */
void 
GuiNameSelectionDialog::loadStudyDataType()
{
   std::vector<QString> names;
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   smdf->getAllDataTypes(names);
   addNamesToListBox(names, true);
}
      
/**
 * load the name for the file type and order.
 */
void 
GuiNameSelectionDialog::loadStudyKeywords()
{
   std::vector<QString> names;
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   smdf->getAllKeywords(names);
   addNamesToListBox(names, true);
}

/**
 * load the name for the file type and order.
 */
void 
GuiNameSelectionDialog::loadStudyMedialSubjectHeadings()
{
   std::vector<QString> names;
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   smdf->getAllMedicalSubjectHeadings(names);
   addNamesToListBox(names, true);
}
      
/**
 * load study table headers.
 */
void 
GuiNameSelectionDialog::loadStudyTableHeaders()
{
   std::vector<QString> names;
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   smdf->getAllTableHeaders(names);
   addNamesToListBox(names, true);
}

/**
 * load study table subheaders.
 */
void 
GuiNameSelectionDialog::loadStudyTableSubHeaders()
{
   std::vector<QString> names;
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   smdf->getAllTableSubHeaderShortNames(names);
   addNamesToListBox(names, true);
}

/**
 * load the name for the file type and order.
 */
void 
GuiNameSelectionDialog::loadFociClassesAlphaOrder()
{
   std::vector<QString> names;
   
   FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   const int num = fpf->getNumberOfCellProjections();
   for (int j = 0; j < num; j++) {
      names.push_back(fpf->getCellProjection(j)->getClassName());
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
   bmbs->getAllBorderNames(names, false);
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
 * load the species names.
 */
void 
GuiNameSelectionDialog::loadSpecies()
{
   std::vector<Species::TYPE> speciesTypes;
   std::vector<QString> speciesNames;
   Species::getAllSpeciesTypesAndNames(speciesTypes, speciesNames);
   addNamesToListBox(speciesNames, true);
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
 * load the structures.
 */
void 
GuiNameSelectionDialog::loadStructures()
{
   std::vector<Structure::STRUCTURE_TYPE> types;
   std::vector<QString> names;
   Structure::getAllTypesAndNames(types, names, true, true);
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

      

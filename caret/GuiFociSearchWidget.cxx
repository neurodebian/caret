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

#include <iostream>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalMapper>
#include <QSpinBox>
#include <QToolButton>

#include "BrainModelSurfaceFociSearch.h"
#include "BrainSet.h"
#include "DisplaySettingsFoci.h"
#include "FociProjectionFile.h"
#include "FociSearchFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFociSearchWidget.h"
#include "GuiMainWindow.h"
#include "GuiNameSelectionDialog.h"
#include "WuQDataEntryDialog.h"
#include "WuQWidgetGroup.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiFociSearchWidget::GuiFociSearchWidget(FociProjectionFile* fociProjectionFileIn,
                                         FociSearchFile* fociSearchFileIn,
                                         const bool useScrollAreaFlag,
                                         QWidget* parent)
   : QWidget(parent)
{
   fociProjectionFile = fociProjectionFileIn;
   fociSearchFile     = fociSearchFileIn;
   
   //
   // Create signal mappers
   //
   insertSearchToolButtonSignalMapper = new QSignalMapper(this);
   QObject::connect(insertSearchToolButtonSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotInsertSearchToolButtonClicked(int)));
   deleteSearchToolButtonSignalMapper = new QSignalMapper(this);
   QObject::connect(deleteSearchToolButtonSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotDeleteSearchToolButtonClicked(int)));
   searchLogicComboBoxSignalMapper = new QSignalMapper(this);
   QObject::connect(searchLogicComboBoxSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotSearchLogicChanged(int)));
   searchAttributeComboBoxSignalMapper = new QSignalMapper(this);
   QObject::connect(searchAttributeComboBoxSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotSearchAttributeChanged(int)));
   searchMatchingComboBoxSignalMapper = new QSignalMapper(this);
   QObject::connect(searchMatchingComboBoxSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotSearchMatchingChanged(int)));
   searchTextLineEditSignalMapper = new QSignalMapper(this);
   QObject::connect(searchTextLineEditSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotSearchTextChanged(int)));
   assistanceToolButtonSignalMapper = new QSignalMapper(this);
   QObject::connect(assistanceToolButtonSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotAssistanceToolButtonClicked(int)));
                                     
   //
   // Create the widget groups
   //
   QWidget* searchSelectionWidget  = createSearchSetGroup();
   QWidget* searchOperationsWidget    = createSearchOperationsGroup();
   //QWidget* searchSurfacesWidget   = createSearchSurfacesGroup();
   QWidget* searchParametersWidget = createSearchParametersGroup(useScrollAreaFlag);
   
   //
   // Layout for this widget
   //
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(searchSelectionWidget);
   layout->addWidget(searchOperationsWidget);
   layout->addWidget(searchParametersWidget);
   //layout->addWidget(searchSurfacesWidget);
   layout->addStretch();
   
   loadFociSearches();
}

/**
 * destructor.
 */
GuiFociSearchWidget::~GuiFociSearchWidget()
{
}
                     
/**
 * create the search operation group.
 */
QWidget* 
GuiFociSearchWidget::createSearchOperationsGroup()
{
   //
   // Search All button
   //
   QPushButton* searchAllPushButton = new QPushButton("Search All");
   searchAllPushButton->setFixedSize(searchAllPushButton->sizeHint());
   searchAllPushButton->setAutoDefault(false);
   QObject::connect(searchAllPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSearchAllPushButton()));
   searchAllPushButton->setToolTip("Apply the search \n"
                                "parameters to ALL foci.");                 

   //
   // Search Displayed button
   //
   QPushButton* searchDisplayedPushButton = new QPushButton("Search Displayed");
   searchDisplayedPushButton->setFixedSize(searchDisplayedPushButton->sizeHint());
   searchDisplayedPushButton->setAutoDefault(false);
   QObject::connect(searchDisplayedPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSearchDisplayedPushButton()));
   searchDisplayedPushButton->setToolTip("Apply the search parameters\n"
                                         "to DISPLAYED foci.");                 

   //
   // Reset button
   //
   QPushButton* resetPushButton = new QPushButton("Reset");
   resetPushButton->setFixedSize(resetPushButton->sizeHint());
   resetPushButton->setAutoDefault(false);
   QObject::connect(resetPushButton, SIGNAL(clicked()),
                    this, SLOT(slotResetPushButton()));
   resetPushButton->setToolTip("Cease application of search \n"
                                "parameters to the foci.");                 
                    
   //
   // Buttons layout
   //
   QHBoxLayout* applySearchLayout = new QHBoxLayout;
   applySearchLayout->addWidget(searchAllPushButton);
   applySearchLayout->addWidget(searchDisplayedPushButton);
   applySearchLayout->addWidget(resetPushButton);
   applySearchLayout->addStretch();
   
   //
   // Show foci in matching studies check box
   //
   showFociFromMatchingStudiesCheckBox = new QCheckBox("Show Foci From Matching Studies");
   
   //
   // Search results label
   //
   searchResultsLabel = new QLabel("                                     ");
   
   //
   // Search options group box and layout
   //
   QGroupBox* searchOperationsGroupBox = new QGroupBox("Search Foci Operations");
   QVBoxLayout* searchOperationsGroupLayout = new QVBoxLayout(searchOperationsGroupBox);
   searchOperationsGroupLayout->addLayout(applySearchLayout);
   searchOperationsGroupLayout->addWidget(showFociFromMatchingStudiesCheckBox);
   searchOperationsGroupLayout->addWidget(searchResultsLabel);

   return searchOperationsGroupBox;
}
      
/**
 * create the search set group.
 */
QWidget* 
GuiFociSearchWidget::createSearchSetGroup()
{
   //
   // Search number pushbutton
   //
   QPushButton* searchNumberPushButton = new QPushButton("Search Number...");
   searchNumberPushButton->setFixedSize(searchNumberPushButton->sizeHint());
   searchNumberPushButton->setAutoDefault(false);
   QObject::connect(searchNumberPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSearchNumberPushButton()));
   
   //
   // Search number spin box
   //
   searchSelectionSpinBox = new QSpinBox;
   searchSelectionSpinBox->setSingleStep(1);
   QObject::connect(searchSelectionSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSearchSelectionSpinBox(int)));
   
   //
   // Delete search button
   //
   QPushButton* deleteSearchPushButton = new QPushButton("Delete Selected Search Set");
   deleteSearchPushButton->setAutoDefault(false);
   deleteSearchPushButton->setFixedSize(deleteSearchPushButton->sizeHint());
   QObject::connect(deleteSearchPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteSearchPushButton()));
   //
   // Add search button
   //
   QPushButton* addSearchPushButton = new QPushButton("Add New Search Set");
   addSearchPushButton->setAutoDefault(false);
   addSearchPushButton->setFixedSize(addSearchPushButton->sizeHint());
   QObject::connect(addSearchPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddSearchPushButton()));
                    
   //
   // Copy Search Set button
   //
   QPushButton* copySearchPushButton = new QPushButton("Copy Search Set");
   copySearchPushButton->setAutoDefault(false);
   copySearchPushButton->setFixedSize(copySearchPushButton->sizeHint());
   QObject::connect(copySearchPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCopySearchSetPushButton()));
                    
   //
   // Layout
   //
   QHBoxLayout* searchSelectionLayout = new QHBoxLayout;
   searchSelectionLayout->addWidget(searchNumberPushButton);
   searchSelectionLayout->addWidget(searchSelectionSpinBox);
   searchSelectionLayout->addWidget(deleteSearchPushButton);
   searchSelectionLayout->addStretch();
   
   //
   // Layout
   //
   QHBoxLayout* newSearchLayout = new QHBoxLayout;
   newSearchLayout->addWidget(addSearchPushButton);
   newSearchLayout->addWidget(copySearchPushButton);
   newSearchLayout->addStretch();
               
   //
   // Search selection group box and layout
   //
   QGroupBox* searchSelectionGroupBox = new QGroupBox("Search Set Selection");
   QVBoxLayout* searchSelectionGroupLayout = new QVBoxLayout(searchSelectionGroupBox);
   searchSelectionGroupLayout->addLayout(searchSelectionLayout);
   searchSelectionGroupLayout->addLayout(newSearchLayout);

   return searchSelectionGroupBox;
}

/**
 * create the search parameters group.
 */
QWidget* 
GuiFociSearchWidget::createSearchParametersGroup(const bool useScrollAreaFlag)
{
   //
   // name of search line edit
   //
   searchNameLabel = new QLabel("Search Name ");
   searchNameLineEdit = new QLineEdit;
   QObject::connect(searchNameLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotSearchNameLineEdit(const QString&)));
   QHBoxLayout* searchNameLayout = new QHBoxLayout;
   searchNameLayout->addWidget(searchNameLabel);
   searchNameLayout->addWidget(searchNameLineEdit);
   searchNameLayout->setStretchFactor(searchNameLabel, 0);
   searchNameLayout->setStretchFactor(searchNameLineEdit, 100);
                    
   //
   // Widget and layout for searches
   //
   QWidget* searchWidget = new QWidget;
   searchGridLayout = new QGridLayout(searchWidget);
   
   //
   // Set column stretching
   //
   searchGridLayout->setColumnStretch(0, 0);
   searchGridLayout->setColumnStretch(1, 0);
   searchGridLayout->setColumnStretch(2, 0);
   searchGridLayout->setColumnStretch(3, 0);
   searchGridLayout->setColumnStretch(4, 0);
   searchGridLayout->setColumnStretch(5, 0);
   searchGridLayout->setColumnStretch(6, 1000);
      
   //
   // Column Titles
   //
   searchGridLayout->addWidget(new QLabel("Logic"), 0, 3, Qt::AlignLeft);
   searchGridLayout->addWidget(new QLabel("Categories"), 0, 4, Qt::AlignLeft);
   searchGridLayout->addWidget(new QLabel("Matching"), 0, 5, Qt::AlignLeft);
   
   //
   // scroll view and layout for search lines
   //
   QWidget* theSearchWidget = searchWidget;
   searchScrollArea = NULL;
   if (useScrollAreaFlag) {
      searchScrollArea = new QScrollArea;
      searchScrollArea->setWidget(searchWidget);
      searchScrollArea->setWidgetResizable(true);
      theSearchWidget = searchScrollArea;
   }
   
   //
   // Widget and layout for searches
   //
   QGroupBox* searchParametersGroupBox = new QGroupBox("Search Parameters");
   QVBoxLayout* searchParametersLayout = new QVBoxLayout(searchParametersGroupBox);
   searchParametersLayout->addLayout(searchNameLayout);
   searchParametersLayout->addWidget(theSearchWidget);
   //searchParametersLayout->addStretch();
   
   return searchParametersGroupBox;
}


/**
 * create the search surfaces group.
 */
/*
QWidget* 
GuiFociSearchWidget::createSearchSurfacesGroup()
{
   //
   // Left surface selection
   //
   QLabel* leftLabel = new QLabel("Left");
   leftSurfaceSelectionComboBox = 
      new GuiBrainModelSelectionComboBox((GuiBrainModelSelectionComboBox::OPTION_SHOW_SURFACES_FIDUCIAL
                                          | GuiBrainModelSelectionComboBox::OPTION_SHOW_ADD_NEW),
                                         "None");
   leftSurfaceSelectionComboBox->setSurfaceStructureRequirement(
      Structure::STRUCTURE_TYPE_CORTEX_LEFT);
      
   //
   // Left surface selection
   //
   QLabel* rightLabel = new QLabel("Right");
   rightSurfaceSelectionComboBox = 
      new GuiBrainModelSelectionComboBox((GuiBrainModelSelectionComboBox::OPTION_SHOW_SURFACES_FIDUCIAL
                                          | GuiBrainModelSelectionComboBox::OPTION_SHOW_ADD_NEW),
                                         "None");
   rightSurfaceSelectionComboBox->setSurfaceStructureRequirement(
      Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
      
   //
   // Cerebellum surface selection
   //
   QLabel* cerebellumLabel = new QLabel("Cerebellum");
   cerebellumSurfaceSelectionComboBox = 
      new GuiBrainModelSelectionComboBox((GuiBrainModelSelectionComboBox::OPTION_SHOW_SURFACES_FIDUCIAL
                                          | GuiBrainModelSelectionComboBox::OPTION_SHOW_ADD_NEW),
                                         "None");
   cerebellumSurfaceSelectionComboBox->setSurfaceStructureRequirement(
      Structure::STRUCTURE_TYPE_CEREBELLUM);
      
   //
   // Widget and layout for searches
   //
   QGroupBox* searchSurfacesGroupBox = new QGroupBox("Spatial Search Surfaces");
   QGridLayout* searchSurfacesGridLayout = new QGridLayout(searchSurfacesGroupBox);
   searchSurfacesGridLayout->addWidget(leftLabel, 0, 0);
   searchSurfacesGridLayout->addWidget(leftSurfaceSelectionComboBox, 0, 1);
   searchSurfacesGridLayout->addWidget(rightLabel, 1, 0);
   searchSurfacesGridLayout->addWidget(rightSurfaceSelectionComboBox, 1, 1);
   searchSurfacesGridLayout->addWidget(cerebellumLabel, 2, 0);
   searchSurfacesGridLayout->addWidget(cerebellumSurfaceSelectionComboBox, 2, 1);
   searchSurfacesGridLayout->setColumnStretch(0, 0);
   searchSurfacesGridLayout->setColumnStretch(1, 100);
   
   return searchSurfacesGroupBox;
}
*/

/**
 * called when add search button pressed.
 */
void 
GuiFociSearchWidget::slotAddSearchPushButton()
{
   FociSearchSet* fss = new FociSearchSet;
   fociSearchFile->addFociSearchSet(fss);
   
   //
   // Note that "searchSelectionSpinBox" indexes 1..N
   //
   loadFociSearches();
   searchSelectionSpinBox->setValue(fociSearchFile->getNumberOfFociSearchSets());
   slotSearchSelectionSpinBox(searchSelectionSpinBox->value());
}

/**
 * called when delete search button pressed.
 */
void 
GuiFociSearchWidget::slotDeleteSearchPushButton()
{
   //
   // Note that "searchSelectionSpinBox" indexes 1..N
   //
   const int searchNumber = searchSelectionSpinBox->value() - 1;
   if ((searchNumber >= 0) &&
       (searchNumber < fociSearchFile->getNumberOfFociSearchSets())) {
      if (QMessageBox::question(this,
                                "CONFIRM",
                                "Delete current search set?",
                                (QMessageBox::Ok | QMessageBox::Cancel)) ==
                        QMessageBox::Ok) {
         fociSearchFile->deleteFociSearchSet(searchNumber);
      }
   }
   loadFociSearches();
}

/**
 * called when search number button pressed.
 */
void 
GuiFociSearchWidget::slotSearchNumberPushButton()
{
   const int numSearchSets = fociSearchFile->getNumberOfFociSearchSets();
   if (numSearchSets <= 0) {
      return;
   }
   
   //
   // Get the names of all of the search sets
   //
   QStringList searchSetNamesStringList;
   for (int i = 0; i < numSearchSets; i++) {
      const FociSearchSet* fss = fociSearchFile->getFociSearchSet(i);
      searchSetNamesStringList << fss->getName();
   }
   
   //
   // Popup a dialog to allow the user to choose a search set
   //
   WuQDataEntryDialog ded(this);
   ded.setWindowTitle("Select Search Set");
   QListWidget* lw = ded.addListWidget("", searchSetNamesStringList);
   if (ded.exec() == WuQDataEntryDialog::Accepted) {
      const int searchSetNumber = lw->currentRow();
      
      //
      // Note that "searchSelectionSpinBox" indexes 1..N
      //
      searchSelectionSpinBox->setValue(searchSetNumber + 1);
      slotSearchSelectionSpinBox(searchSelectionSpinBox->value());
   }
}      

/**
 * called when copy search set button pressed.
 */
void 
GuiFociSearchWidget::slotCopySearchSetPushButton()
{
   //
   // Copy search set
   //
   const int searchNumber = searchSelectionSpinBox->value() - 1;
   if ((searchNumber >= 0) &&
       (searchNumber < fociSearchFile->getNumberOfFociSearchSets())) {
      fociSearchFile->copySearchSetToNewSearchSet(searchNumber);
   }

   //
   // Note that "searchSelectionSpinBox" indexes 1..N
   //
   loadFociSearches();
   searchSelectionSpinBox->setValue(fociSearchFile->getNumberOfFociSearchSets());
   slotSearchSelectionSpinBox(searchSelectionSpinBox->value());
}
      
/**
 * update the widget.
 */
void 
GuiFociSearchWidget::updateWidget(FociProjectionFile* fpf,
                                  FociSearchFile* fsf)
{
   fociProjectionFile = fpf;
   fociSearchFile     = fsf;
   
   //leftSurfaceSelectionComboBox->updateComboBox();
   //rightSurfaceSelectionComboBox->updateComboBox();
   //cerebellumSurfaceSelectionComboBox->updateComboBox();
   
   loadFociSearches();
}
                        
/**
 * called when search selection spin box value is changed.
 */
void 
GuiFociSearchWidget::slotSearchSelectionSpinBox(int)
{
   loadFociSearches();
}

/**
 * load the foci searches.
 */
void 
GuiFociSearchWidget::loadFociSearches()
{
   //
   // Get active search
   //
   const FociSearchSet* fss = getSelectedSearchSet();

   //
   // If no search sets, hide all searches
   //
   const int numSearchesInDialog = static_cast<int>(searchWidgetGroups.size());
   if (fss == NULL) {
      for (int i = 0; i < numSearchesInDialog; i++) {
         searchWidgetGroups[i]->setHidden(true);
      }
      searchNameLabel->setHidden(true);
      searchNameLineEdit->setHidden(true);
      return;
   }
   
   //
   // Set name
   //
   searchNameLineEdit->setText(fss->getName());
   searchNameLineEdit->setHidden(false);
   searchNameLabel->setHidden(false);
   
   //
   // Get type and names for search controls
   //
   std::vector<FociSearch::LOGIC>   logicTypes;
   std::vector<QString> logicNames;
   FociSearch::getLogicTypesAndNames(logicTypes, logicNames);
   const int numLogicTypes = static_cast<int>(logicTypes.size());
   std::vector<FociSearch::ATTRIBUTE>   attributeTypes;
   std::vector<QString> attributeNames;
   FociSearch::getAttributeTypesAndNames(attributeTypes, attributeNames);
   const int numAttributeTypes = static_cast<int>(attributeTypes.size());
   std::vector<FociSearch::MATCHING>   matchingTypes;
   std::vector<QString> matchingNames;
   FociSearch::getMatchingTypesAndNames(matchingTypes, matchingNames);
   const int numMatchingTypes = static_cast<int>(matchingTypes.size());
   
   //
   // Add rows to GUI
   //
   const int numValidSearches = fss->getNumberOfFociSearches();
   for (int i = numSearchesInDialog; i < numValidSearches; i++) {
      //
      // Search number label
      //
      QLabel* searchNumberLabel = new QLabel(QString::number(i + 1));
      
      //
      // Create insert search button
      //
      QToolButton* insertSearchButton = new QToolButton;
      insertSearchButton->setText("+");
      insertSearchButton->setToolTip("Add an additional\n"
                                     "search parameter.");
      QObject::connect(insertSearchButton, SIGNAL(clicked()),
                       insertSearchToolButtonSignalMapper, SLOT(map()));
      insertSearchToolButtonSignalMapper->setMapping(insertSearchButton, i);
      insertSearchToolButtons.push_back(insertSearchButton);
      
      //
      // Create delete search button
      //
      QToolButton* deleteSearchButton = new QToolButton;
      deleteSearchButton->setText("-");
      deleteSearchButton->setToolTip("Delete this \n"
                                     "search parameter.");
      QObject::connect(deleteSearchButton, SIGNAL(clicked()),
                       deleteSearchToolButtonSignalMapper, SLOT(map()));
      deleteSearchToolButtonSignalMapper->setMapping(deleteSearchButton, i);
      deleteSearchToolButtons.push_back(deleteSearchButton);
      
      //
      // Create the logic selection combo box
      //
      QComboBox* logicComboBox = new QComboBox;
      for (int j = 0; j < numLogicTypes; j++) {
         logicComboBox->addItem(logicNames[j], static_cast<int>(logicTypes[j]));
      }
      logicComboBox->setFixedSize(logicComboBox->sizeHint());
      logicComboBoxes.push_back(logicComboBox);
      QObject::connect(logicComboBox, SIGNAL(activated(int)),
                       searchLogicComboBoxSignalMapper, SLOT(map()));
      searchLogicComboBoxSignalMapper->setMapping(logicComboBox, i);
      logicComboBox->setToolTip("The search parameters are applied in their\n"
                                "listed order.  Union functions as a logical\n"
                                "OR and broadens the search results to include\n"
                                "more foci.  Intersection functions as a logical\n"
                                "AND and narrows the search results.");
      
      //
      // Create the attribute selection combo box
      //
      QComboBox* attributeComboBox = new QComboBox;
      for (int j = 0; j < numAttributeTypes; j++) {
         attributeComboBox->addItem(attributeNames[j], static_cast<int>(attributeTypes[j]));
      }
      attributeComboBox->setFixedSize(attributeComboBox->sizeHint());
      attributeComboBoxes.push_back(attributeComboBox);
      QObject::connect(attributeComboBox, SIGNAL(activated(int)),
                       searchAttributeComboBoxSignalMapper, SLOT(map()));
      searchAttributeComboBoxSignalMapper->setMapping(attributeComboBox, i);
      attributeComboBox->setToolTip("Chooses the focus or study \n"
                                    "attribute on which to search.");
                                    
      //
      // Create the matching selection combo box
      //
      QComboBox* matchingComboBox = new QComboBox;
      for (int j = 0; j < numMatchingTypes; j++) {
         matchingComboBox->addItem(matchingNames[j], static_cast<int>(matchingTypes[j]));
      }
      matchingComboBox->setFixedSize(matchingComboBox->sizeHint());
      matchingComboBoxes.push_back(matchingComboBox);
      QObject::connect(matchingComboBox, SIGNAL(activated(int)),
                       searchMatchingComboBoxSignalMapper, SLOT(map()));
      searchMatchingComboBoxSignalMapper->setMapping(matchingComboBox, i);
      matchingComboBox->setToolTip("Chooses how the search text is matched. \n"
                                   "This choice is ignored during a \n"
                                   "spatial search.");
                                   
      //
      // Create the assistance tool button
      //
      QToolButton* assistanceButton = new QToolButton;
      assistanceButton->setText("Terms...");
      assistanceButton->setToolTip("Provides assistance for \n"
                                   "for setting the search text.");
      QObject::connect(assistanceButton, SIGNAL(clicked()),
                       assistanceToolButtonSignalMapper, SLOT(map()));
      assistanceToolButtonSignalMapper->setMapping(assistanceButton, i);
      assistanceToolButtons.push_back(assistanceButton);
      
      //
      // Create the search text line edit
      //
      QLineEdit* searchLineEdit = new QLineEdit;
      searchTextLineEdits.push_back(searchLineEdit);
      QObject::connect(searchLineEdit, SIGNAL(textEdited(const QString&)),
                       searchTextLineEditSignalMapper, SLOT(map()));
      searchTextLineEditSignalMapper->setMapping(searchLineEdit, i);
      searchLineEdit->setToolTip("Enter the text for searching here. \n"
                                 "Separate multiple items with a semicolon.\n"
                                 "\n"
                                 "For a spatial search, this field \n"
                                 "should contain four consecutive \n"
                                 "numbers separated by spaces representing\n"
                                 "the X, Y, Z coordinates and radius of \n"
                                 "the spatial spherical region.");
                                 
      //
      // Add to layout
      // If new widgets are added, may need to adjust column stretching
      // that is set when the layout is created in "createSearchParametersGroup()"
      //
      const int row = searchGridLayout->rowCount();
      searchGridLayout->addWidget(searchNumberLabel,  row, 0);
      searchGridLayout->addWidget(insertSearchButton, row, 1);
      searchGridLayout->addWidget(deleteSearchButton, row, 2);
      searchGridLayout->addWidget(logicComboBox,      row, 3);
      searchGridLayout->addWidget(attributeComboBox,  row, 4);
      searchGridLayout->addWidget(matchingComboBox,   row, 5);
      searchGridLayout->addWidget(assistanceButton,   row + 1, 1, 1, 2, Qt::AlignRight);
      searchGridLayout->addWidget(searchLineEdit,     row + 1, 3, 1, 4);
      
      //
      // Add to widget group for row
      //
      WuQWidgetGroup* wg = new WuQWidgetGroup(this);
      wg->addWidget(searchNumberLabel);
      wg->addWidget(insertSearchButton);
      wg->addWidget(deleteSearchButton);
      wg->addWidget(logicComboBox);
      wg->addWidget(attributeComboBox);
      wg->addWidget(matchingComboBox);
      wg->addWidget(assistanceButton);
      wg->addWidget(searchLineEdit);
      searchWidgetGroups.push_back(wg);
   }
   
   //
   // Update existing GUI
   //
   for (int i = 0; i < numValidSearches; i++) {
      searchWidgetGroups[i]->blockSignals(true);
      const FociSearch* fs = fss->getFociSearch(i);
      logicComboBoxes[i]->setCurrentIndex(static_cast<int>(fs->getLogic()));
      attributeComboBoxes[i]->setCurrentIndex(static_cast<int>(fs->getAttribute()));
      matchingComboBoxes[i]->setCurrentIndex(static_cast<int>(fs->getMatching()));
      searchTextLineEdits[i]->setText(fs->getSearchText());
      searchWidgetGroups[i]->setHidden(false);
      searchWidgetGroups[i]->blockSignals(false);
   }
   
   //
   // Hide rows not needed
   //
   for (int i = numValidSearches; i < numSearchesInDialog; i++) {
      searchWidgetGroups[i]->setHidden(true);
   }
}
      
/**
 * called when an assistance tool button is clicked.
 */ 
void 
GuiFociSearchWidget::slotAssistanceToolButtonClicked(int item)
{
   FociSearchSet* fss = getSelectedSearchSet();
   FociSearch* fs = fss->getFociSearch(item);
   const FociSearch::ATTRIBUTE attribute = fs->getAttribute();

   unsigned int itemsToDisplay = GuiNameSelectionDialog::LIST_ALL;
   GuiNameSelectionDialog::LIST_ITEMS_TYPE defaultItem = 
      GuiNameSelectionDialog::LIST_NONE;
     
   switch (attribute) {
      case FociSearch::ATTRIBUTE_ALL:
         break;
      case FociSearch::ATTRIBUTE_FOCUS_AREA:
         defaultItem = GuiNameSelectionDialog::LIST_FOCI_AREAS_ALPHA;
         break;
      case FociSearch::ATTRIBUTE_FOCUS_CLASS:
         defaultItem = GuiNameSelectionDialog::LIST_FOCI_CLASSES_ALPHA;
         break;
      case FociSearch::ATTRIBUTE_FOCUS_COMMENT:
         defaultItem = GuiNameSelectionDialog::LIST_FOCI_COMMENT_ALPHA;
         break;
      case FociSearch::ATTRIBUTE_FOCUS_GEOGRAPHY:
         defaultItem = GuiNameSelectionDialog::LIST_FOCI_GEOGRAPHY_ALPHA;
         break;
      case FociSearch::ATTRIBUTE_FOCUS_ROI:
         defaultItem = GuiNameSelectionDialog::LIST_FOCI_ROI_ALPHA;
         break;
      case FociSearch::ATTRIBUTE_FOCUS_SPATIAL:
         {
            WuQDataEntryDialog ded(this);
            QDoubleSpinBox* xDSB = ded.addDoubleSpinBox("X", 0.0);
            QDoubleSpinBox* yDSB = ded.addDoubleSpinBox("Y", 0.0);
            QDoubleSpinBox* zDSB = ded.addDoubleSpinBox("Z", 0.0);
            QDoubleSpinBox* distanceDSB = ded.addDoubleSpinBox("Distance", 10.0);
            if (ded.exec() == WuQDataEntryDialog::Accepted) {
               const QString s = 
                  QString::number(xDSB->value(), 'f', 3) + " "
                  + QString::number(yDSB->value(), 'f', 3) + " "
                  + QString::number(zDSB->value(), 'f', 3) + " "
                  + QString::number(distanceDSB->value(), 'f', 3);
                  
                  searchTextLineEdits[item]->blockSignals(true);
                  searchTextLineEdits[item]->setText(s);
                  searchTextLineEdits[item]->blockSignals(false);
                  slotSearchTextChanged(item);
            }
            
            return;
         }
         break;
      case FociSearch::ATTRIBUTE_FOCUS_STRUCTURE:
         defaultItem = GuiNameSelectionDialog::LIST_STRUCTURE;
         break;
      case FociSearch::ATTRIBUTE_STUDY_AUTHORS:
         break;
      case FociSearch::ATTRIBUTE_STUDY_CITATION:
         defaultItem = GuiNameSelectionDialog::LIST_STUDY_CITATION;
         break;
      case FociSearch::ATTRIBUTE_STUDY_COMMENT:
         break;
      case FociSearch::ATTRIBUTE_STUDY_DATA_FORMAT:
         defaultItem = GuiNameSelectionDialog::LIST_STUDY_DATA_FORMAT;
         break;
      case FociSearch::ATTRIBUTE_STUDY_DATA_TYPE:
         defaultItem = GuiNameSelectionDialog::LIST_STUDY_DATA_TYPE;
         break;
      case FociSearch::ATTRIBUTE_STUDY_KEYWORDS:
         defaultItem = GuiNameSelectionDialog::LIST_STUDY_KEYWORDS_ALPHA;
         break;
      case FociSearch::ATTRIBUTE_STUDY_MESH_TERMS:
         defaultItem = GuiNameSelectionDialog::LIST_STUDY_MESH_ALPHA;
         break;
      case FociSearch::ATTRIBUTE_STUDY_NAME:
         break;
      case FociSearch::ATTRIBUTE_STUDY_STEREOTAXIC_SPACE:
         defaultItem = GuiNameSelectionDialog::LIST_STEREOTAXIC_SPACES;
         break;
      case FociSearch::ATTRIBUTE_STUDY_TABLE_HEADER:
         defaultItem = GuiNameSelectionDialog::LIST_STUDY_TABLE_HEADERS;
         break;
      case FociSearch::ATTRIBUTE_STUDY_TABLE_SUBHEADER:
         defaultItem = GuiNameSelectionDialog::LIST_STUDY_TABLE_SUBHEADERS;
         break;
      case FociSearch::ATTRIBUTE_STUDY_TITLE:
         break;
   }
   
   GuiNameSelectionDialog nsd(this,
                              itemsToDisplay,
                              defaultItem,
                              true);
   if (nsd.exec() == GuiNameSelectionDialog::Accepted) {
      QString txt = searchTextLineEdits[item]->text();
      const QStringList nameList = nsd.getNamesSelected();
      if (nameList.size() > 0) {
         if (txt.isEmpty() == false) {
            txt += "; ";
         }
         txt += nameList.join("; ");
      }
      searchTextLineEdits[item]->blockSignals(true);
      searchTextLineEdits[item]->setText(txt);
      searchTextLineEdits[item]->blockSignals(false);
      slotSearchTextChanged(item);
   }
}
      
/**
 * called when insert search tool button pressed.
 */
void 
GuiFociSearchWidget::slotInsertSearchToolButtonClicked(int buttNum)
{
   QMessageBox msgBox(insertSearchToolButtons[buttNum]);
   msgBox.setWindowTitle("New Search Parameter");
   msgBox.setText("Add new search parameter before or after search "
                  + QString::number(buttNum + 1) + "?");
   QPushButton* afterPushButton = msgBox.addButton("After", 
                                                   QMessageBox::NoRole);
   QPushButton* beforePushButton = msgBox.addButton("Before", 
                                                    QMessageBox::NoRole);
   
   QPushButton* cancelPushButton = msgBox.addButton("Cancel", 
                                                   QMessageBox::NoRole);
   msgBox.exec();
   if (msgBox.clickedButton() == afterPushButton) {
      FociSearchSet* fss = getSelectedSearchSet();
      FociSearch* fs = new FociSearch();
      fss->insertFociSearch(fs, buttNum);
   }
   else if (msgBox.clickedButton() == beforePushButton) {
      FociSearchSet* fss = getSelectedSearchSet();
      FociSearch* fs = new FociSearch();
      fss->insertFociSearch(fs, buttNum - 1);
   }
   else if (msgBox.clickedButton() == cancelPushButton) {
   }
   
   loadFociSearches();
}

/**
 * called when delete search tool button pressed.
 */
void 
GuiFociSearchWidget::slotDeleteSearchToolButtonClicked(int buttNum)
{
   FociSearchSet* fss = getSelectedSearchSet();
   if (fss->getNumberOfFociSearches() <= 1) {
      QMessageBox::critical(this,
                            "ERROR",
                            "You may not delete all search parameters.");
      return;
   }
   
   if (QMessageBox::question(this,
                             "CONFIRM",
                             "Remove search parameter " + QString::number(buttNum + 1),
                             (QMessageBox::Ok | QMessageBox::Cancel)) ==
                     QMessageBox::Ok) {                                          
      fss->deleteFociSearch(buttNum);
   }
   
   loadFociSearches();
}
      
/**
 * called when a search logic is changed.
 */
void 
GuiFociSearchWidget::slotSearchLogicChanged(int indx)
{
   FociSearchSet* fss = getSelectedSearchSet();
   FociSearch* fs = fss->getFociSearch(indx);
   const int logicIndex = logicComboBoxes[indx]->currentIndex();
   fs->setLogic(static_cast<FociSearch::LOGIC>(
                        logicComboBoxes[indx]->itemData(logicIndex).toInt()));
}

/**
 * called when a search attribute is changed.
 */
void 
GuiFociSearchWidget::slotSearchAttributeChanged(int indx)
{
   FociSearchSet* fss = getSelectedSearchSet();
   FociSearch* fs = fss->getFociSearch(indx);
   const int attributeIndex = attributeComboBoxes[indx]->currentIndex();
   fs->setAttribute(static_cast<FociSearch::ATTRIBUTE>(
                        attributeComboBoxes[indx]->itemData(attributeIndex).toInt()));
}

/**
 * called when a search matching is changed.
 */
void 
GuiFociSearchWidget::slotSearchMatchingChanged(int indx)
{
   FociSearchSet* fss = getSelectedSearchSet();
   FociSearch* fs = fss->getFociSearch(indx);
   const int matchingIndex = matchingComboBoxes[indx]->currentIndex();
   fs->setMatching(static_cast<FociSearch::MATCHING>(
                        matchingComboBoxes[indx]->itemData(matchingIndex).toInt()));
}

/**
 * called when a search text is changed.
 */
void 
GuiFociSearchWidget::slotSearchTextChanged(int indx)
{
   FociSearchSet* fss = getSelectedSearchSet();
   FociSearch* fs = fss->getFociSearch(indx);
   fs->setSearchText(searchTextLineEdits[indx]->text().trimmed());
}
      
/**
 * called when search name is changed.
 */
void 
GuiFociSearchWidget::slotSearchNameLineEdit(const QString& s)
{
   FociSearchSet* fss = getSelectedSearchSet();
   fss->setName(s.trimmed());
}

/**
 * called when reset button pressed.
 */
void 
GuiFociSearchWidget::slotResetPushButton()
{
   searchResultsLabel->setText("");
   fociProjectionFile->setAllSearchStatus(true);
   DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   dsf->determineDisplayedFoci();   
   GuiBrainModelOpenGL::updateAllGL();         
}
      
/**
 * called when search displayed button pressed.
 */
void 
GuiFociSearchWidget::slotSearchDisplayedPushButton()
{
   performSearch(true);
}
      
/**
 * called when search all button pressed.
 */
void 
GuiFociSearchWidget::slotSearchAllPushButton()
{
   searchResultsLabel->setText("");
   fociProjectionFile->setAllSearchStatus(true);
   performSearch(false);
}

/**
 * perform the search.
 */
void 
GuiFociSearchWidget::performSearch(const bool searchDisplayedOnlyFlag)
{      
   BrainModelSurfaceFociSearch::SEARCH_MODE searchMode = BrainModelSurfaceFociSearch::SEARCH_MODE_ALL_FOCI;
   if (searchDisplayedOnlyFlag) {
      searchMode = BrainModelSurfaceFociSearch::SEARCH_MODE_DISPLAYED_FOCI;
   }
   
   searchResultsLabel->setText("");
   
   FociSearchSet* fss = getSelectedSearchSet();
   if (fss != NULL) {
      BrainModelSurfaceFociSearch fociSearch(theMainWindow->getBrainSet(),
                                             theMainWindow->getBrainSet()->getStudyMetaDataFile(),
                                             fociProjectionFile,
                                             fss,
                                             searchMode,
                                             showFociFromMatchingStudiesCheckBox->isChecked());
      int numberOfMatchingFoci = 0;
      int numberOfFociFromMatchingStudies = 0;
      try {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         fociSearch.execute();
         numberOfMatchingFoci = fociSearch.getNumberOfMatchingFoci();
         numberOfFociFromMatchingStudies = fociSearch.getNumberOfFociFromMatchingStudies();
         QApplication::restoreOverrideCursor();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
      }
      QString matchingStudiesString;
      if (numberOfFociFromMatchingStudies > 0) {
         matchingStudiesString = "/" + QString::number(numberOfFociFromMatchingStudies);
      }
      searchResultsLabel->setText(QString::number(numberOfMatchingFoci)
                                  + matchingStudiesString
                                  + " of "
                                  + QString::number(fociProjectionFile->getNumberOfCellProjections())
                                  + " foci matched.");
      DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
      dsf->determineDisplayedFoci();   
      GuiBrainModelOpenGL::updateAllGL();         
   }
}
      
/**
 * get the selected search set.
 */
FociSearchSet* 
GuiFociSearchWidget::getSelectedSearchSet()
{
   //
   // Get the number of the selected search
   // Note that "searchSelectionSpinBox" indexes 1..N
   //
   int searchNumber = searchSelectionSpinBox->value() - 1;

   //
   // Get number of search sets
   //
   const int numSearchSets = fociSearchFile->getNumberOfFociSearchSets();
   
   //
   // Update search set number
   //
   if ((searchNumber < 0) ||
       (searchNumber >= numSearchSets)) {
      searchNumber = 0;
   }

   //
   // Update search selection spin box
   //
   if (numSearchSets <= 0) {
      searchSelectionSpinBox->setEnabled(false);
   }
   else {
      //
      // Note that "searchSelectionSpinBox" indexes 1..N
      //
      searchSelectionSpinBox->blockSignals(true);
      searchSelectionSpinBox->setEnabled(true);
      searchSelectionSpinBox->setMinimum(1);
      searchSelectionSpinBox->setMaximum(numSearchSets);
      searchSelectionSpinBox->setValue(searchNumber + 1);
      searchSelectionSpinBox->blockSignals(false);
   }
   
   //
   // Get active search
   //
   FociSearchSet* fss = fociSearchFile->getFociSearchSet(searchNumber);
   
   return fss;
}      



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

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "GuiMessageBox.h"
#include "GuiStudyInfoEditorWidget.h"
#include "StereotaxicSpace.h"
#include "QtListBoxSelectionDialog.h"
#include "QtWidgetGroup.h"

static const QString newStudyRetainEntriesString("New - Retain Entries");
static const QString newStudyClearEntriesString("New - Clear Entries");

/**
 * constructor.
 */
GuiStudyInfoEditorWidget::GuiStudyInfoEditorWidget(std::vector<CellStudyInfo>* studyInfoIn,
                                                   QWidget* parent)
   : QWidget(parent)
{
   //
   // Point to the study info
   //
   studyInfo = studyInfoIn;
   
   //
   // study selection label and combo box
   //
   QLabel* studySelectionLabel = new QLabel("Study");
   studySelectionComboBox = new QComboBox;
   QObject::connect(studySelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotStudySelectionComboBox(int)));
   
   // label and line edit
   QLabel* titleLabel = new QLabel("Title");
   titleLineEdit = new QLineEdit;
    
   // label and line edit
   QLabel* authorsLabel = new QLabel("Authors");
   authorsLineEdit = new QLineEdit;
    
   // label and line edit
   QLabel* citationLabel = new QLabel("Citation");
   citationLineEdit = new QLineEdit;
    
   // label and line edit
   QLabel* urlLabel = new QLabel("URL");
   urlLineEdit = new QLineEdit;
    
   // label and line edit
   QLabel* keywordsLabel = new QLabel("Keywords");
   keywordsLineEdit = new QLineEdit;
    
   // label and line edit
   QPushButton* stereotaxicSpacePushButton = new QPushButton("Stereotaxic\nSpace...");
   stereotaxicSpacePushButton->setFixedSize(stereotaxicSpacePushButton->sizeHint());
   stereotaxicSpacePushButton->setAutoDefault(false);
   QObject::connect(stereotaxicSpacePushButton, SIGNAL(clicked()),
                    this, SLOT(slotStereotaxicSpacePushButton()));
   stereotaxicSpaceLineEdit = new QLineEdit;
    
   //
   // Widget group for stereotaxic space items
   //
   stereotaxicSpaceWidgetGroup = new QtWidgetGroup(this);
   stereotaxicSpaceWidgetGroup->addWidget(stereotaxicSpacePushButton);
   stereotaxicSpaceWidgetGroup->addWidget(stereotaxicSpaceLineEdit);
   
   // label and line edit
   QLabel* partitioningSchemeAbbreviationLabel = new QLabel("Partitioning Scheme\nAbbreviation");
   partitioningSchemeAbbreviationLineEdit = new QLineEdit;
    
   // label and line edit
   QLabel* partitioningSchemeFullNameLabel = new QLabel("Partitioning Scheme\nFull Name");
   partitioningSchemeFullNameLineEdit = new QLineEdit;
   
   //
   // Widget group for stereotaxic space items
   //
   partitioningSchemeWidgetGroup = new QtWidgetGroup(this);
   partitioningSchemeWidgetGroup->addWidget(partitioningSchemeAbbreviationLabel);
   partitioningSchemeWidgetGroup->addWidget(partitioningSchemeAbbreviationLineEdit);
   partitioningSchemeWidgetGroup->addWidget(partitioningSchemeFullNameLabel);
   partitioningSchemeWidgetGroup->addWidget(partitioningSchemeFullNameLineEdit);

   // comment text edit
   QLabel* commentLabel = new QLabel("Comment");
   commentTextEdit = new QTextEdit;
   
   //
   // layout the widget items
   //
   QGridLayout* grid = new QGridLayout(this);
   grid->addWidget(studySelectionLabel, 0, 0);
   grid->addWidget(studySelectionComboBox, 0, 1);
   grid->addWidget(titleLabel, 1, 0);
   grid->addWidget(titleLineEdit, 1, 1);
   grid->addWidget(authorsLabel, 2, 0);
   grid->addWidget(authorsLineEdit, 2, 1);
   grid->addWidget(citationLabel, 3, 0);
   grid->addWidget(citationLineEdit, 3, 1);
   grid->addWidget(urlLabel, 4, 0);
   grid->addWidget(urlLineEdit, 4, 1);
   grid->addWidget(keywordsLabel, 5, 0);
   grid->addWidget(keywordsLineEdit, 5, 1);
   grid->addWidget(stereotaxicSpacePushButton, 6, 0);
   grid->addWidget(stereotaxicSpaceLineEdit, 6, 1);
   grid->addWidget(partitioningSchemeAbbreviationLabel, 7, 0);
   grid->addWidget(partitioningSchemeAbbreviationLineEdit, 7, 1);
   grid->addWidget(partitioningSchemeFullNameLabel, 8, 0);
   grid->addWidget(partitioningSchemeFullNameLineEdit, 8, 1);
   grid->addWidget(commentLabel, 9, 0);
   grid->addWidget(commentTextEdit, 9, 1);
}

/**
 * destructor.
 */
GuiStudyInfoEditorWidget::~GuiStudyInfoEditorWidget()
{
}

/**
 * load the selection combo box.
 */
void 
GuiStudyInfoEditorWidget::loadStudySelectionComboBox()
{
   bool newRetainFlag = false;
   bool newClearFlag = false;
   
   //
   // Get the currently selected study number
   //
   int currentStudyNumber = studySelectionComboBox->currentIndex();
   if (studySelectionComboBox->currentText() == newStudyRetainEntriesString) {
      newRetainFlag = true;
   }
   if (studySelectionComboBox->currentText() == newStudyClearEntriesString) {
      newClearFlag = true;
   }
   
   //
   // load the study number combo box
   //
   studySelectionComboBox->clear();
   const int numStudies = static_cast<int>(studyInfo->size());
   for (int i = 0; i < numStudies; i++) {
      const CellStudyInfo& csi = (*studyInfo)[i];
      QString s(csi.getTitle());
      if (s.length() > 30) {
         s.resize(30);
      }
      QString qs(QString::number(i));
      qs.append(" - ");
      qs.append(s);
      studySelectionComboBox->addItem(qs);
   }
   
   //
   // Load new study info selection options
   //
   const int indexNewStudyClearEntries = studySelectionComboBox->count();
   studySelectionComboBox->addItem(newStudyClearEntriesString);
   const int indexNewStudyRetainEntries = studySelectionComboBox->count();
   studySelectionComboBox->addItem(newStudyRetainEntriesString);
   
   //
   // Set selected item
   //
   if (newRetainFlag) {
      studySelectionComboBox->setCurrentItem(indexNewStudyRetainEntries);
   }
   else if (newClearFlag) {
      studySelectionComboBox->setCurrentItem(indexNewStudyClearEntries);
   }
   else if ((currentStudyNumber >= 0) &&
            (currentStudyNumber < numStudies)) {
      studySelectionComboBox->setCurrentItem(currentStudyNumber);
   }
   else {
      studySelectionComboBox->setCurrentItem(indexNewStudyRetainEntries);
   }
   
   slotStudySelectionComboBox(studySelectionComboBox->currentItem());
}

/**
 * update the widget.
 */
void 
GuiStudyInfoEditorWidget::updateWidget(std::vector<CellStudyInfo>* studyInfoIn)
{
   studyInfo = studyInfoIn;
   loadStudySelectionComboBox();
}

/**
 * called when study selection combo box selected.
 */
void 
GuiStudyInfoEditorWidget::slotStudySelectionComboBox(int item)
{
   const int numStudies = static_cast<int>(studyInfo->size());
   
   if (studySelectionComboBox->currentText() == newStudyRetainEntriesString) {
      // nothing
   }
   else if (studySelectionComboBox->currentText() == newStudyClearEntriesString) {
      titleLineEdit->setText("");
      authorsLineEdit->setText("");
      citationLineEdit->setText("");
      urlLineEdit->setText("");
      keywordsLineEdit->setText("");
      stereotaxicSpaceLineEdit->setText("");
      partitioningSchemeAbbreviationLineEdit->setText("");
      partitioningSchemeFullNameLineEdit->setText("");
      commentTextEdit->setText("");
   } 
   else if ((item >= 0) && (item < numStudies)) {
      const CellStudyInfo& csi = (*studyInfo)[item];
      titleLineEdit->setText(csi.getTitle());
      authorsLineEdit->setText(csi.getAuthors());
      citationLineEdit->setText(csi.getCitation());
      urlLineEdit->setText(csi.getURL());
      keywordsLineEdit->setText(csi.getKeywords());
      stereotaxicSpaceLineEdit->setText(csi.getStereotaxicSpace());
      partitioningSchemeAbbreviationLineEdit->setText(csi.getPartitioningSchemeAbbreviation());
      partitioningSchemeFullNameLineEdit->setText(csi.getPartitioningSchemeFullName());
      commentTextEdit->setText(csi.getComment());
   }
}

/**
 * accept changes in editor.
 */
void 
GuiStudyInfoEditorWidget::slotAcceptEditorContents()
{
   if (titleLineEdit->text().isEmpty()) {
      GuiMessageBox::critical(this, "ERROR", "You must enter a title.", "OK");
      return;
   }
   
   int indx = studySelectionComboBox->currentIndex();
   if ((indx >= 0) && (indx < static_cast<int>(studyInfo->size()))) {
      // nothing to do -- editing existing entry 
   }
   else {
      CellStudyInfo temp;
      studyInfo->push_back(temp);
      indx = studyInfo->size() - 1;
   }

   CellStudyInfo& csi = (*studyInfo)[indx];
   csi.setTitle(titleLineEdit->text());
   csi.setAuthors(authorsLineEdit->text());
   csi.setCitation(citationLineEdit->text());
   csi.setURL(urlLineEdit->text());
   csi.setKeywords(keywordsLineEdit->text());
   csi.setStereotaxicSpace(stereotaxicSpaceLineEdit->text());
   csi.setPartitioningSchemeAbbreviation(partitioningSchemeAbbreviationLineEdit->text());
   csi.setPartitioningSchemeFullName(partitioningSchemeFullNameLineEdit->text());
   csi.setComment(commentTextEdit->text());
   
   if (studySelectionComboBox->currentText() == newStudyClearEntriesString) {
      titleLineEdit->setText("");
      authorsLineEdit->setText("");
      citationLineEdit->setText("");
      urlLineEdit->setText("");
      keywordsLineEdit->setText("");
      stereotaxicSpaceLineEdit->setText("");
      partitioningSchemeAbbreviationLineEdit->setText("");
      partitioningSchemeFullNameLineEdit->setText("");
      commentTextEdit->setText("");
   }
}
      
/**
 * called when stereotaxic space push button pressed.
 */
void 
GuiStudyInfoEditorWidget::slotStereotaxicSpacePushButton()
{
   std::vector<StereotaxicSpace> spaces;
   StereotaxicSpace::getAllStereotaxicSpaces(spaces);
   std::vector<QString> spaceNames;
   
   for (unsigned int i = 0; i < spaces.size(); i++) {
      spaceNames.push_back(spaces[i].getName());
   }
   
   QtListBoxSelectionDialog lbsd(this,
                                 "Stereotaxic Spaces",
                                 "",
                                 spaceNames,
                                 -1);

   if (lbsd.exec() == QDialog::Accepted) {
      const int item = lbsd.getSelectedItemIndex();
      if (item >= 0) {
         stereotaxicSpaceLineEdit->setText(spaceNames[item]);
      }
   }
}

/**
 * hide stereotaxic space controls.
 */
void 
GuiStudyInfoEditorWidget::hideStereotaxicSpaceControls(const bool hideThem)
{
   stereotaxicSpaceWidgetGroup->setHidden(hideThem);
}

/**
 * hide partitioning scheme controls.
 */
void 
GuiStudyInfoEditorWidget::hidePartitioningSchemeControls(const bool hideThem)
{
   partitioningSchemeWidgetGroup->setHidden(hideThem);
}            

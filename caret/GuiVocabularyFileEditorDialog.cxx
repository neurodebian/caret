
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

#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>

#include "BrainSet.h"
#include "GuiMainWindow.h"
#include "GuiNameSelectionDialog.h"
#include "GuiStudyInfoEditorWidget.h"
#include "GuiStudyMetaDataLinkCreationDialog.h"
#include "GuiVocabularyFileEditorDialog.h"
#include "WuQWidgetGroup.h"
#include "VocabularyFile.h"
#include "global_variables.h"

static const QString noneStudyName("None");

/**
 * constructor.
 */
GuiVocabularyFileEditorDialog::GuiVocabularyFileEditorDialog(QWidget* parent)
   : QtDialogNonModal(parent)
{
   setWindowTitle("Vocabulary Editor");
   
   //
   // Create the vocabulary entry widget
   //
   vocabularyWidget = createVocabularyWidget();
   
   //
   // Create the study info editor widget
   //
   VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();
   studyInfoEditorWidget = new GuiStudyInfoEditorWidget(vf->getPointerToStudyInfo());
   studyInfoEditorWidget->hideStereotaxicSpaceControls(true);
   
   //
   // Create the tab widget
   //
   tabWidget = new QTabWidget;
   tabWidget->addTab(vocabularyWidget, "Vocabulary");
   tabWidget->addTab(studyInfoEditorWidget, "Studies");
   
   //
   // get the layout for the dialog
   //
   QVBoxLayout* layout = getDialogLayout();
   layout->addWidget(tabWidget);
   
   //
   // connect signals for Apply and Close buttons
   //
   QObject::connect(getApplyPushButton(), SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
   QObject::connect(getClosePushButton(), SIGNAL(clicked()),
                    this, SLOT(close()));
   
   //
   // Initialize
   //
   entryAddRadioButton->setChecked(true);
   updateDialog();
}

/**
 * destructor.
 */
GuiVocabularyFileEditorDialog::~GuiVocabularyFileEditorDialog()
{
}

/**
 * create the vocabulary widget.
 */
QWidget* 
GuiVocabularyFileEditorDialog::createVocabularyWidget()
{
   //
   // Add and edit radio buttons radio button
   //
   entryAddRadioButton = new QRadioButton("Add");
   entryEditRadioButton = new QRadioButton("Edit");
    
   //
   // Button group to keep add/edit buttons mutually exclusive
   //
   QButtonGroup* entryButtonGroup = new QButtonGroup(this);
   QObject::connect(entryButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEntryModeChanged()));
   entryButtonGroup->addButton(entryAddRadioButton, 0);
   entryButtonGroup->addButton(entryEditRadioButton, 1);
   
   //
   // Edit spin box
   //
   entryEditSpinBox = new QSpinBox;
   entryEditSpinBox->setSingleStep(1);
   QObject::connect(entryEditSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotLoadVocabularyEntry(int)));
   
   //
   // delete push button
   //
   QPushButton* deleteEntryPushButton = new QPushButton("Delete");
   deleteEntryPushButton->setFixedSize(deleteEntryPushButton->sizeHint());
   deleteEntryPushButton->setAutoDefault(false);
   QObject::connect(deleteEntryPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteEntryPushButton()));
                    
   //
   // Widget group for edit spin box and delete button
   //
   entryEditWidgetGroup = new WuQWidgetGroup(this);
   entryEditWidgetGroup->addWidget(entryEditSpinBox);
   entryEditWidgetGroup->addWidget(deleteEntryPushButton);
   
   //
   // Group box and layout for entry mode
   //
   QGroupBox* entryModeGroupBox = new QGroupBox("Entry Mode");
   QGridLayout* entryModeGridLayout = new QGridLayout(entryModeGroupBox);
   entryModeGridLayout->addWidget(entryAddRadioButton, 0, 0);
   entryModeGridLayout->addWidget(entryEditRadioButton, 1, 0);
   entryModeGridLayout->addWidget(entryEditSpinBox, 1, 1);
   entryModeGridLayout->addWidget(deleteEntryPushButton, 1, 2);
   entryModeGridLayout->setColumnStretch(0, 0);
   entryModeGridLayout->setColumnStretch(1, 0);
   entryModeGridLayout->setColumnStretch(2, 0);
   entryModeGridLayout->setColumnStretch(3, 100);
    
   //
   // pushbutton and line edit for abbreviation
   //
   QPushButton* abbreviationPushButton = new QPushButton("Abbreviation...");
   abbreviationPushButton->setAutoDefault(false);
   QObject::connect(abbreviationPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAbbreviationPushButton()));
   abbreviationLineEdit = new QLineEdit;
   
   //
   // Label and line edit for full name
   //
   QLabel* fullNameLabel = new QLabel("Full Name");
   fullNameLineEdit = new QLineEdit;
    
   //
   // Label and line edit for class name
   //
   QLabel* classNameLabel = new QLabel("Class Name");
   classNameLineEdit = new QLineEdit;
    
   //
   // Label and line edit for vocabulary
   //
   QLabel* vocabularyIdLabel = new QLabel("Vocabulary ID");
   vocabularyIdLineEdit = new QLineEdit;
    
   //
   // Label and line edit for study number
   //
   QLabel* studyNumberLabel = new QLabel("Study in Vocab File");
   studyNumberComboBox = new QComboBox;
    
   //
   // Label and line edit for study meta data
   //
   QPushButton* vocabularyStudyMetaDataPushButton = new QPushButton("Study Metadata...");
   vocabularyStudyMetaDataPushButton->setAutoDefault(false);
   QObject::connect(vocabularyStudyMetaDataPushButton, SIGNAL(clicked()),
                    this, SLOT(slotVocabularStudyMetaDataPushButton()));
   vocabularyStudyMetaDataLineEdit = new QLineEdit;
   
   //
   // Label and text edit for description
   //
   QLabel* descriptionLabel = new QLabel("Description");
   descriptionTextEdit = new QTextEdit;
               
   //
   // Group Box and Layout for vocabulary data
   //
   QGroupBox* dataGroupBox = new QGroupBox("Data");
   QGridLayout* dataGridLayout = new QGridLayout(dataGroupBox);
   dataGridLayout->addWidget(abbreviationPushButton, 0, 0);
   dataGridLayout->addWidget(abbreviationLineEdit, 0, 1);
   dataGridLayout->addWidget(fullNameLabel, 1, 0);
   dataGridLayout->addWidget(fullNameLineEdit, 1, 1);
   dataGridLayout->addWidget(classNameLabel, 2, 0);
   dataGridLayout->addWidget(classNameLineEdit, 2, 1);
   dataGridLayout->addWidget(vocabularyIdLabel, 3, 0);
   dataGridLayout->addWidget(vocabularyIdLineEdit, 3, 1);
   dataGridLayout->addWidget(studyNumberLabel, 4, 0);
   dataGridLayout->addWidget(studyNumberComboBox, 4, 1);
   dataGridLayout->addWidget(vocabularyStudyMetaDataPushButton, 5, 0);
   dataGridLayout->addWidget(vocabularyStudyMetaDataLineEdit, 5, 1);
   dataGridLayout->addWidget(descriptionLabel, 6, 0);
   dataGridLayout->addWidget(descriptionTextEdit, 6, 1);

   //
   // widget and layout
   //
   QWidget* w = new QWidget;
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addWidget(entryModeGroupBox);
   l->addWidget(dataGroupBox);
   return w;
}
      
/**
 * called when study meta data button pressed.
 */
void 
GuiVocabularyFileEditorDialog::slotVocabularStudyMetaDataPushButton()
{
   GuiStudyMetaDataLinkCreationDialog smdlcd(this);
   StudyMetaDataLinkSet smdls;
   smdls.setLinkSetFromCodedText(vocabularyStudyMetaDataLineEdit->text());
   smdlcd.initializeSelectedLinkSet(smdls);
   if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
      vocabularyStudyMetaDataLineEdit->setText(smdlcd.getLinkSetCreated().getLinkSetAsCodedText());
      vocabularyStudyMetaDataLineEdit->home(false);
   }
}      

/**
 * called when apply button pressed.
 */
void 
GuiVocabularyFileEditorDialog::slotApplyButton()
{
   if (tabWidget->currentWidget() == vocabularyWidget) {
      const QString& abbreviation = abbreviationLineEdit->text();
      
      if (abbreviation.isEmpty()) {
         QMessageBox::critical(this, "ERROR", "Abbreviation is empty.");
         return;
      }
      
      //
      // Get the vocabulary file
      //
      VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();      
      
      //
      // Add button checked ?
      //
      if (entryAddRadioButton->isChecked()) {
         //
         // Add a new vocabulary entry and update the edit spin box to new entry
         //
         VocabularyFile::VocabularyEntry temp(abbreviation);
         vf->addVocabularyEntry(temp);
         updateEditEntrySpinBoxMinMax();
         entryEditSpinBox->blockSignals(true);
         entryEditSpinBox->setValue(vf->getNumberOfVocabularyEntries() - 1);
         entryEditSpinBox->blockSignals(false);
      }
      //
      // Edit button checked
      //
      else if (entryEditRadioButton->isChecked()) {
         // do nothing
      }
      else {
         std::cout << "PROGRAM ERROR in GuiVocabularyFileEditorDialog::slotApplyButton()" << std::endl;
         return;
      }
      const int indx = entryEditSpinBox->value();
      if ((indx < 0) || (indx >= vf->getNumberOfVocabularyEntries())) {
         QMessageBox::critical(this, "ERROR", "Invalid editing number selection.");
         return;
      }
      
      StudyMetaDataLinkSet smdls;
      smdls.setLinkSetFromCodedText(vocabularyStudyMetaDataLineEdit->text());
      
      //
      // Get the currently selected item and set its parameters
      //
      VocabularyFile::VocabularyEntry* ve = vf->getVocabularyEntry(indx);
      ve->setAbbreviation(abbreviation);
      ve->setFullName(fullNameLineEdit->text());
      ve->setClassName(classNameLineEdit->text());
      ve->setVocabularyID(vocabularyIdLineEdit->text());
      ve->setStudyMetaDataLinkSet(smdls);
      int studyNum = studyNumberComboBox->currentIndex();
      if (studyNumberComboBox->currentText() == noneStudyName) {
         studyNum = -1;
      }
      else if (studyNum >= vf->getNumberOfStudyInfo()) {
         studyNum = -1;
      }
      ve->setStudyNumber(studyNum);
      ve->setDescription(descriptionTextEdit->toPlainText());
   }
   else if (tabWidget->currentWidget() == studyInfoEditorWidget) {
      studyInfoEditorWidget->slotAcceptEditorContents();
      slotUpdateStudyNumberComboBox();
   }
}

/**
 * called to load a vocabulary entry.
 */
void 
GuiVocabularyFileEditorDialog::slotLoadVocabularyEntry(int indx)
{
   //
   // Get the vocabulary file
   //
   VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();      

   //
   // Get the currently selected item and get its parameters
   //
   if ((indx >= 0) && (indx < vf->getNumberOfVocabularyEntries())) {
      VocabularyFile::VocabularyEntry* ve = vf->getVocabularyEntry(indx);
      abbreviationLineEdit->setText(ve->getAbbreviation());
      fullNameLineEdit->setText(ve->getFullName());
      classNameLineEdit->setText(ve->getClassName());
      vocabularyIdLineEdit->setText(ve->getVocabularyID());
      int studyNum = ve->getStudyNumber();
      if ((studyNum < 0) ||
          (studyNum >= vf->getNumberOfStudyInfo())) {
         studyNum = vf->getNumberOfStudyInfo();
      }
      studyNumberComboBox->setCurrentIndex(studyNum);
      descriptionTextEdit->setText(ve->getDescription());
      vocabularyStudyMetaDataLineEdit->setText(ve->getStudyMetaDataLinkSet().getLinkSetAsCodedText());
      vocabularyStudyMetaDataLineEdit->home(false);
   }   
}

/**
 * called when Add or Edit radio button selected.
 */
void 
GuiVocabularyFileEditorDialog::slotEntryModeChanged()
{
   VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();    
   
   //
   // If no entries, switch to add mode
   //
   if (vf->getNumberOfVocabularyEntries() == 0) {
      entryAddRadioButton->blockSignals(true);
      entryEditRadioButton->blockSignals(true);
      entryAddRadioButton->setChecked(true);
      entryEditRadioButton->setChecked(false);
      entryAddRadioButton->blockSignals(false);
      entryEditRadioButton->blockSignals(false);
   }

   //
   // Disable editing controls if "Add" mode
   //
   entryEditWidgetGroup->setDisabled(entryAddRadioButton->isChecked());
   
   //
   // If switched to editing, update edit number spin box
   //
   if (entryEditRadioButton->isChecked()) {
      int indx = entryEditSpinBox->value();
      if ((indx < 0) || (indx >= vf->getNumberOfVocabularyEntries())) {
         indx = 0;
      }
      slotLoadVocabularyEntry(indx);      
   }
}
      
/**
 * called when delete entry button pressed.
 */
void 
GuiVocabularyFileEditorDialog::slotDeleteEntryPushButton()
{
   if (QMessageBox::question(this, "Confirm",
                               "Are you sure you want to delete the current vocabulary entry?",
                               (QMessageBox::Yes | QMessageBox::No),
                               QMessageBox::Yes)
                                  == QMessageBox::Yes) {
      if (entryEditRadioButton->isChecked()) {
         int indx = entryEditSpinBox->value();
         VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();      
         if ((indx >= 0) && (indx < vf->getNumberOfVocabularyEntries())) {
            vf->deleteVocabularyEntry(indx);
         }
         updateEditEntrySpinBoxMinMax();
         slotEntryModeChanged();
      }
   }
}

/**
 * called when abbreviation button pressed.
 */
void 
GuiVocabularyFileEditorDialog::slotAbbreviationPushButton()
{
   GuiNameSelectionDialog nsd(this, GuiNameSelectionDialog::LIST_ALL);
   if (nsd.exec() == QDialog::Accepted) {
      QString name(nsd.getName());
      if (name.isEmpty() == false) {
         abbreviationLineEdit->setText(name);
      }
   }
}

/**
 * update edit spin box min/max.
 */
void 
GuiVocabularyFileEditorDialog::updateEditEntrySpinBoxMinMax()
{
   VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();      
   entryEditSpinBox->setMinimum(0);
   const int maxNum = std::max(0, vf->getNumberOfVocabularyEntries() - 1);
   entryEditSpinBox->setMaximum(maxNum);
}
      
/**
 * update the dialog.
 */
void 
GuiVocabularyFileEditorDialog::updateDialog()
{
   updateEditEntrySpinBoxMinMax();
   slotEntryModeChanged();
   VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();
   studyInfoEditorWidget->updateWidget(vf->getPointerToStudyInfo());
   slotUpdateStudyNumberComboBox();
}

/**
 * update the study number combo box.
 */
void 
GuiVocabularyFileEditorDialog::slotUpdateStudyNumberComboBox()
{
   const bool noneFlag = (studyNumberComboBox->currentText() == noneStudyName);
   const int oldIndx = studyNumberComboBox->currentIndex();
   
   studyNumberComboBox->clear();
   
   VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();
   const int numStudys = vf->getNumberOfStudyInfo();
   for (int i = 0; i < numStudys; i++) {
      const CellStudyInfo* csi = vf->getStudyInfo(i);
      QString s(csi->getTitle());
      if (s.length() > 30) {
         s.resize(30);
      }
      QString qs(QString::number(i));
      qs.append(" - ");
      qs.append(s);
      studyNumberComboBox->addItem(qs);
   }
   
   const int noneIndex = studyNumberComboBox->count();
   studyNumberComboBox->addItem(noneStudyName);
   
   if (noneFlag) {
      studyNumberComboBox->setCurrentIndex(noneIndex);
   }
   else if ((oldIndx >= 0) && (oldIndx < numStudys)) {
      studyNumberComboBox->setCurrentIndex(oldIndx);
   }
   else {
      studyNumberComboBox->setCurrentIndex(noneIndex);
   }
}      


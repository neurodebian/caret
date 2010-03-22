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
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSignalMapper>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolButton>

#include "BrainSet.h"
#include "FociProjectionFile.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiStudyCollectionFileEditorDialog.h"
#include "StudyCollectionFile.h"
#include "StudyMetaDataFile.h"
#include "WuQDataEntryDialog.h"
#include "WuQWidgetGroup.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiStudyCollectionFileEditorDialog::GuiStudyCollectionFileEditorDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Study Collection Editor");
   
   //
   // Collection Selection
   //
   QWidget* collectionSelectionWidget = createStudyCollectionSelectionSection();
   
   //
   // File Operations
   //
   QWidget* fileOperationsWidget = createFileOperationsSection();
   
   //
   // Collection Operations
   //
   collectionOperationsWidget = createCollectionOperationsSection();
   
   //
   // Collection Studies
   //
   collectionStudiesWidget = createStudyCollectionSection();
   
   // Close button
   //
   QDialogButtonBox* buttonBox = 
      new QDialogButtonBox(QDialogButtonBox::Close);
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(close()));
                    
   //
   // Left column layout
   //
   QVBoxLayout* leftColumnLayout = new QVBoxLayout;
   leftColumnLayout->addWidget(collectionSelectionWidget);
   leftColumnLayout->addWidget(fileOperationsWidget);
   leftColumnLayout->addWidget(collectionOperationsWidget);
   leftColumnLayout->addStretch();
   
   //
   // Horizontal layout
   //
   QHBoxLayout* horizontalLayout = new QHBoxLayout;
   horizontalLayout->addLayout(leftColumnLayout);
   horizontalLayout->addWidget(collectionStudiesWidget);
   horizontalLayout->setStretchFactor(leftColumnLayout, 0);
   horizontalLayout->setStretchFactor(collectionStudiesWidget, 100);

   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addLayout(horizontalLayout);
   dialogLayout->addWidget(buttonBox);
   
   updateDialog();
}

/**
 * destructor.
 */
GuiStudyCollectionFileEditorDialog::~GuiStudyCollectionFileEditorDialog()
{
}

/**
 * update the dialog.
 */
void 
GuiStudyCollectionFileEditorDialog::updateDialog()
{
   StudyCollectionFile* scf = theMainWindow->getBrainSet()->getStudyCollectionFile();
   const int numCollections = scf->getNumberOfStudyCollections();
   
   studySelectionSpinBox->blockSignals(true);
   studySelectionSpinBox->setMinimum(1);
   studySelectionSpinBox->setMaximum(numCollections);
   studySelectionSpinBox->blockSignals(false);
   
   loadSelectedCollectionIntoEditor();
}

/**
 * create the study collection selection section.
 */
QWidget* 
GuiStudyCollectionFileEditorDialog::createStudyCollectionSelectionSection()
{
   //
   // Spin Box for study selection
   //
   studySelectionSpinBox = new QSpinBox;
   studySelectionSpinBox->setMinimum(1);
   studySelectionSpinBox->setMaximum(1);
   studySelectionSpinBox->setSingleStep(1);
   QObject::connect(studySelectionSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotStudySelectionSpinBox(int)));
                    
   //
   // Group box and layout
   //
   QGroupBox* selectionGroupBox = new QGroupBox("Collection Selection");
   QVBoxLayout* selectionLayout = new QVBoxLayout(selectionGroupBox);
   selectionLayout->addWidget(studySelectionSpinBox);
   
   return selectionGroupBox;
}

/**
 * when study collection selection spin box value changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotStudySelectionSpinBox(int /*value*/)
{
   loadSelectedCollectionIntoEditor();
}

/**
 * create the study collection section.
 */
QWidget* 
GuiStudyCollectionFileEditorDialog::createStudyCollectionSection()
{
   //
   // collection name
   //
   QLabel* collectionNameLabel = new QLabel("Collection\nName");
   collectionNameLineEdit = new QLineEdit;
   QObject::connect(collectionNameLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionNameLineEditChanged(const QString&)));
                    
   //
   // creator
   //
   QLabel* collectionCreatorLabel = new QLabel("Creator");
   collectionCreatorLineEdit = new QLineEdit;
   QObject::connect(collectionCreatorLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionCreatorLineEditChanged(const QString&)));
    
   //
   // type
   //
   QLabel* collectionTypeLabel = new QLabel("Type");
   collectionTypeLineEdit = new QLineEdit;
   QObject::connect(collectionTypeLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionTypeLineEditChanged(const QString&)));
    
   //
   // comment
   //
   QLabel* collectionCommentLabel = new QLabel("Comment");
   collectionCommentTextEdit = new QTextEdit;
   collectionCommentTextEdit->setMaximumHeight(100);
   QObject::connect(collectionCommentTextEdit, SIGNAL(textChanged()),
                    this, SLOT(slotCollectionCommentTextEditChanged()));
    
   //
   // study name
   //
   QLabel* collectionStudyNameLabel = new QLabel("Study Name");
   collectionStudyNameLineEdit = new QLineEdit;
   QObject::connect(collectionStudyNameLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionStudyNameLineEditChanged(const QString&)));
    
   //
   // pubmed id
   //
   QLabel* collectionStudyPMIDLabel = new QLabel("PubMed ID");
   collectionStudyPMIDLineEdit = new QLineEdit;
   QObject::connect(collectionStudyPMIDLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionStudyPMIDLineEditChanged(const QString&)));
    
   //
   // search id
   //
   QLabel* collectionSearchIDLabel = new QLabel("Search ID");
   collectionSearchIDLineEdit = new QLineEdit;
   QObject::connect(collectionSearchIDLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionSearchIDLineEditChanged(const QString&)));
    
   //
   // Foci List ID
   //
   QLabel* collectionFociListIDLabel = new QLabel("Foci List ID");
   collectionFociListIDLineEdit = new QLineEdit;
   QObject::connect(collectionFociListIDLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionFociListIDLineEditChanged(const QString&)));
    
   //
   // Foci Color List ID
   //
   QLabel* collectionFociColorListIDLabel = new QLabel("Foci Color List ID");
   collectionFociColorListIDLineEdit = new QLineEdit;
   QObject::connect(collectionFociColorListIDLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionFociColorListIDLineEditChanged(const QString&)));
    
   //
   // topic
   //
   QLabel* collectionTopicLabel = new QLabel("Topic");
   collectionTopicLineEdit = new QLineEdit;
   QObject::connect(collectionTopicLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionTopicLineEditChanged(const QString&)));
    
   //
   // category ID
   //
   QLabel* collectionCategoryIDLabel = new QLabel("Category ID");
   collectionCategoryIDLineEdit = new QLineEdit;
   QObject::connect(collectionCategoryIDLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionCategoryIDLineEditChanged(const QString&)));
   
   //
   // collection ID
   QLabel* collectionIDLabel = new QLabel("Collection ID");
   collectionIDLineEdit = new QLineEdit;
   QObject::connect(collectionIDLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCollectionIDLineEditChanged(const QString&)));
                    
   //
   // Layout for collection information
   //
   QGridLayout* infoGridLayout = new QGridLayout;
   infoGridLayout->addWidget(collectionNameLabel, 0, 0);
   infoGridLayout->addWidget(collectionNameLineEdit, 0, 1);
   infoGridLayout->addWidget(collectionIDLabel, 1, 0);
   infoGridLayout->addWidget(collectionIDLineEdit, 1, 1);
   infoGridLayout->addWidget(collectionCreatorLabel, 2, 0);
   infoGridLayout->addWidget(collectionCreatorLineEdit, 2, 1);
   infoGridLayout->addWidget(collectionTypeLabel, 3, 0);
   infoGridLayout->addWidget(collectionTypeLineEdit, 3, 1);
   infoGridLayout->addWidget(collectionCommentLabel, 4, 0);
   infoGridLayout->addWidget(collectionCommentTextEdit, 4, 1);
   infoGridLayout->addWidget(collectionTopicLabel, 5, 0);
   infoGridLayout->addWidget(collectionTopicLineEdit, 5, 1);
   infoGridLayout->addWidget(collectionCategoryIDLabel, 6, 0);
   infoGridLayout->addWidget(collectionCategoryIDLineEdit, 6, 1);
   infoGridLayout->addWidget(collectionStudyNameLabel, 7, 0);
   infoGridLayout->addWidget(collectionStudyNameLineEdit, 7, 1);   
   infoGridLayout->addWidget(collectionStudyPMIDLabel, 8, 0);
   infoGridLayout->addWidget(collectionStudyPMIDLineEdit, 8, 1);
   infoGridLayout->addWidget(collectionSearchIDLabel, 9, 0);
   infoGridLayout->addWidget(collectionSearchIDLineEdit, 9, 1);
   infoGridLayout->addWidget(collectionFociListIDLabel, 10, 0);
   infoGridLayout->addWidget(collectionFociListIDLineEdit, 10, 1);
   infoGridLayout->addWidget(collectionFociColorListIDLabel, 11, 0);
   infoGridLayout->addWidget(collectionFociColorListIDLineEdit, 11, 1);
   infoGridLayout->addWidget(new QLabel(" "), 12, 0); // empty row
   
   //
   // Layout for study name/PMIDs
   //
   QGroupBox* studiesGroupBox = new QGroupBox("Studies");
   collectionStudiesNamePmidLayout = new QGridLayout(studiesGroupBox);
   collectionStudiesNamePmidLayout->addWidget(new QLabel("Index"), 0, 0);
   collectionStudiesNamePmidLayout->addWidget(new QLabel("Delete"), 0, 1);
   collectionStudiesNamePmidLayout->addWidget(new QLabel("Name"), 0, 2);
   collectionStudiesNamePmidLayout->addWidget(new QLabel("PMID"), 0, 3);
   collectionStudiesNamePmidLayout->addWidget(new QLabel("MSL ID"), 0, 4);
   
   //
   // Signal mappers for study names/PMIDS
   //
   collectionStudyNameSignalMapper = new QSignalMapper(this);
   QObject::connect(collectionStudyNameSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotCollectionStudyNameChanged(int)));
   collectionStudyPMIDSignalMapper = new QSignalMapper(this);
   QObject::connect(collectionStudyPMIDSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotCollectionStudyPMIDChanged(int)));
   collectionStudyMSLIDSignalMapper = new QSignalMapper(this);
   QObject::connect(collectionStudyMSLIDSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotCollectionStudyMSLIDChanged(int)));
   collectionStudyDeleteSignalMapper = new QSignalMapper(this);
   QObject::connect(collectionStudyDeleteSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotCollectionStudyDeletePushButton(int)));
                                     
   //
   // Widget and layout
   //
   QWidget* collectionWidget = new QWidget;
   QVBoxLayout* collectionWidgetLayout = new QVBoxLayout(collectionWidget);
   collectionWidgetLayout->addLayout(infoGridLayout);
   collectionWidgetLayout->addWidget(studiesGroupBox);
   
   //
   // Scroll Area
   //
   QScrollArea* scrollArea = new QScrollArea;
   scrollArea->setWidget(collectionWidget);
   scrollArea->setWidgetResizable(true);
   
   //
   // Group box and layout
   //
   QGroupBox* collectionGroupBox = new QGroupBox("Collection Studies");
   QVBoxLayout* collectionLayout = new QVBoxLayout(collectionGroupBox);
   collectionLayout->addWidget(scrollArea);
   
   return collectionGroupBox;
}

/**
 * called when collection name changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionNameLineEditChanged(const QString& text)
{
   getSelectedCollection()->setStudyCollectionName(text);
}

/**
 * called when collection creator changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionCreatorLineEditChanged(const QString& text)
{
   getSelectedCollection()->setStudyCollectionCreator(text);
}

/**
 * called when collection type changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionTypeLineEditChanged(const QString& text)
{
   getSelectedCollection()->setStudyType(text);
}

/**
 * called when collection comment changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionCommentTextEditChanged()
{
   getSelectedCollection()->setComment(collectionCommentTextEdit->toPlainText());
}

/**
 * called when collection study name changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionStudyNameLineEditChanged(const QString& text)
{
   getSelectedCollection()->setStudyName(text);
}

/**
 * called when collection PMID changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionStudyPMIDLineEditChanged(const QString& text)
{
   getSelectedCollection()->setPMID(text);
}

/**
 * called when collection search ID changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionSearchIDLineEditChanged(const QString& text)
{
   getSelectedCollection()->setSearchID(text);
}

/**
 * called when collection foci list ID changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionFociListIDLineEditChanged(const QString& text)
{
   getSelectedCollection()->setFociListID(text);
}
      
/**
 * called when collection foci color list ID changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionFociColorListIDLineEditChanged(const QString& text)
{
   getSelectedCollection()->setFociColorListID(text);
}
      
/**
 * called when collection category ID changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionCategoryIDLineEditChanged(const QString& text)
{
   getSelectedCollection()->setCategoryID(text);
}
      
/**
 * called when collection ID changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionIDLineEditChanged(const QString& text)
{
   getSelectedCollection()->setStudyCollectionID(text);
}
      
/**
 * called when collection topic changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionTopicLineEditChanged(const QString& text)
{
   getSelectedCollection()->setTopic(text);
}

/**
 * create the file operations section.
 */
QWidget* 
GuiStudyCollectionFileEditorDialog::createFileOperationsSection()
{
   //
   // Add study collection push button
   //
   QPushButton* addCollectionPushButton = new QPushButton("Add New Collection...");
   addCollectionPushButton->setAutoDefault(false);
   QObject::connect(addCollectionPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddCollectionPushButton()));
                    
   //
   // Group box and layout
   //
   QGroupBox* fileOperationsGroupBox = new QGroupBox("File Operations");
   QVBoxLayout* fileOperationsLayout = new QVBoxLayout(fileOperationsGroupBox);
   fileOperationsLayout->addWidget(addCollectionPushButton);
   
   return fileOperationsGroupBox;
}

/**
 * called when add new collection push button pressed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotAddCollectionPushButton()
{
   //
   // Should any studies be added to the new collection
   //
   WuQDataEntryDialog ded(this);
   ded.setWindowTitle("Studies for New Collection");
   //QRadioButton* emptyStudyRadioButton = 
      ded.addRadioButton("No Studies", true);
   QRadioButton* fociStudyRadioButton =
      ded.addRadioButton("Add Studies Linked by Displayed Foci");
   QRadioButton* allStudyRadioButton =
      ded.addRadioButton("Add All Studies");
      
   if (ded.exec() == WuQDataEntryDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      StudyCollectionFile* scf = theMainWindow->getBrainSet()->getStudyCollectionFile();
      const StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
      
      StudyCollection* sc = new StudyCollection;
      
      std::vector<QString> studyPMIDs;
      if (fociStudyRadioButton->isChecked()) {
         FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
         fpf->getPubMedIDsOfAllLinkedStudyMetaData(studyPMIDs, true);
      }
      else if (allStudyRadioButton->isChecked()) {
         const int num = smdf->getNumberOfStudyMetaData();
         for (int i = 0; i < num; i++) {
            const StudyMetaData* smd = smdf->getStudyMetaData(i);
            studyPMIDs.push_back(smd->getPubMedID());
         }
      }
      
      const int numStudyPMIDs = static_cast<int>(studyPMIDs.size());
      for (int i = 0; i < numStudyPMIDs; i++) {
         const QString pmid = studyPMIDs[i];
         const int indx = smdf->getStudyIndexFromPubMedID(pmid);
         if (indx >= 0) {
            const StudyMetaData* smd = smdf->getStudyMetaData(indx);
            sc->addStudyPMID(new StudyNamePubMedID(smd->getName(),
                                                   pmid,
                                                   ""));
         }
      }
      
      scf->addStudyCollection(sc);
      
      GuiFilesModified fm;
      fm.setStudyCollectionModified();
      theMainWindow->fileModificationUpdate(fm);
      
      studySelectionSpinBox->setValue(scf->getNumberOfStudyCollections());
      
      loadSelectedCollectionIntoEditor();

      QApplication::restoreOverrideCursor();   
   }
}
      
/**
 * create the collection operations section.
 */
QWidget* 
GuiStudyCollectionFileEditorDialog::createCollectionOperationsSection()
{
   //
   // Add Studies to Collection
   //
   QPushButton* addStudiesPushButton = new QPushButton("Add Studies...");
   addStudiesPushButton->setAutoDefault(false);
   QObject::connect(addStudiesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddStudiesPushButton()));
                    
   //
   // Delete Collection
   //
   QPushButton* deleteCollectionPushButton = new QPushButton("Delete Collection...");
   deleteCollectionPushButton->setAutoDefault(false);
   QObject::connect(deleteCollectionPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteCollectionPushButton()));
                    
   //
   // Group box and layout
   //
   QGroupBox* collectionOperationsGroupBox = new QGroupBox("Collection Operations");
   QVBoxLayout* collectionOperationsLayout = new QVBoxLayout(collectionOperationsGroupBox);
   collectionOperationsLayout->addWidget(addStudiesPushButton);
   collectionOperationsLayout->addWidget(deleteCollectionPushButton);
   
   return collectionOperationsGroupBox;
}

/**
 * called when add studies to collection push button pressed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotAddStudiesPushButton()
{
   StudyCollection* sc = getSelectedCollection();
   if (sc == NULL) {
      return;
   }
   
   const StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   const int numStudies = smdf->getNumberOfStudyMetaData();
   if (numStudies <= 0) {
      QMessageBox::warning(this,
                           "WARNING",
                           "The Study Metadata File contains no studies.");
      return;
   }
   
   //
   // Create dialog for selecting studies
   //
   std::vector<QCheckBox*> checkBoxes;
   WuQDataEntryDialog ded(this, true);
   ded.setWindowTitle("Choose Studies for Collection");
   for (int i = 0; i < numStudies; i++) {
      const StudyMetaData* smd = smdf->getStudyMetaData(i);
      const QString labelText = 
         (smd->getName() 
          + " " 
          + smd->getPubMedID());
      checkBoxes.push_back(ded.addCheckBox(labelText));
   }
   
   //
   // Add studies to current collection
   //
   if (ded.exec() == WuQDataEntryDialog::Accepted) {
      for (int i = 0; i < numStudies; i++) {
         if (checkBoxes[i]->isChecked()) {
            const StudyMetaData* smd = smdf->getStudyMetaData(i);
            sc->addStudyPMID(new StudyNamePubMedID(smd->getName(),
                                                   smd->getPubMedID(),
                                                   ""));
         }
      }
   }
   
   loadSelectedCollectionIntoEditor();
}

/**
 * called when delete collection push button pressed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotDeleteCollectionPushButton()
{
   if (QMessageBox::question(this,
                             "CONFIRM",
                             "Delete current collection?",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes) == QMessageBox::Yes) {
      const int indx = getIndexOfSelectedCollection();
      if (indx >= 0) {
         StudyCollectionFile* scf = theMainWindow->getBrainSet()->getStudyCollectionFile();
         scf->deleteStudyCollection(indx);
   
         GuiFilesModified fm;
         fm.setStudyCollectionModified();
         theMainWindow->fileModificationUpdate(fm);
         
         loadSelectedCollectionIntoEditor();
      }
   }
}
      
/**
 * get the selected study collection.
 */
StudyCollection* 
GuiStudyCollectionFileEditorDialog::getSelectedCollection()
{
   StudyCollection* sc = NULL;
   const int indx = getIndexOfSelectedCollection();
   if (indx >= 0) {
      StudyCollectionFile* scf = theMainWindow->getBrainSet()->getStudyCollectionFile();
      sc = scf->getStudyCollection(indx);
   }
   
   return sc;
}

/**
 * get the index of the selected collection (-1 if invalid).
 */
int 
GuiStudyCollectionFileEditorDialog::getIndexOfSelectedCollection() const
{
   StudyCollectionFile* scf = theMainWindow->getBrainSet()->getStudyCollectionFile();
   const int numCollections = scf->getNumberOfStudyCollections();
   
   const int indx = studySelectionSpinBox->value() - 1;
   if ((indx >= 0) && (indx < numCollections)) {
      return indx;
   }
   
   return -1;
}
      
/**
 * load the currently selected collection into the editor.
 */
void 
GuiStudyCollectionFileEditorDialog::loadSelectedCollectionIntoEditor()
{
   StudyCollection* sc = getSelectedCollection();
   if (sc != NULL) {
      collectionNameLineEdit->setText(sc->getStudyCollectionName());
      collectionCreatorLineEdit->setText(sc->getStudyCollectionCreator());
      collectionTypeLineEdit->setText(sc->getStudyType());
      collectionCommentTextEdit->setText(sc->getComment());
      collectionStudyNameLineEdit->setText(sc->getStudyName());
      collectionStudyPMIDLineEdit->setText(sc->getPMID());
      collectionSearchIDLineEdit->setText(sc->getSearchID());
      collectionTopicLineEdit->setText(sc->getTopic());
      collectionFociListIDLineEdit->setText(sc->getFociListID());
      collectionFociColorListIDLineEdit->setText(sc->getFociColorListID());
      collectionCategoryIDLineEdit->setText(sc->getCategoryID());
      collectionIDLineEdit->setText(sc->getStudyCollectionID());
      
      const int numStudiesInCollection = sc->getNumberOfStudyPMIDs();
      const int numStudiesInDialog = static_cast<int>(collectionStudiesWidgetGroups.size());
      
      //
      // Add new studies as needed
      //
      for (int i = numStudiesInDialog; i < numStudiesInCollection; i++) {
         //
         // Create new widgets for Name/PMID/MSL ID
         //
         QLineEdit* nameLE = new QLineEdit;
         nameLE->setMinimumWidth(160);
         QLineEdit* pmdidLE = new QLineEdit;
         pmdidLE->setFixedWidth(100);
         QLineEdit* mslidLE = new QLineEdit;
         mslidLE->setFixedWidth(100);
         
         //
         // Connect signals for text changed
         //
         QObject::connect(nameLE, SIGNAL(textEdited(const QString&)),
                          collectionStudyNameSignalMapper, SLOT(map()));
         collectionStudyNameSignalMapper->setMapping(nameLE, i);
         QObject::connect(pmdidLE, SIGNAL(textEdited(const QString&)),
                          collectionStudyPMIDSignalMapper, SLOT(map()));
         collectionStudyPMIDSignalMapper->setMapping(pmdidLE, i);
         QObject::connect(mslidLE, SIGNAL(textEdited(const QString&)),
                          collectionStudyMSLIDSignalMapper, SLOT(map()));
         collectionStudyMSLIDSignalMapper->setMapping(mslidLE, i);
         
         //
         // Index in collection
         //
         QLabel* indexLabel = new QLabel(QString::number(i+1));
         
         //
         // Delete push button
         //
         QToolButton* deletePushButton = new QToolButton;
         deletePushButton->setText("X");
         deletePushButton->setToolTip("Delete this Study");
         QObject::connect(deletePushButton, SIGNAL(clicked(bool)),
                          collectionStudyDeleteSignalMapper, SLOT(map()));
         collectionStudyDeleteSignalMapper->setMapping(deletePushButton, i);
         
         //
         // Add to layout
         //
         collectionStudiesNamePmidLayout->addWidget(indexLabel, i + 1, 0, Qt::AlignRight);
         collectionStudiesNamePmidLayout->addWidget(deletePushButton, i + 1, 1);
         collectionStudiesNamePmidLayout->addWidget(nameLE,  i + 1, 2);
         collectionStudiesNamePmidLayout->addWidget(pmdidLE, i + 1, 3);
         collectionStudiesNamePmidLayout->addWidget(mslidLE, i + 1, 4);
         
         //
         // Keep track of widgets
         //
         collectionStudiesDeletePushButtons.push_back(deletePushButton);
         collectionStudiesNameLineEdits.push_back(nameLE);
         collectionStudiesPMIDLineEdits.push_back(pmdidLE);
         collectionStudyMslIDStudyLineEdits.push_back(mslidLE);
         
         //
         // Add new widgets to widget group for that row
         //
         WuQWidgetGroup* wg = new WuQWidgetGroup(this);
         wg->addWidget(nameLE);
         wg->addWidget(pmdidLE);
         wg->addWidget(mslidLE);
         wg->addWidget(deletePushButton);
         collectionStudiesWidgetGroups.push_back(wg);
      }
      
      //
      // Put studies into GUI
      //
      for (int i = 0; i < numStudiesInCollection; i++) {
         const StudyNamePubMedID* snp = sc->getStudyPMID(i);
         collectionStudiesNameLineEdits[i]->setText(snp->getName());
         collectionStudiesPMIDLineEdits[i]->setText(snp->getPubMedID());
         collectionStudyMslIDStudyLineEdits[i]->setText(snp->getMslID());
         collectionStudiesWidgetGroups[i]->setVisible(true);
      }
      
      //
      // Hide unused items in GUI
      //
      for (int i = numStudiesInCollection; i < numStudiesInDialog; i++) {
         collectionStudiesWidgetGroups[i]->setVisible(false);
      }
   }
   
   //
   // Enable widgets if there is a valid study
   //
   collectionOperationsWidget->setEnabled(sc != NULL);
   collectionStudiesWidget->setEnabled(sc != NULL);
}
      
/**
 * called when a study name is changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionStudyNameChanged(int indx)
{
   StudyCollection* sc = getSelectedCollection();
   if (sc != NULL) {
      if ((indx >= 0) && (indx < sc->getNumberOfStudyPMIDs())) {
         StudyNamePubMedID* sid = sc->getStudyPMID(indx);
         sid->setName(collectionStudiesNameLineEdits[indx]->text().trimmed());
      }
   }
}

/**
 * called when a study pmid is changed.
 */
void
GuiStudyCollectionFileEditorDialog::slotCollectionStudyPMIDChanged(int indx)
{
   StudyCollection* sc = getSelectedCollection();
   if (sc != NULL) {
      if ((indx >= 0) && (indx < sc->getNumberOfStudyPMIDs())) {
         StudyNamePubMedID* sid = sc->getStudyPMID(indx);
         sid->setPubMedID(collectionStudiesPMIDLineEdits[indx]->text().trimmed());
      }
   }
}

/**
 * called when a study mslid is changed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionStudyMSLIDChanged(int indx)
{
   StudyCollection* sc = getSelectedCollection();
   if (sc != NULL) {
      if ((indx >= 0) && (indx < sc->getNumberOfStudyPMIDs())) {
         StudyNamePubMedID* sid = sc->getStudyPMID(indx);
         sid->setMslID(collectionStudyMslIDStudyLineEdits[indx]->text().trimmed());
      }
   }
}
      
/**
 * called when a study delete push button pressed.
 */
void 
GuiStudyCollectionFileEditorDialog::slotCollectionStudyDeletePushButton(int indx)
{
   StudyCollection* sc = getSelectedCollection();
   if (sc != NULL) {
      if ((indx >= 0) && (indx < sc->getNumberOfStudyPMIDs())) {
         sc->removeStudyPMID(indx);
         loadSelectedCollectionIntoEditor();
      }
   }
}

      


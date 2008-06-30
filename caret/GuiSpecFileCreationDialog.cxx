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

#include <sstream>

#include <QApplication>
#include <QDir>
#include "WuQFileDialog.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QToolTip>

#include "BrainSet.h"
#include "Categories.h"
#include "GuiStructureComboBox.h"
#include "GuiMainWindow.h"
#include "GuiSpecFileCreationDialog.h"
#include "QtListBoxSelectionDialog.h"
#include "QtUtilities.h"
#include "Species.h"
#include "SpecFile.h"
#include "StereotaxicSpace.h"
#include "StringUtilities.h"
#include "global_variables.h"

/**
 * constructor (dialog is modal).
 */
GuiSpecFileCreationDialog::GuiSpecFileCreationDialog(QDialog* parent)
   : WuQDialog(parent)
{
   setModal(true);
   const int lineEditWidth = 250;
 
   setWindowTitle("Create Spec File");
     
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setSpacing(3);
   dialogLayout->setMargin(3);
   
   //
   // Directory push button
   //
   QPushButton* directoryButton = new QPushButton("Select...");
   directoryButton->setAutoDefault(false);
   directoryButton->setToolTip(
                 "Press this button to set the directory\n"
                 "in which the new spec file will be placed.");
   QObject::connect(directoryButton, SIGNAL(clicked()),
                    this, SLOT(slotDirectoryPushButton()));
   
   //
   // Directory line edit
   //
   directoryLineEdit = new QLineEdit;
   directoryLineEdit->setText(QDir::currentPath());
   directoryLineEdit->setReadOnly(true);
   directoryLineEdit->setMinimumWidth(lineEditWidth);
   
   //
   // Group box and layout for directory
   //
   QGroupBox* directoryGroup = new QGroupBox("Directory");
   dialogLayout->addWidget(directoryGroup);
   QHBoxLayout* directoryLayout = new QHBoxLayout(directoryGroup);
   directoryLayout->addWidget(directoryButton);
   directoryLayout->addWidget(directoryLineEdit);
   

   //
   // Species name line edit
   //
   QPushButton* speciesButton = new QPushButton("Species...");
   speciesButton->setAutoDefault(false);
   speciesButton->setToolTip(
                 "Press this button select from\n"
                 "a list of valid species.");
   QObject::connect(speciesButton, SIGNAL(clicked()),
                    this, SLOT(slotSpeciesPushButton()));
   speciesLineEdit = new QLineEdit;
   speciesLineEdit->setMinimumWidth(lineEditWidth);
   QString defSpecies(theMainWindow->getBrainSet()->getSpecies());
   if (defSpecies.isEmpty()) {
      defSpecies = "Human";
   }
   speciesLineEdit->setText(defSpecies);
    
   //
   // case name line edit
   //
   QLabel* subjectLabel = new QLabel("Subject");
   subjectLineEdit = new QLineEdit;
   subjectLineEdit->setMinimumWidth(lineEditWidth);
   subjectLineEdit->setText(theMainWindow->getBrainSet()->getSubject());
   subjectLineEdit->setToolTip(
                 "Enter the name of\n"
                 "the subject here.");
    
   //
   // structure combo box and line edit
   //
   QLabel* structureLabel = new QLabel("Structure");
   structureComboBox = new GuiStructureComboBox(0, "structureComboBox",
                                                 true);
   structureComboBox->setStructure(theMainWindow->getBrainSet()->getStructure().getType());
   
   //
   // space
   //
   QPushButton* spacePushButton = new QPushButton("Space...");
   spacePushButton->setAutoDefault(false);
   QObject::connect(spacePushButton, SIGNAL(clicked()),
                    this, SLOT(slotSpacePushButton()));
   spacePushButton->setToolTip(
                 "Press this button select from a\n"
                 "list of valid stereotaxic spaces.");
   spaceLineEdit = new QLineEdit;
   spaceLineEdit->setFixedWidth(lineEditWidth);
   QString defSpace(theMainWindow->getBrainSet()->getStereotaxicSpace());
   if (defSpace.isEmpty()) {
      defSpace = "Other";
   }
   spaceLineEdit->setText(defSpace);
   
   //
   // Category line edit
   //
   QPushButton* categoryPushButton = new QPushButton("Category...");
   categoryPushButton->setAutoDefault(false);
   QObject::connect(categoryPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCategoryPushButton()));
   categoryPushButton->setToolTip(
                 "Press this button select from\n"
                 "a list of valid categories.");
   categoryLineEdit = new QLineEdit;
   categoryLineEdit->setMinimumWidth(lineEditWidth);
   std::vector<QString> categoryValues;
   categoryLineEdit->setText("INDIVIDUAL");
   
   //
   // Group box for spec info
   //
   QGroupBox* specInfoGroupBox = new QGroupBox("Subject Information");
   dialogLayout->addWidget(specInfoGroupBox);
   QGridLayout* specInfoLayout = new QGridLayout(specInfoGroupBox);
   int rowNum = 0;
   specInfoLayout->addWidget(speciesButton, rowNum, 0);
   specInfoLayout->addWidget(speciesLineEdit, rowNum, 1);
   rowNum++;
   specInfoLayout->addWidget(subjectLabel, rowNum, 0);
   specInfoLayout->addWidget(subjectLineEdit, rowNum, 1);
   rowNum++;
   specInfoLayout->addWidget(structureLabel, rowNum, 0);
   specInfoLayout->addWidget(structureComboBox, rowNum, 1);
   rowNum++;
   specInfoLayout->addWidget(spacePushButton, rowNum, 0);
   specInfoLayout->addWidget(spaceLineEdit, rowNum, 1);
   rowNum++;
   specInfoLayout->addWidget(categoryPushButton, rowNum, 0);
   specInfoLayout->addWidget(categoryLineEdit, rowNum, 1);
   rowNum++;
   
   //
   // Comment text edit
   //
   commentTextEdit = new QTextEdit;
   QGroupBox* commentGroupBox = new QGroupBox("Comment");
   dialogLayout->addWidget(commentGroupBox);
   QVBoxLayout* commentGroupLayout = new QVBoxLayout(commentGroupBox);
   commentGroupLayout->addWidget(commentTextEdit);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(3);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   buttonsLayout->addWidget(okButton);
   okButton->setAutoDefault(true);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
                    
   //
   // Cancel Button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * destructor.
 */
GuiSpecFileCreationDialog::~GuiSpecFileCreationDialog()
{
}

/**
 * called to set directory.
 */
void 
GuiSpecFileCreationDialog::slotDirectoryPushButton()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setFileMode(WuQFileDialog::DirectoryOnly);
   fd.setWindowTitle("Choose Directory");
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   if (fd.exec() == QDialog::Accepted) {
      const QString ds = fd.directory().absolutePath();
      directoryLineEdit->setText(ds);
   }
}

/**
 * called when species button pressed.
 */
void 
GuiSpecFileCreationDialog::slotSpeciesPushButton()      
{
   std::vector<QString> values;
   Species::getAllSpecies(values);
   
   int defaultIndex = 0;
   const QString currentValue = speciesLineEdit->text();
   for (int i = 0; i < static_cast<int>(values.size()); i++) {
      if (currentValue == values[i]) {
         defaultIndex = i;
         break;
      }
   }
   
   QtListBoxSelectionDialog lbsd(this,
                                  "Choose Species",
                                  "",
                                  values,
                                  defaultIndex);
   if (lbsd.exec() == QDialog::Accepted) {
      speciesLineEdit->setText(lbsd.getSelectedText());
   }
}

/** 
 * called when category button pressed.
 */
void 
GuiSpecFileCreationDialog::slotCategoryPushButton()
{
   std::vector<QString> values;
   Categories::getAllCategories(values);
   
   int defaultIndex = 1;
   const QString currentValue = categoryLineEdit->text();
   for (int i = 0; i < static_cast<int>(values.size()); i++) {
      if (currentValue == values[i]) {
         defaultIndex = i;
         break;
      }
   }
   
   QtListBoxSelectionDialog lbsd(this,
                                  "Choose Category",
                                  "",
                                  values,
                                  defaultIndex);
   if (lbsd.exec() == QDialog::Accepted) {
      categoryLineEdit->setText(lbsd.getSelectedText());
   }
}

/**
 * called when space button pressed.
 */
void 
GuiSpecFileCreationDialog::slotSpacePushButton()
{
   std::vector<StereotaxicSpace> allSpaces;
   StereotaxicSpace::getAllStereotaxicSpaces(allSpaces);

   std::vector<QString> values;   
   int defaultIndex = 0;
   const QString currentValue = spaceLineEdit->text();
   for (int i = 0; i < static_cast<int>(allSpaces.size()); i++) {
      values.push_back(allSpaces[i].getName());
      if (currentValue == values[i]) {
         defaultIndex = i;
      }
   }
   
   QtListBoxSelectionDialog lbsd(this,
                                  "Choose Stereotaxic Space",
                                  "",
                                  values,
                                  defaultIndex);
   if (lbsd.exec() == QDialog::Accepted) {
      spaceLineEdit->setText(lbsd.getSelectedText());
   }
}
      
/**
 * called when OK or Cancel button pressed.
 */
void 
GuiSpecFileCreationDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QString msg;
      
      QString directory = directoryLineEdit->text();
      QDir dir(directory);
      if (dir.exists() == false) {
         if (dir.mkdir(".") == false) {
            QMessageBox::critical(this, "ERROR", "Unable to create spec file directory.");
            return;
         }
      }
      
      const Structure::STRUCTURE_TYPE hem = structureComboBox->getSelectedStructure();
      if (hem == Structure::STRUCTURE_TYPE_INVALID) {
         msg.append("Structure type of INVALID is not allowed.\n");
      }
      
      QString hemString = Structure::convertTypeToAbbreviatedString(hem);
/*
      switch (hem) {
         case Structure::STRUCTURE_TYPE_INVALID:
           hemString = "U");
           break;
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
           hemString = "L");
           break;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
           hemString = "R");
           break;
         case Structure::STRUCTURE_TYPE_CEREBELLUM:
           hemString = "LR");
           break;
      }
*/      
      QString species = speciesLineEdit->text();
      if (species.isEmpty()) {
         msg.append("No Species was entered.\n");
      }
      
      QString subject = subjectLineEdit->text();
      if (subject.isEmpty()) {
         msg.append("No Subject was entered.\n");
      }

      if (msg.isEmpty() == false) {
         QMessageBox::critical(this, "ERROR", msg);
         return;
      }
      
      species = StringUtilities::replace(species, ' ', '_');
      subject = StringUtilities::replace(subject, ' ', '_');
      
      std::ostringstream str;
      str << species.toAscii().constData()
          << "."
          << subject.toAscii().constData()
          << "."
          << hemString.toAscii().constData();
      if (theMainWindow->getBrainSet()->getNumberOfNodes() > 0) {
         str << "."
             << theMainWindow->getBrainSet()->getNumberOfNodes();
      }
      
      str << SpecFile::getSpecFileExtension().toAscii().constData();
      
      QString specFileName = str.str().c_str();
      
      bool valid = false;
      QApplication::beep();
      
      QString qName = QInputDialog::getText(this, "Spec File Name",
                                "Name of Spec File that will be created.  You may change the name.",
                                QLineEdit::Normal,
                                specFileName,
                                &valid);
      specFileName = "";
      if (valid && !qName.isEmpty()) {
         specFileName = qName;
      }
      if (valid) {
         if (specFileName.isEmpty()) {
            QMessageBox::critical(this, "ERROR", "Spec File name is empty.");
            return;
         }
         
         //
         // Set to the user entered directory
         //
         QDir::setCurrent(directory);
         
         //
         // Try to read the spec file but ignore read errors since it might not exist
         //
         SpecFile sf;
         try {
            sf.readFile(specFileName);
         }
         catch (FileException&) {
         }
         
         //
         // Comment
         //
         sf.setFileComment(commentTextEdit->toPlainText());
         
         //
         // Name with path that can be requested by creator
         //
         createdSpecFileName = directory;
         if (createdSpecFileName.isEmpty() == false) {
            createdSpecFileName.append("/");
         }
         createdSpecFileName.append(specFileName);
         
         //
         // Set brain parameters
         //
         theMainWindow->getBrainSet()->setStereotaxicSpace(spaceLineEdit->text());
         theMainWindow->getBrainSet()->setSubject(subject);
         theMainWindow->getBrainSet()->setSpecies(species);
         theMainWindow->getBrainSet()->setStructure(hem);
         theMainWindow->getBrainSet()->setSpecFileName(createdSpecFileName);

         //
         // Set spec file parameters
         //
         sf.setSpecies(species);
         sf.setSubject(subject);
         sf.setStructure(Structure::convertTypeToString(hem));
         sf.setSpace(spaceLineEdit->text());
         sf.setCategory(categoryLineEdit->text());
         
         //
         // Write the spec file
         //
         try {
            sf.writeFile(specFileName);
         }
         catch (FileException& e) {
            QString s("Unable to create spec file.\n");
            s.append(e.whatQString());
            QMessageBox::critical(this, "ERROR", s);
            createdSpecFileName = "";
            return;
         }
         
      }
      else {
         return;
      }
   }
   
   QDialog::done(r);
}



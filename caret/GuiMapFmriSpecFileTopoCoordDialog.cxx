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
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>

#include "FileUtilities.h"
#include "GuiChooseSpecFileDialog.h"
#include "GuiMapFmriSpecFileTopoCoordDialog.h"
#include "QtUtilities.h"
#include "SpecFile.h"

/**
 * Constructor.
 */
GuiMapFmriSpecFileTopoCoordDialog::GuiMapFmriSpecFileTopoCoordDialog(QWidget* parent,
                                                                     PreferencesFile* pf)
   : QtDialog(parent, true)
{
   setWindowTitle("Mapping Surface Selection");
   
   preferencesFile = pf;
   
   //
   // Spec file selection
   //
   QPushButton* selectSpecPushButton = new QPushButton("Select...");
   selectSpecPushButton->setAutoDefault(false);
   selectSpecPushButton->setFixedSize(selectSpecPushButton->sizeHint());
   QObject::connect(selectSpecPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectSpecPushButton()));
   
   specFileLineEdit = new QLineEdit;
   specFileLineEdit->setReadOnly(true);
   specFileLineEdit->setMinimumWidth(250);
   
   //
   // Group box and layout for spec file selection
   //
   QGroupBox* specGroupBox = new QGroupBox("Spec File");
   QHBoxLayout* specGroupLayout = new QHBoxLayout(specGroupBox);
   specGroupLayout->addWidget(selectSpecPushButton);
   specGroupLayout->addWidget(specFileLineEdit);
   
   //
   // Topology selection
   //
   topoFileComboBox = new QComboBox;
   topoGroupBox = new QGroupBox("Topology Files");
   QHBoxLayout* topoGroupLayout = new QHBoxLayout(topoGroupBox);
   topoGroupLayout->addWidget(topoFileComboBox);
   
   //
   // Coordinate file selection
   //
   coordFileListBox = new QListWidget;
   coordFileListBox->setSelectionMode(QListWidget::ExtendedSelection);
   coordFileListBox->setMinimumWidth(500);

   //
   // Select all coords button
   //
   QPushButton* selectAllCoordPushButton = new QPushButton("Select All Coord Files");
   selectAllCoordPushButton->setFixedSize(selectAllCoordPushButton->sizeHint());
   selectAllCoordPushButton->setAutoDefault(false);
   QObject::connect(selectAllCoordPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectAllCoordPushButton()));
   
   //
   // Coordinate group box and layout
   //
   coordGroupBox = new QGroupBox("Fiducial Coordinate Files");
   QVBoxLayout* coordGroupLayout = new QVBoxLayout(coordGroupBox);
   coordGroupLayout->addWidget(coordFileListBox);
   coordGroupLayout->addWidget(selectAllCoordPushButton);
   
   const QString instructions = 
                   "Choose a topology file and one or more fiducial coordinate files.  "
                   "All coordinate files MUST be in the same stereotaxic space as the "
                   "functional volumes.  Hold down the shift key or the CTRL key (Apple "
                   "key on Macintoshes) to select multiple coordinate files.";
   
   QTextEdit* textEditor = new QTextEdit;
   textEditor->setReadOnly(true);
   textEditor->setPlainText(instructions);

   //
   // OK and Cancel buttons
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(okButton);
   buttonsLayout->addWidget(cancelButton);
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(specGroupBox);
   dialogLayout->addWidget(topoGroupBox);
   dialogLayout->addWidget(coordGroupBox);
   dialogLayout->addWidget(textEditor);
   dialogLayout->addLayout(buttonsLayout);   
}

                           
/**
 * Destructor.
 */
GuiMapFmriSpecFileTopoCoordDialog::~GuiMapFmriSpecFileTopoCoordDialog()
{
}

/**
 * called when select spec file pushbutton pressed.
 */
void 
GuiMapFmriSpecFileTopoCoordDialog::slotSelectSpecPushButton()
{
   //
   // Popup the choose spec file dialog
   //
   GuiChooseSpecFileDialog sfd(this, preferencesFile, true);
   if (sfd.exec() == QDialog::Accepted) {
      const QString specFileName = sfd.getSelectedSpecFile();
      specFileLineEdit->setText(specFileName);

      //
      // Clear and disable topo and coord selections
      //      
      topoFileComboBox->clear();
      coordFileListBox->clear();
      topoGroupBox->setEnabled(false);
      coordGroupBox->setEnabled(false);
      
      //
      // Clear the topo and coord file 
      //
      //
      // Read the spec file
      //
      try {
         specFile.readFile(specFileName);
      }
      catch (FileException& e) {
         QString msg("Unable to read spec file ");
         msg.append(FileUtilities::basename(specFileName));
         QMessageBox::critical(this, "ERROR", msg);
         return;
      }

      //
      // Load the topo files into combo box
      //
      const int numTopo = static_cast<int>(specFile.closedTopoFile.getNumberOfFiles());
      for (int i = 0; i < numTopo; i++) {
         topoFileComboBox->addItem(
            FileUtilities::basename(specFile.closedTopoFile.getFileName(i)));
      }
      QString errorMessage;
      if (numTopo > 0) {
         topoGroupBox->setEnabled(true);
      }
      else {
         errorMessage.append("Closed topology files are required and there are none in the selected spec file.");
      }
      
      //
      // load the coord file list box
      //
      const int numCoord = static_cast<int>(specFile.fiducialCoordFile.getNumberOfFiles());
      for (int i = 0; i < numCoord; i++) {
         coordFileListBox->insertItem(i,
            FileUtilities::basename(specFile.fiducialCoordFile.getFileName(i)));
      }
      if (numCoord > 0) {
         if (numCoord == 1) {
            coordFileListBox->setCurrentRow(0);
         }
         coordGroupBox->setEnabled(true);
      }
      else {
         if (errorMessage.isEmpty() == false) {
            errorMessage.append("\n");
         }
         errorMessage.append("Fiducial coordinate files are required and there are none in the selected spec file.");
      }
      
      if (errorMessage.isEmpty() == false) {
         QMessageBox::critical(this, "ERROR", errorMessage);
         return;
      }
   }
}

/**
 * called when select all coord files pushbutton is pressed.
 */
void 
GuiMapFmriSpecFileTopoCoordDialog::slotSelectAllCoordPushButton()
{
   coordFileListBox->selectAll();
}
      
/**
 * Called when OK or Cancel button pressed.
 */
void
GuiMapFmriSpecFileTopoCoordDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      std::vector<QString> names;
      getCoordFileNames(names);
      if (names.empty()) {
         QMessageBox::critical(this, "ERROR", 
                       "You must select at least one coordinate file.");
         return;
      }
   }
   
   QDialog::done(r);
}

/**
 * Get the name of the selected spec file.
 */
QString
GuiMapFmriSpecFileTopoCoordDialog::getSpecFileName() const 
{
   return specFileLineEdit->text();
}

/**
 * get the name of the topo file.
 */
QString 
GuiMapFmriSpecFileTopoCoordDialog::getTopoFileName() const
{
   QString name;
   
   if (topoFileComboBox != NULL) {
      name = specFile.closedTopoFile.getFileName(topoFileComboBox->currentIndex());
   }
   
   return name;
}


/**
 * get the name of the coordinate file.
 */
void 
GuiMapFmriSpecFileTopoCoordDialog::getCoordFileNames(std::vector<QString>& names) const
{
   names.clear();
   if (coordFileListBox == NULL) {
      return;
   }
   
   for (int i = 0; i < static_cast<int>(specFile.fiducialCoordFile.getNumberOfFiles()); i++) {
      const QListWidgetItem* item = coordFileListBox->item(i);
      if (coordFileListBox->isItemSelected(item)) {
         names.push_back(specFile.fiducialCoordFile.getFileName(i));
      }
   }
}



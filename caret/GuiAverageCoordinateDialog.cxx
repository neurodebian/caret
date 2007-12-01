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

#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>

#include "CoordinateFile.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiAverageCoordinateDialog.h"
#include "GuiFileDialogWithInstructions.h"
#include "QtUtilities.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "WuQFileDialog.h"

/**
 * Constructor.
 */
GuiAverageCoordinateDialog::GuiAverageCoordinateDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setWindowTitle("Average Coordinates");

   const int lineEditWidth = 400;
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Group box for input coordinate files
   //
   QGroupBox* inputGroupBox = new QGroupBox("Input Coordinate Files");
   dialogLayout->addWidget(inputGroupBox);
   QVBoxLayout* inputGroupLayout = new QVBoxLayout(inputGroupBox);
   
   //
   // list box for input coordinate files
   //
   inputListWidget = new QListWidget;
   inputGroupLayout->addWidget(inputListWidget);
   inputListWidget->setSelectionMode(QListWidget::ExtendedSelection);
   
   //
   // Buttons for adding/removing coordinate files
   //
   QHBoxLayout* inputButtonBoxLayout = new QHBoxLayout;
   inputGroupLayout->addLayout(inputButtonBoxLayout);
   
   //
   // Add coordinate files button
   //
   QPushButton* addButton = new QPushButton("Add...");
   inputButtonBoxLayout->addWidget(addButton);
   addButton->setAutoDefault(false);
   QObject::connect(addButton, SIGNAL(clicked()),
                    this, SLOT(slotAddButton()));
                    
   //
   // Remove coordinate files button
   //
   QPushButton* removeButton = new QPushButton("Remove...");
   inputButtonBoxLayout->addWidget(removeButton);
   removeButton->setAutoDefault(false);
   QObject::connect(removeButton, SIGNAL(clicked()),
                    this, SLOT(slotRemoveButton()));
                    
   //
   // Make Add/Remove buttons the same size
   //
   QtUtilities::makeButtonsSameSize(addButton, removeButton);
   
   //
   // Group box for output coordinate file
   //
   QGroupBox* outputGroupBox = new QGroupBox("Output Coordinate File");
   dialogLayout->addWidget(outputGroupBox);
   QHBoxLayout* outputGroupLayout = new QHBoxLayout(outputGroupBox);
   
   //
   // Button to select output coordinate file
   //
   QPushButton* outputCoordinateButton = new QPushButton("Select...");
   outputGroupLayout->addWidget(outputCoordinateButton);
   outputCoordinateButton->setAutoDefault(false);
   outputCoordinateButton->setFixedSize(outputCoordinateButton->sizeHint());
   QObject::connect(outputCoordinateButton, SIGNAL(clicked()),
                    this, SLOT(slotOutputCoordinateButton()));
   
   //
   // Line edit for output coordinate file name
   //
   outputCoordinateFileLineEdit = new QLineEdit;
   outputGroupLayout->addWidget(outputCoordinateFileLineEdit);
   outputCoordinateFileLineEdit->setMinimumWidth(lineEditWidth);
   outputCoordinateFileLineEdit->setText("average.coord");
       
   //
   // Group box for surface shape file
   //
   QGroupBox* shapeGroupBox = new QGroupBox("Surface Shape");
   dialogLayout->addWidget(shapeGroupBox);
   QVBoxLayout* shapeGroupLayout = new QVBoxLayout(shapeGroupBox);
   
   //
   // Enable shape check box
   //
   createShapeCheckBox = new QCheckBox("Create Sample Standard Deviation");
   shapeGroupLayout->addWidget(createShapeCheckBox);
   createShapeCheckBox->setChecked(true);
                                       
   //
   // Grid for file and column name
   //
   QWidget* shapeGridWidget = new QWidget;
   shapeGroupLayout->addWidget(shapeGridWidget);
   QGridLayout* shapeGridLayout = new QGridLayout(shapeGridWidget);
   shapeGridLayout->setSpacing(5);
   
   //
   // file name label and line edit
   //
   shapeGridLayout->addWidget(new QLabel("File Name "), 0, 0);
   shapeFileNameLineEdit = new QLineEdit;
   shapeFileNameLineEdit->setText("avg_coord_uncertainty.surface_shape");
   shapeFileNameLineEdit->setMinimumWidth(lineEditWidth);
   shapeGridLayout->addWidget(shapeFileNameLineEdit, 0, 1);

   //
   // column name label and line edit
   //
   shapeGridLayout->addWidget(new QLabel("Column Name "), 1, 0);
   shapeColumnNameLineEdit = new QLineEdit;
   shapeColumnNameLineEdit->setText("SHAPE_STANDARD_UNCERTAINTY");
   shapeColumnNameLineEdit->setMinimumWidth(lineEditWidth);
   shapeGridLayout->addWidget(shapeColumnNameLineEdit, 1, 1);

   //
   // Enable shape grid when checkbox is enabled
   //
   QObject::connect(createShapeCheckBox, SIGNAL(toggled(bool)),
                    shapeGridWidget, SLOT(setEnabled(bool)));                                    
   shapeGridWidget->setEnabled(createShapeCheckBox->isChecked());
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(2);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                  this, SLOT(slotApplyButton()));
                  
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                  this, SLOT(close()));
   
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
}

/**
 * Destructor.
 */
GuiAverageCoordinateDialog::~GuiAverageCoordinateDialog()
{
}

/**
 * called to select output coordinate file name.
 */
void
GuiAverageCoordinateDialog::slotOutputCoordinateButton()
{
   WuQFileDialog outputCoordinateFileDialog(this);
   outputCoordinateFileDialog.setDirectory(QDir::currentPath());
   outputCoordinateFileDialog.setModal(true);
   outputCoordinateFileDialog.setWindowTitle("Choose Coordinate File");
   outputCoordinateFileDialog.setFileMode(WuQFileDialog::AnyFile);
   outputCoordinateFileDialog.setFilter(FileFilters::getCoordinateGenericFileFilter());
   if (outputCoordinateFileDialog.exec() == QDialog::Accepted) {
      outputCoordinateFileLineEdit->setText(outputCoordinateFileDialog.selectedFiles().at(0));
   }
}

/**
 * called when add button is pressed.
 */
void 
GuiAverageCoordinateDialog::slotAddButton()
{
   QString instructions = "To select multiple coordinate files, hold down the CTRL key while selecting "
                          "coordinate file names with the mouse (Macintosh users should hold down "
                          "the Apple key).";
   GuiFileDialogWithInstructions openCoordinateFileDialog(this, instructions, "chooseCoordinateFile", true);
   openCoordinateFileDialog.setWindowTitle("Choose Coordinate File");
   openCoordinateFileDialog.setFileMode(GuiFileDialogWithInstructions::ExistingFiles);
   openCoordinateFileDialog.setFilters(QStringList(FileFilters::getCoordinateGenericFileFilter()));
   openCoordinateFileDialog.selectFilter(FileFilters::getCoordinateGenericFileFilter());
   if (openCoordinateFileDialog.exec() == QDialog::Accepted) {
      QStringList list = openCoordinateFileDialog.selectedFiles();
      QStringList::Iterator it = list.begin();
      while( it != list.end() ) {
         QString filename((*it));
         coordinateFileNames.push_back(filename);
         ++it;
      }
   }
   
   loadCoordinateFileListBox();
}

/**
 * Load the coordinate file list box
 */
void
GuiAverageCoordinateDialog::loadCoordinateFileListBox()
{
   inputListWidget->clear();
   
   for (int i = 0; i < static_cast<int>(coordinateFileNames.size()); i++) {
      inputListWidget->addItem(FileUtilities::basename(coordinateFileNames[i]));
   }
}

/**
 * called when remove button is pressed.
 */
void 
GuiAverageCoordinateDialog::slotRemoveButton()
{
   for (int i = static_cast<int>(coordinateFileNames.size()) - 1; i >= 0; i--) {
      const QListWidgetItem* item = inputListWidget->item(i);
      if (item != NULL) {
         if (inputListWidget->isItemSelected(item)) {
            coordinateFileNames.erase(coordinateFileNames.begin() + i);
         }
      }
   }
      
   loadCoordinateFileListBox();
}

/**
 * called when apply button is pressed.
 */
void 
GuiAverageCoordinateDialog::slotApplyButton()
{
   //
   // Verify valid data entered
   //
   if (coordinateFileNames.empty()) {
      QMessageBox::critical(this, "ERROR", "No input coordinate file are selected.");
      return;
   }
   QString outputFileName(outputCoordinateFileLineEdit->text());
   if (outputFileName.length() <= 0) {
      QMessageBox::critical(this, "ERROR", "No output coordinate file specified.");
      return;      
   }
   
   //
   // The surface shape file
   //
   SurfaceShapeFile* ssf = NULL;
   const QString shapeFileName(shapeFileNameLineEdit->text());
   const QString shapeColumnName(shapeColumnNameLineEdit->text());
   if (createShapeCheckBox->isChecked()) {
      if (shapeFileName.isEmpty()) {
         QMessageBox::critical(this, "ERROR", "No surface shape file specified.");
         return;      
      }
      if (shapeColumnName.isEmpty()) {
         QMessageBox::critical(this, "ERROR", "No surface shape column name specified.");
         return;      
      }
      ssf = new SurfaceShapeFile;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // Read the coordinate files
   //
   std::vector<CoordinateFile*> coordinateFiles;
   for (int i = 0; i < static_cast<int>(coordinateFileNames.size()); i++) {
      CoordinateFile* c = new CoordinateFile;
      try {
         c->readFile(coordinateFileNames[i]);
         coordinateFiles.push_back(c);
      }
      catch (FileException& e) {
         for (int i = 0; i < static_cast<int>(coordinateFiles.size()); i++) {
            delete coordinateFiles[i];
         }
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "File Read Error", e.whatQString());
         return;
      }
   }
   
   //
   // Create the average coordinate file
   //
   CoordinateFile averageCoordinateFile;
   try {
      CoordinateFile::createAverageCoordinateFile(coordinateFiles,
                                              averageCoordinateFile,
                                              ssf);
   }
   catch (FileException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "ERROR", e.whatQString());
      return;
   }
   
   //
   // Write the coordinate file
   //
   try {
      //
      // Add the file extension, if needed
      //
      const QString ext(SpecFile::getCoordinateFileExtension());
      if (StringUtilities::endsWith(outputFileName, ext) == false) {
         outputFileName.append(ext);
      }
      averageCoordinateFile.writeFile(outputFileName);
   }
   catch (FileException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "ERROR", e.whatQString());
      if (ssf != NULL) {
         delete ssf;
         ssf = NULL;
      }
      return;
   }
   
   //
   // Write the surface shape file
   //
   if (ssf != NULL) {
      const int column = ssf->getNumberOfColumns() - 1;
      if (column >= 0) {
         ssf->setColumnName(column, shapeColumnName);
      }
      
      try {
         ssf->writeFile(shapeFileName);
      }
      catch (FileException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
         return;         
      }
      delete ssf;
      ssf = NULL;
   }
   
   //
   // Free memory
   //
   for (int i = 0; i < static_cast<int>(coordinateFiles.size()); i++) {
      delete coordinateFiles[i];
   }
   
   QApplication::restoreOverrideCursor();
   
   //
   // Let the user know the file was created.
   //
   QMessageBox::information(this, "Success", "Average Coordinate File Created");
}

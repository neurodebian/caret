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

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>

#include "BorderFile.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiAverageBorderDialog.h"
#include "GuiFileDialogWithInstructions.h"
#include "QtUtilities.h"
#include "WuQFileDialog.h"
#include "StringUtilities.h"

/**
 * Constructor.
 */
GuiAverageBorderDialog::GuiAverageBorderDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setWindowTitle("Average Borders");

   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Group box for input border files
   //
   QGroupBox* inputGroupBox = new QGroupBox("Input Border Files");
   dialogLayout->addWidget(inputGroupBox);
   QVBoxLayout* inputGroupLayout = new QVBoxLayout(inputGroupBox);
   
   //
   // list box for input border files
   //
   inputListWidget = new QListWidget;
   inputListWidget->setSelectionMode(QListWidget::ExtendedSelection);
   inputGroupLayout->addWidget(inputListWidget);
   
   //
   // Buttons for adding/removing border files
   //
   QHBoxLayout* inputButtonBoxLayout = new QHBoxLayout;
   inputGroupLayout->addLayout(inputButtonBoxLayout);
   
   //
   // Add border files button
   //
   QPushButton* addButton = new QPushButton("Add...");
   inputButtonBoxLayout->addWidget(addButton);
   addButton->setAutoDefault(false);
   QObject::connect(addButton, SIGNAL(clicked()),
                    this, SLOT(slotAddButton()));
                    
   //
   // Remove border files button
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
   // Group box for output border file
   //
   QGroupBox* outputGroupBox = new QGroupBox("Output Border File");
   dialogLayout->addWidget(outputGroupBox);
   QHBoxLayout* outputGroupLayout = new QHBoxLayout(outputGroupBox);

   //
   // Button to select output border file
   //
   QPushButton* outputBorderButton = new QPushButton("Select...");
   outputGroupLayout->addWidget(outputBorderButton);
   outputBorderButton->setAutoDefault(false);
   outputBorderButton->setFixedSize(outputBorderButton->sizeHint());
   QObject::connect(outputBorderButton, SIGNAL(clicked()),
                    this, SLOT(slotOutputBorderButton()));
   
   //
   // Line edit for output border file name
   //
   outputBorderFileLineEdit = new QLineEdit;
   outputGroupLayout->addWidget(outputBorderFileLineEdit);
   outputBorderFileLineEdit->setMinimumWidth(200);
                                            
   //
   // Group box for resampling
   //
   QGroupBox* resampleGroupBox = new QGroupBox("Resampling");
   dialogLayout->addWidget(resampleGroupBox);
   QVBoxLayout* resampleGroupLayout = new QVBoxLayout(resampleGroupBox);
   
   //
   // Resampling distance
   //
   QHBoxLayout* distHBoxLayout = new QHBoxLayout;
   resampleGroupLayout->addLayout(distHBoxLayout);
   distHBoxLayout->addWidget(new QLabel("Distance (mm) "));
   resamplingDoubleSpinBox = new QDoubleSpinBox;
   resamplingDoubleSpinBox->setMinimum(0.0);
   resamplingDoubleSpinBox->setMaximum(100000.0);
   resamplingDoubleSpinBox->setSingleStep(1.0);
   resamplingDoubleSpinBox->setDecimals(2);
   resamplingDoubleSpinBox->setValue(10.0);
   distHBoxLayout->addWidget(resamplingDoubleSpinBox);

   // 
   // Sphere check box
   //
   sphereCheckBox = new QCheckBox("Project to Sphere");
   resampleGroupLayout->addWidget(sphereCheckBox);
      
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
GuiAverageBorderDialog::~GuiAverageBorderDialog()
{
}

/**
 * called to select output border file name.
 */
void
GuiAverageBorderDialog::slotOutputBorderButton()
{
   WuQFileDialog outputBorderFileDialog(this);
   outputBorderFileDialog.setModal(true);
   outputBorderFileDialog.setWindowTitle("Choose Border File");
   outputBorderFileDialog.setFileMode(WuQFileDialog::AnyFile);
   outputBorderFileDialog.setFilter(FileFilters::getBorderGenericFileFilter());
   if (outputBorderFileDialog.exec() == QDialog::Accepted) {
      outputBorderFileLineEdit->setText(outputBorderFileDialog.selectedFiles().at(0));
   }
}

/**
 * called when add button is pressed.
 */
void 
GuiAverageBorderDialog::slotAddButton()
{
   QString instructions = "To select multiple border files, hold down the CTRL key while selecting "
                          "border file names with the mouse (Macintosh users should hold down "
                          "the Apple key).";
   GuiFileDialogWithInstructions openBorderFileDialog(this, instructions, "chooseBorderFile", true);
   openBorderFileDialog.setWindowTitle("Choose Border File");
   openBorderFileDialog.setFileMode(GuiFileDialogWithInstructions::ExistingFiles);
   openBorderFileDialog.setFilters(QStringList(FileFilters::getBorderGenericFileFilter()));
   openBorderFileDialog.selectFilter(FileFilters::getBorderGenericFileFilter());
   if (openBorderFileDialog.exec() == QDialog::Accepted) {
      QStringList list = openBorderFileDialog.selectedFiles();
      QStringList::Iterator it = list.begin();
      while( it != list.end() ) {
         QString filename((*it));
         borderFileNames.push_back(filename);
         ++it;
      }
   }
   
   loadBorderFileListBox();
}

/**
 * Load the border file list box
 */
void
GuiAverageBorderDialog::loadBorderFileListBox()
{
   inputListWidget->clear();
   
   for (int i = 0; i < static_cast<int>(borderFileNames.size()); i++) {
      inputListWidget->addItem(FileUtilities::basename(borderFileNames[i]));
   }
}

/**
 * called when remove button is pressed.
 */
void 
GuiAverageBorderDialog::slotRemoveButton()
{
   for (int i = static_cast<int>(borderFileNames.size()) - 1; i >= 0; i--) {
      const QListWidgetItem* item = inputListWidget->item(i);
      if (item != NULL) {
         if (inputListWidget->isItemSelected(item)) {
            borderFileNames.erase(borderFileNames.begin() + i);
         }
      }
   }
      
   loadBorderFileListBox();
}

/**
 * called when apply button is pressed.
 */
void 
GuiAverageBorderDialog::slotApplyButton()
{
   //
   // Verify valid data entered
   //
   if (borderFileNames.empty()) {
      QMessageBox::critical(this, "ERROR", "No input border file are selected.");
      return;
   }
   QString outputFileName(outputBorderFileLineEdit->text());
   if (outputFileName.length() <= 0) {
      QMessageBox::critical(this, "ERROR", "No output border file specified.");
      return;      
   }
   const float resampling = resamplingDoubleSpinBox->value();
   if (resampling <= 0.0) {
      QMessageBox::critical(this, "ERROR", "Resampling must be greater than zero");
      return;
   }
   
   //
   // Read the border files
   //
   std::vector<BorderFile*> borderFiles;
   for (int i = 0; i < static_cast<int>(borderFileNames.size()); i++) {
      BorderFile* b = new BorderFile;
      try {
         b->readFile(borderFileNames[i]);
         borderFiles.push_back(b);
      }
      catch (FileException& e) {
         for (int i = 0; i < static_cast<int>(borderFiles.size()); i++) {
            delete borderFiles[i];
         }
         QMessageBox::critical(this, "File Read Error", e.whatQString());
         return;
      }
   }
   
   
   //
   //
   //
   BorderFile averageBorderFile;
   try {
      BorderFile::createAverageBorderFile(borderFiles,
                                          resampling,
                                          sphereCheckBox->isChecked(),
                                          averageBorderFile);
   }
   catch (FileException& e) {
      QMessageBox::critical(this, "ERROR", e.whatQString());
      return;
   }
   
   //
   // Write the border file
   //
   try {
      //
      // Add the file extension, if needed
      //
      const QString ext(".border");
      if (StringUtilities::endsWith(outputFileName, ext) == false) {
         outputFileName.append(ext);
      }
      averageBorderFile.writeFile(outputFileName);
   }
   catch (FileException& e) {
      QMessageBox::critical(this, "ERROR", e.whatQString());
      return;
   }
   
   //
   // Free memory
   //
   for (int i = 0; i < static_cast<int>(borderFiles.size()); i++) {
      delete borderFiles[i];
   }
   
   //
   // Let the user know the file was created.
   //
   QMessageBox::information(this, "Success", "Average Border File Created");
}

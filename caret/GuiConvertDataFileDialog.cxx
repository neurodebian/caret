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
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>

#include "AbstractFile.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiConvertDataFileDialog.h"
#include "GuiFileDialogWithInstructions.h"
#include "QtUtilities.h"
#include "StringUtilities.h"

/**
 * constructor.
 */
GuiConvertDataFileDialog::GuiConvertDataFileDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setWindowTitle("Convert Data Files");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
  
   //
   // Group box for data files
   //
   QGroupBox* dataFileGroupBox = new QGroupBox("Data Files");
   dialogLayout->addWidget(dataFileGroupBox);
   QVBoxLayout* dataFileLayout = new QVBoxLayout(dataFileGroupBox);
   
   //
   // List box for data files
   //
   dataFilesListBox = new QListWidget;
   dataFilesListBox->setSelectionMode(QListWidget::ExtendedSelection);
   dataFileLayout->addWidget(dataFilesListBox);
   
   //
   // Horizontal box for add/remove buttons
   //
   QHBoxLayout* dataButtonLayout = new QHBoxLayout;
   dataFileLayout->addLayout(dataButtonLayout);
   
   //
   // Add data files push button
   //
   QPushButton* addFilesButton = new QPushButton("Add Files...");
   addFilesButton->setAutoDefault(false);
   QObject::connect(addFilesButton, SIGNAL(clicked()),
                    this, SLOT(slotAddFilesButton()));   
   dataButtonLayout->addWidget(addFilesButton);
   
   //
   // Remove data files push button
   //
   QPushButton* removeFilesButton = new QPushButton("Remove Selected Files");
   removeFilesButton->setAutoDefault(false);
   QObject::connect(removeFilesButton, SIGNAL(clicked()),
                    this, SLOT(slotRemoveFilesButton()));   
   dataButtonLayout->addWidget(removeFilesButton);
   
   //
   // Make add/remove buttons same size
   //
   QtUtilities::makeButtonsSameSize(addFilesButton, removeFilesButton);
   
   //
   // File formats and names
   //
   std::vector<AbstractFile::FILE_FORMAT> fileFormats;
   std::vector<QString> fileFormatNames;
   AbstractFile::getFileFormatTypesAndNames(fileFormats, fileFormatNames);
   const int numberOfFormats = static_cast<int>(fileFormats.size());
   
   //
   // Button group for format buttons
   //
   QGroupBox* formatGroup = new QGroupBox("Convert to Format");
   QGridLayout* formatGridLayout = new QGridLayout(formatGroup);
   dialogLayout->addWidget(formatGroup);
   
   //
   // Create the file format combo boxes
   //
   for (int i = 0; i < numberOfFormats; i++) {
      QLabel* label = new QLabel("");
      if (i == 0) {
         label->setText("Highest Priority");
      }
      else if (i == (numberOfFormats - 1)) {
         label->setText("Lowest Priority");
      }
      QComboBox* comboBox = new QComboBox;
      formatGridLayout->addWidget(label, i, 0);
      formatGridLayout->addWidget(comboBox, i, 1);
      
      for (int j = 0; j < numberOfFormats; j++) {
         comboBox->addItem(fileFormatNames[j],
                           QVariant(static_cast<int>(fileFormats[j])));
      }
      formatComboBoxes.push_back(comboBox);
   }
   
   //
   // Horizontal layout for dialog buttons
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
   // Close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseButton()));
   
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
}

/**
 * destructor.
 */
GuiConvertDataFileDialog::~GuiConvertDataFileDialog()
{
}

/**
 * called when close button is pressed.
 */
void 
GuiConvertDataFileDialog::slotCloseButton()
{
   //
   // Remove all files from dialog
   //
   dataFileNames.clear();
   loadFileNamesIntoListBox();
   
   //
   // Close the dialog
   //
   close();
}
      
/**
 * called when apply button is pressed.
 */
void 
GuiConvertDataFileDialog::slotApplyButton()
{
   if (dataFileNames.empty()) {
      QMessageBox::critical(this, "ERROR", "No data files selected.");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   QString errorMessage;
   int convertedCount = 0;
   
   const int numFiles = static_cast<int>(dataFileNames.size());
   
   QProgressDialog progressDialog(this);
   progressDialog.setMaximum(numFiles + 1);
   progressDialog.setValue(0);
   progressDialog.setWindowTitle("Convert Data Files");
   progressDialog.setLabelText("");
   progressDialog.show();
   
   //
   // Get the desired formats
   //
   std::vector<AbstractFile::FILE_FORMAT> requestedFormats;
   for (unsigned int i = 0; i < formatComboBoxes.size(); i++) {
      const int indx = formatComboBoxes[i]->currentIndex();
      requestedFormats.push_back(static_cast<AbstractFile::FILE_FORMAT>(
                                   formatComboBoxes[i]->itemData(indx).toInt()));
   }
   
   for (int i = 0; i < numFiles; i++) {
      const QString filename(dataFileNames[i]);

      //
      // Update the progress dialog
      //
      qApp->processEvents();  // note: qApp is global in QApplication
      if (progressDialog.wasCanceled()) {
         break;
      }
      progressDialog.setValue(i);
      progressDialog.setLabelText("Checking: " + 
                                  QString(FileUtilities::basename(filename)));
      progressDialog.show();
      
      //
      // Read the header of the file
      //
      AbstractFile* af = AbstractFile::readAnySubClassDataFile(filename, true, errorMessage);

      QString progressLabel(FileUtilities::basename(filename) + " ");
      
      if (af != NULL) {
         
         if (af->getFileHasHeader()) {
            //
            // Get files current encoding
            //
            const QString formatString(af->getHeaderTag(AbstractFile::headerTagEncoding));

            bool validFormatNameFlag = false;
            const AbstractFile::FILE_FORMAT currentFormat = AbstractFile::convertFormatNameToType(formatString,
                                                                  &validFormatNameFlag);
            if (validFormatNameFlag) {
               bool doneFlag = false;
               for (unsigned int j = 0; j < requestedFormats.size(); j++) {
                  const AbstractFile::FILE_FORMAT newFormat = requestedFormats[j];
                  const QString newFormatName(AbstractFile::convertFormatTypeToName(newFormat));
                  if (currentFormat == newFormat) {
                     progressLabel += ("already in "
                                       + newFormatName
                                       + " file format.");
                     doneFlag = true;
                  }
                  else if (af->getCanWrite(newFormat)) {
                     try {
                        af->readFile(filename);
                        af->setFileWriteType(newFormat);
                        af->writeFile(filename);
                        progressLabel += ("converted to "
                                          + newFormatName
                                          + ".");
                        convertedCount++;
                     }
                     catch (FileException& e) {
                        progressLabel += ("error converting or writing.");
                     }
                     doneFlag = true;
                  }
                  
                  if (doneFlag) {
                     break;
                  }
               }
               
               if (doneFlag == false) {
                  progressLabel += ("does not support the requested format(s).");
               }
            }
            else {
               progressLabel += ("is of an unknown format.");
            }
         }
         else {
            progressLabel += ("is of an unknown format (has no header)");
         }

         delete af;  // can't delete ?? compiler bug ??
      }
      else {
         progressLabel += ("read error: "
                           + errorMessage);
      }
      
      progressDialog.setLabelText(progressLabel);
      progressDialog.show();
   }
   
   //
   // Remove the progress dialog
   //
   progressDialog.cancel();
   
   QApplication::restoreOverrideCursor();

   if (errorMessage.isEmpty()) {
      QString msg(StringUtilities::fromNumber(convertedCount));
      msg.append(" files were converted.");
      QMessageBox::information(this, "INFO", msg);
   }
   else {
      QMessageBox::critical(this, "ERROR", errorMessage);
   }
}

/**
 * called when add files button is pressed.
 */
void 
GuiConvertDataFileDialog::slotAddFilesButton()
{
   QString instructions = "To select multiple files, hold down the CTRL key while selecting "
                       "file names with the mouse (Macintosh users should hold down "
                       "the Apple key).";

   GuiFileDialogWithInstructions dwi(this, instructions, "addDataFiles", true);
   QStringList caretFileFilters;
   FileFilters::getAllCaretFileFilters(caretFileFilters);
   const QString allFilesFilter("All Files (*)");
   QStringList filterList;
   filterList.append(allFilesFilter);
   for (QStringList::Iterator it = caretFileFilters.begin(); it != caretFileFilters.end(); ++it) {
      filterList.append(*it);
   }
   dwi.setFilters(filterList);
   dwi.selectFilter(allFilesFilter);
   dwi.setWindowTitle("Choose Data File");
   dwi.setFileMode(GuiFileDialogWithInstructions::ExistingFiles);
   if (dwi.exec() == QDialog::Accepted) {
      QStringList list = dwi.selectedFiles();
      QStringList::Iterator it = list.begin();
      while( it != list.end() ) {
         QString filename((*it));
         dataFileNames.push_back(filename);
         ++it;
      }
      loadFileNamesIntoListBox();
   }
}

/**
 * called when remove files button is pressed.
 */
void 
GuiConvertDataFileDialog::slotRemoveFilesButton()
{
   std::vector<QString> names;
   
   for (int i = 0; i < static_cast<int>(dataFileNames.size()); i++) {
      QListWidgetItem* lwi = dataFilesListBox->item(i);
      if (dataFilesListBox->isItemSelected(lwi)) {
         names.push_back(dataFileNames[i]);
      }
   }
   dataFileNames = names;
   
   loadFileNamesIntoListBox();
}

/**
 * load file names into list box.
 */
void 
GuiConvertDataFileDialog::loadFileNamesIntoListBox()
{
   dataFilesListBox->clear();
   for (int i = 0; i < static_cast<int>(dataFileNames.size()); i++) {
      dataFilesListBox->addItem(FileUtilities::basename(dataFileNames[i]));
   }
}


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
#include <QButtonGroup>
#include <QGroupBox>
#include <QLayout>
#include <QListWidget>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>

#include "AbstractFile.h"
#include "GuiDataFileDialog.h"
#include "FileUtilities.h"
#include "GuiConvertDataFileDialog.h"
#include "GuiFileDialogWithInstructions.h"
#include "GuiMessageBox.h"
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
   // Button group for format buttons
   //
   QGroupBox* formatGroup = new QGroupBox("Convert to Format");
   QVBoxLayout* formatLayout = new QVBoxLayout(formatGroup);
   QButtonGroup* formatButtonGroup = new QButtonGroup(this);
   dialogLayout->addWidget(formatGroup);
   
   //
   // binary format radio button
   //
   formatBinaryRadioButton = new QRadioButton("Binary");
   formatLayout->addWidget(formatBinaryRadioButton);
   formatButtonGroup->addButton(formatBinaryRadioButton, 0);
   
   // text format radio button
   //
   formatTextRadioButton = new QRadioButton("Text");
   formatLayout->addWidget(formatTextRadioButton);
   formatButtonGroup->addButton(formatTextRadioButton, 1);
   
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
      GuiMessageBox::critical(this, "ERROR", "No data files selected.", "OK");
      return;
   }
   
   const bool doText   = formatTextRadioButton->isChecked();
   const bool doBinary = formatBinaryRadioButton->isChecked();
   const bool doXML    = false;
   
   if ((doText == false) && (doBinary == false) && (doXML == false)) {
      GuiMessageBox::critical(this, "ERROR", "Select conversion format.", "OK");
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

      if (af != NULL) {
         if (af->getFileHasHeader()) {
            //
            // Get files current encoding
            //
            const QString formatString(af->getHeaderTag(AbstractFile::headerTagEncoding));

            //
            // Format to convert file into
            //
            AbstractFile::FILE_FORMAT outputFormat = 
                              AbstractFile::FILE_FORMAT_OTHER;
                            
            //
            // Are text files desired
            //
            if (doText) {
               //
               // Are these files not in text format but support text format
               //
               if ((formatString != AbstractFile::headerTagEncodingValueAscii) &&
                   (af->getCanWrite(AbstractFile::FILE_FORMAT_ASCII))) {
                  outputFormat = AbstractFile::FILE_FORMAT_ASCII;
               }
            }
            else if (doBinary) {
               //
               // Are these files not in binary format but support binary format
               //
               if ((formatString != AbstractFile::headerTagEncodingValueBinary) &&
                   (af->getCanWrite(AbstractFile::FILE_FORMAT_BINARY))) {
                  outputFormat = AbstractFile::FILE_FORMAT_BINARY;
               }
            }
            else if (doXML) {
               //
               // Are these files not in XML format but support XML format
               //
               if ((formatString != AbstractFile::headerTagEncodingValueXML) &&
                   (af->getCanWrite(AbstractFile::FILE_FORMAT_XML))) {
                  outputFormat = AbstractFile::FILE_FORMAT_XML;
               }
            }
            
            if (outputFormat != AbstractFile::FILE_FORMAT_OTHER) {
               progressDialog.setLabelText("Converting: " + 
                                           QString(FileUtilities::basename(filename)));
         
                     try {
                  af->readFile(filename);
                  af->setFileWriteType(outputFormat);
                  af->writeFile(filename);
                  convertedCount++;
               }
               catch (FileException& e) {
                  errorMessage.append(FileUtilities::basename(filename));
                  errorMessage.append(": error converting or writing.\n");
               }
            }
         }
         delete af;  // can't delete ?? compiler bug ??
      }
   }
   
   //
   // Remove the progress dialog
   //
   progressDialog.cancel();
   
   QApplication::restoreOverrideCursor();

   if (errorMessage.isEmpty()) {
      QString msg(StringUtilities::fromNumber(convertedCount));
      msg.append(" files were converted.");
      GuiMessageBox::information(this, "INFO", msg, "OK");
   }
   else {
      GuiMessageBox::critical(this, "ERROR", errorMessage, "OK");
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
   GuiDataFileDialog::getCaretFileFilters(caretFileFilters);
   const QString allFilesFilter("All Files (*)");
   dwi.setFilter(allFilesFilter);
   for (QStringList::Iterator it = caretFileFilters.begin(); it != caretFileFilters.end(); ++it) {
      dwi.addFilter(*it);
   }
   dwi.setFilter(allFilesFilter);
   dwi.setWindowTitle("Choose Data File");
   dwi.setMode(GuiFileDialogWithInstructions::ExistingFiles);
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


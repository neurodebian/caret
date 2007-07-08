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
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QToolTip>
#include <QButtonGroup>

#include "DebugControl.h"
#include "FileUtilities.h"
#include "GuiChooseSpecFileDialog.h"
#include "GuiCopySpecFileDialog.h"
#include "GuiMessageBox.h"
#include "PreferencesFile.h"
#include "QtUtilities.h"
#include "SpecFile.h"
#include "SpecFileUtilities.h"

/**
 * constructor.
 */
GuiCopySpecFileDialog::GuiCopySpecFileDialog(QWidget* parent, PreferencesFile* pref)
   : QtDialog(parent, false)
{
   preferencesFile = pref;
   
   const int minimumLineEditWidth = 400;
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);

   //
   // Copy spec button and line edit
   //
   QPushButton* copySpecFilePushButton = new QPushButton("Copy This Spec File...");
   copySpecFilePushButton->setAutoDefault(false);
   copySpecFilePushButton->setToolTip(
                 "Press this button to display a\n"
                 "dialog for selecting the spec\n"
                 "file that is to be copied.");
   QObject::connect(copySpecFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotCopySpecFilePushButton()));
   copySpecFileLineEdit = new QLineEdit;
   copySpecFileLineEdit->setReadOnly(true);
   copySpecFileLineEdit->setMinimumWidth(minimumLineEditWidth);
   
   //
   // Copy into directory button and line edit
   //
   QPushButton* copyIntoDirectoryPushButton = new QPushButton("Into This Directory...");
   copyIntoDirectoryPushButton->setAutoDefault(false);
   copyIntoDirectoryPushButton->setToolTip(
                 "Press this button to display a\n"
                 "dialog for choosing the directory in\n"
                 "which to place the copied spec file.");
   QObject::connect(copyIntoDirectoryPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCopyIntoDirectoryPushButton()));
   copyIntoDirectoryLineEdit = new QLineEdit;
   copyIntoDirectoryLineEdit->setReadOnly(false);
   copyIntoDirectoryLineEdit->setText(QDir::currentPath());
   copyIntoDirectoryLineEdit->setToolTip(
                 "Enter or edit the directory in which to place\n"
                 "the copied spec file.  If the directory does\n"
                 "not exist, it will be created.");
   copyIntoDirectoryLineEdit->setMinimumWidth(minimumLineEditWidth);
   
   //
   // Previous directories list
   //
   QLabel* previousDirectoryLabel = new QLabel("Previous Directories");
   QComboBox* previousDirectoryComboBox = new QComboBox;
   previousDirectoryComboBox->setToolTip("The directory selected using this\n"
                                         "control will be copied into the \n"
                                         "\"Into This Directory\" line edit above.");
   preferencesFile->getRecentDataFileDirectories(previousDirectories);
   for (int i = 0; i < static_cast<int>(previousDirectories.size()); i++) {
      QString dirName(previousDirectories[i]);
      const int dirNameLength = static_cast<int>(dirName.length());
      if (dirNameLength > 50) {
         QString s("...");
         s.append(dirName.mid(dirNameLength - 50));
         dirName = s;
      }
      previousDirectoryComboBox->addItem(dirName);
   }
   QObject::connect(previousDirectoryComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotPreviousDirectory(int)));
   
   //
   // New spec file label and name line edit
   //
   QLabel* newNameLabel = new QLabel("Name of New Spec File");
   newNameLabel->setFont(copyIntoDirectoryPushButton->font());
   newSpecFileNameLineEdit = new QLineEdit;
   newSpecFileNameLineEdit->setMinimumWidth(minimumLineEditWidth);
   
   //
   // Group Box for file items
   //   
   QGroupBox* fileGroupBox = new QGroupBox("Spec File");
   dialogLayout->addWidget(fileGroupBox);
   QGridLayout* fileGroupGrid = new QGridLayout(fileGroupBox);
   fileGroupGrid->addWidget(copySpecFilePushButton, 0, 0);
   fileGroupGrid->addWidget(copySpecFileLineEdit, 0, 1);
   fileGroupGrid->addWidget(copyIntoDirectoryPushButton, 1, 0);
   fileGroupGrid->addWidget(copyIntoDirectoryLineEdit, 1, 1);
   fileGroupGrid->addWidget(previousDirectoryLabel, 2, 0);
   fileGroupGrid->addWidget(previousDirectoryComboBox, 2, 1);
   fileGroupGrid->addWidget(newNameLabel, 3, 0);
   fileGroupGrid->addWidget(newSpecFileNameLineEdit, 3, 1);

   //
   // Copy data files push button
   //
   copyDataFilesRadioButton = new QRadioButton("Copy Data Files"); 
   copyDataFilesRadioButton->setToolTip(
                 "If this option is selected, the data\n"
                 "data files associated with the spec\n"
                 "file will also be copied.");

   //
   // Point to data files abs path push button
   //
   pointToDataFilesAbsPathRadioButton = new QRadioButton("Point To Data Files (Absolute Path)");
   pointToDataFilesAbsPathRadioButton->setToolTip(
                 "If this option is selected, the data\n"
                 "data files associated with the spec\n"
                 "file will NOT be copied.  When the\n"
                 "spec file is loaded, the files will\n"
                 "be read from their original location.\n"
                 "Each data file will be specified with\n"
                 "an absolute path.");
                                               
   //
   // Point to data files relative path push button
   //
   pointToDataFilesRelPathRadioButton = new QRadioButton("Point To Data Files (Relative Path)");
   pointToDataFilesRelPathRadioButton->setToolTip(
                 "If this option is selected, the data\n"
                 "data files associated with the spec\n"
                 "file will NOT be copied.  When the\n"
                 "spec file is loaded, the files will\n"
                 "be read from their original location.\n"
                 "Each data file will be specified with\n"
                 "a relative path.");
                                               
   //
   // Button group for data file copying
   //
   QGroupBox* dataFileButtonGroupBox = new QGroupBox("Data File Copying");
   dialogLayout->addWidget(dataFileButtonGroupBox);
   QVBoxLayout* dataFileLayout = new QVBoxLayout(dataFileButtonGroupBox);
   QButtonGroup* dataFileButtonGroup = new QButtonGroup(this);
   dataFileLayout->addWidget(copyDataFilesRadioButton);
   dataFileButtonGroup->addButton(copyDataFilesRadioButton);
   dataFileLayout->addWidget(pointToDataFilesAbsPathRadioButton);
   dataFileButtonGroup->addButton(pointToDataFilesAbsPathRadioButton);
   dataFileLayout->addWidget(pointToDataFilesRelPathRadioButton);
   dataFileButtonGroup->addButton(pointToDataFilesRelPathRadioButton);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
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
                    this, SLOT(close()));

   QtUtilities::makeButtonsSameSize(applyButton, closeButton);

}

/**
 * destructor.
 */
GuiCopySpecFileDialog::~GuiCopySpecFileDialog()
{
}

/**
 * called when a previous directory is selected.
 */
void 
GuiCopySpecFileDialog::slotPreviousDirectory(int item)
{
   if ((item >= 0) && (item < static_cast<int>(previousDirectories.size()))) {
      copyIntoDirectoryLineEdit->setText(previousDirectories[item]);
   }
}
      
/**
 * called when apply button is pressed.
 */
void 
GuiCopySpecFileDialog::slotApplyButton()
{
   //
   // Check spec file that is to be copied
   //
   QString specFileToCopy(copySpecFileLineEdit->text());
   if (specFileToCopy.isEmpty()) {
      GuiMessageBox::critical(this, "ERROR", "Name of spec file for copying is empty.", "OK");
      return;
   }
   if (QFile::exists(specFileToCopy) == false) {
      GuiMessageBox::critical(this, "ERROR", "Name of spec file for copying does not exist.", "OK");
      return;
   }
 
   //
   // Verify that the data files exist and are readable
   //
   try {
      SpecFile sf;
      sf.readFile(specFileToCopy);
      QString msg1;
      if (sf.validate(msg1) == false) {
         QString msg("Missing data files so unable to copy ");
         msg.append(FileUtilities::basename(specFileToCopy));
         msg.append(":\n");
         msg.append(msg1);
         GuiMessageBox::critical(this, "ERROR", msg, "OK");
         return;
      }
   }
   catch (FileException&) {
      QString msg("Unable to read: ");
      msg.append(FileUtilities::basename(specFileToCopy));
      GuiMessageBox::critical(this, "ERROR", msg, "OK");
      return;
   }

   //
   // Check destination directory
   //
   const QString directory(copyIntoDirectoryLineEdit->text());
   if (directory.isEmpty()) {
      GuiMessageBox::critical(this, "ERROR", "Name of directory is empty.", "OK");
      return;
   }
   
   //
   // Check output file name
   //
   QString outputFileName(newSpecFileNameLineEdit->text());
   if (outputFileName.isEmpty()) {
      GuiMessageBox::critical(this, "ERROR", "Name of new spec file is empty.", "OK");
      return;
   }
   
   //
   // Check data file selection
   //
   if ((copyDataFilesRadioButton->isChecked() == false) &&
       (pointToDataFilesAbsPathRadioButton->isChecked() == false) &&
       (pointToDataFilesRelPathRadioButton->isChecked() == false)) {
      GuiMessageBox::critical(this, "ERROR", "There is no data file copying selection.", "OK");
      return;
   }
   
   //
   // Create the output directory if needed
   //
   if (QFile::exists(directory) == false) {
      FileUtilities::createDirectory(directory);
      //
      // Make sure output directory was successfully created
      //
      if (QFile::exists(directory) == false) {
         GuiMessageBox::critical(this, "ERROR", "Unable to create output directory.", "OK");
         return;
      }
   }

   //
   // Switch to the output directory
   //
   const QString savedDirectory(QDir::currentPath());
   QDir::setCurrent(directory);
   
/*   
   //
   // Create the output spec file
   //
   QString outputSpecFileName(directory);
   outputSpecFileName.append("/"); // FileUtilities::directorySeparator());
   outputSpecFileName.append(outputFileName);
*/
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // Set the spec file copy mode
   //
   SpecFileUtilities::MODE_COPY_SPEC_FILE copySpecMode =
                           SpecFileUtilities::MODE_COPY_SPEC_FILE_POINT_TO_DATA_FILES;
   if (copyDataFilesRadioButton->isChecked()) {
      copySpecMode = SpecFileUtilities::MODE_COPY_SPEC_FILE_COPY_ALL;
   }
   
   //
   // Create the output spec file with path and name
   //
   QString outputSpecFileName(directory);
   outputSpecFileName.append("/");  //FileUtilities::directorySeparator());
   outputSpecFileName.append(outputFileName);
   
   //
   // If relative paths are desired
   //
   if (pointToDataFilesRelPathRadioButton->isChecked()) {
      //
      // If input path is absolute
      //
      QFileInfo fi(specFileToCopy);
      if (fi.isRelative() == false) {
         const QString inputPath(FileUtilities::dirname(specFileToCopy));
         if ((inputPath != ".") && (inputPath == directory)) {
            specFileToCopy = FileUtilities::basename(specFileToCopy);
         }
         else {
            //
            // Convert input spec file to have a relative path
            //
            QString relativeDirectory;
            FileUtilities::relativePath(inputPath,
                                        directory,
                                        relativeDirectory);
            if (DebugControl::getDebugOn()) {
               std::cout << "Copy Path: " << inputPath.toAscii().constData() << std::endl;
               std::cout << "Output Path: " << directory.toAscii().constData() << std::endl;
               std::cout << "Relative Path: " << relativeDirectory.toAscii().constData() << std::endl;
            }
            if (relativeDirectory.isEmpty() == false) {
               QString newPath(relativeDirectory);
               newPath.append("/");  //FileUtilities::directorySeparator());
               newPath.append(FileUtilities::basename(specFileToCopy));
               specFileToCopy = newPath;
            }
         }
      }
   }
   
   //
   // See if the output spec file exists
   //
   if (QFile::exists(outputSpecFileName)) {
      QString msg("The output spec file ");
      msg.append(outputFileName);
      msg.append(" exists.\n");
      msg.append("Press \"Continue\" to overwrite it, else press \"Cancel\"");
      if (GuiMessageBox::question(this, "Overwrite Spec File", msg, 
                                  "Continue", "Cancel") != 0) {
         return;
      }
   }
   
   //
   // copy the spec file
   //
   QString errorMessage;
   const bool error = SpecFileUtilities::copySpecFile(specFileToCopy,
                                       outputFileName,
                                       copySpecMode,
                                       errorMessage,
                                       false,
                                       false);
                                       
   //
   // Restore current directory
   //
   QDir::setCurrent(savedDirectory);
   
   //
   // Did the copy fail ?
   //
   if (error) {
      GuiMessageBox::critical(this, "ERROR", errorMessage, "OK");
      return;
   }
   else {
      GuiMessageBox::information(this, "SUCCESS", "Spec file has been copied.", "OK");
   }
           
   //
   // Add to previous spec files
   //
   if (preferencesFile != NULL) {
      preferencesFile->addToRecentSpecFiles(outputSpecFileName, true);
   }
   
   QApplication::restoreOverrideCursor();
}

/**
 * called when copy spec file button is pressed.
 */
void 
GuiCopySpecFileDialog::slotCopySpecFilePushButton()
{
   //
   // Choose the spec file to be copied
   //
   GuiChooseSpecFileDialog csfd(this,
                                preferencesFile,
                                true);
   if (csfd.exec() == GuiChooseSpecFileDialog::Accepted) {
      copySpecFileLineEdit->setText(csfd.getSelectedSpecFile());
      newSpecFileNameLineEdit->setText(FileUtilities::basename(csfd.getSelectedSpecFile()));
   }
}

/**
 * called when copy into directory button is pressed.
 */
void 
GuiCopySpecFileDialog::slotCopyIntoDirectoryPushButton()
{
   QFileDialog fd(this);
   fd.setModal(true);
   fd.setAcceptMode(QFileDialog::AcceptOpen);
   fd.setWindowTitle("Choose Output Directory");
   fd.setDirectory(QDir::currentPath());
   fd.setFileMode(QFileDialog::DirectoryOnly);
   if (fd.exec() == QFileDialog::Accepted) {
      copyIntoDirectoryLineEdit->setText(fd.selectedFiles().at(0));
   }
}

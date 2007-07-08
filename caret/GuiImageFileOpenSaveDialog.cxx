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
#include <QComboBox>
#include <QDir>
#include <QGroupBox>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QLayout>

#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiSpecFileCreationDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "ImageFile.h"
#include "PreferencesFile.h"
#include "QtUtilities.h"
#include "StringUtilities.h"
#include "global_variables.h"


#define __GUI_IMAGE_FILE_OPEN_SAVE_DIALOG_MAIN__
#include "GuiImageFileOpenSaveDialog.h"
#undef __GUI_IMAGE_FILE_OPEN_SAVE_DIALOG_MAIN__

/**
 * constructor.
 */
GuiImageFileOpenSaveDialog::GuiImageFileOpenSaveDialog(QWidget* parent, 
                                            const DIALOG_MODE dialogModeIn)
   : Q3FileDialog(parent, "GuiImageFileOpenSaveDialog", false)
{
   defaultImageFileForSaving = NULL;
   
   dialogMode = dialogModeIn;
   
   //
   // Create the file filters and file extensions and find the jpeg and ppm filters
   //
   QStringList fileFilterList;
   QString jpegFileFilter;
   QString jpgFileFilter;
   QString ppmFileFilter;
   for (int i = 0; i < QImageWriter::supportedImageFormats().count(); i++) {
      QString str = QString(QImageWriter::supportedImageFormats().at(i));
      
      QString filter = QString("%1 Image File (*.%2)").arg(str).arg(
                                        StringUtilities::makeLowerCase(str));
      fileExtensions.push_back(StringUtilities::makeLowerCase(str));
      if ((filter == "PPM") ||
          (filter == "ppm")) {
         ppmFileFilter = filter;
      }
      if (str == "jpg") {
         jpgFileFilter = filter;
      }
      if (str == "jpeg") {
         jpegFileFilter = filter;
      }
      fileFilterList << filter;
      fileFilters.push_back(filter);
      fileFormats.push_back(str);
   }
   
   //
   // Set the file filters
   //
   setFilters(fileFilterList);
   if (jpgFileFilter.isEmpty() == false) {
      setSelectedFilter(jpgFileFilter);
   }
   else if (jpegFileFilter.isEmpty() == false) {
      setSelectedFilter(jpegFileFilter);
   }
   else if (ppmFileFilter.isEmpty() == false) {
      setSelectedFilter(ppmFileFilter);
   }

   setDir(QDir::currentPath());

   switch (dialogMode) {
      case DIALOG_MODE_OPEN_FILES:
         setMode(ExistingFiles);
         setWindowTitle("Open Image File");
         break;
      case DIALOG_MODE_SAVE_LOADED_IMAGE:
         setMode(AnyFile);
         setWindowTitle("Save Image File");
         break;
      case DIALOG_MODE_SAVE_IMAGE_OF_MAIN_WINDOW:
         setMode(AnyFile);
         setWindowTitle("Save Image File");
         break;
      case DIALOG_NUMBER_OF_MODES:
         break;
   }

   //
   // Box for additional widgets
   //
   QWidget* caretWidget = createCaretUniqueSection();
   addWidgets(NULL, caretWidget, NULL);   

   QtUtilities::setMaximumHeightToNinetyPercentOfScreenHeight(this);
}

/**
 * destructor.
 */
GuiImageFileOpenSaveDialog::~GuiImageFileOpenSaveDialog()
{
}

/**
 * set default image for saving.
 */
void 
GuiImageFileOpenSaveDialog::setDefaultImageForSaving(ImageFile* defaultImageFileForSavingIn)
{
  defaultImageFileForSaving = defaultImageFileForSavingIn;
}
      
/**
 * show the dialog.
 */
void 
GuiImageFileOpenSaveDialog::show()
{
   setDir(QDir::currentPath());
   updateDialog();

   Q3FileDialog::show();
}

/**
 * update the dialog.
 */
void 
GuiImageFileOpenSaveDialog::updateDialog()
{
   //
   // Update previous directories
   //
   previousDirectoryComboBox->clear();
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   pf->getRecentDataFileDirectories(previousDirectories);
   
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
   
   //
   // Update image selections
   //
   if (imageSelectionForSavingComboBox != NULL) {
      QString currentName;
      if (defaultImageFileForSaving == NULL) {
         currentName = imageSelectionForSavingComboBox->currentText();
      }
      imageSelectionForSavingComboBox->clear();
      const int num = theMainWindow->getBrainSet()->getNumberOfImageFiles();
      for (int i = 0; i < num; i++) {
         const ImageFile* imageFile = theMainWindow->getBrainSet()->getImageFile(i);
         QString name(FileUtilities::basename(imageFile->getFileName()));
         imageSelectionForSavingComboBox->addItem(name);
         if (i == 0) {
            setSelection(name);
         }
         if (imageFile == defaultImageFileForSaving) {
            setSelection(name);
            imageSelectionForSavingComboBox->setCurrentIndex(i);
            defaultImageFileForSaving = NULL;
         }
         if (currentName.isEmpty()) {
            if (name == currentName) {
               setSelection(name);
               imageSelectionForSavingComboBox->setCurrentIndex(i);
               currentName = "";
            }
         }
      }
   }
}

/**
 * create the caret unique section.
 */
QWidget* 
GuiImageFileOpenSaveDialog::createCaretUniqueSection()
{
   //--------------------------------------------------------------------------
   //
   // Previous directories combo box
   //
   previousDirectoryComboBox = new QComboBox;
   QObject::connect(previousDirectoryComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotPreviousDirectory(int)));

   //
   // Previous directories group box
   //
   QGroupBox* prevDirGroupBox = new QGroupBox("Previous Directories");
   QVBoxLayout* prevDirGroupLayout = new QVBoxLayout(prevDirGroupBox);
   prevDirGroupLayout->addWidget(previousDirectoryComboBox);
   
   //
   // Image saving/opening options
   //
   QString additionalOptionsTitle;
   QString additionalLabel;
   bool showAppendOption = false;
   switch (dialogMode) {
      case DIALOG_MODE_OPEN_FILES:
         additionalOptionsTitle = "Open Image Options";
         additionalLabel = "Multiple image files may be opened by holding\n"
                                "down the CTRL key (Apple key on Macintoshes)\n"
                                "when selecting files with the mouse.";
         showAppendOption = true;
         break;
      case DIALOG_MODE_SAVE_LOADED_IMAGE:
         additionalOptionsTitle = "Save Image Options";
         break;
      case DIALOG_MODE_SAVE_IMAGE_OF_MAIN_WINDOW:
         additionalOptionsTitle = "Save Image Options";
         break;
      case DIALOG_NUMBER_OF_MODES:
         break;
   }

   //
   // Add optional label
   //
   QLabel* additionalOptionsLabel = NULL;
   if (additionalLabel.isEmpty() == false) {
      additionalOptionsLabel = new QLabel(additionalLabel);
   }
   
   //
   // Add to spec file check box
   //
   addToSpecFileCheckBox = new QCheckBox("Add to Specification File");
   addToSpecFileCheckBox->setChecked(true);
   
   //
   // Append option
   //
   appendToExistingFilesCheckBox = NULL;
   if (showAppendOption) {
      appendToExistingFilesCheckBox = new QCheckBox("Append To Loaded Image");
      appendToExistingFilesCheckBox->setChecked(true);
   }
   
   imageSelectionForSavingComboBox = NULL;
   
   QHBoxLayout* imageSelectionLayout = NULL;
   switch (dialogMode) {
      case DIALOG_MODE_OPEN_FILES:
         break;
      case DIALOG_MODE_SAVE_LOADED_IMAGE:
         {
            QLabel* imageLabel = new QLabel("Image ");
            imageSelectionForSavingComboBox = new QComboBox;
            QObject::connect(imageSelectionForSavingComboBox, SIGNAL(activated(int)),
                             this, SLOT(slotImageSelectionForSavingComboBox(int)));
            imageSelectionLayout = new QHBoxLayout;
            imageSelectionLayout->addWidget(imageLabel);
            imageSelectionLayout->addWidget(imageSelectionForSavingComboBox);
            imageSelectionLayout->setStretchFactor(imageLabel, 0);
            imageSelectionLayout->setStretchFactor(imageSelectionForSavingComboBox, 10);
         }
         break;
      case DIALOG_MODE_SAVE_IMAGE_OF_MAIN_WINDOW:
         break;
      case DIALOG_NUMBER_OF_MODES:
         break;
   }
   
   //
   // additional options group box
   //
   QGroupBox* optionsGroupBox = new QGroupBox(additionalOptionsTitle);
   QVBoxLayout* optionsGroupLayout = new QVBoxLayout(optionsGroupBox);
   if (additionalOptionsLabel != NULL) {
      optionsGroupLayout->addWidget(additionalOptionsLabel);
   }
   optionsGroupLayout->addWidget(addToSpecFileCheckBox);
   if (appendToExistingFilesCheckBox != NULL) {
      optionsGroupLayout->addWidget(appendToExistingFilesCheckBox);
   }
   if (imageSelectionLayout != NULL) {
      optionsGroupLayout->addLayout(imageSelectionLayout);
   }
   
   //--------------------------------------------------------------------------
   //
   // Caret widgets and layout
   //
   QWidget* caretWidget = new QWidget;
   QVBoxLayout* caretWidgetLayout = new QVBoxLayout(caretWidget);
   caretWidgetLayout->addWidget(prevDirGroupBox);
   caretWidgetLayout->addWidget(optionsGroupBox);
   
   return caretWidget;
}

/**
 * called when an image file is selected for saving.
 */
void 
GuiImageFileOpenSaveDialog::slotImageSelectionForSavingComboBox(int item)
{
   if ((item >= 0) && (item < theMainWindow->getBrainSet()->getNumberOfImageFiles())) {
     setSelection(FileUtilities::basename(theMainWindow->getBrainSet()->getImageFile(item)->getFileName()));
   }
}
      
/**
 * called when a previous directory is selected.
 */
void 
GuiImageFileOpenSaveDialog::slotPreviousDirectory(int item)
{
   if ((item >= 0) && (item < static_cast<int>(previousDirectories.size()))) {
      setDir(previousDirectories[item]);
   }
}      

/**
 * called when Open/Save/Cancel button pressed.
 */
void 
GuiImageFileOpenSaveDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      //
      // Find the file filter
      //
      int fileFilterIndex = -1;
      for (unsigned int i = 0; i < fileFilters.size(); i++) {
         if (fileFilters[i] == selectedFilter()) {
            fileFilterIndex = i;
            break;
         }
      }
      
      //
      // See if invalid filter index
      //
      if (fileFilterIndex < 0) {
         QString msg("PROGRAM ERROR: invalid file filter index when saving image file.");
         GuiMessageBox::critical(this, "PROGRAM ERROR", msg, "OK");
         return;
      }
      
      QStringList fileNameList;
      switch (dialogMode) {
         case DIALOG_MODE_OPEN_FILES:
            fileNameList = selectedFiles();
            break;
         case DIALOG_MODE_SAVE_LOADED_IMAGE:
            fileNameList += selectedFile();
            break;
         case DIALOG_MODE_SAVE_IMAGE_OF_MAIN_WINDOW:
            fileNameList += selectedFiles();
            break;
         case DIALOG_NUMBER_OF_MODES:
            break;
      }

      if (fileNameList.empty()) {
         GuiMessageBox::critical(this, "ERROR", "No files are selected.", "OK");
      }
      
      //
      // Get append to spec
      //
      bool addToSpecFileFlag = false;
      if (addToSpecFileCheckBox != NULL) {
         addToSpecFileFlag = addToSpecFileCheckBox->isChecked();
      }
      
      //
      // Get add to existing files
      //
      bool addToExistingFilesFlag = false;
      if (appendToExistingFilesCheckBox != NULL) {
         addToExistingFilesFlag = appendToExistingFilesCheckBox->isChecked();
      }
      
      //
      // Allow user to set the spec file name if not already set
      //
      if (addToSpecFileFlag && 
          theMainWindow->getBrainSet()->getSpecFileName().isEmpty()) {
         const int result = GuiMessageBox::question(this, "Spec File", 
                                      "Would you like to create a Spec File ?",
                                      "Yes", "No", "Cancel");
         if (result == 0) {
            //
            // Create spec file dialog will set the directory and create the spec file
            //
            GuiSpecFileCreationDialog sfcd(this);
            sfcd.exec();
            PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
            pf->addToRecentSpecFiles(sfcd.getCreatedSpecFileName(), true);
         }
         else if (result == 1) {
         }
         else if (result == 2) {
            return;
         }
      }
      
      QString errorMessage;
      
      QStringList::Iterator fileNameIterator = fileNameList.begin();      
      while (fileNameIterator != fileNameList.end()) {
         const QString qname(*fileNameIterator);
         QString name(qname);
      
         switch (dialogMode) {
            case DIALOG_MODE_OPEN_FILES:
               break;
            case DIALOG_MODE_SAVE_LOADED_IMAGE:
               if (FileUtilities::filenameExtension(name) != fileExtensions[fileFilterIndex]) {
                  name.append(".");
                  name.append(fileExtensions[fileFilterIndex]);
               }
               break;
            case DIALOG_MODE_SAVE_IMAGE_OF_MAIN_WINDOW:
               if (FileUtilities::filenameExtension(name) != fileExtensions[fileFilterIndex]) {
                  name.append(".");
                  name.append(fileExtensions[fileFilterIndex]);
               }
               break;
            case DIALOG_NUMBER_OF_MODES:
               break;
         }
         
         switch (dialogMode) {
            case DIALOG_MODE_OPEN_FILES:
               {
                  try {
                     theMainWindow->getBrainSet()->readImageFile(name, 
                                                                 addToExistingFilesFlag, 
                                                                 addToSpecFileFlag);
                  }
                  catch (FileException& e) {
                     errorMessage.append(e.whatQString());
                     errorMessage.append("\n");
                  }
               }
               break;
            case DIALOG_MODE_SAVE_LOADED_IMAGE:
               try {
                  const int imageNum = imageSelectionForSavingComboBox->currentIndex();
                  if ((imageNum >= 0) && (imageNum < theMainWindow->getBrainSet()->getNumberOfImageFiles())) {
                     theMainWindow->getBrainSet()->writeImageFile(name,
                                           theMainWindow->getBrainSet()->getImageFile(imageNum));
                  }
               }
               catch (FileException& e) {
                     errorMessage.append(e.whatQString());
                     errorMessage.append("\n");
               }
               break;
            case DIALOG_MODE_SAVE_IMAGE_OF_MAIN_WINDOW:
               {
                  GuiBrainModelOpenGL* brainModelOpenGL = theMainWindow->getBrainModelOpenGL();
                  QString format;
                  if (brainModelOpenGL->saveImage(name, 
                                                  fileFormats[fileFilterIndex],
                                                  addToSpecFileFlag)) {
                     errorMessage.append("Unable to write ");
                     errorMessage.append(FileUtilities::basename(name));
                     errorMessage.append("\n");
                  }
               }
            case DIALOG_NUMBER_OF_MODES:
               break;
         }
         
         fileNameIterator++;
      }

      //
      // Files have changed
      //
      GuiFilesModified fm;
      fm.setImagesModified();
      theMainWindow->fileModificationUpdate(fm);
      
      //
      // display graphics windows.
      //
      GuiBrainModelOpenGL::updateAllGL();
      
      //
      // Was there an error ?
      //
      if (errorMessage.isEmpty() == false) {
         GuiMessageBox::warning(this, "ERROR", errorMessage, "OK");
         return;
      }
      
      //
      // Emit a signal indicating that an image has been saved
      //
      emit signalImageHasBeenSaved();
   }
   
   Q3FileDialog::done(r);
}
      

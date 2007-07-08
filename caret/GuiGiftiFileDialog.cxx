
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

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiDataFileDialog.h"
#include "GuiGiftiFileDialog.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "SpecFile.h"
#include "SurfaceShapeFile.h"
#include "global_variables.h"
#include "QtRadioButtonSelectionDialog.h"
/**
 * constructor.
 */
GuiGiftiFileDialog::GuiGiftiFileDialog(QWidget* parent,
                                       const DIALOG_MODE modeIn)
   : QFileDialog(parent)
{
   //
   // set dialog non-modal
   //
   setModal(false);
   
   //
   // Set to the current directory
   //
   setDirectory(QDir::currentPath());
   
   //
   // Create the file filters
   //
   QStringList filterTypes;
   filterTypes << GuiDataFileDialog::giftiFunctionalFileFilter
           << GuiDataFileDialog::giftiLabelFileFilter
           << GuiDataFileDialog::giftiShapeFileFilter;

   //
   // Set up the dialog based upon the mode
   // 
   dialogMode = modeIn;
   switch (dialogMode) {
      case DIALOG_MODE_FILE_OPEN:
         setAcceptMode(AcceptOpen);
         setFileMode(ExistingFiles);
         setWindowTitle("Open GIFTI Files");
         filterTypes << GuiDataFileDialog::giftiSurfaceFileFilter;
         break;
      case DIALOG_MODE_FILE_SAVE:
         setAcceptMode(AcceptSave);
         setFileMode(AnyFile);
         setWindowTitle("Save GIFTI Files");
         if (theMainWindow->getBrainModelSurface() != NULL) {
            filterTypes << GuiDataFileDialog::giftiSurfaceFileFilter;
         }
         break;
   }
   
   //
   // Set the file filters
   //
   setFilters(filterTypes);
}
                   
/**
 * destructor.
 */
GuiGiftiFileDialog::~GuiGiftiFileDialog()
{
}

/**
 * called when open/save/cancel pushbutton pressed.
 */
void 
GuiGiftiFileDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      switch (dialogMode) {
         case DIALOG_MODE_FILE_OPEN:
            if (openFile() == false) {
               return;
            }
            break;
         case DIALOG_MODE_FILE_SAVE:
            if (saveFile() == false) {
               return;
            }
            break;
      }
   }
   
   QDialog::done(r);
}

/**
 * save the file and return true if successful.
 */
bool 
GuiGiftiFileDialog::saveFile()
{
   
   //
   // See if a file is selected
   //
   QStringList theFiles = selectedFiles();
   if (theFiles.isEmpty() == false) {
   
      //
      // Get encoding for the XML file
      //
      AbstractFile::FILE_FORMAT fileFormat = AbstractFile::FILE_FORMAT_ASCII;
      std::vector<QString> fileFormatLabels;
      fileFormatLabels.push_back("ASCII");
      fileFormatLabels.push_back("Base64 Binary");
      fileFormatLabels.push_back("GZip Base64 Binary");
      QtRadioButtonSelectionDialog fileFormatDialog(this,
                                                    "Choose File Format",
                                                    "Choose the encoding of the XML file",
                                                    fileFormatLabels,
                                                    fileFormat);
      if (fileFormatDialog.exec() == QDialog::Accepted) {
         const int item = fileFormatDialog.getSelectedItemIndex();
         fileFormat = static_cast<AbstractFile::FILE_FORMAT>(item + 2);
      }
      else {
         return false;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      //
      // Get the name of the file
      //
      QString name(theFiles.at(0));
      
      //
      // Get the type of file being saved
      //
      const QString filterName = selectedFilter();

      //
      // Get the extension for this file type
      //
      QString desiredExtension;
      if (filterName == GuiDataFileDialog::giftiFunctionalFileFilter) {
         desiredExtension = SpecFile::getGiftiFileExtension();
      }
      else if (filterName == GuiDataFileDialog::giftiLabelFileFilter) {
         desiredExtension = SpecFile::getGiftiFileExtension();
      }
      else if (filterName == GuiDataFileDialog::giftiShapeFileFilter) {
         desiredExtension = SpecFile::getGiftiFileExtension();
      }
      else if (filterName == GuiDataFileDialog::giftiSurfaceFileFilter) {
         desiredExtension = SpecFile::getGiftiFileExtension();
      }
      else {
         QString msg("PROGRAMMING ERROR: File filter not handled properly at ");
         msg += __LINE__;
         msg += " in ";
         msg += __FILE__;
         GuiMessageBox::critical(this, "ERROR", msg, "OK");
         return false;
      }
      const QString filesExtension = "." + FileUtilities::filenameExtension(name);
      if (filesExtension != desiredExtension) {
         name.append(desiredExtension);
      }
      
      //
      // Save the file
      //
      try {
         BrainSet* brainSet = theMainWindow->getBrainSet();
         BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
         if (filterName == GuiDataFileDialog::giftiFunctionalFileFilter) {
            MetricFile* mf = brainSet->getMetricFile();
            mf->setFileWriteType(fileFormat);
            brainSet->writeMetricFile(name);
         }
         else if (filterName == GuiDataFileDialog::giftiLabelFileFilter) {
            PaintFile* pf = brainSet->getPaintFile();
            pf->setFileWriteType(fileFormat);
            brainSet->writePaintFile(name);
         }
         else if (filterName == GuiDataFileDialog::giftiShapeFileFilter) {
            SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
            ssf->setFileWriteType(fileFormat);
            brainSet->writeSurfaceShapeFile(name);
         }
         else if (filterName == GuiDataFileDialog::giftiSurfaceFileFilter) {
           theMainWindow->getBrainSet()->writeSurfaceFile(name,
                                                          bms->getSurfaceType(),
                                                          bms,
                                                          true,
                                                          fileFormat);
         }

         QApplication::restoreOverrideCursor(); 
      }
      catch (FileException& e) {
         GuiMessageBox::critical(this, "Error Writing File", e.whatQString(), "OK");
         return false;
      }
   }

   return true;
}

/**
 * open the file and return true if successful.
 */
bool 
GuiGiftiFileDialog::openFile()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   //
   // See if a file is selected
   //
   QStringList theFiles = selectedFiles();
   const int numFiles = theFiles.count();
   
   //
   // Get the type of file being opened
   //
   const QString filterName = selectedFilter();

   QString errorMessage;
   GuiFilesModified fm;
   
   //
   // Open each of the selected files
   //
   for (int i = 0; i < numFiles; i++) {   
      //
      // Get the name of the file
      //
      const QString name(theFiles.at(i));
      
      //
      // Open the file
      //
      try {
         const bool updateSpecFileFlag = true;
         const bool appendFileFlag = true;
         BrainSet* brainSet = theMainWindow->getBrainSet();
         if (filterName == GuiDataFileDialog::giftiFunctionalFileFilter) {
            brainSet->readMetricFile(name, appendFileFlag, updateSpecFileFlag);
            fm.setMetricModified();
         }
         else if (filterName == GuiDataFileDialog::giftiLabelFileFilter) {
            brainSet->readPaintFile(name, appendFileFlag, updateSpecFileFlag);
            fm.setPaintModified();
         }
         else if (filterName == GuiDataFileDialog::giftiShapeFileFilter) {
            brainSet->readSurfaceShapeFile(name, appendFileFlag, updateSpecFileFlag);
            fm.setSurfaceShapeModified();
         }
         else if (filterName == GuiDataFileDialog::giftiSurfaceFileFilter) {
            brainSet->readSurfaceFile(name,
                                      BrainModelSurface::SURFACE_TYPE_UNKNOWN,
                                      false,
                                      appendFileFlag,
                                      updateSpecFileFlag);
            fm.setCoordinateModified();
            fm.setTopologyModified();
         }
         else {
            QString msg("PROGRAMMING ERROR: File filter not handled properly at ");
            msg += __LINE__;
            msg += " in ";
            msg += __FILE__;
            GuiMessageBox::critical(this, "ERROR", msg, "OK");
            return false;
         }
      }
      catch (FileException& e) {
         errorMessage += e.whatQString();
      }
   }

   theMainWindow->fileModificationUpdate(fm);
   if (fm.getCoordinateModified()) {
      theMainWindow->displayNewestSurfaceInMainWindow();
   }

   GuiBrainModelOpenGL::updateAllGL();
   QApplication::restoreOverrideCursor(); 
   
   if (errorMessage.isEmpty() == false) {
      GuiMessageBox::critical(this, "Error Opening File", errorMessage, "OK");
      return false;
   }

   return true;
}

/**
 * show the dialog.
 */
void 
GuiGiftiFileDialog::show()
{
   //
   // Cause any files since dialog last shown to reappear
   //
   selectFilter(selectedFilter());
   
   QFileDialog::show();
}
      

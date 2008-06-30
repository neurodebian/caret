
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
#include <vector>

#include <QApplication>
#include <QImage>
#include <QMessageBox>
#include <QPixmap>
#include <QImageWriter>

#include "FileUtilities.h"
#include "WuQSaveWidgetAsImagePushButton.h"
#include "StringUtilities.h"
#include "WuQFileDialog.h"

/**
 * Constructor.
 */
WuQSaveWidgetAsImagePushButton::WuQSaveWidgetAsImagePushButton(const QString& text,
                              QWidget* widgetToSaveImageOfIn,
                              QWidget* parent)
   : QPushButton(text, parent)
{
   setAutoDefault(false);
   widgetToSaveImageOf = widgetToSaveImageOfIn;
   
   QObject::connect(this, SIGNAL(clicked()),
                    this, SLOT(slotClicked()));
}
                              
/**
 * Destructor.
 */
WuQSaveWidgetAsImagePushButton::~WuQSaveWidgetAsImagePushButton()
{
}
      
/**
 * called when button pressed to save the image.
 */
void 
WuQSaveWidgetAsImagePushButton::slotClicked()
{
   static QString previousFileFilter;
   
   //
   // Create the file filters and file extensions and find the jpeg and ppm filters
   //
   QStringList fileFilterList;
   std::vector<QString> fileFilters;
   std::vector<QString> fileExtensions;
   std::vector<QString> fileFormats;
   QString jpegFileFilter;
   QString ppmFileFilter;
   for (int i = 0; i < QImageWriter::supportedImageFormats().size(); i++) {
      QString str = QString(QImageWriter::supportedImageFormats().at(i));
      
      QString filter = QString("%1 Image File (*.%2)").arg(str).arg(
                                        StringUtilities::makeLowerCase(str));
      fileExtensions.push_back(StringUtilities::makeLowerCase(str));
      if (str == "ppm") {
         ppmFileFilter = filter;
      }
      else if (str == "jpg") {
         jpegFileFilter = filter;
      }
      fileFilterList << filter;
      fileFilters.push_back(filter);
      fileFormats.push_back(str);
   }
   
   //
   // Create the file dialog
   //
   WuQFileDialog saveImageDialog(this);
   saveImageDialog.setModal(true);
   saveImageDialog.setWindowTitle("Save Image File");
   saveImageDialog.setFileMode(WuQFileDialog::AnyFile);
   saveImageDialog.setDirectory(".");
   saveImageDialog.setAcceptMode(WuQFileDialog::AcceptSave);
   
   
   //
   // Set the file filters
   //
   saveImageDialog.setFilters(fileFilterList);
   if (previousFileFilter.isEmpty() == false) {
      saveImageDialog.selectFilter(previousFileFilter);
   }
   else if (jpegFileFilter.isEmpty() == false) {
      saveImageDialog.selectFilter(jpegFileFilter);
   }
   else if (ppmFileFilter.isEmpty() == false) {
      saveImageDialog.selectFilter(ppmFileFilter);
   }
   
   //
   // Execute the dialog
   //
   if (saveImageDialog.exec() == WuQFileDialog::Accepted) {
      QString name(saveImageDialog.selectedFiles().at(0));
      
      //
      // Get the selected file filter
      //
      previousFileFilter = saveImageDialog.selectedFilter();
      
      //
      // Find the file filter
      //
      int fileFilterIndex = -1;
      for (unsigned int i = 0; i < fileFilters.size(); i++) {
         if (fileFilters[i] == previousFileFilter) {
            fileFilterIndex = i;
            break;
         }
      }
      
      //
      // See if invalid filter index
      //
      if (fileFilterIndex < 0) {
         QString msg("Program Error: invalid file filter index when saving image file.");
         QApplication::beep();
         QMessageBox::critical(this, "PROGRAM ERROR", msg, "OK");
         return;
      }
      
      if (FileUtilities::filenameExtension(name) != fileExtensions[fileFilterIndex]) {
         name.append(".");
         name.append(fileExtensions[fileFilterIndex]);
      }
      
      QImage image = QPixmap::grabWidget(widgetToSaveImageOf).toImage();
      if (image.save(name, fileFormats[fileFilterIndex].toAscii().constData(), 100) == false) {
         QApplication::beep();
         QMessageBox::critical(this, "ERROR", "Unable to save image.", "OK");
      }
   }
}


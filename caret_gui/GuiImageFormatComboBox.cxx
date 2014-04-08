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
#include <QImageReader>
#include <QImageWriter>

#include "GuiImageFormatComboBox.h"

/**
 * constructor.
 */
GuiImageFormatComboBox::GuiImageFormatComboBox(const IMAGE_MODE im, 
                                               const DISPLAY_MODE dm,
                                               QWidget* parent)
   : QComboBox(parent)
{
   QList<QByteArray> formatsList;
   
   switch (im) {
      case IMAGE_MODE_OPEN:
         formatsList = QImageReader::supportedImageFormats();
         break;         
      case IMAGE_MODE_SAVE:
         formatsList = QImageWriter::supportedImageFormats();
         break;
   }
   
   int defaultItem = -1;
   
   for (int i = 0; i < formatsList.size(); i++) {
      const QString name(formatsList.at(i));
      imageFormatNames.append(name);
      
      QString ext("." + name);
      imageFormatExtensions.append(ext);
      
      QString filter;
      filter += name;
      filter += " image file";
      filter += " (*";
      filter += ext;
      filter += ")";
      imageFormatFilters << filter;
      
      switch (dm) {
         case DISPLAY_IMAGE_FORMAT_NAMES:
            addItem(name);
            break;
         case DISPLAY_IMAGE_FILE_FILTERS:
            addItem(filter);
            break;
      }
      
      if (defaultItem == -1) {
         if (name == "jpg") {
            defaultItem = i;
         }
         else if (name == "jpeg") {
            defaultItem = i;
         }
         else if (name == "ppm") {
            defaultItem = i;
         }
      }
      if (name == "jpg") {
         defaultItem = i;
      }
   }
   
   if ((defaultItem >= 0) && (count() > 0)) {
      setCurrentIndex(defaultItem);
   }
}

/**
 * destructor.
 */
GuiImageFormatComboBox::~GuiImageFormatComboBox()
{
}

/**
 * get the selected image format name.
 */
QString 
GuiImageFormatComboBox::getSelectedImageFormatName() const
{
   const int item = currentIndex();
   if ((item >= 0) && (item < count())) {
      return imageFormatNames.at(item);
   }
   
   return "";
}

/**
 * get the selection image format file extension.
 */
QString 
GuiImageFormatComboBox::getSelectedImageFormatExtension() const
{
   const int item = currentIndex();
   if ((item >= 0) && (item < count())) {
      return imageFormatExtensions.at(item);
   }
   
   return "";
}

/**
 * get the selected image filter.
 */
QString
GuiImageFormatComboBox::getSelectedImageFormatFilter() const
{
   const int item = currentIndex();
   if ((item >= 0) && (item < count())) {
      return imageFormatFilters.at(item);
   }
   
   return "";
}

/**
 * get all file filters.
 */
QStringList 
GuiImageFormatComboBox::getAllFileFilters() const
{
   return imageFormatFilters;
}

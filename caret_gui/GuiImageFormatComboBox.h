
#ifndef __GUI_IMAGE_FORMAT_COMBO_BOX_H__
#define __GUI_IMAGE_FORMAT_COMBO_BOX_H__

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
#include <QStringList>

/// class for choosing an image format
class GuiImageFormatComboBox : public QComboBox {
   Q_OBJECT
   
   public:
      /// image mode
      enum IMAGE_MODE{ 
         /// open image mode
         IMAGE_MODE_OPEN,
         /// save image mode
         IMAGE_MODE_SAVE
      };
      
      /// display mode
      enum DISPLAY_MODE {
         /// show the image format names
         DISPLAY_IMAGE_FORMAT_NAMES,
         /// show the image format file filters
         DISPLAY_IMAGE_FILE_FILTERS
      };
      
      // constructor
      GuiImageFormatComboBox(const IMAGE_MODE im, 
                             const DISPLAY_MODE dm,
                             QWidget* parent = 0);
      
      // destructor
      ~GuiImageFormatComboBox();
      
      // get the selected image format name
      QString getSelectedImageFormatName() const;
      
      // get the selected image format file extension
      QString getSelectedImageFormatExtension() const;
      
      // get the selected image filter
      QString getSelectedImageFormatFilter() const;
       
      // get all file filters
      QStringList getAllFileFilters() const;
      
   protected:
      /// the image format names
      QStringList imageFormatNames;

      /// the image formats
      QStringList imageFormatExtensions;
      
      /// the image format file filters
      QStringList imageFormatFilters;
};

#endif // __GUI_IMAGE_FORMAT_COMBO_BOX_H__


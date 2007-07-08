
#ifndef __GUI_IMAGE_FILE_OPEN_SAVE_DIALOG_H__
#define __GUI_IMAGE_FILE_OPEN_SAVE_DIALOG_H__

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

#include <vector>

#include <q3filedialog.h>
#include <QString>

class ImageFile;
class QCheckBox;
class QComboBox;

/// class for dialog used to open and save image files
class GuiImageFileOpenSaveDialog : public Q3FileDialog {
   Q_OBJECT
   
   public:
      /// dialog mode
      enum DIALOG_MODE {
         /// open files mode
         DIALOG_MODE_OPEN_FILES,
         /// save loaded image to file
         DIALOG_MODE_SAVE_LOADED_IMAGE,
         /// save main window as image file mode
         DIALOG_MODE_SAVE_IMAGE_OF_MAIN_WINDOW,
         /// number of modes
         DIALOG_NUMBER_OF_MODES
      };
      
      // constructor
      GuiImageFileOpenSaveDialog(QWidget* parent, const DIALOG_MODE dialogModeIn);
      
      // destructor
      ~GuiImageFileOpenSaveDialog();
                  
      // show the dialog
      virtual void show();
      
      // update the dialog
      void updateDialog();

      // set default image for saving
      void setDefaultImageForSaving(ImageFile* defaultImageFileForSavingIn);
      
   signals:
      // emitted when an image has been saved
      void signalImageHasBeenSaved();
      
   protected slots:
      // called when an image file is selected for saving
      void slotImageSelectionForSavingComboBox(int item);
      
      /// called when a previous directory is selected
      void slotPreviousDirectory(int item);
      
   protected:
      /// create the caret unique section.
      QWidget* createCaretUniqueSection();
      
      // called when Open/Save/Cancel button pressed
      void done(int r);
      
      /// add to spec file check box
      QCheckBox* addToSpecFileCheckBox;
      
      /// append to existing files check box
      QCheckBox* appendToExistingFilesCheckBox;
      
      /// the file filter
      std::vector<QString> fileFilters;

      /// the file extensions
      std::vector<QString> fileExtensions;
      
      /// the file formats
      std::vector<QString> fileFormats;
      
      /// the dialog mode
      DIALOG_MODE dialogMode;
      
      /// image selection combo box for saving image files
      QComboBox* imageSelectionForSavingComboBox;

      /// previous directories combo box
      QComboBox* previousDirectoryComboBox;
      
      /// previous directories
      std::vector<QString> previousDirectories;
      
      /// default file for image saving
      ImageFile* defaultImageFileForSaving;
};

#ifdef __GUI_IMAGE_FILE_OPEN_SAVE_DIALOG_MAIN__
#endif // __GUI_IMAGE_FILE_OPEN_SAVE_DIALOG_MAIN__

#endif // __GUI_IMAGE_FILE_OPEN_SAVE_DIALOG_H__


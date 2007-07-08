
#ifndef __GUI_CAPTURE_MAIN_WINDOW_IMAGE_DIALOG_H__
#define __GUI_CAPTURE_MAIN_WINDOW_IMAGE_DIALOG_H__

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

#include "QtDialogNonModal.h"

class QCheckBox;
class QLineEdit;
class GuiImageFormatComboBox;
class QRadioButton;
class QTabWidget;

/// dialog for capturing images of the main window
class GuiCaptureMainWindowImageDialog : public QtDialogNonModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaptureMainWindowImageDialog(QWidget* parent);
   
      // destructor
      ~GuiCaptureMainWindowImageDialog();
      
      // show the dialog
      virtual void show();
      
   protected slots:
      // capture an image of the main window
      void slotCaptureImage();
   
      // image file name selection
      void slotImageFileNameDialog();
      
      // image capture type selected
      void slotImageCaptureTypeSelected();
      
      // close the dialog
      void closeDialog();
      
   protected:
      /// create capture type group
      QWidget* createCaptureGroup();
      
      /// create image destination group
      QWidget* createImageDestinationGroup();
      
      /// create standard views group
      QWidget* createStandardViewsGroup();
      
      /// capture a normal image
      void captureNormalImage();
      
      /// capture images of standard view
      void captureStandardViewImages();
      
      /// tab for std views vs image dest
      QTabWidget* tabWidget;
      
      /// capture and image destination group widget
      QWidget* captureAndDestinationWidget;
      
      /// standard view group widget
      QWidget* standardViewGroupWidget;
      
      /// all image capture radio button
      QRadioButton* allCaptureRadioButton;
      
      /// selection image capture radio button
      QRadioButton* selectionImageRadioButton;
      
      /// copy to clipboard check box
      QCheckBox* copyToClipBoardCheckBox;
      
      /// add to spec file check box
      QCheckBox* addToSpecFileCheckBox;
      
      /// add to loaded images check box
      QCheckBox* addToLoadedImagesCheckBox;
      
      /// print image checkb box
      QCheckBox* printImageCheckBox;
      
      /// save to file check box
      QCheckBox* saveToFileCheckBox;
      
      /// save file name line edit
      QLineEdit* saveFileNameLineEdit;
      
      /// image format combo box
      GuiImageFormatComboBox* imageFormatComboBox;
      
      /// std view image file names line edit
      QLineEdit* medialViewLineEdit;
      
      /// std view image file names line edit
      QLineEdit* lateralViewLineEdit;
      
      /// std view image file names line edit
      QLineEdit* posteriorViewLineEdit;
      
      /// std view image file names line edit
      QLineEdit* anteriorViewLineEdit;
      
      /// std view image file names line edit
      QLineEdit* ventralViewLineEdit;
      
      /// std view image file names line edit
      QLineEdit* dorsalViewLineEdit;
};

#endif // __GUI_CAPTURE_MAIN_WINDOW_IMAGE_DIALOG_H__



#ifndef __GUI_CAPTURE_WINDOW_IMAGE_DIALOG_H__
#define __GUI_CAPTURE_WINDOW_IMAGE_DIALOG_H__

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


#include "BrainModel.h"
#include "WuQDialog.h"

class GuiImageFormatComboBox;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTabWidget;
class WuQWidgetGroup;

/// dialog for capturing images of caret windows
class GuiCaptureWindowImageDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaptureWindowImageDialog(QWidget* parent);
   
      // destructor
      ~GuiCaptureWindowImageDialog();
      
      // show the dialog
      virtual void show();
      
   protected slots:
      // capture an image of the main window
      void slotCaptureImage();
   
      // image file name selection
      void slotImageFileNameDialog();
      
      // image capture type selected
      void slotWindowSelectionComboBox(int indx);
      
      // close the dialog
      void closeDialog();
      
      // called when help button pressed
      void slotHelpPushButton();
      
   protected:
      /// window to capture
      enum WINDOW {
         /// capture main window graphics area
         WINDOW_MAIN_GRAPHICS_AREA,
         /// capture part of main window graphics area using mouse
         WINDOW_MAIN_SELECT_PART_OF_GRAPHICS_AREA,
         /// capture viewing window 2 graphics area
         WINDOW_VIEWING_WINDOW_2_GRAPHICS_AREA,
         /// capture viewing window 3 graphics area
         WINDOW_VIEWING_WINDOW_3_GRAPHICS_AREA,
         /// capture viewing window 4 graphics area
         WINDOW_VIEWING_WINDOW_4_GRAPHICS_AREA,
         /// capture viewing window 5 graphics area
         WINDOW_VIEWING_WINDOW_5_GRAPHICS_AREA,
         /// capture viewing window 6 graphics area
         WINDOW_VIEWING_WINDOW_6_GRAPHICS_AREA,
         /// capture viewing window 7 graphics area
         WINDOW_VIEWING_WINDOW_7_GRAPHICS_AREA,
         /// capture viewing window 8 graphics area
         WINDOW_VIEWING_WINDOW_8_GRAPHICS_AREA,
         /// capture viewing window 9 graphics area
         WINDOW_VIEWING_WINDOW_9_GRAPHICS_AREA,
         /// capture viewing window 10 graphics area
         WINDOW_VIEWING_WINDOW_10_GRAPHICS_AREA,
         /// main window and open viewing window graphics areas
         WINDOW_MAIN_AND_OPEN_VIEWING_WINDOW_GRAPHICS,
         /// copy image from clipboard
         WINDOW_CLIPBOARD,
         /// capture image of desktop
         WINDOW_DESKTOP,
         /// capture display control dialog
         WINDOW_DISPLAY_CONTROL_DIALOG,
         /// capture draw border dialog
         WINDOW_DRAW_BORDER_DIALOG,
         /// capture identify dialog
         WINDOW_IDENTIFY_DIALOG,
         /// capture image capture dialog
         WINDOW_IMAGE_CAPTURE_DIALOG,
         /// main window including toolbar
         WINDOW_MAIN_WINDOW,
         /// main window toolbar
         WINDOW_MAIN_WINDOW_TOOLBAR,
         /// capture map stereotaxic focus dialog
         WINDOW_MAP_STEREOTAXIC_FOCUS_DIALOG,
         /// capture recording dialog
         WINDOW_RECORDING_DIALOG,
         /// capture study metadata editor dialog
         WINDOW_STUDY_METADATA_EDITOR_DIALOG,
         /// capture surface region of interest dialog
         WINDOW_SURFACE_REGION_OF_INTEREST_DIALOG,
         /// viewing window 2 including toolbar
         WINDOW_VIEWING_WINDOW_2,
         /// viewing window 2 toolbar
         WINDOW_VIEWING_WINDOW_2_TOOLBAR
      };
      
      // create capture type group
      QWidget* createCaptureGroup();
      
      // create capture options group
      QWidget* captureOptionsGroup();
      
      // create image destination group
      QWidget* createImageDestinationGroup();
      
      // create standard views group
      QWidget* createStandardViewsGroup();
      
      // capture a normal image
      void captureNormalImage();
      
      // capture images of standard view
      void captureStandardViewImages();
      
      // capture an image of a viewing window (true if image valid)
      bool captureViewingWindowGraphicsArea(const BrainModel::BRAIN_MODEL_VIEW_NUMBER windowNumber,
                                            QImage& image);
                                            
      // load window selection combo box items
      void loadWindowSelectionComboBoxItems();
      
      // capture image of a widget
      bool captureImageOfWidget(QWidget* w, 
                                QImage& image,
                                const bool captureWidgetsWindowFlag);
      
      // capture image of main and viewing window graphics
      bool captureMainAndViewingWindowGraphics(QImage& image,
                                               const bool cropImagesFlag,
                                               const int cropMargin);
      
      // crop the image
      void cropImage(QImage& image,
                     const int margin);
                     
      /// capture push button
      QPushButton* capturePushButton;
      
      /// tab for std views vs image dest
      QTabWidget* tabWidget;
      
      /// combo box for window selection
      QComboBox* windowSelectionComboBox;
      
      /// multi image wrapping spin box
      QSpinBox* multiImageWrappingSpinBox;
      
      /// widget group for multi image wrapping widgets
      WuQWidgetGroup* multiImageWrappingWidgetGroup;
      
      /// capture and image destination group widget
      QWidget* captureAndDestinationWidget;
      
      /// standard view group widget
      QWidget* standardViewGroupWidget;
      
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
      
      /// adjust captured image size check box
      QCheckBox* adjustCapturedImageSizeCheckBox;
      
      /// auto crop image check box
      QCheckBox* autoCropImageCheckBox;
      
      /// auto crop image margin spin box
      QSpinBox* autoCropImageMarginSpinBox;
      
      /// widget group for auto crop options
      WuQWidgetGroup* autoCropWidgetGroup;
};

#endif // __GUI_CAPTURE_WINDOW_IMAGE_DIALOG_H__


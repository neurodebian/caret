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


#ifndef __GUI_IMAGE_EDITOR_WINDOW_H__
#define __GUI_IMAGE_EDITOR_WINDOW_H__

#include <QColor>
#include "QtDialog.h"
#include <QFont>

#include "SceneFile.h"

class GuiImageEditorWidget;
class GuiImageEditorWidgetAddTextDialog;
class ImageFile;
class QCheckBox;
class QComboBox;
class QContextMenuEvent;
class QImage;
class QLabel;
class QLineEdit;
class QPaintEvent;
class QSpinBox;

/// Dialog used to display and edit images
class GuiImageEditorWindow : public QtDialog {
   
   Q_OBJECT
   
   public:
      /// Constructor
      GuiImageEditorWindow(QWidget* parent);
      
      /// Destructor
      ~GuiImageEditorWindow();
      
      /// update the toolbar
      void updateWindow();
      
      /// display the image in the window
      void displayImage(const int imageNumberIn);
      
      /// save scene
      SceneFile::SceneClass saveScene();
      
      /// show scene
      void showScene(const SceneFile::SceneClass sc);
      
   protected slots:
      /// called when an image is selected
      void slotImageSelectionComboBox(int item);
      
      /// called to open an image file
      void slotOpenImageFile();
      
      /// called to save an image file
      void slotSaveImageFile();
      
      /// called to print an image
      void slotPrintImage();
      
      /// called when image size changed
      void slotImageSizeChanged();
      
      /// called to inform editor that an image has been saved
      void slotImageHasBeenSaved();
      
   protected:
      /// image size combo box
      QComboBox* imageSizeComboBox;
      
      /// the image selection combo box
      QComboBox* imageSelectionComboBox;
      
      /// the image viewer
      GuiImageEditorWidget* imageViewer;
};


//********************************************************************************
//********************************************************************************
//********************************************************************************


/// image editing widget
class GuiImageEditorWidget : public QWidget {
   Q_OBJECT
   
   public:
      /// viewing size of image
      enum VIEWING_IMAGE_SIZE {
         /// scale image to 25% of its original size
         VIEWING_IMAGE_SIZE_25,
         /// scale image to 50% of its original size
         VIEWING_IMAGE_SIZE_50,
         /// scale image to 75% of its original size
         VIEWING_IMAGE_SIZE_75,
         /// scale image to 100% of its original size
         VIEWING_IMAGE_SIZE_100,
         /// scale image to 125% of its original size
         VIEWING_IMAGE_SIZE_125,
         /// scale image to 150% of its original size
         VIEWING_IMAGE_SIZE_150,
         /// scale image to 200% of its original size
         VIEWING_IMAGE_SIZE_200,
         /// scale image to 400% of its original size
         VIEWING_IMAGE_SIZE_400,
         /// scale image to 800% of its original size
         VIEWING_IMAGE_SIZE_800
      };
      
      /// popup menu style
      enum POPUP_MENU {
         /// no popup menu
         POPUP_MENU_NONE,
         /// edit popup menu
         POPUP_MENU_EDIT_ONLY,
         /// edit, open, and save popup menu
         POPUP_MENU_EDIT_OPEN_SAVE
      };
      
      // constructor
      GuiImageEditorWidget(QWidget *parent, 
                        const POPUP_MENU popupMenuStyleIn,
                        Qt::WFlags f = 0);
      
      // destructor
      ~GuiImageEditorWidget();
      
      // set the image
      void setImageFile(ImageFile* img);
      
      /// get the image number in the widget
      ImageFile* getImageFile() { return imageFile; }
      
      /// get the image number in the widget (const method)
      const ImageFile* getImageFile() const { return imageFile; }
      
      // set the viewing size
      void setViewingSize(const VIEWING_IMAGE_SIZE vis);
      
      // get the viewing size
      VIEWING_IMAGE_SIZE getViewingSize() const;
      
      // get size hint
      QSize sizeHint() const;
    
   signals:
      void signalGeometryChanged();
      
   public slots:
      /// called to load an image
      void slotLoadImage();
      
      /// called to save an image
      void slotSaveImage();
      
      /// called to resize the image
      void slotResizeImage();
      
      /// called to crop the image
      void slotCropImage();
      
      /// called to add text to the image (text X/Y set to popup menu position)
      void slotAddTextPopupMenu();
      
      /// called to add text to the image (text X/Y at center of image)
      void slotAddText();
      
      /// called when a viewing size is selected
      void slotViewingSizeMenu(int id);
      
   protected:
      // called when mouse pressed
      void mousePressEvent(QMouseEvent* me);
      
      // called when mouse moved
      void mouseMoveEvent(QMouseEvent* me);
      
      // get cropping min/max
      void getCropping(int& minX, int& maxX,
                       int& minY, int& maxY) const;
                       
      // add text 
      void addText(const int x, const int y);
      
      // get the displayed image file
      ImageFile* getDisplayedImageFile();
      
      // paint the widget
      void paintEvent(QPaintEvent* pe);
      
      // popup the context menu
      void contextMenuEvent(QContextMenuEvent* cme);
      
      /// Get the image viewing width and height.
      void getImageViewingWidthAndHeight(int& w, int& h) const;

      /// window size changed
      void windowSizeChanged();

      /// get the QImage of the ImageFile being displayed
      QImage* getImageBeingDisplayed();
      
      /// get the QImage of the ImageFile being displayed (const method)
      const QImage* getImageBeingDisplayed() const;
      
      /// the image file being edited
      ImageFile* imageFile;
      
      /// image size combo box
      //QComboBox* imageSizeComboBox;

      /// cropping lines
      int croppingLines[4];
      
      /// cropping lines valid
      bool croppingLinesValid;

      /// the add text dialog
      GuiImageEditorWidgetAddTextDialog* addTextDialog;
      
      /// x of menu popup
      int menuX;

      /// y of menu popup
      int menuY;
      
      /// popup menu style
      POPUP_MENU popupMenuStyle;
      
      /// viewing image size
      VIEWING_IMAGE_SIZE viewingImageSize;
};

//********************************************************************************
//********************************************************************************
//********************************************************************************

/// dialog for resizing an image
class GuiImageEditorWidgetResizeDialog : public QtDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiImageEditorWidgetResizeDialog(QWidget* parent, const int xSize, const int ySize);
      
      // destructor
      ~GuiImageEditorWidgetResizeDialog();
   
      // get the image sizes
      void getImageSizes(int& x, int& y) const;
      
   protected slots:
      // called when x spin box value is changed
      void slotXSizeSpinBox(int value);
      
      // called when y spin box value is changed
      void slotYSizeSpinBox(int value);
      
   protected:
      // called when OK or Cancel button is pressed
      void done(int r);
      
      /// x size spin box
      QSpinBox* xSizeSpinBox;
      
      /// y size spin box
      QSpinBox* ySizeSpinBox;
      
      /// maintain aspect check box
      QCheckBox* aspectCheckBox;
      
      /// original aspect of image
      float originalAspect;
};

//********************************************************************************
//********************************************************************************
//********************************************************************************

/// dialog for adding text to an image
class GuiImageEditorWidgetAddTextDialog : public QtDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiImageEditorWidgetAddTextDialog(QWidget* parent, const int x, const int y);
      
      // destructor
      ~GuiImageEditorWidgetAddTextDialog();

      // get the info about the text
      void getTextInfo(QString& text,
                       int& x,
                       int& y,
                       int& rotation,
                       QFont& textFont,
                       QColor& textColor) const;
                       
   signals:
      /// emitted when text is being updated
      void signalTextUpdated();
      
   protected slots:
      // called to select the font
      void slotFontPushButton();
      
      // called to select the font color
      void slotFontColorPushButton();
      
   protected:
      // called when OK or Cancel button is pressed
      void done(int r);
      
      /// x spin box
      QSpinBox* xSpinBox;
      
      /// y spin box
      QSpinBox* ySpinBox;
      
      /// rotation box
      QSpinBox* rotationSpinBox;
      
      /// the text line edit
      QLineEdit* textLineEdit;
      
      /// the font push button
      QPushButton* fontPushButton;
      
      /// the font color push button
      QPushButton* fontColorPushButton;
      
      /// the font being used
      static QFont theFont;
      
      /// the font color
      static QColor fontColor;
      
      /// static data initialized flag
      static bool staticDataInitialized;
};

#ifdef __IMAGE_EDITOR_WIDGET_ADD_TEXT_DIALOG_MAIN__
QFont GuiImageEditorWidgetAddTextDialog::theFont;
bool GuiImageEditorWidgetAddTextDialog::staticDataInitialized = false;
QColor GuiImageEditorWidgetAddTextDialog::fontColor;
#endif // __IMAGE_EDITOR_WIDGET_ADD_TEXT_DIALOG_MAIN__


//********************************************************************************
//********************************************************************************
//********************************************************************************


#endif // __GUI_IMAGE_EDITOR_WINDOW_H__

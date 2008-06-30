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


#ifndef __GUI_IMAGE_VIEWING_WINDOW_H__
#define __GUI_IMAGE_VIEWING_WINDOW_H__

#include "WuQDialog.h"

#include "SceneFile.h"

class QComboBox;
class QLabel;

/// Dialog used to display images
class GuiImageViewingWindow : public WuQDialog {
   
   Q_OBJECT
   
   public:
      /// Constructor
      GuiImageViewingWindow(QWidget* parent);
      
      /// Destructor
      ~GuiImageViewingWindow();
      
      /// update the toolbar
      void updateWindow();
      
      /// display the image in the window
      void displayImage(const int imageNumberIn);
      
      /// get the displayed image number
      int getDisplayedImageNumber() const { return imageNumber; }
      
      /// save scene
      SceneFile::SceneClass saveScene();
      
      /// show scene
      void showScene(const SceneFile::SceneClass sc);
      
   protected slots:
      /// called when an image is selected
      void slotImageSelectionComboBox(int item);
      
      /// called when an image is resized
      void slotImageSizeComboBox(int item);
      
   protected:
      /// size of image
      enum IMAGE_SIZE {
         /// scale image to 25% of its original size
         IMAGE_SIZE_25,
         /// scale image to 50% of its original size
         IMAGE_SIZE_50,
         /// scale image to 75% of its original size
         IMAGE_SIZE_75,
         /// scale image to 100% of its original size
         IMAGE_SIZE_100,
         /// scale image to 125% of its original size
         IMAGE_SIZE_125,
         /// scale image to 150% of its original size
         IMAGE_SIZE_150,
         /// scale image to 200% of its original size
         IMAGE_SIZE_200,
         /// scale image to 400% of its original size
         IMAGE_SIZE_400,
         /// scale image to 800% of its original size
         IMAGE_SIZE_800
      };
      
      /// image size combo box
      QComboBox* imageSizeComboBox;
      
      /// the image selection combo box
      QComboBox* imageSelectionComboBox;
      
      /// currently viewed image
      int imageNumber;
      
      /// the label that will contain the image
      QLabel* imageLabel;
};

#endif // __GUI_IMAGE_VIEWING_WINDOW_H__


#ifndef __GUI_IMAGE_RESIZE_DIALOG_H__
#define __GUI_IMAGE_RESIZE_DIALOG_H__

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

#include <QDialog>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QImage;
class QSpinBox;

/// dialog for resizing a QImage
class GuiImageResizeDialog : public QDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiImageResizeDialog(QWidget* parent,
                           QImage& imageIn);
   
      // destructor
      ~GuiImageResizeDialog();
   
   public slots:
      // called when OK/Cancel pressed
      void done(int r);
      
   protected slots:
      /// called when pixel width spin box value changed
      void slotPixelWidthSpinBox(int);
      
      /// called when pixel height spin box value changed
      void slotPixelHeightSpinBox(int);
      
      /// called when image size width double spin box value changes
      void slotImageSizeWidthDoubleSpinBox(double);
      
      /// called when image size height double spin box value changes
      void slotImageSizeHeightDoubleSpinBox(double);
      
      /// called when image resolution spin box value changed
      void slotImageResolutionSpinBox(int);
      
      /// called when image resolution units combo box value changed
      void slotImageResolutionUnitsComboBox(int);
      
      /// called when image width/height units combo box value changed
      void slotImageWidthHeightUnitsComboBox(int);
      
      /// called when main aspect ratio check box toggled
      void slotMaintainAspectRatioCheckBoxToggled(bool);
      
   protected:
      /// image units 
      enum IMAGE_UNITS {
         /// image units centimeters 
         IMAGE_UNITS_CM,
         /// image units inches
         IMAGE_UNITS_INCHES,
         /// image units millimeters
         IMAGE_UNITS_MM
      };
      
      /// resolution units
      enum RESOLUTION_UNITS {
         /// resolution units pixels per centimeter
         RESOLUTION_UNITS_PIXEL_PERM_CM,
         /// resolution units pixels per inch
         RESOLUTION_UNITS_PIXELS_PER_INCH,
         /// resolution units pixels per millimeter
         RESOLUTION_UNITS_PIXELS_PER_MM
      };
      
      // create the pixel dimensions widget
      QWidget* createPixelDimensionsWidget();
      
      // create the image size widget
      QWidget* createImageSizeWidget();
      
      // create the options widget
      QWidget* createOptionsWidget();
      
      // load the image data into the dialog
      void loadImageData();
      
      // get the resolution in pixels per centimeter
      float getResolutionInPixelsPerCentimeter() const;
      
      // update pixel dimensions due to image size change
      void updatePixelDimensionsDueToImageSizeChange();
      
      // update image size due to pixel dimensions change
      void updateImageSizeDueToPixelDimensionsChange();
      
      /// the image being resized
      QImage& image;
      
      /// pixel widget spin box
      QSpinBox* pixelWidthSpinBox;
      
      /// pixel height spin box
      QSpinBox* pixelHeightSpinBox;
      
      /// image size width double spin box
      QDoubleSpinBox* imageSizeWidthDoubleSpinBox;
      
      /// image size height double spin box
      QDoubleSpinBox* imageSizeHeightDoubleSpinBox;
      
      /// image resolution spin box
      QSpinBox* imageResolutionSpinBox;
      
      /// image width/height units combo box
      QComboBox* imageWidthHeightUnitsComboBox;
      
      /// image resolution units combo box
      QComboBox* imageResolutionUnitsComboBox;
      
      /// maintain aspect ratio check box
      QCheckBox* maintainAspectRatioCheckBox;
      
      /// aspect ratio height divided by width;
      float aspectRatioHeightDividedByWidth;
};

#endif // __GUI_IMAGE_RESIZE_DIALOG_H__

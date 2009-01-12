
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

#include <limits>

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QSpinBox>

#include "GuiImageResizeDialog.h"

/**
 * constructor.
 */
GuiImageResizeDialog::GuiImageResizeDialog(QWidget* parent,
                                           QImage& imageIn)
   : WuQDialog(parent),
     image(imageIn)
{
   setWindowTitle("Image Size");
   
   //
   // Create the groups
   //
   QWidget* pixelDimWidget  = createPixelDimensionsWidget();
   QWidget* imageSizeWidget = createImageSizeWidget();
   QWidget* optionsWidget   = createOptionsWidget();
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                      QDialogButtonBox::Cancel);
   QObject::connect(buttonBox, SIGNAL(accepted()),
                    this, SLOT(accept()));
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(reject()));
                    
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(pixelDimWidget);
   dialogLayout->addWidget(imageSizeWidget);
   dialogLayout->addWidget(optionsWidget);
   dialogLayout->addWidget(buttonBox);

   aspectRatioHeightDividedByWidth = 1.0;
   loadImageData();
}

/**
 * destructor.
 */
GuiImageResizeDialog::~GuiImageResizeDialog()
{
}

/**
 * called when OK/Cancel pressed.
 */
void 
GuiImageResizeDialog::done(int r)
{
   if (r == GuiImageResizeDialog::Accepted) {
      const int width = pixelWidthSpinBox->value();
      const int height = pixelHeightSpinBox->value();
      if ((width <= 0) ||
          (height <= 0)) {
         QMessageBox::critical(this,
                               "ERROR",
                               "Pixel width or height is invalid.");
         return;
      }
      
      image = image.scaled(width,
                           height,
                           Qt::IgnoreAspectRatio,
                           Qt::SmoothTransformation);
      const int pixelsPerMeter = static_cast<int>(getResolutionInPixelsPerCentimeter() * 100.0);
      image.setDotsPerMeterX(pixelsPerMeter);
      image.setDotsPerMeterY(pixelsPerMeter);
   }
   
   WuQDialog::done(r);
}

/**
 * create the pixel dimensions widget.
 */
QWidget* 
GuiImageResizeDialog::createPixelDimensionsWidget()
{
   //
   // Width label and spin box
   //
   QLabel* widthLabel = new QLabel("Width");
   pixelWidthSpinBox = new QSpinBox;
   pixelWidthSpinBox->setMinimum(1);
   pixelWidthSpinBox->setMaximum(std::numeric_limits<int>::max());
   pixelWidthSpinBox->setSingleStep(1);
   QObject::connect(pixelWidthSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotPixelWidthSpinBox(int)));
                    
   //
   // height label and spin box
   //
   QLabel* heightLabel = new QLabel("Height");
   pixelHeightSpinBox = new QSpinBox;
   pixelHeightSpinBox->setMinimum(1);
   pixelHeightSpinBox->setMaximum(std::numeric_limits<int>::max());
   pixelHeightSpinBox->setSingleStep(1);
   QObject::connect(pixelHeightSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotPixelHeightSpinBox(int)));
                    
   //
   // Group box and layout
   //
   QGroupBox* gb = new QGroupBox("Pixel Dimensions");
   QGridLayout* gridLayout = new QGridLayout(gb);
   gridLayout->addWidget(widthLabel, 0, 0);
   gridLayout->addWidget(pixelWidthSpinBox, 0, 1);
   gridLayout->addWidget(heightLabel, 1, 0);
   gridLayout->addWidget(pixelHeightSpinBox, 1, 1);
   gridLayout->setColumnStretch(0, 0);
   gridLayout->setColumnStretch(1, 0);
   gridLayout->setColumnStretch(2, 100); // push to left
   
   return gb;
}

/**
 * create the image size widget.
 */
QWidget* 
GuiImageResizeDialog::createImageSizeWidget()
{
   //
   // Image width label and double spin box
   //
   QLabel* widthLabel = new QLabel("Width");
   imageSizeWidthDoubleSpinBox = new QDoubleSpinBox;
   imageSizeWidthDoubleSpinBox->setMinimum(0.0);
   imageSizeWidthDoubleSpinBox->setMaximum(std::numeric_limits<float>::max());
   imageSizeWidthDoubleSpinBox->setSingleStep(1.0);
   imageSizeWidthDoubleSpinBox->setDecimals(3);
   QObject::connect(imageSizeWidthDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotImageSizeWidthDoubleSpinBox(double)));
                    
   //
   // Image height label and double spin box
   //
   QLabel* heightLabel = new QLabel("Height");
   imageSizeHeightDoubleSpinBox = new QDoubleSpinBox;
   imageSizeHeightDoubleSpinBox->setMinimum(0.0);
   imageSizeHeightDoubleSpinBox->setMaximum(std::numeric_limits<float>::max());
   imageSizeHeightDoubleSpinBox->setSingleStep(1.0);
   imageSizeHeightDoubleSpinBox->setDecimals(3);
   QObject::connect(imageSizeHeightDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotImageSizeHeightDoubleSpinBox(double)));
     
   //
   // image width/height units combo box
   //
   imageWidthHeightUnitsComboBox = new QComboBox;
   imageWidthHeightUnitsComboBox->addItem("CM",
                                          static_cast<int>(IMAGE_UNITS_CM));
   imageWidthHeightUnitsComboBox->addItem("Inches",
                                          static_cast<int>(IMAGE_UNITS_INCHES));
   imageWidthHeightUnitsComboBox->addItem("MM",
                                          static_cast<int>(IMAGE_UNITS_MM));
   imageWidthHeightUnitsComboBox->setCurrentIndex(1);
   QObject::connect(imageWidthHeightUnitsComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotImageWidthHeightUnitsComboBox(int)));
                    
   //
   // Image resolution label and spin box
   //
   QLabel* resolutionLabel = new QLabel("Resolution");
   imageResolutionSpinBox = new QSpinBox;
   imageResolutionSpinBox->setMinimum(1);
   imageResolutionSpinBox->setMaximum(std::numeric_limits<int>::max());
   imageResolutionSpinBox->setSingleStep(1);
   QObject::connect(imageResolutionSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotImageResolutionSpinBox(int)));
   
   //
   // Image resolution units combo box
   //
   imageResolutionUnitsComboBox = new QComboBox;
   imageResolutionUnitsComboBox->addItem("Pixels/CM",
                                         static_cast<int>(RESOLUTION_UNITS_PIXEL_PERM_CM));
   imageResolutionUnitsComboBox->addItem("Pixels/Inch",
                                         static_cast<int>(RESOLUTION_UNITS_PIXELS_PER_INCH));
   imageResolutionUnitsComboBox->addItem("Pixels/MM",
                                         static_cast<int>(RESOLUTION_UNITS_PIXELS_PER_MM));
   imageResolutionUnitsComboBox->setCurrentIndex(1);
   QObject::connect(imageResolutionUnitsComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotImageResolutionUnitsComboBox(int)));
                    
   //
   // Group box and layout
   //
   QGroupBox* gb = new QGroupBox("Image Size");
   QGridLayout* gridLayout = new QGridLayout(gb);
   gridLayout->addWidget(widthLabel, 0, 0);
   gridLayout->addWidget(imageSizeWidthDoubleSpinBox, 0, 1);
   gridLayout->addWidget(heightLabel, 1, 0);
   gridLayout->addWidget(imageSizeHeightDoubleSpinBox, 1, 1);
   gridLayout->addWidget(imageWidthHeightUnitsComboBox, 0, 2, 2, 1, Qt::AlignVCenter 
                                                                    | Qt::AlignLeft);
   gridLayout->addWidget(resolutionLabel, 2, 0);
   gridLayout->addWidget(imageResolutionSpinBox, 2, 1);
   gridLayout->addWidget(imageResolutionUnitsComboBox, 2, 2);
   
   return gb;
}

/**
 * create the options widget.
 */
QWidget* 
GuiImageResizeDialog::createOptionsWidget()
{
   //
   // Maintain aspect ratio checkbox
   //
   maintainAspectRatioCheckBox = new QCheckBox("Keep Aspect Ratio");
   maintainAspectRatioCheckBox->setChecked(true);
   QObject::connect(maintainAspectRatioCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotMaintainAspectRatioCheckBoxToggled(bool)));
                    
   //
   // Group box and layout
   //
   QGroupBox* gb = new QGroupBox("Options");
   QVBoxLayout* layout = new QVBoxLayout(gb);
   layout->addWidget(maintainAspectRatioCheckBox);
   
   return gb;
}

/**
 * called when pixel width spin box value changed.
 */
void 
GuiImageResizeDialog::slotPixelWidthSpinBox(int)
{
   if (maintainAspectRatioCheckBox->isChecked()) {
      const float width = pixelWidthSpinBox->value();
      if (width != 0.0) {
         const float height = width * aspectRatioHeightDividedByWidth;
         pixelHeightSpinBox->blockSignals(true);
         pixelHeightSpinBox->setValue(static_cast<int>(height + 0.5));
         pixelHeightSpinBox->blockSignals(false);
      }
   }
   updateImageSizeDueToPixelDimensionsChange();
}

/**
 * called when pixel height spin box value changed.
 */
void 
GuiImageResizeDialog::slotPixelHeightSpinBox(int)
{
   if (maintainAspectRatioCheckBox->isChecked()) {
      const float height = pixelHeightSpinBox->value();
      if (height != 0.0) {
         const float width = height / aspectRatioHeightDividedByWidth;
         pixelWidthSpinBox->blockSignals(true);
         pixelWidthSpinBox->setValue(static_cast<int>(width + 0.5));
         pixelWidthSpinBox->blockSignals(false);
      }
   }
   updateImageSizeDueToPixelDimensionsChange();
}

/**
 * called when image size width double spin box value changes.
 */
void 
GuiImageResizeDialog::slotImageSizeWidthDoubleSpinBox(double)
{
   if (maintainAspectRatioCheckBox->isChecked()) {
      const float width = imageSizeWidthDoubleSpinBox->value();
      if (width != 0.0) {
         const float height = width * aspectRatioHeightDividedByWidth;
         imageSizeHeightDoubleSpinBox->blockSignals(true);
         imageSizeHeightDoubleSpinBox->setValue(height);
         imageSizeHeightDoubleSpinBox->blockSignals(false);
      }
   }
   updatePixelDimensionsDueToImageSizeChange();
}

/**
 * called when image size height double spin box value changes.
 */
void 
GuiImageResizeDialog::slotImageSizeHeightDoubleSpinBox(double)
{
   if (maintainAspectRatioCheckBox->isChecked()) {
      const float height = imageSizeHeightDoubleSpinBox->value();
      if (height != 0.0) {
         const float width = height / aspectRatioHeightDividedByWidth;
         imageSizeWidthDoubleSpinBox->blockSignals(true);
         imageSizeWidthDoubleSpinBox->setValue(width);
         imageSizeWidthDoubleSpinBox->blockSignals(false);
      }
   }
   updatePixelDimensionsDueToImageSizeChange();
}

/**
 * called when image resolution spin box value changed.
 */
void 
GuiImageResizeDialog::slotImageResolutionSpinBox(int)
{
   updatePixelDimensionsDueToImageSizeChange();
}
      
/**
 * called when image resolution units combo box value changed.
 */
void 
GuiImageResizeDialog::slotImageResolutionUnitsComboBox(int resolutionIndex)
{
   //
   // Load resolution
   //
   const float resolutionPerMeter = image.dotsPerMeterX();
   const RESOLUTION_UNITS resolutionUnits = 
      static_cast<RESOLUTION_UNITS>(
         imageResolutionUnitsComboBox->itemData(resolutionIndex).toInt());
   double resolutionValue = 1.0;
   switch (resolutionUnits) {
      case RESOLUTION_UNITS_PIXEL_PERM_CM:
         resolutionValue = resolutionPerMeter / 100.0;
         break;
      case RESOLUTION_UNITS_PIXELS_PER_INCH:
         resolutionValue = resolutionPerMeter / 39.37008;
         break;
      case RESOLUTION_UNITS_PIXELS_PER_MM:
         resolutionValue = resolutionPerMeter / 1000.0;
         break;
   }
   
   //
   // Add 0.5 so rounding occurs
   //
   imageResolutionSpinBox->blockSignals(true);
   imageResolutionSpinBox->setValue(static_cast<int>(resolutionValue + 0.5));
   imageResolutionSpinBox->blockSignals(false);
}

/**
 * get the resolution in pixels per centimeter.
 */
float 
GuiImageResizeDialog::getResolutionInPixelsPerCentimeter() const
{
   const int resolutionIndex = imageResolutionUnitsComboBox->currentIndex();
   const RESOLUTION_UNITS resolutionUnits = 
      static_cast<RESOLUTION_UNITS>(
         imageResolutionUnitsComboBox->itemData(resolutionIndex).toInt());

   float resolutionPixelsPerCentimeter = 10;
   const float resValue = imageResolutionSpinBox->value();
   
   switch (resolutionUnits) {
      case RESOLUTION_UNITS_PIXEL_PERM_CM:
         resolutionPixelsPerCentimeter = resValue;
         break;
      case RESOLUTION_UNITS_PIXELS_PER_INCH:
         resolutionPixelsPerCentimeter = resValue / 2.54;
         break;
      case RESOLUTION_UNITS_PIXELS_PER_MM:
         resolutionPixelsPerCentimeter = resValue / 10.0;
         break;
   }
   
   if (resolutionPixelsPerCentimeter <= 0.0) {
      resolutionPixelsPerCentimeter = 1.0;
   }
   
   return resolutionPixelsPerCentimeter;
}
      
/**
 * called when image width/height units combo box value changed.
 */
void 
GuiImageResizeDialog::slotImageWidthHeightUnitsComboBox(int /*imageUnitsIndex*/)
{
   updateImageSizeDueToPixelDimensionsChange();

/*
   const IMAGE_UNITS imageUnits =
      static_cast<IMAGE_UNITS>(
         imageWidthHeightUnitsComboBox->itemData(imageUnitsIndex).toInt());

   const float resolutionPixelsPerCentimeter = getResolutionInPixelsPerCentimeter();

   const float widthInCM  = pixelWidthSpinBox->value() 
                            / resolutionPixelsPerCentimeter;
   const float heightInCM = pixelHeightSpinBox->value() 
                            / resolutionPixelsPerCentimeter;
                            
   switch (imageUnits) {
      case IMAGE_UNITS_CM:
         imageSizeWidthDoubleSpinBox->setValue(widthInCM);
         imageSizeHeightDoubleSpinBox->setValue(heightInCM);
         break;
      case IMAGE_UNITS_INCHES:
         imageSizeWidthDoubleSpinBox->setValue(widthInCM * 0.3937008);
         imageSizeHeightDoubleSpinBox->setValue(heightInCM * 0.3937008);
         break;
      case IMAGE_UNITS_MM:
         imageSizeWidthDoubleSpinBox->setValue(widthInCM * 10.0);
         imageSizeHeightDoubleSpinBox->setValue(heightInCM * 10.0);
         break;
   }
*/
}

/**
 * called when main aspect ratio check box toggled.
 */
void 
GuiImageResizeDialog::slotMaintainAspectRatioCheckBoxToggled(bool isOn)
{
   if (isOn) {
      if (pixelWidthSpinBox->value() != 0.0) {
         aspectRatioHeightDividedByWidth = 
            static_cast<float>(pixelHeightSpinBox->value())
            / static_cast<float>(pixelWidthSpinBox->value());
         if (aspectRatioHeightDividedByWidth == 0.0) {
            aspectRatioHeightDividedByWidth = 1.0;
         }
      }
      else {
         aspectRatioHeightDividedByWidth = 1.0;
      }
   }
}
      
/**
 * load the image data into the dialog.
 */
void 
GuiImageResizeDialog::loadImageData()
{
   //
   // Set width and height
   //
   pixelWidthSpinBox->blockSignals(true);
   pixelWidthSpinBox->setValue(image.width());
   pixelWidthSpinBox->blockSignals(false);
   pixelHeightSpinBox->blockSignals(true);
   pixelHeightSpinBox->setValue(image.height());  
   pixelHeightSpinBox->blockSignals(false);
   
   //
   // Set aspect ratio using pixel width and height
   //
   slotMaintainAspectRatioCheckBoxToggled(maintainAspectRatioCheckBox->isChecked());

   //
   // Set resolution
   //
   slotImageResolutionUnitsComboBox(imageResolutionUnitsComboBox->currentIndex());
   
   //
   // Set image size
   // 
   slotImageWidthHeightUnitsComboBox(imageWidthHeightUnitsComboBox->currentIndex());
}
      
/**
 * update pixel dimensions due to image size change.
 */
void 
GuiImageResizeDialog::updatePixelDimensionsDueToImageSizeChange()
{
   const int imageUnitsIndex = imageWidthHeightUnitsComboBox->currentIndex();
   const IMAGE_UNITS imageUnits =
      static_cast<IMAGE_UNITS>(
         imageWidthHeightUnitsComboBox->itemData(imageUnitsIndex).toInt());

   const float resolutionPixelsPerCentimeter = getResolutionInPixelsPerCentimeter();

   float widthInCM = 1.0;
   float heightInCM = 1.0;
   switch (imageUnits) {
      case IMAGE_UNITS_CM:
         widthInCM = imageSizeWidthDoubleSpinBox->value() 
                     * resolutionPixelsPerCentimeter;
         heightInCM = imageSizeHeightDoubleSpinBox->value()
                     * resolutionPixelsPerCentimeter;
         break;
      case IMAGE_UNITS_INCHES:
         widthInCM = imageSizeWidthDoubleSpinBox->value()
                     * resolutionPixelsPerCentimeter
                     * 2.54;
         heightInCM = imageSizeHeightDoubleSpinBox->value()
                     * resolutionPixelsPerCentimeter
                     * 2.54;
         break;
      case IMAGE_UNITS_MM:
         widthInCM = imageSizeWidthDoubleSpinBox->value()
                     * resolutionPixelsPerCentimeter
                     * 10.0;
         heightInCM = imageSizeHeightDoubleSpinBox->value()
                     * resolutionPixelsPerCentimeter
                     * 10.0;
         break;
   }
   
   pixelWidthSpinBox->blockSignals(true);
   pixelWidthSpinBox->setValue(static_cast<int>(widthInCM));
   pixelWidthSpinBox->blockSignals(false);
   pixelHeightSpinBox->blockSignals(true);
   pixelHeightSpinBox->setValue(static_cast<int>(heightInCM));
   pixelHeightSpinBox->blockSignals(false);
}

/**
 * update image size due to pixel dimensions change.
 */
void 
GuiImageResizeDialog::updateImageSizeDueToPixelDimensionsChange()
{
   const int imageUnitsIndex = imageWidthHeightUnitsComboBox->currentIndex();
   const IMAGE_UNITS imageUnits =
      static_cast<IMAGE_UNITS>(
         imageWidthHeightUnitsComboBox->itemData(imageUnitsIndex).toInt());

   const float resolutionPixelsPerCentimeter = getResolutionInPixelsPerCentimeter();

   const float widthInCM  = pixelWidthSpinBox->value() 
                            / resolutionPixelsPerCentimeter;
   const float heightInCM = pixelHeightSpinBox->value() 
                            / resolutionPixelsPerCentimeter;
                            
   imageSizeWidthDoubleSpinBox->blockSignals(true);
   imageSizeHeightDoubleSpinBox->blockSignals(true);
   switch (imageUnits) {
      case IMAGE_UNITS_CM:
         imageSizeWidthDoubleSpinBox->setValue(widthInCM);
         imageSizeHeightDoubleSpinBox->setValue(heightInCM);
         break;
      case IMAGE_UNITS_INCHES:
         imageSizeWidthDoubleSpinBox->setValue(widthInCM * 0.3937008);
         imageSizeHeightDoubleSpinBox->setValue(heightInCM * 0.3937008);
         break;
      case IMAGE_UNITS_MM:
         imageSizeWidthDoubleSpinBox->setValue(widthInCM * 10.0);
         imageSizeHeightDoubleSpinBox->setValue(heightInCM * 10.0);
         break;
   }
   imageSizeWidthDoubleSpinBox->blockSignals(false);
   imageSizeHeightDoubleSpinBox->blockSignals(false);
}
      


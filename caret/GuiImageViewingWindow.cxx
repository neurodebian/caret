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

#include <algorithm>
#include <iostream>
#include <sstream>

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QPushButton>
#include <QToolTip>

#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "GuiImageViewingWindow.h"
#include "GuiMainWindow.h"
#include "ImageFile.h"
#include "StringUtilities.h"
#include "global_variables.h"

/**
 * The Constructor.
 */
GuiImageViewingWindow::GuiImageViewingWindow(QWidget* parent)
   : WuQDialog(parent)
{
   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle("Image Viewing Window");
   
   //
   // selection and size   
   //
   QGroupBox* selHGroupBox = new QGroupBox("Image Selection and Scaling");   
   imageSelectionComboBox = new QComboBox;
   imageSelectionComboBox->setFixedHeight(imageSelectionComboBox->sizeHint().height());
   imageSelectionComboBox->setToolTip( "Use this control to choose\n"
                                         "the displayed image.");
   QObject::connect(imageSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotImageSelectionComboBox(int)));
   imageSizeComboBox = new QComboBox;
   imageSizeComboBox->setFixedHeight(imageSizeComboBox->sizeHint().height());
   imageSizeComboBox->addItem(" 25%");
   imageSizeComboBox->addItem(" 50%");
   imageSizeComboBox->addItem(" 75%");
   imageSizeComboBox->addItem("100%");
   imageSizeComboBox->addItem("125%");
   imageSizeComboBox->addItem("150%");
   imageSizeComboBox->addItem("200%");
   imageSizeComboBox->addItem("400%");
   imageSizeComboBox->addItem("800%");
   imageSizeComboBox->setCurrentIndex(IMAGE_SIZE_100);
   imageSizeComboBox->setToolTip( "Use this control to change\n"
                                    "the image's display size.");
   QObject::connect(imageSizeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotImageSizeComboBox(int)));
                    
   //
   // the image label
   // 
   imageLabel = new QLabel;
   //imageLabel->setFrameStyle(QFrame::StyledPanel + QFrame::Plain);
   imageLabel->setLineWidth(2);
   
   //
   // Image Group Box
   //
   QGroupBox* imageGroupBox = new QGroupBox("Image");
   QVBoxLayout* imageLayout = new QVBoxLayout(imageGroupBox);
   imageLayout->addWidget(imageLabel);
   
   //
   // Close Button
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   closeButton->setFixedSize(closeButton->sizeHint());
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   
   //
   // Layouts for the dialog
   //
   QHBoxLayout* sizeScaleLayout = new QHBoxLayout;
   sizeScaleLayout->addWidget(imageSelectionComboBox);
   sizeScaleLayout->addWidget(imageSizeComboBox);
   sizeScaleLayout->setStretchFactor(imageSelectionComboBox, 100);
   sizeScaleLayout->setStretchFactor(imageSizeComboBox, 0);
   selHGroupBox->setLayout(sizeScaleLayout);
   
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(closeButton);
   
   QVBoxLayout* dialogLayout = new QVBoxLayout;
   dialogLayout->setSpacing(5);
   dialogLayout->addWidget(selHGroupBox);
   dialogLayout->addWidget(imageGroupBox);
   dialogLayout->addLayout(buttonsLayout);
   setLayout(dialogLayout);
   
   selHGroupBox->setFixedHeight(selHGroupBox->sizeHint().height());
   
   imageNumber = 0;
   updateWindow();
}

/**
 * The Destructor.
 */
GuiImageViewingWindow::~GuiImageViewingWindow()
{
   theMainWindow->removeImageViewingWindow(this);
}

/**
 * called when an image is selected.
 */
void 
GuiImageViewingWindow::slotImageSelectionComboBox(int item)
{
   imageSizeComboBox->blockSignals(true);
   //imageSizeComboBox->setCurrentIndex(IMAGE_SIZE_100);
   imageSizeComboBox->blockSignals(false);
   displayImage(item);
}
      
/**
 * called when an image is resized.
 */
void 
GuiImageViewingWindow::slotImageSizeComboBox(int /*item*/)
{
   displayImage(imageNumber);
}
      
/**
 */
void 
GuiImageViewingWindow::updateWindow()
{
   const int numImages = theMainWindow->getBrainSet()->getNumberOfImageFiles();
   if (imageNumber >= numImages) {
      imageNumber = 0;
   }
   displayImage(imageNumber);
   
   imageSelectionComboBox->blockSignals(true);
   imageSelectionComboBox->clear();
   for (int i = 0; i < numImages; i++) {
      ImageFile* img = theMainWindow->getBrainSet()->getImageFile(i);
      imageSelectionComboBox->addItem(FileUtilities::basename(img->getFileName()));
   }
   if ((imageNumber >= 0) && (imageNumber < imageSelectionComboBox->count())) {
      imageSelectionComboBox->setCurrentIndex(imageNumber);
   }
   imageSelectionComboBox->blockSignals(false);
}

/**
 */
void 
GuiImageViewingWindow::displayImage(const int imageNumberIn)
{
   const int windowX = std::max(x(), 10);
   const int windowY = std::max(y(), 10);

   const int numImages = theMainWindow->getBrainSet()->getNumberOfImageFiles();
   imageNumber = imageNumberIn;
   
   QImage image;
   if ((imageNumber >= 0) && (imageNumber < numImages)) {
      ImageFile* img = theMainWindow->getBrainSet()->getImageFile(imageNumber);
      image = *(img->getImage());
   }
   
   float w = image.width();
   float h = image.height();
   
   switch (static_cast<IMAGE_SIZE>(imageSizeComboBox->currentIndex())) {
      case IMAGE_SIZE_25:
         w *= 0.25;
         h *= 0.25;
         break;
      case IMAGE_SIZE_50:
         w *= 0.50;
         h *= 0.50;
         break;
      case IMAGE_SIZE_75:
         w *= 0.75;
         h *= 0.75;
         break;
      case IMAGE_SIZE_100:
         w *= 1.0;
         h *= 1.0;
         break;
      case IMAGE_SIZE_125:
         w *= 1.25;
         h *= 1.25;
         break;
      case IMAGE_SIZE_150:
         w *= 1.50;
         h *= 1.50;
         break;
      case IMAGE_SIZE_200:
         w *= 2.0;
         h *= 2.0;
         break;
      case IMAGE_SIZE_400:
         w *= 4.0;
         h *= 4.0;
         break;
      case IMAGE_SIZE_800:
         w *= 8.0;
         h *= 8.0;
         break;
   }
   
   if ((w > 0.0) && (h > 0.0)) {
      image = image.scaled(static_cast<int>(w), static_cast<int>(h),
                              Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation);
   }
   
   imageLabel->setPixmap(QPixmap::fromImage(image));
   
   //imageLabel->setFixedSize(imageLabel->sizeHint());

   //
   // Allows dialog to resize to fit image and don't let it change position
   //   
   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   move(windowX, windowY);
   setMinimumSize(sizeHint());
   setMaximumSize(sizeHint());
   updateGeometry();
   imageLabel->adjustSize();
   resize(sizeHint());
   adjustSize();
   move(windowX, windowY);
   
   if (DebugControl::getDebugOn()) {
      const QSize qs1 = sizeHint();
      const QSize qs2 = size();
      const QSize qs3 = maximumSize();
      std::cout << "Image View Window Size Hint: " << qs1.width() << ", " << qs1.height() << std::endl;
      std::cout << "Image View Window Size:      " << qs2.width() << ", " << qs2.height() << std::endl;
      std::cout << "Image View Window Max Size:  " << qs3.width() << ", " << qs3.height() << std::endl;
   }
}      
      
/**
 * save scene.
 */
SceneFile::SceneClass 
GuiImageViewingWindow::saveScene()
{      
   SceneFile::SceneClass sc("ImageViewingWindow");
      
   const ImageFile* img = theMainWindow->getBrainSet()->getImageFile(imageNumber);
   if (img != NULL) {
      const int geometry[4] = {
         x(),
         y(),
         width(),
         height()
      };
      
      std::ostringstream str;
      str << geometry[0] << " "
          << geometry[1] << " "
          << geometry[2] << " "
          << geometry[3];
      sc.addSceneInfo(SceneFile::SceneInfo("Geometry", str.str().c_str()));
      sc.addSceneInfo(SceneFile::SceneInfo("ImageName", FileUtilities::basename(img->getFileName())));
      
      QString sizeString;
      switch (static_cast<IMAGE_SIZE>(imageSizeComboBox->currentIndex())) {
         case IMAGE_SIZE_25:
            sizeString = "25";
            break;
         case IMAGE_SIZE_50:
            sizeString = "50";
            break;
         case IMAGE_SIZE_75:
            sizeString = "75";
            break;
         case IMAGE_SIZE_100:
            sizeString = "100";
            break;
         case IMAGE_SIZE_125:
            sizeString = "125";
            break;
         case IMAGE_SIZE_150:
            sizeString = "150";
            break;
         case IMAGE_SIZE_200:
            sizeString = "200";
            break;
         case IMAGE_SIZE_400:
            sizeString = "400";
            break;
         case IMAGE_SIZE_800:
            sizeString = "800";
            break;
      }
      
      if (sizeString.isEmpty() == false) {
         sc.addSceneInfo(SceneFile::SceneInfo("ImageScale", sizeString));
      }
   }
   
   return sc;
}

/**
 * show scene.
 */
void 
GuiImageViewingWindow::showScene(const SceneFile::SceneClass sc)
{
   QString imageName;
   QString sizeString;
   int geometry[4] = { 0, 0, 0, 0 };
      
   const int num = sc.getNumberOfSceneInfo();
   for (int i = 0; i < num; i++) {
      const SceneFile::SceneInfo* si = sc.getSceneInfo(i);
      const QString infoName = si->getName();      
      
      if (infoName == "ImageName") {
         imageName = si->getValueAsString();
      }
      else if (infoName == "ImageScale") {
         sizeString = si->getValueAsString();
      }
      else if (infoName == "Geometry") {
         std::vector<QString> tokens;
         StringUtilities::token(si->getValueAsString(), " ", tokens);
         if (tokens.size() >= 4) {
            geometry[0] = StringUtilities::toInt(tokens[0]);
            geometry[1] = StringUtilities::toInt(tokens[1]);
            geometry[2] = StringUtilities::toInt(tokens[2]);
            geometry[3] = StringUtilities::toInt(tokens[3]);
         }
      }
   }
      
   if ((imageName.isEmpty() == false) &&
       (geometry[2] > 0)) {
      imageNumber = -1;
      for (int j = 0; j < theMainWindow->getBrainSet()->getNumberOfImageFiles(); j++) {
         ImageFile* img = theMainWindow->getBrainSet()->getImageFile(j);
         if (FileUtilities::basename(img->getFileName()) == imageName) {
            imageNumber = j;
            break;
         }
      }
      
      if (imageNumber >= 0) {
         IMAGE_SIZE sz = IMAGE_SIZE_100;
         if (sizeString == "25") {
            sz = IMAGE_SIZE_25;
         }
         else if (sizeString == "50") {
            sz = IMAGE_SIZE_50;
         }
         else if (sizeString == "75") {
            sz = IMAGE_SIZE_75;
         }
         else if (sizeString == "100") {
            sz = IMAGE_SIZE_100;
         }
         else if (sizeString == "125") {
            sz = IMAGE_SIZE_125;
         }
         else if (sizeString == "150") {
            sz = IMAGE_SIZE_150;
         }
         else if (sizeString == "200") {
            sz = IMAGE_SIZE_200;
         }
         else if (sizeString == "400") {
            sz = IMAGE_SIZE_400;
         }
         else if (sizeString == "800") {
            sz = IMAGE_SIZE_800;
         }
         
         imageSizeComboBox->setCurrentIndex(sz);

         move(geometry[0], geometry[1]);
         //resize(geometry[2], geometry[3]);         

         updateWindow();

         move(geometry[0], geometry[1]);
         //resize(geometry[2], geometry[3]);         
      }
   }
}

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

#include <QApplication>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QCursor>
#include <QFontDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QSpinBox>
#include <QToolButton>
#include <QToolTip>

#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"

#define __IMAGE_EDITOR_WIDGET_ADD_TEXT_DIALOG_MAIN__
#include "GuiImageEditorWindow.h"
#undef __IMAGE_EDITOR_WIDGET_ADD_TEXT_DIALOG_MAIN__

#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiImageFileOpenSaveDialog.h"
#include "ImageFile.h"
#include "StringUtilities.h"
#include "global_variables.h"

/**
 * The Constructor.
 */
GuiImageEditorWindow::GuiImageEditorWindow(QWidget* parent)
   : QtDialog(parent, false)
{
   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   setWindowTitle("Image Editing Window");
   
   //
   // Layout for the dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);

   //
   // box for toolbar row 1   
   //
   QHBoxLayout* toolbar1Layout = new QHBoxLayout;
   dialogLayout->addLayout(toolbar1Layout);

   //
   // The selection combo box
   //
   imageSelectionComboBox = new QComboBox;
   toolbar1Layout->addWidget(imageSelectionComboBox);
   imageSelectionComboBox->setFixedHeight(imageSelectionComboBox->sizeHint().height());
   imageSelectionComboBox->setToolTip( "Use this control to choose\n"
                                         "the displayed image.");
   QObject::connect(imageSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotImageSelectionComboBox(int)));
    
   //toolbar1->setFixedHeight(toolbar1->sizeHint().height());

   //
   // box for toolbar row 2   
   //
   QHBoxLayout* toolbar2Layout = new QHBoxLayout;
   dialogLayout->addLayout(toolbar2Layout);

   //
   // Open image file button
   //
   QToolButton* openToolButton = new QToolButton;
   toolbar2Layout->addWidget(openToolButton);
   openToolButton->setText("O");
   openToolButton->setToolTip(
                   "Open an Image File");
   QObject::connect(openToolButton, SIGNAL(clicked()),
                    this, SLOT(slotOpenImageFile()));

   //
   // Save image file button
   //
   QToolButton* saveToolButton = new QToolButton;
   toolbar2Layout->addWidget(saveToolButton);
   saveToolButton->setText("S");
   saveToolButton->setToolTip(
                   "Save an Image File");
   QObject::connect(saveToolButton, SIGNAL(clicked()),
                    this, SLOT(slotSaveImageFile()));
                    
   //
   // Print image file button
   //
   QToolButton* printToolButton = new QToolButton;
   toolbar2Layout->addWidget(printToolButton);
   printToolButton->setText("P");
   printToolButton->setToolTip(
                   "Print Image");
   QObject::connect(printToolButton, SIGNAL(clicked()),
                    this, SLOT(slotPrintImage()));
                    
   //
   // Space
   //
   QLabel* dumbLabel = new QLabel(" ");
   dumbLabel->setFixedSize(dumbLabel->sizeHint());
   toolbar2Layout->addWidget(dumbLabel);
   
   //
   // Annotate tool button
   //
   QToolButton* annotateToolButton = new QToolButton;
   toolbar2Layout->addWidget(annotateToolButton);
   annotateToolButton->setText("A");
   annotateToolButton->setToolTip(
                   "Annotate the Image");
   
   //
   // Crop tool button
   //
   QToolButton* cropToolButton = new QToolButton;
   toolbar2Layout->addWidget(cropToolButton);
   cropToolButton->setText("C");
   cropToolButton->setToolTip("To crop an image, first move the mouse to\n"
                            "a corner of the desired subregion of the image\n"
                            "and, while holding down the left mouse button,\n"
                            "drag the mouse to the opposite corner of the\n"
                            "image subregion and release the mouse.  Next,\n"
                            "press the crop button \"C\" in the toolbar to\n"
                            "crop the image.");
   
   //
   // Resize tool button
   //
   QToolButton* resizeToolButton = new QToolButton;
   toolbar2Layout->addWidget(resizeToolButton);
   resizeToolButton->setText("R");
   resizeToolButton->setToolTip(
                   "Resize the Image");
   
   //
   // Size combo box
   //
   imageSizeComboBox = new QComboBox;
   toolbar2Layout->addWidget(imageSizeComboBox);
   imageSizeComboBox->setFixedHeight(imageSizeComboBox->sizeHint().height());
   imageSizeComboBox->insertItem(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_25,  " 25%");
   imageSizeComboBox->insertItem(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_50,  " 50%");
   imageSizeComboBox->insertItem(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_75,  " 75%");
   imageSizeComboBox->insertItem(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_100, "100%");
   imageSizeComboBox->insertItem(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_125, "125%");
   imageSizeComboBox->insertItem(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_150, "150%");
   imageSizeComboBox->insertItem(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_200, "200%");
   imageSizeComboBox->insertItem(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_400, "400%");
   imageSizeComboBox->insertItem(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_800, "800%");
   imageSizeComboBox->setFixedSize(imageSizeComboBox->sizeHint());
   imageSizeComboBox->setToolTip( "Use this control to change\n"
                                    "the image's viewing size.");
   imageSizeComboBox->hide();
                                    
   //
   // pack toolbar together
   //
   toolbar2Layout->setSpacing(5);
   //toolbar2->setFixedHeight(toolbar2->sizeHint().height());
   toolbar2Layout->setStretchFactor(annotateToolButton, 0);
   toolbar2Layout->setStretchFactor(cropToolButton, 0);
   toolbar2Layout->setStretchFactor(resizeToolButton, 0);
   toolbar2Layout->setStretchFactor(imageSizeComboBox, 0);
   QLabel* rightToolbar2Label = new QLabel(" ");
   toolbar2Layout->addWidget(rightToolbar2Label);
   toolbar2Layout->setStretchFactor(rightToolbar2Label, 1000);
   
   //
   // the image viewer
   //
   imageViewer = new GuiImageEditorWidget(this, 
                                          GuiImageEditorWidget::POPUP_MENU_EDIT_ONLY);
   imageSizeComboBox->setCurrentIndex(imageViewer->getViewingSize());
   QObject::connect(imageSizeComboBox, SIGNAL(activated(int)),
                    imageViewer, SLOT(slotViewingSizeMenu(int)));
   dialogLayout->addWidget(imageViewer);
   QObject::connect(imageViewer, SIGNAL(signalGeometryChanged()),
                    this, SLOT(slotImageSizeChanged()));
   
   //
   // Connect the toolbar buttons to the image editor
   //
   QObject::connect(annotateToolButton, SIGNAL(clicked()),
                    imageViewer, SLOT(slotAddText()));
   QObject::connect(cropToolButton, SIGNAL(clicked()),
                    imageViewer, SLOT(slotCropImage()));
   QObject::connect(resizeToolButton, SIGNAL(clicked()),
                    imageViewer, SLOT(slotResizeImage()));

   //
   // Close Button
   //
   QHBoxLayout* buttons = new QHBoxLayout;
   dialogLayout->addLayout(buttons);
   
   QPushButton* close = new QPushButton("Close");
   close->setAutoDefault(false);
   close->setFixedSize(close->sizeHint());
   QObject::connect(close, SIGNAL(clicked()),
                    this, SLOT(close()));
   buttons->addWidget(close);
   
   dialogLayout->setStretchFactor(toolbar1Layout, 0);
   dialogLayout->setStretchFactor(toolbar2Layout, 0);
   dialogLayout->setStretchFactor(imageViewer, 0);
   dialogLayout->setStretchFactor(buttons, 0);
//   dialogLayout->setResizeMode(QLayout::Minimum);
   
   updateWindow();
}

/**
 * The Destructor.
 */
GuiImageEditorWindow::~GuiImageEditorWindow()
{
  // theMainWindow->removeImageViewingWindow(this);
}

/**
 * called to open an image file.
 */
void 
GuiImageEditorWindow::slotOpenImageFile()
{
   static GuiImageFileOpenSaveDialog* sd = NULL;
   if (sd == NULL) {
      sd = new GuiImageFileOpenSaveDialog(this, GuiImageFileOpenSaveDialog::DIALOG_MODE_OPEN_FILES);
      //QObject::connect(sd, SIGNAL(signalImageHasBeenSaved()),
      //                 this, SLOT(slotImageHasBeenSaved()));
   }
   sd->show();
   sd->activateWindow();
}

/**
 * called to inform editor that an image has been saved.
 */
void 
GuiImageEditorWindow::slotImageHasBeenSaved()
{
   const int numImages = theMainWindow->getBrainSet()->getNumberOfImageFiles();
   if (numImages > 0) {
      imageSelectionComboBox->blockSignals(true);
      imageSelectionComboBox->setCurrentIndex(numImages - 1);
      imageSelectionComboBox->blockSignals(false);
      slotImageSelectionComboBox(numImages - 1);
   }
}
      
/**
 * called to save an image file.
 */
void 
GuiImageEditorWindow::slotSaveImageFile()
{
   ImageFile* imageFile = imageViewer->getImageFile();
   if (imageFile != NULL) {
      static GuiImageFileOpenSaveDialog* sd = NULL;
      if (sd == NULL) {
         sd = new GuiImageFileOpenSaveDialog(this, 
                                             GuiImageFileOpenSaveDialog::DIALOG_MODE_SAVE_LOADED_IMAGE);
         sd->setDefaultImageForSaving(imageFile);
      }
      sd->show();
      sd->activateWindow();
   }
}
      
/**
 * called to print an image.
 */
void 
GuiImageEditorWindow::slotPrintImage()
{
   ImageFile* imageFile = imageViewer->getImageFile();
   if (imageFile != NULL) {
      QPrinter printer;
      QPrintDialog pd(&printer, this);
      if (pd.exec() == QPrintDialog::Accepted) {
         QImage image = *(imageFile->getImage());

         QPainter painter(&printer);
         painter.drawImage(0, 0, image);
      }
   }
}
      
/**
 * called when an image is selected.
 */
void 
GuiImageEditorWindow::slotImageSelectionComboBox(int item)
{
   displayImage(item);
   imageSizeComboBox->blockSignals(true);
   imageSizeComboBox->setCurrentIndex(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_100);
   imageSizeComboBox->blockSignals(false);
   imageViewer->setViewingSize(GuiImageEditorWidget::VIEWING_IMAGE_SIZE_100);
}
      
/**
 * Update the window as image files may have changed.
 */
void 
GuiImageEditorWindow::updateWindow()
{
   const int numImages = theMainWindow->getBrainSet()->getNumberOfImageFiles();
   if (numImages <= 0) {
      close();
   }
   ImageFile* imageFile = imageViewer->getImageFile();

   int defaultItem = -1;

   imageSelectionComboBox->blockSignals(true);
   imageSelectionComboBox->clear();
   for (int i = 0; i < numImages; i++) {
      ImageFile* img = theMainWindow->getBrainSet()->getImageFile(i);
      imageSelectionComboBox->addItem(FileUtilities::basename(img->getFileName()));
      if (img == imageFile) {
         defaultItem = i;
      }
   }
   
   if (defaultItem < 0) {
      imageFile = NULL;
      if (numImages > 0) {
         defaultItem = 0;
      }
   }
   
   if ((defaultItem >= 0) && (defaultItem < imageSelectionComboBox->count())) {
      imageSelectionComboBox->setCurrentIndex(defaultItem);
      if (imageFile == NULL) {
         imageViewer->setImageFile(theMainWindow->getBrainSet()->getImageFile(defaultItem));
      }      
   }
   imageSelectionComboBox->blockSignals(false);
}

/**
 * display the image in the window
 */
void 
GuiImageEditorWindow::displayImage(const int imageNumberIn)
{
   imageViewer->setImageFile(theMainWindow->getBrainSet()->getImageFile(imageNumberIn));
   //const int windowX = std::max(x(), 10);
   //const int windowY = std::max(y(), 10);

   //
   // Allows dialog to resize to fit image and don't let it change position
   //   
   //move(windowX, windowY);
   //adjustSize();
   //move(windowX, windowY);
   slotImageSizeChanged();
}      
      
/**
 * called when image size changed.
 */
void 
GuiImageEditorWindow::slotImageSizeChanged()
{
   //
   // Allows dialog to resize to fit image and don't let it change position
   //   
   imageViewer->adjustSize();
   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   setMinimumSize(sizeHint());
   setMaximumSize(sizeHint());
   updateGeometry();
   resize(sizeHint());
   adjustSize();
}
      
/**
 * save scene.
 */
SceneFile::SceneClass 
GuiImageEditorWindow::saveScene()
{     
   SceneFile::SceneClass sc("GuiImageEditorWindow");
      
/*
   const ImageFile* img = imageViewer->getImageFile();
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
      switch (imageViewer->getViewingSize()) {
         case GuiImageEditorWidget::VIEWING_IMAGE_SIZE_25:
            sizeString = "25";
            break;
         case GuiImageEditorWidget::VIEWING_IMAGE_SIZE_50:
            sizeString = "50";
            break;
         case GuiImageEditorWidget::VIEWING_IMAGE_SIZE_75:
            sizeString = "75";
            break;
         case GuiImageEditorWidget::VIEWING_IMAGE_SIZE_100:
            sizeString = "100";
            break;
         case GuiImageEditorWidget::VIEWING_IMAGE_SIZE_125:
            sizeString = "125";
            break;
         case GuiImageEditorWidget::VIEWING_IMAGE_SIZE_150:
            sizeString = "150";
            break;
         case GuiImageEditorWidget::VIEWING_IMAGE_SIZE_200:
            sizeString = "200";
            break;
         case GuiImageEditorWidget::VIEWING_IMAGE_SIZE_400:
            sizeString = "400";
            break;
         case GuiImageEditorWidget::VIEWING_IMAGE_SIZE_800:
            sizeString = "800";
            break;
      }
      
      if (sizeString.empty() == false) {
         sc.addSceneInfo(SceneFile::SceneInfo("ImageScale", sizeString));
      }
   }
*/   
   return sc;
}

/**
 * show scene.
 */
void 
GuiImageEditorWindow::showScene(const SceneFile::SceneClass /*sc*/)
{
/*
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
      
   if ((imageName.empty() == false) &&
       (geometry[2] > 0)) {
      int imageNumber = -1;
      for (int j = 0; j < theMainWindow->getBrainSet()->getNumberOfImageFiles(); j++) {
         ImageFile* img = theMainWindow->getBrainSet()->getImageFile(j);
         if (FileUtilities::basename(img->getFileName()) == imageName) {
            imageNumber = j;
            break;
         }
      }
      
      if (imageNumber >= 0) {
         GuiImageEditorWidget::VIEWING_IMAGE_SIZE sz = GuiImageEditorWidget::VIEWING_IMAGE_SIZE_100;
         if (sizeString == "25") {
            sz = GuiImageEditorWidget::VIEWING_IMAGE_SIZE_25;
         }
         else if (sizeString == "50") {
            sz = GuiImageEditorWidget::VIEWING_IMAGE_SIZE_50;
         }
         else if (sizeString == "75") {
            sz = GuiImageEditorWidget::VIEWING_IMAGE_SIZE_75;
         }
         else if (sizeString == "100") {
            sz = GuiImageEditorWidget::VIEWING_IMAGE_SIZE_100;
         }
         else if (sizeString == "125") {
            sz = GuiImageEditorWidget::VIEWING_IMAGE_SIZE_125;
         }
         else if (sizeString == "150") {
            sz = GuiImageEditorWidget::VIEWING_IMAGE_SIZE_150;
         }
         else if (sizeString == "200") {
            sz = GuiImageEditorWidget::VIEWING_IMAGE_SIZE_200;
         }
         else if (sizeString == "400") {
            sz = GuiImageEditorWidget::VIEWING_IMAGE_SIZE_400;
         }
         else if (sizeString == "800") {
            sz = GuiImageEditorWidget::VIEWING_IMAGE_SIZE_800;
         }
         
         if (imageNumber >= 0) {
            displayImage(imageNumber);
         }
         
         imageSizeComboBox->setCurrentIndex(sz);
         imageViewer->setViewingSize(sz);

         move(geometry[0], geometry[1]);
         //resize(geometry[2], geometry[3]);         

         updateWindow();

         move(geometry[0], geometry[1]);
         //resize(geometry[2], geometry[3]);         
      }
   }
*/
}

//********************************************************************************
//********************************************************************************
//********************************************************************************

/**
 * constructor.
 */
GuiImageEditorWidget::GuiImageEditorWidget(QWidget *parent, 
                                     const POPUP_MENU popupMenuStyleIn,
                                     Qt::WFlags f)
   : QWidget(parent, f)
{
   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   
   viewingImageSize = VIEWING_IMAGE_SIZE_100;
   imageFile = NULL;
   popupMenuStyle = popupMenuStyleIn;
   
   addTextDialog = NULL;
   setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
   croppingLinesValid = false;
   if (popupMenuStyle != POPUP_MENU_NONE) {
      this->setToolTip(
                    "Press the right mouse button\n"
                    "(control key + mouse click\n"
                    "on Macs) for menu.");
   }
}

/**
 * destructor.
 */
GuiImageEditorWidget::~GuiImageEditorWidget()
{
}

/**
 * set the image.
 */
void 
GuiImageEditorWidget::setImageFile(ImageFile* img)
{
   imageFile = img;
   windowSizeChanged();
}
      
/**
 * get size hint.
 */
QSize 
GuiImageEditorWidget::sizeHint() const
{
   QSize sz;
   
   int w, h;
   getImageViewingWidthAndHeight(w, h);
   
   sz.setWidth(std::max(w, 100));
   sz.setHeight(std::max(h, 100));

   return sz;
}

/**
 * get the QImage of the ImageFile being displayed.
 */
QImage* 
GuiImageEditorWidget::getImageBeingDisplayed()
{
   if (theMainWindow->getBrainSet()->getImageFileValid(imageFile)) {
      return imageFile->getImage();
   }
   return NULL;
}      

/**
 * get the QImage of the ImageFile being displayed.
 */
const QImage* 
GuiImageEditorWidget::getImageBeingDisplayed() const
{
   if (theMainWindow->getBrainSet()->getImageFileValid(imageFile)) {
      return imageFile->getImage();
   }
   return NULL;
}      

/**
 * Get the image viewing width and height.
 */
void 
GuiImageEditorWidget::getImageViewingWidthAndHeight(int& w, int& h) const
{
   w = 10;
   h = 10;
   
   const QImage* image = getImageBeingDisplayed();
   if (image != NULL) {
      float width = image->width();
      float height = image->height();
      
      switch (viewingImageSize) {
         case VIEWING_IMAGE_SIZE_25:
            width  *= 0.25;
            height *= 0.25;
            break;
         case VIEWING_IMAGE_SIZE_50:
            width  *= 0.50;
            height *= 0.50;
            break;
         case VIEWING_IMAGE_SIZE_75:
            width  *= 0.75;
            height *= 0.75;
            break;
         case VIEWING_IMAGE_SIZE_100:
            width  *= 1.0;
            height *= 1.0;
            break;
         case VIEWING_IMAGE_SIZE_125:
            width  *= 1.25;
            height *= 1.25;
            break;
         case VIEWING_IMAGE_SIZE_150:
            width  *= 1.50;
            height *= 1.50;
            break;
         case VIEWING_IMAGE_SIZE_200:
            width  *= 2.00;
            height *= 2.00;
            break;
         case VIEWING_IMAGE_SIZE_400:
            width  *= 4.00;
            height *= 4.00;
            break;
         case VIEWING_IMAGE_SIZE_800:
            width  *= 8.00;
            height *= 8.00;
            break;
      }
      
      w = static_cast<int>(width);
      h = static_cast<int>(height);
   }
}

/**
 * paint the widget.
 */
void 
GuiImageEditorWidget::paintEvent(QPaintEvent* /*pe*/)
{
   QImage* image = getImageBeingDisplayed();
   if (image == NULL) {
      return;
   }
   else if (image->isNull()) {
      return;
   }
   
   QPainter painter(this);
   
   //
   // Draw the image including any viewing scaling
   //
   QImage drawnImage = *image;
   int viewWidth, viewHeight;
   getImageViewingWidthAndHeight(viewWidth, viewHeight);
   drawnImage = image->scaled(viewWidth, viewHeight,
                              Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation);
   painter.drawImage(0, 0, drawnImage);
   
   if (croppingLinesValid) {
      QPen pen(QColor(255, 0, 255), 1);
      painter.setPen(pen);
      int minX, maxX, minY, maxY;
      getCropping(minX, maxX, minY, maxY);
      painter.drawLine(minX, minY, maxX, minY);
      painter.drawLine(maxX, minY, maxX, maxY);
      painter.drawLine(maxX, maxY, minX, maxY);
      painter.drawLine(minX, maxY, minX, minY);
   }
   
   if (addTextDialog != NULL) {
      QString text;
      int x, y, rotation;
      QFont theFont;
      QColor fontColor;
      addTextDialog->getTextInfo(text, x, y, rotation, theFont, fontColor);
      QPen pen(fontColor);
      painter.setPen(pen);
      painter.setFont(theFont);
      painter.translate(x, y);
      painter.rotate(rotation);
      painter.drawText(0, 0, text);
      painter.resetMatrix(); 
   }
}      

/**
 * called when mouse pressed.
 */
void 
GuiImageEditorWidget::mousePressEvent(QMouseEvent* me)
{
   croppingLinesValid = false;
   
   const int button = me->button();
   const int modifiers = me->modifiers();
   //std::cout << "Button: " << button << std::endl;
   //std::cout << "Modifiers: " << modifiers << std::endl;
   
   if (button == Qt::LeftButton) {
      if (modifiers == Qt::NoModifier) {
         croppingLines[0] = me->x();
         croppingLines[1] = me->y();
         croppingLines[2] = me->x();
         croppingLines[3] = me->y();
         croppingLinesValid = true;
      }
   }
   update();
}

/**
 * get cropping min/max.
 */
void 
GuiImageEditorWidget::getCropping(int& minX, int& maxX,
                                  int& minY, int& maxY) const
{
   minX = std::min(croppingLines[0], croppingLines[2]);
   maxX = std::max(croppingLines[0], croppingLines[2]);
   minY = std::min(croppingLines[1], croppingLines[3]);
   maxY = std::max(croppingLines[1], croppingLines[3]);
}
                       
/**
 * called when mouse moved.
 */
void 
GuiImageEditorWidget::mouseMoveEvent(QMouseEvent* me)
{
   const int button = me->button();
   const int modifiers = me->modifiers();

   if (croppingLinesValid) {
      if (button == Qt::NoButton) {
         if (modifiers == Qt::NoModifier) {
            croppingLines[2] = me->x();
            croppingLines[3] = me->y();
         }
      }
      update();
   }
}

/**
 * popup the context menu.
 */
void 
GuiImageEditorWidget::contextMenuEvent(QContextMenuEvent* cme)
{
   bool haveEditOptions = false;
   bool haveOpenSaveOptions = false;
   
   switch (popupMenuStyle) {
      case POPUP_MENU_NONE:
         return;
         break;
      case POPUP_MENU_EDIT_ONLY:
         haveEditOptions = true;
         break;
      case POPUP_MENU_EDIT_OPEN_SAVE:
         haveEditOptions = true;
         haveOpenSaveOptions = true;
         break;
   }
   
   menuX = cme->x();
   menuY = cme->y();
   
   QMenu menu(this);
   
   if (haveOpenSaveOptions) {
      menu.addAction("Load Image...", this, SLOT(slotLoadImage()));
      menu.addAction("Save Image...", this, SLOT(slotSaveImage()));
   }
   if (haveEditOptions) {
      if (haveOpenSaveOptions) {
         menu.addSeparator();
      }
      menu.addAction("Add Text...", this, SLOT(slotAddTextPopupMenu()));
      menu.addAction("Crop Image...", this, SLOT(slotCropImage()));
      menu.addAction("Scale Image...", this, SLOT(slotResizeImage()));
   }
      
   menu.exec(QCursor::pos());
}      

/**
 * called when a viewing size is selected.
 */
void 
GuiImageEditorWidget::slotViewingSizeMenu(int id)
{
   setViewingSize(static_cast<VIEWING_IMAGE_SIZE>(id));
}
      
/**
 * add text.
 */
void 
GuiImageEditorWidget::addText(const int x, const int y)
{
   QImage* image = getImageBeingDisplayed();
   if (image == NULL) {
      return;
   }
   if (viewingImageSize != VIEWING_IMAGE_SIZE_100) {
      QApplication::beep();
      GuiMessageBox::critical(this, "ERROR", 
                            "The image must be at 100% viewing\n"
                            "scale for adding text.", "OK");
      return;
   }
   
   addTextDialog = new GuiImageEditorWidgetAddTextDialog(this, x, y);
   QObject::connect(addTextDialog, SIGNAL(signalTextUpdated()),
                    this, SLOT(update()));
   if (addTextDialog->exec() == QDialog::Accepted) {
      update();
      *image = QPixmap::grabWidget(this).toImage();
      imageFile->setModified();
      
      GuiFilesModified fm;
      fm.setImagesModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
   }
   delete addTextDialog;
   addTextDialog = NULL;
   update();
}
      
/**
 * called to add text to the image (text X/Y set to popup menu position).
 */
void 
GuiImageEditorWidget::slotAddTextPopupMenu()
{
   addText(menuX, menuY);
}
      
/**
 * called to add text to the image.
 */
void 
GuiImageEditorWidget::slotAddText()
{
   QImage* image = getImageBeingDisplayed();
   if (image == NULL) {
      return;
   }
   addText((image->width() / 2),
           (image->height() / 2));
}
      
/**
 * called to resize the image.
 */
void 
GuiImageEditorWidget::slotResizeImage()
{
   if (theMainWindow->getBrainSet()->getImageFileValid(imageFile)) {
      QImage* image = imageFile->getImage();
      if (image == NULL) {
         return;
      }

      GuiImageEditorWidgetResizeDialog iewrd(this, image->width(), image->height());
      if (iewrd.exec() == QDialog::Accepted) {
         int x, y;
         iewrd.getImageSizes(x, y);
         *image = image->scaled(x, y,
                              Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation);
         imageFile->setModified();

         GuiFilesModified fm;
         fm.setImagesModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL();
      }
   }
   
   windowSizeChanged();
}

/**
 * called to crop the image.
 */
void 
GuiImageEditorWidget::slotCropImage()
{
   QImage* image = getImageBeingDisplayed();
   if (image == NULL) {
      return;
   }
   if (viewingImageSize != VIEWING_IMAGE_SIZE_100) {
      QApplication::beep();
      GuiMessageBox::critical(this, "ERROR", 
                            "The image must be at 100% viewing\n"
                            "scale for cropping.", "OK");
      return;
   }
   
   int minX, maxX, minY, maxY;
   getCropping(minX, maxX, minY, maxY);
   
   if ((croppingLinesValid == false) ||
       (minX == maxX) ||
       (minY == maxY)) {
      GuiMessageBox::critical(this, "ERROR", 
                            "No part of the image is selected.\n"
                            "\n"
                            "After closing this dialog, move the mouse to\n"
                            "a corner of the desired subregion of the image\n"
                            "and, while holding down the left mouse button,\n"
                            "drag the mouse to the opposite corner of the\n"
                            "image subregion and release the mouse.  Next,\n"
                            "press the crop button \"C\" in the toolbar to\n"
                            "crop the image.", "OK");
      return;
   }

   *image = image->copy(minX,
                        minY,
                        maxX - minX + 1,
                        maxY - minY + 1);
   imageFile->setModified();

   GuiFilesModified fm;
   fm.setImagesModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
   croppingLinesValid = false;
   
   windowSizeChanged();
}
      
/**
 * called to load an image.
 */
void 
GuiImageEditorWidget::slotLoadImage()
{
/*
   QFileDialog fd(this, "fd" , true);
   fd.setWindowTitle("Save Image File");
   fd.setFileMode(QFileDialog::ExistingFile);
   fd.setFilter("JPEG FILES (*.jpg *.jpeg)");
   if (fd.exec() == QFileDialog::Accepted) {
      image->load(fd.selectedFiles().at(0));
   }
   windowSizeChanged();
*/
}

/**
 * window size changed.
 */
void
GuiImageEditorWidget::windowSizeChanged()
{
   //
   // Allows dialog to resize to fit image and don't let it change position
   //   
   update();
   updateGeometry();
   adjustSize();
   parentWidget()->updateGeometry();
   parentWidget()->adjustSize();

   emit signalGeometryChanged();
}

/**
 * called to save an image.
 */
void 
GuiImageEditorWidget::slotSaveImage()
{
/*
   QFileDialog fd(this, "fd" , true);
   fd.setWindowTitle("Save Image File");
   fd.setFileMode(QFileDialog::AnyFile);
   fd.setFilter("JPEG FILES (*.jpg *.jpeg)");
   if (fd.exec() == QFileDialog::Accepted) {
      image->save(fd.selectedFiles().at(0), "JPEG");
   }
   
   windowSizeChanged();
*/
}

/**
 * set the viewing size.
 */
void 
GuiImageEditorWidget::setViewingSize(const VIEWING_IMAGE_SIZE vis)
{
   viewingImageSize = vis;
   windowSizeChanged();
}

/**
 * get the viewing size.
 */
GuiImageEditorWidget::VIEWING_IMAGE_SIZE 
GuiImageEditorWidget::getViewingSize() const
{
   return viewingImageSize;
}

//********************************************************************************
//********************************************************************************
//********************************************************************************
/**
 * constructor.
 */
GuiImageEditorWidgetResizeDialog::GuiImageEditorWidgetResizeDialog(QWidget* parent, 
                                                             const int xSize, 
                                                             const int ySize)
   : QtDialog(parent, true)
{
   setWindowTitle("Scaling");
   originalAspect = static_cast<float>(xSize) / static_cast<float>(ySize);
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Grid for spin boxes
   //
   QGridLayout* gridLayout = new QGridLayout;
   dialogLayout->addLayout(gridLayout);
   
   //
   // X size spin boxes
   //
   gridLayout->addWidget(new QLabel("Width "), 0, 0);
   xSizeSpinBox = new QSpinBox;
   xSizeSpinBox->setMinimum(1);
   xSizeSpinBox->setMaximum(std::numeric_limits<int>::max());
   xSizeSpinBox->setSingleStep(1);
   gridLayout->addWidget(xSizeSpinBox, 0, 1);
   xSizeSpinBox->setValue(xSize);
   QObject::connect(xSizeSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotXSizeSpinBox(int)));
                    
   //
   // Y size spin boxes
   //
   gridLayout->addWidget(new QLabel("Height "), 1, 0);
   ySizeSpinBox = new QSpinBox;
   ySizeSpinBox->setMinimum(1);
   ySizeSpinBox->setMaximum(std::numeric_limits<int>::max());
   ySizeSpinBox->setSingleStep(1);
   gridLayout->addWidget(ySizeSpinBox, 1, 1);
   ySizeSpinBox->setValue(ySize);
   QObject::connect(ySizeSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotYSizeSpinBox(int)));
   
   //
   // aspect check box
   //
   aspectCheckBox = new QCheckBox("Maintain Aspect", this);
   aspectCheckBox->setChecked(true);
   dialogLayout->addWidget(aspectCheckBox);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   buttonsLayout->addWidget(okButton);
   okButton->setAutoDefault(false);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   
   //
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   cancelButton->setAutoDefault(false);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   
   //QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * destructor.
 */
GuiImageEditorWidgetResizeDialog::~GuiImageEditorWidgetResizeDialog()
{
}

/**
 * get the image sizes.
 */
void 
GuiImageEditorWidgetResizeDialog::getImageSizes(int& x, int&y) const
{
   x = xSizeSpinBox->value();
   y = ySizeSpinBox->value();
}
      
/**
 * called when x spin box value is changed.
 */
void 
GuiImageEditorWidgetResizeDialog::slotXSizeSpinBox(int value)
{
   if (aspectCheckBox->isChecked()) {
      ySizeSpinBox->blockSignals(true);
      ySizeSpinBox->setValue(static_cast<int>(value / originalAspect));
      ySizeSpinBox->blockSignals(false);
   }
}

/**
 * called when y spin box value is changed.
 */
void 
GuiImageEditorWidgetResizeDialog::slotYSizeSpinBox(int value)
{
   if (aspectCheckBox->isChecked()) {
      xSizeSpinBox->blockSignals(true);
      xSizeSpinBox->setValue(static_cast<int>(value * originalAspect));
      xSizeSpinBox->blockSignals(false);
   }
}

/**
 * called when OK or Cancel button is pressed.
 */
void 
GuiImageEditorWidgetResizeDialog::done(int r)
{
   QDialog::done(r);
}

//********************************************************************************
//********************************************************************************
//********************************************************************************

/**
 * constructor.
 */
GuiImageEditorWidgetAddTextDialog::GuiImageEditorWidgetAddTextDialog(QWidget* parent, 
                                                             const int x, 
                                                             const int y)
   : QtDialog(parent, true)
{
   if (staticDataInitialized == false) {
      staticDataInitialized = true;
      fontColor.setRgb(0, 0, 0);
   }
   setWindowTitle("Add Text");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Choose font push button
   //
   fontPushButton = new QPushButton("Choose Font...");
   fontPushButton->setFont(theFont);
   dialogLayout->addWidget(fontPushButton);
   fontPushButton->setAutoDefault(false);
   QObject::connect(fontPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFontPushButton()));
   fontPushButton->setToolTip(
                 "Pressing this button will display a\n"
                 "dialog for choosing the font for the text.");
                    
   //
   // Choose font color push button
   //
   fontColorPushButton = new QPushButton("Choose Font Color...");
   dialogLayout->addWidget(fontColorPushButton);
   fontColorPushButton->setAutoDefault(false);
   QObject::connect(fontColorPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFontColorPushButton()));
   fontColorPushButton->setToolTip(
                 "Pressing this button will display a\n"
                 "dialog for choosing the color for the text.");
                    
   //
   // Grid for spin boxes
   //
   QGridLayout* grid = new QGridLayout;
   dialogLayout->addLayout(grid);
   
   //
   // Text line edit
   //
   grid->addWidget(new QLabel("Text "), 0, 0);
   textLineEdit = new QLineEdit;
   grid->addWidget(textLineEdit, 0, 1);
   QObject::connect(textLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SIGNAL(signalTextUpdated()));

   //
   // X size spin boxes
   //
   grid->addWidget(new QLabel("X "), 1, 0);
   xSpinBox = new QSpinBox;
   xSpinBox->setMinimum(0);
   xSpinBox->setMaximum(parent->width());
   xSpinBox->setSingleStep(1);
   xSpinBox->setValue(x);
   grid->addWidget(xSpinBox, 1, 1);
   QObject::connect(xSpinBox, SIGNAL(valueChanged(int)),
                    this, SIGNAL(signalTextUpdated()));
                    
   //
   // Y size spin boxes
   //
   grid->addWidget(new QLabel("Y "), 2, 0);
   ySpinBox = new QSpinBox;
   ySpinBox->setValue(ySpinBox->maximum() - y);
   ySpinBox->setMinimum(0);
   ySpinBox->setMaximum(parent->height());
   ySpinBox->setSingleStep(1);
   grid->addWidget(ySpinBox, 2, 1);
   QObject::connect(ySpinBox, SIGNAL(valueChanged(int)),
                    this, SIGNAL(signalTextUpdated()));
   
   //
   // Rotation spin boxes
   //
   grid->addWidget(new QLabel("Rotation "), 3, 0);
   rotationSpinBox = new QSpinBox;
   rotationSpinBox->setValue(0);
   rotationSpinBox->setMinimum(0);
   rotationSpinBox->setMaximum(360);
   rotationSpinBox->setWrapping(true);
   rotationSpinBox->setSingleStep(1);
   grid->addWidget(rotationSpinBox, 3, 1);
   QObject::connect(rotationSpinBox, SIGNAL(valueChanged(int)),
                    this, SIGNAL(signalTextUpdated()));
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   buttonsLayout->addWidget(okButton);
   okButton->setAutoDefault(false);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   okButton->setToolTip(
                 "Pressing this button will apply\n"
                 "the text to the image.  Note: \n"
                 "There is no undo capability.");
   
   //
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   cancelButton->setAutoDefault(false);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   cancelButton->setToolTip(
                 "Pressing this button will close\n"
                 "this dialog and no text will be\n"
                 "added to the image.");
   
   //QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * destructor.
 */
GuiImageEditorWidgetAddTextDialog::~GuiImageEditorWidgetAddTextDialog()
{
}

/**
 * called to select the font color.
 */
void 
GuiImageEditorWidgetAddTextDialog::slotFontColorPushButton()
{
   fontColor = QColorDialog::getColor(fontColor, this);
   //fontColorPushButton->setPaletteForegroundColor(fontColor);
   QPalette palette;
   palette.setColor(fontColorPushButton->foregroundRole(), fontColor);
   fontColorPushButton->setPalette(palette);
/*
   if (fontColorPushButton->eraseColor() == fontColor) {
      if (fontColor.red() < 128) {
         fontColorPushButton->setEraseColor(QColor(255, 255, 255));
      }
      else {
         fontColorPushButton->setEraseColor(QColor(0, 0, 0));
      }
   }
*/
   emit signalTextUpdated();
}
      
/**
 * called to select the font.
 */
void 
GuiImageEditorWidgetAddTextDialog::slotFontPushButton()
{
   bool ok;
   theFont = QFontDialog::getFont(&ok, theFont, this);
   fontPushButton->setFont(theFont);
   emit signalTextUpdated();
}
      
/**
 * get the info about the text.
 */
void 
GuiImageEditorWidgetAddTextDialog::getTextInfo(QString& text,
                                            int& x,
                                            int& y,
                                            int& rotation,
                                            QFont& textFont,
                                            QColor& textColor) const
{
   text = textLineEdit->text();
   x = xSpinBox->value();
   y = ySpinBox->maximum() - ySpinBox->value();
   rotation = rotationSpinBox->value();
   textFont = theFont;
   textColor = fontColor;
}
      
/**
 * called when OK or Cancel button is pressed.
 */
void 
GuiImageEditorWidgetAddTextDialog::done(int r)
{
   QDialog::done(r);
}

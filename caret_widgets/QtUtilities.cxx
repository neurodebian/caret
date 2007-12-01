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

#include <QApplication>
#include <QClipboard>
#include <QDesktopWidget>
#include <QDir>
#include <QImage>
#include <QImageWriter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <QString>
#include <QDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QSizePolicy>
#include <QThread>

#include "FileUtilities.h"
#include "QtRadioButtonSelectionDialog.h"
#include "QtUtilities.h"
#include "StringUtilities.h"
#include "WuQFileDialog.h"

/**
 * Set the sizes of a vector of buttons to the size of the button with
 * the largest width.
 */
void
QtUtilities::makeButtonsSameSize(std::vector<QPushButton*>& buttons)
{
   int maxWidth = -1;
   QSize size;
   for (unsigned int i = 0; i < buttons.size(); i++) {
      if (buttons[i] != NULL) {
         if (buttons[i]->sizeHint().width() > maxWidth) {
            size = buttons[i]->sizeHint();
            maxWidth = size.width();
         }
      }
   }
   
   if (maxWidth > 0) {
      for (unsigned int i = 0; i < buttons.size(); i++) {
         if (buttons[i] != NULL) {
            buttons[i]->setFixedSize(size);
         }
      }
   }
}

/**
 * Set the sizes of buttons to the size of the button with the largest width. 
 */
void 
QtUtilities::makeButtonsSameSize(QPushButton* b1, 
                               QPushButton* b2,
                               QPushButton* b3,
                               QPushButton* b4,
                               QPushButton* b5,
                               QPushButton* b6)
{
   std::vector<QPushButton*> buttons;
   buttons.push_back(b1);
   buttons.push_back(b2);
   buttons.push_back(b3);
   buttons.push_back(b4);
   buttons.push_back(b5);
   buttons.push_back(b6);
   
   if (buttons.size() > 0) {
      makeButtonsSameSize(buttons);
   }
}

/**
 * position a dialog to that it is off of a window.
 */
void 
QtUtilities::positionWindowOffOtherWindow(const QWidget* otherWindow,
                                          QWidget* window)
{
   QDesktopWidget* dt = QApplication::desktop();
   const int dtWidth = dt->width();
   const int dtHeight = dt->height();

   const int otherX = otherWindow->x();
   const int otherY = otherWindow->y();
   const int otherWidth = otherWindow->width();
   const int otherHeight = otherWindow->height();
   
   const int spaceOnSide[4] = {
      otherX, // left
      dtWidth - otherX + otherWidth,  // right
      dtHeight - (otherY + otherHeight), // bottom (0 is top)
      otherY  // top
   };
   
   int maxIndx = 0;
   int maxSpace = spaceOnSide[0];
   for (int i = 1; i < 4; i++) {
      if (spaceOnSide[i] > maxSpace) {
         maxIndx = i;
         maxSpace = spaceOnSide[i];
      }
   }
   
   int newX = 0;
   int newY = 0;
   switch(maxIndx) {
      case 0:
         newX = otherX - window->width();
         newY = otherY;
         break;
      case 1:
         newX = otherX + otherWidth;
         newY = otherY;
         break;
      case 2:
         newX = otherX;
         newY = otherY + otherHeight;
         break;
      case 3:
         newX = otherX;
         newY = otherY - window->height();
         break;         
   }
      
   newX = std::max(newX, 100);
   newX = std::min(newX, (dtWidth - 100));
   newY = std::max(newY, 100);
   newY = std::min(newY, (dtHeight - 100));
   
   window->move(newX, newY);
}                                               

/**
 * Position and set the size of a dialog so that it is over the main window.
 */
void 
QtUtilities::positionAndSetDialogSize(QWidget* mainWindow, QDialog* dialog, const QSize& desiredSize)
{
   QDesktopWidget desktop;
   QRect desktopRect = desktop.screenGeometry();
   
   QRect mainWindowRect = mainWindow->geometry();
   
   const int x = mainWindowRect.left();
   const int y = mainWindowRect.top() + 25;
   const int width = mainWindowRect.width();
   
 //  std::cout << "Main window: "
   
   int xsize = desiredSize.width();
   if ((x +xsize) > desktopRect.right()) {
      xsize = desktopRect.right() - x - 20;
   }
   if (xsize < width) {
      xsize = width;
      if (xsize > 600) {
         xsize = 600;
      }
   }
   
   const int windowHeight = (desktopRect.bottom() - 50);
   int ysize = desiredSize.height();
   if ((y + ysize) > windowHeight) {
      ysize = windowHeight - y - 20;
   }
   if (ysize < 300) {
      ysize = 300;
   }
   
   dialog->setGeometry(x, y, xsize, ysize);
}

QSizePolicy&
QtUtilities::fixedSizePolicy()
{
   static QSizePolicy sizePolicyFixed(QSizePolicy::Fixed, QSizePolicy::Fixed);
   return sizePolicyFixed;
}

/**
 * save the dialog into an image.
 */
void 
QtUtilities::saveWidgetAsImage(QWidget* widget)
{
   std::vector<QString> itemChoices;
   itemChoices.push_back("Copy to Clipboard");
   itemChoices.push_back("Print");
   itemChoices.push_back("Save to File");
   static int defaultItem = 2;
   QtRadioButtonSelectionDialog choiceDialog(widget,
                                             "Capture Image",
                                             "What would you like to do\n"
                                             "with the captured image?",
                                             itemChoices,
                                             defaultItem);
   if (choiceDialog.exec() != QDialog::Accepted) {
      return;
   }
   
   QImage image = QPixmap::grabWidget(widget).toImage();

   defaultItem = choiceDialog.getSelectedItemIndex();
   switch (choiceDialog.getSelectedItemIndex()) {
      case 0:
         saveWidgetAsImageToClipboard(image);
         break;
      case 1:
         saveWidgetAsImageToPrinter(widget, image);
         break;
      case 2:
         saveWidgetAsImageToFile(widget, image);
         break;
   }
}

/**
 * copy an image of the widget to the clipboard.
 */
void 
QtUtilities::saveWidgetAsImageToClipboard(QImage& image)
{
   QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
}


/**
 * print an image of the widget.
 */
void 
QtUtilities::saveWidgetAsImageToPrinter(QWidget* widget, QImage& image)
{
   QPrinter printer;
   QPrintDialog dialog(&printer, widget);
   if (dialog.exec() == QDialog::Accepted) {
      QPainter painter(&printer);
      painter.drawImage(0, 0, image);
   }
}

/**
 * save an image of the widget to file.
 */
void 
QtUtilities::saveWidgetAsImageToFile(QWidget* widget, QImage& image)
{
   static QString previousFileFilter;
   
   //
   // Create the file filters and file extensions and find the jpeg and ppm filters
   //
   QStringList fileFilterList;
   std::vector<QString> fileFilters;
   std::vector<QString> fileExtensions;
   std::vector<QString> fileFormats;
   QString jpegFileFilter;
   QString ppmFileFilter;
   for (int i = 0; i < QImageWriter::supportedImageFormats().count(); i++) {
      QString str = QString(QImageWriter::supportedImageFormats().at(i));
      
      QString filter;
      if ((str == "JPEG") || (str == "jpeg")) {
         filter = QString("%1 Image File (*.jpg *.jpeg)").arg(str);
         fileExtensions.push_back("jpg");
         jpegFileFilter = filter;
      }
      else if ((str == "JPEG") || (str == "JPG")) {
         filter = QString("%1 Image File (*.jpg *.jpeg)").arg(str);
         fileExtensions.push_back("jpg");
         jpegFileFilter = filter;
      }
      else {
         filter = QString("%1 Image File (*.%2)").arg(str).arg(
                                        StringUtilities::makeLowerCase(str));
         fileExtensions.push_back(StringUtilities::makeLowerCase(str));
         if (filter == "PPM") {
            ppmFileFilter = filter;
         }
      }
      fileFilterList << filter;
      fileFilters.push_back(filter);
      fileFormats.push_back(str);
   }
   
   //
   // Create the file dialog
   //
   WuQFileDialog saveImageDialog(widget);
   saveImageDialog.setModal(true);
   saveImageDialog.setWindowTitle(QString("Save Image of ") + widget->objectName());
   saveImageDialog.setFileMode(WuQFileDialog::AnyFile);
   saveImageDialog.setAcceptMode(WuQFileDialog::AcceptSave);
   saveImageDialog.setDirectory(QDir::currentPath());
   
   //
   // Set the file filters
   //
   saveImageDialog.setFilters(fileFilterList);
   if (previousFileFilter.isEmpty() == false) {
      saveImageDialog.selectFilter(previousFileFilter);
   }
   else if (jpegFileFilter.isEmpty() == false) {
      saveImageDialog.selectFilter(jpegFileFilter);
   }
   else if (ppmFileFilter.isEmpty() == false) {
      saveImageDialog.selectFilter(ppmFileFilter);
   }
   
   //
   // Execute the dialog
   //
   if (saveImageDialog.exec() == WuQFileDialog::Accepted) {
      QString name(saveImageDialog.selectedFiles().at(0));
      
      //
      // Get the selected file filter
      //
      previousFileFilter = saveImageDialog.selectedFilter();
      
      //
      // Find the file filter
      //
      int fileFilterIndex = -1;
      for (unsigned int i = 0; i < fileFilters.size(); i++) {
         if (fileFilters[i] == previousFileFilter) {
            fileFilterIndex = i;
            break;
         }
      }
      
      //
      // See if invalid filter index
      //
      if (fileFilterIndex < 0) {
         QString msg("Program Error: invalid file filter index when saving image file.");
         QApplication::beep();
         QMessageBox::critical(widget, "PROGRAM ERROR", msg, "OK");
         return;
      }
      
      if (FileUtilities::filenameExtension(name) != fileExtensions[fileFilterIndex]) {
         name.append(".");
         name.append(fileExtensions[fileFilterIndex]);
      }
      
      if (image.save(name, fileFormats[fileFilterIndex].toAscii().constData(), 100) == false) {
         QApplication::beep();
         QMessageBox::critical(widget, "ERROR", "Unable to save image.", "OK");
      }
   }
}

/**
 * get the image capture key.
 */
bool 
QtUtilities::getImageCaptureKeySelected(QKeyEvent* ke) 
{ 
   if ((ke->key() == Qt::Key_F1) && 
       (ke->modifiers() & Qt::ControlModifier) &&
       (ke->modifiers() & Qt::ShiftModifier)) {
      return true;
   }
   return false;
}
      
/**
 * get the maximum height for a dialog or window.
 */
int 
QtUtilities::getMaximumWindowHeight()
{
   const int mh = QApplication::desktop()->height() - 100;
   return mh;
}

/**
 * print the size of the widget.
 */
void 
QtUtilities::printWidgetSizeHint(QWidget* w, const QString& widgetName)
{
   const QSize sz = w->sizeHint();
   std::cout << "Size of widget " << widgetName.toAscii().constData() << " ("
             << sz.width() << ", " << sz.height() << ")" << std::endl;
}

/**
 * set the maximum height for a widget (gets around bug in QT if minimum size greater than maximum).
 */
void 
QtUtilities::setMaximumHeightToNinetyPercentOfScreenHeight(QWidget* w)
{
   //std::cout << "Min Height: " << minimumHeight() << std::endl;
   const float mh = QtUtilities::getMaximumWindowHeight();
   const int maxHeight = static_cast<int>(mh * 0.90);
   //std::cout << "Max Height: " << mh << std::endl;
   w->setMinimumHeight(maxHeight / 2);
   w->setMaximumHeight(maxHeight);
   w->updateGeometry();
}

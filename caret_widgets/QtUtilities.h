
#ifndef __QT_UTILITIES_H__
#define __QT_UTILITIES_H__

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

#include <QSize>

//
// Avoid include files.
//
class QDialog;
class QImage;
class QKeyEvent;
class QPushButton;
class QSizePolicy;

/// This class contains methods that help simplify Gui Programming with QT.
class QtUtilities {
   public:
      // Set the sizes of a vector of buttons to the size of
      // the button with the largest width. 
      static void makeButtonsSameSize(std::vector<QPushButton*>& buttons);

      // Set the sizes of buttons to the size of the button with the largest width. 
      static void makeButtonsSameSize(QPushButton* b1, 
                                      QPushButton* b2,
                                      QPushButton* b3 = NULL,
                                      QPushButton* b4 = NULL,
                                      QPushButton* b5 = NULL,
                                      QPushButton* b6 = NULL);


      // position a dialog to that it is off of a window
      static void positionWindowOffOtherWindow(const QWidget* otherWindow,
                                               QWidget* window);
                                               
      // position and set the size of a dialog so that it is over the main window
      static void positionAndSetDialogSize(QWidget* mainWindow, QDialog* dialog, 
                                           const QSize& desiredSize);
      
      static QSizePolicy& fixedSizePolicy();

      
      // save an image of the widget
      static void saveWidgetAsImage(QWidget* widget);
      
      /// get the image capture key selected
      static bool getImageCaptureKeySelected(QKeyEvent* ke);
      
      // get the maximum height for a dialog or window
      static int getMaximumWindowHeight();
      
      /// print the size hint of the widget
      static void printWidgetSizeHint(QWidget* w, const QString& widgetName);
   
      /// set the maximum height for a widget (gets around bug in QT if minimum size greater than maximum)
      static void setMaximumHeightToNinetyPercentOfScreenHeight(QWidget* w);
      
   protected:
      // copy an image of the widget to the clipboard
      static void saveWidgetAsImageToClipboard(QImage& image);
      
      // print an image of the widget 
      static void saveWidgetAsImageToPrinter(QWidget* widget, QImage& image);
      
      // save an image of the widget to file
      static void saveWidgetAsImageToFile(QWidget* widget, QImage& image);
      
};

#endif

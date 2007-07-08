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


#ifndef __VE_GUI_COLOR_SELECTION_DIALOG_H__
#define __VE_GUI_COLOR_SELECTION_DIALOG_H__

#include <QString>

#include <QColor>
#include "QtDialog.h"

#include "ColorFile.h"

class QComboBox;
class QSlider;
class QSpinBox;
class QDoubleSpinBox;

/// This modal dialog is used to select a single color.
class GuiColorSelectionDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiColorSelectionDialog(QWidget* parent,
                              const QString& title,
                              const bool showAlpha,
                              const bool showPointSize,
                              const bool showLineSize,
                              const bool showSymbol);
      
      /// Destructor
      ~GuiColorSelectionDialog();
      
      /// get the color information
      void getColorInformation(unsigned char& r, unsigned char& g, unsigned char& b,
                               unsigned char& a,
                               float& pointSize, float& lineSize,
                               ColorFile::ColorStorage::SYMBOL& symbol);
                               
   private slots:
      /// called when the red slider is moved
      void redSliderMovedSlot(int value);
      
      /// called when the green slider is moved
      void greenSliderMovedSlot(int value);
      
      /// called when the blue slider is moved
      void blueSliderMovedSlot(int value);
      
      /// called when the alpha slider is moved
      void alphaSliderMovedSlot(int value);
      
      /// called when dialog closed
      void done(int r);
      
      /// called when color chooser button is pressed.
      void slotColorChooserButton();
   
      /// called when web name push button pressed
      void slotWebHtmlColorNamesPushButton();
      
   private:
      /// initialize the controls using the current "color"
      void initializeControls();
      
      /// update the color of the color swatch
      void updateColorSwatch();
      
      /// the color swatch
      QWidget* colorSwatch;
      
      /// the red spin box
      QSpinBox* redSpinBox;
      
      /// the  red slider
      QSlider* redSlider;
      
      /// the green spin box
      QSpinBox* greenSpinBox;
      
      /// the green slider
      QSlider* greenSlider;
      
      /// the blue spin box
      QSpinBox* blueSpinBox;
      
      /// the blue slider
      QSlider* blueSlider;
      
      /// the alpha spin box
      QSpinBox* alphaSpinBox;
      
      /// the alpha slider
      QSlider* alphaSlider;
      
      /// the line size spin box
      QDoubleSpinBox* lineSizeDoubleSpinBox;
      
      /// the point size spin box
      QDoubleSpinBox* pointSizeDoubleSpinBox;
      
      /// the symbol combo box
      QComboBox* symbolComboBox;
      
      /// the color of the color swatch
      QColor color;
      
      /// the alpha value
      unsigned char colorAlpha;
      
      /// point size
      float pointSize;
      
      /// line size
      float lineSize;
};

#endif // __VE_GUI_COLOR_SELECTION_DIALOG_H__



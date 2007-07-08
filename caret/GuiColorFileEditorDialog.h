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


#ifndef __VE_GUI_COLOR_FILE_EDITOR_DIALOG_H__
#define __VE_GUI_COLOR_FILE_EDITOR_DIALOG_H__

#include <QString>
#include <vector>

#include "QtDialog.h"

class ColorFile;

class QComboBox;
class QListWidget;
class QSlider;
class QSpinBox;
class QDoubleSpinBox;


///Class used for sorting colors alphabetically.
class ColorSorter {
   public:
      /// Constructor
      ColorSorter(const QString& nameIn, const int indexIn) {
         colorName = nameIn;
         colorFileIndex = indexIn;
      }
      
      /// name of color
      QString colorName;
      
      /// index into color file
      int colorFileIndex;
};

#ifdef __COLOR_FILE_EDITOR_MAIN__
   bool operator<(const ColorSorter& c1, const ColorSorter& c2)
   {
      return (c1.colorName < c2.colorName);
   }
#endif // __COLOR_FILE_EDITOR_MAIN__


/// This dialog is used edit a ColorFile.
class GuiColorFileEditorDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiColorFileEditorDialog(QWidget* parent, 
                              ColorFile* cf, 
                              const bool showAlpha,
                              const bool showPointSize,
                              const bool showLineSize,
                              const bool showSymbol,
                              const bool modalFlag /*= false*/,
                              const bool deleteWhenClosed = true);
      
      /// Destructor
      ~GuiColorFileEditorDialog();
      
   signals:
      /// display should be redrawn (apply button was pressed)
      void redrawRequested();
      
   private slots:
      /// called when OK/Canel pressed if dialog is modal
      void done(int r);
      
      /// called when apply button is pressed
      void applySlot();
      
      /// called when an item is selected in the name list box
      void nameListSelectionSlot(int index);
      
      /// called when the red slider is moved
      void redSliderMovedSlot(int value);
      
      /// called when the green slider is moved
      void greenSliderMovedSlot(int value);
      
      /// called when the blue slider is moved
      void blueSliderMovedSlot(int value);
      
      /// called when the alpha slider is moved
      void alphaSliderMovedSlot(int value);
      
      /// called when line size is changed
      void lineSizeChangedSlot(double value);
      
      /// called when line size is changed
      void pointSizeChangedSlot(double value);
      
      /// called when new color button is pressed
      void newColorButtonSlot();
      
      /// called when symbol combo box is selected
      void symbolComboBoxSlot(int value);
      
      /// called when delete color button is pressed
      void deleteColorButtonSlot();
      
      /// called whne button is pressed
      void slotColorChooserButton();
      
      /// called when web name push button pressed
      void slotWebHtmlColorNamesPushButton();
      
   private:
      /// load the name list box
      void loadColorsIntoListBox();
      
      /// update the color of the color swatch
      void updateColorSwatch();
      
      /// the color swatch
      QWidget* colorSwatch;
      
      /// the  value label
      QSpinBox* redValueSpinBox;
      
      /// the  color slider
      QSlider* redSlider;
      
      /// the  value label
      QSpinBox* greenValueSpinBox;
      
      /// the  color slider
      QSlider* greenSlider;
      
      /// the  value label
      QSpinBox* blueValueSpinBox;
      
      /// the  color slider
      QSlider* blueSlider;
      
      /// the alpha value spin box
      QSpinBox* alphaValueSpinBox;
      
      /// the alpha color slider
      QSlider* alphaSlider;
      
      /// the line size spin box
      QDoubleSpinBox* lineSizeSpinBox;
      
      /// the point size spin box
      QDoubleSpinBox* pointSizeSpinBox;
      
      /// listbox for name selection
      QListWidget* nameSelectionListBox;
      
      /// combo box for symbol selection
      QComboBox* symbolComboBox;
      
      /// the color file being edited
      ColorFile* colorFile;
      
      /// sorted colors
      std::vector<ColorSorter> colors;
      
      /// color being edited
      int currentColorFileIndex;
};

#endif // __VE_GUI_COLOR_FILE_EDITOR_DIALOG_H__


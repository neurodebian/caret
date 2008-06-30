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


#ifndef __GUI_SECTION_CONTROL_DIALOG_H__
#define __GUI_SECTION_CONTROL_DIALOG_H__

#include "WuQDialog.h"

class QComboBox;
class QLabel;
class QRadioButton;
class QScrollBar;
class QSpinBox;
class QSlider;

class GuiNodeAttributeColumnSelectionComboBox;

/// Dialog for controlling selected sections.
class GuiSectionControlDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiSectionControlDialog(QWidget* parent);
      
      /// Destructor
      ~GuiSectionControlDialog();
      
      /// Update dialog due to change in sections
      void updateDialog();
      
   private slots:
      /// called when minimum spin box value is changed
      void slotMinimumSpinBox(int value);
      
      /// called when maximum spin box value is changed
      void slotMaximumSpinBox(int value);
      
      /// called when a slider is released
      void sliderReleasedSlot();
      
      /// slot when column selection combo box is changed
      void fileColumnComboBoxSlot(int item);
      
      /// called when minimum slider released
      void minimumSliderMovedSlot(int value);
        
      /// called when maximum slider released
      void maximumSliderMovedSlot(int value);
        
      /// called when a section type radio button is pressed
      void sectionTypeSlot(int item);
      
   private:
      /// section file column combo box;
      GuiNodeAttributeColumnSelectionComboBox* sectionFileColumnComboBox;
      
      /// single section radio button
      QRadioButton* singleSectionRadioButton;

      /// multiple section radio button
      QRadioButton* multipleSectionRadioButton;

      /// all section radio button
      QRadioButton* allSectionRadioButton;
      
      /// minimum section slider
      QSlider* minimumSlider;
      
      /// maximum section slider
      QSlider* maximumSlider;
      
      /// minimum section spin box
      QSpinBox* minimumSpinBox;
      
      /// maximum section spin box
      QSpinBox* maximumSpinBox;
};

#endif

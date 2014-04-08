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

#ifndef __GUI_RESECTION_DIALOG_H__
#define __GUI_RESECTION_DIALOG_H__

#include "WuQDialog.h"

class GuiBrainModelSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;
class QComboBox;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QDoubleSpinBox;

/// Dialog for resectioning
class GuiResectionDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiResectionDialog(QWidget* parent);
      
      /// Destructor
      ~GuiResectionDialog();
      
   public slots:
      /// called when OK or Cancel button pressed
      void done(int r);
      
      /// called when a section file column is selected
      void slotColumnComboBox(int item);
      
      /// called when a how to radio button is selected
      void slotHowToRadioButton();
      
   private:
      /// surface for sectioning combo box
      GuiBrainModelSelectionComboBox* surfaceComboBox;
      
      /// section file column
      GuiNodeAttributeColumnSelectionComboBox* columnComboBox;
      
      /// section file column name line edit
      QLineEdit* columnNameLineEdit;
      
      /// axis combo box
      QComboBox* axisComboBox;
      
      /// number of sections radio button
      QRadioButton* numberOfSectionsRadioButton;
      
      /// number of sections spin box
      QSpinBox* numberOfSectionsSpinBox;
      
      /// thickness radio button
      QRadioButton* thicknessRadioButton;
      
      /// thickness float spin box
      QDoubleSpinBox* thicknessDoubleSpinBox;
};

#endif // __GUI_RESECTION_DIALOG_H__

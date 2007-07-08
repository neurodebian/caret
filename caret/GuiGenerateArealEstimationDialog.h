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

#ifndef __GUI_GENERATE_AREAL_ESTIMATION_DIALOG_H__
#define __GUI_GENERATE_AREAL_ESTIMATION_DIALOG_H__

#include "QtDialog.h"

class QRadioButton;
class QLineEdit;
class QTextEdit;
class QDoubleSpinBox;
class GuiNodeAttributeColumnSelectionComboBox;

/// dialog for creating areal estimation files
class GuiGenerateArealEstimationDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiGenerateArealEstimationDialog(QWidget* parent);
      
      /// Destructor
      ~GuiGenerateArealEstimationDialog();
      
   public slots:
      /// called when OK or Cancel button pressed
      void done(int r);      
      
      /// called when paint name select button pressed
      void slotPaintNamePushButton();
      
      /// called when an areal estimation file column is selected
      void slotArealEstComboBox(int item);
      
   private:
      /// areal estimation file column selection
      GuiNodeAttributeColumnSelectionComboBox* arealEstColumnComboBox;
      
      /// areal estimation column name line edit
      QLineEdit* arealEstColumnNameLineEdit;
      
      /// all nodes radio button
      QRadioButton* nodesAllRadioButton;
      
      /// restrict nodes radio button
      QRadioButton* nodesRestrictRadioButton;
      
      /// paint column combo box
      GuiNodeAttributeColumnSelectionComboBox* paintTypeComboBox;
      
      /// paint name line edit
      QLineEdit* paintNameLineEdit;
      
      /// border file uncertainty radio button
      QRadioButton* borderFileUncertaintyRadioButton;
      
      /// border uncertainty override radio button
      QRadioButton* borderOverrideUncertaintyRadioButton;
      
      /// border override float spin box
      QDoubleSpinBox* borderOverrideDoubleSpinBox;
      
      /// long name line edit
      QLineEdit* longNameLineEdit;
      
      /// comment text edit
      QTextEdit* commentTextEdit;
};

#endif // __GUI_GENERATE_AREAL_ESTIMATION_DIALOG_H__


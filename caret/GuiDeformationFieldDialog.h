
#ifndef __GUI_DEFORMATION_FIELD_DIALOG_H__
#define __GUI_DEFORMATION_FIELD_DIALOG_H__

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

#include "WuQDialog.h"

class GuiBrainModelSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;
class QGroupBox;
class QLineEdit;
class QRadioButton;

/// dialog for creating deformation fields
class GuiDeformationFieldDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiDeformationFieldDialog(QWidget* parent);
      
      /// destructor
      ~GuiDeformationFieldDialog();
      
   protected slots:
      /// called when dialog is closed
      void done(int r);
      
      /// called when a column is selected
      void slotDeformationFileColumnComboBox(int item);
      
      /// called when pushbutton is pressed
      void slotIndivTopoFilePushButton();
      
      /// called when pushbutton is pressed
      void slotIndivCoordFilePushButton();
      
      /// called when pushbutton is pressed
      void slotIndivDeformedCoordFilePushButton();
      
      /// called when a mode button is selected
      void slotModeButtonGroup();
      
   protected:
      /// mode type
      enum MODE_TYPE {
         MODE_TYPE_NONE,
         MODE_TYPE_INDIV_TO_ATLAS,
         MODE_TYPE_ATLAS_TO_INDIV
      };
      
      /// the dialog mode
      MODE_TYPE dialogMode;
      
      /// atlas surface combo box
      GuiBrainModelSelectionComboBox* atlasSurfaceComboBox;
      
      /// line edit
      QLineEdit* indivTopoFileLineEdit;
      
      /// line edit
      QLineEdit* indivCoordFileLineEdit;
      
      /// line edit
      QLineEdit* indivDeformedCoordFileLineEdit;
      
      /// surface combo box
      GuiBrainModelSelectionComboBox* surfaceComboBox;
      
      /// deformed surface combo box
      GuiBrainModelSelectionComboBox* deformedSurfaceComboBox;
      
      /// column combo box
      GuiNodeAttributeColumnSelectionComboBox* deformationFileColumnComboBox;
      
      /// column name line edit
      QLineEdit* columnNameLineEdit;
      
      /// atlas to indiv radio button
      QRadioButton* dialogModeAtlasIndivRadioButton;
      
      /// indiv to atlas radio button
      QRadioButton* dialogModeIndivAtlasRadioButton;
      
      /// atlas to indiv items
      QGroupBox* atlasIndivGroupBox;
      
      /// indiv to atlas items
      QGroupBox* indivAtlasGroupBox;
};

#endif //  __GUI_DEFORMATION_FIELD_DIALOG_H__


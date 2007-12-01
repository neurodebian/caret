
#ifndef __GUI_CELL_AND_FOCI_ATTRIBUTE_ASSIGNMENT_DIALOG_H__
#define __GUI_CELL_AND_FOCI_ATTRIBUTE_ASSIGNMENT_DIALOG_H__

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

#include "QtDialogModal.h"

class GuiBrainModelSelectionComboBox;
class CellFile;
class QCheckBox;
class QRadioButton;

/// dialog for cell and foci attribute assignment
class GuiCellAndFociAttributeAssignmentDialog : public QtDialogModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiCellAndFociAttributeAssignmentDialog(QWidget* parent, 
                                 const bool fociFlag);
                                 
      // destructor
      ~GuiCellAndFociAttributeAssignmentDialog();
   
   protected:
      // called when ok/cancel pressed
      void done(int r);
            
      // create the surface section
      QWidget* createSurfaceSection();
      
      // create the cell/foci section
      QWidget* createCellFociSection(const QString& typeString);
                
      // create the assignement options section
      QWidget* createAssignmentSection();
      
      // create the paint section
      QWidget* createPaintSection();
      
      // determine if a check box is shown and checked
      bool checked(const QCheckBox* cb) const;
      
      /// append to current values 
      QCheckBox* appendToCurrentValuesCheckBox;
      
      /// ignore "?" entries
      QCheckBox* ignoreQuestionEntriesCheckBox;
      
      /// area radio button
      QRadioButton* areaRadioButton;
      
      /// geography radio button
      QRadioButton* geographyRadioButton;
      
      /// paint name check boxes
      std::vector<QCheckBox*> paintNameCheckBoxes;
      
      /// file type string (cell or foci)
      QString typeString;
      
      /// foci flag
      bool fociFlag;
      
      /// combo box for left hem surface selection
      GuiBrainModelSelectionComboBox* leftHemSelectionComboBox;
      
      /// left hem check box
      QCheckBox* leftHemSelectionCheckBox;
      
      /// combo box for right hem surface selection
      GuiBrainModelSelectionComboBox* rightHemSelectionComboBox;
      
      /// right hem check box
      QCheckBox* rightHemSelectionCheckBox;
};

#endif // __GUI_CELL_AND_FOCI_ATTRIBUTE_ASSIGNMENT_DIALOG_H__


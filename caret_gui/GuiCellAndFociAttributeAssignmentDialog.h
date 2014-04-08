
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

#include "WuQDialog.h"

class GuiBrainModelSelectionComboBox;
class CellFile;
class QAbstractButton;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QSpinBox;
class QVBoxLayout;

/// dialog for cell and foci attribute assignment
class GuiCellAndFociAttributeAssignmentDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiCellAndFociAttributeAssignmentDialog(QWidget* parent, 
                                 const bool fociFlag);
                                 
      // destructor
      ~GuiCellAndFociAttributeAssignmentDialog();
      
      // update the dialog
      void updateDialog();
   
   protected slots:
      // called when a button pressed
      void slotPushButton(QAbstractButton* buttonPressed);
      
   protected:
      // create the surface section
      QWidget* createSurfaceSection();
      
      // create the assignement options section
      QWidget* createAssignmentSection();
      
      // create the paint section
      QWidget* createPaintSection();
      
      // update the paint column section
      void updatePaintColumnSection();
      
      // determine if a check box is shown and checked
      bool checked(const QCheckBox* cb) const;
      
      /// perform assignment
      void performAssignment();
      
      /// perform assignment using algorithm
      void performAssignmentUsingAlgorithm();
      
      /// assignment method combo box
      QComboBox* assignmentMethodComboBox;
      
      /// assignment attribute combo box
      QComboBox* assignmentAttributeComboBox;
      
      /// paint name check boxes
      std::vector<QCheckBox*> paintNameCheckBoxes;
      
      /// ignore "?" entries
      QCheckBox* ignorePaintQuestionEntriesCheckBox;
      
      /// layout for paint name checkboxes
      QVBoxLayout* paintNameCheckBoxesLayout;
      
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
      
      /// combo box for cerebellum surface selection
      GuiBrainModelSelectionComboBox* cerebellumSelectionComboBox;
      
      /// cerebellum check box
      QCheckBox* cerebellumSelectionCheckBox;
      
      /// maximum distance of focus from surface double spin box
      QDoubleSpinBox* maximumDistanceDoubleSpinBox;
      
      /// the dialog button box
      QDialogButtonBox* dialogButtonBox;
      
      /// attribute ID Spin Box
      QSpinBox* attributeIDSpinBox;
};

#endif // __GUI_CELL_AND_FOCI_ATTRIBUTE_ASSIGNMENT_DIALOG_H__


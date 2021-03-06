

#ifndef __GUI_VECTORS_FROM_COORDINATES_SUBTRACTION_DIALOG_H__
#define __GUI_VECTORS_FROM_COORDINATES_SUBTRACTION_DIALOG_H__

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
class QComboBox;
class QLineEdit;

/// dialog for creating vectors by subracting coordinate files
class GuiShapeOrVectorsFromCoordinateSubtractionDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// mode of dialog
      enum MODE {
         /// Generating shape data
         MODE_SHAPE,
         /// Generating vector data
         MODE_VECTOR
      };
      
      // constructor
      GuiShapeOrVectorsFromCoordinateSubtractionDialog(QWidget* parent,
                                                       const MODE modeIn);
      
      // destructor
      ~GuiShapeOrVectorsFromCoordinateSubtractionDialog();
         
   protected:
      // called when OK or Cancel buttons pressed
      void done(int r);
      
      // surface "A" combo box
      GuiBrainModelSelectionComboBox* surfaceAComboBox;

      // surface "B" combo box
      GuiBrainModelSelectionComboBox* surfaceBComboBox;
      
      // the vector column name
      QLineEdit* columnNameLineEdit;
      
      // the vector column comment
      QLineEdit* columnCommentLineEdit;
      
      // shape difference mode combo box
      QComboBox* shapeDiffModeComboBox;

      // mode of the dialog
      MODE mode;
};

#endif // __GUI_VECTORS_FROM_COORDINATES_SUBTRACTION_DIALOG_H__


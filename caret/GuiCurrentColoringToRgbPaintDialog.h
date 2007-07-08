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


#ifndef __GUI_CURRENT_COLORING_TO_RGB_PAINT_DIALOG_H__
#define __GUI_CURRENT_COLORING_TO_RGB_PAINT_DIALOG_H__

#include "QtDialog.h"

class GuiNodeAttributeColumnSelectionComboBox;
class QLineEdit;

/// Dialog for copying current coloring to RGB paint
class GuiCurrentColoringToRgbPaintDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiCurrentColoringToRgbPaintDialog(QWidget* parent);
      
      /// destructor
      ~GuiCurrentColoringToRgbPaintDialog();
      
   private slots:
      /// called when an rgb paint column is selected
      void slotRgbPaintColumnSelection(int col);
      
      /// called when dialog is being closed
      void done(int r);
      
   private:
      ///  column name line edit
      QLineEdit* columnNameLineEdit;
      
      /// rgb paint column selection;
      GuiNodeAttributeColumnSelectionComboBox* rgbColumnComboBox;
      
      /// comment line edit
      QLineEdit* commentLineEdit;
};

#endif // __GUI_CURRENT_COLORING_TO_RGB_PAINT_DIALOG_H__


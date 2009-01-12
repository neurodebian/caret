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

#ifndef __GUI_NODE_ATTRIBUTE_FILE_CLEAR_RESET_DIALOG_H__
#define __GUI_NODE_ATTRIBUTE_FILE_CLEAR_RESET_DIALOG_H__

#include "WuQDialog.h"

#include "GuiFilesModified.h"
#include "GuiNodeFileType.h"

class GuiNodeAttributeColumnSelectionComboBox;
class QRadioButton;

/// dialog for clearing a node attribute file, clearing a column, or resetting a column
class GuiNodeAttributeFileClearResetDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiNodeAttributeFileClearResetDialog(QWidget* parent,
                                           const GUI_NODE_FILE_TYPE nodeFileTypeIn);
      
      /// Destructor
      ~GuiNodeAttributeFileClearResetDialog();
      
   private slots:
      /// called when apply button pressed
      void slotApplyButton();
      
   private:
      /// update the dialog
      void updateDialog();
      
      /// type of node file
      GUI_NODE_FILE_TYPE nodeFileType;

      /// file changed mask
      GuiFilesModified fileChanged;
      
      /// clear column combo box
      GuiNodeAttributeColumnSelectionComboBox* clearColumnComboBox;
      
      /// remove column combo box
      GuiNodeAttributeColumnSelectionComboBox* removeColumnComboBox;
      
      /// clear file radio button
      QRadioButton* clearFileRadioButton;
      
      /// clear column radio button
      QRadioButton* clearColumnRadioButton;
      
      /// remove column radio button
      QRadioButton* removeColumnRadioButton;
      
};

#endif // __GUI_NODE_ATTRIBUTE_FILE_CLEAR_RESET_DIALOG_H__


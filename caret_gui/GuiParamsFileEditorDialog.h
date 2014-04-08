
#ifndef __GUI_PARAMS_FILE_EDITOR_DIALOG_H__
#define __GUI_PARAMS_FILE_EDITOR_DIALOG_H__

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

class QGridLayout;
class QLineEdit;
class QScrollArea;

/// Dialog for editing a parameters file
class GuiParamsFileEditorDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiParamsFileEditorDialog(QWidget* parent);
      
      /// Destructor
      ~GuiParamsFileEditorDialog();
      
      /// update the dialog
      void updateDialog();
      
   protected slots:
      /// called when add new parameter button is pressed
      void slotAddNewParameterButton(); 
      
      /// called when apply button is pressed
      void slotApplyButton();
      
      /// called when close button is pressed
      void slotCloseButton();
      
   protected:
      /// widget for the parameters
      QWidget* paramsWidget;
      
      /// grid layout for paramsWidget
      QGridLayout* paramsGridLayout;
      
      /// scrollview for the parameters
      QScrollArea* paramsEntryScrollView;
      
      /// storage for param labels
      std::vector<QLineEdit*> keyLineEdits;

      /// storage for param values
      std::vector<QLineEdit*> valueLineEdits;

};

#endif // __GUI_PARAMS_FILE_EDITOR_DIALOG_H__


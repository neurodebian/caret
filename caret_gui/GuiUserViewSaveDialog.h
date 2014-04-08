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

#ifndef __GUI_USER_VIEW_SAVE_DIALOG_H__
#define __GUI_USER_VIEW_SAVE_DIALOG_H__

#include "WuQDialog.h"

class QCheckBox;
class QLineEdit;

/// class for saving user views
class GuiUserViewSaveDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiUserViewSaveDialog(QWidget* parent);
      
      /// Destructor
      ~GuiUserViewSaveDialog();
      
   private:
      /// called when OK or Cancel button pressed.
      void done(int r);
      
      /// view name line edit
      QLineEdit* viewNameLineEdit;
      
      /// save rotation check box
      QCheckBox* saveRotationCheckBox;
      
      /// save scaling check box
      QCheckBox* saveScalingCheckBox;
      
      /// save translation check box
      QCheckBox* saveTranslationCheckBox;
};

#endif // __GUI_USER_VIEW_SAVE_DIALOG_H__


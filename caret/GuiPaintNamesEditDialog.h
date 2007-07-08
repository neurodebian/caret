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

#ifndef __GUI_PAINT_NAMES_EDIT_DIALOG_H__
#define __GUI_PAINT_NAMES_EDIT_DIALOG_H__

#include <QString>
#include <vector>

#include "QtDialog.h"

class QLineEdit;

/// Dialog for editing paint names
class GuiPaintNamesEditDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiPaintNamesEditDialog(QWidget* parent);
      
      /// Deconstructor
      ~GuiPaintNamesEditDialog();
      
   private slots:

      
   private:
      /// called when dialog is being closed
      void done(int r);
      
      /// the name line edit
      std::vector<QLineEdit*> paintNameLineEdits;
      
      /// the paint names
      std::vector<QString> oldPaintNames;
};

#endif // __GUI_PAINT_NAMES_EDIT_DIALOG_H__


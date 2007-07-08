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


#ifndef __GUI_MULTIPLE_INPUT_DIALOG_H__
#define __GUI_MULTIPLE_INPUT_DIALOG_H__

#include <QString>
#include <vector>

#include "QtDialog.h"

class QLineEdit;

/// This dialog contains multiple rows of label and line edit boxes
class GuiMultipleInputDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMultipleInputDialog(QWidget* parent, 
                             const QString& caption,
                             const std::vector<QString>& labels);
                             
      /// Destructor
      ~GuiMultipleInputDialog();
      
      /// Set a line edit box with an integer 
      void setLineEdit(const int index, const int value);
      
      /// Set a line edit box with a float
      void setLineEdit(const int index, const float value, const int precision = 3);
      
      /// Set a line edit box with an string
      void setLineEdit(const int index, const QString& value);
      
      /// get the integer value in a line edit box
      void getLineEdit(const int index, int& value) const;
      
      /// get the float value in a line edit box
      void getLineEdit(const int index, float& value) const;
      
      /// get the string value in a line edit box
      void getLineEdit(const int index, QString& value) const;
      
   private:
      /// the line edits for all of the rows
      std::vector<QLineEdit*> lineEdits;
      
};

#endif  // __GUI_MULTIPLE_INPUT_DIALOG_H__


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

#ifndef __QT_MULTIPLE_INPUT_DIALOG_H__
#define __QT_MULTIPLE_INPUT_DIALOG_H__

#include <QString>
#include <vector>

#include "WuQDialog.h"

class QLineEdit;

/// class that creates a modal dialog for input of multiple items
class QtMultipleInputDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      QtMultipleInputDialog(QWidget* parent,
                            const QString& title,
                            const QString& instructions,
                            const std::vector<QString>& rowLabels,
                            const std::vector<QString>& rowValues,
                            const bool enableCancelButton = true,
                            const bool modalIn = true,
                            Qt::WindowFlags f = 0);
                            
      /// destructor
      ~QtMultipleInputDialog();
      
      /// get values as strings
      void getValues(std::vector<QString>& values) const;
      
      /// get values as integers
      void getValues(int values[]) const;
      
      /// get values as floats
      void getValues(float values[]) const;
      
      /// get values as doubles
      void getValues(double values[]) const;
      
      /// get values as integers in a vector
      void getValues(std::vector<int>& values) const;
      
      /// get values as floats in a vector
      void getValues(std::vector<float>& values) const;
      
      /// get values as doubles in a vector
      void getValues(std::vector<double>& values) const;
      
   signals:
      /// emitted when apply button pressed (non-modal only)
      void signalApplyPressed();
            
   private:
      /// value line edits
      std::vector<QLineEdit*> valueLineEdits;
      
};

#endif // __QT_MULTIPLE_INPUT_DIALOG_H__


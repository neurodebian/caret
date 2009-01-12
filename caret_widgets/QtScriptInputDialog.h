
#ifndef __QT_INPUT_DIALOG_H__
#define __QT_INPUT_DIALOG_H__

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

#include <QDialog>

class QLineEdit;

/// dialog for input data with optional file selection button
class QtScriptInputDialog : public QDialog {
   Q_OBJECT
   
   public:
      // constructor
      QtScriptInputDialog(QWidget* parent,
                    const QString& messageText,
                    const bool showFileSelectionPushButtonFlag = false,
                    Qt::WindowFlags f = 0);
      
      // constructor
      ~QtScriptInputDialog();
      
      // get text entered by user
      QString getInputText() const;
      
   protected slots:
      // called when file push button is selected
      void slotFilePushButton();
      
   protected:    
      // the line edit
      QLineEdit* lineEdit;
      
};

#endif // __QT_INPUT_DIALOG_H__


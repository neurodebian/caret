
#ifndef __QT_DIALOG_MODAL_H__
#define __QT_DIALOG_MODAL_H__

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

#include "QtDialog.h"

class QVBoxLayout;

/// abstract class for a modal dialog with OK and Cancel buttons.  User may want to
/// override QDialog::done(int r) to do error checking of dialog contents. 
class QtDialogModal : public QtDialog {
   Q_OBJECT
   
   public:
      // constructor
      QtDialogModal(QWidget* parent,
                    Qt::WFlags f = 0);
                    
      // destructor
      virtual ~QtDialogModal();
      
   signals:
      /// help button pressed (user must process this signal)
      void signalHelpButtonPressed();
      
   protected slots:
      // set the OK button enabled
      void setOkButtonEnabled(bool enableIt);
      
   protected:
      /// get the layout for the user's dialog widgets
      QVBoxLayout* getDialogLayout() { return usersLayout; }
      
      // show the help button
      void showHelpButton(const bool showIt);
      
   private:
      /// layout for the dialog
      QVBoxLayout* usersLayout;

      /// OK push button
      QPushButton* okButton;
      
      /// help push button
      QPushButton* helpButton;
};

#endif // __QT_DIALOG_MODAL_H__


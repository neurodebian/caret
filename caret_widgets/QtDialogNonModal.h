
#ifndef __QT_DIALOG_NON_MODAL_H__
#define __QT_DIALOG_NON_MODAL_H__

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

class QPushButton;
class QVBoxLayout;

/// base class for non-modal dialogs
class QtDialogNonModal : public QtDialog {
   Q_OBJECT
   
   public:
      // constructor
      QtDialogNonModal(QWidget* parent = 0,
                       Qt::WFlags f = 0);
                       
      // destructor
      virtual ~QtDialogNonModal();
      
   signals:
      /// apply button pressed (user must process this signal)
      void signalApplyButtonPressed();
      
      /// close button pressed (user must process this signal)
      void signalCloseButtonPressed();

      /// help button pressed (user must process this signal)
      void signalHelpButtonPressed();
      
      
   protected:
      /// get the layout for the user's dialog widgets
      QVBoxLayout* getDialogLayout() { return usersLayout; }
      
      /// get the apply push button
      QPushButton* getApplyPushButton() { return applyButton; }
      
      /// get the close push button
      QPushButton* getClosePushButton() { return closeButton; }
      
      /// show the help button
      void showHelpButton(const bool showIt);
      
      /// resize the buttons
      void resizeButtons();
      
   private:
      /// layout for the dialog
      QVBoxLayout* usersLayout;
      
      /// apply push button
      QPushButton* applyButton;
      
      /// close push button
      QPushButton* closeButton;
      
      /// help push button
      QPushButton* helpButton;
};

#endif // __QT_DIALOG_NON_MODAL_H__


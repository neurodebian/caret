
#ifndef __CARET_TIPS_DIALOG_H__
#define __CARET_TIPS_DIALOG_H__

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

#include "CaretTips.h"
#include "QtDialogNonModal.h"

class QCheckBox;
class QPushButton;
class QTextEdit;

/// dialog for displaying caret tips
class GuiCaretTipsDialog : public QtDialogNonModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretTipsDialog(QWidget* parent);

      // destructor
      ~GuiCaretTipsDialog();
      
   protected slots:
      // called when previous tip button pressed
      void slotPreviousTipPushButton();
      
      // called when next tip button pressed
      void slotNextTipPushButton();
      
      // called when more info button pressed
      void slotMoreInfoPushButton();
      
      // called when tips at startup toggled
      void slotShowTipsAtStartupCheckBox(bool showTips);
      
   protected:
      /// show a tip
      void showTip(const int tipNumber);
      
      /// text edit for tips
      QTextEdit* tipsTextEdit;
      
      /// more info push button
      QPushButton* moreInfoTipPushButton;
      
      /// the caret tips
      CaretTips caretTips;
      
      /// show tips at caret startup
      QCheckBox* showTipsAtCaretStartupCheckBox;
      
};

#endif // __CARET_TIPS_DIALOG_H__

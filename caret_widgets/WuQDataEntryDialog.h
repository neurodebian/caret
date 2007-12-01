
#ifndef __WU_Q_DATA_ENTRY_DIALOG_H__
#define __WU_Q_DATA_ENTRY_DIALOG_H__

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
#include <QStringList>

class QDialogButtonBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QListWidget;

/// class for a modal data entry dialog
class WuQDataEntryDialog : public QDialog {
   Q_OBJECT
   
   public:
      // constructor
      WuQDataEntryDialog(QWidget* parent = 0,
                         Qt::WindowFlags f = 0);
                         
      // destructor
      ~WuQDataEntryDialog();
      
      // add line edit
      QLineEdit* addLineEditWidget(const QString& labelText,
                                     const QString& defaultText = "");
      
      // add list box
      QListWidget* addListWidget(const QString& labelText,
                                 const QStringList& listBoxItems);
      
      // hide the cancel button
      void hideCancelButton();
      
      // set text at top of dialog (text is automatically wrapped)
      void setTextAtTop(const QString& s);
      
   protected:
      // override to verify data after OK button pressed if subclassing this dialog
      virtual bool dataEnteredIsValid();

   private slots:
      // called when OK button pressed
      void slotOKButtonPressed();
      
   private:      
      /// widgets in dialog
      QVector<QWidget*> widgets;
      
      /// layout for widgets in dialog
      QGridLayout* widgetGridLayout;
      
      /// the dialog's ok/cancel buttons
      QDialogButtonBox* buttonBox;
  
      /// label for text at dialog top
      QLabel* textAtTopLabel;
};

#endif // __WU_Q_DATA_ENTRY_DIALOG_H__

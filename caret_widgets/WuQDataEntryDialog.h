
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

#include <QStringList>
#include <QVariant>

#include "WuQDialog.h"

class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QListWidget;
class QSpinBox;

/// class for a modal data entry dialog
class WuQDataEntryDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      WuQDataEntryDialog(QWidget* parent = 0,
                         Qt::WindowFlags f = 0);
                         
      // destructor
      ~WuQDataEntryDialog();
      
      // add widget to next available row in the dialog
      QWidget* addWidget(const QString& labelText,
                         QWidget* widget);
                     
      // add widgets to the next available row in the dialog
      void addWidgetsToNextRow(QWidget* leftColumnWidget,
                               QWidget* rightColumnWidget);
                               
      // add a check box
      QCheckBox* addCheckBox(const QString& text,
                             const bool defaultValue = false);
      
      // add a combo box
      QComboBox* addComboBox(const QString& labelText,
                             const QStringList& comboBoxItems,
                             const QList<QVariant>* comboBoxItemsUserData = NULL);
                       
      // add line edit
      QLineEdit* addLineEditWidget(const QString& labelText,
                                     const QString& defaultText = "");
      
      // add list box
      QListWidget* addListWidget(const QString& labelText,
                                 const QStringList& listBoxItems);
      
      // add spin box
      QSpinBox* addSpinBox(const QString& labelText,
                           const int defaultValue,
                           const int minimumValue = -10000,
                           const int maximumValue =  10000,
                           const int singleStep = 1);
                            
      // add double spin box
      QDoubleSpinBox* addDoubleSpinBox(const QString& labelText,
                                       const float defaultValue,
                                       const float minimumValue = -10000000.0,
                                       const float maximumValue =  10000000.0,
                                       const float singleStep = 1.0,
                                       const int numberOfDecimals = 3);
                            
      // hide the cancel button
      void hideCancelButton();
      
      // set text at top of dialog 
      void setTextAtTop(const QString& s,
                        const bool wrapTheText);
      
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

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

#include <iostream>

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include "WuQDataEntryDialog.h"

/**
 * constructor.
 */
WuQDataEntryDialog::WuQDataEntryDialog(QWidget* parent,
                                       Qt::WindowFlags f)
   : QDialog(parent, f)
{
   //
   // Layout for user's widgets
   //
   widgetGridLayout = new QGridLayout;
   
   //
   // Labels for text at top
   //
   textAtTopLabel = new QLabel;
   textAtTopLabel->setWordWrap(true);
   
   //
   // Button box for dialog's buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                      QDialogButtonBox::Cancel);
   QObject::connect(buttonBox, SIGNAL(accepted()),
                    this, SLOT(slotOKButtonPressed()));
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(reject()));
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(textAtTopLabel);
   dialogLayout->addLayout(widgetGridLayout);
   dialogLayout->addWidget(buttonBox);
}
                   
/**
 * destructor.
 */
WuQDataEntryDialog::~WuQDataEntryDialog()
{
}

/**
 * called when OK button pressed.
 */
void 
WuQDataEntryDialog::slotOKButtonPressed()
{
   if (dataEnteredIsValid()) {
      accept();
   }
}

/**
 * hide the cancel button.
 */
void 
WuQDataEntryDialog::hideCancelButton()
{
   QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
   if (cancelButton != NULL) {
      buttonBox->removeButton(cancelButton);
      delete cancelButton;
   }
}
      
/**
 * override to verify data after OK button pressed.
 */
bool 
WuQDataEntryDialog::dataEnteredIsValid()
{
   return true;
}

/**
 * set text at top of dialog (text is automatically wrapped).
 */
void 
WuQDataEntryDialog::setTextAtTop(const QString& s)
{
   textAtTopLabel->setText(s);
}
      
/**
 * add line edit.
 */
QLineEdit* 
WuQDataEntryDialog::addLineEditWidget(const QString& labelText,
                                      const QString& defaultText)
{
   //
   // Create line edit
   //
   QLineEdit* le = new QLineEdit;
   le->setText(defaultText);

   //
   // Keep pointer to widget
   //
   widgets.push_back(le);
   
   //
   // add widget to layout
   //
   const int rowNumber = widgetGridLayout->rowCount();
   widgetGridLayout->addWidget(new QLabel(labelText), rowNumber, 0);
   widgetGridLayout->addWidget(le, rowNumber, 1);
   
   return le;
}
      
/*
 * add list box.
 */
QListWidget* 
WuQDataEntryDialog::addListWidget(const QString& labelText,
                                  const QStringList& listBoxItems)
{
   //
   // Create and populate list widget
   //
   QListWidget* lw = new QListWidget;
   lw->addItems(listBoxItems);
   
   //
   // Keep pointer to widget
   //
   widgets.push_back(lw);
   
   //
   // add widget to layout
   //
   const int rowNumber = widgetGridLayout->rowCount();
   widgetGridLayout->addWidget(new QLabel(labelText), rowNumber, 0);
   widgetGridLayout->addWidget(lw, rowNumber, 1);
   
   return lw;
}

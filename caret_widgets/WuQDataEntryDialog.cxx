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

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include "WuQDataEntryDialog.h"

/**
 * constructor.
 */
WuQDataEntryDialog::WuQDataEntryDialog(QWidget* parent,
                                       Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   //
   // Layout for user's widgets
   //
   widgetGridLayout = new QGridLayout;
   
   //
   // Labels for text at top
   //
   textAtTopLabel = new QLabel;
   
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
WuQDataEntryDialog::setTextAtTop(const QString& s,
                                 const bool wrapTheText)
{
   textAtTopLabel->setText(s);
   textAtTopLabel->setWordWrap(wrapTheText);
}
      
/**
 * add widgets to the next available row in the dialog.
 */
void 
WuQDataEntryDialog::addWidgetsToNextRow(QWidget* leftColumnWidget,
                                        QWidget* rightColumnWidget)
{
   //
   // add widgets to the next row
   //
   const int rowNumber = widgetGridLayout->rowCount();
   if (leftColumnWidget != NULL) {
      widgetGridLayout->addWidget(leftColumnWidget, rowNumber, 0);
   }
   if (rightColumnWidget != NULL) {
      widgetGridLayout->addWidget(rightColumnWidget, rowNumber, 1);
   }
}

/**
 * add widget to next available row in the dialog.
 */
QWidget* 
WuQDataEntryDialog::addWidget(const QString& labelText,
                              QWidget* widget)
{
   //
   // Create the label
   //
   QLabel* label = new QLabel(labelText);
   
   //
   // Keep pointer to widget
   //
   widgets.push_back(widget);
   
   //
   // add widget to layout
   //
   addWidgetsToNextRow(label, widget);
   
   return widget;
}
                     
/**
 * add a check box.
 */
QCheckBox* 
WuQDataEntryDialog::addCheckBox(const QString& text,
                                const bool defaultValue)
{
   //
   // Create check box
   //
   QCheckBox* cb = new QCheckBox(text);
   cb->setChecked(defaultValue);

   //
   // Keep pointer to widget
   //
   widgets.push_back(cb);
   
   //
   // add widget to both columns of layout
   //
   const int rowNumber = widgetGridLayout->rowCount();
   widgetGridLayout->addWidget(cb, rowNumber, 0, 1, 2, Qt::AlignLeft);
   
   return cb;
}
                             
/**
 * add a combo box.
 */
QComboBox* 
WuQDataEntryDialog::addComboBox(const QString& labelText,
                                const QStringList& comboBoxItems,
                                const QList<QVariant>* comboBoxItemsUserData)
{
   //
   // Create combo box
   //
   QComboBox* comboBox = new QComboBox;
   for (int i = 0; i < comboBoxItems.size(); i++) {
      QVariant userData;
      if (comboBoxItemsUserData != NULL) {
         if (i < comboBoxItemsUserData->size()) {
            userData = (*comboBoxItemsUserData).at(i);
         }
      }
      comboBox->addItem(comboBoxItems.at(i), 
                        userData);
   }
   
   //
   // Add to dialog
   //
   addWidget(labelText, comboBox);
   
   return comboBox;
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
   // Add to dialog
   //
   addWidget(labelText, le);
   
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
   if (listBoxItems.count() > 0) {
      lw->setCurrentRow(0);
   }
   
   //
   // Add to dialog
   //
   addWidget(labelText, lw);
      
   return lw;
}

/**
 * add spin box.
 */
QSpinBox* 
WuQDataEntryDialog::addSpinBox(const QString& labelText,
                               const int defaultValue,
                               const int minimumValue,
                               const int maximumValue,
                               const int singleStep)
{
   //
   // Create spin box
   //
   QSpinBox* sb = new QSpinBox;
   sb->setMinimum(minimumValue);
   sb->setMaximum(maximumValue);
   sb->setSingleStep(singleStep);
   sb->setValue(defaultValue);
   
   //
   // Add to dialog
   //
   addWidget(labelText, sb);
   
   return sb;
}                            

/**
 * add double spin box.
 */
QDoubleSpinBox* 
WuQDataEntryDialog::addDoubleSpinBox(const QString& labelText,
                                     const float defaultValue,
                                     const float minimumValue,
                                     const float maximumValue,
                                     const float singleStep,
                                     const int numberOfDecimals)
{
   //
   // Create spin box
   //
   QDoubleSpinBox* sb = new QDoubleSpinBox;
   sb->setMinimum(minimumValue);
   sb->setMaximum(maximumValue);
   sb->setSingleStep(singleStep);
   sb->setValue(defaultValue);
   sb->setDecimals(numberOfDecimals);
   
   //
   // Add to dialog
   //
   addWidget(labelText, sb);
   
   return sb;
}                                     

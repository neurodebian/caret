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

#include <QButtonGroup>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QRadioButton>

#include "QtRadioButtonSelectionDialog.h"
#include "QtUtilities.h"

/**
 * Constructor.
 */
QtRadioButtonSelectionDialog::QtRadioButtonSelectionDialog(QWidget* parent,
                                   const QString& title,
                                   const QString& message,
                                   const std::vector<QString>& itemLabels,
                                   const int defaultItem)
   : QtDialog(parent, true)
{
   setWindowTitle(title);

   //
   // layout for dialog
   //
   QVBoxLayout* layout = new QVBoxLayout;
   layout->setMargin(5);
   layout->setSpacing(5);
   setLayout(layout);
   
   //
   // display message
   //
   if (message.isEmpty() == false) {
      layout->addWidget(new QLabel(message));
   }
   
   //
   // radio button group for radio buttons
   //
   radioButtonGroup = new QButtonGroup(this);
   
   //
   // vertical box for radio buttons
   //
   QVBoxLayout* buttonBox = new QVBoxLayout;
   layout->addLayout(buttonBox);
   
   //
   // create the radio buttons
   //
   for (int i = 0; i < static_cast<int>(itemLabels.size()); i++) {
      QRadioButton* rb = new QRadioButton(itemLabels[i]);
      buttonBox->addWidget(rb);
      radioButtonGroup->addButton(rb, i);
   }
   
   //
   // set the default radio button
   //
   if ((defaultItem >= 0) && (defaultItem < radioButtonGroup->buttons().count())) {
      QRadioButton* rb = dynamic_cast<QRadioButton*>(radioButtonGroup->button(defaultItem));
      if (rb != NULL) {
         rb->setChecked(true);
      }
   }
   
   //
   // Add the OK button and connect to accept slot
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(5);
   layout->addLayout(buttonsLayout);
   QPushButton* okButton = new QPushButton("OK");
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));

   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * Destructor.
 */
QtRadioButtonSelectionDialog::~QtRadioButtonSelectionDialog()
{
}

/**
 * Get the selected item index.
 */
int
QtRadioButtonSelectionDialog::getSelectedItemIndex() const
{
   const int itemNum = radioButtonGroup->checkedId();
   return itemNum;
/*
   QButton* selectedButton = radioButtonGroup->selected();
   if (selectedButton != NULL) {
      return radioButtonGroup->id(selectedButton);
   }
   return -1;
*/
}


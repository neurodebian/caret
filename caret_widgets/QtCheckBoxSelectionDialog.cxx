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

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

#include "QtCheckBoxSelectionDialog.h"
#include "QtUtilities.h"

/**
 * Constructor.
 */
QtCheckBoxSelectionDialog::QtCheckBoxSelectionDialog(QWidget* parent,
                                             const QString& title,
                                             const QString& textMessage,
                                             const std::vector<QString>& itemLabels,
                                             const std::vector<bool>& itemChecked)
   : QtDialog(parent, true)
{
   setWindowTitle(title);

   //
   // layout for dialog
   //
   QVBoxLayout* layout = new QVBoxLayout;
   layout->setSpacing(5);
   this->setLayout(layout);
   
   //
   // Label above check boxes
   //
   if (textMessage.isEmpty() == false) {
      layout->addWidget(new QLabel(textMessage, this));
   }
   
   //
   // create the checkboxes 
   //
   for (int i = 0; i < static_cast<int>(itemLabels.size()); i++) {
      QCheckBox* cb = new QCheckBox(itemLabels[i]);
      if (i < static_cast<int>(itemChecked.size())) {
         cb->setChecked(itemChecked[i]);
      }
      checkBoxes.push_back(cb);
      layout->addWidget(cb);
   }
   
   //
   // Add the OK button and connect to accept slot
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   layout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(5);
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
QtCheckBoxSelectionDialog::~QtCheckBoxSelectionDialog()
{
}

/**
 * Get the checkbox selected.
 */
bool
QtCheckBoxSelectionDialog::getCheckBoxStatus(const int index) const
{
   if (index < static_cast<int>(checkBoxes.size())) {
      return checkBoxes[index]->isChecked();
   }
   
   return false;
}


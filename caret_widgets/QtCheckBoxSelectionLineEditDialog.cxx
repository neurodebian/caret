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
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

#include "QtCheckBoxSelectionLineEditDialog.h"

/**
 * Constructor.
 */
QtCheckBoxSelectionLineEditDialog::QtCheckBoxSelectionLineEditDialog(QWidget* parent,
                                             const QString& title,
                                             const QString& textMessage,
                                             const std::vector<bool>& itemChecked,
                                             const std::vector<QString>& lineEditValues)
   : QtDialogModal(parent)
{
   setWindowTitle(title);
   
   //
   // layout for dialog from parent QtDialogModal
   //
   QVBoxLayout* layout = getDialogLayout();
   
   // Label above check boxes
   //
   if (textMessage.isEmpty() == false) {
      layout->addWidget(new QLabel(textMessage, this));
   }
   
   //
   // The grid layout
   //
   QGridLayout* grid = NULL;
   
   //
   // Should a scroll area be used since lots of items in list
   //
   if (itemChecked.size() > 20) {
      QWidget* w = new QWidget;
      grid = new QGridLayout(w);
      QScrollArea* scrollArea = new QScrollArea;
      scrollArea->setWidget(w);
      scrollArea->setWidgetResizable(true);
      layout->addWidget(scrollArea);
   }
   else {   
      grid = new QGridLayout;
      layout->addLayout(grid);
   }   
   //
   // create the checkboxes and line edits
   //
   for (int i = 0; i < static_cast<int>(itemChecked.size()); i++) {
      QCheckBox* cb = new QCheckBox("");
      cb->setChecked(itemChecked[i]);
      checkBoxes.push_back(cb);
      QObject::connect(cb, SIGNAL(toggled(bool)),
                       this, SLOT(slotCheckBoxToggled()));
      
      QLineEdit* le = new QLineEdit;
      if (i < static_cast<int>(lineEditValues.size())) {
         le->setText(lineEditValues[i]);
      }
      lineEdits.push_back(le);
      
      grid->addWidget(cb, i, 0);
      grid->addWidget(le, i, 1);
   }
   
   //
   // Enable/Disable line edits
   //
   slotCheckBoxToggled();
}

/**
 * Destructor.
 */
QtCheckBoxSelectionLineEditDialog::~QtCheckBoxSelectionLineEditDialog()
{
}

/**
 * called when a check box is toggled.
 */
void 
QtCheckBoxSelectionLineEditDialog::slotCheckBoxToggled()
{
   for (unsigned int i = 0; i < checkBoxes.size(); i++) {
      if (i < lineEdits.size()) {
         lineEdits[i]->setEnabled(checkBoxes[i]->isChecked());
      }
   }
}
      
/**
 * Get the checkbox selected.
 */
bool
QtCheckBoxSelectionLineEditDialog::getCheckBoxStatus(const int indx) const
{
   if (indx < static_cast<int>(checkBoxes.size())) {
      return checkBoxes[indx]->isChecked();
   }
   
   return false;
}

/**
 * get line edit value.
 */
QString 
QtCheckBoxSelectionLineEditDialog::getLineEditValue(const int indx) const
{
   if (indx < static_cast<int>(lineEdits.size())) {
      return lineEdits[indx]->text();
   }
   
   return "";
}      


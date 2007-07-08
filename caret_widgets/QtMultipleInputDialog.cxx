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

#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

#include "QtMultipleInputDialog.h"
#include "QtUtilities.h"

/**
 * Constructor.
 * Instructions (if any) are placed at the top of the dialog.
 * The dialog may be modal or non-modal.  If non-model, Qt::WDestructiveClose may be
 * passed for the WFlags to automatically destroy the dialog when the Close button
 * is pressed.
 * enableCancelButton is valid only for a modal dialog.
 */
QtMultipleInputDialog::QtMultipleInputDialog(QWidget* parent,
                                             const QString& title,
                                             const QString& instructions,
                                             const std::vector<QString>& rowLabels,
                                             const std::vector<QString>& rowValues,
                                             const bool enableCancelButton,
                                             const bool modalIn,
                                             Qt::WFlags f)
   : QtDialog(parent, modalIn, f)
{
   setWindowTitle(title);

   QVBoxLayout* layout = new QVBoxLayout;
   layout->setSpacing(3);
   layout->setMargin(3);
   setLayout(layout);
   
   if (instructions.isEmpty() == false) {
      layout->addWidget(new QLabel(instructions, this));
   }
   
   //
   // Grid for labels and line edits
   //
   QGridLayout* grid = new QGridLayout;
   layout->addLayout(grid);
   grid->setSpacing(5);
   
   //
   // create the labels and line edits
   //
   for (unsigned int i = 0; i < rowLabels.size(); i++) {
      grid->addWidget(new QLabel(rowLabels[i]), i, 0);
      QLineEdit* le = new QLineEdit;
      valueLineEdits.push_back(le);
      if (i < rowValues.size()) {
         le->setText(rowValues[i]);
      }
      grid->addWidget(le, i, 1);
   }
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   layout->setSpacing(2);
   layout->addLayout(buttonsLayout);
   
   //
   // is this dialog modal
   //
   if (isModal()) {
      //
      // OK button
      //
      QPushButton* okButton = new QPushButton("OK");    
      buttonsLayout->addWidget(okButton);
      QObject::connect(okButton, SIGNAL(clicked()),
                     this, SLOT(accept()));
   
      //
      // Cancel button connects to QDialogs close() slot.
      //
      if (enableCancelButton) {
         QPushButton* cancelButton = new QPushButton("Cancel");
         buttonsLayout->addWidget(cancelButton);
         QObject::connect(cancelButton, SIGNAL(clicked()),
                        this, SLOT(reject()));
         QtUtilities::makeButtonsSameSize(okButton, cancelButton);
      }
      else {
         okButton->setFixedSize(okButton->sizeHint());
      }                 
   }
   else {
      //
      // Apply button
      //
      QPushButton* applyButton = new QPushButton("Apply");    
      buttonsLayout->addWidget(applyButton);
      applyButton->setAutoDefault(false);
      QObject::connect(applyButton, SIGNAL(clicked()),
                       this, SIGNAL(signalApplyPressed()));
   
      //
      // Close button connects to QDialogs close() slot.
      //
      QPushButton* closeButton = new QPushButton("Close");
      buttonsLayout->addWidget(closeButton);
      closeButton->setAutoDefault(false);
      QObject::connect(closeButton, SIGNAL(clicked()),
                       this, SLOT(close()));
                     
      QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   }
}
                        
/**
 * destructor.
 */
QtMultipleInputDialog::~QtMultipleInputDialog()
{
}

/**
 * get values as strings.
 */
void 
QtMultipleInputDialog::getValues(std::vector<QString>& values) const
{
   values.clear();
   for (unsigned int i = 0; i < valueLineEdits.size(); i++) {
      values.push_back(valueLineEdits[i]->text());
   }
}

/**
 * get values as integers in a vector.
 */
void 
QtMultipleInputDialog::getValues(std::vector<int>& values) const
{
   values.clear();
   for (unsigned int i = 0; i < valueLineEdits.size(); i++) {
      values.push_back(valueLineEdits[i]->text().toInt());
   }
}

/**
 * get values as floats in a vector.
 */
void 
QtMultipleInputDialog::getValues(std::vector<float>& values) const
{
   values.clear();
   for (unsigned int i = 0; i < valueLineEdits.size(); i++) {
      values.push_back(valueLineEdits[i]->text().toFloat());
   }
}

/**
 * get values as doubles in a vector.
 */
void 
QtMultipleInputDialog::getValues(std::vector<double>& values) const
{
   values.clear();
   for (unsigned int i = 0; i < valueLineEdits.size(); i++) {
      values.push_back(valueLineEdits[i]->text().toDouble());
   }
}

/**
 * get values as integers.
 */
void 
QtMultipleInputDialog::getValues(int values[3]) const
{
   for (unsigned int i = 0; i < valueLineEdits.size(); i++) {
      values[i] = valueLineEdits[i]->text().toInt();
   }
}

/**
 * get values as floats.
 */
void 
QtMultipleInputDialog::getValues(float values[3]) const
{
   for (unsigned int i = 0; i < valueLineEdits.size(); i++) {
      values[i] = valueLineEdits[i]->text().toFloat();
   }
}

/**
 * get values as doubles.
 */
void 
QtMultipleInputDialog::getValues(double values[3]) const
{
   for (unsigned int i = 0; i < valueLineEdits.size(); i++) {
      values[i] = valueLineEdits[i]->text().toDouble();
   }
}

      

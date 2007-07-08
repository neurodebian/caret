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


#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QString>

#include "BrainSet.h"
#include "FociUncertaintyToRgbPaintConverter.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "QtUtilities.h"
#include "RgbPaintFile.h"
#include "global_variables.h"

#define __GUI_FOCI_UNCERTAINTY_DIALOG_MAIN__ 
#include "GuiFociUncertaintyLimitsDialog.h"
#undef __GUI_FOCI_UNCERTAINTY_DIALOG_MAIN__
/**
 * Constructor.
 */
GuiFociUncertaintyLimitsDialog::GuiFociUncertaintyLimitsDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle("Uncertainty Limits to RGB Paint");
   
   //
   // Vertical box layout of all items
   //
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(2);
   rows->setSpacing(3);
   
   //
   // Grid for uncertainty limits
   //
   QGridLayout* ulGrid = new QGridLayout;
   ulGrid->setMargin(2);
   ulGrid->setSpacing(5);
   rows->addLayout(ulGrid);
   
   //
   // lower limit
   //
   ulGrid->addWidget(new QLabel("Lower Limit (mm)"), 0, 0, Qt::AlignLeft);
   lowerLimitLineEdit = new QLineEdit;
   ulGrid->addWidget(lowerLimitLineEdit, 0, 1, Qt::AlignLeft);
   
   //
   // middle limit
   //
   ulGrid->addWidget(new QLabel("Middle Limit (mm)"), 1, 0, Qt::AlignLeft);
   middleLimitLineEdit = new QLineEdit;
   ulGrid->addWidget(middleLimitLineEdit, 1, 1, Qt::AlignLeft);
   
   //
   // upper limit
   //
   ulGrid->addWidget(new QLabel("Upper Limit (mm)"), 2, 0, Qt::AlignLeft);
   upperLimitLineEdit = new QLineEdit;
   ulGrid->addWidget(upperLimitLineEdit, 2, 1, Qt::AlignLeft);

   //
   // RGB Paint column selection Layout
   //
   QHBoxLayout* rgbLayout = new QHBoxLayout;
   rows->addLayout(rgbLayout);
   rgbLayout->setSpacing(5);
   
   //
   // RGB Label and column selection
   //
   rgbLayout->addWidget(new QLabel("RGB Paint"));
   rgbPaintSelectionComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                GUI_NODE_FILE_TYPE_RGB_PAINT,
                                                true,
                                                false,
                                                false);
   rgbLayout->addWidget(rgbPaintSelectionComboBox);
   
   //
   // New column name
   //
   const QString columnName("Foci Uncertainty");
   rgbPaintColumnNewName = new QLineEdit;
   rgbPaintColumnNewName->setText(columnName);
   rgbLayout->addWidget(rgbPaintColumnNewName);
   
   //
   // Initialize RGB Paint column selection 
   //
   rgbPaintSelectionComboBox->setCurrentIndex(rgbPaintSelectionComboBox->count() - 1);
   for (int i = 0; i < rgbPaintSelectionComboBox->count(); i++) {
      if (rgbPaintSelectionComboBox->itemText(i) == columnName) {
         rgbPaintSelectionComboBox->setCurrentIndex(i);
         break;
      }
   }
   
   //
   // Initialize uncertainty line edits
   //
   lowerLimitLineEdit->setText(QString::number(lowerLimit, 'f', 1));
   middleLimitLineEdit->setText(QString::number(middleLimit, 'f', 1));
   upperLimitLineEdit->setText(QString::number(upperLimit, 'f', 1));

   //
   //
   // Dialog Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   rows->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK", this);
   okButton->setAutoDefault(true);
   QObject::connect(okButton, SIGNAL(clicked()),
                  this, SLOT(accept()));
   buttonsLayout->addWidget(okButton);
   
   //
   // Close button
   //
   QPushButton* cancelButton = new QPushButton("Cancel", this);
   cancelButton->setAutoDefault(false);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                  this, SLOT(reject()));
   buttonsLayout->addWidget(cancelButton);
   
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * Destructor.
 */
GuiFociUncertaintyLimitsDialog::~GuiFociUncertaintyLimitsDialog()
{
}

/**
 * Called when OK button pressed.
 */
void
GuiFociUncertaintyLimitsDialog::accept()
{
   const int rgbColumn = rgbPaintSelectionComboBox->currentIndex();
   const QString columnName(rgbPaintColumnNewName->text());
   lowerLimit  = lowerLimitLineEdit->text().toFloat();
   middleLimit = middleLimitLineEdit->text().toFloat();
   upperLimit  = upperLimitLineEdit->text().toFloat();
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   QString errorMessage;
   FociUncertaintyToRgbPaintConverter fuc;
   const bool error = fuc.convert(theMainWindow->getBrainSet(), lowerLimit, middleLimit, upperLimit,
                                  rgbColumn, columnName, this, errorMessage);
                                  
   QApplication::restoreOverrideCursor();
   
   if (error) {
      GuiMessageBox::critical(this, "Error", errorMessage, "OK");
      return;
   }
   
   GuiFilesModified fm;
   fm.setRgbPaintModified();
   theMainWindow->fileModificationUpdate(fm);
   
   theMainWindow->speakText("Uncertainty limits completed.", false);
   
   QDialog::accept();
}


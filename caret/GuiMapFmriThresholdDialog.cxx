
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

#include <sstream>

#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QString>
#include <QTextEdit>

#include "GuiMapFmriThresholdDialog.h"
#include "GuiMessageBox.h"
#include "FileUtilities.h"
#include "StringUtilities.h"

/**
 * Constructor.
 */
GuiMapFmriThresholdDialog::GuiMapFmriThresholdDialog(QWidget* parent,
                                                     const QString& volumeFileName)
   : QtDialog(parent, true)
{
   setWindowTitle("Volume Thresholding");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(3);
   dialogLayout->setSpacing(3);
   
   QLabel* nameLabel = new QLabel(FileUtilities::basename(volumeFileName));
   dialogLayout->addWidget(nameLabel);
   
   //
   // Help text edit
   //
   std::ostringstream str;
   str << "Enter the positive and negative "
       << "thresholds for the volume.\n"
       << "These thresholds are used when "
       << "the data is displayed on the surface "
       << "in Caret.  If you do not know the "
       << "thresholds, leave them at zero.  ";
   QTextEdit* helpTextEdit = new QTextEdit;
   helpTextEdit->setPlainText(str.str().c_str());
   helpTextEdit->setReadOnly(true);
   dialogLayout->addWidget(helpTextEdit);
   
   //
   // Pos and neg thresholds
   //
   QLabel* posLabel = new QLabel("Positive ");
   positiveDoubleSpinBox = new QDoubleSpinBox;
   positiveDoubleSpinBox->setMinimum(0.0);
   positiveDoubleSpinBox->setMaximum(100000000.0);
   positiveDoubleSpinBox->setSingleStep(1.0);
   positiveDoubleSpinBox->setDecimals(4);
   positiveDoubleSpinBox->setValue(0.0);
   QLabel* negLabel = new QLabel("Negative ");
   negativeDoubleSpinBox = new QDoubleSpinBox;
   negativeDoubleSpinBox->setMinimum(-100000000.0);
   negativeDoubleSpinBox->setMaximum(0.0);
   negativeDoubleSpinBox->setSingleStep(1.0);
   negativeDoubleSpinBox->setDecimals(4);
   negativeDoubleSpinBox->setValue(0.0);
   
   //
   // Layout for thresholds
   //
   QGridLayout* threshGridLayout = new QGridLayout;
   threshGridLayout->addWidget(posLabel, 0, 0);
   threshGridLayout->addWidget(positiveDoubleSpinBox, 0, 1);
   threshGridLayout->addWidget(negLabel, 1, 0);
   threshGridLayout->addWidget(negativeDoubleSpinBox, 1, 1);
   threshGridLayout->setColumnStretch(0, 0);
   threshGridLayout->setColumnStretch(1, 0);
   threshGridLayout->setColumnStretch(2, 100);
   dialogLayout->addLayout(threshGridLayout);
   
   //
   // layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   okButton->setFixedSize(okButton->sizeHint());
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
}
                          
/**
 * Destructor.
 */
GuiMapFmriThresholdDialog::~GuiMapFmriThresholdDialog()
{
}

/**
 * get the threshold values.
 */
void 
GuiMapFmriThresholdDialog::getThresholds(float& negThresh, float& posThresh) const
{
   negThresh = negativeDoubleSpinBox->value();
   posThresh = positiveDoubleSpinBox->value();
}
      
/**
 * overrides parent and called when OK pressed.
 */
void 
GuiMapFmriThresholdDialog::done(int r)
{
   float negThresh, posThresh;
   getThresholds(negThresh, posThresh);
   
   QString msg;
   if (negThresh > 0.0) {
      msg.append("Negative threshold is greater than zero.");
   }
   if (posThresh < 0.0) {
      if (msg.isEmpty() == false) {
         msg.append("\n");
      }
      msg.append("Positive threshold is less than zero.");
   }
   if (msg.isEmpty() == false) {
      msg.append("\nDo you want to continue ?");
      if (GuiMessageBox::warning(this, "WARNING", msg, "Continue", "Change Values") != 0) {
         return;
      }
   }
   
   QDialog::done(r);
}

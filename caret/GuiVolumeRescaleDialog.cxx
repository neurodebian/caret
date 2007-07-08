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
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>

#include "GuiBrainModelOpenGL.h"
#include "GuiMessageBox.h"
#include "GuiVolumeRescaleDialog.h"
#include "QtRadioButtonSelectionDialog.h"
#include "QtUtilities.h"
#include "VolumeFile.h"

/**
 * Constructor.
 */
GuiVolumeRescaleDialog::GuiVolumeRescaleDialog(QWidget* parent, 
                                               const QString& message,
                                               VolumeFile* volumeFileIn,
                                               const float* outputMinMaxValues)
   : QtDialog(parent, true)
{
   volumeFile = volumeFileIn;
   setWindowTitle("Rescale Voxels");

   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Display message
   //
   if (message.isEmpty() == false) {
      dialogLayout->addWidget(new QLabel(message, this));
   }
   
   //
   // Input values
   //
   QLabel* currentRangeLabel = new QLabel("Current Range ");
   inputMinimumLineEdit = new QLineEdit;
   new QDoubleValidator(inputMinimumLineEdit);
   inputMaximumLineEdit = new QLineEdit;
   new QDoubleValidator(inputMaximumLineEdit);
   float minValue = 0.0, maxValue = 0.0;
   if (volumeFile != NULL) {
      volumeFile->getMinMaxVoxelValues(minValue, maxValue);
   }
   loadInputMinMaxLineEdits(minValue, maxValue);   
   
   //
   // Set input values push button
   //
   QPushButton* setInputValuesPushButton = new QPushButton("Set...");
   setInputValuesPushButton->setAutoDefault(false);
   setInputValuesPushButton->setFixedSize(setInputValuesPushButton->sizeHint());
   QObject::connect(setInputValuesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSetInputValuesPushButton()));
                    
   //
   // output values
   //
   QLabel* outputRangeLabel = new QLabel("Output Range ");
   outputMinimumLineEdit = new QLineEdit;
   new QDoubleValidator(outputMinimumLineEdit);
   outputMaximumLineEdit = new QLineEdit;
   new QDoubleValidator(outputMaximumLineEdit);   
   if (outputMinMaxValues != NULL) {
      minValue = outputMinMaxValues[0];
      maxValue = outputMinMaxValues[1];
   }
   loadOutputMinMaxLineEdits(minValue, maxValue);
   
   //
   // Set output values push button
   //
   QPushButton* setOutputValuesPushButton = new QPushButton("Set...");
   setOutputValuesPushButton->setAutoDefault(false);
   setOutputValuesPushButton->setFixedSize(setOutputValuesPushButton->sizeHint());
   QObject::connect(setOutputValuesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSetOutputValuesPushButton()));
                    
   //
   // Group box and grid for scaling
   //
   QGroupBox* scaleGroupBox = new QGroupBox("Voxel Rescaling");
   dialogLayout->addWidget(scaleGroupBox);
   QGridLayout* scaleGridLayout = new QGridLayout(scaleGroupBox);
   scaleGridLayout->addWidget(currentRangeLabel, 0, 0);
   scaleGridLayout->addWidget(inputMinimumLineEdit, 0, 1);
   scaleGridLayout->addWidget(inputMaximumLineEdit, 0, 2);
   scaleGridLayout->addWidget(setInputValuesPushButton, 0, 3);
   scaleGridLayout->addWidget(outputRangeLabel, 1, 0);
   scaleGridLayout->addWidget(outputMinimumLineEdit, 1, 1);
   scaleGridLayout->addWidget(outputMaximumLineEdit, 1, 2);
   scaleGridLayout->addWidget(setOutputValuesPushButton, 1, 3);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(true);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));

   //
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(true);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));

   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * Destructor.
 */
GuiVolumeRescaleDialog::~GuiVolumeRescaleDialog()
{
}

/**
 * called when set input values button pressed.
 */
void 
GuiVolumeRescaleDialog::slotSetInputValuesPushButton()
{
   chooseValuesDialog(true);
}

/**
 * called when set output values button pressed.
 */
void 
GuiVolumeRescaleDialog::slotSetOutputValuesPushButton()
{
   chooseValuesDialog(false);
}

/**
 * loads the input min/max line edits.
 */
void 
GuiVolumeRescaleDialog::loadInputMinMaxLineEdits(const float minValue, const float maxValue)
{
   inputMinimumLineEdit->setText(QString::number(minValue, 'f', 3));
   inputMaximumLineEdit->setText(QString::number(maxValue, 'f', 3));
}

/**
 * loads the output min/max line edits.
 */
void 
GuiVolumeRescaleDialog::loadOutputMinMaxLineEdits(const float minValue, const float maxValue)
{
   outputMinimumLineEdit->setText(QString::number(minValue, 'f', 3));
   outputMaximumLineEdit->setText(QString::number(maxValue, 'f', 3));
}

/**
 * called when ok/cancel pressed.
 */
void 
GuiVolumeRescaleDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      if (volumeFile != NULL) {
         volumeFile->rescaleVoxelValues(inputMinimumLineEdit->text().toFloat(),
                                        inputMaximumLineEdit->text().toFloat(),
                                        outputMinimumLineEdit->text().toFloat(),
                                        outputMaximumLineEdit->text().toFloat());
      }
      
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::restoreOverrideCursor();
   }
   
   QDialog::done(r);
}

/**
 * choose values selection dialog.
 */
void 
GuiVolumeRescaleDialog::chooseValuesDialog(const bool inputVolumeFlag)
{
   std::vector<QString> labels;
   labels.push_back("Byte - Unsigned ("
                    + QString::number(std::numeric_limits<unsigned char>::min())
                    + " to "
                    + QString::number(std::numeric_limits<unsigned char>::max())
                    + ")");
   labels.push_back("Byte - Signed ("
                    + QString::number(std::numeric_limits<char>::min())
                    + " to "
                    + QString::number(std::numeric_limits<char>::max())
                    + ")");
   labels.push_back("Short - Unsigned ("
                    + QString::number(std::numeric_limits<unsigned short>::min())
                    + " to "
                    + QString::number(std::numeric_limits<unsigned short>::max())
                    + ")");
   labels.push_back("Short - Signed ("
                    + QString::number(std::numeric_limits<short>::min())
                    + " to "
                    + QString::number(std::numeric_limits<short>::max())
                    + ")");
   labels.push_back("Int - Unsigned ("
                    + QString::number(std::numeric_limits<unsigned int>::min())
                    + " to "
                    + QString::number(std::numeric_limits<unsigned int>::max())
                    + ")");
   labels.push_back("Int - Signed ("
                    + QString::number(std::numeric_limits<int>::min())
                    + " to "
                    + QString::number(std::numeric_limits<int>::max())
                    + ")");
   labels.push_back("Float ("
                    + QString::number(-std::numeric_limits<float>::max())
                    + " to "
                    + QString::number(std::numeric_limits<float>::max())
                    + ")");
   labels.push_back("Volume Min/Max");
   labels.push_back("Volume 2% to 98%");
   
   QtRadioButtonSelectionDialog rbsd(this,
                                     "Choose Values",
                                     "Choose Desired Values",
                                     labels,
                                     -1);
   if (rbsd.exec() == QDialog::Accepted) {
      const int item = rbsd.getSelectedItemIndex();
      if (item < 0) {
         GuiMessageBox::critical(this, "ERROR", "No selection made.", "OK");
         return;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      float minValue = 0.0, maxValue = 0.0;
      switch (item) {
         case 0:
            minValue = std::numeric_limits<unsigned char>::min();
            maxValue = std::numeric_limits<unsigned char>::max();
            break;
         case 1:
            minValue = std::numeric_limits<char>::min();
            maxValue = std::numeric_limits<char>::max();
            break;
         case 2:
            minValue = std::numeric_limits<unsigned short>::min();
            maxValue = std::numeric_limits<unsigned short>::max();
            break;
         case 3:
            minValue = std::numeric_limits<short>::min();
            maxValue = std::numeric_limits<short>::max();
            break;
         case 4:
            minValue = std::numeric_limits<unsigned int>::min();
            maxValue = std::numeric_limits<unsigned int>::max();
            break;
         case 5:
            minValue = std::numeric_limits<int>::min();
            maxValue = std::numeric_limits<int>::max();
            break;
         case 6:
            minValue = -std::numeric_limits<float>::max();
            maxValue =  std::numeric_limits<float>::max();
            break;
         case 7:
            if (volumeFile != NULL) {
               volumeFile->getMinMaxVoxelValues(minValue, maxValue);
            }
            break;
         case 8:
            if (volumeFile != NULL) {
               volumeFile->getTwoToNinetyEightPercentMinMaxVoxelValues(minValue, maxValue);
            }
            break;
      }
      
      if (inputVolumeFlag) {
         loadInputMinMaxLineEdits(minValue, maxValue);
      }
      else {
         loadOutputMinMaxLineEdits(minValue, maxValue);
      }
      
      QApplication::restoreOverrideCursor();
   }
}      


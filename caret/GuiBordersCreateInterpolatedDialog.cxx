
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

#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include "BorderColorFile.h"
#include "BrainModelBorderSet.h"
#include "BrainSet.h"
#include "GuiBordersCreateInterpolatedDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorSelectionDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiBordersCreateInterpolatedDialog::GuiBordersCreateInterpolatedDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Create Interpolated Borders.");
   
   //
   // first border label
   //
   const int numberAndNameLength = 30;
   QLabel* firstBorderLabel = new QLabel("Border 1 (left click): ");
   firstBorderNumberAndNameLabel = new QLabel(QString(' ', numberAndNameLength));
   
   //
   // second border label
   //
   QLabel* secondBorderLabel = new QLabel("Border 2 (shift-left click): ");
   secondBorderNumberAndNameLabel = new QLabel(QString(' ', numberAndNameLength));
   
   //
   // Name prefix
   //
   QLabel* namePrefixLabel = new QLabel("Name Prefix");
   namePrefixLineEdit = new QLineEdit;
   namePrefixLineEdit->setText("LANDMARK.Interpolated");
   namePrefixLineEdit->setMinimumWidth(300);
   
   //
   // Sampling
   //
   QLabel* samplingLabel = new QLabel("Sampling");
   samplingDoubleSpinBox = new QDoubleSpinBox;
   samplingDoubleSpinBox->setMinimum(0.0);
   samplingDoubleSpinBox->setMaximum(1000000.0);
   samplingDoubleSpinBox->setSingleStep(1.0);
   samplingDoubleSpinBox->setDecimals(2);
   samplingDoubleSpinBox->setValue(1.0);
   
   //
   // Number of interpolated borders
   //
   QLabel* numberOfInterpolatedBordersLabel = new QLabel("Number of Interpolated Borders");
   numberOfInterplatedBordersSpinBox = new QSpinBox;
   numberOfInterplatedBordersSpinBox->setMinimum(1);
   numberOfInterplatedBordersSpinBox->setMaximum(100000);
   numberOfInterplatedBordersSpinBox->setSingleStep(1);
   numberOfInterplatedBordersSpinBox->setValue(2);
   
   //
   // Row numbers for widgets
   //
   int numberOfRows = 0;
   const int border1Row = numberOfRows++;
   const int border2Row = numberOfRows++;
   const int namePrefixRow = numberOfRows++;
   const int samplingRow = numberOfRows++;
   const int interpolateRow = numberOfRows++;
   
   //
   // Grid layout for items
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(firstBorderLabel, border1Row, 0, 1, 1);
   gridLayout->addWidget(firstBorderNumberAndNameLabel, border1Row, 1, 1, 1);
   gridLayout->addWidget(secondBorderLabel, border2Row, 0, 1, 1);
   gridLayout->addWidget(secondBorderNumberAndNameLabel, border2Row, 1, 1, 1);
   gridLayout->addWidget(namePrefixLabel, namePrefixRow, 0, 1, 1);
   gridLayout->addWidget(namePrefixLineEdit, namePrefixRow, 1, 1, 1);
   gridLayout->addWidget(samplingLabel, samplingRow, 0, 1, 1);
   gridLayout->addWidget(samplingDoubleSpinBox, samplingRow, 1, 1, 1);
   gridLayout->addWidget(numberOfInterpolatedBordersLabel, interpolateRow, 0, 1, 1);
   gridLayout->addWidget(numberOfInterplatedBordersSpinBox, interpolateRow, 1, 1, 1);
   
   //
   // Enable mouse button
   //
   QPushButton* enableMousePushButton = new QPushButton("Enable Mouse");
   enableMousePushButton->setFixedSize(enableMousePushButton->sizeHint());
   enableMousePushButton->setAutoDefault(false);
   QObject::connect(enableMousePushButton, SIGNAL(clicked()),
                    this, SLOT(slotSetMouseModeForThisDialog()));
                    
   //
   // Get the dialogs layout
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addLayout(gridLayout);
   dialogLayout->addWidget(enableMousePushButton);
   dialogLayout->addStretch();
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Apply 
                                                      | QDialogButtonBox::Close);
   dialogLayout->addWidget(buttonBox);
   QPushButton* applyButton = buttonBox->button(QDialogButtonBox::Apply);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyPushButton()));
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(close()));
}

/**
 * destructor.
 */
GuiBordersCreateInterpolatedDialog::~GuiBordersCreateInterpolatedDialog()
{
}

/**
 * called to update a border selection.
 */
void 
GuiBordersCreateInterpolatedDialog::updateBorderSelection(const INTERPOLATE_BORDER interpolateBorderChoice,
                                                          const int borderIndex)
{
   //
   // Get index and name of border
   //
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   const BrainModelBorder* b = bmbs->getBorder(borderIndex);
   const QString labelValue(QString::number(borderIndex)
                            + "  "
                            + b->getName());
                            
   switch (interpolateBorderChoice) {
      case INTERPOLATE_BORDER_1:
         firstBorderIndex = borderIndex;
         firstBorderNumberAndNameLabel->setText(labelValue);
         break;
      case INTERPOLATE_BORDER_2:
         secondBorderIndex = borderIndex;
         secondBorderNumberAndNameLabel->setText(labelValue);
         break;
   }
}
                           
/**
 * called when Apply button pressed.
 */
void 
GuiBordersCreateInterpolatedDialog::slotApplyPushButton()
{
   //
   // Interpolate the borders
   //
   const QString borderName(namePrefixLineEdit->text());
   QString errorMessage;
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->createInterpolatedBorders(theMainWindow->getBrainModelSurface(),
                                   firstBorderIndex,
                                   secondBorderIndex,
                                   borderName,
                                   numberOfInterplatedBordersSpinBox->value(),
                                   samplingDoubleSpinBox->value(),
                                   errorMessage);
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this, "ERROR", errorMessage);
      return;
   }      
   
   //
   // Find the matching color
   //
   bool borderColorMatch = false;
   BorderColorFile* borderColorFile = theMainWindow->getBrainSet()->getBorderColorFile();
   const int borderColorIndex = borderColorFile->getColorIndexByName(borderName, borderColorMatch);
  
   //
   // Border color may need to be created
   //
   bool createBorderColor = false;
   if ((borderColorIndex >= 0) && (borderColorMatch == true)) {
      createBorderColor = false;
   }   
   else if ((borderColorIndex >= 0) && (borderColorMatch == false)) {
      QString msg("Use border color \"");
      msg.append(borderColorFile->getColorNameByIndex(borderColorIndex));
      msg.append("\" for border ");
      msg.append(borderName);
      msg.append(" ?");
      QString noButton("No, define color ");
      noButton.append(borderName);
      if (QMessageBox::information(this, 
                                   "Use Partially Matching Color",
                                   msg, 
                                   (QMessageBox::Yes | QMessageBox::No),
                                        QMessageBox::No) == QMessageBox::No) {
         createBorderColor = true;      
      }
   }   
   else {
      createBorderColor = true;
   }

   if (createBorderColor) {
      QString title("Create Border Color: ");
      title.append(borderName);
      GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this,
                                                                 title,
                                                                 false,
                                                                 false,
                                                                 false,
                                                                 false);
      csd->exec();

      //
      // Add new border color
      //
      float pointSize = 2.0, lineSize = 1.0;
      unsigned char r, g, b, a;
      ColorFile::ColorStorage::SYMBOL symbol;
      csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
      borderColorFile->addColor(borderName, r, g, b, a, pointSize, lineSize, symbol);
   }


   GuiFilesModified fm;
   fm.setBorderModified();
   fm.setBorderColorModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();   
}

/**
 * set the mouse mode for picking borders.
 */
void 
GuiBordersCreateInterpolatedDialog::slotSetMouseModeForThisDialog()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_INTERPOLATE);   
}

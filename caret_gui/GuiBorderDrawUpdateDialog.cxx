
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

#include <QButtonGroup>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include "GuiBorderDrawUpdateDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiBorderDrawUpdateDialog::GuiBorderDrawUpdateDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Draw Border Update");
   
   //-------------------------------------------------------------------------
   //
   // Resampling Double Spin Box
   //
   resamplingDoubleSpinBox = new QDoubleSpinBox;
   resamplingDoubleSpinBox->setMinimum(0.001);
   resamplingDoubleSpinBox->setMaximum(10000000);
   resamplingDoubleSpinBox->setSingleStep(1.0);
   resamplingDoubleSpinBox->setDecimals(2);
   resamplingDoubleSpinBox->setValue(2.0);
   QGroupBox* resamplingGroupBox = new QGroupBox("Resampling");
   QVBoxLayout* resamplingLayout = new QVBoxLayout(resamplingGroupBox);
   resamplingLayout->addWidget(resamplingDoubleSpinBox);
   
   //-------------------------------------------------------------------------
   //
   // Dimension Radio Buttons
   //
   dimensionTwoRadioButton = new QRadioButton("2D (Flat)");
   dimensionThreeRadioButton = new QRadioButton("3D");
   
   //
   // Button Group to keep radio buttons mutually exclusive
   //
   QButtonGroup* dimButtGroup = new QButtonGroup(this);
   dimButtGroup->addButton(dimensionTwoRadioButton);
   dimButtGroup->addButton(dimensionThreeRadioButton);
   
   //
   // Group box and layout for dimension
   //
   QGroupBox* dimensionGroupBox = new QGroupBox("Dimensions");
   QVBoxLayout* dimensionLayout = new QVBoxLayout(dimensionGroupBox);
   dimensionLayout->addWidget(dimensionTwoRadioButton);
   dimensionLayout->addWidget(dimensionThreeRadioButton);
   
   //-------------------------------------------------------------------------
   //
   // Auto Project Radio Buttons
   //
   autoProjectYesRadioButton = new QRadioButton("Yes");
   autoProjectNoRadioButton = new QRadioButton("No");
   
   //
   // Button Group to keep project radio buttons mutually exclusive
   //
   QButtonGroup* projectButtGroup = new QButtonGroup(this);
   projectButtGroup->addButton(autoProjectYesRadioButton);
   projectButtGroup->addButton(autoProjectNoRadioButton);
   
   //
   // Group box and layout for auto project
   //
   QGroupBox* autoProjectGroupBox = new QGroupBox("Auto Project");
   QVBoxLayout* autoProjectLayout = new QVBoxLayout(autoProjectGroupBox);
   autoProjectLayout->addWidget(autoProjectYesRadioButton);
   autoProjectLayout->addWidget(autoProjectNoRadioButton);
   
   //-------------------------------------------------------------------------
   //
   // Mode radio buttons
   //
   extendBorderModeRadioButton = new QRadioButton("Extend From Either End");
   replaceSegmentInBorderModeRadioButton = new QRadioButton("Replace Segment");
   eraseBorderModeRadioButton = new QRadioButton("Erase Segment");
   //
   // Button Group to keep mode radio buttons mutually exclusive
   //
   QButtonGroup* modeButtGroup = new QButtonGroup(this);
   modeButtGroup->addButton(eraseBorderModeRadioButton);
   modeButtGroup->addButton(extendBorderModeRadioButton);
   modeButtGroup->addButton(replaceSegmentInBorderModeRadioButton);
   
   //
   // Group box and layout for mode
   //
   QGroupBox* modeGroupBox = new QGroupBox("Mode");
   QVBoxLayout* modeLayout = new QVBoxLayout(modeGroupBox);
   modeLayout->addWidget(eraseBorderModeRadioButton);
   modeLayout->addWidget(extendBorderModeRadioButton);
   modeLayout->addWidget(replaceSegmentInBorderModeRadioButton);
   
   //-------------------------------------------------------------------------
   //
   // Dialog buttons
   //
   dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Apply |
                                          QDialogButtonBox::Close |
                                          QDialogButtonBox::Help);
   QObject::connect(dialogButtonBox, SIGNAL(clicked(QAbstractButton*)),
                    this, SLOT(slotStandardButtonPressed(QAbstractButton*)));
                                                           
   //-------------------------------------------------------------------------
   //
   // Layout for dialog
   //
   QHBoxLayout* contentLayout = new QHBoxLayout;
   contentLayout->addWidget(dimensionGroupBox);
   contentLayout->addWidget(autoProjectGroupBox);
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(modeGroupBox);
   dialogLayout->addLayout(contentLayout);
   dialogLayout->addWidget(resamplingGroupBox);
   dialogLayout->addWidget(dialogButtonBox);
   
   //-------------------------------------------------------------------------
   //
   // Initialize
   //
   dimensionThreeRadioButton->setChecked(true);
   autoProjectYesRadioButton->setChecked(true);
}

/**
 * destructor.
 */
GuiBorderDrawUpdateDialog::~GuiBorderDrawUpdateDialog()
{
}

/**
 * called when standard button pressed.
 */
void 
GuiBorderDrawUpdateDialog::slotStandardButtonPressed(QAbstractButton* buttonPressed)
{
   if (buttonPressed == dialogButtonBox->button(QDialogButtonBox::Apply)) {
      if (getBorderUpdateMode() == BrainModelBorderSet::UPDATE_BORDER_MODE_NONE) {
         QMessageBox::critical(this, "ERROR", "You must select a mode.");
         return;
      }
      theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_UPDATE);
   }
   else if (buttonPressed == dialogButtonBox->button(QDialogButtonBox::Close)) {
      theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
      close();
   }
   else if (buttonPressed == dialogButtonBox->button(QDialogButtonBox::Help)) {
      theMainWindow->showHelpViewerDialog("dialogs/draw_border_update_dialog.html");
   }
}

/**
 * get the resampling density.
 */
float 
GuiBorderDrawUpdateDialog::getResampling() const 
{ 
   return resamplingDoubleSpinBox->value(); 
}

/**
 * get the 3D flag.
 */
bool 
GuiBorderDrawUpdateDialog::getThreeDimensional() const 
{ 
   return dimensionThreeRadioButton->isChecked(); 
}

/**
 * get the auto project border flag.
 */
bool 
GuiBorderDrawUpdateDialog::getAutoProjectBorder() const 
{ 
   return autoProjectYesRadioButton->isChecked(); 
}
      
/**
 * get the update mode.
 */
BrainModelBorderSet::UPDATE_BORDER_MODE 
GuiBorderDrawUpdateDialog::getBorderUpdateMode() const
{
   BrainModelBorderSet::UPDATE_BORDER_MODE borderMode =
      BrainModelBorderSet::UPDATE_BORDER_MODE_NONE;
      
   if (replaceSegmentInBorderModeRadioButton->isChecked()) {
      borderMode = BrainModelBorderSet::UPDATE_BORDER_MODE_REPLACE_SEGMENT_IN_MIDDLE_OF_BORDER;
   }
   else if (eraseBorderModeRadioButton->isChecked()) {
      borderMode = BrainModelBorderSet::UPDATE_BORDER_MODE_ERASE;
   }
   else if (extendBorderModeRadioButton->isChecked()) {
      borderMode = BrainModelBorderSet::UPDATE_BORDER_MODE_EXTEND_BORDER_FROM_END;
   }
   
   return borderMode;
}

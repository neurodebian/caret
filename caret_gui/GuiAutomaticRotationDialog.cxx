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

#include <limits>
#include <iostream>

#include <QApplication>
#include <QButtonGroup>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QRadioButton>

#include "GuiAutomaticRotationDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "BrainModelSurface.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "global_variables.h"
#include "vtkTransform.h"

/** 
 * Constructor
 */
GuiAutomaticRotationDialog::GuiAutomaticRotationDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Automatic Rotation");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // X, Y, and Z Axis Radio buttons
   //
   xAxisRadioButton = new QRadioButton("X Axis");
   yAxisRadioButton = new QRadioButton("Y Axis");
   zAxisRadioButton = new QRadioButton("Z Axis");
   QGroupBox* axisGroupBox = new QGroupBox("Axis");
   dialogLayout->addWidget(axisGroupBox);
   QVBoxLayout* axisGroupLayout = new QVBoxLayout(axisGroupBox);
   axisGroupLayout->addWidget(xAxisRadioButton);
   axisGroupLayout->addWidget(yAxisRadioButton);
   axisGroupLayout->addWidget(zAxisRadioButton);
   
   //
   // Button Group for axis selection
   //
   QButtonGroup* axisButtonGroup= new QButtonGroup(this);
   axisButtonGroup->addButton(xAxisRadioButton, 0);
   axisButtonGroup->addButton(yAxisRadioButton, 1);
   axisButtonGroup->addButton(zAxisRadioButton, 2);
   yAxisRadioButton->setChecked(true);
   
   //
   // Total Rotation Group Box
   //
   QGroupBox* totalGroupBox = new QGroupBox("Total Rotation");
   QGridLayout* totalGroupLayout = new QGridLayout(totalGroupBox);
   dialogLayout->addWidget(totalGroupBox);

   //
   // Button group for radio buttons so mutually exclusive
   //
   QButtonGroup* buttGroup = new QButtonGroup(this);
   
   //
   // FixedTotal Rotation
   // 
   fixedTotalRadioButton = new QRadioButton("Fixed (Degrees) ");
   totalGroupLayout->addWidget(fixedTotalRadioButton, 0, 0); 
   buttGroup->addButton(fixedTotalRadioButton, 0);
   fixedRotationSpinBox = new QDoubleSpinBox;
   fixedRotationSpinBox->setMinimum(0.0);
   fixedRotationSpinBox->setMaximum(100000.0);
   fixedRotationSpinBox->setSingleStep(10.0);
   fixedRotationSpinBox->setDecimals(3);
   totalGroupLayout->addWidget(fixedRotationSpinBox, 0, 1);
   fixedRotationSpinBox->setValue(360);
   
   //
   // Continuous Rotation
   //
   continuousTotalRadioButton = new QRadioButton("Continuous ");
   totalGroupLayout->addWidget(continuousTotalRadioButton, 1, 0);
   buttGroup->addButton(continuousTotalRadioButton, 1);
   
   //
   // Default to fixed rotation
   //
   fixedTotalRadioButton->setChecked(true);
   
   //
   // Rotation Increment Group Box
   //
   QGroupBox* incrementGroupBox = new QGroupBox("Rotation Increment");
   QGridLayout* incrementGridLayout = new QGridLayout(incrementGroupBox);
   dialogLayout->addWidget(incrementGroupBox);

   //
   // Rotation Increment
   //
   incrementGridLayout->addWidget(new QLabel("Increment (Degrees) "), 0, 0);
   incrementRotationSpinBox = new QDoubleSpinBox;
   incrementRotationSpinBox->setMinimum(-360.0);
   incrementRotationSpinBox->setMaximum(360.0);
   incrementRotationSpinBox->setSingleStep(1.0);
   incrementRotationSpinBox->setDecimals(3);
   incrementGridLayout->addWidget(incrementRotationSpinBox, 0, 1);
   incrementRotationSpinBox->setValue(5);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout1 = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout1);
   buttonsLayout1->setSpacing(2);
   
   //
   // Start pushbutton
   //
   startButton = new QPushButton("Start");
   startButton->setAutoDefault(false);
   buttonsLayout1->addWidget(startButton);
   QObject::connect(startButton, SIGNAL(clicked()),
                    this, SLOT(startButtonSlot()));
   
   //
   // Stop pushbutton
   //
   stopButton = new QPushButton("Stop");
   stopButton->setEnabled(false);
   stopButton->setAutoDefault(false);
   buttonsLayout1->addWidget(stopButton);
   QObject::connect(stopButton, SIGNAL(clicked()),
                    this, SLOT(stopButtonSlot()));
                    
   //
   // Force buttons to be same size
   //
   QtUtilities::makeButtonsSameSize(startButton, stopButton);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout2 = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout2);
   buttonsLayout2->setSpacing(2);
   
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   closeButton->setFixedSize(closeButton->sizeHint());
   buttonsLayout2->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(closeButtonSlot()));
                    
}

/** 
 * Destructor
 */
GuiAutomaticRotationDialog::~GuiAutomaticRotationDialog()
{
}

/** 
 * Called when close button is pressed.
 */
void
GuiAutomaticRotationDialog::closeButtonSlot()
{
   stopButtonSlot();
   close();
}

/** 
 * Called when stop button is pressed.
 */
void
GuiAutomaticRotationDialog::stopButtonSlot()
{
   continueRotatingFlag = false;
}

/** 
 * Called when apply button is pressed.
 */
void
GuiAutomaticRotationDialog::startButtonSlot()
{
   continueRotatingFlag = true;
   stopButton->setEnabled(true);
   startButton->setEnabled(false);
   
   //
   // Get the total rotation and increment
   //
   const float totalRotation = fixedRotationSpinBox->value();
   float rotationIncrement = incrementRotationSpinBox->value();
   
   //
   // Get the rotation axis
   //
   char axis = 'x';
   if (xAxisRadioButton->isChecked()) {
      axis = 'x';
   }
   else if (yAxisRadioButton->isChecked()) {
      axis = 'y';
   }
   else if (zAxisRadioButton->isChecked()) {
      axis = 'z';
   }
   
   //
   // Get the surface in the caret main window
   //
   BrainModelSurface* mainWindowModelSurface = NULL;
   GuiBrainModelOpenGL* mainWindowBrainModelOpenGL = NULL;
   int mainWindowModelViewNumber = -1;
   
   if (GuiBrainModelOpenGL::getCaretMainWindowModelInfo(mainWindowModelSurface,
                                                        mainWindowBrainModelOpenGL,
                                                        mainWindowModelViewNumber)) {
                                                        
      //
      // Rotate the surface until finished
      //
      const float posRotationIncrement = fabs(rotationIncrement);
      float rotationSum = 0.0;
      vtkTransform* matrix = mainWindowModelSurface->getRotationTransformMatrix(mainWindowModelViewNumber);
      GuiBrainModelOpenGL::updateAllGL(mainWindowBrainModelOpenGL); 
      while (continueRotatingFlag) {
         if (fixedTotalRadioButton->isChecked()) {
            const float maxRotation = totalRotation - rotationSum;
            if (posRotationIncrement > maxRotation) {
               if (rotationIncrement < 0.0) {
                  rotationIncrement = maxRotation;
               }
               else {
                  rotationIncrement = maxRotation;
               }
            }
         }
         
         switch(axis) {
            case 'x':
               matrix->RotateX(rotationIncrement);
               break;
            case 'y':
               matrix->RotateY(rotationIncrement);
               break;
            case 'z':
               matrix->RotateZ(rotationIncrement);
               break;
         }
         qApp->processEvents();
         GuiBrainModelOpenGL::updateAllGL(mainWindowBrainModelOpenGL); 
         if (fixedTotalRadioButton->isChecked()) {
            rotationSum += posRotationIncrement;
            if (rotationSum > totalRotation) {
               continueRotatingFlag = false;
            }
         }
      }
      theMainWindow->speakText("Automatic rotation completed.", false);
   }
   
   stopButton->setEnabled(false);
   startButton->setEnabled(true);
}


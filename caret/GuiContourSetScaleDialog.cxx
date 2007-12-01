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

#include <cmath>

#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>

#include "BrainModelContours.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiContourSetScaleDialog.h"
#include "GuiMainWindow.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"

#include "global_variables.h"

/**
 * Constructor
 */
GuiContourSetScaleDialog::GuiContourSetScaleDialog(QWidget* parent, bool modal, Qt::WFlags f)
   : QtDialog(parent, modal, f)
{
   scaleStartPoint[0] = 0.0;
   scaleStartPoint[1] = 0.0;
   scaleEndPoint[0]   = 0.0;
   scaleEndPoint[1]   = 0.0;
   
   setWindowTitle("Contour Set Scale");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // GridLayout for start and end point labels and distance line edit
   //
   QGridLayout* gridLayout = new QGridLayout;
   dialogLayout->addLayout(gridLayout);
   gridLayout->setSpacing(3);
   
   //
   // Start point x and y labels
   //
   gridLayout->addWidget(new QLabel("Scale Start (Left-Click)"), 0, 0, Qt::AlignLeft);
   scaleStartPointXLabel = new QLabel("0.0");
   gridLayout->addWidget(scaleStartPointXLabel, 0, 1, Qt::AlignLeft);
   scaleStartPointYLabel = new QLabel("0.0");
   gridLayout->addWidget(scaleStartPointYLabel, 0, 2, Qt::AlignLeft);
   
   //
   // Start end x and y labels
   //
   gridLayout->addWidget(new QLabel("Scale End (Shift Left-Click)"), 1, 0, Qt::AlignLeft);
   scaleEndPointXLabel = new QLabel("0.0");
   gridLayout->addWidget(scaleEndPointXLabel, 1, 1, Qt::AlignLeft);
   scaleEndPointYLabel = new QLabel("0.0");
   gridLayout->addWidget(scaleEndPointYLabel, 1, 2, Qt::AlignLeft);
   
   //
   // Distance float spin box
   //
   gridLayout->addWidget(new QLabel("Distance"), 2, 0, Qt::AlignLeft);
   distanceDoubleSpinBox = new QDoubleSpinBox;
   distanceDoubleSpinBox->setMinimum(0.0);
   distanceDoubleSpinBox->setMaximum(1000.0);
   distanceDoubleSpinBox->setSingleStep(1.0);
   distanceDoubleSpinBox->setDecimals(2);
   gridLayout->addWidget(distanceDoubleSpinBox, 2, 1, 1, 2, Qt::AlignLeft);
   distanceDoubleSpinBox->setValue(1.0);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(2);
   
   //
   // Apply button 
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
                    
   //
   // Reset button
   //
   QPushButton* resetButton = new QPushButton("Reset");
   resetButton->setAutoDefault(false);
   buttonsLayout->addWidget(resetButton);
   QObject::connect(resetButton, SIGNAL(clicked()),
                    this, SLOT(slotResetButton()));
   
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   QtUtilities::makeButtonsSameSize(applyButton, resetButton, closeButton);
   
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                    GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_SET_SCALE);
}

/**
 * Destructor
 */
GuiContourSetScaleDialog::~GuiContourSetScaleDialog()
{
}

/**
 * set the scale start point.
 */
void 
GuiContourSetScaleDialog::setScaleStartPoint(const float x, const float y)
{
   scaleStartPoint[0] = x;
   scaleStartPoint[1] = y;
   scaleStartPointXLabel->setNum(x);
   scaleStartPointYLabel->setNum(y);
}

/**
 * set the scale end point.
 */
void 
GuiContourSetScaleDialog::setScaleEndPoint(const float x, const float y)
{
   scaleEndPoint[0] = x;
   scaleEndPoint[1] = y;
   scaleEndPointXLabel->setNum(x);
   scaleEndPointYLabel->setNum(y);
}
      
/**
 * update the dialog due to contour changes.
 */
void 
GuiContourSetScaleDialog::updateDialog()
{
}      

/**
 * Called when apply button is pressed.
 */
void
GuiContourSetScaleDialog::slotApplyButton()
{
   //
   // Get and check the scale points
   //
   const float dx = scaleEndPoint[0] - scaleStartPoint[0];
   const float dy = scaleEndPoint[1] - scaleStartPoint[1];
   const float dist = std::sqrt(dx*dx + dy*dy);
   if (dist == 0.0) {
      QMessageBox::critical(this, "Contour Set Scale Error",
                            "Start and End Points must not be the same.");
      return;
   } 
   
   //
   // Get and check the contour distance
   //
   const float contourDistance = distanceDoubleSpinBox->value();
   if (contourDistance <= 0.0) {
      QMessageBox::critical(this, "Contour Set Scale Error",
                            "Distance must be greater than zero.");
      return;
   }
   
   //
   // Get and check the contour model in the main window
   //
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      QMessageBox::critical(this, "Contour Set Scale Error",
                            "A contour model must be in the main window.");
      return;
   }
  
   //
   // Get the current scaling
   //
   float currentScaling[3];
   bmc->getScaling(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW, currentScaling);
   
   //
   // set the new scaling
   //
   float newScaling[3];
   for (int j = 0; j < 3; j++) {
      newScaling[j] = dist / contourDistance;
      if (currentScaling[j] != 0.0) {
         newScaling[j] *= currentScaling[j];
      }
   }
   
   //
   // Update the contour scaling
   // 
   bmc->setScaling(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                   newScaling);
                   
   //
   // Redraw
   //
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

/**
 * Called when reset button is pressed.
 */
void
GuiContourSetScaleDialog::slotResetButton()
{
   //
   // Get and check the contour model in the main window
   //
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      QMessageBox::critical(this, "Contour Set Scale Error",
                            "A contour model must be in the main window.");
      return;
   }
  
   const float ones[3] = { 1.0, 1.0, 1.0 };
   bmc->setScaling(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                   ones);
                   
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                    GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_SET_SCALE);

   //
   // Redraw
   //
   GuiBrainModelOpenGL::updateAllGL(NULL);
}


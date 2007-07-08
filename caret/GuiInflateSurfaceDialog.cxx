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
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiInflateSurfaceDialog.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiInflateSurfaceDialog::GuiInflateSurfaceDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setWindowTitle("Surface Inflation");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Smoothing iterations
   //
   QLabel* smoothingIterationsLabel = new QLabel("Smoothing Iterations");
   smoothingIterationsSpinBox = new QSpinBox;
   smoothingIterationsSpinBox->setMinimum(1);
   smoothingIterationsSpinBox->setMaximum(1000000);
   smoothingIterationsSpinBox->setSingleStep(10);
   smoothingIterationsSpinBox->setValue(500);
   
   //
   // Inflation iterations
   //
   QLabel* inflateIterationsLabel = new QLabel("Inflate Every X Iterations");
   inflateIterationsSpinBox = new QSpinBox;
   inflateIterationsSpinBox->setMinimum(1);
   inflateIterationsSpinBox->setMaximum(1000000);
   inflateIterationsSpinBox->setSingleStep(10);
   inflateIterationsSpinBox->setValue(10);
   
   //
   // Inflation factor
   //
   QLabel* inflationFactorLabel = new QLabel("Inflation Factor");
   inflationFactorDoubleSpinBox = new QDoubleSpinBox;
   inflationFactorDoubleSpinBox->setMinimum(1.0);
   inflationFactorDoubleSpinBox->setMaximum(2.0);
   inflationFactorDoubleSpinBox->setSingleStep(0.01);
   inflationFactorDoubleSpinBox->setDecimals(2);
   inflationFactorDoubleSpinBox->setValue(1.02);
   
   //
   // grid layout for items
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->setSpacing(5);
   dialogLayout->addLayout(gridLayout);
   gridLayout->addWidget(smoothingIterationsLabel, 0, 0);
   gridLayout->addWidget(smoothingIterationsSpinBox, 0, 1);
   gridLayout->addWidget(inflateIterationsLabel, 1, 0);
   gridLayout->addWidget(inflateIterationsSpinBox, 1, 1);
   gridLayout->addWidget(inflationFactorLabel, 2, 0);
   gridLayout->addWidget(inflationFactorDoubleSpinBox, 2, 1);

   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
                    
   //
   // Close button 
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
}

/**
 * Destructor
 */
GuiInflateSurfaceDialog::~GuiInflateSurfaceDialog()
{
}

/**
 * called when OK or Cancel button pressed.
 */
void 
GuiInflateSurfaceDialog::slotApplyButton()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms == NULL) {
      GuiMessageBox::critical(this, "ERROR", "There is no surface in the main window.", "OK");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   bms->inflate(smoothingIterationsSpinBox->value(),
                  inflateIterationsSpinBox->value(),
                  inflationFactorDoubleSpinBox->value());
            
   GuiBrainModelOpenGL::updateAllGL();

   QApplication::restoreOverrideCursor();
   
   theMainWindow->speakText("Surface inflation has completed.", false);
}


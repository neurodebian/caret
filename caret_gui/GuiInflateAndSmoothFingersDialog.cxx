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
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiInflateAndSmoothFingersDialog.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiInflateAndSmoothFingersDialog::GuiInflateAndSmoothFingersDialog(QWidget* parent, 
                                                                   const bool modalFlag)
   : WuQDialog(parent)
{
   setModal(modalFlag);
   setWindowTitle("Inflate Surface And Smooth Fingers");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Smoothing cycles
   //
   QLabel* smoothingCyclesLabel = new QLabel("Number of Smoothing Cycles");
   smoothingCyclesSpinBox = new QSpinBox;
   smoothingCyclesSpinBox->setMinimum(1);
   smoothingCyclesSpinBox->setMaximum(100000);
   smoothingCyclesSpinBox->setSingleStep(1);
   smoothingCyclesSpinBox->setValue(3);
   
   //
   // regular smoothing strength
   //
   QLabel* regularSmoothingStrengthLabel = new QLabel("Regular Smoothing Strength");
   regularSmoothStrengthDoubleSpinBox = new QDoubleSpinBox;
   regularSmoothStrengthDoubleSpinBox->setMinimum(0.0);
   regularSmoothStrengthDoubleSpinBox->setMaximum(1.0);
   regularSmoothStrengthDoubleSpinBox->setSingleStep(0.01);
   regularSmoothStrengthDoubleSpinBox->setDecimals(2);
   regularSmoothStrengthDoubleSpinBox->setValue(1.0);
   
   //
   // regular smoothing iterations spin box
   //
   QLabel* regularSmoothingIterationsLabel = new QLabel("Regular Smoothing Iterations");
   regularSmoothIterationsSpinBox = new QSpinBox;
   regularSmoothIterationsSpinBox->setMinimum(1);
   regularSmoothIterationsSpinBox->setMaximum(100000);
   regularSmoothIterationsSpinBox->setSingleStep(1);
   regularSmoothIterationsSpinBox->setValue(50);
   
   //
   // Inflation Factor
   //
   QLabel* inflationFactorLabel = new QLabel("Inflation Factor");
   inflationFactorDoubleSpinBox = new QDoubleSpinBox;
   inflationFactorDoubleSpinBox->setMinimum(0.0);
   inflationFactorDoubleSpinBox->setMaximum(100.0);
   inflationFactorDoubleSpinBox->setSingleStep(0.1);
   inflationFactorDoubleSpinBox->setDecimals(2);
   inflationFactorDoubleSpinBox->setValue(1.02);
   
   //
   // Finger smoothing threshold float spin box
   //
   QLabel* fingerSmoothingThresholdLabel = new QLabel("Finger Smoothing Threshold");
   fingerSmoothingThresholdDoubleSpinBox = new QDoubleSpinBox;
   fingerSmoothingThresholdDoubleSpinBox->setMinimum(0.0);
   fingerSmoothingThresholdDoubleSpinBox->setMaximum(1000.0);
   fingerSmoothingThresholdDoubleSpinBox->setSingleStep(0.1);
   fingerSmoothingThresholdDoubleSpinBox->setDecimals(2);
   fingerSmoothingThresholdDoubleSpinBox->setValue(1.0);
   
   //
   // Finger smoothing strength float spin box
   //
   QLabel* fingerSmoothingStrengthLabel = new QLabel("Finger Smoothing Strength");
   fingerSmoothingStrengthDoubleSpinBox = new QDoubleSpinBox;
   fingerSmoothingStrengthDoubleSpinBox->setMinimum(0.0);
   fingerSmoothingStrengthDoubleSpinBox->setMaximum(1.0);
   fingerSmoothingStrengthDoubleSpinBox->setSingleStep(0.01);
   fingerSmoothingStrengthDoubleSpinBox->setDecimals(2);
   fingerSmoothingStrengthDoubleSpinBox->setValue(1.0);
   
   //
   // Fingers smoothing iterations 
   //
   QLabel* fingerSmoothingIterationsLabel = new QLabel("Finger Smoothing Iterations");
   fingerSmoothingIterationsSpinBox = new QSpinBox;
   fingerSmoothingIterationsSpinBox->setMinimum(0);
   fingerSmoothingIterationsSpinBox->setMaximum(100000);
   fingerSmoothingIterationsSpinBox->setSingleStep(10);
   fingerSmoothingIterationsSpinBox->setValue(100);
   
   //
   // Grid layout for parameters
   //
   QGridLayout* parametersGridLayout = new QGridLayout;
   parametersGridLayout->setSpacing(5);
   dialogLayout->addLayout(parametersGridLayout);
   parametersGridLayout->addWidget(smoothingCyclesLabel, 0, 0);
   parametersGridLayout->addWidget(smoothingCyclesSpinBox, 0, 1);
   parametersGridLayout->addWidget(regularSmoothingStrengthLabel, 1, 0);
   parametersGridLayout->addWidget(regularSmoothStrengthDoubleSpinBox, 1, 1);
   parametersGridLayout->addWidget(regularSmoothingIterationsLabel, 2, 0);
   parametersGridLayout->addWidget(regularSmoothIterationsSpinBox, 2, 1);
   parametersGridLayout->addWidget(inflationFactorLabel, 3, 0);
   parametersGridLayout->addWidget(inflationFactorDoubleSpinBox, 3, 1);
   parametersGridLayout->addWidget(fingerSmoothingThresholdLabel, 4, 0);
   parametersGridLayout->addWidget(fingerSmoothingThresholdDoubleSpinBox, 4, 1);
   parametersGridLayout->addWidget(fingerSmoothingStrengthLabel, 5, 0);
   parametersGridLayout->addWidget(fingerSmoothingStrengthDoubleSpinBox, 5, 1);
   parametersGridLayout->addWidget(fingerSmoothingIterationsLabel, 6, 0);
   parametersGridLayout->addWidget(fingerSmoothingIterationsSpinBox, 6, 1);

   //
   // Add measurements to metrics check box
   //
   addToMetricsCheckBox = new QCheckBox("Add Measurements to Metrics",
                                        this);
   dialogLayout->addWidget(addToMetricsCheckBox);
   addToMetricsCheckBox->setChecked(true);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   if (isModal()) {
      //
      // OK button
      //
      QPushButton* okButton = new QPushButton("OK");
      buttonsLayout->addWidget(okButton);
      QObject::connect(okButton, SIGNAL(clicked()),
                     this, SLOT(accept()));
                     
      //
      // Cancel button
      //
      QPushButton* cancelButton = new QPushButton("Cancel");
      buttonsLayout->addWidget(cancelButton);
      QObject::connect(cancelButton, SIGNAL(clicked()),
                     this, SLOT(reject()));
      
      QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   }
   else {
      //
      // Apply button
      //
      QPushButton* applyButton = new QPushButton("Apply");
      buttonsLayout->addWidget(applyButton);
      QObject::connect(applyButton, SIGNAL(clicked()),
                     this, SLOT(slotApply()));
                     
      //
      // Close button connects to QDialogs close() slot.
      //
      QPushButton* closeButton = new QPushButton("Close");
      buttonsLayout->addWidget(closeButton);
      QObject::connect(closeButton, SIGNAL(clicked()),
                     this, SLOT(close()));
      
      QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   }
}

/**
 * Destructor.
 */
GuiInflateAndSmoothFingersDialog::~GuiInflateAndSmoothFingersDialog()
{
}

/**
 * Slot for OK or Cancel button in modal mode.
 */
void
GuiInflateAndSmoothFingersDialog::done(int r) 
{
   if (r == QDialog::Accepted) {
      if (doSmoothing()) {
         return;
      }
   }
   QDialog::done(r);
}
 
/**
 * Slot for apply button in non-modal mode.
 */
void
GuiInflateAndSmoothFingersDialog::slotApply()
{
   doSmoothing();
}

/**
 * Called to do the actual smoothing.  Returns true if an error occurs.
 */   
bool
GuiInflateAndSmoothFingersDialog::doSmoothing()
{
   const int   numCycles = smoothingCyclesSpinBox->value();
   const float regularStrength = regularSmoothStrengthDoubleSpinBox->value();
   const int   regularIterations = regularSmoothIterationsSpinBox->value();
   const float inflationFactor = inflationFactorDoubleSpinBox->value();
   const float fingerThreshold = fingerSmoothingThresholdDoubleSpinBox->value();
   const float fingerStrength  = fingerSmoothingStrengthDoubleSpinBox->value();
   const int   fingerIterations = fingerSmoothingIterationsSpinBox->value();
   
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      MetricFile* mf = NULL;
      if (addToMetricsCheckBox->isChecked()) {
         mf = theMainWindow->getBrainSet()->getMetricFile();
      }
      
      bms->inflateSurfaceAndSmoothFingers(theMainWindow->getBrainSet()->getActiveFiducialSurface(),
                                          numCycles,
                                          regularStrength,
                                          regularIterations,
                                          inflationFactor,
                                          fingerThreshold,
                                          fingerStrength,
                                          fingerIterations,
                                          mf);
      GuiFilesModified fm;
      fm.setMetricModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
      QApplication::restoreOverrideCursor();
      theMainWindow->speakText("Surface inflation and finger smoothing is complete.", false);
   }
   
   return false;
}



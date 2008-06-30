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

#include <QApplication>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QString>
#include <QTime>

#include "GuiBrainModelSelectionComboBox.h"
#include "BrainModelSurfaceFociUncertaintyToRgbPaint.h"
#include "BrainSet.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
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
   : QDialog(parent)
{
   setModal(true);
   setWindowTitle("Uncertainty Limits to RGB Paint");
   
   //
   // Create the surfaces and limits widgets
   //
   leftSurfaceGroupBox = createLeftSurfaceWidgets();
   rightSurfaceGroupBox = createRightSurfaceWidgets();
   QWidget* limitsWidget = createLimitsWidgets();
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK", this);
   okButton->setAutoDefault(true);
   QObject::connect(okButton, SIGNAL(clicked()),
                  this, SLOT(accept()));
   
   //
   // Close button
   //
   QPushButton* cancelButton = new QPushButton("Cancel", this);
   cancelButton->setAutoDefault(false);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                  this, SLOT(reject()));
   
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   
   //
   //
   // Layout Dialog Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(okButton);
   buttonsLayout->addWidget(cancelButton);
   
   //
   // Layout dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(leftSurfaceGroupBox);
   dialogLayout->addWidget(rightSurfaceGroupBox);
   dialogLayout->addWidget(limitsWidget);
   dialogLayout->addLayout(buttonsLayout);   
}

/**
 * Destructor.
 */
GuiFociUncertaintyLimitsDialog::~GuiFociUncertaintyLimitsDialog()
{
}

/**
 * create the limits widgets.
 */
QWidget* 
GuiFociUncertaintyLimitsDialog::createLimitsWidgets()
{
   //
   // lower limit
   //
   QLabel* lowerLabel = new QLabel("Lower Limit (mm)");
   lowerLimitDoubleSpinBox = new QDoubleSpinBox;
   lowerLimitDoubleSpinBox->setMinimum(0.0);
   lowerLimitDoubleSpinBox->setMaximum(std::numeric_limits<float>::max());
   lowerLimitDoubleSpinBox->setSingleStep(1.0);
   lowerLimitDoubleSpinBox->setDecimals(3);
   lowerLimitDoubleSpinBox->setValue(lowerLimit);
   
   //
   // middle limit
   //
   QLabel* middleLabel = new QLabel("Middle Limit (mm)");
   middleLimitDoubleSpinBox = new QDoubleSpinBox;
   middleLimitDoubleSpinBox->setMinimum(0.0);
   middleLimitDoubleSpinBox->setMaximum(std::numeric_limits<float>::max());
   middleLimitDoubleSpinBox->setSingleStep(1.0);
   middleLimitDoubleSpinBox->setDecimals(3);
   middleLimitDoubleSpinBox->setValue(middleLimit);
   
   //
   // upper limit
   //
   QLabel* upperLabel = new QLabel("Upper Limit (mm)");
   upperLimitDoubleSpinBox = new QDoubleSpinBox;
   upperLimitDoubleSpinBox->setMinimum(0.0);
   upperLimitDoubleSpinBox->setMaximum(std::numeric_limits<float>::max());
   upperLimitDoubleSpinBox->setSingleStep(1.0);
   upperLimitDoubleSpinBox->setDecimals(3);
   upperLimitDoubleSpinBox->setValue(upperLimit);
   
   //
   // Groupbox and layouts for limits
   //
   QGroupBox* limitsGroupBox = new QGroupBox("Foci Limits");
   QGridLayout* limitsLayout = new QGridLayout(limitsGroupBox);
   limitsLayout->addWidget(lowerLabel, 0, 0);
   limitsLayout->addWidget(lowerLimitDoubleSpinBox, 0, 1);
   limitsLayout->addWidget(middleLabel, 1, 0);
   limitsLayout->addWidget(middleLimitDoubleSpinBox, 1, 1);
   limitsLayout->addWidget(upperLabel, 2, 0);
   limitsLayout->addWidget(upperLimitDoubleSpinBox, 2, 1);
   limitsLayout->setColumnStretch(0, 0);
   limitsLayout->setColumnStretch(1, 0);
   limitsLayout->setColumnStretch(1, 100);
   
   return limitsGroupBox;
}      

/**
 * create the left surface widgets.
 */
QGroupBox*
GuiFociUncertaintyLimitsDialog::createLeftSurfaceWidgets()      
{
   //
   // Surface selection
   //
   QLabel* surfaceLabel = new QLabel("Left Surface");
   leftSurfaceComboBox =
      new GuiBrainModelSelectionComboBox(
         GuiBrainModelSelectionComboBox::OPTION_SHOW_SURFACES_ALL |
            GuiBrainModelSelectionComboBox::OPTION_SHOW_ADD_NEW,
         "NONE");
   leftSurfaceComboBox->setSelectedBrainModelToFirstSurfaceOfType(
      BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
      Structure::STRUCTURE_TYPE_CORTEX_LEFT);

   //
   // RGB Label and column selection
   //
   QLabel* rgbLabel = new QLabel("RGB Paint Column");
   rgbPaintLeftSelectionComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                GUI_NODE_FILE_TYPE_RGB_PAINT,
                                                true,
                                                false,
                                                false);
   const QString columnName("Left Foci Uncertainty");
   rgbPaintLeftColumnNewNameLineEdit = new QLineEdit;
   rgbPaintLeftColumnNewNameLineEdit->setText(columnName);
   
   //
   // Initialize RGB Paint column selection 
   //
   rgbPaintLeftSelectionComboBox->setCurrentIndex(rgbPaintLeftSelectionComboBox->count() - 1);
   for (int i = 0; i < rgbPaintLeftSelectionComboBox->count(); i++) {
      if (rgbPaintLeftSelectionComboBox->itemText(i) == columnName) {
         rgbPaintLeftSelectionComboBox->setCurrentIndex(i);
         break;
      }
   }
   rgbPaintLeftColumnNewNameLineEdit->setText(rgbPaintLeftSelectionComboBox->currentText());
   QObject::connect(rgbPaintLeftSelectionComboBox, SIGNAL(activated(const QString&)),
                    rgbPaintLeftColumnNewNameLineEdit, SLOT(setText(const QString&)));
 
   //
   // Group box and layout for left surface
   //  
   QGroupBox* groupBox = new QGroupBox("Left Surface");
   QGridLayout* leftLayout = new QGridLayout(groupBox);
   leftLayout->addWidget(surfaceLabel, 0, 0);
   leftLayout->addWidget(leftSurfaceComboBox, 0, 1, 1, 2);
   leftLayout->addWidget(rgbLabel, 1, 0);
   leftLayout->addWidget(rgbPaintLeftSelectionComboBox, 1, 1);
   leftLayout->addWidget(rgbPaintLeftColumnNewNameLineEdit, 1, 2);

   groupBox->setCheckable(true);
   groupBox->setChecked(true);
      
   return groupBox;
}

/**
 * create the right surface widgets.
 */
QGroupBox*
GuiFociUncertaintyLimitsDialog::createRightSurfaceWidgets()      
{
   //
   // Surface selection
   //
   QLabel* surfaceLabel = new QLabel("Right Surface");
   rightSurfaceComboBox =
      new GuiBrainModelSelectionComboBox(
         GuiBrainModelSelectionComboBox::OPTION_SHOW_SURFACES_ALL |
            GuiBrainModelSelectionComboBox::OPTION_SHOW_ADD_NEW,
         "NONE");
   rightSurfaceComboBox->setSelectedBrainModelToFirstSurfaceOfType(
      BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
      Structure::STRUCTURE_TYPE_CORTEX_RIGHT);

   //
   // RGB Label and column selection
   //
   QLabel* rgbLabel = new QLabel("RGB Paint Column");
   rgbPaintRightSelectionComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                GUI_NODE_FILE_TYPE_RGB_PAINT,
                                                true,
                                                false,
                                                false);
   const QString columnName("Right Foci Uncertainty");
   rgbPaintRightColumnNewNameLineEdit = new QLineEdit;
   rgbPaintRightColumnNewNameLineEdit->setText(columnName);
   
   //
   // Initialize RGB Paint column selection 
   //
   rgbPaintRightSelectionComboBox->setCurrentIndex(rgbPaintRightSelectionComboBox->count() - 1);
   for (int i = 0; i < rgbPaintRightSelectionComboBox->count(); i++) {
      if (rgbPaintRightSelectionComboBox->itemText(i) == columnName) {
         rgbPaintRightSelectionComboBox->setCurrentIndex(i);
         break;
      }
   }
   rgbPaintRightColumnNewNameLineEdit->setText(rgbPaintRightSelectionComboBox->currentText());
   QObject::connect(rgbPaintRightSelectionComboBox, SIGNAL(activated(const QString&)),
                    rgbPaintRightColumnNewNameLineEdit, SLOT(setText(const QString&)));
 
   //
   // Group box and layout for right surface
   //  
   QGroupBox* groupBox = new QGroupBox("Right Surface");
   QGridLayout* rightLayout = new QGridLayout(groupBox);
   rightLayout->addWidget(surfaceLabel, 0, 0);
   rightLayout->addWidget(rightSurfaceComboBox, 0, 1, 1, 2);
   rightLayout->addWidget(rgbLabel, 1, 0);
   rightLayout->addWidget(rgbPaintRightSelectionComboBox, 1, 1);
   rightLayout->addWidget(rgbPaintRightColumnNewNameLineEdit, 1, 2);
   
   groupBox->setCheckable(true);
   groupBox->setChecked(true);

   return groupBox;
}

/**
 * Called when OK or cancelbutton pressed.
 */
void
GuiFociUncertaintyLimitsDialog::done(int r)
{
   QTime timer;
   timer.start();
   
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      BrainSet* brainSet = theMainWindow->getBrainSet();
      
      lowerLimit  = lowerLimitDoubleSpinBox->value();
      middleLimit = middleLimitDoubleSpinBox->value();
      upperLimit  = upperLimitDoubleSpinBox->value();

      BrainModelSurface* leftSurface = NULL;
      if (leftSurfaceGroupBox->isChecked()) {
         leftSurface = leftSurfaceComboBox->getSelectedBrainModelSurface();
      }
      BrainModelSurface* rightSurface = NULL;
      if (rightSurfaceGroupBox->isChecked()) {
         rightSurface = rightSurfaceComboBox->getSelectedBrainModelSurface();
      }
      
      BrainModelSurfaceFociUncertaintyToRgbPaint furp(brainSet,
                              leftSurface,
                              rightSurface,
                              brainSet->getRgbPaintFile(),
                              rgbPaintLeftSelectionComboBox->currentIndex(),
                              rgbPaintLeftColumnNewNameLineEdit->text(),
                              rgbPaintRightSelectionComboBox->currentIndex(),
                              rgbPaintRightColumnNewNameLineEdit->text(),
                              brainSet->getFociProjectionFile(),
                              brainSet->getFociColorFile(),
                              lowerLimit,
                              middleLimit,
                              upperLimit);
      QString errorMessage;
      try {
         furp.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessage = e.whatQString();
      }
                              
      QApplication::restoreOverrideCursor();
      
      GuiFilesModified fm;
      fm.setRgbPaintModified();
      
      theMainWindow->fileModificationUpdate(fm);
      if (errorMessage.isEmpty() == false) {
         QMessageBox::critical(this, "Error", errorMessage);
         return;
      }
   }
   
   QDialog::done(r);
   
   //const float elapsedTime = (static_cast<float>(timer.elapsed()) * 0.001);
   //std::cout << "Elapsed time: " << elapsedTime << std::endl;
}


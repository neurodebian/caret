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
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QToolTip>

#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiDataFileMathDialog.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiVolumeSelectionControl.h"
#include "MetricFile.h"
#include "QtUtilities.h"
#include "SurfaceShapeFile.h"
#include "VolumeFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiDataFileMathDialog::GuiDataFileMathDialog(QWidget* parent, 
                                             const DIALOG_MODE modeIn)
   : QtDialog(parent, false)
{
   dialogMode = modeIn;
   
   //
   // Set caption
   //
   switch (dialogMode) {
      case DIALOG_MODE_METRIC_FILE:
         setWindowTitle("Metric File Mathematical Operations");
         break;
      case DIALOG_MODE_SURFACE_SHAPE_FILE:
         setWindowTitle("Surface Shape File Mathematical Operations");
         break;
      case DIALOG_MODE_VOLUME_FILE:
         setWindowTitle("Volume File Mathematical Operations");
         break;
   }
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);

   switch (dialogMode) {
      case DIALOG_MODE_METRIC_FILE:
      case DIALOG_MODE_SURFACE_SHAPE_FILE:
         //
         // Create the metric column part of the dialog
         //   
         dialogLayout->addWidget(createMetricColumnPartOfDialog());
         break;
      case DIALOG_MODE_VOLUME_FILE:
         //
         // Create the volume part of the dialog
         //   
         dialogLayout->addWidget(createVolumePartOfDialog());
         break;
   }
   
   //
   // Create the operations part of the dialog
   //   
   QWidget* operationsWidget = createOperationsPartOfDialog();
   QScrollArea* scrollArea = new QScrollArea;
   scrollArea->setWidget(operationsWidget);
   scrollArea->setWidgetResizable(true);
   dialogLayout->addWidget(scrollArea);
   
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
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                  this, SLOT(close()));

   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   updateDialog();
}

/**
 * Destructor.
 */
GuiDataFileMathDialog::~GuiDataFileMathDialog()
{
}

/**
 * Called when apply button pressed.
 */
void 
GuiDataFileMathDialog::slotApplyButton()
{
   QString errorMessage;
   
   switch (dialogMode) {
      case DIALOG_MODE_METRIC_FILE:
         {
            performMetricMathOperations(errorMessage);
            GuiFilesModified fm;
            fm.setMetricModified();
            theMainWindow->fileModificationUpdate(fm);
         }
         break;
      case DIALOG_MODE_SURFACE_SHAPE_FILE:
         {
            performMetricMathOperations(errorMessage);
            GuiFilesModified fm;
            fm.setSurfaceShapeModified();
            theMainWindow->fileModificationUpdate(fm);
         }
         break;
      case DIALOG_MODE_VOLUME_FILE:
         {
            performVolumeMathOperations(errorMessage);
            GuiFilesModified fm;
            fm.setVolumeModified();
            theMainWindow->fileModificationUpdate(fm);
         }
         break;
   }

   if (errorMessage.isEmpty() == false) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "ERROR", errorMessage);
      return;
   }
   
   GuiBrainModelOpenGL::updateAllGL();
   
   QApplication::beep();
   QApplication::restoreOverrideCursor();
}

/**
 * perform metric math operations.
 */
void 
GuiDataFileMathDialog::performMetricMathOperations(QString& errorMessage)
{
   int columnA = inputMetricColumnComboBoxA->currentIndex();
   int columnB = inputMetricColumnComboBoxB->currentIndex();
   int outputColumn = outputMetricColumnComboBox->currentIndex();
   
   if ((columnA < 0) || (columnA > getNodeDataFile()->getNumberOfColumns())) {
      errorMessage.append("Input column A is invalid.\n");
   }
   if ((columnB < 0) || (columnB > getNodeDataFile()->getNumberOfColumns())) {
      errorMessage.append("Input column B is invalid.\n");
   }
   if (errorMessage.isEmpty() == false) {
      return;
   }

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   const bool creatingNewOutputColumn = (outputColumn < 0);
   const QString outputColumnName(outputMetricNameLineEdit->text());
   
   try {
      if (addColumnsRadioButton->isChecked()) {
         getNodeDataFile()->performBinaryOperation(MetricFile::BINARY_OPERATION_ADD,
                                columnA,
                                columnB,
                                outputColumn,
                                outputColumnName);
      }
      else if (subtractColumnsRadioButton->isChecked()) {
         getNodeDataFile()->performBinaryOperation(MetricFile::BINARY_OPERATION_SUBTRACT,
                                columnA,
                                columnB,
                                outputColumn,
                                outputColumnName);
      }
      else if (multiplyColumnsRadioButton->isChecked()) {
         getNodeDataFile()->performBinaryOperation(MetricFile::BINARY_OPERATION_MULTIPLY,
                                columnA,
                                columnB,
                                outputColumn,
                                outputColumnName);
      }
      else if (divideColumnsRadioButton->isChecked()) {
         getNodeDataFile()->performBinaryOperation(MetricFile::BINARY_OPERATION_DIVIDE,
                                columnA,
                                columnB,
                                outputColumn,
                                outputColumnName);
      }
      else if (averageColumnsRadioButton->isChecked()) {
         getNodeDataFile()->performBinaryOperation(MetricFile::BINARY_OPERATION_AVERAGE,
                                columnA,
                                columnB,
                                outputColumn,
                                outputColumnName);
      }
      else if (addScalarToColumnRadioButton->isChecked()) {
         getNodeDataFile()->performUnaryOperation(MetricFile::UNARY_OPERATION_ADD,
                                         columnA,
                                         outputColumn,
                                         outputColumnName,
                                         addScalarDoubleSpinBox->value());
      }
      else if (multiplyColumnByScalarRadioButton->isChecked()) {
         getNodeDataFile()->performUnaryOperation(MetricFile::UNARY_OPERATION_MULTIPLY,
                                         columnA,
                                         outputColumn,
                                         outputColumnName,
                                         multiplyScalarDoubleSpinBox->value());
      }
      else if (ceilingColumnByScalarRadioButton->isChecked()) {
         getNodeDataFile()->performUnaryOperation(MetricFile::UNARY_OPERATION_CEILING,
                                         columnA,
                                         outputColumn,
                                         outputColumnName,
                                         ceilingScalarDoubleSpinBox->value());
      }
      else if (floorColumnByScalarRadioButton->isChecked()) {
         getNodeDataFile()->performUnaryOperation(MetricFile::UNARY_OPERATION_FLOOR,
                                         columnA,
                                         outputColumn,
                                         outputColumnName,
                                         floorScalarDoubleSpinBox->value());
      }
      else if (fixNaNRadioButton->isChecked()) {
         getNodeDataFile()->performUnaryOperation(MetricFile::UNARY_OPERATION_FIX_NOT_A_NUMBER,
                                         columnA,
                                         outputColumn,
                                         outputColumnName,
                                         0.0);
      }
      else if (squareRootRadioButton->isChecked()) {
         getNodeDataFile()->performUnaryOperation(MetricFile::UNARY_OPERATION_SQUARE_ROOT,
                                         columnA,
                                         outputColumn,
                                         outputColumnName,
                                         0.0);
      }
      else if (logRadioButton->isChecked()) {
         getNodeDataFile()->performUnaryOperation(MetricFile::UNARY_OPERATION_LOG2,
                                         columnA,
                                         outputColumn,
                                         outputColumnName,
                                         logBaseLineEdit->text().toDouble());
      }
      else if (absValueRadioButton->isChecked()) {
         getNodeDataFile()->performUnaryOperation(MetricFile::UNARY_OPERATION_ABS_VALUE,
                                         columnA,
                                         outputColumn,
                                         outputColumnName,
                                         0.0);
      }
      else if (inclusiveSetRangeToZeroRadioButton->isChecked()) {
         getNodeDataFile()->setRangeOfValuesToZero(columnA,
                                          outputColumn,
                                          outputColumnName,
                                          inclusiveSetRangeToZeroMinValueDoubleSpinBox->value(),
                                          inclusiveSetRangeToZeroMaxValueDoubleSpinBox->value(),
                                          true);
      }
      else if (exclusiveSetRangeToZeroRadioButton->isChecked()) {
         getNodeDataFile()->setRangeOfValuesToZero(columnA,
                                          outputColumn,
                                          outputColumnName,
                                          exclusiveSetRangeToZeroMinValueDoubleSpinBox->value(),
                                          exclusiveSetRangeToZeroMaxValueDoubleSpinBox->value(),
                                          false);
      }
      else if (normalizeValuesRadioButton->isChecked()) {
         getNodeDataFile()->remapColumnToNormalDistribution(columnA,
                                                   outputColumn,
                                                   outputColumnName,
                                                   normalizeMeanDoubleSpinBox->value(),
                                                   normalizeDeviationDoubleSpinBox->value());
      }
      else if (oneMinusValueRadioButton->isChecked()) {
         getNodeDataFile()->performUnaryOperation(MetricFile::UNARY_OPERATION_SUBTRACT_FROM_ONE,
                                         columnA,
                                         outputColumn,
                                         outputColumnName,
                                         0.0);
      }
      else {
         errorMessage = "No Operation Selected.";
         return;
      }
   }
   catch (FileException& e) {
      errorMessage = e.whatQString();
      return;
   }
   
   if (creatingNewOutputColumn) {
      const int col = getNodeDataFile()->getNumberOfColumns() - 1;
      outputMetricColumnComboBox->setCurrentIndex(col);
      outputMetricNameLineEdit->setText(getNodeDataFile()->getColumnName(col));
   }
}

/**
 * perform volume math operations.
 */
void 
GuiDataFileMathDialog::performVolumeMathOperations(QString& errorMessage)
{
   //
   // Get the input volumes
   //
   VolumeFile* volumeA = volumeASelectionControl->getSelectedVolumeFile();
   VolumeFile* volumeB = volumeBSelectionControl->getSelectedVolumeFile();
   
   //
   // Make sure volume A is valid
   //
   if (volumeA == NULL) {
      errorMessage = "Volume A is invalid.";
      return;
   }
   
   //
   // Get the output volume
   //
   VolumeFile* volumeOutput = volumeOutputSelectionControl->getSelectedVolumeFile();
   bool createdNewVolume = false;
   if (volumeOutput == NULL) {
      if (volumeOutputSelectionControl->getNewVolumeSelected()) {
         //
         // Make output volume same as volume A
         //
         volumeOutput = new VolumeFile(*volumeA);
         volumeOutput->setVolumeType(volumeOutputSelectionControl->getSelectedVolumeType());
         createdNewVolume = true;
      }
   }
   volumeOutput->setFileName(volumeOutputSelectionControl->getSelectedVolumeFileName());
   volumeOutput->setDescriptiveLabel(volumeOutputSelectionControl->getSelectedVolumeDescriptiveLabel());
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   try {
      if (combineColumnsRadioButton->isChecked()) {
         if (volumeB == NULL) {
            errorMessage = "Volume B is invalid.";
            return;
         }
         VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_COMBINE_PAINT,
                                            volumeA,
                                            volumeB,
                                            NULL,
                                            volumeOutput);
      }
      else if (addColumnsRadioButton->isChecked()) {
         if (volumeB == NULL) {
            errorMessage = "Volume B is invalid.";
            return;
         }
         VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_ADD,
                                            volumeA,
                                            volumeB,
                                            NULL,
                                            volumeOutput);
      }
      else if (subtractColumnsRadioButton->isChecked()) {
         if (volumeB == NULL) {
            errorMessage = "Volume B is invalid.";
            return;
         }
         VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT,
                                            volumeA,
                                            volumeB,
                                            NULL,
                                            volumeOutput);
      }
      else if (multiplyColumnsRadioButton->isChecked()) {
         if (volumeB == NULL) {
            errorMessage = "Volume B is invalid.";
            return;
         }
         VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_MULTIPLY,
                                            volumeA,
                                            volumeB,
                                            NULL,
                                            volumeOutput);
      }
      else if (divideColumnsRadioButton->isChecked()) {
         if (volumeB == NULL) {
            errorMessage = "Volume B is invalid.";
            return;
         }
         VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_DIVIDE,
                                            volumeA,
                                            volumeB,
                                            NULL,
                                            volumeOutput);
      }
      else if (averageColumnsRadioButton->isChecked()) {
         if (volumeB == NULL) {
            errorMessage = "Volume B is invalid.";
            return;
         }
         VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_AVERAGE,
                                            volumeA,
                                            volumeB,
                                            NULL,
                                            volumeOutput);
      }
      else if (andColumnsRadioButton->isChecked()) {
         if (volumeB == NULL) {
            errorMessage = "Volume B is invalid.";
            return;
         }
         VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_AND,
                                            volumeA,
                                            volumeB,
                                            NULL,
                                            volumeOutput);
      }
      else if (orColumnsRadioButton->isChecked()) {
         if (volumeB == NULL) {
            errorMessage = "Volume B is invalid.";
            return;
         }
         VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            volumeA,
                                            volumeB,
                                            NULL,
                                            volumeOutput);
      }
      else if (copyVolumeRadioButton->isChecked()) {
         *volumeOutput = *volumeA;
         volumeOutput->setFileName(volumeOutputSelectionControl->getSelectedVolumeFileName());
         volumeOutput->setDescriptiveLabel(volumeOutputSelectionControl->getSelectedVolumeDescriptiveLabel());
         volumeOutput->setVolumeType(volumeOutputSelectionControl->getSelectedVolumeType());
      }
      else if (addScalarToColumnRadioButton->isChecked()) {
         VolumeFile::performUnaryOperation(VolumeFile::UNARY_OPERATION_ADD,
                                           volumeA,
                                           volumeOutput,
                                           addScalarDoubleSpinBox->value());
      }
      else if (multiplyColumnByScalarRadioButton->isChecked()) {
         VolumeFile::performUnaryOperation(VolumeFile::UNARY_OPERATION_MULTIPLY,
                                           volumeA,
                                           volumeOutput,
                                           multiplyScalarDoubleSpinBox->value());
      }
      else if (ceilingColumnByScalarRadioButton->isChecked()) {
         VolumeFile::performUnaryOperation(VolumeFile::UNARY_OPERATION_CEILING,
                                           volumeA,
                                           volumeOutput,
                                           ceilingScalarDoubleSpinBox->value());
      }
      else if (floorColumnByScalarRadioButton->isChecked()) {
         VolumeFile::performUnaryOperation(VolumeFile::UNARY_OPERATION_FLOOR,
                                           volumeA,
                                           volumeOutput,
                                           floorScalarDoubleSpinBox->value());
      }
      else if (fixNaNRadioButton->isChecked()) {
         VolumeFile::performUnaryOperation(VolumeFile::UNARY_OPERATION_FIX_NOT_A_NUMBER,
                                           volumeA,
                                           volumeOutput,
                                           0.0);
      }
      else if (absValueRadioButton->isChecked()) {
         VolumeFile::performUnaryOperation(VolumeFile::UNARY_OPERATION_ABS_VALUE,
                                           volumeA,
                                           volumeOutput,
                                           0.0);
      }
      else if (inclusiveSetRangeToZeroRadioButton->isChecked()) {
         VolumeFile::setRangeOfValuesToZero(volumeA,
                                          volumeOutput,
                                          inclusiveSetRangeToZeroMinValueDoubleSpinBox->value(),
                                          inclusiveSetRangeToZeroMaxValueDoubleSpinBox->value(),
                                          true);
      }
      else if (exclusiveSetRangeToZeroRadioButton->isChecked()) {
         VolumeFile::setRangeOfValuesToZero(volumeA,
                                          volumeOutput,
                                          exclusiveSetRangeToZeroMinValueDoubleSpinBox->value(),
                                          exclusiveSetRangeToZeroMaxValueDoubleSpinBox->value(),
                                          false);
      }
      else if (squareRootRadioButton->isChecked()) {
         VolumeFile::performUnaryOperation(VolumeFile::UNARY_OPERATION_SQUARE_ROOT,
                                           volumeA,
                                           volumeOutput,
                                           0.0);
      }
      else if (logRadioButton->isChecked()) {
         VolumeFile::performUnaryOperation(VolumeFile::UNARY_OPERATION_LOG2,
                                           volumeA,
                                           volumeOutput,
                                           logBaseLineEdit->text().toDouble());
      }
      else if (oneMinusValueRadioButton->isChecked()) {
         VolumeFile::performUnaryOperation(VolumeFile::UNARY_OPERATION_SUBTRACT_FROM_ONE,
                                           volumeA,
                                           volumeOutput,
                                           0.0);
      }
      else {
         errorMessage = "No Operation Selected.";
         return;
      }
      
      //
      // Add the new volume if necessary
      //
      if (createdNewVolume) {
         theMainWindow->getBrainSet()->addVolumeFile(volumeOutput->getVolumeType(),
                              volumeOutput,
                              "",
                              true,
                              false);
         volumeOutputSelectionControl->updateControl();
         volumeOutputSelectionControl->setSelectedVolumeFile(volumeOutput);
      }
      
      //
      // Update the volume's coloring
      //
      //BrainModelVolumeVoxelColoring* bmvvc = theMainWindow->getBrainSet()->getVoxelColoring();
      //bmvvc->updateVolumeFileColoring(volumeOutput);
      volumeOutput->setVoxelColoringInvalid();
   }
   catch (FileException& e) {
      errorMessage = e.whatQString();
      if (createdNewVolume) {
         delete volumeOutput;
      }
      return;
   }
}      

/**
 * update the dialog.
 */
void 
GuiDataFileMathDialog::updateDialog()
{
   switch (dialogMode) {
      case DIALOG_MODE_METRIC_FILE:
      case DIALOG_MODE_SURFACE_SHAPE_FILE:
         updateMetricPartOfDialog();
         break;
      case DIALOG_MODE_VOLUME_FILE:
         updateVolumePartOfDialog();
         break;
   }
}

/**
 * update metric part of dialog.
 */
void 
GuiDataFileMathDialog::updateMetricPartOfDialog()
{
   inputMetricColumnComboBoxA->updateComboBox();
   inputMetricColumnComboBoxB->updateComboBox();
   outputMetricColumnComboBox->updateComboBox();
   slotOutputMetricColumnComboBox();
}

/**
 * update volume part of dialog.
 */
void 
GuiDataFileMathDialog::updateVolumePartOfDialog()
{
   volumeASelectionControl->updateControl();
   volumeBSelectionControl->updateControl();
   volumeOutputSelectionControl->updateControl();
}

/**
 * create the operations part of the dialog.
 */
QWidget* 
GuiDataFileMathDialog::createOperationsPartOfDialog()
{
   bool volumeFlag = false;
   QString idName;
   switch (dialogMode) {
      case DIALOG_MODE_METRIC_FILE:
      case DIALOG_MODE_SURFACE_SHAPE_FILE:
         idName = "Column";
         break;
      case DIALOG_MODE_VOLUME_FILE:
         idName = "Volume";
         volumeFlag = true;
         break;
   }

   QWidget* operationsWidget = new QWidget;
   QGridLayout* operationsGridLayout = new QGridLayout(operationsWidget);
   int rowCount = 0;
   //
   // Button group used to keep operation buttons mutually exclusive
   //
   QButtonGroup* operationsButtonGroup = new QButtonGroup(this);
   int opNum = 0;
   QObject::connect(operationsButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotOperationsButtonGroup()));
   
   //
   // combine paint volume
   //
   if (volumeFlag) {
      combineColumnsRadioButton = new QRadioButton("Combine Paint " + idName + " A and Paint " + idName + " B");
      operationsButtonGroup->addButton(combineColumnsRadioButton, opNum++);
      operationsGridLayout->addWidget(combineColumnsRadioButton, rowCount, 0);
      rowCount++;
   }
   else {
      combineColumnsRadioButton = NULL;
   }
   
   //
   // columnA  +  columnB
   //
   addColumnsRadioButton = new QRadioButton("Add " + idName + " A to " + idName + " B");
   if (volumeFlag == false) {
      addColumnsRadioButton->setText("A+B");
   }
   operationsButtonGroup->addButton(addColumnsRadioButton, opNum++);
   operationsGridLayout->addWidget(addColumnsRadioButton, rowCount, 0);
   rowCount++;
   
   //
   // columnA - columnB
   //
   subtractColumnsRadioButton = new QRadioButton("Subtract " + idName + " B from " + idName + " A");
   if (volumeFlag == false) {
      subtractColumnsRadioButton->setText("A-B");
   }
   operationsButtonGroup->addButton(subtractColumnsRadioButton, opNum++);
   operationsGridLayout->addWidget(subtractColumnsRadioButton, rowCount, 0);
   rowCount++;
   
   //
   // columnA * columnB
   //
   multiplyColumnsRadioButton = new QRadioButton("Multiply " + idName + " A by " + idName + " B");
   if (volumeFlag == false) {
      multiplyColumnsRadioButton->setText("A*B");
   }
   operationsButtonGroup->addButton(multiplyColumnsRadioButton, opNum++);
   operationsGridLayout->addWidget(multiplyColumnsRadioButton, rowCount, 0);
   rowCount++;

   
   //
   // columnA / columnB
   //
   divideColumnsRadioButton = new QRadioButton("Divide " + idName + " A by " + idName + " B");
   if (volumeFlag == false) {
      divideColumnsRadioButton->setText("A/B");
   }
   operationsButtonGroup->addButton(divideColumnsRadioButton, opNum++);
   operationsGridLayout->addWidget(divideColumnsRadioButton, rowCount, 0);
   rowCount++;

   
   //
   // average of columnA & columnB
   //
   averageColumnsRadioButton = new QRadioButton("Average of " + idName + " and " + idName + " B");
   if (volumeFlag == false) {
      averageColumnsRadioButton->setText("Average(A,B)");
   }
   operationsButtonGroup->addButton(averageColumnsRadioButton, opNum++);
   operationsGridLayout->addWidget(averageColumnsRadioButton, rowCount, 0);
   rowCount++;
   
   //
   // AND Segmentation volumes
   //
   if (volumeFlag) {
      andColumnsRadioButton = new QRadioButton("AND Segmentation " + idName + " A and Segmentation " + idName + " B");
      operationsButtonGroup->addButton(andColumnsRadioButton, opNum++);
      operationsGridLayout->addWidget(andColumnsRadioButton, rowCount, 0);
      rowCount++;
   }
   else {
      andColumnsRadioButton = NULL;
   }
   
   //
   // OR Segmentation volumes
   //
   if (volumeFlag) {
      orColumnsRadioButton = new QRadioButton("OR Segmentation " + idName + " A and Segmentation " + idName + " B");
      operationsButtonGroup->addButton(orColumnsRadioButton, opNum++);
      operationsGridLayout->addWidget(orColumnsRadioButton, rowCount, 0);
      rowCount++;
   }
   else {
      orColumnsRadioButton = NULL;
   }
   
   //
   // Absolute value
   //
   absValueRadioButton = new QRadioButton("Absolute value in " + idName + " A");
   operationsButtonGroup->addButton(absValueRadioButton, opNum++);
   operationsGridLayout->addWidget(absValueRadioButton, rowCount, 0);
   rowCount++;
   
   //
   // Log
   //
   logRadioButton = new QRadioButton("Log of " + idName + " A, enter base");
   operationsButtonGroup->addButton(logRadioButton, opNum++);
   logBaseLineEdit = new QLineEdit;
   logBaseLineEdit->setObjectName("logBaseLineEdit");
   logBaseLineEdit->setText("2.0");
   logBaseLineEdit->setToolTip( "Enter logarithm\n"
                                  "base here.");
   operationsGridLayout->addWidget(logRadioButton, rowCount, 0);
   operationsGridLayout->addWidget(logBaseLineEdit, rowCount, 1);
   rowCount++;
   
   //
   // Square Root
   //
   squareRootRadioButton = new QRadioButton("Square Root in " + idName + " A");
   operationsButtonGroup->addButton(squareRootRadioButton, opNum++);
   operationsGridLayout->addWidget(squareRootRadioButton, rowCount, 0);
   rowCount++;
   
   //
   // columnA + scalar
   //
   addScalarToColumnRadioButton = new QRadioButton("Add Scalar to " + idName + " A");
   operationsButtonGroup->addButton(addScalarToColumnRadioButton, opNum++);
   addScalarDoubleSpinBox = new QDoubleSpinBox;
   addScalarDoubleSpinBox->setMinimum(-100000.0);
   addScalarDoubleSpinBox->setMaximum(100000.0);
   addScalarDoubleSpinBox->setSingleStep(1.0);
   addScalarDoubleSpinBox->setDecimals(3);
   addScalarDoubleSpinBox->setValue(0.0);
   operationsGridLayout->addWidget(addScalarToColumnRadioButton, rowCount, 0);
   operationsGridLayout->addWidget(addScalarDoubleSpinBox, rowCount, 1);
   rowCount++;
         
   copyVolumeRadioButton = NULL;
   if (volumeFlag) {
      copyVolumeRadioButton = new QRadioButton("Copy " + idName + " A");
      operationsButtonGroup->addButton(copyVolumeRadioButton, opNum++);
      operationsGridLayout->addWidget(copyVolumeRadioButton, rowCount, 0);
      rowCount++;
   }
   
   //
   //  columnA * scalar
   //
   multiplyColumnByScalarRadioButton = new QRadioButton("Multiply " + idName + " A by Scalar");
   operationsButtonGroup->addButton(multiplyColumnByScalarRadioButton, opNum++);
   multiplyScalarDoubleSpinBox = new QDoubleSpinBox;
   multiplyScalarDoubleSpinBox->setMinimum(-100000.0);
   multiplyScalarDoubleSpinBox->setMaximum( 100000.0);
   multiplyScalarDoubleSpinBox->setSingleStep(1.0);
   multiplyScalarDoubleSpinBox->setDecimals(3);
   multiplyScalarDoubleSpinBox->setValue(1.0);
   operationsGridLayout->addWidget(multiplyColumnByScalarRadioButton, rowCount, 0);
   operationsGridLayout->addWidget(multiplyScalarDoubleSpinBox, rowCount, 1);
   rowCount++;
   
   //
   // columnA ceiling
   //
   ceilingColumnByScalarRadioButton = new QRadioButton("Limit Maximum Value in " + idName + " A");
   operationsButtonGroup->addButton(ceilingColumnByScalarRadioButton, opNum++);
   ceilingScalarDoubleSpinBox = new QDoubleSpinBox;
   ceilingScalarDoubleSpinBox->setMinimum(-100000.0);
   ceilingScalarDoubleSpinBox->setMaximum( 100000.0);
   ceilingScalarDoubleSpinBox->setSingleStep(1.0);
   ceilingScalarDoubleSpinBox->setDecimals(3);
   ceilingScalarDoubleSpinBox->setValue(10000.0);
   operationsGridLayout->addWidget(ceilingColumnByScalarRadioButton, rowCount, 0);
   operationsGridLayout->addWidget(ceilingScalarDoubleSpinBox, rowCount, 1);
   rowCount++;
                                                       
   //
   // columnA floor
   //
   floorColumnByScalarRadioButton = new QRadioButton("Limit Minimum Value in " + idName + " A");
   operationsButtonGroup->addButton(floorColumnByScalarRadioButton, opNum++);
   floorScalarDoubleSpinBox = new QDoubleSpinBox;
   floorScalarDoubleSpinBox->setMinimum(-100000.0);
   floorScalarDoubleSpinBox->setMaximum( 100000.0);
   floorScalarDoubleSpinBox->setSingleStep(1.0);
   floorScalarDoubleSpinBox->setDecimals(3);
   floorScalarDoubleSpinBox->setValue(-10000.0);
   operationsGridLayout->addWidget(floorColumnByScalarRadioButton, rowCount, 0);
   operationsGridLayout->addWidget(floorScalarDoubleSpinBox, rowCount, 1);
   rowCount++;
   
   //
   // Fix NaN
   //
   fixNaNRadioButton = new QRadioButton("Fix NaN (Not a Number) in " + idName + " A");
   operationsButtonGroup->addButton(fixNaNRadioButton, opNum++);
   operationsGridLayout->addWidget(fixNaNRadioButton, rowCount, 0);
   rowCount++;
   
   //
   // Inclusive set range to zero 
   //
   inclusiveSetRangeToZeroRadioButton = new QRadioButton("Set Inclusive Range to Zero");
   operationsButtonGroup->addButton(inclusiveSetRangeToZeroRadioButton, opNum++);
   inclusiveSetRangeToZeroMinValueDoubleSpinBox = new QDoubleSpinBox;
   inclusiveSetRangeToZeroMinValueDoubleSpinBox->setMinimum(-100000.0);
   inclusiveSetRangeToZeroMinValueDoubleSpinBox->setMaximum( 100000.0);
   inclusiveSetRangeToZeroMinValueDoubleSpinBox->setSingleStep(1.0);
   inclusiveSetRangeToZeroMinValueDoubleSpinBox->setDecimals(3);
   inclusiveSetRangeToZeroMinValueDoubleSpinBox->setValue(0.0);
   inclusiveSetRangeToZeroMinValueDoubleSpinBox->setToolTip(
                "Minimum value for range.");
   inclusiveSetRangeToZeroMaxValueDoubleSpinBox = new QDoubleSpinBox;
   inclusiveSetRangeToZeroMaxValueDoubleSpinBox->setMinimum(-100000.0);
   inclusiveSetRangeToZeroMaxValueDoubleSpinBox->setMaximum( 100000.0);
   inclusiveSetRangeToZeroMaxValueDoubleSpinBox->setSingleStep(1.0);
   inclusiveSetRangeToZeroMaxValueDoubleSpinBox->setDecimals(3);
   inclusiveSetRangeToZeroMaxValueDoubleSpinBox->setValue(0.0);
   inclusiveSetRangeToZeroMaxValueDoubleSpinBox->setToolTip(
                "Maximum value for range.");
   operationsGridLayout->addWidget(inclusiveSetRangeToZeroRadioButton, rowCount, 0);
   operationsGridLayout->addWidget(inclusiveSetRangeToZeroMinValueDoubleSpinBox, rowCount, 1);
   operationsGridLayout->addWidget(inclusiveSetRangeToZeroMaxValueDoubleSpinBox, rowCount, 2);
   rowCount++;

   //
   // Inclusive set range to zero 
   //
   exclusiveSetRangeToZeroRadioButton = new QRadioButton("Set Exclusive Range to Zero");
   operationsButtonGroup->addButton(exclusiveSetRangeToZeroRadioButton, opNum++);
   exclusiveSetRangeToZeroMinValueDoubleSpinBox = new QDoubleSpinBox;
   exclusiveSetRangeToZeroMinValueDoubleSpinBox->setMinimum(-100000.0);
   exclusiveSetRangeToZeroMinValueDoubleSpinBox->setMaximum( 100000.0);
   exclusiveSetRangeToZeroMinValueDoubleSpinBox->setSingleStep(1.0);
   exclusiveSetRangeToZeroMinValueDoubleSpinBox->setDecimals(3);
   exclusiveSetRangeToZeroMinValueDoubleSpinBox->setValue(0.0);
   exclusiveSetRangeToZeroMinValueDoubleSpinBox->setToolTip(
                "Minimum value for range.");
   exclusiveSetRangeToZeroMaxValueDoubleSpinBox = new QDoubleSpinBox;
   exclusiveSetRangeToZeroMaxValueDoubleSpinBox->setMinimum(-100000.0);
   exclusiveSetRangeToZeroMaxValueDoubleSpinBox->setMaximum( 100000.0);
   exclusiveSetRangeToZeroMaxValueDoubleSpinBox->setSingleStep(1.0);
   exclusiveSetRangeToZeroMaxValueDoubleSpinBox->setDecimals(3);
   exclusiveSetRangeToZeroMaxValueDoubleSpinBox->setValue(0.0);
   exclusiveSetRangeToZeroMaxValueDoubleSpinBox->setToolTip(
                "Maximum value for range.");
   operationsGridLayout->addWidget(exclusiveSetRangeToZeroRadioButton, rowCount, 0);
   operationsGridLayout->addWidget(exclusiveSetRangeToZeroMinValueDoubleSpinBox, rowCount, 1);
   operationsGridLayout->addWidget(exclusiveSetRangeToZeroMaxValueDoubleSpinBox, rowCount, 2);
   rowCount++;

   normalizeValuesRadioButton = NULL;
   if (volumeFlag == false) {
      normalizeValuesRadioButton = new QRadioButton("Normalize " + idName + " A");
      operationsButtonGroup->addButton(normalizeValuesRadioButton, opNum++);
      normalizeValuesRadioButton->setToolTip(
                    "Redistributes values so that they fit\n"
                    "a normal distribution.  The data's median\n"
                    "value is mapped to the mean value of the\n"
                    "normal distribution.");

      normalizeMeanDoubleSpinBox = new QDoubleSpinBox;
      normalizeMeanDoubleSpinBox->setMinimum(-10000.0);
      normalizeMeanDoubleSpinBox->setMaximum( 10000.0);
      normalizeMeanDoubleSpinBox->setSingleStep(1.0);
      normalizeMeanDoubleSpinBox->setDecimals(3);
      normalizeMeanDoubleSpinBox->setValue(0.0);
      normalizeMeanDoubleSpinBox->setToolTip(
                    "Mean of Normal Distribution");

      normalizeDeviationDoubleSpinBox = new QDoubleSpinBox;
      normalizeDeviationDoubleSpinBox->setMinimum(-10000.0);
      normalizeDeviationDoubleSpinBox->setMaximum( 10000.0);
      normalizeDeviationDoubleSpinBox->setSingleStep(1.0);
      normalizeDeviationDoubleSpinBox->setDecimals(3);
      normalizeDeviationDoubleSpinBox->setValue(1.0);
      normalizeDeviationDoubleSpinBox->setToolTip(
                    "Deviation of Normal Distribution");
      operationsGridLayout->addWidget(normalizeValuesRadioButton, rowCount, 0);
      operationsGridLayout->addWidget(normalizeMeanDoubleSpinBox, rowCount, 1);
      operationsGridLayout->addWidget(normalizeDeviationDoubleSpinBox, rowCount, 2);
      rowCount++;
   }
   
   //
   // 1.0 - value
   //
   oneMinusValueRadioButton = new QRadioButton("1.0 - value in " + idName + " A");
   operationsButtonGroup->addButton(oneMinusValueRadioButton, opNum++);
   operationsGridLayout->addWidget(oneMinusValueRadioButton, rowCount, 0);
   rowCount++;
   
   operationsWidget->setFixedSize(operationsWidget->sizeHint());
   
   QGroupBox* operationsGroupBox = new QGroupBox("Operations");
   QVBoxLayout* operationsLayout = new QVBoxLayout(operationsGroupBox);
   
   QScrollArea* scrollArea = new QScrollArea;
   scrollArea->setWidget(operationsWidget);
   scrollArea->setWidgetResizable(true);
   operationsLayout->addWidget(scrollArea);
   
   return operationsGroupBox;
}

/**
 * called when an operations radio button is selected.
 */
void 
GuiDataFileMathDialog::slotOperationsButtonGroup()
{
   bool enableColumnB = true;
   
   if (multiplyColumnByScalarRadioButton->isChecked()  ||
       addScalarToColumnRadioButton->isChecked()       ||
       ceilingColumnByScalarRadioButton->isChecked()   ||
       floorColumnByScalarRadioButton->isChecked()     ||
       fixNaNRadioButton->isChecked()                  ||
       absValueRadioButton->isChecked()                ||
       inclusiveSetRangeToZeroRadioButton->isChecked() ||
       exclusiveSetRangeToZeroRadioButton->isChecked()) {
      enableColumnB = false;
   }
   else if (copyVolumeRadioButton != NULL) {
      if (copyVolumeRadioButton->isChecked()) {
         enableColumnB = false;
      }
   }
   else if (normalizeValuesRadioButton != NULL) {
      if (normalizeValuesRadioButton->isChecked()) {
         enableColumnB = false;
      }
   }
   
   switch (dialogMode) {
      case DIALOG_MODE_METRIC_FILE:
      case DIALOG_MODE_SURFACE_SHAPE_FILE:
         inputMetricColumnComboBoxB->setEnabled(enableColumnB);
         inputMetricColumnLabelB->setEnabled(enableColumnB);
         break;
      case DIALOG_MODE_VOLUME_FILE:
         volumeBGroupBox->setEnabled(enableColumnB);
         break;
   }
}

/**
 * create the volume part of the dialog.
 */
QWidget* 
GuiDataFileMathDialog::createVolumePartOfDialog()
{  
   QWidget* volumeWidget = new QWidget;
   QVBoxLayout* volumeLayout = new QVBoxLayout(volumeWidget);
   
   //
   // Volume A
   //
   QGroupBox* volumeAGroupBox = new QGroupBox("Volume A");
   QVBoxLayout* volumeALayout = new QVBoxLayout(volumeAGroupBox);
   volumeASelectionControl = new GuiVolumeSelectionControl(0,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                           "volumeASelectionControl",
                                                           false,
                                                           false,
                                                           false);
   volumeALayout->addWidget(volumeASelectionControl);
   volumeLayout->addWidget(volumeAGroupBox);
   
   //
   // Volume B
   //
   volumeBGroupBox = new QGroupBox("Volume B");
   QVBoxLayout* volumeBLayout = new QVBoxLayout(volumeBGroupBox);
   volumeBSelectionControl = new GuiVolumeSelectionControl(0,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                           "volumeASelectionControl",
                                                           false,
                                                           false,
                                                           false);
   volumeBLayout->addWidget(volumeBSelectionControl);
   volumeLayout->addWidget(volumeBGroupBox);

   //
   // Output Volume 
   //
   QGroupBox* volumeCGroupBox = new QGroupBox("Volume Output");
   QVBoxLayout* volumeCLayout = new QVBoxLayout(volumeCGroupBox);
   volumeOutputSelectionControl = new GuiVolumeSelectionControl(0,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                                           true,
                                     GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                           "volumeOutputSelectionControl",
                                                           true,
                                                           true,
                                                           true);
   volumeCLayout->addWidget(volumeOutputSelectionControl);
   volumeLayout->addWidget(volumeCGroupBox);

   return volumeWidget;
}

/**
 * create the metric column part of the dialog.
 */
QWidget* 
GuiDataFileMathDialog::createMetricColumnPartOfDialog()
{  
   GUI_NODE_FILE_TYPE nodeFileType = GUI_NODE_FILE_TYPE_NONE;
   QString title; 
   switch (dialogMode) {
      case DIALOG_MODE_METRIC_FILE:
         nodeFileType = GUI_NODE_FILE_TYPE_METRIC;
         title = "Metric Columns";
         break;
      case DIALOG_MODE_SURFACE_SHAPE_FILE:
         nodeFileType = GUI_NODE_FILE_TYPE_SURFACE_SHAPE;
         title = "Shape Columns";
         break;
      case DIALOG_MODE_VOLUME_FILE:
         title = "";
         break;
   }

   QGroupBox* columnGroupBox = new QGroupBox(title);
   QGridLayout* columnGridLayout = new QGridLayout(columnGroupBox);
   int rowCount = 0;
   
   //
   // input metric column "A"
   //
   columnGridLayout->addWidget(new QLabel("Column A"), rowCount, 0);
   inputMetricColumnComboBoxA = new GuiNodeAttributeColumnSelectionComboBox(
                                        nodeFileType,
                                        false,
                                        false,
                                        false);
   columnGridLayout->addWidget(inputMetricColumnComboBoxA, rowCount, 1);
   rowCount++;
   
   //
   // input metric column "B"
   //
   inputMetricColumnLabelB = new QLabel("Column B");
   columnGridLayout->addWidget(inputMetricColumnLabelB, rowCount, 0);
   inputMetricColumnComboBoxB = new GuiNodeAttributeColumnSelectionComboBox(
                                        nodeFileType,
                                        false,
                                        false,
                                        false);
   columnGridLayout->addWidget(inputMetricColumnComboBoxB, rowCount, 1);
   rowCount++;
   
   //
   // output metric column
   //
   columnGridLayout->addWidget(new QLabel("Result"), rowCount, 0);
   outputMetricColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                        nodeFileType,
                                        true,
                                        false,
                                        false);
   columnGridLayout->addWidget(outputMetricColumnComboBox, rowCount, 1);
   QObject::connect(outputMetricColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotOutputMetricColumnComboBox()));
   outputMetricColumnComboBox->setCurrentIndex(
                    GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
                    
   outputMetricNameLineEdit = new QLineEdit;
   columnGridLayout->addWidget(outputMetricNameLineEdit, rowCount, 2);
   
   return columnGroupBox;
}

/**
 * get the data file since.
 */
MetricFile* 
GuiDataFileMathDialog::getNodeDataFile()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   MetricFile* mf = NULL;
   switch (dialogMode) {
      case DIALOG_MODE_METRIC_FILE:
         mf = bs->getMetricFile();
         break;
      case DIALOG_MODE_SURFACE_SHAPE_FILE:
         mf = bs->getSurfaceShapeFile();
         break;
      case DIALOG_MODE_VOLUME_FILE:
         break;
   }
   return mf;
}
      
/**
 * called when output column selected.
 */
void 
GuiDataFileMathDialog::slotOutputMetricColumnComboBox()
{
   const int metricColumn = outputMetricColumnComboBox->currentIndex();
   if ((metricColumn >= 0) && (metricColumn < getNodeDataFile()->getNumberOfColumns())) {
      outputMetricNameLineEdit->setText(getNodeDataFile()->getColumnName(metricColumn));
   }
   else {
      outputMetricNameLineEdit->setText("New Column Name");
   }
}



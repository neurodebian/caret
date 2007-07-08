
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

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextStream>

#include "BrainModelSurfaceMetricOneAndPairedTTest.h"
#include "FileUtilities.h"
#include "GuiDataFileDialog.h"
#include "GuiFileSelectionButton.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiMetricShapeOneAndPairedTTestDialog.h"
#include "SurfaceShapeFile.h"
#include "global_variables.h"

/**
 * constructor
 */
GuiMetricShapeOneAndPairedTTestDialog::GuiMetricShapeOneAndPairedTTestDialog(QWidget* parent,
                                                                  const bool pairedFlagIn)
   : QtDialogNonModal(parent)
{
   pairedFlag = pairedFlagIn;
      
   if (pairedFlag) {
      setWindowTitle("Find Clusters Using Paired T-Test");
   }
   else {
      setWindowTitle("Find Clusters Using One-Sampled T-Test");
   }
   
   lineEditWidth = 300;
   numericMaxWidth = 150;

   //
   // Tab widget
   //
   tabby = new QTabWidget(this);
   
   //
   // create input widget
   //
   tabby->addTab(createInputWidget(), "Inputs");
   tabby->addTab(createOutputWidget(), "Outputs");
   reportWidget = createReportWidget();
   tabby->addTab(reportWidget, "Report");
   
   //
   // layout for dialog
   //
   QVBoxLayout* dialogLayout = getDialogLayout();
   dialogLayout->addWidget(tabby);
   
   // connect apply/close signals
   //
   QObject::connect(this, SIGNAL(signalApplyButtonPressed()),
                    this, SLOT(slotApplyButton()));
   QObject::connect(this, SIGNAL(signalCloseButtonPressed()),
                    this, SLOT(close())); 
                    
   //
   // Enable and connect help button
   //
   QObject::connect(this, SIGNAL(signalHelpButtonPressed()),
                    this, SLOT(slotShowHelpPage())); 
   showHelpButton(true);
   
   slotAreaCorrectionShapeFile("");
}   

/**
 * destructor.
 */
GuiMetricShapeOneAndPairedTTestDialog::~GuiMetricShapeOneAndPairedTTestDialog()
{
}

/**
 * show the help page.
 */
void 
GuiMetricShapeOneAndPairedTTestDialog::slotShowHelpPage()
{
   theMainWindow->showHelpViewerDialog("statistics/one_sample_t_test.html");
}
      
/**
 * create the input widget.
 */
QWidget* 
GuiMetricShapeOneAndPairedTTestDialog::createInputWidget()
{   
   //
   // metric/shape button and line edit
   //
   GuiFileSelectionButton* metricFileButton = new GuiFileSelectionButton(0,
                                                                    "Metric/Shape File",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    true);
   metricFileLineEdit = new QLineEdit;
   metricFileLineEdit->setReadOnly(true);
   metricFileLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(metricFileButton, SIGNAL(fileSelected(const QString&)),
                    metricFileLineEdit, SLOT(setText(const QString&)));
   QObject::connect(metricFileButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotMetricShapeFileSelected(const QString&)));

   //
   // metric/shape button and line edit
   //
   GuiFileSelectionButton* metricFileTwoButton = new GuiFileSelectionButton(0,
                                                                "Metric/Shape File Two",
                                                                GuiDataFileDialog::metricShapeFileFilter,
                                                                true);
   metricFileTwoLineEdit = new QLineEdit;
   metricFileTwoLineEdit->setReadOnly(true);
   metricFileTwoLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(metricFileTwoButton, SIGNAL(fileSelected(const QString&)),
                    metricFileTwoLineEdit, SLOT(setText(const QString&)));

   //
   // fiducial coord file button and line edit
   //
   GuiFileSelectionButton* fiducialCoordFileButton = new GuiFileSelectionButton(0,
                                                                    "Fiducial Coord File",
                                                                    GuiDataFileDialog::coordinateFiducialFileFilter,
                                                                    true);
   fiducialCoordFileLineEdit = new QLineEdit;
   fiducialCoordFileLineEdit->setReadOnly(true);
   fiducialCoordFileLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(fiducialCoordFileButton, SIGNAL(fileSelected(const QString&)),
                    fiducialCoordFileLineEdit, SLOT(setText(const QString&)));

   //
   // open topo file button and line edit
   //
   GuiFileSelectionButton* openTopoFileButton = new GuiFileSelectionButton(0,
                                                                    "Open Topology File",
                                                                    GuiDataFileDialog::topologyOpenFileFilter,
                                                                    true);
   openTopoFileLineEdit = new QLineEdit;
   openTopoFileLineEdit->setReadOnly(true);
   openTopoFileLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(openTopoFileButton, SIGNAL(fileSelected(const QString&)),
                    openTopoFileLineEdit, SLOT(setText(const QString&)));

   //
   // area correction shapefile button and line edit
   //
   GuiFileSelectionButton* areaCorrectionShapeFileButton = new GuiFileSelectionButton(0,
                                                                    "Area Correction Metric/Shape File",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    true);
   areaCorrectionShapeFileLineEdit = new QLineEdit;
   areaCorrectionShapeFileLineEdit->setReadOnly(true);
   areaCorrectionShapeFileLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(areaCorrectionShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    areaCorrectionShapeFileLineEdit, SLOT(setText(const QString&)));
   QObject::connect(areaCorrectionShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotAreaCorrectionShapeFile(const QString&)));

   //
   // area correction shape file column
   //
   QLabel* shapeCorrectionColumnLabel = new QLabel("Area Correction Shape Column");
   areaCorrectionShapeColumnComboBox = new QComboBox;
   
   //
   // Cluster negative threshold
   //
   QLabel* negThreshLabel = new QLabel("Cluster Negative Threshold");
   negThreshDoubleSpinBox = new QDoubleSpinBox;
   negThreshDoubleSpinBox->setMinimum(-500000.0);
   negThreshDoubleSpinBox->setMaximum(0);
   negThreshDoubleSpinBox->setSingleStep(1.0);
   negThreshDoubleSpinBox->setDecimals(3);
   negThreshDoubleSpinBox->setFixedWidth(numericMaxWidth);
   negThreshDoubleSpinBox->setValue(-3.0);
   
   //
   // Cluster positive threshold
   //
   QLabel* posThreshLabel = new QLabel("Cluster Positive Threshold");
   posThreshDoubleSpinBox = new QDoubleSpinBox;
   posThreshDoubleSpinBox->setMinimum(0.0);
   posThreshDoubleSpinBox->setMaximum(500000.0);
   posThreshDoubleSpinBox->setSingleStep(1.0);
   posThreshDoubleSpinBox->setDecimals(3);
   posThreshDoubleSpinBox->setFixedWidth(numericMaxWidth);
   posThreshDoubleSpinBox->setValue(3.0);
   
   //
   // Shuffled T-Map Iterations
   //
   QLabel* permutedTMapIterationsLabel = new QLabel("Permuted T-Map Iterations");
   permutedTMapIterationSpinBox = new QSpinBox;
   permutedTMapIterationSpinBox->setMinimum(0);
   permutedTMapIterationSpinBox->setMaximum(10000000);
   permutedTMapIterationSpinBox->setSingleStep(10);
   permutedTMapIterationSpinBox->setFixedWidth(numericMaxWidth);
   permutedTMapIterationSpinBox->setValue(128);
   
   //
   // P-Value
   //
   QLabel* pValueLabel = new QLabel("P-Value (Type I Error)");
   pValueDoubleSpinBox = new QDoubleSpinBox;
   pValueDoubleSpinBox->setMinimum(0.0);
   pValueDoubleSpinBox->setMaximum(1.0);
   pValueDoubleSpinBox->setSingleStep(0.05);
   pValueDoubleSpinBox->setDecimals(5);
   pValueDoubleSpinBox->setFixedWidth(numericMaxWidth);
   pValueDoubleSpinBox->setValue(0.05);
   
   //
   // T-Test constant
   //
   QLabel* tTestLabel = new QLabel("T-Test Constant");
   tTestConstantDoubleSpinBox = new QDoubleSpinBox;
   tTestConstantDoubleSpinBox->setMinimum(-100000000.0);
   tTestConstantDoubleSpinBox->setMaximum( 100000000.0);
   tTestConstantDoubleSpinBox->setSingleStep(1.0);
   tTestConstantDoubleSpinBox->setDecimals(3);
   tTestConstantDoubleSpinBox->setFixedWidth(numericMaxWidth);
   tTestConstantDoubleSpinBox->setValue(0.0);
   
   //
   // Number of threads
   //
   QLabel* threadsLabel = new QLabel("Number of Threads");
   threadsSpinBox = new QSpinBox;
   threadsSpinBox->setMinimum(1);
   threadsSpinBox->setMaximum(2048);
   threadsSpinBox->setSingleStep(1);
   threadsSpinBox->setFixedWidth(numericMaxWidth);
   threadsSpinBox->setValue(1);
   
   //
   // Widgets for input
   //
   QGroupBox* inputsWidget = new QGroupBox("Inputs");
   QGridLayout* grid = new QGridLayout(inputsWidget);
   grid->setMargin(5);
   grid->setSpacing(5);
   
   //
   // arrange dialog items
   //
   grid->addWidget(metricFileButton, 0, 0);
   grid->addWidget(metricFileLineEdit, 0, 1);
   grid->addWidget(metricFileTwoButton, 1, 0);
   grid->addWidget(metricFileTwoLineEdit, 1, 1);
   grid->addWidget(fiducialCoordFileButton, 2, 0);
   grid->addWidget(fiducialCoordFileLineEdit, 2, 1);
   grid->addWidget(openTopoFileButton, 3, 0);
   grid->addWidget(openTopoFileLineEdit, 3, 1);
   grid->addWidget(areaCorrectionShapeFileButton, 4, 0);
   grid->addWidget(areaCorrectionShapeFileLineEdit, 4, 1);
   grid->addWidget(shapeCorrectionColumnLabel, 5, 0);
   grid->addWidget(areaCorrectionShapeColumnComboBox, 5, 1);
   grid->addWidget(negThreshLabel, 6, 0);
   grid->addWidget(negThreshDoubleSpinBox, 6, 1, Qt::AlignLeft);
   grid->addWidget(posThreshLabel, 7, 0);
   grid->addWidget(posThreshDoubleSpinBox, 7, 1, Qt::AlignLeft);
   grid->addWidget(permutedTMapIterationsLabel, 8, 0);
   grid->addWidget(permutedTMapIterationSpinBox, 8, 1, Qt::AlignLeft);
   grid->addWidget(pValueLabel, 9, 0);
   grid->addWidget(pValueDoubleSpinBox, 9, 1, Qt::AlignLeft);
   grid->addWidget(tTestLabel, 10, 0);
   grid->addWidget(tTestConstantDoubleSpinBox, 10, 1);
   grid->addWidget(threadsLabel, 11, 0);
   grid->addWidget(threadsSpinBox, 11, 1);
   
   
   if (pairedFlag) {
      tTestLabel->hide();
      tTestConstantDoubleSpinBox->hide();
   }
   else {
      metricFileTwoButton->hide();
      metricFileTwoLineEdit->hide();
   }
   
   //
   // widget and grid layout
   //
   QWidget* w = new QWidget;  
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(inputsWidget);
   layout->addWidget(createVarianceSmoothingWidget());
   return w;
}

/**
 * called when shape/metric file one is selected.
 */
void 
GuiMetricShapeOneAndPairedTTestDialog::slotMetricShapeFileSelected(const QString& name)
{
   static bool firstTime = true;
   if (firstTime) {
      firstTime = false;
      
      QString extension = SpecFile::getSurfaceShapeFileExtension();
      if (name.endsWith(SpecFile::getMetricFileExtension())) {
         extension = SpecFile::getMetricFileExtension();
      }
      else {
         extension = SpecFile::getSurfaceShapeFileExtension();
      }
      
      tMapMetricFileLineEdit->setText("TMap" + extension);
      permutedTMapMetricFileLineEdit->setText("Permuted_TMap" + extension);
   }
}
      
/**
 * create the output widget.
 */
QWidget* 
GuiMetricShapeOneAndPairedTTestDialog::createOutputWidget()
{
   //
   // t-map shape file B button and line edit
   //
   GuiFileSelectionButton* tMapMetricFileButton = new GuiFileSelectionButton(0,
                                                                    "T-Map Metric/Shape File",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    false);
   tMapMetricFileLineEdit = new QLineEdit;
   tMapMetricFileLineEdit->setReadOnly(false);
   tMapMetricFileLineEdit->setMinimumWidth(lineEditWidth);
   tMapMetricFileLineEdit->setText("TMap.metric");
   QObject::connect(tMapMetricFileButton, SIGNAL(fileSelected(const QString&)),
                    tMapMetricFileLineEdit, SLOT(setText(const QString&)));

   //
   // shuffled T-map file  button and line edit
   //
   GuiFileSelectionButton* permutedTMapMetricFileButton = new GuiFileSelectionButton(0,
                                                                    "Permuted T-Map Metric/Shape File",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    false);
   permutedTMapMetricFileLineEdit = new QLineEdit;
   permutedTMapMetricFileLineEdit->setReadOnly(false);
   permutedTMapMetricFileLineEdit->setMinimumWidth(lineEditWidth);
   permutedTMapMetricFileLineEdit->setText("Permuted_TMap.metric");
   QObject::connect(permutedTMapMetricFileButton, SIGNAL(fileSelected(const QString&)),
                    permutedTMapMetricFileLineEdit, SLOT(setText(const QString&)));

   //
   // paint file  button and line edit
   //
   GuiFileSelectionButton* paintFileButton = new GuiFileSelectionButton(0,
                                                                    "Paint File",
                                                                    GuiDataFileDialog::paintFileFilter,
                                                                    true);
   paintFileLineEdit = new QLineEdit;
   paintFileLineEdit->setReadOnly(false);
   paintFileLineEdit->setMinimumWidth(lineEditWidth);
   paintFileLineEdit->setText("clusters.paint");
   QObject::connect(paintFileButton, SIGNAL(fileSelected(const QString&)),
                    paintFileLineEdit, SLOT(setText(const QString&)));

   //
   // report file button and line edit
   //
   GuiFileSelectionButton* reportFileButton = new GuiFileSelectionButton(0,
                                                                    "Report File",
                                                                    GuiDataFileDialog::textFileFilter,
                                                                    false);
   reportFileLineEdit = new QLineEdit;
   reportFileLineEdit->setReadOnly(false);
   reportFileLineEdit->setMinimumWidth(lineEditWidth);
   reportFileLineEdit->setText("Significant_Clusters.txt");
   QObject::connect(reportFileButton, SIGNAL(fileSelected(const QString&)),
                    reportFileLineEdit, SLOT(setText(const QString&)));

   
   //
   // grid layout
   //
   QWidget* w = new QWidget;
   QGridLayout* grid = new QGridLayout(w);
   grid->setMargin(5);
   grid->setSpacing(5);
   
   //
   // arrange dialog items
   //
   grid->addWidget(tMapMetricFileButton, 0, 0);
   grid->addWidget(tMapMetricFileLineEdit, 0, 1);
   grid->addWidget(permutedTMapMetricFileButton, 1, 0);
   grid->addWidget(permutedTMapMetricFileLineEdit, 1, 1);
   grid->addWidget(paintFileButton, 2, 0);
   grid->addWidget(paintFileLineEdit, 2, 1);
   grid->addWidget(reportFileButton, 3, 0);
   grid->addWidget(reportFileLineEdit, 3, 1);
   grid->addWidget(new QWidget, 4, 0);
   grid->setRowStretch(4, 1000);
   
   return w;
}

/**
 * create the report widget.
 */
QWidget* 
GuiMetricShapeOneAndPairedTTestDialog::createReportWidget()
{
   //
   // Text edit for report
   //
   reportTextEdit = new QTextEdit;

   //
   // Widget for holding inputs
   //
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(reportTextEdit);
   
   return w;
}

/**
 * create the variance smoothing options widget.
 */
QGroupBox* 
GuiMetricShapeOneAndPairedTTestDialog::createVarianceSmoothingWidget()
{
   //
   // variance smoothing iterations
   //
   QLabel* varianceSmoothingIterationsLabel = new QLabel("Iterations");
   varianceSmoothingIterationsSpinBox = new QSpinBox;
   varianceSmoothingIterationsSpinBox->setMinimum(0);
   varianceSmoothingIterationsSpinBox->setMaximum(1000000);
   varianceSmoothingIterationsSpinBox->setSingleStep(10);
   varianceSmoothingIterationsSpinBox->setValue(10);
   varianceSmoothingIterationsSpinBox->setMaximumWidth(140);
   
   //
   // variance smoothing strength
   //
   QLabel* varianceSmoothingStrengthLabel = new QLabel("Strength");
   varianceSmoothingStrengthSpinBox = new QDoubleSpinBox;
   varianceSmoothingStrengthSpinBox->setMinimum(0.0);
   varianceSmoothingStrengthSpinBox->setMaximum(1.0);
   varianceSmoothingStrengthSpinBox->setSingleStep(0.01);
   varianceSmoothingStrengthSpinBox->setDecimals(2);
   varianceSmoothingStrengthSpinBox->setValue(1.0);
   varianceSmoothingStrengthSpinBox->setMaximumWidth(140);
   
   //
   // Variance smoothing group box
   //
   varianceSmoothingGroupBox = new QGroupBox("Variance Smoothing");
   varianceSmoothingGroupBox->setCheckable(true);
   QGridLayout* varianceSmoothingLayout = new QGridLayout(varianceSmoothingGroupBox);
   varianceSmoothingLayout->addWidget(varianceSmoothingIterationsLabel, 0, 0);
   varianceSmoothingLayout->addWidget(varianceSmoothingIterationsSpinBox, 0, 1);
   varianceSmoothingLayout->addWidget(varianceSmoothingStrengthLabel, 1, 0);
   varianceSmoothingLayout->addWidget(varianceSmoothingStrengthSpinBox, 1, 1);
   varianceSmoothingGroupBox->setChecked(false);
   varianceSmoothingGroupBox->setFixedSize(varianceSmoothingGroupBox->sizeHint());
   
   return varianceSmoothingGroupBox;
}
      
/**
 * called when area correction shape file is selected.
 */
void 
GuiMetricShapeOneAndPairedTTestDialog::slotAreaCorrectionShapeFile(const QString& name)
{
   areaCorrectionShapeColumnComboBox->clear();
   
   if (name.isEmpty() == false) {
      try {
         SurfaceShapeFile ssf;
         ssf.readFileMetaDataOnly(name);
         for (int i = 0; i < ssf.getNumberOfColumns(); i++) {
            areaCorrectionShapeColumnComboBox->addItem(ssf.getColumnName(i));
         }
      }
      catch (FileException& e) {
         GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
      }
   }
}

/**
 * update the dialog.
 */
void 
GuiMetricShapeOneAndPairedTTestDialog::updateDialog()
{
}
      
/**
 * called when user presses apply button
 */
void 
GuiMetricShapeOneAndPairedTTestDialog::slotApplyButton()
{
   //
   // overwrite files check
   //
   const QString tMapFileName(tMapMetricFileLineEdit->text());
   if (QFile::exists(tMapFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(tMapFileName).toAscii().constData()
          << " ?";
      if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
         return;
      }
   }
   const QString permutedTMapFileName(permutedTMapMetricFileLineEdit->text());
   if (QFile::exists(permutedTMapFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(permutedTMapFileName).toAscii().constData()
          << " ?";
      if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
         return;
      }
   }
   const QString reportFileName(reportFileLineEdit->text());
   if (QFile::exists(reportFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(reportFileName).toAscii().constData()
          << " ?";
      if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
         return;
      }
   }
   const QString paintFileName(paintFileLineEdit->text());
   if (paintFileName.isEmpty() == false) {
      if (QFile::exists(paintFileName)) {
         std::ostringstream str;
         str << "Overwrite "
             << FileUtilities::basename(paintFileName).toAscii().constData()
             << " ?";
         if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
            return;
         }
      }
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   try {
      //
      // Set the mode and test-constant
      //
      float testConstant = 0;
      BrainModelSurfaceMetricOneAndPairedTTest::T_TEST_MODE mode = 
           BrainModelSurfaceMetricOneAndPairedTTest::T_TEST_MODE_ONE_SAMPLE;
      if (pairedFlag) {
         mode = BrainModelSurfaceMetricOneAndPairedTTest::T_TEST_MODE_PAIRED;
      }
      else {
         testConstant = tTestConstantDoubleSpinBox->value();
      }
      
      //
      // Get the names of the metric files
      //
      std::vector<QString> metricFileNames;
      metricFileNames.push_back(metricFileLineEdit->text());
      metricFileNames.push_back(metricFileTwoLineEdit->text());
      
      int varianceSmoothingIterations = 0;
      float varianceSmoothingStrength = 0.0;
      if (varianceSmoothingGroupBox->isEnabled() &&
          varianceSmoothingGroupBox->isChecked()) {
         varianceSmoothingIterations = varianceSmoothingIterationsSpinBox->value();
         varianceSmoothingStrength = varianceSmoothingStrengthSpinBox->value();
      }

      //
      // Create the algorithm and execute
      //
      BrainModelSurfaceMetricOneAndPairedTTest 
            ssc(theMainWindow->getBrainSet(),
                mode,
                metricFileNames,
                fiducialCoordFileLineEdit->text(),
                openTopoFileLineEdit->text(),
                areaCorrectionShapeFileLineEdit->text(),
                tMapFileName,
                permutedTMapFileName,
                paintFileName,
                "",   // metric clusters
                reportFileName,
                areaCorrectionShapeColumnComboBox->currentIndex(),
                negThreshDoubleSpinBox->value(),
                posThreshDoubleSpinBox->value(),
                pValueDoubleSpinBox->value(),
                varianceSmoothingIterations,
                varianceSmoothingStrength,
                permutedTMapIterationSpinBox->value(),
                testConstant,
                threadsSpinBox->value());
         
      ssc.execute();
      QApplication::restoreOverrideCursor();   
   }
   catch (BrainModelAlgorithmException &e) {
      GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
      return;
   }
   
   //
   // Put the report into the report text edit
   //
   QFile reportFile(reportFileName);
   if (reportFile.open(QIODevice::ReadOnly)) {
      QTextStream stream(&reportFile);
      reportTextEdit->setPlainText(stream.readAll());
      QTextCursor tc;
      tc.movePosition(QTextCursor::Start);
      reportTextEdit->setTextCursor(tc);
      tabby->setCurrentWidget(reportWidget);
   }
   reportFile.close();
   
   beep();
}

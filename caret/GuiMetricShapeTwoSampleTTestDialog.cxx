
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

#include <QCheckBox>
#include <QComboBox>
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

#include "FileUtilities.h"
#include "GuiDataFileDialog.h"
#include "GuiFileSelectionButton.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiMetricShapeTwoSampleTTestDialog.h"
#include <QDoubleSpinBox>
#include "SurfaceShapeFile.h"
#include "global_variables.h"

/**
 * constructor
 */
GuiMetricShapeTwoSampleTTestDialog::GuiMetricShapeTwoSampleTTestDialog(QWidget* parent,
                                  const BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_MODE dataTransformModeIn)
   : QtDialogNonModal(parent)
{
   dataTransformMode = dataTransformModeIn;
   
   switch (dataTransformMode) {
      case BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_NONE:
         setWindowTitle("Two Sample T-Test");
         break;
      case BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_WILCOXON_RANK_SUM_THEN_TWO_SAMPLE_T_TEST:
         setWindowTitle("Wilcoxon Rank-Sum Into T-Test");
         break;
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
   tabby->addTab(createOptionsWidget(), "Options");
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
GuiMetricShapeTwoSampleTTestDialog::~GuiMetricShapeTwoSampleTTestDialog()
{
}

/**
 * show the help page.
 */
void 
GuiMetricShapeTwoSampleTTestDialog::slotShowHelpPage()
{
   switch (dataTransformMode) {
      case BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_NONE:
         theMainWindow->showHelpViewerDialog("statistics/two_sample_t_test.html");
         break;
      case BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_WILCOXON_RANK_SUM_THEN_TWO_SAMPLE_T_TEST:
         theMainWindow->showHelpViewerDialog("statistics/wilcoxon_rank_sum.html");
         break;
   }
}
      
/**
 * create the input widget.
 */
QWidget* 
GuiMetricShapeTwoSampleTTestDialog::createInputWidget()
{   
   //
   // shape file A button and line edit
   //
   GuiFileSelectionButton* shapeFileAButton = new GuiFileSelectionButton(0,
                                                                    "Metric/Shape File A",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    true);
   shapeFileALineEdit = new QLineEdit;
   shapeFileALineEdit->setReadOnly(true);
   shapeFileALineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(shapeFileAButton, SIGNAL(fileSelected(const QString&)),
                    shapeFileALineEdit, SLOT(setText(const QString&)));
   QObject::connect(shapeFileAButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotMetricShapeFileASelected(const QString&)));

   //
   // shape file B button and line edit
   //
   GuiFileSelectionButton* shapeFileBButton = new GuiFileSelectionButton(0,
                                                                    "Metric/Shape File B",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    true);
   shapeFileBLineEdit = new QLineEdit;
   shapeFileBLineEdit->setReadOnly(true);
   shapeFileBLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(shapeFileBButton, SIGNAL(fileSelected(const QString&)),
                    shapeFileBLineEdit, SLOT(setText(const QString&)));

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
   negThreshDoubleSpinBox->setMaximum(0.0);
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
   QLabel* shuffleTMapIterationsLabel = new QLabel("Shuffled T-Map Iterations");
   shuffledTMapIterationSpinBox = new QSpinBox;
   shuffledTMapIterationSpinBox->setMinimum(0);
   shuffledTMapIterationSpinBox->setMaximum(10000000);
   shuffledTMapIterationSpinBox->setSingleStep(10);
   shuffledTMapIterationSpinBox->setFixedWidth(numericMaxWidth);
   shuffledTMapIterationSpinBox->setValue(500);
   
   //
   // P-Value
   //
   QLabel* pValueLabel = new QLabel("P-Value (Type I Error)");
   pValueDoubleSpinBox = new QDoubleSpinBox;
   pValueDoubleSpinBox->setMinimum(0.0);
   pValueDoubleSpinBox->setMaximum(1.0);
   pValueDoubleSpinBox->setSingleStep(0.05);
   pValueDoubleSpinBox->setDecimals(6);
   pValueDoubleSpinBox->setFixedWidth(numericMaxWidth);
   pValueDoubleSpinBox->setValue(0.05);
   
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
   grid->addWidget(shapeFileAButton, 0, 0);
   grid->addWidget(shapeFileALineEdit, 0, 1);
   grid->addWidget(shapeFileBButton, 1, 0);
   grid->addWidget(shapeFileBLineEdit, 1, 1);
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
   grid->addWidget(shuffleTMapIterationsLabel, 8, 0);
   grid->addWidget(shuffledTMapIterationSpinBox, 8, 1, Qt::AlignLeft);
   grid->addWidget(pValueLabel, 9, 0);
   grid->addWidget(pValueDoubleSpinBox, 9, 1, Qt::AlignLeft);
   grid->addWidget(threadsLabel, 10, 0);
   grid->addWidget(threadsSpinBox, 10, 1, Qt::AlignLeft);
   
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
 * create the options widget.
 */
QWidget* 
GuiMetricShapeTwoSampleTTestDialog::createOptionsWidget()
{   
   //
   // Degrees of freedom checkbox
   //
   tMapDOFCheckBox = new QCheckBox("Compute Degrees of Freedom");
   tMapDOFCheckBox->setChecked(true);
   
   //
   // P-Value checkbox
   //
   tMapPValueCheckBox = new QCheckBox("Compute P-Value");
   tMapPValueCheckBox->setChecked(true);
   
   //
   // arrange the widgets
   //
   QWidget* w = new QWidget;
   QVBoxLayout* bl = new QVBoxLayout(w);
   bl->setSpacing(3);
   bl->addWidget(tMapDOFCheckBox);
   bl->addWidget(tMapPValueCheckBox);
   bl->addStretch();
   
   return w;
}
   
/**
 * called when shape/metric file A is selected.
 */
void 
GuiMetricShapeTwoSampleTTestDialog::slotMetricShapeFileASelected(const QString& name)
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
      
      tMapShapeFileLineEdit->setText("TMap" + extension);
      shuffledTMapShapeFileLineEdit->setText("Shuffled_TMap" + extension);
      
   }
}
      
/**
 * create the output widget.
 */
QWidget* 
GuiMetricShapeTwoSampleTTestDialog::createOutputWidget()
{
   //
   // t-map shape file B button and line edit
   //
   GuiFileSelectionButton* tMapShapeFileButton = new GuiFileSelectionButton(0,
                                                                    "T-Map Metric/Shape File",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    false);
   tMapShapeFileLineEdit = new QLineEdit;
   tMapShapeFileLineEdit->setReadOnly(false);
   tMapShapeFileLineEdit->setMinimumWidth(lineEditWidth);
   tMapShapeFileLineEdit->setText("TMap.surface_shape");
   QObject::connect(tMapShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    tMapShapeFileLineEdit, SLOT(setText(const QString&)));

   //
   // shuffled T-map file  button and line edit
   //
   GuiFileSelectionButton* shuffledTMapShapeFileButton = new GuiFileSelectionButton(0,
                                                                    "Shuffled T-Map Metric/Shape File",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    false);
   shuffledTMapShapeFileLineEdit = new QLineEdit;
   shuffledTMapShapeFileLineEdit->setReadOnly(false);
   shuffledTMapShapeFileLineEdit->setMinimumWidth(lineEditWidth);
   shuffledTMapShapeFileLineEdit->setText("Shuffled_TMap.surface_shape");
   QObject::connect(shuffledTMapShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    shuffledTMapShapeFileLineEdit, SLOT(setText(const QString&)));

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
   switch (dataTransformMode) {
      case BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_NONE:
         reportFileLineEdit->setText("Two_Sample_Significant_Clusters.txt");
         break;
      case BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_WILCOXON_RANK_SUM_THEN_TWO_SAMPLE_T_TEST:
         reportFileLineEdit->setText("Wilcoxon_Significant_Clusters.txt");
         break;
   }
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
   grid->addWidget(tMapShapeFileButton, 0, 0);
   grid->addWidget(tMapShapeFileLineEdit, 0, 1);
   grid->addWidget(shuffledTMapShapeFileButton, 1, 0);
   grid->addWidget(shuffledTMapShapeFileLineEdit, 1, 1);
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
GuiMetricShapeTwoSampleTTestDialog::createReportWidget()
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
GuiMetricShapeTwoSampleTTestDialog::createVarianceSmoothingWidget()
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
GuiMetricShapeTwoSampleTTestDialog::slotAreaCorrectionShapeFile(const QString& name)
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
GuiMetricShapeTwoSampleTTestDialog::updateDialog()
{
}
      
/**
 * called when user presses apply button
 */
void 
GuiMetricShapeTwoSampleTTestDialog::slotApplyButton()
{
   //
   // overwrite files check
   //
   const QString tMapFileName(tMapShapeFileLineEdit->text());
   if (QFile::exists(tMapFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(tMapFileName).toAscii().constData()
          << " ?";
      if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
         return;
      }
   }
   const QString shuffledTMapFileName(shuffledTMapShapeFileLineEdit->text());
   if (QFile::exists(shuffledTMapFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(shuffledTMapFileName).toAscii().constData()
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
   
   int varianceSmoothingIterations = 0;
   float varianceSmoothingStrength = 0.0;
   if (varianceSmoothingGroupBox->isEnabled() &&
       varianceSmoothingGroupBox->isChecked()) {
      varianceSmoothingIterations = varianceSmoothingIterationsSpinBox->value();
      varianceSmoothingStrength = varianceSmoothingStrengthSpinBox->value();
   }

   try {
      BrainModelSurfaceMetricTwoSampleTTest 
            ssc(theMainWindow->getBrainSet(),
                dataTransformMode,
                BrainModelSurfaceMetricTwoSampleTTest::VARIANCE_MODE_UNPOOLED,
                shapeFileALineEdit->text(),
                shapeFileBLineEdit->text(),
                fiducialCoordFileLineEdit->text(),
                openTopoFileLineEdit->text(),
                areaCorrectionShapeFileLineEdit->text(),
                tMapFileName,
                shuffledTMapFileName,
                paintFileName,
                "",  // metric clusters
                reportFileName,
                areaCorrectionShapeColumnComboBox->currentIndex(),
                shuffledTMapIterationSpinBox->value(),
                negThreshDoubleSpinBox->value(),
                posThreshDoubleSpinBox->value(),
                pValueDoubleSpinBox->value(),
                varianceSmoothingIterations,
                varianceSmoothingStrength,
                tMapDOFCheckBox->isChecked(),
                tMapPValueCheckBox->isChecked(),
                threadsSpinBox->value());
         
      /*
      GuiBrainModelAlgorithmProgressDialog algProgDialog(&ssc,
                                                         "Surface Shape Significant Clusters",
                                                         theMainWindow,
                                                         "shape-sig-clust");
      algProgDialog.runAlgorithm();
      */
      ssc.execute();
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

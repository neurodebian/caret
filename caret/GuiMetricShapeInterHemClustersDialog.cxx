
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

#include "BrainModelSurfaceMetricInterHemClusters.h"
#include "FileUtilities.h"
#include "GuiDataFileDialog.h"
#include "GuiFileSelectionButton.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiMetricShapeInterHemClustersDialog.h"
#include <QDoubleSpinBox>
#include "SurfaceShapeFile.h"
#include "global_variables.h"

/**
 * constructor
 */
GuiMetricShapeInterHemClustersDialog::GuiMetricShapeInterHemClustersDialog(QWidget* parent)
   : QtDialogNonModal(parent)
{
   setWindowTitle("Find Interhemispheric Clusters");
   
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
GuiMetricShapeInterHemClustersDialog::~GuiMetricShapeInterHemClustersDialog()
{
}

/**
 * show the help page.
 */
void 
GuiMetricShapeInterHemClustersDialog::slotShowHelpPage()
{
   theMainWindow->showHelpViewerDialog("statistics/interhemispheric_clusters.html");
}
      
/**
 * create the input widget.
 */
QWidget* 
GuiMetricShapeInterHemClustersDialog::createInputWidget()
{
   //
   // Left shape file A button and line edit
   //
   GuiFileSelectionButton* shapeFileLeftAButton = new GuiFileSelectionButton(0,
                                                                    "Left Metric/Shape File A",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    true);
   shapeFileLeftALineEdit = new QLineEdit;
   shapeFileLeftALineEdit->setReadOnly(true);
   shapeFileLeftALineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(shapeFileLeftAButton, SIGNAL(fileSelected(const QString&)),
                    shapeFileLeftALineEdit, SLOT(setText(const QString&)));
   QObject::connect(shapeFileLeftAButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotMetricShapeFileASelected(const QString&)));

   //
   // Left shape file B button and line edit
   //
   GuiFileSelectionButton* shapeFileLeftBButton = new GuiFileSelectionButton(0,
                                                                    "Left Metric/Shape File B",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    true);
   shapeFileLeftBLineEdit = new QLineEdit;
   shapeFileLeftBLineEdit->setReadOnly(true);
   shapeFileLeftBLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(shapeFileLeftBButton, SIGNAL(fileSelected(const QString&)),
                    shapeFileLeftBLineEdit, SLOT(setText(const QString&)));

   //
   // Right shape file A button and line edit
   //
   GuiFileSelectionButton* shapeFileRightAButton = new GuiFileSelectionButton(0,
                                                                    "Right Metric/Shape File A",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    true);
   shapeFileRightALineEdit = new QLineEdit;
   shapeFileRightALineEdit->setReadOnly(true);
   shapeFileRightALineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(shapeFileRightAButton, SIGNAL(fileSelected(const QString&)),
                    shapeFileRightALineEdit, SLOT(setText(const QString&)));

   //
   // Right shape file B button and line edit
   //
   GuiFileSelectionButton* shapeFileRightBButton = new GuiFileSelectionButton(0,
                                                                    "Right Metric/Shape File B",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    true);
   shapeFileRightBLineEdit = new QLineEdit;
   shapeFileRightBLineEdit->setReadOnly(true);
   shapeFileRightBLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(shapeFileRightBButton, SIGNAL(fileSelected(const QString&)),
                    shapeFileRightBLineEdit, SLOT(setText(const QString&)));

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
   negThreshDoubleSpinBox->setMinimum(-5000000.0);
   negThreshDoubleSpinBox->setMaximum(0.0);
   negThreshDoubleSpinBox->setSingleStep(1.0);
   negThreshDoubleSpinBox->setDecimals(3);
   negThreshDoubleSpinBox->setFixedWidth(numericMaxWidth);
   negThreshDoubleSpinBox->setValue(-6.0);
   
   //
   // Cluster positive threshold
   //
   QLabel* posThreshLabel = new QLabel("Cluster Positive Threshold");
   posThreshDoubleSpinBox = new QDoubleSpinBox;
   posThreshDoubleSpinBox->setMinimum(0.0);
   posThreshDoubleSpinBox->setMaximum(5000000.0);
   posThreshDoubleSpinBox->setSingleStep(1.0);
   posThreshDoubleSpinBox->setDecimals(3);
   posThreshDoubleSpinBox->setFixedWidth(numericMaxWidth);
   posThreshDoubleSpinBox->setValue(6.0);
   
   //
   // Shuffled T-Map Iterations
   //
   QLabel* leftRightShuffleTMapIterationsLabel = new QLabel("Left/Right Shuffled T-Map Iterations");
   leftRightShuffledTMapIterationSpinBox = new QSpinBox;
   leftRightShuffledTMapIterationSpinBox->setMinimum(0);
   leftRightShuffledTMapIterationSpinBox->setMaximum(10000000);
   leftRightShuffledTMapIterationSpinBox->setSingleStep(10);
   leftRightShuffledTMapIterationSpinBox->setFixedWidth(numericMaxWidth);
   leftRightShuffledTMapIterationSpinBox->setValue(200);
   
   //
   // Shuffled T-Map Iterations
   //
   QLabel* shuffleTMapIterationsLabel = new QLabel("Shuffled T-Map Iterations");
   shuffledTMapIterationSpinBox = new QSpinBox;
   shuffledTMapIterationSpinBox->setMinimum(0);
   shuffledTMapIterationSpinBox->setMaximum(10000000);
   shuffledTMapIterationSpinBox->setSingleStep(10);
   shuffledTMapIterationSpinBox->setFixedWidth(numericMaxWidth);
   shuffledTMapIterationSpinBox->setValue(2500);
   
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
   int rowNum = 0;
   grid->addWidget(shapeFileLeftAButton, rowNum, 0);
   grid->addWidget(shapeFileLeftALineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(shapeFileLeftBButton, rowNum, 0);
   grid->addWidget(shapeFileLeftBLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(shapeFileRightAButton, rowNum, 0);
   grid->addWidget(shapeFileRightALineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(shapeFileRightBButton, rowNum, 0);
   grid->addWidget(shapeFileRightBLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(fiducialCoordFileButton, rowNum, 0);
   grid->addWidget(fiducialCoordFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(openTopoFileButton, rowNum, 0);
   grid->addWidget(openTopoFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(areaCorrectionShapeFileButton, rowNum, 0);
   grid->addWidget(areaCorrectionShapeFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(shapeCorrectionColumnLabel, rowNum, 0);
   grid->addWidget(areaCorrectionShapeColumnComboBox, rowNum, 1);
   rowNum++;
   grid->addWidget(negThreshLabel, rowNum, 0);
   grid->addWidget(negThreshDoubleSpinBox, rowNum, 1, Qt::AlignLeft);
   rowNum++;
   grid->addWidget(posThreshLabel, rowNum, 0);
   grid->addWidget(posThreshDoubleSpinBox, rowNum, 1, Qt::AlignLeft);
   rowNum++;
   grid->addWidget(leftRightShuffleTMapIterationsLabel, rowNum, 0);
   grid->addWidget(leftRightShuffledTMapIterationSpinBox, rowNum, 1, Qt::AlignLeft);
   rowNum++;
   grid->addWidget(shuffleTMapIterationsLabel, rowNum, 0);
   grid->addWidget(shuffledTMapIterationSpinBox, rowNum, 1, Qt::AlignLeft);
   rowNum++;
   grid->addWidget(pValueLabel, rowNum, 0);
   grid->addWidget(pValueDoubleSpinBox, rowNum, 1, Qt::AlignLeft);
   rowNum++;
   grid->addWidget(threadsLabel, rowNum, 0);
   grid->addWidget(threadsSpinBox, rowNum, 1, Qt::AlignLeft);
   rowNum++;
   
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
 * called when shape/metric file A is selected.
 */
void 
GuiMetricShapeInterHemClustersDialog::slotMetricShapeFileASelected(const QString& name)
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
      
      leftTMapShapeFileLineEdit->setText("Left_TMap" + extension);
      leftShuffledTMapShapeFileLineEdit->setText("Left_Shuffled_TMap" + extension);
      rightTMapShapeFileLineEdit->setText("Right_TMap" + extension);
      rightShuffledTMapShapeFileLineEdit->setText("Right_Shuffled_TMap" + extension);
      tMapShapeFileLineEdit->setText("TMap" + extension);
      shuffledTMapShapeFileLineEdit->setText("Shuffled_TMap" + extension);
   }
}
      
/**
 * create the options widget.
 */
QWidget* 
GuiMetricShapeInterHemClustersDialog::createOptionsWidget()
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
 * create the output widget.
 */
QWidget* 
GuiMetricShapeInterHemClustersDialog::createOutputWidget()
{
   
   //
   // Left t-map shape file B button and line edit
   //
   GuiFileSelectionButton* leftTMapShapeFileButton = new GuiFileSelectionButton(0,
                                                                    "Left T-Map Metric/Shape File",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    false);
   leftTMapShapeFileLineEdit = new QLineEdit;
   leftTMapShapeFileLineEdit->setReadOnly(false);
   leftTMapShapeFileLineEdit->setMinimumWidth(lineEditWidth);
   leftTMapShapeFileLineEdit->setText("Left_TMap.surface_shape");
   QObject::connect(leftTMapShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    leftTMapShapeFileLineEdit, SLOT(setText(const QString&)));

   //
   // Left shuffled T-map file  button and line edit
   //
   GuiFileSelectionButton* leftShuffledTMapShapeFileButton = new GuiFileSelectionButton(0,
                                                                    "Left Shuffled T-Map Metric/Shape File",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    false);
   leftShuffledTMapShapeFileLineEdit = new QLineEdit;
   leftShuffledTMapShapeFileLineEdit->setReadOnly(false);
   leftShuffledTMapShapeFileLineEdit->setMinimumWidth(lineEditWidth);
   leftShuffledTMapShapeFileLineEdit->setText("Left_Shuffled_TMap.surface_shape");
   QObject::connect(leftShuffledTMapShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    leftShuffledTMapShapeFileLineEdit, SLOT(setText(const QString&)));

   //
   // Right t-map shape file B button and line edit
   //
   GuiFileSelectionButton* rightTMapShapeFileButton = new GuiFileSelectionButton(0,
                                                                    "Right T-Map Metric/Shape File",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    false);
   rightTMapShapeFileLineEdit = new QLineEdit;
   rightTMapShapeFileLineEdit->setReadOnly(false);
   rightTMapShapeFileLineEdit->setMinimumWidth(lineEditWidth);
   rightTMapShapeFileLineEdit->setText("Right_TMap.surface_shape");
   QObject::connect(rightTMapShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    rightTMapShapeFileLineEdit, SLOT(setText(const QString&)));

   //
   // Right shuffled T-map file  button and line edit
   //
   GuiFileSelectionButton* rightShuffledTMapShapeFileButton = new GuiFileSelectionButton(0,
                                                                    "Right Shuffled T-Map Metric/Shape File",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    false);
   rightShuffledTMapShapeFileLineEdit = new QLineEdit;
   rightShuffledTMapShapeFileLineEdit->setReadOnly(false);
   rightShuffledTMapShapeFileLineEdit->setMinimumWidth(lineEditWidth);
   rightShuffledTMapShapeFileLineEdit->setText("Right_Shuffled_TMap.surface_shape");
   QObject::connect(rightShuffledTMapShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    rightShuffledTMapShapeFileLineEdit, SLOT(setText(const QString&)));

   //
   // t-map shape file button and line edit
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
   int rowNum = 0;
   grid->addWidget(leftTMapShapeFileButton, rowNum, 0);
   grid->addWidget(leftTMapShapeFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(leftShuffledTMapShapeFileButton, rowNum, 0);
   grid->addWidget(leftShuffledTMapShapeFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(rightTMapShapeFileButton, rowNum, 0);
   grid->addWidget(rightTMapShapeFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(rightShuffledTMapShapeFileButton, rowNum, 0);
   grid->addWidget(rightShuffledTMapShapeFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(tMapShapeFileButton, rowNum, 0);
   grid->addWidget(tMapShapeFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(shuffledTMapShapeFileButton, rowNum, 0);
   grid->addWidget(shuffledTMapShapeFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(paintFileButton, rowNum, 0);
   grid->addWidget(paintFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(reportFileButton, rowNum, 0);
   grid->addWidget(reportFileLineEdit, rowNum, 1);
   rowNum++;
   grid->addWidget(new QWidget, rowNum, 0);
   grid->setRowStretch(rowNum, 1000);
   
   return w;
}

/**
 * create the report widget.
 */
QWidget* 
GuiMetricShapeInterHemClustersDialog::createReportWidget()
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
GuiMetricShapeInterHemClustersDialog::createVarianceSmoothingWidget()
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
GuiMetricShapeInterHemClustersDialog::slotAreaCorrectionShapeFile(const QString& name)
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
GuiMetricShapeInterHemClustersDialog::updateDialog()
{
}
      
/**
 * called when user presses apply button
 */
void 
GuiMetricShapeInterHemClustersDialog::slotApplyButton()
{
/*
   //
   // overwrite files check
   //
   const QString tMapFileName(tMapShapeFileLineEdit->text());
   if (QFile::exists(tMapFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(tMapFileName)
          << " ?";
      if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
         return;
      }
   }
   const QString shuffledTMapFileName(shuffledTMapShapeFileLineEdit->text());
   if (QFile::exists(shuffledTMapFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(shuffledTMapFileName)
          << " ?";
      if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
         return;
      }
   }
   const QString reportFileName(reportFileLineEdit->text());
   if (QFile::exists(reportFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(reportFileName)
          << " ?";
      if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
         return;
      }
   }
   const QString paintFileName(paintFileLineEdit->text());
   if (paintFileName.empty() == false) {
      if (QFile::exists(paintFileName)) {
         std::ostringstream str;
         str << "Overwrite "
             << FileUtilities::basename(paintFileName)
             << " ?";
         if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
            return;
         }
      }
   }
*/   
   int varianceSmoothingIterations = 0;
   float varianceSmoothingStrength = 0.0;
   if (varianceSmoothingGroupBox->isEnabled() &&
       varianceSmoothingGroupBox->isChecked()) {
      varianceSmoothingIterations = varianceSmoothingIterationsSpinBox->value();
      varianceSmoothingStrength = varianceSmoothingStrengthSpinBox->value();
   }

   try {
      BrainModelSurfaceMetricInterHemClusters 
            ssc(theMainWindow->getBrainSet(),
                shapeFileRightALineEdit->text(),
                shapeFileRightBLineEdit->text(),
                shapeFileLeftALineEdit->text(),
                shapeFileLeftBLineEdit->text(),
                fiducialCoordFileLineEdit->text(),
                openTopoFileLineEdit->text(),
                areaCorrectionShapeFileLineEdit->text(),
                rightTMapShapeFileLineEdit->text(),
                leftTMapShapeFileLineEdit->text(),
                rightShuffledTMapShapeFileLineEdit->text(),
                leftShuffledTMapShapeFileLineEdit->text(),
                tMapShapeFileLineEdit->text(),
                shuffledTMapShapeFileLineEdit->text(),
                paintFileLineEdit->text(),
                "",  // metric clusters
                reportFileLineEdit->text(),
                areaCorrectionShapeColumnComboBox->currentIndex(),
                shuffledTMapIterationSpinBox->value(),
                leftRightShuffledTMapIterationSpinBox->value(),
                negThreshDoubleSpinBox->value(),
                posThreshDoubleSpinBox->value(),
                pValueDoubleSpinBox->value(),
                varianceSmoothingIterations,
                varianceSmoothingStrength,
                tMapDOFCheckBox->isChecked(),
                tMapPValueCheckBox->isChecked(),
                threadsSpinBox->value());
         
      ssc.execute();
   }
   catch (BrainModelAlgorithmException &e) {
      GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
      return;
   }
   
   //
   // Put the report into the report text edit
   //
   QFile reportFile(reportFileLineEdit->text());
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

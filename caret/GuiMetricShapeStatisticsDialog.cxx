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
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSpinBox>

#include "GuiDataFileDialog.h"
#include "GuiFileSelectionButton.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiMetricShapeStatisticsDialog.h"
#include "MetricFile.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "SpecFile.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiMetricShapeStatisticsDialog::GuiMetricShapeStatisticsDialog(QWidget* parent, 
                                                               const DIALOG_MODE dialogModeIn)
   : QtDialog(parent, true)
{
   dialogMode = dialogModeIn;
   
   bool enableRepetitions = false;
   bool enableMeanDeviation = false;
   bool twoInputFiles = true;
   bool twoOutputFiles = false;
   bool enableSizeOfGroup1 = false;
   QString theCaption;
   switch (dialogMode) {
      case DIALOG_MODE_LEVENE:
         theCaption.append("Compute Levene-Map on ");
         twoInputFiles = true;
         helpPage = "statistics/levene_map.html";
         break;
      case DIALOG_MODE_ZMAP:
         theCaption.append("Compute Z-Map on ");
         twoInputFiles = false;
         helpPage = "statistics/z_map.html";
         break;
      case DIALOG_MODE_TMAP:
         theCaption.append("Compute T-Map on ");
         helpPage = "statistics/tmap.html";
         break;
      case DIALOG_MODE_SUBTRACT_GROUP_AVERAGE:
         theCaption.append("Subtract Group Average on ");
         twoInputFiles = true;
         twoOutputFiles = true;
         helpPage = "statistics/subtract_group_average.html";
         break;
      case DIALOG_MODE_SHUFFLED_TMAP:
         theCaption.append("Compute T-Map on Shuffled Columns on ");
         twoInputFiles = false;
         enableRepetitions = true;
         enableSizeOfGroup1 = true;
         helpPage = "statistics/t_map_shuffled_columns.html";
         break;
      case DIALOG_MODE_NORMALIZE:
         theCaption.append("Normalize ");
         twoInputFiles = false;
         enableMeanDeviation = true;
         helpPage = "statistics/normalization.html";
         break;
      case DIALOG_MODE_SHUFF_CROSS_CORRELATION:
         theCaption.append("Shuffle Cross-Correlation Maps on ");
         twoInputFiles = false;
         enableRepetitions = true;
         helpPage = "statistics/shuffled_cross_correlation_maps.html";
         break;
      case DIALOG_MODE_ROOT_MEAN_SQUARE:
         theCaption.append("Compute Root Mean Square on ");
         twoInputFiles = false;
   }
   theCaption.append("Metric and Surface Shape Data");
   setWindowTitle(theCaption);
   
   QString description;
   switch (dialogMode) {
      case DIALOG_MODE_LEVENE:
         description = "A Levene-Map is computed for each node.";
         break;
      case DIALOG_MODE_ZMAP:
         description = "For each node, all values are converted to a\n"
                            "Z-Score (value - mean)/deviation.";
         break;
      case DIALOG_MODE_TMAP:
         description = "A T-Map between the two files is computed for\n"
                            "each node.";
         break;
      case DIALOG_MODE_SUBTRACT_GROUP_AVERAGE:
         description = "For each node in both files, an average values is\n"
                            "determined and that average is subtracted from all\n"
                            "values for that node in both files.";
         break;
      case DIALOG_MODE_SHUFFLED_TMAP:
         description = "The columns in the metric file are randomly shuffled\n"
                            "and then split into to groups.  A T-Map is then computed\n"
                            "between the two groups.  This process is repeated for\n"
                            "the specified number of iterations.  Use the Size of Group\n"
                            "A control to set the size of the first group.  If the size\n"
                            "of the first group is set to zero or a negative number,\n"
                            "two groups of equal size will be created.";
         break;
      case DIALOG_MODE_NORMALIZE:
         description = "Remap each column of values in the file so that\n"
                            "it matches a normal distribution.  The median value\n"
                            "is mapped to the mean.";
         break;
      case DIALOG_MODE_SHUFF_CROSS_CORRELATION:
         description = "For the specified number of repetitions, randomly select\n"
                            "two data columns and multiply them to produce the new\n"
                            "data file.";
         break;
      case DIALOG_MODE_ROOT_MEAN_SQUARE:
         description = "For each node, the mean, standard deviation, and root mean\n"
                       "square are computed for the node's values.";
         break;
   }
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(5);

   //
   // Grid for items
   //
   QGridLayout* gridLayout = new QGridLayout;
   dialogLayout->addLayout(gridLayout);
   
   //
   // Repetitions controls
   //
   repetitionsSpinBox = NULL;
   if (enableRepetitions) {
      const int row = gridLayout->rowCount();
      gridLayout->addWidget(new QLabel("Repetitions "), row, 0);
      repetitionsSpinBox = new QSpinBox;
      repetitionsSpinBox->setMinimum(1);
      repetitionsSpinBox->setMaximum(100000);
      repetitionsSpinBox->setSingleStep(1);
      repetitionsSpinBox->setValue(40);
      gridLayout->addWidget(repetitionsSpinBox, row, 1);
   }
   
   //
   // Size of Group 1
   //
   sizeOfGroup1SpinBox = NULL;
   if (enableSizeOfGroup1) {
      const int row = gridLayout->rowCount();
      gridLayout->addWidget(new QLabel("Size of Group A"), row, 0);
      sizeOfGroup1SpinBox = new QSpinBox;
      sizeOfGroup1SpinBox->setMinimum(std::numeric_limits<int>::min());
      sizeOfGroup1SpinBox->setMaximum(std::numeric_limits<int>::max());
      sizeOfGroup1SpinBox->setSingleStep(1);
      sizeOfGroup1SpinBox->setValue(0);
      gridLayout->addWidget(sizeOfGroup1SpinBox);
   }
   
   meanDoubleSpinBox = NULL;
   deviationDoubleSpinBox = NULL;
   if (enableMeanDeviation) {
      int row = gridLayout->rowCount();
      gridLayout->addWidget(new QLabel("Mean "), row, 0);
      meanDoubleSpinBox = new QDoubleSpinBox;
      meanDoubleSpinBox->setMinimum(0.0);
      meanDoubleSpinBox->setMaximum(10000.0);
      meanDoubleSpinBox->setSingleStep(1.0);
      meanDoubleSpinBox->setDecimals(3);
      meanDoubleSpinBox->setValue(0.0);
      gridLayout->addWidget(meanDoubleSpinBox, row, 1);

      row = gridLayout->rowCount();
      gridLayout->addWidget(new QLabel("Std Deviation "), row, 0);
      deviationDoubleSpinBox = new QDoubleSpinBox;
      deviationDoubleSpinBox->setMinimum(0.0);
      deviationDoubleSpinBox->setMaximum(10000.0);
      deviationDoubleSpinBox->setSingleStep(1.0);
      deviationDoubleSpinBox->setDecimals(3);
      deviationDoubleSpinBox->setValue(1.0);
      gridLayout->addWidget(deviationDoubleSpinBox, row, 1);
   }
   
   const int minLineEditWidth = 400;
   
   //
   // Input file 1
   //
   int row = gridLayout->rowCount();
   QLabel* inputLabel1 = new QLabel("Input File ");
   gridLayout->addWidget(inputLabel1, row, 0);
   inputName1LineEdit = new QLineEdit;
   gridLayout->addWidget(inputName1LineEdit, row, 1);
   inputName1LineEdit->setMinimumWidth(minLineEditWidth);
   QPushButton* inputName1PushButton = new QPushButton("Input File...");
   gridLayout->addWidget(inputName1PushButton, row, 2);
   inputName1PushButton->setAutoDefault(false);
   QObject::connect(inputName1PushButton, SIGNAL(clicked()),
                    this, SLOT(slotInputFile1PushButton()));
                    
   //
   // Input file 2
   //
   inputName2LineEdit = NULL;
   if (twoInputFiles) {
      row = gridLayout->rowCount();
      inputLabel1->setText("Input File A");
      gridLayout->addWidget(new QLabel("Input File B"), row, 0);
      inputName2LineEdit = new QLineEdit;
      inputName2LineEdit->setMinimumWidth(minLineEditWidth);
      gridLayout->addWidget(inputName2LineEdit, row, 1);
      QPushButton* inputName2PushButton = new QPushButton("Input File...");
      gridLayout->addWidget(inputName2PushButton, row, 2);
      inputName2PushButton->setAutoDefault(false);
      QObject::connect(inputName2PushButton, SIGNAL(clicked()),
                       this, SLOT(slotInputFile2PushButton()));
   }
                    
   //
   // Output file A
   //
   row = gridLayout->rowCount();
   QLabel* outputLabel1 = new QLabel("Output File ");
   gridLayout->addWidget(outputLabel1, row, 0);
   outputName1LineEdit = new QLineEdit;
   gridLayout->addWidget(outputName1LineEdit, row, 1);
   outputName1LineEdit->setMinimumWidth(minLineEditWidth);
   QPushButton* outputName1PushButton = new QPushButton("Output File...");
   gridLayout->addWidget(outputName1PushButton, row, 2);
   outputName1PushButton->setAutoDefault(false);
   QObject::connect(outputName1PushButton, SIGNAL(clicked()),
                    this, SLOT(slotOutputFile1PushButton()));
                    
   //
   // Output file B
   //
   outputName2LineEdit = NULL;
   if (twoOutputFiles) {
      row = gridLayout->rowCount();
      outputLabel1->setText("Output File A ");
      gridLayout->addWidget(new QLabel("Output File B "), row, 0);
      outputName2LineEdit = new QLineEdit;
      gridLayout->addWidget(outputName2LineEdit, row, 1);
      outputName2LineEdit->setMinimumWidth(minLineEditWidth);
      QPushButton* outputName2PushButton = new QPushButton("Output File...");
      gridLayout->addWidget(outputName2PushButton, row, 2);
      outputName2PushButton->setAutoDefault(false);
      QObject::connect(outputName2PushButton, SIGNAL(clicked()),
                       this, SLOT(slotOutputFile2PushButton()));
   }
     
   //
   // Area there options ?
   //
   bool haveVarianceSmoothing = false;
   bool haveFalseDiscoveryRate = false;
   switch (dialogMode) {
      case DIALOG_MODE_LEVENE:
         break;
      case DIALOG_MODE_ZMAP:
         break;
      case DIALOG_MODE_TMAP:
         {
            //
            // Enable false discovery rate
            //
            haveFalseDiscoveryRate = true;
            
            //
            // Enable variance smoothing
            //
            haveVarianceSmoothing = true;
            
            //
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
            // Group box and arrange the widgets
            //
            QGroupBox* gb = new QGroupBox("Options");
            QVBoxLayout* gbLayout = new QVBoxLayout(gb);
            gbLayout->addWidget(tMapDOFCheckBox);
            gbLayout->addWidget(tMapPValueCheckBox);
            dialogLayout->addWidget(gb);
         }
         break;
      case DIALOG_MODE_SUBTRACT_GROUP_AVERAGE:
         break;
      case DIALOG_MODE_SHUFFLED_TMAP:
         //
         // Enable variance smoothing
         //
         haveVarianceSmoothing = true;
         break;
      case DIALOG_MODE_NORMALIZE:
         break;
      case DIALOG_MODE_SHUFF_CROSS_CORRELATION:
         break;
      case DIALOG_MODE_ROOT_MEAN_SQUARE:
         break;
   }
   
   //
   // False discovery rate
   //
   if (haveFalseDiscoveryRate) {
      QLabel* falseDiscoveryRateQValueLabel = new QLabel("Q "); 
      falseDiscoveryRateQValueDoubleSpinBox = new QDoubleSpinBox;
      falseDiscoveryRateQValueDoubleSpinBox->setMinimum(0.0);
      falseDiscoveryRateQValueDoubleSpinBox->setMaximum(1.0);
      falseDiscoveryRateQValueDoubleSpinBox->setSingleStep(0.01);
      falseDiscoveryRateQValueDoubleSpinBox->setDecimals(3);
      falseDiscoveryRateQValueDoubleSpinBox->setValue(0.05);
      falseDiscoveryRateGroupBox = new QGroupBox("False Discovery Rate");
      falseDiscoveryRateGroupBox->setCheckable(true);
      falseDiscoveryRateGroupBox->setChecked(true);
      QHBoxLayout* falseDiscoveryLayout = new QHBoxLayout(falseDiscoveryRateGroupBox);
      falseDiscoveryLayout->addWidget(falseDiscoveryRateQValueLabel);
      falseDiscoveryLayout->addWidget(falseDiscoveryRateQValueDoubleSpinBox);
      falseDiscoveryLayout->addStretch();
      dialogLayout->addWidget(falseDiscoveryRateGroupBox);
   }
   
   //
   // Variance smoothing topology file
   //
   GuiFileSelectionButton* varianceSmoothingTopologyFileButton = new GuiFileSelectionButton(0,
                                                                      "Topology File",
                                                                      GuiDataFileDialog::topologyGenericFileFilter,
                                                                      true);
   varianceSmoothingTopologyFileLineEdit = new QLineEdit;
   varianceSmoothingTopologyFileLineEdit->setReadOnly(true);
   QObject::connect(varianceSmoothingTopologyFileButton, SIGNAL(fileSelected(const QString&)),
                    varianceSmoothingTopologyFileLineEdit, SLOT(setText(const QString&)));
   
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
   varianceSmoothingLayout->addWidget(varianceSmoothingTopologyFileButton, 0, 0);
   varianceSmoothingLayout->addWidget(varianceSmoothingTopologyFileLineEdit, 0, 1);
   varianceSmoothingLayout->addWidget(varianceSmoothingIterationsLabel, 1, 0);
   varianceSmoothingLayout->addWidget(varianceSmoothingIterationsSpinBox, 1, 1);
   varianceSmoothingLayout->addWidget(varianceSmoothingStrengthLabel, 2, 0);
   varianceSmoothingLayout->addWidget(varianceSmoothingStrengthSpinBox, 2, 1);
   if (haveVarianceSmoothing == false) {
      varianceSmoothingGroupBox->setHidden(true);
   }
   varianceSmoothingGroupBox->setChecked(false);
   dialogLayout->addWidget(varianceSmoothingGroupBox);
   
   //
   // Add description
   //
   QGroupBox* descripGroupBox = new QGroupBox("Description of Operation");
   QVBoxLayout* descripLayout = new QVBoxLayout(descripGroupBox);
   dialogLayout->addWidget(descripGroupBox);
   descripLayout->addWidget(new QLabel(description));
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);

   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));   

   //
   // Cancel button connects to QDialogs close() slot.   
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   cancelButton->setAutoDefault(false);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
  
   //
   // help button.   
   //
   QPushButton* helpButton = new QPushButton("Help");
   buttonsLayout->addWidget(helpButton);
   helpButton->setAutoDefault(false);
   QObject::connect(helpButton, SIGNAL(clicked()),
                    this, SLOT(slotHelpButton()));
   
   QtUtilities::makeButtonsSameSize(okButton, cancelButton, helpButton);
}

/**
 * destructor.
 */
GuiMetricShapeStatisticsDialog::~GuiMetricShapeStatisticsDialog()
{
}

/**
 * called when help button pressed.
 */
void 
GuiMetricShapeStatisticsDialog::slotHelpButton()
{
   theMainWindow->showHelpPageOverModalDialog(this, helpPage);
}
      
/**
 * called when output name 1 push button pressed.
 */
void 
GuiMetricShapeStatisticsDialog::slotOutputFile1PushButton()
{
   const QString fileName = getFile("Output File", false);
   if (fileName.isEmpty() == false) {
      outputName1LineEdit->setText(fileName);
   }
}

/**
 * called when output name 2 push button pressed.
 */
void 
GuiMetricShapeStatisticsDialog::slotOutputFile2PushButton()
{
   const QString fileName = getFile("Output File", false);
   if (fileName.isEmpty() == false) {
      outputName2LineEdit->setText(fileName);
   }
}

/**
 * get a file using a file selection dialog.
 */
QString 
GuiMetricShapeStatisticsDialog::getFile(const QString& caption,
                                        const bool fileMustExist)
{      
   QFileDialog fd(this);
   fd.setModal(true);
   fd.setWindowTitle(caption);
   fd.setDirectory(QDir::currentPath());
   if (fileMustExist) {
      fd.setFileMode(QFileDialog::ExistingFile);
   }
   else {
      fd.setFileMode(QFileDialog::AnyFile);
   }
   fd.setAcceptMode(QFileDialog::AcceptOpen);
   
   const QString theFileFilter(GuiDataFileDialog::metricShapeFileFilter);
   fd.setFilters(QStringList(theFileFilter));
   fd.selectFilter(theFileFilter);
   if (fd.exec() == QDialog::Accepted) {
      QStringList files = fd.selectedFiles();
      if (files.count() > 0) {
         QString name(files.at(0));
         if (name.isEmpty() == false) {
            return name;
         }
      }
   }
   
   return "";
}
      
/**
 * called when input name 1 push button pressed.
 */
void 
GuiMetricShapeStatisticsDialog::slotInputFile1PushButton()
{
   const QString fileName = getFile("Input File", true);
   if (fileName.isEmpty() == false) {
      inputName1LineEdit->setText(fileName);
   }
}

/**
 * called when input name 2 push button pressed.
 */
void 
GuiMetricShapeStatisticsDialog::slotInputFile2PushButton()
{
   const QString fileName = getFile("Input File", true);
   if (fileName.isEmpty() == false) {
      inputName2LineEdit->setText(fileName);
   }
}
      
/**
 * called when OK or Cancel button pressed.
 */
void 
GuiMetricShapeStatisticsDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      const QString inputFile1Name = inputName1LineEdit->text();
      if (inputFile1Name.isEmpty()) {
         GuiMessageBox::critical(this, "ERROR", "Input file name is missing.", "OK");
         return;
      }
      QString inputFile2Name;
      if (inputName2LineEdit != NULL) {
         inputFile2Name = inputName2LineEdit->text();
         if (inputFile2Name.isEmpty()) {
            GuiMessageBox::critical(this, "ERROR", "Input file name is missing.", "OK");
            return;
         }
      }
      const QString outputFile1Name = outputName1LineEdit->text();
      if (outputFile1Name.isEmpty()) {
         GuiMessageBox::critical(this, "ERROR", "Output file name is missing.", "OK");
         return;
      }
      
      QString outputFile2Name;
      if (outputName2LineEdit != NULL) {
         outputFile2Name = outputName2LineEdit->text();
         if (outputName2LineEdit != NULL) {
            if (outputFile2Name.isEmpty()) {
               GuiMessageBox::critical(this, "ERROR", "Output file name is missing.", "OK");
               return;
            }
         }
      }
      
      MetricFile mf1;
      try {
         mf1.readFile(inputFile1Name);
      }
      catch (FileException& e) {
         QString msg("Unable to read file ");
         msg.append(inputFile1Name);
         msg.append("\n");
         msg.append(e.whatQString());
         GuiMessageBox::critical(this, "ERROR", msg, "OK");
         return;
      }
      
      MetricFile mf2;
      if (inputName2LineEdit != NULL) {
         try {
            mf2.readFile(inputFile2Name);
         }
         catch (FileException& e) {
            QString msg("Unable to read file ");
            msg.append(inputFile2Name);
            msg.append("\n");
            msg.append(e.whatQString());
            GuiMessageBox::critical(this, "ERROR", msg, "OK");
            return;
         }
      }
      
      //
      // Process variance smoothing
      //
      TopologyFile topologyFile;
      int varianceSmoothingIterations = 0;
      float varianceSmoothingStrength = 0.0;
      if (varianceSmoothingGroupBox->isEnabled() &&
          varianceSmoothingGroupBox->isChecked()) {
         const QString& name = varianceSmoothingTopologyFileLineEdit->text();
         if (name.isEmpty()) {
            GuiMessageBox::critical(this, "ERROR", "A topology file is needed for variance smoothing.", "OK");
            return;
         }
         try {
            topologyFile.readFile(name);
         }
         catch (FileException& e) {
            GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
            return;
         }
         
         varianceSmoothingIterations = varianceSmoothingIterationsSpinBox->value();
         varianceSmoothingStrength = varianceSmoothingStrengthSpinBox->value();
      }
      
      try {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         
         MetricFile* outputMetricFile1 = NULL;
         MetricFile* outputMetricFile2 = NULL;
      
         switch (dialogMode) {
            case DIALOG_MODE_LEVENE:
               {
                  std::vector<MetricFile*> files;
                  files.push_back(&mf1);
                  files.push_back(&mf2);
                  outputMetricFile1 = mf1.computeStatisticalLeveneMap(files);
               }
               break;
            case DIALOG_MODE_ZMAP:
               outputMetricFile1 = mf1.computeStatisticalZMap();
               break;
            case DIALOG_MODE_TMAP:
               outputMetricFile1 = MetricFile::computeStatisticalTMap(&mf1, 
                                                                      &mf2,
                                                                      &topologyFile,
                                                                      varianceSmoothingIterations,
                                                                      varianceSmoothingStrength,
                                                                      false,
                                                                      falseDiscoveryRateQValueDoubleSpinBox->value(),
                                                                      falseDiscoveryRateGroupBox->isChecked(),
                                                                      tMapDOFCheckBox,
                                                                      tMapPValueCheckBox);
               break;
            case DIALOG_MODE_SUBTRACT_GROUP_AVERAGE:
               outputMetricFile1 = new MetricFile;
               outputMetricFile2 = new MetricFile;
               MetricFile::subtractMeanFromRowElements(&mf1,
                                                       &mf2,
                                                       outputMetricFile1,
                                                       outputMetricFile2);
               break;
            case DIALOG_MODE_SHUFFLED_TMAP:
               outputMetricFile1 = mf1.computeStatisticalShuffledTMap(repetitionsSpinBox->value(),
                                                                      sizeOfGroup1SpinBox->value(),
                                                                      &topologyFile,
                                                                      varianceSmoothingIterations,
                                                                      varianceSmoothingStrength,
                                                                      false);
               break;
            case DIALOG_MODE_NORMALIZE:
               outputMetricFile1 = mf1.computeNormalization(meanDoubleSpinBox->value(),
                                                           deviationDoubleSpinBox->value());
               break;
            case DIALOG_MODE_SHUFF_CROSS_CORRELATION:
               outputMetricFile1 = mf1.computeShuffledCrossCorrelationsMap(repetitionsSpinBox->value());
               break;
            case DIALOG_MODE_ROOT_MEAN_SQUARE:
               outputMetricFile1 = mf1.descriptiveStatistics(false,
                                                             "Mean",
                                                             "",  // variance
                                                             "",  // sample variance
                                                             "",  // std dev
                                                             "StdDev", // sample std dev
                                                             "",  // std error
                                                             "RMS", // root-mean square
                                                             "",  // minimum
                                                             "",  // maximum
                                                             "",  // median
                                                             "",  // skewness
                                                             ""); // kurtosis
               break;
         }
         
         if (outputMetricFile1 != NULL) {
            try { 
               outputMetricFile1->writeFile(outputFile1Name);
               delete outputMetricFile1;
               outputMetricFile1 = NULL;
            }
            catch (FileException& e) {
               GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
               return;
            }
         }
         else {
            GuiMessageBox::critical(this, "ERROR", "Failed to create output metric file.", "OK");
            return;
         }
         
         if (outputName2LineEdit != NULL) {
            if (outputMetricFile2 != NULL) {
               try { 
                  outputMetricFile2->writeFile(outputFile2Name);
                  delete outputMetricFile2;
                  outputMetricFile2 = NULL;
               }
               catch (FileException& e) {
                  GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
                  return;
               }
            }
            else {
               GuiMessageBox::critical(this, "ERROR", "Failed to create output metric file.", "OK");
               return;
            }
         }

         QApplication::restoreOverrideCursor();         
      }
      catch (FileException& e) {
         GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
         return;
      }
   }
   
   QDialog::done(r);
}

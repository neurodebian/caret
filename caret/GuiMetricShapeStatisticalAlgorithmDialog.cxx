
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
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QProgressBar>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextStream>

#include "BrainModelSurfaceMetricCoordinateDifference.h"
#include "BrainModelSurfaceMetricInterHemClusters.h"
#include "BrainModelSurfaceMetricOneAndPairedTTest.h"
#include "BrainModelSurfaceMetricTwoSampleTTest.h"
#include "FileUtilities.h"
#include "GuiDataFileDialog.h"
#include "GuiFileSelectionButton.h"
#include "GuiFileSelectionButtonAndLineEditWidget.h"
#include "GuiFileSelectionListWidget.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiMetricShapeStatisticalAlgorithmDialog.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "QtWidgetGroup.h"
#include "SurfaceShapeFile.h"
#include "SystemUtilities.h"
#include "global_variables.h"

/**
 * constructor
 */
GuiMetricShapeStatisticalAlgorithmDialog::GuiMetricShapeStatisticalAlgorithmDialog(QWidget* parent)
   : QtDialog(parent)
{
   setWindowTitle("Statistical Algorithm Processing");

   maximumNumberOfInputFileGroupsForDisplay = 0;
   maximumNumberOfSingleInputFilesForDisplay = 0;
   
   mode = ALGORITHM_MODE_NONE;
   
   //
   // Keeps track of left/right widgets
   //
   rightLeftTMapWidgetsGroup = new QtWidgetGroup(this);

   const QString dataFileExt = SpecFile::getSurfaceShapeFileExtension();
   //
   // name of files for the various algorithms
   //
   algorithmFileNames[ALGORITHM_MODE_NONE].outputMetricShapeFileName = "";
   algorithmFileNames[ALGORITHM_MODE_NONE].outputMetricShapeShuffledTMapFileName = "";
   algorithmFileNames[ALGORITHM_MODE_NONE].outputPaintClustersFileName = "";
   algorithmFileNames[ALGORITHM_MODE_NONE].outputMetricClustersFileName = "";
   algorithmFileNames[ALGORITHM_MODE_NONE].outputReportFileName = "";
   //-------------------
   algorithmFileNames[ALGORITHM_MODE_COORDINATE_DIFFERENCE].outputMetricShapeFileName = "distance" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_COORDINATE_DIFFERENCE].outputMetricShapeShuffledTMapFileName = "distance_shuffled" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_COORDINATE_DIFFERENCE].outputPaintClustersFileName = "distance_clusters" + SpecFile::getPaintFileExtension();
   algorithmFileNames[ALGORITHM_MODE_COORDINATE_DIFFERENCE].outputMetricClustersFileName = "distance_clusters" + SpecFile::getMetricFileExtension();
   algorithmFileNames[ALGORITHM_MODE_COORDINATE_DIFFERENCE].outputReportFileName = "distance_statistical_report.txt";
   //-------------------
   algorithmFileNames[ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP].outputMetricShapeFileName = "Tdistance" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP].outputMetricShapeShuffledTMapFileName = "Tdistance_shuffled" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP].outputPaintClustersFileName = "Tdistance_clusters" + SpecFile::getPaintFileExtension();
   algorithmFileNames[ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP].outputMetricClustersFileName = "Tdistance_clusters" + SpecFile::getMetricFileExtension();
   algorithmFileNames[ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP].outputReportFileName = "Tdistance_statistical_report.txt";
   //-------------------
   algorithmFileNames[ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS].outputMetricShapeFileName = "TMap" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS].outputMetricShapeShuffledTMapFileName = "Shuffled_TMap" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS].outputPaintClustersFileName = "clusters" + SpecFile::getPaintFileExtension();
   algorithmFileNames[ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS].outputMetricClustersFileName = "clusters" + SpecFile::getMetricFileExtension();
   algorithmFileNames[ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS].outputReportFileName = "statistical_report.txt";
   //-------------------
   algorithmFileNames[ALGORITHM_MODE_ONE_SAMPLE_T_TEST].outputMetricShapeFileName = "TMap" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_ONE_SAMPLE_T_TEST].outputMetricShapeShuffledTMapFileName = "Shuffled_TMap" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_ONE_SAMPLE_T_TEST].outputPaintClustersFileName = "clusters" + SpecFile::getPaintFileExtension();
   algorithmFileNames[ALGORITHM_MODE_ONE_SAMPLE_T_TEST].outputMetricClustersFileName = "clusters" + SpecFile::getMetricFileExtension();
   algorithmFileNames[ALGORITHM_MODE_ONE_SAMPLE_T_TEST].outputReportFileName = "statistical_report.txt";
   //-------------------
   algorithmFileNames[ALGORITHM_MODE_PAIRED_T_TEST].outputMetricShapeFileName = "TMap" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_PAIRED_T_TEST].outputMetricShapeShuffledTMapFileName = "Shuffled_TMap" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_PAIRED_T_TEST].outputPaintClustersFileName = "clusters" + SpecFile::getPaintFileExtension();
   algorithmFileNames[ALGORITHM_MODE_PAIRED_T_TEST].outputMetricClustersFileName = "clusters" + SpecFile::getMetricFileExtension();
   algorithmFileNames[ALGORITHM_MODE_PAIRED_T_TEST].outputReportFileName = "statistical_report.txt";
   //-------------------
   algorithmFileNames[ALGORITHM_MODE_TWO_SAMPLE_T_TEST].outputMetricShapeFileName = "TMap" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_TWO_SAMPLE_T_TEST].outputMetricShapeShuffledTMapFileName = "Shuffled_TMap" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_TWO_SAMPLE_T_TEST].outputPaintClustersFileName = "clusters" + SpecFile::getPaintFileExtension();
   algorithmFileNames[ALGORITHM_MODE_TWO_SAMPLE_T_TEST].outputMetricClustersFileName = "clusters" + SpecFile::getMetricFileExtension();
   algorithmFileNames[ALGORITHM_MODE_TWO_SAMPLE_T_TEST].outputReportFileName = "statistical_report.txt";
   //-------------------
   algorithmFileNames[ALGORITHM_MODE_WILCOXON_INTO_TWO_SAMPLE_T_TEST].outputMetricShapeFileName = "TMap" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_WILCOXON_INTO_TWO_SAMPLE_T_TEST].outputMetricShapeShuffledTMapFileName = "Shuffled_TMap" + dataFileExt;
   algorithmFileNames[ALGORITHM_MODE_WILCOXON_INTO_TWO_SAMPLE_T_TEST].outputPaintClustersFileName = "clusters" + SpecFile::getPaintFileExtension();
   algorithmFileNames[ALGORITHM_MODE_WILCOXON_INTO_TWO_SAMPLE_T_TEST].outputMetricClustersFileName = "clusters" + SpecFile::getMetricFileExtension();
   algorithmFileNames[ALGORITHM_MODE_WILCOXON_INTO_TWO_SAMPLE_T_TEST].outputReportFileName = "statistical_report.txt";
   //-------------------
   
   lineEditWidth = 300;
   numericMaxWidth = 150;

   //
   // create widgets
   //
   pageAlgorithmSelection = createAlgorithmWidget();
   pageInputSingleFiles = createSingleFilesInputWidget();
   pageInputMultipleFiles = createMultipleFilesInputWidget();
   pageInputSurfaceFiles = createSurfaceFilesInputWidget();
   pageInputClusterSearch = createClusterSearchInputWidget();
   pageOutputFiles = createOutputWidget();
   pageOptions = createOptionsWidget();
   pageThreads = createThreadsWidget();
   pageRunAlgorithm = createRunAlgorithmWidget();
   pageReport = createReportWidget();
   
   //
   // stacked widget
   //
   stackedWidget = new QStackedWidget;
   stackedWidget->addWidget(pageAlgorithmSelection);
   stackedWidget->addWidget(pageInputSingleFiles);
   stackedWidget->addWidget(pageInputMultipleFiles);
   stackedWidget->addWidget(pageInputSurfaceFiles);
   stackedWidget->addWidget(pageInputClusterSearch);
   stackedWidget->addWidget(pageOutputFiles);
   stackedWidget->addWidget(pageOptions);
   stackedWidget->addWidget(pageThreads);
   stackedWidget->addWidget(pageRunAlgorithm);
   stackedWidget->addWidget(pageReport);
   
   //
   // Initialize all pages as valid
   //
   for (int i = 0; i < stackedWidget->count(); i++) {
      pageValidityFlag[stackedWidget->widget(i)] = true;
   }
   
   //
   // prev push button
   //
   prevPushButton = new QPushButton("Prev");
   prevPushButton->setAutoDefault(false);
   QObject::connect(prevPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPrevPushButton()));
                    
   //
   // next push button
   //
   nextPushButton = new QPushButton("Next");
   nextPushButton->setAutoDefault(false);
   QObject::connect(nextPushButton, SIGNAL(clicked()),
                    this, SLOT(slotNextPushButton()));
                    
   //
   // Run push button
   //
   runPushButton = new QPushButton("Run");
   runPushButton->setAutoDefault(false);
   QObject::connect(runPushButton, SIGNAL(clicked()),
                    this, SLOT(slotRunPushButton()));
                    
   //
   // close push button
   //
   QPushButton* closePushButton = new QPushButton("Close");
   closePushButton->setAutoDefault(false);
   QObject::connect(closePushButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   //
   // help push button
   //
   QPushButton* helpPushButton = new QPushButton("Help");
   helpPushButton->setAutoDefault(false);
   QObject::connect(helpPushButton, SIGNAL(clicked()),
                    this, SLOT(slotShowHelpPage()));
                    
   //
   // Make push buttons same size
   //
   QtUtilities::makeButtonsSameSize(prevPushButton,
                                    nextPushButton,
                                    runPushButton,
                                    closePushButton,
                                    helpPushButton);
   
   //
   // layout buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(prevPushButton);
   buttonsLayout->addWidget(nextPushButton);
   buttonsLayout->addWidget(runPushButton);
   buttonsLayout->addWidget(closePushButton);
   buttonsLayout->addWidget(helpPushButton);

   //
   // layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(stackedWidget);
   dialogLayout->addLayout(buttonsLayout);
   
   slotAreaCorrectionShapeFile("");
   slotEnableDisablePushButtons();
}   

/**
 * destructor.
 */
GuiMetricShapeStatisticalAlgorithmDialog::~GuiMetricShapeStatisticalAlgorithmDialog()
{
}

/**
 * called when prev button pressed.
 */
void 
GuiMetricShapeStatisticalAlgorithmDialog::slotPrevPushButton()
{
   QWidget* prevPage = NULL;
   
   int indx = stackedWidget->currentIndex() - 1;
   while ((indx >= 0) && (indx < stackedWidget->count())) {
      prevPage = stackedWidget->widget(indx);
      if (pageValidityFlag[prevPage]) {
         break;
      }
      prevPage = NULL;
      indx--;
   }
   
   if (prevPage != NULL) {
      showPage(prevPage);
   }
}

/**
 * called when next button pressed.
 */
void 
GuiMetricShapeStatisticalAlgorithmDialog::slotNextPushButton()
{
   QWidget* prevPage = stackedWidget->currentWidget();
   QWidget* nextPage = NULL;
   
   int indx = stackedWidget->currentIndex() + 1;
   while ((indx >= 0) && (indx < stackedWidget->count())) {
      nextPage = stackedWidget->widget(indx);
      if (pageValidityFlag[nextPage]) {
         break;
      }
      nextPage = NULL;
      indx++;
   }
   
   if (nextPage != NULL) {
      //
      // If last page was output files
      //
      if (prevPage == pageOutputFiles) {      
         //
         // Save the file names entered by the user
         //
         algorithmFileNames[mode].outputMetricShapeFileName = tMapShapeFileSelectionControl->getFileName();
         algorithmFileNames[mode].outputMetricShapeShuffledTMapFileName = shuffledTMapShapeFileSelectionControl->getFileName();
         algorithmFileNames[mode].outputPaintClustersFileName = paintClustersFileSelectionControl->getFileName();
         algorithmFileNames[mode].outputMetricClustersFileName = metricClustersFileSelectionControl->getFileName();
         algorithmFileNames[mode].outputReportFileName = reportFileControl->getFileName();
      }
      
      //
      // Show the nextpage
      //
      showPage(nextPage);
   }
}

/**
 * called when next button pressed.
 */
void 
GuiMetricShapeStatisticalAlgorithmDialog::slotRunPushButton()
{
   //
   // overwrite files check
   //
   const QString tMapFileName(tMapShapeFileSelectionControl->getFileName());
   if (QFile::exists(tMapFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(tMapFileName).toAscii().constData()
          << " ?";
      if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
         return;
      }
   }
   const QString shuffledTMapFileName(shuffledTMapShapeFileSelectionControl->getFileName());
   if (QFile::exists(shuffledTMapFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(shuffledTMapFileName).toAscii().constData()
          << " ?";
      if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
         return;
      }
   }
   const QString reportFileName(reportFileControl->getFileName());
   if (QFile::exists(reportFileName)) {
      std::ostringstream str;
      str << "Overwrite "
          << FileUtilities::basename(reportFileName).toAscii().constData()
          << " ?";
      if (GuiMessageBox::question(this, "OVERWRITE", str.str().c_str(), "Yes", "No") != 0) {
         return;
      }
   }
   const QString paintFileName(paintClustersFileSelectionControl->getFileName());
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
   const QString metricClustersFileName(metricClustersFileSelectionControl->getFileName());
   if (metricClustersFileName.isEmpty() == false) {
      if (QFile::exists(metricClustersFileName)) {
         std::ostringstream str;
         str << "Overwrite "
             << FileUtilities::basename(metricClustersFileName).toAscii().constData()
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

   BrainModelAlgorithm* bma = NULL;
   
   switch (mode) {
      case ALGORITHM_MODE_NONE:
         return;
         break;
      case ALGORITHM_MODE_COORDINATE_DIFFERENCE:
         {
            std::vector<QString> groupA, groupB;
            inputFilesGroupListWidgets[0]->getFileNames(groupA);
            inputFilesGroupListWidgets[1]->getFileNames(groupB);
            bma = new BrainModelSurfaceMetricCoordinateDifference(
                        theMainWindow->getBrainSet(),
                        BrainModelSurfaceMetricCoordinateDifference::MODE_COORDINATE_DIFFERENCE,
                        groupA,
                        groupB,
                        fiducialCoordFileLineEdit->text(),
                        openTopoFileLineEdit->text(),
                        areaCorrectionShapeFileLineEdit->text(),
                        tMapFileName,
                        shuffledTMapFileName,
                        paintFileName,
                        metricClustersFileName,
                        reportFileName,
                        areaCorrectionShapeColumnComboBox->currentIndex(),
                        shuffledTMapIterationSpinBox->value(),
                        posThreshDoubleSpinBox->value(),
                        pValueDoubleSpinBox->value(),
                        threadsSpinBox->value());
         }
         break;
      case ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP:
         {
            std::vector<QString> groupA, groupB;
            inputFilesGroupListWidgets[0]->getFileNames(groupA);
            inputFilesGroupListWidgets[1]->getFileNames(groupB);
            bma = new BrainModelSurfaceMetricCoordinateDifference(
                        theMainWindow->getBrainSet(),
                        BrainModelSurfaceMetricCoordinateDifference::MODE_TMAP_DIFFERENCE,
                        groupA,
                        groupB,
                        fiducialCoordFileLineEdit->text(),
                        openTopoFileLineEdit->text(),
                        areaCorrectionShapeFileLineEdit->text(),
                        tMapFileName,
                        shuffledTMapFileName,
                        paintFileName,
                        metricClustersFileName,
                        reportFileName,
                        areaCorrectionShapeColumnComboBox->currentIndex(),
                        shuffledTMapIterationSpinBox->value(),
                        posThreshDoubleSpinBox->value(),
                        pValueDoubleSpinBox->value(),
                        threadsSpinBox->value());
         }
         break;
      case ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS:
         {
            bma = new BrainModelSurfaceMetricInterHemClusters(
                         theMainWindow->getBrainSet(),
                         inputFilesSingleSelectionControls[2]->getFileName(),
                         inputFilesSingleSelectionControls[3]->getFileName(),
                         inputFilesSingleSelectionControls[0]->getFileName(),
                         inputFilesSingleSelectionControls[1]->getFileName(),
                         fiducialCoordFileLineEdit->text(),
                         openTopoFileLineEdit->text(),
                         areaCorrectionShapeFileLineEdit->text(),
                         rightTMapMetricShapeControl->getFileName(),
                         leftTMapMetricShapeControl->getFileName(),
                         rightShuffledTMapMetricShapeControl->getFileName(),
                         leftShuffledTMapMetricShapeControl->getFileName(),
                         tMapFileName,
                         shuffledTMapFileName,
                         paintFileName,
                         metricClustersFileName,
                         reportFileName,
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
                         
         }
         break;
      case ALGORITHM_MODE_ONE_SAMPLE_T_TEST:
         {
            std::vector<QString> files;
            files.push_back(inputFilesSingleSelectionControls[0]->getFileName());
            bma = new BrainModelSurfaceMetricOneAndPairedTTest(
                         theMainWindow->getBrainSet(),
                         BrainModelSurfaceMetricOneAndPairedTTest::T_TEST_MODE_ONE_SAMPLE,
                         files,
                         fiducialCoordFileLineEdit->text(),
                         openTopoFileLineEdit->text(),
                         areaCorrectionShapeFileLineEdit->text(),
                         tMapFileName,
                         shuffledTMapFileName,
                         paintFileName,
                         metricClustersFileName,
                         reportFileName,
                         areaCorrectionShapeColumnComboBox->currentIndex(),
                         negThreshDoubleSpinBox->value(),
                         posThreshDoubleSpinBox->value(),
                         pValueDoubleSpinBox->value(),
                         varianceSmoothingIterations,
                         varianceSmoothingStrength,
                         shuffledTMapIterationSpinBox->value(),
                         tTestConstantDoubleSpinBox->value(),
                         threadsSpinBox->value());
         }             
         break;
      case ALGORITHM_MODE_PAIRED_T_TEST:
         {
            std::vector<QString> files;
            files.push_back(inputFilesSingleSelectionControls[0]->getFileName());
            files.push_back(inputFilesSingleSelectionControls[1]->getFileName());
            bma = new BrainModelSurfaceMetricOneAndPairedTTest(
                         theMainWindow->getBrainSet(),
                         BrainModelSurfaceMetricOneAndPairedTTest::T_TEST_MODE_PAIRED,
                         files,
                         fiducialCoordFileLineEdit->text(),
                         openTopoFileLineEdit->text(),
                         areaCorrectionShapeFileLineEdit->text(),
                         tMapFileName,
                         shuffledTMapFileName,
                         paintFileName,
                         metricClustersFileName,
                         reportFileName,
                         areaCorrectionShapeColumnComboBox->currentIndex(),
                         negThreshDoubleSpinBox->value(),
                         posThreshDoubleSpinBox->value(),
                         pValueDoubleSpinBox->value(),
                         varianceSmoothingIterations,
                         varianceSmoothingStrength,
                         shuffledTMapIterationSpinBox->value(),
                         tTestConstantDoubleSpinBox->value(),
                         threadsSpinBox->value());
         }             
         break;
      case ALGORITHM_MODE_TWO_SAMPLE_T_TEST:
         bma = new BrainModelSurfaceMetricTwoSampleTTest( 
                      theMainWindow->getBrainSet(),
                      BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_NONE,
                      BrainModelSurfaceMetricTwoSampleTTest::VARIANCE_MODE_UNPOOLED,
                      inputFilesSingleSelectionControls[0]->getFileName(),
                      inputFilesSingleSelectionControls[1]->getFileName(),
                      fiducialCoordFileLineEdit->text(),
                      openTopoFileLineEdit->text(),
                      areaCorrectionShapeFileLineEdit->text(),
                      tMapFileName,
                      shuffledTMapFileName,
                      paintFileName,
                      metricClustersFileName,
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
         break;
      case ALGORITHM_MODE_WILCOXON_INTO_TWO_SAMPLE_T_TEST:
         bma = new BrainModelSurfaceMetricTwoSampleTTest( 
                      theMainWindow->getBrainSet(),
                      BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_WILCOXON_RANK_SUM_THEN_TWO_SAMPLE_T_TEST,
                      BrainModelSurfaceMetricTwoSampleTTest::VARIANCE_MODE_UNPOOLED,
                      inputFilesSingleSelectionControls[0]->getFileName(),
                      inputFilesSingleSelectionControls[1]->getFileName(),
                      fiducialCoordFileLineEdit->text(),
                      openTopoFileLineEdit->text(),
                      areaCorrectionShapeFileLineEdit->text(),
                      tMapFileName,
                      shuffledTMapFileName,
                      paintFileName,
                      metricClustersFileName,
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
         break;
      case ALGORITHM_MODE_COUNT:
         return;
         break;
   }

   //
   // Attach the progress signal
   //
   QObject::connect(bma, SIGNAL(signalProgressUpdate(const QString&,
                                                     const int,
                                                     const int)),
                    this, SLOT(slotDisplayProgressOnRunPage(const QString&,
                                                            const int,
                                                            const int)));

   //
   // Set time algorithm started
   //
   runStartTime.start();
   
   //
   // Run the algorithm
   //                    
   try {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      bma->execute();
      delete bma;
      bma = NULL;
      QApplication::restoreOverrideCursor();
   }
   catch (BrainModelAlgorithmException &e) {
      delete bma;
      bma = NULL;
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
      showPage(pageReport);
   }
   reportFile.close();
   
   beep();
}

/**
 * enable disable push button.
 */
void 
GuiMetricShapeStatisticalAlgorithmDialog::slotEnableDisablePushButtons()
{
   bool nextValid = false;
   bool runValid  = false;
   
   //
   // get the currently displayed page
   //
   const QWidget* currentPage = stackedWidget->currentWidget();

   //
   // Algorithm Page
   //
   if (currentPage == pageAlgorithmSelection) {
      //
      // Make some settings based upon the algorithm selected
      //
      mode = ALGORITHM_MODE_NONE;
      maximumNumberOfInputFileGroupsForDisplay = static_cast<int>(inputFilesGroupListWidgets.size());
      maximumNumberOfSingleInputFilesForDisplay = static_cast<int>(inputFilesSingleSelectionControls.size());

      //
      // hide T-Test constant group and left/right widgets
      //
      tTestConstantWidgetGroup->setHidden(true);
      rightLeftTMapWidgetsGroup->setHidden(true);
   
      
      //
      // Initialize all pages as valid
      //
      for (int i = 0; i < stackedWidget->count(); i++) {
         pageValidityFlag[stackedWidget->widget(i)] = true;
      }

      //
      // Most algorithm's use T-Maps
      //
      tMapShapeFileSelectionControl->setButtonLabelText("T-Map Metric/Shape File...");
      shuffledTMapShapeFileSelectionControl->setButtonLabelText("Shuffled T-Map Metric/Shape File...");
      
      //
      // Most use a negative threshold
      //
      negativeThresholdWidgetGroup->setVisible(true);
      
      //
      // Setup some parameters based upon mode selected by user
      //
      if (algorithmCoordinateDifferenceRadioButton->isChecked()) {
         mode = ALGORITHM_MODE_COORDINATE_DIFFERENCE;
         inputFileFilters = QStringList(GuiDataFileDialog::coordinateGenericFileFilter);
         pageValidityFlag[pageInputSingleFiles] = false;
         pageValidityFlag[pageOptions] = false;
         maximumNumberOfInputFileGroupsForDisplay = 2;
         nextValid = true;
         
         tMapShapeFileSelectionControl->setButtonLabelText("Distance Metric/Shape File...");
         shuffledTMapShapeFileSelectionControl->setButtonLabelText("Shuffled Distance Metric/Shape File...");
         negativeThresholdWidgetGroup->setHidden(true);
         
         if (tMapShapeFileSelectionControl->getFileName().indexOf("TMap") >= 0) {
            tMapShapeFileSelectionControl->setFileName("distance.surface_shape");
         }
         if (shuffledTMapShapeFileSelectionControl->getFileName().indexOf("TMap") >= 0) {
            shuffledTMapShapeFileSelectionControl->setFileName("shuffled_distance.surface_shape");
         }
      }
      else if (algorithmCoordinateDifferenceTMapRadioButton->isChecked()) {
         mode = ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP;
         inputFileFilters = QStringList(GuiDataFileDialog::coordinateGenericFileFilter);
         pageValidityFlag[pageInputSingleFiles] = false;
         pageValidityFlag[pageOptions] = false;
         maximumNumberOfInputFileGroupsForDisplay = 2;
         nextValid = true;
         
         tMapShapeFileSelectionControl->setButtonLabelText("Distance Metric/Shape File...");
         shuffledTMapShapeFileSelectionControl->setFileName("Shuffled Distance Metric/Shape File...");
         negativeThresholdWidgetGroup->setHidden(true);
         
         if (tMapShapeFileSelectionControl->getFileName().indexOf("TMap") >= 0) {
            tMapShapeFileSelectionControl->setFileName("distance.surface_shape");
         }
         if (shuffledTMapShapeFileSelectionControl->getFileName().indexOf("TMap") >= 0) {
            shuffledTMapShapeFileSelectionControl->setFileName("shuffled_distance.surface_shape");
         }
      }
      else if (algorithmInterhemisphericClustersRadioButton->isChecked()) {
         mode = ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS;
         inputFileFilters = QStringList(GuiDataFileDialog::metricShapeFileFilter);
         pageValidityFlag[pageInputMultipleFiles] = false;
         maximumNumberOfSingleInputFilesForDisplay = 4;
         inputFilesSingleSelectionControls[0]->setButtonLabelText("Left Metric/Shape File A");
         inputFilesSingleSelectionControls[1]->setButtonLabelText("Left Metric/Shape File B");
         inputFilesSingleSelectionControls[2]->setButtonLabelText("Right Metric/Shape File A");
         inputFilesSingleSelectionControls[3]->setButtonLabelText("Right Metric/Shape File B");
         rightLeftTMapWidgetsGroup->setHidden(false);
         nextValid = true;
      }
      else if (algorithmOneSampleTTestRadioButton->isChecked()) {
         mode = ALGORITHM_MODE_ONE_SAMPLE_T_TEST;
         inputFileFilters = QStringList(GuiDataFileDialog::metricShapeFileFilter);
         pageValidityFlag[pageInputMultipleFiles] = false;
         pageValidityFlag[pageOptions] = false;
         maximumNumberOfSingleInputFilesForDisplay = 1;
         inputFilesSingleSelectionControls[0]->setButtonLabelText("Metric/Shape File");
         tTestConstantWidgetGroup->setHidden(false);
         nextValid = true;
      }
      else if (algorithmPairedTTestRadioButton->isChecked()) {
         mode = ALGORITHM_MODE_PAIRED_T_TEST;
         inputFileFilters = QStringList(GuiDataFileDialog::metricShapeFileFilter);
         pageValidityFlag[pageInputMultipleFiles] = false;
         pageValidityFlag[pageOptions] = false;
         maximumNumberOfSingleInputFilesForDisplay = 2;
         inputFilesSingleSelectionControls[0]->setButtonLabelText("Metric/Shape File A");
         inputFilesSingleSelectionControls[1]->setButtonLabelText("Metric/Shape File B");
         nextValid = true;
      }
      else if (algorithmTwoSampleTTestRadioButton->isChecked()) {
         mode = ALGORITHM_MODE_TWO_SAMPLE_T_TEST;
         inputFileFilters = QStringList(GuiDataFileDialog::metricShapeFileFilter);
         pageValidityFlag[pageInputMultipleFiles] = false;
         maximumNumberOfSingleInputFilesForDisplay = 2;
         inputFilesSingleSelectionControls[0]->setButtonLabelText("Metric/Shape File A");
         inputFilesSingleSelectionControls[1]->setButtonLabelText("Metric/Shape File B");
         nextValid = true;
      }
      else if (algorithmWilcoxonRankSumRadioButton->isChecked()) {
         mode = ALGORITHM_MODE_WILCOXON_INTO_TWO_SAMPLE_T_TEST;
         inputFileFilters = QStringList(GuiDataFileDialog::metricShapeFileFilter);
         pageValidityFlag[pageInputMultipleFiles] = false;
         maximumNumberOfSingleInputFilesForDisplay = 2;
         inputFilesSingleSelectionControls[0]->setButtonLabelText("Metric/Shape File A");
         inputFilesSingleSelectionControls[1]->setButtonLabelText("Metric/Shape File B");
         nextValid = true;
      }
      
      //
      // Load up the file names
      //
      tMapShapeFileSelectionControl->setFileName(algorithmFileNames[mode].outputMetricShapeFileName);
      shuffledTMapShapeFileSelectionControl->setFileName(algorithmFileNames[mode].outputMetricShapeShuffledTMapFileName);
      paintClustersFileSelectionControl->setFileName(algorithmFileNames[mode].outputPaintClustersFileName);
      metricClustersFileSelectionControl->setFileName(algorithmFileNames[mode].outputMetricClustersFileName);
      reportFileControl->setFileName(algorithmFileNames[mode].outputReportFileName);
      
      //
      // Update group input files filter
      //
      for (int i = 0; i < static_cast<int>(inputFilesGroupListWidgets.size()); i++) {
         if (i < maximumNumberOfInputFileGroupsForDisplay) {
            inputFilesGroupListWidgets[i]->setHidden(false);
         }
         else {
            inputFilesGroupListWidgets[i]->setHidden(true);
         }
         inputFilesGroupListWidgets[i]->setFileSelectionFilters(inputFileFilters);
      }
      
      //
      // Update single files
      //
      for (int i = 0; i < static_cast<int>(inputFilesSingleSelectionControls.size()); i++) {
         if (i < maximumNumberOfSingleInputFilesForDisplay) {
            inputFilesSingleSelectionControls[i]->setHidden(false);
         }
         else {
            inputFilesSingleSelectionControls[i]->setHidden(true);
         }
      }
   }
   else if (currentPage == pageInputSingleFiles) {
      nextValid = true;
      for (int i = 0; i < maximumNumberOfSingleInputFilesForDisplay; i++) {
         if (inputFilesSingleSelectionControls[i]->getFileName().isEmpty()) {
            nextValid = false;
            break;
         }
      }
   }
   else if (currentPage == pageInputMultipleFiles) {
      nextValid = true;
      for (int i = 0; i < maximumNumberOfInputFileGroupsForDisplay; i++) {
         if (inputFilesGroupListWidgets[i]->containsFiles() == false) {
            nextValid = false;
            break;
         }
      }
   }
   else if (currentPage == pageInputSurfaceFiles) {
      if ((fiducialCoordFileLineEdit->text().isEmpty() == false) &&
          (openTopoFileLineEdit->text().isEmpty() == false) &&
          (areaCorrectionShapeFileLineEdit->text().isEmpty() == false) &&
          (areaCorrectionShapeColumnComboBox->count() > 0)) {
         nextValid = true;
      }
   }
   else if (currentPage == pageInputClusterSearch) {
      nextValid = true;
   }
   else if (currentPage == pageOutputFiles) {
      if ((tMapShapeFileSelectionControl->getFileName().isEmpty() == false) &&
          (shuffledTMapShapeFileSelectionControl->getFileName().isEmpty() == false) &&
          (paintClustersFileSelectionControl->getFileName().isEmpty() == false) &&
          (reportFileControl->getFileName().isEmpty() == false)) {
         if (mode == ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS) {
            if ((leftTMapMetricShapeControl->getFileName().isEmpty() == false) &&
                (rightTMapMetricShapeControl->getFileName().isEmpty() == false) &&
                (leftShuffledTMapMetricShapeControl->getFileName().isEmpty() == false) &&
                (rightShuffledTMapMetricShapeControl->getFileName().isEmpty() == false)) {
               nextValid = true;
            }
         }
         else {
            nextValid = true;
         }
      }
   }
   else if (currentPage == pageOptions) {
      nextValid = true;
   }
   else if (currentPage == pageThreads) {
      nextValid = true;
   }
   else if (currentPage == pageRunAlgorithm) {
      runValid = true;
   }
   else if (currentPage == pageReport) {
   }
   
   nextPushButton->setEnabled(nextValid);
   runPushButton->setEnabled(runValid);
}

/**
 * show a page.
 */
void 
GuiMetricShapeStatisticalAlgorithmDialog::showPage(QWidget* page)
{
   switch (mode) {
      case ALGORITHM_MODE_NONE:
         break;
      case ALGORITHM_MODE_COORDINATE_DIFFERENCE:
         break;
      case ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP:
         break;
      case ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS:
         break;
      case ALGORITHM_MODE_ONE_SAMPLE_T_TEST:
         break;
      case ALGORITHM_MODE_PAIRED_T_TEST:
         break;
      case ALGORITHM_MODE_TWO_SAMPLE_T_TEST:
         break;
      case ALGORITHM_MODE_WILCOXON_INTO_TWO_SAMPLE_T_TEST:
         break;
      case ALGORITHM_MODE_COUNT:
         break;
   }
   
   if (page == pageInputMultipleFiles) {
   }
   
   stackedWidget->setCurrentWidget(page);
   slotEnableDisablePushButtons();
}

/**
 * show the help page.
 */
void 
GuiMetricShapeStatisticalAlgorithmDialog::slotShowHelpPage()
{
   switch (mode) {
      case ALGORITHM_MODE_NONE:
         theMainWindow->showHelpViewerDialog("index.html");
         break;
      case ALGORITHM_MODE_COORDINATE_DIFFERENCE:
         theMainWindow->showHelpViewerDialog("statistics/coordinate_difference.html");
         break;
      case ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP:
         theMainWindow->showHelpViewerDialog("statistics/coordinate_difference.html");
         break;
      case ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS:
         theMainWindow->showHelpViewerDialog("statistics/interhemispheric_clusters.html");
         break;
      case ALGORITHM_MODE_ONE_SAMPLE_T_TEST:
         theMainWindow->showHelpViewerDialog("statistics/one_sample_t_test.html");
         break;
      case ALGORITHM_MODE_PAIRED_T_TEST:
         theMainWindow->showHelpViewerDialog("statistics/one_sample_t_test.html");
         break;
      case ALGORITHM_MODE_TWO_SAMPLE_T_TEST:
         theMainWindow->showHelpViewerDialog("statistics/two_sample_t_test.html");
         break;
      case ALGORITHM_MODE_WILCOXON_INTO_TWO_SAMPLE_T_TEST:
         theMainWindow->showHelpViewerDialog("statistics/wilcoxon_rank_sum.html");
         break;
      case ALGORITHM_MODE_COUNT:
         break;
   }
}
      
/**
 * create the algorithm widget.
 */
QWidget* 
GuiMetricShapeStatisticalAlgorithmDialog::createAlgorithmWidget()
{
   //
   // Algorithm selection radio buttons
   //
   algorithmCoordinateDifferenceRadioButton = new QRadioButton("Coordinate File Group Difference");
   algorithmCoordinateDifferenceTMapRadioButton = new QRadioButton("Coordinate File T-Map Group Difference");
   algorithmInterhemisphericClustersRadioButton = new QRadioButton("Interhemispheric Clusters");
   algorithmOneSampleTTestRadioButton = new QRadioButton("One-Sample T-Test");
   algorithmPairedTTestRadioButton = new QRadioButton("Paired T-Test (Dependent Means)");
   algorithmTwoSampleTTestRadioButton = new QRadioButton("Two-Sample T-Test (Independent Means)");
   algorithmWilcoxonRankSumRadioButton = new QRadioButton("Wilcoxon Rank Sum into Two-Sample T-Test");
   
   //
   // Button group to keep selections mutually exclusive
   //
   QButtonGroup* buttGroup = new QButtonGroup(this);
   QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisablePushButtons()));
   buttGroup->addButton(algorithmCoordinateDifferenceRadioButton);
   buttGroup->addButton(algorithmCoordinateDifferenceTMapRadioButton);
   buttGroup->addButton(algorithmInterhemisphericClustersRadioButton);
   buttGroup->addButton(algorithmOneSampleTTestRadioButton);
   buttGroup->addButton(algorithmPairedTTestRadioButton);
   buttGroup->addButton(algorithmTwoSampleTTestRadioButton);
   buttGroup->addButton(algorithmWilcoxonRankSumRadioButton);
   
   //
   // Widget and layout for widget
   //
   QGroupBox* groupBox = new QGroupBox("Algorithm");
   QVBoxLayout* layout = new QVBoxLayout(groupBox);
   layout->addWidget(algorithmCoordinateDifferenceRadioButton);
   layout->addWidget(algorithmCoordinateDifferenceTMapRadioButton);
   layout->addWidget(algorithmInterhemisphericClustersRadioButton);
   layout->addWidget(algorithmOneSampleTTestRadioButton);
   layout->addWidget(algorithmPairedTTestRadioButton);
   layout->addWidget(algorithmTwoSampleTTestRadioButton);
   layout->addWidget(algorithmWilcoxonRankSumRadioButton);
   layout->addStretch();
   
   return groupBox;
}

/**
 * create the multiple files input widget.
 */
QWidget* 
GuiMetricShapeStatisticalAlgorithmDialog::createMultipleFilesInputWidget()
{
   const int numberOfGroups = 10;
   char groupTitleChar = 'A';
   
   for (int i = 0; i < numberOfGroups; i++) {
      QString groupTitle = "Group " + QString(groupTitleChar);
      groupTitleChar++;
      GuiFileSelectionListWidget* lw = new GuiFileSelectionListWidget(groupTitle);
      lw->setMinimumHeight(200);
      QObject::connect(lw, SIGNAL(signalFilesChanged()),
                       this, SLOT(slotEnableDisablePushButtons()));
      inputFilesGroupListWidgets.push_back(lw);
   }
   
   //
   // widget and layout for page
   //
   QGroupBox* w = new QGroupBox("Input Data File Groups");
   QVBoxLayout* layout = new QVBoxLayout(w);
   for (unsigned int i = 0; i < inputFilesGroupListWidgets.size(); i++) {
      layout->addWidget(inputFilesGroupListWidgets[i]);
   }
   
   //
   // Put in scrollable widget
   //
   QScrollArea* scrollArea = new QScrollArea;
   scrollArea->setWidget(w);
   scrollArea->setWidgetResizable(true);
   return scrollArea;
}
      
      
/**
 * create the single files input widget.
 */
QWidget* 
GuiMetricShapeStatisticalAlgorithmDialog::createSingleFilesInputWidget()
{   
   const int numberOfFileEntries = 4;
   
   //
   // Create the file entries
   //
   char letter = 'A';
   for (int i = 0; i < numberOfFileEntries; i++) {
      const QString label = "Metric/Shape File " + QString(letter);
      GuiFileSelectionButtonAndLineEditWidget* fileSelectionWidget =
            new GuiFileSelectionButtonAndLineEditWidget(label,
                                                        GuiDataFileDialog::metricShapeFileFilter,
                                                        true);
      QObject::connect(fileSelectionWidget, SIGNAL(fileSelected(const QString&)),
                       this, SLOT(slotEnableDisablePushButtons()));
      
      inputFilesSingleSelectionControls.push_back(fileSelectionWidget);
      letter++;
   }
   
   //
   // Widgets for input
   //
   QGroupBox* inputsWidget = new QGroupBox("Input Data Files");
   QVBoxLayout* inputsLayout = new QVBoxLayout(inputsWidget);
   for (unsigned int i = 0; i < inputFilesSingleSelectionControls.size(); i++) {
      inputsLayout->addWidget(inputFilesSingleSelectionControls[i]);
   }
   
   //
   // widget and grid layout
   //
   QWidget* w = new QWidget;  
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(inputsWidget);
   layout->addStretch();
   
   return w;
}

/**
 * create the surface files widget.
 */
QWidget* 
GuiMetricShapeStatisticalAlgorithmDialog::createSurfaceFilesInputWidget()
{
   //
   // fiducial coord file button and line edit
   //
   GuiFileSelectionButton* fiducialCoordFileButton = new GuiFileSelectionButton(0,
                                                                    "Fiducial Coord File...",
                                                                    GuiDataFileDialog::coordinateFiducialFileFilter,
                                                                    true);
   fiducialCoordFileLineEdit = new QLineEdit;
   fiducialCoordFileLineEdit->setReadOnly(true);
   fiducialCoordFileLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(fiducialCoordFileButton, SIGNAL(fileSelected(const QString&)),
                    fiducialCoordFileLineEdit, SLOT(setText(const QString&)));
   QObject::connect(fiducialCoordFileButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));

   //
   // open topo file button and line edit
   //
   GuiFileSelectionButton* openTopoFileButton = new GuiFileSelectionButton(0,
                                                                    "Open Topology File...",
                                                                    GuiDataFileDialog::topologyOpenFileFilter,
                                                                    true);
   openTopoFileLineEdit = new QLineEdit;
   openTopoFileLineEdit->setReadOnly(true);
   openTopoFileLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(openTopoFileButton, SIGNAL(fileSelected(const QString&)),
                    openTopoFileLineEdit, SLOT(setText(const QString&)));
   QObject::connect(openTopoFileButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));

   //
   // area correction shapefile button and line edit
   //
   GuiFileSelectionButton* areaCorrectionShapeFileButton = new GuiFileSelectionButton(0,
                                                                    "Area Correction Metric/Shape File...",
                                                                    GuiDataFileDialog::metricShapeFileFilter,
                                                                    true);
   areaCorrectionShapeFileLineEdit = new QLineEdit;
   areaCorrectionShapeFileLineEdit->setReadOnly(true);
   areaCorrectionShapeFileLineEdit->setMinimumWidth(lineEditWidth);
   QObject::connect(areaCorrectionShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    areaCorrectionShapeFileLineEdit, SLOT(setText(const QString&)));
   QObject::connect(areaCorrectionShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotAreaCorrectionShapeFile(const QString&)));
   QObject::connect(areaCorrectionShapeFileButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));

   //
   // area correction shape file column
   //
   QLabel* shapeCorrectionColumnLabel = new QLabel("Area Correction Shape Column");
   areaCorrectionShapeColumnComboBox = new QComboBox;
   QObject::connect(areaCorrectionShapeColumnComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotEnableDisablePushButtons()));
   

   //
   // Widgets for input
   //
   QGroupBox* inputsWidget = new QGroupBox("Surface Files");
   QGridLayout* grid = new QGridLayout(inputsWidget);
   grid->setMargin(5);
   grid->setSpacing(5);
   
   //
   // arrange dialog items
   //
   grid->addWidget(fiducialCoordFileButton, 2, 0);
   grid->addWidget(fiducialCoordFileLineEdit, 2, 1);
   grid->addWidget(openTopoFileButton, 3, 0);
   grid->addWidget(openTopoFileLineEdit, 3, 1);
   grid->addWidget(areaCorrectionShapeFileButton, 4, 0);
   grid->addWidget(areaCorrectionShapeFileLineEdit, 4, 1);
   grid->addWidget(shapeCorrectionColumnLabel, 5, 0);
   grid->addWidget(areaCorrectionShapeColumnComboBox, 5, 1);
   
   //
   // widget and grid layout
   //
   QWidget* w = new QWidget;  
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(inputsWidget);
   layout->addStretch();
   return w;
}

/**
 * create the cluster search widget.
 */
QWidget* 
GuiMetricShapeStatisticalAlgorithmDialog::createClusterSearchInputWidget()
{
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
   // Widget group for hiding negative threshold items
   //
   negativeThresholdWidgetGroup = new QtWidgetGroup(this);
   negativeThresholdWidgetGroup->addWidget(negThreshLabel);
   negativeThresholdWidgetGroup->addWidget(negThreshDoubleSpinBox);
   
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
   // Left/Right Shuffled T-Map Iterations
   //
   QLabel* leftRightShuffleTMapIterationsLabel = new QLabel("Left/Right Shuffled T-Map Iterations");
   leftRightShuffledTMapIterationSpinBox = new QSpinBox;
   leftRightShuffledTMapIterationSpinBox->setMinimum(0);
   leftRightShuffledTMapIterationSpinBox->setMaximum(10000000);
   leftRightShuffledTMapIterationSpinBox->setSingleStep(10);
   leftRightShuffledTMapIterationSpinBox->setFixedWidth(numericMaxWidth);
   leftRightShuffledTMapIterationSpinBox->setValue(500);
   rightLeftTMapWidgetsGroup->addWidget(leftRightShuffleTMapIterationsLabel);
   rightLeftTMapWidgetsGroup->addWidget(leftRightShuffledTMapIterationSpinBox);
   
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
   // T-Test Constant
   //
   QLabel* tTestConstantLabel = new QLabel("T-Test Constant");
   tTestConstantDoubleSpinBox = new QDoubleSpinBox;
   tTestConstantDoubleSpinBox->setMinimum(-1.0e10);
   tTestConstantDoubleSpinBox->setMaximum( 1.0e10);
   tTestConstantDoubleSpinBox->setSingleStep(0.01);
   tTestConstantDoubleSpinBox->setDecimals(6);
   tTestConstantDoubleSpinBox->setFixedWidth(numericMaxWidth);
   tTestConstantDoubleSpinBox->setValue(0.0);
   tTestConstantWidgetGroup = new QtWidgetGroup(this);
   tTestConstantWidgetGroup->addWidget(tTestConstantLabel);
   tTestConstantWidgetGroup->addWidget(tTestConstantDoubleSpinBox);
   
   //
   // Widgets for input
   //
   QGroupBox* inputsWidget = new QGroupBox("Cluster Search");
   QGridLayout* grid = new QGridLayout(inputsWidget);
   grid->setMargin(5);
   grid->setSpacing(5);
   
   //
   // arrange dialog items
   //
   grid->addWidget(negThreshLabel, 0, 0);
   grid->addWidget(negThreshDoubleSpinBox, 0, 1, Qt::AlignLeft);
   grid->addWidget(posThreshLabel, 1, 0);
   grid->addWidget(posThreshDoubleSpinBox, 1, 1, Qt::AlignLeft);
   grid->addWidget(shuffleTMapIterationsLabel, 2, 0);
   grid->addWidget(shuffledTMapIterationSpinBox, 2, 1, Qt::AlignLeft);
   grid->addWidget(leftRightShuffleTMapIterationsLabel, 3, 0);
   grid->addWidget(leftRightShuffledTMapIterationSpinBox, 3, 1, Qt::AlignLeft);
   grid->addWidget(pValueLabel, 4, 0);
   grid->addWidget(pValueDoubleSpinBox, 4, 1, Qt::AlignLeft);
   grid->addWidget(tTestConstantLabel, 5, 0);
   grid->addWidget(tTestConstantDoubleSpinBox, 5, 1, Qt::AlignLeft);
   
   //
   // widget and grid layout
   //
   QWidget* w = new QWidget;  
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(inputsWidget);
   layout->addStretch();
   return w;
}

/**
 * create the options widget.
 */
QWidget* 
GuiMetricShapeStatisticalAlgorithmDialog::createOptionsWidget()
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
   // Group Box for statistical options
   //
   QGroupBox* statGroupBox = new QGroupBox("Statistical Options");
   QVBoxLayout* statLayout = new QVBoxLayout(statGroupBox);
   statLayout->addWidget(tMapDOFCheckBox);
   statLayout->addWidget(tMapPValueCheckBox);
   
   //
   // arrange the widgets
   //
   QGroupBox* w = new QGroupBox("Options");
   QVBoxLayout* bl = new QVBoxLayout(w);
   bl->addWidget(statGroupBox);
   bl->addWidget(createVarianceSmoothingWidget());
   bl->addStretch();
   
   return w;
}
   
/**
 * create the options widget.
 */
QWidget* 
GuiMetricShapeStatisticalAlgorithmDialog::createThreadsWidget()
{   
   //
   // Number of threads
   //
   QLabel* threadsLabel = new QLabel("Number of Threads for Cluster Search");
   threadsSpinBox = new QSpinBox;
   threadsSpinBox->setMinimum(1);
   threadsSpinBox->setMaximum(2048);
   threadsSpinBox->setSingleStep(1);
   threadsSpinBox->setFixedWidth(100);
   threadsSpinBox->setValue(1);
   QHBoxLayout* threadsLayout = new QHBoxLayout;
   threadsLayout->addWidget(threadsLabel);
   threadsLayout->addWidget(threadsSpinBox);
   threadsLayout->addStretch();
   
   const QString cpu = QString::number(SystemUtilities::getNumberOfProcessors());
   const QString text =
      "\n"
      "The cluster search algorithm may take a long time to run,\n"
      "particularly if the number of shuffled iterations is large.\n"
      "The Number of Threads for Cluster Search specifies the number\n"
      "of columns in the shuffled file that may searched concurrently.\n"
      "If your computer has more than one CPU or a \"multi-core\"\n"
      "CPU, setting the number of threads to the number of CPUs or\n"
      "cores may reduce the cluster search algorithms executing time.\n"
      "\n"
      "It appears that your computer has " + cpu + " CPUs and/or cores.\n"
      "";
   QLabel* threadsTextLabel = new QLabel(text);
      
   //
   // arrange the widgets
   //
   QWidget* w = new QWidget;
   QVBoxLayout* bl = new QVBoxLayout(w);
   bl->addLayout(threadsLayout);
   bl->addWidget(threadsTextLabel);
   bl->addStretch();
   
   return w;
}

/**
 * create the output widget.
 */
QWidget* 
GuiMetricShapeStatisticalAlgorithmDialog::createOutputWidget()
{
   //
   // t-map shape file button and line edit
   //
   tMapShapeFileSelectionControl = 
      new GuiFileSelectionButtonAndLineEditWidget("T-Map Metric/Shape File...",
                                                  GuiDataFileDialog::metricShapeFileFilter,
                                                  false);
   tMapShapeFileSelectionControl->setFileName("TMap.surface_shape");
   QObject::connect(tMapShapeFileSelectionControl, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));

   //
   // shuffled T-map file  button and line edit
   //
   shuffledTMapShapeFileSelectionControl = 
      new GuiFileSelectionButtonAndLineEditWidget("Shuffled T-Map Metric/Shape File...",
                                                  GuiDataFileDialog::metricShapeFileFilter,
                                                  false);
   shuffledTMapShapeFileSelectionControl->setFileName("Shuffled_TMap.surface_shape");
   QObject::connect(shuffledTMapShapeFileSelectionControl, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));

   //
   // paint file  button and line edit
   //
   paintClustersFileSelectionControl = 
      new GuiFileSelectionButtonAndLineEditWidget("Paint File...",
                                                  GuiDataFileDialog::paintFileFilter,
                                                  false);
   paintClustersFileSelectionControl->setFileName("Clusters.paint");
   QObject::connect(paintClustersFileSelectionControl, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));

   //
   // metric file  button and line edit
   //
   metricClustersFileSelectionControl = 
      new GuiFileSelectionButtonAndLineEditWidget("Metric File...",
                                                  GuiDataFileDialog::metricFileFilter,
                                                  false);
   metricClustersFileSelectionControl->setFileName("Clusters.metric");
   QObject::connect(metricClustersFileSelectionControl, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));

   //
   // report file button and line edit
   //
   reportFileControl = 
      new GuiFileSelectionButtonAndLineEditWidget("Report File...",
                                                  GuiDataFileDialog::textFileFilter,
                                                  false);
   reportFileControl->setFileName("Statistical_Report.txt");
   QObject::connect(reportFileControl, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));
/*
   switch (mode) {
      case ALGORITHM_MODE_NONE:
         break;
      case ALGORITHM_MODE_COORDINATE_DIFFERENCE:
         reportFileLineEdit->setText("Coordinate_Difference.txt");
         break;
      case ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP:
         reportFileLineEdit->setText("Coordinate_Difference.txt");
         break;
      case ALGORITHM_TWO_SAMPLE_T_TEST:
         reportFileLineEdit->setText("Two_Sample_Significant_Clusters.txt");
         break;
      case ALGORITHM_WILCOXON_INTO_TWO_SAMPLE_T_TEST:
         reportFileLineEdit->setText("Wilcoxon_Significant_Clusters.txt");
         break;
   }
*/

   //
   // Left T-Map Metric/Shape File
   // 
   leftTMapMetricShapeControl = 
      new GuiFileSelectionButtonAndLineEditWidget("Left T-Map Metric/Shape File...",
                                                  GuiDataFileDialog::metricShapeFileFilter,
                                                  false);
   leftTMapMetricShapeControl->setFileName("Left_TMap.surface_shape");
   QObject::connect(leftTMapMetricShapeControl, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));      
                                                                
   //
   // Left Shuffled T-Map Metric/Shape File
   // 
   leftShuffledTMapMetricShapeControl = 
      new GuiFileSelectionButtonAndLineEditWidget("Left Shuffled T-Map Metric/Shape File...",
                                                  GuiDataFileDialog::metricShapeFileFilter,
                                                  false);
   leftShuffledTMapMetricShapeControl->setFileName("Left_Shuffled_TMap.surface_shape");
   QObject::connect(leftShuffledTMapMetricShapeControl, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));      
                                                                
   //
   // Right T-Map Metric/Shape File
   // 
   rightTMapMetricShapeControl = 
      new GuiFileSelectionButtonAndLineEditWidget("Right T-Map Metric/Shape File...",
                                                  GuiDataFileDialog::metricShapeFileFilter,
                                                  false);
   rightTMapMetricShapeControl->setFileName("Right_TMap.surface_shape");
   QObject::connect(rightTMapMetricShapeControl, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));      
                                                                
   //
   // Right Shuffled T-Map Metric/Shape File
   // 
   rightShuffledTMapMetricShapeControl = 
      new GuiFileSelectionButtonAndLineEditWidget("Right Shuffled T-Map Metric/Shape File...",
                                                  GuiDataFileDialog::metricShapeFileFilter,
                                                  false);
   rightShuffledTMapMetricShapeControl->setFileName("Right_Shuffled_TMap.surface_shape");
   QObject::connect(rightShuffledTMapMetricShapeControl, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));      
      
   //
   // add left/right T-Maps to interhemispheric group
   //
   rightLeftTMapWidgetsGroup->addWidget(leftTMapMetricShapeControl);
   rightLeftTMapWidgetsGroup->addWidget(rightTMapMetricShapeControl);
   rightLeftTMapWidgetsGroup->addWidget(leftShuffledTMapMetricShapeControl);
   rightLeftTMapWidgetsGroup->addWidget(rightShuffledTMapMetricShapeControl);
   
   //
   // Make all of the buttons the same size
   //
   std::vector<QPushButton*> buttonWidgets;
   buttonWidgets.push_back(tMapShapeFileSelectionControl->getFileSelectionButton());
   buttonWidgets.push_back(shuffledTMapShapeFileSelectionControl->getFileSelectionButton());
   buttonWidgets.push_back(paintClustersFileSelectionControl->getFileSelectionButton());
   buttonWidgets.push_back(metricClustersFileSelectionControl->getFileSelectionButton());
   buttonWidgets.push_back(reportFileControl->getFileSelectionButton());
   buttonWidgets.push_back(leftTMapMetricShapeControl->getFileSelectionButton());
   buttonWidgets.push_back(rightTMapMetricShapeControl->getFileSelectionButton());
   buttonWidgets.push_back(leftShuffledTMapMetricShapeControl->getFileSelectionButton());
   buttonWidgets.push_back(rightShuffledTMapMetricShapeControl->getFileSelectionButton());
   QtUtilities::makeButtonsSameSize(buttonWidgets);
   
   //
   // arrange dialog items in a group widget
   //
   QGroupBox* w = new QGroupBox("Output Files");
   QVBoxLayout* entriesLayout = new QVBoxLayout(w);
   entriesLayout->addWidget(tMapShapeFileSelectionControl);
   entriesLayout->addWidget(shuffledTMapShapeFileSelectionControl);
   entriesLayout->addWidget(paintClustersFileSelectionControl);
   entriesLayout->addWidget(metricClustersFileSelectionControl);
   entriesLayout->addWidget(reportFileControl);
   entriesLayout->addWidget(leftTMapMetricShapeControl);
   entriesLayout->addWidget(rightTMapMetricShapeControl);
   entriesLayout->addWidget(leftShuffledTMapMetricShapeControl);
   entriesLayout->addWidget(rightShuffledTMapMetricShapeControl);
   entriesLayout->addStretch();
   
   return w;
}

/**
 * create the run algorithm widget.
 */
QWidget* 
GuiMetricShapeStatisticalAlgorithmDialog::createRunAlgorithmWidget()
{
   const QString text = "Press the <B>Run</B> button to begin executing\n"
                        "the statistical algorithm.";
   QLabel* label = new QLabel(text);
   
   //
   // Progress label
   //
   progressLabel = new QLabel("                                               ");
   
   //
   // Progress elapsed time label
   //
   progressElapsedTimeLabel = new QLabel("                                    ");
   
   //
   // Progress bar
   //
   progressBar = new QProgressBar;
   progressBar->setMinimum(0);
   progressBar->setMaximum(10);
   progressBar->setValue(0);

   //
   // Widget for page
   //
   QGroupBox* w = new QGroupBox("Run Algorithm");
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(label);
   layout->addWidget(new QLabel(" "));
   layout->addWidget(progressLabel);
   layout->addWidget(progressElapsedTimeLabel);
   layout->addWidget(progressBar);
   layout->addStretch();
   
   return w;
}
      
/**
 * updated by the algorithm.
 */
void 
GuiMetricShapeStatisticalAlgorithmDialog::slotDisplayProgressOnRunPage(const QString& text,
                                                                       const int progressNumber,
                                                                       const int totalNumberOfSteps)
{
   const int totalSeconds = runStartTime.elapsed() / 1000;
   const int numMinutes = totalSeconds / 60;
   const int numSeconds = totalSeconds - (numMinutes * 60);
   QString elapsedText("Execution Time: " + 
                       QString::number(numMinutes) +
                       " minutes " +
                       QString::number(numSeconds) +
                       " seconds.");
   
   progressLabel->setText(text);
   progressElapsedTimeLabel->setText(elapsedText);
   progressBar->setValue(progressNumber);
   progressBar->setMaximum(totalNumberOfSteps);
   qApp->processEvents();
}
                                        
/**
 * create the report widget.
 */
QWidget* 
GuiMetricShapeStatisticalAlgorithmDialog::createReportWidget()
{
   //
   // Text edit for report
   //
   reportTextEdit = new QTextEdit;

   //
   // Widget for page
   //
   QGroupBox* w = new QGroupBox("Report");
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(reportTextEdit);
   
   return w;
}

/**
 * create the variance smoothing options widget.
 */
QGroupBox* 
GuiMetricShapeStatisticalAlgorithmDialog::createVarianceSmoothingWidget()
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
   varianceSmoothingGroupBox = new QGroupBox("Variance Smoothing of Input Metric/Shape Files");
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
GuiMetricShapeStatisticalAlgorithmDialog::slotAreaCorrectionShapeFile(const QString& name)
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
GuiMetricShapeStatisticalAlgorithmDialog::updateDialog()
{
}

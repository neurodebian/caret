
#ifndef __GUI_METRIC_SHAPE_STATISTICAL_ALGORITHM_DIALOG_H__
#define __GUI_METRIC_SHAPE_STATISTICAL_ALGORITHM_DIALOG_H__

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

#include <map>

#include <QTime>

#include "BrainModelSurfaceMetricTwoSampleTTest.h"
#include "QtDialog.h"

class GuiFileSelectionButton;
class GuiFileSelectionButtonAndLineEditWidget;
class GuiFileSelectionListWidget;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QDoubleSpinBox;
class QProgressBar;
class QRadioButton;
class QSpinBox;
class QStackedWidget;
class QTextEdit;
class QtWidgetGroup;

// dialog for performing statistical algorithms on metric/shape files
class GuiMetricShapeStatisticalAlgorithmDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// algorithm mode
      enum ALGORITHM_MODE {
         /// no mode selected
         ALGORITHM_MODE_NONE,
         /// coordinate difference
         ALGORITHM_MODE_COORDINATE_DIFFERENCE,
         /// coordinate difference
         ALGORITHM_MODE_COORDINATE_DIFFERENCE_TMAP,
         /// interhemispheric clusters
         ALGORITHM_MODE_INTERHEMISPHERIC_CLUSTERS,
         /// one-sample t-test
         ALGORITHM_MODE_ONE_SAMPLE_T_TEST,
         /// paired t-test
         ALGORITHM_MODE_PAIRED_T_TEST,
         /// two-sample t-Test
         ALGORITHM_MODE_TWO_SAMPLE_T_TEST,
         /// wilcoxon rank into two-sample t-test
         ALGORITHM_MODE_WILCOXON_INTO_TWO_SAMPLE_T_TEST,
         /// number of algorithms
         ALGORITHM_MODE_COUNT
      };
       
      // constructor
      GuiMetricShapeStatisticalAlgorithmDialog(QWidget* parent);
      
      // destructor
      ~GuiMetricShapeStatisticalAlgorithmDialog();
      
      // update the dialog
      void updateDialog();
      
   protected slots:
      // called when area correction shape file is selected
      void slotAreaCorrectionShapeFile(const QString&);      
      
      // show the help page
      void slotShowHelpPage();
      
      // called when prev button pressed
      void slotPrevPushButton();
      
      // called when next button pressed
      void slotNextPushButton();
      
      // called when run button pressed
      void slotRunPushButton();
      
      // enable disable push button
      void slotEnableDisablePushButtons();
      
      // updated by the algorithm
      void slotDisplayProgressOnRunPage(const QString& text,
                                        const int progressNumber,
                                        const int totalNumberOfSteps);
                                        
   protected:
      /// filenames for the algorithms
      class AlgorithmFileNames {
         public:
            /// output metric/shape file (often T-Map)
            QString outputMetricShapeFileName;

            /// output shuffled metric/shape file (often Shuffled T-Map)
            QString outputMetricShapeShuffledTMapFileName;

            /// name of paint clusters file
            QString outputPaintClustersFileName;

            /// name of metric clusters file
            QString outputMetricClustersFileName;
            
            /// name of text report file
            QString outputReportFileName;

      };
      
      // show a page
      void showPage(QWidget* page);
      
      // create the algorithm widget
      QWidget* createAlgorithmWidget();
      
      // create the single files input widget
      QWidget* createSingleFilesInputWidget();
      
      // create the multiple files input widget
      QWidget* createMultipleFilesInputWidget();
      
      // create the surface files widget
      QWidget* createSurfaceFilesInputWidget();
      
      // create the cluster search files widget
      QWidget* createClusterSearchInputWidget();
      
      // create the output widget
      QWidget* createOutputWidget();
      
      // create the options widget.
      QWidget* createOptionsWidget();
      
      // create the threads widget
      QWidget* createThreadsWidget();
      
      // create the run algorithm widget
      QWidget* createRunAlgorithmWidget();
      
      // create the report widget
      QWidget* createReportWidget();
      
      // create the variance smoothing options widget
      QGroupBox* createVarianceSmoothingWidget();
      
      /// mode of algorithm
      ALGORITHM_MODE mode;
      
      /// the algorithm selection widget
      QWidget* pageAlgorithmSelection;
      
      /// the single input files widget
      QWidget* pageInputSingleFiles;
      
      /// the multiple input files widget
      QWidget* pageInputMultipleFiles;
      
      /// the input surface files widget
      QWidget* pageInputSurfaceFiles;
      
      /// the input cluster search widget
      QWidget* pageInputClusterSearch;
      
      /// the output files widget
      QWidget* pageOutputFiles;
      
      /// the options widget
      QWidget* pageOptions;
      
      /// the threads widget
      QWidget* pageThreads;
      
      /// the run algorithm widget
      QWidget* pageRunAlgorithm;
      
      /// the report widget
      QWidget* pageReport;
      
      /// validity of a page for the current mode
      std::map<QWidget*, bool> pageValidityFlag;

      /// input single files selection buttons and line edits
      std::vector<GuiFileSelectionButtonAndLineEditWidget*> inputFilesSingleSelectionControls;

      /// fiducial coord line Edit
      QLineEdit* fiducialCoordFileLineEdit;

      /// open topo line Edit
      QLineEdit* openTopoFileLineEdit;

      /// area correction shape file line Edit
      QLineEdit* areaCorrectionShapeFileLineEdit;

      /// area correction shape file column
      QComboBox* areaCorrectionShapeColumnComboBox;
      
      /// negative threshold spinbox
      QDoubleSpinBox* negThreshDoubleSpinBox;
            
      /// widget group for negative threshold
      QtWidgetGroup* negativeThresholdWidgetGroup;

      /// positive threshold spinbox
      QDoubleSpinBox* posThreshDoubleSpinBox;
      
      /// shuffled T-Map iteration spin box
      QSpinBox* shuffledTMapIterationSpinBox;
      
      /// left/right shuffled T-Map iteration spin box
      QSpinBox* leftRightShuffledTMapIterationSpinBox;
      
      /// p-value float spin box
      QDoubleSpinBox* pValueDoubleSpinBox;
      
      /// t-test constant
      QDoubleSpinBox* tTestConstantDoubleSpinBox;
      
      /// t-test constant widget group
      QtWidgetGroup* tTestConstantWidgetGroup;
      
      /// the paint file selection control
      GuiFileSelectionButtonAndLineEditWidget* paintClustersFileSelectionControl;
      
      /// the metric p-value file selection control
      GuiFileSelectionButtonAndLineEditWidget* metricClustersFileSelectionControl;
      
      /// report file selection control
      GuiFileSelectionButtonAndLineEditWidget* reportFileControl;
      
      /// report text edit
      QTextEdit* reportTextEdit;
      
      /// left T-Map metric/shape selection control
      GuiFileSelectionButtonAndLineEditWidget* leftTMapMetricShapeControl;
      
      /// right T-Map metric/shape selection control
      GuiFileSelectionButtonAndLineEditWidget* rightTMapMetricShapeControl;
      
      /// left Shuffled T-Map metric/shape selection control
      GuiFileSelectionButtonAndLineEditWidget* leftShuffledTMapMetricShapeControl;
      
      /// right Shuffled T-Map metric/shape selection control
      GuiFileSelectionButtonAndLineEditWidget* rightShuffledTMapMetricShapeControl;
      
      /// right and left hem T-Map widgets
      QtWidgetGroup* rightLeftTMapWidgetsGroup;
      
      /// the tab widget
      QStackedWidget* stackedWidget;
      
      /// tmap degrees of freedom check box
      QCheckBox* tMapDOFCheckBox;
      
      /// tmap p-value check box
      QCheckBox* tMapPValueCheckBox;
      
      /// line edit width
      int lineEditWidth;
      
      /// maximum width for numeric entries
      int numericMaxWidth;
      
      /// group box for variance smoothing options
      QGroupBox* varianceSmoothingGroupBox;
      
      /// variance smoothing iterations spin box
      QSpinBox* varianceSmoothingIterationsSpinBox;
      
      /// variance smoothing strength spin box
      QDoubleSpinBox* varianceSmoothingStrengthSpinBox;  
      
      /// radio button
      QRadioButton* algorithmCoordinateDifferenceRadioButton;
      
      /// radio button
      QRadioButton* algorithmCoordinateDifferenceTMapRadioButton;
      
      /// radio button
      QRadioButton* algorithmInterhemisphericClustersRadioButton;
      
      /// radio button
      QRadioButton* algorithmOneSampleTTestRadioButton;
      
      /// radio button
      QRadioButton* algorithmPairedTTestRadioButton;
      
      /// radio button
      QRadioButton* algorithmTwoSampleTTestRadioButton;
      
      /// radio button
      QRadioButton* algorithmWilcoxonRankSumRadioButton;
      
      /// prev pushbutton
      QPushButton* prevPushButton;
      
      /// next push button
      QPushButton* nextPushButton;
      
      /// run push button
      QPushButton* runPushButton;
      
      /// list widgets for groups of input files
      std::vector<GuiFileSelectionListWidget*> inputFilesGroupListWidgets;
      
      /// file filter for input files
      QStringList inputFileFilters;
      
      /// maximum number of groups for input files for display based upon mode
      int maximumNumberOfInputFileGroupsForDisplay;
      
      /// maximum number of single input files for display based upon mode
      int maximumNumberOfSingleInputFilesForDisplay;
      
      /// progress label
      QLabel* progressLabel;
      
      /// progress elapsed time label
      QLabel* progressElapsedTimeLabel;
      
      /// progress bar
      QProgressBar* progressBar;
      
      /// time running of algorithm was started
      QTime runStartTime;
      
      /// t-map file selection button and line edit
      GuiFileSelectionButtonAndLineEditWidget* tMapShapeFileSelectionControl;
      
      /// shuffled t-map shape file selection button and line edit
      GuiFileSelectionButtonAndLineEditWidget* shuffledTMapShapeFileSelectionControl;
      
      /// thread spin box
      QSpinBox* threadsSpinBox;      
      
      /// algorithm file names
      AlgorithmFileNames algorithmFileNames[ALGORITHM_MODE_COUNT];
};

#endif // __GUI_METRIC_SHAPE_STATISTICAL_ALGORITHM_DIALOG_H__


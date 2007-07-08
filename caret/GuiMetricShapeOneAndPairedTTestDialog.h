
#ifndef __GUI_METRIC_SHAPE_ONE_SAMPLE_T_TEST_DIALOG_H__
#define __GUI_METRIC_SHAPE_ONE_SAMPLE_T_TEST_DIALOG_H__

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

#include "QtDialogNonModal.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLineEdit;
class QSpinBox;
class QTabWidget;
class QTextEdit;

// dialog for finding significant clusters in metric/shape files
class GuiMetricShapeOneAndPairedTTestDialog : public QtDialogNonModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiMetricShapeOneAndPairedTTestDialog(QWidget* parent,
                                         const bool twoSampleFlagIn);
      
      // destructor
      ~GuiMetricShapeOneAndPairedTTestDialog();
      
      // update the dialog
      void updateDialog();
      
   protected slots:
      // called when user presses apply button
      void slotApplyButton();
      
      // called when area correction shape file is selected
      void slotAreaCorrectionShapeFile(const QString&);
      
      // called when shape/metric file one is selected
      void slotMetricShapeFileSelected(const QString&);
      
      // show the help page
      void slotShowHelpPage();
      
   protected:
      // create the input widget
      QWidget* createInputWidget();
      
      // create the output widget
      QWidget* createOutputWidget();
      
      // create the report widget
      QWidget* createReportWidget();
      
      // create the variance smoothing options widget
      QGroupBox* createVarianceSmoothingWidget();
      
      /// the report widget
      QWidget* reportWidget;
      
      /// metric file line Edit
      QLineEdit* metricFileLineEdit;

      /// metric file two line Edit (used by two-sample only)
      QLineEdit* metricFileTwoLineEdit;

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
      
      /// positive threshold spinbox
      QDoubleSpinBox* posThreshDoubleSpinBox;
      
      /// permuted T-Map iteration spin box
      QSpinBox* permutedTMapIterationSpinBox;
      
      /// p-value float spin box
      QDoubleSpinBox* pValueDoubleSpinBox;
      
      /// threads spin box
      QSpinBox* threadsSpinBox;
      
      /// t-map metric file line edit
      QLineEdit* tMapMetricFileLineEdit;
      
      /// permuted t-map metric file line edit
      QLineEdit* permutedTMapMetricFileLineEdit;
      
      /// the paint file name line edit
      QLineEdit* paintFileLineEdit;
      
      /// report file line edit
      QLineEdit* reportFileLineEdit;
      
      /// report text edit
      QTextEdit* reportTextEdit;
      
      /// the tab widget
      QTabWidget* tabby;
      
      /// t-test constant spin box
      QDoubleSpinBox* tTestConstantDoubleSpinBox;
      
      /// line edit width
      int lineEditWidth;
      
      /// maximum width for numeric entries
      int numericMaxWidth;
      
      /// paired T-Test flag
      bool pairedFlag;
      
      /// group box for variance smoothing options
      QGroupBox* varianceSmoothingGroupBox;
      
      /// variance smoothing iterations spin box
      QSpinBox* varianceSmoothingIterationsSpinBox;
      
      /// variance smoothing strength spin box
      QDoubleSpinBox* varianceSmoothingStrengthSpinBox;      
};

#endif // __GUI_METRIC_SHAPE_ONE_SAMPLE_T_TEST_DIALOG_H__


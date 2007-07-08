
#ifndef __GUI_SHAPE_INTERHEM_CLUSTERS_DIALOG_H__
#define __GUI_SHAPE_INTERHEM_CLUSTERS_DIALOG_H__

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
class QGroupBox;
class QLineEdit;
class QDoubleSpinBox;
class QSpinBox;
class QTabWidget;
class QTextEdit;

// dialog for finding inter-hemispheric clusters in metric and shape files
class GuiMetricShapeInterHemClustersDialog : public QtDialogNonModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiMetricShapeInterHemClustersDialog(QWidget* parent);
      
      // destructor
      ~GuiMetricShapeInterHemClustersDialog();
      
      // update the dialog
      void updateDialog();
      
   protected slots:
      // called when user presses apply button
      void slotApplyButton();
      
      // called when area correction shape file is selected
      void slotAreaCorrectionShapeFile(const QString&);
      
      // called when shape/metric file A is selected
      void slotMetricShapeFileASelected(const QString&);
      
      // show the help page
      void slotShowHelpPage();
      
   protected:
      // create the input widget
      QWidget* createInputWidget();
      
      // create the output widget
      QWidget* createOutputWidget();
      
      // create the options widget.
      QWidget* createOptionsWidget();
      
      // create the report widget
      QWidget* createReportWidget();
      
      // create the variance smoothing options widget
      QGroupBox* createVarianceSmoothingWidget();
      
      /// the report widget
      QWidget* reportWidget;
      
      /// shape file Left A line Edit
      QLineEdit* shapeFileLeftALineEdit;

      /// shape file Left B line Edit
      QLineEdit* shapeFileLeftBLineEdit;

      /// shape file Right A line Edit
      QLineEdit* shapeFileRightALineEdit;

      /// shape file Right B line Edit
      QLineEdit* shapeFileRightBLineEdit;

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
      
      /// left/right shuffled T-Map iteration spin box
      QSpinBox* leftRightShuffledTMapIterationSpinBox;
      
      /// shuffled T-Map iteration spin box
      QSpinBox* shuffledTMapIterationSpinBox;
      
      /// p-value float spin box
      QDoubleSpinBox* pValueDoubleSpinBox;
      
      /// Left t-map shape file line edit
      QLineEdit* leftTMapShapeFileLineEdit;
      
      /// Right t-map shape file line edit
      QLineEdit* rightTMapShapeFileLineEdit;
      
      /// t-map shape file line edit
      QLineEdit* tMapShapeFileLineEdit;
      
      /// left shuffled t-map shape file line edit
      QLineEdit* leftShuffledTMapShapeFileLineEdit;
      
      /// right shuffled t-map shape file line edit
      QLineEdit* rightShuffledTMapShapeFileLineEdit;
      
      /// shuffled t-map shape file line edit
      QLineEdit* shuffledTMapShapeFileLineEdit;
      
      /// the paint file name line edit
      QLineEdit* paintFileLineEdit;
      
      /// report file line edit
      QLineEdit* reportFileLineEdit;
      
      /// report text edit
      QTextEdit* reportTextEdit;
      
      /// the tab widget
      QTabWidget* tabby;
      
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
      
      /// thread spin box
      QSpinBox* threadsSpinBox;
};

#endif // __GUI_SHAPE_INTERHEM_CLUSTERS_DIALOG_H__


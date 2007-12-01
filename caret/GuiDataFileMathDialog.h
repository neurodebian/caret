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

#ifndef __GUI_METRIC_MATH_DIALOG_H__
#define __GUI_METRIC_MATH_DIALOG_H__

#include "QtDialog.h"

class GuiNodeAttributeColumnSelectionComboBox;
class GuiVolumeSelectionControl;
class MetricFile;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QDoubleSpinBox;

/// dialog for performing mathematical operations on metric files
class GuiDataFileMathDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// dialog mode
      enum DIALOG_MODE {
         DIALOG_MODE_METRIC_FILE,
         DIALOG_MODE_SURFACE_SHAPE_FILE,
         DIALOG_MODE_VOLUME_FILE
      };
      
      /// Constructor
      GuiDataFileMathDialog(QWidget* parent, 
                            const DIALOG_MODE modeIn);
      
      /// Constructor
      ~GuiDataFileMathDialog();
      
      /// update the dialog
      void updateDialog();
      
   protected slots:
      /// called when apply button pressed
      void slotApplyButton();
      
      /// called when output column selected
      void slotOutputMetricColumnComboBox();
      
      /// called when an operations radio button is selected
      void slotOperationsButtonGroup();
      
   protected:
      /// perform metric math operations
      void performMetricMathOperations(QString& errorMessage);
      
      /// perform volume math operations
      void performVolumeMathOperations(QString& errorMessage);
      
      /// update metric part of dialog
      void updateMetricPartOfDialog();

      /// update volume part of dialog
      void updateVolumePartOfDialog();
      
      /// create the metric column part of the dialog.
      QWidget* createMetricColumnPartOfDialog();

      /// create the volume part of the dialog
      QWidget* createVolumePartOfDialog();
      
      /// create the operations part of the dialog.
      QWidget* createOperationsPartOfDialog();
      
      /// get the data file since 
      MetricFile* getNodeDataFile();
      
      /// input column A
      GuiNodeAttributeColumnSelectionComboBox* inputMetricColumnComboBoxA;
      
      /// input column B
      GuiNodeAttributeColumnSelectionComboBox* inputMetricColumnComboBoxB;
      
      /// input colum B label
      QLabel* inputMetricColumnLabelB;
      
      /// output column
      GuiNodeAttributeColumnSelectionComboBox* outputMetricColumnComboBox;
      
      /// output metric name line edit
      QLineEdit* outputMetricNameLineEdit;
      
      /// combine columns radio button
      QRadioButton* combineColumnsRadioButton;
      
      /// add columns radio button
      QRadioButton* addColumnsRadioButton;
      
      /// subtract columns radio button
      QRadioButton* subtractColumnsRadioButton;
      
      /// multiply columns radio button
      QRadioButton* multiplyColumnsRadioButton;
      
      /// divide columns radio button
      QRadioButton* divideColumnsRadioButton;
      
      /// average columns radio button
      QRadioButton* averageColumnsRadioButton;
      
      /// multiply single column by scalar radio button
      QRadioButton* multiplyColumnByScalarRadioButton;
      
      /// add scalar to single column radio button
      QRadioButton* addScalarToColumnRadioButton;
      
      /// ceiling by scalar radio button
      QRadioButton* ceilingColumnByScalarRadioButton;
      
      /// floor by scalar radio button
      QRadioButton* floorColumnByScalarRadioButton;
      
      /// AND columns radio button
      QRadioButton* andColumnsRadioButton;
      
      /// OR columns radio button
      QRadioButton* orColumnsRadioButton;
      
      /// fix NaN radio button
      QRadioButton* fixNaNRadioButton;
      
      /// square root radio button
      QRadioButton* squareRootRadioButton;
      
      /// log radio button
      QRadioButton* logRadioButton;
      
      /// log base line edit
      QLineEdit* logBaseLineEdit;
      
      /// copy volume radio button
      QRadioButton* copyVolumeRadioButton;
      
      /// absolute value radio button
      QRadioButton* absValueRadioButton;
      
      /// multiply by scalar float spin box
      QDoubleSpinBox* multiplyScalarDoubleSpinBox;
      
      /// add by scalar float spin box
      QDoubleSpinBox* addScalarDoubleSpinBox;
      
      /// ceiling scalar float spin box
      QDoubleSpinBox* ceilingScalarDoubleSpinBox;
      
      /// floor scalar float spin box
      QDoubleSpinBox* floorScalarDoubleSpinBox;
      
      /// inclusive set range to zero radio button
      QRadioButton* inclusiveSetRangeToZeroRadioButton;
      
      /// inclusive set range to zero min value float spin box
      QDoubleSpinBox* inclusiveSetRangeToZeroMinValueDoubleSpinBox;
      
      /// inclusive set range to zero max value float spin box
      QDoubleSpinBox* inclusiveSetRangeToZeroMaxValueDoubleSpinBox;
      
      /// exclusive set range to zero radio button
      QRadioButton* exclusiveSetRangeToZeroRadioButton;
      
      /// exclusive set range to zero min value float spin box
      QDoubleSpinBox* exclusiveSetRangeToZeroMinValueDoubleSpinBox;
      
      /// exclusive set range to zero max value float spin box
      QDoubleSpinBox* exclusiveSetRangeToZeroMaxValueDoubleSpinBox;
      
      /// fit values to a normal distribution
      QRadioButton* normalizeValuesRadioButton;
      
      /// normalize mean flaot spin box
      QDoubleSpinBox* normalizeMeanDoubleSpinBox;
      
      /// normalize deviation float spin box
      QDoubleSpinBox* normalizeDeviationDoubleSpinBox;
      
      /// 1.0 - value check box
      QRadioButton* oneMinusValueRadioButton;
      
      /// the dialog's mode
      DIALOG_MODE dialogMode;
      
      /// volume A
      GuiVolumeSelectionControl* volumeASelectionControl;
      
      /// volume B
      GuiVolumeSelectionControl* volumeBSelectionControl;
      
      /// ouptut volume 
      GuiVolumeSelectionControl* volumeOutputSelectionControl;
      
      /// group box for volume B
      QGroupBox* volumeBGroupBox;
};

#endif // __GUI_METRIC_MATH_DIALOG_H__


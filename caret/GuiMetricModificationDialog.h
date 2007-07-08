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

#ifndef __GUI_METRIC_MODIFICATION_DIALOG_H__
#define __GUI_METRIC_MODIFICATION_DIALOG_H__

#include "QtDialog.h"

class QComboBox;
class QGroupBox;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QTabWidget;

class GuiBrainModelSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;
class MetricFile;
class QDoubleSpinBox;

/// dialog for modifying a metric column
class GuiMetricModificationDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// mode for working with metric or surface shape
      enum FILE_TYPE_MODE {
         FILE_TYPE_MODE_METRIC,
         FILE_TYPE_MODE_SURFACE_SHAPE
      };
      
      /// constructor
      GuiMetricModificationDialog(QWidget* parent, 
                                  const FILE_TYPE_MODE fileTypeModeIn);
      
      /// destructor
      ~GuiMetricModificationDialog();

      /// update the dialog
      void updateDialog();
           
   private slots:
      /// called when Apply button is pressed
      void slotApplyButton();

      /// called when help button is pressed
      void slotHelpButton();
      
      /// called when input metric column is changed
      void slotInputMetricColumnComboBox();
      
      /// called when output metric column is changed
      void slotOutputMetricColumnComboBox();
      
      /// Called when a smoothing algorithm is selected.
      void slotSmoothingAlgorithmComboBox(int item);

   private:
      /// type of file to operate upon
      FILE_TYPE_MODE fileTypeMode;

      /// get the data file since 
      MetricFile* getNodeDataFile();
      
      /// create the surface part of the dialog
      QWidget* createSurfacePartOfDialog();
      
      /// create the metric column part of the dialog
      QWidget* createMetricColumnPartOfDialog();
      
      /// create the smoothing part of the dialog
      QWidget* createSmoothingPartOfDialog();
      
      /// create the clustering part of the dialog
      QWidget* createClusteringPartOfDialog();
      
      /// smoothing iterations spin box
      QSpinBox* iterationsSpinBox;

      /// surface selection combo box
      GuiBrainModelSelectionComboBox* surfaceSelectionComboBox;

      /// input metric column selection combo box
      GuiNodeAttributeColumnSelectionComboBox* inputMetricColumnComboBox;

      /// output metric column selection combo box
      GuiNodeAttributeColumnSelectionComboBox* outputMetricColumnComboBox;

      /// smoothing strength float spin box
      QDoubleSpinBox* strengthDoubleSpinBox;
      
      /// output metric column name
      QLineEdit* outputMetricNameLineEdit;
      
      /// smoothing algorithm combo box
      QComboBox* smoothingAlgorithmComboBox;
      
      /// the modification type tab
      QTabWidget* modificationTypeTab;
      
      /// smoothing panel 
      QWidget* smoothingWidget;
      
      /// gaussian spherical surface group box
      QGroupBox* gaussSurfaceGroupBox;
      
      /// clustering panel
      QWidget* clusteringWidget;     
      
      /// cluster size any radio button
      QRadioButton* clusterSizeAnyRadioButton;
      
      /// cluster size number of nodes radio button
      QRadioButton* clusterSizeNumberOfNodesRadioButton;
      
      /// cluster size surface area radio button
      QRadioButton* clusterSizeMinimumSurfaceAreaRadioButton;
      
      /// cluster size minimum number of nodes
      QLineEdit* clusterSizeNumberOfNodesLineEdit;
      
      /// cluster size surface area
      QLineEdit* clusterSizeSurfaceAreaLineEdit;
      
      /// cluster pos maximum threshold line edit
      QLineEdit* clusterThresholdPosMaxLineEdit;

      /// cluster pos minimum threshold line edit
      QLineEdit* clusterThresholdPosMinLineEdit;

      /// cluster neg maximum threshold line edit
      QLineEdit* clusterThresholdNegMaxLineEdit;

      /// cluster neg minimum threshold line edit
      QLineEdit* clusterThresholdNegMinLineEdit;
      
      /// gaussian smoothing parameters grid
      QGroupBox* gaussSmoothParametersGroupBox;
      
      /// gaussian smoothing norm above cutoff
      QDoubleSpinBox* gaussSmoothNormAboveDoubleSpinBox;
      
      /// gaussian smoothing norm below cutoff
      QDoubleSpinBox* gaussSmoothNormBelowDoubleSpinBox;
      
      /// gaussian smoothing tangent cutoff
      QDoubleSpinBox* gaussSmoothTangentDoubleSpinBox;
      
      /// gaussian smoothing sigma norm
      QDoubleSpinBox* gaussSmoothSigmaNormDoubleSpinBox;
      
      /// gaussian smoothing sigma tang
      QDoubleSpinBox* gaussSmoothSigmaTangDoubleSpinBox;
      
      /// gaussian spherical surface combo box
      GuiBrainModelSelectionComboBox* gaussSphericalSurfaceComboBox;
};

#endif // __GUI_METRIC_MODIFICATION_DIALOG_H__


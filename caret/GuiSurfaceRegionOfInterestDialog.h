
#ifndef __GUI_SURFACE_REGION_OF_INTEREST_DIALOG_H__
#define __GUI_SURFACE_REGION_OF_INTEREST_DIALOG_H__

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

#include <vector>

#include "WuQWizard.h"

class BrainModelSurface;
class CoordinateFile;
class GuiBrainModelSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;
class GuiSurfaceROIOperationPage;
class GuiSurfaceROINodeSelectionPage;
class GuiSurfaceROIReportPage;
class GuiSurfaceROISurfaceAndTopologySelectionPage;
class GuiTopologyFileComboBox;
class MetricFile;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class QRadioButton;
class QStackedWidget;
class QTextEdit;

/// wizard dialog for surface region of interest operations
class GuiSurfaceRegionOfInterestDialog: public WuQWizard {
   Q_OBJECT
   
   public:
      // constructor
      GuiSurfaceRegionOfInterestDialog(QWidget* parent);
      
      // destructor
      ~GuiSurfaceRegionOfInterestDialog();
      
      // update the dialog (typically due to file changes)
      void updateDialog();
      
      // set border name for query
      void setBorderNameForQuery(const QString& borderName);
      
      // set paint index for query
      void setPaintIndexForQuery(const int paintIndex);
      
      // set metric node for query
      void setMetricNodeForQuery(const int metricNodeForQueryIn);
      
      // set shape node for query
      void setShapeNodeForQuery(const int shapeNodeForQueryIn);
      
      // set node for geodesic query
      void setNodeForGeodesicQuery(const int nodeNumber);
      
      // set linear border start node
      void setCreateLinearBorderStartNode(const int nodeNumber);
      
      // set linear border end node
      void setCreateLinearBorderEndNode(const int nodeNumber);

      // get the query paint column
      int getQueryPaintColumn() const;
      
      // get the operation coordinate file
      CoordinateFile* getOperationCoordinateFile();
      
      // get a COPY of operation surface with operation topology (user must delete object)
      BrainModelSurface* getCopyOfOperationSurface();
      
   public slots:
      // show the dialog
      void show();
      
      // called when dialog closed
      void reject();
      
   protected:
      // page identifiers
      enum PAGE_ID {
         // operation surface page identifier
         PAGE_ID_OPERATION_SURFACE_AND_TOPOLOGY,
         // node selection page identifier
         PAGE_ID_NODE_SELECTION,
         // operation page
         PAGE_ID_OPERATION,
         // report page
         PAGE_ID_REPORT
      };
      
      // node selection page
      GuiSurfaceROINodeSelectionPage* roiNodeSelectionPage;
      
      // surface selection page
      GuiSurfaceROISurfaceAndTopologySelectionPage* surfaceSelectionPage;
      
      // operation page
      GuiSurfaceROIOperationPage* operationPage;
      
      // report page
      GuiSurfaceROIReportPage* reportPage;
};

/// surface region of interest report page
class GuiSurfaceROIReportPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiSurfaceROIReportPage(GuiSurfaceRegionOfInterestDialog* roiDialogIn);
   
      // destructor
      ~GuiSurfaceROIReportPage();
      
      // clean up the page
      void cleanupPage();
      
      // initialize the page
      void initializePage();
      
      // validate the page
      bool validatePage();
      
      // is page complete
      bool isComplete();
   
      // update the page
      void updatePage();
      
      // set the report page's text edit
      void setReportText(const QString& S);
      
   protected slots:
      // called when save report to text file button pressed
      void slotSaveReportToTextFile();
      
      // called when clear report button pressed
      void slotClearReportTextEdit();
      
   protected:
      /// parent ROI dialog
      GuiSurfaceRegionOfInterestDialog* roiDialog;
      
      // the report text edit
      QTextEdit* reportTextEdit;
      
};

/// surface region of interest operation page
class GuiSurfaceROIOperationPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiSurfaceROIOperationPage(GuiSurfaceRegionOfInterestDialog* roiDialogIn,
                                 GuiSurfaceROIReportPage* reportPageIn);
   
      // destructor
      ~GuiSurfaceROIOperationPage();
      
      // clean up the page
      void cleanupPage();
      
      // initialize the page
      void initializePage();
      
      // validate the page
      bool validatePage();
      
      // is page complete
      bool isComplete();
   
      // update the page
      void updatePage();
      
   protected slots:
      // called when an operation is selected
      void slotOperationSelectionComboBox(int);
      
      // called to assign metric
      void slotAssignMetricPushButton();
      
      // called to assign node areas to metric
      void slotNodeAreaMetricPushButton();
      
      // called to assign paint
      void slotAssignPaintPushButton();
      
      // called to assign shape
      void slotAssignShapePushButton();
      
      /// called to run compute integrated folding index
      void slotComputeIntegratedFoldingIndex();
      
      /// called to run the folding measurements
      void slotFoldingMeasurements();
      
      /// Called to set create borders from clusters name.
      void slotArealBorderClusterNamePushButton();

      /// Called run create borders from clusters.
      void slotCreateArealBordersFromClusters();

      /// Called to set linear border name
      void slotLinearCreateBorderFromROINamePushButton();
      
      /// called to set linear border start node
      void slotCreateLinearBorderFromROIStartNodePushButton();
      
      /// called to set linear border end node
      void slotCreateLinearBorderFromROIEndNodePushButton();
      
      /// called to create the linear border
      void slotCreateLinearBorderFromROIPushButton();

      /// called to create a volume ROI from ROI nodes
      void slotCreateVolumeFromQueryNodesButton();
      
      /// called to disconnect node in ROI
      void slotDisconnectNodes();
      
      /// called to update geodesic column names
      void slotGeodesicUpdateColumnNames();
      
      /// called when geodesic choose node with mouse button pressed
      void slotGeodesicNodePushButton();
      
      /// called to create geodesic
      void slotGeodesicPushButton();
      
      /// called to generate prob atlas overlap report
      void slotCreateProbAtlasOverlapReport();
      
      /// called to smooth nodes
      void slotSmoothNodesPushButtonPressed();
      
      /// called to create paint region report
      void slotCreatePaintReportButton();
      
      /// called to create statistical report
      void slotCreateStatisticalReportButton();
      
      /// called to create shape correleation report
      void slotCorrelationShapeReportPushButton();
      
      /// called to create shape cluster report
      void slotShapeClusterReportPushButton();
      
      /// called to create metric cluster report
      void slotMetricClusterReportPushButton();
      
   protected:
      /// operation mode
      enum OPERATION_MODE {
         OPERATION_MODE_ASSIGN_METRIC,
         OPERATION_MODE_ASSIGN_METRIC_NODE_AREAS,
         OPERATION_MODE_ASSIGN_PAINT,
         OPERATION_MODE_ASSIGN_SURFACE_SHAPE,
         OPERATION_MODE_COMPUTE_INTEGRATED_FOLDING_INDEX,
         OPERATION_MODE_CREATE_BORDERS_FROM_CLUSTERS,
         OPERATION_MODE_CREATE_BORDERS_FROM_ROI,
         OPERATION_MODE_CREATE_VOLUME_ROI,
         OPERATION_MODE_DISCONNECT_NODES,
         OPERATION_MODE_FOLDING_MEASUREMENTS,
         OPERATION_MODE_GEODESIC,
         OPERATION_MODE_METRIC_CLUSTER_ANALYSIS,
         OPERATION_MODE_PROB_ATLAS_OVERLAP,
         OPERATION_MODE_SMOOTH_NODES,
         OPERATION_MODE_STATISTICAL_PAINT_REPORT,
         OPERATION_MODE_STATISTICAL_REPORT,
         OPERATION_MODE_SHAPE_CORRELATION,
         OPERATION_MODE_SHAPE_CLUSTER_ANALYSIS
      };
      
      // create the assign metric page
      QWidget* createAssignMetricPage();
      
      // create the assign paint page
      QWidget* createAssignPaintPage();
      
      // create the assign shape page
      QWidget* createAssignShapePage();
      
      // create the compute integrated folding index page
      QWidget* createComputeIntegratedFoldingIndexPage();
      
      // create the folding measurements page
      QWidget* createFoldingMeasurementsPage();
      
      // create the create borders from clusters page
      QWidget* createCreateArealBordersPage();
      
      // create the create linear borders page
      QWidget* createLinearBordersPage();
      
      // create the create volume roi page
      QWidget* createCreateVolumeROIPage();
      
      // create the disconnect nodes page
      QWidget* createDisconnectNodesPage();
      
      // create the geodesic page
      QWidget* createGeodesicPage();
      
      // create the metric cluster analysis page
      QWidget* createMetricClusterAnalysisPage();
      
      // create the probabilistic atlas overlap page
      QWidget* createProbAtlasOverlapPage();
      
      // create the smoothing page
      QWidget* createSmoothingPage();
      
      // create the statistical paint report page
      QWidget* createPaintReportPage();
      
      // create the statistical report page
      QWidget* createStatisticalReportPage();
      
      // create the shape correlation page
      QWidget* createShapeCorrelationPage();
      
      // create the shape cluster analysis page
      QWidget* createShapeClusterAnalysisPage();
      
      // create the node areas page
      QWidget* createNodeAreasPage();
      
      // run a metric or shape cluster report
      void runMetricShapeClusterAnalysis(MetricFile* metricShapeFile,
                                         MetricFile* distortionMetricFile,
                                         const int distortionMetricColumn,
                                         const std::vector<bool>& metricShapeColumnsSelected,
                                         const float thresholdValue,
                                   const bool separateReportWithSemicolonsFlag);
                                         
      /// parent ROI dialog
      GuiSurfaceRegionOfInterestDialog* roiDialog;
     
      /// the report page
      GuiSurfaceROIReportPage* reportPage;
      
      /// operation selection combo box
      QComboBox* operationSelectionComboBox;
      
      /// stacked widget for operations
      QStackedWidget* operationsStackedWidget;

      /// the assign metric page
      QWidget* assignMetricPage;
      
      /// the assign node areas page
      QWidget* nodeAreaMetricPage;
      
      /// the assign paint page
      QWidget* assignPaintPage;
      
      /// the assign shape page
      QWidget* assignShapePage;
      
      /// the compute integrated folding index page
      QWidget* computeIntegratedFoldingIndexPage;
      
      /// the create borders from clusters page
      QWidget* createArealBordersPage;
      
      /// the folding measurements page
      QWidget* foldingMeasurementsPage;
      
      /// the create linear borders page
      QWidget* linearBordersPage;
      
      /// the create volume roi page
      QWidget* createVolumeROIPage;
      
      /// the disconnect nodes page
      QWidget* disconnectNodesPage;
      
      /// the geodesic page
      QWidget* geodesicPage;
      
      /// the metric cluster analysis page
      QWidget* metricClusterAnalysisPage;
      
      /// the probabilistic atlas overlap page
      QWidget* probAtlasOverlapPage;
      
      /// the smoothing page
      QWidget* smoothingPage;
      
      /// the statistical paint report page
      QWidget* paintReportPage;
      
      /// the statistical report page
      QWidget* statisticalReportPage;
      
      /// the shape correlation page
      QWidget* shapeCorrelationPage;
      
      /// the shape cluster analysis page
      QWidget* shapeClusterAnalysisPage;
      
      /// assign metric column selection
      GuiNodeAttributeColumnSelectionComboBox* assignMetricColumnSelectionComboBox;
      
      /// assign metric column name line edit
      QLineEdit* assignMetricColumnNameLineEdit;
      
      /// assign metric value double spin box
      QDoubleSpinBox* assignMetricValueDoubleSpinBox;

      /// node area column selection
      GuiNodeAttributeColumnSelectionComboBox* nodeAreaMetricColumnSelectionComboBox;
      
      /// node area column name line edit
      QLineEdit* nodeAreaMetricColumnNameLineEdit;
      
      /// node area percentage check box
      QCheckBox* nodeAreaPercentageCheckBox;
      
      /// assign paint column selection
      GuiNodeAttributeColumnSelectionComboBox* assignPaintColumnSelectionComboBox;
      
      /// assign paint column name line edit
      QLineEdit* assignPaintColumnNameLineEdit;
      
      /// assign paint name line edit
      QLineEdit* assignPaintNameLineEdit;

      /// assign shape column selection
      GuiNodeAttributeColumnSelectionComboBox* assignShapeColumnSelectionComboBox;
      
      /// assign shape column name line edit
      QLineEdit* assignShapeColumnNameLineEdit;
      
      /// assign shape value double spin box
      QDoubleSpinBox* assignShapeValueDoubleSpinBox;
      
      /// semicolon separet integrated folding index report
      QCheckBox* semicolonSeparateFoldingIndexReportCheckBox;
      
      /// areal border name line edit
      QLineEdit* clusterArealBorderNameLineEdit;
      
      /// areal border auto project check box
      QCheckBox* clusterArealBorderAutoProjectCheckBox;
      
      /// linear border name line edit
      QLineEdit* createLinearBorderFromROINameLineEdit;
      
      /// linear border sampling density double spin box
      QDoubleSpinBox* createLinearBorderFromROISamplingDensityDoubleSpinBox;
      
      /// linear border automatic radio button
      QRadioButton* createLinearBorderFromROIAutomaticRadioButton;
      
      /// linear border manual radio button
      QRadioButton* createLinearBorderFromROIManualRadioButton;
      
      /// linear border start node spin box
      QSpinBox* createLinearBorderFromROIStartNodeSpinBox;
      
      /// linear border end node spin box
      QSpinBox* createLinearBorderFromROIEndNodeSpinBox;
      
      /// geodesic node spin box
      QSpinBox* geodesicNodeSpinBox;
      
      /// geodesic's metric column selection
      GuiNodeAttributeColumnSelectionComboBox* geodesicMetricColumnComboBox;
      
      /// geodesic's metric column name line edit
      QLineEdit* geodesicMetricColumnNameLineEdit;
      
      /// geodesic's geodesic column selection
      GuiNodeAttributeColumnSelectionComboBox* geodesicDistanceColumnComboBox;
      
      /// geodesic's geodesic column name line edit
      QLineEdit* geodesicDistanceColumnNameLineEdit;
      
      /// prob atlas overlap separate with semicolons check box
      QCheckBox* probAtlasOverlapSemicolonSeparateCheckBox;
      
      /// paint region report's paint column selection combo box
      GuiNodeAttributeColumnSelectionComboBox* paintRegionReportColumnComboBox;
      
      /// paint report metric column areal distortion
      GuiNodeAttributeColumnSelectionComboBox* paintRegionReportDistortionCorrectionMetricColumnComboBox;
      
      /// paint region report separate with semicolons
      QCheckBox* paintReportSeparateWithSemicolonsCheckBox;
      
      /// statistical report metric column areal distortion
      GuiNodeAttributeColumnSelectionComboBox* statisticalReportDistortionCorrectionMetricColumnComboBox;
      
      /// statistical report separate with semicolons
      QCheckBox* statisticalReportSeparateWithSemicolonsCheckBox;
      
      /// shape correlation combo box
      GuiNodeAttributeColumnSelectionComboBox* shapeCorrelationColumnComboBox;
      
      /// shape correlation report separate with semicolons
      QCheckBox* shapeCorrelationSeparateWithSemicolonsCheckBox;
      
      /// shape cluster metric distortion column combo box
      GuiNodeAttributeColumnSelectionComboBox* shapeClusterMetricArealDistortionComboBox;
      
      /// shape cluster threshold double spin box
      QDoubleSpinBox* shapeClusterThresholdDoubleSpinBox;
      
      /// shape cluster report separate with semicolons
      QCheckBox* shapeClusterSeparateWithSemicolonsCheckBox;
      
      /// metric cluster metric distortion column combo box
      GuiNodeAttributeColumnSelectionComboBox* metricClusterMetricArealDistortionComboBox;
      
      /// metric cluster threshold double spin box
      QDoubleSpinBox* metricClusterThresholdDoubleSpinBox;
      
      /// metric cluster report separate with semicolons
      QCheckBox* metricClusterSeparateWithSemicolonsCheckBox;
      
     QCheckBox* foldingMeasurementsMetricFileCheckBox;
     QLineEdit* foldingMeasurementsMetricFileNameLineEdit;

   friend class GuiSurfaceRegionOfInterestDialog;
};

/// surface region of interest surface topology selection page
class GuiSurfaceROISurfaceAndTopologySelectionPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiSurfaceROISurfaceAndTopologySelectionPage(GuiSurfaceRegionOfInterestDialog* roiDialogIn);
   
      // destructor
      ~GuiSurfaceROISurfaceAndTopologySelectionPage();
      
      // clean up the page
      void cleanupPage();
      
      // initialize the page
      void initializePage();
      
      // validate the page
      bool validatePage();
      
      // is page complete
      bool isComplete();
   
      // update the page
      void updatePage();
      
      // get the operation coordinate file
      CoordinateFile* getOperationCoordinateFile();
      
      // get a COPY of operation surface with operation topology (user must delete object)
      BrainModelSurface* getCopyOfOperationSurface();
      
   protected slots:
      // called when a surface is selected
      void slotOperationSurfaceSelectionComboBox();
      
      // called when topology is selected
      void slotOperationTopologyComboBox();
      
   protected:
      // parent ROI dialog
      GuiSurfaceRegionOfInterestDialog* roiDialog;
     
      /// operation surface selection control
      GuiBrainModelSelectionComboBox* operationSurfaceSelectionComboBox;
     
      /// operation topology selection control
      GuiTopologyFileComboBox* operationTopologyComboBox;
};

/// surface region of interest node selection page
class GuiSurfaceROINodeSelectionPage : public QWizardPage {
   Q_OBJECT 
   
   public:
      // constructor
      GuiSurfaceROINodeSelectionPage(GuiSurfaceRegionOfInterestDialog* roiDialogIn);
   
      // destructor
      ~GuiSurfaceROINodeSelectionPage();
      
      // clean up the page
      void cleanupPage();
      
      // initialize the page
      void initializePage();
      
      // validate the page
      bool validatePage();
      
      // is page complete
      bool isComplete();
   
      // update the page
      void updatePage();
      
   protected slots:
      // called when a selection source is made
      void slotSelectionSourceComboBox(int);
      
      // called when select nodes push button pressed
      void slotSelectedNodesPushButton();
      
      // called when delselect nodes push button pressed
      void slotDeselectNodesPushButton();
      
      // called when invert push button pressed
      void slotInvertNodesPushButton();
      
      // called when load ROI push button pressed
      void slotLoadROIPushButton();
      
      // called when save ROI push button pressed
      void slotSaveROIPushButton();
      
      // called when dilate ROI push button pressed
      void slotDilateROIPushButton();
      
      // called when erode ROI push button pressed
      void slotErodeROIPushButton();
      
      // called when extent push button pressed
      void slotExtentPushButton();
      
      // called when remove islands button pressed
      void slotRemoveIslands();
      
      // called when paint name from list pushbutton pressed
      void slotSelectPaintNameFromListPushButton();
      
      // called when paint select node with mouse pushbutton pressed
      void slotSelectPaintNameNodeWithMousePushButton();
      
      // called when border name from list pushbutton pressed
      void slotSelectBorderNameFromListPushButton();
      
      // called when border select with mouse pushbutton pressed
      void slotSelectBorderNameWithMousePushButton();
      
      // called when metric threshold radio button clicked
      void slotMetricSelectionButtonGroup();
      
      // called when shape threshold radio button clicked
      void slotShapeSelectionButtonGroup();
      
      // update selected number of nodes label
      void updateNumberOfSelectedNodesLabel();
      
      // called when show selected nodes checkbox toggled
      void slotShowSelectedNodesCheckBox(bool);
      
   protected:
      /// selection source (source of node selection)
      enum SELECTION_SOURCE {
         SELECTION_SOURCE_ALL_NODES,
         SELECTION_SOURCE_NODES_WITHIN_BORDER,
         SELECTION_SOURCE_NODES_WITH_CROSSOVERS,
         SELECTION_SOURCE_NODES_WITH_EDGES,
         SELECTION_SOURCE_NODES_WITHIN_LATLON,
         SELECTION_SOURCE_NODES_WITH_METRIC,
         SELECTION_SOURCE_NODES_WITH_PAINT,
         SELECTION_SOURCE_NODES_WITH_SHAPE
      };
      
     // parent ROI dialog
     GuiSurfaceRegionOfInterestDialog* roiDialog;
     
     // create the node selection source options widget
     QStackedWidget* createNodeSelectionSourceOptionsWidget();
     
     /// selection source combo box
     QComboBox* selectionSourceComboBox;
     
     /// selection logic combo box
     QComboBox* selectionLogicComboBox;
     
     /// number of nodes selected label
     QLabel* numberOfNodesSelectedLabel;
     
     /// show selected nodes check box
     QCheckBox* showSelectedNodesCheckBox;
     
     /// deselect nodes push button
     QPushButton* deselectNodesPushButton;
     
     /// dilate nodes push button
     QPushButton* dilateSelectedNodesPushButton;
     
     /// erode nodes push button
     QPushButton* erodeSelectedNodesPushButton;
     
     /// extend nodes push button
     QPushButton* extentPushButton;
     
     /// invert nodes push button
     QPushButton* invertSelectedNodesPushButton;
     
     /// remove islands nodes push button
     QPushButton* removeIslandsPushButton;
     
     /// load ROI push button
     QPushButton* loadROIPushButton;
     
     /// save ROI push button
     QPushButton* saveROIPushButton;
     
     /// selection options stacked widget
     QStackedWidget* selectionSourceOptionsStackedWidget;
     
     /// selection options for all nodes
     QWidget* selectionSourceOptionsAllNodesWidget;
     
     /// selection options for borders
     QWidget* selectionSourceOptionsBordersWidget;
     
     /// border flat surface selection control
     GuiBrainModelSelectionComboBox* borderFlatSurfaceSelectionComboBox;
     
     /// border name selection label
     QLabel* borderNameSelectionLabel;
     
     /// selection options for crossovers
     QWidget* selectionSourceOptionsCrossoversWidget;
     
     /// selection options for edges
     QWidget* selectionSourceOptionsEdgesWidget;
     
     /// selection options for lat/long
     QWidget* selectionSourceOptionsLatLongWidget;
     
     /// lat min double spin box
     QDoubleSpinBox* latLowerRangeDoubleSpinBox;
      
     /// lat max double spin box
     QDoubleSpinBox* latUpperRangeDoubleSpinBox;
   
     /// lon min double spin box
     QDoubleSpinBox* lonLowerRangeDoubleSpinBox;
   
     /// lon min double spin box
     QDoubleSpinBox* lonUpperRangeDoubleSpinBox;
      
     /// selection options for metric
     QWidget* selectionSourceOptionsMetricWidget;
     
     /// metric column selection combo box
     GuiNodeAttributeColumnSelectionComboBox* metricColumnSelectionComboBox;
     
     /// metric range lower double spin box;
     QDoubleSpinBox* metricSelectionLowerDoubleSpinBox;
     
     /// metric range upper double spin box;
     QDoubleSpinBox* metricSelectionUpperDoubleSpinBox;
     
     /// metric selection all nodes radio button
     QRadioButton* metricSelectionAllNodesRadioButton;
     
     /// metric selection connect to node with mouse radio button
     QRadioButton* metricSelectionConnectedToNodeRadioButton;
     
     /// metric selection node number label
     QLabel* metricSelectionNodeNumberLabel;
     
     /// metric selection node value label
     QLabel* metricSelectionNodeValueLabel;
     
     /// selection options for paint
     QWidget* selectionSourceOptionsPaintWidget;
     
     /// paint column selection combo box
     GuiNodeAttributeColumnSelectionComboBox* paintColumnSelectionComboBox;
     
     /// paint name selection label
     QLabel* paintNameSelectionLabel;
     
     /// selection options for shape
     QWidget* selectionSourceOptionsShapeWidget;
     
     /// shape column selection combo box
     GuiNodeAttributeColumnSelectionComboBox* shapeColumnSelectionComboBox;
     
     /// shape range lower double spin box;
     QDoubleSpinBox* shapeSelectionLowerDoubleSpinBox;
     
     /// shape range upper double spin box;
     QDoubleSpinBox* shapeSelectionUpperDoubleSpinBox;
     
     /// shape selection all nodes radio button
     QRadioButton* shapeSelectionAllNodesRadioButton;
     
     /// shape selection connect to node with mouse radio button
     QRadioButton* shapeSelectionConnectedToNodeRadioButton;
     
     /// shape selection node number label
     QLabel* shapeSelectionNodeNumberLabel;
     
     /// shape selection node value label
     QLabel* shapeSelectionNodeValueLabel;
     
   friend class GuiSurfaceRegionOfInterestDialog;
};

#endif // __GUI_SURFACE_REGION_OF_INTEREST_DIALOG_H__

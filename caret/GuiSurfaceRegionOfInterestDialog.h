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


#ifndef __VE_GUI_SURFACE_REGION_OF_INTEREST_DIALOG_H__
#define __VE_GUI_SURFACE_REGION_OF_INTEREST_DIALOG_H__

#include <QString>
#include <vector>

#include "QtDialog.h"

class BrainModelSurface;
class GuiBrainModelSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;
class GuiPaintColumnNamesComboBox;
//class GuiTopologyFileComboBox;
class GiftiNodeDataFile;
class NodeAttributeFile;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QStackedWidget;
class QTextEdit;
class QDoubleSpinBox;
class QTabWidget;
class QVBoxLayout;
class TopologyFile;

/// Dialog for performing surface region of interest queries 
class GuiSurfaceRegionOfInterestDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiSurfaceRegionOfInterestDialog(QWidget* parent);
      
      /// Destructor
      ~GuiSurfaceRegionOfInterestDialog();
      
      /// get the query paint column
      int getQueryPaintColumn() const;
      
      /// update the dialog (typically due to file changes)
      void updateDialog();
      
      /// set border name for query
      void setBorderNameForQuery(const QString& borderName);
      
      /// set paint index for query
      void setPaintIndexForQuery(const int paintIndex);
      
      /// set metric node for query
      void setMetricNodeForQuery(const int metricNodeForQueryIn);
      
      /// set shape node for query
      void setShapeNodeForQuery(const int shapeNodeForQueryIn);
      
      /// set node for geodesic query
      void setNodeForGeodesicQuery(const int nodeNumber);
      
      /// set open border start node
      void setCreateBorderOpenStartNode(const int nodeNumber);
      
      /// set open border end node
      void setCreateBorderOpenEndNode(const int nodeNumber);
      
   private slots:
      /// called when dialog closed
      void close();
      
      /// called when help button is pressed
      void slotHelpButton();
      
      /// Called to assign paint to nodes
      void slotAssignPaintToNodes();
      
      /// called when an assign paint column name column selected
      void slotAssignPaintColumn(int item);
      
      /// called when create volume from displayed query nodes push button pressed
      void slotCreateVolumeFromQueryNodesButton();
      
      /// called when select nodes button pressed
      void slotSelectNodesButton();
      
      /// called when deselect nodes button pressed
      void slotDeselectNodesButton();
      
      /// called when create report button pressed
      void slotCreateReportButton();
      
      /// called when create paint subregion report button pressed
      void slotCreatePaintReportButton();
      
      /// called when disconnect nodes button is pressed
      void slotDisconnectNodes();
      
      ///  called when border name from list push button pressed
      void slotBorderNameFromListPushButton();
      
      ///  called when border name from mouse push button pressed
      void slotBorderNameFromMousePushButton();
      
      /// called when paint name from list push button pressed
      void slotPaintNameFromListPushButton();
      
      /// called when paint name from mouse push button pressed
      void slotPaintNameFromMousePushButton();
      
      /// called when report save push button pressed
      void slotSavePushButton();
      
      /// called when report clear push button pressed
      void slotClearPushButton();
      
      ///  Called to change the mouse mode to select the node for the geodesic query.
      void slotGeodesicNodePushButton();

      /// called when geodesic push button pressed
      void slotGeodesicPushButton();
      
      /// called to default geodesic column names
      void slotUpdateGeodesicColumnNames();
      
      /// called when a metric mode is selected
      void slotMetricModeSelection(int metricMode);
      
      /// called when an operation mode is selected
      void slotOperationMode(int item);
      
      /// called when the selection logic is changed
      void slotSelectionLogic(int item);
      
      /// called when one of the query mode radio buttons is selected
      void slotSelectionMode(int);
      
      /// called when show selected nodes toggle is changed
      void slotShowSelectedNodes(bool on);
      
      /// update the operation topology label
      void slotSurfaceSelection();
      
      /// called when topology is changed
      //void slotTopologySelection();
      
      /// called to display smoothing dialog
      void slotSmoothNodes();
      
      /// Called to create shape correlation report.
      void slotCorrelationShapeReport();

      /// Called when metric all on pushbutton is pressed
      void slotMetricAllOnPushButton();
      
      /// Called when metric all off pushbutton is pressed
      void slotMetricAllOffPushButton();
      
      /// Called when paint all on pushbutton is pressed
      void slotPaintAllOnPushButton();
      
      /// Called when paint all off pushbutton is pressed
      void slotPaintAllOffPushButton();
      
      /// Called when shape all on pushbutton is pressed
      void slotShapeAllOnPushButton();
      
      /// Called when shape all off pushbutton is pressed
      void slotShapeAllOffPushButton();
      
      /// Called when a shape mode is selected.
      void slotShapeModeSelection(int shapeMode);

      /// Called to assign metrics for a node
      void slotAssignMetricToNodes();
      
      /// Called to assign surface shape for a node
      void slotAssignSurfaceShapeToNodes();
      
      /// Called to create the prob atlas report
      void slotCreateProbAtlasReport();
      
      /// Called to create shape cluster report
      void slotShapeClusterReport();
      
      /// Called to set create borders from clusters name
      void slotBorderClusterNamePushButton();
      
      /// Called run create borders from clusters
      void slotCreateBordersFromClusters();
      
      /// called to run compute integrated folding index
      void slotComputeIntegratedFoldingIndex();
      
      /// called to create a border from the ROI
      void slotCreateBorderFromROIPushButton();
      
      /// called to set name of border for border from ROI
      void slotCreateBorderFromROINamePushButton();
      
      /// called to select start node for border from ROI
      void slotCreateBorderFromROIStartNodePushButton();
      
      /// called to select start node for border from ROI
      void slotCreateBorderFromROIEndNodePushButton();
      
   private:
   
      /// operation mode
      enum OPERATION_MODE {
         OPERATION_MODE_ASSIGN_METRIC,
         OPERATION_MODE_ASSIGN_PAINT,
         OPERATION_MODE_ASSIGN_SURFACE_SHAPE,
         OPERATION_MODE_COMPUTE_INTEGRATED_FOLDING_INDEX,
         OPERATION_MODE_CREATE_BORDERS_FROM_CLUSTERS,
         OPERATION_MODE_CREATE_BORDERS_FROM_ROI,
         OPERATION_MODE_CREATE_VOLUME_ROI,
         OPERATION_MODE_DISCONNECT_NODES,
         OPERATION_MODE_GEODESIC,
         OPERATION_MODE_PROB_ATLAS_OVERLAP,
         OPERATION_MODE_SMOOTH_NODES,
         OPERATION_MODE_STATISTICAL_REPORT,
         OPERATION_MODE_STATISTICAL_PAINT_REPORT,
         OPERATION_MODE_SHAPE_CORRELATION,
         OPERATION_MODE_SHAPE_CLUSTER_ANALYSIS
      };
      
      /// logical selection (and, or, etc)
      enum SELECTION_LOGIC {
         SELECTION_LOGIC_NORMAL,
         SELECTION_LOGIC_AND,
         SELECTION_LOGIC_OR,
         SELECTION_LOGIC_NOT,
         SELECTION_LOGIC_AND_NOT
      };
      
      /// selction mode (type of attribute used to select)
      enum SELECTION_MODE {
         SELECTION_MODE_ENTIRE_SURFACE,
         SELECTION_MODE_NODES_WITH_PAINT,
         SELECTION_MODE_NODES_WITHIN_BORDER,
         SELECTION_MODE_NODES_WITH_METRIC,
         SELECTION_MODE_NODES_WITH_SHAPE,
         SELECTION_MODE_NODES_WITH_CROSSOVERS,
         SELECTION_MODE_NONE
      };
      
      /// the selection logic
      SELECTION_LOGIC selectionLogic;
      
      /// the query mode
      SELECTION_MODE selectionMode;
      
      /// create the node selection section
      QWidget* createNodeSelectionSection();

      /// create the node selection with paint section
      void createNodeSelectionPaint();
      
      /// create the node selection with border section
      void createNodeSelectionBorder();
      
      /// create the node selection with metric
      void createNodeSelectionMetric();
      
      /// create the node selection with shape
      void createNodeSelectionShape();
      
      /// create the assign metric operation section
      void createOperationAssignMetric();
      
      /// create the assign paint operation section
      void createOperationAssignPaint();
      
      /// create the prob atlas analysis operation section
      void createOperationProbAtlas();
      
      /// create the assign surface shape operation section
      void createOperationAssignSurfaceShape();
      
      /// create the borders around clusters section
      void createOperationsBordersAroundClusters();
      
      /// create the border from ROI
      void createOperationsBordersFromROI();
      
      /// create the create volume ROI operation section
      void createOperationCreateVolumeROI();
      
      /// create the disconnect nodes operation section
      void createOperationDisconnectNodes();
      
      /// create the geodesic operation section
      void createOperationGeodesicDistance();
      
      /// create the integrated folding index operation
      void createOperationIntegratedFoldingIndex();
      
      /// create the statistical report operation section
      void createOperationStatisticalReport();
      
      /// create the statistical report on paint subregion operation section
      void createOperationStatisticalPaintReport();
      
      /// create the smoothnodes operation section
      void createOperationSmoothNodes();
      
      /// create the surface shape correlation coefficient section
      void createShapeCorrelationCoefficientReport();
      
      /// create the surface shape cluster section
      void createShapeClusterReport();
      
      /// create the query page
      void createQuerySelectionPage();
      
      /// create the attribute page
      void createAttributeSelectionPage();
      
      /// Create the report header (returns true if no nodes in query)
      bool createReportHeader(const QString& headerText, 
                              const bool tabSeparateReportIn,
                              float& roiAreaOut);

      /// create the report page
      void createReportPage();
      
      /// create the report
      void createReport(const QString& headerText,
                        const bool tabSeparateFlag,
                        const bool doConclusion);
      
      /// see if there are any nodes in the ROI
      bool haveNodesInROI() const;
      
      /// reset the marked nodes and update/clear the report header
      void resetMarkedNodesAndReportHeader();
      
      /// update a node attribute file's categories
      void updateNodeAttributeGroupBox(
                                 QVBoxLayout* layout,
                                 std::vector<QCheckBox*>& checkBoxes,
                                 NodeAttributeFile* naf);
                                 
      /// update a node attribute file's categories
      void updateNodeAttributeGroupBox(
                                 QVBoxLayout* layout,
                                 std::vector<QCheckBox*>& checkBoxes,
                                 GiftiNodeDataFile* naf);
                                 
      // /// perform ROI
      //void metricAndSurfaceShapeROI(const bool metricFlag);
      
      // /// perform ROI
      //void paintROI(const BrainModelSurface* bms, const double roiArea);
      
      /// remove nodes without neighbors from the ROI
      void removeNodesWithoutNeighborsFromROI();
      
      /// select all nodes
      void selectNodesAll();
      
      /// select nodes by border
      void selectNodesBorder();
      
      /// select nodes by metric
      void selectNodesMetric();
      
      /// select nodes by shape
      void selectNodesShape();
      
      /// select nodes by paint
      void selectNodesPaint();
      
      /// select nodes by crossovers
      void selectNodesCrossovers();
      
      /// update the number of selected nodes labels
      void updateNumberOfSelectedNodesLabel();
      
      /// query page for how ROI selected
      QWidget* queryPage;
      
      /// attributes page for controlling query output
      QWidget* attributesPage;
      
      /// report page
      QWidget* reportPage;
      
      /// selection mode combo box
      QComboBox* selectionModeComboBox;
      
      /// nodes with crossovers widget
      QWidget* nodesWithCrossoversWidget;
      
      /// entire surface empty widget
      QWidget* queryEntireSurfaceWidget;
      
      /// widget containing nodes with metric items
      QWidget* nodesWithMetricQVBox;
      
      /// widget containing nodes with paint items
      QWidget* nodesWithPaintQVBox;
      
      /// query control widget stack
      QStackedWidget* queryControlWidgetStack;
      
      /// layout for metric
      QVBoxLayout* metricOutputLayout;
      
      /// line edit for lower metric threshold
      QDoubleSpinBox* metricLowerThresholdDoubleSpinBox;
      
      /// line edit for upper metric threshold
      QDoubleSpinBox* metricUpperThresholdDoubleSpinBox;
      
      /// metric all nodes radio button
      QRadioButton* metricAllNodesRadioButton;
      
      /// metric choose nodes with mouse radio button
      QRadioButton* metricChooseNodesRadioButton;
      
      /// metric node number label
      QLabel* metricNodeNumberLabel;
      
      /// check buttons for metric attribute selections
      std::vector<QCheckBox*> metricCheckBoxes;
      
      /// layout for paint output
      QVBoxLayout* paintOutputLayout;
      
      /// check buttons for paint selections
      std::vector<QCheckBox*> paintCheckBoxes;
      
      /// layout for surface shape
      QVBoxLayout* shapeOutputLayout;
      
      /// check buttons for surface shape selections
      std::vector<QCheckBox*> surfaceShapeCheckBoxes;
      
      /// report text editor
      QTextEdit* reportTextEdit;
      
      /// nodes with paint category combo box
      GuiNodeAttributeColumnSelectionComboBox* paintWithNameCategoryComboBox;
      
      /// nodes with paint selected name label
      QLabel* paintWithNameSelectedLabel;

      /// selected paint index
      int paintWithNameIndex;
      
      /// borders widget
      QWidget* nodesWithinBorderQVBox;
      
      /// selected border name label
      QLabel* borderNameSelectedLabel;
      
      /// name of the selected border
      QString selectedBorderName;
      
      /// if true, indicates node is in ROI
      std::vector<bool> nodeInROI;
      
      /// if true, indicates tile is in ROI
      std::vector<bool> tileInROI;
      
      /// tile area
      std::vector<float> tileArea;
      
      /// combo box for operation surface
      GuiBrainModelSelectionComboBox* operationSurfaceComboBox;
      
      /// combo box topology file
      //GuiTopologyFileComboBox* topologyComboBox;
      
      /// tab separate check box
      QCheckBox* tabSeparateReportCheckBox;
      
      /// tab separate the report
      bool tabSeparateReport;
      
      /// combo box for selecting border surface
      GuiBrainModelSelectionComboBox* borderSurfaceComboBox;
      
      /// metric node for query
      int metricNodeForQuery;
      
      /// shape node for query
      int shapeNodeForQuery;
      
      /// number of nodes selected label
      QLabel* numberOfNodesSelectedLabel;
      
      /// show selected nodes check box
      QCheckBox* showSelectedNodesCheckBox;
      
      /// select logic combo box
      QComboBox* selectionLogicComboBox;
      
      /// operation combo box
      QComboBox* operationComboBox;
      
      /// operations widget stack
      QStackedWidget* operationsWidgetStack;
      
      /// assign metric operation widget
      QWidget* operationAssignMetricWidget;
      
      /// assign paint operation widget
      QWidget* operationAssignPaintWidget;
      
      /// assign surface shape operation widget
      QWidget* operationAssignSurfaceShapeWidget;
      
      /// create borders around clusters widget
      QWidget* operationCreateBordersFromClustersWidget;
      
      /// create border from ROI widget
      QWidget* operationCreateBordersFromROIWidget;
      
      /// volume ROI operation widget
      QWidget* operationCreateVolumeRoiWidget;
      
      /// disconnect nodes operation widget
      QWidget* operationDisconnectNodesWidget;
      
      /// statistical paint subregion report operation widget
      QWidget* operationStatisticalPaintReportWidget;
      
      /// tabe separate paint report widget
      QCheckBox* tabSeparatePaintReportCheckBox;
      
      /// statistical report operation widget
      QWidget* operationStatisticalReportWidget;
      
      /// smooth nodes operation widget
      QWidget* operationSmoothNodesWidget;
      
      /// shape cluster analysis
      QWidget* operationShapeClusterWidget;
      
      /// geodesic operations widget
      QWidget* operationGeodesicWidget;
      
      /// integrated folding index operations widget
      QWidget* operationComputeIntegratedFoldingIndexWidget;
      
      /// shape correlation coefficient widget
      QWidget* operationShapeCorrelationWidget;
      
      /// report header that describes node selection
      QString reportHeader;
      
      /// assign paint name combo box
      GuiNodeAttributeColumnSelectionComboBox* paintColumnAssignComboBox;
      
      /// assign paint name column name
      QLineEdit* paintColumnAssignNameLineEdit;
      
      /// assign paint name 
      QLineEdit* paintAssignNameLineEdit;
      
      /// geodesic node number spin box
      QSpinBox* geodesicNodeSpinBox;
      
      /// shape correlation column combo box
      GuiNodeAttributeColumnSelectionComboBox* shapeCorrelationColumnComboBox;
      
      /// shape correlation tab separate check box
      QCheckBox* shapeCorrelationTabSeparateCheckBox;
      
      /// group box for shape selection items
      QWidget* nodesWithShapeQVBox;
      
      /// shape lower threshold line edit
      QDoubleSpinBox* shapeLowerThresholdDoubleSpinBox;

      /// shape upper threshold line edit
      QDoubleSpinBox* shapeUpperThresholdDoubleSpinBox;
      
      /// shape radio button
      QRadioButton* shapeAllNodesRadioButton;

      /// shape radio button
      QRadioButton* shapeChooseNodesRadioButton;

      /// shape node number label
      QLabel* shapeNodeNumberLabel;
      
      /// metric column assign combo box
      GuiNodeAttributeColumnSelectionComboBox* metricColumnAssignComboBox;
      
      /// metric column name line edit
      QLineEdit* metricColumnAssignNameLineEdit;
      
      /// metric column assign line 
      QDoubleSpinBox* metricValueDoubleSpinBox;
      
      /// surface shape column assign combo box
      GuiNodeAttributeColumnSelectionComboBox* surfaceShapeColumnAssignComboBox;
      
      /// surface shape column name line edit
      QLineEdit* surfaceShapeColumnAssignNameLineEdit;
      
      /// surface shape column assign line 
      QDoubleSpinBox* surfaceShapeValueDoubleSpinBox;
      
      /// geodesic metric file column combo box
      GuiNodeAttributeColumnSelectionComboBox* geodesicMetricColumnComboBox;
      
      /// geodesic metric column name line edit
      QLineEdit* geodesicMetricColumnNameLineEdit;
      
      /// geodesic distance file column combo box
      GuiNodeAttributeColumnSelectionComboBox* geodesicDistanceColumnComboBox;
      
      /// geodesic distance column name line edit
      QLineEdit* geodesicDistanceColumnNameLineEdit;
      
      /// paint region combo box
      GuiNodeAttributeColumnSelectionComboBox* paintRegionReportColumnComboBox;
      
      /// prob atlas operation widget
      QWidget* operationProbAtlasWidget;
      
      /// prob atlas separate with tab option
      QCheckBox* probAtlasTabSeparateCheckBox;
      
      /// the separator character
      QString separatorCharacter;
      
      /// shape cluster report threshold float spin box
      QDoubleSpinBox* shapeClusterThresholdDoubleSpinBox;
      
      /// check box for shape cluster report separate with tabs
      QCheckBox* shapeClusterTabSeparateCheckBox;
      
      /// line edit for name for border around clusters
      QLineEdit* clusterBorderNameLineEdit;
      
      /// auto project borders around clusters
      QCheckBox* clusterBorderAutoProjectCheckBox;
      
      /// distortion correction for statistical report
      GuiNodeAttributeColumnSelectionComboBox* distortionCorrectionMetricColumnComboBox;

      /// tab widget for dialog
      QTabWidget* tabWidget;
      
      /// name for create border from ROI line edit
      QLineEdit* createBorderFromROINameLineEdit;

      /// create border automatic radio button
      QRadioButton* createBorderFromROIAutomaticRadioButton;
      
      /// create border choose nodes radio button
      QRadioButton* createBorderFromROIManualRadioButton;
      
      /// create border start node spin box
      QSpinBox* createBorderFromROIStartNodeSpinBox;
      
      /// create border end node spin box
      QSpinBox* createBorderFromROIEndNodeSpinBox;
      
      /// create border from roi node selection widget
      QWidget* createBorderFromROINodeSelectionWidget;
      
      /// create border from roi sampling density
      QDoubleSpinBox* createBorderFromROISamplingDensityDoubleSpinBox;
};

#endif // __VE_GUI_SURFACE_REGION_OF_INTEREST_DIALOG_H__


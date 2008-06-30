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
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTextEdit>
#include <QWizardPage>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurfaceClusterToBorderConverter.h"
#include "BrainModelSurfaceGeodesic.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfaceROIAssignMetric.h"
#include "BrainModelSurfaceROIAssignPaint.h"
#include "BrainModelSurfaceROIAssignShape.h"
#include "BrainModelSurfaceROICreateBorderUsingGeodesic.h"
#include "BrainModelSurfaceROIFoldingMeasurementReport.h"
#include "BrainModelSurfaceROIIntegratedFoldingIndexReport.h"
#include "BrainModelSurfaceROIMetricClusterReport.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROIPaintReport.h"
#include "BrainModelSurfaceROIProbAtlasOverlapReport.h"
#include "BrainModelSurfaceROIShapeCorrelationReport.h"
#include "BrainModelSurfaceROITextReport.h"
#include "BrainModelSurfaceToVolumeConverter.h"
#include "BrainSet.h"
#include "DisplaySettingsBorders.h"
#include "FileFilters.h"
#include "GuiBorderNamesListBoxSelectionDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiColorSelectionDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNameSelectionDialog.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiPaintColumnNamesListBoxSelectionDialog.h"
#include "GuiSmoothingDialog.h"
#include "GuiSurfaceRegionOfInterestDialog.h"
#include "GuiSurfaceToVolumeDialog.h"
#include "GuiTopologyFileComboBox.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "NodeRegionOfInterestFile.h"
#include "PaintFile.h"
#include "QtUtilities.h"
#include "SurfaceShapeFile.h"
#include "WuQFileDialog.h"
#include "WuQWidgetGroup.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiSurfaceRegionOfInterestDialog::GuiSurfaceRegionOfInterestDialog(QWidget* parent)
   : QWizard(parent)
{
   setWindowTitle("Surface Region of Interest");
   setOption(QWizard::NoCancelButton, false);
   setWizardStyle(QWizard::ModernStyle);
   
   surfaceSelectionPage = new GuiSurfaceROISurfaceAndTopologySelectionPage(this);
   setPage(PAGE_ID_OPERATION_SURFACE_AND_TOPOLOGY,
           surfaceSelectionPage);
           
   roiNodeSelectionPage = new GuiSurfaceROINodeSelectionPage(this);
   setPage(PAGE_ID_NODE_SELECTION,
           roiNodeSelectionPage);
           
   reportPage = new GuiSurfaceROIReportPage(this);
   setPage(PAGE_ID_REPORT,
           reportPage);

   operationPage = new GuiSurfaceROIOperationPage(this, reportPage);
   setPage(PAGE_ID_OPERATION,
           operationPage);
           
}

/**
 * destructor.
 */
GuiSurfaceRegionOfInterestDialog::~GuiSurfaceRegionOfInterestDialog()
{
}

/**
 * show the dialog.
 */
void 
GuiSurfaceRegionOfInterestDialog::show()
{
   //
   // May want to show selected nodes 
   //
   roiNodeSelectionPage->showSelectedNodesCheckBox->setChecked(true);
   roiNodeSelectionPage->slotShowSelectedNodesCheckBox(roiNodeSelectionPage->showSelectedNodesCheckBox->isChecked());
   roiNodeSelectionPage->updateNumberOfSelectedNodesLabel();

   QWizard::show();
}
      
/**
 * called when dialog closed.
 */
void 
GuiSurfaceRegionOfInterestDialog::reject()
{
   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   roi->setDisplaySelectedNodes(false);
   
   GuiBrainModelOpenGL::MOUSE_MODES m = theMainWindow->getBrainModelOpenGL()->getMouseMode();
   if ((m == GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_BORDER_SELECT) ||
       (m == GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_PAINT_INDEX_SELECT) ||
       (m == GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_METRIC_NODE_SELECT) ||
       (m == GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_GEODESIC_NODE_SELECT)) {
      theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   }
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL();

   QWizard::reject();
}
      
/**
 * update the dialog (typically due to file changes).
 */
void 
GuiSurfaceRegionOfInterestDialog::updateDialog()
{
   roiNodeSelectionPage->updatePage();
   surfaceSelectionPage->updatePage();
   operationPage->updatePage();
}

/**
 * set border name for query.
 */
void 
GuiSurfaceRegionOfInterestDialog::setBorderNameForQuery(const QString& borderName)
{
   roiNodeSelectionPage->borderNameSelectionLabel->setText(borderName);
}

/**
 * set paint index for query.
 */
void 
GuiSurfaceRegionOfInterestDialog::setPaintIndexForQuery(const int paintIndex)
{
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   roiNodeSelectionPage->paintNameSelectionLabel->setText(pf->getPaintNameFromIndex(paintIndex));
}

/**
 * set metric node for query.
 */
void 
GuiSurfaceRegionOfInterestDialog::setMetricNodeForQuery(const int metricNodeForQuery)
{
   //BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   //if (bms == NULL) {
   //   return;
   //}
   
   roiNodeSelectionPage->metricSelectionNodeNumberLabel->setNum(metricNodeForQuery);
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   
   if (mf->getNumberOfColumns() <= 0) {
      QMessageBox::critical(this, "Error", "There are no metric columns.");
      return;
   }
   const int column = roiNodeSelectionPage->metricColumnSelectionComboBox->currentIndex();
   if ((column < 0) || (column >= mf->getNumberOfColumns())) {
      QMessageBox::critical(this, "Error", "Metric column is invalid.");
      return;
   }
   
   roiNodeSelectionPage->metricSelectionNodeValueLabel->setText(
            QString::number(mf->getValue(metricNodeForQuery, column), 'f', 4));
}

/**
 * set shape node for query.
 */
void 
GuiSurfaceRegionOfInterestDialog::setShapeNodeForQuery(const int shapeNodeForQuery)
{
   //BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   //if (bms == NULL) {
   //   return;
   //}
   
   roiNodeSelectionPage->shapeSelectionNodeNumberLabel->setNum(shapeNodeForQuery);
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   
   if (ssf->getNumberOfColumns() <= 0) {
      QMessageBox::critical(this, "Error", "There are no surface shape columns.");
      return;
   }
   const int column = roiNodeSelectionPage->shapeColumnSelectionComboBox->currentIndex();
   if ((column < 0) || (column >= ssf->getNumberOfColumns())) {
      QMessageBox::critical(this, "Error", "Surface Shape column is invalid.");
      return;
   }
   
   roiNodeSelectionPage->shapeSelectionNodeValueLabel->setText(
      QString::number(ssf->getValue(shapeNodeForQuery, column), 'f', 4));
}

/**
 * set node for geodesic query.
 */
void 
GuiSurfaceRegionOfInterestDialog::setNodeForGeodesicQuery(const int nodeNumber)
{
   operationPage->geodesicNodeSpinBox->setValue(nodeNumber);
}

/**
 * get the query paint column.
 */
int 
GuiSurfaceRegionOfInterestDialog::getQueryPaintColumn() const
{
   return roiNodeSelectionPage->paintColumnSelectionComboBox->currentIndex();
}

/**
 * get a COPY of operation surface with operation topology (user must delete object).
 */
BrainModelSurface* 
GuiSurfaceRegionOfInterestDialog::getCopyOfOperationSurface()
{
   return surfaceSelectionPage->getCopyOfOperationSurface();
}
      
/**
 * get the operation coordinate file.
 */
CoordinateFile* 
GuiSurfaceRegionOfInterestDialog::getOperationCoordinateFile()
{
   return surfaceSelectionPage->getOperationCoordinateFile();
}      
      
//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
GuiSurfaceROIReportPage::GuiSurfaceROIReportPage(GuiSurfaceRegionOfInterestDialog* roiDialogIn)
   : QWizardPage(0)
{
   roiDialog = roiDialogIn;
   setTitle("ROI Report");
   
   //
   // The report text edit
   //
   reportTextEdit = new QTextEdit;
   reportTextEdit->setReadOnly(true);
   reportTextEdit->setLineWrapMode(QTextEdit::NoWrap);

   //
   // Clear text report push button
   //   
   QPushButton* clearReportPushButton = new QPushButton("Clear");
   clearReportPushButton->setAutoDefault(false);
   QObject::connect(clearReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotClearReportTextEdit()));

   //
   // Save text edit to a file push button
   //
   QPushButton* saveReportPushButton = new QPushButton("Save Report to File...");
   saveReportPushButton->setAutoDefault(false);
   saveReportPushButton->setFixedSize(saveReportPushButton->sizeHint());
   QObject::connect(saveReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSaveReportToTextFile()));
                    
   QtUtilities::makeButtonsSameSize(clearReportPushButton,
                                    saveReportPushButton);
                                    
   //
   // Layout for buttons
   // 
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(clearReportPushButton);
   buttonsLayout->addWidget(saveReportPushButton);
   
   //
   // Layout for page
   //
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(reportTextEdit);
   layout->addLayout(buttonsLayout);
}

/**
 * destructor.
 */
GuiSurfaceROIReportPage::~GuiSurfaceROIReportPage()
{
}

/**
 * clean up the page.
 */
void 
GuiSurfaceROIReportPage::cleanupPage()
{
}

/**
 * initialize the page.
 */
void 
GuiSurfaceROIReportPage::initializePage()
{
}

/**
 * validate the page.
 */
bool 
GuiSurfaceROIReportPage::validatePage()
{
   return true;
}

/**
 * is page complete.
 */
bool 
GuiSurfaceROIReportPage::isComplete()
{
   return true;
}

/**
 * update the page.
 */
void 
GuiSurfaceROIReportPage::updatePage()
{
}

/**
 * set the report page's text edit.
 */
void 
GuiSurfaceROIReportPage::setReportText(const QString& s)
{
   reportTextEdit->setPlainText(s);
}
      
/**
 * called when clear report button pressed.
 */
void 
GuiSurfaceROIReportPage::slotClearReportTextEdit()
{
   reportTextEdit->clear();
}
      
/**
 * called when save report to text file button pressed.
 */
void 
GuiSurfaceROIReportPage::slotSaveReportToTextFile()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(WuQFileDialog::AcceptSave);
   fd.setWindowTitle("Choose Report Text File Name");
   fd.setFilter(FileFilters::getTextFileFilter());
   fd.setFileMode(WuQFileDialog::AnyFile);
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         const QString fileName = fd.selectedFiles().at(0);
         QFile file(fileName);
         if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream << reportTextEdit->toPlainText();
            file.close();
         }
         else {
            QString msg("Unable to open for writing: ");
            msg.append(fileName);
            QMessageBox::critical(this, "Error Opening File", msg);         
         }
      }
   }
}

//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
GuiSurfaceROINodeSelectionPage::GuiSurfaceROINodeSelectionPage(GuiSurfaceRegionOfInterestDialog* roiDialogIn)
   : QWizardPage(0)
{
   roiDialog = roiDialogIn;
   setTitle("Node Selection");
   
   // 
   // selection source label and combo box
   //
   QLabel* selectionSourceLabel = new QLabel("Selection Source");
   selectionSourceComboBox = new QComboBox;
   QObject::connect(selectionSourceComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSelectionSourceComboBox(int)));
   selectionSourceComboBox->addItem("All Nodes", 
                                    static_cast<int>(SELECTION_SOURCE_ALL_NODES));
   selectionSourceComboBox->addItem("Borders",  
                                    static_cast<int>(SELECTION_SOURCE_NODES_WITHIN_BORDER));
   selectionSourceComboBox->addItem("Crossovers",  
                                    static_cast<int>(SELECTION_SOURCE_NODES_WITH_CROSSOVERS));
   selectionSourceComboBox->addItem("Edges",  
                                    static_cast<int>(SELECTION_SOURCE_NODES_WITH_EDGES));
   selectionSourceComboBox->addItem("Lat/Long",  
                                    static_cast<int>(SELECTION_SOURCE_NODES_WITHIN_LATLON));
   selectionSourceComboBox->addItem("Metric",  
                                    static_cast<int>(SELECTION_SOURCE_NODES_WITH_METRIC));
   selectionSourceComboBox->addItem("Paint",  
                                    static_cast<int>(SELECTION_SOURCE_NODES_WITH_PAINT));
   selectionSourceComboBox->addItem("Surface Shape",  
                                    static_cast<int>(SELECTION_SOURCE_NODES_WITH_SHAPE));

   //
   // selection logic label and combo box
   //
   QLabel* selectionLogicLabel = new QLabel("Selection Logic");
   selectionLogicComboBox = new QComboBox;
   selectionLogicComboBox->addItem("Normal",
               static_cast<int>(BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL));
   selectionLogicComboBox->addItem("And (Intersection)",
               static_cast<int>(BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND));
   selectionLogicComboBox->addItem("Or (Union)",
               static_cast<int>(BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_OR));
   selectionLogicComboBox->addItem("And-Not",
               static_cast<int>(BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND_NOT));
 
   //
   // Nodes selected labels and layout
   //  
   QLabel* nodesSelectedLabel = new QLabel("Nodes Selected: ");
   numberOfNodesSelectedLabel = new QLabel("          ");
   QHBoxLayout* nodesSelectedLayout = new QHBoxLayout;
   nodesSelectedLayout->addWidget(nodesSelectedLabel);
   nodesSelectedLayout->addWidget(numberOfNodesSelectedLabel);
   nodesSelectedLayout->addStretch();
   
   //
   // Show selected nodes check box
   //
   showSelectedNodesCheckBox = new QCheckBox("Show Selected Nodes");
   QObject::connect(showSelectedNodesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowSelectedNodesCheckBox(bool)));
                    
   //
   // selection options stacked widget
   //
   selectionSourceOptionsStackedWidget = createNodeSelectionSourceOptionsWidget();
   
   //
   // Group box and layout for node selection
   //
   QGridLayout* selectionGridLayout = new QGridLayout;
   selectionGridLayout->addWidget(selectionSourceLabel, 0, 0);
   selectionGridLayout->addWidget(selectionSourceComboBox, 0, 1);
   selectionGridLayout->addWidget(selectionLogicLabel, 1, 0);
   selectionGridLayout->addWidget(selectionLogicComboBox, 1, 1);
   selectionGridLayout->addWidget(showSelectedNodesCheckBox, 2, 0, 1, 1);
   selectionGridLayout->addLayout(nodesSelectedLayout, 2, 1, 1, 2);
   QGroupBox* selectionGroupBox = new QGroupBox("Node Selection Source on Main Window Surface");
   QVBoxLayout* selectionGroupLayout = new QVBoxLayout(selectionGroupBox);
   selectionGroupLayout->addLayout(selectionGridLayout);
   selectionGroupLayout->addWidget(selectionSourceOptionsStackedWidget);
   selectionGroupLayout->addStretch();
   
   //
   //-------------------------------------------------------------------------
   //

   //
   // Select Nodes push button
   //
   QPushButton* selectNodesPushButton = new QPushButton("Select Nodes");
   selectNodesPushButton->setToolTip("Select nodes using the current\n"
                                     "selection source and logic.");
   selectNodesPushButton->setAutoDefault(true);
   QObject::connect(selectNodesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectedNodesPushButton()));  

   //
   // Deselect Nodes push button
   //
   deselectNodesPushButton = new QPushButton("Deselect All Nodes");
   deselectNodesPushButton->setToolTip("Deselect all nodes.");
   deselectNodesPushButton->setAutoDefault(false);
   QObject::connect(deselectNodesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeselectNodesPushButton()));  
                    
   //
   // Dilate ROI push button
   //
   dilateSelectedNodesPushButton = new QPushButton("Dilate");
   dilateSelectedNodesPushButton->setToolTip("Dilate the selected nodes.\n"
                                             "\n"
                                             "Select a node if any of its \n"
                                             "immediate neighbors are selected\n"
                                             "(\"Grows\" the ROI).");
   dilateSelectedNodesPushButton->setAutoDefault(false);
   QObject::connect(dilateSelectedNodesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDilateROIPushButton()));

   //
   // erode ROI push button
   //
   erodeSelectedNodesPushButton = new QPushButton("Erode");
   erodeSelectedNodesPushButton->setToolTip("Erode the selected nodes.\n"
                                            "\n"
                                            "Deselect a node if any of \n"
                                            "its immediate neighbors are \n"
                                            "not selected.\n"
                                            "(\"Shrinks\" the ROI).");
   erodeSelectedNodesPushButton->setAutoDefault(false);
   QObject::connect(erodeSelectedNodesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotErodeROIPushButton()));

   //
   // Invert ROI push button
   //
   invertSelectedNodesPushButton = new QPushButton("Invert");
   invertSelectedNodesPushButton->setToolTip("Invert the current selection.");
   invertSelectedNodesPushButton->setAutoDefault(false);
   QObject::connect(invertSelectedNodesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotInvertNodesPushButton()));
                    
   //
   // Remove islands
   //
   removeIslandsPushButton = new QPushButton("Remove Islands");
   removeIslandsPushButton->setToolTip("If there are disjoint (unconnected)\n"
                                       "regions in the selected nodes, the\n"
                                       "largest region is retained and any\n"
                                       "nodes in smaller regions are deselected.");
   removeIslandsPushButton->setAutoDefault(false);
   QObject::connect(removeIslandsPushButton, SIGNAL(clicked()),
                    this, SLOT(slotRemoveIslands()));
                    
   //
   // Group box and layout for modify ROI
   //
   QHBoxLayout* row1Layout = new QHBoxLayout;
   row1Layout->addWidget(selectNodesPushButton);
   row1Layout->addWidget(deselectNodesPushButton);
   row1Layout->addWidget(invertSelectedNodesPushButton);
   row1Layout->addStretch();
   QHBoxLayout* row2Layout = new QHBoxLayout;
   row2Layout->addWidget(removeIslandsPushButton);
   row2Layout->addWidget(dilateSelectedNodesPushButton);
   row2Layout->addWidget(erodeSelectedNodesPushButton);
   row2Layout->addStretch();
   QGroupBox* modifyROIGroupBox = new QGroupBox("Select Nodes and Modify Region of Interest");
   QVBoxLayout* modifyROILayout = new QVBoxLayout(modifyROIGroupBox);
   modifyROILayout->addLayout(row1Layout);
   modifyROILayout->addLayout(row2Layout);
   modifyROILayout->addStretch();

   //
   //-------------------------------------------------------------------------
   //

   //
   // Load an ROI pushbutton
   //
   loadROIPushButton = new QPushButton("Load...");
   loadROIPushButton->setToolTip("Set the ROI using the status from\n"
                                 "Region of Interest File.");
   loadROIPushButton->setAutoDefault(false);
   QObject::connect(loadROIPushButton, SIGNAL(clicked()),
                    this, SLOT(slotLoadROIPushButton()));
   
   //
   // Save an ROI pushbutton
   //
   saveROIPushButton = new QPushButton("Save...");
   saveROIPushButton->setToolTip("Save the ROI status into\n"
                                 "a Region of Interest File.");
   saveROIPushButton->setAutoDefault(false);
   QObject::connect(saveROIPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSaveROIPushButton()));
                    
   //
   // Group box and layout for laod/save options
   //
   QGroupBox* fileOperationsGroupBox = new QGroupBox("ROI File Operations");
   QVBoxLayout* fileOperationsLayout = new QVBoxLayout(fileOperationsGroupBox);
   fileOperationsLayout->addWidget(loadROIPushButton);
   fileOperationsLayout->addWidget(saveROIPushButton);
   fileOperationsLayout->addStretch();
   
   
   //
   //-------------------------------------------------------------------------
   //
   //
   // Layout for page
   //
   QHBoxLayout* modifyAndFileLayout = new QHBoxLayout;
   modifyAndFileLayout->addWidget(modifyROIGroupBox);
   modifyAndFileLayout->addWidget(fileOperationsGroupBox);
   modifyAndFileLayout->addStretch();
   QVBoxLayout* pageLayout = new QVBoxLayout(this);
   pageLayout->addWidget(selectionGroupBox);
   pageLayout->addLayout(modifyAndFileLayout);
   pageLayout->addStretch();
}

/**
 * destructor.
 */
GuiSurfaceROINodeSelectionPage::~GuiSurfaceROINodeSelectionPage()
{
}

/**
 * create the node selection source options widget.
 */
QStackedWidget* 
GuiSurfaceROINodeSelectionPage::createNodeSelectionSourceOptionsWidget()
{
   //--------------------------------------------------------------------------
   //
   // selection widget for all nodes (no options)
   //
   selectionSourceOptionsAllNodesWidget = new QWidget;
   
   //--------------------------------------------------------------------------
   //
   // selection widget for borders
   //
   
   //
   // Flat surface label and selection
   //
   QLabel* borderFlatSurfaceLabel = new QLabel("Flat Surface");
   borderFlatSurfaceSelectionComboBox = 
      new GuiBrainModelSelectionComboBox(
         GuiBrainModelSelectionComboBox::OPTION_SHOW_SURFACES_FLAT);

   //
   // Choose border name from a list of border names
   //
   QPushButton* selectBorderNameFromListPushButton =
      new QPushButton("Select Name From List...");
   selectBorderNameFromListPushButton->setAutoDefault(false);
   QObject::connect(selectBorderNameFromListPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectBorderNameFromListPushButton()));
                    
   //
   // Choose paint name by clicking on a node with the mouse
   //
   QPushButton* selectBorderNameNodeWithMousePushButton =
      new QPushButton("Select Border With Mouse...");
   selectBorderNameNodeWithMousePushButton->setAutoDefault(false);
   QObject::connect(selectBorderNameNodeWithMousePushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectBorderNameWithMousePushButton()));
                    
   //
   // Make the buttons the same size
   //
   WuQWidgetGroup* borderButtonWidgetGroup = new WuQWidgetGroup(this);
   borderButtonWidgetGroup->addWidget(selectBorderNameFromListPushButton);
   borderButtonWidgetGroup->addWidget(selectBorderNameNodeWithMousePushButton);
   borderButtonWidgetGroup->resizeAllToLargestSizeHint();
                         
   //
   // Selected name label
   //
   QLabel* borderNameLabel = new QLabel("Name ");
   borderNameSelectionLabel = new QLabel("                                   ");

   //
   // borders group box and layout
   //
   selectionSourceOptionsBordersWidget = new QGroupBox("Border Options");
   QGridLayout* selectionBorderLayout = new QGridLayout(selectionSourceOptionsBordersWidget);
   selectionBorderLayout->setColumnStretch(0, 0);
   selectionBorderLayout->setColumnStretch(1, 100);
   selectionBorderLayout->addWidget(borderFlatSurfaceLabel, 0, 0);
   selectionBorderLayout->addWidget(borderFlatSurfaceSelectionComboBox, 0, 1, Qt::AlignLeft);
   selectionBorderLayout->addWidget(selectBorderNameFromListPushButton, 1, 0, 1, 2, Qt::AlignLeft);
   selectionBorderLayout->addWidget(selectBorderNameNodeWithMousePushButton, 2, 0, 1, 2, Qt::AlignLeft);
   selectionBorderLayout->addWidget(borderNameLabel, 3, 0);
   selectionBorderLayout->addWidget(borderNameSelectionLabel, 3, 1, Qt::AlignLeft);
   
   //--------------------------------------------------------------------------
   //
   // selection widget for crossovers (no options)
   //
   selectionSourceOptionsCrossoversWidget = new QWidget;
   
   //--------------------------------------------------------------------------
   //
   // selection widget for edges (no options)
   //
   selectionSourceOptionsEdgesWidget = new QWidget;
   
   //--------------------------------------------------------------------------
   //
   // selection widget for lat/long
   //
   
   //
   // Lower latitude
   //
   latLowerRangeDoubleSpinBox = new QDoubleSpinBox;
   latLowerRangeDoubleSpinBox->setMinimum(-90.0);
   latLowerRangeDoubleSpinBox->setMaximum( 90.0);
   latLowerRangeDoubleSpinBox->setSingleStep(1.0);
   latLowerRangeDoubleSpinBox->setDecimals(3);
   latLowerRangeDoubleSpinBox->setValue(-90.0);
   latLowerRangeDoubleSpinBox->setFixedWidth(120);

   //
   // Upper latitude
   //
   latUpperRangeDoubleSpinBox = new QDoubleSpinBox;
   latUpperRangeDoubleSpinBox->setMinimum(-90.0);
   latUpperRangeDoubleSpinBox->setMaximum( 90.0);
   latUpperRangeDoubleSpinBox->setSingleStep(1.0);
   latUpperRangeDoubleSpinBox->setDecimals(3);
   latUpperRangeDoubleSpinBox->setValue(90.0);
   latUpperRangeDoubleSpinBox->setFixedWidth(120);
   
   //
   // lower longitude
   //
   lonLowerRangeDoubleSpinBox = new QDoubleSpinBox;
   lonLowerRangeDoubleSpinBox->setMinimum(-360.0);
   lonLowerRangeDoubleSpinBox->setMaximum( 360.0);
   lonLowerRangeDoubleSpinBox->setSingleStep(1.0);
   lonLowerRangeDoubleSpinBox->setDecimals(3);
   lonLowerRangeDoubleSpinBox->setValue(-180.0);
   lonLowerRangeDoubleSpinBox->setFixedWidth(120);

   //
   // upper longitude
   //
   lonUpperRangeDoubleSpinBox = new QDoubleSpinBox;
   lonUpperRangeDoubleSpinBox->setMinimum(-360.0);
   lonUpperRangeDoubleSpinBox->setMaximum( 360.0);
   lonUpperRangeDoubleSpinBox->setSingleStep(1.0);
   lonUpperRangeDoubleSpinBox->setDecimals(3);
   lonUpperRangeDoubleSpinBox->setValue(180.0);
   lonUpperRangeDoubleSpinBox->setFixedWidth(120);
   
   //
   // Widget and layout for latitutude/longitude
   //
   selectionSourceOptionsLatLongWidget = new QGroupBox("Lat/Long Options") ;
   QGridLayout* latLonLayout = new QGridLayout(selectionSourceOptionsLatLongWidget);
   latLonLayout->addWidget(new QLabel("Min-Value"), 0, 1);
   latLonLayout->addWidget(new QLabel("Max-Value"), 0, 2);
   latLonLayout->addWidget(new QLabel("Latitude"), 1, 0);
   latLonLayout->addWidget(new QLabel("Longitude"), 2, 0);
   latLonLayout->addWidget(latLowerRangeDoubleSpinBox, 1, 1);
   latLonLayout->addWidget(latUpperRangeDoubleSpinBox, 1, 2);
   latLonLayout->addWidget(lonLowerRangeDoubleSpinBox, 2, 1);
   latLonLayout->addWidget(lonUpperRangeDoubleSpinBox, 2, 2);
   
   //--------------------------------------------------------------------------
   //
   // selection widget for metric
   //

   //
   // Metric column selection and label
   //
   QLabel* metricCategoryLabel = new QLabel("Category");
   metricColumnSelectionComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_METRIC,
                                                  false,
                                                  false,
                                                  false);
   QHBoxLayout* metricCategoryLayout = new QHBoxLayout;
   metricCategoryLayout->addWidget(metricCategoryLabel);
   metricCategoryLayout->addWidget(metricColumnSelectionComboBox);
   metricCategoryLayout->addStretch();
                                                  
   //
   // Metric range label and spin boxes
   //
   QLabel* metricSelectionRangeLabel = new QLabel("Range ");
   metricSelectionLowerDoubleSpinBox = new QDoubleSpinBox;
   metricSelectionLowerDoubleSpinBox->setMinimum(-10000000.0);
   metricSelectionLowerDoubleSpinBox->setMaximum( 10000000.0);
   metricSelectionLowerDoubleSpinBox->setSingleStep(1.0);
   metricSelectionLowerDoubleSpinBox->setDecimals(4);
   metricSelectionLowerDoubleSpinBox->setValue(1.0);
   QLabel* metricSelectionToLabel = new QLabel(" to ");
   metricSelectionUpperDoubleSpinBox = new QDoubleSpinBox;
   metricSelectionUpperDoubleSpinBox->setMinimum(-10000000.0);
   metricSelectionUpperDoubleSpinBox->setMaximum( 10000000.0);
   metricSelectionUpperDoubleSpinBox->setSingleStep(1.0);
   metricSelectionUpperDoubleSpinBox->setDecimals(4);
   metricSelectionUpperDoubleSpinBox->setValue(50000.0);
   QHBoxLayout* metricRangeLayout = new QHBoxLayout;
   metricRangeLayout->addWidget(metricSelectionRangeLabel);
   metricRangeLayout->addWidget(metricSelectionLowerDoubleSpinBox);
   metricRangeLayout->addWidget(metricSelectionToLabel);
   metricRangeLayout->addWidget(metricSelectionUpperDoubleSpinBox);
   metricRangeLayout->addStretch();
   
   //
   // metric selection radio buttons
   //
   metricSelectionAllNodesRadioButton = 
      new QRadioButton("All Nodes Within Range");
   metricSelectionConnectedToNodeRadioButton = 
      new QRadioButton("Nodes Within Range Connected to Node Selected With Mouse");
   QButtonGroup* metricSelectionButtonGroup = new QButtonGroup(this);
   metricSelectionButtonGroup->addButton(metricSelectionAllNodesRadioButton);
   metricSelectionButtonGroup->addButton(metricSelectionConnectedToNodeRadioButton);
   QObject::connect(metricSelectionButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotMetricSelectionButtonGroup()));
                    
   //
   // Metric selection node number and value labels
   //
   QLabel* metricSelectionNodeLabel = new QLabel("   Node Number ");
   metricSelectionNodeNumberLabel = new QLabel("          ");
   QLabel* metricSelectionValueLabel = new QLabel(" Value ");
   metricSelectionNodeValueLabel = new QLabel("                        ");
   QHBoxLayout* metricNodeLayout = new QHBoxLayout;
   metricNodeLayout->addWidget(metricSelectionNodeLabel);
   metricNodeLayout->addWidget(metricSelectionNodeNumberLabel);
   metricNodeLayout->addWidget(metricSelectionValueLabel);
   metricNodeLayout->addWidget(metricSelectionNodeValueLabel);
   metricNodeLayout->addStretch();
   
   //
   // Metric widget and options
   //
   selectionSourceOptionsMetricWidget = new QGroupBox("Metric Options");
   QVBoxLayout* metricSelectionLayout = new QVBoxLayout(selectionSourceOptionsMetricWidget);
   metricSelectionLayout->addLayout(metricCategoryLayout);
   metricSelectionLayout->addLayout(metricRangeLayout);
   metricSelectionLayout->addWidget(metricSelectionAllNodesRadioButton);
   metricSelectionLayout->addWidget(metricSelectionConnectedToNodeRadioButton);
   metricSelectionLayout->addLayout(metricNodeLayout);
   metricSelectionLayout->addStretch();
   
   //--------------------------------------------------------------------------
   //
   // selection widget for paint 
   //
   
   //
   // Paint column selection and label
   //
   QLabel* paintCategoryLabel = new QLabel("Category");
   paintColumnSelectionComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_PAINT,
                                                  false,
                                                  false,
                                                  false);
                                                  
   //
   // Choose paint name from a list of paint names
   //
   QPushButton* selectPaintNameFromListPushButton =
      new QPushButton("Select Name From List...");
   selectPaintNameFromListPushButton->setAutoDefault(false);
   QObject::connect(selectPaintNameFromListPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectPaintNameFromListPushButton()));
                    
   //
   // Choose paint name by clicking on a node with the mouse
   //
   QPushButton* selectPaintNameNodeWithMousePushButton =
      new QPushButton("Select Node With Mouse...");
   selectPaintNameNodeWithMousePushButton->setAutoDefault(false);
   QObject::connect(selectPaintNameNodeWithMousePushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectPaintNameNodeWithMousePushButton()));
                    
   //
   // Make the buttons the same size
   //
   WuQWidgetGroup* buttonWidgetGroup = new WuQWidgetGroup(this);
   buttonWidgetGroup->addWidget(selectPaintNameFromListPushButton);
   buttonWidgetGroup->addWidget(selectPaintNameNodeWithMousePushButton);
   buttonWidgetGroup->resizeAllToLargestSizeHint();
                         
   //
   // Selected name label
   //
   QLabel* paintNameLabel = new QLabel("Name ");
   paintNameSelectionLabel = new QLabel("                                   ");
   
   //
   // Paint options widget and layout
   //
   selectionSourceOptionsPaintWidget = new QGroupBox("Paint Options");
   QGridLayout* selectionPaintLayout = new QGridLayout(selectionSourceOptionsPaintWidget);
   selectionPaintLayout->setColumnStretch(0, 0);
   selectionPaintLayout->setColumnStretch(0, 100);
   selectionPaintLayout->addWidget(paintCategoryLabel, 0, 0);
   selectionPaintLayout->addWidget(paintColumnSelectionComboBox, 0, 1, Qt::AlignLeft);
   selectionPaintLayout->addWidget(selectPaintNameFromListPushButton, 1, 0, 1, 2, Qt::AlignLeft);
   selectionPaintLayout->addWidget(selectPaintNameNodeWithMousePushButton, 2, 0, 1, 2, Qt::AlignLeft);
   selectionPaintLayout->addWidget(paintNameLabel, 3, 0);
   selectionPaintLayout->addWidget(paintNameSelectionLabel, 3, 1, Qt::AlignLeft);
   
   //--------------------------------------------------------------------------
   //
   // selection widget for surface shape 
   //

   //
   // Shape column selection and label
   //
   QLabel* shapeCategoryLabel = new QLabel("Category");
   shapeColumnSelectionComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                                  false,
                                                  false,
                                                  false);
   QHBoxLayout* shapeCategoryLayout = new QHBoxLayout;
   shapeCategoryLayout->addWidget(shapeCategoryLabel);
   shapeCategoryLayout->addWidget(shapeColumnSelectionComboBox);
   shapeCategoryLayout->addStretch();
                                                  
   //
   // Shape range label and spin boxes
   //
   QLabel* shapeSelectionRangeLabel = new QLabel("Range ");
   shapeSelectionLowerDoubleSpinBox = new QDoubleSpinBox;
   shapeSelectionLowerDoubleSpinBox->setMinimum(-10000000.0);
   shapeSelectionLowerDoubleSpinBox->setMaximum( 10000000.0);
   shapeSelectionLowerDoubleSpinBox->setSingleStep(1.0);
   shapeSelectionLowerDoubleSpinBox->setDecimals(4);
   shapeSelectionLowerDoubleSpinBox->setValue(1.0);
   QLabel* shapeSelectionToLabel = new QLabel(" to ");
   shapeSelectionUpperDoubleSpinBox = new QDoubleSpinBox;
   shapeSelectionUpperDoubleSpinBox->setMinimum(-10000000.0);
   shapeSelectionUpperDoubleSpinBox->setMaximum( 10000000.0);
   shapeSelectionUpperDoubleSpinBox->setSingleStep(1.0);
   shapeSelectionUpperDoubleSpinBox->setDecimals(4);
   shapeSelectionUpperDoubleSpinBox->setValue(50000.0);
   QHBoxLayout* shapeRangeLayout = new QHBoxLayout;
   shapeRangeLayout->addWidget(shapeSelectionRangeLabel);
   shapeRangeLayout->addWidget(shapeSelectionLowerDoubleSpinBox);
   shapeRangeLayout->addWidget(shapeSelectionToLabel);
   shapeRangeLayout->addWidget(shapeSelectionUpperDoubleSpinBox);
   shapeRangeLayout->addStretch();
   
   //
   // shape selection radio buttons
   //
   shapeSelectionAllNodesRadioButton = 
      new QRadioButton("All Nodes Within Range");
   shapeSelectionConnectedToNodeRadioButton = 
      new QRadioButton("Nodes Within Range Connected to Node Selected With Mouse");
   QButtonGroup* shapeSelectionButtonGroup = new QButtonGroup(this);
   shapeSelectionButtonGroup->addButton(shapeSelectionAllNodesRadioButton);
   shapeSelectionButtonGroup->addButton(shapeSelectionConnectedToNodeRadioButton);
   QObject::connect(shapeSelectionButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotShapeSelectionButtonGroup()));
                    
   //
   // Shape selection node number and value labels
   //
   QLabel* shapeSelectionNodeLabel = new QLabel("   Node Number ");
   shapeSelectionNodeNumberLabel = new QLabel("          ");
   QLabel* shapeSelectionValueLabel = new QLabel(" Value ");
   shapeSelectionNodeValueLabel = new QLabel("                        ");
   QHBoxLayout* shapeNodeLayout = new QHBoxLayout;
   shapeNodeLayout->addWidget(shapeSelectionNodeLabel);
   shapeNodeLayout->addWidget(shapeSelectionNodeNumberLabel);
   shapeNodeLayout->addWidget(shapeSelectionValueLabel);
   shapeNodeLayout->addWidget(shapeSelectionNodeValueLabel);
   shapeNodeLayout->addStretch();
   
   //
   // Shape widget and options
   //
   selectionSourceOptionsShapeWidget = new QGroupBox("Surface Shape Options");
   QVBoxLayout* shapeSelectionLayout = new QVBoxLayout(selectionSourceOptionsShapeWidget);
   shapeSelectionLayout->addLayout(shapeCategoryLayout);
   shapeSelectionLayout->addLayout(shapeRangeLayout);
   shapeSelectionLayout->addWidget(shapeSelectionAllNodesRadioButton);
   shapeSelectionLayout->addWidget(shapeSelectionConnectedToNodeRadioButton);
   shapeSelectionLayout->addLayout(shapeNodeLayout);
   shapeSelectionLayout->addStretch();

   //
   // stacked widget
   //
   QStackedWidget* stackedWidget = new QStackedWidget;
   stackedWidget->addWidget(selectionSourceOptionsAllNodesWidget);
   stackedWidget->addWidget(selectionSourceOptionsBordersWidget);
   stackedWidget->addWidget(selectionSourceOptionsCrossoversWidget);
   stackedWidget->addWidget(selectionSourceOptionsEdgesWidget);
   stackedWidget->addWidget(selectionSourceOptionsLatLongWidget);
   stackedWidget->addWidget(selectionSourceOptionsMetricWidget);
   stackedWidget->addWidget(selectionSourceOptionsPaintWidget);
   stackedWidget->addWidget(selectionSourceOptionsShapeWidget);
   
   return stackedWidget;
}
   
/**
 * called when shape threshold radio button clicked.
 */
void 
GuiSurfaceROINodeSelectionPage::slotShapeSelectionButtonGroup()
{
   if (shapeSelectionConnectedToNodeRadioButton->isChecked()) {
      theMainWindow->getBrainModelOpenGL()->setMouseMode(
            GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_SHAPE_NODE_SELECT);
   }
}
      

/**
 * called when metric threshold radio button clicked.
 */
void 
GuiSurfaceROINodeSelectionPage::slotMetricSelectionButtonGroup()
{
   if (metricSelectionConnectedToNodeRadioButton->isChecked()) {
      theMainWindow->getBrainModelOpenGL()->setMouseMode(
            GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_METRIC_NODE_SELECT);
   }
}
      
/**
 * called when border name from list pushbutton pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotSelectBorderNameFromListPushButton()
{
   GuiBorderNamesListBoxSelectionDialog bs(this,
                                           borderNameSelectionLabel->text());
   if (bs.exec() == QDialog::Accepted) {
      roiDialog->setBorderNameForQuery(bs.getSelectedText());
   }
}

/**
 * called when border select with mouse pushbutton pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotSelectBorderNameWithMousePushButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
      GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_BORDER_SELECT);
}
      
/**
 * called when paint name from list pushbutton pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotSelectPaintNameFromListPushButton()
{
   GuiPaintColumnNamesListBoxSelectionDialog pd(this,
                                                paintColumnSelectionComboBox->currentIndex());
   if (pd.exec() == QDialog::Accepted) {
      roiDialog->setPaintIndexForQuery(pd.getSelectedItemIndex());
   }      
}

/**
 * called when paint select node with mouse pushbutton pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotSelectPaintNameNodeWithMousePushButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
      GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_PAINT_INDEX_SELECT);
}

/**
 * called when a selection source is made.
 */
void 
GuiSurfaceROINodeSelectionPage::slotSelectionSourceComboBox(int item)
{
   const SELECTION_SOURCE selectionSource = 
      static_cast<SELECTION_SOURCE>(selectionSourceComboBox->itemData(item).toInt());
   
   switch (selectionSource) {
      case SELECTION_SOURCE_ALL_NODES:
         selectionSourceOptionsStackedWidget->setCurrentWidget(selectionSourceOptionsAllNodesWidget);
         break;
      case SELECTION_SOURCE_NODES_WITHIN_BORDER:
         selectionSourceOptionsStackedWidget->setCurrentWidget(selectionSourceOptionsBordersWidget);
         break;
      case SELECTION_SOURCE_NODES_WITH_CROSSOVERS:
         selectionSourceOptionsStackedWidget->setCurrentWidget(selectionSourceOptionsCrossoversWidget);
         break;
      case SELECTION_SOURCE_NODES_WITH_EDGES:
         selectionSourceOptionsStackedWidget->setCurrentWidget(selectionSourceOptionsEdgesWidget);
         break;
      case SELECTION_SOURCE_NODES_WITHIN_LATLON:
         selectionSourceOptionsStackedWidget->setCurrentWidget(selectionSourceOptionsLatLongWidget);
         break;
      case SELECTION_SOURCE_NODES_WITH_METRIC:
         selectionSourceOptionsStackedWidget->setCurrentWidget(selectionSourceOptionsMetricWidget);
         break;
      case SELECTION_SOURCE_NODES_WITH_PAINT:
         selectionSourceOptionsStackedWidget->setCurrentWidget(selectionSourceOptionsPaintWidget);
         break;
      case SELECTION_SOURCE_NODES_WITH_SHAPE:
         selectionSourceOptionsStackedWidget->setCurrentWidget(selectionSourceOptionsShapeWidget);
         break;
   }
}
      
/**
 * called when select nodes push button pressed.
 */
void GuiSurfaceROINodeSelectionPage::slotSelectedNodesPushButton()
{
   //
   // Get operational surface
   //
   const BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms == NULL) {
      QMessageBox::critical(this,
                            "ERROR",
                            "The operational surface is invalid.");
      return;
   }

   //
   // Get type of data used to select nodes
   //
   const int sourceIndex = selectionSourceComboBox->currentIndex();
   const SELECTION_SOURCE selectionSource = 
      static_cast<SELECTION_SOURCE>(selectionSourceComboBox->itemData(sourceIndex).toInt());
 
   //
   // Get logic for selection
   //
   const int logicIndex = selectionLogicComboBox->currentIndex();
   const BrainModelSurfaceROINodeSelection::SELECTION_LOGIC selectionLogic =
      static_cast<BrainModelSurfaceROINodeSelection::SELECTION_LOGIC>(
         selectionLogicComboBox->itemData(logicIndex).toInt());
      
   //
   // Get the ROI
   //
   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
   QString errorMessage;
   
   switch (selectionSource) {
      case SELECTION_SOURCE_ALL_NODES:
         errorMessage = roi->selectAllNodes(bms);
         break;
      case SELECTION_SOURCE_NODES_WITHIN_BORDER:
         {
            const BrainModelSurface* flatSurface =
               borderFlatSurfaceSelectionComboBox->getSelectedBrainModelSurface();
            if (flatSurface == NULL) {
               errorMessage = "There is no selected flat surface which is required for border queries.";
            }
            else {
               const QString selectedBorderName = borderNameSelectionLabel->text();
               if (selectedBorderName.isEmpty()) {
                  errorMessage = "There is no selected border name.";
               }
               else {
                  BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();

                  errorMessage = roi->selectNodesWithinBorder(selectionLogic,
                                                      bms,
                                                      flatSurface,
                                                      bmbs,
                                                      selectedBorderName);
               }
            }
         }
         break;
      case SELECTION_SOURCE_NODES_WITH_CROSSOVERS:
         errorMessage = roi->selectNodesThatAreCrossovers(selectionLogic,
                                                          bms);
         break;
      case SELECTION_SOURCE_NODES_WITH_EDGES:
         {
            errorMessage = roi->selectNodesThatAreEdges(selectionLogic, bms);

            //
            // Update Display Control
            //
            GuiFilesModified fm;
            theMainWindow->fileModificationUpdate(fm);
         }
         break;
      case SELECTION_SOURCE_NODES_WITHIN_LATLON:
         {
            const double minLat = latLowerRangeDoubleSpinBox->value();
            const double maxLat = latUpperRangeDoubleSpinBox->value();
            if (minLat > maxLat) {
               errorMessage = "Minimum Latitude is greater than Maximum Latitude";
            }
            const double minLon = lonLowerRangeDoubleSpinBox->value();
            const double maxLon = lonUpperRangeDoubleSpinBox->value();
             if (minLon > maxLon) {
               errorMessage = "Minimum Longitude is greater than Maximum Longitude";
            }
            
            if (errorMessage.isEmpty()) {
               const int columnNumber = 0;
               errorMessage = roi->selectNodesWithLatLong(selectionLogic,
                                                  bms,
                                                  theMainWindow->getBrainSet()->getLatLonFile(),
                                                  columnNumber,
                                                  minLat,
                                                  maxLat,
                                                  minLon,
                                                  maxLon);
            }
         }
         break;
      case SELECTION_SOURCE_NODES_WITH_METRIC:
         {
            MetricFile* metricFile = theMainWindow->getBrainSet()->getMetricFile();
            
            if (metricSelectionAllNodesRadioButton->isChecked()) {
               errorMessage = roi->selectNodesWithMetric(selectionLogic,
                                                 bms,
                                                 metricFile,
                                                 metricColumnSelectionComboBox->currentIndex(),
                                                 metricSelectionLowerDoubleSpinBox->value(),
                                                 metricSelectionUpperDoubleSpinBox->value());
            }
            else if (metricSelectionConnectedToNodeRadioButton->isChecked()) {
               const int metricNodeForQuery = metricSelectionNodeNumberLabel->text().toInt();
               if ((metricNodeForQuery < 0) ||
                   (metricNodeForQuery >= bms->getNumberOfNodes())) {
                  errorMessage = "Metric node number selected is invalid.";
               }
               else {
                  errorMessage = roi->selectConnectedNodesWithMetric(selectionLogic,
                                                             bms,
                                                             metricFile,
                                                             metricColumnSelectionComboBox->currentIndex(),
                                                             metricSelectionLowerDoubleSpinBox->value(),
                                                             metricSelectionUpperDoubleSpinBox->value(),
                                                             metricNodeForQuery);
               }
            }
            else {
               errorMessage = "Select either All Nodes or Node or Nodes Within \n"
                              "Range Connected to Node Selected with Mouse button.";
            }
         }
         break;
      case SELECTION_SOURCE_NODES_WITH_PAINT:
         {
            const QString paintName = paintNameSelectionLabel->text();
            if (paintName.isEmpty()) {
               errorMessage = "No paint name is selected.";
            }
            else {
               const PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
               errorMessage = roi->selectNodesWithPaint(selectionLogic,
                                                        bms,
                                                        pf,
                                                        paintColumnSelectionComboBox->currentIndex(),
                                                        paintName);
            }
         }
         break;
      case SELECTION_SOURCE_NODES_WITH_SHAPE:
         {
            SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
            
            if (shapeSelectionAllNodesRadioButton->isChecked()) {
               errorMessage = roi->selectNodesWithSurfaceShape(selectionLogic,
                                                 bms,
                                                 ssf,
                                                 shapeColumnSelectionComboBox->currentIndex(),
                                                 shapeSelectionLowerDoubleSpinBox->value(),
                                                 shapeSelectionUpperDoubleSpinBox->value());
            }
            else if (shapeSelectionConnectedToNodeRadioButton->isChecked()) {
               const int shapeNodeForQuery = shapeSelectionNodeNumberLabel->text().toInt();
               if ((shapeNodeForQuery < 0) ||
                   (shapeNodeForQuery >= bms->getNumberOfNodes())) {
                  errorMessage = "Surface Shape node number selected is invalid.";
               }
               else {
                  errorMessage = roi->selectConnectedNodesWithSurfaceShape(selectionLogic,
                                                             bms,
                                                             ssf,
                                                             shapeColumnSelectionComboBox->currentIndex(),
                                                             shapeSelectionLowerDoubleSpinBox->value(),
                                                             shapeSelectionUpperDoubleSpinBox->value(),
                                                             shapeNodeForQuery);
               }
            }
            else {
               errorMessage = "Select either All Nodes or Node or Nodes Within \n"
                              "Range Connected to Node Selected with Mouse button.";
            }
         }
         break;
   }
   
   updateNumberOfSelectedNodesLabel();
   slotShowSelectedNodesCheckBox(showSelectedNodesCheckBox->isChecked());
   
   QApplication::restoreOverrideCursor();

   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this, "ERROR", errorMessage);
   }
   
   //
   // Free surface copy
   //
   delete bms;
   bms = NULL;
   
   //
   // Update complete status
   //
   emit completeChanged();
}

/**
 * called when show selected nodes checkbox toggled.
 */
void 
GuiSurfaceROINodeSelectionPage::slotShowSelectedNodesCheckBox(bool on)
{
   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   roi->setDisplaySelectedNodes(on);
   
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
}
      
/**
 * update selected number of nodes label.
 */
void 
GuiSurfaceROINodeSelectionPage::updateNumberOfSelectedNodesLabel()
{
   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   numberOfNodesSelectedLabel->setNum(roi->getNumberOfNodesSelected());
}
      
/**
 * called when delselect nodes push button pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotDeselectNodesPushButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   roi->deselectAllNodes();
   updateNumberOfSelectedNodesLabel();
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
   QApplication::restoreOverrideCursor();
   
   //
   // Update complete status
   //
   emit completeChanged();
}

/**
 * called when remove islands button pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotRemoveIslands()
{
   //
   // Get operational surface
   //
   const BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms == NULL) {
      QMessageBox::critical(this,
                            "ERROR",
                            "The operational surface is invalid.");
      return;
   }
   
   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   const int numIslandsRemoved = roi->discardIslands(bms);                               
   updateNumberOfSelectedNodesLabel();
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
   QApplication::restoreOverrideCursor();
   
   //
   // Free copy of surface
   //
   delete bms;
   bms = NULL;
   
   QMessageBox::information(this,
                            "Remove Islands",
                            QString::number(numIslandsRemoved)
                               + " islands were removed.");

   //
   // Update complete status
   //
   emit completeChanged();
}
      
/**
 * called when invert push button pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotInvertNodesPushButton()
{
   //
   // Get operational surface
   //
   const BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms == NULL) {
      QMessageBox::critical(this,
                            "ERROR",
                            "The operational surface is invalid.");
      return;
   }
   

   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   roi->invertSelectedNodes(bms);
   updateNumberOfSelectedNodesLabel();
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
   QApplication::restoreOverrideCursor();
   
   //
   // Free copy of surface
   //
   delete bms;
   bms = NULL;
   
   //
   // Update complete status
   //
   emit completeChanged();
}

/**
 * called when load ROI push button pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotLoadROIPushButton()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setWindowTitle("Choose Region of Interest File Name");
   fd.setFilter(FileFilters::getRegionOfInterestFileFilter());
   fd.setFileMode(WuQFileDialog::ExistingFile);
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         const QString fileName = fd.selectedFiles().at(0);
         NodeRegionOfInterestFile roiFile;
         try {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            roiFile.readFile(fileName);
            BrainModelSurfaceROINodeSelection* roi = 
               theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
            roi->getRegionOfInterestFromFile(roiFile);

            updateNumberOfSelectedNodesLabel();
            theMainWindow->getBrainSet()->clearAllDisplayLists();
            GuiBrainModelOpenGL::updateAllGL(NULL);
            QApplication::restoreOverrideCursor();
         }
         catch (FileException& e) {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "ERROR", e.whatQString());
         }
      }
   }
   
   //
   // Update complete status
   //
   emit completeChanged();
}

/**
 * called when save ROI push button pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotSaveROIPushButton()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(WuQFileDialog::AcceptSave);
   fd.setWindowTitle("Choose Region of Interest File Name");
   fd.setFilter(FileFilters::getRegionOfInterestFileFilter());
   fd.setFileMode(WuQFileDialog::AnyFile);
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         const QString fileName = fd.selectedFiles().at(0);
         NodeRegionOfInterestFile roiFile;
         BrainModelSurfaceROINodeSelection* roi = 
            theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
         roi->setRegionOfInterestIntoFile(roiFile);
         try {
            roiFile.writeFile(fileName);
         }
         catch (FileException& e) {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "ERROR", e.whatQString());
            return;
         }
         QApplication::restoreOverrideCursor();
      }
   }
}

/**
 * called when dilate ROI push button pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotDilateROIPushButton()
{
   //
   // Get operational surface
   //
   const BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms == NULL) {
      QMessageBox::critical(this,
                            "ERROR",
                            "The operational surface is invalid.");
      return;
   }
   

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   roi->dilate(bms, 1);
   updateNumberOfSelectedNodesLabel();
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
   QApplication::restoreOverrideCursor();
   
   //
   // Free copy of surface
   //
   delete bms;
   bms = NULL;
   
   //
   // Update complete status
   //
   emit completeChanged();
}

/**
 * called when erode ROI push button pressed.
 */
void 
GuiSurfaceROINodeSelectionPage::slotErodeROIPushButton()
{
   //
   // Get operational surface
   //
   const BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms == NULL) {
      QMessageBox::critical(this,
                            "ERROR",
                            "The operational surface is invalid.");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   roi->erode(bms, 1);
   updateNumberOfSelectedNodesLabel();
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
   QApplication::restoreOverrideCursor();
   
   //
   // Free the copy of the surface
   //
   delete bms;
   bms = NULL;
   
   //
   // Update complete status
   //
   emit completeChanged();
}

/**
 * clean up the page.
 */
void 
GuiSurfaceROINodeSelectionPage::cleanupPage()
{
   updateNumberOfSelectedNodesLabel();
}
      
/**
 * initialize the page.
 */
void 
GuiSurfaceROINodeSelectionPage::initializePage()
{
   updateNumberOfSelectedNodesLabel();
}

/**
 * update the page.
 */
void 
GuiSurfaceROINodeSelectionPage::updatePage()
{
   borderFlatSurfaceSelectionComboBox->updateComboBox();
   metricColumnSelectionComboBox->updateComboBox();
   paintColumnSelectionComboBox->updateComboBox();
   shapeColumnSelectionComboBox->updateComboBox();
   updateNumberOfSelectedNodesLabel();
}

/**
 * validate the page.
 */
bool 
GuiSurfaceROINodeSelectionPage::validatePage()
{
   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   if (roi->anyNodesSelected()) {
      return true;
   }
   
   QMessageBox::critical(this,
                         "ERROR",
                         "No Nodes Selected\n"
                               "Did you forget to press the \"Select Nodes\" button?");
   return false;
}

/**
 * is page complete.
 */
bool 
GuiSurfaceROINodeSelectionPage::isComplete()
{
   BrainModelSurfaceROINodeSelection* roi = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   if (roi->anyNodesSelected()) {
      return true;
   }

   return false;
}

//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
GuiSurfaceROISurfaceAndTopologySelectionPage::GuiSurfaceROISurfaceAndTopologySelectionPage(
                                           GuiSurfaceRegionOfInterestDialog* roiDialogIn)
{
   roiDialog = roiDialogIn;
   setTitle("Surface and Topology Selection");
   
   //
   // Surface selection
   //
   QLabel* surfaceSelectionLabel = new QLabel("Surface");
   operationSurfaceSelectionComboBox = 
      new GuiBrainModelSelectionComboBox(GuiBrainModelSelectionComboBox::OPTION_SHOW_SURFACES_ALL);
   QObject::connect(operationSurfaceSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotOperationSurfaceSelectionComboBox()));
                     
   //
   // Topology Selection
   //
   QLabel* topologySelectionLabel = new QLabel("Topology");
   operationTopologyComboBox = new GuiTopologyFileComboBox;
   QObject::connect(operationTopologyComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotOperationTopologyComboBox()));
                    
   //
   // Layout the widgets
   //
   QGridLayout* surfaceTopologyLayout = new QGridLayout;
   surfaceTopologyLayout->setColumnStretch(0, 0);
   surfaceTopologyLayout->setColumnStretch(100, 0);
   surfaceTopologyLayout->addWidget(surfaceSelectionLabel, 0, 0);
   surfaceTopologyLayout->addWidget(operationSurfaceSelectionComboBox, 0, 1);
   surfaceTopologyLayout->addWidget(topologySelectionLabel, 1, 0);
   surfaceTopologyLayout->addWidget(operationTopologyComboBox, 1, 1);
   QVBoxLayout* pageLayout = new QVBoxLayout(this);
   pageLayout->addLayout(surfaceTopologyLayout);
   pageLayout->addStretch();
}

/**
 * destructor.
 */
GuiSurfaceROISurfaceAndTopologySelectionPage::~GuiSurfaceROISurfaceAndTopologySelectionPage()
{
}

/**
 * clean up the page.
 */
void 
GuiSurfaceROISurfaceAndTopologySelectionPage::cleanupPage()
{
}

/**
 * initialize the page.
 */
void 
GuiSurfaceROISurfaceAndTopologySelectionPage::initializePage()
{
   updatePage();
}

/**
 * validate the page.
 */
bool 
GuiSurfaceROISurfaceAndTopologySelectionPage::validatePage()
{
   return isComplete();
}

/**
 * is page complete.
 */
bool 
GuiSurfaceROISurfaceAndTopologySelectionPage::isComplete()
{
   if ((operationSurfaceSelectionComboBox->getSelectedBrainModelSurface() != NULL) &&
       (operationTopologyComboBox->getSelectedTopologyFile() != NULL)) {
      return true;
   }
   
   return false;
}

/**
 * update the page.
 */
void 
GuiSurfaceROISurfaceAndTopologySelectionPage::updatePage()
{
   operationSurfaceSelectionComboBox->updateComboBox();
   operationTopologyComboBox->updateComboBox();
   slotOperationSurfaceSelectionComboBox();
}

/**
 * get the operation coordinate file.
 */
CoordinateFile* 
GuiSurfaceROISurfaceAndTopologySelectionPage::getOperationCoordinateFile()
{
   BrainModelSurface* bms = 
      operationSurfaceSelectionComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      return bms->getCoordinateFile();
   }
   return NULL;
}
      
/**
 * get a COPY of operation surface with operation topology (user must delete object).
 */
BrainModelSurface* 
GuiSurfaceROISurfaceAndTopologySelectionPage::getCopyOfOperationSurface()
{
   BrainModelSurface* bms = 
      operationSurfaceSelectionComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      TopologyFile* tf = operationTopologyComboBox->getSelectedTopologyFile();
      if (tf != NULL) {
         BrainModelSurface* bmsCopy = new BrainModelSurface(*bms);
         bmsCopy->setTopologyFile(tf);
         return bmsCopy;
      }
   }
   
   return NULL;
}

/**
 * called when a surface is selected.
 */
void 
GuiSurfaceROISurfaceAndTopologySelectionPage::slotOperationSurfaceSelectionComboBox()
{
   const BrainModelSurface* bms = 
      operationSurfaceSelectionComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      const TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
         operationTopologyComboBox->setSelectedTopologyFile(tf);
      }
   }
}

/**
 * called when topology is selected.
 */
void 
GuiSurfaceROISurfaceAndTopologySelectionPage::slotOperationTopologyComboBox()
{
}

//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
GuiSurfaceROIOperationPage::GuiSurfaceROIOperationPage(GuiSurfaceRegionOfInterestDialog* roiDialogIn,
                                                       GuiSurfaceROIReportPage* reportPageIn)
{
   roiDialog = roiDialogIn;
   reportPage = reportPageIn;
   setTitle("Region of Interest Operation");
   
   //
   // Operation selection combo box
   //
   operationSelectionComboBox = new QComboBox;
   QObject::connect(operationSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotOperationSelectionComboBox(int)));
   
   //
   // Load operation
   //
   operationSelectionComboBox->addItem("Assign Metric",
                                       OPERATION_MODE_ASSIGN_METRIC);
   operationSelectionComboBox->addItem("Assign Paint",
                                       OPERATION_MODE_ASSIGN_PAINT);
   operationSelectionComboBox->addItem("Assign Surface Shape",
                                       OPERATION_MODE_ASSIGN_SURFACE_SHAPE);
   operationSelectionComboBox->addItem("Compute Integrated Folding Index",
                                       OPERATION_MODE_COMPUTE_INTEGRATED_FOLDING_INDEX);
   operationSelectionComboBox->addItem("Create Areal Borders From Clusters",
                                       OPERATION_MODE_CREATE_BORDERS_FROM_CLUSTERS);
   operationSelectionComboBox->addItem("Create Linear Borders From Region of Interest",
                                       OPERATION_MODE_CREATE_BORDERS_FROM_ROI);
   operationSelectionComboBox->addItem("Create Volume Region of Interest",
                                       OPERATION_MODE_CREATE_VOLUME_ROI);
   operationSelectionComboBox->addItem("Disconnect Nodes",
                                       OPERATION_MODE_DISCONNECT_NODES);
   operationSelectionComboBox->addItem("Folding Measurements",
                                       OPERATION_MODE_FOLDING_MEASUREMENTS);
   operationSelectionComboBox->addItem("Geodesic Distance",
                                       OPERATION_MODE_GEODESIC);
   operationSelectionComboBox->addItem("Metric Clusters Analysis",
                                       OPERATION_MODE_METRIC_CLUSTER_ANALYSIS);
   operationSelectionComboBox->addItem("Probabilistic Atlas Overlap",
                                       OPERATION_MODE_PROB_ATLAS_OVERLAP);
   operationSelectionComboBox->addItem("Smoothing",
                                       OPERATION_MODE_SMOOTH_NODES);
   operationSelectionComboBox->addItem("Statistical Paint Report",
                                       OPERATION_MODE_STATISTICAL_PAINT_REPORT);
   operationSelectionComboBox->addItem("Statistical Report",
                                       OPERATION_MODE_STATISTICAL_REPORT);
   operationSelectionComboBox->addItem("Surface Shape Correlation Report",
                                       OPERATION_MODE_SHAPE_CORRELATION);
   operationSelectionComboBox->addItem("Surface Shape Clusters Analysis",
                                       OPERATION_MODE_SHAPE_CLUSTER_ANALYSIS);
   
   //
   // Group box for operation selection
   //
   QGroupBox* operationSelectionGroupBox = new QGroupBox("Operation Selection");
   QHBoxLayout* operationSelectionLayout = new QHBoxLayout(operationSelectionGroupBox);
   operationSelectionLayout->addWidget(operationSelectionComboBox);
   operationSelectionLayout->addStretch();

   //
   // the assign metric page
   //
   assignMetricPage = createAssignMetricPage();
   
   // the assign paint page
   assignPaintPage = createAssignPaintPage();
   
   // the assign shape page
   assignShapePage = createAssignShapePage();
   
   // the compute integrated folding index page
   computeIntegratedFoldingIndexPage = createComputeIntegratedFoldingIndexPage();
   
   // the folding measurements page
   foldingMeasurementsPage = createFoldingMeasurementsPage();
   
   // the create borders from clusters page
   createArealBordersPage = createCreateArealBordersPage();
   
   // the create linear borders page
   linearBordersPage = createLinearBordersPage();
   
   // the create volume roi page
   createVolumeROIPage = createCreateVolumeROIPage();
   
   // the disconnect nodes page
   disconnectNodesPage = createDisconnectNodesPage();
   
   // the geodesic page
   geodesicPage = createGeodesicPage();
   
   // the metric cluster analysis page
   metricClusterAnalysisPage = createMetricClusterAnalysisPage();
   
   // the probabilistic atlas overlap page
   probAtlasOverlapPage = createProbAtlasOverlapPage();
   
   // the smoothing page
   smoothingPage = createSmoothingPage();
   
   // the statistical paint report page
   paintReportPage = createPaintReportPage();
   
   // the statistical report page
   statisticalReportPage = createStatisticalReportPage();
   
   // the shape correlation page
   shapeCorrelationPage = createShapeCorrelationPage();
   
   // the shape cluster analysis page
   shapeClusterAnalysisPage = createShapeClusterAnalysisPage();

   //
   // Operation stacked widget
   //
   operationsStackedWidget = new QStackedWidget;
   operationsStackedWidget->addWidget(assignMetricPage);
   operationsStackedWidget->addWidget(assignPaintPage);
   operationsStackedWidget->addWidget(assignShapePage);
   operationsStackedWidget->addWidget(computeIntegratedFoldingIndexPage);
   operationsStackedWidget->addWidget(createArealBordersPage);
   operationsStackedWidget->addWidget(foldingMeasurementsPage);
   operationsStackedWidget->addWidget(linearBordersPage);
   operationsStackedWidget->addWidget(createVolumeROIPage);
   operationsStackedWidget->addWidget(disconnectNodesPage);
   operationsStackedWidget->addWidget(geodesicPage);
   operationsStackedWidget->addWidget(metricClusterAnalysisPage);
   operationsStackedWidget->addWidget(probAtlasOverlapPage);
   operationsStackedWidget->addWidget(smoothingPage);
   operationsStackedWidget->addWidget(paintReportPage);
   operationsStackedWidget->addWidget(statisticalReportPage);
   operationsStackedWidget->addWidget(shapeCorrelationPage);
   operationsStackedWidget->addWidget(shapeClusterAnalysisPage);
   
   //
   // Group box for operation parameters
   //
   QGroupBox* operationParametersGroupBox = new QGroupBox("Operation Parameters");
   QVBoxLayout* operationParametersLayout = new QVBoxLayout(operationParametersGroupBox);
   operationParametersLayout->addWidget(operationsStackedWidget);
   operationParametersLayout->addStretch();
   
   //
   // Layout for widget
   //
   QVBoxLayout* pageLayout = new QVBoxLayout(this);
   pageLayout->addWidget(operationSelectionGroupBox);
   pageLayout->addWidget(operationParametersGroupBox);
   pageLayout->addStretch();
   
}

/**
 * destructor.
 */
GuiSurfaceROIOperationPage::~GuiSurfaceROIOperationPage()
{
}

/**
 * clean up the page.
 */
void 
GuiSurfaceROIOperationPage::cleanupPage()
{
   updatePage();
}

/**
 * initialize the page.
 */
void 
GuiSurfaceROIOperationPage::initializePage()
{
   updatePage();
}

/**
 * validate the page.
 */
bool 
GuiSurfaceROIOperationPage::validatePage()
{
   return true;
}

/**
 * is page complete.
 */
bool 
GuiSurfaceROIOperationPage::isComplete()
{
   return true;
}

/**
 * update the page.
 */
void 
GuiSurfaceROIOperationPage::updatePage()
{
   assignMetricColumnSelectionComboBox->updateComboBox();
   assignMetricColumnNameLineEdit->setText(assignMetricColumnSelectionComboBox->currentText());

   assignPaintColumnSelectionComboBox->updateComboBox();
   assignPaintColumnNameLineEdit->setText(assignPaintColumnSelectionComboBox->currentText());

   assignShapeColumnSelectionComboBox->updateComboBox();
   assignShapeColumnNameLineEdit->setText(assignShapeColumnSelectionComboBox->currentText());

   geodesicMetricColumnComboBox->updateComboBox();
   geodesicMetricColumnNameLineEdit->setText(geodesicMetricColumnComboBox->currentText());
   
   geodesicDistanceColumnComboBox->updateComboBox();
   geodesicDistanceColumnNameLineEdit->setText(geodesicDistanceColumnComboBox->currentText());
   
   paintRegionReportColumnComboBox->updateComboBox();
   
   statisticalReportDistortionCorrectionMetricColumnComboBox->updateComboBox();
   
   paintRegionReportDistortionCorrectionMetricColumnComboBox->updateComboBox();

   shapeCorrelationColumnComboBox->updateComboBox();
   
   shapeClusterMetricArealDistortionComboBox->updateComboBox();
   metricClusterMetricArealDistortionComboBox->updateComboBox();
}

/**
 * called when an operation is selected.
 */
void 
GuiSurfaceROIOperationPage::slotOperationSelectionComboBox(int indx)
{
   const OPERATION_MODE mode = 
      static_cast<OPERATION_MODE>(operationSelectionComboBox->itemData(indx).toInt());
      
   switch (mode) {
      case OPERATION_MODE_ASSIGN_METRIC:
         operationsStackedWidget->setCurrentWidget(assignMetricPage);
         break;
      case OPERATION_MODE_ASSIGN_PAINT:
         operationsStackedWidget->setCurrentWidget(assignPaintPage);
         break;
      case OPERATION_MODE_ASSIGN_SURFACE_SHAPE:
         operationsStackedWidget->setCurrentWidget(assignShapePage);
         break;
      case OPERATION_MODE_COMPUTE_INTEGRATED_FOLDING_INDEX:
         operationsStackedWidget->setCurrentWidget(computeIntegratedFoldingIndexPage);
         break;
      case OPERATION_MODE_CREATE_BORDERS_FROM_CLUSTERS:
         operationsStackedWidget->setCurrentWidget(createArealBordersPage);
         break;
      case OPERATION_MODE_CREATE_BORDERS_FROM_ROI:
         operationsStackedWidget->setCurrentWidget(linearBordersPage);
         break;
      case OPERATION_MODE_CREATE_VOLUME_ROI:
         operationsStackedWidget->setCurrentWidget(createVolumeROIPage);
         break;
      case OPERATION_MODE_DISCONNECT_NODES:
         operationsStackedWidget->setCurrentWidget(disconnectNodesPage);
         break;
      case OPERATION_MODE_FOLDING_MEASUREMENTS:
         operationsStackedWidget->setCurrentWidget(foldingMeasurementsPage);
         break;
      case OPERATION_MODE_GEODESIC:
         operationsStackedWidget->setCurrentWidget(geodesicPage);
         break;
      case OPERATION_MODE_METRIC_CLUSTER_ANALYSIS:
         operationsStackedWidget->setCurrentWidget(metricClusterAnalysisPage);
         break;
      case OPERATION_MODE_PROB_ATLAS_OVERLAP:
         operationsStackedWidget->setCurrentWidget(probAtlasOverlapPage);
         break;
      case OPERATION_MODE_SMOOTH_NODES:
         operationsStackedWidget->setCurrentWidget(smoothingPage);
         break;
      case OPERATION_MODE_STATISTICAL_PAINT_REPORT:
         operationsStackedWidget->setCurrentWidget(paintReportPage);
         break;
      case OPERATION_MODE_STATISTICAL_REPORT:
         operationsStackedWidget->setCurrentWidget(statisticalReportPage);
         break;
      case OPERATION_MODE_SHAPE_CORRELATION:
         operationsStackedWidget->setCurrentWidget(shapeCorrelationPage);
         break;
      case OPERATION_MODE_SHAPE_CLUSTER_ANALYSIS:
         operationsStackedWidget->setCurrentWidget(shapeClusterAnalysisPage);
         break;
   }
}

/**
 * create the assign metric page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createAssignMetricPage()
{
   //
   // Column selection
   //
   QLabel* metricLabel = new QLabel("Metric Column Selection");
   assignMetricColumnSelectionComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_METRIC,
                                                  true,
                                                  false,
                                                  false);
   
   // 
   // Edit Column name
   //
   QLabel* editLabel = new QLabel("Edit Column Name");
   assignMetricColumnNameLineEdit = new QLineEdit;
   QObject::connect(assignMetricColumnSelectionComboBox, SIGNAL(itemNameSelected(const QString&)),
                    assignMetricColumnNameLineEdit, SLOT(setText(const QString&)));

   //
   // New metric value
   //
   QLabel* newValueLabel = new QLabel("Assign Value");
   assignMetricValueDoubleSpinBox = new QDoubleSpinBox;
   assignMetricValueDoubleSpinBox->setMinimum(-std::numeric_limits<float>::max());
   assignMetricValueDoubleSpinBox->setMaximum(std::numeric_limits<float>::max());
   assignMetricValueDoubleSpinBox->setDecimals(4);
   assignMetricValueDoubleSpinBox->setSingleStep(1.0);
   
   //
   // Assign metric button
   //
   QPushButton* assignMetricPushButton = new QPushButton("Assign Metric");
   assignMetricPushButton->setAutoDefault(false);
   assignMetricPushButton->setFixedSize(assignMetricPushButton->sizeHint());
   QObject::connect(assignMetricPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAssignMetricPushButton()));
                    
   //
   // Layout widgets
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(metricLabel, 0, 0);
   gridLayout->addWidget(assignMetricColumnSelectionComboBox, 0, 1);
   gridLayout->addWidget(editLabel, 1, 0);
   gridLayout->addWidget(assignMetricColumnNameLineEdit, 1, 1);
   gridLayout->addWidget(newValueLabel, 2, 0);
   gridLayout->addWidget(assignMetricValueDoubleSpinBox, 2, 1);
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(gridLayout);
   layout->addWidget(assignMetricPushButton);
   layout->addStretch();

   return w;
}

/**
 * called to assign metric.
 */
void 
GuiSurfaceROIOperationPage::slotAssignMetricPushButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   QString errorMessage;

   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   int metricColumn = assignMetricColumnSelectionComboBox->currentIndex();

   try {
      BrainModelSurfaceROINodeSelection* roi = 
         theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();

      BrainModelSurfaceROIAssignMetric metricROI(theMainWindow->getBrainSet(),
                                                 bms,
                                                 roi,
                                                 mf,
                                                 metricColumn,
                                                 assignMetricColumnNameLineEdit->text(),
                                                 assignMetricValueDoubleSpinBox->value());
      metricROI.execute();
      metricColumn = metricROI.getAssignedMetricColumn();
   }
   catch (BrainModelAlgorithmException& e) {
      errorMessage = e.whatQString();
   }
   
   delete bms;
   
   //
   // Metric File has changed
   //
   GuiFilesModified fm;
   fm.setMetricModified();
   theMainWindow->fileModificationUpdate(fm);

   //
   // Update node colors and redraw
   //
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
   
   //
   // Save assigned column since it may be new
   //
   assignMetricColumnSelectionComboBox->setCurrentIndex(metricColumn);
   assignMetricColumnNameLineEdit->setText(assignMetricColumnSelectionComboBox->currentText());
   
   QApplication::restoreOverrideCursor();
   
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this,
                            "ERROR",
                            errorMessage);
   }                            
}      

/**
 * create the assign paint page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createAssignPaintPage()
{
   //
   // Column selection
   //
   QLabel* paintLabel = new QLabel("Paint Column Selection");
   assignPaintColumnSelectionComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_PAINT,
                                                  true,
                                                  false,
                                                  false);
   
   // 
   // Edit Column name
   //
   QLabel* editLabel = new QLabel("Edit Column Name");
   assignPaintColumnNameLineEdit = new QLineEdit;
   QObject::connect(assignPaintColumnSelectionComboBox, SIGNAL(itemNameSelected(const QString&)),
                    assignPaintColumnNameLineEdit, SLOT(setText(const QString&)));

   //
   // New paint name
   //
   QLabel* newNameLabel = new QLabel("Assign Name");
   assignPaintNameLineEdit = new QLineEdit;
   
   //
   // Assign paint button
   //
   QPushButton* assignPaintPushButton = new QPushButton("Assign Paint");
   assignPaintPushButton->setAutoDefault(false);
   assignPaintPushButton->setFixedSize(assignPaintPushButton->sizeHint());
   QObject::connect(assignPaintPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAssignPaintPushButton()));
                    
   //
   // Layout widgets
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(paintLabel, 0, 0);
   gridLayout->addWidget(assignPaintColumnSelectionComboBox, 0, 1);
   gridLayout->addWidget(editLabel, 1, 0);
   gridLayout->addWidget(assignPaintColumnNameLineEdit, 1, 1);
   gridLayout->addWidget(newNameLabel, 2, 0);
   gridLayout->addWidget(assignPaintNameLineEdit, 2, 1);
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(gridLayout);
   layout->addWidget(assignPaintPushButton);
   layout->addStretch();

   return w;
}

/**
 * called to assign paint.
 */
void 
GuiSurfaceROIOperationPage::slotAssignPaintPushButton()
{
   const QString paintName = assignPaintNameLineEdit->text();
   if (paintName.isEmpty()) {
      QMessageBox::critical(this, 
                            "ERROR",
                            "Paint name is empty.");
   }
   
   AreaColorFile* cf = theMainWindow->getBrainSet()->getAreaColorFile();
   bool areaColorMatch = false;
   int areaColorIndex = cf->getColorIndexByName(paintName, areaColorMatch);
   
   // area color may need to be created
   //
   bool createAreaColor = false;
   if ((areaColorIndex >= 0) && (areaColorMatch == true)) {
      createAreaColor = false;
   }
   else if ((areaColorIndex >= 0) && (areaColorMatch == false)) {
      QString msg("Use area color \"");
      msg.append(cf->getColorNameByIndex(areaColorIndex));
      msg.append("\" for paint ");
      msg.append(paintName);
      msg.append(" ?");
      QString noButton("No, define color ");
      noButton.append(paintName);
      QMessageBox msgBox(this);
      msgBox.setWindowTitle("Use Partially Matching Color");
      msgBox.setText(msg);
      msgBox.addButton("Yes", QMessageBox::YesRole);
      QPushButton* noPushButton = msgBox.addButton(noButton, QMessageBox::NoRole);
      msgBox.exec();
      if (msgBox.clickedButton() == noPushButton) {
         createAreaColor = true;
      }
   }
   else {
      createAreaColor = true;
   }
   
   if (createAreaColor) {
      QString title("Create Area Color: ");
      title.append(paintName);
      GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this,
                                                                 title, 
                                                                 false,
                                                                 false, 
                                                                 false, 
                                                                 false);
      csd->exec();
      
      //
      // Add new area color
      //
      float pointSize = 2.0, lineSize = 1.0;
      unsigned char r, g, b, a;
      ColorFile::ColorStorage::SYMBOL symbol;
      csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
      cf->addColor(paintName, r, g, b, a, pointSize, lineSize, symbol);
      areaColorIndex = cf->getNumberOfColors() - 1;   
   }

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   QString errorMessage;

   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   int paintColumn = assignPaintColumnSelectionComboBox->currentIndex();
   
   try {
      BrainModelSurfaceROINodeSelection* roi = 
         theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();

      BrainModelSurfaceROIAssignPaint paintROI(theMainWindow->getBrainSet(),
                                                 bms,
                                                 roi,
                                                 pf,
                                                 paintColumn,
                                                 assignPaintColumnNameLineEdit->text(),
                                                 paintName);
      paintROI.execute();
      paintColumn = paintROI.getAssignedPaintColumn();
   }
   catch (BrainModelAlgorithmException& e) {
      errorMessage = e.whatQString();
   }
   
   delete bms;
   
   //
   // Paint File has changed
   //
   GuiFilesModified fm;
   fm.setPaintModified();
   theMainWindow->fileModificationUpdate(fm);

   //
   // Update node colors and redraw
   //
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
   
   //
   // Save assigned column since it may be new
   //
   assignPaintColumnSelectionComboBox->setCurrentIndex(paintColumn);
   assignPaintColumnNameLineEdit->setText(assignPaintColumnSelectionComboBox->currentText());
   
   QApplication::restoreOverrideCursor();
   
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this,
                            "ERROR",
                            errorMessage);
   }                            
}      

/**
 * create the assign shape page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createAssignShapePage()
{
   //
   // Column selection
   //
   QLabel* shapeLabel = new QLabel("Shape Column Selection");
   assignShapeColumnSelectionComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                                  true,
                                                  false,
                                                  false);
   
   // 
   // Edit Column name
   //
   QLabel* editLabel = new QLabel("Edit Column Name");
   assignShapeColumnNameLineEdit = new QLineEdit;
   QObject::connect(assignShapeColumnSelectionComboBox, SIGNAL(itemNameSelected(const QString&)),
                    assignShapeColumnNameLineEdit, SLOT(setText(const QString&)));

   //
   // New shape value
   //
   QLabel* newValueLabel = new QLabel("Assign Value");
   assignShapeValueDoubleSpinBox = new QDoubleSpinBox;
   assignShapeValueDoubleSpinBox->setMinimum(-std::numeric_limits<float>::max());
   assignShapeValueDoubleSpinBox->setMaximum(std::numeric_limits<float>::max());
   assignShapeValueDoubleSpinBox->setDecimals(4);
   assignShapeValueDoubleSpinBox->setSingleStep(1.0);
   
   //
   // Assign shape button
   //
   QPushButton* assignShapePushButton = new QPushButton("Assign Shape");
   assignShapePushButton->setAutoDefault(false);
   assignShapePushButton->setFixedSize(assignShapePushButton->sizeHint());
   QObject::connect(assignShapePushButton, SIGNAL(clicked()),
                    this, SLOT(slotAssignShapePushButton()));
                    
   //
   // Layout widgets
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(shapeLabel, 0, 0);
   gridLayout->addWidget(assignShapeColumnSelectionComboBox, 0, 1);
   gridLayout->addWidget(editLabel, 1, 0);
   gridLayout->addWidget(assignShapeColumnNameLineEdit, 1, 1);
   gridLayout->addWidget(newValueLabel, 2, 0);
   gridLayout->addWidget(assignShapeValueDoubleSpinBox, 2, 1);
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(gridLayout);
   layout->addWidget(assignShapePushButton);
   layout->addStretch();

   return w;
}

/**
 * called to assign metric.
 */
void 
GuiSurfaceROIOperationPage::slotAssignShapePushButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   QString errorMessage;

   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   int shapeColumn = assignShapeColumnSelectionComboBox->currentIndex();

   try {
      BrainModelSurfaceROINodeSelection* roi = 
         theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();

      BrainModelSurfaceROIAssignShape shapeROI(theMainWindow->getBrainSet(),
                                                 bms,
                                                 roi,
                                                 ssf,
                                                 shapeColumn,
                                                 assignShapeColumnNameLineEdit->text(),
                                                 assignShapeValueDoubleSpinBox->value());
      shapeROI.execute();
      shapeColumn = shapeROI.getAssignedShapeColumn();
   }
   catch (BrainModelAlgorithmException& e) {
      errorMessage = e.whatQString();
   }
   
   delete bms;
   
   //
   // Shape File has changed
   //
   GuiFilesModified fm;
   fm.setSurfaceShapeModified();
   theMainWindow->fileModificationUpdate(fm);

   //
   // Update node colors and redraw
   //
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
   
   //
   // Save assigned column since it may be new
   //
   assignShapeColumnSelectionComboBox->setCurrentIndex(shapeColumn);
   assignShapeColumnNameLineEdit->setText(assignShapeColumnSelectionComboBox->currentText());
   
   QApplication::restoreOverrideCursor();
   
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this,
                            "ERROR",
                            errorMessage);
   }                            
}      

/**
 * create the compute integrated folding index page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createComputeIntegratedFoldingIndexPage()
{
   semicolonSeparateFoldingIndexReportCheckBox = 
      new QCheckBox("Semicolon Separate Integrated Folding Report");
      
   QPushButton* computePushButton = new QPushButton("Compute Integrated Folding Index");
   computePushButton->setFixedSize(computePushButton->sizeHint());
   computePushButton->setAutoDefault(false);
   QObject::connect(computePushButton, SIGNAL(clicked()),
                    this, SLOT(slotComputeIntegratedFoldingIndex()));

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(semicolonSeparateFoldingIndexReportCheckBox);
   layout->addWidget(computePushButton);

   return w;
}

/**
 * called to run compute integrated folding index.
 */
void 
GuiSurfaceROIOperationPage::slotComputeIntegratedFoldingIndex()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   QString errorMessage;

   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();

   try {
      BrainModelSurfaceROINodeSelection* roi = 
         theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();

      QString headerText("Integrated Folding Index");
      BrainModelSurfaceROIIntegratedFoldingIndexReport ifi(
                               theMainWindow->getBrainSet(),
                               bms,
                               roi,
                               ssf,
                               headerText,
                               semicolonSeparateFoldingIndexReportCheckBox->isChecked());
      ifi.execute();
   
      //
      // set report text
      //
      reportPage->setReportText(ifi.getReportText());
   }
   catch (BrainModelAlgorithmException& e) {
      errorMessage = e.whatQString();
   }
   
   delete bms;
   
   QApplication::restoreOverrideCursor();
   
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this,
                            "ERROR",
                            errorMessage);
   }      
   
   //
   // Move to next page
   //
   roiDialog->next();
}

/**
 * create the folding measurements page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createFoldingMeasurementsPage()
{
   QPushButton* computePushButton = new QPushButton("Compute Folding Measurements");
   computePushButton->setFixedSize(computePushButton->sizeHint());
   computePushButton->setAutoDefault(false);
   QObject::connect(computePushButton, SIGNAL(clicked()),
                    this, SLOT(slotFoldingMeasurements()));

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(computePushButton);

   return w;
}

/**
 * called to run the folding measurements.
 */
void 
GuiSurfaceROIOperationPage::slotFoldingMeasurements()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   QString errorMessage;

   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();

   try {
      BrainModelSurfaceROINodeSelection* roi = 
         theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();

      QString headerText("Folding Measures");
      BrainModelSurfaceROIFoldingMeasurementReport fr(
                               theMainWindow->getBrainSet(),
                               bms,
                               roi,
                               headerText,
                               false,
                               NULL);
      fr.execute();
   
      //
      // set report text
      //
      reportPage->setReportText(fr.getReportText());
   }
   catch (BrainModelAlgorithmException& e) {
      errorMessage = e.whatQString();
   }
   
   delete bms;
   
   QApplication::restoreOverrideCursor();
   
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this,
                            "ERROR",
                            errorMessage);
   }      
   
   //
   // Move to next page
   //
   roiDialog->next();
}

/**
 * create the create borders from clusters page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createCreateArealBordersPage()
{
   //
   // border to assign
   //
   QPushButton* clusterNamePushButton = new QPushButton("Border Name...");
   clusterNamePushButton->setAutoDefault(false);
   clusterNamePushButton->setFixedSize(clusterNamePushButton->sizeHint());
   QObject::connect(clusterNamePushButton, SIGNAL(clicked()),
                    this, SLOT(slotArealBorderClusterNamePushButton()));
   clusterArealBorderNameLineEdit = new QLineEdit;
   QHBoxLayout* borderNameLayout = new QHBoxLayout;
   borderNameLayout->addWidget(clusterNamePushButton);
   borderNameLayout->addWidget(clusterArealBorderNameLineEdit);
   borderNameLayout->setStretchFactor(clusterNamePushButton, 0);
   borderNameLayout->setStretchFactor(clusterArealBorderNameLineEdit, 100);
   
   //
   // Auto project borders
   //
   clusterArealBorderAutoProjectCheckBox = new QCheckBox("Auto Project");
   clusterArealBorderAutoProjectCheckBox->setChecked(true);
   
   //
   // create borders push button
   //
   QPushButton* createBordersPushButton = new QPushButton("Create Borders Around Clusters");
   createBordersPushButton->setFixedSize(createBordersPushButton->sizeHint());
   createBordersPushButton->setAutoDefault(false);
   QObject::connect(createBordersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateArealBordersFromClusters()));

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(borderNameLayout);
   layout->addWidget(clusterArealBorderAutoProjectCheckBox);
   layout->addWidget(createBordersPushButton);
   layout->addStretch();

   return w;
}

/**
 * Called to set create borders from clusters name.
 */
void 
GuiSurfaceROIOperationPage::slotArealBorderClusterNamePushButton()
{
   GuiNameSelectionDialog nsd(this);
   if (nsd.exec() == QDialog::Accepted) {
      clusterArealBorderNameLineEdit->setText(nsd.getNameSelected());
   }
}

/**
 * Called run create borders from clusters.
 */ 
void 
GuiSurfaceROIOperationPage::slotCreateArealBordersFromClusters()
{
   const QString borderName = clusterArealBorderNameLineEdit->text();
   if (borderName.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "Please enter a border name.");
      return;
   }
   
   //
   // Surface must be in the brain set
   //
   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   theMainWindow->getBrainSet()->addBrainModel(bms, false);
   
   int numberOfBordersCreated = 0;
   
   BrainModelSurfaceROINodeSelection* surfaceROI = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   try {
      BrainModelSurfaceClusterToBorderConverter scbc(theMainWindow->getBrainSet(),
                                                     bms,
                                                     bms->getTopologyFile(),
                                                     borderName,
                                                     surfaceROI,
                                                     clusterArealBorderAutoProjectCheckBox->isChecked());
      scbc.execute();
      numberOfBordersCreated = scbc.getNumberOfBordersCreated();
   }
   catch (BrainModelAlgorithmException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "ERROR", e.whatQString());
   }
   QApplication::restoreOverrideCursor();
   
   if (numberOfBordersCreated > 0) {
      
      QString str = 
         (QString::number(numberOfBordersCreated)
          + " border(s) were created\n");
          
      //
      // Find the matching color
      //
      bool borderColorMatch = false;
      BorderColorFile* borderColorFile = theMainWindow->getBrainSet()->getBorderColorFile();
      const int borderColorIndex = borderColorFile->getColorIndexByName(borderName, borderColorMatch);
     
      //
      // Border color may need to be created
      //
      bool createBorderColor = false;
      if ((borderColorIndex >= 0) && (borderColorMatch == true)) {
         QMessageBox::information(this, "INFO", str);
         createBorderColor = false;
      }
      else if ((borderColorIndex >= 0) && (borderColorMatch == false)) {
         str += ("You may use border color \""
                 + borderColorFile->getColorNameByIndex(borderColorIndex)
                 + "\" for border "
                 + borderName
                 + "\"\n or define a new color.");
         QString b2("Use color ");
         b2.append(borderColorFile->getColorNameByIndex(borderColorIndex));
         QString b1("Define color ");
         b1.append(borderName);
         QMessageBox msgBox(this);
         msgBox.setWindowTitle("Color");
         msgBox.setText(str);
         QPushButton* pb1 = msgBox.addButton(b1, QMessageBox::NoRole);
         msgBox.addButton(b2, QMessageBox::NoRole);
         msgBox.exec();
         if (msgBox.clickedButton() == pb1) {
            createBorderColor = true;
         }
      }
      else {
         str += ("There is no matching color for "
                 + borderName
                 + "\n"
                 + "Would you like to define the color "
                 + borderName
                 + "?");
         QString b1("Define color ");
         b1.append(borderName);
         QMessageBox msgBox(this);
         msgBox.setWindowTitle("Color");
         msgBox.setText(str);
         QPushButton* pb1 = msgBox.addButton(b1, QMessageBox::YesRole);
         msgBox.addButton("No", QMessageBox::NoRole);
         msgBox.exec();
         if (msgBox.clickedButton() == pb1) {
            createBorderColor = true;
         }
      }
      
      if (createBorderColor) {
         QString title("Create Border Color: ");
         title.append(borderName);
         GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this,
                                                                    title,
                                                                    false,
                                                                    false,
                                                                    false,
                                                                    false);
         csd->exec();

         //
         // Add new border color
         //
         float pointSize = 2.0, lineSize = 1.0;
         unsigned char r, g, b, a;
         ColorFile::ColorStorage::SYMBOL symbol;
         csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
         borderColorFile->addColor(borderName, r, g, b, a, pointSize, lineSize, symbol);
      }
      
      //
      // Display borders
      //
      theMainWindow->getBrainSet()->assignBorderColors();
      DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
      dsb->setDisplayBorders(true);

      //
      // Update GUI
      //
      GuiFilesModified fm;
      fm.setBorderColorModified();
      fm.setBorderModified();
      theMainWindow->fileModificationUpdate(fm);
   }
   else {
      QMessageBox::information(this, "INFO", "No clusters were found.");
   }
   
   //
   // Delete the brain model
   //
   theMainWindow->getBrainSet()->deleteBrainModel(bms);
   
   GuiBrainModelOpenGL::updateAllGL();
}


/**
 * create the create linear borders page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createLinearBordersPage()
{
   //
   // Border info widgets
   //
   QPushButton* borderNamePushButton = new QPushButton("Name...");
   borderNamePushButton->setAutoDefault(false);
   borderNamePushButton->setFixedSize(borderNamePushButton->sizeHint());
   QObject::connect(borderNamePushButton, SIGNAL(clicked()),
                    this, SLOT(slotLinearCreateBorderFromROINamePushButton()));
   createLinearBorderFromROINameLineEdit = new QLineEdit;
   QLabel* densityLabel = new QLabel("Sampling Density");
   createLinearBorderFromROISamplingDensityDoubleSpinBox = new QDoubleSpinBox;
   createLinearBorderFromROISamplingDensityDoubleSpinBox->setMinimum(0.01);
   createLinearBorderFromROISamplingDensityDoubleSpinBox->setMaximum(100000.0);
   createLinearBorderFromROISamplingDensityDoubleSpinBox->setSingleStep(1.0);
   createLinearBorderFromROISamplingDensityDoubleSpinBox->setDecimals(2);
   createLinearBorderFromROISamplingDensityDoubleSpinBox->setValue(2.0);
   createLinearBorderFromROISamplingDensityDoubleSpinBox->setMaximumWidth(100);
   
   //
   // Border info group box and layout
   //
   QGroupBox* borderInfoGroupBox = new QGroupBox("Border Information");
   QGridLayout* borderInfoGroupLayout = new QGridLayout(borderInfoGroupBox);
   borderInfoGroupLayout->addWidget(borderNamePushButton, 0, 0);
   borderInfoGroupLayout->addWidget(createLinearBorderFromROINameLineEdit, 0, 1);
   borderInfoGroupLayout->addWidget(densityLabel, 1, 0);
   borderInfoGroupLayout->addWidget(createLinearBorderFromROISamplingDensityDoubleSpinBox, 1, 1, 1, 2);
   
   //
   // Node selection options
   //
   createLinearBorderFromROIAutomaticRadioButton   = new QRadioButton("Automatic");
   createLinearBorderFromROIManualRadioButton = new QRadioButton("Manual");
   QLabel* startNodeLabel = new QLabel("Start Node");
   QLabel* endNodeLabel   = new QLabel("End Node");
   createLinearBorderFromROIStartNodeSpinBox = new QSpinBox;
   createLinearBorderFromROIStartNodeSpinBox->setMinimum(0);
   createLinearBorderFromROIStartNodeSpinBox->setMaximum(std::numeric_limits<int>::max());
   createLinearBorderFromROIStartNodeSpinBox->setSingleStep(1);
   createLinearBorderFromROIEndNodeSpinBox = new QSpinBox;
   createLinearBorderFromROIEndNodeSpinBox->setMinimum(-1);
   createLinearBorderFromROIEndNodeSpinBox->setMaximum(std::numeric_limits<int>::max());
   createLinearBorderFromROIEndNodeSpinBox->setSingleStep(1);
   
   //
   // Select start/end nodes with mouse push buttons
   //
   QPushButton* selectStartNodeWithMousePushButton = new QPushButton("Select with Mouse");
   selectStartNodeWithMousePushButton->setAutoDefault(false);
   selectStartNodeWithMousePushButton->setFixedSize(selectStartNodeWithMousePushButton->sizeHint());
   QObject::connect(selectStartNodeWithMousePushButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateLinearBorderFromROIStartNodePushButton()));
   QPushButton* selectEndNodeWithMousePushButton = new QPushButton("Select with Mouse");
   selectEndNodeWithMousePushButton->setAutoDefault(false);
   selectEndNodeWithMousePushButton->setFixedSize(selectEndNodeWithMousePushButton->sizeHint());
   QObject::connect(selectEndNodeWithMousePushButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateLinearBorderFromROIEndNodePushButton()));
   
   //
   // Button group to keep radio buttons mutually exclusive
   //
   QButtonGroup* buttGroup = new QButtonGroup;
   buttGroup->addButton(createLinearBorderFromROIAutomaticRadioButton);
   buttGroup->addButton(createLinearBorderFromROIManualRadioButton);
   
   //
   // Layout for start and end node node selection options
   //
   QWidget* createBorderFromROINodeSelectionWidget = new QWidget;
   QGridLayout* nodeNumbersLayout = new QGridLayout(createBorderFromROINodeSelectionWidget);
   nodeNumbersLayout->addWidget(new QLabel("   "), 0, 0);
   nodeNumbersLayout->addWidget(startNodeLabel, 0, 1);
   nodeNumbersLayout->addWidget(createLinearBorderFromROIStartNodeSpinBox, 0, 2);
   nodeNumbersLayout->addWidget(selectStartNodeWithMousePushButton, 0, 3);
   nodeNumbersLayout->addWidget(endNodeLabel, 1, 1);
   nodeNumbersLayout->addWidget(createLinearBorderFromROIEndNodeSpinBox, 1, 2);
   nodeNumbersLayout->addWidget(selectEndNodeWithMousePushButton, 1, 3);

   //
   // Node selection group box and layout
   //
   QGroupBox* nodeSelectionGroupBox = new QGroupBox("Starting and Ending Node Selection");
   QVBoxLayout* nodeSelectionLayout = new QVBoxLayout(nodeSelectionGroupBox);
   nodeSelectionLayout->addWidget(createLinearBorderFromROIAutomaticRadioButton);
   nodeSelectionLayout->addWidget(createLinearBorderFromROIManualRadioButton);
   nodeSelectionLayout->addWidget(createBorderFromROINodeSelectionWidget);

   //
   // Connect signals to disable node selection if automatic
   //
   createBorderFromROINodeSelectionWidget->setEnabled(false);
   QObject::connect(createLinearBorderFromROIManualRadioButton, SIGNAL(toggled(bool)),
                    createBorderFromROINodeSelectionWidget, SLOT(setEnabled(bool)));
   createLinearBorderFromROIAutomaticRadioButton->setChecked(true);
   
   //
   // Create border push button
   //
   QPushButton* createBorderPushButton = new QPushButton("Create Linear Border Using Geodesic");
   createBorderPushButton->setAutoDefault(false);
   createBorderPushButton->setFixedSize(createBorderPushButton->sizeHint());
   QObject::connect(createBorderPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateLinearBorderFromROIPushButton()));
                    
   //
   // Widget and layout
   //
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(borderInfoGroupBox);
   layout->addWidget(nodeSelectionGroupBox, 0, Qt::AlignLeft);
   layout->addWidget(createBorderPushButton);

   return w;
}

/**
 * Called to set linear border name.
 */
void 
GuiSurfaceROIOperationPage::slotLinearCreateBorderFromROINamePushButton()
{
   static GuiNameSelectionDialog::LIST_ITEMS_TYPE itemForDisplay =
                   GuiNameSelectionDialog::LIST_BORDER_COLORS_ALPHA;

   GuiNameSelectionDialog nsd(this,
                              GuiNameSelectionDialog::LIST_ALL,
                              itemForDisplay);
   if (nsd.exec() == QDialog::Accepted) {
      createLinearBorderFromROINameLineEdit->setText(nsd.getNameSelected());
   }
}

/**
 * called to set linear border start node.
 */
void 
GuiSurfaceROIOperationPage::slotCreateLinearBorderFromROIStartNodePushButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_SULCAL_BORDER_NODE_START);
}

/**
 * called to set linear border end node.
 */
void 
GuiSurfaceROIOperationPage::slotCreateLinearBorderFromROIEndNodePushButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_SULCAL_BORDER_NODE_END);
}

/**
 * set open border start node.
 */
void 
GuiSurfaceRegionOfInterestDialog::setCreateLinearBorderStartNode(const int nodeNumber)
{
   BrainModelSurfaceROINodeSelection* surfaceROI = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   if (surfaceROI->getNodeSelected(nodeNumber)) {
      operationPage->createLinearBorderFromROIStartNodeSpinBox->setValue(nodeNumber);
   }
   else {
      QMessageBox::critical(this, "ERROR", "Node selected is not in the ROI.");
   }
}

/**
 * set open border end node.
 */
void 
GuiSurfaceRegionOfInterestDialog::setCreateLinearBorderEndNode(const int nodeNumber)
{
   BrainModelSurfaceROINodeSelection* surfaceROI = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   if (surfaceROI->getNodeSelected(nodeNumber)) {
      operationPage->createLinearBorderFromROIEndNodeSpinBox->setValue(nodeNumber);
   }
   else {
      QMessageBox::critical(this, "ERROR", "Node selected is not in the ROI.");
   }
}

/**
 * called to create the linear border.
 */
void 
GuiSurfaceROIOperationPage::slotCreateLinearBorderFromROIPushButton()
{
   //
   // Surface must be in the brain set
   //
   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   theMainWindow->getBrainSet()->addBrainModel(bms, false);
   
   //
   // Create the border from the ROI
   //
   const QString borderName(createLinearBorderFromROINameLineEdit->text());
   int startNode = -1;
   int endNode   = -1;
   if (createLinearBorderFromROIManualRadioButton->isChecked()) {
      startNode = createLinearBorderFromROIStartNodeSpinBox->value();
      endNode   = createLinearBorderFromROIEndNodeSpinBox->value();
   }
   BrainModelSurfaceROICreateBorderUsingGeodesic roi(theMainWindow->getBrainSet(),
                                         bms,
                                         theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection(),
                                         borderName,
                                         startNode,
                                         endNode,
                                         createLinearBorderFromROISamplingDensityDoubleSpinBox->value());
   try {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      roi.execute();
      QApplication::restoreOverrideCursor();
   }
   catch (BrainModelAlgorithmException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "ERROR", e.whatQString());
      theMainWindow->getBrainSet()->deleteBrainModel(bms);
      return;
   }
   
   //
   // Get border created and display it
   //
   Border border = roi.getBorder();
   if (border.getNumberOfLinks() <= 0) {
      QMessageBox::critical(this, "ERROR", "Border created has no links.");
      theMainWindow->getBrainSet()->deleteBrainModel(bms);
      return;
   }

   //
   // Find the matching color
   //
   bool borderColorMatch = false;
   BorderColorFile* borderColorFile = theMainWindow->getBrainSet()->getBorderColorFile();
   int borderColorIndex = borderColorFile->getColorIndexByName(borderName, borderColorMatch);
  
   //
   // Border color may need to be created
   //
   bool createBorderColor = false;
   if ((borderColorIndex >= 0) && (borderColorMatch == true)) {
      createBorderColor = false;
   }
   else if ((borderColorIndex >= 0) && (borderColorMatch == false)) {
      QString msg("Use border color \"");
      msg.append(borderColorFile->getColorNameByIndex(borderColorIndex));
      msg.append("\" for border ");
      msg.append(borderName);
      msg.append(" ?");
      QString noButton("No, define color ");
      noButton.append(borderName);
      QMessageBox msgBox(this);
      msgBox.setWindowTitle("Use Partially Matching Color");
      msgBox.setText(msg);
      msgBox.addButton("Yes", QMessageBox::YesRole);
      QPushButton* defineColorPushButton = msgBox.addButton(noButton, QMessageBox::NoRole);
      msgBox.exec();
      if (msgBox.clickedButton() == defineColorPushButton) {
         createBorderColor = true;
      }
   }   
   else {
      createBorderColor = true;
   }
   
   if (createBorderColor) {
      QString title("Create Border Color: ");
      title.append(borderName);
      QApplication::beep();
      GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this,
                                                                 title,
                                                                 false,
                                                                 false,
                                                                 false,
                                                                 false);
      csd->exec();

      //
      // Add new border color
      //
      float pointSize = 2.0, lineSize = 1.0;
      unsigned char r, g, b, a;
      ColorFile::ColorStorage::SYMBOL symbol;
      csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
      borderColorFile->addColor(borderName, r, g, b, a, pointSize, lineSize, symbol);
      borderColorIndex = borderColorFile->getNumberOfColors() - 1;
   }


   //
   // Add border
   //
   BrainModelBorder* b = new BrainModelBorder(theMainWindow->getBrainSet(),
                                              &border, 
                                              bms->getSurfaceType());
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->addBorder(b);
   
   //
   // Project the border
   //
   const int borderNumber = bmbs->getNumberOfBorders() - 1;
   if (borderNumber >= 0) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      bmbs->projectBorders(bms,
                           true,
                           borderNumber,
                           borderNumber);
      QApplication::restoreOverrideCursor();
   }

   //
   // Display borders
   //
   DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
   dsb->setDisplayBorders(true);
   
   //
   // Delete copy of surface
   //
   theMainWindow->getBrainSet()->deleteBrainModel(bms);

   //
   // Files have changed
   //
   GuiFilesModified fm;
   fm.setBorderColorModified();
   fm.setBorderModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * create the create volume roi page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createCreateVolumeROIPage()
{
   QPushButton* createVolumeFromQueryNodesPushButton = new QPushButton(
                                             "Create Volume From Displayed Region of Interest Nodes...");
   createVolumeFromQueryNodesPushButton->setAutoDefault(false);
   createVolumeFromQueryNodesPushButton->setFixedSize(createVolumeFromQueryNodesPushButton->sizeHint());
   QObject::connect(createVolumeFromQueryNodesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateVolumeFromQueryNodesButton()));

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(createVolumeFromQueryNodesPushButton);
   layout->addStretch();
   
   return w;
}

/**
 * Called to create a volume from the display query nodes.
 */
void
GuiSurfaceROIOperationPage::slotCreateVolumeFromQueryNodesButton()
{
   BrainModelSurfaceROINodeSelection* surfaceROI = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   if (surfaceROI->anyNodesSelected() == false) {
      QMessageBox::critical(this, "Error: no nodes in ROI",
                            "There are no nodes in the region of interest.");                            
      return;
   }

   GuiSurfaceToVolumeDialog svd(this, 
                                GuiSurfaceToVolumeDialog::DIALOG_MODE_NORMAL,
                                "Create Region Of Interest Volume",
                                false);
   if (svd.exec() == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();

      float offset[3];
      int dim[3];
      float origin[3];
      float voxelSize[3];
      svd.getSurfaceOffset(offset);
      svd.getVolumeDimensions(dim);
      svd.getVolumeVoxelSizes(voxelSize);
      svd.getVolumeOrigin(origin);
      BrainModelSurfaceToVolumeConverter stv(theMainWindow->getBrainSet(),
                                             bms,
                                             svd.getStandardVolumeSpace(),
                                             offset,
                                             dim,
                                             voxelSize,
                                             origin,
                                             svd.getSurfaceInnerBoundary(),
                                             svd.getSurfaceOuterBoundary(),
                                             svd.getSurfaceThicknessStep(),
                   BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_ROI_NODES,
                                             svd.getIntersectionMode());
      stv.setNodeToVoxelMappingEnabled(svd.getNodeToVoxelMappingEnabled(),
                                       svd.getNodeToVoxelMappingFileName());
      try {
         stv.execute();
         delete bms;
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Error", e.whatQString());
         delete bms;
         return;
      }
      
      theMainWindow->speakText("The region of interest volume has been created.", false);
      
      GuiFilesModified fm;
      fm.setVolumeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL(NULL);
      
      QApplication::restoreOverrideCursor();
   }
}

/**
 * create the disconnect nodes page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createDisconnectNodesPage()
{
   QPushButton* disconnectButton = new QPushButton("Disconnect Selected Nodes");
   disconnectButton->setAutoDefault(false);
   disconnectButton->setFixedSize(disconnectButton->sizeHint());
   QObject::connect(disconnectButton, SIGNAL(clicked()),
                    this, SLOT(slotDisconnectNodes()));

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(disconnectButton);
   layout->addStretch();
   
   return w;
}

/**
 * Called to disconnect the selected nodes.
 */
void
GuiSurfaceROIOperationPage::slotDisconnectNodes()
{
   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms != NULL) {
      TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
         BrainModelSurfaceROINodeSelection* surfaceROI = 
            theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
         std::vector<bool> nodesAreInROI;
         surfaceROI->getNodesInROI(nodesAreInROI);
         
         theMainWindow->getBrainSet()->disconnectNodes(tf, nodesAreInROI);
         GuiBrainModelOpenGL::updateAllGL(NULL); 
         theMainWindow->speakText("Nodes have been disconnected.", false);
      }
   }
   delete bms;
}

/**
 * create the geodesic page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createGeodesicPage()
{
   static const int maxComboBoxWidth = 400;

   QLabel* nodeLabel = new QLabel("Query Node");
   geodesicNodeSpinBox = new QSpinBox;
   geodesicNodeSpinBox->setMinimum(0);
   geodesicNodeSpinBox->setMaximum(100000000);
   geodesicNodeSpinBox->setSingleStep(1);
   geodesicNodeSpinBox->setValue(0);
   QObject::connect(geodesicNodeSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotGeodesicUpdateColumnNames()));
   QPushButton* geodesicNodeButton = new QPushButton("Choose Node With Mouse");
   geodesicNodeButton->setAutoDefault(false);
   geodesicNodeButton->setFixedSize(geodesicNodeButton->sizeHint());
   QObject::connect(geodesicNodeButton, SIGNAL(clicked()),
                    this, SLOT(slotGeodesicNodePushButton()));
   QHBoxLayout* nodeLayout = new QHBoxLayout;
   nodeLayout->addWidget(nodeLabel);
   nodeLayout->addWidget(geodesicNodeSpinBox);
   nodeLayout->addWidget(geodesicNodeButton);
   nodeLayout->addStretch();
                    
   QLabel* metricLabel = new QLabel("Metric ");
   geodesicMetricColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                            GUI_NODE_FILE_TYPE_METRIC,
                                            true,
                                            true,
                                            false);
   geodesicMetricColumnComboBox->setMaximumWidth(maxComboBoxWidth);
   geodesicMetricColumnNameLineEdit = new QLineEdit;
   QObject::connect(geodesicMetricColumnComboBox, SIGNAL(itemNameSelected(const QString&)),
                    geodesicMetricColumnNameLineEdit, SLOT(setText(const QString&)));
   geodesicMetricColumnNameLineEdit->setText(geodesicMetricColumnComboBox->getCurrentLabel());
   
   QLabel* geoLabel = new QLabel("Geodesic ");
   geodesicDistanceColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                            GUI_NODE_FILE_TYPE_GEODESIC_DISTANCE,
                                            true,
                                            true,
                                            false);
   geodesicDistanceColumnComboBox->setMaximumWidth(maxComboBoxWidth);
   geodesicDistanceColumnNameLineEdit = new QLineEdit;
   QObject::connect(geodesicDistanceColumnComboBox, SIGNAL(itemNameSelected(const QString&)),
                    geodesicDistanceColumnNameLineEdit, SLOT(setText(const QString&)));
   geodesicDistanceColumnNameLineEdit->setText(geodesicDistanceColumnComboBox->getCurrentLabel());
   
   QGridLayout* fileGridLayout = new QGridLayout;
   fileGridLayout->addWidget(metricLabel, 0, 0);
   fileGridLayout->addWidget(geodesicMetricColumnComboBox, 0, 1);
   fileGridLayout->addWidget(geodesicMetricColumnNameLineEdit, 0, 2);
   fileGridLayout->addWidget(geoLabel, 1, 0);
   fileGridLayout->addWidget(geodesicDistanceColumnComboBox, 1, 1);
   fileGridLayout->addWidget(geodesicDistanceColumnNameLineEdit, 1, 2);
   QHBoxLayout* fileLayout = new QHBoxLayout;
   fileLayout->addLayout(fileGridLayout);
   fileLayout->addStretch();
   
   QPushButton* geodesicButton = new QPushButton("Determine Geodesic Distances");
   geodesicButton->setAutoDefault(false);
   geodesicButton->setFixedSize(geodesicButton->sizeHint());
   QObject::connect(geodesicButton, SIGNAL(clicked()),
                    this, SLOT(slotGeodesicPushButton()));
                    

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(nodeLayout);
   layout->addLayout(fileLayout);
   layout->addWidget(geodesicButton);
   layout->addStretch();

   return w;
}

/**
 * called to update geodesic column names.
 */
void 
GuiSurfaceROIOperationPage::slotGeodesicUpdateColumnNames()
{
   const int nodeNumber = geodesicNodeSpinBox->value();
   QString newName("Node ");
   newName.append(QString::number(nodeNumber));
   
   const QString geoName(geodesicDistanceColumnNameLineEdit->text());
   if (geoName == geodesicDistanceColumnComboBox->getNewColumnLabel()) {
      geodesicDistanceColumnNameLineEdit->setText(newName);
   }
   
   const QString metricName(geodesicMetricColumnNameLineEdit->text());
   if (metricName == geodesicMetricColumnComboBox->getNewColumnLabel()) {
      geodesicMetricColumnNameLineEdit->setText(newName);
   }
}

/**
 * called when geodesic choose node with mouse button pressed.
 */
void 
GuiSurfaceROIOperationPage::slotGeodesicNodePushButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_GEODESIC_NODE_SELECT);
}

/**
 * called to create geodesic.
 */
void 
GuiSurfaceROIOperationPage::slotGeodesicPushButton()
{
   BrainModelSurfaceROINodeSelection* surfaceROI = 
      theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
   if (surfaceROI->anyNodesSelected() == false) {
      QMessageBox::critical(this, "Error: no nodes in ROI",
                            "There are no nodes in the region of interest.");
      return;
   }
   
   const int nodeNumber = geodesicNodeSpinBox->value();
   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms != NULL) {
      if ((nodeNumber < 0) || (nodeNumber >= bms->getNumberOfNodes())) {
         QMessageBox::critical(this, "Error", "No query node selected");
      }
      else {
         //TopologyFile* tf = topologyComboBox->getSelectedTopologyFile(); //bms->getTopologyFile();
         TopologyFile* tf = bms->getTopologyFile();
         if (tf != NULL) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            BrainModelSurfaceGeodesic bmsg(theMainWindow->getBrainSet(),
                                           bms,
                                           theMainWindow->getBrainSet()->getMetricFile(),
                                           geodesicMetricColumnComboBox->currentIndex(),
                                           geodesicMetricColumnNameLineEdit->text(),
                                           theMainWindow->getBrainSet()->getGeodesicDistanceFile(),
                                           geodesicDistanceColumnComboBox->currentIndex(),
                                           geodesicDistanceColumnNameLineEdit->text(),
                                           nodeNumber,
                                           surfaceROI);
            try {
               bmsg.execute();
            }
            catch (BrainModelAlgorithmException& e) {
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "Error", e.whatQString());
            }

            GuiFilesModified fm;
            fm.setMetricModified();
            fm.setGeodesicModified();
            theMainWindow->fileModificationUpdate(fm);
            GuiBrainModelOpenGL::updateAllGL(NULL); 
            QApplication::restoreOverrideCursor();
            
            theMainWindow->speakText("Geodesic data has been created.", false);
         }
      }
      
      delete bms;
   }
}
      
/**
 * create the probabilistic atlas overlap page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createProbAtlasOverlapPage()
{
   QPushButton* createReportPushButton = 
      new QPushButton("Create Probabilistic Atlas Overlap Report");
   createReportPushButton->setAutoDefault(false);
   createReportPushButton->setFixedSize(createReportPushButton->sizeHint());
   QObject::connect(createReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateProbAtlasOverlapReport()));
                    
   probAtlasOverlapSemicolonSeparateCheckBox = new QCheckBox("Separate Report With Semicolons (for import to spreadsheet)");

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(createReportPushButton);
   layout->addWidget(probAtlasOverlapSemicolonSeparateCheckBox);
   layout->addStretch();

   return w;
}

/**
 * called to generate prob atlas overlap report.
 */
void 
GuiSurfaceROIOperationPage::slotCreateProbAtlasOverlapReport()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   QString errorMessage;

   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   ProbabilisticAtlasFile* probAtlasFile = 
      theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();

   try {
      BrainModelSurfaceROINodeSelection* roi = 
         theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();

      QString headerText("Probabilistic Overlap Report");
      BrainModelSurfaceROIProbAtlasOverlapReport overlap(
                               theMainWindow->getBrainSet(),
                               bms,
                               roi,
                               probAtlasFile,
                               headerText,
                               probAtlasOverlapSemicolonSeparateCheckBox->isChecked());
      overlap.execute();
   
      //
      // set report text
      //
      reportPage->setReportText(overlap.getReportText());
   }
   catch (BrainModelAlgorithmException& e) {
      errorMessage = e.whatQString();
   }
   
   delete bms;
   
   QApplication::restoreOverrideCursor();
   
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this,
                            "ERROR",
                            errorMessage);
   }      
   
   //
   // Move to next page
   //
   roiDialog->next();
}

/**
 * create the smoothing page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createSmoothingPage()
{
   QPushButton* smoothButton = new QPushButton("Smooth Selected Nodes...");
   smoothButton->setAutoDefault(false);
   smoothButton->setFixedSize(smoothButton->sizeHint());
   QObject::connect(smoothButton, SIGNAL(clicked()),
                    this, SLOT(slotSmoothNodesPushButtonPressed()));

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(smoothButton);
   layout->addStretch();

   return w;
}

/**
 * called to smooth nodes.
 */
void
GuiSurfaceROIOperationPage::slotSmoothNodesPushButtonPressed()
{
   //
   // Copy surface
   //
   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms != NULL) {
      BrainModelSurfaceROINodeSelection* surfaceROI = 
         theMainWindow->getBrainSet()->getBrainModelSurfaceRegionOfInterestNodeSelection();
      std::vector<bool> nodesAreInROI;
      surfaceROI->getNodesInROI(nodesAreInROI);

      //
      // Smooth the surface copy.  We cannot smooth the ROI surface because
      // the users is allowed to choose the coord and topo files which may
      // not be one of the loaded surfaces.
      //
      GuiSmoothingDialog sd(this, true, false, &nodesAreInROI);
      sd.setSmoothingSurface(bms);
      if (sd.exec()) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         GuiBrainModelOpenGL::updateAllGL(NULL); 
         QApplication::restoreOverrideCursor();
      }

      //
      // Copy coordinates to the ROI surfaces's coordinates
      //
      CoordinateFile* operationCoords = roiDialog->getOperationCoordinateFile(); 
      const CoordinateFile* smoothedCoords = bms->getCoordinateFile();
      const int numCoords = smoothedCoords->getNumberOfCoordinates();
      for (int i = 0; i < numCoords; i++) {
         operationCoords->setCoordinate(i, smoothedCoords->getCoordinate(i));
      }
      
      delete bms;
   }
}

/**
 * create the statistical paint report page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createPaintReportPage()
{
   QLabel* paintLabel = new QLabel("Paint Column ");
   paintRegionReportColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                             GUI_NODE_FILE_TYPE_PAINT,
                                             false,
                                             false,
                                             false);
   QHBoxLayout* columnLayout = new QHBoxLayout;
   columnLayout->addWidget(paintLabel);
   columnLayout->addWidget(paintRegionReportColumnComboBox);
   columnLayout->addStretch();
   
   //
   // Distortion correction
   //
   QLabel* distLabel = new QLabel("Distortion Correction");
   paintRegionReportDistortionCorrectionMetricColumnComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_METRIC,
                                                  false,
                                                  true,
                                                  false);
   paintRegionReportDistortionCorrectionMetricColumnComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   paintRegionReportDistortionCorrectionMetricColumnComboBox->setToolTip(
                 "Use this control to select a metric file column\n"
                 "that contains a distortion correction ratio.");
   QHBoxLayout* distLayout = new QHBoxLayout;
   distLayout->addWidget(distLabel);
   distLayout->addWidget(paintRegionReportDistortionCorrectionMetricColumnComboBox);
   distLayout->addStretch();

   paintReportSeparateWithSemicolonsCheckBox = new QCheckBox(
                 "Separate Paint Subregion Report With Semicolons (for import to spreadsheet)");
   
   QPushButton* createPaintReportButton = new QPushButton("Create Paint Subregion Report");
   createPaintReportButton->setAutoDefault(false);
   createPaintReportButton->setFixedSize(createPaintReportButton->sizeHint());
   QObject::connect(createPaintReportButton, SIGNAL(clicked()),
                    this, SLOT(slotCreatePaintReportButton()));
                    
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(columnLayout);
   layout->addLayout(distLayout);
   layout->addWidget(paintReportSeparateWithSemicolonsCheckBox);
   layout->addWidget(createPaintReportButton);
   layout->addStretch();

   return w;
}

/**
 * called to create paint region report.
 */
void 
GuiSurfaceROIOperationPage::slotCreatePaintReportButton()
{
   //
   // Copy surface
   //
   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      BrainSet* bs = theMainWindow->getBrainSet();
      
      MetricFile* mf = bs->getMetricFile();
      std::vector<bool> metricSelections(mf->getNumberOfColumns(), true);

      SurfaceShapeFile* ssf = bs->getSurfaceShapeFile();
      std::vector<bool> shapeSelections(ssf->getNumberOfColumns(), true);

      PaintFile* pf = bs->getPaintFile();
      std::vector<bool> paintSelections(pf->getNumberOfColumns(), true);
      
      BrainModelSurfaceROINodeSelection* roi = 
         bs->getBrainModelSurfaceRegionOfInterestNodeSelection();
         
      BrainModelSurfaceROIPaintReport roipr(bs,
                                           bms,
                                           roi,
                                           mf,
                                           metricSelections,
                                           ssf,
                                           shapeSelections,
                                           pf,
                                           paintSelections,
                                           paintRegionReportColumnComboBox->currentIndex(),
                                           bs->getLatLonFile(),
                                           0,  // lat/lon file column
                                           mf,
                                           paintRegionReportDistortionCorrectionMetricColumnComboBox->currentIndex(),
                                           paintReportSeparateWithSemicolonsCheckBox->isChecked());
      try {
         roipr.execute();
         QApplication::restoreOverrideCursor();
      }
      catch (BrainModelAlgorithmException& e) {
        QApplication::restoreOverrideCursor();   
        QMessageBox::critical(this, "ERROR", e.whatQString());
      }

   
      reportPage->setReportText(roipr.getReportText());
      
      delete bms;

      //
      // Move to next page
      //
      roiDialog->next();
   }
}

/**
 * create the statistical report page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createStatisticalReportPage()
{
   //
   // Distortion correction
   //
   QLabel* distLabel = new QLabel("Distortion Correction");
   statisticalReportDistortionCorrectionMetricColumnComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_METRIC,
                                                  false,
                                                  true,
                                                  false);
   statisticalReportDistortionCorrectionMetricColumnComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   statisticalReportDistortionCorrectionMetricColumnComboBox->setToolTip(
                 "Use this control to select a metric file column\n"
                 "that contains a distortion correction ratio.");
   QHBoxLayout* distLayout = new QHBoxLayout;
   distLayout->addWidget(distLabel);
   distLayout->addWidget(statisticalReportDistortionCorrectionMetricColumnComboBox);
   distLayout->addStretch();
          
   //
   // Separate report with semicolon's check box
   //
   statisticalReportSeparateWithSemicolonsCheckBox = new QCheckBox(
                 "Separate Report With Semicolons");
   
   QPushButton* createReportButton = new QPushButton("Create Report");
   createReportButton->setAutoDefault(false);
   createReportButton->setFixedSize(createReportButton->sizeHint());
   QObject::connect(createReportButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateStatisticalReportButton()));
                    
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(distLayout);
   layout->addWidget(statisticalReportSeparateWithSemicolonsCheckBox);
   layout->addWidget(createReportButton);
   layout->addStretch();

   return w;
}

/**
 * called to create statistical report.
 */
void 
GuiSurfaceROIOperationPage::slotCreateStatisticalReportButton()
{
   //
   // Copy surface
   //
   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      BrainSet* bs = theMainWindow->getBrainSet();
      
      MetricFile* mf = bs->getMetricFile();
      std::vector<bool> metricSelections(mf->getNumberOfColumns(), true);

      SurfaceShapeFile* ssf = bs->getSurfaceShapeFile();
      std::vector<bool> shapeSelections(ssf->getNumberOfColumns(), true);

      PaintFile* pf = bs->getPaintFile();
      std::vector<bool> paintSelections(pf->getNumberOfColumns(), true);
      
      BrainModelSurfaceROINodeSelection* roi = 
         bs->getBrainModelSurfaceRegionOfInterestNodeSelection();
         
      BrainModelSurfaceROITextReport bmsri(bs,
                                           bms,
                                           roi,
                                           mf,
                                           metricSelections,
                                           ssf,
                                           shapeSelections,
                                           pf,
                                           paintSelections,
                                           bs->getLatLonFile(),
                                           0,  // lat/lon file column
                                           //reportHeader,
                                           "Statistical Report",
                                           mf,
                                           statisticalReportDistortionCorrectionMetricColumnComboBox->currentIndex(),
                                           statisticalReportSeparateWithSemicolonsCheckBox->isChecked());
      try {
         bmsri.execute();
      }
      catch (BrainModelAlgorithmException& e) {
        QApplication::restoreOverrideCursor();   
        QMessageBox::critical(this, "ERROR", e.whatQString());
      }

      QApplication::restoreOverrideCursor();
   
      reportPage->setReportText(bmsri.getReportText());
      
      delete bms;

      //
      // Move to next page
      //
      roiDialog->next();
   }
}
      
/**
 * create the shape correlation page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createShapeCorrelationPage()
{
   QLabel* shapeLabel = new QLabel("Reference Column ");
   shapeCorrelationColumnComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(
                                              GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                              false, 
                                              false,
                                              true);
   shapeCorrelationColumnComboBox->setCurrentIndex(
                       GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_ALL);
   QHBoxLayout* columnLayout = new QHBoxLayout;
   columnLayout->addWidget(shapeLabel);  
   columnLayout->addWidget(shapeCorrelationColumnComboBox);  
   columnLayout->addStretch();
   
   shapeCorrelationSeparateWithSemicolonsCheckBox = new QCheckBox("Separate Report With Semicolons");
   
   QPushButton* createReportPushButton = new QPushButton("Create Correlation Report");
   createReportPushButton->setFixedSize(createReportPushButton->sizeHint());
   createReportPushButton->setAutoDefault(false);
   QObject::connect(createReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCorrelationShapeReportPushButton()));

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(columnLayout);
   layout->addWidget(shapeCorrelationSeparateWithSemicolonsCheckBox);
   layout->addWidget(createReportPushButton);
   layout->addStretch();
   
   return w;
}

/**
 * called to create shape correleation report.
 */
void 
GuiSurfaceROIOperationPage::slotCorrelationShapeReportPushButton()
{
   //
   // Copy surface
   //
   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      BrainSet* bs = theMainWindow->getBrainSet();
      SurfaceShapeFile* ssf = bs->getSurfaceShapeFile();

      BrainModelSurfaceROINodeSelection* roi = 
         bs->getBrainModelSurfaceRegionOfInterestNodeSelection();
      const int column = shapeCorrelationColumnComboBox->currentIndex();
      
      BrainModelSurfaceROIShapeCorrelationReport shapeCorr(bs,
                                           bms,
                                           roi,
                                           ssf,
                                           column,
                     shapeCorrelationSeparateWithSemicolonsCheckBox->isChecked());
      try {
         shapeCorr.execute();
      }
      catch (BrainModelAlgorithmException& e) {
        QApplication::restoreOverrideCursor();   
        QMessageBox::critical(this, "ERROR", e.whatQString());
      }

      QApplication::restoreOverrideCursor();
   
      reportPage->setReportText(shapeCorr.getReportText());
      
      delete bms;

      //
      // Move to next page
      //
      roiDialog->next();
   }
}
      
/**
 * create the shape cluster analysis page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createShapeClusterAnalysisPage()
{
   QLabel* metricArealDistortionLabel = new QLabel("Metric Areal Distortion");
   shapeClusterMetricArealDistortionComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_METRIC,
                                              false,
                                              true,
                                              false);
   shapeClusterMetricArealDistortionComboBox->setCurrentIndex(
      GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   QHBoxLayout* arealDistLayout = new QHBoxLayout;
   arealDistLayout->addWidget(metricArealDistortionLabel);
   arealDistLayout->addWidget(shapeClusterMetricArealDistortionComboBox);
   arealDistLayout->addStretch();
   
   QLabel* threshLabel = new QLabel("Threshold ");
   shapeClusterThresholdDoubleSpinBox = new QDoubleSpinBox;
   shapeClusterThresholdDoubleSpinBox->setMinimum(-10000000.0);
   shapeClusterThresholdDoubleSpinBox->setMaximum( 10000000.0);
   shapeClusterThresholdDoubleSpinBox->setSingleStep(1.0);
   shapeClusterThresholdDoubleSpinBox->setDecimals(3);
   QHBoxLayout* threshLayout = new QHBoxLayout;
   threshLayout->addWidget(threshLabel);
   threshLayout->addWidget(shapeClusterThresholdDoubleSpinBox);
   threshLayout->addStretch();
   
   shapeClusterSeparateWithSemicolonsCheckBox = new QCheckBox("Separate Report With Semicolons");
   
   QPushButton* createClusterReportPushButton = new QPushButton("Create Cluster Report");
   createClusterReportPushButton->setFixedSize(createClusterReportPushButton->sizeHint());
   createClusterReportPushButton->setAutoDefault(false);
   QObject::connect(createClusterReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotShapeClusterReportPushButton()));

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(arealDistLayout);
   layout->addLayout(threshLayout);
   layout->addWidget(shapeClusterSeparateWithSemicolonsCheckBox);
   layout->addWidget(createClusterReportPushButton);
   layout->addStretch();

   return w;
}

/**
 * called to creaet shape cluster report.
 */
void 
GuiSurfaceROIOperationPage::slotShapeClusterReportPushButton()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   std::vector<bool> columnsSelected(bs->getSurfaceShapeFile()->getNumberOfColumns(), true);
   runMetricShapeClusterAnalysis(bs->getSurfaceShapeFile(),
                                 bs->getMetricFile(),
                                 shapeClusterMetricArealDistortionComboBox->currentIndex(),
                                 columnsSelected,
                                 shapeClusterThresholdDoubleSpinBox->value(),
                                 shapeClusterSeparateWithSemicolonsCheckBox->isChecked());


/*
   //
   // Copy surface
   //
   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      BrainSet* bs = theMainWindow->getBrainSet();
      SurfaceShapeFile* ssf = bs->getSurfaceShapeFile();
      std::vector<bool> columnSelectedFlags(ssf->getNumberOfColumns(), true);
      BrainModelSurfaceROINodeSelection* roi = 
         bs->getBrainModelSurfaceRegionOfInterestNodeSelection();
      
      BrainModelSurfaceROIMetricClusterReport cluster(bs,
                                           bms,
                                           roi,
                                           ssf,
                                           columnSelectedFlags,
                                           bs->getMetricFile(),
                                           shapeClusterMetricArealDistortionComboBox->currentIndex(),
                                           shapeClusterThresholdDoubleSpinBox->value(),
                     shapeClusterSeparateWithSemicolonsCheckBox->isChecked());
      try {
         cluster.execute();
      }
      catch (BrainModelAlgorithmException& e) {
        QApplication::restoreOverrideCursor();   
        QMessageBox::critical(this, "ERROR", e.whatQString());
      }

      QApplication::restoreOverrideCursor();
   
      reportPage->setReportText(cluster.getReportText());
      
      delete bms;

      //
      // Move to next page
      //
      roiDialog->next();
   }
*/
}
      
/**
 * create the metric cluster analysis page.
 */
QWidget* 
GuiSurfaceROIOperationPage::createMetricClusterAnalysisPage()
{
   QLabel* metricArealDistortionLabel = new QLabel("Metric Areal Distortion");
   metricClusterMetricArealDistortionComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_METRIC,
                                              false,
                                              true,
                                              false);
   metricClusterMetricArealDistortionComboBox->setCurrentIndex(
      GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   QHBoxLayout* arealDistLayout = new QHBoxLayout;
   arealDistLayout->addWidget(metricArealDistortionLabel);
   arealDistLayout->addWidget(metricClusterMetricArealDistortionComboBox);
   arealDistLayout->addStretch();
   
   QLabel* threshLabel = new QLabel("Threshold ");
   metricClusterThresholdDoubleSpinBox = new QDoubleSpinBox;
   metricClusterThresholdDoubleSpinBox->setMinimum(-10000000.0);
   metricClusterThresholdDoubleSpinBox->setMaximum( 10000000.0);
   metricClusterThresholdDoubleSpinBox->setSingleStep(1.0);
   metricClusterThresholdDoubleSpinBox->setDecimals(3);
   QHBoxLayout* threshLayout = new QHBoxLayout;
   threshLayout->addWidget(threshLabel);
   threshLayout->addWidget(metricClusterThresholdDoubleSpinBox);
   threshLayout->addStretch();
   
   metricClusterSeparateWithSemicolonsCheckBox = new QCheckBox("Separate Report With Semicolons");
   
   QPushButton* createClusterReportPushButton = new QPushButton("Create Cluster Report");
   createClusterReportPushButton->setFixedSize(createClusterReportPushButton->sizeHint());
   createClusterReportPushButton->setAutoDefault(false);
   QObject::connect(createClusterReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotMetricClusterReportPushButton()));

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(arealDistLayout);
   layout->addLayout(threshLayout);
   layout->addWidget(metricClusterSeparateWithSemicolonsCheckBox);
   layout->addWidget(createClusterReportPushButton);
   layout->addStretch();

   return w;
}

/**
 * called to create metric cluster report.
 */
void 
GuiSurfaceROIOperationPage::slotMetricClusterReportPushButton()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   std::vector<bool> columnsSelected(bs->getMetricFile()->getNumberOfColumns(), true);
   runMetricShapeClusterAnalysis(bs->getMetricFile(),
                                 bs->getMetricFile(),
                                 metricClusterMetricArealDistortionComboBox->currentIndex(),
                                 columnsSelected,
                                 metricClusterThresholdDoubleSpinBox->value(),
                                 metricClusterSeparateWithSemicolonsCheckBox->isChecked());
}
      
/**
 * run a metric or shape cluster report.
 */
void 
GuiSurfaceROIOperationPage::runMetricShapeClusterAnalysis(MetricFile* metricShapeFile,
                                   MetricFile* distortionMetricFile,
                                   const int distortionMetricColumn,
                                   const std::vector<bool>& metricShapeColumnsSelected,
                                   const float thresholdValue,
                                   const bool separateReportWithSemicolonsFlag)
{
   //
   // Copy surface
   //
   BrainModelSurface* bms = roiDialog->getCopyOfOperationSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      BrainSet* bs = theMainWindow->getBrainSet();
      std::vector<bool> columnSelectedFlags(metricShapeFile->getNumberOfColumns(), true);
      BrainModelSurfaceROINodeSelection* roi = 
         bs->getBrainModelSurfaceRegionOfInterestNodeSelection();
      
      BrainModelSurfaceROIMetricClusterReport cluster(bs,
                                           bms,
                                           roi,
                                           metricShapeFile,
                                           metricShapeColumnsSelected,
                                           distortionMetricFile,
                                           distortionMetricColumn,
                                           thresholdValue,
                                           separateReportWithSemicolonsFlag);
      try {
         cluster.execute();
      }
      catch (BrainModelAlgorithmException& e) {
        QApplication::restoreOverrideCursor();   
        QMessageBox::critical(this, "ERROR", e.whatQString());
      }

      QApplication::restoreOverrideCursor();
   
      reportPage->setReportText(cluster.getReportText());
      
      delete bms;

      //
      // Move to next page
      //
      roiDialog->next();
   }
}
                                         


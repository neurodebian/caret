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

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>
#include <QToolTip>
#include <QLayout>

#include "ArealEstimationFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelContours.h"
#include "BrainModelIdentification.h"
#include "BrainModelOpenGLSelectedItem.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "CellFile.h"
#include "CellProjectionFile.h"
#include "CommunicatorClientAFNI.h"
#include "CommunicatorClientFIV.h"
#include "ContourCellFile.h"
#include "ContourFile.h"
#include "DebugControl.h"
#include "DisplaySettingsArealEstimation.h"
#include "DisplaySettingsCoCoMac.h"
#include "DisplaySettingsDeformationField.h"
#include "DisplaySettingsGeodesicDistance.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsPaint.h"
#include "DisplaySettingsRgbPaint.h"
#include "DisplaySettingsScene.h"
#include "DisplaySettingsSurface.h"
#include "DisplaySettingsSurfaceShape.h"
#include "DisplaySettingsTopography.h"
#include "DisplaySettingsWustlRegion.h"
#include "FileUtilities.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "GeodesicDistanceFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiHyperLinkTextBrowser.h"
#include "GuiIdentifyDialog.h"
#include "GuiMainWindow.h"
#include "GuiToolBar.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "LatLonFile.h"
#include "PaintFile.h"
#include "PaletteFile.h"
#include "PreferencesFile.h"
#include "ProbabilisticAtlasFile.h"
#include "RgbPaintFile.h"
#include "SectionFile.h"
#include "StereotaxicSpace.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopographyFile.h"
#include "VocabularyFile.h"
#include "VtkModelFile.h"
#include "WustlRegionFile.h"
#include "global_variables.h"

/**
 * Constructor for Main Window containing text area and toolbars.
 */
GuiIdentifyMainWindow::GuiIdentifyMainWindow(GuiIdentifyDialog* parent) 
                      : QMainWindow(parent)
{
   //
   // Tab widget for filtering and text display
   //
   QTabWidget* mainTabWidget = new QTabWidget;
   mainTabWidget->addTab(createTextDisplayWidget(), "Display");
   mainTabWidget->addTab(createDisplayFilterWidget(), "Filtering");
   setCentralWidget(mainTabWidget);
   
   //
   // Add the toolbar
   // Must be created after text display since it uses a signal from the text display
   //
   addToolBar(createToolBar());   
   
   updateToolBarButtons();
   updateFilteringSelections();
}

/**
 * create the tool bar.
 */
QToolBar* 
GuiIdentifyMainWindow::createToolBar()
{
   //
   // Toolbar for node/border/cell/foci/clear buttons
   //
   QToolBar* toolbar = new QToolBar(this);
   toolbar->setMovable(false);
   
   //
   // Clear Identified Nodes toolbar button.
   //
   QAction* clearIDAction = new QAction("CID", this);
   clearIDAction->setToolTip(
                   "Remove all border, foci,\n"
                   "and node highlighting."); 
   QObject::connect(clearIDAction, SIGNAL(triggered()),
                    this, SLOT(clearNodeID()));
   QToolButton* clearIDButton = new QToolButton;
   clearIDButton->setDefaultAction(clearIDAction);
   toolbar->addWidget(clearIDButton);
   
   //
   // Add a separator to the toolbar
   //
   toolbar->addSeparator();
   
   //
   // clear toolbar button.
   //
   QAction* clearAction = new QAction("Clear", this);
   clearAction->setToolTip(
                   "Clear all identification\n"
                   "information from the \n"
                   "text window."); 
   QObject::connect(clearAction, SIGNAL(triggered()),
                    textDisplayBrowser, SLOT(clear()));
   QToolButton* clearButton = new QToolButton;
   clearButton->setDefaultAction(clearAction);
   toolbar->addWidget(clearButton);
          
   //
   // Copy highlighted text to clipboard
   //
/*
   QAction* copyAction = new QAction("Copy", this);
   copyAction->setEnabled(false);
   QObject::connect(copyAction, SIGNAL(triggered(bool)),
                    textDisplayBrowser, SLOT(copy()));
   copyAction->setToolTip(
                 "Highlight text with the mouse and\n"
                 "then press this button to copy\n"
                 "the highlighted text to the clipboard.");
*/
   QToolButton* copyButton = new QToolButton;
   copyButton->setText("Copy");
   //copyButton->setDefaultAction(copyAction);
   QObject::connect(copyButton, SIGNAL(clicked()),
                    textDisplayBrowser, SLOT(copy()));
   copyButton->setToolTip(
                 "Highlight text with the mouse and\n"
                 "then press this button to copy\n"
                 "the highlighted text to the clipboard.");
   toolbar->addWidget(copyButton);

   //
   // Enable the copy button when text is highlighted
   //
   QObject::connect(textDisplayBrowser, SIGNAL(copyAvailable(bool)),
                    copyButton, SLOT(setEnabled(bool)));

   //
   // Add a separator to the toolbar
   //
   toolbar->addSeparator();
   
   //
   // Show Identified Nodes
   // 
   showIDAction = new QAction("SID", this);
   showIDAction->setCheckable(true);
   showIDAction->setToolTip(
                 " Enable green ID\n"
                 "symbols on surface.");
   showIDAction->setChecked(true);
   QObject::connect(showIDAction, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowIDAction(bool)));
   QToolButton* showIDButton = new QToolButton;
   showIDButton->setDefaultAction(showIDAction);
   toolbar->addWidget(showIDButton);
   
   return toolbar;
}
      
/**
 * create the text display widget.
 */
QWidget* 
GuiIdentifyMainWindow::createTextDisplayWidget()
{
   //
   // Text Editor displays identification information
   //   
   textDisplayBrowser = new GuiHyperLinkTextBrowser(0); //QTextBrowser(vbox, "textDisplayBrowser");  //new QTextEdit(vbox);
   textDisplayBrowser->setReadOnly(true);
   textDisplayBrowser->resize(QSize(textDisplayBrowser->sizeHint().width(), 100));
   
   return textDisplayBrowser;
}
      
/**
 * create the widget for the display filtering.
 */
QWidget* 
GuiIdentifyMainWindow::createDisplayFilterWidget()
{
   //
   // all on/off buttons
   //
   QPushButton* allOnPushButton = new QPushButton("All On");
   allOnPushButton->setAutoDefault(false);
   QObject::connect(allOnPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAllOn()));
   QPushButton* allOffPushButton = new QPushButton("All Off");
   allOffPushButton->setAutoDefault(false);
   QObject::connect(allOffPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAllOff()));
   QHBoxLayout* allOnOffButtonLayout = new QHBoxLayout;
   allOnOffButtonLayout->addWidget(allOnPushButton);
   allOnOffButtonLayout->addWidget(allOffPushButton);
   allOnOffButtonLayout->addStretch();
   
   //
   // show node  info check box  
   //
   showNodeArealEstInfoCheckBox = new QCheckBox("Areal Estimation");
   QObject::connect(showNodeArealEstInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodeArealEstAction(bool)));
                    
   //
   // show node coord info check box  
   //
   showNodeCoordInfoCheckBox = new QCheckBox("Coordinate");
   QObject::connect(showNodeCoordInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodeCoordAction(bool)));
                    
   //
   // show node lat/lon info check box  
   //
   showNodeLatLonInfoCheckBox = new QCheckBox("Latitude/Longitude");
   QObject::connect(showNodeLatLonInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodeLatLonAction(bool)));
                    
   //
   // show node metric info check box  
   //
   showNodeMetricInfoCheckBox = new QCheckBox("Metric");
   QObject::connect(showNodeMetricInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodeMetricAction(bool)));
                    
   //
   // show node paint info check box  
   //
   showNodePaintInfoCheckBox = new QCheckBox("Paint");
   QObject::connect(showNodePaintInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodePaintAction(bool)));
                    
   //
   // show node prob atlas info check box  
   //
   showNodeProbAtlasInfoCheckBox = new QCheckBox("Probabilistic Atlas");
   QObject::connect(showNodeProbAtlasInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodeProbAtlasAction(bool)));
                    
   //
   // show node rgb paint info check box  
   //
   showNodeRgbPaintInfoCheckBox = new QCheckBox("RGB Paint");
   QObject::connect(showNodeRgbPaintInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodeRgbPaintAction(bool)));
                    
   //
   // show node section info check box  
   //
   showNodeSectionInfoCheckBox = new QCheckBox("Section");
   QObject::connect(showNodeSectionInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodeSectionAction(bool)));
                    
   //
   // show node shape info check box  
   //
   showNodeShapeInfoCheckBox = new QCheckBox("Shape");
   QObject::connect(showNodeShapeInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodeShapeAction(bool)));
                    
   //
   // show node topography info check box  
   //
   showNodeTopographyInfoCheckBox = new QCheckBox("Topography");
   QObject::connect(showNodeTopographyInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodeTopographyAction(bool)));
                    
   //
   // show node info group box
   //
   showNodeInfoGroupBox = new QGroupBox("Show Identified Node Information");
   showNodeInfoGroupBox->setCheckable(true);
   QObject::connect(showNodeInfoGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotNodeAction(bool)));
   QVBoxLayout* nodeGroupLayout = new QVBoxLayout(showNodeInfoGroupBox);
   nodeGroupLayout->addWidget(showNodeArealEstInfoCheckBox);
   nodeGroupLayout->addWidget(showNodeCoordInfoCheckBox);
   nodeGroupLayout->addWidget(showNodeLatLonInfoCheckBox);
   nodeGroupLayout->addWidget(showNodeMetricInfoCheckBox);
   nodeGroupLayout->addWidget(showNodePaintInfoCheckBox);
   nodeGroupLayout->addWidget(showNodeProbAtlasInfoCheckBox);
   nodeGroupLayout->addWidget(showNodeRgbPaintInfoCheckBox);
   nodeGroupLayout->addWidget(showNodeSectionInfoCheckBox);
   nodeGroupLayout->addWidget(showNodeShapeInfoCheckBox);
   nodeGroupLayout->addWidget(showNodeTopographyInfoCheckBox);
   
   //
   // Show border info check box
   //
   showBorderInfoCheckBox = new QCheckBox("Show Identified Border Information");
   QObject::connect(showBorderInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotBorderAction(bool)));
                    
   //
   // Show cell info check box
   //
   showCellInfoCheckBox = new QCheckBox("Show Identified Cell Information");
   QObject::connect(showCellInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotCellAction(bool)));
                    
   //
   // Show contour info check box
   //
   showContourInfoCheckBox = new QCheckBox("Show Identified Contour Information");
   QObject::connect(showContourInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotContourAction(bool)));
                    
   //
   // show foci name info check box
   //
   showFociNameInfoCheckBox = new QCheckBox("Name");
   QObject::connect(showFociNameInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociNameAction(bool)));
                    
   //
   // show foci class info check box
   //
   showFociClassInfoCheckBox = new QCheckBox("Class");
   QObject::connect(showFociClassInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociClassAction(bool)));
                    
   //
   // show original stereotaxic position info check box
   //
   showFociOriginalStereotaxicPositionInfoCheckBox = new QCheckBox("Stereotaxic Position (Original)");
   QObject::connect(showFociOriginalStereotaxicPositionInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociOriginalStereotaxicPositionAction(bool)));
                    
   //
   // show stereotaxic position info check box
   //
   showFociStereotaxicPositionInfoCheckBox = new QCheckBox("Stereotaxic Position");
   QObject::connect(showFociStereotaxicPositionInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociStereotaxicPositionAction(bool)));
                    
   //
   // show foci area info check box
   //
   showFociAreaInfoCheckBox = new QCheckBox("Area");
   QObject::connect(showFociAreaInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociAreaAction(bool)));
                    
   //
   // show foci geography info check box
   //
   showFociGeographyInfoCheckBox = new QCheckBox("Geography");
   QObject::connect(showFociGeographyInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociGeographyAction(bool)));
                    
   //
   // show foci region of interest info check box
   //
   showFociRegionOfInterestInfoCheckBox = new QCheckBox("Region of Interest");
   QObject::connect(showFociRegionOfInterestInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociRegionOfInterestAction(bool)));
                    
   //
   // show foci size info check box
   //
   showFociSizeInfoCheckBox = new QCheckBox("Extent");
   QObject::connect(showFociSizeInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociSizeAction(bool)));
                    
   //
   // show foci statistic info check box
   //
   showFociStatisticInfoCheckBox = new QCheckBox("Statistic");
   QObject::connect(showFociStatisticInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociStatisticAction(bool)));
                    
   //
   // show foci structure info check box
   //
   showFociStructureInfoCheckBox = new QCheckBox("Structure");
   QObject::connect(showFociStructureInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociStructureAction(bool)));
                    
   //
   // show foci comment info check box
   //
   showFociCommentInfoCheckBox = new QCheckBox("Comment");
   QObject::connect(showFociCommentInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociCommentAction(bool)));
                    
   // Show foci info check box
   //
   showFociInfoGroupBox = new QGroupBox("Show Identified Foci Information");
   showFociInfoGroupBox->setCheckable(true);
   QObject::connect(showFociInfoGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotFociAction(bool)));
   QVBoxLayout* fociGroupLayout = new QVBoxLayout(showFociInfoGroupBox);
   fociGroupLayout->addWidget(showFociAreaInfoCheckBox);
   fociGroupLayout->addWidget(showFociClassInfoCheckBox);
   fociGroupLayout->addWidget(showFociCommentInfoCheckBox);
   fociGroupLayout->addWidget(showFociGeographyInfoCheckBox);
   fociGroupLayout->addWidget(showFociNameInfoCheckBox);
   fociGroupLayout->addWidget(showFociRegionOfInterestInfoCheckBox);
   fociGroupLayout->addWidget(showFociSizeInfoCheckBox);
   fociGroupLayout->addWidget(showFociOriginalStereotaxicPositionInfoCheckBox);
   fociGroupLayout->addWidget(showFociStereotaxicPositionInfoCheckBox);
   fociGroupLayout->addWidget(showFociStatisticInfoCheckBox);
   fociGroupLayout->addWidget(showFociStructureInfoCheckBox);
   
   //
   // Show voxel info check box
   //
   showVoxelInfoCheckBox = new QCheckBox("Show Identified Voxel Information");
   QObject::connect(showVoxelInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotVoxelAction(bool)));
                    
   //
   // show study title info check box
   //
   showStudyTitleInfoCheckBox = new QCheckBox("Title");
   QObject::connect(showStudyTitleInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyTitleInfoCheckBox(bool)));
                    
   //
   // show study author info check box
   //
   showStudyAuthorInfoCheckBox = new QCheckBox("Authors");
   QObject::connect(showStudyAuthorInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyAuthorInfoCheckBox(bool)));
   
   //
   // show study citation info check box
   //
   showStudyCitationInfoCheckBox = new QCheckBox("Citation");
   QObject::connect(showStudyCitationInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyCitationInfoCheckBox(bool)));
   
   //
   // show study comment info check box
   //
   showStudyCommentInfoCheckBox = new QCheckBox("Comment");
   QObject::connect(showStudyCommentInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyCommentInfoCheckBox(bool)));
   
   //
   // show study data format info check box
   //
   showStudyDataFormatInfoCheckBox = new QCheckBox("Data Format");
   QObject::connect(showStudyDataFormatInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyDataFormatInfoCheckBox(bool)));
   
   //
   // show study data type info check box
   //
   showStudyDataTypeInfoCheckBox = new QCheckBox("Data Type");
   QObject::connect(showStudyDataTypeInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyDataTypeInfoCheckBox(bool)));
   
   //
   // show study DOI info check box
   //
   showStudyDOIInfoCheckBox = new QCheckBox("DOI (Document Object Identifier)");
   QObject::connect(showStudyDOIInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyDOIInfoCheckBox(bool)));
   
   //
   // show study keywords info check box
   //
   showStudyKeywordsInfoCheckBox = new QCheckBox("Keywords");
   QObject::connect(showStudyKeywordsInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyKeywordsInfoCheckBox(bool)));
   
   //
   // show study medical subject headings info check box
   //
   showStudyMedicalSubjectHeadingInfoCheckBox = new QCheckBox("Medical Subject Headings");
   QObject::connect(showStudyMedicalSubjectHeadingInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyMedicalSubjectHeadingInfoCheckBox(bool)));
                    
   //
   // show study name info check box
   //
   showStudyNameInfoCheckBox = new QCheckBox("Name");
   QObject::connect(showStudyNameInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyNameInfoCheckBox(bool)));
                    
   //
   // show study part scheme abbrev info check box
   //
   showStudyPartSchemeAbbrevInfoCheckBox = new QCheckBox("Partitioning Scheme Abbreviation");
   QObject::connect(showStudyPartSchemeAbbrevInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPartSchemeAbbrevInfoCheckBox(bool)));
   
   //
   // show study part scheme full info check box
   //
   showStudyPartSchemeFullInfoCheckBox = new QCheckBox("Partitioning Scheme Full Name");
   QObject::connect(showStudyPartSchemeFullInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPartSchemeFullInfoCheckBox(bool)));
   
   //
   // show study PubMed ID info check box
   //
   showStudyPubMedIDInfoCheckBox = new QCheckBox("PubMed ID");
   QObject::connect(showStudyPubMedIDInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPubMedIDInfoCheckBox(bool)));
   
   //
   // show study project ID info check box
   //
   showStudyProjectIDInfoCheckBox = new QCheckBox("Project ID");
   QObject::connect(showStudyProjectIDInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyProjectIDInfoCheckBox(bool)));
   
   //
   // show study stereotaxic space info check box
   //
   showStudyStereotaxicSpaceInfoCheckBox = new QCheckBox("Stereotaxic Space");
   QObject::connect(showStudyStereotaxicSpaceInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyStereotaxicSpaceInfoCheckBox(bool)));
   
   //
   // show study stereotaxic space details info check box
   //
   showStudyStereotaxicSpaceDetailsInfoCheckBox = new QCheckBox("Stereotaxic Space Details");
   QObject::connect(showStudyStereotaxicSpaceDetailsInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyStereotaxicSpaceDetailsInfoCheckBox(bool)));
   
   //
   // show study URL info check box
   //
   showStudyURLInfoCheckBox = new QCheckBox("URL");
   QObject::connect(showStudyURLInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyURLInfoCheckBox(bool)));
   
   //
   // show study table header info check box
   //
   showStudyTableHeaderInfoCheckBox = new QCheckBox("Header");
   QObject::connect(showStudyTableHeaderInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyTableHeaderInfoCheckBox(bool)));

   //
   // show study table footer info check box
   //
   showStudyTableFooterInfoCheckBox = new QCheckBox("Footer");
   QObject::connect(showStudyTableFooterInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyTableFooterInfoCheckBox(bool)));

   //
   // show study table size units info check box
   //
   showStudyTableSizeUnitsInfoCheckBox = new QCheckBox("Size Units");
   QObject::connect(showStudyTableSizeUnitsInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyTableSizeUnitsInfoCheckBox(bool)));

   //
   // show study table voxel size info check box
   //
   showStudyTableVoxelSizeInfoCheckBox = new QCheckBox("Voxel Size");
   QObject::connect(showStudyTableVoxelSizeInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyTableVoxelSizeInfoCheckBox(bool)));

   //
   // show study table statistic info check box
   //
   showStudyTableStatisticInfoCheckBox = new QCheckBox("Statistic Type");
   QObject::connect(showStudyTableStatisticInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyTableStatisticInfoCheckBox(bool)));

   //
   // show study table statistic description info check box
   //
   showStudyTableStatisticDescriptionInfoCheckBox = new QCheckBox("Statistic Description");
   QObject::connect(showStudyTableStatisticDescriptionInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyTableStatisticDescriptionInfoCheckBox(bool)));

   //
   // show study table info group box
   //
   showStudyTableInfoGroupBox = new QGroupBox("Tables");
   showStudyTableInfoGroupBox->setCheckable(true);
   QObject::connect(showStudyTableInfoGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyTableInfoGroupBox(bool)));
   QVBoxLayout* studyTableLayout = new QVBoxLayout(showStudyTableInfoGroupBox);
   studyTableLayout->addWidget(showStudyTableFooterInfoCheckBox);
   studyTableLayout->addWidget(showStudyTableHeaderInfoCheckBox);
   studyTableLayout->addWidget(showStudyTableSizeUnitsInfoCheckBox);
   studyTableLayout->addWidget(showStudyTableStatisticInfoCheckBox);
   studyTableLayout->addWidget(showStudyTableStatisticDescriptionInfoCheckBox);
   studyTableLayout->addWidget(showStudyTableVoxelSizeInfoCheckBox);
   
   //
   // show study figure legend info check box
   //
   showStudyFigureLegendInfoCheckBox = new QCheckBox("Legend");
   QObject::connect(showStudyFigureLegendInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyFigureLegendInfoCheckBox(bool)));
   
   //
   // figure panel description check box
   //
   showStudyFigurePanelDescriptionInfoCheckBox = new QCheckBox("Description");
   QObject::connect(showStudyFigurePanelDescriptionInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyFigurePanelDescriptionInfoCheckBox(bool)));
                    
   //
   // figure panel task description check box
   //
   showStudyFigurePanelTaskDescriptionInfoCheckBox = new QCheckBox("Task Description");
   QObject::connect(showStudyFigurePanelTaskDescriptionInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyFigurePanelTaskDescriptionInfoCheckBox(bool)));
                    
   //
   // figure panel task baseline check box
   //
   showStudyFigurePanelTaskBaselineInfoCheckBox = new QCheckBox("Task Baseline");
   QObject::connect(showStudyFigurePanelTaskBaselineInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyFigurePanelTaskBaselineInfoCheckBox(bool)));
                    
   //
   // figure panel test attributes check box
   //
   showStudyFigurePanelTestAttributesInfoCheckBox = new QCheckBox("Test Attributes");
   QObject::connect(showStudyFigurePanelTestAttributesInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyFigurePanelTestAttributesInfoCheckBox(bool)));
                    
   //
   // Figure panel group box and layout
   //
   showStudyFigurePanelInfoGroupBox = new QGroupBox("Panel");
   showStudyFigurePanelInfoGroupBox->setCheckable(true);
   QObject::connect(showStudyFigurePanelInfoGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyFigurePanelInfoGroupBox(bool)));
   QVBoxLayout* studyFigurePanelLayout = new QVBoxLayout(showStudyFigurePanelInfoGroupBox);
   studyFigurePanelLayout->addWidget(showStudyFigurePanelDescriptionInfoCheckBox);
   studyFigurePanelLayout->addWidget(showStudyFigurePanelTaskDescriptionInfoCheckBox);
   studyFigurePanelLayout->addWidget(showStudyFigurePanelTaskBaselineInfoCheckBox);
   studyFigurePanelLayout->addWidget(showStudyFigurePanelTestAttributesInfoCheckBox);
   
   //
   // show study figure info group box and layout
   //
   showStudyFigureInfoGroupBox = new QGroupBox("Figures");
   showStudyFigureInfoGroupBox->setCheckable(true);
   QObject::connect(showStudyFigureInfoGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyFigureInfoGroupBox(bool)));
   QVBoxLayout* studyFigureLayout = new QVBoxLayout(showStudyFigureInfoGroupBox);
   studyFigureLayout->addWidget(showStudyFigureLegendInfoCheckBox);
   studyFigureLayout->addWidget(showStudyFigurePanelInfoGroupBox);
   
   //
   // show study sub header name check box
   //
   showStudySubHeaderNameInfoCheckBox = new QCheckBox("Name");
   QObject::connect(showStudySubHeaderNameInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudySubHeaderNameInfoCheckBox(bool)));
                    
   //
   // show study sub header short name check box
   //
   showStudySubHeaderShortNameInfoCheckBox = new QCheckBox("Short Name");
   QObject::connect(showStudySubHeaderShortNameInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudySubHeaderShortNameInfoCheckBox(bool)));
                    
   //
   // show study sub header task description check box
   //
   showStudySubHeaderTaskDescriptionInfoCheckBox = new QCheckBox("Task Description");
   QObject::connect(showStudySubHeaderTaskDescriptionInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudySubHeaderTaskDescriptionInfoCheckBox(bool)));
                    
   //
   // show study sub header task baseline check box
   //
   showStudySubHeaderTaskBaselineInfoCheckBox = new QCheckBox("Task Baseline");
   QObject::connect(showStudySubHeaderTaskBaselineInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudySubHeaderTaskBaselineInfoCheckBox(bool)));
                    
   //
   // show study sub header test attributes check box
   //
   showStudySubHeaderTestAttributesInfoCheckBox = new QCheckBox("Test Attributes");
   QObject::connect(showStudySubHeaderTestAttributesInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudySubHeaderTestAttributesInfoCheckBox(bool)));
                    
   //
   // show study sub header group box and layout
   //
   showStudySubHeaderInfoGroupBox = new QGroupBox("Study Subheaders");
   showStudySubHeaderInfoGroupBox->setCheckable(true);
   QObject::connect(showStudySubHeaderInfoGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudySubHeaderInfoGroupBox(bool)));
   QVBoxLayout* studySubHeaderLayout = new QVBoxLayout(showStudySubHeaderInfoGroupBox);
   studySubHeaderLayout->addWidget(showStudySubHeaderNameInfoCheckBox);
   studySubHeaderLayout->addWidget(showStudySubHeaderShortNameInfoCheckBox);
   studySubHeaderLayout->addWidget(showStudySubHeaderTaskDescriptionInfoCheckBox);
   studySubHeaderLayout->addWidget(showStudySubHeaderTaskBaselineInfoCheckBox);
   studySubHeaderLayout->addWidget(showStudySubHeaderTestAttributesInfoCheckBox);
   
   //
   // show study page reference header info check box
   //
   showStudyPageReferenceHeaderInfoCheckBox = new QCheckBox("Header");
   QObject::connect(showStudyPageReferenceHeaderInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPageReferenceHeaderInfoCheckBox(bool)));
   
   //
   // show study page reference comment info check box
   //
   showStudyPageReferenceCommentInfoCheckBox = new QCheckBox("Comment");
   QObject::connect(showStudyPageReferenceCommentInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPageReferenceCommentInfoCheckBox(bool)));
   
   //
   // show study page reference size units info check box
   //
   showStudyPageReferenceSizeUnitsInfoCheckBox = new QCheckBox("Size Units");
   QObject::connect(showStudyPageReferenceSizeUnitsInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPageReferenceSizeUnitsInfoCheckBox(bool)));
   
   //
   // show study page reference voxel size info check box
   //
   showStudyPageReferenceVoxelSizeInfoCheckBox = new QCheckBox("Voxel Size");
   QObject::connect(showStudyPageReferenceVoxelSizeInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPageReferenceVoxelSizeInfoCheckBox(bool)));
   
   //
   // show study page reference statistic info check box
   //
   showStudyPageReferenceStatisticInfoCheckBox = new QCheckBox("Statistic");
   QObject::connect(showStudyPageReferenceStatisticInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPageReferenceStatisticInfoCheckBox(bool)));
   
   //
   // show study page reference statistic descriptioninfo check box
   //
   showStudyPageReferenceStatisticDescriptionInfoCheckBox = new QCheckBox("Statistic Description");
   QObject::connect(showStudyPageReferenceStatisticDescriptionInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPageReferenceStatisticDescriptionInfoCheckBox(bool)));
   
   //
   // show study page reference info check box
   //
   showStudyPageReferenceInfoGroupBox = new QGroupBox("Page Reference");
   showStudyPageReferenceInfoGroupBox->setCheckable(true);
   QObject::connect(showStudyPageReferenceInfoGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPageReferenceInfoGroupBox(bool)));
   QVBoxLayout* studyPageReferenceLayout = new QVBoxLayout(showStudyPageReferenceInfoGroupBox);
   studyPageReferenceLayout->addWidget(showStudyPageReferenceHeaderInfoCheckBox);
   studyPageReferenceLayout->addWidget(showStudyPageReferenceCommentInfoCheckBox);
   studyPageReferenceLayout->addWidget(showStudyPageReferenceSizeUnitsInfoCheckBox);
   studyPageReferenceLayout->addWidget(showStudyPageReferenceVoxelSizeInfoCheckBox);
   studyPageReferenceLayout->addWidget(showStudyPageReferenceStatisticInfoCheckBox);
   studyPageReferenceLayout->addWidget(showStudyPageReferenceStatisticDescriptionInfoCheckBox);
   
   //
   // show study page number info check box
   //
   showStudyPageNumberInfoCheckBox = new QCheckBox("Page Number");
   QObject::connect(showStudyPageNumberInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowStudyPageNumberInfoCheckBox(bool)));

   //
   // Show study info group box
   //
   showStudyInfoGroupBox = new QGroupBox("Show Study Information for Identified Items");
   showStudyInfoGroupBox->setCheckable(true);
   QObject::connect(showStudyInfoGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotStudyAction(bool)));
   QVBoxLayout* studyInfoGroupLayout = new QVBoxLayout(showStudyInfoGroupBox);
   studyInfoGroupLayout->addWidget(showStudyTitleInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyAuthorInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyCitationInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyCommentInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyDataFormatInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyDataTypeInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyDOIInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyKeywordsInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyMedicalSubjectHeadingInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyNameInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyPartSchemeAbbrevInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyPartSchemeFullInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyPubMedIDInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyProjectIDInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyStereotaxicSpaceInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyStereotaxicSpaceDetailsInfoCheckBox);
   studyInfoGroupLayout->addWidget(showStudyURLInfoCheckBox);
   studyInfoGroupLayout->addWidget(new QLabel(" "));
   studyInfoGroupLayout->addWidget(showStudyTableInfoGroupBox);
   studyInfoGroupLayout->addWidget(new QLabel(" "));
   studyInfoGroupLayout->addWidget(showStudyFigureInfoGroupBox);
   studyInfoGroupLayout->addWidget(new QLabel(" "));
   studyInfoGroupLayout->addWidget(showStudySubHeaderInfoGroupBox);
   studyInfoGroupLayout->addWidget(new QLabel(" "));
   studyInfoGroupLayout->addWidget(showStudyPageReferenceInfoGroupBox);
   studyInfoGroupLayout->addWidget(new QLabel(" "));
   studyInfoGroupLayout->addWidget(showStudyPageNumberInfoCheckBox);
   
   //
   // Meta-Analysis 
   //
   showStudyMetaAnalysisNameInfoCheckBox = new QCheckBox("Name");
   QObject::connect(showStudyMetaAnalysisNameInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowMetaAnalysisNameInfoCheckBox(bool)));
                    
   showStudyMetaAnalysisTitleInfoCheckBox = new QCheckBox("Title");
   QObject::connect(showStudyMetaAnalysisTitleInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowMetaAnalysisTitleInfoCheckBox(bool)));
                    
   showStudyMetaAnalysisAuthorInfoCheckBox = new QCheckBox("Authors");
   QObject::connect(showStudyMetaAnalysisAuthorInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowMetaAnalysisAuthorsInfoCheckBox(bool)));
                    
   showStudyMetaAnalysisCitationInfoCheckBox = new QCheckBox("Citation");
   QObject::connect(showStudyMetaAnalysisCitationInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowMetaAnalysisCitationInfoCheckBox(bool)));
                    
   showStudyMetaAnalysisDOIInfoCheckBox = new QCheckBox("DOI/URL");
   QObject::connect(showStudyMetaAnalysisDOIInfoCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowMetaAnalysisDoiUrlInfoCheckBox(bool)));
                    
   showStudyMetaAnalysisInfoGroupBox = new QGroupBox("Show Meta-Analysis for Identified Items");
   showStudyMetaAnalysisInfoGroupBox->setCheckable(true);
   QObject::connect(showStudyMetaAnalysisInfoGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowMetaAnalysisInfoGroupBox(bool)));
   QVBoxLayout* metaAnalysisGroupLayout = new QVBoxLayout(showStudyMetaAnalysisInfoGroupBox);                 
   metaAnalysisGroupLayout->addWidget(showStudyMetaAnalysisNameInfoCheckBox);
   metaAnalysisGroupLayout->addWidget(showStudyMetaAnalysisTitleInfoCheckBox);
   metaAnalysisGroupLayout->addWidget(showStudyMetaAnalysisAuthorInfoCheckBox);
   metaAnalysisGroupLayout->addWidget(showStudyMetaAnalysisCitationInfoCheckBox);
   metaAnalysisGroupLayout->addWidget(showStudyMetaAnalysisDOIInfoCheckBox);
   
   //
   // Display significant digits spin box
   //
   QLabel* sigDigitLabel = new QLabel("Digits Right of Decimal ");
   significantDigitsSpinBox = new QSpinBox;
   significantDigitsSpinBox->setMinimum(0);
   significantDigitsSpinBox->setMaximum(128);
   significantDigitsSpinBox->setSingleStep(1);
   QObject::connect(significantDigitsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSignificantDigitsSpinBox(int)));
   significantDigitsSpinBox->setToolTip(
                 "Controls number of digits\n"
                 "displayed right of decimal.");
   QHBoxLayout* sigDigitsLayout = new QHBoxLayout;
   sigDigitsLayout->addWidget(sigDigitLabel);
   sigDigitsLayout->addWidget(significantDigitsSpinBox);
   sigDigitsLayout->addStretch();
   
   //
   // Widget for filter items
   //
   QWidget* w = new QWidget;
   QVBoxLayout* l = new QVBoxLayout(w);
   l->setSpacing(15);
   l->addLayout(allOnOffButtonLayout);
   l->addLayout(sigDigitsLayout);
   l->addWidget(showNodeInfoGroupBox);
   l->addWidget(showBorderInfoCheckBox);
   l->addWidget(showCellInfoCheckBox);
   l->addWidget(showContourInfoCheckBox);
   l->addWidget(showFociInfoGroupBox);
   l->addWidget(showVoxelInfoCheckBox);
   l->addWidget(showStudyInfoGroupBox);
   l->addWidget(showStudyMetaAnalysisInfoGroupBox);
   
   //
   // Scroll area for filter items
   //
   QScrollArea* sa = new QScrollArea;
   sa->setWidget(w);
   sa->setWidgetResizable(true);
   
   return sa;
}

/**
 * update the filtering selections.
 */
void 
GuiIdentifyMainWindow::updateFilteringSelections()
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();

   showNodeInfoGroupBox->setChecked(bmi->getDisplayNodeInformation());
   showNodeArealEstInfoCheckBox->setChecked(bmi->getDisplayNodeArealEstInformation());
   showNodeCoordInfoCheckBox->setChecked(bmi->getDisplayNodeCoordInformation());
   showNodeLatLonInfoCheckBox->setChecked(bmi->getDisplayNodeLatLonInformation());
   showNodeMetricInfoCheckBox->setChecked(bmi->getDisplayNodeMetricInformation());
   showNodePaintInfoCheckBox->setChecked(bmi->getDisplayNodePaintInformation());
   showNodeProbAtlasInfoCheckBox->setChecked(bmi->getDisplayNodeProbAtlasInformation());
   showNodeRgbPaintInfoCheckBox->setChecked(bmi->getDisplayNodeRgbPaintInformation());
   showNodeSectionInfoCheckBox->setChecked(bmi->getDisplayNodeSectionInformation());
   showNodeShapeInfoCheckBox->setChecked(bmi->getDisplayNodeShapeInformation());
   showNodeTopographyInfoCheckBox->setChecked(bmi->getDisplayNodeTopographyInformation());
   showBorderInfoCheckBox->setChecked(bmi->getDisplayBorderInformation());
   showCellInfoCheckBox->setChecked(bmi->getDisplayCellInformation());
   showContourInfoCheckBox->setChecked(bmi->getDisplayContourInformation());
   showFociInfoGroupBox->setChecked(bmi->getDisplayFociInformation());
   showFociNameInfoCheckBox->setChecked(bmi->getDisplayFociNameInformation());
   showFociClassInfoCheckBox->setChecked(bmi->getDisplayFociClassInformation());
   showFociOriginalStereotaxicPositionInfoCheckBox->setChecked(bmi->getDisplayFociOriginalStereotaxicPositionInformation());
   showFociStereotaxicPositionInfoCheckBox->setChecked(bmi->getDisplayFociStereotaxicPositionInformation());
   showFociAreaInfoCheckBox->setChecked(bmi->getDisplayFociAreaInformation());
   showFociGeographyInfoCheckBox->setChecked(bmi->getDisplayFociGeographyInformation());
   showFociRegionOfInterestInfoCheckBox->setChecked(bmi->getDisplayFociRegionOfInterestInformation());
   showFociSizeInfoCheckBox->setChecked(bmi->getDisplayFociSizeInformation());
   showFociStatisticInfoCheckBox->setChecked(bmi->getDisplayFociStatisticInformation());
   showFociStructureInfoCheckBox->setChecked(bmi->getDisplayFociStructureInformation());
   showFociCommentInfoCheckBox->setChecked(bmi->getDisplayFociCommentInformation());
   showVoxelInfoCheckBox->setChecked(bmi->getDisplayVoxelInformation());
   showStudyMetaAnalysisInfoGroupBox->setChecked(bmi->getDisplayStudyMetaAnalysisInformation());
   showStudyMetaAnalysisNameInfoCheckBox->setChecked(bmi->getDisplayStudyMetaAnalysisNameInformation());
   showStudyMetaAnalysisTitleInfoCheckBox->setChecked(bmi->getDisplayStudyMetaAnalysisTitleInformation());
   showStudyMetaAnalysisAuthorInfoCheckBox->setChecked(bmi->getDisplayStudyMetaAnalysisAuthorsInformation());
   showStudyMetaAnalysisCitationInfoCheckBox->setChecked(bmi->getDisplayStudyMetaAnalysisCitationInformation());
   showStudyMetaAnalysisDOIInfoCheckBox->setChecked(bmi->getDisplayStudyMetaAnalysisDoiUrlInformation());
   showStudyInfoGroupBox->setChecked(bmi->getDisplayStudyInformation());
   showStudyNameInfoCheckBox->setChecked(bmi->getDisplayStudyNameInformation());
   showStudyTitleInfoCheckBox->setChecked(bmi->getDisplayStudyTitleInformation());
   showStudyAuthorInfoCheckBox->setChecked(bmi->getDisplayStudyAuthorsInformation());
   showStudyCitationInfoCheckBox->setChecked(bmi->getDisplayStudyCitationInformation());
   showStudyCommentInfoCheckBox->setChecked(bmi->getDisplayStudyCommentInformation());
   showStudyDataFormatInfoCheckBox->setChecked(bmi->getDisplayStudyDataFormatInformation());
   showStudyDataTypeInfoCheckBox->setChecked(bmi->getDisplayStudyDataTypeInformation());
   showStudyDOIInfoCheckBox->setChecked(bmi->getDisplayStudyDOIInformation());
   showStudyKeywordsInfoCheckBox->setChecked(bmi->getDisplayStudyKeywordsInformation());
   showStudyMedicalSubjectHeadingInfoCheckBox->setChecked(bmi->getDisplayStudyMedicalSubjectHeadingsInformation());
   showStudyPartSchemeAbbrevInfoCheckBox->setChecked(bmi->getDisplayStudyPartSchemeAbbrevInformation());
   showStudyPartSchemeFullInfoCheckBox->setChecked(bmi->getDisplayStudyPartSchemeFullInformation());
   showStudyPubMedIDInfoCheckBox->setChecked(bmi->getDisplayStudyPubMedIDInformation());
   showStudyProjectIDInfoCheckBox->setChecked(bmi->getDisplayStudyProjectIDInformation());
   showStudyStereotaxicSpaceInfoCheckBox->setChecked(bmi->getDisplayStudyStereotaxicSpaceInformation());
   showStudyStereotaxicSpaceDetailsInfoCheckBox->setChecked(bmi->getDisplayStudyStereotaxicSpaceDetailsInformation());
   showStudyURLInfoCheckBox->setChecked(bmi->getDisplayStudyURLInformation());
   showStudyTableInfoGroupBox->setChecked(bmi->getDisplayStudyTableInformation());
   showStudyTableHeaderInfoCheckBox->setChecked(bmi->getDisplayStudyTableHeaderInformation());
   showStudyTableFooterInfoCheckBox->setChecked(bmi->getDisplayStudyTableFooterInformation());
   showStudyTableSizeUnitsInfoCheckBox->setChecked(bmi->getDisplayStudyTableSizeUnitsInformation());
   showStudyTableVoxelSizeInfoCheckBox->setChecked(bmi->getDisplayStudyTableVoxelSizeInformation());
   showStudyTableStatisticInfoCheckBox->setChecked(bmi->getDisplayStudyTableStatisticInformation());
   showStudyTableStatisticDescriptionInfoCheckBox->setChecked(bmi->getDisplayStudyTableStatisticDescriptionInformation());
   showStudyFigureInfoGroupBox->setChecked(bmi->getDisplayStudyFigureInformation());
   showStudyFigureLegendInfoCheckBox->setChecked(bmi->getDisplayStudyFigureLegendInformation());
   showStudyFigurePanelInfoGroupBox->setChecked(bmi->getDisplayStudyFigurePanelInformation());
   showStudyFigurePanelDescriptionInfoCheckBox->setChecked(bmi->getDisplayStudyFigurePanelDescriptionInformation());
   showStudyFigurePanelTaskDescriptionInfoCheckBox->setChecked(bmi->getDisplayStudyFigurePanelTaskDescriptionInformation());
   showStudyFigurePanelTaskBaselineInfoCheckBox->setChecked(bmi->getDisplayStudyFigurePanelTaskBaselineInformation());
   showStudyFigurePanelTestAttributesInfoCheckBox->setChecked(bmi->getDisplayStudyFigurePanelTestAttributesInformation());
   showStudySubHeaderInfoGroupBox->setChecked(bmi->getDisplayStudySubHeaderInformation());
   showStudySubHeaderNameInfoCheckBox->setChecked(bmi->getDisplayStudySubHeaderNameInformation());
   showStudySubHeaderShortNameInfoCheckBox->setChecked(bmi->getDisplayStudySubHeaderShortNameInformation());
   showStudySubHeaderTaskDescriptionInfoCheckBox->setChecked(bmi->getDisplayStudySubHeaderTaskDescriptionInformation());
   showStudySubHeaderTaskBaselineInfoCheckBox->setChecked(bmi->getDisplayStudySubHeaderTaskBaselineInformation());
   showStudySubHeaderTestAttributesInfoCheckBox->setChecked(bmi->getDisplayStudySubHeaderTestAttributesInformation());
   showStudyPageReferenceInfoGroupBox->setChecked(bmi->getDisplayStudyPageReferenceInformation());
   showStudyPageReferenceHeaderInfoCheckBox->setChecked(bmi->getDisplayStudyPageReferenceHeaderInformation());
   showStudyPageReferenceCommentInfoCheckBox->setChecked(bmi->getDisplayStudyPageReferenceCommentInformation());
   showStudyPageReferenceSizeUnitsInfoCheckBox->setChecked(bmi->getDisplayStudyPageReferenceSizeUnitsInformation());
   showStudyPageReferenceVoxelSizeInfoCheckBox->setChecked(bmi->getDisplayStudyPageReferenceVoxelSizeInformation());
   showStudyPageReferenceStatisticInfoCheckBox->setChecked(bmi->getDisplayStudyPageReferenceStatisticInformation());
   showStudyPageReferenceStatisticDescriptionInfoCheckBox->setChecked(bmi->getDisplayStudyPageReferenceStatisticDescriptionInformation());
   showStudyPageNumberInfoCheckBox->setChecked(bmi->getDisplayStudyPageNumberInformation());
   
   significantDigitsSpinBox->setValue(bmi->getSignificantDigits());
}
      
/**
 * clear all node ID symbols
 */
void 
GuiIdentifyMainWindow::clearNodeID()
{
   theMainWindow->getBrainSet()->clearNodeHighlightSymbols();
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->clearBorderHighlighting();
   FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   fpf->clearAllHighlightFlags();
   BrainModelContours* bmc = theMainWindow->getBrainSet()->getBrainModelContours(-1);
   if (bmc != NULL) {
      bmc->getContourFile()->clearHighlightFlags();
   }
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

/** 
 * Destructor.
 */
GuiIdentifyMainWindow::~GuiIdentifyMainWindow()
{
}

/**
 * append html.
 */
void 
GuiIdentifyMainWindow::appendHtml(const QString& html)
{
   textDisplayBrowser->appendHtml(html);

   //
   // Scroll to newest text (at end of scroll bar)
   //
   QScrollBar* vsb = textDisplayBrowser->verticalScrollBar();
   vsb->setValue(vsb->maximum());
}
      
/**
 * Append to the text in the text editor.
 */
void
GuiIdentifyMainWindow::appendText(const QString& qs)
{
   textDisplayBrowser->append(qs);

   //
   // Scroll to newest text (at end of scroll bar)
   //
   QScrollBar* vsb = textDisplayBrowser->verticalScrollBar();
   vsb->setValue(vsb->maximum());
}

/**
 *  all on button.
 */
void 
GuiIdentifyMainWindow::slotAllOn()
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setAllIdentificationOn();
   updateToolBarButtons();
   updateFilteringSelections();
}

/**
 *  all off button.
 */
void 
GuiIdentifyMainWindow::slotAllOff()
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setAllIdentificationOff();
   updateToolBarButtons();
   updateFilteringSelections();
}

/**
 * display border information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotBorderAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayBorderInformation(val);
}

/**
 * display cell information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotCellAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayCellInformation(val);
}

/**
 * display foci information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociAction(bool val)  
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociInformation(val);
}

/**
 * display foci name information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociNameAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociNameInformation(val);
}

/**
 * display foci class information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociClassAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociClassInformation(val);
}

/**
 * display foci original stereotaxic position information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociOriginalStereotaxicPositionAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociOriginalStereotaxicPositionInformation(val);
}    

/**
 * display foci stereotaxic position information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociStereotaxicPositionAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociStereotaxicPositionInformation(val);
}

/**
 * display foci area information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociAreaAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociAreaInformation(val);
}

/**
 * display foci geography information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociGeographyAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociGeographyInformation(val);
}

/**
 * display foci region of interest information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociRegionOfInterestAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociRegionOfInterestInformation(val);
}
      
/**
 * display foci size information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociSizeAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociSizeInformation(val);
}

/**
 * display foci Statistic information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociStatisticAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociStatisticInformation(val);
}

/**
 * display foci structure information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociStructureAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociStructureInformation(val);
}

/**
 * display foci comment information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotFociCommentAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayFociCommentInformation(val);
}
      
/**
 * display voxel information toolbar button.
 */
void 
GuiIdentifyMainWindow::slotVoxelAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayVoxelInformation(val);
}

/**
 * display contour information.
 */
void 
GuiIdentifyMainWindow::slotContourAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayContourInformation(val);
}
      

/**
 * display node information.
 */
void 
GuiIdentifyMainWindow::slotNodeAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodeInformation(val);
}
      
/**
 * display node coordinate information.
 */
void 
GuiIdentifyMainWindow::slotNodeCoordAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodeCoordInformation(val);
}

/**
 * display node lat/lon information.
 */
void 
GuiIdentifyMainWindow::slotNodeLatLonAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodeLatLonInformation(val);
}

/**
 * display node paint information.
 */
void 
GuiIdentifyMainWindow::slotNodePaintAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodePaintInformation(val);
}

/**
 * display node probabilistic atlas information.
 */
void 
GuiIdentifyMainWindow::slotNodeProbAtlasAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodeProbAtlasInformation(val);
}

/**
 * display node rgb paint information.
 */
void 
GuiIdentifyMainWindow::slotNodeRgbPaintAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodeRgbPaintInformation(val);
}

/**
 * display node metric information.
 */
void 
GuiIdentifyMainWindow::slotNodeMetricAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodeMetricInformation(val);
}

/**
 * display node shape information.
 */
void 
GuiIdentifyMainWindow::slotNodeShapeAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodeShapeInformation(val);
}

/**
 * display node section information.
 */
void 
GuiIdentifyMainWindow::slotNodeSectionAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodeSectionInformation(val);
}

/**
 * display node areal estimation information.
 */
void 
GuiIdentifyMainWindow::slotNodeArealEstAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodeArealEstInformation(val);
}

/**
 * display node topography information.
 */
void 
GuiIdentifyMainWindow::slotNodeTopographyAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayNodeTopographyInformation(val);
}

/**
 * show ID symbols on surface.
 */
void 
GuiIdentifyMainWindow::slotShowIDAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayIDSymbol(val);
}

/**
 * significant digits display.
 */
void 
GuiIdentifyMainWindow::slotSignificantDigitsSpinBox(int val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setSignificantDigits(val);
}

/**
 * display study information.
 */
void 
GuiIdentifyMainWindow::slotStudyAction(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyInformation(val);
}
      
/**
 * show study name info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyNameInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyNameInformation(val);
}

/**
 * show study title info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyTitleInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyTitleInformation(val);
}

/**
 * show study author info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyAuthorInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyAuthorsInformation(val);
}

/**
 * show study citation info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyCitationInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyCitationInformation(val);
}

/**
 * show study comment info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyCommentInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyCommentInformation(val);
}

/**
 * show study data format info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyDataFormatInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyDataFormatInformation(val);
}

/**
 * show study data type info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyDataTypeInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyDataTypeInformation(val);
}

/**
 * show study DOI info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyDOIInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyDOIInformation(val);
}

/**
 * show study keywords info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyKeywordsInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyKeywordsInformation(val);
}

/**
 * show study medical subject headings info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyMedicalSubjectHeadingInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyMedicalSubjectHeadingsInformation(val);
}

/**
 * show study meta-analysis info check box.
 */
void 
GuiIdentifyMainWindow::slotShowMetaAnalysisInfoGroupBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyMetaAnalysisInformation(val);
}
   
/**
 * show study meta-analysis name info check box.
 */
void 
GuiIdentifyMainWindow::slotShowMetaAnalysisNameInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyMetaAnalysisNameInformation(val);
}

/**
 * show study meta-analysis title info check box.
 */
void 
GuiIdentifyMainWindow::slotShowMetaAnalysisTitleInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyMetaAnalysisTitleInformation(val);
}

/**
 * show study meta-analysis authors info check box.
 */
void 
GuiIdentifyMainWindow::slotShowMetaAnalysisAuthorsInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyMetaAnalysisAuthorsInformation(val);
}

/**
 * show study meta-analysis citation info check box.
 */
void 
GuiIdentifyMainWindow::slotShowMetaAnalysisCitationInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyMetaAnalysisCitationInformation(val);
}

/**
 * show study meta-analysis DOI/URL info check box.
 */
void 
GuiIdentifyMainWindow::slotShowMetaAnalysisDoiUrlInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyMetaAnalysisDoiUrlInformation(val);
}

/**
 * show study part scheme abbrev info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyPartSchemeAbbrevInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPartSchemeAbbrevInformation(val);
}

/**
 * show study part scheme full info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyPartSchemeFullInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPartSchemeFullInformation(val);
}

/**
 * show study PubMed ID info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyPubMedIDInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPubMedIDInformation(val);
}

/**
 * show study project ID info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyProjectIDInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyProjectIDInformation(val);
}

/**
 * show study stereotaxic space info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyStereotaxicSpaceInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyStereotaxicSpaceInformation(val);
}

/**
 * show study stereotaxic space details info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyStereotaxicSpaceDetailsInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyStereotaxicSpaceDetailsInformation(val);
}

/**
 * show study URL info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyURLInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyURLInformation(val);
}

/**
 * show study table info group box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyTableInfoGroupBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyTableInformation(val);
}

/**
 * show study table header info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyTableHeaderInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyTableHeaderInformation(val);
}

/**
 * show study table footer info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyTableFooterInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyTableFooterInformation(val);
}

/**
 * show study table size units info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyTableSizeUnitsInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyTableSizeUnitsInformation(val);
}

/**
 * show study table voxel size info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyTableVoxelSizeInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyTableVoxelSizeInformation(val);
}

/**
 * show study table statistic info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyTableStatisticInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyTableStatisticInformation(val);
}

/**
 * show study table statistic description info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyTableStatisticDescriptionInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyTableStatisticDescriptionInformation(val);
}

/**
 * show study figure info group box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyFigureInfoGroupBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyFigureInformation(val);
}

/**
 * show study figure legend info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyFigureLegendInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyFigureLegendInformation(val);
}

/**
 * show study figure panel info group box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyFigurePanelInfoGroupBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyFigurePanelInformation(val);
}

/**
 * show study figure panel description info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyFigurePanelDescriptionInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyFigurePanelDescriptionInformation(val);
}

/**
 * show study figure panel task description info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyFigurePanelTaskDescriptionInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyFigurePanelTaskDescriptionInformation(val);
}

/**
 * show study figure panel task baseline info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyFigurePanelTaskBaselineInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyFigurePanelTaskBaselineInformation(val);
}

/**
 * show study figure panel test attributes info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyFigurePanelTestAttributesInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyFigurePanelTestAttributesInformation(val);
}

/**
 * show study subheader info group box.
 */
void 
GuiIdentifyMainWindow::slotShowStudySubHeaderInfoGroupBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudySubHeaderInformation(val);
}

/**
 * show study subheader name info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudySubHeaderNameInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudySubHeaderNameInformation(val);
}

/**
 * show study subheader short name info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudySubHeaderShortNameInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudySubHeaderShortNameInformation(val);
}

/**
 * show study subheader task description info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudySubHeaderTaskDescriptionInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudySubHeaderTaskDescriptionInformation(val);
}

/**
 * show study subheader task baseline info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudySubHeaderTaskBaselineInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudySubHeaderTaskBaselineInformation(val);
}

/**
 * show study subheader test attributes info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudySubHeaderTestAttributesInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudySubHeaderTestAttributesInformation(val);
}
      
/**
 * show study page reference info group box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyPageReferenceInfoGroupBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPageReferenceInformation(val);
}

/**
 * show study page reference header info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyPageReferenceHeaderInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPageReferenceHeaderInformation(val);
}

/**
 * show study page reference comment info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyPageReferenceCommentInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPageReferenceCommentInformation(val);
}

/**
 * show study page reference size units info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyPageReferenceSizeUnitsInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPageReferenceSizeUnitsInformation(val);
}

/**
 * show study page reference voxel size info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyPageReferenceVoxelSizeInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPageReferenceVoxelSizeInformation(val);
}

/**
 * show study page reference statistic info check box
 */
void 
GuiIdentifyMainWindow::slotShowStudyPageReferenceStatisticInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPageReferenceStatisticInformation(val);
}

/**
 * show study page reference statistic description info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyPageReferenceStatisticDescriptionInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPageReferenceStatisticDescriptionInformation(val);
}
      
/**
 * show study page number info check box.
 */
void 
GuiIdentifyMainWindow::slotShowStudyPageNumberInfoCheckBox(bool val)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   bmi->setDisplayStudyPageNumberInformation(val);
}

/**
 * update the filtering toggle buttons.
 */
void 
GuiIdentifyMainWindow::updateToolBarButtons()
{
   const BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   showIDAction->setChecked(bmi->getDisplayIDSymbol());
}
      
//=================================================================================

/**
 * The constructor.
 */
GuiIdentifyDialog::GuiIdentifyDialog(QWidget* parent)
   : WuQDialog(parent)
{
   //resize(400, 200);

   setWindowTitle("Identify Window");
   
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   
   //
   // main window containing toolbar and text area
   //
   idMainWindow = new GuiIdentifyMainWindow;
   dialogLayout->addWidget(idMainWindow);
   
   //
   // Close Button
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   
   QPushButton* close = new QPushButton("Close");
   QObject::connect(close, SIGNAL(clicked()),
                    this, SLOT(accept()));
   close->setFixedSize(close->sizeHint());
   close->setAutoDefault(false);
   buttonsLayout->addWidget(close);
   
   resize(650, 200);
   
   updateDialog();
}

/**
 * The destructor.
 */
GuiIdentifyDialog::~GuiIdentifyDialog()
{
}

/**
 * append to the text display.
 */
void 
GuiIdentifyDialog::appendText(const QString& s)
{
   idMainWindow->appendText(s);
   show();
}
      
/**
 * append html to the text display.
 */
void 
GuiIdentifyDialog::appendHtml(const QString& s)
{
   idMainWindow->appendHtml(s);
   show();
}
      
/**
 * apply a scene (update dialog).
 */
void 
GuiIdentifyDialog::showScene(const SceneFile::Scene& scene,
                             const int mainWindowX,
                             const int mainWindowY,
                             const int mainWindowSceneX,
                             const int mainWindowSceneY,
                             const int screenMaxX,
                             const int screenMaxY,
                             QString& /*errorMessage*/)
{
   bool idDialogFound = false;
   
   const int numClasses = scene.getNumberOfSceneClasses();
   
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "GuiIdentifyDialog") {
         idDialogFound = true;
         
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();
            if (infoName == "Geometry") {
               std::vector<QString> tokens;
               StringUtilities::token(si->getValueAsString(), " ", tokens);
               if (tokens.size() >= 4) {
                  int geometry[4] = {
                     StringUtilities::toInt(tokens[0]),
                     StringUtilities::toInt(tokens[1]),
                     StringUtilities::toInt(tokens[2]),
                     StringUtilities::toInt(tokens[3])
                  };
                  
                  geometry[0] = std::min(geometry[0], screenMaxX);
                  geometry[1] = std::min(geometry[1], screenMaxY);
                  
                  const DisplaySettingsScene* dss = theMainWindow->getBrainSet()->getDisplaySettingsScene();
                  switch(dss->getWindowPositionPreference()) {
                     case DisplaySettingsScene::WINDOW_POSITIONS_USE_ALL:
                        move(geometry[0], geometry[1]);
                        resize(geometry[2], geometry[3]);
                        break;
                     case DisplaySettingsScene::WINDOW_POSITIONS_IGNORE_MAIN_OTHERS_RELATIVE:
                        geometry[0] = (geometry[0] - mainWindowSceneX) + mainWindowX;
                        geometry[1] = (geometry[1] - mainWindowSceneY) + mainWindowY;
                        move(geometry[0], geometry[1]);
                        resize(geometry[2], geometry[3]);
                        break;
                     case DisplaySettingsScene::WINDOW_POSITIONS_IGNORE_ALL:
                        break;
                  }
               }
            }
         }
         
         show();
      }
   }
   
   if (idDialogFound) {
      //
      // Get the highlighted nodes
      //
      std::vector<int> highlightedNodes;
      std::vector<BrainSetNodeAttribute::HIGHLIGHT_NODE_TYPE> highlightedType;
      const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
      for (int i = 0; i < numNodes; i++) {
         BrainSetNodeAttribute* bna = theMainWindow->getBrainSet()->getNodeAttributes(i);
         if (bna->getHighlighting() != BrainSetNodeAttribute::HIGHLIGHT_NODE_NONE) {
            highlightedNodes.push_back(i);
            highlightedType.push_back(bna->getHighlighting());
         }
      }
       
      //
      // Need to clear and then set node highlighting.  This is because the nodes
      // have already been highlighted in the BrainSet.  Trying to highlight a 
      // node while it is already highlighted will clear its highlight just like
      // clicking a highlighted node removes it highlighting.
      //
      //JWH 01/16/2007 theMainWindow->getBrainSet()->clearNodeHighlightSymbols();
      BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
      QString idString;
      for (unsigned int i = 0; i < highlightedNodes.size(); i++) {
         idString += bmi->getIdentificationTextForNode(theMainWindow->getBrainSet(),
                                                       highlightedNodes[i],
                                                       true,
                                                       true);
      }
      appendHtml(idString);
   }
   else {
      close();
   }
}

/**
 * create a scene (save dialog settings).
 */
void 
GuiIdentifyDialog::saveScene(std::vector<SceneFile::SceneClass>& scs)
{
   if (isVisible()) {
      SceneFile::SceneClass sc("GuiIdentifyDialog");
      std::ostringstream str;
      str << x() << " "
          << y() << " "
          << width() << " "
          << height();
      sc.addSceneInfo(SceneFile::SceneInfo("Geometry", str.str().c_str()));
      
      scs.push_back(sc);
   }
}
  
/**
 * display vocabulary name in identify window.
 */
void 
GuiIdentifyDialog::displayVocabularyNameData(const QString& name)
{
   BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
   const QString txt = bmi->getIdentificationTextForVocabulary(true, name);
   if (txt.isEmpty() == false) {
      idMainWindow->appendHtml(txt);
      show();
      activateWindow();
   }
/*
   const VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();
   const VocabularyFile::VocabularyEntry* ve = vf->getBestMatchingVocabularyEntry(name);
   if (ve != NULL) {
      idMainWindow->appendText(ve->getFullDescriptionForDisplayToUser(true));
      show();
      activateWindow();
   }   
*/
}

/**
 * update the dialog.
 */
void 
GuiIdentifyDialog::updateDialog()
{
   idMainWindow->updateToolBarButtons();
   idMainWindow->updateFilteringSelections();
}      

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

#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX 
#endif

#include <algorithm>
#include <iostream>
#include <set>

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QCursor>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTextEdit>
#include <QToolButton>
#include <QToolTip>
#include <QValidator>

#include "AreaColorFile.h"
#include "ArealEstimationFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurfaceAndVolume.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelVolume.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "CellProjectionFile.h"
#include "CellColorFile.h"
#include "ContourCellColorFile.h"
#include "ContourCellFile.h"
#include "CocomacConnectivityFile.h"
#include "DebugControl.h"
#include "DeformationFieldFile.h"
#include "DisplaySettingsArealEstimation.h"
#include "DisplaySettingsBorders.h"
#include "DisplaySettingsCells.h"
#include "DisplaySettingsCoCoMac.h"
#include "DisplaySettingsContours.h"
#include "DisplaySettingsCuts.h"
#include "DisplaySettingsDeformationField.h"
#include "DisplaySettingsFoci.h"
#include "DisplaySettingsGeodesicDistance.h"
#include "DisplaySettingsImages.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsModels.h"
#include "DisplaySettingsSurface.h"
#include "DisplaySettingsPaint.h"
#include "DisplaySettingsProbabilisticAtlas.h"
#include "DisplaySettingsRgbPaint.h"
#include "DisplaySettingsScene.h"
#include "DisplaySettingsStudyMetaData.h"
#include "DisplaySettingsSurfaceShape.h"
#include "DisplaySettingsSurfaceVectors.h"
#include "DisplaySettingsTopography.h"
#include "DisplaySettingsVolume.h"
#include "DisplaySettingsWustlRegion.h"
#include "FileUtilities.h"
#include "FociColorFile.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "GeodesicDistanceFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiDataFileCommentDialog.h"
#include "GuiDisplayControlDialog.h"
#include "GuiFilesModified.h"
#include "GuiHistogramDisplayDialog.h"
#include "GuiMainWindow.h"
#include "GuiSpecAndSceneFileCreationDialog.h"
#include "GuiStudyMetaDataLinkCreationDialog.h"
#include "GuiToolBar.h"
#include "GuiTransformationMatrixSelectionControl.h"
#include "GuiVolumeResizingDialog.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "ImageFile.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProbabilisticAtlasFile.h"
#include <QDoubleSpinBox>
#include "QtMultipleInputDialog.h"
#include "QtRadioButtonSelectionDialog.h"
#include "QtSaveWidgetAsImagePushButton.h"
#include "QtUtilities.h"
#include "QtTextEditDialog.h"
#include "RgbPaintFile.h"
#include "SceneFile.h"
#include "SectionFile.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "SurfaceVectorFile.h"
#include "SystemUtilities.h"
#include "TopographyFile.h"
#include "TransformationMatrixFile.h"
#include "VtkModelFile.h"
#include "WuQWidgetGroup.h"
#include "WustlRegionFile.h"

#include "global_variables.h"


/**
 * The Constructor
 */
GuiDisplayControlDialog::GuiDisplayControlDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   GuiBrainModelOpenGL::getPointSizeRange(minPointSize, maxPointSize);
   maxPointSize = 100000.0;
   GuiBrainModelOpenGL::getLineWidthRange(minLineSize, maxLineSize);
   
   pageOverlayUnderlaySurface = NULL;
   surfaceModelIndex = -1;
   
   creatingDialog = true;
   dataInfoDialog = NULL;
   
   setSizeGripEnabled(true);
   setWindowTitle("Display Control");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(5);
   
   //
   // Actions for back and forward tool buttons
   //
   QAction* pageBackToolButtonAction = new QAction(this);
   QAction* pageForwardToolButtonAction = new QAction(this);
   
   //
   // Create the combo box for selecting the individual pages
   //
   QLabel* pageSelLabel = new QLabel("Page Selection");
   pageBackToolButton = new QToolButton;
   pageBackToolButton->setToolTip("Go back to the \n"
                                  "previous page.");
   pageBackToolButton->setArrowType(Qt::LeftArrow);
   pageBackToolButton->setDefaultAction(pageBackToolButtonAction);
   QObject::connect(pageBackToolButton, SIGNAL(triggered(QAction*)),
                    this, SLOT(slotPageBackToolButtonPressed(QAction*)));
   pageForwardToolButton = new QToolButton;
   pageForwardToolButton->setToolTip("Go forward to the \n"
                                     "next page as a \n"
                                     "result of using \n"
                                     "the back arrow.");
   pageForwardToolButton->setArrowType(Qt::RightArrow);
   pageForwardToolButton->setDefaultAction(pageForwardToolButtonAction);
   QObject::connect(pageForwardToolButton, SIGNAL(triggered(QAction*)),
                    this, SLOT(slotPageForwardToolButtonPressed(QAction*)));
   pageComboBox = new QComboBox;
   QObject::connect(pageComboBox, SIGNAL(activated(int)),
                    this, SLOT(pageComboBoxSelection(int)));
   QHBoxLayout* pageSelLayout = new QHBoxLayout;
   pageSelLayout->addWidget(pageSelLabel);
   pageSelLayout->addWidget(pageBackToolButton);
   pageSelLayout->addWidget(pageForwardToolButton);
   pageSelLayout->addWidget(pageComboBox);
   pageSelLayout->setStretchFactor(pageSelLabel, 0);
   pageSelLayout->setStretchFactor(pageBackToolButton, 0);
   pageSelLayout->setStretchFactor(pageForwardToolButton, 0);
   pageSelLayout->setStretchFactor(pageComboBox, 100);
   dialogLayout->addLayout(pageSelLayout);
   
   //
   // Create the surface selection page
   //
   surfaceModelGroupBox = new QGroupBox("Surface Coloration Applies To");
   dialogLayout->addWidget(surfaceModelGroupBox);
   QVBoxLayout* surfaceModelGroupLayout = new QVBoxLayout(surfaceModelGroupBox);
   surfaceModelIndexComboBox = new QComboBox;
   surfaceModelGroupLayout->addWidget(surfaceModelIndexComboBox);
   QObject::connect(surfaceModelIndexComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSurfaceModelIndexComboBox(int)));
                    
   //
   // Shape apply left/right named columns
   //
   shapeApplySelectionToLeftAndRightStructuresFlagCheckBox = new QCheckBox("Apply Shape L-to-L, R-to-R Matching to Coord Files");
   shapeApplySelectionToLeftAndRightStructuresFlagCheckBox->setToolTip("If checked and a surface shape column\n"
                                                          "is selected for display and the column\n"
                                                          "name contains left (right), the column\n"
                                                          "is assigned to all left surfaces and\n"
                                                          "if a column with right (left) in its\n"
                                                          "exists, it is applied to right (left)\n"
                                                          "surfaces.");
   QObject::connect(shapeApplySelectionToLeftAndRightStructuresFlagCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readShapeL2LR2R()));
   surfaceModelGroupLayout->addWidget(shapeApplySelectionToLeftAndRightStructuresFlagCheckBox);
   
   //
   // Metric apply left/right named columns
   //
   metricApplySelectionToLeftAndRightStructuresFlagCheckBox = new QCheckBox("Apply Metric L-to-L, R-to-R Matching to Coord Files");
   metricApplySelectionToLeftAndRightStructuresFlagCheckBox->setToolTip("If checked and a metric column\n"
                                                          "is selected for display and the column\n"
                                                          "name contains left (right), the column\n"
                                                          "is assigned to all left surfaces and\n"
                                                          "if a column with right (left) in its\n"
                                                          "exists, it is applied to right (left)\n"
                                                          "surfaces.");
   QObject::connect(metricApplySelectionToLeftAndRightStructuresFlagCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readMetricL2LR2R()));
   surfaceModelGroupLayout->addWidget(metricApplySelectionToLeftAndRightStructuresFlagCheckBox);
   
   //
   // paint apply left/right named columns
   //
   paintApplySelectionToLeftAndRightStructuresFlagCheckBox = new QCheckBox("Apply Paint L-to-L, R-to-R Matching to Coord Files");
   paintApplySelectionToLeftAndRightStructuresFlagCheckBox->setToolTip("If checked and a paint column\n"
                                                          "is selected for display and the column\n"
                                                          "name contains left (right), the column\n"
                                                          "is assigned to all left surfaces and\n"
                                                          "if a column with right (left) in its\n"
                                                          "exists, it is applied to right (left)\n"
                                                          "surfaces.");
   QObject::connect(paintApplySelectionToLeftAndRightStructuresFlagCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readPaintL2LR2R()));
   surfaceModelGroupLayout->addWidget(paintApplySelectionToLeftAndRightStructuresFlagCheckBox);
   
   //
   // RGB Paint apply left/right named columns
   //
   rgbApplySelectionToLeftAndRightStructuresFlagCheckBox = new QCheckBox("Apply RGB Paint L-to-L, R-to-R Matching to Coord Files");
   rgbApplySelectionToLeftAndRightStructuresFlagCheckBox->setToolTip("If checked and an RGB paint column\n"
                                                          "is selected for display and the column\n"
                                                          "name contains left (right), the column\n"
                                                          "is assigned to all left surfaces and\n"
                                                          "if a column with right (left) in its\n"
                                                          "exists, it is applied to right (left)\n"
                                                          "surfaces.");
   QObject::connect(rgbApplySelectionToLeftAndRightStructuresFlagCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readRgbPaintL2LR2R()));
   surfaceModelGroupLayout->addWidget(rgbApplySelectionToLeftAndRightStructuresFlagCheckBox);
   
   //
   // prob atlas apply left/right named columns
   //
   probAtlasSurfaceApplySelectionToLeftAndRightStructuresFlagCheckBox = new QCheckBox("Apply Prob Atlas L-to-L, R-to-R Matching to Coord Files");
   probAtlasSurfaceApplySelectionToLeftAndRightStructuresFlagCheckBox->setToolTip("If checked and a probabilistic atlas colum\n"
                                                                                  "name contains left (right), the column\n"
                                                                                  "is assigned to all left surfaces and\n"
                                                                                  "if a column with right (left) in its\n"
                                                                                  "exists, it is applied to right (left)\n"
                                                                                  "surfaces.");
   QObject::connect(probAtlasSurfaceApplySelectionToLeftAndRightStructuresFlagCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readProbAtlasSurfaceL2LR2R()));
   surfaceModelGroupLayout->addWidget(probAtlasSurfaceApplySelectionToLeftAndRightStructuresFlagCheckBox);
                    
   surfaceModelGroupBox->setMaximumHeight(surfaceModelGroupBox->sizeHint().height());

#undef HAVE_SCROLL_VIEW
#define HAVE_SCROLL_VIEW
#ifdef HAVE_SCROLL_VIEW   
   //
   // Scroll widget for widget stack containing all sub pages
   //
   widgetStackScrollArea = new QScrollArea;
   dialogLayout->addWidget(widgetStackScrollArea);
   widgetStackScrollArea->setWidgetResizable(true);
   widgetStackScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   widgetStackScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

   //
   // Widget stack for all sub pages
   //
   pageWidgetStack = new QStackedWidget;
   widgetStackScrollArea->setWidget(pageWidgetStack);
#else  // HAVE_SCROLL_VIEW
   //
   // Widget stack for all sub pages
   //
   pageWidgetStack = new QStackedWidget;
   dialogLayout->addWidget(pageWidgetStack);
#endif // HAVE_SCROLL_VIEW
#undef HAVE_SCROLL_VIEW

   //
   // Volume Pages
   //
   pageVolumeSelection = NULL;
   pageVolumeSettings  = NULL;
   pageVolumeSurfaceOutline = NULL;
   
   pageArealEstimation = NULL;
   arealEstimationSelectionButtonGroup = NULL;
   arealEstimationSelectionGridLayout = NULL;
   
   borderNameButtonGroup  = NULL;
   borderNameGridLayout   = NULL;
   borderColorButtonGroup = NULL;
   borderColorGridLayout  = NULL;
   numValidBorderNames    = 0;
   numValidBorderColors   = 0;
   pageBorderMain = NULL;
   pageBorderColor = NULL;
   pageBorderName = NULL;
   
   cellColorButtonGroup = NULL;
   cellColorGridLayout  = NULL;
   cellClassButtonGroup = NULL;
   cellClassGridLayout  = NULL;
   numValidCellClasses  = 0;
   numValidCellColors   = 0;
   pageCellsMain = NULL;
   pageCellsClass = NULL;
   pageCellsColor = NULL;
   
   pageCocomacDisplay = NULL;
   pageCocomacInformation = NULL;
   
   contourCellColorButtonGroup = NULL;
   contourCellColorGridLayout  = NULL;
   contourCellClassButtonGroup = NULL;
   contourCellClassGridLayout  = NULL;
   numValidContourCellClasses  = 0;
   numValidContourCellColors   = 0;
   pageContourMain = NULL;
   pageContourClass = NULL;
   pageContourColor = NULL;
   
   pageDeformationField = NULL;
   
   fociColorButtonGroup = NULL;
   fociColorGridLayout  = NULL;
   fociClassButtonGroup = NULL;
   fociClassGridLayout  = NULL;
   numValidFociClasses  = 0;
   numValidFociColors   = 0;
   pageFociMain = NULL;
   pageFociClass = NULL;
   pageFociColor = NULL;
   pageFociKeyword = NULL;
   pageFociName = NULL;
   pageFociTable = NULL;
   
   pageGeodesicMain = NULL;
   geodesicSelectionGridLayout = NULL;

   pageImages = NULL;
   
   latLonSelectionGridLayout = NULL;
   pageLatLonMain = NULL;
   
   metricViewButtonGroup = NULL;
   metricThresholdButtonGroup = NULL;
   metricCommentButtonGroup = NULL;
   metricSubPageSelectionsLayout = NULL;
   metricMetaDataButtonGroup = NULL;
   numValidMetrics = 0;
   pageMetricSettings = NULL;
   pageMetricSelection = NULL;
   
   modelSelectionGridWidgetLayout = NULL;
   numValidModels = 0;
   pageModelsMain = NULL;
   pageModelsSettings = NULL;
   
   paintColumnMetaDataButtonGroup = NULL;
   paintColumnSelectionGridLayout = NULL;
   pagePaintColumn = NULL;
   
   pagePaintName = NULL;
   
   numValidProbAtlasSurfaceChannels   = 0;
   probAtlasSurfaceSubPageChannelLayout = NULL;
   probAtlasSurfaceChannelGridLayout = NULL;
   probAtlasSurfaceChannelButtonGroup = NULL;
   numValidProbAtlasSurfaceAreas   = 0;
   probAtlasSurfaceAreasGridLayout = NULL;
   probAtlasSurfaceSubPageAreaLayout = NULL;
   probAtlasSurfaceAreasButtonGroup = NULL;
   pageProbAtlasSurfaceMain = NULL;
   pageProbAtlasSurfaceChannel = NULL;
   pageProbAtlasSurfaceArea = NULL;
   
   numValidProbAtlasVolumeChannels   = 0;
   probAtlasVolumeChannelGridLayout       = NULL;
   probAtlasVolumeChannelButtonGroup = NULL;
   numValidProbAtlasVolumeAreas   = 0;
   probAtlasVolumeAreasGridLayout       = NULL;
   probAtlasVolumeAreasButtonGroup = NULL;
   pageProbAtlasVolumeMain = NULL;
   pageProbAtlasVolumeArea = NULL;
   pageProbAtlasVolumeChannel = NULL;
   
   pageRegionMain = NULL;
   
   pageRgbPaintMain = NULL;
   
   pageSceneMain = NULL;
   skipScenePageUpdate = false;
   
   surfaceShapeViewButtonGroup = NULL;
   surfaceShapeCommentButtonGroup = NULL;
   pageSurfaceShapeSelections = NULL;
   pageSurfaceShapeSettings = NULL;
   surfaceShapeSubSelectionsLayout = NULL;
   surfaceShapeMetaDataButtonGroup = NULL;
   numValidSurfaceShape = 0;
   
   pageSurfaceAndVolume = NULL;
   
   pageSurfaceMisc = NULL;

   pageSurfaceVectorSelection = NULL;
   pageSurfaceVectorSettings = NULL;
   
   pageTopography = NULL;
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(5);
   dialogLayout->addLayout(buttonsLayout);

   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(applySelected()));   

   //
   // Save as image button
   //
   QtSaveWidgetAsImagePushButton* saveAsImageButton =
              new QtSaveWidgetAsImagePushButton("Save As Image...",
                                                0,
                                                this);
   saveAsImageButton->setAutoDefault(false);
   buttonsLayout->addWidget(saveAsImageButton);
   
   //
   // Help button
   //
   QPushButton* helpButton = new QPushButton("Help");
   buttonsLayout->addWidget(helpButton);
   helpButton->setAutoDefault(false);
   QObject::connect(helpButton, SIGNAL(clicked()),
                    this, SLOT(slotHelpButton()));
                    
   //
   // Close button connects to QDialogs close() slot.   
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
  
   QtUtilities::makeButtonsSameSize(applyButton, saveAsImageButton, helpButton, closeButton);
 
   dialogDefaultSize = QDialog::sizeHint();
   if (DebugControl::getDebugOn()) {
      std::cout << "Initial D/C Size Hint: (" << dialogDefaultSize.width()
                << " " << dialogDefaultSize.height() << ")" << std::endl;
   }
   //07/06/2005setMinimumSize(dialogDefaultSize);
   //setMaximumSize(dialogDefaultSize);
   
   updateAllItemsInDialog(true, false);
   
   pagesVisitedIndex = -1;
   
   updatePageSelectionComboBox();
   if (pageComboBoxItems.empty() == false) {
      pageComboBoxSelection(0);
   }
   
   //surfaceModelGroupBox->setFixedWidth(pageOverlayUnderlaySurface->sizeHint().width());
   creatingDialog = false;
   
   //
   // Default to O/U Volume page if there are volumes and no surfaces
   //
   if (validSurfaceData) {
      showDisplayControlPage(PAGE_NAME_SURFACE_OVERLAY_UNDERLAY, false);
   }
   else if (validVolumeData) {
      showDisplayControlPage(PAGE_NAME_VOLUME_SELECTION, false);
   }
   else if (validContourData) {
      showDisplayControlPage(PAGE_NAME_CONTOUR_MAIN, false);
   }
   else {
      showDisplayControlPage(PAGE_NAME_SCENE, false);
   }

/*   
   //
   // Show scene page if no brain models but have scenes
   //
   if (theMainWindow->getBrainSet()->getNumberOfBrainModels() <= 0) {
      SceneFile* sf = theMainWindow->getBrainSet()->getSceneFile();
      if (sf->getNumberOfScenes() > 0) {
         showScenePage();
      }
   }
*/
   
   //07/06/05widgetStackScrollView->setMinimumHeight(pageWidgetStack->sizeHint().height());
   resize(585, 740);
}
   
/** 
 * print the size of the pages.
 */
void 
GuiDisplayControlDialog::printPageSizes()
{
   if (DebugControl::getDebugOn()) {
      std::cout << std::endl;
      std::cout << "Display Control Page Sizes and Size Hints: " << std::endl;
      printPageSizesHelper("Areal Estimation Page", pageArealEstimation);
      //printPageSizesHelper("Border Page", borderPage);
      //printPageSizesHelper("Cell Page", cellPage);
      //printPageSizesHelper("CoCoMac Page", cocomacPage);
      //printPageSizesHelper("Contour Page", contourPage);
      printPageSizesHelper("Deformation Field Page", pageDeformationField);
      //printPageSizesHelper("Foci Page", fociPage);
      printPageSizesHelper("Geodesic Page", pageGeodesicMain);
      printPageSizesHelper("Lat Lon Page", pageLatLonMain);
      //printPageSizesHelper("Metric Page", metricPage);
      printPageSizesHelper("Misc Page", pageSurfaceMisc);
      //printPageSizesHelper("Models Page", modelsPage);
      printPageSizesHelper("O/U Surface Page", pageOverlayUnderlaySurface);
      printPageSizesHelper("O/U Volume Page", pageVolumeSelection);
      printPageSizesHelper("Page Page", pagePaintColumn);
      //printPageSizesHelper("Prob Atlas Surface Page", probAtlasSurfacePage);
      //printPageSizesHelper("Prob Atlas Volume Page", probAtlasVolumePage);
      printPageSizesHelper("Regions File Main Page", pageRegionMain);
      printPageSizesHelper("Rgb Paint Main Page", pageRgbPaintMain);
      //printPageSizesHelper("Shape Main Page", shapeMainPage);
      printPageSizesHelper("Surface and Volume Page", pageSurfaceAndVolume);
      //printPageSizesHelper("Surface Vector Page", surfaceVectorMainPage);
      printPageSizesHelper("Topography Page", pageTopography);
      printPageSizesHelper("Page Widget Stack", pageWidgetStack);
      printPageSizesHelper("Display Control Dialog", this);
      std::cout << std::endl;
   }
}

/**
 * called when help button pressed.
 */
void 
GuiDisplayControlDialog::slotHelpButton()
{
   theMainWindow->showHelpViewerDialog("dialogs/display_control_dialog.html");
}
      

/**
 * show the scene page.
 */
void 
GuiDisplayControlDialog::showScenePage()
{
   showDisplayControlPage(PAGE_NAME_SCENE, true);
}
      
/**
 * override of parent method.
 */
void 
GuiDisplayControlDialog::resizeEvent(QResizeEvent* re)
{
   QDialog::resizeEvent(re);

   if (DebugControl::getDebugOn()) {
      std::cout << "Display Control Dialog Resized: " << std::endl;
      printPageSizesHelper("", this);
      std::cout << std::endl;
   }
}

/**
 * context menu event.
 */
void 
GuiDisplayControlDialog::contextMenuEvent(QContextMenuEvent* /*e*/)
{
   //
   // Popup menu for selection of pages
   //
   QMenu menu(this);
   QObject::connect(&menu, SIGNAL(triggered(QAction*)),
                    this, SLOT(popupMenuSelection(QAction*)));
   for (int i = 0; i < pageComboBox->count(); i++) {
      QAction* action = menu.addAction(pageComboBox->itemText(i));
      action->setData(QVariant(i));
   }
   menu.exec(QCursor::pos());
}      

/**
 * override of sizeHint (limits width of dialog but user can stretch).
 */
/*
QSize 
GuiDisplayControlDialog::sizeHint() const
{
   //
   // Default size was set after dialog created but before data loaded
   //
   QSize sz(500, dialogDefaultSize.height());
   return sz;
}
*/
/**
 * print the sizes of each page when debugging is on.
 */
void 
GuiDisplayControlDialog::printPageSizesHelper(const QString& pageName,
                                        QWidget* thePage)
{
   if (thePage != NULL) {
      QSize sizeHint = thePage->sizeHint();
      QSize theSize  = thePage->size();
      std::cout << "   " << pageName.toAscii().constData()
                << " size " << theSize.width() << ", " << theSize.height()
                << " hint " << sizeHint.width() << ", " << sizeHint.height() << std::endl;
   }
}                          

/**
 * get the name of a page.
 */
QString 
GuiDisplayControlDialog::getPageName(const PAGE_NAME pageName) const
{
   QString s("Unknown");
   
   switch (pageName) {
      case PAGE_NAME_SURFACE_OVERLAY_UNDERLAY:
         s = "Overlay/Underlay - Surface";
         break;
      case PAGE_NAME_VOLUME_SETTINGS:
         s = "Volume Settings";
         break;
      case PAGE_NAME_VOLUME_SURFACE_OUTLINE:
         s = "Volume Surface Outline";
         break;
      case PAGE_NAME_VOLUME_SELECTION:
         s = "Overlay/Underlay - Volume";
         break;
      case PAGE_NAME_AREAL_ESTIMATION:
         s = "Areal Estimation";
         break;
      case PAGE_NAME_BORDER_MAIN:
         s = "Border Main";
         break;
      case PAGE_NAME_BORDER_COLOR:
         s = "Border Color";
         break;
      case PAGE_NAME_BORDER_NAME:
         s = "Border Name";
         break;
      case PAGE_NAME_CELL_MAIN:
         s = "Cell Main";
         break;
      case PAGE_NAME_CELL_CLASS:
         s = "Cell Class";
         break;
      case PAGE_NAME_CELL_COLOR:
         s = "Cell Color";
         break;
      case PAGE_NAME_COCOMAC_DISPLAY:
         s = "CoCoMac Display";
         break;
      case PAGE_NAME_COCOMAC_INFORMATION:
         s = "CoCoMac Information";
         break;
      case PAGE_NAME_CONTOUR_MAIN:
         s = "Contours";
         break;
      case PAGE_NAME_CONTOUR_CLASS:
         s = "Contour Cell Class";
         break;
      case PAGE_NAME_CONTOUR_COLOR:
         s = "Contour Cell Color";
         break;
      case PAGE_NAME_DEFORMATION_FIELD:
         s = "Deformation Field";
         break;
      case PAGE_NAME_FOCI_MAIN:
         s = "Foci Main";
         break;
      case PAGE_NAME_FOCI_CLASS:
         s = "Foci Class";
         break;
      case PAGE_NAME_FOCI_COLOR:
         s = "Foci Color";
         break;
      case PAGE_NAME_FOCI_KEYWORD:
         s = "Foci Keyword";
         break;
      case PAGE_NAME_FOCI_NAME:
         s = "Foci Name";
         break;
      case PAGE_NAME_FOCI_TABLE:
         s = "Foci Table";
         break;
      case PAGE_NAME_GEODESIC:
         s = "Geodesic";
         break;
      case PAGE_NAME_IMAGES:
         s = "Images";
         break;
      case PAGE_NAME_LATLON:
         s = "Lat/Lon";
         break;
      case PAGE_NAME_METRIC_SELECTION:
         s = "Metric Selection";
         break;
      case PAGE_NAME_METRIC_SETTINGS:
         s = "Metric Settings";
         break;
      case PAGE_NAME_MODELS_MAIN:
         s = "Models Main";
         break;
      case PAGE_NAME_MODELS_SETTINGS:
         s = "Models Settings";
         break;
      case PAGE_NAME_PAINT_COLUMN:
         s = "Paint Columns";
         break;
      case PAGE_NAME_PAINT_NAMES:
         s = "Paint Names";
         break;
      case PAGE_NAME_PROB_ATLAS_SURFACE_MAIN:
         s = "Probabilistic Atlas - Surface Main";
         break;
      case PAGE_NAME_PROB_ATLAS_SURFACE_AREA:
         s = "Probabilistic Atlas - Surface Areas";
         break;
      case PAGE_NAME_PROB_ATLAS_SURFACE_CHANNEL:
         s = "Probabilistic Atlas - Surface Channels";
         break;
      case PAGE_NAME_PROB_ATLAS_VOLUME_MAIN:
         s = "Probabilistic Atlas - Volume Main";
         break;
      case PAGE_NAME_PROB_ATLAS_VOLUME_AREA:
         s = "Probabilistic Atlas - Volume Area";
         break;
      case PAGE_NAME_PROB_ATLAS_VOLUME_CHANNEL:
         s = "Probabilistic Atlas - Volume Channel";
         break;
      case PAGE_NAME_REGION:
         s = "Region";
         break;
      case PAGE_NAME_RGB_PAINT:
         s = "RGB Paint";
         break;
      case PAGE_NAME_SCENE:
         s = "Scene";
         break;
      case PAGE_NAME_SHAPE_SELECTION:
         s = "Surface Shape Selection";
         break;
      case PAGE_NAME_SHAPE_SETTINGS:
         s = "Surface Shape Settings";
         break;
      case PAGE_NAME_SURFACE_AND_VOLUME:
         s = "Surface and Volume";
         break;
      case PAGE_NAME_SURFACE_MISC:
         s = "Surface Miscellaneous";
         break;
      case PAGE_NAME_SURFACE_VECTOR_SELECTION:
         s = "Surface Vector Selection";
         break;
      case PAGE_NAME_SURFACE_VECTOR_SETTINGS:
         s = "Surface Vector Settings";
         break;
      case PAGE_NAME_TOPOGRAPHY:
         s = "Topography";
         break;
      case PAGE_NAME_INVALID:
         s = "Invalid";
         break;
   }
   
   return s;
}      

/**
 * update the data validity flag.
 */
void 
GuiDisplayControlDialog::updateDataValidityFlags()      
{
   BrainSet* brainSet = theMainWindow->getBrainSet();
   bool haveSurfaceFlag = false;
   for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
      if (brainSet->getBrainModelSurface(i) != NULL) {
         haveSurfaceFlag = true;
         break;
      }
   }
   BrainModelBorderSet* bmbs = brainSet->getBorderSet();
   
   validSurfaceData = haveSurfaceFlag;
   validVolumeData = (brainSet->getBrainModelVolume() != NULL);
   validArealEstimationData = (brainSet->getArealEstimationFile()->getNumberOfColumns() > 0);
   validBorderData = ((bmbs->getNumberOfBorders() > 0) ||
                             (theMainWindow->getBrainSet()->getVolumeBorderFile()->getNumberOfBorders() > 0));
   validCellData = ((theMainWindow->getBrainSet()->getCellProjectionFile()->getNumberOfCellProjections() > 0) ||
                       (theMainWindow->getBrainSet()->getVolumeCellFile()->empty() == false) ||
                       theMainWindow->getBrainSet()->getHaveTransformationDataCellFiles());
   validCocomacData = (brainSet->getCocomacFile()->empty() == false);
   validContourData = (brainSet->getBrainModelContours() != NULL);
   validDeformationFieldData = (brainSet->getDeformationFieldFile()->getNumberOfColumns() > 0);
   validFociData = ((theMainWindow->getBrainSet()->getFociProjectionFile()->getNumberOfCellProjections() > 0)  ||
                       (theMainWindow->getBrainSet()->getVolumeFociFile()->empty() == false) ||
                       theMainWindow->getBrainSet()->getHaveTransformationDataFociFiles());
   validGeodesicData = (brainSet->getGeodesicDistanceFile()->getNumberOfColumns() > 0);
   validImageData = (brainSet->getNumberOfImageFiles() > 0);
   validLatLonData = (brainSet->getLatLonFile()->getNumberOfColumns() > 0);
   validMetricData = (brainSet->getMetricFile()->getNumberOfColumns() > 0);
   validModelData = (brainSet->getNumberOfVtkModelFiles() > 0);
   validPaintData = (brainSet->getPaintFile()->getNumberOfColumns() > 0);
   validProbAtlasSurfaceData = (brainSet->getProbabilisticAtlasSurfaceFile()->getNumberOfColumns() > 0);
   validProbAtlasVolumeData = (brainSet->getNumberOfVolumeProbAtlasFiles() > 0);
   const WustlRegionFile* wrf = theMainWindow->getBrainSet()->getWustlRegionFile();
   validRegionData = ((wrf->getNumberOfTimeCourses() > 0) &&
                      (brainSet->getNumberOfVolumePaintFiles() > 0));
   validRgbPaintData = (brainSet->getRgbPaintFile()->getNumberOfColumns() > 0);
   validSceneData = true;  // always valid
   validShapeData = (brainSet->getSurfaceShapeFile()->getNumberOfColumns() > 0);
   validSurfaceAndVolumeData = (brainSet->getBrainModelSurfaceAndVolume() != NULL);
   validSurfaceVectorData = (brainSet->getSurfaceVectorFile()->getNumberOfColumns() > 0);
   validTopographyData = (brainSet->getTopographyFile()->getNumberOfColumns() > 0);
}

/**
 * Update the page selection combo box based upon enabled pages.
 */
void
GuiDisplayControlDialog::updatePageSelectionComboBox()
{
   updateDataValidityFlags();
   
   PAGE_NAME currentPageName = PAGE_NAME_INVALID;
   if (pageComboBoxItems.empty() == false) {
      const int item = pageComboBox->currentIndex();
      if ((item >= 0) && (item < static_cast<int>(pageComboBoxItems.size()))) {
         currentPageName = pageComboBoxItems[item];
      }
   }
   
   pageComboBox->clear();
   pageComboBoxItems.clear();
   
   if (validSurfaceData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_SURFACE_OVERLAY_UNDERLAY));
      pageComboBoxItems.push_back(PAGE_NAME_SURFACE_OVERLAY_UNDERLAY);
   }
   if (validVolumeData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_VOLUME_SELECTION));
      pageComboBoxItems.push_back(PAGE_NAME_VOLUME_SELECTION);
   }
   if (validArealEstimationData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_AREAL_ESTIMATION));
      pageComboBoxItems.push_back(PAGE_NAME_AREAL_ESTIMATION);
   }
   if (validBorderData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_BORDER_MAIN));
      pageComboBoxItems.push_back(PAGE_NAME_BORDER_MAIN);
   }
   if (validBorderData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_BORDER_COLOR));
      pageComboBoxItems.push_back(PAGE_NAME_BORDER_COLOR);
   }
   if (validBorderData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_BORDER_NAME));
      pageComboBoxItems.push_back(PAGE_NAME_BORDER_NAME);
   }
   if (validCellData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_CELL_MAIN));
      pageComboBoxItems.push_back(PAGE_NAME_CELL_MAIN);
   }
   if (validCellData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_CELL_CLASS));
      pageComboBoxItems.push_back(PAGE_NAME_CELL_CLASS);
   }
   if (validCellData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_CELL_COLOR));
      pageComboBoxItems.push_back(PAGE_NAME_CELL_COLOR);
   }
   if (validCocomacData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_COCOMAC_DISPLAY));
      pageComboBoxItems.push_back(PAGE_NAME_COCOMAC_DISPLAY);
   }
   if (validCocomacData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_COCOMAC_INFORMATION));
      pageComboBoxItems.push_back(PAGE_NAME_COCOMAC_INFORMATION);
   }
   if (validContourData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_CONTOUR_MAIN));
      pageComboBoxItems.push_back(PAGE_NAME_CONTOUR_MAIN);
   }
   if (validContourData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_CONTOUR_CLASS));
      pageComboBoxItems.push_back(PAGE_NAME_CONTOUR_CLASS);
   }
   if (validContourData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_CONTOUR_COLOR));
      pageComboBoxItems.push_back(PAGE_NAME_CONTOUR_COLOR);
   }
   if (validDeformationFieldData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_DEFORMATION_FIELD));
      pageComboBoxItems.push_back(PAGE_NAME_DEFORMATION_FIELD);
   }
   if (validFociData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_FOCI_MAIN));
      pageComboBoxItems.push_back(PAGE_NAME_FOCI_MAIN);
   }
   if (validFociData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_FOCI_CLASS));
      pageComboBoxItems.push_back(PAGE_NAME_FOCI_CLASS);
   }
   if (validFociData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_FOCI_COLOR));
      pageComboBoxItems.push_back(PAGE_NAME_FOCI_COLOR);
   }
   if (validFociData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_FOCI_KEYWORD));
      pageComboBoxItems.push_back(PAGE_NAME_FOCI_KEYWORD);
   }
   if (validFociData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_FOCI_NAME));
      pageComboBoxItems.push_back(PAGE_NAME_FOCI_NAME);
   }
   if (validFociData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_FOCI_TABLE));
      pageComboBoxItems.push_back(PAGE_NAME_FOCI_TABLE);
   }
   if (validGeodesicData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_GEODESIC));
      pageComboBoxItems.push_back(PAGE_NAME_GEODESIC);
   }
   if (validImageData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_IMAGES));
      pageComboBoxItems.push_back(PAGE_NAME_IMAGES);
   }
   if (validLatLonData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_LATLON));
      pageComboBoxItems.push_back(PAGE_NAME_LATLON);
   }
   if (validMetricData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_METRIC_SELECTION));
      pageComboBoxItems.push_back(PAGE_NAME_METRIC_SELECTION);
   }
   if (validMetricData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_METRIC_SETTINGS));
      pageComboBoxItems.push_back(PAGE_NAME_METRIC_SETTINGS);
   }
   if (validModelData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_MODELS_MAIN));
      pageComboBoxItems.push_back(PAGE_NAME_MODELS_MAIN);
   }
   if (validModelData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_MODELS_SETTINGS));
      pageComboBoxItems.push_back(PAGE_NAME_MODELS_SETTINGS);
   }
   if (validPaintData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_PAINT_COLUMN));
      pageComboBoxItems.push_back(PAGE_NAME_PAINT_COLUMN);
      pageComboBox->addItem(getPageName(PAGE_NAME_PAINT_NAMES));
      pageComboBoxItems.push_back(PAGE_NAME_PAINT_NAMES);
   }
   if (validProbAtlasSurfaceData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_PROB_ATLAS_SURFACE_MAIN));
      pageComboBoxItems.push_back(PAGE_NAME_PROB_ATLAS_SURFACE_MAIN);
   }
   if (validProbAtlasSurfaceData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_PROB_ATLAS_SURFACE_AREA));
      pageComboBoxItems.push_back(PAGE_NAME_PROB_ATLAS_SURFACE_AREA);
   }
   if (validProbAtlasSurfaceData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_PROB_ATLAS_SURFACE_CHANNEL));
      pageComboBoxItems.push_back(PAGE_NAME_PROB_ATLAS_SURFACE_CHANNEL);
   }
   if (validProbAtlasVolumeData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_PROB_ATLAS_VOLUME_MAIN));
      pageComboBoxItems.push_back(PAGE_NAME_PROB_ATLAS_VOLUME_MAIN);
   }
   if (validProbAtlasVolumeData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_PROB_ATLAS_VOLUME_AREA));
      pageComboBoxItems.push_back(PAGE_NAME_PROB_ATLAS_VOLUME_AREA);
   }
   if (validProbAtlasVolumeData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_PROB_ATLAS_VOLUME_CHANNEL));
      pageComboBoxItems.push_back(PAGE_NAME_PROB_ATLAS_VOLUME_CHANNEL);
   }
   if (validRegionData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_REGION));
      pageComboBoxItems.push_back(PAGE_NAME_REGION);
   }
   if (validRgbPaintData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_RGB_PAINT));
      pageComboBoxItems.push_back(PAGE_NAME_RGB_PAINT);
   }
   if (validSceneData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_SCENE));
      pageComboBoxItems.push_back(PAGE_NAME_SCENE);
   }
   if (validShapeData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_SHAPE_SELECTION));
      pageComboBoxItems.push_back(PAGE_NAME_SHAPE_SELECTION);
   }
   if (validShapeData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_SHAPE_SETTINGS));
      pageComboBoxItems.push_back(PAGE_NAME_SHAPE_SETTINGS);
   }
   if (validSurfaceAndVolumeData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_SURFACE_AND_VOLUME));
      pageComboBoxItems.push_back(PAGE_NAME_SURFACE_AND_VOLUME);
   }
   if (validSurfaceData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_SURFACE_MISC));
      pageComboBoxItems.push_back(PAGE_NAME_SURFACE_MISC);
   }
   if (validSurfaceVectorData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_SURFACE_VECTOR_SELECTION));
      pageComboBoxItems.push_back(PAGE_NAME_SURFACE_VECTOR_SELECTION);
   }
   if (validSurfaceVectorData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_SURFACE_VECTOR_SETTINGS));
      pageComboBoxItems.push_back(PAGE_NAME_SURFACE_VECTOR_SETTINGS);
   }
   if (validTopographyData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_TOPOGRAPHY));
      pageComboBoxItems.push_back(PAGE_NAME_TOPOGRAPHY);
   }
   if (validVolumeData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_VOLUME_SETTINGS));
      pageComboBoxItems.push_back(PAGE_NAME_VOLUME_SETTINGS);
   }
   if (validVolumeData) {
      pageComboBox->addItem(getPageName(PAGE_NAME_VOLUME_SURFACE_OUTLINE));
      pageComboBoxItems.push_back(PAGE_NAME_VOLUME_SURFACE_OUTLINE);
   }
   
   int defaultItem = 0;
   for (int i = 0; i < static_cast<int>(pageComboBoxItems.size()); i++) {
      if (pageComboBoxItems[i] == currentPageName) {
         defaultItem = i;
         break;
      }
   }
   if (pageComboBoxItems.empty() == false) {
      pageComboBox->blockSignals(true);
      pageComboBox->setCurrentIndex(defaultItem);
      pageComboBox->blockSignals(false);
   }
}

/**
 * called when popup menu selection is made.
 */
void
GuiDisplayControlDialog::popupMenuSelection(QAction* action)
{
   // get integer from action->data
   const int item = action->data().toInt();
   pageComboBoxSelection(item);
}

/**
 * called when page back tool button pressed.
 */
void 
GuiDisplayControlDialog::slotPageBackToolButtonPressed(QAction*)
{
   pagesVisitedIndex--;
   if ((pagesVisitedIndex >= 0) &&
       (pagesVisitedIndex < static_cast<int>(pagesVisited.size()))) {
      showDisplayControlPage(pagesVisited[pagesVisitedIndex], false);
   }
}

/**
 * called when page forward tool button pressed.
 */
void 
GuiDisplayControlDialog::slotPageForwardToolButtonPressed(QAction*)
{
   pagesVisitedIndex++;
   if ((pagesVisitedIndex >= 0) &&
       (pagesVisitedIndex < static_cast<int>(pagesVisited.size()))) {
      showDisplayControlPage(pagesVisited[pagesVisitedIndex], false);
   }
}
      
/**
 * show a display control page.
 */
void 
GuiDisplayControlDialog::showDisplayControlPage(const PAGE_NAME pageName,
                                                const bool updatePagesVisited)
{
   bool enableSurfaceModelIndexComboBox = false;
   
   pageComboBox->blockSignals(true);
   for (int i = 0; i < static_cast<int>(pageComboBoxItems.size()); i++) {
      if (pageComboBoxItems[i] == pageName) {
         pageComboBox->blockSignals(true);
         pageComboBox->setCurrentIndex(i);
         pageComboBox->blockSignals(false);
      }
   }
   pageComboBox->blockSignals(false);
   
   metricApplySelectionToLeftAndRightStructuresFlagCheckBox->setHidden(true);
   shapeApplySelectionToLeftAndRightStructuresFlagCheckBox->setHidden(true);
   paintApplySelectionToLeftAndRightStructuresFlagCheckBox->setHidden(true);
   rgbApplySelectionToLeftAndRightStructuresFlagCheckBox->setHidden(true);
   probAtlasSurfaceApplySelectionToLeftAndRightStructuresFlagCheckBox->setHidden(true);
   
   switch(pageName) {
      case PAGE_NAME_SURFACE_OVERLAY_UNDERLAY:
         if (pageOverlayUnderlaySurface == NULL) {
            createOverlayUnderlaySurfacePage();
            updateAllItemsInDialog(true, false);
         }
         pageWidgetStack->setCurrentWidget(pageOverlayUnderlaySurface);
         enableSurfaceModelIndexComboBox = true;
         break;
      case PAGE_NAME_AREAL_ESTIMATION:
         if (pageArealEstimation == NULL) {
            createArealEstimationPage();
            updateArealEstimationItems();
         }
         pageWidgetStack->setCurrentWidget(pageArealEstimation);
         break;
      case PAGE_NAME_BORDER_MAIN:
         if (pageBorderMain == NULL) {
            createBorderMainPage();
            updateBorderMainPage();
         }
         pageWidgetStack->setCurrentWidget(pageBorderMain);
         break;
      case PAGE_NAME_BORDER_COLOR:
         if (pageBorderColor == NULL) {
            createBorderColorPage();
            updateBorderColorPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageBorderColor);
         break;
      case PAGE_NAME_BORDER_NAME:
         if (pageBorderName == NULL) {
            createBorderNamePage();
            updateBorderNamePage(true);
         }
         pageWidgetStack->setCurrentWidget(pageBorderName);
         break;
      case PAGE_NAME_CELL_MAIN:
         if (pageCellsMain == NULL) {
            createCellMainPage();
            updateCellMainPage();
         }
         pageWidgetStack->setCurrentWidget(pageCellsMain);
         break;
      case PAGE_NAME_CELL_CLASS:
         if (pageCellsClass == NULL) {
            createCellClassPage();
            updateCellClassPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageCellsClass);
         break;
      case PAGE_NAME_CELL_COLOR:
         if (pageCellsColor == NULL) {
            createCellColorPage();
            updateCellColorPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageCellsColor);
         break;
      case PAGE_NAME_COCOMAC_DISPLAY:
         if (pageCocomacDisplay == NULL) {
            createCocomacDisplayPage();
            updateCocomacDisplayPage();
         }
         pageWidgetStack->setCurrentWidget(pageCocomacDisplay);
         break;
      case PAGE_NAME_COCOMAC_INFORMATION:
         if (pageCocomacInformation == NULL) {
            createCocomacFileInformationPage();
            updateCocomacInformationPage();
         }
         pageWidgetStack->setCurrentWidget(pageCocomacInformation);
         break;
      case PAGE_NAME_CONTOUR_MAIN:
         if (pageContourMain == NULL) {
            createContourMainPage();
            updateContourMainPage();
         }
         pageWidgetStack->setCurrentWidget(pageContourMain);
         break;
      case PAGE_NAME_CONTOUR_CLASS:
         if (pageContourClass == NULL) {
            createContourClassPage();
            updateContourClassPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageContourClass);
         break;
      case PAGE_NAME_CONTOUR_COLOR:
         if (pageContourColor == NULL) {
            createContourColorPage();
            updateContourColorPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageContourColor);
         break;
      case PAGE_NAME_DEFORMATION_FIELD:
         if (pageDeformationField == NULL) {
            createDeformationFieldPage();
            updateDeformationFieldPage();
         }
         pageWidgetStack->setCurrentWidget(pageDeformationField);
         break;
      case PAGE_NAME_FOCI_MAIN:
         if (pageFociMain == NULL) {
            createFociMainPage();
            updateFociMainPage();
         }
         pageWidgetStack->setCurrentWidget(pageFociMain);
         break;
      case PAGE_NAME_FOCI_CLASS:
         if (pageFociClass == NULL) {
            createFociClassPage();
            updateFociClassPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageFociClass);
         break;
      case PAGE_NAME_FOCI_COLOR:
         if (pageFociColor == NULL) {
            createFociColorPage();
            updateFociColorPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageFociColor);
         break;
      case PAGE_NAME_FOCI_KEYWORD:
         if (pageFociKeyword == NULL) {
            createFociKeywordPage();
            updateFociKeywordPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageFociKeyword);
         break;
      case PAGE_NAME_FOCI_NAME:
         if (pageFociName == NULL) {
            createFociNamePage();
            updateFociNamePage(true);
         }
         pageWidgetStack->setCurrentWidget(pageFociName);
         break;
      case PAGE_NAME_FOCI_TABLE:
         if (pageFociTable == NULL) {
            createFociTablePage();
            updateFociTablePage(true);
         }
         pageWidgetStack->setCurrentWidget(pageFociTable);
         break;
      case PAGE_NAME_GEODESIC:
         if (pageGeodesicMain == NULL) {
            createGeodesicPage();
            updateGeodesicItems();
         }
         pageWidgetStack->setCurrentWidget(pageGeodesicMain);
         break;
      case PAGE_NAME_IMAGES:
         if (pageImages == NULL) {
            createImagesPage();
            updateImagesItems();
         }
         pageWidgetStack->setCurrentWidget(pageImages);
         break;
      case PAGE_NAME_LATLON:
         if (pageLatLonMain == NULL) {
            createLatLonPage();
            updateLatLonItems();
         }
         pageWidgetStack->setCurrentWidget(pageLatLonMain);
         break;
      case PAGE_NAME_METRIC_SELECTION:
         if (pageMetricSelection == NULL) {
            createMetricSelectionPage();
            updateMetricSelectionPage();
         }
         pageWidgetStack->setCurrentWidget(pageMetricSelection);
         enableSurfaceModelIndexComboBox = true;
         metricApplySelectionToLeftAndRightStructuresFlagCheckBox->setHidden(false);
         break;
      case PAGE_NAME_METRIC_SETTINGS:
         if (pageMetricSettings == NULL) {
            createMetricSettingsPage();
            updateMetricSettingsPage();
         }
         pageWidgetStack->setCurrentWidget(pageMetricSettings);
         break;
      case PAGE_NAME_MODELS_MAIN:
         if (pageModelsMain == NULL) {
            createModelsMainPage();
            updateModelMainPage();
         }
         pageWidgetStack->setCurrentWidget(pageModelsMain);
         break;
      case PAGE_NAME_MODELS_SETTINGS:
         if (pageModelsSettings == NULL) {
            createModelsSettingsPage();
            updateModelSettingsPage();
         }
         pageWidgetStack->setCurrentWidget(pageModelsSettings);
         break;
      case PAGE_NAME_PAINT_COLUMN:
         if (pagePaintColumn == NULL) {
            createPaintColumnPage();
            updatePaintColumnPage();
         }
         pageWidgetStack->setCurrentWidget(pagePaintColumn);
         enableSurfaceModelIndexComboBox = true;
         paintApplySelectionToLeftAndRightStructuresFlagCheckBox->setHidden(false);
         break;
      case PAGE_NAME_PAINT_NAMES:
         if (pagePaintName == NULL) {
            createPaintNamePage();
            updatePaintNamePage();
         }
         pageWidgetStack->setCurrentWidget(pagePaintName);
         break;
      case PAGE_NAME_PROB_ATLAS_SURFACE_MAIN:
         if (pageProbAtlasSurfaceMain == NULL) {
            createProbAtlasSurfaceMainPage();
            updateProbAtlasSurfaceMainPage();
         }
         pageWidgetStack->setCurrentWidget(pageProbAtlasSurfaceMain);
         enableSurfaceModelIndexComboBox = true;
         probAtlasSurfaceApplySelectionToLeftAndRightStructuresFlagCheckBox->setHidden(false);
         break;
      case PAGE_NAME_PROB_ATLAS_SURFACE_AREA:
         if (pageProbAtlasSurfaceArea == NULL) {
            createProbAtlasSurfaceAreaPage();
            updateProbAtlasSurfaceAreaPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageProbAtlasSurfaceArea);
         break;
      case PAGE_NAME_PROB_ATLAS_SURFACE_CHANNEL:
         if (pageProbAtlasSurfaceChannel == NULL) {
            createProbAtlasSurfaceChannelPage();
            updateProbAtlasSurfaceChannelPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageProbAtlasSurfaceChannel);
         break;
      case PAGE_NAME_PROB_ATLAS_VOLUME_MAIN:
         if (pageProbAtlasVolumeMain == NULL) {
            createProbAtlasVolumeMainPage();
            updateProbAtlasVolumeMainPage();
         }
         pageWidgetStack->setCurrentWidget(pageProbAtlasVolumeMain);
         break;
      case PAGE_NAME_PROB_ATLAS_VOLUME_AREA:
         if (pageProbAtlasVolumeArea == NULL) {
            createProbAtlasVolumeAreaPage();
            updateProbAtlasVolumeAreaPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageProbAtlasVolumeArea);
         break;
      case PAGE_NAME_PROB_ATLAS_VOLUME_CHANNEL:
         if (pageProbAtlasVolumeChannel == NULL) {
            createProbAtlasVolumeChannelPage();
            updateProbAtlasVolumeChannelPage(true);
         }
         pageWidgetStack->setCurrentWidget(pageProbAtlasVolumeChannel);
         break;
      case PAGE_NAME_REGION:
         if (pageRegionMain == NULL) {
            createRegionPage();
            updateRegionItems();
         }
         pageWidgetStack->setCurrentWidget(pageRegionMain);
         break;
      case PAGE_NAME_RGB_PAINT:
         if (pageRgbPaintMain == NULL) {
            createRgbPaintPage();
            updateRgbPaintItems();
         }
         pageWidgetStack->setCurrentWidget(pageRgbPaintMain);
         enableSurfaceModelIndexComboBox = true;
         rgbApplySelectionToLeftAndRightStructuresFlagCheckBox->setHidden(false);
         break;
      case PAGE_NAME_SCENE:
         if (pageSceneMain == NULL) {
            createScenePage();
            updateSceneItems();
         }
         sceneListBox->blockSignals(true);
         pageWidgetStack->setCurrentWidget(pageSceneMain);
         sceneListBox->blockSignals(false);
         break;
      case PAGE_NAME_SHAPE_SELECTION:
         if (pageSurfaceShapeSelections == NULL) {
            createShapeSelectionPage();
            updateShapeSelections();
         }
         pageWidgetStack->setCurrentWidget(pageSurfaceShapeSelections);
         enableSurfaceModelIndexComboBox = true;
         shapeApplySelectionToLeftAndRightStructuresFlagCheckBox->setHidden(false);
         break;
      case PAGE_NAME_SHAPE_SETTINGS:
         if (pageSurfaceShapeSettings == NULL) {
            createShapeSettingsPage();
            updateShapeSettings();
         }
         pageWidgetStack->setCurrentWidget(pageSurfaceShapeSettings);
         break;
      case PAGE_NAME_SURFACE_AND_VOLUME:
         if (pageSurfaceAndVolume == NULL) {
            createSurfaceAndVolumePage();
            updateSurfaceAndVolumeItems();
         }
         pageWidgetStack->setCurrentWidget(pageSurfaceAndVolume);
         break;
      case PAGE_NAME_SURFACE_MISC:
         if (pageSurfaceMisc == NULL) {
            createSurfaceMiscPage();
            updateMiscItems();
         }
         pageWidgetStack->setCurrentWidget(pageSurfaceMisc);
         break;
      case PAGE_NAME_SURFACE_VECTOR_SELECTION:
         if (pageSurfaceVectorSelection == NULL) {
            createSurfaceVectorSelectionPage();
            updateSurfaceVectorSelectionPage();
         }
         pageWidgetStack->setCurrentWidget(pageSurfaceVectorSelection);
         break;
      case PAGE_NAME_SURFACE_VECTOR_SETTINGS:
         if (pageSurfaceVectorSettings == NULL) {
            createSurfaceVectorSettingsPage();
            updateSurfaceVectorSettingsPage();
         }
         pageWidgetStack->setCurrentWidget(pageSurfaceVectorSettings);
         break;
      case PAGE_NAME_TOPOGRAPHY:
         if (pageTopography == NULL) {
            createTopographyPage();
            updateTopographyItems();
         }
         pageWidgetStack->setCurrentWidget(pageTopography);
         enableSurfaceModelIndexComboBox = true;
         break;
      case PAGE_NAME_VOLUME_SELECTION:
         if (pageVolumeSelection == NULL) {
            createOverlayUnderlayVolumeSelectionPage();
            updateVolumeSelectionPage();
         }
         pageWidgetStack->setCurrentWidget(pageVolumeSelection);
         break;
      case PAGE_NAME_VOLUME_SETTINGS:
         if (pageVolumeSettings == NULL) {
            createOverlayUnderlayVolumeSettingsPage();
            updateVolumeSettingsPage();
         }
         pageWidgetStack->setCurrentWidget(pageVolumeSettings);
         break;
      case PAGE_NAME_VOLUME_SURFACE_OUTLINE:
         if (pageVolumeSurfaceOutline == NULL) {
            createOverlayUnderlayVolumeSurfaceOutlinePage();
            updateVolumeSurfaceOutlinePage();
         }
         pageWidgetStack->setCurrentWidget(pageVolumeSurfaceOutline);
         break;
      case PAGE_NAME_INVALID:
         break;
   }
   
   //
   // Should pages visited be updated
   //
   if (updatePagesVisited) {
      //
      // Remove any pages "forward" of the current page index
      //
      const int numPages = static_cast<int>(pagesVisited.size());
      if ((pagesVisitedIndex >= 0) &&
          (pagesVisitedIndex < (numPages - 1))) {
         pagesVisited.erase(pagesVisited.begin() + pagesVisitedIndex + 1,
                            pagesVisited.end());
      }
      
      //
      // Add new pages
      //
      pagesVisited.push_back(pageName);
      pagesVisitedIndex = static_cast<int>(pagesVisited.size() - 1);
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "PAGES VISITED TRACKING" << std::endl;
      for (int i = 0; i < static_cast<int>(pagesVisited.size()); i++) {
         std::cout << i
                   << ": " 
                   << getPageName(pagesVisited[i]).toAscii().constData();
         if (pagesVisitedIndex == i) {
            std::cout << " <==== current page visited index";
         }
         std::cout << std::endl;
      }
      std::cout << std::endl;
   }


   surfaceModelGroupBox->setHidden(enableSurfaceModelIndexComboBox == false);
   
   //
   // Scroll to top of page and left
   //
   QScrollBar* vertScrollBar = widgetStackScrollArea->verticalScrollBar();
   vertScrollBar->setValue(vertScrollBar->minimum());
   QScrollBar* horizScrollBar = widgetStackScrollArea->horizontalScrollBar();
   horizScrollBar->setValue(horizScrollBar->minimum());
   
   //
   // Enable/Disable back and forward buttons
   //
   pageBackToolButton->setEnabled(pagesVisitedIndex > 0);
   pageForwardToolButton->setEnabled(pagesVisitedIndex 
                                     < static_cast<int>((pagesVisited.size() - 1)));
}
      
/**
 * Called when a page is selected using the page combo box
 */
void
GuiDisplayControlDialog::pageComboBoxSelection(int itemIn)
{
   if ((itemIn >= 0) &&
       (itemIn < static_cast<int>(pageComboBoxItems.size()))) {
      const int item = pageComboBoxItems[itemIn];
      showDisplayControlPage(static_cast<PAGE_NAME>(item), true);
   }
   else {
      std::cout << "PROGRAM ERROR: Display Control Dialog pageComboBox not properly set up."
                << std::endl;
   }
}

/**
 * create the overlay underlay volume selection page.
 */
void 
GuiDisplayControlDialog::createOverlayUnderlayVolumeSelectionPage()
{
   QWidget* volSelQVBox = new QWidget;
   QVBoxLayout* volSelLayout = new QVBoxLayout(volSelQVBox);
   
   const int PRIMARY_COLUMN   = 0;
   const int SECONDARY_COLUMN = PRIMARY_COLUMN + 1;
   const int UNDERLAY_COLUMN  = SECONDARY_COLUMN + 1;
   const int NAME_COLUMN      = UNDERLAY_COLUMN + 1;
   const int INFO_COLUMN      = NAME_COLUMN + 1;
   const int METADATA_COLUMN  = INFO_COLUMN + 1;
   const int COMBO_COLUMN     = METADATA_COLUMN + 1;
   //const int NUM_COLUMNS      = COMBO_COLUMN + 1;
   
   //
   // Widget and grid layout for volume selections
   //
   QGroupBox* uoGroup = new QGroupBox("Overlay Underlay");
   volSelLayout->addWidget(uoGroup);
   QGridLayout* grid = new QGridLayout(uoGroup);
   int rowNumber = 0;
   
   //
   // column titles
   //
   grid->addWidget(new QLabel("Primary\nOverlay"),  rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter); 
   grid->addWidget(new QLabel("Secondary\nOverlay"),  rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter); 
   grid->addWidget(new QLabel("Underlay"), rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter); 
   grid->addWidget(new QLabel("Coloring"), rowNumber, NAME_COLUMN, Qt::AlignLeft);
   rowNumber++;
   
   //
   // Button groups to keep radio buttons set correctly
   //
   QButtonGroup* volumePrimaryOverlayButtonGroup = new QButtonGroup(this);
   volumePrimaryOverlayButtonGroup->setExclusive(true);
   QObject::connect(volumePrimaryOverlayButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(volumePrimaryOverlaySelection(int)));
                    
   QButtonGroup* volumeSecondaryOverlayButtonGroup = new QButtonGroup(this);
   volumeSecondaryOverlayButtonGroup->setExclusive(true);
   QObject::connect(volumeSecondaryOverlayButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(volumeSecondaryOverlaySelection(int)));
                    
   QButtonGroup* volumeUnderlayButtonGroup = new QButtonGroup(this);
   volumeUnderlayButtonGroup->setExclusive(true);
   QObject::connect(volumeUnderlayButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(volumeUnderlaySelection(int)));
                    
   
   //
   // None selections
   //
   volumePrimaryOverlayNoneButton = new QRadioButton("");
   grid->addWidget(volumePrimaryOverlayNoneButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   volumePrimaryOverlayButtonGroup->addButton(volumePrimaryOverlayNoneButton, BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE);

   volumeSecondaryOverlayNoneButton = new QRadioButton("");
   grid->addWidget(volumeSecondaryOverlayNoneButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   volumeSecondaryOverlayButtonGroup->addButton(volumeSecondaryOverlayNoneButton, BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE);

   volumeUnderlayNoneButton = new QRadioButton("");
   grid->addWidget(volumeUnderlayNoneButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   volumeUnderlayButtonGroup->addButton(volumeUnderlayNoneButton, BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE);
   grid->addWidget(new QLabel("No Coloring"), rowNumber, NAME_COLUMN);
   grid->addWidget(new QLabel(" "),    rowNumber, COMBO_COLUMN); 
   rowNumber++;
   
   //
   // This QString is added to each combo box prior to setting its fixed size so 
   // that it will display the number of characters in comboSize.  Changing the
   // number of characters in "comboSize" will change the sizes of the combo boxes
   // for the different data files.
   //
//#ifdef Q_OS_WIN32
//   const QString comboSize("                     ");
//#else
//   const QString comboSize("               ");
//#endif
   const int columnWidth = 500;
   
   //
   // Anatomy Selections
   //
   volumePrimaryOverlayAnatomyRadioButton = new QRadioButton;
   grid->addWidget(volumePrimaryOverlayAnatomyRadioButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   volumePrimaryOverlayButtonGroup->addButton(volumePrimaryOverlayAnatomyRadioButton, 
                                     BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY);

   volumeSecondaryOverlayAnatomyRadioButton = new QRadioButton;
   grid->addWidget(volumeSecondaryOverlayAnatomyRadioButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   volumeSecondaryOverlayButtonGroup->addButton(volumeSecondaryOverlayAnatomyRadioButton, 
                                       BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY);

   volumeUnderlayAnatomyRadioButton = new QRadioButton;
   grid->addWidget(volumeUnderlayAnatomyRadioButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   volumeUnderlayButtonGroup->addButton(volumeUnderlayAnatomyRadioButton, 
                               BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY);
   
   volumeAnatomyLabel = new QLabel("Anatomy");
   grid->addWidget(volumeAnatomyLabel, rowNumber, NAME_COLUMN); 
   
   volumeAnatomyInfoPushButton = new QPushButton("?");
   QSize infoButtonSize = volumeAnatomyInfoPushButton->sizeHint();
   infoButtonSize.setWidth(40);
   volumeAnatomyInfoPushButton->setAutoDefault(false);
   volumeAnatomyInfoPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeAnatomyInfoPushButton, rowNumber, INFO_COLUMN);
   volumeAnatomyInfoPushButton->setToolTip( "Press for Info About Anatomy Volume");
   QObject::connect(volumeAnatomyInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeAnatomyInfoPushButtonSelection()));
                    
   volumeAnatomyMetaDataPushButton = new QPushButton("M");
   volumeAnatomyMetaDataPushButton->setAutoDefault(false);
   volumeAnatomyMetaDataPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeAnatomyMetaDataPushButton, rowNumber, METADATA_COLUMN);
   volumeAnatomyMetaDataPushButton->setToolTip( "Press to Edit Anatomy Volume Metadata.");
   QObject::connect(volumeAnatomyMetaDataPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeAnatomyMetaDataPushButtonSelection()));
                    
   volumeAnatomyComboBox = new QComboBox;
   //volumeAnatomyComboBox->insertItem(comboSize);
   //volumeAnatomyComboBox->setFixedSize(volumeAnatomyComboBox->sizeHint());
   volumeAnatomyComboBox->setMinimumWidth(columnWidth);
   grid->addWidget(volumeAnatomyComboBox, rowNumber, COMBO_COLUMN);
   volumeAnatomyComboBox->setToolTip( "Choose Anatomy File");
   QObject::connect(volumeAnatomyComboBox, SIGNAL(activated(int)),
                    this, SLOT(volumeAnatomySelection(int)));
   rowNumber++;
   
   //
   // Functional Selections
   //
   volumePrimaryOverlayFunctionalRadioButton = new QRadioButton;
   grid->addWidget(volumePrimaryOverlayFunctionalRadioButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   volumePrimaryOverlayButtonGroup->addButton(volumePrimaryOverlayFunctionalRadioButton, 
                                     BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL);

   volumeSecondaryOverlayFunctionalRadioButton = new QRadioButton;
   grid->addWidget(volumeSecondaryOverlayFunctionalRadioButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   volumeSecondaryOverlayButtonGroup->addButton(volumeSecondaryOverlayFunctionalRadioButton, 
                                       BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL);

   volumeUnderlayFunctionalRadioButton = new QRadioButton;
   grid->addWidget(volumeUnderlayFunctionalRadioButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   volumeUnderlayButtonGroup->addButton(volumeUnderlayFunctionalRadioButton, 
                               BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL);
   
   volumeFunctionalViewLabel = new QLabel("Functional-View");
   grid->addWidget(volumeFunctionalViewLabel, rowNumber, NAME_COLUMN); 
   
   volumeFunctionalViewInfoPushButton = new QPushButton("?");
   volumeFunctionalViewInfoPushButton->setAutoDefault(false);
   volumeFunctionalViewInfoPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeFunctionalViewInfoPushButton, rowNumber, INFO_COLUMN);
   volumeFunctionalViewInfoPushButton->setToolTip( "Press for Info About Functional View Volume");
   QObject::connect(volumeFunctionalViewInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeFunctionalViewInfoPushButtonSelection()));
                    
   volumeFunctionalViewMetaDataPushButton = new QPushButton("M");
   volumeFunctionalViewMetaDataPushButton->setAutoDefault(false);
   volumeFunctionalViewMetaDataPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeFunctionalViewMetaDataPushButton, rowNumber, METADATA_COLUMN);
   volumeFunctionalViewMetaDataPushButton->setToolTip( "Press to Edit Functional-View Volume Metadata.");
   QObject::connect(volumeFunctionalViewMetaDataPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeFunctionalViewMetaDataPushButtonSelection()));
                    
   volumeFunctionalViewComboBox = new QComboBox;
   //volumeFunctionalViewComboBox->insertItem(comboSize);
   //volumeFunctionalViewComboBox->setFixedSize(volumeFunctionalViewComboBox->sizeHint());
   volumeFunctionalViewComboBox->setMinimumWidth(columnWidth);
   grid->addWidget(volumeFunctionalViewComboBox, rowNumber, COMBO_COLUMN);
   volumeFunctionalViewComboBox->setToolTip("Choose Functional Viewing Volume");
   QObject::connect(volumeFunctionalViewComboBox, SIGNAL(activated(int)),
                    this, SLOT(volumeFunctionalViewSelection(int)));
   rowNumber++;
   
   volumeFunctionalThresholdLabel = new QLabel("Functional-Thresh");
   grid->addWidget(volumeFunctionalThresholdLabel, rowNumber, NAME_COLUMN);
   
   volumeFunctionalThreshInfoPushButton = new QPushButton("?");
   volumeFunctionalThreshInfoPushButton->setAutoDefault(false);
   volumeFunctionalThreshInfoPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeFunctionalThreshInfoPushButton, rowNumber, INFO_COLUMN);
   volumeFunctionalThreshInfoPushButton->setToolTip( "Press for Info About Functional Thresh Volume");
   QObject::connect(volumeFunctionalThreshInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeFunctionalThreshInfoPushButtonSelection()));
                    
   volumeFunctionalThreshMetaDataPushButton = new QPushButton("M");
   volumeFunctionalThreshMetaDataPushButton->setAutoDefault(false);
   volumeFunctionalThreshMetaDataPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeFunctionalThreshMetaDataPushButton, rowNumber, METADATA_COLUMN);
   volumeFunctionalThreshMetaDataPushButton->setToolTip( "Press to Edit Functional-Thresh Volume Metadata.");
   QObject::connect(volumeFunctionalThreshMetaDataPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeFunctionalThreshMetaDataPushButtonSelection()));
                    
   volumeFunctionalThresholdComboBox = new QComboBox;
   //volumeFunctionalThresholdComboBox->insertItem(comboSize);
   //volumeFunctionalThresholdComboBox->setFixedSize(volumeFunctionalThresholdComboBox->sizeHint());
   volumeFunctionalThresholdComboBox->setMinimumWidth(columnWidth);
   grid->addWidget(volumeFunctionalThresholdComboBox, rowNumber, COMBO_COLUMN);
   volumeFunctionalThresholdComboBox->setToolTip( "Choose Functional Thresholding Volume");
   QObject::connect(volumeFunctionalThresholdComboBox, SIGNAL(activated(int)),
                    this, SLOT(volumeFunctionalThresholdSelection(int)));
   rowNumber++;
   
   //
   // Paint Selections
   //
   volumePrimaryOverlayPaintRadioButton = new QRadioButton;
   grid->addWidget(volumePrimaryOverlayPaintRadioButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   volumePrimaryOverlayButtonGroup->addButton(volumePrimaryOverlayPaintRadioButton, 
                                     BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT);

   volumeSecondaryOverlayPaintRadioButton = new QRadioButton;
   grid->addWidget(volumeSecondaryOverlayPaintRadioButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   volumeSecondaryOverlayButtonGroup->addButton(volumeSecondaryOverlayPaintRadioButton, 
                                       BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT);

   volumeUnderlayPaintRadioButton = new QRadioButton;
   grid->addWidget(volumeUnderlayPaintRadioButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   volumeUnderlayButtonGroup->addButton(volumeUnderlayPaintRadioButton, 
                               BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT);
   
   volumePaintLabel = new QLabel("Paint");
   grid->addWidget(volumePaintLabel, rowNumber, NAME_COLUMN); 
   
   volumePaintInfoPushButton = new QPushButton("?");
   volumePaintInfoPushButton->setAutoDefault(false);
   volumePaintInfoPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumePaintInfoPushButton, rowNumber, INFO_COLUMN);
   volumePaintInfoPushButton->setToolTip( "Press for Info About Paint Volume");
   QObject::connect(volumePaintInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(volumePaintInfoPushButtonSelection()));
                    
   volumePaintMetaDataPushButton = new QPushButton("M");
   volumePaintMetaDataPushButton->setAutoDefault(false);
   volumePaintMetaDataPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumePaintMetaDataPushButton, rowNumber, METADATA_COLUMN);
   volumePaintMetaDataPushButton->setToolTip( "Press to Edit Paint Volume Metadata.");
   QObject::connect(volumePaintMetaDataPushButton, SIGNAL(clicked()),
                    this, SLOT(volumePaintMetaDataPushButtonSelection()));
                    
   volumePaintComboBox = new QComboBox;
   //volumePaintComboBox->insertItem(comboSize);
   //volumePaintComboBox->setFixedSize(volumePaintComboBox->sizeHint());
   volumePaintComboBox->setMinimumWidth(columnWidth);
   grid->addWidget(volumePaintComboBox, rowNumber, COMBO_COLUMN);
   volumePaintComboBox->setToolTip( "Choose Paint Volume");
   QObject::connect(volumePaintComboBox, SIGNAL(activated(int)),
                    this, SLOT(volumePaintSelection(int)));
   rowNumber++;
   
   //
   // Prob atlas Selections
   //
   volumePrimaryOverlayProbAtlasRadioButton = new QRadioButton;
   grid->addWidget(volumePrimaryOverlayProbAtlasRadioButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   volumePrimaryOverlayButtonGroup->addButton(volumePrimaryOverlayProbAtlasRadioButton, 
                                     BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS);

   volumeSecondaryOverlayProbAtlasRadioButton = new QRadioButton;
   grid->addWidget(volumeSecondaryOverlayProbAtlasRadioButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   volumeSecondaryOverlayButtonGroup->addButton(volumeSecondaryOverlayProbAtlasRadioButton, 
                                       BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS);

   volumeUnderlayProbAtlasRadioButton = new QRadioButton;
   grid->addWidget(volumeUnderlayProbAtlasRadioButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   volumeUnderlayButtonGroup->addButton(volumeUnderlayProbAtlasRadioButton, 
                               BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS);
   
   volumeProbAtlasLabel = new QLabel("Prob Atlas");
   grid->addWidget(volumeProbAtlasLabel, rowNumber, NAME_COLUMN); 
   
   volumeProbAtlasInfoPushButton = new QPushButton("?");
   volumeProbAtlasInfoPushButton->setAutoDefault(false);
   volumeProbAtlasInfoPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeProbAtlasInfoPushButton, rowNumber, INFO_COLUMN);
   volumeProbAtlasInfoPushButton->setToolTip( "Press for Info About Prob Atlas Volume");
   QObject::connect(volumeProbAtlasInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeProbAtlasInfoPushButtonSelection()));
       
   volumeProbAtlasInfoPushButton->hide(); // LOOK !!  BUTTON IS HIDDEN
   
   volumeProbAtlasMetaDataPushButton = new QPushButton("M");
   volumeProbAtlasMetaDataPushButton->setAutoDefault(false);
   volumeProbAtlasMetaDataPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeProbAtlasMetaDataPushButton, rowNumber, METADATA_COLUMN);
   volumeProbAtlasMetaDataPushButton->setToolTip( "Press to Edit Prob Atlas Volume Metadata.");
   QObject::connect(volumeProbAtlasMetaDataPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeProbAtlasVolumeStudyMetaDataPushButton()));
      
   volumeProbAtlasMetaDataPushButton->hide();  // LOOK !!  BUTTON IS HIDDEN
                                               // Uses button on Vol-Prob Atlas Main Page
   rowNumber++;
   
   //
   // RGB Selections
   //
   volumePrimaryOverlayRgbRadioButton = new QRadioButton;
   grid->addWidget(volumePrimaryOverlayRgbRadioButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   volumePrimaryOverlayButtonGroup->addButton(volumePrimaryOverlayRgbRadioButton, 
                                     BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB);

   volumeSecondaryOverlayRgbRadioButton = new QRadioButton;
   grid->addWidget(volumeSecondaryOverlayRgbRadioButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   volumeSecondaryOverlayButtonGroup->addButton(volumeSecondaryOverlayRgbRadioButton, 
                                       BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB);

   volumeUnderlayRgbRadioButton = new QRadioButton;
   grid->addWidget(volumeUnderlayRgbRadioButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   volumeUnderlayButtonGroup->addButton(volumeUnderlayRgbRadioButton, 
                               BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB);
   
   volumeRgbLabel = new QLabel("RGB");
   grid->addWidget(volumeRgbLabel, rowNumber, NAME_COLUMN); 
   
   volumeRgbInfoPushButton = new QPushButton("?");
   volumeRgbInfoPushButton->setAutoDefault(false);
   volumeRgbInfoPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeRgbInfoPushButton, rowNumber, INFO_COLUMN);
   volumeRgbInfoPushButton->setToolTip( "Press for Info About RGB Volume");
   QObject::connect(volumeRgbInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeRgbInfoPushButtonSelection()));
                    
   volumeRgbMetaDataPushButton = new QPushButton("M");
   volumeRgbMetaDataPushButton->setAutoDefault(false);
   volumeRgbMetaDataPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeRgbMetaDataPushButton, rowNumber, METADATA_COLUMN);
   volumeRgbMetaDataPushButton->setToolTip( "Press to Edit RGB Volume Metadata.");
   QObject::connect(volumeRgbMetaDataPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeRgbMetaDataPushButtonSelection()));
                    
   volumeRgbComboBox = new QComboBox;
   //volumeRgbComboBox->insertItem(comboSize);
   //volumeRgbComboBox->setFixedSize(volumeRgbComboBox->sizeHint());
   volumeRgbComboBox->setMinimumWidth(columnWidth);
   grid->addWidget(volumeRgbComboBox, rowNumber, COMBO_COLUMN);
   volumeRgbComboBox->setToolTip( "Choose RGB Volume");
   QObject::connect(volumeRgbComboBox, SIGNAL(activated(int)),
                    this, SLOT(volumeRgbSelection(int)));
   rowNumber++;
   
   //
   // Segmentation Selections
   //
   volumePrimaryOverlaySegmentationRadioButton = new QRadioButton;
   grid->addWidget(volumePrimaryOverlaySegmentationRadioButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   volumePrimaryOverlayButtonGroup->addButton(volumePrimaryOverlaySegmentationRadioButton, 
                                     BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION);

   volumeSecondaryOverlaySegmentationRadioButton = new QRadioButton;
   grid->addWidget(volumeSecondaryOverlaySegmentationRadioButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   volumeSecondaryOverlayButtonGroup->addButton(volumeSecondaryOverlaySegmentationRadioButton, 
                                       BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION);

   volumeUnderlaySegmentationRadioButton = new QRadioButton;
   grid->addWidget(volumeUnderlaySegmentationRadioButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   volumeUnderlayButtonGroup->addButton(volumeUnderlaySegmentationRadioButton, 
                               BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION);
   
   volumeSegmentationLabel = new QLabel("Segmentation");
   grid->addWidget(volumeSegmentationLabel, rowNumber, NAME_COLUMN); 
   
   volumeSegmentationInfoPushButton = new QPushButton("?");
   volumeSegmentationInfoPushButton->setAutoDefault(false);
   volumeSegmentationInfoPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeSegmentationInfoPushButton, rowNumber, INFO_COLUMN);
   volumeSegmentationInfoPushButton->setToolTip( "Press for Info About Segmentation Volume");
   QObject::connect(volumeSegmentationInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeSegmentationInfoPushButtonSelection()));
                    
   volumeSegmentationMetaDataPushButton = new QPushButton("M");
   volumeSegmentationMetaDataPushButton->setAutoDefault(false);
   volumeSegmentationMetaDataPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeSegmentationMetaDataPushButton, rowNumber, METADATA_COLUMN);
   volumeSegmentationMetaDataPushButton->setToolTip( "Press to Edit Segmentation Volume Metadata.");
   QObject::connect(volumeSegmentationMetaDataPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeSegmentationMetaDataPushButtonSelection()));
                    
   volumeSegmentationComboBox = new QComboBox;
   volumeSegmentationComboBox->setMinimumWidth(columnWidth);
   grid->addWidget(volumeSegmentationComboBox, rowNumber, COMBO_COLUMN);
   volumeSegmentationComboBox->setToolTip( "Choose Segmentation Volume");
   QObject::connect(volumeSegmentationComboBox, SIGNAL(activated(int)),
                    this, SLOT(volumeSegmentationSelection(int)));
   rowNumber++;
   
   //
   // Vector Selections
   //
   volumePrimaryOverlayVectorRadioButton = new QRadioButton;
   grid->addWidget(volumePrimaryOverlayVectorRadioButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   volumePrimaryOverlayButtonGroup->addButton(volumePrimaryOverlayVectorRadioButton, 
                                     BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR);

   volumeSecondaryOverlayVectorRadioButton = new QRadioButton;
   grid->addWidget(volumeSecondaryOverlayVectorRadioButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   volumeSecondaryOverlayButtonGroup->addButton(volumeSecondaryOverlayVectorRadioButton, 
                                       BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR);

   volumeUnderlayVectorRadioButton = new QRadioButton;
   grid->addWidget(volumeUnderlayVectorRadioButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   volumeUnderlayButtonGroup->addButton(volumeUnderlayVectorRadioButton, 
                               BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR);
   
   volumeVectorLabel = new QLabel("Vector");
   grid->addWidget(volumeVectorLabel, rowNumber, NAME_COLUMN); 
   
   volumeVectorInfoPushButton = new QPushButton("?");
   volumeVectorInfoPushButton->setAutoDefault(false);
   volumeVectorInfoPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeVectorInfoPushButton, rowNumber, INFO_COLUMN);
   volumeVectorInfoPushButton->setToolTip( "Press for Info About Vector Volume");
   QObject::connect(volumeVectorInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeVectorInfoPushButtonSelection()));
                    
   volumeVectorMetaDataPushButton = new QPushButton("M");
   volumeVectorMetaDataPushButton->setAutoDefault(false);
   volumeVectorMetaDataPushButton->setFixedSize(infoButtonSize);
   grid->addWidget(volumeVectorMetaDataPushButton, rowNumber, METADATA_COLUMN);
   volumeVectorMetaDataPushButton->setToolTip( "Press to Edit Vector Volume Metadata.");
   QObject::connect(volumeVectorMetaDataPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeVectorMetaDataPushButtonSelection()));
                    
   volumeVectorComboBox = new QComboBox;
   volumeVectorComboBox->setMinimumWidth(columnWidth);
   grid->addWidget(volumeVectorComboBox, rowNumber, COMBO_COLUMN);
   volumeVectorComboBox->setToolTip( "Choose Vector Volume");
   QObject::connect(volumeVectorComboBox, SIGNAL(activated(int)),
                    this, SLOT(volumeVectorSelection(int)));
   rowNumber++;

   uoGroup->setFixedSize(uoGroup->sizeHint());

   //
   // Pushes selections to top of page
   //
   volSelLayout->addStretch();
   //volSelQVBox->setStretchFactor(new QLabel(" ", volSelQVBox), 100);
   
   pageVolumeSelection = volSelQVBox;
   pageWidgetStack->addWidget(pageVolumeSelection);
}

/**
 * create the overlay underlay volume setttings page.
 */
void 
GuiDisplayControlDialog::createOverlayUnderlayVolumeSettingsPage()
{   
   //
   // Anatomy draw type
   //
   QHBoxLayout* anatomyDrawBoxLayout = new QHBoxLayout;
   anatomyDrawBoxLayout->addWidget(new QLabel("Draw Type  "));
   volumeAnatomyDrawTypeComboBox = new QComboBox;
   anatomyDrawBoxLayout->addWidget(volumeAnatomyDrawTypeComboBox);
   volumeAnatomyDrawTypeComboBox->setToolTip(
                 "0 to 255: \n"
                 "   Assumes voxels are within 0 to 255.\n"
                 "Min to Max\n"
                 "   Maps smallest voxel to black and\n"
                 "   the largest voxel to white.\n"
                 "2% to 98%\n"
                 "   Computes histogram of voxels.\n"
                 "   Voxels at 2% in histogram are mapped\n"
                 "   to black.  Voxels at 98% in histogram\n"
                 "   are mapped to white.\n");
   volumeAnatomyDrawTypeComboBox->insertItem(DisplaySettingsVolume::ANATOMY_COLORING_TYPE_0_255,
                                             "0 to 255");
   volumeAnatomyDrawTypeComboBox->insertItem(DisplaySettingsVolume::ANATOMY_COLORING_TYPE_MIN_MAX,
                                             "Min to Max");
   volumeAnatomyDrawTypeComboBox->insertItem(DisplaySettingsVolume::ANATOMY_COLORING_TYPE_2_98,
                                             "2% to 98%");
   QObject::connect(volumeAnatomyDrawTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(readVolumeSettings()));
   anatomyDrawBoxLayout->addStretch();
   
   //
   // Brighness/Contrast
   //
   QLabel* brightnessLabel = new QLabel("Brightness ");
   volumeAnatomyBrightnessLabel = new QLabel("         ");
   volumeAnatomyBrightnessLabel->setFixedSize(volumeAnatomyBrightnessLabel->sizeHint());
   volumeAnatomyBrightnessSlider = new QSlider(Qt::Horizontal);
   volumeAnatomyBrightnessSlider->setMinimum(-150);
   volumeAnatomyBrightnessSlider->setMaximum(150);
   volumeAnatomyBrightnessSlider->setPageStep(1);
   volumeAnatomyBrightnessSlider->setValue(1);
   volumeAnatomyBrightnessSlider->setFixedWidth(251);
   QObject::connect(volumeAnatomyBrightnessSlider, SIGNAL(sliderReleased()),
                    this, SLOT(readVolumeSettings()));
   QObject::connect(volumeAnatomyBrightnessSlider, SIGNAL(sliderMoved(int)),
                    volumeAnatomyBrightnessLabel, SLOT(setNum(int)));

   QLabel* contrastLabel = new QLabel("Contrast ");
   volumeAnatomyContrastLabel = new QLabel("          ");
   volumeAnatomyContrastLabel->setFixedSize(volumeAnatomyContrastLabel->sizeHint());
   volumeAnatomyContrastSlider = new QSlider(Qt::Horizontal);
   volumeAnatomyContrastSlider->setMinimum(-99);
   volumeAnatomyContrastSlider->setMaximum(99);
   volumeAnatomyContrastSlider->setPageStep(1);
   volumeAnatomyContrastSlider->setValue(1);
   volumeAnatomyContrastSlider->setFixedWidth(251);
   QObject::connect(volumeAnatomyContrastSlider, SIGNAL(sliderReleased()),
                    this, SLOT(readVolumeSettings()));
   QObject::connect(volumeAnatomyContrastSlider, SIGNAL(sliderMoved(int)),
                    volumeAnatomyContrastLabel, SLOT(setNum(int)));
   QGridLayout* brightContrastGridLayout = new QGridLayout;
   brightContrastGridLayout->addWidget(brightnessLabel, 0, 0);
   brightContrastGridLayout->addWidget(volumeAnatomyBrightnessLabel, 0, 1);
   brightContrastGridLayout->addWidget(volumeAnatomyBrightnessSlider, 0, 2);
   brightContrastGridLayout->addWidget(contrastLabel, 1, 0);
   brightContrastGridLayout->addWidget(volumeAnatomyContrastLabel, 1, 1);
   brightContrastGridLayout->addWidget(volumeAnatomyContrastSlider, 1, 2);
   
   QPushButton* makeDefaultPushButton = new QPushButton("Make Default");
   makeDefaultPushButton->setAutoDefault(false);
   makeDefaultPushButton->setFixedSize(makeDefaultPushButton->sizeHint());
   makeDefaultPushButton->setToolTip( "Set default brightness/contrast\n"
                                        "to current entries.");
   QObject::connect(makeDefaultPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDefaultVolumeContrastBrightness()));
   QPushButton* histoPushButton = new QPushButton("Histogram...");
   histoPushButton->setAutoDefault(false);
   histoPushButton->setFixedSize(histoPushButton->sizeHint());
   QObject::connect(histoPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyVolumeHistogram()));
   QHBoxLayout* anatButtonLayout = new QHBoxLayout;
   anatButtonLayout->addWidget(makeDefaultPushButton);
   anatButtonLayout->addWidget(histoPushButton);
   
   //
   // Anatomy group box
   //
   QGroupBox* anatomyGroupBox = new QGroupBox("Anatomy Volume");
   QVBoxLayout* anatomyGroupLayout = new QVBoxLayout(anatomyGroupBox);
   anatomyGroupLayout->addLayout(anatomyDrawBoxLayout);
   anatomyGroupLayout->addLayout(brightContrastGridLayout);
   anatomyGroupLayout->addLayout(anatButtonLayout);
   anatomyGroupBox->setFixedSize(anatomyGroupBox->sizeHint());
   
   //
   // Display color bar check box
   //
   volumeFunctionalDisplayColorBarCheckBox = new QCheckBox("Display Color Bar");
   QObject::connect(volumeFunctionalDisplayColorBarCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readVolumeSettings()));
                    
   //
   // Functional volume group box
   //
   QGroupBox* functionalGroupBox = new QGroupBox("Functional Volume");
   QVBoxLayout* functionalGroupLayout = new QVBoxLayout(functionalGroupBox);
   functionalGroupLayout->addWidget(volumeFunctionalDisplayColorBarCheckBox);
   functionalGroupBox->setFixedSize(functionalGroupBox->sizeHint());
   
   //
   // Segmenation Volume draw type
   //
   volumeSegmentationDrawTypeComboBox = new QComboBox;
   volumeSegmentationDrawTypeComboBox->insertItem(DisplaySettingsVolume::SEGMENTATION_DRAW_TYPE_BLEND,
                                                  "Translucent");
   volumeSegmentationDrawTypeComboBox->insertItem(DisplaySettingsVolume::SEGMENTATION_DRAW_TYPE_SOLID,
                                                  "Opaque");
   volumeSegmentationDrawTypeComboBox->insertItem(DisplaySettingsVolume::SEGMENTATION_DRAW_TYPE_BOX,
                                                  "Outline");
   volumeSegmentationDrawTypeComboBox->insertItem(DisplaySettingsVolume::SEGMENTATION_DRAW_TYPE_CROSS,
                                                  "Cross");
   QObject::connect(volumeSegmentationDrawTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(readVolumeSettings()));
   volumeSegmentationDrawTypeComboBox->setToolTip( 
                 "Selects drawing method \n"
                 "for segmentation voxels.");
  
   //
   // Segmentation Volume Translucency
   //
   QHBoxLayout* segTransHBoxLayout = new QHBoxLayout;
   segTransHBoxLayout->addWidget(new QLabel("Translucency"));
   volumeSegmentationTranslucencyDoubleSpinBox = new QDoubleSpinBox;
   volumeSegmentationTranslucencyDoubleSpinBox->setMinimum(0.0);
   volumeSegmentationTranslucencyDoubleSpinBox->setMaximum(1.0);
   volumeSegmentationTranslucencyDoubleSpinBox->setSingleStep(0.1);
   volumeSegmentationTranslucencyDoubleSpinBox->setDecimals(2);
   segTransHBoxLayout->addWidget(volumeSegmentationTranslucencyDoubleSpinBox);
   QObject::connect(volumeSegmentationTranslucencyDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readVolumeSettings()));
                    

   //
   // Segmentation group box
   //
   QGroupBox* segmentGroupBox = new QGroupBox("Segmentation Volume");
   QVBoxLayout* segmentGroupLayout = new QVBoxLayout(segmentGroupBox);
   segmentGroupLayout->addWidget(volumeSegmentationDrawTypeComboBox);
   segmentGroupLayout->addLayout(segTransHBoxLayout);
   segmentGroupBox->setFixedSize(segmentGroupBox->sizeHint());
   
   //
   // Overlay opacity
   //
   volumeOverlayOpacityDoubleSpinBox = new QDoubleSpinBox;
   volumeOverlayOpacityDoubleSpinBox->setMinimum(0.0);
   volumeOverlayOpacityDoubleSpinBox->setMaximum(1.0);
   volumeOverlayOpacityDoubleSpinBox->setSingleStep(0.1);
   volumeOverlayOpacityDoubleSpinBox->setDecimals(2);
   volumeOverlayOpacityDoubleSpinBox->setToolTip(
                 "Values less than 1.0 will blend\n"
                 "the overlay and the underlay.");
   QObject::connect(volumeOverlayOpacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readVolumeSettings()));
   QGroupBox* overlayOpacityGroupBox = new QGroupBox("Overlay Opacity");
   QVBoxLayout* overlayOpacityGroupLayout = new QVBoxLayout(overlayOpacityGroupBox);
   overlayOpacityGroupLayout->addWidget(volumeOverlayOpacityDoubleSpinBox);
   overlayOpacityGroupBox->setFixedSize(overlayOpacityGroupBox->sizeHint());
   
   //
   // start button
   //
   QPushButton* animateStartPushButton = new QPushButton("Start");
   animateStartPushButton->setFixedSize(animateStartPushButton->sizeHint());
   animateStartPushButton->setAutoDefault(false);
   QObject::connect(animateStartPushButton, SIGNAL(clicked()),
                    this, SLOT(slotVolumeAnimateStartPushButton()));

   //
   // stop button
   //
   QPushButton* animateStopPushButton = new QPushButton("Stop");
   animateStopPushButton->setFixedSize(animateStopPushButton->sizeHint());
   animateStopPushButton->setAutoDefault(false);
   QObject::connect(animateStopPushButton, SIGNAL(clicked()),
                    this, SLOT(slotVolumeAnimateStopPushButton()));
                   
   //
   // Horizontal box for direction
   //
   QLabel* volumeAnimateDirectionLabel = new QLabel("Direction ");
   volumeAnimateDirectionComboBox = new QComboBox;
   volumeAnimateDirectionComboBox->insertItem(VOLUME_ANIMATE_DIRECTION_INCREMENT,
                                              "Positive");
   volumeAnimateDirectionComboBox->insertItem(VOLUME_ANIMATE_DIRECTION_DECREMENT,
                                              "Negative");
   QHBoxLayout* animateDirectionLayout = new QHBoxLayout;
   animateDirectionLayout->addWidget(volumeAnimateDirectionLabel);
   animateDirectionLayout->addWidget(volumeAnimateDirectionComboBox);
   animateDirectionLayout->addStretch();

   //
   // Animation section
   //
   QGroupBox* animateGroupBox = new QGroupBox("Animation");
   QVBoxLayout* animateGroupLayout = new QVBoxLayout(animateGroupBox);
   animateGroupLayout->addWidget(animateStartPushButton);
   animateGroupLayout->addWidget(animateStopPushButton);
   animateGroupLayout->addLayout(animateDirectionLayout);
   animateGroupBox->setFixedSize(animateGroupBox->sizeHint());
   
   //
   // paint color key button
   //
   QPushButton* paintColorKeyPushButton = new QPushButton("Paint Color Key...");
   paintColorKeyPushButton->setFixedSize(paintColorKeyPushButton->sizeHint());
   paintColorKeyPushButton->setAutoDefault(false);
   QObject::connect(paintColorKeyPushButton, SIGNAL(clicked()),
                    theMainWindow, SLOT(displayVolumePaintColorKey()));

   //
   // prob atlas color key button
   //
   QPushButton* probAtlasColorKeyPushButton = new QPushButton("Prob Atlas Color Key...");
   probAtlasColorKeyPushButton->setFixedSize(probAtlasColorKeyPushButton->sizeHint());
   probAtlasColorKeyPushButton->setAutoDefault(false);
   QObject::connect(probAtlasColorKeyPushButton, SIGNAL(clicked()),
                    theMainWindow, SLOT(displayVolumeProbabilisticAtlasColorKey()));

   //
   // Color Key buttons
   //
   QGroupBox* colorKeyGroupBox = new QGroupBox("Color Keys");
   QVBoxLayout* colorKeyLayout = new QVBoxLayout(colorKeyGroupBox);
   colorKeyLayout->addWidget(paintColorKeyPushButton);
   colorKeyLayout->addWidget(probAtlasColorKeyPushButton);
   colorKeyGroupBox->setFixedSize(colorKeyGroupBox->sizeHint());
   
   //
   // Sparsity spin box
   //
   QLabel* sparsityLabel = new QLabel("Sparsity ");
   vectorVolumeSparsitySpinBox = new QSpinBox;
   vectorVolumeSparsitySpinBox->setMinimum(1);
   vectorVolumeSparsitySpinBox->setMaximum(100000);
   vectorVolumeSparsitySpinBox->setSingleStep(1);
   vectorVolumeSparsitySpinBox->setToolTip(
                 "Draw Vectors for\n"
                 "Every N Voxels");
   QObject::connect(vectorVolumeSparsitySpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(readVolumeSettings()));
                    
   //
   // vector volume group box
   //
   vectorVolumeGroupBox = new QGroupBox("Vector Volume");
   QHBoxLayout* vectorVolumeGroupLayout = new QHBoxLayout(vectorVolumeGroupBox);
   vectorVolumeGroupLayout->addWidget(sparsityLabel);
   vectorVolumeGroupLayout->addWidget(vectorVolumeSparsitySpinBox);
   vectorVolumeGroupBox->setFixedSize(vectorVolumeGroupBox->sizeHint());
   
   //
   // montage rows spin box
   //
   QLabel* montageRowsLabel = new QLabel("Rows");
   volumeMontageRowsSpinBox = new QSpinBox;
   volumeMontageRowsSpinBox->setMinimum(1);
   volumeMontageRowsSpinBox->setMaximum(100);
   volumeMontageRowsSpinBox->setSingleStep(1);
   volumeMontageRowsSpinBox->setToolTip(
                 "Number of rows in\n"
                 "the montage.");
   QObject::connect(volumeMontageRowsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(readVolumeSettings()));
                    
   //
   // columns spin box
   //
   QLabel* montageColumnsLabel = new QLabel("Columns");
   volumeMontageColumnsSpinBox = new QSpinBox;
   volumeMontageColumnsSpinBox->setMinimum(1);
   volumeMontageColumnsSpinBox->setMaximum(100);
   volumeMontageColumnsSpinBox->setSingleStep(1);
   volumeMontageColumnsSpinBox->setToolTip(
                 "Number of columns\n"
                 "in the montage.");
   QObject::connect(volumeMontageColumnsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(readVolumeSettings()));
                    
   //
   // slices spin box
   //
   QLabel* montageIncrementLabel = new QLabel("Increment");
   volumeMontageSliceIncrementSpinBox = new QSpinBox;
   volumeMontageSliceIncrementSpinBox->setMinimum(1);
   volumeMontageSliceIncrementSpinBox->setMaximum(100);
   volumeMontageSliceIncrementSpinBox->setSingleStep(1);
   volumeMontageSliceIncrementSpinBox->setToolTip(
                 "Slices to skip between\n"
                 "each montage image.");
   QObject::connect(volumeMontageSliceIncrementSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(readVolumeSettings()));
   
   //
   // Montage group box
   //
   volumeMontageGroupBox = new QGroupBox("Montage");
   volumeMontageGroupBox->setCheckable(true);
   QObject::connect(volumeMontageGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(readVolumeSettings()));
   QGridLayout* volumeMontageGroupLayout = new QGridLayout(volumeMontageGroupBox);
   volumeMontageGroupLayout->addWidget(montageRowsLabel, 0, 0);
   volumeMontageGroupLayout->addWidget(volumeMontageRowsSpinBox, 0, 1);
   volumeMontageGroupLayout->addWidget(montageColumnsLabel, 1, 0);
   volumeMontageGroupLayout->addWidget(volumeMontageColumnsSpinBox, 1, 1);
   volumeMontageGroupLayout->addWidget(montageIncrementLabel, 2, 0);
   volumeMontageGroupLayout->addWidget(volumeMontageSliceIncrementSpinBox, 2, 1);
   volumeMontageGroupBox->setFixedSize(volumeMontageGroupBox->sizeHint());
   
   //
   // Show crosshairs check box
   //
   volumeShowCrosshairsCheckBox = new QCheckBox("Show Crosshairs");
   QObject::connect(volumeShowCrosshairsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readVolumeSettings()));
   
   //
   // Show crosshair coordinates check box
   //
   volumeShowCrosshairCoordinatesCheckBox = new QCheckBox("Show Crosshair Coordinates");
   QObject::connect(volumeShowCrosshairCoordinatesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readVolumeSettings()));
   
   //
   // crosshairs group box
   //
   QGroupBox* crosshairsGroupBox = new QGroupBox("Crosshairs");
   QVBoxLayout* crosshairsGroupLayout = new QVBoxLayout(crosshairsGroupBox);
   crosshairsGroupLayout->addWidget(volumeShowCrosshairsCheckBox);
   crosshairsGroupLayout->addWidget(volumeShowCrosshairCoordinatesCheckBox);
   crosshairsGroupBox->setFixedSize(crosshairsGroupBox->sizeHint());
   
   //
   // Oblique slice viewing
   //
   QLabel* obliqueViewLabel = new QLabel("View Matrix");
   obliqueVolumeSliceMatrixControl = new GuiTransformationMatrixSelectionControl(
                                            0,
                                            theMainWindow->getBrainSet()->getTransformationMatrixFile(),
                                            true);
   obliqueVolumeSliceMatrixControl->setNoneLabel("Main Window Oblique View Matrix");
   QObject::connect(obliqueVolumeSliceMatrixControl, SIGNAL(activated(int)),
                    this, SLOT(readVolumeSettings()));
   QLabel* obliqueSampleLabel = new QLabel("Slice Sampling");
   obliqueSlicesSamplingSizeDoubleSpinBox = new QDoubleSpinBox;
   obliqueSlicesSamplingSizeDoubleSpinBox->setMinimum(0.01);
   obliqueSlicesSamplingSizeDoubleSpinBox->setMaximum(100.0);
   obliqueSlicesSamplingSizeDoubleSpinBox->setSingleStep(0.1);
   obliqueSlicesSamplingSizeDoubleSpinBox->setDecimals(2);
   QObject::connect(obliqueSlicesSamplingSizeDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readVolumeSettings()));
                    
   // Oblique View Matrix Selection
   //
   QGroupBox* obliqueGroupBox = new QGroupBox("Oblique Slices Viewing");
   QGridLayout* obliqueViewGrid = new QGridLayout(obliqueGroupBox);
   obliqueViewGrid->addWidget(obliqueViewLabel, 0, 0, Qt::AlignLeft);
   obliqueViewGrid->addWidget(obliqueVolumeSliceMatrixControl, 0, 1, Qt::AlignLeft);
   obliqueViewGrid->addWidget(obliqueSampleLabel, 1, 0, Qt::AlignLeft);
   obliqueViewGrid->addWidget(obliqueSlicesSamplingSizeDoubleSpinBox, 1, 1, Qt::AlignLeft);
   obliqueGroupBox->setFixedSize(obliqueGroupBox->sizeHint());
   
   //
   // Left column layout
   //
   QVBoxLayout* leftLayout = new QVBoxLayout;
   leftLayout->addWidget(functionalGroupBox);
   leftLayout->addWidget(segmentGroupBox);
   leftLayout->addWidget(colorKeyGroupBox);
   leftLayout->addWidget(animateGroupBox);
   
   //
   // Right column layout
   //
   QVBoxLayout* rightLayout = new QVBoxLayout;
   rightLayout->addWidget(volumeMontageGroupBox);
   rightLayout->addWidget(overlayOpacityGroupBox);
   rightLayout->addWidget(crosshairsGroupBox);
   rightLayout->addWidget(vectorVolumeGroupBox);
   
   //
   // left and right columns layout
   //
   QHBoxLayout* columnsLayout = new QHBoxLayout;
   columnsLayout->addLayout(leftLayout);
   columnsLayout->addLayout(rightLayout);
   columnsLayout->addStretch();
   
   //
   // The volume page and layout
   //
   QWidget* volumeSettingsWidget = new QWidget;
   QVBoxLayout* volumeSettingsLayout = new QVBoxLayout(volumeSettingsWidget);
   volumeSettingsLayout->addWidget(anatomyGroupBox);
   volumeSettingsLayout->addWidget(obliqueGroupBox);
   volumeSettingsLayout->addLayout(columnsLayout);
   volumeSettingsWidget->setFixedHeight(volumeSettingsWidget->sizeHint().height());
   
   pageVolumeSettings = volumeSettingsWidget;
   pageWidgetStack->addWidget(pageVolumeSettings);
   
   pageVolumeSettingsWidgetGroup = new WuQWidgetGroup(this);
   pageVolumeSettingsWidgetGroup->addWidget(volumeAnatomyDrawTypeComboBox);
   pageVolumeSettingsWidgetGroup->addWidget(volumeAnatomyBrightnessSlider);
   pageVolumeSettingsWidgetGroup->addWidget(volumeAnatomyContrastSlider);
   pageVolumeSettingsWidgetGroup->addWidget(volumeFunctionalDisplayColorBarCheckBox);
   pageVolumeSettingsWidgetGroup->addWidget(volumeSegmentationDrawTypeComboBox);
   pageVolumeSettingsWidgetGroup->addWidget(volumeSegmentationTranslucencyDoubleSpinBox);
   pageVolumeSettingsWidgetGroup->addWidget(volumeOverlayOpacityDoubleSpinBox);
   pageVolumeSettingsWidgetGroup->addWidget(vectorVolumeSparsitySpinBox);
   pageVolumeSettingsWidgetGroup->addWidget(volumeMontageRowsSpinBox);
   pageVolumeSettingsWidgetGroup->addWidget(volumeMontageColumnsSpinBox);
   pageVolumeSettingsWidgetGroup->addWidget(volumeMontageSliceIncrementSpinBox);
   pageVolumeSettingsWidgetGroup->addWidget(volumeShowCrosshairsCheckBox);
   pageVolumeSettingsWidgetGroup->addWidget(volumeShowCrosshairCoordinatesCheckBox);
   pageVolumeSettingsWidgetGroup->addWidget(obliqueVolumeSliceMatrixControl);
   pageVolumeSettingsWidgetGroup->addWidget(obliqueSlicesSamplingSizeDoubleSpinBox);
}

/**
 * create the overlay underlay volume surface outline page.
 */
void 
GuiDisplayControlDialog::createOverlayUnderlayVolumeSurfaceOutlinePage()
{
   pageVolumeSurfaceOutlineWidgetGroup = new WuQWidgetGroup(this);
   
   //
   // outline for fiducial surface Color combo box
   //
   for (int i = 0; i < DisplaySettingsVolume::MAXIMUM_OVERLAY_SURFACES; i++) {
      volumeOverlaySurfaceOutlineColorComboBox[i] = new QComboBox;
      volumeOverlaySurfaceOutlineColorComboBox[i]->insertItem(DisplaySettingsVolume::SURFACE_OUTLINE_COLOR_BLACK,
                                                    "Black");
      volumeOverlaySurfaceOutlineColorComboBox[i]->insertItem(DisplaySettingsVolume::SURFACE_OUTLINE_COLOR_BLUE,
                                                    "Blue");
      volumeOverlaySurfaceOutlineColorComboBox[i]->insertItem(DisplaySettingsVolume::SURFACE_OUTLINE_COLOR_GREEN,
                                                    "Green");
      volumeOverlaySurfaceOutlineColorComboBox[i]->insertItem(DisplaySettingsVolume::SURFACE_OUTLINE_COLOR_RED,
                                                    "Red");
      volumeOverlaySurfaceOutlineColorComboBox[i]->insertItem(DisplaySettingsVolume::SURFACE_OUTLINE_COLOR_WHITE,
                                                    "White");
      QObject::connect(volumeOverlaySurfaceOutlineColorComboBox[i], SIGNAL(activated(int)),
                       this, SLOT(readVolumeSurfaceOutline()));
      pageVolumeSurfaceOutlineWidgetGroup->addWidget(volumeOverlaySurfaceOutlineColorComboBox[i]);
      
      //
      // Fiducial Thickness float spin box
      //
      volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i] = new QDoubleSpinBox;
      volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i]->setMinimum(0.0);
      volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i]->setMaximum(1000.0);
      volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i]->setSingleStep(0.1);
      volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i]->setDecimals(2);
      QObject::connect(volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i], SIGNAL(valueChanged(double)),
                       this, SLOT(readVolumeSurfaceOutline()));
      pageVolumeSurfaceOutlineWidgetGroup->addWidget(volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i]);
   
      //
      // Show fiducial surface outline check box
      //
      volumeShowOverlaySurfaceOutlineCheckBox[i] = new QCheckBox("");
      QObject::connect(volumeShowOverlaySurfaceOutlineCheckBox[i], SIGNAL(toggled(bool)),
                       this, SLOT(readVolumeSurfaceOutline()));
      pageVolumeSurfaceOutlineWidgetGroup->addWidget(volumeShowOverlaySurfaceOutlineCheckBox[i]);
                       
      //
      // overlay surface selection
      //
      if (i > 0) {
         volumeOverlaySurfaceSelectionComboBox[i] = 
            new GuiBrainModelSelectionComboBox(false,
                                               true,
                                               false,
                                               "");
         QObject::connect(volumeOverlaySurfaceSelectionComboBox[i], SIGNAL(activated(int)),
                          this, SLOT(readVolumeSurfaceOutline()));
         pageVolumeSurfaceOutlineWidgetGroup->addWidget(volumeOverlaySurfaceSelectionComboBox[i]);
      }
      else {
         volumeOverlaySurfaceSelectionComboBox[i] = NULL;
      }
   }
   
   //
   // Surface outline section
   //
   QGroupBox* volumeShowSurfaceOutlineGroupBox = new QGroupBox("Surface Outline");
   QObject::connect(volumeShowSurfaceOutlineGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(readVolumeSurfaceOutline()));
   QGridLayout* volumeShowSurfaceOutlineGroupLayout = new QGridLayout(volumeShowSurfaceOutlineGroupBox);
   volumeShowSurfaceOutlineGroupLayout->addWidget(new QLabel("Show"), 0, 0);
   volumeShowSurfaceOutlineGroupLayout->addWidget(new QLabel("Color"), 0, 1);
   volumeShowSurfaceOutlineGroupLayout->addWidget(new QLabel("Thickness"), 0, 2); 
   volumeShowSurfaceOutlineGroupLayout->addWidget(new QLabel("Surface"), 0, 3);
   for (int i = 0; i < DisplaySettingsVolume::MAXIMUM_OVERLAY_SURFACES; i++) {
      volumeShowSurfaceOutlineGroupLayout->addWidget(volumeShowOverlaySurfaceOutlineCheckBox[i], i+1, 0);  
      volumeShowSurfaceOutlineGroupLayout->addWidget(volumeOverlaySurfaceOutlineColorComboBox[i], i+1, 1);
      volumeShowSurfaceOutlineGroupLayout->addWidget(volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i], i+1, 2);
      if (i > 0) {
         volumeShowSurfaceOutlineGroupLayout->addWidget(volumeOverlaySurfaceSelectionComboBox[i]);
      }
      else {
         volumeShowSurfaceOutlineGroupLayout->addWidget(new QLabel("Active Fiducial Surface"));
      }
      
       //
      // Connect some signals
      //   
      //QObject::connect(volumeShowOverlaySurfaceOutlineCheckBox[i], SIGNAL(toggled(bool)),
      //                 volumeOverlaySurfaceOutlineColorComboBox[i], SLOT(setEnabled(bool)));
      //QObject::connect(volumeShowOverlaySurfaceOutlineCheckBox[i], SIGNAL(toggled(bool)),
      //                 volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i], SLOT(setEnabled(bool)));
      if (i > 0) {
         //QObject::connect(volumeShowOverlaySurfaceOutlineCheckBox[i], SIGNAL(toggled(bool)),
         //                 volumeOverlaySurfaceSelectionComboBox[i], SLOT(setEnabled(bool)));
      }
   }
   volumeShowSurfaceOutlineGroupLayout->setColumnStretch(0, 0);
   volumeShowSurfaceOutlineGroupLayout->setColumnStretch(1, 0);
   volumeShowSurfaceOutlineGroupLayout->setColumnStretch(2, 0);
   volumeShowSurfaceOutlineGroupLayout->setColumnStretch(3, 100);
   volumeShowSurfaceOutlineGroupBox->setFixedHeight(volumeShowSurfaceOutlineGroupBox->sizeHint().height());
   
   QWidget* volumeSurfaceOutlineWidget = new QWidget;
   QVBoxLayout* volumeSurfaceOutlineLayout = new QVBoxLayout(volumeSurfaceOutlineWidget);
   volumeSurfaceOutlineLayout->addWidget(volumeShowSurfaceOutlineGroupBox);
   volumeSurfaceOutlineWidget->setFixedHeight(volumeSurfaceOutlineWidget->sizeHint().height());
   
   pageVolumeSurfaceOutline = volumeSurfaceOutlineWidget;
   pageWidgetStack->addWidget(pageVolumeSurfaceOutline);
}
      
/**
 * Called to use current brightness/contrast default for anatomy volume.
 */
void
GuiDisplayControlDialog::slotDefaultVolumeContrastBrightness()
{
   PreferencesFile* pf = BrainSet::getPreferencesFile();
   pf->setAnatomyVolumeBrightness(volumeAnatomyBrightnessSlider->value());
   pf->setAnatomyVolumeContrast(volumeAnatomyContrastSlider->value());
   try {
      if (pf->getFileName().isEmpty() == false) {
         pf->writeFile(pf->getFileName());
      }
   }
   catch (FileException& /*e*/) {
   }
}

/**
 * Called to start the volume animation.
 */
void
GuiDisplayControlDialog::slotVolumeAnimateStartPushButton()
{
   //
   // Make sure there is a brain model volume in the main window
   //
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      QMessageBox::warning(this, "No Volume in Main Window",
                           "There must be a volume slice displayed in the main window");
      return;
   }
   
   //
   // Find number of slices to animate through
   //
   const VolumeFile::VOLUME_AXIS axis = bmv->getSelectedAxis(0);
   int firstSlice = 0;
   int lastSlice = 0;
   int sliceIndex = -1;
   switch(axis) {
      case VolumeFile::VOLUME_AXIS_X:
         sliceIndex = 0;
         break;
      case VolumeFile::VOLUME_AXIS_Y:
         sliceIndex = 1;
         break;
      case VolumeFile::VOLUME_AXIS_Z:
         sliceIndex = 2;
         break;
      case VolumeFile::VOLUME_AXIS_ALL:
      case VolumeFile::VOLUME_AXIS_OBLIQUE:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
      case VolumeFile::VOLUME_AXIS_UNKNOWN:
         QMessageBox::critical(this, "ERROR", "Animation of all or oblique slices not supported.");
         break;
   }
      
   //
   // Get the volume file
   //
   VolumeFile* vf = bmv->getMasterVolumeFile();
   if (vf != NULL) {
      //
      // Enable the animation
      //
      //
      // Get the main windows OpenGL widget
      //
      GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
      
      // Get the volume's dimensions
      //
      int dim[3];
      vf->getDimensions(dim);
         
      //
      // Get the slices that have data
      //
      int extent[6];
      vf->getNonZeroVoxelExtent(extent);
      
      //
      // Get the currently selected slices of the volume
      //
      int slices[3];
      bmv->getSelectedOrthogonalSlices(0, slices);
         
      //
      // First and last slices to show
      //
      firstSlice = extent[sliceIndex*2];
      lastSlice = extent[sliceIndex*2 + 1];
      
      //
      // while animation should be performed
      //   
      continueVolumeAnimation = true;
      while (continueVolumeAnimation) {
         //
         // set slices and update
         //
         if ((slices[sliceIndex] >= firstSlice) &&
             (slices[sliceIndex] <= lastSlice)) {
            bmv->setSelectedOrthogonalSlices(0, slices);
         }
         GuiToolBar::updateAllToolBars(false);
         GuiBrainModelOpenGL::updateAllGL(openGL);
          
         //
         // Get slice increment
         //
         int sliceDelta = 1;
         switch (static_cast<VOLUME_ANIMATE_DIRECTION>(volumeAnimateDirectionComboBox->currentIndex())) {
            case VOLUME_ANIMATE_DIRECTION_INCREMENT:
               sliceDelta = 1;
               break;
            case VOLUME_ANIMATE_DIRECTION_DECREMENT:
               sliceDelta = -1;
               break;
         }
         
         //
         // Update the slice
         //
         slices[sliceIndex] += sliceDelta;

         //
         // Switch directions if needed
         //
         if (slices[sliceIndex] > lastSlice) {
            slices[sliceIndex] = lastSlice;
            volumeAnimateDirectionComboBox->setCurrentIndex(VOLUME_ANIMATE_DIRECTION_DECREMENT);
         }
         else if (slices[sliceIndex] < firstSlice) {
            volumeAnimateDirectionComboBox->setCurrentIndex(VOLUME_ANIMATE_DIRECTION_INCREMENT);
         } 
         
         //
         // Allow events to process
         //
         qApp->processEvents();
      }
   }
}

/**
 * Called to stop the volume animation.
 */
void
GuiDisplayControlDialog::slotVolumeAnimateStopPushButton()
{
   continueVolumeAnimation = false;
}

/**
 * Called to display a histogram of the anatomy volume
 */
void
GuiDisplayControlDialog::slotAnatomyVolumeHistogram()
{
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   if (bmv != NULL) {
      VolumeFile* vf = bmv->getSelectedVolumeAnatomyFile();
      if (vf != NULL) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         const int numVoxels = vf->getTotalNumberOfVoxels();
         std::vector<float> values;
         for (int i = 0; i < numVoxels; i++) {
            values.push_back(vf->getVoxelWithFlatIndex(i));
         } 
         GuiHistogramDisplayDialog* ghd = new GuiHistogramDisplayDialog(theMainWindow, 
                                             FileUtilities::basename(vf->getFileName()),
                                             values,
                                             true,
                                             false);
         ghd->show();
         QApplication::restoreOverrideCursor();
      }
   }
}

/**
 * Reads the volume selections
 */
void
GuiDisplayControlDialog::readVolumeSelections()
{
   if (pageVolumeSelection == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
}

/**
 * reads the volume settings.
 */
void 
GuiDisplayControlDialog::readVolumeSettings()
{
   if (pageVolumeSettings == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();

   dsv->setAnatomyVolumeBrightness(volumeAnatomyBrightnessSlider->value());
   dsv->setAnatomyVolumeContrast(volumeAnatomyContrastSlider->value());
   dsv->setVectorVolumeSparsity(vectorVolumeSparsitySpinBox->value());
   dsv->setMontageViewSelected(volumeMontageGroupBox->isChecked());
   dsv->setMontageViewSettings(volumeMontageRowsSpinBox->value(),
                               volumeMontageColumnsSpinBox->value(),
                               volumeMontageSliceIncrementSpinBox->value());
   dsv->setDisplayCrosshairCoordinates(volumeShowCrosshairCoordinatesCheckBox->isChecked());
   dsv->setDisplayCrosshairs(volumeShowCrosshairsCheckBox->isChecked());
   dsv->setAnatomyVolumeColoringType(
      static_cast<DisplaySettingsVolume::ANATOMY_COLORING_TYPE>(
                         volumeAnatomyDrawTypeComboBox->currentIndex()));
   dsv->setSegmentationDrawType(
      static_cast<DisplaySettingsVolume::SEGMENTATION_DRAW_TYPE>(
         volumeSegmentationDrawTypeComboBox->currentIndex()));
   dsv->setSegmentationTranslucency(volumeSegmentationTranslucencyDoubleSpinBox->value());
   dsv->setOverlayOpacity(volumeOverlayOpacityDoubleSpinBox->value());
   dsv->setDisplayColorBar(volumeFunctionalDisplayColorBarCheckBox->isChecked());
   dsv->setObliqueSlicesTransformationMatrix(obliqueVolumeSliceMatrixControl->getSelectedMatrix());
   dsv->setObliqueSlicesSamplingSize(obliqueSlicesSamplingSizeDoubleSpinBox->value());

   BrainModelVolumeVoxelColoring* voxelColoring = theMainWindow->getBrainSet()->getVoxelColoring();
   voxelColoring->setVolumeAnatomyColoringInvalid();
   GuiBrainModelOpenGL::updateAllGL();   
}

/**
 * reads the volume surface outline.
 */
void 
GuiDisplayControlDialog::readVolumeSurfaceOutline()
{
   if (pageVolumeSurfaceOutline == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();

   for (int i = 0; i < DisplaySettingsVolume::MAXIMUM_OVERLAY_SURFACES; i++) {
      dsv->setDisplayOverlaySurfaceOutline(i, volumeShowOverlaySurfaceOutlineCheckBox[i]->isChecked());
      dsv->setOverlaySurfaceOutlineColor(i,
         static_cast<DisplaySettingsVolume::SURFACE_OUTLINE_COLOR>(
            volumeOverlaySurfaceOutlineColorComboBox[i]->currentIndex()));
      dsv->setOverlaySurfaceOutlineThickness(i, volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i]->value());
      if (i > 0) {
         dsv->setOverlaySurface(i, dynamic_cast<BrainModelSurface*>(volumeOverlaySurfaceSelectionComboBox[i]->getSelectedBrainModel()));
      }
   }
   GuiBrainModelOpenGL::updateAllGL();   
}
      
/**
 * Update the selected volume tooltips.
 */
void
GuiDisplayControlDialog::updateVolumeToolTips()
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   
   if (theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles() > 0) {
      volumeAnatomyComboBox->setToolTip(
                     theMainWindow->getBrainSet()->getVolumeAnatomyFile(
                        dsv->getSelectedAnatomyVolume())->getDescriptiveLabel());
   }
   
   if (theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles() > 0) {
      volumeFunctionalViewComboBox->setToolTip(
                     theMainWindow->getBrainSet()->getVolumeFunctionalFile(
                        dsv->getSelectedFunctionalVolumeView())->getDescriptiveLabel());
      volumeFunctionalThresholdComboBox->setToolTip(
                     theMainWindow->getBrainSet()->getVolumeFunctionalFile(
                        dsv->getSelectedFunctionalVolumeThreshold())->getDescriptiveLabel());
   }
   
   if (theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles() > 0) {
      volumePaintComboBox->setToolTip(
                    theMainWindow->getBrainSet()->getVolumePaintFile(
                        dsv->getSelectedPaintVolume())->getDescriptiveLabel());
   }
   
   if (theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles() > 0) {
      volumeRgbComboBox->setToolTip(
                    theMainWindow->getBrainSet()->getVolumeRgbFile(
                        dsv->getSelectedRgbVolume())->getDescriptiveLabel());
   }
   
   if (theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles() > 0) {
      volumeSegmentationComboBox->setToolTip( 
                    theMainWindow->getBrainSet()->getVolumeSegmentationFile(
                        dsv->getSelectedSegmentationVolume())->getDescriptiveLabel());
   }
}

/**
 * Update the volume items
 */
void
GuiDisplayControlDialog::updateVolumeItems()
{
   updatePageSelectionComboBox();
   
   updateVolumeSelectionPage();
   updateVolumeSettingsPage();
   updateVolumeSurfaceOutlinePage();
}

/**
 * update volume selection page.
 */
void 
GuiDisplayControlDialog::updateVolumeSelectionPage()
{
   if (pageVolumeSelection == NULL) {
      return;
   }

   pageVolumeSelection->setEnabled(validVolumeData); 
   BrainModelVolumeVoxelColoring* vvc = theMainWindow->getBrainSet()->getVoxelColoring();
   DisplaySettingsVolume* dsv         = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   //
   // Set the primary overlay radio buttons
   //
   switch(vvc->getPrimaryOverlay()) {
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY:
         volumePrimaryOverlayAnatomyRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL:
         volumePrimaryOverlayFunctionalRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT: 
         volumePrimaryOverlayPaintRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS: 
         volumePrimaryOverlayProbAtlasRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB:
         volumePrimaryOverlayRgbRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION:
         volumePrimaryOverlaySegmentationRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR:
         volumePrimaryOverlayVectorRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE:   
         volumePrimaryOverlayNoneButton->setChecked(true);
         break;
   }

   //
   // set the secondary overlay radio buttons
   //
   switch(vvc->getSecondaryOverlay()) {
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY:
         volumeSecondaryOverlayAnatomyRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL:
         volumeSecondaryOverlayFunctionalRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT: 
         volumeSecondaryOverlayPaintRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS: 
         volumeSecondaryOverlayProbAtlasRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB:
         volumeSecondaryOverlayRgbRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION:
         volumeSecondaryOverlaySegmentationRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR:
         volumeSecondaryOverlayVectorRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE:   
         volumeSecondaryOverlayNoneButton->setChecked(true);
         break;
   }
   //
   // set the underlay radio buttons
   //
   switch(vvc->getUnderlay()) {
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY:
         volumeUnderlayAnatomyRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL:
         volumeUnderlayFunctionalRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT: 
         volumeUnderlayPaintRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS: 
         volumeUnderlayProbAtlasRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB:
         volumeUnderlayRgbRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION:
         volumeUnderlaySegmentationRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR:
         volumeUnderlayVectorRadioButton->setChecked(true);
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE:   
         volumeUnderlayNoneButton->setChecked(true);
         break;
   }
   
   //
   // update anatomy volume combo box
   //
   volumePrimaryOverlayAnatomyRadioButton->setEnabled(false);
   volumeSecondaryOverlayAnatomyRadioButton->setEnabled(false);
   volumeUnderlayAnatomyRadioButton->setEnabled(false);
   volumeAnatomyComboBox->setEnabled(false);
   volumeAnatomyLabel->setEnabled(false);
   volumeAnatomyInfoPushButton->setEnabled(false);
   volumeAnatomyMetaDataPushButton->setEnabled(false);
   volumeAnatomyComboBox->clear();
   if (theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles() > 0) {
      for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles(); i++) {
         volumeAnatomyComboBox->addItem(
            theMainWindow->getBrainSet()->getVolumeAnatomyFile(i)->getDescriptiveLabel());
      }
      volumeAnatomyComboBox->setCurrentIndex(dsv->getSelectedAnatomyVolume());
      
      volumePrimaryOverlayAnatomyRadioButton->setEnabled(true);
      volumeSecondaryOverlayAnatomyRadioButton->setEnabled(true);
      volumeUnderlayAnatomyRadioButton->setEnabled(true);
      volumeAnatomyComboBox->setEnabled(true);
      volumeAnatomyLabel->setEnabled(true);
      volumeAnatomyInfoPushButton->setEnabled(true);
      volumeAnatomyMetaDataPushButton->setEnabled(true);
   }
   
   //
   // update functional volume combo box
   //
   volumePrimaryOverlayFunctionalRadioButton->setEnabled(false);
   volumeSecondaryOverlayFunctionalRadioButton->setEnabled(false);
   volumeUnderlayFunctionalRadioButton->setEnabled(false);
   volumeFunctionalViewComboBox->setEnabled(false);
   volumeFunctionalThresholdComboBox->setEnabled(false);
   volumeFunctionalViewLabel->setEnabled(false);
   volumeFunctionalViewInfoPushButton->setEnabled(false);
   volumeFunctionalThreshInfoPushButton->setEnabled(false);
   volumeFunctionalViewMetaDataPushButton->setEnabled(false);
   volumeFunctionalThreshMetaDataPushButton->setEnabled(false);
   volumeFunctionalThresholdLabel->setEnabled(false);
   volumeFunctionalViewComboBox->clear();
   volumeFunctionalThresholdComboBox->clear();
   if (theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles() > 0) {
      for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles(); i++) {
         volumeFunctionalViewComboBox->addItem(
            theMainWindow->getBrainSet()->getVolumeFunctionalFile(i)->getDescriptiveLabel());
         volumeFunctionalThresholdComboBox->addItem(
            theMainWindow->getBrainSet()->getVolumeFunctionalFile(i)->getDescriptiveLabel());
      }
      volumeFunctionalViewComboBox->setCurrentIndex(dsv->getSelectedFunctionalVolumeView());
      volumeFunctionalThresholdComboBox->setCurrentIndex(dsv->getSelectedFunctionalVolumeThreshold());
      
      volumePrimaryOverlayFunctionalRadioButton->setEnabled(true);
      volumeSecondaryOverlayFunctionalRadioButton->setEnabled(true);
      volumeUnderlayFunctionalRadioButton->setEnabled(true);
      volumeFunctionalViewComboBox->setEnabled(true);
      volumeFunctionalViewLabel->setEnabled(true);
      volumeFunctionalThresholdComboBox->setEnabled(true);
      volumeFunctionalThresholdLabel->setEnabled(true);
      volumeFunctionalViewInfoPushButton->setEnabled(true);
      volumeFunctionalThreshInfoPushButton->setEnabled(true);
      volumeFunctionalViewMetaDataPushButton->setEnabled(true);
      volumeFunctionalThreshMetaDataPushButton->setEnabled(true);
   }

   //
   // update paint volume combo box
   //
   volumePrimaryOverlayPaintRadioButton->setEnabled(false);
   volumeSecondaryOverlayPaintRadioButton->setEnabled(false);
   volumeUnderlayPaintRadioButton->setEnabled(false);
   volumePaintComboBox->setEnabled(false);
   volumePaintLabel->setEnabled(false);
   volumePaintInfoPushButton->setEnabled(false);
   volumePaintMetaDataPushButton->setEnabled(false);
   volumePaintComboBox->clear();
   if (theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles() > 0) {
      for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles(); i++) {
         volumePaintComboBox->addItem(
            theMainWindow->getBrainSet()->getVolumePaintFile(i)->getDescriptiveLabel());
      }
      volumePaintComboBox->setCurrentIndex(dsv->getSelectedPaintVolume());
      
      volumePrimaryOverlayPaintRadioButton->setEnabled(true);
      volumeSecondaryOverlayPaintRadioButton->setEnabled(true);
      volumeUnderlayPaintRadioButton->setEnabled(true);
      volumePaintComboBox->setEnabled(true);
      volumePaintLabel->setEnabled(true);
      volumePaintInfoPushButton->setEnabled(true);
      volumePaintMetaDataPushButton->setEnabled(true);
   }

   //
   // update prob atlas volume combo box
   //
   volumePrimaryOverlayProbAtlasRadioButton->setEnabled(false);
   volumeSecondaryOverlayProbAtlasRadioButton->setEnabled(false);
   volumeUnderlayProbAtlasRadioButton->setEnabled(false);
   volumeProbAtlasLabel->setEnabled(false);
   volumeProbAtlasInfoPushButton->setEnabled(false);
   volumeProbAtlasMetaDataPushButton->setEnabled(false);
   if (theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles() > 0) {
      volumePrimaryOverlayProbAtlasRadioButton->setEnabled(true);
      volumeSecondaryOverlayProbAtlasRadioButton->setEnabled(true);
      volumeUnderlayProbAtlasRadioButton->setEnabled(true);
      volumeProbAtlasLabel->setEnabled(true);
      volumeProbAtlasInfoPushButton->setEnabled(true);
      volumeProbAtlasMetaDataPushButton->setEnabled(true);
   }

   //
   // update rgb volume combo box
   //
   volumePrimaryOverlayRgbRadioButton->setEnabled(false);
   volumeSecondaryOverlayRgbRadioButton->setEnabled(false);
   volumeUnderlayRgbRadioButton->setEnabled(false);
   volumeRgbComboBox->setEnabled(false);
   volumeRgbLabel->setEnabled(false);
   volumeRgbComboBox->clear();
   volumeRgbInfoPushButton->setEnabled(false);
   volumeRgbMetaDataPushButton->setEnabled(false);
   if (theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles() > 0) {
      for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles(); i++) {
         volumeRgbComboBox->addItem(
            theMainWindow->getBrainSet()->getVolumeRgbFile(i)->getDescriptiveLabel());
      }
      volumeRgbComboBox->setCurrentIndex(dsv->getSelectedRgbVolume());
      
      volumePrimaryOverlayRgbRadioButton->setEnabled(true);
      volumeSecondaryOverlayRgbRadioButton->setEnabled(true);
      volumeUnderlayRgbRadioButton->setEnabled(true);
      volumeRgbComboBox->setEnabled(true);
      volumeRgbLabel->setEnabled(true);
      volumeRgbInfoPushButton->setEnabled(true);
      volumeRgbMetaDataPushButton->setEnabled(true);
   }

   //
   // update segmentation volume combo box
   //
   volumePrimaryOverlaySegmentationRadioButton->setEnabled(false);
   volumeSecondaryOverlaySegmentationRadioButton->setEnabled(false);
   volumeUnderlaySegmentationRadioButton->setEnabled(false);
   volumeSegmentationComboBox->setEnabled(false);
   volumeSegmentationLabel->setEnabled(false);
   volumeSegmentationInfoPushButton->setEnabled(false);
   volumeSegmentationMetaDataPushButton->setEnabled(false);
   volumeSegmentationComboBox->clear();
   if (theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles() > 0) {
      for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles(); i++) {
         volumeSegmentationComboBox->addItem(
            theMainWindow->getBrainSet()->getVolumeSegmentationFile(i)->getDescriptiveLabel());
      }
      volumeSegmentationComboBox->setCurrentIndex(dsv->getSelectedSegmentationVolume());
      
      volumePrimaryOverlaySegmentationRadioButton->setEnabled(true);
      volumeSecondaryOverlaySegmentationRadioButton->setEnabled(true);
      volumeUnderlaySegmentationRadioButton->setEnabled(true);
      volumeSegmentationComboBox->setEnabled(true);
      volumeSegmentationLabel->setEnabled(true);
      volumeSegmentationInfoPushButton->setEnabled(true);
      volumeSegmentationMetaDataPushButton->setEnabled(true);
   }
   
   //
   // update vector volume combo box
   //
   volumePrimaryOverlayVectorRadioButton->setEnabled(false);
   volumeSecondaryOverlayVectorRadioButton->setEnabled(false);
   volumeUnderlayVectorRadioButton->setEnabled(false);
   volumeVectorComboBox->setEnabled(false);
   volumeVectorLabel->setEnabled(false);
   volumeVectorInfoPushButton->setEnabled(false);
   volumeVectorMetaDataPushButton->setEnabled(false);
   volumeVectorComboBox->clear();
   if (theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles() > 0) {
      for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles(); i++) {
         volumeVectorComboBox->addItem(
            theMainWindow->getBrainSet()->getVolumeVectorFile(i)->getDescriptiveLabel());
      }
      volumeVectorComboBox->setCurrentIndex(dsv->getSelectedVectorVolume());
      
      volumePrimaryOverlayVectorRadioButton->setEnabled(true);
      volumeSecondaryOverlayVectorRadioButton->setEnabled(true);
      volumeUnderlayVectorRadioButton->setEnabled(true);
      volumeVectorComboBox->setEnabled(true);
      volumeVectorLabel->setEnabled(true);
      volumeVectorInfoPushButton->setEnabled(true);
      volumeVectorMetaDataPushButton->setEnabled(true);
      vectorVolumeSparsitySpinBox->setValue(dsv->getVectorVolumeSparsity());
   }

   updateVolumeToolTips();
   updateProbAtlasVolumeItems(true);
}

/**
 * update volume settings page.
 */
void 
GuiDisplayControlDialog::updateVolumeSettingsPage()
{
   if (pageVolumeSettings == NULL) {
      return;
   }
   pageVolumeSettingsWidgetGroup->blockSignals(true);
   
   pageVolumeSettings->setEnabled(validVolumeData);
   
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();

   volumeSegmentationDrawTypeComboBox->setCurrentIndex(dsv->getSegmentationDrawType());
   volumeAnatomyDrawTypeComboBox->setCurrentIndex(dsv->getAnatomyVolumeColoringType());
   volumeSegmentationTranslucencyDoubleSpinBox->setValue(dsv->getSegmentationTranslucency());
   volumeOverlayOpacityDoubleSpinBox->setValue(dsv->getOverlayOpacity());
   volumeAnatomyBrightnessSlider->setValue(dsv->getAnatomyVolumeBrightness());
   volumeAnatomyContrastSlider->setValue(dsv->getAnatomyVolumeContrast());
   
   volumeAnatomyBrightnessLabel->setNum(volumeAnatomyBrightnessSlider->value());
   volumeAnatomyContrastLabel->setNum(volumeAnatomyContrastSlider->value());
   
   volumeFunctionalDisplayColorBarCheckBox->setChecked(dsv->getDisplayColorBar());
   
   volumeMontageGroupBox->setChecked(dsv->getMontageViewSelected());
   int rows, cols, slices;
   dsv->getMontageViewSettings(rows, cols, slices);
   volumeMontageRowsSpinBox->setValue(rows);
   volumeMontageColumnsSpinBox->setValue(cols);
   volumeMontageSliceIncrementSpinBox->setValue(slices);
   
   volumeShowCrosshairCoordinatesCheckBox->blockSignals(true);
   volumeShowCrosshairCoordinatesCheckBox->setChecked(dsv->getDisplayCrosshairCoordinates());
   volumeShowCrosshairCoordinatesCheckBox->blockSignals(false);
   volumeShowCrosshairsCheckBox->blockSignals(true);
   volumeShowCrosshairsCheckBox->setChecked(dsv->getDisplayCrosshairs());
   volumeShowCrosshairsCheckBox->blockSignals(false);
   obliqueVolumeSliceMatrixControl->updateControl();
   obliqueVolumeSliceMatrixControl->setSelectedMatrix(dsv->getObliqueSlicesTransformationMatrix());
   obliqueSlicesSamplingSizeDoubleSpinBox->setValue(dsv->getObliqueSlicesSamplingSize());
   continueVolumeAnimation = false;   
   
   vectorVolumeGroupBox->setEnabled(theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles() > 0);
   
   pageVolumeSettingsWidgetGroup->blockSignals(false);
}

/**
 * update volume surface outline page.
 */
void 
GuiDisplayControlDialog::updateVolumeSurfaceOutlinePage()
{
   if (pageVolumeSurfaceOutline == NULL) {
      return;
   }
   pageVolumeSurfaceOutlineWidgetGroup->blockSignals(true);
   pageVolumeSurfaceOutline->setEnabled(validVolumeData);
   
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   for (int i = 0; i < DisplaySettingsVolume::MAXIMUM_OVERLAY_SURFACES; i++) {
      volumeShowOverlaySurfaceOutlineCheckBox[i]->blockSignals(true);
      volumeShowOverlaySurfaceOutlineCheckBox[i]->setChecked(dsv->getDisplayOverlaySurfaceOutline(i));
      volumeShowOverlaySurfaceOutlineCheckBox[i]->blockSignals(false);
      volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i]->blockSignals(true);
      volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i]->setValue(dsv->getOverlaySurfaceOutlineThickness(i));
      volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[i]->blockSignals(false);
      volumeOverlaySurfaceOutlineColorComboBox[i]->blockSignals(true);
      volumeOverlaySurfaceOutlineColorComboBox[i]->setCurrentIndex(dsv->getOverlaySurfaceOutlineColor(i));
      volumeOverlaySurfaceOutlineColorComboBox[i]->blockSignals(false);
      if (i > 0) {
         volumeOverlaySurfaceSelectionComboBox[i]->blockSignals(true);
         volumeOverlaySurfaceSelectionComboBox[i]->updateComboBox();
         volumeOverlaySurfaceSelectionComboBox[i]->setSelectedBrainModel(dsv->getOverlaySurface(i));
         volumeOverlaySurfaceSelectionComboBox[i]->blockSignals(false);
      }
   }
   pageVolumeSurfaceOutlineWidgetGroup->blockSignals(false);
}
      
/**
 * called when a volume primary overlay is selected
 */
void 
GuiDisplayControlDialog::volumePrimaryOverlaySelection(int n)
{
   BrainModelVolumeVoxelColoring* vvc = theMainWindow->getBrainSet()->getVoxelColoring();
   vvc->setPrimaryOverlay(static_cast<BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_TYPE>(n));
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false);
}

/**
 * called when a volume secondary overlay is selected
 */
void 
GuiDisplayControlDialog::volumeSecondaryOverlaySelection(int n)
{
   BrainModelVolumeVoxelColoring* vvc = theMainWindow->getBrainSet()->getVoxelColoring();
   vvc->setSecondaryOverlay(static_cast<BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_TYPE>(n));
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false);
}

/**
 * called when a volume underlay is selected
 */
void 
GuiDisplayControlDialog::volumeUnderlaySelection(int n)
{
   BrainModelVolumeVoxelColoring* vvc = theMainWindow->getBrainSet()->getVoxelColoring();
   vvc->setUnderlay(static_cast<BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_TYPE>(n));
   GuiVolumeResizingDialog* volResizeDialog = theMainWindow->getVolumeResizingDialog(false);
   if (volResizeDialog != NULL) {
      volResizeDialog->updateDialog(false);
   }
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false);
}

/**
 * called to display comment info for volume.
 */
void 
GuiDisplayControlDialog::volumeAnatomyInfoPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeAnatomyFile();
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow, vf);
      dfcd->show();
   }
}

/**
 * called to display comment info for volume.
 */
void 
GuiDisplayControlDialog::volumeFunctionalViewInfoPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeFunctionalViewFile();
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow, vf);
      dfcd->show();
   }
}

/**
 * called to display comment info for volume.
 */
void 
GuiDisplayControlDialog::volumeFunctionalThreshInfoPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeFunctionalThresholdFile();
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow, vf);
      dfcd->show();
   }
}

/**
 * called to display comment info for volume.
 */
void 
GuiDisplayControlDialog::volumePaintInfoPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumePaintFile();
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow, vf);
      dfcd->show();
   }
}

/**
 * called to display comment info for volume.
 */
void 
GuiDisplayControlDialog::volumeProbAtlasInfoPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles() > 0) {
      if (theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles() > 0) {
         VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeProbAtlasFile(0);
         GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow, vf);
         dfcd->show();
      }
   }
}

/** 
 * called to display comment info for volume.
 */
void 
GuiDisplayControlDialog::volumeRgbInfoPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeRgbFile();
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow, vf);
      dfcd->show();
   }
}

/** 
 * called to display comment info for segmentation volume.
 */
void 
GuiDisplayControlDialog::volumeSegmentationInfoPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeSegmentationFile();
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow, vf);
      dfcd->show();
   }
}
      
/** 
 * called to display comment info for vector volume.
 */
void 
GuiDisplayControlDialog::volumeVectorInfoPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeVectorFile();
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow, vf);
      dfcd->show();
   }
}
      
/**
 * called to display metadata info for anatomy volume.
 */
void 
GuiDisplayControlDialog::volumeAnatomyMetaDataPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeAnatomyFile();
      StudyMetaDataLinkSet smdls = vf->getStudyMetaDataLinkSet();
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         vf->setStudyMetaDataLinkSet(smdls);
      }
   }
}

/**
 * called to display metadata info for functional view volume.
 */
void 
GuiDisplayControlDialog::volumeFunctionalViewMetaDataPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeFunctionalViewFile();
      StudyMetaDataLinkSet smdls = vf->getStudyMetaDataLinkSet();
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         vf->setStudyMetaDataLinkSet(smdls);
      }
   }
}

/**
 * called to display metadata info for function threshold volume.
 */
void 
GuiDisplayControlDialog::volumeFunctionalThreshMetaDataPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeFunctionalThresholdFile();
      StudyMetaDataLinkSet smdls = vf->getStudyMetaDataLinkSet();
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         vf->setStudyMetaDataLinkSet(smdls);
      }
   }
}

/**
 * called to display metadata info for paint volume.
 */
void 
GuiDisplayControlDialog::volumePaintMetaDataPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumePaintFile();
      StudyMetaDataLinkSet smdls = vf->getStudyMetaDataLinkSet();
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         vf->setStudyMetaDataLinkSet(smdls);
      }
   }
}

/**
 * called to display metadata info for prob atlas volume.
 */
void 
GuiDisplayControlDialog::volumeProbAtlasVolumeStudyMetaDataPushButton()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles() > 0) {
      const int numFiles = theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles();
      if (numFiles > 0) {
         VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeProbAtlasFile(0);
         StudyMetaDataLinkSet smdls = vf->getStudyMetaDataLinkSet();
         GuiStudyMetaDataLinkCreationDialog smdlcd(this);
         smdlcd.initializeSelectedLinkSet(smdls);
         if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
            smdls = smdlcd.getLinkSetCreated();
            for (int i = 0; i < numFiles; i++) {
               VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeProbAtlasFile(i);
               vf->setStudyMetaDataLinkSet(smdls);
            }
         }
      }
   }
}

/** 
 * called to display metadata info for rgb volume.
 */
void 
GuiDisplayControlDialog::volumeRgbMetaDataPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeRgbFile();
      StudyMetaDataLinkSet smdls = vf->getStudyMetaDataLinkSet();
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         vf->setStudyMetaDataLinkSet(smdls);
      }
   }
}

/** 
 * called to display metadata info for segmentation volume.
 */
void 
GuiDisplayControlDialog::volumeSegmentationMetaDataPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeSegmentationFile();
      StudyMetaDataLinkSet smdls = vf->getStudyMetaDataLinkSet();
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         vf->setStudyMetaDataLinkSet(smdls);
      }
   }
}
      
/** 
 * called to display metadata info for vector volume.
 */
void 
GuiDisplayControlDialog::volumeVectorMetaDataPushButtonSelection()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles() > 0) {
      BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
      VolumeFile* vf = bmv->getSelectedVolumeVectorFile();
      StudyMetaDataLinkSet smdls = vf->getStudyMetaDataLinkSet();
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         vf->setStudyMetaDataLinkSet(smdls);
      }
   }
}
      
/**
 * Called when a volume anatomy column is selected.
 */
void
GuiDisplayControlDialog::volumeAnatomySelection(int n)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setSelectedAnatomyVolume(n);
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false);
   updateVolumeToolTips();
}

/**
 * Called when a volume functional view column is functional selected.
 */
void
GuiDisplayControlDialog::volumeFunctionalViewSelection(int n)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setSelectedFunctionalVolumeView(n);
   dsv->setSelectedFunctionalVolumeThreshold(n);
   volumeFunctionalThresholdComboBox->setCurrentIndex(n);
   BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
   bsnc->assignColors();
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false);
   updateVolumeToolTips();
}

/**
 * Called when a volume functional theshold column is functional selected.
 */
void
GuiDisplayControlDialog::volumeFunctionalThresholdSelection(int n)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setSelectedFunctionalVolumeThreshold(n);
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false);
   updateVolumeToolTips();
}

/**
 * Called when a volume paint column is selected.
 */
void
GuiDisplayControlDialog::volumePaintSelection(int n)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setSelectedPaintVolume(n);
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false);
   updateVolumeToolTips();
}

/**
 * Called when a volume rgb column is selected.
 */
void
GuiDisplayControlDialog::volumeRgbSelection(int n)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setSelectedRgbVolume(n);
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false);
   updateVolumeToolTips();
}

/**
 * Called when a volume segmentation column is selected.
 */
void
GuiDisplayControlDialog::volumeSegmentationSelection(int n)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setSelectedSegmentationVolume(n);
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false);
   updateVolumeToolTips();
}

/**
 * Called when a volume vector column is selected.
 */
void
GuiDisplayControlDialog::volumeVectorSelection(int n)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setSelectedVectorVolume(n);
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false);
   updateVolumeToolTips();
}

/**
 * Create the contour main page
 */
void
GuiDisplayControlDialog::createContourClassPage()
{
   //
   // Vertical Box Layout for all contour items
   //
   pageContourClass = new QWidget;
   contourSubPageClassLayout = new QVBoxLayout(pageContourClass);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* contourClassAllOnButton = new QPushButton("All On");
   contourClassAllOnButton->setAutoDefault(false);
   QObject::connect(contourClassAllOnButton, SIGNAL(clicked()),
                    this, SLOT(contourCellClassAllOn()));
   QPushButton* contourClassAllOffButton = new QPushButton("All Off");
   contourClassAllOffButton->setAutoDefault(false);
   QObject::connect(contourClassAllOffButton, SIGNAL(clicked()),
                    this, SLOT(contourCellClassAllOff()));
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(contourClassAllOnButton);
   allOnOffLayout->addWidget(contourClassAllOffButton);
   allOnOffLayout->addStretch();
   contourSubPageClassLayout->addLayout(allOnOffLayout);
   
   createAndUpdateContourCellClassCheckBoxes();

   pageWidgetStack->addWidget(pageContourClass);
}

/**
 * Create and update contour class checkboxes.  Because the number of contour cells may change,
 * this method may update and change the label on existing checkboxes, add new 
 * checkboxes, and hide existing checkboxes if the number of checkboxes is greater
 * than the number of contour cell classes.
 */
void
GuiDisplayControlDialog::createAndUpdateContourCellClassCheckBoxes()
{
   ContourCellFile* cf = theMainWindow->getBrainSet()->getContourCellFile();
   numValidContourCellClasses = cf->getNumberOfCellClasses();
   
   const int numExistingCheckBoxes = static_cast<int>(contourCellClassCheckBoxes.size());
   
   if (contourCellClassGridLayout == NULL) {
      //
      // Scroll View for contour cell color checkboxes
      //
      QWidget* classWidget = new QWidget; 
      contourCellClassGridLayout = new QGridLayout(classWidget);
      const int rowStretchNumber    = 15000;
      contourCellClassGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, Qt::AlignLeft);
      contourCellClassGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      contourSubPageClassLayout->addWidget(classWidget);
   }
   
   if (contourCellClassButtonGroup == NULL) {
      contourCellClassButtonGroup = new QButtonGroup(this);
      contourCellClassButtonGroup->setExclusive(false);
      QObject::connect(contourCellClassButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readContourClassPage()));
   }
   
   //
   // Update exising checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidContourCellClasses) {
         contourCellClassCheckBoxes[i]->setText(cf->getCellClassNameByIndex(i));
         contourCellClassCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidContourCellClasses; j++) {
      QCheckBox* cb = new QCheckBox(cf->getCellClassNameByIndex(j));
      contourCellClassCheckBoxes.push_back(cb);
      contourCellClassButtonGroup->addButton(cb, j);
      contourCellClassGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
   }
   
   //
   // Hide existing checkboxes that are not needed.
   //
   for (int k = numValidContourCellClasses; k < numExistingCheckBoxes; k++) {
      contourCellClassCheckBoxes[k]->hide();
   }
}

/**
 * This slot is called when the contour class all on button is pressed.
 */
void
GuiDisplayControlDialog::contourCellClassAllOn()
{
   ContourCellFile* cf = theMainWindow->getBrainSet()->getContourCellFile();
   if (cf != NULL) {
      cf->setAllCellClassStatus(true);
   }
   updateContourItems();
   readContourClassPage();
}

/**
 * This slot is called when the contour class all off button is pressed.
 */
void
GuiDisplayControlDialog::contourCellClassAllOff()
{
   ContourCellFile* cf = theMainWindow->getBrainSet()->getContourCellFile();
   if (cf != NULL) {
      cf->setAllCellClassStatus(false);
   }
   updateContourItems();
   readContourClassPage();
}


/**
 * Create the contour main page
 */
void
GuiDisplayControlDialog::createContourColorPage()
{
   //
   // Vertical Box Layout for all contour items
   //
   pageContourColor = new QWidget;
   contourSubPageColorLayout = new QVBoxLayout(pageContourColor);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* contourColorAllOnButton = new QPushButton("All On");
   contourColorAllOnButton->setAutoDefault(false);
   QObject::connect(contourColorAllOnButton, SIGNAL(clicked()),
                    this, SLOT(contourCellColorAllOn()));
   QPushButton* contourColorAllOffButton = new QPushButton("All Off");
   contourColorAllOffButton->setAutoDefault(false);
   QObject::connect(contourColorAllOffButton, SIGNAL(clicked()),
                    this, SLOT(contourCellColorAllOff()));
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(contourColorAllOnButton);
   allOnOffLayout->addWidget(contourColorAllOffButton);
   allOnOffLayout->addStretch();
   contourSubPageColorLayout->addLayout(allOnOffLayout);
   
   createAndUpdateContourCellColorCheckBoxes();

   pageWidgetStack->addWidget(pageContourColor);
}

/**
 * Create and update contour color checkboxes.  Because the number of colors may change,
 * this method may update and change the label on existing checkboxes, add new 
 * checkboxes, and hide existing checkboxes if the number of checkboxes is greater
 * than the number of contour cell colors.
 */
void
GuiDisplayControlDialog::createAndUpdateContourCellColorCheckBoxes()
{
   ContourCellColorFile* contourCellColors = theMainWindow->getBrainSet()->getContourCellColorFile();
   numValidContourCellColors = contourCellColors->getNumberOfColors();
   
   const int numExistingCheckBoxes = static_cast<int>(contourCellColorCheckBoxes.size());
   
   if (contourCellColorGridLayout == NULL) {
      QWidget* colorWidget = new QWidget; 
      contourCellColorGridLayout = new QGridLayout(colorWidget);
      const int rowStretchNumber    = 15000;
      contourCellColorGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, Qt::AlignLeft);
      contourCellColorGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      contourSubPageColorLayout->addWidget(colorWidget);
   }
   
   if (contourCellColorButtonGroup == NULL) {
      contourCellColorButtonGroup = new QButtonGroup(this);
      contourCellColorButtonGroup->setExclusive(false);
      QObject::connect(contourCellColorButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readContourColorPage()));
   }
   
   //
   // Update exising checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidContourCellColors) {
         contourCellColorCheckBoxes[i]->setText(contourCellColors->getColorNameByIndex(i));
         contourCellColorCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidContourCellColors; j++) {
      QCheckBox* cb = new QCheckBox(contourCellColors->getColorNameByIndex(j));
      contourCellColorCheckBoxes.push_back(cb);
      contourCellColorButtonGroup->addButton(cb, j);
      contourCellColorGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
   }
   
   //
   // Hide existing checkboxes that are not needed.
   //
   for (int k = numValidContourCellColors; k < numExistingCheckBoxes; k++) {
      contourCellColorCheckBoxes[k]->hide();
   }
}

/**
 * This slot is called when the contour cell color all on button is pressed.
 */
void
GuiDisplayControlDialog::contourCellColorAllOn()
{
   ContourCellColorFile* contourCellColors = theMainWindow->getBrainSet()->getContourCellColorFile();
   contourCellColors->setAllSelectedStatus(true);
   updateContourItems();
   readContourColorPage();
}

/**
 * This slot is called when the contour color all off button is pressed.
 */
void
GuiDisplayControlDialog::contourCellColorAllOff()
{
   ContourCellColorFile* contourCellColors = theMainWindow->getBrainSet()->getContourCellColorFile();
   contourCellColors->setAllSelectedStatus(false);
   updateContourItems();
   readContourColorPage();
}

/**
 * Create the contour main page
 */
void
GuiDisplayControlDialog::createContourMainPage()
{
   //
   // origin cross
   //
   contourOriginCrossCheckBox = new QCheckBox("Show Cross at Origin");
   QObject::connect(contourOriginCrossCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readContourMainPage()));
                    
   //
   // draw as lines check box
   //
   QLabel* drawModeLabel = new QLabel("Draw Mode ");
   contourDrawModeComboBox = new QComboBox;
   contourDrawModeComboBox->insertItem(DisplaySettingsContours::DRAW_MODE_POINTS, "Points");
   contourDrawModeComboBox->insertItem(DisplaySettingsContours::DRAW_MODE_LINES, "Lines");
   contourDrawModeComboBox->insertItem(DisplaySettingsContours::DRAW_MODE_POINTS_AND_LINES, "Points and Lines");
   QObject::connect(contourDrawModeComboBox, SIGNAL(activated(int)),
                    this, SLOT(readContourMainPage()));
   QHBoxLayout* drawModeLayout = new QHBoxLayout;
   drawModeLayout->addWidget(drawModeLabel);
   drawModeLayout->addWidget(contourDrawModeComboBox);
   drawModeLayout->addStretch();
   
   //
   // Show end points check box
   //
   contourShowEndPointsCheckBox = new QCheckBox("Show First Point In Each Contour In Red");
   QObject::connect(contourShowEndPointsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readContourMainPage()));
                                        
   //
   // point size
   //
   QLabel* pointSizeLabel = new QLabel("Point Size");
   contourPointSizeSpinBox = new QDoubleSpinBox;
   contourPointSizeSpinBox->setMinimum(0.5);
   contourPointSizeSpinBox->setMaximum(20.0);
   contourPointSizeSpinBox->setSingleStep(1.0);
   contourPointSizeSpinBox->setDecimals(2);
   QObject::connect(contourPointSizeSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readContourMainPage()));
   QHBoxLayout* pointSizeLayout = new QHBoxLayout;
   pointSizeLayout->addWidget(pointSizeLabel);
   pointSizeLayout->addWidget(contourPointSizeSpinBox);
   pointSizeLayout->addStretch();
   
   //
   // line size
   //
   QLabel* lineSizeLabel = new QLabel("Line Size");
   contourLineThicknessSpinBox = new QDoubleSpinBox;
   contourLineThicknessSpinBox->setMinimum(0.5);
   contourLineThicknessSpinBox->setMaximum(20.0);
   contourLineThicknessSpinBox->setSingleStep(1.0);
   contourLineThicknessSpinBox->setDecimals(2);
   QObject::connect(contourLineThicknessSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readContourMainPage()));
   QHBoxLayout* lineSizeLayout = new QHBoxLayout;
   lineSizeLayout->addWidget(lineSizeLabel);
   lineSizeLayout->addWidget(contourLineThicknessSpinBox);
   lineSizeLayout->addStretch();
   
   //
   // Show Contour Cells check box
   //
   contourShowCellsCheckBox = new QCheckBox("Show Contour Cells");
   QObject::connect(contourShowCellsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readContourMainPage()));
                    
   //
   // Contour Cell Size
   //
   QLabel* cellSizeLabel = new QLabel("Cell Size ");
   contourCellSizeSpinBox = new QSpinBox;
   contourCellSizeSpinBox->setMinimum(1);
   contourCellSizeSpinBox->setMaximum(10);
   contourCellSizeSpinBox->setSingleStep(1);
   QObject::connect(contourCellSizeSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(readContourMainPage()));
   QHBoxLayout* cellSizeLayout = new QHBoxLayout;
   cellSizeLayout->addWidget(cellSizeLabel);
   cellSizeLayout->addWidget(contourCellSizeSpinBox);
   cellSizeLayout->addStretch();

   //
   // Widget for Contour items
   //
   pageContourMain = new QWidget;
   QVBoxLayout* contourMainPageLayout = new QVBoxLayout(pageContourMain);
   contourMainPageLayout->addWidget(contourOriginCrossCheckBox);
   contourMainPageLayout->addLayout(drawModeLayout);
   contourMainPageLayout->addLayout(pointSizeLayout);
   contourMainPageLayout->addLayout(lineSizeLayout);
   contourMainPageLayout->addWidget(contourShowEndPointsCheckBox);
   contourMainPageLayout->addWidget(contourShowCellsCheckBox);
   contourMainPageLayout->addLayout(cellSizeLayout);
   contourMainPageLayout->addStretch();

   pageWidgetStack->addWidget(pageContourMain);
}

/**
 * read the contour main page.
 */
void 
GuiDisplayControlDialog::readContourMainPage()
{
   if (creatingDialog) {
      return;
   }
   if (pageContourMain == NULL) {
      return;
   }
   DisplaySettingsContours* dsc = theMainWindow->getBrainSet()->getDisplaySettingsContours();
   
   dsc->setDisplayCrossAtOrigin(contourOriginCrossCheckBox->isChecked());
   dsc->setDrawMode(static_cast<DisplaySettingsContours::DRAW_MODE>(contourDrawModeComboBox->currentIndex()));
   dsc->setShowEndPoints(contourShowEndPointsCheckBox->isChecked());
   dsc->setPointSize(contourPointSizeSpinBox->value());
   dsc->setLineThickness(contourLineThicknessSpinBox->value());
   dsc->setDisplayContourCells(contourShowCellsCheckBox->isChecked());
   dsc->setContourCellSize(contourCellSizeSpinBox->value());
   dsc->determineDisplayedContourCells();   
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * read the contour class page.
 */
void 
GuiDisplayControlDialog::readContourClassPage()
{
   if (creatingDialog) {
      return;
   }
   if (pageContourClass == NULL) {
      return;
   }
   ContourCellFile* cellFile = theMainWindow->getBrainSet()->getContourCellFile();
   const int numClasses = cellFile->getNumberOfCellClasses();
   if (numClasses == numValidContourCellClasses) {
      for (int i = 0; i < numValidContourCellClasses; i++) {
         cellFile->setCellClassSelectedByIndex(i, contourCellClassCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of contour cell class checkboxes does not equal number of contour cell classes."
                << std::endl;
   }
   DisplaySettingsContours* dsc = theMainWindow->getBrainSet()->getDisplaySettingsContours();
   dsc->determineDisplayedContourCells();   
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * read the contour color page.
 */
void 
GuiDisplayControlDialog::readContourColorPage()
{
   if (creatingDialog) {
      return;
   }
   if (pageContourColor == NULL) {
      return;
   }
   ContourCellColorFile* contourCellColors = theMainWindow->getBrainSet()->getContourCellColorFile();
   const int numColors = contourCellColors->getNumberOfColors();
   if (numColors == numValidContourCellColors) {
      for (int i = 0; i < numValidContourCellColors; i++) {
         contourCellColors->setSelected(i, contourCellColorCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of contour cell color checkboxes does not equal number of contour cell classes."
                << std::endl;
   }
   DisplaySettingsContours* dsc = theMainWindow->getBrainSet()->getDisplaySettingsContours();
   dsc->determineDisplayedContourCells();   
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}
      
/**
 * update all contour main page in dialog.
 */
void 
GuiDisplayControlDialog::updateContourMainPage()
{   
   if (pageContourMain == NULL) {
      return;
   }
   pageContourMain->setEnabled(validContourData);

   DisplaySettingsContours* dsc = theMainWindow->getBrainSet()->getDisplaySettingsContours();   
   contourOriginCrossCheckBox->setChecked(dsc->getDisplayCrossAtOrigin());
   contourDrawModeComboBox->setCurrentIndex(dsc->getDrawMode());
   contourShowEndPointsCheckBox->setChecked(dsc->getShowEndPoints());
   contourPointSizeSpinBox->setValue(dsc->getPointSize());
   contourLineThicknessSpinBox->setValue(dsc->getLineThickness());
   contourShowCellsCheckBox->setChecked(dsc->getDisplayContourCells());
   contourCellSizeSpinBox->setValue(static_cast<int>(dsc->getContourCellSize()));
}

/**
 * update all contour class page in dialog.
 */
void 
GuiDisplayControlDialog::updateContourClassPage(const bool filesChanged)
{
   if (pageContourClass == NULL) {
      return;
   }
   pageContourClass->setEnabled(validContourData);

   if (filesChanged) {
      createAndUpdateContourCellClassCheckBoxes();
   }
   ContourCellFile* contourCellFile = theMainWindow->getBrainSet()->getContourCellFile();   
   const int numClasses = contourCellFile->getNumberOfCellClasses();
   if (numClasses == numValidContourCellClasses) {
      for (int i = 0; i < numValidContourCellClasses; i++) {
         contourCellClassCheckBoxes[i]->setChecked(contourCellFile->getCellClassSelectedByIndex(i));
      }
   }
   else {
      std::cerr << "Number of contour cell class checkboxes does not equal number of contour cell classes."
                << std::endl;
   }
}

/**
 * update all contour color page in dialog.
 */
void 
GuiDisplayControlDialog::updateContourColorPage(const bool filesChanged)
{
   if (pageContourColor == NULL) {
      return;
   }
   pageContourColor->setEnabled(validContourData);
   
   if (filesChanged) {
      createAndUpdateContourCellColorCheckBoxes();
   }
   ContourCellColorFile* contourCellColorFile = theMainWindow->getBrainSet()->getContourCellColorFile();
   const int numColors = contourCellColorFile->getNumberOfColors();
   if (numColors == numValidContourCellColors) {
      for (int i = 0; i < numValidContourCellColors; i++) {
         contourCellColorCheckBoxes[i]->setChecked(contourCellColorFile->getSelected(i));
      }
   }
   else {
      std::cerr << "Number of contour cell color checkboxes does not equal number of contour cell colors."
                << std::endl;
   }
}
      
/**
 * Update the contour items
 */
void
GuiDisplayControlDialog::updateContourItems(const bool filesChanged)
{
   updatePageSelectionComboBox();

   updateContourMainPage();
   updateContourClassPage(filesChanged);
   updateContourColorPage(filesChanged);
}

/**
 * Create the CoCoMac Display control sub page.
 */
void
GuiDisplayControlDialog::createCocomacDisplayPage()
{
   //
   // Connection radio buttons
   //
   cocomacAfferentRadioButton = new QRadioButton("Afferent");
   cocomacEfferentRadioButton = new QRadioButton("Efferent");
   cocomacAfferentAndEfferentRadioButton = new QRadioButton("Afferent and Efferent");
   cocomacAfferentOrEfferentRadioButton = new QRadioButton("Afferent or Efferent");
   
   //
   // button group for connection display type
   //
   QButtonGroup* connBG = new QButtonGroup(this); 
   QObject::connect(connBG, SIGNAL(buttonClicked(int)),
                    this, SLOT(readCocomacDisplayPage()));
   connBG->addButton(cocomacAfferentRadioButton);
   connBG->addButton(cocomacEfferentRadioButton);
   connBG->addButton(cocomacAfferentAndEfferentRadioButton);
   connBG->addButton(cocomacAfferentOrEfferentRadioButton);
   
   //
   // Group box and layout for connection type buttons
   //
   QGroupBox* connGroupBox = new QGroupBox("View Connection");
   QVBoxLayout* connGroupLayout = new QVBoxLayout(connGroupBox);
   connGroupLayout->addWidget(cocomacAfferentRadioButton);
   connGroupLayout->addWidget(cocomacEfferentRadioButton);
   connGroupLayout->addWidget(cocomacAfferentAndEfferentRadioButton);
   connGroupLayout->addWidget(cocomacAfferentOrEfferentRadioButton);
   
   //
   // Paint column label and combo box
   //
   QLabel* paintLabel = new QLabel("Paint Column ");
   cocomacPaintColumnComboBox = new QComboBox;
   cocomacPaintColumnComboBox->setMinimumWidth(400);
   QObject::connect(cocomacPaintColumnComboBox, SIGNAL(activated(int)),
                    this, SLOT(readCocomacDisplayPage()));
   QHBoxLayout* paintLayout = new QHBoxLayout;
   paintLayout->addWidget(paintLabel);
   paintLayout->addWidget(cocomacPaintColumnComboBox);
   paintLayout->setStretchFactor(paintLabel, 0);
   paintLayout->setStretchFactor(cocomacPaintColumnComboBox, 100);

   //
   // Create the page and layout its widgets
   //
   pageCocomacDisplay = new QWidget; 
   QVBoxLayout* cocoLayout = new QVBoxLayout(pageCocomacDisplay);
   cocoLayout->addWidget(connGroupBox);
   cocoLayout->addLayout(paintLayout);
   cocoLayout->addStretch();

   pageWidgetStack->addWidget(pageCocomacDisplay);
}

/**
 * Create the CoCoMac File Info sub page.
 */
void
GuiDisplayControlDialog::createCocomacFileInformationPage()
{
   //
   // CoCoMac version label
   //
   QString spaces("                                                                           ");
   cocomacVersionLabel = new QLabel(spaces);
   
   //
   // CoCoMac export date
   //
   cocomacExportDate = new QLabel(spaces);
   
   //
   //  CoCoMac data type
   //
   cocomacDataType = new QLabel(spaces);
   
   //
   // comments text area
   //
   cocomacCommentsTextEdit = new QTextEdit;
   cocomacCommentsTextEdit->setFixedSize(QSize(500, 150));
   
   //
   // Projections text area
   //
   cocomacProjectionsTextEdit = new QTextEdit;
   cocomacProjectionsTextEdit->setFixedSize(QSize(500, 300));
   
   //
   // Page and layout
   //
   pageCocomacInformation = new QWidget;
   QVBoxLayout* cocoLayout = new QVBoxLayout(pageCocomacInformation);
   cocoLayout->addWidget(cocomacVersionLabel);
   cocoLayout->addWidget(cocomacExportDate);
   cocoLayout->addWidget(cocomacDataType);
   cocoLayout->addWidget(cocomacCommentsTextEdit);
   cocoLayout->addWidget(cocomacProjectionsTextEdit);
   cocoLayout->addStretch();

   pageWidgetStack->addWidget(pageCocomacInformation);
}

/**
 * Read the CoCoMac selections.
 */
void
GuiDisplayControlDialog::readCocomacDisplayPage()
{
   if (pageCocomacDisplay == NULL) {
      return;
   }
   DisplaySettingsCoCoMac* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCoCoMac();
   
   if (cocomacAfferentRadioButton->isChecked()) {
      dsc->setConnectionDisplayType(DisplaySettingsCoCoMac::CONNECTION_DISPLAY_AFFERENT);
   }
   else if (cocomacEfferentRadioButton->isChecked()) {
      dsc->setConnectionDisplayType(DisplaySettingsCoCoMac::CONNECTION_DISPLAY_EFFERENT);
   }
   else if (cocomacAfferentAndEfferentRadioButton->isChecked()) {
      dsc->setConnectionDisplayType(DisplaySettingsCoCoMac::CONNECTION_DISPLAY_AFFERENT_AND_EFFERENT);
   }
   else if (cocomacAfferentOrEfferentRadioButton->isChecked()) {
      dsc->setConnectionDisplayType(DisplaySettingsCoCoMac::CONNECTION_DISPLAY_AFFERENT_OR_EFFERENT);
   }
   
   dsc->setSelectedPaintColumn(cocomacPaintColumnComboBox->currentIndex());
}

/**
 * update the cocomac display page.
 */
void 
GuiDisplayControlDialog::updateCocomacDisplayPage()
{
   if (pageCocomacDisplay == NULL) {
      return;
   }
   DisplaySettingsCoCoMac* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCoCoMac();
   CocomacConnectivityFile* cocomacFile = theMainWindow->getBrainSet()->getCocomacFile();
   
   switch(dsc->getConnectionDisplayType()) {
      case DisplaySettingsCoCoMac::CONNECTION_DISPLAY_AFFERENT:
         cocomacAfferentRadioButton->setChecked(true);
         break;
      case DisplaySettingsCoCoMac::CONNECTION_DISPLAY_EFFERENT:
         cocomacEfferentRadioButton->setChecked(true);
         break;
      case DisplaySettingsCoCoMac::CONNECTION_DISPLAY_AFFERENT_AND_EFFERENT:
         cocomacAfferentAndEfferentRadioButton->setChecked(true);
         break;
      case DisplaySettingsCoCoMac::CONNECTION_DISPLAY_AFFERENT_OR_EFFERENT:
         cocomacAfferentOrEfferentRadioButton->setChecked(true);
         break;
   }
   
   cocomacPaintColumnComboBox->clear();
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   for (int i = 0; i < pf->getNumberOfColumns(); i++) {
      cocomacPaintColumnComboBox->addItem(pf->getColumnName(i));
   }
   
   if (cocomacPaintColumnComboBox->count() > 0) {
      int num = dsc->getSelectedPaintColumn();
      if (num >= cocomacPaintColumnComboBox->count()) {
         num = 0;
         dsc->setSelectedPaintColumn(num);
      }
      cocomacPaintColumnComboBox->setCurrentIndex(num);
   }
   
   pageCocomacDisplay->setEnabled(cocomacFile->empty() == false);
}

/**
 * update the cocomac information page.
 */
void 
GuiDisplayControlDialog::updateCocomacInformationPage()
{
   if (pageCocomacInformation == NULL) {
      return;
   }
   CocomacConnectivityFile* cocomacFile = theMainWindow->getBrainSet()->getCocomacFile();
   QString version("Version: ");
   version.append(cocomacFile->getVersion());
   cocomacVersionLabel->setText(version);
   
   QString exportDate("Export Date: ");
   exportDate.append(cocomacFile->getExportDate());
   cocomacExportDate->setText(exportDate);
   
   QString dataType("Data Type: ");
   dataType.append(cocomacFile->getDataType());
   cocomacDataType->setText(dataType);
   
   cocomacCommentsTextEdit->setPlainText(cocomacFile->getComments());
   
   QString projections;
   const int numProj = cocomacFile->getNumberOfCocomacProjections();
   for (int i = 0; i < numProj; i++) {
      CocomacProjection* proj = cocomacFile->getCocomacProjection(i);
      projections.append(proj->getSourceSite());
      projections.append(" ");
      projections.append(proj->getDensity());
      projections.append(" ");
      projections.append(proj->getTargetSite());
      projections.append("\n");
   }
   cocomacProjectionsTextEdit->setPlainText(projections);
   
   pageCocomacInformation->setEnabled(cocomacFile->empty() == false);
}

/**
 * Update the CoCoMac items in the dialog.
 */
void
GuiDisplayControlDialog::updateCocomacItems()
{
   updatePageSelectionComboBox();

   if (pageOverlayUnderlaySurface != NULL) {
      primaryOverlayCocomacButton->setEnabled(validCocomacData);
      secondaryOverlayCocomacButton->setEnabled(validCocomacData);
      underlayCocomacButton->setEnabled(validCocomacData);
      cocomacSelectionLabel->setEnabled(validCocomacData);
   }

   updateCocomacDisplayPage();
   updateCocomacInformationPage();
}

/**
 * Create the surface and volume page
 */
void
GuiDisplayControlDialog::createSurfaceAndVolumePage()
{
   //
   // Show surface check box
   //
   surfaceAndVolumeShowSurfaceCheckBox = new QCheckBox("Show Surface");
   QObject::connect(surfaceAndVolumeShowSurfaceCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
                                                       
   //
   // Group box for surface control
   //
   QGroupBox* surfaceGroupBox = new QGroupBox("Surface");
   QVBoxLayout* surfaceGroupLayout =  new QVBoxLayout(surfaceGroupBox);
   surfaceGroupLayout->addWidget(surfaceAndVolumeShowSurfaceCheckBox);
   surfaceGroupBox->setFixedSize(surfaceGroupBox->sizeHint());
   
   //
   // check box and spin box for horizontal slice
   //
   surfaceAndVolumeHorizontalSliceCheckBox = new QCheckBox("Horizontal Slice");
   QObject::connect(surfaceAndVolumeHorizontalSliceCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
   surfaceAndVolumeHorizontalSliceSpinBox = new QSpinBox;
   surfaceAndVolumeHorizontalSliceSpinBox->setMinimum(1);
   surfaceAndVolumeHorizontalSliceSpinBox->setMaximum(10000);
   surfaceAndVolumeHorizontalSliceSpinBox->setSingleStep(1); 
   QObject::connect(surfaceAndVolumeHorizontalSliceSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
    
   //
   // check box and spin box for parasagittal slice
   //
   surfaceAndVolumeParasagittalSliceCheckBox = new QCheckBox("Parasagittal Slice");
   QObject::connect(surfaceAndVolumeParasagittalSliceCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
   surfaceAndVolumeParasagittalSliceSpinBox = new QSpinBox;
   surfaceAndVolumeParasagittalSliceSpinBox->setMinimum(1);
   surfaceAndVolumeParasagittalSliceSpinBox->setMaximum(10000);
   surfaceAndVolumeParasagittalSliceSpinBox->setSingleStep(1); 
   QObject::connect(surfaceAndVolumeParasagittalSliceSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
    
   //
   // check box and spin box for coronal slice
   //
   surfaceAndVolumeCoronalSliceCheckBox = new QCheckBox("Coronal Slice");
   QObject::connect(surfaceAndVolumeCoronalSliceCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
   surfaceAndVolumeCoronalSliceSpinBox = new QSpinBox;
   surfaceAndVolumeCoronalSliceSpinBox->setMinimum(1);
   surfaceAndVolumeCoronalSliceSpinBox->setMaximum(10000);
   surfaceAndVolumeCoronalSliceSpinBox->setSingleStep(1); 
   QObject::connect(surfaceAndVolumeCoronalSliceSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
    
   surfaceAndVolumeAnatomyBlackCheckBox = new QCheckBox("Draw Black Voxels");
   QObject::connect(surfaceAndVolumeAnatomyBlackCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
                                                        
   //
   // Group box for anatomical slices
   //
   QGroupBox* anatomyGroupBox = new QGroupBox("Anatomy Volume");
   QGridLayout* anatomyGridLayout = new QGridLayout(anatomyGroupBox);
   anatomyGridLayout->addWidget(surfaceAndVolumeHorizontalSliceCheckBox, 0, 0);
   anatomyGridLayout->addWidget(surfaceAndVolumeHorizontalSliceSpinBox, 0, 1);
   anatomyGridLayout->addWidget(surfaceAndVolumeParasagittalSliceCheckBox, 1, 0);
   anatomyGridLayout->addWidget(surfaceAndVolumeParasagittalSliceSpinBox, 1, 1);
   anatomyGridLayout->addWidget(surfaceAndVolumeCoronalSliceCheckBox, 2, 0);
   anatomyGridLayout->addWidget(surfaceAndVolumeCoronalSliceSpinBox, 2, 1);
   anatomyGridLayout->addWidget(surfaceAndVolumeAnatomyBlackCheckBox, 3, 0);
   anatomyGroupBox->setFixedSize(anatomyGroupBox->sizeHint());
   
   
   //
   // show primary overlay on slices
   //
   surfaceAndVolumeShowPrimaryCheckBox = new QCheckBox("Show Primary Overlay");
   QObject::connect(surfaceAndVolumeShowPrimaryCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
   
   //
   // show secondary overlay on slices
   //
   surfaceAndVolumeShowSecondaryCheckBox = new QCheckBox("Show Secondary Overlay");
   QObject::connect(surfaceAndVolumeShowSecondaryCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));

   //
   // Group box for overlay volume control
   //
   QGroupBox* surfaceAndVolumeOverlaysGroupBox = new QGroupBox("Volume Overlays");
   QVBoxLayout* surfaceAndVolumeOverlaysGroupBoxLayout = new QVBoxLayout(surfaceAndVolumeOverlaysGroupBox);
   surfaceAndVolumeOverlaysGroupBoxLayout->addWidget(surfaceAndVolumeShowPrimaryCheckBox);
   surfaceAndVolumeOverlaysGroupBoxLayout->addWidget(surfaceAndVolumeShowSecondaryCheckBox);
   surfaceAndVolumeOverlaysGroupBox->setFixedSize(surfaceAndVolumeOverlaysGroupBox->sizeHint());

   
  
   //
   // Show functional data cloud checkbox
   //
   surfaceAndVolumeShowFunctionalCloudCheckBox = new QCheckBox("Show Volume Cloud");
   QObject::connect(surfaceAndVolumeShowFunctionalCloudCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
   
   //
   // Functional data cloud opacity check box and spin box
   //
   surfaceAndVolumeFunctionalCloudOpacityCheckBox = new QCheckBox("Opacity");
   QObject::connect(surfaceAndVolumeFunctionalCloudOpacityCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
   surfaceAndVolumeFunctionalCloudOpacitySpinBox = new QDoubleSpinBox;
   surfaceAndVolumeFunctionalCloudOpacitySpinBox->setMinimum(0.0);
   surfaceAndVolumeFunctionalCloudOpacitySpinBox->setMaximum(1.0);
   surfaceAndVolumeFunctionalCloudOpacitySpinBox->setSingleStep(0.05);
   surfaceAndVolumeFunctionalCloudOpacitySpinBox->setDecimals(3);
   QObject::connect(surfaceAndVolumeFunctionalCloudOpacitySpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
   QHBoxLayout* functionalVolumeLayout = new QHBoxLayout;
   functionalVolumeLayout->addWidget(surfaceAndVolumeFunctionalCloudOpacityCheckBox);
   functionalVolumeLayout->addWidget(surfaceAndVolumeFunctionalCloudOpacitySpinBox);
   
   //
   // Group box for functional volume control
   //
   surfaceAndVolumeFunctionalGroupBox = new QGroupBox("Functional Volume");
   QVBoxLayout* surfaceAndVolumeFunctionalGroupLayout = new QVBoxLayout(surfaceAndVolumeFunctionalGroupBox);
   surfaceAndVolumeFunctionalGroupLayout->addWidget(surfaceAndVolumeShowFunctionalCloudCheckBox);
   surfaceAndVolumeFunctionalGroupLayout->addLayout(functionalVolumeLayout);


   //
   // Distance threshold
   //
   surfaceAndVolumeFunctionalDistanceSpinBox = new QDoubleSpinBox;
   surfaceAndVolumeFunctionalDistanceSpinBox->setMinimum(0.0);
   surfaceAndVolumeFunctionalDistanceSpinBox->setMaximum(1000.0);
   surfaceAndVolumeFunctionalDistanceSpinBox->setSingleStep(10.0);
   surfaceAndVolumeFunctionalDistanceSpinBox->setDecimals(1);
   QObject::connect(surfaceAndVolumeFunctionalDistanceSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
   surfaceAndVolumeFunctionalGroupBox->setFixedSize(surfaceAndVolumeFunctionalGroupBox->sizeHint());
   QGroupBox* distanceGroupBox = new QGroupBox("Distance Threshold");
   QVBoxLayout* distanceGroupLayout = new QVBoxLayout(distanceGroupBox);
   distanceGroupLayout->addWidget(surfaceAndVolumeFunctionalDistanceSpinBox);
   distanceGroupBox->setFixedSize(distanceGroupBox->sizeHint());
   distanceGroupBox->hide();
   
   
   //
   // Show segmentation data cloud checkbox
   //
   surfaceAndVolumeShowSegmentationCloudCheckBox = new QCheckBox("Show Volume Cloud");
   QObject::connect(surfaceAndVolumeShowSegmentationCloudCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
   
   //
   // Group box for segmentation volume control
   //
   surfaceAndVolumeSegmentationGroupBox = new QGroupBox("Segmentation Volume");
   QVBoxLayout* surfaceAndVolumeSegmentationGroupLayout = new QVBoxLayout(surfaceAndVolumeSegmentationGroupBox);
   surfaceAndVolumeSegmentationGroupLayout->addWidget(surfaceAndVolumeShowSegmentationCloudCheckBox);
   surfaceAndVolumeSegmentationGroupBox->setFixedSize(surfaceAndVolumeSegmentationGroupBox->sizeHint());
   
   //
   // Show vector data cloud check box
   //
   surfaceAndVolumeShowVectorCloudCheckBox = new QCheckBox("Show Volume Cloud");
   QObject::connect(surfaceAndVolumeShowVectorCloudCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSurfaceAndVolumeSelections()));
   surfaceAndVolumeVectorVBox = new QGroupBox("Vector Volume");
   QVBoxLayout* surfaceAndVolumeVectorLayout = new QVBoxLayout(surfaceAndVolumeVectorVBox);
   surfaceAndVolumeVectorLayout->addWidget(surfaceAndVolumeShowVectorCloudCheckBox);
   surfaceAndVolumeVectorVBox->setFixedSize(surfaceAndVolumeVectorVBox->sizeHint());

   //
   // Create page ang its layout
   //                    
   pageSurfaceAndVolume = new QWidget;
   pageWidgetStack->addWidget(pageSurfaceAndVolume); //, PAGE_NAME_SURFACE_AND_VOLUME);
   QVBoxLayout* surfaceAndVolumeLayout = new QVBoxLayout(pageSurfaceAndVolume);
   surfaceAndVolumeLayout->addWidget(surfaceGroupBox);
   surfaceAndVolumeLayout->addWidget(anatomyGroupBox);
   surfaceAndVolumeLayout->addWidget(surfaceAndVolumeOverlaysGroupBox);
   surfaceAndVolumeLayout->addWidget(surfaceAndVolumeFunctionalGroupBox);
   surfaceAndVolumeLayout->addWidget(distanceGroupBox);
   surfaceAndVolumeLayout->addWidget(surfaceAndVolumeSegmentationGroupBox);
   surfaceAndVolumeLayout->addWidget(surfaceAndVolumeVectorVBox);
   surfaceAndVolumeLayout->addStretch();
   
   pageSurfaceAndVolumeWidgetGroup = new WuQWidgetGroup(this);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeShowSurfaceCheckBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeHorizontalSliceCheckBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeParasagittalSliceCheckBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeCoronalSliceCheckBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeCoronalSliceSpinBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeAnatomyBlackCheckBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeShowPrimaryCheckBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeShowSecondaryCheckBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeShowFunctionalCloudCheckBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeFunctionalCloudOpacityCheckBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeFunctionalCloudOpacitySpinBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeShowSegmentationCloudCheckBox);
   pageSurfaceAndVolumeWidgetGroup->addWidget(surfaceAndVolumeShowVectorCloudCheckBox);
}

/**
 * Read the selections on the surface and volume page.
 */
void
GuiDisplayControlDialog::readSurfaceAndVolumeSelections()
{
   BrainModelSurfaceAndVolume* bmsv = theMainWindow->getBrainSet()->getBrainModelSurfaceAndVolume();
   if (bmsv != NULL) {
      //
      // Setting the checkboxes of volume slices in "updateSurfaceAndVolumeItems()" 
      // triggers QT signals which then call this method.  So see if items are
      // being updated in the dialog.
      //
      bmsv->setDisplaySurface(surfaceAndVolumeShowSurfaceCheckBox->isChecked());
      
      bmsv->setDisplayHorizontalSlice(surfaceAndVolumeHorizontalSliceCheckBox->isChecked());
      bmsv->setDisplayParasagittalSlice(surfaceAndVolumeParasagittalSliceCheckBox->isChecked());
      bmsv->setDisplayCoronalSlice(surfaceAndVolumeCoronalSliceCheckBox->isChecked());
      
      const int slices[3] = { surfaceAndVolumeParasagittalSliceSpinBox->value(),
                              surfaceAndVolumeCoronalSliceSpinBox->value(),
                              surfaceAndVolumeHorizontalSliceSpinBox->value() };
      bmsv->setSelectedSlices(slices);
      
      bmsv->setDisplayPrimaryOverlayVolumeOnSlices(surfaceAndVolumeShowPrimaryCheckBox->isChecked());
      bmsv->setDisplaySecondaryOverlayVolumeOnSlices(surfaceAndVolumeShowSecondaryCheckBox->isChecked());

      bmsv->setDisplayFunctionalVolumeCloud(surfaceAndVolumeShowFunctionalCloudCheckBox->isChecked());
      bmsv->setFunctionalVolumeCloudOpacityEnabled(surfaceAndVolumeFunctionalCloudOpacityCheckBox->isChecked());
      bmsv->setFunctionalVolumeCloudOpacity(surfaceAndVolumeFunctionalCloudOpacitySpinBox->value());
      bmsv->setFunctionalVolumeDistanceThreshold(surfaceAndVolumeFunctionalDistanceSpinBox->value());
      bmsv->setDisplaySegmentationVolumeCloud(surfaceAndVolumeShowSegmentationCloudCheckBox->isChecked());
      bmsv->setDisplayVectorVolumeCloud(surfaceAndVolumeShowVectorCloudCheckBox->isChecked());
      bmsv->setDrawAnatomyBlackVoxels(surfaceAndVolumeAnatomyBlackCheckBox->isChecked());
   }
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * update the surface and volume selections.
 */
void 
GuiDisplayControlDialog::updateSurfaceAndVolumeItems()
{
   updatePageSelectionComboBox();
   if (pageSurfaceAndVolume == NULL) {
      return;
   }
   pageSurfaceAndVolumeWidgetGroup->blockSignals(true);
   
   BrainModelSurfaceAndVolume* bmsv = theMainWindow->getBrainSet()->getBrainModelSurfaceAndVolume();
   if (bmsv != NULL) {
      pageSurfaceAndVolume->setEnabled(validSurfaceAndVolumeData);
      
      surfaceAndVolumeShowSurfaceCheckBox->setChecked(bmsv->getDisplaySurface());
      
      surfaceAndVolumeHorizontalSliceCheckBox->setChecked(bmsv->getDisplayHorizontalSlice());
      surfaceAndVolumeParasagittalSliceCheckBox->setChecked(bmsv->getDisplayParasagittalSlice());
      surfaceAndVolumeCoronalSliceCheckBox->setChecked(bmsv->getDisplayCoronalSlice());
      
      int dim[3] = { 0, 0, 0 };
      VolumeFile* anatomyVolumeFile = bmsv->getAnatomyVolumeFile();
      if (anatomyVolumeFile != NULL) {
         anatomyVolumeFile->getDimensions(dim);
      }
      
      surfaceAndVolumeParasagittalSliceSpinBox->setMaximum(dim[0]);
      surfaceAndVolumeCoronalSliceSpinBox->setMaximum(dim[1]);
      surfaceAndVolumeHorizontalSliceSpinBox->setMaximum(dim[2]);

      int slices[3];
      bmsv->getSelectedSlices(slices);
      surfaceAndVolumeParasagittalSliceSpinBox->setValue(slices[0]);
      surfaceAndVolumeCoronalSliceSpinBox->setValue(slices[1]);
      surfaceAndVolumeHorizontalSliceSpinBox->setValue(slices[2]);

      surfaceAndVolumeShowPrimaryCheckBox->setChecked(bmsv->getDisplayPrimaryOverlayVolumeOnSlices());
      surfaceAndVolumeShowSecondaryCheckBox->setChecked(bmsv->getDisplaySecondaryOverlayVolumeOnSlices());
      
      if (bmsv->getFunctionalVolumeFile() != NULL) {
         surfaceAndVolumeFunctionalGroupBox->setEnabled(true);
         surfaceAndVolumeShowFunctionalCloudCheckBox->setChecked(bmsv->getDisplayFunctionalVolumeCloud());
         surfaceAndVolumeFunctionalCloudOpacityCheckBox->setChecked(bmsv->getFunctionalVolumeCloudOpacityEnabled());
         surfaceAndVolumeFunctionalCloudOpacitySpinBox->setValue(bmsv->getFunctionalVolumeCloudOpacity());
      }
      else {
         surfaceAndVolumeFunctionalGroupBox->setEnabled(false);
      }
      surfaceAndVolumeFunctionalDistanceSpinBox->setValue(bmsv->getFunctionalVolumeDistanceThreshold());

      if (bmsv->getSegmentationVolumeFile() != NULL) {
         surfaceAndVolumeSegmentationGroupBox->setEnabled(true);
         surfaceAndVolumeShowSegmentationCloudCheckBox->setChecked(bmsv->getDisplaySegmentationVolumeCloud());
      }
      else {
         surfaceAndVolumeSegmentationGroupBox->setEnabled(false);
      }
      surfaceAndVolumeAnatomyBlackCheckBox->setChecked(bmsv->getDrawAnatomyBlackVoxels());
      
      if (bmsv->getVectorVolumeFile() != NULL) {
         surfaceAndVolumeVectorVBox->setEnabled(true);
         surfaceAndVolumeShowVectorCloudCheckBox->setChecked(bmsv->getDisplayVectorVolumeCloud());
      }
      else {
         surfaceAndVolumeVectorVBox->setEnabled(false);
      }
   }
   else {
      pageSurfaceAndVolume->setEnabled(false);
   }
   
   updatePageSelectionComboBox();
   pageSurfaceAndVolumeWidgetGroup->blockSignals(false);
}      

/**
 * create the surface vector selection page.
 */
void 
GuiDisplayControlDialog::createSurfaceVectorSelectionPage()
{
   //
   // Grid for column selection page
   // box prevents gray background from showing through
   //
   pageSurfaceVectorSelection = new QWidget;
   pageSurfaceVectorSelection->setFixedWidth(450);
   surfaceVectorColumnsGridLayout = new QGridLayout(pageSurfaceVectorSelection);
   surfaceVectorColumnsGridLayout->setColumnStretch(0, 0);
   surfaceVectorColumnsGridLayout->setColumnStretch(1, 0);
   surfaceVectorColumnsGridLayout->setColumnStretch(2, 1);
   const int rowStretchNumber = 15000;
   surfaceVectorColumnsGridLayout->addWidget(new QLabel(""),
                                        rowStretchNumber, 0, Qt::AlignLeft);
   surfaceVectorColumnsGridLayout->setRowStretch(rowStretchNumber, 1000);

   //
   // Button Group for selections
   //
   surfaceVectorButtonGroup = new QButtonGroup(this);
   QObject::connect(surfaceVectorButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(readSurfaceVectorSelectionPage()));
   
   //
   // Button Group for comment
   //
   surfaceVectorCommentButtonGroup = new QButtonGroup(this);
   QObject::connect(surfaceVectorCommentButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotSurfaceVectorCommentPushButton(int)));

   pageWidgetStack->addWidget(pageSurfaceVectorSelection);
}

/**
 * create the surface vector settings page.
 */
void 
GuiDisplayControlDialog::createSurfaceVectorSettingsPage()
{
   //
   // Display mode
   //
   QLabel* displayModeLabel = new QLabel("Display Mode");
   surfaceVectorDisplayModeComboBox = new QComboBox;
   surfaceVectorDisplayModeComboBox->insertItem(DisplaySettingsSurfaceVectors::DISPLAY_MODE_ALL,
                                                "All");
   surfaceVectorDisplayModeComboBox->insertItem(DisplaySettingsSurfaceVectors::DISPLAY_MODE_NONE,
                                                "None");
   surfaceVectorDisplayModeComboBox->insertItem(DisplaySettingsSurfaceVectors::DISPLAY_MODE_SPARSE,
                                                "Sparse");
   QObject::connect(surfaceVectorDisplayModeComboBox, SIGNAL(activated(int)),
                    this, SLOT(readSurfaceVectorSettingsPage()));
   
   //
   // Sparse distance spin box
   //
   QLabel* sparseDistanceLabel = new QLabel("Sparse Distance");
   surfaceVectorSparseDistanceSpinBox = new QSpinBox;
   surfaceVectorSparseDistanceSpinBox->setMinimum(0);
   surfaceVectorSparseDistanceSpinBox->setMaximum(100000);
   surfaceVectorSparseDistanceSpinBox->setSingleStep(1);
   QObject::connect(surfaceVectorSparseDistanceSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(readSurfaceVectorSettingsPage()));
                    
   //
   // Length multiplier
   //
   QLabel* lengthLabel = new QLabel("Length Multiplier ");
   surfaceVectorLengthMultiplierDoubleSpinBox = new QDoubleSpinBox;
   surfaceVectorLengthMultiplierDoubleSpinBox->setMinimum(0.0001);
   surfaceVectorLengthMultiplierDoubleSpinBox->setMaximum(1000.0);
   surfaceVectorLengthMultiplierDoubleSpinBox->setSingleStep(1.0);
   surfaceVectorLengthMultiplierDoubleSpinBox->setDecimals(3);
   surfaceVectorLengthMultiplierDoubleSpinBox->setValue(1.0);
   QObject::connect(surfaceVectorLengthMultiplierDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readSurfaceVectorSettingsPage()));
                    
   //
   // Box for settings page
   //
   pageSurfaceVectorSettings = new QWidget;
   QGridLayout* surfaceVectorSettingsPageLayout = new QGridLayout(pageSurfaceVectorSettings);
   surfaceVectorSettingsPageLayout->addWidget(displayModeLabel, 0, 0);
   surfaceVectorSettingsPageLayout->addWidget(surfaceVectorDisplayModeComboBox, 0, 1);
   surfaceVectorSettingsPageLayout->addWidget(sparseDistanceLabel, 1, 0);
   surfaceVectorSettingsPageLayout->addWidget(surfaceVectorSparseDistanceSpinBox, 1, 1);
   surfaceVectorSettingsPageLayout->addWidget(lengthLabel, 2, 0);
   surfaceVectorSettingsPageLayout->addWidget(surfaceVectorLengthMultiplierDoubleSpinBox, 2, 1);
   pageSurfaceVectorSettings->setFixedSize(pageSurfaceVectorSettings->sizeHint());

   pageWidgetStack->addWidget(pageSurfaceVectorSettings);
}

/**
 * update surface vector selection page.
 */
void 
GuiDisplayControlDialog::updateSurfaceVectorSelectionPage()
{
   if (pageSurfaceVectorSelection == NULL) {
      return;
   }
   DisplaySettingsSurfaceVectors* dssv = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceVectors();
   SurfaceVectorFile* svf = theMainWindow->getBrainSet()->getSurfaceVectorFile();
   
   const int numInPage = static_cast<int>(surfaceVectorRadioButtons.size());
   for (int i = numInPage; i < svf->getNumberOfColumns(); i++) {
      QRadioButton* rb = new QRadioButton("");
      surfaceVectorButtonGroup->addButton(rb, i);
      surfaceVectorRadioButtons.push_back(rb);
      
      QPushButton* pb = new QPushButton("?");
      pb->setObjectName("pb");
      pb->setFixedSize(pb->sizeHint());
      pb->setAutoDefault(false);
      surfaceVectorCommentButtonGroup->addButton(pb, i);
      surfaceVectorCommentPushButtons.push_back(pb);
      
      QLineEdit* le = new QLineEdit;
      le->setObjectName("le");
      surfaceVectorLineEdits.push_back(le);
      
      surfaceVectorColumnsGridLayout->addWidget(rb, i, 0);
      surfaceVectorColumnsGridLayout->addWidget(pb, i, 1);
      surfaceVectorColumnsGridLayout->addWidget(le, i, 2);
   }

   for (int i = 0; i < static_cast<int>(surfaceVectorRadioButtons.size()); i++) {
      if (i < svf->getNumberOfColumns()) {
         surfaceVectorRadioButtons[i]->show();
         surfaceVectorCommentPushButtons[i]->show();
         surfaceVectorLineEdits[i]->setText(svf->getColumnName(i));
         surfaceVectorLineEdits[i]->show();
      }
      else {
         surfaceVectorRadioButtons[i]->hide();
         surfaceVectorCommentPushButtons[i]->hide();
         surfaceVectorLineEdits[i]->hide();
      }
   }
   
   if ((dssv->getSelectedColumn(surfaceModelIndex) >= 0) &&
       (dssv->getSelectedColumn(surfaceModelIndex) < static_cast<int>(surfaceVectorRadioButtons.size()))) {
      surfaceVectorRadioButtons[dssv->getSelectedColumn(surfaceModelIndex)]->setChecked(true);
   }
   pageSurfaceVectorSelection->setEnabled(validSurfaceVectorData);
}

/**
 * update surface vector settings page.
 */
void 
GuiDisplayControlDialog::updateSurfaceVectorSettingsPage()
{
   if (pageSurfaceVectorSettings == NULL) {
      return;
   }
   DisplaySettingsSurfaceVectors* dssv = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceVectors();
   
   surfaceVectorDisplayModeComboBox->setCurrentIndex(dssv->getDisplayMode());
   surfaceVectorSparseDistanceSpinBox->setValue(dssv->getSparseDisplayDistance());
   surfaceVectorLengthMultiplierDoubleSpinBox->setValue(dssv->getLengthMultiplier());
   pageSurfaceVectorSettings->setEnabled(validSurfaceVectorData);
}

/**
 * update surface vector items in dialog.
 */
void 
GuiDisplayControlDialog::updateSurfaceVectorItems()
{
   updatePageSelectionComboBox();

   updateSurfaceVectorSelectionPage();
   updateSurfaceVectorSettingsPage();
}
      
/**
 * read the surface vector selection page.
 */
void 
GuiDisplayControlDialog::readSurfaceVectorSelectionPage()
{
   if (pageSurfaceVectorSelection == NULL) {
      return;
   }
   SurfaceVectorFile* svf = theMainWindow->getBrainSet()->getSurfaceVectorFile();
   const int num = std::min(static_cast<int>(surfaceVectorRadioButtons.size()),
                            svf->getNumberOfColumns());
   for (int i = 0; i < num; i++) {
      const QString name(surfaceVectorLineEdits[i]->text());
      if (name != svf->getColumnName(i)) {
         svf->setColumnName(i, name);
      }
   }
   
   GuiFilesModified fm;
   fm.setSurfaceVectorModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * read the surface vector settings page.
 */
void 
GuiDisplayControlDialog::readSurfaceVectorSettingsPage()
{
   if (pageSurfaceVectorSettings == NULL) {
      return;
   }
   DisplaySettingsSurfaceVectors* dssv = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceVectors();
   
   dssv->setSelectedColumn(surfaceModelIndex, surfaceVectorButtonGroup->checkedId());
   dssv->setDisplayMode(static_cast<DisplaySettingsSurfaceVectors::DISPLAY_MODE>(
                          surfaceVectorDisplayModeComboBox->currentIndex()));
   dssv->setSparseDisplayDistance(surfaceVectorSparseDistanceSpinBox->value());
   dssv->setLengthMultiplier(surfaceVectorLengthMultiplierDoubleSpinBox->value());
   
   GuiBrainModelOpenGL::updateAllGL();
}
      
/**
 * called when surface vector comment button pressed.
 */
void 
GuiDisplayControlDialog::slotSurfaceVectorCommentPushButton(int item)
{
   SurfaceVectorFile* svf = theMainWindow->getBrainSet()->getSurfaceVectorFile();
   if ((item >= 0) && (item < svf->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    svf, 
                                                                    item);
      dfcd->show();
   }
}
      
/**
 * Create the misc tab page
 */
void
GuiDisplayControlDialog::createSurfaceMiscPage()
{
   //
   // Display settings for nodes
   //
   DisplaySettingsSurface* dsn = theMainWindow->getBrainSet()->getDisplaySettingsSurface();
   
   //
   // Widget for misc items
   //
   pageSurfaceMisc = new QWidget;
   pageWidgetStack->addWidget(pageSurfaceMisc); //, PAGE_NAME_SURFACE_MISC);
   QVBoxLayout* miscVBoxLayout = new QVBoxLayout(pageSurfaceMisc);
   miscVBoxLayout->setSpacing(3);
   
   //
   // Grid layout for brighness, contrast, node size, link size
   //
   //
   int numRows = 0;
   const int FIDUCIAL_ROW     = numRows++;
   const int DRAW_MODE_ROW    = numRows++;
   const int PARTIAL_VIEW_ROW = numRows++;
   const int PROJECTION_ROW   = numRows++;
   const int BRIGHTNESS_ROW = numRows++;
   const int CONTRAST_ROW   = numRows++;
   const int NODE_SIZE_ROW  = numRows++;
   const int LINK_SIZE_ROW  = numRows++;
   const int IDENTIFY_COLOR_ROW = numRows++;
   
   const int maxWidth = 250;
   
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->setSpacing(2);
   miscVBoxLayout->addLayout(gridLayout);
   //gridLayout->setColumnMaximumWidth(1, 200);
   
   //
   // Active fiducial surface selection
   //
   gridLayout->addWidget(new QLabel("Active Fiducial"), FIDUCIAL_ROW, 0, Qt::AlignLeft);
   miscActiveFiducialComboBox = new GuiBrainModelSelectionComboBox(false, true, false,
                                                               "", 0, 
                                                               "miscActiveFiducialComboBox",
                                                               false,
                                                               true);
   gridLayout->addWidget(miscActiveFiducialComboBox, FIDUCIAL_ROW, 1, 1, 2, Qt::AlignLeft);
   QObject::connect(miscActiveFiducialComboBox, SIGNAL(activated(int)),
                    this, SLOT(readMiscSelections()));
   
   //
   // Combo box for drawing mode
   //
   gridLayout->addWidget(new QLabel("Drawing Mode "), DRAW_MODE_ROW, 0, Qt::AlignLeft);
   miscDrawModeComboBox = new QComboBox;
   miscDrawModeComboBox->setMaximumWidth(maxWidth);
   miscDrawModeComboBox->insertItem(DisplaySettingsSurface::DRAW_MODE_NODES,
                                    "Nodes");
   miscDrawModeComboBox->insertItem(DisplaySettingsSurface::DRAW_MODE_LINKS,
                                    "Links");
   miscDrawModeComboBox->insertItem(DisplaySettingsSurface::DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL,
                                    "Links (No Backside)");
   miscDrawModeComboBox->insertItem(DisplaySettingsSurface::DRAW_MODE_LINKS_EDGES_ONLY,
                                    "Links (Edges Only)");
   miscDrawModeComboBox->insertItem(DisplaySettingsSurface::DRAW_MODE_NODES_AND_LINKS,
                                    "Nodes and Links");
   miscDrawModeComboBox->insertItem(DisplaySettingsSurface::DRAW_MODE_TILES,
                                    "Tiles without Lighting");
   miscDrawModeComboBox->insertItem(DisplaySettingsSurface::DRAW_MODE_TILES_WITH_LIGHT,
                                    "Tiles with Lighting (Default)");
   miscDrawModeComboBox->insertItem(DisplaySettingsSurface::DRAW_MODE_TILES_LINKS_NODES,
                                    "Tiles, Links, and Nodes (surface editing)");
   miscDrawModeComboBox->insertItem(DisplaySettingsSurface::DRAW_MODE_NONE,
                                    "Hide Surface");
   miscDrawModeComboBox->setFixedSize(miscDrawModeComboBox->sizeHint());
   gridLayout->addWidget(miscDrawModeComboBox, DRAW_MODE_ROW, 1, Qt::AlignLeft);
   miscDrawModeComboBox->setToolTip("Choose Drawing Mode");
   QObject::connect(miscDrawModeComboBox, SIGNAL(activated(int)),
                    this, SLOT(readMiscSelections()));
   
   //
   // Partial View 
   //
   gridLayout->addWidget(new QLabel("Partial View"), PARTIAL_VIEW_ROW, 0, Qt::AlignLeft);
   miscPartialViewComboBox = new QComboBox;
   miscPartialViewComboBox->setMaximumWidth(maxWidth);
   gridLayout->addWidget(miscPartialViewComboBox, PARTIAL_VIEW_ROW, 1, Qt::AlignLeft);
   QObject::connect(miscPartialViewComboBox, SIGNAL(activated(int)),
                    this, SLOT(readMiscSelections()));
   miscPartialViewComboBox->insertItem(DisplaySettingsSurface::PARTIAL_VIEW_ALL,
                                       "All");
   miscPartialViewComboBox->insertItem(DisplaySettingsSurface::PARTIAL_VIEW_POSITIVE_X,
                                       "Positive X");
   miscPartialViewComboBox->insertItem(DisplaySettingsSurface::PARTIAL_VIEW_NEGATIVE_X,
                                       "Negative X");
   miscPartialViewComboBox->insertItem(DisplaySettingsSurface::PARTIAL_VIEW_POSITIVE_Y,
                                       "Positive Y");
   miscPartialViewComboBox->insertItem(DisplaySettingsSurface::PARTIAL_VIEW_NEGATIVE_Y,
                                       "Negative Y");
   miscPartialViewComboBox->insertItem(DisplaySettingsSurface::PARTIAL_VIEW_POSITIVE_Z,
                                       "Positive Z");
   miscPartialViewComboBox->insertItem(DisplaySettingsSurface::PARTIAL_VIEW_NEGATIVE_Z,
                                       "Negative Z");
   
   //
   // Projection
   //
   gridLayout->addWidget(new QLabel("Projection"), PROJECTION_ROW, 0, Qt::AlignLeft);
   miscProjectionComboBox = new QComboBox;
   miscProjectionComboBox->setMaximumWidth(maxWidth);
   gridLayout->addWidget(miscProjectionComboBox, PROJECTION_ROW, 1, Qt::AlignLeft);
   QObject::connect(miscProjectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(readMiscSelections()));
   miscProjectionComboBox->insertItem(DisplaySettingsSurface::VIEWING_PROJECTION_ORTHOGRAPHIC,
                                      "Orthographic");
   miscProjectionComboBox->insertItem(DisplaySettingsSurface::VIEWING_PROJECTION_PERSPECTIVE,
                                      "Perspective");
                                      
   //
   // Brightness line edit and label
   //
   gridLayout->addWidget(new QLabel("Brightness"), BRIGHTNESS_ROW, 0, Qt::AlignLeft);
   miscBrightnessLineEdit = new QLineEdit;
   miscBrightnessLineEdit->setMaximumWidth(maxWidth);
   gridLayout->addWidget(miscBrightnessLineEdit, BRIGHTNESS_ROW, 1, Qt::AlignLeft);
   miscBrightnessLineEdit->setText(QString("%1").arg(dsn->getNodeBrightness(), 0, 'f', 3));
   QObject::connect(miscBrightnessLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(readMiscSelections()));
   
   //
   // Contrast line edit and label
   //
   gridLayout->addWidget(new QLabel("Contrast"), CONTRAST_ROW, 0, Qt::AlignLeft);
   miscContrastLineEdit = new QLineEdit;
   miscContrastLineEdit->setMaximumWidth(maxWidth);
   gridLayout->addWidget(miscContrastLineEdit, CONTRAST_ROW, 1, Qt::AlignLeft);
   miscContrastLineEdit->setText(QString("%1").arg(dsn->getNodeContrast(), 0, 'f', 3));
   QObject::connect(miscContrastLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(readMiscSelections()));
   
   //
   // Node Size spin box and label
   //
   gridLayout->addWidget(new QLabel("Node Size"), NODE_SIZE_ROW, 0, Qt::AlignLeft);
   miscNodeSizeSpinBox = new QDoubleSpinBox;
   miscNodeSizeSpinBox->setMinimum(minPointSize);
   miscNodeSizeSpinBox->setMaximum(maxPointSize);
   miscNodeSizeSpinBox->setSingleStep(1.0);
   miscNodeSizeSpinBox->setDecimals(1);
   miscNodeSizeSpinBox->setMaximumWidth(maxWidth);
   gridLayout->addWidget(miscNodeSizeSpinBox, NODE_SIZE_ROW, 1, Qt::AlignLeft);
   miscNodeSizeSpinBox->setValue(dsn->getNodeSize());
   QObject::connect(miscNodeSizeSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMiscSelections()));
   
   //
   // Link Size spin box and label
   //
   gridLayout->addWidget(new QLabel("Link Size"), LINK_SIZE_ROW, 0, Qt::AlignLeft);
   miscLinkSizeSpinBox = new QDoubleSpinBox;
   miscLinkSizeSpinBox->setMinimum(minLineSize);
   miscLinkSizeSpinBox->setMaximum(maxLineSize);
   miscLinkSizeSpinBox->setSingleStep(1.0);
   miscLinkSizeSpinBox->setDecimals(1);
   miscLinkSizeSpinBox->setMaximumWidth(maxWidth);
   gridLayout->addWidget(miscLinkSizeSpinBox, LINK_SIZE_ROW, 1, Qt::AlignLeft);
   miscLinkSizeSpinBox->setValue(dsn->getLinkSize());
   QObject::connect(miscLinkSizeSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMiscSelections()));
   
   //
   // identify node color combo box
   //
   gridLayout->addWidget(new QLabel("Identify Color"), IDENTIFY_COLOR_ROW, 0, Qt::AlignLeft);
   miscIdentifyNodeColorComboBox = new QComboBox;
   miscIdentifyNodeColorComboBox->setMaximumWidth(maxWidth);
   gridLayout->addWidget(miscIdentifyNodeColorComboBox, IDENTIFY_COLOR_ROW, 1, Qt::AlignLeft);
   QObject::connect(miscIdentifyNodeColorComboBox, SIGNAL(activated(int)),
                    this, SLOT(readMiscSelections()));
   miscIdentifyNodeColorComboBox->insertItem(DisplaySettingsSurface::IDENTIFY_NODE_COLOR_BLACK, "Black");
   miscIdentifyNodeColorComboBox->insertItem(DisplaySettingsSurface::IDENTIFY_NODE_COLOR_BLUE, "Blue");
   miscIdentifyNodeColorComboBox->insertItem(DisplaySettingsSurface::IDENTIFY_NODE_COLOR_GREEN, "Green");
   miscIdentifyNodeColorComboBox->insertItem(DisplaySettingsSurface::IDENTIFY_NODE_COLOR_RED, "Red");
   miscIdentifyNodeColorComboBox->insertItem(DisplaySettingsSurface::IDENTIFY_NODE_COLOR_WHITE, "White");

   //
   // show normals check box
   //
   miscShowNormalsCheckBox = new QCheckBox("Show Normals");
   miscVBoxLayout->addWidget(miscShowNormalsCheckBox, 0, Qt::AlignLeft);
   miscShowNormalsCheckBox->setChecked(dsn->getShowNormals());
   QObject::connect(miscShowNormalsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readMiscSelections()));
   
   //
   // total forces check box
   //
   miscTotalForcesCheckBox = new QCheckBox("Show Morphing Total Forces");
   miscVBoxLayout->addWidget(miscTotalForcesCheckBox, 0, Qt::AlignLeft);
   miscTotalForcesCheckBox->setChecked(dsn->getShowMorphingTotalForces());
   QObject::connect(miscTotalForcesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readMiscSelections()));
   
   //
   // angular forces check box
   //
   miscAngularForcesCheckBox = new QCheckBox("Show Morphing Angular Forces");
   miscVBoxLayout->addWidget(miscAngularForcesCheckBox, 0, Qt::AlignLeft);
   miscAngularForcesCheckBox->setChecked(dsn->getShowMorphingAngularForces());
   QObject::connect(miscAngularForcesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readMiscSelections()));
   
   //
   // linear forces check box
   //
   miscLinearForcesCheckBox = new QCheckBox("Show Morphing Linear Forces");
   miscVBoxLayout->addWidget(miscLinearForcesCheckBox, 0, Qt::AlignLeft);
   miscLinearForcesCheckBox->setChecked(dsn->getShowMorphingLinearForces());
   QObject::connect(miscLinearForcesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readMiscSelections()));
   
   //
   // force vector length float spin box
   //
   QLabel* forceVectorLabel = new QLabel("Force Vector Length ");
   miscForceVectorLengthDoubleSpinBox = new QDoubleSpinBox;
   miscForceVectorLengthDoubleSpinBox->setMinimum(0.0);
   miscForceVectorLengthDoubleSpinBox->setMaximum(5000.0);
   miscForceVectorLengthDoubleSpinBox->setSingleStep(10.0);
   miscForceVectorLengthDoubleSpinBox->setDecimals(2);
   miscForceVectorLengthDoubleSpinBox->setValue(dsn->getForceVectorDisplayLength());
   QObject::connect(miscForceVectorLengthDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMiscSelections()));
   QHBoxLayout* forceHBoxLayout = new QHBoxLayout;
   miscVBoxLayout->addLayout(forceHBoxLayout);
   forceHBoxLayout->addWidget(forceVectorLabel);
   forceHBoxLayout->addWidget(miscForceVectorLengthDoubleSpinBox);
   forceHBoxLayout->addStretch();
   
   //
   // cartesian axes show letters check box
   //
   miscAxesShowLettersCheckBox = new QCheckBox("Show Orientation Labels");
   QObject::connect(miscAxesShowLettersCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readMiscSelections()));

   //
   // cartesian axesShow tick marks
   //
   miscAxesShowTickMarksCheckBox = new QCheckBox("Show Tick Marks");
   QObject::connect(miscAxesShowTickMarksCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readMiscSelections()));

   //
   // axes length
   //
   QLabel* miscSurfAxesLabel = new QLabel("Axes Length ");
   miscAxesLengthDoubleSpinBox = new QDoubleSpinBox;
   miscAxesLengthDoubleSpinBox->setMinimum(0.0);
   miscAxesLengthDoubleSpinBox->setMaximum(50000.0);
   miscAxesLengthDoubleSpinBox->setSingleStep(10.0);
   miscAxesLengthDoubleSpinBox->setDecimals(1);
   QObject::connect(miscAxesLengthDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMiscSelections()));
   QHBoxLayout* miscSurfAxesLayout = new QHBoxLayout; 
   miscSurfAxesLayout->addWidget(miscSurfAxesLabel);
   miscSurfAxesLayout->addWidget(miscAxesLengthDoubleSpinBox);
   miscSurfAxesLayout->addStretch();
   
   //
   // Axes offset 
   //
   QLabel* miscSurfAxesOffsetLabel = new QLabel("Axes Offset ");
   miscAxesOffsetDoubleSpinBox[0] = new QDoubleSpinBox;
   miscAxesOffsetDoubleSpinBox[0]->setMinimum(-1000000.0);
   miscAxesOffsetDoubleSpinBox[0]->setMaximum( 1000000.0);
   miscAxesOffsetDoubleSpinBox[0]->setSingleStep(10.0);
   miscAxesOffsetDoubleSpinBox[0]->setDecimals(2);
   QObject::connect(miscAxesOffsetDoubleSpinBox[0], SIGNAL(valueChanged(double)),
                    this, SLOT(readMiscSelections()));
   miscAxesOffsetDoubleSpinBox[1] = new QDoubleSpinBox;
   miscAxesOffsetDoubleSpinBox[1]->setMinimum(-1000000.0);
   miscAxesOffsetDoubleSpinBox[1]->setMaximum( 1000000.0);
   miscAxesOffsetDoubleSpinBox[1]->setSingleStep(10.0);
   miscAxesOffsetDoubleSpinBox[1]->setDecimals(2);
   QObject::connect(miscAxesOffsetDoubleSpinBox[1], SIGNAL(valueChanged(double)),
                    this, SLOT(readMiscSelections()));
   miscAxesOffsetDoubleSpinBox[2] = new QDoubleSpinBox;
   miscAxesOffsetDoubleSpinBox[2]->setMinimum(-1000000.0);
   miscAxesOffsetDoubleSpinBox[2]->setMaximum( 1000000.0);
   miscAxesOffsetDoubleSpinBox[2]->setSingleStep(10.0);
   miscAxesOffsetDoubleSpinBox[2]->setDecimals(2);
   QObject::connect(miscAxesOffsetDoubleSpinBox[2], SIGNAL(valueChanged(double)),
                    this, SLOT(readMiscSelections()));
   QHBoxLayout* miscSurfAxesOffsetLayout = new QHBoxLayout;
   miscSurfAxesOffsetLayout->addWidget(miscSurfAxesOffsetLabel);
   miscSurfAxesOffsetLayout->addWidget(miscAxesOffsetDoubleSpinBox[0]);
   miscSurfAxesOffsetLayout->addWidget(miscAxesOffsetDoubleSpinBox[1]);
   miscSurfAxesOffsetLayout->addWidget(miscAxesOffsetDoubleSpinBox[2]);
   miscSurfAxesOffsetLayout->addStretch();
   
   //
   // Surface axes group box
   //
   miscAxesGroupBox = new QGroupBox("Surface Cartesian Axes");
   miscAxesGroupBox->setCheckable(true);
   miscVBoxLayout->addWidget(new QLabel(" "));
   miscVBoxLayout->addWidget(miscAxesGroupBox);
   QVBoxLayout* miscAxesGroupLayout = new QVBoxLayout(miscAxesGroupBox);
   miscAxesGroupLayout->addWidget(miscAxesShowLettersCheckBox);
   miscAxesGroupLayout->addWidget(miscAxesShowTickMarksCheckBox);
   miscAxesGroupLayout->addLayout(miscSurfAxesLayout);
   miscAxesGroupLayout->addLayout(miscSurfAxesOffsetLayout);
   QObject::connect(miscAxesGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(readMiscSelections()));
   miscAxesGroupBox->setFixedSize(miscAxesGroupBox->sizeHint());
   
   //
   // Limit size of page
   //
   pageSurfaceMisc->setFixedSize(pageSurfaceMisc->sizeHint());
}

/**
 * Update the items on the misc page
 */
void
GuiDisplayControlDialog::updateMiscItems()
{
   updatePageSelectionComboBox();
   if (pageSurfaceMisc == NULL) {
      return;
   }
   
   //
   // Display settings for nodes
   //
   DisplaySettingsSurface* dsn = theMainWindow->getBrainSet()->getDisplaySettingsSurface();
 
   miscActiveFiducialComboBox->updateComboBox();
   miscActiveFiducialComboBox->setSelectedBrainModel(theMainWindow->getBrainSet()->getActiveFiducialSurface());
   miscDrawModeComboBox->setCurrentIndex(dsn->getDrawMode());
   miscPartialViewComboBox->setCurrentIndex(dsn->getPartialView());
   miscProjectionComboBox->setCurrentIndex(dsn->getViewingProjection());
   miscBrightnessLineEdit->setText(QString::number(dsn->getNodeBrightness(), 'f', 2));
   miscContrastLineEdit->setText(QString::number(dsn->getNodeContrast(), 'f', 2));
   miscNodeSizeSpinBox->setValue(dsn->getNodeSize());
   miscLinkSizeSpinBox->setValue(dsn->getLinkSize());
   miscShowNormalsCheckBox->setChecked(dsn->getShowNormals());
   miscTotalForcesCheckBox->setChecked(dsn->getShowMorphingTotalForces());
   miscAngularForcesCheckBox->setChecked(dsn->getShowMorphingAngularForces());
   miscLinearForcesCheckBox->setChecked(dsn->getShowMorphingLinearForces());
   miscForceVectorLengthDoubleSpinBox->setValue(dsn->getForceVectorDisplayLength());
   miscIdentifyNodeColorComboBox->setCurrentIndex(dsn->getIdentifyNodeColor());
   bool showAxes, showLetters, showHashMarks;
   float axesLength, offset[3];
   dsn->getSurfaceAxesInfo(showAxes, showLetters, showHashMarks, axesLength, offset);
   miscAxesGroupBox->setChecked(showAxes);
   miscAxesShowLettersCheckBox->setChecked(showLetters);
   miscAxesShowTickMarksCheckBox->setChecked(showHashMarks);
   miscAxesLengthDoubleSpinBox->setValue(axesLength);
   miscAxesOffsetDoubleSpinBox[0]->setValue(offset[0]);
   miscAxesOffsetDoubleSpinBox[1]->setValue(offset[1]);
   miscAxesOffsetDoubleSpinBox[2]->setValue(offset[2]);
}

/**
 * Read the selections on the misc page
 */
void
GuiDisplayControlDialog::readMiscSelections()
{
   if (creatingDialog) {
      return;
   }
   
   //
   // Update active fiducial surface
   //
   BrainModelSurface* active = miscActiveFiducialComboBox->getSelectedBrainModelSurface();
   if (active != NULL) {
      BrainModelSurface* currentActive = theMainWindow->getBrainSet()->getActiveFiducialSurface();
      if (currentActive != active) {
         theMainWindow->getBrainSet()->setActiveFiducialSurface(active);
      }
   }
   
   //
   // Display settings for nodes
   //
   DisplaySettingsSurface* dsn = theMainWindow->getBrainSet()->getDisplaySettingsSurface();
   
   dsn->setDrawMode(
      static_cast<DisplaySettingsSurface::DRAW_MODE>(miscDrawModeComboBox->currentIndex()));
   dsn->setPartialView(static_cast<DisplaySettingsSurface::PARTIAL_VIEW_TYPE>(
                       miscPartialViewComboBox->currentIndex()));
   dsn->setViewingProjection(static_cast<DisplaySettingsSurface::VIEWING_PROJECTION>(
                       miscProjectionComboBox->currentIndex()));
   dsn->setNodeBrightness(miscBrightnessLineEdit->text().toFloat());
   dsn->setNodeContrast(miscContrastLineEdit->text().toFloat());
   dsn->setNodeSize(miscNodeSizeSpinBox->value());
   dsn->setLinkSize(miscLinkSizeSpinBox->value());
   dsn->setShowNormals(miscShowNormalsCheckBox->isChecked());
   dsn->setShowMorphingTotalForces(miscTotalForcesCheckBox->isChecked());
   dsn->setShowMorphingAngularForces(miscAngularForcesCheckBox->isChecked());
   dsn->setShowMorphingLinearForces(miscLinearForcesCheckBox->isChecked());
   dsn->setForceVectorDisplayLength(miscForceVectorLengthDoubleSpinBox->value());
   dsn->setIdentifyNodeColor(static_cast<DisplaySettingsSurface::IDENTIFY_NODE_COLOR>(
                                       miscIdentifyNodeColorComboBox->currentIndex()));
                                       
   const float offset[3] = {
      miscAxesOffsetDoubleSpinBox[0]->value(),
      miscAxesOffsetDoubleSpinBox[1]->value(),
      miscAxesOffsetDoubleSpinBox[2]->value()
   };
   dsn->setSurfaceAxesInfo(miscAxesGroupBox->isChecked(),
                           miscAxesShowLettersCheckBox->isChecked(),
                           miscAxesShowTickMarksCheckBox->isChecked(),
                           miscAxesLengthDoubleSpinBox->value(),
                           offset);

   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
}

/**
 * Create the probabilistic atlas surface main page
 */
void
GuiDisplayControlDialog::createProbAtlasSurfaceMainPage()
{
   //
   // Normal/Threshold buttons
   //
   probAtlasSurfaceNormalButton = new QRadioButton("Normal");
   probAtlasSurfaceThresholdButton = new QRadioButton("Threshold");

   //
   // Button group for probabilistic display type
   //
   QButtonGroup* probButtonGroup = new QButtonGroup(this);
   QObject::connect(probButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(probAtlasSurfaceModeSelection(int)));
   probButtonGroup->addButton(probAtlasSurfaceNormalButton, 0);
   probButtonGroup->addButton(probAtlasSurfaceThresholdButton, 1);

   //
   // Group box for display mode
   //
   QGroupBox* displayModeGroupBox = new QGroupBox("Display Mode");
   QVBoxLayout* displayModeGroupLayout = new QVBoxLayout(displayModeGroupBox);
   displayModeGroupLayout->addWidget(probAtlasSurfaceNormalButton);
   displayModeGroupLayout->addWidget(probAtlasSurfaceThresholdButton);
   displayModeGroupBox->setFixedSize(displayModeGroupBox->sizeHint());
   
   //
   // Treat button
   //
   probAtlasSurfaceUnassignedButton = new QCheckBox("Treat name \"???\" as if it was name \"Unassigned\"");
   QObject::connect(probAtlasSurfaceUnassignedButton, SIGNAL(clicked()),
                    this, SLOT(readProbAtlasSurfaceMainPage()));
   //
   // Threshold ratio
   //
   QLabel* ratioLabel = new QLabel("Threshold Ratio  ");
   probAtlasSurfaceThresholdRatioDoubleSpinBox = new QDoubleSpinBox;
   probAtlasSurfaceThresholdRatioDoubleSpinBox->setMinimum(0.0);
   probAtlasSurfaceThresholdRatioDoubleSpinBox->setMaximum(1.0);
   probAtlasSurfaceThresholdRatioDoubleSpinBox->setSingleStep(0.1);
   probAtlasSurfaceThresholdRatioDoubleSpinBox->setDecimals(2);
   QObject::connect(probAtlasSurfaceThresholdRatioDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readProbAtlasSurfaceMainPage()));
   QHBoxLayout* ratioLayout = new QHBoxLayout;
   ratioLayout->addWidget(ratioLabel);
   ratioLayout->addWidget(probAtlasSurfaceThresholdRatioDoubleSpinBox);
   ratioLayout->addStretch();
   
   QLabel* questLabel = new QLabel("\nNote: Names \"???\", \"GYRAL\", and \"GYRUS\" are\n"
                                   "ignored when Threshold Display Mode is selected.");
              
                    
   //
   // Display color key button
   //
   QPushButton* colorKeyPushButton = new QPushButton("Display Color Key...");
   colorKeyPushButton->setFixedSize(colorKeyPushButton->sizeHint());
   colorKeyPushButton->setAutoDefault(false);
   QObject::connect(colorKeyPushButton, SIGNAL(clicked()),
                    theMainWindow, SLOT(displayProbabilisticAtlasColorKey()));
                    
   //
   // Set study metadata link button
   //
   QPushButton* studyMetaDataLinkPushButton = new QPushButton("Study Metadata Link...");
   studyMetaDataLinkPushButton->setFixedSize(studyMetaDataLinkPushButton->sizeHint());
   studyMetaDataLinkPushButton->setAutoDefault(false);
   QObject::connect(studyMetaDataLinkPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeProbAtlasSurfaceStudyMetaDataPushButton()));
   
   //
   // Widget and layout
   //
   pageProbAtlasSurfaceMain = new QWidget;
   QVBoxLayout* probAtlasSurfaceLayout = new QVBoxLayout(pageProbAtlasSurfaceMain);
   probAtlasSurfaceLayout->addWidget(displayModeGroupBox);
   probAtlasSurfaceLayout->addWidget(probAtlasSurfaceUnassignedButton);
   probAtlasSurfaceLayout->addLayout(ratioLayout);
   probAtlasSurfaceLayout->addWidget(questLabel);
   probAtlasSurfaceLayout->addWidget(colorKeyPushButton);
   probAtlasSurfaceLayout->addWidget(studyMetaDataLinkPushButton);
   probAtlasSurfaceLayout->addStretch();

   pageWidgetStack->addWidget(pageProbAtlasSurfaceMain);
}

/**
 * called to set prob atlas surface study metadata link.
 */
void 
GuiDisplayControlDialog::volumeProbAtlasSurfaceStudyMetaDataPushButton()
{
   ProbabilisticAtlasFile* paf = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   StudyMetaDataLinkSet smdls = paf->getStudyMetaDataLinkSet();
   GuiStudyMetaDataLinkCreationDialog smdlcd(this);
   smdlcd.initializeSelectedLinkSet(smdls);
   if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
      smdls = smdlcd.getLinkSetCreated();
      paf->setStudyMetaDataLinkSet(smdls);
   }
}
      
/**
 * Slot for prob atlas mode selection.
 */
void
GuiDisplayControlDialog::probAtlasSurfaceModeSelection(int num)
{
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   dspa->setDisplayType(
      static_cast<DisplaySettingsProbabilisticAtlas::PROBABILISTIC_DISPLAY_TYPE>(num));
   readProbAtlasSurfaceMainPage();
}

/**
 * Create prob atlas surface channel selection page
 */
void
GuiDisplayControlDialog::createProbAtlasSurfaceChannelPage()
{
   //
   // widget and layout for chanel sub page
   //
   pageProbAtlasSurfaceChannel = new QWidget;
   pageProbAtlasSurfaceChannel->setFixedWidth(450);
   probAtlasSurfaceSubPageChannelLayout = new QVBoxLayout(pageProbAtlasSurfaceChannel);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* probAtlasSurfaceChannelAllOnButton = new QPushButton("All On");
   probAtlasSurfaceChannelAllOnButton->setAutoDefault(false);
   QObject::connect(probAtlasSurfaceChannelAllOnButton, SIGNAL(clicked()),
                    this, SLOT(probAtlasSurfaceChannelAllOn()));
   QPushButton* probAtlasSurfaceChannelAllOffButton = new QPushButton("All Off");
   probAtlasSurfaceChannelAllOffButton->setAutoDefault(false);
   QObject::connect(probAtlasSurfaceChannelAllOffButton, SIGNAL(clicked()),
                    this, SLOT(probAtlasSurfaceChannelAllOff()));
   QHBoxLayout* allOnOffButtonsLayout = new QHBoxLayout;         
   allOnOffButtonsLayout->addWidget(probAtlasSurfaceChannelAllOnButton);
   allOnOffButtonsLayout->addWidget(probAtlasSurfaceChannelAllOffButton);
   allOnOffButtonsLayout->addStretch();
   probAtlasSurfaceSubPageChannelLayout->addLayout(allOnOffButtonsLayout);
   
   createAndUpdateProbAtlasSurfaceChannelCheckBoxes();

   pageWidgetStack->addWidget(pageProbAtlasSurfaceChannel);
}

/**
 * This slot is called when the prob atlas surface channel all on button is pressed
 */
void
GuiDisplayControlDialog::probAtlasSurfaceChannelAllOn()
{
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   dspa->setAllChannelsSelectedStatus(true);
   updateProbAtlasSurfaceItems();
   readProbAtlasSurfaceChannelPage();
}

/**
 * This slot is called when the prob atlas surfacechannel all off button is pressed
 */
void
GuiDisplayControlDialog::probAtlasSurfaceChannelAllOff()
{
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   dspa->setAllChannelsSelectedStatus(false);
   updateProbAtlasSurfaceItems();
   readProbAtlasSurfaceChannelPage();
}

/**
 * caret prob atlas surface area selection page
 */
void
GuiDisplayControlDialog::createProbAtlasSurfaceAreaPage()
{
   //
   // Vertical Box Layout for all items
   //
   pageProbAtlasSurfaceArea = new QWidget;
   pageProbAtlasSurfaceArea->setFixedWidth(450);
   probAtlasSurfaceSubPageAreaLayout = new QVBoxLayout(pageProbAtlasSurfaceArea);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* probAtlasSurfaceAreasAllOnButton = new QPushButton("All On");
   probAtlasSurfaceAreasAllOnButton->setAutoDefault(false);
   QObject::connect(probAtlasSurfaceAreasAllOnButton, SIGNAL(clicked()),
                    this, SLOT(probAtlasSurfaceAreasAllOn()));
   QPushButton* probAtlasSurfaceAreasAllOffButton = new QPushButton("All Off");
   probAtlasSurfaceAreasAllOffButton->setAutoDefault(false);
   QObject::connect(probAtlasSurfaceAreasAllOffButton, SIGNAL(clicked()),
                    this, SLOT(probAtlasSurfaceAreasAllOff()));
   QHBoxLayout* allOnOffButtonsLayout = new QHBoxLayout;
   allOnOffButtonsLayout->addWidget(probAtlasSurfaceAreasAllOnButton);
   allOnOffButtonsLayout->addWidget(probAtlasSurfaceAreasAllOffButton);
   allOnOffButtonsLayout->addStretch();
   probAtlasSurfaceSubPageAreaLayout->addLayout(allOnOffButtonsLayout);
   
   createAndUpdateProbAtlasSurfaceAreaNameCheckBoxes();

   pageWidgetStack->addWidget(pageProbAtlasSurfaceArea);
}

/**
 * This slot is called when the prob atlas surfce areas all on button is pressed
 */
void
GuiDisplayControlDialog::probAtlasSurfaceAreasAllOn()
{
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   dspa->setAllAreasSelectedStatus(true);
   updateProbAtlasSurfaceItems();
   readProbAtlasSurfaceAreaPage();
}

/**
 * This slot is called when the prob atlas surface areas all off button is pressed
 */
void
GuiDisplayControlDialog::probAtlasSurfaceAreasAllOff()
{
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   dspa->setAllAreasSelectedStatus(false);
   updateProbAtlasSurfaceItems();
   readProbAtlasSurfaceAreaPage();
}

/**
 * read the probabilistic atlas surface main page.
 */
void 
GuiDisplayControlDialog::readProbAtlasSurfaceMainPage()
{
   if (pageProbAtlasSurfaceMain == NULL) {
      return;
   }
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   dspa->setTreatQuestColorAsUnassigned(probAtlasSurfaceUnassignedButton->isChecked());
   dspa->setThresholdDisplayTypeRatio(probAtlasSurfaceThresholdRatioDoubleSpinBox->value());  
    
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
}

/**
 * read the probabilistic atlas surface channel page.
 */
void 
GuiDisplayControlDialog::readProbAtlasSurfaceChannelPage()
{
   if (pageProbAtlasSurfaceChannel == NULL) {
      return;
   }
   ProbabilisticAtlasFile* paf = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   const int numChannels = paf->getNumberOfColumns();
   if (numChannels == numValidProbAtlasSurfaceChannels) {
      for (int i = 0; i < numChannels; i++)  {
         dspa->setChannelSelected(i, probAtlasSurfaceChannelCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of prob atlas surfce channel checkboxes does not equal number of "
                << "prob atlas channels." << std::endl;
   }
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
}

/**
 * read the probabilistic atlas surface area page.
 */
void 
GuiDisplayControlDialog::readProbAtlasSurfaceAreaPage()
{
   if (pageProbAtlasSurfaceArea == NULL) {
      return;
   }
   ProbabilisticAtlasFile* paf = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   const int numAreas = paf->getNumberOfPaintNames();
   if (numAreas == numValidProbAtlasSurfaceAreas) {
      for (int i = 0; i < numAreas; i++) {
         dspa->setAreaSelected(i, probAtlasSurfaceAreasCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of prob atlas surface area checkboxes does not equal number of "
                << "prob atlas surface areas." << std::endl;
   }
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
}
      
/**
 * Read the prob atlas surface selections.
 */
void 
GuiDisplayControlDialog::readProbAtlasSurfaceL2LR2R()
{
   if (creatingDialog) {
      return;
   }
   
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   dspa->setApplySelectionToLeftAndRightStructuresFlag(probAtlasSurfaceApplySelectionToLeftAndRightStructuresFlagCheckBox->isChecked());
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
}

/**
 * create and update the check boxes for prob atlas surface channels
 */
void
GuiDisplayControlDialog::createAndUpdateProbAtlasSurfaceChannelCheckBoxes()
{
   ProbabilisticAtlasFile* paf = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   numValidProbAtlasSurfaceChannels = paf->getNumberOfColumns();
   
   const int numExistingCheckBoxes = static_cast<int>(probAtlasSurfaceChannelCheckBoxes.size());
   
   if (probAtlasSurfaceChannelGridLayout == NULL) {
      //
      // Create the page and layout
      //
      QWidget* channelPage = new QWidget;
      probAtlasSurfaceChannelGridLayout = new QGridLayout(channelPage);
      const int rowStretchNumber    = 15000;
      probAtlasSurfaceChannelGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, 1, 1, Qt::AlignLeft);
      probAtlasSurfaceChannelGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      probAtlasSurfaceSubPageChannelLayout->addWidget(channelPage);
   }
   
   if (probAtlasSurfaceChannelButtonGroup == NULL) {
      probAtlasSurfaceChannelButtonGroup = new QButtonGroup(this);
      probAtlasSurfaceChannelButtonGroup->setExclusive(false);
      QObject::connect(probAtlasSurfaceChannelButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readProbAtlasSurfaceChannelPage()));
   }
   
   //
   // update existing checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidProbAtlasSurfaceChannels) {
         probAtlasSurfaceChannelCheckBoxes[i]->setText(paf->getColumnName(i));
         probAtlasSurfaceChannelCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidProbAtlasSurfaceChannels; j++) {
      QCheckBox* cb = new QCheckBox(paf->getColumnName(j));
      probAtlasSurfaceChannelCheckBoxes.push_back(cb);
      probAtlasSurfaceChannelButtonGroup->addButton(cb, j);
      probAtlasSurfaceChannelGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
   }
   
   //
   // Hide existing checkboxes that are not needed
   //
   for (int k = numValidProbAtlasSurfaceChannels; k < numExistingCheckBoxes; k++) {
      probAtlasSurfaceChannelCheckBoxes[k]->hide();
   }
}

/**
 * create and update the check boxes for prob atlas surface area names
 */
void
GuiDisplayControlDialog::createAndUpdateProbAtlasSurfaceAreaNameCheckBoxes()
{
   ProbabilisticAtlasFile* paf = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   numValidProbAtlasSurfaceAreas = paf->getNumberOfPaintNames();
   
   const int numExistingCheckBoxes = static_cast<int>(probAtlasSurfaceAreasCheckBoxes.size());
   
   if (probAtlasSurfaceAreasGridLayout == NULL) {
      //
      QWidget* areasWidget = new QWidget; 
      probAtlasSurfaceAreasGridLayout = new QGridLayout(areasWidget);
      const int rowStretchNumber    = 15000;
      probAtlasSurfaceAreasGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, 1, 1, Qt::AlignLeft);
      probAtlasSurfaceAreasGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      probAtlasSurfaceSubPageAreaLayout->addWidget(areasWidget);
   }
   
   if (probAtlasSurfaceAreasButtonGroup == NULL) {
      probAtlasSurfaceAreasButtonGroup = new QButtonGroup(this);
      probAtlasSurfaceAreasButtonGroup->setExclusive(false);
      QObject::connect(probAtlasSurfaceAreasButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readProbAtlasSurfaceAreaPage()));
   }
   
   //
   // update existing checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidProbAtlasSurfaceAreas) {
         probAtlasSurfaceAreasCheckBoxes[i]->setText(paf->getPaintNameFromIndex(i));
         probAtlasSurfaceAreasCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidProbAtlasSurfaceAreas; j++) {
      QCheckBox* cb = new QCheckBox(paf->getPaintNameFromIndex(j));
      probAtlasSurfaceAreasCheckBoxes.push_back(cb);
      probAtlasSurfaceAreasButtonGroup->addButton(cb, j);
      probAtlasSurfaceAreasGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
   }
   
   //
   // Hide existing checkboxes that are not needed
   //
   for (int k = numValidProbAtlasSurfaceAreas; k < numExistingCheckBoxes; k++) {
      probAtlasSurfaceAreasCheckBoxes[k]->hide();
   }
}

/**
 * update prob atlas surface main page.
 */
void 
GuiDisplayControlDialog::updateProbAtlasSurfaceMainPage()
{
   if (pageProbAtlasSurfaceMain == NULL) {
      return;
   }
   
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   probAtlasSurfaceUnassignedButton->setChecked(dspa->getTreatQuestColorAsUnassigned());
   
   switch(dspa->getDisplayType()) {
      case DisplaySettingsProbabilisticAtlas::PROBABILISTIC_DISPLAY_TYPE_NORMAL:
         probAtlasSurfaceNormalButton->setChecked(true);
         break;
      case DisplaySettingsProbabilisticAtlas::PROBABILISTIC_DISPLAY_TYPE_THRESHOLD:
         probAtlasSurfaceThresholdButton->setChecked(true);
         break;
   }
   probAtlasSurfaceThresholdRatioDoubleSpinBox->setValue(dspa->getThresholdDisplayTypeRatio());
   pageProbAtlasSurfaceMain->setEnabled(validProbAtlasSurfaceData);
}

/**
 * update prob atlas surface channel page.
 */
void 
GuiDisplayControlDialog::updateProbAtlasSurfaceChannelPage(const bool filesWereChanged)
{
   if (pageProbAtlasSurfaceChannel == NULL) {
      return;
   }

   ProbabilisticAtlasFile* paf = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();

   if (filesWereChanged) {
      createAndUpdateProbAtlasSurfaceChannelCheckBoxes();
   }
   const int numChannels = paf->getNumberOfColumns();
   if (numChannels == numValidProbAtlasSurfaceChannels) {
      for (int i = 0; i < numValidProbAtlasSurfaceChannels; i++) {
         probAtlasSurfaceChannelCheckBoxes[i]->setChecked(dspa->getChannelSelected(i));
      }
   }
   else {
      std::cerr << "Number of prob atlas surface channel checkboxes " << numValidProbAtlasSurfaceChannels
                << " does not equal number of prob "
                << "atlas surface channels" << numChannels << "." << std::endl;
   }
   pageProbAtlasSurfaceChannel->setEnabled(validProbAtlasSurfaceData);
}

/**
 * update prob atlas surface area page.
 */
void 
GuiDisplayControlDialog::updateProbAtlasSurfaceAreaPage(const bool filesWereChanged)
{
   if (pageProbAtlasSurfaceArea == NULL) {
      return;
   }
   
   ProbabilisticAtlasFile* paf = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();

   if (filesWereChanged) {
      createAndUpdateProbAtlasSurfaceAreaNameCheckBoxes();
   }
   const int numAreas = paf->getNumberOfPaintNames();
   if (numAreas == numValidProbAtlasSurfaceAreas) {
      for (int i = 0; i < numValidProbAtlasSurfaceAreas; i++) {
         probAtlasSurfaceAreasCheckBoxes[i]->setChecked(dspa->getAreaSelected(i));
      }
   }
   else {
      std::cerr << "Number of prob atlas surface area checkboxes " << numValidProbAtlasSurfaceAreas
                << " does not equal number of prob "
                << "atlas surface areas" << numAreas << "." << std::endl;
   }
   pageProbAtlasSurfaceArea->setEnabled(validProbAtlasSurfaceData);
}

/**
 * update prob atlas surface overlay/underlay selection.
 */
void 
GuiDisplayControlDialog::updateProbAtlasSurfaceOverlayUnderlaySelection()
{
   if (pageOverlayUnderlaySurface != NULL) {
      probAtlasSurfaceSelectionLabel->setEnabled(validProbAtlasSurfaceData);
      primaryOverlayProbAtlasSurfaceButton->setEnabled(validProbAtlasSurfaceData);
      secondaryOverlayProbAtlasSurfaceButton->setEnabled(validProbAtlasSurfaceData);
      underlayProbAtlasSurfaceButton->setEnabled(validProbAtlasSurfaceData);
      probAtlasSurfaceSelectionLabel->setEnabled(validProbAtlasSurfaceData);
      probAtlasSurfaceInfoPushButton->setEnabled(validProbAtlasSurfaceData);
   }   
}

/**
 * Update prob atlas surface items in dialog
 */
void 
GuiDisplayControlDialog::updateProbAtlasSurfaceItems(const bool filesChanged)
{   
   updatePageSelectionComboBox();

   updateProbAtlasSurfaceOverlayUnderlaySelection();

   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   
   probAtlasSurfaceApplySelectionToLeftAndRightStructuresFlagCheckBox->setChecked(dspa->getApplySelectionToLeftAndRightStructuresFlag());
   
   updateProbAtlasSurfaceMainPage();
   updateProbAtlasSurfaceChannelPage(filesChanged);
   updateProbAtlasSurfaceAreaPage(filesChanged);
}

/**
 * Create the topography page.
 */
void
GuiDisplayControlDialog::createTopographyPage()
{
  
   //
   // Combo box for file selection
   //
   topographyFileComboBox = new QComboBox;
   topographyFileComboBox->setToolTip("Choose Topography Column");
   QObject::connect(topographyFileComboBox, SIGNAL(activated(int)),
                    this, SLOT(topographyFileSelection(int)));
   
   //
   // Eccentricity/Polar Angle.
   //
   topographyTypeEccentricityRadioButton = new QRadioButton("Eccentricity");
   topographyPolarAngleRadioButton = new QRadioButton("Polar Angle");
   
   //
   // Button group for eccentricity/polar angle buttons
   //
   QButtonGroup* topographyButtonGroup = new QButtonGroup(this);
   QObject::connect(topographyButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(topographyTypeSelection(int)));
   topographyButtonGroup->addButton(topographyTypeEccentricityRadioButton);
   topographyButtonGroup->addButton(topographyPolarAngleRadioButton);
   
   //
   //  Group box for display mode
   //
   QGroupBox* topographyGroup = new QGroupBox("Display Mode");
   QVBoxLayout* topographyGroupLayout = new QVBoxLayout(topographyGroup);
   topographyGroupLayout->addWidget(topographyTypeEccentricityRadioButton);
   topographyGroupLayout->addWidget(topographyPolarAngleRadioButton);
   topographyGroup->setFixedSize(topographyGroup->sizeHint());
   
   //
   // Topography page and layout
   //
   pageTopography = new QWidget;
   pageWidgetStack->addWidget(pageTopography); // adds to dialog
   QVBoxLayout* topographyLayout = new QVBoxLayout(pageTopography);
   topographyLayout->addWidget(topographyFileComboBox);
   topographyLayout->addWidget(topographyGroup);
   topographyLayout->addStretch();
}

/**
 * Read the topography selections.
 */
void 
GuiDisplayControlDialog::readTopographySelections()
{
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * update topography overlay/underlay selection.
 */
void 
GuiDisplayControlDialog::updateTopographyOverlayUnderlaySelection()
{
   if (pageOverlayUnderlaySurface == NULL) {
      return;
   }
   
   TopographyFile* tf = theMainWindow->getBrainSet()->getTopographyFile();
   const int numCols = tf->getNumberOfColumns();
   DisplaySettingsTopography* dst = theMainWindow->getBrainSet()->getDisplaySettingsTopography();
   
   topographySelectionComboBox->clear();
   
   bool valid = false;
   if (numCols > 0) {
      valid = true;
      for (int i = 0; i < numCols; i++) {
         QString name(QString("Column %1").arg(i + 1));
         topographySelectionComboBox->addItem(name);
      }
      topographySelectionComboBox->setCurrentIndex(dst->getSelectedColumn(surfaceModelIndex));
   }
   
   topographySelectionComboBox->setEnabled(validTopographyData);
   primaryOverlayTopographyButton->setEnabled(validTopographyData);
   secondaryOverlayTopographyButton->setEnabled(validTopographyData);
   underlayTopographyButton->setEnabled(validTopographyData);
   topographySelectionLabel->setEnabled(validTopographyData);
}

/**
 * Update topography items in dialog
 */
void 
GuiDisplayControlDialog::updateTopographyItems()
{
   updatePageSelectionComboBox();
   updateTopographyOverlayUnderlaySelection();

   if (pageTopography == NULL) {
      return;
   }
   
   TopographyFile* tf = theMainWindow->getBrainSet()->getTopographyFile();
   const int numCols = tf->getNumberOfColumns();
   DisplaySettingsTopography* dst = theMainWindow->getBrainSet()->getDisplaySettingsTopography();
   
   topographyFileComboBox->clear();
   
   bool valid = false;
   if (numCols > 0) {
      valid = true;
      for (int i = 0; i < numCols; i++) {
         QString name(QString("Column %1").arg(i + 1));
         topographyFileComboBox->addItem(name);
      }
      topographyFileComboBox->setCurrentIndex(dst->getSelectedColumn(surfaceModelIndex));
      
      topographyFileComboBox->setToolTip(
                    tf->getColumnName(dst->getSelectedColumn(surfaceModelIndex)));
   }
   
   switch(dst->getDisplayType()) {
      case DisplaySettingsTopography::TOPOGRAPHY_DISPLAY_ECCENTRICITY:
         topographyTypeEccentricityRadioButton->setChecked(true);
         break;
      case DisplaySettingsTopography::TOPOGRAPHY_DISPLAY_POLAR_ANGLE:
         topographyPolarAngleRadioButton->setChecked(true);
         break;
   }
   
   topographyFileComboBox->setEnabled(validTopographyData);
   
   pageTopography->setEnabled(validTopographyData);
}

void
GuiDisplayControlDialog::topographyFileSelection(int fileNumber)
{
   DisplaySettingsTopography* dst = theMainWindow->getBrainSet()->getDisplaySettingsTopography();
   dst->setSelectedColumn(surfaceModelIndex, fileNumber);
   
   readTopographySelections();   
}

void
GuiDisplayControlDialog::topographyTypeSelection(int typeSelected)
{
   DisplaySettingsTopography* dst = theMainWindow->getBrainSet()->getDisplaySettingsTopography();
   dst->setDisplayType(static_cast<DisplaySettingsTopography::TOPOGRAPHY_DISPLAY_TYPE>(
                                                                           typeSelected));
   
   readTopographySelections();   
}

/**
 * create the geodesic page.
 */
void 
GuiDisplayControlDialog::createGeodesicPage()
{
   //
   // Widget for geodesic items
   //
   pageGeodesicMain = new QWidget;
   pageWidgetStack->addWidget(pageGeodesicMain);  // adds to dialog
   geodesicMainPageLayout = new QVBoxLayout(pageGeodesicMain);
   
   //
   // Enable geodesic check box
   //
   geodesicDistanceEnableCheckBox = new QCheckBox("Enable Geodesic Path For Identified Nodes");
   QObject::connect(geodesicDistanceEnableCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readGeodesicSelections()));
   geodesicMainPageLayout->addWidget(geodesicDistanceEnableCheckBox);
   
   //
   // Show root node check box
   //
   geodesicShowRootNodeCheckBox = new QCheckBox("Show Root Node");
   QObject::connect(geodesicShowRootNodeCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readGeodesicSelections()));
   geodesicMainPageLayout->addWidget(geodesicShowRootNodeCheckBox);
                    
   //
   // Path width spin box
   //
   QLabel* pathLabel = new QLabel("Path Line Width ");
   float minWF, maxWF;
   GuiBrainModelOpenGL::getLineWidthRange(minWF, maxWF);
   const int minW = static_cast<int>(minWF);
   const int maxW = static_cast<int>(maxWF);
   geodesicPathLineWidthSpinBox = new QSpinBox;
   geodesicPathLineWidthSpinBox->setMinimum(minW);
   geodesicPathLineWidthSpinBox->setMaximum(maxW);
   geodesicPathLineWidthSpinBox->setSingleStep(1);
   QObject::connect(geodesicPathLineWidthSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(readGeodesicSelections()));
   QHBoxLayout* pathLayout = new QHBoxLayout;
   pathLayout->addWidget(pathLabel);
   pathLayout->addWidget(geodesicPathLineWidthSpinBox);
   pathLayout->addStretch();
   geodesicMainPageLayout->addLayout(pathLayout);
   
   
   //
   // Selection button group
   //
   geodesicSelectionButtonGroup = new QButtonGroup(this);
   geodesicSelectionButtonGroup->setExclusive(true);
   QObject::connect(geodesicSelectionButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(readGeodesicSelections()));
                    
   //
   // comment button group
   //
   geodesicCommentButtonGroup = new QButtonGroup(this);
   QObject::connect(geodesicCommentButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(geodesicCommentColumnSelection(int)));
}

/**
 * Called to display comment information about a geodesic column.
 */
void
GuiDisplayControlDialog::geodesicCommentColumnSelection(int column)
{
   GeodesicDistanceFile* gdf = theMainWindow->getBrainSet()->getGeodesicDistanceFile();
   if ((column >= 0) && (column < gdf->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    gdf, column);
      dfcd->show();
   }
}

/**
 * Create and update the geodesic page.
 */
void
GuiDisplayControlDialog::createAndUpdateGeodesicPage()
{
   GeodesicDistanceFile* gdf = theMainWindow->getBrainSet()->getGeodesicDistanceFile();
   numValidGeodesic = gdf->getNumberOfColumns();
   
   const int numExistingGeodesic = static_cast<int>(geodesicNameLineEdits.size());
   
   const int nameMinimumWidth = 400;

   if (geodesicSelectionGridLayout == NULL) {
      QWidget* geodesicSelectionWidget = new QWidget;
      geodesicSelectionGridLayout = new QGridLayout(geodesicSelectionWidget);
      geodesicSelectionGridLayout->setMargin(3);
      geodesicSelectionGridLayout->setSpacing(3);
      geodesicSelectionGridLayout->setColumnMinimumWidth(2, nameMinimumWidth+20);
      const int rowStretchNumber    = 15000;
      geodesicSelectionGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 3, 1, 1, Qt::AlignLeft);
      geodesicSelectionGridLayout->setColumnStretch(0, 1);
      geodesicSelectionGridLayout->setColumnStretch(1, 1);
      geodesicSelectionGridLayout->setColumnStretch(2, 1); //1000);
      geodesicSelectionGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      geodesicMainPageLayout->addWidget(geodesicSelectionWidget, 10, Qt::AlignLeft);
   }
   
   //
   // Add radio buttons and text boxes
   //
   for (int i = numExistingGeodesic; i < numValidGeodesic; i++) {
      //
      // Selection button
      //
      QRadioButton* selectButton = new QRadioButton("");
      geodesicSelectionButtonGroup->addButton(selectButton, i);
      geodesicSelectionRadioButtons.push_back(selectButton);
      geodesicSelectionGridLayout->addWidget(selectButton, i, 0, Qt::AlignHCenter);
      
      //
      // Comment push button
      //
      QPushButton* commentPushButton = new QPushButton("?");
      commentPushButton->setAutoDefault(false);
      commentPushButton->setFixedWidth(40);
      geodesicColumnCommentPushButtons.push_back(commentPushButton);
      geodesicCommentButtonGroup->addButton(commentPushButton, i);
      geodesicSelectionGridLayout->addWidget(commentPushButton, i, 1, Qt::AlignHCenter);
      
      QLineEdit* le = new QLineEdit;
      le->setMinimumWidth(nameMinimumWidth);
      le->setMaximumWidth(1000);
      geodesicNameLineEdits.push_back(le);
      QObject::connect(le, SIGNAL(returnPressed()),
                       this, SLOT(readGeodesicSelections()));
      geodesicSelectionGridLayout->addWidget(le, i, 2, Qt::AlignLeft);
   }
   
   //
   // Update items already in the dialog
   //
   for (int i = 0; i < numValidGeodesic; i++) {
      geodesicSelectionRadioButtons[i]->show();
      geodesicColumnCommentPushButtons[i]->show();
      geodesicNameLineEdits[i]->setText(gdf->getColumnName(i));
      geodesicNameLineEdits[i]->home(false);
      geodesicNameLineEdits[i]->show();
   }
   
   //
   // Hide geodesic that are not needed
   //
   for (int i = numValidGeodesic; i < numExistingGeodesic; i++) {
      geodesicSelectionRadioButtons[i]->hide();
      geodesicColumnCommentPushButtons[i]->hide();
      geodesicNameLineEdits[i]->hide();
   }
}

/**
 * read the geodesic page.
 */
void 
GuiDisplayControlDialog::readGeodesicSelections()
{
   if (creatingDialog) {
      return;
   }
   
   DisplaySettingsGeodesicDistance* dsgd = theMainWindow->getBrainSet()->getDisplaySettingsGeodesicDistance();
   GeodesicDistanceFile* gdf = theMainWindow->getBrainSet()->getGeodesicDistanceFile();
   if (gdf->getNumberOfColumns() > 0) {      
      for (int i = 0; i < gdf->getNumberOfColumns(); i++) {
         const QString name(geodesicNameLineEdits[i]->text());
         if (name != gdf->getColumnName(i)) {
            gdf->setColumnName(i, name);
         }
      }
      dsgd->setDisplayColumn(geodesicSelectionButtonGroup->checkedId());
   }
   dsgd->setPathLineWidth(geodesicPathLineWidthSpinBox->value());
   dsgd->setShowRootNode(geodesicShowRootNodeCheckBox->isChecked());
   dsgd->setPathDisplayEnabled(geodesicDistanceEnableCheckBox->isChecked());
   
   GuiFilesModified fm;
   fm.setGeodesicModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}
      
/**
 * update geodesic items.
 */
void 
GuiDisplayControlDialog::updateGeodesicItems()
{
   updatePageSelectionComboBox();

   if (pageGeodesicMain == NULL) {
      return;
   }
   
   createAndUpdateGeodesicPage();
   DisplaySettingsGeodesicDistance* dsgd = theMainWindow->getBrainSet()->getDisplaySettingsGeodesicDistance();
   pageGeodesicMain->setEnabled(validGeodesicData);
   geodesicDistanceEnableCheckBox->setChecked(dsgd->getPathDisplayEnabled());
   if ((dsgd->getDisplayColumn() >= 0) &&
       (dsgd->getDisplayColumn() < static_cast<int>(geodesicSelectionRadioButtons.size()))) {
      geodesicSelectionRadioButtons[dsgd->getDisplayColumn()]->setChecked(true);
   }
   geodesicShowRootNodeCheckBox->setChecked(dsgd->getShowRootNode());
   geodesicPathLineWidthSpinBox->setValue(dsgd->getPathLineWidth());
}
      
/**
 * create the lat/lon page.
 */
void 
GuiDisplayControlDialog::createLatLonPage()
{
   //
   // Widget for region items
   //
   pageLatLonMain = new QWidget;
   pageWidgetStack->addWidget(pageLatLonMain);
   latLonMainPageLayout = new QVBoxLayout(pageLatLonMain);
   
   //
   // comment button group
   //
   latLonCommentButtonGroup = new QButtonGroup(this);
   QObject::connect(latLonCommentButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(latLonCommentColumnSelection(int)));
}
      
/**
 * Called to display comment information about a lat lon column.
 */
void
GuiDisplayControlDialog::latLonCommentColumnSelection(int column)
{
   LatLonFile* llf = theMainWindow->getBrainSet()->getLatLonFile();
   if ((column >= 0) && (column < llf->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    llf, column);
      dfcd->show();
   }
}

/**
 * Create and update the lat lon page.
 */
void
GuiDisplayControlDialog::createAndUpdateLatLonPage()
{
   LatLonFile* llf = theMainWindow->getBrainSet()->getLatLonFile();
   numValidLatLon = llf->getNumberOfColumns();
   
   const int numExistingLatLon = static_cast<int>(latLonNameLineEdits.size());
   const int nameMinimumWidth = 400;
   
   if (latLonSelectionGridLayout == NULL) {
      QWidget* latLonSelectionWidget = new QWidget;
      latLonSelectionGridLayout = new QGridLayout(latLonSelectionWidget);
      latLonSelectionGridLayout->setMargin(3);
      latLonSelectionGridLayout->setSpacing(3);
      latLonSelectionGridLayout->setColumnMinimumWidth(2, nameMinimumWidth+20);
      const int rowStretchNumber    = 15000;
      latLonSelectionGridLayout->addWidget(new QLabel(" "),
                                           rowStretchNumber, 2, 1, 1, Qt::AlignLeft);
      
      latLonSelectionGridLayout->setColumnStretch(0, 1);
      latLonSelectionGridLayout->setColumnStretch(1, 1);
      latLonSelectionGridLayout->setColumnStretch(2, 1);
      latLonSelectionGridLayout->setRowStretch(rowStretchNumber, 100);
      
      latLonMainPageLayout->addWidget(latLonSelectionWidget);
   }
   
   //
   // Add radio buttons and text boxes
   //
   for (int i = numExistingLatLon; i < numValidLatLon; i++) {
      //
      // Comment push button
      //
      QPushButton* commentPushButton = new QPushButton("?");
      commentPushButton->setAutoDefault(false);
      commentPushButton->setFixedWidth(40);
      latLonColumnCommentPushButtons.push_back(commentPushButton);
      latLonCommentButtonGroup->addButton(commentPushButton, i);
      latLonSelectionGridLayout->addWidget(commentPushButton, i, 0, Qt::AlignHCenter);
      
      QLineEdit* le = new QLineEdit;
      le->setMinimumWidth(nameMinimumWidth);
      le->setMaximumWidth(1000);
      latLonNameLineEdits.push_back(le);
      QObject::connect(le, SIGNAL(returnPressed()),
                       this, SLOT(readLatLonSelections()));
      latLonSelectionGridLayout->addWidget(le, i, 1, Qt::AlignLeft);
   }
   
   //
   // Update items already in the dialog
   //
   for (int i = 0; i < numValidLatLon; i++) {
      latLonColumnCommentPushButtons[i]->show();
      latLonNameLineEdits[i]->setText(llf->getColumnName(i));
      latLonNameLineEdits[i]->home(false);
      latLonNameLineEdits[i]->show();
   }
   
   //
   // Hide lat lon that are not needed
   //
   for (int i = numValidLatLon; i < numExistingLatLon; i++) {
      latLonColumnCommentPushButtons[i]->hide();
      latLonNameLineEdits[i]->hide();
   }
}

/**
 * read the lat lon page.
 */
void 
GuiDisplayControlDialog::readLatLonSelections()
{
   if (creatingDialog) {
      return;
   }
   LatLonFile* llf = theMainWindow->getBrainSet()->getLatLonFile();
   if (llf->getNumberOfColumns() > 0) {      
      for (int i = 0; i < llf->getNumberOfColumns(); i++) {
         const QString name(latLonNameLineEdits[i]->text());
         if (name != llf->getColumnName(i)) {
            llf->setColumnName(i, name);
         }
      }
   }
   GuiFilesModified fm;
   fm.setLatLonModified();
   theMainWindow->fileModificationUpdate(fm);
}
      
/**
 * update lat lon items.
 */
void 
GuiDisplayControlDialog::updateLatLonItems()
{
   updatePageSelectionComboBox();
   if (pageLatLonMain == NULL) {
      return;
   }
   
   latLonCommentButtonGroup->blockSignals(true);
   createAndUpdateLatLonPage();
   latLonCommentButtonGroup->blockSignals(false);
   
   pageLatLonMain->setEnabled(validLatLonData);
}

/**
 * Create the region page.
 */
void
GuiDisplayControlDialog::createScenePage()
{
   //
   // Widget for scene items
   //
   pageSceneMain = new QWidget;
   pageWidgetStack->addWidget(pageSceneMain); //, PAGE_NAME_SCENE); // adds to dialog
   QVBoxLayout* sceneLayout = new QVBoxLayout(pageSceneMain);
   
   //
   // List box for scenes
   //
   sceneListBox = new QListWidget;
   sceneListBox->setMinimumWidth(700);
   sceneListBox->setFixedHeight(350);
   sceneListBox->setSelectionMode(QListWidget::ExtendedSelection);
   QObject::connect(sceneListBox, SIGNAL(itemActivated(QListWidgetItem*)),
                    this, SLOT(slotSceneListBox(QListWidgetItem*)));
   sceneLayout->addWidget(sceneListBox);
                    
   //
   // Show scene push button
   //
   QPushButton* showScenePushButton = new QPushButton("Show Selected Scene...");
   showScenePushButton->setAutoDefault(false);
   showScenePushButton->setToolTip(
                 "Show the selected scene.");
   QObject::connect(showScenePushButton, SIGNAL(clicked()),
                    this, SLOT(slotShowScenePushButton()));
                    
   //
   // Append scene push button
   //
   QPushButton* appendScenePushButton = new QPushButton("Append New Scene...");
   appendScenePushButton->setAutoDefault(false);
   appendScenePushButton->setToolTip(
                 "Create and add a new scene at\n"
                 "the end of the list of scenes.");
   QObject::connect(appendScenePushButton, SIGNAL(clicked()),
                    this, SLOT(slotAppendScenePushButton()));
                    
   //
   // Insert scene push button
   //
   QPushButton* insertScenePushButton = new QPushButton("Insert New Scene...");
   insertScenePushButton->setAutoDefault(false);
   insertScenePushButton->setToolTip( 
                 "Create and insert a new scene\n"
                 "into the list of scenes after\n"
                 "the selected scene.");
   QObject::connect(insertScenePushButton, SIGNAL(clicked()),
                    this, SLOT(slotInsertScenePushButton()));
                    
   //
   // Replace scene push button
   //
   QPushButton* replaceScenePushButton = new QPushButton("Replace Selected Scene...");
   replaceScenePushButton->setAutoDefault(false);
   replaceScenePushButton->setToolTip(
                 "Replace the selected\n"
                 "scene with the current\n"
                 "display settings.");
   QObject::connect(replaceScenePushButton, SIGNAL(clicked()),
                    this, SLOT(slotReplaceScenePushButton()));
                    
   //
   // Delete scene push button
   //
   QPushButton* deleteScenePushButton = new QPushButton("Delete Selected Scene...");
   deleteScenePushButton->setAutoDefault(false);
   deleteScenePushButton->setToolTip(
                 "Delete the selected scene(s).");
   QObject::connect(deleteScenePushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteScenePushButton()));
    
   //
   // Check all scenes button
   //
   QPushButton* checkAllScenesPushButton = new QPushButton("Check All Scenes...");
   checkAllScenesPushButton->setAutoDefault(false);
   checkAllScenesPushButton->setToolTip(
                 "This will verify that all\n"
                 "scenes successfully display.");
   QObject::connect(checkAllScenesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCheckAllScenesPushButton()));
                    
   //
   // Unload all files (except scene)
   //
   QPushButton* unloadFilesPushButton = new QPushButton("Unload All Files Except Scene...");
   unloadFilesPushButton->setAutoDefault(false);
   unloadFilesPushButton->setToolTip( 
                 "Remove all loaded data\n"
                 "except the scenes.");
   QObject::connect(unloadFilesPushButton, SIGNAL(clicked()),
                    theMainWindow, SLOT(slotCloseSpecFileKeepSceneAndSpec()));
   
   //
   // Create a spec file from selected scenes
   //
   QPushButton* createSpecFromSelectedScenesPushButton = new QPushButton("Create Spec From Selected Scenes...");
   createSpecFromSelectedScenesPushButton->setAutoDefault(false);
   createSpecFromSelectedScenesPushButton->setToolTip("Create a Spec File from\n"
                                                      "the data files listed in\n"
                                                      "the selected scenes.");
   QObject::connect(createSpecFromSelectedScenesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateSpecFromSelectedScenesPushButton()));
                    
   //
   // Make all of the buttons the same size
   //
   std::vector<QPushButton*> buttVect;
   buttVect.push_back(showScenePushButton);
   buttVect.push_back(appendScenePushButton);
   buttVect.push_back(insertScenePushButton);
   buttVect.push_back(replaceScenePushButton);
   buttVect.push_back(deleteScenePushButton);
   buttVect.push_back(checkAllScenesPushButton);
   buttVect.push_back(unloadFilesPushButton);
   buttVect.push_back(createSpecFromSelectedScenesPushButton);
   QtUtilities::makeButtonsSameSize(buttVect);
   
   //
   // Grid for buttons
   //
   QGridLayout* buttonsLayout = new QGridLayout;
   sceneLayout->addLayout(buttonsLayout);
   sceneLayout->setSpacing(0);
   buttonsLayout->addWidget(showScenePushButton, 0, 0);
   buttonsLayout->addWidget(appendScenePushButton, 1, 0);
   buttonsLayout->addWidget(insertScenePushButton, 2, 0);
   buttonsLayout->addWidget(replaceScenePushButton, 3, 0);
   buttonsLayout->addWidget(deleteScenePushButton, 0, 1);
   buttonsLayout->addWidget(checkAllScenesPushButton, 1, 1);
   buttonsLayout->addWidget(unloadFilesPushButton, 2, 1);
   buttonsLayout->addWidget(createSpecFromSelectedScenesPushButton, 3, 1);
   
/*
   QVBoxLayout* buttonsLayout = new QVBoxLayout;
   sceneLayout->addLayout(buttonsLayout);
   sceneLayout->setSpacing(0);
   buttonsLayout->addWidget(showScenePushButton, 0, Qt::AlignHCenter);
   buttonsLayout->addWidget(appendScenePushButton, 0, Qt::AlignHCenter);
   buttonsLayout->addWidget(insertScenePushButton, 0, Qt::AlignHCenter);
   buttonsLayout->addWidget(replaceScenePushButton, 0, Qt::AlignHCenter);
   buttonsLayout->addWidget(deleteScenePushButton, 0, Qt::AlignHCenter);
   if (sendScenePushButton != NULL) {
      buttonsLayout->addWidget(sendScenePushButton, 0, Qt::AlignHCenter);
   }
   if (sendSettingsPushButton != NULL) {
      buttonsLayout->addWidget(sendSettingsPushButton, 0, Qt::AlignHCenter);
   }
   buttonsLayout->addWidget(checkAllScenesPushButton, 0, Qt::AlignHCenter);
   buttonsLayout->addWidget(unloadFilesPushButton, 0, Qt::AlignHCenter);
*/   
   
/*
   QtUtilities::makeButtonsSameSize(showScenePushButton,
                                    createScenePushButton, 
                                    replaceScenePushButton, 
                                    deleteScenePushButton,
                                    unloadFilesPushButton,
                                    sendScenePushButton);
*/

   //
   // Preserve foci, foci colors, study metadata option
   //   
   scenePreserveFociCheckBox = new QCheckBox("Preserve Foci, Foci Colors, and Study Metadata");
   QObject::connect(scenePreserveFociCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readSceneSelections()));
   sceneLayout->addWidget(scenePreserveFociCheckBox);
   
   //
   // window positions
   //
   sceneWindowPositionComboBox = new QComboBox;
   sceneWindowPositionComboBox->insertItem(DisplaySettingsScene::WINDOW_POSITIONS_USE_ALL,
                                           "Use All Window Positions From Scene");
   sceneWindowPositionComboBox->insertItem(DisplaySettingsScene::WINDOW_POSITIONS_IGNORE_MAIN_OTHERS_RELATIVE,
                                           "Do Not Move Main Window, All Other Windows Relative to Main Window");
   sceneWindowPositionComboBox->insertItem(DisplaySettingsScene::WINDOW_POSITIONS_IGNORE_ALL,
                                           "Ignore All Window Positions in Scene");
   QObject::connect(sceneWindowPositionComboBox, SIGNAL(activated(int)),
                    this, SLOT(readSceneSelections()));
   QGroupBox* windowPosGroupBox = new QGroupBox("Window Positioning");
   QVBoxLayout* windowPosGroupLayout = new QVBoxLayout(windowPosGroupBox);
   windowPosGroupLayout->addWidget(sceneWindowPositionComboBox);
   sceneLayout->addWidget(windowPosGroupBox);
                     
   //
   // Put extra space at bottom of dialog
   //
   sceneLayout->addStretch();
   pageSceneMain->setFixedSize(pageSceneMain->sizeHint());
}

/**
 * called to create a spec file from files used in selected scenes.
 */
void 
GuiDisplayControlDialog::slotCreateSpecFromSelectedScenesPushButton()
{
   const std::vector<int> selectedScenes = getSelectedScenes();
   if (selectedScenes.empty()) {
      QMessageBox::critical(this, "ERROR", "There are no selected scenes.");
      return;
   }
   
   //
   // Display dialog to create new spec and scene files
   //
   GuiSpecAndSceneFileCreationDialog d(this,
                                       theMainWindow->getBrainSet(),
                                       selectedScenes);
   d.exec();
}
      
/**
 * get the selected scenes.
 */
std::vector<int> 
GuiDisplayControlDialog::getSelectedScenes() const
{
   std::vector<int> sel;
   
   for (int i = 0; i < sceneListBox->count(); i++) {
      QListWidgetItem* item = sceneListBox->item(i);
      if (sceneListBox->isItemSelected(item)) {
         sel.push_back(i);
      }
   }
   
   return sel;
}

/**
 * set the selected scene.
 */
void 
GuiDisplayControlDialog::setSelectedSceneItem(const int item)
{
   sceneListBox->setCurrentRow(item);
/*
   for (unsigned int i = 0; i < sceneListBox->count(); i++) {
      sceneListBox->setSelected(i, (i == static_cast<unsigned int>(item)));
   }
*/
}      

/**
 * called when a scene is selected.
 */
void 
GuiDisplayControlDialog::slotSceneListBox(QListWidgetItem* item)
{
   for (int i = 0; i < sceneListBox->count(); i++) {
      if (sceneListBox->item(i) == item) {
         slotSceneListBox(i);
         break;
      }
   }
}
      
/**
 * called when a scene is selected.
 */
void 
GuiDisplayControlDialog::slotSceneListBox(int item)
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   SceneFile* sf = theMainWindow->getBrainSet()->getSceneFile();
   if ((item >= 0) && (item < sf->getNumberOfScenes())) {
      SceneFile::Scene* scene = sf->getScene(item);
      
      QString errorMessage;
      
      theMainWindow->showScene(scene, false, errorMessage);
      
      if (errorMessage.isEmpty() == false) {
         //if (DebugControl::getDebugOn()) {
            QApplication::beep();
            QtTextEditDialog* te = new QtTextEditDialog(this, true);
            QString msg("The following errors occurred while applying the scene:\n\n");
            msg.append(errorMessage);
            te->setText(msg);
            te->show();
         //}
      }
      setSelectedSceneItem(item);  
   } 
     
   QApplication::restoreOverrideCursor();
}
      
/**
 * called to check all scenes.
 */
void 
GuiDisplayControlDialog::slotCheckAllScenesPushButton()
{
   QString msg1("Press \"Continue\" to sequence through and validate\n"
                   "all of the scenes.  This may take a while.");
   if (QMessageBox::information(this, 
                                "Confirm Check Scenes", 
                                msg1, 
                                (QMessageBox::Ok | QMessageBox::Cancel),
                                QMessageBox::Ok)
                                    == QMessageBox::Cancel) {
      return;
   }
   
   bool ok = false;
   const int pauseBetweenScenes = QInputDialog::getInteger(this,
                                                           "Verify Scenes",
                                                           "Pause for Seconds Between Scenes",
                                                           0,
                                                           0,
                                                           60,
                                                           1,
                                                           &ok);
   if (ok == false) {
      return;
   }
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   QString errorMessage;
      
   SceneFile* sf = theMainWindow->getBrainSet()->getSceneFile();
   const int numScenes = sf->getNumberOfScenes();
   
   QProgressDialog pd("Verifying Scenes",
                      "Cancel",
                      0,
                      numScenes,
                      this);
   pd.setWindowTitle("Verifying Scenes");
   
   for (int i = 0; i < numScenes; i++) {
      SceneFile::Scene* scene = sf->getScene(i);
      
      pd.setValue(i);
      pd.setLabelText(scene->getName());
      pd.show();
      qApp->processEvents();
      if (pd.wasCanceled()) {
         break;
      }
      
      QString msg;
      theMainWindow->closeSpecFile(true, false);
      theMainWindow->showScene(scene, true, msg);
      if (msg.isEmpty() == false) {
         errorMessage.append("--------------------------------------------------------------\n");
         errorMessage.append(scene->getName());
         errorMessage.append("\n");
         errorMessage.append(msg);
         errorMessage.append("\n");
         errorMessage.append("\n");
      }
      
      if (pauseBetweenScenes > 0) {
         SystemUtilities::sleepForSeconds(pauseBetweenScenes);
      }
   } 
   
   pd.setValue(numScenes + 1);
   pd.close();
   
   QApplication::restoreOverrideCursor();
   
   if (errorMessage.isEmpty() == false) {   
      QApplication::beep();
      QtTextEditDialog* te = new QtTextEditDialog(this, true, false);
      te->setText(errorMessage);
      te->show();
   }
   else {
      QMessageBox::information(this, "INFO", "All scenes successfully displayed."); 
   }
}
      
/**
 * called to show a scene.
 */
void 
GuiDisplayControlDialog::slotShowScenePushButton()
{
   const std::vector<int> selectedScenes = getSelectedScenes();
   if (selectedScenes.empty()) {
      QMessageBox::critical(this, "ERROR", "There is no selected scene.");
      return;
   }
   else if (selectedScenes.size() > 1) {
      QMessageBox::critical(this, "ERROR", "There is more than one selected scene.");
      return;
   }
   const int selectedIndex = selectedScenes[0];
   
   slotSceneListBox(selectedIndex);
}

/** 
 * called to delete a scene.
 */
void 
GuiDisplayControlDialog::slotDeleteScenePushButton()
{
   const std::vector<int> selectedScenes = getSelectedScenes();
   if (selectedScenes.empty()) {
      QMessageBox::critical(this, "ERROR", "There are no selected scenes.");
      return;
   }

   SceneFile* sf = theMainWindow->getBrainSet()->getSceneFile();
   if (QMessageBox::question(this, 
                             "Confirm", 
                             "Delete Selected Scene(s) ?", 
                             (QMessageBox::Ok | QMessageBox::Cancel),
                             QMessageBox::Ok)
                                == QMessageBox::Ok) {
      for (int i = selectedScenes.size() - 1; i >= 0; i--) {
         sf->deleteScene(selectedScenes[i]);
      }
      updateSceneItems();
      const int item = selectedScenes[0];
      if (item < sf->getNumberOfScenes()) {
         setSelectedSceneItem(item);
      }
      else if (sf->getNumberOfScenes() > 0) {
         setSelectedSceneItem(sf->getNumberOfScenes());
      }
   }
}

/**
 * called to replace a scene.
 */
void 
GuiDisplayControlDialog::slotReplaceScenePushButton()
{
   //
   // Check for multiple brain sets
   //
   if (theMainWindow->getNumberOfBrainSets() > 1) {
      QMessageBox::critical(this, "ERROR", "There is MORE than one Spec File loaded.\n"
                                    "At this time, Caret can only create scenes with one spec file loaded.");
      return;
   }

   const std::vector<int> selectedScenes = getSelectedScenes();
   if (selectedScenes.empty()) {
      QMessageBox::critical(this, "ERROR", "There is no selected scene.");
      return;
   }
   else if (selectedScenes.size() > 1) {
      QMessageBox::critical(this, "ERROR", "There is more than one selected scene.");
      return;
   }
   const int selectedIndex = selectedScenes[0];

   SceneFile* sf = theMainWindow->getBrainSet()->getSceneFile();
   
   //
   // See if data files are modified
   //
   QString msg;
   theMainWindow->checkForModifiedFiles(theMainWindow->getBrainSet(), msg, false);
   if (msg.isEmpty() == false) {
      //
      // Return value of zero is YES button.
      //
      QString msg2("It appears that data files have been modified.  Since\n"
                       "data file names are inserted into the scene file, the\n"
                       "data files should be saved prior to saving the scene.\n\n"
                       "\n");
      msg2.append(msg);
      msg2.append("\n");
      msg2.append("Are you sure you want to save the scene?\n");
      
      if (QMessageBox::warning(this, 
                              "WARNING",
                              msg2, 
                              (QMessageBox::Yes | QMessageBox::No),
                              QMessageBox::No)
                                 == QMessageBox::No) {
         return;
      }
   }
   
   bool ok = false;
   const QString defName = sf->getScene(selectedIndex)->getName();
   const QString labelValue = QString("Name of Scene").leftJustified(100, ' ');
   const QString qname = QInputDialog::getText(this, "Scene Name",
                         labelValue,
                         QLineEdit::Normal,
                         defName,
                         &ok);
   if (ok) {
      QString name(qname);
      //
      // Save window information
      //
      std::vector<SceneFile::SceneClass> mainWindowSceneClasses;
      theMainWindow->saveScene(mainWindowSceneClasses);
      
      //
      // Save all display settings
      //
      QString errorMessage;
      theMainWindow->getBrainSet()->replaceScene(sf, selectedIndex, mainWindowSceneClasses, 
                          name, false, errorMessage);

      updateSceneItems();
      setSelectedSceneItem(selectedIndex);
      
      if (errorMessage.isEmpty() == false) {
         QApplication::beep();
         QtTextEditDialog* te = new QtTextEditDialog(this, true);
         QString msg(errorMessage);
         te->setText(msg);
         te->show();
      }
   }
}
      
/**
 * called to append a new scene.
 */
void 
GuiDisplayControlDialog::slotAppendScenePushButton()
{
   //
   // Check for multiple brain sets
   //
   if (theMainWindow->getNumberOfBrainSets() > 1) {
      QMessageBox::critical(this, "ERROR", "There is MORE than one Spec File loaded.\n"
                                    "At this time, Caret can only create scenes with one spec file loaded.");
      return;
   }
   
   //
   // See if data files are modified
   //
   QString msg;
   theMainWindow->checkForModifiedFiles(theMainWindow->getBrainSet(), msg, false);
   if (msg.isEmpty() == false) {
      //
      // Return value of zero is YES button.
      //
      QString msg2("It appears that data files have been modified.  Since\n"
                       "data file names are inserted into the scene file, the\n"
                       "data files should be saved prior to saving the scene.\n"                       
                       "\n");
      msg2.append(msg);
      msg2.append("\n");
      msg2.append("Are you sure you want to save the scene?\n");
      
      if (QMessageBox::warning(this, 
                              "WARNING",
                              msg2, 
                              (QMessageBox::Yes | QMessageBox::No),
                              QMessageBox::No)
                                 == QMessageBox::No) {
         return;
      }
   }
   
   bool ok = false;
   const QString labelValue = QString("Enter Name of New Scene").leftJustified(100, ' ');
   const QString qname = QInputDialog::getText(this, "Scene Name",
                         labelValue,
                         QLineEdit::Normal,
                         "",
                         &ok);
   if (ok) {
      QString name(qname);
      //
      // Save window information
      //
      std::vector<SceneFile::SceneClass> mainWindowSceneClasses;
      theMainWindow->saveScene(mainWindowSceneClasses);
      
      //
      // Save all display settings
      //
      QString errorMessage;
      theMainWindow->getBrainSet()->saveScene(theMainWindow->getBrainSet()->getSceneFile(), mainWindowSceneClasses, name, false, errorMessage);

      updateSceneItems();
      setSelectedSceneItem(sceneListBox->count() - 1);
      
      if (errorMessage.isEmpty() == false) {
         QApplication::beep();
         QtTextEditDialog* te = new QtTextEditDialog(this, true);
         QString msg(errorMessage);
         te->setText(msg);
         te->show();
      }
   }
}
      
/**
 * called to insert a new scene.
 */
void 
GuiDisplayControlDialog::slotInsertScenePushButton()
{
   //
   // Check for multiple brain sets
   //
   if (theMainWindow->getNumberOfBrainSets() > 1) {
      QMessageBox::critical(this, "ERROR", "There is MORE than one Spec File loaded.\n"
                                    "At this time, Caret can only create scenes with one spec file loaded.");
      return;
   }

   const std::vector<int> selectedScenes = getSelectedScenes();
   if (selectedScenes.empty()) {
      QMessageBox::critical(this, "ERROR", "There is no selected scene.");
      return;
   }
   else if (selectedScenes.size() > 1) {
      QMessageBox::critical(this, "ERROR", "There is more than one selected scene.");
      return;
   }
   const int selectedIndex = selectedScenes[0];

   SceneFile* sf = theMainWindow->getBrainSet()->getSceneFile();
   
   //
   // See if data files are modified
   //
   QString msg;
   theMainWindow->checkForModifiedFiles(theMainWindow->getBrainSet(), msg, false);
   if (msg.isEmpty() == false) {
      //
      // Return value of zero is YES button.
      //
      QString msg2("It appears that data files have been modified.  Since\n"
                       "data file names are inserted into the scene file, the\n"
                       "data files should be saved prior to saving the scene.\n\n"
                       "\n");
      msg2.append(msg);
      msg2.append("\n");
      msg2.append("Are you sure you want to save the scene?\n");
      
      if (QMessageBox::warning(this, 
                              "WARNING",
                              msg2, 
                              (QMessageBox::Yes | QMessageBox::No),
                              QMessageBox::No)
                                 == QMessageBox::No) {
         return;
      }
   }
   
   bool ok = false;
   const QString labelValue = QString("Enter Name of New Scene").leftJustified(100, ' ');
   const QString qname = QInputDialog::getText(this, "Scene Name",
                         labelValue,
                         QLineEdit::Normal,
                         "",
                         &ok);
   if (ok) {
      QString name(qname);
      //
      // Save window information
      //
      std::vector<SceneFile::SceneClass> mainWindowSceneClasses;
      theMainWindow->saveScene(mainWindowSceneClasses);
      
      //
      // Save all display settings
      //
      QString errorMessage;
      theMainWindow->getBrainSet()->insertScene(sf, selectedIndex, mainWindowSceneClasses, name, false, errorMessage);

      updateSceneItems();
      setSelectedSceneItem(selectedIndex + 1);
      
      if (errorMessage.isEmpty() == false) {
         QApplication::beep();
         QtTextEditDialog* te = new QtTextEditDialog(this, true);
         QString msg(errorMessage);
         te->setText(msg);
         te->show();
      }
   }
}
      
/**
 * read the scene selections.
 */
void 
GuiDisplayControlDialog::readSceneSelections()
{
   DisplaySettingsScene* dss = theMainWindow->getBrainSet()->getDisplaySettingsScene();
   dss->setWindowPositionPreference(
      static_cast<DisplaySettingsScene::WINDOW_POSITIONS>(sceneWindowPositionComboBox->currentIndex()));
   dss->setPreserveFociAndFociColorsAndStudyMetaDataFlag(
                                  scenePreserveFociCheckBox->isChecked());
}
      
/**
 * update scene items.
 */
void 
GuiDisplayControlDialog::updateSceneItems()
{
   updatePageSelectionComboBox();
   
   if (pageSceneMain == NULL) {
      return;
   }
   //
   // Updating of scene page is usually skipped when a scene is being loaded
   //
   if (skipScenePageUpdate) {
      return;
   }
   
   DisplaySettingsScene* dss = theMainWindow->getBrainSet()->getDisplaySettingsScene();
   sceneWindowPositionComboBox->blockSignals(true);
   sceneWindowPositionComboBox->setCurrentIndex(dss->getWindowPositionPreference());
   sceneWindowPositionComboBox->blockSignals(false);
   
   scenePreserveFociCheckBox->blockSignals(true);
   scenePreserveFociCheckBox->setChecked(dss->getPreserveFociAndFociColorsAndStudyMetaDataFlag());
   scenePreserveFociCheckBox->blockSignals(false);
   
   const int item = sceneListBox->currentRow();
   sceneListBox->clear();
   sceneListBox->blockSignals(true);
   SceneFile* sf = theMainWindow->getBrainSet()->getSceneFile();
   for (int i = 0; i < sf->getNumberOfScenes(); i++) {
      const SceneFile::Scene* ss = sf->getScene(i);
      sceneListBox->addItem(ss->getName());
   }
   if ((item >= 0) && (item < static_cast<int>(sceneListBox->count()))) {
      setSelectedSceneItem(item);
   }
   else if (sceneListBox->count() > 0) {
      setSelectedSceneItem(0);
   }
   sceneListBox->blockSignals(false);
   
   pageSceneMain->setEnabled(validSceneData);
}

/**
 * Create the region page.
 */
void
GuiDisplayControlDialog::createRegionPage()
{
   const int maxWidth = 350;
   
   //
   // Popup graph check box
   //
   regionPopupGraphCheckBox = new QCheckBox("Popup Time Course Graph When Node Identified");
   QObject::connect(regionPopupGraphCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readRegionSelections()));
                    
   //
   // All time courses
   //
   regionAllTimeCoursesCheckBox = new QRadioButton("");
   QObject::connect(regionAllTimeCoursesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readRegionSelections()));
   QLabel* allLabel = new QLabel("All ");
   
   //
   // Single Time Course
   //
   regionSingleTimeCourseCheckBox = new QRadioButton("");
   QObject::connect(regionSingleTimeCourseCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readRegionSelections()));
   QLabel* singleLabel = new QLabel("Single ");
   regionTimeCourseComboBox = new QComboBox;
   regionTimeCourseComboBox->setMaximumWidth(maxWidth);
   regionTimeCourseComboBox->setFixedWidth(250);
   QObject::connect(regionTimeCourseComboBox, SIGNAL(activated(int)),
                    this, SLOT(readRegionSelections()));
   
   //
   // Time Course Group Box
   //
   QGroupBox* timeGroupBox = new QGroupBox("Time Course");
   QGridLayout* timeGroupLayout = new QGridLayout(timeGroupBox);
   timeGroupLayout->addWidget(regionAllTimeCoursesCheckBox, 0, 0);
   timeGroupLayout->addWidget(allLabel, 0, 1);
   timeGroupLayout->addWidget(regionSingleTimeCourseCheckBox, 1, 0);
   timeGroupLayout->addWidget(singleLabel, 1, 1);
   timeGroupLayout->addWidget(regionTimeCourseComboBox, 1, 2);
   timeGroupBox->setFixedSize(timeGroupBox->sizeHint());
   
   //
   // Button group to keep radio buttons mutually exclusive
   //
   QButtonGroup* timeButtGroup = new QButtonGroup(this);
   timeButtGroup->addButton(regionAllTimeCoursesCheckBox, 0);
   timeButtGroup->addButton(regionSingleTimeCourseCheckBox, 1);
   
   //
   // Region Case
   //
   QLabel* caseLabel = new QLabel("Case");
   regionCaseComboBox = new QComboBox;
   regionCaseComboBox->setFixedWidth(maxWidth);
   QObject::connect(regionCaseComboBox, SIGNAL(activated(int)),
                    this, SLOT(readRegionSelections()));
   
   //
   // Paint volume
   //
   QLabel* paintVolumeLabel = new QLabel("Paint Volume");
   regionPaintVolumeComboBox = new QComboBox;
   regionPaintVolumeComboBox->setFixedWidth(maxWidth);
   QObject::connect(regionPaintVolumeComboBox, SIGNAL(activated(int)),
                    this, SLOT(readRegionSelections()));
   
   //
   // Grid for other items
   //
   QWidget* regionWidget = new QWidget;
   QGridLayout* regionGridLayout = new QGridLayout(regionWidget);
   regionGridLayout->setSpacing(5);
   regionGridLayout->addWidget(caseLabel, 0, 0);
   regionGridLayout->addWidget(regionCaseComboBox, 0, 1);
   regionGridLayout->addWidget(paintVolumeLabel, 1, 0);
   regionGridLayout->addWidget(regionPaintVolumeComboBox, 1, 1);
   regionWidget->setFixedSize(regionWidget->sizeHint());
   
   //
   // Selection group box
   //
   QGroupBox* selectGroupBox = new QGroupBox("Selection");
   QVBoxLayout* selectGroupLayout = new QVBoxLayout(selectGroupBox);
   selectGroupLayout->addWidget(regionPopupGraphCheckBox);
   selectGroupLayout->addWidget(timeGroupBox);
   selectGroupLayout->addWidget(regionWidget);
   selectGroupBox->setFixedSize(selectGroupBox->sizeHint());
   
   //
   // Auto Scale
   //
   regionGraphAutoScaleRadioButton = new QRadioButton("Auto Scale");
   //
   // User Scale
   //
   regionGraphUserScaleRadioButton = new QRadioButton("User Scale");
   regionGraphUserScaleMinLineEdit = new QLineEdit;
   regionGraphUserScaleMinLineEdit->setFixedWidth(125);
   QObject::connect(regionGraphUserScaleMinLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(readRegionSelections()));
   regionGraphUserScaleMinLineEdit->setToolTip(
                 "Minimum Value on Graph");
   regionGraphUserScaleMaxLineEdit = new QLineEdit;
   regionGraphUserScaleMaxLineEdit->setFixedWidth(125);
   QObject::connect(regionGraphUserScaleMaxLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(readRegionSelections()));
   regionGraphUserScaleMaxLineEdit->setToolTip(
                 "Minimum Value on Graph");

   //
   // Button group for keeping buttons mutually exclusive
   //
   QButtonGroup* buttGroup = new QButtonGroup(this);
   buttGroup->addButton(regionGraphAutoScaleRadioButton, 0);
   buttGroup->addButton(regionGraphUserScaleRadioButton, 1);
   QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(readRegionSelections()));
   
   //
   // Auto/manual scale
   //
   QGroupBox* scaleGroupBox = new QGroupBox("Graph Scale");
   QGridLayout* scaleGroupLayout = new QGridLayout(scaleGroupBox);
   scaleGroupLayout->addWidget(regionGraphAutoScaleRadioButton, 0, 0);
   scaleGroupLayout->addWidget(regionGraphUserScaleRadioButton, 1, 0);
   scaleGroupLayout->addWidget(regionGraphUserScaleMinLineEdit, 1, 1);
   scaleGroupLayout->addWidget(regionGraphUserScaleMaxLineEdit, 1, 2);
   scaleGroupBox->setFixedSize(scaleGroupBox->sizeHint());
   
   //
   // Widget for region items
   //
   pageRegionMain = new QWidget;
   pageWidgetStack->addWidget(pageRegionMain); // adds to dialog
   QVBoxLayout* regionMainPageLayout = new QVBoxLayout(pageRegionMain);
   regionMainPageLayout->addWidget(selectGroupBox);
   regionMainPageLayout->addWidget(scaleGroupBox);
   regionMainPageLayout->addStretch();
}

/**
 * read the region selections.
 */
void 
GuiDisplayControlDialog::readRegionSelections()
{
   if (creatingDialog) {
      return;
   }
   
   DisplaySettingsWustlRegion* dswr = theMainWindow->getBrainSet()->getDisplaySettingsWustlRegion();
   
   if (regionAllTimeCoursesCheckBox->isChecked()) {
      dswr->setTimeCourseSelection(DisplaySettingsWustlRegion::TIME_COURSE_SELECTION_ALL);
   }
   else if (regionSingleTimeCourseCheckBox->isChecked()) {
      dswr->setTimeCourseSelection(DisplaySettingsWustlRegion::TIME_COURSE_SELECTION_SINGLE);
   }
   
   dswr->setSelectedTimeCourse(regionTimeCourseComboBox->currentIndex());
   
   const int item = regionCaseComboBox->currentIndex();
   if (item >= 0) {
      dswr->setSelectedCaseName(regionCaseComboBox->currentText());
   }
   dswr->setSelectedPaintVolume(regionPaintVolumeComboBox->currentIndex());
   dswr->setPopupGraphEnabled(regionPopupGraphCheckBox->isChecked());
   
   if (regionGraphAutoScaleRadioButton->isChecked()) {
      dswr->setGraphMode(DisplaySettingsWustlRegion::GRAPH_MODE_AUTO_SCALE);
   }
   else if (regionGraphUserScaleRadioButton->isChecked()) {
      dswr->setGraphMode(DisplaySettingsWustlRegion::GRAPH_MODE_USER_SCALE);
   }
   dswr->setUserScale(regionGraphUserScaleMinLineEdit->text().toFloat(),
                      regionGraphUserScaleMaxLineEdit->text().toFloat());
}

/**
 * update region items.
 */
void 
GuiDisplayControlDialog::updateRegionItems()
{
   updatePageSelectionComboBox();

   if (pageRegionMain == NULL) {
      return;
   }
   
   DisplaySettingsWustlRegion* dswr = theMainWindow->getBrainSet()->getDisplaySettingsWustlRegion();
   const WustlRegionFile* wrf = theMainWindow->getBrainSet()->getWustlRegionFile();
   
   switch (dswr->getTimeCourseSelection()) {
      case DisplaySettingsWustlRegion::TIME_COURSE_SELECTION_ALL:
         regionAllTimeCoursesCheckBox->setChecked(true);
         break;
      case DisplaySettingsWustlRegion::TIME_COURSE_SELECTION_SINGLE:
         regionSingleTimeCourseCheckBox->setChecked(true);
         break;
   }
   
   const int numTimeCourses = wrf->getNumberOfTimeCourses();
   const int numPaintVolumes = theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles();
   bool valid = (numTimeCourses > 0) && (numPaintVolumes > 0);
   
   pageRegionMain->setEnabled(validRegionData);
   
   if (valid) {
      regionTimeCourseComboBox->clear();
      for (int i = 0; i < numTimeCourses; i++) {
         regionTimeCourseComboBox->addItem(wrf->getTimeCourse(i)->getName());
      }
      regionTimeCourseComboBox->setCurrentIndex(dswr->getSelectedTimeCourse());
      
      int item = -1;
      regionCaseComboBox->clear();
      const int selectedTimeCourse = dswr->getSelectedTimeCourse();
      if ((selectedTimeCourse >= 0) &&
          (selectedTimeCourse < wrf->getNumberOfTimeCourses())) {
         const WustlRegionFile::TimeCourse* tc = wrf->getTimeCourse(selectedTimeCourse);
         std::vector<QString> caseNames;
         tc->getAllRegionCaseNames(caseNames);
         for (int j = 0; j < static_cast<int>(caseNames.size()); j++) {
            regionCaseComboBox->addItem(caseNames[j]);
            if (caseNames[j] == dswr->getSelectedCaseName()) {
               item = j;
            }
         }
      }
      if (item < 0) {
         item = 0;
      }
      regionCaseComboBox->setCurrentIndex(item);
      dswr->setSelectedCaseName(regionCaseComboBox->currentText());

      regionPaintVolumeComboBox->clear();
      for (int i = 0; i < numPaintVolumes; i++) {
         const QString name = FileUtilities::basename(
                      theMainWindow->getBrainSet()->getVolumePaintFile(i)->getFileName());
         regionPaintVolumeComboBox->addItem(name);
      }
      regionPaintVolumeComboBox->setCurrentIndex(dswr->getSelectedPaintVolume());
      
      regionPopupGraphCheckBox->setChecked(dswr->getPopupGraphEnabled());
      
      switch (dswr->getGraphMode()) {
         case DisplaySettingsWustlRegion::GRAPH_MODE_AUTO_SCALE:
            regionGraphAutoScaleRadioButton->setChecked(true);
            break;
         case DisplaySettingsWustlRegion::GRAPH_MODE_USER_SCALE:
            regionGraphUserScaleRadioButton->setChecked(true);
            break;
      }
      
      float minScale, maxScale;
      dswr->getUserScale(minScale, maxScale);
      regionGraphUserScaleMinLineEdit->setText(QString::number(minScale));
      regionGraphUserScaleMaxLineEdit->setText(QString::number(maxScale));
   }
}
      
/**
 * Create the RGB Paint page
 */
void
GuiDisplayControlDialog::createRgbPaintPage()
{
   //
   //
   // label combo box for file selection
   //
   rgbSelectionComboBox = new QComboBox;
   rgbSelectionComboBox->setToolTip( "Choose RGB Paint File");
   QObject::connect(rgbSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(rgbPaintFileSelection(int)));
                    
   const int CHECK_COLUMN   = 0;
   const int NAME_COLUMN    = CHECK_COLUMN + 1;
   const int THRESH_COLUMN  = NAME_COLUMN + 1;
   const int NEG_MAX_COLUMN = THRESH_COLUMN + 1;
   const int POS_MAX_COLUMN = NEG_MAX_COLUMN + 1;
   const int COMMENT_COLUMN = POS_MAX_COLUMN + 1;
   //const int NUM_COLUMNS    = COMMENT_COLUMN + 1;
   
   const int TITLE_ROW = 0;
   const int RED_ROW   = TITLE_ROW + 1;
   const int GREEN_ROW = RED_ROW + 1;
   const int BLUE_ROW  = GREEN_ROW + 1;
   //const int NUM_ROWS  = BLUE_ROW + 1;
   
   //
   // Grid Layout for R/G/B items
   //
   QGridLayout* rgbGridLayout = new QGridLayout;
   
   //
   // Grid Column titles
   //
   rgbGridLayout->addWidget(new QLabel(" "), TITLE_ROW, CHECK_COLUMN);
   rgbGridLayout->addWidget(new QLabel("Name"), TITLE_ROW, NAME_COLUMN, 
                            Qt::AlignLeft);
   rgbGridLayout->addWidget(new QLabel("Threshold"), TITLE_ROW, THRESH_COLUMN,  
                            Qt::AlignHCenter);
   rgbGridLayout->addWidget(new QLabel("Neg Max"), TITLE_ROW, NEG_MAX_COLUMN,  
                            Qt::AlignLeft);
   rgbGridLayout->addWidget(new QLabel("Pos Max"), TITLE_ROW, POS_MAX_COLUMN,  
                            Qt::AlignLeft);
   rgbGridLayout->addWidget(new QLabel("Comment"), TITLE_ROW, COMMENT_COLUMN,
                            Qt::AlignHCenter);
    
   //
   // Button group for comment "?" pushbuttons
   //
   QButtonGroup* commentButtonGroup = new QButtonGroup(this);
   QObject::connect(commentButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(rgbPaintCommentSelection(int)));
   
   //
   // Red row
   //
   rgbRedCheckBox = new QCheckBox("Red");
   rgbGridLayout->addWidget(rgbRedCheckBox, RED_ROW, CHECK_COLUMN);
   QObject::connect(rgbRedCheckBox, SIGNAL(clicked()),
                    this, SLOT(readRgbPaintSelections()));
   rgbRedNameLabel = new QLabel("                    ");
   rgbRedNameLabel->setFixedSize(rgbRedNameLabel->sizeHint());
   rgbGridLayout->addWidget(rgbRedNameLabel, RED_ROW, NAME_COLUMN);
   rgbRedThreshLineEdit = new QLineEdit;
   QSize qs = rgbRedThreshLineEdit->sizeHint();
   qs.setWidth(qs.width() / 2);
   rgbRedThreshLineEdit->setFixedSize(qs);
   rgbGridLayout->addWidget(rgbRedThreshLineEdit, RED_ROW, THRESH_COLUMN);
   QObject::connect(rgbRedThreshLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(readRgbPaintSelections()));
   rgbRedNegMaxLabel = new QLabel("            ");
   rgbRedNegMaxLabel->setFixedSize(rgbRedNegMaxLabel->sizeHint());
   rgbGridLayout->addWidget(rgbRedNegMaxLabel, RED_ROW, NEG_MAX_COLUMN);
   rgbRedPosMaxLabel = new QLabel("            ");
   rgbRedPosMaxLabel->setFixedSize(rgbRedPosMaxLabel->sizeHint());
   rgbGridLayout->addWidget(rgbRedPosMaxLabel, RED_ROW, POS_MAX_COLUMN);
   QPushButton* redCommentPushButton = new QPushButton("?");
   rgbGridLayout->addWidget(redCommentPushButton, RED_ROW, COMMENT_COLUMN);
   QSize infoButtonSize = redCommentPushButton->sizeHint();
   infoButtonSize.setWidth(40);
   redCommentPushButton->setFixedSize(infoButtonSize);
   redCommentPushButton->setAutoDefault(false);
   redCommentPushButton->setToolTip( "Show Red Comment");
   commentButtonGroup->addButton(redCommentPushButton);
   
   //
   // Green row
   //
   rgbGreenCheckBox = new QCheckBox("Green");
   rgbGridLayout->addWidget(rgbGreenCheckBox, GREEN_ROW, CHECK_COLUMN);
   QObject::connect(rgbGreenCheckBox, SIGNAL(clicked()),
                    this, SLOT(readRgbPaintSelections()));
   rgbGreenNameLabel = new QLabel("                    ");
   rgbGreenNameLabel->setFixedSize(rgbGreenNameLabel->sizeHint());
   rgbGridLayout->addWidget(rgbGreenNameLabel, GREEN_ROW, NAME_COLUMN);
   rgbGreenThreshLineEdit = new QLineEdit;
   rgbGreenThreshLineEdit->setFixedSize(qs);
   rgbGridLayout->addWidget(rgbGreenThreshLineEdit, GREEN_ROW, THRESH_COLUMN);
   QObject::connect(rgbGreenThreshLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(readRgbPaintSelections()));
   rgbGreenNegMaxLabel = new QLabel("            ");
   rgbGreenNegMaxLabel->setFixedSize(rgbGreenNegMaxLabel->sizeHint());
   rgbGridLayout->addWidget(rgbGreenNegMaxLabel, GREEN_ROW, NEG_MAX_COLUMN);
   rgbGreenPosMaxLabel = new QLabel("            ");
   rgbGreenPosMaxLabel->setFixedSize(rgbGreenPosMaxLabel->sizeHint());
   rgbGridLayout->addWidget(rgbGreenPosMaxLabel, GREEN_ROW, POS_MAX_COLUMN);
   QPushButton* greenCommentPushButton = new QPushButton("?");
   rgbGridLayout->addWidget(greenCommentPushButton, GREEN_ROW, COMMENT_COLUMN);
   greenCommentPushButton->setFixedSize(infoButtonSize);
   greenCommentPushButton->setAutoDefault(false);
   greenCommentPushButton->setToolTip( "Show Green Comment");
   commentButtonGroup->addButton(greenCommentPushButton);
   
   //
   // Blue row
   //
   rgbBlueCheckBox = new QCheckBox("Blue");
   rgbGridLayout->addWidget(rgbBlueCheckBox, BLUE_ROW, CHECK_COLUMN);
   QObject::connect(rgbBlueCheckBox, SIGNAL(clicked()),
                    this, SLOT(readRgbPaintSelections()));
   rgbBlueNameLabel = new QLabel("                    ");
   rgbBlueNameLabel->setFixedSize(rgbBlueNameLabel->sizeHint());
   rgbGridLayout->addWidget(rgbBlueNameLabel, BLUE_ROW, NAME_COLUMN);
   rgbBlueThreshLineEdit = new QLineEdit;
   rgbBlueThreshLineEdit->setFixedSize(qs);
   rgbGridLayout->addWidget(rgbBlueThreshLineEdit, BLUE_ROW, THRESH_COLUMN);
   QObject::connect(rgbBlueThreshLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(readRgbPaintSelections()));
   rgbBlueNegMaxLabel = new QLabel("            ");
   rgbBlueNegMaxLabel->setFixedSize(rgbBlueNegMaxLabel->sizeHint());
   rgbGridLayout->addWidget(rgbBlueNegMaxLabel, BLUE_ROW, NEG_MAX_COLUMN);
   rgbBluePosMaxLabel = new QLabel("            ");
   rgbBluePosMaxLabel->setFixedSize(rgbBluePosMaxLabel->sizeHint());
   rgbGridLayout->addWidget(rgbBluePosMaxLabel, BLUE_ROW, POS_MAX_COLUMN);
   QPushButton* blueCommentPushButton = new QPushButton("?");
   rgbGridLayout->addWidget(blueCommentPushButton, BLUE_ROW, COMMENT_COLUMN);
   blueCommentPushButton->setFixedSize(infoButtonSize);
   blueCommentPushButton->setAutoDefault(false);
   blueCommentPushButton->setToolTip( "Show Blue Comment");
   commentButtonGroup->addButton(blueCommentPushButton);
   
   //
   // Button group fror positive and negative only buttons
   //
   QButtonGroup* displayModeButtonGroup = new QButtonGroup(this);
   displayModeButtonGroup->setExclusive(true);
   QObject::connect(displayModeButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(rgbDisplayModeSelection(int)));
                    
   //
   // Positive and Negative only buttons
   //
   rgbPositiveOnlyRadioButton = new QRadioButton("Positive Only");   
   displayModeButtonGroup->addButton(rgbPositiveOnlyRadioButton,
                                  DisplaySettingsRgbPaint::RGB_DISPLAY_MODE_POSITIVE);
   rgbNegativeOnlyRadioButton = new QRadioButton("Negative Only");   
   displayModeButtonGroup->addButton(rgbNegativeOnlyRadioButton,
                                  DisplaySettingsRgbPaint::RGB_DISPLAY_MODE_NEGATIVE);
   
   //
   // Group Box for RGB File selection and parameters (a QWidget is placed in it so that
   // a layout can be added).
   //
   QGroupBox* rgbFileGroupBox = new QGroupBox("RGB Paint File");
   QVBoxLayout* rgbFileLayout = new QVBoxLayout(rgbFileGroupBox);
   rgbFileLayout->addWidget(rgbSelectionComboBox);
   rgbFileLayout->addLayout(rgbGridLayout);
   
   //
   // Group Box for RGB display mode positive and negative only buttons.
   //
   QGroupBox* rgbDisplayModeGroupBox = new QGroupBox("Display Mode");
   QVBoxLayout* rgbDisplayModeLayout = new QVBoxLayout(rgbDisplayModeGroupBox);
   rgbDisplayModeLayout->addWidget(rgbPositiveOnlyRadioButton);
   rgbDisplayModeLayout->addWidget(rgbNegativeOnlyRadioButton);
   
   //
   // widget for rgb paint items
   //
   pageRgbPaintMain = new QWidget;
   pageWidgetStack->addWidget(pageRgbPaintMain); //, PAGE_NAME_RGB_PAINT);   // adds to dialog

   //
   // Vertical Box Layout for all rgb paint items
   //
   QVBoxLayout* rgbLayout = new QVBoxLayout(pageRgbPaintMain);
   rgbLayout->addWidget(rgbFileGroupBox, 0, Qt::AlignLeft);
   rgbLayout->addWidget(rgbDisplayModeGroupBox, 0, Qt::AlignLeft);
   rgbLayout->addStretch();
}

/**
 * Called when display mode selected on RGB panel.
 */
void
GuiDisplayControlDialog::rgbDisplayModeSelection(int itemNumber)
{
   DisplaySettingsRgbPaint* dsrp = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();
   dsrp->setDisplayMode(static_cast<DisplaySettingsRgbPaint::RGB_DISPLAY_MODE>(itemNumber));
   readRgbPaintSelections();
}

/**
 * RGB "?" comment pushbutton slot
 */
void
GuiDisplayControlDialog::rgbPaintCommentSelection(int buttonNum)
{
   DisplaySettingsRgbPaint* dsrp = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();
   const int col = dsrp->getSelectedColumn(surfaceModelIndex);
   RgbPaintFile* rpf = theMainWindow->getBrainSet()->getRgbPaintFile();
   switch(buttonNum) {
      case 0:
         displayDataInfoDialog(rpf->getFileTitle(), rpf->getCommentRed(col));
         break;
      case 1:
         displayDataInfoDialog(rpf->getFileTitle(), rpf->getCommentGreen(col));
         break;
      case 2:
         displayDataInfoDialog(rpf->getFileTitle(), rpf->getCommentBlue(col));
         break;
   }
}

/**
 * read rgb paint L-to-L, R-to-R.
 */
void 
GuiDisplayControlDialog::readRgbPaintL2LR2R()
{
   DisplaySettingsRgbPaint* dsrp = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();
   dsrp->setApplySelectionToLeftAndRightStructuresFlag(rgbApplySelectionToLeftAndRightStructuresFlagCheckBox->isChecked());
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/** 
 * Read the RGB paint selections from the dialog.
 */
void
GuiDisplayControlDialog::readRgbPaintSelections()
{
   if (pageRgbPaintMain == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   DisplaySettingsRgbPaint* dsrp = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();

   dsrp->setRedEnabled(rgbRedCheckBox->isChecked());
   dsrp->setGreenEnabled(rgbGreenCheckBox->isChecked());
   dsrp->setBlueEnabled(rgbBlueCheckBox->isChecked());

   dsrp->setThresholds(rgbRedThreshLineEdit->text().toFloat(),
                       rgbGreenThreshLineEdit->text().toFloat(),
                       rgbBlueThreshLineEdit->text().toFloat());
                       
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * update rgb paint overlay/underlay selections.
 */
void 
GuiDisplayControlDialog::updateRgbPaintOverlayUnderlaySelection()      
{
   updatePageSelectionComboBox();
   
   if (pageOverlayUnderlaySurface == NULL) {
      return;
   }
   DisplaySettingsRgbPaint* dsrp = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();
   RgbPaintFile* rpf = theMainWindow->getBrainSet()->getRgbPaintFile();
   rgbPaintSelectionComboBox->clear();
   rgbPaintSelectionComboBox->setEnabled(validRgbPaintData);
   primaryOverlayRgbPaintButton->setEnabled(validRgbPaintData);
   secondaryOverlayRgbPaintButton->setEnabled(validRgbPaintData);
   underlayRgbPaintButton->setEnabled(validRgbPaintData);
   rgbPaintSelectionLabel->setEnabled(validRgbPaintData);
   rgbPaintInfoPushButton->setEnabled(validRgbPaintData);
   const int numCols = rpf->getNumberOfColumns();
   bool valid = false;
   if (numCols > 0) {
      valid = true;
      for (int i = 0; i < numCols; i++) {
         rgbPaintSelectionComboBox->addItem(rpf->getColumnName(i));
      }
      
      const int column = dsrp->getSelectedColumn(surfaceModelIndex);
      if ((column >= 0) && (column < numCols)) {
         rgbPaintSelectionComboBox->setCurrentIndex(column);
      }
   }
}

/**
 * Update all rgb paint items in the dialog.
 */
void
GuiDisplayControlDialog::updateRgbPaintItems()
{
   updatePageSelectionComboBox();
   updateRgbPaintOverlayUnderlaySelection();
   
   if (pageRgbPaintMain == NULL) {
      return;
   }
   
   DisplaySettingsRgbPaint* dsrp = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();
   rgbApplySelectionToLeftAndRightStructuresFlagCheckBox->setChecked(dsrp->getApplySelectionToLeftAndRightStructuresFlag());
   RgbPaintFile* rpf = theMainWindow->getBrainSet()->getRgbPaintFile();
   
   rgbSelectionComboBox->clear();
   
   const int numCols = rpf->getNumberOfColumns();
   bool valid = false;
   if (numCols > 0) {
      valid = true;
      for (int i = 0; i < numCols; i++) {
         rgbSelectionComboBox->addItem(rpf->getColumnName(i));
      }
      
      const int column = dsrp->getSelectedColumn(surfaceModelIndex);
      if ((column >= 0) && (column < numCols)) {
         rgbSelectionComboBox->setCurrentIndex(column);
         
         rgbSelectionComboBox->setToolTip(
                       rpf->getColumnName(column));
                       
         rgbRedNameLabel->setText(rpf->getTitleRed(column));
         rgbGreenNameLabel->setText(rpf->getTitleGreen(column));
         rgbBlueNameLabel->setText(rpf->getTitleBlue(column));
         
         float minScale, maxScale;
         rpf->getScaleRed(column, minScale, maxScale);
         rgbRedNegMaxLabel->setText(QString::number(minScale, 'f', 3));
         rgbRedPosMaxLabel->setText(QString::number(maxScale, 'f', 3));

         rpf->getScaleGreen(column, minScale, maxScale);
         rgbGreenNegMaxLabel->setText(QString::number(minScale, 'f', 3));
         rgbGreenPosMaxLabel->setText(QString::number(maxScale, 'f', 3));

         rpf->getScaleBlue(column, minScale, maxScale);
         rgbBlueNegMaxLabel->setText(QString::number(minScale, 'f', 3));
         rgbBluePosMaxLabel->setText(QString::number(maxScale, 'f', 3));
      }
   }
   
   switch(dsrp->getDisplayMode()) {
      case DisplaySettingsRgbPaint::RGB_DISPLAY_MODE_POSITIVE:
         rgbPositiveOnlyRadioButton->setChecked(true);
         break;
      case DisplaySettingsRgbPaint::RGB_DISPLAY_MODE_NEGATIVE:
         rgbNegativeOnlyRadioButton->setChecked(true);
         break;
   }
   
   rgbRedCheckBox->setChecked(dsrp->getRedEnabled());
   rgbGreenCheckBox->setChecked(dsrp->getGreenEnabled());
   rgbBlueCheckBox->setChecked(dsrp->getBlueEnabled());

   float redThresh, greenThresh, blueThresh;
   dsrp->getThresholds(redThresh, greenThresh, blueThresh);
   rgbRedThreshLineEdit->setText(QString::number(redThresh, 'f', 3));
   rgbGreenThreshLineEdit->setText(QString::number(greenThresh, 'f', 3));
   rgbBlueThreshLineEdit->setText(QString::number(blueThresh, 'f', 3));

   rgbSelectionComboBox->setEnabled(validRgbPaintData);
   
   pageRgbPaintMain->setEnabled(validRgbPaintData);
   
   updatePageSelectionComboBox();
}

/**
 * Cell page containing main items.
 */
void
GuiDisplayControlDialog::createCellMainPage()
{
   showCellsCheckBox = new QCheckBox("Show Cells");
   QObject::connect(showCellsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(showCellsToggleSlot(bool)));
   
   showVolumeCellsCheckBox = new QCheckBox("Show Volume Cells");
   QObject::connect(showVolumeCellsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readCellMainPage()));
   
   showRaisedCellCheckBox = new QCheckBox("Show Raised Cells (flat surfaces only)");
   QObject::connect(showRaisedCellCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readCellMainPage()));
    
   showCorrectHemisphereCellsCheckBox = new QCheckBox("Show Cells on Correct Hemisphere Only");
   QObject::connect(showCorrectHemisphereCellsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readCellMainPage()));
   
   cellDisplayModeComboBox = new QComboBox;
   cellDisplayModeComboBox->insertItem(DisplaySettingsCells::CELL_DISPLAY_MODE_SHOW_ALL,
                                       "Show Deep and Superficial Cells");
   cellDisplayModeComboBox->insertItem(DisplaySettingsCells::CELL_DISPLAY_MODE_SHOW_DEEP_ONLY,
                                       "Show Deep Cells Only");
   cellDisplayModeComboBox->insertItem(DisplaySettingsCells::CELL_DISPLAY_MODE_SHOW_SUPERFICIAL_ONLY,
                                       "Show Superficial Cells Only");
   cellDisplayModeComboBox->setFixedSize(cellDisplayModeComboBox->sizeHint());
   QObject::connect(cellDisplayModeComboBox, SIGNAL(activated(int)),
                    this, SLOT(readCellMainPage()));
                    
   QLabel* symbolLabel = new QLabel("Symbol Override ");
   cellSymbolOverrideComboBox = new QComboBox;
   std::vector<QString> cellOverrideLabels;
   ColorFile::ColorStorage::getAllSymbolTypesAsStrings(cellOverrideLabels);
   for (unsigned int i = 0; i < cellOverrideLabels.size(); i++) {
      cellSymbolOverrideComboBox->addItem(cellOverrideLabels[i]);
   }
   QObject::connect(cellSymbolOverrideComboBox, SIGNAL(activated(int)),
                    this, SLOT(readCellMainPage()));
   QHBoxLayout* symbolLayout = new QHBoxLayout;
   symbolLayout->addWidget(symbolLabel);
   symbolLayout->addWidget(cellSymbolOverrideComboBox);
   symbolLayout->addStretch();
   
   QLabel* opacityLabel = new QLabel("Opacity");
   cellOpacityDoubleSpinBox = new QDoubleSpinBox;
   cellOpacityDoubleSpinBox->setMinimum(0.0);
   cellOpacityDoubleSpinBox->setMaximum(1.0);
   cellOpacityDoubleSpinBox->setSingleStep(0.01);
   cellOpacityDoubleSpinBox->setDecimals(3);
   QObject::connect(cellOpacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readCellMainPage()));
   QHBoxLayout* opacityLayout = new QHBoxLayout;
   opacityLayout->addWidget(opacityLabel);
   opacityLayout->addWidget(cellOpacityDoubleSpinBox);
   opacityLayout->addStretch();
                    
   QLabel* spinLabel = new QLabel("Cell Size");
   cellSizeSpinBox = new QDoubleSpinBox;
   cellSizeSpinBox->setMinimum(0.001);
   cellSizeSpinBox->setMaximum(maxPointSize);
   cellSizeSpinBox->setSingleStep(0.5);
   cellSizeSpinBox->setDecimals(3);
   cellSizeSpinBox->setSizePolicy(QtUtilities::fixedSizePolicy());
   QObject::connect(cellSizeSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readCellMainPage()));
   QHBoxLayout* sizeLayout = new QHBoxLayout;
   sizeLayout->addWidget(spinLabel);
   sizeLayout->addWidget(cellSizeSpinBox);
   sizeLayout->addStretch();

   QLabel* distanceLabel = new QLabel("Distance");   
   cellDistSpinBox = new QDoubleSpinBox;
   cellDistSpinBox->setMinimum(0.0);
   cellDistSpinBox->setMaximum(1000.0);
   cellDistSpinBox->setSingleStep(10.0);
   cellDistSpinBox->setDecimals(1);
   cellDistSpinBox->setValue(1000.0);
   QObject::connect(cellDistSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readCellMainPage()));
   cellDistSpinBox->setToolTip("Cells whose distances to the surface are\n"
                                  "less than this value will be displayed.\n"
                                  "The cells must have been projected.");
   QHBoxLayout* distanceLayout = new QHBoxLayout;
   distanceLayout->addWidget(distanceLabel);
   distanceLayout->addWidget(cellDistSpinBox);
   distanceLayout->addStretch();
   
   //
   // Display color key button
   //
   QPushButton* colorKeyPushButton = new QPushButton("Display Color Key...");
   colorKeyPushButton->setFixedSize(colorKeyPushButton->sizeHint());
   colorKeyPushButton->setAutoDefault(false);
   QObject::connect(colorKeyPushButton, SIGNAL(clicked()),
                    theMainWindow, SLOT(displayCellColorKey()));
                    
   //
   // Vertical Box Layout for all cell items
   //
   pageCellsMain = new QWidget;
   QVBoxLayout* cellSubPageMainLayout = new QVBoxLayout(pageCellsMain);
   cellSubPageMainLayout->addWidget(showCellsCheckBox);
   cellSubPageMainLayout->addWidget(showVolumeCellsCheckBox);
   cellSubPageMainLayout->addWidget(showRaisedCellCheckBox);
   //cellSubPageMainLayout->addWidget(showLeftHemisphereCellCheckBox);
   //cellSubPageMainLayout->addWidget(showRightHemisphereCellCheckBox);
   cellSubPageMainLayout->addWidget(showCorrectHemisphereCellsCheckBox);
   cellSubPageMainLayout->addWidget(cellDisplayModeComboBox);
   cellSubPageMainLayout->addLayout(symbolLayout);
   cellSubPageMainLayout->addLayout(sizeLayout);
   cellSubPageMainLayout->addLayout(opacityLayout);
   cellSubPageMainLayout->addLayout(distanceLayout);
   cellSubPageMainLayout->addWidget(colorKeyPushButton);
   cellSubPageMainLayout->addStretch();

   pageWidgetStack->addWidget(pageCellsMain);
   
   pageCellsMainWidgetGroup = new WuQWidgetGroup(this);
   pageCellsMainWidgetGroup->addWidget(showCellsCheckBox);
   pageCellsMainWidgetGroup->addWidget(showVolumeCellsCheckBox);
   pageCellsMainWidgetGroup->addWidget(showRaisedCellCheckBox);
   pageCellsMainWidgetGroup->addWidget(showCorrectHemisphereCellsCheckBox);
   pageCellsMainWidgetGroup->addWidget(cellDisplayModeComboBox);
   pageCellsMainWidgetGroup->addWidget(cellSymbolOverrideComboBox);
   pageCellsMainWidgetGroup->addWidget(cellOpacityDoubleSpinBox);
   pageCellsMainWidgetGroup->addWidget(cellSizeSpinBox);
   pageCellsMainWidgetGroup->addWidget(cellDistSpinBox);
   pageCellsMainWidgetGroup->addWidget(colorKeyPushButton);
}

/** 
 * Cell page containing class selections.
 */
void 
GuiDisplayControlDialog::createCellClassPage()
{
   //
   // Vertical Box Layout for all cell items
   //
   pageCellsClass = new QWidget; 
   cellSubPageClassLayout = new QVBoxLayout(pageCellsClass);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* cellClassAllOnButton = new QPushButton("All On");
   cellClassAllOnButton->setAutoDefault(false);
   QObject::connect(cellClassAllOnButton, SIGNAL(clicked()),
                    this, SLOT(cellClassAllOn()));
   QPushButton* cellClassAllOffButton = new QPushButton("All Off");
   cellClassAllOffButton->setAutoDefault(false);
   QObject::connect(cellClassAllOffButton, SIGNAL(clicked()),
                    this, SLOT(cellClassAllOff()));
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(cellClassAllOnButton);
   allOnOffLayout->addWidget(cellClassAllOffButton);
   allOnOffLayout->addStretch();
   cellSubPageClassLayout->addLayout(allOnOffLayout);

   pageCellsClassWidgetGroup = new WuQWidgetGroup(this);
   pageCellsClassWidgetGroup->addWidget(cellClassAllOnButton);
   pageCellsClassWidgetGroup->addWidget(cellClassAllOffButton);
   
   createAndUpdateCellClassCheckBoxes();

   pageWidgetStack->addWidget(pageCellsClass);
}

/**
 * Create and update cell class checkboxes.  Because the number of cell may change,
 * this method may update and change the label on existing checkboxes, add new 
 * checkboxes, and hide existing checkboxes if the number of checkboxes is greater
 * than the number of cell classes.
 */
void
GuiDisplayControlDialog::createAndUpdateCellClassCheckBoxes()
{
   CellProjectionFile* cf = theMainWindow->getBrainSet()->getCellProjectionFile();
   if (cf != NULL) {
      numValidCellClasses = cf->getNumberOfCellClasses();
   }
   else {
      numValidCellClasses = 0;
   }
      
   const int numExistingCheckBoxes = static_cast<int>(cellClassCheckBoxes.size());
   
   if (cellClassGridLayout == NULL) {
      QWidget* classWidget = new QWidget; 
      cellClassGridLayout = new QGridLayout(classWidget);
      const int rowStretchNumber    = 15000;
      cellClassGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, Qt::AlignLeft);
      cellClassGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      cellSubPageClassLayout->addWidget(classWidget);
   }
   
   if (cellClassButtonGroup == NULL) {
      cellClassButtonGroup = new QButtonGroup(this);
      cellClassButtonGroup->setExclusive(false);
      QObject::connect(cellClassButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readCellClassPage()));
   }
   
   //
   // Update exising checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidCellClasses) {
         cellClassCheckBoxes[i]->setText(cf->getCellClassNameByIndex(i));
         cellClassCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidCellClasses; j++) {
      QCheckBox* cb = new QCheckBox(cf->getCellClassNameByIndex(j));
      cellClassCheckBoxes.push_back(cb);
      cellClassButtonGroup->addButton(cb, j);
      cellClassGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
      pageCellsClassWidgetGroup->addWidget(cb);
   }
   
   //
   // Hide existing checkboxes that are not needed.
   //
   for (int k = numValidCellClasses; k < numExistingCheckBoxes; k++) {
      cellClassCheckBoxes[k]->hide();
   }
}

/**
 * This slot is called when the cell class all on button is pressed.
 */
void
GuiDisplayControlDialog::cellClassAllOn()
{
   CellProjectionFile* cf = theMainWindow->getBrainSet()->getCellProjectionFile();
   if (cf != NULL) {
      cf->setAllCellClassStatus(true);
   }
   updateCellItems();
   readCellClassPage();
}

/**
 * This slot is called when the cell class all off button is pressed.
 */
void
GuiDisplayControlDialog::cellClassAllOff()
{
   CellProjectionFile* cf = theMainWindow->getBrainSet()->getCellProjectionFile();
   if (cf != NULL) {
      cf->setAllCellClassStatus(false);
   }
   updateCellItems();
   readCellClassPage();
}

/**
 * Cell page containing color selections.
 */
void
GuiDisplayControlDialog::createCellColorPage()
{
   //
   // Vertical Box Layout for all cell items
   //
   pageCellsColor = new QWidget; 
   cellSubPageColorLayout = new QVBoxLayout(pageCellsColor);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* cellColorAllOnButton = new QPushButton("All On");
   cellColorAllOnButton->setAutoDefault(false);
   QObject::connect(cellColorAllOnButton, SIGNAL(clicked()),
                    this, SLOT(cellColorAllOn()));
   QPushButton* cellColorAllOffButton = new QPushButton("All Off");
   cellColorAllOffButton->setAutoDefault(false);
   QObject::connect(cellColorAllOffButton, SIGNAL(clicked()),
                    this, SLOT(cellColorAllOff()));
  QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(cellColorAllOnButton);
   allOnOffLayout->addWidget(cellColorAllOffButton);
   allOnOffLayout->addStretch();
   cellSubPageColorLayout->addLayout(allOnOffLayout);

   pageCellsColorWidgetGroup = new WuQWidgetGroup(this);
   pageCellsColorWidgetGroup->addWidget(cellColorAllOnButton);
   pageCellsColorWidgetGroup->addWidget(cellColorAllOffButton);
   
   createAndUpdateCellColorCheckBoxes();

   pageWidgetStack->addWidget(pageCellsColor);
}

/**
 * Create and update cell color checkboxes.  Because the number of colors may change,
 * this method may update and change the label on existing checkboxes, add new 
 * checkboxes, and hide existing checkboxes if the number of checkboxes is greater
 * than the number of cell colors.
 */
void
GuiDisplayControlDialog::createAndUpdateCellColorCheckBoxes()
{
   CellColorFile* cellColors = theMainWindow->getBrainSet()->getCellColorFile();
   numValidCellColors = cellColors->getNumberOfColors();
   
   const int numExistingCheckBoxes = static_cast<int>(cellColorCheckBoxes.size());
   
   if (cellColorGridLayout == NULL) {
      QWidget* colorWidget = new QWidget; 
      cellColorGridLayout = new QGridLayout(colorWidget);
      const int rowStretchNumber    = 15000;
      cellColorGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, Qt::AlignLeft);
      cellColorGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      cellSubPageColorLayout->addWidget(colorWidget);
   }
   
   if (cellColorButtonGroup == NULL) {
      cellColorButtonGroup = new QButtonGroup(this);
      cellColorButtonGroup->setExclusive(false);
      QObject::connect(cellColorButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readCellColorPage()));
   }
   
   //
   // Update exising checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidCellColors) {
         cellColorCheckBoxes[i]->setText(cellColors->getColorNameByIndex(i));
         cellColorCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidCellColors; j++) {
      QCheckBox* cb = new QCheckBox(cellColors->getColorNameByIndex(j));
      cellColorCheckBoxes.push_back(cb);
      cellColorButtonGroup->addButton(cb, j);
      cellColorGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
      pageCellsColorWidgetGroup->addWidget(cb);
   }
   
   //
   // Hide existing checkboxes that are not needed.
   //
   for (int k = numValidCellColors; k < numExistingCheckBoxes; k++) {
      cellColorCheckBoxes[k]->hide();
   }
}

/**
 * This slot is called when the cell color all on button is pressed.
 */
void
GuiDisplayControlDialog::cellColorAllOn()
{
   CellColorFile* cellColors = theMainWindow->getBrainSet()->getCellColorFile();
   cellColors->setAllSelectedStatus(true);
   updateCellItems();
   readCellColorPage();
}

/**
 * This slot is called when the cell color all off button is pressed.
 */
void
GuiDisplayControlDialog::cellColorAllOff()
{
   CellColorFile* cellColors = theMainWindow->getBrainSet()->getCellColorFile();
   cellColors->setAllSelectedStatus(false);
   updateCellItems();
   readCellColorPage();
}

/**
 * upate cell main page.
 */
void 
GuiDisplayControlDialog::updateCellMainPage()
{
   if (pageCellsMain == NULL) {
      return;
   }
   pageCellsMainWidgetGroup->blockSignals(true);
   
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCells();
   
   showCellsCheckBox->setChecked(dsc->getDisplayCells());
   showVolumeCellsCheckBox->setChecked(dsc->getDisplayVolumeCells());
   showRaisedCellCheckBox->setChecked(dsc->getDisplayFlatCellsRaised());
   cellOpacityDoubleSpinBox->setValue(dsc->getOpacity());
   
   showCorrectHemisphereCellsCheckBox->setChecked(dsc->getDisplayCellsOnCorrectHemisphereOnly());
   
   cellDisplayModeComboBox->setCurrentIndex(dsc->getDisplayMode());
   cellSymbolOverrideComboBox->setCurrentIndex(dsc->getSymbolOverride());
   cellSizeSpinBox->setValue(dsc->getDrawSize());
   cellDistSpinBox->setValue(dsc->getDistanceToSurfaceLimit());

   pageCellsMain->setEnabled(validCellData);
   
   pageCellsMainWidgetGroup->blockSignals(false);
}

/**
 * update cell class page.
 */
void 
GuiDisplayControlDialog::updateCellClassPage(const bool cellsWereChanged)
{
   if (pageCellsClass == NULL) {
      return;
   }
   if (cellsWereChanged) {
      createAndUpdateCellClassCheckBoxes();
   }
   pageCellsClassWidgetGroup->blockSignals(true);

   CellProjectionFile* cf = theMainWindow->getBrainSet()->getCellProjectionFile();
   if (cf != NULL) {
      const int numClasses = cf->getNumberOfCellClasses();   
      if (numClasses == numValidCellClasses) {
         for (int i = 0; i < numValidCellClasses; i++) {
            cellClassCheckBoxes[i]->setChecked(cf->getCellClassSelectedByIndex(i));
         }
      }
      else {
         std::cerr << "Number of cell class checkboxes does not equal number of cell classes."
                   << std::endl;
      }
   }
   pageCellsClass->setEnabled(validCellData);
   pageCellsClassWidgetGroup->blockSignals(false);
}

/**
 * update cell color page.
 */
void 
GuiDisplayControlDialog::updateCellColorPage(const bool cellsWereChanged)
{
   if (pageCellsColor == NULL) {
      return;
   }
   if (cellsWereChanged) {
      createAndUpdateCellColorCheckBoxes();
   }
   pageCellsColorWidgetGroup->blockSignals(true);

   CellColorFile* cellColors = theMainWindow->getBrainSet()->getCellColorFile();
   const int numColors = cellColors->getNumberOfColors();   
   if (numColors == numValidCellColors) {
      for (int i = 0; i < numValidCellColors; i++) {
         cellColorCheckBoxes[i]->setChecked(cellColors->getSelected(i));
      }
   }
   else {
      std::cerr << "Number of cell color checkboxes does not equal number of cell colors."
                << std::endl;
   }

   pageCellsColor->setEnabled(validCellData);
   pageCellsColorWidgetGroup->blockSignals(false);
}
      
/**
 * Update all of the cell items in the dialog
 */
void
GuiDisplayControlDialog::updateCellItems(const bool filesChanged)
{
   updatePageSelectionComboBox();

   if (pageOverlayUnderlaySurface != NULL) {
      DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCells();
      layersCellsCheckBox->setChecked(dsc->getDisplayCells());
      layersCellsCheckBox->setEnabled(validCellData);
   }
   
   updateCellMainPage();
   updateCellClassPage(filesChanged);
   updateCellColorPage(filesChanged);
   updatePageSelectionComboBox();
}

/**
 * called when cells selected on cells page or overlay/underlay surface page.
 */
void 
GuiDisplayControlDialog::showCellsToggleSlot(bool b)
{
   if (pageCellsMain != NULL) {
      showCellsCheckBox->blockSignals(true);
      showCellsCheckBox->setChecked(b);
      showCellsCheckBox->blockSignals(false);
   }
   if (pageOverlayUnderlaySurface != NULL) {
      layersCellsCheckBox->blockSignals(true);
      layersCellsCheckBox->setChecked(b);
      layersCellsCheckBox->blockSignals(false);
   }
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCells();
   dsc->setDisplayCells(b);
   dsc->determineDisplayedCells();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
   //readCellMainPage();
   //readCellColorPage();
   //readCellClassPage();
}      

/**
 * read the cell main page.
 */
void 
GuiDisplayControlDialog::readCellMainPage()
{
   if (pageCellsMain == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCells();
   
   dsc->setDisplayCells(showCellsCheckBox->isChecked());
   dsc->setDisplayVolumeCells(showVolumeCellsCheckBox->isChecked());
   dsc->setDisplayFlatCellsRaised(showRaisedCellCheckBox->isChecked());
   
   //dsc->setDisplayLeftHemisphereCells(showLeftHemisphereCellCheckBox->isChecked());
   //dsc->setDisplayRightHemisphereCells(showRightHemisphereCellCheckBox->isChecked());
   dsc->setDisplayCellsOnCorrectHemisphereOnly(showCorrectHemisphereCellsCheckBox->isChecked());
   
   dsc->setDisplayMode(static_cast<DisplaySettingsCells::CELL_DISPLAY_MODE>(
                                cellDisplayModeComboBox->currentIndex()));
   dsc->setSymbolOverride(static_cast<ColorFile::ColorStorage::SYMBOL>(
                                cellSymbolOverrideComboBox->currentIndex()));
   dsc->setOpacity(cellOpacityDoubleSpinBox->value());
   dsc->setDrawSize(cellSizeSpinBox->value());
   dsc->setDistanceToSurfaceLimit(cellDistSpinBox->value());
   
   dsc->determineDisplayedCells();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * read the cell color page.
 */
void 
GuiDisplayControlDialog::readCellColorPage()
{
   if (pageCellsColor == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   CellColorFile* cellColors = theMainWindow->getBrainSet()->getCellColorFile();
   const int numColors = cellColors->getNumberOfColors();   
   if (numColors == numValidCellColors) {
      for (int i = 0; i < numValidCellColors; i++) {
         cellColors->setSelected(i, cellColorCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of cell color checkboxes does not equal number of cell colors."
                << std::endl;
   }
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCells();
   dsc->determineDisplayedCells();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * read the cell class page.
 */
void 
GuiDisplayControlDialog::readCellClassPage()
{
   if (pageCellsClass == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   CellProjectionFile* cf = theMainWindow->getBrainSet()->getCellProjectionFile();
   int numClasses = 0;
   if (cf != NULL) {
      numClasses = cf->getNumberOfCellClasses();
   }
   if (numClasses == numValidCellClasses) {
      for (int i = 0; i < numValidCellClasses; i++) {
         cf->setCellClassSelectedByIndex(i, cellClassCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of cell class checkboxes does not equal number of cell classes."
                << std::endl;
   }
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCells();
   dsc->determineDisplayedCells();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}
      
/**
 * Update all items in the dialog.  Typically called after loading
 * a spec file.
 */
void
GuiDisplayControlDialog::updateAllItemsInDialog(const bool filesChanged,
                                                const bool updateResultOfSceneChange)
{
   updatePageSelectionComboBox();
   skipScenePageUpdate = updateResultOfSceneChange;
   
   updateOverlayUnderlayItems();
   updateArealEstimationItems();
   updateBorderItems(filesChanged);
   updateCellItems(filesChanged);
   updateCocomacItems();
   updateContourItems(filesChanged);
   updateDeformationFieldPage();
   updateGeodesicItems();
   updateLatLonItems();
   updateRegionItems();
   updateRgbPaintItems();
   updateSceneItems();
   updateShapeItems();
   updateFociItems(filesChanged);
   updateImagesItems();
   updateMetricItems();
   updateMiscItems();
   updateModelItems();
   updatePaintItems();
   updateProbAtlasSurfaceItems(filesChanged);
   updateProbAtlasVolumeItems(filesChanged);
   updateSurfaceAndVolumeItems();
   updateSurfaceVectorItems();
   updateTopographyItems();
   updateVolumeItems();
   
   updatePageSelectionComboBox();
   
   skipScenePageUpdate = false;
   
   QString title("Display Control");
   const QString specName(theMainWindow->getBrainSet()->getSpecFileName());
   if (specName.isEmpty() == false) {
      title.append(" - ");
      title.append(FileUtilities::basename(specName));
   }
   setWindowTitle(title);
}

/**
 * create the shape selection page.
 */
void 
GuiDisplayControlDialog::createShapeSelectionPage()
{
   pageSurfaceShapeSelections = new QWidget;
   pageWidgetStack->addWidget(pageSurfaceShapeSelections);
                                                 
   createAndUpdateSurfaceShapeSelections();
}

/**
 * create the shape settings page.
 */
void 
GuiDisplayControlDialog::createShapeSettingsPage()
{
   //-----------------------------------------------------------------------------
   //
   // Selected column information.
   //   
   QLabel* minMaxLabel = new QLabel("Min/Max ");
   shapeViewMinimumLabel = new QLabel("");
   shapeViewMaximumLabel = new QLabel("");
   QLabel* mappingLabel = new QLabel("Mapping ");
   shapeMinimumMappingDoubleSpinBox = new QDoubleSpinBox;
   shapeMinimumMappingDoubleSpinBox->setMinimum(-1000000.0);
   shapeMinimumMappingDoubleSpinBox->setMaximum( 1000000.0);
   shapeMinimumMappingDoubleSpinBox->setSingleStep(1.0);
   shapeMinimumMappingDoubleSpinBox->setDecimals(6);
   QObject::connect(shapeMinimumMappingDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readShapeSettings()));
   shapeMaximumMappingDoubleSpinBox = new QDoubleSpinBox;
   shapeMaximumMappingDoubleSpinBox->setMinimum(-1000000.0);
   shapeMaximumMappingDoubleSpinBox->setMaximum( 1000000.0);
   shapeMaximumMappingDoubleSpinBox->setSingleStep(1.0);
   shapeMaximumMappingDoubleSpinBox->setDecimals(6);
   QObject::connect(shapeMaximumMappingDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readShapeSettings()));
   QGridLayout* selectionGridLayout = new QGridLayout;
   selectionGridLayout->addWidget(minMaxLabel, 0, 0);
   selectionGridLayout->addWidget(shapeViewMinimumLabel, 0, 1);
   selectionGridLayout->addWidget(shapeViewMaximumLabel, 0, 2);
   selectionGridLayout->addWidget(mappingLabel, 1, 0);
   selectionGridLayout->addWidget(shapeMinimumMappingDoubleSpinBox, 1, 1);
   selectionGridLayout->addWidget(shapeMaximumMappingDoubleSpinBox, 1, 2);

   //
   // Histogram button
   //
   QPushButton* histoPushButton = new QPushButton("Histogram...");
   histoPushButton->setAutoDefault(false);
   histoPushButton->setFixedSize(histoPushButton->sizeHint());
   QObject::connect(histoPushButton, SIGNAL(clicked()),
                    this, SLOT(surfaceShapeHistogram()));

   //
   // Group box and layouts
   //
   QVBoxLayout* selectionLeftLayout = new QVBoxLayout;
   selectionLeftLayout->addLayout(selectionGridLayout);
   selectionLeftLayout->addWidget(histoPushButton);
   QGroupBox* selectionGroupBox = new QGroupBox("Selected Column");
   QHBoxLayout* selectionGroupLayout = new QHBoxLayout(selectionGroupBox);
   selectionGroupLayout->addLayout(selectionLeftLayout);
   selectionGroupLayout->addStretch();
   
   //-----------------------------------------------------------------------------
   //
   // Color map selections
   //
   shapeColorMapGrayRadioButton = new QRadioButton("Gray");
   shapeColorMapOrangeYellowRadioButton = new QRadioButton("Orange-Yellow");
   shapeColorMapPaletteRadioButton = new QRadioButton("Palette");
   shapeColorMapPaletteComboBox = new QComboBox;   
   QObject::connect(shapeColorMapPaletteComboBox, SIGNAL(activated(int)),
                    this, SLOT(readShapeSettings()));
                    
   //
   // Button group to keep radio buttons mutually exclusive
   //
   palColorMapButtonGroup = new QButtonGroup(this);
   palColorMapButtonGroup->setExclusive(true);
   palColorMapButtonGroup->addButton(shapeColorMapGrayRadioButton, DisplaySettingsSurfaceShape::SURFACE_SHAPE_COLOR_MAP_GRAY);
   palColorMapButtonGroup->addButton(shapeColorMapOrangeYellowRadioButton, DisplaySettingsSurfaceShape::SURFACE_SHAPE_COLOR_MAP_ORANGE_YELLOW);
   palColorMapButtonGroup->addButton(shapeColorMapPaletteRadioButton, DisplaySettingsSurfaceShape::SURFACE_SHAPE_COLOR_MAP_PALETTE);
   QObject::connect(palColorMapButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(shapeColorMapSelection(int)));
  
   //
   // Interpolate palette colors
   //
   shapeColorMapInterpolatePaletteCheckBox = new QCheckBox("Interpolate Palette Colors");
   QObject::connect(shapeColorMapInterpolatePaletteCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readShapeSettings()));
    
   //
   // Display color bar
   //
   shapeDisplayColorBarCheckBox = new QCheckBox("Display Color Bar");
   QObject::connect(shapeDisplayColorBarCheckBox, SIGNAL(clicked()),
                    this, SLOT(readShapeSettings()));

   //
   // Color mapping items
   //
   QHBoxLayout* paletteBoxLayout = new QHBoxLayout;
   QLabel* paletteDummy = new QLabel(" ");
   paletteBoxLayout->addWidget(shapeColorMapPaletteRadioButton);
   paletteBoxLayout->addWidget(shapeColorMapPaletteComboBox);
   paletteBoxLayout->addWidget(paletteDummy);
   paletteBoxLayout->setStretchFactor(paletteDummy, 1000);
   QGroupBox* colorMappingGroupBox = new QGroupBox("Color Mapping");
   QVBoxLayout* colorMappingGroupLayout = new QVBoxLayout(colorMappingGroupBox);
   colorMappingGroupLayout->addWidget(shapeColorMapGrayRadioButton);
   colorMappingGroupLayout->addWidget(shapeColorMapOrangeYellowRadioButton);
   colorMappingGroupLayout->addLayout(paletteBoxLayout);
   colorMappingGroupLayout->addWidget(shapeColorMapInterpolatePaletteCheckBox);
   colorMappingGroupLayout->addWidget(shapeDisplayColorBarCheckBox);
   
   //-----------------------------------------------------------------------------
   //
   // Uncertainty column
   //
   shapeNodeIdDeviationComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                             GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                             false,
                                             false,
                                             false);
   shapeNodeIdDeviationComboBox->setNoneSelectionLabel("Disabled");
   QObject::connect(shapeNodeIdDeviationComboBox, SIGNAL(activated(int)),
                    this, SLOT(readShapeSettings()));
   shapeNodeIdDeviationGroupBox = new QGroupBox("Node ID Deviation");
   shapeNodeIdDeviationGroupBox->setCheckable(true);
   QObject::connect(shapeNodeIdDeviationGroupBox, SIGNAL(toggled(bool)),
                    shapeNodeIdDeviationComboBox, SLOT(setEnabled(bool)));                 
   QObject::connect(shapeNodeIdDeviationGroupBox, SIGNAL(toggled(bool)),
                    this, SLOT(readShapeSettings()));                 
   QVBoxLayout* shapeNodeLayout = new QVBoxLayout(shapeNodeIdDeviationGroupBox);
   shapeNodeLayout->addWidget(shapeNodeIdDeviationComboBox);
   
   //
   // Place all items in a vertical box 
   //
   pageSurfaceShapeSettings = new QWidget;
   pageWidgetStack->addWidget(pageSurfaceShapeSettings);
   QVBoxLayout* shapeSettingsLayout = new QVBoxLayout(pageSurfaceShapeSettings);
   shapeSettingsLayout->addWidget(selectionGroupBox);
   shapeSettingsLayout->addWidget(colorMappingGroupBox);
   shapeSettingsLayout->addWidget(shapeNodeIdDeviationGroupBox);
   pageSurfaceShapeSettings->setFixedHeight(pageSurfaceShapeSettings->sizeHint().height());
   pageSurfaceShapeSettings->setMaximumWidth(400);
   
   pageSurfaceShapeSettingsWidgetGroup = new WuQWidgetGroup(this);
   pageSurfaceShapeSettingsWidgetGroup->addWidget(shapeMinimumMappingDoubleSpinBox);
   pageSurfaceShapeSettingsWidgetGroup->addWidget(shapeMaximumMappingDoubleSpinBox);
   pageSurfaceShapeSettingsWidgetGroup->addWidget(shapeColorMapPaletteComboBox);
   pageSurfaceShapeSettingsWidgetGroup->addWidget(shapeColorMapGrayRadioButton);
   pageSurfaceShapeSettingsWidgetGroup->addWidget(shapeColorMapOrangeYellowRadioButton);
   pageSurfaceShapeSettingsWidgetGroup->addWidget(shapeColorMapPaletteRadioButton);
   pageSurfaceShapeSettingsWidgetGroup->addWidget(shapeColorMapInterpolatePaletteCheckBox);
   pageSurfaceShapeSettingsWidgetGroup->addWidget(shapeDisplayColorBarCheckBox);
   pageSurfaceShapeSettingsWidgetGroup->addWidget(shapeNodeIdDeviationComboBox);
   pageSurfaceShapeSettingsWidgetGroup->addWidget(shapeNodeIdDeviationGroupBox);
}

/**
 * Create and update surface shape selections.  Because the number of columns may change,
 * this method may update and change the labels on existing checkboxes, add new 
 * checkboxes, and hide existing checkboxes.
 */
void
GuiDisplayControlDialog::createAndUpdateSurfaceShapeSelections()
{
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   numValidSurfaceShape = ssf->getNumberOfColumns();
   
   const int numExistingSurfaceShape = static_cast<int>(surfaceShapeViewRadioButtons.size());
   const int nameMinimumWidth = 500;
     
   if (surfaceShapeSubSelectionsLayout == NULL) {
      //
      // Grid layout for surfaceShape selections
      //
      QWidget* shapeSelectionsGridWidget = new QWidget;
      surfaceShapeSelectionGridLayout = new QGridLayout(shapeSelectionsGridWidget);
      //surfaceShapeSelectionGridLayout->setMargin(3);
      surfaceShapeSelectionGridLayout->setSpacing(3);
      surfaceShapeSelectionGridLayout->setColumnMinimumWidth(4, nameMinimumWidth+20);
      surfaceShapeSelectionGridLayout->addWidget(new QLabel("#"),
                                           0, 0, Qt::AlignRight);
      surfaceShapeSelectionGridLayout->addWidget(new QLabel("View"),
                                           0, 1, Qt::AlignHCenter);
      surfaceShapeSelectionGridLayout->addWidget(new QLabel("Cmt"),
                                           0, 2, Qt::AlignHCenter);
      surfaceShapeSelectionGridLayout->addWidget(new QLabel("MD"),
                                           0, 3, Qt::AlignHCenter);
      surfaceShapeSelectionGridLayout->addWidget(new QLabel("Name"),
                                           0, 4, Qt::AlignLeft);
      
      //
      // For stretching on  bottom
      //
      //const int rowStretchNumber    = 15000;
      //surfaceShapeSelectionGridLayout->addWidget(new QLabel(""),
      //                                     rowStretchNumber, 4, 1, 1, Qt::AlignLeft);
                                           
      surfaceShapeSelectionGridLayout->setColumnStretch(0, 0);
      surfaceShapeSelectionGridLayout->setColumnStretch(1, 0);
      surfaceShapeSelectionGridLayout->setColumnStretch(2, 0);
      surfaceShapeSelectionGridLayout->setColumnStretch(3, 0);
      surfaceShapeSelectionGridLayout->setColumnStretch(4, 0);
      //surfaceShapeSelectionGridLayout->setRowStretch(rowStretchNumber, 1000);

      //
      // Create the shape selections layout and push selections to the left
      //
      surfaceShapeSubSelectionsLayout = new QVBoxLayout(pageSurfaceShapeSelections);
      surfaceShapeSubSelectionsLayout->addWidget(shapeSelectionsGridWidget, 100, Qt::AlignLeft | Qt::AlignTop);
   }
   
   //
   // Create the button group for the surfaceShape view buttons.
   //
   if (surfaceShapeViewButtonGroup == NULL) {
      surfaceShapeViewButtonGroup = new QButtonGroup(this);
      surfaceShapeViewButtonGroup->setExclusive(true);
      QObject::connect(surfaceShapeViewButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(shapeColumnSelection(int)));
   }
   
   //
   // Create the button group for the surfaceShape buttons.
   //
   if (surfaceShapeCommentButtonGroup == NULL) {
      surfaceShapeCommentButtonGroup = new QButtonGroup(this);
      QObject::connect(surfaceShapeCommentButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(surfaceShapeCommentColumnSelection(int)));
   }
   
   //
   // Create the button group for the surface shape metadata buttons
   //
   if (surfaceShapeMetaDataButtonGroup == NULL) {
      surfaceShapeMetaDataButtonGroup = new QButtonGroup(this);
      QObject::connect(surfaceShapeMetaDataButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(surfaceShapeMetaDataColumnSelection(int)));
   }
   
   //
   // Add new widgets as needed
   //
   for (int i = numExistingSurfaceShape; i < numValidSurfaceShape; i++) {
      //
      // Column number
      //
      QLabel* columnNumberLabel = new QLabel(QString::number(i + 1));
      surfaceShapeColumnNumberLabels.push_back(columnNumberLabel);
      surfaceShapeSelectionGridLayout->addWidget(columnNumberLabel, i + 1, 0,
                                           Qt::AlignRight);
                                           
      //
      // View radio button
      //
      QRadioButton* viewRadioButton = new QRadioButton("");
      surfaceShapeViewRadioButtons.push_back(viewRadioButton);
      surfaceShapeViewButtonGroup->addButton(viewRadioButton, i);
      surfaceShapeSelectionGridLayout->addWidget(viewRadioButton, i + 1, 1, Qt::AlignHCenter);
      
      //
      // Comment push button
      //
      QPushButton* commentPushButton = new QPushButton("?");
      commentPushButton->setFixedWidth(40);
      commentPushButton->setAutoDefault(false);
      surfaceShapeColumnCommentPushButtons.push_back(commentPushButton);
      surfaceShapeCommentButtonGroup->addButton(commentPushButton, i);
      surfaceShapeSelectionGridLayout->addWidget(commentPushButton, i + 1, 2, Qt::AlignHCenter);
      
      //
      // Metadata push button
      //
      QPushButton* metaDataPushButton = new QPushButton("M");
      metaDataPushButton->setToolTip("Press the button to set the\n"
                                     "metadata link for this column");
      metaDataPushButton->setFixedWidth(40);
      metaDataPushButton->setAutoDefault(false);
      surfaceShapeColumnMetaDataPushButtons.push_back(metaDataPushButton);
      surfaceShapeMetaDataButtonGroup->addButton(metaDataPushButton, i);
      surfaceShapeSelectionGridLayout->addWidget(metaDataPushButton, i + 1, 3, Qt::AlignHCenter);
      
      //
      // Name line edit
      //
      QLineEdit* surfaceShapeLineEdit = new QLineEdit;
      surfaceShapeLineEdit->setText(ssf->getColumnName(i));
      surfaceShapeLineEdit->setMinimumWidth(nameMinimumWidth);
      surfaceShapeColumnNameLineEdits.push_back(surfaceShapeLineEdit);
      surfaceShapeSelectionGridLayout->addWidget(surfaceShapeLineEdit, i + 1, 4, Qt::AlignLeft);
   }
      
   //
   // Show and update all valid surfaceShapes
   //
   for (int i = 0; i < numValidSurfaceShape; i++) {
      surfaceShapeColumnNameLineEdits[i]->blockSignals(true);
      surfaceShapeColumnNameLineEdits[i]->setText(ssf->getColumnName(i));
      surfaceShapeColumnNameLineEdits[i]->blockSignals(false);
      if (i < numExistingSurfaceShape) {
         if (surfaceShapeColumnNumberLabels[i]->isVisible() == false) {
            surfaceShapeColumnNumberLabels[i]->show();
            surfaceShapeViewRadioButtons[i]->show();
            surfaceShapeColumnCommentPushButtons[i]->show();
            surfaceShapeColumnMetaDataPushButtons[i]->show();
            surfaceShapeColumnNameLineEdits[i]->show();
            surfaceShapeColumnNameLineEdits[i]->home(true);
         }
      }
   }
   
   //
   // Hide widgets that are not needed
   //
   for (int i = numValidSurfaceShape; i < numExistingSurfaceShape; i++) {
      surfaceShapeColumnNumberLabels[i]->hide();
      surfaceShapeViewRadioButtons[i]->hide();
      surfaceShapeColumnCommentPushButtons[i]->hide();
      surfaceShapeColumnMetaDataPushButtons[i]->hide();
      surfaceShapeColumnNameLineEdits[i]->hide();
   }
}

/**
 * called when a metric metadata column is selected.
 */
void 
GuiDisplayControlDialog::surfaceShapeMetaDataColumnSelection(int col)
{
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   if ((col >= 0) && (col < ssf->getNumberOfColumns())) {
      StudyMetaDataLinkSet smdls = ssf->getColumnStudyMetaDataLinkSet(col);
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         ssf->setColumnStudyMetaDataLinkSet(col, smdls);
      }
   }
}
/**
 * Called when a surface shape column comment is selected.
 */
void
GuiDisplayControlDialog::surfaceShapeCommentColumnSelection(int column)
{
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   if ((column >= 0) && (column < ssf->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    ssf, column);
      dfcd->show();
   }
}


/**
 * called when surface shape histogram pushbutton is pressed.
 */
void 
GuiDisplayControlDialog::surfaceShapeHistogram()
{
   DisplaySettingsSurfaceShape* dss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   const int column = dss->getSelectedDisplayColumn(surfaceModelIndex);
   if ((column >= 0) && (column < ssf->getNumberOfColumns())) {
      const int numNodes = ssf->getNumberOfNodes();
      std::vector<float> values(numNodes);
      for (int i = 0; i < numNodes; i++) {
         values[i] = ssf->getValue(i, column);
      }

      GuiHistogramDisplayDialog* ghd = new GuiHistogramDisplayDialog(theMainWindow, 
                                             ssf->getColumnName(column),
                                             values,
                                             false,
                                             false);

      ghd->show();
   }
}

/**
 * Called when surface shape color map selected.
 */
void
GuiDisplayControlDialog::shapeColorMapSelection(int mapNumber)
{
   DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
   
   dsss->setColorMap(static_cast<DisplaySettingsSurfaceShape::SURFACE_SHAPE_COLOR_MAP>(mapNumber));
   readShapeSettings();
}

/**
 * update shape overlay/underlay selection.
 */
void 
GuiDisplayControlDialog::updateShapeOverlayUnderlaySelection()
{
   if (pageOverlayUnderlaySurface == NULL) {
      return;
   }
   shapeSelectionComboBox->setEnabled(validShapeData);
   primaryOverlayShapeButton->setEnabled(validShapeData);
   secondaryOverlayShapeButton->setEnabled(validShapeData);
   underlayShapeButton->setEnabled(validShapeData);
   shapeSelectionLabel->setEnabled(validShapeData);
   shapeInfoPushButton->setEnabled(validShapeData);
   
   //
   // Update surface shape selection combo boxes
   //
   shapeSelectionComboBox->clear();
   
   DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   bool valid = false;
   if (ssf->getNumberOfColumns() > 0) {
      valid = true;
      for (int i = 0; i < ssf->getNumberOfColumns(); i++) {
         shapeSelectionComboBox->addItem(ssf->getColumnName(i));
      }
      shapeSelectionComboBox->setCurrentIndex(dsss->getSelectedDisplayColumn(surfaceModelIndex));
      
      shapeSelectionComboBox->setToolTip(
                    ssf->getColumnName(dsss->getSelectedDisplayColumn(surfaceModelIndex)));
                    
   }
}

/**
 * update the shape items.
 */
void 
GuiDisplayControlDialog::updateShapeItems()
{
   updatePageSelectionComboBox();
   updateShapeSettings();
   updateShapeSelections();
   updateShapeOverlayUnderlaySelection();
}
      
/**
 * update shape settings items in dialog.
 */
void 
GuiDisplayControlDialog::updateShapeSettings()
{
   if (pageSurfaceShapeSettings == NULL) {
      return;
   }
   pageSurfaceShapeSettingsWidgetGroup->blockSignals(true);
   
   DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   
   if (ssf->getNumberOfColumns() > 0) {
      float colMin, colMax;
      ssf->getDataColumnMinMax(dsss->getSelectedDisplayColumn(surfaceModelIndex), colMin, colMax);
      shapeViewMinimumLabel->setText(QString::number(colMin, 'f', 6));
      shapeViewMaximumLabel->setText(QString::number(colMax, 'f', 6));
      
      ssf->getColumnColorMappingMinMax(dsss->getSelectedDisplayColumn(surfaceModelIndex), colMin, colMax);
      shapeMinimumMappingDoubleSpinBox->setValue(colMin);
      shapeMaximumMappingDoubleSpinBox->setValue(colMax);
      
      shapeDisplayColorBarCheckBox->setChecked(dsss->getDisplayColorBar());
   }
   shapeNodeIdDeviationComboBox->updateComboBox(theMainWindow->getBrainSet()->getSurfaceShapeFile());
   shapeNodeIdDeviationGroupBox->setChecked(dsss->getNodeUncertaintyEnabled());
   shapeNodeIdDeviationComboBox->setCurrentIndex(dsss->getNodeUncertaintyColumn());

   shapeDisplayColorBarCheckBox->setEnabled(validShapeData);

   shapeColorMapPaletteComboBox->clear();
   PaletteFile* pf = theMainWindow->getBrainSet()->getPaletteFile();
   const int numPalettes = pf->getNumberOfPalettes();
   for (int i = 0; i < numPalettes; i++) {
      const Palette* p = pf->getPalette(i);
      QString name(p->getName());
      if (p->getPositiveOnly()) {
         name.append("+");
      }
      shapeColorMapPaletteComboBox->addItem(name);
   }
   shapeColorMapPaletteComboBox->setCurrentIndex(dsss->getSelectedPaletteIndex());
   shapeColorMapInterpolatePaletteCheckBox->setChecked(dsss->getInterpolatePaletteColors());
   
   const QList<QAbstractButton*> colorMapButtons = palColorMapButtonGroup->buttons();
   const int numColorMapButtons = colorMapButtons.count();
   if ((dsss->getColorMap() >= 0) &&
       (dsss->getColorMap() < numColorMapButtons)) {
      const int buttonNum = dsss->getColorMap();
      QAbstractButton* ab = palColorMapButtonGroup->button(buttonNum);
      QRadioButton* rb = dynamic_cast<QRadioButton*>(ab);
      if (rb != NULL) {
         rb->setChecked(true);
      }
   }
   pageSurfaceShapeSettings->setEnabled(validShapeData);
   
   updatePageSelectionComboBox();
   
   pageSurfaceShapeSettingsWidgetGroup->blockSignals(false);
}
      
/**
 * Update surface shape selections in dialog.
 */
void
GuiDisplayControlDialog::updateShapeSelections()
{
   if (pageSurfaceShapeSelections == NULL) {
      return;
   }
   
   DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
   shapeApplySelectionToLeftAndRightStructuresFlagCheckBox->setChecked(dsss->getApplySelectionToLeftAndRightStructuresFlag());

   pageSurfaceShapeSelections->setEnabled(validShapeData);
   
   createAndUpdateSurfaceShapeSelections();
   if (surfaceShapeViewButtonGroup != NULL) {
      QRadioButton* rb = dynamic_cast<QRadioButton*>(
                               surfaceShapeViewButtonGroup->button(dsss->getSelectedDisplayColumn(surfaceModelIndex)));
      if (rb != NULL) {
         rb->setChecked(true);
      }
   }
   
   updatePageSelectionComboBox();
}

/**
 * Read surface shape color mapping line edits.
 */
void
GuiDisplayControlDialog::readShapeColorMapping()
{
   if (creatingDialog) {
      return;
   }
   
   if (pageWidgetStack->currentWidget() == pageSurfaceShapeSettings) {
      DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
      
      SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
      
      ssf->setColumnColorMappingMinMax(dsss->getSelectedDisplayColumn(surfaceModelIndex),
                                       shapeMinimumMappingDoubleSpinBox->value(),
                                       shapeMaximumMappingDoubleSpinBox->value());
   }
}

/**
 * read the surface shape settings.
 */
void 
GuiDisplayControlDialog::readShapeSettings()
{
   if (pageSurfaceShapeSettings == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();

   dsss->setSelectedPaletteIndex(shapeColorMapPaletteComboBox->currentIndex());
   dsss->setInterpolatePaletteColors(shapeColorMapInterpolatePaletteCheckBox->isChecked());
   
   dsss->setNodeUncertaintyColumn(shapeNodeIdDeviationComboBox->currentIndex());
   dsss->setNodeUncertaintyEnabled(shapeNodeIdDeviationGroupBox->isChecked());
   dsss->setDisplayColorBar(shapeDisplayColorBarCheckBox->isChecked());
   readShapeColorMapping();
   
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}
      
/**
 * read the shape atlas L-to-L, R-to-R.
 */
void 
GuiDisplayControlDialog::readShapeL2LR2R()
{
   DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
   dsss->setApplySelectionToLeftAndRightStructuresFlag(shapeApplySelectionToLeftAndRightStructuresFlagCheckBox->isChecked());
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}
      
/**
 * Read the surface shape selections.
 */
void
GuiDisplayControlDialog::readShapeSelections()
{
   if (pageSurfaceShapeSelections == NULL) {
      return;
   }
   
   if (creatingDialog) {
      return;
   }
   
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   if (ssf->getNumberOfColumns() > 0) {
      for (int i = 0; i < ssf->getNumberOfColumns(); i++) {
         const QString name(surfaceShapeColumnNameLineEdits[i]->text());
         if (name != ssf->getColumnName(i)) {
            ssf->setColumnName(i, name);
            if (pageOverlayUnderlaySurface != NULL) {
               shapeSelectionComboBox->setItemText(i, name);
            }
         }
      }
   }
   
   GuiFilesModified fm;
   fm.setSurfaceShapeModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * create the paint name page.
 */
void 
GuiDisplayControlDialog::createPaintNamePage()
{
   //
   // All on/off push button
   //
   QPushButton* allOnPushButton = new QPushButton("All On");
   allOnPushButton->setAutoDefault(false);
   QObject::connect(allOnPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPaintNamesAllOnPushButton()));
   QPushButton* allOffPushButton = new QPushButton("All Off");
   allOffPushButton->setAutoDefault(false);
   QObject::connect(allOffPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPaintNamesAllOffPushButton()));
   QHBoxLayout* allButtonLayout = new QHBoxLayout;
   allButtonLayout->addWidget(allOnPushButton);                 
   allButtonLayout->addWidget(allOffPushButton);                 
   allButtonLayout->addStretch();   
            
   //
   // For name checkboxes
   //
   paintNameCheckBoxGridLayout = new QGridLayout;
   paintNameCheckBoxWidgetGroup = new WuQWidgetGroup(this);
   
   //
   // Create the paint page and its layout
   //
   pagePaintName = new QWidget;
   pageWidgetStack->addWidget(pagePaintName);
   QVBoxLayout* pagePaintNameLayout = new QVBoxLayout(pagePaintName);
   pagePaintNameLayout->addLayout(allButtonLayout);
   pagePaintNameLayout->addLayout(paintNameCheckBoxGridLayout);
   pagePaintNameLayout->setAlignment(paintNameCheckBoxGridLayout,
                                     Qt::AlignTop | Qt::AlignLeft);
}

/**
 * create and update the paint names.
 */
void 
GuiDisplayControlDialog::createAndUpdatePaintNamePage()
{
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   
   const int numNamesInDialog = static_cast<int>(paintNameCheckBoxes.size());

   std::vector<QString> paintNames;
   std::vector<int> paintNameIndices;
   pf->getAllPaintNamesAndIndices(paintNames, paintNameIndices);
   const int numValidNames = static_cast<int>(paintNames.size());
   
   //
   // Add new paint name checkboxes
   //
   for (int i = numNamesInDialog; i < numValidNames; i++) {
      QCheckBox* cb = new QCheckBox("");
      QObject::connect(cb, SIGNAL(toggled(bool)),
                       this, SLOT(readPaintNameSelections()));
      paintNameCheckBoxGridLayout->addWidget(cb,
                                             paintNameCheckBoxGridLayout->rowCount(),
                                             0);
      paintNameCheckBoxes.push_back(cb);
      paintNameCheckBoxWidgetGroup->addWidget(cb);
      
      paintNameCheckBoxPaintFileNameIndices.push_back(i);
   }
   
   //
   // Update items in dialog
   //
   paintNameCheckBoxWidgetGroup->blockSignals(true);
   for (int i = 0; i < numValidNames; i++) {
      paintNameCheckBoxes[i]->show();
      paintNameCheckBoxes[i]->setText(paintNames[i]);
      paintNameCheckBoxes[i]->setChecked(pf->getPaintNameEnabled(paintNameIndices[i]));
      paintNameCheckBoxPaintFileNameIndices[i] = paintNameIndices[i];
   }
   paintNameCheckBoxWidgetGroup->blockSignals(false);
   
   //
   // Hide unused checkboxes
   //
   for (int i = numValidNames; i < numNamesInDialog; i++) {
      paintNameCheckBoxes[i]->hide();
   }
}
      
/**
 * Page containing paints.
 */
void
GuiDisplayControlDialog::createPaintColumnPage()
{
   //
   // Medial wall override
   //
   paintMedWallCheckBox = new QCheckBox("Enable Medial Wall Override");
   QObject::connect(paintMedWallCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readPaintColumnSelections()));
   paintMedWallColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                     GUI_NODE_FILE_TYPE_PAINT,
                                                     false,
                                                     false,
                                                     false);
   QObject::connect(paintMedWallColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(readPaintColumnSelections()));
   QGroupBox* medWallBox = new QGroupBox("Medial Wall Override");
   QVBoxLayout* medWallBoxLayout = new QVBoxLayout(medWallBox);
   medWallBoxLayout->addWidget(paintMedWallCheckBox);
   medWallBoxLayout->addWidget(paintMedWallColumnComboBox);

   paintColumnButtonGroup = new QButtonGroup(this);
   paintColumnButtonGroup->setExclusive(true);
   QObject::connect(paintColumnButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(paintColumnSelection(int)));
   
   //
   // Display color key button
   //
   QPushButton* colorKeyPushButton = new QPushButton("Display Color Key...");
   colorKeyPushButton->setFixedSize(colorKeyPushButton->sizeHint());
   colorKeyPushButton->setAutoDefault(false);
   QObject::connect(colorKeyPushButton, SIGNAL(clicked()),
                    theMainWindow, SLOT(displayPaintColorKey()));
                    
   //
   // Create the paint page and its layout
   //
   pagePaintColumn = new QWidget;
   pageWidgetStack->addWidget(pagePaintColumn);
   paintPageLayout = new QVBoxLayout(pagePaintColumn);
   paintPageLayout->addWidget(medWallBox);
   paintPageLayout->addWidget(colorKeyPushButton);
   
   //
   // comment button group
   //
   paintColumnCommentButtonGroup = new QButtonGroup(this);
   QObject::connect(paintColumnCommentButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(paintCommentColumnSelection(int)));
   
   //
   // 
   //
   paintColumnMetaDataButtonGroup = new QButtonGroup(this);
   QObject::connect(paintColumnMetaDataButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(paintMetaDataColumnSelection(int)));
                    
   pagePaintColumnWidgetGroup = new WuQWidgetGroup(this);
   pagePaintColumnWidgetGroup->addWidget(paintMedWallCheckBox);
}

/**
 * called when a metric metadata column is selected.
 */
void 
GuiDisplayControlDialog::paintMetaDataColumnSelection(int col)
{
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   if ((col >= 0) && (col < pf->getNumberOfColumns())) {
      StudyMetaDataLinkSet smdls = pf->getColumnStudyMetaDataLinkSet(col);
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         pf->setColumnStudyMetaDataLinkSet(col, smdls);
      }
   }
}

/**
 * Create and update the paint page.
 */
void
GuiDisplayControlDialog::createAndUpdatePaintColumnPage()
{
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   numValidPaints = pf->getNumberOfColumns();
   
   const int numExistingPaints = static_cast<int>(paintColumnRadioButtons.size());
   
   const int nameMinimumWidth = 500;

   if (paintColumnSelectionGridLayout == NULL) {
      
      QWidget* paintSelectionWidget = new QWidget;
      paintColumnSelectionGridLayout = new QGridLayout(paintSelectionWidget);
      //paintSelectionGridLayout->setMargin(3);
      paintColumnSelectionGridLayout->setSpacing(3);
      paintColumnSelectionGridLayout->setColumnMinimumWidth(4, nameMinimumWidth+20);
      
      paintColumnSelectionGridLayout->addWidget(new QLabel("#"),
                                           0, 0, Qt::AlignRight);
      paintColumnSelectionGridLayout->addWidget(new QLabel("View"),
                                           0, 1, Qt::AlignHCenter);
      paintColumnSelectionGridLayout->addWidget(new QLabel("Cmt"),
                                           0, 2, Qt::AlignHCenter);
      paintColumnSelectionGridLayout->addWidget(new QLabel("MD"),
                                           0, 3, Qt::AlignHCenter);
      paintColumnSelectionGridLayout->addWidget(new QLabel("Name"),
                                           0, 4, Qt::AlignLeft);
                                           
      //
      // For stretching on  bottom
      //
     // const int rowStretchNumber    = 15000;
     // paintSelectionGridLayout->addWidget(new QLabel(""),
     //                                      rowStretchNumber, 4, 1, 1, Qt::AlignLeft);
      paintColumnSelectionGridLayout->setColumnStretch(0, 0);
      paintColumnSelectionGridLayout->setColumnStretch(1, 0);
      paintColumnSelectionGridLayout->setColumnStretch(2, 0);
      paintColumnSelectionGridLayout->setColumnStretch(3, 0);
      paintColumnSelectionGridLayout->setColumnStretch(4, 0);
     // paintSelectionGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      paintPageLayout->addWidget(paintSelectionWidget, 100, Qt::AlignLeft | Qt::AlignTop);
   }
   
   //
   // Add radio buttons and text boxes
   //
   for (int i = numExistingPaints; i < numValidPaints; i++) {
      QLabel* colLabel = new QLabel(QString::number(i + 1));
      paintColumnNameLabels.push_back(colLabel);
      paintColumnSelectionGridLayout->addWidget(colLabel, i + 1, 0, Qt::AlignLeft);
      
      QRadioButton* rb = new QRadioButton("");
      paintColumnRadioButtons.push_back(rb);
      paintColumnButtonGroup->addButton(rb, i);
      paintColumnSelectionGridLayout->addWidget(rb, i + 1, 1, Qt::AlignLeft);
      
      //
      // Comment push button
      //
      QPushButton* commentPushButton = new QPushButton("?");
      commentPushButton->setFixedWidth(40);
      commentPushButton->setAutoDefault(false);
      paintColumnCommentPushButtons.push_back(commentPushButton);
      paintColumnCommentButtonGroup->addButton(commentPushButton, i);
      paintColumnSelectionGridLayout->addWidget(commentPushButton, i + 1, 2, Qt::AlignHCenter);
      
      //
      // metadata push button
      //
      QPushButton* metaDataPushButton = new QPushButton("M");
      metaDataPushButton->setToolTip("Press the button to set the\n"
                                     "metadata link for this column");
      metaDataPushButton->setFixedWidth(40);
      metaDataPushButton->setAutoDefault(false);
      paintColumnMetaDataPushButtons.push_back(metaDataPushButton);
      paintColumnMetaDataButtonGroup->addButton(metaDataPushButton, i);
      paintColumnSelectionGridLayout->addWidget(metaDataPushButton, i + 1, 3, Qt::AlignHCenter);
      
      //
      // Name line edit
      //
      QLineEdit* le = new QLineEdit;
      le->setMinimumWidth(nameMinimumWidth);
      paintColumnNameLineEdits.push_back(le);
      QObject::connect(le, SIGNAL(returnPressed()),
                       this, SLOT(readPaintColumnSelections()));
      paintColumnSelectionGridLayout->addWidget(le, i + 1, 4, Qt::AlignLeft);
      
      pagePaintColumnWidgetGroup->addWidget(rb);
      pagePaintColumnWidgetGroup->addWidget(le);
   }
   
   //
   // Update items already in the dialog
   //
   for (int i = 0; i < numValidPaints; i++) {
      paintColumnNameLabels[i]->show();
      paintColumnRadioButtons[i]->show();
      paintColumnCommentPushButtons[i]->show();
      paintColumnMetaDataPushButtons[i]->show();
      paintColumnNameLineEdits[i]->setText(pf->getColumnName(i));
      paintColumnNameLineEdits[i]->home(true);
      paintColumnNameLineEdits[i]->show();
   }
   
   //
   // Hide paints that are not needed
   //
   for (int i = numValidPaints; i < numExistingPaints; i++) {
      paintColumnNameLabels[i]->hide();
      paintColumnRadioButtons[i]->hide();
      paintColumnMetaDataPushButtons[i]->hide();
      paintColumnCommentPushButtons[i]->hide();
      paintColumnNameLineEdits[i]->hide();
   }
}

/**
 * update paint overlay/underlay selection.
 */
void 
GuiDisplayControlDialog::updatePaintOverlayUnderlaySelection()
{
   if (pageOverlayUnderlaySurface == NULL) {
      return;
   }

   DisplaySettingsPaint* dsp = theMainWindow->getBrainSet()->getDisplaySettingsPaint();
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   paintSelectionComboBox->clear();
   if (pf->getNumberOfColumns() > 0) {
      for (int i = 0; i < pf->getNumberOfColumns(); i++) {
         paintSelectionComboBox->addItem(pf->getColumnName(i));
      }
   
      const int selCol = dsp->getSelectedColumn(surfaceModelIndex);
      paintSelectionComboBox->setCurrentIndex(selCol);
      paintSelectionComboBox->setToolTip(
                    pf->getColumnName(dsp->getSelectedColumn(surfaceModelIndex)));
   }
   paintSelectionComboBox->setEnabled(validPaintData);
   primaryOverlayPaintButton->setEnabled(validPaintData);
   secondaryOverlayPaintButton->setEnabled(validPaintData);
   underlayPaintButton->setEnabled(validPaintData);
   paintSelectionLabel->setEnabled(validPaintData);
   paintInfoPushButton->setEnabled(validPaintData);
   
   bool geoBlendDataValid = true;
   if (validPaintData) {
      if (pf->getGeographyColumnNumber() < 0) {
         geoBlendDataValid = false;
      }
   }
   underlayGeographyBlendingButton->setEnabled(geoBlendDataValid);
   geographyBlendingSelectionLabel->setEnabled(geoBlendDataValid);
   geographyBlendingDoubleSpinBox->setEnabled(geoBlendDataValid);
}

/**
 * Update paint items in the dialog.
 */
void
GuiDisplayControlDialog::updatePaintItems()
{
   updatePageSelectionComboBox();
   
   updatePaintOverlayUnderlaySelection();
   
   updatePaintColumnPage();
   
   updatePaintNamePage();
}

/**
 * update paint name page.
 */
void 
GuiDisplayControlDialog::updatePaintNamePage()
{
   if (pagePaintName == NULL) {
      return;
   }
   
   createAndUpdatePaintNamePage();
}
      
/**
 * update paint column name page.
 */
void 
GuiDisplayControlDialog::updatePaintColumnPage()
{
   if (pagePaintColumn == NULL) {
      return;
   }   
   createAndUpdatePaintColumnPage();
   pagePaintColumnWidgetGroup->blockSignals(true);
   
   
   DisplaySettingsPaint* dsp = theMainWindow->getBrainSet()->getDisplaySettingsPaint();
   paintApplySelectionToLeftAndRightStructuresFlagCheckBox->setChecked(dsp->getApplySelectionToLeftAndRightStructuresFlag());
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   if (pf->getNumberOfColumns() > 0) {
      const int selCol = dsp->getSelectedColumn(surfaceModelIndex);
      //if (pageOverlayUnderlaySurface != NULL) {
      //   paintSelectionComboBox->setCurrentIndex(selCol);
      //}
      if ((selCol >= 0) && (selCol < paintColumnButtonGroup->buttons().count())) {
         QRadioButton* rb = dynamic_cast<QRadioButton*>(paintColumnButtonGroup->button(selCol));
         if (rb != NULL) {
            rb->setChecked(true);
         }
      }
   }
   if (paintColumnButtonGroup != NULL) {
      QRadioButton* rb = dynamic_cast<QRadioButton*>(
                               paintColumnButtonGroup->button(dsp->getSelectedColumn(surfaceModelIndex)));
      if (rb != NULL) {
         rb->setChecked(true);
      }
   }

   paintMedWallCheckBox->setChecked(dsp->getMedialWallOverrideColumnEnabled());
   paintMedWallColumnComboBox->setEnabled(dsp->getMedialWallOverrideColumnEnabled());
   paintMedWallColumnComboBox->updateComboBox(theMainWindow->getBrainSet()->getPaintFile());
   paintMedWallColumnComboBox->setCurrentIndex(dsp->getMedialWallOverrideColumn());
   
   pagePaintColumn->setEnabled(validPaintData);
   
   pagePaintColumnWidgetGroup->blockSignals(false);
}

/**
 * called to read paint L-to-L, R-to-R.
 */
void 
GuiDisplayControlDialog::readPaintL2LR2R()
{
   DisplaySettingsPaint* dsp = theMainWindow->getBrainSet()->getDisplaySettingsPaint();
   dsp->setApplySelectionToLeftAndRightStructuresFlag(paintApplySelectionToLeftAndRightStructuresFlagCheckBox->isChecked());
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}
                
/**
 * called when paint name all on button is pressed.
 */
void 
GuiDisplayControlDialog::slotPaintNamesAllOnPushButton()
{
   paintNameCheckBoxWidgetGroup->blockSignals(true);
   const int num = static_cast<int>(paintNameCheckBoxes.size());
   for (int i = 0; i < num; i++) {
      paintNameCheckBoxes[i]->setChecked(true);
   }
   paintNameCheckBoxWidgetGroup->blockSignals(false);
   readPaintNameSelections();   
}

/**
 * called when paint name all off button is pressed.
 */
void 
GuiDisplayControlDialog::slotPaintNamesAllOffPushButton()
{
   paintNameCheckBoxWidgetGroup->blockSignals(true);
   const int num = static_cast<int>(paintNameCheckBoxes.size());
   for (int i = 0; i < num; i++) {
      paintNameCheckBoxes[i]->setChecked(false);
   }
   paintNameCheckBoxWidgetGroup->blockSignals(false);
   readPaintNameSelections();   
}

/**
 * called to read paint name items in the dialog.
 */
void 
GuiDisplayControlDialog::readPaintNameSelections()
{
   if (creatingDialog) {
      return;
   }
   if (pagePaintName == NULL) {
      return;
   }
      
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   const int num = std::min(pf->getNumberOfPaintNames(),
                            static_cast<int>(paintNameCheckBoxes.size()));
   for (int i = 0; i < num; i++) {
      pf->setPaintNameEnabled(paintNameCheckBoxPaintFileNameIndices[i],
                              paintNameCheckBoxes[i]->isChecked());
   }
   
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}
                
/**
 * Read all paint items in the dialog.
 */
void
GuiDisplayControlDialog::readPaintColumnSelections()
{
   if (creatingDialog) {
      return;
   }
   
   if (pagePaintColumn != NULL) {
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      DisplaySettingsPaint* dsp = theMainWindow->getBrainSet()->getDisplaySettingsPaint();
      if (pf->getNumberOfColumns() > 0) {
         //pf->setSelectedColumn(paintButtonGroup->selectedId());
         
         for (int i = 0; i < pf->getNumberOfColumns(); i++) {
            const QString name(paintColumnNameLineEdits[i]->text());
            if (name != pf->getColumnName(i)) {
               pf->setColumnName(i, name);
            }
         }
      }
      
      dsp->setMedialWallOverrideColumnEnabled(paintMedWallCheckBox->isChecked());
      dsp->setMedialWallOverrideColumn(paintMedWallColumnComboBox->currentIndex());
      paintMedWallColumnComboBox->setEnabled(dsp->getMedialWallOverrideColumnEnabled());
   }
   
   GuiFilesModified fm;
   fm.setPaintModified();
   theMainWindow->fileModificationUpdate(fm);
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * Called to display comment information about a paint column.
 */
void
GuiDisplayControlDialog::paintCommentColumnSelection(int column)
{
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   if ((column >= 0) && (column < pf->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    pf, column);
      dfcd->show();
   }
}

/**
 * Page containing images.
 */
void
GuiDisplayControlDialog::createImagesPage()
{
   pageImages = new QWidget;
   pageWidgetStack->addWidget(pageImages); //, PAGE_NAME_IMAGES); // adds to dialog
   QVBoxLayout* imagesPageLayout = new QVBoxLayout(pageImages);
   
   showImageInMainWindowCheckBox = new QCheckBox("Show Image in Main Window");
   imagesPageLayout->addWidget(showImageInMainWindowCheckBox);
   QObject::connect(showImageInMainWindowCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readImagesSelections()));
   
   QWidget* imagesWidget = new QWidget;
   imagesPageLayout->addWidget(imagesWidget);
   
   QVBoxLayout* imageWidgetLayout = new QVBoxLayout(imagesWidget);
   imagesLayout = new QVBoxLayout;
   imageWidgetLayout->addLayout(imagesLayout);
   imageWidgetLayout->addStretch(100); // pushes images radio buttons to the top
   imagesButtonGroup = new QButtonGroup(this); //("Images", imageSelectionScrollView);

   QObject::connect(imagesButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(readImagesSelections()));
}

/**
 * Update all image items in the dialog.
 */
void
GuiDisplayControlDialog::updateImagesItems()
{
   updatePageSelectionComboBox();
   
   if (pageImages == NULL) {
      return;
   }
   
   DisplaySettingsImages* dsi = theMainWindow->getBrainSet()->getDisplaySettingsImages();
   const int numImages = theMainWindow->getBrainSet()->getNumberOfImageFiles();
   
   showImageInMainWindowCheckBox->blockSignals(true);
   showImageInMainWindowCheckBox->setChecked(dsi->getShowImageInMainWindow());
   showImageInMainWindowCheckBox->blockSignals(false);
   
   //
   // Update already created radio buttons
   //
   int numRadioButtons = static_cast<int>(imagesRadioButtons.size());
   for (int i = 0; i < numRadioButtons; i++) {
      if (i < numImages) {
         const ImageFile* img = theMainWindow->getBrainSet()->getImageFile(i);
         const QString name(FileUtilities::basename(img->getFileName()));
         imagesRadioButtons[i]->setText(name);
      }
   }
   
   //
   // Create new radio buttons
   //
   for (int i = numRadioButtons; i < numImages; i++) {
      const ImageFile* img = theMainWindow->getBrainSet()->getImageFile(i);
      const QString name(FileUtilities::basename(img->getFileName()));
      QRadioButton* butt = new QRadioButton(name);
      imagesLayout->addWidget(butt);
      imagesButtonGroup->addButton(butt, i);
      imagesRadioButtons.push_back(butt);
   }
   
   //
   // Hide unneeded radio buttons
   //
   numRadioButtons = static_cast<int>(imagesRadioButtons.size());
   for (int i = 0; i < numRadioButtons; i++) {
      if (i < numImages) {
         imagesRadioButtons[i]->show();
      }
      else {
         imagesRadioButtons[i]->hide();
      }
   }
   
   if (numImages > 0) {
      imagesButtonGroup->blockSignals(true);
      //imagesButtonGroup->setButton(dsi->getMainWindowImageNumber());
      int imageNum = dsi->getMainWindowImageNumber();
      if (imageNum <= 0) {
         imageNum = 0;
      }
      if (imageNum < static_cast<int>(imagesRadioButtons.size())) {
         imagesRadioButtons[imageNum]->setChecked(true);
      }
      imagesButtonGroup->blockSignals(false);
   }
   pageImages->setEnabled(validImageData);
}

/**
 * Read all images items in the dialog.
 */
void
GuiDisplayControlDialog::readImagesSelections()
{
   if (creatingDialog) {
      return;
   }
   DisplaySettingsImages* dsi = theMainWindow->getBrainSet()->getDisplaySettingsImages();
   dsi->setMainWindowImageNumber(imagesButtonGroup->checkedId());
   dsi->setShowImageInMainWindow(showImageInMainWindowCheckBox->isChecked());
   
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Create the main page for models.
 */
void
GuiDisplayControlDialog::createModelsMainPage()
{
   //
   // Vertical Box Layout for all selection items
   //
   pageModelsMain = new QWidget;
   modelsSubPageMainLayout = new QVBoxLayout(pageModelsMain);
   
   //
   // All On/Off buttons
   //
   QPushButton* allOnPushButton = new QPushButton("All On");
   allOnPushButton->setAutoDefault(false);
   QObject::connect(allOnPushButton, SIGNAL(clicked()),
                    this, SLOT(slotModelsAllOn()));
   QPushButton* allOffPushButton = new QPushButton("All Off");
   allOffPushButton->setAutoDefault(false);
   QObject::connect(allOffPushButton, SIGNAL(clicked()),
                    this, SLOT(slotModelsAllOff()));
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(allOnPushButton);
   allOnOffLayout->addWidget(allOffPushButton);
   allOnOffLayout->addStretch();
   modelsSubPageMainLayout->addLayout(allOnOffLayout);
   createAndUpdateModelsMainPage();

   pageWidgetStack->addWidget(pageModelsMain);
}

/**
 * create and update the models main page as models added/deleted
 */
void
GuiDisplayControlDialog::createAndUpdateModelsMainPage()
{
   numValidModels = theMainWindow->getBrainSet()->getNumberOfVtkModelFiles();
   
   const int numExistingModels = static_cast<int>(modelCheckBoxes.size());
   
   if (modelSelectionGridWidgetLayout == NULL) {
      QWidget* modelsWidget = new QWidget;
      modelSelectionGridWidgetLayout = new QGridLayout(modelsWidget);
      
      modelSelectionGridWidgetLayout->addWidget(new QLabel("Model"), 0, 0);
      modelSelectionGridWidgetLayout->addWidget(new QLabel("  Transform"), 0, 1);
      modelSelectionGridWidgetLayout->addWidget(new QLabel(""), 0, 2);
      const int rowStretchNumber = 15000;
      modelSelectionGridWidgetLayout->addWidget(new QLabel(""), rowStretchNumber, 0);
      modelSelectionGridWidgetLayout->setRowStretch(rowStretchNumber, 1000);
      modelSelectionGridWidgetLayout->setColumnStretch(0, 1);
      modelSelectionGridWidgetLayout->setColumnStretch(1, 1);
      modelSelectionGridWidgetLayout->setColumnStretch(2, 100);
      
      modelsSubPageMainLayout->addWidget(modelsWidget);
   }
   
   //
   // Add row of checkbox and transform matrix selection 
   //
   for (int i = numExistingModels; i < numValidModels; i++) {
      QCheckBox* cb = new QCheckBox("adding");
      modelCheckBoxes.push_back(cb);
      QObject::connect(cb, SIGNAL(toggled(bool)),
                       this, SLOT(readModelMainPage()));
      GuiTransformationMatrixSelectionControl* msc = new
           GuiTransformationMatrixSelectionControl(0,
                                                   theMainWindow->getBrainSet()->getTransformationMatrixFile(),
                                                   true);
      
      const VtkModelFile* vmf = theMainWindow->getBrainSet()->getVtkModelFile(i);
      const TransformationMatrix* tm = vmf->getAssociatedTransformationMatrix();
      msc->setSelectedMatrix(tm);
      modelTransformControls.push_back(msc);
      QObject::connect(msc, SIGNAL(activated(int)),
                       this, SLOT(readModelMainPage()));
                       
      modelSelectionGridWidgetLayout->addWidget(cb, i + 1, 0, 1, 1, Qt::AlignLeft);
      modelSelectionGridWidgetLayout->addWidget(msc, i + 1, 1, 1, 1, Qt::AlignLeft);
   }
   
   //
   // update items already in dialog
   //
   TransformationMatrixFile* tmf = theMainWindow->getBrainSet()->getTransformationMatrixFile();
   for (int i = 0; i < numValidModels; i++) {
      VtkModelFile* vmf = theMainWindow->getBrainSet()->getVtkModelFile(i);
      const TransformationMatrix* tm = vmf->getAssociatedTransformationMatrix();
      const QString name(FileUtilities::basename(
                              FileUtilities::filenameWithoutExtension(vmf->getFileName())));
      modelCheckBoxes[i]->setText(name);
      modelCheckBoxes[i]->show();
      modelCheckBoxes[i]->setChecked(vmf->getDisplayFlag());
      modelTransformControls[i]->blockSignals(true);
      modelTransformControls[i]->updateControl();
      modelTransformControls[i]->setSelectedMatrixIndex(tmf->getMatrixIndex(tm));
      modelTransformControls[i]->show();
      modelTransformControls[i]->blockSignals(false);
   }
   
   //
   // Hide widgets that are not needed
   //
   for (int i = numValidModels; i < numExistingModels; i++) {
      modelCheckBoxes[i]->hide();
      modelTransformControls[i]->hide();
   }
}

/**
 * Create the settings page for models.
 */
void
GuiDisplayControlDialog::createModelsSettingsPage()
{
   QLabel* opacityLabel = new QLabel("Opacity ");
   modelsOpacityDoubleSpinBox = new QDoubleSpinBox;
   modelsOpacityDoubleSpinBox->setMinimum(0.0);
   modelsOpacityDoubleSpinBox->setMaximum(1.0);
   modelsOpacityDoubleSpinBox->setSingleStep(0.1);
   modelsOpacityDoubleSpinBox->setDecimals(2);
   QObject::connect(modelsOpacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readModelSettingsPage()));
                    
   QLabel* lineSizeLabel = new QLabel("Line Size ");
   modelsLineSizeDoubleSpinBox = new QDoubleSpinBox; 
   modelsLineSizeDoubleSpinBox->setMinimum(minLineSize);
   modelsLineSizeDoubleSpinBox->setMaximum(maxLineSize);
   modelsLineSizeDoubleSpinBox->setSingleStep(1.0);
   modelsLineSizeDoubleSpinBox->setDecimals(1);
   QObject::connect(modelsLineSizeDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readModelSettingsPage()));

   QLabel* vertexSizeLabel = new QLabel("Vertex Size");
   modelsVertexSizeDoubleSpinBox = new QDoubleSpinBox;  
   modelsVertexSizeDoubleSpinBox->setMinimum(minPointSize);
   modelsVertexSizeDoubleSpinBox->setMaximum(maxPointSize);
   modelsVertexSizeDoubleSpinBox->setSingleStep(1.0);
   modelsVertexSizeDoubleSpinBox->setDecimals(1);
   QObject::connect(modelsVertexSizeDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readModelSettingsPage()));
   
   //
   // Grid for size controls
   //
   QWidget* sizeWidget = new QWidget;
   QGridLayout* sizeLayout = new QGridLayout(sizeWidget);
   sizeLayout->addWidget(opacityLabel, 0, 0);
   sizeLayout->addWidget(modelsOpacityDoubleSpinBox, 0, 1);
   sizeLayout->addWidget(lineSizeLabel, 1, 0);
   sizeLayout->addWidget(modelsLineSizeDoubleSpinBox, 1, 1);
   sizeLayout->addWidget(vertexSizeLabel, 2, 0);
   sizeLayout->addWidget(modelsVertexSizeDoubleSpinBox, 2, 1);
   sizeWidget->setFixedSize(sizeWidget->sizeHint());
   
   modelLinesLightingCheckBox = new QCheckBox("Light Lines");
   QObject::connect(modelLinesLightingCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readModelSettingsPage()));
                    
   modelPolygonsLightingCheckBox = new QCheckBox("Light Polygons");
   QObject::connect(modelPolygonsLightingCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readModelSettingsPage()));
                    
   modelVerticesLightingCheckBox = new QCheckBox("Light Vertices");
   QObject::connect(modelVerticesLightingCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readModelSettingsPage()));
                    
   //
   // Page and layouts
   //
   pageModelsSettings = new QWidget;
   QVBoxLayout* modelSettingsLayout = new QVBoxLayout(pageModelsSettings);
   modelSettingsLayout->addWidget(sizeWidget, 0, Qt::AlignLeft);
   modelSettingsLayout->addWidget(modelLinesLightingCheckBox);
   modelSettingsLayout->addWidget(modelPolygonsLightingCheckBox);
   modelSettingsLayout->addWidget(modelVerticesLightingCheckBox);
   modelSettingsLayout->addStretch();

   pageWidgetStack->addWidget(pageModelsSettings);
}

/**
 * update model main page.
 */
void 
GuiDisplayControlDialog::updateModelMainPage()
{
  if (pageModelsMain == NULL) {
     return;
  }
   createAndUpdateModelsMainPage();
   pageModelsMain->setEnabled(validModelData);
}

/**
 * update model settings page.
 */
void 
GuiDisplayControlDialog::updateModelSettingsPage()
{
   if (pageModelsSettings == NULL) {
      return;
   }
   DisplaySettingsModels* dsm = theMainWindow->getBrainSet()->getDisplaySettingsModels();
   modelsOpacityDoubleSpinBox->setValue(dsm->getOpacity());
   modelsLineSizeDoubleSpinBox->setValue(dsm->getLineWidth());
   modelsVertexSizeDoubleSpinBox->setValue(dsm->getVertexSize());  
   modelLinesLightingCheckBox->setChecked(dsm->getLightLinesEnabled()); 
   modelPolygonsLightingCheckBox->setChecked(dsm->getLightPolygonsEnabled()); 
   modelVerticesLightingCheckBox->setChecked(dsm->getLightVerticesEnabled());
    
   pageModelsSettings->setEnabled(validModelData);
}

/**
 * Update all model items in the dialog.
 */
void
GuiDisplayControlDialog::updateModelItems()
{
   updatePageSelectionComboBox();
   updateModelMainPage();
   updateModelSettingsPage();
   updatePageSelectionComboBox();
}

/**
 * called to read model main page items from dialog.
 */
void 
GuiDisplayControlDialog::readModelMainPage()
{
   if (pageModelsMain == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   
   int numModels = theMainWindow->getBrainSet()->getNumberOfVtkModelFiles();
   if (numModels != numValidModels) {
      std::cout << "PROGRAM ERROR at " << __LINE__ << " in " << __FILE__ << ".  "
                << "Model number mismatch." << std::endl;
      numModels = std::min(numModels, numValidModels);
   }
   TransformationMatrixFile* tmf = theMainWindow->getBrainSet()->getTransformationMatrixFile();
   for (int i = 0; i < numModels; i++) {
      VtkModelFile* vmf = theMainWindow->getBrainSet()->getVtkModelFile(i);
      vmf->setDisplayFlag(modelCheckBoxes[i]->isChecked());
      const int matrixIndex = modelTransformControls[i]->getSelectedMatrixIndex();
      vmf->setAssociatedTransformationMatrix(tmf->getTransformationMatrix(matrixIndex));
   }
   
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * called to read model main page items from dialog.
 */
void 
GuiDisplayControlDialog::readModelSettingsPage()
{
   if (pageModelsSettings == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   DisplaySettingsModels* dsm = theMainWindow->getBrainSet()->getDisplaySettingsModels();
   dsm->setOpacity(modelsOpacityDoubleSpinBox->value());
   dsm->setLineWidth(modelsLineSizeDoubleSpinBox->value());
   dsm->setVertexSize(modelsVertexSizeDoubleSpinBox->value());
   dsm->setLightLinesEnabled(modelLinesLightingCheckBox->isChecked());
   dsm->setLightPolygonsEnabled(modelPolygonsLightingCheckBox->isChecked());
   dsm->setLightVerticesEnabled(modelVerticesLightingCheckBox->isChecked());
   
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * called when all models on pushbutton pressed.
 */
void 
GuiDisplayControlDialog::slotModelsAllOn()
{
   for (int i = 0; i < static_cast<int>(modelCheckBoxes.size()); i++) {
      modelCheckBoxes[i]->setChecked(true);
   }
   readModelMainPage();
}
      
/**
 * called when all models off pushbutton pressed.
 */
void 
GuiDisplayControlDialog::slotModelsAllOff()
{
   for (int i = 0; i < static_cast<int>(modelCheckBoxes.size()); i++) {
      modelCheckBoxes[i]->setChecked(false);
   }
   readModelMainPage();
}

/** 
 * Create the metric selections page.
 */
void
GuiDisplayControlDialog::createMetricSelectionPage()
{
   //
   // Vertical Box Layout for all selection items
   //
   pageMetricSelection = new QWidget;
   createAndUpdateMetricSelectionPage();

   pageWidgetStack->addWidget(pageMetricSelection);
}

/**
 * Create and update metric selections.  Because the number of metrics may change,
 * this method may update and change the labels on existing checkboxes, add new 
 * checkboxes, and hide existing checkboxes.
 */
void
GuiDisplayControlDialog::createAndUpdateMetricSelectionPage()
{
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   numValidMetrics = mf->getNumberOfColumns();
   
   const int numExistingMetrics = static_cast<int>(metricViewRadioButtons.size());
   const int nameMinimumWidth = 500;
      
   if (metricSubPageSelectionsLayout == NULL) {
      //
      // Grid layout for metric selections
      //
      QWidget* metricSelectionGridWidget = new QWidget;
      metricSelectionGridLayout = new QGridLayout(metricSelectionGridWidget);
      metricSelectionGridLayout->setSpacing(3);
      metricSelectionGridLayout->setColumnMinimumWidth(5, nameMinimumWidth+20);
      metricSelectionGridLayout->addWidget(new QLabel("#"),
                                           0, 0, Qt::AlignRight);
      metricSelectionGridLayout->addWidget(new QLabel("View"),
                                           0, 1, Qt::AlignHCenter);
      metricSelectionGridLayout->addWidget(new QLabel("Thresh"),
                                           0, 2, Qt::AlignHCenter);
      metricSelectionGridLayout->addWidget(new QLabel("Cmt"),
                                           0, 3, Qt::AlignHCenter);
      metricSelectionGridLayout->addWidget(new QLabel("MD"),
                                           0, 4, Qt::AlignHCenter);
      metricSelectionGridLayout->addWidget(new QLabel("Name"),
                                           0, 5, Qt::AlignLeft);
                                           
      //
      // For stretching on  bottom
      //
      //const int rowStretchNumber    = 15000;
      //metricSelectionGridLayout->addWidget(new QLabel(""),
      //                                     rowStretchNumber, 5, 1, 1, Qt::AlignLeft);
                                           
      //
      // Only allow the name column to stretch
      // 
      metricSelectionGridLayout->setColumnStretch(0, 0);
      metricSelectionGridLayout->setColumnStretch(1, 0);
      metricSelectionGridLayout->setColumnStretch(2, 0);
      metricSelectionGridLayout->setColumnStretch(3, 0);
      metricSelectionGridLayout->setColumnStretch(4, 0);
      metricSelectionGridLayout->setColumnStretch(5, 0);
      //metricSelectionGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      //
      // Create the metric selections layout and push selections to the left
      //
      metricSubPageSelectionsLayout = new QVBoxLayout(pageMetricSelection);
      metricSubPageSelectionsLayout->addWidget(metricSelectionGridWidget, 100, Qt::AlignLeft | Qt::AlignTop);
   }
   
   //
   // Create the button group for the metric view buttons.
   //
   if (metricViewButtonGroup == NULL) {
      metricViewButtonGroup = new QButtonGroup(this);
      metricViewButtonGroup->setExclusive(true);
      QObject::connect(metricViewButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(metricDisplayColumnSelection(int)));
   }
   
   //
   // Create the button group for the metric threshold buttons.
   //
   if (metricThresholdButtonGroup == NULL) {
      metricThresholdButtonGroup = new QButtonGroup(this);
      metricThresholdButtonGroup->setExclusive(true);
      QObject::connect(metricThresholdButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(metricThresholdColumnSelection(int)));
   }
   
   //
   // Create the button group for the metric comment buttons.
   //
   if (metricCommentButtonGroup == NULL) {
      metricCommentButtonGroup = new QButtonGroup(this);
      QObject::connect(metricCommentButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(metricCommentColumnSelection(int)));
   }
   
   //
   // Create the button group for the metric metadata buttons.
   //
   if (metricMetaDataButtonGroup == NULL) {
      metricMetaDataButtonGroup = new QButtonGroup(this);
      QObject::connect(metricMetaDataButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(metricMetaDataColumnSelection(int)));
   }
   
   //
   // Add new widgets as needed
   //
   for (int i = numExistingMetrics; i < numValidMetrics; i++) {
      //
      // Column number
      //
      QLabel* columnNumberLabel = new QLabel(QString::number(i + 1));
      metricColumnNumberLabels.push_back(columnNumberLabel);
      metricSelectionGridLayout->addWidget(columnNumberLabel, i + 1, 0,
                                           Qt::AlignRight);
                                           
      //
      // View radio button
      //
      QRadioButton* viewRadioButton = new QRadioButton("");
      metricViewRadioButtons.push_back(viewRadioButton);
      metricViewButtonGroup->addButton(viewRadioButton, i);
      metricSelectionGridLayout->addWidget(viewRadioButton, i + 1, 1, Qt::AlignHCenter);
      
      //
      // Threshold radio button
      //
      QRadioButton* threshRadioButton = new QRadioButton("");
      metricThresholdRadioButtons.push_back(threshRadioButton);
      metricThresholdButtonGroup->addButton(threshRadioButton, i);
      metricSelectionGridLayout->addWidget(threshRadioButton, i + 1, 2, Qt::AlignHCenter);
      
      //
      // Comment push button
      //
      QPushButton* commentPushButton = new QPushButton("?");
      commentPushButton->setFixedWidth(40);
      commentPushButton->setAutoDefault(false);
      metricColumnCommentPushButtons.push_back(commentPushButton);
      metricCommentButtonGroup->addButton(commentPushButton, i);
      metricSelectionGridLayout->addWidget(commentPushButton, i + 1, 3, Qt::AlignHCenter);
      
      //
      // Metadata push button
      //
      QPushButton* metaDataPushButton = new QPushButton("M");
      metaDataPushButton->setToolTip("Press the button to set the\n"
                                     "metadata link for this column");
      metaDataPushButton->setFixedWidth(40);
      metaDataPushButton->setAutoDefault(false);
      metricColumnMetaDataPushButtons.push_back(metaDataPushButton);
      metricMetaDataButtonGroup->addButton(metaDataPushButton, i);
      metricSelectionGridLayout->addWidget(metaDataPushButton, i + 1, 4, Qt::AlignHCenter);
      
      //
      // Name line edit
      //
      QLineEdit* metricLineEdit = new QLineEdit;
      metricLineEdit->setText(mf->getColumnName(i));
      metricLineEdit->setMinimumWidth(nameMinimumWidth);
      metricColumnNameLineEdits.push_back(metricLineEdit);
      metricSelectionGridLayout->addWidget(metricLineEdit, i + 1, 5, Qt::AlignLeft);
   }
      
   //
   // Show and update all valid metrics
   //
   for (int i = 0; i < numValidMetrics; i++) {
      metricColumnNameLineEdits[i]->setText(mf->getColumnName(i));
      metricColumnNumberLabels[i]->show();
      metricViewRadioButtons[i]->show();
      metricThresholdRadioButtons[i]->show();
      metricColumnCommentPushButtons[i]->show();
      metricColumnMetaDataPushButtons[i]->show();
      metricColumnNameLineEdits[i]->show();
      metricColumnNameLineEdits[i]->home(true);
   }
   
   //
   // Hide widgets that are not needed
   //
   for (int i = numValidMetrics; i < numExistingMetrics; i++) {
      metricColumnNumberLabels[i]->hide();
      metricViewRadioButtons[i]->hide();
      metricThresholdRadioButtons[i]->hide();
      metricColumnCommentPushButtons[i]->hide();
      metricColumnMetaDataPushButtons[i]->hide();
      metricColumnNameLineEdits[i]->hide();
   }
   
   if (numValidMetrics > 0) {
      if (DebugControl::getDebugOn()) {
         std::cout << " " << std::endl;
         std::cout << "Metric created " << std::endl;
         printPageSizesHelper("pageMetricSelection", pageMetricSelection);
         std::cout << " " << std::endl;
      }
   }
}

/**
 * Page containing metric control.
 */
void
GuiDisplayControlDialog::createMetricSettingsPage()
{
   const int floatSpinBoxWidth = 120;
   
   //
   // Animation section
   //
   QPushButton* animatePushButton = new QPushButton("Animate");
   animatePushButton->setAutoDefault(false);
   animatePushButton->setFixedSize(animatePushButton->sizeHint());
   QObject::connect(animatePushButton, SIGNAL(clicked()),
                    this, SLOT(metricAnimatePushButtonSelection()));
   QLabel* animateLabel = new QLabel(" Interpolate Frames ");
   metricAnimateSpinBox = new QSpinBox;
   metricAnimateSpinBox->setMinimum(0);
   metricAnimateSpinBox->setMaximum(1000);
   metricAnimateSpinBox->setSingleStep(1);
   QGroupBox* animateGroupBox = new QGroupBox("Animation");
   QHBoxLayout* animageGroupLayout = new QHBoxLayout(animateGroupBox);
   animageGroupLayout->addWidget(animatePushButton);
   animageGroupLayout->addWidget(animateLabel);
   animageGroupLayout->addWidget(metricAnimateSpinBox);
   animateGroupBox->setFixedSize(animateGroupBox->sizeHint());
   
   //
   // Popup graph when node identified
   //
   QLabel* popupGraphLabel = new QLabel("Show");
   metricGraphPopupComboBox = new QComboBox;
   metricGraphPopupComboBox->insertItem(DisplaySettingsMetric::METRIC_DATA_PLOT_OFF, 
                                        "Off");
   metricGraphPopupComboBox->insertItem(DisplaySettingsMetric::METRIC_DATA_PLOT_NODE,
                                        "Node");
   metricGraphPopupComboBox->insertItem(DisplaySettingsMetric::METRIC_DATA_PLOT_NODE_AND_NEIGHBORS,
                                        "Node and Neighbors");
   QObject::connect(metricGraphPopupComboBox, SIGNAL(activated(int)),
                    this, SLOT(readMetricSettingsPage()));
   metricGraphManualScaleCheckBox = new QCheckBox("Manual Scaling ");
   QObject::connect(metricGraphManualScaleCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readMetricSettingsPage()));
   const float big = 10000.0;
   metricGraphManualScaleMinDoubleSpinBox = new QDoubleSpinBox;
   metricGraphManualScaleMinDoubleSpinBox->setMinimum(-big);
   metricGraphManualScaleMinDoubleSpinBox->setMaximum(big);
   metricGraphManualScaleMinDoubleSpinBox->setSingleStep(1.0);
   metricGraphManualScaleMinDoubleSpinBox->setDecimals(3);
   QObject::connect(metricGraphManualScaleMinDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
   metricGraphManualScaleMaxDoubleSpinBox = new QDoubleSpinBox;
   metricGraphManualScaleMaxDoubleSpinBox->setMinimum(-big);
   metricGraphManualScaleMaxDoubleSpinBox->setMaximum(big);
   metricGraphManualScaleMaxDoubleSpinBox->setSingleStep(1.0);
   metricGraphManualScaleMaxDoubleSpinBox->setDecimals(3);
   QObject::connect(metricGraphManualScaleMaxDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
   QHBoxLayout* popupGraphShowLayout = new QHBoxLayout;
   popupGraphShowLayout->addWidget(popupGraphLabel);
   popupGraphShowLayout->addWidget(metricGraphPopupComboBox);
   popupGraphShowLayout->addStretch();
   QHBoxLayout* popupGraphScaleLayout = new QHBoxLayout;
   popupGraphScaleLayout->addWidget(metricGraphManualScaleCheckBox);
   popupGraphScaleLayout->addWidget(metricGraphManualScaleMinDoubleSpinBox);
   popupGraphScaleLayout->addWidget(metricGraphManualScaleMaxDoubleSpinBox);
   popupGraphScaleLayout->addStretch();
   QGroupBox* popupGraphGroup = new QGroupBox("Popup Graph When Node Identified");
   QVBoxLayout* popupGraphLayout = new QVBoxLayout(popupGraphGroup);
   popupGraphLayout->addLayout(popupGraphShowLayout);
   popupGraphLayout->addLayout(popupGraphScaleLayout);
   popupGraphGroup->setFixedSize(popupGraphGroup->sizeHint());
   
   //
   // Combo boxes for view and threshold column selection and
   // labels for minimum and maximum values.
   //
   const int minColNameWidth = 250;
   QLabel* viewLabel = new QLabel("View ");
   metricViewLabel = new QLabel("");
   metricViewLabel->setMinimumWidth(minColNameWidth);
   metricViewMinimumLabel = new QLabel("");
   metricViewMaximumLabel = new QLabel("");
   QPushButton* viewHistPushButton = new QPushButton("Histogram...");
   viewHistPushButton->setFixedSize(viewHistPushButton->sizeHint());
   viewHistPushButton->setAutoDefault(false);
   QObject::connect(viewHistPushButton, SIGNAL(clicked()),
                    this, SLOT(metricViewHistogram()));
   QLabel* threshLabel = new QLabel("Thresh ");
   metricThresholdLabel = new QLabel("");
   metricThresholdLabel->setMinimumWidth(minColNameWidth);
   metricThreshMinimumLabel = new QLabel("");
   metricThreshMaximumLabel = new QLabel("");
   QPushButton* threshHistPushButton = new QPushButton("Histogram...");
   threshHistPushButton->setFixedSize(threshHistPushButton->sizeHint());
   threshHistPushButton->setAutoDefault(false);
   QObject::connect(threshHistPushButton, SIGNAL(clicked()),
                    this, SLOT(metricThreshHistogram()));
   QGridLayout* selectionGridLayout = new QGridLayout;
   selectionGridLayout->addWidget(new QLabel("Min"), 0, 2, 1, 1, Qt::AlignHCenter);
   selectionGridLayout->addWidget(new QLabel("Max"), 0, 3, 1, 1, Qt::AlignHCenter);
   selectionGridLayout->addWidget(new QLabel("Name"), 0, 4, 1, 1, Qt::AlignLeft);
   selectionGridLayout->addWidget(viewLabel, 1, 0);
   selectionGridLayout->addWidget(viewHistPushButton, 1, 1);
   selectionGridLayout->addWidget(metricViewMinimumLabel, 1, 2);
   selectionGridLayout->addWidget(metricViewMaximumLabel, 1, 3);
   selectionGridLayout->addWidget(metricViewLabel, 1, 4);
   selectionGridLayout->addWidget(threshLabel, 2, 0);
   selectionGridLayout->addWidget(threshHistPushButton, 2, 1);
   selectionGridLayout->addWidget(metricThreshMinimumLabel, 2, 2);
   selectionGridLayout->addWidget(metricThreshMaximumLabel, 2, 3);
   selectionGridLayout->addWidget(metricThresholdLabel, 2, 4);
   QGroupBox* selectionGroupBox = new QGroupBox("Current Selection");
   QHBoxLayout* selectionGroupLayout = new QHBoxLayout(selectionGroupBox);
   selectionGroupLayout->addLayout(selectionGridLayout);
   selectionGroupLayout->addStretch();
   
   //
   // Color Mapping
   //
   QButtonGroup* scaleButtonGroup = new QButtonGroup(this);
   QObject::connect(scaleButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(readMetricSettingsPage()));
   metricFileAutoScaleRadioButton = new QRadioButton("Auto Scale - Metric");
   metricFileAutoScaleRadioButton->setToolTip(
                 "This selection will map the most negative\n"
                 "and positive values in the selected metric\n"
                 "column to the -1.0 and 1.0 values in the\n"
                 "selected palette.");
   scaleButtonGroup->addButton(metricFileAutoScaleRadioButton, DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_METRIC);

   metricFuncVolumeAutoScaleRadioButton = new QRadioButton("Auto Scale - Functional Volume");
   metricFuncVolumeAutoScaleRadioButton->setToolTip(
                 "This selection will map the most negative\n"
                 "and positive values in the selected functional\n"
                 "volume to the -1.0 and 1.0 values in the\n"
                 "selected palette.");
   scaleButtonGroup->addButton(metricFuncVolumeAutoScaleRadioButton, DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_FUNC_VOLUME);

   metricUserScaleRadioButton = new QRadioButton("User Scale");
   metricUserScaleRadioButton->setToolTip(
                 "This selection will map the Pos Max value\n"
                 "to the 1.0 value in the palette, the Pos\n"
                 "Min value to the 0.0 value in the palettte,\n"
                 "the Neg Min value to the 0.0 value in the \n"
                 "palette, and the Neg Max value to the -1.0\n"
                 "value in the palette.");
   scaleButtonGroup->addButton(metricUserScaleRadioButton, DisplaySettingsMetric::METRIC_OVERLAY_SCALE_USER);
   
   QLabel* colorMapPosMaxLabel = new QLabel("Pos Max");
   metricColorPositiveMaxDoubleSpinBox = new QDoubleSpinBox;
   metricColorPositiveMaxDoubleSpinBox->setMinimum(0);
   metricColorPositiveMaxDoubleSpinBox->setMaximum(10000000.0);
   metricColorPositiveMaxDoubleSpinBox->setSingleStep(1.0);
   metricColorPositiveMaxDoubleSpinBox->setDecimals(6);
   metricColorPositiveMaxDoubleSpinBox->setToolTip(
                 "This positive metric value is mapped to\n"
                 "the 1.0 value in the color palette.");
   QObject::connect(metricColorPositiveMaxDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
                    
   QLabel* colorMapPosMinLabel = new QLabel("Pos Min (>) ");
   metricColorPositiveMinDoubleSpinBox = new QDoubleSpinBox;
   metricColorPositiveMinDoubleSpinBox->setMinimum(0);
   metricColorPositiveMinDoubleSpinBox->setMaximum(10000000.0);
   metricColorPositiveMinDoubleSpinBox->setSingleStep(1.0);
   metricColorPositiveMinDoubleSpinBox->setDecimals(6);
   metricColorPositiveMinDoubleSpinBox->setToolTip(
                 "This positive metric value is mapped to\n"
                 "the 0.0 value in the color palette.");
   QObject::connect(metricColorPositiveMinDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
   
   QLabel* colorMapNegMinLabel = new QLabel("Neg Min (<=)");
   metricColorNegativeMinDoubleSpinBox = new QDoubleSpinBox;
   metricColorNegativeMinDoubleSpinBox->setMinimum(-10000000.0);
   metricColorNegativeMinDoubleSpinBox->setMaximum(0.0);
   metricColorNegativeMinDoubleSpinBox->setSingleStep(1.0);
   metricColorNegativeMinDoubleSpinBox->setDecimals(6);
   metricColorNegativeMinDoubleSpinBox->setToolTip(
                 "This negative metric value is mapped to\n"
                 "the 0.0 value in the color palette.");
   QObject::connect(metricColorNegativeMinDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
   
   QLabel* colorMapNegMaxLabel = new QLabel("Neg Max ");
   metricColorNegativeMaxDoubleSpinBox = new QDoubleSpinBox;
   metricColorNegativeMaxDoubleSpinBox->setMinimum(-10000000.0);
   metricColorNegativeMaxDoubleSpinBox->setMaximum(0.0);
   metricColorNegativeMaxDoubleSpinBox->setSingleStep(1.0);
   metricColorNegativeMaxDoubleSpinBox->setDecimals(6);
   metricColorNegativeMaxDoubleSpinBox->setToolTip(
                 "This negative metric value is mapped to\n"
                 "the -1.0 value in the color palette.");
   QObject::connect(metricColorNegativeMaxDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
                    
   metricColorInterpolateCheckBox = new QCheckBox("Interpolate");
   QObject::connect(metricColorInterpolateCheckBox, SIGNAL(clicked()),
                    this, SLOT(readMetricSettingsPage()));
   
   QGridLayout* colorMapGridLayout = new QGridLayout;
   colorMapGridLayout->setColumnMinimumWidth(0, 25);
   colorMapGridLayout->addWidget(colorMapPosMaxLabel, 0, 1);
   colorMapGridLayout->addWidget(metricColorPositiveMaxDoubleSpinBox, 0, 2);
   colorMapGridLayout->addWidget(colorMapPosMinLabel, 1, 1);
   colorMapGridLayout->addWidget(metricColorPositiveMinDoubleSpinBox, 1, 2);
   colorMapGridLayout->addWidget(colorMapNegMinLabel, 2, 1);
   colorMapGridLayout->addWidget(metricColorNegativeMinDoubleSpinBox, 2, 2);
   colorMapGridLayout->addWidget(colorMapNegMaxLabel, 3, 1);
   colorMapGridLayout->addWidget(metricColorNegativeMaxDoubleSpinBox, 3, 2);
   QGroupBox* colorGroupBox = new QGroupBox("Color Mapping");
   QVBoxLayout* colorGroupLayout = new QVBoxLayout(colorGroupBox);
   colorGroupLayout->addWidget(metricFileAutoScaleRadioButton);
   colorGroupLayout->addWidget(metricFuncVolumeAutoScaleRadioButton);
   colorGroupLayout->addWidget(metricUserScaleRadioButton);
   colorGroupLayout->addLayout(colorMapGridLayout);
   colorGroupLayout->addWidget(metricColorInterpolateCheckBox);
   colorGroupBox->setFixedSize(colorGroupBox->sizeHint());
   
   //
   // Display Mode Buttons
   //
   metricDisplayModeBothRadioButton     = new QRadioButton("Both");
   metricDisplayModePositiveRadioButton = new QRadioButton("Positive");
   metricDisplayModeNegativeRadioButton = new QRadioButton("Negative");
   
   QButtonGroup* displayModeButtonGroup = new QButtonGroup(this);
   QObject::connect(displayModeButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(readMetricSettingsPage()));
   displayModeButtonGroup->addButton(metricDisplayModeBothRadioButton, DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_AND_NEGATIVE);
   displayModeButtonGroup->addButton(metricDisplayModePositiveRadioButton, DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_ONLY);
   displayModeButtonGroup->addButton(metricDisplayModeNegativeRadioButton, DisplaySettingsMetric::METRIC_DISPLAY_MODE_NEGATIVE_ONLY);

   QGroupBox* displayModeGroupBox = new QGroupBox("Display Mode");
   QVBoxLayout* displayModeGroupLayout = new QVBoxLayout(displayModeGroupBox);
   displayModeGroupLayout->addWidget(metricDisplayModePositiveRadioButton);
   displayModeGroupLayout->addWidget(metricDisplayModeNegativeRadioButton);
   displayModeGroupLayout->addWidget(metricDisplayModeBothRadioButton);
   displayModeGroupBox->setFixedSize(displayModeGroupBox->sizeHint());
   
   //
   // Palette Selection
   //
   metricPaletteComboBox = new QComboBox;
   QObject::connect(metricPaletteComboBox, SIGNAL(activated(int)),
                    this, SLOT(metricPaletteSelection(int)));
   metricDisplayColorBarCheckBox = new QCheckBox("Display Color Bar");
   QObject::connect(metricDisplayColorBarCheckBox, SIGNAL(clicked()),
                    this, SLOT(readMetricSettingsPage()));
   QGroupBox* paletteGroupBox = new QGroupBox("Palette");
   QVBoxLayout* paletteLayout = new QVBoxLayout(paletteGroupBox);
   paletteLayout->addWidget(metricPaletteComboBox);
   paletteLayout->addWidget(metricDisplayColorBarCheckBox);
   
   //
   // Threshold line edits
   //
   QLabel* threshColLabel = new QLabel("Column");
   QLabel* threshAvgAreaLabel = new QLabel("Avg Area");
   QLabel* threshUserLabel = new QLabel("User");
   QLabel* threshPosLabel = new QLabel("Pos");
   QLabel* threshNegLabel = new QLabel("Neg");
   metricThresholdColumnPositiveDoubleSpinBox = new QDoubleSpinBox;
   metricThresholdColumnPositiveDoubleSpinBox->setMinimum(-10000000.0);
   metricThresholdColumnPositiveDoubleSpinBox->setMaximum( 10000000.0);
   metricThresholdColumnPositiveDoubleSpinBox->setSingleStep(1.0);
   metricThresholdColumnPositiveDoubleSpinBox->setDecimals(6);
   metricThresholdColumnPositiveDoubleSpinBox->setMaximumWidth(floatSpinBoxWidth);
   metricThresholdColumnPositiveDoubleSpinBox->setToolTip(
                 "Positive metric values less positive\n"
                 "than this amount are not shown when\n"
                 "threshold type COLUMN is selected. This\n"
                 "value comes from and is stored in the\n"
                 "metric file.");
   QObject::connect(metricThresholdColumnPositiveDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
                    
   metricThresholdAveragePositiveDoubleSpinBox = new QDoubleSpinBox;
   metricThresholdAveragePositiveDoubleSpinBox->setMinimum(-10000000.0);
   metricThresholdAveragePositiveDoubleSpinBox->setMaximum( 10000000.0);
   metricThresholdAveragePositiveDoubleSpinBox->setSingleStep(1.0);
   metricThresholdAveragePositiveDoubleSpinBox->setDecimals(6);
   metricThresholdAveragePositiveDoubleSpinBox->setMaximumWidth(floatSpinBoxWidth);
   metricThresholdAveragePositiveDoubleSpinBox->setToolTip(
                 "Positive metric values less positive\n"
                 "than this amount are not shown when\n"
                 "threshold type AVERAGE AREA is selected.\n"
                 "This value comes from and is stored in the\n"
                 "the metric file.");
   QObject::connect(metricThresholdAveragePositiveDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
                    
   metricThresholdUserPositiveDoubleSpinBox = new QDoubleSpinBox;
   metricThresholdUserPositiveDoubleSpinBox->setMinimum(-10000000.0);
   metricThresholdUserPositiveDoubleSpinBox->setMaximum( 10000000.0);
   metricThresholdUserPositiveDoubleSpinBox->setSingleStep(0.05);
   metricThresholdUserPositiveDoubleSpinBox->setDecimals(6);
   metricThresholdUserPositiveDoubleSpinBox->setMaximumWidth(floatSpinBoxWidth);
   metricThresholdUserPositiveDoubleSpinBox->setToolTip(
                 "Positive metric values less positive\n"
                 "than this amount are not shown when\n"
                 "threshold type USER is selected.");
   QObject::connect(metricThresholdUserPositiveDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
                    
   metricThresholdColumnNegativeDoubleSpinBox = new QDoubleSpinBox;
   metricThresholdColumnNegativeDoubleSpinBox->setMinimum(-10000000.0);
   metricThresholdColumnNegativeDoubleSpinBox->setMaximum( 10000000.0);
   metricThresholdColumnNegativeDoubleSpinBox->setSingleStep(1.0);
   metricThresholdColumnNegativeDoubleSpinBox->setDecimals(6);
   metricThresholdColumnNegativeDoubleSpinBox->setMaximumWidth(floatSpinBoxWidth);
   metricThresholdColumnNegativeDoubleSpinBox->setToolTip(
                 "Negative metric values less negative\n"
                 "than this amount are not shown when\n"
                 "threshold type COLUMN is selected. This\n"
                 "value comes from and is stored in the\n"
                 "metric file.");
   QObject::connect(metricThresholdColumnNegativeDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
                    
   metricThresholdAverageNegativeDoubleSpinBox = new QDoubleSpinBox;
   metricThresholdAverageNegativeDoubleSpinBox->setMinimum(-10000000.0);
   metricThresholdAverageNegativeDoubleSpinBox->setMaximum( 10000000.0);
   metricThresholdAverageNegativeDoubleSpinBox->setSingleStep(1.0);
   metricThresholdAverageNegativeDoubleSpinBox->setDecimals(6);
   metricThresholdAverageNegativeDoubleSpinBox->setMaximumWidth(floatSpinBoxWidth);
   metricThresholdAverageNegativeDoubleSpinBox->setToolTip(
                 "Negative metric values less negative\n"
                 "than this amount are not shown when\n"
                 "threshold type AVERAGE AREA is selected.\n"
                 "This value comes from and is stored in\n"
                 "the metric file.");
   QObject::connect(metricThresholdAverageNegativeDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
                    
   metricThresholdUserNegativeDoubleSpinBox = new QDoubleSpinBox;
   metricThresholdUserNegativeDoubleSpinBox->setMinimum(-10000000.0);
   metricThresholdUserNegativeDoubleSpinBox->setMaximum( 10000000.0);
   metricThresholdUserNegativeDoubleSpinBox->setSingleStep(0.05);
   metricThresholdUserNegativeDoubleSpinBox->setDecimals(6);
   metricThresholdUserNegativeDoubleSpinBox->setMaximumWidth(floatSpinBoxWidth);
   metricThresholdUserNegativeDoubleSpinBox->setToolTip(
                 "Negative metric values less negative\n"
                 "than this amount are not shown when\n"
                 "threshold type USER is selected.");
   QObject::connect(metricThresholdUserNegativeDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readMetricSettingsPage()));
                    
   metricShowThresholdedRegionsCheckBox = new QCheckBox("Show Subthresh Region Green");
   metricShowThresholdedRegionsCheckBox->setToolTip(
                 "When checked, nodes/voxels that fail\n"
                 "the threshold test are colored in green.\n"
                 "Nodes/voxels with zero values are not\n"
                 "colored.");
   QObject::connect(metricShowThresholdedRegionsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readMetricSettingsPage()));
      
   QLabel* threshTypeLabel = new QLabel("Threshold Type ");
   metricThresholdTypeComboBox = new QComboBox;
   QObject::connect(metricThresholdTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(readMetricSettingsPage()));
   metricThresholdTypeComboBox->insertItem(DisplaySettingsMetric::METRIC_THRESHOLDING_TYPE_FILE_COLUMN,
                                           "Column");
   metricThresholdTypeComboBox->insertItem(DisplaySettingsMetric::METRIC_THRESHOLDING_TYPE_FILE_COLUMN_AVERAGE,
                                           "Average Area");
   metricThresholdTypeComboBox->insertItem(DisplaySettingsMetric::METRIC_THRESHOLDING_TYPE_USER_VALUES,
                                           "User");

   QGridLayout* threshLayout = new QGridLayout;
   threshLayout->addWidget(threshColLabel, 0, 1);
   threshLayout->addWidget(threshAvgAreaLabel, 0, 2);
   threshLayout->addWidget(threshUserLabel, 0, 3);
   threshLayout->addWidget(threshPosLabel, 1, 0);
   threshLayout->addWidget(metricThresholdColumnPositiveDoubleSpinBox, 1, 1);
   threshLayout->addWidget(metricThresholdAveragePositiveDoubleSpinBox, 1, 2);
   threshLayout->addWidget(metricThresholdUserPositiveDoubleSpinBox, 1, 3);
   threshLayout->addWidget(threshNegLabel, 2, 0);
   threshLayout->addWidget(metricThresholdColumnNegativeDoubleSpinBox, 2, 1);
   threshLayout->addWidget(metricThresholdAverageNegativeDoubleSpinBox, 2, 2);
   threshLayout->addWidget(metricThresholdUserNegativeDoubleSpinBox, 2, 3);
   QHBoxLayout* threshTypeLayout = new QHBoxLayout;
   threshTypeLayout->addWidget(threshTypeLabel);
   threshTypeLayout->addWidget(metricThresholdTypeComboBox);
   QVBoxLayout* threshRightLayout = new QVBoxLayout;
   threshRightLayout->addWidget(metricShowThresholdedRegionsCheckBox);
   threshRightLayout->addLayout(threshTypeLayout);
   QGroupBox* thresholdGroupBox = new QGroupBox("Threshold");
   QHBoxLayout* thresholdGroupLayout = new QHBoxLayout(thresholdGroupBox);
   thresholdGroupLayout->addLayout(threshLayout);
   thresholdGroupLayout->addLayout(threshRightLayout);
   thresholdGroupBox->setFixedSize(thresholdGroupBox->sizeHint());
   
   //
   // Metric page and all layouts
   //
   QVBoxLayout* rightLayout = new QVBoxLayout;
   rightLayout->addWidget(displayModeGroupBox);
   rightLayout->addWidget(paletteGroupBox);
   QHBoxLayout* miscLayout = new QHBoxLayout;
   miscLayout->addWidget(colorGroupBox);
   miscLayout->addLayout(rightLayout);
   miscLayout->addStretch();
   
   pageMetricSettings = new QWidget;
   QVBoxLayout* metricLayout = new QVBoxLayout(pageMetricSettings);
   metricLayout->addWidget(selectionGroupBox);
   metricLayout->addLayout(miscLayout);
   metricLayout->addWidget(thresholdGroupBox);
   metricLayout->addWidget(animateGroupBox);
   metricLayout->addWidget(popupGraphGroup);
   pageMetricSettings->setFixedSize(pageMetricSettings->sizeHint());

   pageWidgetStack->addWidget(pageMetricSettings);
   
   pageMetricSettingsWidgetGroup = new WuQWidgetGroup(this);
   pageMetricSettingsWidgetGroup->addWidget(metricGraphPopupComboBox);
   pageMetricSettingsWidgetGroup->addWidget(metricGraphManualScaleMinDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricGraphManualScaleMaxDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricFileAutoScaleRadioButton);
   pageMetricSettingsWidgetGroup->addWidget(metricFuncVolumeAutoScaleRadioButton);
   pageMetricSettingsWidgetGroup->addWidget(metricUserScaleRadioButton);
   pageMetricSettingsWidgetGroup->addWidget(metricColorPositiveMaxDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricColorPositiveMinDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricColorNegativeMinDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricColorNegativeMaxDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricColorInterpolateCheckBox);
   pageMetricSettingsWidgetGroup->addWidget(metricDisplayModeBothRadioButton);
   pageMetricSettingsWidgetGroup->addWidget(metricDisplayModePositiveRadioButton);
   pageMetricSettingsWidgetGroup->addWidget(metricDisplayModeNegativeRadioButton);
   pageMetricSettingsWidgetGroup->addWidget(metricPaletteComboBox);
   pageMetricSettingsWidgetGroup->addWidget(metricDisplayColorBarCheckBox);
   pageMetricSettingsWidgetGroup->addWidget(metricThresholdColumnPositiveDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricThresholdAveragePositiveDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricThresholdUserPositiveDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricThresholdColumnNegativeDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricThresholdAverageNegativeDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricThresholdUserNegativeDoubleSpinBox);
   pageMetricSettingsWidgetGroup->addWidget(metricShowThresholdedRegionsCheckBox);
   pageMetricSettingsWidgetGroup->addWidget(metricThresholdTypeComboBox);
}

/**
 * called when metric view column histogram pushbutton is pressed.
 */
void 
GuiDisplayControlDialog::metricViewHistogram()
{
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   const int column = dsm->getSelectedDisplayColumn(surfaceModelIndex);
   if ((column >= 0) && (column < mf->getNumberOfColumns())) {
      const int numNodes = mf->getNumberOfNodes();
      std::vector<float> values(numNodes);
      for (int i = 0; i < numNodes; i++) {
         values[i] = mf->getValue(i, column);
      }
      
      GuiHistogramDisplayDialog* ghd = new GuiHistogramDisplayDialog(theMainWindow, 
                                             mf->getColumnName(column),
                                             values,
                                             false,
                                             false);
      ghd->show();
   }
}

/**
 * called when metric thresh column histogram pushbutton is pressed.
 */
void 
GuiDisplayControlDialog::metricThreshHistogram()
{
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   const int column = dsm->getSelectedThresholdColumn(surfaceModelIndex);
   if ((column >= 0) && (column < mf->getNumberOfColumns())) {
      const int numNodes = mf->getNumberOfNodes();
      std::vector<float> values(numNodes);
      for (int i = 0; i < numNodes; i++) {
         values[i] = mf->getValue(i, column);
      }
      GuiHistogramDisplayDialog* ghd = new GuiHistogramDisplayDialog(theMainWindow, 
                                             mf->getColumnName(column),
                                             values,
                                             false,
                                             false);
      ghd->show();
   }
}

/**
 * Called when a metric palette is selected using the metric palette combo box.
 */
void
GuiDisplayControlDialog::metricPaletteSelection(int itemNum)
{
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   dsm->setSelectedPaletteIndex(itemNum);
   readMetricSettingsPage();
   updateMetricSettingsPage();
}

/**
 * Called when animate pushbutton is pressed.
 */
void
GuiDisplayControlDialog::metricAnimatePushButtonSelection()
{
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   const int numMetrics = mf->getNumberOfColumns();
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   const int numInterpolateFrames = metricAnimateSpinBox->value();
   BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
   
   if (numInterpolateFrames > 0) {
      for (int i = 0; i < (numMetrics - 1); i++) {
         //
         // Set current metrics
         //
         dsm->setSelectedDisplayColumn(surfaceModelIndex, i);
         dsm->setSelectedThresholdColumn(surfaceModelIndex, i);
         if (pageOverlayUnderlaySurface != NULL) {
            metricSelectionComboBox->setCurrentIndex(i);
         }
         QRadioButton* rb = dynamic_cast<QRadioButton*>(metricViewButtonGroup->button(i));
         rb->setChecked(true);
         QRadioButton* rbt = dynamic_cast<QRadioButton*>(metricThresholdButtonGroup->button(dsm->getSelectedThresholdColumn(surfaceModelIndex)));
         rbt->setChecked(true);
         
         //
         // Assign colors with current metric and save the colors
         //
         bsnc->assignColors();
         const int numNodes = mf->getNumberOfNodes();
         const int colorSize = numNodes * 3;
         unsigned char* colors = new unsigned char[colorSize];
         for (int k = 0; k < numNodes; k++) {
            const unsigned char* c = bsnc->getNodeColor(surfaceModelIndex, k);
            colors[k * 3] = c[0];
            colors[k * 3 + 1] = c[1];
            colors[k * 3 + 2] = c[2];
         }
         
         //
         // Assign colors with next metric and save the colors
         //
         dsm->setSelectedDisplayColumn(surfaceModelIndex, i + 1);
         dsm->setSelectedThresholdColumn(surfaceModelIndex, i + 1);
         bsnc->assignColors();
         unsigned char* colors2 = new unsigned char[colorSize];
         for (int k = 0; k < numNodes; k++) {
            const unsigned char* c = bsnc->getNodeColor(surfaceModelIndex, k);
            colors2[k * 3] = c[0];
            colors2[k * 3 + 1] = c[1];
            colors2[k * 3 + 2] = c[2];
         }
         
         //
         // Compute the delta of the colors
         //
         const float steps = numInterpolateFrames + 1.0;
         float* colorDelta = new float[colorSize];
         for (int k = 0; k < colorSize; k++) {
            colorDelta[k] = static_cast<float>(colors2[k] - colors[k]) / steps;
         }
         
         for (int j = 0; j <= numInterpolateFrames; j++) {
            for (int m = 0; m < numNodes; m++) {
               //
               // Interpolate color deltas
               //
               const int m3 = m * 3;
               const float rd = colorDelta[m3] * j;
               const float gd = colorDelta[m3 + 1] * j;
               const float bd = colorDelta[m3 + 2] * j;
               
               //
               // Assign colors to the nodes
               //
               const unsigned char n255 = 255;
               unsigned char rgb[3];
               rgb[0] = std::min(static_cast<unsigned char>(colors[m3] + rd), n255);
               rgb[1] = std::min(static_cast<unsigned char>(colors[m3 + 1] + gd), n255);
               rgb[2] = std::min(static_cast<unsigned char>(colors[m3 + 2] + bd), n255);
               bsnc->setNodeColor(surfaceModelIndex, m, rgb);
               
/*
               if (m == 26545) {
                  std::cout << "metric# " << i << " frame: " << j 
                            << " rgb: " << static_cast<int>(rgb[0])
                            << " " << static_cast<int>(rgb[1])
                            << " " << static_cast<int>(rgb[2])
                            << std::endl;
               }
*/
            }
               
            //
            // Draw the surfaces
            //
            GuiBrainModelOpenGL::updateAllGL(NULL); 
         }
         
         delete[] colors;
         delete[] colors2;
         delete[] colorDelta;
      }
      //
      // Set to last metric and draw it
      //
      dsm->setSelectedDisplayColumn(surfaceModelIndex, numMetrics - 1);
      dsm->setSelectedThresholdColumn(surfaceModelIndex, numMetrics - 1);
      if (pageOverlayUnderlaySurface != NULL) {
         metricSelectionComboBox->setCurrentIndex(numMetrics - 1);
      }
      QRadioButton* rb = dynamic_cast<QRadioButton*>(metricViewButtonGroup->button(numMetrics - 1));
      rb->setChecked(true);
      QRadioButton* rb2 = dynamic_cast<QRadioButton*>(metricThresholdButtonGroup->button(dsm->getSelectedThresholdColumn(surfaceModelIndex)));
      rb2->setChecked(true);
      GuiBrainModelOpenGL::updateAllGL(NULL); 
   }
   else {
      for (int i = 0; i < numMetrics; i++) {
         dsm->setSelectedDisplayColumn(surfaceModelIndex, i);
         dsm->setSelectedThresholdColumn(surfaceModelIndex, i);
         if (pageOverlayUnderlaySurface != NULL) {
            metricSelectionComboBox->setCurrentIndex(i);
         }
         QRadioButton* rb = dynamic_cast<QRadioButton*>(metricViewButtonGroup->button(i));
         rb->setChecked(true);
         QRadioButton* rb2 = dynamic_cast<QRadioButton*>(metricThresholdButtonGroup->button(dsm->getSelectedThresholdColumn(surfaceModelIndex)));
         rb2->setChecked(true);
         bsnc->assignColors();
         GuiBrainModelOpenGL::updateAllGL(NULL);   
      }
   }
}

/**
 * update the metric selection page.
 */
void 
GuiDisplayControlDialog::updateMetricSelectionPage()
{
   if (pageMetricSelection == NULL) {
      return;
   }

   createAndUpdateMetricSelectionPage();
   
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   if (mf->getNumberOfColumns() > 0) {
      const int numView = metricViewButtonGroup->buttons().count();
      const int viewCol = dsm->getSelectedDisplayColumn(surfaceModelIndex);
      if ((viewCol >= 0) && (viewCol < numView)) {
         QRadioButton* rb = dynamic_cast<QRadioButton*>(metricViewButtonGroup->button(viewCol));
         rb->setChecked(true);
      }
      
      const int numThresh = metricThresholdButtonGroup->buttons().count();
      const int threshCol = dsm->getSelectedThresholdColumn(surfaceModelIndex);
      if ((threshCol >= 0) && (threshCol < numThresh)) {
         QRadioButton* rb2 = dynamic_cast<QRadioButton*>(metricThresholdButtonGroup->button(threshCol));
         rb2->setChecked(true);      
      }
   }
   
   pageMetricSelection->setEnabled(validMetricData);
}

/**
 * update the metric settings page.
 */
void 
GuiDisplayControlDialog::updateMetricSettingsPage()
{
   if (pageMetricSettings == NULL) {
      return;
   }
   pageMetricSettingsWidgetGroup->blockSignals(true);

   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   if (mf->getNumberOfColumns() > 0) {
      float colMin, colMax;
      mf->getDataColumnMinMax(dsm->getSelectedDisplayColumn(surfaceModelIndex), colMin, colMax);
      metricViewMinimumLabel->setText(QString::number(colMin, 'f', 6));
      metricViewMaximumLabel->setText(QString::number(colMax, 'f', 6));
      mf->getDataColumnMinMax(dsm->getSelectedThresholdColumn(surfaceModelIndex), colMin, colMax);
      metricThreshMinimumLabel->setText(QString::number(colMin, 'f', 6));
      metricThreshMaximumLabel->setText(QString::number(colMax, 'f', 6));
      
      metricViewLabel->setText(mf->getColumnName(dsm->getSelectedDisplayColumn(surfaceModelIndex)));
      metricThresholdLabel->setText(mf->getColumnName(dsm->getSelectedThresholdColumn(surfaceModelIndex)));

      float negThresh, posThresh;
      mf->getColumnThresholding(dsm->getSelectedThresholdColumn(surfaceModelIndex),
                                                                 negThresh,
                                                                 posThresh);
      metricThresholdColumnPositiveDoubleSpinBox->setValue(posThresh);
      metricThresholdColumnNegativeDoubleSpinBox->setValue(negThresh);
      mf->getColumnAverageThresholding(dsm->getSelectedThresholdColumn(surfaceModelIndex),
                                                                       negThresh,
                                                                       posThresh);
      metricThresholdAveragePositiveDoubleSpinBox->setValue(posThresh);
      metricThresholdAverageNegativeDoubleSpinBox->setValue(negThresh);
   }
   float negThresh, posThresh;
   dsm->getUserThresholdingValues(negThresh, posThresh);
   metricThresholdUserPositiveDoubleSpinBox->setValue(posThresh);
   metricThresholdUserNegativeDoubleSpinBox->setValue(negThresh);
   metricThresholdTypeComboBox->setCurrentIndex(dsm->getMetricThresholdingType());
   
   //
   // Color Mapping auto scale radio buttons, pos/neg max line edits, and interpolate radio button
   //
   switch(dsm->getSelectedOverlayScale()) {
      case DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_METRIC:
         metricFileAutoScaleRadioButton->setChecked(true);
         break;
      case DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_FUNC_VOLUME:
         metricFuncVolumeAutoScaleRadioButton->setChecked(true);
         break;
      case DisplaySettingsMetric::METRIC_OVERLAY_SCALE_USER:
         metricUserScaleRadioButton->setChecked(true);
         break;
   }
   
   float posMinMetric = 0.0, posMaxMetric = 0.0, negMinMetric = 0.0, negMaxMetric = 0.0;
   dsm->getUserScaleMinMax(posMinMetric, posMaxMetric, negMinMetric, negMaxMetric);
   metricColorPositiveMinDoubleSpinBox->setValue(posMinMetric);
   metricColorPositiveMaxDoubleSpinBox->setValue(posMaxMetric);
   metricColorNegativeMinDoubleSpinBox->setValue(negMinMetric);
   metricColorNegativeMaxDoubleSpinBox->setValue(negMaxMetric);
   
   metricColorInterpolateCheckBox->setChecked(dsm->getInterpolateColors());
   
   metricGraphPopupComboBox->setCurrentIndex(dsm->getMDataPlotOnNodeID());
   float manMin, manMax;
   const bool manFlag = dsm->getDataPlotManualScaling(manMin, manMax);
   metricGraphManualScaleCheckBox->setChecked(manFlag);
   metricGraphManualScaleMinDoubleSpinBox->setValue(manMin);
   metricGraphManualScaleMaxDoubleSpinBox->setValue(manMax);
   
   //
   // Threshold positive and negative
   //
   metricShowThresholdedRegionsCheckBox->setChecked(dsm->getShowSpecialColorForThresholdedNodes());
   
   //
   // Display mode positive, negative, or both
   //
   switch(dsm->getDisplayMode()) {
      case DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_ONLY:
         metricDisplayModePositiveRadioButton->setChecked(true);
         break;
      case DisplaySettingsMetric::METRIC_DISPLAY_MODE_NEGATIVE_ONLY:
         metricDisplayModeNegativeRadioButton->setChecked(true);
         break;
      case DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_AND_NEGATIVE:
         metricDisplayModeBothRadioButton->setChecked(true);
         break;
   }
   
   //
   // Palette selection and display color bar
   //
   metricDisplayColorBarCheckBox->setChecked(dsm->getDisplayColorBar());
   metricPaletteComboBox->clear();
   PaletteFile* pf = theMainWindow->getBrainSet()->getPaletteFile();
   const int numPalettes = pf->getNumberOfPalettes();
   for (int i = 0; i < numPalettes; i++) {
      const Palette* p = pf->getPalette(i);
      QString name(p->getName());
      if (p->getPositiveOnly()) {
         name.append("+");
      }
      metricPaletteComboBox->addItem(name);
   }
   metricPaletteComboBox->setCurrentIndex(dsm->getSelectedPaletteIndex());

   const bool volumeValid = (theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles() > 0);
   pageMetricSettings->setEnabled(validMetricData | volumeValid);

   pageMetricSettingsWidgetGroup->blockSignals(false);
}
      
/**
 * update metric overlay/underlay selection.
 */
void 
GuiDisplayControlDialog::updateMetricOverlayUnderlaySelection()
{
   if (pageOverlayUnderlaySurface == NULL) {
      return;
   }
   
   //
   // Update metric combo box on overlay/underlay panel and the 
   // two combo boxes on the metric sub panel
   //
   metricSelectionComboBox->clear();
   
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   bool valid = false;
   if (mf->getNumberOfColumns() > 0) {
      valid = true;
      for (int i = 0; i < mf->getNumberOfColumns(); i++) {
         metricSelectionComboBox->addItem(mf->getColumnName(i));
      }
   
      metricSelectionComboBox->setCurrentIndex(dsm->getSelectedDisplayColumn(surfaceModelIndex));
      const int colNum = dsm->getSelectedDisplayColumn(surfaceModelIndex);
      if ((colNum >= 0) && (colNum < mf->getNumberOfColumns())) {
         metricSelectionComboBox->setToolTip( mf->getColumnName(colNum));
      }
   }
   
   primaryOverlayMetricButton->setEnabled(validMetricData);
   secondaryOverlayMetricButton->setEnabled(validMetricData);
   underlayMetricButton->setEnabled(validMetricData);
   metricSelectionLabel->setEnabled(validMetricData);
   metricSelectionComboBox->setEnabled(validMetricData);
   metricInfoPushButton->setEnabled(validMetricData);
}

/**
 * Update all metric items in the dialog.
 */
void
GuiDisplayControlDialog::updateMetricItems()
{
   updatePageSelectionComboBox();
   
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   metricApplySelectionToLeftAndRightStructuresFlagCheckBox->setChecked(dsm->getApplySelectionToLeftAndRightStructuresFlag());
   
   
   updateMetricOverlayUnderlaySelection();
   
   updateMetricSelectionPage();
   updateMetricSettingsPage();
}

/**
 * read the metric settings page.
 */
void 
GuiDisplayControlDialog::readMetricSettingsPage()
{
   if (creatingDialog) {
      return;
   }
   if (pageMetricSettings == NULL) {
      return;
   }
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();

   //
   // Get display mode
   //
   if (metricDisplayModePositiveRadioButton->isChecked()) {
      dsm->setDisplayMode(DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_ONLY);
   }
   else if (metricDisplayModeNegativeRadioButton->isChecked()) {
      dsm->setDisplayMode(DisplaySettingsMetric::METRIC_DISPLAY_MODE_NEGATIVE_ONLY);
   }
   else {
      dsm->setDisplayMode(DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_AND_NEGATIVE);
   }
   
   //
   // Get auto/user scale, pos/neg max, and interpolate color mapping
   //
   if (metricFileAutoScaleRadioButton->isChecked()) {
      dsm->setSelectedOverlayScale(DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_METRIC);
   }
   else if (metricFuncVolumeAutoScaleRadioButton->isChecked()) {
      dsm->setSelectedOverlayScale(DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_FUNC_VOLUME);
   }
   else {
      dsm->setSelectedOverlayScale(DisplaySettingsMetric::METRIC_OVERLAY_SCALE_USER);
   }
   dsm->setUserScaleMinMax(metricColorPositiveMinDoubleSpinBox->value(),
                           metricColorPositiveMaxDoubleSpinBox->value(),
                           metricColorNegativeMinDoubleSpinBox->value(),
                           metricColorNegativeMaxDoubleSpinBox->value());
   dsm->setInterpolateColors(metricColorInterpolateCheckBox->isChecked());
                           
   //
   // Get negative and positive threshold
   //
   dsm->setShowSpecialColorForThresholdedNodes(metricShowThresholdedRegionsCheckBox->isChecked());
   
   //
   // Get palette index and display color bar flag
   //
   dsm->setSelectedPaletteIndex(metricPaletteComboBox->currentIndex());
   dsm->setDisplayColorBar(metricDisplayColorBarCheckBox->isChecked());

   dsm->setDataPlotOnNodeID(static_cast<DisplaySettingsMetric::METRIC_DATA_PLOT>(
                                              metricGraphPopupComboBox->currentIndex()));
   dsm->setDataPlotManualScaling(metricGraphManualScaleCheckBox->isChecked(),
                                 metricGraphManualScaleMinDoubleSpinBox->value(),
                                 metricGraphManualScaleMaxDoubleSpinBox->value());

   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   if (mf->getNumberOfColumns() > 0) {
      mf->setColumnThresholding(dsm->getSelectedThresholdColumn(surfaceModelIndex),
                                metricThresholdColumnNegativeDoubleSpinBox->value(),
                                metricThresholdColumnPositiveDoubleSpinBox->value());
      mf->setColumnAverageThresholding(dsm->getSelectedThresholdColumn(surfaceModelIndex),
                                metricThresholdAverageNegativeDoubleSpinBox->value(),
                                metricThresholdAveragePositiveDoubleSpinBox->value());
   }
   dsm->setUserThresholdingValues(metricThresholdUserNegativeDoubleSpinBox->value(),
                                  metricThresholdUserPositiveDoubleSpinBox->value());
   dsm->setMetricThresholdingType(
      static_cast<DisplaySettingsMetric::METRIC_THRESHOLDING_TYPE>(
         metricThresholdTypeComboBox->currentIndex()));

   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   theMainWindow->getBrainSet()->getVoxelColoring()->setVolumeFunctionalColoringInvalid();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}
      
/**
 * read metric L-to-L, R-to-R.
 */
void 
GuiDisplayControlDialog::readMetricL2LR2R()
{
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   dsm->setApplySelectionToLeftAndRightStructuresFlag(metricApplySelectionToLeftAndRightStructuresFlagCheckBox->isChecked());
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}
      
/**
 * Read the selections from the metric dialog.
 */
void
GuiDisplayControlDialog::readMetricSelectionPage()
{
   if (creatingDialog) {
      return;
   }
   if (pageMetricSelection != NULL) {
      MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
      if (mf->getNumberOfColumns() > 0) {
         for (int i = 0; i < mf->getNumberOfColumns(); i++) {
            const QString name(metricColumnNameLineEdits[i]->text());
            if (name != mf->getColumnName(i)) {
               mf->setColumnName(i, name);
               if (pageOverlayUnderlaySurface != NULL) {
                  metricSelectionComboBox->setItemText(i, name);
               }
            }
         }
      }
   }
   
   GuiFilesModified fm;
   fm.setMetricModified();
   theMainWindow->fileModificationUpdate(fm);
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   theMainWindow->getBrainSet()->getVoxelColoring()->setVolumeFunctionalColoringInvalid();
   GuiBrainModelOpenGL::updateAllGL();   
}

/**
 * Page containing selection of borders by color
 */
void
GuiDisplayControlDialog::createBorderColorPage()
{
   //
   // Vertical Box Layout for all border items
   //
   pageBorderColor = new QWidget;
   borderSubPageColorLayout = new QVBoxLayout(pageBorderColor);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* borderColorAllOnButton = new QPushButton("All On");
   borderColorAllOnButton->setAutoDefault(false);
   QObject::connect(borderColorAllOnButton, SIGNAL(clicked()),
                    this, SLOT(borderColorAllOn()));
   QPushButton* borderColorAllOffButton = new QPushButton("All Off");
   borderColorAllOffButton->setAutoDefault(false);
   QObject::connect(borderColorAllOffButton, SIGNAL(clicked()),
                    this, SLOT(borderColorAllOff()));
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(borderColorAllOnButton);
   allOnOffLayout->addWidget(borderColorAllOffButton);
   allOnOffLayout->addStretch();
   borderSubPageColorLayout->addLayout(allOnOffLayout);
   
   pageBorderColorWidgetGroup = new WuQWidgetGroup(this);
   pageBorderColorWidgetGroup->addWidget(borderColorAllOnButton);
   pageBorderColorWidgetGroup->addWidget(borderColorAllOffButton);
   
   createAndUpdateBorderColorCheckBoxes();

   pageWidgetStack->addWidget(pageBorderColor);
}

/**
 * Create and update border color checkboxes.  Because the number of border colors may change,
 * this method may update and change the label on existing checkboxes, add new 
 * checkboxes, and hide existing checkboxes if the number of checkboxes is greater
 * than the number of border colors.
 */
void 
GuiDisplayControlDialog::createAndUpdateBorderColorCheckBoxes()
{
   BorderColorFile* borderColors = theMainWindow->getBrainSet()->getBorderColorFile();
   numValidBorderColors = borderColors->getNumberOfColors();
   
   const int numExistingCheckBoxes = static_cast<int>(borderColorCheckBoxes.size());
   
   if (borderColorGridLayout == NULL) {
      QWidget* colorsWidget = new QWidget;
      borderColorGridLayout = new QGridLayout(colorsWidget);
      const int rowStretchNumber    = 15000;
      borderColorGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, Qt::AlignLeft);
      borderColorGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      borderSubPageColorLayout->addWidget(colorsWidget);
   }
   
   if (borderColorButtonGroup == NULL) {
      borderColorButtonGroup = new QButtonGroup(this);
      borderColorButtonGroup->setExclusive(false);
      QObject::connect(borderColorButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readBorderColorPage()));
   }
   
   //
   // Update exising checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidBorderColors) {
         borderColorCheckBoxes[i]->setText(borderColors->getColorNameByIndex(i));
         borderColorCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidBorderColors; j++) {
      QCheckBox* cb = new QCheckBox(borderColors->getColorNameByIndex(j));
      borderColorCheckBoxes.push_back(cb);
      borderColorButtonGroup->addButton(cb, j);      
      borderColorGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
      pageBorderColorWidgetGroup->addWidget(cb);
   }
   
   //
   // Hide existing checkboxes that are not needed.
   //
   for (int k = numValidBorderColors; k < numExistingCheckBoxes; k++) {
      borderColorCheckBoxes[k]->hide();
   }
}

/**
 * This slot is called when the border color all on button is pressed
 */
void
GuiDisplayControlDialog::borderColorAllOn()
{
   BorderColorFile* borderColors = theMainWindow->getBrainSet()->getBorderColorFile();
   borderColors->setAllSelectedStatus(true);
   updateBorderItems();
   readBorderColorPage();
}

/**
 * This slot is called when the border color all on button is pressed
 */
void
GuiDisplayControlDialog::borderColorAllOff()
{
   BorderColorFile* borderColors = theMainWindow->getBrainSet()->getBorderColorFile();
   borderColors->setAllSelectedStatus(false);
   updateBorderItems();
   readBorderColorPage();
}
 
/**
 * Page containing selection of borders by name
 */
void
GuiDisplayControlDialog::createBorderNamePage()
{
   //
   // Vertical Box Layout for all border items
   //
   pageBorderName = new QWidget;
   borderSubPageNameLayout = new QVBoxLayout(pageBorderName);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* borderNameAllOnButton = new QPushButton("All On");
   borderNameAllOnButton->setAutoDefault(false);
   QObject::connect(borderNameAllOnButton, SIGNAL(clicked()),
                    this, SLOT(borderNameAllOn()));
   QPushButton* borderNameAllOffButton = new QPushButton("All Off");
   borderNameAllOffButton->setAutoDefault(false);
   QObject::connect(borderNameAllOffButton, SIGNAL(clicked()),
                    this, SLOT(borderNameAllOff()));
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(borderNameAllOnButton);
   allOnOffLayout->addWidget(borderNameAllOffButton);
   allOnOffLayout->addStretch();
   borderSubPageNameLayout->addLayout(allOnOffLayout);
   
   pageBorderNameWidgetGroup = new WuQWidgetGroup(this);
   pageBorderNameWidgetGroup->addWidget(borderNameAllOnButton);
   pageBorderNameWidgetGroup->addWidget(borderNameAllOffButton);
   
   createAndUpdateBorderNameCheckBoxes();
   pageWidgetStack->addWidget(pageBorderName);
}

/**
 * Create and update border name checkboxes.  Because the number of border names may change,
 * this method may update and change the label on existing checkboxes, add new 
 * checkboxes, and hide existing checkboxes if the number of checkboxes is greater
 * than the number of border names.
 */
void 
GuiDisplayControlDialog::createAndUpdateBorderNameCheckBoxes()
{
   borderNames.clear();
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->getAllBorderNames(borderNames);
   
   numValidBorderNames = static_cast<int>(borderNames.size());
   const int numExistingCheckBoxes = static_cast<int>(borderNameCheckBoxes.size());
   
   if (borderNameGridLayout == NULL) {
      QWidget* namesWidget = new QWidget;
      namesWidget->setMaximumWidth(450);
      borderNameGridLayout = new QGridLayout(namesWidget);
      const int rowStretchNumber    = 15000;
      borderNameGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, Qt::AlignLeft);
      borderNameGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      borderSubPageNameLayout->addWidget(namesWidget);
   }
   
   if (borderNameButtonGroup == NULL) {
      borderNameButtonGroup = new QButtonGroup(this);
      borderNameButtonGroup->setExclusive(false);
      QObject::connect(borderNameButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readBorderNamePage()));
   }
   
   //
   // Update exising checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidBorderNames) {
         borderNameCheckBoxes[i]->setText(borderNames[i]);
         borderNameCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidBorderNames; j++) {
      QCheckBox* cb = new QCheckBox(borderNames[j]);
      borderNameCheckBoxes.push_back(cb);
      borderNameButtonGroup->addButton(cb, j);
      borderNameGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
      pageBorderNameWidgetGroup->addWidget(cb);
   }
   
   //
   // Hide existing checkboxes that are not needed.
   //
   for (int k = numValidBorderNames; k < numExistingCheckBoxes; k++) {
      borderNameCheckBoxes[k]->hide();
   }
}

/**
 * This slot is called when the border name all on button is pressed
 */
void
GuiDisplayControlDialog::borderNameAllOn()
{
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   const int numBorders = bmbs->getNumberOfBorders();
   for (int j = 0; j < numBorders; j++) {
      BrainModelBorder* border = bmbs->getBorder(j);
      border->setNameDisplayFlag(true);
   }
   BorderFile* volumeBorders = theMainWindow->getBrainSet()->getVolumeBorderFile();
   for (int j = 0; j < volumeBorders->getNumberOfBorders(); j++) {
      Border* b = volumeBorders->getBorder(j);
      b->setNameDisplayFlag(true);
   }
   updateBorderItems();
   readBorderNamePage();
}

/**
 * This slot is called when the border name all on button is pressed
 */
void
GuiDisplayControlDialog::borderNameAllOff()
{
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   const int numBorders = bmbs->getNumberOfBorders();
   for (int j = 0; j < numBorders; j++) {
      BrainModelBorder* border = bmbs->getBorder(j);
      border->setNameDisplayFlag(false);
   }
   BorderFile* volumeBorders = theMainWindow->getBrainSet()->getVolumeBorderFile();
   for (int j = 0; j < volumeBorders->getNumberOfBorders(); j++) {
      Border* b = volumeBorders->getBorder(j);
      b->setNameDisplayFlag(false);
   }
   updateBorderItems();
   readBorderNamePage();
}

/**
 * Border page containing main selections
 */
void
GuiDisplayControlDialog::createBorderMainPage()
{
   showBordersCheckBox = new QCheckBox("Show Borders");
   QObject::connect(showBordersCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(showBordersToggleSlot(bool)));

   showRaisedBordersCheckBox = new QCheckBox("Show Raised Borders (flat surfaces only)");
   QObject::connect(showRaisedBordersCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readBorderMainPage()));
    
   showUncertaintyBordersCheckBox = new QCheckBox("Show Uncertainty Vectors (flat surfaces only)");
   QObject::connect(showUncertaintyBordersCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readBorderMainPage()));
    
   showFirstLinkRedBordersCheckBox = new QCheckBox("Show First Link Red");
   QObject::connect(showFirstLinkRedBordersCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readBorderMainPage()));
    
   bordersDrawTypeComboBox = new QComboBox;
   bordersDrawTypeComboBox->insertItem(DisplaySettingsBorders::BORDER_DRAW_AS_SYMBOLS,
                                       "Draw Borders as Symbols");
   bordersDrawTypeComboBox->insertItem(DisplaySettingsBorders::BORDER_DRAW_AS_LINES,
                                       "Draw Borders as Lines");
   bordersDrawTypeComboBox->insertItem(DisplaySettingsBorders::BORDER_DRAW_AS_SYMBOLS_AND_LINES,
                                       "Draw Borders as Symbols And Lines");
   bordersDrawTypeComboBox->insertItem(DisplaySettingsBorders::BORDER_DRAW_AS_UNSTRETCHED_LINES,
                                       "Draw Borders as Unstretched Lines");
   bordersDrawTypeComboBox->insertItem(DisplaySettingsBorders::BORDER_DRAW_AS_VARIABILITY,
                                       "Draw Borders Showing Variability");
   bordersDrawTypeComboBox->insertItem(DisplaySettingsBorders::BORDER_DRAW_AS_VARIABILITY_AND_LINES,
                                       "Draw Borders Showing Variability and Lines");
   bordersDrawTypeComboBox->setFixedSize(bordersDrawTypeComboBox->sizeHint());
   
   QObject::connect(bordersDrawTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(readBorderMainPage()));
                    
   QLabel* symbolLabel = new QLabel("Symbol Type ");
   borderSymbolComboBox = new QComboBox;
   std::vector<QString> borderOverrideLabels;
   ColorFile::ColorStorage::getAllSymbolTypesAsStrings(borderOverrideLabels);
   for (unsigned int i = 0; i < borderOverrideLabels.size(); i++) {
      borderSymbolComboBox->addItem(borderOverrideLabels[i]);
   }
   QObject::connect(borderSymbolComboBox, SIGNAL(activated(int)),
                    this, SLOT(readBorderMainPage()));
   QHBoxLayout* symbolLayout = new QHBoxLayout;
   symbolLayout->addWidget(symbolLabel);
   symbolLayout->addWidget(borderSymbolComboBox);
   symbolLayout->addStretch();

   QLabel* opacityLabel = new QLabel("Opacity");
   borderOpacityDoubleSpinBox = new QDoubleSpinBox;
   borderOpacityDoubleSpinBox->setMinimum(0.0);
   borderOpacityDoubleSpinBox->setMaximum(1.0);
   borderOpacityDoubleSpinBox->setSingleStep(0.01);
   borderOpacityDoubleSpinBox->setDecimals(3);
   QObject::connect(borderOpacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readBorderMainPage()));
   QHBoxLayout* opacityLayout = new QHBoxLayout;
   opacityLayout->addWidget(opacityLabel);
   opacityLayout->addWidget(borderOpacityDoubleSpinBox);
   opacityLayout->addStretch();
                    
   QLabel* spinLabel = new QLabel("Border Line Width / Point Size");   
   borderSizeSpinBox = new QDoubleSpinBox;
   borderSizeSpinBox->setMinimum(minPointSize);
   borderSizeSpinBox->setMaximum(maxPointSize);
   borderSizeSpinBox->setSingleStep(1.0);
   borderSizeSpinBox->setDecimals(1);
   QObject::connect(borderSizeSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readBorderMainPage()));
   QHBoxLayout* sizeLayout = new QHBoxLayout;
   sizeLayout->addWidget(spinLabel);
   sizeLayout->addWidget(borderSizeSpinBox);
   sizeLayout->addStretch();
   
   QLabel* stretchLabel = new QLabel("Unstretched Lines Factor");
   unstretchedBordersDoubleSpinBox = new QDoubleSpinBox;
   unstretchedBordersDoubleSpinBox->setMinimum(0.001);
   unstretchedBordersDoubleSpinBox->setMaximum(100000.0);
   unstretchedBordersDoubleSpinBox->setSingleStep(1.0);
   unstretchedBordersDoubleSpinBox->setDecimals(2);
   QObject::connect(unstretchedBordersDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readBorderMainPage()));
   QHBoxLayout* stretchLayout = new QHBoxLayout;
   stretchLayout->addWidget(stretchLabel);
   stretchLayout->addWidget(unstretchedBordersDoubleSpinBox);
   stretchLayout->addStretch();
   
   //
   // Display color key button
   //
   QPushButton* colorKeyPushButton = new QPushButton("Display Color Key...");
   colorKeyPushButton->setFixedSize(colorKeyPushButton->sizeHint());
   colorKeyPushButton->setAutoDefault(false);
   QObject::connect(colorKeyPushButton, SIGNAL(clicked()),
                    theMainWindow, SLOT(displayBorderColorKey()));
                    
   //
   // Override border with area colors check box
   //
   overrideBorderWithAreaColorsCheckBox = new QCheckBox("Override Border Colors with Area Colors");
   QObject::connect(overrideBorderWithAreaColorsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readBorderMainPage()));
                    
   //
   // Vertical Box Layout for all border items
   //
   pageBorderMain = new QWidget; 
   QVBoxLayout* bordersSubPageMainLayout = new QVBoxLayout(pageBorderMain);
   bordersSubPageMainLayout->addWidget(showBordersCheckBox);
   bordersSubPageMainLayout->addWidget(showRaisedBordersCheckBox);
   bordersSubPageMainLayout->addWidget(showUncertaintyBordersCheckBox);
   bordersSubPageMainLayout->addWidget(showFirstLinkRedBordersCheckBox);
   bordersSubPageMainLayout->addWidget(overrideBorderWithAreaColorsCheckBox);
   bordersSubPageMainLayout->addWidget(bordersDrawTypeComboBox);
   bordersSubPageMainLayout->addLayout(symbolLayout);
   bordersSubPageMainLayout->addLayout(sizeLayout);
   bordersSubPageMainLayout->addLayout(opacityLayout);
   bordersSubPageMainLayout->addLayout(stretchLayout);
   bordersSubPageMainLayout->addWidget(colorKeyPushButton);
   bordersSubPageMainLayout->addStretch();

   pageBorderMainWidgetGroup = new WuQWidgetGroup(this);
   pageBorderMainWidgetGroup->addWidget(showBordersCheckBox);
   pageBorderMainWidgetGroup->addWidget(showRaisedBordersCheckBox);
   pageBorderMainWidgetGroup->addWidget(showUncertaintyBordersCheckBox);
   pageBorderMainWidgetGroup->addWidget(showFirstLinkRedBordersCheckBox);
   pageBorderMainWidgetGroup->addWidget(bordersDrawTypeComboBox);
   pageBorderMainWidgetGroup->addWidget(borderSymbolComboBox);
   pageBorderMainWidgetGroup->addWidget(borderOpacityDoubleSpinBox);
   pageBorderMainWidgetGroup->addWidget(borderSizeSpinBox);
   pageBorderMainWidgetGroup->addWidget(unstretchedBordersDoubleSpinBox);
   pageBorderMainWidgetGroup->addWidget(overrideBorderWithAreaColorsCheckBox);
   
   pageWidgetStack->addWidget(pageBorderMain);
}

/**
 * update border main page.
 */
void 
GuiDisplayControlDialog::updateBorderMainPage()
{
   if (pageBorderMain == NULL) {
      return;
   }
   pageBorderMainWidgetGroup->blockSignals(true);
   
   DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
   showBordersCheckBox->setChecked(dsb->getDisplayBorders());
   borderSymbolComboBox->setCurrentIndex(dsb->getSymbolType());
   overrideBorderWithAreaColorsCheckBox->setChecked(dsb->getOverrideBorderColorsWithAreaColors());
   borderOpacityDoubleSpinBox->setValue(dsb->getOpacity());
   
   showRaisedBordersCheckBox->setChecked(dsb->getDisplayFlatBordersRaised());
   showUncertaintyBordersCheckBox->setChecked(dsb->getDisplayUncertaintyVectors());
   showFirstLinkRedBordersCheckBox->setChecked(dsb->getDisplayFirstLinkRed());
   
   showBordersCheckBox->setEnabled(validBorderData);
      
   bordersDrawTypeComboBox->setCurrentIndex(dsb->getDrawMode());
   borderSizeSpinBox->setValue(dsb->getDrawSize());
   unstretchedBordersDoubleSpinBox->setValue(dsb->getDrawAsStretchedLinesStretchFactor());
   
   pageBorderMain->setEnabled(validBorderData);
   
   pageBorderMainWidgetGroup->blockSignals(false);
}

/**
 * update border colors page.
 */
void 
GuiDisplayControlDialog::updateBorderColorPage(const bool filesChanged)
{
   if (pageBorderColor == NULL) {
      return;
   }
   
   if (filesChanged) {
      createAndUpdateBorderColorCheckBoxes();
   }
   pageBorderColorWidgetGroup->blockSignals(true);

   BorderColorFile* borderColors = theMainWindow->getBrainSet()->getBorderColorFile();
   const int numColors = borderColors->getNumberOfColors();   
   if (numColors == numValidBorderColors) {
      for (int i = 0; i < numValidBorderColors; i++) {
         borderColorCheckBoxes[i]->setChecked(borderColors->getSelected(i));
      }
   }
   else {
      std::cerr << "Number of border color checkboxes does not equal number of border colors."
                << std::endl;
   }
   
   pageBorderColor->setEnabled(validBorderData);
   pageBorderColorWidgetGroup->blockSignals(false);
}

/**
 * update border name page.
 */
void 
GuiDisplayControlDialog::updateBorderNamePage(const bool filesChanged)
{
   if (pageBorderName == NULL) {
      return;
   }

   if (filesChanged) {
      createAndUpdateBorderNameCheckBoxes();
   }
   pageBorderNameWidgetGroup->blockSignals(true);

   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   const int numBorders = bmbs->getNumberOfBorders();
   for (int j = 0; j < numBorders; j++) {
      BrainModelBorder* border = bmbs->getBorder(j);
      const QString name = border->getName();
      for (int k = 0; k < numValidBorderNames; k++) {
         if (name == borderNames[k]) {
            borderNameCheckBoxes[k]->setChecked(border->getNameDisplayFlag());
            break;
         }
      }
   }
   
   BorderFile* volumeBorderFile = theMainWindow->getBrainSet()->getVolumeBorderFile();
   const int numVolumeBorders = volumeBorderFile->getNumberOfBorders();
   for (int j = 0; j < numVolumeBorders; j++) {
      Border* b = volumeBorderFile->getBorder(j);
      const QString name = b->getName();
      for (int k = 0; k < numValidBorderNames; k++) {
         if (name == borderNames[k]) {
            borderNameCheckBoxes[k]->setChecked(b->getNameDisplayFlag());
            break;
         }
      }
   }

   pageBorderName->setEnabled(validBorderData);   
   pageBorderNameWidgetGroup->blockSignals(false);

}
      
/**
 * Update border items in dialog
 */
void
GuiDisplayControlDialog::updateBorderItems(const bool filesChanged)
{
   updatePageSelectionComboBox();
   
   DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
   
   if (pageOverlayUnderlaySurface != NULL) {
      layersBorderCheckBox->setChecked(dsb->getDisplayBorders());
      layersBorderCheckBox->setEnabled(validBorderData);
   }
   
   updateBorderMainPage();
   updateBorderColorPage(filesChanged);
   updateBorderNamePage(filesChanged);
   updatePageSelectionComboBox();   
}

/**
 * Called when either the show borders on border page check box selected or the
 * borders check box on the Surface - Overlay/Underlay page is selected.
 */
void
GuiDisplayControlDialog::showBordersToggleSlot(bool b)
{
   if (pageBorderMain != NULL) {
      showBordersCheckBox->blockSignals(true);
      showBordersCheckBox->setChecked(b);
      showBordersCheckBox->blockSignals(false);
   }
   if (pageOverlayUnderlaySurface != NULL) {
      layersBorderCheckBox->blockSignals(true);
      layersBorderCheckBox->setChecked(b);
      layersBorderCheckBox->blockSignals(false);
   }
   //if (pageBorderMain == NULL) {
   DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();   
   dsb->setDisplayBorders(b);
   dsb->determineDisplayedBorders();   
   GuiBrainModelOpenGL::updateAllGL(NULL);   
   //}
   //readBorderMainPage();
   //readBorderColorPage();
   //readBorderNamePage();
}

/**
 * read border main page.
 */
void 
GuiDisplayControlDialog::readBorderMainPage()
{
   if (pageBorderMain == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   
   DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
   
   dsb->setDisplayBorders(showBordersCheckBox->isChecked());
   dsb->setDisplayFlatBordersRaised(showRaisedBordersCheckBox->isChecked());
   dsb->setDisplayUncertaintyVector(showUncertaintyBordersCheckBox->isChecked());
   dsb->setDisplayFirstLinkRed(showFirstLinkRedBordersCheckBox->isChecked());
   dsb->setOverrideBorderColorsWithAreaColors(overrideBorderWithAreaColorsCheckBox->isChecked());
   dsb->setDrawMode(static_cast<DisplaySettingsBorders::BORDER_DRAW_MODE>(
                                 bordersDrawTypeComboBox->currentIndex()));
   dsb->setDrawSize(borderSizeSpinBox->value());
   dsb->setSymbolType(static_cast<ColorFile::ColorStorage::SYMBOL>(borderSymbolComboBox->currentIndex()));
   dsb->setOpacity(borderOpacityDoubleSpinBox->value());
   dsb->setDrawAsStretchedLinesStretchFactor(unstretchedBordersDoubleSpinBox->value());
   
   dsb->determineDisplayedBorders();   
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * read border color page.
 */
void 
GuiDisplayControlDialog::readBorderColorPage()
{
   if (pageBorderColor == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   BorderColorFile* borderColors = theMainWindow->getBrainSet()->getBorderColorFile();
   const int numColors = borderColors->getNumberOfColors();   
   if (numColors == numValidBorderColors) {
      for (int i = 0; i < numValidBorderColors; i++) {
         borderColors->setSelected(i, borderColorCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of border color checkboxes does not equal number of border colors."
                << std::endl;
   }   
   
   DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
   dsb->determineDisplayedBorders();   
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * read border name page.
 */
void 
GuiDisplayControlDialog::readBorderNamePage()
{
   if (pageBorderName == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }

   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   for (int j = 0; j < numValidBorderNames; j++) {
      bmbs->setNameDisplayFlagForBordersWithName(borderNames[j],
                                                 borderNameCheckBoxes[j]->isChecked());
   }   
   
   DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
   dsb->determineDisplayedBorders();   
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * Create the deformation field page.
 */
void 
GuiDisplayControlDialog::createDeformationFieldPage()
{
   //
   // Display mode combo box
   //
   QLabel* columnLabel = new QLabel("Column ");
   deformationFieldComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                GUI_NODE_FILE_TYPE_DEFORMATION_FIELD,
                                                false,
                                                false,
                                                false);
   QObject::connect(deformationFieldComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(readDeformationFieldPage()));

   QLabel* displayModeLabel = new QLabel("Display Mode");
   deformationFieldModeComboBox = new QComboBox;
   deformationFieldModeComboBox->insertItem(DisplaySettingsDeformationField::DISPLAY_MODE_ALL,
                                            "All");
   deformationFieldModeComboBox->insertItem(DisplaySettingsDeformationField::DISPLAY_MODE_NONE,
                                            "None");
   deformationFieldModeComboBox->insertItem(DisplaySettingsDeformationField::DISPLAY_MODE_SPARSE,
                                            "Sparse");
   QObject::connect(deformationFieldModeComboBox, SIGNAL(activated(int)),
                    this, SLOT(readDeformationFieldPage()));
                    
   //
   // Sparse distance spin box
   //
   QLabel* sparseLabel = new QLabel("Sparse Distance");
   deformationFieldSparseDistanceSpinBox = new QSpinBox;
   deformationFieldSparseDistanceSpinBox->setMinimum(0);
   deformationFieldSparseDistanceSpinBox->setMaximum(100000);
   deformationFieldSparseDistanceSpinBox->setSingleStep(1);
   
   //
   // Place items into a grid
   //
   QGroupBox* optionsGroupBox = new QGroupBox("Options");
   QGridLayout* optionsGroupLayout = new QGridLayout(optionsGroupBox);
   optionsGroupLayout->addWidget(columnLabel, 0, 0);
   optionsGroupLayout->addWidget(deformationFieldComboBox, 0, 1, 1, 1, Qt::AlignLeft);
   optionsGroupLayout->addWidget(displayModeLabel, 1, 0);
   optionsGroupLayout->addWidget(deformationFieldModeComboBox, 1, 1, 1, 1, Qt::AlignLeft);
   optionsGroupLayout->addWidget(sparseLabel, 2, 0);
   optionsGroupLayout->addWidget(deformationFieldSparseDistanceSpinBox, 2, 1, 1, 1, Qt::AlignLeft);
   optionsGroupLayout->setColumnStretch(0, 0);
   optionsGroupLayout->setColumnStretch(1, 1);
   optionsGroupLayout->setColumnStretch(2, 100);
   optionsGroupBox->setFixedHeight(optionsGroupBox->sizeHint().height());
   
   //
   // Show on identified nodes
   //
   deformationFieldShowIdNodesCheckBox = new QCheckBox("Show Vectors On Identified Nodes");
   QObject::connect(deformationFieldShowIdNodesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readDeformationFieldPage()));
   
   //
   // Unstretched on flag check box
   //
   deformationFieldShowUnstretchedCheckBox = new QCheckBox("Show Unstretched on Flat Surface ");
   QObject::connect(deformationFieldShowUnstretchedCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readDeformationFieldPage()));
   deformationFieldUnstretchedDoubleSpinBox = new QDoubleSpinBox;
   deformationFieldUnstretchedDoubleSpinBox->setMinimum(0.01);
   deformationFieldUnstretchedDoubleSpinBox->setMaximum(100.0);
   deformationFieldUnstretchedDoubleSpinBox->setSingleStep(0.5);
   deformationFieldUnstretchedDoubleSpinBox->setDecimals(2);
   QObject::connect(deformationFieldUnstretchedDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readDeformationFieldPage()));
   QHBoxLayout* unstrechLayout = new QHBoxLayout;
   unstrechLayout->addWidget(deformationFieldShowUnstretchedCheckBox);
   unstrechLayout->addWidget(deformationFieldUnstretchedDoubleSpinBox);
   unstrechLayout->addStretch();
   
   //
   // Page and layout
   //
   pageDeformationField = new QWidget;
   pageWidgetStack->addWidget(pageDeformationField); //, PAGE_NAME_DEFORMATION_FIELD);
   QVBoxLayout* deformationFieldLayout = new QVBoxLayout(pageDeformationField);
   deformationFieldLayout->addWidget(optionsGroupBox);
   deformationFieldLayout->addWidget(deformationFieldShowIdNodesCheckBox);
   deformationFieldLayout->addLayout(unstrechLayout);
   deformationFieldLayout->addStretch();
}

/**
 * Read the contents of the deformation field page.
 */
void
GuiDisplayControlDialog::readDeformationFieldPage()
{
   if (creatingDialog) {
      return;
   }
   
   DisplaySettingsDeformationField* dsdf = theMainWindow->getBrainSet()->getDisplaySettingsDeformationField();
   dsdf->setSelectedDisplayColumn(deformationFieldComboBox->currentIndex());
   dsdf->setShowUnstretchedOnFlat(deformationFieldUnstretchedDoubleSpinBox->value(),
                                  deformationFieldShowUnstretchedCheckBox->isChecked());
   dsdf->setDisplayMode(static_cast<DisplaySettingsDeformationField::DISPLAY_MODE>(
                                    deformationFieldModeComboBox->currentIndex()));
   dsdf->setDisplayIdentifiedNodes(deformationFieldShowIdNodesCheckBox->isChecked());
   dsdf->setSparseDisplayDistance(deformationFieldSparseDistanceSpinBox->value());
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Update the deformation field page.
 */
void
GuiDisplayControlDialog::updateDeformationFieldPage()
{
   updatePageSelectionComboBox();

   if (pageDeformationField == NULL) {
      return;
   }
   
   DisplaySettingsDeformationField* dsdf = theMainWindow->getBrainSet()->getDisplaySettingsDeformationField();
   
   deformationFieldComboBox->updateComboBox(theMainWindow->getBrainSet()->getDeformationFieldFile());
   deformationFieldComboBox->setCurrentIndex(dsdf->getSelectedDisplayColumn());
   deformationFieldModeComboBox->setCurrentIndex(dsdf->getDisplayMode());
   deformationFieldShowIdNodesCheckBox->setChecked(dsdf->getDisplayIdentifiedNodes());
   deformationFieldSparseDistanceSpinBox->setValue(dsdf->getSparseDisplayDistance());
   bool showUnstretched;
   float stretchFactor;
   dsdf->getShowUnstretchedOnFlat(stretchFactor, showUnstretched);
   deformationFieldShowUnstretchedCheckBox->setChecked(showUnstretched);
   deformationFieldUnstretchedDoubleSpinBox->setValue(stretchFactor);
   
   pageDeformationField->setEnabled(validDeformationFieldData);
}

 
/**
 * Foci page containing main items.
 */
void
GuiDisplayControlDialog::createFociMainPage()
{
   showFociCheckBox = new QCheckBox("Show Foci");
   QObject::connect(showFociCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(showFociToggleSlot(bool)));
   
   showVolumeFociCheckBox = new QCheckBox("Show Volume Foci");
   QObject::connect(showVolumeFociCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readFociMainPage()));
                    
   showRaisedFociCheckBox = new QCheckBox("Show Raised Foci on Flat Surfaces");
   QObject::connect(showRaisedFociCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readFociMainPage()));
    
   showPastedOnto3DFociCheckBox = new QCheckBox("Show Foci Pasted Onto 3D Surfaces");
   QObject::connect(showPastedOnto3DFociCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readFociMainPage()));

   showCorrectHemisphereFociCheckBox = new QCheckBox("Show Foci on Correct Hemisphere Only");
   QObject::connect(showCorrectHemisphereFociCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readFociMainPage()));
   
   QLabel* symbolLabel = new QLabel("Symbol Override ");
   fociSymbolOverrideComboBox = new QComboBox;
   std::vector<QString> cellOverrideLabels;
   ColorFile::ColorStorage::getAllSymbolTypesAsStrings(cellOverrideLabels);
   for (unsigned int i = 0; i < cellOverrideLabels.size(); i++) {
      fociSymbolOverrideComboBox->addItem(cellOverrideLabels[i]);
   }
   QObject::connect(fociSymbolOverrideComboBox, SIGNAL(activated(int)),
                    this, SLOT(readFociMainPage()));
   QHBoxLayout* symbolLayout = new QHBoxLayout;
   symbolLayout->addWidget(symbolLabel);
   symbolLayout->addWidget(fociSymbolOverrideComboBox);
   symbolLayout->addStretch();
   
   QLabel* spinLabel = new QLabel("Foci Size");
   fociSizeSpinBox = new QDoubleSpinBox;
   fociSizeSpinBox->setMinimum(0.001);
   fociSizeSpinBox->setMaximum(maxPointSize);
   fociSizeSpinBox->setSingleStep(0.5);
   fociSizeSpinBox->setDecimals(3);
   QObject::connect(fociSizeSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readFociMainPage()));
   QHBoxLayout* sizeLayout = new QHBoxLayout;
   sizeLayout->addWidget(spinLabel);
   sizeLayout->addWidget(fociSizeSpinBox);
   sizeLayout->addStretch();

   QLabel* opacityLabel = new QLabel("Opacity");
   fociOpacityDoubleSpinBox = new QDoubleSpinBox;
   fociOpacityDoubleSpinBox->setMinimum(0.0);
   fociOpacityDoubleSpinBox->setMaximum(1.0);
   fociOpacityDoubleSpinBox->setSingleStep(0.01);
   fociOpacityDoubleSpinBox->setDecimals(3);
   QObject::connect(fociOpacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readFociMainPage()));
   QHBoxLayout* opacityLayout = new QHBoxLayout;
   opacityLayout->addWidget(opacityLabel);
   opacityLayout->addWidget(fociOpacityDoubleSpinBox);
   opacityLayout->addStretch();
                    
   QLabel* distanceLabel = new QLabel("Distance");   
   fociDistSpinBox = new QDoubleSpinBox;
   fociDistSpinBox->setMinimum(0.0);
   fociDistSpinBox->setMaximum(1000.0);
   fociDistSpinBox->setSingleStep(10.0);
   fociDistSpinBox->setDecimals(1);
   fociDistSpinBox->setValue(1000.0);
   QObject::connect(fociDistSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readFociMainPage()));
   fociDistSpinBox->setToolTip("Foci whose distances to the surface are\n"
                                  "less than this value will be displayed.\n"
                                  "The foci must have been projected.");
   QHBoxLayout* distanceLayout = new QHBoxLayout;
   distanceLayout->addWidget(distanceLabel);
   distanceLayout->addWidget(fociDistSpinBox);
   distanceLayout->addStretch();

   //
   // Display color key button
   //
   QPushButton* colorKeyPushButton = new QPushButton("Display Color Key...");
   colorKeyPushButton->setFixedSize(colorKeyPushButton->sizeHint());
   colorKeyPushButton->setAutoDefault(false);
   QObject::connect(colorKeyPushButton, SIGNAL(clicked()),
                    theMainWindow, SLOT(displayFociColorKey()));
          
   //
   // Color mode
   //
   QLabel* colorModeLabel = new QLabel("Coloring Mode");
   fociColorModeComboBox = new QComboBox;
   fociColorModeComboBox->insertItem(CellBase::CELL_COLOR_MODE_NAME, 
                                     "Focus Name",
                                     static_cast<int>(CellBase::CELL_COLOR_MODE_NAME));
   fociColorModeComboBox->insertItem(CellBase::CELL_COLOR_MODE_CLASS, 
                                     "Focus Class",
                                     static_cast<int>(CellBase::CELL_COLOR_MODE_CLASS));
   QObject::connect(fociColorModeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotFociColorModeComboBox(int)));
   QHBoxLayout* colorModeLayout = new QHBoxLayout;
   colorModeLayout->addWidget(colorModeLabel);
   colorModeLayout->addWidget(fociColorModeComboBox);
   colorModeLayout->addStretch();
   
   //
   // Widget to hold all foci main itmes
   //
   pageFociMain = new QWidget; 
   QVBoxLayout* fociSubPageMainLayout = new QVBoxLayout(pageFociMain);
   fociSubPageMainLayout->addWidget(showFociCheckBox);
   fociSubPageMainLayout->addWidget(showVolumeFociCheckBox);
   fociSubPageMainLayout->addWidget(showRaisedFociCheckBox);
   fociSubPageMainLayout->addWidget(showPastedOnto3DFociCheckBox);
   fociSubPageMainLayout->addWidget(showCorrectHemisphereFociCheckBox);
   fociSubPageMainLayout->addLayout(symbolLayout);
   fociSubPageMainLayout->addLayout(colorModeLayout);
   fociSubPageMainLayout->addLayout(sizeLayout);
   fociSubPageMainLayout->addLayout(opacityLayout);
   fociSubPageMainLayout->addLayout(distanceLayout);
   fociSubPageMainLayout->addWidget(colorKeyPushButton);
   fociSubPageMainLayout->addStretch();

   pageWidgetStack->addWidget(pageFociMain);
   
   //
   // Control signals
   //
   pageFociMainWidgetGroup = new WuQWidgetGroup(this);
   pageFociMainWidgetGroup->addWidget(showFociCheckBox);
   pageFociMainWidgetGroup->addWidget(showVolumeFociCheckBox);
   pageFociMainWidgetGroup->addWidget(showRaisedFociCheckBox);
   pageFociMainWidgetGroup->addWidget(showPastedOnto3DFociCheckBox);
   pageFociMainWidgetGroup->addWidget(showCorrectHemisphereFociCheckBox);
   pageFociMainWidgetGroup->addWidget(fociSymbolOverrideComboBox);
   pageFociMainWidgetGroup->addWidget(fociSizeSpinBox);
   pageFociMainWidgetGroup->addWidget(fociOpacityDoubleSpinBox);
   pageFociMainWidgetGroup->addWidget(fociDistSpinBox);
   pageFociMainWidgetGroup->addWidget(fociColorModeComboBox);
}

/**
 * called when foci color mode changed.
 */
void 
GuiDisplayControlDialog::slotFociColorModeComboBox(int /*i*/)
{
   DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   dsf->setColorMode(static_cast<CellBase::CELL_COLOR_MODE>(fociColorModeComboBox->currentIndex()));
   theMainWindow->getBrainSet()->assignFociColors();
   readFociColorPage();
}
      
/**
 * create the foci name page.
 */
void 
GuiDisplayControlDialog::createFociNamePage()
{
   pageFociName = new QWidget;
   fociSubPageNamesLayout = new QVBoxLayout(pageFociName);

   pageFociNameWidgetGroup = new WuQWidgetGroup(this);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* fociNamesAllOnButton = new QPushButton("All On");
   fociNamesAllOnButton->setAutoDefault(false);
   QObject::connect(fociNamesAllOnButton, SIGNAL(clicked()),
                    this, SLOT(fociNamesAllOn()));
   QPushButton* fociNamesAllOffButton = new QPushButton("All Off");
   fociNamesAllOffButton->setAutoDefault(false);
   QObject::connect(fociNamesAllOffButton, SIGNAL(clicked()),
                    this, SLOT(fociNamesAllOff()));
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(fociNamesAllOnButton);
   allOnOffLayout->addWidget(fociNamesAllOffButton);
   allOnOffLayout->addStretch();
   
   //
   // Names check box layout
   //
   fociNamesGridLayout = new QGridLayout;
   const int rowStretchNumber    = 15000;
   fociNamesGridLayout->addWidget(new QLabel(""),
                                        rowStretchNumber, 0, Qt::AlignLeft);
   fociNamesGridLayout->setRowStretch(rowStretchNumber, 1000);
   
   fociSubPageNamesLayout->addLayout(allOnOffLayout);
   fociSubPageNamesLayout->addLayout(fociNamesGridLayout);

   //
   // Button group for foci names
   //
   fociNamesButtonGroup = new QButtonGroup(this);
   fociNamesButtonGroup->setExclusive(false);
   QObject::connect(fociNamesButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(readFociNamePage()));
                    
   createAndUpdateFociNamesCheckBoxes();

   pageWidgetStack->addWidget(pageFociName);
}

/**
 * create and update foci names toggles section.
 */
void 
GuiDisplayControlDialog::createAndUpdateFociNamesCheckBoxes()
{
   const CellProjectionFile* cpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   const int numExistingCheckBoxes = fociNamesCheckBoxes.size();
   const int numValidNames = cpf->getNumberOfCellUniqueNames();
   
   //
   // Update existing check boxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidNames) {
         fociNamesCheckBoxes[i]->setText(cpf->getCellUniqueNameByIndex(i));
         fociNamesCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidNames; j++) {
      QCheckBox* cb = new QCheckBox(cpf->getCellUniqueNameByIndex(j));
      fociNamesCheckBoxes.push_back(cb);
      fociNamesGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      fociNamesButtonGroup->addButton(cb);
      cb->show();
      pageFociNameWidgetGroup->addWidget(cb);
   }
   
   //
   // Hide existing checkboxes that are not needed
   //
   for (int k = numValidNames; k < numExistingCheckBoxes; k++) {
      fociNamesCheckBoxes[k]->hide();
   }
}
      
/**
 * called when foci names All On button is pressed.
 */
void 
GuiDisplayControlDialog::fociNamesAllOn()
{
   fociNamesButtonGroup->blockSignals(true);
   const int numNames = fociNamesCheckBoxes.size();
   for (int n = 0; n < numNames; n++) {
      fociNamesCheckBoxes[n]->setChecked(true);
   }
   fociNamesButtonGroup->blockSignals(false);
   readFociNamePage();
}

/**
 * called when foci names All Off button is pressed.
 */
void 
GuiDisplayControlDialog::fociNamesAllOff()
{
   fociNamesButtonGroup->blockSignals(true);
   const int numNames = fociNamesCheckBoxes.size();
   for (int n = 0; n < numNames; n++) {
      fociNamesCheckBoxes[n]->setChecked(false);
   }
   fociNamesButtonGroup->blockSignals(false);
   readFociNamePage();
}
      
/**
 * create the foci keywords page.
 */
void 
GuiDisplayControlDialog::createFociKeywordPage()
{
   pageFociKeyword = new QWidget;
   fociSubPageKeywordsLayout = new QVBoxLayout(pageFociKeyword);
   
   pageFociKeywordsWidgetGroup = new WuQWidgetGroup(this);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* fociKeywordsAllOnButton = new QPushButton("All On");
   fociKeywordsAllOnButton->setAutoDefault(false);
   QObject::connect(fociKeywordsAllOnButton, SIGNAL(clicked()),
                    this, SLOT(fociKeywordsAllOn()));
   QPushButton* fociKeywordsAllOffButton = new QPushButton("All Off");
   fociKeywordsAllOffButton->setAutoDefault(false);
   QObject::connect(fociKeywordsAllOffButton, SIGNAL(clicked()),
                    this, SLOT(fociKeywordsAllOff()));
   QPushButton* fociKeywordsUpdateButton = new QPushButton("Update");
   fociKeywordsUpdateButton->setAutoDefault(false);
   QObject::connect(fociKeywordsUpdateButton, SIGNAL(clicked()),
                    this, SLOT(fociKeywordsAndTablesUpdate()));
   fociKeywordsUpdateButton->setToolTip("Update the list of keywords.  This may sometimes\n"
                                        "be necessary after editing keywords in the study\n"
                                        "metadata editor dialog.");
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(fociKeywordsAllOnButton);
   allOnOffLayout->addWidget(fociKeywordsAllOffButton);
   allOnOffLayout->addWidget(fociKeywordsUpdateButton);
   allOnOffLayout->addStretch();
   
   fociWithoutLinkToStudyWithKeywordsCheckBox = new QCheckBox("Show Foci Without Link to Study With Keywords");
   fociWithoutLinkToStudyWithKeywordsCheckBox->setToolTip("If this item is checked, foci wthout\n"
                                                          "a link to study metadata with keyword\n"
                                                          "will be displayed.");
   QObject::connect(fociWithoutLinkToStudyWithKeywordsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readFociKeywordPage()));
   pageFociKeywordsWidgetGroup->addWidget(fociWithoutLinkToStudyWithKeywordsCheckBox);
                    
   //
   // Keywords check box layout
   //
   fociKeywordGridLayout = new QGridLayout;
   const int rowStretchNumber    = 15000;
   fociKeywordGridLayout->addWidget(new QLabel(""),
                                        rowStretchNumber, 0, Qt::AlignLeft);
   fociKeywordGridLayout->setRowStretch(rowStretchNumber, 1000);
   
   fociSubPageKeywordsLayout->addLayout(allOnOffLayout);
   fociSubPageKeywordsLayout->addWidget(fociWithoutLinkToStudyWithKeywordsCheckBox);
   fociSubPageKeywordsLayout->addLayout(fociKeywordGridLayout);

   //
   // Button group for foci keywords
   //
   fociKeywordButtonGroup = new QButtonGroup(this);
   fociKeywordButtonGroup->setExclusive(false);
   QObject::connect(fociKeywordButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(readFociKeywordPage()));
                    
   createAndUpdateFociKeywordCheckBoxes();

   pageWidgetStack->addWidget(pageFociKeyword);
}

/**
 * called when foci keywords Update button is pressed.
 */
void 
GuiDisplayControlDialog::fociKeywordsAndTablesUpdate()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   bs->getDisplaySettingsStudyMetaData()->update();
   bs->getDisplaySettingsFoci()->update();
   bs->assignFociColors();
   GuiFilesModified fm;
   fm.setFociModified();
   fm.setStudyMetaDataModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}
      
/**
 * called when foci keywords All On button is pressed.
 */
void 
GuiDisplayControlDialog::fociKeywordsAllOn()
{
   fociKeywordButtonGroup->blockSignals(true);
   const int numKeywords = fociKeywordCheckBoxes.size();
   for (int k = 0; k < numKeywords; k++) {
      fociKeywordCheckBoxes[k]->setChecked(true);
   }   
   fociKeywordButtonGroup->blockSignals(false);
   readFociKeywordPage();
}

/**
 * called when foci keywords All Off button is pressed.
 */
void 
GuiDisplayControlDialog::fociKeywordsAllOff()
{
   fociKeywordButtonGroup->blockSignals(true);
   const int numKeywords = fociKeywordCheckBoxes.size();
   for (int k = 0; k < numKeywords; k++) {
      fociKeywordCheckBoxes[k]->setChecked(false);
   }   
   fociKeywordButtonGroup->blockSignals(false);
   readFociKeywordPage();
}

/**
 * create and update foci keywords toggles section.
 */
void 
GuiDisplayControlDialog::createAndUpdateFociKeywordCheckBoxes()
{
   const int numExistingCheckBoxes = fociKeywordCheckBoxes.size();
   const DisplaySettingsStudyMetaData* dssmd = theMainWindow->getBrainSet()->getDisplaySettingsStudyMetaData();
   const int numValidKeywords = dssmd->getNumberOfKeywords();
   
   //
   // Update existing check boxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidKeywords) {
         fociKeywordCheckBoxes[i]->setText(dssmd->getKeywordNameByIndex(i));
         fociKeywordCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidKeywords; j++) {
      QCheckBox* cb = new QCheckBox(dssmd->getKeywordNameByIndex(j));
      fociKeywordCheckBoxes.push_back(cb);
      fociKeywordGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      fociKeywordButtonGroup->addButton(cb);
      cb->show();
      pageFociKeywordsWidgetGroup->addWidget(cb);
   }
   
   //
   // Hide existing checkboxes that are not needed.
   //
   for (int k = numValidKeywords; k < numExistingCheckBoxes; k++) {
      fociKeywordCheckBoxes[k]->hide();
   }
}
      
/**
 * create the foci tables page.
 */
void 
GuiDisplayControlDialog::createFociTablePage()
{
   pageFociTable = new QWidget;
   fociSubPageTablesLayout = new QVBoxLayout(pageFociTable);
   
   pageFociTableWidgetGroup = new WuQWidgetGroup(this);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* fociTablesAllOnButton = new QPushButton("All On");
   fociTablesAllOnButton->setAutoDefault(false);
   QObject::connect(fociTablesAllOnButton, SIGNAL(clicked()),
                    this, SLOT(fociTablesAllOn()));
   QPushButton* fociTablesAllOffButton = new QPushButton("All Off");
   fociTablesAllOffButton->setAutoDefault(false);
   QObject::connect(fociTablesAllOffButton, SIGNAL(clicked()),
                    this, SLOT(fociTablesAllOff()));
   QPushButton* fociTablesUpdateButton = new QPushButton("Update");
   fociTablesUpdateButton->setAutoDefault(false);
   QObject::connect(fociTablesUpdateButton, SIGNAL(clicked()),
                    this, SLOT(fociKeywordsAndTablesUpdate()));
   fociTablesUpdateButton->setToolTip("Update the list of tables.  This may sometimes\n"
                                        "be necessary after editing tables in the study\n"
                                        "metadata editor dialog.");
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(fociTablesAllOnButton);
   allOnOffLayout->addWidget(fociTablesAllOffButton);
   allOnOffLayout->addWidget(fociTablesUpdateButton);
   allOnOffLayout->addStretch();

   fociWithoutLinkToStudyWithTableSubHeaderCheckBox = new QCheckBox("Show Foci Without Link to Table Subheader");
   fociWithoutLinkToStudyWithTableSubHeaderCheckBox->setToolTip("If this item is checked, foci wthout\n"
                                                          "a link to study metadata table subheader\n"
                                                          "will be displayed.");
   QObject::connect(fociWithoutLinkToStudyWithTableSubHeaderCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readFociTablePage()));
   pageFociTableWidgetGroup->addWidget(fociWithoutLinkToStudyWithTableSubHeaderCheckBox);
                    
   //
   // Tables check box layout
   //
   fociTablesGridLayout = new QGridLayout;
   const int rowStretchNumber    = 15000;
   fociTablesGridLayout->addWidget(new QLabel(""),
                                        rowStretchNumber, 0, Qt::AlignLeft);
   fociTablesGridLayout->setRowStretch(rowStretchNumber, 1000);
   
   fociSubPageTablesLayout->addLayout(allOnOffLayout);
   fociSubPageTablesLayout->addWidget(fociWithoutLinkToStudyWithTableSubHeaderCheckBox);
   fociSubPageTablesLayout->addLayout(fociTablesGridLayout);

   //
   // Button group for foci tables
   //
   fociTablesButtonGroup = new QButtonGroup(this);
   fociTablesButtonGroup->setExclusive(false);
   QObject::connect(fociTablesButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(readFociTablePage()));
                    
   createAndUpdateFociTableCheckBoxes();

   pageWidgetStack->addWidget(pageFociTable);
}
      
/**
 * called when foci tables All On button is pressed.
 */
void 
GuiDisplayControlDialog::fociTablesAllOn()
{
   fociTablesButtonGroup->blockSignals(true);
   const int numTables = fociTablesCheckBoxes.size();
   for (int k = 0; k < numTables; k++) {
      fociTablesCheckBoxes[k]->setChecked(true);
   }   
   fociTablesButtonGroup->blockSignals(false);
   readFociTablePage();
}

/**
 * called when foci tables All Off button is pressed.
 */
void 
GuiDisplayControlDialog::fociTablesAllOff()
{
   fociTablesButtonGroup->blockSignals(true);
   const int numTables = fociTablesCheckBoxes.size();
   for (int k = 0; k < numTables; k++) {
      fociTablesCheckBoxes[k]->setChecked(false);
   }   
   fociTablesButtonGroup->blockSignals(false);
   readFociTablePage();
}

/**
 * create and update foci table toggles section.
 */
void 
GuiDisplayControlDialog::createAndUpdateFociTableCheckBoxes()
{
   const int numExistingCheckBoxes = fociTablesCheckBoxes.size();
   const DisplaySettingsStudyMetaData* dssmd = theMainWindow->getBrainSet()->getDisplaySettingsStudyMetaData();
   const int numValidTables = dssmd->getNumberOfSubHeaderNames();
   
   //
   // Update existing check boxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidTables) {
         fociTablesCheckBoxes[i]->setText(dssmd->getSubHeaderNameByIndex(i));
         fociTablesCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidTables; j++) {
      QCheckBox* cb = new QCheckBox(dssmd->getSubHeaderNameByIndex(j));
      fociTablesCheckBoxes.push_back(cb);
      fociTablesGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      fociTablesButtonGroup->addButton(cb);
      cb->show();
      pageFociTableWidgetGroup->addWidget(cb);
   }
   
   //
   // Hide existing checkboxes that are not needed.
   //
   for (int k = numValidTables; k < numExistingCheckBoxes; k++) {
      fociTablesCheckBoxes[k]->hide();
   }
}
      
/** 
 * Foci page containing class selections.
 */
void 
GuiDisplayControlDialog::createFociClassPage()
{
   //
   // Vertical Box Layout for all foci items
   //
   pageFociClass = new QWidget;
   fociSubPageClassLayout = new QVBoxLayout(pageFociClass);
   
   pageFociClassWidgetGroup = new WuQWidgetGroup(this);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* fociClassAllOnButton = new QPushButton("All On");
   fociClassAllOnButton->setAutoDefault(false);
   QObject::connect(fociClassAllOnButton, SIGNAL(clicked()),
                    this, SLOT(fociClassAllOn()));
   QPushButton* fociClassAllOffButton = new QPushButton("All Off");
   fociClassAllOffButton->setAutoDefault(false);
   QObject::connect(fociClassAllOffButton, SIGNAL(clicked()),
                    this, SLOT(fociClassAllOff()));
                    
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(fociClassAllOnButton);
   allOnOffLayout->addWidget(fociClassAllOffButton);
   allOnOffLayout->addStretch();
   
   fociWithoutClassAssignmentsCheckBox = new QCheckBox("Show Foci Without Class Assignments");
   fociWithoutClassAssignmentsCheckBox->setToolTip("If this item is checked, foci wthout\n"
                                                   "a class assignment will be displayed.");
   QObject::connect(fociWithoutClassAssignmentsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readFociClassPage()));
   pageFociClassWidgetGroup->addWidget(fociWithoutClassAssignmentsCheckBox);
                    
   fociSubPageClassLayout->addLayout(allOnOffLayout);
   fociSubPageClassLayout->addWidget(fociWithoutClassAssignmentsCheckBox);
   
   createAndUpdateFociClassCheckBoxes();

   pageWidgetStack->addWidget(pageFociClass);
}

/**
 * Create and update foci class checkboxes.  Because the number of foci may change,
 * this method may update and change the label on existing checkboxes, add new 
 * checkboxes, and hide existing checkboxes if the number of checkboxes is greater
 * than the number of foci classes.
 */
void
GuiDisplayControlDialog::createAndUpdateFociClassCheckBoxes()
{
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   numValidFociClasses = ff->getNumberOfCellClasses();
   
   const int numExistingCheckBoxes = static_cast<int>(fociClassCheckBoxes.size());
   
   if (fociClassGridLayout == NULL) {
      QWidget* classWidget = new QWidget; 
      fociClassGridLayout = new QGridLayout(classWidget);
      const int rowStretchNumber    = 15000;
      fociClassGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, Qt::AlignLeft);
      fociClassGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      fociSubPageClassLayout->addWidget(classWidget);
   }
   
   if (fociClassButtonGroup == NULL) {
      fociClassButtonGroup = new QButtonGroup(this);
      fociClassButtonGroup->setExclusive(false);
      QObject::connect(fociClassButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readFociClassPage()));
   }
   
   //
   // Update exising checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidFociClasses) {
         QString name(ff->getCellClassNameByIndex(i));
         if (name.isEmpty()) {
            name = "\"Foci that are NOT members of any class\"";
         }
         fociClassCheckBoxes[i]->setText(name);
         fociClassCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidFociClasses; j++) {
      QCheckBox* cb = new QCheckBox(ff->getCellClassNameByIndex(j));
      fociClassCheckBoxes.push_back(cb);
      fociClassButtonGroup->addButton(cb, j);
      fociClassGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
      pageFociClassWidgetGroup->addWidget(cb);
   }
   
   //
   // Hide existing checkboxes that are not needed.
   //
   for (int k = numValidFociClasses; k < numExistingCheckBoxes; k++) {
      fociClassCheckBoxes[k]->hide();
   }
}

/**
 * This slot is called when the foci class all on button is pressed.
 */
void
GuiDisplayControlDialog::fociClassAllOn()
{
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   if (ff != NULL) {
      ff->setAllCellClassStatus(true);
   }
   updateFociItems();
   readFociClassPage();
}

/**
 * This slot is called when the foci class all off button is pressed.
 */
void
GuiDisplayControlDialog::fociClassAllOff()
{
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   if (ff != NULL) {
      ff->setAllCellClassStatus(false);
   }
   updateFociItems();
   readFociClassPage();
}

/**
 * Foci page containing color selections.
 */
void
GuiDisplayControlDialog::createFociColorPage()
{
   //
   // Vertical Box Layout for all foci items
   //
   pageFociColor = new QWidget;
   fociSubPageColorLayout = new QVBoxLayout(pageFociColor);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* fociColorAllOnButton = new QPushButton("All On");
   fociColorAllOnButton->setAutoDefault(false);
   QObject::connect(fociColorAllOnButton, SIGNAL(clicked()),
                    this, SLOT(fociColorAllOn()));
   QPushButton* fociColorAllOffButton = new QPushButton("All Off");
   fociColorAllOffButton->setAutoDefault(false);
   QObject::connect(fociColorAllOffButton, SIGNAL(clicked()),
                    this, SLOT(fociColorAllOff()));
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(fociColorAllOnButton);
   allOnOffLayout->addWidget(fociColorAllOffButton);
   allOnOffLayout->addStretch();

   pageFociColorWidgetGroup = new WuQWidgetGroup(this);
   
   fociWithoutMatchingColorCheckBox = new QCheckBox("Show Foci Without Color Assigments");
   fociWithoutMatchingColorCheckBox->setToolTip("If this item is checked, foci wthout\n"
                                                   "a color assignment will be displayed.");
   QObject::connect(fociWithoutMatchingColorCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(readFociColorPage()));
   pageFociColorWidgetGroup->addWidget(fociWithoutMatchingColorCheckBox);
               
   fociSubPageColorLayout->addLayout(allOnOffLayout);
   fociSubPageColorLayout->addWidget(fociWithoutMatchingColorCheckBox);
   
   createAndUpdateFociColorCheckBoxes();

   pageWidgetStack->addWidget(pageFociColor);
}

/**
 * Create and update foci color checkboxes.  Because the number of colors may change,
 * this method may update and change the label on existing checkboxes, add new 
 * checkboxes, and hide existing checkboxes if the number of checkboxes is greater
 * than the number of foci colors.
 */
void
GuiDisplayControlDialog::createAndUpdateFociColorCheckBoxes()
{
   FociColorFile* fociColors = theMainWindow->getBrainSet()->getFociColorFile();
   numValidFociColors = fociColors->getNumberOfColors();
   
   const int numExistingCheckBoxes = static_cast<int>(fociColorCheckBoxes.size());
   
   if (fociColorGridLayout == NULL) {
      QWidget* colorWidget = new QWidget; 
      fociColorGridLayout = new QGridLayout(colorWidget);
      const int rowStretchNumber    = 15000;
      fociColorGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, Qt::AlignLeft);
      fociColorGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      fociSubPageColorLayout->addWidget(colorWidget);
   }
   
   if (fociColorButtonGroup == NULL) {
      fociColorButtonGroup = new QButtonGroup(this);
      fociColorButtonGroup->setExclusive(false);
      QObject::connect(fociColorButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readFociColorPage()));
   }
   
   //
   // Update exising checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidFociColors) {
         fociColorCheckBoxes[i]->setText(fociColors->getColorNameByIndex(i));
         fociColorCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidFociColors; j++) {
      QCheckBox* cb = new QCheckBox(fociColors->getColorNameByIndex(j));
      fociColorCheckBoxes.push_back(cb);
      fociColorButtonGroup->addButton(cb, j);
      fociColorGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
      pageFociColorWidgetGroup->addWidget(cb);
   }
   
   //
   // Hide existing checkboxes that are not needed.
   //
   for (int k = numValidFociColors; k < numExistingCheckBoxes; k++) {
      fociColorCheckBoxes[k]->hide();
   }
}

/**
 * This slot is called when the foci color all on button is pressed.
 */
void
GuiDisplayControlDialog::fociColorAllOn()
{
   FociColorFile* fociColors = theMainWindow->getBrainSet()->getFociColorFile();
   fociColors->setAllSelectedStatus(true);
   updateFociItems();
   readFociColorPage();
}

/**
 * This slot is called when the foci color all off button is pressed.
 */
void
GuiDisplayControlDialog::fociColorAllOff()
{
   FociColorFile* fociColors = theMainWindow->getBrainSet()->getFociColorFile();
   fociColors->setAllSelectedStatus(false);
   updateFociItems();
   readFociColorPage();
}

/**
 * update foci main page.
 */
void 
GuiDisplayControlDialog::updateFociMainPage()
{
   if (pageFociMain == NULL) {
      return;
   }
   pageFociMainWidgetGroup->blockSignals(true);
   
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   
   showFociCheckBox->setChecked(dsc->getDisplayCells());
   showVolumeFociCheckBox->setChecked(dsc->getDisplayVolumeCells());
   showRaisedFociCheckBox->setChecked(dsc->getDisplayFlatCellsRaised());
   showPastedOnto3DFociCheckBox->setChecked(dsc->getDisplayPasteCellsOnto3D());
   
   
   showCorrectHemisphereFociCheckBox->setChecked(dsc->getDisplayCellsOnCorrectHemisphereOnly());
   
   fociSymbolOverrideComboBox->setCurrentIndex(dsc->getSymbolOverride());
   fociSizeSpinBox->setValue(dsc->getDrawSize());
   fociDistSpinBox->setValue(dsc->getDistanceToSurfaceLimit());
   fociOpacityDoubleSpinBox->setValue(dsc->getOpacity());
   fociColorModeComboBox->setCurrentIndex(static_cast<int>(dsc->getColorMode()));
   
   pageFociMain->setEnabled(validFociData);
   pageFociMainWidgetGroup->blockSignals(false);
   
}

/**
 * update foci class page.
 */
void 
GuiDisplayControlDialog::updateFociClassPage(const bool filesChanged)
{
   if (pageFociClass == NULL) {
      return;
   }
   
   pageFociClassWidgetGroup->blockSignals(true);
   
   if (filesChanged) {
      createAndUpdateFociClassCheckBoxes();
   }
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   fociWithoutClassAssignmentsCheckBox->setChecked(dsc->getDisplayCellsWithoutClassAssignments());
   fociClassButtonGroup->blockSignals(true);
   int numClasses = 0;
   if (ff != NULL) {
      numClasses = ff->getNumberOfCellClasses();  
   }
   if (numClasses == numValidFociClasses) {
      for (int i = 0; i < numValidFociClasses; i++) {
         fociClassCheckBoxes[i]->setChecked(ff->getCellClassSelectedByIndex(i));
      }
   }
   else {
      std::cerr << "Number of foci class checkboxes does not equal number of foci classes."
                << std::endl;
   }
   fociClassButtonGroup->blockSignals(false);
   pageFociClass->setEnabled(validFociData);
   
   pageFociClassWidgetGroup->blockSignals(false);
}

/**
 * update foci color page.
 */
void 
GuiDisplayControlDialog::updateFociColorPage(const bool filesChanged)
{
   if (pageFociColor == NULL) {
      return;
   }
   
   pageFociColorWidgetGroup->blockSignals(true);
   
   if (filesChanged) {
      createAndUpdateFociColorCheckBoxes();
   }
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   fociWithoutMatchingColorCheckBox->setChecked(dsc->getDisplayCellsWithoutMatchingColor());
   fociColorButtonGroup->blockSignals(true);
   FociColorFile* fociColors = theMainWindow->getBrainSet()->getFociColorFile();
   const int numColors = fociColors->getNumberOfColors();   
   if (numColors == numValidFociColors) {
      for (int i = 0; i < numValidFociColors; i++) {
         fociColorCheckBoxes[i]->setChecked(fociColors->getSelected(i));
      }
   }
   else {
      std::cerr << "Number of foci color checkboxes does not equal number of foci colors."
                << std::endl;
   }
   fociColorButtonGroup->blockSignals(false);
   pageFociColor->setEnabled(validFociData);

   pageFociColorWidgetGroup->blockSignals(false);
}

/**
 * update foci keyword page.
 */
void 
GuiDisplayControlDialog::updateFociKeywordPage(const bool filesChanged)
{
   if (pageFociKeyword == NULL) {
      return;
   }
   
   if (filesChanged) {
      createAndUpdateFociKeywordCheckBoxes();
   }
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   fociWithoutLinkToStudyWithKeywordsCheckBox->setChecked(dsc->getDisplayCellsWithoutLinkToStudyWithKeywords());
   fociKeywordButtonGroup->blockSignals(true);
   const int numKeywordsInGUI = fociKeywordCheckBoxes.size();
   const DisplaySettingsStudyMetaData* dssmd = theMainWindow->getBrainSet()->getDisplaySettingsStudyMetaData();
   const int numValidKeywords = dssmd->getNumberOfKeywords();
   if (numValidKeywords <= numKeywordsInGUI) {
      for (int k = 0; k < numValidKeywords; k++) {
         fociKeywordCheckBoxes[k]->setChecked(dssmd->getKeywordSelected(k));
      }
   }
   else {
      std::cout << "Number of foci keyword checkboxes is less than number of study metadata keywords."
                << std::endl;
   }
   fociKeywordButtonGroup->blockSignals(false);
   pageFociKeyword->setEnabled(validFociData);
}

/**
 * update foci name page.
 */
void 
GuiDisplayControlDialog::updateFociNamePage(const bool filesChanged)
{
   if (pageFociName == NULL) {
      return;
   }
   
   if (filesChanged) {
      createAndUpdateFociNamesCheckBoxes();
   }
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   fociNamesButtonGroup->blockSignals(true);
   const int numNamesInGUI = fociNamesCheckBoxes.size();
   const int numUniqueNames = ff->getNumberOfCellUniqueNames();
   if (numUniqueNames <= numNamesInGUI) {
      for (int k = 0; k < numUniqueNames; k++) {
         fociNamesCheckBoxes[k]->setChecked(ff->getCellUniqueNameSelectedByIndex(k));
      }
   }
   else {
      std::cout << "Number of foci name checkboxes is less than number of unique names if foci file."
                << std::endl;
   }   
   fociNamesButtonGroup->blockSignals(false);
   pageFociName->setEnabled(validFociData);
}

/**
 * update foci page.
 */
void 
GuiDisplayControlDialog::updateFociTablePage(const bool filesChanged)
{
   if (pageFociTable == NULL) {
      return;
   }
   
   if (filesChanged) {
      createAndUpdateFociTableCheckBoxes();
   }
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   fociWithoutLinkToStudyWithTableSubHeaderCheckBox->setChecked(dsc->getDisplayCellsWithoutLinkToStudyWithTableSubHeader());
   const DisplaySettingsStudyMetaData* dssmd = theMainWindow->getBrainSet()->getDisplaySettingsStudyMetaData();
   fociTablesButtonGroup->blockSignals(true);
   const int numSubHeadersInGUI = fociTablesCheckBoxes.size();
   const int numValidSubHeaders = dssmd->getNumberOfSubHeaderNames();
   if (numValidSubHeaders <= numSubHeadersInGUI) {
      for (int k = 0; k < numValidSubHeaders; k++) {
         fociTablesCheckBoxes[k]->setChecked(dssmd->getSubHeaderNameSelected(k));
      }
   }
   else {
      std::cout << "Number of foci subheader checkboxes is less than number of study metadata subheaders."
                << std::endl; 
   }
   fociTablesButtonGroup->blockSignals(false);
   pageFociTable->setEnabled(validFociData);
}

/**
 * Update all of the foci items in the dialog
 */
void
GuiDisplayControlDialog::updateFociItems(const bool filesChanged)
{
   updatePageSelectionComboBox();

   if (pageOverlayUnderlaySurface != NULL) {
      DisplaySettingsCells* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
      layersFociCheckBox->setEnabled(validFociData);
      layersFociCheckBox->setChecked(dsf->getDisplayCells());
   }
   
   updateFociMainPage();
   updateFociClassPage(filesChanged);
   updateFociColorPage(filesChanged);
   updateFociKeywordPage(filesChanged);
   updateFociNamePage(filesChanged);
   updateFociTablePage(filesChanged);
   updatePageSelectionComboBox();
}

/**
 * called when foci selected on foci page or overlay/underlay surface page.
 */
void 
GuiDisplayControlDialog::showFociToggleSlot(bool b)
{
   if (pageFociMain != NULL) {
      showFociCheckBox->blockSignals(true);
      showFociCheckBox->setChecked(b);
      showFociCheckBox->blockSignals(false);
   }
   if (pageOverlayUnderlaySurface != NULL) {
      layersFociCheckBox->blockSignals(true);
      layersFociCheckBox->setChecked(b);
      layersFociCheckBox->blockSignals(false);
   }
/*
   readFociMainPage(false);
   readFociClassPage(false);
   readFociColorPage(false);
   readFociKeywordPage(false);
   readFociNamePage(false);
   readFociTablePage(false);
*/   
   DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   dsf->setDisplayCells(b);
   dsf->determineDisplayedFoci();   
   GuiBrainModelOpenGL::updateAllGL();   
}      

/**
 * read the foci main page.
 */
void 
GuiDisplayControlDialog::readFociMainPage(const bool updateDisplay)
{
   if (pageFociMain == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   
   dsf->setDisplayCells(showFociCheckBox->isChecked());
   dsf->setDisplayVolumeCells(showVolumeFociCheckBox->isChecked());
   dsf->setDisplayFlatCellsRaised(showRaisedFociCheckBox->isChecked());
   dsf->setDisplayPasteCellsOnto3D(showPastedOnto3DFociCheckBox->isChecked());
   
   dsf->setDisplayCellsOnCorrectHemisphereOnly(showCorrectHemisphereFociCheckBox->isChecked());
   
   dsf->setSymbolOverride(static_cast<ColorFile::ColorStorage::SYMBOL>(
                                fociSymbolOverrideComboBox->currentIndex()));
   dsf->setDrawSize(fociSizeSpinBox->value());
   dsf->setDistanceToSurfaceLimit(fociDistSpinBox->value());
   dsf->setOpacity(fociOpacityDoubleSpinBox->value());
   dsf->setColorMode(static_cast<CellBase::CELL_COLOR_MODE>(fociColorModeComboBox->currentIndex()));
   
   if (updateDisplay) {
      dsf->determineDisplayedFoci();   
      GuiBrainModelOpenGL::updateAllGL();   
   }
}

/**
 * read the foci class page.
 */
void 
GuiDisplayControlDialog::readFociClassPage(const bool updateDisplay)
{
   if (pageFociClass == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   const int numClasses = ff->getNumberOfCellClasses();
   if (numClasses == numValidFociClasses) {
      for (int i = 0; i < numValidFociClasses; i++) {
         ff->setCellClassSelectedByIndex(i, fociClassCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of foci class checkboxes does not equal number of foci classes."
                << std::endl;
   }
   DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   dsf->setDisplayCellsWithoutClassAssignments(fociWithoutClassAssignmentsCheckBox->isChecked());
   if (updateDisplay) {
      dsf->determineDisplayedFoci();   
      GuiBrainModelOpenGL::updateAllGL();   
   }
}

/**
 * read the foci color page.
 */
void 
GuiDisplayControlDialog::readFociColorPage(const bool updateDisplay)
{
   if (pageFociColor == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   FociColorFile* fociColors = theMainWindow->getBrainSet()->getFociColorFile();
   const int numColors = fociColors->getNumberOfColors();   
   if (numColors == numValidFociColors) {
      for (int i = 0; i < numValidFociColors; i++) {
         fociColors->setSelected(i, fociColorCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of foci color checkboxes does not equal number of foci colors."
                << std::endl;
   }
   DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   dsf->setDisplayCellsWithoutMatchingColor(fociWithoutMatchingColorCheckBox->isChecked());
   if (updateDisplay) {
      dsf->determineDisplayedFoci();   
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * read the foci keyword page.
 */
void 
GuiDisplayControlDialog::readFociKeywordPage(const bool updateDisplay)
{
   if (pageFociKeyword == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   pageFociKeywordsWidgetGroup->blockSignals(true);
   
   const int numKeywordsInGUI = fociKeywordCheckBoxes.size();
   DisplaySettingsStudyMetaData* dssmd = theMainWindow->getBrainSet()->getDisplaySettingsStudyMetaData();
   const int numValidKeywords = dssmd->getNumberOfKeywords();
   if (numValidKeywords <= numKeywordsInGUI) {
      for (int k = 0; k < numValidKeywords; k++) {
         dssmd->setKeywordSelected(k, fociKeywordCheckBoxes[k]->isChecked());
      }
   }
   else {
      std::cout << "Number of foci keyword checkboxes is less than number of study metadata keywords."
                << std::endl;
   }
   DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   dsf->setDisplayCellsWithoutLinkToStudyWithKeywords(fociWithoutLinkToStudyWithKeywordsCheckBox->isChecked());
   if (updateDisplay) {
      dsf->determineDisplayedFoci();   
      GuiBrainModelOpenGL::updateAllGL();   
   }
   pageFociKeywordsWidgetGroup->blockSignals(false);
}

/**
 * read the foci name page.
 */
void 
GuiDisplayControlDialog::readFociNamePage(const bool updateDisplay)
{
   if (pageFociName == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   pageFociNameWidgetGroup->blockSignals(true);
   
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   const int numNames = ff->getNumberOfCellUniqueNames();   
   if (numNames <= static_cast<int>(fociNamesCheckBoxes.size())) {
      for (int i = 0; i < numNames; i++) {
         ff->setCellUniqueNameSelectedByIndex(i, fociNamesCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of foci unique name checkboxes is too small when reading foci selections"
                << std::endl;
   }
   DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   if (updateDisplay) {
      dsf->determineDisplayedFoci();   
      GuiBrainModelOpenGL::updateAllGL();   
   }
   pageFociNameWidgetGroup->blockSignals(false);
}

/**
 * read the foci table page.
 */
void 
GuiDisplayControlDialog::readFociTablePage(const bool updateDisplay)
{
   if (pageFociTable == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   pageFociTableWidgetGroup->blockSignals(true);
   
   DisplaySettingsStudyMetaData* dssmd = theMainWindow->getBrainSet()->getDisplaySettingsStudyMetaData();
   const int numSubHeadersInGUI = fociTablesCheckBoxes.size();
   const int numValidSubHeaders = dssmd->getNumberOfSubHeaderNames();
   if (numValidSubHeaders <= numSubHeadersInGUI) {
      for (int k = 0; k < numValidSubHeaders; k++) {
         dssmd->setSubHeaderNameSelected(k, fociTablesCheckBoxes[k]->isChecked());
      }
   }
   else {
      std::cout << "Number of foci subheader checkboxes is less than number of study metadata subheaders."
                << std::endl; 
   }
   DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   dsf->setDisplayCellsWithoutLinkToStudyWithTableSubHeader(fociWithoutLinkToStudyWithTableSubHeaderCheckBox->isChecked());
   if (updateDisplay) {
      dsf->determineDisplayedFoci();   
      GuiBrainModelOpenGL::updateAllGL();   
   }
   pageFociTableWidgetGroup->blockSignals(false);
}

/**
 * Called when a new spec file is loaded
 */
void
GuiDisplayControlDialog::newSpecFileLoaded()
{
   slotSurfaceModelIndexComboBox(0);
}

/**
 * called when surface model combo box is changed.
 */
void 
GuiDisplayControlDialog::slotSurfaceModelIndexComboBox(int item)      
{
   if (pageOverlayUnderlaySurface == NULL) {
      return;
   }
   if ((item >= 0) && (item < static_cast<int>(surfaceModelIndexComboBoxValues.size()))) { 
      const int prevItem = surfaceModelIndex; 
      surfaceModelIndex = surfaceModelIndexComboBoxValues[item];
      
      //
      // If switched to all surfaces
      //
      if (surfaceModelIndex == -1) {
         //
         // Need to copy current selections to ALL
         //
         BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
         bsnc->setPrimaryOverlay(surfaceModelIndex,
                static_cast<BrainModelSurfaceNodeColoring::OVERLAY_SELECTIONS>(
                      primaryOverlayButtonGroup->checkedId()));
         bsnc->setSecondaryOverlay(surfaceModelIndex,
                static_cast<BrainModelSurfaceNodeColoring::OVERLAY_SELECTIONS>(
                      secondaryOverlayButtonGroup->checkedId()));
         bsnc->setUnderlay(surfaceModelIndex,
               static_cast<BrainModelSurfaceNodeColoring::OVERLAY_SELECTIONS>(
                      underlayButtonGroup->checkedId()));
         
         DisplaySettingsArealEstimation* dsae = theMainWindow->getBrainSet()->getDisplaySettingsArealEstimation();
         dsae->setSelectedColumn(surfaceModelIndex, arealEstSelectionComboBox->currentIndex());

         DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
         dsm->setSelectedDisplayColumn(surfaceModelIndex, metricSelectionComboBox->currentIndex());
         dsm->setSelectedThresholdColumn(surfaceModelIndex, dsm->getSelectedThresholdColumn(prevItem));

         DisplaySettingsPaint* dsp = theMainWindow->getBrainSet()->getDisplaySettingsPaint();    
         dsp->setSelectedColumn(surfaceModelIndex, paintSelectionComboBox->currentIndex());

         DisplaySettingsRgbPaint* dsrp = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();
         dsrp->setSelectedColumn(surfaceModelIndex, rgbPaintSelectionComboBox->currentIndex());

         DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
         dsss->setSelectedDisplayColumn(surfaceModelIndex, shapeSelectionComboBox->currentIndex());

         DisplaySettingsTopography* dst = theMainWindow->getBrainSet()->getDisplaySettingsTopography();
         dst->setSelectedColumn(surfaceModelIndex, topographySelectionComboBox->currentIndex());
      }
      
      surfaceModelIndexComboBox->blockSignals(true);
      updateOverlayUnderlayItems();
      surfaceModelIndexComboBox->blockSignals(false);
      
      BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
      bsnc->assignColors();
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * Update the surface model combo box
 */
void
GuiDisplayControlDialog::updateSurfaceModelComboBoxes()
{
   bool previousSurfaceFound = false;
   int previousSurfaceIndex = -1;
   surfaceModelIndexComboBox->clear();
   surfaceModelIndexComboBoxValues.clear();
   surfaceModelIndexComboBox->addItem("All Surfaces");
   surfaceModelIndexComboBoxValues.push_back(-1);
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
      if (theMainWindow->getBrainSet()->getBrainModelSurface(i) != NULL) {
         const BrainModel* bm = theMainWindow->getBrainSet()->getBrainModel(i);
         const QString name(bm->getDescriptiveName());
         surfaceModelIndexComboBox->addItem(name);
         surfaceModelIndexComboBoxValues.push_back(i);
         if (i == surfaceModelIndex) {
            previousSurfaceFound = true;
            previousSurfaceIndex = surfaceModelIndexComboBoxValues.size() - 1;
         }
      }
   }
   
   if (previousSurfaceFound) {
      surfaceModelIndexComboBox->setCurrentIndex(previousSurfaceIndex);
   }
   else {
      surfaceModelIndex = -1;
      surfaceModelIndexComboBox->setCurrentIndex(0);
   }
}

/**
 * Create a surface model combo box
 */
void 
GuiDisplayControlDialog::createSurfaceModelIndexComboBox()
{
}

/**
 * Page containing overlay/underlay control.
 */
void
GuiDisplayControlDialog::createOverlayUnderlaySurfacePage()
{
   pageOverlayUnderlaySurface = new QWidget;
   QVBoxLayout* overlayUnderlayMainPageLayout = new QVBoxLayout(pageOverlayUnderlaySurface);
   pageWidgetStack->addWidget(pageOverlayUnderlaySurface); //, PAGE_NAME_SURFACE_OVERLAY_UNDERLAY);
                                         
   const int PRIMARY_COLUMN   = 0;
   const int SECONDARY_COLUMN = PRIMARY_COLUMN + 1;
   const int UNDERLAY_COLUMN  = SECONDARY_COLUMN + 1;
   const int NAME_COLUMN      = UNDERLAY_COLUMN + 1;
   const int INFO_COLUMN      = NAME_COLUMN + 1;
   const int COMBO_COLUMN     = INFO_COLUMN + 1;
   //const int NUM_COLUMNS      = COMBO_COLUMN + 1;
   
   //
   // Grid Layout for overlay/underlay selection
   //
   QGroupBox* surfaceAttributesGroupBox = new QGroupBox("Surface Attributes");
   overlayUnderlayMainPageLayout->addWidget(surfaceAttributesGroupBox);
   QGridLayout* grid = new QGridLayout(surfaceAttributesGroupBox);
   int rowNumber = 0;
   
   //
   // Set stretching so that column selection combo boxes will stretch
   //
/*
   grid->setColumnStretch(NAME_COLUMN, 1);
   grid->setColumnStretch(INFO_COLUMN, 1);
   grid->setColumnStretch(COMBO_COLUMN, 1000000);
   grid->setColumnStretch(PRIMARY_COLUMN, 1);
   grid->setColumnStretch(SECONDARY_COLUMN, 1);
   grid->setColumnStretch(UNDERLAY_COLUMN, 1);
*/   
   //
   // column titles
   //
   grid->addWidget(new QLabel("Primary\nOverlay"),  rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter); 
   grid->addWidget(new QLabel("Secondary\nOverlay"),  rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter); 
   grid->addWidget(new QLabel("Underlay"), rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter); 
   grid->addWidget(new QLabel("Coloring"), rowNumber, NAME_COLUMN, Qt::AlignLeft);
   rowNumber++;
   
   //
   // Button groups to keep radio buttons set correctly
   //
   primaryOverlayButtonGroup = new QButtonGroup(this);
   QObject::connect(primaryOverlayButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(primaryOverlaySelection(int)));
                    
   secondaryOverlayButtonGroup = new QButtonGroup(this);
   QObject::connect(secondaryOverlayButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(secondaryOverlaySelection(int)));
                    
   underlayButtonGroup = new QButtonGroup(this);
   QObject::connect(underlayButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(underlaySelection(int)));
                    
   
   //
   // None selections
   //
   primaryOverlayNoneButton = new QRadioButton;
   grid->addWidget(primaryOverlayNoneButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayNoneButton, BrainModelSurfaceNodeColoring::OVERLAY_NONE);

   secondaryOverlayNoneButton = new QRadioButton;
   grid->addWidget(secondaryOverlayNoneButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   secondaryOverlayButtonGroup->addButton(secondaryOverlayNoneButton, BrainModelSurfaceNodeColoring::OVERLAY_NONE);

   underlayNoneButton = new QRadioButton;
   grid->addWidget(underlayNoneButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   underlayButtonGroup->addButton(underlayNoneButton, BrainModelSurfaceNodeColoring::OVERLAY_NONE);
   grid->addWidget(new QLabel("No Coloring"), rowNumber, NAME_COLUMN);
   grid->addWidget(new QLabel(" "),    rowNumber, INFO_COLUMN);
   grid->addWidget(new QLabel(" "),    rowNumber, COMBO_COLUMN); 
   rowNumber++;
   
   //
   // This QString is added to each combo box prior to setting its fixed size so 
   // that it will display the number of characters in comboSize.  Changing the
   // number of characters in "comboSize" will change the sizes of the combo boxes
   // for the different data files.
   //
   //const QString comboSize("                     ");
   const int minComboSize = 500;
   
   //
   // Areal Estimation Selections
   //
   primaryOverlayArealEstButton = new QRadioButton;
   grid->addWidget(primaryOverlayArealEstButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayArealEstButton, 
                                     BrainModelSurfaceNodeColoring::OVERLAY_AREAL_ESTIMATION);

   secondaryOverlayArealEstButton = new QRadioButton;
   grid->addWidget(secondaryOverlayArealEstButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   secondaryOverlayButtonGroup->addButton(secondaryOverlayArealEstButton, 
                                       BrainModelSurfaceNodeColoring::OVERLAY_AREAL_ESTIMATION);

   underlayArealEstButton = new QRadioButton;
   grid->addWidget(underlayArealEstButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   underlayButtonGroup->addButton(underlayArealEstButton, 
                               BrainModelSurfaceNodeColoring::OVERLAY_AREAL_ESTIMATION);
   
   arealEstSelectionLabel = new QLabel("Areal Est");
   grid->addWidget(arealEstSelectionLabel, rowNumber, NAME_COLUMN); 
   
   arealEstInfoPushButton = new QPushButton("?");
   QSize infoButtonSize = arealEstInfoPushButton->sizeHint();
   infoButtonSize.setWidth(40);
   arealEstInfoPushButton->setAutoDefault(false);
   arealEstInfoPushButton->setFixedSize(infoButtonSize); 
   grid->addWidget(arealEstInfoPushButton, rowNumber, INFO_COLUMN);
   arealEstInfoPushButton->setToolTip("Press for Info About Areal Estimation");
   QObject::connect(arealEstInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(arealEstInfoPushButtonSelection()));
   
   arealEstSelectionComboBox = new QComboBox;
   //arealEstSelectionComboBox->insertItem(comboSize);
   arealEstSelectionComboBox->setMinimumWidth(minComboSize);
   //arealEstSelectionComboBox->setMaximumWidth(1000);
   //arealEstSelectionComboBox->setFixedSize(arealEstSelectionComboBox->sizeHint());
   grid->addWidget(arealEstSelectionComboBox, rowNumber, COMBO_COLUMN);
   arealEstSelectionComboBox->setToolTip("Choose Areal Estimation Column");
   QObject::connect(arealEstSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(arealEstFileSelection(int)));
   rowNumber++;

   //
   // CoCoMac Selections
   //
   primaryOverlayCocomacButton = new QRadioButton;
   grid->addWidget(primaryOverlayCocomacButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayCocomacButton,
                                     BrainModelSurfaceNodeColoring::OVERLAY_COCOMAC);
                                     
   secondaryOverlayCocomacButton = new QRadioButton;
   grid->addWidget(secondaryOverlayCocomacButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   secondaryOverlayButtonGroup->addButton(secondaryOverlayCocomacButton,
                                     BrainModelSurfaceNodeColoring::OVERLAY_COCOMAC);
   
   underlayCocomacButton = new QRadioButton;
   grid->addWidget(underlayCocomacButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   underlayButtonGroup->addButton(underlayCocomacButton,
                               BrainModelSurfaceNodeColoring::OVERLAY_COCOMAC);
   
   cocomacSelectionLabel = new QLabel("CoCoMac");
   grid->addWidget(cocomacSelectionLabel, rowNumber, NAME_COLUMN);
   
   
   rowNumber++;
   
   //
   // Metric Selections
   //
   primaryOverlayMetricButton = new QRadioButton;
   grid->addWidget(primaryOverlayMetricButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayMetricButton, BrainModelSurfaceNodeColoring::OVERLAY_METRIC);

   secondaryOverlayMetricButton = new QRadioButton;
   grid->addWidget(secondaryOverlayMetricButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   secondaryOverlayButtonGroup->addButton(secondaryOverlayMetricButton, BrainModelSurfaceNodeColoring::OVERLAY_METRIC);

   underlayMetricButton = new QRadioButton;
   grid->addWidget(underlayMetricButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   underlayButtonGroup->addButton(underlayMetricButton, BrainModelSurfaceNodeColoring::OVERLAY_METRIC);
   
   metricSelectionLabel = new QLabel("Metric");
   grid->addWidget(metricSelectionLabel, rowNumber, NAME_COLUMN); 
   
   metricInfoPushButton = new QPushButton("?");
   metricInfoPushButton->setFixedSize(infoButtonSize);
   metricInfoPushButton->setAutoDefault(false);
   grid->addWidget(metricInfoPushButton, rowNumber, INFO_COLUMN);
   metricInfoPushButton->setToolTip("Press for Info About Metric");
   QObject::connect(metricInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(metricInfoPushButtonSelection()));
   
   metricSelectionComboBox = new QComboBox;
   //metricSelectionComboBox->insertItem(comboSize);
   metricSelectionComboBox->setMinimumWidth(minComboSize);
   //metricSelectionComboBox->setMaximumWidth(1000);
   //metricSelectionComboBox->setFixedSize(metricSelectionComboBox->sizeHint());
   grid->addWidget(metricSelectionComboBox, rowNumber, COMBO_COLUMN);
   metricSelectionComboBox->setToolTip("Choose Metric Display Column");
   QObject::connect(metricSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(metricDisplayColumnSelection(int)));
   rowNumber++;

   //
   // Paint Selections
   //
   primaryOverlayPaintButton = new QRadioButton;
   grid->addWidget(primaryOverlayPaintButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayPaintButton, BrainModelSurfaceNodeColoring::OVERLAY_PAINT);

   secondaryOverlayPaintButton = new QRadioButton;
   grid->addWidget(secondaryOverlayPaintButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   secondaryOverlayButtonGroup->addButton(secondaryOverlayPaintButton, BrainModelSurfaceNodeColoring::OVERLAY_PAINT);

   underlayPaintButton = new QRadioButton;
   grid->addWidget(underlayPaintButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   underlayButtonGroup->addButton(underlayPaintButton, BrainModelSurfaceNodeColoring::OVERLAY_PAINT);
   
   paintSelectionLabel = new QLabel("Paint");
   grid->addWidget(paintSelectionLabel, rowNumber, NAME_COLUMN); 
   
   paintInfoPushButton = new QPushButton("?");
   paintInfoPushButton->setFixedSize(infoButtonSize);
   paintInfoPushButton->setAutoDefault(false);
   grid->addWidget(paintInfoPushButton, rowNumber, INFO_COLUMN);
   paintInfoPushButton->setToolTip("Press for Info About Paint");
   QObject::connect(paintInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(paintInfoPushButtonSelection()));
   
   paintSelectionComboBox = new QComboBox;
   //paintSelectionComboBox->insertItem(comboSize);
   paintSelectionComboBox->setMinimumWidth(minComboSize);
   //paintSelectionComboBox->setMaximumWidth(1000);
   //paintSelectionComboBox->setFixedSize(paintSelectionComboBox->sizeHint());
   grid->addWidget(paintSelectionComboBox, rowNumber, COMBO_COLUMN);
   paintSelectionComboBox->setToolTip("Choose Paint Column");
   QObject::connect(paintSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(paintColumnSelection(int)));
   rowNumber++;

   //
   // Probabilistic Atlas Selections
   //
   primaryOverlayProbAtlasSurfaceButton = new QRadioButton;
   grid->addWidget(primaryOverlayProbAtlasSurfaceButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayProbAtlasSurfaceButton, 
                                     BrainModelSurfaceNodeColoring::OVERLAY_PROBABILISTIC_ATLAS);

   secondaryOverlayProbAtlasSurfaceButton = new QRadioButton;
   grid->addWidget(secondaryOverlayProbAtlasSurfaceButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   secondaryOverlayButtonGroup->addButton(secondaryOverlayProbAtlasSurfaceButton, 
                                       BrainModelSurfaceNodeColoring::OVERLAY_PROBABILISTIC_ATLAS);

   underlayProbAtlasSurfaceButton = new QRadioButton;
   grid->addWidget(underlayProbAtlasSurfaceButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   underlayButtonGroup->addButton(underlayProbAtlasSurfaceButton, 
                               BrainModelSurfaceNodeColoring::OVERLAY_PROBABILISTIC_ATLAS);
   
   probAtlasSurfaceSelectionLabel = new QLabel("ProbAtlas");
   grid->addWidget(probAtlasSurfaceSelectionLabel, rowNumber, NAME_COLUMN); 
   
   probAtlasSurfaceInfoPushButton = new QPushButton("?");
   grid->addWidget(probAtlasSurfaceInfoPushButton, rowNumber, INFO_COLUMN);
   probAtlasSurfaceInfoPushButton->setFixedSize(infoButtonSize);
   probAtlasSurfaceInfoPushButton->setAutoDefault(false);
   probAtlasSurfaceInfoPushButton->setToolTip("Press for Info About Probabilistic Atlas");
   QObject::connect(probAtlasSurfaceInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(probAtlasSurfaceInfoPushButtonSelection()));   
   rowNumber++;
   
   //
   // RGB Paint Selections
   //
   primaryOverlayRgbPaintButton = new QRadioButton;
   grid->addWidget(primaryOverlayRgbPaintButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayRgbPaintButton, 
                                     BrainModelSurfaceNodeColoring::OVERLAY_RGB_PAINT);

   secondaryOverlayRgbPaintButton = new QRadioButton;
   grid->addWidget(secondaryOverlayRgbPaintButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   secondaryOverlayButtonGroup->addButton(secondaryOverlayRgbPaintButton, 
                                       BrainModelSurfaceNodeColoring::OVERLAY_RGB_PAINT);

   underlayRgbPaintButton = new QRadioButton;
   grid->addWidget(underlayRgbPaintButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   underlayButtonGroup->addButton(underlayRgbPaintButton, 
                               BrainModelSurfaceNodeColoring::OVERLAY_RGB_PAINT);
   
   rgbPaintSelectionLabel = new QLabel("RGB");
   grid->addWidget(rgbPaintSelectionLabel, rowNumber, NAME_COLUMN); 
   
   rgbPaintInfoPushButton = new QPushButton("?");
   rgbPaintInfoPushButton->setFixedSize(infoButtonSize);
   rgbPaintInfoPushButton->setAutoDefault(false);
   grid->addWidget(rgbPaintInfoPushButton, rowNumber, INFO_COLUMN);
   rgbPaintInfoPushButton->setToolTip("Press for Info About RGB Paint");
   QObject::connect(rgbPaintInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(rgbPaintInfoPushButtonSelection()));
   
   rgbPaintSelectionComboBox = new QComboBox;
   //rgbPaintSelectionComboBox->insertItem(comboSize);
   //rgbPaintSelectionComboBox->setFixedSize(rgbPaintSelectionComboBox->sizeHint());
   rgbPaintSelectionComboBox->setMinimumWidth(minComboSize);
   //rgbPaintSelectionComboBox->setMaximumWidth(1000);
   grid->addWidget(rgbPaintSelectionComboBox, rowNumber, COMBO_COLUMN);
   rgbPaintSelectionComboBox->setToolTip("Choose RGB Paint Column");
   QObject::connect(rgbPaintSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(rgbPaintFileSelection(int)));
   rowNumber++;
   
   //
   // Surface shape Selections
   //
   primaryOverlayShapeButton = new QRadioButton;
   grid->addWidget(primaryOverlayShapeButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayShapeButton, 
                                    BrainModelSurfaceNodeColoring::OVERLAY_SURFACE_SHAPE);

   secondaryOverlayShapeButton = new QRadioButton;
   grid->addWidget(secondaryOverlayShapeButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   secondaryOverlayButtonGroup->addButton(secondaryOverlayShapeButton, 
                                       BrainModelSurfaceNodeColoring::OVERLAY_SURFACE_SHAPE);

   underlayShapeButton = new QRadioButton;
   grid->addWidget(underlayShapeButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   underlayButtonGroup->addButton(underlayShapeButton, 
                               BrainModelSurfaceNodeColoring::OVERLAY_SURFACE_SHAPE);
   
   shapeSelectionLabel = new QLabel("Shape");
   grid->addWidget(shapeSelectionLabel, rowNumber, NAME_COLUMN); 
   
   shapeInfoPushButton = new QPushButton("?");
   grid->addWidget(shapeInfoPushButton, rowNumber, INFO_COLUMN);
   shapeInfoPushButton->setFixedSize(infoButtonSize);
   shapeInfoPushButton->setAutoDefault(false);
   shapeInfoPushButton->setToolTip("Press for Info About Surface Shape");
   QObject::connect(shapeInfoPushButton, SIGNAL(clicked()),
                    this, SLOT(shapeInfoPushButtonSelection()));
   
   shapeSelectionComboBox = new QComboBox;
   //shapeSelectionComboBox->insertItem(comboSize);
   //shapeSelectionComboBox->setFixedSize(shapeSelectionComboBox->sizeHint());
   shapeSelectionComboBox->setMinimumWidth(minComboSize);
   //shapeSelectionComboBox->setMaximumWidth(1000);
   grid->addWidget(shapeSelectionComboBox, rowNumber, COMBO_COLUMN);
   shapeSelectionComboBox->setToolTip("Choose Surface Shape Column");
   QObject::connect(shapeSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(shapeColumnSelection(int)));
   rowNumber++;

   //
   // Topography Selections
   //
   primaryOverlayTopographyButton = new QRadioButton;
   grid->addWidget(primaryOverlayTopographyButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayTopographyButton, 
                                     BrainModelSurfaceNodeColoring::OVERLAY_TOPOGRAPHY);

   secondaryOverlayTopographyButton = new QRadioButton;
   grid->addWidget(secondaryOverlayTopographyButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   secondaryOverlayButtonGroup->addButton(secondaryOverlayTopographyButton, 
                                       BrainModelSurfaceNodeColoring::OVERLAY_TOPOGRAPHY);

   underlayTopographyButton = new QRadioButton;
   grid->addWidget(underlayTopographyButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   underlayButtonGroup->addButton(underlayTopographyButton, 
                               BrainModelSurfaceNodeColoring::OVERLAY_TOPOGRAPHY);
   
   topographySelectionLabel = new QLabel("Topography");
   grid->addWidget(topographySelectionLabel, rowNumber, NAME_COLUMN); 
   
   grid->addWidget(new QLabel(" "), rowNumber, INFO_COLUMN);
   
   topographySelectionComboBox = new QComboBox;
   //topographySelectionComboBox->insertItem(comboSize);
   //topographySelectionComboBox->setFixedSize(topographySelectionComboBox->sizeHint());
   topographySelectionComboBox->setMinimumWidth(minComboSize);
   //topographySelectionComboBox->setMaximumWidth(1000);
   grid->addWidget(topographySelectionComboBox, rowNumber, COMBO_COLUMN);
   topographySelectionComboBox->setToolTip("Choose Topography File");
   QObject::connect(topographySelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(topographyFileSelection(int)));
   rowNumber++;
   
   //
   // Crossovers selections
   //
   primaryOverlayCrossoversButton = new QRadioButton;
   grid->addWidget(primaryOverlayCrossoversButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayCrossoversButton, 
                                     BrainModelSurfaceNodeColoring::OVERLAY_SHOW_CROSSOVERS);
   grid->addWidget(new QLabel("Crossovers"), rowNumber, NAME_COLUMN);
   rowNumber++;
   
   //
   // Sections selections
   //
   primaryOverlaySectionsButton = new QRadioButton;
   grid->addWidget(primaryOverlaySectionsButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlaySectionsButton,
                                     BrainModelSurfaceNodeColoring::OVERLAY_SECTIONS);
   primaryOverlaySectionsLineEdit = new QLineEdit;
   primaryOverlaySectionsLineEdit->setFixedWidth(100);
   QRegExp sectionRX("\\d+[xX]?");
   primaryOverlaySectionsLineEdit->setValidator(new QRegExpValidator(sectionRX,
                                                         primaryOverlaySectionsLineEdit));
   grid->addWidget(primaryOverlaySectionsLineEdit, rowNumber, COMBO_COLUMN);
   QObject::connect(primaryOverlaySectionsLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(readOverlayUnderlaySelections()));
   primaryOverlaySectionsLineEdit->setToolTip(
                 "Enter a number to display a specific section.\n"
                 "Enter a number followed by \"X\" to view every\n"
                 "\"number\" sections.\n"
                 "Example:  \n"
                 "   10    displays only section ten.\n"
                 "   10X   displays every 10th section.");
   primaryOverlaySectionsLabel = new QLabel("Sections");
   grid->addWidget(primaryOverlaySectionsLabel, rowNumber, NAME_COLUMN);
   rowNumber++;
   
   //
   // Edges selections
   //
   primaryOverlayEdgesButton = new QRadioButton;
   grid->addWidget(primaryOverlayEdgesButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   primaryOverlayButtonGroup->addButton(primaryOverlayEdgesButton, 
                                     BrainModelSurfaceNodeColoring::OVERLAY_SHOW_EDGES);
   grid->addWidget(new QLabel("Edges"), rowNumber, NAME_COLUMN);
   rowNumber++;
   
   //
   // Geography blending selections
   //
   grid->addWidget(new QLabel(" "), rowNumber, PRIMARY_COLUMN);
   
   grid->addWidget(new QLabel(" "), rowNumber, SECONDARY_COLUMN);
   
   underlayGeographyBlendingButton = new QRadioButton;
   grid->addWidget(underlayGeographyBlendingButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   underlayButtonGroup->addButton(underlayGeographyBlendingButton,
                               BrainModelSurfaceNodeColoring::OVERLAY_GEOGRAPHY_BLENDING);
                               
   geographyBlendingSelectionLabel = new QLabel("Geo Blend");
   grid->addWidget(geographyBlendingSelectionLabel, rowNumber, NAME_COLUMN);
   
   grid->addWidget(new QLabel(" "), rowNumber, INFO_COLUMN);
   geographyBlendingDoubleSpinBox = new QDoubleSpinBox;
   geographyBlendingDoubleSpinBox->setMinimum(0.0);
   geographyBlendingDoubleSpinBox->setMaximum(1.0);
   geographyBlendingDoubleSpinBox->setSingleStep(0.1);
   geographyBlendingDoubleSpinBox->setDecimals(3);
   geographyBlendingDoubleSpinBox->setFixedWidth(100);
   grid->addWidget(geographyBlendingDoubleSpinBox, rowNumber, COMBO_COLUMN);
   geographyBlendingDoubleSpinBox->setToolTip(
                   "Geography blending factor: \n"
                   "color = factor * overlay-color +\n"
                   "        (1.0 - factor) * underlay-color");
   QObject::connect(geographyBlendingDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readOverlayUnderlaySelections()));                    
   rowNumber++;
   
   //
   // Lighting
   //
   primaryOverlayLightingButton = new QCheckBox;
   grid->addWidget(primaryOverlayLightingButton, rowNumber, PRIMARY_COLUMN, Qt::AlignHCenter);
   QObject::connect(primaryOverlayLightingButton, SIGNAL(clicked()),
                    this, SLOT(primaryOverlayLightSelection()));
   
   secondaryOverlayLightingButton = new QCheckBox;
   grid->addWidget(secondaryOverlayLightingButton, rowNumber, SECONDARY_COLUMN, Qt::AlignHCenter);
   QObject::connect(secondaryOverlayLightingButton, SIGNAL(clicked()),
                    this, SLOT(secondaryOverlayLightSelection()));
   
   underlayLightingButton = new QCheckBox;
   grid->addWidget(underlayLightingButton, rowNumber, UNDERLAY_COLUMN, Qt::AlignHCenter);
   QObject::connect(underlayLightingButton, SIGNAL(clicked()),
                    this, SLOT(underlayLightSelection()));
   
   grid->addWidget(new QLabel("Lighting Enabled"), rowNumber,
                            NAME_COLUMN, 1, COMBO_COLUMN - NAME_COLUMN + 1);
   rowNumber++;
   
   //
   // Opacity
   //
   grid->addWidget(new QLabel("Opacity "), rowNumber, NAME_COLUMN);   
   opacityDoubleSpinBox = new QDoubleSpinBox;
   opacityDoubleSpinBox->setMinimum(0.0);
   opacityDoubleSpinBox->setMaximum(1.0);
   opacityDoubleSpinBox->setSingleStep(0.1);
   opacityDoubleSpinBox->setDecimals(3);
   opacityDoubleSpinBox->setFixedWidth(100);
   opacityDoubleSpinBox->setValue(1.0);
   opacityDoubleSpinBox->setToolTip("Overlay and Underlay Opacity\n"
                                      "color = opacity * overlay-color +\n"
                                      "        (1.0 - opacity) * underlay-color");
   grid->addWidget(opacityDoubleSpinBox, rowNumber, COMBO_COLUMN);
   QObject::connect(opacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readOverlayUnderlaySelections()));
   rowNumber++;
   
   //
   // Fix the size of the stuff
   //
   surfaceAttributesGroupBox->setFixedHeight(surfaceAttributesGroupBox->sizeHint().height());
   //surfaceModelIndexComboBox->setFixedWidth(overlayUnderlayPage->sizeHint().width());
   
   //
   // Layers selections
   // 
   layersBorderCheckBox = new QCheckBox("Borders ");
   QObject::connect(layersBorderCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(showBordersToggleSlot(bool)));
   layersBorderCheckBox->setToolTip("Enable/disable\n"
                                       "display of borders,");

   layersCellsCheckBox  = new QCheckBox("Cells ");
   QObject::connect(layersCellsCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(showCellsToggleSlot(bool)));
   layersCellsCheckBox->setToolTip("Enable/disable\n"
                                      "display of cells.");

   layersFociCheckBox   = new QCheckBox("Foci ");
   QObject::connect(layersFociCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(showFociToggleSlot(bool)));
   layersFociCheckBox->setToolTip("Enable/disable\n"
                                     "display of foci.");
                                     
   //
   // Group box for layers
   //
   QGroupBox* layersGroupBox = new QGroupBox("Layers");
   overlayUnderlayMainPageLayout->addWidget(layersGroupBox);
   QHBoxLayout* layersGroupLayout = new QHBoxLayout(layersGroupBox);
   layersGroupLayout->addWidget(layersBorderCheckBox);
   layersGroupLayout->addWidget(layersCellsCheckBox);
   layersGroupLayout->addWidget(layersFociCheckBox);
   //layersGroupLayout->addStretch();
   layersGroupBox->setFixedSize(layersGroupBox->sizeHint());
   
   pageOverlayUnderlaySurface->setFixedHeight(pageOverlayUnderlaySurface->sizeHint().height());

}

/**
 * Reads the opacity and geography blending factor text boxes
 */
void
GuiDisplayControlDialog::readOverlayUnderlaySelections()
{
   if (creatingDialog) {
      return;
   }
   
   BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
   DisplaySettingsSurface* dss = theMainWindow->getBrainSet()->getDisplaySettingsSurface();
   
   bsnc->setOpacity(opacityDoubleSpinBox->value());
   
   bsnc->setGeographyBlending(geographyBlendingDoubleSpinBox->value());
   
   const QString sectionText = primaryOverlaySectionsLineEdit->text();
   const int xPos = sectionText.indexOf('x', 0, Qt::CaseInsensitive);
   int sectionNumber = 0;
   if (xPos >= 0) {
      sectionNumber = sectionText.left(xPos).toInt();
   }
   else {
      sectionNumber = sectionText.toInt();
   }
   dss->setSectionHighlighting(sectionNumber, (xPos >= 0));
   
   bsnc->assignColors();
   
   if ((bsnc->getPrimaryOverlay(surfaceModelIndex) == BrainModelSurfaceNodeColoring::OVERLAY_METRIC) ||
       (bsnc->getSecondaryOverlay(surfaceModelIndex) == BrainModelSurfaceNodeColoring::OVERLAY_METRIC) ||
       (bsnc->getUnderlay(surfaceModelIndex) == BrainModelSurfaceNodeColoring::OVERLAY_METRIC)) {
      BrainModelVolumeVoxelColoring* voxelColoring = theMainWindow->getBrainSet()->getVoxelColoring();
      voxelColoring->setVolumeFunctionalColoringInvalid();
   }
   
   GuiBrainModelOpenGL::updateAllGL(NULL);      
}

/**
 * Called when return is pressed in a line edit and by other methods to update graphics.
 */
void
GuiDisplayControlDialog::applySelected()
{ 
   PAGE_NAME currentPageName = PAGE_NAME_INVALID;
   if (pageComboBoxItems.empty() == false) {
      const int item = pageComboBox->currentIndex();
      if ((item >= 0) && (item < static_cast<int>(pageComboBoxItems.size()))) {
         currentPageName = pageComboBoxItems[item];
      }
   }
   
   switch (currentPageName) {
      case PAGE_NAME_SURFACE_OVERLAY_UNDERLAY:
         readOverlayUnderlaySelections();
         break;
      case PAGE_NAME_AREAL_ESTIMATION:
         readArealEstimationSelections();
         break;
      case PAGE_NAME_BORDER_MAIN:
         readBorderMainPage();
         break;
      case PAGE_NAME_BORDER_COLOR:
         readBorderColorPage();
         break;
      case PAGE_NAME_BORDER_NAME:
         readBorderNamePage();
         break;
      case PAGE_NAME_CELL_MAIN:
         readCellMainPage();
         break;
      case PAGE_NAME_CELL_CLASS:
         readCellClassPage();
         break;
      case PAGE_NAME_CELL_COLOR:
         readCellColorPage();
         break;
      case PAGE_NAME_COCOMAC_DISPLAY:
         readCocomacDisplayPage();
         break;
      case PAGE_NAME_COCOMAC_INFORMATION:
         // nothing to read
         break;
      case PAGE_NAME_CONTOUR_MAIN:
         readContourMainPage();
         break;
      case PAGE_NAME_CONTOUR_CLASS:
         readContourClassPage();
         break;
      case PAGE_NAME_CONTOUR_COLOR:
         readContourColorPage();
         break;
      case PAGE_NAME_DEFORMATION_FIELD:
         readDeformationFieldPage();
         break;
      case PAGE_NAME_FOCI_MAIN:
         readFociMainPage();
         break;
      case PAGE_NAME_FOCI_CLASS:
         readFociClassPage();
         break;
      case PAGE_NAME_FOCI_COLOR:
         readFociColorPage();
         break;
      case PAGE_NAME_FOCI_KEYWORD:
         readFociKeywordPage();
         break;
      case PAGE_NAME_FOCI_NAME:
         readFociNamePage();
         break;
      case PAGE_NAME_FOCI_TABLE:
         readFociTablePage();
         break;
      case PAGE_NAME_GEODESIC:
         readGeodesicSelections();
         break;
      case PAGE_NAME_IMAGES:
         readImagesSelections();
         break;
      case PAGE_NAME_LATLON:
         readLatLonSelections();
         break;
      case PAGE_NAME_METRIC_SELECTION:
         readMetricSelectionPage();
         break;
      case PAGE_NAME_METRIC_SETTINGS:
         readMetricSettingsPage();
         break;
      case PAGE_NAME_MODELS_MAIN:
         readModelMainPage();
         break;
      case PAGE_NAME_MODELS_SETTINGS:
         readModelSettingsPage();
         break;
      case PAGE_NAME_SHAPE_SELECTION:
         readShapeSelections();
         break;
      case PAGE_NAME_SHAPE_SETTINGS:
         readShapeSettings();
         break;
      case PAGE_NAME_PAINT_COLUMN:
         readPaintColumnSelections();
         break;
      case PAGE_NAME_PAINT_NAMES:
         readPaintNameSelections();
         break;
      case PAGE_NAME_SURFACE_AND_VOLUME:
         readSurfaceAndVolumeSelections();
         break;
      case PAGE_NAME_SURFACE_MISC:
         readMiscSelections();
         break;
      case PAGE_NAME_PROB_ATLAS_SURFACE_MAIN:
         readProbAtlasSurfaceMainPage();
         break;
      case PAGE_NAME_PROB_ATLAS_SURFACE_AREA:
         readProbAtlasSurfaceAreaPage();
         break;
      case PAGE_NAME_PROB_ATLAS_SURFACE_CHANNEL:
         readProbAtlasSurfaceChannelPage();
         break;
      case PAGE_NAME_PROB_ATLAS_VOLUME_MAIN:
         readProbAtlasVolumeMainPage();
         break;
      case PAGE_NAME_PROB_ATLAS_VOLUME_AREA:
         readProbAtlasVolumeAreaPage();
         break;
      case PAGE_NAME_PROB_ATLAS_VOLUME_CHANNEL:
         readProbAtlasVolumeChannelPage();
         break;
      case PAGE_NAME_REGION:
         readRegionSelections();
         break;
      case PAGE_NAME_RGB_PAINT:
         readRgbPaintSelections();
         break;
      case PAGE_NAME_SCENE:
         slotSceneListBox(sceneListBox->currentRow());
         break;
      case PAGE_NAME_SURFACE_VECTOR_SELECTION:
         readSurfaceVectorSelectionPage();
         break;
      case PAGE_NAME_SURFACE_VECTOR_SETTINGS:
         readSurfaceVectorSettingsPage();
         break;
      case PAGE_NAME_TOPOGRAPHY:
         readTopographySelections();
         break;
      case PAGE_NAME_VOLUME_SELECTION:
         readVolumeSelections();
         break;
      case PAGE_NAME_VOLUME_SETTINGS:
         readVolumeSettings();
         break;
      case PAGE_NAME_VOLUME_SURFACE_OUTLINE:
         readVolumeSurfaceOutline();
         break;
      case PAGE_NAME_INVALID:
         break;
   }
   
/*
   if (pageWidgetStack->visibleWidget() == overlayUnderlayPage) {
      readOverlayUnderlaySelections();
   }
   else if (pageWidgetStack->visibleWidget() == cocomacPage) {
      readCocomacSelections();
   }
   else if (pageWidgetStack->visibleWidget() == contourPage) {
      readContourSelections();
   }
   else if (pageWidgetStack->visibleWidget() == metricPage) {
      readMetricSelections();
   }
   else if (currentPage() == shapeMainPage) {
      readShapeSelections();
   }
   else if (currentPage() == borderPage) {
      readBorderSelections();
   }
   else if (currentPage() == cellPage) {
      readCellSelections();
   }
   else if (currentPage() == fociPage) {
      readFociSelections();
   }
   else if (currentPage() == pageSurfaceMisc) {
      readMiscSelections();
   }
   else if (currentPage() == probAtlasPage) {
      readProbAtlasSelections();
   }
   else if (currentPage() == pageRgbPaintMain) {
      readRgbPaintSelections();
   }
   else if (currentPage() == pageTopography) {
      readTopographySelections();
   }
   else if (currentPage() == volumePage) {
      readVolumeSelections();
   }
*/
}

/**
 * Called when primary overlay light checkbox is toggled
 */
void
GuiDisplayControlDialog::primaryOverlayLightSelection()
{
   BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
   bsnc->setPrimaryOverlayLightingOn(! bsnc->getPrimaryOverlayLightingOn());
   primaryOverlayLightingButton->setChecked(bsnc->getPrimaryOverlayLightingOn());
   readOverlayUnderlaySelections();
}

/**
 * Called when secondary overlay light checkbox is toggled
 */
void
GuiDisplayControlDialog::secondaryOverlayLightSelection()
{
   BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
   bsnc->setSecondaryOverlayLightingOn(! bsnc->getSecondaryOverlayLightingOn());
   secondaryOverlayLightingButton->setChecked(bsnc->getSecondaryOverlayLightingOn());
   readOverlayUnderlaySelections();
}

/**
 * Called when underlay light checkbox is toggled
 */
void
GuiDisplayControlDialog::underlayLightSelection()
{
   BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
   bsnc->setUnderlayLightingOn(! bsnc->getUnderlayLightingOn());
   underlayLightingButton->setChecked(bsnc->getUnderlayLightingOn());
   readOverlayUnderlaySelections();
}

/**
 * Update overlay/underlay toggles buttons and combo boxes.  Uusually called after loading 
 * a spec file, a data file, or when the program wants to change an overlay underlay selection.
 */
void
GuiDisplayControlDialog::updateOverlayUnderlayItems()
{
   updatePageSelectionComboBox();
   updateSurfaceModelComboBoxes();
   
   if (pageOverlayUnderlaySurface != NULL) {
      BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
      const DisplaySettingsSurface* dss = theMainWindow->getBrainSet()->getDisplaySettingsSurface();
      
      QRadioButton* primary = NULL;
      switch(bsnc->getPrimaryOverlay(surfaceModelIndex)) {
         case BrainModelSurfaceNodeColoring::OVERLAY_NONE:
            primary = primaryOverlayNoneButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_AREAL_ESTIMATION:
            primary = primaryOverlayArealEstButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_COCOMAC:
            primary = primaryOverlayCocomacButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_METRIC:
            primary = primaryOverlayMetricButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_PAINT:
            primary = primaryOverlayPaintButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_PROBABILISTIC_ATLAS:
            primary = primaryOverlayProbAtlasSurfaceButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_RGB_PAINT:
            primary = primaryOverlayRgbPaintButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SHOW_CROSSOVERS:
            primary = primaryOverlayCrossoversButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SHOW_EDGES:
            primary = primaryOverlayEdgesButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SECTIONS:
            primary = primaryOverlaySectionsButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SURFACE_SHAPE:
            primary = primaryOverlayShapeButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_TOPOGRAPHY:
            primary = primaryOverlayTopographyButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_GEOGRAPHY_BLENDING:
            break;
      }
      if (primary != NULL) {
         if (primary->isChecked() == false) {
            primary->setChecked(true);
         }
      }
      
      QRadioButton* secondary = NULL;
      switch(bsnc->getSecondaryOverlay(surfaceModelIndex)) {
         case BrainModelSurfaceNodeColoring::OVERLAY_NONE:
            secondary = secondaryOverlayNoneButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_AREAL_ESTIMATION:
            secondary = secondaryOverlayArealEstButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_COCOMAC:
            secondary = secondaryOverlayCocomacButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_METRIC:
            secondary = secondaryOverlayMetricButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_PAINT:
            secondary = secondaryOverlayPaintButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_PROBABILISTIC_ATLAS:
            secondary = secondaryOverlayProbAtlasSurfaceButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_RGB_PAINT:
            secondary = secondaryOverlayRgbPaintButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SECTIONS:
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SHOW_CROSSOVERS:
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SHOW_EDGES:
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SURFACE_SHAPE:
            secondary = secondaryOverlayShapeButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_TOPOGRAPHY:
            secondary = secondaryOverlayTopographyButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_GEOGRAPHY_BLENDING:
            break;
      }
      if (secondary != NULL) {
         if (secondary->isChecked() == false) {
            secondary->setChecked(true);
         }
      }
      
      QRadioButton* underlay = NULL;
      switch(bsnc->getUnderlay(surfaceModelIndex)) {
         case BrainModelSurfaceNodeColoring::OVERLAY_NONE:
            underlay = underlayNoneButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_AREAL_ESTIMATION:
            underlay = underlayArealEstButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_COCOMAC:
            underlay = underlayCocomacButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_METRIC:
            underlay = underlayMetricButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_PAINT:
            underlay = underlayPaintButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_PROBABILISTIC_ATLAS:
            underlay = underlayProbAtlasSurfaceButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_RGB_PAINT:
            underlay = underlayRgbPaintButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SECTIONS:
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SHOW_CROSSOVERS:
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SHOW_EDGES:
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_SURFACE_SHAPE:
            underlay = underlayShapeButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_TOPOGRAPHY:
            underlay = underlayTopographyButton;
            break;
         case BrainModelSurfaceNodeColoring::OVERLAY_GEOGRAPHY_BLENDING:
            underlay = underlayGeographyBlendingButton;
            break;
      }
      if (underlay != NULL) {
         if (underlay->isChecked() == false) {
            underlay->setChecked(true);
         }
      }
      
      int sectionNumber;
      bool sectionEveryX;
      dss->getSectionHighlighting(sectionNumber, sectionEveryX);
      QString sectionText = QString::number(sectionNumber);
      if (sectionEveryX) {
         sectionText.append("X");
      }
      primaryOverlaySectionsLineEdit->setText(sectionText);
      
      const SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
      const bool sectionsValid = (sf->getNumberOfColumns() > 0);
      primaryOverlaySectionsButton->setEnabled(sectionsValid);
      primaryOverlaySectionsLabel->setEnabled(sectionsValid);
      primaryOverlaySectionsLineEdit->setEnabled(sectionsValid);
      
      primaryOverlayLightingButton->setChecked(bsnc->getPrimaryOverlayLightingOn());
      secondaryOverlayLightingButton->setChecked(bsnc->getSecondaryOverlayLightingOn());
      underlayLightingButton->setChecked(bsnc->getUnderlayLightingOn());
      
      geographyBlendingDoubleSpinBox->setValue(bsnc->getGeographyBlending());
      
      opacityDoubleSpinBox->setValue(bsnc->getOpacity());
      
      pageOverlayUnderlaySurface->setEnabled(validSurfaceData);
   }
   
   updateArealEstimationItems();
   updateMetricItems();
   updatePaintItems();
   updateShapeItems();
   updateTopographyItems();
}

/**
 * Primary overlay selection slot.
 */
void
GuiDisplayControlDialog::primaryOverlaySelection(int num)
{
   theMainWindow->getBrainSet()->getNodeColoring()->setPrimaryOverlay(surfaceModelIndex,
                static_cast<BrainModelSurfaceNodeColoring::OVERLAY_SELECTIONS>(num));
   readOverlayUnderlaySelections();
}

/**
 * Secondary overlay selection slot.
 */
void
GuiDisplayControlDialog::secondaryOverlaySelection(int num)
{
   theMainWindow->getBrainSet()->getNodeColoring()->setSecondaryOverlay(surfaceModelIndex,
                  static_cast<BrainModelSurfaceNodeColoring::OVERLAY_SELECTIONS>(num));
   readOverlayUnderlaySelections();
}

/**
 * Underlay selection slot.
 */
void
GuiDisplayControlDialog::underlaySelection(int num)
{
   theMainWindow->getBrainSet()->getNodeColoring()->setUnderlay(surfaceModelIndex,
                        static_cast<BrainModelSurfaceNodeColoring::OVERLAY_SELECTIONS>(num));
   readOverlayUnderlaySelections();
}

/**
 * The destructor
 */
GuiDisplayControlDialog::~GuiDisplayControlDialog()
{
}

/**
 * Called when an areal estimation file is selected.
 */
void
GuiDisplayControlDialog::arealEstFileSelection(int col)
{
   DisplaySettingsArealEstimation* dsae = theMainWindow->getBrainSet()->getDisplaySettingsArealEstimation();
   dsae->setSelectedColumn(surfaceModelIndex, col);
   readOverlayUnderlaySelections();
   if (arealEstimationSelectionButtonGroup != NULL) {
      arealEstimationSelectionButtonGroup->blockSignals(true);
      const int num = arealEstimationSelectionButtonGroup->buttons().count();
      if ((col >= 0) && (col < num)) {
         QRadioButton* rb = dynamic_cast<QRadioButton*>(arealEstimationSelectionButtonGroup->button(col));
         rb->setChecked(true);
      }
      arealEstimationSelectionButtonGroup->blockSignals(false);
   }
   if (pageOverlayUnderlaySurface != NULL) {
      arealEstSelectionComboBox->blockSignals(true);
      arealEstSelectionComboBox->setCurrentIndex(col);
      arealEstSelectionComboBox->blockSignals(false);
   }
}

/**
 * This pops up a text area dialog providing information about a data type.
 */
void
GuiDisplayControlDialog::displayDataInfoDialog(const QString& title, const QString& info)
{
   if (dataInfoDialog == NULL) {
      dataInfoDialog = new QtTextEditDialog(this);
   }
   dataInfoDialog->setWindowTitle(title);
   dataInfoDialog->setText(info);
   dataInfoDialog->exec();
}

/**
 * This slot is called when the areal estimation info push button is clicked.
 */
void
GuiDisplayControlDialog::arealEstInfoPushButtonSelection()
{
   DisplaySettingsArealEstimation* dsae = theMainWindow->getBrainSet()->getDisplaySettingsArealEstimation();
   const int col = dsae->getSelectedColumn(surfaceModelIndex);
   arealEstimationCommentColumnSelection(col);
}

/**
 * create the areal estimation page.
 */
void 
GuiDisplayControlDialog::createArealEstimationPage()
{
   //
   // Widget for  items
   //
   pageArealEstimation = new QWidget;
   pageWidgetStack->addWidget(pageArealEstimation);
   arealEstimationMainPageLayout = new QVBoxLayout(pageArealEstimation);
   
   //
   // Display color key button
   //
   QPushButton* colorKeyPushButton = new QPushButton("Display Color Key...");
   colorKeyPushButton->setFixedSize(colorKeyPushButton->sizeHint());
   colorKeyPushButton->setAutoDefault(false);
   QObject::connect(colorKeyPushButton, SIGNAL(clicked()),
                    theMainWindow, SLOT(displayArealEstimationColorKey()));
   arealEstimationMainPageLayout->addWidget(colorKeyPushButton);
                    
   //
   // comment button group
   //
   arealEstimationCommentButtonGroup = new QButtonGroup(this);
   QObject::connect(arealEstimationCommentButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(arealEstimationCommentColumnSelection(int)));

   //
   // metadata button group
   //
   arealEstimationMetaDataButtonGroup = new QButtonGroup(this);
   QObject::connect(arealEstimationMetaDataButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(arealEstimationMetaDataColumnSelection(int)));

   //
   // selection button group
   //
   arealEstimationSelectionButtonGroup = new QButtonGroup(this);
   QObject::connect(arealEstimationSelectionButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(arealEstFileSelection(int)));
}
      
/**
 * Called to display metadata information about an areal estimation column.
 */
void 
GuiDisplayControlDialog::arealEstimationMetaDataColumnSelection(int col)
{
   ArealEstimationFile* aef = theMainWindow->getBrainSet()->getArealEstimationFile();
   if ((col >= 0) && (col < aef->getNumberOfColumns())) {
      StudyMetaDataLinkSet smdls = aef->getColumnStudyMetaDataLinkSet(col);
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         aef->setColumnStudyMetaDataLinkSet(col, smdls);
      }
   }
}

/**
 * Called to display comment information about an areal estimation column.
 */
void
GuiDisplayControlDialog::arealEstimationCommentColumnSelection(int column)
{
   ArealEstimationFile* aef = theMainWindow->getBrainSet()->getArealEstimationFile();
   if ((column >= 0) && (column < aef->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    aef, column);
      dfcd->show();
   }
}

/**
 * Create and update the areal estimation page.
 */
void
GuiDisplayControlDialog::createAndUpdateArealEstimationPage()
{
   ArealEstimationFile* aef = theMainWindow->getBrainSet()->getArealEstimationFile();
   numValidArealEstimation = aef->getNumberOfColumns();
   
   const int numExistingArealEstimation = static_cast<int>(arealEstimationNameLineEdits.size());
   const int nameMinimumWidth = 400;
   
   if (arealEstimationSelectionGridLayout == NULL) {
      QWidget* arealEstimationSelectionWidget = new QWidget;
      arealEstimationSelectionGridLayout = new QGridLayout(arealEstimationSelectionWidget);
      arealEstimationSelectionGridLayout->setMargin(3);
      arealEstimationSelectionGridLayout->setSpacing(3);
      arealEstimationSelectionGridLayout->setColumnMinimumWidth(3, nameMinimumWidth+20);
      //const int rowStretchNumber    = 15000;
      //arealEstimationSelectionGridLayout->addWidget(new QLabel(" "),
      //                                     rowStretchNumber, 2, 1, 1, Qt::AlignLeft);
      
      arealEstimationSelectionGridLayout->setColumnStretch(0, 0);
      arealEstimationSelectionGridLayout->setColumnStretch(1, 0);
      arealEstimationSelectionGridLayout->setColumnStretch(2, 0);
      arealEstimationSelectionGridLayout->setColumnStretch(3, 0);
      //arealEstimationSelectionGridLayout->setRowStretch(rowStretchNumber, 100);
      
      arealEstimationMainPageLayout->addWidget(arealEstimationSelectionWidget, 100, Qt::AlignLeft | Qt::AlignTop);
   }
   
   //
   // Add radio buttons and text boxes
   //
   for (int i = numExistingArealEstimation; i < numValidArealEstimation; i++) {
      //
      // Selection radio button  
      //
      QRadioButton* selRadioButton = new QRadioButton("");
      arealEstimationSelectionRadioButtons.push_back(selRadioButton);
      arealEstimationSelectionGridLayout->addWidget(selRadioButton, i, 0, Qt::AlignLeft);
      arealEstimationSelectionButtonGroup->addButton(selRadioButton, i);
      
      //
      // Comment push button
      //
      QPushButton* commentPushButton = new QPushButton("?");
      commentPushButton->setAutoDefault(false);
      commentPushButton->setFixedWidth(40);
      arealEstimationColumnCommentPushButtons.push_back(commentPushButton);
      arealEstimationCommentButtonGroup->addButton(commentPushButton, i);
      arealEstimationSelectionGridLayout->addWidget(commentPushButton, i, 1, Qt::AlignHCenter);
      
      //
      // Metadata push button
      //
      QPushButton* metaDataPushButton = new QPushButton("M");
      metaDataPushButton->setAutoDefault(false);
      metaDataPushButton->setFixedWidth(40);
      arealEstimationColumnMetaDataPushButtons.push_back(metaDataPushButton);
      arealEstimationMetaDataButtonGroup->addButton(metaDataPushButton, i);
      arealEstimationSelectionGridLayout->addWidget(metaDataPushButton, i, 2, Qt::AlignHCenter);
      
      QLineEdit* le = new QLineEdit;
      le->setMinimumWidth(nameMinimumWidth);
      le->setMaximumWidth(1000);
      arealEstimationNameLineEdits.push_back(le);
      QObject::connect(le, SIGNAL(returnPressed()),
                       this, SLOT(readArealEstimationSelections()));
      arealEstimationSelectionGridLayout->addWidget(le, i, 3, Qt::AlignLeft);
   }
   
   //
   // Update items already in the dialog
   //
   for (int i = 0; i < numValidArealEstimation; i++) {
      arealEstimationColumnCommentPushButtons[i]->show();
      arealEstimationColumnMetaDataPushButtons[i]->show();
      arealEstimationNameLineEdits[i]->setText(aef->getColumnName(i));
      arealEstimationNameLineEdits[i]->home(false);
      arealEstimationNameLineEdits[i]->show();
   }
   
   //
   // Hide columns that are not needed
   //
   for (int i = numValidArealEstimation; i < numExistingArealEstimation; i++) {
      arealEstimationColumnCommentPushButtons[i]->hide();
      arealEstimationColumnMetaDataPushButtons[i]->hide();
      arealEstimationNameLineEdits[i]->hide();
   }
}

/**
 * read the areal estimation page.
 */
void 
GuiDisplayControlDialog::readArealEstimationSelections()
{
   if (pageArealEstimation == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }
   ArealEstimationFile* aef = theMainWindow->getBrainSet()->getArealEstimationFile();
   if (aef->getNumberOfColumns() > 0) {      
      for (int i = 0; i < aef->getNumberOfColumns(); i++) {
         const QString name(arealEstimationNameLineEdits[i]->text());
         if (name != aef->getColumnName(i)) {
            aef->setColumnName(i, name);
         }
      }
   }
   updateArealEstimationItems();
   GuiFilesModified fm;
   fm.setArealEstimationModified();
   theMainWindow->fileModificationUpdate(fm);
}
      
/**
 * update areal estimation items.
 */
void 
GuiDisplayControlDialog::updateArealEstimationItems()
{
   updatePageSelectionComboBox();

   updateArealEstOverlayUnderlaySelections();
   if (pageArealEstimation == NULL) {
      return;
   }
   createAndUpdateArealEstimationPage();
   pageArealEstimation->setEnabled(validArealEstimationData);
   arealEstimationSelectionButtonGroup->blockSignals(true);
   const int num = arealEstimationSelectionButtonGroup->buttons().count();
   DisplaySettingsArealEstimation* dsae = theMainWindow->getBrainSet()->getDisplaySettingsArealEstimation();
   const int col = dsae->getSelectedColumn(surfaceModelIndex);
   if ((col >= 0) && (col < num)) {
      QRadioButton* rb = dynamic_cast<QRadioButton*>(arealEstimationSelectionButtonGroup->button(col));
      rb->setChecked(true);
   }
   arealEstimationSelectionButtonGroup->blockSignals(false);
}

/**
 * Update the areal estimation selection combo box with the currently loaded areal estimation files
 */
void
GuiDisplayControlDialog::updateArealEstOverlayUnderlaySelections()
{
   if (pageOverlayUnderlaySurface == NULL) {
      return;
   }
   
   arealEstSelectionComboBox->clear();
   
   ArealEstimationFile* aef = theMainWindow->getBrainSet()->getArealEstimationFile();
   DisplaySettingsArealEstimation* dsae = theMainWindow->getBrainSet()->getDisplaySettingsArealEstimation();
   
   const int numColumns = aef->getNumberOfColumns();
   
   bool valid = false;
   if (numColumns > 0) {
      valid = true;
      for (int i = 0; i < numColumns; i++) {
         arealEstSelectionComboBox->addItem(aef->getColumnName(i));
      }
      arealEstSelectionComboBox->setCurrentIndex(dsae->getSelectedColumn(surfaceModelIndex));
      arealEstSelectionComboBox->setToolTip(
                    aef->getColumnName(dsae->getSelectedColumn(surfaceModelIndex)));
   }
   
   arealEstSelectionComboBox->setEnabled(valid);
   primaryOverlayArealEstButton->setEnabled(valid);
   secondaryOverlayArealEstButton->setEnabled(valid);
   underlayArealEstButton->setEnabled(valid);
   arealEstSelectionLabel->setEnabled(valid);
   arealEstInfoPushButton->setEnabled(valid);
}

/**
 * Called when a metric file display column is selected.
 */
void
GuiDisplayControlDialog::metricDisplayColumnSelection(int col)
{
/*
   const QObject* obj = sender();
   std::cout << "Sender name " << obj->name() << std::endl;
   std::cout << "Sender class name " << obj->className() << std::endl;
   const QObject* parent = obj->parent();
   std::cout << "Parent name " << parent->name() << std::endl;
   std::cout << std::endl;
*/
   
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   dsm->setApplySelectionToLeftAndRightStructuresFlag(metricApplySelectionToLeftAndRightStructuresFlagCheckBox->isChecked());
   dsm->setSelectedDisplayColumn(surfaceModelIndex, col);
   dsm->setSelectedThresholdColumn(surfaceModelIndex, col);
   
   if (pageMetricSelection != NULL) {
      metricViewButtonGroup->blockSignals(true);
      metricThresholdButtonGroup->blockSignals(true);
   }
   if (pageOverlayUnderlaySurface != NULL) {
      metricSelectionComboBox->blockSignals(true);
   }
   updateMetricSelectionPage();
   if (pageMetricSelection != NULL) {
      metricViewButtonGroup->blockSignals(false);
      metricThresholdButtonGroup->blockSignals(false);
   }
   if (pageOverlayUnderlaySurface != NULL) {
      metricSelectionComboBox->blockSignals(false);
   }

   if (pageOverlayUnderlaySurface != NULL) {
      metricSelectionComboBox->blockSignals(true);
      metricSelectionComboBox->setCurrentIndex(dsm->getSelectedDisplayColumn(surfaceModelIndex));
      metricSelectionComboBox->blockSignals(false);
   }
   readMetricSelectionPage();
}

/**
 * This slot is called when the metric info push button is clicked.
 */
void
GuiDisplayControlDialog::metricInfoPushButtonSelection()
{
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   const int column = dsm->getSelectedDisplayColumn(surfaceModelIndex);
   if ((column >= 0) && (column < mf->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    mf, column);
      dfcd->show();
   }
}

/**
 * Called when a metric column comment is selected.
 */
void
GuiDisplayControlDialog::metricCommentColumnSelection(int column)
{
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   if ((column >= 0) && (column < mf->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    mf, column);
      dfcd->show();
   }
}

/**
 * called when a metric metadata column is selected.
 */
void 
GuiDisplayControlDialog::metricMetaDataColumnSelection(int col)
{
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   if ((col >= 0) && (col < mf->getNumberOfColumns())) {
      StudyMetaDataLinkSet smdls = mf->getColumnStudyMetaDataLinkSet(col);
      GuiStudyMetaDataLinkCreationDialog smdlcd(this);
      smdlcd.initializeSelectedLinkSet(smdls);
      if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
         smdls = smdlcd.getLinkSetCreated();
         mf->setColumnStudyMetaDataLinkSet(col, smdls);
      }
   }
}
      
/**
 * Called when a metric file threshold column is selected.
 */
void
GuiDisplayControlDialog::metricThresholdColumnSelection(int col)
{
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   dsm->setSelectedThresholdColumn(surfaceModelIndex, col);
   dsm->setApplySelectionToLeftAndRightStructuresFlag(metricApplySelectionToLeftAndRightStructuresFlagCheckBox->isChecked());
   updateMetricSelectionPage();
   readMetricSelectionPage();
}

/**
 * Called when a paint column is selected.
 */
void
GuiDisplayControlDialog::paintColumnSelection(int col)
{
   DisplaySettingsPaint* dsp = theMainWindow->getBrainSet()->getDisplaySettingsPaint();
   dsp->setSelectedColumn(surfaceModelIndex, col);
   if (pageOverlayUnderlaySurface != NULL) {
      paintSelectionComboBox->setCurrentIndex(col);
   }
   readPaintColumnSelections();
   updatePaintItems();
   
   // not needed since readPaintSelections makes these calls
   //theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   //GuiBrainModelOpenGL::updateAllGL(NULL);
}

/**
 * This slot is called when the paint info push button is clicked.
 */
void
GuiDisplayControlDialog::paintInfoPushButtonSelection()
{
   DisplaySettingsPaint* dsp = theMainWindow->getBrainSet()->getDisplaySettingsPaint();
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   const int column = dsp->getSelectedColumn(surfaceModelIndex);
   if ((column >= 0) && (column < pf->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    pf, column);
      dfcd->show();
   }
}

/**
 * This slot is called when the rgb paint info push button is clicked.
 */
void
GuiDisplayControlDialog::rgbPaintInfoPushButtonSelection()
{
   DisplaySettingsRgbPaint* dsrp = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();
   RgbPaintFile* rpf = theMainWindow->getBrainSet()->getRgbPaintFile();
   const int column = dsrp->getSelectedColumn(surfaceModelIndex);
   if ((column >= 0) && (column < rpf->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    rpf, column);
      dfcd->show();
   }
}

/**
 * Called when a paint column is selected.
 *
void
GuiDisplayControlDialog::probAtlasFileSelection()
{
   //PaintFile* pf = theMainWindow->getBrainSet()->getProbabilisticAtlasFile();
   //pf->setSelectedColumn(col);
   readOverlayUnderlaySelections();
}
*/

/**
 * This slot is called when the paint info push button is clicked.
 */
void
GuiDisplayControlDialog::probAtlasSurfaceInfoPushButtonSelection()
{
   ProbabilisticAtlasFile* pf = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   displayDataInfoDialog(pf->getFileTitle(), "Prob Atlas"); //pf->getComment());
}

/**
 * Called when a rgb paint file is selected.
 */
void
GuiDisplayControlDialog::rgbPaintFileSelection(int col)
{
   DisplaySettingsRgbPaint* dsrp = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();
   dsrp->setSelectedColumn(surfaceModelIndex, col);

   readRgbPaintSelections();
   updateRgbPaintItems();
}

/**
 * Called when a surface shape column is selected.
 */
void
GuiDisplayControlDialog::shapeColumnSelection(int col)
{
   DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
   dsss->setApplySelectionToLeftAndRightStructuresFlag(shapeApplySelectionToLeftAndRightStructuresFlagCheckBox->isChecked());
   readShapeColorMapping();   // 
   dsss->setSelectedDisplayColumn(surfaceModelIndex, col);
   //shapeSelectionComboBox->setCurrentIndex(dsss->getSelectedDisplayColumn());
   updateShapeItems();
   
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);   
}

/**
 * This slot is called when the surface shape info push button is clicked.
 */
void
GuiDisplayControlDialog::shapeInfoPushButtonSelection()
{
   DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   const int column = dsss->getSelectedDisplayColumn(surfaceModelIndex);
   if ((column >= 0) && (column < ssf->getNumberOfColumns())) {
      GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(theMainWindow,
                                                                    ssf, column);
      dfcd->show();
   }
}

/**
 * Create the probabilistic atlas volume main page
 */
void
GuiDisplayControlDialog::createProbAtlasVolumeMainPage()
{
   //
   // Display mode radio buttons
   //
   probAtlasVolumeNormalButton = new QRadioButton("Normal");
   probAtlasVolumeThresholdButton = new QRadioButton("Threshold");

   //
   // Button group for probabilistic display type
   //
   QButtonGroup* probButtonGroup = new QButtonGroup(this);
   QObject::connect(probButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(probAtlasVolumeModeSelection(int)));
   probButtonGroup->addButton(probAtlasVolumeNormalButton, 0);
   probButtonGroup->addButton(probAtlasVolumeThresholdButton, 1);
       
   //
   // Group box and layout for display mode
   //
   QGroupBox* displayModeGroupBox = new QGroupBox("DisplayMode");
   QVBoxLayout* displayModeGroupBoxLayout = new QVBoxLayout(displayModeGroupBox);
   displayModeGroupBoxLayout->addWidget(probAtlasVolumeNormalButton);
   displayModeGroupBoxLayout->addWidget(probAtlasVolumeThresholdButton);
   
   //
   // Treat ??? as unassiged check box
   //
   probAtlasVolumeUnassignedButton = new QCheckBox("Treat name \"???\" as if it was name \"Unassigned\"");
   QObject::connect(probAtlasVolumeUnassignedButton, SIGNAL(clicked()),
                    this, SLOT(readProbAtlasVolumeMainPage()));

   //
   // Threshold ratio
   //
   QLabel* ratioLabel = new QLabel("Threshold Ratio  ");
   probAtlasVolumeThresholdRatioDoubleSpinBox = new QDoubleSpinBox;
   probAtlasVolumeThresholdRatioDoubleSpinBox->setMinimum(0.0);
   probAtlasVolumeThresholdRatioDoubleSpinBox->setMaximum(1.0);
   probAtlasVolumeThresholdRatioDoubleSpinBox->setSingleStep(0.1);
   probAtlasVolumeThresholdRatioDoubleSpinBox->setDecimals(2);
   QObject::connect(probAtlasVolumeThresholdRatioDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(readProbAtlasVolumeMainPage()));
   QHBoxLayout* ratioLayout = new QHBoxLayout;
   ratioLayout->addWidget(ratioLabel);
   ratioLayout->addWidget(probAtlasVolumeThresholdRatioDoubleSpinBox);
   ratioLayout->addStretch();
   
   //
   // Note label
   //
   QLabel* noteLabel = new QLabel("\nNote: Names \"???\", \"GYRAL\", and \"GYRUS\" are\n"
                                  "ignored when Threshold Display Mode is selected.");
              
   //
   // Study Metadata link
   //
   QPushButton* studyMetaDataLinkPushButton = new QPushButton("Study Metadata Link...");
   studyMetaDataLinkPushButton->setAutoDefault(false);
   studyMetaDataLinkPushButton->setFixedSize(studyMetaDataLinkPushButton->sizeHint());
   QObject::connect(studyMetaDataLinkPushButton, SIGNAL(clicked()),
                    this, SLOT(volumeProbAtlasVolumeStudyMetaDataPushButton()));
                    
   //
   // prob atlas main page and layout
   //
   pageProbAtlasVolumeMain = new QWidget;
   QVBoxLayout* probAtlasVolumeSubPageMainLayout = new QVBoxLayout(pageProbAtlasVolumeMain);
   probAtlasVolumeSubPageMainLayout->addWidget(displayModeGroupBox);
   probAtlasVolumeSubPageMainLayout->addWidget(probAtlasVolumeUnassignedButton);
   probAtlasVolumeSubPageMainLayout->addLayout(ratioLayout);
   probAtlasVolumeSubPageMainLayout->addWidget(noteLabel);
   probAtlasVolumeSubPageMainLayout->addWidget(studyMetaDataLinkPushButton);
   probAtlasVolumeSubPageMainLayout->addStretch();

   pageWidgetStack->addWidget(pageProbAtlasVolumeMain);
}

/**
 * Slot for prob atlas mode selection.
 */
void
GuiDisplayControlDialog::probAtlasVolumeModeSelection(int num)
{
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   dspa->setDisplayType(
      static_cast<DisplaySettingsProbabilisticAtlas::PROBABILISTIC_DISPLAY_TYPE>(num));
   readProbAtlasVolumeMainPage();
}

/**
 * Create prob atlas volume channel selection page
 */
void
GuiDisplayControlDialog::createProbAtlasVolumeChannelPage()
{
   //
   // Vertical Box Layout for all items
   //
   pageProbAtlasVolumeChannel = new QWidget;
   pageProbAtlasVolumeChannel->setFixedWidth(450);
   probAtlasVolumeSubPageChannelLayout = new QVBoxLayout(pageProbAtlasVolumeChannel);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* probAtlasVolumeChannelAllOnButton = new QPushButton("All On");
   probAtlasVolumeChannelAllOnButton->setAutoDefault(false);
   QObject::connect(probAtlasVolumeChannelAllOnButton, SIGNAL(clicked()),
                    this, SLOT(probAtlasVolumeChannelAllOn()));
   QPushButton* probAtlasVolumeChannelAllOffButton = new QPushButton("All Off");
   probAtlasVolumeChannelAllOffButton->setAutoDefault(false);
   QObject::connect(probAtlasVolumeChannelAllOffButton, SIGNAL(clicked()),
                    this, SLOT(probAtlasVolumeChannelAllOff()));
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;
   allOnOffLayout->addWidget(probAtlasVolumeChannelAllOnButton);
   allOnOffLayout->addWidget(probAtlasVolumeChannelAllOffButton);
   allOnOffLayout->addStretch();
   probAtlasVolumeSubPageChannelLayout->addLayout(allOnOffLayout);

   createAndUpdateProbAtlasVolumeChannelCheckBoxes();

   pageWidgetStack->addWidget(pageProbAtlasVolumeChannel);
}

/**
 * This slot is called when the prob atlas volume channel all on button is pressed
 */
void
GuiDisplayControlDialog::probAtlasVolumeChannelAllOn()
{
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   dspa->setAllChannelsSelectedStatus(true);
   updateProbAtlasVolumeItems();
   readProbAtlasVolumeChannelPage();
}

/**
 * This slot is called when the prob atlas volumechannel all off button is pressed
 */
void
GuiDisplayControlDialog::probAtlasVolumeChannelAllOff()
{
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   dspa->setAllChannelsSelectedStatus(false);
   updateProbAtlasVolumeItems();
   readProbAtlasVolumeChannelPage();
}

/**
 * caret prob atlas volume area selection page
 */
void
GuiDisplayControlDialog::createProbAtlasVolumeAreaPage()
{
   //
   // Vertical Box Layout for all items
   //
   pageProbAtlasVolumeArea = new QWidget;
   pageProbAtlasVolumeArea->setFixedWidth(450);
   probAtlasVolumeSubPageAreaLayout = new QVBoxLayout(pageProbAtlasVolumeArea);
   
   //
   // All off and all on pushbuttons
   //
   QPushButton* probAtlasVolumeAreasAllOnButton = new QPushButton("All On");
   QObject::connect(probAtlasVolumeAreasAllOnButton, SIGNAL(clicked()),
                    this, SLOT(probAtlasVolumeAreasAllOn()));
   probAtlasVolumeAreasAllOnButton->setAutoDefault(false);
   QPushButton* probAtlasVolumeAreasAllOffButton = new QPushButton("All Off");
   probAtlasVolumeAreasAllOffButton->setAutoDefault(false);
   QObject::connect(probAtlasVolumeAreasAllOffButton, SIGNAL(clicked()),
                    this, SLOT(probAtlasVolumeAreasAllOff()));
   QHBoxLayout* allOnOffLayout = new QHBoxLayout;                 
   allOnOffLayout->addWidget(probAtlasVolumeAreasAllOnButton);
   allOnOffLayout->addWidget(probAtlasVolumeAreasAllOffButton);
   allOnOffLayout->addStretch();
   probAtlasVolumeSubPageAreaLayout->addLayout(allOnOffLayout);
   
   createAndUpdateProbAtlasVolumeAreaNameCheckBoxes();

   pageWidgetStack->addWidget(pageProbAtlasVolumeArea);
}

/**
 * This slot is called when the prob atlas surfce areas all on button is pressed
 */
void
GuiDisplayControlDialog::probAtlasVolumeAreasAllOn()
{
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   dspa->setAllAreasSelectedStatus(true);
   updateProbAtlasVolumeItems();
   readProbAtlasVolumeAreaPage();
}

/**
 * This slot is called when the prob atlas volume areas all off button is pressed
 */
void
GuiDisplayControlDialog::probAtlasVolumeAreasAllOff()
{
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   dspa->setAllAreasSelectedStatus(false);
   updateProbAtlasVolumeItems();
   readProbAtlasVolumeAreaPage();
}

/**
 * read the probabilistic Volume main page.
 */
void 
GuiDisplayControlDialog::readProbAtlasVolumeMainPage()
{
   if (pageProbAtlasVolumeMain == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }   
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   if (bmv == NULL) {
      return;
   }
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   
   dspa->setTreatQuestColorAsUnassigned(probAtlasVolumeUnassignedButton->isChecked());
   dspa->setThresholdDisplayTypeRatio(probAtlasVolumeThresholdRatioDoubleSpinBox->value());  

   BrainModelVolumeVoxelColoring* vvc = theMainWindow->getBrainSet()->getVoxelColoring();
   vvc->setVolumeProbAtlasColoringInvalid();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
}

/**
 * read the probabilistic Volume area page.
 */
void 
GuiDisplayControlDialog::readProbAtlasVolumeAreaPage()
{
   if (pageProbAtlasVolumeArea == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }   
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   if (bmv == NULL) {
      return;
   }
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   const int numAreas = bmv->getNumberOfProbAtlasNames();
   if (numAreas == numValidProbAtlasVolumeAreas) {
      for (int i = 0; i < numAreas; i++) {
         dspa->setAreaSelected(i, probAtlasVolumeAreasCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of prob atlas volume area checkboxes does not equal number of "
                << "prob atlas volume areas." << std::endl;
   }
   dspa->setThresholdDisplayTypeRatio(probAtlasVolumeThresholdRatioDoubleSpinBox->value());  
   BrainModelVolumeVoxelColoring* vvc = theMainWindow->getBrainSet()->getVoxelColoring();
   vvc->setVolumeProbAtlasColoringInvalid();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
}

/**
 * read the probabilistic Volumechannel page.
 */
void 
GuiDisplayControlDialog::readProbAtlasVolumeChannelPage()
{
   if (pageProbAtlasVolumeChannel == NULL) {
      return;
   }
   if (creatingDialog) {
      return;
   }   
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   if (bmv == NULL) {
      return;
   }
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   const int numChannels = theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles();   
   if (numChannels == numValidProbAtlasVolumeChannels) {
      for (int i = 0; i < numChannels; i++)  {
         dspa->setChannelSelected(i, probAtlasVolumeChannelCheckBoxes[i]->isChecked());
      }
   }
   else {
      std::cerr << "Number of prob atlas volume channel checkboxes does not equal number of "
                << "prob atlas volume channels." << std::endl;
   }
   if (pageProbAtlasSurfaceMain != NULL) {
      dspa->setThresholdDisplayTypeRatio(probAtlasVolumeThresholdRatioDoubleSpinBox->value());  
   }
   BrainModelVolumeVoxelColoring* vvc = theMainWindow->getBrainSet()->getVoxelColoring();
   vvc->setVolumeProbAtlasColoringInvalid();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
}
      
/**
 * create and update the check boxes for prob atlas volume channels
 */
void
GuiDisplayControlDialog::createAndUpdateProbAtlasVolumeChannelCheckBoxes()
{
   numValidProbAtlasVolumeChannels = theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles();
   
   const int numExistingCheckBoxes = static_cast<int>(probAtlasVolumeChannelCheckBoxes.size());
   
   if (probAtlasVolumeChannelGridLayout == NULL) {
      QWidget* channelsWidget = new QWidget;
      probAtlasVolumeChannelGridLayout = new QGridLayout(channelsWidget);
      const int rowStretchNumber    = 15000;
      probAtlasVolumeChannelGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, 1, 1, Qt::AlignLeft);
      probAtlasVolumeChannelGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      probAtlasVolumeSubPageChannelLayout->addWidget(channelsWidget);
   }
   
   if (probAtlasVolumeChannelButtonGroup == NULL) {
      probAtlasVolumeChannelButtonGroup = new QButtonGroup(this);
      probAtlasVolumeChannelButtonGroup->setExclusive(false);
      QObject::connect(probAtlasVolumeChannelButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readProbAtlasVolumeChannelPage()));
   }
   
   //
   // update existing checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidProbAtlasVolumeChannels) {
         VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeProbAtlasFile(i);
         probAtlasVolumeChannelCheckBoxes[i]->setText(vf->getDescriptiveLabel());
         probAtlasVolumeChannelCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidProbAtlasVolumeChannels; j++) {
      VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeProbAtlasFile(j);
      QCheckBox* cb = new QCheckBox(vf->getDescriptiveLabel());
      probAtlasVolumeChannelCheckBoxes.push_back(cb);
      probAtlasVolumeChannelButtonGroup->addButton(cb, j);
      probAtlasVolumeChannelGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
   }
   
   //
   // Hide existing checkboxes that are not needed
   //
   for (int k = numValidProbAtlasVolumeChannels; k < numExistingCheckBoxes; k++) {
      probAtlasVolumeChannelCheckBoxes[k]->hide();
   }
}

/**
 * create and update the check boxes for prob atlas volume area names
 */
void
GuiDisplayControlDialog::createAndUpdateProbAtlasVolumeAreaNameCheckBoxes()
{
   numValidProbAtlasVolumeAreas = 0;
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   if (bmv != NULL) {
      numValidProbAtlasVolumeAreas = bmv->getNumberOfProbAtlasNames();
   }
   
   const int numExistingCheckBoxes = static_cast<int>(probAtlasVolumeAreasCheckBoxes.size());
   
   if (probAtlasVolumeAreasGridLayout == NULL) {
      QWidget* areasWidget = new QWidget; //(probAtlasVolumeAreasScrollView->viewport(), "probAtlasVolumeAreasQVBox"); //probAtlasSubPageArea);
      probAtlasVolumeAreasGridLayout = new QGridLayout(areasWidget);
      const int rowStretchNumber    = 15000;
      probAtlasVolumeAreasGridLayout->addWidget(new QLabel(""),
                                           rowStretchNumber, 0, 1, 1, Qt::AlignLeft);
      probAtlasVolumeAreasGridLayout->setRowStretch(rowStretchNumber, 1000);
      
      probAtlasVolumeSubPageAreaLayout->addWidget(areasWidget);
   }
   
   if (probAtlasVolumeAreasButtonGroup == NULL) {
      probAtlasVolumeAreasButtonGroup = new QButtonGroup(this);
      probAtlasVolumeAreasButtonGroup->setExclusive(false);
      QObject::connect(probAtlasVolumeAreasButtonGroup, SIGNAL(buttonClicked(int)),
                       this, SLOT(readProbAtlasVolumeAreaPage()));
   }
   
   //
   // update existing checkboxes
   //
   for (int i = 0; i < numExistingCheckBoxes; i++) {
      if (i < numValidProbAtlasVolumeAreas) {
         probAtlasVolumeAreasCheckBoxes[i]->setText(bmv->getProbAtlasNameFromIndex(i));
         probAtlasVolumeAreasCheckBoxes[i]->show();
      }
   }
   
   //
   // Add new checkboxes as needed
   //
   for (int j = numExistingCheckBoxes; j < numValidProbAtlasVolumeAreas; j++) {
      QCheckBox* cb = new QCheckBox(bmv->getProbAtlasNameFromIndex(j));
      probAtlasVolumeAreasCheckBoxes.push_back(cb);
      probAtlasVolumeAreasButtonGroup->addButton(cb, j);
      probAtlasVolumeAreasGridLayout->addWidget(cb, j, 0, 1, 1, Qt::AlignLeft);
      cb->show();
   }
   
   //
   // Hide existing checkboxes that are not needed
   //
   for (int k = numValidProbAtlasVolumeAreas; k < numExistingCheckBoxes; k++) {
      probAtlasVolumeAreasCheckBoxes[k]->hide();
   }
}

/**
 * update all prob atlas volume main page.
 */
void 
GuiDisplayControlDialog::updateProbAtlasVolumeMainPage()
{
   if (pageProbAtlasVolumeMain == NULL) {
      return;
   }
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   
   probAtlasVolumeUnassignedButton->setChecked(dspa->getTreatQuestColorAsUnassigned());
   
   switch(dspa->getDisplayType()) {
      case DisplaySettingsProbabilisticAtlas::PROBABILISTIC_DISPLAY_TYPE_NORMAL:
         probAtlasVolumeNormalButton->setChecked(true);
         break;
      case DisplaySettingsProbabilisticAtlas::PROBABILISTIC_DISPLAY_TYPE_THRESHOLD:
         probAtlasVolumeThresholdButton->setChecked(true);
         break;
   }
   probAtlasVolumeThresholdRatioDoubleSpinBox->setValue(dspa->getThresholdDisplayTypeRatio());
   pageProbAtlasVolumeMain->setEnabled(validProbAtlasVolumeData);
}

/**
 * update all prob atlas volume area page.
 */
void 
GuiDisplayControlDialog::updateProbAtlasVolumeAreaPage(const bool filesChanged)
{
   if (pageProbAtlasVolumeArea == NULL) {
      return;
   }
   if (filesChanged) {
      createAndUpdateProbAtlasVolumeAreaNameCheckBoxes();
   }
   int numAreas = 0;
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   if (bmv != NULL) {
      numAreas = bmv->getNumberOfProbAtlasNames();
   }
   if (numAreas == numValidProbAtlasVolumeAreas) {
      for (int i = 0; i < numValidProbAtlasVolumeAreas; i++) {
         probAtlasVolumeAreasCheckBoxes[i]->setChecked(dspa->getAreaSelected(i));
      }
   }
   else {
      std::cerr << "Number of prob atlas volume area checkboxes " << numValidProbAtlasVolumeAreas
                << " does not equal number of prob "
                << "atlas volume areas" << numAreas << "." << std::endl;
   }
   pageProbAtlasVolumeArea->setEnabled(validProbAtlasVolumeData);
}

/**
 * update all prob atlas volume channel page.
 */
void 
GuiDisplayControlDialog::updateProbAtlasVolumeChannelPage(const bool filesChanged)
{
   if (pageProbAtlasVolumeChannel == NULL) {
      return;
   }
   if (filesChanged) {
      createAndUpdateProbAtlasVolumeChannelCheckBoxes();
   }
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasVolume();
   const int numChannels = theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles();
   if (numChannels == numValidProbAtlasVolumeChannels) {
      for (int i = 0; i < numValidProbAtlasVolumeChannels; i++) {
         probAtlasVolumeChannelCheckBoxes[i]->setChecked(dspa->getChannelSelected(i));
      }
   }
   else {
      std::cerr << "Number of prob atlas volume channel checkboxes " << numValidProbAtlasVolumeChannels
                << " does not equal number of prob "
                << "atlas volume channels" << numChannels << "." << std::endl;
   }
   pageProbAtlasVolumeChannel->setEnabled(validProbAtlasVolumeData);
}
      
/**
 * Update prob atlas volume items in dialog
 */
void 
GuiDisplayControlDialog::updateProbAtlasVolumeItems(const bool filesChanged)
{   
   updatePageSelectionComboBox();
   updateProbAtlasVolumeMainPage();
   updateProbAtlasVolumeAreaPage(filesChanged);
   updateProbAtlasVolumeChannelPage(filesChanged);
   
   updatePageSelectionComboBox();
}


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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <QString>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QToolTip>
#include <QStackedWidget>

#include "global_variables.h"

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceClusterToBorderConverter.h"
#include "BrainModelSurfaceConnectedSearchMetric.h"
#include "BrainModelSurfaceGeodesic.h"
#include "BrainModelSurfaceMetricClustering.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfaceRegionOfInterest.h"
#include "BrainModelSurfaceToVolumeConverter.h"
#include "BrainSet.h"
#include "ColorFile.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsSurface.h"
#include "DisplaySettingsSurfaceShape.h"
#include "FileUtilities.h"
#include "GuiBorderNamesListBoxSelectionDialog.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiColorSelectionDialog.h"
#include "GuiFilesModified.h"
#include "GeodesicDistanceFile.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiNameSelectionDialog.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiPaintColumnNamesListBoxSelectionDialog.h"
#include "GuiSmoothingDialog.h"
#include "GuiSurfaceRegionOfInterestDialog.h"
#include "GuiSurfaceToVolumeDialog.h"
#include "LatLonFile.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "NameIndexSort.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "PaintFile.h"
#include "ProbabilisticAtlasFile.h"
#include "QtUtilities.h"
#include "StatisticDataGroup.h"
#include "StatisticsUtilities.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyHelper.h"
#include "global_variables.h"

static const int maxComboBoxWidth = 400;

/**
 * Constructor.
 */
GuiSurfaceRegionOfInterestDialog::GuiSurfaceRegionOfInterestDialog(QWidget* parent)
   : QtDialog(parent)
{
   separatorCharacter = ";";
   paintWithNameIndex = -1;
   metricNodeForQuery = -1;
   shapeNodeForQuery  = -1;
   selectionMode = SELECTION_MODE_ENTIRE_SURFACE;
   
   setSizeGripEnabled(true);
   setWindowTitle("Surface Region of Interest");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(3);
   
   //
   // Tab widget for dialog
   //
   tabWidget = new QTabWidget;
   dialogLayout->addWidget(tabWidget);
   
   createQuerySelectionPage();
   
   createAttributeSelectionPage();
   
   createReportPage();
   
   //
   // The close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
 
   QPushButton* helpButton = new QPushButton("Help");
   helpButton->setAutoDefault(false);
   QObject::connect(helpButton, SIGNAL(clicked()),
                    this, SLOT(slotHelpButton()));
                    
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(5);
   buttonsLayout->addWidget(closeButton);
   buttonsLayout->addWidget(helpButton);
   dialogLayout->addLayout(buttonsLayout);
   
   QtUtilities::makeButtonsSameSize(closeButton, helpButton);
   
   const int width = 750;
   const QSize defSize(width /*queryPage->sizeHint().width()*/, sizeHint().height());
   updateDialog();
   slotSelectionMode(selectionMode);
   
   resize(defSize);
   setFixedWidth(width);
   adjustSize();
//   std::cout << "Current: " << static_cast<int>(size().width())
//             << ", " << static_cast<int>(size().height()) << std::endl;
//   std::cout << "Maximum: " << static_cast<int>(maximumSize().width())
//             << ", " << static_cast<int>(maximumSize().height()) << std::endl;
}

/**
 * Destructor.
 */
GuiSurfaceRegionOfInterestDialog::~GuiSurfaceRegionOfInterestDialog()
{
}

/**
 * Called when help button pressed.
 */
void
GuiSurfaceRegionOfInterestDialog::slotHelpButton()
{
   theMainWindow->showHelpViewerDialog("dialogs/surface_roi_dialog.html");
}

/**
 * Called when dialog closed.
 */
void
GuiSurfaceRegionOfInterestDialog::close()
{
   resetMarkedNodesAndReportHeader();
   const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
         BrainSetNodeAttribute* bna = theMainWindow->getBrainSet()->getNodeAttributes(i);
         bna->setNodeInROI(false);
   }
   updateNumberOfSelectedNodesLabel();
   
   GuiBrainModelOpenGL::MOUSE_MODES m = theMainWindow->getBrainModelOpenGL()->getMouseMode();
   if ((m == GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_BORDER_SELECT) ||
       (m == GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_PAINT_INDEX_SELECT) ||
       (m == GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_METRIC_NODE_SELECT) ||
       (m == GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_GEODESIC_NODE_SELECT)) {
      theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   }
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL();
   
   QDialog::close();
}

/**
 * See if any nodes are in the ROI.
 */
bool
GuiSurfaceRegionOfInterestDialog::haveNodesInROI() const
{
   return (std::find(nodeInROI.begin(), nodeInROI.end(), true) != nodeInROI.end());
}

/**
 * Reset/resize the marked nodes flags.
 */
void
GuiSurfaceRegionOfInterestDialog::resetMarkedNodesAndReportHeader()
{
   const unsigned int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   if (nodeInROI.size() != numNodes) {
      nodeInROI.resize(numNodes, false);
   }

   switch (selectionLogic) {
      case SELECTION_LOGIC_NORMAL:
         reportHeader = "";
         break;
      case SELECTION_LOGIC_AND:
         reportHeader.append("\n--- AND ---\n");
         break;
      case SELECTION_LOGIC_OR:
         reportHeader.append("\n--- OR ----\n");
         break;
      case SELECTION_LOGIC_NOT:
         reportHeader = "\n--- NOT ---\n";
         break;
      case SELECTION_LOGIC_AND_NOT:
         reportHeader = "\n--- AND NOT ---\n";
         break;
   }
   
   std::fill(nodeInROI.begin(), nodeInROI.end(), false);
}

/**
 * Remove any nodes without neighbors from the ROI.
 */
void
GuiSurfaceRegionOfInterestDialog::removeNodesWithoutNeighborsFromROI()
{
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   
   //
   // Unmark any nodes without neighbors
   //
   //const TopologyFile* tf = topologyComboBox->getSelectedTopologyFile(); //()bms->getTopologyFile();
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf != NULL) {
      TopologyHelper th(tf, false, true, false);
      for (int i = 0; i < numNodes; i++) {
         if (th.getNodeHasNeighbors(i) == false) {
            nodeInROI[i] = false;
         }
      }
   }
}

/**
 * Peform Metric ROI.
 */
/*
void
GuiSurfaceRegionOfInterestDialog::metricAndSurfaceShapeROI(const bool metricFlag)
{
   MetricFile* mf = NULL;
   if (metricFlag) {
      mf = theMainWindow->getBrainSet()->getMetricFile();
   }
   else {
      mf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   }
   const int numNodes = mf->getNumberOfNodes();
   const int numCols = mf->getNumberOfColumns();
   if ((numNodes <= 0) || (numCols <= 0)) {
      return;
   }
   
   bool firstWrite = true;
   
   int longestColumnNameLength = 10;
   for (int j = 0; j < numCols; j++) {
      longestColumnNameLength = std::max(longestColumnNameLength,
                                   static_cast<int>(mf->getColumnName(j).length()));
   }
   longestColumnNameLength += 5;
   
   const int columnNumberLength = 10;
   const int numberSize = 16;
   
   for (int j = 0; j < numCols; j++) {
      bool doIt = false;
      if (metricFlag) {
         doIt = metricCheckBoxes[j]->isChecked();
      }
      else {
         doIt = surfaceShapeCheckBoxes[j]->isChecked();
      }
      
      if (doIt) {
         std::vector<float> values;
         for (int i = 0; i < numNodes; i++) {
            if (nodeInROI[i]) { 
               values.push_back(mf->getValue(i, j));
            }
         }
         
         if (values.size() > 0) {
            StatisticsUtilities::DescriptiveStatistics statistics;
            StatisticsUtilities::computeStatistics(values,
                                                  true,
                                                  statistics);
                                                  
            if (firstWrite) {
               if (metricFlag) {
                  reportTextEdit->append("\nMetrics for Region Of Interest\n");
               }
               else {
                  reportTextEdit->append("\nSurface Shape for Region of Interest\n");
               }
               
               QString str;
               str.append(StringUtilities::rightJustify("Column", columnNumberLength));
               str.append(" ");
               if (tabSeparateReport) str.append(separatorCharacter);
               
               str.append(StringUtilities::leftJustify("Name", longestColumnNameLength));
               if (tabSeparateReport) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Average", numberSize));
               if (tabSeparateReport) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Sample Deviation", numberSize));
               if (tabSeparateReport) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Abs-Average", numberSize));
               if (tabSeparateReport) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Sample Abs-Deviation", numberSize));
               if (tabSeparateReport) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Minimum", numberSize));
               if (tabSeparateReport) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Maximum", numberSize));
               if (tabSeparateReport) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Range", numberSize));
               if (tabSeparateReport) str.append(separatorCharacter);

               str.append(StringUtilities::rightJustify("Median", numberSize));
               if (tabSeparateReport) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Abs-Median", numberSize));

               reportTextEdit->append(str);
               firstWrite = false;
            }
            
            QString stats;
            stats.append(StringUtilities::rightJustify(QString::number(j), 
                                                         columnNumberLength));
            stats.append(" ");
            if (tabSeparateReport) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::leftJustify(mf->getColumnName(j),
                                                      longestColumnNameLength));
            if (tabSeparateReport) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.average, 'f', 6),
                                                      numberSize));
            if (tabSeparateReport) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.standardDeviation, 'f', 6), 
                                                      numberSize));
            if (tabSeparateReport) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.absAverage, 'f', 6),
                                                      numberSize));
            if (tabSeparateReport) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.absStandardDeviation, 'f', 6), 
                                                      numberSize));
            if (tabSeparateReport) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.minValue, 'f', 6),
                                                      numberSize));
            if (tabSeparateReport) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.maxValue, 'f', 6),
                                                      numberSize));
            if (tabSeparateReport) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.range, 'f', 6),
                                                      numberSize));
            if (tabSeparateReport) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.median, 'f', 6),
                                                      numberSize));
            if (tabSeparateReport) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.absMedian, 'f', 6),
                                                      numberSize));
            
            reportTextEdit->append(stats);
         }
      }
   }   
}
*/

/**
 * Peform Paint ROI.
 */
/*
void
GuiSurfaceRegionOfInterestDialog::paintROI(const BrainModelSurface* bms,
                                           const double roiArea)
{
   const CoordinateFile* cf = bms->getCoordinateFile();
   //const TopologyFile* tf = topologyComboBox->getSelectedTopologyFile();
   const TopologyFile* tf = bms->getTopologyFile();
   const int numTiles = tf->getNumberOfTiles();
   
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   const int numPaintNames = pf->getNumberOfPaintNames();
   LatLonFile* llf = theMainWindow->getBrainSet()->getLatLonFile();
   const bool latLonValid = (llf->getNumberOfColumns() > 0);
   
   MetricFile* metricFile = theMainWindow->getBrainSet()->getMetricFile();
   const int metricCorrectionColumn = distortionCorrectionMetricColumnComboBox->currentIndex();
   
   //
   // Find longest paint name use in the ROI
   //
   int longestPaintNameLength = 11;
   for (int m = 0; m < numPaintNames; m++) {
      longestPaintNameLength = std::max(static_cast<int>(pf->getPaintNameFromIndex(m).length()),
                                       longestPaintNameLength);
   }
   longestPaintNameLength += 3;
   const int numberSize = 16;
   
   //
   // surface area for each paint name
   //
   std::vector<double> paintNameAreas(numPaintNames, 0.0);
   std::vector<double> paintNameAreasCorrected(numPaintNames, 0.0);
   std::vector<double> paintNameAreasCogX(numPaintNames, 0.0);
   std::vector<double> paintNameAreasCogY(numPaintNames, 0.0);
   std::vector<double> paintNameAreasCogZ(numPaintNames, 0.0);
   std::vector<double> paintNameAreasLat(numPaintNames, 0.0);
   std::vector<double> paintNameAreasLon(numPaintNames, 0.0);
   std::vector<int> paintNameNodeCounts(numPaintNames, 0);
      
   const int numPaintColumns = pf->getNumberOfColumns();
   for (int j = 0; j < numPaintColumns; j++) {
      if (paintCheckBoxes[j]->isChecked()) {
      
         //
         // Reset surface area for each paint name, COG, Lat/Lon
         //
         std::fill(paintNameAreas.begin(), paintNameAreas.end(), 0.0);
         std::fill(paintNameAreasCorrected.begin(), paintNameAreasCorrected.end(), 0.0);

         for (int i = 0; i < numTiles; i++) {
            //
            // Is tile in the ROI ?
            //
            if (tileInROI[i]) {
               int tileNodes[3];
               tf->getTile(i, tileNodes);
  
               for (int k = 0; k < 3; k++) {
                  //
                  // Is this node in the ROI
                  //
                  if (nodeInROI[tileNodes[k]]) {
                     const int node = tileNodes[k];
                     //
                     // Update area node's paint
                     //
                     const int paintNameIndex = pf->getPaint(node, j);
                     float nodeArea = tileArea[i] / 3.0;
                     paintNameAreas[paintNameIndex] += nodeArea;
                     
                     float areaDistCorrect = nodeArea;
                     if (metricCorrectionColumn >= 0) {
                        const double metric = metricFile->getValue(node, metricCorrectionColumn);
                        areaDistCorrect *= std::pow(2.0, metric);
                     }
                     paintNameAreasCorrected[paintNameIndex] += areaDistCorrect;
                  }
               }
            }
         }
         
         std::fill(paintNameAreasCogX.begin(), paintNameAreasCogX.end(), 0.0);
         std::fill(paintNameAreasCogY.begin(), paintNameAreasCogY.end(), 0.0);
         std::fill(paintNameAreasCogZ.begin(), paintNameAreasCogZ.end(), 0.0);
         std::fill(paintNameAreasLat.begin(), paintNameAreasLat.end(), 0.0);
         std::fill(paintNameAreasLon.begin(), paintNameAreasLon.end(), 0.0);
         std::fill(paintNameNodeCounts.begin(), paintNameNodeCounts.end(), 0);
         
         const int numNodes = bms->getNumberOfNodes();
         for (int i = 0; i < numNodes; i++) { 
            if (nodeInROI[i]) {           
               const int paintNameIndex = pf->getPaint(i, j);
               //
               // Update COG
               //
               const float* xyz = cf->getCoordinate(i);
               paintNameAreasCogX[paintNameIndex] += xyz[0];
               paintNameAreasCogY[paintNameIndex] += xyz[1];
               paintNameAreasCogZ[paintNameIndex] += xyz[2];
               paintNameNodeCounts[paintNameIndex]++;
               
               //
               // Update Lat/Lon
               //
               if (latLonValid) {
                  float lat, lon;
                  llf->getLatLon(i, 0, lat, lon);
                  paintNameAreasLat[paintNameIndex] += lat;
                  paintNameAreasLon[paintNameIndex] += lon;
               }
            }
         }
         
         //
         // See which paint names are used by the ROI
         //
         bool headerWritten = false;
         for (int m = 0; m < numPaintNames; m++) {
            if (paintNameAreas[m] > 0.0) {
               const QString indent("     ");
               if (headerWritten == false) {
                  //
                  // Write the header for this paint column
                  //
                  headerWritten = true;
                  
                  reportTextEdit->append("\n");
                  
                  QString line;
                  line.append("Paint Column ");
                  line.append(QString::number(j));
                  line.append(" ");
                  line.append(pf->getColumnName(j));
                  reportTextEdit->append(line);
                  
                  line = "";
                  line.append(indent);
                  if (tabSeparateReport) line.append(separatorCharacter);
   
                  line.append(StringUtilities::leftJustify("Paint Name", 
                                                           longestPaintNameLength));
                  if (tabSeparateReport) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("Area",
                                                            numberSize));
                  if (tabSeparateReport) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("Percent Area",
                                                            numberSize));                                                            
                  if (tabSeparateReport) line.append(separatorCharacter);
                  
                  if (metricCorrectionColumn >= 0) {
                     line.append(StringUtilities::rightJustify("Area-Corr",
                                                               numberSize));
                     if (tabSeparateReport) line.append(separatorCharacter);
                  }
                  
                  line.append(StringUtilities::rightJustify("COG - X",
                                                            numberSize));
                  if (tabSeparateReport) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("COG - Y",
                                                            numberSize));
                  if (tabSeparateReport) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("COG - Z",
                                                            numberSize));
                  if (tabSeparateReport) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("Latitude",
                                                            numberSize));
                  if (tabSeparateReport) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("Longitude",
                                                            numberSize));
                  reportTextEdit->append(line);
               }
               
               //
               // Write the area for this paint name
               //
               QString stats;
               
               stats.append(indent);
               if (tabSeparateReport) stats.append(separatorCharacter);
               
               stats.append(StringUtilities::leftJustify(pf->getPaintNameFromIndex(m),
                                                         longestPaintNameLength));
               if (tabSeparateReport) stats.append(separatorCharacter);
               
               stats.append(StringUtilities::rightJustify(QString::number(paintNameAreas[m], 'f', 3),
                                                          numberSize));
               if (tabSeparateReport) stats.append(separatorCharacter);
                              
               const double percent = (paintNameAreas[m] / roiArea) * 100.0;
               stats.append(StringUtilities::rightJustify(QString::number(percent, 'f', 3),
                                                          numberSize));
               if (tabSeparateReport) stats.append(separatorCharacter);
      
               if (metricCorrectionColumn >= 0) {
                  stats.append(StringUtilities::rightJustify(QString::number(paintNameAreasCorrected[m], 'f', 3),
                                                             numberSize));
                  if (tabSeparateReport) stats.append(separatorCharacter);
                }

               const double numNodesForPaint = paintNameNodeCounts[m];
               
               double cogX = 0.0;
               if (numNodesForPaint > 0) {
                  cogX = paintNameAreasCogX[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogX, 'f', 3),
                                                          numberSize));
               
               if (tabSeparateReport) stats.append(separatorCharacter);
               double cogY = 0.0;
               if (numNodesForPaint > 0) {
                  cogY = paintNameAreasCogY[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogY, 'f', 3),
                                                          numberSize));

               if (tabSeparateReport) stats.append(separatorCharacter);
               double cogZ = 0.0;
               if (numNodesForPaint > 0) {
                  cogZ = paintNameAreasCogZ[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogZ, 'f', 3),
                                                          numberSize));
                                                          
               if (tabSeparateReport) stats.append(separatorCharacter);
               double cogLat = 0.0;
               if (numNodesForPaint > 0) {
                  cogLat = paintNameAreasLat[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogLat, 'f', 3),
                                                          numberSize));
                                                          
               if (tabSeparateReport) stats.append(separatorCharacter);
               double cogLon = 0.0;
               if (numNodesForPaint > 0) {
                  cogLon = paintNameAreasLon[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogLon, 'f', 3),
                                                          numberSize));
               reportTextEdit->append(stats);
            }
         }
      }
   }
}
*/

/**
 * Peform a query on all nodes.
 */
void
GuiSurfaceRegionOfInterestDialog::selectNodesAll()
{
   std::fill(nodeInROI.begin(), nodeInROI.end(), true);
   reportHeader.append("\nQUERY: All Nodes.");
}


/**
 * Select nodes by border.
 */
void
GuiSurfaceRegionOfInterestDialog::selectNodesBorder()
{
   const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   
   //
   // Get the selected flat surface for border queries
   //
   if (borderSurfaceComboBox->count() == 0) {
      GuiMessageBox::warning(this, "No Flat Surface",
             "There is no flat surface which is required for border queries.", "OK");
      return;
   }
   const BrainModelSurface* bms = borderSurfaceComboBox->getSelectedBrainModelSurface();
   const CoordinateFile* cf = bms->getCoordinateFile();
   const float* coords = cf->getCoordinate(0);
   
   //
   // Get the border file for the surface type
   //
   BorderFile bf;
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->copyBordersToBorderFile(bms, bf);
   const int numBorders = bf.getNumberOfBorders();
   if (bf.getNumberOfBorders() <= 0) {
      GuiMessageBox::warning(this, "No Borders",
                          "Flat surface has no borders.", "OK");
      return;
   }
   
   //
   // Need to check all borders since there may be more than one 
   // with the same name.
   //
   for (int i = 0; i < numBorders; i++) {
      Border* b = bf.getBorder(i);
      if (b->getName() == selectedBorderName) {
         std::vector<bool> insideFlags;
         b->pointsInsideBorder2D(coords, numNodes, insideFlags);
         
         for (int j = 0; j < numNodes; j++) {
            if (insideFlags[j]) {
               nodeInROI[j] = true;
            }
         }
      }
   }
   
   QString str("\nQuery: All nodes within all border(s) named ");
   str.append(selectedBorderName);
   reportTextEdit->append(str);
   
   str = "Flat surface for border node inclusion: ";
   str.append(bms->getDescriptiveName());
   reportHeader.append(str);
}

/**
 * Select nodes by metric.
 */
void
GuiSurfaceRegionOfInterestDialog::selectNodesMetric()
{
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   const int modelIndex = bms->getBrainModelIndex();
   
   const bool checkAllNodes = metricAllNodesRadioButton->isChecked();
   
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   const int numNodes = mf->getNumberOfNodes();
   
   if (mf->getNumberOfColumns() <= 0) {
      GuiMessageBox::critical(this, "Error", "There are no metric columns.", "OK");
      return;
   }
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   const int thresholdColumn = dsm->getSelectedThresholdColumn(modelIndex);
   if ((thresholdColumn < 0) || (thresholdColumn >= mf->getNumberOfColumns())) {
      GuiMessageBox::critical(this, "Error", "Threshold column is invalid.", "OK");
      return;
   }
   
   const float lowMetric  = metricLowerThresholdLineEdit->text().toFloat();
   const float highMetric = metricUpperThresholdLineEdit->text().toFloat();
   
   if (checkAllNodes) {
      for (int i = 0; i < numNodes; i++) {
         const float metric = mf->getValue(i, thresholdColumn);
         if ((metric >= lowMetric) && (metric <= highMetric)) {
            nodeInROI[i] = true;
         }
      }
   }
   else {
      if ((metricNodeForQuery < 0) || (metricNodeForQuery >= numNodes)) {
         GuiMessageBox::critical(this, "Error", "Starting metric node invalid.  Reselect it.", "OK");
         return;
      }
      
      //
      // Find metrics connected to selected node that are within metric threshold values
      //
      BrainModelSurfaceConnectedSearchMetric metricSearch(theMainWindow->getBrainSet(), bms, metricNodeForQuery,
                                                          mf,
                                                          thresholdColumn, lowMetric, highMetric);
      try {
         metricSearch.execute();
         for (int i = 0; i < numNodes; i++) {
            if (metricSearch.getNodeConnected(i)) {
               nodeInROI[i] = true;
            }
         }
      }
      catch (BrainModelAlgorithmException& bmae) {
         GuiMessageBox::critical(this, "Error", bmae.whatQString(), "OK");
         return;
      }
   }
   
   QString modeMessage;   
   if (checkAllNodes) {
      modeMessage = "All nodes with metric threshold range";
   }
   else {
      modeMessage = "Node selected with mouse with metric threshold range";
   }
   std::ostringstream str;
   str << "\nQUERY:  "
      << modeMessage.toAscii().constData()
      << " ("
      << lowMetric
      << ", "
      << highMetric
      << ")\n"
      << "Starting Node: "
      << metricNodeForQuery
      << "\n"
      << "Column Name: "
      << mf->getColumnName(thresholdColumn).toAscii().constData()
      << "\n";
   reportHeader.append(str.str().c_str());
}

/**
 * select nodes by crossovers.
 */
void 
GuiSurfaceRegionOfInterestDialog::selectNodesCrossovers()
{
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   const BrainSet* bs = theMainWindow->getBrainSet();
   
   const int numNodes = bms->getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
      const BrainSetNodeAttribute* bna = bs->getNodeAttributes(i);
      if (bna->getCrossover() == BrainSetNodeAttribute::CROSSOVER_YES) {
         nodeInROI[i] = true;
      }
   }
}
      
/**
 * Select nodes by shape.
 */
void
GuiSurfaceRegionOfInterestDialog::selectNodesShape()
{
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   const int modelIndex = bms->getBrainModelIndex();
   
   const bool checkAllNodes = shapeAllNodesRadioButton->isChecked();
   
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   const int numNodes = ssf->getNumberOfNodes();
   
   if (ssf->getNumberOfColumns() <= 0) {
      GuiMessageBox::critical(this, "Error", "There are no shape columns.", "OK");
      return;
   }
   DisplaySettingsSurfaceShape* dss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
   const int column = dss->getSelectedDisplayColumn(modelIndex);
   if ((column < 0) || (column >= ssf->getNumberOfColumns())) {
      GuiMessageBox::critical(this, "Error", "Threshold column is invalid.", "OK");
      return;
   }
   
   const float lowShape  = shapeLowerThresholdLineEdit->text().toFloat();
   const float highShape = shapeUpperThresholdLineEdit->text().toFloat();
   
   if (checkAllNodes) {
      for (int i = 0; i < numNodes; i++) {
         const float shape = ssf->getValue(i, column);
         if ((shape >= lowShape) && (shape <= highShape)) {
            nodeInROI[i] = true;
         }
      }
   }
   else {
      if ((shapeNodeForQuery < 0) || (shapeNodeForQuery >= numNodes)) {
         GuiMessageBox::critical(this, "Error", "Starting shape node invalid.  Reselect it.", "OK");
         return;
      }
      
      //
      // Find shapes connected to selected node that are within shape threshold values
      //
      BrainModelSurfaceConnectedSearchMetric shapeSearch(theMainWindow->getBrainSet(), bms, shapeNodeForQuery,
                                                          ssf,
                                                          column, lowShape, highShape);
      try {
         shapeSearch.execute();
         for (int i = 0; i < numNodes; i++) {
            if (shapeSearch.getNodeConnected(i)) {
               nodeInROI[i] = true;
            }
         }
      }
      catch (BrainModelAlgorithmException& bmae) {
         GuiMessageBox::critical(this, "Error", bmae.whatQString(), "OK");
         return;
      }
   }
   
   QString modeMessage;   
   if (checkAllNodes) {
      modeMessage = "All nodes with shape threshold range";
   }
   else {
      modeMessage = "Node selected with mouse with shape threshold range";
   }
   std::ostringstream str;
   str << "\nQUERY:  "
      << modeMessage.toAscii().constData()
      << " ("
      << lowShape
      << ", "
      << highShape
      << ")\n"
      << "Starting Node: "
      << shapeNodeForQuery
      << "\n"
      << "Column Name: "
      << ssf->getColumnName(column).toAscii().constData()
      << "\n";
   reportHeader.append(str.str().c_str());
}

/**
 * select nodes by paint
 */
void
GuiSurfaceRegionOfInterestDialog::selectNodesPaint()
{
   const int column = paintWithNameCategoryComboBox->currentIndex();
   if (column >= 0) {
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      if (column < pf->getNumberOfColumns()) {
         const int numNodes = pf->getNumberOfNodes();
         for (int i = 0; i < numNodes; i++) {
            if (pf->getPaint(i, column) == paintWithNameIndex) {
               nodeInROI[i] = true;
            }
         }
      }
      
      //
      // Header for text report
      //
      QString str("\nQUERY: All nodes with paint category ");
      str.append(pf->getColumnName(column));
      str.append(" equal to ");
      str.append(pf->getPaintNameFromIndex(paintWithNameIndex));
      reportHeader.append(str);
   }   
}

/** 
 * Called to deselect nodes.
 */
void
GuiSurfaceRegionOfInterestDialog::slotDeselectNodesButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   resetMarkedNodesAndReportHeader();   
   const int numQueryNodes = static_cast<int>(nodeInROI.size());
   for (int i = 0; i < numQueryNodes; i++) {
      if (i < numQueryNodes) {
         BrainSetNodeAttribute* bna = theMainWindow->getBrainSet()->getNodeAttributes(i);
         bna->setNodeInROI(nodeInROI[i]);
      }
   }
   updateNumberOfSelectedNodesLabel();
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
   QApplication::restoreOverrideCursor();
}

/** 
 * Called to select nodes.
 */
void
GuiSurfaceRegionOfInterestDialog::slotSelectNodesButton()
{
   
   if (selectionMode == SELECTION_MODE_NONE) {
      GuiMessageBox::critical(this, "ROI Error",
                            "You must select a query type", "OK");
      return;
   }
   
   switch(selectionLogic) {
      case SELECTION_LOGIC_NORMAL:
         break;
      case SELECTION_LOGIC_AND:
         if (haveNodesInROI() == false) {
            GuiMessageBox::critical(this, "No Nodes Selected",
               "No nodes are presently selected so an \"AND Selection\" will\n"
               "select nothing.  Change to \"Normal Selection\" then select nodes.",
               "OK");
            return;
         }
         break;
      case SELECTION_LOGIC_OR:
         break;
      case SELECTION_LOGIC_NOT:
         if (haveNodesInROI() == false) {
            GuiMessageBox::critical(this, "No Nodes Selected",
               "No nodes are presently selected so a \"NOT Selection\" will\n"
               "select nothing.  Change to \"Normal Selection\" then select nodes.",
               "OK");
            return;
         }
         break;
      case SELECTION_LOGIC_AND_NOT:
         if (haveNodesInROI() == false) {
            GuiMessageBox::critical(this, "No Nodes Selected",
               "No nodes are presently selected so a \"NOT AND Selection\" will\n"
               "select nothing.  Change to \"Normal Selection\" then select nodes.",
               "OK");
            return;
         }
         break;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   std::vector<bool> savedNodeInROI = nodeInROI;
   const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   if (static_cast<int>(savedNodeInROI.size()) != numNodes) {
      savedNodeInROI.resize(numNodes, false);
   }
   
   resetMarkedNodesAndReportHeader();
   
   switch(selectionMode) {
      case SELECTION_MODE_ENTIRE_SURFACE:
         selectNodesAll();
         break;
      case SELECTION_MODE_NODES_WITH_PAINT:
         selectNodesPaint();
         break;
      case SELECTION_MODE_NODES_WITHIN_BORDER:
         if (selectedBorderName.isEmpty()) {
            GuiMessageBox::critical(this, "ROI Error",
                                 "You must select a border name", "OK");
            return;
         }
         selectNodesBorder();
         break;
      case SELECTION_MODE_NODES_WITH_METRIC:
         selectNodesMetric();
         break;
      case SELECTION_MODE_NODES_WITH_SHAPE:
         selectNodesShape();
         break;
      case SELECTION_MODE_NODES_WITH_CROSSOVERS:
         selectNodesCrossovers();
         break;
      case SELECTION_MODE_NONE:
         break;
   }
   
   switch (selectionLogic) {
      case SELECTION_LOGIC_NORMAL:
         break;
      case SELECTION_LOGIC_AND:
         {
            for (int i = 0; i < numNodes; i++) {
               if (savedNodeInROI[i] && nodeInROI[i]) {
                  nodeInROI[i] = true;
               }
               else {
                  nodeInROI[i] = false;
               }
            }
         }
         break;
      case SELECTION_LOGIC_OR:
         {
            for (int i = 0; i < numNodes; i++) {
               if (savedNodeInROI[i] || nodeInROI[i]) {
                  nodeInROI[i] = true;
               }
               else {
                  nodeInROI[i] = false;
               }
            }
         }
         break;
      case SELECTION_LOGIC_NOT:
         {
            for (int i = 0; i < numNodes; i++) {
               nodeInROI[i] = ! nodeInROI[i];
            }
         }
         break;
      case SELECTION_LOGIC_AND_NOT:
         {
            for (int i = 0; i < numNodes; i++) {
               if (savedNodeInROI[i] && (nodeInROI[i] == false)) {
                  nodeInROI[i] = true;
               }
               else {
                  nodeInROI[i] = false;
               }
            }
         }
         break;
   }
   
   //
   // Remove any nodes that do not have neighbors from the ROI
   //
   removeNodesWithoutNeighborsFromROI();
   
   //
   // Set node highlighting
   //
   const int numQueryNodes = static_cast<int>(nodeInROI.size());
   for (int i = 0; i < numNodes; i++) {
      if (i < numQueryNodes) {
         BrainSetNodeAttribute* bna = theMainWindow->getBrainSet()->getNodeAttributes(i);
         bna->setNodeInROI(nodeInROI[i]);
      }
   }
   updateNumberOfSelectedNodesLabel();
   //theMainWindow->getBrainSet()->clearAllDisplayLists();
   slotShowSelectedNodes(showSelectedNodesCheckBox->isChecked());   
   QApplication::restoreOverrideCursor();
}

/**
 * Create the report header (returns true if no nodes in query)
 */
bool
GuiSurfaceRegionOfInterestDialog::createReportHeader(const QString& headerText,
                                                     const bool tabSeparateReportIn,
                                                     float& roiAreaOut)
{
   tabSeparateReport = tabSeparateReportIn;

   //
   // Add the header describing the node selection
   //
   reportTextEdit->append(reportHeader);
   
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      return true;
   }
//   const TopologyFile* tf = topologyComboBox->getSelectedTopologyFile(); //bms->getTopologyFile();
   const TopologyFile* tf = bms->getTopologyFile();
   
   const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   
   //
   // Determine total area and selected area.
   //
   const CoordinateFile* cf = bms->getCoordinateFile();
   double totalArea = 0.0;
   roiAreaOut = 0.0;
   const int numTiles = tf->getNumberOfTiles();
   tileArea.resize(numTiles);
   std::fill(tileArea.begin(), tileArea.end(), 0.0);
   tileInROI.resize(numTiles);
   std::fill(tileInROI.begin(), tileInROI.end(), false);
   
   double centerOfGravity[3] = { 0.0, 0.0, 0.0 };
   
   for (int i = 0; i < numTiles; i++) {
      int nodes[3];
      tf->getTile(i, nodes);
      tileArea[i] = MathUtilities::triangleArea(cf->getCoordinate(nodes[0]),
                                                cf->getCoordinate(nodes[1]),
                                                cf->getCoordinate(nodes[2]));
      totalArea += tileArea[i];
      
      double numMarked = 0.0;
      if (nodeInROI[nodes[0]]) numMarked += 1.0;
      if (nodeInROI[nodes[1]]) numMarked += 1.0;
      if (nodeInROI[nodes[2]]) numMarked += 1.0;
      
      if (tileArea[i] > 0.0) {
         roiAreaOut += (numMarked / 3.0) * tileArea[i];
      }
      tileInROI[i] = (numMarked > 0.0);
   }
   
   for (int m = 0; m < numNodes; m++) {
      if (nodeInROI[m]) {
         const float* xyz = cf->getCoordinate(m);
         centerOfGravity[0] += xyz[0];
         centerOfGravity[1] += xyz[1];
         centerOfGravity[2] += xyz[2];
      }
   }
      
   if (headerText.isEmpty() == false) {
      reportTextEdit->append(headerText);
      reportTextEdit->append("\n");
   }
   QString surf("Surface: ");
   surf.append(bms->getDescriptiveName());
   surf.append("\n");
   reportTextEdit->append(surf);
   
   QString topo("Topology: ");
   topo.append(tf->getDescriptiveName());
   topo.append("\n");
   reportTextEdit->append(topo);
   
   reportTextEdit->append("");
   const int count = std::count(nodeInROI.begin(), nodeInROI.end(), true);
   std::ostringstream str;
   str << count << " of " << numNodes << " nodes in region of interest\n" << std::ends;
   reportTextEdit->append(str.str().c_str());

   str.str("");  // clears contents
   str << "Total Surface Area: " << totalArea << std::ends;
   reportTextEdit->append(str.str().c_str());
   
   str.str("");  // clears contents
   str << "Region of Interest Surface Area: " << roiAreaOut << std::ends;
   reportTextEdit->append(str.str().c_str());
   
   centerOfGravity[0] = centerOfGravity[0] / static_cast<float>(count);
   centerOfGravity[1] = centerOfGravity[1] / static_cast<float>(count);
   centerOfGravity[2] = centerOfGravity[2] / static_cast<float>(count);
   str.str("");  // clears contents
   str << "Region of Interest Center of Gravity: " 
       << centerOfGravity[0] << " "
       << centerOfGravity[1] << " "
       << centerOfGravity[2] << std::ends;
   reportTextEdit->append(str.str().c_str());
   
   str.str("");  // clears contents
   str << "Region Mean Distance Between Nodes: "
       << bms->getMeanDistanceBetweenNodes(&nodeInROI)
       << std::ends;
   reportTextEdit->append(str.str().c_str());
   
   reportTextEdit->append(" ");
   
   return false;
}

/**
 * Called when create paint subregion report button pressed.
 */
void
GuiSurfaceRegionOfInterestDialog::slotCreatePaintReportButton()
{
   //
   // Make sure nodes are selected
   //
   if (haveNodesInROI() == false) {
      GuiMessageBox::critical(this, "Error: no nodes in ROI",
                            "There are no nodes in the region of interest.", "OK");
      return;
   }
      
   //
   // Make sure a valid paint column is selected
   //
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   const int paintColumn = paintRegionReportColumnComboBox->currentIndex();
   if ((paintColumn < 0) || (paintColumn >= pf->getNumberOfColumns())) {
      GuiMessageBox::critical(this, "ERROR", "Invalid (or no) paint column selected.", "OK");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));   
   
   //
   // Get all of the paint names for the column
   //
   std::vector<int> paintIndices;
   pf->getPaintNamesForColumn(paintColumn, paintIndices);
   
   //
   // process each paint index
   //
   const int numPaintIndices = static_cast<int>(paintIndices.size());
   for (int i = 0; i < numPaintIndices; i++) {
      //
      // Get the index
      //
      const int paintIndex = paintIndices[i];
      
      //
      // Save the selected nodes
      //
      std::vector<bool> savedNodeInROI = nodeInROI;
      
      //
      // Limit nodes in ROI to those with valid paint column
      //
      const int numNodes = pf->getNumberOfNodes();
      for (int j = 0; j < numNodes; j++) {
         if (nodeInROI[j]) {
            if (pf->getPaint(j, paintColumn) != paintIndex) {
               nodeInROI[j] = false;
            }
         }
      }
      
      //
      // Print the report
      //
      std::ostringstream str;
      str << "Paint Subregion Name: "
          << pf->getPaintNameFromIndex(paintIndex).toAscii().constData();
      createReport(str.str().c_str(), tabSeparatePaintReportCheckBox->isChecked(),
                   (i == (numPaintIndices - 1)));
      
      //
      // Restore the selected nodes
      //
      nodeInROI = savedNodeInROI;
   }
   QApplication::restoreOverrideCursor();
}

/**
 * Called when create report button pressed.
 */
void
GuiSurfaceRegionOfInterestDialog::slotCreateReportButton()
{
   if (haveNodesInROI() == false) {
      GuiMessageBox::critical(this, "Error: no nodes in ROI",
                            "There are no nodes in the region of interest.", "OK");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   createReport("", tabSeparateReportCheckBox->isChecked(), true);
   QApplication::restoreOverrideCursor();
}

/**
 * create the report.
 */
void 
GuiSurfaceRegionOfInterestDialog::createReport(const QString& headerText, 
                                               const bool tabSeparateReportFlag,
                                               const bool doConclusion)
{
   std::vector<bool> metricSelections;
   for (unsigned int i = 0; i < metricCheckBoxes.size(); i++) {
      metricSelections.push_back(metricCheckBoxes[i]->isChecked());
   }
   
   std::vector<bool> shapeSelections;
   for (unsigned int i = 0; i < surfaceShapeCheckBoxes.size(); i++) {
      shapeSelections.push_back(surfaceShapeCheckBoxes[i]->isChecked());
   }
   
   std::vector<bool> paintSelections;
   for (unsigned int i = 0; i < paintCheckBoxes.size(); i++) {
      paintSelections.push_back(paintCheckBoxes[i]->isChecked());
   }
   
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelSurfaceRegionOfInterest bmsri(bs,
                                           operationSurfaceComboBox->getSelectedBrainModelSurface(),
                                           BrainModelSurfaceRegionOfInterest::OPERATION_TEXT_REPORT,
                                           nodeInROI);
   bmsri.setTextReportControlsAndOptions(bs->getMetricFile(),
                                         metricSelections,
                                         bs->getSurfaceShapeFile(),
                                         shapeSelections,
                                         bs->getPaintFile(),
                                         paintSelections,
                                         bs->getLatLonFile(),
                                         0,  // lat/lon file column
                                         reportHeader,
                                         headerText,
                                         bs->getMetricFile(),
                                         distortionCorrectionMetricColumnComboBox->currentIndex(),
                                         tabSeparateReportFlag);
   try {
      bmsri.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
      return;
   }

   reportTextEdit->setText(bmsri.getReportText());
   
/*                                           
   float roiArea = 0.0;
   if (createReportHeader(headerText, tabSeparateReportFlag, roiArea)) {
      return;
   }
   
   //
   // ROI for metrics
   //   
   metricAndSurfaceShapeROI(true);
   
   //
   // ROI for surface shape
   //   
   metricAndSurfaceShapeROI(false);
   
   //
   // ROI for paint
   //
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   paintROI(bms, roiArea);
*/
   
   if (doConclusion) {
      //
      // Switch to report page
      //
      tabWidget->setCurrentWidget(reportPage);
      
      QApplication::beep();
      
      theMainWindow->speakText("The report is ready.", false);
   }
}

/**
 * Create the query page
 */
void
GuiSurfaceRegionOfInterestDialog::createQuerySelectionPage()
{
   //
   // Create the query page
   //
   queryPage = new QWidget;  
   tabWidget->addTab(queryPage, "Query");
   QVBoxLayout* layout = new QVBoxLayout(queryPage);
   layout->setSpacing(3);
   
   //-----------------------------------------------------------------------------
   //
   // Group Box for node selection
   //
   QGroupBox* selectionModeGroupBox = new QGroupBox("Region of Interest Node Selection");
   layout->addWidget(selectionModeGroupBox);
   QVBoxLayout* selectionModeGroupLayout = new QVBoxLayout(selectionModeGroupBox);
   
   //
   // Node selection method label and combo box
   //
   QLabel* selectionMethodLabel = new QLabel("Selection Method ");
   selectionModeComboBox = new QComboBox;
   QObject::connect(selectionModeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSelectionMode(int)));
   selectionModeComboBox->insertItem(SELECTION_MODE_ENTIRE_SURFACE, "Entire Surface");
   selectionModeComboBox->insertItem(SELECTION_MODE_NODES_WITH_PAINT, "Nodes With Paint");
   selectionModeComboBox->insertItem(SELECTION_MODE_NODES_WITHIN_BORDER, "Nodes Within Border");
   selectionModeComboBox->insertItem(SELECTION_MODE_NODES_WITH_METRIC, "Nodes With Metric");
   selectionModeComboBox->insertItem(SELECTION_MODE_NODES_WITH_SHAPE, "Nodes with Surface Shape");
   selectionModeComboBox->insertItem(SELECTION_MODE_NODES_WITH_CROSSOVERS, "Nodes with Crossovers");
   selectionModeComboBox->setFixedWidth(selectionModeComboBox->sizeHint().width());
   QHBoxLayout* selectionMethodBoxLayout = new QHBoxLayout;
   selectionModeGroupLayout->addLayout(selectionMethodBoxLayout);
   selectionMethodBoxLayout->addWidget(selectionMethodLabel);
   selectionMethodBoxLayout->addWidget(selectionModeComboBox);
   //selectionMethodBoxLayout->setStretchFactor(selectionMethodLabel, 0);
   //selectionMethodBoxLayout->setStretchFactor(selectionModeComboBox, 100);
   selectionMethodBoxLayout->addStretch();
   
   //-----------------------------------------------------------------------------
   //
   // Widget stack for different query controls
   //
   queryControlWidgetStack = new QStackedWidget;
   selectionModeGroupLayout->addWidget(queryControlWidgetStack);
   
   //-----------------------------------------------------------------------------
   //
   // Empty widget for entire surface
   //
   queryEntireSurfaceWidget = new QWidget;
   queryControlWidgetStack->addWidget(queryEntireSurfaceWidget);
   
   //-----------------------------------------------------------------------------
   //
   // nodes with paint
   //
   createNodeSelectionPaint();
   queryControlWidgetStack->addWidget(nodesWithPaintQVBox);
   
   //-----------------------------------------------------------------------------
   //
   // nodes within border
   //
   createNodeSelectionBorder();
   queryControlWidgetStack->addWidget(nodesWithinBorderQVBox);
   
   //-----------------------------------------------------------------------------
   //
   // nodes with metric
   //
   createNodeSelectionMetric();
   queryControlWidgetStack->addWidget(nodesWithMetricQVBox);
   
   //-----------------------------------------------------------------------------
   //
   // nodes with shape
   //
   createNodeSelectionShape();
   queryControlWidgetStack->addWidget(nodesWithShapeQVBox);
   
   //-----------------------------------------------------------------------------
   //
   // nodes with crossovers
   //
   nodesWithCrossoversWidget = new QWidget;
   queryControlWidgetStack->addWidget(nodesWithCrossoversWidget);
   
   //-----------------------------------------------------------------------------
   //
   // Frame used as a separator
   //
   //QT4 CONVERSION Q3GroupBox* separatorFrame = new Q3GroupBox(selectionModeGroupBox);
   //QT4 CONVERSION separatorFrame->setFrameStyle(Q3Frame::HLine);
   //QT4 CONVERSION separatorFrame->setLineWidth(3);
   
   //-----------------------------------------------------------------------------
   //
   // Select Nodes
   //
   selectionModeGroupLayout->addWidget(createNodeSelectionSection());
   
   //-----------------------------------------------------------------------------
   
   //
   // operation surface label and combo box
   //
   QLabel* operationSurfaceLabel = new QLabel("Surface ");
   operationSurfaceComboBox = new GuiBrainModelSelectionComboBox(false,
                                                             true,
                                                             false,
                                                             "",
                                                             0, 
                                                             "operationSurfaceComboBox");
   operationSurfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainSet()->getActiveFiducialSurface());
   operationSurfaceComboBox->setMaximumWidth(maxComboBoxWidth);
   QObject::connect(operationSurfaceComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSurfaceSelection()));
   QHBoxLayout* operationSurfaceLayout = new QHBoxLayout;
   operationSurfaceLayout->addWidget(operationSurfaceLabel);
   operationSurfaceLayout->addWidget(operationSurfaceComboBox);
   //operationSurfaceLayout->setStretchFactor(operationSurfaceLabel, 0);
   //operationSurfaceLayout->setStretchFactor(operationSurfaceComboBox, 100);
   operationSurfaceLayout->addStretch();
   
   //
   // operation topology file
   //
/*
   QLabel* operationTopoLabel = new QLabel("Topology ");
   topologyComboBox = new GuiTopologyFileComboBox(0);
   QObject::connect(topologyComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotTopologySelection()));
   QHBoxLayout* operationTopologyLayout = new QHBoxLayout;
   operationTopologyLayout->addWidget(operationTopoLabel);
   operationTopologyLayout->addWidget(topologyComboBox);
   operationTopologyLayout->setStretchFactor(operationTopoLabel, 0);
   operationTopologyLayout->setStretchFactor(topologyComboBox, 100);
*/
   
   //
   // Group Box for operation surface section
   //
   QGroupBox* operationSurfaceGroupBox = new QGroupBox("Operation Surface and Topology");
   layout->addWidget(operationSurfaceGroupBox);
   QVBoxLayout* operationSurfaceGroupLayout = new QVBoxLayout(operationSurfaceGroupBox);
   operationSurfaceGroupLayout->addLayout(operationSurfaceLayout);
  // operationSurfaceGroupLayout->addLayout(operationTopologyLayout);
   
   //-----------------------------------------------------------------------------
   //
   // Operation combo box
   //
   QLabel* operationLabel = new QLabel("Operation");
   operationComboBox = new QComboBox;
   QObject::connect(operationComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotOperationMode(int)));
   operationComboBox->insertItem(OPERATION_MODE_ASSIGN_METRIC,
                                 "Assign Metric Column for Selected Nodes");
   operationComboBox->insertItem(OPERATION_MODE_ASSIGN_PAINT,
                                 "Assign Paint Attributes to Selected Nodes");
   operationComboBox->insertItem(OPERATION_MODE_ASSIGN_SURFACE_SHAPE,
                                 "Assign Surface Shape Column for Selected Nodes");
   operationComboBox->insertItem(OPERATION_MODE_COMPUTE_INTEGRATED_FOLDING_INDEX,
                                 "Compute Integrated Folding Index");
   operationComboBox->insertItem(OPERATION_MODE_CREATE_BORDERS_FROM_CLUSTERS,
                                 "Create Borders Around Clusters");
   operationComboBox->insertItem(OPERATION_MODE_CREATE_VOLUME_ROI,
                                 "Create Volume ROI from Selected Nodes");
   operationComboBox->insertItem(OPERATION_MODE_DISCONNECT_NODES,
                                 "Disconnect Selected Nodes");
   operationComboBox->insertItem(OPERATION_MODE_GEODESIC,
                                 "Geodesic Distance");
   operationComboBox->insertItem(OPERATION_MODE_PROB_ATLAS_OVERLAP,
                                 "Probabilistic Atlas Overlap Analysis");
   operationComboBox->insertItem(OPERATION_MODE_SMOOTH_NODES,
                                 "Smooth Selected Nodes");
   operationComboBox->insertItem(OPERATION_MODE_STATISTICAL_REPORT,
                                 "Statistical Report on Selected Nodes");
   operationComboBox->insertItem(OPERATION_MODE_STATISTICAL_PAINT_REPORT,
                                 "Statistical Report on Paint Subregions");
   operationComboBox->insertItem(OPERATION_MODE_SHAPE_CORRELATION,
                                 "Surface Shape Correlation Coefficient Report");
   operationComboBox->insertItem(OPERATION_MODE_SHAPE_CLUSTER_ANALYSIS,
                                 "Surface Shape Cluster Analysis");
   QHBoxLayout* operationBoxLayout = new QHBoxLayout;
   operationBoxLayout->addWidget(operationLabel);
   operationBoxLayout->addWidget(operationComboBox);
   //operationBoxLayout->setStretchFactor(operationLabel, 0);
   //operationBoxLayout->setStretchFactor(operationComboBox, 100);
   operationBoxLayout->addStretch();
   
   //
   // Widget stack for different operations
   //
   operationsWidgetStack = new QStackedWidget;
   
   //  
   // create the assign metric operations widget
   //
   createOperationAssignMetric();
   operationsWidgetStack->addWidget(operationAssignMetricWidget);
   
   //
   // create the assign paint operations widget
   //
   createOperationAssignPaint();
   operationsWidgetStack->addWidget(operationAssignPaintWidget);
   
   //  
   // create the assign surface shape operations widget
   //
   createOperationAssignSurfaceShape();
   operationsWidgetStack->addWidget(operationAssignSurfaceShapeWidget);
   
   //
   // create the create borders around clusters widget
   //
   createOperationsBordersAroundClusters();
   operationsWidgetStack->addWidget(operationCreateBordersFromClustersWidget);
   
   //
   // create the create volume roi operations widget
   //
   createOperationCreateVolumeROI();
   operationsWidgetStack->addWidget(operationCreateVolumeRoiWidget);
   
   //
   // create the disconnect nodes operations widget
   //
   createOperationDisconnectNodes();
   operationsWidgetStack->addWidget(operationDisconnectNodesWidget);
   
   //
   // Create the geodesic distance widget
   //
   createOperationGeodesicDistance();
   operationsWidgetStack->addWidget(operationGeodesicWidget);
   
   //
   // Create the integrated folding index widget
   //
   createOperationIntegratedFoldingIndex();
   operationsWidgetStack->addWidget(operationComputeIntegratedFoldingIndexWidget);
   
   //
   //
   // Create the geodesic distance widget
   //
   createOperationProbAtlas();
   operationsWidgetStack->addWidget(operationProbAtlasWidget);
   
   //
   // create the smooth nodes operation widget
   //
   createOperationSmoothNodes();
   operationsWidgetStack->addWidget(operationSmoothNodesWidget);
   
   //
   // create the statistical report operations widget
   //
   createOperationStatisticalReport();
   operationsWidgetStack->addWidget(operationStatisticalReportWidget);   
   
   //
   // create the statistical report on paint subregion operations widget
   //
   createOperationStatisticalPaintReport();
   operationsWidgetStack->addWidget(operationStatisticalPaintReportWidget);   
   
   //
   // create the surface shape correlation coefficient report widget
   //
   createShapeCorrelationCoefficientReport();
   operationsWidgetStack->addWidget(operationShapeCorrelationWidget);

   //
   // create the surface shape cluster report widget
   //
   createShapeClusterReport();
   operationsWidgetStack->addWidget(operationShapeClusterWidget);

   //
   // Group Box for operation section
   //
   QGroupBox* operationGroupBox = new QGroupBox("Operate on Selected Nodes");
   layout->addWidget(operationGroupBox);
   QVBoxLayout* operationGroupLayout = new QVBoxLayout(operationGroupBox);
   operationGroupLayout->addLayout(operationBoxLayout);
   operationGroupLayout->addWidget(operationsWidgetStack);
}

/**
 * create the assign metric operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationAssignMetric()
{
   //
   // Metric column and new metric value
   //
   QLabel* metricColumnLabel = new QLabel("Metric Column ");
   metricColumnAssignComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                                     GUI_NODE_FILE_TYPE_METRIC,
                                                                     true,
                                                                     false,
                                                                     false);
   metricColumnAssignComboBox->setMaximumWidth(maxComboBoxWidth);
   metricColumnAssignNameLineEdit = new QLineEdit;
   metricColumnAssignNameLineEdit->setMinimumWidth(150);
   QObject::connect(metricColumnAssignComboBox, SIGNAL(itemNameSelected(const QString&)),
                    metricColumnAssignNameLineEdit, SLOT(setText(const QString&)));
   metricColumnAssignNameLineEdit->setText(metricColumnAssignComboBox->getCurrentLabel());
   QHBoxLayout* columnLayout = new QHBoxLayout;
   columnLayout->addWidget(metricColumnLabel);
   columnLayout->addWidget(metricColumnAssignComboBox);
   columnLayout->addWidget(metricColumnAssignNameLineEdit);
   columnLayout->setStretchFactor(metricColumnLabel, 0);
   columnLayout->setStretchFactor(metricColumnAssignNameLineEdit, 0);
   //columnLayout->setStretchFactor(metricColumnAssignComboBox, 100);
   columnLayout->addStretch();
   
   //
   // Metric value and assign button
   //
   QLabel* valueLabel = new QLabel("New Value "); 
   metricValueLineEdit = new QLineEdit;
   metricValueLineEdit->setFixedWidth(150);
   metricValueLineEdit->setText("0.0");
   QPushButton* assignMetricPushButton = new QPushButton("Assign Metric");
   assignMetricPushButton->setFixedSize(assignMetricPushButton->sizeHint());
   assignMetricPushButton->setAutoDefault(false);
   QObject::connect(assignMetricPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAssignMetricToNodes()));
   QHBoxLayout* assignLayout = new QHBoxLayout;
   assignLayout->addWidget(valueLabel);
   assignLayout->addWidget(metricValueLineEdit);
   assignLayout->addWidget(new QLabel("  "));
   assignLayout->addWidget(assignMetricPushButton);
   assignLayout->addStretch();

   //
   // layout metric page
   //   
   operationAssignMetricWidget = new QWidget;
   QVBoxLayout* metricLayout = new QVBoxLayout(operationAssignMetricWidget);
   metricLayout->setSpacing(3);
   metricLayout->addLayout(columnLayout);
   metricLayout->addLayout(assignLayout);
}

/**
 * Called to assign metrics for a node.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotAssignMetricToNodes()
{
   if (haveNodesInROI() == false) {
      GuiMessageBox::critical(this, "Error: no nodes in ROI",
                              "There are no nodes in the region of interest.", "OK");
      return;
   }
   
   const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   int metricColumn = metricColumnAssignComboBox->currentIndex();
   if (metricColumn == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
      if (mf->getNumberOfColumns() == 0) {
         mf->setNumberOfNodesAndColumns(numNodes, 1);
      }
      else {
         mf->addColumns(1);
      }
      metricColumn = mf->getNumberOfColumns() - 1;
   }
   
   if ((metricColumn < 0) || (metricColumn >= mf->getNumberOfColumns())) {
      GuiMessageBox::critical(this, "Invalid metric column",
         "An invalid metric column is selected.", "OK");
      return;
   }
      
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // Set column name
   //
   mf->setColumnName(metricColumn, metricColumnAssignNameLineEdit->text());
   
   //
   // Assign the metric index to the nodes
   //
   const float value = metricValueLineEdit->text().toFloat();
   for (int i = 0; i < numNodes; i++) {
      if (nodeInROI[i]) {
         mf->setValue(i, metricColumn, value);
      }
   }

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
   // Save assigned column
   //
   metricColumnAssignComboBox->setCurrentIndex(metricColumn);
   
   QApplication::restoreOverrideCursor();
   
   theMainWindow->speakText("Metric assignments to nodes are complete.", false);
}

/**
 * create the borders around clusters operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationsBordersAroundClusters()
{
   //
   // border to assign
   //
   QPushButton* clusterNamePushButton = new QPushButton("Border Name...");
   clusterNamePushButton->setAutoDefault(false);
   clusterNamePushButton->setFixedSize(clusterNamePushButton->sizeHint());
   QObject::connect(clusterNamePushButton, SIGNAL(clicked()),
                    this, SLOT(slotBorderClusterNamePushButton()));
   clusterBorderNameLineEdit = new QLineEdit;
   clusterBorderNameLineEdit->setFixedWidth(250);
   QHBoxLayout* borderNameLayout = new QHBoxLayout;
   borderNameLayout->addWidget(clusterNamePushButton);
   borderNameLayout->addWidget(clusterBorderNameLineEdit);
   borderNameLayout->addStretch();
   
   //
   // create borders push button
   //
   QPushButton* createBordersPushButton = new QPushButton("Create Borders Around Clusters");
   createBordersPushButton->setFixedSize(createBordersPushButton->sizeHint());
   createBordersPushButton->setAutoDefault(false);
   QObject::connect(createBordersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateBordersFromClusters()));

   operationCreateBordersFromClustersWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationCreateBordersFromClustersWidget);
   layout->setSpacing(3);
   layout->addLayout(borderNameLayout);
   layout->addWidget(createBordersPushButton);
   layout->addStretch();
}

/**
 * Called to set create borders from clusters name.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotBorderClusterNamePushButton()
{
   GuiNameSelectionDialog nsd(this);
   if (nsd.exec() == QDialog::Accepted) {
      clusterBorderNameLineEdit->setText(nsd.getName());
   }
}

/**
 * Called run create borders from clusters.
 */ 
void 
GuiSurfaceRegionOfInterestDialog::slotCreateBordersFromClusters()
{
   const QString borderName = clusterBorderNameLineEdit->text();
   if (borderName.isEmpty()) {
      GuiMessageBox::critical(this, "ERROR", "Please enter a border name.");
      return;
   }
   
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      GuiMessageBox::critical(this, "ERROR", "Operation surface is invalid.");
      return;
   }
   //TopologyFile* topologyFile =  topologyComboBox->getSelectedTopologyFile();
   TopologyFile* topologyFile =  bms->getTopologyFile();
   if (topologyFile == NULL) {
      GuiMessageBox::critical(this, "ERROR", "Operation topology is invalid.");
      return;
   }
   int numberOfBordersCreated = 0;
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   try {
      BrainModelSurfaceClusterToBorderConverter scbc(theMainWindow->getBrainSet(),
                                                     bms,
                                                     topologyFile,
                                                     borderName,
                                                     nodeInROI);
      scbc.execute();
      numberOfBordersCreated = scbc.getNumberOfBordersCreated();
   }
   catch (BrainModelAlgorithmException& e) {
      GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
      return;
   }
   QApplication::restoreOverrideCursor();
   
   if (numberOfBordersCreated > 0) {
      
      std::ostringstream str;
      str << numberOfBordersCreated
          << " border(s) were created\n";
          
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
         GuiMessageBox::information(this, "INFO", str.str().c_str(), "OK");
         createBorderColor = false;
      }
      else if ((borderColorIndex >= 0) && (borderColorMatch == false)) {
         str << "You may use border color \""
             << borderColorFile->getColorNameByIndex(borderColorIndex).toAscii().constData()
             << "\" for border "
             << borderName.toAscii().constData()
             << "\"\n or define a new color.";
         QString b2("Use color ");
         b2.append(borderColorFile->getColorNameByIndex(borderColorIndex));
         QString b1("Define color ");
         b1.append(borderName);
         const int buttonPressed = GuiMessageBox::question(this, "INFO", str.str().c_str(),
                                    b1, b2); 
         if (buttonPressed == 0) {
            createBorderColor = true;
         }
      }
      else {
         str << "There is no matching color for "
             << borderName.toAscii().constData();
         QString b1("Define color ");
         b1.append(borderName);
         const int buttonPressed = GuiMessageBox::question(this, "INFO", str.str().c_str(),
                                     b1, "Cancel");
         if (buttonPressed == 0) {
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
      
      theMainWindow->getBrainSet()->assignBorderColors();
      GuiFilesModified fm;
      fm.setBorderColorModified();
      fm.setBorderModified();
      theMainWindow->fileModificationUpdate(fm);
   }
   else {
      GuiMessageBox::information(this, "INFO", "No clusters were found.", "OK");
   }
   
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * create the assign surface shape operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationAssignSurfaceShape()
{
   //
   // SurfaceShape column and new surface shape value
   //
   QLabel* surfaceShapeColumnLabel = new QLabel("Surface Shape Column ");
   surfaceShapeColumnAssignComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                                     GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                                                     true,
                                                                     false,
                                                                     false);
   surfaceShapeColumnAssignComboBox->setMaximumWidth(maxComboBoxWidth);
   surfaceShapeColumnAssignNameLineEdit = new QLineEdit;
   surfaceShapeColumnAssignNameLineEdit->setMinimumWidth(150);
   QObject::connect(surfaceShapeColumnAssignComboBox, SIGNAL(itemNameSelected(const QString&)),
                    surfaceShapeColumnAssignNameLineEdit, SLOT(setText(const QString&)));
   surfaceShapeColumnAssignNameLineEdit->setText(surfaceShapeColumnAssignComboBox->getCurrentLabel());
   QHBoxLayout* columnLayout = new QHBoxLayout;
   columnLayout->addWidget(surfaceShapeColumnLabel);
   columnLayout->addWidget(surfaceShapeColumnAssignComboBox);
   columnLayout->addWidget(surfaceShapeColumnAssignNameLineEdit);
   columnLayout->setStretchFactor(surfaceShapeColumnLabel, 0);
   //columnLayout->setStretchFactor(surfaceShapeColumnAssignComboBox, 100);
   columnLayout->setStretchFactor(surfaceShapeColumnAssignNameLineEdit, 0);
   columnLayout->addStretch();
   
   //
   // Value to assign
   //
   QLabel* valueLabel = new QLabel("New Value ");
   surfaceShapeValueLineEdit = new QLineEdit;
   surfaceShapeValueLineEdit->setFixedWidth(150);
   surfaceShapeValueLineEdit->setText("0.0");
   
   //
   // Assign surfaceShape push button
   //
   QPushButton* assignSurfaceShapePushButton = new QPushButton("Assign Surface Shape");
   assignSurfaceShapePushButton->setFixedSize(assignSurfaceShapePushButton->sizeHint());
   assignSurfaceShapePushButton->setAutoDefault(false);
   QObject::connect(assignSurfaceShapePushButton, SIGNAL(clicked()),
                    this, SLOT(slotAssignSurfaceShapeToNodes()));
   
   //
   // assign layout
   //
   QHBoxLayout* assignLayout = new QHBoxLayout;
   assignLayout->addWidget(valueLabel);
   assignLayout->addWidget(surfaceShapeValueLineEdit);
   assignLayout->addWidget(new QLabel(" "));
   assignLayout->addWidget(assignSurfaceShapePushButton);
   assignLayout->addStretch();

   //
   // layout for page
   //                 
   operationAssignSurfaceShapeWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationAssignSurfaceShapeWidget);
   layout->setSpacing(3);
   layout->addLayout(columnLayout);
   layout->addLayout(assignLayout);
}

/**
 * Called to assign surfaceShape for a node.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotAssignSurfaceShapeToNodes()
{  
   if (haveNodesInROI() == false) {
      GuiMessageBox::critical(this, "Error: no nodes in ROI",
                              "There are no nodes in the region of interest.", "OK");
      return;
   }
   
   const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   int surfaceShapeColumn = surfaceShapeColumnAssignComboBox->currentIndex();
   if (surfaceShapeColumn == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
      if (ssf->getNumberOfColumns() == 0) {
         ssf->setNumberOfNodesAndColumns(numNodes, 1);
      }
      else {
         ssf->addColumns(1);
      }
      surfaceShapeColumn = ssf->getNumberOfColumns() - 1;
   }
   
   if ((surfaceShapeColumn < 0) || (surfaceShapeColumn >= ssf->getNumberOfColumns())) {
      GuiMessageBox::critical(this, "Invalid surface shape column",
         "An invalid surface shape column is selected.", "OK");
      return;
   }
      
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // Assign the surface shape index to the nodes
   //
   const float value = surfaceShapeValueLineEdit->text().toFloat();
   for (int i = 0; i < numNodes; i++) {
      if (nodeInROI[i]) {
         ssf->setValue(i, surfaceShapeColumn, value);
      }
   }

   //
   // SurfaceShape File has changed
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
   // Save assigned column
   //
   surfaceShapeColumnAssignComboBox->setCurrentIndex(surfaceShapeColumn);
   
   QApplication::restoreOverrideCursor();
   
   theMainWindow->speakText("Surface Shape assignments to nodes are complete.", false);
}      

/**
 * create the assign paint operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationAssignPaint()
{
   //
   // Paint column and new paint column name
   //
   QLabel* paintColumnLabel = new QLabel("Paint Column ");
   paintColumnAssignComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                                     GUI_NODE_FILE_TYPE_PAINT,
                                                                     true,
                                                                     false,
                                                                     false);
   QObject::connect(paintColumnAssignComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotAssignPaintColumn(int)));
   paintColumnAssignComboBox->setMaximumWidth(maxComboBoxWidth);
   paintColumnAssignNameLineEdit = new QLineEdit;
   paintColumnAssignNameLineEdit->setMinimumWidth(150);
   
   //
   // layout for column widgets
   //
   QHBoxLayout* columnLayout = new QHBoxLayout;
   columnLayout->addWidget(paintColumnLabel);
   columnLayout->addWidget(paintColumnAssignComboBox);
   columnLayout->addWidget(paintColumnAssignNameLineEdit);
   columnLayout->setStretchFactor(paintColumnLabel, 0);
   //columnLayout->setStretchFactor(paintColumnAssignComboBox, 100);
   columnLayout->setStretchFactor(paintColumnAssignNameLineEdit, 0);
   columnLayout->addStretch();
   slotAssignPaintColumn(paintColumnAssignComboBox->currentIndex());
   
   //
   // Paint name for assignment
   //
   QLabel* paintNameLabel = new QLabel("Paint Name ");
   paintAssignNameLineEdit = new QLineEdit;
   paintAssignNameLineEdit->setMinimumWidth(250);
   
   //
   // Assignment layout
   //
   QHBoxLayout* assignLayout = new QHBoxLayout;
   assignLayout->addWidget(paintNameLabel);
   assignLayout->addWidget(paintAssignNameLineEdit);
   //assignLayout->setStretchFactor(paintNameLabel, 0);
   //assignLayout->setStretchFactor(paintAssignNameLineEdit, 100);
   assignLayout->addStretch();
   
   //
   // Assign paint push button
   //
   QPushButton* assignPaintPushButton = new QPushButton("Assign Paint");
   assignPaintPushButton->setFixedSize(assignPaintPushButton->sizeHint());
   assignPaintPushButton->setAutoDefault(false);
   QObject::connect(assignPaintPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAssignPaintToNodes()));

   operationAssignPaintWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationAssignPaintWidget);
   layout->setSpacing(3);
   layout->addLayout(columnLayout);
   layout->addLayout(assignLayout);
   layout->addWidget(assignPaintPushButton);
}

/**
 * Called to assign paint IDs to nodes.
 */
void
GuiSurfaceRegionOfInterestDialog::slotAssignPaintToNodes()
{
   if (haveNodesInROI() == false) {
      GuiMessageBox::critical(this, "Error: no nodes in ROI",
                              "There are no nodes in the region of interest.", "OK");
      return;
   }
   
   const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   int paintColumn = paintColumnAssignComboBox->currentIndex();
   if (paintColumn == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
      if (pf->getNumberOfColumns() == 0) {
         pf->setNumberOfNodesAndColumns(numNodes, 1);
      }
      else {
         pf->addColumns(1);
      }
      paintColumn = pf->getNumberOfColumns() - 1;
   }
   
   if ((paintColumn < 0) || (paintColumn >= pf->getNumberOfColumns())) {
      GuiMessageBox::critical(this, "Invalid paint column",
         "An invalid paint column is selected.", "OK");
      return;
   }
   
   const QString paintName(paintAssignNameLineEdit->text());
   if (paintName.isEmpty()) {
      GuiMessageBox::critical(this, "No Paint Name",
         "The name for the paint is empty.", "OK");
      return;
   }
   
   //
   // Set the name of the paint column
   //
   pf->setColumnName(paintColumn, paintColumnAssignNameLineEdit->text());
   
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
      if (GuiMessageBox::information(this, "Use Partially Matching Color",
                                   msg, "Yes", noButton, QString::null, 0) != 0) {
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
   
   //
   // add or get index of paint name
   //
   const int paintIndex = pf->addPaintName(paintName);
   
   //
   // Assign the paint index to the nodes
   //
   for (int i = 0; i < numNodes; i++) {
      if (nodeInROI[i]) {
         pf->setPaint(i, paintColumn, paintIndex);
      }
   }

   //
   // Node Color File has changed
   //
   GuiFilesModified fm;
   fm.setPaintModified();
   if (createAreaColor) {
      fm.setAreaColorModified();
   }
   theMainWindow->fileModificationUpdate(fm);

   //
   // Update node colors and redraw
   //
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL); 
   
   //
   // Save assigned column
   //
   paintColumnAssignComboBox->setCurrentIndex(paintColumn);
   slotAssignPaintColumn(paintColumn);
   
   QApplication::restoreOverrideCursor();
   
   theMainWindow->speakText("Paint assignments to nodes are complete.", false);
}

/**
 * Called when an assign paint column is selected.
 */
void
GuiSurfaceRegionOfInterestDialog::slotAssignPaintColumn(int item)
{
   paintColumnAssignNameLineEdit->setText("New Column Name");
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   if ((item >= 0) && (item < pf->getNumberOfColumns())) {
      paintColumnAssignNameLineEdit->setText(pf->getColumnName(item));
   }
}

/**
 * create the create volume ROI operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationCreateVolumeROI()
{
   QPushButton* createVolumeFromQueryNodesPushButton = new QPushButton(
                                                      "Create Volume From Displayed Query Nodes...");
   createVolumeFromQueryNodesPushButton->setAutoDefault(false);
   createVolumeFromQueryNodesPushButton->setFixedSize(createVolumeFromQueryNodesPushButton->sizeHint());
   QObject::connect(createVolumeFromQueryNodesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateVolumeFromQueryNodesButton()));

   operationCreateVolumeRoiWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationCreateVolumeRoiWidget);
   layout->addWidget(createVolumeFromQueryNodesPushButton);
}

/**
 * create the prob atlas operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationProbAtlas()
{
   QPushButton* createReportPushButton = new QPushButton("Create Report");
   createReportPushButton->setAutoDefault(false);
   createReportPushButton->setFixedSize(createReportPushButton->sizeHint());
   QObject::connect(createReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateProbAtlasReport()));
                    
   probAtlasTabSeparateCheckBox = new QCheckBox("Separate Report With Semicolons (for import to spreadsheet)");

   operationProbAtlasWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationProbAtlasWidget);;
   layout->addWidget(createReportPushButton);
   layout->addWidget(probAtlasTabSeparateCheckBox);
}

/**
 * Called to create the prob atlas report.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotCreateProbAtlasReport()
{
   ProbabilisticAtlasFile* pf = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   
   const int numNodes = pf->getNumberOfNodes();
   const int numCols  = pf->getNumberOfColumns();
   
   if ((numNodes <= 0) || (numCols <= 0)) {
      GuiMessageBox::critical(this, "ERROR", "The Probabilistic Atlas File is Empty.", "OK");
      return;
   }
   
   const int numNames = pf->getNumberOfPaintNames();
   if (numNames <= 0) {
      GuiMessageBox::critical(this, "ERROR", "No names in Probabilistic Atlas File.", "OK");
   }

   float roiArea = 0.0;
   if (createReportHeader("Probabilistic Atlas Analysis", false, roiArea)) {
      return;
   }

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   QString blankTab(" ");
   const bool useTabs = probAtlasTabSeparateCheckBox->isChecked();
   if (useTabs) {
      blankTab = separatorCharacter;
   }

   const int numNodesSelected = std::count(nodeInROI.begin(), nodeInROI.end(), true);
   
   const int countCols = numCols + 1;
   int* counts = new int[countCols];

   //
   // Sort the paint names
   //
   std::vector<int> indices;
   std::vector<QString> names;
   for (int i = 0; i < numNames; i++) {
      indices.push_back(i);
      names.push_back(pf->getPaintNameFromIndex(i));
   }
   NameIndexSort sortNames(indices, names);
   
   if (useTabs) {
      std::ostringstream str;
      str << "Name ";
      for (int j = 0; j < countCols; j++) {
         str << blankTab.toAscii().constData() << j;
      }
      reportTextEdit->append(str.str().c_str());
   }
   
   //
   // Loop through paint names
   //      
   for (int m = 0; m < numNames; m++) {
      //
      // Get the paint name and index
      //
      int indx = -1;
      QString name;
      sortNames.getNameAndIndex(m, indx, name);
      
      //
      // Clear counts
      //
      for (int j = 0; j < countCols; j++) {
         counts[j] = 0;
      }
      
      //
      // Find nodes using paint
      //
      for (int i = 0; i < numNodes; i++) {
         if (nodeInROI[i]) {
            int cnt = 0;
            for (int j = 0; j < numCols; j++) {
               if (pf->getPaint(i, j) == indx) {
                  cnt++;
               }
            }
            counts[cnt]++;
         }
      }
      
      std::ostringstream str;
      str.precision(2);
      
      if (useTabs) {
         str << name.toAscii().constData();
         for (int j = 0; j < countCols; j++) {
            str << blankTab.toAscii().constData() << counts[j];
         }
      }
      else {
         reportTextEdit->append(name);
         str << blankTab.toAscii().constData();
         for (int j = 0; j < countCols; j++) {
            str << counts[j] << " with " << j << " entries ";
         }
      }
      reportTextEdit->append(str.str().c_str());
      
      str.str("");
      str.setf(std::ios::fixed);
      if (useTabs) {
         str << " ";
         for (int j = 0; j < countCols; j++) {
            const float pct = (static_cast<float>(counts[j]) /
                               static_cast<float>(numNodesSelected)) * 100.0;
            str << blankTab.toAscii().constData() << pct;
         }
      }
      else {
         str << blankTab.toAscii().constData();
         for (int j = 0; j < countCols; j++) {
            const float pct = (static_cast<float>(counts[j]) /
                               static_cast<float>(numNodesSelected)) * 100.0;
            str << pct << "% with " << j << " entries ";
         }
      }
      reportTextEdit->append(str.str().c_str());
      
      reportTextEdit->append(" ");
   }
   
   delete[] counts;
/*
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   std::vector<float> coefficients;
   const int column = shapeCorrelationColumnComboBox->currentIndex();
   if (column == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_ALL) {
      const int lastColumn = ssf->getNumberOfColumns() - 1;
      std::ostringstream str;
      for (int j = 0; j < ssf->getNumberOfColumns(); j++) {
         str << ssf->getColumnName(j);
         if (j != lastColumn) {
             str << blankTab;
         }
      }
      reportTextEdit->append(str.str().c_str());
      
      for (int j = 0; j < ssf->getNumberOfColumns(); j++) {
         ssf->correlationCoefficient(j,
                                     coefficients,
                                     &nodeInROI);
         std::ostringstream str;
         str.setf(std::ios::fixed);
         for (int i = 0; i < ssf->getNumberOfColumns(); i++) {
            str << coefficients[i];
            if (i != lastColumn) {
               str << blankTab;
            }
         }
         reportTextEdit->append(str.str().c_str());
      }
   }
   else {
      ssf->correlationCoefficient(column,
                                  coefficients,
                                  &nodeInROI);

      std::ostringstream str;
      str.setf(std::ios::fixed);
      str << "\n"
          << "Correlation for "
          << ssf->getColumnName(column);
      reportTextEdit->append(str.str().c_str());
      
      for (int i = 0; i < ssf->getNumberOfColumns(); i++) {
         str.str("");
         str << (i + 1) 
             << blankTab
             << ssf->getColumnName(i)
             << blankTab
             << coefficients[i];
         reportTextEdit->append(str.str().c_str());
      }
   }
*/
   
   //
   // Swith to report page
   //
   tabWidget->setCurrentWidget(reportPage);
   
   QApplication::beep();
   
   QApplication::restoreOverrideCursor();
   
   theMainWindow->speakText("The report is ready.", false);

}      

/**
 * create the disconnect nodes operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationDisconnectNodes()
{
   QPushButton* disconnectButton = new QPushButton("Disconnect Selected Nodes");
   disconnectButton->setAutoDefault(false);
   disconnectButton->setFixedSize(disconnectButton->sizeHint());
   QObject::connect(disconnectButton, SIGNAL(clicked()),
                    this, SLOT(slotDisconnectNodes()));

   operationDisconnectNodesWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationDisconnectNodesWidget);
   layout->addWidget(disconnectButton);
}

/**
 * create the surface shape correlation coefficient section.
 */
void 
GuiSurfaceRegionOfInterestDialog::createShapeClusterReport()
{
   QLabel* threshLabel = new QLabel("Threshold ");
   shapeClusterThresholdDoubleSpinBox = new QDoubleSpinBox;
   shapeClusterThresholdDoubleSpinBox->setMinimum(-10000.0);
   shapeClusterThresholdDoubleSpinBox->setMaximum( 10000.0);
   shapeClusterThresholdDoubleSpinBox->setSingleStep(1.0);
   shapeClusterThresholdDoubleSpinBox->setDecimals(3);
   QHBoxLayout* threshLayout = new QHBoxLayout;
   threshLayout->addWidget(threshLabel);
   threshLayout->addWidget(shapeClusterThresholdDoubleSpinBox);
   threshLayout->addStretch();
   
   shapeClusterTabSeparateCheckBox = new QCheckBox("Separate Report With Semicolons");
   
   QPushButton* createClusterReportPushButton = new QPushButton("Create Cluster Report");
   createClusterReportPushButton->setFixedSize(createClusterReportPushButton->sizeHint());
   createClusterReportPushButton->setAutoDefault(false);
   QObject::connect(createClusterReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotShapeClusterReport()));

   operationShapeClusterWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationShapeClusterWidget);
   layout->addLayout(threshLayout);
   layout->addWidget(shapeClusterTabSeparateCheckBox);
   layout->addWidget(createClusterReportPushButton);
}

/**
 * Called to create shape correlation report.
 */
void
GuiSurfaceRegionOfInterestDialog::slotShapeClusterReport()
{
   if (haveNodesInROI() == false) {
      GuiMessageBox::critical(this, "Error: no nodes in ROI",
                            "There are no nodes in the region of interest.", "OK");
      return;
   }

   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   const int numNodes = ssf->getNumberOfNodes();
   if (numNodes <= 0) {
      GuiMessageBox::critical(this, "ERROR", "The surface shape file is empty.", "OK");
      return;
   }
   
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      GuiMessageBox::critical(this, "ERROR", "There is no selected surface.", "OK");
      return;
   }

   //
   // Metric column for area correction
   //
   int metricColumn = -1;
   const MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   if (mf->empty() == false) {
      DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
      if ((dsm->getSelectedDisplayColumn(0) >= 0) &&
          (dsm->getSelectedDisplayColumn(0) < mf->getNumberOfColumns())) {
         metricColumn = dsm->getSelectedDisplayColumn(0);
         QString msg("Areal Distortion Correction using metric column:\n   ");
         msg.append(mf->getColumnName(metricColumn));
         if (GuiMessageBox::information(this, "INFO", msg, "Continue", "Cancel") != 0) {
            return;
         }
      }
   }
   else {
      const QString msg("No metric column for distortion correction is loaded.");
      if (GuiMessageBox::information(this, "INFO", msg, "Continue", "Cancel") != 0) {
         return;
      }
   }
   
   float roiArea = 0.0;
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   if (createReportHeader("", shapeClusterTabSeparateCheckBox->isChecked(), roiArea)) {
      return;
   }
   std::ostringstream str1;
   str1 << "Surface Shape File: " << FileUtilities::basename(ssf->getFileName()).toAscii().constData() << "\n";
   if ((mf != NULL ) && (metricColumn >= 0)) {
      str1 << "Metric File: " << FileUtilities::basename(mf->getFileName()).toAscii().constData() << "\n"
           << "Metric Column " << mf->getColumnName(metricColumn).toAscii().constData() << "\n";
   }
   str1 << "\n";
   reportTextEdit->append(str1.str().c_str());
   
   QString blankTab(" ");
   if (shapeClusterTabSeparateCheckBox->isChecked()) {
      blankTab = separatorCharacter;
   }

   //
   // Report header
   //
   std::ostringstream str;
   str << "Threshold" << blankTab.toAscii().constData()
       << "Column" << blankTab.toAscii().constData()
       <<  "Num-Nodes" << blankTab.toAscii().constData()
       <<  "Area " << blankTab.toAscii().constData()
       <<  "Area Corrected" << blankTab.toAscii().constData()
       <<  "COG-X" << blankTab.toAscii().constData()
       <<  "COG-Y" << blankTab.toAscii().constData()
       <<  "COG-Z";
   reportTextEdit->append(str.str().c_str());
   
   float posMin = 0.0;
   float posMax = 0.0;
   float negMin = 0.0;
   float negMax = 0.0;
   
   const float thresh = shapeClusterThresholdDoubleSpinBox->value();
   if (thresh >= 0.0) {
      posMin = thresh;
      posMax = std::numeric_limits<float>::max();
      negMin = -1.0;
      negMax =  0.0;
   }
   else {
      posMin =  1.0;
      posMax =  0.0;
      negMin =  thresh;
      negMax = -std::numeric_limits<float>::max();
   }
   
   //
   // Process each column
   //
   const int numColumns = ssf->getNumberOfColumns();
   for (int i = 0; i < numColumns; i++) {
      //
      // Ignore deselected shape columns
      //
      if (i < static_cast<int>(surfaceShapeCheckBoxes.size())) {
         if (surfaceShapeCheckBoxes[i]->isChecked() == false) {
            continue;
         }
      }
      
      if (DebugControl::getDebugOn()) {
         std::cout << "Cluster analysis for column: " << i << std::endl;
      }
         
      //
      // Create a shape file containing just the single column
      //
      SurfaceShapeFile tempShapeFile;
      tempShapeFile.setNumberOfNodesAndColumns(numNodes, 1);
      std::vector<float> nodeValues;
      ssf->getColumnForAllNodes(i, nodeValues);
      tempShapeFile.setColumnForAllNodes(0, nodeValues);
      
      //
      // Find the clusters
      //
      BrainModelSurfaceMetricClustering bmsmc(theMainWindow->getBrainSet(),
                                              bms,
                                              &tempShapeFile,
                                              BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA,
                                              0,
                                              0,
                                              "cluster",
                                              1,
                                              0.001,
                                              negMin,
                                              negMax,
                                              posMin,
                                              posMax);
      try {
         bmsmc.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
         return;
      }
      
      //
      // Process the clusters
      //
      const int numClusters = bmsmc.getNumberOfClusters();
      for (int j = 0; j < numClusters; j++) {
         const BrainModelSurfaceMetricClustering::Cluster* cluster = bmsmc.getCluster(j);
         const int numNodesInCluster = cluster->getNumberOfNodesInCluster();

         //
         // Corrected area is sum of each node's area multiplied by
         // 2 to the power of the selected metric column for the node
         //
         float correctedArea = 0.0;
         if (metricColumn >= 0) {
            for (int k = 0; k < numNodesInCluster; k++) {
               const int nodeNum = cluster->getNodeInCluster(k);
               float nodeArea = bmsmc.getNodeArea(nodeNum);
               const double metric = mf->getValue(nodeNum, metricColumn);
               correctedArea += (nodeArea * std::pow(2.0, metric));
            }
         }
         
         //
         // Add to the report
         //
         float cog[3];
         cluster->getCenterOfGravity(cog);
         std::ostringstream str;
         str << thresh << blankTab.toAscii().constData()
             << (i + 1) << blankTab.toAscii().constData()
             << numNodesInCluster << blankTab.toAscii().constData()
             << cluster->getArea() << blankTab.toAscii().constData()
             << correctedArea << blankTab.toAscii().constData()
             << cog[0] << blankTab.toAscii().constData()
             << cog[1] << blankTab.toAscii().constData()
             << cog[2];
         reportTextEdit->append(str.str().c_str());
      }
   }
   
   //
   // Swith to report page
   //
   tabWidget->setCurrentWidget(reportPage);
   
   QApplication::beep();
   
   QApplication::restoreOverrideCursor();
   
   theMainWindow->speakText("The report is ready.", false);
}

/**
 * create the surface shape correlation coefficient section.
 */
void 
GuiSurfaceRegionOfInterestDialog::createShapeCorrelationCoefficientReport()
{
   QLabel* shapeLabel = new QLabel("Reference Column ");
   shapeCorrelationColumnComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(
                                              GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                              false, 
                                              false,
                                              true);
   shapeCorrelationColumnComboBox->setMaximumWidth(maxComboBoxWidth);
   shapeCorrelationColumnComboBox->setCurrentIndex(
                       GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_ALL);
   QHBoxLayout* columnLayout = new QHBoxLayout;
   columnLayout->addWidget(shapeLabel);  
   columnLayout->addWidget(shapeCorrelationColumnComboBox);  
   //columnLayout->setStretchFactor(shapeLabel, 0);  
   //columnLayout->setStretchFactor(shapeCorrelationColumnComboBox, 100);  
   columnLayout->addStretch();
   
   shapeCorrelationTabSeparateCheckBox = new QCheckBox("Separate Report With Semicolons");
   
   QPushButton* createReportPushButton = new QPushButton("Create Correlation Report");
   createReportPushButton->setFixedSize(createReportPushButton->sizeHint());
   createReportPushButton->setAutoDefault(false);
   QObject::connect(createReportPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCorrelationShapeReport()));

   operationShapeCorrelationWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationShapeCorrelationWidget);
   layout->addLayout(columnLayout);
   layout->addWidget(shapeCorrelationTabSeparateCheckBox);
   layout->addWidget(createReportPushButton);
}

/**
 * Called to create shape correlation report.
 */
void
GuiSurfaceRegionOfInterestDialog::slotCorrelationShapeReport()
{
   float roiArea = 0.0;
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   if (createReportHeader("", shapeCorrelationTabSeparateCheckBox->isChecked(), roiArea)) {
      return;
   }
   
   QString blankTab(" ");
   if (shapeCorrelationTabSeparateCheckBox->isChecked()) {
      blankTab = separatorCharacter;
   }

   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   std::vector<float> coefficients;
   const int column = shapeCorrelationColumnComboBox->currentIndex();
   if (column == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_ALL) {
      const int lastColumn = ssf->getNumberOfColumns() - 1;
      std::ostringstream str;
      for (int j = 0; j < ssf->getNumberOfColumns(); j++) {
         str << ssf->getColumnName(j).toAscii().constData();
         if (j != lastColumn) {
             str << blankTab.toAscii().constData();
         }
      }
      reportTextEdit->append(str.str().c_str());
      
      for (int j = 0; j < ssf->getNumberOfColumns(); j++) {
         ssf->correlationCoefficient(j,
                                     coefficients,
                                     &nodeInROI);
         std::ostringstream str;
         str.setf(std::ios::fixed);
         for (int i = 0; i < ssf->getNumberOfColumns(); i++) {
            str << coefficients[i];
            if (i != lastColumn) {
               str << blankTab.toAscii().constData();
            }
         }
         reportTextEdit->append(str.str().c_str());
      }
   }
   else {
      ssf->correlationCoefficient(column,
                                  coefficients,
                                  &nodeInROI);

      std::ostringstream str;
      str.setf(std::ios::fixed);
      str << "\n"
          << "Correlation for "
          << ssf->getColumnName(column).toAscii().constData();
      reportTextEdit->append(str.str().c_str());
      
      for (int i = 0; i < ssf->getNumberOfColumns(); i++) {
         str.str("");
         str << (i + 1) 
             << blankTab.toAscii().constData()
             << ssf->getColumnName(i).toAscii().constData()
             << blankTab.toAscii().constData()
             << coefficients[i];
         reportTextEdit->append(str.str().c_str());
      }
   }
   
   //
   // Swith to report page
   //
   tabWidget->setCurrentWidget(reportPage);
   
   QApplication::beep();
   
   QApplication::restoreOverrideCursor();
   
   theMainWindow->speakText("The report is ready.", false);
}

/**
 * Called to disconnect the selected nodes.
 */
void
GuiSurfaceRegionOfInterestDialog::slotDisconnectNodes()
{
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      //TopologyFile* tf = topologyComboBox->getSelectedTopologyFile(); //bms->getTopologyFile();
      TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
         theMainWindow->getBrainSet()->disconnectNodes(tf, nodeInROI);
         GuiBrainModelOpenGL::updateAllGL(NULL); 
         theMainWindow->speakText("Nodes have been disconnected.", false);
      }
   }
}

/**
 * set node for geodesic query.
 */
void 
GuiSurfaceRegionOfInterestDialog::setNodeForGeodesicQuery(const int nodeNumber)
{
   geodesicNodeSpinBox->setValue(nodeNumber);
}

/**
 * called to default geodesic column names.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotUpdateGeodesicColumnNames()
{
   const int nodeNumber = geodesicNodeSpinBox->value();
   QString newName("Node ");
   newName.append(StringUtilities::fromNumber(nodeNumber));
   
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
 * create the integrated folding index widget
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationIntegratedFoldingIndex()
{
   QPushButton* computePushButton = new QPushButton("Compute Integrated Folding Index");
   computePushButton->setFixedSize(computePushButton->sizeHint());
   computePushButton->setAutoDefault(false);
   QObject::connect(computePushButton, SIGNAL(clicked()),
                    this, SLOT(slotComputeIntegratedFoldingIndex()));
                    
   operationComputeIntegratedFoldingIndexWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationComputeIntegratedFoldingIndexWidget);
   layout->addWidget(computePushButton);
}

/**
 * called to run compute integrated folding index.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotComputeIntegratedFoldingIndex()
{
   if (haveNodesInROI() == false) {
      GuiMessageBox::critical(this, "Error: no nodes in ROI",
                            "There are no nodes in the region of interest.", "OK");
      return;
   }
   
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   const int numColumns = ssf->getNumberOfColumns();
   if (numColumns <= 0) {
      GuiMessageBox::critical(this, "ERROR", "No surface shape columns are selected.", "OK");
      return;
   }

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   float roiArea = 0.0;
   if (createReportHeader("Integrated Folding Index", false, roiArea)) {
      return;
   }
   
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
         std::vector<float> areaTimesFoldingSum(numColumns, 0.0);
         
         const int numTriangles = tf->getNumberOfTiles();
         for (int i = 0; i < numTriangles; i++) {
            if (tileInROI[i]) {
               int n1, n2, n3;
               tf->getTile(i, n1, n2, n3);
               
               for (int j = 0; j < numColumns; j++) {
                  if (surfaceShapeCheckBoxes[j]->isChecked()) {
                  
                     //
                     // Average folding index is:
                     //    SUM(Ai * Abs(Fi)) / SUM(Ai);
                     //
                     //       Ai = area of triangle (less if not all nodes in ROI)
                     //       Fi = average shape value of triangle's three nodes
                     //       SUM(Ai) is the same as the ROI's area
                     //
                     float shapeSum = 0.0;
                     float numInROI = 0.0;
                     if (nodeInROI[n1]) {
                        shapeSum += ssf->getValue(n1, j);
                        numInROI += 1.0;
                     }
                     if (nodeInROI[n2]) {
                        shapeSum += ssf->getValue(n2, j);
                        numInROI += 1.0;
                     }
                     if (nodeInROI[n3]) {
                        shapeSum += ssf->getValue(n3, j);
                        numInROI += 1.0;
                     }
                     if (numInROI > 0.0) {
                        const float averageShape = std::fabs(shapeSum) / numInROI;
                        areaTimesFoldingSum[j] += averageShape * tileArea[i];
                     }
                  }
               }
            }
         }
         
         int longestColumnNameLength = 10;
         for (int j = 0; j < numColumns; j++) {
            if (surfaceShapeCheckBoxes[j]->isChecked()) {
               longestColumnNameLength = std::max(longestColumnNameLength,
                                            static_cast<int>(ssf->getColumnName(j).length()));
            }
         }
         longestColumnNameLength += 5;
         
         QString s = StringUtilities::leftJustify("Name", longestColumnNameLength)
                     + StringUtilities::rightJustify("IFI", 12);
         reportTextEdit->append(s);
         
         for (int j = 0; j < numColumns; j++) {
            if (surfaceShapeCheckBoxes[j]->isChecked()) {
               const float integratedFloatingIndex = areaTimesFoldingSum[j] / roiArea;
               
               QString s = StringUtilities::leftJustify(ssf->getColumnName(j), longestColumnNameLength)
                           + StringUtilities::rightJustify(QString::number(integratedFloatingIndex, 'f', 6), 12);
               reportTextEdit->append(s);
            }
         }
      }
   }
   
   //
   // Switch to report page
   //
   tabWidget->setCurrentWidget(reportPage);
   
   QApplication::beep();

   QApplication::restoreOverrideCursor();
}
      
/**
 * create the disconnect nodes operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationGeodesicDistance()
{
   QLabel* nodeLabel = new QLabel("Query Node");
   geodesicNodeSpinBox = new QSpinBox;
   geodesicNodeSpinBox->setMinimum(0);
   geodesicNodeSpinBox->setMaximum(100000000);
   geodesicNodeSpinBox->setSingleStep(1);
   QObject::connect(geodesicNodeSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotUpdateGeodesicColumnNames()));
   QPushButton* geodesicNodeButton = new QPushButton("Choose Node With Mouse");
   geodesicNodeSpinBox->setValue(-1);
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
                    

   operationGeodesicWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationGeodesicWidget);
   layout->addLayout(nodeLayout);
   layout->addLayout(fileLayout);
   layout->addWidget(geodesicButton);
}

/**
 * Called to change the mouse mode to select the node for the geodesic query.
 */
void
GuiSurfaceRegionOfInterestDialog::slotGeodesicNodePushButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_GEODESIC_NODE_SELECT);
}

/**
 * Called to execute the geodesic query.
 */
void
GuiSurfaceRegionOfInterestDialog::slotGeodesicPushButton()
{
   if (haveNodesInROI() == false) {
      GuiMessageBox::critical(this, "Error: no nodes in ROI",
                            "There are no nodes in the region of interest.", "OK");
      return;
   }
   
   const int nodeNumber = geodesicNodeSpinBox->value();
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      if ((nodeNumber < 0) || (nodeNumber >= bms->getNumberOfNodes())) {
         GuiMessageBox::critical(this, "Error", "No query node selected", "OK");
         return;
      }
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
                                        &nodeInROI);
         try {
            bmsg.execute();
         }
         catch (BrainModelAlgorithmException& e) {
            GuiMessageBox::critical(this, "Error", e.whatQString(), "OK");
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
}

/**
 * create the smooth nodes operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationSmoothNodes()
{
   QPushButton* smoothButton = new QPushButton("Smooth Selected Nodes...");
   smoothButton->setAutoDefault(false);
   smoothButton->setFixedSize(smoothButton->sizeHint());
   QObject::connect(smoothButton, SIGNAL(clicked()),
                    this, SLOT(slotSmoothNodes()));

   operationSmoothNodesWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationSmoothNodesWidget);
   layout->addWidget(smoothButton);
}

/**
 * Called to smooth the selected nodes.
 */
void
GuiSurfaceRegionOfInterestDialog::slotSmoothNodes()
{
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      GuiSmoothingDialog sd(this, true, false, &nodeInROI);
      if (sd.exec()) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         GuiBrainModelOpenGL::updateAllGL(NULL); 
         QApplication::restoreOverrideCursor();
      }
   }
}

/**
 * create the statistical report operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationStatisticalReport()
{
   QLabel* distLabel = new QLabel("Distortion Correction");
   distortionCorrectionMetricColumnComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_METRIC,
                                                  false,
                                                  true,
                                                  false);
   distortionCorrectionMetricColumnComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   distortionCorrectionMetricColumnComboBox->setToolTip(
                 "Use this control to select a metric file column\n"
                 "that contains a distortion correction ratio.");
   QHBoxLayout* distLayout = new QHBoxLayout;
   distLayout->addWidget(distLabel);
   distLayout->addWidget(distortionCorrectionMetricColumnComboBox);
   distLayout->addWidget(distLabel, 0);
   distLayout->addWidget(distortionCorrectionMetricColumnComboBox, 100);
                 
   tabSeparateReportCheckBox = new QCheckBox(
                 "Separate Report With Semicolons (for import to spreadsheet)");
   
   QPushButton* createReportButton = new QPushButton("Create Report");
   createReportButton->setAutoDefault(false);
   createReportButton->setFixedSize(createReportButton->sizeHint());
   QObject::connect(createReportButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateReportButton()));
                    
   operationStatisticalReportWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationStatisticalReportWidget);
   layout->addLayout(distLayout);
   layout->addWidget(tabSeparateReportCheckBox);
   layout->addWidget(createReportButton);   
}

/**
 * create the statistical report operation section
 */
void
GuiSurfaceRegionOfInterestDialog::createOperationStatisticalPaintReport()
{
   QLabel* paintLabel = new QLabel("Paint Column ");
   paintRegionReportColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                             GUI_NODE_FILE_TYPE_PAINT,
                                             false,
                                             false,
                                             false);
   paintRegionReportColumnComboBox->setMaximumWidth(maxComboBoxWidth);
   QHBoxLayout* columnLayout = new QHBoxLayout;
   columnLayout->addWidget(paintLabel);
   columnLayout->addWidget(paintRegionReportColumnComboBox);
   //columnLayout->setStretchFactor(paintLabel, 0);
   //columnLayout->setStretchFactor(paintRegionReportColumnComboBox, 100);
   columnLayout->addStretch();
   
   tabSeparatePaintReportCheckBox = new QCheckBox(
                 "Separate Paint Subregion Report With Semicolons (for import to spreadsheet)");
   
   QPushButton* createPaintReportButton = new QPushButton("Create Paint Subregion Report");
   createPaintReportButton->setAutoDefault(false);
   createPaintReportButton->setFixedSize(createPaintReportButton->sizeHint());
   QObject::connect(createPaintReportButton, SIGNAL(clicked()),
                    this, SLOT(slotCreatePaintReportButton()));

   operationStatisticalPaintReportWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(operationStatisticalPaintReportWidget);
   layout->addLayout(columnLayout);
   layout->addWidget(tabSeparatePaintReportCheckBox);
   layout->addWidget(createPaintReportButton);
}

/**
 * Called when an operation combo box selection is made
 */
void
GuiSurfaceRegionOfInterestDialog::slotOperationMode(int item)
{
   const OPERATION_MODE opMode = static_cast<OPERATION_MODE>(item);
   
   switch(opMode) {
      case OPERATION_MODE_ASSIGN_METRIC:
         operationsWidgetStack->setCurrentWidget(operationAssignMetricWidget);
         break;
      case OPERATION_MODE_ASSIGN_PAINT:
         operationsWidgetStack->setCurrentWidget(operationAssignPaintWidget);
         break;
      case OPERATION_MODE_ASSIGN_SURFACE_SHAPE:
         operationsWidgetStack->setCurrentWidget(operationAssignSurfaceShapeWidget);
         break;
      case OPERATION_MODE_COMPUTE_INTEGRATED_FOLDING_INDEX:
         operationsWidgetStack->setCurrentWidget(operationComputeIntegratedFoldingIndexWidget);
         break;
      case OPERATION_MODE_CREATE_BORDERS_FROM_CLUSTERS:
         operationsWidgetStack->setCurrentWidget(operationCreateBordersFromClustersWidget);
         break;
      case OPERATION_MODE_CREATE_VOLUME_ROI:
         operationsWidgetStack->setCurrentWidget(operationCreateVolumeRoiWidget);
         break;
      case OPERATION_MODE_DISCONNECT_NODES:
         operationsWidgetStack->setCurrentWidget(operationDisconnectNodesWidget);
         break;
      case OPERATION_MODE_GEODESIC:
         operationsWidgetStack->setCurrentWidget(operationGeodesicWidget);
         break;
      case OPERATION_MODE_PROB_ATLAS_OVERLAP:
         operationsWidgetStack->setCurrentWidget(operationProbAtlasWidget);
         break;
      case OPERATION_MODE_STATISTICAL_REPORT:
         operationsWidgetStack->setCurrentWidget(operationStatisticalReportWidget);
         break;
      case OPERATION_MODE_STATISTICAL_PAINT_REPORT:
         operationsWidgetStack->setCurrentWidget(operationStatisticalPaintReportWidget);
         break;
      case OPERATION_MODE_SMOOTH_NODES:
         operationsWidgetStack->setCurrentWidget(operationSmoothNodesWidget);
         break;
      case OPERATION_MODE_SHAPE_CORRELATION:
         operationsWidgetStack->setCurrentWidget(operationShapeCorrelationWidget);
         break;
      case OPERATION_MODE_SHAPE_CLUSTER_ANALYSIS:
         operationsWidgetStack->setCurrentWidget(operationShapeClusterWidget);
   }
}

/**
 * Create the node selection section.
 */
QWidget* 
GuiSurfaceRegionOfInterestDialog::createNodeSelectionSection()
{
   //
   // Select Nodes
   //
   QPushButton* selectNodesPushButton = new QPushButton("Select Nodes");
   selectNodesPushButton->setAutoDefault(false);
   selectNodesPushButton->setFixedSize(selectNodesPushButton->sizeHint());
   QObject::connect(selectNodesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectNodesButton()));
     
   //
   // Selection logic combo box
   //
   selectionLogicComboBox = new QComboBox;
   selectionLogicComboBox->insertItem(SELECTION_LOGIC_NORMAL, "Normal Selection");
   selectionLogicComboBox->insertItem(SELECTION_LOGIC_AND, "And Selection (Intersection)");
   selectionLogicComboBox->insertItem(SELECTION_LOGIC_OR, "Or Selection (Union)");
   selectionLogicComboBox->insertItem(SELECTION_LOGIC_NOT, "Not Selection");
   selectionLogicComboBox->insertItem(SELECTION_LOGIC_AND_NOT, "And-Not Selection");
   selectionLogicComboBox->setFixedSize(selectionLogicComboBox->sizeHint());
   QObject::connect(selectionLogicComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSelectionLogic(int)));
   slotSelectionLogic(selectionLogicComboBox->currentIndex());
   
   //
   // layout on left
   //
   QVBoxLayout* selectLayout = new QVBoxLayout;
   selectLayout->addWidget(selectNodesPushButton);
   selectLayout->addWidget(selectionLogicComboBox);
   selectLayout->addStretch();
   
   //
   // Deselect Nodes
   //
   QPushButton* deselectNodesPushButton = new QPushButton("Deselect Nodes");
   deselectNodesPushButton->setAutoDefault(false);
   deselectNodesPushButton->setFixedSize(deselectNodesPushButton->sizeHint());
   QObject::connect(deselectNodesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeselectNodesButton()));
     
   //
   // Selected nodes label
   //
   QLabel* nodesSelLabel = new QLabel("Nodes Selected: ");
   numberOfNodesSelectedLabel = new QLabel("              ");
   updateNumberOfSelectedNodesLabel();
   
   //
   // Show nodes check box
   //
   showSelectedNodesCheckBox = new QCheckBox("Show Selected Nodes");
   QObject::connect(showSelectedNodesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowSelectedNodes(bool)));
   showSelectedNodesCheckBox->setChecked(true);
   slotShowSelectedNodes(showSelectedNodesCheckBox->isChecked());
   
   //
   // layout for some items
   //
   QHBoxLayout* selNodesLayout = new QHBoxLayout;
   selNodesLayout->addWidget(nodesSelLabel);
   selNodesLayout->addWidget(numberOfNodesSelectedLabel);
   selNodesLayout->addStretch();
   QVBoxLayout* togglesLayout = new QVBoxLayout;
   togglesLayout->addWidget(deselectNodesPushButton);
   togglesLayout->addLayout(selNodesLayout);
   togglesLayout->addWidget(showSelectedNodesCheckBox);
   
   //
   // Widget and layout for page
   //
   QWidget* nodeBox = new QWidget;
   QHBoxLayout* layout = new QHBoxLayout(nodeBox);
   layout->addLayout(selectLayout);
   layout->addLayout(togglesLayout);
   return nodeBox;
}

/**
 * Called when the selection logic is changed.
 */
void
GuiSurfaceRegionOfInterestDialog::slotSelectionLogic(int item)
{
   selectionLogic = static_cast<SELECTION_LOGIC>(item);
}

/**
 * Called when show selected nodes paint toggle is changed.
 */
void
GuiSurfaceRegionOfInterestDialog::slotShowSelectedNodes(bool on)
{
   DisplaySettingsSurface* dss = theMainWindow->getBrainSet()->getDisplaySettingsSurface();
   dss->setDisplayRoiNodeHighlights(on);
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

/**
 * Update the number of selected nodes label
 */
void
GuiSurfaceRegionOfInterestDialog::updateNumberOfSelectedNodesLabel()
{
   int numSelected = std::count(nodeInROI.begin(), nodeInROI.end(), true);
   numberOfNodesSelectedLabel->setNum(numSelected);
}

/**
 * Create the paint node selection section.
 */
void
GuiSurfaceRegionOfInterestDialog::createNodeSelectionPaint()
{
   const int minComboWidth = 200;

   //
   // paint category
   //
   QLabel* categoryLabel = new QLabel("Category ");
   paintWithNameCategoryComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                                     GUI_NODE_FILE_TYPE_PAINT,
                                                                     false,
                                                                     false,
                                                                     false);
   paintWithNameCategoryComboBox->setMaximumWidth(maxComboBoxWidth);
   paintWithNameCategoryComboBox->setMinimumWidth(minComboWidth);
   QHBoxLayout* categoryLayout = new QHBoxLayout;
   categoryLayout->addWidget(categoryLabel);
   categoryLayout->addWidget(paintWithNameCategoryComboBox);
   categoryLayout->setStretchFactor(categoryLabel, 0);
   categoryLayout->setStretchFactor(paintWithNameCategoryComboBox, 100);
   categoryLayout->addStretch();
   
   //
   // Selection buttons
   //
   QPushButton* paintNameFromListPushButton = new QPushButton("Select Name From List...");
   paintNameFromListPushButton->setAutoDefault(false);
   QObject::connect(paintNameFromListPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPaintNameFromListPushButton()));
   
   QPushButton* paintNameFromMousePushButton = new QPushButton("Select Node With Mouse...");
   paintNameFromMousePushButton->setAutoDefault(false);
   QObject::connect(paintNameFromMousePushButton, SIGNAL(clicked()),
                    this, SLOT(slotPaintNameFromMousePushButton()));
   QtUtilities::makeButtonsSameSize(paintNameFromListPushButton, paintNameFromMousePushButton);
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(paintNameFromListPushButton);
   buttonsLayout->addWidget(paintNameFromMousePushButton);
   buttonsLayout->addStretch();
   
   //
   // paint name
   //
   QLabel* nameLabel = new QLabel("Name ");
   paintWithNameSelectedLabel = new QLabel("                                            ");
   QHBoxLayout* nameLayout = new QHBoxLayout;
   nameLayout->addWidget(nameLabel);
   nameLayout->addWidget(paintWithNameSelectedLabel);
   nameLayout->setStretchFactor(nameLabel, 0);
   nameLayout->setStretchFactor(paintWithNameSelectedLabel, 100);
   
   //
   // Group Box for Nodes with Paint
   //
   nodesWithPaintQVBox = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(nodesWithPaintQVBox);
   layout->addLayout(categoryLayout);
   layout->addLayout(buttonsLayout);
   layout->addLayout(nameLayout);
}

/**
 * Create the border node selection section.
 */
void
GuiSurfaceRegionOfInterestDialog::createNodeSelectionBorder()
{
   const int minComboWidth = 200;
   
   //
   // Selection buttons
   //
   QPushButton* borderNameFromListPushButton = new QPushButton("Select Name From List...");
   borderNameFromListPushButton->setAutoDefault(false);
   QObject::connect(borderNameFromListPushButton, SIGNAL(clicked()),
                    this, SLOT(slotBorderNameFromListPushButton()));
   
   QPushButton* borderNameFromMousePushButton = new QPushButton("Select Border With Mouse...");
   borderNameFromMousePushButton->setAutoDefault(false);
   QObject::connect(borderNameFromMousePushButton, SIGNAL(clicked()),
                    this, SLOT(slotBorderNameFromMousePushButton()));
   QtUtilities::makeButtonsSameSize(borderNameFromListPushButton, borderNameFromMousePushButton);
   QHBoxLayout* selectLayout = new QHBoxLayout;
   selectLayout->addWidget(borderNameFromListPushButton);
   selectLayout->addWidget(borderNameFromMousePushButton);
   selectLayout->addStretch();
   
   //
   // border name label
   //
   QLabel* nameLabel = new QLabel("Name ");
   borderNameSelectedLabel = new QLabel("                                     ");
   QHBoxLayout* nameLayout = new QHBoxLayout;
   nameLayout->addWidget(nameLabel);
   nameLayout->addWidget(borderNameSelectedLabel);
   nameLayout->setStretchFactor(nameLabel, 0);
   nameLayout->setStretchFactor(borderNameSelectedLabel, 100);
   
   //
   // Flat surface
   //
   QLabel* surfaceLabel = new QLabel("Flat Surface");
   borderSurfaceComboBox = new GuiBrainModelSelectionComboBox(false,
                                                              true,
                                                              false,
                                                              "",
                                                              0, 
                                                              "borderSurfaceComboBox",
                                                              true);
   borderSurfaceComboBox->setMaximumWidth(maxComboBoxWidth);
   borderSurfaceComboBox->setMinimumWidth(minComboWidth);
   QHBoxLayout* surfaceLayout = new QHBoxLayout;
   surfaceLayout->addWidget(surfaceLabel);
   surfaceLayout->addWidget(borderSurfaceComboBox);
   //surfaceLayout->setStretchFactor(surfaceLabel, 0);
   //surfaceLayout->setStretchFactor(borderSurfaceComboBox, 100);
   surfaceLayout->addStretch();
   
   //
   // widget and layout for nodes within border
   //
   nodesWithinBorderQVBox = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(nodesWithinBorderQVBox);
   layout->addLayout(selectLayout);
   layout->addLayout(nameLayout);
   layout->addLayout(surfaceLayout);
}

/**
 * Create the metric node selection section.
 */
void
GuiSurfaceRegionOfInterestDialog::createNodeSelectionMetric()
{
   QLabel* lowLabel = new QLabel("Threshold   Low");
   metricLowerThresholdLineEdit = new QLineEdit;
   metricLowerThresholdLineEdit->setText("1.0");
   metricLowerThresholdLineEdit->setFixedWidth(120);

   QLabel* highLabel = new QLabel("  High");
   metricUpperThresholdLineEdit = new QLineEdit;
   metricUpperThresholdLineEdit->setText("50000.0");
   metricUpperThresholdLineEdit->setFixedWidth(120);
   
   QHBoxLayout* threshLayout = new QHBoxLayout;
   threshLayout->addWidget(lowLabel);
   threshLayout->addWidget(metricLowerThresholdLineEdit);
   threshLayout->addWidget(highLabel);
   threshLayout->addWidget(metricUpperThresholdLineEdit);
   threshLayout->addStretch();
   
   QButtonGroup* metricSelectionButtonGroup = new QButtonGroup(this);
   QObject::connect(metricSelectionButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotMetricModeSelection(int)));
   metricAllNodesRadioButton = new QRadioButton("All Nodes Within Threshold");
   metricSelectionButtonGroup->addButton(metricAllNodesRadioButton, 0);
   metricChooseNodesRadioButton = new QRadioButton(
                              "Nodes Within Threshold Connected to Node Selected With Mouse");
   metricSelectionButtonGroup->addButton(metricChooseNodesRadioButton, 1);
   metricAllNodesRadioButton->setChecked(true);
   
   metricNodeNumberLabel = new QLabel("                                                ");
   
   //
   // Group Box for nodes with metric
   //
   nodesWithMetricQVBox = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(nodesWithMetricQVBox);
   layout->addLayout(threshLayout);
   layout->addWidget(metricAllNodesRadioButton);
   layout->addWidget(metricChooseNodesRadioButton);
   layout->addWidget(metricNodeNumberLabel);
}

/**
 * Create the shape node selection section.
 */
void
GuiSurfaceRegionOfInterestDialog::createNodeSelectionShape()
{
   QLabel* lowLabel = new QLabel("Threshold   Low");
   shapeLowerThresholdLineEdit = new QLineEdit;
   shapeLowerThresholdLineEdit->setText("1.0");
   shapeLowerThresholdLineEdit->setFixedWidth(120);

   QLabel* highLabel = new QLabel("  High");
   shapeUpperThresholdLineEdit = new QLineEdit;
   shapeUpperThresholdLineEdit->setText("50000.0");
   shapeUpperThresholdLineEdit->setFixedWidth(120);
   QHBoxLayout* threshLayout = new QHBoxLayout;
   threshLayout->addWidget(lowLabel);
   threshLayout->addWidget(shapeLowerThresholdLineEdit);
   threshLayout->addWidget(highLabel);
   threshLayout->addWidget(shapeUpperThresholdLineEdit);
   threshLayout->addStretch();
   
   QButtonGroup* shapeSelectionButtonGroup = new QButtonGroup(this);
   QObject::connect(shapeSelectionButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotShapeModeSelection(int)));
   shapeAllNodesRadioButton = new QRadioButton("All Nodes Within Threshold");
   shapeSelectionButtonGroup->addButton(shapeAllNodesRadioButton, 0);
   shapeChooseNodesRadioButton = new QRadioButton(
                              "Nodes Within Threshold Connected to Node Selected With Mouse");
   shapeSelectionButtonGroup->addButton(shapeChooseNodesRadioButton, 1);
   shapeAllNodesRadioButton->setChecked(true);
   
   shapeNodeNumberLabel = new QLabel("                                                ");   

   //
   // widget and layout for nodes with shape
   //
   nodesWithShapeQVBox = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(nodesWithShapeQVBox);
   layout->addLayout(threshLayout);
   layout->addWidget(shapeAllNodesRadioButton);
   layout->addWidget(shapeChooseNodesRadioButton);
   layout->addWidget(shapeNodeNumberLabel);
}

/**
 * Called when a shape mode is selected.
 */
void
GuiSurfaceRegionOfInterestDialog::slotShapeModeSelection(int shapeMode)
{
   if (shapeMode == 1) {
      theMainWindow->getBrainModelOpenGL()->setMouseMode(
            GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_SHAPE_NODE_SELECT);
   }
}

/**
 * Called to create a volume from the display query nodes.
 */
void
GuiSurfaceRegionOfInterestDialog::slotCreateVolumeFromQueryNodesButton()
{
   if (haveNodesInROI() == false) {
      GuiMessageBox::critical(this, "Error: no nodes in ROI",
                            "There are no nodes in the region of interest.", "OK");                            
      return;
   }

   GuiSurfaceToVolumeDialog svd(this, GuiSurfaceToVolumeDialog::DIALOG_MODE_NORMAL,
                                "Create Region Of Interest Volume");
   if (svd.exec() == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      float offset[3];
      int dim[3];
      float origin[3];
      float voxelSize[3];
      svd.getSurfaceOffset(offset);
      svd.getVolumeDimensions(dim);
      svd.getVolumeVoxelSizes(voxelSize);
      svd.getVolumeOrigin(origin);
      BrainModelSurfaceToVolumeConverter stv(theMainWindow->getBrainSet(),
                                             svd.getSelectedSurface(),
                                             svd.getStandardVolumeSpace(),
                                             offset,
                                             dim,
                                             voxelSize,
                                             origin,
                                             svd.getSurfaceInnerBoundary(),
                                             svd.getSurfaceOuterBoundary(),
                                             svd.getSurfaceThicknessStep(),
                   BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_ROI_NODES);
      stv.setNodeToVoxelMappingEnabled(svd.getNodeToVoxelMappingEnabled(),
                                       svd.getNodeToVoxelMappingFileName());
      try {
         stv.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         GuiMessageBox::critical(this, "Error", e.whatQString(), "OK");
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
 * Called when a metric mode is selected.
 */
void
GuiSurfaceRegionOfInterestDialog::slotMetricModeSelection(int metricMode)
{
   if (metricMode == 1) {
      theMainWindow->getBrainModelOpenGL()->setMouseMode(
            GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_METRIC_NODE_SELECT);
   }
}

/**
 * Called when paint name push button is selected.
 */
void
GuiSurfaceRegionOfInterestDialog::slotPaintNameFromListPushButton()
{
   GuiPaintColumnNamesListBoxSelectionDialog pd(this,
                                                paintWithNameCategoryComboBox->currentIndex());
   if (pd.exec() == QDialog::Accepted) {
      setPaintIndexForQuery(pd.getSelectedItemIndex());
   }      
}

/**
 * Called when paint name from mouse push button is selected.
 */
void
GuiSurfaceRegionOfInterestDialog::slotPaintNameFromMousePushButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
      GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_PAINT_INDEX_SELECT);
}

/**
 * Called when border name from mouse push button is pressed.
 */
void
GuiSurfaceRegionOfInterestDialog::slotBorderNameFromMousePushButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
      GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_BORDER_SELECT);
}

/**
 * Called when border name push from list button is pressed.
 */
void
GuiSurfaceRegionOfInterestDialog::slotBorderNameFromListPushButton()
{
   GuiBorderNamesListBoxSelectionDialog bs(this,
                                           selectedBorderName);
   if (bs.exec() == QDialog::Accepted) {
      setBorderNameForQuery(bs.getSelectedText());
   }
}

/**
 * Called when a query mode selection is made.
 */
void
GuiSurfaceRegionOfInterestDialog::slotSelectionMode(int item)
{
   selectionModeComboBox->setCurrentIndex(item);
   
   selectionMode = static_cast<SELECTION_MODE>(item);

   const bool havePaints = (theMainWindow->getBrainSet()->getPaintFile()->getNumberOfColumns() > 0);

   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();   
   bool haveBorders = bmbs->getNumberOfBorders();
   
   const bool haveMetrics = (theMainWindow->getBrainSet()->getMetricFile()->getNumberOfColumns() > 0);
   const bool haveShape   = (theMainWindow->getBrainSet()->getSurfaceShapeFile()->getNumberOfColumns() > 0);
   //
   // Disable items initially
   nodesWithPaintQVBox->setEnabled(false);
   nodesWithinBorderQVBox->setEnabled(false);
   nodesWithMetricQVBox->setEnabled(false);
   
   //
   // Enable dialog items based upon mode.
   //   
   switch(selectionMode) {
      case SELECTION_MODE_ENTIRE_SURFACE:
         queryControlWidgetStack->setCurrentWidget(queryEntireSurfaceWidget);
         break;
      case SELECTION_MODE_NODES_WITH_PAINT:
         nodesWithPaintQVBox->setEnabled(havePaints);
         queryControlWidgetStack->setCurrentWidget(nodesWithPaintQVBox);
         break;
      case SELECTION_MODE_NODES_WITHIN_BORDER:
         nodesWithinBorderQVBox->setEnabled(haveBorders);
         queryControlWidgetStack->setCurrentWidget(nodesWithinBorderQVBox);
         break;
      case SELECTION_MODE_NODES_WITH_METRIC:
         nodesWithMetricQVBox->setEnabled(haveMetrics);
         queryControlWidgetStack->setCurrentWidget(nodesWithMetricQVBox);
         break;
      case SELECTION_MODE_NODES_WITH_SHAPE:
         nodesWithShapeQVBox->setEnabled(haveShape);
         queryControlWidgetStack->setCurrentWidget(nodesWithShapeQVBox);
         break;
      case SELECTION_MODE_NODES_WITH_CROSSOVERS:
         queryControlWidgetStack->setCurrentWidget(nodesWithCrossoversWidget);
         break;
      case SELECTION_MODE_NONE:
         break;
   }
}

/**
 * Create the attributes page
 */
void
GuiSurfaceRegionOfInterestDialog::createAttributeSelectionPage()
{
   //
   // Create the output page.
   //
   attributesPage = new QWidget;
   attributesPage->setMinimumSize(300, 300);
   tabWidget->addTab(attributesPage, "Attributes");
   
   //
   // Vertical layout output groups
   //
   QVBoxLayout* attributesLayout = new QVBoxLayout(attributesPage);
   attributesLayout->setMargin(3);
   attributesLayout->setSpacing(3);
   
   //
   // Scrolling widget and QWidget for output selections
   //
   //scrollView->setMinimumSize(400, 400);
   QWidget* outputScrolledContents = new QWidget;
   QScrollArea* scrollView = new QScrollArea;
   attributesLayout->addWidget(scrollView);
   scrollView->setWidget(outputScrolledContents);
   scrollView->setWidgetResizable(true);
   
   //
   // Vertical layout output groups
   //
   QVBoxLayout* outputLayout = new QVBoxLayout(outputScrolledContents);
   outputLayout->setMargin(3);
   outputLayout->setSpacing(3);
   
   //
   // Metric All On/Off buttons
   //
   QPushButton* metricAllOnPushButton = new QPushButton("All On");
   metricAllOnPushButton->setAutoDefault(false);
   QObject::connect(metricAllOnPushButton, SIGNAL(clicked()),
                    this, SLOT(slotMetricAllOnPushButton()));
   QPushButton* metricAllOffPushButton = new QPushButton("All Off");
   metricAllOffPushButton->setAutoDefault(false);
   QObject::connect(metricAllOffPushButton, SIGNAL(clicked()),
                    this, SLOT(slotMetricAllOffPushButton()));
   QtUtilities::makeButtonsSameSize(metricAllOnPushButton, metricAllOffPushButton);
   QHBoxLayout* metricHBoxLayout = new QHBoxLayout;
   metricHBoxLayout->addWidget(metricAllOnPushButton);
   metricHBoxLayout->addWidget(metricAllOffPushButton);
   metricHBoxLayout->addStretch();

   //
   // Vertical group box for metric selections
   //
   QGroupBox* metricOutputGroupBox = new QGroupBox("Metric Categories");
   metricOutputLayout = new QVBoxLayout(metricOutputGroupBox);
   outputLayout->addWidget(metricOutputGroupBox);
   metricOutputLayout->addLayout(metricHBoxLayout);
   
   //
   //  All On/Off buttons
   //
   QPushButton* paintAllOnPushButton = new QPushButton("All On");
   paintAllOnPushButton->setAutoDefault(false);
   QObject::connect(paintAllOnPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPaintAllOnPushButton()));
   QPushButton* paintAllOffPushButton = new QPushButton("All Off");
   paintAllOffPushButton->setAutoDefault(false);
   QObject::connect(paintAllOffPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPaintAllOffPushButton()));
   QtUtilities::makeButtonsSameSize(paintAllOnPushButton, paintAllOffPushButton);
   QHBoxLayout* paintHBoxLayout = new QHBoxLayout;
   paintHBoxLayout->addWidget(paintAllOnPushButton);
   paintHBoxLayout->addWidget(paintAllOffPushButton);
   paintHBoxLayout->addStretch();
   
   //
   // Vertical group box for paint selections
   //
   QGroupBox* paintOutputGroupBox = new QGroupBox("Paint Categories");
   paintOutputLayout = new QVBoxLayout(paintOutputGroupBox);
   outputLayout->addWidget(paintOutputGroupBox);
   paintOutputLayout->addLayout(paintHBoxLayout);
      
   //
   // Shape All On/Off buttons
   //
   QPushButton* shapeAllOnPushButton = new QPushButton("All On");
   shapeAllOnPushButton->setAutoDefault(false);
   QObject::connect(shapeAllOnPushButton, SIGNAL(clicked()),
                    this, SLOT(slotShapeAllOnPushButton()));
   QPushButton* shapeAllOffPushButton = new QPushButton("All Off");
   shapeAllOffPushButton->setAutoDefault(false);
   QObject::connect(shapeAllOffPushButton, SIGNAL(clicked()),
                    this, SLOT(slotShapeAllOffPushButton()));
   QtUtilities::makeButtonsSameSize(shapeAllOnPushButton, shapeAllOffPushButton);
   QHBoxLayout* shapeHBoxLayout = new QHBoxLayout;
   shapeHBoxLayout->addWidget(shapeAllOnPushButton);
   shapeHBoxLayout->addWidget(shapeAllOffPushButton);
   shapeHBoxLayout->addStretch();
   
   //
   // Vertical group box for shape selections
   //
   QGroupBox* surfaceShapeOutputGroupBox = new QGroupBox("Surface Shape Categories");
   shapeOutputLayout = new QVBoxLayout(surfaceShapeOutputGroupBox);
   outputLayout->addWidget(surfaceShapeOutputGroupBox);
   shapeOutputLayout->addLayout(shapeHBoxLayout);
   
   
   //
   // Squish the items together
   //
   //outputScrolledContents->setMinimumSize(outputScrolledContents->sizeHint());
}

/**
 * Called when metric all on pushbutton is pressed.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotMetricAllOnPushButton()
{
   for (int i = 0; i < static_cast<int>(metricCheckBoxes.size()); i++) {
      metricCheckBoxes[i]->setChecked(true);
   }
}

/**
 * Called when metric all off pushbutton is pressed.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotMetricAllOffPushButton()
{
   for (int i = 0; i < static_cast<int>(metricCheckBoxes.size()); i++) {
      metricCheckBoxes[i]->setChecked(false);
   }
}

/**
 * Called when paint all on pushbutton is pressed.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotPaintAllOnPushButton()
{
   for (int i = 0; i < static_cast<int>(paintCheckBoxes.size()); i++) {
      paintCheckBoxes[i]->setChecked(true);
   }
}

/**
 * Called when paint all off pushbutton is pressed.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotPaintAllOffPushButton()
{
   for (int i = 0; i < static_cast<int>(paintCheckBoxes.size()); i++) {
      paintCheckBoxes[i]->setChecked(false);
   }
}

/**
 * Called when shape all on pushbutton is pressed.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotShapeAllOnPushButton()
{
   for (int i = 0; i < static_cast<int>(surfaceShapeCheckBoxes.size()); i++) {
      surfaceShapeCheckBoxes[i]->setChecked(true);
   }
}

/**
 * Called when shape all off pushbutton is pressed.
 */
void 
GuiSurfaceRegionOfInterestDialog::slotShapeAllOffPushButton()
{
   for (int i = 0; i < static_cast<int>(surfaceShapeCheckBoxes.size()); i++) {
      surfaceShapeCheckBoxes[i]->setChecked(false);
   }
}

/**
 * Create the report page.
 */
void
GuiSurfaceRegionOfInterestDialog::createReportPage()
{
   //
   // Create the query page
   //
   reportPage = new QWidget;
   tabWidget->addTab(reportPage, "Report");
   
   //
   // Layout for page
   //
   QVBoxLayout* layout = new QVBoxLayout(reportPage);
   layout->setMargin(3);
   layout->setSpacing(3);
   
   //
   // Font for text edit
   //
   //QFont font("Fixed", 12);
   QFont font("Monaco", 12);
   
   //
   // Report text editor
   //
   reportTextEdit = new QTextEdit;
   reportTextEdit->setLineWrapMode(QTextEdit::NoWrap);
   reportTextEdit->setFont(font);
   layout->addWidget(reportTextEdit);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(3);
   layout->addLayout(buttonsLayout);
   
   //
   // Buttons
   //
   QPushButton* clearPushButton = new QPushButton("Clear");
   clearPushButton->setAutoDefault(false);
   buttonsLayout->addWidget(clearPushButton);
   QObject::connect(clearPushButton, SIGNAL(clicked()),
                    this, SLOT(slotClearPushButton()));
   QPushButton* savePushButton = new QPushButton("Save To File...");
   savePushButton->setAutoDefault(false);
   buttonsLayout->addWidget(savePushButton);
   QObject::connect(savePushButton, SIGNAL(clicked()),
                    this, SLOT(slotSavePushButton()));
   QtUtilities::makeButtonsSameSize(clearPushButton, savePushButton);
}

/**
 * slot called when report  push button pressed.
 */
void
GuiSurfaceRegionOfInterestDialog::slotClearPushButton()
{
   reportTextEdit->clear();
}

/**
 * slot called when report  push button pressed.
 */
void
GuiSurfaceRegionOfInterestDialog::slotSavePushButton()
{
   QFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(QFileDialog::AcceptSave);
   fd.setWindowTitle("Choose ROI Text File Name");
   fd.setFilter("Text Files (*.txt)");
   fd.setFileMode(QFileDialog::AnyFile);
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
            GuiMessageBox::critical(this, "Error Opening File", msg, "OK");         
         }
      }
   }
}

/**
 * Update the section with the node attribute file
 */
void
GuiSurfaceRegionOfInterestDialog::updateNodeAttributeGroupBox(
                                 QVBoxLayout* layout,
                                 std::vector<QCheckBox*>& checkBoxes,
                                 NodeAttributeFile* naf)
{
   const int numColumns = naf->getNumberOfColumns();
   const int numExistingButtons = checkBoxes.size();
   
   
   //
   // save existing selections
   //
   std::map<QString,bool> currentSelections;
   const int maxButton = std::min(numExistingButtons, numColumns);
   for (int i = 0; i < maxButton; i++) {
      currentSelections[naf->getColumnName(i)] = checkBoxes[i]->isChecked();
   }
   
   //
   // Load up with current columns
   //
   for (int i = 0; i < numColumns; i++) {
      if (i < numExistingButtons) {
         checkBoxes[i]->setText(naf->getColumnName(i));
         checkBoxes[i]->show();
         checkBoxes[i]->setChecked(true);
         std::map<QString,bool>::iterator iter = 
             currentSelections.find(naf->getColumnName(i));
         if (iter != currentSelections.end()) {
            checkBoxes[i]->setChecked(iter->second);
         }
      }
      else {
         QCheckBox* checkBox = new QCheckBox(naf->getColumnName(i));
         checkBox->setChecked(true);
         checkBoxes.push_back(checkBox);
         layout->addWidget(checkBox);
      }
   }
   
   //
   // Hide unneeded radio buttons
   //
   for (int i = numColumns; i < numExistingButtons; i++) {
      checkBoxes[i]->hide();
   }
}

/**
 * Update the section with the node attribute file
 */
void
GuiSurfaceRegionOfInterestDialog::updateNodeAttributeGroupBox(
                                 QVBoxLayout* layout,
                                 std::vector<QCheckBox*>& checkBoxes,
                                 GiftiNodeDataFile* naf)
{
   const int numColumns = naf->getNumberOfColumns();
   const int numExistingButtons = checkBoxes.size();
   
   
   //
   // save existing selections
   //
   std::map<QString,bool> currentSelections;
   const int maxButton = std::min(numExistingButtons, numColumns);
   for (int i = 0; i < maxButton; i++) {
      currentSelections[naf->getColumnName(i)] = checkBoxes[i]->isChecked();
   }
   
   //
   // Load up with current columns
   //
   for (int i = 0; i < numColumns; i++) {
      if (i < numExistingButtons) {
         checkBoxes[i]->setText(naf->getColumnName(i));
         checkBoxes[i]->show();
         checkBoxes[i]->setChecked(true);
         std::map<QString,bool>::iterator iter = 
             currentSelections.find(naf->getColumnName(i));
         if (iter != currentSelections.end()) {
            checkBoxes[i]->setChecked(iter->second);
         }
      }
      else {
         QCheckBox* checkBox = new QCheckBox(naf->getColumnName(i));
         checkBox->setChecked(true);
         checkBoxes.push_back(checkBox);
         layout->addWidget(checkBox);
      }
   }
   
   //
   // Hide unneeded radio buttons
   //
   for (int i = numColumns; i < numExistingButtons; i++) {
      checkBoxes[i]->hide();
   }
}

/**
 * Set border name for query.
 */
void
GuiSurfaceRegionOfInterestDialog::setBorderNameForQuery(const QString& name)
{
   selectedBorderName = name;
   borderNameSelectedLabel->setText(selectedBorderName);
}

/**
 * Set paint index for query.
 */
void
GuiSurfaceRegionOfInterestDialog::setPaintIndexForQuery(const int paintIndex)
{
   paintWithNameIndex = paintIndex;
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   paintWithNameSelectedLabel->setText(pf->getPaintNameFromIndex(paintWithNameIndex));
}

/**
 * Set the node for a metric query.
 */
void
GuiSurfaceRegionOfInterestDialog::setMetricNodeForQuery(const int metricNodeForQueryIn) 
{
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   const int modelIndex = bms->getBrainModelIndex();
   
   metricNodeForQuery = metricNodeForQueryIn;
   MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
   
   if (mf->getNumberOfColumns() <= 0) {
      GuiMessageBox::critical(this, "Error", "There are no metric columns.", "OK");
      return;
   }
   DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
   const int thresholdColumn = dsm->getSelectedThresholdColumn(modelIndex);
   if ((thresholdColumn < 0) || (thresholdColumn >= mf->getNumberOfColumns())) {
      GuiMessageBox::critical(this, "Error", "Threshold column is invalid.", "OK");
      return;
   }
   
   std::ostringstream str;
   str << "    Node Number: "
       << metricNodeForQuery
       << " Metric "
       << QString::number(mf->getValue(metricNodeForQuery, thresholdColumn), 'f', 6).toAscii().constData();
       
   metricNodeNumberLabel->setText(str.str().c_str());
}

/**
 * Set the node for a shape query.
 */
void
GuiSurfaceRegionOfInterestDialog::setShapeNodeForQuery(const int shapeNodeForQueryIn) 
{
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   const int modelIndex = bms->getBrainModelIndex();
   
   shapeNodeForQuery = shapeNodeForQueryIn;
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   
   if (ssf->getNumberOfColumns() <= 0) {
      GuiMessageBox::critical(this, "Error", "There are no shape columns.", "OK");
      return;
   }
   DisplaySettingsSurfaceShape* dss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
   const int column = dss->getSelectedDisplayColumn(modelIndex);
   if ((column < 0) || (column >= ssf->getNumberOfColumns())) {
      GuiMessageBox::critical(this, "Error", "Column is invalid.", "OK");
      return;
   }
   
   std::ostringstream str;
   str << "    Node Number: "
       << shapeNodeForQuery
       << " Shape "
       << QString::number(ssf->getValue(shapeNodeForQuery, column), 'f', 6).toAscii().constData();
       
   shapeNodeNumberLabel->setText(str.str().c_str());
}

/**
 * Get the paint query column.
 */
int
GuiSurfaceRegionOfInterestDialog::getQueryPaintColumn() const
{
   return paintWithNameCategoryComboBox->currentIndex();
}

/**
 *
 */
void
GuiSurfaceRegionOfInterestDialog::slotSurfaceSelection()
{
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      const TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
//         topologyComboBox->setSelectedTopologyFile(tf);
      } 
   }
}

/**
 * Called when topology is changed.
 */
/*
void
GuiSurfaceRegionOfInterestDialog::slotTopologySelection()
{
   BrainModelSurface* bms = operationSurfaceComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      if (topologyComboBox->getSelectedTopologyFile() != bms->getTopologyFile()) {
         TopologyFile* oldTopologyFile = bms->getTopologyFile();
         if (bms->setTopologyFile(topologyComboBox->getSelectedTopologyFile())) {
            std::ostringstream str;
            str << "Topology File "
                << FileUtilities::basename(bms->getTopologyFile()->getFileName()).toAscii().constData()
                << "\n is not for use with coordinate file "
                << FileUtilities::basename(bms->getCoordinateFile()->getFileName()).toAscii().constData()
                << ".\n  Topo file has tiles with node numbers exceeding \n"
                << "the number of coordinates in the coordinate file.";
            bms->setTopologyFile(oldTopologyFile);
            GuiMessageBox::critical(this, "ERROR", str.str().c_str(), "OK");
            return;
         }
         GuiFilesModified fm;
         fm.setCoordinateModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL(NULL);
      }
   }
}
*/

/**
 * Update the dialog.
 */
void
GuiSurfaceRegionOfInterestDialog::updateDialog()
{
   //
   // Update border surface combo box
   //
   borderSurfaceComboBox->updateComboBox();
      
   //
   //  update query surface combo box
   //
   operationSurfaceComboBox->updateComboBox();
   slotSurfaceSelection();
   
   //
   // Update metric distortion
   //
   distortionCorrectionMetricColumnComboBox->updateComboBox();

   //
   // update topology combo box
   //
//   topologyComboBox->updateComboBox();
//   slotTopologySelection();
   
   //
   // Initialize the query mode
   //
   slotSelectionMode(selectionMode);
   
   //
   // Reset paints
   //
   paintWithNameIndex = -1;
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   const QString paintName(paintWithNameSelectedLabel->text());
   if (paintName.isEmpty() == false) {
      paintWithNameIndex = pf->getPaintIndexFromName(paintName);
   }
   paintWithNameSelectedLabel->setText(pf->getPaintNameFromIndex(paintWithNameIndex));
   paintWithNameCategoryComboBox->updateComboBox(pf);
      
   //
   // paint column assignment
   //
   paintColumnAssignComboBox->updateComboBox(pf);
   slotAssignPaintColumn(paintColumnAssignComboBox->currentIndex());
   
   //
   // Update metric assignment
   //
   metricColumnAssignComboBox->updateComboBox();
   
   //
   // Update surface shape assignment
   //
   surfaceShapeColumnAssignComboBox->updateComboBox();
   
   //
   // Update geodesic
   //
   geodesicDistanceColumnComboBox->updateComboBox();
   geodesicMetricColumnComboBox->updateComboBox();
   
   //
   // Reset borders
   //
   bool haveBorderName = false;
   if (selectedBorderName.isEmpty() == false) {
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      const int numBorders = bmbs->getNumberOfBorders();
      for (int j = 0; j < numBorders; j++) {
         if (bmbs->getBorder(j)->getName() == selectedBorderName) {
            haveBorderName = true;
            break;
         }
      }
   }
   if (haveBorderName == false) {
      selectedBorderName = "";
   }
   borderNameSelectedLabel->setText(selectedBorderName);
   
   //
   // Update paint region column
   //
   paintRegionReportColumnComboBox->updateComboBox();
   
   updateNodeAttributeGroupBox(metricOutputLayout, metricCheckBoxes,
                               theMainWindow->getBrainSet()->getMetricFile());
   updateNodeAttributeGroupBox(paintOutputLayout, paintCheckBoxes,
                               theMainWindow->getBrainSet()->getPaintFile());
   updateNodeAttributeGroupBox(shapeOutputLayout, surfaceShapeCheckBoxes,
                               theMainWindow->getBrainSet()->getSurfaceShapeFile());
   updateNumberOfSelectedNodesLabel();
   
   shapeCorrelationColumnComboBox->updateComboBox();
}

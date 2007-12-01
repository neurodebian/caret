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

#include <cmath>

#include <QStringList>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROITextReport.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "StatisticsUtilities.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceROITextReport::BrainModelSurfaceROITextReport(BrainSet* bs,
                                                 BrainModelSurface* bmsIn,
                                                 BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                                 MetricFile* metricFileIn,
                                                 const std::vector<bool>& selectedMetricColumnsForReportIn,
                                                 MetricFile* shapeFileIn,
                                                 const std::vector<bool>& selectedShapeColumnsForReportIn,
                                                 PaintFile* paintFileIn,
                                                 const std::vector<bool>& selectedPaintColumnsForReportIn,
                                                 LatLonFile* latLonFileIn,
                                                 const int latLonFileColumnIn,
                                                 const QString& nodeSelectionTextIn,
                                                 const QString& headerTextIn,
                                                 MetricFile* metricCorrectionFileIn,
                                                 const int metricCorrectionColumnIn,
                                                 const bool tabSeparateReportFlagIn)
   : BrainModelSurfaceROIOperation(bs, bmsIn, surfaceROIIn)
{
   reportLatLonFile = NULL;
   reportMetricFile = NULL;
   reportShapeFile = NULL;
   reportPaintFile = NULL;
   reportMetricCorrectionFile = NULL;

   reportMetricFile = metricFileIn;
   selectedMetricColumnsForReport = selectedMetricColumnsForReportIn;
   reportShapeFile = shapeFileIn;
   selectedShapeColumnsForReport  = selectedShapeColumnsForReportIn;
   reportPaintFile = paintFileIn;
   selectedPaintColumnsForReport  = selectedPaintColumnsForReportIn;
   reportLatLonFile = latLonFileIn;
   reportLatLonFileColumn = latLonFileColumnIn;
   setNodeSelectionInformation(nodeSelectionTextIn);
   setHeaderText(headerTextIn);
   reportMetricCorrectionFile = metricCorrectionFileIn;
   metricCorrectionColumn = metricCorrectionColumnIn;
   tabSeparateReportFlag = tabSeparateReportFlagIn;
}                                  

/**
 * destructor.
 */
BrainModelSurfaceROITextReport::~BrainModelSurfaceROITextReport()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROITextReport::executeOperation() throw (BrainModelAlgorithmException)
{
   reportText = "";
   
   createTextReport();
}

/**
 * create the text report.
 */
void 
BrainModelSurfaceROITextReport::createTextReport() throw (BrainModelAlgorithmException)
{
   float roiArea = 0.0;
   createReportHeader(roiArea);

   //
   // ROI for metrics
   // 
   if (reportMetricFile != NULL) {
      if (std::count(selectedMetricColumnsForReport.begin(), 
                     selectedMetricColumnsForReport.end(),
                     true) > 0) {    
         metricAndSurfaceShapeReport(true);
      }
   }
   
   //
   // ROI for surface shape
   //   
   if (reportShapeFile != NULL) {
      if (std::count(selectedShapeColumnsForReport.begin(), 
                     selectedShapeColumnsForReport.end(),
                     true) > 0) {    
         metricAndSurfaceShapeReport(false);
      }
   }
   
   //
   // ROI for paint
   //
   if (reportPaintFile != NULL) {
      if (std::count(selectedPaintColumnsForReport.begin(), 
                     selectedPaintColumnsForReport.end(),
                     true) > 0) {    
         paintReport(roiArea);
      }
   }
   
   reportText.append("\n");
}

/**
 * perform paint report.
 */
void 
BrainModelSurfaceROITextReport::paintReport(const float roiArea)
{
   const CoordinateFile* cf = bms->getCoordinateFile();
   const TopologyFile* tf = bms->getTopologyFile();
   const int numTiles = tf->getNumberOfTiles();
   
   const int numPaintNames = reportPaintFile->getNumberOfPaintNames();
   bool latLonValid = false;
   if (reportLatLonFile != NULL) {
      if ((reportLatLonFileColumn >= 0) && 
          (reportLatLonFileColumn < reportLatLonFile->getNumberOfColumns() > 0)) {
         latLonValid = true;
      }
   }
   
   //
   // Find longest paint name use in the ROI
   //
   int longestPaintNameLength = 11;
   for (int m = 0; m < numPaintNames; m++) {
      longestPaintNameLength = std::max(static_cast<int>(reportPaintFile->getPaintNameFromIndex(m).length()),
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
      
   const int numPaintColumns = reportPaintFile->getNumberOfColumns();
   for (int j = 0; j < numPaintColumns; j++) {
      if (selectedPaintColumnsForReport[j]) {
      
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
                  if (surfaceROI->getNodeSelected(tileNodes[k])) {
                     const int node = tileNodes[k];
                     //
                     // Update area node's paint
                     //
                     const int paintNameIndex = reportPaintFile->getPaint(node, j);
                     float nodeArea = tileArea[i] / 3.0;
                     paintNameAreas[paintNameIndex] += nodeArea;
                     
                     float areaDistCorrect = nodeArea;
                     if ((reportMetricCorrectionFile != NULL) &&
                         (metricCorrectionColumn >= 0)) {
                        const double metric = reportMetricCorrectionFile->getValue(node, metricCorrectionColumn);
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
            if (surfaceROI->getNodeSelected(i)) {           
               const int paintNameIndex = reportPaintFile->getPaint(i, j);
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
                  reportLatLonFile->getLatLon(i, reportLatLonFileColumn, lat, lon);
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
                  
                  reportText.append("\n\n");
                  
                  QString line;
                  line.append("Paint Column ");
                  line.append(QString::number(j));
                  line.append(" ");
                  line.append(reportPaintFile->getColumnName(j));
                  reportText.append(line);
                  reportText.append("\n");
                  
                  line = "";
                  line.append(indent);
                  if (tabSeparateReportFlag) line.append(getSeparatorCharacter());
   
                  line.append(StringUtilities::leftJustify("Paint Name", 
                                                           longestPaintNameLength));
                  if (tabSeparateReportFlag) line.append(getSeparatorCharacter());
                  
                  line.append(StringUtilities::rightJustify("Area",
                                                            numberSize));
                  if (tabSeparateReportFlag) line.append(getSeparatorCharacter());
                  
                  line.append(StringUtilities::rightJustify("Percent Area",
                                                            numberSize));                                                            
                  if (tabSeparateReportFlag) line.append(getSeparatorCharacter());
                  
                  if (metricCorrectionColumn >= 0) {
                     line.append(StringUtilities::rightJustify("Area-Corr",
                                                               numberSize));
                     if (tabSeparateReportFlag) line.append(getSeparatorCharacter());
                  }
                  
                  line.append(StringUtilities::rightJustify("COG - X",
                                                            numberSize));
                  if (tabSeparateReportFlag) line.append(getSeparatorCharacter());
                  
                  line.append(StringUtilities::rightJustify("COG - Y",
                                                            numberSize));
                  if (tabSeparateReportFlag) line.append(getSeparatorCharacter());
                  
                  line.append(StringUtilities::rightJustify("COG - Z",
                                                            numberSize));
                  if (tabSeparateReportFlag) line.append(getSeparatorCharacter());
                  
                  line.append(StringUtilities::rightJustify("Latitude",
                                                            numberSize));
                  if (tabSeparateReportFlag) line.append(getSeparatorCharacter());
                  
                  line.append(StringUtilities::rightJustify("Longitude",
                                                            numberSize));
                  reportText.append(line);
                  reportText.append("\n");
               }
               
               //
               // Write the area for this paint name
               //
               QString stats;
               
               stats.append(indent);
               if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
               
               stats.append(StringUtilities::leftJustify(reportPaintFile->getPaintNameFromIndex(m),
                                                         longestPaintNameLength));
               if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
               
               stats.append(StringUtilities::rightJustify(QString::number(paintNameAreas[m], 'f', 3),
                                                          numberSize));
               if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
                              
               const double percent = (paintNameAreas[m] / roiArea) * 100.0;
               stats.append(StringUtilities::rightJustify(QString::number(percent, 'f', 3),
                                                          numberSize));
               if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
      
               if (metricCorrectionColumn >= 0) {
                  stats.append(StringUtilities::rightJustify(QString::number(paintNameAreasCorrected[m], 'f', 3),
                                                             numberSize));
                  if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
                }

               const double numNodesForPaint = paintNameNodeCounts[m];
               
               double cogX = 0.0;
               if (numNodesForPaint > 0) {
                  cogX = paintNameAreasCogX[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogX, 'f', 3),
                                                          numberSize));
               
               if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
               double cogY = 0.0;
               if (numNodesForPaint > 0) {
                  cogY = paintNameAreasCogY[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogY, 'f', 3),
                                                          numberSize));

               if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
               double cogZ = 0.0;
               if (numNodesForPaint > 0) {
                  cogZ = paintNameAreasCogZ[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogZ, 'f', 3),
                                                          numberSize));
                                                          
               if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
               double cogLat = 0.0;
               if (numNodesForPaint > 0) {
                  cogLat = paintNameAreasLat[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogLat, 'f', 3),
                                                          numberSize));
                                                          
               if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
               double cogLon = 0.0;
               if (numNodesForPaint > 0) {
                  cogLon = paintNameAreasLon[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogLon, 'f', 3),
                                                          numberSize));
               reportText.append(stats);
               reportText.append("\n");
            }
         }
      }
   }
}
      
/**
 * Peform Metric and Surface Shape Report.
 */
void
BrainModelSurfaceROITextReport::metricAndSurfaceShapeReport(const bool metricFlag)
{
   MetricFile* mf = NULL;
   if (metricFlag) {
      mf = reportMetricFile;
   }
   else {
      mf = reportShapeFile;
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
         doIt = selectedMetricColumnsForReport[j];
      }
      else {
         doIt = selectedShapeColumnsForReport[j];
      }
      
      if (doIt) {
         std::vector<float> values;
         for (int i = 0; i < numNodes; i++) {
            if (surfaceROI->getNodeSelected(i)) { 
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
                  reportText.append("\nMetrics for Region Of Interest\n");
                  reportText.append("\n");
               }
               else {
                  reportText.append("\nSurface Shape for Region of Interest\n");
                  reportText.append("\n");
               }
               
               QString str;
               str.append(StringUtilities::rightJustify("Column", columnNumberLength));
               str.append(" ");
               if (tabSeparateReportFlag) str.append(getSeparatorCharacter());
               
               str.append(StringUtilities::leftJustify("Name", longestColumnNameLength));
               if (tabSeparateReportFlag) str.append(getSeparatorCharacter());
               
               str.append(StringUtilities::rightJustify("Average", numberSize));
               if (tabSeparateReportFlag) str.append(getSeparatorCharacter());
               
               str.append(StringUtilities::rightJustify("Sample Deviation", numberSize));
               if (tabSeparateReportFlag) str.append(getSeparatorCharacter());
               
               str.append(StringUtilities::rightJustify("Abs-Average", numberSize));
               if (tabSeparateReportFlag) str.append(getSeparatorCharacter());
               
               str.append(StringUtilities::rightJustify("Sample Abs-Deviation", numberSize));
               if (tabSeparateReportFlag) str.append(getSeparatorCharacter());
               
               str.append(StringUtilities::rightJustify("Minimum", numberSize));
               if (tabSeparateReportFlag) str.append(getSeparatorCharacter());
               
               str.append(StringUtilities::rightJustify("Maximum", numberSize));
               if (tabSeparateReportFlag) str.append(getSeparatorCharacter());
               
               str.append(StringUtilities::rightJustify("Range", numberSize));
               if (tabSeparateReportFlag) str.append(getSeparatorCharacter());

               str.append(StringUtilities::rightJustify("Median", numberSize));
               if (tabSeparateReportFlag) str.append(getSeparatorCharacter());
               
               str.append(StringUtilities::rightJustify("Abs-Median", numberSize));

               reportText.append(str);
               reportText.append("\n");
               firstWrite = false;
            }
            
            QString stats;
            stats.append(StringUtilities::rightJustify(QString::number(j), 
                                                         columnNumberLength));
            stats.append(" ");
            if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
            
            stats.append(StringUtilities::leftJustify(mf->getColumnName(j),
                                                      longestColumnNameLength));
            if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.average, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.standardDeviation, 'f', 6), 
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.absAverage, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.absStandardDeviation, 'f', 6), 
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.minValue, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.maxValue, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.range, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.median, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(getSeparatorCharacter());
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.absMedian, 'f', 6),
                                                      numberSize));
            
            reportText.append(stats);
            reportText.append("\n");
         }
      }
   }   
}


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
#include "BrainModelSurfaceGeodesic.h"
#define __BRAIN_MODEL_SURFACE_REGION_MAIN__
#include "BrainModelSurfaceRegionOfInterest.h"
#undef __BRAIN_MODEL_SURFACE_REGION_MAIN__
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "GeodesicDistanceFile.h"
#include "LatLonFile.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "StatisticsUtilities.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceRegionOfInterest::BrainModelSurfaceRegionOfInterest(BrainSet* bs,
                                                 BrainModelSurface* bmsIn,
                                                 const OPERATION operationIn,
                                                 const std::vector<bool> selectedNodeFlagsIn)
   : BrainModelAlgorithm(bs)
{
   bms = bmsIn;
   operation = operationIn;
   selectedNodeFlags = selectedNodeFlagsIn;
   
   reportLatLonFile = NULL;
   reportMetricFile = NULL;
   reportShapeFile = NULL;
   reportPaintFile = NULL;
   reportMetricCorrectionFile = NULL;
   
   createBorderStartNode = -1;
   createBorderEndNode = -1;
}                                  

/**
 * destructor.
 */
BrainModelSurfaceRegionOfInterest::~BrainModelSurfaceRegionOfInterest()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceRegionOfInterest::execute() throw (BrainModelAlgorithmException)
{
   if (bms == NULL) {
      throw BrainModelAlgorithmException("Surface is NULL.");
   }
   if (bms->getNumberOfNodes() <= 0) {
      throw BrainModelAlgorithmException("Surface contains no nodes.");
   }
   const int numNodesInROI = std::count(selectedNodeFlags.begin(), 
                                        selectedNodeFlags.end(), 
                                        true);
   if (numNodesInROI <= 0) {
      throw BrainModelAlgorithmException("No nodes are selected.");
   }
   
   reportText = "";
   
   switch (operation) {
      case OPERATION_CREATE_BORDER:
         createBorder(numNodesInROI);
         break;
      case OPERATION_SURFACE_XYZ_MEANS_REPORT:
         createSurfaceMeansTextReport();
         break;
      case OPERATION_TEXT_REPORT:
         createTextReport();
         break;
   }
}

/**
 * create the border.
 */
void 
BrainModelSurfaceRegionOfInterest::createBorder(const int numNodesInROI) throw (BrainModelAlgorithmException)
{
   if (numNodesInROI == 1) {
      throw BrainModelAlgorithmException("There is only one node, the starting node, in the ROI.");
   }
   
   const int numNodes = bms->getNumberOfNodes();
   
   createBorderModeBorder.clearLinks();
   
   //
   // Check Inputs
   //
   if (createBorderName.isEmpty()) {
      throw BrainModelAlgorithmException("Name for border is empty.");
   }
   
   if (createBorderStartNode >= 0) {
      if (createBorderStartNode >= numNodes) {
         throw BrainModelAlgorithmException("Starting node is invalid.");
      }
      if (selectedNodeFlags[createBorderStartNode] == false) {
         throw BrainModelAlgorithmException("Starting node is not in the ROI.");
      }
   }
   
   if (createBorderEndNode >= 0) {
      if (createBorderEndNode >= numNodes) {
         throw BrainModelAlgorithmException("Ending node is invalid.");
      }
      if (selectedNodeFlags[createBorderEndNode] == false) {
         throw BrainModelAlgorithmException("Ending node is not in the ROI.");
      }
      if (createBorderStartNode < 0) {
         throw BrainModelAlgorithmException("If the end node is specified, the start node must also be specified");
      }
      if (createBorderStartNode == createBorderEndNode) {
         throw BrainModelAlgorithmException("Starting and ending node are the same.");
      }
   }
   
   //
   // If starting node is not specified
   //
   int iterStart = 2;
   if (createBorderStartNode < 0) {
      //
      // First iteration will find the starting node
      //
      iterStart = 1;
      
      //
      // Just pick the first selected node to use as the starting node
      //
      for (int i = 0; i < numNodes; i++) {
         if (selectedNodeFlags[i]) {
            createBorderStartNode = 1;
            break;
         }
      }
   }
   
   //
   // (Iter == 1) is only performed if a starting node was not specified.  In this
   // iteration, find the node that is furthest from the first selected node which 
   // should be at one end of the sulcus.  Use this node as the start node.
   //
   // (Iter == 2) performs the geodesic calculation from the start node.  If the 
   // end node was not specified, just pick the node furthest from the starting
   // node as the end node.
   //
   for (int iter = iterStart; iter <= 2; iter++) {
      //
      // Determine the geodesic distances
      //
      GeodesicDistanceFile geodesicFile;
      BrainModelSurfaceGeodesic geodesic(brainSet,
                                         bms,
                                         NULL,
                                         -2,
                                         "metric-column-name",
                                         &geodesicFile,
                                         -2,
                                         "geodesic-column-name",
                                         createBorderStartNode,
                                         &selectedNodeFlags);
      geodesic.execute();
      
      //
      // Verify geodesic file
      //
      if ((geodesicFile.getNumberOfNodes() != numNodes) ||
          (geodesicFile.getNumberOfColumns() != 1)) {
         throw BrainModelAlgorithmException("PROGRAM ERROR: Geodesic distance file was not properly created.");
      }
      
      //
      // Find node furthest from start node for this iteration
      //
      int furthestNode = -1;
      float furthestNodeDistance = 0.0;
      for (int i = 0; i < numNodes; i++) {
         const float dist = geodesicFile.getNodeParentDistance(i, 0);
         if (dist > furthestNodeDistance) {
            furthestNodeDistance = dist;
            furthestNode = i;
         }
      }
         
      //
      // If searching for the starting node
      //
      if (iter == 1) {
         createBorderStartNode = furthestNode;
         if (createBorderStartNode < 0) {
            throw BrainModelAlgorithmException("Unable to determine starting node for sulcus.");
         }
         if (DebugControl::getDebugOn()) {
            std::cout << "Starting node is " << createBorderStartNode << std::endl;
         }
      }

      //
      // If finding geodesic path and possibly the end node
      //
      if (iter == 2) {      
         //
         // Was end node NOT specified
         //
         if (createBorderEndNode < 0) {
            createBorderEndNode = furthestNode;
            
            if (createBorderEndNode < 0) {
               throw BrainModelAlgorithmException("Unable to determine the ending node for the border.");
            }
            
            if (DebugControl::getDebugOn()) {
               std::cout << "Ending node is " << createBorderEndNode << std::endl;
            }
         }
         
         
         //
         // Set border name
         //
         createBorderModeBorder.setName(createBorderName);
         
         //
         // Find path for border
         //
         bool done = false;
         int nodeNum = createBorderEndNode;
         const CoordinateFile* coordFile = bms->getCoordinateFile();
         while (done == false) {
            //
            // Add on to border
            //
            float xyz[3];
            coordFile->getCoordinate(nodeNum, xyz);
            createBorderModeBorder.addBorderLink(xyz);
            
            //
            // Next node in geodesic path
            //
            nodeNum = geodesicFile.getNodeParent(nodeNum, 0);
            if ((nodeNum == createBorderStartNode) ||
                (nodeNum < 0)) {
               done = true;
            }
         }
      } // if (iter == 2...
   }  // for (iter...
   
   //
   // resample the border
   //
   if (createBorderSamplingDensity >= 0) {
      int dummyUnused;
      createBorderModeBorder.resampleBorderToDensity(createBorderSamplingDensity,
                                                     2,
                                                     dummyUnused);
   }
   
   //
   // Need to reverse the links since we start from the end node in the geodesic path
   //
   createBorderModeBorder.reverseBorderLinks();
}

/**
 * set create border options.
 */
void 
BrainModelSurfaceRegionOfInterest::setCreateBorderControlsAndOptions(const QString& borderNameIn,
                                                                     const int startNodeIn,
                                                                     const int endNodeIn,
                                                                     const float samplingDensityIn)
{
   createBorderName      = borderNameIn;
   createBorderStartNode = startNodeIn;
   createBorderEndNode   = endNodeIn;
   createBorderSamplingDensity = samplingDensityIn;
}
                                       
/**
 * get the border that was created by create border mode.
 */
Border 
BrainModelSurfaceRegionOfInterest::getBorder() const
{
   return createBorderModeBorder;
}

/**
 * set surface means report controls and options.
 */
void 
BrainModelSurfaceRegionOfInterest::setSurfaceMeansReportControlsAndOptions(std::vector<CoordinateFile*>& coordFilesIn)
{
   surfaceMeansCoordFiles = coordFilesIn;
}
                                              
/**
 * create the surface means text report.
 */
void 
BrainModelSurfaceRegionOfInterest::createSurfaceMeansTextReport() throw (BrainModelAlgorithmException)
{
   const int numCoordFiles = static_cast<int>(surfaceMeansCoordFiles.size());
   if (numCoordFiles <= 0) {
      throw BrainModelAlgorithmException("ERROR: There are no coord files.");
   }
   
   //
   // Determine surface means
   //
   const int numNodes = brainSet->getNumberOfNodes();
   for (int j = 0; j < numCoordFiles; j++) {
      double meanX = 0.0;
      double meanY = 0.0;
      double meanZ = 0.0;
      double nodeCount = 0.0;
      
      const CoordinateFile* coordFile = surfaceMeansCoordFiles[j];
      for (int i = 0; i < numNodes; i++) {
         if (selectedNodeFlags[i]) {
            float xyz[3];
            coordFile->getCoordinate(i, xyz);
            meanX += xyz[0];
            meanY += xyz[1];
            meanZ += xyz[2];
            nodeCount += 1.0;
         }
      }
      
      meanX /= nodeCount;
      meanY /= nodeCount;
      meanZ /= nodeCount;
      
      const QString s = 
         (FileUtilities::basename(coordFile->getFileName())
          + " "
          + QString::number(meanX, 'f', 6)
          + " "
          + QString::number(meanY, 'f', 6)
          + " "
          + QString::number(meanZ, 'f', 6)
          + "\n");
          
      reportText.append(s);
   }
}
      
/**
 * set report controls and options.
 */
void 
BrainModelSurfaceRegionOfInterest::setTextReportControlsAndOptions(
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
{
   reportMetricFile = metricFileIn;
   selectedMetricColumnsForReport = selectedMetricColumnsForReportIn;
   reportShapeFile = shapeFileIn;
   selectedShapeColumnsForReport  = selectedShapeColumnsForReportIn;
   reportPaintFile = paintFileIn;
   selectedPaintColumnsForReport  = selectedPaintColumnsForReportIn;
   reportLatLonFile = latLonFileIn;
   reportLatLonFileColumn = latLonFileColumnIn;
   nodeSelectionText = nodeSelectionTextIn;
   headerText = headerTextIn;
   reportMetricCorrectionFile = metricCorrectionFileIn;
   metricCorrectionColumn = metricCorrectionColumnIn;
   tabSeparateReportFlag = tabSeparateReportFlagIn;
}

/**
 * create the text report.
 */
void 
BrainModelSurfaceRegionOfInterest::createTextReport() throw (BrainModelAlgorithmException)
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
BrainModelSurfaceRegionOfInterest::paintReport(const float roiArea)
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
                  if (selectedNodeFlags[tileNodes[k]]) {
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
            if (selectedNodeFlags[i]) {           
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
                  if (tabSeparateReportFlag) line.append(separatorCharacter);
   
                  line.append(StringUtilities::leftJustify("Paint Name", 
                                                           longestPaintNameLength));
                  if (tabSeparateReportFlag) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("Area",
                                                            numberSize));
                  if (tabSeparateReportFlag) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("Percent Area",
                                                            numberSize));                                                            
                  if (tabSeparateReportFlag) line.append(separatorCharacter);
                  
                  if (metricCorrectionColumn >= 0) {
                     line.append(StringUtilities::rightJustify("Area-Corr",
                                                               numberSize));
                     if (tabSeparateReportFlag) line.append(separatorCharacter);
                  }
                  
                  line.append(StringUtilities::rightJustify("COG - X",
                                                            numberSize));
                  if (tabSeparateReportFlag) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("COG - Y",
                                                            numberSize));
                  if (tabSeparateReportFlag) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("COG - Z",
                                                            numberSize));
                  if (tabSeparateReportFlag) line.append(separatorCharacter);
                  
                  line.append(StringUtilities::rightJustify("Latitude",
                                                            numberSize));
                  if (tabSeparateReportFlag) line.append(separatorCharacter);
                  
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
               if (tabSeparateReportFlag) stats.append(separatorCharacter);
               
               stats.append(StringUtilities::leftJustify(reportPaintFile->getPaintNameFromIndex(m),
                                                         longestPaintNameLength));
               if (tabSeparateReportFlag) stats.append(separatorCharacter);
               
               stats.append(StringUtilities::rightJustify(QString::number(paintNameAreas[m], 'f', 3),
                                                          numberSize));
               if (tabSeparateReportFlag) stats.append(separatorCharacter);
                              
               const double percent = (paintNameAreas[m] / roiArea) * 100.0;
               stats.append(StringUtilities::rightJustify(QString::number(percent, 'f', 3),
                                                          numberSize));
               if (tabSeparateReportFlag) stats.append(separatorCharacter);
      
               if (metricCorrectionColumn >= 0) {
                  stats.append(StringUtilities::rightJustify(QString::number(paintNameAreasCorrected[m], 'f', 3),
                                                             numberSize));
                  if (tabSeparateReportFlag) stats.append(separatorCharacter);
                }

               const double numNodesForPaint = paintNameNodeCounts[m];
               
               double cogX = 0.0;
               if (numNodesForPaint > 0) {
                  cogX = paintNameAreasCogX[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogX, 'f', 3),
                                                          numberSize));
               
               if (tabSeparateReportFlag) stats.append(separatorCharacter);
               double cogY = 0.0;
               if (numNodesForPaint > 0) {
                  cogY = paintNameAreasCogY[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogY, 'f', 3),
                                                          numberSize));

               if (tabSeparateReportFlag) stats.append(separatorCharacter);
               double cogZ = 0.0;
               if (numNodesForPaint > 0) {
                  cogZ = paintNameAreasCogZ[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogZ, 'f', 3),
                                                          numberSize));
                                                          
               if (tabSeparateReportFlag) stats.append(separatorCharacter);
               double cogLat = 0.0;
               if (numNodesForPaint > 0) {
                  cogLat = paintNameAreasLat[m] / numNodesForPaint;
               }
               stats.append(StringUtilities::rightJustify(QString::number(cogLat, 'f', 3),
                                                          numberSize));
                                                          
               if (tabSeparateReportFlag) stats.append(separatorCharacter);
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
BrainModelSurfaceRegionOfInterest::metricAndSurfaceShapeReport(const bool metricFlag)
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
            if (selectedNodeFlags[i]) { 
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
               if (tabSeparateReportFlag) str.append(separatorCharacter);
               
               str.append(StringUtilities::leftJustify("Name", longestColumnNameLength));
               if (tabSeparateReportFlag) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Average", numberSize));
               if (tabSeparateReportFlag) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Sample Deviation", numberSize));
               if (tabSeparateReportFlag) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Abs-Average", numberSize));
               if (tabSeparateReportFlag) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Sample Abs-Deviation", numberSize));
               if (tabSeparateReportFlag) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Minimum", numberSize));
               if (tabSeparateReportFlag) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Maximum", numberSize));
               if (tabSeparateReportFlag) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Range", numberSize));
               if (tabSeparateReportFlag) str.append(separatorCharacter);

               str.append(StringUtilities::rightJustify("Median", numberSize));
               if (tabSeparateReportFlag) str.append(separatorCharacter);
               
               str.append(StringUtilities::rightJustify("Abs-Median", numberSize));

               reportText.append(str);
               reportText.append("\n");
               firstWrite = false;
            }
            
            QString stats;
            stats.append(StringUtilities::rightJustify(QString::number(j), 
                                                         columnNumberLength));
            stats.append(" ");
            if (tabSeparateReportFlag) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::leftJustify(mf->getColumnName(j),
                                                      longestColumnNameLength));
            if (tabSeparateReportFlag) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.average, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.standardDeviation, 'f', 6), 
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.absAverage, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.absStandardDeviation, 'f', 6), 
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.minValue, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.maxValue, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.range, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.median, 'f', 6),
                                                      numberSize));
            if (tabSeparateReportFlag) stats.append(separatorCharacter);
            
            stats.append(StringUtilities::rightJustify(QString::number(statistics.absMedian, 'f', 6),
                                                      numberSize));
            
            reportText.append(stats);
            reportText.append("\n");
         }
      }
   }   
}

/**
 * Create the report header.
 */
void 
BrainModelSurfaceRegionOfInterest::createReportHeader(float& roiAreaOut)
{
   //
   // Add the header describing the node selection
   //
   reportText.append(nodeSelectionText);
   reportText.append("\n");
   
   const TopologyFile* tf = bms->getTopologyFile();
   
   const int numNodes = bms->getNumberOfNodes();
   
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
      if (selectedNodeFlags[nodes[0]]) numMarked += 1.0;
      if (selectedNodeFlags[nodes[1]]) numMarked += 1.0;
      if (selectedNodeFlags[nodes[2]]) numMarked += 1.0;
      
      if (tileArea[i] > 0.0) {
         roiAreaOut += (numMarked / 3.0) * tileArea[i];
      }
      tileInROI[i] = (numMarked > 0.0);
   }
   
   for (int m = 0; m < numNodes; m++) {
      if (selectedNodeFlags[m]) {
         const float* xyz = cf->getCoordinate(m);
         centerOfGravity[0] += xyz[0];
         centerOfGravity[1] += xyz[1];
         centerOfGravity[2] += xyz[2];
      }
   }
      
   if (headerText.isEmpty() == false) {
      reportText.append(headerText);
      reportText.append("\n");
   }
   QString surf("Surface: ");
   surf.append(bms->getDescriptiveName());
   surf.append("\n");
   reportText.append(surf);
   reportText.append("\n");
   
   QString topo("Topology: ");
   topo.append(tf->getDescriptiveName());
   topo.append("\n");
   reportText.append(topo);
   reportText.append("\n");
   
   reportText.append("\n");
   const int count = std::count(selectedNodeFlags.begin(), selectedNodeFlags.end(), true);
   QStringList sl;
   sl << QString::number(count) << " of " << QString::number(numNodes) << " nodes in region of interest\n";
   reportText.append(sl.join(""));
   reportText.append("\n");

   sl.clear();
   sl << "Total Surface Area: " << QString::number(totalArea, 'f', 1);
   reportText.append(sl.join(""));
   reportText.append("\n");
   
   sl.clear();
   sl << "Region of Interest Surface Area: " << QString::number(roiAreaOut, 'f', 1);
   reportText.append(sl.join(""));
   reportText.append("\n");
   
   centerOfGravity[0] = centerOfGravity[0] / static_cast<float>(count);
   centerOfGravity[1] = centerOfGravity[1] / static_cast<float>(count);
   centerOfGravity[2] = centerOfGravity[2] / static_cast<float>(count);
   sl.clear();
   sl << "Region of Interest Center of Gravity: " 
       << QString::number(centerOfGravity[0], 'f', 4) << " "
       << QString::number(centerOfGravity[1], 'f', 4) << " "
       << QString::number(centerOfGravity[2], 'f', 4);
   reportText.append(sl.join(""));
   reportText.append("\n");
   
   sl.clear();
   sl << "Region Mean Distance Between Nodes: "
       << QString::number(bms->getMeanDistanceBetweenNodes(&selectedNodeFlags), 'f', 5);
   reportText.append(sl.join(""));
   reportText.append("\n");
   reportText.append(" \n");
}                                                     
      

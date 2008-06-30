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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricClustering.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROIMetricClusterReport.h"
#include "MetricFile.h"
#include "SurfaceShapeFile.h"

/**
 * Constructor.
 */
BrainModelSurfaceROIMetricClusterReport::BrainModelSurfaceROIMetricClusterReport(
                                       BrainSet* bs,
                                       BrainModelSurface* bmsIn,
                                       BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                       MetricFile* metricOrShapeFileIn,
                                       const std::vector<bool>& columnSelectedFlagsIn,
                                       MetricFile* distortionMetricFileIn,
                                       const int distortionMetricFileColumnIn,
                                       const float metricThresholdValueIn,
                                       const bool separateWithSemicolonsFlagIn)
   : BrainModelSurfaceROIOperation(bs,
                                   bmsIn,
                                   surfaceROIIn)
{
   metricFile = metricOrShapeFileIn;
   columnSelectedFlags = columnSelectedFlagsIn;
   distortionMetricFile = distortionMetricFileIn;
   distortionMetricFileColumn = distortionMetricFileColumnIn;
   metricThresholdValue = metricThresholdValueIn;
   separateWithSemicolonsFlag = separateWithSemicolonsFlagIn;
}
                                 
/**
 * destructor.
 */
BrainModelSurfaceROIMetricClusterReport::~BrainModelSurfaceROIMetricClusterReport()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROIMetricClusterReport::executeOperation() throw (BrainModelAlgorithmException)
{
   reportText = "";
   QString fileTypeName("Metric");
   if (dynamic_cast<SurfaceShapeFile*>(metricFile) != NULL) {
      fileTypeName = "Shape";
   }
   setHeaderText(fileTypeName + " Cluster Report");
   
   const int numColumns = metricFile->getNumberOfColumns();
   if (numColumns <= 0) {
      throw BrainModelAlgorithmException(fileTypeName + " file contains no data.");
   }

   QString separator(" ");
   if (separateWithSemicolonsFlag) {
      separator = ";";
   }
   float roiArea = 0.0;
   createReportHeader(roiArea);
   
   const int numNodes = bms->getNumberOfNodes();
   
   //
   // Report header
   //
   QString str =
      ("Threshold" + separator
       + "Column" + separator
       +  "Num-Nodes" + separator
       +  "Area " + separator
       +  "Area Corrected" + separator
       +  "COG-X" + separator
       +  "COG-Y" + separator
       +  "COG-Z"
       + "\n");
   reportText.append(str);
   
   float posMin = 0.0;
   float posMax = 0.0;
   float negMin = 0.0;
   float negMax = 0.0;
   
   if (metricThresholdValue >= 0.0) {
      posMin = metricThresholdValue;
      posMax = std::numeric_limits<float>::max();
      negMin = -1.0;
      negMax =  0.0;
   }
   else {
      posMin =  1.0;
      posMax =  0.0;
      negMin =  metricThresholdValue;
      negMax = -std::numeric_limits<float>::max();
   }
   
   //
   // Process each column
   //
   for (int i = 0; i < numColumns; i++) {
      //
      // Ignore deselected shape columns
      //
      if (columnSelectedFlags[i] == false) {
         continue;
      }
      
      //
      // Create a shape file containing just the single column
      //
      MetricFile tempMetricFile;
      tempMetricFile.setNumberOfNodesAndColumns(numNodes, 1);
      std::vector<float> nodeValues;
      metricFile->getColumnForAllNodes(i, nodeValues);
      tempMetricFile.setColumnForAllNodes(0, nodeValues);
      
      //
      // Find the clusters
      //
      BrainModelSurfaceMetricClustering bmsmc(brainSet,
                                              bms,
                                              &tempMetricFile,
                                              BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA,
                                              0,
                                              0,
                                              "cluster",
                                              1,
                                              0.001,
                                              negMin,
                                              negMax,
                                              posMin,
                                              posMax,
                                              true);
      try {
         bmsmc.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         throw BrainModelAlgorithmException(e.whatQString());
      }
      
      //
      // Node areas
      //
      std::vector<float> nodeAreas;
      bms->getAreaOfAllNodes(nodeAreas);
      
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
         if ((distortionMetricFileColumn >= 0) &&
             (distortionMetricFile != NULL)) {
            for (int k = 0; k < numNodesInCluster; k++) {
               const int nodeNum = cluster->getNodeInCluster(k);
               float nodeArea = nodeAreas[nodeNum];
               const double metric = distortionMetricFile->getValue(nodeNum, distortionMetricFileColumn);
               correctedArea += (nodeArea * std::pow(2.0, metric));
            }
         }
         else {
            correctedArea = cluster->getArea();
         }
         
         //
         // Add to the report
         //
         float cog[3];
         cluster->getCenterOfGravity(cog);
         QString str =
             (QString::number(metricThresholdValue, 'f', 5) + separator
              + QString::number(i + 1)  + separator
              + QString::number(numNodesInCluster)  + separator
              + QString::number(cluster->getArea(), 'f', 5)  + separator
              + QString::number(correctedArea, 'f', 5) + separator
              + QString::number(cog[0], 'f', 5) + separator
              + QString::number(cog[1], 'f', 5) + separator
              + QString::number(cog[2], 'f', 5)
              + "\n");
         reportText.append(str);
      }
   }
}
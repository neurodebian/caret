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
#include <limits>
#include <sstream>
#include <limits>

#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricOneAndPairedTTest.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "PaintFile.h"
#include "MetricFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceMetricOneAndPairedTTest::BrainModelSurfaceMetricOneAndPairedTTest(BrainSet* bs,
                                const T_TEST_MODE tTestModeIn,
                                const std::vector<QString>& metricFileNamesIn,
                                const QString& fiducialCoordFileNameIn,
                                const QString& openTopoFileNameIn,
                                const QString& areaCorrectionMetricFileNameIn,
                                const QString& tMapFileNameIn,
                                const QString& permutedTMapFileNameIn,
                                const QString& clustersPaintFileNameIn,
                                const QString& clustersMetricFileNameIn,
                                const QString& reportFileNameIn,
                                const int areaCorrectionMetricFileColumnIn,
                                const float negativeThreshIn,
                                const float positiveThreshIn,
                                const float pValueIn,
                                const int tVarianceSmoothingIterationsIn,
                                const float tVarianceSmoothingStrengthIn,
                                const int permutationIterationsIn,
                                const float tTestConstantIn,
                                const int numberOfThreadsIn)

   : BrainModelSurfaceMetricFindClustersBase(bs,
                                            fiducialCoordFileNameIn,
                                            openTopoFileNameIn,
                                            areaCorrectionMetricFileNameIn,
                                            tMapFileNameIn,
                                            permutedTMapFileNameIn,
                                            clustersPaintFileNameIn,
                                            clustersMetricFileNameIn,
                                            reportFileNameIn,
                                            areaCorrectionMetricFileColumnIn,
                                            negativeThreshIn,
                                            positiveThreshIn,
                                            pValueIn,
                                            tVarianceSmoothingIterationsIn,
                                            tVarianceSmoothingStrengthIn,
                                            false,
                                            false,
                                            numberOfThreadsIn)
{
   tTestMode = tTestModeIn;
   metricFileNames = metricFileNamesIn;
   tTestConstant  = tTestConstantIn;
   permutationIterations = permutationIterationsIn;
}

/**
 * destructor.
 */
BrainModelSurfaceMetricOneAndPairedTTest::~BrainModelSurfaceMetricOneAndPairedTTest()
{
}

/**
 * must be implemented by subclasses.
 */
void 
BrainModelSurfaceMetricOneAndPairedTTest::executeClusterSearch() throw (BrainModelAlgorithmException)
{
   //
   // Steps in algorithm
   //
   enum {
      ALG_STEP_CHECKING_INPUT,
      ALG_STEP_TMAP,
      ALG_STEP_PERMUTED_TMAP,
      ALG_STEP_FINDING_CLUSTERS_T_MAP,
      ALG_STEP_FINDING_CLUSTERS_PERMUTED_T_MAP,
      ALG_NUM_STEPS
   };

   //
   // Update progress
   //
   createProgressDialog("Metric/Shape One Sample T-Test",
                        ALG_NUM_STEPS,
                        "oneSampProgress");
   updateProgressDialog("Verifying Input",
                        ALG_STEP_CHECKING_INPUT,
                        ALG_NUM_STEPS);
                        
   //
   // check iterations
   //
   if (permutationIterations < 0) {
      throw BrainModelAlgorithmException("Permutation iterations must be positive.");
   }

   //
   // Metric file used for processing
   //   
   MetricFile metricFile;

   //
   // Check the mode
   //
   switch (tTestMode) {
      case T_TEST_MODE_ONE_SAMPLE:
         oneSampleTTestProcessing(metricFile);
         break;
      case T_TEST_MODE_PAIRED:
         pairedTTestProcessing(metricFile);
         break;
   }
   
   //
   // Get number of columns and limit iterations to 2**numCols
   //
   const int numCols = metricFile.getNumberOfColumns();
   int maxIters = std::numeric_limits<int>::max();
   const double maxItersDouble = std::pow(2.0, numCols);
   if (maxItersDouble < static_cast<double>(maxIters)) {
      maxIters = static_cast<int>(maxItersDouble);
   }
   permutationIterations = std::min(permutationIterations, maxIters);
   
   //
   // Number of nodes in surface
   //
   const int numberOfNodes = bms->getNumberOfNodes();
   
   //
   // Check file compatibilities
   //
   if (numberOfNodes != metricFile.getNumberOfNodes()) {
      throw BrainModelAlgorithmException("Metric/shape file has different number of nodes"
                                         "than the coordinate file.");
   }
   if (numberOfNodes != areaCorrectionShapeFile->getNumberOfNodes()) {
      throw BrainModelAlgorithmException("Area correction shape file has different number of nodes"
                                         "than the coordinate file.");
   }
   
   //
   // Update progress
   //
   updateProgressDialog("Doing T-Map",
                        ALG_STEP_TMAP,
                        ALG_NUM_STEPS);
   //
   // Create T-Map
   //
   try {
      statisticalMapShapeFile = metricFile.computeTValues(tTestConstant,
                                                brain->getTopologyFile(0),
                                                tVarianceSmoothingIterations,
                                                tVarianceSmoothingStrength);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "T-Value failure: "
          << e.whatQString().toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Write T-Map file
   //
   try {
      statisticalMapShapeFile->writeFile(statisticalMapFileName);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Unable to write T-Map: "
          << FileUtilities::basename(statisticalMapFileName).toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   
   //
   // Update progress
   //
   updateProgressDialog("Doing Permuted T-Map",
                        ALG_STEP_PERMUTED_TMAP,
                        ALG_NUM_STEPS);
                        
   //
   // Do shuffled T-Map
   //
   try {
      shuffleStatisticalMapShapeFile = metricFile.computePermutedTValues(tTestConstant,
                                                               permutationIterations,
                                                               brain->getTopologyFile(0),
                                                               tVarianceSmoothingIterations,
                                                               tVarianceSmoothingStrength);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Permuted T-Map failure: "
          << e.whatQString().toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }

   //
   // Write Shuffled  T-Map file
   //
   try {
      shuffleStatisticalMapShapeFile->writeFile(shuffledStatisticalMapFileName);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Unable to write Permuted T-Map: "
          << FileUtilities::basename(shuffledStatisticalMapFileName).toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }

   //
   // Update progress
   //
   updateProgressDialog("Finding clusters in T-Map",
                        ALG_STEP_FINDING_CLUSTERS_T_MAP,
                        ALG_NUM_STEPS);
                        
   //
   // find the clusters in T-Map
   // Note: column 0 is the T-Map
   //
   std::vector<Cluster> tMapClusters;
   findClusters(statisticalMapShapeFile, tMapClusters, "Finding Clusters in T-Map", 0, false);
      
   //
   // Update progress
   //
   updateProgressDialog("Finding Clusters in Permuted T-Map",
                        ALG_STEP_FINDING_CLUSTERS_PERMUTED_T_MAP,
                        ALG_NUM_STEPS);
                        
   //
   // find the clusters in Shuffled T-Map
   // Note: Only use largest cluster from each column
   //
   std::vector<Cluster> shuffleTMapClusters;
   findClusters(shuffleStatisticalMapShapeFile, shuffleTMapClusters, "Finding Clusters in Permuted T-Map", -1, true);
   
   //
   // Find area of the "P-Value" cluster in the permuted T-Map
   //
   float significantCorrectedArea = std::numeric_limits<float>::max();
   int pValueClusterIndex = -1;
   if (shuffleTMapClusters.empty() == false) {
      pValueClusterIndex = std::min(static_cast<int>(pValue * permutationIterations) - 1,
                                    static_cast<int>(shuffleTMapClusters.size()));
      pValueClusterIndex = std::max(pValueClusterIndex, 0);
   }
   if (pValueClusterIndex >= 0) {
      significantCorrectedArea = shuffleTMapClusters[pValueClusterIndex].areaCorrected;
   }
   
   //
   // Find P-Value for significant clusters in T-Map
   //
   for (unsigned int i = 0; i < tMapClusters.size(); i++) {
      Cluster& tMapCluster = tMapClusters[i];
      int cnt = shuffleTMapClusters.size() - 1;
      if (shuffleTMapClusters.empty() == false) {
         if (tMapCluster.areaCorrected > shuffleTMapClusters[0].areaCorrected) {
            cnt = 0;
         }
         else {
            for (unsigned int j = 0; j < shuffleTMapClusters.size() - 1; j++) {
               if ((tMapCluster.areaCorrected < shuffleTMapClusters[j].areaCorrected) &&
                   (tMapCluster.areaCorrected >= shuffleTMapClusters[j+1].areaCorrected)) {
                  cnt = j;
               }
            }
         }
      }
      cnt = std::min(cnt, permutationIterations);
      tMapCluster.pValue = static_cast<float>(cnt)
                         / static_cast<float>(permutationIterations);
   }
   
   //
   // So only do column 0 in T-Map when searching for T-Map clusters
   //
   //
   // For each T-Map that has area corrected that exceeds significant area
   //    Find its rank in shuffled tmap
   //    P-value = rank / iterations
   //
   // Option for paint file to show the clusters
   //
   // Open the report file
   //
   QFile reportFile(reportFileName);
   if (reportFile.open(QIODevice::WriteOnly) == false) {
      std::ostringstream str;
      str << "Unable to open report file for writing: "
          << FileUtilities::basename(reportFileName).toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Create the text stream
   //
   QTextStream reportStream(&reportFile);
   
   //
   // Show area and thresholds
   //
   reportStream << "Date/Time:           " << QDateTime::currentDateTime().toString("MMM d, yyyy hh:mm:ss") << "\n";
   reportStream << "Metric/Shape:        " << metricFileNames[0] << "\n";
   switch (tTestMode) {
      case T_TEST_MODE_ONE_SAMPLE:
         break;
      case T_TEST_MODE_PAIRED:
         reportStream << "Metric/Shape Two:    " << metricFileNames[1] << "\n";
         break;
   }
   reportStream << "Fiducial Coord File: " << fiducialCoordFileName << "\n";
   reportStream << "Open Topo File:      " << openTopoFileName << "\n";
   if ((areaCorrectionShapeFile != NULL) ||
       (areaCorrectionShapeFileColumn >= 0)) {
      reportStream << "Area Correct File:   " << areaCorrectionShapeFileName << "\n";
      reportStream << "Area Correct Column: " << areaCorrectionShapeFile->getColumnName(areaCorrectionShapeFileColumn) << "\n";
   }
   reportStream << "Negative Threshold:  " << negativeThresh << "\n";
   reportStream << "Positive Threshold:  " << positiveThresh << "\n";
   reportStream << "Permutation Iterations: " << permutationIterations << "\n";
   reportStream << "P-Value:             " << pValue << "\n";
   reportStream << "Significant Area:    " << significantCorrectedArea << "\n";
   reportStream << "\n";
   
   //
   // Add significant clusters to report file
   //
   reportStream << "Permuted TMap" << "\n";
   reportStream << "-------------" << "\n";
   printClusters(reportStream, shuffleTMapClusters, significantCorrectedArea);
   reportStream << "\n\n\n";
   reportStream << "TMap" << "\n";
   reportStream << "----" << "\n";
   printClusters(reportStream, tMapClusters, significantCorrectedArea);
   
   reportStream << "\n\n\n";
   
   //
   // Add all clusters to report file
   //
   reportStream << "Permuted TMap" << "\n";
   reportStream << "-------------" << "\n";
   printClusters(reportStream, shuffleTMapClusters);
   reportStream << "\n\n\n";
   reportStream << "TMap" << "\n";
   reportStream << "----" << "\n";
   printClusters(reportStream, tMapClusters);
   
   //
   // Close the report file
   //
   reportFile.close();
   
   //
   // Do the paint file
   //
   createClustersPaintFile(tMapClusters, significantCorrectedArea, numberOfNodes);
      
   //
   // Do the clusters metric file
   //
   const int tValueColumn = 0;
   createClustersMetricFile(tMapClusters, tValueColumn, numberOfNodes);

   //
   // Do cluster reports
   //
   for (unsigned int i = 0; i < metricFileNames.size(); i++) {
      createMetricShapeClustersReportFile(tMapClusters,
                                          metricFileNames[i]);
   }
}

/**
 * create the metric file for one-sample T-Test processing.
 */
void 
BrainModelSurfaceMetricOneAndPairedTTest::oneSampleTTestProcessing(MetricFile& metricFileOut) throw (BrainModelAlgorithmException)
{
   //
   // Verify metric files are present
   //
   if (metricFileNames.size() < 1) {
      throw BrainModelAlgorithmException("One metric/shape file must be provided.");
   }
   
   //
   // Read the single metric file
   //
   try {
      metricFileOut.readFile(metricFileNames[0]);
   }
   catch (FileException &e) {
      std::ostringstream str;
      str << "Unable to read metric/shape file: "
          << FileUtilities::basename(metricFileNames[0]).toAscii().constData();
      throw BrainModelAlgorithmException(str.str().c_str());
   }   
}

/**
 * create the metric file for two-sample T-Test processing.
 */
void 
BrainModelSurfaceMetricOneAndPairedTTest::pairedTTestProcessing(MetricFile& metricFileOut) throw (BrainModelAlgorithmException)
{
   //
   // Verifty metric files are present
   //
   if (metricFileNames.size() < 2) {
      throw BrainModelAlgorithmException("Two metric/shape files must be provided.");
   }
   
   //
   // Read the two metric files
   //
   MetricFile mf1, mf2;
   try {
      mf1.readFile(metricFileNames[0]);
      mf2.readFile(metricFileNames[1]);
   }
   catch (FileException &e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   if (mf1.getNumberOfColumns() != mf2.getNumberOfColumns()) {
      throw BrainModelAlgorithmException("The two input metric/shape files have a different number of columns.");
   }
   if (mf1.getNumberOfNodes() != mf2.getNumberOfNodes()) {
      throw BrainModelAlgorithmException("The two input metric/shape files have a different number of nodes.");
   }
   
   //
   // Load the metricFile with the subtraction of the two input metric files
   //
   const int numCols = mf1.getNumberOfColumns();
   const int numNodes = mf1.getNumberOfNodes();
   metricFileOut.setNumberOfNodesAndColumns(numNodes, numCols);
   metricFileOut.setFileComment("Subtraction of " +
                         FileUtilities::basename(mf1.getFileName()) +
                         " minus " +
                         FileUtilities::basename(mf2.getFileName()));
   for (int j = 0; j < numCols; j++) {
      metricFileOut.setColumnName(j, mf1.getColumnName(j));
   }
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         metricFileOut.setValue(i, j, (mf1.getValue(i, j) - mf2.getValue(i, j)));
      }
   }
}
      
/**
 * free memory.
 */
void 
BrainModelSurfaceMetricOneAndPairedTTest::cleanUp()
{
   BrainModelSurfaceMetricFindClustersBase::cleanUp();
}

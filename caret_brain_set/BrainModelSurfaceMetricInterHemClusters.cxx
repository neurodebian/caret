
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

#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "BrainModelSurfaceMetricInterHemClusters.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "StatisticRandomNumber.h"

/**
 * constructor.
 */
BrainModelSurfaceMetricInterHemClusters::BrainModelSurfaceMetricInterHemClusters(
                                          BrainSet* bs,
                                          const QString& shapeFileRightANameIn,
                                          const QString& shapeFileRightBNameIn,
                                          const QString& shapeFileLeftANameIn,
                                          const QString& shapeFileLeftBNameIn,
                                          const QString& fiducialCoordFileNameIn,
                                          const QString& openTopoFileNameIn,
                                          const QString& areaCorrectionShapeFileNameIn,
                                          const QString& rightTMapFileNameIn,
                                          const QString& leftTMapFileNameIn,
                                          const QString& rightShuffledTMapFileNameIn,
                                          const QString& leftShuffledTMapFileNameIn,
                                          const QString& tMapFileNameIn,
                                          const QString& shuffledTMapFileNameIn,
                                          const QString& clustersPaintFileNameIn,
                                          const QString& clustersMetricFileNameIn,
                                          const QString& reportFileNameIn,
                                          const int areaCorrectionShapeFileColumnIn,
                                          const int iterationsShuffledTMapIn,
                                          const int iterationsRightLeftShuffledTMapIn,
                                          const float negativeThreshIn,
                                          const float positiveThreshIn,
                                          const float pValueIn,
                                          const int tVarianceSmoothingIterationsIn,
                                          const float tVarianceSmoothingStrengthIn,
                                          const bool doTMapDOFIn,
                                          const bool doTMapPValueIn,
                                          const int numberOfThreadsIn)
   : BrainModelSurfaceMetricFindClustersBase(bs,
                                            fiducialCoordFileNameIn,
                                            openTopoFileNameIn,
                                            areaCorrectionShapeFileNameIn,
                                            tMapFileNameIn,
                                            shuffledTMapFileNameIn,
                                            clustersPaintFileNameIn,
                                            clustersMetricFileNameIn,
                                            reportFileNameIn,
                                            areaCorrectionShapeFileColumnIn,
                                            negativeThreshIn,
                                            positiveThreshIn,
                                            pValueIn,
                                            tVarianceSmoothingIterationsIn,
                                            tVarianceSmoothingStrengthIn,
                                            doTMapDOFIn,
                                            doTMapPValueIn,
                                            numberOfThreadsIn)
{
   shapeFileRightAName = shapeFileRightANameIn;
   shapeFileRightBName = shapeFileRightBNameIn;
   shapeFileLeftAName  = shapeFileLeftANameIn;
   shapeFileLeftBName  = shapeFileLeftBNameIn;
   rightTMapFileName   = rightTMapFileNameIn;
   leftTMapFileName    = leftTMapFileNameIn;
   rightShuffledTMapFileName = rightShuffledTMapFileNameIn;
   leftShuffledTMapFileName = leftShuffledTMapFileNameIn;
   iterationsShuffledTMap = iterationsShuffledTMapIn;
   iterationsRightLeftShuffledTMap = iterationsRightLeftShuffledTMapIn;

   leftTMapShapeFile = NULL;
   rightTMapShapeFile = NULL;
   leftShuffledTMapShapeFile = NULL;
   rightShuffledTMapShapeFile = NULL;
}
      
/**
 * destructor.
 */
BrainModelSurfaceMetricInterHemClusters::~BrainModelSurfaceMetricInterHemClusters()
{
   cleanUp();
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceMetricInterHemClusters::executeClusterSearch() throw (BrainModelAlgorithmException)
{
   //
   // Steps in algorithm
   //
   enum {
      ALG_STEP_CHECKING_INPUT,
      ALG_STEP_TMAP_LEFT,
      ALG_STEP_TMAP_RIGHT,
      ALG_STEP_TMAP_PRODUCT,
      ALG_STEP_SHUFFLED_TMAP_LEFT,
      ALG_STEP_SHUFFLED_TMAP_RIGHT,
      ALG_STEP_SHUFFLED_TMAP_PRODUCT,
      ALG_STEP_FINDING_CLUSTERS_T_MAP,
      ALG_STEP_FINDING_CLUSTERS_SHUFFLE_T_MAP,
      ALG_NUM_STEPS
   };
   
   //
   // Update progress
   //
   createProgressDialog("Surface Shape Interhemispheric Clusters",
                        ALG_NUM_STEPS,
                        "sifClustDialog");
   updateProgressDialog("Verifying Input",
                        ALG_STEP_CHECKING_INPUT,
                        ALG_NUM_STEPS);
                        
   //
   // check iterations
   //
   if (iterationsShuffledTMap < 0) {
      throw BrainModelAlgorithmException("Iterations for shuffled T-Map product be positive.");
   }
   if (iterationsRightLeftShuffledTMap < 0) {
      throw BrainModelAlgorithmException("Iterations for left/right shuffled T-Map be positive.");
   }
   
   //
   // Read the surface shape right A file
   //
   MetricFile ssfRightA;
   try {
      ssfRightA.readFile(shapeFileRightAName);
   }
   catch (FileException &e) {
      std::ostringstream str;
      str << "Unable to read surface shape file Right A: "
          << FileUtilities::basename(shapeFileRightAName).toAscii().constData();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Read the surface shape right B file
   //
   MetricFile ssfRightB;
   try {
      ssfRightB.readFile(shapeFileRightBName);
   }
   catch (FileException &e) {
      std::ostringstream str;
      str << "Unable to read surface shape file Right B: "
          << FileUtilities::basename(shapeFileRightBName).toAscii().constData();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Read the surface shape left A file
   //
   MetricFile ssfLeftA;
   try {
      ssfLeftA.readFile(shapeFileLeftAName);
   }
   catch (FileException &e) {
      std::ostringstream str;
      str << "Unable to read surface shape file Left A: "
          << FileUtilities::basename(shapeFileLeftAName).toAscii().constData();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Read the surface shape left B file
   //
   MetricFile ssfLeftB;
   try {
      ssfLeftB.readFile(shapeFileLeftBName);
   }
   catch (FileException &e) {
      std::ostringstream str;
      str << "Unable to read surface shape file Left B: "
          << FileUtilities::basename(shapeFileLeftBName).toAscii().constData();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   
   //
   // Number of nodes in surface
   //
   const int numberOfNodes = bms->getNumberOfNodes();
   
   //
   // Check file compatibilities
   //
   if (numberOfNodes != ssfRightA.getNumberOfNodes()) {
      throw BrainModelAlgorithmException("Right shape file A has different number of nodes"
                                         "than the coordinate file.");
   }
   if (numberOfNodes != ssfRightB.getNumberOfNodes()) {
      throw BrainModelAlgorithmException("Right shape file B has different number of nodes"
                                         "than the coordinate file.");
   }
   if (numberOfNodes != ssfLeftA.getNumberOfNodes()) {
      throw BrainModelAlgorithmException("Left shape file A has different number of nodes"
                                         "than the coordinate file.");
   }
   if (numberOfNodes != ssfLeftB.getNumberOfNodes()) {
      throw BrainModelAlgorithmException("Left shape file B has different number of nodes"
                                         "than the coordinate file.");
   }
   if (numberOfNodes != areaCorrectionShapeFile->getNumberOfNodes()) {
      throw BrainModelAlgorithmException("Area correction shape file has different number of nodes"
                                         "than the coordinate file.");
   }
   
   //
   // Update progress
   //
   updateProgressDialog("Doing Left T-Map",
                        ALG_STEP_TMAP_LEFT,
                        ALG_NUM_STEPS);
   //
   // Create Left T-Map
   //
   try {
      leftTMapShapeFile = MetricFile::computeStatisticalTMap(&ssfLeftA, &ssfLeftB, 
                                                             brain->getTopologyFile(0),
                                                             tVarianceSmoothingIterations,
                                                             tVarianceSmoothingStrength,
                                                             false,
                                                             0.05,
                                                             false,
                                                             doStatisticalMapDOF, 
                                                             doStatisticalMapPValue);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Left T-Map failure: "
          << e.whatQString().toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Write Left T-Map file
   //
   try {
      leftTMapShapeFile->writeFile(leftTMapFileName);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Unable to write T-Map: "
          << FileUtilities::basename(leftTMapFileName).toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Update progress
   //
   updateProgressDialog("Doing Right T-Map",
                        ALG_STEP_TMAP_RIGHT,
                        ALG_NUM_STEPS);
   //
   // Create Right T-Map
   //
   try {
      rightTMapShapeFile = MetricFile::computeStatisticalTMap(&ssfRightA, &ssfRightB, 
                                                              brain->getTopologyFile(0),
                                                              tVarianceSmoothingIterations,
                                                              tVarianceSmoothingStrength,
                                                              false,
                                                              0.05,
                                                              false,
                                                              doStatisticalMapDOF, 
                                                              doStatisticalMapPValue);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Right T-Map failure: "
          << e.whatQString().toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Write Right T-Map file
   //
   try {
      rightTMapShapeFile->writeFile(rightTMapFileName);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Unable to write T-Map: "
          << FileUtilities::basename(rightTMapFileName).toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Update progress
   //
   updateProgressDialog("Creating Left/Right T-Map Product",
                        ALG_STEP_TMAP_PRODUCT,
                        ALG_NUM_STEPS);
   
   //
   // Do Left/Right T-Map product
   //
   statisticalMapShapeFile = new MetricFile;
   statisticalMapShapeFile->setNumberOfNodesAndColumns(numberOfNodes, 3);
   statisticalMapShapeFile->setColumnName(0, "Unused");
   statisticalMapShapeFile->setColumnName(1, "Unused");
   statisticalMapShapeFile->setColumnName(2, "T-Map");
   QString tMapCom("T-Map Product of ");
   tMapCom.append(FileUtilities::basename(leftTMapFileName));
   tMapCom.append(" and ");
   tMapCom.append(FileUtilities::basename(rightTMapFileName));
   statisticalMapShapeFile->setFileComment(tMapCom);
   for (int i = 0; i < numberOfNodes; i++) {
      statisticalMapShapeFile->setValue(i, 2,
                              leftTMapShapeFile->getValue(i, 2) *
                              rightTMapShapeFile->getValue(i, 2));
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
   updateProgressDialog("Doing Left Shuffled T-Map",
                        ALG_STEP_SHUFFLED_TMAP_LEFT,
                        ALG_NUM_STEPS);
                        
   //
   // Combine input shape for shuffled T-Map
   //
   MetricFile allShapeLeft = ssfLeftA;
   allShapeLeft.append(ssfLeftB);
   
   //
   // Do shuffled T-Map
   //
   try {
      leftShuffledTMapShapeFile = allShapeLeft.computeStatisticalShuffledTMap(iterationsRightLeftShuffledTMap,
                                                                ssfLeftA.getNumberOfColumns(),
                                                                brain->getTopologyFile(0),
                                                                tVarianceSmoothingIterations,
                                                                tVarianceSmoothingStrength,
                                                                false);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Left Shuffled T-Map failure: "
          << e.whatQString().toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }

   //
   // Write Left Shuffled  T-Map file
   //
   try {
      leftShuffledTMapShapeFile->writeFile(leftShuffledTMapFileName);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Unable to write Left Shuffled T-Map: "
          << FileUtilities::basename(leftShuffledTMapFileName).toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }

   //
   // Update progress
   //
   updateProgressDialog("Doing Right Shuffled T-Map",
                        ALG_STEP_SHUFFLED_TMAP_RIGHT,
                        ALG_NUM_STEPS);
                        
   //
   // Combine input shape for shuffled T-Map
   //
   MetricFile allShapeRight = ssfRightA;
   allShapeRight.append(ssfRightB);
   
   //
   // Do Right shuffled T-Map
   //
   try {
      rightShuffledTMapShapeFile = allShapeRight.computeStatisticalShuffledTMap(iterationsRightLeftShuffledTMap,
                                                                ssfRightA.getNumberOfColumns(),
                                                                brain->getTopologyFile(0),
                                                                tVarianceSmoothingIterations,
                                                                tVarianceSmoothingStrength,
                                                                false);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Right Shuffled T-Map failure: "
          << e.whatQString().toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }

   //
   // Write Right Shuffled  T-Map file
   //
   try {
      rightShuffledTMapShapeFile->writeFile(rightShuffledTMapFileName);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Unable to write right Shuffled T-Map: "
          << FileUtilities::basename(rightShuffledTMapFileName).toAscii().constData();
      cleanUp();
      throw BrainModelAlgorithmException(str.str().c_str());
   }

   //
   // Update progress
   //
   updateProgressDialog("Creating Shuffled T-Map Product",
                        ALG_STEP_SHUFFLED_TMAP_PRODUCT,
                        ALG_NUM_STEPS);
     
   //
   // Create shuffled T-Map product file
   //
   shuffleStatisticalMapShapeFile = new MetricFile;
   shuffleStatisticalMapShapeFile->setNumberOfNodesAndColumns(numberOfNodes, iterationsShuffledTMap);
   QString stMapCom("Shuffled T-Map Product of ");
   stMapCom.append(FileUtilities::basename(leftShuffledTMapFileName));
   stMapCom.append(" and ");
   stMapCom.append(FileUtilities::basename(rightShuffledTMapFileName));
   statisticalMapShapeFile->setFileComment(stMapCom);
   for (int j = 0; j < iterationsShuffledTMap; j++) {
      const int leftCol = StatisticRandomNumber::randomInteger(0, leftShuffledTMapShapeFile->getNumberOfColumns() - 1);
      const int rightCol = StatisticRandomNumber::randomInteger(0, rightShuffledTMapShapeFile->getNumberOfColumns() - 1);
      std::ostringstream str;
      str << "Left=" << leftCol
          << "   "
          << "Right=" << rightCol;
      shuffleStatisticalMapShapeFile->setColumnComment(j, str.str().c_str());
      for (int i = 0; i < numberOfNodes; i++) {
         shuffleStatisticalMapShapeFile->setValue(i, j,
                                 leftShuffledTMapShapeFile->getValue(i, leftCol) *
                                 rightShuffledTMapShapeFile->getValue(i, rightCol));
      }
   }

   //
   // Write Right Shuffled  T-Map product file
   //
   try {
      shuffleStatisticalMapShapeFile->writeFile(shuffledStatisticalMapFileName);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Unable to write right Shuffled T-Map: "
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
   // Note: column 2 is the T-Map
   //
   const int tMapColumn = 2;
   std::vector<Cluster> tMapClusters;
   findClusters(statisticalMapShapeFile, tMapClusters, "Finding clusters in T-Map", tMapColumn, false);
      
   //
   // Update progress
   //
   updateProgressDialog("Finding clusters in Shuffled T-Map",
                        ALG_STEP_FINDING_CLUSTERS_SHUFFLE_T_MAP,
                        ALG_NUM_STEPS);
                        
   //
   // find the clusters in Shuffled T-Map
   // Note: Only use largest cluster from each column
   //
   std::vector<Cluster> shuffleTMapClusters;
   findClusters(shuffleStatisticalMapShapeFile, shuffleTMapClusters, "Finding clusters in Shuffled T-Map", -1, true);
   
   //
   // Find area of the "P-Value" cluster in the shuffled T-Map
   //
   float significantCorrectedArea = std::numeric_limits<float>::max();
   int pValueClusterIndex = -1;
   if (shuffleTMapClusters.empty() == false) {
      pValueClusterIndex = std::min(static_cast<int>(pValue * iterationsShuffledTMap) - 1,
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
      cnt = std::min(cnt, iterationsShuffledTMap);
      tMapCluster.pValue = static_cast<float>(cnt)
                         / static_cast<float>(iterationsShuffledTMap);
   }
   
   //
   // So only do column 3 in T-Map when searching for T-Map clusters
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
   reportStream << "Shape File Left A:   " << shapeFileLeftAName << "\n";
   reportStream << "Shape File Left B:   " << shapeFileLeftBName << "\n";
   reportStream << "Shape File Left A:   " << shapeFileRightAName << "\n";
   reportStream << "Shape File Left B:   " << shapeFileRightBName << "\n";
   reportStream << "Fiducial Coord File: " << fiducialCoordFileName << "\n";
   reportStream << "Open Topo File:      " << openTopoFileName << "\n";
   if ((areaCorrectionShapeFile != NULL) ||
       (areaCorrectionShapeFileColumn >= 0)) {
      reportStream << "Area Correct File:   " << areaCorrectionShapeFileName << "\n";
      reportStream << "Area Correct Column: " << areaCorrectionShapeFile->getColumnName(areaCorrectionShapeFileColumn) << "\n";
   }
   reportStream << "Negative Threshold:  " << negativeThresh << "\n";
   reportStream << "Positive Threshold:  " << positiveThresh << "\n";
   reportStream << "Right/LeftIterations:          " << iterationsRightLeftShuffledTMap << "\n";
   reportStream << "Shuffled T-Map Iterations: " << iterationsShuffledTMap << "\n";
   reportStream << "P-Value:             " << pValue << "\n";
   reportStream << "Significant Area:    " << significantCorrectedArea << "\n";
   reportStream << "\n";
   
   //
   // Add significant clusters to report file
   //
   reportStream << "Shuffled TMap" << "\n";
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
   reportStream << "Shuffled TMap" << "\n";
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
   createClustersMetricFile(tMapClusters, tMapColumn, numberOfNodes);

   //
   // Do cluster reports
   //
   createMetricShapeClustersReportFile(tMapClusters,
                                       shapeFileRightAName);
   createMetricShapeClustersReportFile(tMapClusters,
                                       shapeFileRightBName);
   createMetricShapeClustersReportFile(tMapClusters,
                                       shapeFileLeftAName);
   createMetricShapeClustersReportFile(tMapClusters,
                                       shapeFileLeftBName);
}
   
/**
 * Free memory.
 */
void
BrainModelSurfaceMetricInterHemClusters::cleanUp()
{
   BrainModelSurfaceMetricFindClustersBase::cleanUp();
   
   if (leftTMapShapeFile != NULL) {
      delete leftTMapShapeFile;
      leftTMapShapeFile = NULL;
   }
   if (rightTMapShapeFile != NULL) {
      delete rightTMapShapeFile;
      rightTMapShapeFile = NULL;
   }
   if (leftShuffledTMapShapeFile != NULL) {
      delete leftShuffledTMapShapeFile;
      leftShuffledTMapShapeFile = NULL;
   }
   if (rightShuffledTMapShapeFile != NULL) {
      delete rightShuffledTMapShapeFile;
      rightShuffledTMapShapeFile = NULL;
   }
}

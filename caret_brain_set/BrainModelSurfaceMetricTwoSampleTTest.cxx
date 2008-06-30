
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
#include <sstream>
#include <limits>

#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricTwoSampleTTest.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "PaintFile.h"
#include "MetricFile.h"
#include "StatisticDataGroup.h"
#include "StatisticMeanAndDeviation.h"
#include "StatisticRankTransformation.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceMetricTwoSampleTTest::BrainModelSurfaceMetricTwoSampleTTest(
                                          BrainSet* bs,
                                          const DATA_TRANSFORM_MODE dataTransformModeIn,
                                          const VARIANCE_MODE varianceModeIn,
                                          const QString& shapeFileANameIn,
                                          const QString& shapeFileBNameIn,
                                          const QString& fiducialCoordFileNameIn,
                                          const QString& openTopoFileNameIn,
                                          const QString& areaCorrectionShapeFileNameIn,
                                          const QString& tMapFileNameIn,
                                          const QString& shuffledTMapFileNameIn,
                                          const QString& clustersPaintFileNameIn,
                                          const QString& clustersMetricFileNameIn,
                                          const QString& reportFileNameIn,
                                          const int areaCorrectionShapeFileColumnIn,
                                          const int iterationsIn,
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
   dataTransformMode = dataTransformModeIn;
   varianceMode = varianceModeIn;
   shapeFileAName = shapeFileANameIn;
   shapeFileBName = shapeFileBNameIn;
   iterations = iterationsIn;
}
      
/**
 * destructor.
 */
BrainModelSurfaceMetricTwoSampleTTest::~BrainModelSurfaceMetricTwoSampleTTest()
{
   cleanUp();
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceMetricTwoSampleTTest::executeClusterSearch() throw (BrainModelAlgorithmException)
{
   //
   // Steps in algorithm
   //
   enum {
      ALG_STEP_CHECKING_INPUT,
      ALG_STEP_TMAP,
      ALG_STEP_SHUFFLED_TMAP,
      ALG_STEP_FINDING_CLUSTERS_T_MAP,
      ALG_STEP_FINDING_CLUSTERS_SHUFFLE_T_MAP,
      ALG_NUM_STEPS
   };
   
   //
   // Update progress
   //
   QString title;
   switch (dataTransformMode) {
      case DATA_TRANSFORM_NONE:
         title = "Two Sample T-Test";
         break;
      case DATA_TRANSFORM_WILCOXON_RANK_SUM_THEN_TWO_SAMPLE_T_TEST:
         title = "Wilcoxon Rank-Sum Applied to Input Data";
         break;
   }
   bool pooledVarianceFlag = false;
   switch (varianceMode) {
      case VARIANCE_MODE_SIGMA:
         title += " donna's sigma";
         break;
      case VARIANCE_MODE_POOLED:
         pooledVarianceFlag = true;
         title += " pooled variance";
         break;
      case VARIANCE_MODE_UNPOOLED:
         title += " unpooled variance";
         break;
   }
   
   createProgressDialog(title,
                        ALG_NUM_STEPS,
                        "sifClustDialog");
   updateProgressDialog("Verifying Input",
                        ALG_STEP_CHECKING_INPUT,
                        ALG_NUM_STEPS);
                        
   //
   // check iterations
   //
   if (iterations < 0) {
      throw BrainModelAlgorithmException("Iterations must be positive.");
   }
   
   //
   // Read the surface shape A file
   //
   MetricFile ssfA;
   try {
      ssfA.readFile(shapeFileAName);
   }
   catch (FileException &e) {
      std::ostringstream str;
      str << "Unable to read surface shape file A: "
          << FileUtilities::basename(shapeFileAName).toAscii().constData();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Read the surface shape B file
   //
   MetricFile ssfB;
   try {
      ssfB.readFile(shapeFileBName);
   }
   catch (FileException &e) {
      std::ostringstream str;
      str << "Unable to read surface shape file B: "
          << FileUtilities::basename(shapeFileBName).toAscii().constData();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   //
   // Number of nodes in surface
   //
   const int numberOfNodes = bms->getNumberOfNodes();
   
   //
   // Check file compatibilities
   //
   if (numberOfNodes != ssfA.getNumberOfNodes()) {
      throw BrainModelAlgorithmException("First shape file has different number of nodes"
                                         "than the coordinate file.");
   }
   if (numberOfNodes != ssfB.getNumberOfNodes()) {
      throw BrainModelAlgorithmException("Second shape file has different number of nodes"
                                         "than the coordinate file.");
   }
   if (numberOfNodes != areaCorrectionShapeFile->getNumberOfNodes()) {
      throw BrainModelAlgorithmException("Area correction shape file has different number of nodes"
                                         "than the coordinate file.");
   }
   
   //
   // data transform mode specific operations
   //
   switch (dataTransformMode) {
      case DATA_TRANSFORM_NONE:
         break;
      case DATA_TRANSFORM_WILCOXON_RANK_SUM_THEN_TWO_SAMPLE_T_TEST:
         //
         // Transform the input data into rank-sum data
         //
         transformToRankSum(ssfA, ssfB);
         break;
   }
   
   //
   // Update progress
   //
   updateProgressDialog("Doing T-Map",
                        ALG_STEP_TMAP,
                        ALG_NUM_STEPS);

   int tMapColumn = 2;
   switch (varianceMode) {
      case VARIANCE_MODE_SIGMA:
         tMapColumn = 0;
         statisticalMapShapeFile = createDonnasSigmaTMap(ssfA, ssfB, true);
         break;
      case VARIANCE_MODE_POOLED:    // NOTE: Same processing for pooled and unpooled
      case VARIANCE_MODE_UNPOOLED:
         //
         // Create T-Map
         //
         tMapColumn = 2;
         try {
            statisticalMapShapeFile = MetricFile::computeStatisticalTMap(&ssfA, &ssfB, 
                                                               brain->getTopologyFile(0),
                                                               tVarianceSmoothingIterations,
                                                               tVarianceSmoothingStrength,
                                                               pooledVarianceFlag,
                                                               0.05,
                                                               false,
                                                               doStatisticalMapDOF, 
                                                               doStatisticalMapPValue);
         }
         catch (FileException& e) {
            std::ostringstream str;
            str << "T-Map failure: "
                << e.whatQString().toAscii().constData();
            cleanUp();
            throw BrainModelAlgorithmException(str.str().c_str());
         }
         break;
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
   updateProgressDialog("Doing Shuffled T-Map",
                        ALG_STEP_SHUFFLED_TMAP,
                        ALG_NUM_STEPS);
                        
   switch (varianceMode) {
      case VARIANCE_MODE_SIGMA:
         tMapColumn = 0;
         //
         // Create the shuffled sigma t-map
         //
         shuffleStatisticalMapShapeFile = createDonnasShuffledSigmaTMap(ssfA, ssfB);
         
         //
         // Determine sigma for each node and apply to both t-map and shuffled t-map
         //
         finishDonnasSigmaTMap(*statisticalMapShapeFile, *shuffleStatisticalMapShapeFile);

         //
         // Write T-Map file again since it was changed when applying sigma
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
         break;
      case VARIANCE_MODE_POOLED:  // NOTE: same processing for pooled and unpooled
      case VARIANCE_MODE_UNPOOLED:
         {
            //
            // Combine input shape for shuffled T-Map
            //
            MetricFile allShape = ssfA;
            allShape.append(ssfB);
            
            //
            // Do shuffled T-Map
            //
            try {
               shuffleStatisticalMapShapeFile = allShape.computeStatisticalShuffledTMap(iterations,
                                                                         ssfA.getNumberOfColumns(),
                                                                         brain->getTopologyFile(0),
                                                                         tVarianceSmoothingIterations,
                                                                         tVarianceSmoothingStrength,
                                                                         pooledVarianceFlag);
            }
            catch (FileException& e) {
               std::ostringstream str;
               str << "Shuffled T-Map failure: "
                   << e.whatQString().toAscii().constData();
               cleanUp();
               throw BrainModelAlgorithmException(str.str().c_str());
            }
         };
         break;
   }

   //
   // Write Shuffled  T-Map file
   //
   try {
      shuffleStatisticalMapShapeFile->writeFile(shuffledStatisticalMapFileName);
   }
   catch (FileException& e) {
      std::ostringstream str;
      str << "Unable to write Shuffled T-Map: "
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
   //
   std::vector<Cluster> tMapClusters;
   findClusters(statisticalMapShapeFile, tMapClusters, "Finding Clusters in T-Map", tMapColumn, false);
      
   //
   // Update progress
   //
   updateProgressDialog("Finding Clusters in Shuffled T-Map",
                        ALG_STEP_FINDING_CLUSTERS_SHUFFLE_T_MAP,
                        ALG_NUM_STEPS);
                        
   //
   // find the clusters in Shuffled T-Map
   // Note: Only use largest cluster from each column
   //
   std::vector<Cluster> shuffleTMapClusters;
   findClusters(shuffleStatisticalMapShapeFile, shuffleTMapClusters, "Finding Clusters in Shuffled T-Map", -1, true);
   
   //
   // Find area of the "P-Value" cluster in the shuffled T-Map
   //
   float significantCorrectedArea = std::numeric_limits<float>::max();
   int pValueClusterIndex = -1;
   if (shuffleTMapClusters.empty() == false) {
      pValueClusterIndex = std::min(static_cast<int>(pValue * iterations) - 1,
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
      cnt = std::min(cnt, iterations);
      tMapCluster.pValue = static_cast<float>(cnt)
                         / static_cast<float>(iterations);
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
   reportStream << "Type of T-Test:      " << title << "\n";
   reportStream << "Date/Time:           " << QDateTime::currentDateTime().toString("MMM d, yyyy hh:mm:ss") << "\n";
   reportStream << "Shape File A:        " << shapeFileAName << "\n";
   reportStream << "Shape File B:        " << shapeFileBName << "\n";
   reportStream << "Fiducial Coord File: " << fiducialCoordFileName << "\n";
   reportStream << "Open Topo File:      " << openTopoFileName << "\n";
   if ((areaCorrectionShapeFile != NULL) ||
       (areaCorrectionShapeFileColumn >= 0)) {
      reportStream << "Area Correct File:   " << areaCorrectionShapeFileName << "\n";
      reportStream << "Area Correct Column: " << areaCorrectionShapeFile->getColumnName(areaCorrectionShapeFileColumn) << "\n";
   }
   reportStream << "Negative Threshold:  " << negativeThresh << "\n";
   reportStream << "Positive Threshold:  " << positiveThresh << "\n";
   reportStream << "Iterations:          " << iterations << "\n";
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
                                       shapeFileAName);
   createMetricShapeClustersReportFile(tMapClusters,
                                       shapeFileBName);
}
  
/**
 * finish donna's sigma processing.
 */
void 
BrainModelSurfaceMetricTwoSampleTTest::finishDonnasSigmaTMap(MetricFile& tMapFile,
                                                             MetricFile& shuffledTMapFile) throw (BrainModelAlgorithmException)     
{
   const int numNodes = shuffledTMapFile.getNumberOfNodes();
   const int numColumns = shuffledTMapFile.getNumberOfColumns();
   if (numColumns <= 0) {
      return;
   }   
   
   //
   // Compute the deviation for nodes in shuffled sigma t-map
   //
   float* data = new float[numColumns];
   StatisticDataGroup sdg(data, numColumns, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   for (int i = 0; i < numNodes; i++) {
      //
      // Get the data for a node in the shuffled file
      //
      for (int j = 0; j < numColumns; j++) {
         data[j] = shuffledTMapFile.getValue(i, j);
      }
      
      //
      // compute statistics, note "sdg" maintains pointer to "data[]"
      //
      StatisticMeanAndDeviation smd;
      smd.addDataGroup(&sdg);
      try {
         smd.execute();
      }
      catch (StatisticException& e) {
         throw BrainModelAlgorithmException(e);
      }
      
      //
      // divide t-map and shuffled t-map by sigma
      //
      const float sigma = smd.getStandardDeviation();
      if (sigma != 0) {
         tMapFile.setValue(i, 0, (tMapFile.getValue(i, 0) / sigma));
         for (int j = 0; j < numColumns; j++) {
            shuffledTMapFile.setValue(i, j, 
                                      (shuffledTMapFile.getValue(i, j) / sigma));
         }
      }
   }
   
   delete[] data;
}

/**
 * create donna's shuffled sigma t-map.
 */
MetricFile* 
BrainModelSurfaceMetricTwoSampleTTest::createDonnasShuffledSigmaTMap(
                                               const MetricFile& mfA,
                                               const MetricFile& mfB) throw (BrainModelAlgorithmException)
{
   MetricFile* mf = NULL;
   
   try {
      const int numberOfNodes = bms->getNumberOfNodes();
      
      //
      // Copy the input metric files and shuffle them and create output files for shuffling
      //
      std::vector<MetricFile*> shuffleFilesIn;
      shuffleFilesIn.push_back((MetricFile*)&mfA);
      shuffleFilesIn.push_back((MetricFile*)&mfB);
      MetricFile ma = mfA;
      MetricFile mb = mfB;
      std::vector<MetricFile*> shuffleFilesOut;
      shuffleFilesOut.push_back(&ma);
      shuffleFilesOut.push_back(&mb);

      //
      // Create the shuffled metric file
      //
      mf = new MetricFile;
      mf->setNumberOfNodesAndColumns(numberOfNodes, iterations);
      for (int j = 0; j < iterations; j++) {
         //
         // Set the name of the column
         //
         mf->setColumnName(j, "Iteration " + QString::number(j + 1));
         
         //
         // Shuffle the input files into the output files
         //
         MetricFile::shuffle(shuffleFilesIn, shuffleFilesOut);
         
         //
         // Create the Sigma T-Map
         //
         MetricFile* mfT = createDonnasSigmaTMap(ma, mb, false);
         
         //
         // Add Sigma T-Map to shuffled file
         //
         for (int i = 0; i < numberOfNodes; i++) {
            mf->setValue(i, j, mfT->getValue(i, 0));
         }
         
         delete mfT;
      }
   }
   catch (FileException& e) {
      if (mf != NULL) {
         delete mf;
         mf = NULL;
      }
      throw BrainModelAlgorithmException(e);
   }
      
   return mf;
}
      
/**
 * create donna's sigma t-map.
 */
MetricFile* 
BrainModelSurfaceMetricTwoSampleTTest::createDonnasSigmaTMap(const MetricFile& mfA,
                                                             const MetricFile& mfB,
                                                             const bool addMeanValues) throw (BrainModelAlgorithmException)
{
   const int numberOfNodes = bms->getNumberOfNodes();

   //
   // Create the metric file
   //
   MetricFile* mf = new MetricFile;
   int numberOfColumns = 0;
   const int tSigmaColumn = numberOfColumns++;
   int meanGroupAColumn = -1;
   int meanGroupBColumn = -1;
   if (addMeanValues) {
      meanGroupAColumn = numberOfColumns++;
      meanGroupBColumn = numberOfColumns++;
   }
   
   mf->setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns);
   if (addMeanValues) {
      mf->setColumnName(meanGroupAColumn, "Group A Mean");
      mf->setColumnName(meanGroupBColumn, "Group B Mean");
   }
   mf->setColumnName(tSigmaColumn, "T-Sigma");

   mf->setFileComment("T-Sigma Map from "
                      + FileUtilities::basename(shapeFileAName)
                      + " and "
                      + FileUtilities::basename(shapeFileBName));
   
   //
   // Compute A's, B's mean, and abs difference
   //
   const int numColumnsA = mfA.getNumberOfColumns();
   const int numColumnsB = mfB.getNumberOfColumns();
   for (int i = 0; i < numberOfNodes; i++) {
      double sumA = 0.0;
      for (int j = 0; j < numColumnsA; j++) {
         sumA += mfA.getValue(i, j);
      }
      const float meanA = sumA / static_cast<double>(numColumnsA);

      double sumB = 0.0;
      for (int j = 0; j < numColumnsB; j++) {
         sumB += mfB.getValue(i, j);
      }
      const float meanB = sumB / static_cast<double>(numColumnsB);
      
      if (addMeanValues) {
         mf->setValue(i, meanGroupAColumn, meanA);
         mf->setValue(i, meanGroupBColumn, meanB);
      }
      mf->setValue(i, tSigmaColumn, std::fabs(meanA - meanB));
   }
   
   return mf;
}

/**
 * transform the input data into rank-sum data.
 */
void 
BrainModelSurfaceMetricTwoSampleTTest::transformToRankSum(MetricFile& fileA, 
                                                          MetricFile& fileB) throw (BrainModelAlgorithmException)
{
   const int numColA = fileA.getNumberOfColumns();
   const int numColB = fileB.getNumberOfColumns();
   const int numNodes = fileA.getNumberOfNodes();
   float* dataA = NULL;
   if (numColA > 0) {
      dataA = new float[numColA];
   }
   float* dataB = NULL;
   if (numColB > 0) {
      dataB = new float[numColB];
   }

   //
   // Convert data to rank-sum indices
   //
   for (int i = 0; i < numNodes; i++) {
      //
      // Get the data for each node
      //
      fileA.getAllColumnValuesForNode(i, dataA);
      fileB.getAllColumnValuesForNode(i, dataB);
      
      //
      // Transform the data
      //
      StatisticRankTransformation rt;
      StatisticDataGroup dga(dataA,
                             numColA,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticDataGroup dgb(dataB,
                             numColB,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      rt.addDataGroup(&dga);
      rt.addDataGroup(&dgb);
      try {
         rt.execute();
         if (rt.getNumberOfOutputDataGroups() != 2) {
            const QString msg = "StatisticRankTransform failed.  Number of data output data groups should be 2 but is "
                                + QString::number(rt.getNumberOfOutputDataGroups());
            throw BrainModelAlgorithmException(msg);
         }
      }
      catch (StatisticException& e) {
         throw BrainModelAlgorithmException(e);
      }
         
      //
      // set the data for each node
      //
      const StatisticDataGroup* sdgRankA = rt.getOutputDataGroupContainingRankValues(0);
      fileA.setAllColumnValuesForNode(i, sdgRankA->getPointerToData());
      const StatisticDataGroup* sdgRankB = rt.getOutputDataGroupContainingRankValues(1);
      fileB.setAllColumnValuesForNode(i, sdgRankB->getPointerToData());
   }
   
   //
   // Free memory
   //
   if (dataA != NULL) {
      delete[] dataA;
      dataA = NULL;
   }
   if (dataB != NULL) {
      delete[] dataB;
      dataB = NULL;
   }
   
   //
   // write rank-sum files if debugging is on
   //
   if (DebugControl::getDebugOn()) {
      try {
         fileA.writeFile("rank_sum_A.metric");
         fileB.writeFile("rank_sum_B.metric");
      }
      catch (FileException&) {
      }
   }
}
      
/**
 * Free memory.
 */
void
BrainModelSurfaceMetricTwoSampleTTest::cleanUp()
{
   BrainModelSurfaceMetricFindClustersBase::cleanUp();
}

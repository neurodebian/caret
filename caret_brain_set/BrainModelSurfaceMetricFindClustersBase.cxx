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
#include <limits>
#include <sstream>

#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "BrainModelAlgorithmRunAsThread.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricClustering.h"
#include "BrainModelSurfaceMetricFindClustersBase.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROITextReport.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "SpecFile.h"
#include "SurfaceShapeFile.h"
#include "TextFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceMetricFindClustersBase::BrainModelSurfaceMetricFindClustersBase(
                                         BrainSet* bs,
                                         const QString& fiducialCoordFileNameIn,
                                         const QString& openTopoFileNameIn,
                                         const QString& areaCorrectionShapeFileNameIn,
                                         const QString& statisticalMapFileNameIn,
                                         const QString& shuffledStatisticalMapFileNameIn,
                                         const QString& clustersPaintFileNameIn,
                                         const QString& clustersMetricFileNameIn,
                                         const QString& reportFileNameIn,
                                         const int areaCorrectionShapeFileColumnIn,
                                         const float negativeThreshIn,
                                         const float positiveThreshIn,
                                         const float pValueIn,
                                         const int tVarianceSmoothingIterationsIn,
                                         const float tVarianceSmoothingStrengthIn,
                                         const bool doStatisticalMapDOFIn,
                                         const bool doStatisticalMapPValueIn,
                                         const int numberOfThreadsIn)
   : BrainModelAlgorithm(bs)
{
   areaCorrectionShapeFile = NULL;
   bms = NULL;
   brain = NULL;
   shuffleStatisticalMapShapeFile = NULL;
   statisticalMapShapeFile   = NULL;

   fiducialCoordFileName = fiducialCoordFileNameIn;
   openTopoFileName = openTopoFileNameIn;
   areaCorrectionShapeFileName = areaCorrectionShapeFileNameIn;
   statisticalMapFileName = statisticalMapFileNameIn;
   shuffledStatisticalMapFileName = shuffledStatisticalMapFileNameIn;
   clustersPaintFileName = clustersPaintFileNameIn;
   clustersMetricFileName = clustersMetricFileNameIn;
   reportFileName = reportFileNameIn;
   areaCorrectionShapeFileColumn = areaCorrectionShapeFileColumnIn;
   negativeThresh = negativeThreshIn;
   positiveThresh = positiveThreshIn;
   pValue = pValueIn;
   tVarianceSmoothingIterations = tVarianceSmoothingIterationsIn;
   tVarianceSmoothingStrength = tVarianceSmoothingStrengthIn;
   doStatisticalMapDOF = doStatisticalMapDOFIn;
   doStatisticalMapPValue = doStatisticalMapPValueIn;
   numberOfThreads = numberOfThreadsIn;
}
      
/**
 * destructor.
 */
BrainModelSurfaceMetricFindClustersBase::~BrainModelSurfaceMetricFindClustersBase()
{
   cleanUp();
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceMetricFindClustersBase::execute() throw (BrainModelAlgorithmException)
{
   //
   // check negative threshold
   //
   if (negativeThresh > 0.0) {
      throw BrainModelAlgorithmException("Negative threshold cannot be positive.");
   }

   //
   // check positive threshold
   //
   if (positiveThresh < 0.0) {
      throw BrainModelAlgorithmException("Positive threshold cannot be negative.");
   }
   
   //
   // check p-value
   //
   if ((pValue < 0.0) || (pValue > 1.0)) {
      throw BrainModelAlgorithmException("P-Value must be between 0.0 and 1.0.");
   }
  
   //
   // check Statistical file name
   // 
   if (statisticalMapFileName.isEmpty()) {
      throw BrainModelAlgorithmException("Statistical Map file name is empty.");
   }
   
   //
   // check Shuffled TMap file name
   // 
   if (shuffledStatisticalMapFileName.isEmpty()) {
      throw BrainModelAlgorithmException("Shuffled T-Map file name is empty.");
   }
   
   //
   // check Report file name
   // 
   if (reportFileName.isEmpty()) {
      throw BrainModelAlgorithmException("Report file name is empty.");
   }
   
   //
   // Read the area correction file
   //
   areaCorrectionShapeFile = new MetricFile;
   try {
      areaCorrectionShapeFile->readFile(areaCorrectionShapeFileName);
   }
   catch (FileException &e) {
      std::ostringstream str;
      str << "Unable to read area correction shape file: "
          << FileUtilities::basename(areaCorrectionShapeFileName).toAscii().constData();
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   
   if ((areaCorrectionShapeFileColumn < 0) ||
       (areaCorrectionShapeFileColumn >= areaCorrectionShapeFile->getNumberOfColumns())) {
      QString msg("Invalid distortion correction column=" + QString::number(areaCorrectionShapeFileColumn));
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // Create a brain set with the fiducial surface and the topology file
   //
   brain = new BrainSet(openTopoFileName, fiducialCoordFileName);
   if (brain == NULL) {
      throw BrainModelAlgorithmException("Unable to create brain set.");
   }
   if (brain->getNumberOfBrainModels() <= 0) {
      throw BrainModelAlgorithmException("Problem with coordinate or topology file.");
   }
   bms = brain->getBrainModelSurface(0);
   if (bms == NULL) {
      throw BrainModelAlgorithmException("Problem with coordinate or topology file.");
   }
   if (bms->getTopologyFile() == NULL) {
      throw BrainModelAlgorithmException("Problem with topology file.");
   }
   if (bms->getTopologyFile()->getNumberOfTiles() <= 0) {
      throw BrainModelAlgorithmException("Topology file has no tiles.");
   }
   const int numberOfNodes = bms->getNumberOfNodes();
   if (numberOfNodes <= 0) {
      throw BrainModelAlgorithmException("Coordinate file has no nodes.");
   }
   
   //
   // Execute subclasses search
   //
   executeClusterSearch();
   
   cleanUp();   
}
   
/**
 * find clusters.
 *
 * If limitToColumn is greater than or equal to zero, only that column of the 
 * file will be searched for clusters.
 * If useOneClusterPerColumnFlag is set, only the largest cluster from each 
 * column will be used.
 */
void 
BrainModelSurfaceMetricFindClustersBase::findClusters(MetricFile* mf, 
                                    std::vector<Cluster>& clustersOut,
                                    const QString& progressMessage,
                                    const int limitToColumn,
                                    const bool useLargestClusterPerColumnFlag) throw (BrainModelAlgorithmException)
{
   QTime timer;
   timer.start();
   
   if(numberOfThreads <= 1) {
      findClustersSingleThread(mf,
                               clustersOut,
                               progressMessage,
                               limitToColumn,
                               useLargestClusterPerColumnFlag);
   }
   else {
      findClustersMultiThread(mf,
                              clustersOut,
                              progressMessage,
                              limitToColumn,
                              useLargestClusterPerColumnFlag,
                              numberOfThreads);
   }
   
   setNamesForClusters(clustersOut);
   
   std::cout << "Cluster search with " << numberOfThreads << " threads: "
             << (static_cast<float>(timer.elapsed()) / 1000.0) << " seconds." << std::endl;
}
      
/**
 * find clusters
 */
void 
BrainModelSurfaceMetricFindClustersBase::findClustersSingleThread(MetricFile* mf,
                                                        std::vector<Cluster>& clustersOut,
                                                        const QString& progressMessage,
                                                        const int limitToColumn,
                                                        const bool useLargestClusterPerColumnFlag)
                                                           throw (BrainModelAlgorithmException)
{
   float posMin = positiveThresh;
   float posMax = std::numeric_limits<float>::max();
   float negMin = negativeThresh;
   float negMax =  -std::numeric_limits<float>::max();
   
   //
   // Determine columns for finding clusters
   //
   int startColumn = 0;
   int endColumn = mf->getNumberOfColumns() - 1;
   if (limitToColumn >= 0) {
      if (limitToColumn >= mf->getNumberOfColumns()) {
         std::ostringstream str;
         str << "Invalid column: "
             << limitToColumn
             << " for file " 
             << FileUtilities::basename(mf->getFileName()).toAscii().constData();
         throw BrainModelAlgorithmException(str.str().c_str());
      }
      startColumn = limitToColumn;
      endColumn   = limitToColumn;
   }
   
   //
   // Process each column
   //
   int progressCounter = 0;
   const int numProgress = endColumn - startColumn + 1;
   for (int i = startColumn; i <= endColumn; i++) {
      if (DebugControl::getDebugOn()) {
         std::cout << "Cluster analysis for column: " << i << std::endl;
      }
      
      progressCounter++;
      if (progressMessage.isEmpty() == false) {
         std::ostringstream str;
         str << progressMessage.toAscii().constData()
             << ": "
             << progressCounter
             << " of "
             << numProgress;
         updateProgressDialog(str.str().c_str(), -1, -1);
      }

      //
      // Create a shape file containing just the single column
      //
      MetricFile tempShapeFile;
      tempShapeFile.setNumberOfNodesAndColumns(mf->getNumberOfNodes(), 1);
      std::vector<float> nodeValues;
      mf->getColumnForAllNodes(i, nodeValues);
      tempShapeFile.setColumnForAllNodes(0, nodeValues);
      
      //
      // Find the clusters
      //
      BrainModelSurfaceMetricClustering bmsmc(brain,
                                              bms,
                                              &tempShapeFile,
                                              BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA,
                                              0,
                                              0,
                                              "cluster",
                                              1,
                                              0.1,
                                              negMin,
                                              negMax,
                                              posMin,
                                              posMax,
                                              true);
      try {
         bmsmc.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         cleanUp();
         throw e;
      }
      
      //
      // Storage for the clusters
      //
      std::vector<Cluster> clusters;
      
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
         if ((areaCorrectionShapeFile != NULL) ||
             (areaCorrectionShapeFileColumn >= 0)) {
            for (int k = 0; k < numNodesInCluster; k++) {
               const int nodeNum = cluster->getNodeInCluster(k);
               float nodeArea = nodeAreas[nodeNum];
               const double metric = areaCorrectionShapeFile->getValue(nodeNum, areaCorrectionShapeFileColumn);
               correctedArea += (nodeArea * std::pow(2.0, metric));
            }
         }
         
         //
         // Save the cluster information
         //
         float cog[3];
         cluster->getCenterOfGravity(cog);
         Cluster c;
         c.column = i + 1;
         c.numberOfNodes = cluster->getNumberOfNodesInCluster();
         c.nodes = cluster->getNodesInCluster();
         c.area = cluster->getArea();
         c.areaCorrected = correctedArea;
         c.cogX = cog[0];
         c.cogY = cog[1];
         c.cogZ = cog[2];
         float threshMin, threshMax;
         cluster->getThresholds(threshMin, threshMax);
         c.threshMin = threshMin;
         c.threshMax = threshMax;
         clusters.push_back(c);
      }
      
      //
      // Should only largest cluster be used
      //
      if (useLargestClusterPerColumnFlag) {
         if (clusters.empty() == false) {
            std::sort(clusters.begin(), clusters.end());
            const int largestClusterIndex = static_cast<int>(clusters.size() - 1);
            clustersOut.push_back(clusters[largestClusterIndex]);
         }
      }
      else {
         clustersOut.insert(clustersOut.end(), clusters.begin(), clusters.end());
      }
   }
   
   //
   // Sort clusters so biggest elements first
   //
   std::sort(clustersOut.begin(), clustersOut.end());
   std::reverse(clustersOut.begin(), clustersOut.end());
}
 
/**
 * Get the clusters after the cluster finding algorithm has finished
 */
void
BrainModelSurfaceMetricFindClustersBase::saveClusters(BrainModelSurfaceMetricClustering* bmsmc,
                                                      std::vector<Cluster>& clustersOut,
                                                      const int columnNumber,
                                                      const bool useLargestClusterPerColumnFlag)
{            
   //
   // Storage for the clusters
   //
   std::vector<Cluster> clusters;
   
   //
   // Node areas
   //
   std::vector<float> nodeAreas;
   bms->getAreaOfAllNodes(nodeAreas);

   //
   // Process the clusters
   //
   const int numClusters = bmsmc->getNumberOfClusters();
   for (int j = 0; j < numClusters; j++) {
      const BrainModelSurfaceMetricClustering::Cluster* cluster = bmsmc->getCluster(j);
      const int numNodesInCluster = cluster->getNumberOfNodesInCluster();

      //
      // Corrected area is sum of each node's area multiplied by
      // 2 to the power of the selected metric column for the node
      //
      float correctedArea = 0.0;
      if ((areaCorrectionShapeFile != NULL) ||
          (areaCorrectionShapeFileColumn >= 0)) {
         for (int k = 0; k < numNodesInCluster; k++) {
            const int nodeNum = cluster->getNodeInCluster(k);
            float nodeArea = nodeAreas[nodeNum];
            const double metric = areaCorrectionShapeFile->getValue(nodeNum, areaCorrectionShapeFileColumn);
            correctedArea += (nodeArea * std::pow(2.0, metric));
         }
      }
      
      //
      // Save the cluster information
      //
      float cog[3];
      cluster->getCenterOfGravity(cog);
      Cluster c;
      c.column = columnNumber;
      c.numberOfNodes = cluster->getNumberOfNodesInCluster();
      c.nodes = cluster->getNodesInCluster();
      c.area = cluster->getArea();
      c.areaCorrected = correctedArea;
      c.cogX = cog[0];
      c.cogY = cog[1];
      c.cogZ = cog[2];
      float threshMin, threshMax;
      cluster->getThresholds(threshMin, threshMax);
      c.threshMin = threshMin;
      c.threshMax = threshMax;
      clusters.push_back(c);
   } // for (int j = 0; j < numClusters
   
   //
   // Should only largest cluster be used
   //
   if (useLargestClusterPerColumnFlag) {
      if (clusters.empty() == false) {
         std::sort(clusters.begin(), clusters.end());
         const int largestClusterIndex = static_cast<int>(clusters.size() - 1);
         clustersOut.push_back(clusters[largestClusterIndex]);
      }
   }
   else {
      clustersOut.insert(clustersOut.end(), clusters.begin(), clusters.end());
   }
}

/**
 * find clusters
 */
void 
BrainModelSurfaceMetricFindClustersBase::findClustersMultiThread(MetricFile* mf,
                                                        std::vector<Cluster>& clustersOut,
                                                        const QString& progressMessage,
                                                        const int limitToColumn,
                                                        const bool useLargestClusterPerColumnFlag,
                                                        const int numberOfThreads)
                                                           throw (BrainModelAlgorithmException)
{
   float posMin = positiveThresh;
   float posMax = std::numeric_limits<float>::max();
   float negMin = negativeThresh;
   float negMax =  -std::numeric_limits<float>::max();
   
   //
   // Determine columns for finding clusters
   //
   int startColumn = 0;
   int endColumn = mf->getNumberOfColumns() - 1;
   if (limitToColumn >= 0) {
      if (limitToColumn >= mf->getNumberOfColumns()) {
         std::ostringstream str;
         str << "Invalid column: "
             << limitToColumn
             << " for file " 
             << FileUtilities::basename(mf->getFileName()).toAscii().constData();
         throw BrainModelAlgorithmException(str.str().c_str());
      }
      startColumn = limitToColumn;
      endColumn   = limitToColumn;
   }
   
   //
   // Next column that is to be processed
   //
   int nextColumnToProcess = startColumn;
   
   //
   // Time to wait for threads in milliseconds
   //
   const unsigned long totalWaitTime = 1000;  // 1000 milliseconds == 1 second
   const unsigned long threadWaitTime = totalWaitTime / numberOfThreads;
   
   //
   // Pointers for algorithm threads
   //
   std::vector<BrainModelAlgorithmRunAsThread*> algorithmThreads(numberOfThreads);
   for (int i = 0; i < numberOfThreads; i++) {
      algorithmThreads[i] = NULL;
   }
   
   //
   // Column number on which thread is operating
   //
   std::vector<int> threadColumnNumber(numberOfThreads, -1);
   
   //
   // One metric column per thread
   //
   std::vector<MetricFile> metricFilesForThreads(numberOfThreads);
   
   //
   // Progress data
   //
   int progressCounter = 0;
   const int numProgress = endColumn - startColumn + 1;

   //
   // Find clusters in all selected columns
   //
   bool done = false;
   while (done == false) {
      //
      // Loop through the threads to see if any are done and create new threads
      //
      for (int iThread = 0; iThread < numberOfThreads; iThread++) {
         //
         // Is thread valid
         //
         if (algorithmThreads[iThread] != NULL) {
            //
            // Wait on the thread
            //
            algorithmThreads[iThread]->wait(threadWaitTime);
            
            //
            // Is the thread finished
            //
            if (algorithmThreads[iThread]->isFinished()) {
               //
               // Get the algorithm
               //
               BrainModelSurfaceMetricClustering* bmsmc = 
                  dynamic_cast<BrainModelSurfaceMetricClustering*>(algorithmThreads[iThread]->getBrainModelAlgorithm());
                  
               //
               // Save the clusters
               //
               saveClusters(bmsmc,
                            clustersOut,
                            threadColumnNumber[iThread],
                            useLargestClusterPerColumnFlag);
               
               //
               // delete the thread (which also deletes the algorithm)
               //
               delete algorithmThreads[iThread];
               algorithmThreads[iThread] = NULL;
            }
         }
         
         //
         // Is the thread available ?
         //
         if (algorithmThreads[iThread] == NULL) {
            //
            // Are there still columns of data to process
            //
            if (nextColumnToProcess <= endColumn) {
               //
               // Update progress
               //
               progressCounter++;
               if (progressMessage.isEmpty() == false) {
                  std::ostringstream str;
                  str << progressMessage.toAscii().constData()
                      << ": "
                      << progressCounter
                      << " of "
                      << numProgress;
                  updateProgressDialog(str.str().c_str(), -1, -1);
               }

               //
               // Set column being processed by thread (columns in report start at one)
               //
               threadColumnNumber[iThread] = nextColumnToProcess + 1;
               
               //
               // Copy data to the thread's metric file
               //
               metricFilesForThreads[iThread].setNumberOfNodesAndColumns(mf->getNumberOfNodes(), 1);
               std::vector<float> nodeValues;
               mf->getColumnForAllNodes(nextColumnToProcess, nodeValues);
               metricFilesForThreads[iThread].setColumnForAllNodes(0, nodeValues);
               
               //
               // Create a new cluster finding algorithm
               //
               BrainModelSurfaceMetricClustering* clusterAlgorithm =
                  new BrainModelSurfaceMetricClustering (brain,
                                                    bms,
                                                    &metricFilesForThreads[iThread],
                                                    BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA,
                                                    0,
                                                    0,
                                                    "cluster",
                                                    1,
                                                    0.1,
                                                    negMin,
                                                    negMax,
                                                    posMin,
                                                    posMax,
                                                    true);
               
               //
               // Create a new thread to run the algorithm and take ownership of cluster algorithm
               //
               algorithmThreads[iThread] = 
                  new BrainModelAlgorithmRunAsThread(clusterAlgorithm, true);
               algorithmThreads[iThread]->start(QThread::HighestPriority);
               //if (DebugControl::getDebugOn()) {
                  std::cout << "Started thread " << iThread << " for column " << nextColumnToProcess << std::endl;
               //}
               
               //
               // Move on to next metric column
               //
               nextColumnToProcess++;
            } // if (nextColumnToProcess <= endColumn)
         } // if (algorithmThreads[iThread] == NULL) 
      } // for (int iThread
      
      //
      // Loop through the threads to see if any are still valid and if time to stop
      //
      done = true;
      for (int iThread = 0; iThread < numberOfThreads; iThread++) {
         if (algorithmThreads[iThread] != NULL) {
            done = false;
         }
      }

      //
      // Allow other events to process
      //
      allowEventsToProcess();

   } // while (done == false)
      
   //
   // Sort clusters so biggest elements first
   //
   std::sort(clustersOut.begin(), clustersOut.end());
   std::reverse(clustersOut.begin(), clustersOut.end());
}
/*
void 
BrainModelSurfaceMetricFindClustersBase::findClustersMultiThread(MetricFile* mf,
                                                        std::vector<Cluster>& clustersOut,
                                                        const QString& progressMessage,
                                                        const int limitToColumn,
                                                        const bool useLargestClusterPerColumnFlag,
                                                        const int numberOfThreads)
                                                           throw (BrainModelAlgorithmException)
{
   float posMin = positiveThresh;
   float posMax = std::numeric_limits<float>::max();
   float negMin = negativeThresh;
   float negMax =  -std::numeric_limits<float>::max();
   
   //
   // Determine columns for finding clusters
   //
   int startColumn = 0;
   int endColumn = mf->getNumberOfColumns() - 1;
   if (limitToColumn >= 0) {
      if (limitToColumn >= mf->getNumberOfColumns()) {
         std::ostringstream str;
         str << "Invalid column: "
             << limitToColumn
             << " for file " 
             << FileUtilities::basename(mf->getFileName()).toAscii().constData();
         throw BrainModelAlgorithmException(str.str().c_str());
      }
      startColumn = limitToColumn;
      endColumn   = limitToColumn;
   }
   
   //
   // Process each column group with the size being the number of threads
   //
   int progressCounter = 0;
   const int numProgress = endColumn - startColumn + 1;
   for (int i = startColumn; i <= endColumn; i += numberOfThreads) {
      if (DebugControl::getDebugOn()) {
         std::cout << "Cluster analysis for column: " << i << std::endl;
      }
      
      progressCounter++;
      if (progressMessage.isEmpty() == false) {
         std::ostringstream str;
         str << progressMessage.toAscii().constData()
             << ": "
             << progressCounter
             << " of "
             << numProgress;
         updateProgressDialog(str.str().c_str(), -1, -1);
      }

      //
      // Create shape files for each of the threads
      //
      std::vector<MetricFile> tempShapeFile(numberOfThreads);

      //
      // Create storage for the algorithms and the threads
      //
      std::vector<BrainModelSurfaceMetricClustering*> clusterAlgorithms(numberOfThreads);
      std::vector<BrainModelAlgorithmRunAsThread*> algorithmThreads(numberOfThreads);
      
      //
      // Allow other events to process
      //
      allowEventsToProcess();
      
      //
      // Create and start each of the threads
      //
      for (int iThread = 0; iThread < numberOfThreads; iThread++) {
         //
         // Column number being processed
         //
         const int columnNumber = i + iThread;
         algorithmThreads[iThread] = NULL;
         clusterAlgorithms[iThread] = NULL;
         
         if (columnNumber <= endColumn) {
            //
            // Create a shape file containing just the single column
            //
            tempShapeFile[iThread].setNumberOfNodesAndColumns(mf->getNumberOfNodes(), 1);
            std::vector<float> nodeValues;
            mf->getColumnForAllNodes(columnNumber, nodeValues);
            tempShapeFile[iThread].setColumnForAllNodes(0, nodeValues);
            
            //
            // Create the cluster algorithm
            //
            clusterAlgorithms[iThread] = new BrainModelSurfaceMetricClustering (brain,
                                                    bms,
                                                    &tempShapeFile[iThread],
                                                    BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA,
                                                    0,
                                                    0,
                                                    "cluster",
                                                    1,
                                                    0.1,
                                                    negMin,
                                                    negMax,
                                                    posMin,
                                                    posMax);
            
            //
            // Create the thread to run the algorithm and start it
            //
            algorithmThreads[iThread] = 
               new BrainModelAlgorithmRunAsThread(clusterAlgorithms[iThread]);
            algorithmThreads[iThread]->start();
            algorithmThreads[iThread]->setPriority(QThread::HighestPriority);
            if (DebugControl::getDebugOn()) {
               std::cout << "Started thread " << iThread << " for column " << columnNumber << std::endl;
            }
         } // if (columnNumber <= endColumn)
      } // for (int iThread
         
      //
      // Allow other events to process
      //
      allowEventsToProcess();
      
      //
      // Wait for all of the threads to finish
      //
      for (int iThread = 0; iThread < numberOfThreads; iThread++) {
         if (algorithmThreads[iThread] != NULL) {
            algorithmThreads[iThread]->wait();
            if (DebugControl::getDebugOn()) {
               std::cout << "Thread finished " << iThread << std::endl;
            }
         }
      } // for (int iThread
      
      //
      // Process the outputs of the cluster algorithms
      //
      for (int iThread = 0; iThread < numberOfThreads; iThread++) {
         if (algorithmThreads[iThread] != NULL) {
            //
            // Pointer to the cluster algorithm
            //
            BrainModelSurfaceMetricClustering* bmsmc = clusterAlgorithms[iThread];
            
            //
            // Storage for the clusters
            //
            std::vector<Cluster> clusters;
            
            //
            // Process the clusters
            //
            const int numClusters = bmsmc->getNumberOfClusters();
            for (int j = 0; j < numClusters; j++) {
               const BrainModelSurfaceMetricClustering::Cluster* cluster = bmsmc->getCluster(j);
               const int numNodesInCluster = cluster->getNumberOfNodesInCluster();

               //
               // Corrected area is sum of each node's area multiplied by
               // 2 to the power of the selected metric column for the node
               //
               float correctedArea = 0.0;
               if ((areaCorrectionShapeFile != NULL) ||
                   (areaCorrectionShapeFileColumn >= 0)) {
                  for (int k = 0; k < numNodesInCluster; k++) {
                     const int nodeNum = cluster->getNodeInCluster(k);
                     float nodeArea = bmsmc->getNodeArea(nodeNum);
                     const double metric = areaCorrectionShapeFile->getValue(nodeNum, areaCorrectionShapeFileColumn);
                     correctedArea += (nodeArea * std::pow(2.0, metric));
                  }
               }
               
               //
               // Save the cluster information
               //
               float cog[3];
               cluster->getCenterOfGravity(cog);
               Cluster c;
               c.column = i + iThread + 1;
               c.numberOfNodes = cluster->getNumberOfNodesInCluster();
               c.nodes = cluster->getNodesInCluster();
               c.area = cluster->getArea();
               c.areaCorrected = correctedArea;
               c.cogX = cog[0];
               c.cogY = cog[1];
               c.cogZ = cog[2];
               float threshMin, threshMax;
               cluster->getThresholds(threshMin, threshMax);
               c.threshMin = threshMin;
               c.threshMax = threshMax;
               clusters.push_back(c);
            } // for (int j = 0; j < numClusters
            
            //
            // Should only largest cluster be used
            //
            if (useLargestClusterPerColumnFlag) {
               if (clusters.empty() == false) {
                  std::sort(clusters.begin(), clusters.end());
                  const int largestClusterIndex = static_cast<int>(clusters.size() - 1);
                  clustersOut.push_back(clusters[largestClusterIndex]);
               }
            }
            else {
               clustersOut.insert(clustersOut.end(), clusters.begin(), clusters.end());
            }
         } // if (algorithmThreads[iThread]
         
         //
         // Free up memory
         //
         if (algorithmThreads[iThread] != NULL) {
            delete algorithmThreads[iThread];
            algorithmThreads[iThread] = NULL;
         }
         if (clusterAlgorithms[iThread] != NULL) {
            delete clusterAlgorithms[iThread];
            clusterAlgorithms[iThread] = NULL;
         }
      } // for (int iThread
   }  // for (int i = startColumn
   
   //
   // Sort clusters so biggest elements first
   //
   std::sort(clustersOut.begin(), clustersOut.end());
   std::reverse(clustersOut.begin(), clustersOut.end());
}
*/

/**
 * print the clusters
 */
void 
BrainModelSurfaceMetricFindClustersBase::printClusters(QTextStream& stream, 
                                                         const std::vector<Cluster>& clusters,
                                                         const float sigArea)
{
   //                  1         2         3         4         5         6         7         8         9       100       110
   //         12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
   stream << "Column    Thresh  Num-Nodes          Area  Area-Corrected     COG-X     COG-Y     COG-Z   P-Value" 
          << "\n";
    
   for (std::vector<Cluster>::const_iterator it = clusters.begin();
        it != clusters.end(); it++) {
      const Cluster& c = *it;
      if (c.areaCorrected >= sigArea) {
         float thresh = c.threshMin;
         if (thresh < 0.0) {
            thresh = c.threshMax;
         }
         QString str = QString("%1  %2  %3  %4  %5  %6  %7  %8")
                              .arg(c.column, 6)
                              .arg(thresh, 8, 'f', 3)
                              .arg(c.numberOfNodes, 9)
                              .arg(c.area, 12, 'f', 6)
                              .arg(c.areaCorrected, 14, 'f', 6)
                              .arg(c.cogX, 8, 'f', 3)
                              .arg(c.cogY, 8, 'f', 3)
                              .arg(c.cogZ, 8, 'f', 3);
         stream << str;
         if (c.pValue >= 0.0) {
            stream << QString("  %1").arg(c.pValue, 8, 'f', 6);
         }
         stream << "\n";
      }
   }
}
      
/**
 * create the clusters metric file.
 */
void 
BrainModelSurfaceMetricFindClustersBase::createClustersMetricFile(const std::vector<Cluster>& clusters,
                                                                  const int statisticalMapColumn,
                                                                  const int numNodes)
{
   if (clustersMetricFileName.isEmpty()) {
      return;
   }
   
   MetricFile metricFile;
   metricFile.setNumberOfNodesAndColumns(numNodes, 2);
   metricFile.setColumnName(0, statisticalMapShapeFile->getColumnName(statisticalMapColumn));
   metricFile.setColumnName(1, "1 - P");
   
   for (std::vector<Cluster>::const_iterator it = clusters.begin();
        it != clusters.end(); it++) {
      const Cluster& c = *it;
      if (c.areaCorrected > 0) {
         for (int n = 0; n < c.numberOfNodes; n++) {
            const float q = 1.0 - c.pValue;
            const int nodeNum = c.nodes[n];
            metricFile.setValue(nodeNum, 0, statisticalMapShapeFile->getValue(nodeNum, statisticalMapColumn));
            metricFile.setValue(nodeNum, 1, q);
         }
      }
   }
   
   try {
      metricFile.writeFile(clustersMetricFileName);
   }
   catch (FileException&) {
   }
}
                                    
/**
 * create the clusters paint file.
 */
void 
BrainModelSurfaceMetricFindClustersBase::createClustersPaintFile(const std::vector<Cluster>& clusters,
                                                           const float sigArea,
                                                           const int numNodes)
{
   if (clustersPaintFileName.isEmpty()) {
      return;
   }
   
   PaintFile paintFile;
   paintFile.setNumberOfNodesAndColumns(numNodes, 1);
   paintFile.setColumnName(0, "Clusters");
   
   for (std::vector<Cluster>::const_iterator it = clusters.begin();
        it != clusters.end(); it++) {
      const Cluster& c = *it;
      if (c.areaCorrected >= sigArea) {
/*
         std::ostringstream str;
         if (c.threshMax < 0.0) {
            str << "minus_";
         }
         else {
            str << "plus_";
         }
         str << "cluster_area_"
             << c.areaCorrected
             << "_nodes_"
             << c.numberOfNodes;
         const int paintIndex = paintFile.addPaintName(str.str().c_str());
*/
         const int paintIndex = paintFile.addPaintName(c.name);
         
         for (int n = 0; n < c.numberOfNodes; n++) {
            paintFile.setPaint(c.nodes[n], 0, paintIndex);
         }
      }
   }
   
   try {
      paintFile.writeFile(clustersPaintFileName);
   }
   catch (FileException&) {
   }
}
                          
/**
 * set names of all clusters.
 */
void 
BrainModelSurfaceMetricFindClustersBase::setNamesForClusters(std::vector<Cluster>& clusters)
{
   for (std::vector<Cluster>::iterator it = clusters.begin();
        it != clusters.end(); it++) {
      Cluster& c = *it;
      c.setName();
   }
}
      
/**
 * create the clusters report file.
 */
void 
BrainModelSurfaceMetricFindClustersBase::createMetricShapeClustersReportFile(
                                                     const std::vector<Cluster>& clusters,
                                                     const QString& metricShapeFileName)
{
   //
   // Text file for ROI report
   //
   TextFile textFile;
   
   //
   // Read the metric file
   //
   MetricFile inputMetricFile;
   try {
      inputMetricFile.readFile(metricShapeFileName);
   }
   catch (FileException& e) {
      textFile.appendLine(e.whatQString());
   }
   
   //
   // loop through the clusters
   //
   for (std::vector<Cluster>::const_iterator it = clusters.begin();
        it != clusters.end(); it++) {
      const Cluster& c = *it;
      
      //
      // Set nodes in ROI to nodes in cluster
      //
      BrainModelSurfaceROINodeSelection surfaceROI(brain);
      surfaceROI.deselectAllNodes();
      for (int i = 0; i < c.numberOfNodes; i++) {
         surfaceROI.setNodeSelected(c.nodes[i], 1);
      }
      
      //
      // Files for input to ROI operation
      //
      MetricFile* mf = NULL;
      std::vector<bool> selectedMetricColumns;
      MetricFile* ssf = NULL;
      std::vector<bool> selectedShapeColumns;
      PaintFile* pf = NULL;
      std::vector<bool> selectedPaintColumns;
      LatLonFile* llf = NULL;
      int latLonFileColumn = 0;
      if (metricShapeFileName.endsWith(SpecFile::getSurfaceShapeFileExtension())) {
         ssf = &inputMetricFile;
         const int numCols = ssf->getNumberOfColumns();
         selectedShapeColumns.resize(numCols, true);
      }
      else {
         mf = &inputMetricFile;
         const int numCols = mf->getNumberOfColumns();
         selectedMetricColumns.resize(numCols, true);
      }
      if (brain->getLatLonFile()->getNumberOfColumns() > 0) {
         llf = brain->getLatLonFile();
      }
      
      //
      // Perform the ROI operation
      //
      BrainModelSurfaceROITextReport bmsri(brain,
                                              bms,
                                              &surfaceROI,
                                            mf,
                                            selectedMetricColumns,
                                            ssf,
                                            selectedShapeColumns,
                                            pf,
                                            selectedPaintColumns,
                                            llf,
                                            latLonFileColumn,
                                            c.name,
                                            areaCorrectionShapeFile,
                                            areaCorrectionShapeFileColumn,
                                            false);
      try {
         bmsri.execute();
         textFile.appendLine(bmsri.getReportText());
      }
      catch (BrainModelAlgorithmException& e) {
         textFile.appendLine(e.whatQString());
      }
   }

   //
   // Write the text file containing the clusters ROI report
   //
   try {
      const QString textFileName(FileUtilities::basename(metricShapeFileName) 
                                 + ".ClusterROI_report"
                                 + SpecFile::getTextFileExtension());
      textFile.writeFile(textFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
   }
}
                                    
/**
 * Free memory.
 */
void
BrainModelSurfaceMetricFindClustersBase::cleanUp()
{
   if (areaCorrectionShapeFile != NULL) {
      delete areaCorrectionShapeFile;
      areaCorrectionShapeFile = NULL;
   }
   if (statisticalMapShapeFile != NULL) {
      delete statisticalMapShapeFile;
      statisticalMapShapeFile = NULL;
   }
   if (shuffleStatisticalMapShapeFile != NULL) {
      delete shuffleStatisticalMapShapeFile;
      shuffleStatisticalMapShapeFile = NULL;
   }
   if (brain != NULL) {
      delete brain;
      brain = NULL;
   }
   
   //
   // Do not delete:
   //
   // bms - it is owned by "brain"
}

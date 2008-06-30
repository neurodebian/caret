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
#include <limits>

#include <QDateTime>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricAnovaTwoWay.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "StatisticAnovaTwoWay.h"
#include "StatisticDataGroup.h"

/**
 * constructor.
 */
BrainModelSurfaceMetricAnovaTwoWay::BrainModelSurfaceMetricAnovaTwoWay(
                                BrainSet* bs,
                                const ANOVA_MODEL_TYPE anovaModelTypeIn,
                                const int numberOfRowsIn,
                                const int numberOfColumnsIn,
                                const QString& fiducialCoordFileNameIn,
                                const QString& openTopoFileNameIn,
                                const QString& areaCorrectionShapeFileNameIn,
                                const QString& fMapFileNameIn,
                                const QString& shuffledFMapFileNameIn,
                                const QString& clustersPaintFileNameIn,
                                const QString& clustersMetricFileNameIn,
                                const QString& reportFileNameIn,
                                const int areaCorrectionShapeFileColumnIn,
                                const int iterationsIn,
                                const float positiveThreshIn,
                                const float pValueIn,
                                const bool doFMapDOFIn,
                                const bool doFMapPValueIn,
                                const int numberOfThreadsIn)
   : BrainModelSurfaceMetricFindClustersBase(bs,
                                            fiducialCoordFileNameIn,
                                            openTopoFileNameIn,
                                            areaCorrectionShapeFileNameIn,
                                            fMapFileNameIn,
                                            shuffledFMapFileNameIn,
                                            clustersPaintFileNameIn,
                                            clustersMetricFileNameIn,
                                            reportFileNameIn,
                                            areaCorrectionShapeFileColumnIn,
                                            -std::numeric_limits<float>::max(), // no neg
                                            positiveThreshIn,
                                            pValueIn,
                                            0,
                                            0.0,
                                            doFMapDOFIn,
                                            doFMapPValueIn,
                                            numberOfThreadsIn)
{
   anovaModelType = anovaModelTypeIn;
   numberOfRows = numberOfRowsIn;
   numberOfColumns = numberOfColumnsIn;
   const int numFiles = numberOfRows * numberOfColumns;
   inputMetricFiles.resize(numFiles);
   inputMetricFileNames.resize(numFiles);
   iterations = iterationsIn;
}
                                
/**
 * destructor.
 */
BrainModelSurfaceMetricAnovaTwoWay::~BrainModelSurfaceMetricAnovaTwoWay()
{
   for (unsigned int i = 0; i < inputMetricFiles.size(); i++) {
      delete inputMetricFiles[i];
      inputMetricFiles[i] = NULL;
   }
   inputMetricFiles.clear();
   
   for (unsigned int i = 0; i < shuffledMetricFiles.size(); i++) {
      delete shuffledMetricFiles[i];
      shuffledMetricFiles[i] = NULL;
   }
   shuffledMetricFiles.clear();
}

/**
 * set metric/shape file for a row and column.
 */
void 
BrainModelSurfaceMetricAnovaTwoWay::setMetricShapeFileName(const int row, 
                                                           const int column,
                                                           const QString& metricFileName) throw (BrainModelAlgorithmException)
{
   const int indx = getFileIndex(row, column);
   if (indx >= 0) {
      inputMetricFileNames[indx] = metricFileName;
   }
   else {
      throw BrainModelAlgorithmException("Invalid indices.");
   }
}
                         
/**
 * get the index into one dimensional array of files or names.
 */
int 
BrainModelSurfaceMetricAnovaTwoWay::getFileIndex(const int rowNumber,
                                                 const int columnNumber) const
{
   const int indx = (rowNumber * numberOfColumns) 
                  + columnNumber;
   const int maxFiles = (numberOfRows * numberOfColumns);
   if ((indx < 0) ||
       (indx >= maxFiles)) {
      return -1;
   }
   
   return indx;
}

/**
 * must be implemented by subclasses.
 */
void 
BrainModelSurfaceMetricAnovaTwoWay::executeClusterSearch() throw (BrainModelAlgorithmException)
{
   //
   // Steps in algorithm
   //
   enum { 
      ALG_STEP_CHECKING_INPUT,
      ALG_STEP_F_MAP,
      ALG_STEP_SHUFFLED_F_MAP,
      ALG_STEP_FINDING_CLUSTERS_F_MAP,
      ALG_STEP_FINDING_CLUSTERS_SHUFFLED_F_MAP,
      ALG_NUM_STEPS
   };
   
   //
   // Initialize the progress dialog
   //
   createProgressDialog("Two-Way ANOVA",
                        ALG_NUM_STEPS,
                        "BrainModelSurfaceMetricAnovaTwoWay");
   updateProgressDialog("Verifying Input",
                        ALG_STEP_CHECKING_INPUT,
                        ALG_NUM_STEPS);
   
   //
   // Read the input files
   //
   const int numInputFiles = static_cast<int>(inputMetricFileNames.size());
   if (numInputFiles < 2) {
      throw BrainModelAlgorithmException("There must be at least two input metric files.");
   }
   inputMetricFiles.resize(numInputFiles);
   for (int i = 0; i < numInputFiles; i++) {
      if (inputMetricFileNames[i].isEmpty()) {
         throw BrainModelAlgorithmException("Metric file names have not been specified "
                                            "for all rows and columns.");
      }
      inputMetricFiles[i] = new MetricFile;
      try {
         inputMetricFiles[i]->readFile(inputMetricFileNames[i]);
      }
      catch (FileException& e) {
         throw BrainModelAlgorithmException(e);
      }
   }
   
   //
   // Verify number of nodes and columns
   //
   const int numberOfNodes = bms->getNumberOfNodes();
   std::vector<int> numberOfColumns(numInputFiles, 0);
   for (int i = 0; i < numInputFiles; i++) {
      if (inputMetricFiles[i]->getNumberOfNodes() != numberOfNodes) {
         const QString msg("Files have different number of nodes: \n"
                           + FileUtilities::basename(inputMetricFileNames[0])
                           + " AND " 
                           + FileUtilities::basename(inputMetricFileNames[i]));
         throw BrainModelAlgorithmException(msg);
      }
      
      const int numCols = inputMetricFiles[i]->getNumberOfColumns();
      if (numCols <= 0) {
         const QString msg(FileUtilities::basename(inputMetricFileNames[i])
                           + " contains no data columns.");
         throw BrainModelAlgorithmException(msg);
      }
      numberOfColumns[i] = numCols;
   }
   if (numberOfNodes != areaCorrectionShapeFile->getNumberOfNodes()) {
      throw BrainModelAlgorithmException("Area correction shape file has different number of nodes"
                                         "than the coordinate file.");
   }
   
   //
   // check iterations
   //
   if (iterations < 0) {
      throw BrainModelAlgorithmException("Iterations must be positive.");
   }
   
   //
   // Update progress
   //
   updateProgressDialog("Doing F-Map",
                        ALG_STEP_F_MAP,
                        ALG_NUM_STEPS);
    
   //
   // Create the F-Map Metric (output) File
   //
   int numOutputColumns = 0;
   const int fStatisticColumn = numOutputColumns++;
   int dofColumn = -1;
   if (doStatisticalMapDOF) {
      dofColumn = numOutputColumns++;
   }
   int pValueColumn = -1;
   if (doStatisticalMapPValue) {
      pValueColumn = numOutputColumns++;
   }
   statisticalMapShapeFile = new MetricFile;
   statisticalMapShapeFile->setNumberOfNodesAndColumns(numberOfNodes, numOutputColumns);

   //
   // Set the file comment
   //
   QString fileComment("Two way ANOVA for files: \n");
   for (int i = 0; i < numInputFiles; i++) {
      fileComment += ("   "
                      + FileUtilities::basename(inputMetricFiles[i]->getFileName())
                      + "\n");
   }
   statisticalMapShapeFile->setFileComment(fileComment);
   
   //
   // Create the F-Statistic Metric file
   //
   performFTest(inputMetricFiles,
                 statisticalMapShapeFile,
                 fStatisticColumn,
                 dofColumn,
                 pValueColumn);
   
   //
   // Write the F-Statistic metric file
   //
   try {
      statisticalMapShapeFile->writeFile(statisticalMapFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
   //
   // Should a shuffled Statistical Map be created
   //
   if (iterations > 0) {
      //
      // Update progress
      //
      updateProgressDialog("Doing Shuffled F-Map",
                           ALG_STEP_SHUFFLED_F_MAP,
                           ALG_NUM_STEPS);
      //
      // Create shuffled statistical map metric file
      //
      shuffleStatisticalMapShapeFile = new MetricFile;
      shuffleStatisticalMapShapeFile->setNumberOfNodesAndColumns(numberOfNodes,
                                                                 iterations);
              
      //
      // Create metric files that will be the output of the shuffle process
      //
      for (int i = 0; i < numInputFiles; i++) {
         shuffledMetricFiles.push_back(new MetricFile(*inputMetricFiles[i]));
      }
      
      //
      // Perform for specified number of iterations
      //
      for (int i = 0; i < iterations; i++) {
         //
         // Shuffle the input files
         //
         try {
            MetricFile::shuffle(inputMetricFiles, shuffledMetricFiles);
         }
         catch (FileException& e) {
            throw BrainModelAlgorithmException(e);
         }
         
         //
         // Create the F-Statistic
         //
         performFTest(shuffledMetricFiles,
                      shuffleStatisticalMapShapeFile,
                      i,
                      -1,
                      -1);
      }

      //
      // Write the shuffled F-Statistic metric file
      //
      try {
         shuffleStatisticalMapShapeFile->writeFile(shuffledStatisticalMapFileName);
      }
      catch (FileException& e) {
         throw BrainModelAlgorithmException(e);
      }   
      
      //
      // Update progress
      //
      updateProgressDialog("Finding Clusters in F-Map",
                           ALG_STEP_FINDING_CLUSTERS_F_MAP,
                           ALG_NUM_STEPS);
      //
      // find the clusters in F-Map
      // Note: column 2 is the F-Map
      //
      std::vector<Cluster> fMapClusters;
      findClusters(statisticalMapShapeFile, fMapClusters, "Finding Clusters in F-Map", 
                   fStatisticColumn, false);
         
      //
      // Update progress
      //
      updateProgressDialog("Finding Clusters in Shuffled F-Map",
                           ALG_STEP_FINDING_CLUSTERS_SHUFFLED_F_MAP,
                           ALG_NUM_STEPS);

                           
      //
      // find the clusters in Shuffled F-Map
      // Note: Only use largest cluster from each column
      //
      std::vector<Cluster> shuffleFMapClusters;
      findClusters(shuffleStatisticalMapShapeFile, shuffleFMapClusters, "Finding Clusters in Shuffled F-Map", 
                   -1, true);
      
      //
      // Find area of the "P-Value" cluster in the shuffled F-Map
      //
      float significantCorrectedArea = std::numeric_limits<float>::max();
      int pValueClusterIndex = -1;
      if (shuffleFMapClusters.empty() == false) {
         pValueClusterIndex = std::min(static_cast<int>(pValue * iterations) - 1,
                                       static_cast<int>(shuffleFMapClusters.size()));
         pValueClusterIndex = std::max(pValueClusterIndex, 0);
      }
      if (pValueClusterIndex >= 0) {
         significantCorrectedArea = shuffleFMapClusters[pValueClusterIndex].areaCorrected;
      }
      
      //
      // Find P-Value for significant clusters in T-Map
      //
      for (unsigned int i = 0; i < fMapClusters.size(); i++) {
         Cluster& fMapCluster = fMapClusters[i];
         int cnt = shuffleFMapClusters.size() - 1;
         if (shuffleFMapClusters.empty() == false) {
            if (fMapCluster.areaCorrected > shuffleFMapClusters[0].areaCorrected) {
               cnt = 0;
            }
            else {
               for (unsigned int j = 0; j < shuffleFMapClusters.size() - 1; j++) {
                  if ((fMapCluster.areaCorrected < shuffleFMapClusters[j].areaCorrected) &&
                      (fMapCluster.areaCorrected >= shuffleFMapClusters[j+1].areaCorrected)) {
                     cnt = j;
                  }
               }
            }
         }
         cnt = std::min(cnt, iterations);
         fMapCluster.pValue = static_cast<float>(cnt)
                            / static_cast<float>(iterations);
      }
      
      //
      // For each F-Map that has area corrected that exceeds significant area
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
      for (int m = 0; m < numInputFiles; m++) {
      reportStream << "Shape File :         " << inputMetricFileNames[m] << "\n";
      }
      reportStream << "Fiducial Coord File: " << fiducialCoordFileName << "\n";
      reportStream << "Open Topo File:      " << openTopoFileName << "\n";
      if ((areaCorrectionShapeFile != NULL) ||
          (areaCorrectionShapeFileColumn >= 0)) {
         reportStream << "Area Correct File:   " << areaCorrectionShapeFileName << "\n";
         reportStream << "Area Correct Column: " << areaCorrectionShapeFile->getColumnName(areaCorrectionShapeFileColumn) << "\n";
      }
      reportStream << "Positive Threshold:  " << positiveThresh << "\n";
      reportStream << "Iterations:          " << iterations << "\n";
      reportStream << "P-Value:             " << pValue << "\n";
      reportStream << "Significant Area:    " << significantCorrectedArea << "\n";
      reportStream << "\n";
      
      //
      // Add significant clusters to report file
      //
      reportStream << "Shuffled FMap" << "\n";
      reportStream << "-------------" << "\n";
      printClusters(reportStream, shuffleFMapClusters, significantCorrectedArea);
      reportStream << "\n\n\n";
      reportStream << "TMap" << "\n";
      reportStream << "----" << "\n";
      printClusters(reportStream, fMapClusters, significantCorrectedArea);
      
      reportStream << "\n\n\n";
      
      //
      // Add all clusters to report file
      //
      reportStream << "Shuffled FMap" << "\n";
      reportStream << "-------------" << "\n";
      printClusters(reportStream, shuffleFMapClusters);
      reportStream << "\n\n\n";
      reportStream << "FMap" << "\n";
      reportStream << "----" << "\n";
      printClusters(reportStream, fMapClusters);
      
      //
      // Close the report file
      //
      reportFile.close();
      
      //
      // Do the clusters paint file
      //
      createClustersPaintFile(fMapClusters, significantCorrectedArea, numberOfNodes);
      
      //
      // Do the clusters metric file
      //
      createClustersMetricFile(fMapClusters, fStatisticColumn, numberOfNodes);

      //
      // Do cluster reports
      //
      for (unsigned int i = 0; i < inputMetricFileNames.size(); i++) {
         createMetricShapeClustersReportFile(fMapClusters,
                                             inputMetricFileNames[i]);
      }
   } // if (iterations > 0
}

void
BrainModelSurfaceMetricAnovaTwoWay::performFTest(const std::vector<MetricFile*>& metricFiles,
                                                MetricFile* outputMetricFile,
                                                const int fStatisticColumn,
                                                const int dofColumn,
                                                const int pValueColumn) throw (BrainModelAlgorithmException)
{
   StatisticAnovaTwoWay::ANOVA_MODEL_TYPE statisticAnovaModel = 
                                          StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_INVALID;
   switch (anovaModelType) {
      case ANOVA_MODEL_TYPE_INVALID:
         throw BrainModelAlgorithmException("ANOVA model type is invalid");
         break;
      case ANOVA_MODEL_TYPE_FIXED_EFFECT:
         statisticAnovaModel = StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_FIXED_EFFECT;
         break;
      case ANOVA_MODEL_TYPE_RANDOM_EFFECT:
         statisticAnovaModel = StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_RANDOM_EFFECT;
         break;
      case ANOVA_MODEL_TYPE_ROWS_FIXED_EFFECT_COLUMN_RANDOM_EFFECT:
         statisticAnovaModel = StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_MIXED_EFFECT;
         break;
   }
   
   const int numberOfNodes = metricFiles[0]->getNumberOfNodes();
   //const int numInputFiles = static_cast<int>(metricFiles.size());

   //
   // Set column names
   //
   outputMetricFile->setColumnName(fStatisticColumn, "F-Statistic");
   if (dofColumn >= 0) {
      outputMetricFile->setColumnName(dofColumn, "DOF");
   }
   if (pValueColumn >= 0) {
      outputMetricFile->setColumnName(pValueColumn, "P-Value");
   }
   
   //
   // Loop through the nodes and create the F-Statistic for each node
   //
   for (int i = 0; i < numberOfNodes; i++) {
      StatisticAnovaTwoWay anova;
      anova.setNumberOfFactorLevels(numberOfRows, numberOfColumns);
      anova.setAnovaModelType(statisticAnovaModel);
      
      //
      // Create the data groups and add them to the algorithm
      //
      for (int m = 0; m < numberOfRows; m++) {
         for (int n = 0; n < numberOfColumns; n++) {
            //
            // Note, the anova algorithm will take care of deleting everything
            //
            const int indx = getFileIndex(m, n);
            const int numData = metricFiles[indx]->getNumberOfColumns();
            float* data = new float[numData];
            metricFiles[indx]->getAllColumnValuesForNode(i, data);
            anova.setDataArray(m, n, data, numData, true);
         }
      }
      
      //
      // Execute the two-way anova algorithm
      //
      try {
         anova.execute();
      }
      catch (StatisticException& e) {
         throw BrainModelAlgorithmException(e);
      }
      
      //
      // Get the outputs of the anova algorithm
      //
      outputMetricFile->setValue(i, fStatisticColumn, anova.getfStatisticInteraction());
      if (dofColumn >= 0) {
         outputMetricFile->setValue(i, dofColumn, anova.getDegreesOfFreedomInteractions());
      }
      if (pValueColumn >= 0) {
         outputMetricFile->setValue(i, pValueColumn, anova.getPValueInteraction());
      }
   }
   
}


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
#include "BrainModelSurfaceMetricCoordinateDifference.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "PaintFile.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "StatisticDataGroup.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceMetricCoordinateDifference::BrainModelSurfaceMetricCoordinateDifference(
                                                BrainSet* bs,
                                                const MODE modeIn,
                                                const std::vector<QString>& coordFileNameGroupAIn,
                                                const std::vector<QString>& coordFileNameGroupBIn,
                                                const QString& fiducialCoordFileNameIn,
                                                const QString& openTopoFileNameIn,
                                                const QString& distortionShapeFileNameIn,
                                                const QString& distanceMetricFileNameIn,
                                                const QString& shuffledDistanceMetricFileNameIn,
                                                const QString& clustersPaintFileNameIn,
                                                const QString& clustersMetricFileNameIn,
                                                const QString& reportFileNameIn,
                                                const int distortionShapeFileColumnIn,
                                                const int iterationsIn,
                                                const float thresholdIn,
                                                const float pValueIn,
                                                const int numberOfThreadsIn)
   : BrainModelSurfaceMetricFindClustersBase(bs,
                                            fiducialCoordFileNameIn,
                                            openTopoFileNameIn,
                                            distortionShapeFileNameIn,
                                            distanceMetricFileNameIn, //tMapFileNameIn,
                                            shuffledDistanceMetricFileNameIn, //shuffledTMapFileNameIn,
                                            clustersPaintFileNameIn,
                                            clustersMetricFileNameIn,
                                            reportFileNameIn,
                                            distortionShapeFileColumnIn,
                                            0.0, // negative threshold
                                            thresholdIn,
                                            pValueIn,
                                            0,   // variance smoothing iterations
                                            0.0, // variance smoothing strength
                                            false,  // do DOF
                                            false,  // do P-Value
                                            numberOfThreadsIn)
{
   mode = modeIn;
   coordFileNameGroupA = coordFileNameGroupAIn;
   coordFileNameGroupB = coordFileNameGroupBIn;
   iterations = iterationsIn;
}
      
/**
 * destructor.
 */
BrainModelSurfaceMetricCoordinateDifference::~BrainModelSurfaceMetricCoordinateDifference()
{
   for (unsigned int i = 0; i < coordGroupA.size(); i++) {
      delete coordGroupA[i];
   }
   coordGroupA.clear();
   
   for (unsigned int i = 0; i < coordGroupB.size(); i++) {
      delete coordGroupB[i];
   }
   coordGroupB.clear();
   
   cleanUp();
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceMetricCoordinateDifference::executeClusterSearch() throw (BrainModelAlgorithmException)
{
   try {
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
      //createProgressDialog("Coordinate Distance Test",
      //                     ALG_NUM_STEPS,
      //                     "sifClustDialog");
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
      // Read the group A coordinate files
      //
      for (unsigned int i = 0; i < coordFileNameGroupA.size(); i++) {
         CoordinateFile* cf = new CoordinateFile;
         cf->readFile(coordFileNameGroupA[i]);
         coordGroupA.push_back(cf);
      }
      
      //
      // Read the group B coordinate files
      //
      for (unsigned int i = 0; i < coordFileNameGroupB.size(); i++) {
         CoordinateFile* cf = new CoordinateFile;
         cf->readFile(coordFileNameGroupB[i]);
         coordGroupB.push_back(cf);
      }
      
      //
      // Check input files
      //
      if (coordGroupA.empty()) {
         throw BrainModelAlgorithmException("No coordinate files in Group A");
      }
      if (coordGroupB.empty()) {
         throw BrainModelAlgorithmException("No coordinate files in Group B");
      }
      
      //
      // Number of nodes in surface
      //
      const int numberOfNodes = coordGroupA[0]->getNumberOfCoordinates();
      
      //
      // Create average coordinate files for group A
      //
      CoordinateFile coordAverageGroupA;
      CoordinateFile::createAverageCoordinateFile(coordGroupA,
                                                  coordAverageGroupA);
      
      //
      // Create average coordinate files for group B
      //
      CoordinateFile coordAverageGroupB;
      CoordinateFile::createAverageCoordinateFile(coordGroupB,
                                                  coordAverageGroupB);

      //
      // Compute the deviations
      //
      MetricFile deviationGroupA;
      computeCoordinateDeviation(coordGroupA,
                                 coordAverageGroupA,
                                 deviationGroupA);
      MetricFile deviationGroupB;
      computeCoordinateDeviation(coordGroupB,
                                 coordAverageGroupB,
                                 deviationGroupB);
                                 
      //
      // Check file compatibilities
      //
      if (numberOfNodes != bms->getNumberOfNodes()) {
         throw BrainModelAlgorithmException("Fiducial Coord has different number of nodes than coord file group.");
      }
      if (numberOfNodes != areaCorrectionShapeFile->getNumberOfNodes()) {
         throw BrainModelAlgorithmException("Area correction shape file has different number of nodes"
                                            "than the coordinate file.");
      }
      
      //
      // Update progress
      //
      updateProgressDialog("Doing Real Distance File",
                           ALG_STEP_TMAP,
                           ALG_NUM_STEPS);
      //
      // Create Distance File
      //
      statisticalMapShapeFile = new MetricFile;
      switch (mode) {
         case MODE_COORDINATE_DIFFERENCE:
            statisticalMapShapeFile->setNumberOfNodesAndColumns(numberOfNodes, 4);
            statisticalMapShapeFile->addColumnOfCoordinateDifference(&coordAverageGroupA,
                                                           &coordAverageGroupB,
                                                           bms->getTopologyFile(),
                                                           0,
                                                           "Coord Difference",
                                                           "Difference of two average coord files",
                                                           1,
                                                           2,
                                                           3);
            break;
         case MODE_TMAP_DIFFERENCE:
            statisticalMapShapeFile->setNumberOfNodesAndColumns(numberOfNodes, 1);
            statisticalMapShapeFile->addColumnOfCoordinateDifferenceTMap(&coordAverageGroupA,
                                                           &coordAverageGroupB,
                                                           bms->getTopologyFile(),
                                                           0,
                                                           "T-Map Coord Difference",
                                                           "T-Map of Difference of two average coord files",
                                                           &deviationGroupA,
                                                           0,
                                                           &deviationGroupB,
                                                           0,
                                                           true);
            try {
               coordAverageGroupA.writeFile("A_AverageFiducial.coord");
               coordAverageGroupB.writeFile("B_AverageFiducial.coord");
               deviationGroupA.writeFile("A_3D_VARIABILITY.surface_shape");
               deviationGroupB.writeFile("B_3D_VARIABILITY.surface_shape");
            }
            catch (FileException& e) {
               throw BrainModelAlgorithmException(e.whatQString());
            }
            break;
      }
      
      //
      // Write Distance file
      //
      try {
         statisticalMapShapeFile->writeFile(statisticalMapFileName);
      }
      catch (FileException& e) {
         std::ostringstream str;
         str << "Unable to write Distance File: "
             << FileUtilities::basename(statisticalMapFileName).toAscii().constData();
         cleanUp();
         throw BrainModelAlgorithmException(str.str().c_str());
      }
      
      
      //
      // Update progress
      //
      updateProgressDialog("Doing Shuffled Distance File",
                           ALG_STEP_SHUFFLED_TMAP,
                           ALG_NUM_STEPS);
                           
      //
      // Get all coords in one group
      //
      std::vector<CoordinateFile*> allCoords;
      allCoords.insert(allCoords.end(),
                       coordGroupA.begin(),
                       coordGroupA.end());
      allCoords.insert(allCoords.end(),
                       coordGroupB.begin(),
                       coordGroupB.end());
      const int totalNumCoords = static_cast<int>(allCoords.size());
      if (totalNumCoords < 2) {
         throw BrainModelAlgorithmException("There must be at least two coord files.");
      }                 
      
      //
      // Do shuffled Distance File
      //
      shuffleStatisticalMapShapeFile = new MetricFile;
      shuffleStatisticalMapShapeFile->setNumberOfNodesAndColumns(numberOfNodes,
                                                       iterations);
      for (int i = 0; i < iterations; i++) {
         CoordinateFile c1, c2;
         CoordinateFile::createShuffledAverageCoordinatesFiles(allCoords,
                                                               -1,
                                                               c1,
                                                               c2);
         switch (mode) {
            case MODE_COORDINATE_DIFFERENCE:
               shuffleStatisticalMapShapeFile->addColumnOfCoordinateDifference(&c1,
                                                              &c2,
                                                              bms->getTopologyFile(),
                                                              i,
                                                              "Coord Difference",
                                                              "Difference of two average coord files");
               break;
            case MODE_TMAP_DIFFERENCE:
               shuffleStatisticalMapShapeFile->addColumnOfCoordinateDifferenceTMap(&c1,
                                                              &c2,
                                                              bms->getTopologyFile(),
                                                              i,
                                                              "T-Map Coord Difference",
                                                              "T-Map of Difference of two average coord files",
                                                              &deviationGroupA,
                                                              0,
                                                              &deviationGroupB,
                                                              0,
                                                              false);
               break;
         }
      }

      //
      // Write Shuffled  Distance file
      //
      try {
         shuffleStatisticalMapShapeFile->writeFile(shuffledStatisticalMapFileName);
      }
      catch (FileException& e) {
         std::ostringstream str;
         str << "Unable to write Shuffled Distance File: "
             << FileUtilities::basename(shuffledStatisticalMapFileName).toAscii().constData();
         cleanUp();
         throw BrainModelAlgorithmException(str.str().c_str());
      }

      //
      // Update progress
      //
      updateProgressDialog("Finding clusters in Real Distance File",
                           ALG_STEP_FINDING_CLUSTERS_T_MAP,
                           ALG_NUM_STEPS);
                           
      //
      // find the clusters in Distance File
      // Note: column 0 is the Distance column
      //
      std::vector<Cluster> tMapClusters;
      findClusters(statisticalMapShapeFile, tMapClusters, "Finding Clusters in Real Distance File", 0, false);
         
      //
      // Update progress
      //
      updateProgressDialog("Finding Clusters in Shuffled Distance File",
                           ALG_STEP_FINDING_CLUSTERS_SHUFFLE_T_MAP,
                           ALG_NUM_STEPS);
                           
      //
      // find the clusters in Shuffled Distance File
      // Note: Only use largest cluster from each column
      //
      std::vector<Cluster> shuffleTMapClusters;
      findClusters(shuffleStatisticalMapShapeFile, shuffleTMapClusters, "Finding Clusters in Shuffled Distance File", -1, true);
      
      //
      // Find area of the "P-Value" cluster in the shuffled Distance File
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
      // Find P-Value for significant clusters in Distance File
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
      // For each distance that has area corrected that exceeds significant area
      //    Find its rank in shuffled tmap
      //    P-value = rank / iterations
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
      for (unsigned int j = 0; j < coordFileNameGroupA.size(); j++) {
         reportStream << "Coord File Group A:  " << coordFileNameGroupA[j] << "\n";
      }
      for (unsigned int j = 0; j < coordFileNameGroupB.size(); j++) {
         reportStream << "Coord File Group B:  " << coordFileNameGroupB[j] << "\n";
      }
      reportStream << "Fiducial Coord File: " << fiducialCoordFileName << "\n";
      reportStream << "Open Topo File:      " << openTopoFileName << "\n";
      if ((areaCorrectionShapeFile != NULL) ||
          (areaCorrectionShapeFileColumn >= 0)) {
         reportStream << "Area Correct File:   " << areaCorrectionShapeFileName << "\n";
         reportStream << "Area Correct Column: " << areaCorrectionShapeFile->getColumnName(areaCorrectionShapeFileColumn) << "\n";
      }
      //reportStream << "Negative Threshold:  " << negativeThresh << "\n";
      reportStream << "         Threshold:  " << positiveThresh << "\n";
      reportStream << "Iterations:          " << iterations << "\n";
      reportStream << "P-Value:             " << pValue << "\n";
      reportStream << "Significant Area:    " << significantCorrectedArea << "\n";
      reportStream << "\n";
      
      //
      // Add significant clusters to report file
      //
      reportStream << "Shuffled Distance File" << "\n";
      reportStream << "-------------" << "\n";
      printClusters(reportStream, shuffleTMapClusters, significantCorrectedArea);
      reportStream << "\n\n\n";
      reportStream << "Real Distance File" << "\n";
      reportStream << "----" << "\n";
      printClusters(reportStream, tMapClusters, significantCorrectedArea);
      
      reportStream << "\n\n\n";
      
      //
      // Add all clusters to report file
      //
      reportStream << "Shuffled Distance File" << "\n";
      reportStream << "-------------" << "\n";
      printClusters(reportStream, shuffleTMapClusters);
      reportStream << "\n\n\n";
      reportStream << "Real Distance File" << "\n";
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
      createClustersMetricFile(tMapClusters, 0, numberOfNodes);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
}
   
/**
 * compute coordinate deviation.  Assume mean is zero and distance
 * between a coord and average is the value being measured.
 */
void 
BrainModelSurfaceMetricCoordinateDifference::computeCoordinateDeviation(const std::vector<CoordinateFile*>& coordFiles,
                                                              const CoordinateFile& averageCoordFile,
                                                              MetricFile& deviationFile)
{
   float minVal = std::numeric_limits<float>::max();
   float maxVal = -std::numeric_limits<float>::max();
   const int numCoordFiles = static_cast<int>(coordFiles.size());
   const float denominator = numCoordFiles - 1;
   const int numNodes = averageCoordFile.getNumberOfCoordinates();
   deviationFile.setNumberOfNodesAndColumns(numNodes, 1);
   for (int i = 0; i < numNodes; i++) {
      float sumSquared = 0.0;
      for (int j = 0; j < numCoordFiles; j++) {
         const float delta = MathUtilities::distance3D(coordFiles[j]->getCoordinate(i),
                                                       averageCoordFile.getCoordinate(i));
         sumSquared += (delta * delta);  // note: mean is always zero
      }
      
      if (denominator > 0.0) {
         sumSquared = std::sqrt(sumSquared / denominator);
      }
      deviationFile.setValue(i, 0, sumSquared);
      
      minVal = std::min(minVal, sumSquared);
      maxVal = std::max(maxVal, sumSquared);
   }
   
   deviationFile.setColumnColorMappingMinMax(0, minVal, maxVal);
}
                                      
/**
 * Free memory.
 */
void
BrainModelSurfaceMetricCoordinateDifference::cleanUp()
{
   BrainModelSurfaceMetricFindClustersBase::cleanUp();
}

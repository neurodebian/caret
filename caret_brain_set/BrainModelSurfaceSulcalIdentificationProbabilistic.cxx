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

#include <QDir>
#include <QFileInfo>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

#include "AreaColorFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricClustering.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceSulcalIdentificationProbabilistic.h"
#include "BrainModelVolumeToSurfaceMapperAlgorithmParameters.h"
#include "BrainModelVolumeToSurfaceMapper.h"
#include "BrainSet.h"
#include "CommaSeparatedValueFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "StringTable.h"
#include "SurfaceShapeFile.h"
#include "ValueIndexSort.h"
#include "VocabularyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceSulcalIdentificationProbabilistic::BrainModelSurfaceSulcalIdentificationProbabilistic(
                                               BrainSet* bs,
                                               const BrainModelSurface* fiducialSurfaceIn,
                                               const BrainModelSurface* inflatedSurfaceIn,
                                               const BrainModelSurface* veryInflatedSurfaceIn,
                                               const PaintFile* inputPaintFileIn,
                                               const int paintFileGeographyColumnNumberIn,
                                               const SurfaceShapeFile* surfaceShapeFileIn,
                                               const int surfaceShapeFileDepthColumnNumberIn,
                                               const QString& probabilisticDepthVolumeCSVFileNameIn,
                                               const float postCentralSulcusOffsetIn,
                                               const float postCentralSulcusStdDevSquareIn,
                                               const float postCentralSulcusSplitIn)
   : BrainModelAlgorithm(bs),
     fiducialSurface(fiducialSurfaceIn),
     inflatedSurface(inflatedSurfaceIn),
     veryInflatedSurface(veryInflatedSurfaceIn),
     inputPaintFile(inputPaintFileIn),
     paintFileGeographyColumnNumber(paintFileGeographyColumnNumberIn),
     surfaceShapeFile(surfaceShapeFileIn),
     surfaceShapeFileDepthColumnNumber(surfaceShapeFileDepthColumnNumberIn),
     probabilisticDepthVolumeCSVFileName(probabilisticDepthVolumeCSVFileNameIn),
     postCentralSulcusOffset(postCentralSulcusOffsetIn),
     postCentralSulcusStdDevSquare(postCentralSulcusStdDevSquareIn),
     postCentralSulcusSplit(postCentralSulcusSplitIn)
{
   defaultAreaColorFile = NULL;   
   outputAreaColorFile = NULL;
   outputPaintFile = NULL;
   defaultVocabularyFile = NULL;
   outputVocabularyFile = NULL;
   probabilisticMetricFile = NULL;
   rotatedVeryInflatedSurface = NULL;
   addingFirstColorFlag = true;
   
   loadColors();
   loadVocabulary();
}

/**
 * destructor.
 */
BrainModelSurfaceSulcalIdentificationProbabilistic::~BrainModelSurfaceSulcalIdentificationProbabilistic()
{
   if (defaultAreaColorFile != NULL) {
      delete defaultAreaColorFile;
      defaultAreaColorFile = NULL;
   }
   if (outputPaintFile != NULL) {
      delete outputPaintFile;
      outputPaintFile = NULL;
   }
   if (outputAreaColorFile != NULL) {
      delete outputAreaColorFile;
      outputAreaColorFile = NULL;
   }
   if (defaultVocabularyFile != NULL) {
      delete defaultVocabularyFile;
      defaultVocabularyFile = NULL;
   }
   if (outputVocabularyFile != NULL) {
      delete outputVocabularyFile;
      outputVocabularyFile = NULL;
   }
   if (probabilisticMetricFile != NULL) {
      delete probabilisticMetricFile;
      probabilisticMetricFile = NULL;
   }
   if (rotatedVeryInflatedSurface != NULL) {
      delete rotatedVeryInflatedSurface;
      rotatedVeryInflatedSurface = NULL;
   }
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::execute() throw (BrainModelAlgorithmException)
{
   //
   // Check inputs
   //
   if (fiducialSurface == NULL) {
      throw BrainModelAlgorithmException("Fiducial surface is invalid.");
   }
   if (veryInflatedSurface == NULL) {
      throw BrainModelAlgorithmException("Very inflated surface is invalid.");
   }
   if (inputPaintFile == NULL) {
      throw BrainModelAlgorithmException("Paint file is invalid.");
   }
   if (surfaceShapeFile == NULL) {
      throw BrainModelAlgorithmException("Surface shape file is invalid.");
   }
   if ((paintFileGeographyColumnNumber < 0) ||
       (paintFileGeographyColumnNumber >= inputPaintFile->getNumberOfColumns())) {
      throw BrainModelAlgorithmException("Paint file column number is invalid.");
   }
   if ((surfaceShapeFileDepthColumnNumber < 0) ||
       (surfaceShapeFileDepthColumnNumber >= surfaceShapeFile->getNumberOfColumns())) {
      throw BrainModelAlgorithmException("Surface shape file column number is invalid.");
   }
   
   //
   // Apply a rotation to the very inflated surface
   //
   rotateVeryInflatedSurface();

   //
   // Get a list of the probabilistic volumes
   //
   readProbabilisticVolumeFileList();
   
   //
   // Copy the input paint file to the output paint file
   //
   outputPaintFile = new PaintFile;
   const int numOldPaints = inputPaintFile->getNumberOfPaintNames();
   for (int i = 0; i < numOldPaints; i++) {
      outputPaintFile->addPaintName(inputPaintFile->getPaintNameFromIndex(i));
   }
   outputPaintFile->setFileName(outputPaintFile->makeDefaultFileName("TEST_Sulcal_ID"));
   
   //
   // Map the probabilistic volumes to metric file
   //
   mapProbabilisticFunctionalVolumes();
   
   //
   // multiply probabilistic functional data by depth
   //
   multiplyProbabilisticFunctionalDataByDepth();
   
   //
   // Create the new initial sulcus ID paint column
   //
   const QString initialSulcusIdName("Initial Sulcus ID");
   const int oldInitSulcusIdColumn = 
      outputPaintFile->getColumnWithName(initialSulcusIdName);
   if (oldInitSulcusIdColumn >= 0) {
      outputPaintFile->removeColumn(oldInitSulcusIdColumn);
   }
   outputPaintFile->copyColumns(inputPaintFile,
                                paintFileGeographyColumnNumber,
                                -1, // create new column
                                initialSulcusIdName);
                          
   const int paintInitialSulciIDColumn = outputPaintFile->getNumberOfColumns() - 1;

   //
   // Make initial sulcal identification
   //
   createInitialSulcalIdentification(paintInitialSulciIDColumn);

   //if (DebugControl::getDebugOn()) {
   //   try {
   //      outputPaintFile->writeFile(outputPaintFile->getFileName());  
   //   }
   //   catch (FileException&) {
   //   }
   //}   
   
   //
   // Copy the initial sulcus ID to sulcus ID
   //
   const QString sulcusIdName(getSulcusIdPaintColumnName());
   const int sulcusIdColumn = 
      outputPaintFile->getColumnWithName(sulcusIdName);
   if (sulcusIdColumn >= 0) {
      outputPaintFile->removeColumn(sulcusIdColumn);
   }
   outputPaintFile->copyColumns(outputPaintFile,
                                paintInitialSulciIDColumn,
                                -1, // create new column
                                sulcusIdName);
                                //addCaseNameToName("Sulcus ID"));
   const int paintSulciIDColumn = outputPaintFile->getNumberOfColumns() - 1;
            
   //
   // Dilate the sulcal identification
   //
   dilateSulcalIdentification(paintSulciIDColumn);

   //
   // Remove the initial sulci id column
   //
   if (DebugControl::getDebugOn() == false) {
      outputPaintFile->removeColumn(paintInitialSulciIDColumn);
   }
}


/**
 * multiply probabilistic volumes by depth.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::multiplyProbabilisticFunctionalDataByDepth() throw (BrainModelAlgorithmException)
{
   const int numColumns = probabilisticMetricFile->getNumberOfColumns();
   const int numNodes   = probabilisticMetricFile->getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
      //
      // Get depth value for node
      //
      const float depthValue = surfaceShapeFile->getValue(i, 
                                                          surfaceShapeFileDepthColumnNumber);
      //
      // Multiply metric values by depth
      //
      for (int j = 0; j < numColumns; j++) {
         probabilisticMetricFile->setValue(i, j,
                                           (probabilisticMetricFile->getValue(i, j)
                                            * depthValue));
      }
   }
   
   //
   // Name columns
   //
   for (int j = 0; j < numColumns; j++) {
      const QString name(brainSet->getSubject()
                         + " Probabilistic "
                         + sulcalNamesAndVolumes[j].getSulcusName()
                         + " X Depth");
      probabilisticMetricFile->setColumnName(j, name);
   }
   
   //
   // Write the metric file
   //
   if (DebugControl::getDebugOn()) {
      try {
         const QString filename = probabilisticMetricFile->makeDefaultFileName("ProbabilisticSulcal_Sulcus_X_Depth");
         probabilisticMetricFile->writeFile(filename);
         //std::cout << "Wrote Prob Metric: "
         //          << filename.toAscii().constData()
         //          << std::endl;
      }
      catch (FileException&) {
      }
   }
}

/**
 * read the probabilistic volume file list.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::readProbabilisticVolumeFileList() throw (BrainModelAlgorithmException)
{
   //
   // Read the file containing the list of sulci, thresholds, and volume files
   //
   CommaSeparatedValueFile csvf;
   try {
      csvf.readFile(probabilisticDepthVolumeCSVFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
   //
   // Directory name of CSV file
   //
   const QString csvDirectoryName = 
      FileUtilities::dirname(probabilisticDepthVolumeCSVFileName);
      
   //
   // Find the needed table in the file
   //
   const QString tableName("ProbInfo");
   const StringTable* st = csvf.getDataSectionByName(tableName);
   if (st == NULL) {
      const QString msg = QString("Unable to find data section named ")
                        + tableName
                        + QString(" in ")
                        + FileUtilities::basename(probabilisticDepthVolumeCSVFileName);
      throw BrainModelAlgorithmException(msg);
   }

   //
   // Find the needed columns in the file
   //
   const QString sulcusNameColumnName("Sulcus Name");
   const QString depthThresholdColumnName("Depth Threshold");
   const QString volumeFileNameColumnName("Volume Name");
   const QString maximumClustersColumnName("Maximum Clusters");
   const int sulcusNameColumnNumber = st->getColumnIndexFromName(sulcusNameColumnName);
   const int depthThresholdColumnNumber = st->getColumnIndexFromName(depthThresholdColumnName);
   const int volumeNameColumnNumber = st->getColumnIndexFromName(volumeFileNameColumnName);   
   const int maximumClustersColumnNumber = st->getColumnIndexFromName(maximumClustersColumnName);
   QString missingColumnsMessage;
   if (sulcusNameColumnNumber < 0) {
      missingColumnsMessage += (sulcusNameColumnName
                                + "\n");
   }
   if (depthThresholdColumnNumber < 0) {
      missingColumnsMessage += (depthThresholdColumnName
                                + "\n");
   }
   if (volumeNameColumnNumber < 0) {
      missingColumnsMessage += (volumeFileNameColumnName
                                + "\n");
   }
   if (maximumClustersColumnNumber < 0) {
      missingColumnsMessage += (maximumClustersColumnName
                                + "\n");
   }
   if (missingColumnsMessage.isEmpty() == false) {
      missingColumnsMessage.insert(0, QString("Required column titles not found in ")
                                      + FileUtilities::basename(probabilisticDepthVolumeCSVFileName)
                                      + "\n");
      throw BrainModelAlgorithmException(missingColumnsMessage);
   }
   
   //
   // Read from the table
   //
   for (int i = 0; i < st->getNumberOfRows(); i++) {
      //
      // Prepend directory name onto volume name, if needed
      //
      QString volumeName = st->getElement(i, volumeNameColumnNumber);
      QFileInfo fi(volumeName);
      if (fi.isAbsolute() == false) {
         volumeName = csvDirectoryName
                      + "/"  //QDir::separator()
                      + volumeName;
      }
      //std::cout << "BEFORE: " << volumeName.toAscii().constData() << std::endl;
      volumeName = volumeName.replace("\\", "/");
      //std::cout << "AFTER: " << volumeName.toAscii().constData() << std::endl;
      
      SulcalNameAndVolume snav(st->getElement(i, sulcusNameColumnNumber),
                               st->getElementAsFloat(i, depthThresholdColumnNumber),
                               volumeName,
                               st->getElementAsInt(i, maximumClustersColumnNumber));
      sulcalNamesAndVolumes.push_back(snav);
   }

   if (sulcalNamesAndVolumes.empty()) {
      throw BrainModelAlgorithmException(QString("No volumes listed in ") 
                                         + probabilisticDepthVolumeCSVFileName);
   }
}
      
/**
 * dilate the sulcal identification.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::dilateSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException)
{
   //
   // Get paint indices for sulcal names
   //
   std::vector<int> paintIndices;
   for (unsigned int i = 0; i < sulcalNamesAndVolumes.size(); i++) {
      paintIndices.push_back(outputPaintFile->getPaintIndexFromName(
                               sulcalNamesAndVolumes[i].getSulcusName()));
   }
   const int numPaintIndices = static_cast<int>(paintIndices.size());
   if (numPaintIndices <= 0) {
      return;
   }
   
   //
   // Find index for paint "SUL"
   //
   const int sulIndex = outputPaintFile->getPaintIndexFromName("SUL");
   if (sulIndex < 0) {
      throw BrainModelAlgorithmException("ERROR: Unable to find paint name \"SUL\".");
   }

   //
   // Find calcarine paint
   //
   const int casIndex = outputPaintFile->getPaintIndexFromName("SUL.CaS");
   const float casMaxZ = 13.0;
   const float casMaxY = -53.0;
   
   //
   // Find index for hippocampal fissure
   //
   const int hfIndex = outputPaintFile->getPaintIndexFromName("SUL.HF");
   const float hfMaxZ = 5.0;
   
   //
   // Dilate the paints until not more dilations can be performed
   //
   bool done = false;
   std::vector<int> paintFullyDilated(numPaintIndices, 0);
   while (done == false) {
      int numDilationsPerformed = 0;
      for (int i = 0; i < numPaintIndices; i++) {
         if (paintFullyDilated[i] == 0) {
            //
            // Limit extent of dilation for some cases
            //
            float maxExtent[6] = {
               -std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
               -std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
               -std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
            };
            if (paintIndices[i] == hfIndex) {
               maxExtent[5] = hfMaxZ;
            }
            if (paintIndices[i] == casIndex) {
               maxExtent[5] = casMaxZ;
               maxExtent[3] = casMaxY;
            }
            const int dilateCount = outputPaintFile->dilatePaintID(
                                                     fiducialSurface->getTopologyFile(),
                                                     fiducialSurface->getCoordinateFile(),
                                                     paintColumn,
                                                     1,
                                                     paintIndices[i],
                                                     sulIndex,
                                                     maxExtent);
            if (dilateCount <= 0) {
               paintFullyDilated[i] = 1;
            }                                         
            numDilationsPerformed += dilateCount;
         }
      }
      if (numDilationsPerformed <= 0) {
         done = true;
      }
   }
}
      
/**
 * add casename to name.
 */
QString 
BrainModelSurfaceSulcalIdentificationProbabilistic::addCaseNameToName(const QString& name) const
{
   QString nameOut = name;
   
   if (brainSet->getSubject().isEmpty() == false) {
      nameOut += ("." 
                  + brainSet->getSubject());
      if (brainSet->getStructure().getTypeAsAbbreviatedString().isEmpty() == false) {
         nameOut += ("."
                     + brainSet->getStructure().getTypeAsAbbreviatedString());
      }
   }
   
   return nameOut;
}
      
/**
 * find clusters using sulcal depth.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::createInitialSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException)
{
   const QString centralSulcusName("SUL.CeS");
   float centralSulcusCOG[3] = { 0.0, 0.0, 0.0 };
   const QString postCentralSulcusName("SUL.PoCeS");
   const QString superiorTemporalSulcusName("SUL.STS");
   
   std::vector<QString> allowRelaxedDepthNames;
   const QString hippocampalFissureName("SUL.HF");
   allowRelaxedDepthNames.push_back(hippocampalFissureName);
   
   const int numColumns = probabilisticMetricFile->getNumberOfColumns();
   for (int j = 0; j < numColumns; j++) {
   
      BrainModelSurfaceMetricClustering* bmsmc = NULL;
      
      const bool hippocampalFissureFlag = 
         (sulcalNamesAndVolumes[j].getSulcusName() == hippocampalFissureName);
         
      //
      // Some depths may be too deep, so if no clusters found, relax depth
      // and try again
      //
      float depthTestValue = sulcalNamesAndVolumes[j].getDepthThreshold();
      const int numTries = 10;
      for (int mm = 0; mm < numTries; mm++) {
         //
         // Find the clusters with specified depth range
         //
         if (bmsmc != NULL) {
            delete bmsmc;
            bmsmc = NULL;
         }
         bmsmc = new BrainModelSurfaceMetricClustering(brainSet,
                                              (BrainModelSurface*)fiducialSurface,
                                              probabilisticMetricFile,
                                              BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_NUMBER_OF_NODES,
                                              j,           // column number
                                              -1,          // output column
                                              "Clusters",  // output column  name
                                              1,           // min number nodes
                                              0.001,       // min surface area
                                              depthTestValue,       // neg min
                                              -100000000.0,// neg max
                                              5.0,         // pos min
                                              4.0,         // pos max
                                              false);      // only clusters with min nodes or more
         try {
            bmsmc->execute();
            
            //
            // Should depth be relaxed
            //
            if ((bmsmc->getNumberOfClusters() <= 0) &&
                (std::find(allowRelaxedDepthNames.begin(),
                           allowRelaxedDepthNames.end(),
                           sulcalNamesAndVolumes[j].getSulcusName()) !=
                        allowRelaxedDepthNames.end())) {
               depthTestValue += 5.0;
            }
            else {
               mm = numTries;  // get out of loop
            }
         }
         catch (BrainModelAlgorithmException& e) {
            throw BrainModelAlgorithmException("Finding clusters: " + e.whatQString());
         }
      }
      
      const int numClusters = bmsmc->getNumberOfClusters();

      //
      // Sum the total values of probabilistic times depth for each cluster
      // and sort indices
      //
      ValueIndexSort vis;
      for (int i = 0; i < numClusters; i++) {
         //
         // Look at the nodes in each cluster
         //
         double sum = 0.0;
         BrainModelSurfaceMetricClustering::Cluster* cluster = bmsmc->getCluster(i);
         const int numClusterNodes = cluster->getNumberOfNodesInCluster();
         for (int k = 0; k < numClusterNodes; k++) {
            const int node = cluster->getNodeInCluster(k);
            sum += probabilisticMetricFile->getValue(node, j);
         }

         if (hippocampalFissureFlag) {
            float cog[3];
            cluster->getCenterOfGravityForSurface(fiducialSurface, cog);
            if (DebugControl::getDebugOn()) {
               std::cout << hippocampalFissureName.toAscii().constData()
                         << " cluster[" << i << "]" 
                         << " area="  <<  cluster->getArea()
                         << " num-nodes=" << cluster->getNumberOfNodesInCluster()
                         << " sum=" << sum
                         << " cog=(" << cog[0]
                         <<    "," << cog[1]
                         <<    "," << cog[2]
                         <<    ")"
                         <<  std::endl;
            }
            //
            // Too far posterior ?
            //
            //if (cog[1] < -34.0) {
            //   continue;
            //}
         }
         
         //
         // Special processing for post-central sulcus
         // bias in favor of clusters about 10mm posterior to CeS
         //
         if (sulcalNamesAndVolumes[j].getSulcusName() == postCentralSulcusName) {
            if (centralSulcusCOG[0] != 0.0) {
               float clusterCOG[3];
               if (DebugControl::getDebugOn()) {
                  cluster->getCenterOfGravity(clusterCOG);
                  std::cout << "PoCeS Cluster Fiducial COG: "
                            << clusterCOG[0] << ", "
                            << clusterCOG[1] << ", "
                            << clusterCOG[2] << std::endl;
               }
               cluster->getCenterOfGravityForSurface(rotatedVeryInflatedSurface,
                                                     clusterCOG);
               cluster->setCenterOfGravity(clusterCOG);
               const float diff = (postCentralSulcusOffset + clusterCOG[1] - centralSulcusCOG[1]);
               const float bias = std::exp(-((diff*diff) / postCentralSulcusStdDevSquare));
               const float sumBefore = sum;
               sum *= bias;
               
               if (DebugControl::getDebugOn()) {
                  std::cout << "PoCeS Cluster Very Inflated COG: "
                            << clusterCOG[0] << ", "
                            << clusterCOG[1] << ", "
                            << clusterCOG[2] << std::endl;
                  std::cout << "   Number of Nodes: " << numClusterNodes << std::endl;
                  std::cout << "   Sum: " << sumBefore << std::endl;
                  std::cout << "   Bias: " << bias << std::endl;
                  std::cout << "   Sum*Bias" << sum << std::endl;
                  std::cout << std::endl;
               }
            }
         }
      
         vis.addValueIndexPair(i, std::fabs(sum));  // use abs since negative sums
      }  // for (i = 0; i < numClusters...

      //
      // Sort the items
      //
      vis.sort();
      int numSortedItems = vis.getNumberOfItems();
      
      if (DebugControl::getDebugOn()) {
         if (sulcalNamesAndVolumes[j].getSulcusName() == postCentralSulcusName) {
               std::cout << "CeS Cluster Very Inflated COG: "
                         << centralSulcusCOG[0] << ", "
                         << centralSulcusCOG[1] << ", "
                         << centralSulcusCOG[2] << std::endl;
            const int iStop = std::max(numSortedItems - 2, 0);
            for (int i = (numSortedItems - 1); i >= iStop; i--) {
               int indx;
               float value;
               vis.getValueAndIndex(i, indx, value);

               //
               // Set paint for nodes in cluster
               //
               const BrainModelSurfaceMetricClustering::Cluster* cluster = bmsmc->getCluster(indx);
               float clusterCOG[3];
               cluster->getCenterOfGravity(clusterCOG);
               std::cout << "PoCeS Cluster Very Inflated COG: "
                         << clusterCOG[0] << ", "
                         << clusterCOG[1] << ", "
                         << clusterCOG[2] << std::endl;
               std::cout << "   Summed Depth: " 
                         << value << std::endl;
            }
         }
/*
         std::cout << sulcalNamesAndVolumes[j].getSulcusName().toAscii().constData()
                   << " clusters"
                   << std::endl;
         for (int i = (vis.getNumberOfItems() - 1); i >= 0; i--) {
            int indx;
            float value;
            vis.getValueAndIndex(i, indx, value);
            std::cout << "   " 
                      << indx
                      << ": "
                      << value
                      << std::endl;
         }
*/
      } // if (DebugControl...
   
      //
      // Set the paint index
      //
      const int paintIndex = outputPaintFile->addPaintName(sulcalNamesAndVolumes[j].getSulcusName());

      //
      // Need to start at end of sorted data and find the 
      // maximum number of clusters items
      //
      int stopIndex = numSortedItems - sulcalNamesAndVolumes[j].getMaximumClusters();
      
      //
      // Special processing for hippocampal fissure
      //
      if (hippocampalFissureFlag) {
         if (numSortedItems > 1) {
            int indx1, indx2;
            float value1, value2;
            vis.getValueAndIndex(numSortedItems - 1, indx1, value1);
            vis.getValueAndIndex(numSortedItems - 2, indx2, value2);
            float cog1[3], cog2[3];
            bmsmc->getCluster(indx1)->getCenterOfGravity(cog1);
            bmsmc->getCluster(indx2)->getCenterOfGravity(cog2);

            //
            // If 1st cluster is too far posterior, skip it and 
            // use the 2nd cluster
            //
            const float posteriorLimitHF = -34.0;            
            if (cog1[1] < posteriorLimitHF) {
               if (cog2[1] > posteriorLimitHF) {
                  numSortedItems--;
                  stopIndex--;
                  std::cout << "SULCAL ID INFO: " 
                            << "HF cluster Y=" << cog1[1] << " skipped." 
                            << std::endl;
               }
            }
         }
      }
      
      //
      // Special processing for post-central sulcus
      //
      if (sulcalNamesAndVolumes[j].getSulcusName() == postCentralSulcusName) {
         if (numSortedItems > 1) {
            int indx1, indx2;
            float value1, value2;
            vis.getValueAndIndex(numSortedItems - 1, indx1, value1);
            vis.getValueAndIndex(numSortedItems - 2, indx2, value2);
            float cog1[3], cog2[3];
            bmsmc->getCluster(indx1)->getCenterOfGravity(cog1);
            bmsmc->getCluster(indx2)->getCenterOfGravity(cog2);
            const float dist = std::fabs(cog1[1] - cog2[1]);
            
            //
            // If the first and second clusters are within 5mm of each other in Y, use both
            //
            if (dist <= postCentralSulcusSplit) {
               stopIndex = numSortedItems - 2;
               //std::cout << "INFO: using second PoCeS cluster." << std::endl;
            }
         }
      }
      
      //
      // Special processing for superior temporal sulcus
      //
      if (sulcalNamesAndVolumes[j].getSulcusName() == superiorTemporalSulcusName) {
         if (numSortedItems > 1) {
            int indx1, indx2;
            float value1, value2;
            vis.getValueAndIndex(numSortedItems - 1, indx1, value1);
            vis.getValueAndIndex(numSortedItems - 2, indx2, value2);
            const float y1 = bmsmc->getCluster(indx1)->getMaximumY(fiducialSurface);
            const float y2 = bmsmc->getCluster(indx2)->getMaximumY(fiducialSurface);
            
            //
            // If largest cluster is far posterior and the second cluster
            // is in front of it, use both clusters
            //
            if ((y1 < -25.0) && (y2 > y1)) {
               stopIndex = numSortedItems - 2;
            }
         }
      }
      
      //
      // Make sure stop index is valid
      //
      stopIndex = std::max(stopIndex, 0);
      
      //
      // Find the cluster nodes starting at end since we want the largest
      //
      for (int i = (numSortedItems - 1); i >= stopIndex; i--) {
         int indx;
         float value;
         vis.getValueAndIndex(i, indx, value);

         //
         // Set paint for nodes in cluster
         //
         const BrainModelSurfaceMetricClustering::Cluster* cluster = bmsmc->getCluster(indx);
         const int numClusterNodes = cluster->getNumberOfNodesInCluster();
         for (int k = 0; k < numClusterNodes; k++) {
            const int node = cluster->getNodeInCluster(k);
            outputPaintFile->setPaint(node, paintColumn, paintIndex);
         }
         
         //
         // Save central sulcus center of gravity
         //
         if (sulcalNamesAndVolumes[j].getSulcusName() == centralSulcusName) {
            cluster->getCenterOfGravityForSurface(rotatedVeryInflatedSurface,
                                                  centralSulcusCOG);
         }
      }

      //
      // Free memory
      //      
      if (bmsmc != NULL) {
         delete bmsmc;
         bmsmc = NULL;
      }
   }
}
      
/**
 * map the probabilistic functional volumes to the metric file.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::mapProbabilisticFunctionalVolumes() throw (BrainModelAlgorithmException)
{
   //
   // Names of functional probabilistic volumes
   //
   const int numVolumes = static_cast<int>(sulcalNamesAndVolumes.size());
   
   //
   // Create the probabilistic metric file
   //
   if (probabilisticMetricFile != NULL) {
      delete probabilisticMetricFile;
      probabilisticMetricFile = NULL;
   }
   probabilisticMetricFile = new MetricFile;

   //
   // Use enclosing voxel algorithm for mapping
   //   
   BrainModelVolumeToSurfaceMapperAlgorithmParameters mappingParameters;
   mappingParameters.setAlgorithm(
      BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_ENCLOSING_VOXEL);

   //
   // Map the functional probabilistic volumes to the surface
   //
   for (int i = 0; i < numVolumes; i++) {
      // Use Sulcus Name only for area colors
      //const QString name(brainSet->getSubject()
      //                   + " "
      //                   + sulcalNamesAndVolumes[i].getSulcusName());
      const QString name(sulcalNamesAndVolumes[i].getSulcusName());
      BrainModelVolumeToSurfaceMapper mapper(brainSet,
                                           (BrainModelSurface*)fiducialSurface,
                                           sulcalNamesAndVolumes[i].getVolumeName(),
                                           probabilisticMetricFile,
                                           mappingParameters,
                                           -1,
                                           name);
      try {
         mapper.execute();
         
         if (name.endsWith("SUL.HF")) {
            specialProcessingForHippocampalFissure(probabilisticMetricFile,
                             probabilisticMetricFile->getNumberOfColumns() - 1);
         }
         
         addAreaColor(name);
         addVocabulary(name);
      }
      catch (BrainModelAlgorithmException& e) {
         const QString msg = "Mapping "
                           + sulcalNamesAndVolumes[i].getVolumeName()
                           + " failed: "
                           + e.whatQString();
         throw BrainModelAlgorithmException(msg);
      }
   }
   
   //
   // Set metrics to zero for any nodes that are not labeled as SUL in paint
   //
   const int numNodes = probabilisticMetricFile->getNumberOfNodes();
   if (numNodes == inputPaintFile->getNumberOfNodes()) {
      const int numMetricColumns = probabilisticMetricFile->getNumberOfColumns();
      const int paintIndex = inputPaintFile->getPaintIndexFromName("SUL");
      if (paintIndex >= 0) {
         for (int i = 0; i < numNodes; i++) {
            if (inputPaintFile->getPaint(i, paintFileGeographyColumnNumber) != paintIndex) {
               for (int j = 0; j < numMetricColumns; j++) {
                  probabilisticMetricFile->setValue(i, j, 0.0);
               }
            }
         }
      }
   }
   
   //
   // Write the metric file
   //
   if (DebugControl::getDebugOn()) {
      try {
         const QString filename = probabilisticMetricFile->makeDefaultFileName("ProbabilisticSulcal_ALL");
         probabilisticMetricFile->writeFile(filename);
      }
      catch (FileException&) {
      }
   }
}
      
/**
 * rotate the very inflated surface.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::rotateVeryInflatedSurface() throw (BrainModelAlgorithmException)
{
   //
   // Set the rotation
   //
   double rotation = 0.0;
   if (veryInflatedSurface->getStructure() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
      rotation = 35.0;
   }
   else if (veryInflatedSurface->getStructure() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
      rotation = -35.0;
   }
   else if (brainSet->getStructure() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
      rotation = 35.0;
   }
   else if (brainSet->getStructure() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
      rotation = -35.0;
   }
   else {
      throw BrainModelAlgorithmException("Unable to determine hemisphere from very inflated "
                                         "surface header or fiducial coord's file name");
   }
   
   //
   // Create a transformation matrix containing the rotation
   //
   TransformationMatrix tm;
   tm.rotateZ(rotation);
   
   //
   // Apply the rotation to the very inflated surface
   //
   rotatedVeryInflatedSurface = new BrainModelSurface(*veryInflatedSurface);
   rotatedVeryInflatedSurface->applyTransformationMatrix(tm);
   
   if (DebugControl::getDebugOn()) {
      try {
         CoordinateFile* cf = rotatedVeryInflatedSurface->getCoordinateFile();
         cf->writeFile("Sulcal_ID_Very_Inflated_Surface_Rotated.coord");
      }
      catch (FileException&) {
      }
   }
}      

/**
 * special process for the hippocampal fissure.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::specialProcessingForHippocampalFissure(
                                                      MetricFile* metricFile,
                                                      const int metricFileColumnNumber)
{
   //std::cout << "Processing special Hippocampal features." << std::endl;
   //
   // Clear metric for any nodes whose normal vector's Z is not positive
   //
   const int numNodes = metricFile->getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
      const float* normalVector = inflatedSurface->getNormal(i);
      if (normalVector[2] <= 0.0) {
         metricFile->setValue(i, metricFileColumnNumber, 0.0);
      }
   }
}
   
/**
 * load the colors.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::loadColors()
{    
   defaultAreaColorFile = new AreaColorFile;  
   defaultAreaColorFile->addColor("SUL",130,130,130,255);
   defaultAreaColorFile->addColor("SUL.AOS",217,0,205,255);
   defaultAreaColorFile->addColor("SUL.AS",0,85,104,255);
   defaultAreaColorFile->addColor("SUL.CaS",255,0,104,255);
   defaultAreaColorFile->addColor("SUL.CaSd",120,0,0,255);
   defaultAreaColorFile->addColor("SUL.CaSv",166,0,0,255);
   defaultAreaColorFile->addColor("SUL.CeS",162,143,0,255);
   defaultAreaColorFile->addColor("SUL.CiS",8,54,255,255);
   defaultAreaColorFile->addColor("SUL.CiSmr",97,0,104,255);
   defaultAreaColorFile->addColor("SUL.CoS",178,58,58,255);
   defaultAreaColorFile->addColor("SUL.FMS",62,46,0,255);
   defaultAreaColorFile->addColor("SUL.FOS",112,46,12,255);
   defaultAreaColorFile->addColor("SUL.HF",255,0,0,255);
   defaultAreaColorFile->addColor("SUL.IFS",120,73,0,255);
   defaultAreaColorFile->addColor("SUL.ILS",152,31,87,255);
   defaultAreaColorFile->addColor("SUL.IOS",0,120,0,255);
   defaultAreaColorFile->addColor("SUL.IPS",104,255,0,255);
   defaultAreaColorFile->addColor("SUL.IPrCeS",161,88,29,255);
   defaultAreaColorFile->addColor("SUL.IRS",81,0,0,255);
   defaultAreaColorFile->addColor("SUL.ISS",255,0,0,255);
   defaultAreaColorFile->addColor("SUL.ITS",0,41,141,255);
   defaultAreaColorFile->addColor("SUL.LOS",141,47,0,255);
   defaultAreaColorFile->addColor("SUL.LS",131,0,0,255);
   defaultAreaColorFile->addColor("SUL.LiS",166,81,66,255);
   defaultAreaColorFile->addColor("SUL.LuS",236,195,1,255);
   defaultAreaColorFile->addColor("SUL.MFS",81,39,54,255);
   defaultAreaColorFile->addColor("SUL.MPrCeS",112,70,73,255);
   defaultAreaColorFile->addColor("SUL.OTS",85,43,0,255);
   defaultAreaColorFile->addColor("SUL.OlfS",100,0,66,255);
   defaultAreaColorFile->addColor("SUL.OrbS",85,39,23,255);
   defaultAreaColorFile->addColor("SUL.POS",93,0,209,255);
   defaultAreaColorFile->addColor("SUL.PS",46,139,46,255);
   defaultAreaColorFile->addColor("SUL.PaCeS",139,27,81,255);
   defaultAreaColorFile->addColor("SUL.PoCeS",27,112,0,255);
   defaultAreaColorFile->addColor("SUL.PoSubCeS",0,178,132,255);
   defaultAreaColorFile->addColor("SUL.RhS",46,0,39,255);
   defaultAreaColorFile->addColor("SUL.SF",0,0,77,255);
   defaultAreaColorFile->addColor("SUL.SFS",100,62,0,255);
   defaultAreaColorFile->addColor("SUL.SPS",67,0,56,255);
   defaultAreaColorFile->addColor("SUL.SPrCeS",155,110,0,255);
   defaultAreaColorFile->addColor("SUL.SRS",103,35,50,255);
   defaultAreaColorFile->addColor("SUL.SSS",100,0,0,255);
   defaultAreaColorFile->addColor("SUL.STS",46,0,0,255);
   defaultAreaColorFile->addColor("SUL.SubPS",67,0,56,255);
   defaultAreaColorFile->addColor("SUL.SupPS",96,33,126,255);
   defaultAreaColorFile->addColor("SUL.TOS",213,77,0,255);
   defaultAreaColorFile->addColor("SUL.TOrbS",62,31,23,255);
   defaultAreaColorFile->addColor("SUL.intFS",117,94,0,255);
   defaultAreaColorFile->addColor("SUL.pITS",81,39,120,255);
}

/**
 * load vocabulary.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::loadVocabulary()
{
   defaultVocabularyFile = new VocabularyFile;
   
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("GYRAL", "Gyral cortex"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("MEDIAL.WALL", "Non-cortical portions of surface along the medial wall"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL", "Sulcal (buried) cortex"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.AOS", "Anterior Occipital Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.AS", "Angular Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.CaS", "Calcarine Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.CaSd", "Calcarine Sulcus (dorsal)"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.CaSv", "Calcarine Sulcus (ventral)"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.CeS", "Central Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.CiS", "Cingulate Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.CiSmr", "Cingulate Sulcus marginal ramus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.CoS", "Collateral Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.FMS", "Fronto-Marginal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.FOS", "Fronto-Orbital Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.HF", "Hippocampal Fissure"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.IFS", "Inferior Frontal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.ILS", "Intra-Lingual Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.intFS", "intermediate Frontal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.IPrCeS", "Inferior Pre-Central Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.IPS", "Inferior Parietal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.IRS", "Inferior Rostral Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.ISS", "Inferior Sagittal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.ITS", "Inferior Temporal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.LOS", "Lateral Occipital Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.LuS", "Lunate Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.MFS", "Middle Frontal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.MPrCeS", "Medial Pre-Central Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.OlfS", "Olfactory Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.OrbS", "Orbital Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.OTS", "Occipito-Temporal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.PaCeS", "Para-Central Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.pITS", "posterior Inferior Temporal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.PoCeS", "Postcentral Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.POS", "Parieto-Occipital Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.PoSubCeS", "Posterior Sub-Central Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.RhS", "Rhinal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.SF", "Sylvian Fissure"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.SFS", "Superior Frontal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.SPrCeS", "Superior Pre-Central Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.SRS", "Superior Rostral Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.SSS", "Superior Sagittal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.STS", "Superior Temporal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.SubPS", "Sub-Parietal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.SupPS", "Superior Parietal Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.TOrbS", "Transverse Orbital Sulcus"));
   defaultVocabularyFile->addVocabularyEntry(VocabularyFile::VocabularyEntry("SUL.TOS", "Transverse Occipital Sulcus"));
}

/**
 * add vocabulary.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::addVocabulary(const QString& name)
{
   //
   // Create output area color file, if needed
   //
   if (outputVocabularyFile == NULL) {
      outputVocabularyFile = new VocabularyFile;
   }
   
   const VocabularyFile::VocabularyEntry* ve =
      defaultVocabularyFile->getVocabularyEntryByName(name);
   if (ve != NULL) {
      outputVocabularyFile->addVocabularyEntry(*ve);
   }
   else {
      std::cout << "WARNING: No vocabular entry found when identifying sulci probabilistically: "
                << name.toAscii().constData()
                << std::endl;
   }
}
      
/**
 * add an area color.
 */
void 
BrainModelSurfaceSulcalIdentificationProbabilistic::addAreaColor(const QString& colorName)
{
   //
   // Create output area color file, if needed
   //
   if (outputAreaColorFile == NULL) {
      outputAreaColorFile = new AreaColorFile;
   }

   //
   // If first time adding a color
   //   
   if (addingFirstColorFlag) {
      addingFirstColorFlag = false;
      defaultAreaColorFile->addColor("???",170,170,170,255);
      defaultAreaColorFile->addColor("SUL",130,130,130,255);
   }
   
   //
   // See if color is available in default colors
   //
   bool exactMatch = false;
   const int indx = defaultAreaColorFile->getColorIndexByName(colorName, exactMatch);
   if ((indx >= 0) &&
       (exactMatch)) {
      //
      // Add the color to the user's color file
      //
      unsigned char r, g, b, a;
      defaultAreaColorFile->getColorByIndex(indx, r, g, b, a);
      outputAreaColorFile->addColor(colorName, r, g, b, a);
   }
   else {
      std::cout << "INFO: BrainModelSurfaceSulcalIdentificationProbabilistic "
                << "color not found "
                << colorName.toAscii().constData();
   }
}

      

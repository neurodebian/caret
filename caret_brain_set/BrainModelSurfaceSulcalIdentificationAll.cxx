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
#include <iostream>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricClustering.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceSulcalIdentificationAll.h"
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

/**
 * constructor.
 */
BrainModelSurfaceSulcalIdentificationAll::BrainModelSurfaceSulcalIdentificationAll(
                                               BrainSet* bs,
                                               BrainModelSurface* fiducialSurfaceIn,
                                               BrainModelSurface* veryInflatedSurfaceIn,
                                               PaintFile* inputPaintFileIn,
                                               const int paintFileGeographyColumnNumberIn,
                                               SurfaceShapeFile* surfaceShapeFileIn,
                                               const int surfaceShapeFileDepthColumnNumberIn,
                                               const QString& probabilisticDepthVolumeCSVFileNameIn,
                                               const float postCentralSulcusOffsetIn,
                                               const float postCentralSulcusStdDevSquareIn,
                                               const float postCentralSulcusSplitIn)
   : BrainModelAlgorithm(bs)
{
   fiducialSurface                   = fiducialSurfaceIn;
   veryInflatedSurface               = veryInflatedSurfaceIn;
   inputPaintFile                    = inputPaintFileIn;
   paintFileGeographyColumnNumber    = paintFileGeographyColumnNumberIn;
   surfaceShapeFile                  = surfaceShapeFileIn;
   surfaceShapeFileDepthColumnNumber = surfaceShapeFileDepthColumnNumberIn;
   probabilisticDepthVolumeCSVFileName = probabilisticDepthVolumeCSVFileNameIn;
   postCentralSulcusOffset           = postCentralSulcusOffsetIn;
   postCentralSulcusStdDevSquare     = postCentralSulcusStdDevSquareIn;
   postCentralSulcusSplit            = postCentralSulcusSplitIn;
   outputPaintFile = NULL;
   probabilisticMetricFile = NULL;
}

/**
 * destructor.
 */
BrainModelSurfaceSulcalIdentificationAll::~BrainModelSurfaceSulcalIdentificationAll()
{
   if (outputPaintFile != NULL) {
      delete outputPaintFile;
      outputPaintFile = NULL;
   }
   if (probabilisticMetricFile != NULL) {
      delete probabilisticMetricFile;
      probabilisticMetricFile = NULL;
   }
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceSulcalIdentificationAll::execute() throw (BrainModelAlgorithmException)
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
   outputPaintFile->copyColumns(inputPaintFile,
                                paintFileGeographyColumnNumber,
                                -1, // create new column
                                "Initial Sulcus ID");
                                //addCaseNameToName("Initial Sulcus ID"));
                          
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
   outputPaintFile->copyColumns(outputPaintFile,
                                paintInitialSulciIDColumn,
                                -1, // create new column
                                "Sulcus ID");
                                //addCaseNameToName("Sulcus ID"));
   const int paintSulciIDColumn = outputPaintFile->getNumberOfColumns() - 1;
            
   //
   // Dilate the sulcal identification
   //
   dilateSulcalIdentification(paintSulciIDColumn);

   //try {
   //   outputPaintFile->writeFile(outputPaintFile->getFileName());  
   //}
   //catch (FileException&) {
   //}
}

/**
 * multiply probabilistic volumes by depth.
 */
void 
BrainModelSurfaceSulcalIdentificationAll::multiplyProbabilisticFunctionalDataByDepth() throw (BrainModelAlgorithmException)
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
   //if (DebugControl::getDebugOn()) {
      try {
         probabilisticMetricFile->writeFile(probabilisticMetricFile->makeDefaultFileName("ProbabilisticSulcal_Sulcus_X_Depth"));
      }
      catch (FileException&) {
      }
   //}
}

/**
 * read the probabilistic volume file list.
 */
void 
BrainModelSurfaceSulcalIdentificationAll::readProbabilisticVolumeFileList() throw (BrainModelAlgorithmException)
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
      SulcalNameAndVolume snav(st->getElement(i, sulcusNameColumnNumber),
                               st->getElementAsFloat(i, depthThresholdColumnNumber),
                               st->getElement(i, volumeNameColumnNumber),
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
BrainModelSurfaceSulcalIdentificationAll::dilateSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException)
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
   // Dilate the paints until not more dilations can be performed
   //
   bool done = false;
   std::vector<int> paintFullyDilated(numPaintIndices, 0);
   while (done == false) {
      int numDilationsPerformed = 0;
      for (int i = 0; i < numPaintIndices; i++) {
         if (paintFullyDilated[i] == 0) {
            const int dilateCount = outputPaintFile->dilatePaintID(
                                                     fiducialSurface->getTopologyFile(),
                                                     paintColumn,
                                                     1,
                                                     paintIndices[i],
                                                     sulIndex);
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
BrainModelSurfaceSulcalIdentificationAll::addCaseNameToName(const QString& name) const
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
BrainModelSurfaceSulcalIdentificationAll::createInitialSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException)
{
   const QString centralSulcusName("SUL.CeS");
   float centralSulcusCOG[3] = { 0.0, 0.0, 0.0 };
   const QString postCentralSulcusName("SUL.PoCeS");
   
   const int numColumns = probabilisticMetricFile->getNumberOfColumns();
   for (int j = 0; j < numColumns; j++) {
      //
      // Find the clusters with specified depth range
      //
      BrainModelSurfaceMetricClustering bmsmc(brainSet,
                                              fiducialSurface,
                                              probabilisticMetricFile,
                                              BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_NUMBER_OF_NODES,
                                              j,           // column number
                                              -1,          // output column
                                              "Clusters",  // output column  name
                                              1,           // min number nodes
                                              0.001,       // min surface area
                                              sulcalNamesAndVolumes[j].getDepthThreshold(),       // neg min
                                              -100000000.0,// neg max
                                              5.0,         // pos min
                                              4.0,         // pos max
                                              false);      // only clusters with min nodes or more
      try {
         bmsmc.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         throw BrainModelAlgorithmException("Finding clusters: " + e.whatQString());
      }
      
      const int numClusters = bmsmc.getNumberOfClusters();

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
         BrainModelSurfaceMetricClustering::Cluster* cluster = bmsmc.getCluster(i);
         const int numClusterNodes = cluster->getNumberOfNodesInCluster();
         for (int k = 0; k < numClusterNodes; k++) {
            const int node = cluster->getNodeInCluster(k);
            sum += probabilisticMetricFile->getValue(node, j);
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
               cluster->getCenterOfGravityForSurface(veryInflatedSurface,
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
      const int numSortedItems = vis.getNumberOfItems();
      
//      if (DebugControl::getDebugOn()) {
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
               const BrainModelSurfaceMetricClustering::Cluster* cluster = bmsmc.getCluster(indx);
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
//      } // if (DebugControl...
   
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
      // Special processing for post-central sulcus
      //
      if (sulcalNamesAndVolumes[j].getSulcusName() == postCentralSulcusName) {
         if (numSortedItems > 1) {
            int indx1, indx2;
            float value1, value2;
            vis.getValueAndIndex(numSortedItems - 1, indx1, value1);
            vis.getValueAndIndex(numSortedItems - 2, indx2, value2);
            float cog1[3], cog2[3];
            bmsmc.getCluster(indx1)->getCenterOfGravity(cog1);
            bmsmc.getCluster(indx2)->getCenterOfGravity(cog2);
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
         const BrainModelSurfaceMetricClustering::Cluster* cluster = bmsmc.getCluster(indx);
         const int numClusterNodes = cluster->getNumberOfNodesInCluster();
         for (int k = 0; k < numClusterNodes; k++) {
            const int node = cluster->getNodeInCluster(k);
            outputPaintFile->setPaint(node, paintColumn, paintIndex);
         }
         
         //
         // Save central sulcus center of gravity
         //
         if (sulcalNamesAndVolumes[j].getSulcusName() == centralSulcusName) {
            cluster->getCenterOfGravityForSurface(veryInflatedSurface,
                                                  centralSulcusCOG);
         }
      }
   }
}
      
/**
 * map the probabilistic functional volumes to the metric file.
 */
void 
BrainModelSurfaceSulcalIdentificationAll::mapProbabilisticFunctionalVolumes() throw (BrainModelAlgorithmException)
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
      const QString name(brainSet->getSubject()
                         + " "
                         + sulcalNamesAndVolumes[i].getSulcusName());
      BrainModelVolumeToSurfaceMapper mapper(brainSet,
                                           fiducialSurface,
                                           sulcalNamesAndVolumes[i].getVolumeName(),
                                           probabilisticMetricFile,
                                           mappingParameters,
                                           -1,
                                           name);
      try {
         mapper.execute();
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
   //if (DebugControl::getDebugOn()) {
      try {
         probabilisticMetricFile->writeFile(probabilisticMetricFile->makeDefaultFileName("ProbabilisticSulcal_ALL"));
      }
      catch (FileException&) {
      }
   //}
}
      
/**
 * rotate the very inflated surface.
 */
void 
BrainModelSurfaceSulcalIdentificationAll::rotateVeryInflatedSurface() throw (BrainModelAlgorithmException)
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
   if (brainSet->getStructure() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
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
   veryInflatedSurface->applyTransformationMatrix(tm);
   
   if (DebugControl::getDebugOn()) {
      try {
         CoordinateFile* cf = veryInflatedSurface->getCoordinateFile();
         cf->writeFile("Sulcal_ID_Very_Inflated_Surface_Rotated.coord");
      }
      catch (FileException&) {
      }
   }
}      


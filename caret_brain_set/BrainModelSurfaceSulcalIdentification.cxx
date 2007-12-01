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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricClustering.h"
#include "BrainModelSurfaceSulcalIdentification.h"
#include "BrainModelVolumeToSurfaceMapperAlgorithmParameters.h"
#include "BrainModelVolumeToSurfaceMapper.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "SurfaceShapeFile.h"

/**
 * constructor.
 */
BrainModelSurfaceSulcalIdentification::BrainModelSurfaceSulcalIdentification(
                                               BrainSet* bs,
                                               BrainModelSurface* fiducialSurfaceIn,
                                               PaintFile* inputPaintFileIn,
                                               const int paintFileGeographyColumnNumberIn,
                                               SurfaceShapeFile* surfaceShapeFileIn,
                                               const int surfaceShapeFileDepthColumnNumberIn,
                                               const float clusterNodeCountRatioIn)
   : BrainModelAlgorithm(bs)
{
   fiducialSurface                   = fiducialSurfaceIn;
   inputPaintFile                    = inputPaintFileIn;
   paintFileGeographyColumnNumber    = paintFileGeographyColumnNumberIn;
   surfaceShapeFile                  = surfaceShapeFileIn;
   surfaceShapeFileDepthColumnNumber = surfaceShapeFileDepthColumnNumberIn;
   clusterNodeCountRatio             = clusterNodeCountRatioIn;
   
   outputPaintFile = NULL;
   probabilisticMetricFile = NULL;
}

/**
 * destructor.
 */
BrainModelSurfaceSulcalIdentification::~BrainModelSurfaceSulcalIdentification()
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
BrainModelSurfaceSulcalIdentification::execute() throw (BrainModelAlgorithmException)
{
   //
   // Check inputs
   //
   if (fiducialSurface == NULL) {
      throw BrainModelAlgorithmException("Fiducial surface is invalid.");
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
   if ((clusterNodeCountRatio < 0.0) ||
       (clusterNodeCountRatio > 1.0)) {
      throw BrainModelAlgorithmException("Cluster node count ratio must be 0.0 to 1.0.");
   }
   
   //
   // Initialize names of sulcus names and probabilistic metric volumes
   //
   sulcalNamesAndVolumes.push_back(SulcalNameAndVolume("SUL.CeS",
                                                       "PALS_B12.RIGHT.PROBABILSTIC-CeS.align+orig.HEAD"));
   sulcalNamesAndVolumes.push_back(SulcalNameAndVolume("SUL.PoCeS",
                                                       "PALS_B12.RIGHT.PROBABILSTIC-PoCeS.align+orig.HEAD"));
   sulcalNamesAndVolumes.push_back(SulcalNameAndVolume("SUL.PrCeS",
                                                       "PALS_B12.RIGHT.PROBABILSTIC-PrCeS.align+orig.HEAD"));
   sulcalNamesAndVolumes.push_back(SulcalNameAndVolume("SUL.SF",
                                                       "PALS_B12.RIGHT.PROBABILSTIC-SF.align+orig.HEAD"));

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
   // Create the new initial sulcus ID paint column
   //
   outputPaintFile->copyColumns(inputPaintFile,
                                paintFileGeographyColumnNumber,
                                -1, // create new column
                                addCaseNameToName("Initial Sulcus ID"));
                          
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
                                addCaseNameToName("Sulcus ID"));
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
 * dilate the sulcal identification.
 */
void 
BrainModelSurfaceSulcalIdentification::dilateSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException)
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
   while (done == false) {
      int numDilationsPerformed = 0;
      for (int i = 0; i < numPaintIndices; i++) {
         numDilationsPerformed = outputPaintFile->dilatePaintID(
                                                 fiducialSurface->getTopologyFile(),
                                                 paintColumn,
                                                 1,
                                                 paintIndices[i],
                                                 sulIndex);
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
BrainModelSurfaceSulcalIdentification::addCaseNameToName(const QString& name) const
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
BrainModelSurfaceSulcalIdentification::createInitialSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException)
{
   //
   // Find the clusters with specified depth range
   //
   BrainModelSurfaceMetricClustering bmsmc(brainSet,
                                           fiducialSurface,
                                           surfaceShapeFile,
                                           BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_NUMBER_OF_NODES,
                                           surfaceShapeFileDepthColumnNumber,
                                           -1,          // output column
                                           "Clusters",  // output column  name
                                           1,           // min number nodes
                                           0.001,       // min surface area
                                           -10.0,       // neg min
                                           -100000000.0,// neg max
                                           5.0,         // pos min
                                           4.0,         // pos max
                                           false);      // only clusters meeting number of nodes
   try {
      bmsmc.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      throw BrainModelAlgorithmException("Finding clusters: " + e.whatQString());
   }
   
   //if (DebugControl::getDebugOn()) {
      try {
         surfaceShapeFile->writeFile(surfaceShapeFile->makeDefaultFileName("TEST_Clusters"));  
      }
      catch (FileException&) {
      }
   //}
   
   //
   // Use to count each metric type in each cluster
   //
   const int numberOfIDs = probabilisticMetricFile->getNumberOfColumns();
   double* idCounts = new double [numberOfIDs];
   
   //
   // Examine each cluster
   //
   const int numClusters = bmsmc.getNumberOfClusters();
   for (int i = 0; i < numClusters; i++) {
      //
      // Clear count for each ID
      //
      for (int j = 0; j < numberOfIDs; j++) {
         idCounts[j] = 0.0;
      }
      
      //
      // Look at the nodes in each cluster
      //
      const BrainModelSurfaceMetricClustering::Cluster* cluster = bmsmc.getCluster(i);
      const int numClusterNodes = cluster->getNumberOfNodesInCluster();
      for (int k = 0; k < numClusterNodes; k++) {
         //
         // Get node in cluster
         //
         const int node = cluster->getNodeInCluster(k);
         
         //
         // Add into counts
         //
         for (int j = 0; j < numberOfIDs; j++) {
            //idCounts[j] += probabilisticMetricFile->getValue(node, j);
            if (probabilisticMetricFile->getValue(node, j) > 0.0) {
               idCounts[j] += 1.0;
            }
         }
      }
      
      //
      // Find biggest count for cluster
      //
      double biggestCount = 0.0;
      int biggestIndex = -1;
      for (int j = 0; j < numberOfIDs; j++) {
         if (idCounts[j] > biggestCount) {
            biggestCount = idCounts[j];
            biggestIndex = j;
         }
      }
      
      //
      // minimum number of nodes that must be identified in cluster
      //
      const double clusterMinimumCount = numClusterNodes
                                       * clusterNodeCountRatio;
                                       
      //
      // Assign all nodes in cluster the ID of the largest count
      //
      if (biggestIndex >= 0) {
         //
         // Does it pass cluster minimum
         //
         if (biggestCount >= clusterMinimumCount) {
            const int paintIndex = outputPaintFile->addPaintName(
                             probabilisticMetricFile->getColumnName(biggestIndex));
            for (int k = 0; k < numClusterNodes; k++) {
               const int node = cluster->getNodeInCluster(k);
               outputPaintFile->setPaint(node, paintColumn, paintIndex);
            }
         }
      }
   }
   
   //
   // Free memory
   //
   delete[] idCounts;
}
      
/**
 * map the probabilistic functional volumes to the metric file.
 */
void 
BrainModelSurfaceSulcalIdentification::mapProbabilisticFunctionalVolumes() throw (BrainModelAlgorithmException)
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
      BrainModelVolumeToSurfaceMapper mapper(brainSet,
                                           fiducialSurface,
                                           sulcalNamesAndVolumes[i].getVolumeName(),
                                           probabilisticMetricFile,
                                           mappingParameters,
                                           -1,
                                           sulcalNamesAndVolumes[i].getSulcusName());
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
   // Write the metric file
   //
   //if (DebugControl::getDebugOn()) {
      try {
         probabilisticMetricFile->writeFile(probabilisticMetricFile->makeDefaultFileName("TEST_ProbabilisticSulcalID"));
      }
      catch (FileException&) {
      }
   //}
}
      


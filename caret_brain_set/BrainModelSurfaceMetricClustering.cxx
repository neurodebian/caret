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
#include <iostream>

#include <QDateTime>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceConnectedSearchMetric.h"
#include "BrainModelSurfaceMetricClustering.h"
#include "DebugControl.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "TopologyFile.h"

/**
 * Constructor.
 */
BrainModelSurfaceMetricClustering::BrainModelSurfaceMetricClustering(BrainSet* bs,
                                    BrainModelSurface* bmsIn,
                                    MetricFile* metricFileIn,
                                    const CLUSTER_ALGORITHM algorithmIn,
                                    const int inputColumnIn, 
                                    const int outputColumnIn,
                                    const QString& outputColumnNameIn,
                                    const int minimumNumberOfNodesIn,
                                    const float minimumSurfaceAreaIn,
                                    const float clusterNegativeMinimumThresholdIn,
                                    const float clusterNegativeMaximumThresholdIn,
                                    const float clusterPositiveMinimumThresholdIn,
                                    const float clusterPositiveMaximumThresholdIn)
   : BrainModelAlgorithm(bs)
{
   bms = bmsIn;
   metricFile = metricFileIn;
   algorithm  = algorithmIn;
   inputColumn = inputColumnIn;
   outputColumn = outputColumnIn;
   outputColumnName = outputColumnNameIn;
   minimumNumberOfNodes = minimumNumberOfNodesIn;
   minimumSurfaceArea = minimumSurfaceAreaIn;
   clusterNegativeMinimumThreshold = clusterNegativeMinimumThresholdIn;
   clusterNegativeMaximumThreshold = clusterNegativeMaximumThresholdIn;
   clusterPositiveMinimumThreshold = clusterPositiveMinimumThresholdIn;
   clusterPositiveMaximumThreshold = clusterPositiveMaximumThresholdIn;
}

/**
 * Destructor.
 */
BrainModelSurfaceMetricClustering::~BrainModelSurfaceMetricClustering()
{
   clusters.clear();
   nodeAreas.clear();
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceMetricClustering::execute() throw (BrainModelAlgorithmException)
{
   //
   // Check for valid input column
   //
   if ((metricFile->getNumberOfColumns() <= 0) || 
       (metricFile->getNumberOfNodes() <= 0)) {
      throw BrainModelAlgorithmException("Metric file has no data");
   }
   
   if ((inputColumn < 0) || (inputColumn >= metricFile->getNumberOfColumns())) {
      throw BrainModelAlgorithmException("Invalid input column number");
   }
   
   //
   // Create a new column if needed.
   //
   if ((outputColumn < 0) || (outputColumn >= metricFile->getNumberOfColumns())){
      metricFile->addColumns(1);
      outputColumn = metricFile->getNumberOfColumns() - 1;
   }
   metricFile->setColumnName(outputColumn, outputColumnName);
   
   //
   // Copy the input column to the output column
   //
   if (inputColumn != outputColumn) {
      std::vector<float> values;
      metricFile->getColumnForAllNodes(inputColumn, values);
      metricFile->setColumnForAllNodes(outputColumn, values);
   }
   
   //
   // Node within threshold flags
   //
   const int numNodes = bms->getNumberOfNodes();
   nodeWithinThresholds.resize(numNodes);
   
   //
   // Find nodes that are within thresholds
   //
   for (int i = 0; i < numNodes; i++) {
      nodeWithinThresholds[i] = false;
      float v = metricFile->getValue(i, outputColumn);
      if ((v >= clusterPositiveMinimumThreshold) && 
          (v <= clusterPositiveMaximumThreshold)) {
         nodeWithinThresholds[i] = true;
      }
      if ((v <= clusterNegativeMinimumThreshold) && 
          (v >= clusterNegativeMaximumThreshold)) {
         nodeWithinThresholds[i] = true;
      }
   }
   
   
   switch (algorithm) {
      case CLUSTER_ALGORITHM_NONE:
         throw BrainModelAlgorithmException("Invalid clustering algorithm = NONE");
         break;
      case CLUSTER_ALGORITHM_ANY_SIZE:
         break;
      case CLUSTER_ALGORITHM_MINIMUM_NUMBER_OF_NODES:
      case CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA:
         //
         // Find the clusters
         // Note find clusters will make sure that a cluster contains just positive nodes
         // or just negative nodes.
         //
         findClusters();
         break;
   }
   
   //
   // Nodes that are to remain after clustering
   //
   std::vector<bool> validClusterNodes(numNodes, false);
   
   const int numClusters = static_cast<int>(clusters.size());
   
   //
   // Determine which clusters are good
   //
   switch (algorithm) {
      case CLUSTER_ALGORITHM_NONE:
         break;
      case CLUSTER_ALGORITHM_ANY_SIZE:
         //
         // Any nodes within the thresholds are in the cluster
         //
         for (int i = 0; i < numNodes; i++) {
            validClusterNodes[i] = nodeWithinThresholds[i];
         }
         break;
      case CLUSTER_ALGORITHM_MINIMUM_NUMBER_OF_NODES:
         //
         // Keep nodes that are in clusters with sufficient number of nodes
         //
         for (int i = 0; i < numClusters; i++) {
            const Cluster& c = clusters[i];
            const int num = c.getNumberOfNodesInCluster();
            if (num >= minimumNumberOfNodes) {
               for (int j = 0; j < num; j++) {
                  validClusterNodes[c.nodeIndices[j]] = true;
               }
            }
         }
         break;
      case CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA:
         {
            const TopologyFile* tf = bms->getTopologyFile();
            if (tf != NULL) {
               const int numTiles = tf->getNumberOfTiles();
               if (numTiles > 0) {
                  //
                  // area used by each node
                  //
                  nodeAreas.resize(numNodes);
                  for (int i = 0; i < numNodes; i++) {
                     nodeAreas[i] = 0.0;
                  }
                  
                  //
                  // Compute 1/3 area of each tile
                  //
                  const float* coords = bms->getCoordinateFile()->getCoordinate(0);
                  for (int i = 0; i < numTiles; i++) {
                    int v1, v2, v3;
                    tf->getTile(i, v1, v2, v3);
                    const float tileArea = MathUtilities::triangleArea(
                                                   (float*)&coords[v1*3],
                                                   (float*)&coords[v2*3],
                                                   (float*)&coords[v3*3]) * 0.33;
                        
                     //
                     // Assign 1/3 of tile area to each node that is part of the tile
                     //
                     nodeAreas[v1] += tileArea;
                     nodeAreas[v2] += tileArea;
                     nodeAreas[v3] += tileArea;
                  }
                  
                  
                  //
                  // Check each cluster
                  //
                  for (int i = 0; i < numClusters; i++) {
                     //
                     // Center of gravity of cluster
                     //
                     float cog[3] = { 0.0, 0.0, 0.0 };
                     
                     //
                     // Area of cluster
                     //
                     Cluster& c = clusters[i];
                     float clusterArea = 0.0;
                     const int num = c.getNumberOfNodesInCluster();
                     for (int j = 0; j < num; j++) {
                        const int nodeNum = c.nodeIndices[j];
                        clusterArea += nodeAreas[nodeNum];
                        
                        cog[0] += coords[nodeNum*3];
                        cog[1] += coords[nodeNum*3+1];
                        cog[2] += coords[nodeNum*3+2];
                     }
                     
                     c.setArea(clusterArea);
                     
                     if (num > 0) {
                        cog[0] /= static_cast<float>(num);
                        cog[1] /= static_cast<float>(num);
                        cog[2] /= static_cast<float>(num);
                     }
                     c.setCenterOfGravity(cog);
                     
                     //
                     // Is this cluster big enough
                     //
                     if (clusterArea >= minimumSurfaceArea) {
                        for (int j = 0; j < num; j++) {
                           validClusterNodes[c.nodeIndices[j]] = true;
                        }
                     }
                  }
               }
            }
         }
         break;
   }
   
   //
   // Any nodes within the thresholds are in the cluster
   //
   for (int i = 0; i < numNodes; i++) {
      if (validClusterNodes[i] == false) {
         metricFile->setValue(i, outputColumn, 0.0);
      }
   }
}

/**
 * Find the clusters
 */
void
BrainModelSurfaceMetricClustering::findClusters() throw (BrainModelAlgorithmException)
{
   clusters.clear();
   
   QTime timer;
   timer.start();
   
   //
   // Search positive values
   //
   const int numNodes = metricFile->getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
      if (nodeWithinThresholds[i]) {
         float minVal = 0.0;
         float maxVal = 0.0;
         bool doIt = false;
         if ((metricFile->getValue(i, outputColumn) >= clusterPositiveMinimumThreshold) &&
             (metricFile->getValue(i, outputColumn) <= clusterPositiveMaximumThreshold)) { 
            minVal = clusterPositiveMinimumThreshold;
            maxVal = clusterPositiveMaximumThreshold;
            doIt = true;
         }
         else if ((metricFile->getValue(i, outputColumn) >= clusterNegativeMaximumThreshold) &&
                   (metricFile->getValue(i, outputColumn) <= clusterNegativeMinimumThreshold)) { 
            minVal = clusterNegativeMaximumThreshold;
            maxVal = clusterNegativeMinimumThreshold;
            doIt = true;
         }
         
         if (doIt) {
            //
            // allow other events to process
            //
            allowEventsToProcess();

            BrainModelSurfaceConnectedSearchMetric bmscsm(brainSet,
                                                      bms,
                                                      i,
                                                      metricFile,
                                                      outputColumn,
                                                      minVal,
                                                      maxVal,
                                                      &nodeWithinThresholds);
            try {
               bmscsm.execute();
            }
            catch (BrainModelAlgorithmException& e) {
               throw e;
            }
            
            //
            // Create a new cluster
            //
            Cluster c(minVal, maxVal);
            
            //
            // See which nodes should be added to the cluster
            //
            for (int j = i; j < numNodes; j++) {
               //
               // Is node part of the cluster ?
               //
               if (bmscsm.getNodeConnected(j)) {
                  //
                  // add to the cluster
                  //
                  c.addNodeToCluster(j);
                  
                  //
                  // do not need to look at this node again
                  //
                  nodeWithinThresholds[j] = false;
               }
            }
            
            //
            // If the cluster has nodes, add it to the clusters
            //
            if (c.getNumberOfNodesInCluster() > 0) {
               clusters.push_back(c);
               if (DebugControl::getDebugOn()) {
                  std::cout << "Cluster starting at node " << i
                            << " contains " << c.getNumberOfNodesInCluster()
                            << " nodes." << std::endl;
               }
            }
         }
         
         //
         // This node no longer needs to be examined
         //
         nodeWithinThresholds[i] = false;
      }
   }
   if (DebugControl::getDebugOn()) {
      std::cout << "Time to find clusters: " 
                << (static_cast<float>(timer.elapsed()) / 1000.0) << std::endl;
   }
}

/**
 * get the area for a node (only valid if cluster by area).
 */
float 
BrainModelSurfaceMetricClustering::getNodeArea(const int nodeNum)
{
   if (nodeNum < static_cast<int>(nodeAreas.size())) {
      return nodeAreas[nodeNum];
   }
   return 0.0;
}      

//*****************************************************************************************
//
// Cluster methods
//

/**
 * Constructor
 */
BrainModelSurfaceMetricClustering::Cluster::Cluster(const float threshMinIn, const float threshMaxIn) 
{ 
   clusterArea = 0.0; 
   centerOfGravity[0] = 0.0;
   centerOfGravity[1] = 0.0;
   centerOfGravity[2] = 0.0;
   threshMin = threshMinIn;
   threshMax = threshMaxIn;
}

/** 
 * get thresholds.
 */
void 
BrainModelSurfaceMetricClustering::Cluster::getThresholds(float& threshMinOut, 
                                                          float& threshMaxOut) const
{
   threshMinOut = threshMin;
   threshMaxOut = threshMax;
}
            
/**
 * get the center of gravity.
 */
void 
BrainModelSurfaceMetricClustering::Cluster::getCenterOfGravity(float cog[3]) const
{
   cog[0] = centerOfGravity[0];
   cog[1] = centerOfGravity[1];
   cog[2] = centerOfGravity[2];
}

/**
 * set the center of gravity.
 */
void 
BrainModelSurfaceMetricClustering::Cluster::setCenterOfGravity(const float cog[3])
{
   centerOfGravity[0] = cog[0];
   centerOfGravity[1] = cog[1];
   centerOfGravity[2] = cog[2];
}            


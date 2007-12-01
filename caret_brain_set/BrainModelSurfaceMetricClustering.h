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

#ifndef __BRAIN_MODEL_SURFACE_METRIC_CLUSTERING_H__
#define __BRAIN_MODEL_SURFACE_METRIC_CLUSTERING_H__

#include <QString>
#include <vector>

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class MetricFile;

/// class for performing clustering of a metric file column
class BrainModelSurfaceMetricClustering : public BrainModelAlgorithm {
   
   public:
      /// cluster of nodes
      class Cluster {
         public:
            /// Constructor
            Cluster(const float threshMinIn, const float threshMaxIn);
            
            /// Destructor
            ~Cluster() {}
            
            /// get number of nodes in cluster
            int getNumberOfNodesInCluster() const { return nodeIndices.size(); }
            
            /// get a node that is in the cluster
            int getNodeInCluster(const int indx) const { return nodeIndices[indx]; }
            
            /// get all nodes in the cluster
            std::vector<int> getNodesInCluster() const { return nodeIndices; }
            
            /// add a node to the cluster
            void addNodeToCluster(const int index) { nodeIndices.push_back(index); }

            /// set the area of the cluster
            void setArea(const float area) { clusterArea = area; }
            
            /// get the cluster area
            float getArea() const { return clusterArea; }
            
            /// get the center of gravity
            void getCenterOfGravity(float cog[3]) const;
            
            /// set the center of gravity
            void setCenterOfGravity(const float cog[3]);
            
            /// get thresholds
            void getThresholds(float& threshMinOut, float& threshMaxOut) const;
            
            /// get the center of gravity using the surface (does not overwrite cluster's cog)
            void getCenterOfGravityForSurface(const BrainModelSurface* bms,
                                              float cog[3]) const;
                                              
         protected:
            /// nodes in the cluster
            std::vector<int> nodeIndices;
            
            /// area of the cluster
            float clusterArea;
            
            /// cluster center of gravity
            float centerOfGravity[3];
         
            /// cluster threshold minimum
            float threshMin;
            
            /// cluster threshold maximum
            float threshMax;
            
         friend class BrainModelSurfaceMetricClustering;
      };
      
      /// clustering algorithm
      enum CLUSTER_ALGORITHM {
         CLUSTER_ALGORITHM_NONE,
         CLUSTER_ALGORITHM_ANY_SIZE,
         CLUSTER_ALGORITHM_MINIMUM_NUMBER_OF_NODES,
         CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA,
      };

      /// Constructor
      BrainModelSurfaceMetricClustering(BrainSet* bs,
                                        BrainModelSurface* bmsIn,
                                        MetricFile* metricFileIn,
                                        const CLUSTER_ALGORITHM algorithmIn,
                                        const int columnIn, 
                                        const int outputColumnIn,
                                        const QString& outputColumnNameIn,
                                        const int minimumNumberOfNodesIn,
                                        const float minimumSurfaceAreaIn,
                                        const float clusterNegativeMinimumThresholdIn,
                                        const float clusterNegativeMaximumThresholdIn,
                                        const float clusterPositiveMinimumThresholdIn,
                                        const float clusterPositiveMaximumThresholdIn,
                                        const bool outputAllClustersFlagIn);

      /// Destructor
      ~BrainModelSurfaceMetricClustering();
      
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// get the number of clusters
      int getNumberOfClusters() const { return clusters.size(); }
      
      /// get a cluster
      Cluster* getCluster(const int indx) { return &clusters[indx]; }
      
      /// get a cluster (const method)
      const Cluster* getCluster(const int indx) const { return &clusters[indx]; }
      
      /// get clusters indices sorted by number of nodes in cluster
      void getClusterIndicesSortedByNumberOfNodesInCluster(std::vector<int>& indices) const;
      
   protected:
      /// find the clusters
      void findClusters() throw (BrainModelAlgorithmException);
      
      /// set clusters center of gravity and area
      void setClustersCenterOfGravityAndArea() throw (BrainModelAlgorithmException);
      
      /// surface for clustering
      BrainModelSurface* bms;
      
      /// metric file being clustered
      MetricFile* metricFile;
      
      /// clustering algorithm
      CLUSTER_ALGORITHM algorithm;
      
      /// input column
      int inputColumn;
      
      /// output column
      int outputColumn;
      
      /// output column name
      QString outputColumnName;
      
      /// minimum number of nodes for number of nodes algorithm
      int minimumNumberOfNodes;
      
      /// minimum surface area for minimum surface area algorithm
      float minimumSurfaceArea;
      
      /// cluster negative minimum threshold
      float clusterNegativeMinimumThreshold;
      
      /// cluster negative maximum threshold
      float clusterNegativeMaximumThreshold;
      
      /// cluster positive minimum threshold
      float clusterPositiveMinimumThreshold;
      
      /// cluster positive maximum threshold
      float clusterPositiveMaximumThreshold;
      
      /// marks nodes whose metric is within the threshold values
      std::vector<int> nodeWithinThresholds;
      
      /// the clusters
      std::vector<Cluster> clusters;
      
      /// output all clusters including those that do not meet cluster area/number of nodes
      bool outputAllClustersFlag;
};

#endif // __BRAIN_MODEL_SURFACE_METRIC_CLUSTERING_H__



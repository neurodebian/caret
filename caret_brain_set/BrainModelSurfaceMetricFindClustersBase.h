#ifndef __BRAIN_MODEL_SURFACE_SHAPE_FIND_CLUSTERS_BASE_H__
#define __BRAIN_MODEL_SURFACE_SHAPE_FIND_CLUSTERS_BASE_H__

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

#include <QString>
#include <vector>

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class BrainModelSurfaceMetricClustering;
class MetricFile;
class QTextStream;

/// base class for finding clusters in surface shape files
class BrainModelSurfaceMetricFindClustersBase : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceMetricFindClustersBase(BrainSet* bs,
                                         const QString& fiducialCoordFileNameIn,
                                         const QString& openTopoFileNameIn,
                                         const QString& distortionShapeFileNameIn,
                                         const QString& statisticalMapFileNameIn,
                                         const QString& shuffledStatisticalMapFileNameIn,
                                         const QString& clustersPaintFileNameIn,
                                         const QString& clustersMetricFileNameIn,
                                         const QString& reportFileNameIn,
                                         const int distortionShapeFileColumnIn,
                                         const float negativeThreshIn,
                                         const float positiveThreshIn,
                                         const float pValueIn,
                                         const int tVarianceSmoothingIterationsIn,
                                         const float tVarianceSmoothingStrengthIn,
                                         const bool doStatisticalMapDOFIn,
                                         const bool doStatisticalMapPValueIn,
                                         const int numberOfThreadsIn);
      
      // destructor
      ~BrainModelSurfaceMetricFindClustersBase();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// class for cluster thresholding
      class Cluster {
         public:
            QString name;
            int column;
            int numberOfNodes;
            std::vector<int> nodes;
            float area;
            float areaCorrected;
            float cogX;
            float cogY;
            float cogZ;
            float pValue;
            float threshMin;
            float threshMax;
            
            /// constructor
            Cluster() {
               name = "";
               column = -1;
               numberOfNodes = 0;
               area = 0.0;
               areaCorrected = 0.0;
               cogX = 0.0;
               cogY = 0.0;
               cogZ = 0.0;
               pValue = -1.0;
               threshMin = 0.0;
               threshMax = 0.0;
               nodes.clear();
            }
            
            /// comparison operator using area corrected
            bool operator<(const Cluster& c) const {
               return (areaCorrected < c.areaCorrected);
            }
            
            /// set the name of a cluster
            void setName() {
               name = "";
               if (threshMax < 0.0) {
                  name += "minus_";
               }
               else {
                  name += "plus_";
               }
               name += "cluster_area_";
               name += QString::number(areaCorrected, 'f', 1);
               name += "_nodes_";
               name += QString::number(numberOfNodes);
            }
      };
      
      /// must be implemented by subclasses
      virtual void executeClusterSearch() throw (BrainModelAlgorithmException) = 0;
      
      // free memory
      virtual void cleanUp();
      
      // find clusters in shape file
      void findClusters(MetricFile* mf, 
                        std::vector<Cluster>& clustersOut,
                        const QString& progressMessage,
                        const int limitToColumn,
                        const bool useLargestClusterPerColumnFlag) throw (BrainModelAlgorithmException);
      
      // find clusters 
      void findClustersSingleThread(MetricFile* mf, 
                        std::vector<Cluster>& clustersOut,
                        const QString& progressMessage,
                        const int limitToColumn,
                        const bool useLargestClusterPerColumnFlag) throw (BrainModelAlgorithmException);
      
      // find clusters in shape file using multiple threads
      void findClustersMultiThread(MetricFile* mf, 
                                std::vector<Cluster>& clustersOut,
                                const QString& progressMessage,
                                const int limitToColumn,
                                const bool useLargestClusterPerColumnFlag,
                                const int numberOfThreads) throw (BrainModelAlgorithmException);
      
      /// Get the clusters after the cluster finding algorithm has finished
      void saveClusters(BrainModelSurfaceMetricClustering* bmsmc,
                        std::vector<Cluster>& clustersOut,
                        const int columnNumber,
                        const bool useLargestClusterPerColumnFlag);
      
      // print the clusters
      void printClusters(QTextStream& stream, const std::vector<Cluster>& clusters,
                         const float sigArea = -1.0);
      
      /// create the clusters paint file
      void createClustersPaintFile(const std::vector<Cluster>& clusters,
                                   const float sigArea,
                                   const int numNodes);
       
      /// create the clusters metric file
      void createClustersMetricFile(const std::vector<Cluster>& clusters,
                                    const int statisticalMapColumn,
                                    const int numNodes);
      
      /// set names of all clusters
      void setNamesForClusters(std::vector<Cluster>& clusters);
      
      /// create the clusters report file
      void createMetricShapeClustersReportFile(const std::vector<Cluster>& clusters,
                                               const QString& metricShapeFileName);
                                    
      /// name of fiducial coord file
      QString fiducialCoordFileName;
   
      /// name of open topo file
      QString openTopoFileName;
   
      /// name of area correction shape file
      QString areaCorrectionShapeFileName;
   
      /// name of Statistical Map file
      QString statisticalMapFileName;
   
      /// name of shuffled Statistical map file
      QString shuffledStatisticalMapFileName;
      
      /// name of clusters paint file
      QString clustersPaintFileName;
      
      /// name of clusters metric file name
      QString clustersMetricFileName;
      
      /// name of report file 
      QString reportFileName;
      
      ///  area correction shape file column
      int areaCorrectionShapeFileColumn;
      
      /// negative threshold
      float negativeThresh;
      
      /// positive threshold
      float positiveThresh;
      
      /// P-Value
      float pValue;
      
      /// brain set used by this algorithm
      BrainSet* brain;
      
      /// surface used by this algorithm
      BrainModelSurface* bms;
      
      // area correction shape file
      MetricFile* areaCorrectionShapeFile;

      /// shuffled statistical map file
      MetricFile* shuffleStatisticalMapShapeFile;
      
      /// statistical map shape fie
      MetricFile* statisticalMapShapeFile;

      /// t-map variance smoothing iterations
      int tVarianceSmoothingIterations;
      
      /// t-map variance smoothing strength
      float tVarianceSmoothingStrength;
      
      /// compute degrees of freedom for statistical map
      bool doStatisticalMapDOF;
      
      /// compute p-value for statistical map
      bool doStatisticalMapPValue;
      
      /// number of threads for cluster search
      int numberOfThreads;
};

#endif // __BRAIN_MODEL_SURFACE_SHAPE_FIND_CLUSTERS_BASE_H__


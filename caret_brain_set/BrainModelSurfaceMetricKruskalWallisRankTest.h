
#ifndef __BRAIN_MODEL_SURFACE_METRIC_KRUSKAL_WALLIS_RANK_TEST_H__
#define __BRAIN_MODEL_SURFACE_METRIC_KRUSKAL_WALLIS_RANK_TEST_H__

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

#include "BrainModelSurfaceMetricFindClustersBase.h"

class MetricFile;

/// class for performing a krusk-wallis (non-parametric ANOVA) on metric files
class BrainModelSurfaceMetricKruskalWallisRankTest : public BrainModelSurfaceMetricFindClustersBase
{
   public:
      // constructor
      BrainModelSurfaceMetricKruskalWallisRankTest(BrainSet* bs,
                                         const std::vector<QString>& inputMetricFileNamesIn,
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
                                         const int numberOfThreadsIn);
                                         
      // destructor
      ~BrainModelSurfaceMetricKruskalWallisRankTest();
      
      
   protected:
      /// must be implemented by subclasses
      void executeClusterSearch() throw (BrainModelAlgorithmException);
      
      /// perform an F-Test on a set of metric files
      void performFTest(const std::vector<MetricFile*>& metricFiles,
                         MetricFile* outputMetricFile,
                         const int fStatisticColumn,
                         const int dofColumn,
                         const int pValueColumn) throw (BrainModelAlgorithmException);
                
      /// the input metric file names
      std::vector<QString> inputMetricFileNames;
      
      /// the input metric files
      std::vector<MetricFile*> inputMetricFiles;
      
      /// metric files that are output of shuffling
      std::vector<MetricFile*> shuffledMetricFiles;
      
      /// interations for generating shuffled F-Map file
      int iterations;
};

#endif // __BRAIN_MODEL_SURFACE_METRIC_KRUSKAL_WALLIS_RANK_TEST_H__

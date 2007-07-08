
#ifndef __BRAIN_MODEL_SURFACE_SHAPE_TWO_SAMPLE_H__
#define __BRAIN_MODEL_SURFACE_SHAPE_TWO_SAMPLE_H__

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

#include "BrainModelSurfaceMetricFindClustersBase.h"

class BrainModelSurface;
class MetricFile;
class QTextStream;

/// find significant clusters in surface shape files using a two sampled T-Test
class BrainModelSurfaceMetricTwoSampleTTest : public BrainModelSurfaceMetricFindClustersBase {
   public:
      /// data transform mode
      enum DATA_TRANSFORM_MODE {
         /// no transformation of data
         DATA_TRANSFORM_NONE,
         /// Wilcoxon Rank Sum input data then two-sample T-Test
         DATA_TRANSFORM_WILCOXON_RANK_SUM_THEN_TWO_SAMPLE_T_TEST
      };
      
      /// variance mode
      enum VARIANCE_MODE {
         /// use Donna's sigma
         VARIANCE_MODE_SIGMA,
         /// use pooled variance
         VARIANCE_MODE_POOLED,
         /// use unpooled variance
         VARIANCE_MODE_UNPOOLED
      };
      
      // constructor
      BrainModelSurfaceMetricTwoSampleTTest(BrainSet* bs,
                                                const DATA_TRANSFORM_MODE dataTransformModeIn,
                                                const VARIANCE_MODE varianceModeIn,
                                                const QString& shapeFileANameIn,
                                                const QString& shapeFileBNameIn,
                                                const QString& fiducialCoordFileNameIn,
                                                const QString& openTopoFileNameIn,
                                                const QString& distortionShapeFileNameIn,
                                                const QString& tMapFileNameIn,
                                                const QString& shuffledTMapFileNameIn,
                                                const QString& clustersPaintFileNameIn,
                                                const QString& clustersMetricFileNameIn,
                                                const QString& reportFileNameIn,
                                                const int distortionShapeFileColumnIn,
                                                const int iterationsIn,
                                                const float negativeThreshIn,
                                                const float positiveThreshIn,
                                                const float pValueIn,
                                                const int tVarianceSmoothingIterationsIn,
                                                const float tVarianceSmoothingStrengthIn,
                                                const bool doTMapDOF,
                                                const bool doTMapPValue,
                                                const int numberOfThreadsIn);
      
      // destructor
      ~BrainModelSurfaceMetricTwoSampleTTest();
      
   protected:
      /// must be implemented by subclasses
      void executeClusterSearch() throw (BrainModelAlgorithmException);
      
      // free memory
      void cleanUp();
      
      // create donna's sigma t-map
      MetricFile* createDonnasSigmaTMap(const MetricFile& mfA,
                                        const MetricFile& mfB,
                                        const bool addMeanValues) throw (BrainModelAlgorithmException);
      
      // create donna's shuffled sigma t-map
      MetricFile* createDonnasShuffledSigmaTMap(const MetricFile& mfA,
                                                const MetricFile& mfB) throw (BrainModelAlgorithmException);
      
      // finish donna's sigma processing
      void finishDonnasSigmaTMap(MetricFile& tMapFile,
                                 MetricFile& shuffledTMapFile) throw (BrainModelAlgorithmException);
      
      // transform the input data into rank-sum data
      void transformToRankSum(MetricFile& fileA, 
                              MetricFile& fileB) throw (BrainModelAlgorithmException);
      
      /// name of shape file A
      QString shapeFileAName;
   
      /// name of shape file B
      QString shapeFileBName;
   
      /// iterations for shuffled T-Map
      int iterations; 
      
      /// data transform mode
      DATA_TRANSFORM_MODE dataTransformMode;
      
      /// the variance mode
      VARIANCE_MODE varianceMode;
};

#endif // __BRAIN_MODEL_SURFACE_SHAPE_TWO_SAMPLE_H__


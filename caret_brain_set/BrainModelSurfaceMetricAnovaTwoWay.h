
#ifndef __BRAIN_MODEL_SURFACE_METRIC_ANOVA_TWO_WAY_H__
#define __BRAIN_MODEL_SURFACE_METRIC_ANOVA_TWO_WAY_H__

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

/// class for performing a two-way anova on metric files
class BrainModelSurfaceMetricAnovaTwoWay : public BrainModelSurfaceMetricFindClustersBase
{
   public:
      /// anova model type
      enum ANOVA_MODEL_TYPE {
         /// invalid type
         ANOVA_MODEL_TYPE_INVALID,
         /// fixed effect
         ANOVA_MODEL_TYPE_FIXED_EFFECT,
         /// random effect
         ANOVA_MODEL_TYPE_RANDOM_EFFECT,
         /// mixed effect (A=fixed, B=random)
         ANOVA_MODEL_TYPE_ROWS_FIXED_EFFECT_COLUMN_RANDOM_EFFECT
      };
      
      // constructor
      BrainModelSurfaceMetricAnovaTwoWay(BrainSet* bs,
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
                                         const int numberOfThreadsIn);
                                         
      // destructor
      ~BrainModelSurfaceMetricAnovaTwoWay();
      
      // set metric/shape file for a row and column
      void setMetricShapeFileName(const int row, 
                                  const int column,
                                  const QString& metricFileName) throw (BrainModelAlgorithmException);
                         
   protected:
      // must be implemented by subclasses
      void executeClusterSearch() throw (BrainModelAlgorithmException);
      
      // perform an F-Test on a set of metric files
      void performFTest(const std::vector<MetricFile*>& metricFiles,
                         MetricFile* outputMetricFile,
                         const int fStatisticColumn,
                         const int dofColumn,
                         const int pValueColumn) throw (BrainModelAlgorithmException);
      
      // get the index into one dimensional array of files or names
      int getFileIndex(const int rowNumber,
                       const int columnNumber) const;
                    
      /// type of ANOVA
      ANOVA_MODEL_TYPE anovaModelType;
      
      /// the input metric file names
      std::vector<QString> inputMetricFileNames;
      
      /// the input metric files
      std::vector<MetricFile*> inputMetricFiles;
      
      /// metric files that are output of shuffling
      std::vector<MetricFile*> shuffledMetricFiles;
      
      /// interations for generating shuffled F-Map file
      int iterations;
      
      /// number of rows of data
      int numberOfRows;
      
      /// number of columns of data
      int numberOfColumns;
      
};

#endif // __BRAIN_MODEL_SURFACE_METRIC_ANOVA_TWO_WAY_H__

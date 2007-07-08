
#ifndef __BRAIN_MODEL_SURFACE_ONE_SAMPLE_T_TEST_H
#define __BRAIN_MODEL_SURFACE_ONE_SAMPLE_T_TEST_H

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

/// class for performing a one sample T-Test on a metric file
class BrainModelSurfaceMetricOneAndPairedTTest : public BrainModelSurfaceMetricFindClustersBase {
   public:
      //
      // T-Test mode
      //
      enum T_TEST_MODE {
         T_TEST_MODE_ONE_SAMPLE,
         T_TEST_MODE_PAIRED
      };
      
      // constructor
      BrainModelSurfaceMetricOneAndPairedTTest(BrainSet* bs,
                                      const T_TEST_MODE tTestModeIn,
                                      const std::vector<QString>& metricFileNamesIn,
                                      const QString& fiducialCoordFileNameIn,
                                      const QString& openTopoFileNameIn,
                                      const QString& areaCorrectionMetricFileNameIn,
                                      const QString& tMapFileNameIn,
                                      const QString& permutedTMapFileNameIn,
                                      const QString& clustersPaintFileNameIn,
                                      const QString& clustersMetricFileNameIn,
                                      const QString& reportFileNameIn,
                                      const int areaCorrectionMetricFileColumnIn,
                                      const float negativeThreshIn,
                                      const float positiveThreshIn,
                                      const float pValueIn,
                                      const int tVarianceSmoothingIterationsIn,
                                      const float tVarianceSmoothingStrengthIn,
                                      const int permutationIterationsIn,
                                      const float tTestConstantIn,
                                      const int numberOfThreadsIn);
      
      // destructor
      ~BrainModelSurfaceMetricOneAndPairedTTest();
      
   protected:
      // execute the search
      virtual void executeClusterSearch() throw (BrainModelAlgorithmException);
      
      // free memory
      virtual void cleanUp();
      
      // create the metric file for one-sample T-Test processing
      void oneSampleTTestProcessing(MetricFile& metricFileOut) throw (BrainModelAlgorithmException);
      
      // create the metric file for paired T-Test processing
      void pairedTTestProcessing(MetricFile& metricFileOut) throw (BrainModelAlgorithmException);
      
      /// name of input metric file
      std::vector<QString> metricFileNames;
      
      /// constant to perform T-Test against
      float tTestConstant;
      
      /// permutation interations
      int permutationIterations;
      
      /// t-test mode
      T_TEST_MODE tTestMode;    
};

#endif // __BRAIN_MODEL_SURFACE_ONE_SAMPLE_T_TEST_H


#ifndef __BRAIN_MODEL_SURFACE_METRIC_GRADIENT_H__
#define __BRAIN_MODEL_SURFACE_METRIC_GRADIENT_H__

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

#include "BrainModelAlgorithm.h"

class CoordinateFile;
class MetricFile;
class VectorFile;
class TopologyHelper;

/// class for create a functional volume using a probabilistic volume
class BrainModelSurfaceROIMetricGradient : public BrainModelAlgorithm {
   public:
      /// Constructor for execution on single column
      BrainModelSurfaceROIMetricGradient(BrainSet* bs,
                                        int bsIndexIn,
                                        MetricFile* roiIn,
                                        MetricFile* valuesIn,
                                        int metricIndexIn,
                                        VectorFile* gradOutIn,
                                        MetricFile* gradMagOutIn,
                                        int magOutIndexIn,
                                        bool avgNormalsIn = false);
                                            
      /// Constructor for execution of all columns
      BrainModelSurfaceROIMetricGradient(BrainSet* bs,
                                        int bsIndexIn,
                                        MetricFile* roiIn,
                                        MetricFile* valuesIn,
                                        bool avgNormalsIn,
                                        bool parallelFlagIn);
                                            
      /// Destructor
      ~BrainModelSurfaceROIMetricGradient();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   private:
      /// execute on single metric column
      void executeSingleColumn() throw (BrainModelAlgorithmException); 
      
      void executeAllColumns() throw (BrainModelAlgorithmException);
      void processSingleColumn(const TopologyHelper* myhelper,
                                                     const float* allnormals, 
                                                     const float* sourceData,
                                                     const float* roiValues,
                                                     const int columnIndex,
                                                     const int numNodes) throw (BrainModelAlgorithmException);
                               
      void initialize();
      
      bool allColumnsFlag;
      bool parallelFlag;
      int setIndex, metricIndex, magOutIndex, depth;
      MetricFile* values, *roi, *gradMagOut;
      VectorFile* gradOut;
      bool avgNormals;
      void crossProd(const float in1[3], const double in2[3], double out[3]);
      void crossProd(const double in1[3], const double in2[3], double out[3]);
      double dotProd(const double in1[3], const double in2[3]);
      double dotProd(const float in1[3], const double in2[3]);
      void normalize(double in[3]);
      void coordDiff(const float* coord1, const float* coord2, double out[3]);
      void calcrref(double* matrix[], int rows, int cols);
      double det3(double* matrix[], int column);
      
      bool haveWarned;
      bool haveFailed;

};

#endif // __BRAIN_MODEL_SURFACE_METRIC_GRADIENT_H__


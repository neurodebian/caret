
#ifndef __BRAIN_MODEL_SURFACE_ROI_METRIC_SMOOTHING_H__
#define __BRAIN_MODEL_SURFACE_ROI_METRIC_SMOOTHING_H__

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

#include <vector>

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class CoordinateFile;
class MetricFile;

/// Class for smoothing metric data
class BrainModelSurfaceROIMetricSmoothing : public BrainModelAlgorithm {
   public:            
      /// Constructor
      BrainModelSurfaceROIMetricSmoothing(BrainSet* bs,
                                       BrainModelSurface* fiducialSurfaceIn,
                                       MetricFile* metricFileIn,
                                       MetricFile* roiFileIn,                                       
                                       const int columnIn, 
                                       const int outputColumnIn,
                                       const QString& outputColumnNameIn,
                                       const float strengthIn,//all of these are unused
                                       const int iterationsIn,//technically this is used, but it should always be 1, though it might be slightly faster (in theory) to iterate small kernels to equal a large kernel
                                       const float gaussNormBelowCutoffIn,//unused
                                       const float gaussNormAboveCutoffIn,//unused
                                       const float gaussSigmaNormIn,//unused
                                       const float gaussSigmaTangIn,//unused
                                       const float gaussTangentCutoffIn,//to here
                                       const float geodesicGaussSigmaIn); // = 2.0f);//this one is the only smoothing parameter actually used
      /// Parallel Smoothing Constructor
      BrainModelSurfaceROIMetricSmoothing(BrainSet* bs,
                                       BrainModelSurface* fiducialSurfaceIn,
                                       MetricFile* metricFileIn,
                                       MetricFile* roiFileIn,                                       
                                       const float strengthIn,//unused
                                       const int iterationsIn,//should be 1
                                       const float gaussNormBelowCutoffIn,//unused
                                       const float gaussNormAboveCutoffIn,//unused
                                       const float gaussSigmaNormIn,//unused
                                       const float gaussSigmaTangIn,//unused
                                       const float gaussTangentCutoffIn,//unused
                                       const float geodesicGaussSigmaIn,//=2.0f
                                       const bool parallelSmoothingFlagIn=true);
      
      /// Destructor
      ~BrainModelSurfaceROIMetricSmoothing();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      void smoothSingleColumn(const QString& columnDescription,
                         const int inputColumn,
                         const int outputColumn);
   protected:
      /// determine neighbors for each node
      void determineNeighbors();
      
      /// class for neighbor information
      class NeighborInfo {
         public:
            /// Constructor
            NeighborInfo(const std::vector<int>& neighborsIn,
                         const std::vector<float>& distances,
                         const std::vector<float>& geoWeights,
                         const float* roiValuesIn);
            
            NeighborInfo() { };//default constructor to make vector resize happy
            
            /// Destructor
            ~NeighborInfo();
            
            /// the neighbors
            std::vector<int> neighbors;
            
            /// distance to neighbor 
            std::vector<float> distanceToNeighbor;
            
            /// distance based precomputed gaussian weight
            std::vector<float> geoGaussWeights;
            
            /// number of neighbors
            int numNeighbors;
      };
      
      /// neighbors for each node
      std::vector<NeighborInfo> nodeNeighbors;
      
      /// number of nodes
      int numberOfNodes;
      
      /// fiducial surface used for smoothing
      BrainModelSurface* fiducialSurface;
      
      /// the metric file 
      MetricFile* metricFile;
      
      /// the roi file 
      MetricFile* roiFile;

      /// column number for input to smoothing
      int column;
      
      /// column number for output of smoothing
      int outputColumn;
      
      /// name of output column
      QString outputColumnName;
      
      /// smoothing strength
      float strength;
      
      /// number of smoothing iterations
      int iterations;
      
      /// gaussian norm below cutoff
      float gaussNormBelowCutoff;
      
      /// gaussian norma above cutoff
      float gaussNormAboveCutoff;
      
      /// gaussian sigma norm
      float gaussSigmaNorm;
      
      /// gaussian sigma tang
      float gaussSigmaTang;
      
      /// gaussian tangent cutoff
      float gaussTangentCutoff;
      
      /// parameter for geodesic gaussian
      float geodesicGaussSigma;
      
      /// roiValues for all nodes
	  float *roiValues;
     
     /// smoothing all columns
     bool smoothAllColumnsFlag;
     
     /// parallelSmoothing
     bool runParallelFlag;
};

#endif // __BRAIN_MODEL_SURFACE_METRIC_SMOOTHING_H__

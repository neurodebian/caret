
#ifndef __BRAIN_MODEL_SURFACE_METRIC_SMOOTHING_H__
#define __BRAIN_MODEL_SURFACE_METRIC_SMOOTHING_H__

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
class BrainModelSurfaceMetricSmoothing : public BrainModelAlgorithm {
   public:
      /// smoothing algorithms
      enum SMOOTH_ALGORITHM {
         SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS,
         SMOOTH_ALGORITHM_DILATE,
         SMOOTH_ALGORITHM_FULL_WIDTH_HALF_MAXIMUM,
         SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN,
         SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN,
         SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS,
         SMOOTH_ALGORITHM_NONE
      };
      
      /// Constructor
      BrainModelSurfaceMetricSmoothing(BrainSet* bs,
                                       BrainModelSurface* fiducialSurfaceIn,
                                       BrainModelSurface* gaussianSphericalSurfaceIn,
                                       MetricFile* metricFileIn,
                                       const SMOOTH_ALGORITHM algorithmIn,
                                       const int columnIn, 
                                       const int outputColumnIn,
                                       const QString& outputColumnNameIn,
                                       const float strengthIn,
                                       const int iterationsIn,
                                       const float desiredFullWidthHalfMaximumIn,
                                       const float gaussNormBelowCutoffIn,
                                       const float gaussNormAboveCutoffIn,
                                       const float gaussSigmaNormIn,
                                       const float gaussSigmaTangIn,
                                       const float gaussTangentCutoffIn,
                                       const float geodesicGaussSigmaIn); // = 2.0f);
      
      /// Destructor
      ~BrainModelSurfaceMetricSmoothing();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// get the full width half maximum smoothing results description
      QString getFullWidthHalfMaximumSmoothingResultsDescription() const 
                         { return fullWidthHalfMaximumSmoothingResultsDescription; }
      
   protected:
      /// determine neighbors for each node
      void determineNeighbors();
      
      /// class for neighbor information
      class NeighborInfo {
         public:
            /// Constructor
            NeighborInfo(const CoordinateFile* cf,
                         const int myNodeNumber,
                         const std::vector<int>& neighborsIn,
                         const float maxDistanceCutoff,
                         const std::vector<float>* distanceWeights = NULL);
            
            /// Destructor
            ~NeighborInfo();
            
            /// Default constructor so vector resize can work
            NeighborInfo() {};
            
            /// the neighbors
            std::vector<int> neighbors;
            
            /// precomputed geodesic gaussian weights
            std::vector<float> geoGaussWeight;
            
            /// neighbor distances
            std::vector<float> distanceToNeighbor;
            
            /// number of neighbors
            int numNeighbors;
      };
      
      /// neighbors for each node
      std::vector<NeighborInfo> nodeNeighbors;
      
      /// number of nodes
      int numberOfNodes;
      
      /// fiducial surface used for smoothing
      BrainModelSurface* fiducialSurface;
      
      /// spherical surface used for overall distance cutoff when gaussian smoothing
      BrainModelSurface* gaussianSphericalSurface;
      
      /// the metric file 
      MetricFile* metricFile;
      
      /// the smoothing algorithm
      SMOOTH_ALGORITHM algorithm;
      
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
      
      /// desired full width half maximum
      float desiredFullWidthHalfMaximum;
      
      /// parameter for geodesic gaussian
      float geodesicGaussSigma;
      
      /// full width half maximum smoothing results description
      QString fullWidthHalfMaximumSmoothingResultsDescription;
};

#endif // __BRAIN_MODEL_SURFACE_METRIC_SMOOTHING_H__

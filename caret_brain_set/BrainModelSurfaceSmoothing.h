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

#ifndef __BRAIN_MODEL_SURFACE_SMOOTHING_H__
#define __BRAIN_MODEL_SURFACE_SMOOTHING_H__

#include <vector>

#include <QMutex>

#include "BrainModelAlgorithmMultiThreaded.h"

class BrainModelSurface;
class QWaitCondition;
class TopologyHelper;

/// class for smoothing a brain model surface, multiple threads optional
class BrainModelSurfaceSmoothing : public BrainModelAlgorithmMultiThreaded {
   public:
      /// Types of smoothing
      enum SMOOTHING_TYPE {
         SMOOTHING_TYPE_AREAL,
         SMOOTHING_TYPE_LINEAR,
         SMOOTHING_TYPE_LANDMARK_CONSTRAINED,
         SMOOTHING_TYPE_LANDMARK_NEIGHBOR_CONSTRAINED
      };
      
      /// constructor
      BrainModelSurfaceSmoothing(BrainSet* bs,
                                 BrainModelSurface* surfaceIn,
                                 const SMOOTHING_TYPE smoothingTypeIn,
                                 const float strengthIn,
                                 const int   iterationsIn,
                                 const int   edgeIterationsIn,
                                 const int   landmarkNeighborIterationsIn,
                                 const std::vector<bool>* smoothOnlyTheseNodesIn,
                                 const std::vector<bool>* landmarkNodeFlagsIn,
                                 const int projectToSphereEveryXIterationsIn,
                                 const int numberOfThreadsIn);
      
      /// Destructor
      ~BrainModelSurfaceSmoothing();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// class stores information about each node
      class NodeInfo {
         public:
            /// constructor
            NodeInfo();
            
            /// node types
            enum NODE_TYPE {
               NODE_TYPE_DO_NOT_SMOOTH,
               NODE_TYPE_NORMAL,
               NODE_TYPE_LANDMARK,
               NODE_TYPE_LANDMARK_NEIGHBOR
            };
            
            /// type of node
            NODE_TYPE nodeType;
            
            /// edge node flag
            int edgeNodeFlag;
            
            /// number of landmark neighbors
            int numLandmarkNeighbors;
            
            /// offset of a landmark node from average of its neighbors
            float offset[3]; 
      };
      
      /// Constructor (protected) used to created threaded executions.
      BrainModelSurfaceSmoothing(BrainSet* bs,
                                       const SMOOTHING_TYPE smoothingTypeIn,
                                       const float strengthIn,
                                       const float landmarkScaleIn,
                                       NodeInfo* nodeInfoIn,
                                       TopologyHelper* topologyHelperIn,
                                       const int startNodeIndexIn,
                                       const int endNodeIndexIn,
                                       BrainModelSurfaceSmoothing* parentOfThisThreadIn,
                                       const int threadNumberIn);
                                       
      /// Initialize member variables.
      void initialize();
      
      /// smooths for an iteration (required by QThread)
      void run();
      
      /// set smooth edges this iteration
      void setSmoothEdgesThisIteration(const bool smooth) { smoothEdgesThisIteration = smooth; }
      
      /// set smooth landmark neighbors this iteration
      void setSmoothLandmarkNeighborsThisIteration(const bool smooth)
                          { smoothLandmarkNeighborsThisIteration = smooth; }
 
      /// set the indices of the nodes that are to be smoothed (inclusive)
      void setIndicesOfNodesToSmooth(const int startNodeIndexIn, const int endNodeIndexIn);
      
      /// set the input and output coords
      void setInputAndOutputCoords(float* inCoords, float* outCoords);
      
      /// surface that is smoothed (DO NOT DELETE)
      BrainModelSurface* surface;
      
      /// smoothing type
      SMOOTHING_TYPE smoothingType;
      
      /// smoothing strength;
      float strength;
      
      /// inverse of strength (1.0 - strength)
      float inverseStrength;
      
      /// smoothing iterations
      int iterations;
      
      /// smoothing edge iterations
      int edgeIterations;
      
      /// smoothing landmark neighbors iterations
      int landmarkNeighborIterations;
      
      /// number of nodes
      int numberOfNodes;
      
      /// topology helper used to get node neighbors
      TopologyHelper* topologyHelper; 
      
      /// array for holding coordinates (inputCoords and outputCoords point to this)
      float* coordsArray1;
      
      /// array for holding coordinates (inputCoords and outputCoords point to this)
      float* coordsArray2;
      
      /// pointers for input coordinates (do not delete)
      float* inputCoords;
      
      /// pointers for output coordinates (do not delete)
      float* outputCoords;

      /// smooth edges this iteration
      bool smoothEdgesThisIteration;
      
      /// index of first node to smooth
      int startNodeIndex;
      
      /// index of last node to smooth
      int endNodeIndex;
      
      /// threads when running multi-threaded.
      std::vector<BrainModelSurfaceSmoothing*> threads;
      
      /// project to sphere every X iterations
      int projectToSphereEveryXIterations;
      
      /// information about each node
      NodeInfo* nodeInfo;
      
      /// scale for landmark nodes
      float landmarkScale;
      
      /// smooth landmark neighbors this iteration
      bool smoothLandmarkNeighborsThisIteration;
};

#ifdef __BRAIN_MODEL_SURFACE_SMOOTHING_MAIN_H__
#endif // __BRAIN_MODEL_SURFACE_SMOOTHING_MAIN_H__


#endif // __BRAIN_MODEL_SURFACE_SMOOTHING_H__


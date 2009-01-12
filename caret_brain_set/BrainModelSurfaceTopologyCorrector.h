
#ifndef __BRAIN_MODEL_SURFACE_TOPOLOGY_CORRECTOR_H__
#define __BRAIN_MODEL_SURFACE_TOPOLOGY_CORRECTOR_H__

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

#include <set>
#include <vector>

#include "BrainModelAlgorithm.h"
#include "Tessellation.h"

class QTime;

class BrainModelSurface;
class BrainModelSurfaceROINodeSelection;
class CoordinateFile;
class PointLocator;
class TopologyFile;

/// Class that corrects topological errors in a surface
class BrainModelSurfaceTopologyCorrector : public BrainModelAlgorithm {
   public:
      // Constructor
      BrainModelSurfaceTopologyCorrector(BrainSet* bsIn,
                                         const BrainModelSurface* fiducialSurfaceIn,
                                         const int numberOfSmoothingCyclesIn = 30,
                                         const bool removeHighlyCompressedNodesIn = true,
                                         const float compressedNodesArealDistortionThresholdIn = -7.0);
      // Destructor
      ~BrainModelSurfaceTopologyCorrector();
      
      /// skip corrected surface generation (just determine nodes that are removed)
      void setSkipCorrectedSurfaceGeneration(const bool skipIt) { skipCorrectedSurfaceGeneration = skipIt; }
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// get a pointer to the new surface
      BrainModelSurface* getPointerToNewSurface() { return fiducialSurface; }
      
      // get a list of node numbers that were removed
      void getListOfNodesThatWereRemoved(std::vector<int>& nodesRemoved) const;
      
      // get a list of node numbers that were removed
      void getListOfNodesThatWereRemoved(BrainModelSurfaceROINodeSelection& nodesRemovedROI) const;
      
   protected:
      /// smooth around removed nodes
      void smoothAroundRemovedNodes();
      
      /// Flag nodes in highly compressed tiles as unavailable
      void removeNodesInHighlyCompressedTilesFromAvailableNodes() throw (BrainModelAlgorithmException);
      
      /// Flag the crossover nodes as unavailable
      void removeCrossoverNodesFromAvailableNodes();
      
      /// retessellate the sphere
      BrainModelSurface* retessellateTheSphericalSurface() throw (BrainModelAlgorithmException);
      
      /// Smooth the spherical surface.
      void smoothSphericalSurfaceToMinimizeCrossovers() throw (BrainModelAlgorithmException);

      /// the fiducial surface   DO NOT DELETE
      BrainModelSurface* fiducialSurface;
      
      /// the spherical surface  DO NOT DELETE
      BrainModelSurface* sphericalSurface;
      
      /// number of nodes
      int numNodes;
      
      /// copy of the original input topology file
      TopologyFile* copyOfOriginalTopologyFile;
      
      /// topology file used to maintain connected nodes
      TopologyFile* workingTopologyFile;
      
      /// number of smoothing cycles to find crossovers
      int numberOfSmoothingCycles;
      
      /// remove highly compressed nodes
      bool removeHighlyCompressedNodes;
      
      /// highly compressed nodes have areal distortion less than this
      float compressedNodesArealDistortionThreshold;
      
      /// skip generation of corrected surfaces
      bool skipCorrectedSurfaceGeneration;
};

#endif // __BRAIN_MODEL_SURFACE_TOPOLOGY_CORRECTOR_H__


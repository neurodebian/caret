
#ifndef __BRAIN_MODEL_SURFACE_CLUSTER_TO_BORDER_CONVERTER_H__
#define __BRAIN_MODEL_SURFACE_CLUSTER_TO_BORDER_CONVERTER_H__

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
class BrainModelSurfaceROINodeSelection;
class TopologyFile;
class TopologyHelper;

/// class for creating borders around clusters of nodes
class BrainModelSurfaceClusterToBorderConverter : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceClusterToBorderConverter(BrainSet* bs,
                                                BrainModelSurface* bms,
                                                TopologyFile* tfIn,
                                                const QString& borderName,
                                                BrainModelSurfaceROINodeSelection *surfaceROIIn,
                                                const bool projectTheBordersFlagIn);
      
      // destructor
      ~BrainModelSurfaceClusterToBorderConverter();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// get the number of borders created
      int getNumberOfBordersCreated() const { return numberOfBordersCreated; }
      
   protected:
      /// node status
      enum STATUS {
         /// node is outside of a cluster
         STATUS_OUTSIDE,
         /// node is inside the cluster
         STATUS_INSIDE,
         /// node is on the boundary of a cluster
         STATUS_BOUNDARY,
         /// node was on the boundary of a cluster
         STATUS_WAS_BOUNDARY
      };

      // execute original algorithm that misses some clusters
      void executeOriginal() throw (BrainModelAlgorithmException);
      
      // execute new algorithm
      void executeNew() throw (BrainModelAlgorithmException);
      
      // get the boundary neighor count for a node
      int getBoundaryNeighborCount(const int nodeNumber) const;
      
      // cleanup cluster nodes such as those with one or zero neighbors
      void cleanupClusterNodes();
      
      /// the surface
      BrainModelSurface* bms;
      
      /// the border name for new borders
      QString borderName;
      
      /// the topology file
      TopologyFile* topologyFile;
      
      /// the topology helper
      TopologyHelper* topologyHelper;
      
      /// node status
      std::vector<int> nodeStatus;
      
      /// number of borders created
      int numberOfBordersCreated;
      
      /// surface region of interest node selection
      BrainModelSurfaceROINodeSelection* surfaceROI;
      
      /// project the borders flag
      bool projectTheBordersFlag;
};

#endif // __BRAIN_MODEL_SURFACE_CLUSTER_TO_BORDER_CONVERTER_H__


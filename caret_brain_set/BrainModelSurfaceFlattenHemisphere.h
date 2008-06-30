
#ifndef __BRAIN_MODEL_SURFACE_FLATTEN_HEMISPHERE_H__
#define __BRAIN_MODEL_SURFACE_FLATTEN_HEMISPHERE_H__

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

#include "BorderProjectionFile.h"
#include "BrainModelAlgorithm.h"

class AreaColorFile;
class BrainModelSurface;
class PaintFile;

/// class for flattening a hemisphere
class BrainModelSurfaceFlattenHemisphere : public BrainModelAlgorithm {
   public:
      /// constructor
      BrainModelSurfaceFlattenHemisphere(BrainSet* bsIn,
                                         const BrainModelSurface* fiducialSurfaceIn,
                                         const BrainModelSurface* ellipsoidOrSphericalSurfaceIn,
                                         const BorderProjectionFile* flattenBorderProjectionFileIn,
                                         PaintFile* paintFileInOut,
                                         AreaColorFile* areaColorFileInOut,
                                         const bool createFiducialWithSmoothedMedialWallFlagIn);

      /// destructor
      ~BrainModelSurfaceFlattenHemisphere();
       
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// get the spherical surface that was added to the brain set
      BrainModelSurface* getSphericalSurface() const { return outputSphericalSurface; }
      
      /// get the flat surface that was added to the brain set
      BrainModelSurface* getInitialFlatSurface() const { return outputInitialFlatSurface; }
      
      /// get the open topology file that was added to the brain set
      TopologyFile* getOpenTopologyFile() const { return outputOpenTopologyFile; }
      
      /// get the cut topology file that was added to the brain set
      TopologyFile* getCutTopologyFile() const { return outputCutTopologyFile; }
      
      /// get the fiducial with smoothed medial wall added to brain set
      BrainModelSurface* getFiducialSurfaceWithSmoothedMedialWall() const
                    { return outputFiducialSurfaceWithSmoothedMedialWall; }

   protected:
      /// find the flattening borders
      void findFlatteningBorders() throw (BrainModelAlgorithmException);
      
      /// create the spherical surface
      void createSphericalSurface() throw (BrainModelAlgorithmException);
      
      /// create the initial flat surface
      void createInitialFlatSurface() throw (BrainModelAlgorithmException);
      
      /// remove medial wall assignments from paint file
      void removeMedialWallAssignmentsFromPaintFile();
      
      /// the input fiducial surface
      const BrainModelSurface* inputFiducialSurface;
      
      /// the input spherical surface
      const BrainModelSurface* inputSphericalSurface;
      
      /// the flatten border projection file
      const BorderProjectionFile* inputFlattenBorderProjectionFile;
      
      /// the create fiducial surface with smoothed medial wall flag
      const bool createFiducialWithSmoothedMedialWallFlag;
      
      /// the output spherical surface
      BrainModelSurface* outputSphericalSurface;
      
      /// the output initial flat surface
      BrainModelSurface* outputInitialFlatSurface;
      
      /// the output open topology file
      TopologyFile* outputOpenTopologyFile;
      
      /// the output cut stopology file
      TopologyFile* outputCutTopologyFile;
      
      /// the medial wall border
      BorderProjection medialWallBorderProjection;
      
      /// borders that are applied as cuts
      BorderProjectionFile cutBorderProjectionFile;

      /// paint file 
      PaintFile* paintFile;
      
      /// area color file
      AreaColorFile* areaColorFile;
      
      /// fiducial surface with smoothed medial wall
      BrainModelSurface* outputFiducialSurfaceWithSmoothedMedialWall;
      
};

#endif // __BRAIN_MODEL_SURFACE_FLATTEN_HEMISPHERE_H__


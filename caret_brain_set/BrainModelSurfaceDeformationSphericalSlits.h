
#ifndef __BRAIN_MODEL_SURFACE_DEFORMATION_SPHERICAL_SLITS_H__
#define __BRAIN_MODEL_SURFACE_DEFORMATION_SPHERICAL_SLITS_H__

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

class BorderProjectionFile;
class BrainModelSurface;

/// class for registering surfaces where the source surface does not have
/// cortical areas that exist in the target
class BrainModelSurfaceDeformationSphericalSlits : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceDeformationSphericalSlits(
                                 BrainSet* brainSetIn,
                                 const BrainModelSurface* sphericalSurfaceIn,
                                 const BorderProjectionFile* slitLandmarkBorderProjectionFileIn);

      // destructor
      ~BrainModelSurfaceDeformationSphericalSlits();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      // get the cut spherical surface that was added to the brain set
      BrainModelSurface* getCutSphericalSurface() { return cutSphericalSurface; }

      // get the cut topology file that was added to the brain set
      TopologyFile* getCutSphericalSurfaceTopologyFile() { return cutSphericalSurfaceTopologyFile; }

      // the smoothed closed spherical surface
      BrainModelSurface* getSmoothedClosedSphericalSurface() { return smoothedClosedSphericalSurface; }

      // get the closed topology file for the smoothed cut spherical surface
      TopologyFile* getSmoothedClosedSphericalSurfaceTopologyFile() 
                    { return smoothedClosedSphericalSurfaceTopologyFile; }     
       
      // get the prefix for slit borders
      static QString getSlitLandmarkBorderNamePrefix() { return "LANDMARK.SLIT"; }
      
   protected:
      /// the input spherical surface
      const BrainModelSurface* inputSphericalSurface;
      
      /// the input slit landmark projections
      const BorderProjectionFile* inputSlitLandmarkBorderProjectionFile;
      
      // the cut spherical surface
      BrainModelSurface* cutSphericalSurface;
      
      // the smoothed closed spherical surface
      BrainModelSurface* smoothedClosedSphericalSurface;
      
      // the cut spherical surface topology file
      TopologyFile* cutSphericalSurfaceTopologyFile;     
      
      // the closed topology file for the smoothed spherical surface
      TopologyFile* smoothedClosedSphericalSurfaceTopologyFile;
};

#endif // __BRAIN_MODEL_SURFACE_DEFORMATION_SPHERICAL_SLITS_H__
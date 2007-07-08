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


#ifndef __BRAIN_MODEL_SURFACE_DEFORMATION_SPHERICAL_H__
#define __BRAIN_MODEL_SURFACE_DEFORMATION_SPHERICAL_H__

#include "BrainModelSurfaceDeformation.h"
#include "SurfaceShapeFile.h"

/// this class performs a spherical deformation
class BrainModelSurfaceDeformationSpherical : public BrainModelSurfaceDeformation {
   public:
      /// Constructor
      BrainModelSurfaceDeformationSpherical(BrainSet* brainSetIn,
                                       DeformationMapFile* DeformationMapFileIn);
      
      /// Destructor
      ~BrainModelSurfaceDeformationSpherical();
      
   protected:
      /// Execute the deformation
      void executeDeformation() throw (BrainModelAlgorithmException);
      
      /// determine the fiducial sphere distortion
      void determineFiducialSphereDistortion();
      
      /// update the fiducial sphere distortion
      void updateSphereFiducialDistortion(const int cycle);
      
      /// Determine distortion ratio of fiducial vs spherical tile areas.
      void determineSphericalDistortion(const BrainModelSurface* fiducialSurface,
                                        const BrainModelSurface* sphericalSurface,
                                        std::vector<float>& tileDistortion);

      /// Perform landmark constrained smoothing on the target deformation sphere
      void landmarkConstrainedSmoothTarget();
      
      // Perform landmark constrained morphing on the sphere with source landmarks
      void landmarkMorphContrainedSource(const int cycleNumber) throw (BrainModelAlgorithmException);
      
      /// Perform landmark neighbor constrained smoothing on the sphere with source landmarks
      void landmarkNeighborConstrainedSmoothSource(const int cycleNumber);

      /// Load the regularly tessellated sphere and set its radius.
      void loadRegularSphere() throw (BrainModelAlgorithmException);

      /// Replace the target landmarks in the deformation sphere with the source landmarks
      void replaceTargetLandmarksWithSourceLandmarks();

      /// tessellate the target border into the target deformation sphere
      void tessellateTargetBordersIntoDeformationSphere() throw (BrainModelAlgorithmException);
      
      /// create the deformed coordinate file at the end of each cycle
      void createDeformedCoordinateFile(const int cycle);
      
      /// update the deformed source borders for the next cycle
      void updateSourceBordersForNextCycle() throw (BrainModelAlgorithmException);
      
      /// the target deformation sphere
      BrainModelSurface* targetDeformationSphere;
      
      /// the unsmoothed target deformation sphere
      BrainModelSurface* unsmoothedTargetDeformationSphere;
      
      /// the unsmoothed source deformation sphere
      BrainModelSurface* unsmoothedSourceDeformationSphere;
      
      /// the smoothed source deformation sphere
      BrainModelSurface* smoothedSourceDeformationSphere;
      
      /// the morphed source deformation sphere
      BrainModelSurface* morphedSourceDeformationSphere;
      
      /// the number of nodes in the regularly tessellated sphere prior to inserting landmarks
      int originalNumberOfNodes;
      
      /// keeps track of borders that are tessellated into the deformation sphere
      std::vector<std::pair<int,int> > usedBorderLinks;
      
      /// target file naming prefix for debugging
      QString debugTargetFileNamePrefix;
      
      /// source file naming prefix for debugging
      QString debugSourceFileNamePrefix;
      
      /// radius of the deformation sphere
      float deformationSphereRadius;
      
      /// Keep track of distortion
      SurfaceShapeFile fiducialSphereDistortion;
   
      /// ratio of target fiducial and spherical tile areas
      std::vector<float> targetTileDistortion;
      
      /// ratio of source fiducial and spherical tile areas
      std::vector<float> sourceTileDistortion;
      
      /// keeps track of landmark nodes
      std::vector<bool> landmarkNodeFlags;
      
      /// keeps track of crossovers in each cycle
      std::vector<int> crossoverCount;
};

#endif // __BRAIN_MODEL_SURFACE_DEFORMATION_SPHERICAL_H__


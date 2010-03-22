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


#ifndef __BRAIN_MODEL_SURFACE_DEFORMATION_SPHERICAL_VECTOR_H__
#define __BRAIN_MODEL_SURFACE_DEFORMATION_SPHERICAL_VECTOR_H__

#include "BrainModelSurfaceDeformation.h"
#include "SurfaceShapeFile.h"

/// this class performs a spherical vector deformation
class BrainModelSurfaceDeformationSphericalVector : public BrainModelSurfaceDeformation {
   public:
      /// Constructor
      BrainModelSurfaceDeformationSphericalVector(BrainSet* brainSetIn,
                                       DeformationMapFile* DeformationMapFileIn);
      
      /// Destructor
      ~BrainModelSurfaceDeformationSphericalVector();
      
   protected:
      /// Execute the deformation
      void executeDeformation() throw (BrainModelAlgorithmException);
      
      // Perform landmark constrained morphing on the sphere with source landmarks
      void landmarkMorphContrainedSource(BrainModelSurface* referenceDeformationSphere,
                                         const int cycleNumber) throw (BrainModelAlgorithmException);
      
      /// Perform landmark neighbor constrained smoothing on the sphere with source landmarks
      void landmarkNeighborConstrainedSmoothSource(const int cycleNumber);

      /// Get a regularly tessellated sphere and set its radius.
      BrainModelSurface* getRegularSphere(BrainSet* bs, float radius) throw (BrainModelAlgorithmException);

      /// tessellate the target border into the target deformation sphere
      void tessellateTargetBordersIntoDeformationSphere() throw (BrainModelAlgorithmException);
      
      /// tessellate the source border into the source deformation sphere
      void tessellateSourceBordersIntoDeformationSphere() throw (BrainModelAlgorithmException);

      /// create the deformed coordinate file at the end of each cycle
      void createDeformedCoordinateFile(BrainModelSurface* sourceDeformationSphere,
                                        BrainModelSurface* registeredDeformationSourceSphere,
                                        const int cycleNumber);
      
      /// write a border file containing the deformed landmarks
      void writeSourceBorderLandmarkFile(BrainModelSurface* surface,
                                         const int cycleNumber);

      /// write coords without the landmarks
      void writeCoordinatesWithoutLandmarks(BrainModelSurface* surface,
                                            const int cycleNumber);

      /// create the surface shape file containing the XYZ differences
      SurfaceShapeFile* createDifferenceShapeFile(int iterations);

      /// determine the fiducial sphere distortion
      void determineFiducialSphereDistortion();

      /// update the fiducial sphere distortion
      void updateSphereFiducialDistortion(const int cycle,
                                    BrainModelSurface* morphedSourceDeformationSphere);

      /// Determine distortion ratio of fiducial vs spherical tile areas.
      void determineSphericalDistortion(const BrainModelSurface* fiducialSurface,
                                        const BrainModelSurface* sphericalSurface,
                                        std::vector<float>& tileDistortion);

      /// move the landmark nodes to the average of their neighboring nodes
      void moveLandmarksToAverageOfNeighbors(BrainModelSurface* bms);

      /// the target deformation sphere
      BrainModelSurface* targetDeformationSphere;

      /// the source deformation sphere
      BrainModelSurface* sourceDeformationSphere;

      /// the reference source deformation sphere
      BrainModelSurface* referenceSourceDeformationSphere;

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
      
      /// keeps track of landmark nodes
      std::vector<bool> landmarkNodeFlags;
      
      /// keeps track of crossovers in each cycle
      std::vector<int> crossoverCount;

      /// the target brain set
      BrainSet* targetDeformationBrainSet;

      /// Keep track of distortion
      SurfaceShapeFile fiducialSphereDistortion;

      /// ratio of target fiducial and spherical tile areas
      std::vector<float> targetTileDistortion;

      /// ratio of source fiducial and spherical tile areas
      std::vector<float> sourceTileDistortion;

      /// shape file containing border variances for the landmark nodes
      SurfaceShapeFile borderVarianceValuesShapeFile;
};

#endif // __BRAIN_MODEL_SURFACE_DEFORMATION_SPHERICAL_VECTOR_H__


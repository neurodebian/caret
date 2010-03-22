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
#ifndef _BRAIN_MODEL_SURFACE_DEFORMATION_MULTI_STAGE_SPHERICAL_VECTOR_H
#define	_BRAIN_MODEL_SURFACE_DEFORMATION_MULTI_STAGE_SPHERICAL_VECTOR_H

#include "BrainModelSurfaceDeformation.h"
#include "SurfaceShapeFile.h"

class BrainModelSurface;
class DeformationMapFile;

class BrainModelSurfaceDeformationMultiStageSphericalVector :
                                                 public BrainModelSurfaceDeformation {
   public:
      /// Constructor
      BrainModelSurfaceDeformationMultiStageSphericalVector(BrainSet* brainSetIn,
                                       DeformationMapFile* DeformationMapFileIn);

      /// Destructor
      ~BrainModelSurfaceDeformationMultiStageSphericalVector();

   protected:
      /// Execute the deformation
      void executeDeformation() throw (BrainModelAlgorithmException);

      // Perform landmark constrained morphing on the sphere with source landmarks
      void landmarkMorphContrainedSource(BrainModelSurface* referenceDeformationSphere,
                                         const int stageNumber,
                                         const int cycleIndex) throw (BrainModelAlgorithmException);

      /// Perform landmark neighbor constrained smoothing on the sphere with source landmarks
      void landmarkNeighborConstrainedSmoothSource(const int stageIndex,
                                                   const int cycleNumber);

      /// Get a regularly tessellated sphere and set its radius.
      BrainModelSurface* getRegularSphere(BrainSet* bs, 
                                          const int stageIndex,
                                          const float radius) throw (BrainModelAlgorithmException);

      /// tessellate the target border into the target deformation sphere
      void tessellateTargetBordersIntoDeformationSphere(const int stageIndex) throw (BrainModelAlgorithmException);

      /// tessellate the source border into the source deformation sphere
      void tessellateSourceBordersIntoDeformationSphere(const int stageIndex) throw (BrainModelAlgorithmException);

      /// create the deformed coordinate file at the end of each cycle
      BrainModelSurface* createDeformedCoordinateFile(BrainModelSurface* sourceDeformationSphere,
                                        BrainModelSurface* registeredDeformationSourceSphere,
                                        const int stageIndex,
                                        const int cycleNumber,
                                        const QString& usersSourceCoordFileName);

      /// write a border file containing the deformed landmarks
      BorderFile* writeSourceBorderLandmarkFile(BrainModelSurface* surface,
                                         const int stageIndex,
                                         const int cycleNumber);

      /// write coords without the landmarks
      void writeCoordinatesWithoutLandmarks(BrainModelSurface* surface,
                                            const int stageIndex,
                                            const int cycleNumber);

      /// create the surface shape file containing the XYZ differences
      SurfaceShapeFile* createDifferenceShapeFile(int iterations);

      /// determine the fiducial sphere distortion
      void determineFiducialSphereDistortion();

      /// update the fiducial sphere distortion
      void updateSphereFiducialDistortion(const int stageIndex,
                                    const int cycle,
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

      /// the source surface operated on in this type of deformation
      BrainModelSurface* workingSourceSurface;

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

#endif // _BRAIN_MODEL_SURFACE_DEFORMATION_MULTI_STAGE_SPHERICAL_VECTOR_H


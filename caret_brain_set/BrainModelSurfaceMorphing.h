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


#ifndef __VE_BRAIN_SURFACE_MORPHING_H__
#define __VE_BRAIN_SURFACE_MORPHING_H__

#include <set>
#include <vector>

#include "BrainModelAlgorithmMultiThreaded.h"
#include "BrainModelSurface.h"
#include "BrainSetNodeAttribute.h"

class BrainModelSurface;
class CoordinateFile;
class QFile;
class SurfaceShapeFile;

/// This class morphs a surface
class BrainModelSurfaceMorphing : public BrainModelAlgorithmMultiThreaded {
   public:
      /// type of surface morphing
      enum MORPHING_SURFACE_TYPE {
         MORPHING_SURFACE_FLAT,
         MORPHING_SURFACE_SPHERICAL
      };
      
      //class NeighborInformation;
      
      /// Constructor
      BrainModelSurfaceMorphing(BrainSet* brainSetIn,
                           BrainModelSurface* referenceSurfaceIn,
                           BrainModelSurface* morphingSurfaceIn,
                           const MORPHING_SURFACE_TYPE morphingSurfaceTypeIn,
                           const int numberOfThreasIn = -1);
                           
      /// Destructor
      virtual ~BrainModelSurfaceMorphing();
      
      /// execute the morphing for the number of iterations
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// get morphing parameters
      void getMorphingParameters(int& iterationsOut,
                                 float& linearForceOut,
                                 float& angularForceOut,
                                 float& stepSizeOut) const;
                                 
      /// set morphing parameters
      void setMorphingParameters(const int iterationIn,
                                 const float linearForceIn,
                                 const float angularForceIn,
                                 const float stepSizeIn);
            
      /// set the nodes that should be morphred
      void setNodesThatShouldBeMorphed(const std::vector<bool>& nodesThatShouldBeMorphed,
                                       const float noMorphStepSizeIn);
      
      /// set the fiducial sphere distortion corrections
      void setFiducialSphereDistortionCorrections(const std::vector<float>& fiducialSphereRatiosIn,
                                                  const float sphereFiducialDistortionFractionIn);
      
      /// get do statistics each pass and save to file
      bool getDoStatisticsEachPass() const { return doStatisticsEachPass; }
      
      /// set do statistics each pass and save to file
      void setDoStatisticsEachPass(const bool b) { doStatisticsEachPass = b; }
      
   protected:
      /// This class is used to sort nodes
      class NodeSort {
         public:
            /// the node number
            int nodeNumber;
            
            /// its sort value
            float sortValue;
            
            /// constructor
            NodeSort(const int node, const float value) {
               nodeNumber = node;
               sortValue  = value;
            }
            
            /// less than method
            bool operator<(const NodeSort& ns) const {
               return (sortValue < ns.sortValue);
            }
      };
      
      /// This class contains information about relationships to neighboring nodes
      class NeighborInformation {
         public:
            /// Constructor
            NeighborInformation();
            
            /// Desstructor
            ~NeighborInformation();
                        
            /// initialize the neighbor information
            void initialize(const float* coords, const int nodeNumberIn,
                             const BrainSetNodeAttribute* nodeAttribute,
                             const int* neighborsIn,
                             const int numNeighborsIn);
            
            /// set forces to zero
            void resetForces();
            
            /// the neighbors sorted
            int* neighbors;
            
            /// the distance to each neighbor
            float* neighborDistance;
            
            /// the angle between neighbors
            float* angle1;
            
            /// the angle between neighbors
            float* angle2;
            
            /// total force on the node (3D)
            float totalForce[3];
            
            /// angular force on the node (3D)
            float angularForce[3];
            
            /// linear force on the node (3D)
            float linearForce[3];
            
            /// number of this node
            int nodeNumber;
            
            /// number of neighbors
            int numNeighbors;
            
            /// node classification
            BrainSetNodeAttribute::CLASSIFICATION_TYPE classification;
            
      };
      
      /// Constructor for a thread instance.
      BrainModelSurfaceMorphing(BrainSet* brainSetIn,
                                BrainModelSurface* referenceSurfaceIn,
                                BrainModelSurface* morphingSurfaceIn,
                                const MORPHING_SURFACE_TYPE morphingSurfaceTypeIn,
                                NeighborInformation* morphNodeInfoIn,
                                int* nodeShouldBeMorphedIn,
                                const float noMorphStepSizeIn,
                                const int startNodeIndexIn,
                                const int endNodeIndexIn,
                                const float sphericalSurfaceRadiusIn,
                                BrainModelSurfaceMorphing* parentOfThisThreadIn,
                                const int threadNumberIn);      
                                           
      /// morph for one iteration
      void run();
      
      /// Compute the angular force on a node by its neighbor.
      void computeAngularForce(const float* coords,
                               const NeighborInformation& nodeInfo,
                               const int neighborIndex,
                               float forcesOut[3]);
                                                
      /// compute the linear force on a node
      void computeLinearForce(const float* coords,
                              const NeighborInformation& nodeInfo,
                              const int nodeNum,
                              const int neighNodeNum,
                              const int nodeInfoNeighIndex,
                              float force[3]);
      
      /// generate the neighbor information for a node
      void generateNeighborInformation();
      
      /// initialize variables for this instance
      void initialize();
      
      /// Map forces to a plane (used in spherical morphing).
      void mapForcesToPlane(const float nodeXYZ[3],
                            float force[3]);
      
      /// project node back to the sphere
      void projectNodeBackToSphere(const int nodeNumber);

      /// set the forces on nodes that are NOT being morphed
      void setForcesOnNoMorphNodes();
      
      /// Update the statistics file used for testing
      void updateStatsFile(QFile& statsFile,
                           CoordinateFile* morphCoordFile,
                           SurfaceShapeFile& measurementsShapeFile,
                           BrainModelSurface::SURFACE_TYPES surfaceTypeHint,
                           const int iterationNumber,
                           const bool firstIterationFlag);
                                           
      /// set the indices of the nodes that are to be morphed (inclusive)
      void setIndicesOfNodesToMorph(const int startNodeIndexIn, const int endNodeIndexIn);
      
      /// set the input and output coords
      void setInputAndOutputCoords(float* inCoords, float* outCoords);
      
      /// the reference surface
      BrainModelSurface* referenceSurface;
      
      /// the morphing surface
      BrainModelSurface* morphingSurface;
      
      /// type of surface being morphed
      MORPHING_SURFACE_TYPE morphingSurfaceType;
      
      /// neighboring node information
      NeighborInformation* morphNodeInfo;
      
      /// flat that denotes nodes that should be morphed (int faster than bool)
      int* nodeShouldBeMorphed;
      
      /// ratios of fiducial to spherical surface 
      std::vector<float> fiducialSphereRatios;
      
      /// the distortion fraction
      float sphereFiducialDistortionFraction;
      
      /// linear force
      float linearForce;
      
      /// angular force
      float angularForce;
      
      /// step size
      float stepSize;
      
      /// step size for inverse of forces when neighbors are not morphred
      float noMorphNeighborStepSize;
      
      /// number of iterations to morph
      int iterations;
      
      /// array for holding coordinates (inputCoords and outputCoords point to this)
      float* coordsArray1;
      
      /// array for holding coordinates (inputCoords and outputCoords point to this)
      float* coordsArray2;
      
      /// pointers for input coordinates (do not delete)
      float* inputCoords;
      
      /// pointers for output coordinates (do not delete)
      float* outputCoords;
      
      /// number of nodes in the surfaces
      int numberOfNodes;

      /// radius of the spherical surface
      float sphericalSurfaceRadius;
      
      /// index of first node to smooth
      int startNodeIndex;
      
      /// index of last node to smooth
      int endNodeIndex;
      
      /// do statistics each pass and save to file
      bool doStatisticsEachPass;
      
      /// all nodes being morphed flag
      bool allNodesBeingMorphed;

      /// threads when running multi-threaded.
      std::vector<BrainModelSurfaceMorphing*> threads;
      
};
#endif // __VE_BRAIN_SURFACE_MORPHING_H__


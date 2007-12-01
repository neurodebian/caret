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

#include <algorithm>
#include <cmath>

#include "vtkMath.h"

#include "BrainModelSurface.h"
#include "BrainModelSurfacePointLocator.h"
#include "BrainModelSurfaceSulcalDepthWithNormals.h"
#include "BrainSet.h"
#include "MathUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelSurfaceSulcalDepthWithNormals::BrainModelSurfaceSulcalDepthWithNormals(
                                            BrainSet* bs,
                                            BrainModelSurface* surfaceIn,
                                            const QString&  hullVtkFileNameIn,
                                            SurfaceShapeFile* shapeFileIn,
                                            const int hullSmoothingIterationsIn,
                                            const int depthSmoothingIterationsIn,
                                            const int depthColumnIn,
                                            const int smoothedDepthColumnIn,
                                            const QString& depthNameIn,
                                            const QString& smoothedDepthNameIn,
                                            CoordinateFile* outputHullCoordFileIn)
   : BrainModelAlgorithm(bs)
{
   surface                  = surfaceIn;
   shapeFile                = shapeFileIn;
   depthColumn              = depthColumnIn;
   smoothedDepthColumn      = smoothedDepthColumnIn;
   hullSmoothingIterations  = hullSmoothingIterationsIn;
   depthSmoothingIterations = depthSmoothingIterationsIn;
   hullVtkFileName = hullVtkFileNameIn;
   depthName = depthNameIn;
   smoothedDepthName = smoothedDepthNameIn;
   outputHullCoordFile = outputHullCoordFileIn;
}

/**
 * Destructor.
 */
BrainModelSurfaceSulcalDepthWithNormals::~BrainModelSurfaceSulcalDepthWithNormals()
{
}

/**
 * execute the algorithm
 */
void 
BrainModelSurfaceSulcalDepthWithNormals::execute() throw (BrainModelAlgorithmException)
{
   //std::cout << "Running David/Donna sulcal depth" << std::endl;

   //
   // check surface
   //
   if (surface == NULL) {
      throw BrainModelAlgorithmException("Surface is invalid.");
   }
   if (surface->getNumberOfNodes() <= 0) {
      throw BrainModelAlgorithmException("Surface has no nodes.");
   }

   //
   // create the inflated surface
   //
   inflatedSurface = new BrainModelSurface(*surface);
   inflatedSurface->inflateSurfaceAndSmoothFingers(surface,
                                                   6,
                                                   1.0,
                                                   200,
                                                   1.1,
                                                   1.0,
                                                   1.0,
                                                   0,
                                                   NULL);
   bool needToDeleteInflatedSurface = true;
   if (DebugControl::getDebugOn()) {
      CoordinateFile* cf = inflatedSurface->getCoordinateFile();
      if (cf != NULL) {
         cf->makeDefaultFileName("DebugSulcalDepthHyperInflated");
         inflatedSurface->setSurfaceType(BrainModelSurface::SURFACE_TYPE_VERY_INFLATED);
         brainSet->addBrainModel(inflatedSurface);
         needToDeleteInflatedSurface = false;
      }
   }

   //
   // Create surface shape columns if needed
   //
   if (depthColumn == DEPTH_COLUMN_CREATE_NEW) {
      if (shapeFile->getNumberOfColumns() == 0) {
         shapeFile->setNumberOfNodesAndColumns(surface->getNumberOfNodes(), 1);
      }
      else {
         shapeFile->addColumns(1);
      }
      depthColumn = shapeFile->getNumberOfColumns() - 1;
   }
   if (smoothedDepthColumn == DEPTH_COLUMN_CREATE_NEW) {
      if (shapeFile->getNumberOfColumns() == 0) {
         shapeFile->setNumberOfNodesAndColumns(surface->getNumberOfNodes(), 1);
      }
      else {
         shapeFile->addColumns(1);
      }
      smoothedDepthColumn = shapeFile->getNumberOfColumns() - 1;
   }

   //
   // Set column names
   //
   if (depthColumn >= 0) {
      shapeFile->setColumnName(depthColumn, depthName);
      shapeFile->setColumnColorMappingMinMax(depthColumn, -30.0, 10.0);
   }
   if (smoothedDepthColumn >= 0) {
      shapeFile->setColumnName(smoothedDepthColumn, smoothedDepthName);
      shapeFile->setColumnColorMappingMinMax(smoothedDepthColumn, -30.0, 10.0);
   }
   
   //
   // Convert the hull to a brain model surface
   //
   BrainSet hullBrainSet;
   try {
      hullBrainSet.importVtkSurfaceFile(hullVtkFileName, true, true, false,
                                        BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
                                        TopologyFile::TOPOLOGY_TYPE_CLOSED);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }

   //
   // Hull should be first brain model
   //
   BrainModelSurface* hullSurface = hullBrainSet.getBrainModelSurface(0);
   if (hullSurface == NULL) {
      throw BrainModelAlgorithmException("Hull surface is missing.");
   }

   TopologyFile* hullTopologyFile = hullSurface->getTopologyFile();
   const int numIslands = hullTopologyFile->disconnectIslands();
   if (numIslands > 0) {
            hullSurface->moveDisconnectedNodesToOrigin();
   }

   //
   // Save hull prior to smoothing
   //
   const BrainModelSurface* unsmoothedHullSurface = new BrainModelSurface(*hullSurface);
   const CoordinateFile* unsmoothedHullCoords = unsmoothedHullSurface->getCoordinateFile();

   //
   // Create the inflated hull surface
   //
   inflatedHullSurface = new BrainModelSurface(*unsmoothedHullSurface);
   hullBrainSet.addBrainModel(inflatedHullSurface);
   lowlySmoothedHullSurface = new BrainModelSurface(*unsmoothedHullSurface);
   hullBrainSet.addBrainModel(lowlySmoothedHullSurface);
   //
   // First, just smooth a little, and save normal for distance to plane check
   //
   lowlySmoothedHullSurface->inflateSurfaceAndSmoothFingers(unsmoothedHullSurface,
                                                       1,
                                                       1.0,
                                                       30,
                                                       1.0,
                                                       1.0,
                                                       1.0,
                                                       0,
                                                       NULL);
   //
   // Now, smooth more for "normalcheck" (i.e., part that keeps sylvian
   // from pairing with medial wall hull nodes)
   //
   inflatedHullSurface->inflateSurfaceAndSmoothFingers(unsmoothedHullSurface,
                                                       6,
                                                       1.0,
                                                       50,
                                                       1.1,
                                                       3.0,
                                                       1.0,
                                                       60,
                                                       NULL);
   //
   // If desired, smooth hull
   //
   if (hullSmoothingIterations > 0) {
      //
      // Smooth the hull
      //
      hullSurface->arealSmoothing(1.0, hullSmoothingIterations, 0);
   }

   //
   // Make sure hull's normals point outward
   //
   hullSurface->orientNormalsOut();


   //
   // determine greatest distance to each neighbor of each hull node
   //
   std::vector<float> hullGreatestNeighborDistance;
   determineGreatestNeighborDistance(hullSurface, hullGreatestNeighborDistance);

   //
   // Create a point locator for the hull surface
   //
   BrainModelSurfacePointLocator hullLocator(hullSurface,
                                             true);


   //
   // Make sure inflated hull's normals point outward
   //
   inflatedHullSurface->computeNormals();
   inflatedHullSurface->orientNormalsOut();
   lowlySmoothedHullSurface->computeNormals();
   lowlySmoothedHullSurface->orientNormalsOut();

   //
   // Get surface and hull coordinates
   //
   const CoordinateFile* surfaceCoords = surface->getCoordinateFile();
   const int numSurfaceNodes = surfaceCoords->getNumberOfCoordinates();
   const CoordinateFile* hullCoords = hullSurface->getCoordinateFile();

   //
   // TopologyHelper for surface
   //
   const TopologyFile* surfaceTopologyFile = surface->getTopologyFile();
   const TopologyHelper* surfaceTopologyHelper = 
                         surfaceTopologyFile->getTopologyHelper(false, true, false);

   //
   // TopologyHelper for hull
   //
   const TopologyHelper* hullTopologyHelper = hullTopologyFile->getTopologyHelper(false, true, false);

   //
   // initialize hull coord file
   //
   if ((numSurfaceNodes > 0) &&
       (outputHullCoordFile != NULL)) {
      outputHullCoordFile->setNumberOfCoordinates(numSurfaceNodes);
   }

   //
   // Determine depth values for surface nodes
   //
   int numHullNodes = hullSurface->getNumberOfNodes();
   for (int i = 0; i < numSurfaceNodes; i++) {
      //
      // initially set the depth to zero
      //
      if (depthColumn >= 0) {
         shapeFile->setValue(i, depthColumn, 0.0);
      }
      if (smoothedDepthColumn >= 0) {
         shapeFile->setValue(i, smoothedDepthColumn, 0.0);
      }

      //
      // Make sure surface node has neighbors
      //
      if (surfaceTopologyHelper->getNodeHasNeighbors(i) == false) {
         continue;
      }

      //
      // coordinate of surface node
      //
      const float* surfaceXYZ = surfaceCoords->getCoordinate(i);

      //
      // Find node in hull that is nearest this surface node
      //
      int hullNode = hullLocator.getNearestPoint(surfaceXYZ);

      //
      // If nearest node found in hull
      //
      if (hullNode >= 0) {
         //
         // Do the normals fail to match in orientation ?
         //
         if (normalCheck(i, hullNode) == false) {
            float nearestHullNodeDistance = std::numeric_limits<float>::max();
            hullNode = -1;

            //
            // Find closest hull node with similar normal orientation
            //
            for (int m = 0; m < numHullNodes; m++) {
               //
               // Does hull node have neighbors and how far away is it
               //
               if (hullTopologyHelper->getNodeHasNeighbors(m) ) {
                  float distSQ = hullCoords->getDistanceToPointSquared(m, surfaceXYZ);
                  if (distSQ < nearestHullNodeDistance) {

                     //
                     // Are normals oriented in a similar direction ?
                     //
                     if (normalCheck(i, m) > 0.0) {
                        //
                        // Could be nearest node
                        //
                        hullNode = m;
                        nearestHullNodeDistance = distSQ;
                     }
                  }
               }
            }
         }
      }

      //
      // If nearest node found in hull
      //
      if (hullNode >= 0) {
         //
         // Get the coordinate of the hull nearest node
         //
         const float* hullXYZ = hullCoords->getCoordinate(hullNode);

         //
         // Get normal of hull node
         //
         const float* hullNormal = hullSurface->getNormal(hullNode);
         const float* inflatedHullNormal = inflatedHullSurface->getNormal(hullNode);
         const float* lowSmoothNormal = lowlySmoothedHullSurface->getNormal(hullNode);

         // 
         // Compute signed distance from surface not to point on plane from hull normal 
         //
         const float distanceFromPlane = 
               MathUtilities::signedDistanceToPlane(lowSmoothNormal,
                                                    hullXYZ,
                                                    surfaceXYZ);

         //
         // Distance between surface and hull point
         //
         float distance = MathUtilities::distance3D(hullXYZ, surfaceXYZ);

         //
         // Is surface node closer than hull node's nearest neighbor?
         //
         if (distance < hullGreatestNeighborDistance[hullNode]) {
            const float dxyz[3] = {
               surfaceXYZ[0] - hullXYZ[0],
               surfaceXYZ[1] - hullXYZ[1],
               surfaceXYZ[2] - hullXYZ[2]
            };

            //
            // determine distance
            //
            distance = MathUtilities::dotProduct(dxyz, hullNormal);
         }
         else {
            //
            // "distance" is an absolute value
            // So, apply the sign of "distanceFromPlane"
            //
            if (distanceFromPlane < 0.0) {
               distance = -distance;
            }
         }

         if (DebugControl::getDebugOn()) {
            if (i == DebugControl::getDebugNodeNumber()) {
               const float* sn = inflatedSurface->getNormal(i);
               std::cout << "surface node:      " << i << std::endl;
               std::cout << "   hull node:      " << hullNode << std::endl;
               std::cout << "   surface node:   " << surfaceXYZ[0] << ", " << surfaceXYZ[1] << ", "
                                                << surfaceXYZ[2] << std::endl;
               std::cout << "   dot:            " << MathUtilities::dotProduct(sn, inflatedHullNormal) << std::endl;
               std::cout << "   inflated surface normal: " << sn[0] << ", " << sn[1] << ", "
                                                << sn[2] << std::endl;
               std::cout << "   hull node:      " << hullXYZ[0] << ", " << hullXYZ[1] << ", "
                                                << hullXYZ[2] << std::endl;
               std::cout << "   hull normal:    " << hullNormal[0] << ", " << hullNormal[1] << ", "
                                                << hullNormal[2] << std::endl;
               std::cout << "   inflated hull normal:    " << inflatedHullNormal[0] << ", " << inflatedHullNormal[1] << ", "
                                                << inflatedHullNormal[2] << std::endl;
               std::cout << "   lowly smoothed hull normal:    " << lowSmoothNormal[0] << ", " << lowSmoothNormal[1] << ", "
                                                << lowSmoothNormal[2] << std::endl;
               std::cout << "   distance:       " << distance << std::endl;
               std::cout << "   distance/plane: " << distanceFromPlane << std::endl; 
               std::cout << "   hull neigh dist:" << hullGreatestNeighborDistance[hullNode] << std::endl;
            }
         }

         //
         // Insert into the surface shape file
         //
         if (depthColumn >= 0) {
            shapeFile->setValue(i, depthColumn, distance);
         }
         if (smoothedDepthColumn >= 0) {
            shapeFile->setValue(i, smoothedDepthColumn, distance);
         }

         //
         // output hull coord file
         //
         if (outputHullCoordFile != NULL) {
            outputHullCoordFile->setCoordinate(i, 
                                unsmoothedHullCoords->getCoordinate(hullNode));
         }
      }
      else {
         //
         // output hull coord file
         //
         if (outputHullCoordFile != NULL) {
            outputHullCoordFile->setCoordinate(i, 0.0, 0.0, 0.0);
         }
      }
   }

   //
   // if creating smoothed depth, smooth it
   //
   if ((smoothedDepthColumn >= 0) && (depthSmoothingIterations > 0)) {
      shapeFile->smoothAverageNeighbors(smoothedDepthColumn,
                                        smoothedDepthColumn,
                                        shapeFile->getColumnName(smoothedDepthColumn),
                                        1.0, 
                                        depthSmoothingIterations,
                                        surface->getTopologyFile());
   }

   //
   // Free memory
   //
   delete unsmoothedHullSurface;
   unsmoothedHullSurface = NULL;
   if (needToDeleteInflatedSurface) {
      delete inflatedSurface;
      inflatedSurface = NULL;
   }
}

/**
 * normal check for node (surface/hull normals oriented in similar direction).
 */
bool 
BrainModelSurfaceSulcalDepthWithNormals::normalCheck(const int surfaceNode, const int hullNode)
{
   //
   // Get the inflated hull surface normal
   //
   const float* inflatedHullNormal = inflatedHullSurface->getNormal(hullNode);

   //
   // Get the inflated surface normal
   //
   const float* inflatedSurfaceNormal = inflatedSurface->getNormal(surfaceNode);

   if (DebugControl::getDebugOn()) {
   }

   //
   // Get dot product which is angles between vectors
   //
   const float dot = vtkMath::Dot(inflatedHullNormal, inflatedSurfaceNormal);

   if (DebugControl::getDebugOn()) {
      if (surfaceNode == DebugControl::getDebugNodeNumber()) {
         std::cout << "Sulcal Depth node: " << surfaceNode
                   << " hull node: " << hullNode
                   << " dot: " << dot << std::endl;
      }
   }

   //
   // In similar direction
   //
   if (dot > 0.0) {
      return true;
   }

   return false;
}      

/**
 * determine greatest neighbor distance from each node.
 */
void 
BrainModelSurfaceSulcalDepthWithNormals::determineGreatestNeighborDistance(
                                       const BrainModelSurface* bms,
                                       std::vector<float>& greatestDistance) const
{
   //
   // Initialize output distances
   //
   const int numNodes = bms->getNumberOfNodes();
   greatestDistance.resize(numNodes);
   std::fill(greatestDistance.begin(), greatestDistance.end(), 0.0);

   //
   // Get a topology helper
   //
   const CoordinateFile* cf = bms->getCoordinateFile();
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf != NULL) {
      const TopologyHelper* th = tf->getTopologyHelper(false, true, false);

      //
      // Determine distance for each node
      //
      for (int i = 0; i < numNodes; i++) {
         float maxDist = 0.0;
         int numNeighbors = 0;
         const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
         for (int j = 0; j < numNeighbors; j++) {
            const float d = cf->getDistanceBetweenCoordinatesSquared(i, neighbors[j]);
            maxDist = std::max(maxDist, d);
         }
         greatestDistance[i] = sqrt(maxDist);
      }
   }
}

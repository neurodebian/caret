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

#include "BrainModelSurface.h"
#include "BrainModelSurfacePointLocator.h"
#include "BrainModelSurfaceSulcalDepth.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "MathUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelSurfaceSulcalDepth::BrainModelSurfaceSulcalDepth(
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
BrainModelSurfaceSulcalDepth::~BrainModelSurfaceSulcalDepth()
{
}

/**
 * execute the algorithm
 */
void 
BrainModelSurfaceSulcalDepth::execute() throw (BrainModelAlgorithmException)
{
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

   //
   // Save hull prior to smoothing
   //
   const BrainModelSurface* unsmoothedHullSurface = new BrainModelSurface(*hullSurface);
   const CoordinateFile* unsmoothedHullCoords = unsmoothedHullSurface->getCoordinateFile();
   
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
   // Create a point locator for the hull surface
   //
   BrainModelSurfacePointLocator hullLocator(hullSurface,
                                             true);
                                             
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
   // initialize hull coord file
   //
   if ((numSurfaceNodes > 0) &&
       (outputHullCoordFile != NULL)) {
      outputHullCoordFile->setNumberOfCoordinates(numSurfaceNodes);
   }
   
   //
   // Determine depth values for surface nodes
   //
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
      const int nearestNode = hullLocator.getNearestPoint(surfaceXYZ);
      
      //
      // If nearest node found in hull
      //
      if (nearestNode >= 0) {
         //
         // Get the coordinate of the hull nearest node
         //
         const float* hullXYZ = hullCoords->getCoordinate(nearestNode);
         
         //
         // Get normal of hull node
         //
         const float* hullNormal = hullSurface->getNormal(nearestNode);
         
         // 
         // Compute signed distance from surface not to point on plane from hull normal 
         //
         const float distanceFromPlane = 
               MathUtilities::signedDistanceToPlane(hullNormal,
                                                   hullXYZ,
                                                   surfaceXYZ);
                                                   
         //
         // Distance between surface and hull point
         //
         float distance = MathUtilities::distance3D(hullXYZ, surfaceXYZ);
         
         //
         // Set the sign for inside or outside the hull
         //
         if (distanceFromPlane < 0.0) {
            distance = -distance;
         }
         
         if (DebugControl::getDebugOn()) {
            if (i == DebugControl::getDebugNodeNumber()) {
               const float* sn = surface->getNormal(i);
               std::cout << "surface node:      " << i << std::endl;
               std::cout << "   hull node:      " << nearestNode << std::endl;
               std::cout << "   surface node:   " << surfaceXYZ[0] << ", " << surfaceXYZ[1] << ", "
                                                << surfaceXYZ[2] << std::endl;
               std::cout << "   dot:            " << MathUtilities::dotProduct(sn, hullNormal) << std::endl;
               std::cout << "   surface normal: " << sn[0] << ", " << sn[1] << ", "
                                                << sn[2] << std::endl;
               std::cout << "   hull node:      " << hullXYZ[0] << ", " << hullXYZ[1] << ", "
                                                << hullXYZ[2] << std::endl;
               std::cout << "   hull normal:    " << hullNormal[0] << ", " << hullNormal[1] << ", "
                                                << hullNormal[2] << std::endl;
               std::cout << "   distance:       " << distance << std::endl;
               std::cout << "   distance/plane: " << distanceFromPlane << std::endl; 
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
                                unsmoothedHullCoords->getCoordinate(nearestNode));
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
}
      

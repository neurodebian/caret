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

#include <QGlobalStatic>  // needed for Q_OS_WIN32
#ifdef Q_OS_WIN32     // required for M_PI in <cmath>
#define NOMINMAX
#endif

#include <cmath>
#include <iostream>
#include <limits>

#include "BrainModelSurface.h"
#include "BrainModelSurfacePointLocator.h"
#include "BrainModelSurfacePointProjector.h"
#include "MathUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkTriangle.h"

/**
 * Constructor.
 * Set "surfaceMayHaveNodesAddedToIt" if the surface will have nodes added to it 
 * after constructing this object.
 */
BrainModelSurfacePointProjector::BrainModelSurfacePointProjector(
                                          const BrainModelSurface* bmsIn,
                                          const SURFACE_TYPE_HINT surfaceTypeHintIn,
                                          const bool surfaceMayHaveNodesAddedToIt)
   : coordinateFile(bmsIn->getCoordinateFile()),  // initialize const members
     topologyFile(bmsIn->getTopologyFile())
{
   surfaceTypeHint = surfaceTypeHintIn;
   
   //
   // Create a point locator for connected nodes
   //
   pointLocator = new BrainModelSurfacePointLocator(bmsIn, true, surfaceMayHaveNodesAddedToIt);
   
   nearestNodeToleranceSquared = 0.01 * 0.01;
   tileAreaTolerance    = -0.01;

   switch (surfaceTypeHint) {
      case SURFACE_TYPE_HINT_FLAT:   
      case SURFACE_TYPE_HINT_SPHERE:
         break;   
      case SURFACE_TYPE_HINT_OTHER:
         {
            //
            // see if coordinate file is flat to speed up queries
            //
            const int numCoords = coordinateFile->getNumberOfCoordinates();
            bool isFlat = true;
            for (int i = 0; i < numCoords; i++) {
               const float* xyz = coordinateFile->getCoordinate(i);
               if (xyz[2] != 0.0) {
                  isFlat = false;
                  break;
               }
            }
            if (isFlat) {
               surfaceTypeHint = SURFACE_TYPE_HINT_FLAT;
            }
         }
   }
}

/**
 * Destructor.
 */
BrainModelSurfacePointProjector::~BrainModelSurfacePointProjector()
{
   if (pointLocator != NULL) delete pointLocator;
   pointLocator = NULL;
}

/**
 * Project to nearest node.  Returns nearest node number of negative if surface
 * had no nodes.
 */
int
BrainModelSurfacePointProjector::projectToNearestNode(const float xyz[3])
{
   return pointLocator->getNearestPoint(xyz);
}

/**
 * Project to the nearest tile.  First, a barycentric projection is performed.  If the query
 * point projects into a tile, a positive number is returned.  If the query point does not project
 * into a tile, the tile nearest to the query point will be determined and a negative number
 * is returned.  If there are no tiles, zero is returned. 
 */
int
BrainModelSurfacePointProjector::projectBarycentricNearestTile(const float xyz[3], 
                                                    int& nearestTileNumberOut,
                                                    int tileNodesOut[3], float barycentricOut[3],
                                                    float& signedDistanceOut,
                                                    float& distanceToTileOut,
                                                    float distanceToTileComponentsOut[3])
{
   int nearestNodeNumber = -1;
   nearestTileNumberOut  = -1;

   //
   // Try a normal barycentric mode projection first
   //
   nearestTileNumberOut = projectBarycentric(xyz, nearestNodeNumber, tileNodesOut, barycentricOut, true);
   
   //
   // Did query point project successfully to a tile
   //
   if (nearestTileNumberOut >= 0) {
      //
      // Coordinates of tile
      //
      const float* p1 = coordinateFile->getCoordinate(tileNodesOut[0]);
      const float* p2 = coordinateFile->getCoordinate(tileNodesOut[1]);
      const float* p3 = coordinateFile->getCoordinate(tileNodesOut[2]);
      
      //
      // Tile's normal
      //
      float normal[3];
      MathUtilities::computeNormal((float*)p1, (float*)p2, (float*)p3, normal);

/*      
      //
      // Project query point onto tile's plane
      //
      float projectedOntoTile[3];
      vtkPlane::ProjectPoint((float*)xyz, (float*)p1, normal, projectedOntoTile);
      
      //
      // Distance from query point to tile
      //
      distanceToTileOut = vtkPlane::DistanceToPlane((float*)xyz, normal, projectedOntoTile);
      const float dx = normal[0] * (xyz[0] - projectedOntoTile[0]);
      const float dy = normal[1] * (xyz[1] - projectedOntoTile[1]);
      const float dz = normal[2] * (xyz[2] - projectedOntoTile[2]);
      const float signedDistance = dx + dy + dz;
      std::cout << "distance: " << distanceToTileOut << " signed: " << signedDistance << std::endl;
*/      
      //
      // get signed distance to plane from query point
      //
      signedDistanceOut = MathUtilities::signedDistanceToPlane(normal, p1, xyz);
      distanceToTileOut = fabs(signedDistanceOut);
      
      //
      // Determine distance and components from query point to tile
      //
      distanceToTileComponentsOut[0] = normal[0] * signedDistanceOut; 
      distanceToTileComponentsOut[1] = normal[1] * signedDistanceOut; 
      distanceToTileComponentsOut[2] = normal[2] * signedDistanceOut; 
      
      return 1;
   }
   else {
      //
      // Look through searched tiles to find the one closest to the query point
      //
      for (std::set<int>::iterator iter = barycentricSearchedTiles.begin(); 
           iter != barycentricSearchedTiles.end(); iter++) {
         const int tileNumber = *iter;
         
         //
         // Determine components magnitude of distance and from query point to tile
         //
         int v1, v2, v3;
         topologyFile->getTile(tileNumber, v1, v2, v3);
         const float* p1 = coordinateFile->getCoordinate(v1);
         const float* p2 = coordinateFile->getCoordinate(v2);
         const float* p3 = coordinateFile->getCoordinate(v3);
         
         //
         // Tile's normal
         //
         float normal[3];
         MathUtilities::computeNormal((float*)p1, (float*)p2, (float*)p3, normal);
/*         
         //
         // Project query point onto tile's plane
         //
         float projectedOntoTile[3];
         vtkPlane::ProjectPoint((float*)xyz, (float*)p1, normal, projectedOntoTile);
         
         //
         // Distance from query point to tile
         //
         const float distance = vtkPlane::DistanceToPlane((float*)xyz, normal, projectedOntoTile);
*/      
         //
         // get signed distance to plane from query point
         //
         signedDistanceOut = MathUtilities::signedDistanceToPlane(normal, p1, xyz);
         const float distance = fabs(signedDistanceOut);
         
         //
         // Determine distance and components from query point to tile
         //
         float distComponents[3];
         distComponents[0] = normal[0] * signedDistanceOut; 
         distComponents[1] = normal[1] * signedDistanceOut; 
         distComponents[2] = normal[2] * signedDistanceOut; 
      
         if ((nearestTileNumberOut < 0) || (distance < distanceToTileOut)) {
            nearestTileNumberOut = tileNumber;
            distanceToTileOut = distance;
            distanceToTileComponentsOut[0] = distComponents[0];
            distanceToTileComponentsOut[1] = distComponents[1];
            distanceToTileComponentsOut[2] = distComponents[2];
         }
      }
      
      if (nearestTileNumberOut >= 0) {
         return -1;
      }
   }
   
   //
   // Should never get here unless there are no tiles
   //
   return 0;
}

/**
 * Project barycentric to "best" tile (must have passed barycentricMode = true to constructor).
 * First, finds the nearest node to the query point.  Next, for each tile used by the nearest
 * node, the total distances to the three nodes of the tile are determined.  The tile with the 
 * smallest of these distances is the "best" tile.  This is used by flat multiresolution 
 * morphing and this method may only be appropriate for flat surfaces.  Returns negative
 * if there are no tiles or greater than or equal to zero upon success.
 * 
 * This is essentially "interpolate_surface" from caret4.
 */
int
BrainModelSurfacePointProjector::projectBarycentricBestTile2D(const float xyz[3], 
                                                              int& nearestTileNumberOut,
                                                              int tileNodesOut[3], 
                                                              float barycentricOut[3])
{
   nearestTileNumberOut = -1;
   tileNodesOut[0] = -1;
   tileNodesOut[1] = -1;
   tileNodesOut[2] = -1;
   barycentricOut[0] = 0.0;
   barycentricOut[1] = 0.0;
   barycentricOut[2] = 0.0;
   
   //
   // generate topology info for node without sorting.
   //
   const TopologyHelper* topologyHelper = topologyFile->getTopologyHelper(false, true, false);
      
   //
   // Find node closest to the point
   //
   const int nearestNodeNumberOut = pointLocator->getNearestPoint(xyz);
   
   if (nearestNodeNumberOut >= 0) {
      //
      // Find tiles used by this node
      //
      std::vector<int> nodesTiles;
      topologyHelper->getNodeTiles(nearestNodeNumberOut, nodesTiles);

      nearestTileNumberOut = -1;
      float closestDistance = std::numeric_limits<float>::max();
            
      //
      // Check each tile
      //
      for (int tileIndex = 0; tileIndex < static_cast<int>(nodesTiles.size()); tileIndex++) {
         //
         // Get the nodes of the tile
         //
         const int tile = nodesTiles[tileIndex];
         int n1, n2, n3;
         topologyFile->getTile(tile, n1, n2, n3);
         
         //
         // Distance squared to each of the tile's nodes
         //
         const float d1 = coordinateFile->getDistanceToPointSquared(n1, xyz);
         const float d2 = coordinateFile->getDistanceToPointSquared(n2, xyz);
         const float d3 = coordinateFile->getDistanceToPointSquared(n3, xyz);
         
         //
         // "Mean" distance to the three nodes
         //
         const float dist = std::sqrt(d1*d1 + d2*d2 + d3*d3);
         
         //
         // Check to see if this tile is "better"
         //
         if ((nearestTileNumberOut < 0) || (dist < closestDistance)) {
            nearestTileNumberOut = tile;
            closestDistance = dist;
         }
      }
      
      //
      // If a tile was found
      //
      if (nearestTileNumberOut >= 0) {
         //
         // Get the coordinate of the nodes of the best tile
         //
         topologyFile->getTile(nearestTileNumberOut, tileNodesOut);
         const float* p1 = coordinateFile->getCoordinate(tileNodesOut[0]);
         const float* p2 = coordinateFile->getCoordinate(tileNodesOut[1]);
         const float* p3 = coordinateFile->getCoordinate(tileNodesOut[2]);
         
         //
         // Determine barycentric areas
         //
         barycentricOut[0] = (MathUtilities::signedTriangleArea2D(p1, xyz, p2));
         barycentricOut[1] = (MathUtilities::signedTriangleArea2D(p2, xyz, p3));
         barycentricOut[2] = (MathUtilities::signedTriangleArea2D(p3, xyz, p1));
         
         return nearestNodeNumberOut;
      }
   }
   
   //
   // Tile not found
   //
   return -1;
}

/**
 * Barycentric projection to tile (must have passed barycentricMode = true to constructor).
 * Returns the index of the tile the points projects to or negative if the point does not
 * project to a tile.
 */
int
BrainModelSurfacePointProjector::projectBarycentric(const float xyz[3], int& nearestNodeNumberOut,
                                                    int tileNodesOut[3], float barycentricOut[3],
                                                    const bool checkNeighbors)
{
   //
   // generate topology info for node without sorting.
   //
   const TopologyHelper* topologyHelper = topologyFile->getTopologyHelper(false, true, false);
      
   barycentricQueryPoint[0] = xyz[0];
   barycentricQueryPoint[1] = xyz[1];
   barycentricQueryPoint[2] = xyz[2];
   barycentricTile = -1;
   barycentricNodes[0] = -1;
   barycentricNodes[1] = -1;
   barycentricNodes[2] = -1;
   barycentricAreas[0] = 0.0;
   barycentricAreas[1] = 0.0;
   barycentricAreas[2] = 0.0;
   
   //
   // reset tiles that have been searched
   //
   barycentricSearchedTiles.clear();
   
   //
   // Find node closest to the point
   //
   nearestNodeNumberOut = pointLocator->getNearestPoint(xyz);
   
   //
   // Reset the search status
   //
   barycentricSearchStatus = TILE_NOT_FOUND;
   
   //
   // Search the tiles used by the nearest node first.
   //
   checkPointInNodesTiles(topologyHelper, nearestNodeNumberOut);
      
   //
   // If projection point not found in tiles using node closest to query point or
   // found degenerately, keep searching
   //
   if ((barycentricSearchStatus != TILE_FOUND) && (checkNeighbors)) {
      if (barycentricSearchStatus == TILE_FOUND_DEGENERATE) {
         //std::cout << "Query point degenerately in nearest node's neighboring tiles." << std::endl;
      }
      else {
         //std::cout << "Query point not found in nearest node's neighboring tiles." << std::endl;
      }
      
      //
      // Check neighboring nodes of node nearest to query point
      //
      std::vector<int> neighbors;
      topologyHelper->getNodeNeighbors(nearestNodeNumberOut, neighbors);
      const int numNeighbors = static_cast<int>(neighbors.size());
      for (int i = 0; i < numNeighbors; i++) {
         checkPointInNodesTiles(topologyHelper, neighbors[i]);
         if (barycentricSearchStatus == TILE_FOUND) {
            break;
         }
      }
   }
   
   //
   // Might be "on" the nearest node
   //
   if (barycentricSearchStatus == TILE_NOT_FOUND) {
      if (vtkMath::Distance2BetweenPoints(barycentricQueryPoint, 
                                          (float*)coordinateFile->getCoordinate(nearestNodeNumberOut)) 
                              <= nearestNodeToleranceSquared) {
         barycentricSearchStatus = TILE_FOUND;
         barycentricNodes[0] = nearestNodeNumberOut;
         barycentricNodes[1] = nearestNodeNumberOut;
         barycentricNodes[2] = nearestNodeNumberOut;
         barycentricAreas[0] = 0.0;
         barycentricAreas[1] = 1.0;
         barycentricAreas[2] = 0.0;
         std::cout << "Point is on nearest node." << std::endl;
      }
   }
   
   //
   // Should not need to search remaining tiles
   //
   if (barycentricSearchStatus == TILE_NOT_FOUND) {
      //std::cout << "IMPLEMENT: Query point not found in neighboring nodes' tiles." << std::endl;
      
      //
      // Find nodes that are within X distance of the node
      //
      
      //
      // Search all remaining tiles
      //
   }
   
   tileNodesOut[0] = barycentricNodes[0];
   tileNodesOut[1] = barycentricNodes[1];
   tileNodesOut[2] = barycentricNodes[2];
   barycentricOut[0] = barycentricAreas[0];
   barycentricOut[1] = barycentricAreas[1];
   barycentricOut[2] = barycentricAreas[2];
   
   return barycentricTile;
}

/**
 * See if a point is in the tiles used by this node.
 */
void
BrainModelSurfacePointProjector::checkPointInNodesTiles(const TopologyHelper* topologyHelper, 
                                                        const int nodeNumber)
{
   //
   // Get the tiles used by the closest node
   //
   std::vector<int> tiles;
   topologyHelper->getNodeTiles(nodeNumber, tiles);
   const int numTiles = static_cast<int>(tiles.size());
   
   for (int i = 0; i < numTiles; i++) {
      checkPointInTile(tiles[i]);
      
      //
      // If all areas positive, terminate search.
      //
      if (barycentricSearchStatus == TILE_FOUND) {
         break;
      }
   }
}

/**
 * See if the point "xyz" is within tile "tileNumber".
 */
void
BrainModelSurfacePointProjector::checkPointInTile(const int tileNumber)
{
   //
   // See if this tile has been searched before.
   //
   if (barycentricSearchedTiles.find(tileNumber) != barycentricSearchedTiles.end()) {
      return;
   }
   barycentricSearchedTiles.insert(tileNumber);
   
   //
   // Get the vertices of the triangle
   //
   int v1, v2, v3;
   topologyFile->getTile(tileNumber, v1, v2, v3);
   const float* p1 = coordinateFile->getCoordinate(v1);
   const float* p2 = coordinateFile->getCoordinate(v2);
   const float* p3 = coordinateFile->getCoordinate(v3);

   float normal[3];
   float queryPoint[3];
   
   switch (surfaceTypeHint) {
      case SURFACE_TYPE_HINT_FLAT:
         normal[0] = 0.0;
         normal[1] = 0.0;
         normal[2] = 1.0;
         queryPoint[0] = barycentricQueryPoint[0];
         queryPoint[1] = barycentricQueryPoint[1];
         queryPoint[2] = barycentricQueryPoint[2];
         break;
      case SURFACE_TYPE_HINT_SPHERE:
         {
            //
            // See if a ray from the origin to the query point intersects
            // the plane of the triangle, and, if so, where
            //
            float origin[3] = { 0.0, 0.0, 0.0 };
            if (MathUtilities::rayIntersectPlane(p1, p2, p3,
                                                origin, barycentricQueryPoint,
                                                queryPoint) == false) {
               //
               // Ray from origin to query point is parallel to the plane
               //
               return;
            }
            
            //
            // Normal of tile
            //
            MathUtilities::computeNormal((float*)p1, (float*)p2, (float*)p3, normal);
         }
         break;
      case SURFACE_TYPE_HINT_OTHER:
         //
         // Project point onto the plane of the tile
         //
         MathUtilities::computeNormal((float*)p1, (float*)p2, (float*)p3, normal);
#ifdef HAVE_VTK5
         {
            double dn[3] = { normal[0], normal[1], normal[2] };
            double db[3] = { barycentricQueryPoint[0], barycentricQueryPoint[1], barycentricQueryPoint[2] };
            double dp[3] = { p1[0], p1[1], p1[2] };
            double dq[3];
            vtkPlane::ProjectPoint(db, dp, dn, dq); 
            queryPoint[0] = dq[0];
            queryPoint[1] = dq[1];
            queryPoint[2] = dq[2];
         }
#else // HAVE_VTK5
         vtkPlane::ProjectPoint((float*)barycentricQueryPoint, (float*)p1, normal, queryPoint);
#endif // HAVE_VTK5
         break;
   }
   
   float area1, area2, area3;
   
   //
   // Note that if tolerance is a small negative number (which is done to handle
   // degenerate cases - projected point on vertex or edge of triangle) an area may
   // be negative and we continue searching tiles.  If all areas are positive
   // then there is no need to continue searching.
   //
   const int result = triangleAreas(p1, p2, p3, normal, queryPoint, area1, area2, area3);
   if (result != 0) {
      if (result > 0) {
         barycentricSearchStatus = TILE_FOUND;
      }
      else {
         barycentricSearchStatus = TILE_FOUND_DEGENERATE;
      }
      barycentricTile     = tileNumber;
      barycentricNodes[0] = v1;
      barycentricNodes[1] = v2;
      barycentricNodes[2] = v3;
      barycentricAreas[0] = area1;
      barycentricAreas[1] = area2;
      barycentricAreas[2] = area3;
   }
}

/**
 * Compute the signed areas formed by assuming "xyz" is contained in the triangle formed
 * by the points "p1, p2, p3".  "area2" and "area3" may not be set if "xyz" is not
 * within the triangle.
 *
 * Returns 1 if all areas are positive (point inside the triangle).
 * Returns -1 if all areas are greater than the tolerance (point may be on edge or vertex)
 * Returns 0 if not in the triangle.
 */
int
BrainModelSurfacePointProjector::triangleAreas(const float* p1, const float* p2, const float* p3,
                                               const float* normal,
                                               const float* xyz,
                                               float& area1, float& area2, float& area3)
{
   int result = 0;
   float triangleArea = 0.0;
   bool inside = false;
   
   switch (surfaceTypeHint) {
      case SURFACE_TYPE_HINT_FLAT:
         area1 = MathUtilities::signedTriangleArea2D(p1, p2, xyz);
         if (area1 > tileAreaTolerance) {
            area2 = MathUtilities::signedTriangleArea2D(p2, p3, xyz);
            if (area2 > tileAreaTolerance) {
               area3 = MathUtilities::signedTriangleArea2D(p3, p1, xyz);
               if (area3 > tileAreaTolerance) {
                  inside = true;
                  triangleArea = MathUtilities::signedTriangleArea2D(p1, p2, p3);
               }
            }
         }
         break;
      case SURFACE_TYPE_HINT_SPHERE:
      case SURFACE_TYPE_HINT_OTHER:
         area1 = MathUtilities::signedTriangleArea3D(normal, p1, p2, xyz);   
         if (area1 >= tileAreaTolerance) {
            area2 = MathUtilities::signedTriangleArea3D(normal, p2, p3, xyz);
            if (area2 >= tileAreaTolerance) {
               area3 = MathUtilities::signedTriangleArea3D(normal, p3, p1, xyz);
               if (area3 >= tileAreaTolerance) {
                  inside = true;
                  triangleArea = MathUtilities::triangleArea((float*)p1, (float*)p2, (float*)p3);
               }
            }
         }
         break;
   }
  
   if (inside) {
      if ((area1 > 0.0) && (area2 > 0.0) && (area3 > 0.0)) {
         result = 1;
      }
      else {
         result = -1;
      }
      if (area1 < 0.0) area1 = -area1;
      if (area2 < 0.0) area2 = -area2;
      if (area3 < 0.0) area3 = -area3;
      
      if (triangleArea > 0.0) {
         //area1 /= triangleArea;
         //area2 /= triangleArea;
         //area3 /= triangleArea;
      }
      else {
         area1 = 1.0;
         area2 = 0.0;
         area3 = 0.0;
      }
   }

   return result;
}

void
BrainModelSurfacePointProjector::unprojectPoint(const int tileNodes[3], const float tileAreas[3],
                                                const CoordinateFile* cf, float xyzOut[3])
{
   const float* v1 = cf->getCoordinate(tileNodes[0]);
   const float* v2 = cf->getCoordinate(tileNodes[1]);
   const float* v3 = cf->getCoordinate(tileNodes[2]);
   
   const float totalArea = tileAreas[0] + tileAreas[1] + tileAreas[2];
   if (totalArea != 0.0) {
      xyzOut[0] = (v1[0] * tileAreas[1] + v2[0] * tileAreas[2] + v3[0] * tileAreas[0])
                / totalArea;
      xyzOut[1] = (v1[1] * tileAreas[1] + v2[1] * tileAreas[2] + v3[1] * tileAreas[0])
                / totalArea;
      xyzOut[2] = (v1[2] * tileAreas[1] + v2[2] * tileAreas[2] + v3[2] * tileAreas[0])
                / totalArea;   
   }
   else {
      xyzOut[0] = v1[0];
      xyzOut[1] = v1[1];
      xyzOut[2] = v1[2];
   }
}

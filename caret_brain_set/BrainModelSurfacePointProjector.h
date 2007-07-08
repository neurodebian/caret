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



#ifndef __VE_BRAIN_MODEL_SURFACE_POINT_PROJECTOR_H__
#define __VE_BRAIN_MODEL_SURFACE_POINT_PROJECTOR_H__

#include <set>

class BrainModelSurface;
class BrainModelSurfacePointLocator;
class CoordinateFile;
class TopologyFile;
class TopologyHelper;

/// This class is used to project points onto a BrainModelSurface.  It can project to
/// the nearest node or to a barycentric position in a tile.
class BrainModelSurfacePointProjector {
   public:
      
      /// Surface type hint passed to constructor
      enum SURFACE_TYPE_HINT {
         SURFACE_TYPE_HINT_FLAT,
         SURFACE_TYPE_HINT_SPHERE,
         SURFACE_TYPE_HINT_OTHER
      };
      
      /// Constructor
      BrainModelSurfacePointProjector(const BrainModelSurface* bmsIn,
                                      const SURFACE_TYPE_HINT surfaceTypeHintIn,
                                      const bool surfaceMayHaveNodesAddedToIt);
                                      
      /// Destructor
      ~BrainModelSurfacePointProjector();
      
      /// project to nearest node
      int projectToNearestNode(const float xyz[3]);
      
      /// barycentric projection (returns tile node projects to else negative)
      int projectBarycentric(const float xyz[3], int& nearestNodeNumberOut,
                             int tileNodesOut[3], float barycentricOut[3],
                             const bool checkNeighbors = true);
      
      /// barycentric projection to nearest tile but may not be within the tile
      int projectBarycentricNearestTile(const float xyz[3], int& nearestTileNumberOut,
                                        int tileNodesOut[3], float barycentricOut[3],
                                        float& signedDistanceOut, float& distanceToTile,
                                        float distanceComponents[3]);
      
      /// barycentric projection to the "best" tile (2D only) 
      int projectBarycentricBestTile2D(const float xyz[3], 
                                       int& nearestTileNumberOut,
                                       int tileNodesOut[3], 
                                       float barycentricOut[3]);
                                     
      /// unproject using the specified coordinate file
      static void unprojectPoint(const int tileNodes[3], const float tileAreas[3],
                                 const CoordinateFile* cf, float xyzOut[3]);
   
   private:
      
      /// Status of the search (degenerate is on an edge, vertex, or "just" outside the tile)
      enum SEARCH_STATUS {
         TILE_NOT_FOUND,
         TILE_FOUND,
         TILE_FOUND_DEGENERATE
      };
      
      /// see if a point is in any of the files used by a node.
      void checkPointInNodesTiles(const TopologyHelper* topologyHelper, const int nodeNumber);
                                  
      /// see if the point "xyz" is within the tile "tileNumber"
      void checkPointInTile(const int tileNumber);      
      
      /// compute areas formed by assuming xyz is within the triangle formed by p1, p2, p3
      int triangleAreas(const float* p1, const float* p2, const float* p3,
                        const float* normal, const float* xyz,
                        float& area1, float& area2, float& area3);
                         
      /// point locator for BrainModelSurface
      BrainModelSurfacePointLocator* pointLocator;
      
      /// coordinate file
      const CoordinateFile* coordinateFile;
      
      /// topology file
      const TopologyFile* topologyFile;
      
      /// the surface type hint
      SURFACE_TYPE_HINT surfaceTypeHint;
      
      /// inside triangle tolerance
      float tileAreaTolerance;
      
      /// "on" the node tolerance
      float nearestNodeToleranceSquared;
      
      /// tiles that have been searched in barycentric mode
      std::set<int> barycentricSearchedTiles;
      
      /// the search status (used in barycentric searching)
      SEARCH_STATUS barycentricSearchStatus;
      
      /// tile found in barycentric mode
      int barycentricTile;
      
      /// nodes of tile found in barycentric mode
      int barycentricNodes[3];
      
      /// barycentric areas of tile found in barycentric mode
      float barycentricAreas[3];
      
      /// the query point for barycentric mode
      float barycentricQueryPoint[3];
      
};

#endif  // __VE_BRAIN_MODEL_SURFACE_POINT_PROJECTOR_H__


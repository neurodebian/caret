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
#include <iostream>
#include <vector>

#include <QDateTime>

#include "vtkLine.h"

#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderCutter.h"
#include "BrainSet.h"
#include "BorderFile.h"
#include "DebugControl.h"
#include "MathUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelSurfaceBorderCutter::BrainModelSurfaceBorderCutter(
                              BrainSet* bsIn,
                              BrainModelSurface* cuttingSurfaceIn,
                              const BorderProjectionFile* cutBorderProjectionFileIn,
                              const CUTTING_MODE cuttingModeIn,
                              const bool extendBordersToNearestEdgeNodeFlagIn)
   : BrainModelAlgorithm(bsIn),
     cuttingSurface(cuttingSurfaceIn),
     cutBorderProjectionFile(cutBorderProjectionFileIn),
     cuttingMode(cuttingModeIn),
     extendBordersToNearestEdgeNodeFlag(extendBordersToNearestEdgeNodeFlagIn)
{
}

/**
 * Destructor.
 */
BrainModelSurfaceBorderCutter::~BrainModelSurfaceBorderCutter()
{
}

/**
 * Execute the flattening.
 */
void 
BrainModelSurfaceBorderCutter::execute() throw (BrainModelAlgorithmException)
{
   QTime timer;
   timer.start();
   
   //
   // NOTE: We could save some time and use a topology helper.  But, if the topology is messed
   // up and more than two tiles use an edge, not all tiles would be removed.  So, just
   // test all tiles.
   //
   
   //
   // Get the topolgoy file
   //
   TopologyFile* topology = cuttingSurface->getTopologyFile();
   const TopologyHelper* topologyHelper = topology->getTopologyHelper(false, true, false);
   
   //
   // Keep track of tiles that need to be deleted.
   //
   const int numTiles = topology->getNumberOfTiles();   
   std::vector<bool> deleteTheTile(numTiles, false);
   
   //
   // Apply all of the cuts
   //
   const int numBorderProjections = 
      static_cast<int>(cutBorderProjectionFile->getNumberOfBorderProjections());
   for (int bpi = 0; bpi < numBorderProjections; bpi++) {
      //
      // only do cuts with at least two links
      //
      const BorderProjection& borderProjection = *cutBorderProjectionFile->getBorderProjection(bpi);
      if (borderProjection.getNumberOfLinks() > 1) {
         //
         // Copy the surface since it may need to be rotated
         //
         BrainModelSurface surface(*cuttingSurface);
         
         //
         // Get the coordinate file
         //
         const CoordinateFile* coords = surface.getCoordinateFile();
         const float* coordXYZ = coords->getCoordinate(0);

         //
         // May need to rotate the surface to apply the cut
         //
         switch (cuttingMode) {
            case CUTTING_MODE_FLAT_SURFACE:
               break;
            case CUTTING_MODE_NON_NEGATIVE_Z_ONLY:
               break;
            case CUTTING_MODE_SPHERICAL_SURFACE:
               {
                  //
                  // Get the center of gravity of the border
                  //
                  float cog[3];
                  if (borderProjection.getCenterOfGravity(coords,
                                                          topologyHelper,
                                                          cog)) {
                  }
                  else {
                     throw BrainModelAlgorithmException(
                        "Unable to get center of gravity for cut "
                        + borderProjection.getName());
                  }
                  
                  //
                  // Orient sphere so that border COG is on positive Z-axis
                  //
                  surface.orientPointToPositiveZAxis(cog);
               }
               break;
         }
         
         //
         // Unproject the border
         //
         Border cut;
         borderProjection.unprojectBorderProjection(coords, topologyHelper, cut);
         
         //
         // Are borders to be extended
         //
         if (extendBordersToNearestEdgeNodeFlag) {
            extendBorderToNearestEdgeNode(cut);
         }
   
         const int numCutPoints = cut.getNumberOfLinks();
         
         //
         // test each segment in the cut
         //
         if (numCutPoints > 1) {
            for (int cpi = 0; cpi < (numCutPoints - 1); cpi++) {
               float c1[3], c2[3];
               cut.getLinkXYZ(cpi, c1);
               cut.getLinkXYZ(cpi + 1, c2);
               c1[2] = 0.0;
               c2[2] = 0.0;
               
               //
               // Test this cut segment agains all tiles
               //
               for (int tile = 0; tile < numTiles; tile++) {
                  //
                  // If tile has not already been marked for deletion
                  //
                  if (deleteTheTile[tile] == false) {
                     //
                     // Get the vertices of the tile
                     //
                     int v1, v2, v3;
                     topology->getTile(tile, v1, v2, v3);
                     
                     const float* p1 = &coordXYZ[v1*3];
                     const float* p2 = &coordXYZ[v2*3];
                     const float* p3 = &coordXYZ[v3*3];
                     bool doCutCheck = true;
                     switch(cuttingMode) {
                        case CUTTING_MODE_FLAT_SURFACE:
                           break;
                        case CUTTING_MODE_NON_NEGATIVE_Z_ONLY:
                        case CUTTING_MODE_SPHERICAL_SURFACE:
                           if ((p1[2] < 0.0) || (p2[2] < 0.0) || (p3[2] < 0.0)) {
                              doCutCheck = false;
                           }
                           break;
                     }
                     
                     if (doCutCheck) {
                        //
                        // Check the cut segment against each tile edge
                        //
                        float intersection[2];
                        if (MathUtilities::lineIntersection2D(c1, c2, p1, p2, intersection)) {
                           deleteTheTile[tile] = true;
                        }
                        else if (MathUtilities::lineIntersection2D(c1, c2, p2, p3, intersection)) {
                           deleteTheTile[tile] = true;
                        }
                        else if (MathUtilities::lineIntersection2D(c1, c2, p3, p1, intersection)) {
                           deleteTheTile[tile] = true;
                        }
                     }
                  }
               }
            }
         }
      }
   }
   
   //
   // If cuts were made
   //
   if (std::find(deleteTheTile.begin(), deleteTheTile.end(), true) != deleteTheTile.end()) {
      //
      // Remove the cut tiles
      //
      if (DebugControl::getDebugOn()) {
         std::cout << "deleting tiles ";
      }
      int cnt = 0;
      std::vector<int> tilesToDelete;
      for (int i = 0; i < numTiles; i++) {
         if (deleteTheTile[i]) {
            tilesToDelete.push_back(i);
            if (DebugControl::getDebugOn()) {
               std::cout << " " << i;
            }
            cnt++;
         }
      }
      if (DebugControl::getDebugOn()) {
         std::cout << std::endl;
         std::cout << "Deleting " << cnt << " tiles." << std::endl;
      }

      const int numTilesBeforeDelete = topology->getNumberOfTiles();
      topology->deleteTiles(tilesToDelete);
      if (DebugControl::getDebugOn()) {
         std::cout << "Tiles before applying cuts: " << numTilesBeforeDelete << std::endl;
         std::cout << "Tiles after applying cuts: " << topology->getNumberOfTiles() << std::endl;
      }
      
      //
      // Set topolgy file type to cut
      //
      topology->setTopologyType(TopologyFile::TOPOLOGY_TYPE_CUT);
   }
   
   //std::cout << "Total time: " << timer.elapsed() / 1000 << " seconds." << std::endl;
}

/**
 * extend the borders to nearest edge node.
 */
void 
BrainModelSurfaceBorderCutter::extendBorderToNearestEdgeNode(Border& border)
{
   //
   // Surface coordinate file
   //
   const CoordinateFile* cf = cuttingSurface->getCoordinateFile();
   const int numCoords = cf->getNumberOfCoordinates();
   TopologyFile* tf = cuttingSurface->getTopologyFile();
   
   //
   // Classify nodes
   //
   brainSet->classifyNodes(tf, false);
   
   //
   // Check border
   //
   const int numLinks = border.getNumberOfLinks();
   if (numLinks > 1) {
      const float* firstLinkXYZ = border.getLinkXYZ(0);
      float distToFirstLink = std::numeric_limits<float>::max();
      int nodeNearestFirstLink = -1;
      const float* lastLinkXYZ = border.getLinkXYZ(numLinks - 1);
      float distToLastLink = std::numeric_limits<float>::max();
      int nodeNearestLastLink = -1;
      
      //
      // Find node nearest to first and last links
      //
      for (int j = 0; j < numCoords; j++) {
         //
         // if NOT interior node
         //
         if (brainSet->getNodeAttributes(j)->getClassification()
             != BrainSetNodeAttribute::CLASSIFICATION_TYPE_INTERIOR) {
            
            const float distFirst = cf->getDistanceToPointSquared(j, firstLinkXYZ);
            if (distFirst < distToFirstLink) {
               distToFirstLink = distFirst;
               nodeNearestFirstLink = j;
            }
            
            const float distLast = cf->getDistanceToPointSquared(j, lastLinkXYZ);
            if (distLast < distToLastLink) {
               distToLastLink = distLast;
               nodeNearestLastLink = j;
            }
         }
      }
      
      //
      // Determine where new link should be inserted
      //
      if (distToFirstLink < distToLastLink) {
         if (nodeNearestFirstLink >= 0) {
            const float* xyz = cf->getCoordinate(nodeNearestFirstLink);
            border.insertBorderLink(0, xyz);
         }
      }
      else {
         if (nodeNearestLastLink >= 0) {
            const float* xyz = cf->getCoordinate(nodeNearestLastLink);
            border.addBorderLink(xyz);
         }
      }
   }
}


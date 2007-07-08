#include <algorithm>
#include <iostream>
#include <vector>

#include <QDateTime>

#include "vtkLine.h"

#include "BrainModelSurface.h"
#include "BrainModelSurfaceCutter.h"
#include "BorderFile.h"
#include "DebugControl.h"
#include "MathUtilities.h"
#include "TopologyFile.h"

/**
 * Constructor.
 */
BrainModelSurfaceCutter::BrainModelSurfaceCutter(BrainModelSurface* cuttingSurfaceIn,
                                                 BorderFile* cutsFileIn,
                                                 const CUTTING_MODE cuttingModeIn)
   : BrainModelAlgorithm(cuttingSurfaceIn->getBrainSet())
{
   cuttingSurface = cuttingSurfaceIn;
   theCuts        = cutsFileIn;
   cuttingMode    = cuttingModeIn;
}

/**
 * Destructor.
 */
BrainModelSurfaceCutter::~BrainModelSurfaceCutter()
{
}

/**
 * Execute the flattening.
 */
void 
BrainModelSurfaceCutter::execute() throw (BrainModelAlgorithmException)
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

   //
   // Get the coordinate file
   //
   const CoordinateFile* coords = cuttingSurface->getCoordinateFile();
   const float* coordXYZ = coords->getCoordinate(0);
   
   //
   // Keep track of tiles that need to be deleted.
   //
   const int numTiles = topology->getNumberOfTiles();   
   std::vector<bool> deleteTheTile(numTiles, false);
   
   //
   // Apply all of the cuts
   //
   const int numCuts = theCuts->getNumberOfBorders();
   for (int cti = 0; cti < numCuts; cti++) {
      //
      // only do cuts with at least two links
      //
      const Border* cut = theCuts->getBorder(cti);
      const int numCutPoints = cut->getNumberOfLinks();
      if (numCutPoints > 1) {
         //
         // test each segment in the cut
         //
         for (int cpi = 0; cpi < (numCutPoints - 1); cpi++) {
            float c1[3], c2[3];
            cut->getLinkXYZ(cpi, c1);
            cut->getLinkXYZ(cpi + 1, c2);
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
                  //float p1[3], p2[3], p3[3];
                  //coords->getCoordinate(v1, p1); 
                  //coords->getCoordinate(v2, p2); 
                  //coords->getCoordinate(v3, p3);
                  
                  const float* p1 = &coordXYZ[v1*3];
                  const float* p2 = &coordXYZ[v2*3];
                  const float* p3 = &coordXYZ[v3*3];
                  bool doCutCheck = true;
                  switch(cuttingMode) {
                     case CUTTING_MODE_NORMAL:
                        break;
                     case CUTTING_MODE_NON_NEGATIVE_Z_ONLY:
                        if ((p1[2] < 0.0) || (p2[2] < 0.0) || (p3[2] < 0.0)) {
                           doCutCheck = false;
                        }
                        break;
                  }
                  
                  if (doCutCheck) {
                     //p1[2] = 0.0;
                     //p2[2] = 0.0;
                     //p3[2] = 0.0;
                     
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



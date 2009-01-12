#include <algorithm>
#include <cmath>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceDistortion.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "StatisticDataGroup.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "vtkTriangle.h"

/**
 * Constructor
 */
BrainModelSurfaceDistortion::BrainModelSurfaceDistortion(BrainSet* brainSetIn,
                                                BrainModelSurface* surfaceIn,
                                                BrainModelSurface* referenceSurfaceIn,
                                                TopologyFile* topologyFileIn,
                                                SurfaceShapeFile* surfaceShapeFileIn,
                                                const int arealDistortionColumnIn,
                                                const int linearDistortionColumnIn,
                                                const QString& arealDistortionNameIn,
                                                const QString& linearDistortionNameIn)
   : BrainModelAlgorithm(brainSetIn)
{
   surface              = surfaceIn;
   referenceSurface     = referenceSurfaceIn;
   topologyFile         = topologyFileIn;
   surfaceShapeFile     = surfaceShapeFileIn;
   arealDistortionColumn = arealDistortionColumnIn;
   linearDistortionColumn = linearDistortionColumnIn;
   arealDistortionName  = arealDistortionNameIn;
   linearDistortionName = linearDistortionNameIn;
}
                        
/**
 * Destructor
 */
BrainModelSurfaceDistortion::~BrainModelSurfaceDistortion()
{
}

/**
 * add/update the distortions in the surface shape file
 */
void 
BrainModelSurfaceDistortion::execute() throw (BrainModelAlgorithmException)
{
   //
   // Coordinate files of both surfaces
   //
   const CoordinateFile* referenceCoords = referenceSurface->getCoordinateFile();
   const CoordinateFile* coords          = surface->getCoordinateFile();
   
   //
   // Get number of nodes
   //
   const int numNodes = referenceCoords->getNumberOfCoordinates();
   
   //
   // Use topology helper to find tiles for each node
   //
   const TopologyHelper th(topologyFile, false, true, false);
   
   //
   // Create comment info about surface used in distortion
   //
   QString commentInfo("Reference Surface: ");
   commentInfo.append(FileUtilities::basename(referenceSurface->getFileName()));
   commentInfo.append("\nSurface:");
   commentInfo.append(FileUtilities::basename(surface->getFileName()));
   
   //
   // Create surface shape file columns if needed
   //
   if (arealDistortionColumn == DISTORTION_COLUMN_CREATE_NEW) {
      if (surfaceShapeFile->getNumberOfColumns() == 0) {
         surfaceShapeFile->setNumberOfNodesAndColumns(surface->getNumberOfNodes(), 1);
      }
      else {
         surfaceShapeFile->addColumns(1);
      }
      arealDistortionColumn = surfaceShapeFile->getNumberOfColumns() - 1;
   }
   if (linearDistortionColumn == DISTORTION_COLUMN_CREATE_NEW) {
      if (surfaceShapeFile->getNumberOfColumns() == 0) {
         surfaceShapeFile->setNumberOfNodesAndColumns(surface->getNumberOfNodes(), 1);
      }
      else {
         surfaceShapeFile->addColumns(1);
      }
      linearDistortionColumn = surfaceShapeFile->getNumberOfColumns() - 1;
   }
   
   //
   // Set column names
   //
   if (arealDistortionColumn >= 0) {
      surfaceShapeFile->setColumnName(arealDistortionColumn, arealDistortionName);
      surfaceShapeFile->setColumnColorMappingMinMax(arealDistortionColumn, -1.0, 1.0);
      surfaceShapeFile->setColumnComment(arealDistortionColumn, commentInfo);
   }
   if (linearDistortionColumn >= 0) {
      surfaceShapeFile->setColumnName(linearDistortionColumn, linearDistortionName);
      surfaceShapeFile->setColumnColorMappingMinMax(linearDistortionColumn, 0.0, 2.0);
      surfaceShapeFile->setColumnComment(linearDistortionColumn, commentInfo);
   }
   
   //
   // If areal distortion should be created
   //
   if (arealDistortionColumn >= 0) {
      //
      // Allocate areal distortion for statistics
      //
      arealDistortionForStatistics.resize(numNodes, 0);
      
      //
      // log(2.0) to avoid recomputation each iteration
      //
      const double log2 = log(2.0);
      
      //
      // Compute distortion for each tile
      //
      const int numTiles = topologyFile->getNumberOfTiles();
      std::vector<float> tileDistortion;
      for (int i = 0; i < numTiles; i++) {
         const int* v = topologyFile->getTile(i);
         
         const float tileAreaReference = MathUtilities::triangleArea(
            (float*)referenceCoords->getCoordinate(v[0]),
            (float*)referenceCoords->getCoordinate(v[1]),
            (float*)referenceCoords->getCoordinate(v[2]));
            
         const float tileArea = MathUtilities::triangleArea(
            (float*)coords->getCoordinate(v[0]),
            (float*)coords->getCoordinate(v[1]),
            (float*)coords->getCoordinate(v[2]));
            
         double distortion = 0.0;
         if (tileAreaReference != 0.0) {
            distortion = tileArea / tileAreaReference;
         }
         else {
            if (tileArea != 0.0) {
               distortion = 10000.0;  // big distortion since denominator is zero
            }
            else {
               distortion = 1.0;   // if both zero then same area
            }
         }
         
         //
         // zero will cause -inf
         //
         if (distortion < 0.00000001) {
            distortion = 0.00000001;
         }
         
         tileDistortion.push_back(log(distortion) / log2);
      }
   
      //
      // Compute areal distortion for all nodes
      //
      for (int i = 0; i < numNodes; i++) {
         float distortion = 1.0;
         const float numberOfNeighbors = th.getNodeNumberOfNeighbors(i);
         
         if (numberOfNeighbors >= 1.0) {
            distortion = 0.0;
            std::vector<int> tiles;
            th.getNodeTiles(i, tiles);
            
            for (int j = 0; j < static_cast<int>(tiles.size()); j++) {
               distortion += tileDistortion[tiles[j]];
            }
            distortion /= numberOfNeighbors;
         }
         surfaceShapeFile->setValue(i, arealDistortionColumn, distortion);
         arealDistortionForStatistics[i] = distortion;
      }
   }
   
   //
   // If linear distortion should be created
   //
   if (linearDistortionColumn >= 0) {
      //
      // Allocate linear distortion for statistics
      //
      linearDistortionForStatistics.resize(numNodes, 0);
      
      //
      // Compute linear distortion for all nodes
      //
      for (int i = 0; i < numNodes; i++) {
         float distortion = 0.0;
         std::vector<int> neighbors;
         th.getNodeNeighbors(i, neighbors);
         const int numberOfNeighbors = neighbors.size();
         
         if (numberOfNeighbors >= 1) {
            const float* me = coords->getCoordinate(i); 
            const float* ref = referenceCoords->getCoordinate(i); 
            
            float distort = 0.0;
            for (int j = 0; j < numberOfNeighbors; j++) {
               const float dist = MathUtilities::distance3D(me, 
                                                  coords->getCoordinate(neighbors[j]));
               const float distRef = MathUtilities::distance3D(ref, 
                                                referenceCoords->getCoordinate(neighbors[j]));
               if (distRef != 0.0) {
                  distort += (dist / distRef);
               }
               else {
                  if (dist != 0.0) {
                     distort += 10000.0;   // big number since denominator is zero
                  }
                  else {
                     distort += 1.0;   // both are zero
                  }
               }
            }
            distortion = distort / static_cast<float>(numberOfNeighbors);
         }
         surfaceShapeFile->setValue(i, linearDistortionColumn, distortion);
         linearDistortionForStatistics[i] = distortion;
      }
   }
}

/**
 * Get statistical measurements for areal distortion
 */
void
BrainModelSurfaceDistortion::getArealDistortionStatistics(StatisticsUtilities::DescriptiveStatistics& stats) const
{
   stats.reset();
   StatisticsUtilities::computeStatistics(arealDistortionForStatistics,
                                          true,
                                          stats);
/*
   StatisticDescriptiveStatistics sds(StatisticDescriptiveStatistics::DATA_TYPE_SAMPLE);
   StatisticDataGroup sdg(&arealDistortionForStatistics);
   sds.addDataGroup(&sdg);
   try {
      sds.execute();
   }
   catch (StatisticException&) {
   }
   stats = sds.getDescriptiveStatistics();
*/
}

/**
 * Get statistical measurements for linear distortion
 */
void
BrainModelSurfaceDistortion::getLinearDistortionStatistics(StatisticsUtilities::DescriptiveStatistics& stats) const
{
   stats.reset();
   StatisticsUtilities::computeStatistics(linearDistortionForStatistics,
                                          true,
                                          stats);
/*
   StatisticDescriptiveStatistics sds(StatisticDescriptiveStatistics::DATA_TYPE_SAMPLE);
   StatisticDataGroup sdg(&linearDistortionForStatistics);
   sds.addDataGroup(&sdg);
   try {
      sds.execute();
   }
   catch (StatisticException&) {
   }
   stats = sds.getDescriptiveStatistics();
*/
}



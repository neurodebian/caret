
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
#include <limits>

#include <QDateTime>

#include "vtkTriangle.h"
#include "vtkPointLocator.h"
#include "vtkPoints.h"

#include "BrainModelSurface.h"
#include "BrainModelSurfaceDistortion.h"
#include "BrainModelSurfaceSmoothing.h"
#include "BrainModelSurfaceSphericalTessellator.h"
#include "BrainModelSurfaceTopologyCorrector.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "MathUtilities.h"
#include "PointLocator.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelSurfaceTopologyCorrector::BrainModelSurfaceTopologyCorrector(
                                                     BrainSet* bsIn,
                                                     const BrainModelSurface* fiducialSurfaceIn,
                                                     const int numberOfSmoothingCyclesIn,
                                                     const bool removeHighlyCompressedNodesIn,
                                                     const float compressedNodesArealDistortionThresholdIn)
   : BrainModelAlgorithm(bsIn) 
{
   fiducialSurface = new BrainModelSurface(*fiducialSurfaceIn);
   sphericalSurface = new BrainModelSurface(*fiducialSurfaceIn);
   numberOfSmoothingCycles = numberOfSmoothingCyclesIn;
   removeHighlyCompressedNodes = removeHighlyCompressedNodesIn;
   compressedNodesArealDistortionThreshold = compressedNodesArealDistortionThresholdIn;
   numNodes = 0;
   copyOfOriginalTopologyFile = NULL;
   workingTopologyFile = NULL;
   skipCorrectedSurfaceGeneration = false;
}

/**
 * Destructor.
 */
BrainModelSurfaceTopologyCorrector::~BrainModelSurfaceTopologyCorrector()
{
   if (copyOfOriginalTopologyFile != NULL) {
      delete copyOfOriginalTopologyFile;
      copyOfOriginalTopologyFile = NULL;
   }
   if (workingTopologyFile != NULL) {
      delete workingTopologyFile;
      workingTopologyFile = NULL;
   }
}

/**
 * Smooth the spherical surface.
 */
void 
BrainModelSurfaceTopologyCorrector::smoothSphericalSurfaceToMinimizeCrossovers() throw (BrainModelAlgorithmException)
{
   int cycleNumber = 0;
   while (cycleNumber < numberOfSmoothingCycles) {
      //
      // Translate to center of mass
      //
      sphericalSurface->translateToCenterOfMass();
   
      //
      // Do spherical smoothing
      //
      const int smoothingIterations = 100;
      BrainModelSurfaceSmoothing smoothing(brainSet,
                                           sphericalSurface,
                                           BrainModelSurfaceSmoothing::SMOOTHING_TYPE_LINEAR,
                                           1.0,
                                           smoothingIterations,
                                           0,
                                           0,
                                           NULL,
                                           NULL,
                                           10,
                                           0);
      smoothing.execute(); 
                                          
      //
      // Do a crossover check
      //
      int numNodeCrossovers, numTileCrossovers;
      sphericalSurface->crossoverCheck(numNodeCrossovers, 
                                       numTileCrossovers,
                                       BrainModelSurface::SURFACE_TYPE_SPHERICAL);
                                       
      int crossoverCount = 0;
      for (int j = 0; j < numNodes; j++) {
         BrainSetNodeAttribute* bna = brainSet->getNodeAttributes(j);
         if (bna->getCrossover() == BrainSetNodeAttribute::CROSSOVER_YES) {
            crossoverCount++;
         }
      }
      if (DebugControl::getDebugOn()) {
         std::cout << "Crossovers at smoothing cycle " << cycleNumber
                   << ": " << crossoverCount << std::endl;
      }
      if (crossoverCount <= 0) {
         break;
      }
      
      cycleNumber++;
   }
}

/**
 * Flag nodes in highly compressed tiles as unavailable.
 */
void 
BrainModelSurfaceTopologyCorrector::removeNodesInHighlyCompressedTilesFromAvailableNodes() throw (BrainModelAlgorithmException)
{
   //
   // Calculate areal distortion
   //
   SurfaceShapeFile ssf;
   BrainModelSurfaceDistortion bmsd(brainSet,
                                    sphericalSurface,
                                    fiducialSurface,
                                    fiducialSurface->getTopologyFile(),
                                    &ssf,
                                    "Areal",
                                    "");
   bmsd.execute();
   if (ssf.getNumberOfColumns() != 1) {
      throw BrainModelAlgorithmException("Calculation of areal distortion failed.");
   }
   
   //
   // Get a topology helper for finding connected nodes to use in tessellation
   //
   const TopologyHelper* th = workingTopologyFile->getTopologyHelper(false, true, false);

   //
   // Mark nodes that exceed areal distortion threshold
   //
   std::vector<bool> distortedNodes(numNodes, false);
   for (int i = 0; i < numNodes; i++) {
      if (ssf.getValue(i, 0) < compressedNodesArealDistortionThreshold) {
         if (th->getNodeHasNeighbors(i)) {
            distortedNodes[i] = true;
         }
      }
   }
   
   //
   // Delete tiles using distorted nodes
   //
   workingTopologyFile->deleteTilesWithMarkedNodes(distortedNodes);
}
      
/**
 * Flag the crossover nodes as unavailable.
 */
void
BrainModelSurfaceTopologyCorrector::removeCrossoverNodesFromAvailableNodes()
{
   //
   // Do a crossover check
   //
   int numNodeCrossovers, numTileCrossovers;
   sphericalSurface->crossoverCheck(numNodeCrossovers, 
                                 numTileCrossovers,
                                 BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   
   //
   // Mark crossovers
   //
   std::vector<bool> nodeIsCrossover(numNodes, false);
   for (int j = 0; j < numNodes; j++) {
      BrainSetNodeAttribute* bna = brainSet->getNodeAttributes(j);
      if (bna->getCrossover() == BrainSetNodeAttribute::CROSSOVER_YES) {
         nodeIsCrossover[j] = true;
      }
   }
   
   //
   // Delete tiles using crossover nodes
   //
   workingTopologyFile->deleteTilesWithMarkedNodes(nodeIsCrossover);
}

/**
 * retessellate the sphere.
 */
BrainModelSurface* 
BrainModelSurfaceTopologyCorrector::retessellateTheSphericalSurface() throw (BrainModelAlgorithmException)
{
   //
   // Get a topology helper for finding connected nodes to use in tessellation
   //
   const TopologyHelper* th = workingTopologyFile->getTopologyHelper(false, true, false);

   //
   // nodes that are to be added to the sphere
   //
   std::vector<bool> nodeAvailableFlags(numNodes, false);
   for (int i = 0; i < numNodes; i++) {
      if (th->getNodeHasNeighbors(i)) {
         nodeAvailableFlags[i] = true;
      }
   }
      
   //
   // Tessllate the available nodes
   //
   BrainModelSurfaceSphericalTessellator tess(brainSet,
                                              sphericalSurface,
                                              nodeAvailableFlags);
   tess.execute();
   
   return tess.getPointerToNewSphericalSurface();
}
      
/**
 * get a list of node numbers that were removed.
 */
void
BrainModelSurfaceTopologyCorrector::getListOfNodesThatWereRemoved(std::vector<int>& nodesRemoved) const
{
   nodesRemoved.clear();
   
   //
   // Get a topology helper for finding connected nodes to use in tessellation
   //
   const TopologyHelper* origTH = copyOfOriginalTopologyFile->getTopologyHelper(false, true, false);
   const TopologyHelper* correctedTH = workingTopologyFile->getTopologyHelper(false, true, false);
   
   //
   // Determine which nodes were disconnected
   //
   for (int i = 0; i < numNodes; i++) {
      if ((origTH->getNodeHasNeighbors(i)) &&
          (correctedTH->getNodeHasNeighbors(i) == false)) {
         nodesRemoved.push_back(i);
      }
   }
}
      
/**
 * smooth around removed nodes.
 */
void 
BrainModelSurfaceTopologyCorrector::smoothAroundRemovedNodes()
{
   //
   // Get nodes that were removed
   //
   std::vector<int> removedNodeNumbers;
   getListOfNodesThatWereRemoved(removedNodeNumbers);
   const int numRemovedNodes = static_cast<int>(removedNodeNumbers.size());
   
   //
   // Use the original topology file to find nodes for smoothing
   //
   const int neighborDepth = 3;
   std::vector<bool> smoothThisNode(numNodes, false);
   const TopologyHelper* th = copyOfOriginalTopologyFile->getTopologyHelper(false, true, false);
   for (int i = 0; i < numRemovedNodes; i++) {
      const int nodeNum = removedNodeNumbers[i];
      std::vector<int> neighbors;
      th->getNodeNeighborsToDepth(nodeNum,
                                  neighborDepth,
                                  neighbors);
      const int numNeighbors = static_cast<int>(neighbors.size());
      for (int j = 0; j < numNeighbors; j++) {
         smoothThisNode[neighbors[j]] = true;
      }
   }
   
   //
   // copy the corrected fiducial surface
   //
   BrainModelSurface* smoothedFiducialSurface = new BrainModelSurface(*fiducialSurface);
   
   //
   // Smooth the surface
   //
   const int numSmoothingIterations = 10;
   BrainModelSurfaceSmoothing bmss(brainSet,
                                   smoothedFiducialSurface,
                                   BrainModelSurfaceSmoothing::SMOOTHING_TYPE_AREAL,
                                   1.0,
                                   numSmoothingIterations,
                                   0,
                                   0,
                                   &smoothThisNode,
                                   NULL,
                                   0,
                                   0);
   bmss.execute();
   
   //
   // Rename and add to brain set
   //
   smoothedFiducialSurface->getCoordinateFile()->replaceFileNameDescription("FIDUCIAL_CORRECTED_SMOOTHED");
   brainSet->addBrainModel(smoothedFiducialSurface);
   
}
      
/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceTopologyCorrector::execute() throw (BrainModelAlgorithmException)
{
   QTime timer;
   timer.start();

   //
   // Get the number of nodes
   //
   numNodes = sphericalSurface->getNumberOfNodes();
      
   sphericalSurface->setSurfaceType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   
   //
   // Make a copy of the topology file which will be used to maintain connected nodes
   //
   copyOfOriginalTopologyFile = new TopologyFile(*(sphericalSurface->getTopologyFile()));
   workingTopologyFile        = new TopologyFile(*(sphericalSurface->getTopologyFile()));
   
   try {
      //
      // Smooth the sphere to unfold it
      //
      smoothSphericalSurfaceToMinimizeCrossovers();

      //
      // Remove crossover nodes from the available nodes
      //
      removeCrossoverNodesFromAvailableNodes();
      
      //
      // Remove nodes that are part of tiles that become highly compressed due to smoothing
      //
      if (removeHighlyCompressedNodes) {
         removeNodesInHighlyCompressedTilesFromAvailableNodes();
      }
      
      //
      // Remove any islands from the working topology file that were caused by node/tile deletion
      //
      workingTopologyFile->disconnectIslands();
      
      //
      // Should corrected surfaces be generated
      //
      BrainModelSurface* retessellatedSphereSurface = NULL;
      if (skipCorrectedSurfaceGeneration == false) {
         //
         // Create a newly tessellated sphere
         //
         retessellatedSphereSurface = retessellateTheSphericalSurface();
         
         //
         // Apply the spherical surface's new topology to the fiducial surface
         //
         fiducialSurface->setTopologyFile(retessellatedSphereSurface->getTopologyFile());
         
         //
         // Rename the fiducial to FiducialCorrected
         //
         fiducialSurface->getCoordinateFile()->replaceFileNameDescription("FIDUCIAL_CORRECTED");
      
         //
         // Add the fiducial to the brain set
         //
         brainSet->addBrainModel(fiducialSurface);
         
         //
         // Create a second fiducial smoothed around removed nodes
         //
         //smoothAroundRemovedNodes();
      }
      
      if (DebugControl::getDebugOn()) {
         sphericalSurface->getCoordinateFile()->replaceFileNameDescription("SPHERE_SMOOTHED_UNCORRECTED");
         brainSet->addBrainModel(sphericalSurface);   
         std::cout << "Time to correct surface: " << timer.elapsed() * 0.001 << std::endl; 
      }
      else {
         //
         // Delete the smoothed sphere
         //
         delete sphericalSurface;
         sphericalSurface = NULL;
         
         //
         // Delete the retessellated sphere
         //
         if (retessellatedSphereSurface != NULL) {
            brainSet->deleteBrainModel(retessellatedSphereSurface);
         }
      }
   }
   catch (TessellationException& te) {
      throw BrainModelAlgorithmException(te.whatQString());
   }
}



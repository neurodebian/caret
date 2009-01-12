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
#define _USE_MATH_DEFINES
#endif

#include <QFile>
#include <QTextStream>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

#include "BrainModelSurfaceDistortion.h"
#include "BrainSetNodeAttribute.h"
#include "BrainSet.h"
#include "BrainModelSurfaceMorphing.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "StatisticsUtilities.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyHelper.h"

#include "vtkMath.h"
#include "vtkTriangle.h"

/**
 * Constructor
 */
BrainModelSurfaceMorphing::BrainModelSurfaceMorphing(BrainSet* brainSetIn,
                                           BrainModelSurface* referenceSurfaceIn,
                                           BrainModelSurface* morphingSurfaceIn,
                                           const MORPHING_SURFACE_TYPE morphingSurfaceTypeIn,
                                           const int numberOfThreadsIn)
   : BrainModelAlgorithmMultiThreaded(brainSetIn, NULL, -1, false)
{
   initialize();
   referenceSurface = referenceSurfaceIn;
   morphingSurface  = morphingSurfaceIn;
   morphingSurfaceType = morphingSurfaceTypeIn;
   
   numberOfNodes = 0;
   if (morphingSurface != NULL) {
      numberOfNodes = morphingSurface->getNumberOfNodes();
      nodeShouldBeMorphed = new int[numberOfNodes];
      for (int i = 0; i < numberOfNodes; i++) {
         nodeShouldBeMorphed[i] = true;
      }
   }
   
   PreferencesFile* pf = brainSet->getPreferencesFile();
   int numThreads = pf->getMaximumNumberOfThreads();
   if (numThreads <= 0) {
      numThreads = 1;
   }
   if (numberOfThreadsIn > 0) {
      numThreads = numberOfThreadsIn;
   }
   setNumberOfThreadsToRun(numThreads);
}

/**
 * Constructor for a thread instance.
 */
BrainModelSurfaceMorphing::BrainModelSurfaceMorphing(BrainSet* brainSetIn,
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
                                           const int threadNumberIn)
   : BrainModelAlgorithmMultiThreaded(brainSetIn, parentOfThisThreadIn, threadNumberIn, true)
{
   referenceSurface    = referenceSurfaceIn;
   morphingSurface     = morphingSurfaceIn;
   morphingSurfaceType = morphingSurfaceTypeIn;
   morphNodeInfo       = morphNodeInfoIn;
   nodeShouldBeMorphed = nodeShouldBeMorphedIn;
   noMorphNeighborStepSize = noMorphStepSizeIn;
   startNodeIndex = startNodeIndexIn;
   endNodeIndex   = endNodeIndexIn;
   sphericalSurfaceRadius = sphericalSurfaceRadiusIn;
}                                           
                                           
/**
 * Initialize variables for this instance.
 */
void
BrainModelSurfaceMorphing::initialize()
{
   doStatisticsEachPass = false;
   
   coordsArray1 = NULL;
   coordsArray2 = NULL;
   morphNodeInfo = NULL;
   referenceSurface = NULL;
   morphingSurface  = NULL;
   iterations      = 1;
   linearForce  = 0.50;
   angularForce = 0.30;
   stepSize     = 0.5;
   noMorphNeighborStepSize = 0.5;
   
   nodeShouldBeMorphed = NULL;
   numberOfNodes = 0;
   allNodesBeingMorphed = true;

   setNumberOfThreadsToRun(1);
}

/**
 * Destructor
 */
BrainModelSurfaceMorphing::~BrainModelSurfaceMorphing()
{
   if (getImAThread() == false) {
      if (coordsArray1 != NULL) {
         delete[] coordsArray1;
      }
      if (coordsArray2 != NULL) {
         delete[] coordsArray2;
      }
      if (morphNodeInfo != NULL) {
         delete[] morphNodeInfo;
      }
      if (nodeShouldBeMorphed != NULL) {
         delete[] nodeShouldBeMorphed;
      }
      for (unsigned int i = 0; i < threads.size(); i++) {
         if (threads[i] != NULL) {
            delete threads[i];
         }
      }
   }
}

/**
 * get morphing parameters
 */
void 
BrainModelSurfaceMorphing::getMorphingParameters(int& iterationsOut,
                                            float& linearForceOut,
                                            float& angularForceOut,
                                            float& stepSizeOut) const
{
   iterationsOut   = iterations;
   linearForceOut  = linearForce;
   angularForceOut = angularForce;
   stepSizeOut     = stepSize;
}
                           
/**
 * set morphing parameters
 */
void 
BrainModelSurfaceMorphing::setMorphingParameters(const int iterationsIn,
                                            const float linearForceIn,
                                            const float angularForceIn,
                                            const float stepSizeIn)
{
   iterations   = iterationsIn;
   linearForce  = linearForceIn;
   angularForce = angularForceIn;
   stepSize     = stepSizeIn;
}
                           
/**
 * execute the morphing
 */
void
BrainModelSurfaceMorphing::execute() throw (BrainModelAlgorithmException)
{
   BrainModelSurface::SURFACE_TYPES surfaceTypeHint = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
   switch (morphingSurfaceType) {
      case MORPHING_SURFACE_FLAT:
         surfaceTypeHint = BrainModelSurface::SURFACE_TYPE_FLAT;
         break;
      case MORPHING_SURFACE_SPHERICAL:
         surfaceTypeHint = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
         break;
      default:
         break;
   }
   
   if (numberOfNodes <= 0) {
      throw BrainModelAlgorithmException("Morphing surface has no nodes.");
   }
   if (numberOfNodes != referenceSurface->getNumberOfNodes()) {
      throw BrainModelAlgorithmException(
         "Reference and Morphing surfaces have a different number of nodes.");
   }
   
   //
   // Morphing surface coordinate file
   //
   CoordinateFile* morphCoordFile = morphingSurface->getCoordinateFile();
   
   //
   // Get the coordinates and load them into arrays
   //
   float* coordsArray1 = new float[numberOfNodes * 3];
   float* coordsArray2 = new float[numberOfNodes * 3];
   for (int i = 0; i < numberOfNodes; i++) {
      morphCoordFile->getCoordinate(i, &coordsArray1[i*3]);
   }
   
   //
   // Set the input and output coord pointers
   //
   inputCoords  = coordsArray1;
   outputCoords = coordsArray2;
   
   //
   // Generate neighbors, lengths, and angles for reference surface
   //
   generateNeighborInformation();
   
   //
   // Save radius of morphing surface in case it is a sphere
   //
   sphericalSurfaceRadius = morphingSurface->getSphericalSurfaceRadius();
   
   //
   // If no iterations (just calculating forces)
   //
   bool forcesOnlyFlag = false;
   if (iterations == 0) {
      forcesOnlyFlag = true;
      iterations = 1;
   }
   
   //
   // If measuring at each iteration
   //
   QFile statsFile("morph_stats.txt");
   bool statsFileValid = false;
   if (doStatisticsEachPass) {
      if (statsFile.open(QIODevice::WriteOnly)) {
         statsFileValid = true;
      }
   }
   
   //
   // Surface shape file for measurements
   //
   SurfaceShapeFile measurementsShapeFile;
   
   //
   // See if threads are being used, and if so, create them.
   //
   const int numberOfThreads = getNumberOfThreadsToRun();
   if (numberOfThreads > 1) {
      //
      // Number of nodes each thread should smooth
      //
      const int numNodesPerThread = numberOfNodes / numberOfThreads;
      int startNode = 0;
      int endNode   = numNodesPerThread;
      for (int i = 0; i < numberOfThreads; i++) {
         //
         // Limit ending node for last thread
         //
         if (i == (numberOfThreads - 1)) {
            endNode = numberOfNodes - 1;
         }
         if (DebugControl::getDebugOn()) {
            std::cout << "Morphing thread " << i << " nodes " 
                      << startNode << " " << endNode << std::endl;
         }
         
         //
         // Create the threads
         //
         BrainModelSurfaceMorphing* bmsm = new BrainModelSurfaceMorphing(
                                                            brainSet,
                                                            referenceSurface,
                                                            morphingSurface,
                                                            morphingSurfaceType,
                                                            morphNodeInfo,
                                                            nodeShouldBeMorphed,
                                                            noMorphNeighborStepSize,
                                                            startNode,
                                                            endNode,
                                                            sphericalSurfaceRadius,
                                                            this,
                                                            i);
         bmsm->setMorphingParameters(iterations, linearForce, angularForce, stepSize);
         threads.push_back(bmsm);  
         
         //
         // Determine node indices which each thread should smooth
         //
         startNode = endNode + 1;
         endNode   += numNodesPerThread;
      }
   }
   
   //
   // morph for the surface
   //
   for (int i = 1; i <= iterations; i++) {
      const bool firstIterationFlag = (i == 1);
      const bool lastIterationFlag = (i == iterations);
      
      //
      // Update surface normals
      //
      //for (int j = 0; j < numberOfNodes; j++) {
      //   morphCoordFile->setCoordinate(j, &inputCoords[j*3]);
      //}
      //morphingSurface->computeNormals();
      morphingSurface->computeNormals(inputCoords);
      //
      // Set the forces on nodes that are NOT being morphed
      //
      if (allNodesBeingMorphed == false) {
         setForcesOnNoMorphNodes();
      }
      
      //
      // If running threads
      //
      if (numberOfThreads > 1) {
         for (int j = 0; j < numberOfThreads; j++) {
            //
            // Run each thread for an iteration of smoothing
            //
            threads[j]->setInputAndOutputCoords(inputCoords, outputCoords);
            
            //
            // Clear the number of child threads done
            //
            resetNumChildThreadDone();
            
            //
            // Start the threads if this is the first iteration
            //
            if (firstIterationFlag) {
               threads[j]->setThreadKeepLoopingFlag(true);
               threads[j]->setThreadedIterationDoneFlag(true);
               threads[j]->start(QThread::TimeCriticalPriority);
            }
         }
         
         //
         // Wait until all threads have started and in their loop.
         // This is need for the case that there is only one iteration.
         //
         while (getNumChildThreadStarted() < numberOfThreads) {
            msleep(1);
         }
         
         for (int j = 0; j < numberOfThreads; j++) {
            //
            // if the last iteration, clear the keep looping flag for the thread
            //
            if (lastIterationFlag ) {
               threads[j]->setThreadKeepLoopingFlag(false);
            }
            
            //
            // wake the thread so that it executes an iteration
            //
            if (DebugControl::getDebugOn()) {
               std::cout << "Allowing morphing thread " << j << " to run." << std::endl;
            }
            threads[j]->setThreadedIterationDoneFlag(false);
         }
         
         //
         // Wait until all of the threads finish
         //
         while (getNumChildThreadDone() < numberOfThreads) {
            msleep(1);
         }
         if (DebugControl::getDebugOn()) {
            std::cout << "All morphing threads completed iteration." << std::endl;
         }
      }
      else {
         //
         // Smooth for one iteration
         //
         setIndicesOfNodesToMorph(0, numberOfNodes - 1);
         run();
      }
      
      //
      // morph one iteration
      //
      //run();
      
      if (DebugControl::getDebugOn()) {
         if ((DebugControl::getDebugNodeNumber() >= 0) &&
             (DebugControl::getDebugNodeNumber() < numberOfNodes)) {
            const int node = DebugControl::getDebugNodeNumber();
            std::cout << "DEBUG iter " << i << " NODE "
                      << node << " coords: " 
                      << outputCoords[node*3] << " "
                      << outputCoords[node*3+1] << " "
                      << outputCoords[node*3+2] << std::endl;
         }
      }
      
      bool coordFileUpdated = false;
      
      //
      // If NOT the last iteration
      //
      if (lastIterationFlag == false) {
         //
         // Update the displayed brain model
         //
         if (brainSet->isIterationUpdate(i)) {
            for (int j = 0; j < numberOfNodes; j++) {
               morphCoordFile->setCoordinate(j, &outputCoords[j*3]);
            }
            brainSet->drawBrainModel(morphingSurface, i);
            coordFileUpdated = true;
         }

         //
         // swap input and output coordinate pointers
         //
         std::swap(inputCoords, outputCoords);
      }

      //
      // If just calculating forces, reset iterations
      //
      if (forcesOnlyFlag) {
         iterations = 0;
      }
      
      //
      // If measuring at each iteration
      //
      if (statsFileValid) {
         //
         // If not just doing forces
         //
         if (forcesOnlyFlag == false) {
            updateStatsFile(statsFile,
                            morphCoordFile,
                            measurementsShapeFile,
                            surfaceTypeHint,
                            i,
                            firstIterationFlag);
         } 
      } 
   }  

   //
   //
   //
   if (statsFileValid) {
      statsFile.close();
   }
   
   //
   // Copy the coordinates back to the morphing surface
   //   
   if (forcesOnlyFlag == false) {
      for (int j = 0; j < numberOfNodes; j++) {
         morphCoordFile->setCoordinate(j, &outputCoords[j*3]);
      }
   }
   
   //
   // Copy the force vectors
   //
   BrainSetNodeAttribute* attributes = brainSet->getNodeAttributes(0);
   for (int i = 0; i < numberOfNodes; i++) {
      attributes[i].setMorphingForces(morphNodeInfo[i].linearForce,
                              morphNodeInfo[i].angularForce,
                              morphNodeInfo[i].totalForce);
   }
   if (DebugControl::getDebugOn()) {
      const int node = DebugControl::getDebugNodeNumber();
      if (node >= 0) {
         int start = node - 5;
         int end = node + 5;
         if (start < 0) {
            start = start - start;
            end = start + 10;
         }
         if (end >= numberOfNodes) {
            end = numberOfNodes - 1;
         }
         for (int j = start; j < end; j++) {
            std::cout << j << ": Forces (L, A): " 
                        << morphNodeInfo[j].linearForce[0] << " "
                        << morphNodeInfo[j].linearForce[1] << " "
                        << morphNodeInfo[j].linearForce[2] << "  "
                        << morphNodeInfo[j].angularForce[0] << " "
                        << morphNodeInfo[j].angularForce[1] << " "
                        << morphNodeInfo[j].angularForce[2] << " "
                        << std::endl;
         }
      }
   }
}

/**
 * Update the statistics file used for testing
 */
void
BrainModelSurfaceMorphing::updateStatsFile(QFile& statsFile,
                                           CoordinateFile* morphCoordFile,
                                           SurfaceShapeFile& measurementsShapeFile,
                                           BrainModelSurface::SURFACE_TYPES surfaceTypeHint,
                                           const int iterationNumber,
                                           const bool firstIterationFlag)
{
   //
   // Add columns to surface shape file if first time
   //
   if (measurementsShapeFile.getNumberOfColumns() <= 1) {
      measurementsShapeFile.setNumberOfNodesAndColumns(numberOfNodes, 2);
   }
   
   //
   // Update coordinates since stats taken on surface
   //
   for (int j = 0; j < numberOfNodes; j++) {
      morphCoordFile->setCoordinate(j, &outputCoords[j*3]);
   }
   
   //
   // Determine distortion
   //
   BrainModelSurfaceDistortion bmsd(brainSet,
                                    morphingSurface,
                                    referenceSurface,
                                    morphingSurface->getTopologyFile(),
                                    &measurementsShapeFile,
                                    BrainModelSurfaceDistortion::DISTORTION_COLUMN_CREATE_NEW,
                                    BrainModelSurfaceDistortion::DISTORTION_COLUMN_CREATE_NEW,
                                    "areal-dist",
                                    "linear-dist");
   try {
      //
      // Generate distortion
      //
      bmsd.execute();
      
      //
      // Get areal and linear distortion statistics
      //
      StatisticsUtilities::DescriptiveStatistics arealStats, linearStats;
      bmsd.getArealDistortionStatistics(arealStats);
      bmsd.getLinearDistortionStatistics(linearStats);
      
      //
      // Do a crossover check
      //
      int numTileCrossovers, numNodeCrossovers;
      morphingSurface->crossoverCheck(numTileCrossovers, 
                                       numNodeCrossovers,
                                       surfaceTypeHint);
      
      //
      // Set textstream for file
      //
      QTextStream stream(&statsFile);
      stream.setRealNumberNotation(QTextStream::FixedNotation);
      
      if (firstIterationFlag) {
         stream << ""
                  << "\t" << "Number of"
                  << "\t" << "Areal"
                  << "\t" << "Areal"
                  << "\t" << "Linear"
                  << "\t" << "Linear"
                  << "\n";
         stream << "Iteration"
                  << "\t" << "Tile"
                  << "\t" << "Distortion"
                  << "\t" << "Distortion"
                  << "\t" << "Distortion"
                  << "\t" << "Distortion"
                  << "\n";
         stream << "Number"
                  << "\t" << "Crossovers"
                  << "\t" << "Average"
                  << "\t" << "Deviation"
                  << "\t" << "Average"
                  << "\t" << "Deviation"
                  << "\n";
      }
      
      stream << iterationNumber
               << "\t" << numTileCrossovers
               << "\t" << arealStats.average
               << "\t" << arealStats.standardDeviation
               << "\t" << linearStats.average
               << "\t" << linearStats.standardDeviation
               << "\n";               
   }
   catch (BrainModelAlgorithmException&) {
   }
}

/**
 * Project nodes back to the sphere.
 */
void
BrainModelSurfaceMorphing::projectNodeBackToSphere(const int nodeNumber)
{
   //
   // scale to sphere
   //
   if (morphNodeInfo[nodeNumber].numNeighbors > 0) {
      const int i3 = nodeNumber * 3;
      const float prad = std::sqrt(outputCoords[i3]   * outputCoords[i3] +
                              outputCoords[i3+1] * outputCoords[i3+1] +
                              outputCoords[i3+2] * outputCoords[i3+2]);
      if (prad > 0.0) {
         const float scale = sphericalSurfaceRadius / prad;
         outputCoords[i3]   *= scale;
         outputCoords[i3+1] *= scale;
         outputCoords[i3+2] *= scale;
      }
   }
}

/**
 * Calculate forces on nodes that are NOT BEING MORPHED.
 * This cannot be done inside the threaded execution since these
 * node's forces are needed by other nodes.
 */
void
BrainModelSurfaceMorphing::setForcesOnNoMorphNodes()
{
   //
   // Calculate forces on nodes that are NOT BEING MORPHED
   // This is needed for some "inverse force" calculations needed below
   //
   for (int j = 0; j < numberOfNodes; j++) {
   
      if (nodeShouldBeMorphed[j] == false) {
         //
         // Info for this node and reset forces
         //
         NeighborInformation& nodeInfo = morphNodeInfo[j];
         nodeInfo.resetForces();
         
         if (nodeInfo.numNeighbors > 1) {
         
            const float floatNumNeighbors = static_cast<float>(nodeInfo.numNeighbors);
            //
            // Determine linear forces for node
            //
            if (linearForce > 0.0) {
               for (int k = 0; k < nodeInfo.numNeighbors; k++) {
                  const int n = nodeInfo.neighbors[k];
                  float linearForceComponents[3];
                  computeLinearForce(inputCoords, nodeInfo, j, n, k, linearForceComponents);
                  for (int i = 0; i < 3; i++) {
                     nodeInfo.totalForce[i] +=
                        linearForceComponents[i] / floatNumNeighbors;
                     nodeInfo.linearForce [i] +=
                        linearForceComponents[i] / floatNumNeighbors;
                  }
                  
               }
            }
            
            // Apply angular forces to node
            //
            if (angularForce > 0.0) {
               if (nodeInfo.classification == BrainSetNodeAttribute::CLASSIFICATION_TYPE_CORNER) {
                  float angularForceComponents[3];
                  computeAngularForce(inputCoords, nodeInfo, 0, angularForceComponents);
                  
                  for (int i = 0; i < 3; i++){
                     nodeInfo.totalForce[i] +=
                              angularForceComponents[i] / (floatNumNeighbors - 1.0);
                     nodeInfo.angularForce[i] +=
                              angularForceComponents[i] / (floatNumNeighbors - 1.0);
                  }
               }
               else {
                  for (int k = 0; k < nodeInfo.numNeighbors; k++){
                     float angularForceComponents[3];
                     computeAngularForce(inputCoords, nodeInfo, k, angularForceComponents);
                     for (int i = 0; i < 3; i++){
                        nodeInfo.totalForce[i] +=
                                 angularForceComponents[i] / floatNumNeighbors;
                        nodeInfo.angularForce[i] +=
                                 angularForceComponents[i] / floatNumNeighbors;
                     }
                  }
               }
            } // if (angularForce > 0.0)
         }
      }
   }
   
}

/**
 * Morph the surface
 */
void
BrainModelSurfaceMorphing::run()
{
   //
   // set looping flag for non-threaded execution so that we do an iteration
   //
   if (getImAThread() == false) {
      setThreadKeepLoopingFlag(true);
   }
   
   //
   // The threaded execution will stay in the loop since run only gets called once
   //
   while (getThreadKeepLoopingFlag()) {
   
      //
      // Is this a threaded instance
      //
      if (getImAThread()) {
         //
         // let parent know that this thread has started
         //
         getParentOfThisThread()->incrementNumChildThreadStarted();
         
         //
         // Wait until parent says it is okay to go
         //
         while (getThreadedIterationDoneFlag()) {
            msleep(1);
         }
         if (DebugControl::getDebugOn()) {
            std::cout << "Morphing Thread " << getThreadNumber() << " now executing." << std::endl;
         }
      }
      
      //
      // morph each node that should be morphed
      //
      for (int j = startNodeIndex; j <= endNodeIndex; j++) {
         //
         // Save node position
         //
         float nodePos[3] = { inputCoords[j*3], inputCoords[j*3+1], inputCoords[j*3+2] };
         outputCoords[j*3]   = nodePos[0];
         outputCoords[j*3+1] = nodePos[1];
         outputCoords[j*3+2] = nodePos[2];
         
         //
         // Info for this node
         //
         NeighborInformation& nodeInfo = morphNodeInfo[j];
         
         //
         // If this node has neighbors and should be morphed
         //
         if ((nodeInfo.numNeighbors > 1) && nodeShouldBeMorphed[j]) {
            //
            // Initialize forces to zero
            //
            nodeInfo.resetForces();
            
            float linearForceComponents[3];
            float angularForceComponents[3];
         
            const float floatNumNeighbors = static_cast<float>(nodeInfo.numNeighbors);
            //
            // Apply linear forces to node
            //
            if (linearForce > 0.0) {
               for (int k = 0; k < nodeInfo.numNeighbors; k++) {
                  const int n = nodeInfo.neighbors[k];
                  computeLinearForce(inputCoords, nodeInfo, j, n, k, linearForceComponents);
                  for (int i = 0; i < 3; i++) {
                     nodeInfo.totalForce[i] +=
                        linearForceComponents[i] / floatNumNeighbors;
                     nodeInfo.linearForce[i] +=
                        linearForceComponents[i] / floatNumNeighbors;
                  
                     //
                     // If this neighbor is not a morphable node
                     //
                     if (nodeShouldBeMorphed[n] == false) {
                        //
                        //  Add the inverse of the neighbor's linear force
                        //
                        nodeInfo.totalForce[i]  -= (noMorphNeighborStepSize * 
                                                   morphNodeInfo[n].linearForce[i])
                                                   / floatNumNeighbors;
                        nodeInfo.linearForce[i] -= (noMorphNeighborStepSize * 
                                                   morphNodeInfo[n].linearForce[i])
                                                   / floatNumNeighbors;
                     }
                  }
               }
   
               if (DebugControl::getDebugOn()) {
                  if (DebugControl::getDebugNodeNumber() == j) {
                     std::cout << std::endl;
                     std::cout << "Total Linear Force for node: " << j
                              << "(" << nodeInfo.linearForce[0]
                              << ", " << nodeInfo.linearForce[1]
                              << ", " << nodeInfo.linearForce[2] << ")" << std::endl;
                     std::cout << std::endl;
                  }
               }
            }
               
            //
            // Apply angular forces to node
            //
            if (angularForce > 0.0) {
               if (nodeInfo.classification == BrainSetNodeAttribute::CLASSIFICATION_TYPE_CORNER) {
                  computeAngularForce(inputCoords, nodeInfo, 0, angularForceComponents);
                  for (int i = 0; i < 3; i++){
                     nodeInfo.totalForce[i] +=
                              angularForceComponents[i] / (floatNumNeighbors - 1.0);
                     nodeInfo.angularForce[i] +=
                              angularForceComponents[i] / (floatNumNeighbors - 1.0);
                  }
               }
               else {
                  for (int k = 0; k < nodeInfo.numNeighbors; k++){
                     const int n = nodeInfo.neighbors[k];
                     computeAngularForce(inputCoords, nodeInfo, k, angularForceComponents);
                     for (int i = 0; i < 3; i++){
                        nodeInfo.totalForce[i] +=
                                 angularForceComponents[i] / floatNumNeighbors;
                        nodeInfo.angularForce[i] +=
                                 angularForceComponents[i] / floatNumNeighbors;
                  
                        //
                        // If this neighbor is not a morphable node
                        //
                        if (nodeShouldBeMorphed[n] == false) {
                           //
                           //  Add the inverse of the neighbor's linear force
                           //
                           nodeInfo.totalForce[i]   -= (noMorphNeighborStepSize * 
                                                      morphNodeInfo[n].angularForce[i])
                                                      / floatNumNeighbors;
                           nodeInfo.angularForce[i] -= (noMorphNeighborStepSize * 
                                                      morphNodeInfo[n].angularForce[i])
                                                      / floatNumNeighbors;
                        }
                     }
                  }
               }
            } // if (angularForce > 0.0)
            
            //
            // Adjust forces during spherical morphing
            //
            if (morphingSurfaceType == MORPHING_SURFACE_SPHERICAL) {
               mapForcesToPlane(nodePos, nodeInfo.totalForce);
               mapForcesToPlane(nodePos, nodeInfo.angularForce);
               mapForcesToPlane(nodePos, nodeInfo.linearForce);
            }
            
            //
            // Add forces and store in temporary node positions
            //
            outputCoords[j*3]   = nodePos[0] + stepSize * nodeInfo.totalForce[0];
            outputCoords[j*3+1] = nodePos[1] + stepSize * nodeInfo.totalForce[1];
            outputCoords[j*3+2] = nodePos[2] + stepSize * nodeInfo.totalForce[2];
            
         }  // if (nodeInfo.numNeighbors > 1)
         
         //
         // Project back to sphere if morphing a sphere
         //
         switch (morphingSurfaceType) {
            case MORPHING_SURFACE_FLAT:
               break;
            case MORPHING_SURFACE_SPHERICAL:
               projectNodeBackToSphere(j);
               break;
         }
         
      } // for (j = 0...
      
      if (getImAThread()) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Morphing Thread " << getThreadNumber() << " finished iteration." << std::endl;
         }
         //
         // A threaded instance needs to tell that it has completed an iteration
         //
         setThreadedIterationDoneFlag(true);
         getParentOfThisThread()->incrementNumChildThreadDone();
      }
      else {
         //
         // A non-threaded instance needs to return
         //
         setThreadKeepLoopingFlag(false);
      }
   } // while (keepLooping)
}

/**
 * Map forces to a plane (used in spherical morphing).
 */
void
BrainModelSurfaceMorphing::mapForcesToPlane(const float nodeXYZ[3],
                                            float force[3])
{
   //
   // Compute the node's normal
   //
   float nodeNormal[3] = { nodeXYZ[0], nodeXYZ[1], nodeXYZ[2] };
   MathUtilities::normalize(nodeNormal);
   
   //
   // angle between normal and force
   //
   const float dot = MathUtilities::dotProduct(nodeNormal, force);
   
   //
   // Adjust force
   //
   force[0] = force[0] - (dot * nodeNormal[0]);
   force[1] = force[1] - (dot * nodeNormal[1]);
   force[2] = force[2] - (dot * nodeNormal[2]);
}

/**
 * Compute the angular force on a node by its neighbor.
 */
void
BrainModelSurfaceMorphing::computeAngularForce(const float* coords,
                                                const NeighborInformation& nodeInfo,
                                                const int neighborIndex,
                                                float forceOut[3])
{
   //
   // Clear forces
   //
   forceOut[0] = 0.0;
   forceOut[1] = 0.0;
   forceOut[2] = 0.0; 
   
   if (nodeInfo.numNeighbors < 1) {
      return;
   }
   
   //
   // Indices of neighboring nodes
   //
   const int neighborNodeNumber = nodeInfo.neighbors[neighborIndex];
   int nextNeighborIndex = neighborIndex + 1;
   if (nextNeighborIndex >= nodeInfo.numNeighbors) nextNeighborIndex = 0;
   const int nextNeighborNodeNumber = nodeInfo.neighbors[nextNeighborIndex];
   
   //
   // coordinates of this node and the neighbors
   //
   const float* nodeCoords = &coords[nodeInfo.nodeNumber * 3];
   const float* neighborCoords = &coords[neighborNodeNumber * 3];
   const float* nextNeighborCoords = &coords[nextNeighborNodeNumber * 3];
   
   //
   // Compute the normal for the triangle
   //
   float triangleNormal[3];
   MathUtilities::computeNormal((float*)nodeCoords, (float*)neighborCoords, 
                              (float*)nextNeighborCoords, triangleNormal);
  
   //
   // Check for crossovers
   //
   bool crossover = false;
   switch (morphingSurfaceType) {
      case MORPHING_SURFACE_FLAT:
         if (triangleNormal[2] < 0.0) {
            crossover = true;
         }
         break;
      case MORPHING_SURFACE_SPHERICAL:
         {
            float avgNormal[3] = { 
               (nodeCoords[0] + neighborCoords[0] + nextNeighborCoords[0]) / 3.0,
               (nodeCoords[1] + neighborCoords[1] + nextNeighborCoords[1]) / 3.0,
               (nodeCoords[2] + neighborCoords[2] + nextNeighborCoords[2]) / 3.0 
            };
            if (MathUtilities::normalize(avgNormal) > 0.0) {
               const float dot = MathUtilities::dotProduct(avgNormal, triangleNormal);
               if (dot < 0.0) {
                  crossover = true;
               }
            }
         }
         break;
   }
   crossover = false; //*********************************************************
   if (crossover) {
      triangleNormal[0] = -triangleNormal[0];
      triangleNormal[1] = -triangleNormal[1];
      triangleNormal[2] = -triangleNormal[2];
   }
   
   //
   // Angle at neighbor
   //
   const float angle1 = MathUtilities::angle(nodeCoords, neighborCoords, nextNeighborCoords);
   
   //
   // angle difference
   //
   float angleDiff1 = nodeInfo.angle1[neighborIndex] - angle1;
   if (crossover) {
      angleDiff1 = nodeInfo.angle1[neighborIndex] + angle1;
   }
   
   //
   // distance components and distance to neighbor
   //
   float distanceComponents1[3];
   MathUtilities::subtractVectors(neighborCoords, nodeCoords, distanceComponents1);
   const float distance1 = MathUtilities::vectorLength(distanceComponents1);
   
   //
   // Amount (magnitude) node should be moved
   //
   const float mag1 = distance1 * std::sin(angleDiff1);
   
   //
   // Determine vector of node movement
   //
   float direction1[3];
   
   //
   // Determine a vector that points orthogonally from the plane defined
   // by the tile's normal vector and the tiles edge (vector perpendicular to
   // edge pointing outside the tringle but in the triangle's plane).
   //
   MathUtilities::normalizedCrossProduct(distanceComponents1, triangleNormal, direction1);
             
   if (MathUtilities::vectorLength(direction1) > 0.0) {
      float force[3];
      force[0] =  angularForce * mag1 * direction1[0];
      force[1] =  angularForce * mag1 * direction1[1];
      force[2] =  angularForce * mag1 * direction1[2];
      if (crossover) {
         force[0] = -force[0];
         force[1] = -force[1];
         force[2] = -force[2];
      }
      
      forceOut[0] +=  force[0];
      forceOut[1] +=  force[1];
      forceOut[2] +=  force[2];
      
      if (DebugControl::getDebugOn()) {
         const int node = DebugControl::getDebugNodeNumber();
         if (node == nodeInfo.nodeNumber) {
            std::cout << "Morphing Debugging Node: " << node << std::endl;
            std::cout << "Neighbor: " << neighborNodeNumber << std::endl;
            std::cout << "Angle (Fiducial, surface): " 
                      << (nodeInfo.angle1[neighborIndex] * MathUtilities::radiansToDegrees()) 
                      << " " << (angle1  * MathUtilities::radiansToDegrees())
                      << std::endl;
            std::cout << "Force: " << force[0] << ", "
                                   << force[1] << ", "
                                   << force[2] << std::endl;
         }
      }   
   }
   
   //---------------------------------------------------------------------------------------
  
   //
   // Angle at neighbor
   //
   const float angle2 = MathUtilities::angle(neighborCoords, nextNeighborCoords, nodeCoords);
   
   //
   // angle difference
   //
   float angleDiff2 = nodeInfo.angle2[neighborIndex] - angle2;
   if (crossover) {
      angleDiff2 = nodeInfo.angle2[neighborIndex] + angle2;
   }
   
   //
   // distance components and distance to neighbor
   //
   float distanceComponents2[3];
   MathUtilities::subtractVectors(nextNeighborCoords, nodeCoords, distanceComponents2);
   const float distance2 = MathUtilities::vectorLength(distanceComponents2);
   
   //
   // Amount (magnitude) node should be moved
   //
   const float mag2 = distance2 * std::sin(angleDiff2);
   
   //
   // Determine vector of node movement
   //
   float direction2[3];
   
   //
   // Determine a vector that points orthogonally from the plane defined
   // by the tile's normal vector and the tiles edge (vector perpendicular to
   // edge pointing outside the tringle but in the triangle's plane).
   //
   MathUtilities::normalizedCrossProduct(triangleNormal, distanceComponents2, direction2);
             
   if (MathUtilities::vectorLength(direction2) > 0.0) {
      float force[3];
      force[0] =  angularForce * mag2 * direction2[0];
      force[1] =  angularForce * mag2 * direction2[1];
      force[2] =  angularForce * mag2 * direction2[2];
      if (crossover) {
         force[0] = -force[0];
         force[1] = -force[1];
         force[2] = -force[2];
      }
      
      forceOut[0] +=  force[0];
      forceOut[1] +=  force[1];
      forceOut[2] +=  force[2];
      if (DebugControl::getDebugOn()) {
         const int node = DebugControl::getDebugNodeNumber();
         if (node == nodeInfo.nodeNumber) {
            std::cout << "Morphing Debugging Node: " << node << std::endl;
            std::cout << "Neighbor: " << nextNeighborNodeNumber << std::endl;
            std::cout << "Angle (Fiducial, surface): " 
                      << (nodeInfo.angle2[neighborIndex] * MathUtilities::radiansToDegrees())
                      << " " << (angle2 * MathUtilities::radiansToDegrees())
                       << std::endl;
            std::cout << "Force: " << force[0] << ", "
                                   << force[1] << ", "
                                   << force[2] << std::endl;
         }
      }
   }   
}

/**
 * Compute linear force on a node from a neighbor
 */
void
BrainModelSurfaceMorphing::computeLinearForce(const float* coords,
                                              const NeighborInformation& nodeInfo,
                                              const int nodeNum,
                                              const int neighNodeNum,
                                              const int nodeInfoNeighIndex,
                                              float force[3])
{
   //
   // Clear forces
   //
   force[0] = 0.0;
   force[1] = 0.0;
   force[2] = 0.0;
   
   //
   // positions of node and neighbor
   //
   const float* nodePos = &coords[nodeNum * 3];
   const float* neighPos = &coords[neighNodeNum * 3];
   
   //
   // Distance between node and its neighbor in the surface that is being morphed
   //
   const float neighborDistance = MathUtilities::distance3D(nodePos, neighPos);
   
   //
   // If neighbor is very close do not update forces
   // IS THIS 
   //
//   if (neighborDistance <= 0.000001) {
//      return;
//   }
   
   //
   // distance to neighbor in fiducial surface
   //
   const float neighborFiducialDistance = nodeInfo.neighborDistance[nodeInfoNeighIndex];   
   
   //
   // Difference between node & neighbor in the fiducial and morphing surface (the "error")
   //
   float errorDistance = neighborFiducialDistance - neighborDistance;
   
   //
   // ratio of distances in morphing and fiducial surfaces
   //
   float errorDistRatio = 0.0;
   if (neighborFiducialDistance != 0.0) {
      errorDistRatio = neighborDistance / neighborFiducialDistance;
   }
   
   //
   // if compressed, double error
   //
   if (errorDistRatio < 0.50) {
      errorDistance *= 2.0;
   }
   
   //
   // apply linear force strength
   //
   errorDistance *= linearForce;
   
   //
   // get distance between node and neighbor in X, Y, and Z
   // Note: want to push "this" node in relation to its neighbor
   //
   float errorComponents[3];
   MathUtilities::subtractVectors(nodePos, neighPos, errorComponents);
   
   //
   // Determine the linear force components
   //
   force[0] = errorDistance * errorComponents[0] / neighborDistance;
   force[1] = errorDistance * errorComponents[1] / neighborDistance;
   force[2] = errorDistance * errorComponents[2] / neighborDistance;
   
   if (DebugControl::getDebugOn()) {
      if (DebugControl::getDebugNodeNumber() == nodeNum) {
         std::cout << std::endl;
         std::cout << "Linear Force Calculation" << std::endl;
         std::cout << "Node " << nodeNum
                   << " (" << nodePos[0] << ", " << nodePos[1] << ", " << nodePos[2] << ")" 
                   << std::endl;
         std::cout << "Neighbor " << neighNodeNum 
                   << " (" << neighPos[0] << ", " << neighPos[1] << ", " << neighPos[2] << ")" 
                   << std::endl;
         std::cout << "Fiducial Distance: " << neighborFiducialDistance << std::endl;
         std::cout << "Distance: " << neighborDistance << std::endl;
         std::cout << "Error Distance: " << errorDistance << std::endl;
         std::cout << "Linear Force: (" << force[0] << ", " << force[1] << ", " << force[2] << ")"
                   << std::endl;
      }
   }
}

/**
 * Set the indices of the nodes that are to be morphed (inclusive).
 */
void 
BrainModelSurfaceMorphing::setIndicesOfNodesToMorph(const int startNodeIndexIn, 
                                                    const int endNodeIndexIn)
{
   startNodeIndex = startNodeIndexIn;
   endNodeIndex   = endNodeIndexIn;
}

/**
 * Set the input and output coords
 */
void
BrainModelSurfaceMorphing::setInputAndOutputCoords(float* inCoords, float* outCoords)
{
   inputCoords  = inCoords;
   outputCoords = outCoords;
}

/**
 * Generate neighbor information
 */
void
BrainModelSurfaceMorphing::generateNeighborInformation()
{
   //
   // Use topology from morphing surface
   //
   TopologyFile* tf = morphingSurface->getTopologyFile();

   //
   // Classify the nodes as interior or exterior
   //
   BrainSet* bs = morphingSurface->getBrainSet();
   bs->classifyNodes(tf);
   
   //
   // Get the coordinates from the reference surface
   //
   const CoordinateFile* cf = referenceSurface->getCoordinateFile();
   const float* firstCoord = cf->getCoordinate(0);
   
   //
   // Create a topology helper to get node neighbors
   //
   const TopologyHelper* th = tf->getTopologyHelper(true, true, true);
   
   //
   // See if fiducial sphere ratios should be used
   //
   const bool useFiducialSphereRatios = (numberOfNodes == static_cast<int>(fiducialSphereRatios.size()));
   const float inverseDistortionFraction = 1.0 - sphereFiducialDistortionFraction;

   //
   // Get the surface's brain set and see if this node is interior or edge
   //
   const BrainSetNodeAttribute* allNodeAttributes = brainSet->getNodeAttributes(0);
   
   //
   // Generate neighbors, lengths, and angles for reference surface
   //
   morphNodeInfo = new NeighborInformation[numberOfNodes];
   for (int i = 0; i < numberOfNodes; i++) {
      int numNeighbors;
      const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
      
      morphNodeInfo[i].initialize(firstCoord, i, &allNodeAttributes[i], neighbors, numNeighbors);
      
      //
      // If fiducial sphere ratios should be used
      //
      if (useFiducialSphereRatios) {
         const float myRatio = fiducialSphereRatios[i];
         //
         // Scale neighbor distances by David's magic formula
         //
         for (int j = 0; j < morphNodeInfo[i].numNeighbors; j++) {
            const int neigh = morphNodeInfo[i].neighbors[j];
            const float neighRatio = fiducialSphereRatios[neigh];
            morphNodeInfo[i].neighborDistance[j] = inverseDistortionFraction
                                 + (sphereFiducialDistortionFraction * ((myRatio + neighRatio) * 0.5) 
                                    * morphNodeInfo[i].neighborDistance[j]);
         }
      }
   }
}

/**
 * set the nodes that should be morphred.
 */
void 
BrainModelSurfaceMorphing::setNodesThatShouldBeMorphed(const std::vector<bool>& nodesThatShouldBeMorphedIn,
                                                       const float noMorphStepSizeIn)
{
   allNodesBeingMorphed = true;
   const int num = static_cast<int>(nodesThatShouldBeMorphedIn.size());
   for (int i = 0; i < numberOfNodes; i++) {
      nodeShouldBeMorphed[i] = false;
      if (i < num) {
         nodeShouldBeMorphed[i] = nodesThatShouldBeMorphedIn[i];
      }
      if (nodeShouldBeMorphed[i] == false) {
         allNodesBeingMorphed = false;
      }
   }
   noMorphNeighborStepSize = noMorphStepSizeIn;
}

/**
 * set the fiducial sphere distortion corrections.
 */
void 
BrainModelSurfaceMorphing::setFiducialSphereDistortionCorrections(
                                             const std::vector<float>& fiducialSphereRatiosIn,
                                             const float sphereFiducialDistortionFractionIn)
{
   fiducialSphereRatios = fiducialSphereRatiosIn;
   sphereFiducialDistortionFraction = sphereFiducialDistortionFractionIn;
}                                                  

//--------------------------------------------------------------------------------

/**
 * Constructor.
 */
BrainModelSurfaceMorphing::NeighborInformation::NeighborInformation()
{
   numNeighbors = 0;
   neighbors = NULL;
   neighborDistance = NULL;
   angle1 = NULL;
   angle2 = NULL;
}

/**
 * Destructor.
 */
BrainModelSurfaceMorphing::NeighborInformation::~NeighborInformation()
{
   if (neighbors != NULL) {
      delete neighbors;
   }
   if (neighborDistance != NULL) {
      delete neighborDistance;
   }
   if (angle1 != NULL) {
      delete angle1;
   }
   if (angle2 != NULL) {
      delete angle2;
   }
}

/**
 * Initialize neighbor information
 */
void
BrainModelSurfaceMorphing::NeighborInformation::initialize(const float* coords, 
                                                const int nodeNumberIn,
                                                const BrainSetNodeAttribute* nodeAttribute,
                                                const int* neighborsIn,
                                                const int numNeighborsIn)
{
   //
   // Index of this node
   //
   nodeNumber = nodeNumberIn;
   
   //
   // Set this nodes interior flag
   //
   classification = nodeAttribute->getClassification();
   
   //
   // Allocate items to number of neighbors
   //
   numNeighbors = numNeighborsIn;
   if (numNeighbors > 0) {
      neighbors = new int[numNeighbors];
      for (int i = 0; i < numNeighbors; i++) {
         neighbors[i] = neighborsIn[i];
      }
      neighborDistance = new float[numNeighbors];
      angle1 = new float[numNeighbors];
      angle2 = new float[numNeighbors];
   
      const float* myCoord = &coords[nodeNumber * 3];
      
      if (numNeighbors > 1) {
         //
         // Compute distances and angles to neighbors
         //      
         for (int i = 0; i < numNeighbors; i++) {
            //
            // length to neighbor
            //
            neighborDistance[i] = MathUtilities::distance3D(myCoord, &coords[neighbors[i] * 3]);
            
            //
            // indices for next and previous neighbors
            //
            int nextIndex = i + 1;
            if (nextIndex >= numNeighbors) nextIndex = 0;
            
            //
            // Coordinates of current and previous neighbor
            //
            const float* currNeighborCoord = &coords[neighbors[i] * 3];
            const float* nextNeighborCoord = &coords[neighbors[nextIndex] * 3];
            
            //
            // Compute angles
            //
            if (classification == BrainSetNodeAttribute::CLASSIFICATION_TYPE_CORNER) {
               //
               // Only compute for first neighbor
               //
               if (i == 0) {
                  angle1[0] = MathUtilities::angle(myCoord, currNeighborCoord, nextNeighborCoord);
                  angle2[0] = MathUtilities::angle(currNeighborCoord, nextNeighborCoord, myCoord);
                  angle1[1] = 0.0;
                  angle2[1] = 0.0;
               }
            }
            else {
               //
               // Angle at current neighbor
               //
               angle1[i] = MathUtilities::angle(myCoord, currNeighborCoord, nextNeighborCoord);
               
               //
               // Angle at next neighbor
               //
               angle2[i] = MathUtilities::angle(currNeighborCoord, nextNeighborCoord, myCoord);               
            }
         }
         if (DebugControl::getDebugOn()) {
            if (nodeNumber == DebugControl::getDebugNodeNumber()) {
               std::cout << "\nNode Number : " << nodeNumber << std::endl;
               for (int i = 0; i < numNeighbors; i++) {
                  std::cout << "Neighbor[" << i << "] " << neighbors[i]
                           << " angle1 (radians, degrees): " << angle1[i] 
                           << " " << angle1[i] * MathUtilities::radiansToDegrees() << std::endl
                           << " angle2 (radians, degrees): " << angle2[i] 
                           << " " << angle2[i] * MathUtilities::radiansToDegrees()
                           << std::endl;
               }
            }
         }
      }
   }
}

/**
 * Initialize neighbor information
 */
void
BrainModelSurfaceMorphing::NeighborInformation::resetForces()
{
   totalForce[0]   = 0.0;
   totalForce[1]   = 0.0;
   totalForce[2]   = 0.0;
   linearForce[0]  = 0.0;
   linearForce[1]  = 0.0;
   linearForce[2]  = 0.0;
   angularForce[0] = 0.0;
   angularForce[1] = 0.0;
   angularForce[2] = 0.0;
}

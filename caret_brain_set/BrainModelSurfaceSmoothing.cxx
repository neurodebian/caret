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

#include <cmath>

#include <QDateTime>
#include <QWaitCondition>

#include "vtkTriangle.h"

#define __BRAIN_MODEL_SURFACE_SMOOTHING_MAIN_H__
#include "BrainModelSurfaceSmoothing.h"
#undef __BRAIN_MODEL_SURFACE_SMOOTHING_MAIN_H__

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "BrainSetNodeAttribute.h"
#include "DebugControl.h"
#include "MathUtilities.h"
#include "StringUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

static const int SLEEP_TIME = 1;
static const int SLEEP_TIME_MICROSECONDS = 250;

/**
 * Constructor for the main controller of smoothing.
 */
BrainModelSurfaceSmoothing::BrainModelSurfaceSmoothing(
                                       BrainSet* bs,
                                       BrainModelSurface* surfaceIn,
                                       const SMOOTHING_TYPE smoothingTypeIn,
                                       const float strengthIn,
                                       const int   iterationsIn,
                                       const int   edgeIterationsIn,
                                       const int   landmarkNeighborIterationsIn,
                                       const std::vector<bool>* smoothOnlyTheseNodesIn,
                                       const std::vector<bool>* landmarkNodeFlagsIn,
                                       const int projectToSphereEveryXIterationsIn,
                                       const int numberOfThreadsIn)
   : BrainModelAlgorithmMultiThreaded(bs, NULL, -1, false)
{
   initialize();
   surface        = surfaceIn;
   smoothingType  = smoothingTypeIn;
   strength       = strengthIn;
   iterations     = iterationsIn;
   edgeIterations = edgeIterationsIn;
   landmarkNeighborIterations = landmarkNeighborIterationsIn;
   
   const int numNodes = surfaceIn->getNumberOfNodes();
   if (numNodes > 0) {
      nodeInfo = new NodeInfo[numNodes];
   }
   if (smoothOnlyTheseNodesIn != NULL) {
      const int smoothOnlyTheseNodesCount = static_cast<int>(smoothOnlyTheseNodesIn->size());
      if (smoothOnlyTheseNodesCount > 0) {
         for (int i = 0; i < smoothOnlyTheseNodesCount; i++) {
            if ((*smoothOnlyTheseNodesIn)[i] == false) {
               nodeInfo[i].nodeType = NodeInfo::NODE_TYPE_DO_NOT_SMOOTH;
            }
         }
      }
   }
   
   //
   // Process landmarks and determine landmark neighbors
   //
   if (landmarkNodeFlagsIn != NULL) {
      const int nodeCount = static_cast<int>(landmarkNodeFlagsIn->size());
      CoordinateFile* cf = surface->getCoordinateFile();
      const float* coords = cf->getCoordinate(0);
      const TopologyFile* topology = surface->getTopologyFile();
      const TopologyHelper* th = topology->getTopologyHelper(false, true, true);
      //const TopologyHelper th(topology, false, true, true);
      if (nodeCount > 0) {
         bool haveLandmarkScale = false;
         for (int i = 0; i < nodeCount; i++) {
            //
            // Get the neighbors for this node
            //
            int numNeighbors = 0;
            const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
               
            //
            // Is this a landmark node ?
            //
            if ((*landmarkNodeFlagsIn)[i]) {
               nodeInfo[i].nodeType = NodeInfo::NODE_TYPE_LANDMARK;
               
               if (smoothingType == SMOOTHING_TYPE_LANDMARK_NEIGHBOR_CONSTRAINED) {
                  //
                  // Determine offset of this landmark from its neighbors
                  //
                  float neighborSum[3] = { 0.0, 0.0, 0.0 };
                  for (int j = 0; j < numNeighbors; j++) {
                     const int n = neighbors[j];
                     neighborSum[0] += coords[n*3];
                     neighborSum[1] += coords[n*3+1];
                     neighborSum[2] += coords[n*3+2];
                  }
                  const float floatNeighbors = numNeighbors;
                  nodeInfo[i].offset[0] = coords[i*3] -   (neighborSum[0] / floatNeighbors);
                  nodeInfo[i].offset[1] = coords[i*3+1] - (neighborSum[1] / floatNeighbors);
                  nodeInfo[i].offset[2] = coords[i*3+2] - (neighborSum[2] / floatNeighbors);
               }
            }
            else {
               //
               //
               // see if any of its neighbors are landmark nodes
               for (int j = 0; j < numNeighbors; j++) {
                  const int n = neighbors[j];
                  if ((*landmarkNodeFlagsIn)[n]) {
                     nodeInfo[i].numLandmarkNeighbors++;
                  }
                  else {
                     if (haveLandmarkScale == false) {
                        haveLandmarkScale = true;
                        landmarkScale = MathUtilities::distance3D(cf->getCoordinate(i),
                                                                  cf->getCoordinate(n)) / std::sqrt(3.0);
                     }
                  }
               }
               if (nodeInfo[i].numLandmarkNeighbors > 0) {
                  nodeInfo[i].nodeType = NodeInfo::NODE_TYPE_LANDMARK_NEIGHBOR;
               }
            }
         }
      }

      //std::cout << "LandmarkScale=" << landmarkScale << std::endl;
      
      //
      // Special stuff for landmark neighbor constrained smoothing
      //
      if (smoothingType == SMOOTHING_TYPE_LANDMARK_NEIGHBOR_CONSTRAINED) {
         for (int i = 0; i < numNodes; i++) {
            //
            // Is this a landmark neighbor
            //
            if (nodeInfo[i].nodeType == NodeInfo::NODE_TYPE_LANDMARK_NEIGHBOR) {
               //
               // Get the neighbors for this node
               //
               int numNeighbors = 0;
               const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
                  
               //
               // Get average of landmark neighbor offsets
               //
               float avg[3] = { 0.0, 0.0, 0.0 };
               for (int j = 0; j < numNeighbors; j++) {
                  const int n = neighbors[j];
                  if (nodeInfo[n].nodeType == NodeInfo::NODE_TYPE_LANDMARK) {
                     avg[0] += nodeInfo[n].offset[0];
                     avg[1] += nodeInfo[n].offset[1];
                     avg[2] += nodeInfo[n].offset[2];
                  }
               }
               
               //
               // Offset this landmark neighbor
               //
               const float floatNumLandmarkNeighbors = nodeInfo[i].numLandmarkNeighbors;
               float xyz[3];
               cf->getCoordinate(i, xyz);
               xyz[0] += (avg[0] / floatNumLandmarkNeighbors);
               xyz[1] += (avg[1] / floatNumLandmarkNeighbors);
               xyz[2] += (avg[2] / floatNumLandmarkNeighbors);
               cf->setCoordinate(i, xyz);
            }
         }         
      }
      
   }  // if (landmarkNodeFlagsIn != NULL)
   
   projectToSphereEveryXIterations = projectToSphereEveryXIterationsIn;
   
   setNumberOfThreadsToRun(numberOfThreadsIn);
}

/**
 * Constructor (protected) used to created threaded executions.
 */
BrainModelSurfaceSmoothing::BrainModelSurfaceSmoothing(BrainSet* bs,
                                       const SMOOTHING_TYPE smoothingTypeIn,
                                       const float strengthIn,
                                       const float landmarkScaleIn,
                                       NodeInfo* nodeInfoIn,
                                       TopologyHelper* topologyHelperIn,
                                       const int startNodeIndexIn,
                                       const int endNodeIndexIn,
                                       BrainModelSurfaceSmoothing* parentOfThisThreadIn,
                                       const int threadNumberIn)
   : BrainModelAlgorithmMultiThreaded(bs, parentOfThisThreadIn, threadNumberIn, true)
{
   initialize();
   
   smoothingType        = smoothingTypeIn;
   strength             = strengthIn;
   inverseStrength      = 1.0 - strength;
   nodeInfo             = nodeInfoIn;
   topologyHelper       = topologyHelperIn;
   startNodeIndex       = startNodeIndexIn;
   endNodeIndex         = endNodeIndexIn;
   landmarkScale        = landmarkScaleIn;
}

/**
 * Destructor.
 */
BrainModelSurfaceSmoothing::~BrainModelSurfaceSmoothing()
{
   if (getImAThread() == false) {
      if (coordsArray1 != NULL) {
         delete[] coordsArray1;
      }
      if (coordsArray2 != NULL) {
         delete[] coordsArray2;
      }
      if (topologyHelper != NULL) {
      //   delete topologyHelper;
      }
      if (nodeInfo != NULL) {
         delete[] nodeInfo;
      }
      for (unsigned int i = 0; i < threads.size(); i++) {
         if (threads[i] != NULL) {
            delete threads[i];
         }
      }
   }
}

/**
 * Initialize member variables.
 */
void
BrainModelSurfaceSmoothing::initialize()
{
   coordsArray1     = NULL;
   coordsArray2     = NULL;
   topologyHelper   = NULL;
   nodeInfo         = NULL;
   landmarkScale    = 1.0;
}

/**
 * Execute the algorithm.
 */
void 
BrainModelSurfaceSmoothing::execute() throw (BrainModelAlgorithmException)
{
   if (surface == NULL) {
      throw BrainModelAlgorithmException("Surfaced is invalid (NULL).");
   }
   numberOfNodes = surface->getNumberOfNodes();
   if (numberOfNodes <= 0) {
      throw BrainModelAlgorithmException("Surface has no nodes to smooth.");
   }
   
   //
   // No need to do anything if iterations is zero
   //
   if (iterations <= 0) {
      return;
   }
     
   //
   // Update node classification
   //
   TopologyFile* topology = surface->getTopologyFile();
   brainSet->classifyNodes(topology, true);
   
   inverseStrength = 1.0 - strength;
   
   if (numberOfNodes <= 0) {
      throw BrainModelAlgorithmException("Surface has no nodes");
   }
   
   //
   // Timer to time entire operation
   //
   QTime timer;
   timer.start();
   
/*
   switch(smoothingType) {
      case SMOOTHING_TYPE_AREAL:
         surface->appendToCoordinateFileComment("Areal ");
         break;
      case SMOOTHING_TYPE_LINEAR:
         surface->appendToCoordinateFileComment("Linear ");
         break;
   }
   surface->appendToCoordinateFileComment(" Smoothing: ");
   surface->appendToCoordinateFileComment(StringUtilities::fromNumber(strength));
   surface->appendToCoordinateFileComment(" ");
   surface->appendToCoordinateFileComment(StringUtilities::fromNumber(iterations));
   surface->appendToCoordinateFileComment(" ");
   surface->appendToCoordinateFileComment(StringUtilities::fromNumber(edgeIterations));
   surface->appendToCoordinateFileComment("\n");
*/
   
   //
   // Get radius in event it is a sphere
   //
   const float sphereRadius = surface->getSphericalSurfaceRadius();
   
   //
   // Topology helper for node neighbors
   //
   //topologyHelper = new TopologyHelper(topology, false, true, true);
   topologyHelper = (TopologyHelper*)topology->getTopologyHelper(false, true, true);
   if (DebugControl::getDebugOn()) {
      std::cout << "Topology Helper time: " << (static_cast<float>(timer.elapsed()) / 1000.0) << std::endl;
   }
   
   //
   // Flag for interior/edge nodes
   //
   for (int i = 0; i < numberOfNodes; i++) {
      const BrainSetNodeAttribute* bna = brainSet->getNodeAttributes(i);
      nodeInfo[i].edgeNodeFlag = ! (bna->getClassification() == 
                                    BrainSetNodeAttribute::CLASSIFICATION_TYPE_INTERIOR);
   }
   
   //
   // Get the coordinates and load them into arrays
   //
   float* coordsArray1 = new float[numberOfNodes * 3];
   float* coordsArray2 = new float[numberOfNodes * 3];
   CoordinateFile* coordFile = surface->getCoordinateFile();
   for (int i = 0; i < numberOfNodes; i++) {
      coordFile->getCoordinate(i, &coordsArray1[i*3]);
   }
   
   //
   // Set the input and output coord pointers
   //
   inputCoords  = coordsArray1;
   outputCoords = coordsArray2;
   
   int smoothNeighborCounter = 1;
   
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
            std::cout << "Smoothing thread " << i << " nodes " 
                      << startNode << " " << endNode << std::endl;
         }
         
         //
         // Create the threads
         //
         BrainModelSurfaceSmoothing* bmss = new BrainModelSurfaceSmoothing(
                                                            brainSet,
                                                            smoothingType,
                                                            strength,
                                                            landmarkScale,
                                                            nodeInfo,
                                                            topologyHelper,
                                                            startNode,
                                                            endNode,
                                                            this,
                                                            i);
         threads.push_back(bmss);  
         
         //
         // Determine node indices which each thread should smooth
         //
         startNode = endNode + 1;
         endNode   += numNodesPerThread;
      }
   }
   
   //
   // Smooth the specified number of iterations
   //
   for (int i = 1; i <= iterations; i++) {
      const bool firstIterationFlag = (i == 1);
      const bool lastIterationFlag  = (i == iterations);
      //
      // See if edges should be smoothed this iteration
      //
      smoothEdgesThisIteration = false;
      if (edgeIterations > 0) {
         if ((i % edgeIterations) == 0) {
            smoothEdgesThisIteration = true;
         }
      }

      //
      // See if landmark neighbors should be smoothed this iteration
      //      
      smoothLandmarkNeighborsThisIteration = false;
      if (smoothNeighborCounter == landmarkNeighborIterations) {
         smoothLandmarkNeighborsThisIteration = true;
         smoothNeighborCounter = 1;
      }
      else {
         smoothNeighborCounter++;
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
            threads[j]->setSmoothEdgesThisIteration(smoothEdgesThisIteration);
            threads[j]->setSmoothLandmarkNeighborsThisIteration(smoothLandmarkNeighborsThisIteration);
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
/*            
            //
            // if the last iteration clear the keep looping flag
            //
            if (lastIterationFlag ) {
               threads[j]->setThreadKeepLoopingFlag(false);
            }
*/
         }
         
         //
         // Wait until all threads have started and in their loop.
         // This is need for the case that there is only one iteration.
         //
         while (getNumChildThreadStarted() < numberOfThreads) {
            //msleep(SLEEP_TIME);
            usleep(SLEEP_TIME_MICROSECONDS);
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
               std::cout << "Allowing smoothing thread " << j << " to run." << std::endl;
            }
            threads[j]->setThreadedIterationDoneFlag(false);
         }
         
         //
         // Wait until all of the threads finish
         //
         while (getNumChildThreadDone() < numberOfThreads) {
            //msleep(SLEEP_TIME);
            usleep(SLEEP_TIME_MICROSECONDS);
         }
         if (DebugControl::getDebugOn()) {
            std::cout << "All smoothing threads completed iteration." << std::endl;
         }
      }
      else {
         //
         //
         // Smooth for one iteration
         //
         setIndicesOfNodesToSmooth(0, numberOfNodes - 1);
         run();
      }
      
      //
      // If the surface should be projected to a sphere
      //
      if (projectToSphereEveryXIterations > 0) {
         if ((i % projectToSphereEveryXIterations) == 0) {
            for (int j = 0; j < numberOfNodes; j++) {
               MathUtilities::setVectorLength(&outputCoords[j*3], sphereRadius);
            }
         }
      }
      
      //
      // If NOT the last iteration
      //
      if (lastIterationFlag == false) {
         //
         // Update the displayed brain model
         //
         if (brainSet->isIterationUpdate(i)) {
            for (int j = 0; j < numberOfNodes; j++) {
               coordFile->setCoordinate(j, &outputCoords[j*3]);
            }
            brainSet->drawBrainModel(surface, i);
         }
         
         //
         // swap input and output coordinate pointers
         //
         std::swap(inputCoords, outputCoords);
      }
   }
   
   //
   // copy the smoothed coordinates back to the surface
   //
   for (int i = 0; i < numberOfNodes; i++) {
      coordFile->setCoordinate(i, &outputCoords[i*3]);
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Total smoothing time: " << (static_cast<float>(timer.elapsed()) / 1000.0) << std::endl;
   }
}

/**
 * smooths for an iteration (required by QThread).
 */
void 
BrainModelSurfaceSmoothing::run()
{
   const int maxNeighbors = topologyHelper->getMaximumNumberOfNeighbors();
   if (maxNeighbors <= 0) {
      return;
   }
   
   float* tileAreas = new float[maxNeighbors];
   float* tileCenters = new float[maxNeighbors * 3];
   
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
            //msleep(SLEEP_TIME);
            usleep(SLEEP_TIME_MICROSECONDS);
         }
         if (DebugControl::getDebugOn()) {
            std::cout << "Smoothing Thread " << getThreadNumber() << " now executing." << std::endl;
         }
      }
      
      for (int i = startNodeIndex; i <= endNodeIndex; i++) {
         const int ix = i * 3;
         const int iy = ix + 1;
         const int iz = iy + 1;
         
         outputCoords[ix] = inputCoords[ix];
         outputCoords[iy] = inputCoords[iy];
         outputCoords[iz] = inputCoords[iz];
         
         //
         // Determine if this node should be smoothed
         //
         bool smoothIt = true;
         if (nodeInfo[i].edgeNodeFlag) {
            smoothIt = smoothEdgesThisIteration;
         }
         
         //
         // Special cases of smoothing
         //
         switch (nodeInfo[i].nodeType) {
            case NodeInfo::NODE_TYPE_DO_NOT_SMOOTH:
               smoothIt = false;
               break;
            case NodeInfo::NODE_TYPE_NORMAL:
               break;
            case NodeInfo::NODE_TYPE_LANDMARK:
               smoothIt = false;
               break;
            case NodeInfo::NODE_TYPE_LANDMARK_NEIGHBOR:
               if (smoothingType == SMOOTHING_TYPE_LANDMARK_NEIGHBOR_CONSTRAINED) {
                  smoothIt = smoothLandmarkNeighborsThisIteration;
               }
               if (smoothingType == SMOOTHING_TYPE_LANDMARK_CONSTRAINED) {
                  smoothIt = false;
                  
                  //
                  // Get the neighbors for this node
                  //
                  int numNeighbors = 0;
                  const int* neighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
                  
                  if (numNeighbors > 2) {
                     //
                     // Determine average of neighbor coordinates
                     //
                     float neighAvg[3] = { 0.0, 0.0, 0.0 };
                     for (int j = 0; j < numNeighbors; j++) {
                        const int n = neighbors[j];
                        neighAvg[0] += inputCoords[n*3];
                        neighAvg[1] += inputCoords[n*3+1];
                        neighAvg[2] += inputCoords[n*3+2];
                     }
                     const float floatNumNeigh = numNeighbors;
                     neighAvg[0] /= floatNumNeigh;
                     neighAvg[1] /= floatNumNeigh;
                     neighAvg[2] /= floatNumNeigh;
                     
                     //
                     // Check each neighbor
                     //
                     for (int k = 0; k < numNeighbors; k++) {
                        //
                        // Is the neighbor a landmark
                        //
                        const int neigh = neighbors[k];
                        if (nodeInfo[neigh].nodeType == NodeInfo::NODE_TYPE_LANDMARK) {
                           //
                           // Get next and previous neighbors
                           //
                           int prevNeighIndex = k - 1;
                           if (prevNeighIndex < 0) {
                              prevNeighIndex = numNeighbors - 1;
                           }
                           const int neighA = neighbors[prevNeighIndex];
                           int nextNeighIndex = k + 1;
                           if (nextNeighIndex >= numNeighbors) {
                              nextNeighIndex = 0;
                           }
                           const int neighB = neighbors[nextNeighIndex];
                           
                           //
                           // Get coordinates of the neighbors
                           //
                           const float* ai = &inputCoords[neighA * 3];
                           const float* bi = &inputCoords[neighB * 3];
                           const float* li = &inputCoords[neigh * 3];
                           
                           //
                           // Adjust position of neighbor average
                           //
                           float p[3] = {
                              2 * li[0] - ai[0] - bi[0],
                              2 * li[1] - ai[1] - bi[1],
                              2 * li[2] - ai[2] - bi[2]
                           };
                           const float len = std::sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
                           p[0] /= len;
                           p[1] /= len;
                           p[2] /= len;
                           
                           neighAvg[0] += li[0] + landmarkScale * p[0];
                           neighAvg[1] += li[1] + landmarkScale * p[1];
                           neighAvg[2] += li[2] + landmarkScale * p[2];

                           //std::cout << "Node "
                           //          << i
                           //          << ": p["
                           //          << p[0]
                           //          << ","
                           //          << p[1]
                           //          << ","
                           //          << p[2]
                           //          << "]"
                           //          << std::endl;
                        }
                     }
                     
                     const float neighPlusOne = nodeInfo[i].numLandmarkNeighbors + 1;
                     neighAvg[0] /= neighPlusOne;
                     neighAvg[1] /= neighPlusOne;
                     neighAvg[2] /= neighPlusOne;
                     
                     outputCoords[ix] = inverseStrength * inputCoords[ix]
                                       + strength * neighAvg[0];
                     outputCoords[iy] = inverseStrength * inputCoords[iy]
                                       + strength * neighAvg[1];
                     outputCoords[iz] = inverseStrength * inputCoords[iz]
                                         + strength * neighAvg[2];
                  }
               }
               break;
         }
         
         if (smoothIt) {
            //
            // Get the neighbors for this node
            //
            int numNeighbors = 0;
            const int* neighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
            
            bool arealSmoothIt = false;
            bool linearSmoothIt = false;
            switch (smoothingType) {
               case SMOOTHING_TYPE_LANDMARK_NEIGHBOR_CONSTRAINED:
                  linearSmoothIt = true;
                  break;
               case SMOOTHING_TYPE_LANDMARK_CONSTRAINED:
                  linearSmoothIt = true;
                  break;
               case SMOOTHING_TYPE_AREAL:
                  arealSmoothIt = true;
                  break;
               case SMOOTHING_TYPE_LINEAR:
                  linearSmoothIt = true;
                  break;
            }
            
            if (arealSmoothIt) {
               if (numNeighbors > 1) {
                  float totalArea = 0.0;
                  for (int j = 0; j < numNeighbors; j++) {                  
                     //
                     // get 2 consecutive neighbors of this node
                     //
                     const int n1 = neighbors[j];
                     int next = j + 1;
                     if (next >= numNeighbors) {
                        next = 0;
                     }
                     const int n2 = neighbors[next];
                  
                     //
                     // Area of the triangle
                     //
                     const float area = MathUtilities::triangleArea(&inputCoords[ix], 
                                                                  &inputCoords[n1*3],
                                                                  &inputCoords[n2*3]);
                     tileAreas[j] = area;
                     totalArea += area;
                     
                     //
                     // Save center of this tile
                     //
                     for (int k = 0; k < 3; k++) {
                        float p = (inputCoords[ix+k] + inputCoords[n1*3+k] + inputCoords[n2*3+k]) / 3.0;
                        tileCenters[j*3+k] = p;
                     }
                  }
                    
                  //
                  // Total area is zero when this node and all of its neighbors
                  // have the exact same XYZ coordinate
                  //
                  if (totalArea > 0.0) {
                     //
                     // Compute the influence of the neighboring nodes
                     //
                     float xa = 0.0;
                     float ya = 0.0;
                     float za = 0.0;
                     
                     for (int j = 0; j < numNeighbors; j++) {
                        if (tileAreas[j] > 0.0) {
                           const float weight = tileAreas[j] / totalArea;
                           xa += weight * tileCenters[j*3];
                           ya += weight * tileCenters[j*3+1];
                           za += weight * tileCenters[j*3+2];
                        }
                     }
      
                     //
                     // Update the nodes position
                     //
                     outputCoords[ix] = inputCoords[ix] * inverseStrength 
                                    + xa * strength;
                     outputCoords[iy] = inputCoords[iy] * inverseStrength 
                                    + ya * strength;
                     outputCoords[iz] = inputCoords[iz] * inverseStrength 
                                    + za * strength;
                  }
               }
            }
            
            if (linearSmoothIt) {
               if (numNeighbors > 1) {
                  float neighXYZ[3] = { 0.0, 0.0, 0.0 };
                  for (int j = 0; j < numNeighbors; j++) {
                     const int n = neighbors[j];
                     neighXYZ[0] += inputCoords[n*3];
                     neighXYZ[1] += inputCoords[n*3+1];
                     neighXYZ[2] += inputCoords[n*3+2];
                  }
                  
                  const float floatNumNeigh = numNeighbors;
                  neighXYZ[0] /= floatNumNeigh;
                  neighXYZ[1] /= floatNumNeigh;
                  neighXYZ[2] /= floatNumNeigh;
                  
                  
                  outputCoords[ix] = inputCoords[ix] * inverseStrength 
                                 + neighXYZ[0] * strength;
                  outputCoords[iy] = inputCoords[iy] * inverseStrength 
                                 + neighXYZ[1] * strength;
                  outputCoords[iz] = inputCoords[iz] * inverseStrength 
                                 + neighXYZ[2] * strength;
               }
            }
         }
      }
      
      if (getImAThread()) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Smoothing Thread " << getThreadNumber() << " finished iteration." << std::endl;
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
   delete[] tileAreas;
   delete[] tileCenters;
}
      
/**
 * Set the indices of the nodes that are to be smoothed (inclusive).
 */
void 
BrainModelSurfaceSmoothing::setIndicesOfNodesToSmooth(const int startNodeIndexIn, 
                                                      const int endNodeIndexIn)
{
   startNodeIndex = startNodeIndexIn;
   endNodeIndex   = endNodeIndexIn;
}

/**
 * Set the input and output coords
 */
void
BrainModelSurfaceSmoothing::setInputAndOutputCoords(float* inCoords, float* outCoords)
{
   inputCoords  = inCoords;
   outputCoords = outCoords;
}

//
//******************************************************************************************
//

/**
 * Constructor
 */
BrainModelSurfaceSmoothing::NodeInfo::NodeInfo()
{
   nodeType = NODE_TYPE_NORMAL;
   numLandmarkNeighbors = 0;
   offset[0] = 0.0;
   offset[1] = 0.0;
   offset[2] = 0.0;
}

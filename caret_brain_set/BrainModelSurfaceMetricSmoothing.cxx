
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

#include <limits>
#include <set>
#include <QDateTime>
#include <cmath>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricFullWidthHalfMaximum.h"
#include "BrainModelSurfaceMetricSmoothing.h"
#include "DebugControl.h"
#include "GaussianComputation.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "StringUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

#ifdef _OPENMP
#include "omp.h"
#endif

/**
 * Constructor.
 */
BrainModelSurfaceMetricSmoothing::BrainModelSurfaceMetricSmoothing(
                                                BrainSet* bs,
                                                BrainModelSurface* fiducialSurfaceIn,
                                                BrainModelSurface* gaussianSphericalSurfaceIn,
                                                MetricFile* metricFileIn,
                                                const SMOOTH_ALGORITHM algorithmIn,
                                                const int columnIn, 
                                                const int outputColumnIn,
                                                const QString& outputColumnNameIn,
                                                const float strengthIn,
                                                const int iterationsIn,
                                                const float desiredFullWidthHalfMaximumIn,
                                                const float gaussNormBelowCutoffIn,
                                                const float gaussNormAboveCutoffIn,
                                                const float gaussSigmaNormIn,
                                                const float gaussSigmaTangIn,
                                                const float gaussTangentCutoffIn,
                                                const float geodesicGaussSigmaIn)
   : BrainModelAlgorithm(bs)
{
   fiducialSurface = fiducialSurfaceIn;
   gaussianSphericalSurface = gaussianSphericalSurfaceIn;
   if (gaussianSphericalSurface == NULL) {
      gaussianSphericalSurface = fiducialSurface;
   }
   metricFile = metricFileIn;
   algorithm = algorithmIn;
   column = columnIn;
   outputColumn = outputColumnIn;
   outputColumnName = outputColumnNameIn;
   strength = strengthIn;
   iterations = iterationsIn;
   desiredFullWidthHalfMaximum = desiredFullWidthHalfMaximumIn;
   gaussNormBelowCutoff = gaussNormBelowCutoffIn;
   gaussNormAboveCutoff = gaussNormAboveCutoffIn;
   gaussSigmaNorm = gaussSigmaNormIn;
   gaussSigmaTang = gaussSigmaTangIn;
   gaussTangentCutoff = gaussTangentCutoffIn;
   geodesicGaussSigma = geodesicGaussSigmaIn;
}

/**
 * Destructor.
 */
BrainModelSurfaceMetricSmoothing::~BrainModelSurfaceMetricSmoothing()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceMetricSmoothing::execute() throw (BrainModelAlgorithmException)
{
   fullWidthHalfMaximumSmoothingResultsDescription = "";
   numberOfNodes = fiducialSurface->getNumberOfNodes();
   
   //
   // Check for valid input column
   //
   if ((metricFile->getNumberOfColumns() <= 0) || 
       (metricFile->getNumberOfNodes() <= 0)) {
      throw BrainModelAlgorithmException("Metric file contains no data.");
   }
   if ((column < 0) || (column >= metricFile->getNumberOfColumns())) {
      throw BrainModelAlgorithmException("Input metric column is invalid.");
   }
   
   //
   // Inverse of strength is applied to the node's current metric value
   //
   const float oneMinusStrength = 1.0 - strength;
   
   //
   // Create a new column if needed.
   //
   if ((outputColumn < 0) || (outputColumn >= metricFile->getNumberOfColumns())){
      metricFile->addColumns(1);
      outputColumn = metricFile->getNumberOfColumns() - 1;
   }
   metricFile->setColumnName(outputColumn, outputColumnName);
   
   //
   // Copy the input column to the output column
   //
   if (column != outputColumn) {
      std::vector<float> values;
      metricFile->getColumnForAllNodes(column, values);
      metricFile->setColumnForAllNodes(outputColumn, values);
   }
   
   //
   // column now being smoothed
   //
   const int smoothColumn = outputColumn;
   
   //
   // Allocate arrays for storing data of column being smoothed
   //
   float* inputValues = new float[numberOfNodes];
   float* outputValues = new float[numberOfNodes];
   
   //
   // Coordinates of all nodes
   //
   const CoordinateFile* coordinateFile = fiducialSurface->getCoordinateFile();
   
   //
   // Used if gaussian smoothing is being performed
   //
   GaussianComputation gauss(gaussNormBelowCutoff,
                             gaussNormAboveCutoff,
                             gaussSigmaNorm,
                             gaussSigmaTang,
                             gaussTangentCutoff);
   
   //
   // Determine the neighbors for each node
   //
   determineNeighbors();
   
   //
   // Full width half maximum measurements
   //
   float fullWidthHalfMaximum = 0.0;
   int fullWidthHalfMaximumNumberOfIterations = 0;
   
   //
   // Prepate for smoothing
   //
   for (int iter = 0; iter < iterations; iter++) {
   
      bool stopSmoothingFlag = false;
      switch (algorithm) {
         case SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS:
            break;
         case SMOOTH_ALGORITHM_DILATE:
            break;
         case SMOOTH_ALGORITHM_FULL_WIDTH_HALF_MAXIMUM:
            {
               //
               // Determine Full Width Half Maximum
               //
               BrainModelSurfaceMetricFullWidthHalfMaximum fwhm(brainSet,
                                                                fiducialSurface,
                                                                metricFile,
                                                                smoothColumn);
               fwhm.execute();
               fullWidthHalfMaximum = fwhm.getFullWidthHalfMaximum();
               
               if (DebugControl::getDebugOn()) {
                  std::cout << "Smoothing Full Width Half Maximum before iteration "
                            << iter
                            << " is "
                            << fullWidthHalfMaximum
                            << std::endl;
               }
               
               fullWidthHalfMaximumSmoothingResultsDescription += 
                  ("Before Iteration " + QString::number(iter)
                   + " Estimated FWHM: " + QString::number(fullWidthHalfMaximum, 'f', 3)
                   + "\n");
                   
               //
               // if FWHM achieved, stop smoothing
               //
               if (fullWidthHalfMaximum >= desiredFullWidthHalfMaximum) {
                  stopSmoothingFlag = true;
               }
            }
            break;
         case SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN:
         case SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS:
         case SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN:
         case SMOOTH_ALGORITHM_NONE:
            break;
      }
      
      //
      // Should smoothing be stopped ???
      //
      if (stopSmoothingFlag) {
         break;
      }
      
      //
      // load arrays for smoothing data
      //
      //std::vector<float> columnValues(numberOfNodes);
      //getColumnForAllNodes(outputColumn, columnValues);
      metricFile->getColumnForAllNodes(smoothColumn, inputValues);        
      
      //
      // smooth all of the nodes
      //
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (int i = 0; i < numberOfNodes; i++) {
         //
         // copy input to output in event this node is not smoothed
         //
         outputValues[i] = inputValues[i];

         const NeighborInfo& neighInfo = nodeNeighbors[i];
                  
         //
         // Does this node have neighbors
         //
         if (neighInfo.numNeighbors > 0) {
         
            float neighborSum = 0.0;
            bool setOutputValueFlag = true;
            bool dilateModeFlag = false;
            switch (algorithm) {
               case SMOOTH_ALGORITHM_NONE:
                  break;
               case SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS:
                  {
                     //
                     // smooth metric data for this node
                     //
                     for (int j = 0; j < neighInfo.numNeighbors; j++) {
                        //
                        // Note: outputColumn has output from last iteration of smoothing
                        //
                        neighborSum += inputValues[neighInfo.neighbors[j]];
                     }
                     neighborSum = neighborSum / static_cast<float>(neighInfo.numNeighbors);
                  };
                  break;
               case SMOOTH_ALGORITHM_DILATE:
                  {
                     if (inputValues[i] != 0.0) {
                        //
                        // Do not process nodes with non-zero values
                        //
                        setOutputValueFlag = false;
                     }
                     else {
                         //
                         // Average of non-zero neighbors
                         //
                         int numNonZeroNeighbors = 0;
                         for (int j = 0; j < neighInfo.numNeighbors; j++) {
                            //
                            // Note: outputColumn has output from last iteration of smoothing
                            //
                            float neighborValue = inputValues[neighInfo.neighbors[j]];
                            if (neighborValue != 0.0) {
                               neighborSum += neighborValue;
                               numNonZeroNeighbors++;
                            }
                         }
                         if (numNonZeroNeighbors > 0) {
                            neighborSum = neighborSum / static_cast<float>(numNonZeroNeighbors);
                            dilateModeFlag = true;
                         }
                     }
                  };
                  break;
               case SMOOTH_ALGORITHM_FULL_WIDTH_HALF_MAXIMUM:
                  {
                     //
                     // smooth metric data for this node
                     //
                     for (int j = 0; j < neighInfo.numNeighbors; j++) {
                        //
                        // Note: outputColumn has output from last iteration of smoothing
                        //
                        neighborSum += inputValues[neighInfo.neighbors[j]];
                     }
                     
                     //neighborSum = neighborSum / static_cast<float>(neighInfo.numNeighbors);

                     //
                     // Ignore strength so set output value here
                     // FWHM paper does average of node and its neighbors
                     //
                     neighborSum += inputValues[i];
                     outputValues[i] = neighborSum / static_cast<float>(neighInfo.numNeighbors + 1);
                     setOutputValueFlag = false;
                  };
                  break;
               case SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN:
                  {
                     //
                     // Get neighbor information for gaussian smoothing
                     //
                     std::vector<GaussianComputation::Point3D> points;
                     for (int j = 0; j < neighInfo.numNeighbors; j++) {
                        const int neigh = neighInfo.neighbors[j];
                        points.push_back(GaussianComputation::Point3D(
                                                      coordinateFile->getCoordinate(neigh),
                                                      inputValues[neigh]));
                     }
                     
                     if (DebugControl::getDebugOn()) {
                       if (iter == 0) {
                           if (DebugControl::getDebugNodeNumber() == i) {
                              std::set<int> sortedNeighbors;
                              for (int j = 0; j < neighInfo.numNeighbors; j++) {
                                 sortedNeighbors.insert(neighInfo.neighbors[j]);
                              }

                              std::cout << "Neighbors ("
                                        << neighInfo.numNeighbors
                                        << ") of "
                                        << i << ": ";
                              for (std::set<int>::iterator it = sortedNeighbors.begin();
                                   it != sortedNeighbors.end();
                                   it++) {
                                 std::cout << *it << " ";
                              }
                              std::cout << std::endl;
                           }
                        }
                     }
                     //
                     // Evaluate the gaussian for the node and its neighbors
                     //
                     neighborSum = gauss.evaluate(coordinateFile->getCoordinate(i),
                                                  fiducialSurface->getNormal(i),
                                                  points);
                  }
                  break;
               case SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS:
                  {
                     //
                     // Distances to each neighbor
                     //
                     std::vector<float> neighborDistance(neighInfo.numNeighbors);
                     float totalDistance = 0.0;
                     for (int j = 0; j < neighInfo.numNeighbors; j++) {
                        totalDistance += neighInfo.distanceToNeighbor[j];
                     }
                     if (totalDistance == 0.0) {
                        totalDistance = 1.0;
                     }
                     
                     //
                     // compute neighbor weighted average
                     //
                     std::vector<float> neighborWeights(neighInfo.numNeighbors);
                     float totalWeight = 0.0;
                     for (int j = 0; j < neighInfo.numNeighbors; j++) {
                        neighborWeights[j] =  1.0 - (neighInfo.distanceToNeighbor[j]
                                                     / totalDistance);
                        totalWeight += neighborWeights[j];
                     }
                     if (totalWeight == 0.0) {
                        totalWeight = 1.0;
                     }
                     
                     //
                     // compute neighbor weighted average
                     //
                     for (int j = 0; j < neighInfo.numNeighbors; j++) {
                        const float weight = neighborWeights[j] / totalWeight;
                        neighborSum += inputValues[neighInfo.neighbors[j]] * weight;
                     }
                  }
                  break;
               case SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN:
                  {//distance to neighbor is geodesic, not euclidean, check determineNeighbors
                     int j, end = neighInfo.numNeighbors;
                     float totalWeight = 0.0f, weight;
                     for (j = 0; j < end; ++j)
                     {//weighted average, using gaussian of geodesic distance as weight
                        weight = neighInfo.geoGaussWeight[j];//the gaussian function, this could be precomputed in determineNeighbors
                        totalWeight += weight;
                        neighborSum += weight * inputValues[neighInfo.neighbors[j]];
                     }
                     neighborSum /= totalWeight;
                     dilateModeFlag = true;//HACK: makes it use the gaussian computation, with no extra precedence for center node
                     //this is equivalent to strength = 1; oneMinusStrength = 0;, but with less computation
                     //IF DILATE IS EVER CHANGED, MAKE SURE TO USE ONLY NEIGHBORSUM IF MODE IS GEODESIC_GAUSSIAN
                  }
               break;
            }
            
            
            //
            // Apply smoothing to the node
            //
            if (setOutputValueFlag) {
               if (DebugControl::getDebugOn()) {
                  if (DebugControl::getDebugNodeNumber() == i) {
                     std::cout << "Smoothing node " << i
                               << " iteration " << iter
                               << " node neighbor sum " << neighborSum
                               << std::endl;
                  }
               }
               if (dilateModeFlag) {
                  outputValues[i] = neighborSum;//WARNING: used for geodesic gaussian, if this is changed, make geogauss gives the same result (ignore strength)
               }
               else {
                  outputValues[i] = (inputValues[i] * oneMinusStrength)
                                  + (neighborSum * strength);
               }
            }
         }
      }
      
      //
      // Copy the smoothed values to the output column
      //
      metricFile->setColumnForAllNodes(smoothColumn, outputValues);
      
      //
      // Keep track of iterations
      //
      fullWidthHalfMaximumNumberOfIterations++;
   } // for +iterations
   
   //
   // Add comments describing smoothing
   //
   QString smoothComment(metricFile->getColumnComment(smoothColumn));
   if (smoothComment.isEmpty() == false) {
      smoothComment.append("\n");
   }
   switch (algorithm) {
      case SMOOTH_ALGORITHM_NONE:
         smoothComment.append("Invalid smoothing algorithm: \n");
         break;
      case SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS:
         smoothComment.append("Average Neighbors Smoothing: \n");
         break;
      case SMOOTH_ALGORITHM_DILATE:
         smoothComment.append("Dilation");
         break;
      case SMOOTH_ALGORITHM_FULL_WIDTH_HALF_MAXIMUM:
         smoothComment.append("Full Width Half Maximum Algorithm: \n"
                              "   Desired Full Width Half Maximum: "
                              + QString::number(desiredFullWidthHalfMaximum, 'f', 3) + "\n"
                              + "   Full Width Half Maximum achieved: "
                              + QString::number(fullWidthHalfMaximum, 'f', 3) + "\n"
                              + "   Number of Iterations to achieve FWHM "
                              + QString::number(fullWidthHalfMaximumNumberOfIterations) + "\n");
         break;
      case SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN:
         smoothComment.append("Gaussian (Surface Normal) Smoothing: \n");
         smoothComment.append("   Norm Below Cutoff: ");
         smoothComment.append(StringUtilities::fromNumber(gaussNormBelowCutoff));
         smoothComment.append("\n");
         smoothComment.append("   Norm Above Cutoff: ");
         smoothComment.append(StringUtilities::fromNumber(gaussNormAboveCutoff));
         smoothComment.append("\n");
         smoothComment.append("   Sigma Norm: ");
         smoothComment.append(StringUtilities::fromNumber(gaussSigmaNorm));
         smoothComment.append("\n");
         smoothComment.append("   Sigma Tang: ");
         smoothComment.append(StringUtilities::fromNumber(gaussSigmaTang));
         smoothComment.append("\n");
         smoothComment.append("   Tangend Cutoff: ");
         smoothComment.append(StringUtilities::fromNumber(gaussTangentCutoff));
         smoothComment.append("\n");
         break;
      case SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS:
         smoothComment.append("Weighted Average Neighbors Smoothing: \n");
         break;
      case SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN:
         smoothComment.append("Geodesic Gaussian Smoothing: \n");
         smoothComment.append("   Sigma: ");
         smoothComment.append(StringUtilities::fromNumber(geodesicGaussSigma));
         smoothComment.append("\n");
         break;
   }
   smoothComment.append("   Stength/Iterations: ");
   smoothComment.append(StringUtilities::fromNumber(strength));
   smoothComment.append(" ");
   smoothComment.append(StringUtilities::fromNumber(iterations));
   smoothComment.append("\n");
   metricFile->setColumnComment(smoothColumn, smoothComment);
   
   delete[] inputValues;
   delete[] outputValues;
}

/**
 * determine neighbors for each node.
 */
void
BrainModelSurfaceMetricSmoothing::determineNeighbors()
{
    //
    // Clear the neighbors
    //
    nodeNeighbors.clear();
    nodeNeighbors.resize(numberOfNodes);

    QTime timer;
    timer.start();
#ifdef _OPENMP
#pragma omp parallel
#endif
    {
        //
        // Get the topology helper
        //
        const TopologyFile* topologyFile = fiducialSurface->getTopologyFile();
        const TopologyHelper topologyHelper(topologyFile, false, true, false);//each thread gets its own, due to locks slowing it down otherwise

        //
        // Coordinate file and maximum distance cutoff
        //
        CoordinateFile* cf = fiducialSurface->getCoordinateFile();
        GeodesicHelper* gh = NULL;
        float maxDistanceCutoff = std::numeric_limits<float>::max();
        float geoCutoff = 4.0f * geodesicGaussSigma;
        std::vector<float>* geoGaussWeight = NULL, distance;
        switch (algorithm) {
        case SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS:
            break;
        case SMOOTH_ALGORITHM_DILATE:
            break;
        case SMOOTH_ALGORITHM_FULL_WIDTH_HALF_MAXIMUM:
            break;
        case SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS:
            break;
        case SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN:
            cf = gaussianSphericalSurface->getCoordinateFile();
            maxDistanceCutoff = std::max(std::max(gaussNormBelowCutoff,
                                                  gaussNormAboveCutoff),
                                         gaussTangentCutoff);
            break;
        case SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN:
            cf = fiducialSurface->getCoordinateFile();
            gh = new GeodesicHelper(cf, topologyFile);//each thread gets its own
            geoGaussWeight = new std::vector<float>;
            break;
        case SMOOTH_ALGORITHM_NONE:
            break;
        }

        //
        // Loop through the nodes
        //
#ifdef _OPENMP
#pragma omp for
#endif
        for (int i = 0; i < numberOfNodes; i++) {
            std::vector<int> neighbors;
            int numNeigh;
            float tempf;

            switch (algorithm) {
            case SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS:
            case SMOOTH_ALGORITHM_DILATE:
            case SMOOTH_ALGORITHM_FULL_WIDTH_HALF_MAXIMUM:
            case SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS:
            {
                //
                // Get the neighbors for the node
                //
                topologyHelper.getNodeNeighbors(i, neighbors);
            }
            break;
            case SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN:
            {
                //
                // Get the neighbors for the node to the specified depth
                //
                topologyHelper.getNodeNeighborsToDepth(i, 5, neighbors);
            }
            break;
            case SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN:
                gh->getNodesToGeoDist(i, geoCutoff, neighbors, distance, true);
                if (neighbors.size() < 7)//5 neighbor nodes may fail this unneccesarily, but better than letting 6 neighbor nodes slip through with 5
                {//in case a really small kernel is specified
                    topologyHelper.getNodeNeighbors(i, neighbors);
                    neighbors.push_back(i);//for geogauss, we want the center node in the list
                    gh->getGeoToTheseNodes(i, neighbors, distance, true);
                }
                numNeigh = (int)neighbors.size();
                geoGaussWeight->resize(numNeigh);
                for (int j = 0; j < numNeigh; ++j)
                {
                   tempf = distance[j] / geodesicGaussSigma;
                   (*geoGaussWeight)[j] = std::exp(-tempf * tempf * 0.5f);//the gaussian function
                }
                break;
            case SMOOTH_ALGORITHM_NONE:
                break;
            }

            //
            // add to all neighbors
            //
            nodeNeighbors[i] = NeighborInfo(cf, i, neighbors, maxDistanceCutoff, geoGaussWeight);
        }
        if (gh) delete gh;
        if (geoGaussWeight) delete geoGaussWeight;
    }//omp parallel
    const float elapsedTime = timer.elapsed() * 0.001;
    if (DebugControl::getDebugOn()) {
        std::cout << "Time to determine neighbors: " << elapsedTime << " seconds." << std::endl;
    }
}

//***************************************************************************************

/**
 * Constructor.
 */
BrainModelSurfaceMetricSmoothing::NeighborInfo::NeighborInfo(const CoordinateFile* cf,
                                                    const int myNodeNumber,
                                                    const std::vector<int>& neighborsIn,
                                                    const float maxDistanceCutoff,
                                                    const std::vector<float>* distanceWeights)
{
   const int numNeighborsIn = static_cast<int>(neighborsIn.size());
   if (distanceWeights)
   {//use STL vector copy operator, don't need to exclude anything
      geoGaussWeight = *distanceWeights;
      neighbors = neighborsIn;
   } else {
      for (int i = 0; i < numNeighborsIn; i++) {
         const int node = neighborsIn[i];
         const float dist = cf->getDistanceBetweenCoordinates(myNodeNumber, node);
         if (dist <= maxDistanceCutoff) {
            neighbors.push_back(node);
            distanceToNeighbor.push_back(dist);
         }
      }
   }
   numNeighbors = static_cast<int>(neighbors.size());
}

/**
 * Destructor.
 */
BrainModelSurfaceMetricSmoothing::NeighborInfo::~NeighborInfo()
{
}

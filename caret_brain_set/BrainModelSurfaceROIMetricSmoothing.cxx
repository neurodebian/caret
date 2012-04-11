
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
#include "BrainModelSurfaceROIMetricSmoothing.h"
#include "DebugControl.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "StringUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#ifdef _OPENMP
#include <omp.h>
#endif

/**
 * Constructor.
 */
BrainModelSurfaceROIMetricSmoothing::BrainModelSurfaceROIMetricSmoothing(
                                                BrainSet* bs,
                                                BrainModelSurface* fiducialSurfaceIn,
                                                MetricFile* metricFileIn,
                                                MetricFile* roiFileIn,
                                                const int columnIn, 
                                                const int outputColumnIn,
                                                const QString& outputColumnNameIn,
                                                const float strengthIn,
                                                const int iterationsIn,                                                
                                                const float gaussNormBelowCutoffIn,
                                                const float gaussNormAboveCutoffIn,
                                                const float gaussSigmaNormIn,
                                                const float gaussSigmaTangIn,
                                                const float gaussTangentCutoffIn,
                                                const float geodesicGaussSigmaIn)
   : BrainModelAlgorithm(bs)
{
   fiducialSurface = fiducialSurfaceIn;

   metricFile = metricFileIn;
   roiFile = roiFileIn;
   column = columnIn;
   outputColumn = outputColumnIn;
   outputColumnName = outputColumnNameIn;
   strength = strengthIn;
   iterations = iterationsIn;
   gaussNormBelowCutoff = gaussNormBelowCutoffIn;
   gaussNormAboveCutoff = gaussNormAboveCutoffIn;
   gaussSigmaNorm = gaussSigmaNormIn;
   gaussSigmaTang = gaussSigmaTangIn;
   gaussTangentCutoff = gaussTangentCutoffIn;
   geodesicGaussSigma = geodesicGaussSigmaIn;
   smoothAllColumnsFlag = false;
   runParallelFlag = false;
}

/**
 * Constructor for all column smoothing
 */
BrainModelSurfaceROIMetricSmoothing::BrainModelSurfaceROIMetricSmoothing(
                                                BrainSet* bs,
                                                BrainModelSurface* fiducialSurfaceIn,                                                
                                                MetricFile* metricFileIn,
                                                MetricFile* roiFileIn,                                                
                                                const float strengthIn,
                                                const int iterationsIn,                                                
                                                const float gaussNormBelowCutoffIn,
                                                const float gaussNormAboveCutoffIn,
                                                const float gaussSigmaNormIn,
                                                const float gaussSigmaTangIn,
                                                const float gaussTangentCutoffIn,
                                                const float geodesicGaussSigmaIn,
                                                const bool  parallelSmoothingFlagIn)
   : BrainModelAlgorithm(bs)
{
   fiducialSurface = fiducialSurfaceIn;
   
   metricFile = metricFileIn;
   roiFile = roiFileIn;  
   strength = strengthIn;
   iterations = iterationsIn;
   gaussNormBelowCutoff = gaussNormBelowCutoffIn;
   gaussNormAboveCutoff = gaussNormAboveCutoffIn;
   gaussSigmaNorm = gaussSigmaNormIn;
   gaussSigmaTang = gaussSigmaTangIn;
   gaussTangentCutoff = gaussTangentCutoffIn;
   geodesicGaussSigma = geodesicGaussSigmaIn;
   runParallelFlag = parallelSmoothingFlagIn;
   smoothAllColumnsFlag = true;
   
   //initialize to avoid errors
   column = 0;
   outputColumn = 0;
}

/**
 * Destructor.
 */
BrainModelSurfaceROIMetricSmoothing::~BrainModelSurfaceROIMetricSmoothing()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceROIMetricSmoothing::execute() throw (BrainModelAlgorithmException)
{
   numberOfNodes = fiducialSurface->getNumberOfNodes();
   
   //
   // Check for valid input column
   //
   if ((metricFile->getNumberOfColumns() <= 0) || 
       (metricFile->getNumberOfNodes() <= 0)) {
      throw BrainModelAlgorithmException("Metric file contains no data.");
   }
   
   if ((roiFile->getNumberOfColumns() <= 0) )
   {
         throw BrainModelAlgorithmException("ROI file contains no data.");
   }

   if (!smoothAllColumnsFlag && ((column < 0) || (column >= metricFile->getNumberOfColumns()))) {
      throw BrainModelAlgorithmException("Input metric column is invalid.");
   }   
   
   //
   // Create a new column if needed.
   //
   if (!smoothAllColumnsFlag && ((outputColumn < 0) || (outputColumn >= metricFile->getNumberOfColumns()))){
      metricFile->addColumns(1);
      outputColumn = metricFile->getNumberOfColumns() - 1;
   }
   if(!outputColumnName.isEmpty())metricFile->setColumnName(outputColumn, outputColumnName);
   
   //
   // Copy the input column to the output column
   //
   if (column != outputColumn) {
      std::vector<float> values;
      metricFile->getColumnForAllNodes(column, values);
      metricFile->setColumnForAllNodes(outputColumn, values);
   }  
  
   //
   // Get array of ROI values for all nodes
   //
   roiValues = new float[numberOfNodes];
   roiFile->getColumnForAllNodes(0,roiValues);
   
   //
   // Determine the neighbors for each node
   //
   determineNeighbors();
   
   QString smoothComment;
   smoothComment.append("Geodesic Gaussian Smoothing: \n");
   smoothComment.append("   Sigma: ");
   smoothComment.append(StringUtilities::fromNumber(geodesicGaussSigma));
   smoothComment.append("\n");

   smoothComment.append("   Stength/Iterations: ");
   smoothComment.append(StringUtilities::fromNumber(strength));
   smoothComment.append(" ");
   smoothComment.append(StringUtilities::fromNumber(iterations));
   smoothComment.append("\n");
   
   /*bool debugOpenMpFlag = false; 
   if (this->smoothAllColumnsFlag) {
      int numColumns = this->metricFile->getNumberOfColumns();
      
      if (this->runParallelFlag) {
         #pragma omp parallel for
         for (int i = 0; i < numColumns; i++) {
            if (debugOpenMpFlag) {
               std::cout << "Start Smoothing Column: "
                         << i
                         << std::endl;
            }
            this->smoothSingleColumn(smoothComment, i, i);
            if (debugOpenMpFlag) {
               std::cout << "Finished Smoothing Column: "
                         << i
                         << std::endl;
            }
         }
      }
      else {
         for (int i = 0; i < numColumns; i++) {
            if (debugOpenMpFlag) {
               std::cout << "Smoothing Column " << i << std::endl;
            }
            this->smoothSingleColumn(smoothComment, i, i);
         }
      }
   }
   else {
      this->smoothSingleColumn(smoothComment, column, outputColumn);
   } //*/
   if (smoothAllColumnsFlag)
   {
      int numColumns = metricFile->getNumberOfColumns();
      for (int i = 0; i < numColumns; ++i)
      {
         smoothSingleColumn(smoothComment, i, i);//this is node parallel
      }
   } else {
      smoothSingleColumn(smoothComment, column, outputColumn);
   }
   
   delete[] roiValues;
}

/**
 * smooth a column in the metric file.
 */
void 
BrainModelSurfaceROIMetricSmoothing::smoothSingleColumn(const QString& columnDescription,
                         const int inputColumn,
                         const int outputColumn)
{
   //
   // Allocate arrays for storing data of column being smoothed
   //
   int numberOfNodes = metricFile->getNumberOfNodes();
   float* inputValues = new float[numberOfNodes];
   float* outputValues = new float[numberOfNodes];

   metricFile->getColumnForAllNodes(inputColumn, inputValues); 
   if (inputColumn != outputColumn) {
      metricFile->setColumnForAllNodes(outputColumn, inputValues);
   }
   int smoothColumn = outputColumn;
   
   //
   // Prepare for smoothing
   //
   for (int iter = 0; iter < iterations; iter++) {
   
      bool stopSmoothingFlag = false;
           
      //
      // Should smoothing be stopped ???
      //
      if (stopSmoothingFlag) {
         break;
      }
      
      //
      // load arrays for smoothing data
      //
      
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

         //Here is where we filter out center nodes that aren't in the ROI
         if(roiValues[i] == 0.0) //make sure round-off errors don't affect this
         {
            outputValues[i] = 0.0;
            continue;
         }
         const NeighborInfo& neighInfo = nodeNeighbors[i];
                  
         //
         // Does this node have neighbors
         //
         if (neighInfo.numNeighbors > 0) {
         
            float neighborSum = 0.0;
            bool setOutputValueFlag = true;
            
            
            //distance to neighbor is geodesic, not euclidean, check determineNeighbors
            int j, end = neighInfo.numNeighbors;
            float totalWeight = 0.0f, weight;
            for (j = 0; j < end; ++j)
            {//weighted average, using gaussian of geodesic distance as weight
               weight = neighInfo.geoGaussWeights[j];//precomputed gaussian weights
               totalWeight += weight;
               neighborSum += weight * inputValues[neighInfo.neighbors[j]];
            }
            neighborSum /= totalWeight;          
            
            
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
       
               outputValues[i] = neighborSum;
               
            }
         }
      }
      
      //
      // Copy the smoothed values to the output column
      //
      metricFile->setColumnForAllNodes(smoothColumn, outputValues);
      
   } // for +iterations
   
   delete[] inputValues;
   delete[] outputValues;

   QString smoothComment(metricFile->getColumnComment(smoothColumn));
   if (smoothComment.isEmpty() == false) {
      smoothComment.append("\n");
   }  
   smoothComment.append(columnDescription);
   metricFile->setColumnComment(smoothColumn, smoothComment);
}

/**
 * determine neighbors for each node.
 */
void 
BrainModelSurfaceROIMetricSmoothing::determineNeighbors()
{
   //
   // Clear the neighbors
   //
   nodeNeighbors.clear();
   nodeNeighbors.resize(numberOfNodes);
   
   //
   // Get the topology helper
   //
   const TopologyFile* topologyFile = fiducialSurface->getTopologyFile();
   
   //
   // Coordinate file and maximum distance cutoff
   //
   CoordinateFile* cf = fiducialSurface->getCoordinateFile();
   float geoCutoff = 4.0f * geodesicGaussSigma;
   cf = fiducialSurface->getCoordinateFile();
   QTime timer;
   timer.start();
#ifdef _OPENMP
#pragma omp parallel
#endif
   {
      TopologyHelper topologyHelper(topologyFile, false, true, false);
      GeodesicHelper gh(cf, topologyFile);//need private copies of this due to mutex locking
      std::vector<float> distance;

      //
      // Loop through the nodes
      //
      std::vector<int> neighbors;
      std::vector<float> geoWeights;
#ifdef _OPENMP
#pragma omp for
#endif
      for (int i = 0; i < numberOfNodes; i++) {
         neighbors.clear();//vectors shouldn't deallocate memory on clear(), so this saves lots and lots of slow new and delete calls
         geoWeights.clear();
         gh.getNodesToGeoDist(i, geoCutoff, neighbors, distance, true);
         if (neighbors.size() < 7)//5 neighbor nodes may fail this unneccesarily, but better than letting 6 neighbor nodes slip through with 5
         {//in case a really small kernel is specified - do NOT test only for ROI neighbors here, this is only to make sure the geoCutoff isn't too harsh
            //maybe it should union getNodeNeighbors and getNodesToGeoDist?  Probably only useful for extreme edge cases
            topologyHelper.getNodeNeighbors(i, neighbors);
            neighbors.push_back(i);//for geogauss, we want the center node in the list
            gh.getGeoToTheseNodes(i, neighbors, distance, true);
         }
         //
         // add to all neighbors - this constructor uses roiValues to exclude neighbors outside ROI
         //
         int numNeigh = (int)neighbors.size();
         for (int j = 0; j < numNeigh; ++j)
         {
            float tempf = distance[j] / geodesicGaussSigma;
            geoWeights.push_back(std::exp((double)(-tempf * tempf * 0.5f)));
         }
         nodeNeighbors[i] = NeighborInfo(neighbors, distance, geoWeights, roiValues);
      }
   }
   const float elapsedTime = timer.elapsed() * 0.001;
   if (DebugControl::getDebugOn()) {
      std::cout << "Time to determine neighbors: " << elapsedTime << " seconds." << std::endl;
   }
}      

//***************************************************************************************

/**
 * Constructor.
 */
BrainModelSurfaceROIMetricSmoothing::NeighborInfo::NeighborInfo(const std::vector<int>& neighborsIn,                                                    
                                                    const std::vector<float>& distances,
                                                    const std::vector<float>& geoWeights,
                                                    const float * roiValuesIn)
{
   const int numNeighborsIn = static_cast<int>(neighborsIn.size());

   for (int i = 0; i < numNeighborsIn; i++) {
      const int node = neighborsIn[i];
      // here is where we filter out neighbors that aren't in the ROI
      if(roiValuesIn[node] == 0.0) continue; //make sure round-off errors don't affect this
      neighbors.push_back(node);
      
      
      distanceToNeighbor.push_back(distances[i]);
      
      geoGaussWeights.push_back(geoWeights[i]);
   }

   numNeighbors = static_cast<int>(neighbors.size());
}

/**
 * Destructor.
 */
BrainModelSurfaceROIMetricSmoothing::NeighborInfo::~NeighborInfo()
{
}

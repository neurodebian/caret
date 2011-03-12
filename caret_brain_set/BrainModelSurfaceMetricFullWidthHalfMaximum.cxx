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
#include <iostream>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricFullWidthHalfMaximum.h"
#include "CoordinateFile.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "StatisticMeanAndDeviation.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
BrainModelSurfaceMetricFullWidthHalfMaximum::BrainModelSurfaceMetricFullWidthHalfMaximum(
                                            BrainSet* bs,
                                            BrainModelSurface* brainModelSurfaceIn,
                                            MetricFile* metricFileIn,
                                            const int metricColumnIn)
   : BrainModelAlgorithm(bs)
{
   fullWidthHalfMaximum = 0.0;
   brainModelSurface = brainModelSurfaceIn;
   metricFile        = metricFileIn;
   metricColumn = metricColumnIn;
}

/**
 * destructor.
 */
BrainModelSurfaceMetricFullWidthHalfMaximum::~BrainModelSurfaceMetricFullWidthHalfMaximum()
{
}


/**
 * Execute the algorithm.
 *
 * This algorithm is from:
 *    Smoothing and cluster thresholding for cortical surface-based group analysis of fMRI data
 *    Donald J. Hagler Jr., Ayse Pinar Saygin, and Martin I. Sereno
 *    NeuroImage 33 (2006) 1093-1103
 *
 *    Full Width Half Maximum (FWHM) is computed using formula 2
 *    on page 1094 of the above paper.
 */
void 
BrainModelSurfaceMetricFullWidthHalfMaximum::execute() throw (BrainModelAlgorithmException)
{
   fullWidthHalfMaximum = 0.0;

   //
   // Check inputs
   //
   if (brainModelSurface == NULL) {
      throw BrainModelAlgorithmException("Surface is NULL.");
   }
   if (metricFile == NULL) {
      throw BrainModelAlgorithmException("Surface is NULL.");
   }
   const int numNodes = brainModelSurface->getNumberOfNodes();
   if (numNodes <= 0) {
      throw BrainModelAlgorithmException("Surface contains no nodes.");
   }
   if (metricFile->getNumberOfNodes() != numNodes) {
      throw BrainModelAlgorithmException("Surface and metric file contain a different number of nodes.");
   }
   if ((metricColumn < 0) ||
       (metricColumn >= metricFile->getNumberOfColumns())) {
      throw BrainModelAlgorithmException("Metric column is invalid.");
   }
   
   //
   // Get topology file and topology helper
   //
   const TopologyFile* tf = brainModelSurface->getTopologyFile();
   if (tf == NULL) {
      throw BrainModelAlgorithmException("Surface has no topology.");
   }
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   // Get distance between each node and its neighbors
   // and the metric differences
   // USE ABSOLUTE VALUE FOR METRIC DIFFERENCES???
   //
   const CoordinateFile*cf = brainModelSurface->getCoordinateFile();
   std::vector<float> nodeToNeighborEuclideanDistances;
   std::vector<float> nodeToNeighborMetricDifferences;
   std::vector<float> nodeMetricValues;
   for (int myNodeNumber = 0; myNodeNumber < numNodes; myNodeNumber++) {
      int numNeighbors = 0;
      const int* neighbors = th->getNodeNeighbors(myNodeNumber, numNeighbors);
      
      //
      // Node is connected??
      //
      if (numNeighbors > 0) {
         const float* myXYZ = cf->getCoordinate(myNodeNumber);
         const float myMetric = metricFile->getValue(myNodeNumber, metricColumn);
         nodeMetricValues.push_back(myMetric);
         for (int neighborIndex = 0; neighborIndex < numNeighbors; neighborIndex++) {
            const int neighborNodeNumber = neighbors[neighborIndex];
            if (myNodeNumber < neighborNodeNumber) {  // avoid counting distance twice
               //
               // Inter neighbor distances
               //
               nodeToNeighborEuclideanDistances.push_back(
                           MathUtilities::distance3D(myXYZ,
                                                     cf->getCoordinate(neighborNodeNumber)));
                                                     
               //
               // Inter neighbor metric differences
               //
               float metricDiff = myMetric - metricFile->getValue(neighborNodeNumber, 
                                                                  metricColumn);
               //metricDiff = std::fabs(metricDiff);
               nodeToNeighborMetricDifferences.push_back(metricDiff);
            }
         }
      }
   }
   
   //
   // Mean and Sample Variance for node to neighbor euclidean distances
   //
   StatisticMeanAndDeviation interNeighborMeanAndDeviation;
   interNeighborMeanAndDeviation.addDataArray(&nodeToNeighborEuclideanDistances[0],
                                             static_cast<int>(nodeToNeighborEuclideanDistances.size()));
   try {
      interNeighborMeanAndDeviation.execute();
   }
   catch (StatisticException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
   //
   // "dv" is average neighbor inter-distance
   //
   const double dv = interNeighborMeanAndDeviation.getMean();

   //
   // Mean and Sample Variance for node to neighbor metric differences
   //
   StatisticMeanAndDeviation metricDifferenceMeanAndDeviation;
   metricDifferenceMeanAndDeviation.addDataArray(&nodeToNeighborMetricDifferences[0],
                                             static_cast<int>(nodeToNeighborMetricDifferences.size()));
   try {
      metricDifferenceMeanAndDeviation.execute();
   }
   catch (StatisticException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
   //
   // "varDS" is the variance of the metric inter-neighbor differences
   //
   const double varDS = metricDifferenceMeanAndDeviation.getPopulationSampleVariance();

   //
   // Sample Variance for node metric values
   //
   StatisticMeanAndDeviation metricMeanAndDeviation;
   metricMeanAndDeviation.addDataArray(&nodeMetricValues[0],
                                       static_cast<int>(nodeMetricValues.size()));
   try {
      metricMeanAndDeviation.execute();
   }
   catch (StatisticException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
   //
   // "varS" is the variance of the all nodes' metric values
   //
   const double varS = metricMeanAndDeviation.getPopulationSampleVariance();

   //
   // Equation from code MRISgaussFWHM
   // Same results as code that duplicates equation in paper
   //
/*
   if (varS != 0) {
      double varratio = -std::log(1.0 - 0.5 * (varDS / varS));
      if (varratio <= 0.0) {
         varratio = 0.5 * (varDS / varS);
      }
      if (varratio <= 0.0) {
         fullWidthHalfMaximum  = 0.0;
      }
      else {
         fullWidthHalfMaximum = std::sqrt(2.0 * std::log(2) / varratio) * dv;
      }
   }
*/
   
   //
   // Calculate FWHM
   // Exactly as in equation in paper
   //
   if (varS != 0.0) {
      const double denom = std::log((double)(1.0 - (varDS / (2.0 * varS))));
      if (denom != 0.0) {
         const double val = (-2.0 * std::log(2.0)) / denom;
         if (val >= 0.0) {
            fullWidthHalfMaximum = dv * std::sqrt(val);
            //std::cout << "Paper Equation: " << XXXfullWidthHalfMaximum
            //          << "   Hagler Code: " << fullWidthHalfMaximum << std::endl;
         }
      }
   }
}

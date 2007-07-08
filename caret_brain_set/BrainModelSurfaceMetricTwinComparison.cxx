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

#include <iostream>

#include "BrainModelSurfaceMetricTwinComparison.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "MetricFile.h"

/**
 * constructor.
 */
BrainModelSurfaceMetricTwinComparison::BrainModelSurfaceMetricTwinComparison(
                                         BrainSet* bs,
                                         const QString& metricShapeTwinAFileNameIn,
                                         const QString& metricShapeTwinBFileNameIn,
                                         const QString& outputMetricShapeFileNameIn)
   : BrainModelAlgorithm(bs)
{
   metricShapeTwinAFileName = metricShapeTwinAFileNameIn;
   metricShapeTwinBFileName = metricShapeTwinBFileNameIn;
   outputMetricShapeFileName = outputMetricShapeFileNameIn;
}

/**
 * destructor.
 */
BrainModelSurfaceMetricTwinComparison::~BrainModelSurfaceMetricTwinComparison()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceMetricTwinComparison::execute() throw (BrainModelAlgorithmException)
{
   //
   // Make sure file names are specified
   //
   if (metricShapeTwinAFileName.isEmpty()) {
      throw BrainModelAlgorithmException("Twin File A filename is empty.");
   }
   if (metricShapeTwinBFileName.isEmpty()) {
      throw BrainModelAlgorithmException("Twin File B filename is empty.");
   }
   if (outputMetricShapeFileName.isEmpty()) {
      throw BrainModelAlgorithmException("Output filename is empty.");
   }
   
   //
   // Read in the input files
   //
   MetricFile metricFileA, metricFileB;
   try {
      metricFileA.readFile(metricShapeTwinAFileName);
      metricFileB.readFile(metricShapeTwinBFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
   //
   // Examine data files
   //
   const int numNodes = metricFileA.getNumberOfNodes();
   const int numCols  = metricFileA.getNumberOfColumns();
   if ((numNodes <= 0) ||
       (numCols  <= 0)) {
      const QString msg = FileUtilities::basename(metricShapeTwinAFileName)
                        + " contains no data.";
      throw BrainModelAlgorithmException(msg);
   }
   if (numNodes != metricFileB.getNumberOfNodes()) {
      throw BrainModelAlgorithmException("The input files contain a different number of nodes.");
   }
   if (numCols != metricFileB.getNumberOfColumns()) {
      throw BrainModelAlgorithmException("The input files contain a different number of columns.");
   }
   
   //
   // Create the output metric file
   //
   int numOutputColumns = 0;
   const int twinVarianceColumn = numOutputColumns++;
   const int nonTwinVarianceColumn = numOutputColumns++;
   const int differenceColumn = numOutputColumns++;
   MetricFile outputMetricFile;
   outputMetricFile.setNumberOfNodesAndColumns(numNodes, numOutputColumns);
   
   //
   // Set column names
   //
   outputMetricFile.setColumnName(twinVarianceColumn,
                                  "Twin Expected Variance");
   outputMetricFile.setColumnName(nonTwinVarianceColumn,
                                  "Non-Twin Expected Variance");
   outputMetricFile.setColumnName(differenceColumn,
                                  "Twin minus Non-Twin");

   //
   // Do twin differences
   //
   for (int i = 0; i < numNodes; i++) {
      //
      // Compute "expected variance" at each node
      //
      double nodeSum = 0.0;
      for (int j = 0; j < numCols; j++) {
         if ((i == 0) && DebugControl::getDebugOn()) {
            std::cout << "Twin comparison File A Column "
                      << j
                      << ", File B Column "
                      << j
                      << std::endl;
         }
         const float valA = metricFileA.getValue(i, j);
         const float valB = metricFileB.getValue(i, j);
         const float d = valA - valB;
         const float d2 = d * d;
         nodeSum += d2;
      }
      const float expectedVariance = nodeSum / static_cast<double>(numCols);
      outputMetricFile.setValue(i, twinVarianceColumn, expectedVariance);
   }
   
   //
   // Do Non-Twin pairs by pairing each twin to everyone but its twin
   // Sort of a "upper triangular matrix" comparison to avoid multiple comparisons
   //
   for (int i = 0; i < numNodes; i++) {
      //
      // Non-Twin comparisons within file A 
      //
      double nodeSum = 0.0;
      int comparisonCount = 0;
      for (int j = 0; j < (numCols - 1); j++) {
         const float valA1 = metricFileA.getValue(i, j);
         for (int k = j + 1; k < numCols; k++) {
            //
            // Compare to subject in file A
            //
            const float valA2 = metricFileA.getValue(i, k);
            const float da = valA1 - valA2;
            const float da2 = da * da;
            nodeSum += da2;
            comparisonCount++;
            if ((i == 0) && DebugControl::getDebugOn()) {
               std::cout << "Non-Twin comparison File A Column "
                         << j
                         << ", File A Column "
                         << k
                         << std::endl;
            }
         }  // for K
      }  // for J
            
      //
      // Non-Twin comparisons between files A & B
      //
      for (int j = 0; j < numCols; j++) {
         const float valA = metricFileA.getValue(i, j);
         for (int k = 0; k < numCols; k++) {
            //
            // Only do non-twins
            //
            if (j != k) {
               //
               // Compare to subject in file B
               //
               const float valB = metricFileB.getValue(i, k);
               const float db = valA - valB;
               const float db2 = db * db;
               nodeSum += db2;
               comparisonCount++;
               if ((i == 0) && DebugControl::getDebugOn()) {
                  std::cout << "Non-Twin comparison File A Column "
                            << j
                            << ", File B Column "
                            << k
                            << std::endl;
               }
            }
         }  // for K
      }  // for J
      
      //
      // Non-Twin comparisons withing file B
      //
      for (int j = 0; j < (numCols - 1); j++) {
         const float valB1 = metricFileB.getValue(i, j);
         for (int k = j + 1; k < numCols; k++) {
            //
            // Compare to subject in file B
            //
            const float valB2 = metricFileB.getValue(i, k);
            const float db = valB1 - valB2;
            const float db2 = db * db;
            nodeSum += db2;
            comparisonCount++;
            if ((i == 0) && DebugControl::getDebugOn()) {
               std::cout << "Non-Twin comparison File B Column "
                         << j
                         << ", File B Column "
                         << k
                         << std::endl;
            }
         }  // for K
      }  // for J

      const float expectedVariance = nodeSum / static_cast<double>(comparisonCount);
      outputMetricFile.setValue(i, nonTwinVarianceColumn, expectedVariance);
      
      if (i == 0) {
         std::cout << "Non-Twin Comparision count: " << comparisonCount << std::endl;
      }
   }  // for I
   
   try {
      //
      // Compute differences in the twin vs non-twin expected variances
      //
      outputMetricFile.performBinaryOperation(MetricFile::BINARY_OPERATION_SUBTRACT,
                                              twinVarianceColumn,
                                              nonTwinVarianceColumn,
                                              differenceColumn,
                                              outputMetricFile.getColumnName(differenceColumn));

      //
      // Set color mapping
      //
      outputMetricFile.setColorMappingToColumnMinMax();
      
      //
      // Write the metric file
      //
      outputMetricFile.writeFile(outputMetricShapeFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
   
}

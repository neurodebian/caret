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

#include "BrainModelSurfaceMetricInGroupDifference.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "MetricFile.h"

/**
 * constructor.
 */
BrainModelSurfaceMetricInGroupDifference::BrainModelSurfaceMetricInGroupDifference(
                                         BrainSet* bs,
                                         const QString& inputMetricShapeFileNameIn,
                                         const QString& outputMetricShapeFileNameIn,
                                         const bool absoluteValueFlagIn)
   : BrainModelAlgorithm(bs)
{
   inputMetricShapeFileName = inputMetricShapeFileNameIn;
   outputMetricShapeFileName = outputMetricShapeFileNameIn;
   absoluteValueFlag = absoluteValueFlagIn;
}

/**
 * destructor.
 */
BrainModelSurfaceMetricInGroupDifference::~BrainModelSurfaceMetricInGroupDifference()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceMetricInGroupDifference::execute() throw (BrainModelAlgorithmException)
{
   //
   // Make sure file names are specified
   //
   if (inputMetricShapeFileName.isEmpty()) {
      throw BrainModelAlgorithmException("Input filename is empty.");
   }
   if (outputMetricShapeFileName.isEmpty()) {
      throw BrainModelAlgorithmException("Output filename is empty.");
   }
   
   //
   // Read in the input files
   //
   MetricFile inputMetricFile;
   try {
      inputMetricFile.readFile(inputMetricShapeFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
   //
   // Examine data files
   //
   const int numNodes = inputMetricFile.getNumberOfNodes();
   const int numCols  = inputMetricFile.getNumberOfColumns();
   if ((numNodes <= 0) ||
       (numCols  <= 0)) {
      const QString msg = FileUtilities::basename(inputMetricShapeFileName)
                        + " contains no data.";
      throw BrainModelAlgorithmException(msg);
   }
   if (numCols < 2) {
      throw BrainModelAlgorithmException("The input metric file must contain at least two columns.");
   }
   
   //
   // Get the number of combinations which will be the number of columns in the output files
   //
   const int numOutputColumns = MathUtilities::combinations(numCols, 2);
   
   //
   // Create the output metric file
   //
   MetricFile outputMetricFile;
   outputMetricFile.setNumberOfNodesAndColumns(numNodes, numOutputColumns);
   
   //
   // Pair up each possible combination of nodes
   //
   int currentColumn = 0;
   for (int j = 0; j < (numCols - 1); j++) {
      for (int k = (j + 1); k < numCols; k++) {
         if (currentColumn >= numOutputColumns) {
            const QString msg =
               "Compute number of column combinations " + QString::number(numOutputColumns) + "\n"
               + "has been exceeded when combining columns \n"
               + QString::number(j) + " and " + QString::number(k);
            throw BrainModelAlgorithmException(msg);
         }
         
         //
         // Set the column name
         //
         const QString columnName = "Columns " + QString::number(j)
                                    + " and " + QString::number(k);
         outputMetricFile.setColumnName(currentColumn, columnName);
         
         //
         // Set column comment
         //
         const QString columnComment = inputMetricFile.getColumnName(j)
                                       + " AND "
                                       + inputMetricFile.getColumnName(k);
         outputMetricFile.setColumnComment(currentColumn, columnComment);
         
         //
         // Compute the differences for each node
         //
         for (int i = 0; i < numNodes; i++) {
            float diff = inputMetricFile.getValue(i, j) - inputMetricFile.getValue(i, k);
            if (absoluteValueFlag) {
               if (diff < 0) {
                  diff = -diff;
               }
            }
            outputMetricFile.setValue(i, currentColumn, diff);
         }
         
         //
         // Next output column
         //
         currentColumn++;
      }
   }

   try {
      //
      // Write the metric file
      //
      outputMetricFile.writeFile(outputMetricShapeFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
   
}

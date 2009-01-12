
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
#include <cmath>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceROIProbAtlasOverlapReport.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "NameIndexSort.h"
#include "ProbabilisticAtlasFile.h"
#include "StringUtilities.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceROIProbAtlasOverlapReport::BrainModelSurfaceROIProbAtlasOverlapReport(
                           BrainSet* bs,
                           BrainModelSurface* bmsIn,
                           BrainModelSurfaceROINodeSelection* inputSurfaceROIIn,
                           ProbabilisticAtlasFile* probAtlasFileIn,
                           const QString& headerTextIn,
                           const bool semicolonSeparateReportFlagIn)
   : BrainModelSurfaceROIOperation(bs,
                                   bmsIn,
                                   inputSurfaceROIIn)
{
   probAtlasFile         = probAtlasFileIn;
   semicolonSeparateReportFlag = semicolonSeparateReportFlagIn;
   setHeaderText(headerTextIn);
}

/**
 * destructor.
 */
BrainModelSurfaceROIProbAtlasOverlapReport::~BrainModelSurfaceROIProbAtlasOverlapReport()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROIProbAtlasOverlapReport::executeOperation() throw (BrainModelAlgorithmException)
{
   const int numNodes = probAtlasFile->getNumberOfNodes();
   const int numCols  = probAtlasFile->getNumberOfColumns();
   
   if ((numNodes <= 0) || (numCols <= 0)) {
      throw BrainModelAlgorithmException("The Probabilistic Atlas File is Empty.");
   }
   
   const int numNames = probAtlasFile->getNumberOfPaintNames();
   if (numNames <= 0) {
      throw BrainModelAlgorithmException("No names in Probabilistic Atlas File.");
   }

   float roiArea = 0.0;
   createReportHeader(roiArea);

   QString separator(" ");
   if (semicolonSeparateReportFlag) {
      separator = ";";
   }

   const int numNodesSelected = operationSurfaceROI->getNumberOfNodesSelected();
   
   const int countCols = numCols + 1;
   int* counts = new int[countCols];

   //
   // Sort the paint names
   //
   std::vector<int> indices;
   std::vector<QString> names;
   for (int i = 0; i < numNames; i++) {
      indices.push_back(i);
      names.push_back(probAtlasFile->getPaintNameFromIndex(i));
   }
   NameIndexSort sortNames(indices, names);
   
   if (semicolonSeparateReportFlag) {
      QString str;
      str += "Name ";
      for (int j = 0; j < countCols; j++) {
         str += (separator + QString::number(j));
      }
      str += "\n";
      reportText.append(str);
   }
   
   //
   // Loop through paint names
   //      
   for (int m = 0; m < numNames; m++) {
      //
      // Get the paint name and index
      //
      int indx = -1;
      QString name;
      sortNames.getSortedNameAndIndex(m, indx, name);
      
      //
      // Clear counts
      //
      for (int j = 0; j < countCols; j++) {
         counts[j] = 0;
      }
      
      //
      // Find nodes using paint
      //
      for (int i = 0; i < numNodes; i++) {
         if (operationSurfaceROI->getNodeSelected(i)) {
            int cnt = 0;
            for (int j = 0; j < numCols; j++) {
               if (probAtlasFile->getPaint(i, j) == indx) {
                  cnt++;
               }
            }
            counts[cnt]++;
         }
      }
      
      QString str;
      
      if (semicolonSeparateReportFlag) {
         str += name + "\n";
         for (int j = 0; j < countCols; j++) {
            if (j > 0) {
               str += separator;
            }
            str += QString::number(counts[j]);
         }
      }
      else {
         str += name + "\n";
         for (int j = 0; j < countCols; j++) {
            str += (QString::number(counts[j]) 
                    + " with " 
                    + QString::number(j) 
                    + " entries ");
         }
      }
      str += "\n";
      reportText.append(str);
      
      if (semicolonSeparateReportFlag) {
         str = " ";
         for (int j = 0; j < countCols; j++) {
            const float pct = (static_cast<float>(counts[j]) /
                               static_cast<float>(numNodesSelected)) * 100.0;
            if (j > 0) {
               str += separator;
            }
            str += QString::number(pct, 'f', 3);
         }
         str += "\n";
      }
      else {
         str = " ";
         for (int j = 0; j < countCols; j++) {
            const float pct = (static_cast<float>(counts[j]) /
                               static_cast<float>(numNodesSelected)) * 100.0;
            str += (QString::number(pct, 'f', 3)
                    + "% with " 
                    + QString::number(j) 
                    + " entries ");
         }
         str += "\n\n";
      }
      reportText.append(str);
      
      reportText.append(" ");
   }
   
   delete[] counts;
/*   
   TopologyFile* tf = bms->getTopologyFile();
   if (tf != NULL) {
      std::vector<float> areaTimesFoldingSum(numColumns, 0.0);
      
      const int numTriangles = tf->getNumberOfTiles();
      for (int i = 0; i < numTriangles; i++) {
         if (tileInROI[i]) {
            int n1, n2, n3;
            tf->getTile(i, n1, n2, n3);
            
            for (int j = 0; j < numColumns; j++) {                  
               //
               // Average folding index is:
               //    SUM(Ai * Abs(Fi)) / SUM(Ai);
               //
               //       Ai = area of triangle (less if not all nodes in ROI)
               //       Fi = average shape value of triangle's three nodes
               //       SUM(Ai) is the same as the ROI's area
               //
               float shapeSum = 0.0;
               float numInROI = 0.0;
               if (operationSurfaceROI->getNodeSelected(n1)) {
                  shapeSum += surfaceShapeFile->getValue(n1, j);
                  numInROI += 1.0;
               }
               if (operationSurfaceROI->getNodeSelected(n2)) {
                  shapeSum += surfaceShapeFile->getValue(n2, j);
                  numInROI += 1.0;
               }
               if (operationSurfaceROI->getNodeSelected(n3)) {
                  shapeSum += surfaceShapeFile->getValue(n3, j);
                  numInROI += 1.0;
               }
               if (numInROI > 0.0) {
                  const float averageShape = std::fabs(shapeSum) / numInROI;
                  areaTimesFoldingSum[j] += averageShape * tileArea[i];
               }
            }
         }
      }
      
      int longestColumnNameLength = 10;
      for (int j = 0; j < numColumns; j++) {
         longestColumnNameLength = std::max(longestColumnNameLength,
                  static_cast<int>(surfaceShapeFile->getColumnName(j).length()));
      }
      longestColumnNameLength += 5;
      
      QString s = StringUtilities::leftJustify("Name", longestColumnNameLength)
                  + StringUtilities::rightJustify("IFI", 12)
                  + "\n";
      reportText.append(s);
      
      for (int j = 0; j < numColumns; j++) {
         const float integratedFloatingIndex = areaTimesFoldingSum[j] / roiArea;
         
         QString s = StringUtilities::leftJustify(surfaceShapeFile->getColumnName(j), longestColumnNameLength)
                     + StringUtilities::rightJustify(QString::number(integratedFloatingIndex, 'f', 6), 12)
                     + "\n";
         reportText.append(s);
      }
   }
*/
}

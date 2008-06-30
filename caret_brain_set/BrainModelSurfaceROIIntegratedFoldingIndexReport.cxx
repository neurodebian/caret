
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
#include "BrainModelSurfaceROIIntegratedFoldingIndexReport.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceROIIntegratedFoldingIndexReport::BrainModelSurfaceROIIntegratedFoldingIndexReport(
                                 BrainSet* bs,
                                 BrainModelSurface* bmsIn,
                                 BrainModelSurfaceROINodeSelection* inputSurfaceROIIn,
                                 SurfaceShapeFile* surfaceShapeFileIn,
                                 const QString& headerTextIn,
                                 const bool semicolonSeparateReportFlagIn)
   : BrainModelSurfaceROIOperation(bs,
                                   bmsIn,
                                   inputSurfaceROIIn)
{
   surfaceShapeFile      = surfaceShapeFileIn;
   semicolonSeparateReportFlag = semicolonSeparateReportFlagIn;
   setHeaderText(headerTextIn);
}

/**
 * destructor.
 */
BrainModelSurfaceROIIntegratedFoldingIndexReport::~BrainModelSurfaceROIIntegratedFoldingIndexReport()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROIIntegratedFoldingIndexReport::executeOperation() throw (BrainModelAlgorithmException)
{
   const int numColumns = surfaceShapeFile->getNumberOfColumns();
   if (numColumns <= 0) {
      throw BrainModelAlgorithmException("No surface shape file contains no data.");
   }

   QString separator("");
   if (semicolonSeparateReportFlag) {
      separator = ";";
   }
   float roiArea = 0.0;
   createReportHeader(roiArea);
   
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
                  + separator
                  + StringUtilities::rightJustify("IFI", 12)
                  + "\n";
      reportText.append(s);
      
      for (int j = 0; j < numColumns; j++) {
         const float integratedFloatingIndex = areaTimesFoldingSum[j] / roiArea;
         
         QString s = StringUtilities::leftJustify(surfaceShapeFile->getColumnName(j), longestColumnNameLength)
                     + separator
                     + StringUtilities::rightJustify(QString::number(integratedFloatingIndex, 'f', 6), 12)
                     + "\n";
         reportText.append(s);
      }
   }
}

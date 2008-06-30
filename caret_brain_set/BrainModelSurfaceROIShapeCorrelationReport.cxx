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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROIShapeCorrelationReport.h"
#include "SurfaceShapeFile.h"

/**
 * Constructor.
 */
BrainModelSurfaceROIShapeCorrelationReport::BrainModelSurfaceROIShapeCorrelationReport(
                                       BrainSet* bs,
                                       BrainModelSurface* bmsIn,
                                       BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                       SurfaceShapeFile* shapeFileIn,
                                       const int shapeReferenceColumnNumberIn,
                                       const bool separateWithSemicolonsFlagIn)
   : BrainModelSurfaceROIOperation(bs,
                                   bmsIn,
                                   surfaceROIIn)
{
   shapeFile = shapeFileIn;
   shapeReferenceColumnNumber = shapeReferenceColumnNumberIn;
   separateWithSemicolonsFlag = separateWithSemicolonsFlagIn;
}
                                 
/**
 * destructor.
 */
BrainModelSurfaceROIShapeCorrelationReport::~BrainModelSurfaceROIShapeCorrelationReport()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROIShapeCorrelationReport::executeOperation() throw (BrainModelAlgorithmException)
{
   reportText = "";
   setHeaderText("Shape Correlation Report");
   
   const int numColumns = shapeFile->getNumberOfColumns();
   if (numColumns <= 0) {
      throw BrainModelAlgorithmException("No surface shape file contains no data.");
   }

   QString separator(" ");
   if (separateWithSemicolonsFlag) {
      separator = ";";
   }
   float roiArea = 0.0;
   createReportHeader(roiArea);
   
   std::vector<bool> nodesAreInROI;
   operationSurfaceROI->getNodesInROI(nodesAreInROI);
   
   std::vector<float> coefficients;
   if (shapeReferenceColumnNumber < 0) {
      const int lastColumn = shapeFile->getNumberOfColumns() - 1;
      QString str;
      for (int j = 0; j < shapeFile->getNumberOfColumns(); j++) {
         str += (shapeFile->getColumnName(j));
         if (j != lastColumn) {
             str += separator;
         }
      }
      str += "\n";
      reportText.append(str);
      
      for (int j = 0; j < shapeFile->getNumberOfColumns(); j++) {
         shapeFile->correlationCoefficient(j,
                                     coefficients,
                                     &nodesAreInROI);
         QString str;
         for (int i = 0; i < shapeFile->getNumberOfColumns(); i++) {
            str += QString::number(coefficients[i],'f', 5);
            if (i != lastColumn) {
               str += separator;
            }
         }
         str += "\n";
         reportText.append(str);
      }
   }
   else {
      shapeFile->correlationCoefficient(shapeReferenceColumnNumber,
                                  coefficients,
                                  &nodesAreInROI);

      QString str;
      str += ("\n"
              "Correlation for "
              + shapeFile->getColumnName(shapeReferenceColumnNumber)
              + "\n");
      reportText.append(str);
      
      for (int i = 0; i < shapeFile->getNumberOfColumns(); i++) {
         str = "";
         str += (QString::number(i + 1) 
                 + separator
                 + shapeFile->getColumnName(i)
                 + separator
                 + QString::number(coefficients[i], 'f', 5)
                 + "\n");
         reportText.append(str);
      }
   }
   
}

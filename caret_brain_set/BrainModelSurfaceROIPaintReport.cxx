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

#include <QStringList>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROIPaintReport.h"
#include "BrainModelSurfaceROITextReport.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "SurfaceShapeFile.h"

/**
 * constructor.
 */
BrainModelSurfaceROIPaintReport::BrainModelSurfaceROIPaintReport(BrainSet* bs,
                                        BrainModelSurface* bmsIn,
                                        BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                        MetricFile* metricFileIn,
                                        const std::vector<bool>& selectedMetricColumnsForReportIn,
                                        MetricFile* shapeFileIn,
                                        const std::vector<bool>& selectedShapeColumnsForReportIn,
                                        PaintFile* paintFileIn,
                                        const std::vector<bool>& selectedPaintColumnsForReportIn,
                                        const int paintRegionColumnNumberIn,
                                        LatLonFile* latLonFileIn,
                                        const int latLonFileColumnIn,
                                        MetricFile* metricCorrectionFileIn,
                                        const int metricCorrectionColumnIn,
                                        const bool semicolonSeparateReportFlagIn)
   : BrainModelSurfaceROIOperation(bs, bmsIn, surfaceROIIn)
{
   reportLatLonFile = NULL;
   reportMetricFile = NULL;
   reportShapeFile = NULL;
   reportPaintFile = NULL;
   reportMetricCorrectionFile = NULL;

   reportMetricFile = metricFileIn;
   selectedMetricColumnsForReport = selectedMetricColumnsForReportIn;
   reportShapeFile = shapeFileIn;
   selectedShapeColumnsForReport  = selectedShapeColumnsForReportIn;
   reportPaintFile = paintFileIn;
   selectedPaintColumnsForReport  = selectedPaintColumnsForReportIn;
   paintRegionColumnNumber = paintRegionColumnNumberIn;
   reportLatLonFile = latLonFileIn;
   reportLatLonFileColumn = latLonFileColumnIn;
   setHeaderText("Paint Region Report");
   reportMetricCorrectionFile = metricCorrectionFileIn;
   metricCorrectionColumn = metricCorrectionColumnIn;
   semicolonSeparateReportFlag = semicolonSeparateReportFlagIn;
}                                  

/**
 * destructor.
 */
BrainModelSurfaceROIPaintReport::~BrainModelSurfaceROIPaintReport()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROIPaintReport::executeOperation() throw (BrainModelAlgorithmException)
{
   reportText = "";

   if (reportPaintFile->getNumberOfColumns() <= 0) {
      throw BrainModelAlgorithmException("Paint file is empty.");
   }
   if ((paintRegionColumnNumber < 0) ||
       (paintRegionColumnNumber >= reportPaintFile->getNumberOfColumns())) {
      throw BrainModelAlgorithmException("Region paint column is invalid.");
   }

   QString paintReportText;
   
   //
   // Get all of the paint names for the column
   //
   std::vector<int> paintIndices;
   reportPaintFile->getPaintNamesForColumn(paintRegionColumnNumber, paintIndices);
   
   //
   // process each paint index
   //
   const int numPaintIndices = static_cast<int>(paintIndices.size());
   for (int i = 0; i < numPaintIndices; i++) {
      //
      // Get the index
      //
      const int paintIndex = paintIndices[i];
      
      //
      // Save the selected nodes
      //
      const int numNodes = reportPaintFile->getNumberOfNodes();
      std::vector<int> savedNodeInROI(numNodes, 0);
      
      //
      // Limit nodes in ROI to those with valid paint column
      //
      for (int j = 0; j < numNodes; j++) {
         savedNodeInROI[j] = operationSurfaceROI->getNodeSelected(j);
         if (operationSurfaceROI->getNodeSelected(j)) {
            operationSurfaceROI->setNodeSelected(j, false);
            if (reportPaintFile->getPaint(j, paintRegionColumnNumber) == paintIndex) {
               operationSurfaceROI->setNodeSelected(j, true);
            }
         }
      }
      
      //
      // Run the report for this paint
      //
      const QString str = ("Paint Subregion Name: "
                           + reportPaintFile->getPaintNameFromIndex(paintIndex));
      
      reportText = "";
      BrainModelSurfaceROITextReport roiReport(brainSet,
                                               bms,
                                               operationSurfaceROI,
                                               reportMetricFile,
                                               selectedMetricColumnsForReport,
                                               reportShapeFile,
                                               selectedShapeColumnsForReport,
                                               reportPaintFile,
                                               selectedPaintColumnsForReport,
                                               reportLatLonFile,
                                               0,
                                               str,
                                               reportMetricFile,
                                               metricCorrectionColumn,
                                               semicolonSeparateReportFlag);
      try {
         roiReport.execute();
         paintReportText += roiReport.getReportText();
      }
      catch (BrainModelAlgorithmException& e) {
         throw BrainModelAlgorithmException(e.whatQString());
      }
      
      //
      // Restore the selected nodes
      //
      for (int j = 0; j < numNodes; j++) {
         operationSurfaceROI->setNodeSelected(j, (savedNodeInROI[j] != 0));
      }
   }
   
   reportText = paintReportText;
}


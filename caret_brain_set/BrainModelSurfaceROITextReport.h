
#ifndef __BRAIN_MODEL_SURFACE_ROI_TEXT_REPORT_H__
#define __BRAIN_MODEL_SURFACE_ROI_TEXT_REPORT_H__

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

#include <vector>

#include "BrainModelSurfaceROIOperation.h"

class CoordinateFile;
class LatLonFile;
class MetricFile;
class PaintFile;
class SurfaceShapeFile;
class TopologyFile;

/// class for performing region of interest operations on a surface
class BrainModelSurfaceROITextReport : public BrainModelSurfaceROIOperation {
   public:      
      // constructor
      BrainModelSurfaceROITextReport(BrainSet* bs,
                                        const BrainModelSurface* bmsIn,
                                        const BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                        MetricFile* metricFileIn,
                                        const std::vector<bool>& selectedMetricColumnsForReportIn,
                                        MetricFile* shapeFileIn,
                                        const std::vector<bool>& selectedShapeColumnsForReportIn,
                                        PaintFile* paintFileIn,
                                        const std::vector<bool>& selectedPaintColumnsForReportIn,
                                        LatLonFile* latLonFileIn,
                                        const int latLonFileColumnIn,
                                        const QString& headerTextIn,
                                        MetricFile* metricCorrectionFileIn,
                                        const int metricCorrectionColumnIn,
                                        const bool tabSeparateReportFlagIn);
      
      // destructor
      ~BrainModelSurfaceROITextReport();
      
   protected:
      // execute the operation
      void executeOperation() throw (BrainModelAlgorithmException);
      
      // create the text report
      void createTextReport() throw (BrainModelAlgorithmException);
      
      // Peform Metric and Surface Shape Report.
      void metricAndSurfaceShapeReport(const bool metricFlag);

      // perform paint report
      void paintReport(const float roiArea);
      
      /// selected metric columns
      std::vector<bool> selectedMetricColumnsForReport;
      
      /// selected surface shape columns
      std::vector<bool> selectedShapeColumnsForReport;
      
      /// selected paint columns
      std::vector<bool> selectedPaintColumnsForReport;
      
      /// lat lon file for report
      LatLonFile* reportLatLonFile;
      
      /// column for report lat lon file
      int reportLatLonFileColumn;
      
      /// metric file for report
      MetricFile* reportMetricFile;
      
      /// shape file for report
      MetricFile* reportShapeFile;
      
      /// paint file for report
      PaintFile* reportPaintFile;
      
      /// metric correction file for report
      MetricFile* reportMetricCorrectionFile;
      
      /// the metric correction column
      int metricCorrectionColumn;
      
      /// tab separate the report
      bool tabSeparateReportFlag;
};

#endif // __BRAIN_MODEL_SURFACE_ROI_TEXT_REPORT_H__



#ifndef __BRAIN_MODEL_SURFACE_ROI_ASSIGN_METRIC_NODE_AREA_H__
#define __BRAIN_MODEL_SURFACE_ROI_ASSIGN_METRIC_NODE_AREA_H__

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

#include "BrainModelSurfaceROIOperation.h"

class MetricFile;

/// class for assigning metrics using an ROI
class BrainModelSurfaceROIAssignMetricNodeArea : public BrainModelSurfaceROIOperation {
   public:
      // constructor
      BrainModelSurfaceROIAssignMetricNodeArea(BrainSet* bs,
                                       BrainModelSurface* bmsIn,
                                       BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                       MetricFile* metricFileIn,
                                       const int metricColumnNumberIn,
                                       const QString& metricColumnNameIn,
                                       const bool percentageAreaFlagIn);
                                       
      // destructor
      ~BrainModelSurfaceROIAssignMetricNodeArea();
      
      // get assigned metric column
      int getAssignedMetricColumn() const { return metricColumnNumber; };
      
   protected:
      // execute the operation
      virtual void executeOperation() throw (BrainModelAlgorithmException);
      
      /// the metric file
      MetricFile* metricFile;
      
      /// metric column number
      int metricColumnNumber;
      
      /// name for metric column
      const QString metricColumnName;
      
      /// do percentage of surface area
      const bool percentageAreaFlag;
};

#endif // __BRAIN_MODEL_SURFACE_ROI_ASSIGN_METRIC_NODE_AREA_H__

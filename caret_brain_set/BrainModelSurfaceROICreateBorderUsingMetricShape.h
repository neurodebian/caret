
#ifndef __BRAIN_MODEL_SURFACE_ROI_CREATE_BORDER_USING_METRIC_SHAPE_H__
#define __BRAIN_MODEL_SURFACE_ROI_CREATE_BORDER_USING_METRIC_SHAPE_H__

#include "BrainModelAlgorithm.h"

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

#include "BorderFile.h"
#include "BrainModelSurfaceROIOperation.h"

class BrainModelSurface;
class BrainModelSurfaceROINodeSelection;
class MetricFile;

/// class for drawing roughly linear borders along nods with metric or shape values
class BrainModelSurfaceROICreateBorderUsingMetricShape : public BrainModelSurfaceROIOperation {
   public:
      /// mode
      enum MODE {
         /// follow nodes with most negative values 
         MODE_FOLLOW_MOST_NEGATIVE,
         /// follow nodes with most positive values
         MODE_FOLLOW_MOST_POSITIVE
      };
      
      // constructor
      BrainModelSurfaceROICreateBorderUsingMetricShape(BrainSet* bs,
                                        BrainModelSurface* bmsIn,
                                        BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                        const MODE modeIn,
                                        MetricFile* metricFileIn,
                                        const int metricColumnNumberIn,
                                        const QString& borderNameIn,
                                        const int startNodeIn,
                                        const int endNodeIn,
                                        const float samplingDensityIn);
      
      // destructor
      ~BrainModelSurfaceROICreateBorderUsingMetricShape();
      
      // execute the operation
      void executeOperation() throw (BrainModelAlgorithmException);
      
      // get the border that was created by create border mode
      Border getBorder() const; 
      
   protected:
      /// metric file
      MetricFile* metricFile;
      
      /// metric column number
      int metricColumnNumber;
      
      /// name for border
      QString borderName;
      
      /// border created
      Border border;

      /// border start node
      int borderStartNode;
      
      /// border end node
      int borderEndNode;
      
      /// border sampling density
      float borderSamplingDensity;
      
      /// mode for search
      MODE mode;
};
 
#endif // __BRAIN_MODEL_SURFACE_ROI_CREATE_BORDER_USING_METRIC_SHAPE_H__

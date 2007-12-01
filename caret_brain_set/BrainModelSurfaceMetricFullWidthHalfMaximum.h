
#ifndef __BRAIN_MODEL_SURFACE_METRIC_FULL_WIDTH_HALF_MAXIMUM_H__
#define __BRAIN_MODEL_SURFACE_METRIC_FULL_WIDTH_HALF_MAXIMUM_H__

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

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class MetricFile;

/// determine the full width half maximum for a metric column
class BrainModelSurfaceMetricFullWidthHalfMaximum : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceMetricFullWidthHalfMaximum(BrainSet* bs,
                                                  BrainModelSurface* brainModelSurfaceIn,
                                                  MetricFile* metricFileIn,
                                                  const int metricColumnIn);
      
      // destructor
      ~BrainModelSurfaceMetricFullWidthHalfMaximum();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      // get the resulting full width half maximum measurement
      float getFullWidthHalfMaximum() const { return fullWidthHalfMaximum; }
      
   protected:
      // the surface on which to smooth
      BrainModelSurface* brainModelSurface;
      
      // the metric file to smooth
      MetricFile* metricFile;
      
      // metric column
      int metricColumn;
      
      // the full width half maximum
      float fullWidthHalfMaximum;
};

#endif // __BRAIN_MODEL_SURFACE_METRIC_FULL_WIDTH_HALF_MAXIMUM_H__

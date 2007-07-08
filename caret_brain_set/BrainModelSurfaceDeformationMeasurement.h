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

#ifndef __BRAIN_MODEL_SURFACE_DEFORMATION_MEASUREMENT_H__
#define __BRAIN_MODEL_SURFACE_DEFORMATION_MEASUREMENT_H__

#include "BrainModelAlgorithm.h"

class BorderFile;
class BrainModelSurface;
class MetricFile;

/// class for performing a deformation measurement
class BrainModelSurfaceDeformationMeasurement : public BrainModelAlgorithm {
   public:
      /// constructor
      BrainModelSurfaceDeformationMeasurement(BrainSet* bs,
                                              BrainModelSurface* bmsIn,
                                              BorderFile* borderFileIn,
                                              MetricFile* metricFileIn,
                                              const int sphereNumberIn);
                                              
      /// Destructor
      ~BrainModelSurfaceDeformationMeasurement();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// the surface being examined
      BrainModelSurface* bms;
      
      /// the borders used for measurement
      BorderFile* borderFile;
      
      /// the metric file into which the measurements should be placed
      MetricFile* userMetricFile;
      
      /// sphere resolution
      int sphereNumber;
};

#endif // __BRAIN_MODEL_SURFACE_DEFORMATION_MEASUREMENT_H__

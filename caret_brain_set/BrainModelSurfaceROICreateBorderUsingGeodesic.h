
#ifndef __BRAIN_MODEL_SURFACE_ROI_CREATE_BORDER_USING_GEODESIC_H__
#define __BRAIN_MODEL_SURFACE_ROI_CREATE_BORDER_USING_GEODESIC_H__

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

/// class for performing creating a borders using geodesic distances
class BrainModelSurfaceROICreateBorderUsingGeodesic : public BrainModelSurfaceROIOperation {
   public:
      // constructor
      BrainModelSurfaceROICreateBorderUsingGeodesic(BrainSet* bs,
                                        BrainModelSurface* bmsIn,
                                        BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                        const QString& borderNameIn,
                                        const int startNodeIn,
                                        const int endNodeIn,
                                        const float samplingDensityIn);
      
      // destructor
      ~BrainModelSurfaceROICreateBorderUsingGeodesic();
      
      // execute the operation
      void executeOperation() throw (BrainModelAlgorithmException);
      
      // get the border that was created by create border mode
      Border getBorder() const;      

   protected:       
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
};

#endif // __BRAIN_MODEL_SURFACE_ROI_CREATE_BORDER_USING_GEODESIC_H__


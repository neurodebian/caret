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

#ifndef __BRAIN_MODEL_SURFACE_FLATTEN_FULL_HEMISPHERE_H__
#define __BRAIN_MODEL_SURFACE_FLATTEN_FULL_HEMISPHERE_H__

#include "BrainModelAlgorithm.h"

class BorderFile;
class BrainModelSurface;
class PaintFile;

/// class for flattening a full hemisphere
class BrainModelSurfaceFlattenFullHemisphere : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelSurfaceFlattenFullHemisphere(BrainModelSurface* fiducialSurfaceIn,
                                             BrainModelSurface* ellipsoidSurfaceIn,
                                             BorderFile* borderFileIn,
                                             const float acPositionIn[3],
                                             const float acOffsetIn[3],
                                             const bool smoothFiducialMedialWallFlagIn);
      
      /// Destructor
      ~BrainModelSurfaceFlattenFullHemisphere();
      
      /// Execute the flattening
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// Execute the second half of the flattening
      virtual void executePart2() throw (BrainModelAlgorithmException);

   protected:
      /// save the borders for creating landmark borders
      void saveBordersForLandmarks(BorderFile* bf);
      
      /// create the landarks for deformation
      void createDeformationBorders(PaintFile* pf);
      
      /// fiducial surface
      BrainModelSurface* fiducialSurface;
      
      /// partial hemisphere surface being flattened
      BrainModelSurface* flattenSurface;
      
      /// position of anterior commissure
      float acPosition[3];
      
      /// offset from anterior commissure
      float acOffset[3];
      
      /// template border file name
      BorderFile* borderFile;
      
      /// the geography column number
      int geographyColumnNumber;
      
      /// medial wall paint file index
      int medialWallPaintFileIndex;
      
      /// hemisphere string for file naming
      QString hemStr;
      
      /// original topology file name
      QString originalTopoFileName;
      
      /// original coordinate file name
      QString originalCoordFileName;
      
      /// landmarks for deformation
      BorderFile landmarksForDeformation;
      
      /// paint file was empty at start of procedure
      bool paintFileEmptyAtStart;
      
      /// smooth the fiducial surface's medial wall during flattening
      bool smoothFiducialMedialWallFlag;
};

#endif // __BRAIN_MODEL_SURFACE_FLATTEN_FULL_HEMISPHERE_H__


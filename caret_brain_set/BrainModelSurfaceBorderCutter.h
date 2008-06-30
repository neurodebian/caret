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

#ifndef __BRAIN_MODEL_SURFACE_BORDER_CUTTER_H__
#define __BRAIN_MODEL_SURFACE_BORDER_CUTTER_H__

#include "BrainModelAlgorithm.h"
#include "BorderFile.h"
#include "BorderProjectionFile.h"

class BrainModelSurface;
class TopologyFile;

/// This class applies borers as cuts to a surface
class BrainModelSurfaceBorderCutter : public BrainModelAlgorithm {
   public:
      /// mode of cutting
      enum CUTTING_MODE {
         CUTTING_MODE_FLAT_SURFACE,
         CUTTING_MODE_NON_NEGATIVE_Z_ONLY,
         CUTTING_MODE_SPHERICAL_SURFACE
      };
      
      /// Constructor
      BrainModelSurfaceBorderCutter(BrainSet* bsIn,
                                    BrainModelSurface* cuttingSurfaceIn,
                                    const BorderProjectionFile* cutBorderProjectionsIn,
                                    const CUTTING_MODE cuttingModeIn,
                                    const bool extendBordersToNearestEdgeNodeFlag);
      
      /// Destructor
      ~BrainModelSurfaceBorderCutter();
      
      /// Execute the flattening
      virtual void execute() throw (BrainModelAlgorithmException);
      
   protected:
      // extend the border to nearest edge node
      void extendBorderToNearestEdgeNode(Border& border);
      
      /// surface to which cuts are applied
      BrainModelSurface* cuttingSurface;
      
      /// the border projections to be used as cuts
      const BorderProjectionFile* cutBorderProjectionFile;
      
      /// cutting mode
      const CUTTING_MODE cuttingMode;
      
      /// extend borders to nearest edge node flag
      const bool extendBordersToNearestEdgeNodeFlag;
};

#endif // __BRAIN_MODEL_SURFACE_BORDER_CUTTER_H__


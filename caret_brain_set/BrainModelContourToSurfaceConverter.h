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



#ifndef __BRAIN_MODEL_CONTOUR_TO_SURFACE_CONVERTER_H__
#define __BRAIN_MODEL_CONTOUR_TO_SURFACE_CONVERTER_H__

#include "BrainModelAlgorithm.h"
#include "Structure.h"
class ContourFile;

/// Convert contours into a surface
class BrainModelContourToSurfaceConverter : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelContourToSurfaceConverter(BrainSet* bs,
                                          ContourFile* contourFileIn,
                                          const int voxelDimIn,
                                          const int polygonLimitIn,
                                          const Structure::STRUCTURE_TYPE structureIn,
                                          const bool convertCellsIn);
                                           
      /// Destructor
      ~BrainModelContourToSurfaceConverter();
                                           
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
      
   private:
      /// the contour file
      ContourFile* contourFile;
      
      /// voxel dimension
      int voxelDimension;
      
      /// polygon limit
      int polygonLimit;
      
      /// structure
      Structure::STRUCTURE_TYPE structure;
      
      /// convert cells flag
      bool convertCellsFlag;
};

#endif // __BRAIN_MODEL_CONTOUR_TO_SURFACE_CONVERTER_H__



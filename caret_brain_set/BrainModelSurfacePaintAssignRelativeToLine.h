
#ifndef __BRAIN_MODEL_SURFACE_PAINT_ASSIGN_RELATIVE_TO_LINE_H__
#define __BRAIN_MODEL_SURFACE_PAINT_ASSIGN_RELATIVE_TO_LINE_H__

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

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class PaintFile;

/// class for assigning paint nodes relative to a line
class BrainModelSurfacePaintAssignRelativeToLine : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfacePaintAssignRelativeToLine(BrainSet* brainSetIn,
                                                 BrainModelSurface * bmsIn,
                                                 PaintFile* paintFileIn,
                                                 const int paintFileColumnNumberIn,
                                                 const int augmentPaintRegionNameIndexIn,
                                                 const int newPaintNameIndexIn,
                                                 const float lineStartXYZIn[3],
                                                 const float lineEndXYZIn[3],
                                                 const float minimumDistanceToLineIn,
                                                 const float maximumDistanceToLineIn,
                                                 const std::vector<int>& limitToPaintIndicesIn,
                                                 const float nodeExtentLimitIn[6]);
      
      // destructor
      ~BrainModelSurfacePaintAssignRelativeToLine();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
   
   protected:
      /// the surface
      BrainModelSurface * bms;
      
      /// the paint file
      PaintFile* paintFile;
      
      /// number of paint column
      int paintFileColumnNumber;
      
      /// name of region for augmenting
      int augmentPaintRegionNameIndex;
      
      /// index for new paint assignment
      int newPaintNameIndex;
      
      /// start of line
      float lineStartXYZ[3];
      
      /// end of line
      float lineEndXYZ[3];
      
      /// minimum distance to line
      float minimumDistanceToLine;
      
      /// maximum distance to line
      float maximumDistanceToLine;
      
      /// limit assignments to these paint indices
      std::vector<int> limitToPaintIndices;
      
      /// absolute node XYZ limits
      float nodeExtentLimit[6];

};

#endif // __BRAIN_MODEL_SURFACE_PAINT_ASSIGN_RELATIVE_TO_LINE_H__

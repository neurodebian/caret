
#ifndef __BRAIN_MODEL_SURFACE_PAINT_TO_BORDER_CONVERTER_H__
#define __BRAIN_MODEL_SURFACE_PAINT_TO_BORDER_CONVERTER_H__

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
class PaintFile;

/// algorithm that creates borders around each paint region
class BrainModelSurfacePaintToBorderConverter : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfacePaintToBorderConverter(BrainSet* brainSetIn,
                                              BrainModelSurface* bmsIn,
                                              PaintFile* paintFileIn,
                                              const int paintFileColumnIn);
                                              
      // destructor
      ~BrainModelSurfacePaintToBorderConverter();

      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
            
   protected:
      // the surface
      BrainModelSurface* bms;
      
      // the paint file
      PaintFile* paintFile;
      
      // the paint file column
      int paintFileColumn;
      
      // the border file
      BorderFile* borderFile;
};

#endif // __BRAIN_MODEL_SURFACE_PAINT_TO_BORDER_CONVERTER_H__

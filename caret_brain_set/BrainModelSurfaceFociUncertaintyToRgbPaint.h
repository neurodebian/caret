

#ifndef __BRAIN_MODEL_SURFACE_FOCI_UNCERTAINTY_TO_RGB_PAINT_H__
#define __BRAIN_MODEL_SURFACE_FOCI_UNCERTAINTY_TO_RGB_PAINT_H__

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
class FociProjectionFile;
class FociColorFile;
class RgbPaintFile;

/// Class that converts foci uncertainty into an RGB Paint file.
class BrainModelSurfaceFociUncertaintyToRgbPaint : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelSurfaceFociUncertaintyToRgbPaint(
                                     BrainSet* bsIn,
                                     const BrainModelSurface* leftSurfaceIn,
                                     const BrainModelSurface* rightSurfaceIn,
                                     RgbPaintFile* rgbPaintFileIn,
                                     const int leftRgbPaintFileColumnIn,
                                     const QString& leftRgbPaintFileColumnNameIn,
                                     const int rightRgbPaintFileColumnIn,
                                     const QString& rightRgbPaintFileColumnNameIn,
                                     const FociProjectionFile* fociProjectionFileIn,
                                     const FociColorFile* fociColorFileIn,
                                     const float lowerLimitIn, 
                                     const float middleLimitIn, 
                                     const float upperLimitIn);
      
      /// Destructor
      ~BrainModelSurfaceFociUncertaintyToRgbPaint();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
              
   protected:
      /// Convert foci uncertainty to rgb paint (returns true if successful).
      void generateLimits(const BrainModelSurface* bms,
                   const int rgbPaintColumnIn, 
                   const QString& rgbPaintColumnNewName,
                   int& progressSteps) throw (BrainModelAlgorithmException);
      
      /// Convert foci uncertainty to rgb paint (returns true if successful).
      void generateLimitsOLD(const BrainModelSurface* bms,
                   const int rgbPaintColumnIn, 
                   const QString& rgbPaintColumnNewName) throw (BrainModelAlgorithmException);
      
      /// the left surface
      const BrainModelSurface* leftSurface;
      
      /// the right surface
      const BrainModelSurface* rightSurface;
      
      /// the rgb paint file
      RgbPaintFile* rgbPaintFile;
      
      /// column to set for left surface data
      int leftRgbPaintFileColumn;
      
      /// name of column for left surface data
      QString leftRgbPaintFileColumnName;
      
      /// column to set for right surface data
      int rightRgbPaintFileColumn;
      
      /// name of column for right surface data
      QString rightRgbPaintFileColumnName;
      
      /// the foci projection file
      const FociProjectionFile* fociProjectionFile;
      
      /// the foci color file
      const FociColorFile* fociColorFile;
      
      /// the lower limit
      float lowerLimit;
      
      /// the middle limit
      float middleLimit;
      
      /// the upper limit
      float upperLimit;

};

#endif // __BRAIN_MODEL_SURFACE_FOCI_UNCERTAINTY_TO_RGB_PAINT_H__


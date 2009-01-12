
#ifndef __BRAIN_MODEL_SURFACE_ROI_ASSIGN_SHAPE_H__
#define __BRAIN_MODEL_SURFACE_ROI_ASSIGN_SHAPE_H__

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

#include "BrainModelSurfaceROIOperation.h"

class SurfaceShapeFile;

/// class for assigning surface shape using an ROI
class BrainModelSurfaceROIAssignShape : public BrainModelSurfaceROIOperation {
   public:
      // constructor
      BrainModelSurfaceROIAssignShape(BrainSet* bs,
                                       BrainModelSurface* bmsIn,
                                       BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                       SurfaceShapeFile* shapeFileIn,
                                       const int shapeColumnNumberIn,
                                       const QString& shapeColumnNameIn,
                                       const float valueIn);
                                       
      // destructor
      ~BrainModelSurfaceROIAssignShape();
      
      // get assigned shape column
      int getAssignedShapeColumn() const { return shapeColumnNumber; };
      
   protected:
      // execute the operation
      virtual void executeOperation() throw (BrainModelAlgorithmException);
      
      /// the surface shape file
      SurfaceShapeFile* shapeFile;
      
      /// shape column number
      int shapeColumnNumber;
      
      /// name for shape column
      QString shapeColumnName;
      
      /// value to assign
      float value;
};

#endif // __BRAIN_MODEL_SURFACE_ROI_ASSIGN_SHAPE_H__

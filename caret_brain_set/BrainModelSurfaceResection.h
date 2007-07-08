
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

#ifndef __BRAIN_MODEL_SURFACE_RESECTION_H__
#define __BRAIN_MODEL_SURFACE_RESECTION_H__

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class SectionFile;
class vtkTransform;

/// Class for resectioning a surface
class BrainModelSurfaceResection : public BrainModelAlgorithm {
   public:
      /// sectioning axis
      enum SECTION_AXIS {
         SECTION_AXIS_X,
         SECTION_AXIS_Y,
         SECTION_AXIS_Z,
         SECTION_AXIS_Z_WITH_ROTATION_MATRIX
      };
      
      /// sectioning type
      enum SECTION_TYPE {
         SECTION_TYPE_THICKNESS,
         SECTION_TYPE_NUM_SECTIONS
      };
      
      /// constructor
      BrainModelSurfaceResection(BrainSet* brainSetIn,
                                 BrainModelSurface* bmsIn,
                                 vtkTransform* rotationMatrixIn,
                                 const SECTION_AXIS sectionAxisIn,
                                 const SECTION_TYPE sectionTypeIn,
                                 SectionFile* sectionFileIn,
                                 const int columnNumberIn,
                                 const QString& columnNameIn,
                                 const float thicknessIn,
                                 const int numSectionsIn);
      
      /// destructor
      ~BrainModelSurfaceResection();
      
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
      
   private:
      /// the surface for to use for sectioning
      BrainModelSurface* bms;
      
      /// the rotation matrix
      vtkTransform* rotationMatrix;
      
      /// the section axis
      SECTION_AXIS sectionAxis;
      
      /// the section type
      SECTION_TYPE sectionType;
      
      /// the section file
      SectionFile* sectionFile;
      
      /// the column in the section file
      int columnNumber;
      
      /// the name for the column
      QString columnName;
      
      /// the thickness for sectioning
      float thickness;
      
      /// the desired number of sections
      int numSections;
};

#endif // __BRAIN_MODEL_SURFACE_RESECTION_H__



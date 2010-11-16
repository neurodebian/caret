
#ifndef __BRAIN_MODEL_SURFACE_DEFORMATION_MAP_CREATE_H__
#define __BRAIN_MODEL_SURFACE_DEFORMATION_MAP_CREATE_H__

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
class DeformationMapFile;

/// class for create a deformation map from two surfaces
class BrainModelSurfaceDeformationMapCreate : public BrainModelAlgorithm {
   public:
      /// deformation surface type
      enum DEFORMATION_SURFACE_TYPE {
         /// spherical surface
         DEFORMATION_SURFACE_TYPE_SPHERE
      };
      
      // constructor
      BrainModelSurfaceDeformationMapCreate(BrainSet* bs,
                                            const BrainModelSurface* sourceSurfaceIn,
                                            const BrainModelSurface* targetSurfaceIn,
                                            DeformationMapFile* deformationMapFileIn,
                                            const DEFORMATION_SURFACE_TYPE deformationSurfaceTypeIn);
      
      // destructor
      ~BrainModelSurfaceDeformationMapCreate();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      // create the spherical deformation map
      void createSphericalDeformationMap();
      
      /// source surface
      BrainModelSurface* sourceSurface;
      
      /// target surface
      BrainModelSurface* targetSurface;
      
      /// source surface
      const BrainModelSurface* sourceSurfaceIn;
      
      /// target surface
      const BrainModelSurface* targetSurfaceIn;
      
      /// deformation map 
      DeformationMapFile* deformationMapFile;
      
      /// deformation surface type
      DEFORMATION_SURFACE_TYPE deformationSurfaceType;
};

#endif // __BRAIN_MODEL_SURFACE_DEFORMATION_MAP_CREATE_H__


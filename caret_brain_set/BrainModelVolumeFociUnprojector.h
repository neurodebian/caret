
#ifndef __BRAIN_MODEL_VOLUME_FOCI_UNPROJECTOR_H__
#define __BRAIN_MODEL_VOLUME_FOCI_UNPROJECTOR_H__

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

/// class for setting the volume coordinates for foci
class BrainModelVolumeFociUnprojector : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelVolumeFociUnprojector(BrainSet* bsIn,
                                      BrainModelSurface* leftSurfaceIn,
                                      BrainModelSurface* rightSurfaceIn,
                                      BrainModelSurface* cerebellumSurfaceIn,
                                      FociProjectionFile* fociProjectionFileIn);
                                      
      // destructor
      ~BrainModelVolumeFociUnprojector();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// the left surface
      BrainModelSurface* leftSurface;
      
      /// the right surface
      BrainModelSurface* rightSurface;
      
      /// the cerebellum surface
      BrainModelSurface* cerebellumSurface;
      
      /// the foci projection file
      FociProjectionFile* fociProjectionFile;
      
};

#endif // __BRAIN_MODEL_VOLUME_FOCI_UNPROJECTOR_H__

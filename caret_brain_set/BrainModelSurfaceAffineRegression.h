
#ifndef __BRAIN_MODEL_SURFACE_AFFINE_REGRESSION_H__
#define __BRAIN_MODEL_SURFACE_AFFINE_REGRESSION_H__

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

class CoordinateFile;

/// class for create a functional volume using a probabilistic volume
class BrainModelSurfaceAffineRegression : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelSurfaceAffineRegression(BrainSet* bs,
                                        CoordinateFile* sourceIn,
                                        CoordinateFile* targetIn,
                                        CoordinateFile* registeredOutIn,
                                        const QString& coordNameIn);
                                            
      /// Destructor
      ~BrainModelSurfaceAffineRegression();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      double getAffine(short i, short j);
      
   private:
      CoordinateFile* source, *target, *registered;
      QString coordName;
      double affine[3][4];
};

#endif // __BRAIN_MODEL_VOLUME_LIGASE_SEGMENTATION_H__


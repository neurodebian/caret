
#ifndef __BRAIN_MODEL_SURFACE_STANDARD_SPHERE_H__
#define __BRAIN_MODEL_SURFACE_STANDARD_SPHERE_H__

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

/// Read a standard sphere into a brain set
class BrainModelSurfaceStandardSphere : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelSurfaceStandardSphere(BrainSet* bs,
                                      const int numberOfNodesIn);
      
      /// Destructor
      ~BrainModelSurfaceStandardSphere();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// get available number of nodes and triangles
      static void getNumberOfNodesAndTriangles(
                                        std::vector<int>& numNodesOut,
                                        std::vector<int>& numTrianglesOut);

   protected:
      /// number of nodse in surface
      const int numberOfNodes;
};

#endif //  __BRAIN_MODEL_SURFACE_STANDARD_SPHERE_H__


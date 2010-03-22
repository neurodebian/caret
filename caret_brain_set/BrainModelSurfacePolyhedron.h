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

#ifndef  __BRAIN_MODEL_SURFACE_POLYAHEDRON_H__
#define	__BRAIN_MODEL_SURFACE_POLYAHEDRON_H__

#include "BrainModelAlgorithm.h"

class BrainModelSurface;

/// class that generates a tetrahedron surface
class BrainModelSurfacePolyhedron : public BrainModelAlgorithm {
    public:

       // constructor
       BrainModelSurfacePolyhedron(BrainSet* bs,
                                    const int numberOfIterations);

       // destructor
       ~BrainModelSurfacePolyhedron();

       // execute the algorithm
       void execute() throw (BrainModelAlgorithmException);

       // get number of nodes and triangles from the number of iterations
       static void getNumberOfNodesAndTrianglesFromIterations(
                        std::vector<int>& iterationsOut,
                        std::vector<int>& numNodesOut,
                        std::vector<int>& numTrianglesOut);
       
protected:
      enum POLYHEDRA {
          POLYHEDRA_TETRAHEDRON,
          POLYHEDRA_OCTAHEDRON,
          POLYHEDRA_ICOSAHEDRON
      };

      /// make the surface a sphere
      void makeSphere();

      /// the surface created
      BrainModelSurface* surface;

      /// the topology file created
      TopologyFile* topologyFile;

      /// number of iterations
      const int numberOfIterations;

      /// type of polyhedra produced
      static const POLYHEDRA polyhedra;
};

#ifdef __BRAIN_MODEL_SURFACE_POLYAHEDRON_MAIN__
const BrainModelSurfacePolyhedron::POLYHEDRA
   BrainModelSurfacePolyhedron::polyhedra =
      BrainModelSurfacePolyhedron::POLYHEDRA_ICOSAHEDRON;
#endif // __BRAIN_MODEL_SURFACE_POLYAHEDRON_MAIN__

#endif	// __BRAIN_MODEL_SURFACE_POLYAHEDRON_H__


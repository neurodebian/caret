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

#ifndef  __BRAIN_MODEL_SURFACE_POLYAHEDRON_NEW_H__
#define	__BRAIN_MODEL_SURFACE_POLYAHEDRON_NEW_H__

#include "BrainModelAlgorithm.h"
#include <vector>

class BrainModelSurface;
class TopologyFile;
class CoordinateFile;

/// class that generates a tetrahedron surface
class BrainModelSurfacePolyhedronNew : public BrainModelAlgorithm {
    public:

       // constructor
       BrainModelSurfacePolyhedronNew(BrainSet* bs,
                                    const int numberOfDivisions);

       // destructor
       ~BrainModelSurfacePolyhedronNew();

       // execute the algorithm
       void execute() throw (BrainModelAlgorithmException);

       // get number of nodes and triangles from the number of iterations
       static void getNumberOfNodesAndTrianglesFromDivisions(
                        int divisions,
                        int& numNodesOut,
                        int& numTrianglesOut);
       
protected:
      enum POLYHEDRA {
          POLYHEDRA_TETRAHEDRON,
          POLYHEDRA_OCTAHEDRON,
          POLYHEDRA_ICOSAHEDRON
      };

      /// make the surface a sphere
      void makeSphere();
      
      ///interpolate a face node
      void interp3(float coord1[3], float coord2[3], float coord3[3], int row, int col, float out[3]);
      
      ///get edge nodes, generating if needed
      void getEdge(int node1, int node2, int* out);
      
      ///edge node info
      std::vector<std::vector<std::vector<int> > > edgenodes;//yes, really

      /// the surface created
      BrainModelSurface* surface;

      /// the topology file created
      TopologyFile* topologyFile;
      
      CoordinateFile* coordinateFile;

      /// number of iterations
      int numberOfDivisions;
      int curNodes, curTiles;
      
      /// type of polyhedra produced
      static const POLYHEDRA polyhedra;
};

/*//this goes in .cxx, so we don't need to use compiler directives
#ifdef __BRAIN_MODEL_SURFACE_POLYAHEDRON_MAIN__
const BrainModelSurfacePolyhedronNew::POLYHEDRA
   BrainModelSurfacePolyhedronNew::polyhedra =
      BrainModelSurfacePolyhedronNew::POLYHEDRA_ICOSAHEDRON;
#endif // __BRAIN_MODEL_SURFACE_POLYAHEDRON_MAIN__
//*/
#endif	// __BRAIN_MODEL_SURFACE_POLYAHEDRON_H__


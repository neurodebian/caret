
#ifndef __BRAIN_MODEL_SURFACE_SPHERICAL_TESSELLATOR_H__
#define __BRAIN_MODEL_SURFACE_SPHERICAL_TESSELLATOR_H__

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

#include <set>
#include <vector>

#include "BrainModelAlgorithm.h"
#include "Tessellation.h"

class BrainModelSurface;
class CoordinateFile;
class PointLocator;
class TopologyFile;

/// Class that retessellates a sphere
class BrainModelSurfaceSphericalTessellator : public BrainModelAlgorithm {
   public:
      /// Constructor 
      BrainModelSurfaceSphericalTessellator(BrainSet* bsIn,
                                            BrainModelSurface* sphereSurfaceIn,
                                            const std::vector<bool>& includeNodeInTessellationFlagsIn);
      /// Destructor
      ~BrainModelSurfaceSphericalTessellator();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// get a pointer to the new spherical surface
      BrainModelSurface* getPointerToNewSphericalSurface() { return newSphericalSurface; }
      
   protected:
      /// clear the tessellation process
      void clear();
      
      /// execute the tessellation algorithm
      void executeTessellation() throw (BrainModelAlgorithmException);
      
      /// create the initial sphere
      void createInitialSphere() throw (BrainModelAlgorithmException);
      
      /// create a sphere from the tessellation
      void createSphereFromTessellation() throw (BrainModelAlgorithmException);
      
      /// Find the tessellation triangle containing the new point
      TessTriangle* findContainingTriangle(const double xyz[3],
                                           TessVertex* tv);
      
      /// Find the nearest node in the tessellation.
      TessVertex* getNearestNodeInTessellation(const double* xyz,
                                               double& distanceSquared);

      /// get a triangles circumcenter and radius
      void getCircumcenterAndRadius(TessTriangle* tt,
                                    double center[3],
                                    double& radius);
                               
      /// insert nodes into the sphere
      void insertNodesIntoSphere(const int callNum) throw (TessellationException);
      
      /// process all of the suspect triangles who made need their edges flipped
      void processSuspectTriangles(Tessellation::TriangleSet& suspectTriangles) throw (TessellationException);
      
      /// orient the triangle so that the normal points out
      void orientTriangleVertices(TessTriangle* tt);
      
      /// Create the point locator
      void createPointLocator();

      /// the new spherical surface that is created
      BrainModelSurface* newSphericalSurface;
      
      /// the original spherical surface
      const BrainModelSurface* originalSphereSurface;
      
      /// number of nodes
      int numNodes;
      
      /// the suspect triangles who may require an edge flip
      //Tessellation::TriangleSet suspectTriangles;
      
      /// the tessellation
      Tessellation* tess;
      
      /// the point locator
      PointLocator* pointLocator;
      
      /// the coordinates
      double* pointXYZ;
      
      /// flags that indicate if node should be included in the tessellation
      std::vector<bool> includeNodeInTessellationFlags;
      
      /// original flags that indicate if node should be included in the tessellation
      const std::vector<bool> originalIncludeNodeInTessellationFlags;
};

#endif // __BRAIN_MODEL_SURFACE_SPHERICAL_TESSELLATOR_H__


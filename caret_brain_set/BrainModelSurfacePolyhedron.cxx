
/*LICENSE_START*/

#include <set>

#include "BrainSet.h"

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

#include <cmath>

#include "BrainModelSurface.h"
#define __BRAIN_MODEL_SURFACE_POLYAHEDRON_MAIN__
#include "BrainModelSurfacePolyhedron.h"
#undef __BRAIN_MODEL_SURFACE_POLYAHEDRON_MAIN__
#include "CoordinateFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "BrainModelSurfaceDeformDataFile.h"

/**
 * constructor.
 */
BrainModelSurfacePolyhedron::BrainModelSurfacePolyhedron(BrainSet* bs,
                                              const int numberOfIterationsIn)
   : BrainModelAlgorithm(bs),
        numberOfIterations(numberOfIterationsIn)
{
   surface = NULL;
}

/**
 * destructor.
 */
BrainModelSurfacePolyhedron::~BrainModelSurfacePolyhedron()
{

}

/**
 * get number of nodes and triangles from the number of iterations.
 */
void
BrainModelSurfacePolyhedron::getNumberOfNodesAndTrianglesFromIterations(
                                        std::vector<int>& iterationsOut,
                                        std::vector<int>& numNodesOut,
                                        std::vector<int>& numTrianglesOut)
{
   iterationsOut.clear();
   numNodesOut.clear();
   numTrianglesOut.clear();

   //
   // Initial number of triangles for initial tetrahedron
   //
   int numTriangles = 0;
   switch (polyhedra) {
      case POLYHEDRA_TETRAHEDRON:
         numTriangles = 4;
         break;
      case POLYHEDRA_OCTAHEDRON:
         numTriangles = 8;
         break;
      case POLYHEDRA_ICOSAHEDRON:
         numTriangles = 20;
         break;
   }

   for (int iter = 0; iter < 8; iter++) {
      //
      // Calculate number of nodes using the Euler Characteristic
      //
      // V - E + F = 2
      // V = 2 + E - F
      // 3 edges per triangle with each edge shared
      // V = 2 + ((F * 3) / 2) - F
      // V = 2 + 1.5 * F - F
      // V = 2 + (1.5 - 1) * F
      // V = 2 + 0.5 * F
      // V = 2 + (F / 2)
      //
      const int numNodes = 2 + (numTriangles / 2);

      iterationsOut.push_back(iter);
      numNodesOut.push_back(numNodes);
      numTrianglesOut.push_back(numTriangles);

      //
      // Each triangle subdivides into four triangles
      //
      numTriangles *= 4;
   }
}

/**
 * execute the algorithm.
 */
void
BrainModelSurfacePolyhedron::execute() throw (BrainModelAlgorithmException)
{
   //
   // Create a surface
   //
   this->surface = new BrainModelSurface(brainSet);
   this->surface->setSurfaceType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   this->brainSet->addBrainModel(surface);

   //
   // Set the initial coordinates
   //
   CoordinateFile* coordinateFile = surface->getCoordinateFile();

   //
   // Create and set the initial topology
   //
   this->topologyFile = new TopologyFile();
   this->topologyFile->setTopologyType(TopologyFile::TOPOLOGY_TYPE_CLOSED);
   this->brainSet->addTopologyFile(topologyFile);
   this->surface->setTopologyFile(this->topologyFile);

   //
   // Create the initial polyhedra
   //
   switch (this->polyhedra) {
      case POLYHEDRA_TETRAHEDRON:
         coordinateFile->setNumberOfCoordinates(4);
         coordinateFile->setCoordinate(0,  1.0,  1.0,  1.0);
         coordinateFile->setCoordinate(1, -1.0, -1.0,  1.0);
         coordinateFile->setCoordinate(2, -1.0,  1.0, -1.0);
         coordinateFile->setCoordinate(3,  1.0, -1.0, -1.0);
         this->topologyFile->setNumberOfTiles(4);
         this->topologyFile->setTile(0, 1, 0, 2);
         this->topologyFile->setTile(1, 1, 3, 0);
         this->topologyFile->setTile(2, 2, 3, 0);
         this->topologyFile->setTile(3, 3, 2, 1);
         break;
      case POLYHEDRA_OCTAHEDRON:
         coordinateFile->setNumberOfCoordinates(6);
         coordinateFile->setCoordinate(0, -1.0, -1.0,  0.0);
         coordinateFile->setCoordinate(1,  1.0, -1.0,  0.0);
         coordinateFile->setCoordinate(2,  1.0,  1.0,  0.0);
         coordinateFile->setCoordinate(3, -1.0,  1.0,  0.0);
         coordinateFile->setCoordinate(4,  0.0,  0.0, -1.0);
         coordinateFile->setCoordinate(5,  0.0,  0.0,  1.0);
         this->topologyFile->setNumberOfTiles(8);
         this->topologyFile->setTile(0, 0, 1, 5);
         this->topologyFile->setTile(1, 1, 2, 5);
         this->topologyFile->setTile(2, 2, 3, 5);
         this->topologyFile->setTile(3, 3, 0, 5);
         this->topologyFile->setTile(4, 0, 1, 4);
         this->topologyFile->setTile(5, 1, 2, 4);
         this->topologyFile->setTile(6, 2, 3, 4);
         this->topologyFile->setTile(7, 3, 0, 4);
         break;
      case POLYHEDRA_ICOSAHEDRON:
         {
            const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
            coordinateFile->setNumberOfCoordinates(12);

            coordinateFile->setCoordinate(0, -phi,  0.0,  1.0);
            coordinateFile->setCoordinate(1,    0, -1.0,  phi);
            coordinateFile->setCoordinate(2,  phi,  0.0,  1.0);
            coordinateFile->setCoordinate(3,    0,  1.0,  phi);

            coordinateFile->setCoordinate(4, -1.0, -phi, 0.0);
            coordinateFile->setCoordinate(5,  1.0, -phi, 0.0);
            coordinateFile->setCoordinate(6,  1.0,  phi, 0.0);
            coordinateFile->setCoordinate(7, -1.0,  phi, 0.0);

            coordinateFile->setCoordinate(8, -phi,  0.0, -1.0);
            coordinateFile->setCoordinate(9,    0, -1.0, -phi);
            coordinateFile->setCoordinate(10, phi,  0.0, -1.0);
            coordinateFile->setCoordinate(11,   0,  1.0, -phi);

            /*
            this->topologyFile->setNumberOfTiles(coordinateFile->getNumberOfCoordinates() + 20);
            for (int i = 0; i < coordinateFile->getNumberOfCoordinates(); i++) {
               this->topologyFile->setTile(i, i, i, i);
            }
             */
            this->topologyFile->setNumberOfTiles(20);
            this->topologyFile->setTile(0, 0, 7, 8);
            this->topologyFile->setTile(1, 0, 3, 7);
            this->topologyFile->setTile(2, 0, 1, 3);
            this->topologyFile->setTile(3, 1, 2, 3);
            this->topologyFile->setTile(4, 1, 5, 2);
            this->topologyFile->setTile(5, 5, 9, 10);
            this->topologyFile->setTile(6, 10, 11, 6);
            this->topologyFile->setTile(7, 4, 1, 0);
            this->topologyFile->setTile(8, 4, 5, 1);
            this->topologyFile->setTile(9, 4, 9, 5);
            this->topologyFile->setTile(10, 5, 10, 2);
            this->topologyFile->setTile(11, 10, 6, 2);
            this->topologyFile->setTile(12, 6, 11, 7);
            this->topologyFile->setTile(13, 11, 9, 8);
            this->topologyFile->setTile(14, 9, 4, 8);
            this->topologyFile->setTile(15, 8, 4, 0);
            this->topologyFile->setTile(16, 11, 8, 7);
            this->topologyFile->setTile(17, 6, 7, 3);
            this->topologyFile->setTile(18, 6, 3, 2);
            this->topologyFile->setTile(19, 11, 10, 9);
         }
         break;
   }

   //
   // Scale the surface
   //
   TransformationMatrix tm;
   tm.scale(100.0, 100.0, 100.0);
   this->surface->applyTransformationMatrix(tm);
   
   this->makeSphere();
   
   //
   // Subdivide for the specified number of iterations
   //
   for (int iterations = 0; iterations < this->numberOfIterations; iterations++) {
      //
      // Create a topology helper
      //
      const TopologyHelper* th = this->topologyFile->getTopologyHelper(true, true, true);

      //
      // Make a copy of the coordinate file
      //
      CoordinateFile newCoords;
      int numOldCoords = coordinateFile->getNumberOfCoordinates();
      newCoords.setNumberOfCoordinates(numOldCoords);
      for (int i = 0; i < numOldCoords; i++) {
         newCoords.setCoordinate(i, coordinateFile->getCoordinate(i));
      }

      //
      // Get the edges and place them in a vector
      //
      const std::set<TopologyEdgeInfo> edgesSet = th->getEdgeInfo();
      std::vector<TopologyEdgeInfo> edges;
      for (std::set<TopologyEdgeInfo>::const_iterator iter = edgesSet.begin();
           iter != edgesSet.end();
           iter++) {
         edges.push_back(*iter);
      }

      //
      // Index of new node for edge
      //
      std::vector<int> edgeMidpointNodeIndex;

      //
      // Add a vertex at the midpoint of each edge
      //
      int numEdges = static_cast<int>(edges.size());
      for (int i = 0; i < numEdges; i++) {
         //
         // Get the edge's nodes
         //
         int node1, node2;
         edges[i].getNodes(node1, node2);

         //
         // Get the midpoint of the nodes
         //
         const float* xyz1 = coordinateFile->getCoordinate(node1);
         const float* xyz2 = coordinateFile->getCoordinate(node2);
         const float midpoint[3] = {
            (xyz1[0] + xyz2[0]) * 0.5,
            (xyz1[1] + xyz2[1]) * 0.5,
            (xyz1[2] + xyz2[2]) * 0.5
         };

         //
         // Add midpoint as new coordinate
         //
         newCoords.addCoordinate(midpoint);
         edgeMidpointNodeIndex.push_back(newCoords.getNumberOfCoordinates() - 1);
      }

      //
      // Copy new coordinates to the surface's coordinates
      //
      const int numCoords = newCoords.getNumberOfCoordinates();
      coordinateFile->setNumberOfCoordinates(numCoords);
      for (int i = 0; i < numCoords; i++) {
         coordinateFile->setCoordinate(i, newCoords.getCoordinate(i));
      }

      //
      // Create a new topology file
      //
      TopologyFile newTopology;

      //
      // Loop through the old topology file
      //
      int numOldTriangles = this->topologyFile->getNumberOfTiles();
      for (int i = 0; i < numOldTriangles; i++) {
         const int* triangle = this->topologyFile->getTile(i);

         //
         // Find the edges used by this triangle
         //
         int newNodes[3] = { -1, -1, -1 };
         for (int k = 0; k < numEdges; k++) {
            TopologyEdgeInfo& tei = edges[k];
            if ((tei.tile1 == i) ||
                (tei.tile2 == i)) {
               if ((tei.node1 == triangle[0]) &&
                   (tei.node2 == triangle[1])) {
                  newNodes[0] = edgeMidpointNodeIndex[k];
               }
               else if ((tei.node1 == triangle[1]) &&
                        (tei.node2 == triangle[0])) {
                  newNodes[0] = edgeMidpointNodeIndex[k];
               }
               else if ((tei.node1 == triangle[1]) &&
                        (tei.node2 == triangle[2])) {
                  newNodes[1] = edgeMidpointNodeIndex[k];
               }
               else if ((tei.node1 == triangle[2]) &&
                        (tei.node2 == triangle[1])) {
                  newNodes[1] = edgeMidpointNodeIndex[k];
               }
               else if ((tei.node1 == triangle[0]) &&
                        (tei.node2 == triangle[2])) {
                  newNodes[2] = edgeMidpointNodeIndex[k];
               }
               else if ((tei.node1 == triangle[2]) &&
                        (tei.node2 == triangle[0])) {
                  newNodes[2] = edgeMidpointNodeIndex[k];
               }
            }
         }

         //
         // Check for errors
         //
         for (int j = 0; j < 3; j++) {
            if (newNodes[j] < 0) {
               throw BrainModelAlgorithmException("ALGORITHM ERROR: "
                       "failed to find edges for a triangle.");
            }
         }

         //
         // Create new tiles
         //
         newTopology.addTile(triangle[0], newNodes[0], newNodes[2]);
         newTopology.addTile(triangle[1], newNodes[1], newNodes[0]);
         newTopology.addTile(triangle[2], newNodes[2], newNodes[1]);
         newTopology.addTile(newNodes[0], newNodes[1], newNodes[2]);
      }

      //
      // Copy new topology to surface's topology
      //
      const int numNewTriangles = newTopology.getNumberOfTiles();
      this->topologyFile->setNumberOfTiles(numNewTriangles);
      for (int j = 0; j < numNewTriangles; j++) {
         this->topologyFile->setTile(j, newTopology.getTile(j));
      }

      this->makeSphere();
   } // for (iterations...

   this->makeSphere();

   this->surface->convertToSphereWithRadius(100.0);
}

/**
 * Make the surface a sphere.
 */
void
BrainModelSurfacePolyhedron::makeSphere()
{
   this->surface->orientTilesOutward(surface->getSurfaceType());
   this->surface->computeNormals();
   this->surface->translateToCenterOfMass();
   this->surface->convertToSphereWithRadius(10000.0);
}

/*LICENSE_START*/

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
//#define __BRAIN_MODEL_SURFACE_POLYAHEDRON_MAIN__
#include "BrainModelSurfacePolyhedronNew.h"
//#undef __BRAIN_MODEL_SURFACE_POLYAHEDRON_MAIN__
#include "CoordinateFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

//define polyhedron type
const BrainModelSurfacePolyhedronNew::POLYHEDRA
   BrainModelSurfacePolyhedronNew::polyhedra =
      BrainModelSurfacePolyhedronNew::POLYHEDRA_ICOSAHEDRON;


/**
 * constructor.
 */
BrainModelSurfacePolyhedronNew::BrainModelSurfacePolyhedronNew(BrainSet* bs,
                                              const int numberOfDivisionsIn)
   : BrainModelAlgorithm(bs),
        numberOfDivisions(numberOfDivisionsIn)
{
   if (numberOfDivisions < 1) numberOfDivisions = 1;
   surface = NULL;
}

/**
 * destructor.
 */
BrainModelSurfacePolyhedronNew::~BrainModelSurfacePolyhedronNew()
{

}

/**
 * get number of nodes and triangles from the number of iterations.
 */
void
BrainModelSurfacePolyhedronNew::getNumberOfNodesAndTrianglesFromDivisions(
                                        int divisions,
                                        int& numNodesOut,
                                        int& numTrianglesOut)
{

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
   numTrianglesOut = numTriangles * divisions * divisions;//yes, its really that simple, the sum of the two triangulars is square
   numNodesOut = 2 + (numTrianglesOut / 2);
}

/**
 * execute the algorithm.
 */
void
BrainModelSurfacePolyhedronNew::execute() throw (BrainModelAlgorithmException)
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
   coordinateFile = surface->getCoordinateFile();

   //
   // Create and set the initial topology
   //
   TopologyFile* initTopo = new TopologyFile();
   topologyFile = new TopologyFile();
   initTopo->setTopologyType(TopologyFile::TOPOLOGY_TYPE_CLOSED);
   topologyFile->setTopologyType(TopologyFile::TOPOLOGY_TYPE_CLOSED);
   this->brainSet->addTopologyFile(topologyFile);
   this->surface->setTopologyFile(this->topologyFile);

   //
   // Get needed sizes of files
   //
   int totalTiles, totalNodes;
   getNumberOfNodesAndTrianglesFromDivisions(numberOfDivisions, totalNodes, totalTiles);
   coordinateFile->setNumberOfCoordinates(totalNodes);
   topologyFile->setNumberOfTiles(totalTiles);
   curTiles = 0;
   
   //
   // Create the initial polyhedra
   //
   switch (this->polyhedra) {
      case POLYHEDRA_TETRAHEDRON:
         curNodes = 4;
         coordinateFile->setCoordinate(0,  1.0,  1.0,  1.0);
         coordinateFile->setCoordinate(1, -1.0, -1.0,  1.0);
         coordinateFile->setCoordinate(2, -1.0,  1.0, -1.0);
         coordinateFile->setCoordinate(3,  1.0, -1.0, -1.0);
         initTopo->setNumberOfTiles(4);
         initTopo->setTile(0, 1, 0, 2);
         initTopo->setTile(1, 1, 3, 0);
         initTopo->setTile(2, 2, 3, 0);
         initTopo->setTile(3, 3, 2, 1);
         break;
      case POLYHEDRA_OCTAHEDRON:
         curNodes = 6;
         coordinateFile->setCoordinate(0, -1.0, -1.0,  0.0);
         coordinateFile->setCoordinate(1,  1.0, -1.0,  0.0);
         coordinateFile->setCoordinate(2,  1.0,  1.0,  0.0);
         coordinateFile->setCoordinate(3, -1.0,  1.0,  0.0);
         coordinateFile->setCoordinate(4,  0.0,  0.0, -1.0);
         coordinateFile->setCoordinate(5,  0.0,  0.0,  1.0);
         initTopo->setNumberOfTiles(8);
         initTopo->setTile(0, 0, 1, 5);
         initTopo->setTile(1, 1, 2, 5);
         initTopo->setTile(2, 2, 3, 5);
         initTopo->setTile(3, 3, 0, 5);
         initTopo->setTile(4, 0, 1, 4);
         initTopo->setTile(5, 1, 2, 4);
         initTopo->setTile(6, 2, 3, 4);
         initTopo->setTile(7, 3, 0, 4);
         break;
      case POLYHEDRA_ICOSAHEDRON:
         {
            const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
            curNodes = 12;

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

            initTopo->setNumberOfTiles(20);
            initTopo->setTile(0, 0, 7, 8);
            initTopo->setTile(1, 0, 3, 7);
            initTopo->setTile(2, 0, 1, 3);
            initTopo->setTile(3, 1, 2, 3);
            initTopo->setTile(4, 1, 5, 2);
            initTopo->setTile(5, 5, 9, 10);
            initTopo->setTile(6, 10, 11, 6);
            initTopo->setTile(7, 4, 1, 0);
            initTopo->setTile(8, 4, 5, 1);
            initTopo->setTile(9, 4, 9, 5);
            initTopo->setTile(10, 5, 10, 2);
            initTopo->setTile(11, 10, 6, 2);
            initTopo->setTile(12, 6, 11, 7);
            initTopo->setTile(13, 11, 9, 8);
            initTopo->setTile(14, 9, 4, 8);
            initTopo->setTile(15, 8, 4, 0);
            initTopo->setTile(16, 11, 8, 7);
            initTopo->setTile(17, 6, 7, 3);
            initTopo->setTile(18, 6, 3, 2);
            initTopo->setTile(19, 11, 10, 9);
         }
         break;
   }
   // Note: if further refinement is required (unlikely), use a dodecahedron, divide each face into 5 triangles, keep track of which vertex is in the
   // center, and always pass that vertex as the first vertex to interp3.  Then, make two separate polynomials to interpolate weights, one for the first
   // vertex and one for the other two.  Then, optimize with a genetic algorithm for both variance of triangle area, and some regularization term to keep
   // the shape of the triangles consistent.  This means the triangles, once projected to the sphere, will each be identical, and have smaller area,
   // causing the polynomial that modifies the linear weights closer to linear, therefore with less edge effects. - Tim C

   //
   // Begin triangle division code - does not rely on spherical, preserves tile orientation
   //
   edgenodes.resize(curNodes - 1);//stores by low node #
   int edgesize = numberOfDivisions + 1;
   int i, j, k, numTiles = initTopo->getNumberOfTiles(), intRows = numberOfDivisions - 1, intCols;
   float tempcoord[3], coord1[3], coord2[3], coord3[3];
   int* edge1 = new int[edgesize], *edge2 = new int[edgesize], *edge3 = new int[edgesize];
   int** facenodes = new int*[edgesize];
   for (i = 0; i < edgesize; ++i)
   {// first index is row from bottom, second index is column from left
      facenodes[i] = new int[edgesize - i];
   }
   
   //
   // Loop through tiles
   //
   for (i = 0; i < numTiles; i++) {
      //
      // Get the tile's nodes
      //
      int node1, node2, node3;
      initTopo->getTile(i, node1, node2, node3);
      
      //convention for visualization: node 1 is bottom left, node 2 is bottom right, node 3 is top
      //tile generation also follows this convention, with the consequence that tile orientation is preserved

      //
      // Get or generate edge nodes
      //
      getEdge(node1, node2, edge1);//bottom edge, left to right
      getEdge(node1, node3, edge2);//left edge, bottom to top
      getEdge(node2, node3, edge3);//right edge, bottom to top
      
      //
      // Get coords for interpolating
      //
      coordinateFile->getCoordinate(node1, coord1);//only 20 times, doesn't matter if we use a local coord aray
      coordinateFile->getCoordinate(node2, coord2);
      coordinateFile->getCoordinate(node3, coord3);
      
      //
      // Copy edge nodes into the face array
      //
      for (j = 0; j < edgesize; ++j)
      {
         facenodes[0][j] = edge1[j];
         facenodes[j][0] = edge2[j];
         facenodes[j][numberOfDivisions - j] = edge3[j];
      }
      
      //
      // Time intensive code start (assuming large numberOfDivisions)
      //
      
      //
      // Generate interior coordinates
      //
      for (j = 1; j < intRows; ++j)
      {
         intCols = numberOfDivisions - j;
         for (k = 1; k < intCols; ++k)
         {
            interp3(coord1, coord2, coord3, j, k, tempcoord);
            coordinateFile->setCoordinate(curNodes, tempcoord);
            facenodes[j][k] = curNodes;
            ++curNodes;
         }
      }
      
      //
      // Generate tiles
      //
      for (j = 0; j < numberOfDivisions; ++j)
      {//lets inner loop short circuit on top row
         for (k = 0; k < numberOfDivisions - j - 1; ++k)
         {//pairs for trapezoidal pieces
            topologyFile->setTile(curTiles, facenodes[j][k], facenodes[j][k + 1], facenodes[j + 1][k]);
            topologyFile->setTile(curTiles + 1, facenodes[j + 1][k], facenodes[j][k + 1], facenodes[j + 1][k + 1]);
            curTiles += 2;
         }//and one more
         topologyFile->setTile(curTiles, facenodes[j][numberOfDivisions - j - 1], facenodes[j][numberOfDivisions - j], facenodes[j + 1][numberOfDivisions - j - 1]);
         ++curTiles;
      }
      
      //
      // Time intensive code end
      //
   }
   //
   // End triangle division code
   //

   this->makeSphere();

   this->surface->convertToSphereWithRadius(100.0);
   delete initTopo;
   for (i = 0; i < edgesize; ++i) delete[] facenodes[i];
   delete[] facenodes;
   delete[] edge1;
   delete[] edge2;
   delete[] edge3;
}

/**
 * Make the surface a sphere.
 */
void
BrainModelSurfacePolyhedronNew::makeSphere()
{
   this->surface->orientTilesOutward(surface->getSurfaceType());
   this->surface->computeNormals();
   this->surface->translateToCenterOfMass();
   this->surface->convertToSphereWithRadius(10000.0);
}

void BrainModelSurfacePolyhedronNew::getEdge(int node1, int node2, int* out)
{//function is called relatively few times, assuming large numberOfDivisions
   bool reverse = false;
   int i, index, edgesize = numberOfDivisions + 1;
   if (node1 > node2)
   {
      reverse = true;
      i = node1;
      node1 = node2;
      node2 = i;
   }
   bool found = false;
   for (i = 0; i < (int)edgenodes[node1].size(); ++i)
   {
      if (edgenodes[node1][i][numberOfDivisions] == node2)
      {
         found = true;
         index = i;
         break;
      }
   }
   if (!found)
   {
      float tempcoord[3], coord1[3], coord2[3], coord3[3] = {0.0f, 0.0f, 0.0f};
      coordinateFile->getCoordinate(node1, coord1);//this function isn't called a lot, so who cares if it uses a local coord array
      coordinateFile->getCoordinate(node2, coord2);
      std::vector<int> tempvec;
      tempvec.resize(edgesize);
      tempvec[0] = node1;
      tempvec[numberOfDivisions] = node2;
      for (i = 1; i < numberOfDivisions; ++i)
      {
         interp3(coord1, coord2, coord3, 0, i, tempcoord);//use 0 as dummy node, with row 0 it is unused
         tempvec[i] = curNodes;
         coordinateFile->setCoordinate(curNodes, tempcoord);
         ++curNodes;
      }
      index = edgenodes[node1].size();
      edgenodes[node1].push_back(tempvec);
   }
   if (reverse)
   {
      for (i = 0; i < edgesize; ++i)
      {
         out[i] = edgenodes[node1][index][edgesize - i - 1];
      }
   } else {
      for (i = 0; i < edgesize; ++i)
      {
         out[i] = edgenodes[node1][index][i];
      }
   }
}

void BrainModelSurfacePolyhedronNew::interp3(float coord1[3], float coord2[3], float coord3[3], int row, int col, float out[3])
{//this is the function to change if you want different spacing
   float weight2 = ((float)col) / numberOfDivisions;//start with flat interpolation weights
   float weight3 = ((float)row) / numberOfDivisions;
   float weight1 = 1.0f - weight2 - weight3;
   //polynomial for each weight - should map 0 to 0 and 1 to 1
   const float quintweight = 0.0537206f;//WEIGHTS TUNED FOR ICOSAHEDRON VIA GENETIC ALGORITHM, ADJUST FOR OTHER INITIAL POLYGONS
   const float quartweight = -0.174466f;//this polynomial should be highly dependent on size of the triangle being interpolated
   const float cubeweight = 0.292547f;
   const float quadweight = -0.456351f;
   const float linweight = 1.0f - quintweight - quartweight - cubeweight - quadweight;//make sure it maps 0 to 0 and 1 to 1
   weight1 = ((((quintweight * weight1 + quartweight) * weight1 + cubeweight) * weight1 + quadweight) * weight1 + linweight) * weight1;//quintic approximation of great arc equal area weight transformation function
   weight2 = ((((quintweight * weight2 + quartweight) * weight2 + cubeweight) * weight2 + quadweight) * weight2 + linweight) * weight2;
   weight3 = ((((quintweight * weight3 + quartweight) * weight3 + cubeweight) * weight3 + quadweight) * weight3 + linweight) * weight3;//three weights no longer sum to 1, but thats ok
   out[0] = coord1[0] * weight1 + coord2[0] * weight2 + coord3[0] * weight3;
   out[1] = coord1[1] * weight1 + coord2[1] * weight2 + coord3[1] * weight3;
   out[2] = coord1[2] * weight1 + coord2[2] * weight2 + coord3[2] * weight3;
}

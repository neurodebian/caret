
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

#include <algorithm>
#include <iostream>
#include <limits>

#include <QDateTime>

#include "vtkTriangle.h"
#include "vtkPointLocator.h"
#include "vtkPoints.h"

#include "BrainModelSurface.h"
#include "BrainModelSurfaceSphericalTessellator.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "MathUtilities.h"
#include "PointLocator.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelSurfaceSphericalTessellator::BrainModelSurfaceSphericalTessellator(
                                                     BrainSet* bsIn,
                                                     BrainModelSurface* sphereSurfaceIn,
                                                     const std::vector<bool>& includeNodeInTessellationFlagsIn)
   : BrainModelAlgorithm(bsIn),
     originalSphereSurface(sphereSurfaceIn),
     originalIncludeNodeInTessellationFlags(includeNodeInTessellationFlagsIn)
{
   pointLocator = NULL;
   pointXYZ = NULL;
   newSphericalSurface = NULL;
   tess = NULL;
}

/**
 * Destructor.
 */
BrainModelSurfaceSphericalTessellator::~BrainModelSurfaceSphericalTessellator()
{
   clear();
}

/**
 * reset the tessellation process.
 */
void 
BrainModelSurfaceSphericalTessellator::clear()
{
   if (pointLocator != NULL) {
      delete pointLocator;
      pointLocator = NULL;
   }
   if (pointXYZ != NULL) {
      delete[] pointXYZ;
   }
   if (tess != NULL) {
      delete tess;
      tess = NULL;
   }
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceSphericalTessellator::execute() throw (BrainModelAlgorithmException)
{
   QTime timer;
   timer.start();

   //
   // Sometimes the tessellation algorithm fails.  Since it randomizes the node
   // insertion order try it several times before giving up
   //
   QString errorMessage;
   const int maxTries = 10;
   for (int i = 0; i < maxTries; i++) {
      clear();
      
      newSphericalSurface = NULL;
      numNodes = 0;
      includeNodeInTessellationFlags = originalIncludeNodeInTessellationFlags;
      
      try {
         executeTessellation();
         return;
      }
      catch (BrainModelAlgorithmException& e) {
         errorMessage += (e.whatQString() + "\n");
         std::cout << "INFO: spherical tessellation failed but trying it again." << std::endl;
      }
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Time to tessellate: " << timer.elapsed() * 0.001 << std::endl; 
   }
   
   throw BrainModelAlgorithmException(errorMessage);
}      

/**
 * create the initial sphere.
 */
void 
BrainModelSurfaceSphericalTessellator::createInitialSphere() throw (BrainModelAlgorithmException)
{
   //
   // nodes with minimum and maximum xyz
   //
   double minmax[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
   int nodes[6] = { -1, -1, -1, -1, -1, -1 };
   
   //
   // Find the extreme nodes
   //
   for (int i = 0; i < numNodes; i++) {
      if (includeNodeInTessellationFlags[i]) {
         const double x = pointXYZ[i*3];
         if (x < minmax[0]) {
            minmax[0] = x;
            nodes[0]  = i;
         }
         if (x > minmax[1]) {
            minmax[1] = x;
            nodes[1]  = i;
         }
         
         const double y = pointXYZ[i*3+1];
         if (y < minmax[2]) {
            minmax[2] = y;
            nodes[2]  = i;
         }
         if (y > minmax[3]) {
            minmax[3] = y;
            nodes[3]  = i;
         }
         
         const double z = pointXYZ[i*3+2];
         if (z < minmax[4]) {
            minmax[4] = z;
            nodes[4]  = i;
         }
         if (z > minmax[5]) {
            minmax[5] = z;
            nodes[5]  = i;
         }
      }
   }
   
   //
   // Make sure extreme coords are unique
   //
   std::set<int> uniqueTest;
   for (int i = 0; i < 6; i++) {
      if (nodes[i] >= 0) {
         uniqueTest.insert(nodes[i]);
      }
      else {
         throw BrainModelAlgorithmException("Could not find extreme nodes");
      }
   }
   if (uniqueTest.size() != 6) {
      throw BrainModelAlgorithmException("Could not find unique extreme nodes");
   }
   
   //
   // Mark extreme nodes no longer available
   //
   for (int i = 0; i < 6; i++) {
      includeNodeInTessellationFlags[nodes[i]] = false;
   }
   
   //
   // Create tessellation vertices
   //
   TessVertex* v1 = new TessVertex(nodes[0]);
   TessVertex* v2 = new TessVertex(nodes[1]);
   TessVertex* v3 = new TessVertex(nodes[2]);
   TessVertex* v4 = new TessVertex(nodes[3]);
   TessVertex* v5 = new TessVertex(nodes[4]);
   TessVertex* v6 = new TessVertex(nodes[5]);
   
   //
   // Add vertices to the tessellation
   //
   tess->addVertex(v1);
   tess->addVertex(v2);
   tess->addVertex(v3);
   tess->addVertex(v4);
   tess->addVertex(v5);
   tess->addVertex(v6);
   
   //
   // Add to the point locator
   //
   if (pointLocator != NULL) {
      const CoordinateFile* coordFile = originalSphereSurface->getCoordinateFile();
      for (int m = 0; m < 6; m++) {
         pointLocator->addPoint(coordFile->getCoordinate(nodes[m]), -1);
      }
   }
   
   //
   // Add triangles to the tessellation
   //
   Tessellation::EdgeVector noEdges;
   tess->addTriangle(v1, v4, v6, noEdges);
   tess->addTriangle(v4, v2, v6, noEdges);
   tess->addTriangle(v2, v3, v6, noEdges);
   tess->addTriangle(v3, v1, v6, noEdges);
   tess->addTriangle(v1, v4, v5, noEdges);
   tess->addTriangle(v4, v2, v5, noEdges);
   tess->addTriangle(v2, v3, v5, noEdges);
   tess->addTriangle(v3, v1, v5, noEdges);
}

/**
 * Find the nearest node in the tessellation.
 */
TessVertex*
BrainModelSurfaceSphericalTessellator::getNearestNodeInTessellation(const double* xyz,
                                                                    double& distanceSquared)
{
   distanceSquared = std::numeric_limits<double>::max();
   TessVertex* vertexOut = NULL;
   
   Tessellation::VertexVector* vertices = tess->getVertexVector();
   
   const float p[3] = { xyz[0], xyz[1], xyz[2] };

   if (pointLocator != NULL) {
      const int plNode = pointLocator->getNearestPoint(p);
      vertexOut = (*vertices)[plNode];
   }

   if (vertexOut == NULL) {
      const CoordinateFile* coordFile = originalSphereSurface->getCoordinateFile();
      for (Tessellation::VertexVectorIterator vi = vertices->begin();
           vi != vertices->end(); vi++) {
         TessVertex* tv = *vi;
         const int nodeNumber = tv->getUniqueID();
         const double distSQ = coordFile->getDistanceToPointSquared(nodeNumber, p);
         if (distSQ < distanceSquared) {
            distanceSquared = distSQ;
            vertexOut = tv;
         }
      }
   }
   
   return vertexOut;
}

/**
 * Find the tessellation triangle containing the new point.
 */
TessTriangle* 
BrainModelSurfaceSphericalTessellator::findContainingTriangle(const double xyz[3],
                                                              TessVertex* tv)
{
   for (int loop = 0; loop < 2; loop++) {
      //
      // Get the vertex's triangles
      //
      Tessellation::TriangleList triangles = *(tv->getMyTriangles());
      
      //
      // Should we check triangles used by neighbors
      //
      if (loop == 1) {
         //
         // Set for unique triangles
         //
         Tessellation::TriangleSet ts;
         
         //
         // Get the neighboring vertices
         //
         Tessellation::VertexList vl;
         tv->getNeighborVertices(vl);
         
         //
         // Loop through neighboring vertices
         //
         for (Tessellation::VertexListIterator vli = vl.begin(); vli != vl.end(); vli++) {
            //
            // Get the triangles from the vertex
            //
            TessVertex* tv = *vli;
            Tessellation::TriangleList* vertexTriangles = tv->getMyTriangles();
            
            //
            // Loop through triangles
            //
            for (Tessellation::TriangleListIterator tli = vertexTriangles->begin();
                 tli != vertexTriangles->end(); tli++) {
               //
               // Get the triangle
               //
               TessTriangle* tvt = *tli;
               
               //
               // If triangle not previously searched
               //
               if (std::find(triangles.begin(), triangles.end(),
                             tvt) == triangles.end()) {
                  // 
                  // Add to search list
                  //
                  ts.insert(tvt);
               }
            }
         }
         
         //
         // Set the new triangles that are to be searched
         //
         triangles.clear();
         triangles.insert(triangles.begin(), ts.begin(), ts.end());
         
         if (DebugControl::getDebugOn()) {
            std::cout << "Checking neighbor triangles." << std::endl;
         }
      }  // if (loop == 1)
      
      //
      // Iterate through the triangles
      //
      for (Tessellation::TriangleListIterator ti = triangles.begin();
           ti != triangles.end(); ti++) {
         TessTriangle* tt = *ti;
         
         //
         // Get the triangles vertices
         //
         TessVertex *tv1, *tv2, *tv3;
         tt->getVertices(tv1, tv2, tv3);
         
         //
         // Get the node number of the triangle's vertices
         //
         const int n1 = tv1->getUniqueID();
         const int n2 = tv2->getUniqueID();
         const int n3 = tv3->getUniqueID();
         
         //
         // Get coordinates of triangle's vertices
         //
         double* p1 = &pointXYZ[n1*3];
         double* p2 = &pointXYZ[n2*3];
         double* p3 = &pointXYZ[n3*3];
         
         //
         // Find out where ray from origin to query point intersects plane of triangle.
         // Returns false if ray is parallel to plane.
         //
         const double origin[3] = { 0.0, 0.0, 0.0 };
         double pointOnPlane[3];
         if (MathUtilities::rayIntersectPlane(p1, p2, p3,
                                              origin, xyz,
                                              pointOnPlane)) {
            //
            // Normal of tile
            //
            double normal[3];
            MathUtilities::computeNormal((double*)p1, (double*)p2, (double*)p3, normal);

            //
            // Compute areas of triangles formed by triangle vertices and point on plane
            //
            const double area1 = MathUtilities::signedTriangleArea3D(normal, p1, p2, pointOnPlane);   
            if (area1 > 0.0) {
               const double area2 = MathUtilities::signedTriangleArea3D(normal, p2, p3, pointOnPlane);
               if (area2 > 0.0) {
                  const double area3 = MathUtilities::signedTriangleArea3D(normal, p3, p1, pointOnPlane);
                  if (area3 > 0.0) {
                     return tt;
                  }
               }
            }
         }
         
      }
   }
      
   return NULL;
}

/**
 * insert nodes into the sphere.
 */
void 
BrainModelSurfaceSphericalTessellator::insertNodesIntoSphere(
                                              const int callNum) throw (TessellationException)
{
   const double nodeToleranceSquared = 0.1 * 0.1;
   
   int cnt = 0;
   
   //
   // Randomly sort the nodes
   //
   std::vector<int> randomized(numNodes);
   for (int k = 0; k < numNodes; k++) {
      randomized[k] = k;
   }
   std::random_shuffle(randomized.begin(), randomized.end());
   
   //
   // Loop through the nodes
   //
   for (int j = 0; j < numNodes; j++) {
      const int nodeNum = randomized[j];
      
      //
      // If this node is not yet in the sphere
      //
      if (includeNodeInTessellationFlags[nodeNum]) {
         //
         // Mark it as no longer available
         //
         includeNodeInTessellationFlags[nodeNum] = false;
         
         //
         // Get the node's coordinates
         //
         double xyz[3] = { pointXYZ[nodeNum*3], pointXYZ[nodeNum*3+1], pointXYZ[nodeNum*3+2] };
         if (callNum > 0) {
            const float delta = 0.00001;
            xyz[0] += delta * callNum;
            xyz[1] += delta * callNum;
            xyz[2] += delta * callNum;
         }
         
         //
         // Find the nearest vertex in the tessellation
         //
         double distSQ = 0.0;
         TessVertex* nearestVertex = getNearestNodeInTessellation(xyz, distSQ);
         if (nearestVertex == NULL) {
            throw BrainModelAlgorithmException("No nearest node found.");
         }
         
         //
         // If distance to nearest node is less than tolerance, then skip node
         //
         if (distSQ < nodeToleranceSquared) {
            if (DebugControl::getDebugOn()) {
               std::cout << "INFO: Node " << nodeNum << " within tolerance so not added to tessellation." << std::endl;
            }
         }
         else {
            //
            // Find the tile that contains the node
            //
            TessTriangle* tt = findContainingTriangle(xyz, nearestVertex);
            
            //
            // Was a containing triangle found
            //
            if (tt != NULL) {
            
               if (DebugControl::getDebugOn()) {
                  std::cout << "Inserting node: " << nodeNum << std::endl;
               }
               else if ((j % 1000) == 0) {
               }
               
               //
               // Get the triangle's neighbors
               //
               TessTriangle* neighbors[3];
               tt->getNeighborTriangles(neighbors);
               
               //
               // Create a new vertex and add to tessellation
               //
               TessVertex* tv = new TessVertex(nodeNum);
               tess->addVertex(tv);
               
               //
               // Add to the point locator
               //
               if (pointLocator != NULL) {
                  float xyzfloat[3] = { xyz[0], xyz[1], xyz[2] };
                  pointLocator->addPoint(xyzfloat, -1);
               }

               //
               // Get the vertices of the triangle
               //
               TessVertex *tv1, *tv2, *tv3;
               tt->getVertices(tv1, tv2, tv3);
               
               //
               // Get the edges of the triangle
               //
               Tessellation::EdgeVector adjacentEdges;
               tt->getEdges(adjacentEdges, NULL);
               
               //
               // Delete the triangle that contained the node
               //
               tess->removeTriangle(tt);
               
               //
               // Create the three new edges
               //
               adjacentEdges.push_back(tess->addEdge(tv1, tv));
               adjacentEdges.push_back(tess->addEdge(tv2, tv));
               adjacentEdges.push_back(tess->addEdge(tv3, tv));
               
               //
               // Create three new triangles
               //
               TessTriangle* tt1 = tess->addTriangle(tv1, tv2, tv, adjacentEdges);
               TessTriangle* tt2 = tess->addTriangle(tv2, tv3, tv, adjacentEdges);
               TessTriangle* tt3 = tess->addTriangle(tv3, tv1, tv, adjacentEdges);
               
               //
               // Orient the triangle vertices so normal points out
               //
               orientTriangleVertices(tt1);
               orientTriangleVertices(tt2);
               orientTriangleVertices(tt3);
               
               //
               // Mark the triangles neighbors as suspect
               //
               Tessellation::TriangleSet suspectTriangles;
               for (int k = 0; k < 3; k++) {
                  if (neighbors[k] != NULL) {
                     suspectTriangles.insert(neighbors[k]);
                  }
               }

               //
               // process all of the suspect triangles
               //
               processSuspectTriangles(suspectTriangles);
               
               //tess->getEulerCount(v, e, f, euler);

               cnt++;
               if (cnt > 50) {
                  //break;
               }
               
            } // (tt != NULL)
            else {
               if (DebugControl::getDebugOn()) {
                  std::cout << "No containing triangle found for node " << nodeNum << std::endl;
               }
               //
               // Mark it as available since it did not get inserted (try later)
               //
               includeNodeInTessellationFlags[nodeNum] = true;
            }
         }
      }
   }
}      

/**
 * get a triangles circumcenter and radius.
 */
void
BrainModelSurfaceSphericalTessellator::getCircumcenterAndRadius(TessTriangle* tt,
                                                                double center[3],
                                                                double& radius)
{
   //
   // Get the triangles vertices
   //
   TessVertex *tv1, *tv2, *tv3;
   tt->getVertices(tv1, tv2, tv3);
   
   //
   // Get the node number of the triangle's vertices
   //
   const int n1 = tv1->getUniqueID();
   const int n2 = tv2->getUniqueID();
   const int n3 = tv3->getUniqueID();
   
   //
   // Get circumcenter and radius
   //
   double* p1 = &pointXYZ[n1*3];
   double* p2 = &pointXYZ[n2*3];
   double* p3 = &pointXYZ[n3*3];
   MathUtilities::triangleCircumcenter3D(p1,  
                                         p2,
                                         p3,
                                         center,
                                         radius);
}

/**
 * process all of the suspect triangles who made need their edges flipped.
 */
void 
BrainModelSurfaceSphericalTessellator::processSuspectTriangles(Tessellation::TriangleSet& suspectTriangles) throw (TessellationException)
{
   //
   // While there are suspect triangles
   //
   while (suspectTriangles.empty() == false) {
      //
      // Get and remove the triangle from the suspect list
      //
      Tessellation::TriangleSetIterator iter = suspectTriangles.begin();
      TessTriangle* tt = *iter;
      suspectTriangles.erase(tt);
      
      //
      // Determine triangles circumcenter and radius
      //
      double center[3], radius;
      getCircumcenterAndRadius(tt, center, radius);
      
      //
      // Shrink the radius a little to avoid precision problems
      //
      radius *= 0.98;
      
      //
      // Get neighboring triangles
      //
      TessTriangle* triangleNeighbors[3];
      tt->getNeighborTriangles(triangleNeighbors);

      //
      // loop through neighboring triangles
      //
      for (int i = 0; i < 3; i++) {
         //
         // Is triangle valid
         //
         if (triangleNeighbors[i] != NULL) {
         
            //
            // Get the non-shared vertex
            //
            TessVertex* tv = tt->getVertexNotInThisTriangle(triangleNeighbors[i]);
            if (tv != NULL) {
               //
               // Get the coordinate of the vertex
               //
               const int uniqueID = tv->getUniqueID();
               double* xyz = &pointXYZ[uniqueID*3];
               
               //
               // Get distance between new vertex and circumcenter
               //
               const double dist = MathUtilities::distance3D(xyz, center);
            
               //
               // if vertex is within the circumcircle
               //
               if (dist < radius) {          
                  //
                  // Remove other triangle from the suspect list
                  //
                  suspectTriangles.erase(triangleNeighbors[i]);
                  
                  //
                  // Create new triangles by swapping the shared edge
                  //
                  TessTriangle *new1, *new2;
                  tess->swapTriangleEdges(tt, triangleNeighbors[i], new1, new2);
                  
                  //
                  // Orient the new triangle so normal points out
                  //
                  orientTriangleVertices(new1);
                  orientTriangleVertices(new2);
                  
                  //
                  // mark new triangle neighbors as suspect
                  //
                  TessTriangle* neighbors[3];
                  new1->getNeighborTriangles(neighbors);               
                  for (int k = 0; k < 3; k++) {
                     if (neighbors[k] != NULL) {
                        if (neighbors[k] != new2) {
                           suspectTriangles.insert(neighbors[k]);
                        }
                     }
                  }
                  new2->getNeighborTriangles(neighbors);               
                  for (int k = 0; k < 3; k++) {
                     if (neighbors[k] != NULL) {
                        if (neighbors[k] != new1) {
                           suspectTriangles.insert(neighbors[k]);
                        }
                     }
                  }
                  
                  //
                  // Get out of neighboring triangles loop
                  //
                  i = 3;
               } // (dist < radius)
            } // (tv != NULL)
            else {
               //
               // Remove other triangle from the suspect list
               //
               suspectTriangles.erase(triangleNeighbors[i]);
            }
         }
      }
   }
}

/**
 * create a sphere from the tessellation.
 */
void 
BrainModelSurfaceSphericalTessellator::createSphereFromTessellation()
                                              throw (BrainModelAlgorithmException)
{
   //
   // Get the triangles in the tessellation
   //
   Tessellation::CaretTriangleList* triangles = tess->getTriangleList();
   if (triangles->empty()) {
      throw BrainModelAlgorithmException("Tessellation produced no triangles.");
   }
   
   //
   // Create a spherical brain model
   //
   newSphericalSurface = new BrainModelSurface(brainSet);
   newSphericalSurface->setSurfaceType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   
   //
   // Get the coordinate file
   //
   CoordinateFile* cf = newSphericalSurface->getCoordinateFile();
   
   //
   // Copy the coordinate file from the input surface
   //
   const CoordinateFile* coordFile = originalSphereSurface->getCoordinateFile();
   *cf = *coordFile;
   cf->appendToFileComment("\nCreated by tessellating sphere.");
   //cf->setFileName("new_sphere_tessellation.coord");
   cf->setModified();
   
   // 
   // Create a new topology file
   //
   TopologyFile* topoFile = new TopologyFile;
   QString topoFileName = topoFile->getFileName();
   const TopologyFile* inputTopoFile = originalSphereSurface->getTopologyFile();
   if (inputTopoFile != NULL) {
      topoFileName = inputTopoFile->getFileName();
   }
   topoFile->setFileName(topoFileName);
   topoFile->replaceFileNameDescription("CLOSED_CORRECTED");
   topoFile->setTopologyType(TopologyFile::TOPOLOGY_TYPE_CLOSED);
   topoFile->setFileComment("Created by spherical tessellator.");
   
   //
   // Add the topology file to the brain set
   //
   brainSet->addTopologyFile(topoFile);
   
   //
   // Copy the triangles from the tessellation
   //
   TessTriangle* tt = (TessTriangle*)triangles->getFront();
   while (tt != NULL) {
      //
      // Get the vertices of the triangle
      //
      TessVertex* v1;
      TessVertex* v2;
      TessVertex* v3;      
      tt->getVertices(v1, v2, v3);
      
      //
      // Add new triangle to topology
      //
      topoFile->addTile(v1->getUniqueID(),
                        v2->getUniqueID(),
                        v3->getUniqueID());
                        
      tt = (TessTriangle*)tt->getNext();
   }
   
   //
   // Add the topology file to the surface
   //
   newSphericalSurface->setTopologyFile(topoFile);
   
   //
   // Orient the tiles so that they point correctly
   //
   newSphericalSurface->orientTilesOutward(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   newSphericalSurface->computeNormals();
   
   //
   // Rename the spherical to SphericalCorrected
   //
   newSphericalSurface->getCoordinateFile()->replaceFileNameDescription("SPHERICAL_RETESSELLATED");
   
   //
   // Add the surface to the brain set
   //
   brainSet->addBrainModel(newSphericalSurface);
}      

/**
 * orient the triangle so that the normal points out.
 */
void 
BrainModelSurfaceSphericalTessellator::orientTriangleVertices(TessTriangle* tt)
{
   //
   // Get the triangles vertices
   //
   TessVertex *v1, *v2, *v3;
   tt->getVertices(v1, v2, v3);
   
   //
   // Get the coordinates of the nodes
   //
   const double* c1 = &pointXYZ[v1->getUniqueID()*3];
   const double* c2 = &pointXYZ[v2->getUniqueID()*3];
   const double* c3 = &pointXYZ[v3->getUniqueID()*3];
   
   //
   // Compute the tiles normal
   //
   double normal[3];
   MathUtilities::computeNormal((double*)c1,
                              (double*)c2,
                              (double*)c3,
                              normal);
    
   //
   // Use average of the three nodes as normal
   // Assumes sphere is centered at origin
   //
   double avg[3] = { (c1[0] + c2[0] + c3[0]) / 3.0,
                    (c1[1] + c2[1] + c3[1]) / 3.0,
                    (c1[2] + c2[2] + c3[2]) / 3.0 };
   MathUtilities::normalize(avg);
   
   //
   // Does normal point into the sphere
   //
   if (MathUtilities::dotProduct(normal, avg) < 0.0) {
      tt->setVertices(v3, v2, v1);
   }
}      

/**
 * Create the point locator.
 */
void
BrainModelSurfaceSphericalTessellator::createPointLocator()
{
   const CoordinateFile* coordFile = originalSphereSurface->getCoordinateFile();
   float bounds[6];
   coordFile->getBounds(bounds);
   const float smallNumber = 0.01;
   bounds[0] -= smallNumber;
   bounds[1] += smallNumber;
   bounds[2] -= smallNumber;
   bounds[3] += smallNumber;
   bounds[4] -= smallNumber;
   bounds[5] += smallNumber;
   int dims[3] = { 20, 20, 20 }; //{ 10, 10, 10 };
   pointLocator = new PointLocator(bounds, dims);
}

/**
 * execute the tessellationalgorithm.
 */
void 
BrainModelSurfaceSphericalTessellator::executeTessellation() throw (BrainModelAlgorithmException)
{
   if (originalSphereSurface == NULL) {
      throw BrainModelAlgorithmException("Input sphere surface is NULL.");
   }
   
   //
   // Create the tessellation object
   //
   tess = new Tessellation;
   
   //
   // Get the number of nodes
   //
   numNodes = originalSphereSurface->getNumberOfNodes();
      
   try {
      //
      // Get the coordinate file
      //
      const CoordinateFile* coordFile = originalSphereSurface->getCoordinateFile();
      
      //
      // Get the points
      //
      pointXYZ = new double[numNodes*3];
      for (int i = 0; i < numNodes; i++) {
         const float* xyz = coordFile->getCoordinate(i);
         pointXYZ[i*3]   = xyz[0];
         pointXYZ[i*3+1] = xyz[1];
         pointXYZ[i*3+2] = xyz[2];
      }
      
      //
      // Create the point locator
      //
      createPointLocator();
      
      //
      // Create the initial sphere
      //
      createInitialSphere();
      
      if (DebugControl::getDebugOn()) {
         tess->printEulerCounts("Initial sphere: ");
      }

      //
      // Insert the remaining nodes into the sphere
      //
      for (int m = 0; m < 5; m++) {
         //std::cout << "Insertion iteration: " << m << std::endl;
         insertNodesIntoSphere(m);
      }
      
      //
      // Create a new sphere from the tessellation
      //
      createSphereFromTessellation();
      
      if (DebugControl::getDebugOn()) {
         tess->printEulerCounts("Final sphere: ");
         std::cout << "Total nodes input: " << numNodes << std::endl;      
      }
   }
   catch (TessellationException& te) {
      throw BrainModelAlgorithmException(te.whatQString());
   }
}



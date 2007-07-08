
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
#include <sstream>

#define __TESSELLATION_MAIN__
#include "Tessellation.h"
#undef __TESSELLATION_MAIN__

//**********************************************************************************
//** TessVertex Methods
//**********************************************************************************

/**
 * Constructor.
 */
TessVertex::TessVertex(const int uniqueIDIn)
{
   uniqueID = uniqueIDIn;
}

/**
 * Destructor.
 */
TessVertex::~TessVertex()
{
}

/**
 * Add a triangle to the triangles that use this vertex.
 */
void 
TessVertex::addTriangle(TessTriangle* tt)
{
   myTriangles.push_back(tt);
}      

/**
 * Remove a triangle from the list of triangles that use this vertex.
 */
void
TessVertex::removeTriangle(TessTriangle* tt)
{
/*
   if (std::find(myTriangles.begin(), myTriangles.end(), tt) == myTriangles.end()) {
      std::ostringstream str;
      str << "Trying to delete triangle " << tt->triangleNumber
          << " not associated with vertex " << uniqueID;
      throw TessellationException(str.str().c_str());
   }
*/
   myTriangles.remove(tt);
}

/**
 * get a list of vertices that are neighbors of this vertex.
 */
void 
TessVertex::getNeighborVertices(Tessellation::VertexList& vl)
{
   //
   // Used to keep unique list of neighbor vertices
   //
   Tessellation::VertexSet vs;
   
   //
   // Loop through the triangles used by this vertex
   //
   for (Tessellation::TriangleListIterator tl = myTriangles.begin(); tl != myTriangles.end(); tl++) {
      TessTriangle* tt = *tl;
      
      //
      // Get the vertices in the triangle
      //
      TessVertex* tv[3];
      tt->getVertices(tv);
      
      //
      // add vertices to the set
      //
      for (int i = 0; i < 3; i++) {
         if (tv[i] != this) {
            vs.insert(tv[i]);
         }
      }
   }
   
   //
   // update the output list
   //
   vl.clear();
   vl.insert(vl.end(), vs.begin(), vs.end());
}

//**********************************************************************************
//** TessEdge Methods
//**********************************************************************************

/**
 * Constructor.
 */
TessEdge::TessEdge(TessVertex* tv1, TessVertex* tv2)
{
  vertices[0] = tv1;
  vertices[1] = tv2;
  if (tv1 > tv2) {
     std::swap(vertices[0], vertices[1]);
  }
  
  triangles[0] = NULL;
  triangles[1] = NULL;
}

/**
 * Constructor.
 */
TessEdge::~TessEdge()
{
}

/**
 * add a triangle to the list of two triangles that use this edge.
 */
void 
TessEdge::addTriangle(TessTriangle* tt) throw (TessellationException)
{
   if (triangles[0] == NULL) {
      triangles[0] = tt;
   }
   else if (triangles[1] == NULL) {
      triangles[1] = tt;
   }
   else {
      std::ostringstream str;
      str << "TessEdge::addTriangle() Edge already has two triangles ("
          << triangles[0]->triangleNumber << ", "
          << triangles[1]->triangleNumber << ") while adding triangle "
          << tt->triangleNumber;
      throw TessellationException(str.str().c_str());
   }
   
   if ((triangles[0] != NULL) &&
       (triangles[1] != NULL)) {
      if (triangles[0] > triangles[1]) {
         std::swap(triangles[0], triangles[1]);
      }
   }
}

/**
 * get the number of triangles that use this edge.
 */
int 
TessEdge::getNumberOfTrianglesUsingEdge() const
{
   int cnt = 0;
   if (triangles[0] != NULL) cnt++;
   if (triangles[1] != NULL) cnt++;
   return cnt;
}
      
/**
 * Given a triangle, return the other triangle that uses this edge.
 */
TessTriangle* 
TessEdge::getOtherTriangle(TessTriangle* tt)
{
   if (triangles[0] == tt) {
      return triangles[1];
   }
   else if (triangles[1] == tt) {
      return triangles[0];
   }
   return NULL;
}

/**
 * remove a triangle from the list of two triangles that use this edge.
 */
void 
TessEdge::removeTriangle(TessTriangle* tt) throw (TessellationException)
{
   if (triangles[0] == tt) {
      triangles[0] = triangles[1];
      triangles[1] = NULL;
   }
   else if (triangles[1] == tt) {
      triangles[1] = NULL;
   }
   else {
      throw TessellationException("TessEdge::removeTriangle() failed.");
   }
}

//**********************************************************************************
//** TessTriangle Methods
//**********************************************************************************

/**
 * Constructor.
 */
TessTriangle::TessTriangle(TessVertex* tv1,
                           TessVertex* tv2,
                           TessVertex* tv3)
{
   vertices[0] = tv1;
   vertices[1] = tv2;
   vertices[2] = tv3;
   //std::sort(vertices, vertices + 3);
   
   edges[0] = NULL;
   edges[1] = NULL;
   edges[2] = NULL;
   
   triangleNumber = triangleCounter;
   triangleCounter++;
   //std::cout << "Creating triangle " << triangleNumber << std::endl;
}

/**
 * Destructor.
 */
TessTriangle::~TessTriangle()
{
   //std::cout << "Deleting triangle " << triangleNumber << std::endl;
}      

/**
 * Disconnect this triangle from its edges and vertices  (typically done before deleting this triangle).
 */
void 
TessTriangle::disconnect()
{
   //
   // Loop through the edges and vertices
   //
   for (int i = 0; i < 3; i++) {
      if (vertices[i] != NULL) {
         //
         // Remove "me" from the vertex
         //
         vertices[i]->removeTriangle(this);
         vertices[i] = NULL;
      }
      else {
         std::cout << "NULL vertex in TessTriangle::disconnect()" << std::endl;
      }
      if (edges[i] != NULL) {
         //
         // Remove "me" from the edge
         //
         edges[i]->removeTriangle(this);
         edges[i] = NULL;
      }
      else {
         std::cout << "NULL edge in TessTriangle::disconnect()" << std::endl;
      }
   }
}

/**
 * Given an adjacent triangle, find the edge that we share.
 */
TessEdge* 
TessTriangle::getCommonEdge(TessTriangle* tt) throw (TessellationException)
{
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
         if (edges[i] == tt->edges[j]) {
            return edges[i];
         }
      }
   }
   throw TessellationException("TessTriangle::getCommonEdge() failed.");
   return NULL;
}      

/**
 * get the neighboring triangles adjacent to "me".
 */
void 
TessTriangle::getNeighborTriangles(TessTriangle* neighbors[3])
{
   neighbors[0] = NULL;
   neighbors[1] = NULL;
   neighbors[2] = NULL;
   
   int cnt = 0;
   for (int i = 0; i < 3; i++) {
      TessTriangle* tt = edges[i]->getOtherTriangle(this);
      if (tt != NULL) {
         neighbors[cnt] = tt;
         cnt++;
      }
   }
}

/**
 * get the vertex in the other triangle that is not used in this triangle.
 */
TessVertex* 
TessTriangle::getVertexNotInThisTriangle(TessTriangle* tt) throw (TessellationException)
{
   for (int i = 0; i < 3; i++) {
      bool found = false;
      TessVertex* tv = tt->vertices[i];
      for (int j = 0; j < 3; j++) {
         if (tv == vertices[j]) {
            found = true;
            break;
         }
      }
      if (found == false) {
         return tv;
      }
   }
   
   //
   // The only way we should get here is if the two triangles are the SAME !!!
   //
   const QString msg("TessTriangle::getVertexNotInThisTriangle() failed.\n"
                     "The two triangles (" + QString::number(triangleNumber)
                     + " and "
                     + QString::number(tt->triangleNumber)
                     + " have the same vertices.");
   throw TessellationException(msg);
}
      
/**
 * get the vertices of the triangle.
 */
void 
TessTriangle::getVertices(TessVertex* &v1,
                          TessVertex* &v2,
                          TessVertex* &v3)
{
   v1 = vertices[0];
   v2 = vertices[1];
   v3 = vertices[2];
}

/**
 * get the vertices of the triangle.
 */
void 
TessTriangle::getVertices(TessVertex* verticesOut[3])
{
   verticesOut[0] = vertices[0];
   verticesOut[1] = vertices[1];
   verticesOut[2] = vertices[2];
}

/**
 * set the vertices of the triangle.
 */
void 
TessTriangle::setVertices(TessVertex* v1,
                          TessVertex* v2,
                          TessVertex* v3)
{
   vertices[0] = v1;
   vertices[1] = v2;
   vertices[2] = v3;
}

/**
 * get the edges in the triangle.
 */
void 
TessTriangle::getEdges(TessEdge* edgesOut[3])
{
   edgesOut[0] = edges[0];
   edgesOut[1] = edges[1];
   edgesOut[2] = edges[2];
}

/**
 * get the edges in the triangle but excluded edge (Note: edgesOut appended and not cleared).
 */
void 
TessTriangle::getEdges(Tessellation::EdgeVector& edgesOut, TessEdge* excludeThisEdge)
{
   for (int i = 0; i < 3; i++) {
      if ((edges[i] != NULL) &&
          (edges[i] != excludeThisEdge)) {
         edgesOut.push_back(edges[i]);
      }
   }
}      
      
/**
 * get the edges in the triangle (Note: edgesOut appended and not cleared).
 */
void 
TessTriangle::getEdges(Tessellation::EdgeVector& edgesOut)
{
   for (int i = 0; i < 3; i++) {
      if (edges[i] != NULL) {
         edgesOut.push_back(edges[i]);
      }
   }
}      
      
/**
 * get vertex of triangle not part of the provided edge.
 */
TessVertex* 
TessTriangle::getVertexNotInEdge(TessEdge* te) throw (TessellationException)
{
   for (int i = 0; i < 3; i++) {
      if ((vertices[i] != NULL) &&
          (vertices[i] != te->vertices[0]) &&
          (vertices[i] != te->vertices[1])) {
         return vertices[i];
      }
   }
   
   throw TessellationException("TessTriangle::getVertexNotInEdge() failed.");
   return NULL;
}
      
//**********************************************************************************
//** Tessellation Methods
//**********************************************************************************

/**
 * Constructor.
 */
Tessellation::Tessellation()
{
   clear();
}

/**
 * Destructor.
 */
Tessellation::~Tessellation()
{
   clear();
}
      
/**
 * clear the tessellation.
 */
void 
Tessellation::clear()
{
   TessTriangle::triangleCounter = 0;
   for (VertexVectorIterator iter = vertices.begin(); iter != vertices.end(); iter++) {
      delete *iter;
   } 

   triangles.clear();

   edges.clear();
}
      
/**
 * add a vertex to the tessellation.
 */
void 
Tessellation::addVertex(TessVertex* tv)
{
   
   vertices.push_back(tv);
}

/**
 * add a triangle to the tessllation. (vetices must be in tessellation)
 */
TessTriangle* 
Tessellation::addTriangle(TessVertex* tv1,
                          TessVertex* tv2,
                          TessVertex* tv3,
                          EdgeVector& adjacentEdges)
{
   //
   // Create a new triangle
   //
   TessTriangle* tt = new TessTriangle(tv1, tv2, tv3);
   
   //
   // Update vertices for this triangle
   //
   tv1->addTriangle(tt);
   tv2->addTriangle(tt);
   tv3->addTriangle(tt);
   
   //
   // Update the edges that use this triangle
   //
   updateEdgeWithTriangle(tt, tv1, tv2, adjacentEdges, 0);
   updateEdgeWithTriangle(tt, tv2, tv3, adjacentEdges, 1);
   updateEdgeWithTriangle(tt, tv3, tv1, adjacentEdges, 2);
   
   //
   // Add to the list of triangles
   //
   triangles.pushFront(tt);
   //tt->triangleListPosition = triangles.begin();

   return tt;
}

/**
 * remove a triangle from the tessellation.
 */
void 
Tessellation::removeTriangle(TessTriangle* &tt)
{
   tt->disconnect();
   //triangles.erase(tt->triangleListPosition);
   triangles.remove(tt, true);
   //delete tt;
   //tt = NULL;
}

/**
 * Swap the edges of a triangle in the tessellation
 */
void 
Tessellation::swapTriangleEdges(TessTriangle* tt1, TessTriangle* tt2,
                                TessTriangle* &newTriangle1,
                                TessTriangle* &newTriangle2)
{
   newTriangle1 = NULL;
   newTriangle2 = NULL;
   
   //
   // Get the edge shared by the two triangles
   //
   TessEdge* sharedEdge = tt1->getCommonEdge(tt2);
   
   //
   // Get vertices not in the shared edge
   //
   TessVertex* v1 = tt1->getVertexNotInEdge(sharedEdge);
   TessVertex* v2 = tt2->getVertexNotInEdge(sharedEdge);
   
   //
   // Get vertices of the shared edge
   //
   TessVertex* v3 = sharedEdge->vertices[0];
   TessVertex* v4 = sharedEdge->vertices[1];
   
   //
   // Get the edges other than the shared edge
   //
   EdgeVector adjacentEdges;
   tt1->getEdges(adjacentEdges, sharedEdge);
   tt2->getEdges(adjacentEdges, sharedEdge);

   //
   // Remove the two triangles from the tessellation
   //
   removeTriangle(tt1);
   removeTriangle(tt2);
   
   //
   // No longer need the edge
   //
   removeEdge(sharedEdge);
   
   //
   // Create the new edge from the vertices not in the shared edge
   //
   TessEdge* te = addEdge(v1, v2);
   adjacentEdges.push_back(te);
   
   //
   // Add the new triangles
   //
   newTriangle1 = addTriangle(v1, v2, v3, adjacentEdges);
   // 03/21/07  ***** newTriangle1->getEdges(adjacentEdges);
   newTriangle2 = addTriangle(v1, v2, v4, adjacentEdges);
   
   //int v, e, f, eulerCount;
   //getEulerCount(v, e, f, eulerCount);
}

/**
 * Add an edge to this tessellation.
 */
TessEdge* 
Tessellation::addEdge(TessVertex* tv1, TessVertex* tv2)
{
   TessEdge* te = new TessEdge(tv1, tv2);
   edges.pushFront(te);
   //te->edgeListPosition = edges.begin();
   return te;
}

/**
 * Remove an edge from the tessellation.  The edge is also deleted.
 */
void 
Tessellation::removeEdge(TessEdge* te)
{
   edges.remove(te, true);
}

/**
 * update an edge that uses a triangle.
 */
void 
Tessellation::updateEdgeWithTriangle(TessTriangle* tt,
                                     TessVertex* tv1,
                                     TessVertex* tv2,
                                     EdgeVector& edgesToSearchFirst,
                                     const int edgeIndex)
{
   TessEdge* te = NULL;

   for (EdgeVectorIterator iter = edgesToSearchFirst.begin();
        iter != edgesToSearchFirst.end(); iter++) {
      TessEdge* e = *iter;
      if (((e->vertices[0] == tv1) && (e->vertices[1] == tv2)) ||
          ((e->vertices[0] == tv2) && (e->vertices[1] == tv1))) {
         te = e;
         break;
      }
   }
   
   if (te == NULL) {
      te = findEdge(tv1, tv2, false);
      //std::cout << "Searching all edges." << std::endl;
   }
   
   if (te == NULL) {
      te = addEdge(tv1, tv2);
   }
   
   te->addTriangle(tt);
   
   tt->edges[edgeIndex] = te;
}

/**
 * find and edge with the two vertices.
 */
TessEdge* 
Tessellation::findEdge(TessVertex* tv1a, TessVertex* tv2a,
                       const bool edgeMustExist) throw (TessellationException)
{
   //
   // Make tv1 the pointer with the smallest value
   //
   TessVertex* tv1 = tv1a;
   TessVertex* tv2 = tv2a;
   if (tv1 > tv2) {
      std::swap(tv1, tv2);
   }
   
   //
   // Find the edge - note: smallest vertex pointer in edge is in [0]
   //
   TessEdge* te = (TessEdge*)edges.getFront();
   while (te != NULL) {
      if ((te->vertices[0] == tv2) && (te->vertices[1] == tv1)) {
         throw TessellationException("Edges out of order. in Tessellation::findEdge()");
      }
      if ((te->vertices[0] == tv1) && (te->vertices[1] == tv2)) {
         return te;
      }
      te = (TessEdge*)te->getNext();
   }
   
   if (edgeMustExist) {
      throw TessellationException("Tessellation::findEdge() failed.");
   }
   return NULL;
}

/**
 * Check the counts to verify that the tessellation is still correct.
 */
void
Tessellation::getEulerCount(int& v, int& e, int& f, int& eulerCount) throw (TessellationException)
{
   v = static_cast<int>(vertices.size());
   e = static_cast<int>(edges.size());
   f = static_cast<int>(triangles.size());
   eulerCount = v - e + f;
   if (eulerCount != 2) {
      std::ostringstream str;
      str << "Euler count invalid = "
          << eulerCount
          << " (V, E, F) = ("
          << v << ", "
          << e << ", "
          << f << ")";
      throw TessellationException(str.str().c_str());
   }
}


/**
 * print counts of vertices, edges, triangles.
 */
void 
Tessellation::printEulerCounts(const QString& message)
{
   if (message.isEmpty() == false) {
      std::cout << message.toAscii().constData() << std::endl;
   }
   int V, E, F, eulerCount;
   getEulerCount(V, E, F, eulerCount);
   std::cout << "There are " << V << " vertices." << std::endl;
   std::cout << "There are " << E << " edges." << std::endl;
   std::cout << "There are " << F << " triangles." << std::endl;
   std::cout << "V - E + F = " << (V - E + F) << " should be 2" << std::endl;
}
      
                            

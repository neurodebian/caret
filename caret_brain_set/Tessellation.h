#ifndef __TESSELLATION_H__
#define __TESSELLATION_H__

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

#include <exception>
#include <list>
#include <set>
#include <QString>
#include <vector>

#include "CaretLinkedList.h"

class TessEdge;
class TessTriangle;
class TessVertex;

//***********************************************************************************

/// Class for a tessellation exception
class TessellationException : public std::exception {
   public:
      /// Constructor
      TessellationException(const QString& s) { description = s; }
      
      /// Destructor
      virtual ~TessellationException() throw() { }
      
      /// get description of exception
      virtual const char* what() const throw() { return description.toAscii().constData(); }

      /// get description of exception
      QString whatQString() const throw() { return description; }

   protected:
      /// description of the exception
      QString description;
};

//***********************************************************************************

/// Class for storing a dynamic (modifiable) tessellation
class Tessellation {
   public:
      /// a vertex list
      typedef std::list<TessVertex*> VertexList;
      
      /// the vertex list iterator
      typedef std::list<TessVertex*>::iterator VertexListIterator;
      
      /// a vertex set
      typedef std::set<TessVertex*> VertexSet;
      
      /// the vertex set iterator
      typedef std::set<TessVertex*>::iterator VertexSetIterator;
      
      /// a vertex vector
      typedef std::vector<TessVertex*> VertexVector;
      
      /// the vertex vector iterator
      typedef std::vector<TessVertex*>::iterator VertexVectorIterator;
      
      /// an edge list
      typedef CaretLinkedList EdgeList;
      
      /// the edge list iterator
      //typedef std::list<TessEdge*>::iterator EdgeListIterator;

      /// an edge vector
      typedef std::vector<TessEdge*> EdgeVector;
      
      /// the edge vector iterator
      typedef std::vector<TessEdge*>::iterator EdgeVectorIterator;

      /// caret triangle list
      typedef CaretLinkedList CaretTriangleList;
      
      /// the triangle list
      typedef std::list<TessTriangle*> TriangleList;
      
      /// the triangle list iterator
      typedef std::list<TessTriangle*>::iterator TriangleListIterator;

      /// the triangle set
      typedef std::set<TessTriangle*> TriangleSet;
      
      /// the triangle set iterator
      typedef std::set<TessTriangle*>::iterator TriangleSetIterator;

      // Constructor
      Tessellation();
      
      // Destructor
      ~Tessellation();
      
      // clear the tessellation
      void clear();
      
      // Add an edge to this tessellation.
      TessEdge* addEdge(TessVertex* tv1, TessVertex* tv2);
      
      // Remove an edge from the tessellation
      void removeEdge(TessEdge* te);

      // add a vertex to the tessellation
      void addVertex(TessVertex* tv);
      
      // add a triangle to the tessllation (vetices must be in tessellation)
      TessTriangle* addTriangle(TessVertex* tv1,
                                TessVertex* tv2,
                                TessVertex* tv3,
                                EdgeVector& adjacentEdges);
      
      // find and edge with the two vertices
      TessEdge* findEdge(TessVertex* tv1, TessVertex* tv2,
                         const bool edgeMustExist = true)  throw (TessellationException);
      
      // remove a triangle from the tessellation
      void removeTriangle(TessTriangle* &tt);
      
      // Swap the edges of a triangle in the tessellation
      void swapTriangleEdges(TessTriangle* tt1, TessTriangle* tt2,
                             TessTriangle* &newTriangle1,
                             TessTriangle* &newTriangle2);
      
      /// get the vertices
      VertexVector* getVertexVector() { return &vertices; }
      
      /// get the edges
      EdgeList* getEdgeList() { return &edges; }
      
      /// get the triangles
      CaretTriangleList* getTriangleList() { return &triangles; }
      
      // Check the counts to verify that the tessellation is still correct.
      void getEulerCount(int& v, int& e, int& f, int& eulerCount) throw (TessellationException);

      // print counts of vertices, edges, triangles
      void printEulerCounts(const QString& message);
      
   protected:
      /// update an edge with a triangle
      void updateEdgeWithTriangle(TessTriangle* tt,
                                  TessVertex* tv1,
                                  TessVertex* tv2,
                                  std::vector<TessEdge*>& edgesToSearchFirst,
                                  const int edgeIndex);
                                  
      /// the vertices in the tessellation
      VertexVector vertices;
      
      /// the triangles in the tessellation
      CaretTriangleList triangles;
      
      /// the edges in the tessellation
      EdgeList edges;
};
      
//***********************************************************************************
      
/// Class for storing a Tessellation vertex
class TessVertex {
   public:
      // Constructor
      TessVertex(const int uniqueIDIn);
      
      // Destructor
      ~TessVertex();
      
      // Add a triangle to the triangles that use this vertex
      void addTriangle(TessTriangle* tt);
      
      // Remove a triangle from the list of triangles that use this vertex
      void removeTriangle(TessTriangle* tt);
      
      /// get the unique ID
      int getUniqueID() const { return uniqueID; }
      
      /// get the triangles used 
      Tessellation::TriangleList* getMyTriangles() { return &myTriangles; }
      
      // get a list of vertices that are neighbors of this vertex
      void getNeighborVertices(Tessellation::VertexList& vl);
      
   protected:
      /// Triangles used by this vertex
      Tessellation::TriangleList myTriangles;
      
      /// unique identification number
      int uniqueID;
      
};

//***********************************************************************************

/// Class for storing a tessellation edge
class TessEdge : public CaretLinkedList::Node {
   public:
      // Constructor
      TessEdge(TessVertex* tv1, TessVertex* tv2);
      
      // Constructor
      ~TessEdge();
      
      // add a triangle to the list of two triangles that use this edge
      void addTriangle(TessTriangle* tt) throw (TessellationException);
      
      // get the number of triangles that use this edge
      int getNumberOfTrianglesUsingEdge() const;
      
      // Given a triangle, return the other triangle that uses this edge
      TessTriangle* getOtherTriangle(TessTriangle* tt);
      
      // remove a triangle from the list of two triangles that use this edge
      void removeTriangle(TessTriangle* tt) throw (TessellationException);
      
   protected:
      /// Vertices that are end points of this edge
      TessVertex* vertices[2];
      
      /// Tiles using this edge
      TessTriangle* triangles[2];
      
      /// Iterator for position of this edge in the EdgeList
      //Tessellation::EdgeListIterator edgeListPosition;
      
      friend class Tessellation;
      friend class TessTriangle;
};

//***********************************************************************************

/// Class for storing a tessellation triangle
class TessTriangle : public CaretLinkedList::Node {
   public:
      // Constructor
      TessTriangle(TessVertex* tv1,
                   TessVertex* tv2,
                   TessVertex* tv3);
      
      // Destructor
      ~TessTriangle();
      
      // Disconnect this triangle from its edges and vertices (typically done before deleting this triangle)
      void disconnect();
      
      // Given an adjacent triangle, find the edge that we share
      TessEdge* getCommonEdge(TessTriangle* tt) throw (TessellationException);
      
      // get vertex of triangle not part of the provided edge
      TessVertex* getVertexNotInEdge(TessEdge* te) throw (TessellationException);
      
      // get the vertex in the other triangle that is not used in this triangle
      TessVertex* getVertexNotInThisTriangle(TessTriangle* tt) throw (TessellationException);
      
      // get the edges in the triangle
      void getEdges(TessEdge* edgesOut[3]);
      
      // get the edges in the triangle but excluded edge (Note: edgesOut appended and not cleared)
      void getEdges(Tessellation::EdgeVector& edgesOut, TessEdge* excludeThisEdge);
      
      // get the edges in the triangle (Note: edgesOut appended and not cleared).
      void getEdges(Tessellation::EdgeVector& edgesOut);
                       
      // get the vertices of the triangle
      void getVertices(TessVertex* &v1,
                       TessVertex* &v2,
                       TessVertex* &v3);
      
      // get the vertices of the triangle
      void getVertices(TessVertex* verticesOut[3]);
      
      // set the vertices of the triangle
      void setVertices(TessVertex* v1,
                       TessVertex* v2,
                       TessVertex* v3);
      
      // get the neighboring triangles adjacent to "me"
      void getNeighborTriangles(TessTriangle* neighbors[3]);
      
   protected:
      /// Vertices used by this triangle
      TessVertex* vertices[3];
      
      /// Edges used by this triangle
      TessEdge* edges[3];
      
      /// number of this triangle
      int triangleNumber;
      
      /// counter for triangles
      static int triangleCounter;
      
      /// Iterator for position of this triangle in the TriangleList
      Tessellation::TriangleListIterator triangleListPosition;
      
      friend class Tessellation;
      friend class TessEdge;
      friend class TessVertex;
};


#ifdef __TESSELLATION_MAIN__
int TessTriangle::triangleCounter = 0;
#endif // __TESSELLATION_MAIN__

#endif // __TESSELLATION_H__


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

#ifndef __FREE_SURFER_SURFACE_FILE_H__
#define __FREE_SURFER_SURFACE_FILE_H__

#include "AbstractFile.h"

/// Class for reading and writing a free surfer surface file
class FreeSurferSurfaceFile : public AbstractFile {
   public:
      /// Constructor
      FreeSurferSurfaceFile();
      
      /// Destructor
      ~FreeSurferSurfaceFile();
      
      /// clear the file
      void clear();

      /// returns true if the file is isEmpty (contains no data)
      bool empty() const;
      
      /// get the number of vertices
      int getNumberOfVertices() const;
      
      /// get the number of triangles
      int getNumberOfTriangles() const;
      
      /// get the vertex for an index
      int getVertexNumber(const int indx) const;
      
      /// get coordinates of a vertex
      void getVertexCoordinates(const int indx, int& vertexNumber, float xyz[3]) const;
      
      /// set coordinates of a vertex
      void setVertexCoordinates(const int indx, const int vertexNumber, const float xyz[3]);
      
      /// get vertices of a triangle
      void getTriangle(const int triangleNumber, int verts[3]) const;
      
      /// set vertices of a triangle
      void setTriangle(const int triangleNumber, const int verts[3]);
      
      /// get is a free surfer an ascii patch surface file
      bool getIsAsciiPatchFile() const { return patchAsciiFileFlag; }
      
      /// get is a free surfer a binary patch surface file
      bool getIsBinaryPatchFile() const { return patchBinaryFileFlag; }
      
      /// set number of vertices and triangles
      void setNumberOfVerticesAndTriangles(const int numVertices, const int numTriangles);
      
      /// read the specified file 
      //void readFile(const QString& filenameIn)  throw (FileException);
      
      /// Write the current file's memory to the specified name
      //void writeFile(const QString& filenameIn) throw (FileException);
      
   private:
      /// read  file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// write  file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
            
      /// class for storing vertices
      class Vertex {
         public:
            int vertexNumber;
            float xyz[3];
      };
      
      /// the vertices
      std::vector<Vertex> vertices;
      
      /// the triangles (3 per triangle)
      std::vector<int> triangles;
      
      /// this is a free surfer ascii patch file
      bool patchAsciiFileFlag;
      
      /// this is a free surfer binary patch file
      bool patchBinaryFileFlag;
};

#endif //


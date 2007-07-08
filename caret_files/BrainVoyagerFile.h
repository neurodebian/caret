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


#ifndef __VE_BRAIN_VOYAGER_FILE_H__
#define __VE_BRAIN_VOYAGER_FILE_H__

#include "AbstractFile.h"

/// class to hold colors
class BrainVoyagerColorTableElement {
   public:
      /// constructor
      BrainVoyagerColorTableElement(const int brainVoyagerIndexIn);
      
      /// constructor
      BrainVoyagerColorTableElement();
      
      /// destructor
      ~BrainVoyagerColorTableElement();
      
      /// get the color name
      QString getColorName() const { return name; }
      
      /// get the rgb components
      void getRgb(unsigned char rgbOut[3]) const;
      
   private:
      /// initialize the instance
      void initialize();
      
      /// name of color
      QString name;
      
      /// rgb components
      unsigned char rgb[3];
      
      /// index in BrainVoyagerFile
      int brainVoyagerIndex;
      
   friend class BrainVoyagerFile;
};

/// class to read a brain voyager file
class BrainVoyagerFile : public AbstractFile {
   public:
      /// constructor
      BrainVoyagerFile();
      
      /// constructor
      ~BrainVoyagerFile();
      
      /// clear the file
      void clear();

      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return false; } //(getNumberOfTiles() == 0); }
      
      /// get number of color table elements
      int getColorTableSize() { return colorTable.size(); }
      
      /// get a color table element
      const BrainVoyagerColorTableElement* getColorTableElement(const int index) const {
         return &colorTable[index]; 
      }
      
      /// get the number of vertices
      int getNumberOfVertices() const { return numberOfVertices; }
      
      /// get the number of triangles
      int getNumberOfTriangles() const { return numberOfTriangles; }
      
      /// get coordinates of a vertex
      void getVertexCoordinates(const int vertexNumber, float xyz[3]) const;
      
      /// get vertices of a tile
      void getTriangle(const int triangleNumber, int verts[3]) const;
      
      /// get the vertex color
      void getVertexColor(const int vertexNumber, unsigned char rgb[3]) const;
      
      /// get the vertices color index
      int getVertexColorIndex(const int vertexNumber) const {
         return vertexColorTableIndices[vertexNumber];
      };

      /// read the specified file 
      void readFile(const QString& filenameIn)  throw (FileException);
      
      /// Write the current file's memory to the specified name
      void writeFile(const QString& filenameIn) throw (FileException);
      
   private:
   
      /// read  file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                        QDomElement& /* rootElement */) throw (FileException);
      
      /// write  file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                         QDomElement& /* rootElement */) throw (FileException);
      
      /// version of brain voyager file
      float version;
      
      /// number of vertices
      int numberOfVertices;
      
      /// number of triangles
      int numberOfTriangles;
      
      /// X coordinates of vertices
      float* verticesX;
      
      /// X coordinates of vertices
      float* verticesY;
      
      /// X coordinates of vertices
      float* verticesZ;
      
      /// color indices
      std::vector<BrainVoyagerColorTableElement> colorTable;
      
      /// color table indices
      int* vertexColorTableIndices;
      
      /// red vertex color
      unsigned char* vertexRedComponent;
      
      /// green vertex color
      unsigned char* vertexGreenComponent;
      
      /// blue vertex color
      unsigned char* vertexBlueComponent;
      
      /// alpha vertex color
      unsigned char* vertexAlphaComponent;
      
      /// the triangles
      int* triangles;
};

#endif  // __VE_BRAIN_VOYAGER_FILE_H__

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

#include "AbstractFile.h"

#ifndef __VTK_MODEL_FILE_H__
#define __VTK_MODEL_FILE_H__

#include "AbstractFile.h"
#include "CoordinateFile.h"

class BorderColorFile;
class BorderFile;
class CellColorFile;
class CellFile;
class FociColorFile;
class FociFile;
class TransformationMatrix;

/// class for reading/writing accessing VTK polydata files (including XML)
class VtkModelFile : public AbstractFile {
   public:
      /// class for storing a line or polygon
      class VtkModelObject {
         public:
            /// Constructor
            VtkModelObject(const int* ptsIn, const int numPtsIn) {
               for (int i = 0; i < numPtsIn; i++) {
                  pts.push_back(ptsIn[i]);
               }
            }
            
            /// get number of items in object
            int getNumberOfItems() const { return pts.size(); }
            
            /// get a pointer to a point number in the item
            const int* getPointIndex(const int indx) const { return &pts[indx]; }
         
         protected:
            std::vector<int> pts;
      };
      
      /// Constructor
      VtkModelFile();
      
      /// Constructor - converts borders to VTK lines
      VtkModelFile(const BorderFile* bf, const BorderColorFile* colors);
      
      /// Constructor - converts cells to VTK vertices
      VtkModelFile(const CellFile* cells, const CellColorFile* colors);
      
      /// Constructor - converts foci to VTK vertices
      VtkModelFile(const FociFile* foci, const FociColorFile* colors);
      
      /// Constructor (construct from polydata)
      VtkModelFile(vtkPolyData* polyData);
      
      /// Destructor
      ~VtkModelFile();
      
      /// add a coordinate
      void addCoordinate(const float xyz[3],
                         const unsigned char* rgbaColorIn = NULL,
                         const float* normalIn = NULL);
      
      /// Clears current file data in memory.
      void clear();
      
      /// returns true if the file is isEmpty
      bool empty() const;
      
      /// apply a transformation matrix to the vtk model coordinates
      void applyTransformationMatrix(const TransformationMatrix& matrix);

      /// get the display flag
      bool getDisplayFlag() const { return displayFlag; }
      
      /// set the display flag
      void setDisplayFlag(const bool df) { displayFlag = df; };
      
      /// get the coordinates
      const CoordinateFile* getCoordinateFile() const { return &coordinates; }
      
      /// get the number of points
      int getNumberOfPoints() const { return coordinates.getNumberOfCoordinates(); }
      
      /// get the RGBA colors for a point (pointer to the RGBA components)
      const unsigned char* getPointColor(const int indx) const;
      
      /// set the RGBA colors for a point 
      void setPointColor(const int indx, const unsigned char[4]);
      
      /// get the normal for a point
      const float* getPointNormal(const int indx) const;     
      
      /// get the number of vertices
      int getNumberOfVertices() const { return vertices.size(); }
      
      /// get a vertex (pointer to its "point" indices)
      const int* getVertex(const int indx) const;
      
      /// get the number of triangles
      int getNumberOfTriangles() const { return (triangles.size() / 3); }
      
      /// get a vertex (pointer to its 3 "point" indices)
      const int* getTriangle(const int indx) const;
      
      /// get a triangles 3D position (average of its coordinates)
      void getTriangleCoordinate(const int indx, float coord[3]) const;
      
      /// get the number of lines
      int getNumberOfLines() const { return lines.size(); }
      
      /// get a line
      const VtkModelObject* getLine(const int indx) const;
      
      /// get the number of polygons
      int getNumberOfPolygons() const { return polygons.size(); }
      
      /// set the entire model to a specific color
      void setToSolidColor(const unsigned char rgba[4]);
      
      /// get a polygon
      const VtkModelObject* getPolygon(const int index) const;
      
      /// read the file header and its volume data
      void readFile(const QString& fileNameIn) throw (FileException);
      
      /// write the volume file
      void writeFile(const QString& filenameIn) throw (FileException);
      
   protected:
      /// Read the spec file data (should never be called)
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// Write the spec file data (should never be called)
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// read in the model from vtk poly data
      void readPolyData(vtkPolyData* polyData);
      
      /// Coordinates of all points
      CoordinateFile coordinates;
      
      /// storage for point colors
      std::vector<unsigned char> pointColors;
      
      /// the triangles (3 per triangle)
      std::vector<int> triangles;
      
      /// the vertices
      std::vector<int> vertices;
      
      /// lines
      std::vector<VtkModelObject> lines;
      
      /// polygons
      std::vector<VtkModelObject> polygons;
      
      /// display this model flag
      bool displayFlag;
      
      /// point normals
      std::vector<float> pointNormals;
};

#endif // __VTK_MODEL_FILE_H__

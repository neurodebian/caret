
#ifndef __MNI_OBJ_SURFACE_FILE_H__
#define __MNI_OBJ_SURFACE_FILE_H__

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

/// class for reading an MNI OBJ surface file
/// http://www.bic.mni.mcgill.ca/users/mishkin/mni_obj_format.pdf
/// http://www.bic.mni.mcgill.ca/~david/FAQ/polygons_format.txt
class MniObjSurfaceFile : public AbstractFile {
   public:
      // constructor
      MniObjSurfaceFile();
      
      // destructor
      ~MniObjSurfaceFile();
      
      // clear the file
      void clear();
      
      // see if the file is emtpy
      bool empty() const;
      
      /// get the number of points
      int getNumberOfPoints() const { return points.size() / 3; }
      
      // get a point
      const float* getPointXYZ(const int indx) const;
      
      /// get the number of triangles
      int getNumberOfTriangles() const { return triangles.size() / 3; }
      
      // get a triangle
      const int* getTriangle(const int indx) const;
      
      // get a normal
      const float* getNormal(const int indx) const;
      
      // get rgba color
      const unsigned char* getColorRGBA(const int indx) const;
      
   protected:
      /// Read the contents of the file (header has already been read)
      void readFileData(QFile& file,
                        QTextStream& stream,
                        QDataStream& binStream,
                        QDomElement& rootElement) throw (FileException);

      /// Write the file's data (header has already been written)
      void writeFileData(QTextStream& stream,
                         QDataStream& binStream,
                         QDomDocument& xmlDoc,
                         QDomElement& rootElement) throw (FileException);
                         
      /// points 
      std::vector<float> points;
      
      /// normals
      std::vector<float> normals;
      
      /// colors 
      std::vector<unsigned char> colors;
      
      /// triangles
      std::vector<int> triangles;
};

#endif // __MNI_OBJ_SURFACE_FILE_H__

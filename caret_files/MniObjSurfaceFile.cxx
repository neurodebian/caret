
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

#include "MniObjSurfaceFile.h"
#include "SpecFile.h"

/**
 * constructor.
 */
MniObjSurfaceFile::MniObjSurfaceFile()
   : AbstractFile("MNI OBJ Surface File",
                  SpecFile::getMniObjeSurfaceFileExtension(),
                  false, // no header
                  FILE_FORMAT_OTHER,  // default format
                  FILE_IO_NONE,   // supports ascii
                  FILE_IO_NONE,   // supports binary
                  FILE_IO_NONE,   // supports xml
                  FILE_IO_NONE,   // supports xml base64
                  FILE_IO_NONE,   // supports xml gzip base64
                  FILE_IO_READ_ONLY,   // supports other
                  FILE_IO_NONE)   // supports csv
{
}

/**
 * destructor.
 */
MniObjSurfaceFile::~MniObjSurfaceFile()
{
}

/**
 * clear the file.
 */
void 
MniObjSurfaceFile::clear()
{
   points.clear();
   normals.clear();
   colors.clear();
   triangles.clear();
}

/**
 * see if the file is empty.
 */
bool 
MniObjSurfaceFile::empty() const
{
   return (points.empty()  &&
           normals.empty() &&
           colors.empty()  &&
           triangles.empty());
}

/**
 * get a point.
 */
const float* 
MniObjSurfaceFile::getPointXYZ(const int indx) const
{
   return &points[indx*3];
}

/**
 * get a triangle.
 */
const int* 
MniObjSurfaceFile::getTriangle(const int indx) const
{
   return &triangles[indx*3];
}

/**
 * get a normal.
 */
const float* 
MniObjSurfaceFile::getNormal(const int indx) const
{
   return &normals[indx*3];
}

/**
 * get rgba color.
 */
const unsigned char* 
MniObjSurfaceFile::getColorRGBA(const int indx) const
{
   return &colors[indx*4];
}
      
/**
 * Read the contents of the file (header has already been read).
 */
void 
MniObjSurfaceFile::readFileData(QFile& /*file*/,
                                QTextStream& stream,
                                QDataStream& /*binStream*/,
                                QDomElement& /*rootElement*/) throw (FileException)
{
   /// http://www.bic.mni.mcgill.ca/users/mishkin/mni_obj_format.pdf
   /// http://www.bic.mni.mcgill.ca/~david/FAQ/polygons_format.txt
   
   //
   // Check object class
   //
   QString objectClass;
   stream >> objectClass;
   if (objectClass != "P") {
      throw FileException("MNI OBJ file does not contains polygons (1st char == 'P')");
   }
   
   //
   // Ignore these lighting items
   //
   float ambientCoefficient,
         diffusionCoefficient,
         specularCoefficient,
         specularExponent,
         opacity;
   stream >> ambientCoefficient
          >> diffusionCoefficient
          >> specularCoefficient
          >> specularExponent
          >> opacity;

   //
   // number of points
   //
   int numPoints = 0;
   stream >> numPoints;
   if (numPoints <= 0) {
      throw FileException("MNI OBJ file contains no points.");
   }
   
   //
   // Size data structures
   //
   const int numPointComponents = numPoints * 3;
   points.resize(numPointComponents);
   normals.resize(numPointComponents);
   const int numPointColorComponents = numPoints * 4;
   colors.resize(numPointColorComponents);
   
   //
   // Read points
   //
   for (int i = 0; i < numPointComponents; i++) {
      stream >> points[i];
   }

   //
   // Read normals
   //
   for (int i = 0; i < numPointComponents; i++) {
      stream >> normals[i];
   }
   
   //
   // Read number of triangles
   //
   int numTriangles = 0;
   stream >> numTriangles;
   if (numTriangles <= 0) {
      throw FileException("MNI OBJ file contains no triangles.");
   }
   const int numTriangleComponents = numTriangles * 3;
   triangles.resize(numTriangleComponents);
   
   //
   // Read the color flag
   //
   bool haveTriangleColors = false;
   std::vector<float> triangleColors;
   const int numTriangleColorComponents = numTriangles * 4;
   int colorFlag = -1;
   stream >> colorFlag;
   switch (colorFlag) {
      case 0:  // same color applies to all points
         {
            float rf, gf, bf, af;
            stream >> rf >> gf >> bf >> af;
            const unsigned char r = static_cast<unsigned char>(rf * 255.0);
            const unsigned char g = static_cast<unsigned char>(gf * 255.0);
            const unsigned char b = static_cast<unsigned char>(bf * 255.0);
            const unsigned char a = static_cast<unsigned char>(af * 255.0);
            for (int i = 0; i < numPoints; i++) {
               const int i4 = i * 4;
               colors[i4]   = r;
               colors[i4+1] = g;
               colors[i4+2] = b;
               colors[i4+3] = a;
            }
         }
         break;
      case 1:  // colors provided for each triangles
         triangleColors.resize(numTriangleColorComponents);
         for (int i = 0; i < numTriangleColorComponents; i++) {
            stream >> triangleColors[i];
         }
         haveTriangleColors = true;
         break;
      case 2:  // colors provided for each point
         {
            for (int i = 0; i < numPoints; i++) {
               float rf, gf, bf, af;
               stream >> rf >> gf >> bf >> af;
               const unsigned char r = static_cast<unsigned char>(rf * 255.0);
               const unsigned char g = static_cast<unsigned char>(gf * 255.0);
               const unsigned char b = static_cast<unsigned char>(bf * 255.0);
               const unsigned char a = static_cast<unsigned char>(af * 255.0);
               const int i4 = i * 4;
               colors[i4]   = r;
               colors[i4+1] = g;
               colors[i4+2] = b;
               colors[i4+3] = a;
            }
         }
         break;
      default:
         throw FileException("MNI OBJ file has invalid color flag (< 0)");
         break;
   }
   
   //
   // Ignore "end indices"
   //
   for (int i = 0; i < numTriangles; i++) {
      int endIndex;
      stream >> endIndex;
   }
   
   //
   // Read in the triangles
   //
   for (int i = 0; i < numTriangleComponents; i++) {
      stream >> triangles[i];
   }
   
   //
   // Handle triangle colors
   //
   if (haveTriangleColors) {
      std::vector<float> nodeCount(numPoints, 0.0);
      std::vector<float> nodeColorComponents(numPointColorComponents, 0.0);
      
      for (int i = 0; i < numTriangles; i++) {
         //
         // Get the color for the triangle
         //
         const int i4 = i*4;         
         const int* t = getTriangle(i);
         const float* triColor = &triangleColors[i4];
         
         //
         // Loop through triangles 3 nodes
         //
         for (int j = 0; j < 3; j++) {
            //
            // Loop through 4 color components
            //
            for (int k = 0; k < 4; k++) {
               const int pointIndex = t[j];
               nodeColorComponents[pointIndex*4 + k] += triColor[k];
            }
            nodeCount[i] += 1.0;
         }
      }
      
      for (int i = 0; i < numPoints; i++) {
         if (nodeCount[i] > 0.0) {
            const int i4 = i * 4;
            for (int k = 0; k < 4; k++) {
               colors[i4+k] = static_cast<unsigned char>(
                               (nodeColorComponents[i4+k] / nodeCount[i]) * 255.0);
            }
         }
      }
      throw FileException("Triangle colors not supported at this time.");
   }
}

/**
 * Write the file's data (header has already been written).
 */
void 
MniObjSurfaceFile::writeFileData(QTextStream& /*stream*/,
                                 QDataStream& /*binStream*/,
                                 QDomDocument& /*xmlDoc*/,
                                 QDomElement& /*rootElement*/) throw (FileException)
{
   throw FileException("Writing of MniObjSurfaceFile files not supported.");
}


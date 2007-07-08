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

#include "GiftiCommon.h"
#include "GiftiDataArray.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "SurfaceFile.h"

/**
 * constructor
 */
SurfaceFile::SurfaceFile()
   : GiftiDataArrayFile("Surface File",
                        GiftiCommon::categoryCoordinates,
                        GiftiDataArray::DATA_TYPE_FLOAT32,
                        SpecFile::getGiftiFileExtension(),
                        FILE_FORMAT_XML,
                        FILE_IO_NONE,
                        FILE_IO_NONE,
                        FILE_IO_NONE,
                        FILE_IO_NONE,
                        false)
{
   clear();
}

/**
 * constructor
 */
SurfaceFile::SurfaceFile(const int numberOfCoordinates,
                         const int numberOfTriangles)
{
   clear();
   setNumberOfCoordinates(numberOfCoordinates);
   setNumberOfTriangles(numberOfTriangles);
   setModified();
}

/**
 * copy constructor
 */
SurfaceFile::SurfaceFile(const SurfaceFile& sf)
   : GiftiDataArrayFile(sf)
{
   copyHelperSurface(sf);
}

/**
 * destructor
 */
SurfaceFile::~SurfaceFile()
{
   clear();
}

/**
 * assignment operator
 */
SurfaceFile& 
SurfaceFile::operator=(const SurfaceFile& sf)
{
   if (&sf != this) {
      GiftiDataArrayFile::operator=(sf);
      copyHelperSurface(sf);
   }
   return *this;
}

/**
 * the copy helper used by copy constructor and assignement operator.
 */
void 
SurfaceFile::copyHelperSurface(const SurfaceFile& sf)
{
   setFileName("");
   normalsValid = sf.normalsValid;
   setModified();
}

/**
 * clear the file
 */
void 
SurfaceFile::clear()
{
   GiftiDataArrayFile::clear();
   normalsValid = false;
}

/**
 * get the number of coordinates
 */
int 
SurfaceFile::getNumberOfCoordinates() const
{
   const GiftiDataArray* gda = getDataArrayWithCategory(GiftiCommon::categoryCoordinates);
   if (gda != NULL) {
      if (gda->getNumberOfDimensions() > 0) {
         return gda->getDimension(0);
      }
   }
   return 0;
}

/**
 * set the number of coordinates
 */
void 
SurfaceFile::setNumberOfCoordinates(const int num)
{
   //
   // Dimensions of coordinates
   //
   std::vector<int> dim;
   dim.push_back(num);
   dim.push_back(3);
   
   //
   // Find the coordinates category
   //
   GiftiDataArray* coordsArray = getDataArrayWithCategory(GiftiCommon::categoryCoordinates);
   if (coordsArray != NULL) {
      coordsArray->setDimensions(dim);
   }
   else {
      //
      // Create coordinates category
      //
      coordsArray = new GiftiDataArray(this,
                                 GiftiCommon::categoryCoordinates,
                                 GiftiDataArray::DATA_TYPE_FLOAT32,
                                 dim);
      addDataArray(coordsArray);
   }

   //
   // Find the normals category
   //
   GiftiDataArray* normalsArray = getDataArrayWithCategory(GiftiCommon::categoryNormals);
   if (normalsArray != NULL) {
      normalsArray->setDimensions(dim);
   }
   else {
      //
      // Create coordinates category
      //
      normalsArray = new GiftiDataArray(this,
                                 GiftiCommon::categoryNormals,
                                 GiftiDataArray::DATA_TYPE_FLOAT32,
                                 dim);
      addDataArray(normalsArray);
   }
   setModified();
}

/**
 * get the number of triangles
 */
int 
SurfaceFile::getNumberOfTriangles() const
{
   const GiftiDataArray* gda = getDataArrayWithCategory(GiftiCommon::categoryTopologyTriangles);
   if (gda != NULL) {
      if (gda->getNumberOfDimensions() > 0) {
         return gda->getDimension(0);
      }
   }
   return 0;
}

/**
 * set the number of triangles
 */
void 
SurfaceFile::setNumberOfTriangles(const int num)
{
   //
   // Dimensions of triangles
   //
   std::vector<int> dim;
   dim.push_back(num);
   dim.push_back(3);
   
   //
   // Find the triangles category
   //
   GiftiDataArray* trianglesArray =getDataArrayWithCategory(GiftiCommon::categoryTopologyTriangles);
   if (trianglesArray != NULL) {
      trianglesArray->setDimensions(dim);
   }
   else {
      //
      // Create coordinates category
      //
      trianglesArray = new GiftiDataArray(this,
                                 GiftiCommon::categoryTopologyTriangles,
                                 GiftiDataArray::DATA_TYPE_INT32,
                                 dim);
      addDataArray(trianglesArray);
   }
   setModified();
}

/**
 * get a coordinate (pointer to coordinates x/y/z)
 */
const float* 
SurfaceFile::getCoordinate(const int indx) const
{
   const GiftiDataArray* coordsArray = getDataArrayWithCategory(GiftiCommon::categoryCoordinates);
   if (coordsArray != NULL) {
      const int indices[2] = { indx, 0 };
      return coordsArray->getDataFloat32Pointer(indices);
   }
   
   return NULL;
}

/**
 * set a coordinate
 */
void
SurfaceFile::setCoordinate(const int indx, const float xyz[3])
{
   const GiftiDataArray* coordsArray = getDataArrayWithCategory(GiftiCommon::categoryCoordinates);
   if (coordsArray != NULL) {
      for (int i = 0; i < 3; i++) {
         const int indices[2] = { indx, i };
         coordsArray->setDataFloat32(indices, xyz[i]);
      }
      setModified();
   }
}

/**
 * set a coordinate
 */
void 
SurfaceFile::setCoordinate(const int indx, const float x, const float y, const float z)
{
   const float xyz[3] = { x, y, z };
   setCoordinate(indx, xyz);
}

/**
 * get a triangles coordinate indices (3)
 */
const int32_t* 
SurfaceFile::getTriangle(const int indx) const
{
   const GiftiDataArray* coordsArray = getDataArrayWithCategory(GiftiCommon::categoryTopologyTriangles);
   if (coordsArray != NULL) {
      const int indices[2] = { indx, 0 };
      return  coordsArray->getDataInt32Pointer(indices);
   }
   return NULL;
}

/**
 * set a triangle
 */
void 
SurfaceFile::setTriangle(const int indx, const int vertices[3])
{
   const GiftiDataArray* topoArray = getDataArrayWithCategory(GiftiCommon::categoryTopologyTriangles);
   if (topoArray != NULL) {
      for (int i = 0; i < 3; i++) {
         const int32_t indices[2] = { indx, i };
         topoArray->setDataInt32(indices, vertices[i]);
      }
      setModified();
   }
}

/**
 * set a triangle
 */
void 
SurfaceFile::setTriangle(const int indx, const int v1, const int v2, const int v3)
{
   const int v[3] = { v1, v2, v3 };
   setTriangle(indx, v);
}

/**
 * get a normal (pointer to normal's x/y/z).
 */
const float* 
SurfaceFile::getNormal(const int indx) const
{
   const GiftiDataArray* normalsArray = getDataArrayWithCategory(GiftiCommon::categoryNormals);
   if (normalsArray != NULL) {
      const int indices[2] = { indx, 0 };
      return normalsArray->getDataFloat32Pointer(indices);
   }
   
   return NULL;
}

/**
 * set a normal.
 */
void 
SurfaceFile::setNormal(const int indx, const float xyz[3])
{
   const GiftiDataArray* normalsArray = getDataArrayWithCategory(GiftiCommon::categoryNormals);
   if (normalsArray != NULL) {
      for (int i = 0; i < 3; i++) {
         const int indices[2] = { indx, i };
         normalsArray->setDataFloat32(indices, xyz[i]);
      }
      setModified();
   }
}

/**
 * set a normal.
 */
void 
SurfaceFile::setNormal(const int indx, const float x, const float y, const float z)
{
   const float xyz[3] = { x, y, z };
   setNormal(indx, xyz);
}

/**
 * generate normals.
 */
void 
SurfaceFile::generateNormals()
{
   //normalsValid = true;
}      

/**
 * get the type of topology.
 */
QString 
SurfaceFile::getTopologyType() const 
{ 
   const GiftiDataArray* topoArray = getDataArrayWithCategory(GiftiCommon::categoryTopologyTriangles);
   if (topoArray != NULL) {
      const GiftiMetaData* md = topoArray->getMetaData();
      QString topoTypeString;
      if (md->get(AbstractFile::headerTagPerimeterID, topoTypeString)) {
         return topoTypeString;
      }
   }
   return "UNKNOWN"; 
}
      
/**
 * set the type of topology.
 */
void 
SurfaceFile::setTopologyType(const QString& t)
{
   GiftiDataArray* topoArray = getDataArrayWithCategory(GiftiCommon::categoryTopologyTriangles);
   if (topoArray != NULL) {
      GiftiMetaData* md = topoArray->getMetaData();
      QString topoTypeString;
      md->set(AbstractFile::headerTagPerimeterID, t);
      setModified();
   }
}

/**
 * get the type of coordinates.
 */
QString 
SurfaceFile::getCoordinateType() const 
{ 
   const GiftiDataArray* coordsArray = getDataArrayWithCategory(GiftiCommon::categoryCoordinates);
   if (coordsArray != NULL) {
      const GiftiMetaData* md = coordsArray->getMetaData();
      QString coordTypeString;
      if (md->get(AbstractFile::headerTagConfigurationID, coordTypeString)) {
         coordTypeString = StringUtilities::makeUpperCase(coordTypeString);
         return coordTypeString;
      }
   }
   return "UNKNOWN"; 
}
      
/**
 * set the type of coordinates.
 */
void 
SurfaceFile::setCoordinateType(const QString& t)
{
   GiftiDataArray* coordsArray = getDataArrayWithCategory(GiftiCommon::categoryCoordinates);
   if (coordsArray != NULL) {
      GiftiMetaData* md = coordsArray->getMetaData();
      md->set(AbstractFile::headerTagConfigurationID, t);
      setModified();
   }
}
      
/**
 * read  file.
 */
void 
SurfaceFile::readLegacyFileData(QFile& /*file*/, 
                                QTextStream& /*stream*/, 
                                QDataStream& /*binStream*/) throw (FileException)
{
   throw FileException("Legacy files not supported for SurfaceFile.");
}

/**
 * write  file.
 */
void 
SurfaceFile::writeLegacyFileData(QTextStream& /*stream*/, 
                                 QDataStream& /*binStream*/) throw (FileException)
{
   throw FileException("Legacy files not supported for SurfaceFile.");
}      

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
                        GiftiCommon::intentCoordinates,
                        GiftiDataArray::DATA_TYPE_FLOAT32,
                        SpecFile::getGiftiSurfaceFileExtension(),
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
SurfaceFile::copyHelperSurface(const SurfaceFile& /*sf*/)
{
   setFileName("");
   setModified();
}

/**
 * clear the file
 */
void 
SurfaceFile::clear()
{
   GiftiDataArrayFile::clear();
}

/**
 * get the number of coordinates
 */
int 
SurfaceFile::getNumberOfCoordinates() const
{
   const GiftiDataArray* gda = getDataArrayWithIntent(GiftiCommon::intentCoordinates);
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
   GiftiDataArray* coordsArray = getDataArrayWithIntent(GiftiCommon::intentCoordinates);
   if (coordsArray != NULL) {
      coordsArray->setDimensions(dim);
   }
   else {
      //
      // Create coordinates category
      //
      coordsArray = new GiftiDataArray(this,
                                 GiftiCommon::intentCoordinates,
                                 GiftiDataArray::DATA_TYPE_FLOAT32,
                                 dim);
      addDataArray(coordsArray);
   }

   setModified();
}

/**
 * get the number of triangles
 */
int 
SurfaceFile::getNumberOfTriangles() const
{
   const GiftiDataArray* gda = getDataArrayWithIntent(GiftiCommon::intentTopologyTriangles);
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
   GiftiDataArray* trianglesArray =getDataArrayWithIntent(GiftiCommon::intentTopologyTriangles);
   if (trianglesArray != NULL) {
      trianglesArray->setDimensions(dim);
   }
   else {
      //
      // Create coordinates category
      //
      trianglesArray = new GiftiDataArray(this,
                                 GiftiCommon::intentTopologyTriangles,
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
   const GiftiDataArray* coordsArray = getDataArrayWithIntent(GiftiCommon::intentCoordinates);
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
   const GiftiDataArray* coordsArray = getDataArrayWithIntent(GiftiCommon::intentCoordinates);
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
   const GiftiDataArray* coordsArray = getDataArrayWithIntent(GiftiCommon::intentTopologyTriangles);
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
   const GiftiDataArray* topoArray = getDataArrayWithIntent(GiftiCommon::intentTopologyTriangles);
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
 * get the type of topology.
 */
QString 
SurfaceFile::getTopologyType() const 
{ 
   const GiftiDataArray* topoArray = getDataArrayWithIntent(GiftiCommon::intentTopologyTriangles);
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
   GiftiDataArray* topoArray = getDataArrayWithIntent(GiftiCommon::intentTopologyTriangles);
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
   const GiftiDataArray* coordsArray = getDataArrayWithIntent(GiftiCommon::intentCoordinates);
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
   GiftiDataArray* coordsArray = getDataArrayWithIntent(GiftiCommon::intentCoordinates);
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

/**
 * get the coordinate metadata (NULL If invalid) const method.
 */
const GiftiMetaData* 
SurfaceFile::getCoordinateMetaData() const
{
   const GiftiDataArray* coordsArray = getDataArrayWithIntent(GiftiCommon::intentCoordinates);
   if (coordsArray != NULL) {
      return coordsArray->getMetaData();
   }
   
   return NULL;
}

/**
 * get the topology metadata (NULL If invalid) const method.
 */
const GiftiMetaData* 
SurfaceFile::getTopologyMetaData() const
{
   const GiftiDataArray* topoArray = getDataArrayWithIntent(GiftiCommon::intentTopologyTriangles);
   if (topoArray != NULL) {
      return topoArray->getMetaData();
   }
   
   return NULL;
}
      
/**
 * get the coordinate metadata (NULL If invalid).
 */
GiftiMetaData* 
SurfaceFile::getCoordinateMetaData() 
{
   GiftiDataArray* coordsArray = getDataArrayWithIntent(GiftiCommon::intentCoordinates);
   if (coordsArray != NULL) {
      return coordsArray->getMetaData();
   }
   
   return NULL;
}

/**
 * get the topology metadata (NULL If invalid).
 */
GiftiMetaData* 
SurfaceFile::getTopologyMetaData() 
{
   GiftiDataArray* topoArray = getDataArrayWithIntent(GiftiCommon::intentTopologyTriangles);
   if (topoArray != NULL) {
      return topoArray->getMetaData();
   }
   
   return NULL;
}
      
/**
 * convert configuration ID to spec file tag.
 */
QString 
SurfaceFile::convertConfigurationIDToSpecFileTag(const QString& nameIn)
{   
   const QString name(nameIn.toUpper());
   if (name == "RAW") return SpecFile::getRawSurfaceFileTag();
   else if (name == "FIDUCIAL") return SpecFile::getFiducialSurfaceFileTag();
   else if (name == "INFLATED") return SpecFile::getInflatedSurfaceFileTag();
   else if (name == "VERY_INFLATED") return SpecFile::getVeryInflatedSurfaceFileTag();
   else if (name == "SPHERICAL") return SpecFile::getSphericalSurfaceFileTag();
   else if (name == "ELLIPSOIDAL") return SpecFile::getEllipsoidSurfaceFileTag();
   else if (name == "CMW") return SpecFile::getCompressedSurfaceFileTag();
   else if (name == "FLAT") return SpecFile::getFlatSurfaceFileTag();
   else if (name == "FLAT_LOBAR") return SpecFile::getLobarFlatSurfaceFileTag();
   else if (name == "HULL") return SpecFile::getHullSurfaceFileTag();
   else return SpecFile::getUnknownSurfaceFileMatchTag();
}
    

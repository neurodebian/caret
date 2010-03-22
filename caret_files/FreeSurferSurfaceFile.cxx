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

#include <cstdlib>
#include <iostream>

#include "DebugControl.h"
#include "FreeSurferSurfaceFile.h"
#include "SpecFile.h"

/**
 * Constructor
 * Format Info: http://wideman-one.com/gw/brain/fs/surfacefileformats.htm
 */
FreeSurferSurfaceFile::FreeSurferSurfaceFile()
   : AbstractFile("Free Surfer Surface File", 
                  SpecFile::getFreeSurferAsciiSurfaceFileExtension(),
                  false,
                  AbstractFile::FILE_FORMAT_ASCII,
                  AbstractFile::FILE_IO_READ_AND_WRITE,
                  AbstractFile::FILE_IO_READ_ONLY)
{
}

/**
 * Destructor
 */
FreeSurferSurfaceFile::~FreeSurferSurfaceFile()
{
}


/**
 * clear the file.
 */
void 
FreeSurferSurfaceFile::clear()
{
   clearAbstractFile();
   patchAsciiFileFlag = false;
   patchBinaryFileFlag = false;
   vertices.clear();
   triangles.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
FreeSurferSurfaceFile::empty() const 
{ 
   return (getNumberOfVertices() == 0);
} 

/**
 * get the number of vertices.
 */
int 
FreeSurferSurfaceFile::getNumberOfVertices() const 
{ 
   return vertices.size(); 
}

/**
 * get the number of triangles.
 */
int 
FreeSurferSurfaceFile::getNumberOfTriangles() const 
{ 
   return (triangles.size() / 3); 
}

/**
 * get the vertex for an index.
 */
int 
FreeSurferSurfaceFile::getVertexNumber(const int indx) const
{
   return vertices[indx].vertexNumber;
}

/**
 * get coordinates of a vertex.
 */
void 
FreeSurferSurfaceFile::getVertexCoordinates(const int indx, int& vertexNumber, 
                                                 float xyz[3]) const
{
   vertexNumber = vertices[indx].vertexNumber;
   xyz[0] = vertices[indx].xyz[0];
   xyz[1] = vertices[indx].xyz[1];
   xyz[2] = vertices[indx].xyz[2];
}

/**
 * set coordinates of a vertex.
 */
void 
FreeSurferSurfaceFile::setVertexCoordinates(const int indx, const int vertexNumber, 
                                                 const float xyz[3])
{
   vertices[indx].vertexNumber = vertexNumber;
   vertices[indx].xyz[0] = xyz[0];
   vertices[indx].xyz[1] = xyz[1];
   vertices[indx].xyz[2] = xyz[2];
}

/**
 * get vertices of a triangle.
 */
void 
FreeSurferSurfaceFile::getTriangle(const int triangleNumber, int verts[3]) const
{
   const int i3 = triangleNumber * 3;
   verts[0] = triangles[i3];
   verts[1] = triangles[i3 + 1];
   verts[2] = triangles[i3 + 2];
}

/**
 * set vertices of a triangle.
 */
void 
FreeSurferSurfaceFile::setTriangle(const int triangleNumber, const int verts[3])
{
   const int i3 = triangleNumber * 3;
   triangles[i3]     = verts[0];
   triangles[i3 + 1] = verts[1];
   triangles[i3 + 2] = verts[2];
}

/**
 * set number of vertices and triangles.
 */
void 
FreeSurferSurfaceFile::setNumberOfVerticesAndTriangles(const int numVertices, 
                                                       const int numTriangles)
{
   vertices.resize(numVertices);
   triangles.resize(numTriangles * 3);
}

/**
 * read the specified file.
 */
/*
void 
FreeSurferSurfaceFile::readFile(const QString& filenameIn)  throw (FileException)
{
   //
   // Open the file as a text stream
   //
   clear();
   filename = filenameIn);
   QFile file(filename);
   if (file.open(IO_ReadOnly)) {
   
      QTextStream stream(&file);
      try {
         readFileData(file, stream);
      }
      catch (FileException& e) {
         file.close();
         throw e;
      }
   
      file.close();
   }
   else {
      throw FileException(filename, "Failure trying to open: ");
   }
}
*/

/**
 * Write the current file's memory to the specified name.
 */
/*
void 
FreeSurferSurfaceFile::writeFile(const QString& filenameIn) throw (FileException)
{
   filename = filenameIn);
   QFile file(filename);
   if (file.open(IO_ReadOnly)) {   
      QTextStream stream(&file);
      stream.setf(QTextStream::fixed);
      stream.precision(6);
      try {
         writeFileData(stream)
      }
      catch (FileException& e) {
         file.close();
         throw e;
      }
   }
   file.close();
}
*/

/**
 * read  file.
 */
void 
FreeSurferSurfaceFile::readFileData(QFile& file, QTextStream& stream, 
                                    QDataStream& dataStream,
                                  QDomElement& /* rootElement */) throw (FileException)
{
/*
 * Example of "orig" file:
 *
 * #!ascii version of rh.orig
 * 12389 24774
 * 7.500000  -22.500000  43.500000  0
 * 6.500000  -22.500000  43.500000  0
 * 5.500000  -22.500000  43.500000  0
 * ...
 * 0 1 5 0
 * 6 5 1 0
 * 0 61 62 0
 * 0 62 1 0
 *
 *------------------------------------------------------------------------
 * Example of older patch file (probably before FreeSurfer version 3.0.5):
 *
 * #!ascii version of patch /sums2/vanessen/TSAO_TEMP/FALCO_TEMP/rh.orig
 * 6876 13408
 * 0
 * -9.240000  -26.459999  0.000000
 * 1
 * -9.590000  -26.680000  0.000000
 * 2
 * -10.400000  -26.750000  0.000000
 * ...
 * 0
 * 0 1 5
 * 1
 * 6 5 1
 * 2
 * 0 61 62
 * 
 *-------------------------------------------------------------------------
 * Example of new patch file (probably FreeSurface version 3.0.5 and later)
 *
 * #!ascii version of patch ./lh.orig. The 1st index is not a vertex number
 * 35660 70518
 * 2502 vno=2501
 * -48.213696  96.493965  0.000000
 * 2503 vno=2502
 * -48.014599  96.371674  0.000000
 * 2504 vno=2503
 * -47.598007  96.246620  0.000000
 * ...
 * 5177
 * 2501 2502 2506
 * 5178
 * 2501 2506 2505
 * 5179
 * 2501 3048 2502
 *
 */             

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            //
            // First line is a comment
            //
            QString commentLine;
            readLine(filename, stream, commentLine);
            if (commentLine.left(3) != "#!a") {
               throw FileException(filename, " is not an ASCII Free Surfer file.");
            }
            patchAsciiFileFlag = (commentLine.indexOf("patch") != -1);
            setFileComment(commentLine);
            
            //
            // Next line is number of points and tiles
            //
            QString pointsTilesLine;
            readLine(filename, stream, pointsTilesLine);
            int numPoints = 0;
            int numTiles  = 0;
            QTextStream(&pointsTilesLine) >> numPoints >> numTiles;

            if ((numPoints <= 0) || (numTiles <= 0)) {
               throw FileException(filename, "Number of nodes and tiles not found.");
            }         
            
            //
            // Allocate memory
            //
            setNumberOfVerticesAndTriangles(numPoints, numTiles);
            
            //
            // Read in the points
            //
            if (patchAsciiFileFlag) {
               //
               // Read in the patch coordinate
               //
               int nodeNumber;
               float xyz[3];
               for (int i = 0; i < numPoints; i++) {
                  QString line;
                  readLine(filename, stream, line);
                  
                  const int equalPos = line.indexOf('=');
                  if (equalPos >= 0) {
                     QString nodeString(line.mid(equalPos + 1));
                     QTextStream(&nodeString) >> nodeNumber;
                  }
                  else {
                     QTextStream(&line) >> nodeNumber;
                  }
                  readLine(filename, stream, line);
                  QTextStream(&line) >> xyz[0] >> xyz[1] >> xyz[2];
                  setVertexCoordinates(i, nodeNumber, xyz);
               }
            }
            else {
               //
               // Read a normal FreeSurfer surface file
               //
               float xyz[3], w;
               for (int i = 0; i < numPoints; i++) {
                  stream >> xyz[0] >> xyz[1] >> xyz[2] >> w;
                  setVertexCoordinates(i, i, xyz);
               }
            }
            
            if (patchAsciiFileFlag) {
               //
               // Read in patch tiles
               //
               for (int i = 0; i < numTiles; i++) {
                  int tile;
                  stream >> tile;
                  int v[3];
                  stream >> v[0] >> v[1] >> v[2];
                  setTriangle(i, v);
               }
            }
            else {
               //
               // Read in normal surface tiles
               //
               for (int i = 0; i < numTiles; i++) {
                  if (stream.atEnd()) {
                     throw FileException(filename,
                        "At end of file before done reading topology.");
                  }
                  int v[3], dummy;
                  stream >> v[0] >> v[1] >> v[2] >> dummy;
                  setTriangle(i, v);
               }
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         {
            quint8 magic[3] = { 0, 0, 0 };
            dataStream >> magic[0] >> magic[1] >> magic[2];
            if (DebugControl::getDebugOn()) {
               std::cout << "magic " << (unsigned int)magic[0] << " "
                         << (unsigned int)magic[1] << " "
                         << (unsigned int)magic[2] << " "
                         << std::endl;
            }
            if ((magic[0] == 0xff) &&
                (magic[1] == 0xff) &&
                (magic[2] == 0xfe)) {
               //
               // FreeSurfer Files are reportedly big endian
               //
               dataStream.setByteOrder(QDataStream::BigEndian);
               
               //
               // Comment ends with two line feeds '\n' (0x0A)
               //
               QString commentLine;
               char prevChar = 0;
               while (dataStream.atEnd() == false) {
                  qint8 c;
                  dataStream >> c;
                  if ((c == '\n') && (prevChar == '\n')) {
                     commentLine.chop(1); // remove last '\n'
                     break;
                  }
                  commentLine += (char)c;
                  prevChar = c;
               }
               
               if (dataStream.atEnd()) {
                  throw FileException(filename, 
                               "Unexpectedly encountered end of file while reading comment.");
               }
               
               //
               // get vertex and face counts
               //
               int numPoints, numTiles;
               dataStream >> numPoints;
               dataStream >> numTiles;

               if (DebugControl::getDebugOn()) {
                  std::cout << "Vertices/Faces/comment: "
                            << numPoints << " "
                            << numTiles << " "
                            << commentLine.toAscii().constData() << std::endl;
               }
               
               if ((numPoints < 0) || (numTiles < 0)) {
                  throw FileException(filename, "Vertex or Face count less than zero.\n"
                                                  "File should be in big endian order.");
               }
               
               //
               // Allocate memory
               //
               setNumberOfVerticesAndTriangles(numPoints, numTiles);
               
               //
               // Read in vertices
               //
               float xyz[3];
               for (int i = 0; i < numPoints; i++) {
                  dataStream >> xyz[0] >> xyz[1] >> xyz[2];
                  setVertexCoordinates(i, i, xyz);
               }
            
               //
               // Read in normal surface tiles
               //
               for (int i = 0; i < numTiles; i++) {
                  if (stream.atEnd()) {
                     throw FileException(filename,
                        "At end of file before done reading topology.");
                  }
                  int v[3];
                  dataStream >> v[0] >> v[1] >> v[2];
                  setTriangle(i, v);
               }
            }
            //
            // Could be a binary patch format
            //
            else if (filename.contains("patch")) {
               //
               // We're reading a patch file
               //
               patchBinaryFileFlag = true;
               
               //
               // FreeSurfer Files are reportedly big endian
               //
               dataStream.setByteOrder(QDataStream::BigEndian);
               
               //
               // Reset to beginning of file
               //
               file.seek(0);
               
               //
               // Get number of points
               //
               qint32 numPoints;
               dataStream >> numPoints;
               
               if (DebugControl::getDebugOn()) {
                  std::cout << "FreeSurface patch point count: " << numPoints << std::endl;
               }
               
               if ((numPoints < 0) || (numPoints > 10000000)) {
                  throw FileException(filename, "Vertex count=" + QString::number(numPoints) + " invalid.\n"
                                                  "File is not in big endian order or format has changed.");
               }

               //
               // Allocate memory
               //
               setNumberOfVerticesAndTriangles(numPoints, 0);
               
               //
               // Read in vertices
               //
               for (int i = 0; i < numPoints; i++) {
                  qint32 vertexNumber;
                  qint16 x, y, z;
                  
                  dataStream >> vertexNumber >> x >> y >> z;
                  const float xyz[3] = { x, y, z };
                  
                  //
                  // Vertex indices are negative if part of border ???
                  //
                  vertexNumber = std::abs(vertexNumber);
                  vertexNumber--;
                  if (vertexNumber < 0) {
                     throw FileException(filename, "Free Surfer vertex number less than zero "
                                         + QString::number(vertexNumber));
                  }
                  setVertexCoordinates(i, vertexNumber, xyz);
               }
            }
            else {
               QString str("FreeSurfer File, invalid magic number (hex): ");
               str += QString::number(magic[0], 16);
               str += " ";
               str += QString::number(magic[1], 16);
               str += " ";
               str += QString::number(magic[2], 16);
               throw FileException(filename, str);
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "\"XML\" file format not supported.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "\"XML-Base64 Encoding\" file format not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "\"XML-Base64 GZIP Encoding\" file format not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "\"Other\" file format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }   
}

/**
 * write  file.
 */
void 
FreeSurferSurfaceFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   stream << "#!ascii version written be Caret\n";
   
   const int numVertices = getNumberOfVertices();
   const int numTriangles = getNumberOfTriangles();
   stream << numVertices << " " << numTriangles << "\n";
   
   for (int i = 0; i < numVertices; i++) {
      const Vertex& v = vertices[i];
      stream << v.xyz[0] << " " << v.xyz[1] << " " << v.xyz[2] << " 0\n";
   }
   
   for (int i = 0; i < numTriangles; i++) {
      const int i3 = i * 3;
      stream << triangles[i3] << " " << triangles[i3+1]
               << " " << triangles[i3+2] << " 0\n";
   }
}

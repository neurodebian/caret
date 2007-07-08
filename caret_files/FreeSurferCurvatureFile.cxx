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

#include <iostream>

#include "DebugControl.h"

#include "FreeSurferCurvatureFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * Constructor.
 * Format Info: http://wideman-one.com/gw/brain/fs/surfacefileformats.htm
 */
FreeSurferCurvatureFile::FreeSurferCurvatureFile()
   : AbstractFile("Free Surfer Curvature File", 
                  SpecFile::getFreeSurferAsciiCurvatureFileExtension(),
                  false)
{
}

/**
 * Destructor.
 */
FreeSurferCurvatureFile::~FreeSurferCurvatureFile()
{
}

/**
 * clear the file.
 */
void 
FreeSurferCurvatureFile::clear()
{
   clearAbstractFile();
   curvature.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
FreeSurferCurvatureFile::empty() const
{
   return (getNumberOfVertices() == 0);
}

/**
 * get the number of vertices.
 */
int 
FreeSurferCurvatureFile::getNumberOfVertices() const
{
   return curvature.size();
}

/**
 * set number of vertices.
 */
void 
FreeSurferCurvatureFile::setNumberOfVertices(const int numVertices)
{
   curvature.resize(numVertices);
}

/**
 * get curvature value.
 */
void 
FreeSurferCurvatureFile::getCurvature(const int vertexNumber,
                                      float xyz[3], float& curve) const
{
   xyz[0] = curvature[vertexNumber].xyz[0];
   xyz[1] = curvature[vertexNumber].xyz[1];
   xyz[2] = curvature[vertexNumber].xyz[2];
   curve  = curvature[vertexNumber].curve;
}

/**
 * set curvature value.
 */
void 
FreeSurferCurvatureFile::setCurvature(const int vertexNumber, const float xyz[3],
                                      const float curve)
{
   curvature[vertexNumber].xyz[0] = xyz[0];
   curvature[vertexNumber].xyz[1] = xyz[1];
   curvature[vertexNumber].xyz[2] = xyz[2];
   curvature[vertexNumber].curve  = curve;
}

/**
 * read the specified file.
 */ 
/*
void 
FreeSurferCurvatureFile::readFile(const QString& filenameIn)  throw (FileException)
{
}
*/

/**
 * Write the current file's memory to the specified name.
 */
/*
void 
FreeSurferCurvatureFile::writeFile(const QString& filenameIn) throw (FileException)
{
}
*/

/**
 * read  file.
 */
void 
FreeSurferCurvatureFile::readFileData(QFile& file, 
                                      QTextStream& stream, 
                                      QDataStream& dataStream,
                                      QDomElement& /* rootElement */) throw (FileException)
{
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            float xyz[3], curv;
            QString line;
            std::vector<QString> tokens;
            while(stream.atEnd() == false) {
               readLineIntoTokens(filename, stream, line, tokens);
               xyz[0] = StringUtilities::toFloat(tokens[1]);
               xyz[1] = StringUtilities::toFloat(tokens[2]);
               xyz[2] = StringUtilities::toFloat(tokens[3]);
               curv   = StringUtilities::toFloat(tokens[4]);
               if (tokens.size() >= 5) {
                  Curvature c(xyz, curv);
                  curvature.push_back(c);
               }
               else if (tokens.size() == 0) {
                  break;
               }
               else {
                  QString msg("Invalid line: ");
                  msg.append(line);
                  throw FileException(filename, msg);
               }
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         {
            const int magic = readThreeByteInteger(dataStream);
            
            if (DebugControl::getDebugOn()) {
               std::cout << "FreeSurfer Binary Curvature File" << std::endl;
               std::cout << "   magic: " << magic << std::endl;
            }
            
            if (magic == 16777215) {
               int vertexCount, faceCount, valuesPerVertex;
               dataStream >> vertexCount >> faceCount >> valuesPerVertex;
               if (DebugControl::getDebugOn()) {
                  std::cout << "   vertex count: " << vertexCount << std::endl;
                  std::cout << "   face count: " << faceCount << std::endl;
                  std::cout << "   values per vertex: " << valuesPerVertex << std::endl;
               }
               
               float xyz[3] = { 0.0, 0.0, 0.0 };
               for (int i = 0; i < vertexCount; i++) {
                  float f;
                  dataStream >> f;
                  Curvature c(xyz, f);
                  curvature.push_back(c);
               }
            }
            else {
               file.seek(0);
               const int vertexCount = readThreeByteInteger(dataStream);
               const int faceCount   = readThreeByteInteger(dataStream);
               if (DebugControl::getDebugOn()) {
                  std::cout << "   vertex count: " << vertexCount << std::endl;
                  std::cout << "   face count: " << faceCount << std::endl;
               }
               
               float xyz[3] = { 0.0, 0.0, 0.0 };
               for (int i = 0; i < vertexCount; i++) {
                  qint16 curv;
                  dataStream >> curv;
                  Curvature c(xyz, static_cast<float>(curv) / 100.0);
                  curvature.push_back(c);
               }
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

   //
   // Read until end of file
   //
}

/**
 * write  file.
 */
void 
FreeSurferCurvatureFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   const int num = getNumberOfVertices();
   for (int i = 0; i < num; i++) {
      const Curvature& c = curvature[i];
      stream << i << " " << c.xyz[0] << " " << c.xyz[1]
             << " " << c.xyz[2] << " " << c.curve << "\n";
   }
}

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
#include "FreeSurferFunctionalFile.h"
#include "SpecFile.h"

/**
 * Constructor.
 * Format Info: http://wideman-one.com/gw/brain/fs/surfacefileformats.htm
 */
FreeSurferFunctionalFile::FreeSurferFunctionalFile()
   : AbstractFile("Free Surfer Functional File", 
                  SpecFile::getFreeSurferAsciiFunctionalFileExtension(),
                  false,
                  AbstractFile::FILE_FORMAT_ASCII,
                  AbstractFile::FILE_IO_READ_AND_WRITE,
                  AbstractFile::FILE_IO_READ_ONLY)
{
}

/**
 * Destructor.
 */
FreeSurferFunctionalFile::~FreeSurferFunctionalFile()
{
}

/**
 * clear the file.
 */
void 
FreeSurferFunctionalFile::clear()
{
   clearAbstractFile();
   functionalData.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
FreeSurferFunctionalFile::empty() const
{
   return (getNumberOfFunctionalItems() == 0);
}

/**
 * get the number of vertices.
 */
int 
FreeSurferFunctionalFile::getNumberOfFunctionalItems() const
{
   return functionalData.size();
}

/**
 * set number of vertices.
 */
void 
FreeSurferFunctionalFile::setNumberOfFunctionalItems(const int numItems)
{
   functionalData.resize(numItems);
}

/**
 * get functional value value.
 */
void 
FreeSurferFunctionalFile::getFunctionalData(const int index, int& vertexNumber, 
                                             float& funcValue) const
{
   vertexNumber = functionalData[index].vertexNumber;
   funcValue    = functionalData[index].funcValue;
}

/**
 * set curvature value.
 */
void 
FreeSurferFunctionalFile::setFunctionalData(const int index, const int vertexNumber, 
                                             const float funcValue)
{
   functionalData[index].vertexNumber = vertexNumber;
   functionalData[index].funcValue    = funcValue;
}

/**
 * add functional value.
 */
void 
FreeSurferFunctionalFile::addFunctionalData(const int vertexNumber, const float funcValue)
{
   FuncData fd(vertexNumber, funcValue);
   functionalData.push_back(fd);
}

/**
 * read  file.
 */
void 
FreeSurferFunctionalFile::readFileData(QFile& /*file*/, 
                                       QTextStream& stream, 
                                       QDataStream& dataStream,
                                       QDomElement& /* rootElement */) throw (FileException)
{
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            //
            // Read in the latency line
            //
            QString latencyLine;
            readLine(stream, latencyLine);
            
            //
            // Read number of points in file
            //
            QString numPointsInFileLine;
            readLine(stream, numPointsInFileLine);
            const int numPoints = numPointsInFileLine.toInt();
            setNumberOfFunctionalItems(numPoints);
            
            //
            // Read in functional data
            //
            int nodeNumber;
            float value;
            for (int i = 0; i < numPoints; i++) {
               stream >> nodeNumber >> value;
               setFunctionalData(i, nodeNumber, value);
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         {
            //
            // ignore latency
            //
            qint16 latency;
            dataStream >> latency;
            
            //
            // Read number of vertex-value pairs
            //
            const int numVertexValuePairs = readThreeByteInteger(dataStream);
            setNumberOfFunctionalItems(numVertexValuePairs);
            
            if (DebugControl::getDebugOn()) {
               std::cout << "FreeSurfer binary wieght file number of vertex-value pairs: "
                         << numVertexValuePairs << std::endl;
            }
            
            //
            // Read in the functional data
            //
            for (int i = 0; i < numVertexValuePairs; i++) {
               const int vertex = readThreeByteInteger(dataStream);
               float f;
               dataStream >> f;
               setFunctionalData(i, vertex, f);
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
         throw FileException(filename, "Reading Comma Separated Value File Format not supported.");
         break;
   }
}

/**
 * write  file.
 */
void 
FreeSurferFunctionalFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Write latency
   //
   stream << "0.0" << "\n"; 
   
   //
   // Write number of vertices
   //
   const int num = getNumberOfFunctionalItems();
   stream << num << "\n";
   
   //
   // Write the functional data
   //
   for (int i = 0; i < num; i++) {
      const FuncData& fd = functionalData[i];
      stream << fd.vertexNumber << " " << fd.funcValue << "\n";
   }
}

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

#include "CoordinateFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "SurfaceVectorFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
SurfaceVectorFile::SurfaceVectorFile()
   : NodeAttributeFile("Surface Vector File",
                       SpecFile::getSurfaceVectorFileExtension(),
                       FILE_FORMAT_BINARY,
                       FILE_IO_READ_AND_WRITE,
                       FILE_IO_READ_AND_WRITE,
                       FILE_IO_NONE,
                       FILE_IO_NONE)
                       
{
   clear();
}
 
/**
 * destructor.
 */
SurfaceVectorFile::~SurfaceVectorFile()
{
   clear();
}

/**
 * creates vectors from coord file differences (A - B).
 */
void
SurfaceVectorFile::addUpdateColumn(const CoordinateFile* coordA,
                                   const CoordinateFile* coordB,
                                   const TopologyFile* tf,
                                   const int columnNumberIn,
                                   const QString& columnNameIn,
                                   const QString& columnCommentIn) throw (FileException)
{
   const int numNodes = coordA->getNumberOfCoordinates();
   if (numNodes <= 0) {
      throw FileException("Surface A has no nodes");
   }
   if (coordB->getNumberOfCoordinates() != numNodes) {
      throw FileException("The surfaces have a different number of nodes.");
   }
   
   int column = columnNumberIn;
   if (getNumberOfNodes() <= 0) {
      setNumberOfNodesAndColumns(numNodes, 1);
      column = getNumberOfColumns() - 1;
   }
   else if ((column < 0) || (column >= getNumberOfColumns())) {
      addColumns(1);
      column = getNumberOfColumns() - 1;
   }

   setColumnName(column, columnNameIn);
   setColumnComment(column, columnCommentIn);
   
   std::vector<bool> nodeHasNeighbors(numNodes, true);
   
   if (tf != NULL) {
      const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
      for (int i = 0; i < numNodes; i++) {
         nodeHasNeighbors[i] = th->getNodeHasNeighbors(i);
      }
   }
   
   for (int i = 0; i < numNodes; i++) {
      if (nodeHasNeighbors[i]) {
         const float* xyzA = coordA->getCoordinate(i);
         const float* xyzB = coordB->getCoordinate(i);
         const float d[3] = {
            xyzA[0] - xyzB[0],
            xyzA[1] - xyzB[1],
            xyzA[2] - xyzB[2]
         };
         
         setVector(i, column, d);
      }
   }
}

/**
 * append a node attribute  file to this one.
 */
void 
SurfaceVectorFile::append(NodeAttributeFile& naf) throw (FileException)
{
   SurfaceVectorFile& svf = dynamic_cast<SurfaceVectorFile&>(naf);
   
   const int oldNumCols  = numberOfColumns;
   
   const int appendNumCols = svf.getNumberOfColumns();
   const int appendNumNodes = svf.getNumberOfNodes();
   if (numberOfNodes > 0) {
      if (numberOfNodes != appendNumNodes) {
         throw FileException("Cannot append metrics, number of columns does not match.");
      }
      addColumns(appendNumCols);
   }
   else {
      setNumberOfNodesAndColumns(appendNumNodes, appendNumCols);
   }
   
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < appendNumCols; j++) {
         const int newColNum = j + oldNumCols;
         setVector(i, newColNum, svf.getVector(i, j));
      }   
   }
   
   for (int j = 0; j < appendNumCols; j++) {
      const int newColNum = j + oldNumCols;
      setColumnName(newColNum, svf.getColumnName(j));
      setColumnComment(newColNum, svf.getColumnComment(j));
   }
   
   appendFileComment(svf);
   setModified();
}

/**
 * append a node attribute file to this one but selectively load/overwrite columns
 * columnDestination is where naf's columns should be (-1=new, -2=do not load)
 */
void 
SurfaceVectorFile::append(NodeAttributeFile& naf, 
                    std::vector<int> columnDestinationIn,
                    const FILE_COMMENT_MODE fcm) throw (FileException)
{
   bool setTheFileNameFlag = false;
   
   std::vector<int> columnDestination = columnDestinationIn;
   
   SurfaceVectorFile& svf = dynamic_cast<SurfaceVectorFile&>(naf);
   
   int numNodes = getNumberOfNodes();
   const int numCols  = getNumberOfColumns();
   
   const int appendNumCols = svf.getNumberOfColumns();
   const int appendNumNodes = svf.getNumberOfNodes();
   bool fileWasEmpty = false;
   if (numNodes > 0) {
      if (numNodes != appendNumNodes) {
         throw FileException("Cannot append metrics, number of nodes does not match.");
      }
   }
   else {
      fileWasEmpty = true;
      numNodes = appendNumNodes;
      setTheFileNameFlag = true;
   }
   
   setModified();
   
   //
   // Find out how many columns need to be added
   //
   int numColumnsToAdd = 0;
   int newColumnIndex = numCols;
   for (int i = 0; i < svf.getNumberOfColumns(); i++) {
      if (columnDestination[i] == APPEND_COLUMN_NEW) {
         numColumnsToAdd++;
         columnDestination[i] = newColumnIndex;
         newColumnIndex++;
      }
   }
   if (numColumnsToAdd <= 0) {
      return;
   }

   //
   // Add the columns
   //
   if (fileWasEmpty) {
      setNumberOfNodesAndColumns(numNodes, numColumnsToAdd);
   }
   else {
      addColumns(numColumnsToAdd);
   }
   
   //
   // add new column naming
   //
   for (int n = 0; n < appendNumCols; n++) {
      if (columnDestination[n] >= 0) {
         const int col = columnDestination[n];
         setColumnName(col, svf.getColumnName(n));
         setColumnComment(col, svf.getColumnComment(n));
      }
   }

   //
   // transfer vector data
   //
   for (int i = 0; i < numNodes; i++) {
      //
      // new data
      //
      for (int k = 0; k < appendNumCols; k++) {
         if (columnDestination[k] >= 0) {
            const int col = columnDestination[k];
            setVector(i, col, svf.getVector(i, k));
         }
      }
   }

   if (setTheFileNameFlag) {
      setFileName(svf.getFileName());
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(svf, fcm);
   setModified();
}

/**
 * add columns to this node attribute file.
 */
void 
SurfaceVectorFile::addColumns(const int numberOfNewColumns)
{
   const int oldNumberOfColumns = numberOfColumns;
   const std::vector<float> oldVectors = vectors;
   
   numberOfColumns += numberOfNewColumns;
   const int newSize = numberOfNodes * numberOfColumns * numberOfItemsPerColumn;
   vectors.resize(newSize, 0.0);
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < oldNumberOfColumns; j++) {
         const int oldOffset = i * oldNumberOfColumns * numberOfItemsPerColumn 
                             + j * numberOfItemsPerColumn;
         const int offset = getOffset(i, j);
         for (int k = 0; k < numberOfItemsPerColumn; k++) {
            vectors[offset + k] = oldVectors[oldOffset + k];
         }
      }
   }
   numberOfNodesColumnsChanged();
   setModified();
}

/**
 * add nodes to this file.
 */
void 
SurfaceVectorFile::addNodes(const int numberOfNodesToAdd)
{
   numberOfNodes += numberOfNodesToAdd;
   const int newSize = numberOfNodes * numberOfColumns + numberOfItemsPerColumn;
   vectors.resize(newSize, 0.0);
   numberOfNodesColumnsChanged();
   setModified();
}

/**
 * deform "this" node attribute file placing the output in "deformedFile".
 */
void 
SurfaceVectorFile::deformFile(const DeformationMapFile& /*dmf*/, 
                        NodeAttributeFile& /*deformedFile*/,
                        const DEFORM_TYPE /*dt*/) const throw (FileException)
{
   throw FileException("Deformation not supported for SurfaceVectorFile.");
}
                        
/**
 * set the number of nodes and columns in the file.
 */
void 
SurfaceVectorFile::setNumberOfNodesAndColumns(const int numNodes, const int numCols)
{
   numberOfNodes = numNodes;
   numberOfColumns = numCols;
   numberOfItemsPerColumn = 3;
   const int num = numberOfNodes * numberOfColumns * numberOfItemsPerColumn;
   
   if (num == 0) {
      vectors.clear();
   }
   else {
      vectors.resize(num, 0.0);
   }
   
   numberOfNodesColumnsChanged();
   setModified();
}

/**
 * reset a column of data.
 */
void 
SurfaceVectorFile::resetColumn(const int columnNumber)
{
   for (int i = 0; i < numberOfNodes; i++) {
      setVector(i, columnNumber, 0.0, 0.0, 0.0);
   }
   setModified();
}

/**
 * remove a column of data.
 */
void 
SurfaceVectorFile::removeColumn(const int columnNumber)
{
   if (numberOfColumns <= 1) {
      clear();
      return;
   }
   
   const int numColM1 = (numberOfColumns - 1);
   if (columnNumber < numColM1) {
      for (int i = 0; i < numberOfNodes; i++) {
         for (int j = columnNumber; j < numColM1; j++) {
            setVector(i, j, getVector(i, j + 1));
         }
      }
      
      for (int j = columnNumber; j < numColM1; j++) {
         setColumnName(j, getColumnName(j + 1));
         setColumnComment(j, getColumnComment(j + 1));
      }
   }

   numberOfColumns--;
   setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns);
   
   numberOfNodesColumnsChanged();
   setModified();
}

/**
 * clear the file's contents.
 */
void 
SurfaceVectorFile::clear()
{
   clearNodeAttributeFile();
   setNumberOfNodesAndColumns(0, 0);
}

/**
 * get a vector.
 */
const float* 
SurfaceVectorFile::getVector(const int node,
                             const int column) const
{
   const int offset = getOffset(node, column);
   return &vectors[offset];  
}
                       
/**
 * get a vector.
 */
void 
SurfaceVectorFile::getVector(const int node,
                             const int column,
                             float xyz[3]) const
{
   const int offset = getOffset(node, column);
   xyz[0] = vectors[offset];  
   xyz[1] = vectors[offset+1];  
   xyz[2] = vectors[offset+2];  
}
               
/**
 * get a vector.
 */
void 
SurfaceVectorFile::getVector(const int node,
                             const int column,
                             float& x,
                             float& y,
                             float& z) const
{
   const int offset = getOffset(node, column);
   x = vectors[offset];  
   y = vectors[offset+1];  
   z = vectors[offset+2];  
}
               
/**
 * set a vector.
 */
void 
SurfaceVectorFile::setVector(const int node, 
                             const int column,
                             const float xyz[3])
{
   setVector(node, column, xyz[0], xyz[1], xyz[2]);
   setModified();
}
               
/**
 * set a vector.
 */
void 
SurfaceVectorFile::setVector(const int node, 
                             const int column,
                             const float x,
                             const float y,
                             const float z)
{
   const int offset = getOffset(node, column);
   vectors[offset]   = x;
   vectors[offset+1] = y;
   vectors[offset+2] = z;
   setModified();
}

/**
 * Read the contents of the file (header has already been read).
 */
void 
SurfaceVectorFile::readFileData(QFile& file, 
                          QTextStream& stream,
                          QDataStream& binStream,
                          QDomElement& /*rootElement*/) throw (FileException)
{
   int numNodes = -1;
   int numCols  = -1;
   
   int fileVersion = 0;
   
   bool readingTags = true;
   while (readingTags) {
      QString tag, tagValue;
      readTagLine(stream, tag, tagValue);
      if (tag == tagFileVersion) {
         fileVersion = tagValue.toInt();
         if (fileVersion > 0) {
            throw FileException(filename, "Surface Vector File version is greater than 0.\n"
                                            "You need a newer version of Caret.");
         }
      }
      else if (tag == tagNumberOfNodes) {
         if (tagValue.isEmpty()) {
            throw FileException(filename, "Reading line containing number of nodes");
         }
         else {
            numNodes = tagValue.toInt();
            if (numCols > 0) {
               setNumberOfNodesAndColumns(numNodes, numCols);
            }
         }
      }
      else if (tag == tagNumberOfColumns) {
         if (tagValue.isEmpty()) {
            throw FileException(filename, "Reading line containing number of columns");
         }
         else {
            numCols = tagValue.toInt();
            if (numNodes > 0) {
               setNumberOfNodesAndColumns(numNodes, numCols);
            }
         }
      }
      else if (tag == tagFileTitle) {
         fileTitle = tagValue;
      }
      else if (tag == tagBeginData) {
         readingTags = false;
      }
      else if (tag == tagColumnName) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         columnNames[index] = name;
      }
      else if (tag == tagColumnComment) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         columnComments[index] = StringUtilities::setupCommentForDisplay(name);
      }
      else {
         std::cerr << "WARNING: Unknown Metric File Tag: " << tag.toAscii().constData() << std::endl;
      }
   }
   

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         for (int i = 0; i < numberOfNodes; i++) {
            int nodeNumDummy;
            stream >> nodeNumDummy;
            for (int j = 0; j < numberOfColumns; j++) {
               const int offset = getOffset(i, j);
               for (int k = 0; k < numberOfItemsPerColumn; k++) {
                  stream >> vectors[offset+k];
               }
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         {
            //
            // Needed for QT 4.2.2.
            //
            file.seek(stream.pos());
            
            for (int i = 0; i < numberOfNodes; i++) {
               for (int j = 0; j < numberOfColumns; j++) {
                  float xyz[3];
                  binStream >> xyz[0] >> xyz[1] >> xyz[2];
                  setVector(i, j, xyz);
               }
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Reading in XML format not supported.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }         
}

/**
 * Write the file's data (header has already been written).
 */
void 
SurfaceVectorFile::writeFileData(QTextStream& stream,
                           QDataStream& binStream,
                           QDomDocument& /*xmlDoc*/,
                           QDomElement& /*rootElement*/) throw (FileException)
{
   stream << tagFileVersion << " 0" << "\n";  //"metric-version 2\n";
   stream << tagNumberOfNodes << " " << numberOfNodes << "\n";
   stream << tagNumberOfColumns << " " << numberOfColumns << "\n";
   stream << tagFileTitle << " " << fileTitle << "\n";
   for (int j = 0; j < numberOfColumns; j++) {
      stream << tagColumnName << " " << j << " " << columnNames[j] << "\n";
   }
   for (int m = 0; m < numberOfColumns; m++) {
      stream << tagColumnComment << " " <<   m 
             << " " << StringUtilities::setupCommentForStorage(columnComments[m]) << "\n";
   }
   stream << tagBeginData << "\n";
   
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         for (int i = 0; i < numberOfNodes; i++) {
            stream << i;
            for (int j = 0; j < numberOfColumns; j++) {
               const float* xyz = getVector(i, j);
               stream << " " << xyz[0];
               stream << " " << xyz[1];
               stream << " " << xyz[2];
            }
            stream << "\n";
         }
         break;
      case FILE_FORMAT_BINARY:
#ifdef QT4_FILE_POS_BUG
         setBinaryFilePosQT4Bug();
#else  // QT4_FILE_POS_BUG
         //
         // still a bug in QT 4.2.2
         //
         setBinaryFilePosQT4Bug();
#endif // QT4_FILE_POS_BUG
         for (int i = 0; i < numberOfNodes; i++) {
            for (int j = 0; j < numberOfColumns; j++) {
               const float* xyz = getVector(i, j);
               binStream << xyz[0];
               binStream << xyz[1];
               binStream << xyz[2];
            }
         }
         break;
      case FILE_FORMAT_XML:
/*
         {
            for (int i = 0; i < numberOfNodes; i++) {
               for (int j = 0; j < numberOfColumns; j++) {
                  float x, y, z;
                  getVector(i, j, x, y, z);
                  QDomElement elem = xmlDoc.createElement("vector");
                  elem.setAttribute("node", i);
                  elem.setAttribute("column", j);
                  elem.setAttribute("x", x);
                  elem.setAttribute("y", y);
                  elem.setAttribute("z", z);
                  rootElement.appendChild(elem);
               }
            }
         }
*/
         throw FileException(filename, "Writing in XML format not supported.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }
}

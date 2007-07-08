
/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <iostream>

#include "GeodesicDistanceFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * Constructor.
 */
GeodesicDistanceFile::GeodesicDistanceFile() :
   NodeAttributeFile("geodesic distance file",
                     SpecFile::getGeodesicDistanceFileExtension(),
                     AbstractFile::FILE_FORMAT_ASCII,
                     FILE_IO_READ_AND_WRITE, 
                     FILE_IO_READ_AND_WRITE, 
                     FILE_IO_NONE, 
                     FILE_IO_NONE)
{
   clear();
}

/**
 * Destructor.
 */
GeodesicDistanceFile::~GeodesicDistanceFile()
{
   clear();
}

/**
 * append a node attribute  file to this one.
 */
void 
GeodesicDistanceFile::append(NodeAttributeFile& naf) throw (FileException)
{
   if (naf.getNumberOfColumns() > 0) {
      std::vector<int> destinationColumns(naf.getNumberOfColumns(), APPEND_COLUMN_NEW);
      append(naf, destinationColumns, FILE_COMMENT_MODE_APPEND);
   }
}

/**
 * append a node attribute file to this one but selectively load/overwrite columns
 * columnDestination is where naf's columns should be (-1=new, -2=do not load).
 */
void 
GeodesicDistanceFile::append(NodeAttributeFile& naf, 
                             std::vector<int> columnDestinationIn,
                             const FILE_COMMENT_MODE fcm) throw (FileException)
{
   bool setTheFileNameFlag = false;
   
   std::vector<int> columnDestination = columnDestinationIn;
   
   GeodesicDistanceFile& gdf = dynamic_cast<GeodesicDistanceFile&>(naf);
   
   if (numberOfNodes != gdf.numberOfNodes) {
      if (numberOfNodes > 0) {
         throw FileException("Trying to append geodesic distance file with a different number "
                          "of nodes");
      }
      else {
         setTheFileNameFlag = true;
      }
   }

   setModified();
   
   //
   // Find out how many columns need to be added
   //   
   int numColumnsToAdd = 0;
   int newColumnIndex = numberOfColumns;
   for (int i = 0; i < gdf.getNumberOfColumns(); i++) {
      if (columnDestination[i] == APPEND_COLUMN_NEW) {
         numColumnsToAdd++;
         columnDestination[i] = newColumnIndex;
         newColumnIndex++;
      }
   }
   //
   // Add on additional columns
   //
   if (getNumberOfNodes() == 0) {
      setNumberOfNodesAndColumns(gdf.getNumberOfNodes(), numColumnsToAdd);
   }
   else {
      addColumns(numColumnsToAdd);
   }
      
   //
   // copy column names from other file
   //
   for (int k = 0; k < gdf.numberOfColumns; k++) {
      if (columnDestination[k] >= 0) {
         const int col = columnDestination[k];
         setColumnName(col, gdf.getColumnName(k));
         setColumnComment(col, gdf.getColumnComment(k));
      }
   }
   
   //
   // copy geodesic data from other file
   //
   for (int j = 0; j < gdf.numberOfColumns; j++) {
      if (columnDestination[j] >= 0) {
         const int col = columnDestination[j];
         for (int i = 0; i < numberOfNodes; i++) {
            setNodeParent(i, col, gdf.getNodeParent(i, j));
            setNodeParentDistance(i, col, getNodeParentDistance(i, j));
         }
         setRootNode(col, gdf.getRootNode(j));
      }
   }
   
   if (setTheFileNameFlag) {
      setFileName(gdf.getFileName());
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(gdf, fcm);
}

/**
 * add columns to this node attribute file.
 */
void 
GeodesicDistanceFile::addColumns(const int numberOfNewColumns)
{
   const int oldNumberOfColumns = numberOfColumns;
   const int totalColumns = numberOfColumns + numberOfNewColumns;
   
   //
   // Save existing lat lon data
   //
   const std::vector<int>     parents = nodeParent;
   const std::vector<float> distances = nodeParentDistance;
   
   //
   // Setup file for new number of columns (will expand space for column naming)
   //
   setNumberOfNodesAndColumns(numberOfNodes, totalColumns);
   
   //
   // transfer existing data
   //
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < numberOfColumns; j++) {
         if (j < oldNumberOfColumns) {
            const int oldIndex = (oldNumberOfColumns * i) + j;
            setNodeParent(i, j, parents[oldIndex]);
            setNodeParentDistance(i, j, distances[oldIndex]);
         }
         else {
            setNodeParent(i, j, -1);
            setNodeParentDistance(i, j, 0.0);
         }
      }
   }
      
   setModified();
}

/**
 * add nodes to this file.
 */
void 
GeodesicDistanceFile::addNodes(const int numberOfNodesToAdd)
{
   setNumberOfNodesAndColumns(numberOfNodes + numberOfNodesToAdd, numberOfColumns);   
}

/**
 * Clears current file data in memory. 
 */
void 
GeodesicDistanceFile::clear()
{
   clearNodeAttributeFile();
   setNumberOfNodesAndColumns(0, 0);
}

/** 
 * deform "this" node attribute file placing the output in "deformedFile".
 */
void 
GeodesicDistanceFile::deformFile(const DeformationMapFile& /*dmf*/, 
                    NodeAttributeFile& /*deformedFile*/,
                    const DEFORM_TYPE /*dt*/) const throw (FileException)
{
   throw FileException("Deformation of Geodesic Distance File not supported.");
}

/**
 * set the number of nodes and columns in the file.
 */
void 
GeodesicDistanceFile::setNumberOfNodesAndColumns(const int numNodes, const int numCols)
{
   numberOfNodes = numNodes;
   numberOfColumns = numCols;
   
   const int num = numberOfNodes * numberOfColumns * numberOfItemsPerColumn;
   if (num <= 0) {
      nodeParent.clear();
      nodeParentDistance.clear();
      rootNode.clear();
   }
   else {
      nodeParent.resize(num);
      nodeParentDistance.resize(num);
      rootNode.resize(numberOfColumns, -1);
   }
   
   numberOfNodesColumnsChanged();
}

/**
 * reset a column of data.
 */
void 
GeodesicDistanceFile::resetColumn(const int columnNumber)
{
   for (int i = 0; i < numberOfNodes; i++) {
      setNodeParent(i, columnNumber, -1);
      setNodeParentDistance(i, columnNumber, 0.0);
   }
   rootNode[columnNumber] = -1;
   setModified();
}

/**
 * remove a column of data.
 */
void 
GeodesicDistanceFile::removeColumn(const int columnNumber)
{
   if (numberOfColumns <= 1) {
      clear();
      return;
   }
   
   //
   // Transfer lat/lon data
   //
   GeodesicDistanceFile gdf;
   gdf.setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);
   for (int i = 0; i < numberOfNodes; i++) {
      int ctr = 0;
      for (int j = 0; j < numberOfColumns; j++) {
         if (j != columnNumber) {
            gdf.setNodeParent(i, ctr, getNodeParent(i, j));
            gdf.setNodeParentDistance(i, ctr, getNodeParentDistance(i, j));
            ctr++;
         }
      }
   }
   int colCtr = 0;
   for (int j = 0; j < numberOfColumns; j++) {
      if (j != columnNumber) {
         gdf.setRootNode(colCtr, getRootNode(j));
         colCtr++;
      }
   }
   
   //
   // Transfer column information
   //
   int ctr = 0;
   for (int j = 0; j < numberOfColumns; j++) {
      if (j != columnNumber) {
         setColumnName(ctr, getColumnName(j));
         setColumnComment(ctr, getColumnComment(j));
         ctr++;
      }
   }
   
   setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);
   nodeParent = gdf.nodeParent;
   nodeParentDistance = gdf.nodeParentDistance;
   rootNode = gdf.rootNode;

   setModified();
}

/**
 * get the parent for a node.
 */
int 
GeodesicDistanceFile::getNodeParent(const int nodeNumber, const int columnNumber) const
{
   const int index = getOffset(nodeNumber, columnNumber);
   return nodeParent[index];
}

/** 
 * set the parent for a node.
 */
void 
GeodesicDistanceFile::setNodeParent(const int nodeNumber, const int columnNumber,
                                    const int parent)
{
   const int index = getOffset(nodeNumber, columnNumber);
   nodeParent[index] = parent;
}
                   
/**
 * get the distance to a parent for a node.
 */
float 
GeodesicDistanceFile::getNodeParentDistance(const int nodeNumber, const int columnNumber) const
{
   const int index = getOffset(nodeNumber, columnNumber);
   return nodeParentDistance[index];
}

/**
 * set the distance to a parent for a node.
 */
void 
GeodesicDistanceFile::setNodeParentDistance(const int nodeNumber, const int columnNumber,
                                            const float distance)
{
   const int index = getOffset(nodeNumber, columnNumber);
   nodeParentDistance[index] = distance;
}

/**
 * get the root node.
 */
int 
GeodesicDistanceFile::getRootNode(const int columnNumber) const
{
   return rootNode[columnNumber];
}

/**
 * set the root node.
 */
void 
GeodesicDistanceFile::setRootNode(const int columnNumber, const int node)
{
   rootNode[columnNumber] = node;
}
      
/**
 * Read the contents of the file (header has already been read).
 */
void 
GeodesicDistanceFile::readFileData(QFile& /*file*/, 
                          QTextStream& stream,
                          QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   int numNodes = -1;
   int numCols  = -1;
   int fileVersion = -1;
   
   bool readingTags = true;
   while(readingTags) {
      QString tag, tagValue;
      readTagLine(stream, tag, tagValue);
      
      if (tag == tagBeginData) {
         readingTags = false;
      }
      else if (tag == tagFileVersion) {
         fileVersion = tagValue.toInt();
      }
      else if (tag == tagNumberOfNodes) {
         numNodes = tagValue.toInt();
         if ((numNodes > 0) && (numCols > 0)) {
            setNumberOfNodesAndColumns(numNodes, numCols);
         }
      }
      else if (tag == tagNumberOfColumns) {
         numCols = tagValue.toInt();
         if ((numNodes > 0) && (numCols > 0)) {
            setNumberOfNodesAndColumns(numNodes, numCols);
         }
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
      else if (tag == tagFileTitle) {
         fileTitle = tagValue;
      }
      else {
         std::cout << "WARNING: Unknown Geodesic File Tag: " << tag.toAscii().constData() << std::endl;
      }
   }

   if (fileVersion != 1) {
      QString msg("Geodesic Distance File Version ");
      msg.append(StringUtilities::fromNumber(fileVersion));
      msg.append(" is not supported in this version of Caret5.  You may need a newer "
                 "version of Caret5.");
      throw FileException(filename, msg);
   }
   
   if (numNodes <= 0) {
      throw FileException(filename, "No data in Geodesic file");
   }
   
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            std::vector<QString> tokens;
            QString line;
            
            readLineIntoTokens(stream, line, tokens);
            if (static_cast<int>(tokens.size()) >= numberOfColumns) {
               for (int j = 0; j < numberOfColumns; j++) {
                  rootNode[j] = tokens[j].toInt();
               }
            }
            
            for (int i = 0; i < numberOfNodes; i++) {
               readLineIntoTokens(stream, line, tokens);
               if (static_cast<int>(tokens.size()) == (numberOfColumns * 2 + 1)) {
                  for (int j = 0; j < numberOfColumns; j++) {
                     const int index  = getOffset(i, j);
                     nodeParent[index]  = tokens[j * 2 + 1].toInt();
                     nodeParentDistance[index] = tokens[j * 2 + 2].toFloat();
                  }
               }
               else {
                  QString msg("Reading Geodesic Distance file line: ");
                  msg.append(line);
                  throw FileException(filename, msg);
               }
            }   
         }
         break;
      case FILE_FORMAT_BINARY:
         //
         // Needed for QT 4.2.2
         //
         binStream.device()->seek(stream.pos());
         for (int j = 0; j < numCols; j++) {
            binStream >> rootNode[j];
         }
         
         for (int i = 0; i < numNodes; i++) {
            for (int j = 0; j < numCols; j++) {
               int parent;
               float distance;
               binStream >> parent
                         >> distance;
               const int index  = getOffset(i, j);
               nodeParent[index]  = parent;
               nodeParentDistance[index] = distance;
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Reading in XML format not supported.");
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
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
GeodesicDistanceFile::writeFileData(QTextStream& stream,
                           QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   const int numNodes = getNumberOfNodes();
   const int numCols  = getNumberOfColumns();
   
   stream << tagFileVersion << " 1" << "\n";
   stream << tagNumberOfNodes << " " << numNodes << "\n";
   stream << tagNumberOfColumns << " " << numCols << "\n";
   stream << tagFileTitle << " " << fileTitle << "\n";
   for (int j = 0; j < numberOfColumns; j++) {
      stream << tagColumnName << " " << j << " " << columnNames[j] << "\n";
      stream << tagColumnComment << " " << j 
             << " " << StringUtilities::setupCommentForStorage(columnComments[j]) << "\n";
   }
   stream << tagBeginData << "\n";
   
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         for (int j = 0; j < numCols; j++) {
            if (j > 0) {
               stream << " ";
            }
            stream << rootNode[j];
         }
         stream << "\n";
         
         for (int i = 0; i < numNodes; i++) {
            stream << i;
            for (int j = 0; j < numCols; j++) {
               stream << " "
                     << getNodeParent(i, j) << " "
                     << getNodeParentDistance(i, j);
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
         for (int j = 0; j < numCols; j++) {
            binStream << rootNode[j];
         }
         
         for (int i = 0; i < numNodes; i++) {
            for (int j = 0; j < numCols; j++) {
               binStream << getNodeParent(i, j)
                         << getNodeParentDistance(i, j);
            }
         }
         break;
      case FILE_FORMAT_XML:
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

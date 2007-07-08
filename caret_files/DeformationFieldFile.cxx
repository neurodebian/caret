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
#include <sstream>

#define __DEFORMATION_FIELD_FILE_MAIN__
#include "DeformationFieldFile.h"
#undef __DEFORMATION_FIELD_FILE_MAIN__
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * Constructor.
 */
DeformationFieldFile::DeformationFieldFile()
   : NodeAttributeFile("Deformation Field File",
                       SpecFile::getDeformationFieldFileExtension(),
                       AbstractFile::FILE_FORMAT_ASCII,
                       FILE_IO_READ_AND_WRITE,
                       FILE_IO_NONE,  // no binary    true,
                       FILE_IO_NONE,
                       FILE_IO_NONE)
{
   clear();
}

/**
 * Destructor.
 */
DeformationFieldFile::~DeformationFieldFile()
{
   clear();
}

/**
 * Clears current file data in memory.
 */
void 
DeformationFieldFile::clear()
{
   clearNodeAttributeFile();
   setNumberOfNodesAndColumns(0, 0);
}
      
/**
 * append a node attribute  file to this one.
 */
void 
DeformationFieldFile::append(NodeAttributeFile& naf) throw (FileException)
{
   if (naf.getNumberOfColumns() > 0) {
      std::vector<int> columnDestination(naf.getNumberOfColumns(), APPEND_COLUMN_NEW);
      append(naf, columnDestination, FILE_COMMENT_MODE_APPEND);
   }
}

/**
 * append a node attribute file to this one but selectively load/overwrite columns
 * columnDestination is where naf's columns should be (-1=new, -2=do not load)
 */
void 
DeformationFieldFile::append(NodeAttributeFile& naf, 
                             std::vector<int> columnDestinationIn,
                             const FILE_COMMENT_MODE fcm) throw (FileException)
{
   bool setTheFileNameFlag = false;
   
   std::vector<int> columnDestination = columnDestinationIn;
   
   DeformationFieldFile& dff = dynamic_cast<DeformationFieldFile&>(naf);
   
   int numNodes = getNumberOfNodes();
   const int numCols  = getNumberOfColumns();
   
   const int appendNumCols = dff.getNumberOfColumns();
   const int appendNumNodes = dff.getNumberOfNodes();
   if (numNodes != appendNumNodes) {
      if (numNodes > 0) {
         throw FileException("Cannot append DeformationFieldFile, number of columns does not match.");
      }
      else {
         numNodes = appendNumNodes;
         setTheFileNameFlag = true;
      }
   }
   
   setModified();
   
   //
   // Find out how many columns need to be added
   //
   int numColumnsToAdd = 0;
   int newColumnIndex = numCols;
   for (int i = 0; i < dff.getNumberOfColumns(); i++) {
      if (columnDestination[i] == APPEND_COLUMN_NEW) {
         numColumnsToAdd++;
         columnDestination[i] = newColumnIndex;
         newColumnIndex++;
      }
   }
   
   //
   // Add additional columns to this file
   //
   addColumns(numColumnsToAdd);
   
   //
   // add new data
   //
   for (int n = 0; n < appendNumCols; n++) {
      if (columnDestination[n] >= 0) {
         //
         // Add column info
         //
         const int col = columnDestination[n];
         columnNames[col] = dff.columnNames[n];
         columnComments[col] = dff.columnComments[n];
         
         //
         // Add file names
         //
         deformedTopologyFileName[col] = dff.deformedTopologyFileName[n];
         preDeformedCoordinateFileName[col] = dff.preDeformedCoordinateFileName[n];
         deformedCoordinateFileName[col] = dff.deformedCoordinateFileName[n];
         coordinateFileName[col] = dff.coordinateFileName[n];
         topologyFileName[col] = dff.topologyFileName[n];
         
         //
         // Add deform node data
         //
         for (int i = 0; i < numNodes; i++) {
            const int index = getOffset(i, col);
            const int dffOffset = dff.getOffset(i, n);
            deformNodeInfo[index] = dff.deformNodeInfo[dffOffset];
         }
      }
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(dff, fcm);
}

/**
 * add columns to this node attribute file.
 */
void 
DeformationFieldFile::addColumns(const int numberOfNewColumns)
{
   int numNodes = getNumberOfNodes();
   const int numCols = getNumberOfColumns();
   
   setModified();
   
   const int newNumCols = numCols + numberOfNewColumns;
   
   DeformationFieldFile defField;
   defField.setNumberOfNodesAndColumns(numNodes, newNumCols);
   
   //
   // transfer column names
   //
   for (int j = 0; j < numCols; j++) {
      defField.columnNames[j]    = columnNames[j];
      defField.columnComments[j] = columnComments[j];
   }
   for (int n = 0; n < numberOfNewColumns; n++) {
      std::ostringstream str;
      str << "Column " << (numCols + n + 1) << " ";
      defField.columnNames[numCols + n] = str.str().c_str();
      defField.columnComments[numCols + n] = str.str().c_str();
   }
   
   //
   // Transfer the data
   //
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         const int index = defField.getOffset(i, j);
         const int otherIndex = getOffset(i, j);
         defField.deformNodeInfo[index] = deformNodeInfo[otherIndex];
      }
   }
   
   numberOfColumns = newNumCols;
   numberOfNodes   = defField.numberOfNodes;
   deformNodeInfo  = defField.deformNodeInfo;
   columnNames     = defField.columnNames;
   columnComments  = defField.columnComments;
}

/**
 * add nodes to this file.
 */
void 
DeformationFieldFile::addNodes(const int numberOfNodesToAdd)
{
   setNumberOfNodesAndColumns(numberOfNodes + numberOfNodesToAdd, numberOfColumns);
}

/** 
 *deform "this" node attribute file placing the output in "deformedFile".
 */
void 
DeformationFieldFile::deformFile(const DeformationMapFile& /*dmf*/, 
                    NodeAttributeFile& /*deformedFile*/,
                    const DEFORM_TYPE /*dt*/) const throw (FileException)
{
   throw FileException(
      "Deformation of DeformationFieldFile files not supported.");
}

/**
 * set the number of nodes and columns in the file.
 */
void 
DeformationFieldFile::setNumberOfNodesAndColumns(const int numNodes, const int numColumns)
{
   numberOfNodes = numNodes;
   numberOfColumns = numColumns;
   const int num = numberOfNodes * numberOfColumns * numberOfItemsPerColumn;
   
   if (num == 0) {
      deformNodeInfo.clear();
      deformedTopologyFileName.clear();
      preDeformedCoordinateFileName.clear();
      deformedCoordinateFileName.clear();
      coordinateFileName.clear();
      topologyFileName.clear();
   }
   else {
      deformNodeInfo.resize(num);
      deformedTopologyFileName.resize(num);
      preDeformedCoordinateFileName.resize(num);
      deformedCoordinateFileName.resize(num);
      coordinateFileName.resize(num);
      topologyFileName.resize(num);
   }
   numberOfNodesColumnsChanged();
   setModified();
}

/**
 * reset a column of data.
 */
void 
DeformationFieldFile::resetColumn(const int columnNumber)
{
   for (int i = 0; i < numberOfNodes; i++) {
      const int offset = getOffset(i, columnNumber);
      deformNodeInfo[offset].reset();
   }
   deformedTopologyFileName[columnNumber] = "";
   preDeformedCoordinateFileName[columnNumber] = "";
   deformedCoordinateFileName[columnNumber] = "";
   coordinateFileName[columnNumber] = "";
   topologyFileName[columnNumber] = "";
}

/**
 * remove a column of data.
 */
void 
DeformationFieldFile::removeColumn(const int columnNumber)
{
   if (numberOfColumns <= 1) {
      clear();
      return;
   }
   
   //
   // Make a copy of this file
   //
   DeformationFieldFile dff = *this;
   
   //
   // Set columns that should be saved
   //
   std::vector<int> columnsToAppend(numberOfColumns, APPEND_COLUMN_NEW);
   columnsToAppend[columnNumber] = APPEND_COLUMN_DO_NOT_LOAD;
   
   //
   // Clear this file
   //
   clear();
   
   //
   // Append copied file to this file minus the column being removed
   //
   append(dff, columnsToAppend, FILE_COMMENT_MODE_APPEND);
}
      
/**
 * get deformation field info for a node.
 */
DeformationFieldNodeInfo* 
DeformationFieldFile::getDeformationInfo(const int node, const int column)
{
   const int index = getOffset(node, column);
   if (index >= 0) {
      return &deformNodeInfo[index];
   }
   return NULL;
}

/**
 * get deformation field info for a node (const method).
 */
const DeformationFieldNodeInfo* 
DeformationFieldFile::getDeformationInfo(const int node, 
                                         const int column) const
{
   const int index = getOffset(node, column);
   if (index >= 0) {
      return &deformNodeInfo[index];
   }
   return NULL;
}
      
/**
 * get the name of the pre-deformed coordinate file.
 */
QString 
DeformationFieldFile::getPreDeformedCoordinateFileName(const int columnNumber) const
{ 
   return preDeformedCoordinateFileName[columnNumber]; 
}

/**
 * set the name of the pre-deformed coordinate file.
 */
void 
DeformationFieldFile::setPreDeformedCoordinateFileName(const int columnNumber,
                                                       const QString& name)
{ 
   preDeformedCoordinateFileName[columnNumber] = name; 
}

/**
 * get the name of the deformed coordinate file.
 */
QString 
DeformationFieldFile::getDeformedCoordinateFileName(const int columnNumber) const
{ 
   return deformedCoordinateFileName[columnNumber]; 
}

/**
 * set the name of the deformed coordinate file.
 */
void 
DeformationFieldFile::setDeformedCoordinateFileName(const int columnNumber,
                                                    const QString& name)
{ 
   deformedCoordinateFileName[columnNumber] = name; 
}

/**
 * get the name of the deformed topology file.
 */
QString 
DeformationFieldFile::getDeformedTopologyFileName(const int columnNumber) const 
{ 
   return deformedTopologyFileName[columnNumber]; 
}

/**
 * set the name of the deformed topology file.
 */
void 
DeformationFieldFile::setDeformedTopologyFileName(const int columnNumber,
                                                  const QString& name)
{
   deformedTopologyFileName[columnNumber] = name;
}

/**
 * get the name of the  topology file.
 */
QString 
DeformationFieldFile::getTopologyFileName(const int columnNumber) const 
{ 
   return topologyFileName[columnNumber]; 
}

/**
 * set the name of the  topology file.
 */
void 
DeformationFieldFile::setTopologyFileName(const int columnNumber,
                                          const QString& name)
{
   topologyFileName[columnNumber] = name;
}

/**
 * get the name of the  coordinate file.
 */
QString 
DeformationFieldFile::getCoordinateFileName(const int columnNumber) const
{ 
   return coordinateFileName[columnNumber]; 
}

/**
 * set the name of the  coordinate file.
 */
void 
DeformationFieldFile::setCoordinateFileName(const int columnNumber,
                                            const QString& name)
{ 
   coordinateFileName[columnNumber] = name; 
}

/**
 * Read the contents of the file (header has already been read)
 */
void 
DeformationFieldFile::readFileData(QFile& /*file*/, 
                                   QTextStream& stream,
                                   QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   int numNodes = -1;
   int numCols  = -1;
   
   int fileVersion = -1;

   bool readingTags = true;
   while (readingTags) {
      QString tag, tagValue;
      readTagLine(stream, tag, tagValue);
      if (tag == tagFileVersion) {
         fileVersion = StringUtilities::toInt(tagValue);
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
      else if ((tag == tagPreDeformedCoordinateFile) ||
               (tag == "tag-original-coord-file")) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         preDeformedCoordinateFileName[index] = name;
      }
      else if (tag == tagDeformedCoordinateFile) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         deformedCoordinateFileName[index] = name;
      }
      else if ((tag == tagDeformedTopologyFile) ||
               (tag == "tag-topology-file")) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         deformedTopologyFileName[index] = name;
      }
      else if (tag == tagTopologyFile) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         topologyFileName[index] = name;
      }
      else if (tag == tagCoordinateFile) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         coordinateFileName[index] = name;
      }
      else {
         std::cerr << "WARNING: Unknown DeformationFieldFile File Tag: " << tag.toAscii().constData() << std::endl;
      }
   }
   
   if (fileVersion != 1) {
      throw FileException(filename, "Only version 1 of DeformationFieldFile supported.");
   }

   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }
   QString line;
   std::vector<QString> tokens;
   
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         for (int i = 0; i < numberOfNodes; i++) {
            readLineIntoTokens(stream, line, tokens);
            
            if (static_cast<int>(tokens.size()) < (numberOfColumns + 1)) {
               throw FileException(filename, "invalid def field line");
            }
            
            const int offset = getOffset(i, 0);
            for (int j = 0; j < numberOfColumns; j++) {
               const int j6 = j * 6 + 1;
               int tileNodes[3];
               float tileAreas[3];
               tileNodes[0] = StringUtilities::toInt(tokens[j6]);
               tileNodes[1] = StringUtilities::toInt(tokens[j6+1]);
               tileNodes[2] = StringUtilities::toInt(tokens[j6+2]);
               tileAreas[0] = StringUtilities::toFloat(tokens[j6+3]);
               tileAreas[1] = StringUtilities::toFloat(tokens[j6+4]);
               tileAreas[2] = StringUtilities::toFloat(tokens[j6+5]);
               deformNodeInfo[offset+j].setData(tileNodes, tileAreas);
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         binStream.device()->seek(stream.pos());
         for (int i = 0; i < numberOfNodes; i++) {
            const int offset = getOffset(i, 0);
            for (int j = 0; j < numberOfColumns; j++) {
               int tileNodes[3];
               float tileAreas[3];
               binStream >> tileNodes[0] >> tileNodes[1] >> tileNodes[2];
               binStream >> tileAreas[0] >> tileAreas[1] >> tileAreas[2];
               deformNodeInfo[offset+j].setData(tileNodes, tileAreas);
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Writing in XML format not supported.");
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Writing Comma Separated Value File Format not supported.");
         break;
   }         
}

/**
 * Write the file's data (header has already been written).
 */
void 
DeformationFieldFile::writeFileData(QTextStream& stream,
                                    QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                    QDomElement& /* rootElement */) throw (FileException)
{
   stream << tagFileVersion << " " << 1 << "\n";
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
   for (int m = 0; m < numberOfColumns; m++) {
      stream << tagPreDeformedCoordinateFile << " "
             << m << " " << preDeformedCoordinateFileName[m] << "\n";
   }
   for (int m = 0; m < numberOfColumns; m++) {
      stream << tagDeformedCoordinateFile << " "
             << m << " " << deformedCoordinateFileName[m] << "\n";
   }
   for (int m = 0; m < numberOfColumns; m++) {
      stream << tagDeformedTopologyFile << " " 
             << m << " " << deformedTopologyFileName[m] << "\n";
   }
   for (int m = 0; m < numberOfColumns; m++) {
      stream << tagTopologyFile << " " 
             << m << " " << topologyFileName[m] << "\n";
   }
   for (int m = 0; m < numberOfColumns; m++) {
      stream << tagCoordinateFile << " "
             << m << " " << coordinateFileName[m] << "\n";
   }
   stream << tagBeginData << "\n";
   
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         for (int i = 0; i < numberOfNodes; i++) {
            stream << i;
            const int offset = getOffset(i, 0);
            for (int j = 0; j < numberOfColumns; j++) {
               int tileNodes[3];
               float tileAreas[3];
               deformNodeInfo[offset + j].getData(tileNodes, tileAreas);
               stream << " " << tileNodes[0]
                      << " " << tileNodes[1]
                      << " " << tileNodes[2]
                      << " " << tileAreas[0]
                      << " " << tileAreas[1]
                      << " " << tileAreas[2];
            }
            stream << "\n";
         }
         break;
      case FILE_FORMAT_BINARY:
         setBinaryFilePosQT4Bug();
         for (int i = 0; i < numberOfNodes; i++) {
            const int offset = getOffset(i, 0);
            for (int j = 0; j < numberOfColumns; j++) {
               int tileNodes[3];
               float tileAreas[3];
               deformNodeInfo[offset + j].getData(tileNodes, tileAreas);
               binStream << tileNodes[0]
                         << tileNodes[1]
                         << tileNodes[2]
                         << tileAreas[0]
                         << tileAreas[1]
                         << tileAreas[2];
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Writing in XML format not supported.");
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
         throw FileException(filename, "Writing Comma Separated Value File Format not supported.");
         break;
   }
}

//**************************************************************************************

/**
 * Constructor
 */
DeformationFieldNodeInfo::DeformationFieldNodeInfo()
{
   reset();
}

/**
 * destructor.
 */
DeformationFieldNodeInfo::~DeformationFieldNodeInfo()
{
}

/**
 * Reset the data.
 */
void
DeformationFieldNodeInfo::reset()
{
   tileNodes[0] = -1;
   tileNodes[1] = -1;
   tileNodes[2] = -1;
   tileBarycentricAreas[0] = 0.0;
   tileBarycentricAreas[1] = 0.0;
   tileBarycentricAreas[2] = 0.0;
}

/**
 * get the data.
 */
void 
DeformationFieldNodeInfo::getData(int tileNodesOut[3], 
                                   float tileBarycentricAreasOut[3]) const
{
   tileNodesOut[0] = tileNodes[0];
   tileNodesOut[1] = tileNodes[1];
   tileNodesOut[2] = tileNodes[2];
   tileBarycentricAreasOut[0] = tileBarycentricAreas[0];
   tileBarycentricAreasOut[1] = tileBarycentricAreas[1];
   tileBarycentricAreasOut[2] = tileBarycentricAreas[2];
}

/**
 * set the data.
 */
void 
DeformationFieldNodeInfo::setData(const int tileNodesIn[3], 
                                   const float tileBarycentricAreasIn[3])
{
   tileNodes[0] = tileNodesIn[0];
   tileNodes[1] = tileNodesIn[1];
   tileNodes[2] = tileNodesIn[2];
   tileBarycentricAreas[0] = tileBarycentricAreasIn[0];
   tileBarycentricAreas[1] = tileBarycentricAreasIn[1];
   tileBarycentricAreas[2] = tileBarycentricAreasIn[2];
}


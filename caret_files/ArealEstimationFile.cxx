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

#define __AREAL_ESTIMATION_FILE_DEFINE__
#include "ArealEstimationFile.h"
#undef __AREAL_ESTIMATION_FILE_DEFINE__ 

#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * The constructor.
 */
ArealEstimationNode::ArealEstimationNode()
{
   arealEstimationFile = NULL;
   reset();
}

/**
 * The destructor.
 */
ArealEstimationNode::~ArealEstimationNode()
{
}

/**
 * Reset the node.
 */
void
ArealEstimationNode::reset()
{
   for (int i = 0; i < 4; i++) {
      areaProbability[i] = 0.0;
      areaNamesIndex[i] = 0;
   }
}

/**
 * Get the data for the node.
 */
void
ArealEstimationNode::getData(int areaNamesIndexOut[4], 
                             float areaProbabilityOut[4]) const
{
   for (int i = 0; i < 4; i++) {
      areaNamesIndexOut[i]  = areaNamesIndex[i];
      areaProbabilityOut[i] = areaProbability[i];
   }
}

/**
 * Set the data for the node.
 */
void
ArealEstimationNode::setData(const int areaNamesIndexIn[4], 
                             const float  areaProbabilityIn[4])
{
   for (int i = 0; i < 4; i++) {
      areaNamesIndex[i]  = areaNamesIndexIn[i];
      areaProbability[i] = areaProbabilityIn[i];
   }
   if (arealEstimationFile != NULL) {
      arealEstimationFile->setModified();
   }
}

/**
 * The constructor.
 */
ArealEstimationFile::ArealEstimationFile()
   : NodeAttributeFile("Areal Estimation File", 
                       SpecFile::getArealEstimationFileExtension(),
                       AbstractFile::FILE_FORMAT_ASCII,
                       FILE_IO_READ_AND_WRITE, 
                       FILE_IO_READ_AND_WRITE, 
                       FILE_IO_NONE, 
                       FILE_IO_NONE)
{
   clear();
   clearModified();
}

/**
 * The destructor.
 */
ArealEstimationFile::~ArealEstimationFile()
{
   clear();
}

/**
 * Add an area name.
 */
int
ArealEstimationFile::addAreaName(const QString& name)
{
   for (unsigned int i = 0; i < areaNames.size(); i++) {
      if (name.compare(areaNames[i]) == 0) {
         return i;
      }
   }
   setModified();
   areaNames.push_back(name);
   return (areaNames.size() - 1);
}

/**
 * Clear the file.
 */
void
ArealEstimationFile::clear()
{
   clearNodeAttributeFile();
   areaNames.clear();
   setNumberOfNodesAndColumns(0, 0);

   addAreaName("???");
}

/**
 * append a node attribute file to this one but selectively load/overwrite columns
 * columnDestination is where naf's columns should be (-1=new, -2=do not load).
 */
void 
ArealEstimationFile::append(NodeAttributeFile& naf, 
                            std::vector<int> columnDestinationIn,
                            const FILE_COMMENT_MODE fcm) throw (FileException)
{
   bool setTheFileNameFlag = false;
   
   std::vector<int> columnDestination = columnDestinationIn;
   
   ArealEstimationFile& aef = dynamic_cast<ArealEstimationFile&>(naf);
   
   if (numberOfNodes != aef.numberOfNodes) {
      if (numberOfNodes > 0) {
         throw FileException("Trying to append areal estimtation file with a different number "
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
   for (int i = 0; i < aef.getNumberOfColumns(); i++) {
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
      setNumberOfNodesAndColumns(aef.getNumberOfNodes(), numColumnsToAdd);
   }
   else {
      addColumns(numColumnsToAdd);
   }
      
   //
   // copy column names from other file
   //
   for (int k = 0; k < aef.numberOfColumns; k++) {
      if (columnDestination[k] >= 0) {
         const int col = columnDestination[k];
         setColumnName(col, aef.getColumnName(k));
         setColumnComment(col, aef.getColumnComment(k));
         setLongName(col, aef.getLongName(k));
      }
   }
   
   //
   // Transfer area names
   //
   std::vector<int> areaNameIndex;
   for (int k = 0; k < aef.getNumberOfAreaNames(); k++) {
      areaNameIndex.push_back(addAreaName(aef.getAreaName(k)));
   }
   
   //
   // copy areal estimation data from other file
   //
   int areaNamesData[4];
   float probData[4];
   for (int j = 0; j < aef.numberOfColumns; j++) {
      if (columnDestination[j] >= 0) {
         const int col = columnDestination[j];
         for (int i = 0; i < numberOfNodes; i++) {
            aef.getNodeData(i, j, areaNamesData, probData);
            for (int m = 0; m < 4; m++) {
               areaNamesData[m] = areaNameIndex[areaNamesData[m]];
            }
            setNodeData(i, col, areaNamesData, probData);
         }
      }
   }
   
   if (setTheFileNameFlag) {
      setFileName(aef.getFileName());
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(aef, fcm);
}

/**
 * Append an areal estimation file to this one.
 */
void
ArealEstimationFile::append(NodeAttributeFile& naf) throw (FileException)
{
   ArealEstimationFile& aef = dynamic_cast<ArealEstimationFile&>(naf);
   if (getNumberOfNodes() != aef.getNumberOfNodes()) {
      throw FileException("Cannot append areal estimation, number of columns does not match.");
   }
      
   const int oldNumCols  = getNumberOfColumns();
   
   const int appendNumCols = aef.getNumberOfColumns();

   //
   // Add the additional columns to the file
   //   
   addColumns(appendNumCols);
   
   //
   // Transfer column attributes
   //
   for (int n = 0; n < appendNumCols; n++) {
      setColumnComment(oldNumCols + n, aef.getColumnComment(n));
      setColumnName(oldNumCols + n, aef.getColumnName(n));
      setLongName(oldNumCols + n, aef.getLongName(n));
   }
   
   //
   // Transfer area names
   //
   std::vector<int> areaNameIndex;
   for (int k = 0; k < aef.getNumberOfAreaNames(); k++) {
      areaNameIndex.push_back(addAreaName(aef.getAreaName(k)));
   }
   
   //
   // Transfer areal estimation data
   //
   int areaNamesData[4];
   float probData[4];
   for (int i = 0; i < numberOfNodes; i++) {
      for (int k = 0; k < appendNumCols; k++) {
         aef.getNodeData(i, k, areaNamesData, probData);
         for (int m = 0; m < 4; m++) {
            areaNamesData[m] = areaNameIndex[areaNamesData[m]];
         }
         setNodeData(i, oldNumCols + k, areaNamesData, probData);
      }
   }

   //
   // transfer the file's comment
   //
   appendFileComment(aef);

   setModified();
}

/**
 * Add column(s) to the file
 */
void
ArealEstimationFile::addColumns(const int numberOfNewColumns)
{
   const int oldNumberOfColumns = numberOfColumns;
   const int totalColumns = numberOfColumns + numberOfNewColumns;
   
   //
   // Keep track of existing areal estimation node data
   //
   const std::vector<ArealEstimationNode> oldData = nodeData;
   
   //
   // Setup file for new number of columns (will expand space for column naming)
   //
   setNumberOfNodesAndColumns(numberOfNodes, totalColumns);
   
   //
   // Transfer existing areal estimation node data
   //
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < numberOfColumns; j++) {
         int areaNameData[4] = { 0, 0, 0, 0 };
         float probData[4]   = { 0.0, 0.0, 0.0, 0.0 };
         
         if (j < oldNumberOfColumns) {
            const int oldIndex = (oldNumberOfColumns * i) + j;
            oldData[oldIndex].getData(areaNameData, probData);
         }
         setNodeData(i, j, areaNameData, probData);
      }
   }
   
   setModified();
}

/**
 * Add nodes to the file
 */
void 
ArealEstimationFile::addNodes(const int numberOfNodesToAdd)
{
   setNumberOfNodesAndColumns(numberOfNodes + numberOfNodesToAdd, numberOfColumns);
   setModified();
}

/**
 * Set the long name for a column.
 */
void
ArealEstimationFile::setLongName(const int columnNumber, const QString& name)
{
   longName[columnNumber] = name;
   setModified();
}

/**
 * Deform this areal estimation data into the deformed file passed in.
 * Returns true if an error occurs.
 */
void
ArealEstimationFile::deformFile(const DeformationMapFile& dmf, 
                            NodeAttributeFile& deformedFile,
                            const DEFORM_TYPE /*dt*/) const throw (FileException)
{
   ArealEstimationFile& deformedArealEstimationFile =
                                     dynamic_cast<ArealEstimationFile&>(deformedFile);
   
   const int numNodes = dmf.getNumberOfNodes();
   
   deformedArealEstimationFile.setNumberOfNodesAndColumns(numNodes, getNumberOfColumns());
   
   //
   // Transfer the area names
   //
   for (int j = 0; j < getNumberOfAreaNames(); j++) {
      deformedArealEstimationFile.addAreaName(getAreaName(j));
      
   }
   
   //
   // Transfer stuff in AbstractFile and NodeAttributeFile
   //
   transferFileDataForDeformation(dmf, deformedArealEstimationFile);

   for (int j = 0; j < getNumberOfColumns(); j++) {
      deformedArealEstimationFile.setLongName(j, getLongName(j));
   }
   
   //
   // Transfer the node probability data
   //
   int tileNodes[3];
   float tileAreas[3];
   int areaNameIndex[4];
   float areaProbability[4];
   
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < getNumberOfColumns(); j++) {
         dmf.getDeformDataForNode(i, tileNodes, tileAreas);
         if (tileNodes[0] > -1) {
            getNodeData(tileNodes[0], j, areaNameIndex, areaProbability);
         }
         else {
            for (int j = 0; j < 4; j++) {
               areaNameIndex[j] = 0;
               areaProbability[j] = 0.0;
            }
         }
         deformedArealEstimationFile.setNodeData(i, j, areaNameIndex, areaProbability);
      }
   }
}

/**
 * reset a column of data.
 */
void 
ArealEstimationFile::resetColumn(const int columnNumber)
{
   for (int i = 0; i < numberOfNodes; i++) {
      const int index = getOffset(i, columnNumber);
      nodeData[index].reset();
   }
   setModified();
}

/**
 * remove a column of data.
 */
void 
ArealEstimationFile::removeColumn(const int columnNumber)
{
   if (numberOfColumns <= 1) {
      clear();
      return;
   }

   ArealEstimationFile tempAEF;
   if (numberOfColumns > 1) {
      //
      // Transfer column attributes
      //
      int ctr = 0;
      for (int n = 0; n < numberOfColumns; n++) {
         if (n != columnNumber) {
            setColumnComment(ctr, getColumnComment(n));
            setColumnName(ctr, getColumnName(n));
            setLongName(ctr, getLongName(n));
            ctr++;
         }
      }
      
      //
      // Transfer areal estimation data
      //
      tempAEF.setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);
      
      int areaNamesData[4];
      float probData[4];
      for (int i = 0; i < numberOfNodes; i++) {
         int ctr = 0;
         for (int k = 0; k < numberOfColumns; k++) {
            if (k != columnNumber) {
               getNodeData(i, k, areaNamesData, probData);
               tempAEF.setNodeData(i, ctr, areaNamesData, probData);
               ctr++;
            }
         }
      }
   }
   setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);
   nodeData = tempAEF.nodeData;
   setModified();
}

/**
 * Get an area name.
 */
QString
ArealEstimationFile::getAreaName(const int index) const
{
   if ((index >= 0) && (index <= static_cast<int>(areaNames.size()))) {
      return areaNames[index];
   }
   return "???";
}

/**
 * Get data for the specified node and column.
 */
void 
ArealEstimationFile::getNodeData(const int nodeNumber,
                                 const int columnNumber,
                                 QString areaNamesOut[4], 
                                 float areaProbabilityOut[4]) const
{
   const int index = getOffset(nodeNumber, columnNumber);
   
   if (index >= 0) {
      int areaNamesIndexOut[4];
      nodeData[index].getData(areaNamesIndexOut, areaProbabilityOut);
      for (int i = 0; i < 4; i++) {
         areaNamesOut[i] = getAreaName(areaNamesIndexOut[i]);
      }
   }
   else {
      for (int i = 0; i < 4; i++) {
         areaNamesOut[i]       = "???";
         areaProbabilityOut[i] = 0.0;
      }
   }
}

/**
 * Get data for the specified node and column.
 */
void 
ArealEstimationFile::getNodeData(const int nodeNumber,
                                 const int columnNumber,
                                 int areaNamesIndexOut[4], 
                                 float areaProbabilityOut[4]) const
{
   const int index = getOffset(nodeNumber, columnNumber);
   
   if (index >= 0) {
      nodeData[index].getData(areaNamesIndexOut, areaProbabilityOut);
   }
   else {
      for (int i = 0; i < 4; i++) {
         areaNamesIndexOut[i]  = 0;
         areaProbabilityOut[i] = 0.0;
      }
   }
}

/**
 * Set the data for the node and column.
 */
void 
ArealEstimationFile::setNodeData(const int nodeNumber,
                                 const int columnNumber,
                                 const QString areaNamesIn[4], 
                                 const float areaProbabilityIn[4])
{
   const int index = getOffset(nodeNumber, columnNumber);
   
   if (index >= 0) {
      int areaNamesIndexIn[4];
      for (int i = 0; i < 4; i++) {
         areaNamesIndexIn[i] = addAreaName(areaNamesIn[i]);
      }
      nodeData[index].setData(areaNamesIndexIn, areaProbabilityIn);
      setModified();
   }
}

/**
 * Set the data for the node and column.
 */
void 
ArealEstimationFile::setNodeData(const int nodeNumber,
                                 const int columnNumber,
                                 const int areaNamesIndexIn[4], 
                                 const float areaProbabilityIn[4])
{
   const int index = getOffset(nodeNumber, columnNumber);
   
   if (index >= 0) {
      nodeData[index].setData(areaNamesIndexIn, areaProbabilityIn);
      setModified();
   }
}

/**
 * Set the number of nodes and columnsfor this file.
 */
void 
ArealEstimationFile::setNumberOfNodesAndColumns(const int numNodes, const int numColumns)
{
   if ((numNodes == 0) || (numColumns == 0)) {
      longName.clear();
      nodeData.clear();
   }
   else {
      longName.resize(numColumns);   
      nodeData.resize(numNodes * numColumns);
   }
      
   numberOfNodes = numNodes;
   numberOfColumns = numColumns;
   numberOfNodesColumnsChanged();
   setModified();
   
   for (int i = 0; i < (numNodes * numColumns); i++) {
      nodeData[i].arealEstimationFile = this;
   }
}

/**
 * Read version 1 of the file.
 */
void
ArealEstimationFile::readFileDataVersion1(QTextStream& stream) throw (FileException)
{
   QString line;
   readLine(stream, line);
   
   const int numAreaNames = line.toInt();
   if (numAreaNames < 1) {
      throw FileException(filename, "No area names in file");
   }
   
   areaNames.reserve(numAreaNames);
   
   for (int i = 0; i < numAreaNames; i++) {
      QString indexStr, name;
      readTagLine(stream, indexStr, name);
      if (name.isEmpty()) {
         throw FileException(filename, "reading line with area name");
      }
      addAreaName(name);
   }
   
   readLine(stream, line);
   const int numNodes = line.toInt();
   if (numNodes < 1) {
      throw FileException(filename, "Reading line with number of nodes");
   }
   
   setNumberOfNodesAndColumns(numNodes, 1);
   
   setColumnComment(0, getFileComment());
   longName[0] = version1LongName;
   setColumnName(0,version1ShortName);
   
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   for (int j = 0; j < numNodes; j++) {
      readLine(stream, line);
      std::vector<QString> tokens;
      StringUtilities::token(line, " ", tokens);
      if (tokens.size() != 9) {
         throw FileException(filename, "Reading a line of data");
      }
      int n[4];
      float p[4];
      n[0] = tokens[1].toInt();
      p[0] = tokens[2].toFloat();
      n[1] = tokens[3].toInt();
      p[1] = tokens[4].toFloat();
      n[2] = tokens[5].toInt();
      p[2] = tokens[6].toFloat();
      n[3] = tokens[7].toInt();
      p[3] = tokens[8].toFloat();
      
      setNodeData(j, 0, n, p);
   }
}

/**
 * Read version 2 of the file.
 */
void
ArealEstimationFile::readFileDataVersion2(QFile& file, QTextStream& stream, QDataStream& binStream) throw (FileException)
{
   QString line;
   readLine(stream, line);
   
   const int numAreaNames = line.toInt();
   if (numAreaNames < 1) {
      throw FileException(filename, "No area names in file");
   }
   
   areaNames.reserve(numAreaNames);
   
#ifdef QT4_FILE_POS_BUG
   QString lastLineRead;
#endif // QT4_FILE_POS_BUG
   for (int i = 0; i < numAreaNames; i++) {
      QString indexStr, line, name;
      readTagLine(stream, line, indexStr, name);
      if (name.isEmpty()) {
         throw FileException(filename, "reading line with area name");
      }
      addAreaName(name);
      
#ifdef QT4_FILE_POS_BUG
      lastLineRead = line;
#endif // QT4_FILE_POS_BUG
   }
   
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

#ifdef QT4_FILE_POS_BUG
   if (getFileReadType() == FILE_FORMAT_BINARY) {
      if (lastLineRead.isEmpty() == false) {
         qint64 offset = findBinaryDataOffsetQT4Bug(file, lastLineRead.toAscii().constData());
         if (offset > 0) {
            offset++;
            file.seek(offset);
         }
      }
   }
#endif // QT4_FILE_POS_BUG

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         for (int j = 0; j < numberOfNodes; j++) {
            readLine(stream, line);
            std::vector<QString> tokens;
            StringUtilities::token(line, " ", tokens);
            if (static_cast<int>(tokens.size()) != (numberOfColumns * 8 + 1)) {
               QString msg("Reading a line of data");
               msg.append(line);
               //std::cout << "Number of tokens is " << tokens.size() 
               //          << " numberOfColumns is " << numberOfColumns << std::endl;
               throw FileException(filename, msg);
            }
            for (int k = 0; k < numberOfColumns; k++) {
               int n[4];
               float p[4];
               n[0] = tokens[k * 8 + 1].toInt();
               p[0] = tokens[k * 8 + 2].toFloat();
               n[1] = tokens[k * 8 + 3].toInt();
               p[1] = tokens[k * 8 + 4].toFloat();
               n[2] = tokens[k * 8 + 5].toInt();
               p[2] = tokens[k * 8 + 6].toFloat();
               n[3] = tokens[k * 8 + 7].toInt();
               p[3] = tokens[k * 8 + 8].toFloat();
            
               setNodeData(j, k, n, p);
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         //
         // Needed for QT 4.2.2
         //
         file.seek(stream.pos());
         for (int j = 0; j < numberOfNodes; j++) {
            for (int k = 0; k < numberOfColumns; k++) {
               int namesIndex[4];
               float prob[4];
               binStream >> namesIndex[0] >> prob[0]
                         >> namesIndex[1] >> prob[1]
                         >> namesIndex[2] >> prob[2]
                         >> namesIndex[3] >> prob[3];
               setNodeData(j, k, namesIndex, prob);
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Reading in XML format not supported.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Reading XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Reading Comma Separated Value File Format not supported.");
         break;
   }
}

/**
 * Read the file's tags.
 */
void
ArealEstimationFile::readTags(QTextStream& stream, const int fileVersion) throw (FileException)
{
   int numNodes = 0;
   int numCols  = 0;
   
   bool readingTags = true;
   while(readingTags) {
      QString tag, tagValue;
      readTagLine(stream, tag, tagValue);
      
      if (tag == tagBeginData) {
         readingTags = false;
      }
      else if (tag == tagNumberOfNodes) {
         numNodes = tagValue.toInt();
         if ((numCols > 0) && (numNodes > 0)) {
            setNumberOfNodesAndColumns(numNodes, numCols);
         }
      }
      else if (tag == tagNumberOfColumns) {
         numCols  = tagValue.toInt();
         if ((numCols > 0) && (numNodes > 0)) {
            setNumberOfNodesAndColumns(numNodes, numCols);
         }
      }
      else if ((tag == tagColumnName) ||
               (tag == "tag-short-name")) {
         if (fileVersion == 1) {
            version1ShortName = tagValue;
         }
         else {
            QString name;
            const int index = splitTagIntoColumnAndValue(tagValue, name);
            setColumnName(index, name);
         }
      }
      else if (tag == tagLongName) {
         if (fileVersion == 1) {
            version1LongName = tagValue;
         }
         else {
            QString name;
            const int index = splitTagIntoColumnAndValue(tagValue, name);
            longName[index] = name;
         }
      }
      else if (tag == tagColumnComment) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         columnComments[index] = StringUtilities::setupCommentForDisplay(name);
      }
      else if (tag == "tag-comment") {
         setFileComment(tagValue);
      }
      else if (tag == tagFileTitle) {
         setFileTitle(tagValue);
      }
      else if (tag == tagColumnStudyMetaData) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         studyMetaDataLinkSet[index].setLinkSetFromCodedText(name);
      }
      else {
         std::cerr << "WARNING: Unknown Areal Estimation File Tag: " << tag.toAscii().constData() << std::endl;
      }
   }
}
    
/**
 * Read the areal estimation file.
 */
void
ArealEstimationFile::readFileData(QFile& file, QTextStream& stream,
                                  QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   int fileVersion = 0;
   QString versionTag;
   QString versionNumStr;
   readTagLine(stream, versionTag, versionNumStr);
   if ((versionTag != tagFileVersion) && (versionTag != "tag-file-version")) {
      throw FileException(filename, "Unable to find line with version tag");
   }
   if (versionNumStr.isEmpty()) {
      throw FileException(filename, "Reading line containing file version number");
   }
   fileVersion = versionNumStr.toInt();   
   
   readTags(stream, fileVersion);
   
   switch(fileVersion) {
      case 2:
         readFileDataVersion2(file, stream, binStream);
         break;         
      case 1:
         readFileDataVersion1(stream);
         break;
      default:
         throw FileException(filename, "Unsupported version of areal estimation file");
         break;
   }
}

/**
 * Write the areal estimation file.
 */
void
ArealEstimationFile::writeFileData(QTextStream& stream, 
                                   QDataStream& binStream,
                                   QDomDocument& /* xmlDoc */,
                                   QDomElement& /* rootElement */) throw (FileException)
{
   stream << tagFileVersion << " 2\n";

   stream << tagNumberOfNodes << " " << numberOfNodes << "\n";
   stream << tagNumberOfColumns << " " << numberOfColumns << "\n";
   stream << tagFileTitle << " " << fileTitle << "\n";
   
   for (int k = 0; k < numberOfColumns; k++) {
      stream << tagColumnComment << " " << k 
             << " " << StringUtilities::setupCommentForStorage(columnComments[k]) << "\n";
      stream << tagLongName << " "  << k << " " << longName[k] << "\n";
      stream << tagColumnName << " "  << k << " "<< columnNames[k] << "\n";
      stream << tagColumnStudyMetaData << " " << k << " " << studyMetaDataLinkSet[k].getLinkSetAsCodedText() << "\n";
   }
      
   stream << tagBeginData << "\n";
   
   stream << static_cast<int>(areaNames.size()) << "\n";
   for (unsigned int i = 0; i < areaNames.size(); i++) {
      stream << static_cast<int>(i)  << " " << areaNames[i] << "\n";
   }
   
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         for (int j = 0; j < numberOfNodes; j++) {
            stream << j;
            for (int k = 0; k < numberOfColumns; k++) {
               int namesIndex[4];
               float prob[4];
               getNodeData(j, k, namesIndex, prob);
               
               stream << " " << namesIndex[0] << " " << prob[0] << " "
                             << namesIndex[1] << " " << prob[1] << " "
                             << namesIndex[2] << " " <<  prob[2] << " " 
                             << namesIndex[3] << " " <<  prob[3]  << " ";
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
         for (int j = 0; j < numberOfNodes; j++) {
            for (int k = 0; k < numberOfColumns; k++) {
               int namesIndex[4];
               float prob[4];
               getNodeData(j, k, namesIndex, prob);
               
               binStream << namesIndex[0] << prob[0]
                         << namesIndex[1] << prob[1]
                         << namesIndex[2] << prob[2]
                         << namesIndex[3] << prob[3];
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Writing in XML format not supported.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Writing XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Writing XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Writing Comma Separated Value File Format not supported.");
         break;
   }
}


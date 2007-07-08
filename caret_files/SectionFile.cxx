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

#include <algorithm>
#include <iostream>
#include <limits>

#define _SECTION_FILE_MAIN_
#include "SectionFile.h"
#undef _SECTION_FILE_MAIN_
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * Constructor.
 */
SectionFile::SectionFile()
   : NodeAttributeFile("Section File", SpecFile::getSectionFileExtension(),
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
SectionFile::~SectionFile()
{
   clear();
}

/**
 * append a node attribute file to this one but selectively load/overwrite columns
 * columnDestination is where naf's columns should be (-1=new, -2=do not load).
 */
void 
SectionFile::append(NodeAttributeFile& /*naf*/, 
                    std::vector<int> /*columnDestination*/,
                    const FILE_COMMENT_MODE /*fcm*/) throw (FileException)
{
   throw FileException("Not implemented yet.");
}

/**
 * append an section file to this one
 */
void 
SectionFile::append(NodeAttributeFile& naf) throw (FileException)
{
   SectionFile& sf = dynamic_cast<SectionFile&>(naf);
   
   if (getNumberOfNodes() != sf.getNumberOfNodes()) {
      throw FileException("Cannot append Section, number of columns does not match.");
   }
      
   const int oldNumCols  = getNumberOfColumns();
   
   const int appendNumCols = sf.getNumberOfColumns();

   //
   // Add the additional columns to the file
   //   
   addColumns(appendNumCols);
   
   //
   // Transfer section data
   //
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < appendNumCols; j++) {
         setSection(i, oldNumCols + j, sf.getSection(i, j));
      }
   }
   
   //
   // Transfer column information
   // Note: minimumSection and maximum section are set by "setSection()"
   //
   for (int j = 0; j < appendNumCols; j++) {
      setColumnName(oldNumCols + j, sf.getColumnName(j));
      setColumnComment(oldNumCols + j, sf.getColumnComment(j));
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(sf);
}

/**      
 * add columns to this section file
 */
void 
SectionFile::addColumns(const int numberOfNewColumns)
{
   const int oldNumberOfColumns = numberOfColumns;
   const int totalColumns = numberOfColumns + numberOfNewColumns;
   
   //
   // Save existing section data
   //
   const std::vector<int> sect = sections;
   
   //
   // Setup file for new number of columns (will expand space for column naming)
   //
   setNumberOfNodesAndColumns(numberOfNodes, totalColumns);
   
   //
   // transfer existing section data
   //
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < numberOfColumns; j++) {
         if (j < oldNumberOfColumns) {
            const int oldIndex = (oldNumberOfColumns * i) + j;
            setSection(i, j, sect[oldIndex]);
         }
         else {
            setSection(i, j, 0);
         }
      }
   }
      
   setModified();
}

/**
 * Add node to the file.
 */
void 
SectionFile::addNodes(const int numberOfNodesToAdd)
{
   setNumberOfNodesAndColumns(numberOfNodes + numberOfNodesToAdd, numberOfColumns);
}
      
/**
 * Clear the file.
 */
void SectionFile::clear()
{
   clearNodeAttributeFile();
   setNumberOfNodesAndColumns(0, 0);
}


/**
 * returns true if an error occurs.
 */
void 
SectionFile::deformFile(const DeformationMapFile& /*dmf*/,
                    NodeAttributeFile& /*deformedFile*/,
                    const DEFORM_TYPE /*dt*/) const throw (FileException)
{
   throw FileException("Deformation of Section files not supported.");
}

/**
 * reset a column of data.
 */
void 
SectionFile::resetColumn(const int columnNumber)
{
   setColumnName(columnNumber, "");
   setColumnComment(columnNumber, "");
   
   for (int i = 0; i < numberOfNodes; i++) {
      setSection(i, columnNumber, 0);
   }
   setModified();
}

/**
 * remove a column of data.
 */
void 
SectionFile::removeColumn(const int columnNumber)
{
   if (numberOfColumns <= 1) {
      clear();
      return;
   }
   
   int ctr = 0;
   for (int i = 0; i < numberOfColumns; i++) {
      if (i != columnNumber) {
         setColumnName(ctr, getColumnName(i));
         setColumnComment(ctr, getColumnComment(i));
         ctr++;
      }
   }
   
   SectionFile sf;
   sf.setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);
   for (int i = 0; i < numberOfNodes; i++) {
      int ctr = 0;
      for (int j = 0; j < numberOfColumns; j++) {
         if (j != columnNumber) {
            setSection(i, ctr, getSection(i, j));
            ctr++;
         }
      }
   }
   
   setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);
   sections = sf.sections;
   setModified();
}

/**
 * Set number of nodes in the file.
 */
void 
SectionFile::setNumberOfNodesAndColumns(const int numNodes, const int numCols)
{
   //const int oldNumberOfColumns = numberOfColumns;
   
   numberOfNodes = numNodes;
   numberOfColumns = numCols;
   
   const int num = numberOfNodes * numberOfColumns * numberOfItemsPerColumn;
   if (num <= 0) {
      sections.clear();
      minimumSection.clear();
      maximumSection.clear();
   }
   else {
      sections.resize(num, 0);
      minimumSection.resize(numberOfColumns, std::numeric_limits<int>::max());
      maximumSection.resize(numberOfColumns, std::numeric_limits<int>::min());
   }
   numberOfNodesColumnsChanged();
}

/**
  * get the minimum section
*/
int 
SectionFile::getMinimumSection(const int col) const
{
   return minimumSection[col];
}

/**
 * get the maximum section
 */
int 
SectionFile::getMaximumSection(const int col) const
{
   return maximumSection[col];
}

/**
 * Get the section for a node.
 */
int 
SectionFile::getSection(const int nodeNumber, const int columnNumber) const
{
   const int index = getOffset(nodeNumber, columnNumber);
   return sections[index];
}

/**
 * Set the section for a node.
 */
void 
SectionFile::setSection(const int nodeNumber, const int columnNumber, const int sectionNum)
{
   const int index = getOffset(nodeNumber, columnNumber);
   sections[index] = sectionNum;
   maximumSection[columnNumber] = std::max(maximumSection[columnNumber], sectionNum);
   minimumSection[columnNumber] = std::min(minimumSection[columnNumber], sectionNum);
   setModified();
}

/**
 * Set minimum and maximum sections and selected
 */
void
SectionFile::postColumnCreation(const int columnNumber)
{
   minimumSection[columnNumber] = std::numeric_limits<int>::max();
   maximumSection[columnNumber] = std::numeric_limits<int>::min();
   for (int i = 0; i < numberOfNodes; i++) {
      minimumSection[columnNumber] = std::min(minimumSection[columnNumber], getSection(i, columnNumber));
      maximumSection[columnNumber] = std::max(maximumSection[columnNumber], getSection(i, columnNumber));
   }
}

/**
 * Read the section file's version 1 data.
 */
void 
SectionFile::readFileDataVersion0(QTextStream& stream,
                                  QDataStream& binStream) throw (FileException)
{
   int numNodes = -1;
   int numCols  = -1;

   bool readingTags = true;
   while(readingTags) {
      QString tag, tagValue;
      readTagLine(stream, tag, tagValue);
      
      if (tag == tagBeginData) {
         readingTags = false;
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
         std::cerr << "WARNING: Unknown Section File Tag: " << tag.toAscii().constData() << std::endl;
      }
   }

   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   if (numNodes <= 0) {
      throw FileException(filename, "No data in Section file");
   }
   
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            std::vector<QString> tokens;
            QString line;
            for (int i = 0; i < numberOfNodes; i++) {
               readLineIntoTokens(stream, line, tokens);
               if (static_cast<int>(tokens.size()) == (numberOfColumns + 1)) {
                  for (int j = 0; j < numberOfColumns; j++) {
                     const int sectionNum = tokens[j + 1].toInt();
                     setSection(i, j, sectionNum);
                  }
               }
               else {
                  QString msg("Reading Section file line: ");
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
         int section;
         for (int i = 0; i < numNodes; i++) {
            for (int j = 0; j < numCols; j++) {
               binStream >> section;
               setSection(i, j, section);
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

/**
 * Read the section file's data.
 */
void 
SectionFile::readFileData(QFile& /*file*/, QTextStream& stream,
                          QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   QString line;
   QString versionStr, versionNumberStr;
   readTagLine(stream, versionStr, versionNumberStr);
      
   int fileVersion = 0;

   if (versionStr == tagFileVersion) {
      fileVersion = versionNumberStr.toInt();
   }
   
   switch(fileVersion) {
      case 0:
         readFileDataVersion0(stream, binStream);
         break;
      default:
         throw FileException(filename, "Invalid Section file version");
         break;
   }   
   
   for (int i = 0; i < numberOfColumns; i++) {
      postColumnCreation(i);
   }
}

/**
 * Write the section file's data.
 */
void 
SectionFile::writeFileData(QTextStream& stream, 
                           QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   const int numNodes = getNumberOfNodes();
   const int numCols  = getNumberOfColumns();
   
   stream << tagFileVersion << " 0" << "\n";
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
         for (int i = 0; i < numNodes; i++) {
            stream << i;
            for (int j = 0; j < numCols; j++) {
               stream << " " << getSection(i, j);
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
         for (int i = 0; i < numNodes; i++) {
            for (int j = 0; j < numCols; j++) {
               binStream << getSection(i, j);
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

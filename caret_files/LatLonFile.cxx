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

#include "DeformationMapFile.h"
#include "FileUtilities.h"
#define _LAT_LON_FILE_MAIN_
#include "LatLonFile.h"
#undef _LAT_LON_FILE_MAIN_
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * Constructor.
 */
LatLonFile::LatLonFile()
   : NodeAttributeFile("Lat Lon File", 
                       SpecFile::getLatLonFileExtension(),
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
LatLonFile::~LatLonFile()
{
   clear();
}
/**
 * append a node attribute file to this one but selectively load/overwrite columns
 * columnDestination is where naf's columns should be (-1=new, -2=do not load).
 */
void 
LatLonFile::append(NodeAttributeFile& naf, 
                     std::vector<int> columnDestinationIn,
                     const FILE_COMMENT_MODE fcm) throw (FileException)
{
   bool setTheFileNameFlag = false;
   
   std::vector<int> columnDestination = columnDestinationIn;
   
   LatLonFile& llf = dynamic_cast<LatLonFile&>(naf);
   
   if (numberOfNodes != llf.numberOfNodes) {
      if (numberOfNodes > 0) {
         throw FileException("Trying to append lat lon file with a different number "
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
   for (int i = 0; i < llf.getNumberOfColumns(); i++) {
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
      setNumberOfNodesAndColumns(llf.getNumberOfNodes(), numColumnsToAdd);
   }
   else {
      addColumns(numColumnsToAdd);
   }
      
   //
   // copy paint column names from other file
   //
   for (int k = 0; k < llf.numberOfColumns; k++) {
      if (columnDestination[k] >= 0) {
         const int col = columnDestination[k];
         setColumnName(col, llf.getColumnName(k));
         setColumnComment(col, llf.getColumnComment(k));
      }
   }
   
   //
   // copy lat/lon data from other file
   //
   for (int j = 0; j < llf.numberOfColumns; j++) {
      if (columnDestination[j] >= 0) {
         const int col = columnDestination[j];
         for (int i = 0; i < numberOfNodes; i++) {
            float lat, lon;
            llf.getLatLon(i, j, lat, lon);
            setLatLon(i, col, lat, lon);
            llf.getDeformedLatLon(i, j, lat, lon);
            setDeformedLatLon(i, col, lat, lon);
         }
      }
   }
   
   if (setTheFileNameFlag) {
      setFileName(llf.getFileName());
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(llf, fcm);
}


/**
 * append an lat/lon file to this one
 */
void 
LatLonFile::append(NodeAttributeFile& naf) throw (FileException)
{
   LatLonFile& llf = dynamic_cast<LatLonFile&>(naf);
   
   if (getNumberOfNodes() != llf.getNumberOfNodes()) {
      throw FileException("Cannot append LatLon, number of columns does not match.");
   }
      
   const int oldNumCols  = getNumberOfColumns();
   
   const int appendNumCols = llf.getNumberOfColumns();

   //
   // Add the additional columns to the file
   //   
   addColumns(appendNumCols);
   
   //
   // Transfer lat/lon data
   //
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < appendNumCols; j++) {
         float lat, lon;
         llf.getLatLon(i, j, lat, lon);
         setLatLon(i, oldNumCols + j, lat, lon);
         llf.getDeformedLatLon(i, j, lat, lon);
         setDeformedLatLon(i, oldNumCols + j, lat, lon);
      }
   }
   
   //
   // Transfer column information
   //
   for (int j = 0; j < appendNumCols; j++) {
      setColumnName(oldNumCols + j, llf.getColumnName(j));
      setColumnComment(oldNumCols + j, llf.getColumnComment(j));
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(llf);
}

/**      
 * add columns to this lat/lon file
 */
void 
LatLonFile::addColumns(const int numberOfNewColumns)
{
   const int oldNumberOfColumns = numberOfColumns;
   const int totalColumns = numberOfColumns + numberOfNewColumns;
   
   //
   // Save existing lat lon data
   //
   const std::vector<float> lat = latitude;
   const std::vector<float> lon = longitude;
   const std::vector<float> deflat = deformedLatitude;
   const std::vector<float> deflon = deformedLongitude;
   const std::vector<bool>  defValid = deformedLatLonValid;
   
   //
   // Setup file for new number of columns (will expand space for column naming)
   //
   setNumberOfNodesAndColumns(numberOfNodes, totalColumns);
   
   //
   // transfer existing lat lon data
   //
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < numberOfColumns; j++) {
         if (j < oldNumberOfColumns) {
            const int oldIndex = (oldNumberOfColumns * i) + j;
            setLatLon(i, j, lat[oldIndex], lon[oldIndex]);
            setDeformedLatLon(i, j, deflat[oldIndex], deflon[oldIndex]);
         }
         else {
            setLatLon(i, j, 0.0, 0.0);
            setDeformedLatLon(i, j, 0.0, 0.0);
         }
      }
   }
   for (int j = 0; j < numberOfColumns; j++) {
      if (j < oldNumberOfColumns) {
         deformedLatLonValid[j] = defValid[j];
      }
      else {
         deformedLatLonValid[j] = false;
      }
   }
      
   setModified();
}

/**
 * Add node to the file.
 */
void 
LatLonFile::addNodes(const int numberOfNodesToAdd)
{
   setNumberOfNodesAndColumns(numberOfNodes + numberOfNodesToAdd, numberOfColumns);
}
      
/**
 * Clear the file.
 */
void LatLonFile::clear()
{
   clearNodeAttributeFile();
   setNumberOfNodesAndColumns(0, 0);
}

/**
 * Set number of nodes in the file.
 */
void 
LatLonFile::setNumberOfNodesAndColumns(const int numNodes, const int numCols)
{
   const int oldNumberOfColumns = numberOfColumns;
   
   numberOfNodes = numNodes;
   numberOfColumns = numCols;
   
   const int num = numberOfNodes * numberOfColumns * numberOfItemsPerColumn;
   if (num <= 0) {
      latitude.clear();
      longitude.clear();
      deformedLatitude.clear();
      deformedLongitude.clear();
      deformedLatLonValid.clear();
   }
   else {
      latitude.resize(num);
      longitude.resize(num);
      deformedLatitude.resize(num);
      deformedLongitude.resize(num);
      deformedLatLonValid.resize(numberOfColumns);
      for (int i = oldNumberOfColumns; i < numberOfColumns; i++) {
         deformedLatLonValid[i] = false;
      }
   }
   numberOfNodesColumnsChanged();
}

/**
 * Get the lat/lon for a node.
 */
void 
LatLonFile::getLatLon(const int nodeNumber, const int columnNumber, float& lat, float& lon) const
{
   const int index = getOffset(nodeNumber, columnNumber);
   lat = latitude[index];
   lon = longitude[index];
}

/**
 * Get the deformed lat/lon for a node.
 */
void 
LatLonFile::getDeformedLatLon(const int nodeNumber, const int columnNumber, float& lat, float& lon) const
{
   const int index = getOffset(nodeNumber, columnNumber);
   lat = deformedLatitude[index];
   lon = deformedLongitude[index];
}

/**
 * Set the lat/lon for a node.
 */
void 
LatLonFile::setLatLon(const int nodeNumber, const int columnNumber, const float lat, const float lon)
{
   const int index = getOffset(nodeNumber, columnNumber);
   latitude[index] = lat;
   longitude[index] = lon;
   setModified();
}

/**
 * Set the deformed lat/lon for a node.
 */
void 
LatLonFile::setDeformedLatLon(const int nodeNumber, const int columnNumber, const float lat, const float lon)
{
   const int index = getOffset(nodeNumber, columnNumber);
   deformedLatitude[index] = lat;
   deformedLongitude[index] = lon;
   if ((lat != 0.0) || (lon != 0.0)) {
      deformedLatLonValid[columnNumber] = true;
   }
   setModified();
}

/**
 * Deform the paint file.
 */
void 
LatLonFile::deformFile(const DeformationMapFile& dmf, 
                   NodeAttributeFile& deformedFile,
                   const DEFORM_TYPE /*dt*/) const throw (FileException)
{   
   LatLonFile& deformedLatLonFile = dynamic_cast<LatLonFile&>(deformedFile);
   
   const int numNodes = dmf.getNumberOfNodes();
   const int numCols = getNumberOfColumns();
   deformedLatLonFile.setNumberOfNodesAndColumns(numNodes, numCols);
   
   //
   // Transfer stuff in AbstractFile and NodeAttributeFile
   //
   transferFileDataForDeformation(dmf, deformedLatLonFile);

   //
   // transfer the latlon columns
   //
   int tileNodes[3];
   float tileAreas[3];
   for (int i = 0; i < numNodes; i++) {
      dmf.getDeformDataForNode(i, tileNodes, tileAreas);
      for (int j = 0; j < numCols; j++) {
         float lat = 0.0, lon = 0.0, deflat = 0.0, deflon = 0.0;
         if (tileNodes[0] >= 0) {
            getLatLon(tileNodes[0], j, lat, lon);
            getDeformedLatLon(tileNodes[0], j, deflat, deflon);
         }
         deformedLatLonFile.setLatLon(i, j, lat, lon);
         deformedLatLonFile.setDeformedLatLon(i, j, lat, lon);
      }
   }
}

/**
 * reset a column of data.
 */
void 
LatLonFile::resetColumn(const int columnNumber)
{
   for (int i = 0; i < numberOfNodes; i++) {
      setLatLon(i, columnNumber, 0.0, 0.0);
      setDeformedLatLon(i, columnNumber, 0.0, 0.0);
   }
   deformedLatLonValid[columnNumber] = true;
   setModified();
}

/**
 * remove a column of data.
 */
void 
LatLonFile::removeColumn(const int columnNumber)
{
   if (numberOfColumns <= 1) {
      clear();
      return;
   }
   
   //
   // Transfer lat/lon data
   //
   LatLonFile llf;
   llf.setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);
   for (int i = 0; i < numberOfNodes; i++) {
      int ctr = 0;
      for (int j = 0; j < numberOfColumns; j++) {
         if (j != columnNumber) {
            float lat, lon;
            getLatLon(i, j, lat, lon);
            llf.setLatLon(i, ctr, lat, lon);
            getDeformedLatLon(i, j, lat, lon);
            llf.setDeformedLatLon(i, ctr, lat, lon);
            ctr++;
         }
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
         deformedLatLonValid[ctr] = deformedLatLonValid[j];
         ctr++;
      }
   }
   
   setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);
   latitude = llf.latitude;
   longitude = llf.longitude;
   deformedLatitude = llf.deformedLatitude;
   deformedLongitude = llf.deformedLongitude;
   deformedLatLonValid = llf.deformedLatLonValid;
   setModified();
}

/**
 * Read the lat/lon file's version 1 data.
 */
void 
LatLonFile::readFileDataVersion1(QTextStream& stream,
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
         std::cerr << "WARNING: Unknown Lat Lon File Tag: " << tag.toAscii().constData() << std::endl;
      }
   }

   if (numNodes <= 0) {
      throw FileException(filename, "No data in Lat/Lon file");
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
            for (int i = 0; i < numberOfNodes; i++) {
               readLineIntoTokens(stream, line, tokens);
               if (static_cast<int>(tokens.size()) == (numberOfColumns * 4 + 1)) {
                  for (int j = 0; j < numberOfColumns; j++) {
                     const int index  = getOffset(i, j);
                     latitude[index]  = tokens[j * 4 + 1].toFloat();
                     longitude[index] = tokens[j * 4 + 2].toFloat();
                     deformedLatitude[index]  = tokens[j * 4 + 3].toFloat();
                     deformedLongitude[index] = tokens[j * 4 + 4].toFloat();
                  }
               }
               else {
                  QString msg("Reading Lat/Lon file line: ");
                  msg.append(line);
                  throw FileException(filename, msg);
               }
            }   
         }
         break;
      case FILE_FORMAT_BINARY:
         for (int i = 0; i < numNodes; i++) {
            for (int j = 0; j < numCols; j++) {
               float lat, lon, deflat, deflon;
               binStream >> lat
                         >> lon
                         >> deflat
                         >> deflon;
               setLatLon(i, j, lat, lon);
               setDeformedLatLon(i, j, deflat, deflon);
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
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }
}

/**
 * Read the lat/lon file's version 0 data.
 */
void 
LatLonFile::readFileDataVersion0(QTextStream& stream,
                                 const bool readNumNodes) throw (FileException)
{
   QString line;
   
   if (readNumNodes) {
      readLine(stream, line);
      const int num = line.toInt();   
      setNumberOfNodesAndColumns(num, 1);
   }
      
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   int nodeNum;
   float lat, lon, deflat, deflon;
   for (int i = 0; i < numberOfNodes; i++) {      
      readLine(stream, line);
      const int numItems = sscanf(line.toAscii().constData(), "%d %f %f %f %f",
                                  &nodeNum, &lat, &lon, &deflat, &deflon);
      if ((numItems == 3) || (numItems == 5)) {
         const int index  = getOffset(i, 0);
         latitude[index]  = lat;
         longitude[index] = lon;
         if (numItems == 5) {
            deformedLatitude[index]  = deflat;
            deformedLongitude[index] = deflon;
         }
         else {
            deformedLatitude[index]  = 0.0;
            deformedLongitude[index] = 0.0;
         }
      }
      else {
         QString str("Invalid lat/lon file line: ");
         str.append(line);
         throw FileException(filename, str);
      }
   }
   setModified();
}

/**
 * Read the lat/lon file's data.
 */
void 
LatLonFile::readFileData(QFile& file, QTextStream& stream, 
                         QDataStream& binStream,
                         QDomElement& /* rootElement */) throw (FileException)
{
   //
   // save file position since version 0 has no tags
   //
   const qint64 oldTextStreamPosition = stream.pos();
   
   QString line;
   QString versionStr, versionNumberStr;
   readTagLine(stream, line, versionStr, versionNumberStr);
      
   int fileVersion = 0;

   if (versionStr == tagFileVersion) {
      fileVersion = versionNumberStr.toInt();
   }
   
   switch(fileVersion) {
      case 0:
#ifdef QT4_FILE_POS_BUG
         setNumberOfNodesAndColumns(line.toInt(), 1);
         readFileDataVersion0(stream, false);
#else  // QT4_FILE_POS_BUG
         file.seek(oldTextStreamPosition);
         stream.seek(oldTextStreamPosition);
         readFileDataVersion0(stream, true);
#endif // QT4_FILE_POS_BUG
         break;
      case 1:
         readFileDataVersion1(stream, binStream);
         break;
      default:
         throw FileException(filename, "Invalid Lat/Lon file version");
         break;
   }   
}

/**
 * Write the lat/lon file's data.
 */
void 
LatLonFile::writeFileData(QTextStream& stream, QDataStream& binStream,
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
         for (int i = 0; i < numNodes; i++) {
            stream << i;
            for (int j = 0; j < numCols; j++) {
               float lat, lon, deflat, deflon;
               getLatLon(i, j, lat, lon);
               getDeformedLatLon(i, j, deflat, deflon);
               stream << " "
                     << lat << " "
                     << lon << " "
                     << deflat << " "
                     << deflon;
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
               float lat, lon, deflat, deflon;
               getLatLon(i, j, lat, lon);
               getDeformedLatLon(i, j, deflat, deflon);
               binStream << lat
                         << lon
                         << deflat
                         << deflon;
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
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }
}

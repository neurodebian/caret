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
#include <limits>

#define _TOPOGRAPHY_FILE_MAIN_
#include "TopographyFile.h"
#undef _TOPOGRAPHY_FILE_MAIN_

#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "SpecFile.h"
#include "StringUtilities.h"

/** 
 * Constructor
 */
NodeTopography::NodeTopography() 
{
   topographyFile = NULL;
   setData(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "");
}

/** 
 * Constructor
 */
NodeTopography::NodeTopography(const float eccentricityMean, 
                               const float eccentricityLow,
                               const float eccentricityHigh,
                               const float polarMean, const float polarLow,
                               const float polarHigh, const QString& areaName)
{
   topographyFile = NULL;
   setData(eccentricityMean, eccentricityLow,
           eccentricityHigh, polarMean, polarLow,
           polarHigh, areaName);
}
           
/** 
 * Get data for a node
 */
void 
NodeTopography::getData(float& eccentricityMean, float& eccentricityLow,
                        float& eccentricityHigh,
                        float& polarMean, float& polarLow,
                        float& polarHigh, QString& areaName) const
{
   eccentricityMean = eMean;
   eccentricityLow = eLow;
   eccentricityHigh = eHigh;
   polarMean = pMean;
   polarLow = pLow;
   polarHigh = pHigh;
   areaName = name;
}

/** 
 * Set the data for a node.
 */
void
NodeTopography::setData(const float eccentricityMean, 
                        const float eccentricityLow,
                        const float eccentricityHigh,
                        const float polarMean, const float polarLow,
                        const float polarHigh, const QString& areaName)
{
   eMean   = eccentricityMean;
   eLow    = eccentricityLow;
   eHigh   = eccentricityHigh;
   pMean   = polarMean;
   pLow    = polarLow;
   pHigh   = polarHigh;
   name    = areaName;
   
   if (topographyFile != NULL) {
      topographyFile->setModified();
   }
}

/** 
 * The constructor.
 */
TopographyFile::TopographyFile()
   : NodeAttributeFile("Topography File",
                       SpecFile::getTopographyFileExtension())
{
   numberOfNodesVersion0 = -1;
   clear();
}

/** 
 * The destructor.
 */
TopographyFile::~TopographyFile()
{
   clear();
}

/** 
 * Clear the topography file.
 */
void
TopographyFile::clear()
{
   clearNodeAttributeFile();
   setNumberOfNodesAndColumns(0, 0);
}

/** 
 * Add topography for a node.
 */
void 
TopographyFile::setNodeTopography(const int nodeNumber, const int columnNumber,
                                  const NodeTopography& nt)
{
   const int indx = getOffset(nodeNumber, columnNumber);
   topography[indx] = nt;
   topography[indx].topographyFile = this;
   setModified();
}

/**
 * append a node attribute file to this one but selectively load/overwrite columns
 * columnDestination is where naf's columns should be (-1=new, -2=do not load).
 */
void 
TopographyFile::append(NodeAttributeFile& /*naf*/, 
                       std::vector<int> /*columnDestination*/,
                       const FILE_COMMENT_MODE /*fcm*/) throw (FileException)
{
   throw FileException("Not implemented yet.");
}


/**
 * append an topography file to this one
 */
void 
TopographyFile::append(NodeAttributeFile &naf) throw (FileException)
{
   TopographyFile& tf = dynamic_cast<TopographyFile&>(naf);
   
   if (getNumberOfNodes() != tf.getNumberOfNodes()) {
      throw FileException("Cannot append Topography, number of columns does not match.");
   }
      
   const int oldNumCols  = getNumberOfColumns();
   
   const int appendNumCols = tf.getNumberOfColumns();

   //
   // Add the additional columns to the file
   //   
   addColumns(appendNumCols);
   
   //
   // Transfer topography data
   //
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < appendNumCols; j++) {
         const NodeTopography nt = tf.getNodeTopography(i, j);
         setNodeTopography(i, oldNumCols + j, nt);
      }
   }
   
   for (int j = 0; j < appendNumCols; j++) {
      setColumnName(oldNumCols + j, getColumnName(j));
      setColumnComment(oldNumCols + j, getColumnComment(j));
   }
   
   setModified();
   
   //
   // transfer the file's comment
   //
   appendFileComment(tf);
}

/**      
 * add columns to this topography file
 */
void 
TopographyFile::addColumns(const int numberOfNewColumns)
{
   const int oldNumberOfColumns = numberOfColumns;
   const int totalColumns = numberOfColumns + numberOfNewColumns;
   
   //
   // Save existing topography data
   //
   const std::vector<NodeTopography> data = topography;
   
   //
   // Setup file for new number of columns (will expand space for column naming)
   //
   setNumberOfNodesAndColumns(numberOfNodes, totalColumns);
   
   //
   // transfer existing topography data
   //
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < numberOfColumns; j++) {
         if (j < oldNumberOfColumns) {
            const int oldIndex = (oldNumberOfColumns * i) + j;
            setNodeTopography(i, j, data[oldIndex]);
         }
      }
   }
   
   setModified();
}

/**
 *  Add nodes to the file.
 */
void 
TopographyFile::addNodes(const int numberOfNodesToAdd)
{
   setNumberOfNodesAndColumns(numberOfNodes + numberOfNodesToAdd, numberOfColumns);
   setModified();
}

/** 
 * Deform topography file.
 */
void 
TopographyFile::deformFile(const DeformationMapFile& dmf, 
                       NodeAttributeFile& deformedFile,
                       const DEFORM_TYPE /*dt*/) const throw (FileException)
{
   TopographyFile& deformedTopographyFile = dynamic_cast<TopographyFile&>(deformedFile);
   const int numNodes = dmf.getNumberOfNodes();
   
   //
   // Transfer stuff in AbstractFile and NodeAttributeFile
   //
   transferFileDataForDeformation(dmf, deformedTopographyFile);
   
   int tileNodes[3];
   float tileAreas[3];
   for (int i = 0; i < numNodes; i++) {
      dmf.getDeformDataForNode(i, tileNodes, tileAreas);
      for (int j = 0; j < getNumberOfColumns(); j++) {
         NodeTopography nt;
         if (tileNodes[0] > -1) {
            nt = getNodeTopography(tileNodes[0], j);
         }
         else if (tileNodes[1] > -1) {
            nt = getNodeTopography(tileNodes[1], j);
         }
         else if (tileNodes[2] > -1) {
            nt = getNodeTopography(tileNodes[2], j);
         }
         
         nt.topographyFile = &deformedTopographyFile;
         deformedTopographyFile.setNodeTopography(i, j, nt);
      }
   }
}

/**
 * reset a column of data.
 */
void 
TopographyFile::resetColumn(const int columnNumber)
{
   const NodeTopography nt(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "");
   for (int i = 0; i < numberOfNodes; i++) {
      setNodeTopography(i, columnNumber, nt);
   }
   
   setColumnName(columnNumber, "");
   setColumnComment(columnNumber, "");
   setModified();
}

/**
 * remove a column of data.
 */
void 
TopographyFile::removeColumn(const int columnNumber)
{
   if (numberOfColumns <= 1) {
      clear();
      return;
   }
   
   TopographyFile tf;
   tf.setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);
   
   for (int i = 0; i < numberOfNodes; i++) {
      int ctr = 0;
      for (int j = 0; j < numberOfColumns; j++) {
         if (j != columnNumber) {
            const NodeTopography nt = getNodeTopography(i, j);
            tf.setNodeTopography(i, ctr, nt);
            ctr++;
         }
      }
   }
   
   int ctr = 0;
   for (int j = 0; j < numberOfNodes; j++) {
      if (j != columnNumber) {
         setColumnName(ctr, getColumnName(j));
         setColumnComment(ctr, getColumnComment(j));
         ctr++;
      }
   }
   
   setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);
   topography = tf.topography;
   setModified();
}

/** 
 * Get the topography by index.
 */
/*
const NodeTopography* 
TopographyFile::getNodeTopographyByIndex(const int topoIndex) const 
{
   return &topography[topoIndex];
}
*/

/**
 * Set the number of nodes and columns.
 */
void
TopographyFile::setNumberOfNodesAndColumns(const int numNodes, const int numColumns)
{
   numberOfNodes = numNodes;
   numberOfColumns = numColumns;
   
   const int num = numberOfNodes * numberOfColumns * numberOfItemsPerColumn;
   if (num <= 0) {
      topography.clear();
   }
   else {
      topography.resize(num);
   }
   numberOfNodesColumnsChanged();
   setModified();
}

/** 
 * Get the topography by node number.
 */
NodeTopography 
TopographyFile::getNodeTopography(const int nodeNumber, const int columnNumber) const
{
   const int indx = getOffset(nodeNumber, columnNumber);
   return topography[indx];
}

/** 
 * Get the min/max topography.
 */
void 
TopographyFile::getMinMaxTopography(const int columnNumber,
                                    float eccentricityMean[2], 
                                    float eccentricityLow[2],
                                    float eccentricityHigh[2],
                                    float polarMean[2], 
                                    float polarLow[2],
                                    float polarHigh[2]) const
{
   eccentricityMean[0] =  std::numeric_limits<float>::max();
   eccentricityMean[1] = -std::numeric_limits<float>::max();
   eccentricityLow[0]  =  std::numeric_limits<float>::max();
   eccentricityLow[1]  = -std::numeric_limits<float>::max();
   eccentricityHigh[0] =  std::numeric_limits<float>::max();
   eccentricityHigh[1] = -std::numeric_limits<float>::max();
   polarMean[0]        =  std::numeric_limits<float>::max();
   polarMean[1]        = -std::numeric_limits<float>::max();
   polarLow[0]         =  std::numeric_limits<float>::max();
   polarLow[1]         = -std::numeric_limits<float>::max();
   polarHigh[0]        =  std::numeric_limits<float>::max();
   polarHigh[1]        = -std::numeric_limits<float>::max();
   
   for (int i = 0; i < getNumberOfNodes(); i++) {
      const NodeTopography nt = getNodeTopography(i, columnNumber);
      float emean, elow, ehigh, pmean, plow, phigh;
      QString areaName;
      nt.getData(emean, elow, ehigh, pmean, plow, phigh, areaName);
      
      if (areaName.isEmpty()) {
         continue;
      }
      
      if (emean < eccentricityMean[0]) {
         eccentricityMean[0] = emean;
      }
      if (emean > eccentricityMean[1]) {
         eccentricityMean[1] = emean;
      }
      if (elow < eccentricityLow[0]) {
         eccentricityLow[0] = elow;
      }
      if (elow > eccentricityLow[1]) {
         eccentricityLow[1] = elow;
      }
      if (ehigh < eccentricityHigh[0]) {
         eccentricityHigh[0] = ehigh;
      }
      if (ehigh > eccentricityHigh[1]) {
         eccentricityHigh[1] = ehigh;
      }
      
      if (pmean < polarMean[0]) {
         polarMean[0] = pmean;
      }
      if (pmean > polarMean[1]) {
         polarMean[1] = pmean;
      }
      if (plow < polarLow[0]) {
         polarLow[0] = plow;
      }
      if (plow > polarLow[1]) {
         polarLow[1] = plow;
      }
      if (phigh < polarHigh[0]) {
         polarHigh[0] = phigh;
      }
      if (phigh > polarHigh[1]) {
         polarHigh[1] = phigh;
      }
      
   }
}

/**
 * Read the version 0 file.
 */
void
TopographyFile::readFileDataVersion0(QTextStream& stream) throw (FileException)
{
   if (numberOfNodesVersion0 <= 0) {
      QString msg("The number of nodes for reading a version 0 topography file "
                      "have not been set.");
      throw FileException(filename, msg);
   }
   
   setNumberOfNodesAndColumns(numberOfNodesVersion0, 1);
   
   QString line;

   int numNodesInFile = -1;
   readLine(stream, line);
   QTextStream(&line, QIODevice::ReadOnly) >> numNodesInFile;
   
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   if (numNodesInFile <= 0) {
      throw FileException(filename, "No data in topography file");
   }

   for (int i = 0; i < numNodesInFile; i++) {
      readLine(stream, line);
      int node;
      QString an;
      float em, el, eh, pm, pl, ph;
      QTextStream(&line, QIODevice::ReadOnly) >> node >> an >> em >> el >> eh >> pm >> pl >> ph;
      NodeTopography nt(em, el, eh, pm, pl, ph, an);
      setNodeTopography(node, 0, nt);
   }
}

/**
 * Read the version 1 file.
 */
void
TopographyFile::readFileDataVersion1(QTextStream& stream) throw (FileException)
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
         const int indx = splitTagIntoColumnAndValue(tagValue, name);
         columnNames[indx] = name;
      }
      else if (tag == tagColumnComment) {
         QString name;
         const int indx = splitTagIntoColumnAndValue(tagValue, name);
         columnComments[indx] = StringUtilities::setupCommentForDisplay(name);
      }
      else if (tag == tagFileTitle) {
         fileTitle = tagValue;
      }
      else {
         std::cerr << "WARNING: Unknown Topography File Tag: " << tag.toAscii().constData() << std::endl;
      }
   }

   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   if (numNodes <= 0) {
      throw FileException(filename, "No data in Topography file");
   }
   
   std::vector<QString> tokens;
   QString line;
   for (int i = 0; i < numberOfNodes; i++) {
      readLineIntoTokens(stream, line, tokens);
      if (static_cast<int>(tokens.size()) == (numberOfColumns * 7 + 1)) {
         for (int j = 0; j < numberOfColumns; j++) {
            QString name(tokens[j * 7 + 1]);
            if (name == "*") {
               name = "";
            }
            const float em = tokens[j * 7 + 2].toFloat();
            const float el = tokens[j * 7 + 3].toFloat();
            const float eh = tokens[j * 7 + 4].toFloat();
            const float pm = tokens[j * 7 + 5].toFloat();
            const float pl = tokens[j * 7 + 6].toFloat();
            const float ph = tokens[j * 7 + 7].toFloat();
            NodeTopography nt;
            nt.setData(em, el, eh, pm, pl, ph, name);
            setNodeTopography(i, j, nt);
         }
      }
      else {
         QString msg("Reading Topography file line: ");
         msg.append(line);
         throw FileException(filename, msg);
      }
   }
}


/** 
 * Read the topography file.
 */
void
TopographyFile::readFileData(QFile& file, QTextStream& stream, QDataStream&,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // save file position since version 0 has no tags
   //
   qint64 oldFilePosition = stream.pos(); //file.pos();
   
   QString line;
   QString versionStr, versionNumberStr;
   readTagLine(stream, versionStr, versionNumberStr);
      
   int fileVersion = 0;

   if (versionStr == tagFileVersion) {
      fileVersion = versionNumberStr.toInt();
   }
   
   switch(fileVersion) {
      case 0:
         file.seek(oldFilePosition);
         stream.seek(oldFilePosition);
         readFileDataVersion0(stream);
         break;
      case 1:
         readFileDataVersion1(stream);
         break;
      default:
         throw FileException(filename, "Invalid Topography file version");
         break;
   }   
}

/** 
 * Write the topography file.
 */
void 
TopographyFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   stream << tagFileVersion << " 1" << "\n";
   stream << tagNumberOfNodes << " " << numberOfNodes << "\n";
   stream << tagNumberOfColumns << " " << numberOfColumns << "\n";
   stream << tagFileTitle << " " << fileTitle << "\n";
   for (int j = 0; j < numberOfColumns; j++) {
      stream << tagColumnName << " " << j << " " << columnNames[j] << "\n";
      stream << tagColumnComment << " " << j 
             << " " << StringUtilities::setupCommentForStorage(columnComments[j]) << "\n";
   }
   stream << tagBeginData << "\n";
   
   for (int i = 0; i < numberOfNodes; i++) {
      stream << i;
      for (int j = 0; j < numberOfColumns; j++) {
         const NodeTopography nt = getNodeTopography(i, j);
         QString name = nt.name;
         if (name.isEmpty()) {
            name = "*";
         }
         stream << " " << name << " "
                << nt.eMean   << " " << nt.eLow << " " << nt.eHigh << " "
                << nt.pMean   << " " << nt.pLow << " " << nt.pHigh;
      }
      stream << "\n";
   }
}

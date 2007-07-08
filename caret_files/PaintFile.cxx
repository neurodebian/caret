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
#include <set>
#include <sstream>

#define _PAINT_FILE_MAIN_
#include "PaintFile.h"
#undef _PAINT_FILE_MAIN_

#include "AreaColorFile.h"
#include "CoordinateFile.h"
#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "FreeSurferLabelFile.h"
#include "GiftiCommon.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "VolumeFile.h"


/**
 * Constructor.
 */
PaintFile::PaintFile(const QString& descriptiveName,
                     const QString& defaultExtensionIn)
   : GiftiNodeDataFile(descriptiveName, 
                       GiftiCommon::categoryLabels,
                       GiftiDataArray::DATA_TYPE_INT32,
                       1,
                       defaultExtensionIn,
                       AbstractFile::FILE_FORMAT_ASCII,
                       FILE_IO_READ_AND_WRITE, 
                       FILE_IO_READ_AND_WRITE, 
                       FILE_IO_NONE,
                       FILE_IO_NONE,
                       true)
{
   clear();
}

/**
 * copy constructor.
 */
PaintFile::PaintFile(const PaintFile& pf)
   : GiftiNodeDataFile(pf)
{
   copyHelperPaint(pf);
}

/**
 * Destructor.
 */
PaintFile::~PaintFile()
{
   clear();
}

/**
 * assignment operator.
 */
PaintFile& 
PaintFile::operator=(const PaintFile& pf)
{
   if (this != &pf) {
      GiftiNodeDataFile::operator=(pf);
      copyHelperPaint(pf);
   }
   
   return *this;
}

/**
 * copy helper used by assignment operator and copy constructor.
 */
void 
PaintFile::copyHelperPaint(const PaintFile& /*pf*/)
{
}
      
/**
 * Clean up paint names (elminate unused ones).
 */
void
PaintFile::cleanUpPaintNames()
{
   std::vector<QString> paintNames;
   labelTable.getAllLabels(paintNames);
   
   if (paintNames.size() == 0) {
      return;
   }
   
   const int numNodes = getNumberOfNodes();
   const int numCols  = getNumberOfColumns();
   if ((numNodes == 0) || (numCols == 0)) {
      labelTable.clear();
      return;
   }

   //
   // Find the "???" paint or add if not found
   //
   int questionPaint = getPaintIndexFromName("???");
   if (questionPaint < 0) {
      questionPaint = addPaintName("???");
   }
   
   const int numPaintNames = getNumberOfPaintNames();
   std::vector<bool> paintNameUsed(numPaintNames, false);
   
   //
   // Handle any duplicate names by using first index for name
   //
   for (int m = 0; m < (numPaintNames - 1); m++) {
      const QString paintName = getPaintNameFromIndex(m);
      for (int n = 0; n < numPaintNames; n++) {
         if (paintName == getPaintNameFromIndex(n)) {
            for (int i = 0; i < numNodes; i++) {
               for (int j = 0; j < numCols; j++) {
                  if (getPaint(i, j) == n) {
                     setPaint(i, j, m);
                  }
               }
            }
         }
      }
   }
   
   //
   // Keep the "???" paint even if it is not used.
   //
   paintNameUsed[questionPaint] = true;
      
   //
   // Determine which paint names are used
   //
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         int paint = getPaint(i, j);
         //
         // If paint index is invalid, set it to the ??? paint
         //
         if ((paint < 0) || (paint >= numPaintNames)) {
            paint = questionPaint;
         }
         paintNameUsed[paint] = true;
      }
   } 
   
   //
   // Create an indices from old paint index to new
   //
   std::vector<QString> usedNames;
   std::vector<int> oldToNew(numPaintNames, 0);
   for (int k = 0; k < numPaintNames; k++) {
      oldToNew[k] = -1;
      if (paintNameUsed[k]) {
         oldToNew[k] = usedNames.size();
         usedNames.push_back(getPaintNameFromIndex(k));
      }
   }
   
   //
   // Update paint indices
   //
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         const int paint = getPaint(i, j);
         setPaint(i, j, oldToNew[paint]);
      }
   }
   
   //
   // Update paint names
   //
   labelTable.clear();
   for (unsigned int i = 0; i < usedNames.size(); i++) {
      labelTable.addLabel(usedNames[i]);
   }
   
   setModified();
}
 
/**
 * add a paint name to this paint file.
 */
int
PaintFile::addPaintName(const QString& nameIn)
{
   if (labelTable.empty()) {
      labelTable.addLabel("???");
   }

   int indx = getPaintIndexFromName(nameIn);
   if (indx >= 0) {
      return indx;
   }
   
   setModified();
   return labelTable.addLabel(nameIn);
}

/**
 * Deform the paint file.
 */
void 
PaintFile::deformFile(const DeformationMapFile& dmf, 
                  GiftiNodeDataFile& deformedFile,
                  const DEFORM_TYPE /*dt*/) const throw (FileException)
{   
   PaintFile& deformedPaintFile = dynamic_cast<PaintFile&>(deformedFile);
   
   const int numNodes = dmf.getNumberOfNodes();
   const int numColumns = getNumberOfColumns();
   deformedPaintFile.setNumberOfNodesAndColumns(numNodes, numColumns);
   
   //
   // transfer paint names
   //
   deformedPaintFile.labelTable = labelTable;
   
   //
   // Transfer stuff in AbstractFile and NodeAttributeFile
   //
   transferFileDataForDeformation(dmf, deformedPaintFile);
   
   //
   // transfer the paint columns
   //
   int* paints = new int[numColumns];
   int tileNodes[3];
   float tileAreas[3];
   for (int i = 0; i < numNodes; i++) {
      dmf.getDeformDataForNode(i, tileNodes, tileAreas);
      if (tileNodes[0] > -1) {
         getPaints(tileNodes[0], paints);
      }
      else {
         for (int j = 0; j < numColumns; j++) {
            paints[j] = 0;
         }
      }
      deformedPaintFile.setPaints(i, paints);
   }
   
   delete[] paints;
}

/**
 * Get the index of the column named "geography"
 */
int 
PaintFile::getGeographyColumnNumber() const
{
   char *geography = "geography";
   
   const int numberOfColumns = getNumberOfColumns();
   for (int i = 0; i < numberOfColumns; i++) {
      const QString colNameLowCase(StringUtilities::makeLowerCase(getColumnName(i)));
      if (colNameLowCase.indexOf(geography) != -1) {
         return i;
      }
   }
   
   return -1;
}

/**
 * assign colors to the labels.
 */
void 
PaintFile::assignColors(const ColorFile& colorFile)
{
   labelTable.assignColors(colorFile);
}
      
/**
 * get the index of a paint name.
 */
int 
PaintFile::getPaintIndexFromName(const QString& nameIn) const
{
   return labelTable.getLabelIndex(nameIn);
}

/**
 * get the name of a paint at an index.
 */
QString 
PaintFile::getPaintNameFromIndex(const int indexIn) const
{
   return labelTable.getLabel(indexIn);
}

/**
 * set the name of a paint at an index.
 */
void 
PaintFile::setPaintName(const int indexIn, const QString& name) 
{
   labelTable.setLabel(indexIn, name);
}

/**
 * get the number of paint names.
 */
int 
PaintFile::getNumberOfPaintNames() const
{
   return labelTable.getNumberOfLabels();
}

/** 
 * Get indices to all paint names used by a column.
 */
void
PaintFile::getPaintNamesForColumn(const int column, std::vector<int>& indices) const
{
   indices.clear();
   
   const int numNames = getNumberOfPaintNames();
   if (numNames > 0) {
      std::vector<int> namesUsed(numNames, -1);
      const int numNodes = getNumberOfNodes();
      for (int i = 0; i < numNodes; i++) {
         namesUsed[getPaint(i, column)] = 1;
      }
      
      for (int i = 0; i < numNames; i++) {
         if (namesUsed[i] >= 0) {
            indices.push_back(i);
         }
      }
   }
}

/**
 * Clear this file.
 */
void
PaintFile::clear()
{
   GiftiNodeDataFile::clear();
   setNumberOfNodesAndColumns(0, 0);
}

/**
 * Get all of the paints for a node.  User must allocate "getNumberOfNodes" number 
 * of ints for seconds parameter.
 */
void PaintFile::getPaints(const int nodeNumber, int* paints) const
{
   const int numberOfColumns = getNumberOfColumns();
   for (int i = 0; i < numberOfColumns; i++) {
      int32_t* nodePaints = dataArrays[i]->getDataPointerInt();
      paints[i] = nodePaints[nodeNumber];
   }
}

/**
 * Get a paint for a specified node and index.
 */
int PaintFile::getPaint(const int nodeNumber, const int columnNumber) const
{
   if ((columnNumber >= 0) &&
       (columnNumber < getNumberOfDataArrays())) {
      int32_t* nodePaints = dataArrays[columnNumber]->getDataPointerInt();
      return nodePaints[nodeNumber];
   }
   return 0;
}

/**
 * Set all of the paints for a node.
 */
void
PaintFile::setPaints(const int nodeNumber, const int* paints)
{
   const int numberOfColumns = getNumberOfColumns();
   for (int i = 0; i < numberOfColumns; i++) {
      int32_t* nodePaints = dataArrays[i]->getDataPointerInt();
      nodePaints[nodeNumber] = paints[i];
   }
   setModified();
}

/**
 * Set a paint for a specified node and column.
 */
void
PaintFile::setPaint(const int nodeNumber, const int columnNumber,
                    const int paint)
{
   if ((columnNumber >= 0) &&
       (columnNumber < getNumberOfDataArrays())) {
      int32_t* nodePaints = dataArrays[columnNumber]->getDataPointerInt();
      nodePaints[nodeNumber] = paint;
      setModified();
   }
}

/**
 * assign paints by intersecting with a volume file.
 * If a coordinate is within a non-zero voxel, the paintName is assigned for that coordinate.
 * This was known as IntersectVolumeWithSurface in SureFit.
 */
void 
PaintFile::assignPaintColumnWithVolumeFile(const VolumeFile* vf,
                                           const CoordinateFile* cf,
                                           const int columnToAssign,
                                           const QString& paintName) throw (FileException)
{
   if (vf == NULL) {
      throw FileException("Invalid volume file (NULL).");
   }
   if (cf == NULL) {
      throw FileException("Invalid coordinate file (NULL).");
   }
   if (getNumberOfNodes() <= 0) {
      throw FileException("Paint file has not been allocated (no nodes).");
   }
   if ((columnToAssign < 0) || (columnToAssign >= getNumberOfColumns())) {
      throw FileException("Paint column is invalid.");
   }
   if (getNumberOfNodes() != cf->getNumberOfCoordinates()) {
      throw FileException("Paint file has different number of coordinates that coordinate file.");
   }
   
   //
   // Index of paint name
   //
   const int paintIndex = addPaintName(paintName);
   
   //
   // Loop through coordinates
   //
   const int numCoords = cf->getNumberOfCoordinates();
   for (int i = 0; i < numCoords; i++) {
      //
      // Convert coordinate to a voxel index
      //
      int ijk[3];
      if (vf->convertCoordinatesToVoxelIJK(cf->getCoordinate(i), ijk)) {
         //
         // assign the paint column for the node if the voxel is non-zero
         //
         if (vf->getVoxel(ijk) != 0) {
            setPaint(i, columnToAssign, paintIndex);
         }
      }
   }
}

/**
 * Read a paint file' data.
 */
void
PaintFile::readLegacyNodeFileData(QFile& file, QTextStream& stream, 
                        QDataStream& binStream) throw (FileException)
{
   QString line;
   
   const qint64 pos = stream.pos(); //file.pos();
   
   readLine(stream, line);
   
   int fileVersion = 0;
   QString versionTag;
   
   QTextStream(&line, QIODevice::ReadOnly) >> versionTag >> fileVersion;

   if (tagFileVersion != versionTag) {
      fileVersion = 0;
      file.seek(pos);
      stream.seek(pos);
   }

   switch(fileVersion) {
      case 0:
         readFileDataVersion0(file, stream, binStream);
         break;
      case 1:
         readFileDataVersion1(file, stream, binStream);
         break;
      default:
         throw FileException(filename, "Unknown version number.  Perhaps paint file format is "
                               "newer than the caret you are using.");
         break;
   }
}
     
/**
 * Read a version 1 paint file.
 */
void
PaintFile::readFileDataVersion1(QFile& file,
                                QTextStream& stream,
                                QDataStream& binStream) throw (FileException)
{
   QString line;
   
   int numNodes = -1;
   int numCols  = -1;
   int numPaintNames = -1;
   
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
      else if (tag == tagFileTitle) {
         fileTitle = tagValue;
      }
      else if (tag == tagColumnName) {
         QString name;
         const int indx = splitTagIntoColumnAndValue(tagValue, name);
         setColumnName(indx, name);
      }
      else if (tag == tagColumnComment) {
         QString name;
         const int indx = splitTagIntoColumnAndValue(tagValue, name);
         setColumnComment(indx, StringUtilities::setupCommentForDisplay(name));
      }
      else if (tag == tagColumnStudyMetaData) {
         QString name;
         const int indx = splitTagIntoColumnAndValue(tagValue, name);
         StudyMetaDataLink smdl;
         smdl.setLinkFromCodedText(name);
         setColumnStudyMetaDataLink(indx, smdl);
      }
      else if (tag == tagNumberOfPaintNames) {
         numPaintNames = tagValue.toInt();
      }
      else {
         std::cerr << "WARNING: Unknown Paint File Tag: " << tag.toAscii().constData() << std::endl;
      }
   }
   
   if ((numNodes <= 0) && (numCols <= 0)) {
      throw FileException(filename, "Number of nodes or columns invalid ");
   }
   if (numPaintNames <= 0) {
      throw FileException(filename, "Number of paint names invalid ");
   }
   
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }
   
   //
   // Duplicate paint names will be discarded by "addPaintName" which will
   // return the index to use for the paint name
   //
#ifdef QT4_FILE_POS_BUG
   QString lastLineRead;
#endif // QT4_FILE_POS_BUG
   std::vector<int> paintToPaintNameIndex;
   for (int m = 0; m < numPaintNames; m++) {
      QString nameLine;
#ifdef QT4_FILE_POS_BUG
      nameLine = file.readLine().trimmed();
#else // QT4_FILE_POS_BUG
      readLine(stream, nameLine);
#endif // QT4_FILE_POS_BUG
      QString name;
      int n;
      
#ifdef QT4_FILE_POS_BUG
      lastLineRead = nameLine;
#endif // QT4_FILE_POS_BUG
      
      QTextStream(&nameLine, QIODevice::ReadOnly) >> n >> name;
      paintToPaintNameIndex.push_back(addPaintName(name));
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
   readPaintDataForNodes(paintToPaintNameIndex, file, stream, binStream);
}

/**
 * Read a version 0 paint file.
 */
void
PaintFile::readFileDataVersion0(QFile& file,
                                QTextStream& stream,
                                QDataStream& binStream) throw (FileException)
{
   bool readingPaints = true;
   
   //
   // Need to keep paint names here because "setNumberOfNodesAndColumns"
   // would erase them
   //
   std::vector<QString> tempPaintNames;
   

   QString line;
   while (readingPaints) {
      std::vector<QString> tokens;
      readLineIntoTokens(stream, line, tokens);
      if (tokens.size() == 2) {
         tempPaintNames.push_back(tokens[1]);
      }
      else {
         readingPaints = false;
      }
   }
   
   const int numNodes = line.toInt();
   //QString line;
   //readLine(stream, line);
   //const int numNodes = line.toInt();
   
   const int numColumns = 5;
   setNumberOfNodesAndColumns(numNodes, numColumns);
   setColumnName(0, "Lobes");
   setColumnName(1, "Geography");
   setColumnName(2, "Functional");
   setColumnName(3, "Brodmann");
   setColumnName(4, "Modality");
   
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }
   
   //
   // Duplicate paint names will be discarded by "addPaintName" which will
   // return the index to use for the paint name
   //
   std::vector<int> paintToPaintNameIndex;

   //
   // Transfer paint names now that setNumberOfNodesAndColumns has been called
   //
   for (unsigned int m = 0; m < tempPaintNames.size(); m++) {
      paintToPaintNameIndex.push_back(addPaintName(tempPaintNames[m]));
   }

   readPaintDataForNodes(paintToPaintNameIndex, file, stream, binStream);
}

/**
 * Read the paint data for the nodes.
 */
void
PaintFile::readPaintDataForNodes(const std::vector<int>& paintToPaintNameIndex,
                                 QFile& file,
                                 QTextStream& stream,
                                 QDataStream& binStream) throw (FileException)
{
   file.seek(stream.pos()); // QT 4.2.2
   
   QString line;
   std::vector<QString> tokens;

   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   
   int* cols = new int[numberOfColumns];
   
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         for (int i = 0; i < numberOfNodes; i++) {
            readLineIntoTokens(stream, line, tokens);
      
            if (static_cast<int>(tokens.size()) < (numberOfColumns+ 1)) {
               throw FileException(filename, "invalid paint data line: \n" + line);
            }
            for (int n = 0; n < numberOfColumns; n++) {
               const int indx = tokens[n+1].toInt();
               if ((indx < 0) || 
                  (indx >= static_cast<int>(paintToPaintNameIndex.size()))) {
                  throw FileException(filename, "invalid paint index =" + QString::number(indx)
                                                + "  node=" + QString::number(i));
               }
               else {
                  cols[n] = paintToPaintNameIndex[indx];
               }
            }
            setPaints(i, cols);
         }
         break;
      case FILE_FORMAT_BINARY:
         for (int j = 0; j < numberOfNodes; j++) {
            for (int k = 0; k < numberOfColumns; k++) {
               binStream >> cols[k];
            }
            setPaints(j, cols);
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
         
   delete[] cols;
}

/**
 * Write a paint file's data.
 */
void
PaintFile::writeLegacyNodeFileData(QTextStream& stream, QDataStream& binStream) throw (FileException)
{
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   //
   // output tags
   //
   stream << tagFileVersion << " 1\n";
   stream << tagNumberOfNodes << " " << numberOfNodes << "\n";
   stream << tagNumberOfColumns << " " << numberOfColumns << "\n";
   stream << tagFileTitle << " " << fileTitle << "\n";
   const int numLabels = labelTable.getNumberOfLabels();
   stream << tagNumberOfPaintNames << " " 
          << numLabels << "\n";
   for (int m = 0; m < numberOfColumns; m++) {
      stream << tagColumnName << " " << m << " " << getColumnName(m) << "\n";
      stream << tagColumnComment << " " << m 
             << " " << StringUtilities::setupCommentForStorage(getColumnComment(m)) << "\n";
   }
   for (int k = 0; k < numberOfColumns; k++) {
      stream << tagColumnStudyMetaData << " " << k
             << " " << getColumnStudyMetaDataLink(k).getLinkAsCodedText().toAscii().constData() << "\n";
   }
   stream << tagBeginData << "\n";
      
   //
   // Output Paint names
   //
   for (int i = 0; i < numLabels; i++) {
      stream << i << " " << labelTable.getLabel(i) << "\n";
   }
 
   //
   // Output paint columns
   //
   int* pti = new int[numberOfColumns];

   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         for (int j = 0; j < numberOfNodes; j++) {
            stream << j;
            getPaints(j, pti);
            for (int k = 0; k < numberOfColumns; k++) {
               stream << " " << pti[k];
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
            getPaints(j, pti);
            for (int k = 0; k < numberOfColumns; k++) {
               binStream << pti[k];
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

   delete[] pti;
}

/**
 * Export to a free surfer label file.
 */
void
PaintFile::exportFreeSurferAsciiLabelFile(const int columnNumber,
                                          const QString& filenamePrefix,
                                          const CoordinateFile* cf)
                                                    throw (FileException)
{
   if ((columnNumber >= 0) && (columnNumber < getNumberOfColumns())) {
      //
      // Find paints that are to be exported
      //
      std::set<int> exportedPaints;
      const int numNodes = getNumberOfNodes();
      for (int i = 0; i < numNodes; i++) {
         const int value = getPaint(i, columnNumber);
         if (value > 0) {
            if (exportedPaints.find(value) == exportedPaints.end()) {
               exportedPaints.insert(value);
               const QString paintName(getPaintNameFromIndex(value));
               QString filename(filenamePrefix);
               filename.append(paintName);
               //filename.append(".label");
               
               
               //
               // Find the nodes that have this paint
               //
               std::vector<int> matchingNodes;
               for (int j = i; j < numNodes; j++) {
                  if (getPaint(j, columnNumber) == value) {
                     matchingNodes.push_back(j);
                  }
               }
               
               //
               // are there any matching nodes
               //
               const int numMatchingNodes = matchingNodes.size();
               if (numMatchingNodes > 0) {
                  FreeSurferLabelFile fslf;
                  fslf.setNumberOfLabelItems(numMatchingNodes);
                  
                  //
                  // Transfer the label data
                  //
                  for (int k = 0; k < numMatchingNodes; k++) {
                     float xyz[3];
                     cf->getCoordinate(matchingNodes[k], xyz);
                     fslf.setLabelItem(k, matchingNodes[k], xyz);
                  }
               
                  //
                  // Write the file
                  //
                  fslf.writeFile(filename);
               }
            }
         }
      }
   }
   else {
      throw FileException(filename, "Invalid column number for export.");
   }
}

/**
 * Import Free Surfer label file(s).
 */
void
PaintFile::importFreeSurferAsciiLabelFile(const int numNodes,
                                          const QString& filename,
                                          AreaColorFile* colorFile,
                                          const bool importAllInDirectory) 
                                                    throw (FileException)
{
   if (numNodes == 0) {
      throw FileException(filename, "A surface must be loaded prior to importing a "
                                      "FreeSurfer label file.");
   }

   //
   // Add a column to this surface shape file
   //
   if (getNumberOfColumns() == 0) {
      setNumberOfNodesAndColumns(numNodes, 1);
   }
   else {
      addColumns(1);
   }
   const int columnNumber = getNumberOfColumns() - 1;
   
   //
   // Set the name of the column to the name of the curvature file
   //
   setColumnName(columnNumber, FileUtilities::basename(filename));
   
   
   setModified();
   
   if (importAllInDirectory) {
      //
      // Find all label files in the directory
      //
      QString directoryName(FileUtilities::dirname(filename));
      if (directoryName.isEmpty()) {
         directoryName = ".";
      }
      std::vector<QString> labelFiles;
      FileUtilities::findFilesInDirectory(directoryName, QStringList("*.label"), labelFiles);
      
      //
      // Import all of the label files found
      //
      for (int i = 0; i < static_cast<int>(labelFiles.size()); i++) {
         QString name;
         if (directoryName.isEmpty() == false) {
            name = directoryName;
            name.append("/");
         }
         name.append(labelFiles[i]);
         importSingleFreeSurferLabelFile(columnNumber, numNodes, colorFile, name);
      }
      if (labelFiles.size() > 0) {
         appendToFileComment(" Imported from multiple files starting with ");
         appendToFileComment(FileUtilities::basename(labelFiles[0]));
      }
   }
   else {
      //
      // Import just this label file
      //
      importSingleFreeSurferLabelFile(columnNumber, numNodes, colorFile, filename);
      appendToFileComment(" Imported from ");
      appendToFileComment(FileUtilities::basename(filename));
   }
}

/**
 * Import a single free surfer label file to the specified paint column.
 */
void
PaintFile::importSingleFreeSurferLabelFile(const int columnNumber,
                                           const int numNodes,
                                           AreaColorFile* colorFile,
                                           const QString& filename) throw (FileException)
{
   FreeSurferLabelFile fslf;
   fslf.readFile(filename);
   
   //
   // Extract the paint area/name from the file's name
   //
   QString nameOfPaint;
   QString fileBasename(FileUtilities::basename(filename));
   const int nameStart = filename.indexOf(fileBasename, '-');
   const int nameEnd   = filename.indexOf(fileBasename, '.');
   if ((nameStart != -1) && (nameEnd != -1) &&
         (nameEnd > nameStart)) {
      const int len = nameEnd - nameStart - 1;
      nameOfPaint = fileBasename.mid(nameStart + 1, len);
   }
   else {
      nameOfPaint = fileBasename;
   }
   const int paintIndex = addPaintName(nameOfPaint);
         
   //
   // Add name to area colors
   //
   if (colorFile != NULL) {
      bool match = false;
      const int colorIndex = colorFile->getColorIndexByName(nameOfPaint, match);
      if ((match == false) || (colorIndex < 0)) {
         colorFile->addColor(nameOfPaint, 255, 0, 0);
      }
   }
         
   //
   // Read in label data
   //
   const int numItems = fslf.getNumberOfLabelItems();
   for (int i = 0; i < numItems; i++) {
      int nodeNumber;
      float xyz[3];
      fslf.getLabelItem(i, nodeNumber, xyz);
      if (nodeNumber >= numNodes) {
         std::ostringstream str;
         str << "Node "
             << nodeNumber
             << " from label file "
             << nameOfPaint.toAscii().constData()
             << " is greater than number of nodes in the surface.";
         throw FileException(filename, str.str().c_str());
      }
      setPaint(nodeNumber, columnNumber, paintIndex);
   }
}

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

#include <QDir>
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
#include "NameIndexSort.h"
#include "NodeRegionOfInterestFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VolumeFile.h"


/**
 * Constructor.
 */
PaintFile::PaintFile(const QString& descriptiveName,
                     const QString& defaultExtensionIn)
   : GiftiNodeDataFile(descriptiveName, 
                       GiftiCommon::intentLabels,
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
      
class LabelOldToNew {
public:
   LabelOldToNew(const QString& nameIn) {
      this->name = nameIn;
      this->newIndex = -1;
      this->usedFlag = false;
      this->duplicateFlag = false;
   }

   int newIndex;
   QString name;
   bool usedFlag;
   bool duplicateFlag;
};

/**
 * Clean up paint names (elminate unused ones).
 */
void
PaintFile::cleanUpPaintNames()
{
   const int numNodes = getNumberOfNodes();
   const int numCols  = getNumberOfColumns();
   if ((numNodes == 0) || (numCols == 0)) {
      labelTable.clear();
      return;
   }

   int numLabels = this->labelTable.getNumberOfLabels();
   if (numLabels <= 0) {
      return;
   }

   /*
    * Create a mapping that will map old indices to new indices
    * after unused and duplicate labels are removed.
    */
   std::vector<LabelOldToNew> labelsOldToNew;
   labelsOldToNew.reserve(numLabels);
   for (int m = 0; m < numLabels; m++) {
      LabelOldToNew lotn(this->labelTable.getLabel(m));
      labelsOldToNew.push_back(lotn);
   }

   /*
    * Find duplicate label names.
    */
   for (int m = 0; m < (numLabels - 1); m++) {
      if (labelsOldToNew[m].duplicateFlag == false) {
         const QString& labelName = this->labelTable.getLabel(m);
         for (int n = (m + 1); n < numLabels; n++) {
             const QString& dupLabelName = this->labelTable.getLabel(n);
             if (labelName == dupLabelName) {
                labelsOldToNew[n].duplicateFlag = true;
                if (DebugControl::getDebugOn()) {
                   std::cout << "Duplicate label: " << dupLabelName.toAscii().constData() << std::endl;
                }
             }
         }
      }
   }

   /*
    * Find labels that are used.
    */
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         const int labelIndex = this->getPaint(i, j);
         labelsOldToNew[labelIndex].usedFlag = true;
      }
   }

   /*
    * Remove duplicate and unused labels
    * Need to start at last index since labels are
    * being removed.
    */
   int numUsedLabels = 0;
   for (int m = (numLabels - 1); m >= 0; m--) {
      const QString& labelName = this->getLabelTable()->getLabel(m);
      if (labelsOldToNew[m].usedFlag == false) {
         this->labelTable.deleteLabel(m);
         if (DebugControl::getDebugOn()) {
            std::cout << "Label is not used: " << labelName.toAscii().constData() << std::endl;
         }
      }
      else if (labelsOldToNew[m].duplicateFlag) {
         this->labelTable.deleteLabel(m);
         if (DebugControl::getDebugOn()) {
            std::cout << "Label is duplicate: " << labelName.toAscii().constData() << std::endl;
         }
      }
      else {
         numUsedLabels++;
      }
   }
   if (DebugControl::getDebugOn()) {
      std::cout << "Labels UNused: " << (numLabels - numUsedLabels) << std::endl;
      std::cout << "Labels used: " << numUsedLabels << std::endl;
   }

   /*
    * If no labels are removed, then exit.
    */
   if (numLabels == numUsedLabels) {
      return;
   }

   /*
    * Create a mapping from old indices to new indices
    * now that labels have been removed.
    */
   int numOldLabels = static_cast<int>(labelsOldToNew.size());
   for (int m = 0; m < numOldLabels; m++) {
      const QString& labelName = labelsOldToNew[m].name;
      int newIndex = this->labelTable.getLabelIndex(labelName);
      labelsOldToNew[m].newIndex = newIndex;
   }

   /*
    * Update to new label indices.
    */
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         int oldIndex = this->getPaint(i, j);
         int newIndex = labelsOldToNew[oldIndex].newIndex;
         if (newIndex >= 0) {
            this->setPaint(i, j, newIndex);
         }
         else {
            std::cout << "PROGRAM ERROR Cleaning paint names: "
                      << "New index for "
                      << labelsOldToNew[oldIndex].name.toAscii().constData()
                      << " is invalid.";
         }
      }
   }


   //   GiftiLabelTable newLabelTable;

//   //
//   // Loop through all label names mapping into new label table
//   //
//   std::map<QString,int> namesToIndicesMap;
//   std::vector<int> remapIndices;
//   namesToIndicesMap.insert(std::make_pair("???", newLabelTable.getNumberOfLabels()));
//   int remapCount = 0;
//   for (int i = 0; i < numPaintNames; i++) {
//      if (i > 0) {
//         if ((i % 10000) == 0) {
//            std::cout << "Remap "
//                      << i
//                      << " of "
//                      << numPaintNames
//                      << std::endl;
//         }
//      }

//      const QString paintName = this->getPaintNameFromIndex(i);
//      std::map<QString,int>::iterator iter = namesToIndicesMap.find(paintName);

//      int newIndex = -1;
//      if (iter != namesToIndicesMap.end()) {
//         newIndex = iter->second;
//      }
//      else {
//         newIndex = newLabelTable.addLabel(paintName);;
//         namesToIndicesMap.insert(std::make_pair(paintName, newIndex));
//      }


//      if (newIndex != i) {
//         remapCount++;
//      }
//      remapIndices.push_back(newIndex);
//   }
//   if (remapCount == 0) {
//      return;
//   }
//   std::cout << "Remapping "
//             << remapCount
//             << " paint names."
//             << std::endl;

//   //
//   // Update any indices
//   //
//   for (int i = 0; i < numNodes; i++) {
//      for (int j = 0; j < numCols; j++) {
//         int paintIndex = this->getPaint(i, j);
//         if ((paintIndex >=0) &&
//             (paintIndex < numPaintNames)) {
//            paintIndex = remapIndices[paintIndex];
//            this->setPaint(i, j, paintIndex);
//         }
//      }
//   }

//   //
//   // Update paint names
//   //
//   labelTable.clear();
//   int numNewNames = newLabelTable.getNumberOfLabels();
//   for (int i = 0; i < numNewNames; i++) {
//      labelTable.addLabel(newLabelTable.getLabel(i));
//   }

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

   deformedPaintFile.cleanUpPaintNames();
}

/**
 * assign paint from an ROI file.
 */
void 
PaintFile::assignNodesFromROIFile(const int columnNumber,
                                    const NodeRegionOfInterestFile& roiFile,
                                    const QString& paintName,
                                    const bool assignNodesInRoiOnlyFlag) throw (FileException)
{
   if ((columnNumber < 0) ||
       (columnNumber >= getNumberOfColumns())) {
      throw FileException("Column number for ROI paint assignment is invalid.");
   }
   
   const int numNodes = getNumberOfNodes();
   if (roiFile.getNumberOfNodes() != numNodes) {
      throw FileException("ROI paint number of nodes does not match.");
   }
   
   const int paintNameIndex = addPaintName(paintName);
   if (assignNodesInRoiOnlyFlag) {
      for (int i = 0; i < numNodes; i++) {
         if (roiFile.getNodeSelected(i)) {
            setPaint(i, columnNumber, paintNameIndex);
         }
      }
   }
   else {
      const int questionNameIndex = addPaintName("???");
      for (int i = 0; i < numNodes; i++) {
         if (roiFile.getNodeSelected(i)) {
            setPaint(i, columnNumber, paintNameIndex);
         }
         else {
            setPaint(i, columnNumber, questionNameIndex);
         }
      }
   }
}                                  

/**
 * Get the index of the column named "geography"
 */
int 
PaintFile::getGeographyColumnNumber() const
{
   const QString geography("geography");
   
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
 * delete a paint any nodes using paint become ???.
 */
void 
PaintFile::deletePaintName(const int paintIndex)
{
   const int questPaintIndex = addPaintName("???");
   const int numNodes = getNumberOfNodes();
   const int numCols  = getNumberOfColumns();
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         const int indx = getPaint(i, j);
         if (indx == paintIndex) {
            setPaint(i, j, questPaintIndex);
         }
         else if (indx > paintIndex) {
            //
            // Move "higher" paint indices down one since "paintIndex" 
            // will be removed.
            //
            setPaint(i, j, indx - 1);
         }
      }
   }
   
   labelTable.deleteLabel(paintIndex);
}

/**
 * deassign a paint any nodes in column using paint become ???
 * if columnNumber is negative, operation is applied to all columns.
 */
void 
PaintFile::deassignPaintName(const int columnNumber,
                             const int paintIndex)
{
   const int questPaintIndex = addPaintName("???");
   reassignPaintName(columnNumber, 
                     paintIndex,
                     questPaintIndex);
}
                   
/**
 * reassign a paint 
 * if columnNumber is negative, operation is applied to all columns.
 */
void 
PaintFile::reassignPaintName(const int columnNumber,
                             const int oldPaintIndex,
                             const int newPaintIndex)
{
   int colStart = 0;
   int colEnd = getNumberOfColumns();
   if ((columnNumber >= 0) &&
       (columnNumber < getNumberOfColumns())) {
      colStart = columnNumber;
      colEnd   = columnNumber + 1;
   }
   else if (columnNumber >= getNumberOfColumns()) {
      return;
   }
   
   const int numNodes = getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
      for (int j = colStart; j < colEnd; j++) {
         const int indx = getPaint(i, j);
         if (indx == oldPaintIndex) {
            setPaint(i, j, newPaintIndex);
         }
      }
   }
}
                             
/**
 * set the name of a paint at an index.
 */
void 
PaintFile::setPaintName(const int indexIn, const QString& name) 
{
   labelTable.setLabel(indexIn, name);
   setModified();
}

/**
 * get paint enabled by index.
 */
bool 
PaintFile::getPaintNameEnabled(const int paintIndex) const
{
   return labelTable.getLabelEnabled(paintIndex);
}

/**
 * set paint enable by index.
 */
void 
PaintFile::setPaintNameEnabled(const int paintIndex,
                               const bool b)
{
   labelTable.setLabelEnabled(paintIndex, b);
}
                               
/**
 * set all paint names enabled.
 */
void 
PaintFile::setAllPaintNamesEnabled(const bool b)
{
   labelTable.setAllLabelsEnabled(b);
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
 * get all paint names and indices sorted by name (may contain duplicate names).
 */
void 
PaintFile::getAllPaintNamesAndIndices(std::vector<QString>& namesOut,
                                      std::vector<int>& indicesOut) const
{
   namesOut.clear();
   indicesOut.clear();

   //
   // Sort by name
   //
   NameIndexSort nis;
   const int num = getNumberOfPaintNames();
   for (int i = 0; i < num; i++) {
      nis.add(i, getPaintNameFromIndex(i));
   }
   nis.sortByNameCaseSensitive();
   
   //
   // Output sorted names/indices
   //
   const int numNames = nis.getNumberOfItems();
   for (int i = 0; i < numNames; i++) {
      int indx;
      QString name;
      nis.getSortedNameAndIndex(i, indx, name);
      namesOut.push_back(name);
      indicesOut.push_back(indx);
   }
}
                                      
/**
 * get all paint names (could be duplicates).
 */
void 
PaintFile::getAllPaintNames(std::vector<QString>& namesOut) const
{
   namesOut.clear();
   
   const int num = getNumberOfPaintNames();
   for (int i = 0; i < num; i++) {
      namesOut.push_back(getPaintNameFromIndex(i));
   }
}

/**
 * get all paint count (# nodes using each paint).
 */
void 
PaintFile::getAllPaintCounts(std::vector<int>& countsOut) const
{
   countsOut.clear();
   const int num = getNumberOfPaintNames();
   if (num <= 0) {
      return;
   }
   
   countsOut.resize(num, 0);
   
   const int numNodes = getNumberOfNodes();
   const int numCols = getNumberOfColumns();
   
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         countsOut[getPaint(i, j)]++;
      }
   }
}
                                     
/** 
 * Get indices to all paint names used by a column.
 */
void
PaintFile::getPaintNamesForColumn(const int column, std::vector<int>& indices) const
{
   indices.clear();
   std::set<int> invalidPaintIndices;

   const int numNames = getNumberOfPaintNames();
   if (numNames > 0) {
      std::vector<int> namesUsed(numNames, -1);
      const int numNodes = getNumberOfNodes();
      for (int i = 0; i < numNodes; i++) {
         int paintIndex = this->getPaint(i, column);
         if ((paintIndex >= 0) && (paintIndex < numNames)) {
             namesUsed[paintIndex] = 1;
         }
         else {
            invalidPaintIndices.insert(paintIndex);
         }
      }
      
      for (int i = 0; i < numNames; i++) {
         if (namesUsed[i] >= 0) {
            indices.push_back(i);
         }
      }

      if (invalidPaintIndices.empty() == false) {
          std::cout << "Invalid paint indices:";
          for (std::set<int>::iterator iter = invalidPaintIndices.begin();
               iter != invalidPaintIndices.end();
               iter++) {
              std::cout << " " << *iter;
          }
          std::cout << std::endl;
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
 * copy one paint column to another.
 */
void 
PaintFile::copyColumns(const PaintFile* fromPaintFile,
                       const int fromColumnNumber,
                       const int newColumnNumberIn,
                       const QString& newColumnName) throw (FileException)
{
   if (fromPaintFile == NULL) {
      throw FileException("PaintFile::copyColumns()  fromPaintFile is NULL.");
   }
   //
   // Validate column numbers
   //
   if ((fromColumnNumber < 0) ||
       (fromColumnNumber >= fromPaintFile->getNumberOfColumns())) {
      throw FileException("PaintFile::copyColumns() fromColumnNumber is invalid.");
   }
   int newColumnNumber = newColumnNumberIn;
   if ((newColumnNumber < 0) ||
       (newColumnNumber >= getNumberOfColumns())) {
      addColumns(1, fromPaintFile->getNumberOfNodes());
      newColumnNumber = getNumberOfColumns() - 1;
   }
   
   //
   // Copy the metadata
   //
   GiftiMetaData* newColMetaData = getDataArray(newColumnNumber)->getMetaData();
   GiftiMetaData* oldColMetaData = (GiftiMetaData*)fromPaintFile->getDataArray(fromColumnNumber)->getMetaData();
   *newColMetaData = *oldColMetaData;
   
   //
   // Index for copying of paint names
   //
   std::vector<int> paintNameIndexTranslation(fromPaintFile->getNumberOfPaintNames(), -1);
   const int numNodes = getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
      const int paintIndex = fromPaintFile->getPaint(i, fromColumnNumber);
      if (paintIndex >= 0) {
         paintNameIndexTranslation[paintIndex] = -2;
      }
   }
   for (int i = 0; i < static_cast<int>(paintNameIndexTranslation.size()); i++) {
      if (paintNameIndexTranslation[i] == -2) {
         paintNameIndexTranslation[i] = addPaintName(fromPaintFile->getPaintNameFromIndex(i));
      }
   }
   
   //
   // Copy the paint data
   //
   for (int i = 0; i < numNodes; i++) {
      setPaint(i, newColumnNumber, 
                  paintNameIndexTranslation[fromPaintFile->getPaint(i, fromColumnNumber)]);
   }
   
   if (newColumnName.isEmpty() == false) {
      setColumnName(newColumnNumber, newColumnName);
   }
}

/**
 * Dilate a paint column.
 */
void
PaintFile::dilateColumn(const TopologyFile* tf,
                        const int columnNumber,
                        const int iterations) throw (FileException)
{
   //
   // Validate column numbers
   //
   if (tf == NULL) {
      throw FileException("PaintFile::dilateColumn() topology is invalid.");
   }
   if ((columnNumber < 0) ||
       (columnNumber >= getNumberOfColumns())) {
      throw FileException("PaintFile::dilateColumn() columnNumber is invalid.");
   }

   //
   // Get unassigned paint index
   //
   const int unassignedPaintIndex = addPaintName("???");

   //
   // Get topology helper for neighbor information
   //
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);

   //
   // temporary and used to during dilation
   //
   const int numNodes = getNumberOfNodes();
   int* outputPaints = new int[numNodes];

   //
   // Do for specified number of iterations
   //
   for (int iter = 0; iter < iterations; iter++) {
      //
      // Copy current paints
      //
      for (int i = 0; i < numNodes; i++) {
         outputPaints[i] = getPaint(i, columnNumber);
      }

      //
      // Check each node
      //
      for (int i = 0; i < numNodes; i++) {
         //
         // Dilate into neighboring nodes
         //
         const int paintIndex = getPaint(i, columnNumber);
         if (paintIndex != unassignedPaintIndex) {
            int numNeighbors;
            const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
            for (int j = 0; j < numNeighbors; j++) {
               const int neighborNodeNum = neighbors[j];
               if (outputPaints[neighborNodeNum] == unassignedPaintIndex) {
                  outputPaints[neighborNodeNum] = paintIndex;
               }
            }
         }
      }

      //
      // Set current paints
      //
      for (int i = 0; i < numNodes; i++) {
         setPaint(i, columnNumber, outputPaints[i]);
      }
   }

   //
   // Free memory
   //
   delete[] outputPaints;
}

/**
 * dilate paint ID "paintIndex" if neighbors paint index >= 0 do only those.
 */
int 
PaintFile::dilatePaintID(const TopologyFile* tf,
                         const CoordinateFile* cf,
                         const int columnNumber,
                         const int iterations,
                         const int paintIndex,
                         const int neighborOnlyWithPaintIndex,
                         const float maximumExtent[6]) throw (FileException)
{
   int numDilated = 0;
   
   //
   // Validate column numbers
   //
   if ((columnNumber < 0) ||
       (columnNumber >= getNumberOfColumns())) {
      throw FileException("PaintFile::dilatePaintID() columnNumber is invalid.");
   }
   if (iterations <= 0) {
      return numDilated;
   }
   
   const int numNodes = getNumberOfNodes();
   if (numNodes <= 0) {
      return numDilated;
   }
   
   //
   // Get topology helper for neighbor information
   //
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   // temporary and used to during dilation
   //
   int* outputPaints = new int[numNodes];
   
   //
   // Do for specified number of iterations
   //
   for (int iter = 0; iter < iterations; iter++) {
      //
      // Copy current paints
      //
      for (int i = 0; i < numNodes; i++) {
         outputPaints[i] = getPaint(i, columnNumber);
      }
      
      //
      // Check each node
      //
      for (int i = 0; i < numNodes; i++) {
         //
         // does node contain paint that is to be smoothed
         //
         const int paint = getPaint(i, columnNumber);
         if (paint == paintIndex) {
            //
            // Check extent
            //
            const float* nodeXYZ = cf->getCoordinate(i);
            if ((nodeXYZ[0] >= maximumExtent[0]) &&
                (nodeXYZ[0] <= maximumExtent[1]) &&
                (nodeXYZ[1] >= maximumExtent[2]) &&
                (nodeXYZ[1] <= maximumExtent[3]) &&
                (nodeXYZ[2] >= maximumExtent[4]) &&
                (nodeXYZ[2] <= maximumExtent[5])) {
               //
               // get neighbors and loop through them
               //
               int numNeighbors;
               const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
               for (int j = 0; j < numNeighbors; j++) {
                  const int neighborNodeNum = neighbors[j];
                  //
                  // Limited to neighbors with specific paint index?
                  //
                  if (neighborOnlyWithPaintIndex >= 0) {
                     if (getPaint(neighborNodeNum, columnNumber) == neighborOnlyWithPaintIndex) {
                        outputPaints[neighborNodeNum] = paintIndex;
                        numDilated++;
                     }
                  }
                  else {
                     outputPaints[neighborNodeNum] = paintIndex;
                     numDilated++;
                  }
               }
            }
         }
      }

      //
      // Set current paints
      //
      for (int i = 0; i < numNodes; i++) {
         setPaint(i, columnNumber, outputPaints[i]);
      }
   }
   
   delete[] outputPaints;
   
   return numDilated;
}
                         
/**
 * append most common column (if name empty, column is not created).
 */
void 
PaintFile::appendMostCommon(const QString& mostCommonColumnName,
                            const QString& mostCommonExcludeQuestionColumnName) throw (FileException)
{
   const int numNodes = getNumberOfNodes();
   const int numCols = getNumberOfColumns();
   if ((numNodes <= 0) ||
       (numCols <= 0)) {
      return;
   }
   
   if ((mostCommonColumnName.isEmpty() == true) &&
       (mostCommonExcludeQuestionColumnName.isEmpty() == true)) {
      return;
   }
   
   int mostCommonColumn = -1;
   if (mostCommonColumnName.isEmpty() == false) {
      addColumns(1);
      mostCommonColumn = getNumberOfColumns() - 1;
      setColumnName(mostCommonColumn, mostCommonColumnName);
   }
   
   int mostCommonExcludeQuestionColumn = -1;
   if (mostCommonExcludeQuestionColumnName.isEmpty() == false) {
      addColumns(1);
      mostCommonExcludeQuestionColumn = getNumberOfColumns() - 1;
      setColumnName(mostCommonExcludeQuestionColumn, mostCommonExcludeQuestionColumnName);
   }
   
   const int questionPaintIndex = getPaintIndexFromName("???");
   
   for (int i = 0; i < numNodes; i++) {
      //
      // Track use of paint indices for the node
      //
      std::map<int,int> indexCounterMap, indexCounterMapExcludeQuestion;
      
      for (int j = 0; j < numCols; j++) {
         const int paintIndex = getPaint(i, j);
         
         //
         // Doing most common?
         //
         if (mostCommonColumn >= 0) {
            //
            // Update count for paint index
            //
            std::map<int,int>::iterator iter = indexCounterMap.find(paintIndex);
            if (iter != indexCounterMap.end()) {
               iter->second++;
            }
            else {
               indexCounterMap[paintIndex] = 1;
            }
         }
         
         //
         // Doing most common but excluding ??? paints
         //
         if ((mostCommonExcludeQuestionColumn >= 0) &&
             (paintIndex != questionPaintIndex)) {
            //
            // Update count for paint index
            //
            std::map<int,int>::iterator iter = indexCounterMapExcludeQuestion.find(paintIndex);
            if (iter != indexCounterMapExcludeQuestion.end()) {
               iter->second++;
            }
            else {
               indexCounterMapExcludeQuestion[paintIndex] = 1;
            }
         }
      }
      
      //
      // set paint most common index
      //
      if (mostCommonColumn >= 0) {
         int maxIndex = questionPaintIndex;
         int maxCount = -1;
         for (std::map<int,int>::iterator iter = indexCounterMap.begin();
              iter != indexCounterMap.end(); iter++) {
            if (iter->second > maxCount) {
               maxIndex = iter->first;
               maxCount = iter->second;
            }
         }

         setPaint(i, mostCommonColumn, maxIndex);
      }
      
      //
      // set paint most common index
      //
      if (mostCommonExcludeQuestionColumn >= 0) {
         int maxIndex = questionPaintIndex;
         int maxCount = -1;
         for (std::map<int,int>::iterator iter = indexCounterMapExcludeQuestion.begin();
              iter != indexCounterMapExcludeQuestion.end(); iter++) {
            if (iter->second > maxCount) {
               maxIndex = iter->first;
               maxCount = iter->second;
            }
         }

         setPaint(i, mostCommonExcludeQuestionColumn, maxIndex);
      }
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
 * Remove prefixes (chars before first period) and/or suffixes (chars after last period)
 * from all paint names.
 */
void
PaintFile::removePrefixesAndSuffixesFromNames(const bool removePrefixesFlag,
                                              const bool removeSuffixesFlag)
{
    int numPaintNames = this->getNumberOfPaintNames();
    for (int i = 0; i < numPaintNames; i++) {
        QString name = this->getPaintNameFromIndex(i);

        bool nameChangedFlag = false;
        if (removePrefixesFlag) {
            int firstPeriod = name.indexOf(".");
            if (firstPeriod >= 0) {
                name = name.mid(firstPeriod + 1);
                nameChangedFlag = true;
            }
        }

        if (removeSuffixesFlag) {
            int lastPeriod = name.lastIndexOf(".");
            if (lastPeriod >= 0) {
                name = name.left(lastPeriod);
                nameChangedFlag = true;
            }
        }

        if (nameChangedFlag) {
            this->setPaintName(i, name);
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
   
   //const qint64 pos = stream.pos(); //file.pos();
   const qint64 pos = this->getQTextStreamPosition(stream);

   readLine(stream, line);
   
   int fileVersion = 0;
   QString versionTag;
   
   QTextStream(&line, QIODevice::ReadOnly) >> versionTag >> fileVersion;

   if (tagFileVersion != versionTag) {
      fileVersion = 0;
      if (file.seek(pos) == false) {
         std::cout << "ERROR: file.seek("
                   << pos
                   << ") failed  at "
                   << __LINE__
                   << " in "
                   << __FILE__
                   << std::endl;
      }
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
         StudyMetaDataLinkSet smdls;
         smdls.setLinkSetFromCodedText(name);
         setColumnStudyMetaDataLinkSet(indx, smdls);
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
   //file.seek(stream.pos()); // QT 4.2.2
   const qint64 streamPos = this->getQTextStreamPosition(stream);
   file.seek(streamPos);

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
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
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
             << " " << getColumnStudyMetaDataLinkSet(k).getLinkSetAsCodedText().toAscii().constData() << "\n";
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
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
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
               QString filename("");
               if (filenamePrefix.isEmpty() == false) {
                  filename.append(filenamePrefix);
                  filename.append(QDir::separator());
               }
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
      else {
         throw FileException("No file found in directory \""
                             + directoryName
                             + "\" that have file name extension \".label\"");
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

/**
 * Write the file's memory in caret6 format to the specified name.
 */
QString
PaintFile::writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException)
{
   QString name = filenameIn;
   if (useCaret6ExtensionFlag) {
      name = FileUtilities::replaceExtension(filenameIn, ".paint",
                                     SpecFile::getGiftiLabelFileExtension());
   }
   if (colorFileIn != NULL) {
      this->assignColors(*colorFileIn);
   }
   this->setFileWriteType(AbstractFile::FILE_FORMAT_XML_GZIP_BASE64);
   this->writeFile(name);

   return name;
}

/**
 * validate the data arrays (optional for subclasses).
 */
void
PaintFile::validateDataArrays() throw (FileException)
{
   int numNodes = this->getNumberOfNodes();
   int numCols = this->getNumberOfColumns();
   int numberOfPaintNames = this->getNumberOfPaintNames();

   bool negativeIndexCount = 0;
   std::set<int> invalidPaintIndices;
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         int paintIndex = this->getPaint(i, j);
         if (paintIndex >= numberOfPaintNames) {
            invalidPaintIndices.insert(paintIndex);
         }
         else if (paintIndex < 0) {
            negativeIndexCount++;
            this->setPaint(i, j, 0);
         }
      }
   }

   if (negativeIndexCount > 0) {
      std::cout << negativeIndexCount << " Negative Paint Indices changed to zero." << std::endl;
   }

   if (invalidPaintIndices.empty() == false) {
       for (std::set<int>::iterator iter = invalidPaintIndices.begin();
            iter != invalidPaintIndices.end();
            iter++) {
           int indx = *iter;
           QString name("InvalidIndex_" + QString::number(indx));
           this->getLabelTable()->setLabel(indx, name);
           std::cout
                 << "INFO: added paint name "
                 << name.toAscii().constData()
                 << " for invalid index "
                 << indx
                 << std::endl;
       }
   }

   this->clearModified();
}

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


#include <set>
#include <sstream>

#define __GIFTI_NODE_DATA_FILE_MAIN_H__
#include "GiftiNodeDataFile.h"
#undef __GIFTI_NODE_DATA_FILE_MAIN_H__

#include "CommaSeparatedValueFile.h"
#include "CoordinateFile.h"
#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "GiftiCommon.h"
#include "StringTable.h"
#include "StringUtilities.h"

static const QString studyLinkMetaDataTag("tagStudyMetaDataLink");
static const QString studyLinkSetMetaDataTag("tagStudyMetaDataLinkSet");

/**
 * Constructor
 */
GiftiNodeDataFile::GiftiNodeDataFile(const QString& descriptiveName,
                           const QString& defaultDataArrayCategoryIn,
                           const GiftiDataArray::DATA_TYPE defaultDataTypeIn,
                           const int numberOfComponentsPerColumnIn,
                           const QString& defaultExt,
                           const FILE_FORMAT defaultWriteTypeIn,
                           const FILE_IO supportsAsciiFormat,
                           const FILE_IO supportsBinaryFormat,
                           const FILE_IO supportsOtherFormat,
                           const FILE_IO supportsCSVfFormat,
                           const bool dataAreIndicesIntoLabelTableIn)
   : GiftiDataArrayFile(descriptiveName, 
                  defaultDataArrayCategoryIn,
                  defaultDataTypeIn,
                  defaultExt,
                  defaultWriteTypeIn, 
                  supportsAsciiFormat, 
                  supportsBinaryFormat,
                  supportsOtherFormat,
                  supportsCSVfFormat,
                  dataAreIndicesIntoLabelTableIn)
{
   numberOfElementsPerColumn = numberOfComponentsPerColumnIn;
}

/**
 * copy constructor.
 */
GiftiNodeDataFile::GiftiNodeDataFile(const GiftiNodeDataFile& nndf)
   : GiftiDataArrayFile(nndf)
{
   copyHelperGiftiNodeDataFile(nndf);
}
      
/**
 * assignment operator.
 */
GiftiNodeDataFile& 
GiftiNodeDataFile::operator=(const GiftiNodeDataFile& nndf)
{
   if (this != &nndf) {
      GiftiDataArrayFile::operator=(nndf);
      copyHelperGiftiNodeDataFile(nndf);
   }
   
   return *this;
}
      
/**
 * copy helper.
 */
void 
GiftiNodeDataFile::copyHelperGiftiNodeDataFile(const GiftiNodeDataFile& nndf)
{
   numberOfElementsPerColumn = nndf.numberOfElementsPerColumn;
}
      
/**
 * Destructor
 */
GiftiNodeDataFile::~GiftiNodeDataFile()
{
   clear();
}

/**
 * Set the name of a data column.
 */
void
GiftiNodeDataFile::setColumnName(const int col, const QString& name)
{
   setDataArrayName(col, name);
}


/**
 * get a node attribute file column number where input may be a column 
 * name or number.  Input numbers range 1..N and output column 
 * numbers range 0..(N-1).
 */
int 
GiftiNodeDataFile::getColumnFromNameOrNumber(const QString& columnNameOrNumber,
                                             const bool addColumnIfNotFound) throw (FileException)
{
   //
   // Try number first
   //
   
   //
   // To see if it is a number, simply convert to int and check for success
   //
   bool ok = false;
   const int columnNumber = columnNameOrNumber.toInt(&ok);
   if (ok) {
      if ((columnNumber > 0) && 
          (columnNumber <= getNumberOfColumns())) {
         return (columnNumber - 1);
      }
      else {
         if (addColumnIfNotFound && columnNumber - getNumberOfColumns() < 2)
         {//only add column if integer specifies exactly one column further
            if (getNumberOfNodes() > 0)
            {
               addColumns(1);
               const int col = getNumberOfColumns() - 1;
               setColumnName(col, "new column");
               return col;
            } else {
               setNumberOfNodesAndColumns(1, 1);//will not accept 0 for either argument
               setColumnName(0, "new column");
               return 0;
            }
         }//if add flag is false, it tries to find it as a column name
      }
   }
   
   //
   // Try Name
   //
   const int numCols = getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString name = getColumnName(i);
      if (name == columnNameOrNumber) {
         return i;
      }
   }
      
   //
   // Add column if there are nodes
   //
   if (addColumnIfNotFound) {
      if (getNumberOfNodes() > 0) {
         addColumns(1);
         const int col = getNumberOfColumns() - 1;
         setColumnName(col, columnNameOrNumber);
         return col;
      } else {
         setNumberOfNodesAndColumns(1, 1);//will not accept 0 for either argument
         setColumnName(0, columnNameOrNumber);
         return 0;
      }
   }
   
   //
   // failed to find 
   //
   throw FileException("ERROR column name/number " 
                          + columnNameOrNumber
                          + " not found in file "
                          + FileUtilities::basename(getFileName()));
}

/**
 * Get the column index for a column with the specified name.  If the
 * name is not found a negative number is returned.
 */
int
GiftiNodeDataFile::getColumnWithName(const QString& n) const
{
   return getDataArrayWithNameIndex(n);
}

/**
 * Get the name for a column.
 */
QString
GiftiNodeDataFile::getColumnName(const int col) const
{
   if ((col >= 0) && (col < getNumberOfDataArrays())) {
      return getDataArrayName(col);
   }
   return "";
}

/**
 * Set the comment for a data column.
 */
void
GiftiNodeDataFile::setColumnComment(const int col, const QString& comm)
{
   setDataArrayComment(col, comm);
}

/**
 * Append to the comment for a data column.
 */
void
GiftiNodeDataFile::appendToColumnComment(const int col, const QString& comm)
{
   appendToDataArrayComment(col, comm);
}

/**
 * Prepend to the comment for a data column.
 */
void
GiftiNodeDataFile::prependToColumnComment(const int col, const QString& comm)
{
   prependToDataArrayComment(col, comm);
}

/**
 * Get the comment for a column.
 */
QString
GiftiNodeDataFile::getColumnComment(const int col) const
{
   return getDataArrayComment(col);
}

/**
 * get indices to all linked studies.
 */
void 
GiftiNodeDataFile::getPubMedIDsOfAllLinkedStudyMetaData(std::vector<QString>& studyPMIDs) const
{
   std::set<QString> pmidSet;
   const int numColumns= getNumberOfColumns();
   for (int i = 0; i < numColumns; i++) {
      const StudyMetaDataLinkSet smdl = getColumnStudyMetaDataLinkSet(i);
      std::vector<QString> pmids;
      smdl.getAllLinkedPubMedIDs(pmids);
      pmidSet.insert(pmids.begin(), pmids.end());
   }
   studyPMIDs.clear();
   studyPMIDs.insert(studyPMIDs.end(),
                     pmidSet.begin(), pmidSet.end());
}

/**
 * get the study metadata link set for a column.
 */
StudyMetaDataLinkSet
GiftiNodeDataFile::getColumnStudyMetaDataLinkSet(const int columnNumber) const
{
   StudyMetaDataLinkSet smdls;
   if ((columnNumber >= 0) &&
       (columnNumber < getNumberOfDataArrays())) {
      QString s;
      if (dataArrays[columnNumber]->getMetaData()->get(studyLinkSetMetaDataTag, s)) {
         smdls.setLinkSetFromCodedText(s);
      }
      if (dataArrays[columnNumber]->getMetaData()->get(studyLinkMetaDataTag, s)) {
         StudyMetaDataLink smdl;
         smdl.setLinkFromCodedText(s);
         smdls.addStudyMetaDataLink(smdl);
      }
   }
   
   return smdls;
}

/**
 * set the study metadata link for a column.
 */
void 
GiftiNodeDataFile::setColumnStudyMetaDataLinkSet(const int columnNumber,
                                              const StudyMetaDataLinkSet smdls)
{
   if ((columnNumber >= 0) &&
       (columnNumber < getNumberOfDataArrays())) {
      const QString s = smdls.getLinkSetAsCodedText(); 
      dataArrays[columnNumber]->getMetaData()->set(studyLinkSetMetaDataTag, s);
      setModified();
   }
}
      
/**
 *
 */
void
GiftiNodeDataFile::clear()
{
   GiftiDataArrayFile::clear();
}

/**
 * transfer file data.
 */
void 
GiftiNodeDataFile::transferFileDataForDeformation(const DeformationMapFile& dmf,
                                    GiftiNodeDataFile& destinationFile) const
{
/*
   destinationFile.setFileTitle(getFileTitle());
   QString comment("Deformed from: ");
   comment.append(FileUtilities::basename(getFileName()));
   comment.append("\n");
   comment.append("Deformed with: ");
   comment.append(FileUtilities::basename(dmf.getFileName()));
   comment.append("\n");
   comment.append(getFileComment());
   destinationFile.setFileComment(comment);
*/   
   for (int j = 0; j < getNumberOfColumns(); j++) {
      GiftiDataArray* destDataArray = destinationFile.getDataArray(j);
      const GiftiDataArray* myDataArray = getDataArray(j);
      destDataArray->setMetaData(myDataArray->getMetaData());
   }
   for (int j = 0; j < getNumberOfColumns(); j++) {
      QString name(dmf.getDeformedColumnNamePrefix());
      name.append(getColumnName(j));
      destinationFile.setColumnName(j, name);
      
      QString comment(getColumnComment(j));
      if (comment.isEmpty() == false) {
         comment.append("\n");
      }
      comment.append("Deformed with: ");
      comment.append(FileUtilities::basename(dmf.getFileName()));
      destinationFile.setColumnComment(j, comment);
   }
}

/**
 * Deform the metric file
 */
void 
GiftiNodeDataFile::deform(const DeformationMapFile& dmf, 
                   GiftiNodeDataFile& deformedFile,
                   const DEFORM_TYPE dt) const throw (FileException)
{
   //
   // Check deformation file to make sure it contains data
   //
   if (dmf.getNumberOfNodes() <= 0) {
      throw FileException("Deformation map file is isEmpty.");
   }
   
   //
   // Check "this" data file to make sure it has data
   //
   if ((getNumberOfNodes() <= 0) ||
       (getNumberOfColumns() <= 0)) {
      QString msg(filename);
      msg.append(" is isEmpty.");
      throw FileException(msg);
   }
   
   //
   // Make sure deformation map will work with this data file
   //
   int maxNodeNum = -1;
   const int numNodes = dmf.getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
      int nodes[3];
      float areas[3];
      dmf.getDeformDataForNode(i, nodes, areas);
      maxNodeNum = std::max(maxNodeNum, nodes[0]);
      maxNodeNum = std::max(maxNodeNum, nodes[1]);
      maxNodeNum = std::max(maxNodeNum, nodes[2]);
   }
   if (maxNodeNum >= getNumberOfNodes()) {
      std::ostringstream str;
      str << filename.toAscii().constData()
          << "\n has "
          << getNumberOfNodes()
          << " nodes but deformation map expects it to have at least "
          << maxNodeNum
          << " nodes.";
      throw FileException(str.str().c_str());
   }
   
   //
   // Deform the data file.
   //
   deformFile(dmf, deformedFile, dt);
}

/**
 * check for columns with the same name (returns true if there are any).
 */
bool 
GiftiNodeDataFile::checkForColumnsWithSameName(std::vector<QString>& multipleColumnNames) const
{
   checkForDataArraysWithSameName(multipleColumnNames);

   return (multipleColumnNames.size() > 0);
}
      
/**
 * add a data array.
 */
void 
GiftiNodeDataFile::addDataArray(GiftiDataArray* nda)
{
   GiftiDataArrayFile::addDataArray(nda);
}

/**
 * append a data array file to this one.
 */
void 
GiftiNodeDataFile::append(const GiftiDataArrayFile& naf) throw (FileException)
{
   GiftiDataArrayFile::append(naf);
}

/**
 ** append a data array file to this one but selectively load/overwrite arraysumns
 * arrayDestination is where naf's arrays should be (-1=new, -2=do not load)
 * "indexDestination" will be updated with the columns actually used.
 */
void 
GiftiNodeDataFile::append(const GiftiDataArrayFile& naf, 
                          std::vector<int>& indexDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException)
{
   GiftiDataArrayFile::append(naf, indexDestination, fcm);
}

/**
 * add columns to this node data file.
 * If the number of nodes is less than zero the number of nodes in the
 * first dataColumn is used.
 */
void 
GiftiNodeDataFile::addColumns(const int numberOfNewColumns, 
                              const int numberOfNodesIn)
                                  throw (FileException)
{
   int numNodes = numberOfNodesIn;
   if (numNodes < 0) {
      if (dataArrays.empty() == false) {
         numNodes = dataArrays[0]->getNumberOfRows();
      }
   }
   if (numNodes <= 0) {
      throw FileException("Cannot append file, number of nodes is unknown.");
   }
   
   std::vector<int> dim;
   dim.push_back(numNodes);
   if (numberOfElementsPerColumn > 1) {
      dim.push_back(numberOfElementsPerColumn);
   }
   for (int i = 0; i < numberOfNewColumns; i++) {
      addDataArray(new GiftiDataArray(this,
                                      getDefaultDataArrayIntent(),
                                      defaultDataType,
                                      dim));
   }
   
   setModified();
}

/**
 * get the number of nodes in the file.
 */
int 
GiftiNodeDataFile::getNumberOfNodes() const
{
   if (dataArrays.empty()) {
      return 0;
   }
   return dataArrays[0]->getNumberOfRows();
}
      
/**
 * add nodes to this file.
 */
void 
GiftiNodeDataFile::addNodes(const int numberOfNodesToAdd)
{
   if (numberOfNodesToAdd > 0) {
      if (dataArrays.empty()) {
         std::vector<int> dim;
         dim.push_back(numberOfNodesToAdd);
         dim.push_back(numberOfElementsPerColumn);
         addDataArray(new GiftiDataArray(this,
                                         getDefaultDataArrayIntent(),
                                         defaultDataType,
                                         dim));
         
         setModified();
      }
      else {
         addRows(numberOfNodesToAdd);
      }
   }
}

/**
 * get all of the column names.
 */
void 
GiftiNodeDataFile::getAllColumnNames(std::vector<QString>& names) const
{
   getAllArrayNames(names);
}
      
/**
 * set the number of nodes and columns in the file.
 */
void 
GiftiNodeDataFile::setNumberOfNodesAndColumns(const int numNodes, const int numCols,
                                              const int numElementsPerCol)
{
   const int nc = getNumberOfColumns();
   for (int i = 0; i < nc; i++) {
      delete dataArrays[i];
   }
   dataArrays.clear();
   
   numberOfElementsPerColumn = numElementsPerCol;
   if ((numCols > 0) && (numNodes > 0)){
      addColumns(numCols, numNodes);
   }

   setModified();
}

/**
 * reset a data array.
 */
void 
GiftiNodeDataFile::resetDataArray(const int arrayIndex)
{
   GiftiDataArrayFile::resetDataArray(arrayIndex);
}

/**
 * remove a data array.
 */
void 
GiftiNodeDataFile::removeDataArray(const int arrayIndex)
{
   GiftiDataArrayFile::removeDataArray(arrayIndex);
}
      
/**
 * reset a column of data.
 */
void 
GiftiNodeDataFile::resetColumn(const int columnNumber)
{
   resetDataArray(columnNumber);
}

/**
 * remove a column of data.
 */
void 
GiftiNodeDataFile::removeColumn(const int columnNumber)
{
   removeDataArray(columnNumber);
}      
      
/**
 * find out if comma separated file conversion supported.
 */
void 
GiftiNodeDataFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                       bool& writeToCSV) const
{
   readFromCSV = true;
   writeToCSV  = true;
   readFromCSV = true;
   writeToCSV  = true;
}
                                        
/**
 * write the file's data into a comma separated values file (throws exception if not supported).
 */
void 
GiftiNodeDataFile::writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException)
{
   csv.clear();
   
   const int numNodes = getNumberOfNodes();
   const int numFileCols  = getNumberOfColumns();
   if ((numNodes <= 0) || (numFileCols <= 0)) {
      return;
   }
   
   //
   // Determine number columns including multiple elements per column
   //
   int numCols = 0;
   for (int j = 0; j < numFileCols; j++) {
      const GiftiDataArray* gda = getDataArray(j);
      numCols += gda->getNumberOfComponents();      
   }
   
   //
   // Create and add to string table
   //
   StringTable* ct = new StringTable(numNodes, numCols, "Data");
   int colIndex = 0;
   for (int j = 0; j < numFileCols; j++) {
      const GiftiDataArray* gda = getDataArray(j);
      for (int k = 0; k < gda->getNumberOfComponents(); k++) {           
         ct->setColumnTitle(colIndex, getColumnName(j));
         colIndex++;
      }
   }
   
   //
   // See if this is a coordinate file
   //
   const bool coordFileFlag = (dynamic_cast<const CoordinateFile*>(this) != NULL);
   if (coordFileFlag) {
      if (colIndex == 3) {
         ct->setColumnTitle(0, "X");
         ct->setColumnTitle(1, "Y");
         ct->setColumnTitle(2, "Z");
      }
   }

   colIndex = 0;
   for (int j = 0; j < numFileCols; j++) {
      const GiftiDataArray* gda = getDataArray(j);
      
      const int numComp = gda->getNumberOfComponents();
      
         switch (gda->getDataType()) {
            case GiftiDataArray::DATA_TYPE_FLOAT32:
               {
                  const float* data = gda->getDataPointerFloat();
                  for (int i = 0; i < numNodes; i++) {
                     for (int k = 0; k < numComp; k++) {
                        ct->setElement(i, colIndex + k, data[i*numComp+k]);
                     }
                  }
               }
               break;
            case GiftiDataArray::DATA_TYPE_INT32:
               {
                  const int32_t* data = gda->getDataPointerInt();
                  for (int i = 0; i < numNodes; i++) {
                     for (int k = 0; k < numComp; k++) {
                        ct->setElement(i, colIndex + k, data[i*numComp+k]);
                     }
                  }
               }
               break;
            case GiftiDataArray::DATA_TYPE_UINT8:
               {
                  const uint8_t* data = gda->getDataPointerUByte();
                  for (int i = 0; i < numNodes; i++) {
                     for (int k = 0; k < numComp; k++) {
                        ct->setElement(i, colIndex + k, data[i*numComp+k]);
                     }
                  }
               }
               break;
         }
         
      colIndex += numComp;
   }

   //
   // Header
   //
   StringTable* headerTable = new StringTable(0, 0);
   writeHeaderDataIntoStringTable(*headerTable);
   csv.addDataSection(headerTable);
   
   //
   // Labels
   //
   if (getLabelTable()->getNumberOfLabels() > 0) {
      StringTable* tableOfLabels = new StringTable(0, 0);
      getLabelTable()->writeDataIntoStringTable(*tableOfLabels);
      csv.addDataSection(tableOfLabels);
   }
   
   //
   // Column metadata
   //
   for (int i = 0; i < numFileCols; i++) {
      const GiftiDataArray* gda = getDataArray(i);
      StringTable* mt = new StringTable(0, 0);
      gda->getMetaData()->writeDataIntoStringTable(*mt);
      csv.addDataSection(mt);
   }
   
   csv.addDataSection(ct);
}

/**
 * read the file's data from a comma separated values file (throws exception if not supported).
 */
void 
GiftiNodeDataFile::readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException)
{
   clear();
   
   const StringTable* dt = csv.getDataSectionByName("Data");   
   if (dt == NULL) {
      throw FileException("No data found.");
   }

   int numNodes = dt->getNumberOfRows();   
   int numCols = dt->getNumberOfColumns();
   if ((numNodes <= 0) || (numCols <= 0)) {
      throw FileException("Number of rows or columns is zero.");
   }
   
   setNumberOfNodesAndColumns(numNodes, numCols);
   
   std::vector<QString> columnNames(numCols, "");
   
   for (int j = 0; j < numCols; j++) {
      GiftiDataArray* gda = getDataArray(j);
      columnNames[j] = dt->getColumnTitle(j);
      
      switch (gda->getDataType()) {
         case GiftiDataArray::DATA_TYPE_FLOAT32:
            {
               float* data = gda->getDataPointerFloat();
               for (int i = 0; i < numNodes; i++) {
                  data[i] = dt->getElementAsFloat(i, j);
               }
            }
            break;
         case GiftiDataArray::DATA_TYPE_INT32:
            {
               int32_t* data = gda->getDataPointerInt();
               for (int i = 0; i < numNodes; i++) {
                  data[i] = dt->getElementAsInt(i, j);
               }
            }
            break;
         case GiftiDataArray::DATA_TYPE_UINT8:
            {
               uint8_t* data = gda->getDataPointerUByte();
               for (int i = 0; i < numNodes; i++) {
                  data[i] = dt->getElementAsInt(i, j);
               }
            }
            break;
      }
   }
      
   // 
   // Do header
   //    
   const StringTable* stHeader = csv.getDataSectionByName("header");
   if (stHeader != NULL) {
      readHeaderDataFromStringTable(*stHeader);
   }

   //
   // Labels
   //
   const StringTable* stLabels = csv.getDataSectionByName(GiftiCommon::tagLabelTable);
   if (stLabels != NULL) {
      GiftiLabelTable* labels = getLabelTable();
      labels->readDataFromStringTable(*stLabels);
   }
   
   //
   // Look for metadata  Match metadata "Name" with table data column name
   //
   for (int i = 0; i < csv.getNumberOfDataSections(); i++) {
      const StringTable* st = csv.getDataSection(i);
      if (st->getTableTitle() == GiftiCommon::tagMetaData) {
         GiftiMetaData gmd;
         gmd.readDataFromStringTable(*st);
         QString name;
         if (gmd.get("Name", name)) {
            for (int j = 0; j < numCols; j++) {
               if (name == columnNames[j]) {
                  GiftiMetaData* metaData = getDataArray(j)->getMetaData();
                  *metaData = gmd;
               }
            }
         }
      }
   }
}
      
/**
 * read legacy file format data.
 */
void 
GiftiNodeDataFile::readLegacyFileData(QFile& file, 
                                      QTextStream& stream, 
                                      QDataStream& binStream) throw (FileException)
{
   bool dataWasRead = false;
   
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         break;
      case FILE_FORMAT_BINARY:
         break;
      case FILE_FORMAT_XML:
         break;
      case FILE_FORMAT_XML_BASE64:
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         break;      
      case FILE_FORMAT_OTHER:
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            csvf.readFromTextStream(file, stream);
            readDataFromCommaSeparatedValuesTable(csvf);
            dataWasRead = true;
         }
         break;
   }   
   
   if (dataWasRead == false) {
      readLegacyNodeFileData(file, stream, binStream);
   }
}

/**
 * write legacy file format data.
 */
void 
GiftiNodeDataFile::writeLegacyFileData(QTextStream& stream, 
                                       QDataStream& binStream) throw (FileException)
{
   bool dataWasWritten = false;
   
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         break;
      case FILE_FORMAT_BINARY:
         break;
      case FILE_FORMAT_XML:
         break;
      case FILE_FORMAT_XML_BASE64:
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         break;      
      case FILE_FORMAT_OTHER:
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            writeDataIntoCommaSeparatedValueFile(csvf);
            csvf.writeToTextStream(stream);
            dataWasWritten = true;
         }
         break;
   }   
   
   if (dataWasWritten == false) {
      writeLegacyNodeFileData(stream, binStream);
   }
}


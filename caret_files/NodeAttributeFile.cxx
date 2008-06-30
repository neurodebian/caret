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

#include "DeformationMapFile.h"
#include "FileUtilities.h"
#define _NODE_ATTRIBUTE_MAIN_
#include "NodeAttributeFile.h"
#undef _NODE_ATTRIBUTE_MAIN_

#include "StringUtilities.h"

/**
 * Constructor
 */
NodeAttributeFile::NodeAttributeFile(const QString& descriptiveName,
                                     const QString& defaultExt,
                                     const FILE_FORMAT defaultWriteTypeIn,
                                     const FILE_IO supportsAsciiFormat,
                                     const FILE_IO supportsBinaryFormat,
                                     const FILE_IO supportsXMLFormat,
                                     const FILE_IO supportsOtherFormat)
   : AbstractFile(descriptiveName, 
                  defaultExt,
                  true, 
                  defaultWriteTypeIn, 
                  supportsAsciiFormat, 
                  supportsBinaryFormat,
                  supportsXMLFormat, 
                  supportsOtherFormat)
{
   numberOfNodes = 0;
   numberOfColumns = 0;
   numberOfItemsPerColumn = 1;
}

/**
 * Destructor
 */
NodeAttributeFile::~NodeAttributeFile()
{
}

/**
 * Get an offset for indexing into file's data
 */
int
NodeAttributeFile::getOffset(const int nodeNumber, const int columnNumber) const
{
   const int index = (nodeNumber * numberOfColumns * numberOfItemsPerColumn) 
                   + (columnNumber * numberOfItemsPerColumn);
   return index;
}

/**
 * get the specified columns name.
 */
QString 
NodeAttributeFile::getColumnName(const int col) const 
{ 
   if ((col >= 0) && (col < numberOfColumns)) {
      return columnNames[col]; 
   }
   return "";
}

/**
 * Set the name of a data column.
 */
void
NodeAttributeFile::setColumnName(const int col, const QString& name)
{
   columnNames[col] = name;
   setModified();
}

/**
 * get a node attribute file column number where input may be a column 
 * name or number.  Input numbers range 1..N and output column 
 * numbers range 0..(N-1).
 */
int 
NodeAttributeFile::getColumnFromNameOrNumber(const QString& columnNameOrNumber,
                                             const bool addColumnIfNotFoundAndNotNumber) throw (FileException)
{
   //
   // Try name first
   //
   const int numCols = getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      if (getColumnName(i) == columnNameOrNumber) {
         return i;
      }
   }
   
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
         throw FileException("ERROR Invalid column name/number " 
                             + QString::number(columnNumber)
                             + " in file "
                             + FileUtilities::basename(getFileName()));
      }
   }
   
   //
   // Add column if there are nodes
   //
   if (addColumnIfNotFoundAndNotNumber) {
      if (getNumberOfNodes() > 0) {
         addColumns(1);
         const int col = getNumberOfColumns() - 1;
         setColumnName(col, columnNameOrNumber);
         return col;
      }
   }
   
   //
   // faild to find 
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
NodeAttributeFile::getColumnWithName(const QString& n) const
{
   for (int i = 0; i < getNumberOfColumns(); i++) {
      if (getColumnName(i) == n) {
         return i;
      }
   }
   return -1;
}

/**
 * Set the comment for a data column.
 */
void
NodeAttributeFile::setColumnComment(const int col, const QString& comm)
{
   //columnComments[col] = StringUtilities::setupCommentForStorage(comm));
   columnComments[col] = comm;
   setModified();
}

/**
 * Append to the comment for a data column.
 */
void
NodeAttributeFile::appendToColumnComment(const int col, const QString& comm)
{
   if (comm.isEmpty() == false) {
      QString s(getColumnComment(col));
      s.append(comm);
      setColumnComment(col, s);
      setModified();
   }
}

/**
 * Prepend to the comment for a data column.
 */
void
NodeAttributeFile::prependToColumnComment(const int col, const QString& comm)
{
   if (comm.isEmpty() == false) {
      QString s(comm);
      s.append(getColumnComment(col));
      setColumnComment(col, s);
      setModified();
   }
}

/**
 * get indices to all linked studies.
 */
void 
NodeAttributeFile::getPubMedIDsOfAllLinkedStudyMetaData(std::vector<QString>& studyPMIDs) const
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
NodeAttributeFile::getColumnStudyMetaDataLinkSet(const int columnNumber) const
{
   StudyMetaDataLinkSet smdls;
   if ((columnNumber >= 0) &&
       (columnNumber < getNumberOfColumns())) {
      smdls = studyMetaDataLinkSet[columnNumber];
   }
   
   return smdls;
}

/**
 * set the study metadata link for a column.
 */
void 
NodeAttributeFile::setColumnStudyMetaDataLinkSet(const int columnNumber,
                                              const StudyMetaDataLinkSet smdls)
{
   if ((columnNumber >= 0) &&
       (columnNumber < getNumberOfColumns())) {
      studyMetaDataLinkSet[columnNumber] = smdls;
      setModified();
   }
}
      
/**
 * Get the comment for a column.
 */
QString
NodeAttributeFile::getColumnComment(const int col) const
{
   //return StringUtilities::setupCommentForDisplay(columnComments[col]);
   return columnComments[col];
}

/**
 *
 */
void
NodeAttributeFile::numberOfNodesColumnsChanged()
{
   const int oldNumberOfColumns = static_cast<int>(columnNames.size());
   
   if (numberOfColumns == 0) {
      columnNames.clear();
      columnComments.clear();
      studyMetaDataLinkSet.clear();
   }
   else {
      columnNames.resize(numberOfColumns);
      columnComments.resize(numberOfColumns);
      studyMetaDataLinkSet.resize(numberOfColumns);
      for (int i = oldNumberOfColumns; i < numberOfColumns; i++) {
         std::ostringstream str;
         str << "column " << i << " ";
         columnNames[i] = str.str().c_str();
      }
   }
}

/**
 *
 */
void
NodeAttributeFile::clearNodeAttributeFile()
{
   clearAbstractFile();
   columnNames.clear();
   columnComments.clear();
   studyMetaDataLinkSet.clear();
}

/**
 * transfer file data.
 */
void 
NodeAttributeFile::transferFileDataForDeformation(const DeformationMapFile& dmf,
                                    NodeAttributeFile& destinationFile) const
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
   for (int j = 0; j < numberOfColumns; j++) {
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
      
      destinationFile.studyMetaDataLinkSet = studyMetaDataLinkSet;
   }
}

/**
 * Deform the metric file
 */
void 
NodeAttributeFile::deform(const DeformationMapFile& dmf, 
                   NodeAttributeFile& deformedFile,
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
NodeAttributeFile::checkForColumnsWithSameName(std::vector<QString>& multipleColumnNames) const
{
   multipleColumnNames.clear();
   
   if (numberOfColumns > 0) {
      std::set<QString> badNames;
      for (int i = 0; i < (numberOfColumns - 1); i++) {
         for (int j = i + 1; j < numberOfColumns; j++) {
            if (columnNames[i] == columnNames[j]) {
               badNames.insert(columnNames[i]);
            }
         }
      }
      
      if (badNames.empty() == false) {
         multipleColumnNames.insert(multipleColumnNames.begin(),
                                    badNames.begin(), badNames.end());
      }
   }
   
   return (multipleColumnNames.size() > 0);
}
      

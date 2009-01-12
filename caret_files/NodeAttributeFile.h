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



#ifndef __VE_NODE_ATTRIBUTE_FILE_H__
#define __VE_NODE_ATTRIBUTE_FILE_H__

#include "AbstractFile.h"
#include "StudyMetaDataLinkSet.h"

class DeformationMapFile;

/// This abstract class defines some variables and methods used for node attribute files.
class NodeAttributeFile : public AbstractFile {
   public:
      /// deformation type 
      enum DEFORM_TYPE {
         DEFORM_NEAREST_NODE,
         DEFORM_TILE_AVERAGE
      };
      
      /// append column index values
      enum APPEND_COLUMN_INDEX {
         APPEND_COLUMN_NEW = -1,
         APPEND_COLUMN_DO_NOT_LOAD = -2
      };
      
      /// constructor
      NodeAttributeFile(const QString& descriptiveName,
                        const QString& defaultExt = "",
                        const FILE_FORMAT defaultWriteTypeIn = FILE_FORMAT_ASCII,
                        const FILE_IO supportsAsciiFormat = FILE_IO_READ_AND_WRITE,
                        const FILE_IO supportsBinaryFormat = FILE_IO_NONE,
                        const FILE_IO supportsXMLFormat = FILE_IO_NONE,
                        const FILE_IO supportsOtherFormat = FILE_IO_NONE);
      
      /// destructor
      virtual ~NodeAttributeFile();
      
      /// append a node attribute  file to this one
      virtual void append(NodeAttributeFile& naf) throw (FileException) = 0;

      /// append a node attribute file to this one but selectively load/overwrite columns
      /// columnDestination is where naf's columns should be (-1=new, -2=do not load)
      virtual void append(NodeAttributeFile& naf, 
                          std::vector<int> columnDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException) = 0;

      /// add columns to this node attribute file
      virtual void addColumns(const int numberOfNewColumns) = 0;

      /// add nodes to this file
      virtual void addNodes(const int numberOfNodesToAdd) = 0;
      
      /// deform "this" node attribute file placing the output in "deformedFile".
      virtual void deformFile(const DeformationMapFile& dmf, 
                              NodeAttributeFile& deformedFile,
                              const DEFORM_TYPE dt) const throw (FileException) = 0;
      
      /// deform "this" node attribute file placing the output in "deformedFile".
      void deform(const DeformationMapFile& dmf, 
                  NodeAttributeFile& deformedFile,
                  const DEFORM_TYPE dt) const throw (FileException);
      
      /// returns true if the file is isEmpty (contains no data)
      virtual bool empty() const { return (numberOfNodes == 0); }
      
      /// get the number of nodes in the file
      virtual int getNumberOfNodes() const { return numberOfNodes; }
      
      /// get the number of columns in the file
      virtual int getNumberOfColumns() const { return numberOfColumns; }
      
      /// set the number of nodes and columns in the file
      virtual void setNumberOfNodesAndColumns(const int numNodes, const int numCols) = 0;
      
      /// reset a column of data
      virtual void resetColumn(const int columnNumber) = 0;
      
      /// remove a column of data
      virtual void removeColumn(const int columnNumber) = 0;
      
      /// get all of the column names
      void getAllColumnNames(std::vector<QString>& names) const {
         names = columnNames;
      }

      /// get the specified columns name
      QString getColumnName(const int col) const;

      /// get the index of the column with the specified name 
      int getColumnWithName(const QString& n) const;
 
      // get a node attribute file column number where input may be a column 
      // name or number.  Input numbers range 1..N and output column 
      // numbers range 0..(N-1)
      int getColumnFromNameOrNumber(const QString& columnNameOrNumber,
                                    const bool addColumnIfNotFoundAndNotNumber)  throw (FileException);
      
      /// get the comment for a column
      QString getColumnComment(const int col) const;
      
      /// set the name of a column
      void setColumnName(const int col, const QString& name);
      
      /// set the comment for a column
      void setColumnComment(const int col, const QString& comm);
      
      /// append to the comment for a column
      void appendToColumnComment(const int col, const QString& comm);
      
      /// prepend to the comment for a column
      void prependToColumnComment(const int col, const QString& comm);
      
      /// get PubMedID's of all linked studies
      void getPubMedIDsOfAllLinkedStudyMetaData(std::vector<QString>& studyPMIDs) const;
      
      // get the study metadata link for a column
      StudyMetaDataLinkSet getColumnStudyMetaDataLinkSet(const int col) const;
      
      // set the study metadata link for a column
      void setColumnStudyMetaDataLinkSet(const int col,
                                         const StudyMetaDataLinkSet smdls);
      
      /// transfer file data for deformation
      void transferFileDataForDeformation(const DeformationMapFile& dmf,
                                          NodeAttributeFile& destinationFile) const;
      
      /// check for columns with the same name (returns true if there are any)
      bool checkForColumnsWithSameName(std::vector<QString>& multipleColumnNames) const;
      
   protected:
      
      /// get the offset for a data item
      virtual int getOffset(const int nodeNumber, const int columnNumber) const;
      
      /// Deriving classes should call this from its setNumberOfNodesAndColumns() method
      /// anytime numberOfNodes or numberOfColumns are changed so that column names and 
      /// comments are properly allocated.
      void numberOfNodesColumnsChanged();
      
      /// Deriving classes should call this from their "clear()" method.  The first thing this
      /// method will do is call clearAbstractFile().
      void clearNodeAttributeFile();
      
      /// number of nodes in the file
      int numberOfNodes;
      
      /// number of data columns in the file
      int numberOfColumns;
      
      /// number of items per column in the file
      int numberOfItemsPerColumn;
      
      /// names of data columns
      std::vector<QString> columnNames;
      
      /// coments for data columns
      std::vector<QString> columnComments;
      
      /// study metadata link sets
      std::vector<StudyMetaDataLinkSet> studyMetaDataLinkSet;
      
      static const QString tagColumnName;
      static const QString tagColumnComment;
      static const QString tagNumberOfNodes;
      static const QString tagNumberOfColumns;
      static const QString tagColumnStudyMetaData;      
};

#endif // __VE_NODE_ATTRIBUTE_FILE_H__

#ifdef _NODE_ATTRIBUTE_MAIN_
   const QString NodeAttributeFile::tagColumnName    = "tag-column-name";
   const QString NodeAttributeFile::tagColumnComment = "tag-column-comment";
   const QString NodeAttributeFile::tagNumberOfNodes = "tag-number-of-nodes";
   const QString NodeAttributeFile::tagNumberOfColumns = "tag-number-of-columns";
   const QString NodeAttributeFile::tagColumnStudyMetaData = "tag-column-study-meta-data";
#endif // _NODE_ATTRIBUTE_MAIN_

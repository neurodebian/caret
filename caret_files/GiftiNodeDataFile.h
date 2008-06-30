#ifndef __GIFTI_NODE_DATA_FILE_H__
#define __GIFTI_NODE_DATA_FILE_H__

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

#include "GiftiDataArrayFile.h"
#include "StudyMetaDataLinkSet.h"

/// This abstract class defines some variables and methods used for node data files.
class GiftiNodeDataFile : public GiftiDataArrayFile {
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
      
      // destructor
      virtual ~GiftiNodeDataFile();

      // add a data array
      virtual void addDataArray(GiftiDataArray* nda);
            
      // append a data array file to this one
      virtual void append(const GiftiDataArrayFile& naf) throw (FileException);

      // append a data array file to this one but selectively load/overwrite arraysumns
      // arrayDestination is where naf's arrays should be (-1=new, -2=do not load)
      virtual void append(const GiftiDataArrayFile& naf, 
                          std::vector<int>& indexDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException);

      // add columns to this node data file
      void addColumns(const int numberOfNewColumns, const int numberOfNodes = -1) throw (FileException);

      // add nodes to this file
      void addNodes(const int numberOfNodesToAdd);
      
      // Clear the node data file.
      virtual void clear();
      
      // deform "this" node data file placing the output in "deformedFile".
      void deform(const DeformationMapFile& dmf, 
                  GiftiNodeDataFile& deformedFile,
                  const DEFORM_TYPE dt) const throw (FileException);
      
      /// get the number of nodes in the file
      virtual int getNumberOfNodes() const;
      
      /// get the number of columns in the file
      virtual int getNumberOfColumns() const { return dataArrays.size(); }
      
      /// set the number of nodes and columns in the file
      virtual void setNumberOfNodesAndColumns(const int numNodes, const int numCols,
                                              const int numElementsPerCol = 1);
      
      /// reset a column of data
      void resetColumn(const int columnNumber);
      
      /// remove a column of data
      void removeColumn(const int columnNumber);
      
      /// reset a data array
      virtual void resetDataArray(const int arrayIndex);
      
      /// remove a data array
      virtual void removeDataArray(const int arrayIndex);
      
      // get all of the column names
      void getAllColumnNames(std::vector<QString>& names) const;
      
      // get the specified columns name
      QString getColumnName(const int col) const;

      // get the index of the column with the specified name 
      int getColumnWithName(const QString& n) const;
 
      // get a node attribute file column number where input may be a column 
      // name or number.  Input numbers range 1..N and output column 
      // numbers range 0..(N-1)
      int getColumnFromNameOrNumber(const QString& columnNameOrNumber,
                                    const bool addColumnIfNotFoundAndNotNumber)  throw (FileException);
      
      // get the comment for a column
      QString getColumnComment(const int col) const;
      
      // set the name of a column
      void setColumnName(const int col, const QString& name);
      
      // set the comment for a column
      void setColumnComment(const int col, const QString& comm);
      
      // append to the comment for a column
      void appendToColumnComment(const int col, const QString& comm);
      
      // prepend to the comment for a column
      void prependToColumnComment(const int col, const QString& comm);
      
      /// get PubMedID's of all linked studies
      void getPubMedIDsOfAllLinkedStudyMetaData(std::vector<QString>& studyPMIDs) const;
      
      // get the study metadata link for a column
      StudyMetaDataLinkSet getColumnStudyMetaDataLinkSet(const int col) const;
      
      // set the study metadata link for a column
      void setColumnStudyMetaDataLinkSet(const int col,
                                         const StudyMetaDataLinkSet smdls);
      
      // transfer file data for deformation
      void transferFileDataForDeformation(const DeformationMapFile& dmf,
                                          GiftiNodeDataFile& destinationFile) const;
      
      // check for columns with the same name (returns true if there are any)
      bool checkForColumnsWithSameName(std::vector<QString>& multipleColumnNames) const;
      
      /// find out if comma separated file conversion supported
      virtual void getCommaSeparatedFileSupport(bool& readFromCSV,
                                                bool& writeToCSV) const;
                                        
      /// write the file's data into a comma separated values file (throws exception if not supported)
      virtual void writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException);
      
      /// read the file's data from a comma separated values file (throws exception if not supported)
      virtual void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException);
      
   protected:

      /// constructor
      GiftiNodeDataFile(const QString& descriptiveName,
                   const QString& giftiElementNameIn,
                   const GiftiDataArray::DATA_TYPE defaultDataTypeIn,
                   const int numberOfElementsPerColumnIn = 1,
                   const QString& defaultExt = "",
                   const FILE_FORMAT defaultWriteTypeIn = FILE_FORMAT_ASCII,
                   const FILE_IO supportsAsciiFormat = FILE_IO_READ_AND_WRITE,
                   const FILE_IO supportsBinaryFormat = FILE_IO_NONE,
                   const FILE_IO supportsOtherFormat = FILE_IO_NONE,
                   const FILE_IO supportsCSVfFormat = FILE_IO_NONE,
                   const bool dataAreIndicesIntoLabelTableIn = false);
      
      // copy constructor
      GiftiNodeDataFile(const GiftiNodeDataFile& nndf);
      
      // assignment operator
      GiftiNodeDataFile& operator=(const GiftiNodeDataFile& nndf);
      
      // copy helper
      void copyHelperGiftiNodeDataFile(const GiftiNodeDataFile& nndf);

      /// deform "this" node data file placing the output in "deformedFile".
      virtual void deformFile(const DeformationMapFile& dmf, 
                              GiftiNodeDataFile& deformedFile,
                              const DEFORM_TYPE dt) const throw (FileException) = 0;
      
      /// read legacy file format data
      virtual void readLegacyFileData(QFile& file, 
                                      QTextStream& stream, 
                                      QDataStream& binStream) throw (FileException);

      /// write legacy file format data
      virtual void writeLegacyFileData(QTextStream& stream, 
                                       QDataStream& binStream) throw (FileException);

      /// read legacy file format data
      virtual void readLegacyNodeFileData(QFile& file, 
                                          QTextStream& stream, 
                                          QDataStream& binStream) throw (FileException) = 0;

      /// write legacy file format data
      virtual void writeLegacyNodeFileData(QTextStream& stream, 
                                           QDataStream& binStream) throw (FileException) = 0;

      /// number of elements per column
      int numberOfElementsPerColumn;
      
      /// legacy format tag names
      static const QString tagColumnName;

      /// legacy format tag names
      static const QString tagColumnComment;

      //  legacy format tag names
      static const QString tagColumnStudyMetaData;
      
      /// legacy format tag names
      static const QString tagNumberOfNodes;

      /// legacy format tag names
      static const QString tagNumberOfColumns;
};

#ifdef __GIFTI_NODE_DATA_FILE_MAIN_H__
   const QString GiftiNodeDataFile::tagColumnName    = "tag-column-name";
   const QString GiftiNodeDataFile::tagColumnComment = "tag-column-comment";
   const QString GiftiNodeDataFile::tagColumnStudyMetaData = "tag-column-study-meta-data";
   const QString GiftiNodeDataFile::tagNumberOfNodes = "tag-number-of-nodes";
   const QString GiftiNodeDataFile::tagNumberOfColumns = "tag-number-of-columns";
#endif // __GIFTI_NODE_DATA_FILE_MAIN_H__

#endif // __GIFTI_NODE_DATA_FILE_H__


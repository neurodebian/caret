#ifndef __GIFTI_DATA_ARRAY_FILE_H__
#define __GIFTI_DATA_ARRAY_FILE_H__

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

#include "AbstractFile.h"
#include "FileException.h"
#include "GiftiDataArray.h"
#include "GiftiLabelTable.h"

class DeformationMapFile;
class QDataStream;
class QDomDocument;
class QDomElement;
class QFile;
class QTextStream;

/// This abstract class defines some variables and methods used for gifti data array files.
/// While this class may be instantiated, it is best subclassed.
class GiftiDataArrayFile : public AbstractFile {
   public:
      /// append array index values
      enum APPEND_ARRAY_INDEX {
         APPEND_ARRAY_NEW = -1,
         APPEND_ARRAY_DO_NOT_LOAD = -2
      };
      
      /// constructor
      GiftiDataArrayFile(const QString& descriptiveName,
                   const QString& defaultDataArrayCategoryIn,
                   const GiftiDataArray::DATA_TYPE defaultDataTypeIn,
                   const QString& defaultExt,
                   const FILE_FORMAT defaultWriteTypeIn,
                   const FILE_IO supportsAsciiFormat,
                   const FILE_IO supportsBinaryFormat,
                   const FILE_IO supportsOtherFormat,
                   const FILE_IO supportsCSVfFormat,
                   const bool dataAreIndicesIntoLabelTableIn);
      
      /// constructor for generic gifti data array file
      GiftiDataArrayFile();
      
      // copy constructor
      GiftiDataArrayFile(const GiftiDataArrayFile& nndf);
      
      // destructor
      virtual ~GiftiDataArrayFile();

      // assignment operator
      GiftiDataArrayFile& operator=(const GiftiDataArrayFile& nndf);
      
      // add a data array
      virtual void addDataArray(GiftiDataArray* nda);
            
      // add rows to this file.
      void addRows(const int numberOfRowsToAdd);

      // append a data array file to this one
      virtual void append(const GiftiDataArrayFile& naf) throw (FileException);

      // append a data array file to this one but selectively load/overwrite arraysumns
      // arrayDestination is where naf's arrays should be (-1=new, -2=do not load)
      virtual void append(const GiftiDataArrayFile& naf, 
                          std::vector<int>& indexDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException);

      /// compare a file for unit testing (returns true if "within tolerance")
      virtual bool compareFileForUnitTesting(const AbstractFile* af,
                                             const float tolerance,
                                             QString& messageOut) const;
                                     
      // Clear the gifti array data file.
      virtual void clear();
      
      // returns true if the file is isEmpty (contains no data)
      virtual bool empty() const;
      
      /// get the number of data arrays
      int getNumberOfDataArrays() const { return dataArrays.size() ; }
      
      /// get a data array
      GiftiDataArray* getDataArray(const int arrayNumber) { return dataArrays[arrayNumber]; }
      
      /// get a data array (const method)
      const GiftiDataArray* getDataArray(const int arrayNumber) const { return dataArrays[arrayNumber]; }
      
      /// reset a data array
      virtual void resetDataArray(const int arrayIndex);
      
      /// remove a data array
      virtual void removeDataArray(const int arrayIndex);
      
      // get all of the data array names
      void getAllArrayNames(std::vector<QString>& names) const;
      
      // get the specified data array's name
      QString getDataArrayName(const int arrayIndex) const;

      // get the index of the data array with the specified name 
      int getDataArrayWithNameIndex(const QString& n) const;
 
      // get the data array with the specified name 
      GiftiDataArray* getDataArrayWithName(const QString& n);
 
      // get the data array with the specified name 
      const GiftiDataArray* getDataArrayWithName(const QString& n) const;
 
      // get the index of the data array of the specified category
      int getDataArrayWithCategoryIndex(const QString& catName) const;
      
      // get the data array of the specified category
      GiftiDataArray* getDataArrayWithCategory(const QString& catName);
      
      // get the data array of the specified category (const method)
      const GiftiDataArray* getDataArrayWithCategory(const QString& catName) const;
      
      // get the comment for a data array
      QString getDataArrayComment(const int arrayIndex) const;
      
      // set the name of a data array
      void setDataArrayName(const int arrayIndex, const QString& name);
      
      // set the comment for a data array
      void setDataArrayComment(const int arrayIndex, const QString& comm);
      
      // append to the comment for a data array
      void appendToDataArrayComment(const int arrayIndex, const QString& comm);
      
      // prepend to the comment for a data array
      void prependToDataArrayComment(const int arrayIndex, const QString& comm);
      
      // check for data arrays with the same name (returns true if there are any)
      bool checkForDataArraysWithSameName(std::vector<QString>& multipleDataArrayNames) const;
      
      // get the metadata
      GiftiMetaData* getMetaData() { return &metaData; }
      
      // get the metadata (const method)
      const GiftiMetaData* getMetaData() const { return &metaData; }
      
      /// get the label table 
      GiftiLabelTable* getLabelTable() { return &labelTable; }
      
      /// get the label table 
      const GiftiLabelTable* getLabelTable() const { return &labelTable; }
            
      /// see if gifti data array data files are enabled
      static bool getGiftiXMLEnabled() { return giftiXMLFilesEnabled; }
      
      /// set gifti data array files enabled
      static void setGiftiXMLEnabled(const bool b) { giftiXMLFilesEnabled = b; }
      
      /// get the current version for GiftiDataArrayFiles
      static int getCurrentFileVersion() { return currentFileVersion; }
      
      /// get the default data array category
      QString getDefaultDataArrayCategory() const { return defaultDataArrayCategory; }
      
   protected:
      // append helper for files where data are label indices
      void appendLabelDataHelper(const GiftiDataArrayFile& naf,
                                 const std::vector<bool>& arrayWillBeAppended,
                                 std::vector<int>& oldIndicesToNewIndicesTable);
                                 
      // copy helper
      void copyHelperGiftiDataArrayFile(const GiftiDataArrayFile& nndf);
      
      /// read legacy file format data
      virtual void readLegacyFileData(QFile& file, 
                                      QTextStream& stream, 
                                      QDataStream& binStream) throw (FileException);

      /// write legacy file format data
      virtual void writeLegacyFileData(QTextStream& stream, 
                                       QDataStream& binStream) throw (FileException);
      
      // read the XML file 
      virtual void readFileDataXML(QFile& file) throw (FileException);
      
      // write the XML file
      virtual void writeFileDataXML(QTextStream& stream) throw (FileException);
      
      /// Read the contents of the file (header has already been read)
      virtual void readFileData(QFile& file,
                                QTextStream& stream,
                                QDataStream& binStream,
                                QDomElement& rootElement) throw (FileException);

      /// Write the file's data (header has already been written)
      virtual void writeFileData(QTextStream& stream,
                                 QDataStream& binStream,
                                 QDomDocument& xmlDoc,
                                 QDomElement& rootElement) throw (FileException);
                                 
      /// the data arrays
      std::vector<GiftiDataArray*> dataArrays;
      
      /// the label table
      GiftiLabelTable labelTable;
      
      /// the file's metadata
      GiftiMetaData metaData;
      
      /// the default data type
      GiftiDataArray::DATA_TYPE defaultDataType;
      
      /// default data array category for this file
      QString defaultDataArrayCategory;
      
      /// data arrays contain indices into label table
      bool dataAreIndicesIntoLabelTable;
      
      /// gifti XML files enabled
      static bool giftiXMLFilesEnabled;
      
      /// current version of the this file
      static int currentFileVersion;
      
      /*!!!! be sure to update copyHelperGiftiDataArrayFile if new member added !!!!*/
   
   // 
   // friends
   //
};

#ifdef __GIFTI_DATA_ARRAY_FILE_MAIN__
   bool GiftiDataArrayFile::giftiXMLFilesEnabled = false;
   int GiftiDataArrayFile::currentFileVersion = -1;
#endif // __GIFTI_DATA_ARRAY_FILE_MAIN__

#endif // __GIFTI_DATA_ARRAY_FILE_H__


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


#ifndef __VE_ABSTRACT_FILE_H__
#define __VE_ABSTRACT_FILE_H__

#include <map>
#include <QString>
#include <vector>

#include <QDataStream>
#include <QFile>
#include <QStringList>
#include <QTextStream>

#include "FileException.h"

class CommaSeparatedValueFile;
class QDomDocument;
class QDomElement;
class StringTable;
class TransformationMatrix;

//
// In QT4, both file.atEnd() and file.pos() return invalid results
// Needed if QT version is less than 4.2.2
//
//#define QT4_FILE_POS_BUG 1

/// AbstractFile - a base class for all data files.
/**
 * Provides methods that must be implemented by all derived data files and convenience
 * methods for the deriving data files to use.
 *
 * Deriving classes must override these methods
 *   clear()       - must call clearAbstractFile from clear()
 *   isEmpty()       - to find out if there is data in the file
 *   readFileData  - handles reading data after the header
 *   writeFileData - handles writing data after the header
 */
class AbstractFile {
   public:
      /// file format types (not all files support all types)
      enum FILE_FORMAT {
         /// ascii file format
         FILE_FORMAT_ASCII,
         /// binary file format
         FILE_FORMAT_BINARY,
         /// XML file format
         FILE_FORMAT_XML,
         /// XML Base64 file format
         FILE_FORMAT_XML_BASE64,
         /// XML GZip Base64 file format
         FILE_FORMAT_XML_GZIP_BASE64,
         /// Other file format
         FILE_FORMAT_OTHER,
         /// comma separated value file format
         FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE
      };
      
      /// file I/O support
      enum FILE_IO {
         FILE_IO_NONE,
         FILE_IO_READ_ONLY,
         FILE_IO_WRITE_ONLY,
         FILE_IO_READ_AND_WRITE
      };
      
      /// binary operationss
      enum BINARY_OPERATION {
         BINARY_OPERATION_ADD,
         BINARY_OPERATION_SUBTRACT,
         BINARY_OPERATION_MULTIPLY,
         BINARY_OPERATION_DIVIDE,
         BINARY_OPERATION_AVERAGE
      };
      
      /// unary operations
      enum UNARY_OPERATION {
         UNARY_OPERATION_ADD,
         UNARY_OPERATION_ABS_VALUE,
         UNARY_OPERATION_CEILING,
         UNARY_OPERATION_FLOOR,
         UNARY_OPERATION_MULTIPLY,
         UNARY_OPERATION_FIX_NOT_A_NUMBER,
         UNARY_OPERATION_SQUARE_ROOT,
         UNARY_OPERATION_LOG2
      };
      
      /// file comment mode
      enum FILE_COMMENT_MODE {
         FILE_COMMENT_MODE_APPEND,
         FILE_COMMENT_MODE_LEAVE_AS_IS,
         FILE_COMMENT_MODE_REPLACE
      };
      
      /// Destructor (Needs to be public)
      virtual ~AbstractFile();
      
      /// container for storing the file's header
      typedef std::map<QString, QString> AbstractFileHeaderContainer;
      
      /// Clears current file data in memory.  Deriving classes must override this method and
      /// call AbstractFile::clearAbstractFile() from its clear method.
      virtual void clear() = 0;
      
      /// returns true if the file is isEmpty (contains no data)
      virtual bool empty() const = 0;
      
      /// returns non-zero if the file has been modified without being saved
      unsigned long getModified() const;
      
      /// set file has been modified
      void setModified();

      /// set modified counter
      void setModifiedCounter(const unsigned long value);
      
      /// clear file has been modified without being saved
      void clearModified();
      
      /// get the name of the file (description only used if file name is isEmpty)
      virtual QString getFileName(const QString& description = "") const;

      /// get the name of the file without any path (description only used if file name is isEmpty)
      virtual QString getFileNameNoPath(const QString& description = "") const;
      
      /// set the name of the file
      void setFileName(const QString name) { filename = name; }

      /// get the default file name (also overrides current file name)
      QString makeDefaultFileName(const QString& description = "") const;
      
      /// replace the caret standard file name's description
      void replaceFileNameDescription(const QString& newDescription);
      
      /// get the path of the file
      QString getFileNamePath() const;
      
      /// get the file's comment
      QString getFileComment() const;
      
      /// set the file's comment
      void setFileComment(const QString& fc);
      
      /// get the file's PubMed ID
      QString getFilePubMedID() const;
      
      /// set the file's PubMed ID
      void setFilePubMedID(const QString& pmid);
      
      /// append to the file's comment
      void appendToFileComment(const QString& fc);
      
      /// append software version to file comment
      void appendSoftwareVersionToFileComment(const QString& msg);
      
      /// get the file's title
      QString getFileTitle() const { return fileTitle; }
      
      /// set the file's title
      void setFileTitle(const QString& ft);
      
      /// get the header (note: comment is stored in header).
      AbstractFile::AbstractFileHeaderContainer getHeader() const { return header; }
      
      /// replace the header (note: comment is stored in header)
      void setHeader(const AbstractFile::AbstractFileHeaderContainer& hdr) { header = hdr; }
      
      /// get a header tag value
      QString getHeaderTag(const QString& name) const;
      
      /// set a header tag value
      void setHeaderTag(const QString& name, const QString& value);
      
      /// remove header tag
      void removeHeaderTag(const QString& name);
      
      /// Append a header tag.
      void appendToHeaderTag(const QString& nameIn, const QString& value,
                             const QString& separator);
                                
      /// see if the file has a header
      bool getFileHasHeader() const { return fileHasHeader; }
      
      /// set file has header
      void setFileHasHeader(const bool hh) { fileHasHeader = hh; }
      
      /// Read a file tag's line  (form: tag value)
      static void readTagLine(const QString& filenameIn, QTextStream& stream , QString& tag, 
                              QString& tagValue) throw (FileException);
      
      /// read a line and parse into whitespace separated tokens
      /// filenameIn is used if an exception is thrown
      static void readLineIntoTokens(const QString& filenameIn,
                              QTextStream& stream,
                              QString& lineOut,
                              std::vector<QString>& tokens) throw (FileException);
      
      /// Read a line into a string list
      QStringList readLineIntoStringList(QTextStream& stream) throw (FileException);

      /// Read a line from a file
      static void readLine(const QString& filenameIn, QTextStream& stream, 
                           QString& lineOut) throw (FileException);

      /// read the specified file but just the metadata, NO DATA
      void readFileMetaDataOnly(const QString& filenameIn)  throw (FileException);
      
      /// read the specified file (override for files without headers, ie: binary)
      virtual void readFile(const QString& filenameIn)  throw (FileException);
      
      /// read the contents of a file stored in memory
      virtual void readFileFromArray(const char* data, 
                                     const unsigned int dataLength,
                                     const QString& debugFileName) throw (FileException);
      
      /// Write the current file's memory to the specified name
      // (override for files without headers, ie: binary)
      virtual void writeFile(const QString& filenameIn) throw (FileException);
      
      /// Write the current file's memory to a byte array
      virtual void writeFileToArray(QByteArray& ba) throw (FileException);
      
      /// Read any subclass of AbstractFile
      static AbstractFile* readAnySubClassDataFile(const QString& filenameIn,
                                                   const bool readMetaDataOnly,
                                                   QString& errorMessageOut);
      
      /// Get a file type name based upon its extension.  If the file's type is not
      /// recognized, the file name's extension is returned.
      static QString getFileTypeNameFromFileName(const QString& filename);

      /// Create a subclass of abstract file based upon the file name's extension.
      /// Returns NULL if not recognized.
      static AbstractFile* getSubClassDataFile(const QString& filename,
                                               QString& errorMessageOut);

      /// Get all filetype names and extensions.
      static void getAllFileTypeNamesAndExtensions(std::vector<QString>& typeNames,
                                            std::vector<QString>& extensions);
                                 
      /// tag for comment
      static QString getHeaderTagComment() { return headerTagComment; }
      
      /// tag for PubMed ID
      static QString getHeaderTagPubMedID() { return headerTagPubMedID; }
      
      /// see if the format can be read
      bool getCanRead(const FILE_FORMAT ff) const;
      
      /// see if the format can be writte
      bool getCanWrite(const FILE_FORMAT ff) const;
      
      /// get the data type of file for writing
      FILE_FORMAT getFileWriteType() const { return fileWriteType; }
      
      /// get the data type of file that was read
      FILE_FORMAT getFileReadType() const { return fileReadType; }
      
      /// set read/write type for a format
      void setFileReadWriteType(const FILE_FORMAT format, const FILE_IO readAndOrWrite);
      
      /// set the data type of file for writing
      void setFileWriteType(const FILE_FORMAT wt) { fileWriteType = wt; }
      
      /// set the data type of file for reading
      void setFileReadType(const FILE_FORMAT wt) { fileReadType = wt; }
      
      /// get the descriptive name
      QString getDescriptiveName() const { return descriptiveName; }
      
      /// get the display list number (negative if invalid)
      unsigned int getDisplayListNumber() const { return displayListNumber; }
      
      /// set the display list number
      void setDisplayListNumber(const unsigned int dln) { displayListNumber = dln; }
      
      /// clear the display list
      void clearDisplayList();
      
      /// get the number of digits right of the decimal when writing float to text files
      static int getTextFileDigitsRightOfDecimal()
                       { return textFileDigitsRightOfDecimal; }
                       
      /// set the number of digits right of the decimal when writing float to text files
      static void setTextFileDigitsRightOfDecimal(const int num); 
                       
      /// get the read meta data only flag
      bool getReadMetaDataOnlyFlag() const { return readMetaDataOnlyFlag; }
      
      /// get the default file name extension
      QString getDefaultFileNameExtension() const { return defaultExtension; }
      
      /// set the default file name extension
      void setDefaultFileNameExtension(const QString& s) { defaultExtension = s; }
      
      /// get the preferred write type
      static FILE_FORMAT getPreferredWriteType() 
         { return preferredWriteType; }

      /// set the preferred write type
      static void setPreferredWriteType(const FILE_FORMAT t)
                                             { preferredWriteType = t; }
      
      /// set the default file name prefix and number of nodes
      static void setDefaultFileNamePrefix(const QString& s,
                                           const int numNodes);
      
      /// get the default file name prefix
      static void getDefaultFileNamePrefix(QString& prefix, int& numNodes);
      
      /// set the root element name
      void setRootXmlElementTagName(const QString& s) { rootXmlElementTagName = s; }
            
      /// get the transformation matrix associated with this file
      const TransformationMatrix* getAssociatedTransformationMatrix() const 
                                                  { return transMatrix; }
      
      /// set the transformation matrix associated with this file
      void setAssociatedTransformationMatrix(TransformationMatrix* tm)
                        { transMatrix = tm; }
         
      /// Get an XML element's first child and return it as a string.
      static QString getXmlElementFirstChildAsString(const QDomElement& elem);

      /// Get an XML element's first child and return it as a float.
      static float getXmlElementFirstChildAsFloat(const QDomElement& elem);

      /// Get an XML element's first child and return it as a double.
      static double getXmlElementFirstChildAsDouble(const QDomElement& elem);

      /// Get an XML element's first child and return it as an int.
      static int getXmlElementFirstChildAsInt(const QDomElement& elem);

      /// Get an XML element's first child and return it as a float.
      static void getXmlElementFirstChildAsFloat(const QDomElement& elem,
                                                 float* values,
                                                 const int numValues);

      /// Get an XML element's first child and return it as an int.
      static void getXmlElementFirstChildAsInt(const QDomElement& elem,
                                               int* values,
                                               const int numValues);

      /// add a text CDATA element to an XML Dom Element
      static void addXmlCdataElement(QDomDocument& xmlDoc,
                                     QDomElement& parentElement,
                                     const QString& childElementName,
                                     const QString& childElementData);
                              
      /// add a Text element to an XML Dom Element
      static void addXmlTextElement(QDomDocument& xmlDoc,
                                    QDomElement& parentElement,
                                    const QString& childElementName,
                                    const QString& childElementData);
                              
      /// add a float element to an XML Dom Element
      static void addXmlTextElement(QDomDocument& xmlDoc,
                                    QDomElement& parentElement,
                                    const QString& childElementName,
                                    const float childElementData);
                              
      /// add a float element to an XML Dom Element
      static void addXmlTextElement(QDomDocument& xmlDoc,
                                    QDomElement& parentElement,
                                    const QString& childElementName,
                                    const double childElementData);
                              
      /// add a Text element to an XML Dom Element
      static void addXmlTextElement(QDomDocument& xmlDoc,
                                    QDomElement& parentElement,
                                    const QString& childElementName,
                                    const float* childElementData,
                                    const int numChildElements);
                              
      /// add a int element to an XML Dom Element
      static void addXmlTextElement(QDomDocument& xmlDoc,
                                    QDomElement& parentElement,
                                    const QString& childElementName,
                                    const int childElementData);
                              
      /// add a Text element to an XML Dom Element
      static void addXmlTextElement(QDomDocument& xmlDoc,
                                    QDomElement& parentElement,
                                    const QString& childElementName,
                                    const int* childElementData,
                                    const int numChildElements);
       
      /// find out if comma separated file conversion supported
      virtual void getCommaSeparatedFileSupport(bool& readFromCSV,
                                                bool& writeToCSV) const;
      
      /// write the file's data into a comma separated values file (throws exception if not supported)
      virtual void writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException);
      
      /// read the file's data from a comma separated values file (throws exception if not supported)
      virtual void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException);
      
      /// compare a file for unit testing (returns true if "within tolerance")
      virtual bool compareFileForUnitTesting(const AbstractFile* af,
                                             const float tolerance,
                                             QString& messageOut) const;
                                     
      // allow events to process (call when doing "busy" operations
      static void allowEventsToProcess();
      
      /// get the time (in seconds) that it took to read the file
      float getTimeToReadFileInSeconds() const { return timeToReadFileInSeconds; }
      
      // generate a unique timestamp that is all numeric 
      static QString generateUniqueNumericTimeStampAsString();
      
      // generate a date and time timestamp
      static QString generateDateAndTimeStamp();
      
   protected:    
      /// Constructor  
      AbstractFile(const QString& descriptiveName,
                   const QString& defaultExtensionIn,
                   const bool fileHasHeaderIn = true,
                   const FILE_FORMAT defaultWriteTypeIn = FILE_FORMAT_ASCII,
                   const FILE_IO supportsAsciiFormat = FILE_IO_READ_AND_WRITE,
                   const FILE_IO supportsBinaryFormat = FILE_IO_NONE,
                   const FILE_IO supportsXMLFormat = FILE_IO_NONE,
                   const FILE_IO supportsXMLBase64Format = FILE_IO_NONE,
                   const FILE_IO supportsXMLGZipBase64Format = FILE_IO_NONE,
                   const FILE_IO supportsOtherFormat = FILE_IO_NONE,
                   const FILE_IO supportsCsvfFormat = FILE_IO_NONE);
      
      /// Copy Constructor
      AbstractFile(const AbstractFile& af);
      
      /// equality operator
      AbstractFile& operator=(const AbstractFile& af);
      
      // append the files comment
      void appendFileComment(const AbstractFile& af,
                             const FILE_COMMENT_MODE fcm = FILE_COMMENT_MODE_APPEND);
         
      /// copy an abstract file to this one (used by copy constructor and operator=)
      void copyHelperAbstractFile(const AbstractFile& af);
      
      /// set the read meta data only flag
      void setReadMetaDataOnlyFlag(const bool flag) { readMetaDataOnlyFlag = flag; }
      
      /// set enable/disable appending to file comment
      void setEnableAppendFileComment(const bool en) { enableAppendFileComment = en; }

      /// get enable/disable appending to file comment
      bool getEnableAppendFileComment() const { return enableAppendFileComment; }

      /// clear the abstract file's members
      void clearAbstractFile();
   
      /// read header data from an XML file
      void readHeaderXML(QDomElement& rootElement);
      
      /// write header data to an XML file
      void writeHeaderXML(QDomDocument& doc, QDomElement& rootElement);
      
      /// Read the contents of the file (header has already been read)
      virtual void readFileData(QFile& file, 
                                QTextStream& stream,
                                QDataStream& binStream,
                                QDomElement& rootElement) throw (FileException) = 0;
      
      /// Write the file's data (header has already been written)
      virtual void writeFileData(QTextStream& stream,
                                 QDataStream& binStream,
                                 QDomDocument& xmlDoc,
                                 QDomElement& rootElement) throw (FileException) = 0;

      // read a 3 byte integer from binary file
      int readThreeByteInteger(QDataStream& binStream) throw (FileException);
      
      /// Read a file tag's line  (form: tag value)
      void readTagLine(QTextStream& stream, QString& tag, QString& tagValue) 
              throw (FileException);
      
      /// Read a file tag's line  (form: tag value)
      void readTagLine(QTextStream& stream, QString& line, 
                       QString& tag, QString& tagValue) 
              throw (FileException);
      
      /// Read a numbered file tag's line  (form: tag value)
      void readNumberedTagLine(QTextStream& stream, int& number,
                               QString& tag, QString& tagValue) 
                                                       throw (FileException);
      
      /// Read a line from a file
      void readLine(QTextStream& stream, QString& lineOut) throw (FileException);

      /// read a line and chop off the comments. 
      void readLineChopComment(QTextStream& stream,
                               QString& lineOut) throw (FileException);
       
      /// read a line and parse into whitespace separated tokens
      void readLineIntoTokens(QTextStream& stream,
                              QString& lineOut,
                              std::vector<QString>& tokens) throw (FileException);
      
      /// split a tag that begins with column number into number and value
      int splitTagIntoColumnAndValue(const QString s, QString& value) const;
      
      /// read the data file's header.
      void readHeader(QFile& file, QTextStream& stream) throw(FileException);
  
      /// write the data file's header
      void writeHeader(QTextStream& stream) throw (FileException);

      /// write the header's data into a StringTable
      void writeHeaderDataIntoStringTable(StringTable& table) const;
      
      /// read the header's data from a StringTable
      void readHeaderDataFromStringTable(const StringTable& table) throw (FileException);
      
      /// find the binary data offset for QT4 bug work around
      qint64 findBinaryDataOffsetQT4Bug(QFile& file,
                                        const char* stringToFind) throw (FileException);
      
      /// set file postion for binary files for QT4 bug work around
      void setBinaryFilePosQT4Bug() throw (FileException);

      /// Name that was passed to read method
      mutable QString filename;
      
      /// the header
      AbstractFileHeaderContainer header;
      
      /// the file's title
      QString fileTitle;
      
      /// name for root xml element
      QString rootXmlElementTagName;
      
      /// QFile used for writing files
      QFile* writingQFile;
      
   private:
      /// determine if the file is an XML file
      bool isFileXML(QFile& file);
      
      /// determine if the file is a comma separated value file
      bool isCommaSeparatedValueFile(QFile& file);
      
      /// method used while reading files
      void readFileContents(QFile& file) throw (FileException);
      
      /// method used for writing files
      void writeFileContents(QTextStream& stream, QDataStream& dataStream) throw (FileException);
      
      /// contents modified (incremented each modification)
      unsigned long modified;
      
      /// display list number (do not clear)
      unsigned int displayListNumber;
      
      /// supports ascii format files (do not clear)
      FILE_IO fileSupportAscii;
      
      /// supports binary format files (do not clear)
      FILE_IO fileSupportBinary;
      
      /// supports xml format files (do not clear)
      FILE_IO fileSupportXML;
      
      /// supports xml base64 encodedformat files (do not clear)
      FILE_IO fileSupportXMLBase64;
      
      /// supports xml gzipped base64 format files (do not clear)
      FILE_IO fileSupportXMLGZipBase64;
      
      /// supports comma separated value format (do not clear)
      FILE_IO fileSupportCommaSeparatedValueFile;
      
      /// supports other format file (do not clear)
      FILE_IO fileSupportOther;
      
      /// data type of file for writing (do not clear)
      FILE_FORMAT fileWriteType;
      
      /// data type of file that was read (do not clear)
      FILE_FORMAT fileReadType;
      
      /// the descriptive name (do not clear)
      QString descriptiveName;
      
      /// the default extension (do not clear)
      QString defaultExtension;
      
      /// the default name (do not clear)
      QString defaultFileName;
      
      /// file has a header (do not clear)
      bool fileHasHeader;
      
      /// enable/disable append to file comment
      bool enableAppendFileComment;

      /// if set before reading, only the meta data is read, no data
      bool readMetaDataOnlyFlag;
      
      /// unique number for the file
      int uniqueFileNumber;
      
      /// matrix associated with this file
      TransformationMatrix* transMatrix;
      
      /// append version id when files written
      static bool appendVersionIdWhenFilesWritten;
      
      /// write to this data type whenever possible
      static FILE_FORMAT preferredWriteType;
      
      /// the number of digits right of the decimal when writing float to text files
      static int textFileDigitsRightOfDecimal;
      
      /// default file name prefix
      static QString defaultFileNamePrefix;
      
      /// default file name number of nodes
      static int defaultFileNameNumberOfNodes;
      
      /// the unique file naming counter
      static int uniqueFileNameCounter;
      
   protected:
      /// time to read the file in seconds
      float timeToReadFileInSeconds;
      
   protected:
      static const QString tagFileTitle;
      static const QString tagFileVersion;
      static const QString tagBeginData;
      static const QString xmlHeaderTagName;
      static const QString xmlHeaderElementTagName;
      static const QString xmlHeaderElementName;
      static const QString xmlHeaderElementValue;
      static const QString xmlHeaderOldTagName;
      static const QString headerTagPubMedID;
      static const QString headerTagComment;  // keep this private
                                                  // use get/setHeaderComment
   public:
      static const QString headerTagEncodingValueAscii;
      static const QString headerTagEncodingValueBinary;
      static const QString headerTagEncodingValueXML;
      static const QString headerTagEncodingValueXMLBase64;
      static const QString headerTagEncodingValueXMLGZipBase64;
      static const QString headerTagEncodingValueOther;
      static const QString headerTagEncodingValueCommaSeparatedValueFile;
      static const QString headerTagConfigurationID;
      static const QString headerTagCoordFrameID;
      static const QString headerTagDate;
      static const QString headerTagEncoding;
      static const QString headerTagOrientation;
      static const QString headerTagPerimeterID;
      static const QString headerTagResolution;
      static const QString headerTagSampling;
      static const QString headerTagScale;
      static const QString headerTagVersionID;

      static const QString headerTagSpace;
      static const QString headerTagStructure;
      static const QString headerTagSpecies;
      static const QString headerTagCategory;
      static const QString headerTagSubject;
};

#ifdef _ABSTRACT_MAIN_

   const QString AbstractFile::xmlHeaderTagName = "FileHeader";
   const QString AbstractFile::xmlHeaderElementTagName = "Element";
   const QString AbstractFile::xmlHeaderElementName = "Name";
   const QString AbstractFile::xmlHeaderElementValue = "Value";
   const QString AbstractFile::xmlHeaderOldTagName = "header";
   const QString AbstractFile::tagFileTitle   = "tag-title";
   const QString AbstractFile::tagFileVersion = "tag-version";
   const QString AbstractFile::tagBeginData   = "tag-BEGIN-DATA";
   const QString AbstractFile::headerTagPubMedID = "pubmed_id";
   const QString AbstractFile::headerTagComment          = "comment";
   const QString AbstractFile::headerTagConfigurationID = "configuration_id";
   const QString AbstractFile::headerTagCoordFrameID    = "coordframe_id";
   const QString AbstractFile::headerTagDate             = "date";
   const QString AbstractFile::headerTagEncoding         = "encoding";
   const QString AbstractFile::headerTagOrientation      = "orientation";
   const QString AbstractFile::headerTagPerimeterID      = "perimeter_id";
   const QString AbstractFile::headerTagResolution       = "resolution";
   const QString AbstractFile::headerTagSampling         = "sampling";
   const QString AbstractFile::headerTagScale            = "scale";
   const QString AbstractFile::headerTagVersionID        = "version_id";
   
   const QString AbstractFile::headerTagSpace        = "space";
   const QString AbstractFile::headerTagStructure    = "structure";
   const QString AbstractFile::headerTagSpecies      = "species";
   const QString AbstractFile::headerTagCategory     = "category";
   const QString AbstractFile::headerTagSubject      = "subject";
      
   const QString AbstractFile::headerTagEncodingValueAscii  = "ASCII";
   const QString AbstractFile::headerTagEncodingValueBinary = "BINARY";
   const QString AbstractFile::headerTagEncodingValueXML    = "XML";
   const QString AbstractFile::headerTagEncodingValueXMLBase64    = "XML_BASE64";
   const QString AbstractFile::headerTagEncodingValueXMLGZipBase64    = "XML_GZIP_BASE64";
   const QString AbstractFile::headerTagEncodingValueOther  = "OTHER";
   const QString AbstractFile::headerTagEncodingValueCommaSeparatedValueFile = "COMMA_SEPARATED_VALUE_FILE";
   int AbstractFile::textFileDigitsRightOfDecimal = 6;
   AbstractFile::FILE_FORMAT AbstractFile::preferredWriteType =
                                       AbstractFile::FILE_FORMAT_OTHER; 
   bool AbstractFile::appendVersionIdWhenFilesWritten = false;
   QString AbstractFile::defaultFileNamePrefix = "";
   int AbstractFile::defaultFileNameNumberOfNodes = 0;
   int AbstractFile::uniqueFileNameCounter = 0;
   
#endif //  _ABSTRACT_MAIN_


#endif //  __VE_ABSTRACT_FILE_H__


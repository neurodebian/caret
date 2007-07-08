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

#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>
#include <QFile>
#include <QGLWidget>
#include <QImageReader>

#define _ABSTRACT_MAIN_
#include "AbstractFile.h"
#undef _ABSTRACT_MAIN_

#include "DebugControl.h"
#include "FileUtilities.h"
#include "StringUtilities.h"

#include "GiftiDataArrayFile.h"

#ifdef CARET_FLAG
#include "AreaColorFile.h"
#include "ArealEstimationFile.h"
#include "AtlasSpaceFile.h"
#include "AtlasSurfaceDirectoryFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BorderProjectionFile.h"
#include "BrainVoyagerFile.h"
#include "CaretVersion.h"
#include "CellColorFile.h"
#include "CellFile.h"
#include "CellProjectionFile.h"
#include "CocomacConnectivityFile.h"
#include "ContourCellColorFile.h"
#include "ContourCellFile.h"
#include "ContourFile.h"
#include "CoordinateFile.h"
#include "CutsFile.h"
#include "DeformationFieldFile.h"
#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "FociColorFile.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "FreeSurferCurvatureFile.h"
#include "FreeSurferFunctionalFile.h"
#include "FreeSurferLabelFile.h"
#include "FreeSurferSurfaceFile.h"
#include "GiftiNodeDataFile.h"
#include "GeodesicDistanceFile.h"
#include "ImageFile.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "PaletteFile.h"
#include "ParamsFile.h"
#include "PreferencesFile.h"
#include "ProbabilisticAtlasFile.h"
#include "RgbPaintFile.h"
#include "SceneFile.h"
#include "SectionFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "SurfaceVectorFile.h"
#include "TopographyFile.h"
#include "TopologyFile.h"
#include "TransformationMatrixFile.h"
#include "VectorFile.h"
#include "VolumeFile.h"
#include "VtkModelFile.h"
#include "WustlRegionFile.h"
#endif // CARET_FLAG

const int commentLengthWarning = 20000;

/**
 * The constructor
 */
AbstractFile::AbstractFile(const QString& descriptiveNameIn,
                           const QString& defaultExtensionIn,
                           const bool fileHasHeaderIn,
                           const FILE_FORMAT defaultWriteTypeIn,
                           const FILE_IO supportsAsciiFormat,
                           const FILE_IO supportsBinaryFormat,
                           const FILE_IO supportsXMLFormat,
                           const FILE_IO supportsXMLBase64Format,
                           const FILE_IO supportsXMLGZipBase64Format,
                           const FILE_IO supportsOtherFormat)
{
   writingQFile = NULL;
   
   //
   // Update copy constructor too
   //
   uniqueFileNumber = uniqueFileNameCounter;
   uniqueFileNameCounter++;
   descriptiveName = descriptiveNameIn;
   rootXmlElementTagName = StringUtilities::replace(descriptiveName, ' ', '_');
   defaultExtension = defaultExtensionIn;
   fileHasHeader       = fileHasHeaderIn;
   fileReadType        = defaultWriteTypeIn;
   fileWriteType       = defaultWriteTypeIn;
   fileSupportAscii    = supportsAsciiFormat;
   fileSupportBinary   = supportsBinaryFormat;
   fileSupportXML      = supportsXMLFormat;
   fileSupportXMLBase64 = supportsXMLBase64Format;
   fileSupportXMLGZipBase64 = supportsXMLGZipBase64Format;
   fileSupportOther    = supportsOtherFormat;

   displayListNumber = 0;

   defaultFileName = StringUtilities::makeLowerCase(descriptiveName);
   defaultFileName = StringUtilities::replace(defaultFileName, ' ', '_');

   enableAppendFileComment = true;
   
   readMetaDataOnlyFlag = false;

   //
   // Set write type and possibly override with the preferred write type
   //
   //fileWriteType = fileReadType;
   if (preferredWriteType != fileWriteType) {
      switch (preferredWriteType) {
         case FILE_FORMAT_ASCII:
            if (getCanWrite(FILE_FORMAT_ASCII)) {
               fileWriteType = preferredWriteType;
            }
            break;
         case FILE_FORMAT_BINARY:
            if (getCanWrite(FILE_FORMAT_BINARY)) {
               fileWriteType = preferredWriteType;
            }
            break;
         case FILE_FORMAT_XML:
            if (getCanWrite(FILE_FORMAT_XML)) {
               fileWriteType = preferredWriteType;
            }
            break;
         case FILE_FORMAT_XML_BASE64:
            if (getCanWrite(FILE_FORMAT_XML_BASE64)) {
               fileWriteType = preferredWriteType;
            }
            break;
         case FILE_FORMAT_XML_GZIP_BASE64:
            if (getCanWrite(FILE_FORMAT_XML_GZIP_BASE64)) {
               fileWriteType = preferredWriteType;
            }
            break;
         case FILE_FORMAT_OTHER:
            if (getCanWrite(FILE_FORMAT_OTHER)) {
               fileWriteType = preferredWriteType;
            }
            break;
      }
   }
}

/**
 * The copy constructor
 */
AbstractFile::AbstractFile(const AbstractFile& af)
{
   copyHelperAbstractFile(af);
}

/**
 * equality operator.
 */
AbstractFile&
AbstractFile::operator=(const AbstractFile& af)
{
   if (this != &af) {
      copyHelperAbstractFile(af);
   }
   
   return *this;
}
      
/**
 * copy an abstract file to this one (used by copy constructor and operator=).
 */
void 
AbstractFile::copyHelperAbstractFile(const AbstractFile& af)
{
   clearModified();
   uniqueFileNumber = uniqueFileNameCounter;
   uniqueFileNameCounter++;
   displayListNumber = 0;
   fileTitle = af.fileTitle;
   header    = af.header;
   filename  = af.filename;  // This must be done for proper spec file reading
   descriptiveName = af.descriptiveName;
   fileHasHeader       = af.fileHasHeader;
   defaultFileName = af.defaultFileName;
   defaultExtension = af.defaultExtension;
   fileReadType        = af.fileReadType;
   fileWriteType       = af.fileWriteType;
   fileSupportAscii  = af.fileSupportAscii;
   fileSupportBinary = af.fileSupportBinary;
   fileSupportXML    = af.fileSupportXML;
   fileSupportXMLBase64 = af.fileSupportXMLBase64;
   fileSupportXMLGZipBase64 = af.fileSupportXMLGZipBase64;
   fileSupportOther  = af.fileSupportOther;
   enableAppendFileComment = af.enableAppendFileComment;
   readMetaDataOnlyFlag = af.readMetaDataOnlyFlag;
   rootXmlElementTagName = af.rootXmlElementTagName;
}
      
/**
 * The Destructor
 */
AbstractFile::~AbstractFile()
{
   clearAbstractFile();
}

/**
 * append the files comment.
 */
void 
AbstractFile::appendFileComment(const AbstractFile& af,
                                      const FILE_COMMENT_MODE fcm)
{
   switch (fcm) {
      case FILE_COMMENT_MODE_APPEND:
         {
            const QString otherFileComment(af.getFileComment());
            if (otherFileComment.isEmpty() == false) {
               QString comment = getFileComment();
               if (af.getFileName().isEmpty() == false) {
                  comment.append("\nAppended File: ");
                  comment.append(af.getFileName());
               }
               else {
                  comment.append("\nAppended Unnamed File");
               }
               comment.append("\n");
               comment.append(otherFileComment);
               setFileComment(comment);
            }
         }
         break;
      case FILE_COMMENT_MODE_LEAVE_AS_IS:
         break;
      case FILE_COMMENT_MODE_REPLACE:
         setFileComment(af.getFileComment());
         break;
   }
}
                             
/**
 * Clear the abstract file's members.
 * This must be called from the deriving file's clear() method.
 */
void
AbstractFile::clearAbstractFile()
{
   clearModified();
   timeToReadFileInSeconds = 0.0;
   fileTitle = "";
   filename = "";
   header.clear();
   transMatrix = NULL;

/*   
   if (defaultFileName.isEmpty() == false) {
      std::ostringstream str;
      str << defaultFileName
          << "_"
          << uniqueFileNumber;
      filename = str.str().c_str());
   }
*/

   clearDisplayList();
   
   //
   // Do not clear these items
   //
   // fileSupportAscii;
   // fileSupportBinary;
   // fileSupportXML;
   // fileSupportXMLBase64
   // fileSupportXMLGZipBase64
   // fileSupportOther
   // fileWriteType
   // descriptiveName
   // defaultFileName
   // defaultExtension
   // fileHasHeader
   // updateVersionIdEnabledFlag
   // rootXmlElementTagName
}

/**
 * set read/write type for a format.
 */
void
AbstractFile::setFileReadWriteType(const FILE_FORMAT format, const FILE_IO readAndOrWrite)
{
   switch (format) {
      case FILE_FORMAT_ASCII:
         fileSupportAscii = readAndOrWrite;
         break;
      case FILE_FORMAT_BINARY:
         fileSupportBinary = readAndOrWrite;
         break;
      case FILE_FORMAT_XML:
         fileSupportXML = readAndOrWrite;
         break;
      case FILE_FORMAT_XML_BASE64:
          fileSupportXMLBase64 = readAndOrWrite;
          break;
      case FILE_FORMAT_XML_GZIP_BASE64:
          fileSupportXMLGZipBase64 = readAndOrWrite;
          break;
      case FILE_FORMAT_OTHER:
         fileSupportOther = readAndOrWrite;
         break;
   }
}
      
/**
 * get the name of the file without any path (description only used if file name is isEmpty).
 */
QString 
AbstractFile::getFileNameNoPath(const QString& description) const
{
   QString name(FileUtilities::basename(getFileName(description)));
   return name;
}
      
/**
 * get the name of the file.
 */
QString 
AbstractFile::getFileName(const QString& description) const 
{ 
   if (filename.isEmpty()) {
      if (defaultFileNamePrefix.isEmpty() == false) {
         filename.append(defaultFileNamePrefix);
         filename.append(".");
      }
      
      if (defaultFileName.isEmpty() == false) {
         std::ostringstream str;
         if (description.isEmpty()) {
            str << defaultFileName.toAscii().constData()
                << "_"
                << uniqueFileNumber;
         }
         else {
            str << description.toAscii().constData();
         }
         str << "."
             << QDateTime::currentDateTime().toString("yyyy-MM-dd").toAscii().constData();
         
         if (defaultFileNameNumberOfNodes > 0) {
            bool showNumNodes = false;
#ifdef CARET_FLAG
            showNumNodes = 
               (dynamic_cast<const NodeAttributeFile*>(this) != NULL) ||
               (dynamic_cast<const CoordinateFile*>(this) != NULL) ||
               (dynamic_cast<const TopologyFile*>(this) != NULL);
#endif // CARET_FLAG            
            if (showNumNodes) {
               str << "."
                   << defaultFileNameNumberOfNodes;
            }
         }

         if (defaultExtension.isEmpty() == false) {
            str << defaultExtension.toAscii().constData();
         }
         filename.append(str.str().c_str());
      }
      
      if (filename.isEmpty()) {
         std::ostringstream str;
         str << "caret_file_"
             << uniqueFileNumber;
         filename.append(str.str().c_str());
         if (defaultExtension.isEmpty() == false) {
            filename.append(defaultExtension);
         }
      }
   }
   
   return filename; 
}

/// get the default file name (also overrides current file name)
QString 
AbstractFile::makeDefaultFileName(const QString& description) const
{
   filename = "";
   return getFileName(description);
}
      

/**
 * Return the path of the file
 */
QString
AbstractFile::getFileNamePath() const
{
   QString path(FileUtilities::dirname(getFileName()));
   if (path.isEmpty()) {
      path = ".";
   }
   return path;
}

/**
 * Get a tag from the header.
 */
QString
AbstractFile::getHeaderTag(const QString& name) const
{
   const std::map<QString, QString>::const_iterator iter = header.find(name);
   if (iter != header.end()) {
      return iter->second;
   }
   return "";
}

/**
 * Set a tag in the header.
 */
void
AbstractFile::setHeaderTag(const QString& nameIn, const QString& value)
{
   //
   // ignore "version_id"
   //
   if (nameIn == headerTagVersionID) {
      return;
   }
   
   const QString name(StringUtilities::makeLowerCase(nameIn));
   header[name] = value;
   setModified();
}

/**
 * remove header tag.
 */
void 
AbstractFile::removeHeaderTag(const QString& name)
{
   header.erase(name);
}

/**
 * Append a header tag.
 */
void
AbstractFile::appendToHeaderTag(const QString& nameIn, const QString& value,
                                const QString& separator)
{
   QString tagValue(getHeaderTag(nameIn));
   if (tagValue.isEmpty() == false) {
      tagValue.append(separator);
   }
   tagValue.append(value);
   setHeaderTag(nameIn, tagValue);
}

/**
 * append software version to file comment.  The version is added after "msg" and
 * followed by a carriage return.
 */
void 
AbstractFile::appendSoftwareVersionToFileComment(const QString& msg)
{
   QString s(msg);
   if (msg.isEmpty() == false) {
      s.append(" ");
   }
#ifdef CARET_FLAG
   s.append("CARET v");
   s.append(CaretVersion::getCaretVersionAsString());
   s.append("\n");
#endif // CARET_FLAG
   appendToFileComment(s);
}      
      
/**
 * Get the file's comment.
 */
QString
AbstractFile::getFileComment() const
{
   QString s(getHeaderTag(headerTagComment));
   //if (s.isEmpty() == false) {
   //   return StringUtilities::setupCommentForDisplay(s); 
   //}
   return s;
}

/**
 * Set the file's comment.
 */
void
AbstractFile::setFileComment(const QString& fc)
{
   //setHeaderTag(headerTagComment, StringUtilities::setupCommentForStorage(fc));
   setHeaderTag(headerTagComment, fc);
}

/**
 * Append to the file's comment.
 */
void
AbstractFile::appendToFileComment(const QString& fc)
{
   if (enableAppendFileComment) {
      if (fc.isEmpty() == false) {
         QString s(getFileComment());
         s.append(fc);
         setFileComment(s);
      }
   }
}

/**
 * see if the format can be read.
 */
bool 
AbstractFile::getCanRead(const FILE_FORMAT ff) const
{
   FILE_IO fio = FILE_IO_NONE;
   
   switch (ff) {
      case FILE_FORMAT_ASCII:
         fio = fileSupportAscii;
         break;
      case FILE_FORMAT_BINARY:
         fio = fileSupportBinary;
         break;
      case FILE_FORMAT_XML:
         fio = fileSupportXML;
         break;
      case FILE_FORMAT_XML_BASE64:
         fio = fileSupportXMLBase64;
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         fio = fileSupportXMLGZipBase64;
         break;
      case FILE_FORMAT_OTHER:
         fio = fileSupportOther;
         break;
   }
   
   const bool b = ((fio == FILE_IO_READ_ONLY) || (fio == FILE_IO_READ_AND_WRITE));
   return b;
}

/**
 * see if the format can be written.
 */
bool 
AbstractFile::getCanWrite(const FILE_FORMAT ff) const
{
   FILE_IO fio = FILE_IO_NONE;
   
   switch (ff) {
      case FILE_FORMAT_ASCII:
         fio = fileSupportAscii;
         break;
      case FILE_FORMAT_BINARY:
         fio = fileSupportBinary;
         break;
      case FILE_FORMAT_XML:
         fio = fileSupportXML;
         break;
      case FILE_FORMAT_XML_BASE64:
         fio = fileSupportXMLBase64;
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         fio = fileSupportXMLGZipBase64;
         break;
      case FILE_FORMAT_OTHER:
         fio = fileSupportOther;
         break;
   }
   
   const bool b = ((fio == FILE_IO_WRITE_ONLY) || (fio == FILE_IO_READ_AND_WRITE));
   return b;
}
      
/**
 * Set the file's title.
 */
void
AbstractFile::setFileTitle(const QString& ft)
{
   fileTitle = ft;
   setModified();
}

/**
 * clear the display list.
 */
void 
AbstractFile::clearDisplayList()
{
   if (displayListNumber > 0) {
      if (glIsList(displayListNumber) == GL_TRUE) {
         glDeleteLists(displayListNumber, 1);
      }
      displayListNumber = 0;
   }
}

/**
 * Find out if the file is modified.
 */
unsigned long
AbstractFile::getModified() const 
{
   return modified;
}

/**
 * set file has been modified.
 */
void 
AbstractFile::setModified()
{
   modified++;
   clearDisplayList();
}

/**
 * set modified counter.
 */
void 
AbstractFile::setModifiedCounter(const unsigned long value)
{
   modified = value;
}

/**
 * clear file has been modified without being saved.
 */
void 
AbstractFile::clearModified()
{
   modified = 0;
}

/**
 * determine if the file is an XML file.
 */
bool 
AbstractFile::isFileXML(QFile& file)
{
   bool isXML = false;
   
   //
   // Save current position in the file
   //
   const QFile::Offset filePos = file.at();
   
   //
   // Create a buffer for the data read
   //
   const QFile::Offset NUM = 512; //4096;
   unsigned char buffer[NUM];
   
   //
   // Set number of bytes to read
   //
   const int numToRead = std::min(file.size(), NUM);
   
   //
   // Read the bytes
   //
   const Q_LONG numRead = file.readBlock((char*)(buffer), numToRead);
   
   //
   // loop through characters
   //
   for (Q_LONG i = 0; i < numRead; i++) {
      const unsigned char c = buffer[i];
      
      if (c >= 127) {    // not ascii
         isXML = false;
         break;
      }
      else if ((c >= 32) && (c <= 126)) {  // visibile characters
         if (c == '<') {
            isXML = true;
            break;
         }
         else {
            isXML = false;
            break;
         }
      }
      else if ((c != 9) &&   // tab
               (c != 10) &&  // line feed
               (c != 13)) {  // carriage return
         isXML = false;
         break;
      }
   }
   
   //
   // Restor position in file
   //
   file.at(filePos);
   
   return isXML;
}
      
/**
 * read the contents of a file stored in memory.
 */
void 
AbstractFile::readFileFromArray(const char* data, 
                                const unsigned int dataLength,
                                const QString& debugFileName) throw (FileException)
{
   //
   // Put data into a file object, then reset position to zero
   //   
   QString tempFileName("caret_temp.file");
   bool keepTempFile = false;
   if (DebugControl::getDebugOn() &&
       (debugFileName.isEmpty() == false)) {
      tempFileName = debugFileName;
      keepTempFile = true;
   }
   QFile::remove(tempFileName);
   QFile file(tempFileName);
   if (file.open(IO_ReadWrite) == false) {
      throw FileException("", "Unable to create temporary read/write file in AbstractFile::readFile");
   }
   QDataStream stream(&file);
   stream.writeRawBytes(data, dataLength);
   //char newline[2] = { '\n', '\0' };
   //stream.writeRawBytes(newline, 1);
   file.at(0);
   readFileContents(file);
   file.close();
   if (keepTempFile == false) {
      QFile::remove(tempFileName);
   }
}      

/**
 * method used while reading files.
 */
void 
AbstractFile::readFileContents(QFile& file) throw (FileException)
{
   //
   // create the text and binary streams
   //
   QTextStream stream(&file);
   QDataStream binStream(&file);

   //
   // Determine if GIFTI node data file
   //
   const bool giftiDataArrayFileFlag = (dynamic_cast<GiftiDataArrayFile*>(this) != NULL);

   //
   // See if this is an XML file
   //
   bool xmlFileFlag = false;
   if (getCanRead(FILE_FORMAT_OTHER) == false) {
      xmlFileFlag = isFileXML(file);
      if (xmlFileFlag) {
         if (DebugControl::getDebugOn()) {
            std::cout << filename.toAscii().constData() << " is an XML file." << std::endl;
         }
         if ((getCanRead(FILE_FORMAT_XML) == false) &&
             (getCanRead(FILE_FORMAT_XML_BASE64) == false) &&
             (getCanRead(FILE_FORMAT_XML_GZIP_BASE64) == false)) {
            throw FileException(filename, 
               "is an XML file but XML is not supported for this type of file.\n"
               "Perhaps you need a new version of Caret.");
         }
      }
      else {
         if (DebugControl::getDebugOn()) {
            std::cout << filename.toAscii().constData() << " is NOT an XML file." << std::endl;
         }
      }
   }
   
   QDomDocument doc(rootXmlElementTagName);
   QDomElement  rootElement;
   
   //
   // Gifti Node data files do not use DOM parser
   //
   if ((xmlFileFlag) && (giftiDataArrayFileFlag == false)) {
      //
      // Parse the file if it is an XML file
      //
      QString errorMessage;
      int errorLine = 0, errorColumn = 0;
      if (doc.setContent(&file, &errorMessage, &errorLine, &errorColumn) == false) {
         std::ostringstream str;
         str << "Error parsing XML at line " << errorLine << " column " << errorColumn << ".  ";
         str << errorMessage.latin1() << std::ends;
         throw FileException(filename, str.str().c_str());
      }
      rootElement = doc.documentElement();
      
      //
      // Verify that the root element tag name is correct
      //
      if (rootElement.tagName() != rootXmlElementTagName) {
         //
         // Special case
         //
         const QString abName("Abstract");
         if (rootXmlElementTagName.left(abName.length()) != abName) {
            std::ostringstream str;
            str << "File's XML Root Element Tag Name should be " 
                << rootXmlElementTagName.toAscii().constData()
                << "\n"
                << "but it is "
                << rootElement.tagName().toAscii().constData()
                << ".\n";
            throw FileException(filename, str.str().c_str());
         }
      }
   }
   
   if (giftiDataArrayFileFlag && xmlFileFlag) {
      fileReadType = FILE_FORMAT_XML;
   }
   else if (fileHasHeader) {
      if (xmlFileFlag) {
         readHeaderXML(rootElement);
         setHeaderTag(headerTagEncoding, headerTagEncodingValueXML);
      }
      else {
         //std::cout << "File position/size/end/stream end (before header): " 
         //          << file.pos() << ", " << file.size() << ", " 
         //          << file.atEnd() << ", " << stream.atEnd() << std::endl;
         readHeader(file, stream);
         //std::cout << "File position/size/end/stream end (after header): " 
         //          << file.pos() << ", " << file.size() << ", " 
         //          << file.atEnd() << ", " << stream.atEnd() << std::endl;
      }

      //
      // Check type of file
      //
      const QString encoding = 
               StringUtilities::makeUpperCase(getHeaderTag(headerTagEncoding));
      if (encoding == headerTagEncodingValueAscii) {
         if (getCanRead(FILE_FORMAT_ASCII) == false) {
            throw FileException(getFileName(), 
                   "Ascii format file not supported for this type of file.\n"
                   "Perhaps you need a newer version of Caret.");
         }
         fileReadType = FILE_FORMAT_ASCII;
      }
      else if (encoding == headerTagEncodingValueBinary) {
         if (getCanRead(FILE_FORMAT_BINARY) == false) {
            throw FileException(getFileName(), 
                   "Binary format file not supported for this type of file.\n"
                   "Perhaps you need a newer version of Caret.");
         }
         fileReadType = FILE_FORMAT_BINARY;   
         
#ifdef QT4_FILE_POS_BUG
         qint64 offset = findBinaryDataOffsetQT4Bug(file, "EndHeader");
         if (offset > 0) {
            offset++;  
            file.seek(offset);
         }
#endif // QT4_FILE_POS_BUG
/*
         throw FileException(getFileName(), 
                "Due to a bug in QT4, Caret is unable to read binary files at this time.\n"
                "Use \"caret_file_convert\" on the command line to convert files to text\n"
                "format (caret_file_convert -text *).");
*/
      }
      else if (encoding == headerTagEncodingValueXML) {
         if (getCanRead(FILE_FORMAT_XML) == false) {
            throw FileException(getFileName(), 
                   "XML format file not supported for this type of file.\n"
                   "Perhaps you need a newer version of Caret.");
         }
         fileReadType = FILE_FORMAT_XML;
      }
      else if (encoding == headerTagEncodingValueOther) {
         if (getCanRead(FILE_FORMAT_OTHER) == false) {
            throw FileException(getFileName(), 
                   "Other format file not supported for this type of file.\n"
                   "Perhaps you need a newer version of Caret.");
         }
         fileReadType = FILE_FORMAT_OTHER;
      }
      else {
         //
         // Assume ascii
         //
         fileReadType = FILE_FORMAT_ASCII;
      }
   }
   
   //
   // Set write type and possibly override with the preferred write type
   //
   if (getCanWrite(fileReadType)) {
      fileWriteType = fileReadType;
   }
   if (preferredWriteType != fileWriteType) {
      switch (preferredWriteType) {
         case FILE_FORMAT_ASCII:
            if (getCanWrite(FILE_FORMAT_ASCII)) {
               fileWriteType = preferredWriteType;
            }
            break;
         case FILE_FORMAT_BINARY:
            if (getCanWrite(FILE_FORMAT_BINARY)) {
               fileWriteType = preferredWriteType;
            }
            break;
         case FILE_FORMAT_XML:
            if (getCanWrite(FILE_FORMAT_XML)) {
               fileWriteType = preferredWriteType;
            }
            break;
         case FILE_FORMAT_XML_BASE64:
            if (getCanWrite(FILE_FORMAT_XML_BASE64)) {
               fileWriteType = preferredWriteType;
            }
            break;
         case FILE_FORMAT_XML_GZIP_BASE64:
            if (getCanWrite(FILE_FORMAT_XML_GZIP_BASE64)) {
               fileWriteType = preferredWriteType;
            }
            break;
         case FILE_FORMAT_OTHER:
            if (getCanWrite(FILE_FORMAT_OTHER)) {
               fileWriteType = preferredWriteType;
            }
            break;
      }
   }

   //QTextStream stream(&file);
   //QDataStream binStream(&file);
   //std::cout << "File position/size: " << file.pos() << ", " << file.size() << std::endl;
   //std::cout << "Stream atEnd(): " << stream.atEnd() << std::endl;
   //if (readMetaDataOnlyFlag == false) {
      readFileData(file, stream, binStream, rootElement);
   //}
   
   //
   // Eliminate "hem_flag" as it is replaced with "structure"
   //
   const QString hemFlagTag("hem_flag");
   const QString structureValue = getHeaderTag(AbstractFile::headerTagStructure);
   const QString hemFlagValue = getHeaderTag(hemFlagTag);
   if (structureValue.isEmpty() && (hemFlagValue.isEmpty() == false)) {
      setHeaderTag(AbstractFile::headerTagStructure, hemFlagValue);
   }
   removeHeaderTag(hemFlagTag);
   
   clearModified();
}

/**
 * Read the specified file.  Will throw a FileException if an 
 * error occurs while reading the file.
 */
void
AbstractFile::readFileMetaDataOnly(const QString& filenameIn) throw(FileException)
{
   setReadMetaDataOnlyFlag(true);
   readFile(filenameIn);
}

/**
 * Read the specified file.  Will throw a FileException if an 
 * error occurs while reading the file.
 */
void
AbstractFile::readFile(const QString& filenameIn) throw(FileException)
{
   // Note: filenameIn could possibly be "this's" filename so make a
   // copy of it before calling "clear()" to prevent it from being erased.
   const QString filenameIn2(filenameIn);
   
   clear();

   if (filenameIn2.isEmpty()) {
      throw FileException(filenameIn2, "Filename for reading is isEmpty");
   }

   filename = filenameIn2;
   
   QFile file(getFileName());
   const float fileSize = file.size() / 1048576.0;

   QTime timer;
   timer.start();
   
   if (file.open(IO_ReadOnly)) {

      try {
         readFileContents(file);
      }
      catch (FileException& e) {
         file.close();
         clearModified();
         throw e;
      }
   
      file.close();
   }
   else {
      throw FileException(getFileName(), "Failure trying to open: ");
   }
   
   //
   // See how long it took to read the file
   //
   timeToReadFileInSeconds = static_cast<float>(timer.elapsed()) / 1000.0;
   if (DebugControl::getDebugOn() ||
       DebugControl::getFileReadTimingFlag()) {
      std::cout << "Time to read " << FileUtilities::basename(getFileName()).toAscii().constData() 
                << " ("
                << fileSize
                << " MB) was "
                << timeToReadFileInSeconds
                << " seconds." << std::endl;
   }
}

/**
 * read header data from an XML file.
 */
void 
AbstractFile::readHeaderXML(QDomElement& rootElement)
{
   //
   // Loop through children of the root node
   //
   QDomNode rootChildNode = rootElement.firstChild();
   while (rootChildNode.isNull() == false) {
      //
      // Is this the header node ?
      //
      if (rootChildNode.toElement().tagName() == xmlHeaderTagName) {
         QDomNode node = rootChildNode.firstChild();
         while (node.isNull() == false) {
            QDomElement elem = node.toElement();
            if (elem.isNull() == false) {     
               const QString tagName(elem.tagName()); 
               QString tagValue = getXmlElementFirstChildAsString(elem);
               setHeaderTag(tagName, tagValue);
               if (tagName == headerTagComment) {
                  const int len = tagValue.length();
                  if (len > commentLengthWarning) {
                     std::cout << "WARNING "
                               << FileUtilities::basename(getFileName()).toAscii().constData()
                               << " comment is "
                               << len
                               << " characters."
                               << std::endl;
                  }
               }
               if (DebugControl::getDebugOn()) {
                  std::cout << "XML Header Tag/Value (" << tagName.toAscii().constData() << "," << tagValue.toAscii().constData() << ")" << std::endl;
               }
            }
            node = node.nextSibling();
         }
      }
      
      rootChildNode = rootChildNode.nextSibling();
   }
}

/**
 * write header data to an XML file.
 */
void 
AbstractFile::writeHeaderXML(QDomDocument& doc, QDomElement& rootElement)
{
   //
   // GIFTI XML files have their own header format
   //
   if (dynamic_cast<GiftiDataArrayFile*>(this) != NULL) {
      return;
   }
   
   //
   // Create the header element
   //
   QDomElement headerElement = doc.createElement(xmlHeaderTagName);
   
   //
   // Loop through the header elements
   //
   std::map<QString, QString>::iterator iter;
   for (iter = header.begin(); iter != header.end(); iter++) {
      //
      // Get the tag and its value
      //
      const QString tag(iter->first);
      const QString value(iter->second);
      
      //
      // Create the new element and add it to the header
      //
      QDomElement elem = doc.createElement(tag);
      QDomText    text = doc.createCDATASection(value); //doc.createTextNode(value);
      elem.appendChild(text);
      headerElement.appendChild(elem);
   }
   
   //
   // Add the header to the root element
   //
   rootElement.appendChild(headerElement);
}
      
/**
 * Get an XML element's first child and return it as a string.
 */
QString
AbstractFile::getXmlElementFirstChildAsString(const QDomElement& elem)
{
   QString value;   
   QDomNode vNode = static_cast<QDomNode>(elem.firstChild());
   if (vNode.isNull() == false) {
      const QDomText textNode = vNode.toText();
      if (textNode.isNull() == false) {
         value = textNode.data();
      }
   }
   return value;
}
      
/**
 * Get an XML element's first child and return it as a float.
 */
float
AbstractFile::getXmlElementFirstChildAsFloat(const QDomElement& elem)
{
   QString value;   
   QDomNode vNode = static_cast<QDomNode>(elem.firstChild());
   if (vNode.isNull() == false) {
      const QDomText textNode = vNode.toText();
      if (textNode.isNull() == false) {
         value = textNode.data();
      }
   }
   const float valueFloat = StringUtilities::toFloat(value);
   return valueFloat;
}
      
/**
 * Get an XML element's first child and return it as an int.
 */
int
AbstractFile::getXmlElementFirstChildAsInt(const QDomElement& elem)
{
   QString value;   
   QDomNode vNode = static_cast<QDomNode>(elem.firstChild());
   if (vNode.isNull() == false) {
      const QDomText textNode = vNode.toText();
      if (textNode.isNull() == false) {
         value = textNode.data();
      }
   }
   
   const int valueInt = StringUtilities::toInt(value);
   return valueInt;
}
      
/**
 * Get an XML element's first child and return it as a float.
 */
void 
AbstractFile::getXmlElementFirstChildAsFloat(const QDomElement& elem,
                                             float* values,
                                             const int numValues)
{
   QString value;   
   QDomNode vNode = static_cast<QDomNode>(elem.firstChild());
   if (vNode.isNull() == false) {
      const QDomText textNode = vNode.toText();
      if (textNode.isNull() == false) {
         value = textNode.data();
      }
   }
   std::vector<float> tokens;
   StringUtilities::token(value, " ", tokens);
   for (int i = 0; i < numValues; i++) {
      if (i < static_cast<int>(tokens.size())) {
         values[i] = tokens[i];
      }
      else {
         values[i] = 0.0;
      }
   }
}

/**
 * Get an XML element's first child and return it as an int.
 */
void 
AbstractFile::getXmlElementFirstChildAsInt(const QDomElement& elem,
                                           int* values,
                                           const int numValues)
{
   QString value;   
   QDomNode vNode = static_cast<QDomNode>(elem.firstChild());
   if (vNode.isNull() == false) {
      const QDomText textNode = vNode.toText();
      if (textNode.isNull() == false) {
         value = textNode.data();
      }
   }
   std::vector<int> tokens;
   StringUtilities::token(value, " ", tokens);
   for (int i = 0; i < numValues; i++) {
      if (i < static_cast<int>(tokens.size())) {
         values[i] = tokens[i];
      }
      else {
         values[i] = 0;
      }
   }
}

/**
 * add a CDATA element to an XML Dom Element.
 */
void 
AbstractFile::addXmlCdataElement(QDomDocument& xmlDoc,
                                 QDomElement& parentElement,
                                 const QString& childElementName,
                                 const QString& childElementData)
{
   QDomElement element = xmlDoc.createElement(childElementName);
   QDomCDATASection node = xmlDoc.createCDATASection(childElementData);
   element.appendChild(node);
   parentElement.appendChild(element);
}
                        
/**
 * add a Text element to an XML Dom Element.
 */
void 
AbstractFile::addXmlTextElement(QDomDocument& xmlDoc,
                                QDomElement& parentElement,
                                const QString& childElementName,
                                const QString& childElementData)
{
   QDomElement element = xmlDoc.createElement(childElementName);
   QDomText node = xmlDoc.createTextNode(childElementData);
   element.appendChild(node);
   parentElement.appendChild(element);
}
                              
/**
 * add a Text element to an XML Dom Element.
 */
void 
AbstractFile::addXmlTextElement(QDomDocument& xmlDoc,
                                QDomElement& parentElement,
                                const QString& childElementName,
                                const int childElementData)
{
   QDomElement element = xmlDoc.createElement(childElementName);
   QDomText node = xmlDoc.createTextNode(StringUtilities::fromNumber(childElementData));
   element.appendChild(node);
   parentElement.appendChild(element);
}
                              
/**
 * add a Text element to an XML Dom Element.
 */
void 
AbstractFile::addXmlTextElement(QDomDocument& xmlDoc,
                                QDomElement& parentElement,
                                const QString& childElementName,
                                const int* childElementData,
                                const int numChildElements)
{
   QDomElement element = xmlDoc.createElement(childElementName);
   QString data;
   for (int i = 0; i < numChildElements; i++) {
      if (i > 0) {
         data.append(" ");
      }
      data.append(StringUtilities::fromNumber(childElementData[i]));
   }
   QDomText node = xmlDoc.createTextNode(data);
   element.appendChild(node);
   parentElement.appendChild(element);
}
                              
/**
 * add a Text element to an XML Dom Element.
 */
void 
AbstractFile::addXmlTextElement(QDomDocument& xmlDoc,
                                QDomElement& parentElement,
                                const QString& childElementName,
                                const float childElementData)
{
   QDomElement element = xmlDoc.createElement(childElementName);
   QDomText node = xmlDoc.createTextNode(StringUtilities::fromNumber(childElementData));
   element.appendChild(node);
   parentElement.appendChild(element);
}
                              
/**
 * add a Text element to an XML Dom Element.
 */
void 
AbstractFile::addXmlTextElement(QDomDocument& xmlDoc,
                                QDomElement& parentElement,
                                const QString& childElementName,
                                const float* childElementData,
                                const int numChildElements)
{
   QDomElement element = xmlDoc.createElement(childElementName);
   QString data;
   for (int i = 0; i < numChildElements; i++) {
      if (i > 0) {
         data.append(" ");
      }
      data.append(StringUtilities::fromNumber(childElementData[i]));
   }
   QDomText node = xmlDoc.createTextNode(data);
   element.appendChild(node);
   parentElement.appendChild(element);
}
                              
/**
 * Read the header from "this" file.
 */
void 
AbstractFile::readHeader(QFile& file, QTextStream& stream) throw(FileException)
{
   QString tag, value;
   
   readTagLine(stream, tag, value);
   if (tag == "BeginHeader") {
      bool done = false;
      while (done == false) {
         readTagLine(stream, tag, value);
         //std::cout << "File position/size/end (while reading header): " 
         //          << file.pos() << ", " << file.size() << ", " << file.atEnd() << " " 
         //          << stream.atEnd() << std::endl;
         if (tag[0] == '#') {
            tag = tag.mid(1);
         }
         if (tag == "EndHeader") {
            done = true;
         }
         else {
            if (tag == headerTagComment) {
               value = StringUtilities::setupCommentForDisplay(value);
               
               const int len = value.length();
               if (len > commentLengthWarning) {
                  std::cout << "WARNING "
                            << FileUtilities::basename(getFileName()).toAscii().constData()
                            << " comment is "
                            << len
                            << " characters."
                            << std::endl;
               }
            }
            setHeaderTag(tag, value);
         }
      }
   }
   else {
      //
      // Close and reopen the file.  I tried to get the file position 
      // prior to reading and then setting the file position but the first 
      // read from the file would get the first character twice.  (JWH)
      //
      //file.close();
      //if (file.open(IO_ReadOnly) == false) {
      //   throw FileException(getFileName(), "Failure trying to open while reading header: ");
      //}
#ifdef QT4_FILE_POS_BUG
      file.reset();
      file.seek(0);
      file.close();
      if (file.open(IO_ReadOnly) == false) {
         throw FileException(filename, "Failed to reopen file after closing file since there\n"
                                       "is no header to get around QT4 file bugs.");
      }
      file.seek(0);
      file.reset();
#else  // QT4_FILE_POS_BUG
      file.reset();
#endif // QT4_FILE_POS_BUG
   }
}

/**
 * Read a line and chop off comment beginning with "#".
 */
void
AbstractFile::readLineChopComment(QTextStream& stream,
                                  QString& lineOut) throw (FileException)
{
   readLine(stream, lineOut);
   int commentCharPos = lineOut.find('#');
   if (commentCharPos != -1) {
      lineOut.setLength(commentCharPos);
   }
}

/**
 * Read a line from the file and return in a QString.  May throw FileException
 */
void
AbstractFile::readLine(QTextStream& stream, QString& lineOut) throw (FileException)
{
   lineOut = "";
#ifdef QT4_FILE_POS_BUG
   QFile* file = dynamic_cast<QFile*>(stream.device());
   if (file != NULL) {
      lineOut = file->readLine();
      lineOut = lineOut.trimmed();
   }
   else {
      lineOut = stream.readLine();
   }
   if (lineOut.isNull()) {
      lineOut = "";
   }
#else  // QT4_FILE_POS_BUG
   lineOut = stream.readLine();
#endif // QT4_FILE_POS_BUG
   if (lineOut.isNull()) {
      lineOut = "";
      // 3/25/05 palette file problems throw FileException(getFileName(), "Premature end of file.  Is file damaged?");
   }
}

/**
 * Read a line from the file and return in a c++ string.  May throw FileException
 */
/*
void
AbstractFile::readLine(QTextStream& stream, QString& lineOut) throw (FileException)
{
   const QString line = stream.readLine();
   if (line.isNull()) {
      lineOut = "");   // 3/25/05
      return;               // 3/25/05
      // 3/25/05 throw FileException(getFileName(), "Premature end of file.  Is file damaged?");
   }
   lineOut = line);
}
*/

/**
 * Read a line from the file and return in a c++ string.  May throw FileException
 */
void
AbstractFile::readLine(const QString& /*filenameIn*/, QTextStream& stream, 
                       QString& lineOut) throw (FileException)
{
   lineOut = "";
#ifdef QT4_FILE_POS_BUG
   QFile* file = dynamic_cast<QFile*>(stream.device());
   if (file != NULL) {
      lineOut = file->readLine();
      //lineOut = lineOut.trimmed();
   }
   else {
      lineOut = stream.readLine();
   }
   if (lineOut.isNull()) {
      lineOut = "";
   }
#else  // QT4_FILE_POS_BUG
   const QString line = stream.readLine();
   if (line.isNull()) {
      lineOut = "";   // 3/25/05
      return;               // 3/25/05 
      // 3/25/05 throw FileException(filenameIn, "Premature end of file.  Is file damaged?");
   }
   lineOut = line;
#endif // QT4_FILE_POS_BUG
}

/**
 * Read a line that contains a tag and its associated value
 */
void
AbstractFile::readTagLine(const QString& fileNameIn,
                          QTextStream& stream, QString& tag, QString& tagValue)
       throw (FileException)
{
   tag = "";
   tagValue = "";
   
   QString sline;
   AbstractFile::readLine(fileNameIn, stream, sline);
   QString line(sline);
   
   QString tagStr;
   QTextIStream(&line) >> tagStr;
   
   if (tagStr.isEmpty()) {
      return;
      //throw FileException(filename, "Invalid line in tags section.");
   }
   tag = tagStr;

   char* lineCh = new char[line.length() + 1];
   strcpy(lineCh, line);
   int blankCount = 0;
   int offset = 0;
   for (unsigned int i = 0; i < strlen(lineCh); i++) {
      if (lineCh[i] == '\n') {
         lineCh[i] = '\0';
         if (blankCount == 0) {
            offset = i;
         }
         break;
      }
      else if (lineCh[i] == ' ') {
         blankCount++;
         if (blankCount == 1) {
            offset = i + 1;
         }
      }
   }

   tagValue = &lineCh[offset];
   tagValue = StringUtilities::trimWhitespace(tagValue);
   
   delete[] lineCh;
}

/**
 * Read a line that contains a tag and its associated value
 */
void
AbstractFile::readTagLine(QTextStream& stream, QString& tag, QString& tagValue)
       throw (FileException)
{
   static bool debugFlag = false;

   tag = "";
   tagValue = "";

   QString line;
   readLine(stream, line);
   if (debugFlag) std::cout << "Line: " << line.toAscii().constData() << std::endl;
   QString tagStr;
   QTextIStream(&line) >> tagStr;
   if (debugFlag) std::cout << "tagStr: " << tagStr.toAscii().constData() << std::endl;
  
   if (tagStr.isEmpty()) {
      return;
      //throw FileException(filename, "Invalid line in tags section.");
   }
   tag = tagStr;

   char* lineCh = new char[line.length() + 1];
   strcpy(lineCh, line);
   int blankCount = 0;
   int offset = 0;
   for (unsigned int i = 0; i < strlen(lineCh); i++) {
      if (lineCh[i] == '\n') {
         lineCh[i] = '\0';
         if (blankCount == 0) {
            offset = i;
         }
         break;
      }
      else if (lineCh[i] == ' ') {
         blankCount++;
         if (blankCount == 1) {
            offset = i + 1;
         }
      }
   }

   tagValue = &lineCh[offset];
   tagValue = StringUtilities::trimWhitespace(tagValue);
   delete[] lineCh;
}

/**
 * Read a line that contains a tag and its associated value
 */
void
AbstractFile::readTagLine(QTextStream& stream, QString& line, QString& tag, QString& tagValue)
       throw (FileException)
{
   static bool debugFlag = false;

   line = "";
   tag = "";
   tagValue = "";

   readLine(stream, line);
   if (debugFlag) std::cout << "Line: " << line.toAscii().constData() << std::endl;
   QString tagStr;
   QTextIStream(&line) >> tagStr;
   if (debugFlag) std::cout << "tagStr: " << tagStr.toAscii().constData() << std::endl;
  
   if (tagStr.isEmpty()) {
      return;
      //throw FileException(filename, "Invalid line in tags section.");
   }
   tag = tagStr;

   char* lineCh = new char[line.length() + 1];
   strcpy(lineCh, line);
   int blankCount = 0;
   int offset = 0;
   for (unsigned int i = 0; i < strlen(lineCh); i++) {
      if (lineCh[i] == '\n') {
         lineCh[i] = '\0';
         if (blankCount == 0) {
            offset = i;
         }
         break;
      }
      else if (lineCh[i] == ' ') {
         blankCount++;
         if (blankCount == 1) {
            offset = i + 1;
         }
      }
   }

   tagValue = &lineCh[offset];
   tagValue = StringUtilities::trimWhitespace(tagValue);
   delete[] lineCh;
}

/**
 * Read a line that contains a number, a tag, and its associated value
 */
void
AbstractFile::readNumberedTagLine(QTextStream& stream, int& number,
                                  QString& tag, QString& tagValue)
                                                       throw (FileException)
{
   static bool debugFlag = false;

   number = -1;
   tag = "";
   tagValue = "";
   
   QString qline;
   readLine(stream, qline);
   if (debugFlag) std::cout << "Line: " << qline.toAscii().constData() << std::endl;
   
   int numberRead = -1;
   QString tagStr;
   QTextIStream(&qline) >> numberRead
                       >> tagStr;
   if (debugFlag) {
      std::cout << "number: " << numberRead << std::endl;
      std::cout << "tagStr: " << tagStr.toAscii().constData() << std::endl;
   }
   
   if (tagStr.isEmpty()) {
      return;
      //throw FileException(filename, "Invalid line in tags section.");
   }
   number = numberRead;
   tag = tagStr;

   //
   // Find first character after tag
   //
   QString line(qline);
   int pos = line.find(tag);
   if (pos != -1) {
      pos += tag.length();
   }
   
   //
   // Find first non-whitespace character and make it the tag value
   //
   const int len = line.length();
   for (int i = pos; i < len; i++) {
      if ((line[i] != ' ') && (line[i] != '\t')) {
         tagValue = line.mid(i);
         tagValue = StringUtilities::trimWhitespace(tagValue);
         return;
      }
   }   
}

/**
 * Read a line and store tokens in a string list
 */ 
QStringList
AbstractFile::readLineIntoStringList(QTextStream& stream) throw (FileException)
{
   QString line;
   readLine(stream, line);
   return QStringList::split(" ", line);
}

/** 
 * Read a line separating each white spaced string into "tokens"
 */
void 
AbstractFile::readLineIntoTokens(QTextStream& stream,
                                 QString& lineOut,
                                 std::vector<QString>& tokens) throw (FileException)
{
   tokens.clear();
   readLine(stream, lineOut);
   StringUtilities::token(lineOut, " \t", tokens);
}

/** 
 * Read a line separating each white spaced string into "tokens"
 */
void 
AbstractFile::readLineIntoTokens(const QString& filenameIn,
                                 QTextStream& stream,
                                 QString& lineOut,
                                 std::vector<QString>& tokens) throw (FileException)
{
   tokens.clear();
   readLine(filenameIn, stream, lineOut);
   StringUtilities::token(lineOut, " \t", tokens);
}

/**
 * Split a tag that start with a column number into the column number
 * and the tag's value.  Returns column number.
 */
int
AbstractFile::splitTagIntoColumnAndValue(const QString s, QString& value) const
{
   std::vector<QString> tokens;
   StringUtilities::token(s, " ", tokens);
   
   if (tokens.size() == 0) { 
      return -1;
   }

   const int columnNumber = tokens[0].toInt();
   if (tokens.size() == 1) {
      return columnNumber;
   }
   value = tokens[1];
   for (unsigned int i = 2; i < tokens.size(); i++) {
      value.append(" ");
      value.append(tokens[i]);
   }
   
   return columnNumber;
}

/**
 * Write the current file's memory to a byte array.
 */
void 
AbstractFile::writeFileToArray(QByteArray& ba) throw (FileException)
{
   QTextStream ts(ba, IO_WriteOnly);
   QDataStream ds(&ba, IO_WriteOnly);
   writeFileContents(ts, ds);
}
      
/**
 * method used for writing files.
 */
void 
AbstractFile::writeFileContents(QTextStream& stream, QDataStream& dataStream) throw (FileException)
{
   bool isXmlFile = false;
   
   switch (fileWriteType) {
      case FILE_FORMAT_ASCII:
         if (getCanWrite(FILE_FORMAT_ASCII) == false) {
            throw FileException(filename, "Ascii (Text) type file not supported.");
         }
         break;
      case FILE_FORMAT_BINARY:
         if (getCanWrite(FILE_FORMAT_BINARY) == false) {
            throw FileException(filename, "Binary type file not supported.");
         }
         break;
      case FILE_FORMAT_XML:
         if (getCanWrite(FILE_FORMAT_XML) == false) {
            throw FileException(filename, "XML type file not supported.");
         }
         isXmlFile = true;
         break;
      case FILE_FORMAT_XML_BASE64:
         if (getCanWrite(FILE_FORMAT_XML_BASE64) == false) {
            throw FileException(filename, "XML Base64 type file not supported.");
         }
         isXmlFile = true;
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         if (getCanWrite(FILE_FORMAT_XML_GZIP_BASE64) == false) {
            throw FileException(filename, "XML GZip Base64 type file not supported.");
         }
         isXmlFile = true;
         break;
      case FILE_FORMAT_OTHER:
         if (getCanWrite(FILE_FORMAT_OTHER) == false) {
            throw FileException(filename, "\"Other\" type file not supported.");
         }
         break;
   }
   
   //
   // Setup for XML file
   //
   QDomDocument xmlDoc;
   QDomElement rootElement;
   if (isXmlFile) {
      //
      // Create the root element
      //
      rootElement = xmlDoc.createElement(rootXmlElementTagName);
      xmlDoc.appendChild(rootElement);
      
      //
      // Add the encoding information
      //
      QDomNode encodeNode = xmlDoc.createProcessingInstruction(
                               "xml",
                               "version=\"1.0\" encoding=\"UTF-8\"");
      xmlDoc.insertBefore(encodeNode, xmlDoc.firstChild());
   }

   stream.setf(QTextStream::fixed);
   stream.precision(textFileDigitsRightOfDecimal);
   if (fileHasHeader) {
      setHeaderTag(headerTagDate, QDateTime::currentDateTime().toString(Qt::TextDate));
      switch (fileWriteType) {
         case FILE_FORMAT_ASCII:
            setHeaderTag(headerTagEncoding, headerTagEncodingValueAscii);
            break;
         case FILE_FORMAT_BINARY:
            setHeaderTag(headerTagEncoding, headerTagEncodingValueBinary);
            break;
         case FILE_FORMAT_XML:
            setHeaderTag(headerTagEncoding, headerTagEncodingValueXML);
            break;
         case FILE_FORMAT_XML_BASE64:
            setHeaderTag(headerTagEncoding, headerTagEncodingValueXML);
            break;
         case FILE_FORMAT_XML_GZIP_BASE64:
            setHeaderTag(headerTagEncoding, headerTagEncodingValueXML);
            break;
         case FILE_FORMAT_OTHER:
            setHeaderTag(headerTagEncoding, headerTagEncodingValueOther);
            break;
      }
      
      //
      // Add caret version to header
      //
#ifdef CARET_FLAG
      setHeaderTag("Caret-Version", CaretVersion::getCaretVersionAsString());
#endif // CARET_FLAG
      
      if (isXmlFile) {
         writeHeaderXML(xmlDoc, rootElement);
      }
      else {
         writeHeader(stream);
      }
   }
   
#ifdef QT4_FILE_POS_BUG
   setBinaryFilePosQT4Bug();
#endif // QT4_FILE_POS_BUG

   writeFileData(stream, dataStream, xmlDoc, rootElement);
   //
   // Handle XML files
   //
   if (isXmlFile) {
      // 
      // Do not set for GIFTI data files
      //
      if (dynamic_cast<GiftiDataArrayFile*>(this) == NULL) {
         xmlDoc.save(stream, 3);
      }
   }
}

      
/**
 * Write the data file.
 */
void
AbstractFile::writeFile(const QString& filenameIn) throw (FileException)
{
   if (filenameIn.isEmpty()) {
      throw FileException(filenameIn, "Filename for writing is isEmpty");
   }

   filename = filenameIn;
   
   //
   // Check file type here to avoid opening and clearing a file on disk.
   //
   switch (fileWriteType) {
      case FILE_FORMAT_ASCII:
         if (getCanWrite(FILE_FORMAT_ASCII) == false) {
            throw FileException(filename, "Ascii (Text) type file not supported.");
         }
         break;
      case FILE_FORMAT_BINARY:
         if (getCanWrite(FILE_FORMAT_BINARY) == false) {
            throw FileException(filename, "Binary type file not supported.");
         }
         break;
      case FILE_FORMAT_XML:
         if (getCanWrite(FILE_FORMAT_XML) == false) {
            throw FileException(filename, "XML type file not supported.");
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         if (getCanWrite(FILE_FORMAT_XML_BASE64) == false) {
            throw FileException(filename, "XML Base64 type file not supported.");
         }
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         if (getCanWrite(FILE_FORMAT_XML_GZIP_BASE64) == false) {
            throw FileException(filename, "XML GZip Base64 type file not supported.");
         }
         break;
      case FILE_FORMAT_OTHER:
         if (getCanWrite(FILE_FORMAT_OTHER) == false) {
            throw FileException(filename, "\"Other\" type file not supported.");
         }
         break;
   }

   writingQFile = new QFile(getFileName());
   if (writingQFile->open(IO_WriteOnly)) {
      QTextStream stream(writingQFile);
      QDataStream binStream(writingQFile);
      
      try {
         writeFileContents(stream, binStream);
            
         writingQFile->close();
         delete writingQFile;
         writingQFile = NULL;
      }
      catch (FileException& e) {
         writingQFile->close();
         delete writingQFile;
         writingQFile = NULL;
         throw e;
      }
      clearModified();
   }
   else {
      delete writingQFile;
      writingQFile = NULL;

      QFileInfo fileInfo(getFileName());
      if (fileInfo.exists() && (fileInfo.isWritable() == false)) {
         throw FileException(getFileName(), "File exists but it is not writable");
      }
      else {
         QString msg("Unable to open for writing.  Current path: ");
         msg.append(QDir::currentDirPath());
         throw FileException(getFileName(), msg);
      }
   }
}

/**
 * Write the file's header.
 */
void
AbstractFile::writeHeader(QTextStream& stream) throw (FileException)
{
   stream << "BeginHeader\n";
   std::map<QString, QString>::iterator iter;
   for (iter = header.begin(); iter != header.end(); iter++) {
      const QString tag(iter->first);
      const QString value(iter->second);
      if (tag == headerTagComment) {
         stream << tag << " " << StringUtilities::setupCommentForStorage(value) << "\n";
      }
      else {
         stream << tag << " " << value << "\n";
      }
   }
   stream << "EndHeader\n";
}

/**
 * Create a subclass of abstract file based upon the file name's extension.
 * Returns NULL if not recognized.
 */
AbstractFile*
AbstractFile::getSubClassDataFile(const QString& filename,
                                  QString& errorMessageOut)
{
   AbstractFile* af = NULL;
   errorMessageOut = "";
   
   //
   // Get the filename's extension to determine the type of file
   // Prepend with a period.
   //
   QString ext(".");
   ext.append(FileUtilities::filenameExtension(filename));

   if (ext.isEmpty() || (ext.length() == 1)) {
      errorMessageOut = "filename has no extension which is needed to determine type of file.";
      return NULL;
   } 
    
#ifdef CARET_FLAG
   if (ext == ".xml") {
      if (StringUtilities::endsWith(filename, SpecFile::getAtlasSpaceFileExtension())) {
         ext = SpecFile::getAtlasSpaceFileExtension();
      }
      if (StringUtilities::endsWith(filename, SpecFile::getCocomacConnectivityFileExtension())) {
         ext = SpecFile::getCocomacConnectivityFileExtension();
      }
   }
   else if (ext == SpecFile::getFreeSurferAsciiSurfaceFileExtension()) {
      if (StringUtilities::endsWith(filename, SpecFile::getFreeSurferAsciiCurvatureFileExtension())) {
         ext = SpecFile::getFreeSurferAsciiCurvatureFileExtension();
      }
      else if (StringUtilities::endsWith(filename, SpecFile::getFreeSurferAsciiFunctionalFileExtension())) {
         ext = SpecFile::getFreeSurferAsciiFunctionalFileExtension();
      }
   }
   
   const QString imageExt = QImageReader::imageFormat(filename);
   if (imageExt.isEmpty() == false) {
      af = new ImageFile;
   }
   else if (ext == SpecFile::getAreaColorFileExtension()) {
      af = new AreaColorFile;
   }
   else if (ext == SpecFile::getArealEstimationFileExtension()) {
      af = new ArealEstimationFile;
   }
   else if (ext == SpecFile::getAtlasSpaceFileExtension()) {
      af = new AtlasSpaceFile;
   }
   else if (ext == SpecFile::getAtlasSurfaceDirectoryFileExtension()) {
      af = new AtlasSurfaceDirectoryFile;
   }
   else if (ext == SpecFile::getBorderFileExtension()) {
      af = new BorderFile;
   }
   else if (ext == SpecFile::getBorderColorFileExtension()) {
      af = new BorderColorFile;
   }
   else if (ext == SpecFile::getBorderProjectionFileExtension()) {
      af = new BorderProjectionFile;
   }
   else if (ext == SpecFile::getBrainVoyagerFileExtension()) {
      af = new BrainVoyagerFile;
   }
   else if (ext == SpecFile::getCellFileExtension()) {
      af = new CellFile;
   }
   else if (ext == SpecFile::getCellColorFileExtension()) {
      af = new CellColorFile;
   }
   else if (ext == SpecFile::getCellProjectionFileExtension()) {
      af = new CellProjectionFile;
   }
   else if (ext == SpecFile::getCocomacConnectivityFileExtension()) {
      af = new CocomacConnectivityFile;
   }
   else if (ext == SpecFile::getContourFileExtension()) {
      af = new ContourFile;
   }
   else if (ext == SpecFile::getContourCellColorFileExtension()) {
      af = new ContourCellColorFile;
   }
   else if (ext == SpecFile::getContourCellFileExtension()) {
      af = new ContourCellFile;
   }
   else if (ext == SpecFile::getCoordinateFileExtension()) {
      af = new CoordinateFile;
   }
   else if (ext == SpecFile::getCutsFileExtension()) {
      af = new CutsFile;
   }
   else if (ext == SpecFile::getDeformationMapFileExtension()) {
      af = new DeformationMapFile;
   }
   else if (ext == SpecFile::getDeformationFieldFileExtension()) {
      af = new DeformationFieldFile;
   }
   else if (ext == SpecFile::getFreeSurferAsciiCurvatureFileExtension()) {
      af = new FreeSurferCurvatureFile;
      af->setFileReadType(AbstractFile::FILE_FORMAT_ASCII);
   }
   else if (ext == SpecFile::getFreeSurferBinaryCurvatureFileExtension()) {
      af = new FreeSurferCurvatureFile;
      af->setFileReadType(AbstractFile::FILE_FORMAT_BINARY);
   }
   else if (ext == SpecFile::getFreeSurferAsciiFunctionalFileExtension()) {
      af = new FreeSurferFunctionalFile;
      af->setFileReadType(AbstractFile::FILE_FORMAT_ASCII);
   }
   else if (ext == SpecFile::getFreeSurferBinaryFunctionalFileExtension()) {
      af = new FreeSurferFunctionalFile;
      af->setFileReadType(AbstractFile::FILE_FORMAT_BINARY);
   }
   else if (ext == SpecFile::getFreeSurferLabelFileExtension()) {
      af = new FreeSurferLabelFile;
   }
   else if (ext == SpecFile::getFreeSurferAsciiSurfaceFileExtension()) {
      af = new FreeSurferSurfaceFile;
      af->setFileReadType(AbstractFile::FILE_FORMAT_ASCII);
   }
   else if (ext == SpecFile::getFociFileExtension()) {
      af = new FociFile;
   }
   else if (ext == SpecFile::getFociColorFileExtension()) {
      af = new FociColorFile;
   }
   else if (ext == SpecFile::getFociProjectionFileExtension()) {
      af = new FociProjectionFile;
   }
   else if (ext == SpecFile::getLatLonFileExtension()) {
      af = new LatLonFile;
   }
   else if (ext == SpecFile::getMetricFileExtension()) {
      af = new MetricFile;
   }
   else if (ext == SpecFile::getPaintFileExtension()) {
      af = new PaintFile;
   }
   else if (ext == SpecFile::getPaletteFileExtension()) {
      af = new PaletteFile;
   }
   else if (ext == SpecFile::getParamsFileExtension()) {
      af = new ParamsFile;
   }
   else if (ext == SpecFile::getPreferencesFileExtension()) {
      af = new PreferencesFile;
   }
   else if (ext == SpecFile::getProbabilisticAtlasFileExtension()) {
      af = new ProbabilisticAtlasFile;
   }
   else if (ext == SpecFile::getRgbPaintFileExtension()) {
      af = new RgbPaintFile;
   }
   else if (ext == SpecFile::getSceneFileExtension()) {
      af = new SceneFile;
   }
   else if (ext == SpecFile::getSectionFileExtension()) {
      af = new SectionFile;
   }
   else if (ext == SpecFile::getSpecFileExtension()) {
      af = new SpecFile;
   }
   else if (ext == SpecFile::getSurfaceShapeFileExtension()) {
      af = new SurfaceShapeFile;
   }
   else if (ext == SpecFile::getSurfaceVectorFileExtension()) {
      af = new SurfaceVectorFile;
   }
   else if (ext == SpecFile::getTopographyFileExtension()) {
      af = new TopographyFile;
   }
   else if (ext == SpecFile::getTopoFileExtension()) {
      af = new TopologyFile;
   }
   else if (ext == SpecFile::getTransformationMatrixFileExtension()) {
      af = new TransformationMatrixFile;
   }
   else if (ext == SpecFile::getAnalyzeVolumeFileExtension()) {
      af = new VolumeFile;
   }
   else if (ext == SpecFile::getAfniVolumeFileExtension()) {
      af = new VolumeFile;
   }
   else if (ext == SpecFile::getWustlVolumeFileExtension()) {
      af = new VolumeFile;
   }
   else if (ext == SpecFile::getNiftiVolumeFileExtension()) {
      af = new VolumeFile;
   }
   else if (ext == SpecFile::getVtkModelFileExtension()) {
      af = new VtkModelFile;
   }
   else if (ext == SpecFile::getTopoFileExtension()) {
      af = new TopologyFile;
   }
   else if (ext == SpecFile::getGeodesicDistanceFileExtension()) {
      af = new GeodesicDistanceFile;
   }
   else if (ext == SpecFile::getWustlRegionFileExtension()) {
      af = new WustlRegionFile;
   }
   else if (ext == SpecFile::getVectorFileExtension()) {
      af = new VectorFile;
   }
   else if (ext == SpecFile::getGiftiFileExtension()) {
      af = new GiftiDataArrayFile;
   }
   else {
      errorMessageOut = "Unrecognized file extension = ";
      errorMessageOut.append(ext);
      return NULL;
   }
   
   return af;
#else  // CARET_FLAG
   //
   // Must be GIFTI API
   //
   if (ext == ".gii") {
      af = new GiftiDataArrayFile;
   }
   else {
      errorMessageOut = "Unrecognized file extension = ";
      errorMessageOut.append(ext);
      return NULL;
   }
   return af;
#endif // CARET_FLAG
}

/**
 * Get a file type name based upon its extension.  If the file's type is not
 * recognized, the file name's extension is returned.
 */
QString
AbstractFile::getFileTypeNameFromFileName(const QString& filename)
{
   QString typeName;
   
   QString errorMessageOut;
   AbstractFile* af = getSubClassDataFile(filename, errorMessageOut);
   if (af != NULL) {
      typeName = af->getDescriptiveName();
      delete af;
   }
   if (typeName.isEmpty()) {
      typeName = FileUtilities::filenameExtension(filename);
      if (typeName == "gz") {
         QString f(FileUtilities::filenameWithoutExtension(filename));
         typeName = FileUtilities::filenameExtension(f);
      }
      if ((typeName == "BRIK") ||
          (typeName == "img")) {
         typeName = "Volume File Data";
      }
   }
   if (typeName.isEmpty()) {
      typeName = "unknown";
   }
   
   //std::cout << "Filename: " << filename 
   //          << " Typename: " << typeName << std::endl;
   return typeName;
}

/**
 * set the default file name prefix.
 */
void 
AbstractFile::setDefaultFileNamePrefix(const QString& s,
                                       const int numNodes)
{
   defaultFileNamePrefix = s;
   defaultFileNameNumberOfNodes = numNodes;
}

/**
 * get the default file name prefix.
 */
void 
AbstractFile::getDefaultFileNamePrefix(QString& prefix, int& numNodes)
{
   prefix = defaultFileNamePrefix;
   numNodes = defaultFileNameNumberOfNodes;
}
      
/**
 * Read any Caret data file.  The type of file is determined by its extension.  The caller
 * must "dynamic_cast" the returned pointer to the appropriate file type to access
 * class specific data/methods.  If returned pointer is NULL check the errorMessage
 * to see a description of why the file was not read successfully.
 */
AbstractFile*
AbstractFile::readAnySubClassDataFile(const QString& filename,
                                      const bool readMetaDataOnly,
                                      QString& errorMessageOut)
{
   AbstractFile* af = getSubClassDataFile(filename, errorMessageOut);
   if (af == NULL) {
      return NULL;
   }
   
   try {
      af->setReadMetaDataOnlyFlag(readMetaDataOnly);
      af->readFile(filename);
      if (readMetaDataOnly) {
         af->setReadMetaDataOnlyFlag(false);
      }
   }
   catch (FileException& e) {
      if (af != NULL) {
         delete af;
      }
      errorMessageOut = e.what();
      return NULL;
   }
   
   return af;
}

/// used by AbstractFile
class TypeExt {
   public:
      TypeExt(const QString& e, const QString& t) {
         extension = e.mid(5);
         typeName = t;
         typeName.append(" (*.");
         typeName.append(extension);
         typeName.append(")");
      }
      
      bool operator<(const TypeExt& te) const {
         return (typeName < te.typeName);
      }
      
      QString typeName;
      QString extension;
};

/**
 * Get all filetype names and extensions.
 */
void
AbstractFile::getAllFileTypeNamesAndExtensions(std::vector<QString>& typeNames,
                                               std::vector<QString>& extensions)
{
   typeNames.clear();
   extensions.clear();
   
   QString ext;
   std::vector<TypeExt> typeAndExtension;

#ifdef CARET_FLAG   
   ext = "file";
   ext.append(SpecFile::getTopoFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getCoordinateFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getLatLonFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getSectionFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getTransformationMatrixFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getPaintFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getAreaColorFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getRgbPaintFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getBorderFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getBorderColorFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getBorderProjectionFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getPaletteFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getTopographyFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getCellFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getCellColorFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getCellProjectionFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getContourFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getContourCellFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getContourCellColorFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getFociFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getFociColorFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getFociProjectionFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getProbabilisticAtlasFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getMetricFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getSurfaceShapeFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getSurfaceVectorFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getCocomacConnectivityFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getArealEstimationFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getCutsFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getParamsFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getDeformationMapFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getDeformationFieldFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getVtkModelFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getAtlasSpaceFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getAtlasSurfaceDirectoryFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getBrainVoyagerFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getFreeSurferAsciiCurvatureFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getFreeSurferBinaryCurvatureFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getFreeSurferAsciiFunctionalFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getFreeSurferBinaryFunctionalFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getFreeSurferLabelFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getFreeSurferAsciiSurfaceFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getSpecFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getAnalyzeVolumeFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getAfniVolumeFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getWustlVolumeFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getNiftiVolumeFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getWustlRegionFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGeodesicDistanceFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getSceneFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));   
   ext = "file";
   ext.append(".jpg");
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getVectorFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
#endif // CARET_FLAG   

   std::sort(typeAndExtension.begin(), typeAndExtension.end());
   
   for (unsigned int i = 0; i < typeAndExtension.size(); i++) {
      extensions.push_back(typeAndExtension[i].extension);
      typeNames.push_back(typeAndExtension[i].typeName);
   }
}

/**
 * load the file's data into a QStringTable (returns NULL if not supported).
 */
QStringTable* 
AbstractFile::loadDataIntoQStringTable() const
{
   return NULL;
}      

/**
 * allow events to process (call when doing "busy" operations
 */
void 
AbstractFile::allowEventsToProcess()
{
   if (qApp != NULL) {
      qApp->processEvents();
   }
}

/**
 * set the number of digits right of the decimal when writing float to text files.
 */
void 
AbstractFile::setTextFileDigitsRightOfDecimal(const int num) 
{ 
   textFileDigitsRightOfDecimal = num; 
   StringUtilities::setFloatDigitsRightOfDecimal(num);
}

/**
 * read a 3 byte integer from binary file.
 */
int 
AbstractFile::readThreeByteInteger(QDataStream& binStream) throw (FileException)
{
   quint8 d[3];
   binStream >> d[0] >> d[1] >> d[2];
   int i[3] = { d[0], d[1], d[2] };
   
   const int result = i[0] * 65536 + i[1] * 256 + i[2];
   
   return result;
}
    
#ifdef QT4_FILE_POS_BUG
/**
 * set file postion for binary files for QT4 bug work around.
 */
void 
AbstractFile::setBinaryFilePosQT4Bug() throw (FileException)
{
   if (getFileWriteType() == FILE_FORMAT_BINARY) {
      if (writingQFile != NULL) {
         if (writingQFile->isOpen()) {
            writingQFile->close();
      
            if (writingQFile->open(QIODevice::Append) == false) {
               throw FileException(filename, "Unable to close and open file for appending\n"
                                       "to work around bugs in QT4 software.");
            }
         }
      }
   }
}
#endif // QT4_FILE_POS_BUG
 
#ifdef QT4_FILE_POS_BUG
/**
 * find the binary data offset for QT4 bug work around (returns >= 0 if found).
 */
qint64 
AbstractFile::findBinaryDataOffsetQT4Bug(QFile& file,
                                         const char* stringToFind) throw (FileException)
{
   file.seek(0);
   
   QDataStream dataStream(&file);
   
   const int arraySize = 2048;
   char buffer[arraySize];
   const int bufferLength = arraySize - 1;
   qint64 offset = 0;
   const qint64 fileSize = file.size() - 1;
   
   bool firstTime = true;
   while (firstTime || (offset < fileSize)) {
      firstTime = false;
      
      const qint64 numRead = dataStream.readRawData(buffer, bufferLength);
      if (numRead > 0) {
         buffer[bufferLength] = '\0';
         const char* found = strstr(buffer, stringToFind);
         if (found != NULL) {
            const qint64 dataOffset = (qint64)(found - buffer) + offset + strlen(stringToFind);
            if (DebugControl::getDebugOn()) {
               std::cout << "Binary data offset is: " << dataOffset << std::endl;
            }
            return dataOffset;
         }
         else if (numRead >= bufferLength) {
            offset += bufferLength - strlen(stringToFind);
            file.seek(offset);
         }
         else {
            break;
         }
      }
      else {
         break;
      }
   }
   
   return -1;
}
#endif // QT4_FILE_POS_BUG


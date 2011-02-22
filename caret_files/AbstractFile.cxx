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
#include <SystemUtilities.h>

#define _ABSTRACT_MAIN_
#include "AbstractFile.h"
#undef _ABSTRACT_MAIN_

#include "DateAndTime.h"
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
#include "CommaSeparatedValueFile.h"
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
#include "FociSearchFile.h"
#include "FreeSurferCurvatureFile.h"
#include "FreeSurferFunctionalFile.h"
#include "FreeSurferLabelFile.h"
#include "FreeSurferSurfaceFile.h"
#include "GiftiNodeDataFile.h"
#include "GeodesicDistanceFile.h"
#include "ImageFile.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "NodeRegionOfInterestFile.h"
#include "PaintFile.h"
#include "PaletteFile.h"
#include "ParamsFile.h"
#include "PreferencesFile.h"
#include "ProbabilisticAtlasFile.h"
#include "StringTable.h"
#include "RgbPaintFile.h"
#include "SceneFile.h"
#include "SectionFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "StudyCollectionFile.h"
#include "StudyMetaDataFile.h"
#include "SurfaceFile.h"
#include "SurfaceShapeFile.h"
#include "TextFile.h"
#include "TopographyFile.h"
#include "TopologyFile.h"
#include "TransformationMatrixFile.h"
#include "SureFitVectorFile.h"
#include "VectorFile.h"
#include "VocabularyFile.h"
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
                           const FILE_IO supportsOtherFormat,
                           const FILE_IO supportsCsvfFormat)
{
   if (preferredWriteType.empty()) {
      std::vector<AbstractFile::FILE_FORMAT> fileFormats;
      std::vector<QString> fileFormatNames;
      AbstractFile::getFileFormatTypesAndNames(fileFormats, fileFormatNames);
      preferredWriteType.resize(fileFormats.size());
      std::fill(preferredWriteType.begin(),
                preferredWriteType.end(),
                AbstractFile::FILE_FORMAT_BINARY);
   }
   
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
   fileSupportCommaSeparatedValueFile = supportsCsvfFormat;

   displayListNumber = 0;

   defaultFileName = StringUtilities::makeLowerCase(descriptiveName);
   defaultFileName = StringUtilities::replace(defaultFileName, ' ', '_');

   enableAppendFileComment = true;
   
   setXmlVersionReadWithSaxParser(false);
   readMetaDataOnlyFlag = false;

   //
   // Set write type and possibly override with the preferred write type
   //
   // NOTE: This code is also in the GiftiDataArrayFile constructor
   // since it may change the supported write types
   //fileWriteType = fileReadType;
   for (unsigned int i = 0; i < preferredWriteType.size(); i++) {
      if (getCanWrite(preferredWriteType[i])) {
         fileWriteType = preferredWriteType[i];
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
   setFileWriteType(af.fileWriteType);
   fileSupportAscii  = af.fileSupportAscii;
   fileSupportBinary = af.fileSupportBinary;
   fileSupportXML    = af.fileSupportXML;
   fileSupportXMLBase64 = af.fileSupportXMLBase64;
   fileSupportXMLGZipBase64 = af.fileSupportXMLGZipBase64;
   fileSupportOther  = af.fileSupportOther;
   fileSupportCommaSeparatedValueFile = af.fileSupportCommaSeparatedValueFile;
   enableAppendFileComment = af.enableAppendFileComment;
   readMetaDataOnlyFlag = af.readMetaDataOnlyFlag;
   rootXmlElementTagName = af.rootXmlElementTagName;
   xmlVersionReadWithSaxParserFlag = af.xmlVersionReadWithSaxParserFlag;
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
   // fileSupportCommaSeparatedValueFile
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
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         fileSupportCommaSeparatedValueFile = readAndOrWrite;
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
   const bool useDateInFileName = false;
   
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
         if (useDateInFileName) {
            str << "."
                << DateAndTime::getDateForNaming().toAscii().constData();
                //<< QDateTime::currentDateTime().toString("yyyy-MM-dd").toAscii().constData();
         }   
         if (defaultFileNameNumberOfNodes > 0) {
            bool showNumNodes = false;
#ifdef CARET_FLAG
            showNumNodes = 
               ((dynamic_cast<const GiftiNodeDataFile*>(this) != NULL) ||
                (dynamic_cast<const NodeAttributeFile*>(this) != NULL) ||
                (dynamic_cast<const CoordinateFile*>(this) != NULL) ||
                (dynamic_cast<const TopologyFile*>(this) != NULL));
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

/**
 * get the default file name (also overrides current file name).
 */
QString 
AbstractFile::makeDefaultFileName(const QString& description) const
{
   filename = "";
   return getFileName(description);
}
      
/**
 * replace the caret standard file name's description.
 */
void 
AbstractFile::replaceFileNameDescription(const QString& newDescription)
{
   QString name = getFileName();
   name = FileUtilities::changeCaretDataFileDescription(name, newDescription);
   setFileName(name);
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
 * get the study metadata link for this file.
 */
StudyMetaDataLinkSet 
AbstractFile::getStudyMetaDataLinkSet() const
{
   StudyMetaDataLinkSet smdls;
   const QString textLink = getHeaderTag(headerTagStudyMetaDataLinkSet);
   if (textLink.isEmpty() == false) {
      smdls.setLinkSetFromCodedText(textLink);
   }
   return smdls;
}

/**
 * set the study metadata link for this file.
 */
void 
AbstractFile::setStudyMetaDataLinkSet(const StudyMetaDataLinkSet smdls)
{
   setHeaderTag(headerTagStudyMetaDataLinkSet,
                smdls.getLinkSetAsCodedText());
}
      
/**
 * Get a tag from the header.
 */
QString
AbstractFile::getHeaderTag(const QString& name) const
{
   const QString nameLower(name.toLower());
   
   for (AbstractFileHeaderContainer::const_iterator iter = header.begin();
        iter != header.end();
        iter++) {
      if (nameLower == iter->first.toLower()) {
         return iter->second;
         break;
      }
   }

/*
   const std::map<QString, QString>::const_iterator iter = header.find(name);
   if (iter != header.end()) {
      return iter->second;
   }
*/
   return "";
}

/**
 * Set a tag in the header.
 */
void
AbstractFile::setHeaderTag(const QString& name, const QString& value)
{
   QString nameLower(name.toLower());
   
   //
   // "hem_flag" has been replaced by "structure"
   //
   if (nameLower == "hem_flag") {
      nameLower = AbstractFile::headerTagStructure;
   }
   
   //
   // ignore "version_id"
   //
   if (nameLower == headerTagVersionID) {
      return;
   }
   
   //
   // Since case may vary, remove matching item
   //
   for (AbstractFileHeaderContainer::iterator iter = header.begin();
        iter != header.end();
        iter++) {
      const QString tagName(iter->first);
      const QString tagNameLower(tagName.toLower());
      if (nameLower == tagNameLower) {
         header.erase(iter);
         break;
      }
   }
   
   //
   // Add to header
   //
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
 * get the file's PubMed ID.
 */
QString 
AbstractFile::getFilePubMedID() const
{
   QString s(getHeaderTag(headerTagPubMedID));
   return s;
}

/**
 * set the file's PubMed ID.
 */
void 
AbstractFile::setFilePubMedID(const QString& pmid)
{
   setHeaderTag(headerTagPubMedID, pmid);
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
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         fio = fileSupportCommaSeparatedValueFile;
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
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         fio = fileSupportCommaSeparatedValueFile;
         break;
   }
   
   const bool b = ((fio == FILE_IO_WRITE_ONLY) || (fio == FILE_IO_READ_AND_WRITE));
   return b;
}
      
/**
 * convert a format type to its name.
 */
QString 
AbstractFile::convertFormatTypeToName(const FILE_FORMAT formatIn)
{
   QString s;
   
   switch (formatIn) {
      case FILE_FORMAT_ASCII:
         s = getHeaderTagEncodingValueAscii();
         break;
      case FILE_FORMAT_BINARY:
         s = getHeaderTagEncodingValueBinary();
         break;
      case FILE_FORMAT_XML:
         s = getHeaderTagEncodingValueXML();
         break;
      case FILE_FORMAT_XML_BASE64:
         s = getHeaderTagEncodingValueXMLBase64();
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         s = getHeaderTagEncodingValueXMLGZipBase64();
         break;
      case FILE_FORMAT_OTHER:
         s = getHeaderTagEncodingValueOther();
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         s = getHeaderTagEncodingValueCommaSeparatedValueFile();
         break;
   }
   
   return s;
}
      
/**
 * convert a file format name to its type.
 */
AbstractFile::FILE_FORMAT 
AbstractFile::convertFormatNameToType(const QString& name,
                                      bool* validNameOut)
{
   if (validNameOut != NULL) {
      *validNameOut = true;
   }
   
   FILE_FORMAT format = FILE_FORMAT_ASCII;
   if (name == getHeaderTagEncodingValueAscii()) {
      format = FILE_FORMAT_ASCII;
   }
   else if (name == getHeaderTagEncodingValueBinary()) {
      format = FILE_FORMAT_BINARY;
   }
   else if (name == getHeaderTagEncodingValueXML()) {
      format = FILE_FORMAT_XML;
   }
   else if (name == getHeaderTagEncodingValueXMLBase64()) {
      format = FILE_FORMAT_XML_BASE64;
   }
   else if (name == getHeaderTagEncodingValueXMLGZipBase64()) {
      format = FILE_FORMAT_XML_GZIP_BASE64;
   }
   else if (name == getHeaderTagEncodingValueOther()) {
      format = FILE_FORMAT_OTHER;
   }
   else if (name == getHeaderTagEncodingValueCommaSeparatedValueFile()) {
      format = FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE;
   }
   else {
      if (validNameOut != NULL) {
         *validNameOut = false;
      }
   }   
   
   return format;
}

/**
 * get file format types and names.
 */
void 
AbstractFile::getFileFormatTypesAndNames(std::vector<FILE_FORMAT>& typesOut,
                                         std::vector<QString>& namesOut)
{
   typesOut.clear();
   namesOut.clear();
   
   typesOut.push_back(FILE_FORMAT_ASCII);
   namesOut.push_back(convertFormatTypeToName(FILE_FORMAT_ASCII));
   
   typesOut.push_back(FILE_FORMAT_BINARY);
   namesOut.push_back(convertFormatTypeToName(FILE_FORMAT_BINARY));
   
   typesOut.push_back(FILE_FORMAT_XML);
   namesOut.push_back(convertFormatTypeToName(FILE_FORMAT_XML));
   
   typesOut.push_back(FILE_FORMAT_XML_BASE64);
   namesOut.push_back(convertFormatTypeToName(FILE_FORMAT_XML_BASE64));
   
   typesOut.push_back(FILE_FORMAT_XML_GZIP_BASE64);
   namesOut.push_back(convertFormatTypeToName(FILE_FORMAT_XML_GZIP_BASE64));
   
   typesOut.push_back(FILE_FORMAT_OTHER);
   namesOut.push_back(convertFormatTypeToName(FILE_FORMAT_OTHER));
   
   typesOut.push_back(FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE);
   namesOut.push_back(convertFormatTypeToName(FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE));
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
 * determine if the file is a comma separated value file.
 */
bool 
AbstractFile::isCommaSeparatedValueFile(QFile& file)
{
   bool isCSVF = false;
   
   //
   // There is a bug in some versions of QT in that QFile::peek() returns 
   // 1 fewer characters than it should, so read extra
   //
   const QString id(CommaSeparatedValueFile::getFirstLineCommaSeparatedValueFileIdentifier());
   const qint64 len = id.length() + 5;
   
   if (len > 0) {
      const QString charsRead = file.peek(len);
      if (charsRead.indexOf(id) >= 0) {
         isCSVF = true;
      }
   }
   
   return isCSVF;
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
   const qint64 filePos = file.pos();
   
   //
   // Create a buffer for the data read
   //
   const qint64 NUM = 512;
   unsigned char buffer[NUM];
   
   //
   // Set number of bytes to read
   //
   const int numToRead = std::min(file.size(), NUM);
   
   //
   // Read the bytes
   //
   const qint64 numRead = file.read((char*)(buffer), numToRead);
   
   //
   // loop through characters
   //
   for (qint64 i = 0; i < numRead; i++) {
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
         else if ((c == 32) ||  // blank character (space)
                  (c == 9)  ||  // tab
                  (c == 10) ||  // line feed
                  (c == 13)) {  // carriage return
            // ignore whitespace
         }
         else {
            isXML = false;
            break;
         }
      }
/*
      else if ((c != 9) &&   // tab
               (c != 10) &&  // line feed
               (c != 13)) {  // carriage return
         isXML = false;
         break;
      }
*/
   }
   
   //
   // Restor position in file
   //
   file.seek(filePos);
   
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
   if (file.open(QFile::ReadWrite) == false) {
      throw FileException("", "Unable to create temporary read/write file in AbstractFile::readFile");
   }
   QDataStream stream(&file);
   stream.writeRawData(data, dataLength);
   //char newline[2] = { '\n', '\0' };
   //stream.writeRawBytes(newline, 1);
   file.seek(0);
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
   bool csvFileFlag = false;
   bool xmlFileFlag = false;
   if (getCanRead(FILE_FORMAT_OTHER) == false) {
      xmlFileFlag = isFileXML(file);
      stream.seek(0);
      file.seek(0);
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
      
      if (isCommaSeparatedValueFile(file)) {
         csvFileFlag = true;
      }
   }
   
   //
   // Special processing for TextFile
   //
   if (dynamic_cast<TextFile*>(this) != NULL) {
      csvFileFlag = false;
      xmlFileFlag = false;
   }
   
   QDomDocument doc(rootXmlElementTagName);
   QDomElement  rootElement;
   
   //
   // Gifti Node data files do not use DOM parser
   //
   if ((xmlFileFlag) && 
       (giftiDataArrayFileFlag == false) &&
       (getXmlVersionReadWithSaxParser() == false)) {
      //
      // Parse the file if it is an XML file
      //
      QString errorMessage;
      int errorLine = 0, errorColumn = 0;
      if (doc.setContent(&file, &errorMessage, &errorLine, &errorColumn) == false) {
         std::ostringstream str;
         str << "Error parsing XML at line " << errorLine << " column " << errorColumn << ".  ";
         str << errorMessage.toAscii().constData() << std::ends;
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
         if (rootXmlElementTagName != "html") {
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
   }
   
   if (giftiDataArrayFileFlag && xmlFileFlag) {
      fileReadType = FILE_FORMAT_XML;
   }
   else if (getXmlVersionReadWithSaxParser() && xmlFileFlag) {
      fileReadType = FILE_FORMAT_XML;
   }
   else if (fileHasHeader) {
      if (xmlFileFlag) {
         readHeaderXML(rootElement);
         setHeaderTag(headerTagEncoding, getHeaderTagEncodingValueXML());
      }
      else if (csvFileFlag) {
         setHeaderTag(headerTagEncoding, getHeaderTagEncodingValueCommaSeparatedValueFile());
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
      if (encoding == getHeaderTagEncodingValueAscii()) {
         if (getCanRead(FILE_FORMAT_ASCII) == false) {
            throw FileException(getFileName(), 
                   "Ascii format file not supported for this type of file.\n"
                   "Perhaps you need a newer version of Caret.");
         }
         fileReadType = FILE_FORMAT_ASCII;
      }
      else if (encoding == getHeaderTagEncodingValueBinary()) {
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
      else if (encoding == getHeaderTagEncodingValueXML()) {
         if (getCanRead(FILE_FORMAT_XML) == false) {
            throw FileException(getFileName(), 
                   "XML format file not supported for this type of file.\n"
                   "Perhaps you need a newer version of Caret.");
         }
         fileReadType = FILE_FORMAT_XML;
      }
      else if (encoding == getHeaderTagEncodingValueCommaSeparatedValueFile()) {
         if (getCanRead(FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE) == false) {
            throw FileException(getFileName(), 
                   "Comma Separated Value format file not supported for this type of file.\n"
                   "Perhaps you need a newer version of Caret.");
         }
         fileReadType = FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE;
      }
      else if (encoding == getHeaderTagEncodingValueOther()) {
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
      setFileWriteType(fileReadType);
   }
   for (unsigned int i = 0; i < preferredWriteType.size(); i++) {
      if (getCanWrite(preferredWriteType[i])) {
         fileWriteType = preferredWriteType[i];
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
   if (filenameIn.isEmpty()) {
      throw FileException("Filename for reading a file of type "
                          + descriptiveName
                          + "is empty.");
   }

   QFileInfo fi(filenameIn);
   if (fi.exists()) {
      if (fi.isDir()) {
         throw FileException(filenameIn + " is a directory, not a file");
      }
   }
   else {
      throw FileException(filenameIn + " does not exist.");
   }
   
   // Note: filenameIn could possibly be "this's" filename so make a
   // copy of it before calling "clear()" to prevent it from being erased.
   const QString filenameIn2(filenameIn);
   
   clear();

   filename = filenameIn2;
   
   QFile file(getFileName());
   const float fileSize = file.size() / 1048576.0;

   QTime timer;
   timer.start();
   
   if (file.open(QFile::ReadOnly)) {

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
      throw FileException(getFileName(), file.errorString()); //"Failure trying to open: ");
   }
   
   //
   // See how long it took to read the file
   //
   timeToReadFileInSeconds = static_cast<float>(timer.elapsed()) / 1000.0;
   if (DebugControl::getDebugOn() ||
       DebugControl::getFileReadTimingFlag()) {
      std::cout << "Time to read " << getFileName().toAscii().constData() 
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
      // Is this the old style header node ?
      //          
      // <header>
      //    <caret-version><![CDATA[5.402]]></caret-version>
      //    <comment><![CDATA[]]></comment>
      //    <date><![CDATA[Fri Jun 30 14:39:49 2006]]></date>
      //    <encoding><![CDATA[XML]]></encoding>
      // </header>
      //
      if (rootChildNode.toElement().tagName() == xmlHeaderOldTagName) {
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
      else if (rootChildNode.toElement().tagName() == xmlHeaderTagName) {
         //
         // Is this the current style header node ?
         //          
         //
         QDomNode node = rootChildNode.firstChild();
         while (node.isNull() == false) {
            QDomElement elem = node.toElement();
            if (elem.isNull() == false) {
               if (elem.tagName() == xmlHeaderElementTagName) {
                  const QDomNode nameNode = elem.namedItem(xmlHeaderElementName);
                  const QDomNode valueNode = elem.namedItem(xmlHeaderElementValue);
                  if ((nameNode.isNull() == false) &&
                      (valueNode.isNull() == false)) {
                     const QDomElement nameElement = nameNode.toElement();
                     const QDomElement valueElement = valueNode.toElement();
                     if ((nameElement.isNull() == false) &&
                         (valueElement.isNull() == false)) {
                        const QString tagName = getXmlElementFirstChildAsString(nameElement);
                        const QString tagValue = getXmlElementFirstChildAsString(valueElement);
                        if (tagName.isEmpty() == false) {
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
                     }
                  }
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
      QDomText    tagName = doc.createCDATASection(tag);
      QDomElement elementName = doc.createElement(xmlHeaderElementName);
      elementName.appendChild(tagName);
      QDomText    tagValue = doc.createCDATASection(value);
      QDomElement elementValue = doc.createElement(xmlHeaderElementValue);
      elementValue.appendChild(tagValue);
      QDomElement elem = doc.createElement(xmlHeaderElementTagName);
      elem.appendChild(elementName);
      elem.appendChild(elementValue);
      
      headerElement.appendChild(elem);
   }
   
   //
   // Add the header to the root element
   //
   rootElement.appendChild(headerElement);
}
      
/**
 * Write header to XML writer.
 */
void
AbstractFile::writeHeaderXMLWriter(XmlGenericWriter& xmlWriter) throw (FileException)
{
   if (this->header.begin() == this->header.end()) {
      return;
   }

   xmlWriter.writeStartElement(GiftiCommon::tagMetaData);
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

      xmlWriter.writeStartElement(GiftiCommon::tagMD);
      xmlWriter.writeElementCData(GiftiCommon::tagName, tag);
      xmlWriter.writeElementCData(GiftiCommon::tagValue, value);
      xmlWriter.writeEndElement();
   }

   xmlWriter.writeEndElement();
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
 * Get an XML element's first child and return it as a double.
 */
double
AbstractFile::getXmlElementFirstChildAsDouble(const QDomElement& elem)
{
   QString value;   
   QDomNode vNode = static_cast<QDomNode>(elem.firstChild());
   if (vNode.isNull() == false) {
      const QDomText textNode = vNode.toText();
      if (textNode.isNull() == false) {
         value = textNode.data();
      }
   }
   const double valueDouble = value.toDouble();
   return valueDouble;
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
                        
/*
 * add a Text element to an XML Dom Element.
 */
void 
AbstractFile::addXmlTextElement(QDomDocument& xmlDoc,
                                QDomElement& parentElement,
                                const QString& childElementName,
                                const QString& childElementData)
{
   //
   // Use CDATA so that the text can be anything
   //
   addXmlCdataElement(xmlDoc, parentElement, childElementName, childElementData);
/*
   QDomElement element = xmlDoc.createElement(childElementName);
   QDomText node = xmlDoc.createTextNode(childElementData);
   element.appendChild(node);
   parentElement.appendChild(element);
*/
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
 * add a float element to an XML Dom Element.
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
 * add a float element to an XML Dom Element.
 */
void 
AbstractFile::addXmlTextElement(QDomDocument& xmlDoc,
                                QDomElement& parentElement,
                                const QString& childElementName,
                                const double childElementData)
{
   QDomElement element = xmlDoc.createElement(childElementName);
   QDomText node = xmlDoc.createTextNode(QString::number(childElementData, 'f', 24));
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
   
   const qint64 origFilePos = stream.pos();
   
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
      
      //
      // Need to set so binary data read correctly
      //
      file.seek(stream.pos());
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
      if (file.open(QFile::ReadOnly) == false) {
         throw FileException(filename, "Failed to reopen file after closing file since there\n"
                                       "is no header to get around QT4 file bugs.");
      }
      file.seek(0);
      file.reset();
#else  // QT4_FILE_POS_BUG
      file.reset();
      stream.reset();
      file.seek(origFilePos);
      stream.seek(origFilePos);
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
   int commentCharPos = lineOut.indexOf('#');
   if (commentCharPos != -1) {
      lineOut.resize(commentCharPos);
   }
}

#ifdef QT4_FILE_POS_BUG
static const int BIG_BUFFER_SIZE = 1024 * 1024;
static char bigBuffer[BIG_BUFFER_SIZE];
#endif // QT4_FILE_POS_BUG

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
      // readLine BUG http://www.trolltech.com/developer/task-tracker/index_html?method=entry&id=104776
      // readLine BUG readLine() will read only a maximum of 4095 characters
      // readLine BUG lineOut = file->readLine();
      if (file->readLine(bigBuffer, BIG_BUFFER_SIZE) > 0) {
         lineOut = bigBuffer;
         lineOut = lineOut.trimmed();
      }
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
      // readLine BUG http://www.trolltech.com/developer/task-tracker/index_html?method=entry&id=104776
      // readLine BUG readLine() will read only a maximum of 4095 characters
      // readLine BUG lineOut = file->readLine();
      if (file->readLine(bigBuffer, BIG_BUFFER_SIZE) > 0) {
         lineOut = bigBuffer;
      }
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
   QTextStream(&line, QIODevice::ReadOnly) >> tagStr;
   
   if (tagStr.isEmpty()) {
      return;
      //throw FileException(filename, "Invalid line in tags section.");
   }
   tag = tagStr;

   char* lineCh = new char[line.length() + 1];
   strcpy(lineCh, line.toAscii().constData());
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
   QTextStream(&line, QIODevice::ReadOnly) >> tagStr;
   if (debugFlag) std::cout << "tagStr: " << tagStr.toAscii().constData() << std::endl;
  
   if (tagStr.isEmpty()) {
      return;
      //throw FileException(filename, "Invalid line in tags section.");
   }
   tag = tagStr;

   char* lineCh = new char[line.length() + 1];
   strcpy(lineCh, line.toAscii().constData());
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
   QTextStream(&line, QIODevice::ReadOnly) >> tagStr;
   if (debugFlag) std::cout << "tagStr: " << tagStr.toAscii().constData() << std::endl;
  
   if (tagStr.isEmpty()) {
      return;
      //throw FileException(filename, "Invalid line in tags section.");
   }
   tag = tagStr;

   char* lineCh = new char[line.length() + 1];
   strcpy(lineCh, line.toAscii().constData());
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
   QTextStream(&qline, QIODevice::ReadOnly) >> numberRead
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
   int pos = line.indexOf(tag);
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
   QStringList sl = line.split(" ");
   return sl;
   //return QStringList::split(" ", line);
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
   QTextStream ts(ba, QIODevice::WriteOnly);
   QDataStream ds(&ba, QIODevice::WriteOnly);
   writeFileContents(ts, ds);
}
      
/**
 * method used for writing files.
 */
void 
AbstractFile::writeFileContents(QTextStream& stream, QDataStream& dataStream) throw (FileException)
{
   bool isXmlFile = false;
   bool isCsvFile = false;
   
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
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         if (getCanWrite(FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE) == false) {
            throw FileException(filename, "\"Comma Separated File Format\" type file not supported.");
         }
         isCsvFile = true;
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

   stream.setRealNumberNotation(QTextStream::FixedNotation);
   stream.setRealNumberPrecision(textFileDigitsRightOfDecimal);
   if (fileHasHeader) {
      setHeaderTag(headerTagDate, QDateTime::currentDateTime().toString(Qt::ISODate));
      switch (fileWriteType) {
         case FILE_FORMAT_ASCII:
            setHeaderTag(headerTagEncoding, getHeaderTagEncodingValueAscii());
            break;
         case FILE_FORMAT_BINARY:
            setHeaderTag(headerTagEncoding, getHeaderTagEncodingValueBinary());
            break;
         case FILE_FORMAT_XML:
            setHeaderTag(headerTagEncoding, getHeaderTagEncodingValueXML());
            break;
         case FILE_FORMAT_XML_BASE64:
            setHeaderTag(headerTagEncoding, getHeaderTagEncodingValueXMLBase64());
            break;
         case FILE_FORMAT_XML_GZIP_BASE64:
            setHeaderTag(headerTagEncoding, getHeaderTagEncodingValueXMLGZipBase64());
            break;
         case FILE_FORMAT_OTHER:
            setHeaderTag(headerTagEncoding, getHeaderTagEncodingValueOther());
            break;
         case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
            setHeaderTag(headerTagEncoding, getHeaderTagEncodingValueCommaSeparatedValueFile());
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
      else if (isCsvFile) {
      }
      else {
         writeHeader(stream);
      }
   }
   
#ifdef QT4_FILE_POS_BUG
   setBinaryFilePosQT4Bug();
#else
   //
   // Still a bug in QT4
   //
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
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         if (getCanWrite(FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE) == false) {
            throw FileException(filename, "\"Comma Separated Value File\" type file not supported.");
         }
         break;
   }

   QTime timer;
   timer.start();

   writingQFile = new QFile(this->filename);
   if (AbstractFile::getOverwriteExistingFilesAllowed() == false) {
      if (writingQFile->exists()) {
         throw FileException("file exists and overwrite is prohibited.");
      }
   }
   QString errMsg;
   if (writingQFile->open(QFile::WriteOnly)) {
      QTextStream stream(writingQFile);
      QDataStream binStream(writingQFile);
      
      try {
         writeFileContents(stream, binStream);
            
         writingQFile->close();
         delete writingQFile;
         writingQFile = NULL;
         
         //
         // Update file permissions ?
         //
         if (fileWritePermissions != 0) {
            QFile::setPermissions(getFileName(), fileWritePermissions);
         }
      }
      catch (FileException& e) {
         writingQFile->close();
         errMsg = writingQFile->errorString();
         delete writingQFile;
         writingQFile = NULL;
         throw e;
      }
      clearModified();      
   }
   else {
      errMsg = " Open for writing, " + writingQFile->errorString();
      delete writingQFile;
      writingQFile = NULL;

      throw FileException(getFileName(), errMsg);
/*
      QFileInfo fileInfo(getFileName());
      if (fileInfo.exists() && (fileInfo.isWritable() == false)) {
         throw FileException(getFileName(), "File exists but it is not writable");
      }
      else {
         QString msg("Unable to open for writing.  Current path: ");
         msg.append(QDir::currentPath());
         throw FileException(getFileName(), msg);
      }
*/
   }

   //
   // See how long it took to read the file
   //
   const float timeToWriteFileInSeconds = static_cast<float>(timer.elapsed()) / 1000.0;
   if (DebugControl::getDebugOn()) {
      std::cout << "Time to write " << getFileName().toAscii().constData()
                << "  was "
                << timeToWriteFileInSeconds
                << " seconds." << std::endl;
   }
}

/**
 * Update the file's metadata for Caret6.
 */
void
AbstractFile::updateMetaDataForCaret6()
{
   this->removeHeaderTag("encoding");
   this->removeHeaderTag("pubmed_id");
   this->setHeaderTag("Caret-Version", CaretVersion::getCaretVersionAsString());
   this->removeHeaderTag("date");
   this->setHeaderTag("Date", QDateTime::currentDateTime().toString(Qt::ISODate));
   this->setHeaderTag("UserName", SystemUtilities::getUserName());
}

/**
 * Write the file's memory in caret6 format to the specified name.
 */
QString
AbstractFile::writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException)
{
   throw FileException(filenameIn
                       + " cannot be written in Caret6 format at this time.");
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
   else if (ext == ".gz") {
      if (filename.endsWith(SpecFile::getNiftiGzipVolumeFileExtension())) {
         ext = SpecFile::getNiftiVolumeFileExtension();
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
   else if (filename.endsWith(SpecFile::getAreaColorFileExtension())) {
      af = new AreaColorFile;
   }
   else if (filename.endsWith(SpecFile::getArealEstimationFileExtension())) {
      af = new ArealEstimationFile;
   }
   else if (filename.endsWith(SpecFile::getAtlasSpaceFileExtension())) {
      af = new AtlasSpaceFile;
   }
   else if (filename.endsWith(SpecFile::getAtlasSurfaceDirectoryFileExtension())) {
      af = new AtlasSurfaceDirectoryFile;
   }
   else if (filename.endsWith(SpecFile::getBorderFileExtension())) {
      af = new BorderFile;
   }
   else if (filename.endsWith(SpecFile::getBorderColorFileExtension())) {
      af = new BorderColorFile;
   }
   else if (filename.endsWith(SpecFile::getBorderProjectionFileExtension())) {
      af = new BorderProjectionFile;
   }
   else if (filename.endsWith(SpecFile::getBrainVoyagerFileExtension())) {
      af = new BrainVoyagerFile;
   }
   else if (filename.endsWith(SpecFile::getCellFileExtension())) {
      af = new CellFile;
   }
   else if (filename.endsWith(SpecFile::getCellColorFileExtension())) {
      af = new CellColorFile;
   }
   else if (filename.endsWith(SpecFile::getCellProjectionFileExtension())) {
      af = new CellProjectionFile;
   }
   else if (filename.endsWith(SpecFile::getCocomacConnectivityFileExtension())) {
      af = new CocomacConnectivityFile;
   }
   else if (filename.endsWith(SpecFile::getContourFileExtension())) {
      af = new ContourFile;
   }
   else if (filename.endsWith(SpecFile::getContourCellColorFileExtension())) {
      af = new ContourCellColorFile;
   }
   else if (filename.endsWith(SpecFile::getContourCellFileExtension())) {
      af = new ContourCellFile;
   }
   else if (filename.endsWith(SpecFile::getCoordinateFileExtension())) {
      af = new CoordinateFile;
   }
   else if (filename.endsWith(SpecFile::getCutsFileExtension())) {
      af = new CutsFile;
   }
   else if (filename.endsWith(SpecFile::getDeformationMapFileExtension())) {
      af = new DeformationMapFile;
   }
   else if (filename.endsWith(SpecFile::getDeformationFieldFileExtension())) {
      af = new DeformationFieldFile;
   }
   else if (filename.endsWith(SpecFile::getFreeSurferAsciiCurvatureFileExtension())) {
      af = new FreeSurferCurvatureFile;
      af->setFileReadType(AbstractFile::FILE_FORMAT_ASCII);
   }
   else if (filename.endsWith(SpecFile::getFreeSurferBinaryCurvatureFileExtension())) {
      af = new FreeSurferCurvatureFile;
      af->setFileReadType(AbstractFile::FILE_FORMAT_BINARY);
   }
   else if (filename.endsWith(SpecFile::getFreeSurferAsciiFunctionalFileExtension())) {
      af = new FreeSurferFunctionalFile;
      af->setFileReadType(AbstractFile::FILE_FORMAT_ASCII);
   }
   else if (filename.endsWith(SpecFile::getFreeSurferBinaryFunctionalFileExtension())) {
      af = new FreeSurferFunctionalFile;
      af->setFileReadType(AbstractFile::FILE_FORMAT_BINARY);
   }
   else if (filename.endsWith(SpecFile::getFreeSurferLabelFileExtension())) {
      af = new FreeSurferLabelFile;
   }
   else if (filename.endsWith(SpecFile::getFreeSurferAsciiSurfaceFileExtension())) {
      af = new FreeSurferSurfaceFile;
      af->setFileReadType(AbstractFile::FILE_FORMAT_ASCII);
   }
   else if (filename.endsWith(SpecFile::getFociFileExtension())) {
      af = new FociFile;
   }
   else if (filename.endsWith(SpecFile::getFociColorFileExtension())) {
      af = new FociColorFile;
   }
   else if (filename.endsWith(SpecFile::getFociProjectionFileExtension())) {
      af = new FociProjectionFile;
   }
   else if (filename.endsWith(SpecFile::getFociSearchFileExtension())) {
      af = new FociSearchFile;
   }
   else if (filename.endsWith(SpecFile::getLatLonFileExtension())) {
      af = new LatLonFile;
   }
   else if (filename.endsWith(SpecFile::getMetricFileExtension())) {
      af = new MetricFile;
   }
   else if (filename.endsWith(SpecFile::getRegionOfInterestFileExtension())) {
      af = new NodeRegionOfInterestFile;
   }
   else if (filename.endsWith(SpecFile::getPaintFileExtension())) {
      af = new PaintFile;
   }
   else if (filename.endsWith(SpecFile::getPaletteFileExtension())) {
      af = new PaletteFile;
   }
   else if (filename.endsWith(SpecFile::getParamsFileExtension())) {
      af = new ParamsFile;
   }
   else if (filename.endsWith(SpecFile::getPreferencesFileExtension())) {
      af = new PreferencesFile;
   }
   else if (filename.endsWith(SpecFile::getProbabilisticAtlasFileExtension())) {
      af = new ProbabilisticAtlasFile;
   }
   else if (filename.endsWith(SpecFile::getRgbPaintFileExtension())) {
      af = new RgbPaintFile;
   }
   else if (filename.endsWith(SpecFile::getSceneFileExtension())) {
      af = new SceneFile;
   }
   else if (filename.endsWith(SpecFile::getSectionFileExtension())) {
      af = new SectionFile;
   }
   else if (filename.endsWith(SpecFile::getSpecFileExtension())) {
      af = new SpecFile;
   }
   else if (filename.endsWith(SpecFile::getSurfaceShapeFileExtension())) {
      af = new SurfaceShapeFile;
   }
   else if (filename.endsWith(SpecFile::getTextFileExtension())) {
      af = new TextFile;
   }
   else if (filename.endsWith(SpecFile::getTopographyFileExtension())) {
      af = new TopographyFile;
   }
   else if (filename.endsWith(SpecFile::getTopoFileExtension())) {
      af = new TopologyFile;
   }
   else if (filename.endsWith(SpecFile::getTransformationMatrixFileExtension())) {
      af = new TransformationMatrixFile;
   }
   else if (filename.endsWith(SpecFile::getAnalyzeVolumeFileExtension())) {
      af = new VolumeFile;
   }
   else if (filename.endsWith(SpecFile::getAfniVolumeFileExtension())) {
      af = new VolumeFile;
   }
   else if (filename.endsWith(SpecFile::getWustlVolumeFileExtension())) {
      af = new VolumeFile;
   }
   else if (filename.endsWith(SpecFile::getNiftiVolumeFileExtension())) {
      af = new VolumeFile;
   }
   else if (filename.endsWith(SpecFile::getNiftiGzipVolumeFileExtension())) {
      af = new VolumeFile;
   }
   else if (filename.endsWith(SpecFile::getVtkModelFileExtension())) {
      af = new VtkModelFile;
   }
   else if (filename.endsWith(SpecFile::getTopoFileExtension())) {
      af = new TopologyFile;
   }
   else if (filename.endsWith(SpecFile::getGeodesicDistanceFileExtension())) {
      af = new GeodesicDistanceFile;
   }
   else if (filename.endsWith(SpecFile::getWustlRegionFileExtension())) {
      af = new WustlRegionFile;
   }
   else if (filename.endsWith(SpecFile::getStudyMetaDataFileExtension())) {
      af = new StudyMetaDataFile;
   }
   else if (filename.endsWith(SpecFile::getStudyCollectionFileExtension())) {
      af = new StudyCollectionFile;
   }
   else if (filename.endsWith(SpecFile::getVocabularyFileExtension())) {
      af = new VocabularyFile;
   }
   else if (filename.endsWith(SpecFile::getSureFitVectorFileExtension())) {
      af = new SureFitVectorFile;
   }
   else if (filename.endsWith(SpecFile::getGiftiCoordinateFileExtension())) {
      af = new CoordinateFile;
   }
   else if (filename.endsWith(SpecFile::getGiftiFunctionalFileExtension())) {
      af = new MetricFile;
   }
   else if (filename.endsWith(SpecFile::getGiftiLabelFileExtension())) {
      af = new PaintFile;
   }
   else if (filename.endsWith(SpecFile::getGiftiRgbaFileExtension())) {
      af = new RgbPaintFile;
   }
   else if (filename.endsWith(SpecFile::getGiftiShapeFileExtension())) {
      af = new SurfaceShapeFile;
   }
   else if (filename.endsWith(SpecFile::getGiftiSurfaceFileExtension())) {
      af = new SurfaceFile;
   }
   else if (filename.endsWith(SpecFile::getGiftiTensorFileExtension())) {
      af = NULL;  // not tensor file at this time
   }
   else if (filename.endsWith(SpecFile::getGiftiTimeSeriesFileExtension())) {
      af = new MetricFile;
   }
   else if (filename.endsWith(SpecFile::getGiftiTopologyFileExtension())) {
      af = new TopologyFile;
   }
   else if (filename.endsWith(SpecFile::getGiftiVectorFileExtension())) {
      af = new VectorFile;
   }
   else if (ext == SpecFile::getGiftiGenericFileExtension()) {
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
      errorMessageOut = e.whatQString();
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
   ext.append(SpecFile::getRegionOfInterestFileExtension());
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
   ext.append(SpecFile::getFociSearchFileExtension());
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
   ext.append(SpecFile::getGiftiVectorFileExtension());
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
   ext.append(SpecFile::getStudyMetaDataFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getVocabularyFileExtension());
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
   ext.append(SpecFile::getSureFitVectorFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiCoordinateFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiFunctionalFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiLabelFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiRgbaFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiShapeFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiSurfaceFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiTensorFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiTimeSeriesFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiTopologyFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getGiftiGenericFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
   ext = "file";
   ext.append(SpecFile::getTextFileExtension());
   typeAndExtension.push_back(TypeExt(ext, getFileTypeNameFromFileName(ext)));
#endif // CARET_FLAG   

   std::sort(typeAndExtension.begin(), typeAndExtension.end());
   
   for (unsigned int i = 0; i < typeAndExtension.size(); i++) {
      extensions.push_back(typeAndExtension[i].extension);
      typeNames.push_back(typeAndExtension[i].typeName);
   }
}

/**
 * find out if comma separated file conversion supported.
 */
void 
AbstractFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                           bool& writeToCSV) const
{
   readFromCSV = false;
   writeToCSV  = false;
}
                                        
/**
 * write the file's data into a comma separated values file (throws exception if not supported).
 */
void 
AbstractFile::writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& /*csv*/) throw (FileException)
{
   throw FileException("Comma Separated File not supported for this file type.");
}

/**
 * read the file's data from a comma separated values file (throws exception if not supported).
 */
void 
AbstractFile::readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& /*csv*/) throw (FileException)
{
   throw FileException("Comma Separated File not supported for this file type.");
}

      
/**
 * write the header's data into a StringTable.
 */
void 
AbstractFile::writeHeaderDataIntoStringTable(StringTable& table) const
{
   const int numRows = header.size();
   table.setNumberOfRowsAndColumns(numRows, 2, "Header");
   
   table.setTableTitle("header");
   table.setColumnTitle(0, "tag");
   table.setColumnTitle(1, "value");
   
   int rowCount = 0;
   
   for (std::map<QString, QString>::const_iterator iter = header.begin(); iter != header.end(); iter++) {
      const QString tag(iter->first);
      QString value(iter->second);
      if (tag == headerTagComment) {
         value = StringUtilities::setupCommentForStorage(value);
      }
      
      table.setElement(rowCount, 0, tag);
      table.setElement(rowCount, 1, value);
      
      rowCount++;
   }
}

/**
 * read the header's data from a StringTable.
 */
void 
AbstractFile::readHeaderDataFromStringTable(const StringTable& table) throw (FileException)
{
   if (table.getTableTitle() != "header") {
      throw FileException("Table title is not named \"header\" when reading the header "
                          "from a string table.");
   }
   
   if (table.getNumberOfColumns() < 2) {
      throw FileException("Table containing header has less than two columns");
   }
  
   //
   // clear the header
   // 
   header.clear();
   
   //
   // Read the header
   //
   for (int i = 0; i < table.getNumberOfRows(); i++) {
      const QString tag(table.getElement(i, 0));
      const QString value(table.getElement(i, 1));
      setHeaderTag(tag, value);
   }
}
      
/**
 * generate a date and time timestamp.
 */
QString 
AbstractFile::generateDateAndTimeStamp()
{
   //
   // In form "03 Jan 2007 13:41:25"
   //
   //const QString s(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss"));
   const QString s(DateAndTime::getDateAndTimeForNaming());
   return s;
}

/**
 * generate a unique timestamp that is all numeric but in a string
 * since too large for an 4 bit integer
 */
QString 
AbstractFile::generateUniqueNumericTimeStampAsString()
{
   static int counter = 0;  // used so timestamp is unique if two calls within 1msec of each other
   
   //
   // Four digit number with leading zeros
   //
   QString fiveDigitNumber;
   if (counter < 10000) {
      fiveDigitNumber += "0";
   } 
   if (counter < 1000) {
      fiveDigitNumber += "0";
   } 
   if (counter < 100) {
      fiveDigitNumber += "0";
   } 
   if (counter < 10) {
      fiveDigitNumber += "0";
   }
   fiveDigitNumber += QString::number(counter);
   
   // where yyyy = 2 digit year
   //       MM = 2 digit month
   //       dd = 2 digit date
   //       hh = 2 digit hour
   //       mm = 2 digit minumte
   //       zzz = 3 digit milliseconds
   //       UUU = 3 digit number from counter
   //

   //
   // TimeStamp string
   //
   const QString s(QDateTime::currentDateTime().toString("yyyyMMddhhmmzzz") + fiveDigitNumber);
   
   //
   // update counter
   //
   counter++;
   if (counter > 99999) {
      counter = 0;
   }

   return s;
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
 * compare a file for unit testing (returns true if "within tolerance").
 */
bool 
AbstractFile::compareFileForUnitTesting(const AbstractFile* /*af*/,
                                        const float /*tolerance*/,
                                        QString& messageOut) const
{
   messageOut = ("ERROR: Comparison for unit testing of "
                 + descriptiveName
                 + " has not been implemented.  The method \"compareFileForUnitTesting\""
                 + " must be implemented for its class.");
   return false;
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


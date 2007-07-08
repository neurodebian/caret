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

#include <QXmlSimpleReader>

#include "DebugControl.h"
#include "FileUtilities.h"
#include "GiftiCommon.h"
#define __GIFTI_DATA_ARRAY_FILE_MAIN__
#include "GiftiDataArrayFile.h"
#undef __GIFTI_DATA_ARRAY_FILE_MAIN__
#include "GiftiDataArrayFileSaxReader.h"

#include "StringUtilities.h"

/**
 * Constructor
 */
GiftiDataArrayFile::GiftiDataArrayFile(const QString& descriptiveName,
                           const QString& defaultDataArrayCategoryIn,
                           const GiftiDataArray::DATA_TYPE defaultDataTypeIn,
                           const QString& defaultExt,
                           const FILE_FORMAT defaultWriteTypeIn,
                           const FILE_IO supportsAsciiFormat,
                           const FILE_IO supportsBinaryFormat,
                           const FILE_IO supportsOtherFormat,
                           const FILE_IO supportsCSVfFormat,
                           const bool dataAreIndicesIntoLabelTableIn)
   : AbstractFile(descriptiveName, 
                  defaultExt,
                  true,  
                  defaultWriteTypeIn, 
                  supportsAsciiFormat, 
                  supportsBinaryFormat,
                  AbstractFile::FILE_IO_READ_AND_WRITE, 
                  AbstractFile::FILE_IO_READ_AND_WRITE, 
                  AbstractFile::FILE_IO_READ_AND_WRITE, 
                  supportsOtherFormat,
                  supportsCSVfFormat)
{
   defaultDataArrayCategory = defaultDataArrayCategoryIn;
   defaultDataType = defaultDataTypeIn;
   dataAreIndicesIntoLabelTable = dataAreIndicesIntoLabelTableIn;
   
   if (giftiXMLFilesEnabled) {
      setFileReadWriteType(FILE_FORMAT_XML, FILE_IO_READ_AND_WRITE);
      setFileReadWriteType(FILE_FORMAT_XML_BASE64, FILE_IO_READ_AND_WRITE);
      setFileReadWriteType(FILE_FORMAT_XML_GZIP_BASE64, FILE_IO_READ_AND_WRITE);
   }
   else {
      setFileReadWriteType(FILE_FORMAT_XML, FILE_IO_READ_ONLY);
      setFileReadWriteType(FILE_FORMAT_XML_BASE64, FILE_IO_READ_ONLY);
      setFileReadWriteType(FILE_FORMAT_XML_GZIP_BASE64, FILE_IO_READ_ONLY);
   }
}

/**
 * Constructor for generic gifti data array file
 */
GiftiDataArrayFile::GiftiDataArrayFile()
   : AbstractFile("GiftiDataArrayFile",   // descriptive name
                  ".gii",                 // default extension
                  true,                   // has a header (metadata)
                  FILE_FORMAT_XML,        // default writing format is XML
                  FILE_IO_NONE,           // does not support caret ascii format
                  FILE_IO_NONE,           // does not support caret binary format
                  FILE_IO_READ_AND_WRITE, // reads and writes XML ascii data
#ifdef HAVE_VTK
                  FILE_IO_READ_AND_WRITE, // reads and writes XML base64 binary data
                  FILE_IO_READ_AND_WRITE, // reads and writes XML compressed base64 binary data
#else  // HAVE_VTK
                  FILE_IO_NONE,           // DOES NOT read and write XML base64 binary data
                  FILE_IO_NONE,           // DOES NOT read and write XML compressed base64 binary data
#endif // HAVE_VTK
                  FILE_IO_NONE,            // does not support "other" format
                  FILE_IO_NONE)            // does not support "csvf" format
{
   defaultDataArrayCategory = "Generic";
   defaultDataType = GiftiDataArray::DATA_TYPE_FLOAT32;
   dataAreIndicesIntoLabelTable = false;
   
   setFileReadWriteType(FILE_FORMAT_XML, FILE_IO_READ_AND_WRITE);
#ifdef HAVE_VTK
   setFileReadWriteType(FILE_FORMAT_XML_BASE64, FILE_IO_READ_AND_WRITE);
   setFileReadWriteType(FILE_FORMAT_XML_GZIP_BASE64, FILE_IO_READ_AND_WRITE);
#else  // HAVE_VTK
   setFileReadWriteType(FILE_FORMAT_XML_BASE64, FILE_IO_NONE);
   setFileReadWriteType(FILE_FORMAT_XML_GZIP_BASE64, FILE_IO_NONE);
#endif // HAVE_VTK
}

/**
 * copy constructor.
 */
GiftiDataArrayFile::GiftiDataArrayFile(const GiftiDataArrayFile& nndf)
   : AbstractFile(nndf)
{
   copyHelperGiftiDataArrayFile(nndf);
}
      
/**
 * assignment operator.
 */
GiftiDataArrayFile& 
GiftiDataArrayFile::operator=(const GiftiDataArrayFile& nndf)
{
   if (this != &nndf) {
      AbstractFile::operator=(nndf);
      copyHelperGiftiDataArrayFile(nndf);
   }
   
   return *this;
}
      
/**
 * copy helper.
 */
void 
GiftiDataArrayFile::copyHelperGiftiDataArrayFile(const GiftiDataArrayFile& nndf)
{
   labelTable = nndf.labelTable;
   metaData = nndf.metaData;
   defaultDataType = nndf.defaultDataType;
   defaultDataArrayCategory = nndf.defaultDataArrayCategory;
   dataAreIndicesIntoLabelTable = nndf.dataAreIndicesIntoLabelTable;
   
   for (unsigned int i = 0; i < nndf.dataArrays.size(); i++) {
      addDataArray(new GiftiDataArray(*nndf.dataArrays[i]));
   }
}
      
/**
 * Destructor
 */
GiftiDataArrayFile::~GiftiDataArrayFile()
{
   clear();
}

/**
 * compare a file for unit testing (returns true if "within tolerance").
 */
bool 
GiftiDataArrayFile::compareFileForUnitTesting(const AbstractFile* af,
                                              const float tolerance,
                                              QString& messageOut) const
{
   messageOut = "";
   
   const GiftiDataArrayFile* gdaf = dynamic_cast<const GiftiDataArrayFile*>(af);
   if (gdaf == NULL) {
      messageOut += "ERROR: File for comparison is not a GiftiDataArrayFile or subtype.\n";
      return false;
   }
   
   const int numLabels = labelTable.getNumberOfLabels();
   if (numLabels != gdaf->labelTable.getNumberOfLabels()) {
      messageOut += "ERROR: The files contain a different number of labels.\n";
   }
   else {
      int labelCount = 0;
      for (int k = 0; k < numLabels; k++) {
         if (labelTable.getLabel(k) != gdaf->getLabelTable()->getLabel(k)) {
            labelCount++;
         }
      }
      
      if (labelCount > 0) {
         messageOut += "ERROR: The files have "
                    + QString::number(labelCount)
                    + " different labels.\n";
      }
   }
   
   const int numArrays = getNumberOfDataArrays();
   if (numArrays != gdaf->getNumberOfDataArrays()) {
      messageOut += "ERROR: The files contain a different number of data arrays (data columns)";
   }
   else {
      for (int i = 0; i < numArrays; i++) {
         const GiftiDataArray* gdf1 = getDataArray(i);
         const GiftiDataArray* gdf2 = gdaf->getDataArray(i);
         
         const std::vector<int> dim1 = gdf1->getDimensions();
         const std::vector<int> dim2 = gdf2->getDimensions();
         if (dim1 != dim2) {
            messageOut += "ERROR: Data Array " 
                          + QString::number(i)
                          + " have a different number of dimensions.\n";
         }
         else {
            if (gdf1->getDataType() != gdf2->getDataType()) {
               messageOut += "ERROR: Data Array "
                          + QString::number(i)
                          + " are different data types.\n";
            }
            else if (gdf1->getTotalNumberOfElements() != gdf2->getTotalNumberOfElements()) {
               messageOut += "ERROR: Data Array "
                          + QString::number(i)
                          + " have a different number of total elements.\n";
            }
            else {
               const int numElem = gdf1->getTotalNumberOfElements();
               int diffCount = 0;
               
               switch (gdf1->getDataType()) {
                  case GiftiDataArray::DATA_TYPE_FLOAT32:
                     {
                        const float* p1 = gdf1->getDataPointerFloat();
                        const float* p2 = gdf2->getDataPointerFloat();
                        for (int m = 0; m < numElem; m++) {
                           float diff = p1[m] - p2[m];
                           if (diff < 0.0) diff = -diff;
                           if (diff > tolerance) {
                              diffCount++;
                           }
                        }
                     }
                     break;
                  case GiftiDataArray::DATA_TYPE_INT32:
                     {
                        const int tol = static_cast<int32_t>(tolerance);
                        const int32_t* p1 = gdf1->getDataPointerInt();
                        const int32_t* p2 = gdf2->getDataPointerInt();
                        for (int m = 0; m < numElem; m++) {
                           float diff = p1[m] - p2[m];
                           if (diff < 0.0) diff = -diff;
                           if (diff > tol) {
                              diffCount++;
                           }
                        }
                     }
                     break;
                  case GiftiDataArray::DATA_TYPE_UINT8:
                     {
                        const uint8_t tol = static_cast<uint8_t>(tolerance);
                        const uint8_t* p1 = gdf1->getDataPointerUByte();
                        const uint8_t* p2 = gdf2->getDataPointerUByte();
                        for (int m = 0; m < numElem; m++) {
                           float diff = p1[m] - p2[m];
                           if (diff < 0.0) diff = -diff;
                           if (diff > tol) {
                              diffCount++;
                           }
                        }
                     }
                     break;
               }
               
               if (diffCount > 0) {
                  messageOut += "ERROR: There are " 
                                + QString::number(diffCount)
                                + " elements with a difference that are greater than "
                                + QString::number(tolerance, 'f', 3)
                                + " in data array "
                                + QString::number(i)
                                + ".\n";
               }
            }
         }
      }
   }
   
   return (messageOut.isEmpty());
}                                     

/**
 * Set the name of a data array.
 */
void
GiftiDataArrayFile::setDataArrayName(const int arrayIndex, const QString& name)
{
   dataArrays[arrayIndex]->getMetaData()->set(GiftiMetaData::getMetaDataName(), name);
   setModified();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
GiftiDataArrayFile::empty() const
{
   return dataArrays.empty();
}
      
/**
 * get the data array with the specified name.
 */
GiftiDataArray* 
GiftiDataArrayFile::getDataArrayWithName(const QString& n) 
{
   for (int i = 0; i < getNumberOfDataArrays(); i++) {
      if (getDataArrayName(i) == n) {
         return getDataArray(i);
      }
   }
   return NULL;
}

/**
 * get the data array with the specified name.
 */
const GiftiDataArray* 
GiftiDataArrayFile::getDataArrayWithName(const QString& n) const 
{
   for (int i = 0; i < getNumberOfDataArrays(); i++) {
      if (getDataArrayName(i) == n) {
         return getDataArray(i);
      }
   }
   return NULL;
}

/**
 * Get the data array index for an array with the specified name.  If the
 * name is not found a negative number is returned.
 */
int
GiftiDataArrayFile::getDataArrayWithNameIndex(const QString& n) const
{
   for (int i = 0; i < getNumberOfDataArrays(); i++) {
      if (getDataArrayName(i) == n) {
         return i;
      }
   }
   return -1;
}

/**
 * get the data array of the specified category.
 */
GiftiDataArray* 
GiftiDataArrayFile::getDataArrayWithCategory(const QString& catName) 
{
   for (int i = 0; i < getNumberOfDataArrays(); i++) {
      GiftiDataArray* gda = getDataArray(i);
      if (gda->getCategory() == catName) {
         return gda;
      }
   }
   return NULL;
}

/**
 * get the data array of the specified category (const method).
 */
const GiftiDataArray* 
GiftiDataArrayFile::getDataArrayWithCategory(const QString& catName) const
{
   for (int i = 0; i < getNumberOfDataArrays(); i++) {
      const GiftiDataArray* gda = getDataArray(i);
      if (gda->getCategory() == catName) {
         return gda;
      }
   }
   return NULL;
}

/**
 * get the index of the data array of the specified category.
 */
int 
GiftiDataArrayFile::getDataArrayWithCategoryIndex(const QString& catName) const
{
   for (int i = 0; i < getNumberOfDataArrays(); i++) {
      const GiftiDataArray* gda = getDataArray(i);
      if (gda->getCategory() == catName) {
         return i;
      }
   }
   return -1;
}

/**
 * Get the name for a data array.
 */
QString
GiftiDataArrayFile::getDataArrayName(const int arrayIndex) const
{
   QString s;
   (void)dataArrays[arrayIndex]->getMetaData()->get(GiftiMetaData::getMetaDataName(), s);
   return s;
}

/**
 * Set the comment for a data array.
 */
void
GiftiDataArrayFile::setDataArrayComment(const int arrayIndex, const QString& comm)
{
   dataArrays[arrayIndex]->getMetaData()->set(GiftiMetaData::getMetaDataDescription(), comm);
   setModified();
}

/**
 * Append to the comment for a data array.
 */
void
GiftiDataArrayFile::appendToDataArrayComment(const int arrayIndex, const QString& comm)
{
   if (comm.isEmpty() == false) {
      QString s(getDataArrayComment(arrayIndex));
      s.append(comm);
      setDataArrayComment(arrayIndex, s);
      setModified();
   }
}

/**
 * Prepend to the comment for a data array.
 */
void
GiftiDataArrayFile::prependToDataArrayComment(const int arrayIndex, const QString& comm)
{
   if (comm.isEmpty() == false) {
      QString s(comm);
      s.append(getDataArrayComment(arrayIndex));
      setDataArrayComment(arrayIndex, s);
      setModified();
   }
}

/**
 * Get the comment for a data array.
 */
QString
GiftiDataArrayFile::getDataArrayComment(const int arrayIndex) const
{
   QString s;
   (void)dataArrays[arrayIndex]->getMetaData()->get(GiftiMetaData::getMetaDataDescription(), s);
   return s;
}

/**
 *
 */
void
GiftiDataArrayFile::clear()
{
   clearAbstractFile();
   
   for (unsigned int i = 0; i < dataArrays.size(); i++) {
      if (dataArrays[i] != NULL) {
         delete dataArrays[i];
         dataArrays[i] = NULL;
      }
   }
   dataArrays.clear();
   
   labelTable.clear();
   metaData.clear();

   // do not clear
   // giftiElementName
   // requiredArrayTypeDataTypes
}

/**
 * get all of the data array names.
 */
void 
GiftiDataArrayFile::getAllArrayNames(std::vector<QString>& names) const
{
   names.clear();
   
   for (int i = 0; i < getNumberOfDataArrays(); i++) {
      names.push_back(getDataArrayName(i));
   }
}

/**
 * check for data arrays with the same name (returns true if there are any).
 */
bool 
GiftiDataArrayFile::checkForDataArraysWithSameName(std::vector<QString>& multipleArrayNames) const
{
   multipleArrayNames.clear();
   
   const int numArrays = getNumberOfDataArrays();
   
   if (numArrays > 0) {
      std::set<QString> badNames;
      for (int i = 0; i < (numArrays - 1); i++) {
         for (int j = i + 1; j < numArrays; j++) {
            if (getDataArrayName(i) == getDataArrayName(j)) {
               badNames.insert(getDataArrayName(i));
            }
         }
      }
      
      if (badNames.empty() == false) {
         multipleArrayNames.insert(multipleArrayNames.begin(),
                                    badNames.begin(), badNames.end());
      }
   }
   
   return (multipleArrayNames.size() > 0);
}
      
/**
 * add a data array.
 */
void 
GiftiDataArrayFile::addDataArray(GiftiDataArray* nda)
{
   nda->setMyParentGiftiDataArrayFile(this);
   dataArrays.push_back(nda);
   
   setModified();
}

/**
 * append a gifti array data  file to this one.
 */
void 
GiftiDataArrayFile::append(const GiftiDataArrayFile& naf) throw (FileException)
{
   const int numArrays = naf.getNumberOfDataArrays();
   if (numArrays <= 0) {
      return;
   }
   
   //
   // Replace filename if "this" file is isEmpty
   //
   if (getNumberOfDataArrays() == 0) {
      setFileName(naf.getFileName());
   }
   
   //
   // Handle cases where arrays are indices into the label table
   //
   std::vector<int> labelIndicesRemapTable;
   if ((dataAreIndicesIntoLabelTable) ||
       (naf.dataAreIndicesIntoLabelTable)) {
      std::vector<bool> arrayWillBeAppended(numArrays, true);
      appendLabelDataHelper(naf, arrayWillBeAppended, labelIndicesRemapTable);
   }

   //
   // Append the data arrays
   //
   for (int i = 0; i < naf.getNumberOfDataArrays(); i++) {
      GiftiDataArray* ndc = new GiftiDataArray(*(naf.dataArrays[i]));
      ndc->remapIntValues(labelIndicesRemapTable);
      ndc->setMyParentGiftiDataArrayFile(this);
      dataArrays.push_back(ndc);
   }
   
   setModified();
}

/**
 * append array data file to this one but selectively load/overwrite arrays
 * indexDestination is where naf's data arrays should be (-1=new, -2=do not load).
 * "indexDestination" will be updated with the columns actually used.
 */
void 
GiftiDataArrayFile::append(const GiftiDataArrayFile& naf, 
                     std::vector<int>& indexDestination,
                     const FILE_COMMENT_MODE fcm) throw (FileException)
{
   const int numArrays = naf.getNumberOfDataArrays();
   if (numArrays <= 0) {
      return;
   }
   
   //
   // Handle cases where arrays are indices into the label table
   //
   std::vector<int> labelIndicesRemapTable;
   if ((dataAreIndicesIntoLabelTable) ||
       (naf.dataAreIndicesIntoLabelTable)) {
      bool haveOne = false;
      std::vector<bool> arrayWillBeAppended(numArrays, false);
      for (int i = 0; i < numArrays; i++) {
         if (indexDestination[i] >= -1) {
            arrayWillBeAppended[i] = true;
            haveOne = true;
         }
      }
      if (haveOne) {
         appendLabelDataHelper(naf, arrayWillBeAppended, labelIndicesRemapTable);
      }
   }
   
   //
   // Replace file name if this file is isEmpty
   //
   if (getNumberOfDataArrays() == 0) {
      setFileName(naf.getFileName());
   }
   
   //
   // append the data arrays
   //
   for (int i = 0; i < numArrays; i++) {
      const int arrayIndex = indexDestination[i];
      
      //
      // Replacing existing array ?
      //
      if (arrayIndex >= 0) {
         delete dataArrays[arrayIndex];
         dataArrays[arrayIndex] = new GiftiDataArray(*(naf.dataArrays[i]));
         dataArrays[arrayIndex]->remapIntValues(labelIndicesRemapTable);
         dataArrays[arrayIndex]->setMyParentGiftiDataArrayFile(this);
      }
      //
      // create new array
      //
      else if (arrayIndex == -1) {
         GiftiDataArray* ndc = new GiftiDataArray(*(naf.dataArrays[i]));
         ndc->remapIntValues(labelIndicesRemapTable);
         ndc->setMyParentGiftiDataArrayFile(this);
         dataArrays.push_back(ndc);
         
         //
         // Lets others know where the array was placed
         //
         indexDestination[i] = getNumberOfDataArrays() - 1;
      }
      //
      // Ignore array
      //
      else {
         // nothing
      }
   }
   
   appendFileComment(naf, fcm);
   
   setModified();
}

/**
 * append helper for files where data are label indices.
 * The table "oldIndicesToNewIndicesTable" maps label indices from 
 * "naf" label indices to the proper indices for "this" file.
 */
void 
GiftiDataArrayFile::appendLabelDataHelper(const GiftiDataArrayFile& naf,
                                          const std::vector<bool>& arrayWillBeAppended,
                                          std::vector<int>& oldIndicesToNewIndicesTable)
{
   oldIndicesToNewIndicesTable.clear();
   
   if ((dataAreIndicesIntoLabelTable == false) ||
       (naf.dataAreIndicesIntoLabelTable == false)) {
      return;
   }
   
   const int numArrays = naf.getNumberOfDataArrays();
   if (numArrays != static_cast<int>(arrayWillBeAppended.size())) {
      return;
   }
   
   const GiftiLabelTable* nltNew = naf.getLabelTable();
   const int numLabelsNew = nltNew->getNumberOfLabels();
   if (numLabelsNew <= 0) {
      return;
   }
   oldIndicesToNewIndicesTable.resize(numLabelsNew, -1);
   
   //
   // Determine which labels will be appended
   //
   for (int i = 0; i < numArrays; i++) {
      GiftiDataArray* nda = naf.dataArrays[i];
      if (nda->getDataType() == GiftiDataArray::DATA_TYPE_INT32) {
         const int numElem = nda->getTotalNumberOfElements();
         if (numElem >= 0) {
            int32_t* dataPtr = nda->getDataPointerInt();
            for (int i = 0; i < numElem; i++) {
               const int indx = dataPtr[i];
               if ((indx >= 0) && (indx < numLabelsNew)) {
                  oldIndicesToNewIndicesTable[indx] = -2;
               }
               else {
                  std::cout << "WARNING Invalid label index set to zero: " << indx << std::endl;
                  dataPtr[i] = 0;
               }
            }
         }
      }
   }
   
   //
   // remap the label indices
   //
   GiftiLabelTable* myLabelTable = getLabelTable();
   for (int i = 0; i < numLabelsNew; i++) {
      if (oldIndicesToNewIndicesTable[i] == -2) {
         oldIndicesToNewIndicesTable[i] = myLabelTable->addLabel(nltNew->getLabel(i));
      }
   }
}

/**
 * add rows to this file.
 */
void 
GiftiDataArrayFile::addRows(const int numberOfRowsToAdd)
{
   for (int i = 0; i < getNumberOfDataArrays(); i++) {
      dataArrays[i]->addRows(numberOfRowsToAdd);
   }
   setModified();
}

/**
 * reset a data array.
 */
void 
GiftiDataArrayFile::resetDataArray(const int arrayIndex)
{
   dataArrays[arrayIndex]->zeroize();
}

/**
 * remove a data array.
 */
void 
GiftiDataArrayFile::removeDataArray(const int arrayIndex)
{
   int numArrays = getNumberOfDataArrays();
   if ((arrayIndex >= 0) && (arrayIndex < numArrays)) {
      delete dataArrays[arrayIndex];
      for (int i = arrayIndex; i < (numArrays - 1); i++) {
         dataArrays[i] = dataArrays[i + 1];
      }
      dataArrays.resize(numArrays - 1);
   }
}      
      
/**
 * Read the contents of the file (header has already been read).
 */
void 
GiftiDataArrayFile::readFileData(QFile& file,
                          QTextStream& stream,
                          QDataStream& binStream,
                          QDomElement& /*rootElement*/) throw (FileException)
{
   switch (getFileReadType()) {      
      case FILE_FORMAT_ASCII:
         readLegacyFileData(file, stream, binStream);
         break;
      case FILE_FORMAT_BINARY:
         readLegacyFileData(file, stream, binStream);
         break;
      case FILE_FORMAT_XML:
         readFileDataXML(file);
         break;
      case FILE_FORMAT_XML_BASE64:
         readFileDataXML(file);
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         readFileDataXML(file);
         break;
      case FILE_FORMAT_OTHER:
         readLegacyFileData(file, stream, binStream);
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         readLegacyFileData(file, stream, binStream);
         break;
   }
}

/**
 * Write the file's data (header has already been written).
 */
void 
GiftiDataArrayFile::writeFileData(QTextStream& stream,
                           QDataStream& binStream,
                           QDomDocument& /* xmlDoc */,
                           QDomElement& /* rootElement */) throw (FileException)
{
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         writeLegacyFileData(stream, binStream);
         break;
      case FILE_FORMAT_BINARY:
         writeLegacyFileData(stream, binStream);
         break;
      case FILE_FORMAT_XML:
         writeFileDataXML(stream);
         break;
      case FILE_FORMAT_XML_BASE64:
         writeFileDataXML(stream);
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         writeFileDataXML(stream);
         break;
      case FILE_FORMAT_OTHER:
         writeLegacyFileData(stream, binStream);
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         writeLegacyFileData(stream, binStream);
         break;
   }
}

/**
 * read the XML file.
 */
void
GiftiDataArrayFile::readFileDataXML(QFile& file) throw (FileException)
{
   QXmlSimpleReader reader;
   GiftiDataArrayFileSaxReader saxReader(this);
   reader.setContentHandler(&saxReader);
   reader.setErrorHandler(&saxReader);
 
   //
   // Some constant to determine how to read a file based upon the file's size
   //
   const int oneMegaByte = 1048576;
   const qint64 bigFileSize = 25 * oneMegaByte;
   
   if (file.size() < bigFileSize) {
      //
      // This call reads the entire file at once but this is a problem
      // since the XML files can be very large and will cause the 
      // QT XML parsing to crash
      //
      if (reader.parse(&file) == false) {
         throw FileException(filename, saxReader.getErrorMessage());
      }
   }
   else {
      //
      // The following code reads the XML file in pieces
      // and hopefully will prevent QT from crashing when
      // reading large files
      //
      
      //
      // Create a data stream
      //   
      QDataStream stream(&file);
      
      //
      // buffer for data read
      //
      const int bufferSize = oneMegaByte;
      char buffer[bufferSize];
      
      //
      // the XML input source
      //
      QXmlInputSource xmlInput;

      int totalRead = 0;
      
      bool firstTime = true;
      while (stream.atEnd() == false) {
         int numRead = stream.readRawData(buffer, bufferSize);
         totalRead += numRead;
         if (DebugControl::getDebugOn()) {
            std::cout << "GIFTI large file read, total: " << numRead << ", " << totalRead << std::endl;
         }
         
         //
         // Place the input data into the XML input
         //
         xmlInput.setData(QByteArray(buffer, numRead));
         
         //
         // Process the data that was just read
         //
         if (firstTime) {
            if (reader.parse(&xmlInput, true) == false) {
               throw FileException(filename, saxReader.getErrorMessage());            
            }
         }
         else {
            if (reader.parseContinue() == false) {
               throw FileException(filename, saxReader.getErrorMessage());
            }
         }
         
         firstTime = false;
      }
      
      //
      // Tells parser that there is no more data
      //
      xmlInput.setData(QByteArray());
      if (reader.parseContinue() == false) {
         throw FileException(filename, saxReader.getErrorMessage());
      }
   }
    
   //
   // Transfer MetaData
   //
   const GiftiMetaData::MetaDataContainer* data = metaData.getMetaData();
   for (GiftiMetaData::ConstMetaDataIterator iter = data->begin(); iter != data->end(); iter++) {
      setHeaderTag(iter->first, iter->second);
   }   
}

/**
 * write the XML file.
 */
void 
GiftiDataArrayFile::writeFileDataXML(QTextStream& stream) throw (FileException)
{
   //
   // Get how the array data should be encoded
   //
   GiftiDataArray::ENCODING encoding = GiftiDataArray::ENCODING_ASCII;
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         break;
      case FILE_FORMAT_BINARY:
         break;
      case FILE_FORMAT_XML:
         encoding = GiftiDataArray::ENCODING_ASCII;
         break;
      case FILE_FORMAT_XML_BASE64:
         encoding = GiftiDataArray::ENCODING_BASE64_BINARY;
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         encoding = GiftiDataArray::ENCODING_COMPRESSED_BASE64_BINARY;
         break;
      case FILE_FORMAT_OTHER:
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         break;
   }
   
   const int fileVersion = GiftiDataArrayFile::getCurrentFileVersion();
   
   stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";

   stream << "<!DOCTYPE GIFTI SYSTEM \"http://brainmap.wustl.edu/gifti/gifti.dtd\">" << "\n";
   
   stream << "<" << GiftiCommon::tagGIFTI << " " 
          << GiftiCommon::attVersion << "=\"" << fileVersion << "\">" << "\n";
   int indent = 0;

#ifdef CARET_FLAG
   //
   // copy the Abstract File header into this file's metadata 
   //
   metaData.clear();
   AbstractFileHeaderContainer::iterator iter;
   for (iter = header.begin(); iter != header.end(); iter++) {
      //
      // Get the tag and its value
      //
      const QString tag(iter->first);
      const QString value(iter->second);
      metaData.set(tag,value);
   }
#endif // CARET_FLAG
   
   indent++;
   metaData.writeAsXML(stream, indent);
   indent--;
   
   //
   // Write the labels
   //
   indent++;
   labelTable.writeAsXML(stream, indent);
   indent--;   
   
   indent++;
   for (unsigned int i = 0; i < dataArrays.size(); i++) {
#ifdef CARET_FLAG
      dataArrays[i]->setEncoding(encoding);
#endif // CARET_FLAG
      dataArrays[i]->writeAsXML(stream, indent);
   }
   indent--;
   
   stream << "</" << GiftiCommon::tagGIFTI << ">" << "\n";
}      

/**
 * read legacy file format data.
 */
void 
GiftiDataArrayFile::readLegacyFileData(QFile& /*file*/, 
                                       QTextStream& /* stream */, 
                                       QDataStream& /* binStream */) throw (FileException)
{
   throw FileException(filename, "GiftiDataArrayFile does not support reading legacy files.");
}

/**
 * write legacy file format data.
 */
void 
GiftiDataArrayFile::writeLegacyFileData(QTextStream& /* stream */, 
                                        QDataStream& /* binStream */) throw (FileException)
{
   throw FileException(filename, "GiftiDataArrayFile does not support writing legacy files.");
}
      

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

#include <QUuid>

#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>

#include <QTextStream>

#include "DebugControl.h"
#include "FileUtilities.h"
#include "GiftiCommon.h"
#include "GiftiDataArray.h"
#include "GiftiDataArrayFile.h"
#include "StringUtilities.h"
#ifdef HAVE_VTK
#include "vtkBase64Utilities.h"
#include "vtkByteSwap.h"
#include "vtkZLibDataCompressor.h"
#endif // HAVE_VTK

/**
 * constructor.
 */
GiftiDataArray::GiftiDataArray(GiftiDataArrayFile* parentGiftiDataArrayFileIn,
                               const QString& intentIn,
                               const DATA_TYPE dataTypeIn,
                               const std::vector<int> dimensionsIn,
                               const ENCODING encodingIn)
{
   parentGiftiDataArrayFile = parentGiftiDataArrayFileIn;
   intentName = intentIn;
   dataTypeSize = 0;
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;
   clear();
   dataType = dataTypeIn;
   setDimensions(dimensionsIn);
   encoding = encodingIn;
   endian = getSystemEndian();
   arraySubscriptingOrder = ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST;
   externalFileName = "";
   externalFileOffset = 0;
   //dataLocation = DATA_LOCATION_INTERNAL;
   
   if (intentName == GiftiCommon::intentCoordinates) {   
      GiftiMatrix gm;
      gm.setDataSpaceName(GiftiCommon::spaceLabelTalairach);
      gm.setTransformedSpaceName(GiftiCommon::spaceLabelTalairach);
      matrices.push_back(gm);
   }
   
   //getDataTypeAppropriateForIntent(intent, dataType);
   metaData.set(GiftiCommon::metaDataNameUniqueID, 
                QUuid::createUuid().toString());
                
}

/**
 * constructor.
 */
GiftiDataArray::GiftiDataArray(GiftiDataArrayFile* parentGiftiDataArrayFileIn,
                               const QString& intentIn)
{
   parentGiftiDataArrayFile = parentGiftiDataArrayFileIn;
   intentName = intentIn;
   dataTypeSize = 0;
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;
   clear();
   dimensions.clear();
   encoding = ENCODING_INTERNAL_ASCII;
   endian = getSystemEndian();
   arraySubscriptingOrder = ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST;
   externalFileName = "";
   externalFileOffset = 0;
   //dataLocation = DATA_LOCATION_INTERNAL;
   
   if (intentName == GiftiCommon::intentCoordinates) {   
      GiftiMatrix gm;
      gm.setDataSpaceName(GiftiCommon::spaceLabelTalairach);
      gm.setTransformedSpaceName(GiftiCommon::spaceLabelTalairach);
      matrices.push_back(gm);
   }
   
   dataType = DATA_TYPE_FLOAT32;
   getDataTypeAppropriateForIntent(intentName, dataType);
}

/**
 * destructor.
 */
GiftiDataArray::~GiftiDataArray()
{
   clear();
}

/**
 * copy constructor.
 */
GiftiDataArray::GiftiDataArray(const GiftiDataArray& nda)
{
   copyHelperGiftiDataArray(nda);
}

/**
 * assignment operator.
 */
GiftiDataArray&
GiftiDataArray::operator=(const GiftiDataArray& nda)
{
   if (this != &nda) {
      copyHelperGiftiDataArray(nda);
   }
   return *this;
}
      
/**
 * the copy helper (used by copy constructor and assignment operator).
 */
void 
GiftiDataArray::copyHelperGiftiDataArray(const GiftiDataArray& nda)
{
   intentName = nda.intentName;
   encoding = nda.encoding;
   arraySubscriptingOrder = nda.arraySubscriptingOrder;
   dataType = nda.dataType;
   //dataLocation = nda.dataLocation;
   dataTypeSize = nda.dataTypeSize;
   endian = nda.endian;
   parentGiftiDataArrayFile = NULL;  // caused modified to be set !! nda.parentGiftiDataArrayFile;
   dimensions = nda.dimensions;
   allocateData();
   data = nda.data;
   metaData = nda.metaData;
   nonWrittenMetaData = nda.nonWrittenMetaData;
   externalFileName = nda.externalFileName;
   externalFileOffset = nda.externalFileOffset;
   minMaxFloatValuesValid = nda.minMaxFloatValuesValid;
   minValueFloat = nda.minValueFloat;
   maxValueFloat = nda.maxValueFloat;
   minMaxFloatValuesValid = nda.minMaxFloatValuesValid;
   minValueInt = nda.minValueInt;
   maxValueInt = nda.maxValueInt;
   minMaxIntValuesValid = nda.minMaxIntValuesValid;
   minMaxPercentageValuesValid = nda.minMaxPercentageValuesValid;
   negMaxPct = nda.negMaxPct;
   negMinPct = nda.negMinPct;
   posMinPct = nda.posMinPct;
   posMaxPct = nda.posMaxPct;
   negMaxPctValue = nda.negMaxPctValue;
   negMinPctValue = nda.negMinPctValue;
   posMinPctValue = nda.posMinPctValue;
   posMaxPctValue = nda.posMaxPctValue;
   matrices = nda.matrices;
   setModified();
}

/**
 * get the data type appropriate for the intent (returns true if intent is valid).
 */
bool 
GiftiDataArray::getDataTypeAppropriateForIntent(const QString& intentIn,
                                                  DATA_TYPE& dataTypeOut)
{
   if (intentIn == GiftiCommon::intentCoordinates) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else if (intentIn == GiftiCommon::intentTimeSeries) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else if (intentIn == GiftiCommon::intentNormals) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else if (intentIn == GiftiCommon::intentLabels) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_INT32;
   }
   else if ((intentIn == GiftiCommon::intentRGBA) ||
            (intentIn == GiftiCommon::intentRGB)) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_UINT8;
   }
   else if (intentIn == GiftiCommon::intentShape) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else if (intentIn == GiftiCommon::intentTensors) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else if (intentIn == GiftiCommon::intentTopologyTriangles) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_INT32;
   }
   else if (intentIn.startsWith(GiftiCommon::intentPrefix)) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else {
      std::cout << "WARNING: unrecogized intent \""
                << intentIn.toAscii().constData()
                << " in GiftiDataArray::getDataTypeAppropriateForIntent()." << std::endl;
      return false;
   }
   return true;
}
      
/**
 * add nodes.
 */
void 
GiftiDataArray::addRows(const int numRowsToAdd)
{
   dimensions[0] += numRowsToAdd;
   allocateData();
}

/**
 * delete rows.
 */
void 
GiftiDataArray::deleteRows(const std::vector<int>& rowsToDeleteIn)
{
   if (rowsToDeleteIn.empty()) {
      return;
   }
   
   //
   // Sort rows in reverse order
   //
   std::vector<int> rowsToDelete = rowsToDeleteIn;
   std::sort(rowsToDelete.begin(), rowsToDelete.end());
   std::unique(rowsToDelete.begin(), rowsToDelete.end());
   std::reverse(rowsToDelete.begin(), rowsToDelete.end());
   
   //
   // size of row in bytes
   //
   int numBytesInRow = 1;
   for (unsigned int i = 1; i < dimensions.size(); i++) {
      numBytesInRow = dimensions[i];
   }
   numBytesInRow *= dataTypeSize;
   
   //
   // Remove the unneeded rows
   //
   for (unsigned int i = 0; i < rowsToDelete.size(); i++) {
      const int offset = rowsToDelete[i] * numBytesInRow;
      data.erase(data.begin() + offset, data.begin() + offset + numBytesInRow);
   }
   
   //
   // Update the dimensions
   //
   dimensions[0] -= rowsToDelete.size();
   setModified();
}      

/**
 * set number of nodes which causes reallocation of data.
 */
void 
GiftiDataArray::setDimensions(const std::vector<int> dimensionsIn)
{
   dimensions = dimensionsIn;
   if (dimensions.size() == 1) {
      dimensions.push_back(1);
   }
   else if (dimensions.empty()) {
      dimensions.push_back(0);
      dimensions.push_back(0);
   }
   allocateData();
}

/**
 * allocate data for this array.
 */
void 
GiftiDataArray::allocateData()
{
   //
   // Determine the number of items to allocate
   //
   int dataSizeInBytes = 1;
   for (unsigned int i = 0; i < dimensions.size(); i++) {
      dataSizeInBytes *= dimensions[i];
   }
   
   //
   // Bytes required by each data type
   //
   dataTypeSize = 0;
   switch (dataType) {
      case DATA_TYPE_FLOAT32:
         dataTypeSize = sizeof(float);
         break;
      case DATA_TYPE_INT32:
         dataTypeSize = sizeof(int32_t);
         break;
      case DATA_TYPE_UINT8:
         dataTypeSize = sizeof(uint8_t);
         break;
   }
   dataSizeInBytes *= dataTypeSize;
   
   //
   // Does data need to be allocated
   //
   if (dataSizeInBytes > 0) {
      //
      //  Allocate the needed memory
      //
      data.resize(dataSizeInBytes);
   }
   else {
      data.clear();
   }
   
   //
   // Update the data pointers
   //
   updateDataPointers();

   setModified();
}

/**
 * update the data pointers.
 */
void 
GiftiDataArray::updateDataPointers()
{
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;
   if (data.empty() == false) {
      switch (dataType) {
         case DATA_TYPE_FLOAT32:
            dataPointerFloat = (float*)&data[0];
            break;
         case DATA_TYPE_INT32:
            dataPointerInt   = (int32_t*)&data[0];
            break;
         case DATA_TYPE_UINT8:
            dataPointerUByte = (uint8_t*)&data[0];
            break;
      }
   }
}
      
/**
 * reset column.
 */
void 
GiftiDataArray::clear()
{
   arraySubscriptingOrder = ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST;
   encoding = ENCODING_INTERNAL_ASCII;
   dataType = DATA_TYPE_FLOAT32;
   endian = getSystemEndian();
   dataTypeSize = sizeof(float);
   metaData.clear();
   nonWrittenMetaData.clear();
   dimensions.clear();
   setDimensions(dimensions);
   externalFileName = "";
   externalFileOffset = 0;
   minMaxFloatValuesValid = false;
   minMaxPercentageValuesValid = false;
   
   // do not clear
   // parentGiftiDataFile;
   // arrayType;
   
   // setDimensions will call allocateData() which will set
   // dataPointer
   // dataPointerFloat
   // dataPointerInt
   // dataPointerUByte;
}

/**
 * get the number of nodes (1st dimension).
 */
int 
GiftiDataArray::getNumberOfRows() const 
{ 
   if (dimensions.empty() == false) {
      return dimensions[0]; 
   }
   return 0;
}

/**
 * get the total number of elements.
 */
int 
GiftiDataArray::getTotalNumberOfElements() const
{
   if (dimensions.empty()) {
      return 0;
   }
   
   int num = 1;
   for (unsigned int i = 0; i < dimensions.size(); i++) {
      num *= dimensions[i];
   }
   return num;
}

/**
 * get number of components per node (2nd dimension).
 */
int 
GiftiDataArray::getNumberOfComponents() const 
{ 
   if (dimensions.size() > 1) {
      return dimensions[1];
   }
   else if (dimensions.size() == 1) {
      return 1;
   }
   return 0; 
}
      
/**
 * get data offset.
 */
int 
GiftiDataArray::getDataOffset(const int nodeNum,
                                  const int componentNum) const
{
   const int off = nodeNum * dimensions[1] + componentNum;
   return off;
}

/**
 * get the data type name.
 */
QString 
GiftiDataArray::getDataTypeName(const DATA_TYPE dataType)
{
   switch (dataType) {
      case DATA_TYPE_FLOAT32:
         return GiftiCommon::typeNameFloat32;
         break;
      case DATA_TYPE_INT32:
         return GiftiCommon::typeNameInt32;
         break;
      case DATA_TYPE_UINT8:
         return GiftiCommon::typeNameUInt8;
         break;
   }
   return "";
}

/**
 * convert a data type name to data type.
 */
GiftiDataArray::DATA_TYPE 
GiftiDataArray::getDataTypeFromName(const QString& name,
                                    bool* validDataTypeOut)
{
   if (validDataTypeOut != NULL) {
      *validDataTypeOut = true;
   }
   if (name == GiftiCommon::typeNameFloat32) {
      return DATA_TYPE_FLOAT32;
   }
   else if (name == GiftiCommon::typeNameInt32) {
      return DATA_TYPE_INT32;
   }
   else if (name == GiftiCommon::typeNameUInt8) {
      return DATA_TYPE_UINT8;
   }
   if (validDataTypeOut != NULL) {
      *validDataTypeOut = false;
   }
   return DATA_TYPE_FLOAT32;
}

/**
 * get the data location type name.
 */
/*
QString 
GiftiDataArray::getDataLocationName(const DATA_LOCATION& location)
{
   switch (location) {
      case DATA_LOCATION_INTERNAL:
         return GiftiCommon::dataLocationInternal;
         break;
      case DATA_LOCATION_EXTERNAL:
         return GiftiCommon::dataLocationExternal;
         break;
   }
   return GiftiCommon::dataLocationInternal;
}
*/

/**
 * convert data location name to type.
 */
/*
GiftiDataArray::DATA_LOCATION 
GiftiDataArray::getDataLocationFromName(const QString& name,
                                        bool* validDataLocationOut)
{
   if (validDataLocationOut != NULL) {
      *validDataLocationOut = true;
   }
   if (name == GiftiCommon::dataLocationInternal) {
      return DATA_LOCATION_INTERNAL;
   }
   else if (name == GiftiCommon::dataLocationExternal) {
      return DATA_LOCATION_EXTERNAL;
   }
   if (validDataLocationOut != NULL) {
      *validDataLocationOut = false;
   }
   return DATA_LOCATION_INTERNAL;
}
*/
      
/**
 * convert endian name to endian.
 */
GiftiDataArray::ENDIAN 
GiftiDataArray::getEndianFromName(const QString& name,
                                  bool* validEndianOut)
{
   if (validEndianOut != NULL) {
      *validEndianOut = true;
   }
   
   if (name == GiftiCommon::endianBig) {
      return ENDIAN_BIG;
   }
   else if (name == GiftiCommon::endianLittle) {
      return ENDIAN_LITTLE;
   }

   if (validEndianOut != NULL) {
      *validEndianOut = false;
   }
   return getSystemEndian();
}
                                
/**
 * get endian name.
 */
QString 
GiftiDataArray::getEndianName(const ENDIAN e)
{
   switch (e) {
      case ENDIAN_BIG:
         return GiftiCommon::endianBig;
         break;
      case ENDIAN_LITTLE:
         return GiftiCommon::endianLittle;
         break;
   }
   
   return "";
}

/**
 * get the system's endian.
 */
GiftiDataArray::ENDIAN 
GiftiDataArray::getSystemEndian()
{
   if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
      return ENDIAN_BIG;
   }
   return ENDIAN_LITTLE;
}
      
/**
 * get encoding type name.
 */
QString 
GiftiDataArray::getEncodingName(const ENCODING encoding)
{
   switch (encoding) {
      case ENCODING_INTERNAL_ASCII:
        return GiftiCommon::encodingNameInternalAscii;
        break;
      case ENCODING_INTERNAL_BASE64_BINARY:
        return GiftiCommon::encodingNameInternalBase64Binary;
        break;
      case ENCODING_INTERNAL_COMPRESSED_BASE64_BINARY:
        return GiftiCommon::encodingNameInternalBase64BinaryGZip;
        break;
      case ENCODING_EXTERNAL_FILE_BINARY:
        return GiftiCommon::encodingNameExternalBinary; 
   }
   return "";
}      

/**
 * convert encoding name to encoding type.
 */
GiftiDataArray::ENCODING 
GiftiDataArray::getEncodingFromName(const QString& name,
                                    bool* validEncodingOut)
{
   if (validEncodingOut != NULL) {
      *validEncodingOut = true;
   }
   if (name == GiftiCommon::encodingNameInternalAscii) {
      return ENCODING_INTERNAL_ASCII;
   }
   else if (name == GiftiCommon::encodingNameInternalBase64Binary) {
      return ENCODING_INTERNAL_BASE64_BINARY;
   }
   else if (name == GiftiCommon::encodingNameInternalBase64BinaryGZip) {
      return ENCODING_INTERNAL_COMPRESSED_BASE64_BINARY;
   }
   else if (name == GiftiCommon::encodingNameExternalBinary) {
      return ENCODING_EXTERNAL_FILE_BINARY;
   }
   
   if (validEncodingOut != NULL) {
      *validEncodingOut = false;
   }
   return ENCODING_INTERNAL_ASCII;
}

/**
 * get array subscripting order name.
 */
QString 
GiftiDataArray::getArraySubscriptingOrderName(const ARRAY_SUBSCRIPTING_ORDER aso)
{
   switch (aso) {
      case ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST:
         return GiftiCommon::arraySubscriptingOrderHighestFirst;
      case ARRAY_SUBSCRIPTING_ORDER_LOWEST_FIRST:
         return GiftiCommon::arraySubscriptingOrderLowestFirst;
   }
   
   return GiftiCommon::arraySubscriptingOrderHighestFirst;
}

/**
 * convert array subscripting name to type.
 */
GiftiDataArray::ARRAY_SUBSCRIPTING_ORDER 
GiftiDataArray::getArraySubscriptingOrderFromName(const QString& name,
                                                  bool* validArraySubscriptingOrderOut)
{
   if (validArraySubscriptingOrderOut != NULL) {
      *validArraySubscriptingOrderOut = true;
   }
   if (name == GiftiCommon::arraySubscriptingOrderHighestFirst) {
      return ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST;
   }
   else if (name == GiftiCommon::arraySubscriptingOrderLowestFirst) {
      return ARRAY_SUBSCRIPTING_ORDER_LOWEST_FIRST;
   }
   if (validArraySubscriptingOrderOut != NULL) {
      *validArraySubscriptingOrderOut = false;
   }
   
   return ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST;
}
      
/**
 * get external file information.
 */
void 
GiftiDataArray::getExternalFileInformation(QString& nameOut,
                                           int& offsetOut) const
{
   nameOut = externalFileName;
   offsetOut = externalFileOffset;
}
                                
/**
 * set external file information.
 */
void 
GiftiDataArray::setExternalFileInformation(const QString& nameIn,
                                           const int offsetIn)
{
   externalFileName = nameIn;
   externalFileOffset = offsetIn;
}
                                      
/**
 * remap integer values that are indices to a table.
 */
void 
GiftiDataArray::remapIntValues(const std::vector<int>& remappingTable)
{
   if (remappingTable.empty()) {
      return;
   }
   if (dataType != DATA_TYPE_INT32) {
      return;
   }
   const int num = getTotalNumberOfElements();
   for (int i = 0; i < num; i++) {
      dataPointerInt[i] = remappingTable[dataPointerInt[i]];
   }
}

/**
 * read a GIFTI data array from text.
 * Data array should already be initialized and allocated.
 */
void 
GiftiDataArray::readFromText(QString& text,
            const QString& dataEndianForReading,
            const ARRAY_SUBSCRIPTING_ORDER arraySubscriptingOrderForReading,
            const DATA_TYPE dataTypeForReading,
            const std::vector<int>& dimensionsForReading,
            const ENCODING encodingForReading,
            const QString& externalFileNameForReading,
            const int externalFileOffsetForReading) throw (FileException)
{
   const DATA_TYPE requiredDataType = dataType;
   dataType = dataTypeForReading;
   encoding = encodingForReading;
   endian   = getEndianFromName(dataEndianForReading);
   setDimensions(dimensionsForReading);
   if (dimensionsForReading.size() == 0) {
      throw FileException("Data array has no dimensions.");
   }
   setExternalFileInformation(externalFileNameForReading,
                              externalFileOffsetForReading);
                              
   //
   // If NOT metadata only
   //
   if (parentGiftiDataArrayFile->getReadMetaDataOnlyFlag() == false) {
      //
      // Total number of elements in Data Array
      //
      const int numElements = getTotalNumberOfElements();
      
      switch (encoding) {
         case ENCODING_INTERNAL_ASCII:
            {
               QTextStream stream(&text, QIODevice::ReadOnly);
               
               switch (dataType) {
                  case DATA_TYPE_FLOAT32:
                     {
                        float* ptr = dataPointerFloat;
                        for (int i = 0; i < numElements; i++) {
                           stream >> *ptr;
                           ptr++;
                        }
                     }
                     break;
                  case DATA_TYPE_INT32:
                     {
                        int32_t* ptr = dataPointerInt;
                        for (int i = 0; i < numElements; i++) {
                           stream >> *ptr;
                           ptr++;
                        }
                     }
                     break;
                  case DATA_TYPE_UINT8:
                     {
                        uint8_t* ptr = dataPointerUByte;
                        char c;
                        for (int i = 0; i < numElements; i++) {
                           stream >> c;
                           *ptr = static_cast<uint8_t>(c);
                           ptr++;
                        }
                     }
                     break;
               }
            }
            break;
         case ENCODING_INTERNAL_BASE64_BINARY:
   #ifdef HAVE_VTK
            {
               //
               // Decode the Base64 data using VTK's algorithm
               //
               const QByteArray ba = text.toAscii();
               const char* textChars = ba.constData();
               // crashes const char* textChars = text.toAscii().constData();
               const unsigned long numDecoded =
                     vtkBase64Utilities::Decode((const unsigned char*)textChars,
                                                data.size(),
                                                &data[0]);
               if (numDecoded != data.size()) {
                  std::ostringstream str;
                  str << "Decoding of Base64 Binary data failed.\n"
                      << "Decoded " << numDecoded << " bytes but should be "
                      << data.size() << " bytes.";
                  throw FileException("", str.str().c_str());
               }
               
               //
               // Is byte swapping needed ?
               //
               if (endian != getSystemEndian()) {
                  byteSwapData(getSystemEndian());
               }
            }
   #else  // HAVE_VTK
            throw FileException("No support for Base64 data since VTK not available at compile time.");
   #endif // HAVE_VTK
            break;
         case ENCODING_INTERNAL_COMPRESSED_BASE64_BINARY:
   #ifdef HAVE_VTK
            {
               //
               // Decode the Base64 data using VTK's algorithm
               //
               unsigned char* dataBuffer = new unsigned char[data.size()];
               const QByteArray ba = text.toAscii();
               const char* textChars = ba.constData();
               // crashes const char* textChars = text.toAscii().constData();
               const unsigned long numDecoded =
                     vtkBase64Utilities::Decode((const unsigned char*)textChars,
                                                data.size(),
                                                dataBuffer);
               if (numDecoded == 0) {
                  throw FileException("", "Decoding of GZip Base64 Binary data failed.");
               }
               
               
               //
               // Uncompress the data using VTK's algorithm
               //
               vtkZLibDataCompressor* compressor = vtkZLibDataCompressor::New();
               const unsigned long uncompressedDataLength = 
                                   compressor->Uncompress(dataBuffer,
                                                          numDecoded,
                                                          &data[0],
                                                          data.size());
               if (uncompressedDataLength != data.size()) {
                  std::ostringstream str;
                  str << "Decompression of Binary data failed.\n"
                      << "Uncompressed " << uncompressedDataLength << " bytes but should be "
                      << data.size() << " bytes.";
                  throw FileException("", str.str().c_str());
               }
               
               //
               // Free memory
               //
               delete[] dataBuffer;
               compressor->Delete();
               
               //
               // Is byte swapping needed ?
               //
               if (endian != getSystemEndian()) {
                  byteSwapData(getSystemEndian());
               }
            }
   #else  // HAVE_VTK
            throw FileException("No support for Base64 data since VTK not available at compile time.");
   #endif // HAVE_VTK
            break;
         case ENCODING_EXTERNAL_FILE_BINARY:
            {
               if (externalFileName.isEmpty()) {
                  throw FileException("External file name is empty.");
               }
               
               QFile file(externalFileName);
               if (file.open(QFile::ReadOnly)) {
                  //
                  // Move to the offset of the data
                  //
                  if (file.seek(externalFileOffset) == false) {
                     throw FileException("Error moving to offset "
                                         + QString::number(externalFileOffset)
                                         + " in file "
                                         + externalFileName);
                  }
                  
                  //
                  // Set the number of bytes that must be read
                  //
                  int numberOfBytes = 0;
                  char* pointerToForReadingData = NULL;
                  switch (dataType) {
                     case DATA_TYPE_FLOAT32:
                        numberOfBytes = numElements * sizeof(float);
                        pointerToForReadingData = (char*)dataPointerFloat;
                        break;
                     case DATA_TYPE_INT32:
                        numberOfBytes = numElements * sizeof(int32_t);
                        pointerToForReadingData = (char*)dataPointerInt;
                        break;
                     case DATA_TYPE_UINT8:
                        numberOfBytes = numElements * sizeof(uint8_t);
                        pointerToForReadingData = (char*)dataPointerUByte;
                        break;
                  }
               
                  //
                  // Read the data
                  //
                  QDataStream stream(&file);
                  stream.setVersion(QDataStream::Qt_4_3);
                  const int numBytesRead = stream.readRawData((char*)pointerToForReadingData,
                                                              numberOfBytes);
                  if (numBytesRead != numberOfBytes) {
                     throw FileException("Tried to read "
                                         + QString::number(numberOfBytes)
                                         + " from "
                                         + externalFileName
                                         + " but only read "
                                         + QString::number(numBytesRead)
                                         + ".");
                  }
                  
                  //
                  // Is byte swapping needed ?
                  //
                  if (endian != getSystemEndian()) {
                     byteSwapData(getSystemEndian());
                  }
               }
               else {
                  throw FileException("Error opening \""
                                      + externalFileName
                                      + "\" \n"
                                      + file.errorString());
               }
            }
            break;
      }
   
      //
      // Check if data type needs to be converted
      //
      if (requiredDataType != dataType) {
         if (intentName != GiftiCommon::intentNodeIndex) {
            convertToDataType(requiredDataType);
         }
      }
      
      //
      // Are array indices in opposite order
      //
      if (arraySubscriptingOrderForReading != arraySubscriptingOrder) {
         convertArrayIndexingOrder();
      }
      
      //
      // Update metadata (GIFTI to Caret)
      //
      updateMetaDataAfterReading();
   } // If NOT metadata only
   
   setModified();
}

/**
 * convert array indexing order of data.
 */
void 
GiftiDataArray::convertArrayIndexingOrder() throw (FileException)
{
   const int numDim = static_cast<int>(dimensions.size());
   if (numDim <= 1) {
      return;
   }
   
   //
   // Copy the data
   //
   std::vector<uint8_t> dataCopy = data;
   
   //
   // Swap data
   //
   if (numDim == 2) {
      switch (dataType) {
         case DATA_TYPE_FLOAT32:
            {
               float* ptr = (float*)&(dataCopy[0]);
               for (int i = 0; i < dimensions[0]; i++) {
                  for (int j = 0; j < dimensions[1]; j++) {
                     const int indx = (i * dimensions[1]) + j;
                     const int ptrIndex = (j * dimensions[1]) + i;
                     dataPointerFloat[indx] = ptr[ptrIndex];
                  }
               }
            }
            break;
         case DATA_TYPE_INT32:
            {
               uint32_t* ptr = (uint32_t*)&(dataCopy[0]);
               for (int i = 0; i < dimensions[0]; i++) {
                  for (int j = 0; j < dimensions[1]; j++) {
                     const int indx = (i * dimensions[1]) + j;
                     const int ptrIndex = (j * dimensions[1]) + i;
                     dataPointerInt[indx] = ptr[ptrIndex];
                  }
               }
            }
            break;
         case DATA_TYPE_UINT8:
            {
               uint8_t* ptr = (uint8_t*)&(dataCopy[0]);
               for (int i = 0; i < dimensions[0]; i++) {
                  for (int j = 0; j < dimensions[1]; j++) {
                     const int indx = (i * dimensions[1]) + j;
                     const int ptrIndex = (j * dimensions[1]) + i;
                     dataPointerUByte[indx] = ptr[ptrIndex];
                  }
               }
            }
            break;
      }
   }
      
   throw FileException("Row/Column Major order conversion unavailable for arrays "
                       "with dimensions greater than two.");
}

/**
 * write the data as XML.
 */
void 
GiftiDataArray::writeAsXML(QTextStream& stream, 
                           const int indentOffset) 
                                                throw (FileException)
{
   //
   // Do not write if data array is isEmpty
   //
   const int numRows = dimensions[0];
   if (numRows <= 0) {
      return;
   }
   
   //
   // Clean up the dimensions by removing any "last" dimensions that
   // are one with the exception of the first dimension
   //   e.g.:   dimension = [73730, 1]  becomes [73730]
   //
   const int dimensionality = static_cast<int>(dimensions.size());
   for (int i = (dimensionality - 1); i >= 1; i--) {
      if (dimensions[i] <= 1) {
         dimensions.resize(i);
      }
   }
   
   //
   // Check name of data type and encoding
   //
   if (intentName.isEmpty()) {
      throw FileException("", "Invalid (isEmpty) XML Intent.");
   }
   if (intentName == "NIFTI_INTENT_UNKNOWN") {
      intentName = "NIFTI_INTENT_NONE";
   }
   const QString dataTypeName(getDataTypeName(dataType));
   if (dataTypeName.isEmpty()) {
      throw FileException("", "Invalid (isEmpty) XML DataArray type.");
   }
   const QString encodingName(getEncodingName(encoding));
   if (encodingName.isEmpty()) {
      throw FileException("", "Invalid (isEmpty) XML encoding type.");
   }
   const QString arraySubscriptingName(getArraySubscriptingOrderName(arraySubscriptingOrder));
   if (arraySubscriptingName.isEmpty()) {
      throw FileException("", "Invalid (isEmpty) XML array subscripting order type.");
   }

   //
   // Update metadata
   //
   updateMetaDataBeforeWriting();
      
   //
   // Indentation to improve file readability
   //
   int indent = indentOffset;
   
   //
   // External file not supported
   //
   const QString externalFileName = "";
   const QString externalFileOffset = "0";
   
   //
   // Write the opening tag
   //
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "<" << GiftiCommon::tagDataArray << " " << GiftiCommon::attIntent << "=\"" 
          << intentName << "\"" << "\n"; 
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "           " << GiftiCommon::attDataType << "=\"" 
          << dataTypeName << "\"" << "\n"; 
   GiftiCommon::writeIndentationXML(stream, indent);
   //stream << "           " << GiftiCommon::attDataLocation << "=\"" 
   //       << GiftiCommon::dataLocationInternal << "\"" << "\n"; 
   //GiftiCommon::writeIndentationXML(stream, indent);
   stream << "           " << GiftiCommon::attArraySubscriptingOrder << "=\"" 
          << arraySubscriptingName << "\"" << "\n"; 
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "           " << GiftiCommon::attDimensionality << "=\"" 
          << dimensions.size() << "\"" << "\n"; 
   for (unsigned int i = 0; i < dimensions.size(); i++) {
      GiftiCommon::writeIndentationXML(stream, indent);
      stream << "           " << GiftiCommon::getAttDim(i) << "=\"" 
             << dimensions[i] << "\"" << "\n"; 
   }
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "           " << GiftiCommon::attEncoding << "=\"" 
          << encodingName << "\"" << "\n"; 
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "           " << GiftiCommon::attEndian << "=\"" 
          << GiftiCommon::getSystemsEndianName() << "\"" << "\n"; 
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "           " << GiftiCommon::attExternalFileName << "=\"" 
          << externalFileName << "\"" << "\n"; 
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "           " << GiftiCommon::attExternalFileOffset << "=\"" 
          << externalFileOffset << "\"" << ">" << "\n"; 
   
   indent++;
   
   //
   // Write the metadata
   //
   metaData.writeAsXML(stream, indent);
   
   //
   // Write the matrices
   //
   for (int i = 0; i < getNumberOfMatrices(); i++) {
      matrices[i].writeAsXML(stream, indent);
   }
   
   //
   // Write the data element opening tag
   //
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "<" << GiftiCommon::tagData << ">";
   indent++;
   
   //
   // Want closing </DATA> immediately after data for gzip & base64
   //
   bool addCloseTagImmediatelyAfterData = false;
   
   //
   // NOTE: for the base64 and ZLIB-Base64 data, it is important that there are
   // no spaces between the <DATA> and </DATA> tags.
   //
   switch (encoding) {
      case ENCODING_INTERNAL_ASCII:
         {
            //
            // Newline after <DATA> tag (only do this for ASCII !!!)
            //
            stream << "\n";
            
            //
            // determine the number of items per row (node)
            //
            int numItemsPerRow = 1;
            for (unsigned int i = 1; i < dimensions.size(); i++) {
               numItemsPerRow *= dimensions[i];
            }
            
            //
            // Write the rows
            //
            int offset = 0;
            for (int i = 0; i < numRows; i++) {
               GiftiCommon::writeIndentationXML(stream, indent);
               switch (dataType) {
                  case DATA_TYPE_FLOAT32:
                     for (int j = 0; j < numItemsPerRow; j++) {
                        stream << dataPointerFloat[offset + j] << " ";
                     }
                     break;
                  case DATA_TYPE_INT32:
                     for (int j = 0; j < numItemsPerRow; j++) {
                        stream << dataPointerInt[offset + j] << " ";
                     }
                     break;
                  case DATA_TYPE_UINT8:
                     for (int j = 0; j < numItemsPerRow; j++) {
                        stream << dataPointerUByte[offset + j] << " ";
                     }
                     break;
               }
               stream << "\n";
               offset += numItemsPerRow;
            }
         }
         break;
      case ENCODING_INTERNAL_BASE64_BINARY:
#ifdef HAVE_VTK
         {
            //
            // Encode the data with VTK's Base64 algorithm
            //
            char* buffer = new char[static_cast<int>(data.size() * 1.5)];
            const unsigned long compressedLength =
               vtkBase64Utilities::Encode(&data[0],
                                          data.size(),
                                          (unsigned char*)buffer);
            buffer[compressedLength] = '\0';
            
            //
            // Write the data
            //
            stream << buffer;
            addCloseTagImmediatelyAfterData = true;
            
            //
            // Free memory
            //
            delete[] buffer;
         }
#else  // HAVE_VTK
         throw FileException("No support for Base64 data since VTK not available at compile time.");
#endif // HAVE_VTK
         break;
      case ENCODING_INTERNAL_COMPRESSED_BASE64_BINARY:
#ifdef HAVE_VTK
         {
            //
            // Compress the data with VTK's ZLIB algorithm
            //
            vtkZLibDataCompressor* compressor = vtkZLibDataCompressor::New();
            unsigned long compressedDataBufferLength = 
                              compressor->GetMaximumCompressionSpace(data.size());
            unsigned char* compressedDataBuffer = new unsigned char[compressedDataBufferLength];
            unsigned long compressedDataLength =
                          compressor->Compress(&data[0],
                                               data.size(),
                                               compressedDataBuffer,
                                               compressedDataBufferLength);
            compressor->Delete();
            
            //
            // Encode the data with VTK's Base64 algorithm
            //
            char* buffer = new char[static_cast<int>(compressedDataLength * 1.5)];
            const unsigned long compressedLength =
               vtkBase64Utilities::Encode(compressedDataBuffer,
                                          compressedDataLength,
                                          (unsigned char*)buffer);
            buffer[compressedLength] = '\0';
            
            if (DebugControl::getDebugOn()) {
               if (compressedLength > 4) {
                  std::cout << "Bytes: " 
                            << (int)buffer[0] << " "
                            << (int)buffer[1] << " "
                            << (int)buffer[2] << " "
                            << (int)buffer[3] << std::endl;
               }
            }
            
            //
            // Write the data
            //
            stream << buffer;
            addCloseTagImmediatelyAfterData = true;
            
            //
            // Free memory
            //
            delete[] buffer;
            delete[] compressedDataBuffer;
         }
#else  // HAVE_VTK
         throw FileException("No support for Base64 data since VTK not available at compile time.");
#endif // HAVE_VTK
         break;
      case ENCODING_EXTERNAL_FILE_BINARY:
         throw FileException("Writing external binary data not supported.");
         break;
   }
   
   //
   // Write the closing tag
   //
   indent--;
   if (addCloseTagImmediatelyAfterData == false) {
      GiftiCommon::writeIndentationXML(stream, indent);
   }
   stream << "</" << GiftiCommon::tagData << ">" << "\n";
   
   //
   // write the closing data array tag
   //
   indent--;
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "</" << GiftiCommon::tagDataArray << ">" << "\n";
}                      

/**
 * convert to data type.
 */
void 
GiftiDataArray::convertToDataType(const DATA_TYPE newDataType)
{
   if (newDataType != dataType) {
      if (DebugControl::getDebugOn()) {
         std::cout << "Converting GIFTI DataArray \"" << intentName.toAscii().constData() << "\""
                   << " from type " << getDataTypeName(dataType).toAscii().constData()
                   << " to type " << getDataTypeName(newDataType).toAscii().constData() << "\n";
      }
      
      //
      // make a copy of myself
      //
      GiftiDataArray copyOfMe(*this);
      
      //
      // Set my new data type and reallocate memory
      //
      const DATA_TYPE oldDataType = dataType;
      dataType = newDataType;
      allocateData();
      
      if (data.empty() == false) {
         //
         //  Get total number of elements
         //
         uint32_t numElements = 1;
         for (unsigned int i = 0; i < dimensions.size(); i++) {
            numElements *= dimensions[i];
         }
         
         //
         // copy the data
         //
         for (uint32_t i = 0; i < numElements; i++) {
            switch (dataType) {
               case DATA_TYPE_FLOAT32:
                  switch (oldDataType) {
                     case DATA_TYPE_FLOAT32:
                        dataPointerFloat[i] = static_cast<float>(copyOfMe.dataPointerFloat[i]);
                        break;
                     case DATA_TYPE_INT32:
                        dataPointerFloat[i] = static_cast<float>(copyOfMe.dataPointerInt[i]);
                        break;
                     case DATA_TYPE_UINT8:
                        dataPointerFloat[i] = static_cast<float>(copyOfMe.dataPointerUByte[i]);
                        break;
                  }
                  break;
               case DATA_TYPE_INT32:
                  switch (oldDataType) {
                     case DATA_TYPE_FLOAT32:
                        dataPointerInt[i] = static_cast<int32_t>(copyOfMe.dataPointerFloat[i]);
                        break;
                     case DATA_TYPE_INT32:
                        dataPointerInt[i] = static_cast<int32_t>(copyOfMe.dataPointerInt[i]);
                        break;
                     case DATA_TYPE_UINT8:
                        dataPointerInt[i] = static_cast<int32_t>(copyOfMe.dataPointerUByte[i]);
                        break;
                  }
                  break;
               case DATA_TYPE_UINT8:
                  switch (oldDataType) {
                     case DATA_TYPE_FLOAT32:
                        dataPointerUByte[i] = static_cast<uint8_t>(copyOfMe.dataPointerFloat[i]);
                        break;
                     case DATA_TYPE_INT32:
                        dataPointerUByte[i] = static_cast<uint8_t>(copyOfMe.dataPointerInt[i]);
                        break;
                     case DATA_TYPE_UINT8:
                        dataPointerUByte[i] = static_cast<uint8_t>(copyOfMe.dataPointerUByte[i]);
                        break;
                  }
                  break;
            }
         }
      }
   }
   
   setModified();
}      

/**
 * byte swap the data (data read is different endian than this system).
 */
void 
GiftiDataArray::byteSwapData(const ENDIAN newEndian)
{
#ifdef HAVE_VTK
   endian = newEndian;
   switch (dataType) {
      case DATA_TYPE_FLOAT32:
         if (DebugControl::getDebugOn()) {
            std::cout << "Byte swapping float data for GiftiDataArray." << std::endl;
         }
         vtkByteSwap::SwapVoidRange(dataPointerFloat, getTotalNumberOfElements(), sizeof(float));
         break;
      case DATA_TYPE_INT32:
         if (DebugControl::getDebugOn()) {
            std::cout << "Byte swapping int data for GiftiDataArray." << std::endl;
         }
         vtkByteSwap::SwapVoidRange(dataPointerInt, getTotalNumberOfElements(), sizeof(int32_t));
         break;
      case DATA_TYPE_UINT8:
         // should not need to swap ??
         break;
   }
#endif // HAVE_VTK
}      

// set the data array modified (actually set's the modified flag for file containing this)
void 
GiftiDataArray::setModified()
{
   if (parentGiftiDataArrayFile != NULL) {
      parentGiftiDataArrayFile->setModified();
   }
}      

/** 
 * get minimum and maximum values (valid for int data only).
 */
void 
GiftiDataArray::getMinMaxValues(int& minValue, int& maxValue) const
{
   if (minMaxIntValuesValid == false) {
      minValueInt = std::numeric_limits<int>::max();
      minValueInt = std::numeric_limits<int>::min();
      
      int numItems = getTotalNumberOfElements();
      for (int i = 0; i < numItems; i++) {
         minValueInt = std::min(minValueInt, dataPointerInt[i]);
         maxValueInt = std::max(maxValueInt, dataPointerInt[i]);
      }
      minMaxIntValuesValid = true;
   }
   minValue = minValueInt;
   maxValue = maxValueInt;
}
      
/** 
 * get minimum and maximum values (valid for float data only).
 */
void 
GiftiDataArray::getMinMaxValues(float& minValue, float& maxValue) const
{
   if (minMaxFloatValuesValid == false) {
      minValueFloat = std::numeric_limits<float>::max();
      maxValueFloat = -std::numeric_limits<float>::max();
      
      int numItems = getTotalNumberOfElements();
      for (int i = 0; i < numItems; i++) {
         minValueFloat = std::min(minValueFloat, dataPointerFloat[i]);
         maxValueFloat = std::max(maxValueFloat, dataPointerFloat[i]);
      }
      minMaxFloatValuesValid = true;
   }
   minValue = minValueFloat;
   maxValue = maxValueFloat;
}

/**
 * Get data column min/max for the specified percentages.
 */
void
GiftiDataArray::getMinMaxValuesFromPercentages(const float negMaxPctIn,
                                               const float negMinPctIn,
                                               const float posMinPctIn,
                                               const float posMaxPctIn,
                                               float& negMaxPctValueOut,
                                               float& negMinPctValueOut,
                                               float& posMinPctValueOut,
                                               float& posMaxPctValueOut)
{
   if ((negMaxPctIn != negMaxPct) ||
       (negMinPctIn != negMinPct) ||
       (posMinPctIn != posMinPct) ||
       (posMaxPctIn != posMaxPct)) {
      minMaxPercentageValuesValid = false;
   }
   if (minMaxPercentageValuesValid == false) {
      negMaxPct = negMaxPctIn;
      negMinPct = negMinPctIn;
      posMinPct = posMinPctIn;
      posMaxPct = posMaxPctIn;

      negMaxPctValue = 0.0;
      negMinPctValue = 0.0;
      posMinPctValue = 0.0;
      posMaxPctValue = 0.0;

      const int num = getTotalNumberOfElements();
      if (num > 0) {
         std::vector<float> negatives, positives;
         negatives.reserve(num);
         positives.reserve(num);
         for (int i = 0; i < num; i++) {
            if (dataPointerFloat[i] > 0.0) {
               positives.push_back(dataPointerFloat[i]);
            }
            else if (dataPointerFloat[i] < 0.0) {
               negatives.push_back(dataPointerFloat[i]);
            }
         }

         int numPos = static_cast<int>(positives.size());
         if (numPos > 0) {
            std::sort(positives.begin(), positives.end());

            if (numPos == 1) {
               posMinPctValue = positives[0];
               posMaxPctValue = positives[0];
            }
            else {
               int minIndex = numPos * (posMinPct / 100.0);
               if (minIndex < 0) minIndex = 0;
               if (minIndex >= numPos) minIndex = numPos - 1;
               posMinPctValue = positives[minIndex];

               int maxIndex = numPos * (posMaxPct / 100.0);
               if (maxIndex < 0) maxIndex = 0;
               if (maxIndex >= numPos) maxIndex = numPos - 1;
               posMaxPctValue = positives[maxIndex];
            }
         }

         int numNeg = static_cast<int>(negatives.size());
         if (numNeg > 0) {
            std::sort(negatives.begin(), negatives.end());

            if (numNeg == 1) {
               negMinPctValue = negatives[0];
               negMaxPctValue = negatives[0];
            }
            else {
               int maxIndex = numNeg * ((100.0 - negMaxPct) / 100.0);
               if (maxIndex < 0) maxIndex = 0;
               if (maxIndex >= numNeg) maxIndex = numNeg - 1;
               negMaxPctValue = negatives[maxIndex];

               int minIndex = numNeg * ((100.0 - negMinPct) / 100.0);
               if (minIndex < 0) minIndex = 0;
               if (minIndex >= numNeg) minIndex = numNeg - 1;
               negMinPctValue = negatives[minIndex];
            }
         }
      }

      minMaxPercentageValuesValid = true;
   }

   negMaxPctValueOut = negMaxPctValue;
   negMinPctValueOut = negMinPctValue;
   posMaxPctValueOut = posMaxPctValue;
   posMinPctValueOut = posMinPctValue;
}

/**
 * set all elements of array to zero.
 */
void 
GiftiDataArray::zeroize()
{
   if (data.empty() == false) {
      std::fill(data.begin(), data.end(), 0);
   }
   metaData.clear();
   nonWrittenMetaData.clear();
}

/**
 * get an offset for indices into data (dimensionality of indices must be same as data).
 */
int 
GiftiDataArray::getDataOffset(const int indices[]) const
{
   const int numDim = static_cast<int>(dimensions.size());
   
   int offset = 0;
   int dimProduct = 1;
   switch (arraySubscriptingOrder) {
      case ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST:
         for (int d = (numDim - 1); d >= 0; d--) {
            offset += indices[d] * dimProduct;
            dimProduct *= dimensions[d];
         }
         break;
      case ARRAY_SUBSCRIPTING_ORDER_LOWEST_FIRST:  // correct???
         for (int d = 0; d <= (numDim - 1); d++) {
            offset += indices[d] * dimProduct;
            dimProduct *= dimensions[d];
         }
         break;
   }
   
   return offset;
}

/**
 * get a float value (data type must be float and dimensionality of indices must be same as data).
 */
float 
GiftiDataArray::getDataFloat32(const int indices[]) const
{
   const int offset = getDataOffset(indices);
   return dataPointerFloat[offset];
}

/**
 * get a float value pointer(data type must be float and dimensionality of indices must be same as data).
 */
const float* 
GiftiDataArray::getDataFloat32Pointer(const int indices[]) const
{
   const int offset = getDataOffset(indices);
   return &dataPointerFloat[offset];
}

/**
 * get an int value (data type must be int and dimensionality of indices must be same as data).
 */
int32_t 
GiftiDataArray::getDataInt32(const int indices[]) const
{
   const int offset = getDataOffset(indices);
   return dataPointerInt[offset];
}

/**
 * get an int value pointer (data type must be int and dimensionality of indices must be same as data).
 */
const int32_t* 
GiftiDataArray::getDataInt32Pointer(const int indices[]) const
{
   const int offset = getDataOffset(indices);
   return &dataPointerInt[offset];
}

/**
 * get a byte value (data type must be unsigned char and dimensionality of indices must be same as data).
 */
uint8_t 
GiftiDataArray::getDataUInt8(const int indices[]) const
{
   const int offset = getDataOffset(indices);
   return dataPointerUByte[offset];
}

/**
 * get a byte value pointer(data type must be unsigned char and dimensionality of indices must be same as data).
 */
const uint8_t*
GiftiDataArray::getDataUInt8Pointer(const int indices[]) const
{
   const int offset = getDataOffset(indices);
   return &dataPointerUByte[offset];
}

/**
 * set a float value (data type must be float and dimensionality of indices must be same as data).
 */
void 
GiftiDataArray::setDataFloat32(const int indices[], const float dataValue) const
{
   const int offset = getDataOffset(indices);
   dataPointerFloat[offset] = dataValue;
}

/**
 * set an int value (data type must be int and dimensionality of indices must be same as data).
 */
void 
GiftiDataArray::setDataInt32(const int indices[], const int32_t dataValue) const
{
   const int offset = getDataOffset(indices);
   dataPointerInt[offset] = dataValue;
}

/**
 * set a byte value (data type must be unsigned char and dimensionality of indices must be same as data).
 */
void 
GiftiDataArray::setDataUInt8(const int indices[], const uint8_t dataValue) const
{
   const int offset = getDataOffset(indices);
   dataPointerUByte[offset] = dataValue;
}      

/**
 * valid intent name.
 */
bool 
GiftiDataArray::intentNameValid(const QString& intentNameIn)
{
   if (intentNameIn.startsWith(GiftiCommon::intentPrefix)) {
      return true;
   }
/*   
   if ((intentNameIn == GiftiCommon::intentCoordinates) ||
       (intentNameIn == GiftiCommon::intentFunctional) ||
       (intentNameIn == GiftiCommon::intentNormals) ||
       (intentNameIn == GiftiCommon::intentLabels) ||
       (intentNameIn == GiftiCommon::intentRGBA) ||
       (intentNameIn == GiftiCommon::intentShape) ||
       (intentNameIn == GiftiCommon::intentTensors) ||
       (intentNameIn == GiftiCommon::intentTopologyTriangles)) {
      return true;
   }
*/   
   return false;
}      

/**
 * update the array's metadata after reading the array.
 */
void 
GiftiDataArray::updateMetaDataAfterReading()
{
   if (intentName == GiftiCommon::intentCoordinates) {
      //
      // Convert GIFTI geometry type to caret surface type
      //
      QString giftiGeomType;
      if (metaData.get(GiftiCommon::metaDataNameGeometricType, giftiGeomType)) {
         QString caretGeomType;
         if (giftiGeomType == GiftiCommon::metaDataValueGeometricTypeReconstruction) {
            caretGeomType = "Raw";
         }
         else if (giftiGeomType == GiftiCommon::metaDataValueGeometricTypeAnatomical) {
            caretGeomType = "Fiducial";
         }
         else if (giftiGeomType == GiftiCommon::metaDataValueGeometricTypeInflated) {
            caretGeomType = "Inflated";
         }
         else if (giftiGeomType == GiftiCommon::metaDataValueGeometricTypeVeryInflated) {
            caretGeomType = "Very_Inflated";
         }
         else if (giftiGeomType == GiftiCommon::metaDataValueGeometricTypeSpherical) {
            caretGeomType = "Spherical";
         }
         else if (giftiGeomType == GiftiCommon::metaDataValueGeometricTypeSemiSpherical) {
            caretGeomType = "CompMedWall";
         }
         else if (giftiGeomType == GiftiCommon::metaDataValueGeometricTypeEllipsoid) {
            caretGeomType = "Ellipsoidal";
         }
         else if (giftiGeomType == GiftiCommon::metaDataValueGeometricTypeFlat) {
            caretGeomType = "Flat";
         }
         else if (giftiGeomType == GiftiCommon::metaDataValueGeometricTypeFlatLobar) {
            caretGeomType = "FlatLobar";
         }
         else if (giftiGeomType == GiftiCommon::metaDataValueGeometricTypeHull) {
            caretGeomType = "Hull";
         }
         if (caretGeomType.isEmpty() == false) {
            metaData.set(AbstractFile::headerTagConfigurationID,
                         caretGeomType);
            metaData.remove(GiftiCommon::metaDataNameGeometricType);
         }
      }
      
      //
      // Convert GIFTI anatomical structure to Caret structure
      //
      QString giftiAnatomicalStructurePrimary;
      if (metaData.get(GiftiCommon::metaDataNameAnatomicalStructurePrimary,
                       giftiAnatomicalStructurePrimary)) {
         QString caretStructure;
         if (giftiAnatomicalStructurePrimary == GiftiCommon::metaDataValueAnatomicalStructurePrimaryCortexLeft) {
            caretStructure = "left";
         }
         else if (giftiAnatomicalStructurePrimary == GiftiCommon::metaDataValueAnatomicalStructurePrimaryCortexRight) {
            caretStructure = "right";
         }
         else if (giftiAnatomicalStructurePrimary == GiftiCommon::metaDataValueAnatomicalStructurePrimaryCortexRightAndLeft) {
            caretStructure = "both";
         }
         else if (giftiAnatomicalStructurePrimary == GiftiCommon::metaDataValueAnatomicalStructurePrimaryCerebellum) {
            caretStructure = "cerebellum";
         }
         else if (giftiAnatomicalStructurePrimary == GiftiCommon::metaDataValueAnatomicalStructurePrimaryHead) {
            caretStructure = "head";
         }
         
         if (caretStructure.isEmpty() == false) {
            metaData.set(AbstractFile::headerTagStructure,
                         caretStructure);
            metaData.remove(GiftiCommon::metaDataNameAnatomicalStructurePrimary);
         }
      }
   }
   else if (intentName == GiftiCommon::intentTopologyTriangles) {
      //
      // Convert GIFTI topology type to Caret Topology Type
      //
      QString giftiTopoType;
      if (metaData.get(GiftiCommon::metaDataNameTopologicalType, giftiTopoType)) {
         QString caretTopoType;
         if (giftiTopoType == GiftiCommon::metaDataValueTopologicalTypeClosed) {
            caretTopoType = "CLOSED";
         }
         else if (giftiTopoType == GiftiCommon::metaDataValueTopologicalTypeOpen) {
            caretTopoType = "OPEN";
         }
         else if (giftiTopoType == GiftiCommon::metaDataValueTopologicalTypeCut) {
            caretTopoType = "CUT";
         }
         else if (giftiTopoType == GiftiCommon::metaDataValueTopologicalTypeCutLobar) {
            caretTopoType = "LOBAR_CUT";
         }
         
         if (caretTopoType.isEmpty() == false) {
            metaData.set(AbstractFile::headerTagPerimeterID,
                         caretTopoType);
            metaData.remove(GiftiCommon::metaDataNameTopologicalType);
         }
      }
   }

   QString commentText;
   if (metaData.get("Description", commentText)) {
      metaData.remove("Description");
      metaData.set("comment", commentText);
   }
}

/**
 * update the array's metadata before writing the array.
 */
void 
GiftiDataArray::updateMetaDataBeforeWriting()
{
   if (intentName == GiftiCommon::intentCoordinates) {
      //
      // Convert Caret surface type to GIFTI geometry type
      //
      QString caretGeomType;
      if (metaData.get(AbstractFile::headerTagConfigurationID, caretGeomType)) {
         QString giftiGeomType;
         if (caretGeomType == "Raw") {
            giftiGeomType = GiftiCommon::metaDataValueGeometricTypeReconstruction;
         }
         else if (caretGeomType == "Fiducial") {
            giftiGeomType = GiftiCommon::metaDataValueGeometricTypeAnatomical;
         }
         else if (caretGeomType == "Inflated") {
            giftiGeomType = GiftiCommon::metaDataValueGeometricTypeInflated;
         }
         else if ((caretGeomType == "Very_Inflated") ||
                  (caretGeomType == "VeryInflated")) {
            giftiGeomType = GiftiCommon::metaDataValueGeometricTypeVeryInflated;
         }
         else if (caretGeomType == "Spherical") {
            giftiGeomType = GiftiCommon::metaDataValueGeometricTypeSpherical;
         }
         else if (caretGeomType == "Ellipsoidal") {
            giftiGeomType = GiftiCommon::metaDataValueGeometricTypeEllipsoid;
         }
         else if (caretGeomType == "CompMedWall") {
            giftiGeomType = GiftiCommon::metaDataValueGeometricTypeSemiSpherical;
         }
         else if (caretGeomType == "Flat") {
            giftiGeomType = GiftiCommon::metaDataValueGeometricTypeFlat;
         }
         else if (caretGeomType == "FlatLobar") {
            giftiGeomType = GiftiCommon::metaDataValueGeometricTypeFlatLobar;
         }
         else if (caretGeomType == "Hull") {
            giftiGeomType = GiftiCommon::metaDataValueGeometricTypeHull;
         }
         
         if (giftiGeomType.isEmpty() == false) {
            metaData.set(GiftiCommon::metaDataNameGeometricType,
                         giftiGeomType);
            metaData.remove(AbstractFile::headerTagConfigurationID);
         }
      }
      
      //
      // Convert Caret structure to GIFTI structure
      //
      QString caretStructureName;
      if (metaData.get(AbstractFile::headerTagStructure, caretStructureName)) {
         caretStructureName = caretStructureName.toLower();
         QString giftiPrimaryStructureName;
         if (caretStructureName == "left") {
            giftiPrimaryStructureName = GiftiCommon::metaDataValueAnatomicalStructurePrimaryCortexLeft;
         }
         else if (caretStructureName == "right") {
            giftiPrimaryStructureName = GiftiCommon::metaDataValueAnatomicalStructurePrimaryCortexRight;
         }
         else if (caretStructureName == "both") {
            giftiPrimaryStructureName = GiftiCommon::metaDataValueAnatomicalStructurePrimaryCortexRightAndLeft;
         }
         else if (caretStructureName == "cerebellum") {
            giftiPrimaryStructureName = GiftiCommon::metaDataValueAnatomicalStructurePrimaryCerebellum;
         }
         else if (caretStructureName == "head") {
            giftiPrimaryStructureName = GiftiCommon::metaDataValueAnatomicalStructurePrimaryHead;
         }
         
         if (giftiPrimaryStructureName.isEmpty() == false) {
            metaData.set(GiftiCommon::metaDataNameAnatomicalStructurePrimary,
                         giftiPrimaryStructureName);
            metaData.remove(AbstractFile::headerTagStructure);
            
            //
            // If no secondary structure assume midlayer since
            // caret does layer 4
            //
            QString giftiSecondaryStructure;
            if (metaData.get(GiftiCommon::metaDataNameAnatomicalStructureSecondary,
                             giftiSecondaryStructure) == false) {
               metaData.set(GiftiCommon::metaDataNameAnatomicalStructureSecondary,
                            GiftiCommon::metaDataValueAnatomicalStructureSecondaryMidLayer);
            }
         }
      }

      metaData.remove("coordframe_id");
   }
   else if (intentName == GiftiCommon::intentTopologyTriangles) {
      //
      // Convert Caret Topology Type to GIFTI Topology Type
      //
      QString caretTopoType;
      if (metaData.get(AbstractFile::headerTagPerimeterID, caretTopoType)) {
         QString giftiTopoType;
         if (caretTopoType == "CLOSED") {
            giftiTopoType = GiftiCommon::metaDataValueTopologicalTypeClosed;
         }
         else if (caretTopoType == "OPEN") {
            giftiTopoType = GiftiCommon::metaDataValueTopologicalTypeOpen;
         }
         else if (caretTopoType == "CUT") {
            giftiTopoType = GiftiCommon::metaDataValueTopologicalTypeCut;
         }
         else if (caretTopoType == "LOBAR_CUT") {
            giftiTopoType = GiftiCommon::metaDataValueTopologicalTypeCutLobar;
         }
         
         if (giftiTopoType.isEmpty() == false) {
            metaData.set(GiftiCommon::metaDataNameTopologicalType,
                         giftiTopoType);
            metaData.remove(AbstractFile::headerTagPerimeterID);
         }
      }

      metaData.remove("perimeter_id");
   }
   
   QString uuidValue;
   if (metaData.get(GiftiCommon::metaDataNameUniqueID, uuidValue) == false) {
      metaData.set(GiftiCommon::metaDataNameUniqueID, 
                   QUuid::createUuid().toString());
   }
   
   QString commentText;
   if (metaData.get("comment", commentText)) {
      metaData.remove("comment");
      metaData.set("Description", commentText);
   }

   //
   // Remove these obsolete or unneeded caret header items
   //
   metaData.remove(AbstractFile::headerTagEncoding);
   metaData.remove(AbstractFile::headerTagResolution);
   metaData.remove(AbstractFile::headerTagSampling);
   metaData.remove(AbstractFile::headerTagScale);
}
      
/**
 * remove all matrices.
 */
void 
GiftiDataArray::removeAllMatrices()
{
   matrices.clear();
   setModified();
}
      
/**
 * remove a matrix.
 */
void 
GiftiDataArray::removeMatrix(const int indx)
{
   matrices.erase(matrices.begin() + indx);
   setModified();
}
      

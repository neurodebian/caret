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

#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>

#include <QTextStream>

#include "DebugControl.h"
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
                               const QString& categoryIn,
                               const DATA_TYPE dataTypeIn,
                               const std::vector<int> dimensionsIn,
                               const ENCODING encodingIn)
{
   parentGiftiDataArrayFile = parentGiftiDataArrayFileIn;
   category = categoryIn;
   dataTypeSize = 0;
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;
   clear();
   dataType = dataTypeIn;
   setDimensions(dimensionsIn);
   encoding = encodingIn;
   arraySubscriptingOrder = ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST;
   dataLocation = DATA_LOCATION_INTERNAL;
   
   if (category == GiftiCommon::categoryCoordinates) {   
      GiftiMatrix* gm = getMatrix();
      gm->setDataSpaceName(GiftiCommon::spaceLabelTalairach);
      gm->setTransformedSpaceName(GiftiCommon::spaceLabelTalairach);
   }
   
   //getDataTypeAppropriateForCategory(category, dataType);
}

/**
 * constructor.
 */
GiftiDataArray::GiftiDataArray(GiftiDataArrayFile* parentGiftiDataArrayFileIn,
                               const QString& categoryIn)
{
   parentGiftiDataArrayFile = parentGiftiDataArrayFileIn;
   category = categoryIn;
   dataTypeSize = 0;
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;
   clear();
   dimensions.clear();
   encoding = ENCODING_ASCII;
   arraySubscriptingOrder = ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST;
   dataLocation = DATA_LOCATION_INTERNAL;
   
   if (category == GiftiCommon::categoryCoordinates) {   
      GiftiMatrix* gm = getMatrix();
      gm->setDataSpaceName(GiftiCommon::spaceLabelTalairach);
      gm->setTransformedSpaceName(GiftiCommon::spaceLabelTalairach);
   }
   
   dataType = DATA_TYPE_FLOAT32;
   getDataTypeAppropriateForCategory(category, dataType);
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
   category = nda.category;
   encoding = nda.encoding;
   arraySubscriptingOrder = nda.arraySubscriptingOrder;
   dataType = nda.dataType;
   dataLocation = nda.dataLocation;
   dataTypeSize = nda.dataTypeSize;
   parentGiftiDataArrayFile = NULL;  // caused modified to be set !! nda.parentGiftiDataArrayFile;
   dimensions = nda.dimensions;
   allocateData();
   data = nda.data;
   metaData = nda.metaData;
   nonWrittenMetaData = nda.nonWrittenMetaData;
   minMaxFloatValuesValid = nda.minMaxFloatValuesValid;
   minValueFloat = nda.minValueFloat;
   maxValueFloat = nda.maxValueFloat;
   
   setModified();
}

/**
 * get the data type appropriate for the category (returns true if category is valid).
 */
bool 
GiftiDataArray::getDataTypeAppropriateForCategory(const QString& categoryIn,
                                                  DATA_TYPE& dataTypeOut)
{
   if (categoryIn == GiftiCommon::categoryCoordinates) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else if (categoryIn == GiftiCommon::categoryFunctional) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else if (categoryIn == GiftiCommon::categoryNormals) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else if (categoryIn == GiftiCommon::categoryLabels) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_INT32;
   }
   else if (categoryIn == GiftiCommon::categoryRGBA) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_UINT8;
   }
   else if (categoryIn == GiftiCommon::categoryShape) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else if (categoryIn == GiftiCommon::categoryTensors) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_FLOAT32;
   }
   else if (categoryIn == GiftiCommon::categoryTopologyTriangles) {
      dataTypeOut = GiftiDataArray::DATA_TYPE_INT32;
   }
   else {
      std::cout << "WARNING: unrecogized category \""
                << categoryIn.toAscii().constData()
                << " in GiftiDataArray::getDataTypeAppropriateForCategory()." << std::endl;
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
   encoding = ENCODING_ASCII;
   dataType = DATA_TYPE_FLOAT32;
   dataTypeSize = sizeof(float);
   metaData.clear();
   nonWrittenMetaData.clear();
   dimensions.clear();
   setDimensions(dimensions);
   minMaxFloatValuesValid = false;
   
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
GiftiDataArray::getDataTypeFromName(const QString& name)
{
   if (name == GiftiCommon::typeNameFloat32) {
      return DATA_TYPE_FLOAT32;
   }
   else if (name == GiftiCommon::typeNameInt32) {
      return DATA_TYPE_INT32;
   }
   else if (name == GiftiCommon::typeNameUInt8) {
      return DATA_TYPE_UINT8;
   }
   return DATA_TYPE_FLOAT32;
}

/**
 * get the data location type name.
 */
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

/**
 * convert data location name to type.
 */
GiftiDataArray::DATA_LOCATION 
GiftiDataArray::getDataLocationFromName(const QString& name)
{
   if (name == GiftiCommon::dataLocationInternal) {
      return DATA_LOCATION_INTERNAL;
   }
   else if (name == GiftiCommon::dataLocationExternal) {
      return DATA_LOCATION_EXTERNAL;
   }
   return DATA_LOCATION_INTERNAL;
}
      
/**
 * get encoding type name.
 */
QString 
GiftiDataArray::getEncodingName(const ENCODING encoding)
{
   switch (encoding) {
      case ENCODING_ASCII:
        return GiftiCommon::encodingNameAscii;
        break;
      case ENCODING_BASE64_BINARY:
        return GiftiCommon::encodingNameBase64Binary;
        break;
      case ENCODING_COMPRESSED_BASE64_BINARY:
        return GiftiCommon::encodingNameBase64BinaryGZip;
        break;
   }
   return "";
}      

/**
 * convert encoding name to encoding type.
 */
GiftiDataArray::ENCODING 
GiftiDataArray::getEncodingFromName(const QString& name)
{
   if (name == GiftiCommon::encodingNameAscii) {
      return ENCODING_ASCII;
   }
   else if (name == GiftiCommon::encodingNameBase64Binary) {
      return ENCODING_BASE64_BINARY;
   }
   else if (name == GiftiCommon::encodingNameBase64BinaryGZip) {
      return ENCODING_COMPRESSED_BASE64_BINARY;
   }
   return ENCODING_ASCII;
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
GiftiDataArray::getArraySubscriptingOrderFromName(const QString& name)
{
   if (name == GiftiCommon::arraySubscriptingOrderHighestFirst) {
      return ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST;
   }
   else if (name == GiftiCommon::arraySubscriptingOrderLowestFirst) {
      return ARRAY_SUBSCRIPTING_ORDER_LOWEST_FIRST;
   }
   
   return ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST;
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
 * read a GIFTI data array from text from XML.
 * Data array should already be initialized and allocated.
 */
void 
GiftiDataArray::readAsXML(QString& text,
            const QString& dataEndian,
            const ARRAY_SUBSCRIPTING_ORDER arraySubscriptingOrderForReading,
            const DATA_TYPE dataTypeForReading,
            const std::vector<int>& dimensionsForReading,
            const ENCODING encodingForReading) throw (FileException)
{
   const DATA_TYPE requiredDataType = dataType;
   dataType = dataTypeForReading;
   encoding = encodingForReading;
   dimensions = dimensionsForReading;
   if (dimensions.size() == 0) {
      throw FileException("Data array has no dimensions.");
   }
   setDimensions(dimensions);
      
   //
   // Total number of elements in Data Array
   //
   const int numElements = getTotalNumberOfElements();
   
   switch (encoding) {
      case ENCODING_ASCII:
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
      case ENCODING_BASE64_BINARY:
#ifdef HAVE_VTK
         {
            //
            // Decode the Base64 data using VTK's algorithm
            //
            const char* textChars = text.toAscii().constData();
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
            if (dataEndian.isEmpty() == false) {
               if (dataEndian != GiftiCommon::getSystemsEndianName()) {
                  byteSwapData();
               }
            }
         }
#else  // HAVE_VTK
         throw FileException("No support for Base64 data since VTK not available at compile time.");
#endif // HAVE_VTK
         break;
      case ENCODING_COMPRESSED_BASE64_BINARY:
#ifdef HAVE_VTK
         {
            //
            // Decode the Base64 data using VTK's algorithm
            //
            unsigned char* dataBuffer = new unsigned char[data.size()];
            const char* textChars = text.toAscii().constData();
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
            if (dataEndian.isEmpty() == false) {
               if (dataEndian != GiftiCommon::getSystemsEndianName()) {
                  byteSwapData();
               }
            }
         }
#else  // HAVE_VTK
         throw FileException("No support for Base64 data since VTK not available at compile time.");
#endif // HAVE_VTK
         break;
   }
   
   //
   // Are array indices in opposite order
   //
   if (arraySubscriptingOrderForReading != arraySubscriptingOrder) {
      throw FileException("Array Subscripts in opposite order and no conversion support at this time.");
   }
   
   //
   // Check if data type needs to be converted
   //
   if (requiredDataType != dataType) {
      convertToDataType(requiredDataType);
   }
   
   setModified();
}

/**
 * write the data as XML.
 */
void 
GiftiDataArray::writeAsXML(QTextStream& stream, 
                           const int indentOffset) const
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
   // Check name of data type and encoding
   //
   if (category.isEmpty()) {
      throw FileException("", "Invalid (isEmpty) XML Category.");
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
   // Indentation to improve file readability
   //
   int indent = indentOffset;
   
   //
   // Write the opening tag
   //
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "<" << GiftiCommon::tagDataArray << " " << GiftiCommon::attCategory << "=\"" 
          << category << "\"" << "\n"; 
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "           " << GiftiCommon::attDataType << "=\"" 
          << dataTypeName << "\"" << "\n"; 
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "           " << GiftiCommon::attDataLocation << "=\"" 
          << GiftiCommon::dataLocationInternal << "\"" << "\n"; 
   GiftiCommon::writeIndentationXML(stream, indent);
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
          << GiftiCommon::getSystemsEndianName() << "\"" << ">" << "\n"; 
   
   indent++;
   
   //
   // Write the metadata
   //
   metaData.writeAsXML(stream, indent);
   
   //
   // Write the matrix
   //
   matrix.writeAsXML(stream, indent);
   
   //
   // Write the data element opening tag
   //
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "<" << GiftiCommon::tagData << ">";
   indent++;
   
   //
   // NOTE: for the base64 and ZLIB-Base64 data, it is important that there are
   // no spaces between the <DATA> and </DATA> tags.
   //
   switch (encoding) {
      case ENCODING_ASCII:
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
      case ENCODING_BASE64_BINARY:
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
            
            //
            // Free memory
            //
            delete[] buffer;
         }
#else  // HAVE_VTK
         throw FileException("No support for Base64 data since VTK not available at compile time.");
#endif // HAVE_VTK
         break;
      case ENCODING_COMPRESSED_BASE64_BINARY:
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
            
            //
            // Write the data
            //
            stream << buffer;
            
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
   }
   
   //
   // Write the closing tag
   //
   indent--;
   GiftiCommon::writeIndentationXML(stream, indent);
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
         std::cout << "Converting GIFTI DataArray \"" << category.toAscii().constData() << "\""
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
                  switch (dataType) {
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
                  switch (dataType) {
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
                  switch (dataType) {
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
GiftiDataArray::byteSwapData()
{
#ifdef HAVE_VTK
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
 * get minimum and maximum values (valid for float data only).
 */
void 
GiftiDataArray::getMinMaxValues(float& minValue, float& maxValue) const
{
   if (minMaxFloatValuesValid == false) {
      minValueFloat = std::numeric_limits<float>::max();
      maxValueFloat = -std::numeric_limits<float>::min();
      
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


#ifndef __GIFTI_DATA_ARRAY_H__
#define __GIFTI_DATA_ARRAY_H__

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

#include <map>
#include <QString>
#include <vector>

#include <stdint.h>

#include "FileException.h"
#include "GiftiMatrix.h"
#include "GiftiMetaData.h"

class GiftiDataArrayFile;
class QDomElement;
class QTextStream;

/// class GiftiDataArray.
class GiftiDataArray {
   public:            
      /// data types supported
      enum DATA_TYPE {
         /// 32 bit floating point
         DATA_TYPE_FLOAT32,
         /// 32 bit signed integer
         DATA_TYPE_INT32,
         /// 8 bit unsigned integer
         DATA_TYPE_UINT8
      };
      
      /// encoding
      enum ENCODING {
         /// ascii
         ENCODING_ASCII,
         /// Base 64 Binary
         ENCODING_BASE64_BINARY,
         /// Compressed Base 64 Binary
         ENCODING_COMPRESSED_BASE64_BINARY
      };
      
      /// array subscripting order
      enum ARRAY_SUBSCRIPTING_ORDER {
         /// highest order first
         ARRAY_SUBSCRIPTING_ORDER_HIGHEST_FIRST,
         /// lowest order first (FORTRAN style)
         ARRAY_SUBSCRIPTING_ORDER_LOWEST_FIRST
      };
      
      /// data location
      enum DATA_LOCATION {
         /// location of data inside file
         DATA_LOCATION_INTERNAL,
         /// location of data external file
         DATA_LOCATION_EXTERNAL
      };
      
      // constructor
      GiftiDataArray(GiftiDataArrayFile* parentGiftiDataArrayFileIn,
                     const QString& categoryIn,
                     const DATA_TYPE dataTypeIn,
                     const std::vector<int> dimensionsIn,
                     const ENCODING encodingIn = ENCODING_ASCII);
      
      // constructor used when reading data
      GiftiDataArray(GiftiDataArrayFile* parentGiftiDataArrayFileIn,
                     const QString& categoryIn);
      
      // copy constructor
      GiftiDataArray(const GiftiDataArray& nda);
      
      // assignment operator
      GiftiDataArray& operator=(const GiftiDataArray& nda);
      
      // destructor
      virtual ~GiftiDataArray();
      
      // add rows (increase 1st dimension)
      void addRows(const int numRowsToAdd);
      
      // delete rows
      void deleteRows(const std::vector<int>& rowsToDelete);
      
      // convert all data arrays to data type
      void convertToDataType(const DATA_TYPE newDataType);
      
      // set the dimensions 
      void setDimensions(const std::vector<int> dimensionsIn);
      
      // reset column
      virtual void clear();
      
      /// set the node data file of which I am a member
      void setMyParentGiftiDataArrayFile(GiftiDataArrayFile* ndf) { parentGiftiDataArrayFile = ndf; }
      
      /// get the number of dimensions
      int getNumberOfDimensions() const { return dimensions.size(); }
      
      /// get the dimensions
      std::vector<int> getDimensions() const { return dimensions; }
      
      /// current size of the data (in bytes)
      uint32_t getDataSizeInBytes() const { return data.size(); }

      /// get a dimension
      int getDimension(const int dimIndex) const { return dimensions[dimIndex]; }
      
      // get the number of rows (1st dimension)
      int getNumberOfRows() const;
      
      // get number of components per node (2nd dimension)
      int getNumberOfComponents() const;
      
      // get the total number of elements
      int getTotalNumberOfElements() const;
      
      // get data offset 
      int getDataOffset(const int nodeNum, 
                    const int componentNum) const;
                    
      // read a data array from text
      void readAsXML(QString& text,
                     const QString& dataEndian,
                     const ARRAY_SUBSCRIPTING_ORDER arraySubscriptingOrderForReading,
                     const DATA_TYPE dataTypeForReading,
                     const std::vector<int>& dimensionsForReading,
                     const ENCODING encodingForReading) throw (FileException);
                                               
      // write the data as XML
      void writeAsXML(QTextStream& stream, 
                      const int indentOffset) const
                                             throw (FileException);
               
      // get the data type name
      static QString getDataTypeName(const DATA_TYPE dataType);

      // convert a data type name to data type
      static DATA_TYPE getDataTypeFromName(const QString& name);
      
      // get encoding type name
      static QString getEncodingName(const ENCODING encoding);
      
      /// convert encoding name to encoding type
      static ENCODING getEncodingFromName(const QString& name);
      
      /// get the data location type name
      static QString getDataLocationName(const DATA_LOCATION& location);
      
      /// convert data location name to type
      static DATA_LOCATION getDataLocationFromName(const QString& name);
      
      /// get array subscripting order name
      static QString getArraySubscriptingOrderName(const ARRAY_SUBSCRIPTING_ORDER aso);
      
      /// convert array subscripting name to type
      static ARRAY_SUBSCRIPTING_ORDER getArraySubscriptingOrderFromName(const QString& name);
      
      /// get the metadata
      GiftiMetaData* getMetaData() { return &metaData; }
      
      /// get the metadata (const method)
      const GiftiMetaData* getMetaData() const { return &metaData; }
      
      /// get the matrix
      GiftiMatrix* getMatrix() { return &matrix; }
      
      /// get the matrix (const method)
      const GiftiMatrix* getMatrix() const { return &matrix; }
      
      /// get the non-written metadata for values not saved to file
      //GiftiMetaData* getNonWrittenMetaData() { return &nonWrittenMetaData; }
      
      /// get the non-written metadata for values not save to file (const method)
      //const GiftiMetaData* getNonWrittenMetaData() const { return &nonWrittenMetaData; }
      
      /// get the data type
      DATA_TYPE getDataType() const { return dataType; }
      
      /// set the data type
      void setDataType(const DATA_TYPE dt) { dataType = dt; setModified(); }
      
      /// get the encoding
      ENCODING getEncoding() const { return encoding; }
      
      /// set the encoding
      void setEncoding(const ENCODING e) { encoding = e; setModified(); }
      
      /// get the data category
      QString getCategory() const { return category; }
      
      /// set the data category
      void setCategory(const QString& cat) { category = cat; setModified(); }
      
      /// get array subscripting order
      ARRAY_SUBSCRIPTING_ORDER getArraySubscriptingOrder() const { return arraySubscriptingOrder; }
      
      /// set array subscripting order
      void setArraySubscriptingOrder(const ARRAY_SUBSCRIPTING_ORDER aso) { arraySubscriptingOrder = aso; }
      
      /// get pointer for floating point data (valid only if data type is FLOAT)
      float* getDataPointerFloat() { return dataPointerFloat; }
      
      /// get pointer for floating point data (const method) (valid only if data type is FLOAT)
      const float* getDataPointerFloat() const { return dataPointerFloat; }
      
      /// get pointer for integer data (valid only if data type is INT)
      int32_t* getDataPointerInt() { return dataPointerInt; }
      
      /// get pointer for integer data (const method) (valid only if data type is INT)
      const int32_t* getDataPointerInt() const { return dataPointerInt; }
      
      /// get pointer for unsigned byte data (valid only if data type is UBYTE)
      uint8_t* getDataPointerUByte() { return dataPointerUByte; }
      
      /// get pointer for unsigned byte data (const method) (valid only if data type is UBYTE)
      const uint8_t* getDataPointerUByte() const { return dataPointerUByte; }
      
      // set the data array modified (actually set's the modified flag for file containing this)
      void setModified();
      
      // set all elements of array to zero
      void zeroize();
      
      // get minimum and maximum values (valid for float data only)
      void getMinMaxValues(float& minValue, float& maxValue) const;
      
      // set min max values invalid
      inline void clearMinMaxFloatValuesValid() { minMaxFloatValuesValid = false; }
      
      // remap integer values that are indices to a table
      void remapIntValues(const std::vector<int>& remappingTable);
      
      // get the data type appropriate for the category (returns true if valid category)
      static bool getDataTypeAppropriateForCategory(const QString& categoryIn,
                                                    DATA_TYPE& dataTypeOut);
      
      // get an offset for indices into data (dimensionality of indices must be same as data)
      int getDataOffset(const int indices[]) const;
      
      // get a float value (data type must be float and dimensionality of indices must be same as data)
      float getDataFloat32(const int indices[]) const;
      
      // get a float value pointer (data type must be float and dimensionality of indices must be same as data)
      const float* getDataFloat32Pointer(const int indices[]) const;
      
      // get an int value (data type must be int and dimensionality of indices must be same as data)
      int32_t getDataInt32(const int indices[]) const;
      
      // get an int value pointer(data type must be int and dimensionality of indices must be same as data)
      const int32_t* getDataInt32Pointer(const int indices[]) const;
      
      // get a byte value (data type must be unsigned char and dimensionality of indices must be same as data)
      uint8_t getDataUInt8(const int indices[]) const;
      
      // get a byte value pointer (data type must be unsigned char and dimensionality of indices must be same as data)
      const uint8_t* getDataUInt8Pointer(const int indices[]) const;
      
      // set a float value (data type must be float and dimensionality of indices must be same as data)
      void setDataFloat32(const int indices[], const float dataValue) const;
      
      // set an int value (data type must be int and dimensionality of indices must be same as data)
      void setDataInt32(const int indices[], const int32_t dataValue) const;
      
      // set a byte value (data type must be unsigned char and dimensionality of indices must be same as data)
      void setDataUInt8(const int indices[], const uint8_t dataValue) const;
      
   protected:
      // allocate data for this column
      virtual void allocateData();
                          
      // the copy helper (used by copy constructor and assignment operator)
      void copyHelperGiftiDataArray(const GiftiDataArray& nda);
      
      // update the data pointers
      void updateDataPointers();
      
      // byte swap the data (data read is different endian than this system)
      void byteSwapData();
      
      /// the data
      std::vector<uint8_t> data;
      
      /// size of one data type element
      uint32_t dataTypeSize;
      
      /// pointer for floating point data
      float* dataPointerFloat;
      
      /// pointer for integer data
      int32_t* dataPointerInt;
      
      /// pointer for unsigned byte data
      uint8_t* dataPointerUByte;
      
      /// the matrix (typically only used by coordinates)
      GiftiMatrix matrix;
      
      /// the metadata
      GiftiMetaData metaData;
      
      /// the metadata not written to file (mainly for file specific data array meta data)
      GiftiMetaData nonWrittenMetaData;
      
      /// data array file of which this is a member
      GiftiDataArrayFile* parentGiftiDataArrayFile;
      
      /// dimensions of the data
      std::vector<int> dimensions;
      
      /// data type
      DATA_TYPE dataType;
      
      /// encoding of data
      ENCODING encoding;
      
      /// location of data
      DATA_LOCATION dataLocation;
      
      /// category name
      QString category;
      
      /// array subscripting order
      ARRAY_SUBSCRIPTING_ORDER arraySubscriptingOrder;
      
      /// minimum float value
      mutable float minValueFloat;
      
      /// maximum float value
      mutable float maxValueFloat;
      
      /// min/max float values valid (child class must set this false when an array value is changed)
      mutable bool minMaxFloatValuesValid;
      
      // ***** BE SURE TO UPDATE copyHelper() if elements are added ******
      
   /// allow NodeDataFile access to protected elements
   friend class GiftiDataArrayFile;
};
      
/*
/// floating point GIFTI data array
class GiftiDataArrayFloat : public GiftiDataArray<float> {
   public:
      /// constructor
      GiftiDataArrayFloat(GiftiNodeDataFile* ndf,
                          const std::vector<int> dimensionsIn)
            : GiftiDataArray<float>(ndf, DATA_TYPE_FLOAT32, dimensionsIn) { };
            
      /// destructor
      virtual ~GiftiDataArrayFloat();
};


/// 32 bit integer GIFTI data array
class GiftiDataArrayInt32 : public GiftiDataArray<int32_t> {
   public:
      /// constructor
      GiftiDataArrayInt32(GiftiNodeDataFile* ndf,
                          const std::vector<int> dimensionsIn)
            : GiftiDataArray<int32_t>(ndf, DATA_TYPE_INT32, dimensionsIn) { };
            
      /// destructor
      virtual ~GiftiDataArrayInt32();
};


/// 8-bit unsigned byte GIFTI data array
class GiftiDataArrayUInt8 : public GiftiDataArray<uint8_t> {
   public:
      /// constructor
      GiftiDataArrayUInt8(GiftiNodeDataFile* ndf,
                          const std::vector<int> dimensionsIn)
            : GiftiDataArray<uint8_t>(ndf, DATA_TYPE_UINT8, dimensionsIn) { };
            
      /// destructor
      virtual ~GiftiDataArrayUInt8();
};
*/

#endif // __GIFTI_DATA_ARRAY_H__

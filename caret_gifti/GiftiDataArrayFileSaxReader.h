
#ifndef __GIFTI_DATA_ARRAY_FILE_SAX_READER_H__
#define __GIFTI_DATA_ARRAY_FILE_SAX_READER_H__

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

#include <stack>
#include <QString>

#include <QXmlDefaultHandler>

class GiftiLabelTable;
class GiftiMetaData;
class GiftiDataArrayFile;

#include "GiftiDataArray.h"

/// class for reading a GIFTI Node Data File with a SAX Parser
class GiftiDataArrayFileSaxReader : public QXmlDefaultHandler {
   public:
      // constructor
      GiftiDataArrayFileSaxReader(GiftiDataArrayFile* giftiFileIn);
      
      // destructor
      virtual ~GiftiDataArrayFileSaxReader();
      
      // start an element
      bool startElement(const QString& namespaceURI,
                        const QString& localName,
                        const QString& qName,
                        const QXmlAttributes& attributes);
                        
      // end an element
      bool endElement(const QString& namspaceURI,
                      const QString& localName,
                      const QString& qName);
                      
      // get characters in an element
      bool characters(const QString& s);
      
      // a fatal error occurs
      bool fatalError(const QXmlParseException& e);
      
      // get the error message
      QString getErrorMessage() const { return errorMessage; }
      
   protected:
      /// file reading states
      enum STATE {
         /// no state
         STATE_NONE,
         /// processing GIFTI tag
         STATE_GIFTI,
         /// processing MetaData tag
         STATE_METADATA,
         /// processing MetaData MD child tag
         STATE_METADATA_MD,
         /// processing MetaData MD Name tag
         STATE_METADATA_MD_NAME,
         /// processing MetaData MD Value tag
         STATE_METADATA_MD_VALUE,
         /// processing LabelTable tag
         STATE_LABEL_TABLE,
         /// processing LabelTable Label
         STATE_LABEL_TABLE_LABEL,
         /// processing DataArray tag
         STATE_DATA_ARRAY,
         /// processing DataArray Data tag
         STATE_DATA_ARRAY_DATA,
         /// processing DataArray Matrix Tag
         STATE_DATA_ARRAY_MATRIX,
         /// processing DataArray Matrix Data Space Tag
         STATE_DATA_ARRAY_MATRIX_DATA_SPACE,
         /// processing DataArray Matrix Transformed Space Tag
         STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE,
         /// processing DataArray Matrix Data Tag
         STATE_DATA_ARRAY_MATRIX_DATA
      };
      
      // process the array data into numbers
      bool processArrayData();
      
      // create a data array
      bool createDataArray(const QXmlAttributes& attributes);
      
      /// file reading state
      STATE state;
      
      /// the state stack used when reading a file
      std::stack<STATE> stateStack;
      
      /// the error message
      QString errorMessage;
      
      /// GIFTI file that is being read
      GiftiDataArrayFile* giftiFile;
      
      /// meta data name
      QString metaDataName;
      
      /// meta data value
      QString metaDataValue;
      
      /// element text
      QString elementText;
      
      /// GIFTI data array being read
      GiftiDataArray* dataArray;
      
      /// GIFTI label table being read
      GiftiLabelTable* labelTable;
      
      /// GIFTI meta data being read
      GiftiMetaData* metaData;
      
      /// GIFTI matrix data being read
      GiftiMatrix* matrix;
      
      /// label index
      int labelIndex;
      
      /// label color component
      float labelRed;

      /// label color component
      float labelGreen;

      /// label color component
      float labelBlue;

      /// label color component
      float labelAlpha;

      /// endian attribute data
      QString endianAttributeNameForReadingArrayData;
      
      /// array subscripting order for reading
      GiftiDataArray::ARRAY_SUBSCRIPTING_ORDER arraySubscriptingOrderForReadingArrayData;
      
      /// data type for reading
      GiftiDataArray::DATA_TYPE dataTypeForReadingArrayData;
      
      /// dimension for reading
      std::vector<int> dimensionsForReadingArrayData;
      
      /// encoding for reading
      GiftiDataArray::ENCODING encodingForReadingArrayData;
      
      /// data location for reading
      //GiftiDataArray::DATA_LOCATION dataLocationForReadingArrayData;
      
      /// external file name
      QString externalFileNameForReadingData;
      
      /// external file offset
      int externalFileOffsetForReadingData;
};

#endif // __GIFTI_DATA_ARRAY_FILE_SAX_READER_H__


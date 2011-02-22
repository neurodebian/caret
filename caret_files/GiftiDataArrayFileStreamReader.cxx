
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

#include <QTextStream>

#include "GiftiCommon.h"
#include "GiftiDataArrayFile.h"
#include "GiftiDataArrayFileStreamReader.h"
#include "GiftiLabelTable.h"
#include "GiftiMatrix.h"
#include "GiftiMetaData.h"
#include "StringUtilities.h"

/**
 * constructor.
 */
GiftiDataArrayFileStreamReader::GiftiDataArrayFileStreamReader(QFile* file,
                                              GiftiDataArrayFile* giftiFileIn)
   : QXmlStreamReader(file)
{
   giftiFile = giftiFileIn;
}

/**
 * destructor.
 */
GiftiDataArrayFileStreamReader::~GiftiDataArrayFileStreamReader()
{
}

/**
 * read the data.
 */
void 
GiftiDataArrayFileStreamReader::readData() throw (FileException)
{
   if (giftiFile == NULL) {
      throw FileException("GIFTI file is invalid (NULL).");
   }
   
   //
   // Loop until done
   //
   while (atEnd() == false) {
      //
      // Read the next token
      //
      readNext();
      
      //
      // Is this the GIFTI element
      //
      if (isStartElement()) {
         const QString elemName = name().toString();
         if (elemName == GiftiCommon::tagGIFTI) {
            //
            // Check Version
            //
            const QString versionString = attributes().value(GiftiCommon::attVersion).toString();
            if (versionString.isEmpty()) {
               raiseError("GIFTI file version unknown.");
            }
            const float version = versionString.toFloat();
            
            if (version == 1.0) {
               readGiftiVersion1();
            }
            else {
               raiseError("GIFTI file must be version 1.0 but is "
                          + versionString);
            }
         }
         else {
            raiseError("File is not a GIFTI file.  Element is "
                       + elemName
                       + " but should be "
                       + GiftiCommon::tagGIFTI
                       + ".");
         }
      }
   }
   
   if (error()) {
      throw FileException(errorString());
   }
}

/**
 * read GIFTI element.
 */
void 
GiftiDataArrayFileStreamReader::readGiftiVersion1()
{
   //
   // Loop through the file
   //
   while (atEnd() == false) {
      //
      // Read next element
      //
      readNext();
      
      //
      // If end of element, get out of loop
      //
      if (isEndElement()) {
         break;
      }
      
      //
      // Is this a starting element
      //
      if (isStartElement()) {
         //
         // Is this a MetaData, LabelTable, or DataArray element
         //
         if (name() == GiftiCommon::tagMetaData) {
            readMetaData(giftiFile->getMetaData());
         }
         else if (name() == GiftiCommon::tagLabelTable) {
            readLabelTable(giftiFile->getLabelTable());
         }
         else if (name() == GiftiCommon::tagDataArray) {
            readDataArray();
         }
         else {
            raiseError("Unrecognized child ("
                       + name().toString()
                       + ") of "
                       + GiftiCommon::tagGIFTI
                       + " element.");
         }
      }
   }
}

/**
 * read MetaData element.
 */
void 
GiftiDataArrayFileStreamReader::readMetaData(GiftiMetaData* metaData)
{
   QString mdName, mdValue;
   
   //
   // Loop through the file
   //
   while (atEnd() == false) {
      //
      // Read next element
      //
      readNext();
      
      //
      // If end element
      //
      if (isEndElement()) {
         //
         // If end of metadata, stop reading
         //
         if (name() == GiftiCommon::tagMetaData) {
            break;
         }       
         else if (name() == GiftiCommon::tagMD) {
            if (mdName.isEmpty() == false) {
               metaData->set(mdName, mdValue);
            }
         }
      }
      
      //
      // Is this a starting element
      //
      if (isStartElement()) {
         //
         // Is this a MD, Name, or Value element
         //
         if (name() == GiftiCommon::tagMD) {
            mdName = "";
            mdValue = "";
         }
         else if (name() == GiftiCommon::tagName) {
            mdName = readElementText();
         }
         else if (name() == GiftiCommon::tagValue) {
            mdValue = readElementText();
         }
         else {
            raiseError("Unrecognized element ("
                       + name().toString()
                       + " in metadata.");
         }
      }
   }
}

/**
 * read LabelTable element.
 */
void 
GiftiDataArrayFileStreamReader::readLabelTable(GiftiLabelTable* labelTable)
{
   //
   // Loop through the file
   //
   while (atEnd() == false) {
      //
      // Read next element
      //
      readNext();
      
      //
      // If end element
      //
      if (isEndElement()) {
         //
         // If end of metadata, stop reading
         //
         if (name() == GiftiCommon::tagLabelTable) {
            break;
         }
      }
      
      //
      // If start element
      //
      if (isStartElement()) {
         if (name() == GiftiCommon::tagLabel) {
            QString keyString = attributes().value(GiftiCommon::attKey).toString();
            if (keyString == NULL) {
               keyString = attributes().value("Index").toString();
            }
            else if (keyString.isEmpty()) {
               keyString = attributes().value("Index").toString();
            }

            float labelRed, labelGreen, labelBlue, labelAlpha;
            GiftiLabelTable::getDefaultColorFloat(labelRed,
                                                  labelGreen,
                                                  labelBlue,
                                                  labelAlpha);
            labelAlpha = 0.0;
            const QString redString = attributes().value(GiftiCommon::attRed).toString();
            if (redString.isEmpty() == false) {
               labelRed = StringUtilities::toFloat(redString);
               labelTable->setHadColorsWhenRead(true);
            }
            const QString greenString = attributes().value(GiftiCommon::attGreen).toString();
            if (greenString.isEmpty() == false) {
               labelGreen = StringUtilities::toFloat(greenString);
               labelTable->setHadColorsWhenRead(true);
            }
            const QString blueString = attributes().value(GiftiCommon::attBlue).toString();
            if (blueString.isEmpty() == false) {
               labelBlue = StringUtilities::toFloat(blueString);
               labelTable->setHadColorsWhenRead(true);
            }
            const QString alphaString = attributes().value(GiftiCommon::attAlpha).toString();
            if (alphaString.isEmpty() == false) {
               labelAlpha = StringUtilities::toFloat(alphaString);
               labelTable->setHadColorsWhenRead(true);
            }

            const QString name = readElementText();
            
            bool valid = false;
            const int indx = keyString.toInt(&valid);
            if (valid) {
               if (indx >= 0) {
                  labelTable->setLabel(indx, name);
                  labelTable->setColorFloat(indx, labelRed, labelGreen, labelBlue, labelAlpha);
               }
            }
         }
      }
   }
}

/**
 * read DataArray element.
 */
void 
GiftiDataArrayFileStreamReader::readDataArray()
{
   //
   // Get the attributes
   //
   const QString intentName = attributes().value(GiftiCommon::attIntent).toString();
   const QString arrayIndexOrderName = attributes().value(GiftiCommon::attArraySubscriptingOrder).toString();
   const QString dataTypeName = attributes().value(GiftiCommon::attDataType).toString();
   const QString dimensionalityName = attributes().value(GiftiCommon::attDimensionality).toString();
   const QString encodingName = attributes().value(GiftiCommon::attEncoding).toString();
   const QString endianName = attributes().value(GiftiCommon::attEndian).toString();
   const QString externalFileName = attributes().value(GiftiCommon::attExternalFileName).toString();
   const QString externalFileOffsetString = attributes().value(GiftiCommon::attExternalFileOffset).toString();

   //
   // Check the intent name
   //
   if (GiftiDataArray::intentNameValid(intentName) == false) {
      raiseError("Intent name invalid: "
                     + intentName); 
      return;
   }

   //
   // Data type name
   //
   bool dataTypeNameValid = false;
   GiftiDataArray::DATA_TYPE dataTypeForReadingArrayData =
                     GiftiDataArray::getDataTypeFromName(dataTypeName,
                                                         &dataTypeNameValid);
   if (dataTypeNameValid == false) {
      raiseError("Attribute "
                     + GiftiCommon::attDataType
                     + "is invalid: "
                     + dataTypeName);
      return;
   }
      
   //
   // Encoding
   //
   bool validEncoding = false;
   GiftiDataArray::ENCODING encodingForReadingArrayData = GiftiDataArray::getEncodingFromName(encodingName,
                                                                     &validEncoding);
   if (validEncoding == false) {
      raiseError("Attribute "
                     + GiftiCommon::attEncoding
                     + "is invalid: "
                     + encodingName);
      return;
   }
   bool externalDataFlag = (encodingForReadingArrayData == GiftiDataArray::ENCODING_EXTERNAL_FILE_BINARY);
    
   //
   // External File Offset
   //
   int externalFileOffsetForReadingData = 0;
   if (externalFileOffsetString.isEmpty() == false) {
      bool validOffsetFlag = false;
      externalFileOffsetForReadingData = externalFileOffsetString.toInt(&validOffsetFlag);
      if (validOffsetFlag == false) {
         raiseError("File Offset is not an integer ("
                             + externalFileOffsetString
                             + ")");
         return;
      }
   }
   
   //
   // Endian
   //
   if ((endianName != GiftiCommon::endianBig) &&
       (endianName != GiftiCommon::endianLittle)) {
      raiseError("Attribute "
                     + GiftiCommon::attEndian
                     + "is invalid: "
                     + endianName);
      return;
   }
   
   //
   // Dimensions
   // 
   if (dimensionalityName.isEmpty()) {
      raiseError("Required attribute "
                     + GiftiCommon::attDimensionality
                     + " not found for DataArray"); 
      return;
   }
   const int numDimensions = dimensionalityName.toInt();
   std::vector<int> dimensionsForReadingArrayData;
   for (int i = 0; i < numDimensions; i++) {
      const QString dimNumString = attributes().value(GiftiCommon::getAttDim(i)).toString();
      if (dimNumString.isEmpty()) {
         raiseError("Required dimension "
                        + GiftiCommon::GiftiCommon::getAttDim(i)
                        + " not found for DataArray"); 
         return;
      }
      
      const int dim = dimNumString.toInt();
      dimensionsForReadingArrayData.push_back(dim);
   }
   //
   
   // Subscript order
   //
   bool validArraySubscriptingOrder = false;
   GiftiDataArray::ARRAY_SUBSCRIPTING_ORDER arraySubscriptingOrderForReadingArrayData = 
                     GiftiDataArray::getArraySubscriptingOrderFromName(
                                                     arrayIndexOrderName,
                                                     &validArraySubscriptingOrder);   
   if (validArraySubscriptingOrder == false) {
      raiseError("Attribute "
                     + GiftiCommon::attArraySubscriptingOrder
                     + "is invalid: "
                     + arrayIndexOrderName);
      return;
   }
       
   GiftiDataArray* dataArray = new GiftiDataArray(giftiFile,
                                                  intentName);
   
   //
   // Loop through the file
   //
   bool dataWasReadFlag = false;
   while (atEnd() == false) {
      //
      // Read next element
      //
      readNext();
      
      //
      // If end element
      //
      if (isEndElement()) {
         //
         // If end of data array, stop reading
         //
         if (name() == GiftiCommon::tagDataArray) {
            if (dataWasReadFlag == false) {
               //
               // If we get here, the file had external data without
               // a set of <Data></Data> tags
               //
               if (externalDataFlag) {
                  try {
                     //
                     // Read the data
                     //
                     dataWasReadFlag = true;
                     QString text = "";
                     dataArray->readFromText(text,
                                             endianName,
                                             arraySubscriptingOrderForReadingArrayData,
                                             dataTypeForReadingArrayData,
                                             dimensionsForReadingArrayData,
                                             encodingForReadingArrayData,
                                             externalFileName,
                                             externalFileOffsetForReadingData);

                     //
                     // Add GIFTI array to GIFTI file
                     //
                     giftiFile->addDataArray(dataArray);
                  }
                  catch (FileException& e) {
                     delete dataArray;
                     raiseError(e.whatQString());
                     return;
                  }

               }
            }
            break;
         }
      }
      
      //
      // If starting element
      //
      if (isStartElement()) {  
         const QString elemName(name().toString());
                
         if (elemName == GiftiCommon::tagMetaData) {
            readMetaData(dataArray->getMetaData());
         }
         else if (elemName == GiftiCommon::tagData) {
            try {
               //
               // Read the data
               //
               dataWasReadFlag = true;
               QString text = readElementText();
               dataArray->readFromText(text,
                                       endianName,
                                       arraySubscriptingOrderForReadingArrayData,
                                       dataTypeForReadingArrayData,
                                       dimensionsForReadingArrayData,
                                       encodingForReadingArrayData,
                                       externalFileName,
                                       externalFileOffsetForReadingData);
               
               //
               // Add GIFTI array to GIFTI file
               //
               giftiFile->addDataArray(dataArray);
            }
            catch (FileException& e) {
               delete dataArray;
               raiseError(e.whatQString());
               return;
            }
         }
         else if (elemName == GiftiCommon::tagMatrix) {
            dataArray->addMatrix(GiftiMatrix());
            GiftiMatrix* matrix = dataArray->getMatrix(dataArray->getNumberOfMatrices() - 1);
            readCoordinateTransformMatrix(matrix);
         }
         else {
            raiseError("Unrecognized element ("
                       + elemName
                       + " in "
                       + GiftiCommon::tagDataArray
                       + ".");
            return;
         }
      }
   }
}

/**
 * read the coordinate transform matrix.
 */
void 
GiftiDataArrayFileStreamReader::readCoordinateTransformMatrix(GiftiMatrix* matrix)
{
   //
   // Loop through the file
   //
   while (atEnd() == false) {
      //
      // Read next element
      //
      readNext();
      
      //
      // If end element
      //
      if (isEndElement()) {
         //
         // If end of matrix, stop reading
         //
         if (name() == GiftiCommon::tagMatrix) {
            break;
         }
      }
      
      //
      // If starting element
      //
      if (isStartElement()) {  
         const QString elemName(name().toString());
                
         if (elemName == GiftiCommon::tagMatrixDataSpace) {
            matrix->setDataSpaceName(readElementText());
         }
         else if (elemName == GiftiCommon::tagMatrixTransformedSpace) {
            matrix->setTransformedSpaceName(readElementText());
         }
         else if (elemName == GiftiCommon::tagMatrixData) {
            QString text(readElementText());
            QTextStream ts(&text);
            double m[4][4];
            for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 4; j++) {
                  ts >> m[i][j];
               }
            }
            matrix->setMatrix(m);
         }
         else {
            raiseError("Unrecognized element ("
                       + elemName
                       + " in "
                       + GiftiCommon::tagMatrix
                       + ".");
            return;
         }
      }
   }
}


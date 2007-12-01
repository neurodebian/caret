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
#include <sstream>

#include <QTextStream>

#include "DebugControl.h"
#include "GiftiCommon.h"
#include "GiftiDataArrayFile.h"
#include "GiftiDataArrayFileSaxReader.h"

/**
 * constructor.
 */
GiftiDataArrayFileSaxReader::GiftiDataArrayFileSaxReader(GiftiDataArrayFile* giftiFileIn)
{
   giftiFile = giftiFileIn;
   state = STATE_NONE;
   stateStack.push(state);
   metaDataName = "";
   metaDataValue = "";
   elementText = "";
   dataArray = NULL;
   labelTable = NULL;
   metaData  = NULL;
}

/**
 * destructor.
 */
GiftiDataArrayFileSaxReader::~GiftiDataArrayFileSaxReader()
{
}


/**
 * start an element.
 */
bool 
GiftiDataArrayFileSaxReader::startElement(const QString& /* namespaceURI */,
                                         const QString& /* localName */,
                                         const QString& qName,
                                         const QXmlAttributes& attributes)
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Start Element: " << qName.toAscii().constData() << std::endl;
   }
   
   const STATE previousState = state;
   switch (state) {
      case STATE_NONE:
         if (qName == GiftiCommon::tagGIFTI) {
            state = STATE_GIFTI;
            
            //
            // Check version of file being read
            //
            const float version = attributes.value(GiftiCommon::attVersion).toFloat();
            if (version > GiftiDataArrayFile::getCurrentFileVersion()) {
               std::ostringstream str;
               str << "File version is " << version << " but this Caret"
                   << " does not support versions newer than "
                   << GiftiDataArrayFile::getCurrentFileVersion() << ".\n"
                   << "You may need a newer version of Caret.";
               errorMessage = str.str().c_str();
               return false;
            }
            else if (version < 1.0) {
               errorMessage = "File version is " + QString::number(version, 'f', 3) + " but this Caret"
                    " does not support versions before 1.0";
               return false;
            }
         }
         else {
            std::ostringstream str;
            str << "Root element is \"" << qName.toAscii().constData() << "\" but should be "
                << GiftiCommon::tagGIFTI.toAscii().constData();
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_GIFTI:
         if (qName == GiftiCommon::tagMetaData) {
            state = STATE_METADATA;
            metaData = giftiFile->getMetaData();
         }
         else if (qName == GiftiCommon::tagDataArray) {
            state = STATE_DATA_ARRAY;
            if (createDataArray(attributes) == false) {
               return false;
            }
         }
         else if (qName == GiftiCommon::tagLabelTable) {
            state = STATE_LABEL_TABLE;
            labelTable = giftiFile->getLabelTable();
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiCommon::tagGIFTI.toAscii().constData() << " is \"" << qName.toAscii().constData() 
                << "\" but should be one of \n"
                << "   " << GiftiCommon::tagMetaData.toAscii().constData() << "\n"
                << "   " << GiftiCommon::tagDataArray.toAscii().constData() << "\n"
                << "   " << GiftiCommon::tagLabelTable.toAscii().constData();
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_METADATA:
         if (qName == GiftiCommon::tagMD) {
            state = STATE_METADATA_MD;
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiCommon::tagMetaData.toAscii().constData() << " is \"" << qName.toAscii().constData() 
                << "\" but should be " << GiftiCommon::tagMD.toAscii().constData();
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_METADATA_MD:
         if (qName == GiftiCommon::tagName) {
            state = STATE_METADATA_MD_NAME;
         }
         else if (qName == GiftiCommon::tagValue) {
            state = STATE_METADATA_MD_VALUE;
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiCommon::tagMD.toAscii().constData() << " is \"" << qName.toAscii().constData() 
                << "\" but should be one of \n"
                << "   " << GiftiCommon::tagName.toAscii().constData() << "\n"
                << "   " << GiftiCommon::tagValue.toAscii().constData();
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_METADATA_MD_NAME:
         {
            std::ostringstream str;
            str << GiftiCommon::tagName.toAscii().constData() << " has child \"" << qName.toAscii().constData() 
                << "\" but should not have any child nodes";
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_METADATA_MD_VALUE:
         {
            std::ostringstream str;
            str << GiftiCommon::tagValue.toAscii().constData() << " has child \"" << qName.toAscii().constData() 
                << "\" but should not have any child nodes";
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_LABEL_TABLE:
         if (qName == GiftiCommon::tagLabel) {
            state = STATE_LABEL_TABLE_LABEL;
            const QString s = attributes.value(GiftiCommon::attIndex);
            if (s.isEmpty()) {
               std::ostringstream str;
               str << "Tag "
                   << GiftiCommon::tagLabel.toAscii().constData()
                   << " is missing its "
                   << GiftiCommon::attIndex.toAscii().constData();
               errorMessage = str.str().c_str();
               return false;
            }
            labelIndex = s.toInt();
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiCommon::tagLabelTable.toAscii().constData() << " is \"" << qName.toAscii().constData() 
                << "\" but should be " << GiftiCommon::tagLabel.toAscii().constData();
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_LABEL_TABLE_LABEL:
         {
            std::ostringstream str;
            str << GiftiCommon::tagLabel.toAscii().constData() << " has child \"" << qName.toAscii().constData() 
                << "\" but should not have any child nodes";
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_DATA_ARRAY:
         if (qName == GiftiCommon::tagMetaData) {
            state = STATE_METADATA;
            metaData = dataArray->getMetaData();
         }
         else if (qName == GiftiCommon::tagData) {
            state = STATE_DATA_ARRAY_DATA;
         }
         else if (qName == GiftiCommon::tagMatrix) {
            state = STATE_DATA_ARRAY_MATRIX;
            matrix = dataArray->getMatrix();
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiCommon::tagDataArray.toAscii().constData() << " is \"" << qName.toAscii().constData() 
                << "\" but should be one of \n"
                << "   " << GiftiCommon::tagMetaData.toAscii().constData() << "\n"
                << "   " << GiftiCommon::tagMatrix.toAscii().constData() << "\n"
                << "   " << GiftiCommon::tagData.toAscii().constData();
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_DATA_ARRAY_DATA:
         {
            std::ostringstream str;
            str << GiftiCommon::tagData.toAscii().constData() << " has child \"" << qName.toAscii().constData() 
                << "\" but should not have any child nodes";
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_DATA_ARRAY_MATRIX:
         if (qName == GiftiCommon::tagMatrixDataSpace) {
            state = STATE_DATA_ARRAY_MATRIX_DATA_SPACE;
         }
         else if (qName == GiftiCommon::tagMatrixTransformedSpace) {
            state = STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE;
         }
         else if (qName == GiftiCommon::tagMatrixData) {
            state = STATE_DATA_ARRAY_MATRIX_DATA;
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiCommon::tagMatrix.toAscii().constData() << " is \"" << qName.toAscii().constData() 
                << "\" but should be one of \n"
                << "   " << GiftiCommon::tagMatrixDataSpace.toAscii().constData() << "\n"
                << "   " << GiftiCommon::tagMatrixTransformedSpace.toAscii().constData() << "\n"
                << "   " << GiftiCommon::tagMatrixData.toAscii().constData();
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA_SPACE:
         {
            std::ostringstream str;
            str << GiftiCommon::tagMatrixDataSpace.toAscii().constData() << " has child \"" << qName.toAscii().constData() 
                << "\" but should not have any child nodes";
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE:
         {
            std::ostringstream str;
            str << GiftiCommon::tagMatrixTransformedSpace.toAscii().constData() << " has child \"" << qName.toAscii().constData() 
                << "\" but should not have any child nodes";
            errorMessage = str.str().c_str();
            return false;
         }
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA:
         {
            std::ostringstream str;
            str << GiftiCommon::tagMatrixData.toAscii().constData() << " has child \"" << qName.toAscii().constData() 
                << "\" but should not have any child nodes";
            errorMessage = str.str().c_str();
            return false;
         }
         break;
   }
   
   //
   // Save previous state
   //
   stateStack.push(previousState);
   
   elementText = "";
   
   //
   // Continue parsing
   //
   return true;
}

/**
 * end an element.
 */
bool 
GiftiDataArrayFileSaxReader::endElement(const QString& /* namspaceURI */,
                                       const QString& /* localName */,
                                       const QString& qName)
{
   if (DebugControl::getDebugOn()) {
      std::cout << "End Element: " << qName.toAscii().constData() << std::endl;
   }

   switch (state) {
      case STATE_NONE:
         break;
      case STATE_GIFTI:
         break;
      case STATE_METADATA:
         metaData = NULL;
         break;
      case STATE_METADATA_MD:
         if ((metaDataName.isEmpty() == false) &&
             (metaDataValue.isEmpty() == false)) {
            if (metaData != NULL) {
               metaData->set(metaDataName, metaDataValue);
            }
            else {
               errorMessage = "ERROR: Have metadata name/value but no MetaDeta.";
               return false; 
            }
            metaDataName = "";
            metaDataValue = "";
         }
         break;
      case STATE_METADATA_MD_NAME:
         metaDataName = elementText;
         break;
      case STATE_METADATA_MD_VALUE:
         metaDataValue = elementText;
         break;
      case STATE_LABEL_TABLE:
         labelTable = NULL;
         break;
      case STATE_LABEL_TABLE_LABEL:
         labelTable->setLabel(labelIndex, elementText);
         break;
      case STATE_DATA_ARRAY:
         if (dataArray != NULL) {
            giftiFile->addDataArray(dataArray);
            dataArray = NULL;
         }
         else {
         }
         break;
      case STATE_DATA_ARRAY_DATA:
         if (processArrayData() == false) {
            return false;
         }
         break;
      case STATE_DATA_ARRAY_MATRIX:
         matrix = NULL;
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA_SPACE:
         matrix->setDataSpaceName(elementText);
         break;
      case STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE:
         matrix->setTransformedSpaceName(elementText);
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA:
         {
            QTextStream ts(&elementText);
            double m[4][4];
            for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 4; j++) {
                  ts >> m[i][j];
               }
            }
            matrix->setMatrix(m);
         }
         break;
   }

   //
   // Clear out for new elements
   //
   elementText = "";
   
   //
   // Go to previous state
   //
   if (stateStack.empty()) {
      errorMessage = "State stack is isEmpty while reading XML NiftDataFile.";
      return false;
   }
   state = stateStack.top();
   stateStack.pop();
   
   //
   // Continue parsing
   //
   return true;
}

/**
 * create a data array.
 */
bool 
GiftiDataArrayFileSaxReader::createDataArray(const QXmlAttributes& attributes)
{
   //
   // Intent
   //
   QString intentName = attributes.value(GiftiCommon::attIntent);
   if (intentName.isEmpty()) {
      intentName = attributes.value("Intent");
   }
   if (intentName.isEmpty()) {
      errorMessage = "Required attribute "
                     + GiftiCommon::attIntent
                     + " not found for DataArray"; 
      return false;
   }
   if (GiftiDataArray::intentNameValid(intentName) == false) {
      errorMessage = "Intent name invalid: "
                     + intentName; 
      return false;
   }
   
   //
   // Data type name
   //
   const QString dataTypeName = attributes.value(GiftiCommon::attDataType);
   if (dataTypeName.isEmpty()) {
      errorMessage = "Required attribute "
                     + GiftiCommon::attDataType
                     + " not found for DataArray"; 
      return false;
   }
   bool dataTypeNameValid = false;
   dataTypeForReadingArrayData = GiftiDataArray::getDataTypeFromName(dataTypeName,
                                                                     &dataTypeNameValid);
   if (dataTypeNameValid == false) {
      errorMessage = "Attribute "
                     + GiftiCommon::attDataType
                     + "is invalid: "
                     + dataTypeName;
      return false;
   }
      
   //
   // Encoding
   //
   const QString encodingName = attributes.value(GiftiCommon::attEncoding);
   if (encodingName.isEmpty()) {
      errorMessage = "Required attribute "
                     + GiftiCommon::attEncoding
                     + " not found for DataArray"; 
      return false;
   }
   bool validEncoding = false;
   encodingForReadingArrayData = GiftiDataArray::getEncodingFromName(encodingName,
                                                                     &validEncoding);
   if (validEncoding == false) {
      errorMessage = "Attribute "
                     + GiftiCommon::attEncoding
                     + "is invalid: "
                     + encodingName;
      return false;
   }
    
   //
   // External File Name
   //
   externalFileNameForReadingData = attributes.value(GiftiCommon::attExternalFileName);
   
   //
   // External File Offset
   //
   externalFileOffsetForReadingData = 0;
   const QString offsetString = attributes.value(GiftiCommon::attExternalFileOffset);
   if (offsetString.isEmpty() == false) {
      bool validOffsetFlag = false;
      externalFileOffsetForReadingData = offsetString.toInt(&validOffsetFlag);
      if (validOffsetFlag == false) {
         throw FileException("File Offset is not an integer ("
                             + offsetString
                             + ")");
      }
   }
   
   //
   // Endian
   //
   endianAttributeNameForReadingArrayData = attributes.value(GiftiCommon::attEndian);
   if (endianAttributeNameForReadingArrayData.isEmpty()) {
      errorMessage = "Required attribute "
                     + GiftiCommon::attEndian
                     + " not found for DataArray"; 
      return false;
   }
   if ((endianAttributeNameForReadingArrayData != GiftiCommon::endianBig) &&
       (endianAttributeNameForReadingArrayData != GiftiCommon::endianLittle)) {
      errorMessage = "Attribute "
                     + GiftiCommon::attEndian
                     + "is invalid: "
                     + endianAttributeNameForReadingArrayData;
      return false;
   }
   
   //
   // Dimensions
   // 
   const QString dimString = attributes.value(GiftiCommon::attDimensionality);
   if (dimString.isEmpty()) {
      errorMessage = "Required attribute "
                     + GiftiCommon::attDimensionality
                     + " not found for DataArray"; 
      return false;
   }
   const int numDimensions = dimString.toInt();
   dimensionsForReadingArrayData.clear();
   for (int i = 0; i < numDimensions; i++) {
      const QString dimNumString = attributes.value(GiftiCommon::getAttDim(i));
      if (dimNumString.isEmpty()) {
         errorMessage = "Required dimension "
                        + GiftiCommon::GiftiCommon::getAttDim(i)
                        + " not found for DataArray"; 
         return false;
      }
      
      const int dim = dimNumString.toInt();
      dimensionsForReadingArrayData.push_back(dim);
   }
   
   //
   // Data Location
   //
/*
   const QString dataLocationString = attributes.value(GiftiCommon::attDataLocation);
   if (dataLocationString.isEmpty()) {
      errorMessage = "Required attribute "
                     + GiftiCommon::attDataLocation
                     + " not found for DataArray"; 
      return false;
   }
   bool validDataLocation = false;
   dataLocationForReadingArrayData = GiftiDataArray::getDataLocationFromName(dataLocationString,
                                                                             &validDataLocation);
   if (validDataLocation == false) {
      errorMessage = "Attribute "
                     + GiftiCommon::attDataLocation
                     + "is invalid: "
                     + dataLocationString;
      return false;
   }
   if (dataLocationForReadingArrayData == GiftiDataArray::DATA_LOCATION_EXTERNAL) {
      errorMessage = "External data storage not supported.";
      return false;
   }
*/
   //
   // Subscript order
   //
   const QString subscriptOrderString = attributes.value(GiftiCommon::attArraySubscriptingOrder);
   if (subscriptOrderString.isEmpty()) {
      errorMessage = "Required attribute "
                     + GiftiCommon::attArraySubscriptingOrder
                     + " not found for DataArray"; 
      return false;
   }
   bool validArraySubscriptingOrder = false;
   arraySubscriptingOrderForReadingArrayData = GiftiDataArray::getArraySubscriptingOrderFromName(
                                                     subscriptOrderString,
                                                     &validArraySubscriptingOrder);   
   if (validArraySubscriptingOrder == false) {
      errorMessage = "Attribute "
                     + GiftiCommon::attArraySubscriptingOrder
                     + "is invalid: "
                     + subscriptOrderString;
      return false;
   }
         
   dataArray = new GiftiDataArray(giftiFile,
                                  intentName);
   return true;
}

/**
 * process the array data into numbers.
 */
bool 
GiftiDataArrayFileSaxReader::processArrayData()
{
   //
   // Should the data arrays be read ?
   //
   //if (giftiFile->getReadMetaDataOnlyFlag()) {
   //   return true;
   //}
   
   try {
      dataArray->readFromText(elementText, 
                           endianAttributeNameForReadingArrayData,
                           arraySubscriptingOrderForReadingArrayData,
                           dataTypeForReadingArrayData,
                           dimensionsForReadingArrayData,
                           encodingForReadingArrayData,
                           externalFileNameForReadingData,
                           externalFileOffsetForReadingData);
   }
   catch (FileException& e) {
      errorMessage = e.whatQString();
      return false;
   }

   return true;
}

/**
 * get characters in an element.
 */
bool 
GiftiDataArrayFileSaxReader::characters(const QString& s)
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Characters (50 max): " << s.left(50).toAscii().constData() << std::endl;
   }
   
   elementText += s;
   //
   // Continue parsing
   //
   return true;
}

/**
 * a fatal error occurs.
 */
bool 
GiftiDataArrayFileSaxReader::fatalError(const QXmlParseException& e)
{
   std::ostringstream str;
   str << "Fatal Error at line number: " << e.lineNumber() << "\n"
       << "Column number: " << e.columnNumber() << "\n"
       << "Message: " << e.message().toAscii().constData();
   if (errorMessage.isEmpty() == false) {
      str << "\n"
          << errorMessage.toAscii().constData();
   }
   errorMessage = str.str().c_str();
   
   //
   // Stop parsing
   //
   return false;
}

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
            const int version = attributes.value(GiftiCommon::attVersion).toInt();
            if (version > GiftiDataArrayFile::getCurrentFileVersion()) {
               std::ostringstream str;
               str << "File version is " << version << " but this Caret"
                   << " does not support versions newer than "
                   << GiftiDataArrayFile::getCurrentFileVersion() << ".\n"
                   << "You may need a newer version of Caret.";
               errorMessage = str.str().c_str();
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

/*
   switch (state) {
      case STATE_NONE:
         break;
      case STATE_GIFTI:
         break;
      case STATE_METADATA:
         break;
      case STATE_METADATA_MD:
         break;
      case STATE_METADATA_MD_NAME:
         break;
      case STATE_METADATA_MD_VALUE:
         break;
      case STATE_NAME_TABLE:
         break;
      case STATE_NAME_TABLE_LABEL:
         break;
      case STATE_DATA_ARRAY:
         break;
      case STATE_DATA_ARRAY_DATA:
         break;
   }
*/
                  
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
   const QString category = attributes.value(GiftiCommon::attCategory);
   
   dataTypeForReadingArrayData = 
           GiftiDataArray::getDataTypeFromName(attributes.value(GiftiCommon::attDataType));
              
   encodingForReadingArrayData = GiftiDataArray::getEncodingFromName(
                               attributes.value(GiftiCommon::attEncoding));
                               
   endianAttributeNameForReadingArrayData = attributes.value(GiftiCommon::attEndian);
   
   const int numDimensions = attributes.value(GiftiCommon::attDimensionality).toInt();
   dimensionsForReadingArrayData.clear();
   for (int i = 0; i < numDimensions; i++) {
      const int dim = attributes.value(GiftiCommon::getAttDim(i)).toInt();
      dimensionsForReadingArrayData.push_back(dim);
   }
   
   dataLocationForReadingArrayData = GiftiDataArray::getDataLocationFromName(
                                        attributes.value(GiftiCommon::attDataLocation));
   if (dataLocationForReadingArrayData == GiftiDataArray::DATA_LOCATION_EXTERNAL) {
      errorMessage = "External data storage not supported.";
      return false;
   }

   arraySubscriptingOrderForReadingArrayData = GiftiDataArray::getArraySubscriptingOrderFromName(
                                        attributes.value(GiftiCommon::attArraySubscriptingOrder));   
         
   dataArray = new GiftiDataArray(giftiFile,
                                  category);
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
   if (giftiFile->getReadMetaDataOnlyFlag()) {
      return true;
   }
   
   try {
      dataArray->readAsXML(elementText, 
                           endianAttributeNameForReadingArrayData,
                           arraySubscriptingOrderForReadingArrayData,
                           dataTypeForReadingArrayData,
                           dimensionsForReadingArrayData,
                           encodingForReadingArrayData);
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

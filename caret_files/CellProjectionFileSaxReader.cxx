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

#include "CellProjectionFile.h"
#include "CellProjectionFileSaxReader.h"
#include "DebugControl.h"

/**
 * constructor.
 */
CellProjectionFileSaxReader::CellProjectionFileSaxReader(CellProjectionFile* cellProjectionFileIn)
{
   cellProjectionFile = cellProjectionFileIn;
   elementText = "";
   state = STATE_NONE;
   stateStack.push(state);
}

/**
 * destructor.
 */
CellProjectionFileSaxReader::~CellProjectionFileSaxReader()
{
}


/**
 * start an element.
 */
bool 
CellProjectionFileSaxReader::startElement(const QString& /* namespaceURI */,
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
         if (qName == cellProjectionFile->getRootXmlElementTagName()) {
            state = STATE_CELL_PROJ_FILE;
         }
         else {
            errorMessage =
               "Root element is \"" 
               + qName
               + "\" but should be "
               + cellProjectionFile->getRootXmlElementTagName();
            return false;
         }
         break;
      case STATE_CELL_PROJ_FILE:
         if (qName == AbstractFile::xmlHeaderTagName) {
            state = STATE_FILE_HEADER;
         }
         else if (qName == CellProjection::tagCellProjection) {
            state = STATE_CELL_PROJECTION;
         }
         else if (qName == CellStudyInfo::tagCellStudyInfo) {
            state = STATE_STUDY_INFO;
         }
         else {
            errorMessage =
               "Child of "
               + cellProjectionFile->getDescriptiveName() 
               + " is \"" 
               + qName 
               + "\" but should be one of \n"
               + "   "  + AbstractFile::xmlHeaderTagName + "\n"
               + "   "  + CellProjection::tagCellProjection + "\n"
               + "   "  + CellStudyInfo::tagCellStudyInfo;
            return false;
         }
         break;
      case STATE_FILE_HEADER:
         if (qName == AbstractFile::xmlHeaderElementTagName) {
            state = STATE_FILE_HEADER_ELEMENT;
         }
         else {
            errorMessage =
               "Child of "
               + AbstractFile::xmlHeaderTagName 
               + " is \"" 
               + qName 
               + "\" but should be  \n"
               + "   "  + AbstractFile::xmlHeaderElementTagName + "\n";
            return false;
         }
         break;
      case STATE_FILE_HEADER_ELEMENT:
         if (qName == AbstractFile::xmlHeaderElementName) {
            state = STATE_FILE_HEADER_ELEMENT_NAME;
            headerNameText = "";
         }
         else if (qName == AbstractFile::xmlHeaderElementValue) {
            state = STATE_FILE_HEADER_ELEMENT_VALUE;
         }
         else {
            errorMessage =
               "Child of "
               + AbstractFile::xmlHeaderElementName 
               + " is \"" 
               + qName 
               + "\" but should be one of \n"
               + "   "  + AbstractFile::xmlHeaderElementName + "\n"
               + "   "  + AbstractFile::xmlHeaderElementValue + "\n";
            return false;
         }
         break;
      case STATE_FILE_HEADER_ELEMENT_NAME:
         errorMessage = AbstractFile::xmlHeaderElementName 
                        + " has child named \"" 
                        + qName 
                        + "\" but should not have any child nodes.";
         return false;
         break;
      case STATE_FILE_HEADER_ELEMENT_VALUE:
         errorMessage = AbstractFile::xmlHeaderElementValue 
                        + " has child named \"" 
                        + qName 
                        + "\" but should not have any child nodes.";
         return false;
         break;
      case STATE_CELL_PROJECTION:
         if (qName == CellProjection::tagCellProjection) {
            cellProjection.initialize(cellProjectionFile->getFileName());
            studyMetaDataLinkSet.clear();
         }
         else if (qName == CellBase::tagCellBase) {
            state = STATE_CELL_BASE;
         }
         break;
      case STATE_CELL_BASE:
         if (qName == StudyMetaDataLinkSet::tagStudyMetaDataLinkSet) {
            state = STATE_METADATA_LINK_SET;
            studyMetaDataLinkSet.clear();
         }
         else if (qName == StudyMetaDataLink::tagStudyMetaDataLink) {
            state = STATE_METADATA_LINK;
            studyMetaDataLink.clear();
            studyMetaDataLinkSet.clear();
         }
         break;
      case STATE_STUDY_INFO:
         if (qName == CellStudyInfo::tagCellStudyInfo) {
            cellStudyInfo.clear();
         }
         break;
      case STATE_METADATA_LINK_SET:
         if (qName == StudyMetaDataLink::tagStudyMetaDataLink) {
            state = STATE_METADATA_LINK;
            studyMetaDataLink.clear();
         }
         else {
            errorMessage =
               "Child of "
               + StudyMetaDataLinkSet::tagStudyMetaDataLinkSet 
               + " is \"" 
               + qName 
               + "\" but should be \n"
               + "   "  + StudyMetaDataLink::tagStudyMetaDataLink;
            return false;
         }
         break;
      case STATE_METADATA_LINK:
         break;
   }
   
   //
   // Save previous state
   //
   stateStack.push(previousState);
   
   //
   // starting new element
   //
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
CellProjectionFileSaxReader::endElement(const QString& /* namspaceURI */,
                                       const QString& /* localName */,
                                       const QString& qName)
{
   if (DebugControl::getDebugOn()) {
      std::cout << "End Element: " << qName.toAscii().constData() << std::endl;
      std::cout << "       Text: " << elementText.toAscii().constData() << std::endl;
   }

   switch (state) {
      case STATE_NONE:
         break;
      case STATE_CELL_PROJ_FILE:
         break;
      case STATE_FILE_HEADER:
         break;
      case STATE_FILE_HEADER_ELEMENT:
         break;
      case STATE_FILE_HEADER_ELEMENT_NAME:
         headerNameText = elementText;
         break;
      case STATE_FILE_HEADER_ELEMENT_VALUE:
         cellProjectionFile->setHeaderTag(headerNameText, elementText);
         break;
      case STATE_CELL_PROJECTION:
         if (qName == CellProjection::tagCellProjection) {
            cellProjectionFile->addCellProjection(cellProjection);
         }
         else {
            cellProjection.setElementFromText(qName, elementText);
         }
         break;
      case STATE_CELL_BASE:
         if (qName != CellBase::tagCellBase) {
            cellProjection.setBaseElementFromText(qName, elementText);
         }
         break;
      case STATE_STUDY_INFO:
         if (qName == CellStudyInfo::tagCellStudyInfo) {
            cellProjectionFile->addStudyInfo(cellStudyInfo);
         }
         else {
            cellStudyInfo.setElementFromText(qName, elementText);
         }
         break;
      case STATE_METADATA_LINK_SET:
         cellProjection.setStudyMetaDataLinkSet(studyMetaDataLinkSet);
         break;
      case STATE_METADATA_LINK:
         if (qName == StudyMetaDataLink::tagStudyMetaDataLink) {
            studyMetaDataLinkSet.addStudyMetaDataLink(studyMetaDataLink);
            cellProjection.setStudyMetaDataLinkSet(studyMetaDataLinkSet);
         }
         else {
            studyMetaDataLink.setElementFromText(qName, elementText);
         }
         break;
   }
   
   //
   // Clear out for next element
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
 * get characters in an element.
 */
bool 
CellProjectionFileSaxReader::characters(const QString& s)
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
CellProjectionFileSaxReader::fatalError(const QXmlParseException& e)
{
   QString msg =
      "Fatal Error at line number: " 
          + QString::number(e.lineNumber()) 
          + "\n"
       "Column number: " 
          + QString::number(e.columnNumber())
          + "\n"
       "Message: "
          +  e.message();
          
   if (errorMessage.isEmpty() == false) {
      msg += ("\n"
              + errorMessage);
   }
   
   errorMessage = msg;
   
   //
   // Stop parsing
   //
   return false;
}


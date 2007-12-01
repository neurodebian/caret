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

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QStringList>

#define __STUDY_META_DATA_LINK_MAIN__
#include "StudyMetaDataLink.h"
#undef __STUDY_META_DATA_LINK_MAIN__
#include "AbstractFile.h"

//====================================================================================
//
// StudyMetaData Link class
//
//====================================================================================

/**
 * constructor.
 */
StudyMetaDataLink::StudyMetaDataLink()
{
   clear();
}

/**
 * destructor.
 */
StudyMetaDataLink::~StudyMetaDataLink()
{
}

/**
 * copy constructor.
 */
StudyMetaDataLink::StudyMetaDataLink(const StudyMetaDataLink& smdl)
{
   copyHelper(smdl);
}

/**
 * assignment opertator.
 */
StudyMetaDataLink& 
StudyMetaDataLink::operator=(const StudyMetaDataLink& smdl)
{
   if (this != &smdl) {
      copyHelper(smdl);
   }
   
   return *this;
}
      
/**
 * copy helper.
 */
void 
StudyMetaDataLink::copyHelper(const StudyMetaDataLink& smdl)
{
   pubMedID = smdl.pubMedID;
   tableNumber = smdl.tableNumber;
   tableSubHeaderNumber = smdl.tableSubHeaderNumber;
   figureNumber = smdl.figureNumber;
   panelNumberOrLetter = smdl.panelNumberOrLetter;
   pageNumber = smdl.pageNumber;
   pageReferencePageNumber = smdl.pageReferencePageNumber;
   pageReferenceSubHeaderNumber = smdl.pageReferenceSubHeaderNumber;
}

/**
 * equality operator.
 */
bool 
StudyMetaDataLink::operator==(const StudyMetaDataLink& smdl) const
{
   const bool theSame = 
      ((pubMedID == smdl.pubMedID) &&
       (tableNumber == smdl.tableNumber) &&
       (tableSubHeaderNumber == smdl.tableSubHeaderNumber) &&
       (figureNumber == smdl.figureNumber) &&
       (panelNumberOrLetter == smdl.panelNumberOrLetter) &&
       (pageNumber == smdl.pageNumber) &&
       (pageReferencePageNumber == smdl.pageReferencePageNumber) &&
       (pageReferenceSubHeaderNumber == smdl.pageReferenceSubHeaderNumber));
   return theSame;
}
      
/**
 * clear the link.
 */
void 
StudyMetaDataLink::clear()
{
   pubMedID = "0";
   tableNumber = "";
   tableSubHeaderNumber = "";
   figureNumber = "";
   panelNumberOrLetter = "";
   pageNumber = "";
   pageReferencePageNumber = "";
   pageReferenceSubHeaderNumber = "";
}
      
/**
 * set the table number (blank if invalid).
 */
void 
StudyMetaDataLink::setTableNumber(const QString& tn) 
{ 
   if (tn == "-1") {
      tableNumber = "";
   }
   else {
      tableNumber = tn; 
   }
}

/**
 * set the table sub header number (blank if invalid).
 */
void 
StudyMetaDataLink::setTableSubHeaderNumber(const QString& tshn) 
{ 
   if (tshn == "-1") {
      tableSubHeaderNumber = "";
   }
   else {
      tableSubHeaderNumber = tshn; 
   }
}

/**
 * set the panel letter/number (blank if invalid).
 */
void 
StudyMetaDataLink::setFigurePanelNumberOrLetter(const QString& pnl)
{
   if (pnl == "-1") {
      panelNumberOrLetter = "";
   }
   else {
      panelNumberOrLetter = pnl; 
   }
}
      
/**
 * set the figure number (blank if invalid).
 */
void 
StudyMetaDataLink::setFigureNumber(const QString& fn) 
{ 
   if (fn == "-1") {
      figureNumber = "";
   }
   else {
      figureNumber = fn; 
   }
}

/**
 * set the page reference page number (blank if invalid).
 */
void 
StudyMetaDataLink::setPageReferencePageNumber(const QString& prpn) 
{ 
   if (prpn == "-1") {
      pageReferencePageNumber = "";
   }
   else {
      pageReferencePageNumber = prpn; 
   }
}

/**
 * set the page reference sub header number (blank if invalid).
 */
void 
StudyMetaDataLink::setPageReferenceSubHeaderNumber(const QString& tshn) 
{ 
   if (tshn == "-1") {
      pageReferenceSubHeaderNumber = "";
   }
   else {
      pageReferenceSubHeaderNumber = tshn; 
   }
}

/**
 * set the page number (negative if invalid).
 */
void 
StudyMetaDataLink::setPageNumber(const QString& pn) 
{ 
   if (pn == "-1") {
      pageNumber = "";
   }
   else {
      pageNumber = pn; 
   }
}

/**
 * set element from text (used by SAX XML parser).
 */
void 
StudyMetaDataLink::setElementFromText(const QString& elementName,
                                      const QString& textValue)
{
   if (elementName == tagPubMedID) {
      setPubMedID(textValue);
   }
   else if (elementName == tagTableNumber) {
      setTableNumber(textValue);
   }
   else if (elementName == tagTableSubHeaderNumber) {
      setTableSubHeaderNumber(textValue);
   }
   else if (elementName == tagFigureNumber) {
      setFigureNumber(textValue);
   }
   else if (elementName == tagPanelNumberOrLetter) {
      setFigurePanelNumberOrLetter(textValue);
   }
   else if (elementName == tagPageNumber) {
      setPageNumber(textValue);
   }
   else if (elementName == tagPageReferencePageNumber) {
      setPageReferencePageNumber(textValue);
   }
   else if (elementName == tagPageReferenceSubHeaderNumber) {
      setPageReferenceSubHeaderNumber(textValue);
   }
   else {
      std::cout << "WARNING: unrecognized StudyMetaDataLink element ignored: "
                << elementName.toAscii().constData()
                << std::endl;
   }
}
      
/**
 * called to read from an XML structure.
 */
void 
StudyMetaDataLink::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != tagStudyMetaDataLink) {
      QString msg("Incorrect element type passed to StudyMetaDataLink::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == tagPubMedID) {
            setPubMedID(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagTableNumber) {
            setTableNumber(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagTableSubHeaderNumber) {
            setTableSubHeaderNumber(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagFigureNumber) {
            setFigureNumber(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagPanelNumberOrLetter) {
            setFigurePanelNumberOrLetter(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagPageNumber) {
            setPageNumber(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagPageReferencePageNumber) {
            setPageReferencePageNumber(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagPageReferenceSubHeaderNumber) {
            setPageReferenceSubHeaderNumber(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else {
            std::cout << "WARNING: unrecognized StudyMetaDataLink element ignored: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
      }
      node = node.nextSibling();
   }
}

/**
 * called to write to an XML structure.
 */
void 
StudyMetaDataLink::writeXML(QDomDocument& xmlDoc,
                            QDomElement&  parentElement) const throw (FileException)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement linkElement = xmlDoc.createElement(tagStudyMetaDataLink);

   //
   // Add the study metadata
   //
   AbstractFile::addXmlCdataElement(xmlDoc, linkElement, tagPubMedID, pubMedID);
   AbstractFile::addXmlCdataElement(xmlDoc, linkElement, tagTableNumber, tableNumber);
   AbstractFile::addXmlCdataElement(xmlDoc, linkElement, tagTableSubHeaderNumber, tableSubHeaderNumber);
   AbstractFile::addXmlCdataElement(xmlDoc, linkElement, tagFigureNumber, figureNumber);
   AbstractFile::addXmlCdataElement(xmlDoc, linkElement, tagPanelNumberOrLetter, panelNumberOrLetter);
   AbstractFile::addXmlCdataElement(xmlDoc, linkElement, tagPageNumber, pageNumber);
   AbstractFile::addXmlCdataElement(xmlDoc, linkElement, tagPageReferencePageNumber, pageReferencePageNumber);
   AbstractFile::addXmlCdataElement(xmlDoc, linkElement, tagPageReferenceSubHeaderNumber, pageReferenceSubHeaderNumber);
   
   //
   // Add to parent
   //
   parentElement.appendChild(linkElement);
}

/**
 * get the entire link in an "coded" text form.
 */
QString 
StudyMetaDataLink::getLinkAsCodedText() const
{
   //
   // Assemble into one string containing key/value pairs separated by a semi-colon
   //
   QStringList sl;
   sl << ("pubMedID=" + pubMedID)
      << ("tableNumber=" + tableNumber)
      << ("tableSubHeaderNumber=" + tableSubHeaderNumber)
      << ("figureNumber=" + figureNumber)
      << ("panelNumberOrLetter=" + panelNumberOrLetter)
      << ("pageNumber=" + pageNumber)
      << ("pageReferencePageNumber=" + pageReferencePageNumber)
      << ("pageReferenceSubHeaderNumber=" + pageReferenceSubHeaderNumber);

   const QString s = sl.join(";");

   return s;
}

/**
 * set the link from "coded" text form.
 */
void 
StudyMetaDataLink::setLinkFromCodedText(const QString& txt)
{
   //
   // Clear this link
   //
   clear();
   
   //
   // Extract the key/value pairs that are separated by a semi-colon
   //
   const QStringList sl = txt.split(";", QString::SkipEmptyParts);
   for (int i = 0; i < sl.size(); i++) {
      const QString keyValueString = sl.at(i);
      
      //
      // Split with "=" into key/value pairs
      //
      const QStringList keyValueList = keyValueString.split("=", QString::SkipEmptyParts);
      if (keyValueList.size() == 2) {
         const QString key = keyValueList.at(0);
         const QString value = keyValueList.at(1);
         
         if (key == "pubMedID") {
            setPubMedID(value);
         }
         else if (key == "tableNumber") {
            setTableNumber(value);
         }
         else if (key == "tableSubHeaderNumber") {
            setTableSubHeaderNumber(value);
         }
         else if (key == "figureNumber") {
            setFigureNumber(value);
         }
         else if (key == "panelNumberOrLetter") {
            setFigurePanelNumberOrLetter(value);
         }
         else if (key == "pageNumber") {
            setPageNumber(value);
         }
         else if (key == "pageReferencePageNumber") {
            setPageReferencePageNumber(value);
         }
         else if (key == "pageReferenceSubHeaderNumber") {
            setPageReferenceSubHeaderNumber(value);
         }
         else {
            std::cout << "Unrecognized StudyMetaDataLink key: " 
                      << key.toAscii().constData() << std::endl;
         }
      }
   }
}
      

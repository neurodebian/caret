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
#include <QDomText>

#include "AbstractFile.h"
#define __CELL_STUDY_INFO_MAIN__
#include "CellStudyInfo.h"
#undef __CELL_STUDY_INFO_MAIN__
#include "StringTable.h"

/**
 * Constructor
 */
CellStudyInfo::CellStudyInfo()
{
   clear();
}

/**
 * Destructor
 */
CellStudyInfo::~CellStudyInfo()
{
}

/**
 * equality operator.
 */
bool 
CellStudyInfo::operator==(const CellStudyInfo& cci) const
{
   return ((url      == cci.url) &&
           (keywords == cci.keywords) &&
           (title    == cci.title) &&
           (authors  == cci.authors) &&
           (citation == cci.citation) &&
           (comment  == cci.comment) &&
           (stereotaxicSpace == cci.stereotaxicSpace) &&
           (partitioningSchemeAbbreviation == cci.partitioningSchemeAbbreviation) &&
           (partitioningSchemeFullName == cci.partitioningSchemeFullName));
}

/**
 * clear the study info.
 */
void 
CellStudyInfo::clear()
{
   url = "";
   keywords = "";
   title = "";
   authors = "";
   citation = "";
   comment = "";
   stereotaxicSpace = "";
   partitioningSchemeAbbreviation = "";
   partitioningSchemeFullName = "";
}

/**
 * set element from text (used by SAX XML parser).
 */
void 
CellStudyInfo::setElementFromText(const QString& elementName,
                                  const QString& textValue)
{
   if (elementName == tagUrl) {
      url = textValue;
   }
   else if (elementName == tagKeywords) {
      keywords = textValue;
   }
   else if (elementName == tagTitle) {
      title = textValue;
   }
   else if (elementName == tagAuthors) {
      authors = textValue;
   }
   else if (elementName == tagCitation) {
      citation = textValue;
   }
   else if (elementName == tagStereotaxicSpace) {
      stereotaxicSpace = textValue;
   }
   else if (elementName == tagComment) {
      comment = textValue;
   }
   else if (elementName == tagStudyNumber) {
      // ignore studyNumber
   }
   else if (elementName == tagPartitioningSchemeAbbreviation) {
      partitioningSchemeAbbreviation = textValue;
   }
   else if (elementName == tagPartitioningSchemeFullName) {
      partitioningSchemeFullName = textValue;
   }
   else {
      std::cout << "WARNING: unrecognized CellStudyInfo element: "
                << elementName.toAscii().constData()
                << std::endl;
   }
}
                              
/**
 * called to read from an XML structure.
 */
void 
CellStudyInfo::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != tagCellStudyInfo) {
      QString msg("Incorrect element type passed to CellStudyInfo::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == tagUrl) {
            url = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagKeywords) {
            keywords = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagTitle) {
            title = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagAuthors) {
            authors = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagCitation) {
            citation = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagStereotaxicSpace) {
            stereotaxicSpace = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagComment) {
            comment = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagStudyNumber) {
            // ignore studyNumber
         }
         else if (elem.tagName() == tagPartitioningSchemeAbbreviation) {
            partitioningSchemeAbbreviation = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagPartitioningSchemeFullName) {
            partitioningSchemeFullName = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else {
            std::cout << "WARNING: unrecognized CellStudyInfo element: "
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
CellStudyInfo::writeXML(QDomDocument& xmlDoc,
                   QDomElement&  parentElement,
                   const int studyNumber)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement cellStudyElement = xmlDoc.createElement(tagCellStudyInfo);

   //
   // study number
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellStudyElement, tagStudyNumber, studyNumber);
   
   //
   // url
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellStudyElement, tagUrl, url);
   
   //
   // keywords
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellStudyElement, tagKeywords, keywords);
   
   //
   // title
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellStudyElement, tagTitle, title);
   
   //
   // authors
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellStudyElement, tagAuthors, authors);
   
   //
   // citation
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellStudyElement, tagCitation, citation);
   
   //
   // stereotaxicSpace
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellStudyElement, tagStereotaxicSpace, stereotaxicSpace);
   
   //
   // comment
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellStudyElement, tagComment, comment);
   
   //
   // partitioning scheme abbreviation
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellStudyElement, tagPartitioningSchemeAbbreviation, partitioningSchemeAbbreviation);
   
   //
   // partitioning scheme full name
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellStudyElement, tagPartitioningSchemeFullName, partitioningSchemeFullName);
   
   //
   // Add to parent
   //
   parentElement.appendChild(cellStudyElement);
}

/**
 * write the data into a StringTable.
 */
void 
CellStudyInfo::writeDataIntoStringTable(const std::vector<CellStudyInfo>& studyInfo,
                                         StringTable& table)
{
   table.clear();
   
   const int numInfo = static_cast<int>(studyInfo.size());
   if (numInfo <= 0) {
      return;
   }
   
   //
   // Column numbers for info
   //
   int numCols = 0;
   const int studyNumberCol = numCols++;
   const int spaceCol = numCols++;
   const int keywordsCol = numCols++;
   const int urlCol = numCols++;
   const int titleCol = numCols++;
   const int authorsCol = numCols++;
   const int citationCol = numCols++;
   const int commentCol = numCols++;
   const int partSchemeAbbrevCol = numCols++;
   const int partSchemeFullNameCol = numCols++;
   
   //
   // Load the table
   //
   table.setNumberOfRowsAndColumns(numInfo, numCols, "Cell Study Info");
   table.setColumnTitle(studyNumberCol, "Study Number");
   table.setColumnTitle(urlCol, "URL");
   table.setColumnTitle(keywordsCol, "Keywords");
   table.setColumnTitle(titleCol, "Title");
   table.setColumnTitle(authorsCol, "Authors");
   table.setColumnTitle(citationCol, "Citation");
   table.setColumnTitle(spaceCol, "Stereotaxic Space");
   table.setColumnTitle(commentCol, "Comment");
   table.setColumnTitle(partSchemeAbbrevCol, "Partitioning Scheme Abbreviation");
   table.setColumnTitle(partSchemeFullNameCol, "Partitioning Scheme Full Name");
   
   for (int i = 0; i < numInfo; i++) {
      const CellStudyInfo& csi = studyInfo[i];
      table.setElement(i, studyNumberCol, i);
      table.setElement(i, urlCol, csi.getURL());
      table.setElement(i, keywordsCol, csi.getKeywords());
      table.setElement(i, titleCol, csi.getTitle());
      table.setElement(i, authorsCol, csi.getAuthors());
      table.setElement(i, citationCol, csi.getCitation());
      table.setElement(i, spaceCol, csi.getStereotaxicSpace());
      table.setElement(i, partSchemeAbbrevCol, csi.getPartitioningSchemeAbbreviation());
      table.setElement(i, partSchemeFullNameCol, csi.getPartitioningSchemeFullName());
      table.setElement(i, commentCol, csi.getComment());
   }
}

/**
 * read the data from a StringTable.
 */
void 
CellStudyInfo::readDataFromStringTable(std::vector<CellStudyInfo>& studyInfo,
                                        const StringTable& table) throw (FileException)
{
   if (table.getTableTitle() != "Cell Study Info") {
      throw FileException("String table for CellStudyInfo does not have name Cell Study Info");
   }
   
   studyInfo.clear();

   int studyNumberCol = -1;
   int urlCol = -1;
   int keywordsCol = -1;
   int titleCol = -1;
   int authorsCol = -1;
   int citationCol = -1;
   int spaceCol = -1;
   int commentCol = -1;
   int partitioningSchemeAbbrevCol = -1;
   int partitioningSchemeFullNameCol = -1;
   
   const int numCols = table.getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString name = table.getColumnTitle(i).toLower();
      if (name == "study number") {
         studyNumberCol = i;
      }
      else if (name == "url") {
         urlCol = i;
      }
      else if (name == "keywords") {
         keywordsCol = i;
      }
      else if (name == "title") {
         titleCol = i;
      }
      else if (name == "authors") {
         authorsCol = i;
      }
      else if (name == "citation") {
         citationCol = i;
      }
      else if (name == "stereotaxic space") {
         spaceCol = i;
      }
      else if (name == "comment") {
         commentCol = i;
      }
      else if (name == "partitioning scheme abbreviation") {
         partitioningSchemeAbbrevCol = i;
      }
      else if (name == "partitioning scheme full name") {
         partitioningSchemeFullNameCol = i;
      }
   }
   
   const int numItems = table.getNumberOfRows();
   for (int i = 0; i < numItems; i++) {
      CellStudyInfo csi;
      
      if (urlCol >= 0) {
        csi.setURL(table.getElement(i, urlCol));
      }
      if (keywordsCol >= 0) {
        csi.setKeywords(table.getElement(i, keywordsCol));
      }
      if (titleCol >= 0) {
        csi.setTitle(table.getElement(i, titleCol));
      }
      if (authorsCol >= 0) {
        csi.setAuthors(table.getElement(i, authorsCol));
      }
      if (citationCol >= 0) {
        csi.setCitation(table.getElement(i, citationCol));
      }
      if (spaceCol >= 0) {
        csi.setStereotaxicSpace(table.getElement(i, spaceCol));
      }
      if (commentCol >= 0) {
        csi.setComment(table.getElement(i, commentCol));
      }
      if (partitioningSchemeAbbrevCol >= 0) {
        csi.setPartitioningSchemeAbbreviation(table.getElement(i, partitioningSchemeAbbrevCol));
      }
      if (partitioningSchemeFullNameCol >= 0) {
        csi.setPartitioningSchemeFullName(table.getElement(i, partitioningSchemeFullNameCol));
      }
      
      studyInfo.push_back(csi);
   }
}      

/**
 * get full description of all fields for display to user.
 */
QString 
CellStudyInfo::getFullDescriptionForDisplayToUser(const bool useHTML) const
{
   const QString boldStart("<B>");
   const QString boldEnd("</B>");
   QString newLine("\n");
   if (useHTML) {
      newLine = "<BR>";
   }
   
   QString s;
   if (useHTML) s += boldStart;
   s += "Study Title";
   if (useHTML) s += boldEnd;
   s += ": ";
   s += title;
   s += newLine;
   
   if (authors.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Study Authors";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += authors;
      s += newLine;
   } 
   
   if (citation.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Study Citation";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += citation;
      s += newLine;
   } 
   
   if (keywords.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Keywords";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += keywords;
      s += newLine;
   } 
   
   if (stereotaxicSpace.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Study Stereotaxic Space";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += stereotaxicSpace;
      s += newLine;
   } 
   
   if (url.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Study URL";
      if (useHTML) s += boldEnd;
      s += ": ";
      //if (useHTML) {
      //   s += "<A HREF=\"";
      //   s += url;
      //   s += "\">";
      //}
      s += url;
      //if (useHTML) {
      //   s += "</A>";
      //}
      s += newLine;
   } 
   
   if (partitioningSchemeAbbreviation.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Study Partitioning Scheme Abbreviation";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += partitioningSchemeAbbreviation;
      s += newLine;
   } 
   
   if (partitioningSchemeFullName.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Study Partitioning Scheme Full Name";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += partitioningSchemeFullName;
      s += newLine;
   } 
   
   if (comment.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Study Comment";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += comment;
      s += newLine;
   } 
   
   return s;
}            


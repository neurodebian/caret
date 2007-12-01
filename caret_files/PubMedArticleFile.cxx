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

#include "DebugControl.h"
#include "HttpFileDownload.h"
#include "PubMedArticleFile.h"
#include "TextFile.h"

/**
 * constructor.
 */
PubMedArticleFile::PubMedArticleFile()
   : AbstractFile("PubMed Article File",
                  ".xml",
                  false,
                  FILE_FORMAT_XML,   // default format
                  FILE_IO_NONE,      // read/write ascii
                  FILE_IO_NONE,      // read/write binary
                  FILE_IO_NONE,      // read/write xml
                  FILE_IO_NONE,      // read/write xml base64
                  FILE_IO_NONE,      // read/write xml gzip
                  FILE_IO_NONE,      // read/write other
                  FILE_IO_NONE)      // read/write csv
{
   clear();
}

/**
 * destructor.
 */
PubMedArticleFile::~PubMedArticleFile()
{
   clear();
}

/**
 * clear the file.
 */
void 
PubMedArticleFile::clear()
{
   articleTitle = "";
   authors = "";
   journalPages = "";
   journalTitle = "";
   journalVolume = "";
   journalYear = "";
   medicalSubjectHeadings = "";
   documentObjectIdentifier = "";
   abstractText = "";
   pubMedID = "";
}

/**
 * retrieve the article information for the specified PubMed ID.
 */
void 
PubMedArticleFile::retrieveArticleWithPubMedID(const QString& pubMedIDIn) throw (FileException)
{
   clear();
   pubMedID = pubMedIDIn.trimmed();
   if (pubMedID.isEmpty()) {
      throw FileException("PubMed ID is empty.");
   }

   //
   // Construct the URL
   //
   const QString pubMedURL = ("http://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=pubmed&id="
                              + pubMedID
                              + "&retmode=xml&rettype=citation");
   if (DebugControl::getDebugOn()) {
      std::cout << "URL to get PubMed Info: "
                << pubMedURL.toAscii().constData()
                << std::endl;
   }
                              
   //
   // Download the file
   //
   const int timeOutInSeconds = 30;
   HttpFileDownload http(pubMedURL, 
                         timeOutInSeconds);
   http.download();
   if (http.getDownloadSuccessful()) {
      QString pubMedString;
      http.getContentUTF8(pubMedString);
      if (DebugControl::getDebugOn()) {
         std::cout << "Retrieved from PubMed: "
                   << std::endl
                   << "Response Code: " 
                   << http.getResponseCode()
                   << std::endl
                   << pubMedString.toAscii().constData()
                   << std::endl
                   << std::endl;
      }
      
      parseXML(pubMedString);
   }
   else {
      throw FileException("HTTP Error: " + http.getErrorMessage());
   }
   
   if (journalYear.isEmpty() == false) {
      journalTitle += (" "
                       + journalYear
                       + ";");
   }
   if (journalVolume.isEmpty() == false) {
      journalTitle += (" "
                       + journalVolume
                       + ":");
   }
   if (journalPages.isEmpty() == false) {
      journalTitle += journalPages;
   }
}

/**
 * parse the xml.
 */
void 
PubMedArticleFile::parseXML(const QString xmlString) throw (FileException)
{
   //
   // Use the DOM parser and set its content
   //
   QDomDocument dom("PubMed-File"); 
   QString domErrorMessage;
   int domErrorLine, domErrorColumn;
   if (dom.setContent(xmlString, 
                      &domErrorMessage, 
                      &domErrorLine,
                      &domErrorColumn) == false) {
      //
      // Creation of DOM structure failed
      //
      QString msg = ("Error parsing document at line "
                     + QString::number(domErrorLine)
                     + ", column "
                     + QString::number(domErrorColumn));
      if (DebugControl::getDebugOn()) {
         TextFile tf;
         tf.setText(xmlString);
         try {
            const QString textFileName("pubmed.txt");
            tf.writeFile(textFileName);
            msg += "\nDownloaded content is in " + textFileName + ".";
         }
         catch (FileException&) {
         }
      }
      throw FileException(msg);
   }
   
   //
   // Get the root element 
   //
   QDomElement rootElement = dom.documentElement();
   if (rootElement.tagName().toLower() == "error") {
      const QString msg = ("ERROR retrienving data.  Is PubMed ID "
                           + pubMedID
                           + " valid?\n");
      throw FileException(msg);
   }
   else if (rootElement.tagName() != "PubmedArticleSet") {
      const QString msg("Root element is "
                        + rootElement.tagName()
                        + " but should be PubmedArticleSet");
      throw FileException(msg);
   }
   QDomNode node = rootElement.firstChild();
   
   if (node.hasChildNodes() == false) {
      const QString msg = "ERROR retrienving data (root element has no children).\n"
                          "Is PubMed ID " + pubMedID + " valid?\n";
      throw FileException(msg);
   }
   
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Element is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         if (elem.tagName() == "PubmedArticle") {
            processPubMedArticleChildren(node.firstChild());
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * process the PubMedArticle child.
 */
void 
PubMedArticleFile::processPubMedArticleChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "PubMedArticle child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }          
         if (elem.tagName() == "MedlineCitation") {
            processMedlineCitationChildren(node.firstChild());
         }
         else if (elem.tagName() == "PubmedData") {
            processPubmedDataChildren(node.firstChild());
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * process the mesh heading list children.
 * For more info: http://www.nlm.nih.gov/bsd/licensee/elements_descriptions.html#meshheadinglist
 */
void 
PubMedArticleFile::processMeshHeadingListChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "MeshHeadingList child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }          
         if (elem.tagName() == "MeshHeading") {
            processMeshHeadingChildren(node.firstChild());
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}

/**
 * process the mesh heading children.
 * For more info: http://www.nlm.nih.gov/bsd/licensee/elements_descriptions.html#meshheadinglist
 */
void 
PubMedArticleFile::processMeshHeadingChildren(QDomNode node) throw (FileException)
{
   QString descriptor;
   QString qualifier;
   bool descriptorValid = false;
   bool qualifierValid = false;
   
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "MeshHeading child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }          
         if (elem.tagName() == "DescriptorName") {
            const QString majorTopicYN = elem.attribute("MajorTopicYN");
            descriptor = elem.text();
            if (majorTopicYN == "Y") {
               descriptorValid = true;
            }
         }
         else if (elem.tagName() == "QualifierName") {
            const QString majorTopicYN = elem.attribute("MajorTopicYN");
            if (majorTopicYN == "Y") {
               if (qualifier.isEmpty() == false) {
                  qualifier += ", ";
               }
               qualifier += elem.text();
               qualifierValid = true;
            }
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
   
   if (descriptor.isEmpty() == false) {
      if (qualifierValid) {
         if (medicalSubjectHeadings.isEmpty() == false) {
            medicalSubjectHeadings += "; ";
         }
         medicalSubjectHeadings += (descriptor
                                    + ": "
                                    + qualifier);
      }
      else if (descriptorValid) {
         if (medicalSubjectHeadings.isEmpty() == false) {
            medicalSubjectHeadings += "; ";
         }
         medicalSubjectHeadings += descriptor;
      }
   }
}
      
/**
 * process the MedlineCitation children.
 */
void 
PubMedArticleFile::processMedlineCitationChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "MedlineCitation child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         if (elem.tagName() == "Article") {
            processArticleChildren(node.firstChild());
         }
         else if (elem.tagName() == "MeshHeadingList") {
            processMeshHeadingListChildren(node.firstChild());
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}

/**
 * process the Article children.
 */
void 
PubMedArticleFile::processArticleChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Article child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         
         if (elem.tagName() == "Journal") {
            processJournalChildren(node.firstChild());
         }
         if (elem.tagName() == "Abstract") {
            abstractText = elem.text();
         }
         if (elem.tagName() == "ArticleTitle") {
            articleTitle = elem.text();
         }
         if (elem.tagName() == "AuthorList") {
            processAuthorListChildren(node.firstChild());
         }
         if (elem.tagName() == "Pagination") {
            processPaginationChildren(node.firstChild());
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * process the Pagination children.
 */
void 
PubMedArticleFile::processPaginationChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Article child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         
         if (elem.tagName() == "MedlinePgn") {
            journalPages = elem.text();
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * process the Journal children.
 */
void 
PubMedArticleFile::processJournalChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Journal child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         
         if (elem.tagName() == "JournalIssue") {
            processJournalIssueChildren(node.firstChild());
         }
         
         if (elem.tagName() == "Title") {
            journalTitle = elem.text();
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * process the Journal Issue children.
 */
void 
PubMedArticleFile::processJournalIssueChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Journal Issue child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         
         if (elem.tagName() == "PubDate") {
            processJournalPubDateChildren(node.firstChild());
         }
         
         if (elem.tagName() == "Volume") {
            journalVolume = elem.text();
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * process the Journal Pub Date children.
 */
void 
PubMedArticleFile::processJournalPubDateChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Journal Issue child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         
         if (elem.tagName() == "Year") {
            journalYear = elem.text();
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * process the AuthorList children.
 */
void 
PubMedArticleFile::processAuthorListChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "AuthorList child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         
         //
         // Get author names
         //
         if (elem.tagName() == "Author") {
            QString authorName;
            QDomNode authorNode = node.firstChild();
            while (authorNode.isNull() == false) {
               QDomElement authorElement = authorNode.toElement();
               if (authorElement.isNull() == false) {
                  if (authorElement.tagName() == "LastName") {
                     authorName = authorElement.text();
                  }
                  else if (authorElement.tagName() == "Initials") {
                     authorName += (" "
                                    + authorElement.text());
                  }
               }
               authorNode = authorNode.nextSibling();
            }
               
            if (authorName.isEmpty() == false) {
               if (authors.isEmpty() == false) {
                  authors += ", ";
               }
               authors += authorName;
            }
               
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * process the PubmedData children.
 */
void 
PubMedArticleFile::processPubmedDataChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "PubmedData child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         
         if (elem.tagName() == "ArticleIdList") {
            processArticleIdListChildren(node.firstChild());
         }
      }
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * process the ArticleIdList children.
 */
void 
PubMedArticleFile::processArticleIdListChildren(QDomNode node) throw (FileException)
{
   //
   // Loop through children
   //
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "ArticleIdList child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         
         if (elem.tagName() == "ArticleId") {
            const QString idTypeValue = elem.attribute("IdType");
            if (idTypeValue == "doi") {
               documentObjectIdentifier = elem.text();
            }
         }
      }
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
PubMedArticleFile::empty() const 
{
   bool isEmpty = (articleTitle.isEmpty() &&
                   authors.isEmpty() &&
                   journalTitle.isEmpty() &&
                   documentObjectIdentifier.isEmpty() &&
                   abstractText.isEmpty() &&
                   pubMedID.isEmpty());
   
   return isEmpty;
}
      
/**
 * read the file.
 */
void 
PubMedArticleFile::readFileData(QFile& /* file */, 
                                QTextStream& /* stream */, 
                                QDataStream& /* binStream */,     
                                QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException("Reading of PubMedArticleFile not supported.\n"
                       "Use the retrieveArticleWithPubMedID() method.");
}

/**
 * write the file.
 */
void 
PubMedArticleFile::writeFileData(QTextStream& /* stream */, 
                                 QDataStream& /* binStream */,
                                 QDomDocument& /* xmlDoc */,
                                 QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException("Writing of PubMedArticleFile not supported.");
}


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

#include <QDomElement>
#include <QDomNode>

#include "CommaSeparatedValueFile.h"
#include "SpecFile.h"
#include "StringTable.h"
#include "StudyCollectionFile.h"

/**
 * constructor.
 */
StudyCollectionFile::StudyCollectionFile()
   : AbstractFile("Study Collection File",
                  SpecFile::getStudyCollectionFileExtension(),
                  true,           // has header
                  FILE_FORMAT_XML,    // default file format
                  FILE_IO_NONE,   // ascii
                  FILE_IO_NONE,   // binary
                  FILE_IO_READ_AND_WRITE,   // xml
                  FILE_IO_NONE,   // xml base64
                  FILE_IO_NONE,   // xml base64 gzip
                  FILE_IO_NONE,   // other
                  FILE_IO_READ_ONLY)   // CSV
                  
{
   clear();
}

/**
 * copy constructor.
 */
StudyCollectionFile::StudyCollectionFile(const StudyCollectionFile& scf)
   : AbstractFile(scf)
{
   copyHelper(scf);
}

/**
 * destructor.
 */
StudyCollectionFile::~StudyCollectionFile()
{
   clear();
}

/**
 * copy constructor.
 */
StudyCollectionFile& 
StudyCollectionFile::operator=(const StudyCollectionFile& scf)
{
   if (this != &scf) {
      copyHelper(scf);
   }
   setModified();
   
   return *this;
}

/**
 * the copy helper.
 */
void 
StudyCollectionFile::copyHelper(const StudyCollectionFile& scf)
{
   clear();
   
   const int num = static_cast<int>(studyCollections.size());
   for (int i = 0; i < num; i++) {
      StudyCollection* sc = new StudyCollection(*scf.getStudyCollection(i));
      addStudyCollection(sc);
   }
   
   setModified();
}

/**
 * append a study collection file to "this" study collection file.
 */
void 
StudyCollectionFile::append(const StudyCollectionFile& scf)
{
   const int num = scf.getNumberOfStudyCollections();
   for (int i = 0; i < num; i++) {
      addStudyCollection(new StudyCollection(*scf.getStudyCollection(i)));
   }
}

/**
 * add a study collection.
 */
void 
StudyCollectionFile::addStudyCollection(StudyCollection* sc)
{
   studyCollections.push_back(sc);
   studyCollections[studyCollections.size() - 1]->setParentStudyCollectionFile(this);
}
      
/**
 * get a study collection.
 */
StudyCollection* 
StudyCollectionFile::getStudyCollection(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfStudyCollections())) {
      return studyCollections[indx];
   }
   
   return NULL;
}

/**
 * get a study collection (const method).
 */
const StudyCollection* 
StudyCollectionFile::getStudyCollection(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfStudyCollections())) {
      return studyCollections[indx];
   }
   
   return NULL;
}

/**
 * delete a study collection.
 */
void 
StudyCollectionFile::deleteStudyCollection(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfStudyCollections())) {
      delete studyCollections[indx];
      studyCollections[indx] = NULL;;
      studyCollections.erase(studyCollections.begin() + indx);
   }
}
      
/**
 * call AbstractFile::clearAbstractFile() from its clear method.
 */
void 
StudyCollectionFile::clear()
{
   clearAbstractFile();
   const int num = getNumberOfStudyCollections();
   for (int i = 0; i < num; i++) {
      delete studyCollections[i];
      studyCollections[i] = NULL;
   }
   studyCollections.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
StudyCollectionFile::empty() const
{
   return studyCollections.empty();
}

/**
 * find out if comma separated file conversion supported.
 */
void 
StudyCollectionFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                                    bool& writeToCSV) const
{
   readFromCSV = true;
   writeToCSV  = false;
}

/**
 * read the file's data from a comma separated values file (throws exception if not supported).
 */
void 
StudyCollectionFile::readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException)
{
   const QString filenameSaved(filename);
   clear();
   filename = filenameSaved;
   
   // 
   // Do header
   //    
   const StringTable* stHeader = csv.getDataSectionByName("header");
   if (stHeader != NULL) {
      readHeaderDataFromStringTable(*stHeader);
   }  

   const StringTable* st = csv.getDataSectionByName("StudyCollectionMetadata");   
   if (st == NULL) {
      throw FileException("No section titled \"StudyCollectionMetadata\" was found.");
   }
   
   const int numRows = st->getNumberOfRows();
   if (numRows != 1) {
      throw FileException("StudyCollectionMetadata section must contain one and only one row of data."); 
   }
   int numCols = st->getNumberOfColumns();
   
   int studyCollectionNameCol = -1;
   int studyCollectionCreatorCol = -1;
   int typeCol = -1;
   int commentCol = -1;
   int studyNameCol = -1;
   int pmidCol = -1;
   int searchIDCol = -1;
   int fociListIDCol = -1;
   int fociColorListIDCol = -1;
   int sclIDCol = -1;
   int topicCol = -1;
   int categoryIDCol = -1;
   
   for (int i = 0; i < numCols; i++) {
      const QString columnTitle = st->getColumnTitle(i).toLower();
      if (columnTitle == "study collection name") {
         studyCollectionNameCol = i;
      }
      else if (columnTitle == "study collection creator") {
         studyCollectionCreatorCol = i;   
      }
      else if (columnTitle == "type") {
         typeCol = i;   
      }
      else if (columnTitle == "comment") {
         commentCol = i;   
      }
      else if (columnTitle == "study name") {
         studyNameCol = i;   
      }
      else if (columnTitle == "pmid") {
         pmidCol = i;
      }
      else if (columnTitle == "focilist_id") {
         fociListIDCol = i;
      }
      else if (columnTitle == "focicolorlist_id") {
         fociColorListIDCol = i;
      }
      else if (columnTitle == "scl_id") {
         sclIDCol = i;
      }
      else if (columnTitle == "search_id") {
         searchIDCol = i;
      }
      else if (columnTitle == "topic") {
         topicCol = i;
      }
      else if (columnTitle == "category_id") {
         categoryIDCol = i;
      }
   }
   
   StudyCollection* sc = new StudyCollection;;
   
   for (int i = 0; i < numRows; i++) {
      if (studyCollectionNameCol >= 0) {
         sc->setStudyCollectionName(st->getElement(i, studyCollectionNameCol));
      }
      if (studyCollectionCreatorCol >= 0) {
         sc->setStudyCollectionCreator(st->getElement(i, studyCollectionCreatorCol));
      }
      if (typeCol >= 0) {
         sc->setStudyType(st->getElement(i, typeCol));
      }
      if (commentCol >= 0) {
         sc->setComment(st->getElement(i, commentCol));
      }
      if (studyNameCol >= 0) {
         sc->setStudyName(st->getElement(i, studyNameCol));
      }
      if (pmidCol >= 0) {
         sc->setPMID(st->getElement(i, pmidCol));
      }
      if (fociListIDCol >= 0) {
         sc->setFociListID(st->getElement(i, fociListIDCol));
      }
      if (fociColorListIDCol >= 0) {
         sc->setFociColorListID(st->getElement(i, fociColorListIDCol));
      }
      if (sclIDCol >= 0) {
         sc->setStudyCollectionID(st->getElement(i, sclIDCol));
      }
      if (searchIDCol >= 0) {
         sc->setSearchID(st->getElement(i, searchIDCol));
      }
      if (topicCol >= 0) {
         sc->setTopic(st->getElement(i, topicCol));
      }
      if (categoryIDCol >= 0) {
         sc->setCategoryID(st->getElement(i, categoryIDCol));
      }
   }
   
   
   //
   // Read Study PMIDs
   //
   const StringTable* pmidTable = csv.getDataSectionByName("Name_PMID");   
   int nameColumn = -1;
   int pmidColumn = -1;
   int mslidColumn = -1;
   
   const int numPMIDCols = pmidTable->getNumberOfColumns();
   for (int i = 0; i < numPMIDCols; i++) {
      const QString name = pmidTable->getColumnTitle(i).toLower();
      if (name == "name") {
         nameColumn = i;
      }
      else if (name == "study pubmed id") {
         pmidColumn = i;
      }
      else if (name == "msl_id") {
         mslidColumn = i;
      }
   }

   const int numItems = pmidTable->getNumberOfRows();
   for (int i = 0; i < numItems; i++) {
      QString name, pmid, msl;
      
      if (nameColumn >= 0) {
        name = pmidTable->getElement(i, nameColumn);
      }
      if (pmidColumn >= 0) {
         pmid = pmidTable->getElement(i, pmidColumn);
      }
      if (mslidColumn >= 0) {
         msl = pmidTable->getElement(i, mslidColumn);
      }

      if (name.isEmpty() == false) {
         sc->addStudyPMID(new StudyNamePubMedID(name, pmid, msl));
      }
   }
   
   addStudyCollection(sc);
}

/**
 * read from XML.
 */
void 
StudyCollectionFile::readXML(QDomNode& nodeIn) throw (FileException)
{
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         //
         // Is this a header element
         //
         if ((elem.tagName() == xmlHeaderOldTagName) ||
                  (elem.tagName() == xmlHeaderTagName)) {
            // ignore, read by AbstractFile::readFile()
         }
         else if (elem.tagName() == "StudyCollection") {
            StudyCollection* sc = new StudyCollection;
            sc->readXML(node);
            addStudyCollection(sc);
         }
         else {
            std::cout << "WARNING: unrecognized Study Collection File Element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
      }
      
      node = node.nextSibling();
   }
}

/**
 * Read the contents of the file (header has already been read).
 */
void 
StudyCollectionFile::readFileData(QFile& file,
                                    QTextStream& stream,
                                    QDataStream& /*binStream*/,
                                    QDomElement& rootElement) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) { 
      return;
   }
 
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Reading in ASCII format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         readXML(rootElement);
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Reading XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            csvf.readFromTextStream(file, stream);
            readDataFromCommaSeparatedValuesTable(csvf);
         }
         break;
   }
}

/**
 * write to XML.
 */
void 
StudyCollectionFile::writeXML(QDomDocument& xmlDoc,
                              QDomElement& parentElement)
{
   const int num = getNumberOfStudyCollections();
   for (int i = 0; i < num; i++) {
      studyCollections[i]->writeXML(xmlDoc, parentElement);
   }
}

/**
 * write the file's memory in caret6 format to the specified name
 */
QString
StudyCollectionFile::writeFileInCaret6Format(const QString& filenameIn,
                                             Structure structure,
                                             const ColorFile* colorFileIn,
                                             const bool useCaret6ExtensionFlag) throw (FileException)
{
   int numCollections = this->getNumberOfStudyCollections();
   if (numCollections <= 0) {
      throw FileException("Contains no study collections.");
   }

   QFile file(filenameIn);
   if (AbstractFile::getOverwriteExistingFilesAllowed() == false) {
      if (file.exists()) {
         throw FileException("file exists and overwrite is prohibited.");
      }
   }
   if (file.open(QFile::WriteOnly) == false) {
      throw FileException("Unable to open for writing");
   }
   QTextStream stream(&file);

   XmlGenericWriter xmlWriter(stream);
   xmlWriter.writeStartDocument();

   XmlGenericWriterAttributes attributes;
   attributes.addAttribute("CaretFileType", "StudyCollection");
   attributes.addAttribute("xmlns:xsi",
                           "http://www.w3.org/2001/XMLSchema-instance");
   attributes.addAttribute("xsi:noNamespaceSchemaLocation",
                           "http://brainvis.wustl.edu/caret6/xml_schemas/StudyCollectionFileSchema.xsd");
   attributes.addAttribute("Version", "6.0");
   xmlWriter.writeStartElement("CaretDataFile", attributes);

   this->writeHeaderXMLWriter(xmlWriter);

   for (int i = 0; i < numCollections; i++) {
      const StudyCollection* sc = this->getStudyCollection(i);
      sc->writeXML(xmlWriter, i);
   }

   xmlWriter.writeEndElement();

   xmlWriter.writeEndDocument();

   file.close();

   return filenameIn;

}


/**
 * Write the file's data (header has already been written).
 */
void 
StudyCollectionFile::writeFileData(QTextStream& /*stream*/,
                                     QDataStream& /*binStream*/,
                                     QDomDocument& xmlDoc,
                                     QDomElement& rootElement) throw (FileException)
{
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Writing in ASCII format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Writing in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         writeXML(xmlDoc, rootElement);
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Writing XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Writing XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Writing in CSVF format not supported.");
         break;
   }
}


//
//==============================================================================
//

/**
 * constructor.
 */
StudyCollection::StudyCollection()
{
   clear();
}

/**
 * destructor.
 */
StudyCollection::~StudyCollection()
{
   clear();
}

/**
 * copy constructor.
 */
StudyCollection::StudyCollection(const StudyCollection& sc)
{
   parentStudyCollectionFile = NULL;
   copyHelper(sc);
}

/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
StudyCollection::copyHelper(const StudyCollection& sc)
{
   StudyCollectionFile* savedParentStudyCollectionFile = parentStudyCollectionFile;
   
   clear();
   
   studyCollectionName = sc.studyCollectionName;
   studyCollectionCreator = sc.studyCollectionCreator;
   studyType = sc.studyType;
   comment = sc.comment;
   studyName = sc.studyName;
   pmid = sc.pmid;
   searchID = sc.searchID;
   fociListID = sc.fociListID;
   fociColorListID = sc.fociColorListID;
   sclID = sc.sclID;
   topic = sc.topic;
   categoryID = sc.categoryID;
   
   const int num = sc.getNumberOfStudyPMIDs();
   for (int i = 0; i < num; i++) {
      StudyNamePubMedID* sid = new StudyNamePubMedID(*sc.getStudyPMID(i));
      addStudyPMID(sid);
   }
   
   parentStudyCollectionFile = savedParentStudyCollectionFile;
   
   setModified();
}

/**
 * assignment operator.
 */
StudyCollection& 
StudyCollection::operator=(const StudyCollection& sc)
{
   if (this != &sc) {
      copyHelper(sc);
   }
   return *this;
}
      
/**
 * clear the collection.
 */
void 
StudyCollection::clear()
{
   parentStudyCollectionFile = NULL;
   
   studyCollectionName = "";
   studyCollectionCreator = "";
   studyType = "";
   comment = "";
   studyName = "";
   pmid = "";
   searchID = "";
   topic = "";
   categoryID = "";
   fociListID = "";
   fociColorListID = "";
   sclID = "";
   
   const unsigned int num = studyPMIDs.size();
   for (unsigned int i = 0; i < num; i++) {
      delete studyPMIDs[i];
      studyPMIDs[i] = NULL;
   }
   studyPMIDs.clear();
   
   setModified();
}

/**
 * add a study PMID.
 */
void 
StudyCollection::addStudyPMID(StudyNamePubMedID* s)
{
   studyPMIDs.push_back(s);
   const int indx = getNumberOfStudyPMIDs() - 1;
   studyPMIDs[indx]->setParent(this);
   setModified();
}

/**
 * get a study PMID.
 */
StudyNamePubMedID* 
StudyCollection::getStudyPMID(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfStudyPMIDs())) {
      return studyPMIDs[indx];
   }
   return NULL;
}

/**
 * get a study PMID.
 */
const StudyNamePubMedID* 
StudyCollection::getStudyPMID(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfStudyPMIDs())) {
      return studyPMIDs[indx];
   }
   return NULL;
}

/**
 * remove a study PMID.
 */
void 
StudyCollection::removeStudyPMID(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfStudyPMIDs())) {
      delete studyPMIDs[indx];
      studyPMIDs[indx] = NULL;
      studyPMIDs.erase(studyPMIDs.begin() + indx);
   }
   setModified();
}
      
/**
 * set study collection name.
 */
void 
StudyCollection::setStudyCollectionName(const QString& s)
{
   if (studyCollectionName != s) {
      studyCollectionName = s;
      setModified();
   }
}

/**
 * set study collection creator.
 */
void 
StudyCollection::setStudyCollectionCreator(const QString& s)
{
   if (studyCollectionCreator != s) {
      studyCollectionCreator = s;
      setModified();
   }
}

/**
 * set study type.
 */
void 
StudyCollection::setStudyType(const QString& s)
{
   if (studyType != s) {
      studyType = s;
      setModified();
   }
}

/**
 * set comment.
 */
void 
StudyCollection::setComment(const QString& s)
{
   if (comment != s) {
      comment = s;
      setModified();
   }
}

/**
 * set study name.
 */
void 
StudyCollection::setStudyName(const QString& s)
{
   if (studyName != s) {
      studyName = s;
      setModified();
   }
}

/**
 * set PMID.
 */
void 
StudyCollection::setPMID(const QString& s)
{
   if (pmid != s) {
      pmid = s;
      setModified();
   }
}

/**
 * set search ID.
 */
void 
StudyCollection::setSearchID(const QString& s)
{
   if (searchID != s) {
      searchID = s;
      setModified();
   }
}
      
/**
 * set topic.
 */
void 
StudyCollection::setTopic(const QString& s)
{
   if (topic != s) {
      topic = s;
      setModified();
   }
}

/**
 * set category id.
 */
void 
StudyCollection::setCategoryID(const QString& s)
{
   if (categoryID != s) {
      categoryID = s;
      setModified();
   }
}

/**
 * set foci list ID.
 */
void 
StudyCollection::setFociListID(const QString& s) 
{ 
   if (fociListID != s) {
      fociListID = s; 
      setModified();
   }
}
      
/**
 * set foci color list ID.
 */
void 
StudyCollection::setFociColorListID(const QString& s)
{
   if (fociColorListID != s) {
      fociColorListID = s;
      setModified();
   }
}
      
/**
 * set study collection ID.
 */
void 
StudyCollection::setStudyCollectionID(const QString& s)
{
   if (sclID != s) {
      sclID = s;
      setModified();
   }
}
      
/**
 * set parent study collection file.
 */
void 
StudyCollection::setParentStudyCollectionFile(StudyCollectionFile* parent)
{
   parentStudyCollectionFile = parent;
   const int num = getNumberOfStudyPMIDs();
   for (int i = 0; i < num; i++) {
      getStudyPMID(i)->setParent(this);
   }
}

/**
 * set modified.
 */
void 
StudyCollection::setModified()
{
   if (parentStudyCollectionFile != NULL) {
      parentStudyCollectionFile->setModified();
   }
}

/**
 * read from XML.
 */
void 
StudyCollection::readXML(QDomNode& nodeIn) throw (FileException)
{
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "studyCollectionName") {
            studyCollectionName = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "studyCollectionCreator") {
            studyCollectionCreator = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "studyType") {
            studyType = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "comment") {
            comment = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "studyName") {
            studyName = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "pmid") {
            pmid = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "searchID") {
            searchID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "fociListID") {
            fociListID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "fociColorListID") {
            fociColorListID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "sclID") {
            sclID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "topic") {
            topic = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "categoryID") {
            categoryID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "Studies") {
            QDomNode studyChildNode = node.firstChild();
            while (studyChildNode.isNull() == false) {
               QDomElement studyChildElement = studyChildNode.toElement();
               if (studyChildElement.isNull() == false) {
                  if (studyChildElement.tagName() == "StudyNamePubMedID") {
                     StudyNamePubMedID* s = new StudyNamePubMedID;
                     s->readXML(studyChildNode);
                     addStudyPMID(s);
                  }
               }
               studyChildNode = studyChildNode.nextSibling();
            }
         }
         else {
            std::cout << "WARNING: unrecognized Study Collection File Element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
      }
      
      node = node.nextSibling();
   }
}

/**
 * write to XML.
 */
void 
StudyCollection::writeXML(QDomDocument& xmlDoc,
                                               QDomElement& parentElement)
{
   //
   // Element for study collection
   //
   QDomElement studyCollectionElement = xmlDoc.createElement("StudyCollection");
   parentElement.appendChild(studyCollectionElement);
   
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    studyCollectionElement,
                                    "studyCollectionName", 
                                    studyCollectionName);
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    studyCollectionElement,
                                    "studyCollectionCreator", 
                                    studyCollectionCreator);
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    studyCollectionElement,
                                    "studyType", 
                                    studyType);
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    studyCollectionElement,
                                    "comment", 
                                    comment);
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    studyCollectionElement,
                                    "studyName", 
                                    studyName);
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    studyCollectionElement,
                                    "pmid", 
                                    pmid);
   AbstractFile::addXmlCdataElement(xmlDoc,
                                    studyCollectionElement,
                                    "fociListID",
                                    fociListID);
   AbstractFile::addXmlCdataElement(xmlDoc,
                                    studyCollectionElement,
                                    "fociColorListID",
                                    fociColorListID);
   AbstractFile::addXmlCdataElement(xmlDoc,
                                    studyCollectionElement,
                                    "sclID",
                                    sclID);
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    studyCollectionElement,
                                    "searchID", 
                                    searchID);
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    studyCollectionElement,
                                    "topic", 
                                    topic);
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    studyCollectionElement,
                                    "categoryID", 
                                    categoryID);
                                    
   //
   // Create element for studies
   //
   QDomElement studyElement = xmlDoc.createElement("Studies");
   studyCollectionElement.appendChild(studyElement);

   const int num = getNumberOfStudyPMIDs();
   for (int i = 0; i < num; i++) {
      studyPMIDs[i]->writeXML(xmlDoc,
                              studyElement);
   }
}

/**
 * Called to write XML
 */
void
StudyCollection::writeXML(XmlGenericWriter& xmlWriter, int indx) const throw (FileException)
{
   XmlGenericWriterAttributes attributes;
   attributes.addAttribute("Index", QString::number(indx));
   xmlWriter.writeStartElement("StudyCollection", attributes);

   xmlWriter.writeElementCData("studyCollectionName", studyCollectionName);
   xmlWriter.writeElementCData("studyCollectionCreator", studyCollectionCreator);
   xmlWriter.writeElementCData("studyType", studyType);
   xmlWriter.writeElementCData("comment", comment);
   xmlWriter.writeElementCData("studyName", studyName);
   xmlWriter.writeElementCData("pmid", pmid);
   xmlWriter.writeElementCData("fociListID", fociListID);
   xmlWriter.writeElementCData("fociColorListID", fociColorListID);
   xmlWriter.writeElementCData("sclID", sclID);
   xmlWriter.writeElementCData("searchID", searchID);
   xmlWriter.writeElementCData("topic", topic);
   xmlWriter.writeElementCData("categoryID", categoryID);

   const int num = getNumberOfStudyPMIDs();
   for (int i = 0; i < num; i++) {
      const StudyNamePubMedID* spmid = this->getStudyPMID(i);
      spmid->writeXML(xmlWriter);
   }

   xmlWriter.writeEndElement();
}

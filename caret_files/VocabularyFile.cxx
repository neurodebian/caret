
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
#include <set>

#include <QDomElement>
#include <QDomNode>

#include "CommaSeparatedValueFile.h"
#include "SpecFile.h"
#include "StringTable.h"
#include "VocabularyFile.h"
#include "XmlGenericWriter.h"

/**
 * constructor.
 */
VocabularyFile::VocabularyFile()
   : AbstractFile("Vocabulary File",
                  SpecFile::getVocabularyFileExtension(),
                  true,
                  FILE_FORMAT_XML,  // default format
                  FILE_IO_NONE,     // ascii format
                  FILE_IO_NONE,     // binary format
                  FILE_IO_READ_AND_WRITE,  // XML format
                  FILE_IO_NONE,     // XML base64
                  FILE_IO_NONE,     // XML gzip base64
                  FILE_IO_NONE,     // other format
                  FILE_IO_READ_AND_WRITE)     // csvf
{
}

/**
 * destructor.
 */
VocabularyFile::~VocabularyFile()
{
}

/**
 * add a vocabulary entry (returns its index).
 */
int 
VocabularyFile::addVocabularyEntry(const VocabularyEntry& ve)
{
   int indx = getVocabularyEntryIndexFromName(ve.getAbbreviation());
   if (indx >= 0) {
      VocabularyEntry* vocabExistsNow = getVocabularyEntry(indx);
      *vocabExistsNow = ve;
   }
   else {
      vocabularyEntries.push_back(ve);
      indx = getNumberOfVocabularyEntries() - 1;
   }
   vocabularyEntries[indx].vocabularyFile = this;
   setModified();
   return indx;
}

/**
 * delete a vocabulary entry.
 */
void 
VocabularyFile::deleteVocabularyEntry(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfVocabularyEntries())) {
      vocabularyEntries.erase(vocabularyEntries.begin() + indx);
   }
   setModified();
}
      
/**
 * append a vocabulary file to "this" vocabulary file.
 */
void 
VocabularyFile::append(const VocabularyFile& vf)
{
   const int origNumberOfStudyInfo = getNumberOfStudyInfo();
   const int num = vf.getNumberOfVocabularyEntries();
   for (int i = 0; i < num; i++) {
      VocabularyEntry ve = *(vf.getVocabularyEntry(i));
      int studyNum = ve.getStudyNumber();
      if (studyNum >= 0) {
         studyNum += origNumberOfStudyInfo;
      }
      ve.setStudyNumber(studyNum);
      addVocabularyEntry(ve);
   }
   
   //
   // Transfer the study info
   //
   for (int j = 0; j < vf.getNumberOfStudyInfo(); j++) {
      addStudyInfo((*vf.getStudyInfo(j)));
   }
   
   //
   // transfer the file's comment
   //
   appendToFileComment(vf.getFileComment());
}
      
/**
 * Clears current file data in memory.  Deriving classes must override this method and
 * call AbstractFile::clearAbstractFile() from its clear method.
 */
void 
VocabularyFile::clear()
{
   clearAbstractFile();
   
   vocabularyEntries.clear();
   studyInfo.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
VocabularyFile::empty() const
{
   return vocabularyEntries.empty();
}

/**
 * Read the contents of the file (header has already been read).
 */
void 
VocabularyFile::readFileData(QFile& file,
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
         {
            QDomNode node = rootElement.firstChild();
            while (node.isNull() == false) {
               QDomElement elem = node.toElement();
               if (elem.isNull() == false) { 
                  //
                  // Is this a "VocabularyEntry" element
                  //
                  if (elem.tagName() == "VocabularyEntry") {
                     VocabularyEntry ve;
                     ve.readXML(node);
                     addVocabularyEntry(ve);
                  }
                  else if (elem.tagName() == CellStudyInfo::tagCellStudyInfo) {
                     CellStudyInfo csi;
                     csi.readXML(node);
                     addStudyInfo(csi);
                  }
                  else if ((elem.tagName() == xmlHeaderOldTagName) ||
                           (elem.tagName() == xmlHeaderTagName)) {
                     // ignore, read by AbstractFile::readFile()
                  }
                  else {
                     std::cout << "WARNING: unrecognized Vocabulary File element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
                  }
               }
               node = node.nextSibling();
            }
         }
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
 * Write the file's data (header has already been written).
 */
void 
VocabularyFile::writeFileData(QTextStream& stream,
                              QDataStream& /*binStream*/,
                              QDomDocument& xmlDoc,
                              QDomElement& rootElement) throw (FileException)
{
   const int num = getNumberOfVocabularyEntries();

   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Writing in Ascii format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Writing in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            //
            // Write the vocabulary entries
            //
            for (int i = 0; i < num; i++) {
               const VocabularyEntry* ve = getVocabularyEntry(i);
               ve->writeXML(xmlDoc, rootElement);
            }
            
            //
            // Write the study info
            //
            const int numStudyInfo = getNumberOfStudyInfo();
            for (int i = 0; i < numStudyInfo; i++) {
               studyInfo[i].writeXML(xmlDoc, rootElement, i);
            }
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Writing in XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Writing in XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            writeDataIntoCommaSeparatedValueFile(csvf);
            csvf.writeToTextStream(stream);
         }
         break;
   }
}

/**
 * get indices to all linked studies.
 */
void 
VocabularyFile::getPubMedIDsOfAllLinkedStudyMetaData(std::vector<QString>& studyPMIDs) const
{
   std::set<QString> pmidSet;
   const int numVocab = getNumberOfVocabularyEntries();
   for (int i = 0; i < numVocab; i++) {
      const VocabularyEntry* ve = getVocabularyEntry(i);
      const StudyMetaDataLinkSet smdl = ve->getStudyMetaDataLinkSet();
      std::vector<QString> pmids;
      smdl.getAllLinkedPubMedIDs(pmids);
      pmidSet.insert(pmids.begin(), pmids.end());
   }
   studyPMIDs.clear();
   studyPMIDs.insert(studyPMIDs.end(),
                     pmidSet.begin(), pmidSet.end());
}

/**
 * a vocabulary entry using its index.
 */
VocabularyFile::VocabularyEntry* 
VocabularyFile::getVocabularyEntry(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfVocabularyEntries())) {
      return &vocabularyEntries[indx];
   }
   return NULL;
}

/**
 * a vocabulary entry using its index (const method).
 */
const VocabularyFile::VocabularyEntry* 
VocabularyFile::getVocabularyEntry(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfVocabularyEntries())) {
      return &vocabularyEntries[indx];
   }
   return NULL;
}

/**
 * get the index of a vocabulary entry from its abbreviation (-1 if not found).
 */
int 
VocabularyFile::getVocabularyEntryIndexFromName(const QString& abbreviationIn) const
{
   const int num = getNumberOfVocabularyEntries();
   for (int i = 0; i < num; i++) {
      const VocabularyEntry* ve = getVocabularyEntry(i);
      if (ve->getAbbreviation() == abbreviationIn) {
         return i;
      }
   }
   
   return -1;
}

/**
 * get a vocabulary entry from its abbreviation (NULL if not found).
 */
VocabularyFile::VocabularyEntry* 
VocabularyFile::getVocabularyEntryByName(const QString& abbreviationIn)
{
   const int indx = getVocabularyEntryIndexFromName(abbreviationIn);
   VocabularyEntry* ve = getVocabularyEntry(indx);
   return ve;
}

/**
 * get a vocabulary entry from its abbreviation (const method).
 */
const VocabularyFile::VocabularyEntry* 
VocabularyFile::getVocabularyEntryByName(const QString& abbreviationIn) const
{
   const int indx = getVocabularyEntryIndexFromName(abbreviationIn);
   const VocabularyEntry* ve = getVocabularyEntry(indx);
   return ve;
}

/**
 * get the best matching vocabulary entry (abbreviationIn begins with entry name).
 */
VocabularyFile::VocabularyEntry* 
VocabularyFile::getBestMatchingVocabularyEntry(const QString abbreviationIn,
                                               const bool caseSensitive)
{
   int bestMatchIndex = -1;
   int bestMatchLength = 0;
   
   Qt::CaseSensitivity cs = Qt::CaseInsensitive;
   if (caseSensitive) {
      cs = Qt::CaseSensitive;
   }
   
   const int num = getNumberOfVocabularyEntries();
   for (int i = 0; i < num; i++) {
      const VocabularyEntry* ve = getVocabularyEntry(i);
      const QString name(ve->getAbbreviation());
      
      if (abbreviationIn.startsWith(name, cs)) {
         const int len = name.length();
         if (len > bestMatchLength) {
            bestMatchIndex = i;
            bestMatchLength = len;
         }
      }
   }
   
   if (bestMatchIndex >= 0) {
      return getVocabularyEntry(bestMatchIndex);
   }
   return NULL;
}

/**
 * get the best matching vocabulary entry (abbreviationIn begins with entry name).
 */
const VocabularyFile::VocabularyEntry* 
VocabularyFile::getBestMatchingVocabularyEntry(const QString abbreviationIn,
                                               const bool caseSensitive) const
{
   int bestMatchIndex = -1;
   int bestMatchLength = 0;
   
   Qt::CaseSensitivity cs = Qt::CaseInsensitive;
   if (caseSensitive) {
      cs = Qt::CaseSensitive;
   }
   
   const int num = getNumberOfVocabularyEntries();
   for (int i = 0; i < num; i++) {
      const VocabularyEntry* ve = getVocabularyEntry(i);
      const QString name(ve->getAbbreviation());
      
      if (abbreviationIn.startsWith(name, cs)) {
         const int len = name.length();
         if (len > bestMatchLength) {
            bestMatchIndex = i;
            bestMatchLength = len;
         }
      }
   }
   
   if (bestMatchIndex >= 0) {
      return getVocabularyEntry(bestMatchIndex);
   }
   return NULL;
}
      
/**
 * find out if comma separated file conversion supported.
 */
void 
VocabularyFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                       bool& writeToCSV) const
{
   readFromCSV = true;
   writeToCSV  = true;
}
                                        
/**
 * write the file's data into a comma separated values file (throws exception if not supported).
 */
void 
VocabularyFile::writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException)
{
   csv.clear();
   
   const int numCells = getNumberOfVocabularyEntries();
   if (numCells <= 0) {
      return;
   }
   
   StringTable* headerTable = new StringTable(0, 0);
   writeHeaderDataIntoStringTable(*headerTable);
   csv.addDataSection(headerTable);
   
   StringTable* entryTable = new StringTable(0, 0);
   VocabularyEntry::writeDataIntoStringTable(vocabularyEntries, *entryTable);
   csv.addDataSection(entryTable);

   StringTable* studyInfoTable = new StringTable(0, 0);
   CellStudyInfo::writeDataIntoStringTable(studyInfo, *studyInfoTable);
   csv.addDataSection(studyInfoTable);
}

/**
 * read the file's data from a comma separated values file (throws exception if not supported).
 */
void 
VocabularyFile::readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException)
{
   clear();
      
   //
   // Do header
   //
   const StringTable* head = csv.getDataSectionByName("header");
   if (head != NULL) {
      readHeaderDataFromStringTable(*head);
   }
   
   //
   // Do vocabulary entries
   //
   const StringTable* stve = csv.getDataSectionByName("Vocabulary Entries");
   if (stve != NULL) {
      VocabularyEntry::readDataFromStringTable(vocabularyEntries,
                                               *stve);
   }
   
   const int num = static_cast<int>(vocabularyEntries.size());
   for (int i = 0; i < num; i++) {
      vocabularyEntries[i].vocabularyFile = this;
   }
   
   //
   // Do study info
   //
   const StringTable* stcsi = csv.getDataSectionByName("Cell Study Info");
   if (stcsi != NULL) {
      CellStudyInfo::readDataFromStringTable(studyInfo, *stcsi);
   }
}
      
/**
 * Get the study info index based upon the study info's value.
 */
int 
VocabularyFile::getStudyInfoFromValue(const CellStudyInfo& csi) const
{
   const int num = getNumberOfStudyInfo();
   for (int i = 0; i < num; i++) {
      if ((*getStudyInfo(i)) == csi) {
         return i;
      }
   }
   return -1;
}

/**
 * Get a study info (const method).
 */
const CellStudyInfo*
VocabularyFile::getStudyInfo(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfStudyInfo())) {
      return &studyInfo[indx];
   }
   return NULL;
}

/**
 * Get a study info.
 */
CellStudyInfo*
VocabularyFile::getStudyInfo(const int indx) 
{
   if ((indx >= 0) && (indx < getNumberOfStudyInfo())) {
      return &studyInfo[indx];
   }
   return NULL;
}

/**
 * Add a study info.
 */
int
VocabularyFile::addStudyInfo(const CellStudyInfo& csi)
{
   studyInfo.push_back(csi);
   const int index = studyInfo.size() - 1;
   return index;
}

/**
 * delete all study info and clear links to study info
 */
void 
VocabularyFile::deleteAllStudyInfo()
{
   const int num = getNumberOfVocabularyEntries();
   for (int i = 0; i < num; i++) {
      VocabularyEntry* ve = getVocabularyEntry(i);
      ve->setStudyNumber(-1);
   }
   studyInfo.clear();
}
      
/**
 * delete study info.
 */
void 
VocabularyFile::deleteStudyInfo(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfStudyInfo())) {
      for (int i = 0; i < getNumberOfVocabularyEntries(); i++) {
         const int studyNum = vocabularyEntries[i].getStudyNumber();
         if (studyNum == indx) {
            vocabularyEntries[i].setStudyNumber(-1);
         }
         else if (studyNum > indx) {
            vocabularyEntries[i].setStudyNumber(studyNum - 1);
         }
      }
      
      studyInfo.erase(studyInfo.begin() + indx);
   }
}
      
/**
 * Set a study info.
 */
void
VocabularyFile::setStudyInfo(const int index, const CellStudyInfo& csi)
{
   studyInfo[index] = csi;
}

//********************************************************************************

/**
 * constructor.
 */
VocabularyFile::VocabularyEntry::VocabularyEntry()
{
   clear();
}
  
/**
 * constructor.
 */
VocabularyFile::VocabularyEntry::VocabularyEntry(const QString& abbreviationIn,
                                                 const QString& fullNameIn,
                                                 const QString& classNameIn,
                                                 const QString& vocabularyIDIn,
                                                 const QString& descriptionIn,
                                                 const QString& ontologySourceIn,
                                                 const QString& termIDIn,
                                                 const int studyNumberIn)
{
   clear();
   setAbbreviation(abbreviationIn);
   setFullName(fullNameIn);
   setClassName(classNameIn);
   setVocabularyID(vocabularyIDIn);
   setDescription(descriptionIn);
   setOntologySource(ontologySourceIn);
   setTermID(termIDIn);
   setStudyNumber(studyNumberIn);
}
  
/**
 * copy constructor.
 */
VocabularyFile::VocabularyEntry::VocabularyEntry(const VocabularyEntry& ve)
{
   clear();
   copyHelper(ve);
}
  
/**
 * assignment operator.
 */
VocabularyFile::VocabularyEntry& 
VocabularyFile::VocabularyEntry::operator=(const VocabularyEntry& ve)
{
   if (this != &ve) {
      copyHelper(ve);
   }
   
   return *this;
}
            
/**
 * destructor.
 */
VocabularyFile::VocabularyEntry::~VocabularyEntry()
{
}

/**
 * copy helper.
 */
void 
VocabularyFile::VocabularyEntry::copyHelper(const VocabularyEntry& ve)
{
   abbreviation = ve.abbreviation;
   fullName     = ve.fullName;
   className    = ve.className;
   ontologySource = ve.ontologySource;
   termID       = ve.termID;
   vocabularyID = ve.vocabularyID;
   description  = ve.description;
   studyNumber  = ve.studyNumber;
   studyMetaDataLinkSet = ve.studyMetaDataLinkSet;
}
            
/**
 * clear this item.
 */
void 
VocabularyFile::VocabularyEntry::clear()
{
   vocabularyFile = NULL;
   studyNumber = -1;
   studyMetaDataLinkSet.clear();
}
            
/**
 * set the abbreviation.
 */
void 
VocabularyFile::VocabularyEntry::setAbbreviation(const QString& a)
{
   abbreviation = a;
   setModified();
}

/**
 * set the description.
 */
void 
VocabularyFile::VocabularyEntry::setDescription(const QString& d)
{
   description = d;
   setModified();
}            

/**
 * set the full name.
 */
void 
VocabularyFile::VocabularyEntry::setFullName(const QString& n)
{
   fullName = n;
   setModified();
}
            
/**
 * set the class name.
 */
void 
VocabularyFile::VocabularyEntry::setClassName(const QString& s)
{
   className = s;
   setModified();
}

/**
 * get ontology source values.
 */
void 
VocabularyFile::VocabularyEntry::getOntologySourceValues(std::vector<QString>& ontologySourceValues)
{
   ontologySourceValues.clear();
   ontologySourceValues.push_back("BIRNLex");
   ontologySourceValues.push_back("NIFSTD");
   ontologySourceValues.push_back("NeuroNames");
}
            
/**
 * set the ontology source.
 */
void 
VocabularyFile::VocabularyEntry::setOntologySource(const QString& s)
{
   ontologySource = s;
   setModified();
}

/**
 * set the term id.
 */
void 
VocabularyFile::VocabularyEntry::setTermID(const QString& s)
{
   termID = s;
   setModified();
}
            
/**
 * set the vocabulary ID.
 */
void 
VocabularyFile::VocabularyEntry::setVocabularyID(const QString& s)
{
   vocabularyID = s;
   setModified();
}

/**
 * set the study number.
 */
void 
VocabularyFile::VocabularyEntry::setStudyNumber(const int sn)
{
   studyNumber = sn;
   setModified();
}
                        
/**
 * set the study metadata link.
 */
void 
VocabularyFile::VocabularyEntry::setStudyMetaDataLinkSet(const StudyMetaDataLinkSet smdls)
{
   studyMetaDataLinkSet = smdls;
   setModified();
}
            
/**
 * set this item modified.
 */
void 
VocabularyFile::VocabularyEntry::setModified()
{
   if (vocabularyFile != NULL) {
      vocabularyFile->setModified();
   }
}

/**
 * called to read from an XML structure.
 */
void 
VocabularyFile::VocabularyEntry::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != "VocabularyEntry") {
      QString msg("Incorrect element type passed to VocabularyFile::VocabularyEntry::readXML(): \"");
      msg.append(elem.tagName());
      msg.append("\"");
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "abbreviation") {
            abbreviation = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "fullName") {
            fullName = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "className") {
            className = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "vocabularyID") {
            vocabularyID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "description") {
            description = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "ontologySource") {
            ontologySource = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "termID") {
            termID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "studyNumber") {
            studyNumber = AbstractFile::getXmlElementFirstChildAsInt(elem);
         }
         else if (elem.tagName() == "StudyMetaDataLink") {
            StudyMetaDataLink smdl;
            smdl.readXML(node);
            studyMetaDataLinkSet.addStudyMetaDataLink(smdl);
         }
         else if (elem.tagName() == "StudyMetaDataLinkSet") {
            studyMetaDataLinkSet.readXML(node);
         }
         else {
            std::cout << "WARNING: unrecognized VocabularyEntry element: "
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
VocabularyFile::VocabularyEntry::writeXML(QDomDocument& xmlDoc,
                                          QDomElement&  parentElement) const
{
   //
   // Create the element for this class instance's data
   //
   QDomElement vocabularyDataElement = xmlDoc.createElement("VocabularyEntry");

   //
   // color elements
   //
   AbstractFile::addXmlCdataElement(xmlDoc, vocabularyDataElement, "abbreviation", abbreviation);
   AbstractFile::addXmlCdataElement(xmlDoc, vocabularyDataElement, "fullName", fullName);
   AbstractFile::addXmlCdataElement(xmlDoc, vocabularyDataElement, "className", className);
   AbstractFile::addXmlCdataElement(xmlDoc, vocabularyDataElement, "vocabularyID", vocabularyID);
   AbstractFile::addXmlCdataElement(xmlDoc, vocabularyDataElement, "description", description);
   AbstractFile::addXmlCdataElement(xmlDoc, vocabularyDataElement, "ontologySource", ontologySource);
   AbstractFile::addXmlCdataElement(xmlDoc, vocabularyDataElement, "termID", termID);
   AbstractFile::addXmlCdataElement(xmlDoc, vocabularyDataElement, "studyNumber", QString::number(studyNumber));
   studyMetaDataLinkSet.writeXML(xmlDoc, vocabularyDataElement);
   //
   // Add class instance's data to the parent
   //
   parentElement.appendChild(vocabularyDataElement);
}

/**
 * called to write XML.
 */
void
VocabularyFile::VocabularyEntry::writeXML(XmlGenericWriter& xmlWriter) const throw (FileException)
{
   xmlWriter.writeStartElement("VocabularyEntry");
   xmlWriter.writeElementCData("abbreviation", abbreviation);
   xmlWriter.writeElementCData("fullName", fullName);
   xmlWriter.writeElementCData("className", className);
   xmlWriter.writeElementCData("vocabularyID", vocabularyID);
   xmlWriter.writeElementCData("description", description);
   xmlWriter.writeElementCData("ontologySource", ontologySource);
   xmlWriter.writeElementCData("termID", termID);
   studyMetaDataLinkSet.writeXML(xmlWriter);
   xmlWriter.writeEndElement();
}
/**
 * write the data into a StringTable.
 */
void 
VocabularyFile::VocabularyEntry::writeDataIntoStringTable(const std::vector<VocabularyEntry>& data,
                                                          StringTable& table)
{
   table.clear();
   
   const int num = static_cast<int>(data.size());
   
   //
   // Column numbers for data
   //
   int numCols = 0;
   const int abbreviationCol = numCols++;
   const int fullNameCol = numCols++;
   const int classNameCol = numCols++;
   const int vocabularyIDCol = numCols++;
   const int descriptionCol = numCols++;
   const int ontologySourceCol = numCols++;
   const int termIDCol = numCols++;
   const int studyPubMedIDCol = numCols++;
   const int studyNumberCol = numCols++;
   
   //
   // Table column names
   //
   table.setNumberOfRowsAndColumns(num, numCols, "Vocabulary Entries");
   table.setColumnTitle(abbreviationCol, "Abbreviation");
   table.setColumnTitle(fullNameCol, "Full Name");
   table.setColumnTitle(classNameCol, "Class Name");
   table.setColumnTitle(vocabularyIDCol, "Vocabulary ID");
   table.setColumnTitle(descriptionCol, "Description");
   table.setColumnTitle(ontologySourceCol, "Ontology Source");
   table.setColumnTitle(termIDCol, "Term ID");
   table.setColumnTitle(studyNumberCol, "Study Number");
   table.setColumnTitle(studyPubMedIDCol, "StudyMetaDataLink");
   for (int i = 0; i < num; i++) {
      const VocabularyEntry& ve = data[i];
      table.setElement(i, abbreviationCol, ve.getAbbreviation());
      table.setElement(i, fullNameCol, ve.getFullName());
      table.setElement(i, classNameCol, ve.getClassName());
      table.setElement(i, vocabularyIDCol, ve.getVocabularyID());
      table.setElement(i, descriptionCol, ve.getDescription());
      table.setElement(i, studyNumberCol, ve.getStudyNumber());
      table.setElement(i, ontologySourceCol, ve.getOntologySource());
      table.setElement(i, termIDCol, ve.getTermID());
      const int numLinks = ve.getStudyMetaDataLinkSet().getNumberOfStudyMetaDataLinks();
      if (numLinks > 1) {
         throw FileException("Vocabulary Entry \""
                             + ve.getFullName() 
                             + "\" has more than one Study Metadata Link.  "
                               "Cannot write to Table");
      }
      else if (numLinks == 1) {
         table.setElement(i, studyPubMedIDCol, 
                          ve.getStudyMetaDataLinkSet().getStudyMetaDataLink(0).getLinkAsCodedText());
      }
   }
}

/**
 * read the data from a StringTable.
 */
void 
VocabularyFile::VocabularyEntry::readDataFromStringTable(std::vector<VocabularyEntry>& data,
                                                         const StringTable& table) throw (FileException)
{
   if (table.getTableTitle() != "Vocabulary Entries") {
      throw FileException("String table for Vocabulary does not have the name Vocabulary");
   }
   
   data.clear();
   
   int abbreviationCol = -1;
   int fullNameCol = -1;
   int classNameCol = -1;
   int vocabularyIDCol = -1;
   int descriptionCol = -1;
   int ontologySourceCol = -1;
   int termIDCol = -1;
   int studyNumberCol = -1;
   int studyPubMedIDCol = -1;

   const int numCols = table.getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString name = table.getColumnTitle(i).toLower();
      if (name == "abbreviation") {
         abbreviationCol = i;
      }
      else if (name == "full name") {
         fullNameCol = i;
      }
      else if (name == "class name") {
         classNameCol = i;
      }
      else if (name == "vocabulary id") {
         vocabularyIDCol = i;
      }
      else if (name == "description") {
         descriptionCol = i;
      }
      else if (name == "ontology source") {
         ontologySourceCol = i;
      }
      else if (name == "term id") {
         termIDCol = i;
      }
      else if (name == "study number") {
         studyNumberCol = i;
      }
      else if ((name == "study pubmed id") ||
               (name == "studymetadatalink")) {
         studyPubMedIDCol = i;
      }
   }
   
   const int numItems = table.getNumberOfRows();
   for (int i = 0; i < numItems; i++) {
      VocabularyEntry ve;
      
      if (abbreviationCol >= 0) {
        ve.setAbbreviation(table.getElement(i, abbreviationCol));
      }
      if (fullNameCol >= 0) {
        ve.setFullName(table.getElement(i, fullNameCol));
      }
      if (classNameCol >= 0) {
        ve.setClassName(table.getElement(i, classNameCol));
      }
      if (vocabularyIDCol >= 0) {
        ve.setVocabularyID(table.getElement(i, vocabularyIDCol));
      }
      if (descriptionCol >= 0) {
        ve.setDescription(table.getElement(i, descriptionCol));
      }
      if (ontologySourceCol >= 0) {
         ve.setOntologySource(table.getElement(i, ontologySourceCol));
      }
      if (termIDCol >= 0) {
         ve.setTermID(table.getElement(i, termIDCol));
      }
      if (studyPubMedIDCol >= 0) {
         StudyMetaDataLink smdl;
         smdl.setLinkFromCodedText(table.getElement(i, studyPubMedIDCol));
         if (smdl.getPubMedID().isEmpty() == false) {
            StudyMetaDataLinkSet smdls;
            smdls.addStudyMetaDataLink(smdl);
            ve.setStudyMetaDataLinkSet(smdls);
         }
      }
      if (studyNumberCol >= 0) {
        ve.setStudyNumber(table.getElementAsInt(i, studyNumberCol));
      }
      
      data.push_back(ve);
   }
}
            
/**
 * Write the file's memory in caret6 format to the specified name.
 */
QString
VocabularyFile::writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException)
{
   int numVocab = this->getNumberOfVocabularyEntries();
   if (numVocab <= 0) {
      throw FileException("Contains no vocabulary");
   }

   QFile file(filenameIn);
   if (file.open(QFile::WriteOnly) == false) {
      throw FileException("Unable to open for writing");
   }
   QTextStream stream(&file);

   XmlGenericWriter xmlWriter(stream);
   xmlWriter.writeStartDocument();

   XmlGenericWriterAttributes attributes;
   attributes.addAttribute("xmlns:xsi",
                           "http://www.w3.org/2001/XMLSchema-instance");
   attributes.addAttribute("xsi:noNamespaceSchemaLocation",
                           "http://brainvis.wustl.edu/caret6/xml_schemas/VocabularyFileSchema.xsd");
   attributes.addAttribute("CaretFileType", "Vocabulary");
   attributes.addAttribute("Version", "6.0");
   xmlWriter.writeStartElement("CaretDataFile", attributes);

   this->writeHeaderXMLWriter(xmlWriter);

   for (int i = 0; i < numVocab; i++) {
      const VocabularyEntry* v = getVocabularyEntry(i);
      v->writeXML(xmlWriter);
   }

   xmlWriter.writeEndElement();

   xmlWriter.writeEndDocument();

   file.close();

   return filenameIn;
}

/**
 * get full description of all fields for display to user.
 */
/*
QString 
VocabularyFile::VocabularyEntry::getFullDescriptionForDisplayToUser(const bool useHTML) const
{
   const QString boldStart("<B>");
   const QString boldEnd("</B>");
   const QString newLine("\n");
   
   QString s;
   if (useHTML) s += boldStart;
   s += "Abbreviation";
   if (useHTML) s += boldEnd;
   s += ": ";
   s += abbreviation;
   s += newLine;
   
   if (fullName.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Full Name";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += fullName;
      s += newLine;
   } 
   
   if (className.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Class Name";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += className;
      s += newLine;
   } 
   
   if (vocabularyID.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Vocabulary ID";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += vocabularyID;
      s += newLine;
   } 
   
   if (description.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Description";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += description;
      s += newLine;
   } 
   
   if (ontologySource.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Ontology Source";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += ontologySource;
      s += newLine;
   }
   
   if (termID.isEmpty() == false) {
      if (useHTML) s += boldStart;
      s += "Term ID";
      if (useHTML) s += boldEnd;
      s += ": ";
      s += termID;
      s += newLine;
   }
   
   if (vocabularyFile != NULL) {
      if ((studyNumber >= 0) && (studyNumber < vocabularyFile->getNumberOfStudyInfo())) {
         const CellStudyInfo* csi = vocabularyFile->getStudyInfo(studyNumber);
         const QString s2 = csi->getFullDescriptionForDisplayToUser(true);
         if (s2.isEmpty() == false) {
            s += s2;
         }
      }
   }
   
   return s;
}            
*/

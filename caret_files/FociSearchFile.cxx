
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
#include <QRegExp>

#include "FociProjectionFile.h"
#define __FOCI_SEARCH_FILE_MAIN__
#include "FociSearchFile.h"
#undef __FOCI_SEARCH_FILE_MAIN__
#include "MathUtilities.h"
#include "SpecFile.h"
#include "StudyMetaDataFile.h"
#include "StudyMetaDataLinkSet.h"

/**
 * constructor.
 */
FociSearchFile::FociSearchFile()
   : AbstractFile("Foci Search File", 
                  SpecFile::getFociSearchFileExtension(),
                  true,
                  FILE_FORMAT_XML,
                  FILE_IO_NONE,   // ascii
                  FILE_IO_NONE,   // binary
                  FILE_IO_READ_AND_WRITE,   // xml
                  FILE_IO_NONE,   // xml base64
                  FILE_IO_NONE,   // xml gzip base64
                  FILE_IO_NONE,   // other
                  FILE_IO_NONE) // csv
{
}

/**
 * copy constructor.
 */
FociSearchFile::FociSearchFile(const FociSearchFile& fsf)
   : AbstractFile("Foci Search File", 
                  SpecFile::getFociSearchFileExtension(),
                  true,
                  FILE_FORMAT_XML,
                  FILE_IO_NONE,   // ascii
                  FILE_IO_NONE,   // binary
                  FILE_IO_READ_AND_WRITE,   // xml
                  FILE_IO_NONE,   // xml base64
                  FILE_IO_NONE,   // xml gzip base64
                  FILE_IO_NONE,   // other
                  FILE_IO_NONE) // csv
{
   copyHelper(fsf);
}
      
/**
 * assignment operator.
 */
FociSearchFile& 
FociSearchFile::operator=(const FociSearchFile& fsf)
{
   if (&fsf != this) {
      AbstractFile::operator=(fsf);
      copyHelper(fsf);
   }
   
   return *this;
}
      
/**
 * copy helper.
 */
void 
FociSearchFile::copyHelper(const FociSearchFile& fsf)
{
   setFileName("");
   clear();
   
   append(fsf);
}

/**
 * destructor.
 */
FociSearchFile::~FociSearchFile()
{
   clear();
}
   
/**
 * append a file.
 */
void 
FociSearchFile::append(const FociSearchFile& fsf)
{
   const int num = fsf.getNumberOfFociSearchSets();
   for (int i = 0; i < num; i++) {
      FociSearchSet* fss = new FociSearchSet(*fsf.getFociSearchSet(i));
      addFociSearchSet(fss);
   }
}

/**
 * add a default empty search.
 */
void 
FociSearchFile::addDefaultSearch()
{
   if (getNumberOfFociSearchSets() <= 0) {      
      //
      // Add a search set with one search
      //
      //FociSearch* fs = new FociSearch;
      FociSearchSet* fss = new FociSearchSet;
      //fss->addFociSearch(fs);
      addFociSearchSet(fss);
      clearModified();
   }
}

/**
 * copy a search set to a new search set
 * returns positive number of new search set, else -1 if error.
 */
int 
FociSearchFile::copySearchSetToNewSearchSet(const int copySearchSetNumber)
{
   const FociSearchSet* copySearchSet = getFociSearchSet(copySearchSetNumber);
   if (copySearchSet != NULL) {
      FociSearchSet* fss = new FociSearchSet(*copySearchSet);
      fss->setName("Copy of " + fss->getName());
      addFociSearchSet(fss);
      const int indx = getNumberOfFociSearchSets() - 1;
      return indx;
   }
   
   return -1;
}
      
/**
 * get a search set.
 */
FociSearchSet* 
FociSearchFile::getFociSearchSet(const int indx) 
{ 
   if ((indx >= 0) && (indx < getNumberOfFociSearchSets())) {
      return fociSearchSets[indx]; 
   }
   
   return NULL;
}

/**
 * get a search set (const method).
 */
const FociSearchSet* 
FociSearchFile::getFociSearchSet(const int indx) const 
{ 
   if ((indx >= 0) && (indx < getNumberOfFociSearchSets())) {
      return fociSearchSets[indx]; 
   }
   
   return NULL;
}
      
/**
 * call AbstractFile::clearAbstractFile() from its clear method.
 */
void 
FociSearchFile::clear()
{
   AbstractFile::clearAbstractFile();
   
   clearFociSearches();
   
   setModified();
}

/**
 * clear foci searches.
 */
void 
FociSearchFile::clearFociSearches()
{
   const int num = getNumberOfFociSearchSets();
   for (int i = 0; i < num; i++) {
      delete fociSearchSets[i];
      fociSearchSets[i] = NULL;
   }
   fociSearchSets.clear();
   
   setModified();
}
      
/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
FociSearchFile::empty() const
{
   return (getNumberOfFociSearchSets() == 0);
}

/**
 * delete a search set.
 */
void 
FociSearchFile::deleteFociSearchSet(const int indx)
{
   if ((indx >= 0) &&
       (indx < getNumberOfFociSearchSets())) {
      fociSearchSets.erase(fociSearchSets.begin() + indx);
      setModified();
   }
}

/**
 * add a search set.
 */
void 
FociSearchFile::addFociSearchSet(FociSearchSet* fss)
{
   fss->setParentFociSearchFile(this);
   fociSearchSets.push_back(fss);
   setModified();
}

/**
 * insert a foci search set.
 */
void 
FociSearchFile::insertFociSearchSet(FociSearchSet* fss,
                                    const int afterIndex)
{
   //
   // Set parent
   //
   fss->setParentFociSearchFile(this);
   
   //
   // No existing searchers?
   //
   if (getNumberOfFociSearchSets() <= 0) {
      fociSearchSets.push_back(fss);
   }
   else {
      //
      // Copy searches
      //
      std::vector<FociSearchSet*> copyOfFociSearchSets = fociSearchSets;
      fociSearchSets.clear();
      const int num = static_cast<int>(copyOfFociSearchSets.size());
      
      //
      // Insert new search set in proper location
      //
      for (int i = 0; i < num; i++) {
         if ((i == 0) && (afterIndex < 0)) {
            fociSearchSets.push_back(fss);
         }
         fociSearchSets.push_back(copyOfFociSearchSets[i]);
         if (afterIndex == i) {
            fociSearchSets.push_back(fss);
         }
      }
   }
   
   setModified();
}
                            
/**
 * Read the contents of the file (header has already been read).
 */
void 
FociSearchFile::readFileData(QFile& /*file*/,
                             QTextStream& /*stream*/,
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
         throw FileException(filename, "Reading in Ascii format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            clearFociSearches();
            
            QDomNode node = rootElement.firstChild();
            while (node.isNull() == false) {
               QDomElement elem = node.toElement();
               if (elem.isNull() == false) { 
                  //
                  // Is this a "FociSearchSet" element
                  //
                  if (elem.tagName() == FociSearchSet::tagFociSearchSet) {
                     FociSearchSet* fss = new FociSearchSet;
                     fss->readXML(node);
                     addFociSearchSet(fss);
                  }
                  else if ((elem.tagName() == xmlHeaderOldTagName) ||
                           (elem.tagName() == xmlHeaderTagName)) {
                     // ignore, read by AbstractFile::readFile()
                  }
                  else {
                     std::cout << "WARNING: unrecognized FociSearchFile element: "
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
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Reading in CSV format not supported.");
         break;
   }
}

/**
 * Write the file's data (header has already been written).
 */
void 
FociSearchFile::writeFileData(QTextStream& /*stream*/,
                              QDataStream& /*binStream*/,
                              QDomDocument& xmlDoc,
                              QDomElement& rootElement) throw (FileException)
{
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
            // Write the search sets
            //
            const int numSearchSets = getNumberOfFociSearchSets();
            for (int i = 0; i < numSearchSets; i++) {
               FociSearchSet* fss = getFociSearchSet(i);
               fss->writeXML(xmlDoc, rootElement);
            }
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Writing XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Writing XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Writing in CSV format not supported.");
         break;
   }
}


//============================================================================

/**
 * constructor.
 */
FociSearchSet::FociSearchSet()
{
   parentFociSearchFile = NULL;
   name = "Name of Search";

   FociSearch* fs = new FociSearch;
   fs->setLogic(FociSearch::LOGIC_INTERSECTION);
   addFociSearch(fs);
}

/**
 * copy constructor.
 */
FociSearchSet::FociSearchSet(const FociSearchSet& fss)
{
   copyHelper(fss);
}
      
/**
 * assignment operator.
 */
FociSearchSet& 
FociSearchSet::operator=(const FociSearchSet& fss)
{
   if (&fss != this) {
      copyHelper(fss);
   }
   
   return *this;
}
      
/**
 * copy helper.
 */
void 
FociSearchSet::copyHelper(const FociSearchSet& fs)
{
   parentFociSearchFile = NULL;
   clear();
   
   const int num = fs.getNumberOfFociSearches();
   for (int i = 0; i < num; i++) {
      addFociSearch(new FociSearch(*fs.getFociSearch(i)));
   }
   
   name = fs.name;
}

/**
 * destructor.
 */
FociSearchSet::~FociSearchSet()
{
   clear();
}

/**
 * clear the search set.
 */
void 
FociSearchSet::clear()
{
   const int num = getNumberOfFociSearches();
   for (int i = 0; i < num; i++) {
      delete fociSearches[i];
      fociSearches[i] = NULL;
   }
   fociSearches.clear();
   
   setModified();
}

/**
 * set parent foci search file.
 */
void 
FociSearchSet::setParentFociSearchFile(FociSearchFile* fsf)
{
   parentFociSearchFile = fsf;
}
      
/**
 * delete a search.
 */
void 
FociSearchSet::deleteFociSearch(const int indx)
{
   if ((indx >= 0) &&
       (indx < getNumberOfFociSearches())) {
      fociSearches.erase(fociSearches.begin() + indx);
      setModified();
   }
}

/**
 * add a search.
 */
void 
FociSearchSet::addFociSearch(FociSearch* fs)
{
   fs->setParentFociSearchSet(this);
   
   fociSearches.push_back(fs);
   setModified();
}
      
/**
 * insert a foci search.
 */
void 
FociSearchSet::insertFociSearch(FociSearch* fs,
                                 const int afterIndex)
{
   //
   // Set parent
   //
   fs->setParentFociSearchSet(this);
   
   //
   // No existing searchers?
   //
   if (getNumberOfFociSearches() <= 0) {
      fociSearches.push_back(fs);
   }
   else {
      //
      // Copy searches
      //
      std::vector<FociSearch*> copyOfFociSearches = fociSearches;
      fociSearches.clear();
      const int num = static_cast<int>(copyOfFociSearches.size());
      
      //
      // Insert new search in proper location
      //
      for (int i = 0; i < num; i++) {
         if ((i == 0) && (afterIndex < 0)) {
            fociSearches.push_back(fs);
         }
         fociSearches.push_back(copyOfFociSearches[i]);
         if (afterIndex == i) {
            fociSearches.push_back(fs);
         }
      }
   }
   
   setModified();
}
                            
/**
 * set the name of the search.
 */
void 
FociSearchSet::setName(const QString& s)
{
   if (name != s) {
      name = s;
      setModified();
   }
}

/**
 * set the file's modified status.
 */
void 
FociSearchSet::setModified()
{
   if (parentFociSearchFile != NULL) {
      parentFociSearchFile->setModified();
   }
}
      
/**
 * called to read from an XML structure.
 */
void 
FociSearchSet::readXML(QDomNode& nodeIn) throw (FileException)
{
   clear();
   
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != tagFociSearchSet) {
      QString msg("Incorrect element type passed to FociSearchSet::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == tagFociSearchSetName) {
            name = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == FociSearch::tagFociSearch) {
            FociSearch* fs = new FociSearch;
            fs->readXML(node);
            addFociSearch(fs);
         }
         else {
            std::cout << "WARNING: unrecognized FociSearchSet element: "
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
FociSearchSet::writeXML(QDomDocument& xmlDoc,
                        QDomElement&  parentElement)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement searchSetElement = xmlDoc.createElement(tagFociSearchSet);

   //
   // Name of search
   //
   AbstractFile::addXmlTextElement(xmlDoc, searchSetElement,
                                   tagFociSearchSetName, name);
   //
   // Write the searches
   //
   const int numSearches = getNumberOfFociSearches();
   for (int i = 0; i < numSearches; i++) {
      FociSearch* fs = getFociSearch(i);
      fs->writeXML(xmlDoc, searchSetElement);
   }
   
   //
   // Add class instance's data to the parent
   //
   parentElement.appendChild(searchSetElement);
}

//============================================================================

/**
 * constructor.
 */
FociSearch::FociSearch()
{
   parentFociSearchSet = NULL;
   
   logic      = LOGIC_UNION;
   attribute  = ATTRIBUTE_ALL;
   matching   = MATCHING_ANY_OF;
   searchText = "";
}

/**
 * copy constructor.
 */
FociSearch::FociSearch(const FociSearch& fs)
{
   copyHelper(fs);
}
      
/**
 * assignment operator.
 */
FociSearch& 
FociSearch::operator=(const FociSearch& fs)
{
   if (&fs != this) {
      copyHelper(fs);
   }
   
   return *this;
}
      
/**
 * copy helper.
 */
void 
FociSearch::copyHelper(const FociSearch& fs)
{
   parentFociSearchSet = NULL;
   logic      = fs.logic;
   attribute  = fs.attribute;
   matching   = fs.matching;
   searchText = fs.searchText;
}

/**
 * destructor.
 */
FociSearch::~FociSearch()
{
}

/**
 * set parent foci search set.
 */
void 
FociSearch::setParentFociSearchSet(FociSearchSet* fss)
{
   parentFociSearchSet = fss;
}
      
/**
 * set the file's modified status.
 */
void 
FociSearch::setModified()
{
   if (parentFociSearchSet != NULL) {
      parentFociSearchSet->setModified();
   }
}

/**
 * set the logic for the search.
 */
void 
FociSearch::setLogic(const LOGIC lg)
{
   if (logic != lg) {
      logic = lg;
      setModified();
   }
}

/**
 * set the attribute.
 */
void 
FociSearch::setAttribute(const ATTRIBUTE a)
{
   if (attribute != a) {
      attribute = a;
      setModified();
   }
}

/**
 * set the matching.
 */
void 
FociSearch::setMatching(const MATCHING m)
{
   if (matching != m) {
      matching = m;
      setModified();
   }
}

/**
 * set the search text.
 */
void 
FociSearch::setSearchText(const QString& st)
{
   if (searchText != st) {
      searchText = st;
      setModified();
   }
}

/**
 * called to read from an XML structure.
 */
void 
FociSearch::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != tagFociSearch) {
      QString msg("Incorrect element type passed to FociSearch::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == tagFociSearchLogic) {
            logic = convertLogicNameToType(
               AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagFociSearchAttribute) {
            attribute = convertAttributeNameToType(
               AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagFociSearchMatching) {
            matching = convertMatchingNameToType(
               AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagFociSearchText) {
            searchText = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }
         else {
            std::cout << "WARNING: unrecognized FociSearch element: "
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
FociSearch::writeXML(QDomDocument& xmlDoc,
                     QDomElement&  parentElement)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement searchElement = xmlDoc.createElement(tagFociSearch);

   //
   // Search attributes
   //
   AbstractFile::addXmlTextElement(xmlDoc, searchElement,
                                   tagFociSearchLogic, 
                                   convertLogicTypeToName(logic));
   AbstractFile::addXmlTextElement(xmlDoc, searchElement,
                                   tagFociSearchAttribute, 
                                   convertAttributeTypeToName(attribute));
   AbstractFile::addXmlTextElement(xmlDoc, searchElement,
                                   tagFociSearchMatching, 
                                   convertMatchingTypeToName(matching));
   AbstractFile::addXmlTextElement(xmlDoc, searchElement,
                                   tagFociSearchText, searchText);
   //
   // Add class instance's data to the parent
   //
   parentElement.appendChild(searchElement);
}

/**
 * get types and names for logic.
 */
void 
FociSearch::getLogicTypesAndNames(std::vector<LOGIC>& typesOut,
                                  std::vector<QString>& namesOut)
{
   typesOut.clear();
   namesOut.clear();
   
   typesOut.push_back(LOGIC_UNION);   
      namesOut.push_back(convertLogicTypeToName(LOGIC_UNION)); //"Union (OR)");
   typesOut.push_back(LOGIC_INTERSECTION);   
      namesOut.push_back(convertLogicTypeToName(LOGIC_INTERSECTION)); //"Intersection (AND)");
}

/**
 * convert a logic type to name.
 */
QString 
FociSearch::convertLogicTypeToName(const LOGIC lg)
{
   QString s;
   
   switch (lg) {
      case LOGIC_UNION:
         s = "Union";
         break;
      case LOGIC_INTERSECTION:
         s = "Intersection";
         break;
   }
   
   return s;
}

/**
 * convert a logic name to type.
 */
FociSearch::LOGIC 
FociSearch::convertLogicNameToType(const QString& s)
{
   LOGIC lg = LOGIC_UNION;
   
   if (s == "Union") {
      lg = LOGIC_UNION;
   }
   else if (s == "Intersection") {
      lg = LOGIC_INTERSECTION;
   }
   
   return lg;
}

/**
 * get types and names for attributes.
 */
void 
FociSearch::getAttributeTypesAndNames(std::vector<ATTRIBUTE>& typesOut,
                                      std::vector<QString>& namesOut)
{
   typesOut.clear();
   namesOut.clear();

   typesOut.push_back(ATTRIBUTE_ALL);  
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_ALL));
   typesOut.push_back(ATTRIBUTE_FOCUS_AREA);   
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_FOCUS_AREA));
   typesOut.push_back(ATTRIBUTE_STUDY_AUTHORS);  
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_AUTHORS));
   typesOut.push_back(ATTRIBUTE_STUDY_CITATION);
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_CITATION));
   typesOut.push_back(ATTRIBUTE_FOCUS_CLASS);   
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_FOCUS_CLASS));
   typesOut.push_back(ATTRIBUTE_FOCUS_COMMENT);   
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_FOCUS_COMMENT));
   typesOut.push_back(ATTRIBUTE_STUDY_COMMENT);   
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_COMMENT));
   typesOut.push_back(ATTRIBUTE_STUDY_DATA_FORMAT); 
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_DATA_FORMAT));
   typesOut.push_back(ATTRIBUTE_STUDY_DATA_TYPE); 
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_DATA_TYPE));
   typesOut.push_back(ATTRIBUTE_FOCUS_GEOGRAPHY);   
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_FOCUS_GEOGRAPHY));
   typesOut.push_back(ATTRIBUTE_FOCUS_ROI);        
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_FOCUS_ROI));
   typesOut.push_back(ATTRIBUTE_STUDY_KEYWORDS);   
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_KEYWORDS));
   typesOut.push_back(ATTRIBUTE_STUDY_MESH_TERMS);   
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_MESH_TERMS));
   typesOut.push_back(ATTRIBUTE_STUDY_NAME);  
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_NAME));
   typesOut.push_back(ATTRIBUTE_FOCUS_SPATIAL);  
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_FOCUS_SPATIAL));
   typesOut.push_back(ATTRIBUTE_FOCUS_STRUCTURE);
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_FOCUS_STRUCTURE));
   typesOut.push_back(ATTRIBUTE_STUDY_STEREOTAXIC_SPACE); 
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_STEREOTAXIC_SPACE));
   typesOut.push_back(ATTRIBUTE_STUDY_TABLE_HEADER);   
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_TABLE_HEADER));
   typesOut.push_back(ATTRIBUTE_STUDY_TABLE_SUBHEADER);   
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_TABLE_SUBHEADER));
   typesOut.push_back(ATTRIBUTE_STUDY_TITLE);   
      namesOut.push_back(convertAttributeTypeToName(ATTRIBUTE_STUDY_TITLE));
}

/**
 * convert an attribute type to name.
 */
QString 
FociSearch::convertAttributeTypeToName(const ATTRIBUTE att)
{
   QString s;
   
   switch (att) {
      case ATTRIBUTE_ALL:  
         s = "ALL";
         break;
      case ATTRIBUTE_FOCUS_AREA:   
         s = "Area";
         break;
      case ATTRIBUTE_STUDY_AUTHORS:  
         s = "Authors";
         break;
      case ATTRIBUTE_STUDY_CITATION:
         s = "Citation";
         break;
      case ATTRIBUTE_FOCUS_CLASS:   
         s = "Class";
         break;
      case ATTRIBUTE_FOCUS_COMMENT:   
         s = "Comment (Focus)";
         break;
      case ATTRIBUTE_STUDY_COMMENT:   
         s = "Comment (Study)";
         break;
      case ATTRIBUTE_STUDY_DATA_FORMAT: 
         s = "Data Format";
         break;
      case ATTRIBUTE_STUDY_DATA_TYPE: 
         s = "Data Type";
         break;
      case ATTRIBUTE_FOCUS_GEOGRAPHY:   
         s = "Geography";
         break;
      case ATTRIBUTE_FOCUS_ROI:        
         s = "ROI";
         break;
      case ATTRIBUTE_STUDY_KEYWORDS:   
         s = "Keywords";
         break;
      case ATTRIBUTE_STUDY_MESH_TERMS:   
         s = "MESH Terms";
         break;
      case ATTRIBUTE_STUDY_NAME:  
         s = "Name";
         break;
      case ATTRIBUTE_FOCUS_SPATIAL:  
         s = "Spatial";
         break;
      case ATTRIBUTE_FOCUS_STRUCTURE:
         s = "Structure";
         break;
      case ATTRIBUTE_STUDY_STEREOTAXIC_SPACE: 
         s = "Stereotaxic Space";
         break;
      case ATTRIBUTE_STUDY_TABLE_HEADER:   
         s = "Table Header";
         break;
      case ATTRIBUTE_STUDY_TABLE_SUBHEADER:   
         s = "Table Subheader";
         break;
      case ATTRIBUTE_STUDY_TITLE:   
         s = "Title";
         break;
   }
   
   return s;
}

/**
 * convert an attribute name to type.
 */
FociSearch::ATTRIBUTE 
FociSearch::convertAttributeNameToType(const QString& s)
{
   ATTRIBUTE att = ATTRIBUTE_ALL;
   
   if (s == "ALL") {
      att = ATTRIBUTE_ALL;
   }   
   else if (s == "Area") {
      att = ATTRIBUTE_FOCUS_AREA;
   }  
   else if (s == "Authors") {
      att = ATTRIBUTE_STUDY_AUTHORS;
   }   
   else if (s == "Citation") {
      att = ATTRIBUTE_STUDY_CITATION;
   }
   else if (s == "Class") {
      att = ATTRIBUTE_FOCUS_CLASS;
   }   
   else if (s == "Comment (Focus)") {
      att = ATTRIBUTE_FOCUS_COMMENT;
   }   
   else if (s == "Comment (Study)") {
      att = ATTRIBUTE_STUDY_COMMENT;
   } 
   else if (s == "Data Format") {
      att = ATTRIBUTE_STUDY_DATA_FORMAT;
   } 
   else if (s == "Data Type") {
      att = ATTRIBUTE_STUDY_DATA_TYPE;
   }   
   else if (s == "Geography") {
      att = ATTRIBUTE_FOCUS_GEOGRAPHY;
   }        
   else if (s == "ROI") {
      att = ATTRIBUTE_FOCUS_ROI;
   }   
   else if (s == "Keywords") {
      att = ATTRIBUTE_STUDY_KEYWORDS;
   }   
   else if (s == "MESH Terms") {
      att = ATTRIBUTE_STUDY_MESH_TERMS;
   }  
   else if (s == "Name") {
      att = ATTRIBUTE_STUDY_NAME;
   }  
   else if (s == "Spatial") {
      att = ATTRIBUTE_FOCUS_SPATIAL;
   } 
   else if (s == "Stereotaxic Space") {
      att = ATTRIBUTE_STUDY_STEREOTAXIC_SPACE;
   } 
   else if (s == "Structure") {
      att = ATTRIBUTE_FOCUS_STRUCTURE;
   }  
   else if (s == "Table Header") {
      att = ATTRIBUTE_STUDY_TABLE_HEADER;
   }   
   else if (s == "Table Subheader") {
      att = ATTRIBUTE_STUDY_TABLE_SUBHEADER;
   }   
   else if (s == "Title") {
      att = ATTRIBUTE_STUDY_TITLE;
   }

   return att;
}
       
/**
 * get types and names for matching.
 */
void 
FociSearch::getMatchingTypesAndNames(std::vector<MATCHING>& typesOut,
                                     std::vector<QString>& namesOut)
{
   typesOut.clear();
   namesOut.clear();

   typesOut.push_back(MATCHING_ANY_OF);   
      namesOut.push_back(convertMatchingTypeToName(MATCHING_ANY_OF));
   typesOut.push_back(MATCHING_ALL_OF);   
      namesOut.push_back(convertMatchingTypeToName(MATCHING_ALL_OF));
   typesOut.push_back(MATCHING_NONE_OF);   
      namesOut.push_back(convertMatchingTypeToName(MATCHING_NONE_OF));
   typesOut.push_back(MATCHING_EXACT_PHRASE);   
      namesOut.push_back(convertMatchingTypeToName(MATCHING_EXACT_PHRASE));
}

/**
 * convert a matching type to name.
 */
QString 
FociSearch::convertMatchingTypeToName(const MATCHING m)
{
   QString s;
   
   switch (m) {
      case MATCHING_ANY_OF:   
         s = "ANY of";
         break;
      case MATCHING_ALL_OF:   
         s = "ALL of";
         break;
      case MATCHING_NONE_OF:   
         s = "NONE of";
         break;
      case MATCHING_EXACT_PHRASE:   
         s = "EXACT PHRASE";
         break;
   }
   
   return s;
}

/**
 * convert a matching name to type.
 */
FociSearch::MATCHING 
FociSearch::convertMatchingNameToType(const QString& s)
{
   MATCHING m = MATCHING_ANY_OF;
   
   if (s == "ANY of") {
      m = MATCHING_ANY_OF;
   }
   else if (s == "ALL of") {
      m = MATCHING_ALL_OF;
   }
   else if (s == "NONE of") {
      m = MATCHING_NONE_OF;
   }
   else if (s == "EXACT PHRASE") {
      m = MATCHING_EXACT_PHRASE;
   }
   
   return m;
}
       


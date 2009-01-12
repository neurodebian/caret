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

#include <algorithm>
#include <iostream>
#include <set>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include "StringTable.h"
#include "StudyCollectionFile.h"
#include "StudyMetaDataFile.h"
#include "StudyNamePubMedID.h"

/**
 * constructor.
 */
StudyNamePubMedID::StudyNamePubMedID()
{
   clear();
}

/**
 * constructor.
 */
StudyNamePubMedID::StudyNamePubMedID(const QString& nameIn,
                                     const QString& pubMedIDIn,
                                     const QString& mslIDIn)
{
   clear();
   setName(nameIn);
   setPubMedID(pubMedIDIn);
   setMslID(mslIDIn);
}

/**
 * destructor.
 */
StudyNamePubMedID::~StudyNamePubMedID()
{
   clear();
}

/**
 * copy constructor.
 */
StudyNamePubMedID::StudyNamePubMedID(const StudyNamePubMedID& as)
{
   parentStudyMetaData = NULL;
   parentStudyCollection = NULL;
   copyHelper(as);
}

/**
 * assignment operator.
 */
StudyNamePubMedID& 
StudyNamePubMedID::operator=(const StudyNamePubMedID& as)
{
   if (this != &as) {
      copyHelper(as);
   }
   return *this;
}
                  
/**
 * clear the studies.
 */
void 
StudyNamePubMedID::clear()
{
   parentStudyMetaData = NULL;
   parentStudyCollection = NULL;
   name = "";
   pubMedID = "";
   mslID = "";
   setModified();
}
             
/**
 * set the name.
 */
void 
StudyNamePubMedID::setName(const QString& s)
{
   if (name != s) {
      name = s;
      setModified();
   }
}

/**
 * set the PubMed ID.
 */
void 
StudyNamePubMedID::setPubMedID(const QString& s)
{
   if (pubMedID != s) {
      pubMedID = s;
      setModified();
   }
}

/**
 * set the MSL ID.
 */
void 
StudyNamePubMedID::setMslID(const QString& s)
{
   if (mslID != s) {
      mslID = s;
      setModified();
   }
}

/**
 * set parent.
 */
void 
StudyNamePubMedID::setParent(StudyMetaData* parentIn)
{
   parentStudyMetaData = parentIn;
}

/**
 * set parent for study collection.
 */
void 
StudyNamePubMedID::setParent(StudyCollection* parentIn)
{
   parentStudyCollection = parentIn;
}
      
/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
StudyNamePubMedID::copyHelper(const StudyNamePubMedID& s)
{
   StudyMetaData* savedParentStudyMetaData = parentStudyMetaData;
   StudyCollection* savedParentStudyCollection = parentStudyCollection;
   
   clear();
   name = s.name;
   pubMedID = s.pubMedID;
   mslID = s.mslID;
   
   parentStudyMetaData = savedParentStudyMetaData;
   parentStudyCollection = savedParentStudyCollection;
   
   setModified();
}

/**
 * set modified status.
 */
void 
StudyNamePubMedID::setModified()
{
   if (parentStudyMetaData != NULL) {
      parentStudyMetaData->setModified();
   }
   if (parentStudyCollection != NULL) {
      parentStudyCollection->setModified();
   }
}

/**
 * write the data to a string table.
 *
void 
StudyNamePubMedID::writeDataToStringTable(StringTable& table,
                            const QString& stringTableName) throw (FileException)
{
   table.clear();
   
   const int num = getNumberOfStudies();
   if (num <= 0) {
      return;
   }

   int numCols = 0;
   const int nameColumn = numCols++;
   const int pmidColumn = numCols++;
   const int mslidColumn = numCols++;
   
   table.setNumberOfRowsAndColumns(num, numCols, stringTableName);
   
   table.setColumnTitle(nameColumn, "Name");
   table.setColumnTitle(pmidColumn, "Study PubMed ID");
   table.setColumnTitle(mslidColumn, "msl_id");
   
   for (int i = 0; i < num; i++) {
      QString name, pubMedID, mslID;
      getStudyNameAndPubMedID(i, name, pubMedID, mslID);
      table.setElement(i, nameColumn, name);
      table.setElement(i, pmidColumn, pubMedID);
      table.setElement(i, mslidColumn, mslID);
   }
}                                  
*/
/**
 * read the data from a StringTable.
 *
void 
StudyNamePubMedID::readDataFromStringTable(const StringTable& st) throw (FileException)
{
   if (st.getTableTitle() != "Name_PMID") {
      throw FileException("String table for StudyMetaData does not have name Study Metadata");
   }
   StudyMetaData* savedParentStudyMetaData = parentStudyMetaData;
   StudyCollectionFile* savedParentStudyCollectionFile = parentStudyCollectionFile;
   
   clear();
   
   int nameColumn = -1;
   int pmidColumn = -1;
   int mslidColumn = -1;
   
   const int numCols = st.getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString name = st.getColumnTitle(i).toLower();
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

   const int numItems = st.getNumberOfRows();
   for (int i = 0; i < numItems; i++) {
      QString name, pmid, msl;
      
      if (nameColumn >= 0) {
        name = st.getElement(i, nameColumn);
      }
      if (pmidColumn >= 0) {
         pmid = st.getElement(i, pmidColumn);
      }
      if (mslidColumn >= 0) {
         msl = st.getElement(i, mslidColumn);
      }

      if (name.isEmpty() == false) {
         addStudyNameAndPubMedID(name, pmid, msl);
      }
   }
   
   parentStudyMetaData = savedParentStudyMetaData;
   parentStudyCollectionFile = savedParentStudyCollectionFile;
   
   setModified();
}
*/

/**
 * called to read from an XML structure.
 */
void 
StudyNamePubMedID::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != "StudyNamePubMedID") {
      QString msg("Incorrect element type passed to StudyNamePubMedID::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "name") {
            name = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "pubMedID") {
            pubMedID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "mslID") {
            mslID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else {
            std::cout << "WARNING: unrecognized StudyNamePubMedID element: "
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
StudyNamePubMedID::writeXML(QDomDocument& xmlDoc,
                            QDomElement& parentElement) const throw (FileException)
{
   //
   // Create element for this instance's data
   //
   QDomElement element = xmlDoc.createElement("StudyNamePubMedID");
   
   //
   // Set data elements
   //
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    element, 
                                    "name", 
                                    name);
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    element, 
                                    "pubMedID", 
                                    pubMedID);
   AbstractFile::addXmlCdataElement(xmlDoc, 
                                    element, 
                                    "mslID", 
                                    mslID);

   //
   // Add class instance's data to the parent
   //
   parentElement.appendChild(element);
}


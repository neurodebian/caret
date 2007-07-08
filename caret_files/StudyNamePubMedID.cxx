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
#include <set>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include "StringTable.h"
#include "StudyMetaAnalysisFile.h"
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
   parentStudyMetaAnalysisFile = NULL;
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
   parentStudyMetaAnalysisFile = NULL;
   studyData.clear();
}
            
/**
 * set the study names and PubMed IDs as a string.
 */
void 
StudyNamePubMedID::setAll(const QString& s)
{
   if (s != getAll()) {
      //
      // Clear existing data
      //
      studyData.clear();
      
      const char cellSeparator = ',';
      const char rowSeparator = ';';
      
      //
      // Each row contains name and optional pubMedID
      //
      QStringList sl1 = s.split(rowSeparator, QString::SkipEmptyParts);
      for (int i = 0; i < sl1.size(); i++) {
         QString s = sl1.at(i);
         QStringList sl2 = s.split(cellSeparator, QString::SkipEmptyParts);
         if (sl2.size() > 0) {
            const QString name = sl2.at(0).trimmed();
            QString id;
            if (sl2.size() > 1) {
               id = sl2.at(1).trimmed();
            }
            if (name.isEmpty() == false) {
               addStudyNameAndPubMedID(name, id);
            }
         }
      }
      setModified();
   }
}

/**
 * get the study names and PubMed IDs as a string.
 */
QString 
StudyNamePubMedID::getAll() const
{
   //
   // Each study is on a line with name and PubMedID separated by a command and 
   // a semicolon after the PubMedID
   //
   QString s;
   for (int i = 0; i < getNumberOfStudies(); i++) {
      QString name, id;
      getStudyNameAndPubMedID(i, name, id);
      if (name.isEmpty() == false) {
         if (s.isEmpty() == false) {
            s += "\n";
         }
         s += name;
         s += ", ";
         s += id;
         s += ";";
      }
   }
   return s;
}

/**
 * get a study name and PubMed ID.
 */
void 
StudyNamePubMedID::getStudyNameAndPubMedID(const int indx,
                                           QString& nameOut,
                                           QString& pubMedIDOut) const
{
   nameOut = "";
   pubMedIDOut = "";
   
   if ((indx >= 0) && (indx < getNumberOfStudies())) {
      nameOut = studyData[indx].name;
      pubMedIDOut = studyData[indx].pmid;
   }
}

/**
 * set a study name and PubMed ID.
 */
void 
StudyNamePubMedID::setStudyNameAndPubMedID(const int indx,
                                           const QString& nameIn,
                                           const QString& pubMedIDIn)
{
   if ((indx >= 0) && (indx < getNumberOfStudies())) {
      if ((nameIn != studyData[indx].name) ||
          (pubMedIDIn != studyData[indx].pmid)) {
         studyData[indx].name = nameIn;
         studyData[indx].pmid = pubMedIDIn;
         setModified();
      }
   }
}
                             
/**
 * add a study name and PubMed ID.
 */
void 
StudyNamePubMedID::addStudyNameAndPubMedID(const QString& nameIn,
                                           const QString& pubMedIDIn)
{
   studyData.push_back(StudyNamePMID(nameIn, pubMedIDIn));
   setModified();
}
                             
/**
 * remove a study.
 */
void 
StudyNamePubMedID::removeStudy(const int indx)
{
   studyData.erase(studyData.begin() + indx);
   setModified();
}

/**
 * remove studies.
 */
void 
StudyNamePubMedID::removeStudiesByIndex(const std::vector<int>& studyIndicesIn)
{
   //
   // Sort into reverse order
   //
   std::set<int> studyIndicesSet(studyIndicesIn.begin(), studyIndicesIn.end());
   std::vector<int> studyIndices(studyIndicesSet.begin(), studyIndicesSet.end());
   std::reverse(studyIndices.begin(), studyIndices.end());
   
   for (unsigned int i = 0; i < studyIndices.size(); i++) {
      removeStudy(studyIndices[i]);
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
 * copy helper used by copy constructor and assignment operator.
 */
void 
StudyNamePubMedID::copyHelper(const StudyNamePubMedID& as)
{
   StudyMetaData* savedParentStudyMetaData = parentStudyMetaData;
   StudyMetaAnalysisFile* savedParentStudyMetaAnalysisFile = parentStudyMetaAnalysisFile;
   
   clear();
   studyData = as.studyData;
   
   parentStudyMetaData = savedParentStudyMetaData;
   parentStudyMetaAnalysisFile = savedParentStudyMetaAnalysisFile;
   
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
   if (parentStudyMetaAnalysisFile != NULL) {
      parentStudyMetaAnalysisFile->setModified();
   }
}

/**
 * read the data from a StringTable.
 */
void 
StudyNamePubMedID::readDataFromStringTable(const StringTable& st) throw (FileException)
{
   if (st.getTableTitle() != "Name_PMID") {
      throw FileException("String table for StudyMetaData does not have name Study Metadata");
   }
   StudyMetaData* savedParentStudyMetaData = parentStudyMetaData;
   StudyMetaAnalysisFile* savedParentStudyMetaAnalysisFile = parentStudyMetaAnalysisFile;
   
   clear();
   
   int nameColumn = -1;
   int pmidColumn = -1;
   
   const int numCols = st.getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString name = st.getColumnTitle(i).toLower();
      if (name == "name") {
         nameColumn = i;
      }
      else if (name == "study pubmed id") {
         pmidColumn = i;
      }
   }

   const int numItems = st.getNumberOfRows();
   for (int i = 0; i < numItems; i++) {
      QString name, pmid;
      
      if (nameColumn >= 0) {
        name = st.getElement(i, nameColumn);
      }
      if (pmidColumn >= 0) {
         pmid = st.getElement(i, pmidColumn);
      }

      if (name.isEmpty() == false) {
         addStudyNameAndPubMedID(name, pmid);
      }
   }
   
   parentStudyMetaData = savedParentStudyMetaData;
   parentStudyMetaAnalysisFile = savedParentStudyMetaAnalysisFile;
   
   setModified();
}
                  
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
   if ((elem.tagName() != "AssociatedStudies") &&
       (elem.tagName() != "StudyNamePubMedID")) {
      QString msg("Incorrect element type passed to StudyNamePubMedID::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   const QString s = AbstractFile::getXmlElementFirstChildAsString(elem);
   setAll(s);
}

/**
 * called to write to an XML structure.
 */
void 
StudyNamePubMedID::writeXML(QDomDocument& xmlDoc,
                            QDomElement& parentElement) const throw (FileException)
{
   const QString s = getAll();
   AbstractFile::addXmlCdataElement(xmlDoc, parentElement, "StudyNamePubMedID", s);
}


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

/*
 * Hierarchy of classes ("has a" classes are indented)
 *   StudyMetaData
 *      Figure
 *         Panel
 *      Table
 *         SubHeader
 *      Page Reference
 *         SubHeader
 */

#include <algorithm>
#include <iostream>
#include <set>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>

#include "AbstractFile.h"
#include "CellFile.h"
#include "CellProjectionFile.h"
#include "CellStudyInfo.h"
#include "CommaSeparatedValueFile.h"
#include "PubMedArticleFile.h"
#include "SpecFile.h"
#include "StudyMetaAnalysisFile.h"
#include "StudyMetaDataFile.h"
#include "StringTable.h"
#include "VocabularyFile.h"

static const QString boldStart("<B>");
static const QString boldEnd("</B>");
static const QString newLine("\n");
   

/**
 * Constructor
 */
StudyMetaData::StudyMetaData()
{
   clear();
}

/**
 * constructor (from old CellStudyInfo).
 */
StudyMetaData::StudyMetaData(const CellStudyInfo& csi)
{
   clear();
   setAuthors(csi.getAuthors());
   setTitle(csi.getTitle());
   setCitation(csi.getCitation());
   setComment(csi.getComment());
   setKeywords(csi.getKeywords());
   setPartitioningSchemeAbbreviation(csi.getPartitioningSchemeAbbreviation());
   setPartitioningSchemeFullName(csi.getPartitioningSchemeFullName());
   setStereotaxicSpace(csi.getStereotaxicSpace());
   setDocumentObjectIdentifier(csi.getURL());
}

/**
 * constructor.
 */
StudyMetaData::StudyMetaData(const StudyMetaAnalysisFile* smaf)
{
   clear();
   setName(smaf->getName());
   setTitle(smaf->getTitle());
   setAuthors(smaf->getAuthors());
   setCitation(smaf->getCitation());
   setDocumentObjectIdentifier(smaf->getDoiURL());
   metaAnalysisStudies = *(smaf->getMetaAnalysisStudies());
   setMetaAnalysisFlag(smaf->getMetaAnalysisStudies()->getNumberOfStudies() > 0);
}
      
/**
 * Destructor
 */
StudyMetaData::~StudyMetaData()
{
   clear();
}

/**
 * copy constructor.
 */
StudyMetaData::StudyMetaData(const StudyMetaData& smd)
{
   parentStudyMetaDataFile = NULL;
   copyHelper(smd);
}

/**
 * assignment operator.
 */
StudyMetaData& 
StudyMetaData::operator=(const StudyMetaData& smd)
{
   if (this != &smd) {
      copyHelper(smd);
   }
   return *this;
}

/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
StudyMetaData::copyHelper(const StudyMetaData& smd)
{
   StudyMetaDataFile* savedParentStudyMetaDataFile = parentStudyMetaDataFile;
   clear();
   
   authors = smd.authors;
   citation = smd.citation;
   comment = smd.comment;
   documentObjectIdentifier = smd.documentObjectIdentifier;
   keywords = smd.keywords;
   medicalSubjectHeadings = smd.medicalSubjectHeadings;
   name = smd.name;
   partitioningSchemeAbbreviation = smd.partitioningSchemeAbbreviation;
   partitioningSchemeFullName = smd.partitioningSchemeFullName;
   projectID = smd.projectID;
   pubMedID = smd.pubMedID;
   stereotaxicSpace = smd.stereotaxicSpace;
   stereotaxicSpaceDetails = smd.stereotaxicSpaceDetails;
   title = smd.title;
   metaAnalysisFlag = smd.metaAnalysisFlag;
   
   //
   // DO NOT COPY THESE MEMBERS !!!
   //
   // 
   provenanceDateAndTimeStamps = "";
   studyDataModifiedFlag = false;
   
   //
   // Copy children
   //
   const int numTables = smd.getNumberOfTables();
   for (int i = 0; i < numTables; i++) {
      addTable(new Table(*(smd.getTable(i))));
   }
   
   const int numFigures = smd.getNumberOfFigures();
   for (int i = 0; i < numFigures; i++) {
      addFigure(new Figure(*(smd.getFigure(i))));
   }
   
   const int numPageRefs = smd.getNumberOfPageReferences();
   for (int i = 0; i < numPageRefs; i++) {
      addPageReference(new PageReference(*(smd.getPageReference(i))));
   }
   
   metaAnalysisStudies = smd.metaAnalysisStudies;
   
   parentStudyMetaDataFile = savedParentStudyMetaDataFile;
   setModified();
}

/**
 * Clear the metadata
 */
void
StudyMetaData::clear()
{
   parentStudyMetaDataFile = NULL;
   authors = "";
   citation = "";
   comment = "";
   documentObjectIdentifier = "";
   keywords = "";
   medicalSubjectHeadings = "";
   name = "";
   partitioningSchemeAbbreviation = "";
   partitioningSchemeFullName = "";
   projectID = getProjectIDInPubMedIDPrefix()
               + AbstractFile::generateUniqueNumericTimeStampAsString();
   provenanceDateAndTimeStamps = "";
   pubMedID = projectID;  // user can override later
   stereotaxicSpace = "";
   stereotaxicSpaceDetails = "";
   studyDataModifiedFlag = false;
   title = "";
   metaAnalysisFlag = false;
   
   for (unsigned int i = 0; i < tables.size(); i++) {
      delete tables[i];
   }
   tables.clear();
   for (unsigned int i = 0; i < figures.size(); i++) {
      delete figures[i];
   }
   figures.clear();
   for (unsigned int i = 0; i < pageReferences.size(); i++) {
      delete pageReferences[i];
   }
   pageReferences.clear();
   metaAnalysisStudies.clear();
}

/**
 * add a figure.
 */
void 
StudyMetaData::addFigure(Figure* f)
{
   f->setParent(this);
   figures.push_back(f);
   setModified();
}
      
/**
 * delete a figure.
 */
void 
StudyMetaData::deleteFigure(const int indx)
{
   delete figures[indx];
   figures.erase(figures.begin() + indx);
   setModified();
}
      
/**
 * delete a figure.
 */
void 
StudyMetaData::deleteFigure(const Figure* figure)
{
   for (int i = 0; i < getNumberOfFigures(); i++) {
      if (getFigure(i) == figure) {
         deleteFigure(i);
         break;
      }
   }
}
      
/**
 * get a figure by its figure number.
 */
StudyMetaData::Figure* 
StudyMetaData::getFigureByFigureNumber(const QString& figureNumber)
{
   for (int i = 0; i < getNumberOfFigures(); i++) {
      Figure* f = getFigure(i);
      if (f->getNumber() == figureNumber) {
         return f;
      }
   }
   return NULL;
}

/**
 * get a figure by its figure number (const method).
 */
const StudyMetaData::Figure* 
StudyMetaData::getFigureByFigureNumber(const QString& figureNumber) const
{
   for (int i = 0; i < getNumberOfFigures(); i++) {
      const Figure* f = getFigure(i);
      if (f->getNumber() == figureNumber) {
         return f;
      }
   }
   return NULL;
}
      
/**
 * add a table.
 */
void 
StudyMetaData::addTable(Table* t)
{
   t->setParent(this);
   tables.push_back(t);
   setModified();
}

/**
 * delete a table.
 */
void 
StudyMetaData::deleteTable(const int indx)
{
   delete tables[indx];
   tables.erase(tables.begin() + indx);
   setModified();
}
      
/**
 * delete a table.
 */
void 
StudyMetaData::deleteTable(const Table* table)
{
   for (int i = 0; i < getNumberOfTables(); i++) {
      if (getTable(i) == table) {
         deleteTable(i);
         break;
      }
   }
}
      
/**
 * get a table by its table number.
 */
StudyMetaData::Table* 
StudyMetaData::getTableByTableNumber(const QString& tableNumber)
{
   for (int i = 0; i < getNumberOfTables(); i++) {
      Table* t = getTable(i);
      if (t->getNumber() == tableNumber) {
         return t;
      }
   }
   return NULL;
}

/**
 * get a table by its table number (const method).
 */
const StudyMetaData::Table* 
StudyMetaData::getTableByTableNumber(const QString& tableNumber) const
{
   for (int i = 0; i < getNumberOfTables(); i++) {
      const Table* t = getTable(i);
      if (t->getNumber() == tableNumber) {
         return t;
      }
   }
   return NULL;
}

/**
 * get the last provenance save date.
 */
QString 
StudyMetaData::getLastProvenanceSaveDate() const
{
   QString s;
   
   const QStringList sl = provenanceDateAndTimeStamps.split(";", QString::SkipEmptyParts);
   if (sl.count() > 0) {
      s = sl.at(0);
   }
   
   return s;
}      
      
/**
 * add a page reference.
 */
void 
StudyMetaData::addPageReference(PageReference* pr)
{
   pr->setParent(this);
   pageReferences.push_back(pr);
   setModified();
}

/**
 * delete a page reference.
 */
void 
StudyMetaData::deletePageReference(const int indx)
{
   delete pageReferences[indx];
   pageReferences.erase(pageReferences.begin() + indx);
   setModified();
}

/**
 * delete a page reference.
 */
void 
StudyMetaData::deletePageReference(const PageReference* pr)
{
   for (int i = 0; i < getNumberOfPageReferences(); i++) {
      if (getPageReference(i) == pr) {
         deletePageReference(i);
         break;
      }
   }
}

/**
 * get a page reference by its page number.
 */
StudyMetaData::PageReference* 
StudyMetaData::getPageReferenceByPageNumber(const QString& pageNumber)
{
   for (int i = 0; i < getNumberOfPageReferences(); i++) {
      PageReference* pr = getPageReference(i);
      if (pr->getPageNumber() == pageNumber) {
         return pr;
      }
   }
   return NULL;
}

/**
 * get a page reference by its page number (const method).
 */
const StudyMetaData::PageReference* 
StudyMetaData::getPageReferenceByPageNumber(const QString& pageNumber) const
{
   for (int i = 0; i < getNumberOfPageReferences(); i++) {
      const PageReference* pr = getPageReference(i);
      if (pr->getPageNumber() == pageNumber) {
         return pr;
      }
   }
   return NULL;
}
      
/**
 * equality operator.
 */
bool 
StudyMetaData::operator==(const StudyMetaData& cci) const
{
   //
   // Note: do not compare pubMedID or projectID
   //
   const bool theSame = 
          ((authors  == cci.authors) &&
           (citation == cci.citation) &&
           (comment  == cci.comment) &&
           (documentObjectIdentifier == cci.documentObjectIdentifier) &&
           (keywords == cci.keywords) &&
           (medicalSubjectHeadings == cci.medicalSubjectHeadings) &&
           (name == cci.name) &&
           (partitioningSchemeAbbreviation == cci.partitioningSchemeAbbreviation) &&
           (partitioningSchemeFullName == cci.partitioningSchemeFullName) &&
           (stereotaxicSpace == cci.stereotaxicSpace) &&
           (stereotaxicSpaceDetails == cci.stereotaxicSpaceDetails) &&
           (title == cci.title));

   return theSame;
}

/**
 * called to read from an XML structure.
 */
void 
StudyMetaData::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != "StudyMetaData") {
      QString msg("Incorrect element type passed to StudyMetaData::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QString oldURL;
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "authors") {
            authors = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "citation") {
            citation = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "comment") {
            comment = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "documentObjectIdentifier") {
            documentObjectIdentifier = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "keywords") {
            keywords = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "mesh") {
            medicalSubjectHeadings = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "name") {
            name = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "partitioningSchemeAbbreviation") {
            partitioningSchemeAbbreviation = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "partitioningSchemeFullName") {
            partitioningSchemeFullName = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "projectID") {
            projectID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "provenanceDateAndTimeStamps") {
            provenanceDateAndTimeStamps = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "pubMedID") {
            pubMedID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "stereotaxicSpace") {
            stereotaxicSpace = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "stereotaxicSpaceDetails") {
            stereotaxicSpaceDetails = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "title") {
            title = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "url") {  // obsolete
            oldURL = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "metaAnalysisFlag") {
            if (AbstractFile::getXmlElementFirstChildAsString(elem) == "true") {
               metaAnalysisFlag = true;
            }
            else {
               metaAnalysisFlag = false;
            }
         }
         else if (elem.tagName() == "StudyMetaDataTable") {
            Table* t = new Table;
            t->readXML(node);
            addTable(t);
         }
         else if (elem.tagName() == "StudyMetaDataFigure") {
            Figure* f = new Figure;
            f->readXML(node);
            addFigure(f);
         }
         else if (elem.tagName() == "StudyMetaDataPageReference") {
            PageReference* pr = new PageReference;
            pr->readXML(node);
            addPageReference(pr);
         }
         else if ((elem.tagName() == "AssociatedStudies") ||
                  (elem.tagName() == "StudyNamePubMedID")) {
            metaAnalysisStudies.readXML(node);
         }
         else {
            std::cout << "WARNING: unrecognized StudyMetaData element ignored: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
      }
      node = node.nextSibling();
   }
   
   //
   // Older versions of file have both DOI and URL but DOI takes precedence
   //
   if (documentObjectIdentifier.isEmpty()) {
      documentObjectIdentifier = oldURL;
   }
}
      
/**
 * called to write to an XML structure.
 */
void 
StudyMetaData::writeXML(QDomDocument& xmlDoc,
                   QDomElement&  parentElement) const throw (FileException)
{
   //
   // Update provenance if modified flag is set
   //
   if (studyDataModifiedFlag) {
      provenanceDateAndTimeStamps = AbstractFile::generateDateAndTimeStamp()
                                  + ";"
                                  + provenanceDateAndTimeStamps;
      studyDataModifiedFlag = false;
   }
   
   //
   // Create the element for this class instance's data
   //
   QDomElement studyElement = xmlDoc.createElement("StudyMetaData");

   //
   // Add the study metadata
   //
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "authors", authors);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "citation", citation);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "comment", comment);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "documentObjectIdentifier", documentObjectIdentifier);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "keywords", keywords);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "mesh", medicalSubjectHeadings);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "name", name);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "partitioningSchemeAbbreviation", partitioningSchemeAbbreviation);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "partitioningSchemeFullName", partitioningSchemeFullName);
   AbstractFile::addXmlCdataElement( xmlDoc, studyElement, "projectID", projectID);
   AbstractFile::addXmlCdataElement( xmlDoc, studyElement, "provenanceDateAndTimeStamps", provenanceDateAndTimeStamps);
   AbstractFile::addXmlCdataElement( xmlDoc, studyElement, "pubMedID", pubMedID);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "stereotaxicSpace", stereotaxicSpace);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "stereotaxicSpaceDetails", stereotaxicSpaceDetails);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "title", title);
   if (metaAnalysisFlag) {
      AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "metaAnalysisFlag", "true");
   }
   else {
      AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "metaAnalysisFlag", "false");
   }

   //
   // Add tables
   //
   for (int i = 0; i < getNumberOfTables(); i++) {
      tables[i]->writeXML(xmlDoc, studyElement);
   }
   
   //
   // Add figures
   //
   for (int i = 0; i < getNumberOfFigures(); i++) {
      figures[i]->writeXML(xmlDoc, studyElement);
   }
   
   //
   // Add page references
   //
   for (int i = 0; i < getNumberOfPageReferences(); i++) {
      pageReferences[i]->writeXML(xmlDoc, studyElement);
   }
   
   //
   // Add meta-analysis studies
   //
   metaAnalysisStudies.writeXML(xmlDoc, studyElement);
   
   //
   // Add to parent
   //
   parentElement.appendChild(studyElement);
}

/**
 * write the data into a comma separated value file
 */
void 
StudyMetaData::writeDataIntoCommaSeparatedValueFile(const std::vector<StudyMetaData*>& studyInfo,
                                         CommaSeparatedValueFile& csvf) throw (FileException)
{
   const int numInfo = static_cast<int>(studyInfo.size());
   if (numInfo <= 0) {
      return;
   }
   
   //
   // Column numbers for info
   //
   int numCols = 0;
   const int authorsCol = numCols++;
   const int citationCol = numCols++;
   const int commentCol = numCols++;
   const int doiCol = numCols++;
   const int keywordsCol = numCols++;
   const int meshCol = numCols++;
   const int nameCol = numCols++;
   const int partSchemeAbbrevCol = numCols++;
   const int partSchemeFullNameCol = numCols++;
   const int projectIDCol = numCols++;
   const int provenanceCol = numCols++;
   const int pubMedCol = numCols++;
   const int spaceCol = numCols++;
   const int spaceDetailsCol = numCols++;
   const int titleCol = numCols++;
   
   //
   // Load the table
   //
   StringTable* st = new StringTable(numInfo, numCols, "Study Metadata");
   st->setColumnTitle(authorsCol, "Authors");
   st->setColumnTitle(citationCol, "Citation");
   st->setColumnTitle(commentCol, "Comment");
   st->setColumnTitle(doiCol, "Document Object Identifier");
   st->setColumnTitle(keywordsCol, "Keywords");
   st->setColumnTitle(meshCol, "Medical Subject Headings");
   st->setColumnTitle(nameCol, "Name");
   st->setColumnTitle(partSchemeAbbrevCol, "Partitioning Scheme Abbreviation");
   st->setColumnTitle(partSchemeFullNameCol, "Partitioning Scheme Full Name");
   st->setColumnTitle(projectIDCol, "Project ID");
   st->setColumnTitle(provenanceCol, "Provenance");
   st->setColumnTitle(pubMedCol, "PubMed ID");
   st->setColumnTitle(spaceCol, "Stereotaxic Space");
   st->setColumnTitle(spaceDetailsCol, "Stereotaxic Space Details");
   st->setColumnTitle(titleCol, "Title");
   
   for (int i = 0; i < numInfo; i++) {
      const StudyMetaData& smd = *(studyInfo[i]);
      //
      // Update provenance if modified flag is set
      //
      if (smd.studyDataModifiedFlag) {
         smd.provenanceDateAndTimeStamps = AbstractFile::generateDateAndTimeStamp()
                                     + ";"
                                     + smd.provenanceDateAndTimeStamps;
         smd.studyDataModifiedFlag = false;
      }

      st->setElement(i, authorsCol, smd.getAuthors());
      st->setElement(i, citationCol, smd.getCitation());
      st->setElement(i, commentCol, smd.getComment());
      st->setElement(i, doiCol, smd.getDocumentObjectIdentifier());
      st->setElement(i, keywordsCol, smd.getKeywords());
      st->setElement(i, meshCol, smd.getMedicalSubjectHeadings());
      st->setElement(i, nameCol, smd.getName());
      st->setElement(i, partSchemeAbbrevCol, smd.getPartitioningSchemeAbbreviation());
      st->setElement(i, partSchemeFullNameCol, smd.getPartitioningSchemeFullName());
      st->setElement(i, projectIDCol, smd.getProjectID());
      st->setElement(i, provenanceCol, smd.getProvenance());
      st->setElement(i, pubMedCol, smd.getPubMedID());
      st->setElement(i, spaceCol, smd.getStereotaxicSpace());
      st->setElement(i, spaceDetailsCol, smd.getStereotaxicSpaceDetails());
      st->setElement(i, titleCol, smd.getTitle());
   }
   
   csvf.addDataSection(st);
}

/**
 * read the data from a StringTable.
 */
void 
StudyMetaData::readDataFromStringTable(std::vector<StudyMetaData*>& studyMetaData,
                                        const StringTable& st) throw (FileException)
{
   if (st.getTableTitle() != "Study Metadata") {
      throw FileException("String table for StudyMetaData does not have name Study Metadata");
   }
   
   studyMetaData.clear();

   int authorsCol = -1;
   int citationCol = -1;
   int commentCol = -1;
   int doiCol = -1;
   int keywordsCol = -1;
   int meshCol = -1;
   int nameCol = -1;
   int partitioningSchemeAbbrevCol = -1;
   int partitioningSchemeFullNameCol = -1;
   int projectIDCol = -1;
   int provenanceCol = -1;
   int pubMedCol = -1;
   int spaceCol = -1;
   int spaceDetailsCol = -1;
   int titleCol = -1;
   int urlCol = -1;
   
   const int numCols = st.getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString name = st.getColumnTitle(i).toLower();
      if (name == "authors") {
         authorsCol = i;
      }
      else if (name == "citation") {
         citationCol = i;
      }
      else if (name == "comment") {
         commentCol = i;
      }
      else if (name == "document object identifier") {
         doiCol = i;
      }
      else if (name == "keywords") {
         keywordsCol = i;
      }
      else if (name == "medical subject headings") {
         meshCol = i;
      }
      else if (name == "name") {
         nameCol = i;
      }
      else if (name == "partitioning scheme abbreviation") {
         partitioningSchemeAbbrevCol = i;
      }
      else if (name == "partitioning scheme full name") {
         partitioningSchemeFullNameCol = i;
      }
      else if (name == "project id") {
         projectIDCol = i;
      }
      else if (name == "provenance") {
         provenanceCol = i;
      }
      else if (name == "pubmed id") {
         pubMedCol = i;
      }
      else if (name == "stereotaxic space") {
         spaceCol = i;
      }
      else if (name == "stereotaxic space details") {
         spaceDetailsCol = i;
      }
      else if (name == "title") {
         titleCol = i;
      }
      else if (name == "url") {
         urlCol = i;
      }
      else {
            std::cout << "WARNING: unrecognized StudyMetaData element in string table ignored: "
                      << name.toAscii().constData()
                      << std::endl;
      }
   }
   
   const int numItems = st.getNumberOfRows();
   for (int i = 0; i < numItems; i++) {
      StudyMetaData* smd = new StudyMetaData;
      
      if (authorsCol >= 0) {
        smd->setAuthors(st.getElement(i, authorsCol));
      }
      if (citationCol >= 0) {
         smd->setCitation(st.getElement(i, citationCol));
      }
      if (commentCol >= 0) {
         smd->setComment(st.getElement(i, commentCol));
      }
      if (doiCol >= 0) {
         smd->setDocumentObjectIdentifier(st.getElement(i, doiCol));
      }
      if (keywordsCol >= 0) {
         smd->setKeywords(st.getElement(i, keywordsCol));
      }
      if (meshCol >= 0) {
         smd->setMedicalSubjectHeadings(st.getElement(i, meshCol));
      }
      if (nameCol >= 0) {
         smd->setName(st.getElement(i, nameCol));
      }
      if (partitioningSchemeAbbrevCol >= 0) {
         smd->setPartitioningSchemeAbbreviation(st.getElement(i, partitioningSchemeAbbrevCol));
      }
      if (partitioningSchemeFullNameCol >= 0) {
         smd->setPartitioningSchemeFullName(st.getElement(i, partitioningSchemeFullNameCol));
      }
      if (projectIDCol >= 0) {
         smd->setProjectID(st.getElement(i, projectIDCol));
      }
      if (provenanceCol >= 0) {
         smd->setProvenance(st.getElement(i, projectIDCol));
      }
      if (pubMedCol >= 0) {
         smd->setPubMedID(st.getElement(i, pubMedCol));
      }
      if (spaceCol >= 0) {
         smd->setStereotaxicSpace(st.getElement(i, spaceCol));
      }
      if (spaceDetailsCol >= 0) {
         smd->setStereotaxicSpaceDetails(st.getElement(i, spaceDetailsCol));
      }
      if (titleCol >= 0) {
         smd->setTitle(st.getElement(i, titleCol));
      }
      if (urlCol >= 0) {
         //
         // Older files had both DOI and URL but DOI takes precedence
         //
         const QString oldURL = st.getElement(i, urlCol);
         if (smd->getDocumentObjectIdentifier().isEmpty()) {
            smd->setDocumentObjectIdentifier(oldURL);
         }
      }
      
      
      studyMetaData.push_back(smd);
   }
}      

/**
 * retrieve data from PubMed using PubMed ID.
 */
void 
StudyMetaData::updateDataFromPubMedDotComUsingPubMedID() throw (FileException)
{
   //
   // Make sure a PubMed ID was entered
   //
   if (pubMedID.isEmpty()) {
      throw FileException("The PubMed ID has not been entered.");
      return;
   }
   
   //
   // Get the article
   //
   PubMedArticleFile pubMedFile;
   pubMedFile.retrieveArticleWithPubMedID(pubMedID); // may throw FileException

   if (pubMedFile.getArticleTitle().isEmpty() == false) {
      setTitle(pubMedFile.getArticleTitle());
   }
   if (pubMedFile.getAuthors().isEmpty() == false) {
      setAuthors(pubMedFile.getAuthors());
   }
   if (pubMedFile.getJournalTitle().isEmpty() == false) {
      setCitation(pubMedFile.getJournalTitle());
   }
   if (pubMedFile.getDocumentObjectIdentifier().isEmpty() == false) {
      setDocumentObjectIdentifier(pubMedFile.getDocumentObjectIdentifier());
   }
   if (pubMedFile.getAbstractText().isEmpty() == false) {
      setComment(pubMedFile.getAbstractText());
   }
   if (pubMedFile.getMedicalSubjectHeadings().isEmpty() == false) {
      setMedicalSubjectHeadings(pubMedFile.getMedicalSubjectHeadings());
   }
}
      
/**
 * see if the PubMed ID is actually a Project ID.
 */
bool 
StudyMetaData::getPubMedIDIsAProjectID() const
{
   return (pubMedID.startsWith(getProjectIDInPubMedIDPrefix()));
}
      
/**
 * set the PubMed ID.
 */
void 
StudyMetaData::setPubMedID(const QString& pmid) 
{ 
   if (pubMedID != pmid) {
      pubMedID = pmid; 
      setModified(); 
   }
}

/**
 * set the project id.
 */
void 
StudyMetaData::setProjectID(const QString& pid) 
{ 
   if (projectID != pid) {
      projectID = pid; 
      setModified(); 
   }
}

/**
 * set keywords.
 */
void 
StudyMetaData::setKeywords(const QString& s) 
{ 
   if (keywords != s) {
      keywords = s; 
      setModified(); 
   }
}

/**
 * set medical subject headings.
 */
void 
StudyMetaData::setMedicalSubjectHeadings(const QString& s)
{
   if (medicalSubjectHeadings != s) {
      medicalSubjectHeadings = s; 
      setModified(); 
   }
}

/**
 * set name.
 */
void 
StudyMetaData::setName(const QString& s)
{
   if (name != s) {
      name = s; 
      setModified(); 
   }
}
      
/**
 * set title.
 */
void 
StudyMetaData::setTitle(const QString& s) 
{ 
   if (title != s) {
      title = s; 
      setModified(); 
   }
}

/**
 * set meta-analysis flag.
 */
void 
StudyMetaData::setMetaAnalysisFlag(const bool b)
{
   if (b != metaAnalysisFlag) {
      metaAnalysisFlag = b;
      setModified();
   }
}

/**
 * set authors.
 */
void 
StudyMetaData::setAuthors(const QString& s) 
{ 
   if (authors != s) {
      authors = s; 
      setModified(); 
   }
}

/**
 * set citation.
 */
void 
StudyMetaData::setCitation(const QString& s) 
{ 
   if (citation != s) {
      citation = s; 
      setModified(); 
   }
}

/**
 * set stereotaxic space.
 */
void 
StudyMetaData::setStereotaxicSpace(const QString& s) 
{ 
   if (stereotaxicSpace != s) {
      stereotaxicSpace = s; 
      setModified(); 
   }
}

/**
 * set stereotaxic space details.
 */
void 
StudyMetaData::setStereotaxicSpaceDetails(const QString& s) 
{ 
   if (stereotaxicSpaceDetails != s) {
      stereotaxicSpaceDetails = s; 
      setModified(); 
   }
}

/**
 * set comment.
 */
void 
StudyMetaData::setComment(const QString& s) 
{ 
   if (comment != s) {
      comment = s; 
      setModified(); 
   }
}

/**
 * set partitioning scheme abbreviation.
 */
void 
StudyMetaData::setPartitioningSchemeAbbreviation(const QString& s) 
{ 
   if (partitioningSchemeAbbreviation != s) {
      partitioningSchemeAbbreviation = s; 
      setModified(); 
   }
}

/**
 * set the provenance.
 */
void 
StudyMetaData::setProvenance(const QString& p)
{
   provenanceDateAndTimeStamps = p;
   
   //
   // NOTE: DO NOT SET MODIFIED STATUS
   //
}
      
/**
 * set partitioning scheme full name.
 */
void 
StudyMetaData::setPartitioningSchemeFullName(const QString& s) 
{ 
   if (partitioningSchemeFullName != s) {
      partitioningSchemeFullName = s; 
      setModified(); 
   }
}

/**
 * set the document object identifier.
 */
void 
StudyMetaData::setDocumentObjectIdentifier(const QString& doi) 
{ 
   if (documentObjectIdentifier != doi) {
      documentObjectIdentifier = doi;
      setModified(); 
   }
}

/**
 * set parent.
 */
void 
StudyMetaData::setParent(StudyMetaDataFile* parentStudyMetaDataFileIn)
{
   parentStudyMetaDataFile = parentStudyMetaDataFileIn;
}
      
/**
 * clear this study info is modified.
 */
void 
StudyMetaData::clearModified()
{
   studyDataModifiedFlag = false;
}
      
/**
 * set this study info is modified.
 */
void 
StudyMetaData::setModified()
{
   studyDataModifiedFlag = true;
   if (parentStudyMetaDataFile != NULL) {
      parentStudyMetaDataFile->setModified();
   }
}      

/**
 * get the keywords.
 */
void 
StudyMetaData::getKeywords(std::vector<QString>& keywordsOut) const
{
   keywordsOut.clear();
   
   const QStringList sl = getKeywords().split(';', QString::SkipEmptyParts);
   for (int k = 0; k < sl.size(); k++) {
      const QString kw = sl.at(k).trimmed();
      if (kw.isEmpty() == false) {
         keywordsOut.push_back(kw);
      }
   }
}

/**
 * get all table sub header short names in this study.
 */
void 
StudyMetaData::getAllTableSubHeaderShortNames(std::vector<QString>& tableSubHeaderShortNamesOut) const
{
   tableSubHeaderShortNamesOut.clear();
   
   const int numTables = getNumberOfTables();
   for (int i = 0; i < numTables; i++) {
      const Table* table = getTable(i);
      const int numSubHeaders = table->getNumberOfSubHeaders();
      for (int j = 0; j < numSubHeaders; j++) {
         const SubHeader* sh = table->getSubHeader(j);
         const QString shortName = sh->getShortName();
         if (shortName.isEmpty() == false) {
            tableSubHeaderShortNamesOut.push_back(shortName);
         }
      }
   }
}
      
//====================================================================================
//
// Figure class
//
//====================================================================================

/**
 * constructor.
 */
StudyMetaData::Figure::Figure()
{
   clear();
}

/**
 * destructor.
 */
StudyMetaData::Figure::~Figure()
{
   clear();
}

/**
 * copy constructor.
 */
StudyMetaData::Figure::Figure(const Figure& f)
{
   parentStudyMetaData = NULL;
   copyHelper(f);
}

/**
 * assignment operator.
 */
StudyMetaData::Figure& 
StudyMetaData::Figure::operator=(const Figure& f)
{
   if (this != &f) {
      copyHelper(f);
   }
   return *this;
}

/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
StudyMetaData::Figure::copyHelper(const Figure& f)
{
   StudyMetaData* savedParentStudyMetaData = parentStudyMetaData;
   
   clear();
   legend = f.legend;
   number = f.number;
   
   const int numPanels = f.getNumberOfPanels();
   for (int i = 0; i < numPanels; i++) {
      addPanel(new Panel(*(f.getPanel(i))));
   }
   
   parentStudyMetaData = savedParentStudyMetaData;
   setModified();
}

/**
 * clear the figure information.
 */
void 
StudyMetaData::Figure::clear()
{
   parentStudyMetaData = NULL;
   legend = "";
   number = "1";
   for (unsigned int i = 0; i < panels.size(); i++) {
      delete panels[i];
   }
   panels.clear();
}
            
/**
 * add a panel.
 */
void 
StudyMetaData::Figure::addPanel(Panel* p)
{
   p->setParent(this);
   panels.push_back(p);
   setModified();
}

/**
 * delete a panel.
 */
void 
StudyMetaData::Figure::deletePanel(const int indx)
{
   delete panels[indx];
   panels.erase(panels.begin() + indx);
   setModified();
}
            
/**
 * delete a panel.
 */
void 
StudyMetaData::Figure::deletePanel(const Panel* panel)
{
   for (int i = 0; i < getNumberOfPanels(); i++) {
      if (getPanel(i) == panel) {
         deletePanel(i);
         break;
      }
   }
}

/**
 * get a panel by its panel number/letter.
 */
StudyMetaData::Figure::Panel* 
StudyMetaData::Figure::getPanelByPanelNumberOrLetter(const QString& panelNumberOrLetter)
{
   for (int i = 0; i < getNumberOfPanels(); i++) {
      Panel* p = getPanel(i);
      if (p->getPanelNumberOrLetter() == panelNumberOrLetter) {
         return p;
      }
   }
   return NULL;
}

/**
 * get a panel by its panel number/letter (const method).
 */
const StudyMetaData::Figure::Panel* 
StudyMetaData::Figure::getPanelByPanelNumberOrLetter(const QString& panelNumberOrLetter) const
{
   for (int i = 0; i < getNumberOfPanels(); i++) {
      const Panel* p = getPanel(i);
      if (p->getPanelNumberOrLetter() == panelNumberOrLetter) {
         return p;
      }
   }
   return NULL;
}
            
/**
 * set the legend.
 */
void 
StudyMetaData::Figure::setLegend(const QString& s)
{
   if (legend != s) {
      legend = s;
      setModified();
   }
}

/**
 * set the figure number.
 */
void 
StudyMetaData::Figure::setNumber(const QString& n)
{
   if (number != n) {
      number = n;
      setModified();
   }
}

/**
 * set modified status.
 */
void 
StudyMetaData::Figure::setModified()
{
   if (parentStudyMetaData != NULL) {
      parentStudyMetaData->setModified();
   }
}

/**
 * set parent.
 */
void 
StudyMetaData::Figure::setParent(StudyMetaData* parentStudyMetaDataIn)
{
   parentStudyMetaData = parentStudyMetaDataIn;
}
            
/**
 * called to read from an XML structure.
 */
void 
StudyMetaData::Figure::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != "StudyMetaDataFigure") {
      QString msg("Incorrect element type passed to StudyMetaData::Figure::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "legend") {
            legend = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "number") {
            number = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "StudyMetaDataFigurePanel") {
            Panel* p = new Panel;
            p->readXML(node);
            addPanel(p);
         }
         else {
            std::cout << "WARNING: unrecognized StudyMetaData::Figure element ignored: "
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
StudyMetaData::Figure::writeXML(QDomDocument& xmlDoc,
                                QDomElement&  parentElement) const throw (FileException)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement element = xmlDoc.createElement("StudyMetaDataFigure");

   //
   // write data
   //
   AbstractFile::addXmlCdataElement(xmlDoc, element, "legend", legend);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "number", number);
   
   //
   // write panels
   //
   for (int i = 0; i < getNumberOfPanels(); i++) {
      panels[i]->writeXML(xmlDoc, element);
   }
   
   //
   // Add to parent
   //
   parentElement.appendChild(element);
}

/**
 * write the data into a comma separated value file.
 */
void 
StudyMetaData::Figure::writeDataIntoCommaSeparatedValueFile(const std::vector<Figure*>& figures,
                                                CommaSeparatedValueFile& csvf)  throw (FileException)
{
   const int numRows = static_cast<int>(figures.size());
   if (numRows <= 0) {
      return;
   }
   
   //
   // Column numbers for info
   //
   int numCols = 0;
   const int legendCol = numCols++;
   const int numberCol = numCols++;
   
   StringTable* st = new StringTable(numRows, numCols, "Study Metadata Figure");
   st->setColumnTitle(legendCol, "Legend");
   st->setColumnTitle(numberCol, "Number");
   
   for (int i = 0; i < numRows; i++) {
      const Figure& f = *(figures[i]);
      st->setElement(i, legendCol, f.getLegend());
      st->setElement(i, numberCol, f.getNumber());
   }
   
   csvf.addDataSection(st);
}

/**
 * read the data from a StringTable.
 */
void 
StudyMetaData::Figure::readDataFromStringTable(std::vector<Figure*>& figures,
                                               const StringTable& st) throw (FileException)
{
   if (st.getTableTitle() != "Study Metadata Figure") {
      throw FileException("String table for StudyMetaData::Figure does not have name Study Metadata Figure");
   }
   
   figures.clear();

   int legendCol = -1;
   int numberCol = -1;
   
   const int numCols = st.getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString name = st.getColumnTitle(i).toLower();
      if (name == "legend") {
         legendCol = i;
      }
      else if (name == "number") {
         numberCol = i;
      }
      else {
            std::cout << "WARNING: unrecognized StudyMetaData::Figure element in string table ignored: "
                      << name.toAscii().constData()
                      << std::endl;
      }
   }
   
   const int numRows = st.getNumberOfRows();
   for (int i = 0; i < numRows; i++) {
      Figure* f = new Figure;
      
      if (legendCol >= 0) {
         f->setLegend(st.getElement(i, legendCol));
      }
      if (numberCol >= 0) {
         f->setNumber(st.getElement(i, numberCol));
      }
      
      figures.push_back(f);
   }
}

//====================================================================================
//
// Figure Panel class
//
//====================================================================================

/**
 * constructor.
 */
StudyMetaData::Figure::Panel::Panel()
{
   clear();
}

/**
 * destructor.
 */
StudyMetaData::Figure::Panel::~Panel()
{
   clear();
}

/**
 * copy constructor.
 */
StudyMetaData::Figure::Panel::Panel(const Panel& p)
{
   parentFigure = NULL;
   copyHelper(p);
}

/**
 * assignment operator.
 */
StudyMetaData::Figure::Panel& 
StudyMetaData::Figure::Panel::operator=(const Panel& p)
{
   if (this != &p) {
      copyHelper(p);
   }
   return *this;
}

/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
StudyMetaData::Figure::Panel::copyHelper(const Panel& p)
{
   Figure* savedParentFigure = parentFigure;
   
   clear();
   description         = p.description;
   panelNumberOrLetter = p.panelNumberOrLetter;
   taskDescription     = p.taskDescription;
   taskBaseline        = p.taskBaseline;
   testAttributes      = p.testAttributes;
   
   parentFigure = savedParentFigure;
   setModified();
}

/**
 * clear the panel's data.
 */
void 
StudyMetaData::Figure::Panel::clear()
{
   parentFigure = NULL;
   panelNumberOrLetter = "";
   description = "";
   taskDescription = "";
   taskBaseline = "";
   testAttributes = "";
}

/**
 * set the panel number/letter.
 */
void 
StudyMetaData::Figure::Panel::setPanelNumberOrLetter(const QString& s)
{
   if (panelNumberOrLetter != s) {
      panelNumberOrLetter = s;
      setModified();
   }
}

/**
 * set the description.
 */
void 
StudyMetaData::Figure::Panel::setDescription(const QString& s)
{
   if (description != s) {
      description = s;
      setModified();
   }
}

/**
 * set Task Description.
 */
void 
StudyMetaData::Figure::Panel::setTaskDescription(const QString& s)
{
   if (taskDescription != s) {
      taskDescription = s;
      setModified();
   }
}

/**
 * set TaskBaseline.
 */
void 
StudyMetaData::Figure::Panel::setTaskBaseline(const QString& s)
{
   if (taskBaseline != s) {
      taskBaseline = s;
      setModified();
   }
}

/**
 * set TestAttributes.
 */
void 
StudyMetaData::Figure::Panel::setTestAttributes(const QString& s)
{
   if (testAttributes != s) {
      testAttributes = s;
      setModified();
   }
}

/**
 * set modified status.
 */
void 
StudyMetaData::Figure::Panel::setModified()
{
   if (parentFigure != NULL) {
      parentFigure->setModified();
   }
}

/**
 * set parent.
 */
void 
StudyMetaData::Figure::Panel::setParent(Figure* parentFigureIn)
{
   parentFigure = parentFigureIn;
}
                 
/**
 * called to read from an XML structure.
 */
void 
StudyMetaData::Figure::Panel::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != "StudyMetaDataFigurePanel") {
      QString msg("Incorrect element type passed to StudyMetaData::Figure::Panel::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "description") {
            description = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "panelNumberOrLetter") {
            panelNumberOrLetter = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "taskDescription") {
            taskDescription = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "taskBaseline") {
            taskBaseline = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "testAttributes") {
            testAttributes = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else {
            std::cout << "WARNING: unrecognized StudyMetaData::Figure::Panel element ignored: "
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
StudyMetaData::Figure::Panel::writeXML(QDomDocument& xmlDoc,
                                       QDomElement&  parentElement) const throw (FileException)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement element = xmlDoc.createElement("StudyMetaDataFigurePanel");

   //
   // write data
   //
   AbstractFile::addXmlCdataElement(xmlDoc, element, "description", description);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "panelNumberOrLetter", panelNumberOrLetter);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "taskDescription", taskDescription);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "taskBaseline", taskBaseline);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "testAttributes", testAttributes);
   
   //
   // Add to parent
   //
   parentElement.appendChild(element);
}

/**
 * write the data into a comma separated value file
 */
void 
StudyMetaData::Figure::Panel::writeDataIntoCommaSeparatedValueFile(const std::vector<Panel*>& panels,
                                                       CommaSeparatedValueFile& csvf)  throw (FileException)
{
   const int numRows = static_cast<int>(panels.size());
   if (numRows <= 0) {
      return;
   }
   
   //
   // Column numbers for info
   //
   int numCols = 0;
   const int descriptionCol = numCols++;
   const int panelNumberOrLetterCol = numCols++;
   const int taskDescriptionCol = numCols++;
   const int taskBaselineCol = numCols++;
   const int testAttributesCol = numCols++;

   StringTable* st = new StringTable(numRows, numCols, "Study Metadata Figure Panel");
   st->setColumnTitle(descriptionCol, "Description");
   st->setColumnTitle(panelNumberOrLetterCol, "Number/Letter");
   st->setColumnTitle(taskDescriptionCol, "Task Description");
   st->setColumnTitle(taskBaselineCol, "Task Baseline");
   st->setColumnTitle(testAttributesCol, "Test Attributes");

   for (int i = 0; i < numRows; i++) {
      const Panel& p = *(panels[i]);
      st->setElement(i, descriptionCol, p.getDescription());
      st->setElement(i, panelNumberOrLetterCol, p.getPanelNumberOrLetter());
      st->setElement(i, taskDescriptionCol, p.getTaskDescription());
      st->setElement(i, taskBaselineCol, p.getTaskBaseline());
      st->setElement(i, testAttributesCol, p.getTestAttributes());
   }
   
   csvf.addDataSection(st);
}

/**
 * read the data from a StringTable.
 */
void 
StudyMetaData::Figure::Panel::readDataFromStringTable(std::vector<Panel*>& panels,
                                                      const StringTable& st) throw (FileException)
{
   if (st.getTableTitle() != "Study Metadata Figure Panel") {
      throw FileException("String table for StudyMetaData::Figure::Panel does not have name Study Metadata Figure Panel");
   }
   
   panels.clear();

   int descriptionCol = -1;
   int panelNumberOrLetterCol = -1;
   int taskDescriptionCol = -1;
   int taskBaselineCol = -1;
   int testAttributesCol = -1;

   const int numCols = st.getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString name = st.getColumnTitle(i).toLower();
      if (name == "description") {
         descriptionCol = i;
      }
      else if (name == "number/letter") {
         panelNumberOrLetterCol = i;
      }
      else if (name == "task description") {
         taskDescriptionCol = i;
      }
      else if (name == "task baseline") {
         taskBaselineCol = i;
      }
      else if (name == "test attributes") {
         testAttributesCol = i;
      }
      else {
            std::cout << "WARNING: unrecognized StudyMetaData::Figure::Panel element in string table ignored: "
                      << name.toAscii().constData()
                      << std::endl;
      }
   }
   
   const int numRows = st.getNumberOfRows();
   for (int i = 0; i < numRows; i++) {
      Panel* p = new Panel;
      
      if (descriptionCol >= 0) {
         p->setDescription(st.getElement(i, descriptionCol));
      }
      if (panelNumberOrLetterCol >= 0) {
         p->setPanelNumberOrLetter(st.getElement(i, panelNumberOrLetterCol));
      }
      if (taskDescriptionCol >= 0) {
         p->setTaskDescription(st.getElement(i, taskDescriptionCol));
      }
      if (taskBaselineCol >= 0) {
         p->setTaskBaseline(st.getElement(i, taskBaselineCol));
      }
      if (testAttributesCol >= 0) {
         p->setTestAttributes(st.getElement(i, testAttributesCol));
      }
      
      panels.push_back(p);
   }
}

//====================================================================================
//
// Table class
//
//====================================================================================

/**
 * constructor.
 */
StudyMetaData::Table::Table()
{
   clear();
}

/**
 * destructor.
 */
StudyMetaData::Table::~Table()
{
   clear();
}

/**
 * copy constructor.
 */
StudyMetaData::StudyMetaData::Table::Table(const Table& t)
{
   parentStudyMetaData = NULL;
   copyHelper(t);
}

/**
 * assignment operator.
 */
StudyMetaData::StudyMetaData::Table& 
StudyMetaData::StudyMetaData::Table::operator=(const Table& t)
{
   if (this != &t) {
      copyHelper(t);
   }
   return *this;
}

/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
StudyMetaData::StudyMetaData::Table::copyHelper(const Table& t)
{
   StudyMetaData* savedParentStudyMetaData = parentStudyMetaData;
   
   clear();
   footer = t.footer;
   header = t.header;
   number = t.number;
   sizeUnits = t.sizeUnits;
   statisticType = t.statisticType;
   statisticDescription = t.statisticDescription;
   voxelDimensions = t.voxelDimensions;
   
   const int numSubHeaders = t.getNumberOfSubHeaders();
   for (int i = 0; i < numSubHeaders; i++) {
      addSubHeader(new SubHeader(*(t.getSubHeader(i))));
   }
   
   parentStudyMetaData = savedParentStudyMetaData;
   setModified();
}

/**
 * clear the table.
 */
void 
StudyMetaData::Table::clear()
{
   parentStudyMetaData = NULL;
   footer = "";
   header = "";
   number = "1";
   sizeUnits = "";
   statisticType = "";
   statisticDescription = "";
   voxelDimensions = "";
   for (unsigned int i = 0; i < subHeaders.size(); i++) {
      delete subHeaders[i];
      subHeaders[i] = NULL;
   }
   subHeaders.clear();
}

/**
 * add a sub header.
 */
void 
StudyMetaData::Table::addSubHeader(SubHeader* sh)
{
   sh->setParent(this);
   subHeaders.push_back(sh);
   setModified();
}

/**
 * delete a sub header.
 */
void 
StudyMetaData::Table::deleteSubHeader(const int indx)
{
   delete subHeaders[indx];
   subHeaders.erase(subHeaders.begin() + indx);
   setModified();
}
            
/**
 * delete a sub header.
 */
void 
StudyMetaData::Table::deleteSubHeader(const SubHeader* subHeader)
{
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      if (getSubHeader(i) == subHeader) {
         deleteSubHeader(i);
         break;
      }
   }
}
            
/**
 * get a sub header by sub header number.
 */
StudyMetaData::SubHeader* 
StudyMetaData::Table::getSubHeaderBySubHeaderNumber(const QString& subHeaderNumber)
{
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      SubHeader* sh = getSubHeader(i);
      if (sh->getNumber() == subHeaderNumber) {
         return sh;
      }
   }
   return NULL;
}

/**
 * get a sub header by sub header number (const method).
 */
const StudyMetaData::SubHeader* 
StudyMetaData::Table::getSubHeaderBySubHeaderNumber(const QString& subHeaderNumber) const
{
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      const SubHeader* sh = getSubHeader(i);
      if (sh->getNumber() == subHeaderNumber) {
         return sh;
      }
   }
   return NULL;
}
            
/**
 * set footer.
 */
void 
StudyMetaData::Table::setFooter(const QString& s)
{
   if (footer != s) {
      footer = s;
      setModified();
   }
}            

/**
 * set header.
 */
void 
StudyMetaData::Table::setHeader(const QString& s)
{
   if (header != s) {
      header = s;
      setModified();
   }
}            

/**
 * set number.
 */
void 
StudyMetaData::Table::setNumber(const QString& n)
{
   if (number != n) {
      number = n;
      setModified();
   }
}            

/**
 * set size units.
 */
void 
StudyMetaData::Table::setSizeUnits(const QString& s)
{
   if (sizeUnits != s) {
      sizeUnits = s;
      setModified();
   }
}            

/**
 * set tatistic type.
 */
void 
StudyMetaData::Table::setStatisticType(const QString& s)
{
   if (statisticType != s) {
      statisticType = s;
      setModified();
   }
}            

/**
 * set statistic description.
 */
void 
StudyMetaData::Table::setStatisticDescription(const QString& s)
{
   if (statisticDescription != s) {
      statisticDescription = s;
      setModified();
   }
}            

/**
 * set voxelDimensions.
 */
void 
StudyMetaData::Table::setVoxelDimensions(const QString& s)
{
   if (voxelDimensions != s) {
      voxelDimensions = s;
      setModified();
   }
}            

/**
 * set modified.
 */
void 
StudyMetaData::Table::setModified()
{
   if (parentStudyMetaData != NULL) {
      parentStudyMetaData->setModified();
   }
}

/**
 * set parent.
 */
void 
StudyMetaData::Table::setParent(StudyMetaData* parentStudyMetaDataIn)
{
   parentStudyMetaData = parentStudyMetaDataIn;
}
            
/**
 * called to read from an XML structure.
 */
void 
StudyMetaData::Table::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != "StudyMetaDataTable") {
      QString msg("Incorrect element type passed to StudyMetaData::Table::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "footer") {
            footer = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "header") {
            header = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "number") {
            number = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "sizeUnits") {
            sizeUnits = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "statisticType") {
            statisticType = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "statisticDescription") {
            statisticDescription = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "voxelDimensions") {
            voxelDimensions = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if ((elem.tagName() == "StudyMetaDataSubHeader") ||
                  (elem.tagName() == "StudyMetaDataTableSubHeader")) {
            SubHeader* sh = new SubHeader;
            sh->readXML(node);
            addSubHeader(sh);
         }
         else {
            std::cout << "WARNING: unrecognized StudyMetaData::Table element ignored: "
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
StudyMetaData::Table::writeXML(QDomDocument& xmlDoc,
                               QDomElement&  parentElement) const throw (FileException)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement element = xmlDoc.createElement("StudyMetaDataTable");

   //
   // write data
   //
   AbstractFile::addXmlCdataElement(xmlDoc, element, "footer", footer);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "header", header);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "number", number);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "sizeUnits", sizeUnits);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "statisticType", statisticType);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "statisticDescription", statisticDescription);
   AbstractFile::addXmlCdataElement( xmlDoc, element, "voxelDimensions", voxelDimensions);
   
   //
   // write sub headers 
   //
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      subHeaders[i]->writeXML(xmlDoc, element);
   }
   
   //
   // Add to parent
   //
   parentElement.appendChild(element);
}

/**
 * write the data into a comma separated value file.
 */
void 
StudyMetaData::Table::writeDataIntoCommaSeparatedValueFile(const std::vector<Table*>& tables,
                                               CommaSeparatedValueFile& csvf) throw (FileException)
{
   const int numRows = static_cast<int>(tables.size());
   if (numRows <= 0) {
      return;
   }
   
   //
   // Column numbers for info
   //
   int numCols = 0;
   const int footerCol = numCols++;
   const int headerCol = numCols++;
   const int numberCol = numCols++;
   const int sizeUnitsCol = numCols++;
   const int statTypeCol = numCols++;
   const int statDescripCol = numCols++;
   const int voxelDimCol = numCols++;
   
   StringTable* st = new StringTable(numRows, numCols, "Study Metadata Table");
   st->setColumnTitle(footerCol, "Footer");
   st->setColumnTitle(headerCol, "Header");
   st->setColumnTitle(numberCol, "Number");
   st->setColumnTitle(sizeUnitsCol, "Size Units");
   st->setColumnTitle(statTypeCol, "Statistic Type");
   st->setColumnTitle(statDescripCol, "Statistic Description");
   st->setColumnTitle(voxelDimCol, "Voxel Dimension");
   
   for (int i = 0; i < numRows; i++) {
      const Table& t = *(tables[i]);
      st->setElement(i, footerCol, t.getFooter());
      st->setElement(i, headerCol, t.getHeader());
      st->setElement(i, numberCol, t.getNumber());
      st->setElement(i, sizeUnitsCol, t.getSizeUnits());
      st->setElement(i, statTypeCol, t.getStatisticType());
      st->setElement(i, statDescripCol, t.getStatisticDescription());
      st->setElement(i, voxelDimCol, t.getVoxelDimensions());
   }
   
   csvf.addDataSection(st);
}

/**
 * read the data from a StringTable.
 */
void 
StudyMetaData::Table::readDataFromStringTable(std::vector<Table*>& tables,
                                              const StringTable& st) throw (FileException)
{
   if (st.getTableTitle() != "Study Metadata Table") {
      throw FileException("String table for StudyMetaData::Table does not have name Study Metadata Table");
   }
   
   tables.clear();

   int footerCol = -1;
   int headerCol = -1;
   int numberCol = -1;
   int sizeUnitsCol = -1;
   int statTypeCol = -1;
   int statDescripCol = -1;
   int voxelDimCol = -1;
   
   const int numCols = st.getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString name = st.getColumnTitle(i).toLower();
      if (name == "footer") {
         footerCol = i;
      }
      else if (name == "header") {
         headerCol = i;
      }
      else if (name == "number") {
         numberCol = i;
      }
      else if (name == "size units") {
         sizeUnitsCol = i;
      }
      else if (name == "statistic type") {
         statTypeCol = i;
      }
      else if (name == "statistic description") {
         statDescripCol = i;
      }
      else if (name == "voxel dimension") {
         voxelDimCol = i;
      }
      else {
            std::cout << "WARNING: unrecognized StudyMetaData::Table element in string table ignored: "
                      << name.toAscii().constData()
                      << std::endl;
      }
   }
   
   const int numRows = st.getNumberOfRows();
   for (int i = 0; i < numRows; i++) {
      Table* t = new Table;
      
      if (footerCol >= 0) {
         t->setFooter(st.getElement(i, footerCol));
      }
      if (headerCol >= 0) {
         t->setHeader(st.getElement(i, headerCol));
      }
      if (numberCol >= 0) {
         t->setNumber(st.getElement(i, numberCol));
      }
      if (sizeUnitsCol >= 0) {
         t->setSizeUnits(st.getElement(i, sizeUnitsCol));
      }
      if (statTypeCol >= 0) {
         t->setStatisticType(st.getElement(i, statTypeCol));
      }
      if (statDescripCol >= 0) {
         t->setStatisticDescription(st.getElement(i, statDescripCol));
      }
      if (voxelDimCol >= 0) {
         t->setVoxelDimensions(st.getElement(i, voxelDimCol));
      }
      
      tables.push_back(t);
   }
}

//====================================================================================
//
// SubHeader class
//
//====================================================================================

/**
 * constructor
 */
StudyMetaData::SubHeader::SubHeader()
{
   clear();
}

/**
 * destructor
 */
StudyMetaData::SubHeader::~SubHeader()
{
   clear();
}

/**
 * copy constructor.
 */
StudyMetaData::SubHeader::SubHeader(const SubHeader& sh)
{
   parentPageReference = NULL;
   parentTable = NULL;
   copyHelper(sh);
}

/**
 * assignment operator.
 */
StudyMetaData::SubHeader& 
StudyMetaData::SubHeader::operator=(const SubHeader& sh)
{
   if (this != &sh) {
      copyHelper(sh);
   }
   return *this;
}

/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
StudyMetaData::SubHeader::copyHelper(const SubHeader& sh)
{
   PageReference* savedParentPageReference = parentPageReference;
   Table* savedParentTable = parentTable;
   
   clear();
   name            = sh.name;
   number          = sh.number;
   shortName       = sh.shortName;
   taskDescription = sh.taskDescription;
   taskBaseline    = sh.taskBaseline;
   testAttributes  = sh.testAttributes;
   selectedFlag    = sh.selectedFlag;
   
   parentPageReference = savedParentPageReference;
   parentTable = savedParentTable;
   setModified();
}

/**
 * clear the sub header.
 */
void 
StudyMetaData::SubHeader::clear()
{
   parentPageReference = NULL;
   parentTable = NULL;
   
   name = "";
   number = "1";
   shortName = "";
   taskDescription = "";
   taskBaseline = "";
   testAttributes = "";
   selectedFlag = true;
}
                  
/**
 * set name.
 */
void 
StudyMetaData::SubHeader::setName(const QString& s)
{
   if (name != s) {
      name = s;
      setModified();
   }
}

/**
 * set number.
 */
void 
StudyMetaData::SubHeader::setNumber(const QString& n)
{
   if (number != n) {
      number = n;
      setModified();
   }
}

/**
 * set short name.
 */
void 
StudyMetaData::SubHeader::setShortName(const QString& s)
{
   if (shortName != s) {
      shortName = s.trimmed();
      setModified();
   }
}

/**
 * set Task Description.
 */
void 
StudyMetaData::SubHeader::setTaskDescription(const QString& s)
{
   if (taskDescription != s) {
      taskDescription = s;
      setModified();
   }
}

/**
 * set TaskBaseline.
 */
void 
StudyMetaData::SubHeader::setTaskBaseline(const QString& s)
{
   if (taskBaseline != s) {
      taskBaseline = s;
      setModified();
   }
}

/** 
 * set TestAttributes.
 */
void 
StudyMetaData::SubHeader::setTestAttributes(const QString& s)
{
   if (testAttributes != s) {
      testAttributes = s;
      setModified();
   }
}

/**
 * set instance modified.
 */
void 
StudyMetaData::SubHeader::setModified()
{
   if (parentPageReference != NULL) {
      parentPageReference->setModified();
   }
   if (parentTable != NULL) {
      parentTable->setModified();
   }
}

/**
 * set parent.
 */
void 
StudyMetaData::SubHeader::setParent(Table* parentTableIn)
{
   parentTable = parentTableIn;
}
                  
/**
 * set parent.
 */
void 
StudyMetaData::SubHeader::setParent(PageReference* parentPageReferenceIn)
{
   parentPageReference= parentPageReferenceIn;
}
                  
/**
 * called to read from an XML structure.
 */
void 
StudyMetaData::SubHeader::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if ((elem.tagName() != "StudyMetaDataTableSubHeader") &&
       (elem.tagName() != "StudyMetaDataSubHeader")) {
      QString msg("Incorrect element type passed to StudyMetaData::SubHeader::readXML() ");
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
         else if (elem.tagName() == "number") {
            number = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "shortName") {
            shortName = AbstractFile::getXmlElementFirstChildAsString(elem).trimmed();
         }
         else if (elem.tagName() == "taskDescription") {
            taskDescription = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "taskBaseline") {
            taskBaseline = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "testAttributes") {
            testAttributes = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else {
            std::cout << "WARNING: unrecognized StudyMetaData::SubHeader element ignored: "
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
StudyMetaData::SubHeader::writeXML(QDomDocument& xmlDoc,
                                          QDomElement&  parentElement) const throw (FileException)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement element = xmlDoc.createElement("StudyMetaDataSubHeader");

   //
   // write data
   //
   AbstractFile::addXmlCdataElement(xmlDoc, element, "name", name);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "number", number);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "shortName", shortName);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "taskDescription", taskDescription);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "taskBaseline", taskBaseline);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "testAttributes", testAttributes);
   
   //
   // Add to parent
   //
   parentElement.appendChild(element);
}

/**
 * write the data into a comma separated value file.
 */
void 
StudyMetaData::SubHeader::writeDataIntoCommaSeparatedValueFile(const std::vector<SubHeader*>& subHeaders,
                                                          CommaSeparatedValueFile& csvf)  throw (FileException)
{
   const int numRows = static_cast<int>(subHeaders.size());
   if (numRows <= 0) {
      return;
   }
   
   //
   // Column numbers for info
   //
   int numCols = 0;
   const int nameCol = numCols++;
   const int numberCol = numCols++;
   const int shortNameCol = numCols++;
   const int taskDescriptionCol = numCols++;
   const int taskBaselineCol = numCols++;
   const int testAttributesCol = numCols++;

   StringTable* st = new StringTable(numRows, numCols, "Study Metadata Sub Header");
   st->setColumnTitle(nameCol, "Name");
   st->setColumnTitle(numberCol, "Number");
   st->setColumnTitle(shortNameCol, "Short Name");
   st->setColumnTitle(taskDescriptionCol, "Task Description");
   st->setColumnTitle(taskBaselineCol, "Task Baseline");
   st->setColumnTitle(testAttributesCol, "Test Attributes");

   for (int i = 0; i < numRows; i++) {
      const SubHeader& sh = *(subHeaders[i]);
      st->setElement(i, nameCol, sh.getName());
      st->setElement(i, numberCol, sh.getNumber());
      st->setElement(i, shortNameCol, sh.getShortName());
      st->setElement(i, taskDescriptionCol, sh.getTaskDescription());
      st->setElement(i, taskBaselineCol, sh.getTaskBaseline());
      st->setElement(i, testAttributesCol, sh.getTestAttributes());
   }
   
   csvf.addDataSection(st);
}

/**
 * read the data from a StringTable.
 */
void 
StudyMetaData::SubHeader::readDataFromStringTable(std::vector<SubHeader*>& subHeaders,
                                                         const StringTable& st) throw (FileException)
{
   if (st.getTableTitle() != "Study Metadata Sub Header") {
      throw FileException("String table for StudyMetaData::SubHeader does not have name Study Metadata Table Sub Header");
   }
   
   subHeaders.clear();

   int nameCol = -1;
   int numberCol = -1;
   int shortNameCol = -1;
   int taskDescriptionCol = -1;
   int taskBaselineCol = -1;
   int testAttributesCol = -1;

   const int numCols = st.getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString name = st.getColumnTitle(i).toLower();
      if (name == "name") {
         nameCol = i;
      }
      else if (name == "number") {
         numberCol = i;
      }
      else if (name == "shortname") {
         shortNameCol = i;
      }
      else if (name == "task description") {
         taskDescriptionCol = i;
      }
      else if (name == "task baseline") {
         taskBaselineCol = i;
      }
      else if (name == "test attributes") {
         testAttributesCol = i;
      }
      else {
            std::cout << "WARNING: unrecognized StudyMetaData::SubHeader element in string table ignored: "
                      << name.toAscii().constData()
                      << std::endl;
      }
   }
   
   const int numRows = st.getNumberOfRows();
   for (int i = 0; i < numRows; i++) {
      SubHeader* sh = new SubHeader;
      
      if (nameCol >= 0) {
         sh->setName(st.getElement(i, nameCol));
      }
      if (numberCol >= 0) {
         sh->setNumber(st.getElement(i, numberCol));
      }
      if (shortNameCol >= 0) {
         sh->setShortName(st.getElement(i, shortNameCol).trimmed());
      }
      if (taskDescriptionCol >= 0) {
         sh->setTaskDescription(st.getElement(i, taskDescriptionCol));
      }
      if (taskBaselineCol >= 0) {
         sh->setTaskBaseline(st.getElement(i, taskBaselineCol));
      }
      if (testAttributesCol >= 0) {
         sh->setTestAttributes(st.getElement(i, testAttributesCol));
      }
      
      subHeaders.push_back(sh);
   }
}

//====================================================================================
//
// Page Reference class
//
//====================================================================================

/**
 * constructor.
 */
StudyMetaData::PageReference::PageReference()
{
   parentStudyMetaData = NULL;
   clear();
}

/**
 * destructor.
 */
StudyMetaData::PageReference::~PageReference()
{
   clear();
}

/**
 * copy constructor.
 */
StudyMetaData::PageReference::PageReference(const PageReference& pr)
{
   parentStudyMetaData = NULL;
   copyHelper(pr);
}

/**
 * assignment operator.
 */
StudyMetaData::PageReference& 
StudyMetaData::PageReference::operator=(const PageReference& pr)
{
   if (this != &pr) {
      copyHelper(pr);
   }
   return *this;
}

/**
 * clear the page link.
 */
void 
StudyMetaData::PageReference::clear()
{
   pageNumber = "1";
   header = "";
   comment = "";
   sizeUnits = "";
   voxelDimensions = "";
   statisticType = "";
   statisticDescription = "";
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      delete subHeaders[i];
      subHeaders[i] = NULL;
   }
   subHeaders.clear();
}

/**
 * add a sub header.
 */
void 
StudyMetaData::PageReference::addSubHeader(SubHeader* sh)
{
   sh->setParent(this);
   subHeaders.push_back(sh);
   setModified();
}

/**
 * delete a sub header.
 */
void 
StudyMetaData::PageReference::deleteSubHeader(const int indx)
{
   delete subHeaders[indx];
   subHeaders.erase(subHeaders.begin() + indx);
   setModified();
}
            
/**
 * delete a sub header.
 */
void 
StudyMetaData::PageReference::deleteSubHeader(const SubHeader* subHeader)
{
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      if (getSubHeader(i) == subHeader) {
         deleteSubHeader(i);
         break;
      }
   }
}
            
/**
 * get a sub header by sub header number.
 */
StudyMetaData::SubHeader* 
StudyMetaData::PageReference::getSubHeaderBySubHeaderNumber(const QString& subHeaderNumber)
{
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      SubHeader* sh = getSubHeader(i);
      if (sh->getNumber() == subHeaderNumber) {
         return sh;
      }
   }
   return NULL;
}

/**
 * get a sub header by sub header number (const method).
 */
const StudyMetaData::SubHeader* 
StudyMetaData::PageReference::getSubHeaderBySubHeaderNumber(const QString& subHeaderNumber) const
{
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      const SubHeader* sh = getSubHeader(i);
      if (sh->getNumber() == subHeaderNumber) {
         return sh;
      }
   }
   return NULL;
}

/**
 * copy helper.
 */
void 
StudyMetaData::PageReference::copyHelper(const PageReference& pr)
{
   StudyMetaData* savedParentStudyMetaData = parentStudyMetaData;
   
   clear();
   pageNumber = pr.pageNumber;
   header     = pr.header;
   comment    = pr.comment;
   sizeUnits  = pr.sizeUnits;
   voxelDimensions  = pr.voxelDimensions;
   statisticType  = pr.statisticType;
   statisticDescription = pr.statisticDescription;
   
   const int numSubHeaders = pr.getNumberOfSubHeaders();
   for (int i = 0; i < numSubHeaders; i++) {
      addSubHeader(new SubHeader(*(pr.getSubHeader(i))));
   }
   
   parentStudyMetaData = savedParentStudyMetaData;
   setModified();
}

/**
 * set the page number.
 */
void 
StudyMetaData::PageReference::setPageNumber(const QString& pn)
{
   if (pageNumber != pn) {
      pageNumber = pn;
      setModified();
   }
}

/**
 * set the header.
 */
void 
StudyMetaData::PageReference::setHeader(const QString& s)
{
   if (header != s) {
      header = s;
      setModified();
   }
}
            
/**
 * set the comment.
 */
void 
StudyMetaData::PageReference::setComment(const QString& s)
{
   if(comment != s) {
      comment = s;
      setModified();
   }
}

/**
 * set the size units.
 */
void 
StudyMetaData::PageReference::setSizeUnits(const QString& s)
{
   if (sizeUnits != s) {
      sizeUnits = s;
      setModified();
   }
}

/**
 * set the voxel size.
 */
void 
StudyMetaData::PageReference::setVoxelDimensions(const QString& s)
{
   if (voxelDimensions != s) {
      voxelDimensions = s;
      setModified();
   }
}

/**
 * set the statistic.
 */
void 
StudyMetaData::PageReference::setStatisticType(const QString& s)
{
   if (statisticType != s) {
      statisticType = s;
      setModified();
   }
}

/**
 * set the statistic description.
 */
void 
StudyMetaData::PageReference::setStatisticDescription(const QString& s)
{
   if (statisticDescription != s) {
      statisticDescription = s;
      setModified();
   }
}

/**
 * called to read from an XML structure.
 */
void 
StudyMetaData::PageReference::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {      
      return;   
   }   
   QDomElement elem = nodeIn.toElement();   
   if (elem.isNull()) {      
      return;   
   }   
   if (elem.tagName() != "StudyMetaDataPageReference") {      
      QString msg("Incorrect element type passed to StudyMetaData::PageReference::readXML() ");
      msg.append(elem.tagName());      
      throw FileException("", msg);   
   }   
   QDomNode node = nodeIn.firstChild();   
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();      
      if (elem.isNull() == false) {         
         if (elem.tagName() == "pageNumber") {            
            pageNumber = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }         
         else if (elem.tagName() == "header") {            
            header = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }
         else if (elem.tagName() == "comment") {            
            comment = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }
         else if (elem.tagName() == "sizeUnits") {            
            sizeUnits = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }
         else if (elem.tagName() == "voxelDimensions") {            
            voxelDimensions = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }
         else if (elem.tagName() == "statisticType") {            
            statisticType = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }
         else if (elem.tagName() == "statisticDescription") {            
            statisticDescription = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }
         else if (elem.tagName() == "StudyMetaDataSubHeader") {
            SubHeader* sh = new SubHeader;
            sh->readXML(node);
            addSubHeader(sh);
         }
         else {
            std::cout << "WARNING: unrecognized StudyMetaData::PageReference element ignored: "
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
StudyMetaData::PageReference::writeXML(QDomDocument& xmlDoc,
              QDomElement&  parentElement) const throw (FileException)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement element = xmlDoc.createElement("StudyMetaDataPageReference");

   //
   // write data
   //
   AbstractFile::addXmlCdataElement(xmlDoc, element, "pageNumber", pageNumber);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "header", header);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "comment", comment);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "sizeUnits", sizeUnits);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "voxelDimensions", voxelDimensions);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "statisticType", statisticType);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "statisticDescription", statisticDescription);
   
   //
   // write sub headers 
   //
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      subHeaders[i]->writeXML(xmlDoc, element);
   }
   
   //
   // Add to parent
   //
   parentElement.appendChild(element);
}

/**
 * set parent.
 */
void 
StudyMetaData::PageReference::setParent(StudyMetaData* parentStudyMetaDataIn)
{
   parentStudyMetaData = parentStudyMetaDataIn;
}

/**
 * set modified.
 */
void 
StudyMetaData::PageReference::setModified()
{
   if (parentStudyMetaData != NULL) {
      parentStudyMetaData->setModified();
   }
}


//====================================================================================
//
// StudyMetaData File class
//
//====================================================================================

/**
 * constructor.
 */
StudyMetaDataFile::StudyMetaDataFile()
   : AbstractFile("StudyMetaData File",
                  SpecFile::getStudyMetaDataFileExtension(),
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
   clear();
}

/**
 * destructor.
 */
StudyMetaDataFile::~StudyMetaDataFile()
{
   clear();
}

/**
 * copy constructor.
 */
StudyMetaDataFile::StudyMetaDataFile(const StudyMetaDataFile& smdf)
   : AbstractFile("StudyMetaData File",
                  SpecFile::getStudyMetaDataFileExtension(),
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
   copyHelper(smdf);
}

/**
 * assignment operator.
 */
StudyMetaDataFile& 
StudyMetaDataFile::operator=(const StudyMetaDataFile& smdf)
{
   if (this != &smdf) {
      copyHelper(smdf);
   }
   return *this;
}

/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
StudyMetaDataFile::copyHelper(const StudyMetaDataFile& smdf)
{
   clear();
   
   //
   // Copy parent's data
   //
   copyHelperAbstractFile(smdf);
  
   const int numStudies = smdf.getNumberOfStudyMetaData();
   for (int i = 0; i < numStudies; i++) {
      addStudyMetaData(new StudyMetaData(*(smdf.getStudyMetaData(i))));
   }
   
   setModified();
}

/**
 * append a study metadata file to "this" study metadata file.
 */
void 
StudyMetaDataFile::append(const StudyMetaDataFile& smdf)
{
   const int num = smdf.getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      StudyMetaData* smf = new StudyMetaData(*(smdf.getStudyMetaData(i)));
      addStudyMetaData(smf);
   }
   setModified();
}
       
/** 
 * append metadata from a cell file to this file.
 */
void 
StudyMetaDataFile::append(CellFile& cf)
{
   const int num = cf.getNumberOfStudyInfo();
   for (int i = 0; i < num; i++) {
      //
      // Convert study info to study metadata and add to study meta data file
      //
      CellStudyInfo* csi = cf.getStudyInfo(i);
      StudyMetaData* smd = new StudyMetaData(*csi);
      addStudyMetaData(smd);
      
      //
      // Convert links from study info to study metadata in all cells
      //
      StudyMetaDataLink smdl;
      smdl.setPubMedID(smd->getPubMedID());
      const int numCells = cf.getNumberOfCells();
      for (int j = 0; j < numCells; j++) {
         CellData* cd = cf.getCell(j);
         if (cd->getStudyNumber() == i) {
            cd->setStudyMetaDataLink(smdl);
         }
      }
   }
   
   //
   // Remove study info from cell file
   //
   cf.deleteAllStudyInfo();
}

/**
 * append metadata from a cell projection file to this file.
 */
void 
StudyMetaDataFile::append(CellProjectionFile& cpf)
{
   const int num = cpf.getNumberOfStudyInfo();
   for (int i = 0; i < num; i++) {
      //
      // Convert study info to study metadata and add to study meta data file
      //
      const CellStudyInfo* csi = cpf.getStudyInfo(i);
      StudyMetaData* smd = new StudyMetaData(*csi);
      addStudyMetaData(smd);
      
      //
      // Convert links from study info to study metadata in all cell projections
      //
      StudyMetaDataLink smdl;
      smdl.setPubMedID(smd->getPubMedID());
      const int numCellProjections = cpf.getNumberOfCellProjections();
      for (int j = 0; j < numCellProjections; j++) {
         CellProjection* cp = cpf.getCellProjection(j);
         if (cp->getStudyNumber() == i) {
            cp->setStudyMetaDataLink(smdl);
         }
      }
   }
   
   //
   // Remove study info from cell projection file
   //
   cpf.deleteAllStudyInfo();
}
      
/**
 * append metadata from a cell projection file to this file.
 */
void 
StudyMetaDataFile::append(VocabularyFile& vf)
{
   const int num = vf.getNumberOfStudyInfo();
   for (int i = 0; i < num; i++) {
      //
      // Convert study info to study metadata and add to study meta data file
      //
      const CellStudyInfo* csi = vf.getStudyInfo(i);
      StudyMetaData* smd = new StudyMetaData(*csi);
      addStudyMetaData(smd);
      
      //
      // Convert links from study info to study metadata in all vocabulary entries
      //
      StudyMetaDataLink smdl;
      smdl.setPubMedID(smd->getPubMedID());
      const int numVocabularyEntries = vf.getNumberOfVocabularyEntries();
      for (int j = 0; j < numVocabularyEntries; j++) {
         VocabularyFile::VocabularyEntry* ve = vf.getVocabularyEntry(j);
         if (ve->getStudyNumber() == i) {
            ve->setStudyMetaDataLink(smdl);
         }
      }
   }
   
   //
   // Remove study info from vocabulary file
   //
   vf.deleteAllStudyInfo();
}
      
/**
 * Clears current file data in memory.  Deriving classes must override this method and
 * call AbstractFile::clearAbstractFile() from its clear method.
 */
void 
StudyMetaDataFile::clear()
{
   clearAbstractFile();
   for (unsigned int i = 0; i < studyMetaData.size(); i++) {
      delete studyMetaData[i];
   }
   studyMetaData.clear();
}

/**
 * given a study, report any meta-analysis studies of which it is a member.
 */
void 
StudyMetaDataFile::getMetaAnalysisStudiesForStudy(const StudyMetaData* mySMD,
                                                  std::vector<QString>& metaAnalysisStudyPMIDsOut) const
{
   metaAnalysisStudyPMIDsOut.clear();
   
   //
   // Get the studies PubMed and Project IDs
   //
   const QString myPMID = mySMD->getPubMedID();
   const QString myProjectID = mySMD->getProjectID();
   
   //
   // Loop through studies
   //
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      //
      // Skip study for which meta-analysis is sought
      //
      if (smd != mySMD) {
         //
         // Get the associated studies
         //
         const StudyNamePubMedID* ms = smd->getMetaAnalysisStudies();
         const int numMetaStudies = ms->getNumberOfStudies();
         for (int j = 0; j < numMetaStudies; j++) {
            QString msName, msPMID;
            ms->getStudyNameAndPubMedID(j, msName, msPMID);
            //
            // Does ID match?
            //
            if ((myPMID == msPMID) ||
                (myProjectID == msPMID)) {
               metaAnalysisStudyPMIDsOut.push_back(smd->getPubMedID());
            }
         }
      }
   }
}
                                          
/**
 * delete studies with the given names.
 */
void 
StudyMetaDataFile::deleteStudiesWithNames(const std::vector<QString>& namesOfStudiesToDelete)
{
   //
   // Find the indices of the studies that are to be deleted
   //
   std::vector<int> studiesToDelete;
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      if (std::find(namesOfStudiesToDelete.begin(),
                    namesOfStudiesToDelete.end(), 
                    smd->getName()) != namesOfStudiesToDelete.end()) {
         studiesToDelete.push_back(i);
      }
   }
   
   //
   // Delete the studies and start at highest index
   // since studies will get shifted as removed
   //
   const int iStart = static_cast<int>(studiesToDelete.size()) - 1;
   for (int i = iStart; i >= 0; i--) {
      deleteStudyMetaData(studiesToDelete[i]);
   }
}
      
/**
 * get the index of a study metadata item.
 */
int 
StudyMetaDataFile::getStudyMetaDataIndex(const StudyMetaData* smdToFind) const
{
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      if (smdToFind == smd) {
         return i;
      }
   }
   return -1;
}
      
/**
 * get study from its name.
 */
int 
StudyMetaDataFile::getStudyIndexFromName(const QString& name) const
{
   if (name.isEmpty()) {
      return -1;
   }
   
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      if (name == smd->getName()) {
         return i;
      }
   }
   
   return -1;
}
      
/**
 * get study from a PubMed ID (may match PubMed ID or Project ID).
 */
int 
StudyMetaDataFile::getStudyIndexFromPubMedID(const QString& pubMedID) const
{
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      if (pubMedID == smd->getPubMedID()) {
         return i;
      }
      else if (pubMedID == smd->getProjectID()) {  // check for ProjectID match
         return i;
      }
   }
   
   return -1;
}
      
/**
 * get the index of study meta data matching the study metadata link (-1 if no match found)
 */
int 
StudyMetaDataFile::getStudyIndexFromLink(const StudyMetaDataLink smdl) const
{
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      if (smdl.getPubMedID() == smd->getPubMedID()) {
         return i;
      }
      else if (smdl.getPubMedID() == smd->getProjectID()) {  // check for ProjectID match
         return i;
      }
   }
   
   return -1;
}

/**
 * add study meta data.
 */
void 
StudyMetaDataFile::addStudyMetaData(StudyMetaData* smd)
{
   smd->setParent(this);
   studyMetaData.push_back(smd);
   setModified();
}

/**
 * delete study meta data.
 */
void 
StudyMetaDataFile::deleteStudyMetaData(const int indx)
{
   delete studyMetaData[indx];
   studyMetaData.erase(studyMetaData.begin() + indx);
   setModified();
}      
   
/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
StudyMetaDataFile::empty() const
{
   return studyMetaData.empty();
}


/**
 * find out if comma separated file conversion supported.
 */
void 
StudyMetaDataFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                                bool& writeToCSV) const
{
   readFromCSV = false; // true;
   writeToCSV  = false; // true;
}

/**
 * get all keywords.
 */
void 
StudyMetaDataFile::getAllKeywords(std::vector<QString>& allKeywords) const
{
   allKeywords.clear();
   
   std::set<QString> keywordSet;
   
   const int numStudies = getNumberOfStudyMetaData();
   for (int i = 0; i < numStudies; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      std::vector<QString> studyKeywords;
      smd->getKeywords(studyKeywords);
      keywordSet.insert(studyKeywords.begin(), studyKeywords.end());
   }
   
   allKeywords.insert(allKeywords.end(),
                      keywordSet.begin(), keywordSet.end());
}
 
/**
 * get all table subheader short names.
 */
void 
StudyMetaDataFile::getAllTableSubHeaderShortNames(std::vector<QString>& allShortNames) const
{
   allShortNames.clear();
   
   std::set<QString> shortNameSet;
   
   const int numStudies = getNumberOfStudyMetaData();
   for (int i = 0; i < numStudies; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      std::vector<QString> studyTableSubHeaderShortNames;
      smd->getAllTableSubHeaderShortNames(studyTableSubHeaderShortNames);
      shortNameSet.insert(studyTableSubHeaderShortNames.begin(), studyTableSubHeaderShortNames.end());
   }
   
   allShortNames.insert(allShortNames.end(),
                        shortNameSet.begin(), shortNameSet.end());
}

/**
 * clear study meta data modified (prevents provenance updates).
 */
void 
StudyMetaDataFile::clearAllStudyMetaDataElementsModified()
{
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      StudyMetaData* smd = getStudyMetaData(i);
      smd->clearModified();
   }
}
                              
/**
 * check for any studies matching those in meta-analysis file.
 */
void 
StudyMetaDataFile::checkForMatchingStudies(const StudyMetaAnalysisFile* smaf,
                                           std::vector<int>& matchingStudyMetaDataFileStudyNumbers,
                                           std::vector<int>& matchingMetaAnalysisFileStudyIndices) const
{
   matchingStudyMetaDataFileStudyNumbers.clear();
   matchingMetaAnalysisFileStudyIndices.clear();
   
   const StudyNamePubMedID* metaStudies = smaf->getMetaAnalysisStudies();
   const int numMetaAnalysisStudies = metaStudies->getNumberOfStudies();
   
   //
   // Look for existing studies with matching name or PubMed ID
   //
   std::set<int> studyIndices;
   std::set<int> metaIndices;
   for (int i = 0; i < numMetaAnalysisStudies; i++) {
      QString name, pubMedID;
      metaStudies->getStudyNameAndPubMedID(i, name, pubMedID);
      const int nameIndex = getStudyIndexFromName(name);
      if (nameIndex >= 0) {
         studyIndices.insert(nameIndex);
         metaIndices.insert(i);
      }
      
      const int pubMedIndex = getStudyIndexFromPubMedID(pubMedID);
      if (pubMedIndex >= 0) {
         studyIndices.insert(pubMedIndex);
         metaIndices.insert(i);
      }
   }
   
   //
   // Remove any duplicates (must be sorted for "unique()" to function properly)
   //
   matchingStudyMetaDataFileStudyNumbers.insert(
      matchingStudyMetaDataFileStudyNumbers.end(),
      studyIndices.begin(), 
      studyIndices.end());
      
   matchingMetaAnalysisFileStudyIndices.insert(
      matchingMetaAnalysisFileStudyIndices.end(),
      metaIndices.begin(), 
      metaIndices.end());
}
                                   
/**
 * add meta-analysis study.
 */
void 
StudyMetaDataFile::addMetaAnalysisStudy(const StudyMetaAnalysisFile* smaf,
                                        const bool createMetaAnalysisStudiesFlag,
                                        const bool fetchDataFromPubMedFlag) throw (FileException)
{
   StudyMetaData* smd = new StudyMetaData(smaf);
   addStudyMetaData(smd);
   
   if (createMetaAnalysisStudiesFlag) {
      const StudyNamePubMedID* pmids = smd->getMetaAnalysisStudies();
      createStudiesFromMetaAnalysisStudiesWithPubMedDotCom(pmids,
                                                           fetchDataFromPubMedFlag);
   }
}
      
/**
 * create new studies from these associated studies.
 */
void 
StudyMetaDataFile::createStudiesFromMetaAnalysisStudiesWithPubMedDotCom(const StudyNamePubMedID* ms,
                                                                        const bool fetchDataFromPubMedFlag) throw (FileException)
{
   QString errorMessage;
   
/*
   //
   // Check for existing studies that use the "new" PubMed IDs
   //
   const int num = ms->getNumberOfStudies();
   int errorCount = 0;
   for (int i = 0; i < num; i++) {
      QString name, pubMedID;
      ms->getStudyNameAndPubMedID(i, name, pubMedID);
      const int indx = getStudyIndexFromPubMedID(pubMedID);
      if (indx >= 0) {
         if (errorMessage.isEmpty()) {
            errorMessage += "ERROR, no studies created because studies with these Indices/PubMed IDs exist:\n";
         }
         errorMessage += (" ("
                          + QString::number(indx + 1)   // index 1..N on dialog
                          + ", "
                          + pubMedID
                          + ")");
         if (errorCount >= 5) {
            errorMessage += "\n";
            errorCount = 0;
         }
         else {
            errorCount++;
         }
      }
   }      
   if (errorMessage.isEmpty() == false) {
      throw FileException(errorMessage);
   }
*/   
   
   //
   // Add the new PubMed IDs by creating new studies
   //
   const int num = ms->getNumberOfStudies();
   for (int i = 0; i < num; i++) {
      QString name, pubMedID;
      ms->getStudyNameAndPubMedID(i, name, pubMedID);
      StudyMetaData* smd = new StudyMetaData;
      smd->setName(name);
      if (pubMedID.isEmpty() == false) {
         smd->setPubMedID(pubMedID);
         
         //
         // Should data be downloaded from PubMed?
         //
         if (fetchDataFromPubMedFlag) {
            if (smd->getPubMedIDIsAProjectID() == false) {
               try {
                  smd->updateDataFromPubMedDotComUsingPubMedID();
               }
               catch (FileException& e) {
                  errorMessage += e.whatQString();
               }
            }
         }
      }
      addStudyMetaData(smd);
   }
   if (errorMessage.isEmpty() == false) {
      throw FileException(errorMessage);
   }
}
      
/**
 * retrieve data from PubMed using PubMed ID for all studies.
 */
void 
StudyMetaDataFile::updateAllStudiesWithDataFromPubMedDotCom() throw (FileException)
{
   QString errorMessage;
   
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      StudyMetaData* smd = getStudyMetaData(i);
      try {
         if (smd->getPubMedIDIsAProjectID() == false) {
            smd->updateDataFromPubMedDotComUsingPubMedID();
         }
      }
      catch (FileException& e) {
         errorMessage += e.whatQString();
      }
   }
   
   if (errorMessage.isEmpty() == false) {
      throw FileException(errorMessage);
   }
}
      
/**
 * write the file's data into a comma separated values file (throws exception if not supported).
 */
void 
StudyMetaDataFile::writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& /*csv*/) throw (FileException)
{
   throw FileException("StudyMetaDataFile cannot write to a comma separated value file.");
}


/**
 * read the file's data from a comma separated values file (throws exception if not supported).
 */
void 
StudyMetaDataFile::readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& /*csv*/) throw (FileException)
{
   throw FileException("StudyMetaDataFile cannot read from a comma separated value file.");
}


/**
 * Read the contents of the file (header has already been read).
 */
void 
StudyMetaDataFile::readFileData(QFile& file,
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
                  // Is this a "StudyMetaData" element
                  //
                  if (elem.tagName() == "StudyMetaData") {
                     StudyMetaData* smd = new StudyMetaData;
                     smd->readXML(node);
                     addStudyMetaData(smd);
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
   
   //
   // Prevent provenance updates if file is saved
   //
   clearAllStudyMetaDataElementsModified();
}


/**
 * Write the file's data (header has already been written).
 */
void 
StudyMetaDataFile::writeFileData(QTextStream& stream,
                                 QDataStream& /*binStream*/,
                                 QDomDocument& xmlDoc,
                                 QDomElement& rootElement) throw (FileException)
{
   const int num = getNumberOfStudyMetaData();

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
            // Write the study meta data entries
            //
            for (int i = 0; i < num; i++) {
               const StudyMetaData* smd = getStudyMetaData(i);
               smd->writeXML(xmlDoc, rootElement);
            }
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
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



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

#include <QDate>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>
#include <QRegExp>

#include "AbstractFile.h"
#include "CellFile.h"
#include "CellProjectionFile.h"
#include "CellStudyInfo.h"
#include "CommaSeparatedValueFile.h"
#include "FileUtilities.h"
#include "FociProjectionFile.h"
#include "PubMedArticleFile.h"
#include "SpecFile.h"
#include "StudyMetaDataFile.h"
#include "SystemUtilities.h"
#include "VocabularyFile.h"
#include "XmlGenericWriter.h"
#include "XmlGenericWriterAttributes.h"

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
   quality = smd.quality;
   species = smd.species;
   stereotaxicSpace = smd.stereotaxicSpace;
   stereotaxicSpaceDetails = smd.stereotaxicSpaceDetails;
   studyDataFormat = smd.studyDataFormat;
   studyDataType = smd.studyDataType;
   title = smd.title;
   
   //
   // DO NOT COPY THESE MEMBERS !!!
   //
   // 
   dateAndTimeStamps = "";
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
   
   const int numProv = smd.getNumberOfProvenances();
   for (int i = 0; i < numProv; i++) {
      addProvenance(new Provenance(*(smd.getProvenance(i))));
   }
   
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
   dateAndTimeStamps = "";
   pubMedID = projectID;  // user can override later
   quality = "";
   species = "";
   stereotaxicSpace = "";
   stereotaxicSpaceDetails = "";
   studyDataFormat = "";
   studyDataType = "";
   studyDataModifiedFlag = false;
   title = "";
   
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
   
   for (unsigned int i = 0; i < provenances.size(); i++) {
      delete provenances[i];
   }
   provenances.clear();
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
 * get the most recent save date.
 */
QString 
StudyMetaData::getMostRecentDateAndTimeStamp() const
{
   QString s;
   
   const QStringList sl = dateAndTimeStamps.split(";", QString::SkipEmptyParts);
   if (sl.count() > 0) {
      s = sl.at(0);
   }
   
   return s;
}      
      
/**
 * add a provenance.
 */
void 
StudyMetaData::addProvenance(Provenance* p)
{
   p->setParent(this);
   provenances.push_back(p);
   setModified();
}

/**
 * delete a provenance.
 */
void 
StudyMetaData::deleteProvenance(const int indx)
{
   delete provenances[indx];
   provenances.erase(provenances.begin() + indx);
   setModified();
}

/**
 * delete a provenance.
 */
void 
StudyMetaData::deleteProvenance(const Provenance* p)
{
   for (int i = 0; i < getNumberOfProvenances(); i++) {
      if (getProvenance(i) == p) {
         deleteProvenance(i);
         break;
      }
   }
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
           (quality == cci.quality) &&
           (species == cci.species) &&
           (stereotaxicSpace == cci.stereotaxicSpace) &&
           (stereotaxicSpaceDetails == cci.stereotaxicSpaceDetails) &&
           (studyDataFormat == cci.studyDataFormat) &&
           (studyDataType == cci.studyDataType) &&
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
         else if ((elem.tagName() == "provenanceDateAndTimeStamps") ||
                  (elem.tagName() == "dateAndTimeStamps")) {
            dateAndTimeStamps = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "pubMedID") {
            pubMedID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "quality") {
            quality = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "species") {
            species = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "stereotaxicSpace") {
            stereotaxicSpace = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "stereotaxicSpaceDetails") {
            stereotaxicSpaceDetails = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "studyDataFormat") {
            studyDataFormat = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "studyDataType") {
            studyDataType = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "title") {
            title = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "mslID") {
            mslID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "parentID") {
            parentID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "coreDataCompleted") {
            coreDataCompleted = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "completed") {
            completed = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "publicAccess") {
            publicAccess = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "url") {  // obsolete
            oldURL = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "provenance") {
            //
            // Obsolete element, ignore it
            //
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
         else if (elem.tagName() == "StudyMetaDataProvenance") {
            Provenance* p = new Provenance;
            p->readXML(node);
            addProvenance(p);
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
   // Update data and time stamp if modified flag is set
   //
   if (studyDataModifiedFlag) {
      dateAndTimeStamps = AbstractFile::generateDateAndTimeStamp()
                                  + ";"
                                  + dateAndTimeStamps;
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
   AbstractFile::addXmlCdataElement( xmlDoc, studyElement, "dateAndTimeStamps", dateAndTimeStamps);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "documentObjectIdentifier", documentObjectIdentifier);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "keywords", keywords);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "mesh", medicalSubjectHeadings);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "name", name);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "partitioningSchemeAbbreviation", partitioningSchemeAbbreviation);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "partitioningSchemeFullName", partitioningSchemeFullName);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "projectID", projectID);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "pubMedID", pubMedID);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "quality", quality);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "species", species); 
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "stereotaxicSpace", stereotaxicSpace);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "stereotaxicSpaceDetails", stereotaxicSpaceDetails);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "studyDataFormat", studyDataFormat);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "studyDataType", studyDataType);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "title", title);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "mslID", mslID);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "parentID", parentID);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "coreDataCompleted", coreDataCompleted);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "completed", completed);
   AbstractFile::addXmlCdataElement(xmlDoc, studyElement, "publicAccess", publicAccess);

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
   // Add provenances
   //
   for (int i = 0; i < getNumberOfProvenances(); i++) {
      provenances[i]->writeXML(xmlDoc, studyElement);
   }
      
   //
   // Add to parent
   //
   parentElement.appendChild(studyElement);
}

/**
 * called to write XML.
 */
void
StudyMetaData::writeXML(XmlGenericWriter& xmlWriter, int indx) const throw (FileException)
{
   //
   // Update data and time stamp if modified flag is set
   //
   if (studyDataModifiedFlag) {
      dateAndTimeStamps = AbstractFile::generateDateAndTimeStamp()
                                  + ";"
                                  + dateAndTimeStamps;
      studyDataModifiedFlag = false;
   }

   //
   // Create the element for this class instance's data
   //
   XmlGenericWriterAttributes atts;
   atts.addAttribute("Index", indx);
   xmlWriter.writeStartElement("StudyMetaData", atts);

   //
   // Add the study metadata
   //
   xmlWriter.writeElementCData("authors", authors);
   xmlWriter.writeElementCData("citation", citation);
   xmlWriter.writeElementCData("comment", comment);
   xmlWriter.writeElementCData("dateAndTimeStamps", dateAndTimeStamps);
   xmlWriter.writeElementCData("documentObjectIdentifier", documentObjectIdentifier);
   xmlWriter.writeElementCData("keywords", keywords);
   xmlWriter.writeElementCData("mesh", medicalSubjectHeadings);
   xmlWriter.writeElementCData("name", name);
   xmlWriter.writeElementCData("partitioningSchemeAbbreviation", partitioningSchemeAbbreviation);
   xmlWriter.writeElementCData("partitioningSchemeFullName", partitioningSchemeFullName);
   xmlWriter.writeElementCData("projectID", projectID);
   xmlWriter.writeElementCData("pubMedID", pubMedID);
   xmlWriter.writeElementCData("quality", quality);
   xmlWriter.writeElementCData("species", species);
   xmlWriter.writeElementCData("stereotaxicSpace", stereotaxicSpace);
   xmlWriter.writeElementCData("stereotaxicSpaceDetails", stereotaxicSpaceDetails);
   xmlWriter.writeElementCData("studyDataFormat", studyDataFormat);
   xmlWriter.writeElementCData("studyDataType", studyDataType);
   xmlWriter.writeElementCData("title", title);
   xmlWriter.writeElementCData("mslID", mslID);
   xmlWriter.writeElementCData("parentID", parentID);
   xmlWriter.writeElementCData("coreDataCompleted", coreDataCompleted);
   xmlWriter.writeElementCData("completed", completed);
   xmlWriter.writeElementCData("publicAccess", publicAccess);

   //
   // Add tables
   //
   for (int i = 0; i < getNumberOfTables(); i++) {
      tables[i]->writeXML(xmlWriter);
   }

   //
   // Add figures
   //
   for (int i = 0; i < getNumberOfFigures(); i++) {
      figures[i]->writeXML(xmlWriter);
   }

   //
   // Add page references
   //
   for (int i = 0; i < getNumberOfPageReferences(); i++) {
      pageReferences[i]->writeXML(xmlWriter);
   }

   //
   // Add provenances
   //
   for (int i = 0; i < getNumberOfProvenances(); i++) {
      provenances[i]->writeXML(xmlWriter);
   }

   xmlWriter.writeEndElement();
}

/**
 * get the study data format entries.
 */
void 
StudyMetaData::getStudyDataFormatEntries(std::vector<QString>& entries)
{
   entries.clear();
   entries.push_back("Stereotaxic Foci");
   entries.push_back("Metric");
   entries.push_back("Surface Shape");
   entries.push_back("Paint");
   entries.push_back("Volume");
   entries.push_back("Surface");
   std::sort(entries.begin(), entries.end());
}

/**
 * get the study data type entries.
 */
void 
StudyMetaData::getStudyDataTypeEntries(std::vector<QString>& entries)
{
   entries.clear();
   entries.push_back("fMRI");
   entries.push_back("PET");
   entries.push_back("Morphometry");
   entries.push_back("Connectivity");
   entries.push_back("Partitioning Scheme");
   entries.push_back("ERP");
   entries.push_back("VEP");
   entries.push_back("Lesion");
   entries.push_back("EEG");
   entries.push_back("TMS");
   entries.push_back("DTI");
   entries.push_back("MEG");
   entries.push_back("Single Unit Neurophysiology");
   entries.push_back("Multi-Unit Neurophysiology");
   std::sort(entries.begin(), entries.end());
   entries.push_back("Other");
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
 * set the quality.
 */
void 
StudyMetaData::setQuality(const QString& s)
{
   if (quality != s) {
      quality = s;
      setModified();
   }
}
      
/**
 * set the study data format.
 */
void 
StudyMetaData::setStudyDataFormat(const QString& s)
{
   if (studyDataFormat != s) {
      studyDataFormat = s;
      setModified();
   }
}

/**
 * set the study data type.
 */
void 
StudyMetaData::setStudyDataType(const QString& s)
{
   if (studyDataType != s) {
      studyDataType = s;
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
 * set MSL ID.
 */
void 
StudyMetaData::setMslID(const QString& s)
{
   if (mslID != s) {
      mslID = s;
      setModified(); 
   }
}

/**
 * set parent ID.
 */
void  
StudyMetaData::setParentID(const QString& s)
{
   if (parentID != s) {
      parentID = s;
      setModified(); 
   }
}

/**
 * set core metadata.
 */
void  
StudyMetaData::setCoreDataCompleted(const QString& s)
{
   if (coreDataCompleted != s) {
      coreDataCompleted = s;
      setModified(); 
   }
}

/**
 * set completed.
 */
void  
StudyMetaData::setCompleted(const QString& s)
{
   if (completed != s) {
      completed = s;
      setModified(); 
   }
}

/**
 * set public access.
 */
void 
StudyMetaData::setPublicAccess(const QString& s)
{
   if (publicAccess != s) {
      publicAccess = s;
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
 * set species.
 */
void 
StudyMetaData::setSpecies(const QString& s)
{
   if (species != s) {
      species = s;
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
 * set the date and time stamps.
 */
void 
StudyMetaData::setDateAndTimeStamps(const QString& p)
{
   dateAndTimeStamps = p;
   
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
 * get medical subject headings.
 */
void 
StudyMetaData::getMedicalSubjectHeadings(std::vector<QString>& meshOut) const
{
   meshOut.clear();
   
   const QStringList sl = getMedicalSubjectHeadings().split(';', QString::SkipEmptyParts);
   for (int k = 0; k < sl.size(); k++) {
      const QString m = sl.at(k).trimmed();
      if (m.isEmpty() == false) {
         meshOut.push_back(m);
      }
   }
}
      
/**
 * get the keywords.
 */
void 
StudyMetaData::getKeywords(std::vector<QString>& keywordsOut) const
{
   static QRegExp regEx(";|\\*|\\|");
   
   keywordsOut.clear();
   
   const QStringList sl = getKeywords().split(regEx, QString::SkipEmptyParts);
   for (int k = 0; k < sl.size(); k++) {
      const QString kw = sl.at(k).trimmed();
      if (kw.isEmpty() == false) {
         keywordsOut.push_back(kw);
      }
   }
}

/**
 * see if study contains a keyword.
 */
bool 
StudyMetaData::containsKeyword(const QString& kw) const
{
   std::vector<QString> words;
   getKeywords(words);
   if (std::find(words.begin(), words.end(), kw) != words.end()) {
      return true;
   }
   return false;
}
      
/**
 * see if study contains a sub header short names.
 */
bool 
StudyMetaData::containsSubHeaderShortName(const QString& shsn) const
{
   std::vector<QString> shortNames;
   getAllTableSubHeaderShortNames(shortNames);
   if (std::find(shortNames.begin(), shortNames.end(), shsn) != shortNames.end()) {
      return true;
   }
   return false;
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
      
/**
 * get all table headers.
 */
void 
StudyMetaData::getAllTableHeaders(std::vector<QString>& headersOut) const
{
   headersOut.clear();
   
   const int numTables = getNumberOfTables();
   for (int i = 0; i < numTables; i++) {
      const Table* table = getTable(i);
      const QString h = table->getHeader().trimmed();
      if (h.isEmpty() == false) {
         headersOut.push_back(h);
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
 * called to write XML.
 */
void
StudyMetaData::Figure::writeXML(XmlGenericWriter& xmlWriter) const throw (FileException)
{
   xmlWriter.writeStartElement("StudyMetaDataFigure");
   xmlWriter.writeElementCData("legend", legend);
   xmlWriter.writeElementCData("number", number);
   for (int i = 0; i < getNumberOfPanels(); i++) {
      panels[i]->writeXML(xmlWriter);
   }
   xmlWriter.writeEndElement();
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
 * called to write XML.
 */
void
StudyMetaData::Figure::Panel::writeXML(XmlGenericWriter& xmlWriter) const throw (FileException)
{
   xmlWriter.writeStartElement("StudyMetaDataFigurePanel");
   xmlWriter.writeElementCData("description", description);
   xmlWriter.writeElementCData("panelNumberOrLetter", panelNumberOrLetter);
   xmlWriter.writeElementCData("taskDescription", taskDescription);
   xmlWriter.writeElementCData("taskBaseline", taskBaseline);
   xmlWriter.writeElementCData("testAttributes", testAttributes);
   xmlWriter.writeEndElement();
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
 * called to write XML.
 */
void
StudyMetaData::Table::writeXML(XmlGenericWriter& xmlWriter) const throw (FileException)
{
   xmlWriter.writeStartElement("StudyMetaDataTable");
   xmlWriter.writeElementCData("footer", footer);
   xmlWriter.writeElementCData("header", header);
   xmlWriter.writeElementCData("number", number);
   xmlWriter.writeElementCData("sizeUnits", sizeUnits);
   xmlWriter.writeElementCData("statisticType", statisticType);
   xmlWriter.writeElementCData("statisticDescription", statisticDescription);
   xmlWriter.writeElementCData("voxelDimensions", voxelDimensions);
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      subHeaders[i]->writeXML(xmlWriter);
   }
   xmlWriter.writeEndElement();
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
 * called to write XML.
 */
void
StudyMetaData::SubHeader::writeXML(XmlGenericWriter& xmlWriter) const throw (FileException)
{
   xmlWriter.writeStartElement("StudyMetaDataSubHeader");
   xmlWriter.writeElementCData("name", name);
   xmlWriter.writeElementCData("number", number);
   xmlWriter.writeElementCData("shortName", shortName);
   xmlWriter.writeElementCData("taskDescription", taskDescription);
   xmlWriter.writeElementCData("taskBaseline", taskBaseline);
   xmlWriter.writeElementCData("testAttributes", testAttributes);
   xmlWriter.writeEndElement();
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
 * called to write XML.
 */
void
StudyMetaData::PageReference::writeXML(XmlGenericWriter& xmlWriter) const throw (FileException)
{
   xmlWriter.writeStartElement("StudyMetaDataPageReference");
   xmlWriter.writeElementCData("pageNumber", pageNumber);
   xmlWriter.writeElementCData("header", header);
   xmlWriter.writeElementCData("comment", comment);
   xmlWriter.writeElementCData("sizeUnits", sizeUnits);
   xmlWriter.writeElementCData("voxelDimensions", voxelDimensions);
   xmlWriter.writeElementCData("statisticType", statisticType);
   xmlWriter.writeElementCData("statisticDescription", statisticDescription);
   for (int i = 0; i < getNumberOfSubHeaders(); i++) {
      subHeaders[i]->writeXML(xmlWriter);
   }
   xmlWriter.writeEndElement();
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
// Provenance class
//
//====================================================================================

/**
 * constructor.
 */
StudyMetaData::Provenance::Provenance()
{
   parentStudyMetaData = NULL;
   clear();
}

/**
 * destructor.
 */
StudyMetaData::Provenance::~Provenance()
{
   clear();
}

/**
 * copy constructor.
 */
StudyMetaData::Provenance::Provenance(const Provenance& p)
{
   parentStudyMetaData = NULL;
   copyHelper(p);
}

/**
 * assignment operator.
 */
StudyMetaData::Provenance& 
StudyMetaData::Provenance::operator=(const Provenance& p)
{
   if (this != &p) {
      copyHelper(p);
   }
   return *this;
}

/**
 * clear the page link.
 */
void 
StudyMetaData::Provenance::clear()
{
   name = SystemUtilities::getUserName();
   date = QDate::currentDate().toString("dd MMM yyyy");
   comment = "";
}

/**
 * set the name.
 */
void 
StudyMetaData::Provenance::setName(const QString& s)
{
   if (name != s) {
      name = s;
      setModified();
   }
}

/**
 * set the date.
 */
void 
StudyMetaData::Provenance::setDate(const QString& s)
{
   if (date != s) {
      date = s;
      setModified();
   }
}

/**
 * set the comment.
 */
void 
StudyMetaData::Provenance::setComment(const QString& s)
{
   if (comment != s) {
      comment = s;
      setModified();
   }
}

/**
 * called to read from XML.
 */
void 
StudyMetaData::Provenance::readXML(QDomNode& nodeIn) throw (FileException)
{
   name = "";
   date = "";
   comment = "";
   
   if (nodeIn.isNull()) {      
      return;   
   }   
   QDomElement elem = nodeIn.toElement();   
   if (elem.isNull()) {      
      return;   
   }   
   if (elem.tagName() != "StudyMetaDataProvenance") {      
      QString msg("Incorrect element type passed to StudyMetaData::Provenance::readXML() ");
      msg.append(elem.tagName());      
      throw FileException(msg);   
   } 
   QDomNode node = nodeIn.firstChild();   
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();      
      if (elem.isNull() == false) {         
         if (elem.tagName() == "name") {            
            name = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }         
         else if (elem.tagName() == "date") {            
            date = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }
         else if (elem.tagName() == "comment") {            
            comment = AbstractFile::getXmlElementFirstChildAsString(elem);         
         }
         else {
            std::cout << "WARNING: unrecognized StudyMetaData::Provenance element ignored: "
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
StudyMetaData::Provenance::writeXML(QDomDocument& xmlDoc,
                                    QDomElement&  parentElement) const throw (FileException)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement element = xmlDoc.createElement("StudyMetaDataProvenance");

   //
   // write data
   //
   AbstractFile::addXmlCdataElement(xmlDoc, element, "name", name);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "date", date);
   AbstractFile::addXmlCdataElement(xmlDoc, element, "comment", comment);
   
   //
   // Add to parent
   //
   parentElement.appendChild(element);
}

/**
 * called to write XML.
 */
void
StudyMetaData::Provenance::writeXML(XmlGenericWriter& xmlWriter) const throw (FileException)
{
   xmlWriter.writeStartElement("StudyMetaDataProvenance");
   xmlWriter.writeElementCData("name", name);
   xmlWriter.writeElementCData("date", date);
   xmlWriter.writeElementCData("comment", comment);
   xmlWriter.writeEndElement();
}


/**
 * set parent.
 */
void 
StudyMetaData::Provenance::setParent(StudyMetaData* parentStudyMetaDataIn)
{
   parentStudyMetaData = parentStudyMetaDataIn;
}

/**
 * set modified.
 */
void 
StudyMetaData::Provenance::setModified()
{
   if (parentStudyMetaData != NULL) {
      parentStudyMetaData->setModified();
   }
}

/**
 * copy helper.
 */
void 
StudyMetaData::Provenance::copyHelper(const Provenance& p)
{
   StudyMetaData* savedParentStudyMetaData = parentStudyMetaData;
   
   clear();
   name    = p.name;
   date    = p.date;
   comment = p.comment;
   
   parentStudyMetaData = savedParentStudyMetaData;
   setModified();
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
                  FILE_IO_NONE)     // csvf
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
      StudyMetaDataLinkSet smdls;
      smdls.addStudyMetaDataLink(smdl);
      const int numCells = cf.getNumberOfCells();
      for (int j = 0; j < numCells; j++) {
         CellData* cd = cf.getCell(j);
         if (cd->getStudyNumber() == i) {
            cd->setStudyMetaDataLinkSet(smdls);
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
      StudyMetaDataLinkSet smdls;
      smdls.addStudyMetaDataLink(smdl);
      const int numCellProjections = cpf.getNumberOfCellProjections();
      for (int j = 0; j < numCellProjections; j++) {
         CellProjection* cp = cpf.getCellProjection(j);
         if (cp->getStudyNumber() == i) {
            cp->setStudyMetaDataLinkSet(smdls);
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
      StudyMetaDataLinkSet smdls;
      smdls.addStudyMetaDataLink(smdl);
      const int numVocabularyEntries = vf.getNumberOfVocabularyEntries();
      for (int j = 0; j < numVocabularyEntries; j++) {
         VocabularyFile::VocabularyEntry* ve = vf.getVocabularyEntry(j);
         if (ve->getStudyNumber() == i) {
            ve->setStudyMetaDataLinkSet(smdls);
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
StudyMetaDataFile::getStudyIndexFromLink(const StudyMetaDataLink& smdl) const
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
 * get all medical subject headings.
 */
void 
StudyMetaDataFile::getAllMedicalSubjectHeadings(std::vector<QString>& meshOut) const
{
   meshOut.clear();
   
   std::set<QString> meshSet;
   
   const int numStudies = getNumberOfStudyMetaData();
   for (int i = 0; i < numStudies; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      std::vector<QString> mesh;
      smd->getMedicalSubjectHeadings(mesh);
      meshSet.insert(mesh.begin(), mesh.end());
   }
   
   meshOut.insert(meshOut.end(),
                  meshSet.begin(), meshSet.end());
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
 * get all citations.
 */
void 
StudyMetaDataFile::getAllCitations(std::vector<QString>& allCitations) const
{
   allCitations.clear();
   
   //
   // Get keywords from studies used by displayed foci
   //
   const int numStudies = getNumberOfStudyMetaData();
   std::set<QString> citationSet;
   for (int i = 0; i < numStudies; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      const QString citation = smd->getCitation();
      if (citation.isEmpty() == false) {
         citationSet.insert(citation);
      }
   }
   
   allCitations.insert(allCitations.end(),
                       citationSet.begin(), citationSet.end());
}

/**
 * get all data formats.
 */
void 
StudyMetaDataFile::getAllDataFormats(std::vector<QString>& allDataFormats) const
{
   allDataFormats.clear();
   
   const int numStudies = getNumberOfStudyMetaData();
   std::set<QString> dataFormatSet;
   for (int i = 0; i < numStudies; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      const QString dataFormat = smd->getStudyDataFormat();
      if (dataFormat.isEmpty() == false) {
         dataFormatSet.insert(dataFormat);
      }
   }
   
   allDataFormats.insert(allDataFormats.end(),
                         dataFormatSet.begin(), dataFormatSet.end());
}

/**
 * get all data types.
 */
void 
StudyMetaDataFile::getAllDataTypes(std::vector<QString>& allDataTypes) const
{
   allDataTypes.clear();
   
   const int numStudies = getNumberOfStudyMetaData();
   std::set<QString> dataTypeSet;
   for (int i = 0; i < numStudies; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      const QString dataType = smd->getStudyDataType();
      if (dataType.isEmpty() == false) {
         dataTypeSet.insert(dataType);
      }
   }
   
   allDataTypes.insert(allDataTypes.end(),
                       dataTypeSet.begin(), dataTypeSet.end());
}
 
/**
 * get all keywords used by displayed foci.
 */
void 
StudyMetaDataFile::getAllKeywordsUsedByDisplayedFoci(const FociProjectionFile* fpf,
                                            std::vector<QString>& keywordsOut) const
{
   keywordsOut.clear();
   
   //
   // Get studies used by foci
   //
   std::vector<bool> studyIsUsedByFocus;
   getStudiesLinkedByDisplayedFoci(fpf, studyIsUsedByFocus);
   const int numStudies = static_cast<int>(studyIsUsedByFocus.size());
   
   //
   // Get keywords from studies used by displayed foci
   //
   std::set<QString> keywordSet;
   for (int i = 0; i < numStudies; i++) {
      if (studyIsUsedByFocus[i]) {
         const StudyMetaData* smd = getStudyMetaData(i);
         std::vector<QString> studyKeywords;
         smd->getKeywords(studyKeywords);
         keywordSet.insert(studyKeywords.begin(), studyKeywords.end());
      }
   }
   
   keywordsOut.insert(keywordsOut.end(),
                      keywordSet.begin(), keywordSet.end());
   
/*   
   const int numStudies = getNumberOfStudyMetaData();
   std::vector<bool> studyIsUsedByFocus(numStudies, false);
   
   //
   // Find studies used by displayed foci
   //
   const int numFoci = fpf->getNumberOfCellProjections();
   for (int i = 0; i < numFoci; i++) {
      const CellProjection* focus = fpf->getCellProjection(i);
      if (focus->getDisplayFlag()) {
         //
         // Loop through studies used by focus
         //      
         const StudyMetaDataLinkSet smdls = focus->getStudyMetaDataLinkSet();
         const int numLinks = smdls.getNumberOfStudyMetaDataLinks();
         for (int j = 0; j < numLinks; j++) {
            const StudyMetaDataLink smdl = smdls.getStudyMetaDataLink(j);
            const int studyIndex = getStudyIndexFromLink(smdl);
            if ((studyIndex >= 0) && 
                (studyIndex < numStudies)) {
               studyIsUsedByFocus[studyIndex] = true;
            }
         }
      }
   }

   //
   // Get keywords from studies used by displayed foci
   //
   std::set<QString> keywordSet;
   for (int i = 0; i < numStudies; i++) {
      if (studyIsUsedByFocus[i]) {
         const StudyMetaData* smd = getStudyMetaData(i);
         std::vector<QString> studyKeywords;
         smd->getKeywords(studyKeywords);
         keywordSet.insert(studyKeywords.begin(), studyKeywords.end());
      }
   }
   
   keywordsOut.insert(keywordsOut.end(),
                      keywordSet.begin(), keywordSet.end());
*/
}
      
/**
 * get studies that are linked by displayed foci.
 */
void 
StudyMetaDataFile::getStudiesLinkedByDisplayedFoci(const FociProjectionFile* fpf,
                                                std::vector<bool>& studyLinkedByFocusOut) const
{
   const int numStudies = getNumberOfStudyMetaData();
   studyLinkedByFocusOut.resize(numStudies);
   std::fill(studyLinkedByFocusOut.begin(), studyLinkedByFocusOut.end(), false);
   
   //
   // Find studies used by displayed foci
   //
   const int numFoci = fpf->getNumberOfCellProjections();
   for (int i = 0; i < numFoci; i++) {
      const CellProjection* focus = fpf->getCellProjection(i);
      if (focus->getDisplayFlag()) {
         //
         // Loop through studies used by focus
         //      
         const StudyMetaDataLinkSet smdls = focus->getStudyMetaDataLinkSet();
         const int numLinks = smdls.getNumberOfStudyMetaDataLinks();
         for (int j = 0; j < numLinks; j++) {
            const StudyMetaDataLink smdl = smdls.getStudyMetaDataLink(j);
            const int studyIndex = getStudyIndexFromLink(smdl);
            if ((studyIndex >= 0) && 
                (studyIndex < numStudies)) {
               studyLinkedByFocusOut[studyIndex] = true;
            }
         }
      }
   }
}
      
/**
 * get all table headers.
 */
void 
StudyMetaDataFile::getAllTableHeaders(std::vector<QString>& headersOut) const
{
   headersOut.clear();

   std::set<QString> headerSet;
   
   const int numStudies = getNumberOfStudyMetaData();
   for (int i = 0; i < numStudies; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      std::vector<QString> tableHeaders;
      smd->getAllTableHeaders(tableHeaders);
      headerSet.insert(tableHeaders.begin(), tableHeaders.end());
   }
   
   headersOut.insert(headersOut.end(),
                     headerSet.begin(), headerSet.end());
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
 * get all table subheader short names used by displayed foci.
 */
void 
StudyMetaDataFile::getAllTableSubHeaderShortNamesUsedByDisplayedFoci(
                                          const FociProjectionFile* fpf,
                                          std::vector<QString>& allShortNamesOut) const
{
   allShortNamesOut.clear();
   
   //
   // Get studies used by foci
   //
   std::vector<bool> studyIsUsedByFocus;
   getStudiesLinkedByDisplayedFoci(fpf, studyIsUsedByFocus);
   const int numStudies = static_cast<int>(studyIsUsedByFocus.size());
   
   std::set<QString> shortNameSet;
   
   //
   // Get names used by displayed foci
   //
   for (int i = 0; i < numStudies; i++) {
      if (studyIsUsedByFocus[i]) {
         const StudyMetaData* smd = getStudyMetaData(i);
         std::vector<QString> studyTableSubHeaderShortNames;
         smd->getAllTableSubHeaderShortNames(studyTableSubHeaderShortNames);
         shortNameSet.insert(studyTableSubHeaderShortNames.begin(), studyTableSubHeaderShortNames.end());
      }
   }
   
   allShortNamesOut.insert(allShortNamesOut.end(),
                           shortNameSet.begin(), shortNameSet.end());
}
      
/**
 * clear study meta data modified (prevents date and time stamp updates).
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
 * count the number of studies without at least one provenance entry.
 */
int 
StudyMetaDataFile::getNumberOfStudyMetaDatWithoutProvenceEntries() const
{
   int count = 0;
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      if (smd->getNumberOfProvenances() <= 0) {
         count++;
      }
   }
   return count;
}

/**
 * update all studies without a provenance entry.
 */
void 
StudyMetaDataFile::addProvenanceToStudiesWithoutProvenanceEntries(const QString& name,
                                                                  const QString& date,
                                                                  const QString& comment)
{
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      StudyMetaData* smd = getStudyMetaData(i);
      if (smd->getNumberOfProvenances() <= 0) {
         StudyMetaData::Provenance* p = new StudyMetaData::Provenance;
         p->setName(name);
         p->setDate(date);
         p->setComment(comment);
         smd->addProvenance(p);
      }
   }
}
                                                          
/**
 * Read the contents of the file (header has already been read).
 */
void 
StudyMetaDataFile::readFileData(QFile& /*file*/,
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
         throw FileException(filename, "Reading in Comma Separated File format not supported.");
         break;
   }
   
   //
   // Prevent data and time stamp updates if file is saved
   //
   clearAllStudyMetaDataElementsModified();
}


/**
 * Write the file's data (header has already been written).
 */
void 
StudyMetaDataFile::writeFileData(QTextStream& /*stream*/,
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
         throw FileException(filename, "Writing in Comma Separated File format not supported.");
         break;
   }
}

/**
 * find duplicate studies (map key is PubMedID, value is filename).
 */
void 
StudyMetaDataFile::findDuplicateStudies(const std::vector<QString>& studyFileNames,
                                        std::multimap<QString,QString>& duplicatesStudiesOut)
                                                             throw (FileException)
{
   duplicatesStudiesOut.clear();
   
   const int numFiles = static_cast<int>(studyFileNames.size());
   if (numFiles <= 0) {
      throw FileException("No filenames provided.");
   }
   
   //
   // keep track of PubMedIDs and files
   //
   std::set<QString> pubMedIDs;
   std::multimap<QString,QString> pubMedIDsAndFiles;
   
   //
   // Loop through the study files
   //
   for (int i = 0; i < numFiles; i++) {
      //
      // Read the study file
      //
      StudyMetaDataFile studyFile;
      studyFile.readFile(studyFileNames[i]);
      const QString fileNameNoPath(FileUtilities::basename(studyFileNames[i]));
      //
      // loop through the studies
      //
      const int numStudies = studyFile.getNumberOfStudyMetaData();
      for (int j = 0; j < numStudies; j++) {
         //
         // Get the studies PubMedID
         //
         const StudyMetaData* smd = studyFile.getStudyMetaData(j);
         const QString pubMedID = smd->getPubMedID();
         pubMedIDs.insert(pubMedID);

         //
         // file name and study name
         //
         QString name = (fileNameNoPath + "    " + smd->getName());
         
         //
         // Insert into map
         //
         pubMedIDsAndFiles.insert(std::make_pair(pubMedID, name));         
      }
   }
   
   //
   // Look for Duplicates
   //
   for (std::set<QString>::iterator iter = pubMedIDs.begin();
        iter != pubMedIDs.end();
        iter++) {
      //
      // Is PubMedID used more than once
      //
      const QString pubMedID = *iter;
      if (pubMedIDsAndFiles.count(pubMedID) > 1) {
         for (std::multimap<QString,QString>::iterator pos = 
                 pubMedIDsAndFiles.lower_bound(pubMedID);
              pos != pubMedIDsAndFiles.upper_bound(pubMedID);
              pos++) {
            //
            // Add to output
            //
            duplicatesStudiesOut.insert(std::make_pair(pubMedID, pos->second));
         }
      }
   }
}
         
/**
 * Validate the study metadata file (missing table numbers, duplicate studies, etc).
 */
QStringList 
StudyMetaDataFile::validStudyMetaDataFile() const
{
   QStringList sl;
   
   const int num = getNumberOfStudyMetaData();
   for (int i = 0; i < num; i++) {
      const QString studyNumberText = 
               "Study Number " + QString::number(i + 1) + " ";
      
      const StudyMetaData* smd = getStudyMetaData(i);
      
      //
      // Check figures
      //
      const int numFigures = smd->getNumberOfFigures();
      for (int j = 0; j < numFigures; j++) {
         const StudyMetaData::Figure* figure = smd->getFigure(j);
         if (figure->getNumber().isEmpty()) {
            sl += studyNumberText + "has figure missing its number";
         }
         const int numPanels = figure->getNumberOfPanels();
         for (int k = 0; k < numPanels; k++) {
            const StudyMetaData::Figure::Panel* panel = figure->getPanel(k);
            if (panel->getPanelNumberOrLetter().isEmpty()) {
               sl += studyNumberText + "has figure panel missing its identifier";
            }
         }
      }
      
      //
      // Check page references
      //
      const int numPageRefs = smd->getNumberOfPageReferences();
      for (int j = 0; j < numPageRefs; j++) {
         const StudyMetaData::PageReference* pageRef= smd->getPageReference(j);
         if (pageRef->getPageNumber().isEmpty()) {
             sl += studyNumberText + "has table missing its page number";
         }
         const int numSubHeaders = pageRef->getNumberOfSubHeaders();
         for (int k = 0; k < numSubHeaders; k++) {
            const StudyMetaData::SubHeader* sh = pageRef->getSubHeader(k);
            if (sh->getNumber().isEmpty()) {
               sl += studyNumberText + "has page-ref subheader missing its number";
            }
            if (sh->getShortName().isEmpty()) {
               sl += studyNumberText + "has page-ref subheader missing its short name";
            }
         }         
      }
      //
      // Check tables
      //
      const int numTables = smd->getNumberOfTables();
      for (int j = 0; j < numTables; j++) {
         const StudyMetaData::Table* table = smd->getTable(j);
         if (table->getNumber().isEmpty()) {
             sl += studyNumberText + "has table missing its number";
         }
         const int numSubHeaders = table->getNumberOfSubHeaders();
         for (int k = 0; k < numSubHeaders; k++) {
            const StudyMetaData::SubHeader* sh = table->getSubHeader(k);
            if (sh->getNumber().isEmpty()) {
               sl += studyNumberText + "has table subheader missing its number";
            }
            if (sh->getShortName().isEmpty()) {
               sl += studyNumberText + "has table subheader missing its short name";
            }
         }         
      }
   }

   //
   // Look for duplicate studies names or pubmed id's
   //
   for (int i = 0; i < (num - 1); i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      const QString studyName = smd->getName();
      if (studyName.isEmpty()) {
         sl += "Study " + QString::number(i + 1) + " has no name";
      }
      else {
         bool found = false;
         QString msg;
         for (int j = i + 1; j < num; j++) {
            const StudyMetaData* smd2 = getStudyMetaData(j);
            if (smd2->getName() == studyName) {
                if (found == false) {
                   msg += "Multiple studies with name " + studyName + ":";
                   msg += " " + QString::number(i + 1);
                   found = true;
                }
                msg += " " + QString::number(j + 1);
            }
         }
         
         if (msg.isEmpty() == false) {
            sl += msg;
         }
      }
      
      const QString pubMedID = smd->getPubMedID();
      if (pubMedID.isEmpty()) {
         sl += "Study " + QString::number(i + 1) + " has no PubMed ID";
      }
      else {
         QString msg;
         bool found = false;
         for (int j = i + 1; j < num; j++) {
            const StudyMetaData* smd2 = getStudyMetaData(j);
            if (smd2->getPubMedID() == pubMedID) {
                if (found == false) {
                   msg += "Multiple studies with PubMed ID " + pubMedID + ":";
                   msg += " " + QString::number(i + 1);
                   found = true;
                }
                msg += " " + QString::number(j + 1);
            }
         }
         
         if (msg.isEmpty() == false) {
            sl += msg;
         }
      }
   }
   
   return sl;
}      

/**
 * Write the file's memory in caret6 format to the specified name.
 */
QString
StudyMetaDataFile::writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException)
{
   int numStudies = this->getNumberOfStudyMetaData();
   if (numStudies <= 0) {
      throw FileException("Contains no studies");
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
   attributes.addAttribute("CaretFileType", "StudyMetaData");
   attributes.addAttribute("xmlns:xsi",
                           "http://www.w3.org/2001/XMLSchema-instance");
   attributes.addAttribute("xsi:noNamespaceSchemaLocation",
                           "http://brainvis.wustl.edu/caret6/xml_schemas/StudyMetaDataFileSchema.xsd");
   attributes.addAttribute("Version", "6.0");
   xmlWriter.writeStartElement("CaretDataFile", attributes);

   this->writeHeaderXMLWriter(xmlWriter);

   for (int i = 0; i < numStudies; i++) {
      const StudyMetaData* smd = getStudyMetaData(i);
      smd->writeXML(xmlWriter, i);
   }

   xmlWriter.writeEndElement();

   xmlWriter.writeEndDocument();

   file.close();

   return filenameIn;
}


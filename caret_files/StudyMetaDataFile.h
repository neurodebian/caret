
#ifndef __STUDY_META_DATA_FILE_H__
#define __STUDY_META_DATA_FILE_H__

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

#include <map>

#include "AbstractFile.h"
#include "StudyNamePubMedID.h"

class CellFile;
class CellProjectionFile;
class CellStudyInfo;
class QDomDocument;
class QDomElement;
class QDomNode;
class StringTable;
class StudyMetaAnalysisFile;
class StudyMetaDataFile;
class StudyMetaDataLink;
class StudyMetaDataTextDisplayFilter;
class VocabularyFile;

/// Class for storing study meta data
class StudyMetaData {
   public: 
   
      /// Figure Class
      class Figure {
         public:
            /// figure panel class
            class Panel {
               public:
                  // constructor
                  Panel();
                  
                  // destructor
                  ~Panel();
                  
                  // copy constructor
                  Panel(const Panel& p);
                  
                  // assignment operator
                  Panel& operator=(const Panel& p);
                  
                  // clear the panel's data
                  void clear();
                  
                  /// get the panel number/letter
                  QString getPanelNumberOrLetter() const { return panelNumberOrLetter; }
                  
                  // set the panel number/letter
                  void setPanelNumberOrLetter(const QString& s);
                  
                  /// get the description
                  QString getDescription() const { return description; }
                  
                  // set the description
                  void setDescription(const QString& s);
                  
                  /// get task description
                  QString getTaskDescription() const { return taskDescription; }
                  
                  // set Task Description
                  void setTaskDescription(const QString& s);
                  
                  /// get TaskBaseline
                  QString getTaskBaseline() const { return taskBaseline; }
                  
                  // set TaskBaseline
                  void setTaskBaseline(const QString& s);
                  
                  /// get test attributes
                  QString getTestAttributes() const { return testAttributes; }
                  
                  // set TestAttributes
                  void setTestAttributes(const QString& s);
                  
                  // called to read from an XML structure
                  void readXML(QDomNode& node) throw (FileException);
                  
                  // called to write to an XML structure
                  void writeXML(QDomDocument& xmlDoc,
                                QDomElement&  parentElement) const throw (FileException);
               
                  // write the data into a comma separated value file
                  static void writeDataIntoCommaSeparatedValueFile(const std::vector<Panel*>& panels,
                                                       CommaSeparatedValueFile& csvf)  throw (FileException);
                  
                  // read the data from a StringTable
                  static void readDataFromStringTable(std::vector<Panel*>& panels,
                                                      const StringTable& st) throw (FileException);
                  
                  /// set parent
                  void setParent(Figure* parentFigureIn);
                  
               protected:
                  // copy helper used by copy constructor and assignment operator
                  void copyHelper(const Panel& p);
                  
                  // set modified status
                  void setModified();
                  
                  /// figure that is parent of instance (DO NOT COPY)
                  Figure* parentFigure;
                  
                  /// description
                  QString description;
                  
                  /// the panel number/letter
                  QString panelNumberOrLetter;
                  
                  // task description
                  QString taskDescription;
                  
                  // task baseline
                  QString taskBaseline;
                  
                  // test attributes
                  QString testAttributes;
               
                  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                  // If additional members are added be sure to update copyHelper() method.
                  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            };  // end of panel class
            
            // constructor
            Figure();
            
            // destructor
            ~Figure();
            
            // copy constructor
            Figure(const Figure& f);
            
            // assignment operator
            Figure& operator=(const Figure& f);
                  
            // clear the figure information
            void clear();
            
            // add a panel
            void addPanel(Panel* p);
            
            // delete a panel
            void deletePanel(const int indx);
            
            // delete a panel
            void deletePanel(const Panel* panel);
            
            /// get the number of panels
            int getNumberOfPanels() const { return panels.size(); }
            
            /// get a panel
            Panel* getPanel(const int indx) { return panels[indx]; }
            
            /// get a panel (const method)
            const Panel* getPanel(const int indx) const { return panels[indx]; }
            
            /// get a panel by its panel number/letter
            Panel* getPanelByPanelNumberOrLetter(const QString& panelNumberOrLetter);
            
            /// get a panel by its panel number/letter (const method)
            const Panel* getPanelByPanelNumberOrLetter(const QString& panelNumberOrLetter) const;
            
            /// get the legend
            QString getLegend() const { return legend; }
            
            // set the legend
            void setLegend(const QString& s);
            
            /// get the figure number
            QString getNumber() const { return number; }
            
            // set the figure number
            void setNumber(const QString& n);
            
            // called to read from an XML structure
            void readXML(QDomNode& nodeIn) throw (FileException);
            
            // called to write to an XML structure
            void writeXML(QDomDocument& xmlDoc,
                          QDomElement&  parentElement) const throw (FileException);
         
            // write the data into a comma separated value file
            static void writeDataIntoCommaSeparatedValueFile(const std::vector<Figure*>& figures,
                                                 CommaSeparatedValueFile& csvf)  throw (FileException);
            
            // read the data from a StringTable
            static void readDataFromStringTable(std::vector<Figure*>& figures,
                                                const StringTable& st) throw (FileException);
            
            /// set parent
            void setParent(StudyMetaData* parentStudyMetaDataIn);
            
         protected:
            // copy helper used by copy constructor and assignment operator
            void copyHelper(const Figure& f);
            
            // set modified status
            void setModified();
            
            /// study metadata that is parent of this figure (DO NOT COPY)
            StudyMetaData* parentStudyMetaData;
            
            /// legend
            QString legend;
            
            /// figure number
            QString number;
            
            /// the panels
            std::vector<Panel*> panels;
      
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // If additional members are added be sure to update copyHelper() method.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      };  // end of Figure class
      
      class Table;
      class PageReference;
      
      // Table Subheader
      class SubHeader {
         public:
            // constructor
            SubHeader();
            
            // destructor
            ~SubHeader();
            
            // copy constructor
            SubHeader(const SubHeader& sh);
            
            // assignment operator
            SubHeader& operator=(const SubHeader& sh);
            
            // clear the sub header
            void clear();
            
            /// get name
            QString getName() const { return name; }
            
            // set name
            void setName(const QString& s);
            
            /// get number
            QString getNumber() const { return number; }
            
            // set number
            void setNumber(const QString& n);
            
            /// get short name
            QString getShortName() const { return shortName; }
            
            // set short name
            void setShortName(const QString& s);
            
            /// get task description
            QString getTaskDescription() const { return taskDescription; }
            
            // set Task Description
            void setTaskDescription(const QString& s);
            
            /// get TaskBaseline
            QString getTaskBaseline() const { return taskBaseline; }
            
            // set TaskBaseline
            void setTaskBaseline(const QString& s);
            
            /// get test attributes
            QString getTestAttributes() const { return testAttributes; }
            
            // set TestAttributes
            void setTestAttributes(const QString& s);
            
            // called to read from an XML structure
            void readXML(QDomNode& node) throw (FileException);
            
            // called to write to an XML structure
            void writeXML(QDomDocument& xmlDoc,
                          QDomElement&  parentElement) const throw (FileException);
         
            // write the data into a comma separated value file
            static void writeDataIntoCommaSeparatedValueFile(const std::vector<SubHeader*>& subHeaders,
                                                 CommaSeparatedValueFile& csvf)  throw (FileException);
            
            // read the data from a StringTable
            static void readDataFromStringTable(std::vector<SubHeader*>& subHeaders,
                                                const StringTable& st) throw (FileException);
            
            // set parent
            void setParent(Table* parentTableIn);
            
            // set parent
            void setParent(PageReference* parentPageReferenceIn);
            
            /// get the selected flag
            bool getSelected() const { return selectedFlag; }
            
            /// set the selected flag (does not modify this object)
            void setSelected(const bool s) { selectedFlag = s; }
            
         protected:
            // copy helper used by copy constructor and assignment operator
            void copyHelper(const SubHeader& sh);
            
            // set instance modified
            void setModified();
            
            /// parent of this instance (DO NOT COPY)
            Table* parentTable;
            
            /// parent of this instance (DO NOT COPY)
            PageReference* parentPageReference;
            
            /// name
            QString name;
            
            /// subheader number
            QString number;
            
            /// short name
            QString shortName;
            
            /// task description
            QString taskDescription;
            
            /// task baseline
            QString taskBaseline;
            
            /// test attributes
            QString testAttributes;

            /// selected flag
            bool selectedFlag;
            
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // If additional members are added be sure to update copyHelper() method.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      }; // end of class SubHeader
            
      /// Table Class
      class Table { 
         public:
            // constructor
            Table();
            
            // destructor
            ~Table();
            
            // copy constructor
            Table(const Table& t);
            
            // assignment operator
            Table& operator=(const Table& t);
                  
            // clear the table
            void clear();

            // add a sub header
            void addSubHeader(SubHeader* sh);
            
            // delete a sub header
            void deleteSubHeader(const int indx);
            
            // delete a sub header
            void deleteSubHeader(const SubHeader* subHeader);
            
            /// get the number of sub headers
            int getNumberOfSubHeaders() const { return subHeaders.size(); }
            
            /// get a sub header
            SubHeader* getSubHeader(const int indx) { return subHeaders[indx]; }
            
            /// get a sub header
            const SubHeader* getSubHeader(const int indx) const { return subHeaders[indx]; }
            
            // get a sub header by sub header number
            SubHeader* getSubHeaderBySubHeaderNumber(const QString& subHeaderNumber);
            
            // get a sub header by sub header number (const method)
            const SubHeader* getSubHeaderBySubHeaderNumber(const QString& subHeaderNumber) const;
            
            /// get footer
            QString getFooter() const { return footer; }
            
            // set footer
            void setFooter(const QString& s);
            
            /// get header
            QString getHeader() const { return header; }
            
            // set header
            void setHeader(const QString& s);
            
            /// get number
            QString getNumber() const { return number; }
            
            // set number
            void setNumber(const QString& n);
            
            /// get size units
            QString getSizeUnits() const { return sizeUnits; }
            
            // set size units
            void setSizeUnits(const QString& s);
            
            /// get statistic type
            QString getStatisticType() const { return statisticType; }
            
            // set tatistic type
            void setStatisticType(const QString& s);
            
            /// get statistic description
            QString getStatisticDescription() const { return statisticDescription; }
            
            // set statistic description
            void setStatisticDescription(const QString& s);
            
            /// get voxel dimensions
            QString getVoxelDimensions() const { return voxelDimensions; }
            
            // set voxelDimensions
            void setVoxelDimensions(const QString& s);
            
            // called to read from an XML structure
            void readXML(QDomNode& node) throw (FileException);
            
            // called to write to an XML structure
            void writeXML(QDomDocument& xmlDoc,
                          QDomElement&  parentElement) const throw (FileException);
         
            // write the data into a comma separated value file
            static void writeDataIntoCommaSeparatedValueFile(const std::vector<Table*>& t,
                                                 CommaSeparatedValueFile& csvf)  throw (FileException);
            
            // read the data from a StringTable
            static void readDataFromStringTable(std::vector<Table*>& t,
                                                const StringTable& st) throw (FileException);
            
            /// set parent
            void setParent(StudyMetaData* parentStudyMetaDataIn);
            
            // set modified
            void setModified();
            
         protected:
            // copy helper used by copy constructor and assignment operator
            void copyHelper(const Table& t);
            
            /// study metadata that is parent of this table (DO NOT COPY)
            StudyMetaData* parentStudyMetaData;
            
            /// the table footer
            QString footer;

            /// the table header
            QString header;
            
            /// the table number in the paper
            QString number;
            
            /// size units
            QString sizeUnits;
            
            /// statistic type
            QString statisticType;
            
            /// statistic description
            QString statisticDescription;
            
            /// voxel dimensions
            QString voxelDimensions;
            
            /// the sub headers
            std::vector<SubHeader*> subHeaders;
      
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // If additional members are added be sure to update copyHelper() method.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      };  // end of class Table
      
      /// page reference
      class PageReference {
         public:
            // constructor
            PageReference();
            
            // destructor
            ~PageReference();
            
            // copy constructor
            PageReference(const PageReference& pr);
            
            // assignment operator
            PageReference& operator=(const PageReference& pr);
            
            // clear the page link
            void clear();
            
            // add a sub header
            void addSubHeader(SubHeader* sh);
            
            // delete a sub header
            void deleteSubHeader(const int indx);
            
            // delete a sub header
            void deleteSubHeader(const SubHeader* subHeader);
            
            /// get the number of sub headers
            int getNumberOfSubHeaders() const { return subHeaders.size(); }
            
            /// get a sub header
            SubHeader* getSubHeader(const int indx) { return subHeaders[indx]; }
            
            /// get a sub header
            const SubHeader* getSubHeader(const int indx) const { return subHeaders[indx]; }
            
            // get a sub header by sub header number
            SubHeader* getSubHeaderBySubHeaderNumber(const QString& subHeaderNumber);
            
            // get a sub header by sub header number (const method)
            const SubHeader* getSubHeaderBySubHeaderNumber(const QString& subHeaderNumber) const;
            
            /// get the page number
            QString getPageNumber() const { return pageNumber; }
            
            // set the 
            void setPageNumber(const QString& pn);
            
            /// get the header
            QString getHeader() const { return header; }

            // set the header
            void setHeader(const QString& s);
                        
            /// get the comment
            QString getComment() const { return comment; }
            
            // set the comment
            void setComment(const QString& s);
            
            /// get the size units
            QString getSizeUnits() const { return sizeUnits; }
            
            // set the size units
            void setSizeUnits(const QString& s);
            
            /// get the voxel dimensions
            QString getVoxelDimensions() const { return voxelDimensions; }
            
            // set the voxel dimensions
            void setVoxelDimensions(const QString& s);
            
            /// get the statistic type
            QString getStatisticType() const { return statisticType; }
            
            // set the statistic type
            void setStatisticType(const QString& s);
            
            /// get the statistic description
            QString getStatisticDescription() const { return statisticDescription; }
            
            // set the statistic description
            void setStatisticDescription(const QString& s);
            
            // called to read from an XML structure
            void readXML(QDomNode& node) throw (FileException);
            
            // called to write to an XML structure
            void writeXML(QDomDocument& xmlDoc,
                          QDomElement&  parentElement) const throw (FileException);
         
            // set parent
            void setParent(StudyMetaData* parentStudyMetaDataIn);
            
            // set modified
            void setModified();
            
         protected:
            // copy helper
            void copyHelper(const PageReference& pr);
            
            /// study metadata that is parent of this table (DO NOT COPY)
            StudyMetaData* parentStudyMetaData;
      
            /// the page number
            QString pageNumber;
            
            /// the header
            QString header;
            
            /// the comment
            QString comment;
            
            /// the size units
            QString sizeUnits;
            
            /// the voxel dimensions
            QString voxelDimensions;
            
            /// the statistic type
            QString statisticType;
            
            /// the statistic description
            QString statisticDescription;
            
            /// the sub headers
            std::vector<SubHeader*> subHeaders;
      
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // If additional members are added be sure to update copyHelper() method.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      };  // end of class PageReference
      
      /// provenance
      class Provenance {
         public:
            // constructor
            Provenance();
            
            // destructor
            ~Provenance();
            
            // copy constructor
            Provenance(const Provenance& p);
            
            // assignment operator
            Provenance& operator=(const Provenance& p);
            
            // clear the page link
            void clear();
            
            /// get the name
            QString getName() const { return name; }
            
            // set the name
            void setName(const QString& s);
            
            /// get the date
            QString getDate() const { return date; }
            
            // set the date
            void setDate(const QString& s);
            
            /// get the command
            QString getComment() const { return comment; }
            
            // set the comment
            void setComment(const QString& s);
            
            // called to read from XML 
            void readXML(QDomNode& node) throw (FileException);
            
            // called to write to an XML structure
            void writeXML(QDomDocument& xmlDoc,
                          QDomElement&  parentElement) const throw (FileException);
         
            // set parent
            void setParent(StudyMetaData* parentStudyMetaDataIn);
            
            // set modified
            void setModified();
            
         protected:
            // copy helper
            void copyHelper(const Provenance& p);
            
            /// study metadata that is parent of this table (DO NOT COPY)
            StudyMetaData* parentStudyMetaData;
      
            /// the name
            QString name;
            
            /// the date
            QString date;
            
            /// the comment
            QString comment;
            
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // If additional members are added be sure to update copyHelper() method.
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      };  // end of class Provenance
      
      // constructor
      StudyMetaData();

      // constructor (from old CellStudyInfo)
      StudyMetaData(const CellStudyInfo& csi);

      // constructor
      StudyMetaData(const StudyMetaAnalysisFile* smaf);
      
      // destructor
      ~StudyMetaData();

      // copy constructor
      StudyMetaData(const StudyMetaData& smd);
      
      // assignment operator
      StudyMetaData& operator=(const StudyMetaData& smd);
                  
      // clear the meta data
      void clear();
      
      // equality operator
      bool operator==(const StudyMetaData& cci) const; 
      
      // called to read from an XML structure
      void readXML(QDomNode& node) throw (FileException);
      
      // called to write to an XML structure
      void writeXML(QDomDocument& xmlDoc,
                            QDomElement&  parentElement) const throw (FileException);
   
      // write the data into a comma separated value file
      static void writeDataIntoCommaSeparatedValueFile(const std::vector<StudyMetaData*>& smd,
                                     CommaSeparatedValueFile& csvf) throw (FileException);
      
      // read the data from a StringTable
      static void readDataFromStringTable(std::vector<StudyMetaData*>& smd,
                                    const StringTable& st) throw (FileException);
      
      // get the study data format entries
      static void getStudyDataFormatEntries(std::vector<QString>& entries);
      
      // get the study data type entries
      static void getStudyDataTypeEntries(std::vector<QString>& entries);
      
      // retrieve data from PubMed using PubMed ID
      void updateDataFromPubMedDotComUsingPubMedID() throw (FileException);
      
      // add a figure
      void addFigure(Figure* f);
      
      // delete a figure
      void deleteFigure(const int indx);
      
      // delete a figure
      void deleteFigure(const Figure* figure);
      
      /// get the number of figures
      int getNumberOfFigures() const { return figures.size(); }
      
      /// get a figure 
      Figure* getFigure(const int indx) { return figures[indx]; }
      
      /// get a figure (const method)
      const Figure* getFigure(const int indx) const { return figures[indx]; }
      
      /// get a figure by its figure number
      Figure* getFigureByFigureNumber(const QString& figureNumber);
      
      /// get a figure by its figure number (const method)
      const Figure* getFigureByFigureNumber(const QString& figureNumber) const;
      
      // add a table
      void addTable(Table* t);
      
      // delete a table
      void deleteTable(const int indx);
      
      // delete a table
      void deleteTable(const Table* table);
      
      /// get number of tables
      int getNumberOfTables() const { return tables.size(); }
      
      /// get a table
      Table* getTable(const int indx) { return tables[indx]; }
      
      /// get a table (const method)
      const Table* getTable(const int indx) const { return tables[indx]; }
      
      /// get a table by its table number
      Table* getTableByTableNumber(const QString& tableNumber);
      
      /// get a table by its table number (const method)
      const Table* getTableByTableNumber(const QString& tableNumber) const;
      
      // add a provenance
      void addProvenance(Provenance* p);
      
      // delete a provenance
      void deleteProvenance(const int indx);
      
      // delete a provenance
      void deleteProvenance(const Provenance* p);
      
      /// get number of provenances
      int getNumberOfProvenances() const { return provenances.size(); }
      
      /// get a provenance
      Provenance* getProvenance(const int indx) { return provenances[indx]; }
      
      /// get a provenance (const method)
      const Provenance* getProvenance(const int indx) const { return provenances[indx]; }
      
      // add a page reference
      void addPageReference(PageReference* pr);
      
      // delete a page reference
      void deletePageReference(const int indx);
      
      // delete a page reference
      void deletePageReference(const PageReference* pr);
      
      // get number of page references
      int getNumberOfPageReferences() const { return pageReferences.size(); }
      
      // get a page reference
      PageReference* getPageReference(const int indx) { return pageReferences[indx]; }
      
      // get a page reference (const method)
      const PageReference* getPageReference(const int indx) const { return pageReferences[indx]; }
      
      // get a page reference by its page number
      PageReference* getPageReferenceByPageNumber(const QString& pageNumber);
      
      // get a page reference by its page number (const method)
      const PageReference* getPageReferenceByPageNumber(const QString& pageNumber) const;
      
      // get a pointer to the meta-analysis studies
      StudyNamePubMedID* getMetaAnalysisStudies() { return &metaAnalysisStudies; }
      
      // get a pointer to the meta-analysis studies (const method)
      const StudyNamePubMedID* getMetaAnalysisStudies() const { return &metaAnalysisStudies; }
      
      /// get authors
      QString getAuthors() const { return authors; }
      
      // set authors
      void setAuthors(const QString& s);
      
      /// get citation
      QString getCitation() const { return citation; }
      
      /// set citation
      void setCitation(const QString& s);
      
      /// get comment
      QString getComment() const { return comment; }
      
      // set comment
      void setComment(const QString& s);
      
      /// get the document object identifier
      QString getDocumentObjectIdentifier() const { return documentObjectIdentifier; }
      
      // set the document object identifier
      void setDocumentObjectIdentifier(const QString& doi);
      
      /// get keywords
      QString getKeywords() const { return keywords; }
      
      // get the keywords
      void getKeywords(std::vector<QString>& keywordsOut) const;
      
      // get all table sub header short names in this study
      void getAllTableSubHeaderShortNames(std::vector<QString>& tableSubHeaderShortNamesOut) const;
      
      // set keywords
      void setKeywords(const QString& s);
      
      /// get medical subject headings
      QString getMedicalSubjectHeadings() const { return medicalSubjectHeadings; }
      
      /// set medical subject headings
      void setMedicalSubjectHeadings(const QString& s);
      
      /// get name
      QString getName() const { return name; }
      
      /// set name
      void setName(const QString& s);
      
      /// get partitioning scheme abbreviation
      QString getPartitioningSchemeAbbreviation() const { return partitioningSchemeAbbreviation; }
      
      // set partitioning scheme abbreviation
      void setPartitioningSchemeAbbreviation(const QString& s);
      
      /// get partitioning scheme full name
      QString getPartitioningSchemeFullName() const { return partitioningSchemeFullName; }
      
      // set partitioning scheme full name
      void setPartitioningSchemeFullName(const QString& s);
      
      /// get the PubMed ID (if negative it is project ID)
      QString getPubMedID() const { return pubMedID; }
      
      // set the PubMed ID
      void setPubMedID(const QString& pmid);
      
      // see if the PubMed ID is actually a Project ID
      bool getPubMedIDIsAProjectID() const;
      
      /// get stereotaxic space
      QString getStereotaxicSpace() const { return stereotaxicSpace; }
      
      // set stereotaxic space
      void setStereotaxicSpace(const QString& s);
      
      /// get stereotaxic space details
      QString getStereotaxicSpaceDetails() const { return stereotaxicSpaceDetails; }
      
      // set stereotaxic space details
      void setStereotaxicSpaceDetails(const QString& s);
      
      /// get the project ID
      QString getProjectID() const { return projectID; }
      
      // set the project ID
      void setProjectID(const QString& num);
      
      /// get the Project ID prefix when used as a
      static QString getProjectIDInPubMedIDPrefix() { return "ProjID"; }
      
      /// get the most recent save date
      QString getMostRecentDateAndTimeStamp() const;
      
      /// get the data and time stamps
      QString getDateAndTimeStamps() const { return dateAndTimeStamps; }
      
      /// get the quality
      QString getQuality() const { return quality; }
      
      /// set the quality
      void setQuality(const QString& s);
      
      /// get the study data format
      QString getStudyDataFormat() const { return studyDataFormat; }
      
      /// set the study data format
      void setStudyDataFormat(const QString& s);
      
      /// get the study data type
      QString getStudyDataType() const { return studyDataType; }
      
      /// set the study data type
      void setStudyDataType(const QString& s);
      
      /// get title
      QString getTitle() const { return title; }
      
      // set title
      void setTitle(const QString& s);
      
      // get parent
      StudyMetaDataFile* getParent() { return parentStudyMetaDataFile; }
      
      // get parent (const method)
      const StudyMetaDataFile* getParent() const { return parentStudyMetaDataFile; }
      
      // set parent
      void setParent(StudyMetaDataFile* parentStudyMetaDataFile);
      
      // set this study info is modified
      void setModified();
      
      // clear this study info is modified
      void clearModified();
      
      /// get meta-analysis flag
      bool getMetaAnalysisFlag() const { return metaAnalysisFlag; }
      
      /// set meta-analysis flag
      void setMetaAnalysisFlag(const bool b);

   protected:
      // copy helper used by copy constructor and assignment operator
      void copyHelper(const StudyMetaData& smd);
      
      // set the date and time stamps
      void setDateAndTimeStamps(const QString& p);
      
      /// study metadata file that is a parent of this instance (do not copy)
      StudyMetaDataFile* parentStudyMetaDataFile;
      
      /// study authors
      QString authors;
      
      /// study citation (journal)
      QString citation;
      
      /// comment
      QString comment;
      
      /// the document object identifier
      QString documentObjectIdentifier;

      /// study key words
      QString keywords;
      
      /// medical subject headings
      QString medicalSubjectHeadings;
      
      /// name
      QString name;
      
      /// partitioning scheme abbreviation
      QString partitioningSchemeAbbreviation;
      
      /// partitioning scheme full name
      QString partitioningSchemeFullName;
      
      /// the project ID
      QString projectID;
      
      /// the PubMed ID
      QString pubMedID;
      
      /// quality of data
      QString quality;
      
      /// study data format
      QString studyDataFormat;
      
      /// study data type
      QString studyDataType;
      
      /// stereotaxic space
      QString stereotaxicSpace;
      
      /// stereotaxic space details
      QString stereotaxicSpaceDetails;
      
      /// study title
      QString title;
      
      /// tables in the study
      std::vector<Table*> tables;
      
      /// figures in the study
      std::vector<Figure*> figures;
      
      /// provenances
      std::vector<Provenance*> provenances;
      
      /// page references
      std::vector<PageReference*> pageReferences;
      
      /// the meta-analysis studies
      StudyNamePubMedID metaAnalysisStudies;
      
      /// meta-analysis flag
      bool metaAnalysisFlag;
      
      /// the date and time stamps  (DO NOT COPY)
      mutable QString dateAndTimeStamps;
      
      /// the modified flag, cleared when file is written (DO NOT COPY)
      mutable bool studyDataModifiedFlag;
      
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // If additional members are added be sure to update copyHelper() method.
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      
};

//========================================================================================
//
/// the Study Meta Data File
class StudyMetaDataFile : public AbstractFile {
   public:
      // constructor
      StudyMetaDataFile();
      
      // destructor
      ~StudyMetaDataFile();
      
      // copy constructor
      StudyMetaDataFile(const StudyMetaDataFile& smdf);
      
      // assignment operator
      StudyMetaDataFile& operator=(const StudyMetaDataFile& smdf);
                  
      // append a study metadata file to "this" study metadata file
      void append(const StudyMetaDataFile& smdf);
       
      // append metadata from a cell file to this file
      void append(CellFile& cf);
      
      // append metadata from a cell projection file to this file
      void append(CellProjectionFile& cpf);
      
      // append metadata from a vocabulary file to this file
      void append(VocabularyFile& vf);
      
      // Clears current file data in memory.  Deriving classes must override this method and
      // call AbstractFile::clearAbstractFile() from its clear method.
      virtual void clear();
         
      // returns true if the file is isEmpty (contains no data)
      virtual bool empty() const;

      // add study meta data
      void addStudyMetaData(StudyMetaData* smd);
      
      // delete study meta data
      void deleteStudyMetaData(const int indx);
      
      // delete studies with the given names
      void deleteStudiesWithNames(const std::vector<QString>& namesOfStudiesToDelete);
      
      // get the index of a study metadata item
      int getStudyMetaDataIndex(const StudyMetaData* smdToFind) const;
      
      // get the index of study meta data matching the study metadata link (-1 if no match found)
      int getStudyIndexFromLink(const StudyMetaDataLink& smdl) const;
                   
      // get study from a PubMed ID (may match PubMed ID or Project ID)
      int getStudyIndexFromPubMedID(const QString& pubMedID) const;
      
      // get study from its name
      int getStudyIndexFromName(const QString& name) const;
      
      /// get study meta data
      StudyMetaData* getStudyMetaData(const int indx) { return studyMetaData[indx]; }
      
      /// get study meta data (const method)
      const StudyMetaData* getStudyMetaData(const int indx) const { return studyMetaData[indx]; }
      
      /// get the number of study metadata
      int getNumberOfStudyMetaData() const { return studyMetaData.size(); }
      
      // given a study, report any meta-analysis studies of which it is a member
      void getMetaAnalysisStudiesForStudy(const StudyMetaData* mySMD,
                                          std::vector<QString>& metaAnalysisStudyPMIDsOut) const;
                                          
      // find out if comma separated file conversion supported
      virtual void getCommaSeparatedFileSupport(bool& readFromCSV,
                                                bool& writeToCSV) const;
                                    
      // write the file's data into a comma separated values file (throws exception if not supported)              
      virtual void writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException);
      
      // read the file's data from a comma separated values file (throws exception if not supported)               
      virtual void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException); 

      // clear study meta data modified (prevents date and time stamp updates)
      void clearAllStudyMetaDataElementsModified();
      
      // get all keywords
      void getAllKeywords(std::vector<QString>& allKeywords) const;
      
      // get all table subheader short names
      void getAllTableSubHeaderShortNames(std::vector<QString>& allShortNames) const;
      
      // retrieve data from PubMed using PubMed ID for all studies
      void updateAllStudiesWithDataFromPubMedDotCom() throw (FileException);
      
      // check for any studies matching those in meta-analysis file
      void checkForMatchingStudies(const StudyMetaAnalysisFile* smaf,
                                   std::vector<int>& matchingStudyMetaDataFileStudyNumbers,
                                   std::vector<int>& matchingMetaAnalysisFileStudyIndices) const;
                                   
      // add meta-analysis study
      void addMetaAnalysisStudy(const StudyMetaAnalysisFile* smaf,
                                const bool createMetaAnalysisStudiesFlag,
                                const bool fetchDataFromPubMedFlag) throw (FileException);
      
      // create new studies from these meta-analysis studies
      void createStudiesFromMetaAnalysisStudiesWithPubMedDotCom(const StudyNamePubMedID* ms,
                                                              const bool fetchDataFromPubMedFlag) throw (FileException);
      
      // count the number of studies without at least one provenance entry
      int getNumberOfStudyMetaDatWithoutProvenceEntries() const;
      
      // update all studies without a provenance entry
      void addProvenanceToStudiesWithoutProvenanceEntries(const QString& name,
                                                          const QString& date,
                                                          const QString& comment);
                                                          
      // find duplicate studies (map key is PubMedID, value is filename)
      static void findDuplicateStudies(const std::vector<QString>& studyFileNames,
                                       std::multimap<QString,QString>& duplicatesStudiesOut)
                                                                throw (FileException);
   protected:
      // copy helper used by copy constructor and assignment operator
      void copyHelper(const StudyMetaDataFile& smdf);
      
      // Read the contents of the file (header has already been read)
      virtual void readFileData(QFile& file,
                                QTextStream& stream,
                                QDataStream& binStream,
                                QDomElement& rootElement) throw (FileException) ;

      // Write the file's data (header has already been written)
      virtual void writeFileData(QTextStream& stream,
                                 QDataStream& binStream,
                                 QDomDocument& xmlDoc,
                                 QDomElement& rootElement) throw (FileException) ;
                                 
      /// the study metadata
      std::vector<StudyMetaData*> studyMetaData;
            
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // If additional members are added be sure to update copyHelper() method.
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
};

#endif // __STUDY_META_DATA_FILE_H__


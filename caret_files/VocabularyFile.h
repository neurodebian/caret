
#ifndef __VOCABULARY_FILE_H__
#define __VOCABULARY_FILE_H__

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

#include "AbstractFile.h"
#include "CellStudyInfo.h"
#include "StudyMetaDataLinkSet.h"

class QDomNode;
class XmlGenericWriter;

/// class for storing vocabulary information
class VocabularyFile : public AbstractFile {
   public:
      /// stores vocabulary data entry
      class VocabularyEntry {
         public:
            // constructor
            VocabularyEntry();
              
            // constructor
            VocabularyEntry(const QString& abbreviationIn,
                            const QString& fullNameIn = "",
                            const QString& classNameIn = "",
                            const QString& vocabularyIDIn = "",
                            const QString& descriptionIn = "",
                            const QString& ontologySourceIn = "",
                            const QString& termIDIn = "",
                            const int studyNumberIn = -1);
              
            // copy constructor
            VocabularyEntry(const VocabularyEntry& ve);
              
            // assignment operator
            VocabularyEntry& operator=(const VocabularyEntry& ve);
            
            // destructor
            ~VocabularyEntry();
            
            // get full description of all fields for display to user
            //QString getFullDescriptionForDisplayToUser(const bool useHTML) const;
            
            /// get the abbreviation
            QString getAbbreviation() const { return abbreviation; }
            
            // set the abbreviation
            void setAbbreviation(const QString& a);
            
            /// get the full name
            QString getFullName() const { return fullName; }
            
            // set the full name
            void setFullName(const QString& n);
            
            /// get the description
            QString getDescription() const { return description; }
            
            // set the description
            void setDescription(const QString& d);
            
            /// get the class name
            QString getClassName() const { return className; }
            
            // set the class name
            void setClassName(const QString& s);
            
            /// get the ontology source
            QString getOntologySource() const { return ontologySource; }
            
            /// set the ontology source
            void setOntologySource(const QString& s);
            
            // get ontology source values
            static void getOntologySourceValues(std::vector<QString>& ontologySourceValues);
            
            /// get the term id
            QString getTermID() const { return termID; }
            
            /// set the term id
            void setTermID(const QString& s);
            
            /// get the vocabulary ID
            QString getVocabularyID() const { return vocabularyID; }
            
            // set the vocabulary ID
            void setVocabularyID(const QString& s);
            
            /// get the study number
            int getStudyNumber() const { return studyNumber; }
            
            // set the study number
            void setStudyNumber(const int sn);
                       
            /// get the study metadata link set
            StudyMetaDataLinkSet getStudyMetaDataLinkSet() const { return studyMetaDataLinkSet; }
            
            /// set the study metadata link set
            void setStudyMetaDataLinkSet(const StudyMetaDataLinkSet smdls);
            
            /// write the data into a StringTable
            static void writeDataIntoStringTable(const std::vector<VocabularyEntry>& data,
                                                 StringTable& table);
            
            /// read the data from a StringTable
            static void readDataFromStringTable(std::vector<VocabularyEntry>& data,
                                                const StringTable& table) throw (FileException);
            
         protected:
            // copy helper
            void copyHelper(const VocabularyEntry& ve);
            
            // clear this item
            void clear();
            
            // set this item modified
            void setModified();
            
            // called to read from an XML structure.
            void readXML(QDomNode& nodeIn) throw (FileException);

            // called to write to an XML structure.
            void writeXML(QDomDocument& xmlDoc,
                          QDomElement&  parentElement) const;
                          
            // called to write XML
            void writeXML(XmlGenericWriter& xmlWriter) const throw (FileException);

            /// VocabularyFile entry with which this is association
            VocabularyFile* vocabularyFile;
            
            /// the abbreviated name
            QString abbreviation;
            
            /// the full name
            QString fullName;
            
            /// the class name
            QString className;
            
            /// ontology source
            QString ontologySource;
            
            /// term id
            QString termID;
            
            /// the vocabulary ID
            QString vocabularyID;
            
            /// the description
            QString description;
         
            /// the study number
            int studyNumber;
            
            /// the study metadata link
            StudyMetaDataLinkSet studyMetaDataLinkSet;
            
            // ***** IF ADDITIONAL MEMBERS ADDED, UPDATE copyHelper() **********
            
         friend class VocabularyFile;
      };
      
      
      // constructor
      VocabularyFile();
      
      // destructor
      ~VocabularyFile();
      
      // add a vocabulary entry (returns its index)
      int addVocabularyEntry(const VocabularyEntry& ve);
      
      // delete a vocabulary entry
      void deleteVocabularyEntry(const int indx);
      
      // append a vocabulary file to "this" vocabulary file
      void append(const VocabularyFile& vf);
      
      // Clears current file data in memory.  Deriving classes must override this method and
      // call AbstractFile::clearAbstractFile() from its clear method.
      void clear();
      
      // returns true if the file is isEmpty (contains no data)
      bool empty() const;

      /// get the number of vocabulary entries
      int getNumberOfVocabularyEntries() const { return vocabularyEntries.size(); }
      
      // a vocabulary entry using its index
      VocabularyEntry* getVocabularyEntry(const int indx);
      
      // a vocabulary entry using its index (const method)
      const VocabularyEntry* getVocabularyEntry(const int indx) const;
      
      // get the index of a vocabulary entry from its abbreviation (-1 if not found)
      int getVocabularyEntryIndexFromName(const QString& abbreviationIn) const;
      
      // get a vocabulary entry from its abbreviation (NULL if not found)
      VocabularyEntry* getVocabularyEntryByName(const QString& abbreviationIn);
      
      // get a vocabulary entry from its abbreviation (const method)
      const VocabularyEntry* getVocabularyEntryByName(const QString& abbreviationIn) const;
      
      // get the best matching vocabulary entry (abbreviationIn begins with entry name)
      VocabularyEntry* getBestMatchingVocabularyEntry(const QString abbreviationIn,
                                                      const bool caseSensitive = true);
      
      // get the best matching vocabulary entry (abbreviationIn begins with entry name)
      const VocabularyEntry* getBestMatchingVocabularyEntry(const QString abbreviationIn,
                                                            const bool caseSensitive = true) const;
      
      /// find out if comma separated file conversion supported
      void getCommaSeparatedFileSupport(bool& readFromCSV,
                                        bool& writeToCSV) const;
                                        
      /// write the file's data into a comma separated values file (throws exception if not supported)
      void writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException);
      
      /// read the file's data from a comma separated values file (throws exception if not supported)
      void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException);

      /// get a pointer to the study info
      std::vector<CellStudyInfo>* getPointerToStudyInfo() { return &studyInfo; }
      
      /// get number of study info
      int getNumberOfStudyInfo() const { return studyInfo.size(); }
      
      /// get the study info index from the study info's value
      int getStudyInfoFromValue(const CellStudyInfo& studyInfo) const;
      
      /// get a study info (const method)
      const CellStudyInfo* getStudyInfo(const int index) const;
      
      /// get a study info
      CellStudyInfo* getStudyInfo(const int index);
      
      /// add a study info
      int addStudyInfo(const CellStudyInfo& studyInfo);
      
      /// delete all study info
      void deleteAllStudyInfo();
      
      /// delete study info
      void deleteStudyInfo(const int indx);
      
      /// set a study info
      void setStudyInfo(const int index, const CellStudyInfo& csi);
            
      /// get PubMedID's of all linked studies
      void getPubMedIDsOfAllLinkedStudyMetaData(std::vector<QString>& studyPMIDs) const;
      
      /// write the file's memory in caret6 format to the specified name
      virtual QString writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException);

   protected:
      // Read the contents of the file (header has already been read)
      void readFileData(QFile& file,
                                QTextStream& stream,
                                QDataStream& binStream,
                                QDomElement& rootElement) throw (FileException);

      // Write the file's data (header has already been written)
      void writeFileData(QTextStream& stream,
                                 QDataStream& binStream,
                                 QDomDocument& xmlDoc,
                                 QDomElement& rootElement) throw (FileException);

      /// the vocabulary entries
      std::vector<VocabularyEntry> vocabularyEntries;
      
      /// the study info
      std::vector<CellStudyInfo> studyInfo;
};

#endif // __VOCABULARY_FILE_H__

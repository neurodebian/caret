
#ifndef __STUDY_COLLECTION_FILE_H__
#define __STUDY_COLLECTION_FILE_H__

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
#include "StudyNamePubMedID.h"

class StudyCollectionFile;

//
// Class for storing a study collection
//
class StudyCollection {
   public:
      // constructor
      StudyCollection();
      
      // copy constructor
      StudyCollection(const StudyCollection& sc);
      
      // destructor
      ~StudyCollection();
      
      // assignment operator
      StudyCollection& operator=(const StudyCollection& sc);
      
      /// clear the collection
      void clear();
      
      /// get number of PMIDs
      int getNumberOfStudyPMIDs() const { return studyPMIDs.size(); }
      
      // add a study PMID
      void addStudyPMID(StudyNamePubMedID* s);
      
      // get a study PMID
      StudyNamePubMedID* getStudyPMID(const int indx);
      
      // get a study PMID
      const StudyNamePubMedID* getStudyPMID(const int indx) const;

      // remove a study PMID
      void removeStudyPMID(const int indx);

      /// get study collection name
      QString getStudyCollectionName() const { return studyCollectionName; }
            
      // set study collection name 
      void setStudyCollectionName(const QString& s);
      
      /// get study collection creator
      QString getStudyCollectionCreator() const { return studyCollectionCreator; }
      
      // set study collection creator
      void setStudyCollectionCreator(const QString& s);
      
      // get study type
      QString getStudyType() const { return studyType; }
      
      // set study type
      void setStudyType(const QString& s);
      
      /// get comment
      QString getComment() const { return comment; }
      
      // set comment
      void setComment(const QString& s);
      
      /// get study name
      QString getStudyName() const { return studyName; }
      
      // set study name
      void setStudyName(const QString& s);
      
      /// get PMID
      QString getPMID() const { return pmid; }
      
      // set PMID
      void setPMID(const QString& s);
      
      /// get search ID
      QString getSearchID() const { return searchID; }
      
      // set search ID
      void setSearchID(const QString& s);
      
      /// get foci list ID
      QString getFociListID() const { return fociListID; }
      
      /// set foci list ID
      void setFociListID(const QString& s);
      
      /// get  foci color list ID
      QString getFociColorListID() const { return fociColorListID; }
      
      /// set foci color list ID
      void setFociColorListID(const QString& s);
      
      /// get study collection ID
      QString getStudyCollectionID() const { return sclID; }
      
      /// set study collection ID
      void setStudyCollectionID(const QString& s);
      
      /// get topic
      QString getTopic() const { return topic; }
      
      // set topic
      void setTopic(const QString& s);
      
      /// get category ID
      QString getCategoryID() const { return categoryID; }
      
      // set category ID
      void setCategoryID(const QString& s);
      
      // set parent study collection file
      void setParentStudyCollectionFile(StudyCollectionFile* parent);
      
   protected:
      // copy helper used by copy constructor and assignment operator
      void copyHelper(const StudyCollection& sc);
            
      // set modified
      void setModified();
      
      // read from XML
      void readXML(QDomNode& nodeIn) throw (FileException);
      
      // write to XML
      void writeXML(QDomDocument& xmlDoc,
                    QDomElement& parentElement);
                    
      // called to write XML
      void writeXML(XmlGenericWriter& xmlWriter, int indx) const throw (FileException);

      /// study collection name
      QString studyCollectionName;
                                 
      /// study collection creator
      QString studyCollectionCreator;
                                 
      /// study type
      QString studyType;
                                 
      /// comment
      QString comment;
                                 
      /// study name
      QString studyName;
      
      /// study pmid
      QString pmid;
      
      /// search id
      QString searchID;
      
      /// topic
      QString topic;
      
      /// category ID 
      QString categoryID;
      
      /// foci list ID
      QString fociListID;
      
      /// foci color list ID
      QString fociColorListID;
      
      /// study collection ID
      QString sclID;
      
      /// collection of studies
      std::vector<StudyNamePubMedID*> studyPMIDs; 
      
      /// parent study collection file
      StudyCollectionFile* parentStudyCollectionFile;
      
      //
      // !!!! UPDATE copyHelper() is any members added !!!!
      //
   friend class StudyCollectionFile;
   friend class StudyNamePubMedID;
};

/// class for a study meta-analysis file
class StudyCollectionFile : public AbstractFile {
   public:      
      // constructor
      StudyCollectionFile();
      
      // copy constructor
      StudyCollectionFile(const StudyCollectionFile& scf);
      
      // destructor
      ~StudyCollectionFile();
      
      // copy constructor
      StudyCollectionFile& operator=(const StudyCollectionFile& scf);

      // append a study collection file to "this" study collection file
      void append(const StudyCollectionFile& smdf);

      // add a study collection
      void addStudyCollection(StudyCollection* sc);
      
      /// get the number of study collections
      int getNumberOfStudyCollections() const { return studyCollections.size(); }
      
      // get a study collection
      StudyCollection* getStudyCollection(const int indx);
      
      // get a study collection (const method)
      const StudyCollection* getStudyCollection(const int indx) const;
      
      // delete a study collection
      void deleteStudyCollection(const int indx);
      
      // Clears current file data in memory.  Deriving classes must override this method and
      // call AbstractFile::clearAbstractFile() from its clear method.
      virtual void clear();
      
      // returns true if the file is isEmpty (contains no data)
      virtual bool empty() const;
      
      
      // find out if comma separated file conversion supported
      virtual void getCommaSeparatedFileSupport(bool& readFromCSV,
                                                bool& writeToCSV) const;

      /// write the file's memory in caret6 format to the specified name
      virtual QString writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException);


      
      
   protected:
      // the copy helper
      void copyHelper(const StudyCollectionFile& scf);
      
      /// read the file's data from a comma separated values file (throws exception if not supported)
      virtual void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException);

      // Read the contents of the file (header has already been read)
      virtual void readFileData(QFile& file,
                                QTextStream& stream,
                                QDataStream& binStream,
                                QDomElement& rootElement) throw (FileException);

      // Write the file's data (header has already been written)
      virtual void writeFileData(QTextStream& stream,
                                 QDataStream& binStream,
                                 QDomDocument& xmlDoc,
                                 QDomElement& rootElement) throw (FileException);
                                 
      // read from XML
      void readXML(QDomNode& nodeIn) throw (FileException);
      
      // write to XML
      void writeXML(QDomDocument& xmlDoc,
                    QDomElement& parentElement);
                    
      // the study collections
      std::vector<StudyCollection*> studyCollections;
      
      //
      // !!!! UPDATE copyHelper() is any members added !!!!
      //
                    
};

#endif // __STUDY_COLLECTION_FILE_H__


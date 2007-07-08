
#ifndef __STUDY_NAME_PUBMED_ID_H__
#define __STUDY_NAME_PUBMED_ID_H__

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

#include <vector>

#include <QString>

class QDomDocument;
class QDomElement;
class QDomNode;
class StudyMetaAnalysisFile;
class StudyMetaData;

/// Study Name and PubMed ID
class StudyNamePubMedID {
   public:
      // constructor
      StudyNamePubMedID();
      
      // destructor
      ~StudyNamePubMedID();
      
      // copy constructor
      StudyNamePubMedID(const StudyNamePubMedID& as);
      
      // assignment operator
      StudyNamePubMedID& operator=(const StudyNamePubMedID& as);
            
      // clear the studies
      void clear();
      
      // set the study names and PubMed IDs as a string
      void setAll(const QString& s);
      
      // get the study names and PubMed IDs as a string
      QString getAll() const;
      
      /// get the number of studies
      int getNumberOfStudies() const { return studyData.size(); }
      
      // get a study name and PubMed ID
      void getStudyNameAndPubMedID(const int indx,
                                   QString& nameOut,
                                   QString& pubMedIDOut) const;
      
      // set a study name and PubMed ID
      void setStudyNameAndPubMedID(const int indx,
                                   const QString& nameIn,
                                   const QString& pubMedIDIn);
                                   
      // add a study name and PubMed ID
      void addStudyNameAndPubMedID(const QString& nameIn,
                                   const QString& pubMedIDIn);
      
      // remove a study
      void removeStudy(const int indx);
      
      // remove studies
      void removeStudiesByIndex(const std::vector<int>& studyIndices);
      
      // set parent for study metadata
      void setParent(StudyMetaData* parentIn);
      
      // set parent for study meta-analysis
      void setParent(StudyMetaAnalysisFile* parentIn);
      
      // read the data from a StringTable
      void readDataFromStringTable(const StringTable& st) throw (FileException);
                  
      // called to read from an XML structure
      void readXML(QDomNode& node) throw (FileException);
      
      // called to write to an XML structure
      void writeXML(QDomDocument& xmlDoc,
                    QDomElement&  parentElement) const throw (FileException);
         
   protected:
      /// class for name and PubMed ID
      class StudyNamePMID {
         public:
            /// constructor
            StudyNamePMID(const QString& nameIn,
                          const QString& pmidIn) {
               name = nameIn;
               pmid = pmidIn;
            }
         
            /// the study name
            QString name;
            
            /// the PubMed ID
            QString pmid;
      };
      
      // copy helper used by copy constructor and assignment operator
      void copyHelper(const StudyNamePubMedID& as);
      
      // set modified status
      void setModified();
      
      /// study metadata that is parent of this pubmed ID list (DO NOT COPY)
      StudyMetaData* parentStudyMetaData;
      
      /// study meta-analysis that is parent of this pubmed ID list
      StudyMetaAnalysisFile* parentStudyMetaAnalysisFile;
      
      /// the name and PubMed ID of the study
      std::vector<StudyNamePMID> studyData;
      
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // If additional members are added be sure to update copyHelper() method.
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
};
      
#endif // __STUDY_NAME_PUBMED_ID_H__


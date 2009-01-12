
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
class StudyCollection;
class StudyMetaData;

/// Study Name and PubMed ID
class StudyNamePubMedID {
   public:
      // constructor
      StudyNamePubMedID();
      
      // constructor
      StudyNamePubMedID(const QString& nameIn,
                        const QString& pubMedIDIn,
                        const QString& mslIDIn);
      
      // destructor
      ~StudyNamePubMedID();
      
      // copy constructor
      StudyNamePubMedID(const StudyNamePubMedID& as);
      
      // assignment operator
      StudyNamePubMedID& operator=(const StudyNamePubMedID& as);
            
      // clear the studies
      void clear();
      
      /// get the name
      QString getName() const { return name; }
      
      // set the name
      void setName(const QString& s);
      
      /// get the PubMed ID
      QString getPubMedID() const { return pubMedID; }
      
      // set the PubMed ID
      void setPubMedID(const QString& s);
      
      /// get the MSL ID
      QString getMslID() const { return mslID; }
      
      // set the MSL ID
      void setMslID(const QString& s);
      
      // set parent for study metadata
      void setParent(StudyMetaData* parentIn);
      
      // set parent for study collection
      void setParent(StudyCollection* parentIn);
      
   protected:
      // called to read from an XML structure
      void readXML(QDomNode& node) throw (FileException);
      
      // called to write to an XML structure
      void writeXML(QDomDocument& xmlDoc,
                    QDomElement&  parentElement) const throw (FileException);
      
      /// the study name
      QString name;
      
      /// the PubMed ID
      QString pubMedID;
      
      /// MSL ID
      QString mslID;
      
      // copy helper used by copy constructor and assignment operator
      void copyHelper(const StudyNamePubMedID& as);
      
      // set modified status
      void setModified();
      
      /// study metadata that is parent of this pubmed ID list (DO NOT COPY)
      StudyMetaData* parentStudyMetaData;
      
      /// study collection that is parent of this pubmed ID list
      StudyCollection* parentStudyCollection;
      
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // If additional members are added be sure to update copyHelper() method.
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

   friend class StudyCollection;
};
      
#endif // __STUDY_NAME_PUBMED_ID_H__


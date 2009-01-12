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

#ifndef __CELL_STUDY_INFO_H__
#define __CELL_STUDY_INFO_H__

#include <vector>

#include <QString>

class QDomDocument;
class QDomElement;
class QDomNode;
class StringTable;

#include "FileException.h"

/// Class for storing cell study info
class CellStudyInfo {
   public: 
      /// constructor
      CellStudyInfo();

      /// destructor
      ~CellStudyInfo();

      /// equality operator
      bool operator==(const CellStudyInfo& cci) const; 
      
      /// clear the study info
      void clear();
      
      /// set element from text (used by SAX XML parser)
      void setElementFromText(const QString& elementName,
                              const QString& textValue);
                              
      /// called to read from an XML structure
      void readXML(QDomNode& node) throw (FileException);
      
      /// called to write to an XML structure
      void writeXML(QDomDocument& xmlDoc,
                            QDomElement&  parentElement,
                            const int studyNumber);
   
      /// write the data into a StringTable
      static void writeDataIntoStringTable(const std::vector<CellStudyInfo>& csi,
                                     StringTable& table);
      
      /// read the data from a StringTable
      static void readDataFromStringTable(std::vector<CellStudyInfo>& csi,
                                    const StringTable& table) throw (FileException);
      
      /// get full description of all fields for display to user
      QString getFullDescriptionForDisplayToUser(const bool useHTML) const;
            
      /// get url
      QString getURL() const { return url; }
      
      /// set url
      void setURL(const QString& s) { url = s; }
      
      /// get keywords
      QString getKeywords() const { return keywords; }
      
      /// set keywords
      void setKeywords(const QString& s) { keywords = s; }
      
      /// get title
      QString getTitle() const { return title; }
      
      /// set title
      void setTitle(const QString& s) { title = s; }
      
      /// get authors
      QString getAuthors() const { return authors; }
      
      /// set authors
      void setAuthors(const QString& s) { authors = s; }
      
      /// get citation
      QString getCitation() const { return citation; }
      
      /// set citation
      void setCitation(const QString& s) { citation = s; }
      
      /// get stereotaxic Space
      QString getStereotaxicSpace() const { return stereotaxicSpace; }
      
      /// set Stereotaxic Space
      void setStereotaxicSpace(const QString& s) { stereotaxicSpace = s; }
      
      /// get comment
      QString getComment() const { return comment; }
      
      /// set comment
      void setComment(const QString& s) { comment = s; }
      
      /// get partitioning scheme abbreviation
      QString getPartitioningSchemeAbbreviation() const { return partitioningSchemeAbbreviation; }
      
      /// set partitioning scheme abbreviation
      void setPartitioningSchemeAbbreviation(const QString& s) { partitioningSchemeAbbreviation = s; }
      
      /// get partitioning scheme full name
      QString getPartitioningSchemeFullName() const { return partitioningSchemeFullName; }
      
      /// set partitioning scheme full name
      void setPartitioningSchemeFullName(const QString& s) { partitioningSchemeFullName = s; }
      
   protected:
      /// study web page URL
      QString url;
      
      /// study key words
      QString keywords;
      
      /// study title
      QString title;
      
      /// study authors
      QString authors;
      
      /// study citation (journal)
      QString citation;
      
      /// stereotaxic space
      QString stereotaxicSpace;
      
      /// comment
      QString comment;
      
      /// partitioning scheme abbreviation
      QString partitioningSchemeAbbreviation;
      
      /// partitioning scheme full name
      QString partitioningSchemeFullName;
      
      /// tag for reading and writing study info
      static const QString tagCellStudyInfo;
      
      /// tag for reading and writing study info
      static const QString tagUrl;
      
      /// tag for reading and writing study info
      static const QString tagKeywords;
      
      /// tag for reading and writing study info
      static const QString tagTitle;
      
      /// tag for reading and writing study info
      static const QString tagAuthors;
      
      /// tag for reading and writing study info
      static const QString tagCitation;
      
      /// tag for reading and writing study info
      static const QString tagStereotaxicSpace;
      
      /// tag for reading and writing study info
      static const QString tagComment;
      
      /// tag for reading and writing study info
      static const QString tagStudyNumber;
      
      /// tag for reading and writing study info
      static const QString tagPartitioningSchemeAbbreviation;
      
      /// tag for reading and writing study info
      static const QString tagPartitioningSchemeFullName;
      
   friend class CellFile;
   friend class CellProjectionFile;
   friend class VocabularyFile;

};

#endif // __CELL_STUDY_INFO_H__

#ifdef __CELL_STUDY_INFO_MAIN__
      const QString CellStudyInfo::tagCellStudyInfo = "CellStudyInfo";
      const QString CellStudyInfo::tagUrl = "url";
      const QString CellStudyInfo::tagKeywords = "keywords";
      const QString CellStudyInfo::tagTitle = "title";
      const QString CellStudyInfo::tagAuthors = "authors";
      const QString CellStudyInfo::tagCitation = "citation";
      const QString CellStudyInfo::tagStereotaxicSpace = "stereotaxicSpace";
      const QString CellStudyInfo::tagComment = "comment";
      const QString CellStudyInfo::tagStudyNumber = "studyNumber";
      const QString CellStudyInfo::tagPartitioningSchemeAbbreviation = "partitioningSchemeAbbreviation";
      const QString CellStudyInfo::tagPartitioningSchemeFullName = "partitioningSchemeFullName";
#endif // __CELL_STUDY_INFO_MAIN__

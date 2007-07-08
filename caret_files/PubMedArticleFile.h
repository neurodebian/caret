
#ifndef __PUB_MED_ARTICLE_FILE_H__
#define __PUB_MED_ARTICLE_FILE_H__

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

// class for process a PubMed file retrieved via an HTTP request
class PubMedArticleFile : public AbstractFile {
   public:
      // constructor
      PubMedArticleFile();
      
      // destructor
      ~PubMedArticleFile();
      
      // retrieve the article information for the specified PubMed ID
      void retrieveArticleWithPubMedID(const QString& pubMedIDIn) throw (FileException);

      // clear the file
      void clear();
      
      // returns true if the file is isEmpty (contains no data)
      bool empty() const;
      
      // the article title
      QString getArticleTitle() const { return articleTitle; }
      
      // the list of authors
      QString getAuthors() const { return authors; }
      
      // the journal title
      QString getJournalTitle() const { return journalTitle; }
      
      // the Document Object Identifier
      QString getDocumentObjectIdentifier() const { return documentObjectIdentifier; }
      
      // the abstract
      QString getAbstractText() const { return abstractText; }
      
      // the PubMed ID
      QString getPubMedID() const { return pubMedID; }

      // get the medical subject headings
      QString getMedicalSubjectHeadings() const { return medicalSubjectHeadings; }
      
   protected:
      // read the file
      void readFileData(QFile& file, 
                        QTextStream& stream, 
                        QDataStream& binStream,     
                        QDomElement& rootElement) throw (FileException);
      
      // write the file
      void writeFileData(QTextStream& stream, 
                         QDataStream& binStream,
                         QDomDocument& xmlDoc,
                         QDomElement& rootElement) throw (FileException);
      
      // parse the xml 
      void parseXML(const QString xmlString) throw (FileException);
      
      // process the PubMedArticle children
      void processPubMedArticleChildren(QDomNode node) throw (FileException);
      
      // process the MedlineCitation children
      void processMedlineCitationChildren(QDomNode node) throw (FileException);
      
      // process the PubmedData children
      void processPubmedDataChildren(QDomNode node) throw (FileException);
      
      // process the Article children
      void processArticleChildren(QDomNode node) throw (FileException);
      
      // process the Journal children
      void processJournalChildren(QDomNode node) throw (FileException);
      
      // process the Journal Issue children
      void processJournalIssueChildren(QDomNode node) throw (FileException);
      
      // process the Journal Pub Date children
      void processJournalPubDateChildren(QDomNode node) throw (FileException);
      
      // process the AuthorList children
      void processAuthorListChildren(QDomNode node) throw (FileException);
      
      // process the ArticleIdList children
      void processArticleIdListChildren(QDomNode node) throw (FileException);
      
      // process the Pagination children
      void processPaginationChildren(QDomNode node) throw (FileException);
      
      // process the mesh heading list children
      void processMeshHeadingListChildren(QDomNode node) throw (FileException);
      
      // process the mesh heading children
      void processMeshHeadingChildren(QDomNode node) throw (FileException);
      
      // process the  children
      //void processChildren(QDomNode node) throw (FileException);
      
      /// the article title
      QString articleTitle;
      
      /// the list of authors
      QString authors;
      
      /// the journal pages
      QString journalPages;
      
      /// the journal title
      QString journalTitle;
      
      /// the journal volume
      QString journalVolume;
      
      /// the journal year
      QString journalYear;
      
      /// the medical subject headings
      QString medicalSubjectHeadings;
      
      /// the Document Object Identifier
      QString documentObjectIdentifier;
      
      /// the abstract
      QString abstractText;
      
      /// the PubMed ID
      QString pubMedID;
};

#endif // __PUB_MED_ARTICLE_FILE_H__


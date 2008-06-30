
#ifndef __STUDY_META_DATA_LINK_H__
#define __STUDY_META_DATA_LINK_H__

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

#include <QString>

#include "FileException.h"

class QDomDocument;
class QDomElement;
class QDomNode;

//========================================================================================
//
/// the Study Meta Data Link (connects data to a study metadata by study number or PubMed ID
class StudyMetaDataLink {
   public:
      // constructor
      StudyMetaDataLink();
      
      // destructor
      ~StudyMetaDataLink();
      
      // copy constructor
      StudyMetaDataLink(const StudyMetaDataLink& smdl);
      
      // assignment opertator
      StudyMetaDataLink& operator=(const StudyMetaDataLink& smdl);
      
      // equality operator
      bool operator==(const StudyMetaDataLink& smdl) const;
      
      // clear the link
      void clear();
      
      /// get the PubMed ID (negative if project ID, zero if invalid)
      QString getPubMedID() const { return pubMedID; }
      
      /// set the PubMed ID (negative if project ID)
      void setPubMedID(const QString& pmid) { pubMedID = pmid; }
      
      /// get the table number (blank if invalid)
      QString getTableNumber() const { return tableNumber; }
      
      /// set the table number (blank if invalid)
      void setTableNumber(const QString& tn);
      
      /// get the table sub header number (blank if invalid)
      QString getTableSubHeaderNumber() const { return tableSubHeaderNumber; }
      
      /// set the table sub header number (blank if invalid)
      void setTableSubHeaderNumber(const QString& tshn);
      
      /// get the figure number (blank if invalid)
      QString getFigureNumber() const { return figureNumber; }
      
      /// set the figure number (blank if invalid)
      void setFigureNumber(const QString& fn);
      
      /// get the panel letter/number (blank if invalid)
      QString getFigurePanelNumberOrLetter() const { return panelNumberOrLetter; }
      
      /// set the panel letter/number (blank if invalid)
      void setFigurePanelNumberOrLetter(const QString& pnl);
      
      /// get the page reference page number (blank if invalid)
      QString getPageReferencePageNumber() const { return pageReferencePageNumber; }
      
      /// set the page reference page number (blank if invalid)
      void setPageReferencePageNumber(const QString& prpn);
      
      /// get the page reference sub header number (blank if invalid)
      QString getPageReferenceSubHeaderNumber() const { return pageReferenceSubHeaderNumber; }
      
      /// set the page reference sub header number (blank if invalid)
      void setPageReferenceSubHeaderNumber(const QString& tshn);
      
      /// get the page number (negative if invalid)
      QString getPageNumber() const { return pageNumber; }
      
      /// set the page number (negative if invalid)
      void setPageNumber(const QString& pn);
      
      /// get the entire link in an "coded" text form
      QString getLinkAsCodedText() const;
      
      /// set the link from "coded" text form
      void setLinkFromCodedText(const QString& txt);
      
      // called to read from an XML structure
      void readXML(QDomNode& node) throw (FileException);
      
      // called to write to an XML structure
      void writeXML(QDomDocument& xmlDoc,
                    QDomElement&  parentElement) const throw (FileException);
   
      /// set element from text (used by SAX XML parser)
      void setElementFromText(const QString& elementName,
                              const QString& textValue);
                              
   protected:
      /// copy helper
      void copyHelper(const StudyMetaDataLink& smdl);
      
      /// the PubMed ID (negative if project ID, 0 if invalid)
      QString pubMedID;
      
      /// the table number (blank if invalid)
      QString tableNumber;
      
      /// the table sub header number (blank if invalid)
      QString tableSubHeaderNumber;
      
      /// the figure number (blank if invalid)
      QString figureNumber;
      
      /// the panel letter/number (blank if invalid)
      QString panelNumberOrLetter;
      
      /// the page number (blank if invalid)
      QString pageNumber;
      
      /// page reference page number (blank if invalid)
      QString pageReferencePageNumber;
      
      /// page reference sub header number (blank if invalid)
      QString pageReferenceSubHeaderNumber;
      
      // NOTE: IF MEMBERS ADDED UPDATE THE COPY HELPER
      
      
      //
      //----- tags for reading and writing 
      //
      /// tag for reading and writing study metadata
      static const QString tagStudyMetaDataLink;
      
      /// tag for reading and writing study metadata
      static const QString tagPubMedID;
      
      /// tag for reading and writing study metadata
      static const QString tagTableNumber;
      
      /// tag for reading and writing study metadata
      static const QString tagTableSubHeaderNumber;
      
      /// tag for reading and writing study metadata
      static const QString tagFigureNumber;
      
      /// tag for reading and writing study metadata
      static const QString tagPanelNumberOrLetter;
      
      /// tag for reading and writing study metadata
      static const QString tagPageNumber;
      
      /// tag for reading and writing study metadata
      static const QString tagPageReferencePageNumber;
      
      /// tag for reading and writing study metadata
      static const QString tagPageReferenceSubHeaderNumber;
      
   friend class CellBase;
   friend class StudyMetaDataLinkSet;
};

#ifdef __STUDY_META_DATA_LINK_MAIN__
      const QString StudyMetaDataLink::tagStudyMetaDataLink = "StudyMetaDataLink";
      const QString StudyMetaDataLink::tagPubMedID = "pubMedID";
      const QString StudyMetaDataLink::tagTableNumber = "tableNumber";
      const QString StudyMetaDataLink::tagTableSubHeaderNumber = "tableSubHeaderNumber";
      const QString StudyMetaDataLink::tagFigureNumber = "figureNumber";
      const QString StudyMetaDataLink::tagPanelNumberOrLetter = "panelNumberOrLetter";
      const QString StudyMetaDataLink::tagPageNumber = "pageNumber";
      const QString StudyMetaDataLink::tagPageReferencePageNumber = "pageReferencePageNumber";
      const QString StudyMetaDataLink::tagPageReferenceSubHeaderNumber = "pageReferenceSubHeaderNumber";
#endif // __STUDY_META_DATA_LINK_MAIN__

#endif // __STUDY_META_DATA_LINK_H__


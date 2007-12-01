
#ifndef __FOCI_PROJECTION_FILE_SAX_READER_H__
#define __FOCI_PROJECTION_FILE_SAX_READER_H__

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

#include <stack>
#include <QString>

#include <QXmlDefaultHandler>

#include "StudyMetaDataLinkSet.h"

class CellProjectionFile;

/// class for reading a Foci Projection File with a SAX Parser
class CellProjectionFileSaxReader : public QXmlDefaultHandler {
   public:
      // constructor
      CellProjectionFileSaxReader(CellProjectionFile* fociProjectionFileIn);
      
      // destructor
      virtual ~CellProjectionFileSaxReader();
      
      // start an element
      bool startElement(const QString& namespaceURI,
                        const QString& localName,
                        const QString& qName,
                        const QXmlAttributes& attributes);
                        
      // end an element
      bool endElement(const QString& namspaceURI,
                      const QString& localName,
                      const QString& qName);
                      
      // get characters in an element
      bool characters(const QString& s);
      
      // a fatal error occurs
      bool fatalError(const QXmlParseException& e);
      
      // get the error message
      QString getErrorMessage() const { return errorMessage; }
      
   protected:
      /// file reading states
      enum STATE {
         /// no state
         STATE_NONE,
         /// processing Cell/Foci_Projection_File 
         STATE_CELL_PROJ_FILE,
         /// processing file header
         STATE_FILE_HEADER,
         /// processing file header element
         STATE_FILE_HEADER_ELEMENT,
         /// processing file header name
         STATE_FILE_HEADER_ELEMENT_NAME,
         /// processing file header value
         STATE_FILE_HEADER_ELEMENT_VALUE,
         /// processing cell projection
         STATE_CELL_PROJECTION,
         /// processing cell base
         STATE_CELL_BASE,
         /// old study info
         STATE_STUDY_INFO,
         /// processing study metadata link set
         STATE_METADATA_LINK_SET,
         /// processing study metadata link
         STATE_METADATA_LINK
      };
      
      /// the file reading state
      STATE state;
      
      /// the state stack used when reading a file
      std::stack<STATE> stateStack;
      
      /// the error message
      QString errorMessage;
      
      /// cell projection file that is being read
      CellProjectionFile* cellProjectionFile;
      
      /// current cell projection being read
      CellProjection cellProjection;
      
      /// text for an element
      QString elementText;
      
      /// header name text
      QString headerNameText;
      
      /// the study meta data link
      StudyMetaDataLink studyMetaDataLink;      
      
      /// the study meta data link set
      StudyMetaDataLinkSet studyMetaDataLinkSet;   
      
      /// the cell study info
      CellStudyInfo cellStudyInfo;
};

#endif // __FOCI_PROJECTION_FILE_SAX_READER_H__



#ifndef __GENERIC_XML_FILE_H__
#define __GENERIC_XML_FILE_H__

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

#include <QDomElement>

class QDomDocument;
class QDomNode;

#include "AbstractFile.h"


/// Class for stroring an XML file.
class GenericXmlFile : public AbstractFile {
   public:
      
      /// Constructor
      GenericXmlFile();
      
      /// Destructor
      ~GenericXmlFile();
      
      /// clear the file
      void clear();
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return false; }  //sumsFileInfo.isEmpty(); }
      
      /// Get element(s) from the XML file.
      void getValue(const QString& elementName,
                    std::vector<QString>& elements);
                         
      /// Get an element from the XML file.
      QString getValue(const QString& elementName);
                         
   protected:
      /// read the file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// write the file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);

      /// Search the XML tree to find the specified element.
      void getValueSearchTree(const std::vector<QString>& elementNameComponents,
                              const int nameComponentIndex,
                              const QDomNode node,
                              std::vector<QString>& elements);
                                  
      /// root element of the document
      QDomElement rootElement;
};

#endif // __GENERIC_XML_FILE_H__


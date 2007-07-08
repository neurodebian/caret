
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

#include <algorithm>
#include <iostream>
#include <sstream>

#include "DebugControl.h"
#include "FileUtilities.h"
#include "GenericXmlFile.h"
#include "StringUtilities.h"

/**
 * Constructor.
 */
GenericXmlFile::GenericXmlFile()
   : AbstractFile("Generic XML File", 
                  ".xml",
                  true, 
                  FILE_FORMAT_XML, 
                  FILE_IO_NONE, 
                  FILE_IO_NONE, 
                  FILE_IO_READ_ONLY,
                  FILE_IO_NONE)
{
   clear();
}

/**
 * Destructor.
 */
GenericXmlFile::~GenericXmlFile()
{
   clear();
}

/**
 * clear the file.
 */
void 
GenericXmlFile::clear()
{
   clearAbstractFile();
   rootElement.clear();
}

/**
 * Get an element from the XML file.
 */
QString 
GenericXmlFile::getValue(const QString& elementName)
{
   QString value;
   
   std::vector<QString> elems;
   getValue(elementName, elems);
   if (elems.empty() == false) {
      value = elems[0];
   }
   
   return value;
}

/**
 * Get element(s) from the XML file.
 */
void
GenericXmlFile::getValue(const QString& elementName,
                         std::vector<QString>& elements) 
{
   elements.clear();

   if (elementName.isEmpty() == false) {
      std::vector<QString> nameComponents;
      StringUtilities::token(elementName, ":", nameComponents);
      if (nameComponents.empty() == false) {
         if (nameComponents[0] == rootElement.tagName()) {
            getValueSearchTree(nameComponents, 1, rootElement.firstChild(), elements);
         }
      }
   }   
}

/**
 * Search the XML tree to find the specified element.
 */
void
GenericXmlFile::getValueSearchTree(const std::vector<QString>& elementNameComponents,
                                   const int nameComponentIndex,
                                   const QDomNode nodeIn,
                                   std::vector<QString>& elements) 
{
   QDomNode node = nodeIn;
   
   while (node.isNull() == false) {
      bool continueLoop = false;
      
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) { 
         continueLoop = true;
         
         //
         // Does this element tag match the current name component
         //
         QString tagName(elem.tagName());
         if (tagName == elementNameComponents[nameComponentIndex]) {
            //
            // Is this last name component 
            //
            if (nameComponentIndex == static_cast<int>(elementNameComponents.size() - 1)) {
               elements.push_back(getXmlElementFirstChildAsString(elem));
            }
            else {
               //
               // Continue searching
               //
               getValueSearchTree(elementNameComponents,
                                  nameComponentIndex + 1,
                                  elem.firstChild(),
                                  elements);
               continueLoop = false;
            }
         }
      }
      node = node.nextSibling();
      
      if (continueLoop == false) {
         return;
      }
   }   
}

/**
 * Process files tag.
 *
void
GenericXmlFile::processFiles(QDomElement& filesElem) throw (FileException)
{
   QDomNode node = filesElem.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) { 
         if (DebugControl::getDebugOn()) {      
            std::cout << "Tag Name: " << elem.tagName().toAscii().constData() << std::endl;
         }
         if (elem.tagName() == "header") {
            readHeaderXML(elem);
         }
         else if (elem.tagName() == "file-version") {
            processFileVersion(elem);
         }
         else if (elem.tagName() == "file") {
            processFile(elem);
         }
         else {
            std::cerr << "Atlas Space node not recognized in root " << elem.tagName().toAscii().constData() << std::endl;
         }
      }
      node = node.nextSibling();
   }
}
*/

/**
 * read the file.
 */
void 
GenericXmlFile::readFileData(QFile&, QTextStream& /* stream */, QDataStream&,
                                  QDomElement& rootElementIn) throw (FileException)
{
   rootElement.clear();
   rootElement = rootElementIn;
   
/*
   //
   // Place the file contents into a QDomDocument which will parse file.
   //
   QString errorMessage;
   int errorLine = 0, errorColumn = 0;
   QDomDocument doc("atlas-space-file-document");
   if (doc.setContent(s, &errorMessage, &errorLine, &errorColumn) == false) {
      std::ostringstream str;
      str << "Error parsing at line " << errorLine << " column " << errorColumn << ".  ";
      str << errorMessage << std::ends;
      throw FileException("", str.str().c_str());
   }
   
   //
   // Traverse the direct children just to get the file header.
   //
   rootElement = doc.documentElement();
*/
   if (rootXmlElementTagName.isEmpty() == false) {
      const QString rootElementNameFound(rootElement.tagName());
      if (rootElementNameFound != rootXmlElementTagName) {
         QString msg("\nNot an GenericXmlFile.  Root element is: ");
         msg.append(rootElementNameFound);
         msg.append(".\nRoot element should be: ");
         msg.append(rootXmlElementTagName);
         throw FileException(filename, msg);
      }
   }
    
   QDomNode node = rootElement.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) { 
         if (DebugControl::getDebugOn()) {      
            std::cout << "Tag Name: " << elem.tagName().toAscii().constData() << std::endl;
         }
         if ((elem.tagName() == xmlHeaderOldTagName) ||
             (elem.tagName() == xmlHeaderTagName)) {
            // ignore
         }
         else if (elem.tagName() == "version") {
         //   processFileVersion(elem);
         }
         else if (elem.tagName() == "files") {
         //   processFiles(elem);
         }
      }
      node = node.nextSibling();
   }
}

/**
 * write the file.
 */
void 
GenericXmlFile::writeFileData(QTextStream&, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException(filename, "Writing GenericXmlFile not supported.");
}


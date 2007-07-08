
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

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>

#include "DebugControl.h"
#include "FileUtilities.h"
#include "StringUtilities.h"

#define __SUMS_FILE_LIST_FILE_MAIN__
#include "SumsFileListFile.h"
#undef __SUMS_FILE_LIST_FILE_MAIN__

/**
 * Constructor.
 */
SumsFileListFile::SumsFileListFile()
   : AbstractFile("Sums File List File", 
                  ".sums", 
                  true, 
                  FILE_FORMAT_XML, 
                  FILE_IO_NONE, 
                  FILE_IO_NONE, 
                  FILE_IO_READ_ONLY,
                  FILE_IO_NONE)
{
   clear();
   setRootXmlElementTagName("sums");
}

/**
 * Destructor.
 */
SumsFileListFile::~SumsFileListFile()
{
   clear();
}

/**
 * clear the file.
 */
void 
SumsFileListFile::clear()
{
   clearAbstractFile();
   sumsFileInfo.clear();
   fileVersion = 1;
   excludeSpecFileFlag = false;
}

/**
 * read the file from a string.
 */
/*
void 
SumsFileListFile::readFileFromString(const QString& s) throw (FileException)
{
   sumsFileInfo.clear();

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
   // Traverse the direct children
   //
   QDomElement docElem = doc.documentElement();
   const QString rootElementNameFound(docElem.tagName());
   if (rootElementNameFound != "sums") {
      QString msg("\nNot an SumsFileListFile.  Root element is: ");
      msg.append(rootElementNameFound);
      msg.append(".\nRoot element should be: ");
      msg.append("sums");
      throw FileException(filename, msg);
   }
    
   QDomNode node = docElem.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) { 
         if (DebugControl::getDebugOn()) {      
            std::cout << "Tag Name: " << elem.tagName() << std::endl;
         }
         if (elem.tagName() == "header") {
            readHeaderXML(elem);
         }
         else if (elem.tagName() == "version") {
            processFileVersion(elem);
         }
         else if (elem.tagName() == "files") {
            processFiles(elem);
         }
         else if (elem.tagName() == "directories") {
         }
         else {
            std::cerr << "SumsFileListFile xml node not recognized \"" 
                      << elem.tagName() << "\"" << std::endl;
         }
      }
      node = node.nextSibling();
   }
 
   //
   // sort by type 
   //
   sort(SORT_ORDER_TYPE);
}      
*/

/**
 * Process files tag.
 */
void
SumsFileListFile::processFiles(QDomElement& filesElem) throw (FileException)
{
   QDomNode node = filesElem.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) { 
         if (DebugControl::getDebugOn()) {      
            std::cout << "Tag Name: " << elem.tagName().toAscii().constData() << std::endl;
         }
         if ((elem.tagName() == xmlHeaderTagName) ||
             (elem.tagName() == xmlHeaderOldTagName)) {
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

/**
 * Process the file version.
 */
void
SumsFileListFile::processFileVersion(QDomElement& elem) throw (FileException)
{
   QDomNode vNode = elem.firstChild();
   if (vNode.isNull() == false) {
      QDomText textNode = vNode.toText();
      if (textNode.isNull() == false) {
         fileVersion = StringUtilities::toInt(textNode.data());
      }
   }
}

/**
 * Process an atlas space.
 */
void
SumsFileListFile::processFile(QDomElement& elem) throw (FileException)
{
   SumsFileInfo sfi;
   
   QDomNode node = elem.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {     
         const QString tagName(elem.tagName()); 

         if (tagName == "comment") {
            sfi.setComment(getXmlElementFirstChildAsString(elem));
         }
         else if (tagName == "date") {
            sfi.setDate(getXmlElementFirstChildAsString(elem));
         }
         else if (tagName == "dir_id") {
         }
         else if (tagName == "id") {
            sfi.setID(getXmlElementFirstChildAsString(elem));
         }
         else if (tagName == "location") {
            sfi.setURL(getXmlElementFirstChildAsString(elem));
         }
         else if (tagName == "name") {
            sfi.setNameWithPath(getXmlElementFirstChildAsString(elem));
         }
         else if (tagName == "parent_id") {
         }
         else if (tagName == "short_name") {
            sfi.setNameWithoutPath(getXmlElementFirstChildAsString(elem));
         }
         else if (tagName == "size") {
            sfi.setSize(StringUtilities::toInt(getXmlElementFirstChildAsString(elem)));
         }
         else if (tagName == "state") {
         }
         else if (tagName == "state_string") {
            sfi.setState(getXmlElementFirstChildAsString(elem));
         }
         else if (tagName == "status") {
         }
         else if (tagName == "user_id") {
         }
         else {
            std::cout << "Unrecognized \"file\" tag (" << tagName.toAscii().constData()
                      << ") for SumsFileListFile " << std::endl;
         }
      }
      node = node.nextSibling();
   }
   
   if (sfi.isValid()) {
      if (DebugControl::getDebugOn()) {
         std::cout << "Have valid file named " << sfi.getNameWithoutPath().toAscii().constData() << "." << std::endl;
      }
      
      //
      // Exclude spec files if requested
      //
      bool addIt = true;
      if (FileUtilities::filenameExtension(sfi.getNameWithoutPath()) == "spec") {
         if (excludeSpecFileFlag) {
            addIt = false;
         }
      }
      
      if (addIt) {
         addSumsFile(sfi);
      }
   }
}

/**
 * Add a file info to this file.
 */
void
SumsFileListFile::addSumsFile(const SumsFileInfo& sfi)
{
   sumsFileInfo.push_back(sfi);
}

/**
 * get a sums file (const method)
 */
const SumsFileInfo*
SumsFileListFile::getSumsFileInfo(const int index) const
{
   if ((index >= 0) && (index < getNumberOfSumsFiles())) {
      return &sumsFileInfo[index];      
   }
   return NULL;
}
      
/**
 * get a sums file 
 */
SumsFileInfo*
SumsFileListFile::getSumsFileInfo(const int index)
{
   if ((index >= 0) && (index < getNumberOfSumsFiles())) {
      return &sumsFileInfo[index];      
   }
   return NULL;
}
      
/**
 * sort the files.
 */
void 
SumsFileListFile::sort(const SORT_ORDER so)
{
   switch (so) {
      case SORT_ORDER_DATE:
         SumsFileInfo::setSortingKey(SumsFileInfo::SORTING_KEY_DATE);
         break;
      case SORT_ORDER_NAME:
         SumsFileInfo::setSortingKey(SumsFileInfo::SORTING_KEY_NAME);
         break;
      case SORT_ORDER_TYPE:
         SumsFileInfo::setSortingKey(SumsFileInfo::SORTING_KEY_TYPE);
         break;
   }
   std::sort(sumsFileInfo.begin(), sumsFileInfo.end());
}
    
/**
 * set the selection status of all files.
 */
void
SumsFileListFile::setAllFileSelectionStatus(const bool status)
{
   const int num = getNumberOfSumsFiles();
   for (int i = 0; i < num; i++) {
      SumsFileInfo* sfi = getSumsFileInfo(i);
      sfi->setSelected(status);
   }
}

/**
 * See if there is a common subdirectory prefix for all of the files
 */
QString
SumsFileListFile::getCommonSubdirectoryPrefix() const
{
   const int num = getNumberOfSumsFiles();
   if (num > 0) {
      const SumsFileInfo* sfi = getSumsFileInfo(0);
      const QString prefix(FileUtilities::getSubdirectoryPrefix(sfi->getNameWithPath()));
      if (prefix.isEmpty() == false) {
         for (int i = 1; i < num; i++) {
            const SumsFileInfo* sfi = getSumsFileInfo(i);
            const QString pf(FileUtilities::getSubdirectoryPrefix(sfi->getNameWithPath()));
            if (pf != prefix) {
               return "";
            }
         }
         return prefix;
      }
   }
   return "";
}

/**
 * Remove paths from all files.
 */
void
SumsFileListFile::removePathsFromAllFiles()
{
   for (int i = 0; i < getNumberOfSumsFiles(); i++) {
      SumsFileInfo* sfi = getSumsFileInfo(i);
      sfi->setNameWithPath(sfi->getNameWithoutPath());
   }
}

/**
 * Remove subdirectory prefix from all files
 */
void
SumsFileListFile::removeSubdirectoryPrefix()
{
   const int num = getNumberOfSumsFiles();
   for (int i = 0; i < num; i++) {
      SumsFileInfo* sfi = getSumsFileInfo(i);
      QString name(sfi->getNameWithPath());
      const int pos = StringUtilities::findFirstOf(name, "/\\");
      if (pos != -1) {
         name = name.mid(pos + 1);
         sfi->setNameWithPath(name);
      }
   }
}

/**
 * read the file.
 */
void 
SumsFileListFile::readFileData(QFile&, QTextStream& /* stream */, QDataStream&,
                                  QDomElement& rootElement) throw (FileException)
{
   sumsFileInfo.clear();

   //
   // Traverse the direct children
   //
   const QString rootElementNameFound(rootElement.tagName());
   if (rootElementNameFound != "sums") {
      QString msg("\nNot an SumsFileListFile.  Root element is: ");
      msg.append(rootElementNameFound);
      msg.append(".\nRoot element should be: ");
      msg.append("sums");
      throw FileException(filename, msg);
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
            readHeaderXML(elem);
         }
         else if (elem.tagName() == "version") {
            processFileVersion(elem);
         }
         else if (elem.tagName() == "files") {
            processFiles(elem);
         }
         else if (elem.tagName() == "directories") {
         }
         else {
            std::cerr << "SumsFileListFile xml node not recognized \"" 
                      << elem.tagName().toAscii().constData() << "\"" << std::endl;
         }
      }
      node = node.nextSibling();
   }
 
   //
   // sort by type 
   //
   sort(SORT_ORDER_DATE);
/*
   try {
      readFileFromString(stream.read());
   }
   catch (FileException& e) {
      throw FileException(filename, e.what());
   }
*/
}

/**
 * write the file.
 */
void 
SumsFileListFile::writeFileData(QTextStream&, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException(filename, "Writing SumsFileListFile not supported.");
}

//---------------------------------------------------------------------------------

/**
 * Constructor.
 */
SumsFileInfo::SumsFileInfo()
{
   fileNameWithPath = "";
   fileNameWithoutPath = "";
   fileURL = "";
   fileDate = "";
   fileComment = "";
   fileID = "";
   fileSelected = true;
   fileSize = 0;
   fileState = "";
   fileTypeName = "";
}

/**
 * Constructor.
 */
SumsFileInfo::~SumsFileInfo()
{
}

/**
 * Get valid.
 */
bool
SumsFileInfo::isValid() const
{
   if ((fileNameWithoutPath.isEmpty() == false) &&
       (fileNameWithPath.isEmpty() == false) &&
       (fileURL.isEmpty() == false)) {
      return true;
   }
   return false;
}

/**
 * set the file's name.
 */
void 
SumsFileInfo::setNameWithoutPath(const QString& s)
{ 
   fileNameWithoutPath = s; 
   fileTypeName = AbstractFile::getFileTypeNameFromFileName(s);
}
      
/**
 * Comparison for sorting
 */
bool
SumsFileInfo::operator<(const SumsFileInfo& sfi) const
{
   bool result = false;
   
   switch (sortingKey) {
      case SORTING_KEY_DATE:
         if (fileDate == sfi.fileDate) {
            if (fileTypeName == sfi.fileTypeName) {
               result = (fileNameWithoutPath < sfi.fileNameWithoutPath);
            }
            else {
               result = (fileTypeName < sfi.fileTypeName);
            }
         }
         else {
            result = (fileDate > sfi.fileDate);
         }
         break;
      case SORTING_KEY_NAME:
         result = (fileNameWithoutPath < sfi.fileNameWithoutPath);
         break;
      case SORTING_KEY_TYPE:
         if (fileTypeName == sfi.fileTypeName) {
            result = (fileNameWithoutPath < sfi.fileNameWithoutPath);
         }
         else {
            result = (fileTypeName < sfi.fileTypeName);
         }
         break;
   }
   
   return result;
}

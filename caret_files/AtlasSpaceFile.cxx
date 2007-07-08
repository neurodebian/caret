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

#include "AtlasSpaceFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * The constructor.
 */
AtlasSpaceFile::AtlasSpaceFile()
   : AbstractFile("Atlas Space File", 
                  SpecFile::getAtlasSpaceFileExtension(),
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
 * The destructor.
 */
AtlasSpaceFile::~AtlasSpaceFile()
{
   clear();
}

/** 
 * Append a Atlas Surface file to this one
 */
void 
AtlasSpaceFile::append(AtlasSpaceFile& asf) throw (FileException)
{
   for (int i = 0; i < asf.getNumberOfAtlasSurfaces(); i++) {
      addAtlasSurface(*asf.getAtlasSurface(i));
   }

   //
   // transfer the file's comment
   //
   appendFileComment(asf);
}

/**
 * Clear the file.
 */
void
AtlasSpaceFile::clear()
{
   clearAbstractFile();
   //rootElementName = "atlas-space-file");
   atlasSurfaces.clear();
   fileVersion = -1;
}

/**
 * sort the files.
 */
void 
AtlasSpaceFile::sort()
{
   std::sort(atlasSurfaces.begin(), atlasSurfaces.end());
}
      

/**
 * Read the file.
 */
void
AtlasSpaceFile::readFileData(QFile& /*file*/, 
                             QTextStream& /* stream */,
                             QDataStream&,
                             QDomElement& rootElement) throw (FileException)
{
/*
   //
   // Read the remainder of the file into a QString.
   //
   const QString remainderOfFile = stream.read();
   
   //
   // Place the file contents into a QDomDocument which will parse file.
   //
   QString errorMessage;
   int errorLine = 0, errorColumn = 0;
   QDomDocument doc("atlas-surface-file-document");
   if (doc.setContent(remainderOfFile, &errorMessage, &errorLine, &errorColumn) == false) {
      std::ostringstream str;
      str << "Error parsing at line " << errorLine << " column " << errorColumn << ".  ";
      str << errorMessage << std::ends;
      throw FileException(filename, str.str().c_str());
   }

   //
   // Traverse the direct children
   //
   //QDomElement docElem = doc.documentElement();
   const QString rootElementNameFound(rootElement.tagName());
   if (rootElementNameFound != rootElementName) {
      QString msg("\nNot an AtlasSurfaceFile.  Root element is: ");
      msg.append(rootElementNameFound);
      msg.append(".\nRoot element should be: ");
      msg.append(rootElementName);
      throw FileException(filename, msg);
   }
*/   
    
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
         else if (elem.tagName() == "file-version") {
            processFileVersion(elem);
         }
         else if (elem.tagName() == "surface") {
            processAtlasSurface(elem);
         }
         else {
            std::cerr << "Atlas Surface node not recognized in root " << elem.tagName().toAscii().constData() << std::endl;
         }
      }
      node = node.nextSibling();
   }
   
   sort();
}

/**
 * Process the file version.
 */
void
AtlasSpaceFile::processFileVersion(QDomElement& elem) throw (FileException)
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
 * Process an atlas surface.
 */
void
AtlasSpaceFile::processAtlasSurface(QDomElement& elem) throw (FileException)
{
   QString description;
   QString format;
   QString species;
   QString space;
   QString structure;
   QString topoFileName;
   QString coordFileName;
   QString vtkFileName;
   
   QDomNode node = elem.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {     
         const QString tagName(elem.tagName()); 

         if (tagName == "description") {
            description = getXmlElementFirstChildAsString(elem);
         }
         else if (tagName == "format") {
            format = getXmlElementFirstChildAsString(elem);
         }
         else if (tagName == "species") {
            species = getXmlElementFirstChildAsString(elem);
         }
         else if (tagName == "space") {
            space = getXmlElementFirstChildAsString(elem);
         }
         else if ((tagName == "hemisphere") ||
                  (tagName == "structure")) {
            structure = getXmlElementFirstChildAsString(elem);
         }
         else if (tagName == "topo-file") {
            topoFileName = getXmlElementFirstChildAsString(elem);
         }
         else if (tagName == "coord-file") {
            coordFileName = getXmlElementFirstChildAsString(elem);
         }
         else if (tagName == "vtk-file") {
            vtkFileName = getXmlElementFirstChildAsString(elem);
         }
      }
      node = node.nextSibling();
   }
   
   bool valid = false;
   
   if ((description.isEmpty() == false) &&
       (format.isEmpty() == false) &&
       (species.isEmpty() == false) &&
       (space.isEmpty() == false) &&
       (structure.isEmpty() == false)) {
      const QString dirPath(FileUtilities::dirname(getFileName()));
      if ((topoFileName.isEmpty() == false) &&
          (coordFileName.isEmpty() == false) &&
          (format == "caret")) {
         valid = true;
         addAtlasSurface(AtlasSpaceSurface(
                                      dirPath,
                                      description,
                                      species,
                                      space,
                                      structure,
                                      topoFileName,
                                      coordFileName));
      }
      else if ((vtkFileName.isEmpty() == false) &&
               (format == "vtk")) {
         valid = true;
         addAtlasSurface(AtlasSpaceSurface(
                                      dirPath,
                                      description,
                                      species,
                                      space,
                                      structure,
                                      vtkFileName));
      }
   }

   if (valid == false) {
      std::cout << "Atlas Surface File Error: " << std::endl;
      std::cout << "   description: " << description.toAscii().constData() << std::endl;
      std::cout << "   format: " << format.toAscii().constData() << std::endl;
      std::cout << "   species: " << species.toAscii().constData() << std::endl;
      std::cout << "   space: " << space.toAscii().constData() << std::endl;
      std::cout << "   structure: " << structure.toAscii().constData() << std::endl;
      std::cout << "   topoFileName: " << topoFileName.toAscii().constData() << std::endl;
      std::cout << "   coordFileName: " << coordFileName.toAscii().constData() << std::endl;
      std::cout << "   vtkFileName: " << vtkFileName.toAscii().constData() << std::endl;
      std::cout << std::endl;
   } 
}


/**
 * Write and atlas surface file.
 */
void
AtlasSpaceFile::writeFileData(QTextStream& /*stream*/, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                              QDomElement& /* rootElement */) throw(FileException)
{
   throw FileException(filename, "Writing Atlas Surface is not supported");
}


//
//===================================================================================
//

/**
 * Constructor for caret format.
 */
AtlasSpaceSurface::AtlasSpaceSurface(const QString& directoryPathIn,
                           const QString& descriptionIn,
                           const QString& speciesIn,
                           const QString& spaceIn,
                           const QString& structureIn,
                           const QString& topologyFileNameIn,
                           const QString& coordinateFileNameIn)
{
   atlasFileFormat = ATLAS_FILE_FORMAT_CARET;
   directoryPath = directoryPathIn;
   description = descriptionIn;
   species = speciesIn;
   space = spaceIn;
   structure = structureIn;
   topologyFileName = topologyFileNameIn;
   coordinateFileName = coordinateFileNameIn;
}
             
/**
 * Constructor for vtk format.
 */
AtlasSpaceSurface::AtlasSpaceSurface(const QString& directoryPathIn,
                           const QString& descriptionIn,
                           const QString& speciesIn,
                           const QString& spaceIn,
                           const QString& structureIn,
                           const QString& vtkFileNameIn)
{
   atlasFileFormat = ATLAS_FILE_FORMAT_VTK;
   directoryPath = directoryPathIn;
   description = descriptionIn;
   species = speciesIn;
   space = spaceIn;
   structure = structureIn;
   vtkFileName = vtkFileNameIn;
}
  
/**
 * Destructor
 */
AtlasSpaceSurface::~AtlasSpaceSurface()
{
}

/**
 * less than operator for sorting.
 */
bool 
AtlasSpaceSurface::operator<(const AtlasSpaceSurface& asf) const
{
   if (species < asf.species) {
      return true;
   } else if (species == asf.species) {
      if (space < asf.space) {
         return true;
      }
      else if (space == asf.space) {
         if (structure < asf.structure) {
            return true;
         }
      }
   }
   
   return false;
}


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

#include <iostream>
#include <sstream>

#define VE_ATLAS_SURFACE_DIRECTORY_FILE_DEFINE
#include "AtlasSurfaceDirectoryFile.h"
#undef VE_ATLAS_SURFACE_DIRECTORY_FILE_DEFINE

#include "FileUtilities.h"
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * Constructor.
 */
AtlasSurface::AtlasSurface(const QString& atlasNameIn,
                           const QString& fileNameIn,
                           const QString& descriptiveNameIn,
                           const QString& specFileNameIn,
                           const ATLAS_SURFACE_ANATOMY_TYPE anatomyTypeIn,
                           const ATLAS_SURFACE_FILE_TYPE fileTypeIn)
{
   setData(atlasNameIn,
           fileNameIn,
           descriptiveNameIn,
           specFileNameIn,
           anatomyTypeIn,
           fileTypeIn);
}

/**
 * Destructor.
 */
AtlasSurface::~AtlasSurface()
{
}

/**
 * Get attribute data for an atlas surface.
 */
void
AtlasSurface::getData(QString& atlasNameOut,
                      QString& fileNameOut,
                      QString& descriptiveNameOut,
                      QString& specFileNameOut,
                      ATLAS_SURFACE_ANATOMY_TYPE& anatomyTypeOut,
                      ATLAS_SURFACE_FILE_TYPE& fileTypeOut) const
{
   atlasNameOut = atlasName;
   fileNameOut = fileName;
   descriptiveNameOut = descriptiveName;
   specFileNameOut = specFileName;
   anatomyTypeOut = anatomyType;
   fileTypeOut = filetype;
}

/**
 * Set attribute data for an atlas surface.
 */
void AtlasSurface::setData(const QString& atlasNameIn,
                           const QString& fileNameIn,
                           const QString& descriptiveNameIn,
                           const QString& specFileNameIn,
                           const ATLAS_SURFACE_ANATOMY_TYPE anatomyTypeIn,
                           const ATLAS_SURFACE_FILE_TYPE fileTypeIn)
{
   atlasName = atlasNameIn;
   descriptiveName = descriptiveNameIn;
   specFileName = specFileNameIn;
   anatomyType = anatomyTypeIn;
   fileName = fileNameIn;
   filetype = fileTypeIn;
}

/**
 * Constructor.
 */
AtlasSurfaceDirectoryFile::AtlasSurfaceDirectoryFile()
   : AbstractFile("Atlas Surface Directory File",
                  SpecFile::getAtlasSurfaceDirectoryFileExtension(),
                  true,
                  FILE_FORMAT_ASCII,
                  FILE_IO_READ_ONLY,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_NONE)
{
   clear();
}

/**
 * Destructor.
 */
AtlasSurfaceDirectoryFile::~AtlasSurfaceDirectoryFile()
{
   clear();
}

/**
 * Clear the file.
 */
void
AtlasSurfaceDirectoryFile::clear()
{
   clearAbstractFile();
   atlasSurfaces.clear();
   fileVersion = 0;
}

/**
 * Add an atlas surface.
 */
void
AtlasSurfaceDirectoryFile::addAtlasSurface(const AtlasSurface& as)
{
   atlasSurfaces.push_back(as);
}

/**
 * Get an atlas surface.
 */
AtlasSurface* 
AtlasSurfaceDirectoryFile::getAtlasSurface(const int index)
{
   return &atlasSurfaces[index];
}

/**
 * Get the number of atlas surfaces.
 */
int 
AtlasSurfaceDirectoryFile::getNumberOfAtlasSurfaces() const
{
   return (int)atlasSurfaces.size();
}

/**
 * Set the file location path.
 */
void 
AtlasSurfaceDirectoryFile::setFileLocationPathName(const QString& path)
{
   fileLocationPathName = path;
}

/**
 * Read the file's data.
 */
void
AtlasSurfaceDirectoryFile::readFileData(QFile& /*file*/, QTextStream& stream, QDataStream&,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   bool readingData = false;
   while (stream.atEnd() == false) {
      QString line;
      readLineChopComment(stream, line);
      if (line.isEmpty() == false) {
         if (readingData) {
            std::vector<QString> items;
            StringUtilities::token(line, "|", items);
            
            if (items.size() == 6) {
               const QString atlasName(items[0]);
               QString filename(items[3]);
               const QString descriptiveName(items[4]);
               const QString anatomyTypeString(items[1]);
               const QString fileTypeString(items[2]);
               const QString specFileName(items[5]);
               bool errorFlag = false;
               
               AtlasSurface::ATLAS_SURFACE_ANATOMY_TYPE anatomyType = 
                  AtlasSurface::ATLAS_SURFACE_TYPE_NONE;
               if (anatomyTypeString == AtlasSurface::anatomyCerebralLeft) {
                  anatomyType = AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT;
               }
               else if (anatomyTypeString == AtlasSurface::anatomyCerebralRight) {
                  anatomyType = AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT;
               }
               else if (anatomyTypeString == AtlasSurface::anatomyCerebellum) {
                  anatomyType = AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM;
               }
               else {
                  QString msg("AtlasSurfaceDirectoryFile: Unknown anatomy type: ");
                  msg.append(anatomyTypeString);
                  throw FileException(FileUtilities::basename(filename), msg);
               }
               
               AtlasSurface::ATLAS_SURFACE_FILE_TYPE fileType = 
                              AtlasSurface::ATLAS_SURFACE_FILE_TYPE_NONE;
               if (fileTypeString.compare(AtlasSurface::fileTypeVTK) == 0) {
                  fileType = AtlasSurface::ATLAS_SURFACE_FILE_TYPE_VTK_POLYDATA;
               }
               else {
                  QString msg("AtlasSurfaceDirectoryFile: Unknown file type: ");
                  msg.append(fileTypeString);
                  throw FileException(FileUtilities::basename(filename), msg);
                  errorFlag = true;
               }
               
               //
               // add directory to files that are relative
               //
               if (fileLocationPathName.length() > 0) {
                  if (filename.length() > 0) {
                     if (filename[0] != '/') {
                        QString temp(fileLocationPathName);
                        temp.append("/");
                        temp.append(filename);
                        filename = temp;
                     }
                  }
               }
               
               if (errorFlag == false) {
                  AtlasSurface as(atlasName,
                                  filename,
                                  descriptiveName,
                                  specFileName,
                                  anatomyType,
                                  fileType);
                  addAtlasSurface(as);
               }
            }
            else {
               std::cout << "AtlasSurfaceDirectoryFile: bad data line " << line.toAscii().constData() << std::endl;
            }
         }
         else {
            QString tag;
            QTextStream istr(&line, QIODevice::ReadOnly);
            istr >> tag
                 >> fileVersion;
            readingData = true;
         }
      }
   }
}

/**
 * Write the file's data.
 */
void
AtlasSurfaceDirectoryFile::writeFileData(QTextStream& /*stream*/, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException(filename, "Writing AtlasSurfaceDirectoryFile files not supported.");
}

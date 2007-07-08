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


#ifndef __VE_ATLAS_SURFACE_DIRECTORY_FILE_H__
#define __VE_ATLAS_SURFACE_DIRECTORY_FILE_H__

#include "AbstractFile.h"

/// This class maintains an atlas surface for used by the fMRI mapper
class AtlasSurface {
   public:
      /// Type of Anatomy
      enum ATLAS_SURFACE_ANATOMY_TYPE {
         ATLAS_SURFACE_TYPE_NONE,
         ATLAS_SURFACE_TYPE_CEREBRAL_LEFT,
         ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT,
         ATLAS_SURFACE_TYPE_CEREBELLUM
      };
      
      /// Type of surface file
      enum ATLAS_SURFACE_FILE_TYPE {
         ATLAS_SURFACE_FILE_TYPE_NONE,
         ATLAS_SURFACE_FILE_TYPE_VTK_POLYDATA
      };
      
   private:
      /// name of the atlas
      QString atlasName;
      
      /// a descriptive name of the atlas
      QString descriptiveName;
      
      /// type of anatomy
      ATLAS_SURFACE_ANATOMY_TYPE anatomyType;
      
      /// name of file
      QString fileName;
      
      /// type of file
      ATLAS_SURFACE_FILE_TYPE filetype;
      
      /// name of spec file
      QString specFileName;
      
      static const QString anatomyCerebralLeft;
      static const QString anatomyCerebralRight;
      static const QString anatomyCerebellum;
      static const QString fileTypeVTK;

   public:
      /// Constructor
      AtlasSurface(const QString& atlasNameIn,
                   const QString& fileNameIn,
                   const QString& descriptiveNameIn,
                   const QString& specFileNameIn,
                   const ATLAS_SURFACE_ANATOMY_TYPE anatomyTypeIn,
                   const ATLAS_SURFACE_FILE_TYPE fileTypeIn);
                   
      /// Destructor
      ~AtlasSurface(); 
      
      /// Get the name of the atlas
      QString getAtlasName() const { return atlasName; }
      
      /// Get a descriptive name of the atlas
      QString getDescriptiveName() const { return descriptiveName; }
      
      /// Get the name of the file
      QString getFileName() const { return fileName; }
      
      /// Get the name of the spec file
      QString getSpecFileName() const { return specFileName; }
      
      /// Get the type of anatomy
      ATLAS_SURFACE_ANATOMY_TYPE getAnatomyType() const { return anatomyType; }
      
      /// Get all of the data attributes
      void getData(QString& atlasNameOut,
                   QString& fileNameOut,
                   QString& descriptiveNameOut,
                   QString& specFileNameOut,
                   ATLAS_SURFACE_ANATOMY_TYPE& anatomyTypeOut,
                   ATLAS_SURFACE_FILE_TYPE& fileTypeOut) const;
                   
      /// Set all of the data attributes
      void setData(const QString& atlasNameIn,
                   const QString& fileNameIn,
                   const QString& descriptiveNameIn,
                   const QString& specFileNameIn,
                   const ATLAS_SURFACE_ANATOMY_TYPE anatomyTypeIn,
                   const ATLAS_SURFACE_FILE_TYPE fileTypeIn);
   
   friend class AtlasSurfaceDirectoryFile;
};

/// This class reads an atlas directory file
class AtlasSurfaceDirectoryFile : public AbstractFile {
   private:
      /// Storage for atlas surfaces
      std::vector<AtlasSurface> atlasSurfaces;
      
      /// Pathname to the atlases
      QString fileLocationPathName;
      
      /// version of the file
      int fileVersion;
      
      static const QString tagFileVersion;

      /// write the file's data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// read the file's data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                        QDomElement& /* rootElement */) throw (FileException);
      
   public:
      
      /// Constructors
      AtlasSurfaceDirectoryFile();
      
      /// Destructor
      ~AtlasSurfaceDirectoryFile();
      
      /// Add an atlas surface to the file
      void addAtlasSurface(const AtlasSurface& as);
      
      /// Clear the file
      void clear();
      
      /// is the file isEmpty
      bool empty() const { return (getNumberOfAtlasSurfaces() == 0); }

      /// get the file's version
      int getFileVersion() const { return fileVersion; }
      
      /// Get an atlas surface
      AtlasSurface* getAtlasSurface(const int index);
      
      /// Get the number of atlases
      int getNumberOfAtlasSurfaces() const;
      
      /// Set the pathname of where the data files are located.  This
      /// needs to be called before "readFile".
      void setFileLocationPathName(const QString& path);
};

#ifdef VE_ATLAS_SURFACE_DIRECTORY_FILE_DEFINE

const QString AtlasSurface::anatomyCerebralLeft  = "Cerebral-Left";
const QString AtlasSurface::anatomyCerebralRight = "Cerebral-Right";
const QString AtlasSurface::anatomyCerebellum    = "Cerebellum";
const QString AtlasSurface::fileTypeVTK          = "VTK";
   
const QString AtlasSurfaceDirectoryFile::tagFileVersion =  "tag-file-version";

#endif // VE_ATLAS_SURFACE_DIRECTORY_FILE_DEFINE

#endif // __VE_ATLAS_SURFACE_DIRECTORY_FILE_H__

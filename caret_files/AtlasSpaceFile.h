#ifndef __ATLAS_SPACE_FILE__
#define __ATLAS_SPACE_FILE__ 

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

class QDomElement;
class QDomNode;

#include "AbstractFile.h"

/// Class for storing information about an atlas surface
class AtlasSpaceSurface {
   public:
      /// type of data files
      enum ATLAS_FILE_FORMAT {
         ATLAS_FILE_FORMAT_CARET,
         ATLAS_FILE_FORMAT_VTK
      };
      
      /// Constructor for caret format
      AtlasSpaceSurface(const QString& directoryPathIn,
                   const QString& descriptionIn,
                   const QString& speciesIn,
                   const QString& spaceIn,
                   const QString& hemisphereIn,
                   const QString& topologyFileNameIn,
                   const QString& coordinateFileNameIn);
                   
      /// Constructor for vtk format
      AtlasSpaceSurface(const QString& directoryPathIn,
                   const QString& descriptionIn,
                   const QString& speciesIn,
                   const QString& spaceIn,
                   const QString& hemisphereIn,
                   const QString& vtkFileNameIn);
        
      /// Destructor for
      ~AtlasSpaceSurface();
      
      /// get the atlas file format
      ATLAS_FILE_FORMAT getAtlasFileFormat() const { return atlasFileFormat; }
      
      /// get the description
      QString getDescription() const { return description; }
      
      /// get the species
      QString getSpecies() const { return species; }
      
      /// get the space
      QString getSpace() const { return space; }
      
      /// get the structure
      QString getStructure() const { return structure; }
      
      /// get the topology file name
      QString getTopologyFileName() const { return topologyFileName; }
      
      /// get the coordinate file name
      QString getCoordinateFileName() const { return coordinateFileName; }
      
      /// get the vtk file name
      QString getVtkFileName() const { return vtkFileName; }
      
      /// get the directory path
      QString getDirectoryPath() const { return directoryPath; }
      
      /// less than operator for sorting
      bool operator<(const AtlasSpaceSurface& asf) const;
      
   protected:
      /// type of surface file
      ATLAS_FILE_FORMAT atlasFileFormat;
      
      /// directory of files
      QString directoryPath;
      
      /// description
      QString description;
      
      /// species
      QString species;
      
      /// stereotaxic space
      QString space;
      
      /// structure
      QString structure;
      
      /// topology file (if ATLAS_FILE_FORMAT_CARET)
      QString topologyFileName;
      
      /// coordinate file (if ATLAS_FILE_FORMAT_CARET)
      QString coordinateFileName;
      
      /// vtk polydata file (if ATLAS_FILE_FORMAT_VTK)
      QString vtkFileName;
      
};


/// class for file containing atlas surfaces
class AtlasSpaceFile : public AbstractFile {
   public:
   
      /// constructor
      AtlasSpaceFile();
      
      /// destructor
      ~AtlasSpaceFile();
      
      /// append an atlas surface file to this one
      void append(AtlasSpaceFile& ccf) throw (FileException);
      
      /// clear the atlas surface file
      void clear();
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return (getNumberOfAtlasSurfaces() == 0); }
      
      /// add an atlas surface
      void addAtlasSurface(const AtlasSpaceSurface& as) { atlasSurfaces.push_back(as); }
      
      /// get the number of atlas surfaces
      int getNumberOfAtlasSurfaces() const { return atlasSurfaces.size(); }
         
      /// get an atlas surface  by index
      AtlasSpaceSurface* getAtlasSurface(const int i) { return &atlasSurfaces[i]; }
         
      /// get file version
      int getFileVersion() const { return fileVersion; }
      
      /// sort the files
      void sort();
      
   protected:
      /// version of the file
      int fileVersion;
      
      /// name of the root element
      //QString rootElementName;
      
      /// the atlas surface
      std::vector<AtlasSpaceSurface> atlasSurfaces;
      
      /// Process the file version.
      void processFileVersion(QDomElement& elem) throw (FileException);

      /// Process an atlas surface.
      void processAtlasSurface(QDomElement& elem)  throw (FileException);

      /// read the file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                        QDomElement& /* rootElement */) throw (FileException);
      
      /// write the file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                         QDomElement& /* rootElement */) throw (FileException);
      
};

#endif // __ATLAS_SPACE_FILE__

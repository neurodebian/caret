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


#ifndef __CELL_PROJECTION_FILE_H__
#define __CELL_PROJECTION_FILE_H__

#include "AbstractFile.h"
#include "CellBase.h"
#include "CellClass.h"
#include "CellStudyInfo.h"
#include "SpecFile.h"

class CellFile;
class CellProjectionFile;
class ColorFile;
class CoordinateFile;
class StudyMetaDataFile;
class TopologyFile;

/// Storage for a cell projection.
class CellProjection : public CellBase {
   public:
      /// cell projection type
      enum PROJECTION_TYPE {
         PROJECTION_TYPE_UNKNOWN,
         PROJECTION_TYPE_INSIDE_TRIANGLE,
         PROJECTION_TYPE_OUTSIDE_TRIANGLE
      };
      
      /// Constructor
      CellProjection();
      
      /// Destructor
      ~CellProjection();
      
      // initialize the cell projection
      void initialize();
      
      /// Get the projected position of this cell (returns true if valid projection)
      bool getProjectedPosition(const CoordinateFile* cf,
                                const TopologyFile* tf,
                                const bool fiducialSurfaceFlag,
                                const bool flatSurfaceFlag,
                                const bool pasteOntoSurfaceFlag,
                                float xyzOut[3]) const;

      /// called to read from an XML structure
      virtual void readXML(QDomNode& node) throw (FileException);
      
      /// called to write to an XML structure
      virtual void writeXML(QDomDocument& xmlDoc,
                            QDomElement&  parentElement,
                            const int cellNumber);
                    
      /// Set the cell's class name
      void setClassName(const QString& name);
      
      /// set the cell's name
      void setName(const QString& name);
      
      /// Get the index of the unique name
      int getUniqueNameIndex() const { return uniqueNameIndex; }
      
   protected:
      /// called when data modified
      void setModified();
      
      /// compute a projection point
      void  computeProjectionPoint(float projection[3]) const;
              
      /// Unproject an inside triangle projection
      bool unprojectInsideTriangle(const CoordinateFile& cf,
                                   const TopologyFile& tf,
                                   const bool pasteOntoSurfaceFlag,
                                   float xyzOut[3]) const;
      
      /// Unproject an outside triangle projection
      bool unprojectOutsideTriangle(const CoordinateFile& cf,
                                    const TopologyFile& tf,
                                    const bool pasteOntoSurfaceFlag,
                                    float xyzOut[3]) const;
   
      /// Add a cell class
      //int addCellClass(const QString& className);
      
      /// The cell classes
      //std::vector<CellClass> cellClasses;
      
      static const QString tagInsideTriangle;
      static const QString tagOutsideTriangle;
      static const QString tagUnknownTriangle;
      
      /// set when reading a cell projection file for throwing exceptions
      static QString filename;
      
      /// the unique name index
      int uniqueNameIndex;
      
      /// cell projection type
      PROJECTION_TYPE projectionType;
      
      /// INSIDE TRIANGLE PROJECTION vertices
      int closestTileVertices[3];
      
      /// INSIDE TRIANGLE PROJECTION tile areas
      float closestTileAreas[3];
      
      /// INSIDE TRIANGLE PROJECTION distances
      float cdistance[3];
      
      /// OUTSIDE TRIANGLE DR
      float dR;
      
      /// OUTSIDE TRIANGLE  fiducial coords
      float triFiducial[2][3][3];
      
      /// OUTSIDE TRIANGLE theta
      float thetaR;
      
      /// OUTSIDE TRIANGLE phi
      float phiR;
      
      /// OUTSIDE TRIANGLE triangle vertices
      int   triVertices[2][3];
      
      /// OUTSIDE TRIANGLE vertices
      int   vertex[2];
      
      /// OUTSIDE TRIANGLE fiducial vertices
      float vertexFiducial[2][3];
      
      /// OUTSIDE TRIANGLE fiducial position
      float posFiducial[3];  // cell's Fiducial surface position
      
      /// OUTSIDE TRIANGLE fracRI
      float fracRI;
      
      /// OUTSIDE TRIANGLE fracRJ
      float fracRJ;
      
      /// cell projection file which the projection is a member of
      CellProjectionFile* cellProjectionFile;
      
      /// Read this cell projection version 1 file
      void readFileDataVersion1(QTextStream& stream) throw (FileException);
      
      /// Read this cell projection version 2 file
      void readFileDataVersion2(QTextStream& stream) throw (FileException);
      
      /// Write this cell projection
      void writeFileData(QTextStream& stream, const int cellNumber) throw (FileException);
      
   friend class CellFileProjector; 
   friend class CellProjectionFile;
   friend class CellProjectionUnprojector;
   friend class FociFileToPalsProjector;
};

/// class for storing cell projections
class CellProjectionFile : public AbstractFile {
   private:   
      static const QString tagFileVersion;
      static const QString tagNumberOfCellProjections;
      static const QString tagNumberOfComments;
      static const QString tagCommentUrl;
      static const QString tagCommentKeyWords;
      static const QString tagCommentTitle;
      static const QString tagCommentAuthors;
      static const QString tagCommentCitation;
      static const QString tagCommentStereotaxicSpace;

      /// The cell projections
      std::vector<CellProjection> cellProjections;
      
      /// Cell projection study info
      std::vector<CellStudyInfo> studyInfo;
      
      /// The cell classes
      std::vector<CellClass> cellClasses;
      
      /// The cell unique names
      std::vector<CellClass> cellUniqueNames;
      
      /// read the version number only flag
      bool readVersionNumberOnly;
      
      /// version number of this file
      int versionNumber;
      
      /// read file version 1 data
      void readFileVersion1(QTextStream&, const int numProjections,
                           const int numStudyInfo) throw (FileException);
      
      /// read file version 2 data
      void readFileVersion2(QTextStream&, const int numProjections,
                           const int numStudyInfo) throw (FileException);
      
      /// read file version 3 data
      void readFileVersion3(QFile& file, QTextStream&, 
                            const int numProjections) throw (FileException);
      
      // read file's data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      // write version 2 file's data
      void writeFileVersion2(QTextStream& stream) throw (FileException);
      
      // write version 3 file's data
      void writeFileVersion3(QTextStream& stream) throw (FileException);
      
      // write file's data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
   public:
      /// Constructor
      CellProjectionFile(const QString& descriptiveName = "Cell Projection File",
                const QString& defaultExtensionIn = SpecFile::getCellProjectionFileExtension());
      
      /// Destructor
      virtual ~CellProjectionFile();
      
      /// add a cell projection
      void addCellProjection(const CellProjection& cp);
      
      /// Add a cell class
      int addCellClass(const QString& className);
      
      /// Add a cell unique name
      int addCellUniqueName(const QString& uniqueName);
      
      /// append a cell projection file to this cell projection file
      void append(const CellProjectionFile& cpf);
      
      /// append a cell file
      void append(const CellFile& cf);
      
      /// assign colors to these cell projections
      void assignColors(ColorFile& cf, const CellBase::CELL_COLOR_MODE colorMode);

      /// apply a transformation matrix to the cells
      //void applyTransformationMatrix(const int sectionLow, 
      //                               const int sectionHigh,
      //                               const float matrix[16],
      //                               const bool onlySpecialCells);

      /// apply a transformation matrix to the cells
      void applyTransformationMatrix(const CoordinateFile* cf,
                                     const TopologyFile* tf,
                                     const bool fiducialSurfaceFlag,
                                     const int sectionLow, 
                                     const int sectionHigh,
                                     const TransformationMatrix& matrix,
                                     const bool onlySpecialCells);

      /// apply a transformation matrix to the cells
      //void applyTransformationMatrix(const TransformationMatrix& matrix,
      //                               const bool onlySpecialCells = false);

      /// clear the file in memory
      void clear();
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return (getNumberOfCellProjections() == 0); }
      
      /// get the version number of this file
      int getVersionNumber() const { return versionNumber; }
      
      /// get number of cell classes
      int getNumberOfCellClasses() const { return cellClasses.size(); }
      
      /// get cell class index by its name
      int getCellClassIndexByName(const QString& name) const;

      /// get cell class selected by index
      bool getCellClassSelectedByIndex(const int index) const;
      
      /// set cell class selected by index
      void setCellClassSelectedByIndex(const int index, const bool sel);
      
      /// get cell class selected by name
      bool getCellClassSelectedByName(const QString& name) const;
      
      /// get cell class name
      QString getCellClassNameByIndex(const int index) const;
      
      /// set the status of all cell classes
      void setAllCellClassStatus(const bool selected);
      
      /// get number of cell unique names
      int getNumberOfCellUniqueNames() const { return cellUniqueNames.size(); }
      
      /// get cell unique name index by its name
      int getCellUniqueNameIndexByName(const QString& name) const;

      /// get cell unique name selected by index
      bool getCellUniqueNameSelectedByIndex(const int index) const;
      
      /// set cell unique name selected by index
      void setCellUniqueNameSelectedByIndex(const int index, const bool sel);
      
      /// get cell unique name selected by name
      bool getCellUniqueNameSelectedByName(const QString& name) const;
      
      /// get cell unique name name
      QString getCellUniqueNameByIndex(const int index) const;
      
      /// set the status of all cell unique names
      void setAllCellUniqueNameStatus(const bool selected);
      
      /// get a cell projection
      CellProjection* getCellProjection(const int i) {
         return &cellProjections[i];
      }
      
      /// get a cell projection (const method)
      const CellProjection* getCellProjection(const int i) const {
         return &cellProjections[i];
      }
      
      /// get number of cell projections
      int getNumberOfCellProjections() const { return cellProjections.size(); }
      
      /// get number of study info
      int getNumberOfStudyInfo() const { return studyInfo.size(); }
      
      /// get a cell file containing the origin (unprojected) coordinates
      void getCellFileOriginalCoordinates(CellFile& cellFileOut) const;
      
      /// get a cell file projecting to supplied coord/topo
      void getCellFile(const CoordinateFile* cf,
                       const TopologyFile* tf,
                       const bool fiducialSurfaceFlag,
                       CellFile& cellFileOut) const;
                       
      /// get a cell file projecting to supplied coord/topo for left and right hemispheres
      void getCellFileForRightLeftFiducials(const CoordinateFile* leftCF,
                                            const TopologyFile* leftTF,
                                            const CoordinateFile* rightCF,
                                            const TopologyFile* rightTF,
                                            CellFile& cellFileOut) const;
                       
      /// delete cell projection at specified index
      void deleteCellProjection(const int index);
      
      /// delete all cell projections whose display flag is false
      void deleteAllNonDisplayedCellProjections(const Structure& keepThisStructureOnly);
      
      /// assign class to all displayed foci
      void assignClassToDisplayedFoci(const QString& className);
      
      /// get a pointer to the study info
      std::vector<CellStudyInfo>* getPointerToStudyInfo() { return &studyInfo; }

      /// get the study info index from the study info's value
      int getStudyInfoFromValue(const CellStudyInfo& studyInfo) const;
      
      /// get a study info (const method)
      const CellStudyInfo* getStudyInfo(const int index) const;
      
      /// get a study info
      CellStudyInfo* getStudyInfo(const int index);
      
      /// add a study info
      int addStudyInfo(const CellStudyInfo& studyInfo);
      
      /// delete all study info and clear links to study info
      void deleteAllStudyInfo();
      
      /// delete study info
      void deleteStudyInfo(const int indx);
      
      /// set a study info
      void setStudyInfo(const int index, const CellStudyInfo& csi);
            
      /// Get the version number of the cell projection file
      static int readFilesVersionNumber(const QString& filename);

      /// set the special flag for all cells within the section numbers and bounds
      void setSpecialFlags(const int sectionLow,
                           const int sectionHigh,
                           const float bounds[4]);
                           
      /// clear all special flags
      void clearAllSpecialFlags();    
        
      /// clear all highlight flags
      void clearAllHighlightFlags();
      
      /// update cell PubMed ID if cell name matches study name
      void updatePubMedIDIfCellNameMatchesStudyName(const StudyMetaDataFile* smdf);
      
      /// find out if comma separated file conversion supported
      virtual void getCommaSeparatedFileSupport(bool& readFromCSV,
                                                bool& writeToCSV) const;
                                        
      /// write the file's data into a comma separated values file (throws exception if not supported)
      virtual void writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException);
      
      /// read the file's data from a comma separated values file (throws exception if not supported)
      virtual void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException);
      
};

#ifdef CELL_PROJECTION_MAIN

   const QString CellProjectionFile::tagFileVersion = "tag-version";
   const QString CellProjectionFile::tagNumberOfCellProjections = 
                                         "tag-number-of-cell-projections";
   const QString CellProjectionFile::tagNumberOfComments = "tag-number-of-comments";
   const QString CellProjectionFile::tagCommentUrl = "tag-url";
   const QString CellProjectionFile::tagCommentKeyWords = "tag-key-words";
   const QString CellProjectionFile::tagCommentTitle = "tag-title";
   const QString CellProjectionFile::tagCommentAuthors = "tag-authors";
   const QString CellProjectionFile::tagCommentCitation = "tag-citation";
   const QString CellProjectionFile::tagCommentStereotaxicSpace = "tag-space";
   
   const QString CellProjection::tagInsideTriangle = "INSIDE";
   const QString CellProjection::tagOutsideTriangle = "OUTSIDE";
   const QString CellProjection::tagUnknownTriangle = "UNKNOWN";

   QString CellProjection::filename = "";

#endif // CELL_PROJECTION_MAIN

#endif // __CELL_PROJECTION_FILE_H__

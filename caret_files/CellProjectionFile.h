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
class Structure;
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
      CellProjection(const QString& fileNameIn = "");
      
      /// Constructor for placing cell projection at a node (no need to project)
      CellProjection(const QString& nameIn,
                     const CoordinateFile* fiducialCoordinateFile,
                     const int nodeNumber,
                     const Structure& structureIn);
                                      
      /// Destructor
      ~CellProjection();

      // initialize the cell projection
      void initialize(const QString& fileNameIn = "");

      /// Get the projected position of this cell (returns true if valid projection)
      bool getProjectedPosition(const CoordinateFile* cf,
                                const TopologyFile* tf,
                                const bool fiducialSurfaceFlag,
                                const bool flatSurfaceFlag,
                                const bool pasteOntoSurfaceFlag,
                                float xyzOut[3]) const;

      /// get the volume position
      void getVolumeXYZ(float xyzOut[3]) const;
      
      /// set the volume position
      void setVolumeXYZ(const float xyzIn[3]);
      
      /// called to read from an XML DOM structure
      virtual void readXMLWithDOM(QDomNode& node) throw (FileException);
      
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
      
      /// set element from text (used by SAX XML parser)
      void setElementFromText(const QString& elementName,
                              const QString& textValue);
        
      /// get duplicate cell projection flag
      bool getDuplicateFlag() const { return duplicateFlag; }
      
      /// set the duplicate cell projection flag
      void setDuplicateFlag(const bool b);
      
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
      
      /// projection is inside triangle
      static const QString tagInsideTriangle;
      
      /// projection is outside triangle
      static const QString tagOutsideTriangle;
      
      /// projection is unknown
      static const QString tagUnknownTriangle;
      
      /// set when reading a cell projection file for throwing exceptions
      QString filename;
      
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
      
      /// position of focus in a volume
      float volumeXYZ[3];
      
      /// duplicate flag (used when near both cerebral and cerebellar)
      bool duplicateFlag;
      
      /// cell projection file which the projection is a member of
      CellProjectionFile* cellProjectionFile;
      
      /// Read this cell projection version 1 file
      void readFileDataVersion1(QTextStream& stream) throw (FileException);
      
      /// Read this cell projection version 2 file
      void readFileDataVersion2(QTextStream& stream) throw (FileException);
      
      /// Write this cell projection
      void writeFileData(QTextStream& stream, const int cellNumber) throw (FileException);
      
      /// tag for reading and writing cells
      static const QString tagCellProjection;
      
      /// tag for reading and writing cells
      static const QString tagCellProjNumber;
      
      /// tag for reading and writing cells
      static const QString tagProjectionType;
      
      /// tag for reading and writing cells
      static const QString tagClosestTileVertices;
      
      /// tag for reading and writing cells
      static const QString tagClosestTileAreas;
      
      /// tag for reading and writing cells
      static const QString tagCDistance;
      
      /// tag for reading and writing cells
      static const QString tagDR;
      
      /// tag for reading and writing cells
      static const QString tagTriFiducial;
      
      /// tag for reading and writing cells
      static const QString tagThetaR;
      
      /// tag for reading and writing cells
      static const QString tagPhiR;
      
      /// tag for reading and writing cells
      static const QString tagTriVertices;
      
      /// tag for reading and writing cells
      static const QString tagVertex;
      
      /// tag for reading and writing cells
      static const QString tagVertexFiducial;
      
      /// tag for reading and writing cells
      static const QString tagPosFiducial;
      
      /// tag for reading and writing cells
      static const QString tagFracRI;
      
      /// tag for reading and writing cells
      static const QString tagFracRJ;
      
      /// tag for reading and writing cells
      static const QString tagVolumeXYZ;
      
      /// tag for reading and writing cells
      static const QString tagDuplicateFlag;
      
   friend class CellFileProjector; 
   friend class CellProjectionFile;
   friend class CellProjectionUnprojector;
   friend class FociFileToPalsProjector;
   friend class FociProjectionFile;
};

#ifdef __CELL_PROJECTION_MAIN__
      const QString CellProjection::tagCellProjection = "CellProjection";
      const QString CellProjection::tagCellProjNumber = "cellProjNumber";
      const QString CellProjection::tagProjectionType = "projectionType";
      const QString CellProjection::tagClosestTileVertices = "closestTileVertices";
      const QString CellProjection::tagClosestTileAreas = "closestTileAreas";
      const QString CellProjection::tagCDistance = "cdistance";
      const QString CellProjection::tagDR = "dR";
      const QString CellProjection::tagTriFiducial = "triFiducial";
      const QString CellProjection::tagThetaR = "thetaR";
      const QString CellProjection::tagPhiR = "phiR";
      const QString CellProjection::tagTriVertices = "triVertices";
      const QString CellProjection::tagVertex = "vertex";
      const QString CellProjection::tagVertexFiducial = "vertexFiducial";
      const QString CellProjection::tagPosFiducial = "posFiducial";
      const QString CellProjection::tagFracRI = "fracRI";
      const QString CellProjection::tagFracRJ = "fracRJ";
      const QString CellProjection::tagVolumeXYZ = "volumeXYZ";
      const QString CellProjection::tagDuplicateFlag = "duplicateFlag";

      const QString CellProjection::tagInsideTriangle = "INSIDE";
      const QString CellProjection::tagOutsideTriangle = "OUTSIDE";
      const QString CellProjection::tagUnknownTriangle = "UNKNOWN";
#endif // __CELL_PROJECTION_MAIN__

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
      
      // read the file with an XML stream reader
      void readFileWithXmlStreamReader(QFile& file) throw (FileException);
                                 
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
      
      /// append a fiducial cell file
      void appendFiducialCellFile(const CellFile& cf);
      
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
      
      /// get cell class indices sorted by name case insensitive
      void getCellClassIndicesSortedByName(std::vector<int>& indicesSortedByNameOut,
                                           const bool reverseOrderFlag,
                                           const bool limitToDisplayedCellsFlag) const;
      
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
      
      /// transfer table subheader short name to cell classes
      void transferTableSubHeaderShortNameToCellClass(const StudyMetaDataLink& smdl,
                                                      const QString& shortName);
      
      /// set the search status of all cells
      void setAllSearchStatus(const bool inSearchFlag);
      
      /// get cell unique name indices sorted by name case insensitive
      void getCellUniqueNameIndicesSortedByName(std::vector<int>& indicesSortedByNameOut,
                                          const bool reverseOrderFlag,
                                          const bool limitToDisplayedCellsFlag) const;
      
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
      
      /// get indices of displayed cell projections
      void getIndicesOfDisplayedCells(std::vector<int>& indicesOut) const;
      
      /// get a cell projection
      CellProjection* getCellProjection(const int i) {
         return &cellProjections[i];
      }
      
      /// get a cell projection (const method)
      const CellProjection* getCellProjection(const int i) const {
         return &cellProjections[i];
      }
      
      /// get first cell projection with specified name
      CellProjection* getFirstCellProjectionWithName(const QString& name);
      
      /// get last cell projection with specified name
      CellProjection* getLastCellProjectionWithName(const QString& name);
      
      /// get first cell projection with specified name (const method)
      const CellProjection* getFirstCellProjectionWithName(const QString& name) const;
      
      /// get last cell projection with specified name (const method)
      const CellProjection* getLastCellProjectionWithName(const QString& name) const;
      
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
                                            const CoordinateFile* cerebellumCF,
                                            const TopologyFile* cerebellumTF,
                                            CellFile& cellFileOut) const;
      
      // get all cell areas
      void getAllCellAreas(std::vector<QString>& allAreasOut) const;
      
      // get all comments
      void getAllCellComments(std::vector<QString>& allCommentsOut) const;
      
      // get all cell geography
      void getAllCellGeography(std::vector<QString>& allGeographyOut) const;
      
      /// delete cell projection at specified index
      void deleteCellProjection(const int index);
      
      // get all cell regions of interest
      void getAllCellRegionsOfInterest(std::vector<QString>& allRegionsOfInterestOut) const;
      
      /// delete all duplicate cell projections
      void deleteAllDuplicateCellProjections();
      
      /// delete cell projections with name
      void deleteCellProjectionsWithName(const QString& name);
      
      /// delete cell projections whose indices are not specified
      void deleteAllButTheseCellProjections(const int* indices,
                                            const int numIndices);
                                            
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
      
      /// get PubMedID's of all linked studies
      void getPubMedIDsOfAllLinkedStudyMetaData(std::vector<QString>& studyPMIDs,
                                                const bool displayedFociOnlyFlag = false) const;
      
      /// update cell PubMed ID if cell name matches study name
      void updatePubMedIDIfCellNameMatchesStudyName(const StudyMetaDataFile* smdf);
      
      /// update cell name with linked study name
      void updateCellNameWithStudyNameForMatchingPubMedIDs(const StudyMetaDataFile* smdf);
      
      /// update cell class if linked to table subheader
      void updateCellClassWithLinkedStudyTableSubheaderShortNames(const StudyMetaDataFile* smdf);
      
      /// update cell class with linked tabel subheader name, linked figure panel task
      /// description, or page reference subheader short name
      void updateCellClassWithLinkedTableFigureOrPageReference(const StudyMetaDataFile* smdf);
      
      /// find out if comma separated file conversion supported
      virtual void getCommaSeparatedFileSupport(bool& readFromCSV,
                                                bool& writeToCSV) const;
                                        
      /// write the file's data into a comma separated values file (throws exception if not supported)
      virtual void writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException);
      
      /// read the file's data from a comma separated values file (throws exception if not supported)
      virtual void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException);
      
};

#endif // __CELL_PROJECTION_FILE_H__

#ifdef __CELL_PROJECTION_FILE_MAIN__
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
#endif // __CELL_PROJECTION_FILE_MAIN__

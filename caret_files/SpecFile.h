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


#ifndef __SPEC_FILE_H__
#define __SPEC_FILE_H__

#include "AbstractFile.h"
#include "SceneFile.h"
#include "Structure.h"

/// SpecFile - This class is used to read, write, and manipulate a specification file.
class SpecFile : public AbstractFile {
   public:
   
      /// True and False for SpecFile.   We would prefer to use "bool" but
      /// std::vector<bool> is treated specially by the compiler to pack the
      /// bools into as little space as possible.  This prevents taking the
      /// address of an element so we have to define our own type.
      /// See page 158 of "The C++ Standard Library" by Josuttis.
      enum SPEC_FILE_BOOL {
         SPEC_FALSE = false,
         SPEC_TRUE = true
      };
      
      /// Sorting methods
      enum SORT {
         SORT_NONE,
         SORT_DATE,
         SORT_NAME
      };
      
      /// brain structure types
      
      /// class for storing info about a file type
      class Entry {
         public:
            // type of file
            enum FILE_TYPE {
               FILE_TYPE_SURFACE,
               FILE_TYPE_VOLUME,
               FILE_TYPE_OTHER
            };
            
            /// the files in the entry
            class Files {
               public:
                  // constructor
                  Files(const QString& filenameIn,
                           const QString& dataFileNameIn,
                           const Structure& structureIn,
                           const SPEC_FILE_BOOL selectionStatus = SPEC_TRUE);
                           
                  // destructor
                  ~Files();
                  
                  // comparison operator
                  bool operator<(const Files& e) const;
            
                  // sort the files
                  static void setSortMethod(const SORT sortMethod);
            
                  /// name of file
                  QString filename;
                  
                  /// name of data file (used by volumes with separate header and data files)
                  QString dataFileName;
                  
                  /// file is selected
                  SPEC_FILE_BOOL selected;
                  
                  /// structure with which file is associated
                  Structure structure;
             
                  /// how to sort
                  static SORT sortMethod;
            };
            

            // constructor
            Entry();
                  
            // destructor
            ~Entry();
            
            // comparison operator
            bool operator<(const Entry& e) const
               { return descriptiveName < e.descriptiveName; }

            // initialize
            void initialize(const QString& descriptiveNameIn,
                            const QString& specFileTagIn,
                            const FILE_TYPE fileTypeIn,
                            const bool specFilePatternMatchFlagIn = false);
                  
            // add if tag matches (returns true if tag matched)
            bool addFile(const QString& tagReadIn,
                         const QString& filenameIn,
                         const QString& dataFileNameIn,
                         const Structure& structureIn);
            
            // clear selection status for a file
            void clearSelectionStatus(const QString& filenameIn);
            
            // deselect files from other spec .
            void deselectFilesOtherSpec(const SpecFile& otherSpecFile);

            // add path to files
            void prependPath(const QString& path,
                             const bool ignoreFilesWithAbsPaths);
                             
            // get all files (including data files) to input vector
            void getAllFiles(std::vector<QString>& allFiles);
            
            // clear all members except type and tag
            void clear(const bool removeFilesFromDiskToo);
            
            /// get number or files
            int getNumberOfFiles() const { return files.size(); }
            
            // get number of files selected
            int getNumberOfFilesSelected() const;
            
            /// get name of file
            QString getFileName(const int indx) const { return files[indx].filename; }
            
            /// get name of data file (used by volumes with separate header and data files)
            QString getDataFileName(const int indx) const { return files[indx].dataFileName; }
            
            /// get file is selected
            SPEC_FILE_BOOL getSelected(const int indx) const { return files[indx].selected; }
            
            /// get structure with which file is associated
            Structure getStructure(const int indx) { return files[indx].structure; }
            
            /// get structure with which file is associated
            const Structure getStructure(const int indx) const { return files[indx].structure; }
            
            // Returns true if all of myFiles are in otherFiles.  
            bool isSubset(const SpecFile& otherSpecFile,
                          QString& errorMessage) const;
                          
            // set selection based upon file name (returns true if selected)
            bool setSelected(const QString& name,
                             const bool addIfNotFound,
                             const Structure& structureIn);
                             
            // make sure all files exist and are readable
            void validate(QString& errorMessage) const;
            
            // clean up this entry (remove entries for files that do not exist)
            bool cleanup();
            
            // set all file selections
            void setAllSelections(const SPEC_FILE_BOOL selStatus);
            
            // remove paths from all files
            void removePaths();
            
            // save scene
            void saveScene(SceneFile::SceneClass& sc,
                           const bool selectedFilesOnlyFlag);
                           
            // sort the files
            void sort(const SORT sortMethod);
            
            // write the files
            void writeFiles(QTextStream& stream,
                            QDomDocument& xmlDoc,
                            QDomElement& rootElement,
                            const FILE_FORMAT fileFormat,
                            const int fileVersionNumber,
                            const bool writeOnlySelectedFiles) throw (FileException);
            
            /// get the descriptive name
            QString getDescriptiveName() const { return descriptiveName; }
            
            /// get the spec file tag
            QString getSpecFileTag() const { return specFileTag; }
            
            /// descriptive name
            QString descriptiveName;
            
            /// type of file
            FILE_TYPE fileType;
            
            /// spec file tag
            QString specFileTag;
            
            /// the files
            std::vector<Files> files;
            
            /// pattern match (this tag is subset of that found in spec file)
            bool specFilePatternMatchFlag;
      };
      
      // Constructor
      SpecFile();

      // Copy constructor
      SpecFile(const SpecFile& sf);
      
      // Destructor
      ~SpecFile();
      
      // assignment operator
      SpecFile& operator=(const SpecFile& sf);
      
      // append a spec file to this one (files only)
      void append(const SpecFile& sf);
            
      // Clear the spec file
      void clear();
      
      /// get the version of the spec file
      int getFileVersion() const { return fileVersion; }
      
      // clean the spec file (remove entries for files that do not exist)
      bool cleanSpecFile();
      
      // remove files from the spec file and possibly the files themselves on the disk
      void clearFiles(const bool clearVolumeFiles, 
                      const bool clearSurfaceFiles, 
                      const bool clearOtherFiles,
                      const bool removeFilesFromDiskToo);
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return getFileName().isEmpty();  }
      
      /// set sorting performed when a spec file is read
      void setSorting(const SORT sort) { sorting = sort; }
      
      // Get the number of selected files
      int getNumberOfSelectedFiles() const;
      
      // see if only scene files are selected
      bool onlySceneFilesSelected() const;
      
      // get all entries
      void getAllEntries(std::vector<Entry>& allEntriesOut);
            
      // convert all data files to the specified data type
      void convertAllDataFilesToType(const std::vector<AbstractFile::FILE_FORMAT> ft,
                                     const bool printInfoToStdout);
      
      // get all of the data files in the spec file
      void getAllDataFilesInSpecFile(std::vector<QString>& allFiles,
                                     const bool includeVolumeDataFiles = true) const;
      
      // validate a spec file - all files exist and are readable (true if valid)
      bool validate(QString& errorMessage) const;
      
      // add a file to the spec file (returns true if file was written)
      bool addToSpecFile(const QString& tag, 
                         const QString& value1,
                         const QString& value2,
                         const bool writeSpecFileIfChanged);
      
      // select or deselect all files
      void setAllFileSelections(const SPEC_FILE_BOOL status); 
      
      // set just fiducial and flat coord/topo selected
      void setDefaultFilesFiducialAndFlat();

      // set the specified files for metric mapping
      // returns an error message (isEmpty if no error)
      QString setFilesForMetricMapping(const QString& topoFileName,
                                           const QString& coordFileName,
                                           const QString& metricFileName,
                                          const Structure structureIn);

      // Set the specified topology and coordinate files as selected.
      void setTopoAndCoordSelected(const QString& topoName,
                                   const QString& coordName,
                                   const Structure structureIn);

      // Set the specified topology and coordinate files as selected.
      void setTopoAndCoordSelected(const QString& topoName,
                                   const std::vector<QString>& coordNames,
                                   const Structure structureIn);

      /// set to write only files that are selected to the spec file
      void setWriteOnlySelectedFiles(const bool selOnly) { writeOnlySelectedFiles = selOnly; }
   
      // get the species
      QString getSpecies() const;
      
      // set the species
      void setSpecies(const QString& speciesIn);
      
      // get the subject
      QString getSubject() const;
      
      // set the subject
      void setSubject(const QString& subjectIn);
      
      // get the space
      QString getSpace() const;
      
      // set the space
      void setSpace(const QString& spaceIn);
      
      // get the structure
      QString getStructure() const;
      
      // set the structure
      void setStructure(const QString& hem);
      
      // get the category
      QString getCategory() const;
      
      // set the category
      void setCategory(const QString& cat);
      
      // set current directory to path of spec file
      void setCurrentDirectoryToSpecFileDirectory();

      // set the files selected for use by deformation
      void setDeformationSelectedFiles(const QString& closedTopoName,
                                       const QString& cutTopoName,
                                       const QString& fiducialCoordName,
                                       const QString& sphericalCoordName,
                                       const QString& flatCoordName,
                                       const QString& borderOrBorderProjName,
                                       const bool addIfNotFound,
                                       const Structure& structureIn);
       
      // set the files selected for multi-resolution morphing
      void setMultiresolutionMorphingFiles(const QString& topoFileName,
                                           const QString& fiducialFileName,
                                           const QString& flatOrSphereFileName,
                                           const Structure& structureIn);
      // Remove paths from all files.
      void removePathsFromAllFiles();
      
      // Prepend paths to a vector of file names 
      void prependPathsToAllFiles(const QString& path,
                                  const bool ignoreFilesWithAbsPaths);


      // sort all of the different files types by name
      void sortAllFilesByName();
      
      // Sort all of the different files types by date
      void sortAllFilesByDate();
      
      // Select files for use by WebCaret.
      void setWebFileSelections();

      // read the spec file tags that are children of the element
      void readTagsFromXML(QDomElement& topElement) throw (FileException);
      
      // write the spec file tags as children of the element
      void writeTagsToXML(QDomDocument& xmlDoc,
                          QDomElement& rootElement) throw (FileException);
         
      // read the spec file from a scene
      void showScene(const SceneFile::Scene& scene, QString& errorMessage);
      
      // write the spec file to a scene
      void saveScene(SceneFile::Scene& scene, const bool selectedFilesOnlyFlag);
      
      // deselect any of "my files" if they are selected in the "other spec"
      void deselectFilesSelectedInOtherSpecFile(SpecFile& otherSpecFile);
      
      // Returns true if "this" spec file is a subset of the "other" spec file.
      // In other words, all of the files listed in this spec file are in 
      // the other spec file.
      bool isSubsetOfOtherSpecFile(const SpecFile& otherSpecFile, QString& errorMessage) const;

      // clear selection status for file name/selection pair
      void clearSelectionStatus(const std::vector<QString>& specNames,
                                       std::vector<SpecFile::SPEC_FILE_BOOL>& selectionStatus,
                                       const QString fileName);
                                       
      static QString getTopoFileExtension() { return ".topo"; }
      static QString getCoordinateFileExtension() { return ".coord"; }
      static QString getTransformationMatrixFileExtension() { return ".matrix"; }
      static QString getLatLonFileExtension() { return ".latlon"; }
      static QString getSectionFileExtension() { return ".section"; }
      static QString getPaintFileExtension() { return ".paint"; }
      static QString getRegionOfInterestFileExtension() { return ".roi"; }
      static QString getProbabilisticAtlasFileExtension() { return ".atlas"; }
      static QString getAreaColorFileExtension() { return ".areacolor"; }
      static QString getRgbPaintFileExtension() { return ".RGB_paint"; }
      static QString getBorderFileExtension() { return ".border"; }
      static QString getBorderColorFileExtension() { return ".bordercolor"; }
      static QString getBorderProjectionFileExtension() { return ".borderproj"; }
      static QString getPaletteFileExtension() { return ".palette"; }
      static QString getTopographyFileExtension() { return ".topography"; }
      static QString getCellFileExtension() { return ".cell"; }
      static QString getCellColorFileExtension() { return ".cell_color"; }
      static QString getCellProjectionFileExtension() { return ".cellproj"; }
      static QString getContourFileExtension() { return ".contours"; }
      static QString getContourCellColorFileExtension() { return ".contour_cell_color"; }
      static QString getContourCellFileExtension() { return ".contour_cells"; }
      static QString getMetricFileExtension() { return ".metric"; }
      static QString getSurfaceShapeFileExtension() { return ".surface_shape"; }
      static QString getCocomacConnectivityFileExtension() { return ".cocomac.xml"; }
      static QString getArealEstimationFileExtension() { return ".areal_estimation"; }
      static QString getCutsFileExtension() { return ".cuts"; }
      static QString getFociFileExtension() { return ".foci"; }
      static QString getFociColorFileExtension() { return ".focicolor"; }
      static QString getFociProjectionFileExtension() { return ".fociproj"; }
      static QString getFociSearchFileExtension() { return ".focisearch"; }
      static QString getParamsFileExtension() { return ".params"; }
      static QString getDeformationMapFileExtension() { return ".deform_map"; }
      static QString getDeformationFieldFileExtension() { return ".deform_field"; }
      static QString getVtkModelFileExtension() { return ".vtk"; }
      static QString getGeodesicDistanceFileExtension() { return ".geodesic"; }
      static QString getAtlasSurfaceDirectoryFileExtension() { return ".directory"; }
      static QString getBrainVoyagerFileExtension() { return ".srf"; }
      static QString getAtlasSpaceFileExtension() { return ".atlas_space.xml"; }
      static QString getFreeSurferAsciiCurvatureFileExtension() { return ".curv.asc"; }
      static QString getFreeSurferBinaryCurvatureFileExtension() { return ".curv"; }
      static QString getFreeSurferAsciiFunctionalFileExtension() { return ".w.asc"; }
      static QString getFreeSurferBinaryFunctionalFileExtension() { return ".w"; }
      static QString getFreeSurferLabelFileExtension() { return ".label"; }
      static QString getFreeSurferAsciiSurfaceFileExtension() { return ".asc"; }
      static QString getFreeSurferBinarySurfaceFileExtension() { return ""; } // no extension
      static QString getSumaRgbFileExtension() { return ".col"; }
      static QString getPreferencesFileExtension() { return "caret5_preferences"; }
      static QString getSpecFileExtension() { return ".spec"; }
      static QString getAnalyzeVolumeFileExtension() { return ".hdr"; }
      static QString getAfniVolumeFileExtension() { return ".HEAD"; }
      static QString getWustlVolumeFileExtension() { return ".ifh"; }
      static QString getMincVolumeFileExtension() { return ".mnc"; }
      static QString getNiftiVolumeFileExtension() { return ".nii"; }
      static QString getNiftiGzipVolumeFileExtension() { return ".nii.gz"; }
      static QString getSceneFileExtension() { return ".scene"; }
      static QString getVectorFileExtension() { return ".vec"; }
      static QString getSurfaceVectorFileExtension() { return ".svec"; }
      static QString getWustlRegionFileExtension() { return ".wustl_txt"; }
      static QString getLimitsFileExtension() { return ".limits"; }
      static QString getMDPlotFileExtension() { return ".mdo"; }
      static QString getGiftiGenericFileExtension() { return ".gii"; }
      static QString getGiftiCoordinateFileExtension() { return ".coord.gii"; }
      static QString getGiftiFunctionalFileExtension() { return ".func.gii"; }
      static QString getGiftiLabelFileExtension() { return ".label.gii"; }
      static QString getGiftiRgbaFileExtension() { return ".rgba.gii"; }
      static QString getGiftiShapeFileExtension() { return ".shape.gii"; }
      static QString getGiftiSurfaceFileExtension() { return ".surf.gii"; }
      static QString getGiftiTensorFileExtension() { return ".tensor.gii"; }
      static QString getGiftiTimeSeriesFileExtension() { return ".time.gii"; }
      static QString getGiftiTopologyFileExtension() { return ".topo.gii"; }
      static QString getCommaSeparatedValueFileExtension() { return ".csv"; }
      static QString getVocabularyFileExtension() { return ".vocabulary"; }
      static QString getStudyMetaDataFileExtension() { return ".study"; }
      static QString getStudyMetaAnalysisFileExtension() { return ".meta_analysis"; }
      static QString getXmlFileExtension() { return ".xml"; }
      static QString getTextFileExtension() { return ".txt"; }
      static QString getNeurolucidaFileExtension() { return ".xml"; }
      static QString getCaretScriptFileExtension() { return ".script"; }
      static QString getMniObjeSurfaceFileExtension() { return ".obj"; }
      
      /// all spec file "Entry" DO NOT CLEAR
      std::vector<Entry*> allEntries;
      
      // volume files have both header and data file
      Entry volumeFunctionalFile;
      
      Entry volumePaintFile;
      
      Entry volumeProbAtlasFile;
      
      Entry volumeRgbFile;
      
      Entry volumeSegmentationFile;
      
      Entry volumeAnatomyFile;
      
      Entry volumeVectorFile;
      
      Entry closedTopoFile;
      Entry openTopoFile;
      Entry cutTopoFile;
      Entry lobarCutTopoFile;
      Entry unknownTopoFile;
      
      Entry rawCoordFile;
      Entry fiducialCoordFile;
      Entry inflatedCoordFile;
      Entry veryInflatedCoordFile;
      Entry sphericalCoordFile;
      Entry ellipsoidCoordFile;
      Entry compressedCoordFile;
      Entry flatCoordFile;
      Entry lobarFlatCoordFile;
      Entry hullCoordFile;
      Entry unknownCoordFile;
      
      Entry rawSurfaceFile;
      Entry fiducialSurfaceFile;
      Entry inflatedSurfaceFile;
      Entry veryInflatedSurfaceFile;
      Entry sphericalSurfaceFile;
      Entry ellipsoidSurfaceFile;
      Entry compressedSurfaceFile;
      Entry flatSurfaceFile;
      Entry lobarFlatSurfaceFile;
      Entry hullSurfaceFile;
      Entry unknownSurfaceFile;
      
      Entry averageFiducialCoordFile;
      
      Entry latLonFile;
      
      Entry sectionFile;
      
      Entry sceneFile;
      
      Entry imageFile;
      
      Entry       transformationMatrixFile;
      
      Entry paintFile;
      Entry areaColorFile;
      Entry rgbPaintFile;
      
      Entry surfaceVectorFile;
      
      Entry rawBorderFile;
      Entry fiducialBorderFile;
      Entry inflatedBorderFile;
      Entry veryInflatedBorderFile;
      Entry sphericalBorderFile;
      Entry ellipsoidBorderFile;
      Entry compressedBorderFile;
      Entry flatBorderFile;
      Entry lobarFlatBorderFile;
      Entry hullBorderFile;
      Entry unknownBorderFile;
      Entry volumeBorderFile;
      
      Entry borderColorFile;
      Entry borderProjectionFile;
      Entry momcTemplateFile;
      Entry momcTemplateMatchFile;
      
      Entry paletteFile;
     
      Entry topographyFile;
      
      Entry cellFile;
      Entry cellColorFile;
      Entry cellProjectionFile;
      Entry volumeCellFile;
      
      Entry contourFile;
      
      Entry contourCellFile;
      
      Entry contourCellColorFile;
      
      Entry atlasFile;
      
      Entry metricFile;
      
      Entry surfaceShapeFile;
      
      Entry cocomacConnectivityFile;
      
      Entry arealEstimationFile;
      
      Entry cutsFile;
      
      Entry fociFile;
      Entry fociColorFile;
      Entry fociProjectionFile;
      Entry fociSearchFile;
      
      Entry paramsFile;
      Entry deformationMapFile;
      Entry deformationFieldFile;
      Entry cerebralHullFile;
      
      Entry    vtkModelFile;
      
      Entry    geodesicDistanceFile;
      
      Entry    wustlRegionFile;
      
      Entry    transformationDataFile;
      
      Entry studyMetaDataFile;
      Entry metaAnalysisFile;
      
      Entry vocabularyFile;
      
      Entry documentFile;
      
      Entry scriptFile;
      
      static const QString volumeFunctionalFileTag;
      static const QString volumePaintFileTag;
      static const QString volumeProbAtlasFileTag;
      static const QString volumeRgbFileTag;
      static const QString volumeSegmentationFileTag;
      static const QString volumeAnatomyFileTag;
      static const QString volumeVectorFileTag;
      
      static const QString closedTopoFileTag;
      static const QString openTopoFileTag;
      static const QString cutTopoFileTag;
      static const QString lobarCutTopoFileTag;
      static const QString unknownTopoFileMatchTag;
      
      static const QString topoFlagTag;
      
      static const QString closedTopoFileTagName;
      static const QString openTopoFileTagName;
      static const QString cutTopoFileTagName;
      static const QString lobarCutTopoFileTagName;
      static const QString unknownTopoFileMatchTagName;
      
      static const QString rawCoordFileTag;
      static const QString fiducialCoordFileTag;
      static const QString inflatedCoordFileTag;
      static const QString veryInflatedCoordFileTag;
      static const QString sphericalCoordFileTag;
      static const QString ellipsoidCoordFileTag;
      static const QString compressedCoordFileTag;
      static const QString flatCoordFileTag;
      static const QString lobarFlatCoordFileTag;
      static const QString hullCoordFileTag;
      static const QString unknownCoordFileMatchTag;
      static const QString averageFiducialCoordFileTag;
      
      static const QString transformationMatrixFileTag;
      static const QString transformationDataFileTag;
      
      static const QString latLonFileTag;
      
      static const QString sectionFileTag;
      
      static const QString rawCoordFileTagName;
      static const QString fiducialCoordFileTagName;
      static const QString inflatedCoordFileTagName;
      static const QString veryInflatedCoordFileTagName;
      static const QString sphericalCoordFileTagName;
      static const QString ellipsoidCoordFileTagName;
      static const QString compressedCoordFileTagName;
      static const QString flatCoordFileTagName;
      static const QString lobarFlatCoordFileTagName;
      static const QString hullCoordFileTagName;
      static const QString unknownCoordFileMatchTagName;
      static const QString averageFiducialCoordFileTagName;
         
      static const QString rawSurfaceFileTag;
      static const QString fiducialSurfaceFileTag;
      static const QString inflatedSurfaceFileTag;
      static const QString veryInflatedSurfaceFileTag;
      static const QString sphericalSurfaceFileTag;
      static const QString ellipsoidSurfaceFileTag;
      static const QString compressedSurfaceFileTag;
      static const QString flatSurfaceFileTag;
      static const QString lobarFlatSurfaceFileTag;
      static const QString hullSurfaceFileTag;
      static const QString unknownSurfaceFileMatchTag;
         
      static const QString paintFileTag;
      static const QString areaColorFileTag;
      static const QString rgbPaintFileTag;
      static const QString surfaceVectorFileTag;
      
      static const QString rawBorderFileTag;
      static const QString fiducialBorderFileTag;
      static const QString inflatedBorderFileTag;
      static const QString veryInflatedBorderFileTag;
      static const QString sphericalBorderFileTag;
      static const QString ellipsoidBorderFileTag;
      static const QString compressedBorderFileTag;
      static const QString flatBorderFileTag;
      static const QString lobarFlatBorderFileTag;
      static const QString hullBorderFileTag;
      static const QString unknownBorderFileMatchTag;
      static const QString volumeBorderFileTag;
      
      static const QString rawBorderFileTagName;
      static const QString fiducialBorderFileTagName;
      static const QString inflatedBorderFileTagName;
      static const QString veryInflatedBorderFileTagName;
      static const QString sphericalBorderFileTagName;
      static const QString ellipsoidBorderFileTagName;
      static const QString compressedBorderFileTagName;
      static const QString flatBorderFileTagName;
      static const QString lobarFlatBorderFileTagName;
      static const QString hullBorderFileTagName;
      static const QString unknownBorderFileMatchTagName;
      static const QString volumeBorderFileTagName;
      
      static const QString borderColorFileTag;
      static const QString borderProjectionFileTag;
      static const QString momcBorderFileTag;
      static const QString momcTemplateFileTag;
      static const QString momcTemplateMatchFileTag;
      
      static const QString paletteFileTag;
      
      static const QString topographyFileTag;
      
      static const QString cellFileTag;      
      static const QString cellColorFileTag;
      static const QString cellProjectionFileTag;
      static const QString volumeCellFileTag;
      
      static const QString contourFileTag;
      static const QString contourCellFileTag;
      static const QString contourCellColorFileTag;
      
      
      static const QString atlasFileTag;
      
      static const QString metricFileTag;
      static const QString surfaceShapeFileTag;
      
      static const QString cocomacConnectivityFileTag;
      
      static const QString arealEstimationFileTag;
      
      static const QString cutsFileTag;
      
      static const QString fociFilterFlagTag;
      static const QString fociFileTag;
      static const QString fociColorFileTag;
      static const QString fociProjectionFileTag;
      static const QString fociSearchFileTag;

      static const QString paramsFileTag;

      static const QString deformationMapFileTag;
             
      static const QString deformationFieldFileTag;
             
      static const QString cerebralHullFileTag;
      
      static const QString vtkModelFileTag;

      static const QString scriptFileTag;
      
      static const QString imageFileTag;
      static const QString sceneFileTag;
      static const QString geodesicDistanceFileTag;
      
      static const QString metaAnalysisFileTag;
      static const QString studyMetaDataFileTag;
      static const QString vocabularyFileTag;
      static const QString wustlRegionFileTag;

      static const QString documentFileTag;
      static const QString resolutionTag;
      static const QString samplingTag;
     
   protected:
      // copy helper used by copy constructor and operator=
      void copyHelperSpecFile(const SpecFile& sf);
      
      // Read the spec file data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      // Write the spec file data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      // process a tag/value pair while reading spec file
      bool processTag(const std::vector<QString>& tokens);
      
      /// version of the spec file
      int fileVersion;
      
      /// sorting performed when a spec file is read
      SORT sorting;
      
      // Write a set of files associated with a tag
      void writeFiles(QTextStream& stream, 
                      QDomDocument& xmlDoc,
                      QDomElement& rootElement,
                      const QString& tagName, 
                      const std::vector<QString>& names,
                      const std::vector<SPEC_FILE_BOOL>& selected) const;
      
      // Write a set of files (two names) associated with a tag
      void writeFiles(QTextStream& stream, 
                      QDomDocument& xmlDoc,
                      QDomElement& rootElement,
                      const QString& tagName, 
                      const std::vector<QString>& names,
                      const std::vector<QString>& names2,
                      const std::vector<SPEC_FILE_BOOL>& selected) const;
      
      // Write a set of tags with files
      void writeFiles(QTextStream& stream, 
                      QDomDocument& xmlDoc,
                      QDomElement& rootElement,
                      const std::vector<QString>& tagName, 
                      const std::vector<QString>& names,
                      const std::vector<SPEC_FILE_BOOL>& selected) const;
      
      // Write a file associated with a tag
      void writeFiles(QTextStream& stream, 
                      QDomDocument& xmlDoc,
                      QDomElement& rootElement,
                      const QString& tagName, 
                      const QString& name,
                      const SPEC_FILE_BOOL selected) const;
      
      // update the allEntries vector
      void updateAllEntries();
      
      /// only write selected files flag
      bool writeOnlySelectedFiles;
      
      std::vector<QString> otherTags;
      std::vector<QString> otherTagsValues;
      
      /// name for first XML file name
      static const QString xmlFileTagName1;
      
      /// name for second XML file name
      static const QString xmlFileTagName2;
      
      /// name for XML structure
      static const QString xmlFileTagStructure;
      
      // NOTE: BE SURE TO UPDATE copyHelperSpecFile() if new *******************
      // variables are added !!!!***********************************************
};  // class

#ifdef __SPEC_FILE_DEFINE__

   const QString SpecFile::volumeFunctionalFileTag   = "volume_functional_file";
   const QString SpecFile::volumePaintFileTag        = "volume_paint_file";
   const QString SpecFile::volumeProbAtlasFileTag = "volume_prob_atlas_file";
   const QString SpecFile::volumeRgbFileTag     = "volume_rgb_file";
   const QString SpecFile::volumeSegmentationFileTag = "volume_segmentation_file";
   const QString SpecFile::volumeAnatomyFileTag   = "volume_anatomy_file";
   const QString SpecFile::volumeVectorFileTag    = "volume_vector_file";
   
   const QString SpecFile::closedTopoFileTag     = "CLOSEDtopo_file";
   const QString SpecFile::openTopoFileTag       = "OPENtopo_file";
   const QString SpecFile::cutTopoFileTag        = "CUTtopo_file";
   const QString SpecFile::lobarCutTopoFileTag   = "LOBAR_CUTtopo_file";
   const QString SpecFile::unknownTopoFileMatchTag = "topo_file";
   
   const QString SpecFile::topoFlagTag           = "topo_flag";
   
   const QString SpecFile::closedTopoFileTagName     = "CLOSED";
   const QString SpecFile::openTopoFileTagName       = "OPEN";
   const QString SpecFile::cutTopoFileTagName        = "CUT";
   const QString SpecFile::lobarCutTopoFileTagName   = "LOBAR_CUT";
   const QString SpecFile::unknownTopoFileMatchTagName = "OTHER";
   
   const QString SpecFile::rawCoordFileTag        = "RAWcoord_file";
   const QString SpecFile::fiducialCoordFileTag   = "FIDUCIALcoord_file";
   const QString SpecFile::inflatedCoordFileTag   = "INFLATEDcoord_file";
   const QString SpecFile::veryInflatedCoordFileTag   = "VERY_INFLATEDcoord_file";
   const QString SpecFile::sphericalCoordFileTag  = "SPHERICALcoord_file";
   const QString SpecFile::ellipsoidCoordFileTag  = "ELLIPSOIDcoord_file";
   const QString SpecFile::compressedCoordFileTag = "COMPRESSED_MEDIAL_WALLcoord_file";
   const QString SpecFile::flatCoordFileTag       = "FLATcoord_file";
   const QString SpecFile::lobarFlatCoordFileTag  = "LOBAR_FLATcoord_file";
   const QString SpecFile::hullCoordFileTag       = "HULLcoord_file";
   const QString SpecFile::unknownCoordFileMatchTag = "coord_file";
   const QString SpecFile::averageFiducialCoordFileTag = "AVERAGE_FIDUCIALcoord_file";
   
   const QString SpecFile::rawCoordFileTagName        = "RAW";
   const QString SpecFile::fiducialCoordFileTagName   = "FIDUCIAL";
   const QString SpecFile::inflatedCoordFileTagName   = "INFLATED";
   const QString SpecFile::veryInflatedCoordFileTagName   = "VERY_INFLATED";
   const QString SpecFile::sphericalCoordFileTagName  = "SPHERICAL";
   const QString SpecFile::ellipsoidCoordFileTagName  = "ELLIPSOID";
   const QString SpecFile::compressedCoordFileTagName = "COMPRESSED_MEDIAL_WALL";
   const QString SpecFile::flatCoordFileTagName       = "FLAT";
   const QString SpecFile::lobarFlatCoordFileTagName  = "LOBAR_FLAT";
   const QString SpecFile::hullCoordFileTagName       = "HULL";
   const QString SpecFile::averageFiducialCoordFileTagName = "AVERAGE_FIDUCIAL";
   const QString SpecFile::unknownCoordFileMatchTagName = "OTHER";
   
   const QString SpecFile::rawSurfaceFileTag        = "RAWsurface_file";
   const QString SpecFile::fiducialSurfaceFileTag   = "FIDUCIALsurface_file";
   const QString SpecFile::inflatedSurfaceFileTag   = "INFLATEDsurface_file";
   const QString SpecFile::veryInflatedSurfaceFileTag   = "VERY_INFLATEDsurface_file";
   const QString SpecFile::sphericalSurfaceFileTag  = "SPHERICALsurface_file";
   const QString SpecFile::ellipsoidSurfaceFileTag  = "ELLIPSOIDsurface_file";
   const QString SpecFile::compressedSurfaceFileTag = "COMPRESSED_MEDIAL_WALLsurface_file";
   const QString SpecFile::flatSurfaceFileTag       = "FLATsurface_file";
   const QString SpecFile::lobarFlatSurfaceFileTag  = "LOBAR_FLATsurface_file";
   const QString SpecFile::hullSurfaceFileTag       = "HULLsurface_file";
   const QString SpecFile::unknownSurfaceFileMatchTag = "surface_file";
   
   const QString SpecFile::latLonFileTag = "lat_lon_file";
   const QString SpecFile::sectionFileTag = "section_file";
   const QString SpecFile::transformationMatrixFileTag = "transformation_matrix_file";
   const QString SpecFile::transformationDataFileTag = "transformation_data_file";
   
   const QString SpecFile::paintFileTag = "paint_file";
   const QString SpecFile::areaColorFileTag = "area_color_file";
   const QString SpecFile::rgbPaintFileTag = "RGBpaint_file";
   const QString SpecFile::surfaceVectorFileTag = "surface_vector_file";
   
   const QString SpecFile::rawBorderFileTag        = "RAWborder_file";
   const QString SpecFile::fiducialBorderFileTag   = "FIDUCIALborder_file";
   const QString SpecFile::inflatedBorderFileTag   = "INFLATEDborder_file";
   const QString SpecFile::veryInflatedBorderFileTag   = "VERY_INFLATEDborder_file";
   const QString SpecFile::sphericalBorderFileTag  = "SPHERICALborder_file";
   const QString SpecFile::ellipsoidBorderFileTag  = "ELLIPSOIDborder_file";
   const QString SpecFile::compressedBorderFileTag = "COMPRESSED_MEDIAL_WALLborder_file";
   const QString SpecFile::flatBorderFileTag  = "FLATborder_file";
   const QString SpecFile::lobarFlatBorderFileTag = "LOBAR_FLATborder_file";
   const QString SpecFile::hullBorderFileTag = "HULLborder_file";
   const QString SpecFile::unknownBorderFileMatchTag = "border_file";
   const QString SpecFile::volumeBorderFileTag = "VOLUMEborder_file";
   
   const QString SpecFile::rawBorderFileTagName        = "RAW";
   const QString SpecFile::fiducialBorderFileTagName   = "FIDUCIAL";
   const QString SpecFile::inflatedBorderFileTagName   = "INFLATED";
   const QString SpecFile::veryInflatedBorderFileTagName   = "VERY_INFLATED";
   const QString SpecFile::sphericalBorderFileTagName  = "SPHERICAL";
   const QString SpecFile::ellipsoidBorderFileTagName  = "ELLIPSOID";
   const QString SpecFile::compressedBorderFileTagName = "COMPRESSED_MEDIAL_WALL";
   const QString SpecFile::flatBorderFileTagName       = "FLAT";
   const QString SpecFile::lobarFlatBorderFileTagName  = "LOBAR_FLAT";
   const QString SpecFile::hullBorderFileTagName = "HULL";
   const QString SpecFile::unknownBorderFileMatchTagName = "OTHER";
   const QString SpecFile::volumeBorderFileTagName = "VOLUME";
   
      
   const QString SpecFile::borderColorFileTag = "border_color_file";
   const QString SpecFile::borderProjectionFileTag = "borderproj_file";
   
   const QString SpecFile::momcBorderFileTag = "momc_border_file";
   const QString SpecFile::momcTemplateFileTag = "momc_template_file";
   const QString SpecFile::momcTemplateMatchFileTag = "momc_template_match_file";
   
   const QString SpecFile::paletteFileTag = "palette_file";
   
   const QString SpecFile::topographyFileTag = "topography_file";
   
   const QString SpecFile::cellFileTag = "cell_file";
   const QString SpecFile::cellColorFileTag = "cell_color_file";
   const QString SpecFile::cellProjectionFileTag = "cellproj_file";
   const QString SpecFile::volumeCellFileTag = "volume_cell_file";
   
   const QString SpecFile::contourFileTag = "contour_file";
   const QString SpecFile::contourCellFileTag = "contour_cell_file";
   const QString SpecFile::contourCellColorFileTag = "contour_cell_color_file";
   
   const QString SpecFile::fociFilterFlagTag     = "foci_filter_flag";
   const QString SpecFile::fociFileTag           = "foci_file";
   const QString SpecFile::fociColorFileTag      = "foci_color_file";
   const QString SpecFile::fociProjectionFileTag = "fociproj_file";
   const QString SpecFile::fociSearchFileTag     = "foci_search_file";
   
   const QString SpecFile::atlasFileTag = "atlas_file";
   
   const QString SpecFile::metricFileTag = "metric_file";
   const QString SpecFile::surfaceShapeFileTag = "surface_shape_file";
   
   const QString SpecFile::cocomacConnectivityFileTag = "cocomac_connect_file";
   
   const QString SpecFile::arealEstimationFileTag = "areal_estimation_file";
   
   const QString SpecFile::cutsFileTag = "cuts_file";
   
   const QString SpecFile::paramsFileTag = "params_file";
   
   const QString SpecFile::deformationMapFileTag = "deform_map_file";
   
   const QString SpecFile::deformationFieldFileTag = "deform_field_file";
   
   const QString SpecFile::cerebralHullFileTag = "CEREBRAL_HULLvtk_file";
   
   const QString SpecFile::vtkModelFileTag = "vtk_model_file";
   
   const QString SpecFile::geodesicDistanceFileTag = "geodesic_distance_file";
   
   const QString SpecFile::imageFileTag = "image_file";
   const QString SpecFile::sceneFileTag = "scene_file";
   const QString SpecFile::scriptFileTag = "script_file";
   
   const QString SpecFile::metaAnalysisFileTag = "meta_analysis_file";
   const QString SpecFile::studyMetaDataFileTag = "study_metadata_file";
   
   const QString SpecFile::vocabularyFileTag = "vocabulary_file";
   
   const QString SpecFile::wustlRegionFileTag = "wustl_region_file";
   
   const QString SpecFile::documentFileTag = "document_file";
   
   const QString SpecFile::xmlFileTagName1 = "file1";
      
   const QString SpecFile::xmlFileTagName2 = "file2";
   
   const QString SpecFile::xmlFileTagStructure = "structure";
   
   
   SpecFile::SORT SpecFile::Entry::Files::sortMethod = SpecFile::SORT_DATE;

#endif // __SPEC_FILE_DEFINE__

#endif // __SPEC_FILE_H__

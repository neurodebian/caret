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

#include <QDateTime>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>
#include <QFileInfo>

#include "FileUtilities.h"
#define __SPEC_FILE_DEFINE__
#include "SpecFile.h"
#undef __SPEC_FILE_DEFINE__
#include "SpecFileUtilities.h"
#include "StringUtilities.h"

/**
 * The Constructor.
 */
SpecFile::SpecFile()
   : AbstractFile("Spec File", 
                  SpecFile::getSpecFileExtension(), 
                  true,
                  AbstractFile::FILE_FORMAT_ASCII,  //FILE_READ_WRITE_TYPE_XML,
                  FILE_IO_READ_AND_WRITE,
                  FILE_IO_NONE,
                  FILE_IO_READ_AND_WRITE,
                  FILE_IO_NONE)
{
   sorting = SORT_DATE;
   clear();
   
   volumeFunctionalFile.initialize("Volume - Functional", volumeFunctionalFileTag, Entry::FILE_TYPE_VOLUME);
   
   volumePaintFile.initialize("Volume - Paint", volumePaintFileTag, Entry::FILE_TYPE_VOLUME);
   
   volumeProbAtlasFile.initialize("Volume - Probabilistic Atlas", volumeProbAtlasFileTag, Entry::FILE_TYPE_VOLUME);
   
   volumeRgbFile.initialize("Volume - RGB", volumeRgbFileTag, Entry::FILE_TYPE_VOLUME);
   
   volumeSegmentationFile.initialize("Volume - Segmentation", volumeSegmentationFileTag, Entry::FILE_TYPE_VOLUME);
   
   volumeAnatomyFile.initialize("Volume - Anatomy", volumeAnatomyFileTag, Entry::FILE_TYPE_VOLUME);
   
   volumeVectorFile.initialize("Volume - Vector", volumeVectorFileTag, Entry::FILE_TYPE_VOLUME);
   
   closedTopoFile.initialize("Topology - Closed", closedTopoFileTag, Entry::FILE_TYPE_SURFACE);
   openTopoFile.initialize("Topology - Open", openTopoFileTag, Entry::FILE_TYPE_SURFACE);
   cutTopoFile.initialize("Topology - Cut", cutTopoFileTag, Entry::FILE_TYPE_SURFACE);
   lobarCutTopoFile.initialize("Topology - Lobar Cut", lobarCutTopoFileTag, Entry::FILE_TYPE_SURFACE);
   unknownTopoFile.initialize("Topology - Unknown", unknownTopoFileMatchTag, Entry::FILE_TYPE_SURFACE, true);
   
   rawCoordFile.initialize("Coordinate - Raw", rawCoordFileTag, Entry::FILE_TYPE_SURFACE);
   fiducialCoordFile.initialize("Coordinate - Fiducial", fiducialCoordFileTag, Entry::FILE_TYPE_SURFACE);
   inflatedCoordFile.initialize("Coordinate - Inflated", inflatedCoordFileTag, Entry::FILE_TYPE_SURFACE);
   veryInflatedCoordFile.initialize("Coordinate - Very Inflated", veryInflatedCoordFileTag, Entry::FILE_TYPE_SURFACE);
   sphericalCoordFile.initialize("Coordinate - Spherical", sphericalCoordFileTag, Entry::FILE_TYPE_SURFACE);
   ellipsoidCoordFile.initialize("Coordinate - Ellipsoid", ellipsoidCoordFileTag, Entry::FILE_TYPE_SURFACE);
   compressedCoordFile.initialize("Coordinate - Compressed Medial Wall", compressedCoordFileTag, Entry::FILE_TYPE_SURFACE);
   flatCoordFile.initialize("Coordinate - Flat", flatCoordFileTag, Entry::FILE_TYPE_SURFACE);
   lobarFlatCoordFile.initialize("Coordinate - Lobar Flat", lobarFlatCoordFileTag, Entry::FILE_TYPE_SURFACE);
   hullCoordFile.initialize("Coordinate - Hull", hullCoordFileTag, Entry::FILE_TYPE_SURFACE);
   averageFiducialCoordFile.initialize("Coordinate - Average Fiducial", averageFiducialCoordFileTag, Entry::FILE_TYPE_SURFACE);
   unknownCoordFile.initialize("Coordinate - Unknown", unknownCoordFileMatchTag, Entry::FILE_TYPE_SURFACE, true);
   
   rawSurfaceFile.initialize("Surface - Raw", rawSurfaceFileTag, Entry::FILE_TYPE_SURFACE);
   fiducialSurfaceFile.initialize("Surface - Fiducial", fiducialSurfaceFileTag, Entry::FILE_TYPE_SURFACE);
   inflatedSurfaceFile.initialize("Surface - Inflated", inflatedSurfaceFileTag, Entry::FILE_TYPE_SURFACE);
   veryInflatedSurfaceFile.initialize("Surface - Very Inflated", veryInflatedSurfaceFileTag, Entry::FILE_TYPE_SURFACE);
   sphericalSurfaceFile.initialize("Surface - Spherical", sphericalSurfaceFileTag, Entry::FILE_TYPE_SURFACE);
   ellipsoidSurfaceFile.initialize("Surface - Ellipsoid", ellipsoidSurfaceFileTag, Entry::FILE_TYPE_SURFACE);
   compressedSurfaceFile.initialize("Surface - Compressed Medial Wall", compressedSurfaceFileTag, Entry::FILE_TYPE_SURFACE);
   flatSurfaceFile.initialize("Surface - Flat", flatSurfaceFileTag, Entry::FILE_TYPE_SURFACE);
   lobarFlatSurfaceFile.initialize("Surface - Lobar Flat", lobarFlatSurfaceFileTag, Entry::FILE_TYPE_SURFACE);
   hullSurfaceFile.initialize("Surface - Hull", hullSurfaceFileTag, Entry::FILE_TYPE_SURFACE);
   unknownSurfaceFile.initialize("Surface - Unknown", unknownSurfaceFileMatchTag, Entry::FILE_TYPE_SURFACE, true);
   
   latLonFile.initialize("Latitude/Longitude File", latLonFileTag, Entry::FILE_TYPE_SURFACE);
   
   sectionFile.initialize("Section File", sectionFileTag, Entry::FILE_TYPE_SURFACE);
   
   sceneFile.initialize("Scene File", sceneFileTag, Entry::FILE_TYPE_OTHER);
   
   imageFile.initialize("Image File", imageFileTag, Entry::FILE_TYPE_OTHER);
   scriptFile.initialize("Script File", scriptFileTag, Entry::FILE_TYPE_OTHER);
   
   transformationMatrixFile.initialize("Transformation Matrix File", transformationMatrixFileTag, Entry::FILE_TYPE_SURFACE);
   
   paintFile.initialize("Paint File", paintFileTag, Entry::FILE_TYPE_SURFACE);
   areaColorFile.initialize("Area Color File", areaColorFileTag, Entry::FILE_TYPE_SURFACE);
   rgbPaintFile.initialize("RGB Paint File", rgbPaintFileTag, Entry::FILE_TYPE_SURFACE);
   
   surfaceVectorFile.initialize("Surface Vector File", surfaceVectorFileTag, Entry::FILE_TYPE_SURFACE);
   
   rawBorderFile.initialize("Border File - Raw", rawBorderFileTag, Entry::FILE_TYPE_SURFACE);
   fiducialBorderFile.initialize("Border File - Fiducial", fiducialBorderFileTag, Entry::FILE_TYPE_SURFACE);
   inflatedBorderFile.initialize("Border File - Inflated", inflatedBorderFileTag, Entry::FILE_TYPE_SURFACE);
   veryInflatedBorderFile.initialize("Border File - Very Inflated", veryInflatedBorderFileTag, Entry::FILE_TYPE_SURFACE);
   sphericalBorderFile.initialize("Border File - Spherical", sphericalBorderFileTag, Entry::FILE_TYPE_SURFACE);
   ellipsoidBorderFile.initialize("Border File - Ellipsoid", ellipsoidBorderFileTag, Entry::FILE_TYPE_SURFACE);
   compressedBorderFile.initialize("Border File - Comp Med Wall", compressedBorderFileTag, Entry::FILE_TYPE_SURFACE);
   flatBorderFile.initialize("Border File - Flat", flatBorderFileTag, Entry::FILE_TYPE_SURFACE);
   lobarFlatBorderFile.initialize("Border File - Lobar Flat", lobarFlatBorderFileTag, Entry::FILE_TYPE_SURFACE);
   hullBorderFile.initialize("Border File - Hull", hullBorderFileTag, Entry::FILE_TYPE_SURFACE);
   unknownBorderFile.initialize("Border File - Unknown", unknownBorderFileMatchTag, Entry::FILE_TYPE_SURFACE, true);
   volumeBorderFile.initialize("Border File - Volume", volumeBorderFileTag, Entry::FILE_TYPE_SURFACE);
   
   borderColorFile.initialize("Border Color File", borderColorFileTag, Entry::FILE_TYPE_SURFACE);
   borderProjectionFile.initialize("Border Projection File", borderProjectionFileTag, Entry::FILE_TYPE_SURFACE);
   momcTemplateFile.initialize("Momc Template", momcTemplateFileTag, Entry::FILE_TYPE_SURFACE);
   momcTemplateMatchFile.initialize("Momc Template Match", momcTemplateMatchFileTag, Entry::FILE_TYPE_SURFACE);
   
   paletteFile.initialize("Palette File", paletteFileTag, Entry::FILE_TYPE_SURFACE);
  
   topographyFile.initialize("Topography File", topographyFileTag, Entry::FILE_TYPE_SURFACE);
   
   cellFile.initialize("Cell File", cellFileTag, Entry::FILE_TYPE_OTHER);
   cellColorFile.initialize("Cell Color File", cellColorFileTag, Entry::FILE_TYPE_OTHER);
   cellProjectionFile.initialize("Cell Projection File", cellProjectionFileTag, Entry::FILE_TYPE_OTHER);
   volumeCellFile.initialize("Volume Cell File", volumeCellFileTag, Entry::FILE_TYPE_OTHER);
   
   contourFile.initialize("Contour File", contourFileTag, Entry::FILE_TYPE_OTHER);
   
   contourCellFile.initialize("Contour Cell File", contourCellFileTag, Entry::FILE_TYPE_OTHER);
   
   contourCellColorFile.initialize("Contour Cell Color File", contourCellColorFileTag, Entry::FILE_TYPE_OTHER);
   
   atlasFile.initialize("Prog Atlas File", atlasFileTag, Entry::FILE_TYPE_SURFACE);
   
   metricFile.initialize("Metric File", metricFileTag, Entry::FILE_TYPE_SURFACE);
   
   surfaceShapeFile.initialize("Surface Shape File", surfaceShapeFileTag, Entry::FILE_TYPE_SURFACE);
   
   cocomacConnectivityFile.initialize("CoCoMac File", cocomacConnectivityFileTag, Entry::FILE_TYPE_SURFACE);
   
   arealEstimationFile.initialize("Areal Estimation File", arealEstimationFileTag, Entry::FILE_TYPE_SURFACE);
   
   cutsFile.initialize("Cuts File", cutsFileTag, Entry::FILE_TYPE_SURFACE);
   
   fociFile.initialize("Foci File", fociFileTag, Entry::FILE_TYPE_OTHER);
   fociColorFile.initialize("Foci Color File", fociColorFileTag, Entry::FILE_TYPE_OTHER);
   fociProjectionFile.initialize("Foci Projection File", fociProjectionFileTag, Entry::FILE_TYPE_OTHER);
   volumeFociFile.initialize("Volume Foci File", volumeFociFileTag, Entry::FILE_TYPE_OTHER);
   
   paramsFile.initialize("Params File", paramsFileTag, Entry::FILE_TYPE_OTHER);
   deformationMapFile.initialize("Deformation Map File", deformationMapFileTag, Entry::FILE_TYPE_SURFACE);
   deformationFieldFile.initialize("Deformation Field File", deformationFieldFileTag, Entry::FILE_TYPE_SURFACE);
   cerebralHullFile.initialize("Cerebral Hull File", cerebralHullFileTag, Entry::FILE_TYPE_OTHER);
   
   vtkModelFile.initialize("VTK Model File", vtkModelFileTag, Entry::FILE_TYPE_SURFACE);
   
   geodesicDistanceFile.initialize("Geodesic Distance File", geodesicDistanceFileTag, Entry::FILE_TYPE_SURFACE);
   
   wustlRegionFile.initialize("WUSTL Region File", wustlRegionFileTag, Entry::FILE_TYPE_OTHER);
   
   metaAnalysisFile.initialize("Study Meta-Analysis File", metaAnalysisFileTag, Entry::FILE_TYPE_OTHER);
   studyMetaDataFile.initialize("Study Metadata File", studyMetaDataFileTag, Entry::FILE_TYPE_OTHER);
   
   transformationDataFile.initialize("Transformation Data File", transformationDataFileTag, Entry::FILE_TYPE_SURFACE);
   
   vocabularyFile.initialize("Vocabulary File", vocabularyFileTag, Entry::FILE_TYPE_OTHER);
   
   documentFile.initialize("Document File", documentFileTag, Entry::FILE_TYPE_OTHER);
   
   updateAllEntries();
}

/**
 * The Destructor.
 */
SpecFile::~SpecFile()
{
   clear();
}

/**
 * update the allEntries vector.
 */
void 
SpecFile::updateAllEntries()
{
   allEntries.clear();
   
   allEntries.push_back(&volumeFunctionalFile);
   
   allEntries.push_back(&volumePaintFile);
   
   allEntries.push_back(&volumeProbAtlasFile);
   
   allEntries.push_back(&volumeRgbFile);
   
   allEntries.push_back(&volumeSegmentationFile);
   
   allEntries.push_back(&volumeAnatomyFile);
   
   allEntries.push_back(&volumeVectorFile);
   
   allEntries.push_back(&closedTopoFile);
   allEntries.push_back(&openTopoFile);
   allEntries.push_back(&cutTopoFile);
   allEntries.push_back(&lobarCutTopoFile);
   //
   // unknownTopoFile File MUST BE AFTER ALL OTHER TOPO FILES
   //
   allEntries.push_back(&unknownTopoFile);
   
   allEntries.push_back(&rawCoordFile);
   allEntries.push_back(&fiducialCoordFile);
   allEntries.push_back(&inflatedCoordFile);
   allEntries.push_back(&veryInflatedCoordFile);
   allEntries.push_back(&sphericalCoordFile);
   allEntries.push_back(&ellipsoidCoordFile);
   allEntries.push_back(&compressedCoordFile);
   allEntries.push_back(&flatCoordFile);
   allEntries.push_back(&lobarFlatCoordFile);
   allEntries.push_back(&hullCoordFile);
   allEntries.push_back(&averageFiducialCoordFile);
   
   //
   // unknownCoordFile MUST BE AFTER ALL OTHER COORD FILES
   //
   allEntries.push_back(&unknownCoordFile);
   
   
   allEntries.push_back(&rawSurfaceFile);
   allEntries.push_back(&fiducialSurfaceFile);
   allEntries.push_back(&inflatedSurfaceFile);
   allEntries.push_back(&veryInflatedSurfaceFile);
   allEntries.push_back(&sphericalSurfaceFile);
   allEntries.push_back(&ellipsoidSurfaceFile);
   allEntries.push_back(&compressedSurfaceFile);
   allEntries.push_back(&flatSurfaceFile);
   allEntries.push_back(&lobarFlatSurfaceFile);
   allEntries.push_back(&hullSurfaceFile);

   //
   // unknownSurfaceFile File MUST BE AFTER ALL OTHER SURFACES
   //
   allEntries.push_back(&unknownSurfaceFile);
   
   allEntries.push_back(&latLonFile);
   
   allEntries.push_back(&sectionFile);
   
   allEntries.push_back(&sceneFile);
   
   allEntries.push_back(&imageFile);
   allEntries.push_back(&scriptFile);
   
   allEntries.push_back(&transformationMatrixFile);
   
   allEntries.push_back(&paintFile);
   allEntries.push_back(&areaColorFile);
   allEntries.push_back(&rgbPaintFile);
   
   allEntries.push_back(&surfaceVectorFile);
   
   allEntries.push_back(&rawBorderFile);
   allEntries.push_back(&fiducialBorderFile);
   allEntries.push_back(&inflatedBorderFile);
   allEntries.push_back(&veryInflatedBorderFile);
   allEntries.push_back(&sphericalBorderFile);
   allEntries.push_back(&ellipsoidBorderFile);
   allEntries.push_back(&compressedBorderFile);
   allEntries.push_back(&flatBorderFile);
   allEntries.push_back(&lobarFlatBorderFile);
   allEntries.push_back(&hullBorderFile);
   allEntries.push_back(&volumeBorderFile);
   
   //
   // unknownBorderFile File MUST BE AFTER ALL OTHER BORDERS
   //
   allEntries.push_back(&unknownBorderFile);

   allEntries.push_back(&borderColorFile);
   allEntries.push_back(&borderProjectionFile);
   allEntries.push_back(&momcTemplateFile);
   allEntries.push_back(&momcTemplateMatchFile);
   
   allEntries.push_back(&paletteFile);
  
   allEntries.push_back(&topographyFile);
   
   allEntries.push_back(&cellFile);
   allEntries.push_back(&cellColorFile);
   allEntries.push_back(&cellProjectionFile);
   allEntries.push_back(&volumeCellFile);
   
   allEntries.push_back(&contourFile);
   
   allEntries.push_back(&contourCellFile);
   
   allEntries.push_back(&contourCellColorFile);
   
   allEntries.push_back(&atlasFile);
   
   allEntries.push_back(&metricFile);
   
   allEntries.push_back(&surfaceShapeFile);
   
   allEntries.push_back(&cocomacConnectivityFile);
   
   allEntries.push_back(&arealEstimationFile);
   
   allEntries.push_back(&cutsFile);
   
   allEntries.push_back(&fociFile);
   allEntries.push_back(&fociColorFile);
   allEntries.push_back(&fociProjectionFile);
   allEntries.push_back(&volumeFociFile);
   
   allEntries.push_back(&paramsFile);
   allEntries.push_back(&deformationMapFile);
   allEntries.push_back(&deformationFieldFile);
   allEntries.push_back(&cerebralHullFile);
   
   allEntries.push_back(&vtkModelFile);
   
   allEntries.push_back(&geodesicDistanceFile);
   
   allEntries.push_back(&wustlRegionFile);
   
   allEntries.push_back(&transformationDataFile);
   
   allEntries.push_back(&metaAnalysisFile);
   allEntries.push_back(&studyMetaDataFile);
   allEntries.push_back(&vocabularyFile);

   allEntries.push_back(&documentFile);
}
      
/**
 * Copy constructor.
 */
SpecFile::SpecFile(const SpecFile& sf)
   : AbstractFile(sf)
{
   copyHelperSpecFile(sf);
}
      
/**
 * assignment operator.
 */
SpecFile& 
SpecFile::operator=(const SpecFile& sf)
{
   if (this != &sf) {
      AbstractFile::operator=(sf);
      copyHelperSpecFile(sf);
   }
   
   return *this;
}

/**
 * copy helper used by copy constructor and operator=
 */
void 
SpecFile::copyHelperSpecFile(const SpecFile& sf)
{
   volumeFunctionalFile = sf.volumeFunctionalFile;
   
   volumePaintFile = sf.volumePaintFile;
   
   volumeProbAtlasFile = sf.volumeProbAtlasFile;
   
   volumeRgbFile = sf.volumeRgbFile;
   
   volumeSegmentationFile = sf.volumeSegmentationFile;
   
   volumeAnatomyFile = sf.volumeAnatomyFile;
   
   volumeVectorFile = sf.volumeVectorFile;
   
   closedTopoFile = sf.closedTopoFile;
   openTopoFile = sf.openTopoFile;
   cutTopoFile = sf.cutTopoFile;
   lobarCutTopoFile = sf.lobarCutTopoFile;
   unknownTopoFile = sf.unknownTopoFile;
   
   rawCoordFile = sf.rawCoordFile;
   fiducialCoordFile = sf.fiducialCoordFile;
   inflatedCoordFile = sf.inflatedCoordFile;
   veryInflatedCoordFile = sf.veryInflatedCoordFile;
   sphericalCoordFile = sf.sphericalCoordFile;
   ellipsoidCoordFile = sf.ellipsoidCoordFile;
   compressedCoordFile = sf.compressedCoordFile;
   flatCoordFile = sf.flatCoordFile;
   lobarFlatCoordFile = sf.lobarFlatCoordFile;
   hullCoordFile = sf.hullCoordFile;
   unknownCoordFile = sf.unknownCoordFile;
   
   averageFiducialCoordFile = sf.averageFiducialCoordFile;
   
   rawSurfaceFile = sf.rawSurfaceFile;
   fiducialSurfaceFile = sf.fiducialSurfaceFile;
   inflatedSurfaceFile = sf.inflatedSurfaceFile;
   veryInflatedSurfaceFile = sf.veryInflatedSurfaceFile;
   sphericalSurfaceFile = sf.sphericalSurfaceFile;
   ellipsoidSurfaceFile = sf.ellipsoidSurfaceFile;
   compressedSurfaceFile = sf.compressedSurfaceFile;
   flatSurfaceFile = sf.flatSurfaceFile;
   lobarFlatSurfaceFile = sf.lobarFlatSurfaceFile;
   hullSurfaceFile = sf.hullSurfaceFile;
   unknownSurfaceFile = sf.unknownSurfaceFile;
   
   latLonFile = sf.latLonFile;
   
   sectionFile = sf.sectionFile;
   
   sceneFile = sf.sceneFile;
   
   imageFile = sf.imageFile;
   scriptFile = sf.scriptFile;
   
   transformationMatrixFile = sf.transformationMatrixFile;
   
   paintFile = sf.paintFile;
   areaColorFile = sf.areaColorFile;
   rgbPaintFile = sf.rgbPaintFile;
   
   surfaceVectorFile = sf.surfaceVectorFile;
   
   rawBorderFile = sf.rawBorderFile;
   fiducialBorderFile = sf.fiducialBorderFile;
   inflatedBorderFile = sf.inflatedBorderFile;
   veryInflatedBorderFile = sf.veryInflatedBorderFile;
   sphericalBorderFile = sf.sphericalBorderFile;
   ellipsoidBorderFile = sf.ellipsoidBorderFile;
   compressedBorderFile = sf.compressedBorderFile;
   flatBorderFile = sf.flatBorderFile;
   lobarFlatBorderFile = sf.lobarFlatBorderFile;
   hullBorderFile = sf.hullBorderFile;
   unknownBorderFile = sf.unknownBorderFile;
   volumeBorderFile = sf.volumeBorderFile;
   
   borderColorFile = sf.borderColorFile;
   borderProjectionFile = sf.borderProjectionFile;
   momcTemplateFile = sf.momcTemplateFile;
   momcTemplateMatchFile = sf.momcTemplateMatchFile;
   
   paletteFile = sf.paletteFile;
  
   topographyFile = sf.topographyFile;
   
   cellFile = sf.cellFile;
   cellColorFile = sf.cellColorFile;
   cellProjectionFile = sf.cellProjectionFile;
   volumeCellFile = sf.volumeCellFile;
   
   contourFile = sf.contourFile;
   
   contourCellFile = sf.contourCellFile;
   
   contourCellColorFile = sf.contourCellColorFile;
   
   atlasFile = sf.atlasFile;
   
   metricFile = sf.metricFile;
   
   surfaceShapeFile = sf.surfaceShapeFile;
   
   cocomacConnectivityFile = sf.cocomacConnectivityFile;
   
   arealEstimationFile = sf.arealEstimationFile;
   
   cutsFile = sf.cutsFile;
   
   fociFile = sf.fociFile;
   fociColorFile = sf.fociColorFile;
   fociProjectionFile = sf.fociProjectionFile;
   volumeFociFile = sf.volumeFociFile;
   
   paramsFile = sf.paramsFile;
   deformationMapFile = sf.deformationMapFile;
   deformationFieldFile = sf.deformationFieldFile;
   cerebralHullFile = sf.cerebralHullFile;
   
   vtkModelFile = sf.vtkModelFile;
   
   geodesicDistanceFile = sf.geodesicDistanceFile;
   
   metaAnalysisFile = sf.metaAnalysisFile;
   studyMetaDataFile = sf.studyMetaDataFile;
   vocabularyFile = sf.vocabularyFile;
   wustlRegionFile = sf.wustlRegionFile;
   
   documentFile = sf.documentFile;
   
   transformationDataFile = sf.transformationDataFile;
   
   fileVersion = sf.fileVersion;
   
   sorting = sf.sorting;
   writeOnlySelectedFiles = sf.writeOnlySelectedFiles;
   
   otherTags = sf.otherTags;
   otherTagsValues = sf.otherTagsValues;

   updateAllEntries();
}

/**
 * Remove paths from all files.
 */
void
SpecFile::removePathsFromAllFiles()
{
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      allEntries[i]->removePaths();
   }
}

/**
 * Prepend paths to a vector of file names.
 */
void 
SpecFile::prependPathsToAllFiles(const QString& path,
                                 const bool ignoreFilesWithAbsPaths)
{
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      allEntries[i]->prependPath(path, ignoreFilesWithAbsPaths);
   }
}

/**
 * Clear the contents of the spec file.
 */
void
SpecFile::clear()
{
   fileVersion = 0;
   
   writeOnlySelectedFiles = false;
   
   setStructure("");
   setSpecies("");
   setCategory("");
   setSpace("");  
     
   clearFiles(true, true, true, false);
   
   // DO NOT CLEAR THESE
   // allEntries
}   

/**
 * Remove files from the spec file and possibly the files themselves on the disk.
 */
void 
SpecFile::clearFiles(const bool clearVolumeFiles, 
                      const bool clearSurfaceFiles, 
                      const bool clearOtherFiles,
                      const bool removeFilesFromDiskToo)
{
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      switch(allEntries[i]->fileType) {
         case Entry::FILE_TYPE_SURFACE:
            if (clearSurfaceFiles) {
               allEntries[i]->clear(removeFilesFromDiskToo);
            }
            break;
         case Entry::FILE_TYPE_VOLUME:
            if (clearVolumeFiles) {
               allEntries[i]->clear(removeFilesFromDiskToo);
            }
            break;
         case Entry::FILE_TYPE_OTHER:
            if (clearOtherFiles) {
               allEntries[i]->clear(removeFilesFromDiskToo);
            }
            break;
      }
   }   
}

/**
 * get the species.
 */
QString 
SpecFile::getSpecies() const
{
   return getHeaderTag(AbstractFile::headerTagSpecies);
}

/**
 * set the species.
 */
void 
SpecFile::setSpecies(const QString& speciesIn)
{
   setHeaderTag(AbstractFile::headerTagSpecies, speciesIn);
}

/**
 * get the subject.
 */
QString 
SpecFile::getSubject() const
{
   return getHeaderTag(AbstractFile::headerTagSubject);
}

/**
 * set the subject.
 */
void 
SpecFile::setSubject(const QString& subjectIn)
{
   setHeaderTag(AbstractFile::headerTagSubject, subjectIn);
}

/**
 * get the space.
 */
QString 
SpecFile::getSpace() const
{
   return getHeaderTag(AbstractFile::headerTagSpace);
}

/**
 * set the space.
 */
void 
SpecFile::setSpace(const QString& spaceIn)
{
   setHeaderTag(AbstractFile::headerTagSpace, spaceIn);
}

/**
 * get the structure.
 */
QString
SpecFile::getStructure() const
{
   QString s = getHeaderTag(AbstractFile::headerTagStructure);
   if (s.isEmpty()) {
      s = getHeaderTag("hem_flag");
   }
   return s;
}

/**
 * set the structure.
 */
void
SpecFile::setStructure(const QString& hem)
{
   setHeaderTag(AbstractFile::headerTagStructure, hem);
}

/**
 * get the category.
 */
QString 
SpecFile::getCategory() const
{
   return getHeaderTag(AbstractFile::headerTagCategory);
}

/**
 * set the category.
 */
void 
SpecFile::setCategory(const QString& cat)
{
   setHeaderTag(AbstractFile::headerTagCategory, cat);
}

/**
 * convert all data files to the specified data type.
 */
void 
SpecFile::convertAllDataFilesToType(const std::vector<AbstractFile::FILE_FORMAT> requestedFormats,
                                    const bool printInfoToStdout)
{
   //
   // Get names of all data files
   //
   std::vector<QString> dataFiles;
   getAllDataFilesInSpecFile(dataFiles);
   
   //
   // Set to spec file's directory
   //
   const QString savedDirectory(QDir::currentPath());
   setCurrentDirectoryToSpecFileDirectory();
   
   //
   // Convert all files
   //
   const int num = static_cast<int>(dataFiles.size());
   for (int i = 0; i < num; i++) {
      const QString filename(dataFiles[i]);
      
      //
      // Read the header of the file
      //
      QString errorMessage;
      AbstractFile* af = AbstractFile::readAnySubClassDataFile(filename, true, errorMessage);
      if (printInfoToStdout) {
         std::cout << FileUtilities::basename(filename).toAscii().constData() << ": ";
      }
      if (af == NULL) {
         if (printInfoToStdout) {
            std::cout << "unable to read file or not a caret data file.\n"
                      << "error: " 
                      << errorMessage.toAscii().constData();
         }
      }
      else if (af->getFileHasHeader() == false) {
         if (printInfoToStdout) {
            std::cout << "file does not have header.";
         }
      }
      else {
         //
         // Get files current encoding
         // 
         const QString formatString(af->getHeaderTag(AbstractFile::headerTagEncoding));
         bool validFormatNameFlag = false;
         const AbstractFile::FILE_FORMAT currentFormat = convertFormatNameToType(formatString,
                                                               &validFormatNameFlag);
         if (validFormatNameFlag) {
            bool doneFlag = false;
            for (unsigned int j = 0; j < requestedFormats.size(); j++) {
               const AbstractFile::FILE_FORMAT newFormat = requestedFormats[j];
               const QString newFormatName(AbstractFile::convertFormatTypeToName(newFormat));
               if (currentFormat == newFormat) {
                  if (printInfoToStdout) {
                     std::cout << "already in "
                               << newFormatName.toAscii().constData()
                               << " file format.";
                  }
                  doneFlag = true;
               }
               else if (af->getCanWrite(newFormat)) {
                  try {
                     af->readFile(filename);
                     af->setFileWriteType(newFormat);
                     af->writeFile(filename);
                     if (printInfoToStdout) {
                        std::cout << "converted to "
                                  << newFormatName.toAscii().constData()
                                  << ".";
                     }
                  }
                  catch (FileException& e) {
                     if (printInfoToStdout) {
                        std::cout << "error converting or writing.";
                     }
                  }
                  doneFlag = true;
               }
               
               if (doneFlag) {
                  break;
               }
            }
            
            if (doneFlag == false) {
               if (printInfoToStdout) {
                  std::cout << "does not support the requested format(s).";
               }
            }
         }
         else {
            if (printInfoToStdout) {
               std::cout << "unrecognized format: "
                         << formatString.toAscii().constData(); 
            }
         }
         if (printInfoToStdout) {
            std::cout << std::endl;
         }
      }

      if (af != NULL) {
        delete af;  // can't delete ?? compiler bug ??
        //af->clear();   // so at least free up memory
      }
   }
   
   //
   // Restore directory
   //
   QDir::setCurrent(savedDirectory);
}

/**
 * validate a spec file - all files exist and are readable (true if valid).
 */
bool 
SpecFile::validate(QString& errorMessage) const
{
   errorMessage = "";
   
   //
   // get a list of the data files in the spec file.
   // exclude the volume data files.
   //
   std::vector<QString> dataFiles;
   getAllDataFilesInSpecFile(dataFiles, false);
   
   //
   // Save the current directory
   //
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Set to spec file's directory
   //
   const QString fn(getFileName());
   if (fn.isEmpty() == false) {
      QDir::setCurrent(FileUtilities::dirname(fn));
   }
   
   //
   // Check each file
   //
   for (unsigned int i = 0; i < dataFiles.size(); i++) {
      if (dataFiles[i].isEmpty() == false) {
         QFile f(dataFiles[i]);
         if (f.exists() == false) {
            errorMessage.append("DATA FILE NOT FOUND: ");
            errorMessage.append(dataFiles[i]);
            errorMessage.append("\n");
         }
      }
   }
   
   //
   // Restore the current directory
   //
   QDir::setCurrent(savedDirectory);
   
   return errorMessage.isEmpty();
}

/**
 * get all of the data files in the spec file.
 */
void 
SpecFile::getAllDataFilesInSpecFile(std::vector<QString>& allFiles,
                                    const bool includeVolumeDataFiles) const
{
   allFiles.clear();
   
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      for (unsigned int j = 0; j < allEntries[i]->files.size(); j++) {
         allFiles.push_back(allEntries[i]->files[j].filename);
         if (includeVolumeDataFiles) {
            if (allEntries[i]->fileType == Entry::Entry::FILE_TYPE_VOLUME) {
               allFiles.push_back(allEntries[i]->files[j].dataFileName);
            }
         }
      }
   }
}

/**
 * Get the number of selected files
 */
int
SpecFile::getNumberOfSelectedFiles() const
{
   int numFilesSelected = 0;
   
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      numFilesSelected += allEntries[i]->getNumberOfFilesSelected();
   }

   return numFilesSelected;
}

/**
 * see if only scene files are selected.
 */
bool 
SpecFile::onlySceneFilesSelected() const
{
   return (getNumberOfSelectedFiles() == sceneFile.getNumberOfFilesSelected());
}      

/**
 * Deselect all files
 */
void
SpecFile::setAllFileSelections(const SPEC_FILE_BOOL status)
{
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      allEntries[i]->setAllSelections(status);
   }
}

/**
 * Set the specified topology and coordinate files as selected.
 */
void 
SpecFile::setTopoAndCoordSelected(const QString& topoName,
                                  const QString& coordName,
                                  const Structure structureIn)
{
   unknownTopoFile.setSelected(topoName,
                               true,
                               structureIn);
   unknownCoordFile.setSelected(coordName,
                                true,
                                structureIn);                         
}

/**
 * Set the specified topology and coordinate files as selected.
 */
void 
SpecFile::setTopoAndCoordSelected(const QString& topoName,
                                  const std::vector<QString>& coordNames,
                                  const Structure structureIn)
{
   unknownTopoFile.setSelected(topoName,
                               true,
                               structureIn);
   for (unsigned int i = 0; i < coordNames.size(); i++) {
      if (coordNames[i].isEmpty() == false) {
         unknownCoordFile.setSelected(coordNames[i],
                                      true,
                                      structureIn);
      }
   }
}

/**
 * set the specified files for metric mapping.  Return error message which
 * is isEmpty if there is no error.
 */
QString
SpecFile::setFilesForMetricMapping(const QString& topoFileName,
                                   const QString& coordFileName,
                                   const QString& metricFileName,
                                   const Structure structureIn)
{
   const bool topoSelected = 
      closedTopoFile.setSelected(topoFileName, false, structureIn) ||
      openTopoFile.setSelected(topoFileName,     false, structureIn) ||
      cutTopoFile.setSelected(topoFileName,      false, structureIn) ||
      lobarCutTopoFile.setSelected(topoFileName, false, structureIn) ||
      unknownTopoFile.setSelected(topoFileName,  false, structureIn);

   const bool coordSelected = 
      fiducialCoordFile.setSelected(coordFileName, false, structureIn) ||
      rawCoordFile.setSelected(coordFileName, false, structureIn) ||
      inflatedCoordFile.setSelected(coordFileName, false, structureIn) ||
      veryInflatedCoordFile.setSelected(coordFileName, false, structureIn) ||
      sphericalCoordFile.setSelected(coordFileName, false, structureIn) ||
      ellipsoidCoordFile.setSelected(coordFileName, false, structureIn) ||
      compressedCoordFile.setSelected(coordFileName, false, structureIn) ||
      flatCoordFile.setSelected(coordFileName, false, structureIn) ||
      lobarFlatCoordFile.setSelected(coordFileName, false, structureIn) ||
      hullCoordFile.setSelected(coordFileName, false, structureIn) ||
      unknownCoordFile.setSelected(coordFileName, false, structureIn) ||
      averageFiducialCoordFile.setSelected(coordFileName, false, structureIn);

   //
   // Note: not a failure if metric not found
   //
   metricFile.setSelected(metricFileName, false, structureIn);
  
   QString msg;
   if (topoSelected == false) {
      msg.append("ERROR: ");
      msg.append(topoFileName);
      msg.append(" is not a topo file in the spec file.");
   }
   
   if (coordSelected == false) {
      if (msg.isEmpty() == false) {
         msg.append("\n");
      }
      msg.append("ERROR: ");
      msg.append(coordFileName);
      msg.append(" is not a coord file in the spec file.");
   }
   
   return msg;
}

/**
 * Set default files (just Fiducial and Flat)
 */
void
SpecFile::setDefaultFilesFiducialAndFlat()
{
   setAllFileSelections(SPEC_FALSE);
   
   if (fiducialSurfaceFile.files.size() > 0) {
      fiducialSurfaceFile.files[0].selected = SPEC_TRUE;
   }
   else {
      if (closedTopoFile.files.size() > 0) {
         closedTopoFile.files[0].selected = SPEC_TRUE;
      }
      if (fiducialCoordFile.files.size() > 0) {
         fiducialCoordFile.files[0].selected = SPEC_TRUE;
      }
   }
   
   if (flatSurfaceFile.files.size() > 0) {
      flatSurfaceFile.files[0].selected = SPEC_TRUE;
   }
   else {
      if (cutTopoFile.files.size() > 0) {
         cutTopoFile.files[0].selected = SPEC_TRUE;
      }
      if (flatCoordFile.files.size() > 0) {
         flatCoordFile.files[0].selected = SPEC_TRUE;
      }
   }
   
   if (surfaceShapeFile.files.size() > 0) {
      surfaceShapeFile.files[0].selected = SPEC_TRUE;
   }
   if (paramsFile.files.size() > 0) {
      paramsFile.files[0].selected = SPEC_TRUE;
   }
   if (cerebralHullFile.files.size() > 0) {
      cerebralHullFile.files[0].selected = SPEC_TRUE;
   }
}

/**
 * Set the selected files or use by a deformation.
 */
void 
SpecFile::setDeformationSelectedFiles(const QString& closedTopoName,
                                      const QString& cutTopoName,
                                      const QString& fiducialCoordName,
                                      const QString& sphericalCoordName,
                                      const QString& flatCoordName,
                                      const QString& borderOrBorderProjName,
                                      const bool addIfNotFound,
                                      const Structure& structureIn)
{
   //
   // Deselect all files
   //
   setAllFileSelections(SPEC_FALSE);

   //
   // Select the topo files
   //
   closedTopoFile.setSelected(closedTopoName, addIfNotFound, structureIn);
   closedTopoFile.setSelected(cutTopoName, addIfNotFound, structureIn);
   cutTopoFile.setSelected(closedTopoName, addIfNotFound, structureIn);
   cutTopoFile.setSelected(cutTopoName, addIfNotFound, structureIn);
   lobarCutTopoFile.setSelected(closedTopoName, addIfNotFound, structureIn);
   lobarCutTopoFile.setSelected(cutTopoName, addIfNotFound, structureIn);
   
   //
   // Select the coordinate files
   //
   fiducialCoordFile.setSelected(fiducialCoordName, addIfNotFound, structureIn);
   sphericalCoordFile.setSelected(sphericalCoordName, addIfNotFound, structureIn);
   flatCoordFile.setSelected(flatCoordName, addIfNotFound, structureIn);
   lobarFlatCoordFile.setSelected(flatCoordName, addIfNotFound, structureIn);
   
   //
   // Select the border file
   //
   sphericalBorderFile.setSelected(borderOrBorderProjName, addIfNotFound, structureIn);
   flatBorderFile.setSelected(borderOrBorderProjName, addIfNotFound, structureIn);
   lobarFlatBorderFile.setSelected(borderOrBorderProjName, addIfNotFound, structureIn);
   borderProjectionFile.setSelected(borderOrBorderProjName, addIfNotFound, structureIn);
}                                       

/**
 * set the files selected for multi-resolution morphing.
 */
void 
SpecFile::setMultiresolutionMorphingFiles(const QString& topoFileName,
                                          const QString& fiducialFileName,
                                          const QString& flatOrSphereFileName,
                                          const Structure& structureIn)
{
   closedTopoFile.setSelected(topoFileName, false, structureIn);
   openTopoFile.setSelected(topoFileName, false, structureIn);
   cutTopoFile.setSelected(topoFileName, false, structureIn);
   lobarCutTopoFile.setSelected(topoFileName, false, structureIn);
   unknownTopoFile.setSelected(topoFileName, false, structureIn);

   fiducialCoordFile.setSelected(fiducialFileName, false, structureIn);
   
   sphericalCoordFile.setSelected(flatOrSphereFileName, false, structureIn);
   flatCoordFile.setSelected(flatOrSphereFileName, false, structureIn);
   lobarFlatCoordFile.setSelected(flatOrSphereFileName, false, structureIn);
}                                           

/**
 * append a spec file to this one (files only).
 */
void 
SpecFile::append(const SpecFile& sf)
{
   for (unsigned int i = 0; i < sf.allEntries.size(); i++) {
      const Entry* e = sf.allEntries[i];
      const QString tag = e->getSpecFileTag();
      for (int j = 0; j < e->getNumberOfFiles(); j++) {
         const QString fileName = e->getFileName(j);
         const QString dataName = e->getDataFileName(j);         
         addToSpecFile(tag, fileName, dataName, false);
      }
   }
}
            
/**
 * Add a tag/file to the spec file.  Make the file's path relative
 * to the spec file and then write the spec file if the spec file has
 * a name.  Returns true if the file was written.
 */
bool
SpecFile::addToSpecFile(const QString& tag, 
                        const QString& value1In,
                        const QString& value2In,
                        const bool writeSpecFileIfChanged)
{
   bool fileWasWrittenFlag = false;
   
   QString value(value1In);
   if (getFileName().isEmpty() == false) {
      FileUtilities::relativePath(value1In, getFileNamePath(), value);
   }
   else {
      value = FileUtilities::basename(value1In);
   }
   
   std::vector<QString> tokens;
   tokens.push_back(tag);
   tokens.push_back(value);
   if (value2In.isEmpty() == false) {
      tokens.push_back(FileUtilities::basename(value2In));
   }
   
   const bool newFileFlag = processTag(tokens);

   if (newFileFlag) {
      setModified();
      
      if (writeSpecFileIfChanged) {
         if (getFileName().isEmpty() == false) {
            try {
               writeFile(getFileName());
               fileWasWrittenFlag = true;
            }
            catch (FileException& e) {
            }
         }
      }
   }
   
   return fileWasWrittenFlag;
}

/**
 * Process tags as they are read from the spec file anfd automatically selecte them.
 * Returns true if the file was not already part of the spec file.
 */
bool
SpecFile::processTag(const std::vector<QString>& tokens)
{
   if (tokens.size() < 2) {
      return false;
   }
   
   Structure st;
   QString filename;
   QString dataFileName;
   
   const int num = static_cast<int>(tokens.size());
   int indx = 0;
   const QString tag(tokens[indx]);
   indx++;
   
   QString stString;
   if (fileVersion >= 1) {
      stString = tokens[indx];
      st.setTypeFromString(tokens[indx]);
      indx++;
   }
   
   if (indx < num) {
      filename = tokens[indx];
      indx++;
   }
   else {
      return false;
   }
   
   if (indx < num) {
      dataFileName = tokens[indx];
      const QString directory(FileUtilities::dirname(filename));
      SpecFileUtilities::setVolumeDataFileName(directory, filename, dataFileName);
   }
   
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      if (allEntries[i]->addFile(tag, filename, dataFileName, st)) {
         return true;
      }
   }
   
   std::cout << "Unknown Spec File tag moved to header: " << tag.toAscii().constData() << std::endl;
   std::ostringstream str;
   str << stString.toAscii().constData() << " "
       << filename.toAscii().constData() << " "
       << dataFileName.toAscii().constData();
   setHeaderTag(tag, filename);
   
   return false;
}

/**
 * Read the spec file's data.
 */
void
SpecFile::readFileData(QFile& /*file*/, QTextStream& stream, QDataStream&,
                                  QDomElement& rootElement) throw (FileException)
{
   fileVersion = 0;
   
   //
   // 7112B changed to 711-2B
   //
   if (getHeaderTag(AbstractFile::headerTagSpace) == "7112B") {
      setHeaderTag(AbstractFile::headerTagSpace, "711-2B");
   }
   
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            while(stream.atEnd() == false) {
               std::vector<QString> tokens;
               QString line;
               readLineIntoTokens(stream, line, tokens);
               if (line.isEmpty() == false) {
                  if (line[0] != '#') {
                     if (tokens.empty() == false) {
                        processTag(tokens);
                     }
                  }
               }
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            QDomNode node = rootElement.firstChild();
            while (node.isNull() == false) {
               QDomElement elem = node.toElement();
               if (elem.isNull() == false) {
                  std::vector<QString> tokens;
                  tokens.push_back(elem.tagName());
                  
                  QString val1, val2;
                  QDomNode childNode = elem.firstChild();
                  while (childNode.isNull() == false) {
                     QDomElement elemChild = childNode.toElement();
                     if (elemChild.isNull() == false) {
                        if (elemChild.tagName() == xmlFileTagName1) {
                           val1 = getXmlElementFirstChildAsString(elemChild);
                        }
                        else if (elemChild.tagName() == xmlFileTagName2) {
                           val2 = getXmlElementFirstChildAsString(elemChild);
                        }
                     }
                     childNode = childNode.nextSibling();
                  }
                  
                  if (val1.isEmpty() == false) {
                     tokens.push_back(val1);
                     if (val2.isEmpty() == false) {
                        tokens.push_back(val2);
                     }
                  }
                  if (tokens.empty() == false) {
                     processTag(tokens);
                  }
               }
               node = node.nextSibling();
            }
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }
   
   switch (sorting) {
      case SORT_NONE:
         break;
      case SORT_DATE:
         sortAllFilesByDate();
         break;
      case SORT_NAME:
         sortAllFilesByName();
         break;
   }
   
   //
   // For version 0 files, all files are for one structure
   //
   if (fileVersion == 0) {
      Structure st(getStructure());
      for (unsigned int i = 0; i < allEntries.size(); i++) {
         for (int j = 0; j < allEntries[i]->getNumberOfFiles(); j++) {
            allEntries[i]->files[j].structure = st;
         }
      }
   }
}

/**
 * read the spec file tags that are children of the element.
 */
void 
SpecFile::readTagsFromXML(QDomElement& topElement)  throw (FileException)
{
   const bool modFlag = getModified();
   FILE_FORMAT savedReadType = getFileReadType();
   setFileReadType(FILE_FORMAT_XML);
  
   QFile file;
   QTextStream textStream;
   QDataStream dataStream;
   readFileData(file, textStream, dataStream, topElement);
  
   setFileReadType(savedReadType);
   if (modFlag == false) {
      clearModified();
   }
}      


/**
 * Write the spec file's data.
 */
void
SpecFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& xmlDoc,
                                  QDomElement& rootElement) throw (FileException)
{
   bool streamValid = false;
   
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         streamValid = true;
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Writing in Binary format not supported.");     
         break;
      case FILE_FORMAT_XML:
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }
   
   if (streamValid) stream << "\n";
   
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      allEntries[i]->writeFiles(stream, xmlDoc, rootElement, getFileWriteType(), 
                                fileVersion, writeOnlySelectedFiles);
   }
   
   if (streamValid) stream << "\n";
   
   writeOnlySelectedFiles = false;
}

/**
 * write the spec file tags as children of the element.
 */
void 
SpecFile::writeTagsToXML(QDomDocument& xmlDoc,
                         QDomElement& topElement) throw (FileException)
{
   const bool modFlag = getModified();
   FILE_FORMAT savedWriteType = getFileWriteType();
   setFileWriteType(FILE_FORMAT_XML);
  
   QTextStream textStream;
   QDataStream dataStream;
   writeFileData(textStream, dataStream, xmlDoc, topElement);
  
   setFileWriteType(savedWriteType);
   if (modFlag == false) {
      clearModified();
   }
}                          

/// This class is used to sort files by the last modified time (used by spec file)
class SortedString {
   public:
      enum SORTING {
         SORTING_BY_DATE,
         SORTING_BY_NAME
      };
      static SORTING sortMethod;
      QString s1;
      QString s2;
      long time;
      
      static void setSortMethod(const SORTING s) { sortMethod = s; }
      static SORTING getSortMethod() { return sortMethod; }
      
      SortedString(const QString& filename, const QString filename2 = "") {
         s1 = filename;
         s2 = filename2;
         
         QFileInfo fi(s1);
         time = static_cast<long>(fi.lastModified().toTime_t());
      }
};
SortedString::SORTING SortedString::sortMethod = SortedString::SORTING_BY_DATE;

bool operator<(const SortedString& s1, const SortedString& s2) {
   switch (SortedString::getSortMethod()) {
      case SortedString::SORTING_BY_DATE:
         return (s1.time < s2.time);
         break;
      case SortedString::SORTING_BY_NAME:
         break;
         return (s1.s1 < s2.s1);
   }
   return false;
}
//bool operator>(const SortedString& s1, const SortedString& s2) {
//   return (s1.time > s2.time);
//}
bool sortSortedString(const SortedString& s1, const SortedString& s2) {
   switch (SortedString::getSortMethod()) {
      case SortedString::SORTING_BY_DATE:
         return (s1.time > s2.time);
         break;
      case SortedString::SORTING_BY_NAME:
         return (s1.s1 < s2.s1);
         break;
   }
   return false;
}

/**
 *
 */
void
SpecFile::setCurrentDirectoryToSpecFileDirectory()
{
   if (QDir::setCurrent(getFileNamePath()) == false) {
      std::cerr << "Unable to set path of current spec file, cannot sort by time" << std::endl;
      return;
   }
}

/**
 * Sort the different file types by date.  Note this only sorts the 
 * "files" and not their "selected" status.
 */
void
SpecFile::sortAllFilesByDate()
{
   const QString savedCurrentDirectory(QDir::currentPath());
   if (QDir::setCurrent(getFileNamePath()) == false) {
      std::cerr << "Unable to set path of current spec file, cannot sort by time" << std::endl;
      return;
   }
   
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      allEntries[i]->sort(SORT_DATE);
   }
   
   QDir::setCurrent(savedCurrentDirectory);
}

/**
 * Sort the different file types by name.  Note this only sorts the 
 * "files" and not their "selected" status.
 */
void
SpecFile::sortAllFilesByName()
{
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      allEntries[i]->sort(SORT_NAME);
   }
}

/**
 * Write a set of files associated with a tag.
 */
void
SpecFile::writeFiles(QTextStream& stream, 
                     QDomDocument& xmlDoc,
                     QDomElement& rootElement,
                     const QString& tagName, 
                     const std::vector<QString>& names,
                     const std::vector<SPEC_FILE_BOOL>& selected) const
{
   for (unsigned int i = 0; i < names.size(); i++) {
      if (names[i].isEmpty() == false) {
         bool writeIt = true;
         if (names[i][0] == '*') {
            writeIt = false;
         }
         if (writeOnlySelectedFiles && (selected[i] == SPEC_FALSE)) {
            writeIt = false;
         }
         if (writeIt) {
            switch (getFileWriteType()) {
               case FILE_FORMAT_ASCII:
                  stream << tagName << " " << names[i] << "\n";
                  break;
               case FILE_FORMAT_BINARY:
                  throw FileException(filename, "Writing in Binary format not supported.");     
                  break;
               case FILE_FORMAT_XML:
                  {
                     QDomElement tagElem = xmlDoc.createElement(tagName);
                     QDomElement tag1 = xmlDoc.createElement(xmlFileTagName1);
                     QDomCDATASection tagValue1 = xmlDoc.createCDATASection(names[i]);
                     tag1.appendChild(tagValue1);
                     tagElem.appendChild(tag1);
                     rootElement.appendChild(tagElem);
                  }
                  break;
               case FILE_FORMAT_XML_BASE64:
                  throw FileException(filename, "XML Base64 not supported.");
                  break;
               case FILE_FORMAT_XML_GZIP_BASE64:
                  throw FileException(filename, "XML GZip Base64 not supported.");
                  break;
               case FILE_FORMAT_OTHER:
                  throw FileException(filename, "Writing in Other format not supported.");
                  break;
               case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
                  throw FileException(filename, "Comma Separated Value File Format not supported.");
                  break;
            }
         }
      }
   }
}

/**
 * Write a set of files (two names) associated with a tag.
 */
void
SpecFile::writeFiles(QTextStream& stream, 
                     QDomDocument& xmlDoc,
                     QDomElement& rootElement,
                     const QString& tagName, 
                     const std::vector<QString>& names,
                     const std::vector<QString>& names2,
                     const std::vector<SPEC_FILE_BOOL>& selected) const
{
   for (unsigned int i = 0; i < names.size(); i++) {
      if (names[i].isEmpty() == false) {
         bool writeIt = true;
         if (names[i][0] == '*') {
            writeIt = false;
         }
         if (writeOnlySelectedFiles && (selected[i] == SPEC_FALSE)) {
            writeIt = false;
         }
         if (writeIt) {
            switch (getFileWriteType()) {
               case FILE_FORMAT_ASCII:
                  stream << tagName << " " << names[i];
                  if ((names2[i].isEmpty() == false) &&
                      (names2[i] != names[i])) {
                     stream << " " << names2[i];
                  }
                  stream << "\n";
                  break;
               case FILE_FORMAT_BINARY:
                  throw FileException(filename, "Writing in Binary format not supported.");     
                  break;
               case FILE_FORMAT_XML:
                  {
                     QDomElement tagElem = xmlDoc.createElement(tagName);
                     QDomElement tag1 = xmlDoc.createElement(xmlFileTagName1);
                     QDomCDATASection tagValue1 = xmlDoc.createCDATASection(names[i]);
                     tag1.appendChild(tagValue1);
                     tagElem.appendChild(tag1);
                     if ((names2[i].isEmpty() == false) &&
                         (names2[i] != names[i])) {
                        QDomElement tag2 = xmlDoc.createElement(xmlFileTagName2);
                        QDomCDATASection tagValue2 = xmlDoc.createCDATASection(names2[i]);
                        tag2.appendChild(tagValue2);
                        tagElem.appendChild(tag2);
                     }
                     rootElement.appendChild(tagElem);
                  }
                  break;
               case FILE_FORMAT_XML_BASE64:
                  throw FileException(filename, "XML Base64 not supported.");
                  break;
               case FILE_FORMAT_XML_GZIP_BASE64:
                  throw FileException(filename, "XML GZip Base64 not supported.");
                  break;
               case FILE_FORMAT_OTHER:
                  throw FileException(filename, "Writing in Other format not supported.");
                  break;
               case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
                  throw FileException(filename, "Comma Separated Value File Format not supported.");
                  break;
            }
         }
      }
   }
}

/**
 * Write a set of files with a set of tags.
 */
void
SpecFile::writeFiles(QTextStream& stream, 
                     QDomDocument& xmlDoc,
                     QDomElement& rootElement,
                     const std::vector<QString>& tagName, 
                     const std::vector<QString>& names,
                     const std::vector<SPEC_FILE_BOOL>& selected) const
{
   for (unsigned int i = 0; i < names.size(); i++) {
      if (names[i].isEmpty() == false) {
         bool writeIt = true;
         if (names[i][0] == '*') {
            writeIt = false;
         }
         if (writeOnlySelectedFiles && (selected[i] == SPEC_FALSE)) {
            writeIt = false;
         }
         if (writeIt) {
            switch (getFileWriteType()) {
               case FILE_FORMAT_ASCII:
                  stream << tagName[i] << " " << names[i] << "\n";
                  break;
               case FILE_FORMAT_BINARY:
                  throw FileException(filename, "Writing in Binary format not supported.");     
                  break;
               case FILE_FORMAT_XML:
                  {
                     QDomElement tagElem = xmlDoc.createElement(tagName[i]);
                     QDomElement tag1 = xmlDoc.createElement(xmlFileTagName1);
                     QDomCDATASection tagValue1 = xmlDoc.createCDATASection(names[i]);
                     tag1.appendChild(tagValue1);
                     tagElem.appendChild(tag1);
                     rootElement.appendChild(tagElem);
                  }
                  break;
               case FILE_FORMAT_XML_BASE64:
                  throw FileException(filename, "XML Base64 not supported.");
                  break;
               case FILE_FORMAT_XML_GZIP_BASE64:
                  throw FileException(filename, "XML GZip Base64 not supported.");
                  break;
               case FILE_FORMAT_OTHER:
                  throw FileException(filename, "Writing in Other format not supported.");
                  break;
               case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
                  throw FileException(filename, "Comma Separated Value File Format not supported.");
                  break;
            }
         }
      }
   }
}
/**
 * Write a file associated with a tag.
 */
void
SpecFile::writeFiles(QTextStream& stream, 
                     QDomDocument& xmlDoc,
                     QDomElement& rootElement,
                     const QString& tagName, 
                     const QString& name,
                     const SPEC_FILE_BOOL selected) const
{
   if (name.isEmpty() == false) {
      bool writeIt = true;
      if (name[0] == '*') {
         writeIt = false;
      }
      if (writeOnlySelectedFiles && (selected == SPEC_FALSE)) {
         writeIt = false;
      }
      if (writeIt) {
         switch (getFileWriteType()) {
            case FILE_FORMAT_ASCII:
               stream << tagName << " " << name << "\n";
               break;
            case FILE_FORMAT_BINARY:
               throw FileException(filename, "Writing in Binary format not supported.");     
               break;
            case FILE_FORMAT_XML:
               {
                  QDomElement tagElem = xmlDoc.createElement(tagName);
                  QDomElement tag1 = xmlDoc.createElement(xmlFileTagName1);
                  QDomCDATASection tagValue1 = xmlDoc.createCDATASection(name);
                  tag1.appendChild(tagValue1);
                  tagElem.appendChild(tag1);
                  rootElement.appendChild(tagElem);
               }
               break;
            case FILE_FORMAT_XML_BASE64:
               throw FileException(filename, "XML Base64 not supported.");
               break;
            case FILE_FORMAT_XML_GZIP_BASE64:
               throw FileException(filename, "XML GZip Base64 not supported.");
               break;
            case FILE_FORMAT_OTHER:
               throw FileException(filename, "Writing in Other format not supported.");
               break;
            case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
               throw FileException(filename, "Comma Separated Value File Format not supported.");
               break;
         }
      }
   }
}

/**
 * Select files for use by WebCaret.
 */
void 
SpecFile::setWebFileSelections()
{
   setAllFileSelections(SPEC_FALSE);

   if (closedTopoFile.files.size() > 0) {
      closedTopoFile.files[0].selected = SPEC_TRUE;
   }
   if (cutTopoFile.files.size() > 0) {
      cutTopoFile.files[0].selected = SPEC_TRUE;
   }

   if (fiducialCoordFile.files.size() > 0) {
      fiducialCoordFile.files[0].selected = SPEC_TRUE;
   }
 
   
   if (inflatedCoordFile.files.size() > 0) {
      inflatedCoordFile.files[0].selected = SPEC_TRUE;
   }
   
   if (veryInflatedCoordFile.files.size() > 0) {
      veryInflatedCoordFile.files[0].selected = SPEC_TRUE;
   }
   if (sphericalCoordFile.files.size() > 0) {
      sphericalCoordFile.files[0].selected = SPEC_TRUE;
   }
   
   if (flatCoordFile.files.size() > 0) {
      flatCoordFile.files[0].selected = SPEC_TRUE;
   }

   if (surfaceShapeFile.files.size() > 0) {
      surfaceShapeFile.files[0].selected = SPEC_TRUE;
   }

   if (areaColorFile.files.size() > 0) {
      areaColorFile.files[0].selected = SPEC_TRUE;
   }
   if (paintFile.files.size() > 0) {
      paintFile.files[0].selected = SPEC_TRUE;
   }
   if (metricFile.files.size() > 0) {
      metricFile.files[0].selected = SPEC_TRUE;
   } 
   if (rgbPaintFile.files.size() > 0) {
      rgbPaintFile.files[0].selected = SPEC_TRUE;
   }
   sceneFile.setAllSelections(SPEC_TRUE);
}

/**
 * read the spec file from a scene.
 */
void 
SpecFile::showScene(const SceneFile::Scene& scene, QString& /*errorMessage*/)
{
   clear();
   
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "SpecFile") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();
            const QString val      = si->getValueAsString();
            
            QString name1, name2;
            if (val.indexOf(':') == -1) {
               name1 = val;
            }
            else {
               std::vector<QString> tokens;
               StringUtilities::token(val, ":", tokens);
               if (tokens.size() > 0) {
                  name1 = tokens[0];
                  if (tokens.size() > 1) {
                     name2 = tokens[1];
                  }
               }
            }
            
            if (name1.isEmpty() == false) {
               std::vector<QString> tokens;
               tokens.push_back(infoName);
               tokens.push_back(name1);
               if (name2.isEmpty() == false) {
                  tokens.push_back(name2);
               }
            
               //
               // Do all but Scene Files
               //
               if (infoName != sceneFileTag) {
                  processTag(tokens);
               }
            }
         }
      }
   }
}

/**
 * write the spec file to a scene.
 */
void 
SpecFile::saveScene(SceneFile::Scene& scene, const bool selectedFilesOnlyFlag)
{
   SceneFile::SceneClass sc("SpecFile");

   for (unsigned int i = 0; i < allEntries.size(); i++) {
      allEntries[i]->saveScene(sc, selectedFilesOnlyFlag);
   }

   if (sc.getNumberOfSceneInfo() > 0) {
      scene.addSceneClass(sc);
   }
}    

/**
 * clear selection status for file name/selection pair.
 */
void 
SpecFile::clearSelectionStatus(const std::vector<QString>& specNames,
                               std::vector<SpecFile::SPEC_FILE_BOOL>& selectionStatus,
                               const QString fileName)
{
   for (unsigned int i = 0; i < specNames.size(); i++) {
      if (FileUtilities::basename(specNames[i]) == FileUtilities::basename(fileName)) {
         selectionStatus[i] = SPEC_FALSE;
         break;
      }
   }
}
                                       
/**
 * deselect any of "my files" if they are selected in the "other spec".
 */
void 
SpecFile::deselectFilesSelectedInOtherSpecFile(SpecFile& otherSpecFile)
{
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      allEntries[i]->deselectFilesOtherSpec(otherSpecFile);
   }
}

/** Returns true if "this" spec file is a subset of the "other" spec file.
 *  In other words, all of the files listed in this spec file are in 
 *  the other spec file.
 */
bool 
SpecFile::isSubsetOfOtherSpecFile(const SpecFile& otherSpecFile,
                                  QString& msg) const
{
   msg = "";
   
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      allEntries[i]->isSubset(otherSpecFile, msg);
   }
   
   return (msg.isEmpty());
}

/**
 * get all entries.
 */
void 
SpecFile::getAllEntries(std::vector<Entry>& allEntriesOut) 
{ 
   allEntriesOut.clear();
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      allEntriesOut.push_back(*(allEntries[i]));
   }
   std::sort(allEntriesOut.begin(), allEntriesOut.end());
}            

//!!***********************************************************************************
//!!***********************************************************************************
//!!***********************************************************************************
//!!***********************************************************************************

/**
 * constructor.
 */
SpecFile::Entry::Entry()
{
}
      
/**
 * destructor.
 */
SpecFile::Entry::~Entry()
{
}

/**
 * initialize.
 */
void 
SpecFile::Entry::initialize(const QString& descriptiveNameIn,
                            const QString& specFileTagIn,
                            const FILE_TYPE fileTypeIn,
                            const bool specFilePatternMatchFlagIn)
{
   descriptiveName = descriptiveNameIn;
   specFileTag = specFileTagIn;
   fileType    = fileTypeIn;
   specFilePatternMatchFlag = specFilePatternMatchFlagIn;
}
                  
/**
 * add if tag matches (returns true if tag matched).
 */
bool 
SpecFile::Entry::addFile(const QString& tagReadIn,
                         const QString& filenameIn,
                         const QString& dataFileNameIn,
                         const Structure& structureIn) 
{
   bool matches = false;
   if (specFilePatternMatchFlag) {
      if (tagReadIn.contains(specFileTag)) {
         matches = true;
      }
   }
   else if (tagReadIn == specFileTag) {
      matches = true;
   }

   if (matches) {
      if (specFileTag.isEmpty() == false) {
      
         //
         // Ignore if already present
         //
         for (unsigned int i = 0; i < files.size(); i++) {
            if (files[i].filename == filenameIn) {
               //
               // If already present, set its selection status and also 
               // override the data file name since a volume's data file
               // name may change if zipped or unzipped
               //
               files[i].selected = SPEC_TRUE;
               files[i].dataFileName = dataFileNameIn;
               return true;
            }
         }
         
         files.push_back(Files(filenameIn,
                               dataFileNameIn,
                               structureIn));
         return true;
      }
   }
   return false;
}
          
/**
 * clear selection status for a file.
 */
void 
SpecFile::Entry::clearSelectionStatus(const QString& filenameIn)
{
   for (unsigned int i = 0; i < files.size(); i++) {
      if (files[i].filename == filenameIn) {
         files[i].selected = SPEC_FALSE;
      }
   }
}

/**
 * Set file selected based upon name.
 * Returns true if the file was selected.
 */
bool
SpecFile::Entry::setSelected(const QString& name,
                             const bool addIfNotFound,
                             const Structure& structureIn)
{
   bool selected = false;
   if (name.isEmpty()) {
      return false;
   }
   for (unsigned int i = 0; i < files.size(); i++) {
      if (FileUtilities::basename(files[i].filename) == FileUtilities::basename(name)) {
         files[i].selected = SPEC_TRUE;
         selected = true;
      }
   }
   
   if (selected == false) {
      if (addIfNotFound) {
         files.push_back(Files(name, "", structureIn));
         selected = true;
      }
   }
   return selected;
}

/**
 * Returns true if all of myFiles are in otherFiles.  
 */
bool 
SpecFile::Entry::isSubset(const SpecFile& otherSpecFile,
                          QString& errorMessage) const
{
   bool isSubset = true;
   
   for (unsigned int m = 0; m < otherSpecFile.allEntries.size(); m++) {
      const Entry& otherEntry = *(otherSpecFile.allEntries[m]);
      if (otherEntry.specFileTag == specFileTag) {
         for (unsigned int i = 0; i < files.size(); i++) {
            bool found = false;
            for (unsigned int j = 0; j < otherEntry.files.size(); j++) {
               if (files[i].filename == otherEntry.files[j].filename) {
                  found = true;
                  break;
               }
            }
            
            if (found == false) {
               std::ostringstream str;
               str << "   " << descriptiveName.toAscii().constData() << " " << files[i].filename.toAscii().constData() << " is not in spec file but is in scene.\n";
               errorMessage.append(str.str().c_str());
               isSubset = false;
            }
         }
         return isSubset;
      }
   }
   
   return isSubset;
}

/**
 * add path to files.
 */
void 
SpecFile::Entry::prependPath(const QString& path,
                             const bool ignoreFilesWithAbsPaths)
{
   for (unsigned int i = 0; i < files.size(); i++) {
      QString f(files[i].filename);
      if (f.isEmpty() == false) {
         if (ignoreFilesWithAbsPaths) {
            if (FileUtilities::isAbsolutePath(f)) {
               continue;
            }
         }
         f = path;
         f.append("/");
         f.append(files[i].filename);
         files[i].filename = f;
      }
   }

   for (unsigned int i = 0; i < files.size(); i++) {
      QString f(files[i].dataFileName);
      if (f.isEmpty() == false) {
         if (ignoreFilesWithAbsPaths) {
            if (FileUtilities::isAbsolutePath(f)) {
               continue;
            }
         }
         f = path;
         f.append("/");
         f.append(files[i].dataFileName);
         files[i].dataFileName = f;
      }
   }
}

/**
 * add files to input vector.
 */
void 
SpecFile::Entry::getAllFiles(std::vector<QString>& allFiles)
{
   for (unsigned int i = 0; i < files.size(); i++) {
      allFiles.push_back(files[i].filename);
      if (files[i].dataFileName.isEmpty() == false) {
         allFiles.push_back(files[i].dataFileName);
      }
   }
}

/**
 * clear all members except type and tag.
 */
void 
SpecFile::Entry::clear(const bool removeFilesFromDiskToo)
{
   if (removeFilesFromDiskToo) {
      for (unsigned int i = 0; i < files.size(); i++) {
         QFile::remove(files[i].filename);
         if (files[i].dataFileName.isEmpty() == false) {
            QFile::remove(files[i].dataFileName);
         }
      }
   }
   
   files.clear();
}

/**
 * sort the files.
 */
void 
SpecFile::Entry::sort(const SORT sortMethod)
{
   Files::setSortMethod(sortMethod);
   std::sort(files.begin(), files.end());
}
            
/**
 * get number of files selected.
 */
int 
SpecFile::Entry::getNumberOfFilesSelected() const
{
   int cnt = 0;
   for (unsigned int i = 0; i < files.size(); i++) {
      if (files[i].selected) {
         cnt++;
      }
   }
   return cnt;
}

/**
 * make sure all files exist and are readable (returns true if valid).
 */
void 
SpecFile::Entry::validate(QString& errorMessage) const
{
   for (unsigned int i = 0; i < files.size(); i++) {
      QFileInfo fi1(files[i].filename);
      if (fi1.exists() == false) {
         errorMessage.append(FileUtilities::basename(files[i].filename));
         errorMessage.append(" does not exist.\n");
      }
      else if (fi1.isReadable() == false) {
         errorMessage.append(FileUtilities::basename(files[i].filename));
         errorMessage.append(" is not readable.\n");
      }

      QFileInfo fi2(files[i].dataFileName);
      if (fi2.exists() == false) {
         errorMessage.append(FileUtilities::basename(files[i].dataFileName));
         errorMessage.append(" does not exist.\n");
      }
      else if (fi2.isReadable() == false) {
         errorMessage.append(FileUtilities::basename(files[i].dataFileName));
         errorMessage.append(" is not readable.\n");
      }
   }
}

/**
 * set all file selections.
 */
void 
SpecFile::Entry::setAllSelections(const SPEC_FILE_BOOL selStatus)
{
   for (unsigned int i = 0; i < files.size(); i++) {
      files[i].selected = selStatus;
   }
}

/**
 * deselect files from other spec file.
 */
void 
SpecFile::Entry::deselectFilesOtherSpec(const SpecFile& otherSpecFile)
{
   if (getNumberOfFilesSelected() <= 0) {
      return;
   }
   
   //
   // Loop through other spec file's entries
   //
   for (unsigned int m = 0; m < otherSpecFile.allEntries.size(); m++) {
      const Entry& otherEntry = *(otherSpecFile.allEntries[m]);
      if (otherEntry.getNumberOfFiles() > 0) {
         if (otherEntry.specFileTag == specFileTag) {
            for (unsigned int i = 0; i < files.size(); i++) {
               if (files[i].selected == SPEC_TRUE) {
                  const QString myName = files[i].filename;
                  for (unsigned int j = 0; j < otherEntry.files.size(); j++) {
                     if (otherEntry.files[j].selected == SPEC_TRUE) {
                        if (myName == otherEntry.files[j].filename) {
                           files[i].selected = SPEC_FALSE;
                           break;
                        }
                     }
                  }
               }
            }
            //return;
         }
      }
   }
}                                        

/**
 * remove paths from all files.
 */
void 
SpecFile::Entry::removePaths()
{
   for (unsigned int i = 0; i < files.size(); i++) {
      files[i].filename = FileUtilities::basename(files[i].filename);
      files[i].dataFileName = FileUtilities::basename(files[i].dataFileName);
   }
}

/**
 * save scene.
 */
void 
SpecFile::Entry::saveScene(SceneFile::SceneClass& sc,
                           const bool selectedFilesOnlyFlag)
{
   for (unsigned int i = 0; i < files.size(); i++) {
      const Files f = files[i];
      bool addIt = true;
      if (selectedFilesOnlyFlag) {
         addIt = (f.selected == SPEC_TRUE);
      }
      
      if (addIt && (f.filename.isEmpty() == false)) {
         QString n(f.filename);
         if (f.dataFileName.isEmpty() == false) {
            n.append(":");
            n.append(f.dataFileName);
         }
         sc.addSceneInfo(SceneFile::SceneInfo(specFileTag, n));
      }
   }
}

/**
 * write the files.
 */
void 
SpecFile::Entry::writeFiles(QTextStream& stream,
                            QDomDocument& xmlDoc,
                            QDomElement& rootElement,
                            const FILE_FORMAT fileFormat,
                            const int fileVersionNumber,
                            const bool writeOnlySelectedFiles) throw (FileException)
{
   for (unsigned int i = 0; i < files.size(); i++) {
      bool writeIt = true;
      if (writeOnlySelectedFiles) {
         writeIt = files[i].selected;
      }
      
      //
      // Files flagged for deletion begin with a splat (asterisk)
      //
      if (files[i].filename.isEmpty() == false) {
         if (files[i].filename[0] == '*') {
            writeIt = false;
         }
      }
      
      if (writeIt) {
         switch (fileFormat) {
            case FILE_FORMAT_ASCII:
               stream << specFileTag << " ";
               if (fileVersionNumber > 0) {
                  stream << files[i].structure.getTypeAsAbbreviatedString() << " ";
               }
               stream << files[i].filename << " ";
               if (files[i].dataFileName.isEmpty() == false) {
                  if (files[i].filename != files[i].dataFileName) {
                     stream << files[i].dataFileName;
                  }
               }
               stream << "\n";
               break;
            case FILE_FORMAT_BINARY:
               throw FileException("", "Writing new spec in binary format not supported");
               break;
            case FILE_FORMAT_XML:
               {
                  QDomElement tagElem = xmlDoc.createElement(specFileTag);
                  
                  if (fileVersionNumber > 0) {
                     QDomElement structElement = xmlDoc.createElement(xmlFileTagStructure);
                     QDomCDATASection structValue = xmlDoc.createCDATASection(files[i].structure.getTypeAsAbbreviatedString());
                     structElement.appendChild(structValue);
                     tagElem.appendChild(structElement);
                  }
                  
                  QDomElement tag1 = xmlDoc.createElement(xmlFileTagName1);
                  QDomCDATASection tagValue1 = xmlDoc.createCDATASection(files[i].filename);
                  tag1.appendChild(tagValue1);
                  tagElem.appendChild(tag1);
                  if (files[i].dataFileName.isEmpty() == false) {
                     if (files[i].filename != files[i].dataFileName) {
                        QDomElement tag2 = xmlDoc.createElement(xmlFileTagName2);
                        QDomCDATASection tagValue2 = xmlDoc.createCDATASection(files[i].dataFileName);
                        tag2.appendChild(tagValue2);
                        tagElem.appendChild(tag2);
                     }
                  }
                  rootElement.appendChild(tagElem);
               }
               break;
            case FILE_FORMAT_XML_BASE64:
               throw FileException("", "Writing new spec in XML Base64 format not supported");
               break;
            case FILE_FORMAT_XML_GZIP_BASE64:
               throw FileException("", "Writing new spec in XML GZIP Base64 format not supported");
               break;
            case FILE_FORMAT_OTHER:
               throw FileException("", "Writing new spec in OTHER format not supported");
               break;
            case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
               throw FileException("", "Comma Separated Value File Format not supported.");
               break;
         }
      }
   }
}
                            
//!!***********************************************************************************
//!!***********************************************************************************
//!!***********************************************************************************
//!!***********************************************************************************

/**
 * constructor.
 */
SpecFile::Entry::Files::Files(const QString& filenameIn,
                                    const QString& dataFileNameIn,
                                    const Structure& structureIn,
                                    const SPEC_FILE_BOOL selectionStatus)
{
   filename = filenameIn;
   dataFileName = dataFileNameIn;
   structure = structureIn;
   selected = selectionStatus;
}
         
/**
 * destructor. 
 */
SpecFile::Entry::Files::~Files()
{
}

/**
 * comparison operator.
 */
bool 
SpecFile::Entry::Files::operator<(const Files& fi) const
{
   switch (sortMethod) {
      case SORT_NONE:
         return false;
         break;
      case SORT_DATE:
         {
            QFileInfo fi1(filename);
            const unsigned long time1 = static_cast<long>(fi1.lastModified().toTime_t());
            QFileInfo fi2(fi.filename);
            const unsigned long time2 = static_cast<long>(fi2.lastModified().toTime_t());
            return (time1 > time2);
         }
         break;
      case SORT_NAME:
         return (filename < fi.filename);
         break;
   }
   
   return false;
}
            
/**
 * sort the files.
 */
void 
SpecFile::Entry::Files::setSortMethod(const SORT sortMethodIn)
{
   sortMethod = sortMethodIn;
}



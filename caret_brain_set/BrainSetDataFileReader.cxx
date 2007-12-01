
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

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "BrainSetDataFileReader.h"
#include "FileUtilities.h"
#include "SpecFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainSetDataFileReader::BrainSetDataFileReader(BrainSet* bsIn,
                       const QString& specFileTagIn,
                       const QString& fileNameIn)
   : BrainModelAlgorithm(bsIn)
{
   specFileTag = specFileTagIn;
   fileName = fileNameIn;
}
                       
/**
 * destructor.
 */
BrainSetDataFileReader::~BrainSetDataFileReader()
{
}

/**
 * execute the algorithm.
 */
void 
BrainSetDataFileReader::execute() throw (BrainModelAlgorithmException)
{
   try {
      if (specFileTag == SpecFile::closedTopoFileTag) {
         brainSet->readTopologyFile(fileName,
                                    TopologyFile::TOPOLOGY_TYPE_CLOSED,
                                    true,
                                    true);
      }
      else if (specFileTag == SpecFile::openTopoFileTag) {
         brainSet->readTopologyFile(fileName,
                                    TopologyFile::TOPOLOGY_TYPE_OPEN,
                                    true,
                                    true);
      }
      else if (specFileTag == SpecFile::cutTopoFileTag) {
         brainSet->readTopologyFile(fileName,
                                    TopologyFile::TOPOLOGY_TYPE_CUT,
                                    true,
                                    true);
      }
      else if (specFileTag == SpecFile::lobarCutTopoFileTag) {
         brainSet->readTopologyFile(fileName,
                                    TopologyFile::TOPOLOGY_TYPE_LOBAR_CUT,
                                    true,
                                    true);
      }
      else if (specFileTag == SpecFile::unknownTopoFileMatchTag) {
         brainSet->readTopologyFile(fileName,
                                    TopologyFile::TOPOLOGY_TYPE_UNKNOWN,
                                    true,
                                    true);
      }
      else if (specFileTag == SpecFile::rawCoordFileTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_RAW,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::fiducialCoordFileTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::inflatedCoordFileTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_INFLATED,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::veryInflatedCoordFileTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_VERY_INFLATED,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::sphericalCoordFileTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::ellipsoidCoordFileTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::compressedCoordFileTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::flatCoordFileTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FLAT,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::lobarFlatCoordFileTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::hullCoordFileTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_HULL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::unknownCoordFileMatchTag) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_UNKNOWN,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::averageFiducialCoordFileTag) {
         // ignore
      }
      else if (specFileTag == SpecFile::rawSurfaceFileTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_RAW,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::fiducialSurfaceFileTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::inflatedSurfaceFileTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_INFLATED,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::veryInflatedSurfaceFileTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_VERY_INFLATED,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::sphericalSurfaceFileTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::ellipsoidSurfaceFileTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::compressedSurfaceFileTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::flatSurfaceFileTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FLAT,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::lobarFlatSurfaceFileTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::hullSurfaceFileTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_HULL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::unknownSurfaceFileMatchTag) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_UNKNOWN,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::volumeFunctionalFileTag) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_FUNCTIONAL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::volumePaintFileTag) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_PAINT,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::volumeProbAtlasFileTag) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_PROB_ATLAS,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::volumeRgbFileTag) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_RGB,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::volumeSegmentationFileTag) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_SEGMENTATION,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::volumeAnatomyFileTag) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_ANATOMY,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::volumeVectorFileTag) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_VECTOR,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::transformationMatrixFileTag) {
         brainSet->readTransformationMatrixFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::transformationDataFileTag) {
         brainSet->readTransformationDataFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::latLonFileTag) {
         brainSet->readLatLonFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::sectionFileTag) {
         brainSet->readSectionFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::paintFileTag) {
         brainSet->readPaintFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::areaColorFileTag) {
         brainSet->readAreaColorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::rgbPaintFileTag) {
         brainSet->readRgbPaintFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::surfaceVectorFileTag) {
         brainSet->readSurfaceVectorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::rawBorderFileTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_RAW,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::fiducialBorderFileTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::inflatedBorderFileTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_INFLATED,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::veryInflatedBorderFileTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_VERY_INFLATED,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::sphericalBorderFileTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::ellipsoidBorderFileTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::compressedBorderFileTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::flatBorderFileTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_FLAT,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::lobarFlatBorderFileTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::hullBorderFileTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_HULL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::unknownBorderFileMatchTag) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_UNKNOWN,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::volumeBorderFileTag) {
         brainSet->readVolumeBorderFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::borderColorFileTag) {
         brainSet->readBorderColorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::borderProjectionFileTag) {
         brainSet->readBorderProjectionFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::paletteFileTag) {
         brainSet->readPaletteFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::topographyFileTag) {
         brainSet->readTopographyFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::cellFileTag) {      
         brainSet->readCellFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::cellColorFileTag) {
         brainSet->readCellColorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::cellProjectionFileTag) {
         brainSet->readCellProjectionFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::volumeCellFileTag) {
         brainSet->readVolumeCellFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::contourFileTag) {
         brainSet->readContourFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::contourCellFileTag) {
         brainSet->readContourCellFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::contourCellColorFileTag) {
         brainSet->readContourCellColorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::atlasFileTag) {
         brainSet->readProbabilisticAtlasFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::metricFileTag) {
         brainSet->readMetricFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::surfaceShapeFileTag) {
         brainSet->readSurfaceShapeFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::cocomacConnectivityFileTag) {
         brainSet->readCocomacConnectivityFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::arealEstimationFileTag) {
         brainSet->readArealEstimationFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::cutsFileTag) {
         brainSet->readCutsFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::fociFileTag) {
         brainSet->readFociFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::fociColorFileTag) {
         brainSet->readFociColorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::fociProjectionFileTag) {
         brainSet->readFociProjectionFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::volumeFociFileTag) {
         brainSet->readVolumeFociFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::paramsFileTag) {
         brainSet->readParamsFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::deformationFieldFileTag) {
         brainSet->readDeformationFieldFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::vtkModelFileTag) {
         brainSet->readVtkModelFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::imageFileTag) {
         brainSet->readImageFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::sceneFileTag) {
         brainSet->readSceneFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::geodesicDistanceFileTag) {
         brainSet->readGeodesicDistanceFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::studyMetaDataFileTag) {
         brainSet->readStudyMetaDataFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::vocabularyFileTag) {
         brainSet->readVocabularyFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::wustlRegionFileTag) {
         brainSet->readWustlRegionFile(fileName,
                     true,
                     true);
      }
      else {
         const QString msg("Unrecognized spec file tag \""
                           + specFileTag
                           + "\" passed to BrainSetDataFileReader");
         throw BrainModelAlgorithmException(msg);
      }
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
}

/**
 * get a text description of algorithm.
 */
QString 
BrainSetDataFileReader::getTextDescription() const
{
   const QString msg("Reading " + FileUtilities::basename(fileName));
   return msg;
}

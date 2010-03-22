
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
      if (specFileTag == SpecFile::getClosedTopoFileTag()) {
         brainSet->readTopologyFile(fileName,
                                    TopologyFile::TOPOLOGY_TYPE_CLOSED,
                                    true,
                                    true);
      }
      else if (specFileTag == SpecFile::getOpenTopoFileTag()) {
         brainSet->readTopologyFile(fileName,
                                    TopologyFile::TOPOLOGY_TYPE_OPEN,
                                    true,
                                    true);
      }
      else if (specFileTag == SpecFile::getCutTopoFileTag()) {
         brainSet->readTopologyFile(fileName,
                                    TopologyFile::TOPOLOGY_TYPE_CUT,
                                    true,
                                    true);
      }
      else if (specFileTag == SpecFile::getLobarCutTopoFileTag()) {
         brainSet->readTopologyFile(fileName,
                                    TopologyFile::TOPOLOGY_TYPE_LOBAR_CUT,
                                    true,
                                    true);
      }
      else if (specFileTag == SpecFile::getUnknownTopoFileMatchTag()) {
         brainSet->readTopologyFile(fileName,
                                    TopologyFile::TOPOLOGY_TYPE_UNKNOWN,
                                    true,
                                    true);
      }
      else if (specFileTag == SpecFile::getRawCoordFileTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_RAW,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getFiducialCoordFileTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getInflatedCoordFileTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_INFLATED,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getVeryInflatedCoordFileTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_VERY_INFLATED,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getSphericalCoordFileTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getEllipsoidCoordFileTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getCompressedCoordFileTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getFlatCoordFileTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FLAT,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getLobarFlatCoordFileTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getHullCoordFileTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_HULL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getUnknownCoordFileMatchTag()) {
         brainSet->readCoordinateFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_UNKNOWN,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getAverageFiducialCoordFileTag()) {
         // ignore
      }
      else if (specFileTag == SpecFile::getRawSurfaceFileTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_RAW,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getFiducialSurfaceFileTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getInflatedSurfaceFileTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_INFLATED,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getVeryInflatedSurfaceFileTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_VERY_INFLATED,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getSphericalSurfaceFileTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getEllipsoidSurfaceFileTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getCompressedSurfaceFileTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getFlatSurfaceFileTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FLAT,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getLobarFlatSurfaceFileTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getHullSurfaceFileTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_HULL,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getUnknownSurfaceFileMatchTag()) {
         brainSet->readSurfaceFile(fileName,
                                      BrainModelSurface::SURFACE_TYPE_UNKNOWN,
                                      true,
                                      true,
                                      true);
      }
      else if (specFileTag == SpecFile::getVolumeFunctionalFileTag()) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_FUNCTIONAL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getVolumePaintFileTag()) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_PAINT,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getVolumeProbAtlasFileTag()) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_PROB_ATLAS,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getVolumeRgbFileTag()) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_RGB,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getVolumeSegmentationFileTag()) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_SEGMENTATION,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getVolumeAnatomyFileTag()) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_ANATOMY,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getVolumeVectorFileTag()) {
         brainSet->readVolumeFile(fileName,
                                  VolumeFile::VOLUME_TYPE_VECTOR,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getTransformationMatrixFileTag()) {
         brainSet->readTransformationMatrixFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getTransformationDataFileTag()) {
         brainSet->readTransformationDataFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getLatLonFileTag()) {
         brainSet->readLatLonFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getSectionFileTag()) {
         brainSet->readSectionFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getPaintFileTag()) {
         brainSet->readPaintFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getAreaColorFileTag()) {
         brainSet->readAreaColorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getRgbPaintFileTag()) {
         brainSet->readRgbPaintFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getVectorFileTag()) {
         brainSet->readVectorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getRawBorderFileTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_RAW,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getFiducialBorderFileTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getInflatedBorderFileTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_INFLATED,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getVeryInflatedBorderFileTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_VERY_INFLATED,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getSphericalBorderFileTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getEllipsoidBorderFileTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getCompressedBorderFileTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getFlatBorderFileTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_FLAT,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getLobarFlatBorderFileTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getHullBorderFileTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_HULL,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getUnknownBorderFileMatchTag()) {
         brainSet->readBorderFile(fileName,
                                  BrainModelSurface::SURFACE_TYPE_UNKNOWN,
                                  true,
                                  true);
      }
      else if (specFileTag == SpecFile::getVolumeBorderFileTag()) {
         brainSet->readVolumeBorderFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getBorderColorFileTag()) {
         brainSet->readBorderColorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getBorderProjectionFileTag()) {
         brainSet->readBorderProjectionFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getPaletteFileTag()) {
         brainSet->readPaletteFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getTopographyFileTag()) {
         brainSet->readTopographyFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getCellFileTag()) {      
         brainSet->readCellFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getCellColorFileTag()) {
         brainSet->readCellColorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getCellProjectionFileTag()) {
         brainSet->readCellProjectionFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getVolumeCellFileTag()) {
         brainSet->readVolumeCellFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getContourFileTag()) {
         brainSet->readContourFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getContourCellFileTag()) {
         brainSet->readContourCellFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getContourCellColorFileTag()) {
         brainSet->readContourCellColorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getAtlasFileTag()) {
         brainSet->readProbabilisticAtlasFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getMetricFileTag()) {
         brainSet->readMetricFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getSurfaceShapeFileTag()) {
         brainSet->readSurfaceShapeFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getCocomacConnectivityFileTag()) {
         brainSet->readCocomacConnectivityFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getArealEstimationFileTag()) {
         brainSet->readArealEstimationFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getCutsFileTag()) {
         brainSet->readCutsFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getFociFileTag()) {
         brainSet->readFociFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getFociColorFileTag()) {
         brainSet->readFociColorFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getFociProjectionFileTag()) {
         brainSet->readFociProjectionFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getFociSearchFileTag()) {
         brainSet->readFociSearchFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getParamsFileTag()) {
         brainSet->readParamsFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getDeformationFieldFileTag()) {
         brainSet->readDeformationFieldFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getVtkModelFileTag()) {
         brainSet->readVtkModelFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getImageFileTag()) {
         brainSet->readImageFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getSceneFileTag()) {
         brainSet->readSceneFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getGeodesicDistanceFileTag()) {
         brainSet->readGeodesicDistanceFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getStudyMetaDataFileTag()) {
         brainSet->readStudyMetaDataFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getVocabularyFileTag()) {
         brainSet->readVocabularyFile(fileName,
                     true,
                     true);
      }
      else if (specFileTag == SpecFile::getWustlRegionFileTag()) {
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

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


#define _GUI_DATA_FILE_DIALOG_MAIN_
#include "GuiDataFileDialog.h"
#undef  _GUI_DATA_FILE_DIALOG_MAIN_

#include "SpecFile.h"

/**
 * Constructor
 */
GuiDataFileDialog::GuiDataFileDialog()
{
   setFileMasks();
}

/**
 * Destructor
 */
GuiDataFileDialog::~GuiDataFileDialog()
{
}

/**
 * get all caret data file filters.
 */
void 
GuiDataFileDialog::getCaretFileFilters(QStringList& allFileFilters)
{
   allFileFilters.clear();
   allFileFilters += areaColorFileFilter;
   allFileFilters += arealEstimationFileFilter;
   allFileFilters += borderGenericFileFilter;
   allFileFilters += borderColorFileFilter;
   allFileFilters += borderProjectionFileFilter;
   allFileFilters += cellFileFilter;
   allFileFilters += cellColorFileFilter;
   allFileFilters += cellProjectionFileFilter;
   allFileFilters += cocomacFileFilter;
   allFileFilters += contourFileFilter;
   allFileFilters += contourCellFileFilter;
   allFileFilters += contourCellColorFileFilter;
   allFileFilters += coordinateGenericFileFilter;
   allFileFilters += cutsFileFilter;
   allFileFilters += deformationFieldFileFilter;
   allFileFilters += deformationMapFileFilter;
   allFileFilters += fociFileFilter;
   allFileFilters += fociColorFileFilter;
   allFileFilters += fociProjectionFileFilter;
   allFileFilters += geodesicDistanceFileFilter;
   allFileFilters += latitudeLongitudeFileFilter;
   allFileFilters += metricFileFilter;
   allFileFilters += paintFileFilter;
   allFileFilters += paletteFileFilter;
   allFileFilters += paramsFileFilter;
   allFileFilters += probAtlasFileFilter;
   allFileFilters += rgbPaintFileFilter;
   allFileFilters += sceneFileFilter;
   allFileFilters += sectionFileFilter;
   allFileFilters += specFileFilter;
   allFileFilters += studyMetaDataFileFilter;
   allFileFilters += surfaceShapeFileFilter;
   allFileFilters += surfaceVectorFileFilter;
   allFileFilters += topographyFileFilter;
   allFileFilters += topologyGenericFileFilter;
   allFileFilters += transformationMatrixFileFilter;
   allFileFilters += vocabularyFileFilter;
   allFileFilters += wustlRegionFileFilter;
}      

/**
 * Setup the file masks
 */
void 
GuiDataFileDialog::setFileMasks()
{
   if (fileMasksValid) {
      return;
   }
   else {
      fileMasksValid = true;
   }
   
   areaColorFileFilter = QString("Area Color File (*%1)").arg(SpecFile::getAreaColorFileExtension());
   
   arealEstimationFileFilter = QString("Areal Estimation Files (*%1)").arg(SpecFile::getArealEstimationFileExtension());
   
   borderRawFileFilter = QString("Border Files RAW (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderFiducialFileFilter = QString("Border Files FIDUCIAL (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderInflatedFileFilter = QString("Border Files INFLATED (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderVeryInflatedFileFilter = QString("Border Files VERY INFLATED (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderSphericalFileFilter = QString("Border Files SPHERICAL (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderEllipsoidalFileFilter = QString("Border Files ELLIPSOIDAL (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderCompressedFileFilter = QString("Border Files COMPRESSED (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderFlatFileFilter = QString("Border Files FLAT (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderFlatLobarFileFilter = QString("Border Files FLAT LOBAR (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderHullFileFilter = QString("Border Files HULL (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderUnknownFileFilter = QString("Border Files UNKNOWN (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderGenericFileFilter = QString("Border Files - Surface (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderVolumeFileFilter = QString("Border Files - Volume (*%1)").arg(SpecFile::getBorderFileExtension());
   
   borderColorFileFilter = QString("Border Color Files (*%1)").arg(SpecFile::getBorderColorFileExtension());
   
   borderProjectionFileFilter = QString("Border Projection Files (*%1)").arg(SpecFile::getBorderProjectionFileExtension());
   
   cellFileFilter = QString("Cell Files (*%1)").arg(SpecFile::getCellFileExtension());
   
   cellColorFileFilter = QString("Cell Color Files (*%1)").arg(SpecFile::getCellColorFileExtension());
   
   cellProjectionFileFilter = QString("Cell Projection Files (*%1)").arg(SpecFile::getCellProjectionFileExtension());
   
   cellVolumeFileFilter = QString("Cell Files - Volume (*%1)").arg(SpecFile::getCellFileExtension());
   
   cocomacFileFilter = QString("CoCoMac Files (*%1 *.xml)").arg(SpecFile::getCocomacConnectivityFileExtension());
   
   contourFileFilter = QString("Contour Files (*%1)").arg(SpecFile::getContourFileExtension());
   
   contourCellFileFilter = QString("Contour Cell Files (*%1)").arg(SpecFile::getContourCellFileExtension());
   
   contourCellColorFileFilter = QString("Contour Cell Color Files (*%1)").arg(SpecFile::getContourCellColorFileExtension());
   
   coordinateRawFileFilter = QString("Coordinate Files RAW (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateFiducialFileFilter = QString("Coordinate Files FIDUCIAL (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateInflatedFileFilter = QString("Coordinate Files INFLATED (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateVeryInflatedFileFilter = QString("Coordinate Files VERY INFLATED (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateSphericalFileFilter = QString("Coordinate Files SPHERICAL (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateEllipsoidalFileFilter = QString("Coordinate Files ELLIPSOIDAL (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateCompressedFileFilter = QString("Coordinate Files COMPRESSED (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateFlatFileFilter = QString("Coordinate Files FLAT (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateFlatLobarFileFilter = QString("Coordinate Files FLAT LOBAR (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateHullFileFilter = QString("Coordinate Files HULL (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateUnknownFileFilter = QString("Coordinate Files UNKNOWN (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   coordinateGenericFileFilter = QString("Coordinate Files (*%1)").arg(SpecFile::getCoordinateFileExtension());
   
   cutsFileFilter = QString("Cuts Files (*%1)").arg(SpecFile::getCutsFileExtension());

   deformationFieldFileFilter = QString("Deformation Field Files (*%1)").arg(SpecFile::getDeformationFieldFileExtension());
   
   deformationMapFileFilter = QString("Deformation Map Files (*%1)").arg(SpecFile::getDeformationMapFileExtension());
   
   fociFileFilter = QString("Foci Files (*%1)").arg(SpecFile::getFociFileExtension());
   
   fociColorFileFilter = QString("Foci Color Files (*%1)").arg(SpecFile::getFociColorFileExtension());
   
   fociProjectionFileFilter = QString("Foci Projection Files (*%1)").arg(SpecFile::getFociProjectionFileExtension());
   
   fociVolumeFileFilter = QString("Foci Files - Volume (*%1)").arg(SpecFile::getFociFileExtension());
   
   geodesicDistanceFileFilter = QString("Geodesic Distance Files (*%1)").arg(SpecFile::getGeodesicDistanceFileExtension());
   
   latitudeLongitudeFileFilter = QString("Latitude Longitude Files (*%1)").arg(SpecFile::getLatLonFileExtension());
   
   metricFileFilter = QString("Metric Files (*%1)").arg(SpecFile::getMetricFileExtension());
   
   metricShapeFileFilter = QString("Metric/Shape Files (*%1 *%2)").arg(SpecFile::getMetricFileExtension()).arg(SpecFile::getSurfaceShapeFileExtension());
   
   paintFileFilter = QString("Paint Files (*%1)").arg(SpecFile::getPaintFileExtension());
   
   paletteFileFilter = QString("Palette Files (*%1 *.pal)").arg(SpecFile::getPaletteFileExtension());
   
   paramsFileFilter = QString("Params Files (*%1)").arg(SpecFile::getParamsFileExtension());
   
   probAtlasFileFilter = QString("Probabilistic Atlas Files (*%1 *atlas.paint*)").arg(SpecFile::getProbabilisticAtlasFileExtension());
   
   rgbPaintFileFilter = QString("RGB Paint Files (*%1)").arg(SpecFile::getRgbPaintFileExtension());
   
   sceneFileFilter = QString("Scene Files (*%1)").arg(SpecFile::getSceneFileExtension());
   
   sectionFileFilter = QString("Section Files (*%1)").arg(SpecFile::getSectionFileExtension());
   
   surfaceShapeFileFilter = QString("Surface Shape Files (*%1)").arg(SpecFile::getSurfaceShapeFileExtension());
   
   surfaceVectorFileFilter = QString("Surface Vector Files (*%1)").arg(SpecFile::getSurfaceVectorFileExtension());
   
   topographyFileFilter = QString("Topography Files (*%1)").arg(SpecFile::getTopographyFileExtension());
   
   topologyClosedFileFilter = QString("Topology Files CLOSED (*%1)").arg(SpecFile::getTopoFileExtension());
   
   topologyOpenFileFilter = QString("Topology Files OPEN (*%1)").arg(SpecFile::getTopoFileExtension());
   
   topologyCutFileFilter = QString("Topology Files CUT (*%1)").arg(SpecFile::getTopoFileExtension());
   
   topologyCutLobarFileFilter = QString("Topology Files CUT LOBAR (*%1)").arg(SpecFile::getTopoFileExtension());
   
   topologyUnknownFileFilter = QString("Topology Files UNKNOWN (*%1)").arg(SpecFile::getTopoFileExtension());
   
   topologyGenericFileFilter = QString("Topology Files (*%1)").arg(SpecFile::getTopoFileExtension());
   
   transformationMatrixFileFilter = QString("Transformation Matrix Files (*%1)").arg(SpecFile::getTransformationMatrixFileExtension());

   transformationDataFileFilter = QString("Transformation Data Files (*%1 *%2 *%3 *.vtp *%4 *%5)"
                                                ).arg(SpecFile::getCellFileExtension()).arg(
                                                      SpecFile::getFociFileExtension()).arg(
                                                      SpecFile::getVtkModelFileExtension()).arg(
                                                      SpecFile::getContourFileExtension()).arg(
                                                      SpecFile::getContourCellFileExtension());
                       
   QString niftiGzipExtension(SpecFile::getNiftiVolumeFileExtension());
   niftiGzipExtension += ".gz";
   
   volumeAnatomyFileFilter = QString("Volume Anatomy Files (*%1 *%2 *%3 *%4 *%5)").arg(
                                               SpecFile::getAfniVolumeFileExtension()).arg(
                                               SpecFile::getAnalyzeVolumeFileExtension()).arg(
                                               SpecFile::getNiftiVolumeFileExtension()).arg(
                                               niftiGzipExtension).arg(
                                               SpecFile::getWustlVolumeFileExtension()); 
   
   volumeFunctionalFileFilter = QString("Volume Functional Files (*%1 *%2 *%3 *%4 *%5)").arg(
                                               SpecFile::getAfniVolumeFileExtension()).arg(
                                               SpecFile::getAnalyzeVolumeFileExtension()).arg(
                                               SpecFile::getNiftiVolumeFileExtension()).arg(
                                               niftiGzipExtension).arg(
                                               SpecFile::getWustlVolumeFileExtension());
   
   volumePaintFileFilter = QString("Volume Paint Files (*%1 *%2 *%3 *%4 *%5)").arg(
                                               SpecFile::getAfniVolumeFileExtension()).arg(
                                               SpecFile::getAnalyzeVolumeFileExtension()).arg(
                                               SpecFile::getNiftiVolumeFileExtension()).arg(
                                               niftiGzipExtension).arg(
                                               SpecFile::getWustlVolumeFileExtension());
   
   volumeProbAtlasFileFilter = QString("Volume Prob Atlas Files (*%1 *%2 *%3 *%4 *%5)").arg(
                                               SpecFile::getAfniVolumeFileExtension()).arg(
                                               SpecFile::getAnalyzeVolumeFileExtension()).arg(
                                               SpecFile::getNiftiVolumeFileExtension()).arg(
                                               niftiGzipExtension).arg(
                                               SpecFile::getWustlVolumeFileExtension());
   
   volumeRgbFileFilter = QString("Volume Rgb Files (*%1 *%2 *%3 *%4 *%5)").arg(
                                               SpecFile::getAfniVolumeFileExtension()).arg(
                                               SpecFile::getAnalyzeVolumeFileExtension()).arg(
                                               SpecFile::getNiftiVolumeFileExtension()).arg(
                                               niftiGzipExtension).arg(
                                               SpecFile::getWustlVolumeFileExtension());
   
   volumeSegmentationFileFilter = QString("Volume Segmentation Files (*%1 *%2 *%3 *%4 *%5)").arg(
                                               SpecFile::getAfniVolumeFileExtension()).arg(
                                               SpecFile::getAnalyzeVolumeFileExtension()).arg(
                                               SpecFile::getNiftiVolumeFileExtension()).arg(
                                               niftiGzipExtension).arg(
                                               SpecFile::getWustlVolumeFileExtension());
   
   volumeVectorFileFilter = QString("Volume Vector Files (*%1 *%2 *%3 *%4 *%5)").arg(
                                               SpecFile::getAfniVolumeFileExtension()).arg(
                                               SpecFile::getAnalyzeVolumeFileExtension()).arg(
                                               SpecFile::getNiftiVolumeFileExtension()).arg(
                                               niftiGzipExtension).arg(
                                               SpecFile::getWustlVolumeFileExtension());
   
   volumeGenericFileFilter = QString("Volume Files (*%1 *%2 *%3 *%4 *%5)").arg(
                                               SpecFile::getAfniVolumeFileExtension()).arg(
                                               SpecFile::getAnalyzeVolumeFileExtension()).arg(
                                               SpecFile::getNiftiVolumeFileExtension()).arg(
                                               niftiGzipExtension).arg(
                                               SpecFile::getWustlVolumeFileExtension());
   
   volumeFileAfniFilter = QString("Volume File - AFNI (*%1)").arg(SpecFile::getAfniVolumeFileExtension());
   
   volumeMultiFileAfniFilter = QString("Volume Files MULTIPLE - AFNI (*%1)").arg(SpecFile::getAfniVolumeFileExtension());
   
   volumeFileNiftiReadFilter = QString("Volume Files - NIFTI (*%1 *%2 %3)").arg(SpecFile::getAnalyzeVolumeFileExtension()).arg(
                                                                 SpecFile::getNiftiVolumeFileExtension()).arg(
                                                                 niftiGzipExtension);
   
   volumeFileNiftiWriteFilter = QString("Volume Files - NIFTI (*%1 *%2)").arg(SpecFile::getNiftiVolumeFileExtension()).arg(
                                                                 niftiGzipExtension);
   
   volumeMultiFileNiftiFilter = QString("Volume Files MULTIPLE - NIFTI (*%1 *%2)").arg(SpecFile::getNiftiVolumeFileExtension()).arg(
                                                                 niftiGzipExtension);
   
   volumeFileSpmMedxFilter = QString("Volume Files - SPM and MEDx (*%1)").arg(SpecFile::getAnalyzeVolumeFileExtension());
   
   volumeFileWuNilFilter = QString("Volume Files - WUNIL (*%1)").arg(SpecFile::getWustlVolumeFileExtension());
   
   vtkModelFileFilter = QString("VTK Model File (*%1 *.vtp)").arg(SpecFile::getVtkModelFileExtension());
   
   vocabularyFileFilter = QString("Vocabulary File (*%1)").arg(SpecFile::getVocabularyFileExtension());
   
   wustlRegionFileFilter = QString("Wustl Region File (*%1)").arg(SpecFile::getWustlRegionFileExtension());

   metaAnalysisFileFilter = QString("Meta-Analysis File (*%1)").arg(SpecFile::getStudyMetaAnalysisFileExtension());
   
   studyMetaDataFileFilter = QString("Study Metadata File (*%1)").arg(SpecFile::getStudyMetaDataFileExtension());
   
   specFileFilter = QString("Spec File (*%1)").arg(SpecFile::getSpecFileExtension());
   
   mdPlotFileFilter = QString("MD Plot File (*%1)").arg(SpecFile::getMDPlotFileExtension());
   
   neurolucidaFileFilter = QString("Neurolucida File (*%1)").arg(SpecFile::getNeurolucidaFileExtension());
   
   brainVoyagerFileFilter = QString("Brain Voyager File (*%1)").arg(SpecFile::getBrainVoyagerFileExtension());
   
   vtkSurfaceFileFilter = QString("VTK Surface File (*%1)").arg(SpecFile::getVtkModelFileExtension());
   
   vtkXmlSurfaceFileFilter = "VTK XML Surface File (*.vtp)";
   
   byuSurfaceFileFilter = "BYU Surface File (*.byu)";
   
   stlSurfaceFileFilter = "STL Surface File (*.stl)";
   
   studio3DSurfaceFileFilter = "3D Studio Max Ascii Scene (*.ase)";
   
   wavefrontObjFileFilter = "Wavefront OBJ Surface File (*.obj)";
   
   vrmlSurfaceFileFilter = "VRML Surface File (*.wrl)";
   
   sumaRgbFileFilter = "SUMA RGB file (*.col)";
   
   textFileFilter = "Text File (*.txt *.text)";
   
   commaSeparatedValuesFileFilter = QString("Comma Separated Value File (*%1)").arg(SpecFile::getCommaSeparatedValueFileExtension());
   
   openInventorSurfaceFileFilter = "Open Inventor Surface File (*.iv)";
   
   freeSurferAsciiSurfaceFileFilter = QString("Free Surfer Ascii Surface File (*%1)").arg(SpecFile::getFreeSurferAsciiSurfaceFileExtension());
   
   freeSurferBinarySurfaceFileFilter = QString("Free Surfer Binary Surface File (*)");
   
   freeSurferAsciiCurvatureFileFilter = 
          QString("Free Surface Ascii Curvature File As Surface Shape (*%1)").arg(SpecFile::getFreeSurferAsciiCurvatureFileExtension());
   
   freeSurferBinaryCurvatureFileFilter = 
          QString("Free Surface Binary Curvature File As Surface Shape (*%1)").arg(SpecFile::getFreeSurferBinaryCurvatureFileExtension());
   
   freeSurferAsciiFunctionalFileFilter =
          QString("Free Surfer Ascii Functional File As Metric(*%1)").arg(SpecFile::getFreeSurferAsciiFunctionalFileExtension());
   
   freeSurferBinaryFunctionalFileFilter =
          QString("Free Surfer Binary Functional File As Metric(*%1)").arg(SpecFile::getFreeSurferBinaryFunctionalFileExtension());
   
   freeSurferAsciiLabelFileFilter = 
          QString("Free Surfer Ascii Label File As Paint(*%1)").arg(SpecFile::getFreeSurferLabelFileExtension());
          
   analyzeVolumeFileFilter = QString("Analyze Volume File (*%1)").arg(SpecFile::getAnalyzeVolumeFileExtension());
   
   mincVolumeFileFilter = QString("MINC Volume File (*%1)").arg(SpecFile::getMincVolumeFileExtension());

   vtkVolumeFileFilter = QString("VTK Volume File (*%1)").arg(SpecFile::getVtkModelFileExtension());
   
   rawVolumeFileFilter = "Raw Volume File (*)";
   
   giftiSurfaceFileFilter = QString("GIFTI Surface File (*%1)").arg(SpecFile::getGiftiFileExtension());
   
   giftiFunctionalFileFilter = QString("GIFTI Functional File (*%1)").arg(SpecFile::getGiftiFileExtension());
   
   giftiShapeFileFilter = QString("GIFTI Shape File (*%1)").arg(SpecFile::getGiftiFileExtension());
   
   giftiLabelFileFilter = QString("GIFTI Label File (*%1)").arg(SpecFile::getGiftiFileExtension());
   
/*
   volumeAnatomyFileFilter = "Volume Anatomy Files (*.HEAD *.hdr *.ifh)";
   
   volumeFunctionalFileFilter = "Volume Functional Files (*.HEAD *.hdr *.ifh)";
   
   volumePaintFileFilter = "Volume Paint Files (*.HEAD *.hdr *.ifh)";
   
   volumeRgbFileFilter = "Volume Rgb Files (*.HEAD *.hdr *.ifh)";
   
   volumeSegmentationFileFilter = "Volume Segmentation Files (*.HEAD *.hdr *.ifh)";
   
   volumeFileAfniFilter = "Volume Files - AFNI (*.HEAD)";
   
   volumeFileSpmMedxFilter = "Volume Files - SPM and MEDx (*.hdr)";
   
   volumeFileWuNilFilter = "Volume Files - WUNIL (*.ifh)";
   
   vtkModelFileFilter = "VTK Model File (*.vtk *.vtp)";
   
   brainVoyagerFileFilter = "Brain Voyager File (*.srf)";
   
   vtkSurfaceFileFilter = "VTK Surface File (*.vtk)";
   
   vtkXmlSurfaceFileFilter = "VTK XML Surface File (*.vtp)";
   
   byuSurfaceFileFilter = "BYU Surface File (*.byu)";
   
   stlSurfaceFileFilter = "STL Surface File (*.stl)";
   
   vrmlSurfaceFileFilter = "VRML Surface File (*.wrl)";
   
   openInventorSurfaceFileFilter = "Open Inventor Surface File (*.iv)";
   
   freeSurferAsciiSurfaceFileFilter = "Free Surfer Ascii Surface File (*.asc)";
   
   freeSurferAsciiCurvatureFileFilter = 
          "Free Surface Ascii Curvature File As Surface Shape (*.curve.asc)";
   
   freeSurferAsciiFunctionalFileFilter =
          "Free Surfer Ascii Functional File As Metric(*.w.asc)";
   
   freeSurferAsciiLabelFileFilter = 
          "Free Surfer Ascii Label File As Paint(*.label)";
          
   analyzeVolumeFileFilter = "Analyze Volume File (*.hdr)";
   
   mincVolumeFileFilter = "MINC Volume File (*.mnc)";

   vtkVolumeFileFilter = "VTK Volume File (*.vtk)";
*/
}


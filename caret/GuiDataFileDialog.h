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


#ifndef __VE_GUI_DATA_FILE_DIALOG_H__
#define __VE_GUI_DATA_FILE_DIALOG_H__

#include <QString>
#include <QStringList>

/// Class that is parent of data file open and save dialogs.
class GuiDataFileDialog {
   public:
      /// Constructor
      GuiDataFileDialog();
      
      /// Destructor
      virtual ~GuiDataFileDialog();
      
      /// get all caret data file filters
      static void getCaretFileFilters(QStringList& allFileFilters);
      
      /// area color data file filter
      static QString areaColorFileFilter;
      
      /// areal estimation data file filter
      static QString arealEstimationFileFilter;
      
      /// border raw file data file filter
      static QString borderRawFileFilter;
      
      /// border fiducial file data file filter
      static QString borderFiducialFileFilter;
      
      /// border inflated file data file filter
      static QString borderInflatedFileFilter;
      
      /// border very inflated file data file filter
      static QString borderVeryInflatedFileFilter;
      
      /// border spherical file data file filter
      static QString borderSphericalFileFilter;
      
      /// border ellipsoidal file data file filter
      static QString borderEllipsoidalFileFilter;
      
      /// border compressed file data file filter
      static QString borderCompressedFileFilter;
      
      /// border flat file data file filter
      static QString borderFlatFileFilter;
      
      /// border flat lobar file data file filter
      static QString borderFlatLobarFileFilter;
      
      /// border hull file data file filter
      static QString borderHullFileFilter;
      
      /// border unknown file data file filter
      static QString borderUnknownFileFilter;
      
      /// border generic file data filter
      static QString borderGenericFileFilter;
      
      /// border volume file data file filter
      static QString borderVolumeFileFilter;
      
      /// border color data file filter
      static QString borderColorFileFilter;
      
      /// border projection data file filter
      static QString borderProjectionFileFilter;
      
      /// cell file data file filter
      static QString cellFileFilter;
      
      /// cell color data file filter
      static QString cellColorFileFilter;
      
      /// cell projection data file filter
      static QString cellProjectionFileFilter;
      
      /// cell volume file data file filter
      static QString cellVolumeFileFilter;
      
      /// cocomac data file filter
      static QString cocomacFileFilter;
      
      /// contour data file filter
      static QString contourFileFilter;
      
      /// contour cell data file filter
      static QString contourCellFileFilter;
      
      /// contour cell color data file filter
      static QString contourCellColorFileFilter;
      
      /// coordinate raw data file filter
      static QString coordinateRawFileFilter;
      
      /// coordinate fiducial data file filter
      static QString coordinateFiducialFileFilter;
      
      /// coordinate inflated data file filter
      static QString coordinateInflatedFileFilter;
      
      /// coordinate very inflated data file filter
      static QString coordinateVeryInflatedFileFilter;
      
      /// coordinate spherical data file filter
      static QString coordinateSphericalFileFilter;
      
      /// coordinate ellipsoidal data file filter
      static QString coordinateEllipsoidalFileFilter;
      
      /// coordinate compresed data file filter
      static QString coordinateCompressedFileFilter;
      
      /// coordinate flat data file filter
      static QString coordinateFlatFileFilter;
      
      /// coordinate flat lobar data file filter
      static QString coordinateFlatLobarFileFilter;
      
      /// coordinate unknown data file filter
      static QString coordinateUnknownFileFilter;
      
      /// coordinate generic data file filter
      static QString coordinateGenericFileFilter;
      
      /// coordinate hull data file filter
      static QString coordinateHullFileFilter;
      
      /// cuts data file filter
      static QString cutsFileFilter;
      
      /// deformation field file filter
      static QString deformationFieldFileFilter;
      
      /// deformation map file filter
      static QString deformationMapFileFilter;
      
      /// foci data file filter
      static QString fociFileFilter;
      
      /// foci color data file filter
      static QString fociColorFileFilter;
      
      /// foci projection data file filter
      static QString fociProjectionFileFilter;
      
      /// foci volume file data file filter
      static QString fociVolumeFileFilter;
      
      /// geodesic distance data file filter
      static QString geodesicDistanceFileFilter;
      
      /// latitude longitude data file filter
      static QString latitudeLongitudeFileFilter;
      
      /// metric data file filter
      static QString metricFileFilter;
      
      /// metric and shape data file filter
      static QString metricShapeFileFilter;
      
      /// paint data file filter
      static QString paintFileFilter;
      
      /// palette data file filter
      static QString paletteFileFilter;
      
      /// params data file filter
      static QString paramsFileFilter;
      
      /// prob atlas data file filter
      static QString probAtlasFileFilter;
      
      /// rgb paint data file filter
      static QString rgbPaintFileFilter;
      
      /// scene data file filter
      static QString sceneFileFilter;
      
      /// section data file filter
      static QString sectionFileFilter;
      
      /// surface shape data file filter
      static QString surfaceShapeFileFilter;
      
      /// surface vector data file filter
      static QString surfaceVectorFileFilter;
      
      /// topography data file filter
      static QString topographyFileFilter;
      
      /// topology closed data file filter
      static QString topologyClosedFileFilter;
      
      /// topology open data file filter
      static QString topologyOpenFileFilter;
      
      /// topology cut data file filter
      static QString topologyCutFileFilter;
      
      /// topology cut lobar data file filter
      static QString topologyCutLobarFileFilter;
      
      /// topology unknown data file filter
      static QString topologyUnknownFileFilter;
      
      /// topology generic data file filter
      static QString topologyGenericFileFilter;
      
      /// transformation matrix file filter
      static QString transformationMatrixFileFilter;
      
      /// transformation data file filter
      static QString transformationDataFileFilter;
      
      /// volume data afni file filter
      static QString volumeFileAfniFilter;
      
      /// multiple volume data afni file filter
      static QString volumeMultiFileAfniFilter;
      
      /// multiple volume data nifti file filter
      static QString volumeMultiFileNiftiFilter;
      
      /// volume data gifti file read filter
      static QString volumeFileNiftiReadFilter;
      
      /// volume data gifti file write filter
      static QString volumeFileNiftiWriteFilter;
      
      /// volume data spm and medx file filter
      static QString volumeFileSpmMedxFilter;
      
      /// volume data wunil file filter
      static QString volumeFileWuNilFilter;
      
      /// volume data anatomy file filter
      static QString volumeAnatomyFileFilter;
      
      /// volume data functional file filter
      static QString volumeFunctionalFileFilter;
      
      /// volume data paint file filter
      static QString volumePaintFileFilter;
      
      /// volume data prob atlas file filter
      static QString volumeProbAtlasFileFilter;
      
      /// volume data rgb file filter
      static QString volumeRgbFileFilter;
      
      /// volume data segmentation file filter
      static QString volumeSegmentationFileFilter;
      
      /// volume data vector file filter
      static QString volumeVectorFileFilter;
      
      /// volume data generic file filter
      static QString volumeGenericFileFilter;
      
      /// vtk model file filter
      static QString vtkModelFileFilter;
      
      /// wustl region file filter
      static QString wustlRegionFileFilter;
      
      /// vocabulary file filter
      static QString vocabularyFileFilter;
      
      /// study meta data file filter
      static QString studyMetaDataFileFilter;
      
      /// meta-analysis file filter
      static QString metaAnalysisFileFilter;
      
      /// spec file filter
      static QString specFileFilter;
      
      /// brian voyager file filter
      static QString brainVoyagerFileFilter;
      
      /// MD Plot file filter
      static QString mdPlotFileFilter;
      
      /// Neurolucida file filter
      static QString neurolucidaFileFilter;
      
      /// vtk surface file filter
      static QString vtkSurfaceFileFilter;
      
      /// vtk XML polydata file filter
      static QString vtkXmlSurfaceFileFilter;
      
      /// byu file filter
      static QString byuSurfaceFileFilter;
      
      /// stl file filter
      static QString stlSurfaceFileFilter;
      
      /// 3D Studio file filter
      static QString studio3DSurfaceFileFilter;
      
      /// vrml file filter
      static QString vrmlSurfaceFileFilter;
      
      /// wavefront object file filter
      static QString wavefrontObjFileFilter;
      
      /// sums rgb file filter
      static QString sumaRgbFileFilter;
      
      /// open inventor file filter
      static QString openInventorSurfaceFileFilter;
      
      /// free surfer binary surface file filter
      static QString freeSurferBinarySurfaceFileFilter;
      
      /// free surfer ascii surface file filter
      static QString freeSurferAsciiSurfaceFileFilter;
      
      /// free surfer ascii curvature file filter
      static QString freeSurferAsciiCurvatureFileFilter;
      
      /// free surfer binary curvature file filter
      static QString freeSurferBinaryCurvatureFileFilter;
      
      /// free surfer ascii functional file filter
      static QString freeSurferAsciiFunctionalFileFilter;
      
      /// free surfer binary functional file filter
      static QString freeSurferBinaryFunctionalFileFilter;
      
      /// free surfer ascii label file filter
      static QString freeSurferAsciiLabelFileFilter;
      
      /// analyze volume file filter
      static QString analyzeVolumeFileFilter;
      
      /// minc volume file filter
      static QString mincVolumeFileFilter;
      
      /// vtk volume file filter
      static QString vtkVolumeFileFilter;
      
      /// raw volume file filter
      static QString rawVolumeFileFilter;
      
      /// text file filter
      static QString textFileFilter;
      
      /// comma separated values file filter
      static QString commaSeparatedValuesFileFilter;
      
      /// GIFTI functional file filter
      static QString giftiFunctionalFileFilter;
      
      /// GIFTI label file filter
      static QString giftiLabelFileFilter;
      
      /// GIFTI shape file filter
      static QString giftiShapeFileFilter;
      
      /// GIFTI surface file filter
      static QString giftiSurfaceFileFilter;
            
   private:
      /// files masks status flag
      static bool fileMasksValid;
      
      /// set the file masks for the open and save dialogs
      static void setFileMasks();
};

#ifdef _GUI_DATA_FILE_DIALOG_MAIN_
bool GuiDataFileDialog::fileMasksValid = false;
QString GuiDataFileDialog::areaColorFileFilter;
QString GuiDataFileDialog::arealEstimationFileFilter;
QString GuiDataFileDialog::borderRawFileFilter;
QString GuiDataFileDialog::borderFiducialFileFilter;
QString GuiDataFileDialog::borderInflatedFileFilter;
QString GuiDataFileDialog::borderVeryInflatedFileFilter;
QString GuiDataFileDialog::borderSphericalFileFilter;
QString GuiDataFileDialog::borderEllipsoidalFileFilter;
QString GuiDataFileDialog::borderCompressedFileFilter;
QString GuiDataFileDialog::borderFlatFileFilter;
QString GuiDataFileDialog::borderFlatLobarFileFilter;
QString GuiDataFileDialog::borderHullFileFilter;
QString GuiDataFileDialog::borderUnknownFileFilter;
QString GuiDataFileDialog::borderGenericFileFilter;
QString GuiDataFileDialog::borderVolumeFileFilter;
QString GuiDataFileDialog::borderColorFileFilter;
QString GuiDataFileDialog::borderProjectionFileFilter;
QString GuiDataFileDialog::cellFileFilter;
QString GuiDataFileDialog::cellColorFileFilter;
QString GuiDataFileDialog::cellProjectionFileFilter;
QString GuiDataFileDialog::cellVolumeFileFilter;
QString GuiDataFileDialog::cocomacFileFilter;
QString GuiDataFileDialog::contourFileFilter;
QString GuiDataFileDialog::contourCellFileFilter;
QString GuiDataFileDialog::contourCellColorFileFilter;
QString GuiDataFileDialog::coordinateRawFileFilter;
QString GuiDataFileDialog::coordinateFiducialFileFilter;
QString GuiDataFileDialog::coordinateInflatedFileFilter;
QString GuiDataFileDialog::coordinateVeryInflatedFileFilter;
QString GuiDataFileDialog::coordinateSphericalFileFilter;
QString GuiDataFileDialog::coordinateEllipsoidalFileFilter;
QString GuiDataFileDialog::coordinateCompressedFileFilter;
QString GuiDataFileDialog::coordinateFlatFileFilter;
QString GuiDataFileDialog::coordinateFlatLobarFileFilter;
QString GuiDataFileDialog::coordinateHullFileFilter;
QString GuiDataFileDialog::coordinateUnknownFileFilter;
QString GuiDataFileDialog::coordinateGenericFileFilter;
QString GuiDataFileDialog::cutsFileFilter;
QString GuiDataFileDialog::deformationFieldFileFilter;
QString GuiDataFileDialog::deformationMapFileFilter;
QString GuiDataFileDialog::fociFileFilter;
QString GuiDataFileDialog::fociColorFileFilter;
QString GuiDataFileDialog::fociProjectionFileFilter;
QString GuiDataFileDialog::fociVolumeFileFilter;
QString GuiDataFileDialog::geodesicDistanceFileFilter;
QString GuiDataFileDialog::latitudeLongitudeFileFilter;
QString GuiDataFileDialog::metricFileFilter;
QString GuiDataFileDialog::metricShapeFileFilter;
QString GuiDataFileDialog::paintFileFilter;
QString GuiDataFileDialog::paletteFileFilter;
QString GuiDataFileDialog::paramsFileFilter;
QString GuiDataFileDialog::probAtlasFileFilter;
QString GuiDataFileDialog::rgbPaintFileFilter;
QString GuiDataFileDialog::sceneFileFilter;
QString GuiDataFileDialog::sectionFileFilter;
QString GuiDataFileDialog::surfaceShapeFileFilter;
QString GuiDataFileDialog::surfaceVectorFileFilter;
QString GuiDataFileDialog::topographyFileFilter;
QString GuiDataFileDialog::topologyClosedFileFilter;
QString GuiDataFileDialog::topologyOpenFileFilter;
QString GuiDataFileDialog::topologyCutFileFilter;
QString GuiDataFileDialog::topologyCutLobarFileFilter;
QString GuiDataFileDialog::topologyUnknownFileFilter;
QString GuiDataFileDialog::topologyGenericFileFilter;
QString GuiDataFileDialog::transformationMatrixFileFilter;
QString GuiDataFileDialog::transformationDataFileFilter;
QString GuiDataFileDialog::volumeMultiFileAfniFilter;
QString GuiDataFileDialog::volumeFileAfniFilter;
QString GuiDataFileDialog::volumeFileNiftiReadFilter;
QString GuiDataFileDialog::volumeFileNiftiWriteFilter;
QString GuiDataFileDialog::volumeMultiFileNiftiFilter;
QString GuiDataFileDialog::volumeFileSpmMedxFilter;
QString GuiDataFileDialog::volumeFileWuNilFilter;
QString GuiDataFileDialog::volumeAnatomyFileFilter;
QString GuiDataFileDialog::volumeFunctionalFileFilter;
QString GuiDataFileDialog::volumePaintFileFilter;
QString GuiDataFileDialog::volumeProbAtlasFileFilter;
QString GuiDataFileDialog::volumeRgbFileFilter;
QString GuiDataFileDialog::volumeSegmentationFileFilter;
QString GuiDataFileDialog::volumeVectorFileFilter;
QString GuiDataFileDialog::volumeGenericFileFilter;
QString GuiDataFileDialog::vtkModelFileFilter;
QString GuiDataFileDialog::wustlRegionFileFilter;
QString GuiDataFileDialog::vocabularyFileFilter;

QString GuiDataFileDialog::metaAnalysisFileFilter;
QString GuiDataFileDialog::studyMetaDataFileFilter;
QString GuiDataFileDialog::specFileFilter;

QString GuiDataFileDialog::mdPlotFileFilter;
QString GuiDataFileDialog::neurolucidaFileFilter;
QString GuiDataFileDialog::brainVoyagerFileFilter;
QString GuiDataFileDialog::vtkSurfaceFileFilter;
QString GuiDataFileDialog::vtkXmlSurfaceFileFilter;
QString GuiDataFileDialog::byuSurfaceFileFilter;
QString GuiDataFileDialog::stlSurfaceFileFilter;
QString GuiDataFileDialog::studio3DSurfaceFileFilter;
QString GuiDataFileDialog::wavefrontObjFileFilter;
QString GuiDataFileDialog::vrmlSurfaceFileFilter;
QString GuiDataFileDialog::openInventorSurfaceFileFilter;

QString GuiDataFileDialog::freeSurferBinarySurfaceFileFilter;
QString GuiDataFileDialog::freeSurferAsciiSurfaceFileFilter;
QString GuiDataFileDialog::freeSurferAsciiCurvatureFileFilter;
QString GuiDataFileDialog::freeSurferBinaryCurvatureFileFilter;
QString GuiDataFileDialog::freeSurferAsciiFunctionalFileFilter;
QString GuiDataFileDialog::freeSurferBinaryFunctionalFileFilter;
QString GuiDataFileDialog::freeSurferAsciiLabelFileFilter;

QString GuiDataFileDialog::sumaRgbFileFilter;

QString GuiDataFileDialog::analyzeVolumeFileFilter;
QString GuiDataFileDialog::mincVolumeFileFilter;
QString GuiDataFileDialog::vtkVolumeFileFilter;
QString GuiDataFileDialog::rawVolumeFileFilter;

QString GuiDataFileDialog::commaSeparatedValuesFileFilter;
QString GuiDataFileDialog::textFileFilter;

QString GuiDataFileDialog::giftiSurfaceFileFilter;
QString GuiDataFileDialog::giftiFunctionalFileFilter;
QString GuiDataFileDialog::giftiShapeFileFilter;
QString GuiDataFileDialog::giftiLabelFileFilter;

#endif  // _GUI_DATA_FILE_DIALOG_MAIN_

#endif  // __VE_GUI_DATA_FILE_DIALOG_H__


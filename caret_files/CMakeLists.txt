
#
# Name of Project
#
PROJECT(CaretFiles)


#
# QT include files
#
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTXML TRUE)

INCLUDE(${QT_USE_FILE})


#
# Create a library
#
ADD_LIBRARY(CaretFiles
	   AfniHeader.h 
      AreaColorFile.h 
      ArealEstimationFile.h 
	   AtlasSpaceFile.h 
	   AtlasSurfaceDirectoryFile.h 
      BorderColorFile.h 
      BorderFile.h 
      BorderProjectionFile.h 
	   BrainVoyagerFile.h 
	   ByteSwapping.h 
      Caret6ProjectedItem.h 
      Caret7ProjectedItem.h 
      CaretScriptFile.h 
      CellClass.h 
      CellColorFile.h 
      CellFile.h 
      CellProjectionFile.h 
	   CellStudyInfo.h 
      CocomacConnectivityFile.h 
      ContourCellColorFile.h 
      ContourCellFile.h 
	   ContourFile.h 
      ColorFile.h 
      CutsFile.h 
	   DeformationFieldFile.h 
      FileFilters.h 
      FociColorFile.h   
      FociFile.h 
      FociProjectionFile.h 
      FociSearchFile.h 
	   FreeSurferCurvatureFile.h 
	   FreeSurferFunctionalFile.h 
	   FreeSurferLabelFile.h 
	   FreeSurferSurfaceFile.h 
	   GenericXmlFile.h 
      GeodesicDistanceFile.h 
      GeodesicHelper.h 
      ImageFile.h 
      LatLonFile.h 
      MDPlotFile.h 
      MetricFile.h 
      MniObjSurfaceFile.h 
      MultiResMorphFile.h 
      NeurolucidaFile.h 
      NiftiCaretExtension.h 
      NiftiFileHeader.h 
      NodeAttributeFile.h 
      NodeRegionOfInterestFile.h 
      PaintFile.h 
      PaletteFile.h 
	   ParamsFile.h 
      PreferencesFile.h 
      ProbabilisticAtlasFile.h 
      PubMedArticleFile.h 
      RgbPaintFile.h 
      SceneFile.h 
      SectionFile.h 
      SegmentationMaskListFile.h 
      SpecFile.h 
      SpecFileUtilities.h 
      StudyCollectionFile.h 
      StudyMetaDataFile.h
      StudyNamePubMedID.h 
	   SumsFileListFile.h 
      SureFitVectorFile.h 
      TopographyFile.h 
	   TopologyHelper.h 
      TopologyNode.h 
	   TransformationMatrixFile.h 
      VectorFile.h 
      VocabularyFile.h 
	   VolumeFile.h 
      VolumeITKImage.h 
      VolumeModification.h 
	   VtkModelFile.h 
	   WuNilHeader.h 
      WustlRegionFile.h 
      minc_cpp.h 
    mayo_analyze.h

	   AfniHeader.cxx 
      AreaColorFile.cxx 
      ArealEstimationFile.cxx 
	   AtlasSpaceFile.cxx 
	   AtlasSurfaceDirectoryFile.cxx 
      BorderColorFile.cxx 
      BorderFile.cxx 
      BorderProjectionFile.cxx 
	   BrainVoyagerFile.cxx 
	   ByteSwapping.cxx 
      Caret6ProjectedItem.cxx 
      Caret7ProjectedItem.cxx 
      CaretScriptFile.cxx 
      CellBase.cxx 
      CellColorFile.cxx 
      CellFile.cxx 
      CellProjectionFile.cxx 
	   CellStudyInfo.cxx 
      CocomacConnectivityFile.cxx 
      ColorFile.cxx 
      CommaSeparatedValueFile.cxx 
      ContourCellColorFile.cxx 
      ContourCellFile.cxx 
	   ContourFile.cxx 
      CutsFile.cxx 
	   DeformationFieldFile.cxx 
      FileFilters.cxx 
      FociColorFile.cxx 
      FociFile.cxx 
      FociProjectionFile.cxx 
      FociSearchFile.cxx 
	   FreeSurferCurvatureFile.cxx 
	   FreeSurferFunctionalFile.cxx 
	   FreeSurferLabelFile.cxx 
	   FreeSurferSurfaceFile.cxx 
	   GenericXmlFile.cxx 
      GeodesicDistanceFile.cxx 
      GeodesicHelper.cxx 
      ImageFile.cxx 
      LatLonFile.cxx 
      MDPlotFile.cxx 
      MetricFile.cxx 
      MniObjSurfaceFile.cxx 
      MultiResMorphFile.cxx 
      NeurolucidaFile.cxx 
      NiftiCaretExtension.cxx 
      NiftiFileHeader.cxx 
      NodeAttributeFile.cxx 
      NodeRegionOfInterestFile.cxx 
      PaintFile.cxx 
      PaletteFile.cxx 
	   ParamsFile.cxx 
      PreferencesFile.cxx 
      ProbabilisticAtlasFile.cxx 
      PubMedArticleFile.cxx 
      RgbPaintFile.cxx 
      SceneFile.cxx 
      SectionFile.cxx 
      SegmentationMaskListFile.cxx 
      SpecFile.cxx 
      SpecFileUtilities.cxx 
      StudyCollectionFile.cxx 
      StudyMetaDataFile.cxx
      StudyNamePubMedID.cxx 
	   SumsFileListFile.cxx 
      SureFitVectorFile.cxx 
      TopographyFile.cxx 
	   TopologyHelper.cxx 
	   TransformationMatrixFile.cxx 
      VectorFile.cxx 
      VocabularyFile.cxx 
	   VolumeFile.cxx 
      VolumeITKImage.cxx 
      VolumeModification.cxx 
	   VtkModelFile.cxx 
	   WuNilHeader.cxx 
      WustlRegionFile.cxx 
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/caret_brain_set
${CMAKE_SOURCE_DIR}/caret_command_operations
${CMAKE_SOURCE_DIR}/caret_common
${CMAKE_SOURCE_DIR}/caret_files
${CMAKE_SOURCE_DIR}/caret_gifti
${CMAKE_SOURCE_DIR}/caret_statistics
${CMAKE_SOURCE_DIR}/caret_uniformize
${CMAKE_SOURCE_DIR}/caret_widgets
)

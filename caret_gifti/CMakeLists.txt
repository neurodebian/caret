
#
# Name of Project
#
PROJECT(CaretGifti)


#
# QT include files
#
SET(QT_DONT_USE_QTGUI TRUE)
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTXML TRUE)

INCLUDE(${QT_USE_FILE})

#
# Create a library
#
ADD_LIBRARY(CaretGifti
      AbstractFile.h 
      CommaSeparatedValueFile.h 
      CoordinateFile.h 
      DeformationMapFile.h 
      FileException.h 
      GiftiCommon.h 
      GiftiDataArray.h 
      GiftiDataArrayFile.h 
      GiftiDataArrayFileSaxReader.h 
      GiftiDataArrayFileStreamReader.h 
      GiftiLabelTable.h 
      GiftiMatrix.h 
      GiftiMetaData.h 
      GiftiNodeDataFile.h 
      StudyMetaDataLink.h 
      StudyMetaDataLinkSet.h 
      SurfaceFile.h 
      SurfaceShapeFile.h 
      XhtmlTableExtractorFile.h 
      XmlGenericWriter.h 
      XmlGenericWriterAttributes.h 
      GiftiDataArrayReadListener.h 
      nifti1.h

      AbstractFile.cxx 
      CoordinateFile.cxx 
      DeformationMapFile.cxx 
      FileException.cxx 
      GiftiCommon.cxx 
      GiftiDataArray.cxx 
      GiftiDataArrayFile.cxx 
      GiftiDataArrayFileSaxReader.cxx 
      GiftiDataArrayFileStreamReader.cxx 
      GiftiLabelTable.cxx 
      GiftiMatrix.cxx 
      GiftiMetaData.cxx 
      GiftiNodeDataFile.cxx 
      StudyMetaDataLink.cxx 
      StudyMetaDataLinkSet.cxx 
      SurfaceFile.cxx 
      SurfaceShapeFile.cxx 
      TextFile.cxx 
      TopologyFile.cxx 
      XhtmlTableExtractorFile.cxx 
      XmlGenericWriter.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/caret_common
${CMAKE_SOURCE_DIR}/caret_files
${CMAKE_SOURCE_DIR}/caret_gifti
${CMAKE_SOURCE_DIR}/caret_statistics
)

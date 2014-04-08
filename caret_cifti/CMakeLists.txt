
#
# Name of Project
#
PROJECT(CaretCifti)


#
# QT include files
#
SET(QT_DONT_USE_QTGUI TRUE)
SET(QT_USE_QTNETWORK TRUE)

INCLUDE(${QT_USE_FILE})

#
# Create a library
#
ADD_LIBRARY(CaretCifti
          CiftiFile.cxx 
          CiftiFileException.cxx 
          CiftiMatrix.cxx 
          CiftiXMLReader.cxx 
          CiftiXMLWriter.cxx 
          CiftiXMLElements.cxx 
          CiftiXML.cxx 
          Nifti2Header.cxx

          CiftiFile.h 
          CiftiFileException.h 
          CiftiMatrix.h 
          CiftiXMLReader.h 
          CiftiXMLWriter.h 
          CiftiXMLElements.h 
          CiftiXML.h 
          nifti1.h 
          nifti2.h 
          Nifti2Header.h
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/caret_cifti
${CMAKE_SOURCE_DIR}/caret_common
${CMAKE_SOURCE_DIR}/caret_files
)

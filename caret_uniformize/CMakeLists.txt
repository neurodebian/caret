
#
# Name of Project
#
PROJECT(CaretUniformize)


#
# QT include files
#
SET(QT_DONT_USE_QTGUI TRUE)

#
# Create a library
#
ADD_LIBRARY(CaretUniformize
   caret_uniformize.h 
   matrix.h

   matrix.cxx
   caret_uniformize.cxx 
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/caret_common
${CMAKE_SOURCE_DIR}/caret_files
${CMAKE_SOURCE_DIR}/caret_gifti
${CMAKE_SOURCE_DIR}/caret_statistics
${CMAKE_SOURCE_DIR}/caret_uniformize
)

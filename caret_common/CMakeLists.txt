
#
# Name of Project
#
PROJECT(CaretCommon)


#
# QT include files
#
SET(QT_DONT_USE_QTGUI TRUE)
SET(QT_USE_QTNETWORK TRUE)

INCLUDE(${QT_USE_FILE})
#message("QT VERSION: " ${QT_VERSION_MAJOR} ${QT_VERSION_MINOR} ${QT_VERSION_PATCH})

SET(MOC_INPUT_HEADER_FILES
HttpFileDownload.h
)

QT4_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})

#
# Create a library
#
ADD_LIBRARY(CaretCommon
Basename.h
CaretException.h
CaretLinkedList.h
CaretTips.h
CaretVersion.h
Category.h
CommandLineUtilities.h
DateAndTime.h
DebugControl.h
FileUtilities.h
GaussianComputation.h
HtmlColors.h
HttpFileDownload.h
MathUtilities.h
MatrixUtilities.h
NameIndexSort.h
PointLocator.h
ProgramParameters.h
ProgramParametersException.h
Species.h
StatisticsUtilities.h
StereotaxicSpace.h
StringTable.h
StringUtilities.h
Structure.h
SystemUtilities.h
UbuntuMessage.h
ValueIndexSort.h

${MOC_SOURCE_FILES}

Basename.cxx
CaretLinkedList.cxx
CaretTips.cxx
Category.cxx
CommandLineUtilities.cxx
DateAndTime.cxx
DebugControl.cxx
FileUtilities.cxx
GaussianComputation.cxx
HtmlColors.cxx
HttpFileDownload.cxx
MathUtilities.cxx
NameIndexSort.cxx
PointLocator.cxx
ProgramParameters.cxx
ProgramParametersException.cxx
Species.cxx
StatisticsUtilities.cxx
StereotaxicSpace.cxx
StringTable.cxx
StringUtilities.cxx
Structure.cxx
SystemUtilities.cxx
ValueIndexSort.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/caret_common
${CMAKE_SOURCE_DIR}/caret_gifti
${CMAKE_SOURCE_DIR}/caret_statistics
)

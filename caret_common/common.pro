#=================================================================================
#
# Check for needed environment variables
#
!exists( $(VTK_INC_DIR)/vtkConfigure.h ) {
error("The environment variable for VTK includes \"VTK_INC_DIR\" not defined.")
}
!exists( $(QTDIR)/include/qt.h ) {
!exists( $(QTDIR)/include/Qt ) {
    !exists( $(QTDIR)/include/qt4/qt.h ) {
        !exists( $(QTDIR)/include/qt4/Qt ) {
            error("The environment variable for QT \"QTDIR\" not defined.")
        }
    }
}
}

#==============================================================================
#
# VTK settings
#
!vs:!nmake:INCLUDEPATH  += $$(VTK_INC_DIR) #visual studio has separate include dirs for debug and release

#
# have VTK compiler defines (primarily for GIFTI API)
#
DEFINES += HAVE_VTK

#
# VTK 5.x flag
#
DEFINES += HAVE_VTK5
message( "Building with VTK5 support" )

#
#
#

VTK_LIBS_TEMP = -lvtkFiltering \
            -lvtkGenericFiltering \
            -lvtkImaging \
            -lvtkGraphics \
            -lvtkIO \
            -lvtkFiltering \
            -lvtkCommon \
            -lvtksys \
            -lvtkjpeg \
            -lvtkpng \
            -lvtkexpat \
            -lvtkzlib

DEFINES += CARET_QT4
QT += network opengl xml
!vs:!nmake:QMAKE_CXXFLAGS += -Wno-deprecated

TARGET   = Common
!vs:TEMPLATE = lib
vs:TEMPLATE=vclib
CONFIG  += staticlib
INCLUDEPATH += .
dll {
   CONFIG -= staticlib
   CONFIG += plugin
}

INCLUDEPATH += ../caret_vtk4_classes ../caret_gifti



# Input
HEADERS += Basename.h \
      CaretException.h \
      CaretLinkedList.h \
      CaretTips.h \
      Category.h \
	   CommandLineUtilities.h \
       DateAndTime.h \
	   DebugControl.h \
	   FileUtilities.h \
      GaussianComputation.h \
      HtmlColors.h \
	   HttpFileDownload.h \
	   MathUtilities.h \
	   MatrixUtilities.h \
      NameIndexSort.h \
      PointLocator.h \
      ProgramParameters.h \
      ProgramParametersException.h \
      StatisticsUtilities.h \
      StringTable.h \
      Species.h \
      StereotaxicSpace.h \
	   StringUtilities.h \
      Structure.h \
	   SystemUtilities.h \
      UbuntuMessage.h \
      ValueIndexSort.h \
    CaretVersion.h

SOURCES += Basename.cxx \
      CaretLinkedList.cxx \
      CaretTips.cxx \
      Category.cxx \
	   CommandLineUtilities.cxx \
      DateAndTime.cxx \
	   DebugControl.cxx \
	   FileUtilities.cxx \
      GaussianComputation.cxx \
      HtmlColors.cxx \
	   HttpFileDownload.cxx \
	   MathUtilities.cxx \
      NameIndexSort.cxx \
      PointLocator.cxx \
      ProgramParameters.cxx \
      ProgramParametersException.cxx \
      StatisticsUtilities.cxx \
      StringTable.cxx \
      Species.cxx \
      StereotaxicSpace.cxx \
	   StringUtilities.cxx \
      Structure.cxx \
	   SystemUtilities.cxx \
      ValueIndexSort.cxx

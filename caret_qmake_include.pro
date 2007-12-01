
#
#  Compile debug on mac since that is where development is done
#  Compile optimized on Linux and Windows
#
macx {
   #
   # Debug
   #
   CONFIG      -= release
   CONFIG      += warn_on debug
   
   #
   # Release
   #
   #CONFIG	+= release
   #CONFIG	-= debug
}
!macx {
   CONFIG	+= release
   CONFIG	-= debug
}

CONFIG	+= qt thread

QT 	+= network opengl xml
#QT	+= qt3support
QT	-= qt3support

#
# when missing, it must be the GIFTI API library
#
DEFINES += CARET_FLAG

#=================================================================================
#
# Check for needed environment variables
#
!exists( $(VTK_INC_DIR)/vtkConfigure.h ) {
   error("The environment variable for VTK includes \"VTK_INC_DIR\" not defined.")
}
!exists( $(QWT_INC_DIR)/qwt.h ) {
   error("The environment variable for QWT includes \"QWT_INC_DIR\" not defined.")
}
!exists( $(QTDIR)/include/qt4/qt.h ) {
   !exists( $(QTDIR)/include/qt4/Qt ) {
      error("The environment variable for QT \"QTDIR\" not defined.")
   }
}

#=================================================================================
#
# if this file exists then QT4 is being used
#
exists( $(QTDIR)/include/qt4/Qt/qicon.h ) {
   DEFINES += CARET_QT4
   QT += network opengl xml
}

#=================================================================================
#
# enable for profile (performance) measurement
#
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg

#=================================================================================
#
# Update include paths
#
INCLUDEPATH	+= $(QWT_INC_DIR)

INCLUDEPATH += \
   ../caret_brain_set \
   ../caret_command_operations \
   ../caret_common \
   ../caret_statistics \
   ../caret_files \
   ../caret_uniformize \
   ../caret_widgets

DEPENDPATH += \
   ../caret_brain_set \
   ../caret_command_operations \
   ../caret_common \
   ../caret_statistics \
   ../caret_files \
   ../caret_uniformize \
   ../caret_widgets

#=================================================================================
#
# Is NetCDF/Minc available ?
#
exists( $(NETCDF_INC_DIR)/minc.h ) {
   DEFINES += HAVE_MINC
   INCLUDEPATH += $(NETCDF_INC_DIR)
   NETCDF_LIBS = -L$(NETCDF_LIB_DIR) \
                 -lminc \
                 -lnetcdf

   message( "Building with MINC support" )
}

#
# Is NetCDF/Minc NOT available
#
!exists( $(NETCDF_INC_DIR)/minc.h ) {
   message( "Building WITHOUT MINC support" )
}

#==============================================================================
#
# VTK settings
#
INCLUDEPATH	+= $(VTK_INC_DIR)

#
# have VTK compiler defines (primarily for GIFTI API)
#
DEFINES += HAVE_VTK

#
# Check for VTK 5.x
#
#exists( $(VTK_INC_DIR)/vtkMPEG2Writer.h ) {
  
   message( "Building WITH VTK5 support" )

   #
   # VTK 5.x flag
   #
   DEFINES += HAVE_VTK5

   #
   #
   #
#   INCLUDEPATH += ../caret_vtk4_classes

   #
   # need by caret_vtk4_classes
   #
#   INCLUDEPATH += $(VTK_INC_DIR)/vtkmpeg2encode

   #
   # VTK Libraries for VTK 5.x
   #
   win32 {
      #VTK_LIBS = ../caret_vtk4_classes/debug/libCaretVtk4Classes.a 
   }
   !win32 {
#      VTK_LIBS = ../caret_vtk4_classes/libCaretVtk4Classes.a 
   }
   VTK_LIBS += \
              -L$(VTK_LIB_DIR) \
	      -lvtkRendering \
              -lvtkFiltering \
              -lvtkGenericFiltering \
              -lvtkImaging \
              -lvtkGraphics \
              -lvtkIO \
              -lvtkFiltering \
              -lvtkCommon
#}

#
# Check for VTK 4.x (does not have vtkMPEG2Writer.h)
#
exists( doesnotexist.never ) {
   #
   # VTK Libraries for VTK 4.x
   #
   VTK_LIBS = -L$(VTK_LIB_DIR) \
              -lvtkGraphics \
              -lvtkFiltering \
              -lvtkIO \
              -lvtkPatented \
              -lvtkImaging \
              -lvtkCommon \
              -lvtkGraphics \
              -lvtkFiltering \
              -lvtkIO \
              -lvtkPatented \
              -lvtkImaging \
              -lvtkCommon \
              -lvtkFiltering \
              -lvtkCommon \
              -lvtkjpeg \
              -lvtkpng \
              -lvtkexpat \
              -lvtkDICOMParser \
	      -lvtksys
   !macx {
      VTK_LIBS += \
              -lvtkzlib
   }
}

#==============================================================================
#
# Is Mesa Offscreen Rendering Available?
#
#exists ( $(OSMESA_INC_DIR)/GL/osmesa.h ) {
#   DEFINES += HAVE_OSMESA
#
#   OSMESA_INCLUDE_PATH = $(OSMESA_INC_DIR)
#   OSMESA_LIBS = -L$(OSMESA_LIB_DIR) \
#                 -lOSMesa \
#                 -lGL \
#                 -lGLU \
#                 -lOSMesa
#
#   message( "Offscreen Mesa Available" )
#}

#==============================================================================
#
# Is ITK available
#
#exists( $(ITK_INC_DIR)/itkConfigure.h ) {
#   message( "Building with ITK support" )
exists( $(ITK_INC_DIR)/INTENTIONALLY_NO_VTK ) {
   DEFINES += HAVE_ITK

   INCLUDEPATH += \
      $(ITK_INC_DIR) \
      $(ITK_INC_DIR)/Algorithms \
      $(ITK_INC_DIR)/BasicFilters \
      $(ITK_INC_DIR)/Common \
      $(ITK_INC_DIR)/IO \
      $(ITK_INC_DIR)/Numerics \
      $(ITK_INC_DIR)/Numerics/Statistics \
      $(ITK_INC_DIR)/Patented \
      $(ITK_INC_DIR)/SpatialObject \
      $(ITK_INC_DIR)/Utilities/vxl/core \
      $(ITK_INC_DIR)/Utilities/vxl/vcl 

   ITK_LIBS = -L$(ITK_LIB_DIR) \
      -lITKBasicFilters \
      -lITKStatistics \
      -lITKNumerics \
      -lITKCommon \
      -litkvnl_inst \
      -litkvnl_algo \
      -litkvnl \
      -litkvcl \
      -litknetlib \
      -litksys
}

#
# is ITK NOT available
#
!exists( $(ITK_INC_DIR)/itkConfigure.h ) {
   message( "Building WITHOUT ITK support" )
}

#=================================================================================
#
# Windows unique stuff
#
win32 {
   !exists( $(ZLIB_INC_DIR)/zlib.h ) {
      error( "You must define ZLIB_INC_DIR to the ZLIB include files directory.")
   }
   CONFIG      += rtti exceptions console
   INCLUDEPATH += $(ZLIB_INC_DIR)
##   LIBS        += -L$(ZLIB_LIB_DIR) -lzlib

      QMAKE_CXXFLAGS_RELEASE +=  -Wno-deprecated
      QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated


      #
      # QWT libraries
      #
      QWT_LIBS = -L$(QWT_LIB_DIR) \
                 -lqwt
}

   

#=================================================================================
# 
# Windows Speech API
#
#win32 {
#   exists( $(SAPI_DIR)\Include\sapi.h ) {
#      DEFINES     += HAVE_MS_SPEECH
#      INCLUDEPATH += $(SAPI_DIR)\Include
#      LIBS        += $(SAPI_DIR)\Lib\i386\sapi.lib
#   }
#}

#=================================================================================
#
# Macintosh unique stuff
#
macx {
   #
   # Build objects for Universal binaries
   #
   #CONFIG += ppc x86
   QMAKE_CXXFLAGS_RELEASE +=  -Wno-deprecated \
      -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc
   QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated \
      -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc

   #
   # Link  for OSX 10.4
   #
   QMAKE_LFLAGS_DEBUG += -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk
   QMAKE_LFLAGS_RELEASE += -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk

   # Link for Intel
   QMAKE_LFLAGS_DEBUG += -arch i386
   QMAKE_LFLAGS_RELEASE += -arch i386

   #
   # Add Link PPC for universal binaries
   #
   #QMAKE_LFLAGS_DEBUG += -arch ppc 
   #QMAKE_LFLAGS_RELEASE += -arch ppc  

   #
   # Universal binaries
   # Note: universal binaries are very slow to link, 
   # so disable except when needed.
   #
   #QMAKE_LFLAGS_DEBUG += -arch i386 -arch ppc
   #QMAKE_LFLAGS_RELEASE += -arch i386 -arch ppc


   #
   # 64 bit support
   # Note that these flags are not sufficient to link
   # May need to recompile VTK, QT, etc
   #
   #QMAKE_CXXFLAGS_RELEASE += -m64
   #QMAKE_CXXFLAGS_DEBUG += -m64
   #QMAKE_LFLAGS_DEBUG += -m64
   #QMAKE_LFLAGS_RELEASE += -m64

#   CONFIG      -= release
#   CONFIG      += warn_on debug

   #
   # QWT libraries
   #
   QWT_LIBS = -L$(QWT_LIB_DIR) \
              -lqwt

}

#=================================================================================
#
# Unix (but not Mac) unique stuff
#
unix:!macx {
   #
   # QWT libraries
   #
   QWT_LIBS = -L$(QWT_LIB_DIR) \
         -lqwt-qt4

   QMAKE_CXXFLAGS_RELEASE +=  -Wno-deprecated
   QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
}




CONFIG	+= qt thread
QT += network opengl xml
QT -= qt3support

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
!exists( $(QTDIR)/include/qt.h ) {
!exists( $(QTDIR)/include/Qt ) {
    !exists( $(QTDIR)/include/qt4/qt.h ) {
        !exists( $(QTDIR)/include/qt4/Qt ) {
            error("The environment variable for QT \"QTDIR\" not defined.")
        }
    }
}
}

#=================================================================================
#
# if this file exists then QT4 is being used
#
exists( $(QTDIR)/include/Qt/qicon.h ) {
DEFINES += CARET_QT4
QT += network opengl xml
}

#=================================================================================
#
# Update include paths
#

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
INCLUDEPATH += $$(NETCDF_INC_DIR)
NETCDF_LIBS = -L$$(NETCDF_LIB_DIR) \
                -lminc \
                -lnetcdf

exists( $(NETCDF_LIB_DIR)/libminc2.a ) {
    NETCDF_LIBS += -lminc2

    #
    # Apparently, if libminc2 exists, libminc
    # may not and is not necessary
    #
    !exists( $(NETCDF_LIB_DIR)/libminc.a ) {
        NETCDF_LIBS -= -lminc
    }
}
exists( $(NETCDF_LIB_DIR)/libminc2.so ) {
    NETCDF_LIBS += -lminc2
}

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
# QWT settings
#
exists( $(QWT_INC_DIR)/qwt.h ) {
   message("Building with QWT support")

   DEFINES += HAVE_QWT
   INCLUDEPATH	+= $$(QWT_INC_DIR)
}
!exists( $(QWT_INC_DIR)/qwt.h ) {
   message("Building WITHOUT QWT support")
}
#==============================================================================
#
# VTK settings
#
!vs:INCLUDEPATH	+= $$(VTK_INC_DIR) #visual studio has separate include dirs for debug and release

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
            
#==============================================================================
#==============================================================================
#
# There are five major target platforms, Unix, OSX, Windows/Msys, Windows/VisualStudio, Debian/Ubuntu.
# 

#
# Windows/Msys Specific
#
win32:!vs {
    INCLUDEPATH += ../caret_vtk4_classes
    INCLUDEPATH += $$(VTK_INC_DIR)/vtkmpeg2encode
    VTK_LIBS = -L$$(VTK_LIB_DIR) $$VTK_LIBS_TEMP
    exists( $(VTK_INC_DIR)/vtkMPEG2Writer.h ) {
        message( "Building WITH MPEG support" )
        DEFINES += HAVE_MPEG
        VTK_LIBS += -lvtkMPEG2Encode
    }
    
    #
    # We need to include zlib on windows
    #
    !exists( $(ZLIB_INC_DIR)/zlib.h ) {
    error( "You must define ZLIB_INC_DIR to the ZLIB include files directory.")
    }
    CONFIG      += rtti exceptions console
    INCLUDEPATH += $$(ZLIB_INC_DIR)
    LIBS        += -L$$(ZLIB_LIB_DIR) -lz
    contains (DEFINES, HAVE_QWT) {
        QWT_LIBS = -L$$(QWT_LIB_DIR) -lqwt 
    }
}

#
# Windows/Visual Studio Specific
#

vs {
    CONFIG(debug,release|debug):INCLUDEPATH	+= $$(VTK_INC_DIR)
	CONFIG(release,release|debug):INCLUDEPATH	+= $$(VTK_RELEASE_INC_DIR)
    INCLUDEPATH += ../caret_vtk4_classes
    CONFIG(debug,release|debug):INCLUDEPATH += $$(VTK_INC_DIR)/vtkmpeg2encode
	CONFIG(release,release|debug):INCLUDEPATH += $$(VTK_RELEASE_INC_DIR)/vtkmpeg2encode
    VTK_LIBS = -L$$(VTK_LIB_DIR) $$VTK_LIBS_TEMP
    VTK_RELEASE_LIBS = -L$$(VTK_RELEASE_LIB_DIR) $$VTK_LIBS_TEMP
	
    exists( $(VTK_INC_DIR)/vtkMPEG2Writer.h ) {
        message( "Building WITH MPEG support" )
        DEFINES += HAVE_MPEG
        VTK_LIBS += -lvtkMPEG2Encode
        VTK_REELEASE_LIBS += -lvtkMPEG2Encode
    }    
    
    #
    # We need to include zlib on windows
    #
    !exists( $(ZLIB_INC_DIR)/zlib.h ) {
    error( "You must define ZLIB_INC_DIR to the ZLIB include files directory.")
    }
    CONFIG      += rtti exceptions console
    INCLUDEPATH += $$(ZLIB_INC_DIR)
    LIBS        += -L$$(ZLIB_LIB_DIR) -lzlib
    
    contains (DEFINES, HAVE_QWT) {
        CONFIG(release,release|debug):QWT_LIBS = -L$$(QWT_LIB_DIR) -lqwt 
        CONFIG(debug,release|debug):QWT_LIBS = -L$$(QWT_LIB_DIR) -lqwtd
    }
}

#
# OsX Specific
# 
macx {
    QMAKE_CXXFLAGS   += -fopenmp
    QMAKE_LFLAGS     += -fopenmp
    INCLUDEPATH += ../caret_vtk4_classes
    INCLUDEPATH += $$(VTK_INC_DIR)/vtkmpeg2encode
    !dll:VTK_LIBS = ../caret_vtk4_classes/libCaretVtk4Classes.a
    VTK_LIBS += -L$$(VTK_LIB_DIR) $$VTK_LIBS_TEMP
    
    exists( $(VTK_INC_DIR)/vtkMPEG2Writer.h ):!ubuntu {
        message( "Building WITH MPEG support" )
        DEFINES += HAVE_MPEG
        VTK_LIBS += -lvtkMPEG2Encode
    }
    
    contains (DEFINES, HAVE_QWT) {
       QWT_LIBS = -L$$(QWT_LIB_DIR) -lqwt    
    }    
    QMAKE_CXXFLAGS +=  -isysroot /Developer/SDKs/MacOSX10.5.sdk
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_LFLAGS_RELEASE += -Wl,-syslibroot,/Developer/SDKs/MacOSX10.5.sdk
    QMAKE_LFLAGS_APP += -w
}

#
# Debian/Ubuntu Specific
#

ubuntu {    
    message ("Building for Ubuntu")
    QMAKE_CXXFLAGS += -fopenmp -DUBUNTU
    QMAKE_LFLAGS += -fopenmp
    VTK_LIBS += -L$$(VTK_LIB_DIR) -lvtkFiltering \
            -lvtkGenericFiltering \
            -lvtkImaging \
            -lvtkGraphics \
            -lvtkIO \
            -lvtkFiltering \
            -lvtkCommon \
            -lvtksys
        
    LIBS += -lpng
    contains (DEFINES, HAVE_QWT) {
       QWT_LIBS = -L$$(QWT_LIB_DIR) -lqwt-qt4
    }
    DEFINES -= HAVE_MPEG
    VTK_LIBS -= -lvtkMPEG2Encode
    exists( $(NETCDF_INC_DIR)/minc.h ) {
        DEFINES += HAVE_MINC
        INCLUDEPATH += $$(NETCDF_INC_DIR)
        NETCDF_LIBS = -L$$(NETCDF_LIB_DIR) \
                        -lminc2 \
                        -lnetcdf
        NETCDF_LIBS -= -lminc
    }
}

# 
# All other Unix variants
#
unix:!macx:!ubuntu {
    contains(QMAKE_CXX, icpc) {
        #QMAKE_CXXFLAGS   += -openmp -ipo
        #QMAKE_LFLAGS     += -openmp -openmp-link static -ipo
        QMAKE_CXXFLAGS   += -openmp
        QMAKE_LFLAGS     += -openmp -openmp-link static
    }
    !contains(QMAKE_CXX, icpc) {
        QMAKE_CXXFLAGS   += -fopenmp
        QMAKE_LFLAGS     += -fopenmp
    }
    INCLUDEPATH += ../caret_vtk4_classes
    INCLUDEPATH += $$(VTK_INC_DIR)/vtkmpeg2encode
    !dll:VTK_LIBS = ../caret_vtk4_classes/libCaretVtk4Classes.a
    VTK_LIBS += -L$$(VTK_LIB_DIR) $$VTK_LIBS_TEMP
    exists( $(VTK_INC_DIR)/vtkMPEG2Writer.h ) {
        message( "Building WITH MPEG support" )
        DEFINES += HAVE_MPEG
        VTK_LIBS += -lvtkMPEG2Encode
    }    
    contains (DEFINES, HAVE_QWT) {
        QWT_LIBS = -L$$(QWT_LIB_DIR) -lqwt
    }
}

#==============================================================================
#==============================================================================
#
# For build flags, there are two major platforms, Unix and Visual Studio
#
#
# Unix (including msys win32) Build Flags
#
!vs {
    release {
        QMAKE_CXXFLAGS -= -g -O1 -Wl,-O1
        QMAKE_LFLAGS -= -g -O1 -Wl,-O1
        QMAKE_CXXFLAGS += -O2 -Wno-deprecated
        QMAKE_LFLAGS += -O2
        !debug:!profile:QMAKE_POST_LINK=strip -S $(TARGET)
    }

    debug {
        QMAKE_CXXFLAGS += -g -Wno-deprecated
        QMAKE_LFLAGS += -g
        QMAKE_CXXFLAGS -= -O2
        QMAKE_LFLAGS -= -O2
    }

    profile {
       QMAKE_CXXFLAGS +=  -pg -O2
       QMAKE_LFLAGS += -pg
    } 
}

#
# Visual Studio specific build flags
#
win32:vs {
    CONFIG(release,release|debug) {
        QMAKE_CXXFLAGS_RELEASE -= -O1 -Wl,-O2
        QMAKE_LFLAGS_RELEASE -= -O1 -Wl,-O2
        QMAKE_CXXFLAGS_RELEASE += -O2 -D_USE_MATH_DEFINES -wd"4290" -wd"4244" -wd"4267" -wd"4305" -wd"4100" -wd"4005" -MP -DNOMINMAX
        QMAKE_LFLAGS_RELEASE += -O2 -D_USE_MATH_DEFINES -STACK:10000000
    }

    CONFIG(debug,release|debug) {
        QMAKE_CXXFLAGS_DEBUG -= -O2 
        QMAKE_LFLAGS_DEBUG -= -O2
        QMAKE_CXXFLAGS_DEBUG += -D_DEBUG -D_USE_MATH_DEFINES -wd"4290" -wd"4244" -wd"4267" -wd"4305" -wd"4100" -wd"4005" -MP -DNOMINMAX
        QMAKE_LFLAGS_DEBUG += -DEBUG -STACK:10000000
    }
}

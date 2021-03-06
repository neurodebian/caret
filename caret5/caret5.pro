######################################################################
# Automatically generated by qmake (1.04a) Tue Jan 14 11:58:13 2003
######################################################################

!vs:TEMPLATE = app
vs:TEMPLATE=vcapp
#
# For JPEG support with static linking
#
#message("QT VERSION:  $$[QT_VERSION]")

contains(QT_VERSION, ^4\\.[0-6]\\..*) {
   QTPLUGIN += qgif qjpeg qtiff
}
LIBS     += -L$(QTDIR)/plugins/imageformats

INCLUDEPATH += .
               
include(../caret_qmake_include.pro)

CONFIG += assistant

unix:release:!debug:!profile {
   QMAKE_POST_LINK=strip $(TARGET)
   ##QMAKE_POST_LINK=strip --strip-debug $(TARGET)
}

win32 {
    !vs:!nmake {
        CONFIG(debug) {
           LIBS += \
  	      ..\caret_gui\debug\libCaretGui.a \
  	      ..\caret_common\debug\libCaretCommon.a \
              ..\caret_command_operations\debug\libCaretCommandOperations.a \
              ..\caret_brain_set\debug\libCaretBrainSet.a \
              ..\caret_vtk4_classes\debug\libCaretVtk4Classes.a \
              ..\caret_gifti\debug\libCaretGifti.a \
              ..\caret_files\debug\libCaretFiles.a \
              ..\caret_uniformize\debug\libCaretUniformize.a \
              ..\caret_statistics\debug\libCaretStatistics.a \
              ..\caret_widgets\debug\libCaretWidgets.a \
              ..\caret_common\debug\libCaretCommon.a \
              ..\caret_cifti\debug\libCaretCifti.a
        }
        CONFIG(release) {
           LIBS += \
  	      ../caret_gui/release/libCaretGui.a \
	      ../caret_common/release/libCaretCommon.a \
              ../caret_command_operations/release/libCaretCommandOperations.a \
              ../caret_brain_set/release/libCaretBrainSet.a \
              ../caret_vtk4_classes/release/libCaretVtk4Classes.a \
              ../caret_gifti/release/libCaretGifti.a \
              ../caret_files/release/libCaretFiles.a \
	      ../caret_uniformize/release/libCaretUniformize.a \
              ../caret_statistics/release/libCaretStatistics.a \
              ../caret_widgets/release/libCaretWidgets.a \
              ../caret_common/release/libCaretCommon.a \
              ../caret_cifti/release/libCaretCifti.a
        }
        LIBS += $$VTK_LIBS
   }

   else { #it's either vs or nmake if it failed the test above
        CONFIG(debug,debug|release) {
                LIBS += \
  	            ..\caret_gui\debug\libCaretGui.lib \
		    ..\caret_common\debug\CaretCommon.lib \
                    ..\caret_command_operations\debug\CaretCommandOperations.lib \
                    ..\caret_brain_set\debug\CaretBrainSet.lib \
                    ..\caret_vtk4_classes\debug\CaretVtk4Classes.lib \
                    ..\caret_gifti\debug\CaretGifti.lib \
                    ..\caret_files\debug\CaretFiles.lib \
                    ..\caret_uniformize\debug\CaretUniformize.lib \
                    ..\caret_statistics\debug\CaretStatistics.lib \
                    ..\caret_widgets\debug\CaretWidgets.lib \
                    ..\caret_common\debug\CaretCommon.lib \
                    ..\caret_cifti\debug\CaretCifti.lib
                LIBS-=$$VTK_RELEASE_LIBS
                LIBS+=$$VTK_LIBS
        }
        CONFIG(release,debug|release) {
                LIBS += \
  	            ..\caret_gui\release\libCaretGui.lib \
		    ..\caret_common\release\CaretCommon.lib \
                    ..\caret_command_operations\release\CaretCommandOperations.lib \
                    ..\caret_brain_set\release\CaretBrainSet.lib \
                    ..\caret_vtk4_classes\release\CaretVtk4Classes.lib \
                    ..\caret_gifti\release\CaretGifti.lib \
                    ..\caret_files\release\CaretFiles.lib \
                    ..\caret_uniformize\release\CaretUniformize.lib \
                    ..\caret_statistics\release\CaretStatistics.lib \
                    ..\caret_widgets\release\CaretWidgets.lib \
                    ..\caret_common\release\CaretCommon.lib \
                    ..\caret_cifti\release\CaretCifti.lib
                LIBS-=$$VTK_LIBS
                LIBS+=$$VTK_RELEASE_LIBS
        }   
    }

  
    RC_FILE = caret5.rc
    CONFIG(debug,debug|release)
    {
        CONFIG -= windows
        CONFIG += console
    }
    CONFIG(release,debug|release)
    {
        CONFIG -= windows
		CONFIG += console        
    }
    contains( DEFINES, HAVE_QWT ) {
        LIBS += $$QWT_LIBS
    }
    contains( DEFINES, HAVE_MINC ) {
        LIBS += $$NETCDF_LIBS
    }
}

unix:!ubuntu {
   !dll {
       PRE_TARGETDEPS +=  \
        ../caret_gui/libCaretGui.a \
        ../caret_command_operations/libCaretCommandOperations.a \
        ../caret_brain_set/libCaretBrainSet.a \
        ../caret_vtk4_classes/libCaretVtk4Classes.a \
        ../caret_gifti/libCaretGifti.a \
        ../caret_files/libCaretFiles.a \
        ../caret_uniformize/libCaretUniformize.a \
        ../caret_statistics/libCaretStatistics.a \
        ../caret_common/libCaretCommon.a \
        ../caret_widgets/libCaretWidgets.a \
        ../caret_cifti/libCaretCifti.a     
    }

   dll {
       PRE_TARGETDEPS +=  \
           ../caret_gui/libCaretGui.so \
           ../caret_command_operations/libCaretCommandOperations.so \
           ../caret_brain_set/libCaretBrainSet.so \
           ../caret_vtk4_classes/libCaretVtk4Classes.so \
           ../caret_gifti/libCaretGifti.so \
           ../caret_files/libCaretFiles.so \
           ../caret_uniformize/libCaretUniformize.so \
           ../caret_statistics/libCaretStatistics.so \
           ../caret_common/libCaretCommon.so \
           ../caret_widgets/libCaretWidgets.so \
           ../caret_cifti/libCaretCifti.so
           LIBS += -ldl 
    }
}

macx {
   QMAKE_LFLAGS_APP -= -prebind
   
   LIBS +=  \
        -L../caret_gui -lCaretGui \
        -L../caret_command_operations -lCaretCommandOperations \
        -L../caret_brain_set -lCaretBrainSet \
        -L../caret_vtk4_classes -lCaretVtk4Classes \
        -L../caret_gifti -lCaretGifti \
        -L../caret_files -lCaretFiles \
        -L../caret_uniformize -lCaretUniformize \
        -L../caret_statistics -lCaretStatistics \
        -L../caret_common -lCaretCommon \
        -L../caret_widgets -lCaretWidgets \
        -L../caret_cifti -lCaretCifti
    
    LIBS += \
        -framework Carbon \
        -framework Quicktime 
    LIBS += $$VTK_LIBS
    #   RC_FILE		= caret5.icns
    #   QMAKE_INFO_PLIST 	= Info.plist

    contains( DEFINES, HAVE_QWT ) {
        LIBS += $$QWT_LIBS
    }
    contains( DEFINES, HAVE_MINC ) {
        LIBS += $$NETCDF_LIBS
    }

    #
    # Icon for Mac version of Caret
    #
    ICON = caret5.icns

    #
    # QMAKE_POST_LINK may have "strip" if release
    # so need semicolon between commands
    #
    #  qt_menu.nib must be in resources or application will abort
    #
    !isEmpty( QMAKE_POST_LINK) {
        QMAKE_POST_LINK += ;
    }
    QMAKE_POST_LINK += cp -r $$(QTDIR)/src/gui/mac/qt_menu.nib ./caret5.app/Contents/Resources
}

ubuntu {
   PRE_TARGETDEPS +=  \
      ../caret_gui/libCaretGui.so \
      ../caret_command_operations/libCaretCommandOperations.so \
           ../caret_brain_set/libCaretBrainSet.so \
           ../caret_gifti/libCaretGifti.so \
           ../caret_files/libCaretFiles.so \
           ../caret_uniformize/libCaretUniformize.so \
           ../caret_statistics/libCaretStatistics.so \
           ../caret_common/libCaretCommon.so \
           ../caret_widgets/libCaretWidgets.so \
           ../caret_cifti/libCaretCifti.so
   LIBS +=  \
      -L../caret_gui -lCaretGui \
      -L../caret_command_operations -lCaretCommandOperations \
           -L../caret_brain_set -lCaretBrainSet \
           -L../caret_gifti -lCaretGifti \
           -L../caret_files -lCaretFiles \
           -L../caret_uniformize -lCaretUniformize \
           -L../caret_statistics -lCaretStatistics \
           -L../caret_common -lCaretCommon \
           -L../caret_widgets -lCaretWidgets \
           -L../caret_cifti -lCaretCifti
   
   LIBS += $$VTK_LIBS
   LIBS -= -lvtkjpeg -lvtkpng -lvtkexpat -lvtkzlib
   LIBS += -ldl
   QTPLUGIN -= qgif qjpeg qtiff
   QMAKE_CXXFLAGS += -DUBUNTU

    contains( DEFINES, HAVE_MINC ) {
        LIBS += $$NETCDF_LIBS
    }
}

unix:!macx:!ubuntu {
   LIBS +=  \
        -L../caret_gui -lCaretGui \
        -L../caret_command_operations -lCaretCommandOperations \
        -L../caret_brain_set -lCaretBrainSet \
        -L../caret_vtk4_classes -lCaretVtk4Classes \
        -L../caret_gifti -lCaretGifti \
        -L../caret_files -lCaretFiles \
        -L../caret_uniformize -lCaretUniformize \
        -L../caret_statistics -lCaretStatistics \
        -L../caret_common -lCaretCommon \
        -L../caret_widgets -lCaretWidgets \
        -L../caret_cifti -lCaretCifti

    LIBS += $$VTK_LIBS
    contains( DEFINES, HAVE_QWT ) {
        LIBS += $$QWT_LIBS
    }
    contains( DEFINES, HAVE_MINC ) {
        LIBS += $$NETCDF_LIBS
    }
    dll:LIBS -= -ldl
    dll:LIBS += -ldl
}

unix:!macx:release:!debug:!profile {
    QMAKE_POST_LINK=strip --strip-debug $(TARGET)
}

# Input
#HEADERS +=

SOURCES += main.cxx

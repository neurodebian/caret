######################################################################
# Automatically generated by qmake (1.04a) Tue Jan 14 11:58:13 2003
######################################################################

!vs:TEMPLATE = app
vs:TEMPLATE=vcapp
#
# For JPEG support with static linking
#
contains(QT_VERSION, ^4\\.[0-6]\\..*) {
   QTPLUGIN += qgif qjpeg qtiff
}

LIBS     += -L$(QTDIR)/plugins/imageformats

CONFIG   += qt console
INCLUDEPATH += .

include(../caret_qmake_include.pro)  

unix:release:!debug:!profile {
   QMAKE_POST_LINK=strip -S $(TARGET)
   !macx:QMAKE_POST_LINK=strip $(TARGET)
}

unix:!ubuntu {
   !dll {
       PRE_TARGETDEPS +=  \
        ../caret_command_operations/libCaretCommandOperations.a \
        ../caret_brain_set/libCaretBrainSet.a \
        ../caret_vtk4_classes/libCaretVtk4Classes.a \
        ../caret_files/libCaretFiles.a \
        ../caret_uniformize/libCaretUniformize.a \
        ../caret_statistics/libCaretStatistics.a \
        ../caret_common/libCaretCommon.a \
        ../caret_widgets/libCaretWidgets.a      
    }

   dll {
       PRE_TARGETDEPS +=  \
           ../caret_command_operations/libCaretCommandOperations.so \
           ../caret_brain_set/libCaretBrainSet.so \
           ../caret_vtk4_classes/libCaretVtk4Classes.so \
           ../caret_files/libCaretFiles.so \
           ../caret_uniformize/libCaretUniformize.so \
           ../caret_statistics/libCaretStatistics.so \
           ../caret_common/libCaretCommon.so \
           ../caret_widgets/libCaretWidgets.so
           LIBS += -ldl 
    }
}

win32 {
    !vs {
        debug {
            LIBS += \
                ..\caret_command_operations\debug\libCaretCommandOperations.a \
                ..\caret_brain_set\debug\libCaretBrainSet.a \
                ..\caret_vtk4_classes\debug\libCaretVtk4Classes.a \
                ..\caret_files\debug\libCaretFiles.a \
                ..\caret_widgets\debug\libCaretWidgets.a \
                ..\caret_uniformize\debug\libCaretUniformize.a \
                ..\caret_statistics\debug\libCaretStatistics.a \
                ..\caret_common\debug\libCaretCommon.a 
        }
        
        release {
            LIBS += \
                ..\caret_command_operations\release\libCaretCommandOperations.a \
                ..\caret_brain_set\release\libCaretBrainSet.a \
                ..\caret_vtk4_classes\release\libCaretVtk4Classes.a \
                ..\caret_files\release\libCaretFiles.a \
                ..\caret_widgets\release\libCaretWidgets.a \
                ..\caret_uniformize\release\libCaretUniformize.a \
                ..\caret_statistics\release\libCaretStatistics.a \
                ..\caret_common\release\libCaretCommon.a 
        }
        LIBS += $$VTK_LIBS
    }

    vs {
        CONFIG(debug,debug|release) {
            LIBS += ..\caret_common\debug\CaretCommon.lib \
                ..\caret_command_operations\debug\CaretCommandOperations.lib \
                ..\caret_brain_set\debug\CaretBrainSet.lib \
                ..\caret_vtk4_classes\debug\CaretVtk4Classes.lib \
                ..\caret_files\debug\CaretFiles.lib \
                ..\caret_uniformize\debug\CaretUniformize.lib \
                ..\caret_statistics\debug\CaretStatistics.lib \
                ..\caret_widgets\debug\CaretWidgets.lib \
                ..\caret_common\debug\CaretCommon.lib
            LIBS-=$$VTK_RELEASE_LIBS
            LIBS+=$$VTK_LIBS
        }
        CONFIG(release,debug|release) {
            LIBS += ..\caret_common\release\CaretCommon.lib \
                ..\caret_command_operations\release\CaretCommandOperations.lib \
                ..\caret_brain_set\release\CaretBrainSet.lib \
                ..\caret_vtk4_classes\release\CaretVtk4Classes.lib \
                ..\caret_files\release\CaretFiles.lib \
                ..\caret_uniformize\release\CaretUniformize.lib \
                ..\caret_statistics\release\CaretStatistics.lib \
                ..\caret_widgets\release\CaretWidgets.lib \
                ..\caret_common\release\CaretCommon.lib
            LIBS-=$$VTK_LIBS
            LIBS+=$$VTK_RELEASE_LIBS
        }

    }
        
    contains( DEFINES, HAVE_MINC ):LIBS += $$NETCDF_LIBS    
}

win32:debug {
   CONFIG += console
}

macx {
   #CONFIG -= app_bundle
   LIBS += \
           -L../caret_command_operations -lCaretCommandOperations \
           -L../caret_brain_set -lCaretBrainSet \
           -L../caret_vtk4_classes -lCaretVtk4Classes \
           -L../caret_files -lCaretFiles \
           -L../caret_widgets -lCaretWidgets \
           -L../caret_uniformize -lCaretUniformize \
           -L../caret_statistics -lCaretStatistics \
           -L../caret_common -lCaretCommon 

   contains ( DEFINES, HAVE_ITK ) {
      LIBS += $$ITK_LIBS
   }
   
   LIBS += $$VTK_LIBS
   contains( DEFINES, HAVE_MINC ):LIBS += $$NETCDF_LIBS

    #
    # QMAKE_POST_LINK may have "strip" if release
    # so need semicolon between commands
    #
    #  qt_menu.nib must be in resources or application will abort
    #
    !isEmpty( QMAKE_POST_LINK) {
        QMAKE_POST_LINK += ;
    }
    QMAKE_POST_LINK += cp -r $$(QTDIR)/src/gui/mac/qt_menu.nib ./caret_command.app/Contents/Resources
}

ubuntu {
   PRE_TARGETDEPS +=  \
      ../caret_command_operations/libCaretCommandOperations.so \
           ../caret_brain_set/libCaretBrainSet.so \
           ../caret_files/libCaretFiles.so \
           ../caret_uniformize/libCaretUniformize.so \
           ../caret_statistics/libCaretStatistics.so \
           ../caret_common/libCaretCommon.so \
           ../caret_widgets/libCaretWidgets.so
   LIBS +=  \
      -L../caret_command_operations -lCaretCommandOperations \
           -L../caret_brain_set -lCaretBrainSet \
           -L../caret_files -lCaretFiles \
           -L../caret_uniformize -lCaretUniformize \
           -L../caret_statistics -lCaretStatistics \
           -L../caret_common -lCaretCommon \
           -L../caret_widgets -lCaretWidgets

   LIBS += $$VTK_LIBS

   contains( DEFINES, HAVE_MINC ):LIBS += $$NETCDF_LIBS
   
   LIBS -= -lvtkjpeg -lvtkpng -lvtkexpat -lvtkzlib
   LIBS += -ldl
   QTPLUGIN -= qgif qjpeg qtiff
   QMAKE_CXXFLAGS += -DUBUNTU
}

unix:!macx:!ubuntu {
   LIBS += \
           -L../caret_command_operations -lCaretCommandOperations \
           -L../caret_brain_set -lCaretBrainSet \
           -L../caret_vtk4_classes -lCaretVtk4Classes \
           -L../caret_files -lCaretFiles \
           -L../caret_widgets -lCaretWidgets \
           -L../caret_uniformize -lCaretUniformize \
           -L../caret_statistics -lCaretStatistics \
           -L../caret_common -lCaretCommon 

   LIBS += $$VTK_LIBS
   
   contains( DEFINES, HAVE_MINC ):LIBS += $$NETCDF_LIBS
}

# Input
#HEADERS += 

SOURCES += main.cxx

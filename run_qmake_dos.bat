cd caret
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\qmake\qmake -makefile -t app
cd ..
cd caret_brain_set
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\qmake\qmake -makefile -t lib
cd ..
cd caret_common
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\qmake\qmake -makefile -t lib
cd ..
cd caret_command_operations
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\qmake\qmake -makefile -t lib
cd ..
cd caret_statistics
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\qmake\qmake -makefile -t lib
cd ..
cd caret_vtk4_classes
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\qmake\qmake -makefile -t lib
cd ..
cd caret_uniformize
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\qmake\qmake -makefile -t lib
cd ..
cd caret_files
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\qmake\qmake -makefile -t lib
cd ..
cd caret_widgets
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\qmake\qmake -makefile -t lib
cd ..
cd caret_command
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM  %QTDIR%\qmake\qmake -makefile -t app
cd ..
cd caret_edit
%QTDIR%\qmake\qmake "CONFIG += release" "CONFIG -= debug"
REM  %QTDIR%\qmake\qmake -makefile -t app
cd ..

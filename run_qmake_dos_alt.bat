cd caret5
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t app
cd ..
cd caret_gui
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_brain_set
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_common
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_command_operations
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_statistics
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_vtk4_classes
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_uniformize
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_files
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_cifti
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_gifti
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_widgets
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM %QTDIR%\bin\qmake -makefile -t lib
cd ..
cd caret_command
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM  %QTDIR%\bin\qmake -makefile -t app
cd ..
cd caret_edit
%QTDIR%\bin\qmake "CONFIG += release" "CONFIG -= debug"
REM  %QTDIR%\bin\qmake -makefile -t app
cd ..

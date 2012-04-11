cd ..\caret
%QTDIR%\bin\qmake -t app "CONFIG+= nmake"
cd ..\caret_brain_set
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..\caret_cifti
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..\caret_common
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..\caret_gifti
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..\caret_command_operations 
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..\caret_statistics
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..\caret_vtk4_classes
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..\caret_uniformize
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..\caret_files
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..\caret_widgets
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..\caret_command
%QTDIR%\bin\qmake -t app "CONFIG+= nmake"
cd ..\caret_edit
%QTDIR%\bin\qmake -t app "CONFIG+= nmake"
cd ..\windows


cd ..\caret
%QTDIR%\bin\qmake -t app "CONFIG+= nmake"
cd ..
cd caret_brain_set
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..
cd caret_cifti
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..
cd caret_common
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..
cd caret_command_operations 
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..
cd caret_statistics
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..
cd caret_vtk4_classes
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..
cd caret_uniformize
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..
cd caret_files
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..
cd caret_widgets
%QTDIR%\bin\qmake -t lib "CONFIG+= nmake"
cd ..
cd caret_command
%QTDIR%\bin\qmake -t app "CONFIG+= nmake"
cd ..
cd caret_edit
%QTDIR%\bin\qmake -t app "CONFIG+= nmake"
cd ..\windows


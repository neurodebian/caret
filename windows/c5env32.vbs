Set objShell = WScript.CreateObject("WScript.Shell")
Set colUsrEnvVars = objShell.Environment("USER")

QtPath = "C:\dev32\install\Qt\bin"
CCommandPath = "C:\dev32\caret5_source\caret_command\debug;C:\dev32\caret5_source\caret_command\release"
Path = colUsrEnvVars("OLDPATH")

If Len(Path) = 0 Then
	colUsrEnvVars("OLDPATH") = colUsrEnvVars("PATH")
    'objShell.ExpandEnvironmentStrings("%PATH%")
	WScript.Echo colUsrEnvVars("OLDPATH")
    Path = colUsrEnvVars("OLDPATH")
End If

NewPath =  (QtPath & ";" & CCommandPath & ";" & Path)
colUsrEnvVars("PATH") = (NewPath)

colUsrEnvVars("QTDIR") = "C:\dev32\install\Qt"
colUsrEnvVars("VTK_LIB_DIR") = "C:\dev32\install\VTKDebug\lib\vtk-5.6"
colUsrEnvVars("VTK_INC_DIR") = "C:\dev32\install\VTKDebug\include\vtk-5.6"
colUsrEnvVars("QWT_LIB_DIR") = "C:\dev32\install\qwt-5.2.1\lib"
colUsrEnvVars("QWT_INC_DIR") = "C:\dev32\install\qwt-5.2.1\include"
colUsrEnvVars("ZLIB_LIB_DIR") = "C:\dev32\install\zlib\lib"
colUsrEnvVars("ZLIB_INC_DIR") = "C:\dev32\install\zlib\include"

colUsrEnvVars("VTK_RELEASE_LIB_DIR") = "C:\dev32\install\VTKRelease\lib\vtk-5.6"
colUsrEnvVars("VTK_RELEASE_INC_DIR") = "C:\dev32\install\VTKRelease\include\vtk-5.6"
colUsrEnvVars("NETCDF_INC_DIR") = "C:\dev32\install\minc-netcdf\include"
colUsrEnvVars("NETCDF_LIB_DIR") = "C:\dev32\install\minc-netcdf\lib"

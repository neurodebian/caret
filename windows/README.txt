c5env32.bat - sets up 32 bit caret environment variabls in a local command prompt session
c5env32.vbs - sets up 64 bit caret environemnt variables system wide (this will affect ALL applications, i.e. Visual Studio, however, they have to launched AFTER running this script)
c5env64.bat - sets up 32 bit caret environment variabls in a local command prompt session
c5env64.vbs - sets up 64 bit caret environemtt variables system wide (this will affect ALL applications, i.e. Visual Studio, however, they have to launched AFTER running this script)

Note that it is possible to compile different versions of caret in separate command prompt session.  In contrast the Visual Studio IDE relies on system wide environment variables.  Therefore, it is not
recommended that you try to compile both 64 and 32 bit version of caret inside visual studio at the same time (even in different sessions).

After setting the environment variables for a session using the scripts above, next execute:
run_qmake_vs.bat

Next, launch visual studio and open caret5.sln

For command line building, run:
run_qmake_nmake.bat
run_nmake.bat
Echo --------------------------------------------------------------------
Echo Generating documentation...
cd .\docs
CALL doxygen "EMULATORS Doxy Configuration"
CALL doxygen "EMULATORS Doxy Configuration No SRC"
cd ..\
Echo --------------------------------------------------------------------

Echo --------------------------------------------------------------------
Echo Package the documentation...
cd .\docs
DEL .\No\html.zip /Q
CALL "C:\Program Files\7-Zip\7z.exe" a -tzip .\No\html.zip .\No\html
cd ..\
Echo --------------------------------------------------------------------

Echo --------------------------------------------------------------------
Echo The final result is copied into the products directory to Github...
DEL ".\products\EMULATOTRS\include\*.*" /S/Q
RMDIR ".\products\EMULATORS\include" /S/Q
XCOPY ".\include\*.*" ".\products\EMULATORS\include\*.*" /s
DEL ".\products\EMULATOTRS\src\*.*" /S/Q
RMDIR ".\products\EMULATORS\src" /S/Q
XCOPY ".\src\*.*" ".\products\EMULATORS\src\*.*" /s
DEL ".\products\EMULATOTRS\tests\*.*" /S/Q
RMDIR ".\products\EMULATORS\tests" /S/Q
XCOPY ".\tests\*.*" ".\products\EMULATORS\tests\*.*" /s
DEL ".\products\EMULATOTRS\projects\*.*" /S/Q
RMDIR ".\products\EMULATORS\projects" /S/Q
XCOPY ".\projects\*.*" ".\products\EMULATORS\projects\*.*" /s
DEL ".\products\EMULATOTRS\emulators\*.*" /S/Q
RMDIR ".\products\EMULATORS\emulators" /S/Q
XCOPY ".\emulators\*.*" ".\products\EMULATORS\emulators\*.*" /s
COPY ".\docs\No\html.zip" ".\products\EMULATORS\html.zip" 
Echo --------------------------------------------------------------------

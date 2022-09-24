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

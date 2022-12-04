Echo --------------------------------------------------------------------
Echo Copying files...
COPY .\exe\x86\C64Emulator.exe .\emulators\C64Emulator\ /Y
COPY .\exe\x86\C64EmulatorD.exe .\emulators\C64Emulator\ /Y
COPY .\exe\x86\C64EmulatorC.exe .\emulators\C64EmulatorC\ /Y
COPY .\exe\x86\C64EmulatorCD.exe .\emulators\C64EmulatorC\ /Y
COPY .\exe\x86\RConsole.exe .\monitors\RConsole\ /Y
COPY .\exe\x86\RConsoleD.exe .\monitors\RConsole\ /Y
COPY .\exe\x86\R64Console.exe .\monitors\R64Console\ /Y
COPY .\exe\x86\R64ConsoleD.exe .\monitors\R64Console\ /Y
COPY .\exe\x86\TestCPU.exe .\tests\TestCPU\ /Y
COPY .\exe\x86\TestCPUD.exe .\tests\TestCPU\ /Y
COPY .\exe\x86\Test6500.exe .\tests\Test6500\ /Y
COPY .\exe\x86\Test6500D.exe .\tests\Test6500\ /Y

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

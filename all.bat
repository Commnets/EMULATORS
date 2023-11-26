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
COPY .\exe\x86\C64Compiler.exe .\compilers\C64Compiler\ /Y
COPY .\exe\x86\C64CompilerD.exe .\compilers\C64Compiler\ /Y
COPY .\exe\x86\Z80Compiler.exe .\compilers\Z80Compiler\ /Y
COPY .\exe\x86\Z80CompilerD.exe .\compilers\Z80Compiler\ /Y
COPY .\exe\x86\TestCPU.exe .\tests\TestCPU\ /Y
COPY .\exe\x86\TestCPUD.exe .\tests\TestCPU\ /Y
COPY .\exe\x86\Test6500.exe .\tests\Test6500\ /Y
COPY .\exe\x86\Test6500D.exe .\tests\Test6500\ /Y
COPY .\exe\x86\TestZ80.exe .\tests\TestZ80\ /Y
COPY .\exe\x86\TestZ80D.exe .\tests\TestZ80\ /Y

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
Echo Creates the instalation C64 package...
COPY .\exe\x86\C64Emulator.exe .\setups\C64\ /Y
COPY .\exe\x86\C64EmulatorC.exe .\setups\C64\ /Y
COPY .\exe\x86\R64Console.exe .\setups\C64\ /Y
COPY .\exe\x86\C64Compiler.exe .\setups\C64\ /Y
COPY .\exe\x86\Test6500.exe .\setups\C64\ /Y
COPY .\emulators\C64Commons\*.* .\setups\C64\ /Y
COPY .\projects\C64\*.fmt .\setups\C64\ /Y
COPY .\projects\C64\*.hlp .\setups\C64\ /Y
COPY .\projects\COMMODORE\*.fmt .\setups\C64\ /Y
COPY .\projects\COMMODORE\*.hlp .\setups\C64\ /Y
COPY .\projects\CONSOLE\*.fmt .\setups\C64\ /Y
COPY .\projects\CONSOLE\*.hlp .\setups\C64\ /Y
COPY .\projects\CORE\*.fmt .\setups\C64\ /Y
COPY .\projects\CORE\*.hlp .\setups\C64\ /Y
COPY .\projects\ASSEMBLER\*.fmt .\setups\C64\ /Y
"C:\Program files (x86)\Inno Setup 6\iscc" ".\setups\C64\C64.iss"



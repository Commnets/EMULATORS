Echo --------------------------------------------------------------------
Echo PHASE 1: Copying exe and configuration files to the right destination
Echo --------------------------------------------------------------------
Echo ********************************************************************
Echo 1.1: Common Things
Echo ********************************************************************
Echo Exe files...
COPY .\exe\x86\RConsole.exe .\monitors\RConsole\ /Y
COPY .\exe\x86\RConsoleD.exe .\monitors\RConsole\ /Y
COPY .\exe\x86\Test6500.exe .\tests\Test6500\ /Y
COPY .\exe\x86\Test6500D.exe .\tests\Test6500\ /Y
COPY .\exe\x86\TestZ80.exe .\tests\TestZ80\ /Y
COPY .\exe\x86\TestZ80D.exe .\tests\TestZ80\ /Y
COPY .\exe\x86\TestCPU.exe .\tests\TestCPU\ /Y
COPY .\exe\x86\TestCPUD.exe .\tests\TestCPU\ /Y
COPY .\exe\x86\6500Compiler.exe .\compilers\6500Compiler\ /Y
COPY .\exe\x86\6500CompilerD.exe .\compilers\6500Compiler\ /Y
COPY .\exe\x86\Z80Compiler.exe .\compilers\Z80Compiler\ /Y
COPY .\exe\x86\Z80CompilerD.exe .\compilers\Z80Compiler\ /Y
Echo Configuration files...
DEL  .\compilers\6500Compiler\*.fmt
DEL  .\compilers\RConsole\*.fmt
DEL  .\compilers\RConsole\*.hlp
COPY .\projects\ASSEMBLER\*.fmt .\compilers\6500Compiler\ /Y
COPY .\projects\CORE\*.fmt .\monitors\RConsole\ /Y
COPY .\projects\CORE\*.hlp .\monitors\RConsole\ /Y
Echo ********************************************************************
Echo ********************************************************************
Echo 1.2: C64
Echo ********************************************************************
Echo Exe files...
COPY .\exe\x86\C64Emulator.exe .\emulators\C64Emulator\ /Y
COPY .\exe\x86\C64EmulatorD.exe .\emulators\C64Emulator\ /Y
COPY .\exe\x86\C64EmulatorC.exe .\emulators\C64EmulatorC\ /Y
COPY .\exe\x86\C64EmulatorCD.exe .\emulators\C64EmulatorC\ /Y
COPY .\exe\x86\R64Console.exe .\monitors\R64Console\ /Y
COPY .\exe\x86\R64ConsoleD.exe .\monitors\R64Console\ /Y
Echo Configuration files...
DEL  .\emulators\C64Emulator\*.fmt
DEL  .\emulators\C64EmulatorC\*.fmt
DEL  .\monitors\R64Console\*.fmt
DEL  .\emulators\C64Emulator\*.hlp
DEL  .\emulators\C64EmulatorC\*.hlp
DEL  .\monitors\R64Console\*.hlp
COPY .\projects\C64\*.fmt .\emulators\C64Emulator\ /Y
DEL .\emulators\C64Emulator\C64RConsole.fmt
COPY .\projects\C64\*.fmt .\emulators\C64EmulatorC\ /Y
DEL .\emulators\C64EmulatorC\C64RConsole.fmt
COPY .\projects\C64\*.fmt .\monitors\R64Console\ /Y
DEL .\monitors\R64Console\C64.fmt
RENAME .\monitors\R64Console\C64RConsole.fmt C64.fmt
COPY .\projects\C64\*.hlp .\emulators\C64EmulatorC\ /Y
DEL .\emulators\C64EmulatorC\C64RConsole.hlp
COPY .\projects\C64\*.hlp .\monitors\R64Console\ /Y
DEL .\monitors\R64Console\C64.hlp
RENAME .\monitors\R64Console\C64RConsole.hlp C64.hlp
COPY .\projects\COMMODORE\*.fmt .\emulators\C64EmulatorC\ /Y
COPY .\projects\COMMODORE\*.fmt .\emulators\C64Emulator\ /Y
COPY .\projects\COMMODORE\*.fmt .\monitors\R64Console\ /Y
COPY .\projects\COMMODORE\*.hlp .\emulators\C64EmulatorC\ /Y
COPY .\projects\COMMODORE\*.hlp .\monitors\R64Console\ /Y
COPY .\projects\CONSOLE\*.fmt .\emulators\C64EmulatorC\ /Y
COPY .\projects\CONSOLE\*.hlp .\emulators\C64EmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\C64EmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\C64Emulator\ /Y
COPY .\projects\CORE\*.fmt .\monitors\R64Console\ /Y
COPY .\projects\CORE\*.hlp .\emulators\C64EmulatorC\ /Y
COPY .\projects\CORE\*.hlp .\emulators\C64Emulator\ /Y
COPY .\projects\CORE\*.hlp .\monitors\R64Console\ /Y
Echo ********************************************************************
Echo ********************************************************************
Echo 1.3: VIC20
Echo ********************************************************************
Echo Exe files...
COPY .\exe\x86\VIC20Emulator.exe .\emulators\VIC20Emulator\ /Y
COPY .\exe\x86\VIC20EmulatorD.exe .\emulators\VIC20Emulator\ /Y
COPY .\exe\x86\VIC20EmulatorC.exe .\emulators\VIC20EmulatorC\ /Y
COPY .\exe\x86\VIC20EmulatorCD.exe .\emulators\VIC20EmulatorC\ /Y
COPY .\exe\x86\RVIC20Console.exe .\monitors\RVIC20Console\ /Y
COPY .\exe\x86\RVIC20ConsoleD.exe .\monitors\RVIC20Console\ /Y
COPY .\exe\x86\VIC20CRTGenerator.exe .\projects\VIC20CRTGenerator\ /Y
COPY .\exe\x86\VIC20CRTGeneratorD.exe .\projects\VIC20CRTGenerator\ /Y
Echo Configuration files...
DEL  .\emulators\VIC20Emulator\*.fmt
DEL  .\emulators\VIC20EmulatorC\*.fmt
DEL  .\monitors\RVIC20Console\*.fmt
DEL  .\emulators\VIC20Emulator\*.hlp
DEL  .\emulators\VIC20EmulatorC\*.hlp
DEL  .\monitors\RVIC20Console\*.hlp
COPY .\projects\VIC20\*.fmt .\emulators\VIC20Emulator\ /Y
DEL .\emulators\VIC20Emulator\VIC20RConsole.fmt
COPY .\projects\VIC20\*.fmt .\emulators\VIC20EmulatorC\ /Y
DEL .\emulators\VIC20EmulatorC\VIC20RConsole.fmt
COPY .\projects\VIC20\*.fmt .\monitors\RVIC20Console\ /Y
DEL .\monitors\RVIC20Console\VIC20.fmt
RENAME .\monitors\RVIC20Console\VIC20RConsole.fmt VIC20.fmt
COPY .\projects\VIC20\*.hlp .\emulators\VIC20EmulatorC\ /Y
DEL .\emulators\VIC20EmulatorC\VIC20RConsole.hlp
COPY .\projects\VIC20\*.hlp .\monitors\RVIC20Console\ /Y
DEL .\monitors\RVIC20Console\VIC20.hlp
RENAME .\monitors\RVIC20Console\VIC20RConsole.hlp VIC20.hlp
COPY .\projects\COMMODORE\*.fmt .\emulators\VIC20EmulatorC\ /Y
COPY .\projects\COMMODORE\*.fmt .\emulators\VIC20Emulator\ /Y
COPY .\projects\COMMODORE\*.fmt .\monitors\RVIC20Console\ /Y
COPY .\projects\COMMODORE\*.hlp .\emulators\VIC20EmulatorC\ /Y
COPY .\projects\COMMODORE\*.hlp .\monitors\RVIC20Console\ /Y
COPY .\projects\CONSOLE\*.fmt .\emulators\VIC20EmulatorC\ /Y
COPY .\projects\CONSOLE\*.hlp .\emulators\VIC20EmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\VIC20EmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\VIC20Emulator\ /Y
COPY .\projects\CORE\*.fmt .\monitors\RVIC20Console\ /Y
COPY .\projects\CORE\*.hlp .\emulators\VIC20EmulatorC\ /Y
COPY .\projects\CORE\*.hlp .\emulators\VIC20Emulator\ /Y
COPY .\projects\CORE\*.hlp .\monitors\RVIC20Console\ /Y
Echo ********************************************************************
Echo
Echo
Echo --------------------------------------------------------------------
Echo PHASE 2: Documentation 
Echo --------------------------------------------------------------------
Echo ********************************************************************
Echo 2.1: DOXY Info
Echo ********************************************************************
cd .\docs
CALL doxygen "EMULATORS Doxy Configuration"
CALL doxygen "EMULATORS Doxy Configuration No SRC"
cd ..\
Echo ********************************************************************
Echo 2.2: Packageing DOCS
Echo ********************************************************************
cd .\docs
DEL .\No\html.zip /Q
CALL "C:\Program Files\7-Zip\7z.exe" a -tzip .\No\html.zip .\No\html
cd ..\
Echo ********************************************************************
Echo
Echo
Echo --------------------------------------------------------------------
Echo PHASE 3: Instalation files 
Echo --------------------------------------------------------------------
Echo ********************************************************************
Echo 3.1: CBM64
Echo ********************************************************************
COPY .\exe\x86\C64Emulator.exe .\setups\C64\ /Y
COPY .\exe\x86\C64EmulatorC.exe .\setups\C64\ /Y
COPY .\exe\x86\R64Console.exe .\setups\C64\ /Y
COPY .\exe\x86\6500Compiler.exe .\setups\C64\ /Y
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
Echo ********************************************************************
Echo 3.2: VIC20
Echo ********************************************************************
COPY .\exe\x86\VIC20EmulatorC.exe .\setups\VIC20\ /Y
COPY .\exe\x86\VIC20Emulator.exe .\setups\VIC20\ /Y
COPY .\exe\x86\RVIC20Console.exe .\setups\VIC20\ /Y
COPY .\exe\x86\6500Compiler.exe .\setups\VIC20\ /Y
COPY .\exe\x86\Test6500.exe .\setups\VIC20\ /Y
COPY .\exe\x86\VIC20CRTGenerator.exe .\setups\VIC20\ /Y
COPY .\emulators\VIC20Commons\*.* .\setups\VIC20\ /Y
COPY .\projects\VIC20\*.fmt .\setups\VIC20\ /Y
COPY .\projects\VIC20\*.hlp .\setups\VIC20\ /Y
COPY .\projects\COMMODORE\*.fmt .\setups\VIC20\ /Y
COPY .\projects\COMMODORE\*.hlp .\setups\VIC20\ /Y
COPY .\projects\CONSOLE\*.fmt .\setups\VIC20\ /Y
COPY .\projects\CONSOLE\*.hlp .\setups\VIC20\ /Y
COPY .\projects\CORE\*.fmt .\setups\VIC20\ /Y
COPY .\projects\CORE\*.hlp .\setups\VIC20\ /Y
COPY .\projects\ASSEMBLER\*.fmt .\setups\VIC20\ /Y
"C:\Program files (x86)\Inno Setup 6\iscc" ".\setups\VIC20\VIC20.iss"
Echo ********************************************************************
Echo
Echo
Echo Finished!

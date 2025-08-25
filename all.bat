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
COPY .\exe\x86\TestInstZ80.exe .\tests\TestInstZ80\ /Y
COPY .\exe\x86\TestInstZ80D.exe .\tests\TestInstZ80\ /Y
COPY .\exe\x86\TestInst6500.exe .\tests\TestInst6500\ /Y
COPY .\exe\x86\TestInst6500D.exe .\tests\TestInst6500\ /Y
COPY .\exe\x86\TestCPU.exe .\tests\TestCPU\ /Y
COPY .\exe\x86\TestCPUD.exe .\tests\TestCPU\ /Y
COPY .\exe\x86\6500Compiler.exe .\compilers\6500Compiler\ /Y
COPY .\exe\x86\6500CompilerD.exe .\compilers\6500Compiler\ /Y
COPY .\exe\x86\Z80Compiler.exe .\compilers\Z80Compiler\ /Y
COPY .\exe\x86\Z80CompilerD.exe .\compilers\Z80Compiler\ /Y
Echo Configuration files...
DEL  .\compilers\6500Compiler\*.fmt
DEL  .\compilers\Z80Compiler\*.fmt
DEL  .\compilers\RConsole\*.fmt
DEL  .\compilers\RConsole\*.hlp
COPY .\projects\ASSEMBLER\*.fmt .\compilers\6500Compiler\ /Y
COPY .\projects\ASSEMBLER\*.fmt .\compilers\Z80Compiler\ /Y
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
DEL  .\monitors\R64Console\*.def
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
COPY .\projects\C64\*.def .\monitors\R64Console\ /Y
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
DEL  .\monitors\RVIC20Console\*.def
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
COPY .\projects\VIC20\*.def .\emulators\VIC20EmulatorC\ /Y
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
Echo ********************************************************************
Echo 1.4: 264 Series
Echo ********************************************************************
Echo Exe files...
COPY .\exe\x86\C264Emulator.exe .\emulators\C264Emulator\ /Y
COPY .\exe\x86\C264EmulatorD.exe .\emulators\C264Emulator\ /Y
COPY .\exe\x86\C264EmulatorC.exe .\emulators\C264EmulatorC\ /Y
COPY .\exe\x86\C264EmulatorCD.exe .\emulators\C264EmulatorC\ /Y
COPY .\exe\x86\RVIC20Console.exe .\monitors\R264Console\ /Y
COPY .\exe\x86\RVIC20ConsoleD.exe .\monitors\R264Console\ /Y
Echo Configuration files...
DEL  .\emulators\C264Emulator\*.fmt
DEL  .\emulators\C264EmulatorC\*.fmt
DEL  .\monitors\R264Console\*.fmt
DEL  .\monitors\R264Console\*.def
DEL  .\emulators\C264Emulator\*.hlp
DEL  .\emulators\C264EmulatorC\*.hlp
DEL  .\monitors\R264Console\*.hlp
COPY .\projects\C264\*.fmt .\emulators\C264Emulator\ /Y
DEL .\emulators\C264Emulator\C264RConsole.fmt
COPY .\projects\C264\*.fmt .\emulators\C264EmulatorC\ /Y
DEL .\emulators\C264EmulatorC\C264RConsole.fmt
COPY .\projects\C264\*.fmt .\monitors\R264Console\ /Y
DEL .\monitors\R264Console\C264.fmt
RENAME .\monitors\R264Console\C264RConsole.fmt C264.fmt
COPY .\projects\C264\*.def .\emulators\C264EmulatorC\ /Y
COPY .\projects\C264\*.hlp .\emulators\C264EmulatorC\ /Y
DEL .\emulators\C264EmulatorC\C264RConsole.hlp
COPY .\projects\C264\*.hlp .\monitors\R264Console\ /Y
DEL .\monitors\R264Console\C264.hlp
RENAME .\monitors\R264Console\C264RConsole.hlp C264.hlp
COPY .\projects\COMMODORE\*.fmt .\emulators\C264EmulatorC\ /Y
COPY .\projects\COMMODORE\*.fmt .\emulators\C264Emulator\ /Y
COPY .\projects\COMMODORE\*.fmt .\monitors\R264Console\ /Y
COPY .\projects\COMMODORE\*.hlp .\emulators\C264EmulatorC\ /Y
COPY .\projects\COMMODORE\*.hlp .\monitors\R264Console\ /Y
COPY .\projects\CONSOLE\*.fmt .\emulators\C264EmulatorC\ /Y
COPY .\projects\CONSOLE\*.hlp .\emulators\C264EmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\C264EmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\C264Emulator\ /Y
COPY .\projects\CORE\*.fmt .\monitors\R264Console\ /Y
COPY .\projects\CORE\*.hlp .\emulators\C264EmulatorC\ /Y
COPY .\projects\CORE\*.hlp .\emulators\C264Emulator\ /Y
COPY .\projects\CORE\*.hlp .\monitors\R264Console\ /Y
Echo ********************************************************************
Echo ********************************************************************
Echo 1.5: ZX81
Echo ********************************************************************
Echo Exe files...
COPY .\exe\x86\ZX81Emulator.exe .\emulators\ZX81Emulator\ /Y
COPY .\exe\x86\ZX81EmulatorD.exe .\emulators\ZX81Emulator\ /Y
COPY .\exe\x86\ZX81EmulatorC.exe .\emulators\ZX81EmulatorC\ /Y
COPY .\exe\x86\ZX81EmulatorCD.exe .\emulators\ZX81EmulatorC\ /Y
COPY .\exe\x86\RZX81Console.exe .\monitors\RZX81Console\ /Y
COPY .\exe\x86\RZX81ConsoleD.exe .\monitors\RZX81Console\ /Y
COPY .\exe\x86\ZX81CRTGenerator.exe .\projects\ZX81CRTGenerator\ /Y
COPY .\exe\x86\ZX81CRTGeneratorD.exe .\projects\ZX81CRTGenerator\ /Y
Echo Configuration files...
DEL  .\emulators\ZX81Emulator\*.fmt
DEL  .\emulators\ZX81EmulatorC\*.fmt
DEL  .\monitors\RZX81Console\*.fmt
DEL  .\monitors\RZX81Console\*.def
DEL  .\emulators\ZX81Emulator\*.hlp
DEL  .\emulators\ZX81EmulatorC\*.hlp
DEL  .\monitors\RZX81Console\*.hlp
DEL  .\emulators\ZX81Emulator\ZX80SysVars.txt
DEL  .\emulators\ZX81Emulator\ZX81SysVars.txt
DEL  .\emulators\ZX81EmulatorC\ZX80SysVars.txt
DEL  .\emulators\ZX81EmulatorC\ZX81SysVars.txt
COPY .\projects\ZX81\*.fmt .\emulators\ZX81Emulator\ /Y
COPY .\projects\ZX81\ZX80SysVars.txt .\emulators\ZX81Emulator\ /Y
COPY .\projects\ZX81\ZX81SysVars.txt .\emulators\ZX81Emulator\ /Y
COPY .\projects\ZX81\ZX80SysVars.txt .\emulators\ZX81EmulatorC\ /Y
COPY .\projects\ZX81\ZX81SysVars.txt .\emulators\ZX81EmulatorC\ /Y
DEL .\emulators\ZX81Emulator\ZX81RConsole.fmt
COPY .\projects\ZX81\*.fmt .\emulators\ZX81EmulatorC\ /Y
DEL .\emulators\ZX81EmulatorC\ZX81RConsole.fmt
COPY .\projects\ZX81\*.fmt .\monitors\RZX81Console\ /Y
DEL .\monitors\RZX81Console\ZX81.fmt
RENAME .\monitors\RZX81Console\ZX81RConsole.fmt ZX81.fmt
COPY .\projects\ZX81\*.def .\emulators\ZX81EmulatorC\ /Y
COPY .\projects\ZX81\*.hlp .\emulators\ZX81EmulatorC\ /Y
DEL .\emulators\ZX81EmulatorC\ZX81RConsole.hlp
COPY .\projects\ZX81\*.hlp .\monitors\RZX81Console\ /Y
DEL .\monitors\RZX81Console\ZX81.hlp
RENAME .\monitors\RZX81Console\ZX81RConsole.hlp ZX81.hlp
COPY .\projects\SINCLAIR\*.fmt .\emulators\ZX81EmulatorC\ /Y
COPY .\projects\SINCLAIR\*.fmt .\emulators\ZX81Emulator\ /Y
COPY .\projects\SINCLAIR\*.fmt .\monitors\RZX81Console\ /Y
COPY .\projects\SINCLAIR\*.hlp .\emulators\ZX81EmulatorC\ /Y
COPY .\projects\SINCLAIR\*.hlp .\monitors\RZX81Console\ /Y
COPY .\projects\CONSOLE\*.fmt .\emulators\ZX81EmulatorC\ /Y
COPY .\projects\CONSOLE\*.hlp .\emulators\ZX81EmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\ZX81EmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\ZX81Emulator\ /Y
COPY .\projects\CORE\*.fmt .\monitors\RZX81Console\ /Y
COPY .\projects\CORE\*.hlp .\emulators\ZX81EmulatorC\ /Y
COPY .\projects\CORE\*.hlp .\emulators\ZX81Emulator\ /Y
COPY .\projects\CORE\*.hlp .\monitors\RZX81Console\ /Y
Echo ********************************************************************
Echo ********************************************************************
Echo 1.6: ZXSpectrum
Echo ********************************************************************
Echo Exe files...
COPY .\exe\x86\ZXSpectrumEmulator.exe .\emulators\ZXSpectrumEmulator\ /Y
COPY .\exe\x86\ZXSpectrumEmulatorD.exe .\emulators\ZXSpectrumEmulator\ /Y
COPY .\exe\x86\ZXSpectrumEmulatorC.exe .\emulators\ZXSpectrumEmulatorC\ /Y
COPY .\exe\x86\ZXSpectrumEmulatorCD.exe .\emulators\ZXSpectrumEmulatorC\ /Y
COPY .\exe\x86\RZXSpectrumConsole.exe .\monitors\RZXSpectrumConsole\ /Y
COPY .\exe\x86\RZXSpectrumConsoleD.exe .\monitors\RZXSpectrumConsole\ /Y
COPY .\exe\x86\ZXSpectrumCRTGenerator.exe .\projects\ZXSpectrumCRTGenerator\ /Y
COPY .\exe\x86\ZXSpectrumCRTGeneratorD.exe .\projects\ZXSpectrumCRTGenerator\ /Y
Echo Configuration files...
DEL  .\emulators\ZXSpectrumEmulator\*.fmt
DEL  .\emulators\ZXSpectrumEmulatorC\*.fmt
DEL  .\monitors\RZXSpectrumConsole\*.fmt
DEL  .\monitors\RZXSpectrumConsole\*.def
DEL  .\emulators\ZXSpectrumEmulator\*.hlp
DEL  .\emulators\ZXSpectrumEmulatorC\*.hlp
DEL  .\emulators\ZXSpectrumEmulator\ZXSpectrumSysVars.txt
DEL  .\emulators\ZXSpectrumEmulatorC\ZXSpectrumSysVars.txt
COPY .\projects\ZXSpectrum\*.fmt .\emulators\ZXSpectrumEmulator\ /Y
COPY .\projects\ZXSpectrum\ZXSpectrumSysVars.txt .\emulators\ZXSpectrumEmulator\ /Y
COPY .\projects\ZXSpectrum\ZXSpectrumSysVars.txt .\emulators\ZXSpectrumEmulatorC\ /Y
DEL .\emulators\ZXSpectrumEmulator\ZXSpectrumRConsole.fmt
COPY .\projects\ZXSpectrum\*.fmt .\emulators\ZXSpectrumEmulatorC\ /Y
DEL .\emulators\ZXSpectrumEmulatorC\ZXSpectrumRConsole.fmt
COPY .\projects\ZXSpectrum\*.fmt .\monitors\RZXSpectrumConsole\ /Y
DEL .\monitors\RZXSpectrumConsole\ZXSpectrum.fmt
RENAME .\monitors\RZXSpectrumConsole\ZXSpectrumRConsole.fmt ZXSpectrum.fmt
COPY .\projects\ZXSpectrum\*.def .\emulators\ZXSpectrumEmulatorC\ /Y
COPY .\projects\ZXSpectrum\*.hlp .\emulators\ZXSpectrumEmulatorC\ /Y
DEL .\emulators\ZXSpectrumEmulatorC\ZXSpectrumRConsole.hlp
COPY .\projects\ZXSpectrum\*.hlp .\monitors\RZXSpectrumConsole\ /Y
DEL .\monitors\RZXSpectrumConsole\ZXSpectrum.hlp
RENAME .\monitors\RZXSpectrumConsole\ZXSpectrumRConsole.hlp ZXSpectrum.hlp
COPY .\projects\SINCLAIR\*.fmt .\emulators\ZXSpectrumEmulatorC\ /Y
COPY .\projects\SINCLAIR\*.fmt .\emulators\ZXSpectrumEmulator\ /Y
COPY .\projects\SINCLAIR\*.fmt .\monitors\RZXSpectrumConsole\ /Y
COPY .\projects\SINCLAIR\*.hlp .\emulators\ZXSpectrumEmulatorC\ /Y
COPY .\projects\SINCLAIR\*.hlp .\monitors\RZXSpectrumConsole\ /Y
COPY .\projects\CONSOLE\*.fmt .\emulators\ZXSpectrumEmulatorC\ /Y
COPY .\projects\CONSOLE\*.hlp .\emulators\ZXSpectrumEmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\ZXSpectrumEmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\ZXSpectrumEmulator\ /Y
COPY .\projects\CORE\*.fmt .\monitors\RZXSpectrumConsole\ /Y
COPY .\projects\CORE\*.hlp .\emulators\ZXSpectrumEmulatorC\ /Y
COPY .\projects\CORE\*.hlp .\emulators\ZXSpectrumEmulator\ /Y
COPY .\projects\CORE\*.hlp .\monitors\RZXSpectrumConsole\ /Y
Echo ********************************************************************
Echo ********************************************************************
Echo 1.7: MSX
Echo ********************************************************************
Echo Exe files...
COPY .\exe\x86\MSXEmulator.exe .\emulators\MSXEmulator\ /Y
COPY .\exe\x86\MSXEmulatorD.exe .\emulators\MSXEmulator\ /Y
COPY .\exe\x86\MSXEmulatorC.exe .\emulators\MSXEmulatorC\ /Y
COPY .\exe\x86\MSXEmulatorCD.exe .\emulators\MSXEmulatorC\ /Y
COPY .\exe\x86\RMSXConsole.exe .\monitors\RMSXConsole\ /Y
COPY .\exe\x86\RMSXConsoleD.exe .\monitors\RMSXConsole\ /Y
COPY .\exe\x86\MSXCRTGenerator.exe .\projects\MSXCRTGenerator\ /Y
COPY .\exe\x86\MSXCRTGeneratorD.exe .\projects\MSXCRTGenerator\ /Y
Echo Configuration files...
DEL  .\emulators\MSXEmulator\*.fmt
DEL  .\emulators\MSXEmulatorC\*.fmt
DEL  .\monitors\RMSXConsole\*.fmt
DEL  .\monitors\RMSXConsole\*.def
DEL  .\emulators\MSXEmulator\*.hlp
DEL  .\emulators\MSXEmulatorC\*.hlp
DEL  .\emulators\MSXEmulator\MSXSysVars.txt
DEL  .\emulators\MSXEmulatorC\MSXSysVars.txt
COPY .\projects\MSX\*.fmt .\emulators\MSXEmulator\ /Y
COPY .\projects\MSX\MSXSysVars.txt .\emulators\MSXEmulator\ /Y
COPY .\projects\MSX\MSXSysVars.txt .\emulators\MSXEmulatorC\ /Y
DEL .\emulators\MSXEmulator\MSXRConsole.fmt
COPY .\projects\MSX\*.fmt .\emulators\MSXEmulatorC\ /Y
DEL .\emulators\MSXEmulatorC\MSXRConsole.fmt
COPY .\projects\MSX\*.fmt .\monitors\RMSXConsole\ /Y
DEL .\monitors\RMSXConsole\MSX.fmt
RENAME .\monitors\RMSXConsole\MSXRConsole.fmt MSX.fmt
COPY .\projects\MSX\*.def .\emulators\MSXEmulatorC\ /Y
COPY .\projects\MSX\*.hlp .\emulators\MSXEmulatorC\ /Y
DEL .\emulators\MSXEmulatorC\MSXRConsole.hlp
COPY .\projects\MSX\*.hlp .\monitors\RMSXConsole\ /Y
DEL .\monitors\RMSXConsole\MSX.hlp
RENAME .\monitors\RMSXConsole\MSXRConsole.hlp MSX.hlp
COPY .\projects\TEXASINSTRUMENTS\*.fmt .\emulators\MSXEmulatorC\ /Y
COPY .\projects\TEXASINSTRUMENTS\*.fmt .\emulators\MSXEmulator\ /Y
COPY .\projects\TEXASINSTRUMENTS\*.fmt .\monitors\RMSXConsole\ /Y
COPY .\projects\TEXASINSTRUMENTS\*.hlp .\emulators\MSXEmulatorC\ /Y
COPY .\projects\TEXASINSTRUMENTS\*.hlp .\monitors\RMSXConsole\ /Y
COPY .\projects\CONSOLE\*.fmt .\emulators\MSXEmulatorC\ /Y
COPY .\projects\CONSOLE\*.hlp .\emulators\MSXEmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\MSXEmulatorC\ /Y
COPY .\projects\CORE\*.fmt .\emulators\MSXEmulator\ /Y
COPY .\projects\CORE\*.fmt .\monitors\RMSXConsole\ /Y
COPY .\projects\CORE\*.hlp .\emulators\MSXEmulatorC\ /Y
COPY .\projects\CORE\*.hlp .\emulators\MSXEmulator\ /Y
COPY .\projects\CORE\*.hlp .\monitors\RMSXConsole\ /Y
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
Echo 3.1: C64
Echo ********************************************************************
COPY .\exe\x86\C64Emulator.exe .\setups\C64\ /Y
COPY .\exe\x86\C64EmulatorC.exe .\setups\C64\ /Y
COPY .\exe\x86\R64Console.exe .\setups\C64\ /Y
COPY .\exe\x86\6500Compiler.exe .\setups\C64\ /Y
COPY .\exe\x86\Test6500.exe .\setups\C64\ /Y
COPY .\emulators\C64Commons\bios\*.* .\setups\C64\bios\ /Y
COPY .\projects\C64\*.fmt .\setups\C64\ /Y
COPY .\projects\C64\*.hlp .\setups\C64\ /Y
COPY .\projects\C64\*.def .\setups\C64\ /Y
COPY .\projects\COMMODORE\*.fmt .\setups\C64\ /Y
COPY .\projects\COMMODORE\*.hlp .\setups\C64\ /Y
COPY .\projects\CONSOLE\*.fmt .\setups\C64\ /Y
COPY .\projects\CONSOLE\*.hlp .\setups\C64\ /Y
COPY .\projects\CORE\*.fmt .\setups\C64\ /Y
COPY .\projects\CORE\*.hlp .\setups\C64\ /Y
COPY .\projects\ASSEMBLER\*.fmt .\setups\C64\ /Y
"C:\Program files (x86)\Inno Setup 6\iscc" ".\setups\C64\C64.iss"
Echo ********************************************************************
Echo ********************************************************************
Echo 3.2: VIC20
Echo ********************************************************************
COPY .\exe\x86\VIC20EmulatorC.exe .\setups\VIC20\ /Y
COPY .\exe\x86\VIC20Emulator.exe .\setups\VIC20\ /Y
COPY .\exe\x86\RVIC20Console.exe .\setups\VIC20\ /Y
COPY .\exe\x86\6500Compiler.exe .\setups\VIC20\ /Y
COPY .\exe\x86\Test6500.exe .\setups\VIC20\ /Y
COPY .\exe\x86\VIC20CRTGenerator.exe .\setups\VIC20\ /Y
COPY .\emulators\VIC20Commons\bios\*.* .\setups\VIC20\bios\ /Y
COPY .\projects\VIC20\*.fmt .\setups\VIC20\ /Y
COPY .\projects\VIC20\*.hlp .\setups\VIC20\ /Y
COPY .\projects\VIC20\*.def .\setups\VIC20\ /Y
COPY .\projects\COMMODORE\*.fmt .\setups\VIC20\ /Y
COPY .\projects\COMMODORE\*.hlp .\setups\VIC20\ /Y
COPY .\projects\CONSOLE\*.fmt .\setups\VIC20\ /Y
COPY .\projects\CONSOLE\*.hlp .\setups\VIC20\ /Y
COPY .\projects\CORE\*.fmt .\setups\VIC20\ /Y
COPY .\projects\CORE\*.hlp .\setups\VIC20\ /Y
COPY .\projects\ASSEMBLER\*.fmt .\setups\VIC20\ /Y
"C:\Program files (x86)\Inno Setup 6\iscc" ".\setups\VIC20\VIC20.iss"
Echo ********************************************************************
Echo ********************************************************************
Echo 3.3: 264 Series
Echo ********************************************************************
COPY .\exe\x86\C264EmulatorC.exe .\setups\C264\ /Y
COPY .\exe\x86\C264Emulator.exe .\setups\C264\ /Y
COPY .\exe\x86\R264Console.exe .\setups\C264\ /Y
COPY .\exe\x86\6500Compiler.exe .\setups\C264\ /Y
COPY .\exe\x86\Test6500.exe .\setups\C264\ /Y
COPY .\emulators\C264Commons\bios\*.* .\setups\C264\bios\ /Y
COPY .\projects\C264\*.fmt .\setups\C264\ /Y
COPY .\projects\C264\*.hlp .\setups\C264\ /Y
COPY .\projects\C264\*.def .\setups\C264\ /Y
COPY .\projects\COMMODORE\*.fmt .\setups\C264\ /Y
COPY .\projects\COMMODORE\*.hlp .\setups\C264\ /Y
COPY .\projects\CONSOLE\*.fmt .\setups\C264\ /Y
COPY .\projects\CONSOLE\*.hlp .\setups\C264\ /Y
COPY .\projects\CORE\*.fmt .\setups\C264\ /Y
COPY .\projects\CORE\*.hlp .\setups\C264\ /Y
COPY .\projects\ASSEMBLER\*.fmt .\setups\C264\ /Y
"C:\Program files (x86)\Inno Setup 6\iscc" ".\setups\C264\C264.iss"
Echo ********************************************************************
Echo ********************************************************************
Echo 3.4: ZX81
Echo ********************************************************************
COPY .\exe\x86\ZX81EmulatorC.exe .\setups\ZX81\ /Y
COPY .\exe\x86\ZX81Emulator.exe .\setups\ZX81\ /Y
COPY .\exe\x86\RZX81Console.exe .\setups\ZX81\ /Y
COPY .\exe\x86\Z80Compiler.exe .\setups\ZX81\ /Y
COPY .\exe\x86\TestZ80.exe .\setups\ZX81\ /Y
COPY .\exe\x86\ZX81CRTGenerator.exe .\setups\ZX81\ /Y
COPY .\emulators\ZX81Commons\bios\*.* .\setups\ZX81\bios\ /Y
COPY .\projects\ZX81\*.fmt .\setups\ZX81\ /Y
COPY .\projects\ZX81\*.hlp .\setups\ZX81\ /Y
COPY .\projects\ZX81\*.def .\setups\ZX81\ /Y
COPY .\projects\SINCLAIR\*.fmt .\setups\ZX81\ /Y
COPY .\projects\SINCLAIR\*.hlp .\setups\ZX81\ /Y
COPY .\projects\CONSOLE\*.fmt .\setups\ZX81\ /Y
COPY .\projects\CONSOLE\*.hlp .\setups\ZX81\ /Y
COPY .\projects\CORE\*.fmt .\setups\ZX81\ /Y
COPY .\projects\CORE\*.hlp .\setups\ZX81\ /Y
COPY .\projects\ASSEMBLER\*.fmt .\setups\ZX81\ /Y
COPY .\projects\ZX81\ZX80SysVars.txt .\setups\ZX81\ /Y
COPY .\projects\ZX81\ZX81SysVars.txt .\setups\ZX81\ /Y
"C:\Program files (x86)\Inno Setup 6\iscc" ".\setups\ZX81\ZX81.iss"
Echo ********************************************************************
Echo ********************************************************************
Echo 3.5: ZXSpectrum
Echo ********************************************************************
COPY .\exe\x86\ZXSpectrumEmulatorC.exe .\setups\ZXSpectrum\ /Y
COPY .\exe\x86\ZXSpectrumEmulator.exe .\setups\ZXSpectrum\ /Y
COPY .\exe\x86\RZXSpectrumConsole.exe .\setups\ZXSpectrum\ /Y
COPY .\exe\x86\Z80Compiler.exe .\setups\ZXSpectrum\ /Y
COPY .\exe\x86\TestZ80.exe .\setups\ZXSpectrum\ /Y
COPY .\exe\x86\ZXSpectrumCRTGenerator.exe .\setups\ZXSpectrum\ /Y
COPY .\emulators\ZXSpectrumCommons\bios\*.* .\setups\ZXSpectrum\bios\ /Y
COPY .\projects\ZXSpectrum\*.fmt .\setups\ZXSpectrum\ /Y
COPY .\projects\ZXSpectrum\*.hlp .\setups\ZXSpectrum\ /Y
COPY .\projects\ZXSpectrum\*.def .\setups\ZXSpectrum\ /Y
COPY .\projects\SINCLAIR\*.fmt .\setups\ZXSpectrum\ /Y
COPY .\projects\SINCLAIR\*.hlp .\setups\ZXSpectrum\ /Y
COPY .\projects\CONSOLE\*.fmt .\setups\ZXSpectrum\ /Y
COPY .\projects\CONSOLE\*.hlp .\setups\ZXSpectrum\ /Y
COPY .\projects\CORE\*.fmt .\setups\ZXSpectrum\ /Y
COPY .\projects\CORE\*.hlp .\setups\ZXSpectrum\ /Y
COPY .\projects\ASSEMBLER\*.fmt .\setups\ZXSpectrum\ /Y
COPY .\projects\ZXSpectrum\ZXSpectrumSysVars.txt .\setups\ZXSpectrum\ /Y
"C:\Program files (x86)\Inno Setup 6\iscc" ".\setups\ZXSpectrum\ZXSpectrum.iss"
Echo ********************************************************************
Echo ********************************************************************
Echo 3.6: MSX
Echo ********************************************************************
COPY .\exe\x86\MSXEmulatorC.exe .\setups\MSX\ /Y
COPY .\exe\x86\MSXEmulator.exe .\setups\MSX\ /Y
COPY .\exe\x86\RMSXConsole.exe .\setups\MSX\ /Y
COPY .\exe\x86\Z80Compiler.exe .\setups\MSX\ /Y
COPY .\exe\x86\TestZ80.exe .\setups\MSX\ /Y
COPY .\exe\x86\MSXCRTGenerator.exe .\setups\MSX\ /Y
COPY .\emulators\MSXCommons\*.* .\setups\MSX\ /Y
COPY .\projects\MSX\*.fmt .\setups\MSX\ /Y
COPY .\projects\MSX\*.hlp .\setups\MSX\ /Y
COPY .\projects\MSX\*.def .\setups\MSX\ /Y
COPY .\projects\TEXASINSTRUMENTS\*.fmt .\setups\MSX\ /Y
COPY .\projects\TEXASINSTRUMENTS\*.hlp .\setups\MSX\ /Y
COPY .\projects\CONSOLE\*.fmt .\setups\MSX\ /Y
COPY .\projects\CONSOLE\*.hlp .\setups\MSX\ /Y
COPY .\projects\CORE\*.fmt .\setups\MSX\ /Y
COPY .\projects\CORE\*.hlp .\setups\MSX\ /Y
COPY .\projects\ASSEMBLER\*.fmt .\setups\MSX\ /Y
COPY .\projects\MSX\MSXSysVars.txt .\setups\MSX\ /Y
"C:\Program files (x86)\Inno Setup 6\iscc" ".\setups\MSX\MSX.iss"
Echo ********************************************************************
Echo
Echo
Echo Finished!

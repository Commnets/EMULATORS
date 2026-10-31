# Test projects in Emulators.sln

All 13 C++ projects under tests/ are visible in the main solution. Each declares Debug/Release for Win32/x64, with solution x86 mapped to project Win32. Outputs use exe/$(PlatformShortName) and intermediates use obj/$(ProjectName)/$(PlatformShortName)/$(Configuration). Debug executable names end in D.

Framework library dependencies are ProjectReferences. New references use LinkLibraryDependencies=false because the matching libraries are already listed explicitly in each linker configuration. This establishes build order without changing the library list.

Local build and runtime verification uses Debug|Win32 only, matching the installed third-party libraries. The other configurations are structurally checked, not certified by local builds.

## Legacy TestEmulOut

TestEmulOut appears in the Legacy solution folder but is excluded from the main solution's default build in all configurations. Its source still includes COMMS/Message.hpp and COMMS/StdMessages.hpp, which no longer exist. Its project metadata has been updated (current toolset/SDK, CORE library name, output paths and references); porting its obsolete communication API is separate work.

RakNetComms and RakNetCommsAd have distinct project GUIDs and four configurations in both the main solution and their standalone solutions. They are interactive network tools and are not launched by the automated regression checks.

Verified integration build: Emulators.sln, Debug|x86, targets Libs\Micros\Tests\TestVIATimer, TestZX81Video, TestVICIIStop, TestZXSpectrumMemoryAccess, RakNetComms and RakNetCommsAd all built successfully (exit 0), including their framework dependencies.
The remaining active projects (TestCPU, Test6500, TestZ80, TestInst6500, TestInstZ80 and TestSDL) also built successfully from Emulators.sln in Debug|x86. All 12 active test projects therefore passed the integration build; the 13th, TestEmulOut, is intentionally excluded as documented above. Structural validation passed for all 52 project configurations and both RakNet standalone solution mappings.

# Foreword

This guide is for a developer who has downloaded EMULATORS and wants to configure Windows, build the framework and start an emulator manually. It follows the repository projects and the dependency layout used by the maintained development environment. No AI agent or automated installation service is required.

Start with SDL2 and Debug x86. SDL2 is the working baseline. The shared build configuration can select SDL3 headers and libraries, but the application code still requires a separate SDL3 API migration. Selecting SDL3 or passing dependency validation is not evidence that a complete SDL3 emulator can be built or run.

Commands assume PowerShell in the repository root unless a different directory is stated. Replace example installation roots with your own paths. Build dependencies must match the executable architecture. The 64-bit Visual Studio application can build the required 32-bit binaries.

# Quick start

1. Install Visual Studio 2022 with Desktop development with C++, the MSVC v143 x86/x64 tools and a Windows 10 or 11 SDK.
2. Extract the complete framework, including Directory.Build.props, Directory.Build.targets, build, include, src, projects, thirdparties and the machine resource folders.
3. Prepare the SDL2 dependencies listed in External dependencies. Supply Debug import libraries and DLLs with the expected trailing D names.
4. Prepare RakNet and set RAKNET_HOME. Resolve the additional test-only JSON include path if you build the entire solution.
5. Define the five SDL2 environment variables. Restart Visual Studio and your developer shell after changing environment variables.
6. Keep EmulatorsSDLVersion set to 2. Open Emulators.sln and select Debug and x86 at solution level.
7. Run the validation target for C64EmulatorC. Resolve every reported missing dependency before building.
8. Build the solution. Prepare the C64 execution folder with the new executable, matching DLLs, bios directory and formatter resources.
9. Start C64EmulatorCD.exe from that execution folder. Enter RUN in its console and confirm that the machine reaches its BASIC startup screen.

The following chapters give the exact variables, filenames, commands and checks needed for these steps. A prebuilt executable included in a download is not a substitute for the executable produced by your own build.

# Tools and repository

## Install the Windows tools

Use Visual Studio Installer to install Visual Studio Community 2022 or another Visual Studio 2022 edition. Select Desktop development with C++. In Individual components, verify MSVC v143 C++ x64/x86 build tools and an installed Windows SDK. The projects select toolset v143 and SDK version 10.0, allowing Visual Studio to resolve an installed Windows 10 or 11 SDK. See source S1 in the appendices.

Open Developer PowerShell for VS 2022 for command-line builds. Run the following commands to check that the shell can find MSBuild and the compiler tools:

```powershell
Get-Command msbuild
Get-Command cl
Get-Command dumpbin
```

Visual Studio 2022 runs as a 64-bit program. Choose x86 in Emulators.sln for the initial build; this maps to Win32 in the individual C++ projects. Use Debug Win32 when building one vcxproj directly. Release and x64 configurations exist, but the local runtime verification baseline is Debug x86. Do not assume that an installed x64 DLL can be loaded by an x86 executable.

## Extract the complete repository

Use a writable directory, for example C:\Workspaces\EMULATORS. Keep the relative folder structure: project files use paths such as ../../include, ../../thirdparties and ../../lib. Do not copy a vcxproj out of its directory and expect these paths to remain valid.

| Directory or file | Purpose |
| --- | --- |
| Emulators.sln | Main Visual Studio solution. |
| include and src | Public headers and implementation sources. |
| projects | Framework projects and canonical formatter, help and definition resources. |
| emulators | Emulator projects, working folders and machine resources. |
| thirdparties | Bundled code including the reSID project. |
| exe, lib and obj | Build outputs, framework libraries and intermediate files. |
| tests, monitors and compilers | Diagnostics and framework tools. |
| setups and all.bat | Existing distribution assembly inputs and scripts. |

Some packages or resource folders may be absent in a source archive. Verify the required files instead of relying on the contents of an older installation. The first-run chapter identifies the files for a C64 example.

# External dependencies

## Reference versions and downloads

The following versions identify the installations used during environment preparation. They are a compatibility reference, not a claim that arbitrary newer versions are interchangeable. The root folder names themselves can differ.

| Component | SDL2 reference | SDL3 reference |
| --- | --- | --- |
| Core | SDL 2.0.20 | SDL 3.4.0 |
| Images | SDL_image 2.0.5 | SDL_image 3.2.6 |
| Mixer | SDL_mixer 2.0.4 | SDL_mixer source snapshot named main |
| Primitives | SDL2_gfx 1.0.4 | SDL3_gfx 1.0.1 |
| Fonts | SDL_ttf 2.0.18 | SDL_ttf 3.1.0 |

Get SDL and the image, mixer and ttf extensions from their upstream projects and release archives, listed as S2 through S6. Get SDL2_gfx from the original author's project, S7. The SDL3_gfx installation is a separate SDL3-compatible port; use a source package that supplies the SDL3_gfxPrimitives.h header and the other files listed below. An identically named package should be checked against these requirements. Record its source revision locally. The installed SDL3_mixer main snapshot is not pinned to a release or commit, so its exact revision cannot be recovered from the folder name alone.

Header search paths are needed for all five components. Current project profiles link core and image where required. Mixer, gfx and ttf remain available as headers for future development; no current project enables mixer linking, and no gfx or ttf import libraries are added automatically. Framework audio uses SDL core directly.

## Prepare SDL2 binaries

Prebuilt release-only development archives may not supply the Debug binaries expected by this repository. Build the matching sources, or obtain an equivalent prepared dependency set. The installed legacy SDL2 Visual Studio projects were configured to produce Debug names ending in D. A fresh upstream package may require this setup explicitly.

1. Extract each dependency to its own directory. Build SDL core before image. Build mixer and ttf only if their functions will actually be linked or deployed.
2. Open the source solution in Visual Studio. Reference solutions are VisualC/SDL.sln, VisualC/SDL_image.sln, VisualC/SDL_mixer.sln and VisualC/SDL_ttf.sln. The gfx solution is SDL2_gfx.sln in its root.
3. If Visual Studio requests conversion, retain the source backup and retarget the library projects to your installed Windows SDK and v143 toolset. Select Debug Win32.
4. In each required DLL project's Debug properties, set Target Name to the expected D filename. For example, SDL2 becomes SDL2D and SDL2_image becomes SDL2_imageD. Let the import library follow the target name, or set its name explicitly under Linker, Advanced, Import Library.
5. For SDL core auxiliary projects, provide SDL2mainD.lib when Debug main support is needed. Release main support uses SDL2main.lib. Match the exact files in the library table in the SDL configuration chapter.
6. In extension projects, configure their include directories and link search paths to the selected core installation. Their Debug link input must use SDL2D.lib, so the generated DLL imports SDL2D.dll. Preserve inherited entries and the extension's codec dependencies.
7. Build the required projects and verify the output files. Configure output paths or copy the matching DLL and import library together to the directory layout below.

```text
<SDL root>\include\SDL.h
<SDL root>\VisualC\Win32\Debug\SDL2D.lib
<SDL root>\VisualC\Win32\Debug\SDL2D.dll
<image root>\SDL_image.h
<image root>\VisualC\Win32\Debug\SDL2_imageD.lib
<image root>\VisualC\Win32\Debug\SDL2_imageD.dll
<mixer root>\SDL_mixer.h
<mixer root>\VisualC\Win32\Debug\SDL2_mixerD.lib
<mixer root>\VisualC\Win32\Debug\SDL2_mixerD.dll
```

Renaming an existing import library alone does not change the DLL it references. Renaming an extension DLL alone does not change its imported SDL core name. Build or prepare the matching set, then inspect the result:

```powershell
dumpbin /dependents "$env:SDLIMAGE_HOME\VisualC\Win32\Debug\SDL2_imageD.dll"
```

Repeat for mixer only if enabling its link option in future development. The mixer binary paths above describe the optional binary layout; current projects need its header, not its import library or DLL. Keep the codec DLLs required by the image formats actually used. They may be loaded dynamically. Gfx needs its four public headers in its root, and legacy ttf needs SDL_ttf.h in its root. Building their DLLs is only necessary when their functions are actually linked or deployed.

## Prepare SDL3 dependencies when needed

SDL3 uses include/SDL3/SDL.h. Image, mixer and ttf use include/SDL3_image, include/SDL3_mixer and include/SDL3_ttf. The gfx port has SDL3_gfxPrimitives.h, SDL3_framerate.h, SDL3_imageFilter.h and SDL3_rotozoom.h in its root.

The configured SDL3 import libraries are SDL3.lib, SDL3_image.lib and SDL3_mixer.lib in both Debug and Release. Keep the names produced by these installations. Place the binaries under VisualC/Win32/Debug or VisualC/Win32/Release as appropriate. If your build system emits another layout, stage the matching headers and binaries into the expected layout or deliberately update the shared properties. Do not alter the framework executable naming convention to match the dependency names.

Prepare SDL3 dependencies for validation and later migration work. Continue using SDL2 for a first complete framework build.

## RakNet and bundled third party code

The reference RakNet installation is 4.081. RAKNET_HOME points to its root, with Source containing headers and Lib containing the import libraries and runtime DLLs. The upstream archive is S8. An arbitrary current checkout is not guaranteed to produce the historical filenames expected by this framework.

For an equivalent source build, open RakNet_VS2008.sln, convert the required DLL project to the installed toolset, select Win32 and build that project rather than every sample. Supply these outputs under RAKNET_HOME/Lib:

```text
RakNet_VS2008_DLL_Debug_Win32.lib
RakNet_VS2008_DLL_Debug_Win32.dll
RakNet_VS2008_DLL_Release_Win32.lib
RakNet_VS2008_DLL_Release_Win32.dll
```

Preserve the API expected by Source and keep the runtime DLL paired with its import library. Runtime architecture and ABI compatibility must be checked independently of filenames. The SDL validation target does not validate RakNet.

The repository includes thirdparties/resid and its RESID.vcxproj. Build it through Emulators.sln; do not download another SID implementation for the initial build. Its Debug output is RESIDD.lib. Other bundled machine-specific code remains under thirdparties and should be retained when extracting the framework.

## Additional requirements for a full solution build

TestInst6500 includes nlohmann/json.hpp. Its vcxproj currently contains the absolute include path C:\Workspaces\CPPTOOLS\json-develop\include. Obtain the nlohmann JSON headers from S9 and either provide that layout or change this include path in all affected configurations to your installation. The emulator itself does not require that test-specific absolute directory. Do not remove all tests from the solution merely to conceal a missing prerequisite.

CORE contains an optional import of packages/Microsoft.Windows.CppWinRT.2.0.220531.1/build/native/Microsoft.Windows.CppWinRT.props, guarded by Exists. Preserve an existing package folder. If additional project requirements in your checkout request it, restore the indicated NuGet package through Visual Studio. The optional import alone is not evidence that every source build needs that package restored.

Inno Setup is needed only to build installers. The current distribution script invokes Inno Setup 6 from C:\Program Files (x86)\Inno Setup 6\iscc. Check or adapt that path to your installation before packaging. Source S10 provides the upstream installer and documentation.

# Windows environment

## Define the variables manually

Open Windows Settings and search for Edit the system environment variables. Open Environment Variables. Add the following names as user variables or system variables, consistently with how you launch Visual Studio. Enter the root directory as the value, without surrounding quotation marks. Save the dialogs and restart Visual Studio and the developer shell.

| Component | SDL2 variable | SDL3 variable |
| --- | --- | --- |
| Core | SDL_HOME | SDL3_HOME |
| Images | SDLIMAGE_HOME | SDL3IMAGE_HOME |
| Mixer | SDLMIXER_HOME | SDL3MIXER_HOME |
| Primitives | SDLGFX_HOME | SDL3GFX_HOME |
| Fonts | SDLTTF_HOME | SDL3TTF_HOME |

Also define RAKNET_HOME. Example roots can be C:\Workspaces\CPPLIBS\SDL2\SDL2-2.0.20-master and C:\Workspaces\CPPLIBS\RakNet-4.081. Each SDL extension has its own root. Your paths may differ, but their internal layout must match the shared properties.

The variable names are exact. SDLIMAGE_HOME is not SDL_IMAGEHOME, and SDL3_HOME includes an underscore. Confirm the values in the shell that will launch the build:

```powershell
Get-ChildItem Env:SDL*
Get-Item Env:RAKNET_HOME
Test-Path "$env:SDL_HOME\include\SDL.h"
Test-Path "$env:SDL_HOME\VisualC\Win32\Debug\SDL2D.lib"
Test-Path "$env:RAKNET_HOME\Source\RakPeerInterface.h"
Test-Path "$env:RAKNET_HOME\Lib\RakNet_VS2008_DLL_Debug_Win32.lib"
```

Each Test-Path should return True. Defining a variable does not install a library or create its required subdirectories. A shell opened before a system-variable change can still contain the old value.

## Keep runtime lookup explicit

Compiler include paths and linker library paths do not control Windows DLL lookup. For a reproducible manual launch, place the matching runtime DLLs beside the executable. An existing PATH entry can otherwise supply an unexpected DLL from an older installation.

Keep codec DLLs out of Windows system directories. Debug binaries require the Visual Studio Debug runtime; the standard redistributable targets Release.

# SDL configuration reference

## The selector and its propagation

Directory.Build.props defines EmulatorsSDLVersion as 2 unless a value was already supplied. Set it to 2 or 3, or pass /p:EmulatorsSDLVersion=3 to an individual MSBuild command. Use the same value for every project and every command involved in one build.

Each integrated vcxproj imports build/SDL.props. That sheet chooses dependency roots, fills EmulatorsSDLIncludeDirectories and resolves the import-library list. It also supplies MCHEMUL_SDL_VERSION through ClCompile PreprocessorDefinitions. The compiler therefore receives /DMCHEMUL_SDL_VERSION=2 or 3 for every source file inheriting that metadata.

The C++ selector headers CORE/SDL.hpp, CORE/SDLImage.hpp, CORE/SDLGfx.hpp and CORE/SDLTtf.hpp choose the external includes. CORE/incs.hpp includes all four. A project consuming that aggregate header needs gfx and ttf include paths even when it does not call their functions.

| File | Responsibility |
| --- | --- |
| Directory.Build.props | Repository default SDL selector. |
| build/SDL.props | Includes, version macro, dependency roots and link inputs. |
| Directory.Build.targets | Imports the shared validation target. |
| build/SDLValidation.targets | Validates required files before build preparation. |

## Project options and inherited settings

Declare project requirements before importing SDL.props. EmulatorsSDLLink enables core linking. EmulatorsSDLLinkImage and EmulatorsSDLLinkMixer add their components only when core linking is enabled. EmulatorsSDLLinkMain adds SDL2main only for SDL2. All four link options default to false. EmulatorsSDLLinkMixer is retained for future use but is not enabled by any current project.

EmulatorsSDLHeadersGfx and EmulatorsSDLHeadersTtf default to true. Keep them enabled for consumers of CORE/incs.hpp. They provide headers and validation, not linking. A header-only framework library can import the sheet without enabling SDL link options. Importing the sheet alone does not enable linking; VIC20CRTGenerator and RMSXConsole currently use that profile.

Preserve %(AdditionalIncludeDirectories), %(PreprocessorDefinitions), %(AdditionalLibraryDirectories) and %(AdditionalDependencies) in project-local lists. These inherited entries bring in the shared configuration. Do not reintroduce direct SDL root paths and SDL import libraries into every vcxproj.

## Library and product names

| Component | SDL2 Debug | SDL3 Debug |
| --- | --- | --- |
| Core | SDL2D.lib | SDL3.lib |
| Images | SDL2_imageD.lib | SDL3_image.lib |
| Mixer if enabled | SDL2_mixerD.lib | SDL3_mixer.lib |
| Main if enabled | SDL2mainD.lib | No main import library |

SDL2 Release omits D. SDL3 Release keeps the same names as SDL3 Debug but selects the Release directory. The common search layout is VisualC/$(Platform)/$(Configuration) under each linked component root.

EMULATORS products follow their own convention independently: CORED.lib and C64EmulatorCD.exe are Debug products; CORE.lib and C64EmulatorC.exe are Release products. The SDL sheet does not rewrite TargetName or existing output directories.

# Build and validation

## Explicit library project dependencies

The main solution declares every internal library prerequisite explicitly, including transitive prerequisites. For example, a library depending on COMMODORE also declares CORE and F6500 rather than relying only on the COMMODORE edge. These entries establish build order; they are separate from an executable's linker input list.

The following table records the static library dependency graph. Solution build dependencies and executable linker inputs are separate settings; the table does not mean that every consumer links every object in each listed library.

| Library | Explicit internal prerequisites |
| --- | --- |
| CORE | None |
| RESID | None |
| ASSEMBLER | CORE |
| COMMS | CORE |
| F6500 | CORE |
| FZ80 | CORE |
| TEXASINSTRUMENTS | CORE |
| GENERALINSTRUMENTS | CORE |
| SINCLAIR | CORE |
| EMULS | CORE, ASSEMBLER, COMMS |
| CONSOLE | CORE, ASSEMBLER, COMMS, EMULS |
| COMMODORE | CORE, F6500, RESID, ASSEMBLER, COMMS, EMULS |
| C64 | CORE, F6500, RESID, ASSEMBLER, COMMS, EMULS, COMMODORE |
| VIC20 | CORE, F6500, RESID, ASSEMBLER, COMMS, EMULS, COMMODORE |
| C264 | CORE, F6500, RESID, ASSEMBLER, COMMS, EMULS, COMMODORE |
| ZX81 | CORE, FZ80, SINCLAIR, ASSEMBLER, COMMS, EMULS |
| ZXSpectrum | CORE, FZ80, SINCLAIR, ASSEMBLER, COMMS, EMULS |
| MSX | CORE, FZ80, TEXASINSTRUMENTS, GENERALINSTRUMENTS, ASSEMBLER, COMMS, EMULS |

SINCLAIR currently uses CORE only. Console implementation dependencies belong to consumers that actually use the CONSOLE library; the machine libraries in this table do not depend on it. VIC20 and C264 list RESID because their shared COMMODORE prerequisite contains the SID integration. These solution dependencies apply to all four solution configurations. Their structural consistency has been checked; a complete build remains the separate compilation check.

## Executable linker inputs

Current projects no longer enable SDL_mixer linking. The six emulator variants without the final C in their names omit CONSOLE; their local console counterparts retain it. ZX81Emulator, ZX81EmulatorC, RZX81Console, ZXSpectrumEmulator, ZXSpectrumEmulatorC and RZXSpectrumConsole omit RESID. Each rule applies to Debug and Release on Win32 and x64, preserving the framework's Debug D suffix.

VIC20CRTGenerator links CORE as its only internal library and does not enable SDL linking. Its VIC20 namespace belongs to local generator classes. RMSXConsole currently contains an empty main and declares no additional libraries beyond inherited toolchain inputs. Both projects retain the SDL property-sheet import and its header validation; neither import enables SDL linking by itself.

Other existing SDL core, image, SDL2main and RakNet link choices are preserved pending a separate review of the object files brought into each executable. An unused-looking API at the entry point is insufficient evidence to remove a library: shared framework code can introduce references. The static library build-order table above is maintained separately from these linker inputs.

## Validate before compiling

From Developer PowerShell at the repository root, run:

```powershell
msbuild emulators\C64EmulatorC\C64EmulatorC.vcxproj `
  /t:ValidateEmulatorsSDL /p:Configuration=Debug /p:Platform=Win32
```

The target checks the selector, dependency roots and concrete header files. It checks import libraries only when enabled by the project profile. Gfx and ttf are checked as headers, without requiring their DLLs or import libraries. Add /p:EmulatorsSDLVersion=3 to inspect SDL3 dependencies without compiling the emulator.

Directory.Build.targets imports this target automatically. It runs before PrepareForBuild and is skipped for Visual Studio design-time builds and projects not importing SDL.props. Success proves file presence, not binary architecture, ABI compatibility, complete source compilation or runtime resolution. It does not validate RakNet, ROMs or DLLs.

## Compile the working SDL2 baseline

Open Emulators.sln. In Configuration Manager select Debug and x86. Ensure the required library projects, RESID and the emulator are enabled for build. Use Build Solution. For a clean command-line build:

```powershell
msbuild Emulators.sln /t:Rebuild `
  /p:Configuration=Debug /p:Platform=x86 /p:EmulatorsSDLVersion=2
```

Use the solution to build dependencies in their declared order. An individual executable vcxproj may also depend on libraries supplied through solution dependencies; building it alone from a clean checkout is not a substitute for the complete solution build.

Expected Debug outputs include lib/x86/CORED.lib, lib/x86/RESIDD.lib and exe/x86/C64EmulatorCD.exe. Framework objects are under obj/<project>/x86/Debug. Check the actual build output and timestamp; do not launch an older executable left in a project directory.

## Switching SDL versions

Output directories remain shared between SDL2 and SDL3 by design. When changing the selector, clean and rebuild the whole solution. Do not mix object files or framework libraries from different SDL versions.

1. Change the selector or provide the same /p:EmulatorsSDLVersion value to every command.
2. Validate dependencies and clean the complete solution for the chosen configuration.
3. Rebuild all required projects for that version.
4. Prepare matching runtime DLLs manually in every execution folder.

```powershell
msbuild Emulators.sln /t:Clean `
  /p:Configuration=Debug /p:Platform=x86 /p:EmulatorsSDLVersion=2
msbuild Emulators.sln /t:Rebuild `
  /p:Configuration=Debug /p:Platform=x86 /p:EmulatorsSDLVersion=2
```

Retain SDL2 for normal use until the SDL3 API migration is complete. A successful SDL3 dependency check does not remove that requirement.

# First run with C64

## Prepare one self contained execution folder

The executable directory and the working directory are distinct concepts. C64EmulatorC opens ./C64.fmt, and C64 memory loads ROMs from ./bios. Relative paths are resolved from the process working directory. Prepare one folder and start the program from that folder.

The example below creates run/C64 inside your checkout and copies the freshly built Debug executable. It copies the canonical formatter and help resources used by the C64, Commodore, console, CORE and assembler layers. These commands are manual instructions; the build does not execute them automatically.

```powershell
$repo = (Get-Location).Path
$run = Join-Path $repo 'run\C64'
New-Item -ItemType Directory -Path $run -Force | Out-Null
Copy-Item "$repo\exe\x86\C64EmulatorCD.exe" $run -Force
foreach ($module in 'C64','COMMODORE','CONSOLE','CORE','ASSEMBLER') {
  foreach ($extension in '*.fmt','*.hlp','*.ps') {
    Get-ChildItem "$repo\projects\$module" -Filter $extension -File |
      Copy-Item -Destination $run -Force
  }
}
Copy-Item "$repo\emulators\C64Commons\bios" $run -Recurse -Force
```

C64.fmt includes Commodore.fmt and LocalConsole.fmt; Commodore.fmt includes Standard.fmt. Keeping the dependency chain together avoids missing formatter definitions after startup. The default English ROM files are:

```text
bios/basic.901226-01.bin
bios/characters.901225-01-ENG.bin
bios/kernal.901227-03-ENG.bin
```

If the downloaded archive does not contain those images, supply the exact matching ROM resources before launching. Other languages and machine models require different images. Do not rename an unrelated ROM to satisfy a filename check.

## Place the Debug runtime DLLs

For SDL2 Debug Win32, copy SDL2D.dll and SDL2_imageD.dll from their corresponding VisualC/Win32/Debug directories. Copy the auxiliary codec DLLs required by image as well. Copy RakNet_VS2008_DLL_Debug_Win32.dll from RAKNET_HOME/Lib. Current projects do not link SDL_mixer and do not require its DLL for framework audio.

```powershell
foreach ($root in $env:SDL_HOME,$env:SDLIMAGE_HOME) {
  Get-ChildItem "$root\VisualC\Win32\Debug" -Filter '*.dll' -File |
    Copy-Item -Destination $run -Force
}
Copy-Item "$env:RAKNET_HOME\Lib\RakNet_VS2008_DLL_Debug_Win32.dll" `
  $run -Force
```

Inspect duplicate filenames if your dependency packages contain multiple copies of SDL core. Use the core DLL from SDL_HOME as the authority and restore it after extension copies if necessary. The gfx and ttf functions are not currently linked, so including their headers alone does not require their DLLs.

## Launch and verify

```powershell
Push-Location $run
.\C64EmulatorCD.exe
Pop-Location
```

In the local console, HELP lists available operations, RUN starts or resumes CPU execution, and QUIT exits the console. Confirm the expected C64 BASIC startup screen, keyboard input and normal shutdown. Sound can be checked with a known program that produces audio; an idle BASIC screen is not an audio test.

For Visual Studio debugging, set C64EmulatorC as the startup project. Under Debugging, set Working Directory to the folder you prepared. The debugger normally launches the executable from its configured build output directory; its DLLs must also be reachable there. Alternatively set Command to the prepared executable when intentionally debugging that copy and refresh it after rebuilding.

## Other machines

Apply the same distinction between executable, DLL location and working directory to other machines. Use their own canonical formatter files and machine resources, rather than copying C64 files into every folder.

| Family | Resource root in emulators | Maintained user guide |
| --- | --- | --- |
| C64 | C64Commons/bios | docs/C64Emulator_UserGuide.docx |
| VIC20 | VIC20Commons | docs/VIC20Data/VIC20Emulator_UserGuide.docx |
| C264 | C264Commons | docs/C264Data/C264Emulator_UserGuide.docx |
| ZX80 and ZX81 | ZX81Commons | docs/ZX81Data/ZX81Emulator_UserGuide.docx |
| Spectrum | ZXSpectrumCommons | docs/ZXSpectrumData/ZXSpectrumEmulator_UserGuide.docx |
| MSX | MSXCommons | docs/MSXData/MSXEmulator_UserGuide.docx |

Consult the selected machine guide for exact model, language and ROM requirements. The console executable variants end in C before the Debug suffix D; the other emulator variants can use remote communication. Start with a local console variant to minimize first-run configuration.

MSXCommons is referenced by the distribution script but is absent from the inspected checkout. Prepare that resource directory from the selected MSX model's requirements before using the packaging script. A missing folder in an archive must not be treated as a completed machine installation.

# Distribution and setup

Runtime deployment remains manual. Directory.Build.props sets VcpkgApplocalDeps to false, disabling vcpkg automatic DLL copying after linking and its applocal.ps1 invocation. Build does not deploy SDL DLLs, assemble resources or update installer inputs. Keep version, architecture and configuration consistent in each destination folder.

The existing all.bat script is an assembly and packaging script, not a prerequisite installer or an MSBuild entry point. It copies executables from exe/x86, refreshes resource files in project execution directories and setup staging areas, and calls Inno Setup. It also deletes and replaces resource files and references multiple machine tools. Review the whole script and its expected outputs before using it on a fresh checkout. Do not use it as the quick-start command.

all.bat reads EmulatorsSDLVersion directly from Directory.Build.props through MSBuild and assigns the evaluated value to its local SDLVERSION variable. Do not maintain a second manual selection in the script. It locates MSBuild 17.8 or later with vswhere.exe from the Visual Studio Installer and evaluates the property without compiling. A missing tool, failed query, or value other than exactly 2 or 3 stops the script with exit code 1 before deployment. An invalid value displays "ERROR: SDLVERSION must be 2 or 3." Temporary query files are removed after evaluation.

Change the central selector, then clean and rebuild the whole solution before packaging. Reading the current configuration does not identify how existing executables were compiled. A previous build with a command-line /p:EmulatorsSDLVersion override does not change the persistent selector; ensure that the packaging query resolves the version used for the binaries being distributed.

For a release installer, first prepare a complete Release binary and runtime set for the intended architecture. Inspect setups/<machine>/<machine>SDL2.iss or <machine>SDL3.iss and its matching setups/Commons/CommonsSDL2.iss or CommonsSDL3.iss. The common scripts select the corresponding SDL core, image, mixer, gfx and ttf DLLs and include a Win32 RakNet Release DLL and the x86 Visual C++ redistributable. Those inputs must exist and match the packaged executable.

Changing EmulatorsSDLVersion does not rewrite the Inno Setup scripts or replace files in setups/Commons. Preparing the SDL3 installer inputs does not establish that the framework migration is complete or that SDL3 executables run correctly. Debug executables are development artifacts; do not assume that the release redistributable supplies their Debug C++ runtime.

Where SDL3 Debug and Release share an execution folder, replace the runtime set when changing configuration: both use filenames such as SDL3.dll. The latest copied DLLs determine what is available, independently of the D suffix on an EMULATORS executable.

# Troubleshooting

## Dependency validation errors

| Error | Meaning and action |
| --- | --- |
| SDL001 | The selector is not 2 or 3. Correct Directory.Build.props or the command-line property. |
| SDL002 | A required root is empty. Define the named environment variable and restart the build process. |
| SDL003 | A selected root does not exist. Correct the path or restore the installation. |
| SDL004 | A required header or import library is missing. Check the filename, root layout, platform and configuration printed in the error. |

CORE and other headers-only profiles do not require SDL import libraries. Executables validate only their enabled link components. If a custom project does not receive validation, confirm its explicit import of build/SDL.props and the inherited root Directory.Build.targets.

## Compiler and linker errors

For missing MCHEMUL_SDL_VERSION, check the SDL.props import and %(PreprocessorDefinitions) inheritance. For missing SDL3 or extension headers, check the dependency root, package layout and inherited include directories.

For LNK1104 on SDL2D.lib, check its filename and selected directory. For CORED.lib or another framework library, ensure its prerequisite project was built through the solution. For LNK1112 or similar conflicts, check x86 versus x64 across all objects and libraries.

For missing nlohmann/json.hpp in TestInst6500, configure the test-only include path described earlier. Install missing Windows SDK or v143 tools, or deliberately retarget the affected projects. SDL validation does not check these prerequisites.

## Startup and resource errors

A missing DLL message is a runtime deployment issue. Check both direct imports and extension codec dependencies. Use dumpbin /dependents on the executable and dependency DLLs, and dumpbin /headers to inspect architecture. Windows status 0xC000007B commonly indicates an incompatible binary format such as an x86/x64 mismatch; investigate the loaded files rather than emulator timing.

For missing C64.fmt or BIOS files, check the working directory, formatter include chain and relative resource layout. Review C64E.log there. Confirm that you launched the freshly built executable rather than an older copy.

# Appendices

## Reference checklist

- Visual Studio 2022 v143 and a Windows SDK are installed.
- The selected SDL roots and RAKNET_HOME are visible in the build shell.
- Debug SDL2 libraries and DLLs carry the expected D suffix.
- Gfx and ttf headers are available to CORE/incs.hpp consumers.
- Test-only JSON headers are configured when building the full solution.
- ValidateEmulatorsSDL succeeds for the selected executable profile.
- The whole solution has been rebuilt after an SDL version change.
- The executable, DLLs, working directory, BIOS and formatter chain match.
- Installer staging is reviewed separately from the compiler configuration.

## Sources and verification scope

The guide was checked against Directory.Build.props, Directory.Build.targets, build/SDL.props, build/SDLValidation.targets, Emulators.sln, the C64EmulatorC entry point, C64 memory ROM paths, canonical formatter includes, all.bat and the Inno Setup scripts. During initial SDL preparation, environment validation succeeded for 51 integrated project files with both SDL versions in Debug Win32. The unused legacy IO project has since been removed, leaving 50 SDL-integrated projects. Those validation runs did not compile or run SDL3 and did not establish a fresh-machine installation result. The static library dependency graph was subsequently checked for explicit transitive closure, valid project identifiers and absence of cycles. The five linker cleanup groups described above were checked structurally across all four project configurations, without compilation or execution. Further executable and test dependency normalization remains pending.

S1 Microsoft C++ installation instructions
https://learn.microsoft.com/en-us/cpp/build/vscpp-step-0-installation

S2 SDL core source and release archives
https://github.com/libsdl-org/SDL/releases
https://github.com/libsdl-org/SDL/releases/tag/release-2.0.20

S3 SDL_image sources and release archives
https://github.com/libsdl-org/SDL_image/releases

S4 SDL_mixer sources and release archives
https://github.com/libsdl-org/SDL_mixer/releases

S5 SDL_ttf sources and release archives
https://github.com/libsdl-org/SDL_ttf/releases

S6 SDL3 migration reference
https://wiki.libsdl.org/SDL3/README-migration

S7 SDL2_gfx original project
https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html

S8 RakNet upstream archive
https://github.com/facebookarchive/RakNet

S9 nlohmann JSON source and releases
https://github.com/nlohmann/json

S10 Inno Setup project and documentation
https://jrsoftware.org/inno-setup

The source and binary preparation steps describe the observed repository contract. Converting older dependency projects or substituting releases may require source-level compatibility work. Record any such changes rather than assuming that the folder name alone identifies a reproducible dependency build.

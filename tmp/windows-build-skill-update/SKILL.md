---
name: emulators-windows-build-runtime
description: Build and run EMULATORS locally on Windows with the repository-compatible Visual Studio configuration and SDL runtime architecture. Use whenever Codex compiles, links, tests, launches, or smoke-tests an EMULATORS executable on Windows.
---

# Build and Run EMULATORS on Windows

For every local build, test, launch, smoke test or log-capture operation in this workspace, always use the 32-bit Debug configuration. This applies to every EMULATORS target, including graphical and console emulators, tests, compilers, monitors, generators, utilities, setup outputs and their supporting libraries. Do not infer an exception from the target type: the available third-party runtime libraries are 32-bit.

- For `Emulators.sln`, use MSBuild properties `/p:Configuration=Debug /p:Platform=x86`.
- For an individual `.vcxproj`, use `/p:Configuration=Debug /p:Platform=Win32` unless inspection of that project proves a different 32-bit platform label.
- Launch the executable produced by that 32-bit Debug build. Do not fall back to an older binary whose architecture has not been verified.
- Do not use `Debug|x64`, `Release|x64`, or a 64-bit executable for local build, test, smoke-test, or log-capture workflows.
- Treat Windows exit status `0xC000007B` as a likely executable/DLL architecture mismatch. Verify that the executable and loaded SDL DLLs are both 32-bit before diagnosing emulator behavior.

Report the exact configuration and platform used whenever build or execution results are part of the evidence.

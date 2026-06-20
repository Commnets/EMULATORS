---
name: msx-system-audit
description: Validate general MSX system emulation in EMULATORS, including BIOS-facing behavior, Z80/VDP interrupt wiring, standard I/O ports, PPI 8255 ports #A8-#AB, keyboard matrix, cassette controls, joystick wiring, model configuration, and coordination with VDP/PSG/memory components. Use when Codex reviews or edits MSX computer/model setup, port managers, PPI, keyboard, cassette, joystick, BIOS/system variables, reset/boot behavior, or cross-chip MSX wiring.
---

# MSX System Audit

## Core Workflow

Use this skill to audit whether EMULATORS behaves like a real MSX machine at the system-integration level.

1. Read `references/system.md` before making claims or edits.
2. Inspect the relevant implementation paths first:
   - `include/MSX/MSX.hpp`, `src/MSX/MSX.cpp`
   - `include/MSX/Model.hpp`, `src/MSX/Model.cpp`
   - `include/MSX/PortManager.hpp`, `src/MSX/PortManager.cpp`
   - `include/MSX/PPI8255.hpp`, `src/MSX/PPI8255.cpp`
   - `include/MSX/PPI8255Registers.hpp`, `src/MSX/PPI8255Registers.cpp`
   - `include/MSX/OSIO.hpp`, `src/MSX/OSIO.cpp`
   - `include/MSX/FileIO.hpp`, `src/MSX/FileIO.cpp`
   - `emulators/MSXEmulator*`, `emulators/MSXPrograms`, and `docs/MSXData`
3. When changing C++ code, also use `$emulators-framework-cpp-style`.
4. Use `$msx-memory-map-audit` for slot/subslot or mapper issues, `$msx-vdp-audit` for TMS9918A behavior, and `$msx-sound-audit` for PSG/AY/YM behavior.
5. Separate generic chip behavior from MSX-specific wiring. The 8255, AY-3-8910, and TMS9918A all have generic semantics, but MSX assigns their ports and some bits very specifically.

## Audit Priorities

Prioritize correctness in this order:

- Standard I/O decode and dispatch: VDP #98-#9B, PSG #A0-#A2, PPI #A8-#AB, printer #90-#91, memory mapper #FC-#FF when present.
- PPI #A8 primary-slot register read/write and propagation into the memory system.
- PPI #A9/#AA keyboard matrix read path, including active-low keys and row selection.
- PPI #AA cassette motor/output, CAPS LED, and key-click output bits; #AB bit set/reset command behavior.
- VDP interrupt wiring to Z80 INT and BIOS interrupt/hook behavior.
- PSG GPIO cooperation with joystick ports, cassette input, keyboard layout bit, and Kana LED.
- Model-specific configuration: BIOS ROMs, RAM size, language/keyboard layout, PAL/NTSC video standard, VDP/PSG/PPI instances, and optional devices.

## Validation Hints

Prefer small port-level or BIOS-level programs over whole-game comparisons. Useful local anchors include:

- `emulators/MSXPrograms/KEY/*.key`
- `emulators/MSXEmulator/MSXSysVars.txt`
- `emulators/MSXEmulatorC/MSXSysVars.txt`
- `emulators/MSXEmulator*/bios/*.rom`
- `docs/MSXData/Keyboard*.jpg`
- `tests/TestZ80`

When reporting a finding, identify the I/O port or BIOS-visible state, the expected MSX wiring, and the exact class or method that diverges.

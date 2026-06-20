---
name: msx-vdp-audit
description: Validate MSX video emulation for the TMS9918A/TMS9928A/TMS9929A VDP family in EMULATORS, including ports #98/#99, VRAM access and auto-increment, VDP register semantics, status flag read side effects, display modes, table base address masking, sprites, 4-sprites-per-line overflow, sprite collision, PAL/NTSC interrupt timing, and BIOS VDP routine compatibility. Use when Codex reviews or edits MSX VDP, TMS99xxFamily, Screen, VDP port manager, VRAM, graphics modes, sprites, raster/interrupt behavior, or VDP register code.
---

# MSX VDP Audit

## Core Workflow

Use this skill to audit whether EMULATORS models the MSX1 VDP family as a port-driven video chip with separate VRAM and TMS9918A-compatible side effects.

1. Read `references/vdp.md` before making claims or edits.
2. Inspect the relevant implementation paths first:
   - `include/MSX/VDP.hpp`, `src/MSX/VDP.cpp`
   - `include/MSX/Screen.hpp`, `src/MSX/Screen.cpp`
   - `include/MSX/PortManager.hpp`, `src/MSX/PortManager.cpp`
   - `include/MSX/Commands.hpp`, `src/MSX/Commands.cpp`
   - `include/TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamily.hpp`, `src/TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamily.cpp`
   - `include/TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamilyRegisters.hpp`, `src/TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamilyRegisters.cpp`
3. When changing C++ code, also use `$emulators-framework-cpp-style`.
4. Confirm the intended chip/model first. TMS9918A/TMS9928A/TMS9929A behavior is not the same as V9938/V9958 behavior.
5. Treat port protocol, register side effects, VRAM table addressing, and status reads as correctness requirements, not renderer details.

## Audit Priorities

Prioritize correctness in this order:

- Port #98/#99 protocol, including two-byte control writes, address latch, register writes, status reads, and VRAM auto-increment.
- Status register flags and clear-on-read behavior for VBlank interrupt, fifth-sprite overflow, and sprite collision.
- Register R0-R7 write masks, read/write exposure, and mode-bit combinations.
- Pattern name, color, pattern generator, sprite attribute, and sprite pattern table base calculations, including Graphics II masking quirks.
- Text, Graphics I, Graphics II, and Multicolor mode rendering rules.
- Sprite evaluation: Y coordinate offset, terminator, size/magnification, priority, 4 sprites per scanline, overflow index, collision flag.
- PAL/NTSC frame timing and VDP INT wiring to the Z80/BIOS.

## Validation Hints

Useful local anchors:

- `emulators/MSXPrograms/KEY/GRAPHICS1.key`
- `emulators/MSXPrograms/KEY/SPRITES1.key`
- `emulators/MSXEmulator*/MSX.fmt`
- `emulators/MSXEmulatorC/TexasInstruments.hlp`
- VDP monitor commands in `src/MSX/Commands.cpp`

For visual bugs, reduce to a small VRAM/register setup before comparing whole games. For port bugs, write Z80 tests that touch #98/#99 directly and also through BIOS routines such as WRTVDP, RDVRM, WRTVRM, SETRD, and SETWRT.

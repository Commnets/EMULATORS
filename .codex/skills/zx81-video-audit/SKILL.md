---
name: zx81-video-audit
description: Validate and implement Sinclair ZX81 video emulation in the EMULATORS repository for /mZX811, /mZX812, and /mZX813, including ULA character generation, Z80 display-file execution, M1/refresh/HALT/INT behavior, scanline timing, HSYNC/VSYNC, NMI generator and WAIT synchronization, SLOW and FAST modes, PAL/NTSC margins, collapsed display files, inverse video, high-resolution techniques, ROM-version interactions, and ULA revision boundaries. Use when Codex reviews or edits namespace ZX81 ULA, ULARegisters, MemoryVideoCode, Memory, PortManager, Screen, CPU interrupt timing, raster, ROM-specific video flow, or ZX81 video tests.
---

# ZX81 Video Audit

## Core Workflow

Treat ZX81 video as cycle-sensitive work shared by the Z80, ROM, RAM, and ULA. Validate control flow and signal edges as well as pixels.

1. Read `references/zx81-video.md` before judging behavior.
2. Inspect `.codex/docs/ZX81Video.pdf` when circuit diagrams, `/NMI`-`/WAIT` gating, counter logic, oscilloscope traces, back porch, or pixel synchronization matter. Render relevant pages because the PDF contains no extractable text.
3. Identify the selected model:
   - `/mZX811` -> `ZX81::Type::_ZX811`, old ROM
   - `/mZX812` -> `ZX81::Type::_ZX812`, rare/intermediate ROM
   - `/mZX813` -> `ZX81::Type::_ZX813`, newest ROM
4. Audit all three selectors when changing shared video behavior. Test ROM control flow separately when addresses, delays, margins, or system variables matter.
5. Do not equate a ROM selector with a ULA silicon revision. ROM version, ULA revision, PAL/NTSC selection, and RAM expansion are independent axes unless repository configuration proves otherwise.
6. Inspect the local implementation paths:
   - `include/ZX81/ULA.hpp`, `src/ZX81/ULA.cpp`
   - `include/ZX81/ULARegisters.hpp`, `src/ZX81/ULARegisters.cpp`
   - `include/ZX81/Memory.hpp`, `src/ZX81/Memory.cpp`
   - `include/ZX81/ZX81.hpp`, `src/ZX81/ZX81.cpp`
   - `include/ZX81/PortManager.hpp`, `src/ZX81/PortManager.cpp`
   - `include/ZX81/Screen.hpp`, `src/ZX81/Screen.cpp`
   - `include/ZX81/Type.hpp`, `src/ZX81/ZX81Emulator.cpp`
   - Z80 M1, refresh, HALT, WAIT, INT, NMI, and instruction-cycle code under `include/FZ80` and `src/FZ80`
7. When changing C++ code, also use `$emulators-framework-cpp-style`.

## Audit Priorities

Prioritize correctness in this order:

- 6.5 MHz ULA/pixel clock, 3.25 MHz CPU clock, horizontal counter, line reset source, and HSYNC edge placement.
- Display-file echo, `/M1` qualification, byte capture, forced NOP for bit-6-clear codes, and pass-through of bit-6-set opcodes.
- `HALT` end-of-line behavior, R-register/A6 maskable interrupt timing, interrupt acknowledge, line completion, and scanline-counter increment.
- Character address formation from `I=0x1E`, character bits 0-5, scanline counter 0-7, bit 7 inverse video, and eight-pixel shift output.
- NMI-generator enable/disable I/O decoding, HSYNC-to-NMI relationship, `/HALT`-dependent `/WAIT` synchronization, NMI priority, and handler entry at `0x0066`.
- SLOW-mode task sequence: VSYNC/keyboard, top blank lines plus user code, 192 visible lines, bottom blank lines plus user code.
- FAST mode: user code at full speed without a maintained display, while ROM display routines remain ZX80-compatible when invoked.
- VSYNC start/stop side effects, line-counter reset/blocking, keyboard reads, cassette coupling, and 50/60 Hz sense.
- PAL versus NTSC blank-line margins and effective CPU time. Do not infer timing solely from host refresh rate.
- Collapsed/expanded display files, pseudo-hi-res and true-hi-res compatibility, and cycle-exact software that depends on undocumented behavior.
- Early 2C184E versus later 2C210E/back-porch behavior when hardware revision is in scope.

## Validation Matrix

Exercise each relevant combination:

| Axis | Required cases |
| --- | --- |
| ROM selector | ZX811, ZX812, ZX813 |
| Video standard | PAL; NTSC when supported by the path under review |
| Execution mode | SLOW, FAST, ROM display while otherwise in FAST |
| Display file | Empty/collapsed, short rows, fully expanded, inverse characters |
| Interrupt phase | Visible INT/HALT path, upper NMI blanking, lower NMI blanking, VSYNC with NMI off |
| RAM behavior | Base RAM and 16K expansion when address aliasing or hi-res is relevant |

Use three kinds of evidence:

1. **Bus trace:** PC, address, data before/after ULA interception, `/M1`, `/MREQ`, `/RFSH`, `/HALT`, `/WAIT`, `/INT`, `/NMI`, acknowledge cycles, I, R, and line counter.
2. **Signal trace:** horizontal counter, HSYNC, VSYNC, NMI-enable latch, character latch, shift-register load/output, and video level.
3. **Rendered result:** standard 32x24 text, collapsed rows, checkerboard patterns, inverse video, SLOW/FAST transitions, and representative hi-res software.

Report exact timing only after naming the convention: CPU T-states versus ULA clocks, whether a 0-based terminal count is described as 207 or 208 cycles, and which signal edge starts the line.

## Change Guardrails

- Keep behavior common to ZX811/ZX812/ZX813 shared unless ROM evidence requires a branch.
- Preserve interrupt ordering when INT and NMI become eligible near the same CPU boundary.
- Do not render directly from `D_FILE` if that bypasses bus-visible or cycle-visible effects.
- Do not model standard-compliant porches as universal ZX81 behavior; distinguish ULA revisions and composite modifications.
- Do not use ROM routine addresses from one ROM image as invariants across all three selectors.
- When sources disagree, prefer measured behavior for the identified model/revision, document the counter/edge convention, and retain uncertainty rather than averaging values.

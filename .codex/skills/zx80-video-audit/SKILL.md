---
name: zx80-video-audit
description: Validate and implement Sinclair ZX80 video emulation in the EMULATORS repository, including CPU-driven display generation, display-file execution, Z80 M1/refresh/HALT/INT interactions, character and inverse-video fetches, line counter and shift register behavior, horizontal and vertical sync, collapsed display files, PAL timing, flicker, and cassette/video coupling. Use when Codex reviews or edits namespace ZX81 code for the /mZX80 model, especially ULA, ULARegisters, MemoryVideoCode, Memory, PortManager, Screen, CPU interrupt timing, raster, or video tests.
---

# ZX80 Video Audit

## Core Workflow

Treat ZX80 video as a cycle-sensitive collaboration between the Z80, ROM software, memory aliases, and discrete TTL logic, not as a conventional framebuffer.

1. Read `references/zx80-video.md` before judging behavior.
2. Inspect `.codex/docs/ZX80Video.pdf` when circuit diagrams, oscilloscope traces, pixel synchronization, or porch behavior matter. Render relevant pages because the PDF contains no extractable text.
3. Confirm the selected machine is `/mZX80` (`ZX81::Type::_ZX80`). Do not silently apply ZX81 NMI or ULA behavior.
4. Inspect the local implementation paths:
   - `include/ZX81/ULA.hpp`, `src/ZX81/ULA.cpp`
   - `include/ZX81/ULARegisters.hpp`, `src/ZX81/ULARegisters.cpp`
   - `include/ZX81/Memory.hpp`, `src/ZX81/Memory.cpp`
   - `include/ZX81/ZX81.hpp`, `src/ZX81/ZX81.cpp`
   - `include/ZX81/PortManager.hpp`, `src/ZX81/PortManager.cpp`
   - `include/ZX81/Screen.hpp`, `src/ZX81/Screen.cpp`
   - Z80 M1, refresh, HALT, WAIT, INT, and instruction-cycle code under `include/FZ80` and `src/FZ80`
5. When changing C++ code, also use `$emulators-framework-cpp-style`.
6. Separate observed hardware behavior, ROM-dependent scheduling, and host-window cropping. A plausible screenshot does not prove correct bus or interrupt timing.

## Audit Priorities

Prioritize correctness in this order:

- 6.5 MHz pixel clock, 3.25 MHz CPU clock, and two pixels per CPU T-state.
- Display-file echo and opcode-fetch qualification: address decoding, `/M1`, memory read, character-byte capture, and forced NOP behavior.
- Bit 6 pass-through, `HALT` end-of-line handling, repeated HALT M1/refresh cycles, R-register progression, A6-driven maskable interrupt, and interrupt acknowledge.
- Character address formation from `I=0x0E`, character bits 0-5, scanline counter 0-7, ROM glyph fetch, and bit 7 inverse video.
- Eight-pixel shift-register output without gaps, overlaps, stale bytes, or off-by-one pixels.
- Collapsed and expanded display-file rows, left alignment, 32-character maximum, and blank remainder after `HALT`.
- Horizontal sync placement and line completion; vertical sync and keyboard/cassette coupling.
- ZX80 flicker semantics: ordinary program execution and key processing pre-empt video generation.
- PAL frame geometry. Treat the repository's current PAL-only and unexpanded restrictions as implementation scope, not universal hardware facts.

## Validation Strategy

Build evidence at three levels:

1. **Bus trace:** record CPU cycles with PC, address bus, data before/after interception, `/M1`, `/MREQ`, `/RFSH`, `/HALT`, `/INT`, interrupt acknowledge, I, R, and line counter.
2. **Signal trace:** record pixel clock, character load, shift output, HSYNC, VSYNC, and composite level transitions.
3. **Rendered result:** compare full, collapsed, short, inverse, and checkerboard rows; also test the transition from display generation to user code.

Use small deterministic fixtures before whole programs:

- One row containing a single normal character followed by `HALT`.
- The same glyph inverted with bit 7.
- Empty, partially expanded, and fully expanded rows.
- Codes with bit 6 clear versus set, including `0x76`.
- Alternating/checkerboard glyphs to expose pixel-width jitter.
- A key-processing or computation interval that must blank the ZX80 display.

Report timing with an explicit convention: CPU T-states versus 6.5 MHz pixel/logic clocks, inclusive counter values versus elapsed cycles, and the edge used as line origin.

## Change Guardrails

- Preserve the shared `ZX81` namespace while keeping `_ZX80` branches explicit where hardware differs.
- Do not add an NMI generator to ZX80 behavior.
- Do not replace CPU/bus-driven behavior with direct display-file rendering when cycle-visible effects are in scope.
- Do not assume standard broadcast porches that the original circuit did not generate.
- If a source and a trace disagree, preserve the trace, identify model/circuit revision and measurement points, and state the uncertainty.

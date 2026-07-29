---
name: zx-spectrum-ula-audit
description: Validate and implement Sinclair ZX Spectrum 16K/48K ULA and board-level behavior in the EMULATORS repository, including PAL/NTSC raster timing, bitmap and attribute fetches, FLASH/BRIGHT/INK/PAPER rendering, CPU clock generation, lower-RAM and I/O contention, floating bus, snow effect, DRAM addressing and refresh, port FE keyboard/tape/border/sound behavior, interrupts, and ULA revision boundaries. Use when Codex reviews or edits namespace ZXSPECTRUM ULA, ULARegisters, Memory, PortManager, Screen, Sound, Datasette, Z80 bus timing, raster, contention, or ZX Spectrum hardware tests.
---

# ZX Spectrum ULA Audit

## Core Workflow

Treat the ULA as the timing owner of the original 16K/48K machine. Validate signal and bus behavior before judging the final framebuffer.

1. Read `references/zx-spectrum-ula.md` completely before judging behavior.
2. Inspect `.codex/docs/ZXSpectrumULA.pdf` for circuit diagrams, die logic, clock phases, RAS/CAS waveforms, fetch/latch relationships, analogue output, and ULA-version details. Use the printed-page to PDF-page locator in the reference and render the relevant pages.
3. Identify the configuration axes explicitly:
   - 16K or 48K RAM.
   - PAL or NTSC.
   - ULA/board issue when revision-sensitive behavior matters.
   - T-state and raster-origin convention.
4. Inspect the local implementation:
   - `include/ZXSpectrum/ULA.hpp`, `src/ZXSpectrum/ULA.cpp`
   - `include/ZXSpectrum/ULARegisters.hpp`, `src/ZXSpectrum/ULARegisters.cpp`
   - `include/ZXSpectrum/Memory.hpp`, `src/ZXSpectrum/Memory.cpp`
   - `include/ZXSpectrum/ZXSpectrum.hpp`, `src/ZXSpectrum/ZXSpectrum.cpp`
   - `include/ZXSpectrum/PortManager.hpp`, `src/ZXSpectrum/PortManager.cpp`
   - `include/ZXSpectrum/Screen.hpp`, `src/ZXSpectrum/Screen.cpp`
   - `include/ZXSpectrum/Sound.hpp`, `src/ZXSpectrum/Sound.cpp`
   - `include/ZXSpectrum/Datasette*.hpp`, `src/ZXSpectrum/Datasette*.cpp`
   - Z80 M-cycle, memory-access, I/O, refresh, WAIT/clock-stop, HALT, and interrupt code under `include/FZ80` and `src/FZ80`
5. Trace actual Z80 bus accesses within each instruction. Do not infer contention from only the final address, instruction length, or total elapsed cycles.
6. When changing C++ code, also use `$emulators-framework-cpp-style`.

## Audit Priorities

Prioritize correctness in this order:

- Master, pixel, and CPU clocks; line length; frame length; raster origin; INT assertion width and phase.
- ULA fetch cadence and exact bitmap/attribute addresses for all 192 display lines.
- Shift-register and attribute-latch phasing, including prefetch, left/right edges, INK/PAPER, BRIGHT, and FLASH.
- Contention for every contended M-cycle and I/O sub-cycle, with accumulated delays shifting later accesses.
- Floating-bus value at the Z80 sampling edge, including idle `0xFF`, bitmap, attribute, and model timing.
- Port FE partial decoding, keyboard row combination, EAR input, MIC/EAR output mixing, border writes, and beeper edges.
- DRAM row/column multiplexing, ULA priority, refresh, 16K open-bus behavior, and ROMCS/expansion interactions.
- Snow effect and other revision-sensitive or electrically emergent behavior only after the normal path is correct.

## Required Evidence

Use three synchronized views:

1. **CPU/bus trace:** instruction, PC, M-cycle, T-state, address, data, `/MREQ`, `/IORQ`, `/RD`, `/WR`, `/M1`, `/RFSH`, `/INT`, and inserted delay.
2. **ULA trace:** frame T-state, raster line/pixel, video-fetch phase, bitmap address/data, attribute address/data, latch/shift load, border, and floating-bus value.
3. **Rendered result:** full screen and border, with deliberately adversarial byte and attribute patterns.

Always state whether frame T-state 0 means the first T-state with `/INT` low, the CPU's interrupt acknowledge, or another edge. Never compare bare timing numbers across different origins.

## Validation Matrix

Exercise at least:

| Axis | Cases |
| --- | --- |
| Memory | 16K; 48K |
| Video | PAL; NTSC when supported |
| Address access | opcode fetch, operand read, write, read-modify-write, stack, refresh, I/O |
| Display phase | top border, first fetch, active row, right border, last row, vertical blank |
| Port | even/uncontended high byte, even/contended high byte, odd/uncontended, odd/contended |
| Attributes | all INK/PAPER, BRIGHT, FLASH phases, border changes |
| Bus quirks | floating bus, I in `0x40-0x7F`, unmapped 16K upper memory |

Prefer deterministic timing fixtures and known hardware test programs. Cross-check against the local Fuse and JSpeccy sources only as comparative implementations, never as the hardware authority.

## Change Guardrails

- Keep 16K/48K behavior separate from 128K, +2, +2A, and +3 timing unless the requested scope explicitly expands.
- Do not implement contention by delaying an instruction only after it has completed if that loses the changed timing of later M-cycles, I/O phases, interrupts, or raster writes.
- Do not treat every unused port read as the last attribute byte; derive the bus value at the exact sample time.
- Do not model port FE as only literal `0x00FE`; A0-low aliases and the full high byte matter.
- Do not conflate EAR input with the MIC/EAR output network or the resulting speaker level.
- Do not make analogue colour artifacts mandatory for a digital framebuffer unless the selected fidelity boundary includes composite encoding.
- Preserve measured disagreements as named model/revision/convention differences.

## Audit Report

Report:

1. configuration and timing convention;
2. findings ordered by compatibility impact;
3. hardware or ROM evidence for each finding;
4. exact affected paths and tests;
5. whether the proposed change is cycle-visible, pixel-visible, audio-visible, or only an implementation cleanup;
6. known fidelity boundaries that remain.

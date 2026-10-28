---
name: vic20-vici-audit
description: Audit VIC20 MOS 6560/6561 VIC-I video and sound emulation in EMULATORS, including raster timing, register access, display counters, character and color fetches, VIC-visible memory, multicolor, light pen, paddles, oscillators, and CPU/VIA synchronization. Use for VICI code reviews or VIC20 visual, audio, and raster trace diagnosis.
---

# VIC20 VIC-I Audit

Audit the VIC-I as a timed memory consumer and video/audio generator in its VIC20 board context. An audit does not authorize code edits; respect the user's requested scope.

## Read and route

- Read [references/vici.md](references/vici.md) for register/memory invariants, display timing, audio, and discriminating validation cases.
- Use [references/sources.md](references/sources.md) to locate the supplied VIC-I datasheet, Mapping the VIC, relevant original manuals, and independent emulator evidence. Check PDF figures when OCR obscures a bit field or timing edge.
- For VIA-driven raster effects, also use `$vic20-via-audit`. For `.LOG` analysis, use `$emulators-debug-log-audit`, first reading the current VIC-I debug methods; a VIC-II field interpretation does not automatically apply to VIC-I.
- If C++ changes are authorized, also use `$emulators-framework-cpp-style`. For a Windows build or runtime comparison, use `$emulators-windows-build-runtime`.

## Repository entry points

- `include/COMMODORE/VICI/VICI.hpp`, `src/COMMODORE/VICI/VICI.cpp`: chip models, raster, display state, fetch/draw flow, transaction projection, debug records, and `SoundFunction`.
- `include/COMMODORE/VICI/VICIRegisters.hpp`, `src/COMMODORE/VICI/VICIRegisters.cpp`: mixed read/write fields, raster reads, pointer translation, colors, light pen and paddles.
- `include/COMMODORE/VICI/VICISoundWrapper.hpp`, `src/COMMODORE/VICI/VICISoundWrapper.cpp`: sound dividers, voice/noise state, sample generation and register readback.
- VIC20 `Memory`, `VIC20`, `Screen`, `Sound`, `OSIO`, `VIC20Emulator`, and `VIA1*`/`VIA2*` under `include/VIC20` and `src/VIC20`: memory visibility, selected model, host scaling, shared inputs and timer synchronization.
- `include/CORE/Raster.hpp`, `src/CORE/Raster.cpp`, CPU/Instruction context and bus-write timing: inspect current definitions before assigning a physical phase to a logged position.

## Audit method

1. Establish exact chip/video variant, interlace setting, oscillator/CPU rate, memory expansion and ROM selection. Separate the emulated raster from host window dimensions and frame pacing.
2. Decode actual `$9000-$900F` values, color RAM contents and CPU-visible versus VIC-visible addresses. Default BASIC layouts are examples, not hardware limits.
3. Follow display-window latches, character row/column counters, matrix/color fetches, glyph fetches, shifts and pixel emission in cycle order. A correct static frame does not establish correct mid-line writes.
4. Place CPU register/RAM accesses on their bus cycles. In full-instruction mode, distinguish a predicted end position for raster reads from replaying writes at the right time. State the uncertainty when logs expose only instruction-level activity.
5. When a scene drifts, compare identical PCs/register writes across frames and correlate VIA timeout/output/CPU IRQ entry separately. When an object is absent, verify its code/data path, clipping, addresses, colors and raster placement before inferring a CPU instruction defect.
6. For each finding give source edition/page or hardware evidence, local method/line, register bits, expected behavior, observed behavior, cycle/line context and confidence. Identify the earliest observed divergence; do not invent a first divergence before a trace starts.

## Verification scope

Select the narrow cases in the reference that distinguish the competing explanations. Compare PAL and NTSC using their own expected periods. Separate visual plausibility, exact pixel agreement, audio pitch, and cycle-level compatibility. A comparison emulator is supporting evidence, and a known limitation or TODO in it is not a hardware requirement.

Existing `emulators/VIC20Programs/Code/Tests/` assets focus on VIA behavior and may help time raster effects. Use a supplied game/trace as a regression only for observations actually present; do not assume that a welcome screen must show a particular actor or match another release.

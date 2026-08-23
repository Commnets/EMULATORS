---
name: c64-vicii-audit
description: Validate Commodore 64 VIC-II emulation in EMULATORS, including $D000-$D3FF register behavior, PAL/NTSC timing, raster counters, badlines, BA/AEC bus stealing, video matrix and bitmap fetches, sprites, collisions, borders, IRQs, color RAM usage, and display modes. Use when Codex reviews or edits VICII, Screen, Memory/PLA, raster, sprite, video, or cycle-level behavior for C64 classes.
---

# C64 VIC-II Audit

When compiling, testing, or executing on Windows, use `$emulators-windows-build-runtime`.

## Core Workflow

Use this skill to audit whether EMULATORS simulates the VIC-II as a timing-sensitive bus participant, not only as a frame renderer.

1. Read `references/vicii.md` before judging VIC-II behavior.
2. Inspect the local implementation paths first:
   - `include/COMMODORE/VICII/VICII.hpp`, `src/COMMODORE/VICII/VICII.cpp`
   - `include/COMMODORE/VICII/VICIIRegisters.hpp`, `src/COMMODORE/VICII/VICIIRegisters.cpp`
   - `include/C64/Screen.hpp`, `src/C64/Screen.cpp`
   - `include/C64/Memory.hpp`, `src/C64/Memory.cpp`
   - `include/C64/ColorMemory.hpp`, `src/C64/ColorMemory.cpp`
   - CIA2 bank-selection code under `include/C64/CIA2*` and `src/C64/CIA2*`
3. When changing C++ code, also use `$emulators-framework-cpp-style`.
4. Decide whether the code path is PAL, NTSC, or model-agnostic before asserting exact line/cycle counts.
5. Treat cycle placement, register side effects, and bus ownership as user-visible behavior. Demos and tests depend on these details.

## Audit Priorities

Prioritize correctness in this order:

- Raster counter and $D011/$D012 high-bit interaction.
- Badline condition, display enable latch, VC/VCBASE/RC updates, and c-access timing.
- BA/AEC bus stealing and CPU-visible wait behavior.
- $D018 video matrix/character/bitmap pointer calculation inside the selected VIC bank.
- Text, multicolor text, bitmap, multicolor bitmap, ECM, invalid modes, and idle state.
- Sprite DMA, pointer fetches, expansion, multicolor mode, priority, and collision registers.
- IRQ flags/masks, collision flag clearing, lightpen, border flip-flops, and open/unconnected read bits.

## Validation Hints

Useful local programs and visual tests:

- `emulators/C64Programs/Code/Tests/TESTGRAPHMODES.asm`
- `emulators/C64Programs/Code/Tests/TESTHRESMODES.asm`
- `emulators/C64Programs/Code/Tests/TESTSPRITESIRQ.asm`
- `emulators/C64Programs/Code/Tests/SPRITESPRIORITY.asm`
- `emulators/C64Programs/Code/Tests/SP4.asm`
- `emulators/C64Programs/Code/Tests/demos/RASTERBARS0.asm`
- `emulators/C64Programs/Code/Tests/demos/RASTERBARS1.asm`
- `emulators/C64Programs/Code/Tests/demos/RASTERBARS2.asm`
- `emulators/C64Programs/Code/Tests/demos/BORDER1.asm`
- `emulators/C64Programs/Code/Tests/demos/Sprite.asm`

When a visual symptom is involved, prefer a narrow raster/sprite test over a whole-game comparison.

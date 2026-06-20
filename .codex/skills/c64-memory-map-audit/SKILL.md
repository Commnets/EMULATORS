---
name: c64-memory-map-audit
description: Validate Commodore 64 memory map, PLA/bank switching, 6510 port $0000/$0001 behavior, VIC bank selection, color RAM, ROM/RAM overlays, cartridge mapping, and mirrored I/O in the EMULATORS C64 namespace. Use when Codex reviews or edits C64 memory, PLA, CPU-visible address decoding, VIC-visible memory, color memory, char ROM, cartridge, expansion I/O, or mapped chip register behavior.
---

# C64 Memory Map Audit

## Core Workflow

Use this skill to audit whether EMULATORS presents the same memory view as real C64 hardware.

1. Read `references/memory-map.md` before making claims or edits.
2. Inspect the relevant implementation paths first:
   - `include/C64/Memory.hpp`, `src/C64/Memory.cpp`
   - `include/C64/PLA.hpp`, `src/C64/PLA.cpp`
   - `include/C64/IO6510PortRegisters.hpp`, `src/C64/IO6510PortRegisters.cpp`
   - `include/C64/ColorMemory.hpp`, `src/C64/ColorMemory.cpp`
   - `include/C64/Cartridge.hpp`, `src/C64/Cartridge.cpp`
   - `include/C64/IOExpansionMemory.hpp`, `src/C64/IOExpansionMemory.cpp`
   - chip register classes under `include/C64`, `src/C64`, `include/COMMODORE`, and `src/COMMODORE`
3. When changing C++ code, also use `$emulators-framework-cpp-style`.
4. Separate CPU-visible mapping from VIC-visible mapping. The CPU sees PLA-selected RAM/ROM/I/O overlays; the VIC sees a selected 16 KB bank plus its own color/char-ROM behavior.
5. For findings, cite the address range, controlling signal/bit, expected visibility, and the class or method that violates it.

## Audit Priorities

Prioritize correctness in this order:

- $0000/$0001 6510 port DDR/data behavior and whether LORAM/HIRAM/CHAREN only act when configured as outputs.
- PLA decoding for RAM, BASIC ROM, KERNAL ROM, character ROM, I/O, color RAM, cartridge ROML/ROMH, GAME, EXROM, and Ultimax-like open regions.
- Writes through visible ROM to underlying RAM.
- I/O range decoding and mirroring: VIC-II, SID, color RAM, CIA1, CIA2, IO1, IO2.
- VIC memory bank selection through CIA2 PA0/PA1, including inverted bank encoding and char ROM visibility in VIC banks 0 and 2.
- Color RAM as 1 KB x 4-bit storage with open/high-nibble read behavior.

## Validation Hints

Prefer small address-decoder tests over broad emulator smoke tests. Useful existing assets include:

- `emulators/C64Programs/Code/Macros/MVICII.asm`
- `emulators/C64Programs/Code/Macros/MCIAS.asm`
- `emulators/C64Programs/Code/Tests/TESTGRAPHMODES.asm`
- `emulators/C64Programs/Code/Tests/TESTHRESMODES.asm`
- `emulators/C64Programs/Code/Tests/TESTCIA1.asm`
- `emulators/C64Programs/Code/Tests/TESTCIA2NMI.asm`

When proposing a correction, include at least one read path and one write path if the change touches mapped memory.

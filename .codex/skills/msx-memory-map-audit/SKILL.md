---
name: msx-memory-map-audit
description: Validate MSX memory mapping in EMULATORS, including 16 KB pages, primary slot selection through PPI port #A8, expanded secondary slots and the #FFFF subslot register, inverted subslot reads, BIOS slot IDs and slot routines, ROM/RAM/cartridge placement, empty slots, and memory mapper ports #FC-#FF. Use when Codex reviews or edits MSX memory, slot/subslot, mapper RAM, cartridge, BIOS ROM placement, PPI slot changes, or CPU-visible address decoding.
---

# MSX Memory Map Audit

## Core Workflow

Use this skill to audit whether EMULATORS presents the same CPU-visible memory as a real MSX slot system.

1. Read `references/memory-map.md` before making claims or edits.
2. Inspect the relevant implementation paths first:
   - `include/MSX/Memory.hpp`, `src/MSX/Memory.cpp`
   - `include/MSX/Model.hpp`, `src/MSX/Model.cpp`
   - `include/MSX/PPI8255.hpp`, `src/MSX/PPI8255.cpp`
   - `include/MSX/PPI8255Registers.hpp`, `src/MSX/PPI8255Registers.cpp`
   - `include/MSX/PortManager.hpp`, `src/MSX/PortManager.cpp`
   - `include/MSX/MSX.hpp`, `src/MSX/MSX.cpp`
   - `emulators/MSXEmulator*/bios/*.rom`
3. When changing C++ code, also use `$emulators-framework-cpp-style`.
4. Separate primary slot selection, secondary slot selection, and mapper segment selection. They are related but not interchangeable.
5. For findings, cite the address page, selected primary slot, selected secondary slot if expanded, expected storage, and the class or method that violates it.

## Audit Priorities

Prioritize correctness in this order:

- #A8 primary slot register bit-pair decode for all four 16 KB pages.
- Expanded slot support: per-primary-slot subslot registers, #FFFF access only through selected page 3, and inverted readback.
- BIOS slot ID handling for RDSLT, WRSLT, CALSLT, ENASLT, RSLREG, and WSLREG compatibility.
- Correct placement of BIOS, BASIC, cartridges, RAM, and empty slots by model.
- Preservation/restoration of page 3 when code accesses a secondary slot register.
- Memory mapper ports #FC-#FF, write-only/unreliable read behavior, and per-mapper segment copies.
- Empty/open slot behavior documented and consistent across read/write paths.

## Validation Hints

Prefer narrow address-decoder tests:

- Write/read #A8 and verify each page maps the intended primary slot.
- For each expanded slot, select its primary in page 3, write #FFFF, read back the complemented value, and verify all four pages use the right subslot.
- Verify BIOS slot routines keep interrupts/page selection safe enough for the boot ROM.
- If mapper RAM exists, write distinct bytes into segments through #FC-#FF and check aliasing/repetition.

For emulator-level checks, compare with the BIOS ROMs in `emulators/MSXEmulator*/bios` and use small Z80 programs before relying on large cartridges.

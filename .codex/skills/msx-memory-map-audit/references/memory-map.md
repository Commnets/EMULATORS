# MSX Memory Map Reference

## Sources

- Local PDF: `.codex/docs/MSXBIOS.pdf` (scanned BIOS reference; text extraction is not reliable).
- Local PDF: `.codex/docs/MSXTechNotes.pdf` (MSX memory and I/O notes; text extraction is not reliable).
- Web: MSX Assembly Page, MSX I/O ports overview: https://map.grauw.nl/resources/msx_io_ports.php
- Web: MSX Assembly Page, MSX BIOS calls: https://map.grauw.nl/resources/msxbios.php
- Web: MSX Wiki, Slots: https://www.msx.org/wiki/Slots
- Web: MSX Wiki, Memory Mapper: https://www.msx.org/wiki/Memory_Mapper
- Web: MSX Wiki, System variables and work area: https://www.msx.org/wiki/System_variables_and_work_area

## CPU Address Space

The Z80 sees 64 KB split into four 16 KB pages:

| Page | Address range | Primary slot bits in #A8 |
| --- | --- | --- |
| 0 | #0000-#3FFF | bits 0-1 |
| 1 | #4000-#7FFF | bits 2-3 |
| 2 | #8000-#BFFF | bits 4-5 |
| 3 | #C000-#FFFF | bits 6-7 |

Each page selects one of four primary slots. A slot can contain ROM, RAM, cartridge hardware, mapper RAM, or no device. Do not assume RAM or ROM purely from the page number; model data and slot contents decide.

Common layouts:

- Main BIOS is normally in primary slot 0 or expanded slot 0-0.
- BASIC often appears in page 2, but cartridge and model layouts can alter what is visible.
- Cartridge ROMs commonly occupy #4000-#7FFF and sometimes #8000-#BFFF.
- Main RAM is often in page 3 on small MSX1 machines and can span multiple pages or mapper segments on larger machines.

Treat these as common patterns, not rules. The model definition should own the actual layout.

## Primary Slot Register #A8

PPI port A at #A8 is the primary slot select register. It is readable and writable. Each two-bit field selects primary slot 0..3 for one page.

When #A8 changes, the CPU-visible memory map changes immediately from the CPU's point of view. Audit for stale cached pointers or delayed remapping unless the delay is a deliberate CPU-cycle model.

## Secondary Slots and #FFFF

Each primary slot can be expanded into four secondary slots, also called subslots. This gives up to 16 logical slots.

The secondary slot select register for an expanded primary slot is memory-mapped at #FFFF, but only when that primary slot is selected into page 3 (#C000-#FFFF). This means direct subslot access usually requires temporarily changing page 3 primary slot selection, accessing #FFFF, then restoring page 3.

Subslot register bit format:

| Bits | Page |
| --- | --- |
| 0-1 | Subslot for #0000-#3FFF |
| 2-3 | Subslot for #4000-#7FFF |
| 4-5 | Subslot for #8000-#BFFF |
| 6-7 | Subslot for #C000-#FFFF |

Important quirks:

- A write to #FFFF stores the subslot selection for the expanded primary slot currently selected in page 3.
- A read from #FFFF returns the bitwise complement of the stored value.
- Non-expanded slots do not provide a real subslot register.
- Each expanded primary slot has its own subslot register state.
- BIOS work area `SLTTBL` mirrors the four secondary slot registers.

If an emulator stores only one global subslot byte, it cannot represent multiple expanded primary slots correctly.

## Slot IDs and BIOS Routines

BIOS slot routines accept a slot ID byte:

- Bits 0-1: primary slot number.
- Bits 2-3: secondary slot number when bit 7 is set.
- Bit 7: expanded slot flag.
- Bits 4-6 are not the primary/secondary selector.

Core BIOS routines:

| Address | Name | Purpose |
| --- | --- | --- |
| #000C | RDSLT | Read one byte from another slot |
| #0014 | WRSLT | Write one byte to another slot |
| #001C | CALSLT | Call routine in another slot |
| #0024 | ENASLT | Select slot for the page containing HL |
| #0138 | RSLREG | Read primary slot register #A8 |
| #013B | WSLREG | Write primary slot register #A8 |

RDSLT, WRSLT, CALSLT, and ENASLT disable interrupts internally in the BIOS. Direct slot switching should be audited for the same safety concerns: page 0 changes can crash if executing from the page being switched, and page 3 must be restored after #FFFF access.

## System Variables

Relevant BIOS variables:

- `EXPTBL` at #FCC1, four bytes. Bit 7 marks whether each primary slot is expanded. `EXPTBL+0`, also called `MNROM`, identifies the main ROM slot.
- `SLTTBL` at #FCC5, four bytes. Stores secondary slot register values for primary slots 0..3.
- `SLTATR` at #FCC9, 64 bytes. Stores slot attributes found during boot.
- `SLTWRK` at #FD09, 128 bytes. Per-slot work area used by ROM applications.
- `SLTSL` at #FFFF. Secondary slot select register visible only through an expanded slot selected in page 3; readback is inverted.

When booting a real BIOS, the emulator's slot hardware must support the BIOS detection algorithm, including inverted #FFFF reads.

## Memory Mapper RAM

Memory mapper RAM is separate from primary/secondary slot selection. It maps larger RAM into the currently selected RAM slot in 16 KB segments.

Standard mapper ports:

| Port | Page |
| --- | --- |
| #FC | Segment for #0000-#3FFF |
| #FD | Segment for #4000-#7FFF |
| #FE | Segment for #8000-#BFFF |
| #FF | Segment for #C000-#FFFF |

Important mapper behavior:

- Registers are normally write-only; reads are not reliable and should not be used for correctness.
- Each mapper device has its own copy of the four registers.
- If multiple mappers are present, writes to #FC-#FF update all mapper register copies at the same time; only the mapper in the selected slot affects CPU memory for that page.
- Mapper segment count depends on RAM size. Detecting size is normally done by writing patterns and finding repetition, not by trusting register reads.
- MSX1 BIOS generally does not initialize mapper segments. MSX2 and later BIOSes normally initialize mapper RAM during boot.

If EMULATORS models only MSX1 machines without mapper RAM, keep mapper behavior out of the baseline path but do not let #FC-#FF accidentally corrupt normal memory.

## Empty Slots and Open Behavior

The MSX standard describes slot selection but not every electrical detail of empty-slot bus values. Choose a model-consistent behavior, commonly #FF/open-bus-like reads, and keep it isolated in empty storage classes. Writes to empty slots should not create RAM unless a device explicitly does that.

## EMULATORS Review Checklist

- Does `MSX::Memory` track active primary slot and active subslot per 16 KB page?
- Does `MSX::SubSlotRegisters` store one register per primary slot and return inverted values on #FFFF reads?
- Does `LastPagePhysicalStorageSubset` only expose #FFFF behavior when the matching expanded primary slot is selected in page 3?
- Does #A8 remapping update all four pages without confusing primary slot fields with subslot fields?
- Do model definitions describe ROM/RAM/cartridge placement rather than hardcoded memory code guesses?
- Are BIOS slot routines able to boot real ROMs and detect expanded slots?
- If mapper RAM is present, are #FC-#FF writes routed to mapper hardware rather than generic memory bytes?

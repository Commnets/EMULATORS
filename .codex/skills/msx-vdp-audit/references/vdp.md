# MSX VDP Reference

## Sources

- Local PDF: `.codex/docs/TMS9918A.pdf` (TMS9918A/TMS9928A/TMS9929A data manual; text extraction is not reliable).
- Local PDF: `.codex/docs/MSXTechNotes.pdf` (MSX I/O and memory notes; text extraction is not reliable).
- Web: MSX Assembly Page, MSX I/O ports overview: https://map.grauw.nl/resources/msx_io_ports.php
- Web: MSX Assembly Page, MSX BIOS calls: https://map.grauw.nl/resources/msxbios.php
- Web: MSX Wiki, Texas Instruments TMS9918: https://www.msx.org/wiki/Texas_Instruments_TMS9918
- Web: TI TMS9918A/TMS9928A/TMS9929A data manual mirror: https://www.cs.columbia.edu/~sedwards/papers/TMS9918.pdf
- Web: TMS9918 Programmer's Guide: https://www.madrigaldesign.it/creativemu/files/tms9918_guide.pdf

## Scope and Models

MSX1 machines normally use a VDP from the TMS9918 family:

- TMS9918A: NTSC composite.
- TMS9928A: NTSC component/luminance-color difference output.
- TMS9929A: PAL component/luminance-color difference output.
- Compatible variants and MSX engines can differ in small quirks.

Baseline MSX1 assumptions:

- 16 KB VRAM.
- CPU accesses VRAM through VDP ports, not through CPU memory.
- Main visible resolution is 256 x 192 for graphics modes.
- Text modes include 40 x 24 and 32 x 24.
- Fixed 16-color palette where color 0 is transparent in the relevant contexts.
- 32 hardware sprites, one color each, maximum 4 visible sprites on a scanline.

Do not apply V9938/V9958 behavior to TMS9918A code. Ports #9A and #9B are V9938/V9958-only standard ports.

## Ports #98 and #99

MSX standard VDP ports:

| Port | Direction | Function |
| --- | --- | --- |
| #98 | read/write | VRAM data |
| #99 | write | Register write or VRAM address setup |
| #99 | read | Status register |

Control-port write protocol:

- Register write: write data byte to #99, then write `#80 | register_number` to #99.
- VRAM address setup: write low address byte to #99, then write high/control byte to #99.
- In address setup, bit 7 of the second byte is 0. Bit 6 selects access direction; 0 is read setup and 1 is write setup.
- The VRAM address auto-increments after each data read/write through #98.

Audit for a two-byte latch. A single #99 write must not immediately change a register or address until the second control byte arrives.

## VRAM Read/Write Buffering

TMS9918A data-port reads use an internal read buffer. After setting a read address, the VDP prefetches VRAM for the first #98 read and auto-increments after data access. Writes through #98 store the byte at the current VRAM address and then increment.

Common failure modes:

- Losing the first byte after SETRD/RDVRM.
- Incrementing on #99 address setup instead of #98 data access.
- Sharing CPU memory and VRAM storage accidentally.
- Allowing CPU direct memory reads to see VRAM.

## Registers R0-R7

TMS9918A exposes eight writable registers:

| Register | Main purpose |
| --- | --- |
| R0 | Mode control, including Graphics II-related mode bit and external video bit |
| R1 | Display enable, VBlank interrupt enable, mode bits, sprite size/magnification |
| R2 | Pattern name table base |
| R3 | Color table base |
| R4 | Pattern generator table base |
| R5 | Sprite attribute table base |
| R6 | Sprite pattern generator table base |
| R7 | Text/graphics color and backdrop color |

The exact usable address bits depend on the register and display mode. Apply masks, not raw shifts:

- Pattern name table is selected in 1 KB units.
- Pattern generator table is selected in 2 KB units.
- Sprite attribute table is selected in 128-byte units.
- Sprite generator table is selected in 2 KB units.
- Graphics II mode uses pattern/color table masking behavior across thirds of the screen; some clones differ, so isolate this logic.

The VDP registers are not ordinary readable CPU registers on TMS9918A. If the emulator exposes "peek" for debugging, keep it separate from CPU-visible reads.

## Display Modes

Documented MSX1 modes:

| MSX BASIC | VDP mode | Notes |
| --- | --- | --- |
| SCREEN 0 width 40 | Text | 40 x 24 text, no sprites |
| SCREEN 1 | Graphics I | 32 x 24 character/tile mode, sprites |
| SCREEN 2 | Graphics II | Bitmap-like tiled mode, sprites |
| SCREEN 3 | Multicolor | 64 x 48 blocky color mode, sprites |

Mode selection comes from mode bits spread across R0 and R1. Do not derive mode solely from BASIC screen numbers; software can write VDP registers directly.

Graphics II is the mode most likely to reveal table-address bugs. It uses screen thirds and separate pattern/color addressing rules that are easy to flatten incorrectly.

## Status Register

Reading #99 returns the VDP status register.

Important bits:

- Bit 7: VBlank/interrupt flag.
- Bit 6: fifth-sprite flag.
- Bit 5: sprite collision flag.
- Bits 0-4: index of the first sprite involved in fifth-sprite overflow.

Reading the status register clears the latched interrupt/overflow/collision flags. It also affects the interrupt line: if the VBlank flag is cleared, INT should deassert unless another condition keeps it asserted.

Keep status read side effects out of debugger-only peek paths.

## Sprites

Sprite data:

- Sprite attribute table has 4 bytes per sprite: Y, X, pattern name, color/early-clock bits.
- There are 32 sprites.
- Sprite pattern data is 8 bytes for 8x8 sprites or 32 bytes for 16x16 sprites.
- R1 selects 8x8 versus 16x16 and normal versus magnified output.
- Lower-numbered sprites have higher priority.

Y coordinate behavior:

- The stored Y coordinate is one less than the displayed top line.
- Attribute Y value #D0 terminates the active sprite list.
- Values after #D0 can be used for negative/offscreen placement depending on chip rules; verify before simplifying.

Line rules:

- Only four sprites are displayed on a scanline.
- If a fifth sprite is encountered, the fifth-sprite flag is set and bits 0-4 report the first dropped sprite.
- Collision is set when nontransparent sprite pattern bits overlap; the status register does not identify the pair.
- Transparent sprite color still matters for visual output, but collision depends on pattern bits.

## Timing and Interrupts

TMS9918A-family chips generate a vertical interrupt at frame rate when enabled in R1. PAL and NTSC variants differ in frame rate. Use the selected model to choose timing rather than hardcoding one standard.

Unlike C64 VIC-II memory fetches, MSX VDP VRAM is separate from CPU RAM and does not steal CPU memory cycles for display fetches. CPU-visible timing issues mainly come from Z80 I/O timing, the VDP port protocol, and interrupt timing.

## BIOS Compatibility

BIOS VDP routines rely on port semantics:

- WRTVDP #0047 writes VDP registers.
- RDVRM #004A reads VRAM.
- WRTVRM #004D writes VRAM.
- SETRD #0050 prepares a VRAM read address.
- SETWRT #0053 prepares a VRAM write address.
- FILVRM #0056, LDIRMV #0059, and LDIRVM #005C rely on auto-increment.
- CHGMOD #005F writes coherent mode/table/color state.

If direct VDP monitor commands work but BIOS routines fail, suspect latch/buffer/auto-increment behavior first.

## EMULATORS Review Checklist

- Does `VDPPortManager` implement the #98/#99 protocol with a real two-byte latch?
- Does `TMS99xxFamilyRegisters` separate CPU-visible register writes from debugger peeks?
- Does VRAM data access auto-increment only on #98 data reads/writes?
- Does status read clear flags and update interrupt output?
- Do table base calculations mask register bits according to mode?
- Does Graphics II use the correct screen-third pattern/color behavior?
- Does sprite evaluation enforce Y offset, #D0 terminator, 4-per-line overflow, priority, and collision?
- Does `MSX::Screen` render from VDP state without becoming the owner of register semantics?

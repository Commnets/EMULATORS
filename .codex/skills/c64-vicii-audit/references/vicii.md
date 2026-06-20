# C64 VIC-II Reference

## Sources

- Local PDF: `.codex/docs/VICII.pdf` (52 pages). Text extraction found badline, sprite, D011, D012, D018, VC/RC, raster, refresh, BA, and AEC material.
- Web: Christian Bauer, "The MOS 6567/6569 video controller (VIC-II) and its application in the Commodore 64": https://www.cebix.net/VIC-Article.txt
- Web: C64-Wiki VIC-II: https://www.c64-wiki.com/wiki/VIC-II
- Web: C64-Wiki Memory Map and Bank Switching: https://www.c64-wiki.com/wiki/Memory_Map and https://www.c64-wiki.com/wiki/Bank_Switching

## Model Selection

Always identify the modeled chip/standard before asserting exact timings:

- PAL 6569/8565 commonly uses 312 raster lines and 63 cycles per line.
- NTSC variants differ in line count and cycles per line; old 6567 revisions are not identical to later 6567R8 behavior.
- The CPU clock is derived from the VIC. PAL and NTSC therefore have different CPU frequencies.

If the code does not expose a model setting, check whether it assumes PAL. Flag hardcoded timings if the emulator claims multi-standard support.

## Register Map Essentials

VIC-II registers are mapped at $D000-$D02E and mirrored every 64 bytes through $D3FF.

Key registers:

- $D000-$D00F: sprite X/Y low bytes for sprites 0..7.
- $D010: high X bits for sprites 0..7.
- $D011: RST8, ECM, BMM, DEN, RSEL, YSCROLL. Bit 7 is raster bit 8 when paired with $D012.
- $D012: raster compare low byte and raster counter readback.
- $D015: sprite enable bits.
- $D016: RES, MCM, CSEL, XSCROLL.
- $D017/$D01D: sprite Y/X expansion.
- $D018: video matrix and character/bitmap base pointers inside the selected VIC bank.
- $D019: interrupt flags. Writing 1 bits clears corresponding flags.
- $D01A: interrupt enables.
- $D01B/$D01C: sprite priority and multicolor bits.
- $D01E/$D01F: sprite-sprite and sprite-data collision flags; reads clear them.
- $D020-$D02E: border/background/sprite colors.

Unconnected bits generally read as 1. Unimplemented $D02F-$D03F reads return $FF and writes are ignored.

## Memory Fetches

The VIC performs a memory access every cycle. Normal cycle sharing gives the VIC the first phase and the 6510 the second phase; extra VIC needs steal CPU-visible phases through BA/AEC.

Access classes:

- c-access: video matrix plus color RAM fetch.
- g-access: character generator or bitmap data fetch.
- p-access: sprite pointer fetch.
- s-access: sprite data fetch.
- r-access: DRAM refresh.
- i-access: idle access.

The video matrix is 1000 entries (40 x 25) and can be placed in 1 KB steps via $D018 VM bits. Character data is in 2 KB steps for text; bitmap data is 8 KB-aligned by the bitmap-relevant $D018 bit. All addresses are relative to the selected VIC bank, except color RAM, which is separate 4-bit storage addressed by the lower 10 VIC address bits.

## Badlines

A badline condition exists when, at the relevant cycle boundary:

- raster is in the display range $30..$F7,
- the low three raster bits equal YSCROLL from $D011,
- DEN was set for at least one cycle during raster line $30.

Badlines are not simply "every eighth line"; mid-frame writes to $D011 can create or suppress them. Correct emulation must model the condition at cycle granularity for FLD, FLI, linecrunch, and VSP-style effects.

Important consequences:

- At cycle 14, VC is loaded from VCBASE; if the badline condition is present there, RC resets to 0.
- If the badline condition occurs in cycles 12..54, BA goes low and c-accesses occur in cycles 15..54.
- BA goes low before the VIC owns the bus; the CPU can continue pending writes, then stops on the first read while RDY/BA is low.
- At cycle 58, if RC is 7 and no badline keeps display active, display logic moves to idle and VCBASE is updated from VC.

## Display State

The graphics sequencer has display and idle states. Badlines transition idle to display. In idle state the VIC still performs g-access-like fetches from an idle address and can display buffered/idle data. VC, VCBASE, RC, and VMLI behavior is central to many demo effects; do not replace it with only scanline-level rendering if timing fidelity is required.

Graphics modes from ECM/BMM/MCM:

- 000: standard text.
- 001: multicolor text.
- 010: standard bitmap.
- 011: multicolor bitmap.
- 100: ECM text.
- 101, 110, 111: invalid modes with specific blank/black behavior; validate before simplifying.

## Sprites

Each sprite uses:

- Position registers plus $D010 high X bits.
- Enable, X/Y expansion, multicolor, priority, and color registers.
- A pointer byte in video matrix offsets $3F8-$3FF.
- 63 bytes of sprite data, fetched as three bytes per displayed sprite row.

Audit sprite logic for:

- DMA start based on enabled sprites and Y comparison.
- Pointer and data fetch timing, including bus stealing.
- MC/MCBASE advancement and termination at 63.
- X expansion shifting every other pixel.
- Multicolor two-bit pixel decoding.
- Priority against background graphics and other sprites.
- Sprite-sprite and sprite-data collision flag set/clear behavior.
- Vertical sprite reuse when Y registers are changed after display.

## IRQs, Borders, and Lightpen

The four VIC interrupt sources are raster compare, sprite-sprite collision, sprite-data collision, and lightpen. $D019 holds flags; $D01A enables them. IRQ output is maskable by the CPU.

Border flip-flops depend on DEN, CSEL, RSEL, and comparison positions. Demos often rely on changing these bits at precise cycles, so audit border behavior together with raster timing, not only visible pixel output.

## EMULATORS Review Checklist

- Does `VICIIRegisters` implement read masks, write-only/read-only side effects, mirrors, and flag clearing?
- Does `VICII` advance by cycles, scanlines, or frames? Match reported fidelity to actual granularity.
- Does the VIC fetch memory through a VIC-specific path that honors CIA2 bank selection and color RAM side data?
- Does badline generation depend on DEN history at line $30 and current YSCROLL/raster low bits?
- Does CPU stalling flow through the same clock/bus mechanism used by the CPU class?
- Do local raster/sprite tests exercise visible behavior after any change?

# ZX81 Video Reference

## Contents

- Scope and configuration axes
- Source hierarchy
- Shared character-generation path
- SLOW and FAST modes
- I/O, sync, WAIT, and interrupts
- Timing and compatibility boundaries
- Audit invariants
- Source index

## Scope and Configuration Axes

Use this reference for `/mZX811`, `/mZX812`, and `/mZX813`.

Keep these axes independent:

- ROM selector: old, rare/intermediate, or newest image.
- ULA silicon revision: for example early 2C184E versus later 2C210E.
- Video standard: 50 Hz/PAL-like versus 60 Hz/NTSC-like timing selected by hardware and ROM margins.
- RAM configuration: base memory versus expansion.
- Execution mode: SLOW versus FAST.

The three repository selectors identify ROM images. They do not, by themselves, prove a particular ULA revision. Hash or disassemble the loaded ROM before relying on routine addresses or delay loops.

## Source Hierarchy

Prefer evidence in this order:

1. The local `.codex/docs/ZX81Video.pdf` for the supplied article's schematics, counter diagrams, `/WAIT` circuit, oscilloscope traces, and porch discussion.
2. Original Sinclair service/circuit documentation.
3. The Zilog Z80 manual for M1, refresh, HALT, WAIT, INT, NMI, and acknowledge semantics.
4. Wilf Rigter's detailed video-system description and ROM listings.
5. Tynemouth Software's measured reconstruction.
6. Nocash timing tables and ROM archives/disassemblies, with disputed counts checked against signal edges.

The local PDF is an image-only print of Dave Curran's "How the ZX81 Generates Video." Render pages rather than relying on text extraction.

## Shared Character-Generation Path

ZX81 retains the central ZX80 technique:

1. ROM jumps into a high-address echo of `D_FILE`.
2. A qualifying `/M1` opcode fetch lets the ULA capture the display byte.
3. If bit 6 is clear, the ULA forces the CPU data bus to `0x00`, producing a four-T-state NOP.
4. The low six bits select a glyph and bit 7 inverts its video.
5. During refresh, I/R plus ULA character and line state address the glyph byte.
6. The ULA loads an eight-bit shift register and emits two pixels per CPU T-state.

The standard ZX81 character set is selected with `I=0x1E`, near `0x1E00`. The 3-bit line counter supplies glyph rows 0-7. `HALT` (`0x76`) terminates a display row; R/A6 and maskable INT timing release the CPU and schedule the next raster line.

Collapsed display rows end early with `HALT`; the remainder of the row is blank. Fully expanded text is 32 by 24 characters, eight scanlines per character row.

## SLOW and FAST Modes

SLOW mode divides a frame into four tasks:

1. VSYNC, frame accounting, and keyboard scan with the NMI generator off.
2. Upper blank lines while user code runs and is interrupted by one NMI per line.
3. The 192-line display routine with NMI off and the HALT/INT character path active.
4. Lower blank lines while user code again runs between NMIs.

The NMI handler at `0x0066` counts blank lines and switches between user execution and ROM video routines. Validate register-bank exchanges, saved application state, and the one-T-state HALT/WAIT synchronization used at transitions.

FAST mode leaves the NMI generator disabled and gives user code the CPU. The display is not maintained during arbitrary computation. When the ROM intentionally generates a picture in FAST mode, it uses a ZX80-compatible software display sequence, including software-generated blank rows.

Test mode transitions, not only steady state. Enabling SLOW mid-frame, starting VSYNC, or encountering the final visible row can expose duplicate or missing lines.

## I/O, Sync, WAIT, and Interrupts

Decode I/O from address bits and bus direction, not only literal port names:

- `OUT (0xFE),A` conventionally enables the NMI generator.
- `OUT (0xFD),A` conventionally disables it.
- A keyboard read with A0 low starts/clamps VSYNC when the relevant conditions hold and resets/blocks the line counter.
- An output operation ends VSYNC and restores normal horizontal-sync/video generation; exact aliases follow decoded address bits.
- The 50/60 Hz input bit informs ROM margin selection.

When NMI generation is enabled, the horizontal timing source also produces `/NMI`. External transistor logic uses `/HALT` and `/NMI` to pull `/WAIT` low so the CPU services NMI at the required T-state. A HALTed CPU is treated differently so its current HALT cycle can complete.

Visible rows normally end through `HALT` and maskable INT. Blank rows in SLOW mode use the free-running horizontal counter and NMI. These paths meet at line-reset/HSYNC state; ensure one and only one source advances each line.

NMI has priority over INT in the Z80. Preserve ordering near a shared boundary and validate the actual acknowledge edge.

## Timing and Compatibility Boundaries

- Master/ULA clock: approximately 6.5 MHz.
- CPU clock: approximately 3.25 MHz.
- Eight pixels are emitted during a four-T-state NOP.
- Nominal horizontal period is about 64 microseconds.
- Sources describe a terminal line count as 207 or 208 CPU cycles depending on whether they report elapsed cycles, counter states, or the early INT reset. Always name the convention and compare actual edges.
- Normal text occupies 256 active pixels per row and 192 visible scanlines.
- PAL-like and NTSC-like systems use different upper/lower blank-line margins; SLOW-mode user CPU time therefore differs.

The early ULA output did not provide a conventional back porch. Later 2C210E devices added one. Composite-video modifications and modern replacements may add pixel synchronization or porch shaping absent from the selected original revision.

Pseudo-hi-res relies on manipulating line-counter/INT behavior. True hi-res software may source pixel bytes from RAM during refresh and can depend on RAM-pack electrical behavior. Do not declare compatibility from standard text output alone.

## Audit Invariants

- One qualifying M1 fetch captures one character and forces one NOP only when bit 6 is clear.
- Bit 7 changes polarity without changing the six-bit glyph index.
- Glyph fetch uses the correct I page and row counter for the selected machine/ROM state.
- Shift-register load and output remain phase-locked at eight pixels per character.
- `HALT`/INT and counter/NMI paths cannot both advance the same raster line.
- NMI enable/disable and VSYNC side effects follow decoded I/O aliases.
- `/WAIT` is asserted only under the modeled NMI/HALT conditions and does not become a generic CPU stall.
- SLOW mode executes user code only in blank-line windows; FAST mode does not retain a synthetic picture.
- ROM-specific addresses and delay constants are verified for ZX811, ZX812, and ZX813 instead of assumed equal.
- Reports distinguish ROM image, ULA revision, video standard, RAM configuration, and counter convention.

## Source Index

- Local visual authority: `.codex/docs/ZX81Video.pdf`
- Tynemouth Software, [How the ZX81 Generates Video](https://blog.tynemouthsoftware.co.uk/2023/10/how-the-zx81-generates-video.html)
- Wilf Rigter, [ZX81 Video Display System](https://k1.spdns.de/Vintage/Sinclair/80/Sinclair%20ZX80/Tech%20specs/Wilf%20Rigter%27s%20ZX81%20Video%20Display%20Info.htm)
- Nocash, [Sinclair ZX specifications](https://k1.spdns.de/Develop/Projects/zxsp/Info/nocash%20Sinclair%20ZX%20Specs.html)
- Sinclair Research, [ZX81 Service Manual and Assembly Instructions](https://datassette.org/manuais/uk-reino-unido-hardware-zx-80-81-sinclair-manuais/zx81-service-manual-and-assembly-instructions)
- Zilog, [Z80 CPU User Manual](https://www.zilog.com/docs/z80/um0080.pdf)
- Andrew Rea, [ZX81 ULA replacement and signal description](https://oldcomputer.info/8bit/zx81/ULA/ula.htm)
- ZX81 ROM archive, [standard and improved ROM versions](https://k1.spdns.de/Vintage/Sinclair/80/Sinclair%20ZX81/ROMs/)
- ZX81 ROM archive, [unimproved versus improved differences](https://k1.spdns.de/Vintage/Sinclair/80/Sinclair%20ZX81/ROMs/zx81%20version%202%20%27improved%27%20rom%20differences.htm)

Use Internet sources as research aids, not as copied content. Record the access date when adding new facts to this reference.

# ZX80 Video Reference

## Contents

- Scope and source hierarchy
- Machine and timing model
- Display-file execution
- Character and sync generation
- Audit invariants
- Source index

## Scope and Source Hierarchy

Use this reference for `/mZX80` only. The code lives in namespace `ZX81`, but ZX80 uses discrete TTL logic and has no ZX81 NMI generator.

Prefer evidence in this order:

1. The local `.codex/docs/ZX80Video.pdf` for the supplied article's diagrams, logic-analyser traces, and visual examples.
2. Original Sinclair circuit/service documentation for wiring and component-level behavior.
3. The Zilog Z80 manual for M1, refresh, HALT, interrupt, and acknowledge semantics.
4. Detailed reconstructions and measurements from Tynemouth Software.
5. Emulator specifications such as Nocash for compact timing tables; verify disputed edge counts against traces.

The local PDF is an image-only print of Dave Curran's "How the ZX80 Generates Video." Render pages rather than relying on text extraction.

## Machine and Timing Model

- Master/video clock: approximately 6.5 MHz.
- Z80 clock: approximately 3.25 MHz, one half of the video clock.
- A Z80 NOP consumes four T-states while eight pixels are shifted at the master-clock rate.
- The standard character area is 32 columns by 24 character rows, eight raster lines per character.
- ZX80 video is generated only while ROM software dedicates the CPU to the display. Normal computation or key processing makes the picture disappear or flicker.
- The original circuit does not provide a ZX81-style NMI generator. Do not fabricate SLOW mode.
- The repository currently starts `/mZX80` as PAL and without RAM expansion. Treat that as emulator scope, not a complete statement about every upgraded physical ZX80.

## Display-File Execution

The ROM prepares a high-address echo of the display file and jumps into it. Audit the actual bus qualification, not only the logical address.

During a qualifying opcode fetch:

1. The display hardware captures the memory byte as a character code.
2. If bit 6 is clear, hardware makes the CPU see `0x00`, so the Z80 executes a NOP and the PC advances.
3. The captured low six bits select one of 64 glyphs.
4. Bit 7 selects inverse video.
5. During the refresh portion of the M1 cycle, the I/R bus state and external logic form the glyph address.
6. The glyph byte is loaded into the shift register and emitted at 6.5 MHz.

For the standard ZX80 ROM, `I=0x0E` selects the character table near `0x0E00`. The eight-line character-row counter supplies the low glyph-row bits. Verify the physical address mux from schematics and traces; do not infer it from a convenient framebuffer formula alone.

Bit-6-set bytes are not converted to NOP. The normal row terminator is `0x76` (`HALT`). When encountered, the Z80 enters HALT and performs repeated internal NOP-like M1/refresh cycles until an enabled maskable interrupt releases it.

The R refresh register is part of horizontal positioning. A6 is coupled to the maskable interrupt path; line termination, R progression, `/INT`, interrupt acknowledge, and HSYNC therefore require cycle-level agreement.

## Character and Sync Generation

- A row can terminate before 32 characters. The remainder is blank, which allows a collapsed display file to save RAM.
- A fully expanded display file contains 32 character bytes per row plus row terminators; a collapsed row can contain only its terminator.
- Character bits must shift with a constant pixel width. Alternating or checkerboard glyphs expose a load/shift phase error quickly.
- Horizontal sync is coupled to the ROM/interrupt display sequence rather than an independent modern raster controller.
- Vertical sync is produced by ROM-controlled keyboard/video I/O. ZX80 cassette output is also coupled to the video/sync path; audit shared side effects when changing port logic.
- Original ZX80 composite output lacks a proper back porch. Do not "correct" it to broadcast-standard timing unless the emulated hardware explicitly includes a later modification.

## Audit Invariants

- Exactly one character byte is captured per qualifying display-file opcode fetch.
- A bit-6-clear byte is both displayed and replaced with NOP for the CPU.
- Bit 7 affects polarity, not glyph index.
- Bits 0-5 and the 3-bit row counter select the glyph byte.
- No new character overwrites a shift register that still has pending pixels.
- `HALT` ends the logical row and the interrupt releases it at the intended horizontal phase.
- INT acknowledge advances the raster/line state once, not once per CPU and once per ULA path.
- User-code intervals do not retain a synthetic stable framebuffer.
- Counter values in reports always state whether they count CPU T-states or 6.5 MHz clocks.

## Source Index

- Local visual authority: `.codex/docs/ZX80Video.pdf`
- Tynemouth Software, [How the ZX80 Generates Video](http://blog.tynemouthsoftware.co.uk/2023/10/how-the-zx80-generates-video.html)
- Tynemouth Software, [How the ZX80 Works](https://blog.tynemouthsoftware.co.uk/2019/10/how-the-zx80-works.html)
- Sinclair Research, [ZX80 original manuals and assembly/service documentation](https://worldofspectrum.net/item/1000979/)
- Zilog, [Z80 CPU User Manual](https://www.zilog.com/docs/z80/um0080.pdf)
- Nocash, [Sinclair ZX specifications](https://k1.spdns.de/Develop/Projects/zxsp/Info/nocash%20Sinclair%20ZX%20Specs.html)
- Wilf Rigter, [ZX video tutorial](https://quix.us/timex/rigter/ZX%4020Video%4020Tutorial.html)

Use Internet sources as research aids, not as copied content. Record the access date when adding new facts to this reference.

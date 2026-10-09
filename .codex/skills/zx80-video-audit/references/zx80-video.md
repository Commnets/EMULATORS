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
- Distinguish INT response, logical line synchronization and presentation wrap. The current ZX80 model suppresses duplicate logical-line updates during its trailing interval; acknowledgement is not itself a vertical presentation advance.
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
## Port FE Readback and Regression Checks

Verified 2026-09-26; bit numbering is D0 through D7.

- D0-D4: keyboard columns, active low.
- D5: unused, normally read as 1.
- D6: television-standard selection, 1 for 50 Hz and 0 for 60 Hz.
- D7: cassette input. Do not describe it as a dedicated readable VSYNC-status bit. Reading the port can change sync state as a side effect; that is distinct from its returned data bits.
- Do not import the ZX Spectrum EAR-on-D6 mapping into ZX80/ZX81.

Evidence: [Andy Rea's ZX81 ULA replacement](https://oldcomputer.info/8bit/zx81/ULA/ula.htm), section "Port $FE input group", explicitly maps tape input to bit 7 and UK/US selection to bit 6. [Sinclair ZX specifications](https://k1.spdns.de/Develop/Projects/zxsp/Info/nocash%20Sinclair%20ZX%20Specs.html), section "ZX80/ZX81 I/O Ports", gives the same readback map for both machines.

Independent firmware check in the repository images: `emulators/ZX81Commons/bios/zx80.rom` at 0x0232 and `zx81_3.rom` at 0x038B contain `DB FE 17` (`IN A,(FE); RLA`) in their cassette sampling loops, followed by carry-dependent control flow. RLA transfers the original D7 into carry. These addresses are evidence for those inspected images, not invariants for every ROM revision.

When moving EAR from an incorrect D6 assignment to D7, check the whole returned byte: this also stops overwriting the configured D6 standard selection. Compare ROM control flow and video timing before and after. A missing cursor alone does not establish whether execution stopped, video generation failed, or the output was cropped. Do not prescribe a fixed crop shift from the ROM margin difference alone: establish actual output positions and line events first, and label any exploratory crop change as diagnostic rather than a proven repair.

## Current EMULATORS Implementation (2026-10-08)

This section describes the implemented approximation, not a claim of pin-level hardware equivalence. Read it alongside `include/src/ZX81` and the maintained debug-format source.

- `MemoryVideoCode` intercepts qualifying opcode fetches with A15 high and bit 6 clear. Operand/data/inspection reads preserve the byte; HALT cycles do not capture characters.
- Captures retain pre-increment I:R. Pattern loading is delayed by 8 ULA pixels for ZX80 and 7 for ZX81. The second M1 is timestamped at instruction start + 4 T, excluding WAIT. Character Capture distinguishes logical CaptureClock from ObservedClock. PerCycle delivery can still be late and cycle prediction can decode the unmodified byte; timestamping alone does not solve either limitation.
- `/w2` equips ZX81 with refresh-capable 16K RAM. A captured I:R address selecting that RAM or its ULA-view mirror supplies the pattern; other addresses use the character path. This fallback does not model an undriven bus and is not a guarantee of universal WRX compatibility. ZX80 configuration remains unexpanded.
- ZX81 uses an independent horizontal generator: 414 ULA clocks per period, HSYNC start at 32 and end at 64. Accepted INT response schedules reset two CPU T-states later. HSYNC start advances the unblocked character-row counter and requests NMI when enabled. Counter wrap alone is not HSYNC. The 207-T baseline is a compatibility choice, not a resolution of the 207/208-T hardware question.
- ZX80 retains its coarse logical-line model: accepted INT or the presentation-tail position updates the character-row counter once per active trailing interval. It has no NMI generator. Do not apply ZX81 generator events to ZX80 logs.
- Presentation is separate: INT response aligns the horizontal position; horizontal wrap advances the displayed row. `Line Advance` is presentation, `Line Sync` is ZX80, and `INT Response`, `Horizontal Reset`, `HSync Start/End` describe ZX81 timing stages.
- CPU IN/OUT accesses are buffered and released at transaction boundaries through CZ80::executeBufferedCommands; their saved clocks do not yet schedule exact intra-instruction chip events. Input completion updates destination registers/flags and block-input memory writes before the next CPU transaction. An actual even-port read drives MIC low and starts VSYNC when NMI is disabled and VSYNC is inactive. Entry restarts presentation and the coarse generator phase, blocking LINECNTRL at 0 for ZX80 or 7 for ZX81. Any output ends VSYNC and releases the block. ZX81 outputs decode A1-low disable followed by A0-low enable; odd outputs drive MIC high. PEEK preserves hardware state and event markers, although it can produce debug output.
- A6 is sampled as an active-low INT level after a completed instruction, using the recorded last bus address. Request eligibility and actual CPU acceptance are distinct; the ULA is told the accepted response-start clock directly by `ZX81::CZ80`.
- PerCycle mode can consume a batch before `specificComputerCycle` and chip simulation. Intermediate boundaries can be lost and the single INT latch can be overwritten. This accepted scheduling limitation is documented in that method.
- Only ULA-origin NMI accepted during HALT with latency 0..4 T receives the ZX81 response extension of `17 - latency` T. It is phase compensation, not a pin-level WAIT implementation. ZX80 receives no extension; ordinary-instruction WAIT is not modeled.
- `Port Read` uses the last ULA simulation clock, not the physical I/O sampling instant. Do not derive exact VSYNC widths or a fixed pixel correction from that timestamp alone.

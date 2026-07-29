# ZX Spectrum 48K ROM Reference

## Contents

- Scope and identity
- Source hierarchy and local PDF locator
- ROM organization and hardware contract
- Reset, restarts, and interrupts
- Keyboard and beeper
- Tape
- Screen, printer, channels, and streams
- BASIC, memory, and calculator
- Trap design and validation
- Audit invariants
- Repository map
- Source index

## Scope and Identity

Apply this reference primarily to the standard English ZX Spectrum 48K ROM. The supplied disassembly describes a 16,384-byte monitor/BASIC ROM derived from earlier Sinclair machines.

The repository's canonical English image is:

```text
emulators/ZXSpectrumCommons/bios/48Standard-ENG.rom
size:   16384 bytes
SHA256: D55DAA439B673B0E3F5897F99AC37ECB45F974D1862B4DADB85DEC34AF99CB42
```

The same hash is present in local Fuse and EightyOne copies. Hash every selected image independently. Spanish, Nordic, Arabic, prototype, 128K, +2, and replacement ROMs can change code, strings, tables, and entry points.

Treat routine addresses as image-local facts, not a platform ABI.

## Source Hierarchy and Local PDF Locator

Prefer:

1. `.codex/docs/ZXSpectrumROM.pdf`, Ian Logan and Frank O'Hara, *The Complete Spectrum ROM Disassembly*.
2. The actual ROM bytes loaded by the emulator.
3. Sinclair Research, *ZX Spectrum BASIC Programming*, for documented behavior and system variables.
4. Sinclair Research, *ZX Spectrum Service Manual*, for the firmware/hardware boundary.
5. Zilog, *Z80 CPU User Manual*, for CPU-visible semantics.
6. Modern searchable disassemblies and mature emulators as navigation/comparison aids.

The PDF has extractable text. Its major sections begin at:

| Subject | Printed page | PDF page |
| --- | ---: | ---: |
| Contents and introduction | i | 3-5 |
| Restarts and tables | 1 | 6 |
| Keyboard | 5 | 10 |
| Loudspeaker | 11 | 16 |
| Cassette | 15 | 20 |
| Screen and printer | 33 | 38 |
| Executive and initialization | 59 | 64 |
| BASIC line/command interpretation | 84 | 89 |
| Expression evaluation | 127 | 132 |
| Arithmetic | 164 | 169 |
| Floating-point calculator | 190 | 195 |
| Algorithms and numeric appendices | 222 | 227 |
| Routine index | 231 | 236 |

Search the local PDF by routine label or hexadecimal address, then read the complete surrounding routine including all alternate entry points and fall-through paths.

## ROM Organization and Hardware Contract

The ROM is organized into:

- input/output routines;
- BASIC interpreter;
- expression and numeric handling.

Its behavior depends on:

- accurate Z80 flags, alternate registers, stack, block instructions, interrupt flip-flops, HALT, and instruction timing;
- port FE partial decoding, keyboard matrix, tape edges, border, and beeper;
- 50/60 Hz frame interrupts;
- screen and attribute memory layout;
- 16K/48K installed-memory behavior;
- writable system variables beginning at `0x5C00`.

A failure while running the ROM can therefore be a CPU, ULA, memory, port, tape, or firmware integration bug. Localize the first divergent bus/state event before patching the ROM-facing layer.

## Reset, Restarts, and Interrupts

Key standard-English entry points:

| Address | Role |
| ---: | --- |
| `0x0000` | START; disables interrupts and enters initialization |
| `0x0008` | error restart |
| `0x0010` | print A restart |
| `0x0018` | collect current character |
| `0x0020` | collect next character |
| `0x0028` | floating-point calculator restart |
| `0x0030` | make BC spaces |
| `0x0038` | IM1 maskable interrupt |
| `0x0066` | NMI path |
| `0x11CB` | START/NEW common initialization |

The IM1 routine at `0x0038` preserves AF/HL/BC/DE, increments the three-byte `FRAMES` counter, calls `KEYBOARD` at `0x02BF`, restores state, executes EI, and returns. Validate the Z80's delayed acceptance after EI.

The disassembly documents a defect in the standard NMI path around `NMIADD`: the conditional branch is opposite to the commentary's intended reset behavior. Preserve the bytes and observed behavior for the unpatched ROM.

Initialization:

- probes RAM and establishes top-of-memory state;
- initializes system variables and user-defined graphics;
- selects IM 1 and enables interrupts;
- constructs initial channels and streams;
- clears the printer buffer and screen;
- distinguishes 16K from 48K through actual writable memory.

Do not pre-fill final system-variable values if that hides broken RAM probing, stack, or port behavior.

## Keyboard and Beeper

`KEY-SCAN` at `0x028E` scans eight keyboard half-rows through full 16-bit port addresses with low byte FE. It returns key identities and shift state and rejects unsupported three-key combinations. `KEYBOARD` at `0x02BF` runs on every maskable interrupt.

The keyboard path maintains two KSTATE sets so a new key can be recognized while a previous one is in its repeat period. It updates `LAST_K` and the new-key flag in `FLAGS`, using `REPDEL` and `REPPER`.

Test:

- no key;
- each of 40 keys;
- CAPS SHIFT and SYMBOL SHIFT alone and with another key;
- two ordinary keys and invalid three-key cases;
- hold through debounce, initial repeat delay, and repeat cadence;
- scan with several keyboard rows selected;
- PAL 50 Hz and NTSC 60 Hz timing.

ROM keyboard repeat is firmware behavior. Electrical row selection and active-low combination belong to the ULA/port model.

The loudspeaker routines generate pitch through precisely timed port output loops. A correct final note with wrong edge timing is still a defect. Run beeper/tape code from both contended and uncontended RAM to expose incorrectly applied waits.

## Tape

Principal standard-ROM entry points include:

- `SA-BYTES` at `0x04C2`;
- `LD-BYTES` at `0x0556`;
- command-level SAVE/LOAD/VERIFY dispatch later in the ROM.

The standard format writes a 17-byte header block followed by a data block. Each block includes a flag byte and XOR checksum. SAVE and LOAD use carefully timed MIC/EAR transitions and long leader/sync sequences.

Validate:

- header and data block ordering;
- program, numeric array, string array, CODE, and SCREEN$ metadata;
- flag matching and checksum;
- LOAD versus VERIFY memory effects;
- pilot, sync, zero, and one pulse timing;
- EAR polarity and sampling edge;
- BREAK handling through the keyboard;
- truncated stream, noise, wrong flag, checksum failure, and end-of-tape behavior;
- exact registers, flags, PC/SP, system variables, and elapsed time on return.

A fast tape trap must be gated by ROM identity and concrete entry bytes. It must preserve failure semantics, interrupt state, border/speaker/tape side effects expected by loaders, and any requested timing mode. Custom loaders must continue through real port/timing emulation.

## Screen, Printer, Channels, and Streams

`PRINT-OUT` at `0x09F4` handles printable characters, tokens, and control codes through shared machinery. `CLS` begins at `0x0D6B`; `CLS-LOWER` at `0x0D6E`; display-address helpers, scroll routines, printer buffer, and editor follow in the same section.

Validate:

- nonlinear bitmap address and linear attribute address;
- OVER, INVERSE, INK, PAPER, FLASH, and BRIGHT;
- AT/TAB and cursor control sequences;
- main screen versus lower editor area;
- scrolling across Spectrum bitmap thirds;
- `DF_CC`, `S_POSN`, `ATTR_P/T`, `MASK_P/T`, `P_FLAG`, and related system variables;
- printer buffer at `0x5B00` and ZX Printer I/O;
- token expansion and recursive print paths.

Initial channels at `0x15AF` describe K, S, R, and P. Streams contain offsets into the channel-information area. Test OPEN/CLOSE and channel selection through system variables rather than hard-wiring only the default screen and keyboard.

ROM screen routines update memory; the ULA determines when changed bytes become visible. Keep firmware memory semantics separate from fetch-time video effects.

## BASIC, Memory, and Calculator

The executive initializes the machine and implements the main edit/syntax/execute loop. BASIC lines are tokenized, and numeric literals include a marker followed by a hidden five-byte numeric form.

Important memory pointers include:

- `CHANS`: channel information;
- `PROG`: BASIC program;
- `VARS`: variables;
- `E_LINE`: edit line;
- `WORKSP`: workspace;
- `STKBOT`/`STKEND`: calculator stack;
- `RAMTOP` and `P_RAMT`: usable and physical memory limits;
- `UDG`: user-defined graphics.

Memory insertion and reclamation routines move dynamic regions and then adjust every affected pointer. Audit overlap direction, stack bounds, out-of-memory errors, and 16-bit wraparound.

The expression evaluator distinguishes syntax checking from execution. Trap or parser code must preserve both paths; accepting a BASIC line is not equivalent to correctly executing it.

Numbers use:

- a five-byte floating-point form with exponent and signed mantissa;
- a special five-byte short-integer representation whose exponent byte is zero;
- a ROM-internal stack language entered through `RST 0x28` and `CALCULATE` at `0x335B`.

The supplied disassembly documents notable original defects, including the lost 34th division bit and inconsistent handling of `-65536`. Do not silently replace original results with host IEEE floating point. Differentially test:

- zero, sign changes, integer boundaries, `-65536`;
- overflow and underflow;
- rounding near mantissa boundaries;
- division corner cases;
- SIN, EXP, LN, and ATN approximation ranges;
- strings and mixed calculator operations;
- exact error reports and stack cleanup.

## Trap Design and Validation

Treat a trap as a replacement implementation of a concrete ROM subgraph.

Before enabling it:

1. Check ROM size and cryptographic hash.
2. Check entry PC and a short byte signature.
3. Check the expected call/jump context if multiple entries share code.
4. Define the synchronization boundary at which state equivalence is compared.

Compare:

- main and alternate registers;
- all flags;
- I, R, IFF1, IFF2, interrupt mode, and HALT;
- PC, SP, machine stack, calculator stack;
- ordered memory writes and system-variable changes;
- complete 16-bit I/O addresses and edge timestamps;
- total T-states including contention;
- success, BREAK, and error exits.

Prefer an optional accuracy mode that runs the real ROM whenever cycle-exact behavior matters. Never intercept custom or modified ROM code merely because the PC matches a standard address.

## Audit Invariants

- The selected standard English ROM is exactly 16K and matches its declared hash.
- IM1 enters `0x0038`; one accepted frame interrupt advances `FRAMES` once and performs one keyboard service.
- EI does not allow another maskable interrupt until after the following instruction.
- HALT waits for an accepted interrupt, not merely a host frame callback.
- Every keyboard row is addressed by the high port byte and keys are active-low.
- SAVE/LOAD block checksums and failure exits match ROM execution.
- `PRINT-OUT` and scrolling preserve bitmap/attribute layout and system-variable state.
- Dynamic memory moves update all pointers that lie beyond the moved region.
- Syntax checking and execution do not produce the same side effects.
- ROM calculator results and documented defects are preserved for the original image.
- Traps self-disable for unknown hashes or mismatched entry signatures.

## Repository Map

- `src/ZXSpectrum/Memory.cpp`: ROM image selection and 16K/48K memory.
- `src/ZXSpectrum/ZXSpectrum.cpp`: CPU/ULA scheduling and device construction.
- `src/ZXSpectrum/PortManager.cpp`: keyboard, tape, border, sound, floating bus.
- `src/ZXSpectrum/FileIO.cpp`: TAP/TZX/snapshot and token-related paths.
- `src/ZXSpectrum/Datasette*.cpp`: cassette signal/data boundary.
- `src/ZXSpectrum/StdPrinter.cpp`: ROM-visible printer behavior and traps.
- `src/ZXSpectrum/ASCIIToCodeConverter.cpp`: host text to Spectrum input/token path.
- `projects/ZXSpectrum/ZXSpectrumSysVars.txt`: local system-variable aid; verify it against the selected ROM.
- `emulators/ZXSpectrumCommons/bios/`: ROM variants used by the implementation.

Comparative local sources:

- `others/fuse-master/roms/48.rom` and Fuse machine/peripheral code.
- `others/EightyOne/EightyOne-Source/Source/ROMs/`.
- `others/JSpeccy-master/src/main/resources/roms/`.

## Source Index

Accessed 2026-07-28:

- Local primary disassembly: `.codex/docs/ZXSpectrumROM.pdf`
- Searchable edition, [The Complete 48K Disassembly](https://speccy.xyz/rom/)
- Sinclair Research, [ZX Spectrum BASIC Programming](https://worldofspectrum.org/ZXBasicManual/)
- Sinclair Research, [Chapter 25: System Variables](https://worldofspectrum.org/ZXBasicManual/zxmanchap25.html)
- Sinclair Research, [Appendix C: Technical Reference](https://worldofspectrum.org/ZXBasicManual/zxmanappc.html)
- Sinclair Research, [ZX Spectrum Service Manual](https://spectrumforeveryone.com/wp-content/uploads/2017/08/ZX-Spectrum-Service-Manual.pdf)
- Zilog, [Z80 CPU User Manual](https://www.zilog.com/docs/z80/um0080.pdf)
- World of Spectrum, [48K ZX Spectrum Technical Information](https://worldofspectrum.org/faq/reference/48kreference.htm)

When sources disagree, verify actual ROM bytes and hardware traces. Record the ROM hash, timing origin, and access date.

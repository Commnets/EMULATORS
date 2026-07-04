---
name: c64-mps801-printer-audit
description: Validate Commodore MPS-801 serial dot matrix printer behavior in EMULATORS, including SerialPrinterMPS801 classes and their MatrixPrinterEmulation and SerialPrinterPeripheralSimulation hierarchy, IEC/KERNAL-trap printer routing, device numbers, secondary addresses, PETSCII/character sets, 6x7 glyphs, double width, graphics mode, reverse field, tab and dot positioning, line/page geometry, PostScript output, and fidelity boundaries. Use when Codex reviews or edits COMMODORE MPS801 printer code, generated printer output, PostScript printer templates, C64/VIC-20/C264 serial-printer behavior, or compares implementation against .codex/docs/MPS801.pdf and historical MPS-801 sources.
---

# C64 MPS-801 Printer Audit

## Core Workflow

Use this skill to audit whether EMULATORS models a Commodore MPS-801 at the intended fidelity level.

1. Read `references/mps801.md` before making claims or edits.
2. Identify the intended behavior tier before judging correctness:
   - Basic text sink: validate printable character filtering, uppercase/lowercase mode, line breaks, double width, output file semantics.
   - PostScript glyph model: validate 6x7 glyph data, graphics bytes, reverse mode, dot positioning, tab positioning, line/page geometry, and required `PSMPS801MatrixPrinter*.ps` files.
   - Serial-printer peripheral: validate KERNAL-trap LISTEN/OPEN/CLOSE/SEND routing, device number behavior, secondary addresses, and active-function lifetime.
   - Hardware/firmware model: only require internal CPU, RAM/ROM, bus timing, buffer, and mechanical behavior if the implementation explicitly claims that tier.
3. Inspect local implementation paths first:
   - `include/COMMODORE/SerialPrinterMPS801.hpp`, `src/COMMODORE/SerialPrinterMPS801.cpp`
   - `include/CORE/MatrixPrinterEmulation.hpp`, `src/CORE/MatrixPrinterEmulation.cpp`
   - `include/COMMODORE/SerialPrinters.hpp`, `src/COMMODORE/SerialPrinters.cpp`
   - `include/COMMODORE/SerialIOPeripherals.hpp`, `src/COMMODORE/SerialIOPeripherals.cpp`
   - `include/COMMODORE/IOPBuilder.hpp`, `src/COMMODORE/IOPBuilder.cpp`
   - `include/C64/StdSerialPrinter.hpp`, `src/C64/StdSerialPrinter.cpp`, and the VIC-20/C264 equivalents when host printer registration matters.
   - `PSMPS801MatrixPrinterI.ps` and `PSMPS801MatrixPrinterII.ps` copies under `projects/`, `setups/`, `emulators/`, and `monitors/` when PostScript rendering is involved.
4. Compare against `.codex/docs/MPS801.pdf` first. Use the external URLs in `references/mps801.md` to verify historical/spec claims when internet access is available or when the user asks for web confirmation.
5. When changing C++ code, also use `$emulators-framework-cpp-style`.
6. When host-side C64 CIA2 IEC wiring, not just KERNAL traps, is involved, also use `$c64-cia-audit`.
7. Report whether each issue is a bug inside the current fidelity tier or a missing higher-fidelity feature.

## Audit Priorities

Prioritize correctness in this order:

- Scope honesty: distinguish an output-file printer simulation from a firmware, timing, or mechanism-accurate MPS-801.
- Device and channel behavior: device selector positions 4/5/T, default printer device use, secondary address `0` for cursor-up graphics/uppercase mode and `7` for cursor-down upper/lowercase mode, and active function reset on LISTEN/CLOSE.
- Character sets: PETSCII to printer-code handling, cursor-up and cursor-down tables, business mode, graphical characters, reverse field, and use of the MPS801 ROM-derived glyph comments in `SerialPrinterMPS801.cpp`.
- Control codes: `0x08` graphics mode, `0x0a` line feed, `0x0e` double width, `0x0f` standard width, `0x10` tab/position setup, `0x11` cursor down, `0x12` reverse on, `0x1a` repeat graphic data, `0x1b` dot address setup, `0x91` cursor up, and `0x92` reverse off.
- Graphics and positioning: 7 vertical printable dots, 480 horizontal dot positions, dot-address limits, tab behavior, graphic-byte interpretation, repeat-graphics behavior, and automatic print behavior at line/buffer limits.
- Page geometry: 80 columns, 6 LPI in character/double-width modes, 9 LPI in graphics mode, 66 lines on 11-inch paper, paper border handling, and PostScript scale constants.
- Basic vs PostScript parity: document any behavior supported only in `MPS801PostscriptMatrixPrinterEmulation` and not in `MPS801BasicMatrixPrinterEmulation`.
- Compatibility limits: do not require 8039 firmware execution, 90-byte line-buffer timing, unidirectional head timing, motor/hammer mechanics, or serial-bus electrical timing unless the implementation claims those features.

## Validation Hints

Prefer focused probes:

- BASIC/KERNAL snippets that `OPEN` the printer on device 4 or 5, choose secondary addresses 0 and 7, send PETSCII uppercase/lowercase text, and `CLOSE` the channel.
- Byte streams that exercise each control code, especially mixed double width, graphics mode, reverse field, tab/dot positioning, and return to standard mode.
- Output-file assertions for basic mode and PostScript token checks for glyph definitions, page setup, `drawByteBits`, page breaks, and missing template-file logging.
- Tests that separate serial-routing failures from matrix-printer rendering failures.
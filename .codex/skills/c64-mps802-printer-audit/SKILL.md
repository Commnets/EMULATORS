---
name: c64-mps802-printer-audit
description: Validate Commodore MPS-802 and 1526 serial dot matrix printer behavior in EMULATORS, including SerialPrinterMPS802 classes and their MatrixPrinterEmulation and SerialPrinterPeripheralSimulation hierarchy, IEC/KERNAL-trap printer routing, device numbers, secondary-address functions, PETSCII/character sets, 8x8 glyphs, enhanced width, reverse and paging modes, formatter strings, programmable characters, line/page geometry, PostScript output, and fidelity boundaries. Use when Codex reviews or edits COMMODORE MPS802 printer code, generated printer output, PostScript printer templates, C64/VIC-20/C264 serial-printer behavior, or compares implementation against .codex/docs/MPS802.pdf, the MPS-802 user guide, and 1526/MPS-802/4023 service information.
---

# C64 MPS-802 Printer Audit

## Core Workflow

Use this skill to audit whether EMULATORS models a Commodore MPS-802/1526 at the intended fidelity level.

1. Read `references/mps802.md` before making claims or edits.
2. Identify the intended behavior tier before judging correctness:
   - Basic text sink: validate printable character filtering, uppercase/lowercase mode, enhanced width, line buffering, formatter definition/use, diagnostic messages, and output file semantics.
   - PostScript glyph model: validate 8x8 glyph data, reverse mode, paging, programmable character, line/page geometry, formatter rendering, and required `PSMPS802MatrixPrinter*.ps` files.
   - Serial-printer peripheral: validate KERNAL-trap LISTEN/OPEN/CLOSE/SEND routing, device number behavior, secondary-address functions, and active-function lifetime.
   - Hardware/firmware model: only require 6504 CPU, RIOT/VIA, ROM, mechanical direction/timing, and bus timing if the implementation explicitly claims that tier.
3. Inspect local implementation paths first:
   - `include/COMMODORE/SerialPrinterMPS802.hpp`, `src/COMMODORE/SerialPrinterMPS802.cpp`
   - `MPS802MatrixPrinterFormatter` nested classes in the same files
   - `include/CORE/MatrixPrinterEmulation.hpp`, `src/CORE/MatrixPrinterEmulation.cpp`
   - `include/COMMODORE/SerialPrinters.hpp`, `src/COMMODORE/SerialPrinters.cpp`
   - `include/COMMODORE/SerialIOPeripherals.hpp`, `src/COMMODORE/SerialIOPeripherals.cpp`
   - `include/COMMODORE/IOPBuilder.hpp`, `src/COMMODORE/IOPBuilder.cpp`
   - `include/C64/StdSerialPrinter.hpp`, `src/C64/StdSerialPrinter.cpp`, and the VIC-20/C264 equivalents when host printer registration matters.
   - `PSMPS802MatrixPrinterI.ps` and `PSMPS802MatrixPrinterII.ps` copies under `projects/`, `setups/`, `emulators/`, and `monitors/` when PostScript rendering is involved.
4. Compare against `.codex/docs/MPS802.pdf` first. Use the external URLs in `references/mps802.md` to verify historical/spec claims when internet access is available or when the user asks for web confirmation.
5. When changing C++ code, also use `$emulators-framework-cpp-style`.
6. When host-side C64 CIA2 IEC wiring, not just KERNAL traps, is involved, also use `$c64-cia-audit`.
7. Report whether each issue is a bug inside the current fidelity tier or a missing higher-fidelity feature.

## Audit Priorities

Prioritize correctness in this order:

- Scope honesty: distinguish an output-file printer simulation from a firmware, timing, or mechanism-accurate MPS-802/1526.
- Device and channel behavior: default device 4, jumper-selectable device range in real hardware, secondary-address function selection, and active function reset on LISTEN/CLOSE.
- Secondary-address functions: `0x00` none, `0x01` use formatter, `0x02` define formatter, `0x03` set lines per page, `0x04` enable format/diagnostic messages, `0x05` define programmable character, `0x06` set line spacing, `0x07` business mode, `0x09` suppress diagnostic messages, and `0x0a` reset.
- Formatter semantics: format-string parsing, text/space/numeric fields, `0x1d` field separation while using a formatter, error strings such as `*PE:F*` and `*.*`, and whether errors are printed only when messages are enabled.
- Character sets and modes: PETSCII to printer-code handling, business mode, graphics/default mode, 8x8 glyph data, enhanced-width enable/disable, uppercase/lowercase control, reverse field, paging on/off, and programmable character use at code `0xfe`.
- Page geometry: 80 columns, 640 horizontal dot positions, 8 vertical dots, 6 LPI text, 9 LPI graphics, 66 lines on 11-inch paper, paper border handling, and PostScript scale constants. Treat any code comment/manual mismatch as an audit target.
- Basic vs PostScript parity: document behavior implemented only in `MPS802PostscriptMatrixPrinterEmulation`, especially paging, programmable character, and line/page features.
- Compatibility limits: do not require 6504 firmware execution, 6532/6522 register behavior, ROM execution, bidirectional head timing, or serial-bus electrical timing unless the implementation claims those features.

## Validation Hints

Prefer focused probes:

- BASIC/KERNAL snippets that `OPEN` the printer on device 4, choose secondary addresses 0, 1, 2, 4, 7, 9, and 10, send data, and `CLOSE` the channel.
- Byte streams that exercise enhanced width, disable enhanced width, reverse on/off, paging on/off, uppercase/lowercase, formatter definition/use, and reset.
- Output-file assertions for basic mode and PostScript token checks for glyph definitions, formatter output, programmable character bytes, page breaks, `drawByteBits`, and missing template-file logging.
- Tests that separate serial-routing failures from matrix-printer rendering or formatter failures.
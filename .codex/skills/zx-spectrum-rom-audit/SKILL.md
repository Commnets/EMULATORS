---
name: zx-spectrum-rom-audit
description: Validate and implement Sinclair ZX Spectrum 48K ROM-visible behavior in the EMULATORS repository, including reset and restart vectors, IM1/NMI handling, keyboard scanning and repeat, beeper and tape routines, screen/printer output, channels and streams, BASIC tokenization and execution, expression evaluation, memory movement, system variables, calculator stack, floating-point formats, ROM traps, localized ROM selection, and firmware/hardware timing contracts. Use when Codex reviews or edits ZX Spectrum ROM loading, KERNAL-style traps, keyboard, tape, screen, printer, BASIC files, tokens, system variables, startup, interrupts, or compatibility tests.
---

# ZX Spectrum ROM Audit

## Core Workflow

Use the 48K ROM as executable specification, while keeping firmware behavior separate from hardware behavior supplied by the ULA and Z80.

1. Read `references/zx-spectrum-rom.md` completely before judging behavior.
2. Inspect `.codex/docs/ZXSpectrumROM.pdf` for the full annotated control flow, register contracts, tables, algorithms, and routine index. Use the PDF-page locator in the reference.
3. Identify and hash the actual ROM loaded by the selected emulator. Do not assume that English, Spanish, Nordic, Arabic, prototype, 128K, or replacement ROMs share entry points or data tables.
4. For the standard English 48K ROM, compare against the canonical 16,384-byte image and verify relevant bytes around every relied-upon entry point.
5. Inspect the local implementation:
   - `include/ZXSpectrum/Memory.hpp`, `src/ZXSpectrum/Memory.cpp`
   - `include/ZXSpectrum/ZXSpectrum.hpp`, `src/ZXSpectrum/ZXSpectrum.cpp`
   - `include/ZXSpectrum/ZXSpectrumEmulator.hpp`, `src/ZXSpectrum/ZXSpectrumEmulator.cpp`
   - `include/ZXSpectrum/PortManager.hpp`, `src/ZXSpectrum/PortManager.cpp`
   - `include/ZXSpectrum/FileIO.hpp`, `src/ZXSpectrum/FileIO.cpp`
   - `include/ZXSpectrum/Datasette*.hpp`, `src/ZXSpectrum/Datasette*.cpp`
   - `include/ZXSpectrum/StdPrinter.hpp`, `src/ZXSpectrum/StdPrinter.cpp`
   - `include/ZXSpectrum/ASCIIToCodeConverter.hpp`, `src/ZXSpectrum/ASCIIToCodeConverter.cpp`
   - Z80 interrupt, HALT, flags, stack, block instruction, and I/O behavior under `include/FZ80` and `src/FZ80`
6. If a trap replaces ROM execution, prove equivalence at entry and exit: registers, flags, alternate registers, stack, PC, memory, system variables, ports, elapsed T-states, interrupts, and error paths.
7. When changing C++ code, also use `$emulators-framework-cpp-style`.

## Audit Priorities

Prioritize:

- Reset at `0x0000`, restarts, IM1 at `0x0038`, NMI at `0x0066`, EI delay, HALT release, and stack effects.
- Keyboard matrix scan, multi-key rejection, shifts, modes, debounce/repeat, `LAST_K`, `FLAGS`, and 50/60 Hz dependence.
- Tape SAVE/LOAD/VERIFY pulse timing, header/data blocks, flags, checksum, BREAK, and failure semantics.
- Screen address calculation, attributes, control codes, scrolling, lower-screen/editor behavior, channels/streams, and printer routing.
- Startup RAM probing, `RAMTOP`/`P_RAMT`, system-variable initialization, memory workspace movement, and 16K/48K differences.
- Token tables, syntax checking versus execution, BASIC program representation, variables, calculator stack, integer short form, five-byte floating point, and documented ROM defects.
- Localized or replacement ROM compatibility only after identifying the concrete image.

## Trap Equivalence Checklist

For every accelerated or intercepted routine:

| Boundary | Verify |
| --- | --- |
| Entry | exact PC(s), call versus jump, input registers/flags, expected ROM image |
| Memory | all reads/writes and ordering, including screen, workspace, stack, and system variables |
| I/O | complete 16-bit port address, edge timing, tape/border/speaker side effects |
| Exit | PC, SP, registers, alternate registers, flags, IFF1/IFF2, error return |
| Time | T-states, contention, interrupt eligibility, HALT behavior |
| Failure | BREAK, checksum mismatch, out-of-memory, invalid parameter, missing device |

Disable the trap automatically when the ROM hash or entry bytes do not match its contract.

## Validation Strategy

Use differential execution when possible:

1. Run the real ROM without traps from a controlled snapshot.
2. Record instruction, bus, port, memory, system-variable, and timing traces.
3. Run the accelerated path from the identical snapshot.
4. Compare all externally observable state at a named synchronization boundary.

Cover reset, held/repeated keys, simultaneous shifts, SAVE/LOAD/VERIFY success and failure, screen controls, scrolling, BASIC editing, numeric edge cases, and 16K versus 48K startup.

## Change Guardrails

- Do not turn descriptive routine labels into a stable ABI across ROM variants.
- Do not shortcut ROM routines by mutating only the final user-visible value.
- Do not hide Z80 flag, IFF, refresh, or contention bugs inside ROM-specific workarounds.
- Do not use host floating point as automatically equivalent to the ROM calculator.
- Do not normalize documented ROM bugs unless the selected ROM is intentionally patched.
- Do not infer firmware behavior from a screenshot when system variables, memory layout, timing, or error paths differ.

## Audit Report

Report the ROM filename, size, hash, language/version, relevant entry bytes, hardware configuration, trap state, and timing convention. For each finding, cite the disassembly routine/address and distinguish:

- ROM requirement;
- ULA/Z80 requirement exposed by the ROM;
- emulator convenience behavior;
- intentional compatibility boundary.

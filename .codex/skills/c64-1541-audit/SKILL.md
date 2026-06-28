---
name: c64-1541-audit
description: Validate Commodore 1540/1541/1541C/1541-II compatible disk-drive behavior in EMULATORS, including IEC serial bus protocol, device numbers, KERNAL traps, CBM DOS commands/status, D64 image parsing, BAM/directory/file chains, PETSCII filenames, 6502 drive CPU/ROM/RAM expectations, 6522 VIA wiring, GCR/track/sector geometry, motor/head timing, and compatibility boundaries for 1541-like devices. Use when Codex reviews or edits C64, VIC-20, C264, COMMODORE disk, serial I/O, D64, CBM DOS, IEC, VIA, GCR, or 1541 drive code.
---

# C64 1541 Audit

## Core Workflow

Use this skill to audit whether EMULATORS models a Commodore 1541-compatible disk drive at the intended fidelity level.

1. Read `references/1541.md` before making claims or edits.
2. Identify the intended behavior tier before judging correctness:
   - KERNAL-trap disk simulation: validate LOAD/SAVE-like workflows, channels, status bytes, D64 parsing, directory and file transfer semantics.
   - DOS/file-system simulation: validate CBM DOS command channel behavior, BAM/directory updates, error/status behavior, file types, wildcards, and disk image mutations.
   - Hardware/cycle drive emulation: validate independent drive CPU, RAM/ROM map, two 6522 VIAs, IEC line timing, GCR bitstream, sync marks, motor/head mechanics, and drive-code upload/execution.
3. Inspect the local implementation paths first:
   - `include/COMMODORE/1540SeriesDisk.hpp`, `src/COMMODORE/1540SeriesDisk.cpp`
   - `include/COMMODORE/SerialIOPeripherals.hpp`, `src/COMMODORE/SerialIOPeripherals.cpp`
   - `include/COMMODORE/SerialPort.hpp`, `src/COMMODORE/SerialPort.cpp`
   - `include/COMMODORE/FileIO.hpp`, `src/COMMODORE/FileIO.cpp`
   - `include/C64/1541Disk.hpp`, `src/C64/1541Disk.cpp`
   - `include/C264/1541Disk.hpp`, `src/C264/1541Disk.cpp`
   - `include/VIC20/1541Disk.hpp`, `src/VIC20/1541Disk.cpp`
   - `include/COMMODORE/VIA/*`, `src/COMMODORE/VIA/*` when auditing hardware-level 6522 behavior.
   - `include/C64/SerialPort.hpp`, `src/C64/SerialPort.cpp` and C64 CIA2 IEC wiring when host-side IEC lines are involved.
4. When changing C++ code, also use `$emulators-framework-cpp-style`.
5. When host-side C64 CIA2 IEC behavior is involved, also use `$c64-cia-audit`.
6. Report whether each issue is a bug inside the current fidelity tier or a missing higher-fidelity feature.

## Audit Priorities

Prioritize correctness in this order:

- Scope honesty: do not describe a KERNAL-trap D64 loader as full 1541 emulation unless it includes the drive CPU, ROM, VIAs, IEC timing, and disk bitstream.
- IEC protocol: LISTEN/TALK/UNLISTEN/UNTALK, OPEN/CLOSE, secondary addresses, device numbers 8-11, channel selection, not-present behavior, EOF/status bits, and daisy-chain assumptions.
- CBM DOS channel 15: power-up status, persistent status string, read-to-OK transition, syntax/file/channel errors, and commands such as `N`, `S`, `C`, `R`, `I`, `V`, block commands, memory commands, and user commands where supported.
- D64 and CBM file system: 35/40/42-track image sizes, per-track sector counts, track 18 BAM and directory, file-chain T/S pointers, last-sector length semantics, 16-byte PETSCII names padded with `$A0`, file type/locked/unclosed flags, wildcard matching, duplicate names, and directory listing format.
- Data mutation: SAVE, scratch, rename, copy, initialize, validate/collect, new/format, BAM updates, directory-sector allocation, write-protect and disk-full behavior.
- Hardware-level behavior if implemented: 1541 6502 at 1 MHz, `$0000-$07FF` RAM, `$C000-$FFFF` DOS ROM, two 6522 VIAs, serial-interface VIA, read/write and motor-control VIA, GCR encode/decode, sync detection, byte-ready timing, spindle speed, half-track stepping, head bump behavior, and LED/motor states.
- Compatibility variants: 1540 timing/ROM differences, 1541C/1541-II ROM and track-0 sensor differences, 1570/1571/1551 compatibility claims, and unsupported copy-protection/nibbler expectations.

## Validation Hints

Prefer focused tests:

- Minimal BASIC/KERNAL programs that exercise `LOAD`, `LOAD"$"`, `OPEN`, `PRINT#`, `INPUT#`, `GET#`, `CLOSE`, and channel 15 status reads.
- D64 fixtures with edge cases: empty disk, full disk, multi-sector files, deleted entries followed by valid entries, unclosed files, locked files, wildcards, duplicate names, 40/42-track images, invalid T/S chains, and missing track 18.
- For hardware-level work, small drive-resident 6502 programs that read/write VIA registers, use timers/interrupts, toggle IEC lines, step the head, and scan for GCR sync.
- Compare final behavior against the local PDF `.codex/docs/1541.pdf` and the source list in `references/1541.md`, not against secondary summaries when they conflict.

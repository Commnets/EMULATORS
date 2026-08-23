---
name: c64-cia-audit
description: Validate Commodore 64 CIA 6526 emulation in EMULATORS, including CIA1/CIA2 $DC00/$DD00 register behavior, ports, DDRs, keyboard/joystick matrix, IEC/user port, VIC bank selection, timers A/B, underflow timing, PB toggle/pulse output, TOD clock/alarm, serial shift register, FLAG, ICR masks and IRQ/NMI lines. Use when Codex reviews or edits C64 CIA, timers, ports, keyboard, joystick, serial bus, interrupts, TOD, or VIC bank control.
---

# C64 CIA Audit

When compiling, testing, or executing on Windows, use `$emulators-windows-build-runtime`.

## Core Workflow

Use this skill to audit whether the two C64 CIA chips behave like 6526 devices in their C64 wiring context.

1. Read `references/cia.md` before making claims or edits.
2. Inspect the local implementation paths first:
   - `include/COMMODORE/CIA/CIA.hpp`, `src/COMMODORE/CIA/CIA.cpp`
   - `include/COMMODORE/CIA/CIARegisters.hpp`, `src/COMMODORE/CIA/CIARegisters.cpp`
   - `include/COMMODORE/CIA/CIATimer.hpp`, `src/COMMODORE/CIA/CIATimer.cpp`
   - `include/COMMODORE/CIA/CIAClock.hpp`, `src/COMMODORE/CIA/CIAClock.cpp`
   - `include/COMMODORE/CIA/CIASerialPort.hpp`, `src/COMMODORE/CIA/CIASerialPort.cpp`
   - `include/C64/CIA1*`, `src/C64/CIA1*`, `include/C64/CIA2*`, `src/C64/CIA2*`
   - `include/C64/SerialPort.hpp`, `src/C64/SerialPort.cpp`
3. When changing C++ code, also use `$emulators-framework-cpp-style`.
4. Separate generic 6526 behavior from C64-specific wiring. CIA1 and CIA2 are the same chip but expose different system effects.
5. For each finding, identify the exact register, bit, read/write side effect, interrupt line, and expected cycle/timer consequence.

## Audit Priorities

Prioritize correctness in this order:

- Port read/write and DDR behavior, including active-low keyboard/joystick/IEC lines.
- CIA1 wiring: keyboard matrix, joystick ports, paddles/control port selection, datasette FLAG, and IRQ output.
- CIA2 wiring: VIC bank selection on PA0/PA1, IEC/user-port/RS-232 lines, and NMI output.
- Timer A/B latch/counter load behavior, underflow reload, one-shot/continuous mode, force-load, CNT counting, and Timer B count sources.
- ICR read-clear behavior and bit 7 set/clear mask semantics.
- TOD BCD clock, alarm selection, 50/60 Hz selection, halt/latch behavior, and AM/PM bit.
- Serial shift register and FLAG edge semantics.

## Validation Hints

Useful local assets:

- `docs/C64Data/CIA1Logic.xlsx`
- `emulators/C64Programs/Code/Macros/MCIAS.asm`
- `emulators/C64Programs/Code/Tests/TESTCIA1.asm`
- `emulators/C64Programs/Code/Tests/TESTCIA1IRQ.asm`
- `emulators/C64Programs/Code/Tests/TESTCIA2NMI.asm`
- `emulators/C64Programs/Code/Tests/TESTKEYBOARD.asm`
- `emulators/C64Programs/Code/Tests/TESTKEYBOARDIRQ.asm`

Timer regressions are easiest to catch with minimal assembly programs that count IRQ/NMI firings or observe PB6/PB7 transitions.

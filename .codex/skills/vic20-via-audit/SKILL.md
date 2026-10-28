---
name: vic20-via-audit
description: Audit MOS 6522 VIA emulation and VIC20 VIA1/VIA2 wiring in EMULATORS, including timers, ports and DDRs, handshake, shift register, IFR/IER, IRQ/NMI, keyboard, joystick, cassette, IEC, and user port. Use for VIC20 VIA code reviews and timing or I/O trace diagnosis.
---

# VIC20 VIA Audit

Audit the generic NMOS 6522 contract separately from the wiring of the two VIC20 VIAs. An audit request does not authorize implementation changes; follow the user's requested scope.

## Read and route

- Read [references/via.md](references/via.md) for registers, timing, pin allocation, and validation cases.
- Read [references/sources.md](references/sources.md) when establishing a hardware requirement, resolving conflicting manuals, or identifying a chip revision. It indexes the supplied VIA datasheet, Mapping the VIC, original manufacturer documentation, and independent emulator implementations.
- For raster effects synchronized by VIA timers, also use `$vic20-vici-audit`. For execution logs, use `$emulators-debug-log-audit` and inspect the actual VIA debug producers before interpreting fields.
- When implementation is authorized, use `$emulators-framework-cpp-style`; when compiling or running locally on Windows, use `$emulators-windows-build-runtime`.

## Repository entry points

Inspect only paths relevant to the question; verify the current implementation rather than treating these references as a description of its present behavior.

- Generic chip: `include/COMMODORE/VIA/` and `src/COMMODORE/VIA/`: `VIA`, `VIARegisters`, `VIATimer`, `VIAPort`, `VIAControlLines`, `VIAShiftRegister`. Some port methods are inline in headers.
- Machine wiring: `include/VIC20/VIA1*`, `src/VIC20/VIA1*`, `include/VIC20/VIA2*`, `src/VIC20/VIA2*`.
- Address decode and construction: `src/VIC20/Memory.cpp`, `include/VIC20/Memory.hpp`, `src/VIC20/VIC20.cpp`.
- Inputs and peripherals: VIC20 `OSIO`, `DatasettePort`, `1530Datasette`, `IOPBuilder`, and the actual IEC/trap path selected by the configuration. Do not equate a KERNAL trap with emulation of electrical bus transitions.
- CPU sampling and bus timing: `src/CORE/CPU.cpp`, `include/CORE/Instruction*`, `src/F6500/IRQInterrupt.cpp`, and `src/F6500/NMIInterrupt.cpp`.

## Audit method

1. Identify VIA1 at `$9110` versus VIA2 at `$9120`, the connected CPU interrupt input, selected 6522 variant, video standard, and active CPU execution mode.
2. Reconstruct configuration from actual writes to DDR, ACR, PCR, timer latches/counters, and IER. Reset defaults and KERNAL initialization are different events.
3. Track output latch, external pins, DDR, input latch, control-line levels/edges, timer counter/latch/phase, IFR flags, IER mask, and output interrupt level separately.
4. Place accesses at their CPU bus cycle. In full-instruction mode, advancing a timer once per recovered cycle is insufficient if a write has already been applied at instruction start. Check dummy writes of 6502 read-modify-write instructions as well as the final write.
5. Separate hardware timeout, assertion of the VIA IRQ output, CPU sampling, and handler entry. Repeated requests while the line remains asserted are not fresh edges; a held VIA1 output must not create repeated NMIs.
6. Produce findings with exact source method, register/bit, configuration, expected transition, observed transition, affected cycles, and evidence/confidence. Distinguish specification discrepancies, implementation defects, trace limitations, and hypotheses.

## Focused verification

Existing assembly assets under `emulators/VIC20Programs/Code/Tests/` include `TESTVIAPORTDDR.asm`, `TESTVIAHANDSHAKE.asm`, `TESTVIAT1PB7.asm`, `TESTVIAT2PHI2.asm`, `TESTVIAT2PB6.asm`, `TESTVIASHIFT.asm`, and `TESTVIA2.asm`. Read their assertions and completion criteria before using them as conformance evidence; their names alone do not establish coverage.

For a timer audit, compare counter reads and IFR/PB7 timing around zero and reload, including small latch values, instead of checking only average interrupt frequency. For visible drift, compare the same register write's raster position over successive frames. Do not encode game-specific compensation or presume that fixing a timer proves all missing graphics are fixed.

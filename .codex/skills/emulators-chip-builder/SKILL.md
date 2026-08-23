---
name: emulators-chip-builder
description: Implement or extend a reusable hardware chip and its machine-specific wiring in EMULATORS, including registers, pins or logical signals, clocks, state machines, memory-mapped behavior, side effects, interrupts, DMA or bus requests, serialization, introspection, and chip tests. Use when adding a controller, timer, PIA, PPI, UART, video or sound device, custom ASIC, register bank, or a variant of an existing chip.
---

# Build an EMULATORS Chip

When compiling, testing, or executing on Windows, use `$emulators-windows-build-runtime`.

## Required Context

1. Read `references/chip-contract.md`.
2. Read `.codex/skills/emulators-new-machine/references/workspace-artifacts.md`. Store completed chip contracts and other code-generation support in the active `.codex/work/` scope, never in `docs/`.
3. Inspect `include/CORE/Chip.hpp`, `include/CORE/ChipRegisters.hpp`, their implementations, and the nearest chip family.
4. Use `$emulators-framework-cpp-style` for C++ changes and `$emulators-fmt-audit` when `InfoStructure` output changes.
5. Use an existing chip-specific audit skill when the target or reused component matches it.

## Workflow

1. Identify the exact chip revision, package-visible contract, clock domains, register map, reset values, side effects, interrupt outputs, and undocumented assumptions.
2. Compare target behavior with existing chips. Reuse only behavior supported by evidence; place board-specific wiring in a derived machine class or adapter.
3. Fill the chip contract before implementation. Resolve read/write asymmetry, write-one actions, read-clear behavior, open bits, mirrors, latches, and event ordering.
4. Implement state and register semantics before connecting the machine. Keep register objects responsible for register-visible behavior and the chip responsible for wider state progression, following the nearest framework pattern.
5. Implement clocked state machines with explicit transition points. Preserve subcycle ordering when it affects bus access, interrupts, audio, or pixels.
6. Model signals and interrupts as emulated state, not host callbacks with hidden timing.
7. Add focused tests for reset, every register operation, boundary transitions, interrupt acknowledgement, simultaneous events, and variant differences.
8. Add a small machine-wiring test after isolated chip tests pass.

## Fidelity Rules

- Distinguish not implemented, approximated, and verified behavior.
- Avoid speculative pin-level complexity when the requested accuracy cannot observe it, but retain extension points for known future requirements.
- Avoid per-cycle allocation and repeated lookup in hot paths.
- Never infer register semantics from power-on values or names alone.
- Do not hide board polarity, address decoding, or interrupt routing inside a generic chip.

## Completion Evidence

Report the revision modeled, generic and machine-specific classes, register coverage, timing granularity, tests run, external references used, and all approximations or unknowns.

---
name: emulators-new-machine
description: Plan, scaffold, implement, and integrate a complete new emulated computer or console in the EMULATORS repository. Use when Codex creates a new machine, platform, hardware model, or emulator executable; decides which existing CPUs, chips, memory, I/O, video, sound, ROM, and framework classes to reuse or extend; or coordinates several emulator subsystems into a bootable tested system.
---

# Build a New EMULATORS Machine

When compiling, testing, or executing on Windows, use `$emulators-windows-build-runtime`.

## Core Workflow

1. Read `references/workspace-artifacts.md`, then read `references/machine-contract.md`. When the scope spans more than one subsystem, copy `assets/machine-spec.yaml` into a task-specific directory under `.codex/work/emulators/<machine>/<model-or-variant>/<scope>/`; never use `docs/` for this working material.
2. Read repository instructions and use `$emulators-framework-cpp-style` for every C++ change.
3. Establish the target model, regional variants, clocks, accuracy target, authoritative documentation, ROM/media availability, and observable acceptance criteria. Mark unknown facts; do not silently invent hardware behavior.
4. Inventory the nearest implementations under `include/`, `src/`, `projects/`, `emulators/`, and `tests/`. Compare behavior and wiring, not only class names.
5. Classify every component as `reuse`, `configure`, `derive`, `adapt`, or `new`. Record the evidence and compatibility boundary in the machine specification.
6. Draw the clock, bus, memory, I/O, interrupt, and ownership relationships before scaffolding classes. Resolve contradictory clock units or address domains early.
7. Route specialist work:
   - Use `$emulators-cpu-builder` for a new or changed CPU, instruction set, interrupt model, or CPU timing.
   - Use `$emulators-machine-core-builder` for machine classes, memory maps, wiring, model selection, executables, and project integration.
   - Use `$emulators-chip-builder` for a reusable hardware chip or register block.
   - Use `$emulators-io-builder` for ports, buses, host input, media, and external peripherals.
   - Use `$emulators-system-test-builder` to design cross-component and conformance tests.
   - Use an existing machine- or chip-specific audit skill whenever its trigger matches a reused component.
8. Implement vertical slices in dependency order: reset and memory access, CPU execution, interrupts and timing, essential I/O, video/sound, media/peripherals, tooling.
9. Integrate each slice into the appropriate Visual Studio project and compile the narrowest affected target before continuing.
10. Close the delivery gates in `references/machine-contract.md`; report implemented behavior, evidence, tests, accuracy limits, and deferred work.

## Coordination Rules

- Apply `references/workspace-artifacts.md` to every plan, specification, contract, workbook, source registry, validation matrix, generator input, and intermediate artifact created by this workflow or a routed specialist skill.
- Keep generic hardware in the lowest suitable reusable namespace and machine wiring in the machine namespace.
- Prefer composition or explicit adapters when inheritance would imply compatibility the hardware does not possess.
- Preserve a single timing vocabulary. State whether a value is in master clocks, CPU cycles, chip cycles, raster cycles, or host time.
- Keep the emulator deterministic below host adapters. Never make emulated state depend directly on wall-clock timing or nondeterministic host polling.
- Do not claim that a machine is complete merely because it compiles or reaches its reset vector.
- Do not broaden the task into unrelated framework modernization.

## Completion Evidence

Provide the final component classification and machine specification, created and reused class graph, exact build and test commands, boot or behavioral evidence, and a concise list of unsupported or approximated behavior.

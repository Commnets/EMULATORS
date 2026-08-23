---
name: emulators-machine-core-builder
description: Create and integrate the machine-level core of a new EMULATORS emulator, including Computer and Emulator derivatives, memory and port maps, chip ownership and wiring, clock coordination, model configuration, commands, formatters, executables, and Visual Studio projects. Use when scaffolding a machine namespace, adding a model or regional variant, assembling existing components into a computer, or making a new emulator build and start.
---

# Build an EMULATORS Machine Core

When compiling, testing, or executing on Windows, use `$emulators-windows-build-runtime`.

## Required Context

1. Read `references/machine-integration.md`.
2. Read `.codex/skills/emulators-new-machine/references/workspace-artifacts.md`. Keep integration plans and other code-generation support in the active `.codex/work/` scope, never in `docs/`.
3. Read the active machine specification from `$emulators-new-machine`; if none exists, capture the relevant fields before editing.
4. Inspect `include/CORE/Computer.hpp`, `include/EMULATORS/Emulator.hpp`, their implementations, and at least two nearby complete machines.
5. Use `$emulators-framework-cpp-style` for C++ changes and `$emulators-fmt-audit` whenever visible `InfoStructure` fields or formatter contracts change.

## Workflow

1. Define machine models, configuration keys, clocks, address spaces, reset behavior, ownership, and executable boundaries.
2. Select the nearest architectural patterns separately for Computer, Emulator, Memory, PortManager, Screen, Sound, CommandBuilder, and executable. Do not clone one whole machine by resemblance alone.
3. Create the smallest namespace and class surface needed for a reset-capable vertical slice.
4. Implement memory and port decoding with explicit priority, mirroring, open-bus or unmapped behavior, ROM/RAM overlays, and model-dependent regions.
5. Construct components in ownership-safe order; connect buses, interrupts, events, screen, sound, and peripherals only after their contracts exist.
6. Express clock ratios exactly and identify the synchronization owner. Avoid conversions through host time in emulation paths.
7. Add model and regional configuration without duplicating common behavior.
8. Register commands, formatters, project files, solution dependencies, runtime resources, and emulator entry points by following the nearest maintained pattern.
9. Build the narrowest library and executable targets. Exercise reset, a bounded execution interval, and one observable machine behavior.
10. Hand cross-component scenarios to `$emulators-system-test-builder`.

## Architectural Boundaries

- Keep reusable CPU and chip behavior outside the machine namespace.
- Keep board wiring, address decoding, clock ratios, ROM selection, and model quirks inside the machine namespace.
- Keep host UI, SDL, filesystem, and command-line concerns above deterministic emulated devices.
- Make ownership explicit and verify destruction order.
- Preserve unrelated project filters and configurations when editing Visual Studio metadata.

## Completion Evidence

Report the implemented class graph, address and port maps, clocks, interrupt routes, build targets, smoke-test result, formatter changes, and intentionally deferred devices.

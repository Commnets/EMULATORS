---
name: emulators-system-test-builder
description: Design and implement unit, conformance, integration, timing, boot, trace, video, audio, and regression tests for EMULATORS processors, chips, peripherals, and complete machines. Use when creating a verification strategy for a new emulator, converting hardware documentation into tests, adding diagnostic programs or golden artifacts, measuring behavioral coverage, or proving that components interact correctly.
---

# Build EMULATORS System Tests

When compiling, testing, or executing on Windows, use `$emulators-windows-build-runtime`.

## Required Context

1. Read `references/validation-matrix.md`.
2. Read `.codex/skills/emulators-new-machine/references/workspace-artifacts.md`. Store working validation matrices, acquisition state, and intermediate evidence in the active `.codex/work/` scope or `.codex/tmp/<task>/` as appropriate, never in `docs/`.
3. Inspect the implementation contract, authoritative hardware sources, nearby tests under `tests/`, and diagnostic programs under `emulators/*Programs/`.
4. Use `$emulators-framework-cpp-style` for C++ tests. Use `$emulators-debug-log-audit` when instruction/cycle traces are part of the evidence.
5. Use the relevant CPU, chip, I/O, memory-map, video, sound, or system audit skill when its trigger matches.

## Workflow

1. Define observable claims before choosing the test mechanism. Map every important claim to a source, stimulus, expected observation, tolerance, and test layer.
2. Build the cheapest reliable layer first: unit, component, integration, diagnostic program, then boot or application test.
3. Derive expectations independently from documentation, known vectors, captured reference traces, or manually justified calculations. Never duplicate the implementation formula as the oracle.
4. Make time explicit. Record sampling points, preconditions, cycles elapsed, intermediate events, and final state for timing-sensitive tests.
5. Control nondeterminism: seed randomness, replace host time, provide fixed input streams, isolate filesystem state, and define audio/video tolerances.
6. Test boundaries and negative paths before declaring coverage complete.
7. Add regression tests for every confirmed defect when a stable observable exists.
8. Build and run the narrowest target, then the relevant integration target. Preserve raw failure evidence when a discrepancy remains.

## Evidence Standards

- Prefer exact state or ordered trace assertions over screenshots and log substring matching.
- Use golden video or audio only with documented format, capture window, tolerance, and update policy.
- Distinguish structural opcode/register coverage from behavioral and timing coverage.
- Treat a passing boot as a smoke test, not proof of component accuracy.
- Mark unavailable ROMs, proprietary suites, hardware captures, or external vectors as evidence gaps.

## Completion Evidence

Report the validation matrix, new tests and assets, commands and results, coverage by behavior class, environmental dependencies, unresolved mismatches, and fidelity claims the evidence actually supports.

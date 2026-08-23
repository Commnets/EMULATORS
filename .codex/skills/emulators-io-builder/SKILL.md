---
name: emulators-io-builder
description: Implement machine input/output, I/O port decoding, external buses, host input adapters, storage and media devices, and peripherals in EMULATORS. Use when adding keyboard matrices, joysticks, controllers, cassette, cartridge, disk, printer, serial or parallel interfaces, protocol state machines, PortManager behavior, or deterministic host-to-emulated-device integration.
---

# Build EMULATORS I/O

When compiling, testing, or executing on Windows, use `$emulators-windows-build-runtime`.

## Required Context

1. Read `references/io-contract.md`.
2. Read `.codex/skills/emulators-new-machine/references/workspace-artifacts.md`. Store completed I/O contracts, protocol plans, and other code-generation support in the active `.codex/work/` scope, never in `docs/`.
3. Inspect the nearest `PortManager`, input device, peripheral simulation, media handler, and host adapter independently.
4. Read the active machine specification from `$emulators-new-machine` when working on a new system.
5. Use `$emulators-framework-cpp-style` for C++ changes. Use matching disk, printer, CIA, PPI, or system audit skills when available.

## Layering Model

Keep machine interface, deterministic device simulation, media format, and host adapter as separate layers. Do not let host events mutate CPU-visible state without passing through the emulated interface and its sampling rules.

## Workflow

1. Define addresses or selectors, line polarity, direction, pull-up/floating behavior, reset state, handshakes, sampling edges, clocks, interrupts, errors, and disconnect behavior.
2. Determine whether the interface, device, protocol, or media format already exists. Reuse each layer independently; use adapters for different board wiring.
3. Specify the protocol state machine and observable transitions before implementing it.
4. Implement port decoding and device state with deterministic inputs. Queue or latch host input according to emulated sampling time.
5. Validate contention, simultaneous drivers, absent devices, short transfers, aborts, timeouts, EOF, write protection, corrupt media, and hot-plug behavior when applicable.
6. Keep file parsing and media mutation bounded against malformed sizes, chains, and counts.
7. Add isolated tests for decoding and protocol transitions, then a machine-level program or command test.
8. Verify that save/load, reset, and reconnection do not leave stale host or protocol state.

## Completion Evidence

Report implemented layers, port or bus map, timing and polarity assumptions, supported devices/media, negative-path tests, host dependencies, and unsupported protocol behavior.

# Machine Integration Checklist

## Pattern Inventory

Inspect the nearest examples for each concern rather than copying one machine wholesale:

- Framework contracts in `include/CORE/Computer.hpp` and `include/EMULATORS/Emulator.hpp`.
- Machine classes under `include/C64`, `C264`, `VIC20`, `ZX81`, `ZXSpectrum`, and `MSX`.
- Implementations under matching `src/` directories.
- Static libraries under `projects/`, executables under `emulators/`, and tests under `tests/`.
- Diagnostics under `emulators/*Programs/`.

## Construction and Ownership

List every object, owner, borrower, construction dependency, reset order, and destruction order. Keep the Computer responsible for the emulated board and the Emulator responsible for configuration and application-level assembly, following local patterns. Prevent observers from outliving publishers and model switching from retaining stale state.

## Maps and Wiring

State address width, masks, priority, mirroring, aliases, overlays, banking, write-through, and unmapped reads. Distinguish CPU-visible, video/DMA-visible, and physical storage views. Record interrupt source, destination, polarity, priority, mask, acknowledgement, and sampling domain. Record every clock as an exact ratio and name its advancement owner.

## Repository Integration

- Add headers and implementations using local directory and namespace conventions.
- Update `.vcxproj` and `.vcxproj.filters` without disturbing other configurations.
- Update solution dependencies only when a new project is required.
- Register commands through the effective CommandBuilder chain.
- Synchronize `.fmt` files with `InfoStructure` producers.
- Add ROM, configuration, and formatter paths using the nearest maintained convention.

## Minimum Smoke Sequence

Construct a known model, reset twice and compare deterministic state, read the reset vector or initial source, execute a bounded interval without host input, observe one expected machine transition, destroy cleanly, and repeat construction.


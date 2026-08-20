# Machine Contract

Use the contract to keep subsystem decisions consistent. Keep unknown values explicit and attach a source or rationale to every hardware-sensitive decision.

## Identity and Scope

- Machine, manufacturer, release, exact models, regions, and board or chip revisions.
- Requested deliverable and fidelity: functional, instruction-accurate, cycle-aware, cycle-exact, or signal-level for named interactions.
- Available ROMs, software, diagnostics, schematics, manuals, traces, and physical-hardware observations.
- Excluded models and explicitly deferred subsystems.

## Timing

- Master oscillator and derived clocks as exact ratios where possible.
- CPU, video, audio, peripheral, and real-time clock domains.
- Synchronization owner and conversion rules.
- Interrupt sampling, DMA/bus arbitration, wait states, contention, and reset duration.

## Address and Signal Domains

- CPU-visible memory map, overlays, mirrors, bank switching, and unmapped/open-bus behavior.
- Device-visible address spaces and DMA visibility.
- I/O port map, selector masks, aliases, and read/write asymmetry.
- Interrupt sources, priorities, polarity, acknowledgement, and destinations.
- External buses and host-facing interfaces.

## Component Decision Record

For each CPU, chip, memory device, interface, and peripheral record role, exact variant, existing candidate, decision (`reuse`, `configure`, `derive`, `adapt`, or `new`), evidence, owning namespace, validation method, and status.

## Delivery Gates

1. Architecture: every component, clock, address region, and interrupt route is classified.
2. Reset slice: the executable builds, resets deterministically, and performs documented first accesses.
3. CPU slice: essential execution and interrupts pass processor tests.
4. Board slice: memory, ports, clocks, and mandatory chips interact through documented wiring.
5. I/O slice: required user input and boot media work without host-time leakage.
6. Observable slice: required video, sound, console, or peripheral output is testable.
7. Release candidate: builds and tests pass; unsupported behavior and fidelity limits are documented.


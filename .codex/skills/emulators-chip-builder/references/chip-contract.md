# Chip Contract

## Identity and Timing

Record part number, revision, compatible variants, authoritative sources, generic behavior versus board wiring, requested fidelity, clocks, asynchronous inputs, reset duration, internal state, simultaneous-event ordering, and interrupt timing.

## Register Ledger

For every register and alias record address/select, masks and mirrors, read/write asymmetry, reset state, bit meaning and polarity, read side effects, write actions, event timing, emitted signals, variant differences, and tests.

## Integration Boundary

- Put intrinsic register and state-machine behavior in the generic chip.
- Put board address decoding outside the chip when it belongs to the machine.
- Put keyboard matrices, bank selection, chip selects, polarity, and interrupt wiring in machine-specific derivatives or adapters.
- Expose logical signals without coupling the reusable chip directly to one Computer class.

## Acceptance Gates

1. Classify all decoded accesses, mirrors, and gaps.
2. Test reset and register side effects.
3. Cover interrupt assertion, masking, acknowledgement, and deassertion.
4. Test every clock boundary visible at the requested fidelity.
5. Keep generic and board-specific responsibilities visibly separate.


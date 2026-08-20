# Validation Matrix

Create one row per observable claim with: requirement, authoritative source, preconditions, stimulus, observation, independent oracle, timing and tolerance, test layer, automation target/assets, and status.

## Coverage Dimensions

Measure encoding/register coverage, semantic transitions, boundaries and negative paths, conditional timing, interrupts and simultaneous events, model/revision differences, cross-component wiring, and software/peripheral compatibility separately. Do not collapse these into one percentage unless denominator and weighting are explicit.

## Oracle Hierarchy

Prefer authoritative vectors or hardware traces, direct calculations from specifications, differential comparison with independent trusted implementations, and documented golden artifacts. Use existing emulator output only as regression evidence, never as sole proof of hardware correctness.

## Timing Record

Include initial state, cycle zero, ordered intermediate events, sampling boundary, final state, clock domain, and tolerance. Preserve the earliest divergent event after failure.

## Golden Artifact Policy

Record producer, model, ROM, input, warm-up duration, capture window, dimensions or sample format, normalization, tolerance, and regeneration command. Review golden updates as behavioral changes; never overwrite them automatically after failure.


# Processor Workbook

Complete the applicable sections before bulk implementation. Use one row per opcode encoding, including prefix bytes or mode selectors.

## Architecture

- Exact processor variants and differences.
- Address, data, I/O, and internal register widths; endianness.
- Register file, aliases, flags, reserved bits, and reset state.
- Stack organization and wrapping rules.
- Run, halt, wait, stop, bus hold, and reset states.
- Interrupt sources, priorities, vectors, masks, entry and return sequences, and sampling boundaries.
- Clock phases, bus transactions, wait states, refresh, and arbitration.

## Opcode Ledger

For each encoding record all bytes and prefixes, canonical mnemonic and aliases, availability by variant, operands, length, semantics, flags, ordered bus trace, base and conditional timing, interrupt boundary, and test identifiers. Classify undocumented and illegal encodings explicitly.

## Test Partitions

- Zero, one, maximum, sign boundary, carry/borrow boundary, and alternating bits.
- Lowest/highest address, page or bank crossing, unaligned access, and wraparound.
- Taken/not-taken branches and every conditional-cycle cause.
- Stack boundaries and nested interrupts.
- Prefix collisions, illegal encodings, and truncated streams.
- Interrupt arrival before, during, and after sampling points.
- Reset or bus request during multi-access instructions when applicable.

## Acceptance Gates

1. Every encoding has an explicit classification.
2. Every supported instruction has independent semantic tests.
3. Every timing branch has a test or recorded evidence gap.
4. Reset and every interrupt mode have ordered-state or bus-trace tests.
5. Disassembly and execution agree on length and operand decoding.
6. Variant differences cannot silently select base-family behavior.


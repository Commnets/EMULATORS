---
name: emulators-cpu-builder
description: Implement or extend a CPU architecture, processor variant, instruction set, addressing modes, registers, interrupts, bus transactions, disassembly, timing, and processor tests in EMULATORS. Use when adding a processor not yet emulated, deriving a CPU from F6500, FZ80, or another family, filling missing opcodes, correcting instruction semantics or cycles, or creating exhaustive instruction and interrupt tests.
---

# Build an EMULATORS CPU

When compiling, testing, or executing on Windows, use `$emulators-windows-build-runtime`.

## Required Context

1. Read `references/processor-workbook.md` before designing instructions or tests.
2. Read `.codex/skills/emulators-new-machine/references/workspace-artifacts.md`. Store the completed processor workbook and other code-generation support in the active `.codex/work/` scope, never in `docs/`.
3. Inspect `include/CORE/CPU*`, `include/CORE/Instruction.hpp`, their implementations, and the closest CPU family under `include/` and `src/`.
4. Inspect `TestCPU`, `Test6500`, `TestZ80`, `TestInst6500`, or `TestInstZ80` patterns when relevant.
5. Use `$emulators-framework-cpp-style` for all C++ work.

## Workflow

1. Establish authoritative manuals, exact processor revision, clock/bus contract, reset state, interrupt modes, and treatment of undocumented behavior.
2. Compare the target with existing CPU families at register, opcode, addressing, bus, interrupt, and timing levels. Choose independently among reuse, configuration, derivation, adapter, and new implementation.
3. Complete the processor workbook before bulk-generating instruction classes. Give every opcode a deliberate status: implemented, alias, undocumented, illegal, model-specific, or unsupported.
4. Implement architecture and registers first, then reset and interrupts, addressing primitives, instruction families, decode/disassembly, transactions, and timing.
5. Preserve observable bus ordering and conditional cycles. Treat correct final registers with incorrect accesses or timing as a failure whenever the accuracy target exposes them.
6. Separate invariant family behavior from variant-specific behavior. Do not put machine wiring into a reusable CPU namespace.
7. Add tests alongside each instruction family. Build expected results from the specification, not from the implementation under test.
8. Add boundary and interaction tests after opcode coverage: page/address wrapping, stack limits, interrupt entry/return, reset, wait/halt states, prefix decoding, illegal instructions, self-modifying code, and overlapping bus requests as applicable.
9. Compile and run the narrowest processor tests, then at least one machine-level smoke test when integration exists.

## Instruction Acceptance

For every supported opcode verify decode length and operands, state and flag results, address rules, ordered bus accesses, base and conditional cycles, interrupt boundaries, disassembly, and supported variant differences.

## Completion Evidence

Report opcode and variant coverage, commands run, failing or unavailable external vectors, timing granularity achieved, undocumented behavior policy, and remaining gaps. Do not describe a processor as complete while opcodes or interrupt states remain implicitly unclassified.

# EMULATORS debug-log format

## Record hierarchy

- A line containing only `+` terminates one emulator execution block.
- A block normally begins with one CPU record and contains chip cycles produced for the CPU time consumed.
- Component headers have the form `COMPONENT absolute-cycle record-kind`; indented lines belong to the preceding header.

## Field provenance

- CPU: `src/CORE/CPU.cpp`, especially `debugLastExecutionData` and interrupt/stop methods.
- VIC-II: `src/COMMODORE/VICII/VICII.cpp`, especially `debugVICIICycle` and event methods.
- CIA: `src/COMMODORE/CIA/CIA.cpp`, especially `debugCIACycle`; timer tuples originate in CIA timer classes.

Re-read these methods whenever the schema may have changed.

## Timing checks

For consecutive instruction blocks, compare the current block chip-cycle count with `Last N cycles` in the following CPU block. The same-block values normally describe different instructions. Reset this comparison across stopped or interrupt-entry blocks. Check absolute chip sequences for gaps/duplicates and exclude `Additional Info` records.

For VIC-II, verify cycle/row wrap; DEN latch at `$30`; badline condition; BA request; CPU stop; c-access; RC reset; cycle-58 state; and the effective timing of CPU accesses to `$d011/$d012/$d019/$d01a`.

The VIC-II snapshot is emitted before `treatBadLineStateAtCurrentCycle` and the remaining per-cycle phases. Read an event appended to that record as occurring during the recorded cycle, with its state visible in the following snapshot. VIC-II memory-access events belong to the VIC phase of that cycle. A following `VIC-II register write` event belongs to the CPU phase of the same absolute cycle, after the VIC-II bus activity and before final pixel/IRQ processing. Because the CPU normally runs a complete instruction before chips catch up, treat any write without that explicit event as ambiguous within the recovered block.

For interrupts, record source/reason, request cycle, wait/rejection, launch, entry PC and acknowledgement. Separate a held level from multiple edges and include BA/RDY delays.

## Useful commands

```powershell
python .codex/skills/emulators-debug-log-audit/scripts/analyze_log.py emulators/example.LOG --summary
python .codex/skills/emulators-debug-log-audit/scripts/analyze_log.py emulators/example.LOG --match 'PC:\$0bd|\$d011|Interrupt' --before 1 --after 2
```

## CIA tuple decoding

From debugCIACycle: port tuples are output latch, DDR, effective port value. Timer tuples are enabled, count mode, initial/latch value, current counter value. If a multi-cycle CPU block shows the timer decrement applied all at once and then repeated values, inspect whether the timer uses the final CPU clock for every recovered CIA cycle.


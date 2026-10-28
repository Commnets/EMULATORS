# 6522 and VIC20 audit reference

## Contents

- Register contract and access side effects
- Timers and interrupt timing
- Ports, handshake, serial transfer
- VIC20 wiring and address decoding
- Validation matrix and evidence limits

Use the source locators in [sources.md](sources.md). The supplied preliminary November 1977 datasheet is not interchangeable with every production NMOS or CMOS VIA. Explicitly identify a disagreement instead of silently combining incompatible behaviors.

## Register contract

Offsets are relative to `$9110` (VIA1) or `$9120` (VIA2). CPU reads, CPU writes, and debugger peeks are distinct operations.

| Offset | Register | Read contract | Write contract |
|---|---|---|---|
| `$0` | ORB/IRB | Resolve B output/input bits and input latch; applicable B control flags acknowledged | Update B output latch; applicable handshake/acknowledgement |
| `$1` | ORA/IRA | Resolve A bits/latch; applicable A control flags acknowledged | Update A output latch; applicable handshake/acknowledgement |
| `$2` | DDRB | B direction bits | 1 selects output, 0 input |
| `$3` | DDRA | A direction bits | 1 selects output, 0 input |
| `$4` | T1C-L | Counter low; clear IFR6 | Set low latch, not live counter |
| `$5` | T1C-H | Counter high | Set high latch, transfer latch to counter, clear IFR6, arm/start T1 |
| `$6` | T1L-L | Low latch | Set low latch without restarting live count |
| `$7` | T1L-H | High latch | Set high latch and clear IFR6; do not restart live count |
| `$8` | T2C-L/T2L-L | Counter low; clear IFR5 | Set low latch |
| `$9` | T2C-H | Counter high | Set high byte, transfer low latch, clear IFR5, arm/start T2 |
| `$A` | SR | Shift register; acknowledge SR flag and apply selected transfer-mode effects | Load SR; acknowledge/restart transfer as mode specifies |
| `$B` | ACR | Auxiliary control | Select latching, SR, T2 source, T1 modes |
| `$C` | PCR | Peripheral control | Select control-line edges/modes |
| `$D` | IFR | Individual flags plus computed bit 7; reading IFR does not clear it | Write ones to clear selected flags 0..6; bit 7 is not a clear-all command |
| `$E` | IER | Enable mask; bit-7 readback is revision-sensitive in the sources | Bit 7 = 1 sets selected enables; bit 7 = 0 clears selected enables; zero bits leave enables unchanged |
| `$F` | ORA without handshake | Same A data resolution, without the normal `$1` handshake/flag side effects | Update A latch without the normal `$1` handshake/flag side effects |

Do not swap `$4/$5` counter reads with `$6/$7` latch reads; some published maps mislabel these. Reading a low timer byte is not automatically permission to invent a coherent high-byte snapshot. Check the target's actual read semantics and software read order.

### ACR and IFR decoding

| ACR bits | Meaning |
|---|---|
| 0, 1 | Input latching for A, B respectively, captured by the associated active C1 edge |
| 4..2 | SR mode: 000 disabled; 001 input/T2; 010 input/PHI2; 011 input/external CB1; 100 output/free-running T2; 101 output/T2; 110 output/PHI2; 111 output/external CB1 |
| 5 | T2: 0 PHI2 interval timer; 1 PB6 negative-pulse counter |
| 7..6 | T1: 00 one-shot/no PB7; 01 continuous/no PB7; 10 one-shot/PB7 pulse; 11 continuous/PB7 square wave |

IFR bits 0..6 are CA2, CA1, SR, CB2, CB1, T2, T1. IFR7 is `((IFR & IER & $7F) != 0)`, not a stored event bit. A source can set its flag while masked. Enabling an already pending source can assert the output immediately; disabling an enable does not erase the flag. Clearing one source cannot deassert the output if another enabled flag remains pending.

## Timers: inspect the cycle sequence, not only the arithmetic

T1 has a 16-bit latch and counter. Writing the low latch alone must not start it. Writing T1C-H transfers both halves and arms the timeout. Latch-only changes affect subsequent reloads, not the active countdown.

For continuous T1, the steady timeout-to-timeout period is `N + 2` PHI2 cycles. The raw visible progression around reload includes zero and underflow (`$FFFF`), rather than jumping directly from 1 to the latch. Separate this period from the first write-to-timeout interval: the supplied datasheet's Figure 6 marks that first interval as `N + 1.5` cycles using its drawn PHI2 reference edges. A whole-cycle emulator must document which edge its timestamps represent before comparing absolute cycles.

Do not add two to the software-visible latch or counter as a substitute for representing the timing. That can fix frequency while breaking counter reads and stable-IRQ compensation. Validate load, countdown, zero/underflow, reload, IFR, and PB7 on a single timeline. A load or latch write during a pending phase requires an explicit ordering rule grounded in the target variant.

One-shot timeout arming is separate from the interrupt flag. Clearing the flag need not arm another timeout; the counter can keep changing after its first timeout. The 1977 text and later measured T1 post-timeout behavior differ in details: identify the intended hardware before enforcing a post-timeout sequence. Do not impose T1's continuous reload policy on T2.

T2 interval mode generates one timeout per counter load and continues counting after timeout. T2 pulse mode counts sampled high-to-low transitions on PB6, not a sustained low level, both edges, or each `simulate()` call. Sampling and edge consumption must occur once in a defined PHI2 phase, even if port resolution is invoked twice per recovered cycle. T2's low-byte timing also participates in SR modes; its full 16-bit IRQ timeout is not interchangeable with the low-byte shift clock.

With T1 PB7 enabled, audit its override of ordinary ORB/DDR behavior against the actual variant. The supplied NMOS datasheet explicitly notes that ACR7 can make PB7 an output even when DDRB7 is clear. A load drives the one-shot pulse low; timeout restores high; free-run toggles on timeouts. ORB writes must not overwrite the timer's output state.

### Timing diagnostic example

For PAL non-interlaced video, `71 * 312 = 22152` CPU cycles per frame. T1 latch `$5686` is 22150, so its continuous period must be 22152. A model producing 22150 advances a raster-timed effect by two CPU cycles per frame. At four native high-resolution dots per CPU cycle, that is eight dots per frame; a renderer doubling horizontal coordinates reports sixteen internal positions. This is a diagnostic invariant, not a PITFALL-specific hardware rule or proof that a particular object must be visible.

## Ports, control lines, and SR

Keep ORA/ORB latches, DDRA/DDRB, live external inputs, latched inputs, and effective output pins separate. For ordinary non-overridden pins, start from `(outputLatch & DDR) | (input & ~DDR)`, then apply the documented port differences and alternate functions. Writing an input-configured bit updates the latch for a later direction change without driving that pin now. A DDR write may itself change an external pin and create a peripheral-visible edge. Do not force all externally unconnected pins low or infer the external bus level from an output latch alone.

Input latching uses active CA1/CB1 transitions when enabled by ACR. It is not a snapshot taken anew on every CPU read. NMOS A/B readback and handshake differences need their own checks; do not reuse a CIA port model without verification.

PCR0/4 choose CA1/CB1 negative (0) or positive (1) active edge. PCR3..1 and PCR7..5 choose CA2/CB2 modes:

| Mode | C2 behavior |
|---|---|
| 000 / 010 | Negative / positive edge input; normal port acknowledgement |
| 001 / 011 | Negative / positive independent interrupt input; ordinary port access does not acknowledge its C2 flag |
| 100 | Handshake output: drive low on the specified port access, restore on active C1 edge |
| 101 | Pulse output: low pulse lasting one PHI2 cycle |
| 110 / 111 | Manual low / high output |

Do not blindly make A and B read/write strobes symmetric. In the supplied NMOS specification, A supports automatic read handshaking; B write handshake is tied to ORB writes. Check flag acknowledgement separately from the handshake strobe. SR use of CB1/CB2 takes priority as specified by its mode, without destroying the PCR configuration needed when SR is disabled again.

For SR audit all eight modes, bit order (MSB first out, serial input shifted into bit 0), exact eight-shift completion, IFR2, restart on SR accesses, and CB1/CB2 directions/clock edges. Mode 100 recirculates output continuously; do not assume the normal eight-bit interrupt behavior. Validate T2-divider behavior independently from PHI2 and external modes. Original NMOS external-clock SR limitations are a variant issue; a W65C22 CMOS datasheet cannot establish that an NMOS bug is absent.

## VIC20 board wiring

| Device | Pins | VIC20 connection |
|---|---|---|
| VIA1 `$9110` | IRQ output | CPU NMI input; active-low transition handling |
| VIA1 | PA0, PA1 | IEC clock input, data input |
| VIA1 | PA2, PA3, PA4 | Joystick up, down, left (active low) |
| VIA1 | PA5 | Joystick fire / light-pen line, also shared with user port |
| VIA1 | PA6 | Cassette switch sense |
| VIA1 | PA7 | IEC ATN output through board inversion |
| VIA1 | PB0..PB7, CB1, CB2 | User port; software RS-232 use is a firmware convention |
| VIA1 | CA1, CA2 | RESTORE key input, cassette motor control |
| VIA2 `$9120` | IRQ output | CPU IRQ input, level-sensitive |
| VIA2 | PA0..PA7 | Keyboard rows |
| VIA2 | PB0..PB7 | Keyboard columns; PB3 also cassette write, PB7 also joystick right |
| VIA2 | CA1 | Cassette read input |
| VIA2 | CA2, CB1, CB2 | IEC clock output, SRQ input, data output |

The 6522 pin is called IRQ even on the VIA wired to the CPU's NMI. Some books invert or misdescribe these CPU destinations. RESTORE is not an ordinary key in the row/column matrix. A held NMI output cannot be treated as a new NMI each emulated cycle; clearing all enabled flags releases it and permits a later falling edge.

Keyboard matrix scans must honor DDRs, selected columns, multiple pressed keys, and shared joystick lines. Define whether ghosting/electrical contention is modeled before claiming full fidelity. IEC outputs pass through inverters/open-collector drivers: separate a VIA logical output from the resolved active-low bus and preserve other devices' ability to pull it low. Cassette sense, motor, read, and write are four different signals.

Inspect partial address decoding outside canonical ranges. In particular, simultaneous VIA selection is possible in decoding schemes; a modulo-16 register mirror alone is not a sufficient model. Require the selected board schematic for exact aliases, read-bus resolution, and side effects. Mapping the VIC warns about `$9130-$93FF`; that warning is not a chip-select equation or evidence that the region is plain RAM.

## Validation matrix

| Area | Cases and observable requirement |
|---|---|
| Ports | All input/output/mixed DDRs; write while input then change DDR; external changes while latched; `$1` versus `$F`; independent C2 mode |
| T1 | Latches 0, 1, 2, `$00FF`, `$0100`, `$FFFF`; continuous period `N+2`; counter reads at zero/underflow/reload; new latch mid-count; new load near timeout; PB7 with DDRB7 0 and 1 |
| T2 | PHI2 and PB6; constant-low PB6 versus discrete pulses; post-timeout reads; rearm versus flag clear; SR clock sharing |
| IFR/IER | Masked source then enable; multiple pending sources; write-one-clear; IFR read non-destructive; IER set/clear masks; IER7 revision conflict |
| PCR/SR | Each PCR C2 mode and all SR modes; pulse width independent of host polling; CB1 ownership; eight-bit completion and external-edge boundaries |
| CPU boundary | IRQ masked/unmasked; NMI held/released/reasserted; 6502 RMW dummy/final writes; chip accesses at instruction bus cycles |
| System | Keyboard/joystick sharing; tape motor versus read; IEC resolved levels; mirrors and simultaneous selects; cold reset versus KERNAL initialization |

Report missing observability explicitly. The current VIA deep trace may contain ports without timer latch/counter/IFR/IER snapshots. Handler cadence can prove a period mismatch, but it cannot by itself establish every internal phase or which source first asserted the line.

# C64 CIA Reference

## Sources

- Local PDF: `.codex/docs/CIA.pdf` (MOS 6526 datasheet, 8 pages). Text extraction found timer, TOD, interrupt, ICR, CRA/CRB, shift, port, latch, and alarm material.
- Web: C64-Wiki CIA: https://www.c64-wiki.com/wiki/CIA
- Datasheet link to verify manually if needed: https://www.6502.org/documents/datasheets/mos/mos_6526_cia.pdf
- Local docs/tests: `docs/C64Data/CIA1Logic.xlsx`, `emulators/C64Programs/Code/Tests/TESTCIA*.asm`.

## Register Map

CIA1 is at $DC00-$DCFF and CIA2 is at $DD00-$DDFF. Each chip has 16 registers mirrored every 16 bytes.

Offsets:

- $0 PRA: port A data.
- $1 PRB: port B data.
- $2 DDRA: port A data direction.
- $3 DDRB: port B data direction.
- $4/$5: Timer A low/high.
- $6/$7: Timer B low/high.
- $8: TOD tenths.
- $9: TOD seconds.
- $A: TOD minutes.
- $B: TOD hours and AM/PM.
- $C: serial data register.
- $D: interrupt control/status.
- $E: control register A.
- $F: control register B.

## C64 Wiring

CIA1:

- Address range $DC00-$DCFF.
- IRQ output goes to CPU IRQ.
- Port A/Port B scan keyboard matrix and joystick ports.
- Joystick inputs are active-low.
- PA bits also select paddles/control port lines.
- FLAG is connected to cassette/serial-related inputs depending C64 wiring.

CIA2:

- Address range $DD00-$DDFF.
- IRQ output goes to CPU NMI.
- PA0/PA1 select VIC bank with inverted encoding: %11=$0000, %10=$4000, %01=$8000, %00=$C000.
- PA3..PA5 drive IEC ATN/CLOCK/DATA outputs with active-low bus semantics.
- PA6..PA7 read IEC CLOCK/DATA inputs.
- Port B maps user port and RS-232 lines.

Keep generic CIA behavior in COMMODORE classes and C64-specific wiring in C64 CIA1/CIA2 classes where the architecture allows it.

## Ports and DDR

For each port bit:

- DDR bit 0 means input.
- DDR bit 1 means output.
- Reads combine output latch, external input, pullups/open-collector effects, and active-low attached devices.
- Writes update the port latch even for input bits; if the DDR later changes to output, the latched value becomes visible.

Do not model keyboard or joystick as direct byte replacement without preserving DDR/latch behavior.

## Timers

Each CIA has Timer A and Timer B:

- 16-bit latch plus 16-bit counter.
- Writes to low/high timer registers update the latch.
- If a timer is stopped, writing the high byte also loads the counter from the latch.
- CRA/CRB bit 0 starts/stops the timer.
- Bit 3 selects continuous reload versus one-shot stop after underflow.
- Bit 4 force-loads the counter from latch.
- Timer A can count system cycles or CNT positive edges.
- Timer B can count system cycles, CNT positive edges, Timer A underflows, or Timer A underflows while CNT is high.
- PB6/PB7 can pulse or toggle on Timer A/B underflow depending control bits.

Audit underflow timing carefully: IRQ/NMI flag setting, reload/stop, PB output change, and next decrement order are common off-by-one sources.

## Interrupt Control Register

Reading ICR:

- Bits 0..4 report Timer A, Timer B, TOD alarm, serial register, and FLAG sources.
- Bit 7 reports whether an enabled source is currently causing IRQ/NMI.
- Reading clears reported interrupt flags.

Writing ICR:

- Bits 0..4 select mask bits to modify.
- Bit 7 is the source/set-clear bit: 1 sets selected mask bits, 0 clears selected mask bits.
- Writing with no low bits set should not change masks.

CIA1's interrupt output is IRQ. CIA2's interrupt output is NMI.

## TOD Clock

TOD registers are BCD:

- Tenths: low nibble 0..9.
- Seconds/minutes: ones 0..9, tens 0..5.
- Hours: BCD plus AM/PM bit.

CRA bit 7 selects 60 Hz or 50 Hz TOD input expectation. CRB bit 7 selects whether writes to TOD registers set the clock or the alarm.

The datasheet/C64 behavior includes latch/stop side effects when reading or writing TOD registers. Do not implement TOD as four independent ordinary registers.

## Serial and FLAG

The serial data register shifts on CNT edges through SP. Direction and source are controlled by CRA. Complete byte transfer can set the serial interrupt flag. FLAG detects an edge and can generate an interrupt if enabled.

Even if full serial hardware is not implemented, preserve register flags and safe no-op behavior so software polling does not break.

## EMULATORS Review Checklist

- Are CIA registers mirrored every 16 bytes in $DC00/$DD00 pages?
- Do port reads honor DDR, latches, external inputs, active-low devices, and pullups?
- Does CIA2 PA0/PA1 update the VIC bank mapping through the C64 memory/VIC path?
- Do Timer A/B operations match latch, force-load, one-shot, continuous, PB output, and interrupt side effects?
- Does ICR read clear flags but not masks, and does write bit 7 set/clear masks?
- Does TOD implement BCD, alarm selection, 50/60 Hz, latch/stop behavior, and AM/PM?
- Are CIA1 IRQ and CIA2 NMI routed to the CPU as separate lines?

# MSX Sound Reference

## Sources

- Local PDF: `.codex/docs/MSXTechNotes.pdf` (MSX I/O notes; text extraction is not reliable).
- Web: MSX Assembly Page, MSX I/O ports overview: https://map.grauw.nl/resources/msx_io_ports.php
- Web: MSX Wiki, General Instrument AY-3-8910: https://www.msx.org/wiki/General_Instrument_AY-3-8910
- Web: MSX Wiki, Yamaha YM2149: https://www.msx.org/wiki/Yamaha_YM2149
- Web: General Instrument AY-3-8910 datasheet mirror: https://map.grauw.nl/resources/sound/generalinstrument_ay-3-8910.pdf
- Web: MSX Wiki, System variables and work area: https://www.msx.org/wiki/System_variables_and_work_area

## Scope

The baseline MSX PSG is compatible with General Instrument AY-3-8910. Many MSX machines use compatible chips or the Yamaha YM2149, often integrated in an MSX engine.

The PSG has two roles in MSX:

- Sound: three tone channels, one shared noise generator, and one envelope generator.
- GPIO: joystick, cassette input, keyboard layout bit, trigger outputs, port selection, and Kana LED through registers 14 and 15.

Do not audit PSG as a sound-only chip. Games and BIOS routines often depend on register 14/15 side effects.

## Ports #A0-#A2

MSX PSG ports:

| Port | Direction | Function |
| --- | --- | --- |
| #A0 | write | Select PSG register |
| #A1 | write | Write value to selected PSG register |
| #A2 | read | Read value from selected PSG register |

The selected register is a latch. Writes to #A1 and reads from #A2 use the current selected register. Register numbers should be constrained to 0..15.

Avoid coupling PSG register selection to CPU memory or to unrelated port aliases unless a specific extension is being implemented.

## Register Map

| Register | Function |
| --- | --- |
| 0 | Channel A tone fine period |
| 1 | Channel A tone coarse period, low 4 bits |
| 2 | Channel B tone fine period |
| 3 | Channel B tone coarse period, low 4 bits |
| 4 | Channel C tone fine period |
| 5 | Channel C tone coarse period, low 4 bits |
| 6 | Noise period, low 5 bits |
| 7 | Mixer and I/O direction |
| 8 | Channel A amplitude |
| 9 | Channel B amplitude |
| 10 | Channel C amplitude |
| 11 | Envelope period low byte |
| 12 | Envelope period high byte |
| 13 | Envelope shape/control |
| 14 | I/O port A, MSX joystick/cassette input |
| 15 | I/O port B, MSX joystick outputs/select/Kana LED |

Mask writes. Upper unused bits must not become part of period or amplitude calculations.

## Tone, Noise, and Envelope

Tone channels:

- Each tone period is 12 bits from fine and coarse registers.
- Period 0 is commonly treated as period 1.
- Tone frequency is derived from the PSG clock and the period divider; keep the PSG clock model-specific.

Noise:

- Noise period uses 5 bits.
- Period 0 is commonly treated as period 1.
- The noise generator is shared by all channels and gated through mixer bits.

Envelope:

- Envelope period is 16 bits from registers 11 and 12.
- Period 0 should be handled deliberately, usually as the minimum period.
- Writing register 13 selects shape and restarts the envelope sequence.
- Shape bits encode continue, attack, alternate, and hold behavior.
- Amplitude registers 8-10 use bit 4 to select envelope control for that channel; bits 0-3 are fixed volume when envelope is disabled.

YM2149 differs from AY-3-8910 in analog/output details, especially envelope/volume smoothness. Keep chip model selection explicit if the emulator wants to distinguish them.

## Mixer Register 7

Register 7 bits:

| Bit | Function |
| --- | --- |
| 0 | Disable tone on channel A when 1 |
| 1 | Disable tone on channel B when 1 |
| 2 | Disable tone on channel C when 1 |
| 3 | Disable noise on channel A when 1 |
| 4 | Disable noise on channel B when 1 |
| 5 | Disable noise on channel C when 1 |
| 6 | I/O port A direction in MSX usage; keep 0 |
| 7 | I/O port B direction in MSX usage; keep 1 |

On MSX, the safest rule is to preserve bits 6 and 7 as `0` and `1` respectively unless deliberately emulating software that violates the standard. Some real machines can be damaged by wrong PSG I/O direction usage, so the emulator should at least make the behavior explicit.

## MSX GPIO Register 14

PSG register 14 is port A and is read-only in normal MSX usage:

| Bit | Meaning |
| --- | --- |
| 0 | Joystick input pin 1, 0 = up |
| 1 | Joystick input pin 2, 0 = down |
| 2 | Joystick input pin 3, 0 = left |
| 3 | Joystick input pin 4, 0 = right |
| 4 | Joystick input pin 6, 0 = trigger A |
| 5 | Joystick input pin 7, 0 = trigger B |
| 6 | Japanese keyboard layout bit, 1 = JIS, 0 = other layout |
| 7 | Cassette input signal |

Register 15 bit 6 selects which joystick port is presented through register 14.

## MSX GPIO Register 15

PSG register 15 is port B and is write/read in normal MSX usage:

| Bit | Meaning |
| --- | --- |
| 0 | Joystick port 1 pin 6 output; set 1 to allow input |
| 1 | Joystick port 1 pin 7 output; set 1 to allow input |
| 2 | Joystick port 2 pin 6 output; set 1 to allow input |
| 3 | Joystick port 2 pin 7 output; set 1 to allow input |
| 4 | Joystick port 1 pin 8 output |
| 5 | Joystick port 2 pin 8 output |
| 6 | Joystick input selection for register 14, 1 = port 2 |
| 7 | Kana LED control, 1 = off |

GPIO reads should combine host input state, output latch state, and active-low MSX wiring. Do not compute joystick state independently in both the PSG and the system input layer.

## BIOS and BASIC PLAY

BASIC PLAY and BIOS sound routines use PSG registers, timer interrupts, and work queues:

- Initialization should set mixer direction bits to MSX-safe values.
- Interrupt-driven PLAY queues update channel registers over time.
- Register writes through BIOS and direct port writes should converge on the same PSG state.

If notes hang, envelope restarts are wrong, or joystick reads break after sound code runs, inspect register 7 and registers 14/15 first.

## EMULATORS Review Checklist

- Does `PSGPortManager` maintain a selected-register latch for #A0/#A1/#A2?
- Do `AY38910Registers` mask unsupported bits and keep debugger peeks separate from CPU-visible reads?
- Does `AY38910SimpleLibWrapper` treat period 0 and envelope restart consistently?
- Does the mixer combine tone and noise using disable bits, not enable bits?
- Are register 14 and 15 wired to MSX joystick/cassette/Kana behavior?
- Does register 7 preserve or explicitly handle MSX I/O direction bits 6 and 7?
- Does `MSX::SoundSystem` use the same chip clock/sample-rate assumptions as the PSG wrapper?
- Are AY and YM differences documented if the model lets the user select a chip variant?

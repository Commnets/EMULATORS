# MSX System Reference

## Sources

- Local PDF: `.codex/docs/MSXBIOS.pdf` (scanned BIOS reference; text extraction is not reliable).
- Local PDF: `.codex/docs/MSXTechNotes.pdf` (MSX memory and I/O notes; text extraction is not reliable).
- Local PDF: `.codex/docs/P8255A.pdf` (Intel 8255A PPI reference; text extraction is not reliable).
- Web: MSX Assembly Page, MSX I/O ports overview: https://map.grauw.nl/resources/msx_io_ports.php
- Web: MSX Assembly Page, MSX BIOS calls: https://map.grauw.nl/resources/msxbios.php
- Web: MSX Wiki, Programmable Peripheral Interface: https://www.msx.org/wiki/Programmable_Peripheral_Interface
- Web: MSX Wiki, System variables and work area: https://www.msx.org/wiki/System_variables_and_work_area
- Web: MSX Wiki, Slots: https://www.msx.org/wiki/Slots

## System Shape

An MSX machine is a Z80 computer with memory selected through slots, most hardware reached through Z80 I/O ports, and a BIOS/BASIC ROM environment that expects precise port and interrupt behavior.

Core MSX1 devices:

- Z80 CPU at the standard MSX clock family.
- VDP from the TMS9918A/TMS9928A/TMS9929A family, with separate VRAM and CPU access through ports #98/#99.
- PSG compatible with AY-3-8910 or YM2149, reached through ports #A0-#A2 and also used for joystick/cassette/Kana wiring.
- PPI compatible with Intel 8255, reached through ports #A8-#AB and used for primary slot selection, keyboard row reads, cassette output, CAPS LED, and key click.
- BIOS, BASIC, cartridge ROMs, RAM, and optional extensions arranged through primary and secondary slots.

Do not hardcode one machine layout as "the MSX layout". Models differ in ROM language, keyboard matrix, cartridge slot placement, RAM size, primary/secondary slot expansion, video standard, and optional devices.

## Standard I/O Ports

Important ports for baseline MSX auditing:

| Port | Function |
| --- | --- |
| #90-#91 | Printer interface on many machines |
| #98 | VDP VRAM data read/write |
| #99 write | VDP register write or VRAM address setup |
| #99 read | VDP status register |
| #9A-#9B | V9938/V9958-only ports; not TMS9918A |
| #A0 write | PSG register select |
| #A1 write | PSG data write |
| #A2 read | PSG data read |
| #A8 read/write | PPI port A, primary slot selection |
| #A9 read | PPI port B, selected keyboard matrix row |
| #AA read/write | PPI port C, keyboard row/cassette/CAPS/click |
| #AB write | PPI control command register |
| #FC-#FF write | Memory mapper segment registers, when mapper RAM is present |

Audit the port manager for exact dispatch and for unmapped-port behavior. Z80 has a 16-bit I/O address bus, but many MSX devices decode only the low 8 bits; only use high-byte decode when a device specification requires it.

## PPI 8255 in MSX

MSX uses an 8255-style PPI in a fixed wiring:

- Port A at #A8 is the primary slot select register. It is readable and writable.
- Port B at #A9 reads one keyboard matrix row.
- Port C at #AA selects the keyboard row and controls cassette/CAPS/click lines.
- Control at #AB is write-only and is normally used in bit set/reset mode for port C bits.

Primary slot register #A8:

| Bits | Page |
| --- | --- |
| 0-1 | #0000-#3FFF |
| 2-3 | #4000-#7FFF |
| 4-5 | #8000-#BFFF |
| 6-7 | #C000-#FFFF |

PPI port C #AA:

| Bit | Function |
| --- | --- |
| 0-3 | Keyboard matrix row selected for #A9 reads |
| 4 | Cassette motor control, 1 = off |
| 5 | Cassette write signal, 1 = high |
| 6 | CAPS LED, 1 = off |
| 7 | Key click output |

PPI control #AB in bit set/reset mode:

- Bit 7 must be 0 for BSR mode.
- Bits 1-3 select the port C bit.
- Bit 0 is the value to set.
- Bits 4-6 are unused for the MSX command.

For generic 8255 behavior, remember that mode 0 outputs are latched and inputs are buffered. Avoid implementing #AB as a full I/O mode reconfiguration unless the emulator intentionally supports generic 8255 modes; the MSX standard path uses BSR for port C bit updates.

## Keyboard, Cassette, Joystick

The keyboard matrix is model/language dependent, but the access pattern is standard:

1. Write the row number in PPI port C bits 0-3 through #AA or #AB.
2. Read PPI port B at #A9.
3. Treat pressed keys as active-low bits unless a model-specific matrix document says otherwise.

The PSG GPIO ports cover joystick and cassette input:

- PSG register 14, port A, is read-only in MSX usage and exposes joystick direction/buttons, keyboard layout, and cassette input.
- PSG register 15, port B, is read/write in MSX usage and drives joystick pin outputs, joystick port select, and Kana LED.

Keep joystick, keyboard, and cassette state in one coherent input model. A key scan through PPI and a joystick scan through PSG should not duplicate or contradict host input state.

## BIOS-Visible Behavior

The BIOS exposes routines and work areas that assume hardware side effects:

- Slot routines: RDSLT #000C, WRSLT #0014, CALSLT #001C, ENASLT #0024.
- VDP routines: WRTVDP #0047, RDVRM #004A, WRTVRM #004D, SETRD #0050, SETWRT #0053, FILVRM #0056, LDIRMV #0059, LDIRVM #005C, CHGMOD #005F.
- PSG routines include initialization and register access paths used by BASIC PLAY and sound statements.
- Interrupt hooks such as the timer/VBlank hook are expected to be called by the BIOS interrupt path after the VDP asserts INT.
- System variables mirror device state, including VDP register shadows, screen table addresses, slot tables, keyboard state, and PLAY queues.

If the emulator boots real BIOS ROMs, device side effects must be good enough for BIOS routines rather than only direct emulator UI commands.

## Reset and Model Configuration

Audit reset in layers:

- CPU reset vector and interrupt flip-flop state.
- ROM visibility and initial slot selection.
- PPI reset values and port directions.
- VDP registers, VRAM contents if modeled, status flags, and interrupt line.
- PSG selected register, mixer defaults, and output state.
- Keyboard/joystick/cassette host state.

Model definitions should own machine-specific choices. Avoid scattering per-model keyboard rows, BIOS filenames, slot expansion, RAM size, and PAL/NTSC selection across unrelated classes.

## EMULATORS Review Checklist

- Does `MSX::MSXComputer` wire chips and port managers once, in a model-driven way?
- Does `MSX::PortManager` decode the standard ports and leave future MSX2/extension ports isolated?
- Does `MSX::PPI8255` notify memory only when #A8 changes, and update port C bits without corrupting keyboard/cassette state?
- Does keyboard input flow through PPI #AA/#A9 and remain language/model aware?
- Does joystick/cassette input flow through PSG GPIO, not through ad hoc reads elsewhere?
- Does the VDP interrupt line reach the Z80 and BIOS-visible timer path?
- Are BIOS ROMs, RAM, and cartridges placed by the memory/slot subsystem rather than special-cased in CPU reads?

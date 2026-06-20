# C64 Memory Map Reference

## Sources

- Local PDF: `.codex/docs/MappingC64.pdf` (292 pages). Text extraction found relevant sections for VIC, SID, CIA, $D000, $D400, $D800, $DC00, $DD00, ROM/RAM banking, and color RAM.
- Local PDF: `.codex/docs/VICII.pdf` (Christian Bauer VIC-II article, 52 pages), especially memory as seen by 6510/VIC.
- Web: C64-Wiki Memory Map: https://www.c64-wiki.com/wiki/Memory_Map
- Web: C64-Wiki Bank Switching: https://www.c64-wiki.com/wiki/Bank_Switching
- Web: Christian Bauer VIC-II article: https://www.cebix.net/VIC-Article.txt

## CPU Address Space

Default no-cartridge startup mode is equivalent to all CPU control lines high: LORAM=1, HIRAM=1, CHAREN=1. The CPU sees RAM in most ranges, BASIC ROM at $A000-$BFFF, I/O at $D000-$DFFF, and KERNAL ROM at $E000-$FFFF.

Important ranges:

- $0000: 6510 port data direction register.
- $0001: 6510 port data register. Bits 0..2 drive LORAM, HIRAM, CHAREN only when the corresponding DDR bits are outputs.
- $0400-$07FF: default screen RAM.
- $A000-$BFFF: BASIC ROM or underlying RAM/cartridge depending PLA mode.
- $C000-$CFFF: normally RAM.
- $D000-$DFFF: I/O, character ROM, or RAM depending CHAREN/HIRAM/LORAM/PLA/cartridge mode.
- $E000-$FFFF: KERNAL ROM, RAM, or cartridge ROM depending PLA mode.

If ROM is visible and the CPU writes to that address range, the write updates underlying RAM while reads still return ROM. Emulation must keep RAM contents coherent under visible ROM.

## $D000-$DFFF I/O Decode

With I/O visible:

- $D000-$D3FF: VIC-II registers, 47 implemented registers mirrored every 64 bytes; unused $D02F-$D03F read as $FF and ignore writes.
- $D400-$D7FF: SID registers, internally decoded by low 5 address bits and therefore mirrored every 32 bytes through the SID range.
- $D800-$DBFF: color RAM, 1024 x 4-bit. CPU writes store only low nibble. High nibble on reads is open-bus/model-dependent, not a stable stored value.
- $DC00-$DCFF: CIA1, 16 registers mirrored every 16 bytes.
- $DD00-$DDFF: CIA2, 16 registers mirrored every 16 bytes.
- $DE00-$DEFF: IO1 expansion range.
- $DF00-$DFFF: IO2 expansion range.

When CHAREN=0 and the PLA mode still selects the character ROM, $D000-$DFFF exposes character ROM to the CPU instead of I/O. In RAM-only modes, CHAREN has no practical I/O/char effect because RAM is selected.

## Bank Switching Rules

The PLA combines CPU port bits LORAM/HIRAM/CHAREN with cartridge GAME/EXROM lines. Do not reduce banking to only the three CPU bits if cartridge support is in scope.

No-cartridge CPU modes to remember:

| HIRAM | LORAM | CHAREN | $A000-$BFFF | $D000-$DFFF | $E000-$FFFF |
| --- | --- | --- | --- | --- | --- |
| 1 | 1 | 1 | BASIC ROM | I/O | KERNAL ROM |
| 1 | 1 | 0 | BASIC ROM | Char ROM | KERNAL ROM |
| 1 | 0 | 1 | RAM | I/O | KERNAL ROM |
| 1 | 0 | 0 | RAM | Char ROM | KERNAL ROM |
| 0 | 1 | 1 | RAM | I/O | RAM |
| 0 | 1 | 0 | RAM | Char ROM | RAM |
| 0 | 0 | x | RAM | RAM | RAM |

Cartridge modes add ROML at $8000-$9FFF, ROMH at $A000-$BFFF or $E000-$FFFF depending GAME/EXROM, and open/unmapped Ultimax-style areas. Treat open regions as bus behavior, not as ordinary RAM.

## VIC Memory View

The VIC-II has a 14-bit address bus and sees one 16 KB bank at a time. CIA2 port A bits 0..1 provide the missing address bits, inverted:

| CIA2 PA1..PA0 | VIC bank | CPU address range |
| --- | --- | --- |
| %11 | bank 0 | $0000-$3FFF |
| %10 | bank 1 | $4000-$7FFF |
| %01 | bank 2 | $8000-$BFFF |
| %00 | bank 3 | $C000-$FFFF |

The VIC sees character ROM at VIC-local $1000-$1FFF in banks 0 and 2. From the CPU address-space diagram this corresponds to physical $1000-$1FFF and $9000-$9FFF. Do not confuse CPU-visible CHAREN with VIC-visible char ROM.

Color RAM is not located in the VIC 16 KB bank like ordinary RAM. It is connected to the upper 4 bits of the VIC data bus and addressed by the lower 10 VIC address bits. It is available to VIC fetches independently of the selected bank.

## Address-Calculation Checklist

- Apply CPU address decode only for CPU reads/writes.
- Apply VIC bank selection for VIC fetches, then apply $D018 video matrix/char/bitmap pointers inside that 16 KB bank.
- Treat color RAM as 4-bit side memory on VIC c-accesses and as CPU-visible 4-bit memory at $D800-$DBFF when I/O is visible.
- Preserve underlying RAM on writes through visible ROM.
- Mirror registers exactly by the chip's decoded address bits.
- Keep open-bus/high-nibble behavior isolated so deterministic code does not accidentally rely on a stable value unless the emulator intentionally models bus residue.

## EMULATORS Review Checklist

- Confirm `C64::PLA` owns the decode decision rather than scattering PLA truth tables across memory devices.
- Confirm `C64::Memory` can answer both CPU and VIC access requests without using the wrong view.
- Confirm `C64::ColorMemory` stores low nibbles and documents/implements high-nibble read behavior.
- Confirm CIA2 PA0/PA1 changes immediately affect VIC bank selection at the appropriate emulated time.
- Confirm cartridge and expansion port classes participate in $8000/$A000/$DE00/$DF00/$E000 decode without breaking no-cartridge modes.

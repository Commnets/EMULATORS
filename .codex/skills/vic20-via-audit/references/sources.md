# VIA sources and conflict policy

Consulted 2026-10-27. Local PDF page numbers are one-based viewer pages. Preserve original documents; these notes are an audit guide, not a replacement manual.

## Supplied documents

- [VIA.pdf](VIA.pdf): MOS Technology, *MCS6522 Versatile Interface Adapter*, preliminary data sheet, November 1977, 24 PDF pages. PDF pp. 4-6 cover ports, handshake and T1 access; pp. 7-8 include Figures 5-7 (one-shot, continuous T1, T2); pp. 9-12 cover SR; pp. 12-16 cover interrupts/PCR/ACR; later pages contain electrical and bus timing. Figure 6 on PDF p. 8 was visually inspected: initial `N+1.5`, repeated `N+2` cycles. Original supplied path: `C:/Temp/VIA.pdf`.
- [MappingVIC20.pdf](../../vic20-vici-audit/references/MappingVIC20.pdf): Russ Davies, *Mapping the VIC*, COMPUTE! Publications, 1984, 452 PDF pages. The single preserved copy is shared by these two skills. Printed chapter 6 pp. 143-176 (PDF pp. 165-198) covers both VIAs; printed pp. 179-180 (PDF pp. 201-202) discuss I/O reflections and color RAM. Printed pp. 75-103 cover page-2/page-3 firmware state and vectors; use the exact ROM version when correlating them with traces. Original supplied path: `C:/Temp/MappingVIC20.pdf`.
- The paired VIC-I skill preserves [VICI.pdf](../../vic20-vici-audit/references/VICI.pdf). Use it when testing a timer against raster timing, not as a VIA specification.

## Internet primary documents and direct implementation evidence

- [MOS/Commodore 6522 production data sheet, 1982 catalog scan](https://www.retrodocs.fr/wp-content/uploads/pdf/MOS-6522.pdf): original manufacturer content, 16 PDF pages. Figure 6 documents the repeated timer period. Use to cross-check preliminary prose and register effects.
- [WDC W65C22 data sheet](https://www.westerndesigncenter.com/wdc/documentation/w65c22.pdf): CMOS comparison, especially T1 free-run timing, register tables, and variant caveats. It is not the controlling specification for an NMOS 6522 without a justified compatibility comparison.
- [Commodore VIC20 Programmer's Reference Guide, etext 1.1](https://www.zimmers.net/anonftp/pub/cbm/vic20/manuals/vic20-programmers-reference-guide-1.1.txt): original Commodore guide transcribed by contributors; search `6522`, `VERSATILE`, `9110`, `9120`, `USER PORT`, `KEYBOARD`. OCR/transcription errors require checking a scan for disputed facts.
- [Original Commodore VIC20/VC20 schematics archive](https://www.zimmers.net/anonftp/pub/cbm/schematics/computers/vic20/): choose the actual board/revision before resolving decode aliases or pin polarity. Includes early PAL and NTSC/CR drawings; the archive itself cautions that some early drawings contain errors. This index is a locator, not evidence that every drawing was checked.
- [MAME VIC20 machine implementation](https://github.com/mamedev/mame/blob/master/src/mame/commodore/vic20.cpp): inspect `via1_pa_r`, `via1_pa_w`, `via2_pa_r`, port callbacks and interrupt configuration. Independent implementation evidence for wiring; not a manufacturer specification.
- [VICE generic VIA core](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/core/viacore.c): useful search terms `FULL_CYCLE_2`, `t1reload`, `t1zero`, `VIA_IER`, `IS_CA2_INDINPUT`, and `do_shiftregister`. Includes timing diagrams, measured deviations from manuals, and unresolved comments. Do not treat all comments or historical notes as proven behavior.
- [VICE VIA1 machine wiring](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/vic20/vic20via1.c) and [VIA2 wiring](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/vic20/vic20via2.c): NMI versus IRQ, shared pins, keyboard and tape callbacks. Check the actual function, not a loose VIA-number label in a general manual.
- [MOS/Synertek chip-document archive](https://www.zimmers.net/anonftp/pub/cbm/documents/chipdata/index.html): locator for `6522.zip`, `6522-VIA.txt`, and `sy6522.zip`; the latter is relevant to original external-clock SR limitations. Retrieve and inspect the referenced scan before claiming a particular erratum is reproduced.

Online source links using `main` or `master` are mutable. Before a future audit relies on implementation-specific behavior, resolve the commit revision and record it in that audit; these links do not pin a golden reference.

## Known conflicts that must survive an audit

1. **IER bit 7 readback:** supplied 1977 PDF p. 13 says zero. WDC and common production emulator behavior use one. Identify the target NMOS revision/production evidence; do not silently choose either because it matches the current code.
2. **NMI destination:** Mapping the VIC's introductory VIA1 text (printed p. 144) calls its destination the CPU IRQ; other passages also mix IRQ/NMI wording. The board wiring and independent machine implementations place VIA1 on NMI and VIA2 on IRQ. Keep the distinction between the VIA IRQ output pin name and CPU destination.
3. **One-shot after timeout:** preliminary prose and later measured T1 reload/readback behavior differ. Do not extrapolate a continuous-mode fix to all post-timeout states or to T2.
4. **Handshake asymmetry and PB7:** simplified port formulas omit alternate functions. Read the explicit NMOS A/B handshake and PB7 notes before judging a general port abstraction.
5. **Map/firmware defaults versus hardware:** a BASIC initialization value, jiffy frequency, or normal keyboard scan is not a fixed chip reset state or maximum capability.

For each disputed behavior, record source edition, PDF/printed page or figure, chip/board target, and which evidence resolves it. If unresolved, report the uncertainty and a minimal hardware/differential experiment instead of modifying the expected result to match an emulator.

## Preserved source identity

SHA-256 of the supplied PDF copies (verified against C:/Temp originals):

- `VIA.pdf`: `11bd249bc40ea636e6c849c0dbecf6e99d2d998db5b4a8cb0213ec209948920f` (10226505 bytes).

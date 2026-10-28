# VIC-I sources and conflict policy

Consulted 2026-10-27. PDF locators count viewer pages from 1; printed book pagination is separate. The two supplied PDFs here and the VIA PDF in the paired skill are preserved without modification.

## Supplied documents and useful locators

- [VICI.pdf](VICI.pdf): MOS/Commodore, *6560/6561 Video Interface Chip*, 18 PDF pages, including application material. PDF p. 1 identifies features/variants; p. 2 is the functional block diagram; p. 3 the bit-field diagram; p. 4 explains CR0..CRF; pp. 5-7 explain mixed hires/multicolor and addressing examples; pp. 9-11 describe CPU/VIC bus phases, clocks, POT inputs and option pins; pp. 12-16 contain electrical/system timing; pp. 17-18 show an application system. The register diagram on PDF p. 3 was visually inspected. Original supplied path: `C:/Temp/VICI.pdf`.
- [MappingVIC20.pdf](MappingVIC20.pdf): Russ Davies, *Mapping the VIC*, COMPUTE! Publications, 1984, 452 PDF pages. Printed chapter 4 pp. 105-120 (PDF pp. 127-142) covers RAM/character ROM; chapter 5 pp. 121-142 (PDF pp. 143-164, substantive register discussion starts printed p. 123) covers VIC-I; chapter 6 pp. 143-176 (PDF pp. 165-198) covers VIA wiring; chapter 7 pp. 177-183 (PDF pp. 199-205) covers I/O and color RAM. Appendices E/G/K/L provide relocation, programmable characters, and screen/color tables. Original supplied path: `C:/Temp/MappingVIC20.pdf`.
- [VIA.pdf](../../vic20-via-audit/references/VIA.pdf): November 1977 NMOS datasheet preserved in the paired skill; PDF p. 8 Figure 6 supplies the timer period needed for cross-chip raster checks.

For local PDF extraction, preserve page boundaries and compare diagrams visually when OCR confuses `0/O`, `1/I`, `$9005/$9085`, or register bit order. Mapping the VIC contains genuine technical errors in addition to OCR errors; it is not an exact timing specification.

## Internet primary documents and direct implementation evidence

- [Commodore Programmer's Reference Guide archive](https://www.zimmers.net/anonftp/pub/cbm/vic20/manuals/): locate the original scanned guide and schematics. The [etext 1.1](https://www.zimmers.net/anonftp/pub/cbm/vic20/manuals/vic20-programmers-reference-guide-1.1.txt) was consulted for graphics, sound, memory map, and chip reference material. Use a scan to settle transcription disputes.
- [Commodore board schematic archive](https://www.zimmers.net/anonftp/pub/cbm/schematics/computers/vic20/): board-specific source for inverted address wiring, chip selects, memory/color RAM and shared light-pen/VIA connections. Choose and inspect the applicable drawing; a directory listing does not establish every net connection.
- [MOS chip-document archive](https://www.zimmers.net/anonftp/pub/cbm/documents/chipdata/index.html): locator for the original 6560/6561 February 1980 scan (`6560.zip`) and historical `VIC-I.txt`. The supplied PDF is the preserved datasheet used here. Some online 6560 datasheet links were inaccessible; no unseen text from them is asserted as reviewed.
- [VICE VIC20 clock and frame constants](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/vic20/vic20.h): non-interlaced periods, NTSC first/last partial-line convention, and interlace distinctions. Implementation evidence helps resolve generic-datasheet ambiguities; it is not a substitute for measured target timing.
- [VICE timing/viewport selection](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/vic20/vic-timing.c): separates video standard and cropping from chip geometry.
- [VICE cycle-based VIC-I](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/vic20/vic-cycle.c): independent evidence for display-window state, pending column counts, fetch phases, and matrix-pointer advancement. Includes unresolved timing TODOs; do not copy them into hardware requirements.
- [VICE VIC-I register interface](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/vic20/vic-mem.c): raster reads, register effects, pixel delays for color/reverse changes and explicit uncertainty around raster counters.
- [VICE VIC20 memory interface](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/vic20/vic20mem.c): distinguish CPU map, VIC backing memory, color RAM, I/O decoding and expansion behavior.
- [VICE VIC20 sound implementation](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/vic20/vic20sound.c): inspect divider, oscillator/shift/noise state, volume/mixing and register-write timing. Its analog approximations are implementation decisions, not universal chip requirements.
- [MAME VIC20 machine wiring](https://github.com/mamedev/mame/blob/master/src/mame/commodore/vic20.cpp): independent board/input cross-check; PA5 light pen, VIA pins and attached memory/peripherals.

For future source comparisons record the retrieved commit of mutable `main`/`master` links. These URLs are discovery points, not frozen golden versions. Prefer the original manufacturer scan and applicable board drawing for documented behavior, use independent implementations or hardware measurements for undocumented timing, and label unresolved behavior explicitly.

## Conflicts and limits worth checking

1. **CPU address versus raw chip address:** VICI.pdf gives the bare chip select and 14-bit bus examples. Do not transplant its `$1000` register location or example CPU/memory system into the VIC20. Translate addresses through board wiring.
2. **Raster line totals:** generic VICI.pdf PDF p. 15 notes 262 non-interlaced NTSC lines, whereas common VIC20 emulation uses 16965 cycles with partial-line counter conventions. Record the chip revision and the counter/time convention; do not adjust the global CPU speed to reconcile them.
3. **PAL and NTSC clocks:** Mapping the VIC printed p. 123 combines a PAL-like CPU frequency with the NTSC crystal description. Do not use that paragraph as a coherent oscillator/divider specification.
4. **VIC options:** VICI.pdf PDF p. 11 describes memory clock, reset, light pen and bus-available alternatives. These are mutually variant-dependent features, not four simultaneous pins on the VIC20 chip.
5. **Address/default restrictions:** book statements about usual screen positions, the need for a particular pointer bit, 506 used color entries, or expansion-selected layouts describe software use and sometimes simplify hardware. Derive electrical address capability independently.
6. **Sound terminology:** the book's oscillator labels, approximate note formulas and waveform descriptions do not establish exact shift-register/noise behavior. The datasheet itself is not a complete transistor-level sound specification.
7. **Palette and view area:** analog television presentation, overscan, RGB palette and a host renderer's horizontal doubling are different from native chip timing and color selection.

If evidence conflicts, include the competing page/figure or function, chosen model, rationale and minimal discriminating test in the audit. Preserve the uncertainty rather than claiming a screenshot or another emulator settles undocumented hardware behavior by itself.

## Preserved source identity

SHA-256 of the supplied PDF copies (verified against C:/Temp originals):

- `VICI.pdf`: `091d7601ab72820de9893cf2f7304e2916f5dfbd91a98a7034220d77789d15d8` (7807093 bytes).
- `MappingVIC20.pdf`: `0356b20202975194fb70ce60aee352baafb6f98be8e7a6578b42b2085a615945` (35782857 bytes).

# VIC-I 6560/6561 audit reference

## Contents

- Variant, clock, and raster
- Register table and graphics decoding
- VIC versus CPU memory views
- Timed fetch/display behavior
- Light pen, paddles, and audio
- Validation matrix and cross-chip diagnosis

Read [sources.md](sources.md) for source precedence, page locators and known documentary conflicts. The generic VIC datasheet describes configurable options and an example system; that example is not the VIC20's complete board map.

## Variant, clock, and raster

| Common non-interlaced target | 6560 NTSC | 6561 PAL |
|---|---:|---:|
| CPU cycles per ordinary line | 65 | 71 |
| Effective complete lines per frame | 261 | 312 |
| Cycles per frame | 16965 | 22152 |
| Approximate CPU frequency | 1.022727 MHz | 1.108405 MHz |

These are steady non-interlaced VIC20 targets, not an interlace specification. NTSC raster counter numbering can expose short first/last portions that sum to a complete line; do not confuse the number of counter labels with effective frame duration. The generic datasheet's NTSC timing table says 262 non-interlaced lines, which needs reconciliation with target-specific measurements/implementations before using it as an exact frame oracle. Interlace must specify field length, half-line handling and counter behavior separately. Do not implement PAL interlace solely because a generic register table includes the bit.

Four native high-resolution dots are emitted per CPU cycle. EMULATORS doubles the horizontal representation to eight internal positions per cycle; one 8-dot character spans two CPU cycles and sixteen internal positions. Keep native dots, doubled positions, cycles, character columns, raster lines and visible-window offsets explicitly named. Host SDL rate, sample rate and frame skipping must not change chip-cycle counts.

The VIC20 uses a 6502, not a 6510: `$0000/$0001` are not a CPU banking port. VIC-I shares memory through alternating bus phases and must not inherit VIC-II badlines, sprite DMA, BA/AEC CPU stealing or raster IRQ registers. Its timer-driven raster software generally gets its interrupt from a VIA.

## Register table

CPU canonical addresses are `$9000-$900F`; the datasheet calls the corresponding registers CR0..CRF. The bare chip's illustrated `$1000` select is not the VIC20 CPU address. Verify board decode for aliases rather than assuming the entire surrounding page is sixteen simple mirrored registers.

| Address | Contract to audit |
|---|---|
| `$9000` | Bit 7 interlace selection where supported; 6..0 horizontal origin, four native dots per increment |
| `$9001` | Vertical origin, two raster lines per increment |
| `$9002` | 6..0 column count; bit 7 supplies video-matrix A9 and associated color address selection |
| `$9003` | Bit 7 reads raster bit 0; 6..1 row count; bit 0 selects 8x8 (0) or 8x16 (1) glyphs |
| `$9004` | Raster bits 8..1, read-only raster source |
| `$9005` | 7..4 matrix address bits 13..10; 3..0 character-base bits 13..10 in VIC address space |
| `$9006`, `$9007` | Latched light-pen horizontal, vertical coordinates; not live raster reads |
| `$9008`, `$9009` | POT X/Y conversion results; ordinary CPU writes do not create input values |
| `$900A..$900C` | Three tone sources; bit 7 enables, 6..0 select divider |
| `$900D` | Noise-source enable and divider selection |
| `$900E` | 7..4 auxiliary color, 3..0 master volume; preserve the other function when modifying either field |
| `$900F` | 7..4 background color, 3 normal/inverted hires selection (1 normal, 0 inverted), 2..0 border color |

Audit read masks independently from writable storage. A write to `$9003` cannot set its live raster bit; a write to `$9004` cannot reposition the beam. Reconstruct raster as `(read9004 << 1) | (read9003 >> 7)`, accounting for the fact that two separate reads may straddle a line or frame boundary. Projection past the final line must wrap to the target's actual raster sequence, not simply add one forever.

An 8x16 glyph is sixteen independently stored bytes, not eight bytes vertically duplicated. The usual 22x23 display is a firmware choice. Zero dimensions, larger counts and writes during display require counter/latch rules, not a static rectangle derived once per frame.

### Color interpretation

Color RAM supplies a nibble per matrix entry: bit 3 selects multicolor for that character; bits 2..0 select its foreground color. Hires and multicolor characters can coexist.

| Mode | Glyph interpretation |
|---|---|
| Hires, normal | Bit 1 -> character foreground, bit 0 -> common background |
| Hires, inverted | Bit 1 -> common background, bit 0 -> character color |
| Multicolor pair 00 | Common background (`$900F` high nibble) |
| Multicolor pair 01 | Border (`$900F` bits 2..0) |
| Multicolor pair 10 | Character foreground (color RAM bits 2..0) |
| Multicolor pair 11 | Auxiliary (`$900E` high nibble) |

Multicolor pairs are consumed from bits 7..6 to 1..0 and span two native dots. The `$900F` inversion bit does not invert multicolor selection. Background and auxiliary have sixteen choices; border and per-character foreground have eight. Verify a model-specific palette separately from these logical codes; labels and RGB approximations are not evidence for analog color fidelity.

## Memory: derive the VIC address before translating to the CPU view

Let `r2` and `r5` be the register bytes. The raw 14-bit chip bases are:

```text
matrixVIC = ((r5 & $F0) << 6) | ((r2 & $80) << 2)
glyphVIC  = (r5 & $0F) << 10
```

On the normal VIC20 board, the principal translation into the CPU view is:

| VIC address window | CPU view of backing region |
|---|---|
| `$0000-$0FFF` | Character ROM `$8000-$8FFF` |
| `$1000-$1FFF` | CPU `$9000-$9FFF` window, subject to physical decode/open bus rather than assumed RAM |
| `$2000-$2FFF` | CPU `$0000-$0FFF`, including only RAM actually present/visible |
| `$3000-$3FFF` | CPU `$1000-$1FFF` internal RAM |

For these windows, the translation can be expressed as `(vicAddress & $1FFF) | ((vicAddress & $2000) ? 0 : $8000)`. This is a wiring translation, not permission to invoke CPU I/O read side effects on every VIC fetch. Determine what drives the VIC data bus in an I/O/unpopulated window from board decode and fetch phase. Standard cartridge expansion at `$2000/$4000/$6000/$A000` in CPU space is not automatically accessible to VIC-I merely because the CPU can read it.

Examples that expose incorrect translations:

- `$9005=$F0`, `$9002=$96`: raw matrix `$3E00` -> CPU `$1E00`; raw glyph `$0000` -> character ROM `$8000`; normal color base `$9600`.
- `$9005=$CD`, `$9002=$15`: raw matrix `$3000` -> CPU `$1000`; raw glyph `$3400` -> CPU `$1400`; normal color base `$9400`.
- Low character nibble `0..7` maps to CPU `$8000,$8400,...,$9C00`; `8..F` maps to `$0000,$0400,...,$1C00`. Preserve the distinction between address selection and whether usable backing storage exists.

The matrix base has 512-byte granularity. A glyph uses `characterCode * (8 or 16) + glyphRow` within the generated VIC address; check chip address-width wrapping before translation. Reverse screen codes are still eight-bit glyph indices, not automatically permission to invert a glyph byte in software.

The physical color RAM covers `$9400-$97FF` (1024 nibbles). The normal 512-byte starting half is `$9400 + ((r2 & $80) << 2)`. Do not equate the KERNAL's default choice for a memory expansion with a hardware disappearance of the other half. Check physical address generation/wrap when the programmed matrix crosses a half or exceeds normal 506-entry use; the default screen size is not a RAM capacity limit. CPU reads of the upper nibble need documented bus behavior, not arbitrary stored eight-bit color data.

CPU memory allocation, ROM defaults, screen-pointer system variables and VIC address selection are separate mechanisms. Changing a BASIC screen pointer alone does not rewire the chip; installing expansion RAM does not itself move an existing VIC matrix. Use Mapping the VIC for software conventions and the schematic for electrically available memory.

## Timed display and fetch audit

Build a timeline of horizontal/vertical window comparison/latches, row-within-glyph, matrix row base, character column, screen/color fetch, glyph fetch, shift and output. A character's matrix/color data and glyph data have fetch times; continuously re-reading everything at each pixel group can create effects absent on hardware. Conversely, caching a whole frame can hide real CPU writes.

Identify where horizontal origin, vertical origin, dimensions and double-height are sampled. A frame-start snapshot cannot be presumed correct for mid-frame modifications. Reopening/closing windows, zero columns/rows and changing width near an internal counter boundary are useful discriminators. Track counter reset/wrap during blanking instead of resetting display state at an arbitrary host presentation event.

For a CPU write, distinguish instruction start, effective bus-write cycle, chip sampling and output latency. Color/reverse changes may have sub-character and different pixel delays; read the actual target timing evidence rather than imposing a shared instant response. Full-instruction execution can apply a new value to cycles that precede the physical write; end-of-instruction raster projection alone does not solve that. RMW dummy writes and RAM/color writes used by raster code also need correct placement.

Host cropping and doubled coordinates are presentation choices. Check bottom/right clipping against the emulated display window and underlying buffer bounds, including partially visible characters. Do not use a narrower host window to explain away a counter or fetch defect.

## Light pen and paddles

The VIC20 light-pen option latches beam coordinates on the active negative edge. Map the shared VIA1 PA5/fire/user-port signal and the chip pin separately from host mouse coordinates. Validate coordinate units, horizontal quantization/read masks, visible versus border positions, and latch/rearm behavior for the selected revision. The datasheet's alternative option pins (memory clock, reset, bus available) describe other variants and must not be simultaneously modeled as VIC20 light-pen features.

POTX/Y are RC conversion inputs, not joystick direction registers. No connected paddle commonly reads `$FF`; test this separately from an actual changing resistance and conversion cadence. A constant `$FF` may be a deliberately unsupported adapter path, not complete paddle emulation. A mouse input adapter does not by itself prove electrical light-pen timing or analog paddle conversion fidelity.

## Sound

Audit three tone dividers, the noise generator, enable/disable sequencing and the shared volume independently from the host sample rate. As a nominal pitch check for divider value `n = register & $7F`, the three tone frequencies are `fCPU / (256 * (128-n))`, `fCPU / (128 * (128-n))`, and `fCPU / (64 * (128-n))`. These checks do not prove exact internal oscillator waveforms, phase reset or behavior at extreme register values.

Do not import SID ADSR/filter/register rules. Inspect VIC oscillator shift state, noise polynomial/clocking, phase continuity on writes, master-volume DAC behavior and mixing/clipping against the actual evidence. Treat register `$7F/$FF` limits and enable transitions as explicit tests; do not generalize a musical note table into a full hardware model. PAL/NTSC pitch changes follow chip clock, not host FPS. `$900E` color writes must preserve sound behavior and vice versa.

When `VICI::SoundFunction` is scheduled separately from `VICI::simulate`, verify it advances exactly once per emulated cycle range; its absence from the video loop is not itself a defect. Separate analog fidelity limitations (palette, filtering, nonlinear volume) from digital divider or timing defects.

## Validation matrix

| Area | Discriminating observations |
|---|---|
| Raster | Exact line/frame duration; `$9003/$9004` around line/frame wrap; both CPU modes; PAL versus NTSC; interlace separately |
| Pointer decoding | Both matrix A9 values; all glyph nibbles; ROM/RAM windows; expansion visible only where wired; base/address wrapping |
| Graphics | Single-bit asymmetric glyph; bit pairs 00/01/10/11; mixed character modes; inversion; 8x16 with distinct upper/lower halves |
| Dynamic writes | `$9000/1/2/3/5/E/F` near latch/fetch/output boundaries; glyph/color changes between fetch and output; 6502 RMW effects |
| Display geometry | Zero dimensions; nonstandard width/height; partial characters at each edge; bottom rows; blanking state independent of SDL presentation |
| Inputs | Light-pen negative edge and coordinate/rearm boundaries; shared PA5; POT disconnected and varying input |
| Sound | Each enabled voice and divider limits; octave ratios; PAL/NTSC scaling; noise repeatability; volume and auxiliary-color isolation; no duplicate simulation |
| VIA coupling | `frameCycles` versus T1 `N+2`; repeated same-PC raster phase; IRQ assertion versus handler latency; counter-read jitter compensation |

For an apparently fast-moving background, distinguish game animation speed from accumulated phase drift: compare the same write sequence against the same raster line over frames. A two-cycle/frame error is eight native dots/frame even when timer frequency differs by only a small fraction. For a missing actor, first determine whether this exact PRG/release/state draws it, then inspect generated glyph/matrix/color writes and fetch timing. A short welcome-screen trace does not prove game logic or the expected attract-mode contents.

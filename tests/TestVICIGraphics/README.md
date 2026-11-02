# VIC-I graphics and memory-view regressions

This component test calls the real VICI::drawGraphics(), hires/multicolor decoders and final ScreenMemory copy. It replaces only the frame storage with an identically sized buffer surrounded by eight sentinel words on each side. It also compares every frame pixel, so an overflow into the next row fails even when it remains inside the allocation.

| Claim | Stimulus and independent observation |
| --- | --- |
| PAL/NTSC bounds | Actual current frame dimensions 467x284 / 420x233; first, middle and final rows |
| Complete and partial spans | Full blocks at x=0, x=40 and width-8; right-edge spans of 7, 6, 5, 4, 3, 2 and 1 positions; all surrounding pixels retain the sentinel |
| Hires normal/inverse | All sixteen nibbles against literal expanded-dot truth tables; character color 5, background 2 |
| Multicolor | All sixteen nibbles against literal expanded-pair truth tables; 00/01/10/11 map to background 2, border 3, character 5 and auxiliary 4 |
| Multicolor ignores inverse | Both inverse-bit settings produce the same multicolor pixels |
| Frame safety | Prefix/suffix sentinels and all pixels outside the intended span remain unchanged, including the bottom-right corner |

Each video system runs 1,920 cases: four mode/inverse combinations, sixteen nibbles, ten horizontal origins and three rows. The expected span lengths are explicit test data rather than a copy of the production clipping calculation.

The graphics-only fixture initializes the real VIC-I register bank and chip without ROM files; the production memory-view fixture described below requires the existing VIC20 ROMs. It injects an already fetched graphics nibble, so it does not certify memory fetch phases, CPU/register timing, raster counters, the physical palette or PITFALL rendering. Frame dimensions describe the current host buffer rather than an independent hardware viewport requirement.

## Build and run

The project is registered under Libs/Micros/Tests in Emulators.sln, with Debug/Release Win32/x64 configurations and CORE/COMMODORE/F6500/VIC20 ProjectReferences. Local verification uses only Debug|Win32 (solution Debug|x86).

    $msbuild = 'C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe'
    & $msbuild Emulators.sln '/t:Libs\Micros\Tests\TestVICIGraphics' /p:Configuration=Debug /p:Platform=x86
    $env:PATH = "$env:RAKNET_HOME\Lib;$env:SDL_HOME\VisualC\Win32\Debug;$env:SDLIMAGE_HOME\VisualC\Win32\Debug;$env:PATH"
    Push-Location emulators/VIC20Emulator
    & ../../exe/x86/TestVICIGraphicsD.exe
    Pop-Location

The baseline failed before the production change: PAL, normal hires, nibble 1, x=460, y=0 overwrote pixels outside the expected span. After the fix, TestVICIGraphicsD.exe returned exit 0 with all 3,840 cases passing.

## Register construction and reinitialization

The same executable checks the constructor presets before connecting sound or raster: matrix $1e00, character data $8000, color RAM $9600, geometry and colors. It then links a real sound wrapper and raster, relocates the matrix/characters/colors to $1000/$1400/$9400, changes geometry and sound registers, and calls initialize() twice.

Both resets must restore all sixteen register readbacks and the derived presets. The existing sound wrapper must receive the reset writes; the existing raster must still report lines 4 and 6. Raster-register reads are performed only after linking the raster.

These postcondition tests do not by themselves detect the former intermediate uninitialized read. Source review establishes its removal: all three address selectors are assigned before initializeInternalValues() performs any register write, while its constructor and initialize() callers are retained. This change does not alter the address translation formulas or the emulated hardware reset specification.

Verification of the initialization change used Debug|Win32 throughout. VIC20Emulator compiled and linked from Emulators.sln in Debug|x86. Windows prevented overwriting TestVICIGraphicsD.exe (LNK1168; no active test process remained), so the already-built libraries were linked into a fresh output with:

    & $msbuild tests/TestVICIGraphics/TestVICIGraphics.vcxproj /p:Configuration=Debug /p:Platform=Win32 /p:BuildProjectReferences=false /p:TargetName=TestVICIRegistersD
    & ./exe/x86/TestVICIRegistersD.exe

TestVICIRegistersD.exe returned exit 0: constructor presets, two connected register resets, and all 3,840 PAL/NTSC graphics/guard cases passed. Source review also confirmed that the sixteen preset writes and the address translation formulas remain unchanged.

## Matrix base translation

The register regression checks all 32 matrix-base combinations ($9005 high nibble and $9002 bit 7) against a literal table of CPU-view equivalent addresses. For each combination it also exercises all sixteen character selectors (512 cases) and all 128 column values (4,096 cases), checking the selected character base and the independence of the matrix/color bases.

The former expression failed the new regression at matrix nibble 0, A9 clear: it selected $2000 instead of $8000. The correction maps VIC A13 to inverted CPU A15, retaining the matrix A12..A10 and A9 bits. Character and color address expressions are unchanged. The normal $1e00/$8000/$9600 presets and the $1000/$1400/$9400 relocation remain covered.

These assertions verify base decoding only. They do not certify which storage drives each VIC bus window, CPU I/O side effects, offset wrapping, fetch timing, or PITFALL behavior.

Verification commands (Debug Win32 test; Debug x86 solution):

    & $msbuild Emulators.sln '/t:Emulators\VIC20\VIC20Emulator' /p:Configuration=Debug /p:Platform=x86 /m:4
    & $msbuild tests/TestVICIGraphics/TestVICIGraphics.vcxproj /p:Configuration=Debug /p:Platform=Win32 /p:BuildProjectReferences=false /p:TargetName=TestVICIMatrixD
    & ./exe/x86/TestVICIMatrixD.exe

The fresh target name avoids the previously observed Windows lock on the standard test executable; the project references use the libraries already rebuilt by the solution command. The PATH setup above supplies the matching Win32 Debug SDL runtime.

VIC20Emulator compiled and linked successfully. TestVICIMatrixD.exe returned exit 0: all 32 matrix bases, 512 character-selector cases, 4,096 column-independence cases, constructor/two connected resets, and 3,840 PAL/NTSC graphics/guard cases passed. The baseline TestVICIMatrixBeforeD.exe returned exit 1 at nibble 0, A9 clear before the formula correction.

## VIC address offsets and wrapping

The register regression walks every 14-bit offset for all 32 matrix bases and all 16 character bases: 524,288 matrix cases and 262,144 character cases. Every case is also checked with an additional $4000 offset. The oracle uses a literal table of the 32 successive 512-byte VIC bus blocks and block-relative arithmetic, rather than the production bit translation. Offset zero must agree with the existing cached base accessors.

The synthetic component memory now supplies ordinary RAM outside the VIC register bank. A fetch probe places $a5 at CPU-view $8000 and $3c at the former incorrect $2000 destination. Matrix base VIC $3e00 plus $0200 must fetch $a5. Glyph base VIC $3c00 plus $0400 must fetch the high/low nibbles $a/$5 for both 8x8 (code $80) and 8x16 (code $40) characters. These probes run for both PAL and NTSC before the existing graphics/guard cases.

The synthetic backing is a test fixture, not a claim that these CPU windows are all physically accessible to the real VIC. Board memory visibility, I/O read side effects, color-address wrapping and fetch phases remain outside this change. Monitor snapshot code now translates every byte address as well; monitor command execution is not covered by the automated fixture.

Verification uses the Debug x86 VIC20Emulator solution build and the Debug Win32 test linked against the rebuilt libraries:

    & $msbuild tests/TestVICIGraphics/TestVICIGraphics.vcxproj /p:Configuration=Debug /p:Platform=Win32 /p:BuildProjectReferences=false /p:TargetName=TestVICIAddressD
    & ./exe/x86/TestVICIAddressD.exe

TestVICIAddressD.exe returned exit 0: the 524,288 matrix and 262,144 glyph offset cases (each also tested one wrap later), the real matrix/glyph fetch probes in both video systems, the previous base/selector/column cases, constructor/two resets, and all 3,840 graphics/guard cases passed. VIC20Emulator compiled and linked in Debug x86; the existing Cartridge.cpp C4065 warning is unrelated.

## Separate CPU and VIC-I memory views

The graphics fixture now has separate ordinary MemoryView instances: CPU view 0 and video view 1. Its video map excludes CPU registers and expansion RAM and includes the production VICIColorRAMSubset alias. The VIC probe uses view 1 explicitly.

A second fixture constructs the actual VIC20::Memory map. It checks all 9,216 shared internal RAM/character-ROM bytes, all 6,144 disconnected addresses in each of the ten configurations, and all 1,024 color cells in each configuration. The four video-only aliases must be absent from the CPU view and must preserve physical backing through initialization and forced writes. Both color starts are checked over 2,048 offsets each, including wrap at the physical 1,024-nibble boundary.

A real connected VIA2 register bank receives a pending T1 flag. The real glyph reader fetches code $24, row 4, with $9005=$f4: its CPU-equivalent address is $9124. The video fetch and all monitor snapshots must preserve T1's flag; the subsequent CPU-view read of $9124 must clear it. The snapshots also verify matrix/color contents and lengths. Changing the active view must not change the view used by VICI. A missing configured video view must fail initialization.

The test project consequently links VIC20/F6500 as well as COMMODORE/CORE in all four configurations, and uses the corresponding existing RakNet import library required by the module headers. Local verification remains Debug Win32 only. The production map loads the existing English BASIC, character and KERNAL ROMs, so run from emulators/VIC20Emulator, which already contains its bios directory; no ROMs are copied into the test.

    & $msbuild tests/TestVICIGraphics/TestVICIGraphics.vcxproj /p:Configuration=Debug /p:Platform=Win32 /p:BuildProjectReferences=false /p:TargetName=TestVICIViewsD
    $env:PATH = "$env:RAKNET_HOME\Lib;$env:SDL_HOME\VisualC\Win32\Debug;$env:SDLIMAGE_HOME\VisualC\Win32\Debug;$env:PATH"
    Push-Location emulators/VIC20Emulator
    & ../../exe/x86/TestVICIViewsD.exe
    Pop-Location

VIC20Emulator compiled and linked in Debug x86. Disconnected video windows deliberately return $ff, and video color reads return $f0 OR the stored low nibble. These are explicit placeholders, not an electrical open-bus model. CPU color mapping/write masks and fetch-phase timing are outside this change. No bus-state class or derived MemoryView is introduced.

TestVICIViewsD.exe returned exit 0 in Debug Win32: the production map/IRQ/snapshot/alias/color tests and all prior register-address and 3,840 PAL/NTSC graphics cases passed. The pre-existing Cartridge.cpp C4065 warning remained in the Debug x86 integration build.

## CPU color RAM access

CPU color RAM now consists of two always-active CPUColorRAMSubset objects backed by physical $9400-$95ff and $9600-$97ff. CPU writes store only the low nibble, including inherited initialization and forced writes. Reads use $f0 OR the stored nibble as the same explicit upper-bit placeholder used by the video alias; the electrical open bus remains unimplemented. The CPU/video/global subset maps remain separate, with 25/9/29 entries and the five existing shared RAM/ROM subsets.

The color regression writes through the CPU view rather than seeding only physical memory. In each of the ten enum configurations it checks all 1,024 cells against physical and video observations (10,240 cases), and exercises all 256 byte values at the four half-boundary addresses (10,240 writes). It then preserves a full independent nibble pattern through all ten enum selections and all 256 byte-mask selections (266 x 1,024 observations). CPU subset initialization is checked against the framework's zero defaults, followed by a forced CPU write and a no-op video alias initialization. These assertions do not specify power-on hardware RAM contents.

The new test failed against the preceding libraries: CPU write $a5 to $9400 with no expansion left the explicitly seeded physical cell at $00 instead of $05. The baseline target was TestVICIColorBeforeD.exe, invoked with --color (exit 1).

Build/run the corrected Debug Win32 suite using the Debug x86 solution libraries:

    & $msbuild Emulators.sln '/t:Emulators\VIC20\VIC20Emulator' /p:Configuration=Debug /p:Platform=x86 /m:4
    & $msbuild tests/TestVICIGraphics/TestVICIGraphics.vcxproj /p:Configuration=Debug /p:Platform=Win32 /p:BuildProjectReferences=false /p:TargetName=TestVICIColorD
    Push-Location emulators/VIC20Emulator
    & ../../exe/x86/TestVICIColorD.exe
    Pop-Location

The PATH setup above supplies the Win32 Debug SDL/RakNet runtime. Passing --color runs only the new CPU color regression; omitting it runs this regression and the complete existing suite.

TestVICIColorD.exe returned exit 0 in Debug Win32: all new CPU color cases, production-view/VIA/snapshot tests, register/address tests and 3,840 PAL/NTSC graphics cases passed. VIC20Emulator compiled and linked in Debug x86 with the existing unrelated Cartridge.cpp C4065 warning. Source comparison confirmed that every expansion-bank setActive operation is unchanged; only color activation was removed.

## CPU bus-cycle writes and sound intervals

`TimedWrites.cpp` executes real F6500::C6502 instructions against production VIC20 memory and a connected VIC-I. Separate test-only CPU/video RAM overlays share backing storage: the CPU overlay records physical commits and the video overlay observes state during VIC fetches. Capture begins before CPU execution so it includes pre-instruction advancement as well as the later simulate() call, while CPU operand reads cannot contaminate the video observations. Expectations use literal 6502 cycle offsets (zero based), independently of the production BusCycleData lookup.

| Claim | Stimulus and observation | Expected result |
|---|---|---|
| Stable multi-subset extraction | Interleave selected A/B/A commands with two foreign commands | Selected values 1/2/3 and excluded values 7/8 retain their own original order; reserved destination capacity is retained |
| Register write timing | STA $900f, observe register during each VIC cycle | Four observations retain the old color; the new value is present at instruction completion |
| RMW and sound | INC $900a from $80, observe sound setter cycle and generated ticks | Dummy/final writes at offsets 4/5, after five/six sound ticks; the following NOP generates two ticks using $81 |
| RAM timing | STA/INC $1000 and STA $10ff,X with X=1 | Physical commits at offsets 3, 4/5 and 4 respectively; pre-write observations retain the previous RAM value |
| Context variants | LDA $10ff,X with X=1 | Five cycles including the page crossing |
| Color masking and timing | STA $9400 with $ab | Pre-write observations retain $03; the committed physical nibble is $0b |
| Consumer isolation | STA $9123 | VIA command remains available to its own extractor |
| Interrupt stack timing | Accepted NMI and IRQ after preparing interrupt sampling | Seven cycles; stack cells change after CPU write offsets 2/3/4 |
| Clock origins and no replay | Run every case for PAL/NTSC with initial CPU clock 0 and 6 | No first instruction is lost; repeated sound simulation at the same endpoint adds no ticks |

The RAM overlay is only test instrumentation; production uses the existing CPU/video views and existing buffered commands. `enableCPUTimedWrites()` validates these buffers and connects the consumer, then aligns the video/sound clock origin. It does not enable memory buffering.

    & $msbuild tests/TestVICIGraphics/TestVICIGraphics.vcxproj /p:Configuration=Debug /p:Platform=Win32 /p:BuildProjectReferences=false /p:TargetName=TestVICITimedD
    Push-Location emulators/VIC20Emulator
    & ../../exe/x86/TestVICITimedD.exe --timed
    & ../../exe/x86/TestVICITimedD.exe
    Pop-Location

Use the PATH setup above and first rebuild the VIC20Emulator solution target in Debug x86. The fresh Debug Win32 timing executable passed both the selective run and the complete existing suite, including all 3,840 graphics/guard cases. TestVIAVICIWritesD.exe, rebuilt from TestVIATimer with the same libraries in Debug Win32, also passed its full regression suite.

This change establishes cycle-granular ordering: the current VIC fetch/output and sound interval precede the CPU write, then raster counters advance. It does not certify physical pixel latency, a phase-accurate fetch pipeline, raster-read timing, audio waveform fidelity, or PITFALL behavior.

## Raster read-point advancement

VIC-I now advances to the selected instruction read point before CPU execution, leaves the final cycle pending, and completes that cycle afterwards. The timing fixture uses separate CPU/video overlays to retain its existing write observations across this scheduling change. No new cases were added, and no compilation or test execution was performed for this change. Raster-read boundary coverage remains to be added and executed when requested.

## Retaining character data across output halves

The preceding retention step acquired the matrix code, color nibble and complete glyph-row byte on the first output half, then consumed the retained byte and color on the second half. The alternating-acquisition change below supersedes that acquisition schedule. The retained data is reset alongside the display counters.

The existing wrapped-address and isolated-VIA fetch probes were adapted to inspect the complete retained byte returned by readGraphicalInfo(), rather than a previously selected output nibble. No new cases were added, and no compilation or test execution was performed for this change. The preceding pass reports refer to earlier revisions. Separate matrix/glyph fetch cycles, their startup timing and the output pipeline remain pending; this step alone does not complete cycle-accurate VIC-I fetch emulation.

## Alternating acquisition and independent output

The current cycle model alternates matrix/color acquisition and glyph-row acquisition inside the existing geometry window. Matrix reads update the pending code/color. Glyph reads publish the complete glyph byte and matching color to output; the high nibble is emitted in that cycle and the low nibble in the following cycle. Acquiring the next matrix cell cannot overwrite the current output color. Output can finish after the acquisition window closes; physical line initialization discards any residual half at the raster boundary.

Reported horizontal output coordinates include one raster step after the matrix-acquisition origin and use the same cached column count as the acquisition controller. This is the explicit cycle-granular convention of this implementation, not a measured physical output latency. Absolute window timing, dynamic geometry latches and subcycle output behavior remain pending. Idle accesses retain their existing placeholder; this is not an open-bus implementation.

The direct-render fixture now marks its manually prepared output nibble as valid. No new cases were added, and no compilation or test execution was performed for this change. Earlier passing results describe earlier revisions and do not validate the alternating acquisition or output timing.

## Live origin comparisons

Horizontal and vertical origins are now compared against the live raster before each video acquisition. A pending opening observes CPU writes to $9000/$9001 from the next video phase; a passed horizontal match is not replayed, and a line cannot open twice. Vertical state distinguishes waiting, pending first horizontal opening, displaying and finished. Glyph rows do not advance while the vertical opening is pending. Frame wrap resets display state independently of GRAPHICSREADY notification.

At this preceding step, column count was sampled when preparing each line, while row count and expanded height used frame initialization. The pending/active column update below supersedes that policy for columns; glyph height now uses the live mode. Row sampling, matrix advancement in unusual geometries, absolute startup latency and subcycle effects remain pending. Reported horizontal bounds follow each line opening; a single rectangle does not describe every dynamically positioned scanline.

The wrapped-address fixture prepares the new window state instead of removed origin countdowns. No new cases were added, and no compilation or test execution was performed for this change. Earlier passing reports do not validate the live origin comparisons.

## Pending and active column counts

The column count is sampled at horizontalCycle == 1 (the second cycle of the current zero-based line model), independently of display activity. A horizontal opening copies that pending count into the active width. The pending count survives line and frame boundaries; complete initialization alone seeds it from the register. At that step, frame start still sampled the number of rows; the row-sampling update below supersedes it. Glyph height uses the live mode bit and masks the glyph-row address; it is no longer cached per frame.

With horizontal origin zero, acquisition uses the preceding pending count. At origin one, sampling precedes activation in this cycle-granular model. This ordering remains provisional until startup latency is validated. Matrix advancement and unusual geometries retain their current policy. No compilation or test execution was performed for this change; earlier passing reports do not validate these boundaries.

## Matrix advancement without horizontal opening

At a character-row boundary, the matrix base advances by the active width only if the horizontal window opened on that raster line. A line without opening cannot consume a matrix row using a width retained from another line. Vertical counters and vertical-window completion still advance independently. An opened zero-width window adds zero; an opened but truncated acquisition retains the active-width policy pending specific validation.

No compilation or test execution was performed for this change. Earlier passing reports do not validate unopened-line or truncated-acquisition boundaries.

## Row sampling independent of frame reset

The row limit is captured at rasterLine == 0 and horizontalCycle == 2, after origin comparisons and before video acquisition in the current zero-based cycle model. Complete initialization seeds the initial limit; frame reset preserves it until the next capture. Sampling does not reset counters or reopen a completed vertical window. Vertical state, rather than a second ROW < ROWMAX drawing condition, controls display lifetime; row completion still compares the incremented row counter with the retained limit.

Origin zero may open or close the window using the previous retained limit before capture. A later positive limit does not reopen a completed window, and capturing zero after opening does not immediately close it. These boundary effects and the capture cycle are a reference-based emulation convention requiring targeted validation. No compilation or test execution was performed for this change.

## Column advancement on glyph acquisition

The matrix column now advances immediately after a glyph read, including a read completed in the last physical cycle of a line. FetchPhase is the only acquisition-phase controller; the independent FH toggle and deferred nextHorizontal update have been removed. The existing direct-fetch probes no longer prepare FH. The retained output still drains independently after the active width is exhausted, and physical line reset discards any residual half.

This change makes the next-column position reflect completed acquisitions; it does not replace the active-width matrix increment with a completed-glyph count. Matrix advancement for truncated lines remains unvalidated. No compilation or test execution was performed, and no new cases were added.

## Experimental half-character matrix counter

The retained matrix base and working acquisition position now use half-character units. The working position is loaded at horizontal opening and advances after each active matrix/color or glyph acquisition. Matrix and color offsets are (matrixCounter + 1) >> 1; the column counter still counts complete glyph acquisitions. On a character-row boundary, an opened line retains the working position, including an unmatched matrix acquisition. Frame initialization clears both counters; idle accesses do not advance them. The wrapped-address probe seeds 1024 half units for its previous 512-cell offset.

This replaces the preceding active-width matrix increment with an experimental arithmetic model. Physical counter width, anomalous carries, exact boundary ordering and startup latency are not implemented or validated. Fetch DEBUG fields MatrixBaseHalf and MatrixHalf are decimal half-character units; Column and Columns are decimal character units. The working counter can retain the preceding line endpoint while no new window has opened. No compilation or test execution was performed; no compatibility improvement or complete truncated-line correction is established.

## Reference acquisition startup delay

Horizontal opening now enters Start with four startup cycles. For an opening at zero-based cycle N, cycles N through N+3 use the provisional idle access without advancing the matrix counter; N+3 prepares Matrix, the first matrix read occurs at N+4, and the first glyph/high output half at N+5. The latest pending column sample is activated when startup finishes, superseding the opening-only active-width policy above. Video output bounds are calculated after the first matrix read, one cycle before its first output group; bounds without acquisition are not evidence of displayed geometry.

Physical line initialization cancels startup and residual output. Fetch DEBUG now reports Phase=Start/Matrix/Glyph and decimal StartupLeft before cycle activity. This delay is a reference-based convention, not validated physical timing; anomalous carries, exact line-boundary ordering and subcycle output remain pending. The existing fixtures were not extended or executed. No compilation or test execution was performed for this change.

## Matrix counter width and boundary carries

The experimental matrix counter now wraps at 12 bits. Both acquisition phases use the same increment helper; the rounded matrix/color offset is also masked before conversion to character units. The working counter reloads at physical horizontal cycle zero, independently of window opening, and is not reloaded a second time when acquisition starts.

A per-cycle marker is cleared before video activity and set by an active matrix or glyph increment. At line completion, only an increment in that final cycle can retain carries into bits 4 and 8. The next physical-line reload applies those carries independently to their four-bit groups, without propagating a further carry, then consumes them. An earlier increment followed by idle cycles cannot produce a retained boundary carry. Frame initialization clears the new state; horizontal initialization preserves it for the following reload. Character-row completion saves the endpoint before that reload.

This implements the proposed experimental ordering, not verified physical subcycle behavior. The 31-versus-32-increment discrepancy for late acquisition remains unresolved; no artificial endpoint increment was added. The $fff rounded-address case, simultaneous row save/reload/carry ordering, ordinary and truncated lines, and frame transitions still require validation. Existing fixtures were not extended or executed. No compilation or test execution was performed for this change.

## Independent counter, acquisition and output timing

The current experimental schedule supersedes the four-cycle shared-startup convention above. For a horizontal comparison at zero-based cycle N, the matrix counter first increments at N+3, the first matrix/color read occurs at N+5, and the first glyph/high output half at N+6. The low output half follows at N+7. The counter captures the pending width on activation and counts twice that width independently of later acquisitions. Acquisitions use the same active width and may finish after counting stops.

Before each pair of counter increments, the rounded, masked matrix offset is retained for acquisition two cycles later. The current matrix read consumes the old offset before the counter replaces it with the following cell offset; glyph completion advances only the acquired column. Bounds are recorded when the first output group is actually emitted. Physical line reset cancels both startups, remaining counter steps, pending address and residual output, while preserving the boundary carries for reload. The CPU-read split still executes each video phase only once.

For PAL with origin 36 and width 22, the proposed schedule permits 32 counter increments from cycle 39 through 70. That arithmetic result is not proof of physical fetch/output timing or hardware compatibility. This remains a cycle-granular interpretation of the 6561 circuit analysis; subcycle phases, simultaneous boundary operations, unusual widths and live register writes still need targeted validation. The direct address fixture now seeds the retained offset as well as the counter. No new cases were added; no compilation or test execution was performed.

## Inactive cycles do not acquire character data

The fixed $1c00 placeholder read has been removed. Startup and inactive cycles preserve the last acquired screen code and pending color; they do not derive a color nibble from an arbitrary RAM byte. Matrix/color reads are invoked only by the active matrix phase. The final startup cycle still prepares that phase for acquisition on the following cycle.

The independent counter controller, retained pixel output, CPU write replay and raster completion continue on inactive cycles. Retained code/color alone do not authorize output. This removes a fabricated acquisition; it does not specify electrical bus levels, disconnected-window readback or undriven color bits. The separate memory-map placeholders remain in place. No compilation or test execution was performed, and no new cases were added.

## Horizontal viewport alignment

The visible horizontal interval is now 88..554 inclusive for PAL (467 internal positions), and 56..475 inclusive for NTSC (420 positions). Both intervals move 48 positions to account for the current six-cycle origin-to-output delay; line periods, acquisition timing and vertical raster definitions are unchanged.

The supplied PAL BASIC trace has origin 12 and 22 columns: output begins at raw position 144 and occupies 352 positions, leaving visible margins of 56 and 59. For NTSC, origin 5 and 22 columns give calculated margins of 32 and 36; that example has not been confirmed by an NTSC trace. This aligns the viewport with the current output model and does not establish measured analog blanking. DEBUG raster columns remain full-line coordinates; visible coordinates subtract 88 for PAL or 56 for NTSC.

No compilation or test execution was performed for this change. Visual confirmation in both models remains pending.

## Retained bus state for active VIC-I acquisitions

Active matrix and glyph acquisitions now retain separate data-byte and color-nibble bus values. Color RAM is addressed with the acquisition address's low ten bits in both phases; only a matrix acquisition captures the character output color. Startup and inactive cycles, line and frame resets, snapshots and dumps do not update the chip's retained bus values. Complete initialization seeds data $ff and color $0f as deterministic emulator defaults.

The three unconnected video-view subsets use the compatibility expression previousData & ($f0 | previousColor), then the acquisition captures the current address's color nibble. Video color-RAM reads provide only their low nibble. CPU color-RAM reads retain the preceding placeholder implementation. CPU-driven changes of video-bus state and physical verification of the compatibility expression remain pending; this is not complete open-bus emulation. The separate CPU, VIC-I and combined subset collections are preserved.

No compilation or test execution was performed for this change, and existing fixtures do not validate the new bus model.

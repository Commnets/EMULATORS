# VIA timer regression

Component tests for the shared `VIATimer::simulate()`, the real T1/T2 PHI2 one-shot implementations, T2/PB6 integration and VIA NMI/IRQ request admission through `VIA::initialize()`, memory-mapped registers and the full VIA cycle loop. The continuous-mode oracle is MOS 6522 preliminary datasheet (November 1977), Figure 6: steady timeout-to-timeout period N + 2. The CPU cases verify whole-cycle read/write placement under the specified emulator ordering; physical half-cycle timing remains outside the suite.

Build from the repository root with Visual Studio MSBuild, in this order:

```powershell
$msbuild = 'C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe'
& $msbuild projects/CORE/CORE.vcxproj /p:Configuration=Debug /p:Platform=Win32
& $msbuild projects/F6500/F6500.vcxproj /p:Configuration=Debug /p:Platform=Win32
& $msbuild projects/COMMODORE/COMMODORE.vcxproj /p:Configuration=Debug /p:Platform=Win32
& $msbuild projects/VIC20/VIC20.vcxproj /p:Configuration=Debug /p:Platform=Win32
& $msbuild tests/TestVIATimer/TestVIATimer.vcxproj /p:Configuration=Debug /p:Platform=Win32
& ./exe/x86/TestVIATimerD.exe
```

Use the repository's SDL2/RakNet environment and 32-bit Debug runtime DLLs. The real VIA1 fixture additionally links VIC20D.lib and the same RakNet Debug Win32 import library used by VIC20. The test is registered in `Emulators.sln` under Libs/Micros/Tests and can also be built independently; ProjectReferences establish the framework dependency order. Failures return exit code 1 and identify the failed observation.

| Claim | Stimulus and observation |
| --- | --- |
| Continuous period | Real T1, latches 0, 1, 2, 255, 256, 22150, 65535; repeated periods 2, 3, 4, 257, 258, 22152, 65537 |
| Visible phases | Latch 2: 1, 0, 65535 with timeout, then latch reload without another event |
| PB7 and zero events | Shared signal-mode timer linked to a real VIAPortB; zero precedes continuous timeout, PB7 toggles at timeout, reload emits no event even for latch zero |
| Writes and acknowledgement | Interrupt acknowledgement preserves pending reload; latch-only write preserves active count and changes next reload; explicit counter load cancels pending reload |
| Reset, stop and mode changes | Cycle flags clear while stopped; initialization clears pending phase; continuous mode changes preserve reload; switching to one-shot discards pending continuous reload |
| One-shot compatibility | Real T1/T2 PHI2 countdown, continued counting without automatic reload or interrupt rearming, and explicit load rearming; shared one-shot PB7 timeout remains unchanged |
| T2/PB6 integration | ACR5 and T2 loaded through registers; fixed high/low and rising edges do not count; falling edges count once, IFR5 asserts at the existing timeout, T2C-L acknowledges without rearming across 65,536 further pulses; a new load rearms; clearing IFR5 by register write works; PHI2 source resumes cycle counting |
| VIA interrupt admission | Real 6502 interrupt system with VIA connected to NMI or IRQ; held NMI does not repeat, held IRQ keeps requesting, partial acknowledgement does not rearm NMI, final-source acknowledgement and IER masking release the output, enabling a pending source admits a fresh request, release preserves an already pending request |
| PCR readback | With SR disabled, all 256 PCR values survive normal reads and peeks; read/modify/write changes 0x10 to 0x11 without changing CB1/CB2; repeated reads and peeks preserve pending CA1/CB1 flags |
| Serial output ownership | External-clock zero/one conflicts with manual PCR levels; all eight CB2 PCR modes and ORB read/write accesses cannot drive serial CB2; disabling SR restores the latest PCR configuration; pre-existing CB2 flags survive handover |
| Serial output timelines | 128 cases: modes 4..7, eight CB2 PCR modes, both CB1 polarities, patterns 0xA5/0x5A; compare every wire notification and end-of-cycle level with specified clock/bit sequences; eight finite pulses then PCR5 idle, continuous/external output keeps shifting; IFR4 and PB latching follow selected clock edges |
| Serial input and peripheral recovery | 24 cases: modes 1..3 against all eight CB2 PCR modes; external data 0xA5, no PCR/ORB glitches or CB2 peripheral IRQ generation, eight-bit capture/IFR2, input remains input after completion; restored CB2 handshake/pulse and independent CA2 pulse |
| Same-mode ACR preservation | Differential runs for all seven active SR modes, all 32 combinations of ACR0/1/5/6/7 and every whole-cycle insertion point before, during and after a transfer; compare ordered wire transitions, CB1/CB2 levels, SR contents and IFR2; verify unrelated ACR fields still update |
| Actual mode changes and SR accesses | PHI2 output changed to external-clock output retains the existing counter-reset behavior; disabling clears IFR2 and restores PCR; selecting an internal mode waits for SR access; SR reads and writes still start complete eight-bit transfers |
| Real VIC20 VIA1 input capture | Real joystick movement/fire and cassette-sense events publish PA before any read; CA1 captures pressed state, release updates live pins while capture remains, repeated reads do not drive pins, a second edge captures release; both polarities; preserve PA0/PA1/PA7; $911f versus $9111 handshake and acknowledgement |
| VIA1 user port / point 9 overlap | PB0 low survives unlatched reads; captured PB remains readable after the external pins change without driving the captured value back into those pins; a later CB1 edge captures the new level, both polarities |
| Capture preparation | Generic CA1 capture resolves newly written OR/DDR before copying PA; PB capture does not count a PB6 falling edge twice or retain it for another T2 decrement |
| T1 PB7 effective direction | 80 cases: one-shot/continuous timer output, five DDRB masks, four ORB patterns, latching on/off; physical pins and read/peek follow T1 while stored DDRB remains unchanged; ORB7 writes cannot override T1; removing ACR7 restores ordinary ORB or external/captured input |
| PB7 input preservation and compatibility | A raw PB0 bus change while T1 drives PB7 high cannot overwrite the retained low PB7 external input; PB7 toggles do not count as PB6 edges; ordinary PB7 input/output combinations remain valid with ACR7 disabled |
| Real VIA2 keyboard / PB7 | A key in column 7 follows T1's low/high output in one-shot and continuous modes, with stored DDRB7 either zero or one; this is the timer-direction correction, not the shared matrix correction documented below |
| Shared VIA2 matrix resolution | Real joystick-right event with PB7 input selects a held key's row; normal reads and peeks agree; all 64 switches scan in both directions and ignore unselected lines; release restores highs |
| Multi-key digital settling | Fifteen pressed switches connect all sixteen pins to the joystick low source; opening the middle switch releases one half; removing the source releases the remaining network; clock counter is unchanged by reads/settling; configured T1 output retains priority |
| Matrix latch/handshake/PB6 | Live rows/columns update while captured values remain readable; peeks retain CA1/CB1 flags, ordinary reads acknowledge, $912f avoids handshake; repeated matrix reads do not multiply keyboard-driven PB6 edges |

## Current coverage and limits

Mode changes during pending reload assert the agreed emulator ordering; they are not independent hardware measurements. The suite covers physical PB7 direction/readback, port capture, matrix resolution, timed CPU writes (including RMW dummy/final writes), advanced operand reads and selected real NMI-entry behavior. The interrupt-admission fixtures inspect pending requests rather than executing a handler or RTI; the timed-write fixture separately executes real NMI entry.

Physical half-cycle timing, external-clock silicon errata, complete interrupt-handler execution, global cross-chip signal interleaving and PITFALL rendering require separate verification. Passing these tests does not establish that the missing scorpion has been corrected. The final section lists the access patterns the advance-to-read-point model does not solve.

## Regression history

The following notes record the scope and results at each implementation stage. Later sections extend that coverage.

All integration fixtures now allocate independent register/storage objects dynamically and use normal Memory ownership; the former static-zero-initialization workaround has been removed. The PB6 case keeps IER disabled. The interrupt-admission cases enable T1/T2 and check the real CPU pending-request queue and admission state; they do not execute the interrupt handler or RTI. Direct component fixtures issue writes before whole-cycle simulation steps; the later CPU fixtures place buffered writes inside actual instructions.

The NMI regression failed before the fix at 'Releasing the final enabled source must rearm NMI' and passes with the fix. Release/reassertion must be observable on separate simulated cycles; a transition hidden between CPU/VIA observations remains outside this test's timing model.

The PCR regression failed before the fix at value 16 (0x10); CB1 polarity belongs to bit 4, independently of CB2 mode bits 7..5. The test uses the public ChipRegisters::peekRegister() path as well as memory-mapped reads. CPU read/modify/write bus timing remains outside this component test.

The serial ownership regression failed before the fix at 'PCR must not overwrite serial zero while no shift clock arrives'. The MOS November 1977 datasheet, PDF p. 11 Figure 11 (visually inspected), documents finite output returning to PCR control after the final clock pulse; PDF p. 15 explicitly retains CB1 interrupt detection in serial operation. Tests record every CB1/CB2 Wire notification, including intra-cycle glitches. They retain the existing SR clock divider and whole-cycle shift/IFR2 phase; they do not establish exact NMOS setup/hold times or external-clock errata. The separate same-mode ACR regression is described below.

The same-mode ACR test failed before the guard in setMode(): mode 1, identical ACR written immediately after the SR load, first divergence at cycle 4 (missing CB1 transition). Differential comparisons verify the invariance of the serial subsystem under unrelated ACR writes, supplementing the independently specified waveform tests above. The stimulus includes the final low half-cycle and completed transfers with pending IFR2; it does not validate subcycle access timing or establish new behavior for actual mode changes.

The real VIA1 regression failed before the fix at 'VIA1 event must publish live joystick/tape pins before any register read'. The fixture sends events through a Notifier observed by VIC20::VIA1; it does not invoke the input-state setters directly. CA1/CB1 test edges are applied through the public wires, so this does not implement RESTORE keyboard routing or test analog signal timing. The point 8 change left VIA2 keyboard scanning outside its scope; the shared digital matrix is covered by the later point 11 tests below. Analog contention, IEC connection implementation and subcycle CPU/VIA access placement remain outside these capture fixtures. The later CPU fixtures cover whole-cycle access placement. Removing VIA1's input-writing read override also resolves the specific point 9 defect that replaced user-port input with pull-ups during reads.

Point 8 verification used Debug|Win32 throughout. Windows held the normal TestVIATimerD.exe output open (LNK1168; no active test process found), so the executable was built with /p:TargetName=TestVIATimerCaptureD and the full suite was run as ./exe/x86/TestVIATimerCaptureD.exe. That run passed all existing checks, including 10,336 same-mode ACR cases, plus the new real-VIA1 and capture-preparation cases. VIC20EmulatorD.exe was also linked successfully.

The point 10 regression failed before the fix at 'T1 must drive physical PB7 independently of DDRB7'. The supplied MOS November 1977 datasheet states that PB7 becomes an output if DDRB7 or ACR7 is set. Effective direction is derived from that rule without modifying stored DDRB. The test retains existing one-shot/continuous timeout phases and checks both live pin values and CPU-visible input-latched versus output bits; subcycle publication timing remains outside its scope.

Point 10 verification: COMMODORE, VIC20 and VIC20Emulator built successfully in Debug|Win32. The full suite ran as TestVIATimerCaptureD.exe with exit code 0: 80 PB7 direction/ORB/latching scenarios, ordinary input/output combinations, retained external input and PB6 independence, plus four real-VIA2 keyboard cases and all preceding regressions passed.

Point 11 baseline failed at 'Joystick right must pull a keyboard row low through input column PB7'. Expected single-key results and the sixteen-pin/cut-chain masks are specified independently of the settling loop. The implemented model propagates low levels bidirectionally through pressed switches while configured outputs retain priority; it is not an electrical contention model. Sixteen settling iterations are a computational bound and do not advance CPU/VIA time. Inputs are refreshed on port read/peek and initialization, as proposed; this change does not guarantee matrix publication before a capture edge without an intervening port access. Generic port resolvedValue() only computes a value; register peeks publish the computed matrix but do not acknowledge or handshake.

Point 11 verification: COMMODORE, VIC20 and VIC20Emulator built successfully in Debug|Win32. TestVIATimerCaptureD.exe was locked for writing (LNK1168), so the same project was linked with /p:TargetName=TestVIATimerMatrixD. Running ./exe/x86/TestVIATimerMatrixD.exe returned exit code 0: the joystick regression, all 64 forward/reverse key scans, simultaneous-column selection, sixteen-pin propagation/cut/release, live-versus-captured read/peek semantics, PB6 checks and all earlier regressions passed.

Point 12 preserves initializeInternalValues() in all three constructors and in initialize(). The nine non-owning component links explicitly start at nullptr. VIA1 joystick/tape and VIA2 joystick/both keyboard matrices reset before the connection guard, so local initialization works before wiring and register/pin initialization remains deferred until wiring exists.

| Construction/reset claim | Stimulus and observation |
| --- | --- |
| No dependence on zero-filled allocation | Placement construction of generic, VIA1 and VIA2 registers in aligned raw storage filled with 0xA5; all nine generic links are null; local inputs have released defaults |
| Safe initialization before wiring | initialize() on all three unconnected register classes; links remain null; changed VIA2 joystick/key state returns to defaults |
| Reusable connected initialization | Real VIA1/VIA2 on independently heap-allocated memory; alter joystick/tape/all keys and DDR/ACR/PCR, then initialize the chip again; machine defaults and released inputs are restored |
| Existing behavior | Run the complete preceding regression suite with ordinary Memory ownership and destruction |

Point 12 verification: COMMODORE, VIC20, VIC20Emulator and TestVIATimer built successfully in Debug|Win32. The regression executable was linked with /p:TargetName=TestVIATimerConstructionD to avoid previously locked executable names.
Running ./exe/x86/TestVIATimerConstructionD.exe returned exit code 0: nonzero-storage construction, unconnected initialization, connected VIA1/VIA2 reset and the entire preceding regression suite passed.

Point 4, writes: VIA now captures immutable CPU transaction bus metadata, extracts only its own buffered register commands and commits each command at its zero-based write-cycle offset. CommodoreVIC20 enables this path for both VIAs and forwards instruction/interrupt start contexts. Complete-instruction CPU execution with buffered registers is required; activation rejects the current per-cycle mode. Generic direct-access fixtures retain the existing path. No CPU, memory command, register decoder or timer arithmetic changes are introduced.

The cycle contract is: existing internal VIA work, consume its transitions, apply the CPU write, publish port effects, evaluate IRQ/NMI output. A loaded counter is not decremented in the write cycle. A write-generated PB6 edge remains pending for the next sample. The legacy CA2/CB2 one-shot hold guard is cleared after a late write because its same-cycle simulation has already occurred; pin levels are retained, and pulses finish on the following cycle. This adjustment was required by the new test, which initially failed at "CA2 pulse must finish on the following cycle".

| Timed-write claim | Preconditions, stimulus and exact oracle |
| --- | --- |
| First transaction and timer load | Real C6502, buffered VIA, initial counter 0x20; STA absolute commits at S+3 with old counter 0x1C and loaded counter 0x120; next cycle is 0x11F, for T1 and T2 |
| Clock origin | Explicit generic CPU reset gives S=0; normal C6500 initialization gives S=6 and first STA commits at absolute cycle 9 |
| RMW order | Real INC absolute: original 0x20 at S+4, modified 0x21 at S+5 |
| Subset isolation and once-only effects | Following VIA1 store commits at cycle 9 after the six-cycle VIA2 INC; no replay in VIA2, NOP adds no write |
| Latch preservation | STA low latch changes the latch while the counter advances from 0x20 to 0x1C |
| PB6 | ORB falling edge at the write leaves T2 unchanged that cycle, decrements once on the next, and never repeats |
| Handshake/PCR | CA2 and CB2 pulses begin on their port writes, finish next cycle; timed PCR change and ORA-without-handshake behave separately |
| ACR timing | T2 retains PHI2 counting for all four STA cycles; the new PB6 source applies afterwards without an invented edge |
| IFR/IER | A timeout generated earlier in the instruction is acknowledged at the write; masking retains its flag and removes enabled status |
| Interrupt context | Real NMI entry after a store generates no VIA writes and permits a following store |
| Reset | Reinitialize both VIAs and reactivate scheduling; no old context or command is replayed |
| Unsupported configuration | Per-cycle CPU and unbuffered VIA register subset are rejected |

The counter/write/phase expectations above are whole-cycle scheduling assertions. They do not establish NMOS half-cycle set/clear priority or first-load setup/hold timing. At that write-only stage, normal VIA reads used the unsynchronized path; the advance-to-read-point extension below addresses the common final-operand cases. CPU bus reads, per-cycle CPU integration, cross-chip signal interleaving and PITFALL rendering remain outside this change.

Build/run this version with /p:Configuration=Debug /p:Platform=Win32 /p:TargetName=TestVIATimerBusD and ./exe/x86/TestVIATimerBusD.exe. Windows returned LNK1168 when relinking earlier executed test names, hence the distinct output name.
Verification: COMMODORE, VIC20 and VIC20Emulator built in Debug|Win32; TestVIATimerBusD completed the timed-write cases and all previous regressions with exit code 0, including 10,336 same-mode ACR comparisons.
The stricter NMI case supplies an edge at the previous instruction polling point and a known vector $0300; it asserts exactly seven launch cycles and the vector destination before checking the following store. The earlier end-of-instruction edge correctly waits for another polling opportunity, so it is unsuitable as an immediate-launch stimulus.
Final verification: ./exe/x86/TestVIATimerBusVerifiedD.exe passed the stricter NMI launch assertion and the full suite with exit code 0. VIC20EmulatorD.exe linked successfully against the final Debug|Win32 libraries.

## Advance to the instruction read point

In complete-instruction mode, CPUAboutToExecute selects the predicted bus structure and advances the VIA through lastReadCycle+1 internal steps, capped at firstWriteCycle. Earlier cycles finish normally; the final advanced cycle postpones port-access completion and IRQ/NMI publication until after the instruction's ordinary reads. The normal simulate() first extracts the buffered writes, closes the pending cycle after the CPU read, then applies due writes while running only the remaining cycles. Context and counters reset at transaction completion or initialize(). Interrupt-entry contexts retain the existing no-advance path.

No new CPU events, opcode handlers, memory decoder changes or register-read overrides are introduced. Logging receives an absolute cycle because the CPU clock has not yet advanced during the instruction-start callback.

| Claim | Independent whole-cycle stimulus/oracle |
| --- | --- |
| T1/T2 low byte | Counter 6, LDA absolute: A=2 and counter=2 after four cycles; one following idle cycle produces 1 |
| Zero flag and dependent control flow | Continuous T1 counter 4: LDA reads 0 and sets Z; following BEQ takes its three-cycle branch |
| T1/T2 high byte | Counter 0x0102: four cycles produce 0x00FE and the high-byte read returns 0 |
| Indexed variants | LDA absolute-X targets $9124 with and without page crossing; four/five elapsed cycles produce counter/data 5/4 from initial 9 |
| Branch variants | BNE taken/not taken, with/without page crossing: exact two/three/four-cycle duration, PC and timer difference |
| Final-cycle acknowledgement | One-shot timeout at cycle 3, acknowledged by the final LDA T1C-L read: no spurious IRQ or NMI request |
| Earlier-cycle request | Timeout at cycle 2, followed by acknowledgement at cycle 3: retain the request and its original timestamp 2 |
| IFR | A timeout before the final read is visible in A and IFR remains asserted after reading it |
| RMW | INC T1C-L reads 2 from initial counter 6; writes original 2 at cycle 4 and modified 3 at cycle 5; counter reaches 0 once and the later timeout remains pending |
| Port read | Normal PA read acknowledges CA1 and starts CA2 pulse; next cycle ends pulse; ORA-without-handshake retains CA1 and starts no pulse |
| SR | PHI2 output A5 rotates to 4B then 96 on cycles 0 and 2; LDA SR returns 96; read restarts transfer and next cycle produces 2D |
| Pre-write boundary | Real JSR, whose reads are not all before its writes, completes correctly without advancing through stack writes |
| Invalid prediction | Predicted four cycles advanced, actual clock advances only two: simulation returns CHIP_ERROR instead of unsigned remaining-count underflow |

This is a whole-instruction advance model for accesses represented by the current CPU. Multiple reads of VIA registers at different bus cycles within one instruction (for example JMP indirect with its pointer in VIA I/O), missing dummy reads, execution of instruction bytes from I/O, un-timestamped external input histories, global cross-chip phase ordering and physical half-cycle event priority are not solved by this change. The enabled mode still explicitly rejects per-cycle CPU execution.
Verification: COMMODORE, VIC20, VIC20Emulator and TestVIATimer built successfully in Debug|Win32. ./exe/x86/TestVIATimerD.exe returned exit code 0 for all advanced-read cases, the existing timed-write cases and every previous regression, including 10,336 same-mode ACR comparisons.

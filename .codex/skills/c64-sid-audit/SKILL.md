---
name: c64-sid-audit
description: Validate Commodore 64 SID 6581/8580 sound emulation in EMULATORS, including $D400-$D7FF register decoding, voices, frequency, pulse width, control bits, ADSR/envelope behavior, oscillator sync/ring/noise/test, filters, volume DAC/sample clicks, paddles, read-only registers, mirrors, and reSID integration. Use when Codex reviews or edits SID, Sound, audio wrapper, register, filter, waveform, envelope, or mapped SID behavior.
---

# C64 SID Audit

## Core Workflow

Use this skill to audit SID behavior while respecting that a large part of the chip is mixed-signal and model-dependent.

1. Read `references/sid.md` before making claims or edits.
2. Inspect the local implementation paths first:
   - `include/COMMODORE/SID/SID.hpp`, `src/COMMODORE/SID/SID.cpp`
   - `include/COMMODORE/SID/SIDRegisters.hpp`, `src/COMMODORE/SID/SIDRegisters.cpp`
   - `include/COMMODORE/SID/SIDLibWrapper.hpp`, `src/COMMODORE/SID/SIDLibWrapper.cpp`
   - `include/C64/Sound.hpp`, `src/C64/Sound.cpp`
   - `thirdparties/resid/*`
3. When changing C++ code, also use `$emulators-framework-cpp-style`.
4. Distinguish digital register semantics from analog sound character. Register behavior can be tested exactly; filter tone, distortion, and DAC nonlinearity are fidelity targets.
5. Prefer delegating low-level waveform/envelope/filter generation to reSID unless the existing architecture intentionally wraps or constrains it.

## Audit Priorities

Prioritize correctness in this order:

- Address decoding and mirrors across $D400-$D7FF.
- Voice register layout for all three voices and correct write-only/read-only behavior.
- Control register side effects: gate, sync, ring modulation, test, and waveform selection.
- ADSR state transitions, envelope readback, and known envelope corner cases.
- 24-bit oscillator phase, noise LFSR behavior, pulse width, combined waveforms, and voice 3 oscillator readback.
- Filter cutoff/resonance/routing, $D418 mode/volume, 6581 versus 8580 differences, and sample-click/digifix behavior.
- Paddle reads through $D419/$D41A and interaction with CIA/control port selection.

## Validation Hints

For register-level bugs, create small write/read tests. For audible fidelity, compare generated samples against reSID behavior or a known-good C64 emulator with the same SID model and sample rate.

Useful local anchors:

- `thirdparties/resid/sid.cc`
- `thirdparties/resid/voice.cc`
- `thirdparties/resid/wave.cc`
- `thirdparties/resid/envelope.cc`
- `thirdparties/resid/filter.cc`

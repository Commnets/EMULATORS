---
name: msx-sound-audit
description: Validate MSX PSG sound emulation in EMULATORS, including AY-3-8910/YM2149 ports #A0-#A2, register latch behavior, tone/noise/envelope registers, mixer and amplitude semantics, period-zero handling, GPIO registers 14/15 for joystick/cassette/Kana wiring, BIOS PLAY compatibility, and AY versus YM output differences. Use when Codex reviews or edits MSX PSG, GENERALINSTRUMENTS AY38910 classes, Sound, PSG port manager, joystick GPIO, cassette input, or audio generation.
---

# MSX Sound Audit

## Core Workflow

Use this skill to audit whether EMULATORS models the MSX PSG as both a sound generator and an MSX-wired GPIO device.

1. Read `references/sound.md` before making claims or edits.
2. Inspect the relevant implementation paths first:
   - `include/MSX/PSG.hpp`, `src/MSX/PSG.cpp`
   - `include/MSX/Sound.hpp`, `src/MSX/Sound.cpp`
   - `include/MSX/PortManager.hpp`, `src/MSX/PortManager.cpp`
   - `include/GENERALINSTRUMENTS/AY38910/AY38910.hpp`, `src/GENERALINSTRUMENTS/AY38910/AY38910.cpp`
   - `include/GENERALINSTRUMENTS/AY38910/AY38910Registers.hpp`, `src/GENERALINSTRUMENTS/AY38910/AY38910Registers.cpp`
   - `include/GENERALINSTRUMENTS/AY38910/AY38910LibWrapper.hpp`, `src/GENERALINSTRUMENTS/AY38910/AY38910LibWrapper.cpp`
3. When changing C++ code, also use `$emulators-framework-cpp-style`.
4. Separate register-level correctness from analog/audio character. Register and GPIO behavior must be exact; volume curves and chip-family tone are fidelity targets.
5. When touching registers 14/15, also consider `$msx-system-audit` because those bits wire to joystick, cassette, keyboard layout, and LEDs.

## Audit Priorities

Prioritize correctness in this order:

- PSG port protocol: #A0 selects register, #A1 writes selected register, #A2 reads selected register.
- Register masking for tone period high nibbles, noise period, mixer, amplitude, envelope period, and envelope shape.
- Period-zero behavior for tone, noise, and envelope counters.
- Mixer semantics: tone/noise disable bits and MSX-required I/O direction bits.
- Amplitude register bit 4 envelope enable and lower 4-bit volume.
- AY/YM envelope shape restart and hold/alternate/attack/continue behavior.
- GPIO register 14/15 MSX wiring for joystick, cassette input, layout bit, port select, trigger outputs, and Kana LED.
- Sample generation clock rate, channel mix, and AY-3-8910 versus YM2149 differences.

## Validation Hints

For register bugs, use direct I/O tests against #A0-#A2 and avoid judging by audio alone. For GPIO bugs, test joystick/cassette reads through PSG register 14 and writes to register 15. For audible bugs, compare short generated samples at the same PSG clock and sample rate.

If a PLAY/BASIC symptom is involved, inspect BIOS-visible queue and interrupt behavior as well as the PSG registers.

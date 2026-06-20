# C64 SID Reference

## Sources

- Local PDF: `.codex/docs/SID.pdf` (13 pages). The uploaded PDF has little/no extractable text with pypdf, so use it as a visual datasheet source when exact tables or diagrams are needed.
- Web: C64-Wiki SID: https://www.c64-wiki.com/wiki/SID
- Datasheet link to verify manually if needed: https://archive.6502.org/datasheets/mos_6581_sid.pdf
- Web summary/source index: https://en.wikipedia.org/wiki/MOS_Technology_6581
- Local code reference: `thirdparties/resid/*`.

## SID Character

The SID is mixed digital/analog. Digital register semantics should be exact. Analog fidelity depends on SID model, revision, filter curves, DAC nonlinearity, external capacitors, and board-level behavior.

The C64 commonly uses:

- 6581 in earlier C64/C128 plastic models, with 12 V analog supply and strong nonlinear filter/distortion behavior.
- 8580/6582 in later C64C/C128DCR models, with 9 V analog supply, different filter behavior, and much quieter $D418 sample clicks unless digifix hardware is modeled.

## Addressing

SID is CPU-visible at $D400-$D7FF when I/O is visible. It internally decodes low 5 address bits, so the 32-byte register window mirrors through the range.

Only $D400-$D41C are meaningful:

- Most registers are write-only from the CPU perspective.
- $D419/$D41A read paddle X/Y.
- $D41B reads voice 3 oscillator output.
- $D41C reads voice 3 envelope output.
- Reads from write-only or unused SID mirrors should follow the emulator's bus/open behavior policy, not return a stored register value unless deliberately documented.

## Register Layout

Voice 1:

- $D400/$D401: frequency low/high.
- $D402/$D403: pulse width low/high, 12-bit effective value.
- $D404: control: gate, sync, ring, test, triangle, sawtooth, pulse, noise.
- $D405: attack/decay.
- $D406: sustain/release.

Voice 2 repeats at $D407-$D40D. Ring/sync source is voice 1 for voice 2.

Voice 3 repeats at $D40E-$D414. Ring/sync source is voice 2 for voice 3; voice 1 uses voice 3 as source.

Filter and global registers:

- $D415: filter cutoff low bits.
- $D416: filter cutoff high bits.
- $D417: resonance and voice/external-input routing to filter.
- $D418: voice 3 mute, high-pass, band-pass, low-pass, and 4-bit master volume.
- $D419/$D41A: paddle X/Y read.
- $D41B/$D41C: voice 3 oscillator/envelope read.

## Digital Behavior Checklist

- Frequency uses a 24-bit phase accumulator: output pitch is proportional to register value and chip clock / 2^24.
- Pulse width is 12-bit. Extremes have hardware-specific behavior; do not clamp casually without checking reSID or hardware references.
- Gate bit transitions drive ADSR attack/decay/sustain/release states.
- Attack/decay/release rates are encoded as 4-bit values with SID-specific timing tables.
- Sustain is a 4-bit level, not a rate.
- Sync resets the oscillator against the source oscillator.
- Ring modulation applies to triangle waveform behavior using the source oscillator.
- Test bit resets/holds oscillator state and affects noise/pulse behavior.
- Noise is generated from a shift-register style source and is not generic random noise.
- Combined waveforms are chip/model dependent, especially on 6581, and should usually be delegated to reSID tables if available.

## Analog/Fidelity Checklist

- $D418 volume changes can create audible 6581 sample clicks; 8580 behavior differs.
- Filter cutoff/resonance response is highly model dependent. 6581 and 8580 should not share one curve unless the emulator only targets approximate sound.
- Voice 3 mute in $D418 removes voice 3 from direct output but may still allow filter/routing side effects depending implementation.
- External input routing matters if the emulator exposes audio-in or digifix-like behavior.
- Paddle reads may be handled outside the audio path but are SID registers and should be wired to control-port/paddle selection.

## reSID Integration

Before modifying custom SID logic, check whether `thirdparties/resid` already implements the behavior. Prefer a thin, correct register/bus wrapper around reSID over reimplementing oscillator/envelope/filter internals.

Review these files when needed:

- `thirdparties/resid/sid.cc`
- `thirdparties/resid/voice.cc`
- `thirdparties/resid/wave.cc`
- `thirdparties/resid/envelope.cc`
- `thirdparties/resid/filter.cc`
- `thirdparties/resid/pot.cc`

## EMULATORS Review Checklist

- Does `SIDRegisters` decode only low 5 bits and mirror correctly through $D400-$D7FF?
- Are read-only registers sourced from the live SID engine rather than stale written state?
- Are write-only reads intentionally open-bus, last-bus, or masked, and is that policy consistent with other I/O?
- Does `SIDLibWrapper` propagate all register writes in order and at the correct emulated clock time?
- Does `C64::Sound` select SID model, clock rate, sample rate, and filter settings explicitly?
- Are paddle inputs routed through SID reads and CIA/control-port selection consistently?

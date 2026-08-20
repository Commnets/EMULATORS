# I/O Contract

## Interface Definition

Record address or selector, data width, direction, tri-state/open-collector behavior, pull-ups, polarity, idle state, sampling and drive edges, debounce or scan timing, handshakes, interrupts, reset, and absent-device behavior.

## Protocol State Machine

For every state record its entry event, preconditions, changed lines/registers/buffers, CPU-visible observation, timeout or clock progression, valid exits, abort paths, and reset behavior. Represent host data as queued events or immutable streams consumed at emulated sampling points.

## Layer Decisions

Evaluate machine interface, deterministic device simulation, media format, and host adapter independently. Reuse a layer only when its protocol or wiring contract matches; adapt board-specific differences explicitly.

## Test Cases

Cover every selector and direction; absent devices; simultaneous drivers; normal, empty, partial, aborted, timed-out, EOF, and reset transfers; corrupt media; write protection; host events around sampling boundaries; repeated reset; and disconnect/reconnect.


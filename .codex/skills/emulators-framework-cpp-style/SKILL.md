---
name: emulators-framework-cpp-style
description: Apply the local C++ coding conventions of the EMULATORS framework. Use when Codex adds, edits, reviews, or scaffolds C++ code in this repository, especially new classes, headers, cpp files, CORE/COMMODORE module code, emulator components, registers, chips, memory, sound, IO, commands, or InfoClass-derived objects.
---

# EMULATORS Framework C++ Style

## Core Workflow

Before writing code, inspect nearby headers and cpp files in the same module. Prefer the nearest local pattern over generic modern C++ style.

Use the framework's existing idioms deliberately:

- Keep the Doxygen file banner, include guard shape, namespace, and end-of-file marker used by neighboring headers.
- Use explicit `MCHEmul::`/module qualification in `.cpp` definitions. Do not add `using namespace`.
- Do not create free functions, file-local helper functions, or anonymous-namespace functions in headers or `.cpp` files. General free functions are allowed only in the existing `global.hpp` and `global.cpp`. Every other new operation must be a method of an appropriate class, using a private or static method when it is only an implementation helper. If no suitable owning class is clear, stop and ask the user where it belongs before writing it.
- Indent with tabs, not spaces, including proposed snippets and patches. Preserve existing tab-based continuation alignment from nearby code.
- When creating or editing source/code files, always write CRLF (`\r\n`) line endings for every line, and preserve or convert touched generated code snippets and file edits to CRLF.
- When proposing or writing code, include comments in the framework's local style only when they explain why the code exists or clarify non-obvious hardware/framework behavior. Avoid comments that merely restate what the code says.
- For emulator methods that implement protocol, hardware, DOS, filesystem, media-layout, or state-machine behavior, add moderate in-method comments explaining why state transitions, status codes, buffer resets, compatibility choices, traversal guards, and data mutations exist.
- For long or functionally complex methods, comment the main phases with a density comparable to the existing `buildAnswerToDirCommand`: validation, syntax parsing, linked-structure traversal, cycle/corruption guards, allocation/mutation, status/EOF decisions, and compatibility limits.
- Design proposed and implemented code for maximum runtime performance, especially in emulator hot paths. Preserve correctness while minimizing allocations, copies, repeated calculations, lookups, branches, virtual calls, and other per-cycle overhead; read the performance guidance in `references/cpp-style.md` before changing execution-critical code.
- When a performance optimization makes a line or construct complex or non-obvious, add a nearby comment explaining how it works, the invariant it relies on, and why it avoids slower work. Do not leave optimized code unexplained.
- Preserve the spacing style: `name ()`, `std::vector <T>`, `static_cast <T>`, `_ptr -> method ()`, `return (value);`.
- Use the fundamental integer types already established by the framework, such as `char`, `unsigned char`, `short`, `unsigned short`, `int`, and `unsigned int`. Do not introduce fixed-width aliases from `<cstdint>` such as `int8_t`, `uint16_t`, `int32_t`, or `int64_t`. Use `size_t` for sizes, memory positions, and indices into arrays or containers.
- Do not introduce SDL width aliases such as `Sint8`, `Uint8`, `Sint16`, `Uint16`, `Sint32`, or `Uint32` when a fundamental integer type can be used. Keep semantic SDL types such as `SDL_AudioFormat` and `SDL_AudioDeviceID` where they express an API domain rather than only an integer width.
- Put non-trivial method implementations in `.cpp` and separate them with `// ---`.
- Define extremely short methods, consisting of a single instruction or expression, directly with their declaration inside the class and omit the `inline` keyword.
- For short methods that are slightly longer than one instruction or a couple of lines, declare them `inline` inside the class and place their definition immediately after the class definition in the same header.
- Avoid introducing a local `const` variable for a transient result used in only one formula. Substitute the expression directly unless it is long or complex enough that naming it materially improves readability. Keep a local `const` result when it is reused in several calculations.
- Use initializer lists for constructors and initialize members in declaration order.
- Use `InfoClass` conventions when applicable: pass the class name to the base constructor, override `getInfoStructure () const`, start with the parent `InfoStructure`, add uppercase field names, and document returned attributes/substructures in the `.cpp`.
- Use raw pointers and explicit ownership only where the framework already does. If a class owns pointers, document ownership and delete them in the destructor.
- Add `assert` checks for required non-null dependencies and impossible modes, matching local debug-only validation.
- Do not perform unrelated modernization, formatting sweeps, or broad refactors while adding code.

## Detailed Reference

For new classes, file templates, naming rules, member layout, and implementation patterns, read `references/cpp-style.md`.

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
- In multi-line Doxygen comments, end every line that must render separately with a period, one space, and `\n`; for example, `Description of the first line. \n`.
- For emulator methods that implement protocol, hardware, DOS, filesystem, media-layout, or state-machine behavior, add moderate in-method comments explaining why state transitions, status codes, buffer resets, compatibility choices, traversal guards, and data mutations exist.
- For long or functionally complex methods, comment the main phases with a density comparable to the existing `buildAnswerToDirCommand`: validation, syntax parsing, linked-structure traversal, cycle/corruption guards, allocation/mutation, status/EOF decisions, and compatibility limits.
- Design proposed and implemented code for maximum runtime performance, especially in emulator hot paths. Preserve correctness while minimizing allocations, copies, repeated calculations, lookups, branches, virtual calls, and other per-cycle overhead; read the performance guidance in `references/cpp-style.md` before changing execution-critical code.
- When a performance optimization makes a line or construct complex or non-obvious, add a nearby comment explaining how it works, the invariant it relies on, and why it avoids slower work. Do not leave optimized code unexplained.
- When adding or changing deep-debug output, preserve the framework's structured block format used by `debug*Cycle` methods: emit a `writeCompleteLine (...)` whose attributes are grouped into stable, semantically named blocks, and build each block value by concatenating its related `Field=value` entries. Extend the nearest existing block when the new state belongs there; create a new block only for a distinct and diagnostically relevant concern. Keep transient narrative messages in `writeLineData (...)`, not hardware state that later log analysis must correlate. Read the deep-debug guidance in `references/cpp-style.md` before changing debug methods.
- Whenever a `MCHEmul::Address` is converted to text with `asString (...)` using `'\0'` as the separator between its `UByte` values, and that string is intended for a textual output channel such as deep debug, logging, `std::cout`, a formatter, or a console, pass the result through `MCHEmul::removeAll0 (...)` before concatenating or emitting it. Embedded null characters must never reach textual output.
- Preserve the spacing style: `name ()`, `std::vector <T>`, `static_cast <T>`, `_ptr -> method ()`, `return (value);`.
- Declare variables of the same type together when their initializers are all
  simple: absent, a literal or constant, or a previously available variable or
  member; for example, `int a = 0, b = previous, c;`. If any initializer is long
  or complex, declare at least that variable separately. The remaining variables
  with simple initializers may stay grouped when declaration order and
  dependencies permit it. Never force a function call, compound expression,
  conditional expression, or similarly non-trivial construction into a comma-
  separated declaration merely to save lines.
- Omit braces around the body of `if`, `else`, `for`, `while`, and similar
  control statements when that body contains exactly one instruction.
  Keep braces only when the body has several instructions.
- Treat `switch` cases as a coordinated group. Uniform, very simple one-line
  actions may use the compact form `case 2: a = 3; break;`. If any peer case
  needs a braced block because it contains several instructions, declarations,
  or a complex action, use braced blocks for the other action-bearing cases in
  that switch as well, even when an individual case would not require one.
- Use the fundamental integer types already established by the framework, such as `char`, `unsigned char`, `short`, `unsigned short`, `int`, and `unsigned int`. Do not introduce fixed-width aliases from `<cstdint>` such as `int8_t`, `uint16_t`, `int32_t`, or `int64_t`. Use `size_t` for sizes, memory positions, and indices into arrays or containers.
- Do not introduce SDL width aliases such as `Sint8`, `Uint8`, `Sint16`, `Uint16`, `Sint32`, or `Uint32` when a fundamental integer type can be used. Keep semantic SDL types such as `SDL_AudioFormat` and `SDL_AudioDeviceID` where they express an API domain rather than only an integer width.
- Put non-trivial method implementations in `.cpp` and separate them with `// ---`.
- Define extremely short methods whose body is one simple instruction or expression directly with their declaration inside the class and omit the `inline` keyword. Treat direct member access, assignment, or an equivalently compact operation as simple.
- Do not classify a method as simple only because its body contains one syntactic instruction. If that instruction has a long or compound expression, several logical or arithmetic clauses, nested calls or casts, or needs several source lines, declare the method `inline` inside the class and place its definition immediately after the class definition in the same header. Apply the same form to short methods containing a couple of instructions.
- Avoid introducing a local `const` variable for a transient result used in only one formula. Substitute the expression directly unless it is long or complex enough that naming it materially improves readability. Keep a local `const` result when it is reused in several calculations.
- Use initializer lists for constructors and initialize members in declaration order.
- Use `InfoClass` conventions when applicable: pass the class name to the base constructor, override `getInfoStructure () const`, start with the parent `InfoStructure`, add uppercase field names, and document returned attributes/substructures in the `.cpp`.
- Use raw pointers and explicit ownership only where the framework already does. If a class owns pointers, document ownership and delete them in the destructor.
- Add `assert` checks for required non-null dependencies and impossible modes, matching local debug-only validation.
- Do not perform unrelated modernization, formatting sweeps, or broad refactors while adding code.
- Treat user-facing documentation and formatter output as part of the interface contract. When changing a C64, VIC-20, C264, ZX80, ZX81, ZX Spectrum, or MSX emulator startup option, `LocalConsole` operation, `CommandBuilder` registration, command syntax/behavior, or command `InfoStructure`, review and update the corresponding maintained guide: `docs/C64Emulator_UserGuide.docx`, `docs/VIC20Data/VIC20Emulator_UserGuide.docx`, `docs/C264Data/C264Emulator_UserGuide.docx`, `docs/ZX81Data/ZX81Emulator_UserGuide.docx`, `docs/ZXSpectrumData/ZXSpectrumEmulator_UserGuide.docx`, or `docs/MSXData/MSXEmulator_UserGuide.docx`. Audit the complete effective command-builder chain (including the currently duplicated standard-builder link in the MSX entry points), distinguish local-only commands from the non-console remote channel, and synchronize canonical `.fmt` files under `projects` when the visible structure changes.

## Detailed Reference

For new classes, file templates, naming rules, member layout, and implementation patterns, read `references/cpp-style.md`.

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
- Indent with tabs, not spaces, including proposed snippets and patches. Preserve existing tab-based continuation alignment from nearby code.
- When proposing or writing code, include comments in the framework's local style only when they explain why the code exists or clarify non-obvious hardware/framework behavior. Avoid comments that merely restate what the code says.
- Preserve the spacing style: `name ()`, `std::vector <T>`, `static_cast <T>`, `_ptr -> method ()`, `return (value);`.
- Put non-trivial method implementations in `.cpp` and separate them with `// ---`.
- Keep small accessors and trivial mutators inline in the header when the surrounding class does.
- Use initializer lists for constructors and initialize members in declaration order.
- Use `InfoClass` conventions when applicable: pass the class name to the base constructor, override `getInfoStructure () const`, start with the parent `InfoStructure`, add uppercase field names, and document returned attributes/substructures in the `.cpp`.
- Use raw pointers and explicit ownership only where the framework already does. If a class owns pointers, document ownership and delete them in the destructor.
- Add `assert` checks for required non-null dependencies and impossible modes, matching local debug-only validation.
- Do not perform unrelated modernization, formatting sweeps, or broad refactors while adding code.

## Detailed Reference

For new classes, file templates, naming rules, member layout, and implementation patterns, read `references/cpp-style.md`.

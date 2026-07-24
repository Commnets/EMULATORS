---
name: emulators-fmt-audit
description: Build, audit, and modify EMULATORS runtime `.fmt` formatter files and keep them synchronized with C++ `getInfoStructure()` producers and their `.hpp` field documentation. Use when adding, renaming, or removing `InfoStructure` attributes or nested structures; diagnosing missing console fields; reviewing formatter includes, formatter names, placeholders, arrays, nested invocations, or table parameters; or changing namespace-specific `.fmt` files under `projects`.
---

# EMULATORS FMT Audit

Audit the complete contract from a C++ producer to its runtime text output:

```text
getInfoStructure() -> documented fields in .hpp -> formatter block -> including machine .fmt -> command/output stream
```

Read [references/fmt-language.md](references/fmt-language.md) completely before creating or changing a `.fmt` file.

## Workflow

1. Identify every relevant `getInfoStructure()` implementation and its inheritance chain.
2. Record each exact `result.add ("KEY", ...)` as either an attribute or a nested `InfoStructure`.
3. Include inherited fields only through the documented parent contract. Do not guess keys from member names.
4. Compare the actual keys with the Doxygen block immediately associated with the declaration in the `.hpp`.
5. Locate formatter blocks by `.CLASSNAME`, nested key name, and command wrapper name.
6. Follow every leading `?path.fmt` include recursively. Resolve paths as the runtime does from the emulator working directory/configuration.
7. Compare formatter placeholders with actual keys and report missing, stale, and mistyped fields.
8. Check all machine variants that include the shared formatter, especially normal and remote consoles.
9. Propose or apply the smallest synchronized change across C++, `.hpp` documentation, and `.fmt`.
10. Validate the formatter parse and, when possible, format a real `InfoStructure` through the formatter used by the command.

## Contract rules

- Treat key spelling and case as an API.
- Document every field added by the method, with exact formatter spelling.
- Classify nested data as `InfoStructure`, not `Attribute`.
- Preserve inherited formatter blocks unless the subclass changes the exposed structure.
- Missing attributes and nested structures produce empty text and can hide stale placeholders. Audit explicitly.
- Keep shared chip formatters in `projects/COMMODORE/Commodore.fmt` when several machines consume the same class.
- Keep machine command wrappers in their machine namespace `.fmt`.
- Update both local-console and remote-console entry files when they duplicate rather than include a definition.
- Preserve comments, block ordering, indentation, and CRLF conventions.

## Search commands

```powershell
rg -n "getInfoStructure|result\\.add" include/COMMODORE src/COMMODORE
rg -n ";(CIA|VIA|TED)|@(CIA|VIA|TED)" projects --glob "*.fmt"
rg -n "\\?\\.\\/.*\\.fmt|;[A-Za-z0-9_.-]+|@[A-Za-z0-9_.-]+[?>]?" projects --glob "*.fmt"
```

Use `git diff` to identify newly added, removed, or renamed keys, but verify the complete current method rather than auditing only the diff.

## Reporting

Separate findings into:

- confirmed mismatch;
- missing presentation;
- stale presentation;
- optional presentation;
- inheritance/include dependency;
- validation limitation.

For every proposed change name the class, method, header, formatter block, and all affected `.fmt` entry points. Do not modify C++ or `.fmt` when the user requested analysis or proposals only.

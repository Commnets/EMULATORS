# EMULATORS `.fmt` language

The authoritative implementation is:

- `include/CORE/StdFormatter.hpp`
- `src/CORE/StdFormatter.cpp`
- `include/CORE/FmterBuilder.hpp`
- `src/CORE/FmterBuilder.cpp`

## File structure

- `#...` is a comment discarded after trimming.
- `?path` recursively includes another formatter file.
- `;Name` starts a formatter named `Name` and ends the previous formatter.
- Ordinary lines form that formatter until the next `;Name` or EOF.
- Empty lines inside a block are retained and can affect output.
- The reader uses a 255-byte line buffer; avoid long physical lines.
- Duplicate formatter names are inserted into a map; do not rely on later definitions overriding earlier ones.

`#---` is only a comment convention, not a syntactic delimiter.

## Pieces

### Attribute

```text
@NAME@
@NAME{length=10}@
@NAME{length=10,right=YES}@
```

Reads `InfoStructure::attribute("NAME")`. Missing attributes become empty text.

`length` fixes output width. Presence of `right` enables right adjustment; its value is not inspected.

`@BYTES{blocksize=16,listsep=\,,tablesep=\ ,minsize=2,maxelements=64}@` creates a table piece.

### Array/default structure formatting

```text
@ITEMS?{key=YES,equal=:,empty=none}\n@
@ITEMS?{fmter=.CLASS,empty=none}\n@
@ITEMS?{fmter=SpecificFormatter,empty=none}\n@
```

`?` requires a nested `InfoStructure` named `ITEMS`.

- `fmter=.CLASS` selects the formatter named by each element's `CLASSNAME`.
- `fmter=Name` selects `Name`.
- Without `fmter`, the default formatter uses `key`, `equal`, `empty`, and optional `blocksize`.
- Text after `?` and before the closing `@` separates formatted elements.

### Nested formatter invocation

```text
@TimerA>@
@TimerA>\n@
@.CLASS>@
```

- `@TimerA>@` requires a nested structure `TimerA` and formatter `TimerA`.
- `@.CLASS>@` formats the current structure using its `CLASSNAME`.
- Text after `>` is appended after successful formatting.

Missing structures or formatters yield empty output.

## Escaping

Within parameters:

- `\,` represents comma.
- `\ ` represents space.
- `\n` represents newline.
- `\t` represents tab.

Fixed text also converts `\n` and `\t`.

## Formatter resolution

`FormatterBuilder` stores formatters per loaded root file and resolves names only inside the current `defaultFormatFile`. Included files are flattened into that root's line set.

Machine entry files such as `projects/C64/C64.fmt`, `projects/VIC20/VIC20.fmt`, and `projects/C264/C264.fmt` include shared definitions such as `Commodore.fmt`, `Standard.fmt`, and a console-specific file. Audit both the source formatter and every entry file that exposes it.

## Semantic validation checklist

1. Match formatter name to the nested key or `CLASSNAME` that invokes it.
2. Match every `@KEY@` to an attribute.
3. Match every `@KEY?…@` or `@KEY>…@` to a nested structure.
4. Verify selected formatters exist in the flattened include set.
5. Verify `.CLASS` targets provide `CLASSNAME`.
6. Check renamed keys in duplicate wrapper blocks.
7. Render representative empty, single-element, and multi-element structures.
8. Treat blank output as a possible mismatch, not proof of correctness.

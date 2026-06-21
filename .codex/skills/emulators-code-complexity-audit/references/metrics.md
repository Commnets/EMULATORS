# Complexity Metrics

## Scope And Attribution

- **Physical lines**: every line in scanned source files.
- **Code lines**: non-empty lines remaining after comments and string contents are removed.
- **Comment lines**: lines containing a line or block comment; inline comments also count.
- **Comment-only lines**: comment lines with no code before or after the comment.
- **Namespace percentage**: namespace code lines divided by all scanned code lines.
- Header ownership comes from the enclosing `namespace NAME` block.
- Source ownership comes from the fully qualified definition, such as `COMMODORE::VICII::simulate`, and remains active through that definition.
- Qualified static-data initializers are assigned to the first namespace in their qualified name.
- `GLOBAL` contains lines that cannot be assigned safely, including includes, macros, and genuinely global declarations.

## Complexity

- **Cyclomatic complexity** is approximated per function as one plus occurrences of `if`, `for`, `while`, `case`, `catch`, `&&`, `||`, and the ternary operator.
- **Maximum nesting** is the greatest brace depth inside a detected function. Initializer and aggregate braces can increase this value.
- **Function size** is the physical span from the detected signature to its closing brace.
- **Hotspots** rank functions by cyclomatic complexity, then size.

These values are deterministic heuristics. Templates, macros, lambdas, unusual operators, and complex preprocessor branches can differ from compiler-AST results.

## Recurrence

- Code lines are normalized by removing whitespace and replacing numeric literals before comparison.
- Trivial structural lines, preprocessor directives, and very short statements are excluded.
- **Repeated-line ratio** is repeated candidate-line occurrences beyond the first divided by all recurrence candidate lines in the namespace.
- **Repeated-block ratio** uses sliding windows of normalized candidate lines. The default block contains five lines.
- Repeated generated tables can legitimately score highly. Inspect locations before classifying recurrence as harmful duplication.

## Coupling

- **Outgoing namespace references** count qualified uses such as `MCHEmul::Address` inside code attributed to another namespace.
- `std` and the current namespace are excluded.
- **Fan-out** is the number of distinct referenced namespaces.
- **Fan-in** is the number of distinct namespaces referencing the current one.
- Qualified references are lexical and do not distinguish inheritance, calls, types, or constants.

## Suggested Interpretation

- Rising code share shows where framework ownership is concentrating.
- High average complexity suggests broad local reasoning cost.
- A small number of extreme hotspot functions suggests targeted decomposition rather than namespace-wide refactoring.
- High block recurrence is stronger evidence of duplicated structure than high line recurrence alone.
- High fan-in identifies namespaces whose changes may have broad impact.
- High fan-out identifies namespaces with many architectural dependencies.

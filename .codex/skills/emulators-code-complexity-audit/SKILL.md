---
name: emulators-code-complexity-audit
description: Measure and audit C/C++ code size, namespace distribution, function complexity, nesting, recurrence, and namespace coupling in the EMULATORS framework. Use when Codex needs codebase statistics, complexity baselines, namespace comparisons, duplication indicators, hotspots, architectural growth analysis, or before/after complexity reports while excluding third-party code.
---

# EMULATORS Code Complexity Audit

## Workflow

1. Run `scripts/analyze_complexity.py` from the repository root. Its default scope is `include/` and `src/`.
2. Use Markdown for a human review and JSON when results will be compared or processed later.
3. Report totals and every requested ratio by namespace. Keep `GLOBAL` visible because it exposes unattributed implementation, macros, includes, and global declarations.
4. Read `references/metrics.md` before interpreting recurrence, cyclomatic complexity, or namespace attribution limitations.
5. Inspect the listed hotspot functions and repeated blocks before proposing refactors. Treat the script as a deterministic screening tool, not a full C++ parser.

## Commands

```powershell
python .codex/skills/emulators-code-complexity-audit/scripts/analyze_complexity.py
python .codex/skills/emulators-code-complexity-audit/scripts/analyze_complexity.py --format json --output complexity.json
python .codex/skills/emulators-code-complexity-audit/scripts/analyze_complexity.py projects emulators tests
python .codex/skills/emulators-code-complexity-audit/scripts/analyze_complexity.py --namespace COMMODORE --namespace C64
```

## Reporting Rules

- State the scanned paths, extensions, exclusions, and number of files.
- Use code lines, not physical lines, as the denominator for namespace ownership percentages.
- Present namespace totals before individual hotspots.
- Keep recurrence and cyclomatic complexity labelled as heuristic.
- Compare changes using JSON generated with identical paths, exclusions, and block size.
- Do not include `thirdparties/`, generated output, binaries, documentation, or Codex metadata unless explicitly requested.

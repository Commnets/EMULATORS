#!/usr/bin/env python3
"""Deterministic C/C++ complexity audit grouped by primary namespace."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path


SOURCE_EXTENSIONS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx"}
DEFAULT_PATHS = ("include", "src")
DEFAULT_EXCLUDED_PARTS = {
	".codex", ".git", ".vs", "bin", "build", "builds", "docs", "exe",
	"external", "generated", "obj", "out", "setups", "third_party",
	"thirdparties", "vendor",
}
GLOBAL_NAMESPACE = "GLOBAL"
CONTROL_NAMES = {"if", "for", "while", "switch", "catch", "return", "sizeof", "alignof"}
IGNORED_REFERENCES = {"std"}

NAMESPACE_RE = re.compile(r"\bnamespace\s+([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)")
QUALIFIED_FUNCTION_RE = re.compile(
	r"((?:[A-Za-z_]\w*::)+(?:operator\s*[^\s(]+|~?[A-Za-z_]\w*))\s*\("
)
UNQUALIFIED_FUNCTION_RE = re.compile(r"\b(~?[A-Za-z_]\w*)\s*\(")
QUALIFIED_CHAIN_RE = re.compile(r"\b([A-Za-z_]\w*(?:::[A-Za-z_]\w*)+)")
REFERENCE_RE = re.compile(r"\b([A-Za-z_]\w*)::")
NUMBER_RE = re.compile(
	r"(?<![A-Za-z_])(?:0[xX][0-9A-Fa-f]+|\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)[uUlLfF]*"
)


@dataclass(frozen=True)
class LineInfo:
	raw: str
	code: str
	has_comment: bool


@dataclass
class FunctionInfo:
	path: str
	name: str
	namespace: str
	start: int
	end: int
	code_lines: int
	complexity: int
	max_nesting: int

	@property
	def physical_lines(self) -> int:
		return self.end - self.start + 1


@dataclass
class NamespaceStats:
	name: str
	physical_lines: int = 0
	blank_lines: int = 0
	code_lines: int = 0
	comment_lines: int = 0
	comment_only_lines: int = 0
	classes: int = 0
	structs: int = 0
	enums: int = 0
	todos: int = 0
	asserts: int = 0
	files: set[str] = field(default_factory=set)
	functions: list[FunctionInfo] = field(default_factory=list)
	references: Counter[str] = field(default_factory=Counter)
	recurrence_lines: list[tuple[str, str, str, int]] = field(default_factory=list)
	recurrence_sequences: dict[str, list[tuple[str, str, int]]] = field(
		default_factory=lambda: defaultdict(list)
	)


def parse_args() -> argparse.Namespace:
	parser = argparse.ArgumentParser(
		description="Measure EMULATORS C/C++ complexity by primary namespace."
	)
	parser.add_argument("paths", nargs="*", help="Paths relative to --root; defaults to include src.")
	parser.add_argument("--root", default=".", help="Repository root. Defaults to the current directory.")
	parser.add_argument("--format", choices=("markdown", "json"), default="markdown")
	parser.add_argument("--output", help="Write the report to this file instead of stdout.")
	parser.add_argument("--exclude", action="append", default=[], help="Additional excluded path component.")
	parser.add_argument("--namespace", action="append", default=[], dest="namespaces")
	parser.add_argument("--top", type=int, default=15, help="Maximum hotspot/repetition entries.")
	parser.add_argument("--block-size", type=int, default=5, help="Normalized lines per recurrence block.")
	return parser.parse_args()


def primary_namespace(name: str | None) -> str:
	return name.split("::", 1)[0] if name else GLOBAL_NAMESPACE


def first_function_match(signature: str):
	candidates = [(match.start(), True, match) for match in QUALIFIED_FUNCTION_RE.finditer(signature)]
	candidates.extend(
		(match.start(), False, match)
		for match in UNQUALIFIED_FUNCTION_RE.finditer(signature)
		if match.group(1) not in CONTROL_NAMES
	)
	return min(candidates, default=None, key=lambda item: item[0])


def lex_lines(text: str) -> list[LineInfo]:
	result: list[LineInfo] = []
	in_block_comment = False
	for raw in text.splitlines():
		code: list[str] = []
		has_comment = in_block_comment
		quote: str | None = None
		escaped = False
		i = 0
		while i < len(raw):
			if in_block_comment:
				end = raw.find("*/", i)
				if end < 0:
					i = len(raw)
					continue
				in_block_comment = False
				i = end + 2
				continue
			ch = raw[i]
			nxt = raw[i + 1] if i + 1 < len(raw) else ""
			if quote:
				code.append(" ")
				if escaped:
					escaped = False
				elif ch == "\\":
					escaped = True
				elif ch == quote:
					quote = None
				i += 1
				continue
			if ch in {'"', "'"}:
				quote = ch
				code.append(" ")
				i += 1
				continue
			if ch == "/" and nxt == "/":
				has_comment = True
				break
			if ch == "/" and nxt == "*":
				has_comment = True
				in_block_comment = True
				i += 2
				continue
			code.append(ch)
			i += 1
		result.append(LineInfo(raw=raw, code="".join(code).rstrip(), has_comment=has_comment))
	return result


def namespace_contexts(lines: list[LineInfo]) -> list[str]:
	contexts: list[str] = []
	stack: list[tuple[int, str]] = []
	pending: str | None = None
	depth = 0
	for line in lines:
		match = NAMESPACE_RE.search(line.code)
		declared = match.group(1) if match else None
		if declared and "::" not in declared and stack:
			declared = f"{stack[-1][1]}::{declared}"
		contexts.append(declared or (stack[-1][1] if stack else GLOBAL_NAMESPACE))
		if declared:
			pending = declared
		for ch in line.code:
			if ch == "{":
				depth += 1
				if pending:
					stack.append((depth, pending))
					pending = None
			elif ch == "}":
				if stack and stack[-1][0] == depth:
					stack.pop()
				depth = max(0, depth - 1)
	return contexts


def longest_known_namespace(chain: str, known_namespaces: set[str]) -> str | None:
	parts = chain.split("::")
	for length in range(len(parts), 0, -1):
		candidate = "::".join(parts[:length])
		if candidate in known_namespaces:
			return candidate
	return None


def function_identity(signature: str, namespace_hint: str,
	known_namespaces: set[str]) -> tuple[str, str] | None:
	selected = first_function_match(signature)
	if not selected:
		return None
	_, qualified, match = selected
	if not qualified:
		return (namespace_hint, match.group(1)) if namespace_hint != GLOBAL_NAMESPACE else None
	chain = re.sub(r"\s+", "", match.group(1))
	owner = namespace_hint if namespace_hint != GLOBAL_NAMESPACE else longest_known_namespace(
		chain, known_namespaces
	)
	return (owner, chain) if owner else None


def signature_and_opening(lines: list[LineInfo], start: int) -> tuple[str, int, int] | None:
	parts: list[str] = []
	for index in range(start, min(len(lines), start + 30)):
		code = lines[index].code.strip()
		if not code and not parts:
			return None
		parts.append(lines[index].code)
		joined = "\n".join(parts)
		selected = first_function_match(joined)
		if not selected:
			if ";" in code or "{" in code:
				return None
			continue
		_, _, identity_probe = selected
		paren_start = joined.find("(", identity_probe.start())
		paren_depth = 0
		params_closed = False
		for pos in range(paren_start, len(joined)):
			ch = joined[pos]
			if ch == "(":
				paren_depth += 1
			elif ch == ")":
				paren_depth -= 1
				params_closed = params_closed or paren_depth == 0
			elif params_closed and paren_depth == 0 and ch == ";":
				return None
			elif params_closed and paren_depth == 0 and ch == "{":
				before = joined[:pos]
				opening_line = start + before.count("\n")
				opening_col = pos - (before.rfind("\n") + 1)
				return joined[:pos], opening_line, opening_col
	return None


def matching_brace(lines: list[LineInfo], opening_line: int, opening_col: int) -> int:
	depth = 0
	for index in range(opening_line, len(lines)):
		code = lines[index].code
		begin = opening_col if index == opening_line else 0
		for ch in code[begin:]:
			if ch == "{":
				depth += 1
			elif ch == "}":
				depth -= 1
				if depth == 0:
					return index
	return len(lines) - 1


def function_complexity(lines: list[LineInfo], start: int, end: int) -> tuple[int, int, int]:
	code_lines = sum(bool(lines[i].code.strip()) for i in range(start, end + 1))
	text = "\n".join(lines[i].code for i in range(start, end + 1))
	complexity = 1
	for keyword in ("if", "for", "while", "case", "catch"):
		complexity += len(re.findall(rf"\b{keyword}\b", text))
	complexity += text.count("&&") + text.count("||")
	complexity += len(re.findall(r"\?(?!\?)", text))
	depth = maximum = 0
	for ch in text:
		if ch == "{":
			depth += 1
			maximum = max(maximum, depth)
		elif ch == "}":
			depth = max(0, depth - 1)
	return code_lines, complexity, max(0, maximum - 1)


def find_functions(path: str, lines: list[LineInfo], contexts: list[str],
	known_namespaces: set[str]) -> list[FunctionInfo]:
	functions: list[FunctionInfo] = []
	index = 0
	while index < len(lines):
		code = lines[index].code.strip()
		if not code or code.startswith("#") or NAMESPACE_RE.search(code):
			index += 1
			continue
		candidate = signature_and_opening(lines, index)
		if not candidate:
			index += 1
			continue
		signature, opening_line, opening_col = candidate
		identity = function_identity(signature, contexts[index], known_namespaces)
		if not identity:
			index += 1
			continue
		namespace, name = identity
		end = matching_brace(lines, opening_line, opening_col)
		code_count, complexity, nesting = function_complexity(lines, index, end)
		functions.append(FunctionInfo(
			path=path, name=name, namespace=namespace, start=index + 1, end=end + 1,
			code_lines=code_count, complexity=complexity, max_nesting=nesting,
		))
		index = end + 1
	return functions


def explicit_owner(code: str, known_namespaces: set[str]) -> str | None:
	candidates = []
	for match in QUALIFIED_CHAIN_RE.finditer(code):
		chain = match.group(1)
		owner = longest_known_namespace(chain, known_namespaces)
		if owner:
			candidates.append((owner.count("::"), match.start(), owner))
	return max(candidates, default=(0, 0, None))[2]


def assign_owners(lines: list[LineInfo], contexts: list[str], functions: list[FunctionInfo],
	known_namespaces: set[str]) -> list[str]:
	owners = list(contexts)
	for function in functions:
		for index in range(function.start - 1, function.end):
			owners[index] = function.namespace
	function_lines = {i for f in functions for i in range(f.start - 1, f.end)}
	data_owner: str | None = None
	for index, line in enumerate(lines):
		if index in function_lines or owners[index] != GLOBAL_NAMESPACE:
			continue
		if data_owner:
			owners[index] = data_owner
			if ";" in line.code:
				data_owner = None
			continue
		owner = explicit_owner(line.code, known_namespaces)
		if owner:
			owners[index] = owner
			if "=" in line.code and ";" not in line.code:
				data_owner = owner
	return owners


def normalize_recurrence_line(code: str) -> str | None:
	stripped = code.strip()
	if not stripped or stripped.startswith("#") or NAMESPACE_RE.search(stripped):
		return None
	normalized = NUMBER_RE.sub("#", stripped)
	normalized = re.sub(r"\s+", "", normalized)
	if len(normalized) < 12 or normalized in {"return;", "break;", "continue;"}:
		return None
	if not re.search(r"[A-Za-z_]", normalized):
		return None
	return normalized


def discover_files(root: Path, paths: list[str], excluded: set[str]) -> list[Path]:
	files: set[Path] = set()
	for supplied in paths:
		candidate = (root / supplied).resolve()
		if candidate.is_file() and candidate.suffix.lower() in SOURCE_EXTENSIONS:
			files.add(candidate)
		elif candidate.is_dir():
			for path in candidate.rglob("*"):
				if path.is_file() and path.suffix.lower() in SOURCE_EXTENSIONS:
					relative_parts = {part.lower() for part in path.relative_to(root).parts}
					if not relative_parts.intersection(excluded):
						files.add(path.resolve())
	return sorted(files, key=lambda p: p.as_posix().lower())


def reference_namespace(chain: str, known_namespaces: set[str]) -> str | None:
	known = longest_known_namespace(chain, known_namespaces)
	if known:
		return known
	root = primary_namespace(chain)
	return root if root.isupper() else None


def analyze_file(root: Path, path: Path, stats: dict[str, NamespaceStats],
	known_namespaces: set[str]) -> None:
	relative = path.relative_to(root).as_posix()
	text = path.read_text(encoding="utf-8", errors="replace")
	lines = lex_lines(text)
	contexts = namespace_contexts(lines)
	functions = find_functions(relative, lines, contexts, known_namespaces)
	owners = assign_owners(lines, contexts, functions, known_namespaces)
	for function in functions:
		stats.setdefault(function.namespace, NamespaceStats(function.namespace)).functions.append(function)
	for index, (line, owner) in enumerate(zip(lines, owners), start=1):
		ns = stats.setdefault(owner, NamespaceStats(owner))
		ns.physical_lines += 1
		ns.files.add(relative)
		if not line.raw.strip():
			ns.blank_lines += 1
		if line.has_comment:
			ns.comment_lines += 1
			if not line.code.strip():
				ns.comment_only_lines += 1
		code = line.code.strip()
		if not code:
			continue
		ns.code_lines += 1
		ns.classes += len(re.findall(r"\bclass\s+[A-Za-z_]\w*", code))
		ns.structs += len(re.findall(r"\bstruct\s+[A-Za-z_]\w*", code))
		ns.enums += len(re.findall(r"\benum(?:\s+class)?\s+[A-Za-z_]\w*", code))
		ns.todos += len(re.findall(r"\b(?:TODO|FIXME)\b", line.raw, flags=re.IGNORECASE))
		ns.asserts += len(re.findall(r"\bassert\s*\(", code))
		for chain in QUALIFIED_CHAIN_RE.findall(code):
			reference = reference_namespace(chain, known_namespaces)
			if reference and reference not in IGNORED_REFERENCES and reference != owner:
				ns.references[reference] += 1
		normalized = normalize_recurrence_line(code)
		if normalized:
			ns.recurrence_lines.append((normalized, code, relative, index))
			ns.recurrence_sequences[relative].append((normalized, code, index))


def percentage(part: int | float, whole: int | float) -> float:
	return round((100.0 * part / whole), 2) if whole else 0.0


def recurrence_metrics(ns: NamespaceStats, block_size: int, top: int) -> dict:
	line_counts = Counter(item[0] for item in ns.recurrence_lines)
	repeated_lines = sum(count - 1 for count in line_counts.values() if count > 1)
	display = {}
	locations: dict[str, list[str]] = defaultdict(list)
	for normalized, code, path, line in ns.recurrence_lines:
		display.setdefault(normalized, code.strip())
		locations[normalized].append(f"{path}:{line}")
	top_lines = [
		{"count": count, "code": display[key], "locations": locations[key][:5]}
		for key, count in sorted(line_counts.items(), key=lambda item: (-item[1], item[0]))
		if count > 1
	][:top]
	blocks: Counter[str] = Counter()
	block_locations: dict[str, list[str]] = defaultdict(list)
	block_text: dict[str, list[str]] = {}
	for path, sequence in ns.recurrence_sequences.items():
		last_block_line: dict[str, int] = {}
		for start in range(max(0, len(sequence) - block_size + 1)):
			window = sequence[start:start + block_size]
			fingerprint = hashlib.sha1("\n".join(x[0] for x in window).encode()).hexdigest()
			if window[0][2] <= last_block_line.get(fingerprint, -block_size) + block_size - 1:
				continue
			last_block_line[fingerprint] = window[0][2]
			blocks[fingerprint] += 1
			block_locations[fingerprint].append(f"{path}:{window[0][2]}")
			block_text.setdefault(fingerprint, [x[1].strip() for x in window])
	repeated_blocks = sum(count - 1 for count in blocks.values() if count > 1)
	top_blocks = [
		{"count": count, "lines": block_text[key], "locations": block_locations[key][:5]}
		for key, count in sorted(blocks.items(), key=lambda item: (-item[1], item[0]))
		if count > 1
	][:top]
	return {
		"candidate_lines": len(ns.recurrence_lines),
		"repeated_line_instances": repeated_lines,
		"repeated_line_ratio": percentage(repeated_lines, len(ns.recurrence_lines)),
		"block_windows": sum(blocks.values()),
		"repeated_block_instances": repeated_blocks,
		"repeated_block_ratio": percentage(repeated_blocks, sum(blocks.values())),
		"top_lines": top_lines,
		"top_blocks": top_blocks,
	}


def build_report(root: Path, paths: list[str], files: list[Path], stats: dict[str, NamespaceStats],
	block_size: int, top: int, namespace_filter: set[str], excluded: set[str]) -> dict:
	total_code = sum(ns.code_lines for ns in stats.values())
	selected_names = sorted(
		(name for name in stats if not namespace_filter or name in namespace_filter),
		key=lambda name: (-stats[name].code_lines, name),
	)
	incoming: dict[str, set[str]] = defaultdict(set)
	for source, ns in stats.items():
		for target in ns.references:
			incoming[target].add(source)
	namespaces = []
	for name in selected_names:
		ns = stats[name]
		complexities = [f.complexity for f in ns.functions]
		sizes = [f.physical_lines for f in ns.functions]
		namespaces.append({
			"name": name,
			"files": len(ns.files),
			"physical_lines": ns.physical_lines,
			"blank_lines": ns.blank_lines,
			"code_lines": ns.code_lines,
			"code_share": percentage(ns.code_lines, total_code),
			"comment_lines": ns.comment_lines,
			"comment_only_lines": ns.comment_only_lines,
			"comment_to_code_ratio": percentage(ns.comment_only_lines, ns.code_lines),
			"classes": ns.classes,
			"structs": ns.structs,
			"enums": ns.enums,
			"functions": len(ns.functions),
			"average_function_lines": round(sum(sizes) / len(sizes), 2) if sizes else 0.0,
			"maximum_function_lines": max(sizes, default=0),
			"cyclomatic_total": sum(complexities),
			"cyclomatic_average": round(sum(complexities) / len(complexities), 2) if complexities else 0.0,
			"cyclomatic_maximum": max(complexities, default=0),
			"maximum_nesting": max((f.max_nesting for f in ns.functions), default=0),
			"todos": ns.todos,
			"asserts": ns.asserts,
			"fan_out": len(ns.references),
			"fan_in": len(incoming.get(name, set())),
			"outgoing_references": dict(
				sorted(ns.references.items(), key=lambda item: (-item[1], item[0]))
			),
			"recurrence": recurrence_metrics(ns, block_size, top),
		})
	all_functions = sorted(
		(f for ns in stats.values() for f in ns.functions
		 if not namespace_filter or f.namespace in namespace_filter),
		key=lambda f: (-f.complexity, -f.physical_lines, f.namespace, f.name),
	)
	return {
		"scope": {
			"root": root.as_posix(), "paths": paths, "extensions": sorted(SOURCE_EXTENSIONS),
			"excluded_parts": sorted(excluded), "files": len(files), "block_size": block_size,
		},
		"totals": {
			"physical_lines": sum(ns.physical_lines for ns in stats.values()),
			"blank_lines": sum(ns.blank_lines for ns in stats.values()),
			"code_lines": total_code,
			"comment_lines": sum(ns.comment_lines for ns in stats.values()),
			"comment_only_lines": sum(ns.comment_only_lines for ns in stats.values()),
			"functions": sum(len(ns.functions) for ns in stats.values()),
			"namespaces": len(stats),
		},
		"namespaces": namespaces,
		"hotspots": [
			{
				"namespace": f.namespace, "name": f.name, "path": f.path,
				"line": f.start, "physical_lines": f.physical_lines,
				"code_lines": f.code_lines, "complexity": f.complexity,
				"max_nesting": f.max_nesting,
			}
			for f in all_functions[:top]
		],
	}


def markdown_report(report: dict) -> str:
	scope, totals = report["scope"], report["totals"]
	lines = [
		"# EMULATORS Code Complexity Audit", "",
		f"- Paths: `{', '.join(scope['paths'])}`",
		f"- Extensions: `{', '.join(scope['extensions'])}`",
		f"- Excluded path parts: `{', '.join(scope['excluded_parts'])}`",
		f"- Files: **{scope['files']}**",
		f"- Physical lines: **{totals['physical_lines']}**",
		f"- Code lines: **{totals['code_lines']}**",
		f"- Comment-only lines: **{totals['comment_only_lines']}**",
		f"- Detected functions: **{totals['functions']}**",
		"", "## Namespace Distribution", "",
		"| Namespace | Files | Code | Share | Comments/code | Functions | Avg CC | Max CC | Max nesting |",
		"|---|---:|---:|---:|---:|---:|---:|---:|---:|",
	]
	for ns in report["namespaces"]:
		lines.append(
			f"| {ns['name']} | {ns['files']} | {ns['code_lines']} | {ns['code_share']:.2f}% | "
			f"{ns['comment_to_code_ratio']:.2f}% | {ns['functions']} | {ns['cyclomatic_average']:.2f} | "
			f"{ns['cyclomatic_maximum']} | {ns['maximum_nesting']} |"
		)
	lines.extend([
		"", "## Namespace Complexity", "",
		"| Namespace | Classes | Structs | Enums | Avg function lines | Max function lines | CC total | TODO/FIXME | Asserts |",
		"|---|---:|---:|---:|---:|---:|---:|---:|---:|",
	])
	for ns in report["namespaces"]:
		lines.append(
			f"| {ns['name']} | {ns['classes']} | {ns['structs']} | {ns['enums']} | "
			f"{ns['average_function_lines']:.2f} | {ns['maximum_function_lines']} | "
			f"{ns['cyclomatic_total']} | {ns['todos']} | {ns['asserts']} |"
		)
	lines.extend([
		"", "## Recurrence By Namespace", "",
		"| Namespace | Candidate lines | Repeated-line ratio | Block windows | Repeated-block ratio |",
		"|---|---:|---:|---:|---:|",
	])
	for ns in report["namespaces"]:
		r = ns["recurrence"]
		lines.append(
			f"| {ns['name']} | {r['candidate_lines']} | {r['repeated_line_ratio']:.2f}% | "
			f"{r['block_windows']} | {r['repeated_block_ratio']:.2f}% |"
		)
	lines.extend([
		"", "## Namespace Coupling", "",
		"| Namespace | Fan-in | Fan-out | Most frequent outgoing references |",
		"|---|---:|---:|---|",
	])
	for ns in report["namespaces"]:
		refs = ", ".join(
			f"{name} ({count})" for name, count in list(ns["outgoing_references"].items())[:5]
		) or "-"
		lines.append(f"| {ns['name']} | {ns['fan_in']} | {ns['fan_out']} | {refs} |")
	lines.extend([
		"", "## Function Hotspots", "",
		"| Namespace | Function | CC | Lines | Nesting | Location |",
		"|---|---|---:|---:|---:|---|",
	])
	for item in report["hotspots"]:
		lines.append(
			f"| {item['namespace']} | `{item['name']}` | {item['complexity']} | "
			f"{item['physical_lines']} | {item['max_nesting']} | `{item['path']}:{item['line']}` |"
		)
	lines.extend(["", "_Cyclomatic complexity, recurrence, nesting, and coupling are heuristic._", ""])
	return "\n".join(lines)


def collect_known_namespaces(files: list[Path]) -> set[str]:
	"""Collect namespace declarations before attributing qualified definitions."""
	result: set[str] = set()
	for path in files:
		text = path.read_text(encoding="utf-8", errors="replace")
		result.update(
			name for name in namespace_contexts(lex_lines(text))
			if name != GLOBAL_NAMESPACE
		)
	return result


def main() -> int:
	args = parse_args()
	root = Path(args.root).resolve()
	paths = args.paths or list(DEFAULT_PATHS)
	excluded = DEFAULT_EXCLUDED_PARTS | {item.lower() for item in args.exclude}
	files = discover_files(root, paths, excluded)
	if not files:
		print("No matching source files found.", file=sys.stderr)
		return 2
	known_namespaces = collect_known_namespaces(files)
	stats: dict[str, NamespaceStats] = {}
	for path in files:
		analyze_file(root, path, stats, known_namespaces)
	report = build_report(
		root, paths, files, stats, max(2, args.block_size), max(1, args.top),
		set(args.namespaces), excluded
	)
	content = (
		json.dumps(report, indent=2, sort_keys=True) + "\n"
		if args.format == "json" else markdown_report(report)
	)
	if args.output:
		Path(args.output).write_text(content, encoding="utf-8")
	else:
		print(content, end="")
	return 0


if __name__ == "__main__":
	raise SystemExit(main())

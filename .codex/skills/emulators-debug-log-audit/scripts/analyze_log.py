#!/usr/bin/env python3
"""Stream and inspect EMULATORS debug logs without loading them into memory."""
from __future__ import annotations
import argparse
import collections
import re
from pathlib import Path

HEADER_RE = re.compile(r"^([A-Za-z][A-Za-z0-9_-]*)\s+(\d+)\s+(.+?)\s*$")
CPU_CYCLES_RE = re.compile(r"Last\s+(\d+)\s+cycles")
PC_RE = re.compile(r"PC:\$([0-9a-fA-F]+).*?\)\s+(.+)$")
VIC_INTERNAL_RE = re.compile(r"\bROW=(\d+).*?\bCycle=(\d+)")

def blocks(path):
    current = []
    with path.open("r", encoding="utf-8", errors="replace") as source:
        for raw in source:
            line = raw.rstrip("\r\n")
            if line.strip() == "+":
                if current:
                    yield current
                    current = []
            else:
                current.append(line)
    if current:
        yield current

def metadata(lines):
    headers, vic = [], []
    duration = pc = instruction = None
    for line in lines:
        match = HEADER_RE.match(line)
        if match:
            headers.append((match.group(1), int(match.group(2)), match.group(3)))
        match = CPU_CYCLES_RE.search(line)
        if match:
            duration = int(match.group(1))
        match = PC_RE.search(line)
        if match:
            pc, instruction = int(match.group(1), 16), match.group(2).strip()
        match = VIC_INTERNAL_RE.search(line)
        if match:
            vic.append((int(match.group(1)), int(match.group(2))))
    return headers, duration, pc, instruction, vic

def compact(index, lines):
    headers, duration, pc, instruction, vic = metadata(lines)
    cpu = next((h[1] for h in headers if h[0] == "CPU"), -1)
    vic_range = "-" if not vic else f"{vic[0][0]}:{vic[0][1]}..{vic[-1][0]}:{vic[-1][1]}"
    pc_text = "-" if pc is None else f"${pc:04x}"
    notable = []
    for line in lines:
        value = line.strip()
        if any(key in value for key in ("Interrupt ", "Stopped", "New stop request", "Bad line situation", "Reading Video Matrix", "$d011", "$d012", "$d019", "$d01a")):
            notable.append(value)
    suffix = " | " + " ; ".join(notable) if notable else ""
    return f"block={index} cpu={cpu} duration={duration} pc={pc_text} inst={instruction or '-'} vic={vic_range}{suffix}"

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path)
    parser.add_argument("--summary", action="store_true")
    parser.add_argument("--match", help="regular expression matched against a whole block")
    parser.add_argument("--before", type=int, default=0)
    parser.add_argument("--after", type=int, default=0)
    parser.add_argument("--full", action="store_true")
    args = parser.parse_args()
    matcher = re.compile(args.match, re.IGNORECASE) if args.match else None
    history = collections.deque(maxlen=max(args.before, 1))
    pending, printed = 0, set()
    counts, chip_cycles = collections.Counter(), collections.Counter()
    shifted_mismatches, discontinuities = [], []
    previous_instruction = None
    previous_vic = None
    def emit(index, lines):
        if index in printed:
            return
        printed.add(index)
        if args.full:
            print(f"===== BLOCK {index} =====")
            print("\n".join(lines))
        else:
            print(compact(index, lines))
    for index, lines in enumerate(blocks(args.log)):
        counts["blocks"] += 1
        headers, duration, pc, instruction, vic = metadata(lines)
        per_chip = collections.Counter(c for c, _, kind in headers if kind == "Info Cycle")
        chip_cycles.update(per_chip)
        counts["cpu_instruction_records"] += pc is not None
        counts["cpu_stopped_blocks"] += any(line.startswith("CPU") and " Stopped" in line for line in lines)
        counts["interrupt_requests"] += sum("Interrupt CPU requested" in line for line in lines)
        counts["interrupt_launches"] += sum("Interrupt launched" in line for line in lines)
        counts["d011_cpu_accesses"] += sum("PC:" in line and "$d011" in line.lower() for line in lines)
        if duration is not None and previous_instruction is not None:
            previous_index, previous_chip_cycles = previous_instruction
            if previous_index == index - 1:
                for chip in ("VICII", "SID", "CIA1", "CIA2"):
                    if previous_chip_cycles[chip] and previous_chip_cycles[chip] != duration and len(shifted_mismatches) < 20:
                        shifted_mismatches.append((previous_index, index, chip, duration, previous_chip_cycles[chip]))
        previous_instruction = (index, per_chip) if pc is not None else None
        for component, absolute, kind in headers:
            if component == "VICII" and kind == "Info Cycle":
                if previous_vic is not None and absolute != previous_vic + 1 and len(discontinuities) < 20:
                    discontinuities.append((index, previous_vic, absolute))
                previous_vic = absolute
        matched = matcher is not None and matcher.search("\n".join(lines))
        if matched:
            for old in history:
                emit(*old)
            emit(index, lines)
            pending = args.after
        elif pending:
            emit(index, lines)
            pending -= 1
        history.append((index, lines))
    if args.summary or matcher is None:
        print("===== SUMMARY =====")
        for key in sorted(counts):
            print(f"{key}={counts[key]}")
        for chip in sorted(chip_cycles):
            print(f"{chip}_info_cycles={chip_cycles[chip]}")
        print(f"shifted_duration_mismatches={len(shifted_mismatches)}")
        for x in shifted_mismatches:
            print(f"shifted_duration_mismatch previous_block={x[0]} current_block={x[1]} chip={x[2]} reported_last={x[3]} previous_chip_cycles={x[4]}")
        print(f"vic_absolute_cycle_discontinuities={len(discontinuities)}")
        for x in discontinuities:
            print(f"vic_discontinuity block={x[0]} previous={x[1]} current={x[2]}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())




"""Read S5 evidence; never overwrite C++ expectations or generate a hardware golden."""

import re
import sys
import json


class Acquisition:
    def __init__(self, path, target):
        self.path, self.target = path, target
        self.records, self.registers = [], {}
        self.code, self.current, self.absolute = 9, None, 0

    @staticmethod
    def fields(text):
        return {key: value.strip() for key, value in re.findall(r"([A-Za-z][A-Za-z0-9]*)=([^,\]]+)", text)}

    @staticmethod
    def members_replay(path, first_absolute, last_line=66):
        """Acquire bus stimulus, including the matrix retained from the preceding frame."""
        records, matrix, registers = [], [None] * 40, {}
        current, initial_matrix, nibble = None, None, 9
        with open(path, encoding="utf-8", errors="replace") as source:
            for line in source:
                header = re.match(r"VICII\s+(\d+)\s+Info Cycle", line)
                if header:
                    if current and current["absolute"] >= first_absolute and int(current.get("internal", {}).get("ROW", -1)) > last_line:
                        break
                    absolute = int(header[1])
                    current = dict(absolute=absolute, registers=dict(registers), nibble=nibble)
                    if absolute == first_absolute:
                        initial_matrix = list(matrix)
                    if absolute >= first_absolute:
                        records.append(current)
                if "Transaction         :" in line and "Code=" in line:
                    nibble = int(Acquisition.fields(line)["Code"]) & 15
                if "Register write      :" in line:
                    fields = Acquisition.fields(line)
                    register = int(fields["Register"].lstrip("$"), 16)
                    value = int(fields["Value"].lstrip("$"), 16)
                    registers[register] = value
                    if current and register in (0x11, 0x16, 0x18, 0x21, 0x22, 0x23):
                        current.setdefault("writes", []).append((register, value))
                if current:
                    for key, marker in (("internal", "Internal            :"), ("memory", "Memory location     :"),
                                        ("sprites", "Sprite DMA state    :"), ("g", "Reading Graphics ["),
                                        ("c", "Reading Video Matrix & Color RAM ["), ("draw", "Drawing pixels [")):
                        if marker in line:
                            fields = Acquisition.fields(line)
                            current[key] = fields
                            if key == "c":
                                matrix[int(fields["VLMI"])] = [int(fields["Screen"], 16), int(fields["Color"], 16)]
        selected = [r for r in records if 50 <= int(r.get("internal", {}).get("ROW", -1)) <= last_line]
        if not selected or selected[0]["absolute"] != first_absolute or len(selected) != (last_line - 49) * 63:
            raise ValueError("Incomplete 10000Members raster window")
        if initial_matrix is None or any(cell is None for cell in initial_matrix):
            raise ValueError("Previous-frame matrix could not be reconstructed")
        memory, colors, rows = {}, {}, []
        initial = selected[0]
        for index, record in enumerate(selected):
            internal = record["internal"]
            if record["absolute"] != first_absolute + index or int(internal["ROW"]) != 50 + index // 63 or int(internal["Cycle"]) != index % 63 + 1:
                raise ValueError("Discontinuous 10000Members capture")
            if record["sprites"]["ActiveMask"] != "0" or record["sprites"]["DisplayMask"] != "0":
                raise ValueError("Active sprites need a separate replay")
            if record["memory"]["Bank"] != initial["memory"]["Bank"]:
                raise ValueError("Bank changes need explicit events")
            g, c = record.get("g"), record.get("c")
            additions = []
            if g:
                additions.append((int(g["Address"].lstrip("$"), 16), int(g["Data"], 16)))
            if c and c["Invalid"] == "0":
                vc, color = int(c["VC"]), int(c["Color"], 16)
                additions.append((int(record["memory"]["Screen"].lstrip("$"), 16) + vc, int(c["Screen"], 16)))
                if vc in colors and colors[vc] != color:
                    raise ValueError("Color RAM changed inside capture")
                colors[vc] = color
            for address, value in additions:
                if address in memory and memory[address] != value:
                    raise ValueError("RAM changed inside capture at " + hex(address))
                memory[address] = value
            rows.append([record["absolute"], int(internal["ROW"]), int(internal["Cycle"]), record["nibble"],
                         [] if not g else [int(g["Address"].lstrip("$"), 16), int(g["Data"], 16), int(g["Screen"], 16), int(g["Color"], 16)],
                         [] if not c else [int(c["VLMI"]), int(c["Screen"], 16), int(c["Color"], 16), int(c["Invalid"])], record.get("writes", [])])
        bank_base = int(initial["memory"]["Bank"]) << 14
        d018 = (((int(initial["memory"]["Screen"].lstrip("$"), 16) - bank_base) >> 6) & 0xf0) | (((int(initial["memory"]["Characters"].lstrip("$"), 16) - bank_base) >> 10) & 14)
        if 0x16 in initial["registers"]:
            d016 = initial["registers"][0x16]
        else:
            # MCM/CSEL are recovered from the following D016 write; XSCROLL is logged.
            next_d016 = next(value for r in selected for register, value in r.get("writes", []) if register == 0x16)
            d016 = (next_d016 & 0xf8) | int(next(r["draw"]["XScroll"] for r in selected if "draw" in r))
        return dict(records=rows, memory=sorted(memory.items()), colors=sorted(colors.items()),
                    initial=initial, matrix=initial_matrix, d018=d018, d016=d016)

    def run(self):
        with open(self.path, encoding="utf-8", errors="replace") as source:
            for line in source:
                header = re.match(r"VICII\s+(\d+)\s+Info Cycle", line)
                if header:
                    self.absolute = int(header[1])
                    self.current = None
                    if self.target - 10 * 63 <= self.absolute <= self.target + 2 * 63:
                        self.current = dict(absolute=self.absolute, registers=dict(self.registers), nibble=self.code)
                        self.records.append(self.current)
                if "Transaction         :" in line and "Code=" in line:
                    self.code = int(self.fields(line)["Code"]) & 15
                if "Register write      :" in line:
                    fields = self.fields(line)
                    register, value = int(fields["Register"].lstrip("$"), 16), int(fields["Value"].lstrip("$"), 16)
                    self.registers[register] = value
                    if self.current is not None:
                        self.current.setdefault("writes", []).append((register, value))
                if self.current is not None:
                    for key, marker in (("internal", "Internal            :"), ("memory", "Memory location     :"),
                                        ("sprites", "Sprite DMA state    :"), ("g", "Reading Graphics ["),
                                        ("c", "Reading Video Matrix & Color RAM ["), ("draw", "Drawing pixels [")):
                        if marker in line:
                            self.current[key] = self.fields(line)
                if self.absolute > self.target + 2 * 63:
                    break
        if len(sys.argv) > 3 and sys.argv[3] == "--json":
            if self.target not in (62135991, 31813199):
                raise ValueError("Unsupported capture window")
            first, last = (52, 54) if self.target == 62135991 else (147, 156)
            selected = [record for record in self.records if first <= int(record.get("internal", {}).get("ROW", -1)) <= last]
            if len(selected) != (last - first + 1) * 63:
                raise ValueError("Incomplete raster window")
            bank = selected[0]["memory"]["Bank"]
            for index, record in enumerate(selected):
                if record["absolute"] != selected[0]["absolute"] + index:
                    raise ValueError("Discontinuous absolute cycles")
                if int(record["internal"]["ROW"]) != first + index // 63 or int(record["internal"]["Cycle"]) != index % 63 + 1:
                    raise ValueError("Discontinuous raster cycles")
                if record["memory"]["Bank"] != bank:
                    raise ValueError("Bank change requires an explicit replay event")
                if record["sprites"]["ActiveMask"] != "0" or record["sprites"]["DisplayMask"] != "0":
                    raise ValueError("Active sprites are outside this replay")
            memory, colors, conflicts, rows = {}, {}, [], []
            for record in selected:
                internal = record["internal"]
                if "g" in record:
                    g = record["g"]
                    address, value = int(g["Address"].lstrip("$"), 16), int(g["Data"], 16)
                    if address in memory and memory[address] != value:
                        conflicts.append((record["absolute"], address, memory[address], value))
                    memory[address] = value
                if "c" in record and record["c"]["Invalid"] == "0":
                    c = record["c"]
                    address = int(record["memory"]["Screen"].lstrip("$"), 16) + int(c["VC"])
                    value = int(c["Screen"], 16)
                    if address in memory and memory[address] != value:
                        conflicts.append((record["absolute"], address, memory[address], value))
                    memory[address] = value
                    vc, color = int(c["VC"]), int(c["Color"], 16)
                    if vc in colors and colors[vc] != color:
                        raise ValueError("Color RAM changes require explicit events")
                    colors[vc] = color
                g = record.get("g")
                c = record.get("c")
                rows.append([record["absolute"], int(internal["ROW"]), int(internal["Cycle"]), record["nibble"],
                             [] if g is None else [int(g["Address"].lstrip("$"), 16), int(g["Data"], 16), int(g["Screen"], 16), int(g["Color"], 16)],
                             [] if c is None else [int(c["VLMI"]), int(c["Screen"], 16), int(c["Color"], 16), int(c["Invalid"])], record.get("writes", [])])
            initial = selected[0]
            if conflicts:
                raise ValueError("RAM changes require explicit events: " + repr(conflicts))
            matrix = [[0, 0] for index in range(40)]
            if first == 52:
                for record in selected:
                    if int(record["internal"]["ROW"]) == 54 and "g" in record and int(record["g"]["VLMI"]) >= 23:
                        g = record["g"]
                        matrix[int(g["VLMI"])] = [int(g["Screen"], 16), int(g["Color"], 16)]
            print(json.dumps(dict(records=rows, memory=sorted(memory.items()), colors=sorted(colors.items()),
                                  conflicts=conflicts, initial=initial, matrix=matrix)))
            return self.records
        for row in sorted({int(record.get("internal", {}).get("ROW", -1)) for record in self.records}):
            selected = [record for record in self.records if int(record.get("internal", {}).get("ROW", -1)) == row]
            c = [(int(record["internal"]["Cycle"]), record["c"]) for record in selected if "c" in record]
            print("C", row, len(c), c[:2], c[-1:])
        for record in self.records:
            internal = record.get("internal", {})
            if internal.get("Cycle") == "1":
                print(record["absolute"], internal, "regs", record["registers"], "memory", record.get("memory"))
        for record in self.records:
            if "writes" in record:
                print("WRITE", record["absolute"], record["internal"].get("ROW"), record["internal"].get("Cycle"), record["writes"])
        return self.records


if __name__ == "__main__":
    if len(sys.argv) > 3 and sys.argv[3] == "--members":
        print(json.dumps(Acquisition.members_replay(sys.argv[1], int(sys.argv[2]),
                                                   int(sys.argv[4]) if len(sys.argv) > 4 else 66)))
    else:
        Acquisition(sys.argv[1], int(sys.argv[2])).run()

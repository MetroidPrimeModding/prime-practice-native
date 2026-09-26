#!/usr/bin/env python3
"""Look up which section, TU, and symbol an address belongs to, using the prime-decomp MWLD link map."""
import argparse
import bisect
import re
import sys
from dataclasses import dataclass
from pathlib import Path

DEFAULT_DECOMP = Path.home() / "projects/vm-temp/prime-decomp"

SECTION_HEADER = re.compile(r"^(\S+) section layout$")
# "  00000018 005af0 80003658  1 .text \tmain.o "; the alignment column is absent on "(entry of ...)" lines.
LAYOUT_LINE = re.compile(r"^  ([0-9a-f]{8}) ([0-9a-f]{6,}) ([0-9a-f]{8})(?: +(\d+))? (.*?) ?\t(.*?) *$")
# Dead-stripped code; its size is still counted in the owning TU's section entry. Even after subtracting these, TU
# sizes can overshoot, so each TU's end is also clamped to the next TU's start.
UNUSED_LINE = re.compile(r"^  UNUSED +([0-9a-f]+) \.+ ")


@dataclass
class Entry:
    addr: int
    size: int
    name: str
    tu: str

    @property
    def end(self) -> int:
        return self.addr + self.size


@dataclass
class Section:
    name: str
    tus: list[Entry]
    symbols: list[Entry]


def configured_version(decomp: Path) -> str:
    ninja = decomp / "build.ninja"
    if ninja.exists():
        m = re.search(r"build/(\w+)/main\.elf", ninja.read_text())
        if m:
            return m.group(1)
    return "GM8E01_00"


def parse_map(path: Path) -> list[Section]:
    sections: list[Section] = []
    cur = None
    in_layouts = False
    with path.open(errors="replace") as f:
        for line in f:
            line = line.rstrip("\r\n")
            if m := SECTION_HEADER.match(line):
                in_layouts = True
                cur = Section(m.group(1), [], [])
                sections.append(cur)
                continue
            if not in_layouts:
                continue
            if line.startswith("Memory map:"):
                break
            if cur is None:
                continue
            if (m := UNUSED_LINE.match(line)) and cur.tus:
                cur.tus[-1].size -= int(m.group(1), 16)
                continue
            m = LAYOUT_LINE.match(line)
            if not m:
                continue
            size, addr, name, tu = int(m.group(2), 16), int(m.group(3), 16), m.group(5), m.group(6)
            # The first entry per TU names the section itself and spans that TU's whole contribution.
            if name == cur.name:
                cur.tus.append(Entry(addr, size, name, tu))
            elif size > 0:
                cur.symbols.append(Entry(addr, size, name, tu))
    for s in sections:
        s.tus.sort(key=lambda e: e.addr)
        for a, b in zip(s.tus, s.tus[1:]):
            a.size = min(a.size, b.addr - a.addr)
        s.symbols.sort(key=lambda e: e.addr)
    return [s for s in sections if s.tus or s.symbols]


def find_containing(entries: list[Entry], starts: list[int], addr: int):
    """Returns (entry containing addr, or None; nearest entry starting at or before addr, or None)."""
    i = bisect.bisect_right(starts, addr) - 1
    if i < 0:
        return None, None
    # Symbols can nest (e.g. a local label inside a function), so scan back a little for the innermost hit.
    for j in range(i, max(i - 8, -1), -1):
        if entries[j].addr <= addr < entries[j].end:
            return entries[j], entries[i]
    return None, entries[i]


def fmt_sym(e: Entry, addr: int) -> str:
    rel = addr - e.addr
    off = f"+0x{rel:x}" if rel >= 0 else f"-0x{-rel:x}"
    return f"{e.name} ({e.tu})  [0x{e.addr:08x}-0x{e.end:08x}) {off}"


def lookup(index, addr: int) -> str:
    for s, tu_starts, sym_starts, lo, hi in index:
        if not lo <= addr < hi:
            continue
        tu, _ = find_containing(s.tus, tu_starts, addr)
        sym, _ = find_containing(s.symbols, sym_starts, addr)
        lines = [f"0x{addr:08x}:", f"  section: {s.name}"]
        # Common (uninitialized global) symbols are laid out after every TU's section chunk, with no chunk line of
        # their own, so the symbol's TU is the only one available for them.
        if tu and (sym is None or sym.tu == tu.tu):
            lines.append(f"  TU:      {tu.tu}  [0x{tu.addr:08x}-0x{tu.end:08x}) +0x{addr - tu.addr:x}")
        elif sym:
            lines.append(f"  TU:      {sym.tu}  (common symbol, no TU chunk)")
        if sym:
            lines.append(f"  symbol:  {fmt_sym(sym, addr)}")
        else:
            i = bisect.bisect_right(sym_starts, addr)
            prev = s.symbols[i - 1] if i > 0 else None
            nxt = s.symbols[i] if i < len(s.symbols) else None
            lines.append("  symbol:  (none; nearest used symbols)")
            if prev:
                lines.append(f"    prev:  {fmt_sym(prev, addr)}")
            if nxt:
                lines.append(f"    next:  {fmt_sym(nxt, addr)}")
        return "\n".join(lines)
    return f"0x{addr:08x}: not in any section"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("addresses", nargs="+", help="addresses (hex, 0x prefix optional)")
    parser.add_argument("--map", type=Path, help="path to main.elf.MAP (overrides --decomp/--version)")
    parser.add_argument("--decomp", type=Path, default=DEFAULT_DECOMP, help=f"decomp checkout (default {DEFAULT_DECOMP})")
    parser.add_argument("--version", help="game version, e.g. GM8E01_00 (default: configured in build.ninja)")
    args = parser.parse_args()

    map_path = args.map or args.decomp / "build" / (args.version or configured_version(args.decomp)) / "main.elf.MAP"
    if not map_path.exists():
        print(f"map not found: {map_path}", file=sys.stderr)
        return 1

    sections = parse_map(map_path)
    index = []
    for s in sections:
        entries = s.tus + s.symbols
        lo, hi = min(e.addr for e in entries), max(e.end for e in entries)
        index.append((s, [e.addr for e in s.tus], [e.addr for e in s.symbols], lo, hi))
    for a in args.addresses:
        try:
            addr = int(a, 16)
        except ValueError:
            print(f"bad address: {a}", file=sys.stderr)
            continue
        print(lookup(index, addr))
    return 0


if __name__ == "__main__":
    sys.exit(main())

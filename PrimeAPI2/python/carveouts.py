#!/usr/bin/env python3
"""Carveout handling for the two-pass link.

  header   - C header with the selected carveouts, for _earlyboot_memset to skip
  ldscript - linker script with nothing in carveouts (pass 1)
  pack     - pack input sections from a pass-1 map into carveouts, emit the pass-2 linker script
"""
import argparse
import fnmatch
import json
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

# Output sections in the template that hold packable input sections during pass 1.
MAINDATA_OUTPUT_SECTIONS = {".text", ".data"}

# lld map line: VMA LMA Size Align <indent><text>. Output sections have no extra indent, input
# sections 8 extra spaces, symbols 16.
MAP_LINE = re.compile(r"^\s*([0-9a-f]+)\s+([0-9a-f]+)\s+([0-9a-f]+)\s+(\d+) ( *)(.*)$")
INPUT_SECTION = re.compile(r"^(.*):\((.*)\)$")


@dataclass
class Carveout:
  name: str
  start: int
  size: int
  priority: int
  # Segments reloaded through DI on reset (OSReboot) are DMA'd with the address and length rounded down to 32
  # bytes, so each segment must start and end on a 32-byte boundary to load where it was linked. Stomps are
  # written into an existing DOL section instead, so they only need instruction alignment.
  seg_align: int = 32

  @property
  def load_start(self):
    return align_up(self.start, self.seg_align)

  @property
  def load_end(self):
    return (self.start + self.size) // self.seg_align * self.seg_align

  @property
  def load_size(self):
    return max(0, self.load_end - self.load_start)


@dataclass
class Item:
  section: str
  size: int
  align: int


@dataclass
class Bin:
  carveout: Carveout
  limit: int
  items: list = field(default_factory=list)
  max_align: int = 1
  cursor: int = 0

  def __post_init__(self):
    self.cursor = self.carveout.load_start

  def _layout_end(self, items, max_align):
    # lld aligns the output section itself to the largest input alignment
    pos = align_up(self.carveout.load_start, max_align)
    for it in items:
      pos = align_up(pos, it.align) + it.size
    return pos

  def try_add(self, item):
    if item.align > self.max_align:
      end = self._layout_end(self.items + [item], item.align)
      if end > self.limit:
        return False
      self.max_align = item.align
      self.cursor = end
    else:
      end = align_up(self.cursor, item.align) + item.size
      if end > self.limit:
        return False
      self.cursor = end
    self.items.append(item)
    return True

  def used(self):
    return align_up(self.cursor, self.carveout.seg_align) - self.carveout.load_start if self.items else 0


def align_up(v, a):
  return (v + a - 1) // a * a


def load_config(path):
  cfg = json.loads(Path(path).read_text())
  carveouts = [
    Carveout(c["name"], int(c["start"], 0), int(c["size"], 0), int(c["priority"]))
    for c in cfg["carveouts"]
  ]
  stomps = [
    Carveout(c["name"], int(c["start"], 0), int(c["size"], 0), 0, seg_align=4)
    for c in cfg.get("stomps", [])
  ]
  names = [c.name for c in carveouts + stomps]
  if len(names) != len(set(names)):
    sys.exit(f"{path}: duplicate carveout names")
  cfg["carveouts"] = carveouts
  cfg["stomps"] = merge_adjacent(stomps)
  return cfg


def merge_adjacent(stomps):
  """Stomps are listed per function; adjacent ones become one region so larger sections fit."""
  merged = []
  for c in sorted(stomps, key=lambda c: c.start):
    if merged and c.start <= merged[-1].start + merged[-1].size:
      last = merged[-1]
      last.size = max(last.size, c.start + c.size - last.start)
    else:
      merged.append(Carveout(f"stomp_{c.start:08X}", c.start, c.size, 0, seg_align=4))
  return merged


def selected_carveouts(cfg):
  ranked = sorted(cfg["carveouts"], key=lambda c: (c.priority, -c.size, c.start))
  return ranked[: cfg["max_carveouts"]]


def matches_any(name, patterns):
  return any(fnmatch.fnmatchcase(name, p) for p in patterns)


def parse_map(map_path, cfg):
  items = {}
  current_out = None
  for line in Path(map_path).read_text().splitlines():
    m = MAP_LINE.match(line)
    if not m:
      continue
    size, align, indent, text = int(m.group(3), 16), int(m.group(4)), len(m.group(5)), m.group(6)
    if indent == 0:
      current_out = text
      continue
    if indent != 8 or current_out not in MAINDATA_OUTPUT_SECTIONS:
      continue
    im = INPUT_SECTION.match(text)
    if not im:
      continue
    obj, section = im.group(1), im.group(2)
    # <internal> sections are lld-synthesized (merged strings/constants) and can't be selected by name
    if obj == "<internal>" or size == 0:
      continue
    if not matches_any(section, cfg["pack_input_sections"]) or matches_any(section, cfg["never_pack"]):
      continue
    # Same-named sections from different objects all match one rule, so they must be placed together.
    if section in items:
      it = items[section]
      it.align = max(it.align, align)
      it.size = align_up(it.size, it.align) + size
    else:
      items[section] = Item(section, size, align)
  return list(items.values())


def pack(items, cfg):
  # Stomps first: they don't use a DOL section slot, so filling them may leave a carveout segment empty.
  bins = [Bin(c, c.load_end) for c in cfg["stomps"]]
  bins += [Bin(c, c.load_end - cfg["reserve_bytes"]) for c in selected_carveouts(cfg)]
  leftover = []
  for item in sorted(items, key=lambda i: (-i.size, i.section)):
    if not any(b.try_add(item) for b in bins):
      leftover.append(item)
  return bins, leftover


def render_ldscript(template_path, bins):
  phdrs = []
  sections = []
  for b in sorted((b for b in bins if b.items), key=lambda b: b.carveout.start):
    c = b.carveout
    phdrs.append(f"  {c.name} PT_LOAD;")
    rules = "\n".join(f"    *({it.section})" for it in b.items)
    sections.append(
      f"  . = 0x{c.load_start:08X};\n"
      f"  .carveout_{c.name} :\n"
      f"  {{\n"
      f"{rules}\n"
      f"    . = ALIGN({c.seg_align});\n"
      f'    ASSERT(. <= 0x{c.load_end:08X}, "carveout {c.name} overflow");\n'
      f"  }} :{c.name}"
    )
  text = Path(template_path).read_text()
  text = text.replace("@CARVEOUT_PHDRS@", "\n".join(phdrs))
  text = text.replace("@CARVEOUT_SECTIONS@", "\n\n".join(sections))
  return text


def write(path, text):
  p = Path(path)
  p.parent.mkdir(parents=True, exist_ok=True)
  p.write_text(text)


def cmd_header(args):
  cfg = load_config(args.config)
  blocks = sorted(selected_carveouts(cfg), key=lambda c: c.start)
  lines = ["#pragma once", "", "#include <types.h>", "",
           "// Generated from carveouts.json: {start, size} pairs sorted by address.",
           "static const u32 CARVEOUT_SAFE_BLOCKS[] = {"]
  lines += [f"    0x{c.start:08X}, 0x{c.size:X}, // {c.name}" for c in blocks]
  lines += ["};", ""]
  write(args.out, "\n".join(lines))


def cmd_ldscript(args):
  write(args.out, render_ldscript(args.template, []))


def cmd_pack(args):
  cfg = load_config(args.config)
  items = parse_map(args.map, cfg)
  if not items:
    sys.exit(f"{args.map}: found no packable input sections; is this a pass-1 map?")
  bins, leftover = pack(items, cfg)
  write(args.out, render_ldscript(args.template, bins))

  report = []
  total_used = total_cap = 0
  for b in bins:
    c = b.carveout
    used = b.used()
    total_used += used
    total_cap += c.load_size
    report.append(f"{c.name:<24} 0x{c.load_start:08X} used 0x{used:05X} / 0x{c.load_size:05X} "
                  f"free 0x{c.load_size - used:05X}  ({len(b.items)} sections)")
  left_size = sum(i.size for i in leftover)
  report.append(f"{'total':<24} {'':10} used 0x{total_used:05X} / 0x{total_cap:05X}")
  report.append(f"left in maindata: 0x{left_size:X} bytes of packable sections ({len(leftover)} sections)")
  report.append("largest leftovers:")
  report += [f"  0x{i.size:05X} {i.section}" for i in leftover[:10]]
  text = "\n".join(report) + "\n"
  print(text, end="")
  if args.report:
    Path(args.report).write_text(text)


def main():
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  sub = parser.add_subparsers(dest="cmd", required=True)

  p = sub.add_parser("header")
  p.add_argument("--config", required=True)
  p.add_argument("--out", required=True)
  p.set_defaults(func=cmd_header)

  p = sub.add_parser("ldscript")
  p.add_argument("--template", required=True)
  p.add_argument("--out", required=True)
  p.set_defaults(func=cmd_ldscript)

  p = sub.add_parser("pack")
  p.add_argument("--config", required=True)
  p.add_argument("--template", required=True)
  p.add_argument("--map", required=True)
  p.add_argument("--out", required=True)
  p.add_argument("--report")
  p.set_defaults(func=cmd_pack)

  args = parser.parse_args()
  args.func(args)


if __name__ == "__main__":
  main()

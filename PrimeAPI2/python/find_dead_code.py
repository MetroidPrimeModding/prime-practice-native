#!/usr/bin/env python3
"""Find game code/data that becomes dead once some functions are dead, as candidates for carveouts.json "stomps".

Builds a reference graph from the prime decomp's disassembly (build/GM8E01_00/asm after `ninja`). Symbols nothing
references (entry points, interrupt handlers, ...) and symbols the mod uses (src/prime-practice.lst) are roots;
the result is everything reachable from them that stops being reachable once the seeds are removed. Unlike
"dead if every referrer is dead", this also catches dead cycles such as a vtable and its destructor.

  find_dead_code.py --decomp ~/projects/vm-temp/prime-decomp SEED_SYMBOL...

Caveats, check each result before stomping it:
  - Seeds are assumed never called, even though live code still references them. Something that calls a seed with
    null (e.g. CodeWarrior destructors check `this` themselves) keeps it alive; don't seed those.
  - Code outside the DOL (RELs, randomizer patches) and computed addresses aren't seen. Code that was already
    unreachable (e.g. only referenced from a cycle) isn't reported.
  - Verify the bytes against the base DOL; the decomp must match GM8E01_00 there.
"""
import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path

# Names with template args are quoted and may contain commas: .fn "vector<a,b>::f", weak
DEF = re.compile(r'^\.(fn|obj)\s+("[^"]+"|[^,\s]+)(?:,\s*(\w+))?')
END = re.compile(r"^\.end(fn|obj)\s")
SECTION = re.compile(r"^(\.text|\.data|\.rodata|\.init|\.bss|\.sdata2?|\.sbss2?)\b|^\.section\s+([^,\s]+)")
SIZE_COMMENT = re.compile(r"^# \.\S+:0x[0-9A-F]+ \| 0x([0-9A-F]{8}) \| size: 0x([0-9A-F]+)")
TOKEN = re.compile(r'"[^"]+"|[A-Za-z_$@.][\w$@.<>,:\-]*')
RELOC_SUFFIX = re.compile(r"@(ha|h|l|sda21|sda2)$")
# Exception tables reference every function they cover; that isn't a use.
IGNORED_REFERRER_SECTIONS = {"extab", "extabindex"}
MOD = ("", "<mod>")


class Program:
  def __init__(self, asm_dir):
    self.defs = {}  # key -> dict(name, file, section, addr, size); key is (file, name) for locals, ("", name) else
    self.locals = defaultdict(dict)
    self.globals = {}
    self.refs = defaultdict(set)  # target key -> referrer keys
    body = defaultdict(list)
    for f in sorted(asm_dir.rglob("*.s")):
      self._parse(f, body)
    for key, lines in body.items():
      if self.defs[key]["section"] not in IGNORED_REFERRER_SECTIONS:
        self._add_refs(key, lines)

  def _parse(self, f, body):
    section = cur = pending = None
    for line in f.read_text(errors="replace").splitlines():
      s = line.strip()
      if m := SECTION.match(s):
        section = m.group(2) or m.group(1)
      elif m := SIZE_COMMENT.match(s):
        pending = (int(m.group(1), 16), int(m.group(2), 16))
      elif m := DEF.match(s):
        name = m.group(2).strip('"')
        local = m.group(3) == "local"
        cur = (str(f), name) if local else ("", name)
        addr, size = pending or (None, None)
        self.defs[cur] = dict(name=name, file=str(f), section=section, addr=addr, size=size)
        (self.locals[str(f)] if local else self.globals)[name] = cur
        pending = None
      elif END.match(s):
        cur = None
      elif cur is not None:
        body[cur].append(s)

  def resolve(self, file, name):
    return self.locals[file].get(name) or self.globals.get(name)

  def _add_refs(self, key, lines):
    file = self.defs[key]["file"]
    for s in lines:
      code = s.split("*/", 1)[-1].split("#", 1)[0]
      for tok in TOKEN.findall(code):
        target = self.resolve(file, RELOC_SUFFIX.sub("", tok.strip('"')))
        if target and target != key:
          self.refs[target].add(key)


def main():
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument("--decomp", type=Path, required=True, help="prime decomp checkout, built for GM8E01_00")
  parser.add_argument("--mod-symbols", type=Path,
                      default=Path(__file__).resolve().parents[2] / "src" / "prime-practice.lst")
  parser.add_argument("seeds", nargs="+", help="mangled names of functions that will never be called")
  args = parser.parse_args()

  prog = Program(args.decomp / "build" / "GM8E01_00" / "asm")
  for line in args.mod_symbols.read_text().splitlines():
    parts = line.split()
    if len(parts) >= 2 and (k := prog.globals.get(parts[1])):
      prog.refs[k].add(MOD)

  seeds = set()
  for s in args.seeds:
    if s not in prog.globals:
      sys.exit(f"unknown seed {s}")
    seeds.add(prog.globals[s])

  uses = defaultdict(set)  # referrer -> targets
  for target, referrers in prog.refs.items():
    for r in referrers:
      uses[r].add(target)
  roots = [MOD] + [k for k in prog.defs if not prog.refs.get(k)]

  def reachable(blocked):
    seen = set()
    stack = [r for r in roots if r not in blocked]
    while stack:
      k = stack.pop()
      if k in seen:
        continue
      seen.add(k)
      stack.extend(t for t in uses[k] if t not in blocked)
    return seen

  dead = (reachable(set()) - reachable(seeds)) | seeds
  dead.discard(MOD)

  for k in sorted(dead, key=lambda k: prog.defs[k]["addr"] or 0):
    d = prog.defs[k]
    print(f"{d['section']:<8} 0x{d['addr']:08X} 0x{d['size']:05X} {d['name']}  [{Path(d['file']).name}]")


if __name__ == "__main__":
  main()

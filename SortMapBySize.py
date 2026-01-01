#!/usr/bin/env python3
import argparse
import re
import sys
from pathlib import Path
from typing import Optional


# format:
# VMA      LMA     Size Align Out     In      Symbol
# 0        0 8056a7e4     1 . = 0x8056A7E4
# 8056a7e4 8056a7e4        0     1 .gap_vsnprintf
# 8056a7e4 8056a7e4        0     1         . = ALIGN ( 4 )
# 8056a7e4 8056a7e4        0     1
# 8056a7e4 8056a7e4        0     1 _GAP_VSNPRINGF_END = .

def parse_line_size(line: str) -> Optional[int]:
    if line.startswith("     VMA"):
        return None
    parts = line.split()
    if len(parts) < 3:
        return None
    size_str = parts[2]
    return int(size_str, 16)


def sort_map_by_size(input_path: Path, output_path: Path) -> int:
    try:
        lines = input_path.read_text().splitlines(keepends=True)
    except FileNotFoundError:
        print(f"Input file not found: {input_path}", file=sys.stderr)
        return 1

    entries = []
    for index, line in enumerate(lines):
        size = parse_line_size(line)
        if size is not None:
            entries.append((size, index, line))

    entries.sort(key=lambda item: (-item[0], item[1]))

    output_path.write_text("".join(line for _, __, line in entries))
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Sort linker map entries by size (descending)."
    )
    parser.add_argument(
        "-i",
        "--input",
        default="cmake-build-release-docker/prime-practice.map",
        help="Path to the map file to sort.",
    )
    parser.add_argument(
        "-o",
        "--output",
        default="cmake-build-release-docker/sorted-prime-practice.map",
        help="Path for the sorted output file.",
    )
    args = parser.parse_args()

    return sort_map_by_size(Path(args.input), Path(args.output))


if __name__ == "__main__":
    raise SystemExit(main())

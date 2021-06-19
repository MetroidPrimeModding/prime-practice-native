import argparse
import mmap
import sys

from src.DataReader import DataReader
from src.DataWriter import DataWriter
from src.GCDisc import GCDiscHeader, FST, FSTEntry
from src.DolPatcher import patch_dol

import hashlib

KNOWN_MP1_MD5 = "eeacd0ced8e2bae491eca14f141a4b7c"


def patch_iso_file(inp_path, out_path, mod_path, ignore_hash=False):
    # Set up the input
    print("Preparing...")
    inp_file = open(inp_path, 'rb')
    inp_mmap = mmap.mmap(inp_file.fileno(), 0, access=mmap.ACCESS_READ)
    inp_reader_root = DataReader(inp_mmap, 0)

    if ignore_hash:
        print("Skipping hash check")
    else:
        print("Calculating hash...")
        m = hashlib.md5()
        m.update(inp_mmap)
        calculated_hash = m.hexdigest()
        print(f"Expected: {calculated_hash}")
        print(f"Actual:   {calculated_hash}")
        if calculated_hash != KNOWN_MP1_MD5:
            print("This is not an unmodified mp1 0-00 NTSC iso!")
            print("*** THIS MOST LIKELY WILL NOT WORK ***")
            try_anyway = input("Try anyway? (y/n)").lower()
            if try_anyway == 'y':
                print("Trying anyway")
            else:
                print("Quitting.")
                sys.exit(1)

    # Parse header & fst
    print("Loading FST")
    header = GCDiscHeader.parse(inp_reader_root)
    fst = FST.parse(inp_reader_root.with_offset(header.fst_offset))

    # Find the first file
    print("Finding suitable file gap")
    ranges = fst.get_ranges()
    first_file = ranges[0][0]
    if first_file < 20 * 1000 * 1000:
        print("Not enough room for practice mod at start of ISO... is this an unmodified ISO?")
        sys.exit(1)

    # Ok, find and extract the dol
    print("Extracting DOL")
    dol_fst_entry = fst.find_offset(header.dol_offset)
    unpatched_dol_bytes = inp_reader_root.with_offset(dol_fst_entry.offset).read_bytes(0, dol_fst_entry.length)

    # Write it to disk
    # TODO: just do this all in memory
    # Involves re-writing DolFile, but it shouldn't be *too* bad
    patched_dol_bytes = patch_dol(mod_path, unpatched_dol_bytes)

    print("Loading bnr")
    with open("opening_practice.bnr", 'rb') as f:
        bnr_bytes = f.read()

    # Figure out our new offsets
    print("Determining file offsets for new files")
    dol_offset = first_file - len(patched_dol_bytes)
    dol_offset -= dol_offset % 8192

    # Build our new FST entries
    patched_dol_fst_entry = FSTEntry(
        directory=False,
        name="default_mod.dol",
        offset=dol_offset,
        length=len(patched_dol_bytes)
    )

    # Add them to the root entry
    print("Patching FST")
    fst.root.children.append(patched_dol_fst_entry)
    # It's time: copy the old
    print("Copying iso (note: this may take some time)")
    out_file = open(out_path, "wb+")
    out_file.truncate(inp_mmap.size())
    out_file.flush()
    out_mmap = mmap.mmap(out_file.fileno(), inp_mmap.size(), access=mmap.ACCESS_DEFAULT)

    # Copy all dat data over
    out_mmap.write(inp_mmap)
    print("Done copying.")

    # Patch time
    print("Patching FST")
    out_writer = DataWriter(out_mmap)
    out_reader = DataReader(out_mmap)
    fst_len = fst.write(out_writer.with_offset(header.fst_offset))
    fst_read = FST.parse(out_reader.with_offset(header.fst_offset))

    # fst.print()
    # fst_read.print()

    print("Updating header")
    header.fst_size = fst_len
    header.fst_max_size = fst_len
    header.dol_offset = patched_dol_fst_entry.offset
    header.write(out_writer)

    print("Writing mod & dol")
    out_writer.write_bytes(patched_dol_fst_entry.offset, patched_dol_bytes)

    print("patching bnr")
    out_writer.write_string(0x20, "Metroid Prime Practice Mod")
    bnr_fst = fst.find(["opening.bnr"])
    out_writer.write_bytes(bnr_fst.offset, bnr_bytes)

    # Cleanup
    print("Closing files")
    inp_mmap.close()
    out_mmap.flush()
    out_mmap.close()

    print("Done!")
    pass


def patch_dol_file(inp_path, out_path, mod_path):
    print(f"Loading {inp_path}")
    with open(inp_path, 'rb') as f:
        dol_bytes = f.read()
    print(f"{len(dol_bytes)} bytes")
    out_bytes = patch_dol(mod_path, dol_bytes)
    print(f"Writing {out_path}")
    with open(out_path, 'wb') as f:
        f.write(out_bytes)
    print(f"{len(out_bytes)} bytes")
    print(f"Mod size: {len(out_bytes) - len(dol_bytes)} bytes")


def main():
    parser = argparse.ArgumentParser(
        description="Patch a MP1 0-00 dol or ISO to the practice mod"
    )
    subparsers = parser.add_subparsers(help="sub-command help", dest="command", required=True)

    patch_iso_parser = subparsers.add_parser('iso', help="Patch an ISO to practice mod")
    patch_iso_parser.add_argument("--input", '-i', required=True, help="Input iso file")
    patch_iso_parser.add_argument("--output", '-o', required=True, help="Output iso file")
    patch_iso_parser.add_argument("--mod", '-m', required=True, help="Statically linked elf to insert")
    patch_iso_parser.add_argument("--skip-hash", dest="skip_hash", action="store_true", help="Skip hash check")

    patch_dol_parser = subparsers.add_parser('dol', help="Patch an ISO to practice mod")
    patch_dol_parser.add_argument("--input", '-i', required=True, help="Input dol file")
    patch_dol_parser.add_argument("--mod", '-m', required=True, help="Statically linked elf to insert")
    patch_dol_parser.add_argument("--output", '-o', required=True, help="Output iso file")

    args = parser.parse_args()

    if args.command == 'iso':
        inp_path = args.input
        out_path = args.output
        mod_path = args.mod
        skip_hash = args.skip_hash

        patch_iso_file(inp_path, out_path, mod_path, ignore_hash=skip_hash)
    else:
        inp_path = args.input
        out_path = args.output
        mod_path = args.mod

        patch_dol_file(inp_path, out_path, mod_path)


if __name__ == '__main__':
    main()

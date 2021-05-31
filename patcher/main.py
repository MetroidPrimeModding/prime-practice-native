import argparse
import mmap
import os
import sys

from DataReader import DataReader
from DataWriter import DataWriter
from formats.GCDisc import GCDiscHeader, FST, FSTEntry
from dol_patcher.dol import DolFile


def patch_iso(inp_path, out_path, mod_path):
    # Set up the input
    print("Preparing...")
    inp_file = open(inp_path, 'rb')
    inp_mmap = mmap.mmap(inp_file.fileno(), 0, access=mmap.ACCESS_READ)
    inp_reader_root = DataReader(inp_mmap, 0)

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
    with open("default.dol", "wb") as f:
        f.write(unpatched_dol_bytes)

    # patch said dol
    print("Patching dol")
    dol_file = DolFile()
    dol_file.read("default.dol")
    if dol_file.buildVersion >= 2:
        print("This isn't a Prime dol file, so I can't patch it")
        sys.exit(1)

    if not dol_file.load_symbols("symbols"):
        raise IOError("Unable to load symbols")

    if not dol_file.apply_patch("DolPatch.bin", "default_mod.dol"):
        print("Failed to patch dol")
        sys.exit(1)

    # read it back from disk
    with open("default_mod.dol", 'rb') as f:
        patched_dol_bytes = f.read()
        # patched_dol_bytes = unpatched_dol_bytes

    # Load mod file
    print("Loading mod")
    with open(mod_path, 'rb') as f:
        mod_rel_bytes = f.read()

    print("Loading bnr")
    with open("opening_practice.bnr", 'rb') as f:
        bnr_bytes = f.read()

    # Figure out our new offsets
    print("Determining file offsets for new files")
    dol_offset = first_file - len(patched_dol_bytes)
    dol_offset -= dol_offset % 8192
    mod_rel_offset = dol_offset - len(mod_rel_bytes)
    mod_rel_offset -= mod_rel_offset % 8192

    # Build our new FST entries
    patched_dol_fst_entry = FSTEntry(
        directory=False,
        name="default_mod.dol",
        offset=dol_offset,
        length=len(patched_dol_bytes)
    )
    mod_rel_fst_entry = FSTEntry(
        directory=False,
        name="Mod.rel",
        offset=mod_rel_offset,
        length=len(mod_rel_bytes)
    )

    # Add them to the root entry
    print("Patching FST")
    fst.root.children.append(patched_dol_fst_entry)
    fst.root.children.append(mod_rel_fst_entry)
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
    out_writer.write_bytes(mod_rel_fst_entry.offset, mod_rel_bytes)

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


def main():
    parser = argparse.ArgumentParser(
        description="Patch a MP1 0-00 ISO to the practice mod"
    )
    parser.add_argument("--input", '-i', required=True, help="Input iso file")
    parser.add_argument("--output", '-o', required=True, help="Output iso file")
    parser.add_argument("--mod", '-m', required=True, help="Mod.rel to insert")

    args = parser.parse_args()
    inp_path = args.input
    out_path = args.output
    mod_path = args.mod

    patch_iso(inp_path, out_path, mod_path)


if __name__ == '__main__':
    main()

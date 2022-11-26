import sys
from typing import Callable

from pyelftools.elftools.elf.elffile import ELFFile
from src.DataReader import DataReader
from src.DataWriter import DataWriter
from src.Dol import DolHeader

def patch_dol(mod_path: str, unpatched_dol_bytes: bytes) -> bytearray:
    header = DolHeader.parse(DataReader(unpatched_dol_bytes))
    out_bytes = bytearray()
    out_bytes.extend(unpatched_dol_bytes)

    # First, add the new sections
    with open(mod_path, 'rb') as f:
        elf_file = ELFFile(f)
        symbol_section = elf_file.get_section_by_name('.symtab')
        if not symbol_section:
            print("Unable to find symbol table")

        link_start = symbol_section.get_symbol_by_name("_LINK_START")[0].entry['st_value']
        link_end = symbol_section.get_symbol_by_name("_LINK_END")[0].entry['st_value']
        link_size = symbol_section.get_symbol_by_name("_LINK_SIZE")[0].entry['st_value']
        patch_arena_lo_1 = symbol_section.get_symbol_by_name("_PATCH_ARENA_LO_1")[0].entry['st_value']
        patch_arena_lo_2 = symbol_section.get_symbol_by_name("_PATCH_ARENA_LO_2")[0].entry['st_value']
        patch_earlyboot_memset = symbol_section.get_symbol_by_name("_PATCH_EARLYBOOT_MEMSET")[0].entry['st_value']
        patch_earlyboot_memset_addr = symbol_section.get_symbol_by_name("_earlyboot_memset")[0].entry['st_value']
        hook_addr = symbol_section.get_symbol_by_name("PPCSetFpIEEEMode")[0].entry['st_value']

        print(f"linking from 0x{link_start:08x} to 0x{link_end:08x} {link_size} bytes")

        entry_addr = elf_file.header['e_entry']

        for segment in elf_file.iter_segments():
            data = segment.data()
            addr = segment.header['p_paddr']

            segment_start = len(out_bytes)
            out_bytes.extend(data)


            # if segment.header['p_flags'] & 1 == 1 and False:
            #     print('Text segment')
            #     type = "text"
            #     target = header.text
            # else:
            #     print('Data segment')
            #     type = "data"
            #     target = header.data

            found = False
            for (i, seg) in enumerate(header.text):
                if seg.offset == 0:
                    found = True
                    seg.offset = segment_start
                    seg.loading = addr
                    seg.size = len(data)
                    print(f"Patching text segment {i}: offset {seg.offset:08X} loading {seg.loading:08X} size {seg.size:08X} end {(seg.loading + seg.size):08X}")
                    break

            for (i, seg) in enumerate(header.data):
                if found:
                    break
                if seg.offset == 0:
                    found = True
                    seg.offset = segment_start
                    seg.loading = addr
                    seg.size = len(data)
                    print(f"Patching data segment {i}: offset {seg.offset:08X} loading {seg.loading:08X} size {seg.size:08X} end {(seg.loading + seg.size):08X}")
                    break

            if not found:
                print("Unable to find empty dol segment for segment from elf file!")
                sys.exit(1)

            print(f"Data: {len(data)} @ 0x{addr:08x}-{(addr + len(data)):08x}")

    print("Updating header")
    header.write(DataWriter(out_bytes))

    def patch_addr_32(addr: int, function: Callable[[int], int]):
        for seg in header.text:
            if seg.loading <= addr < (seg.loading + seg.size):
                seg_offset = addr - seg.loading
                offset = seg.offset + seg_offset
                current = DataReader(out_bytes).read_u32(offset)
                new = function(current)
                print(f"Patching 0x{offset:08x} (0x{addr:08x}) from 0x{current:08x}->0x{new:08x}")
                DataWriter(out_bytes).write_u32(offset, new)

    def b_rel24(addr, target):
        rel = (target - addr) & 0xFFFF_FFFC
        op = 0x4800_0000
        return op | rel
    # Ok now patch the areanalo calls
    arenahi_upper = ((link_end >> 16) & 0xFFFF) + 1 # Not sure if this 1 is necessary, but it's probably not hurting much
    patch_addr_32(patch_arena_lo_1, lambda x: (x & 0xFFFF_0000) | arenahi_upper)
    patch_addr_32(patch_arena_lo_2, lambda x: (x & 0xFFFF_0000) | arenahi_upper)
    patch_addr_32(hook_addr, lambda x: b_rel24(hook_addr, entry_addr))
    patch_addr_32(patch_earlyboot_memset, lambda x: b_rel24(patch_earlyboot_memset, patch_earlyboot_memset_addr))


    return out_bytes

import os
import argparse
import cxxfilt
from mangle import mangle
from preplf import *
from stream import *
from dol import *

DIR = os.path.dirname(os.path.realpath(__file__))
dolFile = DolFile()

def main():
    parser = argparse.ArgumentParser(description='Patch generator')
    parser.add_argument(
        "-o", "--outRel",
        help="Output REL",
        required=True
    )
    parser.add_argument(
        "-i", "--inElf",
        help="Input ELF",
        required=True
    )
    parser.add_argument(
        "-d", "--inDol",
        help="Input dol",
        required=True
    )
    args = parser.parse_args()

    dolFile.read(args.inDol)

    if dolFile.buildVersion >= 3:
        print(
            "The specified dol file belongs to a Wii version of the game. The Wii versions are currently not supported.")
        return False

    if not dolFile.load_symbols("%s/symbols" % DIR):
        raise IOError("Unable to load symbols")

    if not convert_preplf_to_rel(args.inElf, args.outRel):
        return False

    return True


def convert_preplf_to_rel(preplfPath, outRelPath):
    print("Exporting %s to %s" % (preplfPath, outRelPath))
    preplf = PreplfFile(preplfPath)
    rel = OutputStream()

    # Initial header info
    rel.write_long(
        2)  # Module ID. Hardcoding 2 here because 0 is the dol and the game already has a module using 1 (NESemu.rel). Should be more robust ideally
    rel.write_long(0)  # Next module link - always 0
    rel.write_long(0)  # Prev module link - always 0
    rel.write_long(len(preplf.sections))  # Num sections
    rel.write_long(0)  # Section info offset filler
    rel.write_long(0)  # Module name offset (our rel won't include the module name so this is staying null)
    rel.write_long(0)  # Module name size
    rel.write_long(2)  # Module version

    # Fetch data needed for the rest of the header
    bssSec = preplf.section_by_name(".bss")
    assert (bssSec != None)

    prologSymbol = preplf.symbol_by_name("__rel_prolog")
    if prologSymbol is None:
        prologSymbol = preplf.symbol_by_name("__rel_prolog__Fv")

    epilogSymbol = preplf.symbol_by_name("_epilog")
    if epilogSymbol is None:
        epilogSymbol = preplf.symbol_by_name("_epilog__Fv")

    unresolvedSymbol = preplf.symbol_by_name("_unresolved")
    if unresolvedSymbol is None:
        unresolvedSymbol = preplf.symbol_by_name("_unresolved__Fv")

    # Remaining header data
    rel.write_long(bssSec.size)
    rel.write_long(0)  # Relocation table offset filler
    rel.write_long(0)  # Imports offset filler
    rel.write_long(0)  # Imports size filler
    rel.write_byte(prologSymbol['sectionIndex'] if prologSymbol is not None else 0)
    rel.write_byte(epilogSymbol['sectionIndex'] if epilogSymbol is not None else 0)
    rel.write_byte(unresolvedSymbol['sectionIndex'] if unresolvedSymbol is not None else 0)
    rel.write_byte(0)  # Padding
    rel.write_long(prologSymbol['value'] if prologSymbol is not None else 0)
    rel.write_long(epilogSymbol['value'] if epilogSymbol is not None else 0)
    rel.write_long(unresolvedSymbol['value'] if unresolvedSymbol is not None else 0)
    rel.write_long(8)  # Module alignment
    rel.write_long(8)  # BSS alignment

    # Section info filler
    sectionInfoOffset = rel.tell()

    for section in preplf.sections:
        rel.write_long(0)
        rel.write_long(0)

    # Write sections
    sectionInfoList = []
    wroteBss = False

    for section in preplf.sections:
        # Sections not in the to-keep list should be nulled out
        info = {}
        info['exec'] = (section.flags & 0x4) != 0

        secStart = rel.tell()
        name = section.name

        isBss = name == ".bss"
        keepSections = [
            ".text", ".rodata",
            ".ctors", ".dtors",
            ".data",
            ".init",
            ".rela.init", ".rela.text", ".rela.fini", ".rela.rodata", ".rela.eh_frame", ".rela.data",
            ".fini",
            ".eh_frame",  # Not actually used?
            # for rust
            ".got2", ".rela.got2",
            ".gcc_except_table"
        ]
        shouldKeep = name in keepSections
        if not shouldKeep:
            for sec in keepSections:
                if name.startswith(sec):
                    shouldKeep = True
                    break

        if shouldKeep is True:
            info['offset'] = secStart
            rel.write_bytes(section.data)
            rel.write_to_boundary(4)
            info['size'] = rel.tell() - secStart

        elif isBss is True:
            info['offset'] = 0
            info['size'] = section.size
            wroteBss = True

        elif name == ".group":
            info['offset'] = 0
            info['size'] = 0

        # elif ".rela" in name:
        #     info['offset'] = 0
        #     info['size'] = 0

        else:
            assert (not name or wroteBss is True), name
            info['offset'] = 0
            info['size'] = 0

        sectionInfoList.append(info)

    # Generate imports and write imports section filler
    imports = []
    moduleImports = {'moduleID': 2, 'relocsOffset': 0}
    dolImports = {'moduleID': 0, 'relocsOffset': 0}
    imports.append(moduleImports)
    imports.append(dolImports)

    importsOffset = rel.tell()

    for importIdx in range(0, len(imports)):
        rel.write_long(0)
        rel.write_long(0)

    importsSize = rel.tell() - importsOffset

    # Write relocations
    relocsOffset = rel.tell()
    relocWriteSuccess = True
    unresolvedSymbolCount = 0

    for importIdx in range(0, 2):
        imports[importIdx]['relocsOffset'] = rel.tell()
        isDol = imports[importIdx]['moduleID'] == 0

        for section in preplf.sections:
            if section.type != EST_RELA or len(section.relocs) == 0:
                continue

            symbolSection = section.link
            targetSection = section.targetSecIdx

            # Make sure we only write relocations for sections that were written to the file
            sectionInfo = sectionInfoList[targetSection]
            if sectionInfo['offset'] == 0:
                continue

            curOffset = 0
            wroteSectionCommand = False

            # Parse relocations
            for reloc in sorted(section.relocs, key=lambda x: x['offset']):
                # for reloc in section.relocs:
                symbol = preplf.fetch_symbol(symbolSection, reloc['symbolIdx'])
                assert (symbol != None)

                # DOL relocations have a section index of 0; internal relocations have a valid section index
                if (symbol['sectionIndex'] == 0) != isDol:
                    continue

                isAbs = symbol['sectionIndex'] == 65521

                # This is a valid relocation, so we have at least one - write the "change section" directive
                if not wroteSectionCommand:
                    rel.write_short(0)
                    rel.write_byte(R_DOLPHIN_SECTION)
                    rel.write_byte(targetSection)
                    rel.write_long(0)
                    wroteSectionCommand = True

                offset = reloc['offset'] - curOffset

                # Add "nop" directives to make sure the offset will fit
                # NOTE/TODO - not sure if this is actually supposed to be a signed offset - that needs to be verified
                while offset > 0xFFFF:
                    rel.write_short(0xFFFF)
                    rel.write_byte(R_DOLPHIN_NOP)
                    rel.write_byte(0)
                    rel.write_long(0)
                    offset -= 0xFFFF
                    curOffset += 0xFFFF

                # Write relocation
                rel.write_short(offset)
                relocType = reloc['relocType']
                if relocType == 26:
                    print("TWENTY SIX")
                    rel.write_byte(11) # I patched rel14 to be rel32
                    # rel.write_byte(26)
                elif relocType == 18:
                    print("EIGHTEEN")
                    # per the docs, R_PPC_PLTREL24 -> R_PPC_REL24
                    rel.write_byte(10)
                elif relocType == 11:
                    print("Uhoh, ELEVEN")
                else:
                    rel.write_byte(relocType)

                # Internal relocs are easy - just copy data from the ELF reloc/symbol
                if not isDol:
                    if isAbs:
                        # Absolute (e.g. in-dol) symbol
                        rel.write_byte(0)
                        rel.write_long(symbol['value'])
                    else:
                        rel.write_byte(symbol['sectionIndex'])
                        rel.write_long(symbol['value'] + reloc['addend'])
                        # this is basically just the section-relative offset to the symbol

                # DOL relocs will require looking up the address of the symbol in the DOL
                else:
                    symbolName = symbol['name']
                    demangled = cxxfilt.demangle(symbolName)
                    remangled = demangled
                    if ('(' in demangled and ')' in demangled) or '::' in demangled or 'operator' in demangled:
                        remangled = mangle(demangled)
                    dolSymbolAddr = dolFile.get_symbol(remangled)

                    if dolSymbolAddr is None:
                        unresolvedSymbolCount += 1
                        print("Error: Failed to locate dol symbol: %s / %s (GCC: %s)" % (remangled, demangled, symbolName), symbol)
                    else:
                        unresolvedSymbolCount += 1
                        print("Missing symbol: 0x%x %s" % (dolSymbolAddr, symbolName))
                    rel.write_byte(0)
                    rel.write_long(0)
                    relocWriteSuccess = False
                    continue


                    # rel.write_byte(0)
                    # rel.write_long(dolSymbolAddr)

                curOffset += offset

        if unresolvedSymbolCount > 0:
            print("Failed to find %s symbols" % unresolvedSymbolCount)
        # Write "end" directive
        rel.write_short(0)
        rel.write_byte(R_DOLPHIN_END)
        rel.write_byte(0)
        rel.write_long(0)

    # Quit out?
    if not relocWriteSuccess:
        return False

    # Write filler values from the header
    rel.goto(0x10)
    rel.write_long(sectionInfoOffset)
    rel.goto(0x24)
    rel.write_long(relocsOffset)
    rel.write_long(importsOffset)
    rel.write_long(importsSize)

    rel.goto(sectionInfoOffset)

    for section in sectionInfoList:
        # Toggle 0x1 bit on the section offset for sections containing executable code
        offset = section['offset']
        if section['exec'] is True: offset |= 0x1

        rel.write_long(offset)
        rel.write_long(section['size'])

    rel.goto(importsOffset)

    for imp in imports:
        rel.write_long(imp['moduleID'])
        rel.write_long(imp['relocsOffset'])

    # Done
    rel.save_file(outRelPath)
    print("Saved REL to %s" % outRelPath)
    return True

if __name__ == "__main__":
    if not main():
        raise SystemExit(1)

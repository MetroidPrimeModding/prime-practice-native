from stream import *
import os
import struct


def list_as_hex(list):
    return [("0x%08X" % value) for value in list]


def extend_sign_bit(value, numBits):
    signBit = 1 << (numBits - 1)
    return (value & (signBit - 1)) - (value & signBit)


# Relocation type definitions
R_INVALID = -1
R_PPC_NONE = 0
R_PPC_ADDR32 = 1
R_PPC_ADDR24 = 2
R_PPC_ADDR16 = 3
R_PPC_ADDR16_LO = 4
R_PPC_ADDR16_HI = 5
R_PPC_ADDR16_HA = 6
R_PPC_ADDR14 = 7
R_PPC_ADDR14_BRTAKEN = 8
R_PPC_ADDR14_BRNTAKEN = 9
R_PPC_REL24 = 10
R_PPC_REL14 = 11
R_DOLPHIN_NOP = 201
R_DOLPHIN_SECTION = 202
R_DOLPHIN_END = 203


class DolFile:
    def __init__(self):
        self.valid = False

    def read(self, filename):
        self.filename = filename
        stream = InputStream(filename, BIG_ENDIAN)

        # Read DOL Header
        self.textSecOffsets = []
        for textSecIdx in range(0, 7):
            self.textSecOffsets.append(stream.read_long())

        self.dataSecOffsets = []
        for dataSecIdx in range(0, 11):
            self.dataSecOffsets.append(stream.read_long())

        self.textSecAddresses = []
        for textSecIdx in range(0, 7):
            self.textSecAddresses.append(stream.read_long())

        self.dataSecAddresses = []
        for dataSecIdx in range(0, 11):
            self.dataSecAddresses.append(stream.read_long())

        self.textSecSizes = []
        for textSecIdx in range(0, 7):
            self.textSecSizes.append(stream.read_long())

        self.dataSecSizes = []
        for dataSecIdx in range(0, 11):
            self.dataSecSizes.append(stream.read_long())

        self.bssAddress = stream.read_long()
        self.bssSize = stream.read_long()
        self.entryPoint = stream.read_long()
        self.valid = True

        # Fetch game build number
        buildStrOffset = stream.data.find(b'Build v') + 7
        buildStrEnd = stream.data.find(b' ', buildStrOffset)
        self.buildVersionStr = stream.data[buildStrOffset:buildStrEnd].decode()
        self.buildVersion = float(self.buildVersionStr)

    def is_patched(self):
        return self.textSecOffsets[2] != 0

    def patch_rel24(self, buffer, offset, basePatchAddress, targetAddress):
        instruction = struct.unpack(">I", buffer[offset:offset + 4])[0] & ~0x3FFFFFC
        relAddr = targetAddress - (basePatchAddress + offset)
        instruction |= (relAddr & 0x3FFFFFC)
        buffer[offset:offset + 4] = struct.pack(">I", instruction)

    def patch_hi_lo(self, buffer, offsetHi, offsetLo, targetAddress):
        instrHi = struct.unpack(">I", buffer[offsetHi:offsetHi + 4])[0] & 0xFFFF0000
        instrLo = struct.unpack(">I", buffer[offsetLo:offsetLo + 4])[0] & 0xFFFF0000
        addrHi = (targetAddress & 0xFFFF0000)
        addrLo = (targetAddress & 0x0000FFFF)

        if addrLo >= 0x8000:
            addrHi += 0x10000

        instrHi |= (addrHi >> 16)
        instrLo |= addrLo

        buffer[offsetHi:offsetHi + 4] = struct.pack(">I", instrHi)
        buffer[offsetLo:offsetLo + 4] = struct.pack(">I", instrLo)

    def apply_patch(self, patchFile, outFile):
        # Find the addresses of functions we need to call from the patch code
        addrDVDOpen = self.get_symbol("DVDOpen")
        addrDVDReadAsyncPrio = self.get_symbol("DVDReadAsyncPrio")
        addrDVDClose = self.get_symbol("DVDClose")
        addrOSLink = self.get_symbol("OSLink")
        addrPPCSetFpIEEEMode = self.get_symbol("PPCSetFpIEEEMode")
        addrNew = self.get_symbol("__nwa__FUlPCcPCc")
        addrHandleRel14 = self.get_symbol("__OSLinkHandleREL14")

        failed = []
        if addrDVDOpen == None: failed.append("DVDOpen")
        if addrDVDReadAsyncPrio == None: failed.append("DVDReadAsyncPrio")
        if addrDVDClose == None: failed.append("DVDClose")
        if addrOSLink == None: failed.append("OSLink")
        if addrPPCSetFpIEEEMode == None: failed.append("PPCSetFpIEEEMode")
        if addrNew == None: failed.append("__nwa__FUlPCcPCc")
        if addrHandleRel14 == None: failed.append("__OSLinkHandleREL14")

        if len(failed) > 0:
            print("Failed to apply DOL patch! The following symbols are missing: %s" % ", ".join(failed))
            return False

        # Read patch data into memory
        assert (self.valid)
        patchFile = open(patchFile, "rb")
        patchData = bytearray(patchFile.read())
        amtToPad = ((len(patchData) + 31) & ~31) - len(patchData)
        patchData.extend(bytearray(amtToPad))
        patchFile.close()

        # Pick address to write the patch code to
        address = 0x80002B00
        print("Adding patch to 0x%08X" % address)

        # Relocate addresses in the patch code to account for the address it's placed at in memory
        self.patch_hi_lo(patchData, 0x04, 0x08, address + 0xFF)
        self.patch_hi_lo(patchData, 0x20, 0x24, address + 0xF0)
        self.patch_hi_lo(patchData, 0x40, 0x44, address + 0xE9)
        self.patch_hi_lo(patchData, 0x64, 0x68, address)
        self.patch_hi_lo(patchData, 0x74, 0x78, address + 0xFF)
        self.patch_hi_lo(patchData, 0x90, 0x94, address + 0xE9)
        self.patch_rel24(patchData, 0x2C, address, addrDVDOpen)
        self.patch_rel24(patchData, 0x4C, address, addrNew)
        self.patch_rel24(patchData, 0x70, address, addrDVDReadAsyncPrio)
        self.patch_rel24(patchData, 0x88, address, addrDVDClose)
        self.patch_rel24(patchData, 0x9C, address, addrNew)
        self.patch_rel24(patchData, 0xAC, address, addrOSLink)
        self.patch_rel24(patchData, 0xBC, address, addrPPCSetFpIEEEMode)

        # Update DOL header for write
        self.textSecOffsets[2] = self.textSecOffsets[1] + self.textSecSizes[1]
        self.textSecSizes[2] = len(patchData)
        self.textSecAddresses[2] = address

        srcStream = InputStream(self.filename, BIG_ENDIAN)
        outStream = OutputStream(BIG_ENDIAN)

        # Write DOL header
        for textSecIdx in range(0, 7):
            offset = self.textSecOffsets[textSecIdx]

            if textSecIdx > 2 and offset != 0:
                offset += len(patchData)

            outStream.write_long(offset)

        for dataSecIdx in range(0, 11):
            offset = self.dataSecOffsets[dataSecIdx]

            if offset != 0:
                offset += len(patchData)

            outStream.write_long(offset)

        for textSecIdx in range(0, 7):
            outStream.write_long(self.textSecAddresses[textSecIdx])
        for dataSecIdx in range(0, 11):
            outStream.write_long(self.dataSecAddresses[dataSecIdx])
        for textSecIdx in range(0, 7):
            outStream.write_long(self.textSecSizes[textSecIdx])
        for dataSecIdx in range(0, 11):
            outStream.write_long(self.dataSecSizes[dataSecIdx])

        outStream.write_long(self.bssAddress)
        outStream.write_long(self.bssSize)
        outStream.write_long(self.entryPoint)

        for padIdx in range(0, 7):
            outStream.write_long(0)

        # Write section data
        for textSecIdx in range(0, 7):
            if textSecIdx == 2:
                outStream.write_bytes(patchData)
            else:
                srcStream.goto(self.textSecOffsets[textSecIdx])
                data = srcStream.read_bytes(self.textSecSizes[textSecIdx])

                if textSecIdx == 1:
                    array = bytearray(data)

                    # Patch code in the original game code to call the custom linker function
                    # todo - this offset shouldn't be hardcoded
                    self.patch_rel24(array, 0x17B4, self.textSecAddresses[textSecIdx], address + 0x10)
                    # Also patch rel14 to act like rel32
                    secAddr = self.textSecAddresses[textSecIdx]
                    offset = addrHandleRel14 - secAddr
                    array[offset:offset + 12] = struct.pack(
                        ">III",
                        0x60000000,  # NOP
                        0x60000000,  # NOP
                        0x901c0000  # STW r0, 0(r28)
                    )

                    data = bytes(array)

                outStream.write_bytes(data)

        for dataSecIdx in range(0, 11):
            srcStream.goto(self.dataSecOffsets[dataSecIdx])
            data = srcStream.read_bytes(self.dataSecSizes[dataSecIdx])
            outStream.write_bytes(data)

        # Save
        outStream.save_file(outFile)
        return True

    def load_symbols(self, symbolDir):
        # Load symbols
        symbolFileName = "%s/v%s.lst" % (symbolDir, self.buildVersionStr)
        if not os.path.isfile(symbolFileName):
            print("Failed to find a symbol list file for the provided build of the game (v%s)" % self.buildVersionStr)
            return False

        symbolFile = open("%s/v%s.lst" % (symbolDir, self.buildVersionStr))
        self.symbols = {}

        while True:
            line = symbolFile.readline()
            if not line: break

            split = line.strip().split(" ")
            self.symbols[split[1]] = int(split[0], 0)

        return True

    def generate_patches(self, origSymName, newSymName):
        print("Generating patches for %s to %s" % (origSymName, newSymName))
        # Read all instructions in all text sections and find branches to the given address.
        # Only unconditional branch instructions supported for now. More later.
        # Note: newSymbolName should be unmangled.
        PPC_OPCODE_bx = 18

        out = []

        # Find the address of the symbol.
        if origSymName in self.symbols:
            origSymAddress = self.symbols[origSymName]

        # Check for potential unmangled name, which can happen if extern "C" is used (line in SDK libraries)
        else:
            funcNameEnd = origSymName.find("__F")
            if funcNameEnd != -1:
                unmangledName = origSymName[0:funcNameEnd]

                if unmangledName in self.symbols:
                    origSymAddress = self.symbols[unmangledName]

                else:
                    print(
                        "*** Warning: Failed to resolve symbol '%s'. There will be no relocations generated for it. ***" % origSymName)
                    return out

        stream = InputStream(self.filename)
        origSymAddress = self.symbols[origSymName]

        for textSecIdx in range(0, 7):
            # Grab offset/address, check if valid
            secOffset = self.textSecOffsets[textSecIdx]
            if secOffset == 0: continue

            secAddress = self.textSecAddresses[textSecIdx]
            secSize = self.textSecSizes[textSecIdx]
            secEnd = secOffset + secSize
            stream.goto(secOffset)

            # Parse all instructions in this code block
            while stream.tell() < secEnd and not stream.eof():
                address = secAddress + (stream.tell() - secOffset)
                instruction = stream.read_long()
                opcode = (instruction >> 26) & 0x3F

                # bx
                if opcode == PPC_OPCODE_bx:
                    LI = extend_sign_bit(instruction & 0x3FFFFFC, 24)
                    AA = (instruction >> 1) & 0x1
                    target = LI
                    if AA == 0: target = target + address

                    if target == origSymAddress:
                        patch = {}
                        patch['address'] = address
                        patch['type'] = R_PPC_REL24
                        patch['symbol'] = newSymName
                        out.append(patch)

        for dataSecIdx in range(0, 11):
            # Grab offset/address, check if valid
            secOffset = self.dataSecOffsets[dataSecIdx]
            if secOffset == 0: continue

            secAddress = self.dataSecAddresses[dataSecIdx]
            secSize = self.dataSecSizes[dataSecIdx]
            secEnd = secOffset + secSize
            stream.goto(secOffset)

            # Look for 32-bit address references
            while stream.tell() < secEnd and not stream.eof():
                address = secAddress + (stream.tell() - secOffset)
                value = stream.read_long()

                if value == origSymAddress:
                    patch = {}
                    patch['address'] = address
                    patch['type'] = R_PPC_ADDR32
                    patch['symbol'] = newSymName
                    out.append(patch)

        return out

    def get_symbol(self, symName):
        if symName in self.symbols:
            return self.symbols[symName]

        else:
            return None

    def get_section_index(self, address):
        allSecAddresses = self.textSecAddresses + self.dataSecAddresses
        allSecSizes = self.textSecSizes + self.dataSecSizes

        for secIdx in range(0, 18):
            secBegin = allSecAddresses[secIdx]
            secEnd = secBegin + allSecSizes[secIdx]
            if address >= secBegin and address < secEnd:
                return secIdx

        # If we're at this point then the address is invalid
        assert (False)
        return -1

    def print_header_info(self):
        print("Text Section Offsets:    %s" % list_as_hex(self.textSecOffsets))
        print("Data Section Offsets:    %s" % list_as_hex(self.dataSecOffsets))
        print("Text Section Addresses:  %s" % list_as_hex(self.textSecAddresses))
        print("Data Section Addresses:  %s" % list_as_hex(self.dataSecAddresses))
        print("BSS Address:             0x%08X" % self.bssAddress)
        print("BSS Size:                0x%08X" % self.bssSize)
        print("Entry Point:             0x%08X" % self.entryPoint)

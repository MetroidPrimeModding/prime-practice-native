from Mangle import *
from DolFile import *
from PreplfFile import *
import glob
import os
import re
import subprocess
import sys
import cxxfilt
import zlib
# Environment

primeApiRoot = os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/..")
devkitPPCRoot = ""
gccPath = ""
ldPath = ""
linkerPath = ""

# Configuration
projDir = ""
buildDir = ""
moduleName = "Mod"
outFile = ""
dolFile = DolFile()
dolPatches = []
verbose = False
buildDebug = False


def parse_commandline():
    global projDir, buildDir, moduleName, outFile, verbose, buildDebug

    if len(sys.argv) < 3:
        print("Please specify a project directory and a dol to link to!")
        return False

    # Set project directory
    projDir = sys.argv[1]
    if projDir.endswith("/") or projDir.endswith("/"):
        projDir = projDir[:-1]

    buildDir = "%s/build" % projDir

    # Read DOL
    dolFile.read(sys.argv[2])

    if dolFile.buildVersion >= 3:
        print(
            "The specified dol file belongs to a Wii version of the game. The Wii versions are currently not supported.")
        return False

    if not dolFile.load_symbols(primeApiRoot + "/symbols"):
        return False

    # Check other arguments
    argIdx = 3

    while argIdx < len(sys.argv):
        arg = sys.argv[argIdx]

        if arg == "-debug":
            buildDebug = True

        elif arg == "-m":
            moduleName = sys.argv[argIdx + 1]
            argIdx += 1

        elif arg == "-o":
            outFile = sys.argv[argIdx + 1]
            argIdx += 1

        elif arg == "-v":
            verbose = True

        argIdx += 1

    # Set default values for some arguments
    if not outFile:
        outFile = "%s/%s.rel" % (buildDir, moduleName)

    return True


def get_extension(sourceFile):
    return os.path.splitext(sourceFile)[1]


def get_object_path(sourceFile):
    # Hash is appended to solve the "multiple files named the same thing" problem
    hash = hex(zlib.crc32(bytes(sourceFile, 'utf-8')))[2:]
    return "%s/%s-%s.o" % (buildDir, os.path.splitext(os.path.split(sourceFile)[1])[0], hash)


def generate_scoped_decl(declText, declType):
    # This allows us to forward-declare scoped classes and functions which are scoped inside of a namespace or a class. While
    # normally you wouldn't be allowed to declare a namespace with the same name as a class, in this case ApplyCodePatches is
    # compiled separately from the other modules and therefore doesn't know of any name conflicts. Using namespaces like this
    # causes the compiler to generate an identical symbol to the class member, which can then later be resolved by the linker.
    scopes = split_scopes(declText)
    baseName = scopes[-1]
    del scopes[-1]
    outText = ""

    for scope in scopes:
        outText += "namespace %s { " % scope

    outText += "%s %s;" % (declType, baseName)

    for scope in scopes:
        outText += " }"

    outText += '\n'
    return outText


def generate_patch_code():
    template = open("%s/script/ApplyCodePatches_Template.cpp" % primeApiRoot, "r")
    code = template.read()
    template.close()

    # Find memory locations that need patches
    sortedPatches = sorted(dolPatches, key=lambda patch: patch['address'])
    classDecls = ""
    funcDecls = ""
    funcCode = ""
    usedSymbols = []
    usedSymbols = []

    for patch in dolPatches:
        address = patch['address']
        symbol = patch['symbol']
        type = patch['type']

        # For function pointers, use static_cast to ensure the correct overload is used
        argsStart = symbol.find('(')

        if argsStart != -1:
            argsEnd = symbol.rfind(')')
            assert (argsEnd != -1)

            funcName = symbol[0:argsStart]
            symbolRef = "reinterpret_cast<void*>(&%s)" % (funcName)

        else:
            symbolRef = "&%s" % symbol

        # Generate code for this patch
        if type == R_PPC_REL24:
            patchCode = '\n\tRelocate_Rel24((void*) 0x%08X, %s);' % (address, symbolRef)

        elif type == R_PPC_ADDR32:
            patchCode = '\n\tRelocate_Addr32((void*) 0x%08X, %s);' % (address, symbolRef)

        else:
            continue

        if not symbol in usedSymbols:
            paramsStart = symbol.find('(')
            isFunction = paramsStart != -1

            if isFunction:
                # Add forward decl for classes referenced in the function parameters
                paramsEnd = symbol.rfind(')')
                params = split_params(symbol[paramsStart + 1:paramsEnd])

                for param in params:
                    paramStr = ''.join([chr for chr in param if not chr in "*&"])
                    paramWords = paramStr.split(' ')

                    for word in paramWords:
                        if word and word != 'const' and word != 'unsigned' and word != 'signed':
                            # this is probably our type name!
                            if not is_basic_type(word):
                                classDecls += generate_scoped_decl(word, "class")
                            break

                funcDecls += generate_scoped_decl(symbol, "void")
                usedSymbols.append(symbol)

        funcCode = funcCode + patchCode

    cpptext = code % (classDecls + funcDecls, funcCode)

    # Save file
    cppname = "%s/ApplyCodePatches.cpp" % buildDir
    out = open(cppname, "w")
    out.write(cpptext)
    out.close()
    return cppname


def parse_code_macros(sourcePath):
    # Parse file, look for PATCH_SYMBOL macros
    # This implementation leaves a bit to be desired - namely, it doesn't account for commented-out
    # code or #ifdef'd out code, and won't correctly mangle templates that have default parameters
    # print("Reading file %s" % sourcePath)
    sourceFile = open(sourcePath, 'r', encoding="ISO-8859-1")
    regex = re.compile("^PATCH_SYMBOL\((.*)\((.*)\)(.*),(.*)\((.*)\)(.*)\)")
    pos = 0

    while True:
        line = sourceFile.readline()
        if not line:
            break

        # Replace spaces except ones denoting const parameters
        line = line.strip()
        strippedLine = ""

        for chrIdx in range(0, len(line)):
            chr = line[chrIdx]

            if chr != ' ':
                strippedLine += chr

            else:
                if chrIdx >= 5 and line[chrIdx - 5: chrIdx] == "const":
                    # Verify this const precedes a type name by verifying the next non-whitespace character is not a comma
                    for chrIdx2 in range(chrIdx, len(line)):
                        if line[chrIdx2] != ' ':
                            if line[chrIdx2] != ',':
                                strippedLine += chr
                            break

        match = regex.search(strippedLine)

        if match is not None:
            origSymbol = "%s(%s)%s" % (match.group(1), match.group(2), match.group(3))
            patchSymbol = "%s(%s)%s" % (match.group(4), match.group(5), match.group(6))
            pos = match.endpos

            dolPatches.extend(dolFile.generate_patches(mangle(origSymbol), patchSymbol))


def compile_object(sourcePath, outPath):
    # Create output directory
    if not os.path.exists(outPath):
        os.makedirs(outPath)

    if ".lib." not in sourcePath:
        parse_code_macros(sourcePath)

    if os.path.exists(get_object_path(sourcePath)) and os.path.getmtime(sourcePath) < os.path.getmtime(
            get_object_path(sourcePath)):
        print("%s object newer than source - not re-compiling. Delete object to re-compile." % sourcePath)
        return True

    print("Compiling %s" % sourcePath)

    # Check extension
    ext = get_extension(sourcePath)

    if ext == ".cpp":
        lang = "-std=c++17"
    elif ext == ".c":
        lang = ""
    elif ext == '.S':
        lang = ""
    else:
        print("%s: unrecognized extension (%s)" % (sourcePath, ext))
        return False

    # Compile
    includeDirs = [
        "-nostdlib",
        "-I.",
        "-Isrc/",
        "-Isrc/include/",
        "-I%s/libogc/include/" % devkitProRoot,
        "-I%s/newlib/newlib/include/" % projDir,
        "-I%s/newlib/newlib/libc/include/" % projDir,
        "-I%s/newlib/newlib/libm/common/" % projDir,
        "-I%s/newlib/newlib/libm/stdio/" % projDir,
        # We want all our C++ to come from libcxx, which we compile
        "-I%s/libcxx/include" % projDir,
        "-I%s/include" % primeApiRoot,
    ]

    args = [
        # compilerPath,
        gccPath,
        lang,
        "-Os",
        " ".join(includeDirs),
    ]
    if "c++" in lang:
        args += [
            "-Wno-delete-incomplete",
            "-fno-rtti"
        ]
    args += [
        "-fno-function-sections",
        "-fno-data-sections",
        "-fno-exceptions",
        "-mno-sdata",
        "-DSTRING_ONLY",
        "-D_LIBCPP_HAS_NO_THREADS",
        "-D_LDBL_EQ_DBL",
        "-c",
        "%s" % sourcePath,
        "-o %s" % get_object_path(sourcePath)
    ]

    # Run compiler
    command = ' '.join(args)
    if verbose:
        print(">>> %s" % command)
    ret = subprocess.call(command, shell=True)
    return ret == 0


def link_objects(objList):
    print("Linking objects")

    args = [
        ldPath,
        # '-shared',
        ' '.join(objList),
        "-L%s/lib/gcc/powerpc-eabi/8.2.0" % devkitPPCRoot,
        "-L%s/lib/gcc/powerpc-eabi/8.2.0/../../../../powerpc-eabi/lib" % devkitPPCRoot,
        "-nodefaultlibs",
        "-nostdlib",
        '-lgcc',
        # '-lc',
        '-lsysbase',
        "-r",
        "-d",
        "-x",
        "-z nocopyreloc",
        "-call_shared",
        "--strip-discarded",
        "--gc-sections",
        "-e _prolog",
        "--unresolved-symbols=ignore-in-object-files",
        "-o %s/%s.preplf" % (buildDir, moduleName),
        "-T %s/eppc.ld" % (primeApiRoot),
        "-Map %s/%s.map" % (buildDir, moduleName)
    ]

    # Run linker
    command = ' '.join(args)
    if verbose:
        print(">>> %s" % command)
    ret = subprocess.call(command, shell=True)
    return ret == 0


def convert_preplf_to_rel(preplfPath, outRelPath):
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

    prologSymbol = preplf.symbol_by_name("_prolog")
    if prologSymbol is None:
        prologSymbol = preplf.symbol_by_name("_prolog__Fv")

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
            ".eh_frame"  # Not actually used?
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
                    rel.write_byte(11) # I patched rel14 to be rel32
                    # rel.write_byte(26)
                else:
                    rel.write_byte(relocType)

                # Internal relocs are easy - just copy data from the ELF reloc/symbol
                if not isDol:
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
                        print("Error: Failed to locate dol symbol: %s / %s (GCC: %s)" % (
                            remangled, demangled, symbolName))
                        rel.write_byte(0)
                        rel.write_long(0)
                        relocWriteSuccess = False
                        continue

                    rel.write_byte(0)
                    rel.write_long(dolSymbolAddr)

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


def compile_rel():
    sourceFiles = []
    sourceFiles.extend(glob.glob(("%s/src/**/*.cpp" % projDir), recursive=True))
    sourceFiles.extend(glob.glob(("%s/src/**/*.c" % projDir), recursive=True))
    # Pull in a few things we need from newlib - can't use the default -lm compile because it uses sbss/sdata
    sourceFiles.append("%s/newlib/newlib/libc/reent/impure.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/errno/errno.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/common/s_matherr.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/common/s_lib_ver.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/common/s_trunc.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/common/s_log2.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/common/s_cbrt.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/common/s_nan.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/common/sf_nan.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/common/s_isnan.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/math/s_ceil.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/math/e_log10.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/math/w_log10.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/math/e_sqrt.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libm/math/w_sqrt.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/time.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/gmtime.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/gmtime_r.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/lcltime.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/lcltime_r.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/gettzinfo.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/month_lengths.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/tzlock.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/tzset.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/tzset_r.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/tzvars.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/tzcalc_limits.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/mktime.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/time/difftime.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdlib/div.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdlib/dtoa.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdlib/mprec.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdlib/strtoul.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdlib/strtol.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdlib/strtod.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdlib/mbtowc_r.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdlib/wctomb_r.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdlib/gdtoa-gethex.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdlib/gdtoa-hexnan.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/machine/powerpc/setjmp.S" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/ctype/ctype_.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/string/strlcpy.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/string/strcat.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/string/strcasecmp.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/string/strncasecmp.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/locale/locale.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/locale/localeconv.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/ungetc.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/siscanf.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/snprintf.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/vsnprintf.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/stdio.c" % projDir)
    # sourceFiles.append("%s/newlib/newlib/libc/stdio/vfprintf.c" % projDir)
    # sourceFiles.append("%s/newlib/newlib/libc/machine/powerpc/vfprintf.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/nano-vfscanf.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/nano-vfscanf_float.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/nano-vfscanf_i.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/nano-vfprintf.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/nano-vfprintf_float.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/nano-vfprintf_i.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/sprintf.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/sscanf.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/sccl.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/findfp.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/fclose.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/fflush.c" % projDir)
    sourceFiles.append("%s/newlib/newlib/libc/stdio/fwalk.c" % projDir)

    sourceFiles = [file for file in sorted(sourceFiles) if file.find("build/ApplyCodePatches.cpp") == -1]
    sourceFiles = [file for file in sorted(sourceFiles) if file.find("script/ApplyCodePatches_Template.cpp") == -1]
    sourceFiles = [file for file in sorted(sourceFiles) if file.find("cmake-build-debug") == -1]

    # Compile/link source files
    objectFiles = []

    for sourceFile in sourceFiles:
        if not compile_object(sourceFile, buildDir):
            return False
        else:
            objectFiles.append(get_object_path(sourceFile))
            if verbose: print('')

    # Generate patching code
    generatedFile = generate_patch_code()
    genBuildSuccess = compile_object(generatedFile, buildDir)

    if not genBuildSuccess:
        return False
    else:
        objectFiles.append(get_object_path(generatedFile))
        if verbose: print('')

    # Link
    if not link_objects(objectFiles):
        return False

    # We now have a .preplf file in the build folder... final step is to convert it to .rel
    if not convert_preplf_to_rel("%s/%s.preplf" % (buildDir, moduleName), outFile):
        return False

    return True


def main():
    # Verify there is a valid installation of devkitPPC
    global devkitPPCRoot, devkitProRoot
    devkitProRoot = os.getenv("DEVKITPRO")
    devkitPPCRoot = os.getenv("DEVKITPPC")

    if devkitPPCRoot is None:
        print("Error: The DEVKITPPC environment variable is undefined. BuildModule.py requires DevKitPPC")

    if devkitProRoot is None:
        print("Error: The DEVKITPRO environment variable is undefined. BuildModule.py requires DevKitPro")

    if devkitPPCRoot is None or devkitProRoot is None:
        return

    # Set globals
    global linkerPath, gccPath, ldPath
    gccPath = devkitPPCRoot + '/bin/powerpc-eabi-gcc'
    ldPath = devkitPPCRoot + '/bin/powerpc-eabi-ld'

    # Parse commandline argments
    if not parse_commandline():
        return

    # Apply DOL patches
    shouldContinue = True

    if not dolFile.is_patched():
        print("Detected DOL file is unpatched. Applying patch...")
        dolFilename = dolFile.filename
        nameEnd = dolFilename.rfind('.')
        nameBase = dolFilename[0:nameEnd]
        patchedName = "%s_mod.dol" % nameBase
        patchFile = "%s/script/DolPatch.bin" % primeApiRoot

        shouldContinue = dolFile.apply_patch(patchFile, patchedName)

        if shouldContinue:
            print("Saved patched DOL to %s" % patchedName)

    # Compile
    compileSuccess = compile_rel() if shouldContinue else False

    print("***** COMPILE %s! *****" % ("SUCCEEDED" if compileSuccess else "FAILED"))


if __name__ == "__main__":
    main()

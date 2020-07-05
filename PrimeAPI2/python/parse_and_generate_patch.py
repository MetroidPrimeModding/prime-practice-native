from dol import *
from mangle import *
import os
import argparse

dolPatches = []
dolFile = DolFile()

DIR = os.path.dirname(os.path.realpath(__file__))


def main():
    parser = argparse.ArgumentParser(description='Patch generator')
    parser.add_argument(
        "-p", "--patch",
        action='append',
        nargs=2,
        help="Format: originalSymbol newSymbol (ex. -p 'CGraphics::EndScene()' 'RenderHook()')",
        required=True
    )
    parser.add_argument(
        "-o", "--outFile",
        help="Output file",
        required=True
    )
    parser.add_argument(
        "-i", "--inDol",
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

    parse_code_macros(args.patch)
    generate_patch_code(args.outFile)


def parse_code_macros(patches):
    # Parse file, look for PATCH_SYMBOL macros
    # This implementation leaves a bit to be desired - namely, it doesn't account for commented-out
    # code or #ifdef'd out code, and won't correctly mangle templates that have default parameters
    # print("Reading file %s" % sourcePath)
    regex = re.compile("^(.*)\((.*)\)(.*)$")
    pos = 0

    for patch in patches:
        # Replace spaces except ones denoting const parameters
        original = strip_spaces_except_const(patch[0])
        replacement = strip_spaces_except_const(patch[1])

        originalMatch = regex.search(original)
        replacementMatch = regex.search(replacement)

        if originalMatch is None or replacementMatch is None:
            raise IOError(f"Unable to parse patch: {original} -> {patch}")
        origSymbol = "%s(%s)%s" % (originalMatch.group(1), originalMatch.group(2), originalMatch.group(3))
        patchSymbol = "%s(%s)%s" % (replacementMatch.group(1), replacementMatch.group(2), replacementMatch.group(3))
        dolPatches.extend(dolFile.generate_patches(mangle(origSymbol), patchSymbol))


def strip_spaces_except_const(line):
    line = line.strip()
    # TODO: do this backslash nonsense in the cmake file instead of here, somehow
    line = line.replace('\\', '')
    strippedLine = ''
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
    return strippedLine


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


def generate_patch_code(outFile):
    template = open("%s/ApplyCodePatches_Template.cpp" % DIR, "r")
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
    out = open(outFile, "w")
    out.write(cpptext)
    out.close()


if __name__ == "__main__":
    main()

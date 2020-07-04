# CodeWarrior-style C++ name mangling
import re
import sys

MANGLED_TYPES = {
    'void': 'v',
    'bool': 'b',
    'char': 'c',
    'wchar_t': 'w',
    'short': 's',
    'int': 'i',
    'long': 'l',
    'size_t': 'Ul',
    'long long': 'x',
    'signed char': 'c',
    'unsigned char': 'Uc',
    'signed short': 's',
    'unsigned short': 'Us',
    'signed int': 'i',
    'unsigned int': 'Ui',
    'signed long': 'l',
    'unsigned long': 'Ul',
    'signed long long': 'x',
    'unsigned long long': 'Ux',
    'uint': 'Ui',
    'int8': 'c',
    'uint8': 'Uc',
    'int16': 's',
    'uint16': 'Us',
    'int32': 'l',
    'uint32': 'Ul',
    'int64': 'x',
    'uint64': 'Ux',
    'float': 'f',
    'double': 'd'
}


def is_basic_type(typeName):
    return typeName in MANGLED_TYPES


def mangle_operator(operName):
    # Note: There are some missing operators here (particularly bitwise operators).
    name = operName.replace(' ', '')

    # Arithmetic
    if name == "operator=":
        return "__as"
    elif name == "operator+":
        return "__pl"
    elif name == "operator+=":
        return "__apl"
    elif name == "operator-":
        return "__mi"
    elif name == "operator-=":
        return "__ami"
    elif name == "operator*":
        return "__ml"
    elif name == "operator*=":
        return "__amu"
    elif name == "operator/":
        return "__dv"
    elif name == "operator/=":
        return "__adv"

    # Logic
    elif name == "operator<":
        return "__lt"
    elif name == "operator<=":
        return "__le"
    elif name == "operator>":
        return "__gt"
    elif name == "operator>=":
        return "__ge"
    elif name == "operator==":
        return "__eq"
    elif name == "operator!=":
        return "__ne"

    # Allocation
    elif name == "operatornew":
        return "__nw"
    elif name == "operatornew[]":
        return "__nwa"
    elif name == "operatordelete":
        return "__dl"
    elif name == "operatordelete[]":
        return "__dla"

    return operName


def split_scopes(scopedName):
    isOperatorFunc = False
    innerDepth = 0
    lastSplit = -2
    out = []

    for chrIdx in range(0, len(scopedName)):
        if scopedName[chrIdx:chrIdx + 8] == "operator":
            isOperatorFunc = True

        elif (scopedName[chrIdx] == '<' or scopedName[chrIdx] == '(') and not isOperatorFunc:
            innerDepth += 1

        elif (scopedName[chrIdx] == '>' or scopedName[chrIdx] == ')') and not isOperatorFunc:
            innerDepth -= 1

        elif scopedName[chrIdx:chrIdx + 2] == '::' and innerDepth == 0:
            out.append(scopedName[lastSplit + 2: chrIdx])
            lastSplit = chrIdx

    out.append(scopedName[lastSplit + 2: len(scopedName)])
    return out


def split_params(paramText):
    out = []
    templateDepth = 0
    lastSplit = -1

    for chrIdx in range(0, len(paramText)):
        chr = paramText[chrIdx]

        if chr == '<':
            templateDepth += 1
        elif chr == '>':
            templateDepth -= 1
        elif chr == ',' and templateDepth == 0:
            out.append(paramText[lastSplit + 1: chrIdx].strip())
            lastSplit = chrIdx

    out.append(paramText[lastSplit + 1: len(paramText)].strip())
    return out


def mangle_template(templateText):
    assert (templateText.startswith('<') and templateText.endswith('>'))
    templateText = templateText[1:-1]
    params = split_params(templateText)
    mangledParams = []

    for param in params:
        mangledParams.append(mangle_type(param))

    return '<' + ','.join(mangledParams) + '>'


def mangle_type(typename):
    if is_basic_type(typename):
        baseTypeName = MANGLED_TYPES[typename]

    else:
        baseTypeName = ""
        scopes = split_scopes(typename)

        if len(scopes) > 1:
            baseTypeName = "Q%d" % len(scopes)

        for scope in scopes:
            # Check for template
            templateBegin = scope.find('<')

            if templateBegin != -1:
                templateEnd = scope.rfind('>')
                assert (templateEnd != -1)
                baseType = scope[0:templateBegin]
                mangledTemplate = mangle_template(scope[templateBegin: templateEnd + 1])
                baseTypeName += '%d%s%s' % (len(baseType) + len(mangledTemplate), baseType, mangledTemplate)

            else:
                baseTypeName += '%d%s' % (len(scope), scope)

    return baseTypeName


def mangle_params(params):
    if not params:
        return 'Fv'

    # Get list of arguments. Can't use split() due to potential templates with multiple parameters
    args = split_params(params)
    out = ['F']

    for arg in args:
        type = arg
        qualifiers = ''
        baseTypeName = ''

        while len(type) > 0:
            if type.endswith('&'):
                qualifiers += 'R'
                type = type[:-1].strip()

            elif type.endswith('*'):
                qualifiers += 'P'
                type = type[:-1].strip()

            elif type.endswith('const'):
                qualifiers += 'C'
                type = type[:-5].strip()

            else:
                if type.endswith('>'):
                    searchLen = type.find('<')
                else:
                    searchLen = len(type)

                baseBegin = max(type.rfind(chr, 0, searchLen) for chr in " *&") + 1

                if type[baseBegin - 9: baseBegin - 1] == "unsigned":
                    baseBegin -= 9

                elif type[baseBegin - 7: baseBegin - 1] == "signed":
                    baseBegin -= 7

                baseType = type[baseBegin:len(type)]
                baseTypeName = mangle_type(baseType)
                type = type[:-len(baseType)].strip()

        out.append(qualifiers + baseTypeName)

    return ''.join(out)

def handle_demangle_edge_cases(demangled):
    if demangled.startswith('operator new'):
        return demangled.replace('unsigned int', 'unsigned long')
    if demangled.startswith('operator new'):
        return demangled.replace('unsigned int', 'unsigned long')
    ctSearch = re.match("(\\S+)::\\1\(", demangled)
    if ctSearch:
        demangled = demangled.replace("::" + ctSearch.group(1), '::__ct', 1)
    return demangled

def mangle(sig):
    funcSignature = handle_demangle_edge_cases(sig)
    if "__ct" in funcSignature:
        print(sig, funcSignature)
    mangled = ''

    funcName = funcSignature[0:funcSignature.find('(')]
    scopes = split_scopes(funcName)
    isScoped = len(scopes) > 1
    isConst = funcSignature.find('const', funcSignature.rfind(')')) != -1
    baseName = scopes[-1]

    # Check for operators/templates
    if baseName.startswith("operator"):
        baseName = mangle_operator(baseName)

    elif '<' in baseName:
        templateBegin = baseName.find('<')
        templateEnd = baseName.rfind('>')
        assert (templateBegin != -1 and templateEnd != -1)

        baseFuncName = baseName[0:templateBegin]
        mangledTemplate = mangle_template(baseName[templateBegin: templateEnd + 1])
        baseName = baseFuncName + mangledTemplate

    mangled = baseName + '__'

    if isScoped:
        del scopes[-1]

        if len(scopes) > 1:
            mangled += "Q%d" % len(scopes)

        for name in scopes:
            mangled += "%d%s" % (len(name), name)

    if isConst:
        mangled += 'C'

    params = funcSignature[funcSignature.find('(') + 1: funcSignature.rfind(')')]
    return mangled + mangle_params(params)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise RuntimeError("needs symbol argument")
    print(mangle(sys.argv[1]))

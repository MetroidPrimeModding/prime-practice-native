import sys

from demangle import demangle
from subprocess import Popen, PIPE
import re

lst_lines = []

with open("../symbols/v1.088.lst") as f:
    lst_lines += f.readlines()


out = []

def cppmangle(symb):
    symb = symb.replace("@4@", "")
    ct_or_dt = False
    if "__ct" in symb or "__dt" in symb:
        ct_or_dt = true

    split = symb.split("::")
    clazz = split[0]
    symb = symb.replace("__ct", clazz)
    symb = symb.replace("__dt", "~" + clazz)

    split = symb.split("::")
    method = split[1]
    if "<" in symb:
        raise IOError("Templates are evil")
    if symb.count("::") > 1:
        raise IOError("Namespaces are evil")
    if "rstl" == clazz:
        raise IOError("Namespaces are evil")

    builtins = ['int', 'char', 'short', 'float', 'double', 'bool', 'void', 'const', 'unsigned', 'long', 'wchar_t']
    if clazz in builtins:
        raise IOError("Don't need to do this class")

    tomangle = ""

    predecls = []
    for match in re.finditer("([a-zA-Z0-9_]+)", method):
        c = match.group(1).strip()
        if c in builtins:
            continue
        if c not in predecls and c != clazz:
            predecls.append(c)

    for predecl in predecls:
        tomangle += "class {} {{}};\n".format(predecl)

    if ct_or_dt:
        tomangle += """
        class {0} {{
            {1};
        }};
        {2} {{}};
        """.format(clazz, method, symb)
    else:
        tomangle += """
        class {0} {{
            void {1};
        }};
        void {2} {{}};
        """.format(clazz, method, symb)

    # yeah yeah this is a dumb hack so what
    tomangle = tomangle.replace('\n', '').replace('  ', ' ').replace('  ', ' ').replace('  ', ' ').replace('  ', ' ').replace('  ', ' ')

    process = Popen("""g++ -x c++ -S - -o- | grep -Eo "^(_.*:)" | sed -e 's/:$//'""",
    # process = Popen("""g++ -x c++ -S - -o-""",
                    stdout=PIPE,
                    stdin=PIPE,
                    shell=True)
    (output, err) = process.communicate(input=bytes(tomangle, encoding='UTF-8'))
    exit_code = process.wait()
    if exit_code != 0:
        print(err, file=sys.stderr)
        raise IOError("Failed to mangle")
    return str(output, encoding='utf-8').strip()

for line in lst_lines:
    line = line.strip()
    if len(line) == 0:
        continue
    split = line.split(' ')
    addr = split[0].strip()

    if "__" in line:
        try:
            symb = demangle(split[1].strip()).strip()
            symb = cppmangle(symb)
            out.append(".global " + symb + " ## C++ -- " + line)
            out.append(".set " + symb + ", " + split[0].strip())
            # line = "void %s = %s;" % (symb, addr)
            # outCPP.append(line)
        except Exception as e:
            pass
            # print(e)
            # print("Failed to demangle %s" % split[1])
    else:
        symb = split[1].strip()
        out.append(".global " + symb + " ## C -- " + line)
        out.append(".set " + symb + ", " + split[0].strip())

    # for o in out[-4:]:
    #     print(o)

outLines = out

with open("../symbols/v1.088.s", 'wb') as f:
    for line in outLines:
        f.write(bytes(line + "\n", encoding='utf-8'))
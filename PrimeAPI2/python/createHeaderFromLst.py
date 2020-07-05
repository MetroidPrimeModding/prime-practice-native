from demangle import demangle

lst_lines = []

with open("../../PrimeAPI/symbols/v1.088.lst") as f:
    lst_lines += f.readlines()


outCPP = []
outC = []

for line in lst_lines:
    if len(line) == 0:
        continue
    split = line.split(' ')
    addr = split[0].strip()
    symb = ""
    try:
        symb = demangle(split[1].strip()).strip()
        line = "void %s = %s;" % (symb, addr)
        outCPP.append(line)
    except:
        print("Failed to demangle %s" % split[1])
        symb = "%s()" % split[1].strip()
        line = "void %s = %s;" % (symb, addr)
        outC.append(line)

outLines = [
    "#ifdef __cplusplus",
] + outCPP + [
    "extern \"C\" {",
    "#endif"
] + outC + [
    "#ifdef __cplusplus",
    "}",
    "$endif"
]

for line in outLines:
    print(line)
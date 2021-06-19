# Run this script from IDA
import idautils
import os

namesToDump = []

for name in idautils.Names():
    if not SegName(name[0]).startswith('.data') and not name[1].startswith('def_') and not name[1].startswith('nullsub'):
        namesToDump.append(name)

out = open("out.lst", "w")

for name in namesToDump:
    out.write("0x%08X %s\n" % name)

print("Saved symbol list to %s" % os.path.abspath(out.name))
out.close()
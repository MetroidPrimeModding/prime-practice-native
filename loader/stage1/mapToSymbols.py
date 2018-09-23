with open("GALE01.map") as f:
    lines = f.readlines()

out = ""

lines = lines[1:]
for line in lines:
    addr, len, _, _, symbol = [x.strip() for x in line.split(" ")]
    out += ".global " + symbol + "\n"
    out += ".set " + symbol + ", 0x" + addr + '\n'

with open("GALE01.symbols.s", "w") as f:
    f.write(out)
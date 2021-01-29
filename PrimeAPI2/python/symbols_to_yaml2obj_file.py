import sys
from demangle import demangle
import subprocess

yaml_template = '''\
--- !ELF
FileHeader:
  Class:           ELFCLASS32
  Data:            ELFDATA2MSB
  Type:            ET_REL
  Machine:         EM_PPC
Symbols:
{}
...
'''

symbol_template = '''\
  - Name:            {!r}
    Index:           SHN_ABS
    Binding:         STB_GLOBAL
    Value:           {}
'''
# Value:           {:#x}

symbols = []
with open('symbols/prac_mod_symbols.lst') as f:
    for line in f:
        line = line.strip()
        if len(line) == 0:
            continue

        addr, name = line.split(' ', maxsplit=1)
        # demangled = name
        # mangled = name
        # try:
        #     demangled = demangle(name)
        #     mangled =
        # except:
        #     print('Unable to demangle ', name)
        symbols.append((addr.strip(), name.strip()))

symbols_text = "".join([symbol_template.format(name, addr)
                        for addr, name in symbols])

compiledYaml = yaml_template.format(symbols_text)
with open('symbols/prac_mod_symbols.yaml', mode='w') as f:
    f.write(compiledYaml)

args = [
    # probably shouldn't hardcode this
    '/opt/homebrew/Cellar/llvm/11.0.0/bin/yaml2obj'
]
try:
    res = subprocess.run(args, check=True, capture_output=True, input=compiledYaml.encode('utf-8'))
except subprocess.CalledProcessError as e:
    print(e.stdout.decode("utf-8"))
    print(e.stderr.decode("utf-8"))
    # for line in e.stdout:
    #     print(line)
    # for line in e.stderr:
    #     print(line)


with open('symbols/prac_mod_symbols.o', mode='wb') as f:
    f.write(res.stdout)

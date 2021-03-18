import argparse
import os
import subprocess
import sys
from pathlib import Path
from typing import Optional

from demangle import demangle


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


def convert_symbol_list(list_file: Path, output_file: Path, llvm_dir: Path, yaml_file: Optional[Path]):
    symbols = []
    with list_file.open() as f:
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

    if yaml_file is not None:
        with yaml_file.open(mode='w') as f:
            f.write(compiledYaml)

    args = [
        os.fspath(llvm_dir / 'bin/yaml2obj'),
    ]
    try:
        res = subprocess.run(args, check=True, capture_output=True, input=compiledYaml.encode('utf-8'))
    except subprocess.CalledProcessError as e:
        print(e.stdout.decode("utf-8"))
        print(e.stderr.decode("utf-8"))

    with output_file.open(mode='wb') as f:
        f.write(res.stdout)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--llvm-dir", type=Path, help="Path to the LLVM to use", required=True)
    parser.add_argument("--yaml-file", type=Path, help="Path to where to place the converted yaml file")
    parser.add_argument("list_file", type=Path, help="Path to the list of symbols to convert")
    parser.add_argument("object_file", type=Path, help="Path to where to place the object file")
    args = parser.parse_args()

    convert_symbol_list(
        args.list_file,
        args.object_file,
        args.llvm_dir,
        args.yaml_file,
    )


if __name__ == "__main__":
    main()
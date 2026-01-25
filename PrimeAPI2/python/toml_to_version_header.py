import argparse

def parse_arguments():
    parser = argparse.ArgumentParser(description="Convert TOML version info to C++ header file.")
    parser.add_argument("--input", required=True, help="Path to the input TOML file.")
    parser.add_argument("--output", required=True, help="Path to the output C++ header file.")
    return parser.parse_args()

def read_version_from_toml(input_file):
    with open(input_file, 'r') as f:
        for line in f:
            if line.startswith("version"):
                key, value = line.split('=', 1)
                return value.strip().strip('"')
    return None

def write_header(output_file, version):
    with open(output_file, 'w') as f:
        f.write("#pragma once\n\n")
        f.write(f'#define MOD_VERSION "{version}"\n')

def main():
    args = parse_arguments()
    version = read_version_from_toml(args.input)
    write_header(args.output, version)

if __name__ == "__main__":
    main()

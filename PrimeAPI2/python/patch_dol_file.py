from dol import *
from mangle import *
import os
import argparse

dolFile = DolFile()

DIR = os.path.dirname(os.path.realpath(__file__))

def main():
    parser = argparse.ArgumentParser(description='Patch generator')
    parser.add_argument(
        "-o", "--outDol",
        help="Output dol file",
        required=True
    )
    parser.add_argument(
        "-i", "--inDol",
        help="Input dol file",
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

    print("Detected DOL file is unpatched. Applying patch...")
    patchFile = "%s/DolPatch.bin" % DIR

    if not dolFile.apply_patch(patchFile, args.outDol):
        print("Failed to patch dol")
        sys.exit(1)
    else:
        print("Saved patched DOL to %s" % args.outDol)


if __name__ == "__main__":
    main()

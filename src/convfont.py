import io
import math

def main():
    convImage("VGA-ROM.F08", "VGA-ROM.l4")

def convImage(path, outpath):
    fullSrc = open(path, "rb").read()
    destFile = open(outpath, "wb")
    destFile.truncate(0)

    for i in range(0, 256):
        src = fullSrc[i * 8:(i + 1) * 8]
        dest = bytearray(8 * 4)
        for y in range(0, 8):
            for x in range(0, 8):
                bit = (src[y] >> (x)) & 1
                lum = 0xF if bit == 1 else 0
                if x % 2 == 0:
                    lum <<= 4
                dest[y * 4 + int(math.floor(x / 2))] |= lum
        destFile.write(dest)

    destFile.close()

if __name__ == "__main__":
    main()
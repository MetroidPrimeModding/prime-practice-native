from mmap import mmap

class DataReader:
    def __init__(self, data: mmap, offset=0):
        self.data = data
        self.offset = offset

    def with_offset(self, offset: int):
        return DataReader(self.data, self.offset + offset)

    def read_bytes(self, offset: int, count: int):
        return self.data[(self.offset + offset):(self.offset + offset + count)]

    def read_u8(self, offset: int):
        return int.from_bytes(self.read_bytes(offset, 1), "big", signed=False)

    def read_u16(self, offset: int):
        return int.from_bytes(self.read_bytes(offset, 2), "big", signed=False)

    def read_u32(self, offset: int):
        return int.from_bytes(self.read_bytes(offset, 4), "big", signed=False)

    def read_u64(self, offset: int):
        return int.from_bytes(self.read_bytes(offset, 8), "big", signed=False)

    def read_s8(self, offset: int):
        return int.from_bytes(self.read_bytes(offset, 1), "big", signed=True)

    def read_s16(self, offset: int):
        return int.from_bytes(self.read_bytes(offset, 2), "big", signed=True)

    def read_s32(self, offset: int):
        return int.from_bytes(self.read_bytes(offset, 4), "big", signed=True)

    def read_s64(self, offset: int):
        return int.from_bytes(self.read_bytes(offset, 8), "big", signed=True)

    def read_string(self, offset: int, max_len=256):
        res = ''
        i = 0
        while True:
            c = self.read_u8(offset + i)
            i += 1
            if c == 0 or i >= max_len:
                break
            res += chr(c)
        return res

from mmap import mmap


class DataWriter:
    def __init__(self, data: mmap, offset=0):
        self.data = data
        self.offset = offset

    def with_offset(self, offset: int):
        return DataWriter(self.data, self.offset + offset)

    def write_bytes(self, offset: int, b: bytes):
        self.data[(self.offset + offset):(self.offset + offset + len(b))] = b

    def write_u8(self, offset: int, value: int):
        self.write_bytes(offset, value.to_bytes(1, "big", signed=False))

    def write_u16(self, offset: int, value: int):
        self.write_bytes(offset, value.to_bytes(2, "big", signed=False))

    def write_u32(self, offset: int, value: int):
        self.write_bytes(offset, value.to_bytes(4, "big", signed=False))

    def write_u64(self, offset: int, value: int):
        self.write_bytes(offset, value.to_bytes(8, "big", signed=False))

    def write_s8(self, offset: int, value: int):
        self.write_bytes(offset, value.to_bytes(1, "big", signed=True))

    def write_s16(self, offset: int, value: int):
        self.write_bytes(offset, value.to_bytes(2, "big", signed=True))

    def write_s32(self, offset: int, value: int):
        self.write_bytes(offset, value.to_bytes(4, "big", signed=True))

    def write_s64(self, offset: int, value: int):
        self.write_bytes(offset, value.to_bytes(8, "big", signed=True))

    def write_string(self, offset: int, s: str):
        for i in range(0, len(s)):
            c = s[i]
            self.write_u8(offset + i, ord(c))
        self.write_u8(offset + len(s), 0)

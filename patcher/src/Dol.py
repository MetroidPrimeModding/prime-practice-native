from dataclasses import dataclass
from src.DataReader import DataReader
from src.DataWriter import DataWriter
from typing import List

@dataclass
class SectionInfo:
    offset: int
    loading: int
    size: int

@dataclass
class DolHeader:
    text: List[SectionInfo]
    data: List[SectionInfo]
    bss_addr: int
    bss_size: int
    entry: int

    @staticmethod
    def parse(src: DataReader):
        text_offset = [src.read_u32(i * 4) for i in range(0, 7)]
        data_offset = [src.read_u32(0x1c + i * 4) for i in range(0, 11)]
        text_loading = [src.read_u32(0x48 + i * 4) for i in range(0, 7)]
        data_loading = [src.read_u32(0x64 + i * 4) for i in range(0, 11)]
        text_size = [src.read_u32(0x90 + i * 4) for i in range(0, 7)]
        data_size = [src.read_u32(0xAC + i * 4) for i in range(0, 11)]
        bss_addr = src.read_u32(0xD8)
        bss_size = src.read_u32(0xDC)
        entry = src.read_u32(0xE0)

        text = [SectionInfo(offset=text_offset[i], loading=text_loading[i], size=text_size[i]) for i in range(0, 7)]
        data = [SectionInfo(offset=data_offset[i], loading=data_loading[i], size=data_size[i]) for i in range(0, 11)]

        return DolHeader(
            text=text,
            data=data,
            bss_addr=bss_addr,
            bss_size=bss_size,
            entry=entry
        )

    def write(self, dest: DataWriter):
        for i in range(0, 7):
            dest.write_u32(i * 4, self.text[i].offset)
        for i in range(0, 11):
            dest.write_u32(0x1c + i * 4, self.data[i].offset)
        for i in range(0, 7):
            dest.write_u32(0x48 + i * 4, self.text[i].loading)
        for i in range(0, 11):
            dest.write_u32(0x64 + i * 4, self.data[i].loading)
        for i in range(0, 7):
            dest.write_u32(0x90 + i * 4, self.text[i].size)
        for i in range(0, 11):
            dest.write_u32(0xAC + i * 4, self.data[i].size)
        dest.write_u32(0xD8, self.bss_addr)
        dest.write_u32(0xDC, self.bss_size)
        dest.write_u32(0xE0, self.entry)

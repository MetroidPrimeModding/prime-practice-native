from dataclasses import dataclass

from DataReader import DataReader
from DataWriter import DataWriter


@dataclass
class GCDiscHeader:
    code: int
    maker_code: int
    disk_id: int
    version: int
    audio_streaming: int
    stream_buffer_size: int
    unused_1: bytes
    magic_word: int
    game_name: bytes
    debug_monitor: int
    debug_monitor_load: int
    unused_2: bytes
    dol_offset: int
    fst_offset: int
    fst_size: int
    fst_max_size: int
    user_pos: int
    user_len: int
    unused_3: int
    unused_4: int

    @staticmethod
    def parse(src: DataReader):
        return GCDiscHeader(
            code=src.read_u32(0),
            maker_code=src.read_u16(0x4),
            disk_id=src.read_u8(0x6),
            version=src.read_u8(0x07),
            audio_streaming=src.read_u8(0x08),
            stream_buffer_size=src.read_u8(0x09),
            unused_1=src.read_bytes(0xA, 0x12),
            magic_word=src.read_u32(0x1c),
            game_name=src.read_bytes(0x20, 0x3E0),
            debug_monitor=src.read_u32(0x400),
            debug_monitor_load=src.read_u32(0x404),
            unused_2=src.read_bytes(0x408, 0x18),
            dol_offset=src.read_u32(0x420),
            fst_offset=src.read_u32(0x424),
            fst_size=src.read_u32(0x428),
            fst_max_size=src.read_u32(0x42C),
            user_pos=src.read_u32(0x430),
            user_len=src.read_u32(0x434),
            unused_3=src.read_u32(0x438),
            unused_4=src.read_u32(0x43C),
        )

    def write(self, dest: DataWriter):
        dest.write_u32(0, self.code)
        dest.write_u16(0x4, self.maker_code)
        dest.write_u8(0x6, self.disk_id)
        dest.write_u8(0x07, self.version)
        dest.write_u8(0x08, self.audio_streaming)
        dest.write_u8(0x09, self.stream_buffer_size)
        dest.write_bytes(0xA, self.unused_1)
        dest.write_u32(0x1c, self.magic_word)
        dest.write_bytes(0x20, self.game_name)
        dest.write_u32(0x400, self.debug_monitor)
        dest.write_u32(0x404, self.debug_monitor_load)
        dest.write_bytes(0x408, self.unused_2)
        dest.write_u32(0x420, self.dol_offset)
        dest.write_u32(0x424, self.fst_offset)
        dest.write_u32(0x428, self.fst_size)
        dest.write_u32(0x42C, self.fst_max_size)
        dest.write_u32(0x430, self.user_pos)
        dest.write_u32(0x434, self.user_len)
        dest.write_u32(0x438, self.unused_3)
        dest.write_u32(0x43C, self.unused_4)


@dataclass
class FSTEntry:
    directory: bool
    name: str
    children: list = None
    offset: int = None
    length: int = None

    def print(self, depth=0):
        print("\t" * depth + f"{self.name} {self.offset} {self.length}")
        if self.children:
            for child in self.children:
                child.print(depth + 1)

    def get_ranges(self):
        ranges = []
        if self.directory:
            for child in self.children:
                ranges += child.get_ranges()
        else:
            ranges.append((self.offset, self.offset + self.length))
        ranges.sort(key=lambda a: a[0])
        return ranges

    def find(self, path: [str]):
        head = path[0]
        tail = path[1:]
        if head != self.name:
            return None
        if self.directory:
            for child in self.children:
                found = child.find(tail)
                if found:
                    return found
        else:
            return self

    def find_offset(self, offset: int):
        if self.directory:
            for child in self.children:
                found = child.find_offset(offset)
                if found:
                    return found
        else:
            if self.offset == offset:
                return self
            else:
                return None

    def count(self):
        if self.directory:
            count = 1
            for child in self.children:
                count += child.count()
            return count
        else:
            return 1


class FSTRecursiveWriter:
    def __init__(self, fst):
        self.fst = fst
        self.file_offset = 0
        self.string_offset = 0
        self.str_table: DataWriter

    def write(self, dest: DataWriter):
        self.count = self.fst.count()
        self.str_table = dest.with_offset(self.count * 0xC)
        self.write_recursively(self.fst.root, dest, -1)
        self.len = self.count * 0xC + self.string_offset

    def write_recursively(self, entry: FSTEntry, dest: DataWriter, parent_index: int):
        # Write name
        name_offset = self.string_offset
        self.str_table.write_string(name_offset, entry.name)
        self.string_offset += len(entry.name) + 1

        my_offset = self.file_offset
        self.file_offset += 1

        my_byte_offset = my_offset * 0xC

        if entry.directory:
            if parent_index < 0:
                # root
                dest.write_u32(my_byte_offset + 0x0, (0x01 << 24) | name_offset)
                dest.write_u32(my_byte_offset + 0x4, 0)
                dest.write_u32(my_byte_offset + 0x8, self.count)
            else:
                dest.write_u32(my_byte_offset + 0x0, (0x01 << 24) | name_offset)
                dest.write_u32(my_byte_offset + 0x4, parent_index)
                dest.write_u32(my_byte_offset + 0x8, my_offset + len(entry.children) + 1)
            for child in [x for x in entry.children if not x.directory]:
                self.write_recursively(child, dest, my_offset)
            for child in [x for x in entry.children if x.directory]:
                self.write_recursively(child, dest, my_offset)
        else:
            dest.write_u32(my_byte_offset + 0x0, name_offset)
            dest.write_u32(my_byte_offset + 0x4, entry.offset)
            dest.write_u32(my_byte_offset + 0x8, entry.length)


@dataclass
class FST:
    root: FSTEntry

    @staticmethod
    def parse(src: DataReader):
        root_data = FSTEntryData.parse(src)
        entry_datas = []
        for i in range(1, root_data.length):
            entry_datas.append(FSTEntryData.parse(src.with_offset(i * 0xC)))
        string_table = src.with_offset(root_data.length * 0xC)

        root_entry = FSTEntry(
            directory=True,
            name=string_table.read_string(root_data.filename),
            children=[]
        )
        entries = [root_entry]
        current_directory = root_entry
        for entry_data in entry_datas:
            if entry_data.directory:
                entry = FSTEntry(
                    directory=True,
                    name=string_table.read_string(entry_data.filename),
                    children=[]
                )
                entries.append(entry)
                parent = entries[entry_data.offset]
                parent.children.append(entry)
                current_directory = entry
            else:
                entry = FSTEntry(
                    directory=False,
                    name=string_table.read_string(entry_data.filename),
                    offset=entry_data.offset,
                    length=entry_data.length
                )
                entries.append(entry)
                current_directory.children.append(entry)
        return FST(root=root_entry)

    def write(self, dest: DataWriter):
        writer = FSTRecursiveWriter(self)
        writer.write(dest)
        return writer.len

    def print(self):
        self.root.print()

    def get_ranges(self):
        return self.root.get_ranges()

    def find(self, path: [str]):
        return self.root.find([self.root.name] + path)

    def find_offset(self, offset: int):
        return self.root.find_offset(offset)

    def count(self):
        return self.root.count()


@dataclass
class FSTEntryData:
    directory: int
    filename: int
    offset: int
    length: int

    @staticmethod
    def parse(src: DataReader):
        return FSTEntryData(
            directory=src.read_u8(0x0) != 0,
            filename=src.read_u32(0x0) & 0x00FF_FFFF,
            offset=src.read_u32(0x4),
            length=src.read_u32(0x8),
        )

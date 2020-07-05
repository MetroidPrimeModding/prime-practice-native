from stream import *

# Elf Section Type definitions
EST_NULL = 0
EST_PROGBITS = 1
EST_SYMTAB = 2
EST_STRTAB = 3
EST_RELA = 4

class ElfSection:
	def __init__(self, stream, parent):
		self.parent = parent
		self.entryOffset = stream.tell()
		
		# Read section header
		self.name = parent.fetch_name(stream, stream.read_long())
		self.type = stream.read_long()
		self.flags = stream.read_long()
		self.address = stream.read_long()
		self.offset = stream.read_long()
		self.size = stream.read_long()
		self.link = stream.read_long()
		self.targetSecIdx = stream.read_long()
		self.alignment = stream.read_long()
		self.entrySize = stream.read_long()
	
	def print_info(self):
		print(self.name)
		print("Type:    	0x%X" % self.type)
		print("Flags:   	0x%X" % self.flags)
		print("Address: 	0x%08X" % self.address)
		print("Offset:  	0x%X" % self.offset)
		print("Size:    	0x%X" % self.size)
		print("Link:    	%s" % self.parent.get_section_name(self.link))
		print("Target Sec:	%s" % self.parent.get_section_name(self.targetSecIdx))
		print("Alignment:	0x%X" % self.alignment)
		print("Entry Size:	0x%X" % self.entrySize)
		print('')
		
	def read_section(self, stream):
		stream.goto(self.offset)
		self.data = stream.read_bytes(self.size)

class ElfSymbolSection(ElfSection):
	def read_section(self, stream):
		ElfSection.read_section(self, stream)
		assert(self.entrySize == 0x10)
		stream.goto(self.offset)
		
		numSymbols = int(self.size / self.entrySize)
		self.symbols = []
		self.symbolDict = {}
		
		for symIdx in range(0, numSymbols):
			symbol = {}
			
			nameOffset = stream.read_long()
			symbol['name'] = self.parent.fetch_symbol_name(stream, nameOffset, self.link)
			symbol['value'] = stream.read_long()
			symbol['size'] = stream.read_long()
			info = stream.read_byte()
			symbol['bind'] = (info >> 4) & 0xF
			symbol['type'] = info & 0xF
			symbol['visibility'] = stream.read_byte()
			symbol['sectionIndex'] = stream.read_short()
			
			self.symbols.append(symbol)
			self.symbolDict[symbol['name']] = symbol
	
	def symbol_by_name(self, symName):
		if symName in self.symbolDict:
			return self.symbolDict[symName]
		else:
			return None
			
class ElfRelocSection(ElfSection):
	def read_section(self, stream):
		ElfSection.read_section(self, stream)
		assert(self.entrySize == 0xC)
		stream.goto(self.offset)
		
		numRelocs = int(self.size / self.entrySize)
		self.relocs = []
		
		for relIdx in range(0, numRelocs):
			reloc = {}
			reloc['offset'] = stream.read_long()
			info = stream.read_long()
			reloc['symbolIdx'] = (info >> 8) & 0xFFFFFF
			reloc['relocType'] = info & 0xFF
			reloc['addend'] = stream.read_long()
			
			self.relocs.append(reloc)
		
class PreplfFile:
	def __init__(self, preplfFilename):
		stream = InputStream(preplfFilename)
		self.validate_elf_header(stream)
		self.read_elf_sections(stream)
		
	def validate_elf_header(self, stream):
		stream.goto(0)
		magic = stream.read_long()
		format = stream.read_byte()
		endian = stream.read_byte()
		version = stream.read_byte()
		osabi = stream.read_byte()
		assert(magic == 0x7F454C46)
		assert(format == 0x1)
		assert(endian == 0x2)
		assert(version == 0x1)
		assert(osabi == 0x0)
		
		stream.goto(0x10)
		type = stream.read_short()
		machine = stream.read_short()
		version = stream.read_long()
		assert(type == 0x1)
		assert(machine == 0x14)
		assert(version == 0x1)
	
	def read_elf_sections(self, stream):
		# Read section header info
		stream.goto(0x20)
		sectionHeadersOffset = stream.read_long()
		
		stream.goto(0x2E)
		sectionHeaderSize = stream.read_short()
		numSections = stream.read_short()
		nameSectionIdx = stream.read_short()
		
		# Read section header entries
		stream.goto(sectionHeadersOffset + (sectionHeaderSize * nameSectionIdx) + 0x10)
		self.namesOffset = stream.read_long()
		
		stream.goto(sectionHeadersOffset)
		self.sections = []
		
		for secIdx in range(0, numSections):
			stream.skip(4)
			secType = stream.read_long()
			stream.skip(-8)
			
			if secType == EST_SYMTAB: 	section = ElfSymbolSection(stream, self)
			elif secType == EST_RELA:	section = ElfRelocSection(stream, self)
			else:						section = ElfSection(stream, self)
			self.sections.append(section)
		
		for section in self.sections:
			section.read_section(stream)
	
	def section_by_name(self, secName):
		for section in self.sections:
			if section.name == secName:
				return section
		
		return None
		
	def symbol_by_name(self, symName):
		for section in self.sections:
			if section.type == EST_SYMTAB:
				symbol = section.symbol_by_name(symName)
				if symbol is not None: return symbol
		
		return None
		
	def fetch_name(self, stream, nameOffset):
		curOffset = stream.tell()
		stream.goto(self.namesOffset + nameOffset)
		name = stream.read_string()
		stream.goto(curOffset)
		return name
		
	def fetch_symbol_name(self, stream, nameOffset, symNameSecIdx):
		curOffset = stream.tell()
		stream.goto(self.sections[symNameSecIdx].offset + nameOffset)
		name = stream.read_string()
		stream.goto(curOffset)
		return name
		
	def fetch_symbol(self, symSecIdx, symIdx):
		return self.sections[symSecIdx].symbols[symIdx]
		
	def get_section_name(self, sectionIdx):
		if sectionIdx <= 0 or sectionIdx >= len(self.sections):
			return "NULL"
		
		out = self.sections[sectionIdx].name
		if not out: out = "NULL"
		return out
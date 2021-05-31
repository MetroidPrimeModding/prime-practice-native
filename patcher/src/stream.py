import struct

BIG_ENDIAN, LITTLE_ENDIAN = range(2)
SEEK_SET, SEEK_CUR, SEEK_END = range(3)

def define_value_type(formatString):
	# return tuple for big endian + little endian
	return '>' + formatString, '<' + formatString

u64 = define_value_type('Q')
u32 = define_value_type('I')
u16 = define_value_type('H')
u8 = define_value_type('B')

s64 = define_value_type('q')
s32 = define_value_type('i')
s16 = define_value_type('h')
s8 = define_value_type('b')

f32 = define_value_type('f')
f64 = define_value_type('d')

del define_value_type

# Define stream base class
class StreamBase:
	def tell(self):
		return self.offset
	
	def seek(self, offset, origin):
		if origin != SEEK_CUR:
			assert offset >= 0 and offset < self.length
		else:
			assert self.offset + offset >= 0 and self.offset + offset < self.length
		
		assert origin >= SEEK_SET and origin <= SEEK_END
		
		if origin == SEEK_SET:
			self.offset = offset
		elif origin == SEEK_CUR:
			self.offset += offset
		elif origin == SEEK_END:
			self.offset = self.length - offset
	
	def goto(self, offset):
		self.seek(offset, SEEK_SET)
		
	def skip(self, amount):
		self.seek(amount, SEEK_CUR)
		
	def eof(self):
		return self.offset >= self.length

# Define input stream class
class InputStream(StreamBase):
	def __init__(self, filename, endianness = BIG_ENDIAN):
		with open(filename, 'rb') as stream:
			self.data = stream.read()
		self.length = len(self.data)
		self.offset = 0
		self.endian = endianness

	def seek_to_boundary(self, boundary):
		self.skip( (self.offset + boundary - 1) & ~(boundary - 1) )

	def read_bytes(self, numBytes):
		newOffset = self.offset + numBytes
		assert newOffset >= 0 and newOffset <= self.length
		byteData = self.data[self.offset:newOffset]
		self.offset = newOffset
		return byteData

	def read_byte(self):
		return struct.unpack(u8[self.endian], self.read_bytes(1))[0]

	def read_sbyte(self):
		return struct.unpack(s8[self.endian], self.read_bytes(1))[0]

	def read_bool(self):
		byte = self.read_byte()
		assert byte == 0 or byte == 1
		return byte != 0

	def read_short(self):
		return struct.unpack(u16[self.endian], self.read_bytes(2))[0]

	def read_sshort(self):
		return struct.unpack(s16[self.endian], self.read_bytes(2))[0]

	def read_long(self):
		return struct.unpack(u32[self.endian], self.read_bytes(4))[0]

	def read_slong(self):
		return struct.unpack(s32[self.endian], self.read_bytes(4))[0]

	def read_longlong(self):
		return struct.unpack(u64[self.endian], self.read_bytes(8))[0]

	def read_slonglong(self):
		return struct.unpack(s64[self.endian], self.read_bytes(8))[0]

	def read_float(self):
		return struct.unpack(f32[self.endian], self.read_bytes(4))[0]

	def read_double(self):
		return struct.unpack(f64[self.endian], self.read_bytes(8))[0]

	def read_string(self):
		out = ""

		while True:
			byte = self.read_byte()
			if (byte == 0):
				break
			out += chr(byte)

		return out

	def read_sized_string(self):
		size = self.read_long()
		formatStr = "%ds" % size
		return struct.unpack(formatStr, self.read_bytes(size))[0]

# Define output stream class
class OutputStream(StreamBase):
	def __init__(self, endianness = BIG_ENDIAN):
		self.data = bytearray()
		self.length = 0
		self.offset = 0
		self.endian = endianness
	
	def save_file(self, filename):
		stream = open(filename, "wb")
		stream.write(self.data)
		stream.close()

	def write_to_boundary(self, boundary, fill = 0):
		bytesToWrite = ((self.offset + boundary - 1) & ~(boundary - 1)) - self.offset
		
		for b in range(0, bytesToWrite):
			self.write_byte(fill)
		
	def write_bytes(self, data):
		dataSize = len(data)
		
		if self.offset + len(data) > self.length:
			amtToExpand = (self.offset + len(data)) - self.length
			self.data.extend(bytearray(amtToExpand))
			self.length = len(self.data)
		
		self.data[self.offset:self.offset + dataSize] = data
		self.offset += dataSize
		
	def write_byte(self, val):
		self.write_bytes(struct.pack(u8[self.endian], val))
	
	def write_sbyte(self, val):
		self.write_bytes(struct.pack(s8[self.endian], val))
		
	def write_bool(self, val):
		v = 1 if val is True else 0
		self.write_byte(v)
		
	def write_short(self, val):
		self.write_bytes(struct.pack(u16[self.endian], val))
		
	def write_sshort(self, val):
		self.write_bytes(struct.pack(s16[self.endian], val))
		
	def write_long(self, val):
		self.write_bytes(struct.pack(u32[self.endian], val))
		
	def write_slong(self, val):
		self.write_bytes(struct.pack(s32[self.endian], val))
		
	def write_longlong(self, val):
		self.write_bytes(struct.pack(u64[self.endian], val))
		
	def write_slonglong(self, val):
		self.write_bytes(struct.pack(s64[self.endian], val))
		
	def write_float(self, val):
		self.write_bytes(struct.pack(f32[self.endian], val))
		
	def write_double(self, val):
		self.write_bytes(struct.pack(f64[self.endian], val))
		
	def write_string(self, val):
		for chr in val:
			self.write_byte(ord(chr))
			if chr == 0:
				break
		
		# Write terminating 0
		if val[len(val) - 1] != 0:
			self.write_byte(0)
	
	def write_sized_string(self, val):
		self.write_long(len(val))
		self.write_bytes(struct.pack("%ds" % len(val), val.encode()))

# Utility functions
def print_offset(stream):
	print("Offset: 0x%X" % stream.tell())
	
def check_success(sectionName, offset, sectionEnd):
	if offset == sectionEnd:
		print("Fully parsed %s. offset 0x%X." % (sectionName, offset))
		return True
	else:
		print("Failed to fully parse the %s chunk! offset 0x%X, end of section is 0x%X" % (sectionName, offset, sectionEnd))
		return False
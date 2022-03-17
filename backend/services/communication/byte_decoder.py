import struct

DEFAULT_SIZES_OF_TYPES = {'f': 4, 'i': 4}
DEFAULT_SIZE = 4


class ByteDecoder:

    def __init__(self, data: bytes, types: list):
        self.data = data

        self.structure = []
        for type in types:
            if type in DEFAULT_SIZES_OF_TYPES:
                self.structure.append((type, DEFAULT_SIZES_OF_TYPES[type]))
            else:
                self.structure.append((type, DEFAULT_SIZE))

    def get_values(self) -> list:
        values = []
        current_offset = 0
        for (type, size) in self.structure:
            [value] = self.value_at_index(current_offset, type, size)
            values.append(value)
            current_offset += size
        return values

    def value_at_index(self, index: int, value_type: str, size: int):
        return struct.unpack(value_type, self.data[index:index + size])

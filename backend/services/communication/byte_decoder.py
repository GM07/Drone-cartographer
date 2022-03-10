"""This module provides a class to decode bytes
that are sent from the drone """
import struct

DEFAULT_SIZES_OF_TYPES = {'f': 4, 'i': 4}
DEFAULT_SIZE = 4


class ByteDecoder:
    """This class to decode bytes
    that are sent from the drone """

    def __init__(self, data: bytes, types: list):
        self.data = data

        self.structure = []
        for current in types:
            if current in DEFAULT_SIZES_OF_TYPES:
                self.structure.append(
                    (current, DEFAULT_SIZES_OF_TYPES[current]))
            else:
                self.structure.append((current, DEFAULT_SIZE))

    def get_values(self) -> list:
        values = []
        current_offset = 0
        for (current, size) in self.structure:
            [value] = self.value_at_index(current_offset, current, size)
            values.append(value)
            current_offset += size
        return values

    def value_at_index(self, index: int, value_type: str, size: int):
        return struct.unpack(value_type, self.data[index:index + size])

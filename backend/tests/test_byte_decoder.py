"""This module tests the ByteDecoder."""

import unittest
from unittest import mock
from services.communication.byte_decoder import ByteDecoder, DEFAULT_SIZE, DEFAULT_SIZES_OF_TYPES


class TestApplication(unittest.TestCase):

    def test_init_wrong_type(self):
        byt = bytes([1, 2, 3, 4, 5])
        decoder = ByteDecoder(byt, ['test'])
        self.assertEqual(decoder.data, byt)
        self.assertEqual(decoder.structure, [('test', DEFAULT_SIZE)])

    def test_init_right_type(self):
        byt = bytes([1, 2, 3, 4, 5])
        decoder = ByteDecoder(byt, ['test'])
        self.assertEqual(decoder.data, byt)
        self.assertEqual(decoder.structure,
                         [('test', DEFAULT_SIZES_OF_TYPES['f'])])

    @mock.patch(
        "services.communication.byte_decoder.ByteDecoder.value_at_index",
        return_value=[0])
    def test_get_values(self, self_mock):
        byt = bytes([1, 2, 3, 4])
        decoder = ByteDecoder(byt, ['f', 'f', 'f', 'f'])
        self.assertEqual(decoder.get_values(), [0, 0, 0, 0])

    @mock.patch("services.communication.byte_decoder.struct.unpack")
    def test_value_at_index(self, unpack_mock: mock.MagicMock):
        byt = bytes([1, 2, 3, 4])
        decoder = ByteDecoder(byt, ['f', 'f', 'f', 'f'])
        decoder.value_at_index(0, 'i', 1)
        unpack_mock.assert_called_once_with('i', b'\x01')

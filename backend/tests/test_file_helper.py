"""This module is used to declare the TestFileHelper
class that is used to test the FileHelper class"""

import subprocess
import os
import unittest
import pathlib
from services.communication.file_helper import FileHelper
from unittest.mock import MagicMock, patch


class TestFileHelper(unittest.TestCase):
    """This class is used to test the FileHelper class"""

    @patch('subprocess.getoutput', MagicMock())
    def test_get_files(self):
        abs_path = os.path.abspath(__file__ + '/../../embedded')
        directories = [
            abs_path + '/simulation/src', abs_path + '/shared-firmware/src',
            abs_path + '/shared-firmware/include',
            abs_path + '/embedded-firmware/src'
        ]
        FileHelper.get_files(directories)
        subprocess.getoutput.assert_called()

    @patch('pathlib.Path.read_text', MagicMock())
    def test_get_files_content(self):
        file = ['file']
        FileHelper.get_files_content(file)
        pathlib.Path.read_text.assert_called()

    @patch('os.system', MagicMock())
    def test_update_files(self):
        new_path = os.path.abspath(__file__ + '/../../../path')
        new_file = ['path']
        is_written = False
        FileHelper.update_files(new_file, ' ')
        if os.path.exists(new_path):
            is_written = True
            os.remove(new_path)

        assert is_written is True

from abc import abstractmethod
import glob
import os
from pathlib import Path
import subprocess
from typing import Dict, List


class FileReader:
    # Thread safe constructor
    # __new__ is called before the __init__
    def __new__(cls, *args, **kwargs):
        with cls._lock:
            if not cls._instance:
                cls._instance = super(FileReader, cls).__new__(cls)
            return cls._instance

    @staticmethod
    def get_files(directories: List) -> List:

        files = []
        for directory in directories:
            src_files = subprocess.getoutput(
                f'find {directory} -type f -regex ".*\.cc"').split('\n')
            header_files = subprocess.getoutput(
                f'find {directory} -type f -regex ".*\.h"').split('\n')

            if len(src_files[0]) > 0:
                files.extend(src_files)
            if len(header_files[0]) > 0:
                files.extend(header_files)

        return files

    @staticmethod
    def get_files_content(files: List) -> Dict:
        files_content = dict()
        for file in files:
            content = Path(file).read_text()
            files_content[file] = content

        return files_content
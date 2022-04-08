from operator import mod
import os
from pathlib import Path
import shlex
import subprocess
from typing import Dict, List


class FileHelper:
    # Thread safe constructor
    # __new__ is called before the __init__
    def __new__(cls, *args, **kwargs):
        with cls._lock:
            if not cls._instance:
                cls._instance = super(FileHelper, cls).__new__(cls)
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

    @staticmethod
    def update_files(paths: List, contents: List) -> None:
        for index, path in enumerate(paths):
            if len(path) == 0:
                continue
            content: str = contents[index]
            os.system(f'rm ../{path}')
            os.system(f'touch ../{path}')
            file = open('../' + path, 'w')
            file.write(content)
            file.close()
"""This module is used to declare the Abstract communication class that
is used to communicate """
from abc import abstractmethod
from constants import COMMANDS


class AbstractComm:
    DELAY_RECEIVER_MS = 1000

    @abstractmethod
    def send_command(self, command: COMMANDS, links = []) -> None:
        pass

    def shutdown(self):
        pass

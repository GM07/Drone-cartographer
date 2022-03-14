

from abc import abstractmethod
from flask_socketio import SocketIO
from constants import COMMANDS


class AbstractComm:
    DELAY_RECEIVER_MS = 1000

    def __init__(self, socketIO: SocketIO):
        self.SOCKETIO = socketIO

    @abstractmethod
    def send_command(self, command: COMMANDS, links = []) -> None:
        pass

    def shutdown(self):
        pass

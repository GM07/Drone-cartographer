"""This module is used to declare the Abstract communication class that
is used to communicate """
from abc import abstractmethod
from time import perf_counter
from flask_socketio import SocketIO

from typing import List, Tuple
from constants import COMMANDS
from services.communication.database.mongo_interface import Database, Mission


class AbstractComm:
    DELAY_RECEIVER_MS = 1000
    logs: List[Tuple[str, str]] = []
    current_mission: Mission

    def __init__(self, socket_io: SocketIO) -> None:
        self.__socket_io = socket_io

    def send_log(self, log: List[Tuple[str, str]]):
        print(log)

        self.__socket_io.emit('get_logs',
                              log,
                              namespace='/getLogs',
                              broadcast=True,
                              include_self=False,
                              skip_sid=True)

        self.logs += log

    @abstractmethod
    def send_command(self, command: COMMANDS) -> None:
        pass

    def shutdown(self):

        self.current_mission.flight_duration = self.start_time - perf_counter()
        self.current_mission.logs = self.logs
        self.logs = []
        database = Database()
        database.upload_mission_info(self.current_mission)
        pass

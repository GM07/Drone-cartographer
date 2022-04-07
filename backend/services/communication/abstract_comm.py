"""This module is used to declare the Abstract communication class that
is used to communicate """
from abc import ABCMeta, abstractmethod
import gevent.queue
from flask_socketio import SocketIO
from services.communication.comm_tasks import LOGS_QUEUE
from flask_socketio import SocketIO

from services.communication.comm_tasks import DRONE_STATUS_QUEUE, LOGS_QUEUE
from typing import List, Tuple
from constants import COMMANDS
from services.communication.database.mongo_interface import MissionManager
from datetime import datetime


class AbstractComm(metaclass=ABCMeta):
    DELAY_RECEIVER_MS = 50
    logs: List[Tuple[str, str]] = []
    mission_manager: MissionManager = MissionManager()

    def __init__(self, socket_io: SocketIO, drone_list: list):
        self.SOCKETIO = socket_io
        self.drone_list = drone_list

    def send_log(self, log: str):

        dated_log = [(datetime.now().isoformat(), log)]
        try:
            LOGS_QUEUE.put_nowait(dated_log)
        except gevent.queue.Full:
            pass

        self.logs += log

    def send_drone_status(self, status: List[Tuple[str, str]]):
        try:
            DRONE_STATUS_QUEUE.put_nowait(status)
        except gevent.queue.Full:
            pass

    def set_drone(self, drone_list):
        self.drone_list = drone_list

    def get_drones(self):
        return self.drone_list

    @abstractmethod
    def send_command(self, command: COMMANDS, links=[]) -> None:
        pass

    @abstractmethod
    def validate_name(self, name: str) -> str:
        pass

    @abstractmethod
    def shutdown(self):
        pass

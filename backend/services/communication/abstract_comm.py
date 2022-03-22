"""This module is used to declare the Abstract communication class that
is used to communicate """
from abc import ABCMeta, abstractmethod
import gevent.queue
import os
from flask_socketio import SocketIO
from services.communication.comm_tasks import LOGS_QUEUE
from flask_socketio import SocketIO

from services.communication.comm_tasks import DRONE_STATUS_QUEUE, LOGS_QUEUE
from typing import List, Tuple
from constants import COMMANDS
from services.communication.database.mongo_interface import Mission


class AbstractComm(metaclass=ABCMeta):
    DELAY_RECEIVER_MS = 50
    logs: List[Tuple[str, str]] = []
    current_mission: Mission

    def __init__(self, socket_io: SocketIO, drone_list: list):
        self.SOCKETIO = socket_io
        self.drone_list = drone_list

    def send_log(self, log: List[Tuple[str, str]]):
        try:
            LOGS_QUEUE.put_nowait(log)
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

    def recompile(self):
        recompileSimulation = "cd /workspaces/INF3995-106/embedded/simulation && if test -d build; then rm -rf build; fi && mkdir build && cd build && cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../../ && ninja"
        recompileFirmware = "cd /workspaces/INF3995-106/embedded/embedded-firmware make clean && make -j`nproc`"
        os.system(f"rm -f out.log && touch out.log && docker exec -t embedded sh -c '{recompileSimulation} && {recompileFirmware}' > out.log 2>&1")

    @abstractmethod
    def send_command(self, command: COMMANDS, links=[]) -> None:
        pass

    @abstractmethod
    def validate_name(self, name: str) -> str:
        pass

    @abstractmethod
    def shutdown(self):
        pass

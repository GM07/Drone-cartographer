"""This module is used to declare the Abstract communication class that
is used to communicate """
from abc import abstractmethod
import gevent.queue

from services.communication.comm_tasks import LOGS_QUEUE
from typing import List, Tuple
from constants import COMMANDS
from services.communication.database.mongo_interface import Mission


class AbstractComm:
    DELAY_RECEIVER_MS = 1000
    logs: List[Tuple[str, str]] = []
    current_mission: Mission

    def send_log(self, log: List[Tuple[str, str]]):
        try:
            LOGS_QUEUE.put_nowait(log)
        except gevent.queue.Full:
            pass

        self.logs += log

    def set_drone(self, drone_list):
        self.drone_list = drone_list

    def get_drones(self):
        return self.drone_list

    @abstractmethod
    def send_command(self, command: COMMANDS, links=[]) -> None:
        pass

    def shutdown(self):
        pass

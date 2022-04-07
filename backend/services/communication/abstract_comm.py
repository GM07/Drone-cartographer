"""This module is used to declare the Abstract communication class that
is used to communicate """
from abc import ABCMeta, abstractmethod
import threading
import gevent.queue
from flask_socketio import SocketIO
from services.communication.comm_tasks import LOGS_QUEUE
from flask_socketio import SocketIO

from services.communication.comm_tasks import DRONE_STATUS_QUEUE, LOGS_QUEUE
from typing import Any, Dict, List, Tuple
from constants import COMMANDS
from services.data.drone_data import DroneData
from services.data.starting_drone_data import StartingDroneData
from services.communication.database.mongo_interface import MissionManager
from datetime import datetime


class AbstractComm(metaclass=ABCMeta):
    drone_data_lock = threading.Lock()
    DELAY_RECEIVER_MS = 50
    logs: List[Tuple[str, str]] = []
    mission_manager: MissionManager = MissionManager()

    def __init__(self, socket_io: SocketIO,
                 drone_list: List[StartingDroneData]):
        self.SOCKETIO = socket_io
        self.drone_list: List[StartingDroneData] = []
        self.drone_data_list: Dict[str, DroneData] = {}
        self.nb_connections = 0
        self.set_drone(drone_list)

    def _init_drone_data(self):
        with self.drone_data_lock:
            self.drone_data_list: Dict[str, DroneData] = {}
            for drone in self.drone_list:
                drone_data = DroneData(drone.name, bytes(), True)
                drone_data.position.x = drone.starting_x_pos
                drone_data.position.y = drone.starting_y_pos
                self.drone_data_list[drone_data.name] = drone_data

    def set_drone(self, drone_list: List[StartingDroneData]):
        self.drone_list = drone_list
        self.nb_connections = len(drone_list)
        self._init_drone_data()

    def get_drones(self) -> List[StartingDroneData]:
        return self.drone_list

    def send_log(self, log: str):

        dated_log = [(datetime.now().isoformat(), log)]
        try:
            LOGS_QUEUE.put_nowait(dated_log)
        except gevent.queue.Full:
            pass

        self.logs += log

    def send_drone_status(self, status: List[Dict[str, Any]]):
        try:
            DRONE_STATUS_QUEUE.put_nowait(status)
        except gevent.queue.Full:
            pass

    def set_drone_data(self, drone_data: DroneData) -> None:
        with self.drone_data_lock:
            if drone_data.name in self.drone_data_list:
                self.drone_data_list[drone_data.name] = drone_data

    def get_full_drone_data(self) -> Dict[str, Any]:
        with self.drone_data_lock:
            returned_dict_list: List[Dict[str, Any]] = []
            for i in range(len(self.drone_list)):
                current_dict = self.drone_data_list[
                    self.drone_list[i].name].to_dict()
                current_dict['startingXPos'] = self.drone_list[i].starting_x_pos
                current_dict['startingYPos'] = self.drone_list[i].starting_y_pos
                current_dict['startingOrientation'] = self.drone_list[
                    i].starting_orientation
                returned_dict_list.append(current_dict)
            return returned_dict_list

    @abstractmethod
    def send_command(self, command: COMMANDS, links=[]) -> None:
        pass

    @abstractmethod
    def validate_name(self, name: str) -> str:
        pass

    @abstractmethod
    def shutdown(self):
        pass

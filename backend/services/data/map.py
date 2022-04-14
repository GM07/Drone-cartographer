from flask_socketio import SocketIO
import threading
from functools import wraps
from typing import Dict, List

import numpy

from services.data.drone_data import DroneData, DroneSensors


def lock(lock: threading.Lock):

    def real_lock(function):

        @wraps(function)
        def wrapper(*args, **kwargs):
            lock.acquire()
            return_value = function(*args, **kwargs)
            lock.release()
            return return_value

        return wrapper

    return real_lock


class MapData:

    def __init__(self, drone_id: str, drone_data: DroneData):
        self.drone_id = drone_id
        self.drone_data = drone_data

    def to_socket_data(self):
        return {
            "id": self.drone_id,
            "position": [
                self.drone_data.position.x, self.drone_data.position.y
            ],
            "sensors": {
                "front": self.drone_data.sensors.front,
                "right": self.drone_data.sensors.right,
                "back": self.drone_data.sensors.back,
                "left": self.drone_data.sensors.left
            }
        }


# Thread safe Singleton
class Map:
    _instance = None
    _lock = threading.Lock()
    _DATA_NB_FILTER = 4
    raw_individual_data: Dict[str, List[DroneData]] = {}
    raw_data: List[MapData] = list()
    filtered_data: List[MapData] = list()
    buffer_data: Dict[str, List[DroneData]] = {}
    drone_len = 0

    # Thread safe constructor
    # __new__ is called before the __init__
    def __new__(cls, *args, **kwargs):
        with cls._lock:
            if not cls._instance:
                cls._instance = super(Map, cls).__new__(cls)
            return cls._instance

    @lock(_lock)
    def add_data(self, map_data: MapData, socket: SocketIO):

        drone_id = map_data.drone_id
        data = map_data.drone_data

        if not drone_id in self.raw_individual_data:
            self.raw_individual_data[drone_id] = list()
        self.raw_individual_data[drone_id].append(data)

        self.raw_data.append(map_data)

        if drone_id not in self.buffer_data:
            self.buffer_data[drone_id] = list()
        self.buffer_data[drone_id].append(data)

        if len(self.buffer_data[drone_id]) >= Map._DATA_NB_FILTER:
            sending_data = map_data
            front, left, back, right = self.mean_data_per_sensor(
                self.buffer_data[drone_id])
            mean_sensors: DroneSensors = DroneSensors(
                Map.filter_sensor_value(front), Map.filter_sensor_value(left),
                Map.filter_sensor_value(back), Map.filter_sensor_value(right))
            sending_data.drone_data.update_sensors(mean_sensors)
            self.emit_data(sending_data, socket)
            self.filtered_data.append(sending_data)
            self.buffer_data[drone_id].clear()

    @staticmethod
    def filter_sensor_value(value):
        if value > 2000:
            return -1
        return value

    def emit_data(self, map_data: MapData, socket: SocketIO):
        socket.emit(
            'getMapData',
            [map_data.to_socket_data()],
            namespace='/getAllMapData',
            broadcast=True,
        )

    @lock(_lock)
    def get_all_filtered_data():
        socket_filtered_data = []
        for data in Map.filtered_data:
            socket_filtered_data.append(data.to_socket_data())
        return socket_filtered_data

    def filter_outliers(self, initial_data, counter_initial_data):
        sending_data = -1
        if (len(initial_data) > 0 and
                counter_initial_data / len(initial_data) < 0.5):
            diff = numpy.abs(initial_data - numpy.median(initial_data))
            mdev = numpy.median(diff)
            s = diff / mdev if mdev else 0.
            data = numpy.array(initial_data)[s < 5]

            sending_data = numpy.mean(data)

        return sending_data

    def mean_data_per_sensor(self, drone_list: List[DroneData]):
        fronts, lefts, rights, backs = [], [], [], []
        counter_fronts, counter_lefts, counter_backs, counter_rights = 4, 4, 4, 4
        for drone in drone_list:
            if (drone.sensors.front > 0):
                fronts.append(drone.sensors.front)
                counter_fronts -= 1
            if (drone.sensors.left > 0):
                lefts.append(drone.sensors.left)
                counter_lefts -= 1
            if (drone.sensors.right > 0):
                rights.append(drone.sensors.right)
                counter_rights -= 1
            if (drone.sensors.back > 0):
                backs.append(drone.sensors.back)
                counter_backs -= 1

        sending_front_data = self.filter_outliers(fronts, counter_fronts)
        sending_back_data = self.filter_outliers(backs, counter_backs)
        sending_left_data = self.filter_outliers(lefts, counter_lefts)
        sending_right_data = self.filter_outliers(rights, counter_rights)

        return sending_front_data, sending_left_data, sending_back_data, sending_right_data

    def set_drone_len(self, drone_len):
        self.drone_len = drone_len

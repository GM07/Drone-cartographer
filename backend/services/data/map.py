import threading
from functools import wraps
from typing import Dict, List

from services.data.drone_data import DroneData


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

    def __init__(self, drone_id, drone_data: DroneData):
        self.drone_id = drone_id
        self.drone_data = drone_data


# Thread safe Singleton
class Map:
    _instance = None
    _lock = threading.Lock()
    _DATA_NB_FILTER = 4

    def __init__(self):
        self.raw_individual_data: Dict[str, List[DroneData]] = {}
        self.raw_data: List[MapData] = list()
        self.filtered_data: List[MapData] = list()
        self.buffer_data: Dict[str, List[DroneData]] = {}
        self.drone_len = 0

    # Thread safe constructor
    # __new__ is called before the __init__
    def __new__(cls, *args, **kwargs):
        with cls._lock:
            if not cls._instance:
                cls._instance = super(Map, cls).__new__(cls)
            return cls._instance

    @lock(_lock)
    def add_data(self, map_data: MapData):
        id = map_data.drone_id
        data = map_data.drone_data

        if self.raw_individual_data[id] is None:
            self.raw_individual_data[id] = list()
        self.raw_individual_data[id].append(data)

        self.raw_data.append(map_data)

        if self.buffer_data[id] is None:
            self.buffer_data[id] = list()
        self.buffer_data[id].append(data)

        if (len(self.buffer_data[id]) >= self.drone_len * Map._DATA_NB_FILTER):
            front, left, back, right = self.mean_data_per_sensor(
                self.buffer_data[id])

    def mean_data_per_sensor(self, list: List[DroneData]):

        m_front, m_left, m_back, m_right = 0, 0, 0, 0
        for elem in list:
            m_front += elem.sensors.front
            m_left += elem.sensors.left
            m_back += elem.sensors.back
            m_right += elem.sensors.right

        m_front /= len(list)
        m_left /= len(list)
        m_back /= len(list)
        m_right /= len(list)

        return m_front, m_left, m_back, m_right

    def set_drone_len(self, drone_len):
        self.drone_len = drone_len
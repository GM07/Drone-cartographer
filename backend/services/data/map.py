import threading
from functools import wraps
from typing import List

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


# Thread safe Singleton
class Map:
    _instance = None
    _lock = threading.Lock()

    def __init__(self):

        self.raw_data: List[DroneData] = list()
        self.averaged_data: List[DroneData] = list()
        self.drone_len = 0

    # Thread safe constructor
    # __new__ is called before the __init__
    def __new__(cls, *args, **kwargs):
        with cls._lock:
            if not cls._instance:
                cls._instance = super(Map, cls).__new__(cls)
            return cls._instance

    @lock(_lock)
    def add_data(self, data: DroneData):
        self.raw_data.append(data)
        self.averaged_data.append(data)

        # if (len(self.averaged_data) >= self.drone_len * )

    def set_drone_len(self, drone_len):
        self.drone_len = drone_len
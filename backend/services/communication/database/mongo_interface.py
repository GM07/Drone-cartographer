"""This module contains the Database class that is used to connect
with a mongoDb database

Example usage is :
database = Database()

database.upload_mission_info(mission_info)
"""
from dataclasses import dataclass
import threading

from typing import List, Tuple, Dict
from mongomock import ObjectId
from pymongo import MongoClient
from datetime import datetime
from services.data.drone_data import DroneData, Point2D
from services.data.map import Map
from time import perf_counter
import math


@dataclass
class Point:
    x: float
    y: float


@dataclass
class Mission:
    """This class is a dataclass that holds all the parameters
    needed to describe a mission"""
    time_stamp: str
    flight_duration: float
    number_of_drones: int
    is_simulated: bool
    total_distance: float
    map: str
    logs: List[Tuple[str, str]]

    def __init__(self,
                 flight_duration: float,
                 number_of_drones: int,
                 is_simulated: bool,
                 total_distance: float,
                 map: str = ''):

        self.flight_duration = flight_duration
        self.number_of_drones = number_of_drones
        self.is_simulated = is_simulated
        self.total_distance = total_distance
        self.map = map
        self.time_stamp = datetime.now().isoformat()
        self.logs = []


class Database:
    """This class has methods that communicate with a mongoDb that
    is running in localhost
    It lets you add and get objects from the database"""

    def __init__(self) -> None:
        try:

            self.client = MongoClient('mongodb://127.0.0.1:27017/',
                                      connectTimeoutMS=2000,
                                      socketTimeoutMS=2000,
                                      serverSelectionTimeoutMS=2000)
            self.db = self.client['admin']
        except:
            print("Couldn't connect to database please relaunch")

    def upload_mission_info(self, mission: Mission):
        try:
            return self.db.missions.insert_one(mission.__dict__).inserted_id
        except:
            print('Database error')

    def get_all_missions_time_stamps(self) -> list:
        try:
            return serialize_objectid_from_result(
                list(
                    self.db.missions.aggregate([{
                        '$project': {
                            'time_stamp': 1,
                            'is_simulated': 1,
                            'total_distance': 1,
                            'number_of_drones': 1,
                            'flight_duration': 1
                        }
                    }])))
        except:
            print('Database error')
            return []

    def get_mission_from_id(self, identifier: str):
        try:
            mission = self.db.missions.find_one({'_id': ObjectId(identifier)})
        except:
            print('Database error')
            return {}
        if mission is not None:
            mission['_id'] = str(mission['_id'])
        return mission

    def get_mission_logs_from_id(self, identifier: str):
        try:
            mission = self.db.missions.find_one({'_id': ObjectId(identifier)},
                                                {"logs": 1})
        except:
            print('Database error')
            return {}
        if mission is not None:
            mission['_id'] = str(mission['_id'])
        return mission

    def get_mission_maps_from_id(self, identifier: str):
        try:
            mission = self.db.missions.find_one({'_id': ObjectId(identifier)},
                                                {"map": 1})
        except:
            print('Database error')
            return {}
        if mission is not None:
            mission['_id'] = str(mission['_id'])
        return mission

    def remove_mission_from_id(self, identifier: str) -> bool:
        try:
            return self.db.missions.delete_one({
                '_id': ObjectId(identifier)
            }).deleted_count != 0
        except:
            print('Database error')
            return False

    def update_mission_info_from_id(self, mission: Mission,
                                    identifier: str) -> bool:
        try:
            return self.db.missions.replace_one({
                '_id': ObjectId(identifier)
            }, mission.__dict__).matched_count != 0
        except:
            print('Database error')
            return False


def serialize_objectid_from_result(result: list):
    for test in result:
        test['_id'] = str(test['_id'])
    return result


class MissionManager:
    current_mission: Mission = {}
    current_mission_start_time: float
    last_known_positions: Dict[str, Point2D] = {}
    lock = threading.Lock()
    is_mission_started = False

    def start_current_mission(self, drone_list: List[DroneData],
                              is_simulated: bool):
        self.lock.acquire()
        self.current_mission = Mission(0, len(drone_list), is_simulated, 0,
                                       [[]])
        self.mission_start_time = perf_counter()
        for drone in drone_list:
            self.last_known_positions[drone.name] = Point2D(0, 0)
        self.is_mission_started = True
        self.lock.release()

    def end_current_mission(self, logs: List[Tuple[str, str]]):
        self.lock.acquire()

        if not self.is_mission_started:
            self.lock.release()
            return
        self.current_mission.flight_duration = perf_counter(
        ) - self.mission_start_time
        self.current_mission.logs = logs

        self.is_mission_started = False
        Map.raw_data.clear()
        Map.filtered_data.clear()
        database = Database()
        database.upload_mission_info(self.current_mission)
        self.lock.release()

    def update_position(self, data: DroneData):

        self.lock.acquire()
        if not self.is_mission_started:
            self.lock.release()
            return

        self.current_mission.total_distance += math.hypot(
            data.position.x - self.last_known_positions[data.name].x,
            data.position.y - self.last_known_positions[data.name].y)

        self.last_known_positions[data.name] = data.position
        self.lock.release()

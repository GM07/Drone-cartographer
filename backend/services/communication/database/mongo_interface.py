"""This module contains the Database class that is used to connect
with a mongoDb database

Example usage is :
database = Database()

database.upload_mission_info(mission_info)
"""
from dataclasses import dataclass
from typing import List, Tuple
from mongomock import ObjectId
from pymongo import MongoClient
from datetime import datetime
from services.data.drone_data import DroneData
from constants import COMMANDS


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
    maps: List[List[Point]]
    logs: List[Tuple[str, str]]

    def __init__(self,
                 flight_duration: float,
                 number_of_drones: int,
                 is_simulated: bool,
                 total_distance: float,
                 maps: List[List[Point]] = [[]]):

        self.flight_duration = flight_duration
        self.number_of_drones = number_of_drones
        self.is_simulated = is_simulated
        self.total_distance = total_distance
        self.maps = maps
        self.time_stamp = datetime.now().isoformat()
        self.logs = []


class Database:
    """This class has methods that communicate with a mongoDb that
    is running in localhost
    It lets you add and get objects from the database"""

    def __init__(self) -> None:
        self.client = MongoClient('mongodb://127.0.0.1:27017/')
        self.db = self.client['admin']

    def upload_mission_info(self, mission: Mission):
        return self.db.missions.insert_one(mission.__dict__).inserted_id

    def get_all_missions_time_stamps(self) -> list:
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

    def get_mission_from_id(self, identifier: str):
        mission = self.db.missions.find_one({'_id': ObjectId(identifier)})
        if mission is not None:
            mission['_id'] = str(mission['_id'])
        return mission

    def remove_mission_from_id(self, identifier: str) -> bool:
        return self.db.missions.delete_one({
            '_id': ObjectId(identifier)
        }).deleted_count != 0

    def update_mission_info_from_id(self, mission: Mission,
                                    identifier: str) -> bool:
        return self.db.missions.replace_one({
            '_id': ObjectId(identifier)
        }, mission.__dict__).matched_count != 0


def serialize_objectid_from_result(result: list):
    for test in result:
        test['_id'] = str(test['_id'])
    return result

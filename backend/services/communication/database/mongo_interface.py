"""This module contains the Database class that is used to connect
with a mongoDb database

Example usage is :
database = Database()

database.upload_mission_info(mission_info)
"""
from dataclasses import dataclass
from typing import List
from pymongo import MongoClient
from datetime import datetime
from services.data.drone_data import DroneData
from constants import COMMANDS


@dataclass
class Logs:
    drone_info: List[List[DroneData]]
    commands: List[str]

    def __init__(self):
        self.drone_info = [[]]
        self.commands = [COMMANDS]


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
    logs: Logs

    def __init__(self, flight_duration: float, number_of_drones: int,
                 is_simulated: bool, total_distance: float,
                 maps: List[List[Point]]):

        self.flight_duration = flight_duration
        self.number_of_drones = number_of_drones
        self.is_simulated = is_simulated
        self.total_distance = total_distance
        self.maps = maps
        self.time_stamp = datetime.now().isoformat()
        self.logs = Logs()


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
        return self.db.missions.find_one({'_id': identifier})

    def remove_mission_from_id(self, identifier: str) -> bool:
        return self.db.missions.delete_one({
            '_id': identifier
        }).deleted_count != 0

    def update_mission_info_from_id(self, mission: Mission,
                                    identifier: str) -> bool:
        return self.db.missions.replace_one({
            '_id': identifier
        }, mission.__dict__).matched_count != 0

    def test_connection(self):
        self.db.drop_collection('missions')
        self.upload_mission_info(Mission(10, 4, True, 40, [[]]))
        self.upload_mission_info(Mission(10, 4, False, 20, [[]]))
        self.upload_mission_info(Mission(11, 5, True, 41, [[]]))
        self.upload_mission_info(Mission(10, 3, False, 40, [[{
            'x': 1,
            'y': 2
        }]]))

        result = self.get_all_missions_time_stamps()

        for test in result:
            print(test)


def serialize_objectid_from_result(result: list):
    for test in result:
        test['_id'] = str(test['_id'])
    return result

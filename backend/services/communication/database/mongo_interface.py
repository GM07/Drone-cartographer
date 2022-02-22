"""This module contains the Database class that is used to connect
with a mongoDb database

Example usage is :
database = Database()

database.upload_mission_info(mission_info)
"""
from dataclasses import dataclass
from typing import List
from pymongo import MongoClient
import time


@dataclass
class Point:
    x: float
    y: float


@dataclass
class Mission:
    time_stamp: str
    temps_de_vol: float
    nombre_de_drones: int
    est_simule: bool
    distance_totale: float
    cartes: List[List[Point]]

class Database:
    """This class has methods that communicate with a mongoDb that
    is running in localhost
    It lets you add and get objects from the database"""

    def __init__(self) -> None:
        self.client = MongoClient('mongodb://127.0.0.1:27017/')
        self.db = self.client['admin']

    def upload_mission_info(self, mission: Mission):
        mission.time_stamp = time.localtime()
        return self.db.missions.insert_one(mission.__dict__).inserted_id

    def get_all_missions_time_stamps(self) -> list:
        return serialize_objectid_from_result(list(self.db.missions.aggregate(
            [{'$project': {'time_stamp': 1,'est_simule':1}}])))

    def get_mission_from_id(self, identifier: str):
        print(identifier)
        return self.db.missions.find_one({'_id': identifier})

    def remove_mission_from_id(self, identifier: str) -> bool:
        return self.db.missions.delete_one(
            {'_id': identifier}).deleted_count != 0

    def update_mission_info_from_id(self, mission: Mission,
    identifier: str) -> bool:
        return self.db.missions.replace_one(
            {'_id': identifier}, mission.__dict__).matched_count !=0        

    def test_connection(self):
        self.db.drop_collection('missions')
        self.upload_mission_info(Mission('1',10,4,True,40,[[]]))
        self.upload_mission_info(Mission('2',10,4,False,20,[[]]))
        self.upload_mission_info(Mission('3',11,5,True,41,[[]]))
        self.upload_mission_info(Mission('4',10,3,False,40,[[{'x':1,'y':2}]]))

        result = self.get_all_missions_time_stamps()

        for test in result:
            print(test)


def serialize_objectid_from_result(result: list):
    for test in result:
        test['_id'] = str(test['_id'])
    return result

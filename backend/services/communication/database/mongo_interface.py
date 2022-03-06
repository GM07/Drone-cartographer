"""This module contains the Database class that is used to connect
with a mongoDb database

Example usage is :
database = Database()

database.upload_mission_info(mission_info)
"""
from typing import Any
from pymongo import MongoClient
import time


class Database:
    """This class has methods that communicate with a mongoDb that
    is running in localhost
    It lets you add and get objects from the database"""

    def __init__(self) -> None:
        self.client = MongoClient('mongodb://127.0.0.1:27017/')
        self.db = self.client['admin']

    def upload_mission_info(self, mission: Any):
        mission['timeStamp'] = time.localtime()
        return self.db.missions.insert_one(mission).inserted_id

    def get_all_missions_time_stamps(self) -> list:
        return serialize_objectid_from_result(
            list(self.db.missions.aggregate([{
                '$project': {
                    'timeStamp': 1
                }
            }])))

    def get_missions_from_name(self, name: str):
        return serialize_objectid_from_result(
            list(self.db.missions.find({'name': name})))

    def get_mission_from_id(self, identifier: str):
        print(identifier)
        return self.db.missions.find_one({'_id': identifier})

    def remove_mission_from_id(self, identifier: str) -> bool:
        return self.db.missions.delete_one({
            '_id': identifier
        }).deleted_count != 0

    def update_mission_info_from_id(self, mission: Any) -> bool:
        if '_id' in mission:
            return self.db.missions.replace_one({'_id': mission['_id']},
                                                mission)
        return False

    def test_connection(self):
        self.db.drop_collection('missions')
        self.upload_mission_info({'name': 'test0'})
        self.upload_mission_info({'name': 'test1'})
        self.upload_mission_info({'name': 'test2'})
        self.upload_mission_info({'name': 'test3'})

        result = self.db.missions.find()

        for test in result:
            print(test)


def serialize_objectid_from_result(result: list):
    for test in result:
        test['_id'] = str(test['_id'])
    return result

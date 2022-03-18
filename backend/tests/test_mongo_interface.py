"""This module tests the functions implemented to communicate with the mongoDb.

It is called when you run pylint from the root. It contains unit test methods
and implements Mongomock
"""
from datetime import datetime
from email.mime import base
from services.communication.database.mongo_interface import Database, Mission, Point, serialize_objectid_from_result
import mongomock
import unittest
from unittest.mock import patch

BASE_TIMESTAMP = 4
BASE_FLIGHT_DURATION = 10
BASE_NUMBER_OF_DRONES = 3
BASE_TOTAL_DISTANCE = 40
ID_ADDON = '551137c2f9e1fac808a5f57'


def populate_db(db):
    for x in range(3):
        db.missions.insert_one({
            '_id': mongomock.ObjectId(str(x) + ID_ADDON),
            'time_stamp': str(BASE_TIMESTAMP + x),
            'flight_duration': BASE_FLIGHT_DURATION + x,
            'number_of_drones': BASE_NUMBER_OF_DRONES + x,
            'is_simulated': False,
            'total_distance': BASE_TOTAL_DISTANCE + x,
            'maps': [[{
                'x': 1 + x,
                'y': 2 + x
            }]],
        })


class TestApplication(unittest.TestCase):
    """This class has methods that tests all the methods implemented from the
    Database class present in
    services/communication/database/mongo_interface.py"""

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_initialization(self):
        database = Database()
        self.assertTrue(isinstance(database.client, mongomock.MongoClient))

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_upload_mission_info(self):
        database = Database()
        test = Mission(BASE_FLIGHT_DURATION, BASE_NUMBER_OF_DRONES, False,
                       BASE_TOTAL_DISTANCE, [[{
                           'x': 1,
                           'y': 2
                       }]])
        self.assertTrue(len(list(database.db.missions.find())) == 0)
        database.upload_mission_info(test)
        result = database.db.missions.find_one(
            {'total_distance': BASE_TOTAL_DISTANCE})
        self.assertEqual(test.total_distance, result['total_distance'])

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_get_all_missions_time_stamps(self):
        database = Database()
        populate_db(database.db)
        expected_value = []
        for x in range(3):
            expected_value.append({
                '_id': str(x) + ID_ADDON,
                'time_stamp': str(BASE_TIMESTAMP + x),
                'is_simulated': False,
                'total_distance': BASE_TOTAL_DISTANCE + x,
                'number_of_drones': BASE_NUMBER_OF_DRONES + x,
                'flight_duration': BASE_FLIGHT_DURATION + x
            })
        result = database.get_all_missions_time_stamps()
        self.assertEqual(result, expected_value)

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_get_mission_from_id(self):
        database = Database()
        populate_db(database.db)
        expected_value = {
            '_id': ('2' + ID_ADDON),
            'time_stamp': str(BASE_TIMESTAMP + 2),
            'flight_duration': BASE_FLIGHT_DURATION + 2,
            'number_of_drones': BASE_NUMBER_OF_DRONES + 2,
            'is_simulated': False,
            'total_distance': BASE_TOTAL_DISTANCE + 2,
            'maps': [[{
                'x': 1 + 2,
                'y': 2 + 2
            }]]
        }

        result = database.get_mission_from_id('2' + ID_ADDON)
        self.assertEqual(result, expected_value)

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_remove_mission_from_id(self):
        database = Database()
        populate_db(database.db)
        self.assertTrue(database.remove_mission_from_id('1' + ID_ADDON))
        result = database.get_mission_from_id('1' + ID_ADDON)
        self.assertEqual(result, None)

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_update_mission_info_from_id(self):
        database = Database()
        populate_db(database.db)
        test = Mission(BASE_FLIGHT_DURATION, BASE_NUMBER_OF_DRONES, False,
                       BASE_TOTAL_DISTANCE, [[{
                           'x': 1,
                           'y': 2
                       }]])
        self.assertFalse(
            database.update_mission_info_from_id(test, '5' + ID_ADDON))
        self.assertTrue(
            database.update_mission_info_from_id(test, '2' + ID_ADDON))
        self.assertEqual(
            test.time_stamp,
            database.db.missions.find_one(
                {'_id': mongomock.ObjectId('2' + ID_ADDON)})['time_stamp'])

    def test_mission_serialization(self):
        base_list = [{'_id': mongomock.ObjectId('1' + ID_ADDON)}]
        expected_result = [{'_id': ('1' + ID_ADDON)}]

        self.assertEqual(serialize_objectid_from_result(base_list),
                         expected_result)

    def test_mission_interface(self):
        mission = Mission(0, 1, True, 2, [{'x': 0, 'y': 1}])
        self.assertEqual(mission.flight_duration, 0)
        self.assertEqual(mission.number_of_drones, 1)
        self.assertEqual(mission.total_distance, 2)
        self.assertEqual(mission.is_simulated, True)
        self.assertEqual(mission.time_stamp[0:4],
                         datetime.now().isoformat()[0:4])
        self.assertEqual(mission.logs, [])

    def test_point_interface(self):
        point = Point(0, 1)
        self.assertEqual(point.x, 0)
        self.assertEqual(point.y, 1)

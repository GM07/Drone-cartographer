"""This module tests the functions implemented to communicate with the mongoDb.

It is called when you run pylint from the root. It contains unit test methods
and implements Mongomock
"""
from dataclasses import dataclass
from datetime import datetime
from typing import Any
from unittest import mock

import pytest
from services.communication.database.mongo_interface import Database, Mission, MissionManager, Point, serialize_objectid_from_result
import mongomock
import unittest
from unittest.mock import patch

from services.data.drone_data import Point2D

BASE_TIMESTAMP = 4
BASE_FLIGHT_DURATION = 10
BASE_NUMBER_OF_DRONES = 3
BASE_TOTAL_DISTANCE = 40
ID_ADDON = '551137c2f9e1fac808a5f57'


@dataclass
class MissionStub:
    flight_duration: Any
    logs: Any
    number_of_drones: Any
    total_distance: Any


@dataclass
class DroneDataStub:
    position: Point2D
    name: Any


def populate_db(db):
    for x in range(3):
        db.missions.insert_one({
            '_id': mongomock.ObjectId(str(x) + ID_ADDON),
            'time_stamp': str(BASE_TIMESTAMP + x),
            'flight_duration': BASE_FLIGHT_DURATION + x,
            'number_of_drones': BASE_NUMBER_OF_DRONES + x,
            'is_simulated': False,
            'total_distance': BASE_TOTAL_DISTANCE + x,
            'map': str(x),
            'logs': str(x)
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

    @pytest.fixture(autouse=True)
    def inject_fixtures(self, capfd):
        self._capfd = capfd

    @patch('services.communication.database.mongo_interface.MongoClient',
           mock.Mock(side_effect=KeyError))
    def test_initialization_fail(self):
        database = Database()
        self.assertFalse(hasattr(database, 'db'))
        out = self._capfd.readouterr().out

        self.assertEqual(out[0:len(out) - 1],
                         "Couldn't connect to database please relaunch")

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
    def test_upload_mission_info_fail(self):
        database = Database()
        database.db.missions.insert_one = mock.Mock(side_effect=KeyError)
        test = Mission(BASE_FLIGHT_DURATION, BASE_NUMBER_OF_DRONES, False,
                       BASE_TOTAL_DISTANCE, [[{
                           'x': 1,
                           'y': 2
                       }]])
        database.upload_mission_info(test)
        out = self._capfd.readouterr().out
        self.assertEqual(out[0:len(out) - 1], 'Database error')

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
    def test_get_all_missions_time_stamps_fail(self):
        database = Database()
        populate_db(database.db)
        database.db.missions.aggregate = mock.Mock(side_effect=KeyError)
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
        self.assertNotEqual(result, expected_value)
        out = self._capfd.readouterr().out
        self.assertEqual(out[0:len(out) - 1], 'Database error')

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
            'map': '2',
            'logs': '2'
        }

        result = database.get_mission_from_id('2' + ID_ADDON)
        self.assertEqual(result, expected_value)

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_get_mission_from_id_fail(self):
        database = Database()
        populate_db(database.db)
        database.db.missions.find_one = mock.Mock(side_effect=KeyError)
        expected_value = {
            '_id': ('2' + ID_ADDON),
            'time_stamp': str(BASE_TIMESTAMP + 2),
            'flight_duration': BASE_FLIGHT_DURATION + 2,
            'number_of_drones': BASE_NUMBER_OF_DRONES + 2,
            'is_simulated': False,
            'total_distance': BASE_TOTAL_DISTANCE + 2,
            'map': [[{
                'x': 1 + 2,
                'y': 2 + 2
            }]]
        }

        result = database.get_mission_from_id('2' + ID_ADDON)
        database.db.missions.find_one.assert_called()
        self.assertNotEqual(result, expected_value)
        out = self._capfd.readouterr().out
        self.assertEqual(out[0:len(out) - 1], 'Database error')

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_get_mission_logs_from_id(self):
        database = Database()
        populate_db(database.db)
        expected_value = {'_id': ('2' + ID_ADDON), 'logs': '2'}

        result = database.get_mission_logs_from_id('2' + ID_ADDON)
        self.assertEqual(result, expected_value)

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_get_mission_logs_from_id_fail(self):
        database = Database()
        populate_db(database.db)
        database.db.missions.find_one = mock.Mock(side_effect=KeyError)
        expected_value = {'_id': ('2' + ID_ADDON), 'logs': '2'}

        result = database.get_mission_logs_from_id('2' + ID_ADDON)
        self.assertNotEqual(result, expected_value)
        out = self._capfd.readouterr().out
        self.assertEqual(out[0:len(out) - 1], 'Database error')

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_get_mission_map_from_id(self):
        database = Database()
        populate_db(database.db)
        expected_value = {'_id': ('2' + ID_ADDON), 'map': '2'}

        result = database.get_mission_map_from_id('2' + ID_ADDON)
        self.assertEqual(result, expected_value)

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_get_mission_map_from_id_fail(self):
        database = Database()
        populate_db(database.db)
        database.db.missions.find_one = mock.Mock(side_effect=KeyError)
        expected_value = {'_id': ('2' + ID_ADDON), 'map': '2'}

        result = database.get_mission_map_from_id('2' + ID_ADDON)
        self.assertNotEqual(result, expected_value)
        database.db.missions.find_one.assert_called()
        out = self._capfd.readouterr().out
        self.assertEqual(out[0:len(out) - 1], 'Database error')

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

    @patch('services.communication.database.mongo_interface.Mission.__init__')
    def test_mission_manager_start_current_mission(
            self, mission_mock: mock.MagicMock):
        mission_manager = MissionManager()
        mission_mock.return_value = None
        mission_manager.start_current_mission(
            [DroneDataStub(Point2D(0, 0), 'test')], True)
        self.assertEqual(1, len(mission_manager.last_known_positions))
        self.assertTrue(mission_manager.is_mission_started)
        self.assertFalse(mission_manager.lock.locked())

    @patch('services.communication.database.mongo_interface.Database.__init__',
           mock.MagicMock(return_value=None))
    @patch(
        'services.communication.database.mongo_interface.Database.upload_mission_info'
    )
    @patch('services.communication.database.mongo_interface.Map.raw_data')
    @patch('services.communication.database.mongo_interface.Map.filtered_data')
    def test_mission_manager_end_current_mission(
            self, filtered_clear_mock: mock.MagicMock,
            raw_data_clear: mock.MagicMock, upload_mock: mock.MagicMock):

        mission_manager = MissionManager()

        mission = MissionStub(0, '', 1, 0)
        mission_manager.current_mission = mission
        mission_manager.is_mission_started = True

        mission_manager.mission_start_time = 0
        mission_manager.end_current_mission(['', ''])
        self.assertFalse(mission_manager.lock.locked())
        filtered_clear_mock.clear.assert_called_once()
        raw_data_clear.clear.assert_called_once()
        upload_mock.assert_called_once_with(mission)

    def test_mission_manager_update_position_successful(self):
        mission_manager = MissionManager()
        TEST_DISTANCE = 10
        mission_manager.current_mission = MissionStub(0, '', 0, TEST_DISTANCE)
        mission_manager.is_mission_started = True
        mission_manager.last_known_positions = {'test': Point2D(0, 0)}
        point = Point2D(2, 0)
        mission_manager.update_position(DroneDataStub(point, 'test'))
        self.assertEqual(mission_manager.current_mission.total_distance,
                         TEST_DISTANCE + 2)
        self.assertEqual(mission_manager.last_known_positions['test'], point)
        self.assertFalse(mission_manager.lock.locked())

    def test_mission_manager_update_position_unsuccessful(self):
        mission_manager = MissionManager()
        TEST_DISTANCE = 10
        mission_manager.current_mission = MissionStub(0, '', 0, TEST_DISTANCE)
        mission_manager.is_mission_started = False
        mission_manager.last_known_positions = []
        mission_manager.last_known_positions = [Point2D(0, 0)]
        point = Point2D(2, 0)
        mission_manager.update_position(DroneDataStub(point, 'test'))
        self.assertNotEqual(mission_manager.current_mission.total_distance,
                            TEST_DISTANCE + 2)
        self.assertNotEqual(mission_manager.last_known_positions[0], point)
        self.assertFalse(mission_manager.lock.locked())